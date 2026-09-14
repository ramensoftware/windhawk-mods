// ==WindhawkMod==
// @id              explorer-fast-navigation
// @name            Explorer Fast Navigation
// @description     Improves explorer navigation latency
// @version         0.9.4
// @author          Vivy
// @github          https://github.com/enginelesscc
// @twitter         https://x.com/VivyVCCS
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod fixes the navigation flicker introduced in Windows 8 and reduces the latency introduced by Ribbon and Xaml islands.
Mostly done by delaying heavy work and redraws until after navigation finish.
Navigation now feels like it used to in Windows 7 and earlier.
This mod does not remove ribbon to avoid breaking the command bar.

Tested on Windows 11 X64/ARM64 as well as Windows Server 2022 x64
Conceptually this should work down to Windows 8 but may require changes to this code.

Disclosure: This mod was mainly created by GPT6-Astra, however the discovery was made much earlier: https://x.com/VivyVCCS/status/1698420723344187879
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- navigationPaint: true
  $name: Defer immediate navigation paints
  $description: Defer immediate activation and layout paints while Explorer switches views. Native loading and redraw protection remain active.
- deferralDeadlineMs: 200
  $name: UI deferral deadline (ms)
  $description: Maximum delay for enabled ribbon, command bar and navigation bar updates. Updates can run sooner when the folder is ready. Range 0–5000. Set to 0 to disable these deferrals.
- classicRibbon: true
  $name: Defer classic ribbon updates
  $description: Delay classic ribbon refreshes during navigation.
- xamlCommandBar: true
  $name: Defer XAML command bar updates
  $description: Delay modern command bar refreshes during navigation. Navigation bar updates have a separate switch.
- navigationBar: true
  $name: Defer navigation bar updates
  $description: Delay navigation state updates in both the classic and modern navigation bars.
- duserBatching: true
  $name: Accelerate DUser batching
  $description: Shorten folder item batching intervals and time slices to at most 5 ms. Turn off to use the native timing.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <oaidl.h>
#include <shdispid.h>
#include <shlobj.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <new>
#include <vector>

// Shared types.

struct Settings {
    UINT deferralDeadlineMs = 200;
    bool navigationPaint = true;
    bool classicRibbon = true;
    bool xamlCommandBar = true;
    bool navigationBar = true;
    bool duserBatching = true;
};

// Admit window work and stop it under the same lock. Never hold this lock
// across window/COM calls: they can reenter the mod on an owning UI thread.
class WindowLifetime {
    std::mutex mutex;
    std::condition_variable changed;
    bool stopping = false;
    size_t operations = 0;
    size_t callbacks = 0;
    std::vector<HWND> windows;

public:
    class Operation {
        WindowLifetime& owner;
        bool admitted;

    public:
        explicit Operation(WindowLifetime& owner) : owner(owner) {
            std::lock_guard lock(owner.mutex);
            admitted = !owner.stopping;
            if (admitted) {
                ++owner.operations;
            }
        }

        Operation(const Operation&) = delete;

        ~Operation() {
            if (admitted) {
                std::lock_guard lock(owner.mutex);
                --owner.operations;
                owner.changed.notify_all();
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
            std::lock_guard lock(owner.mutex);
            ++owner.callbacks;
        }

        Callback(const Callback&) = delete;

        ~Callback() {
            std::lock_guard lock(owner.mutex);
            --owner.callbacks;
            owner.changed.notify_all();
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
        stopping = true;
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
        // A nested WM_CLOSE during Flush only requests closure. The outer
        // drain destroys the window after releasing its outstanding COM refs.
        std::unique_lock lock(mutex);
        while (!changed.wait_for(lock, std::chrono::seconds(2), [this] { return windows.empty() && callbacks == 0; })) {
            Wh_Log(L"Still waiting for %zu queue window(s) and %zu callback(s)", windows.size(), callbacks);
        }
    }
};

using Callback = void (*)(void*);

struct Action { // GMA_ACTION ABI; checked before reading callback/parameters.
    DWORD size;
    float startDelay;
    float duration;
    float repeatDelay;
    int repeatCount;
    int timeSlice;
    Callback callback;
    void* context;
};

static_assert(sizeof(Action) == 40 && offsetof(Action, callback) == 24);

using CreateAction = void* (*)(const Action*);
using DeleteAction = BOOL(WINAPI*)(void*);

using RedrawFrame = void(__fastcall*)(void*);
using RenderSizer = HRESULT(__fastcall*)(void*);

struct PaintScope {
    unsigned& depth;

