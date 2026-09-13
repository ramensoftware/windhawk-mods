// ==WindhawkMod==
// @id              explorer-fast-navigation
// @name            Explorer Fast Navigation
// @description     Improves explorer navigation latency
// @version         0.9.1
// @author          Vivy
// @github          https://github.com/enginelesscc
// @twitter         https://x.com/VivyVCCS
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lcomctl32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod fixes the navigation flicker introduced in windows 8 and reduces the latency introduced by ribbon and xaml islands.
Mostly done by delaying heavy work and redraws until after navigation finish.
Navigation now feels like it used to in Windows 7 and earlier.
This mod does not remove ribbon to avoid breaking the command bar.

Only tested on Windows 11 x64, but in theory should also work on earlier versions.

Disclosure: This mod was mainly created by GPT6-Astra, however the discovery was made much earlier: https://x.com/VivyVCCS/status/1698420723344187879
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- coverDeadlineMs: 200
  $name: Folder cover deadline (ms)
  $description: Maximum time to keep the previous folder visible while the new folder loads. Range 0–5000. Set to 0 to disable the cover and navigation paint suppression.
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

#ifdef EFN_DIAGNOSTICS
#include <evntprov.h>
#endif

// Shared types.

struct Settings {
    UINT coverDeadlineMs = 200;
    UINT deferralDeadlineMs = 200;
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
            SendMessageW(hwnd, WM_CLOSE, 0, 0);
        }
        // A nested WM_CLOSE during Flush only requests closure. The outer
        // drain destroys the window after releasing its outstanding COM refs.
        std::unique_lock lock(mutex);
        changed.wait(lock, [this] {
            return windows.empty() && callbacks == 0;
        });
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
// covers and queued references finish teardown before the new policy is used.
static Settings settings;

static Callback batchCallback;
static RedrawFrame redrawFrameOriginal;
static RenderSizer renderSizerOriginal;
static thread_local unsigned redrawFrameDepth;
static thread_local unsigned renderSizerDepth;

static CreateAction createActionOriginal;
static decltype(&UpdateWindow) updateWindowOriginal;
static decltype(&RedrawWindow) redrawWindowOriginal;
static decltype(&SetWindowPos) setWindowPosOriginal;

static HMODULE frame;
static HMODULE shell;
static HMODULE duser;

#ifdef EFN_DIAGNOSTICS
static REGHANDLE diagnosticProvider;
#endif

// Shared helpers.