    explicit PaintScope(unsigned& depth) : depth(depth) {
        ++depth;
    }

    PaintScope(const PaintScope&) = delete;

    ~PaintScope() {
        --depth;
    }
};

// File-wide state and original function pointers.

// Loaded before hooks are installed; settings changes reload the mod so active
// queued references finish teardown before the new policy is used.
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

static HMODULE frame;
static HMODULE shell;
static HMODULE duser;

// Shared helpers.

static void LoadSettings(decltype(&Wh_GetIntSetting) readSetting = Wh_GetIntSetting) {
    settings.deferralDeadlineMs = std::clamp(readSetting(L"deferralDeadlineMs"), 0, 5000);
    settings.navigationPaint = readSetting(L"navigationPaint") != 0;
    settings.classicRibbon = readSetting(L"classicRibbon") != 0;
    settings.xamlCommandBar = readSetting(L"xamlCommandBar") != 0;
    settings.navigationBar = readSetting(L"navigationBar") != 0;
    settings.duserBatching = readSetting(L"duserBatching") != 0;
}

template <class T>
static bool Hook(T& original, T replacement) {
    return original && Wh_SetFunctionHook(reinterpret_cast<void*>(original),
                                          reinterpret_cast<void*>(replacement),
                                          reinterpret_cast<void**>(&original));
}

struct SymbolHook {
    const wchar_t* name;
    void** original;
    void* replacement;

    template <class T>
    SymbolHook(const wchar_t* name, T** original,
               std::type_identity_t<T*> replacement = nullptr)
        : name(name), original(reinterpret_cast<void**>(original)),
          replacement(reinterpret_cast<void*>(replacement)) {}
};

static void HookAvailableSymbols(
    HMODULE module, const wchar_t* moduleName, const SymbolHook* hooks, size_t count,
    const WH_HOOK_SYMBOLS_OPTIONS* options,
    decltype(&WindhawkUtils::HookSymbols) resolve = WindhawkUtils::HookSymbols,
    decltype(&Wh_SetFunctionHook) install = Wh_SetFunctionHook) {
    // Resolve optional addresses together to retain Windhawk's symbol cache.
    // Install separately so one failed hook cannot prevent later hooks.
    std::vector<WindhawkUtils::SYMBOL_HOOK> symbols;
    for (size_t i = 0; i < count; ++i) {
        *hooks[i].original = nullptr;
        symbols.emplace_back(std::initializer_list<std::wstring_view>{hooks[i].name},
                             hooks[i].original, nullptr, true);
    }
    if (!module) {
        Wh_Log(L"Symbol module unavailable: %s", moduleName);
    } else if (!resolve(module, symbols.data(), symbols.size(), options)) {
        Wh_Log(L"Symbol lookup incomplete: %s; using available symbols", moduleName);
    }
    for (size_t i = 0; i < count; ++i) {
        const auto& hook = hooks[i];
        if (!*hook.original) {
            Wh_Log(L"Missing symbol: %s!%s", moduleName, hook.name);
        } else if (hook.replacement &&
                   !install(*hook.original, hook.replacement, hook.original)) {
            Wh_Log(L"Hook installation failed: %s!%s", moduleName, hook.name);
            *hook.original = nullptr;
        }
    }
}

static bool CanDeferInvoke(REFIID iid, WORD flags, const DISPPARAMS* args) {
    const GUID nullIid{};
    return IsEqualGUID(iid, nullIid) && flags == DISPATCH_METHOD && args && args->cArgs == 0 &&
           args->cNamedArgs == 0;
}

static HWND FindThreadBrowserWindow() {
    HWND browser = nullptr;
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hwnd, LPARAM param) -> BOOL {
            // A browser thread also owns helper windows and dialogs.
            wchar_t name[64];
            if (!GetClassNameW(hwnd, name, ARRAYSIZE(name)) ||
                wcscmp(name, L"CabinetWClass") != 0) {
                return TRUE;
            }
            auto& found = *reinterpret_cast<HWND*>(param);
            if (found) {
                // Do not choose an arbitrary browser if a thread owns several.
                found = nullptr;
                return FALSE;
            }
            found = hwnd;
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&browser));
    return browser;
}