static void LoadSettings(decltype(&Wh_GetIntSetting) readSetting = Wh_GetIntSetting) {
    settings.coverDeadlineMs = std::clamp(readSetting(L"coverDeadlineMs"), 0, 5000);
    settings.deferralDeadlineMs = std::clamp(readSetting(L"deferralDeadlineMs"), 0, 5000);
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

#ifdef EFN_DIAGNOSTICS
static void Trace(const wchar_t* message) {
    EventWriteString(diagnosticProvider, 4, 1, message);
}
#else
static void Trace(const wchar_t*) {}
#endif

namespace NavigationPolicy {

static bool IsRemotePath(const wchar_t* path, decltype(&GetDriveTypeW) driveType = GetDriveTypeW) {
    if (!path) {
        return false;
    }
    if (_wcsnicmp(path, L"\\\\?\\UNC\\", 8) == 0) {
        return true;
    }
    if (wcsncmp(path, L"\\\\?\\", 4) == 0) {
        path += 4;
    } else if (wcsncmp(path, L"\\\\", 2) == 0) {
        return true;
    }
    if (path[0] && path[1] == L':' && path[2] == L'\\') {
        wchar_t root[] = {path[0], L':', L'\\', 0};
        return driveType(root) == DRIVE_REMOTE;
    }
    return false;
}

static UINT RemainingCoverMs(bool remote, ULONGLONG started, ULONGLONG now) {
    ULONGLONG elapsed = now - started;
    return remote || elapsed >= settings.coverDeadlineMs
               ? 0
               : static_cast<UINT>(settings.coverDeadlineMs - elapsed);
}

} // namespace NavigationPolicy

// Cross-namespace notifications used by FileTransition.
namespace RibbonWork {
static bool DeferInvoke(void*, DISPID, REFIID, LCID, WORD, DISPPARAMS*);
static void NavigationStarted();
static void ViewReady();
} // namespace RibbonWork

namespace FileTransition {
// Types.

struct State {
    HWND overlay{};
    HWND root{};
    HDC dc{};
    HBITMAP bitmap{};
    HGDIOBJ oldBitmap{};
    void* itemsView{};
    ULONG generation = 1;
    ULONGLONG started = 0;
    bool pending = false;
    bool enumerated = false;
    bool finishing = false;
};

struct Navigation {
    ULONGLONG started = 0;
    bool remote = false;
    bool awaitingReset = false;
};

using ResetRoot = HRESULT(__fastcall*)(void*, void*, void*);
using Navigate = HRESULT(__fastcall*)(void*, PCIDLIST_ABSOLUTE, ULONG, ULONG);
using BlockRedraw = void(__fastcall*)(void*, float);
using EnsureBatching = HRESULT(__fastcall*)(void*);
using BatchTimer = void(__fastcall*)(void*, void*);
using DeleteBatchTimer = bool(__fastcall*)(void*);
using Invoke = HRESULT(__fastcall*)(void*, DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*,
                                    EXCEPINFO*, UINT*);

// Constants.

static constexpr wchar_t WindowClass[] = L"Windhawk.ExplorerFastNavigation.FileTransition";
static constexpr UINT PresentMessage = WM_APP + 71;

// State and original function pointers.

static WindowLifetime lifetime;
static HINSTANCE instance;
static bool registered;

static thread_local State* active;
static thread_local void* processingBatch;

static thread_local Navigation navigation;
static thread_local unsigned navigationDepth;

static ResetRoot resetOriginal;
static Navigate navigateOriginal;
static BlockRedraw blockOriginal;
static EnsureBatching ensureOriginal;
static BatchTimer batchOriginal;
static DeleteBatchTimer deleteOriginal;
static Invoke invokeOriginal;

// File-view presentation.

static bool NativeLoadingAllowed() {
    return !settings.coverDeadlineMs || navigation.remote ||
           (navigation.started &&
            !NavigationPolicy::RemainingCoverMs(false, navigation.started, GetTickCount64()));
}

static HWND FindFileView(HWND root) {
    HWND result = nullptr;
    EnumChildWindows(
        root,
        [](HWND hwnd, LPARAM param) -> BOOL {
            wchar_t cls[64];
            if (IsWindowVisible(hwnd) && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
                wcscmp(cls, L"SHELLDLL_DefView") == 0) {
                *reinterpret_cast<HWND*>(param) = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

static bool CanPresent(const State* state) {
    // An empty batch queue can be only a gap between enumeration batches.
    return state && state->enumerated && !state->pending && !navigationDepth &&
           processingBatch != state->itemsView;
}

static void QueuePresent(State* state) {
    if (CanPresent(state)) {
        PostMessageW(state->overlay, PresentMessage, state->generation, 0);
    }
}

static void EnumerationDone() {
    if (active) {
        active->enumerated = true;
        QueuePresent(active);
    }
}

static void Finish(State* state, bool force = false) {
    if (!force && !CanPresent(state)) {
        return;
    }
    if (state->finishing) {
        return;
    }
    state->finishing = true;
    HWND overlay = state->overlay;
    ULONG generation = state->generation;
    // Layered cover stays visible while all replacement child windows paint.
    HWND view = FindFileView(state->root);
    if (view) {
        RedrawWindow(view, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    }
    // Submit the completed GDI paint before uncovering it. Waiting for a DWM
    // present here leaves the old screenshot on screen for an extra frame.
    GdiFlush();
    if (active == state) {
        if (state->generation == generation && (force || CanPresent(state))) {
            if (!force) {
                RibbonWork::ViewReady();
            }
            DestroyWindow(overlay);
            Trace(L"EFN present_done");
        } else {
            state->finishing = false;
        }
    }
}
// Overlay window callbacks.

static LRESULT CALLBACK RootSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR data) {
    WindowLifetime::Callback callback(lifetime);
    auto state = reinterpret_cast<State*>(data);
    if (msg == WM_SIZE || msg == WM_DPICHANGED || msg == WM_NCDESTROY) {
        DestroyWindow(state->overlay);
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    auto state = reinterpret_cast<State*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (!state) {
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    switch (msg) {
    case PresentMessage:
        if (wp == state->generation) {
            Finish(state);
        }
        return 0;
    case WM_TIMER:
        if (wp == 1) {
            // Ignore stale timer messages left queued by an earlier navigation.
            UINT remaining =
                NavigationPolicy::RemainingCoverMs(false, state->started, GetTickCount64());
            if (!remaining) {
                Finish(state, true);
            } else if (!SetTimer(hwnd, 1, remaining, nullptr)) {
                Finish(state, true);
            }
        }
        return 0;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATEANDEAT;
    case WM_SETCURSOR:
        SetCursor(LoadCursorW(nullptr, IDC_ARROW));
        return TRUE;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_NCDESTROY:
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(state->root, RootSubclass);
        if (active == state) {
            active = nullptr;
        }
        SelectObject(state->dc, state->oldBitmap);
        DeleteObject(state->bitmap);
        DeleteDC(state->dc);
        delete state;
        lifetime.Remove(hwnd);
        break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
// Capture the outgoing view.

static void Begin(void* itemsView) {
    WindowLifetime::Operation operation(lifetime);
    if (!operation) {
        return;
    }
    ULONGLONG started = navigation.awaitingReset ? navigation.started
                        : active                 ? active->started
                                                 : GetTickCount64();
    navigation.started = started;
    navigation.awaitingReset = false;
    UINT remaining =
        NavigationPolicy::RemainingCoverMs(navigation.remote, started, GetTickCount64());
    if (!remaining) {
        if (active) {
            DestroyWindow(active->overlay);
        }
        return; // Let the native view display its loading state.
    }
    if (active) {
        active->itemsView = itemsView;
        active->started = started;
        ++active->generation;
        active->pending = active->enumerated = false;
        if (!SetTimer(active->overlay, 1, remaining, nullptr)) {
            DestroyWindow(active->overlay);
        }
        return;
    }
    HWND root = FindThreadBrowserWindow();
    if (!root || IsIconic(root)) {
        return;
    }
    HWND view = FindFileView(root);
    RECT rect{};
    if (!view || !GetWindowRect(view, &rect)) {
        return;
    }
    int width = rect.right - rect.left, height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return;
    }
    auto state = new (std::nothrow) State;
    if (!state) {
        return;
    }
    HDC screen = GetDC(nullptr);
    state->dc = CreateCompatibleDC(screen);
    state->bitmap = CreateCompatibleBitmap(screen, width, height);
    if (state->dc && state->bitmap) {
        state->oldBitmap = SelectObject(state->dc, state->bitmap);
        if (!BitBlt(state->dc, 0, 0, width, height, screen, rect.left, rect.top, SRCCOPY)) {
            SelectObject(state->dc, state->oldBitmap);
            DeleteObject(state->bitmap);
            state->bitmap = nullptr;
        }
    }
    ReleaseDC(nullptr, screen);
    if (!state->dc || !state->bitmap) {
        if (state->bitmap) {
            DeleteObject(state->bitmap);
        }
        if (state->dc) {
            DeleteDC(state->dc);
        }
        delete state;
        return;
    }
    POINT position{rect.left, rect.top};
    ScreenToClient(root, &position);
    HWND overlay =
        CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE, WindowClass, L"", WS_CHILD, position.x,
                        position.y, width, height, root, nullptr, instance, nullptr);
    if (!overlay) {
        SelectObject(state->dc, state->oldBitmap);
        DeleteObject(state->bitmap);
        DeleteDC(state->dc);
        delete state;
        return;
    }
    state->overlay = overlay;
    state->root = root;
    state->itemsView = itemsView;
    state->started = started;
    SetWindowLongPtrW(overlay, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
    active = state;
    lifetime.Add(overlay);
    POINT source{};
    SIZE size{width, height};
    remaining = NavigationPolicy::RemainingCoverMs(false, started, GetTickCount64());
    if (!remaining ||
        !WindhawkUtils::SetWindowSubclassFromAnyThread(root, RootSubclass,
                                                       reinterpret_cast<DWORD_PTR>(state)) ||
        !UpdateLayeredWindow(overlay, nullptr, nullptr, &size, state->dc, &source, 0, nullptr,
                             ULW_OPAQUE) ||
        !SetTimer(overlay, 1, remaining, nullptr)) {
        DestroyWindow(overlay);
        return;
    }
    SetWindowPos(overlay, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}
// Navigation and batching hooks.

static void __fastcall BlockHook(void* self, float delay) {
    // Only shorten a hold protected by our cover. A stale navigation timestamp
    // must not disable native redraw protection during later folder updates.
    if (!active || active->itemsView != self) {
        blockOriginal(self, delay);
        return;
    }
    UINT remaining =
        NavigationPolicy::RemainingCoverMs(navigation.remote, active->started, GetTickCount64());
    if (!remaining) {
        blockOriginal(self, delay);
        return;
    }
    // Match the native redraw hold to the cover deadline, not its usual 250 ms.
    if (std::isfinite(delay) && delay >= 0) {
        delay = (std::min)(delay, remaining / 1000.0f);
    }
    blockOriginal(self, delay);
}

static HRESULT __fastcall NavigateHook(void* self, PCIDLIST_ABSOLUTE pidl, ULONG flags,
                                       ULONG extra) {
    RibbonWork::NavigationStarted();
    Navigation previous = navigation;
    navigation = {GetTickCount64(), false, true};
    std::vector<wchar_t> path(32768);
    if (pidl && SHGetPathFromIDListEx(pidl, path.data(), path.size(), GPFIDL_DEFAULT)) {
        navigation.remote = NavigationPolicy::IsRemotePath(path.data());
    }
    if (active) {
        // Cancel the previous destination's readiness messages before entering native code.
        ++active->generation;
        active->pending = active->enumerated = false;
        active->started = navigation.started;
        if (navigation.remote || !settings.coverDeadlineMs ||
            !SetTimer(active->overlay, 1, settings.coverDeadlineMs, nullptr)) {
            DestroyWindow(active->overlay);
        }
    }
    ++navigationDepth;
    HRESULT result = navigateOriginal(self, pidl, flags, extra);
    --navigationDepth;
    if (FAILED(result)) {
        navigation = previous;
        if (active) {
            Finish(active, true);
        }
    } else {
        QueuePresent(active);
    }
    return result;
}

static HRESULT __fastcall ResetHook(void* self, void* item, void* collection) {
    Begin(self);
    HRESULT result = resetOriginal(self, item, collection);
    if (FAILED(result) && active && active->itemsView == self) {
        Finish(active, true);
    }
    return result;
}

static HRESULT __fastcall EnsureHook(void* self) {
    HRESULT result = ensureOriginal(self);
    if (SUCCEEDED(result) && active && active->itemsView == self) {
        active->pending = true;
    }
    return result;
}

static bool __fastcall DeleteHook(void* self) {
    bool result = deleteOriginal(self);
    if (result && processingBatch == self && active && active->itemsView == self) {
        active->pending = false;
    }
    return result;
}

static void __fastcall BatchHook(void* self, void* info) {
    void* previous = processingBatch;
    processingBatch = self;
    batchOriginal(self, info);
    processingBatch = previous;
    if (active && active->itemsView == self) {
        QueuePresent(active);
    }
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

// Window class lifecycle.

static bool Initialize() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(WindowProc), &instance);
    WNDCLASSW cls{};
    cls.lpfnWndProc = WindowProc;
    cls.hInstance = instance;
    cls.lpszClassName = WindowClass;
    registered = RegisterClassW(&cls) != 0;
    return registered;
}

static void BeforeUninit() {
    lifetime.Stop();
    lifetime.CloseAndWait();
}

static void Uninit() {
    if (registered && UnregisterClassW(WindowClass, instance)) {
        registered = false;
    }
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
        // empty, slow, remote and failed navigations with no batching callback.
        if ((ready && !FileTransition::active) ||
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
    Trace(L"EFN navigation_enter");
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
    if (!settings.classicRibbon || !CanDeferInvoke(iid, flags, args) || !Pending() ||
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
    if (settings.classicRibbon && self == connected && Pending() &&
        Enqueue(reinterpret_cast<IUnknown*>(self), [self] {
            Trace(L"EFN ribbon_run");
            navigatedOriginal(self);
        })) {
        Trace(L"EFN ribbon_queued");
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
            Trace(L"EFN navbar_run");
            navStateOriginal(self, flags);
        })) {
        Trace(L"EFN navbar_queued");
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
    Trace(label);
    HRESULT hr = original(self);
    if (FAILED(hr)) {
        Wh_Log(L"Deferred XAML refresh failed: %s, 0x%08X", label, hr);
    }
    return hr;
}
// Modern command-surface hooks.

static HRESULT __fastcall ChangedHook(void* self) {
    // ConnectToView must unsubscribe the old view and subscribe the new view now.
    Trace(L"EFN xaml_connect");
    HRESULT hr = changedOriginal(self);
    connected = SUCCEEDED(hr) ? self : nullptr;
    return hr;
}

static HRESULT __fastcall NavigatedHook(void* self) {
    if (settings.xamlCommandBar && Pending() && self == connected &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self] {
            Run(navigatedOriginal, self, L"EFN xaml_navigated_run");
        })) {
        Trace(L"EFN xaml_navigated_queued");
        return S_OK;
    }
    return navigatedOriginal(self);
}

static HRESULT __fastcall InvalidatedHook(void* self) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self] {
            Run(invalidatedOriginal, self, L"EFN xaml_invalidated_run");
        })) {
        Trace(L"EFN xaml_invalidated_queued");
        return S_OK;
    }
    return invalidatedOriginal(self);
}

static HRESULT __fastcall StateHook(void* self, ULONG flags) {
    // This adapter uses bit 4 to refresh location and commands; other bits are
    // native no-ops. Preserve each notification's flags and FIFO ordering.
    if (settings.navigationBar && (flags & 4) && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self, flags] {
            Trace(L"EFN xaml_location_run");
            HRESULT hr = stateOriginal(self, flags);
            if (FAILED(hr)) {
                Wh_Log(L"Deferred XAML location failed: 0x%08X", hr);
            }
        })) {
        Trace(L"EFN xaml_location_queued");
        return S_OK;
    }
    return stateOriginal(self, flags);
}

static HRESULT __fastcall InvalidateHook(void* self, ULONG commands) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self, commands] {
            Trace(L"EFN xaml_commands_run");
            invalidateOriginal(self, commands);
        })) {
        Trace(L"EFN xaml_commands_queued");
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
            Trace(L"EFN xaml_event_run");
            DISPPARAMS empty{};
            invokeOriginal(self, id, iid, locale, flags, &empty, nullptr, nullptr, nullptr);
        })) {
        Trace(L"EFN xaml_event_queued");
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
    if (!module) {
        Wh_Log(L"Modern XAML adapter unavailable; classic hooks remain active");
        return;
    }
    // Windows.UI.FileExplorer.dll
    const WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"public: virtual long __cdecl CommandBarViewAdapter::OnBrowserNavigated(void)"},
         &navigatedOriginal,
         NavigatedHook,
         true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::OnShellViewChanged(void)"},
         &changedOriginal,
         ChangedHook,
         true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::OnCommandStateInvalidated(void)"},
         &invalidatedOriginal,
         InvalidatedHook,
         true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::SetNavigationState(unsigned long)"},
         &stateOriginal,
         StateHook,
         true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::InvalidateCommands(enum COMMAND_SET)"},
         &invalidateOriginal,
         InvalidateHook,
         true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *)"},
         &invokeOriginal,
         InvokeHook,
         true},
        {{L"public: long __cdecl CommandBarViewAdapter::Cleanup(void)"},
         &cleanupOriginal,
         CleanupHook,
         true},
    };
    bool resolved = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks), options);
    enabled = resolved && navigatedOriginal && changedOriginal && invalidatedOriginal &&
              stateOriginal && invalidateOriginal && invokeOriginal && cleanupOriginal;
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
    bool defer = FileTransition::navigationDepth && !FileTransition::NativeLoadingAllowed() &&
                 !(flags & SWP_NOREDRAW) &&
                 GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId() &&
                 (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD);
    if (defer) {
        wchar_t cls[80];
        if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) ||
            wcscmp(cls, FileTransition::WindowClass) == 0) {
            defer = false;
        }
    }
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
    return FileTransition::navigationDepth && redrawFrameDepth &&
                   !FileTransition::NativeLoadingAllowed()
               ? TRUE
               : updateWindowOriginal(hwnd);
}