// Cross-namespace notifications used by FileTransition.
namespace RibbonWork {
static bool DeferInvoke(void*, DISPID, REFIID, LCID, WORD, DISPPARAMS*);
static void NavigationStarted();
static void ViewReady();
} // namespace RibbonWork

// Track view completion for deferred UI work without taking over file painting.
namespace FileTransition {
struct State {
    void* itemsView{};
    void* batchAction{};
    ULONG generation = 0;
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
    return settings.navigationPaint && navigationDepth;
}

static bool IsReady() {
    // A temporary gap between enumeration batches isn't completion.
    return completionTracking && active && active->itemsView && active->enumerated &&
           !active->pending && !navigationDepth && !processingBatch;
}

static void NotifyReady() {
    if (IsReady()) {
        // The ribbon timer runs at low priority, after pending native painting.
        RibbonWork::ViewReady();
    }
}

static void EnumerationDone() {
    if (active && active->itemsView) {
        active->enumerated = true;
        NotifyReady();
    }
}

static void Begin(void* itemsView) {
    state = {itemsView, nullptr, state.generation + 1};
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
    HRESULT result;
    {
        // This can run after _NavigateToPidl returns. It updates the frame,
        // activates the new view, and updates the ribbon.
        PaintScope scope(navigationDepth);
        result = connectionOriginal(self, item, connection, flags, extra, first, second, id);
    }
    NotifyReady();
    return result;
}

static HRESULT __fastcall ResetHook(void* self, void* item, void* collection) {
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
        active->pending = true;
    }
    return result;
}

static bool __fastcall DeleteHook(void* self) {
    ULONG generation = state.generation;
    bool result = deleteOriginal(self);
    if (result && processingBatch == self && active && active->itemsView == self &&
        active->generation == generation) {
        active->pending = false;
    }
    return result;
}