static BOOL WINAPI RedrawWindowHook(HWND hwnd, const RECT* rect, HRGN region, UINT flags) {
    if (FileTransition::navigationDepth && renderSizerDepth &&
        !FileTransition::NativeLoadingAllowed()) {
        // _Render flushes every child during navigation layout initialization.
        // Leave invalidation and geometry alone; remove only the immediate flush.
        flags &= ~(RDW_UPDATENOW | RDW_ERASENOW);
    }
    return redrawWindowOriginal(hwnd, rect, region, flags);
}

static void* CreateActionHook(const Action* action) {
    if (settings.duserBatching && action && action->size == sizeof(Action) &&
        action->callback == batchCallback && std::isfinite(action->duration) &&
        action->duration >= 0) {
        Action faster = *action;
        if (faster.duration > 0.005f) {
            faster.duration = 0.005f;
        }
        if (faster.timeSlice == 0 || faster.timeSlice > 5) {
            faster.timeSlice = 5;
        }
        return createActionOriginal(&faster);
    }
    return createActionOriginal(action);
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
#ifdef EFN_DIAGNOSTICS
    GUID provider{0xd1675027, 0xf8d0, 0x4c43, {0x9b, 0x6f, 0x4b, 0x39, 0x0c, 0xe6, 0x46, 0xed}};
    EventRegister(&provider, nullptr, nullptr, &diagnosticProvider);
#endif
    frame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    shell = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (settings.duserBatching) {
        duser = LoadLibraryExW(L"DUser.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    // ExplorerFrame.dll
    const WindhawkUtils::SYMBOL_HOOK frameSymbols[] = {
        {{L"private: static void __cdecl UIItemsView::s_BatchTimerCallback(struct GMA_ACTIONINFO *)"},
         &batchCallback,
         nullptr,
         true},
        {{L"private: long __cdecl CShellBrowser::_NavigateToPidl(struct _ITEMIDLIST_ABSOLUTE const *,unsigned long,unsigned long)"},
         &FileTransition::navigateOriginal,
         FileTransition::NavigateHook},
        {{L"public: void __cdecl UIItemsView::BlockRedrawWithTimeout(float)"},
         &FileTransition::blockOriginal,
         FileTransition::BlockHook},
        {{L"private: long __cdecl UIItemsView::_ResetRoot(struct IItem *,struct IItemCollection *)"},
         &FileTransition::resetOriginal,
         FileTransition::ResetHook},
        {{L"private: long __cdecl UIItemsView::_EnsureBatching(void)"},
         &FileTransition::ensureOriginal,
         FileTransition::EnsureHook},
        {{L"private: void __cdecl UIItemsView::_OnBatchTimer(struct GMA_ACTIONINFO *)"},
         &FileTransition::batchOriginal,
         FileTransition::BatchHook},
        {{L"private: bool __cdecl UIItemsView::DeleteBatchTimer(void)"},
         &FileTransition::deleteOriginal,
         FileTransition::DeleteHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *)"},
         &FileTransition::invokeOriginal,
         FileTransition::InvokeHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::OnBrowserNavigated(void)"},
         &RibbonWork::navigatedOriginal,
         RibbonWork::NavigatedHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::OnShellViewChanged(void)"},
         &RibbonWork::changedOriginal,
         RibbonWork::ChangedHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::DestroyRibbonUI(int)"},
         &RibbonWork::destroyOriginal,
         RibbonWork::DestroyHook},
        {{L"public: virtual long __cdecl CNavBar::SetNavigationState(unsigned long)"},
         &RibbonWork::navStateOriginal,
         RibbonWork::NavStateHook},
    };
    // shell32.dll
    const WindhawkUtils::SYMBOL_HOOK shellSymbols[] = {
        {{L"private: void __cdecl CDUIViewFrame::_RedrawFrame(void)"},
         &redrawFrameOriginal,
         RedrawFrameHook},
        {{L"private: long __cdecl CDUISizerElement::_Render(void)"},
         &renderSizerOriginal,
         RenderSizerHook},
    };
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    bool frameSymbolsReady =
        frame && WindhawkUtils::HookSymbols(frame, frameSymbols, ARRAYSIZE(frameSymbols), &options);
    if (!frameSymbolsReady) {
        Wh_Log(L"Required ExplorerFrame symbols unavailable");
        return FALSE;
    }
    bool shellSymbolsReady =
        shell && WindhawkUtils::HookSymbols(shell, shellSymbols, ARRAYSIZE(shellSymbols), &options);
    if (!shellSymbolsReady) {
        Wh_Log(L"Required activation/layout paint hooks unavailable");
        return FALSE;
    }
    updateWindowOriginal = UpdateWindow;
    redrawWindowOriginal = RedrawWindow;
    setWindowPosOriginal = SetWindowPos;
    bool paint = Hook(updateWindowOriginal, UpdateWindowHook);
    bool layoutPaint = Hook(redrawWindowOriginal, RedrawWindowHook);
    bool positionPaint = Hook(setWindowPosOriginal, SetWindowPosHook);
    if (!paint || !layoutPaint || !positionPaint) {
        Wh_Log(L"Required paint hook failed: activation=%d, layout=%d", paint, layoutPaint);
        return FALSE; // Windhawk rolls back hooks when initialization fails.
    }
    if (!FileTransition::Initialize()) {
        Wh_Log(L"Required file-view transition hooks unavailable");
        return FALSE;
    }
    if (!RibbonWork::Initialize()) {
        Wh_Log(L"Required deferred UI hooks unavailable");
        return FALSE;
    }
    XamlWork::Initialize(&options);
    bool batches = false;
    if (frameSymbolsReady && batchCallback && duser) {
        createActionOriginal =
            reinterpret_cast<CreateAction>(GetProcAddress(duser, "CreateAction"));
        batches = Hook(createActionOriginal, CreateActionHook);
    }
    Wh_Log(L"Ribbon preserved. Hooks: batching=%d, activation paint=%d, layout paint=%d", batches,
           paint, layoutPaint);
    return TRUE;
}

// Windhawk lifecycle entry points (must remain externally visible).

BOOL Wh_ModInit() {
    if (InitializeMod()) {
        return TRUE;
    }
    // Windhawk rolls back hooks on failure, but doesn't call Wh_ModUninit.
    FileTransition::Uninit();
    RibbonWork::Uninit();
    ReleaseModules();
#ifdef EFN_DIAGNOSTICS
    EventUnregister(diagnosticProvider);
#endif
    return FALSE;
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}

void Wh_ModBeforeUninit() {
    FileTransition::lifetime.Stop();
    RibbonWork::lifetime.Stop();
    FileTransition::BeforeUninit();
    RibbonWork::BeforeUninit();
}

void Wh_ModUninit() {
    FileTransition::Uninit();
    RibbonWork::Uninit();
    ReleaseModules();
#ifdef EFN_DIAGNOSTICS
    EventUnregister(diagnosticProvider);
#endif
}