static void __fastcall BatchHook(void* self, void* info) {
    void* previous = processingBatch;
    processingBatch = self;
    batchOriginal(self, info);
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
// OnShellViewChanged still connects the new view immediately. Only defer
// OnBrowserNavigated after that connection succeeded; first-time setup is native.
namespace RibbonWork {
// Types.

struct Task {
    IUnknown* object;
    std::function<void()> run;
};

struct Queue {
    HWND hwnd{};
    HWND root{};
    HHOOK inputHook{};
    std::deque<Task> tasks;
    bool draining = false;
    bool closing = false;
};

using Notification = HRESULT(__fastcall*)(void*);
using Destroy = HRESULT(__fastcall*)(void*, BOOL);
using NavState = HRESULT(__fastcall*)(void*, ULONG);

// Constants.

static constexpr wchar_t ClassName[] = L"Windhawk.ExplorerFastNavigation.RibbonWork";

// State and original function pointers.

static thread_local Queue* queue;
static thread_local void* connected;
static thread_local ULONGLONG started;
static thread_local bool ready;
static WindowLifetime lifetime;
static HINSTANCE instance;
static bool registered;

static Notification navigatedOriginal;
static Notification changedOriginal;
static Destroy destroyOriginal;
static NavState navStateOriginal;

// Queue processing.

static void Flush() {
    WindowLifetime::Callback callback(lifetime);
    Queue* q = queue;
    if (!q || q->draining) {
        return;
    }
    q->draining = true;
    KillTimer(q->hwnd, 1);
    while (!q->tasks.empty()) {
        Task task = std::move(q->tasks.front());
        q->tasks.pop_front();
        if (!q->closing) {
            task.run();
        }
        task.object->Release();
    }
    q->draining = false;
    if (q->closing) {
        DestroyWindow(q->hwnd);
    }
}

// Queue window callbacks.

static LRESULT CALLBACK InputHook(int code, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (code >= 0 && wp == PM_REMOVE && queue && !queue->tasks.empty()) {
        const auto msg = reinterpret_cast<const MSG*>(lp);
        // Refresh before accepting input so deferred ribbon/address commands
        // never execute against their previous displayed state.
        if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN ||
            msg->message == WM_LBUTTONDOWN || msg->message == WM_RBUTTONDOWN ||
            msg->message == WM_MBUTTONDOWN || msg->message == WM_NCLBUTTONDOWN) {
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
        Flush(); // Drop pending presentation work; release COM refs on this thread.
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (msg == WM_TIMER && wp == 1) {
        // WM_TIMER has lower priority than paint. A bounded fallback also handles
        // empty, slow and failed navigations with no batching callback.
        if ((ready && !FileTransition::navigationDepth && !FileTransition::processingBatch) ||
            GetTickCount64() - started >= settings.deferralDeadlineMs) {
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
        q->closing = true;
        // Flush owns q while draining. CloseAndWait waits for its destruction.
        Flush();
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        Queue* q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        if (q && q->inputHook) {
            UnhookWindowsHookEx(q->inputHook);
        }
        if (q && q->root) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(q->root, OwnerSubclass);
        }
        if (queue == q) {
            queue = nullptr;
        }
        delete q;
        lifetime.Remove(hwnd);
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static bool Enqueue(IUnknown* object, std::function<void()> run) {
    // Verified in the saved ExplorerFrame and Windows.UI.FileExplorer vtables:
    // Invoke uses IDispatch; OnBrowserNavigated/OnCommandStateInvalidated use
    // IExplorerRibbon; SetNavigationState uses IShellNavigationBand; and
    // InvalidateCommands uses IExplorerCommandHost. Each has QueryInterface,
    // AddRef and Release in slots 0..2, with thunks adjusting the interface's
    // `this` to the complete object. Keep the incoming interface pointer intact.
    WindowLifetime::Operation operation(lifetime);
    if (!operation || (queue && queue->draining)) {
        return false;
    }
    if (!queue) {
        HWND root = FindThreadBrowserWindow();
        if (!root) {
            return false;
        }
        auto q = new (std::nothrow) Queue;
        if (!q) {
            return false;
        }
        q->hwnd = CreateWindowExW(0, ClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, instance,
                                  nullptr);
        if (!q->hwnd) {
            delete q;
            return false;
        }
        SetWindowLongPtrW(q->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(q));
        q->root = root;
        queue = q;
        lifetime.Add(q->hwnd);
        q->inputHook = SetWindowsHookExW(WH_GETMESSAGE, InputHook, instance, GetCurrentThreadId());
        if (!q->inputHook || !WindhawkUtils::SetWindowSubclassFromAnyThread(
                                 root, OwnerSubclass, reinterpret_cast<DWORD_PTR>(q))) {
            DestroyWindow(q->hwnd);
            return false;
        }
    }
    if (queue->closing || queue->tasks.size() >= 64) {
        Flush();
        return false;
    }
    if (!SetTimer(queue->hwnd, 1, 10, nullptr)) {
        Flush();
        return false;
    }
    queue->tasks.push_back({object, std::move(run)});
    object->AddRef();
    return true;
}

// Navigation state and deferred notifications.

static void NavigationStarted() {
    Flush();
    started = GetTickCount64();
    ready = false;
}

static void ViewReady() {
    ready = true;
}

static bool Pending() {
    return started && !ready && GetTickCount64() - started < settings.deferralDeadlineMs;
}

static bool DeferInvoke(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                        DISPPARAMS* args) {
    // The inspected branches return S_OK without consuming arguments. Check the
    // actual call too; argument-bearing events must retain their native inputs.
    if (!destroyOriginal || !settings.classicRibbon || !CanDeferInvoke(iid, flags, args) || !Pending() ||
        (id != 200 && id != 201 && id != 205 && id != 207 && id != 212 && id != 215 && id != 220)) {
        return false;
    }
    return Enqueue(reinterpret_cast<IUnknown*>(self), [self, id, iid, locale, flags] {
        DISPPARAMS empty{};
        FileTransition::invokeOriginal(self, id, iid, locale, flags, &empty, nullptr, nullptr,
                                       nullptr);
    });
}

static HRESULT __fastcall NavigatedHook(void* self) {
    if (destroyOriginal && settings.classicRibbon && self == connected && Pending() &&
        Enqueue(reinterpret_cast<IUnknown*>(self), [self] {
            navigatedOriginal(self);
        })) {
        return S_OK;
    }
    return navigatedOriginal(self);
}

static HRESULT __fastcall ChangedHook(void* self) {
    // NavigationStarted already drained the previous destination. Keep this
    // destination's queued UI updates behind the new file view.
    if (!Pending()) {
        Flush();
    }
    HRESULT hr = changedOriginal(self);
    connected = SUCCEEDED(hr) ? self : nullptr;
    return hr;
}

static HRESULT __fastcall DestroyHook(void* self, BOOL final) {
    if (queue && queue->draining) {
        queue->closing = true;
    }
    Flush();
    if (connected == self) {
        connected = nullptr;
    }
    return destroyOriginal(self, final);
}

static HRESULT __fastcall NavStateHook(void* self, ULONG flags) {
    if (settings.navigationBar && Pending() &&
        Enqueue(reinterpret_cast<IUnknown*>(self), [self, flags] {
            navStateOriginal(self, flags);
        })) {
        return S_OK;
    }
    return navStateOriginal(self, flags);
}

// Window class lifecycle.

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
    if (registered && UnregisterClassW(ClassName, instance)) {
        registered = false;
    }
}

} // namespace RibbonWork

// The modern command surface has its own event sink and navigation-band
// implementation. Delaying CExplorerRibbon alone never reaches this path.
namespace XamlWork {
// Types.
using Notification = HRESULT(__fastcall*)(void*);
using StateChange = HRESULT(__fastcall*)(void*, ULONG);
using Invoke = FileTransition::Invoke;

// State and original function pointers.

static HMODULE module;
static bool enabled;
static thread_local void* connected;
static Notification navigatedOriginal;
static Notification changedOriginal;
static Notification invalidatedOriginal;
static Notification cleanupOriginal;
static StateChange stateOriginal;
static StateChange invalidateOriginal;
static Invoke invokeOriginal;

static bool Pending() {
    return enabled && RibbonWork::Pending();
}

static HRESULT Run(Notification original, void* self, const wchar_t* label) {
    HRESULT hr = original(self);
    if (FAILED(hr)) {
        Wh_Log(L"Deferred XAML refresh failed: %s, 0x%08X", label, hr);
    }
    return hr;
}
// Modern command-surface hooks.

static HRESULT __fastcall ChangedHook(void* self) {
    // ConnectToView must unsubscribe the old view and subscribe the new view now.
    HRESULT hr = changedOriginal(self);
    connected = SUCCEEDED(hr) ? self : nullptr;
    return hr;
}

static HRESULT __fastcall NavigatedHook(void* self) {
    if (settings.xamlCommandBar && Pending() && self == connected &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self] {
            Run(navigatedOriginal, self, L"EFN xaml_navigated_run");
        })) {
        return S_OK;
    }
    return navigatedOriginal(self);
}

static HRESULT __fastcall InvalidatedHook(void* self) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self] {
            Run(invalidatedOriginal, self, L"EFN xaml_invalidated_run");
        })) {
        return S_OK;
    }
    return invalidatedOriginal(self);
}

static HRESULT __fastcall StateHook(void* self, ULONG flags) {
    // This adapter uses bit 4 to refresh location and commands; other bits are
    // native no-ops. Preserve each notification's flags and FIFO ordering.
    if (settings.navigationBar && (flags & 4) && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self, flags] {
            HRESULT hr = stateOriginal(self, flags);
            if (FAILED(hr)) {
                Wh_Log(L"Deferred XAML location failed: 0x%08X", hr);
            }
        })) {
        return S_OK;
    }
    return stateOriginal(self, flags);
}

static HRESULT __fastcall InvalidateHook(void* self, ULONG commands) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self, commands] {
            invalidateOriginal(self, commands);
        })) {
        return S_OK;
    }
    return invalidateOriginal(self, commands);
}

static HRESULT __fastcall InvokeHook(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                                     DISPPARAMS* args, VARIANT* result, EXCEPINFO* exception,
                                     UINT* argError) {
    // Inspected argument-free invalidations. DISPID 213 consumes arguments and
    // handles Ctrl+wheel; it must always execute synchronously with native args.
    bool invalidation = id == 200 || id == 201 || id == 205 || id == 207 || id == 211 ||
                        id == 212 || id == 215 || id == 220;
    HRESULT hr;
    if (settings.xamlCommandBar && invalidation && CanDeferInvoke(iid, flags, args) && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self, id, iid, locale, flags] {
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
    // Cleanup disconnects the view and site even if queued COM refs keep the
    // allocation alive. Never replay presentation work after that disconnect.
    if (RibbonWork::queue && RibbonWork::queue->draining) {
        RibbonWork::queue->closing = true;
    }
    RibbonWork::Flush();
    connected = nullptr;
    return cleanupOriginal(self);
}

// Adapter initialization and cleanup.

static void Initialize(const WH_HOOK_SYMBOLS_OPTIONS* options) {
    module = LoadLibraryExW(L"Windows.UI.FileExplorer.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    // Windows.UI.FileExplorer.dll
    const SymbolHook hooks[] = {
        {L"public: virtual long __cdecl CommandBarViewAdapter::OnBrowserNavigated(void)",
         &navigatedOriginal,
         NavigatedHook},
        {L"public: virtual long __cdecl CommandBarViewAdapter::OnShellViewChanged(void)",
         &changedOriginal,
         ChangedHook},
        {L"public: virtual long __cdecl CommandBarViewAdapter::OnCommandStateInvalidated(void)",
         &invalidatedOriginal,
         InvalidatedHook},
        {L"public: virtual long __cdecl CommandBarViewAdapter::SetNavigationState(unsigned long)",
         &stateOriginal,
         StateHook},
        {L"public: virtual long __cdecl CommandBarViewAdapter::InvalidateCommands(enum COMMAND_SET)",
         &invalidateOriginal,
         InvalidateHook},
        {L"public: virtual long __cdecl CommandBarViewAdapter::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *)",
         &invokeOriginal,
         InvokeHook},
        {L"public: long __cdecl CommandBarViewAdapter::Cleanup(void)",
         &cleanupOriginal,
         CleanupHook},
    };
    HookAvailableSymbols(module, L"Windows.UI.FileExplorer.dll", hooks, ARRAYSIZE(hooks), options);
    // Cleanup must flush queued work before the adapter disconnects. Individual
    // refresh hooks are independent; NavigatedHook also needs ChangedHook.
    enabled = cleanupOriginal && ((navigatedOriginal && changedOriginal) || invalidatedOriginal ||
                                  stateOriginal || invalidateOriginal || invokeOriginal);
    Wh_Log(L"Modern XAML command/location deferral=%d", enabled);
}

static void Uninit() {
    if (module) {
        FreeLibrary(module);
    }
    module = nullptr;
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
    bool defer = FileTransition::DeferNavigationPaint() &&
                 !(flags & SWP_NOREDRAW) &&
                 GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId() &&
                 (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD);
    BOOL result =
        setWindowPosOriginal(hwnd, after, x, y, cx, cy, defer ? flags | SWP_NOREDRAW : flags);
    if (defer && result) {
        // Geometry and notifications run normally. Queue the complete affected
        // parent region because moving a child also uncovers its old rectangle.
        HWND parent = GetParent(hwnd);
        RedrawWindow(parent ? parent : hwnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
    return result;
}

static BOOL WINAPI UpdateWindowHook(HWND hwnd) {
    // Scope survives intervening API hooks installed by other mods. Suppress
    // only while this thread is navigating inside the relevant shell32 host.
    return redrawFrameDepth && FileTransition::DeferNavigationPaint()
               ? TRUE
               : updateWindowOriginal(hwnd);
}

static BOOL WINAPI RedrawWindowHook(HWND hwnd, const RECT* rect, HRGN region, UINT flags) {
    if (renderSizerDepth && FileTransition::DeferNavigationPaint()) {
        // _Render flushes every child during navigation layout initialization.
        // Leave invalidation and geometry alone; remove only the immediate flush.
        flags &= ~(RDW_UPDATENOW | RDW_ERASENOW);
    }
    return redrawWindowOriginal(hwnd, rect, region, flags);
}

static void* CreateActionHook(const Action* action) {
    bool batch = action && action->size == sizeof(Action) && batchCallback &&
                 action->callback == batchCallback;
    auto state = FileTransition::active;
    bool observe = trackBatchActions && batch && state && state->itemsView == action->context;
    ULONG generation = observe ? state->generation : 0;
    Action faster;
    if (settings.duserBatching && batch && std::isfinite(action->duration) && action->duration >= 0) {
        faster = *action;
        if (faster.duration > 0.005f) {
            faster.duration = 0.005f;
        }
        if (faster.timeSlice == 0 || faster.timeSlice > 5) {
            faster.timeSlice = 5;
        }
        action = &faster;
    }
    void* handle = createActionOriginal(action);
    if (observe && handle && FileTransition::active == state && state->generation == generation) {
        state->batchAction = handle;
        state->pending = true;
    }
    return handle;
}

static BOOL WINAPI DeleteActionHook(void* handle) {
    // Server 2022 inlines DeleteBatchTimer (and some _EnsureBatching calls).
    // Match the exact action captured at creation, not arbitrary DUser deletes
    // reached through nested layout or another view's timer.
    auto state = FileTransition::active;
    bool observe = state && handle && state->batchAction == handle &&
                   FileTransition::processingBatch == state->itemsView;
    ULONG generation = observe ? state->generation : 0;
    BOOL result = deleteActionOriginal(handle);
    if (observe && result && FileTransition::active == state && state->generation == generation &&
        state->batchAction == handle) {
        state->batchAction = nullptr;
        state->pending = false;
        // BatchHook reports readiness only after the native callback unwinds.
    }
    return result;
}

// Module initialization and cleanup.

static void ReleaseModules() {
    XamlWork::Uninit();
    if (duser) {
        FreeLibrary(duser);
    }
    if (shell) {
        FreeLibrary(shell);
    }
    if (frame) {
        FreeLibrary(frame);
    }
    duser = shell = frame = nullptr;
}

static BOOL InitializeMod() {
    LoadSettings();

    frame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    shell = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (settings.duserBatching || settings.deferralDeadlineMs) {
        duser = LoadLibraryExW(L"DUser.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    // ExplorerFrame.dll
    const SymbolHook frameSymbols[] = {
        {L"private: static void __cdecl UIItemsView::s_BatchTimerCallback(struct GMA_ACTIONINFO *)",
         &batchCallback,
         nullptr},
        {L"private: long __cdecl CShellBrowser::_NavigateToPidl(struct _ITEMIDLIST_ABSOLUTE const *,unsigned long,unsigned long)",
         &FileTransition::navigateOriginal,
         FileTransition::NavigateHook},
        {L"private: long __cdecl CShellBrowser::_OnConnectionCreated(struct IShellItem *,struct IUnknown *,unsigned long,unsigned long,struct IUnknown *,struct IUnknown *,struct _GUID const &)",
         &FileTransition::connectionOriginal,
         FileTransition::ConnectionHook},
        {L"private: long __cdecl UIItemsView::_ResetRoot(struct IItem *,struct IItemCollection *)",
         &FileTransition::resetOriginal,
         FileTransition::ResetHook},
        {L"private: long __cdecl UIItemsView::_EnsureBatching(void)",
         &FileTransition::ensureOriginal,
         FileTransition::EnsureHook},
        {L"private: void __cdecl UIItemsView::_OnBatchTimer(struct GMA_ACTIONINFO *)",
         &FileTransition::batchOriginal,
         FileTransition::BatchHook},
        {L"private: bool __cdecl UIItemsView::DeleteBatchTimer(void)",
         &FileTransition::deleteOriginal,
         FileTransition::DeleteHook},
        {L"public: virtual long __cdecl CExplorerRibbon::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *)",
         &FileTransition::invokeOriginal,
         FileTransition::InvokeHook},
        {L"public: virtual long __cdecl CExplorerRibbon::OnBrowserNavigated(void)",
         &RibbonWork::navigatedOriginal,
         RibbonWork::NavigatedHook},
        {L"public: virtual long __cdecl CExplorerRibbon::OnShellViewChanged(void)",
         &RibbonWork::changedOriginal,
         RibbonWork::ChangedHook},
        {L"public: virtual long __cdecl CExplorerRibbon::DestroyRibbonUI(int)",
         &RibbonWork::destroyOriginal,
         RibbonWork::DestroyHook},
        {L"public: virtual long __cdecl CNavBar::SetNavigationState(unsigned long)",
         &RibbonWork::navStateOriginal,
         RibbonWork::NavStateHook},
    };

    // shell32.dll
    const SymbolHook shellSymbols[] = {
        {L"private: void __cdecl CDUIViewFrame::_RedrawFrame(void)",
         &redrawFrameOriginal,
         RedrawFrameHook},
        {L"private: long __cdecl CDUISizerElement::_Render(void)",
         &renderSizerOriginal,
         RenderSizerHook},
    };

    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    HookAvailableSymbols(frame, L"ExplorerFrame.dll", frameSymbols, ARRAYSIZE(frameSymbols), &options);
    if (!FileTransition::navigateOriginal && !FileTransition::resetOriginal &&
        !RibbonWork::navigatedOriginal && !RibbonWork::navStateOriginal &&
        !XamlWork::enabled) {
        Wh_Log(L"No usable hooks on this build");
        return FALSE;
    }

    HookAvailableSymbols(shell, L"shell32.dll", shellSymbols, ARRAYSIZE(shellSymbols), &options);
    if (!RibbonWork::destroyOriginal) {
        Wh_Log(L"Classic ribbon deferral disabled: DestroyRibbonUI hook unavailable");
    }

    // Server 2022 has CExplorerRibbon but no CommandBarViewAdapter. Report
    // classic capabilities separately so expected XAML misses aren't mistaken
    // for disabled ribbon deferral.
    bool deferral = FileTransition::navigateOriginal && settings.deferralDeadlineMs;
    Wh_Log(L"Classic ribbon deferral: navigation=%d, events=%d; navigation bar=%d",
            deferral && settings.classicRibbon && RibbonWork::destroyOriginal &&
            RibbonWork::changedOriginal && RibbonWork::navigatedOriginal,
            deferral && settings.classicRibbon && RibbonWork::destroyOriginal &&
            FileTransition::invokeOriginal,
            deferral && settings.navigationBar && RibbonWork::navStateOriginal);
    
    updateWindowOriginal = UpdateWindow;
    redrawWindowOriginal = RedrawWindow;
    setWindowPosOriginal = SetWindowPos;
    bool paint = Hook(updateWindowOriginal, UpdateWindowHook);
    bool layoutPaint = Hook(redrawWindowOriginal, RedrawWindowHook);
    bool positionPaint = Hook(setWindowPosOriginal, SetWindowPosHook);
    if (!paint || !layoutPaint || !positionPaint) {
        Wh_Log(L"Paint hook availability: UpdateWindow=%d, RedrawWindow=%d, SetWindowPos=%d", paint, layoutPaint, positionPaint);
    }

    if (!RibbonWork::Initialize()) {
        Wh_Log(L"Required deferred UI hooks unavailable");
        return FALSE;
    }

    XamlWork::Initialize(&options);
    bool batches = false;
    if (batchCallback && duser) {
        createActionOriginal = reinterpret_cast<CreateAction>(GetProcAddress(duser, "CreateAction"));
        batches = Hook(createActionOriginal, CreateActionHook);
        if (!batches) {
            Wh_Log(L"DUser.dll!CreateAction unavailable or hook installation failed");
        }
    } else if (settings.duserBatching && !duser) {
        Wh_Log(L"Batching module unavailable: DUser.dll");
    }

    bool nativeBatchTracking = FileTransition::ensureOriginal && FileTransition::deleteOriginal;
    if (!nativeBatchTracking && batches && settings.deferralDeadlineMs) {
        deleteActionOriginal = reinterpret_cast<DeleteAction>(GetProcAddress(duser, "DeleteHandle"));
        trackBatchActions = Hook(deleteActionOriginal, DeleteActionHook);
        if (!trackBatchActions) {
            Wh_Log(L"DUser.dll!DeleteHandle unavailable or hook installation failed");
        }
    }

    FileTransition::completionTracking = FileTransition::batchOriginal && (nativeBatchTracking || trackBatchActions);

    Wh_Log(L"UI completion tracking: native=%d, DUser=%d; deadline is a maximum", 
            nativeBatchTracking && FileTransition::batchOriginal, trackBatchActions);

    if (!FileTransition::completionTracking) {
        Wh_Log(L"Early UI completion unavailable; using deferral deadline fallback");
    }

    Wh_Log(L"Ribbon preserved. Hooks: batching=%d, activation paint=%d, layout paint=%d",
           batches && settings.duserBatching, paint, layoutPaint);

    return TRUE;
}

// Windhawk lifecycle entry points (must remain externally visible).

BOOL Wh_ModInit() {
    if (InitializeMod()) {
        return TRUE;
    }
    // Windhawk rolls back hooks on failure, but doesn't call Wh_ModUninit.
    RibbonWork::Uninit();
    ReleaseModules();
    return FALSE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}

void Wh_ModBeforeUninit() {
    RibbonWork::lifetime.Stop();
    RibbonWork::BeforeUninit();
}

void Wh_ModUninit() {
    RibbonWork::Uninit();
    ReleaseModules();
}
