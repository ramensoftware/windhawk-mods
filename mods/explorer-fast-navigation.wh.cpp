// ==WindhawkMod==
// @id              explorer-fast-navigation
// @name            Explorer Fast Navigation
// @description     Improves explorer navigation latency
// @version         0.9.0
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

#include <windows.h>
#include <shlobj.h>

namespace NavigationPolicy {
constexpr ULONGLONG LoadingDelayMs = 200;
bool IsRemotePath(const wchar_t* path, decltype(&GetDriveTypeW) driveType = GetDriveTypeW) {
    if (!path) return false;
    if (_wcsnicmp(path, L"\\\\?\\UNC\\", 8) == 0) return true;
    if (wcsncmp(path, L"\\\\?\\", 4) == 0) path += 4;
    else if (wcsncmp(path, L"\\\\", 2) == 0) return true;
    if (path[0] && path[1] == L':' && path[2] == L'\\') {
        wchar_t root[] = {path[0], L':', L'\\', 0};
        return driveType(root) == DRIVE_REMOTE;
    }
    return false;
}
UINT RemainingCoverMs(bool remote, ULONGLONG started, ULONGLONG now) {
    ULONGLONG elapsed = now - started;
    return remote || elapsed >= LoadingDelayMs ? 0 : static_cast<UINT>(LoadingDelayMs - elapsed);
}
} // namespace NavigationPolicy

#ifndef EXPLORER_NAVIGATION_POLICY_TEST
#include <windhawk_utils.h>
#include <cmath>
#include <oaidl.h>
#include <shdispid.h>

// Preserve the last rendered file pane until the new view has finished batching.
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <new>
#include <vector>
#include <functional>
#include <deque>

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
            if (admitted) ++owner.operations;
        }
        Operation(const Operation&) = delete;
        ~Operation() {
            if (admitted) {
                std::lock_guard lock(owner.mutex);
                --owner.operations;
                owner.changed.notify_all();
            }
        }
        explicit operator bool() const { return admitted; }
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
            changed.wait(lock, [this] { return operations == 0; });
            copy = windows;
        }
        for (HWND hwnd : copy) SendMessageW(hwnd, WM_CLOSE, 0, 0);
        // A nested WM_CLOSE during Flush only requests closure. The outer
        // drain destroys the window after releasing its outstanding COM refs.
        std::unique_lock lock(mutex);
        changed.wait(lock, [this] { return windows.empty() && callbacks == 0; });
    }
};

bool CanDeferInvoke(REFIID iid, WORD flags, const DISPPARAMS* args) {
    const GUID nullIid{};
    return IsEqualGUID(iid, nullIid) && flags == DISPATCH_METHOD && args &&
        args->cArgs == 0 && args->cNamedArgs == 0;
}

#ifdef EFN_DIAGNOSTICS
#include <evntprov.h>
REGHANDLE diagnosticProvider;
void Trace(const wchar_t* message) { EventWriteString(diagnosticProvider, 4, 1, message); }
#else
void Trace(const wchar_t*) {}
#endif

namespace RibbonWork {
bool Initialize();
void BeforeUninit();
void Uninit();
void Flush();
bool DeferInvoke(void*, DISPID, REFIID, LCID, WORD, DISPPARAMS*);
void NavigationStarted();
void ViewReady();
}

namespace FileTransition {
constexpr wchar_t WindowClass[] = L"Windhawk.ExplorerFastNavigation.FileTransition";
constexpr UINT PresentMessage = WM_APP + 71;
struct State {
    HWND overlay{}, root{};
    HDC dc{};
    HBITMAP bitmap{};
    HGDIOBJ oldBitmap{};
    void* itemsView{};
    ULONG generation = 1;
    ULONGLONG started = 0;
    bool pending = false, enumerated = false, finishing = false;
};
thread_local State* active;
thread_local void* processingBatch;
struct Navigation {
    ULONGLONG started = 0;
    bool remote = false, awaitingReset = false;
};
thread_local Navigation navigation;
thread_local unsigned navigationDepth;
bool NativeLoadingAllowed() {
    return navigation.remote || (navigation.started &&
        !NavigationPolicy::RemainingCoverMs(false, navigation.started, GetTickCount64()));
}
WindowLifetime lifetime;
HINSTANCE instance;
bool registered;

HWND FindFileView(HWND root) {
    HWND result = nullptr;
    EnumChildWindows(root, [](HWND hwnd, LPARAM param) -> BOOL {
        wchar_t cls[64];
        if (IsWindowVisible(hwnd) && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
            wcscmp(cls, L"SHELLDLL_DefView") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}
bool CanPresent(const State* state) {
    // An empty batch queue can be only a gap between enumeration batches.
    return state && state->enumerated && !state->pending && !navigationDepth &&
        processingBatch != state->itemsView;
}
void QueuePresent(State* state) {
    if (CanPresent(state))
        PostMessageW(state->overlay, PresentMessage, state->generation, 0);
}
void Finish(State* state, bool force = false) {
    if (!force && !CanPresent(state)) return;
    if (state->finishing) return;
    state->finishing = true;
    HWND overlay = state->overlay;
    ULONG generation = state->generation;
    // Layered cover stays visible while all replacement child windows paint.
    HWND view = FindFileView(state->root);
    if (view) RedrawWindow(view, nullptr, nullptr,
        RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    // Submit the completed GDI paint before uncovering it. Waiting for a DWM
    // present here leaves the old screenshot on screen for an extra frame.
    GdiFlush();
    if (active == state) {
        if (state->generation == generation && (force || CanPresent(state))) {
            if (!force) RibbonWork::ViewReady();
            DestroyWindow(overlay);
            Trace(L"EFN present_done");
        }
        else state->finishing = false;
    }
}
LRESULT CALLBACK RootSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR data) {
    WindowLifetime::Callback callback(lifetime);
    auto state = reinterpret_cast<State*>(data);
    if (msg == WM_SIZE || msg == WM_DPICHANGED || msg == WM_NCDESTROY)
        DestroyWindow(state->overlay);
    return DefSubclassProc(hwnd, msg, wp, lp);
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    auto state = reinterpret_cast<State*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (!state) return DefWindowProcW(hwnd, msg, wp, lp);
    switch (msg) {
    case PresentMessage:
        if (wp == state->generation) Finish(state);
        return 0;
    case WM_TIMER:
        if (wp == 1) {
            // Ignore stale timer messages left queued by an earlier navigation.
            UINT remaining = NavigationPolicy::RemainingCoverMs(false, state->started, GetTickCount64());
            if (!remaining) Finish(state, true);
            else if (!SetTimer(hwnd, 1, remaining, nullptr)) Finish(state, true);
        }
        return 0;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATEANDEAT;
    case WM_SETCURSOR:
        SetCursor(LoadCursorW(nullptr, IDC_ARROW));
        return TRUE;
    case WM_ERASEBKGND: return TRUE;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_NCDESTROY:
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(state->root, RootSubclass);
        if (active == state) active = nullptr;
        SelectObject(state->dc, state->oldBitmap);
        DeleteObject(state->bitmap);
        DeleteDC(state->dc);
        delete state;
        lifetime.Remove(hwnd);
        break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
void Begin(void* itemsView) {
    WindowLifetime::Operation operation(lifetime);
    if (!operation) return;
    ULONGLONG started = navigation.awaitingReset ? navigation.started :
        active ? active->started : GetTickCount64();
    navigation.started = started;
    navigation.awaitingReset = false;
    UINT remaining = NavigationPolicy::RemainingCoverMs(navigation.remote, started, GetTickCount64());
    if (!remaining) {
        if (active) DestroyWindow(active->overlay);
        return; // Let the native view display its loading state.
    }
    if (active) {
        active->itemsView = itemsView;
        active->started = started;
        ++active->generation;
        active->pending = active->enumerated = false;
        if (!SetTimer(active->overlay, 1, remaining, nullptr)) DestroyWindow(active->overlay);
        return;
    }
    HWND root = GetAncestor(GetForegroundWindow(), GA_ROOT);
    wchar_t cls[64];
    if (!root || GetWindowThreadProcessId(root, nullptr) != GetCurrentThreadId() ||
        IsIconic(root) || !GetClassNameW(root, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"CabinetWClass") != 0) return;
    HWND view = FindFileView(root);
    RECT rect{};
    if (!view || !GetWindowRect(view, &rect)) return;
    int width = rect.right - rect.left, height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) return;
    auto state = new (std::nothrow) State;
    if (!state) return;
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
        if (state->bitmap) DeleteObject(state->bitmap);
        if (state->dc) DeleteDC(state->dc);
        delete state;
        return;
    }
    POINT position{rect.left, rect.top};
    ScreenToClient(root, &position);
    HWND overlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOACTIVATE,
        WindowClass, L"", WS_CHILD, position.x, position.y, width, height,
        root, nullptr, instance, nullptr);
    if (!overlay) {
        SelectObject(state->dc, state->oldBitmap);
        DeleteObject(state->bitmap); DeleteDC(state->dc); delete state;
        return;
    }
    state->overlay = overlay; state->root = root; state->itemsView = itemsView;
    state->started = started;
    SetWindowLongPtrW(overlay, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
    active = state;
    lifetime.Add(overlay);
    POINT source{};
    SIZE size{width, height};
    remaining = NavigationPolicy::RemainingCoverMs(false, started, GetTickCount64());
    if (!remaining || !WindhawkUtils::SetWindowSubclassFromAnyThread(root, RootSubclass,
            reinterpret_cast<DWORD_PTR>(state)) ||
        !UpdateLayeredWindow(overlay, nullptr, nullptr, &size, state->dc,
            &source, 0, nullptr, ULW_OPAQUE) || !SetTimer(overlay, 1, remaining, nullptr)) {
        DestroyWindow(overlay);
        return;
    }
    SetWindowPos(overlay, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}
using ResetRoot = HRESULT(__fastcall*)(void*, void*, void*);
using Navigate = HRESULT(__fastcall*)(void*, PCIDLIST_ABSOLUTE, ULONG, ULONG);
using BlockRedraw = void(__fastcall*)(void*, float);
using UnblockRedraw = void(__fastcall*)(void*);
using EnsureBatching = HRESULT(__fastcall*)(void*);
using BatchTimer = void(__fastcall*)(void*, void*);
using DeleteBatchTimer = bool(__fastcall*)(void*);
using Invoke = HRESULT(__fastcall*)(void*, DISPID, REFIID, LCID, WORD,
    DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
ResetRoot resetOriginal;
Navigate navigateOriginal;
BlockRedraw blockOriginal;
UnblockRedraw unblockRedraw;
EnsureBatching ensureOriginal;
BatchTimer batchOriginal;
DeleteBatchTimer deleteOriginal;
Invoke invokeOriginal;

void __fastcall BlockHook(void* self, float delay) {
    // Only shorten a hold protected by our cover. A stale navigation timestamp
    // must not disable native redraw protection during later folder updates.
    if (!active || active->itemsView != self) {
        blockOriginal(self, delay);
        return;
    }
    UINT remaining = NavigationPolicy::RemainingCoverMs(navigation.remote,
        active->started, GetTickCount64());
    if (!remaining) {
        blockOriginal(self, delay);
        return;
    }
    // Match the native redraw hold to the cover deadline, not its usual 250 ms.
    if (std::isfinite(delay) && delay >= 0)
        delay = (std::min)(delay, remaining / 1000.0f);
    blockOriginal(self, delay);
}
void EnumerationDone() {
    if (active) {
        active->enumerated = true;
        QueuePresent(active);
    }
}

HRESULT __fastcall NavigateHook(void* self, PCIDLIST_ABSOLUTE pidl, ULONG flags, ULONG extra) {
    RibbonWork::NavigationStarted();
    Navigation previous = navigation;
    navigation = {GetTickCount64(), false, true};
    wchar_t path[32768];
    if (pidl && SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path), GPFIDL_DEFAULT))
        navigation.remote = NavigationPolicy::IsRemotePath(path);
    if (active) {
        // Cancel the previous destination's readiness messages before entering native code.
        ++active->generation;
        active->pending = active->enumerated = false;
        active->started = navigation.started;
        if (navigation.remote || !SetTimer(active->overlay, 1,
                static_cast<UINT>(NavigationPolicy::LoadingDelayMs), nullptr))
            DestroyWindow(active->overlay);
    }
    ++navigationDepth;
    HRESULT result = navigateOriginal(self, pidl, flags, extra);
    --navigationDepth;
    if (FAILED(result)) {
        navigation = previous;
        if (active) Finish(active, true);
    }
    else QueuePresent(active);
    return result;
}

HRESULT __fastcall ResetHook(void* self, void* item, void* collection) {
    Begin(self);
    HRESULT result = resetOriginal(self, item, collection);
    if (FAILED(result) && active && active->itemsView == self) Finish(active, true);
    return result;
}
HRESULT __fastcall EnsureHook(void* self) {
    HRESULT result = ensureOriginal(self);
    if (SUCCEEDED(result) && active && active->itemsView == self) {
        active->pending = true;
    }
    return result;
}
bool __fastcall DeleteHook(void* self) {
    bool result = deleteOriginal(self);
    if (result && processingBatch == self && active && active->itemsView == self) {
        active->pending = false;
    }
    return result;
}
void __fastcall BatchHook(void* self, void* info) {
    void* previous = processingBatch;
    processingBatch = self;
    batchOriginal(self, info);
    processingBatch = previous;
    if (active && active->itemsView == self) QueuePresent(active);
}
HRESULT __fastcall InvokeHook(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
        DISPPARAMS* args, VARIANT* result, EXCEPINFO* exception, UINT* argError) {
    HRESULT hr;
    if (RibbonWork::DeferInvoke(self, id, iid, locale, flags, args)) {
        if (result) *result = {};
        hr = S_OK;
    } else {
        hr = invokeOriginal(self, id, iid, locale, flags, args, result, exception, argError);
    }
    if (id == DISPID_FILELISTENUMDONE) EnumerationDone();
    return hr;
}
bool Initialize() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(WindowProc), &instance);
    WNDCLASSW cls{};
    cls.lpfnWndProc = WindowProc; cls.hInstance = instance; cls.lpszClassName = WindowClass;
    registered = RegisterClassW(&cls) != 0;
    return registered;
}
void BeforeUninit() {
    lifetime.Stop();
    lifetime.CloseAndWait();
}
void Uninit() {
    if (registered && UnregisterClassW(WindowClass, instance)) registered = false;
}
} // namespace FileTransition

// Run ribbon state refreshes on the owning UI thread after native view painting.
// OnShellViewChanged still connects the new view immediately. Only defer
// OnBrowserNavigated after that connection succeeded; first-time setup is native.
namespace RibbonWork {
constexpr wchar_t ClassName[] = L"Windhawk.ExplorerFastNavigation.RibbonWork";
struct Task { IUnknown* object; std::function<void()> run; };
struct Queue {
    HWND hwnd{}, root{};
    HHOOK inputHook{};
    std::deque<Task> tasks;
    bool draining = false, closing = false;
};
thread_local Queue* queue;
thread_local void* connected;
thread_local ULONGLONG started;
thread_local bool ready;
WindowLifetime lifetime;
HINSTANCE instance;
bool registered;
using Notification = HRESULT(__fastcall*)(void*);
using Destroy = HRESULT(__fastcall*)(void*, BOOL);
using NavState = HRESULT(__fastcall*)(void*, ULONG);
Notification navigatedOriginal, changedOriginal;
Destroy destroyOriginal;
NavState navStateOriginal;

void Flush() {
    WindowLifetime::Callback callback(lifetime);
    Queue* q = queue;
    if (!q || q->draining) return;
    q->draining = true;
    KillTimer(q->hwnd, 1);
    while (!q->tasks.empty()) {
        Task task = std::move(q->tasks.front());
        q->tasks.pop_front();
        if (!q->closing) task.run();
        task.object->Release();
    }
    q->draining = false;
    if (q->closing) DestroyWindow(q->hwnd);
}
LRESULT CALLBACK InputHook(int code, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (code >= 0 && wp == PM_REMOVE && queue && !queue->tasks.empty()) {
        const auto msg = reinterpret_cast<const MSG*>(lp);
        // Refresh before accepting input so deferred ribbon/address commands
        // never execute against their previous displayed state.
        if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN ||
            msg->message == WM_LBUTTONDOWN || msg->message == WM_RBUTTONDOWN ||
            msg->message == WM_MBUTTONDOWN || msg->message == WM_NCLBUTTONDOWN)
            Flush();
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}
LRESULT CALLBACK OwnerSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR data) {
    WindowLifetime::Callback callback(lifetime);
    if (msg == WM_DESTROY) {
        auto q = reinterpret_cast<Queue*>(data);
        q->closing = true;
        Flush(); // Drop pending presentation work; release COM refs on this thread.
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}
LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (msg == WM_TIMER && wp == 1) {
        // WM_TIMER has lower priority than paint. A bounded fallback also handles
        // empty, slow, remote and failed navigations with no batching callback.
        if ((ready && !FileTransition::active) || GetTickCount64() - started >= 200)
            Flush();
        return 0;
    }
    if (msg == WM_CLOSE) {
        auto q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (!q) { DestroyWindow(hwnd); return 0; }
        q->closing = true;
        // Flush owns q while draining. CloseAndWait waits for its destruction.
        Flush();
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        Queue* q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        if (q && q->inputHook) UnhookWindowsHookEx(q->inputHook);
        if (q && q->root) WindhawkUtils::RemoveWindowSubclassFromAnyThread(q->root, OwnerSubclass);
        if (queue == q) queue = nullptr;
        delete q;
        lifetime.Remove(hwnd);
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
bool Enqueue(IUnknown* object, std::function<void()> run) {
    WindowLifetime::Operation operation(lifetime);
    if (!operation || (queue && queue->draining)) return false;
    if (!queue) {
        HWND root = GetAncestor(GetForegroundWindow(), GA_ROOT);
        wchar_t cls[64];
        if (!root || GetWindowThreadProcessId(root, nullptr) != GetCurrentThreadId() ||
                !GetClassNameW(root, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"CabinetWClass") != 0)
            return false;
        auto q = new (std::nothrow) Queue;
        if (!q) return false;
        q->hwnd = CreateWindowExW(0, ClassName, L"", 0, 0, 0, 0, 0,
            HWND_MESSAGE, nullptr, instance, nullptr);
        if (!q->hwnd) { delete q; return false; }
        SetWindowLongPtrW(q->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(q));
        q->root = root;
        queue = q;
        lifetime.Add(q->hwnd);
        q->inputHook = SetWindowsHookExW(WH_GETMESSAGE, InputHook, instance, GetCurrentThreadId());
        if (!q->inputHook || !WindhawkUtils::SetWindowSubclassFromAnyThread(root, OwnerSubclass,
                reinterpret_cast<DWORD_PTR>(q))) { DestroyWindow(q->hwnd); return false; }
    }
    if (queue->closing || queue->tasks.size() >= 64) { Flush(); return false; }
    if (!SetTimer(queue->hwnd, 1, 10, nullptr)) { Flush(); return false; }
    queue->tasks.push_back({object, std::move(run)});
    object->AddRef();
    return true;
}
void NavigationStarted() { Flush(); started = GetTickCount64(); ready = false; Trace(L"EFN navigation_enter"); }
void ViewReady() { ready = true; }
bool Pending() { return started && !ready && GetTickCount64() - started < 200; }
bool DeferInvoke(void* self, DISPID id, REFIID iid, LCID locale, WORD flags, DISPPARAMS* args) {
    // The inspected branches return S_OK without consuming arguments. Check the
    // actual call too; argument-bearing events must retain their native inputs.
    if (!CanDeferInvoke(iid, flags, args) || !Pending() ||
            (id != 200 && id != 201 && id != 205 && id != 207 &&
            id != 212 && id != 215 && id != 220)) return false;
    return Enqueue(reinterpret_cast<IUnknown*>(self), [self, id, iid, locale, flags] {
        DISPPARAMS empty{};
        FileTransition::invokeOriginal(self, id, iid, locale, flags,
            &empty, nullptr, nullptr, nullptr);
    });
}
HRESULT __fastcall NavigatedHook(void* self) {
    if (self == connected && Pending() &&
            Enqueue(reinterpret_cast<IUnknown*>(self), [self] { Trace(L"EFN ribbon_run"); navigatedOriginal(self); })) {
        Trace(L"EFN ribbon_queued");
        return S_OK;
    }
    return navigatedOriginal(self);
}
HRESULT __fastcall ChangedHook(void* self) {
    // NavigationStarted already drained the previous destination. Keep this
    // destination's queued UI updates behind the new file view.
    if (!Pending()) Flush();
    HRESULT hr = changedOriginal(self);
    connected = SUCCEEDED(hr) ? self : nullptr;
    return hr;
}
HRESULT __fastcall DestroyHook(void* self, BOOL final) {
    if (queue && queue->draining) queue->closing = true;
    Flush();
    if (connected == self) connected = nullptr;
    return destroyOriginal(self, final);
}
HRESULT __fastcall NavStateHook(void* self, ULONG flags) {
    if (Pending() && Enqueue(reinterpret_cast<IUnknown*>(self), [self, flags] {
            Trace(L"EFN navbar_run");
            navStateOriginal(self, flags);
        })) { Trace(L"EFN navbar_queued"); return S_OK; }
    return navStateOriginal(self, flags);
}
bool Initialize() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(Proc), &instance);
    WNDCLASSW cls{};
    cls.lpfnWndProc = Proc; cls.hInstance = instance; cls.lpszClassName = ClassName;
    registered = RegisterClassW(&cls) != 0;
    return registered;
}
void BeforeUninit() {
    lifetime.Stop();
    lifetime.CloseAndWait();
}
void Uninit() {
    if (registered && UnregisterClassW(ClassName, instance)) registered = false;
}
} // namespace RibbonWork

// The modern command surface has its own event sink and navigation-band
// implementation. Delaying CExplorerRibbon alone never reaches this path.
namespace XamlWork {
HMODULE module;
bool enabled;
thread_local void* connected;
using Notification = HRESULT(__fastcall*)(void*);
using StateChange = HRESULT(__fastcall*)(void*, ULONG);
using Invoke = FileTransition::Invoke;
Notification navigatedOriginal, changedOriginal, invalidatedOriginal, cleanupOriginal;
StateChange stateOriginal, invalidateOriginal;
Invoke invokeOriginal;

bool Pending() { return enabled && RibbonWork::Pending(); }
HRESULT Run(Notification original, void* self, const wchar_t* label) {
    Trace(label);
    HRESULT hr = original(self);
    if (FAILED(hr)) Wh_Log(L"Deferred XAML refresh failed: %s, 0x%08X", label, hr);
    return hr;
}
HRESULT __fastcall ChangedHook(void* self) {
    // ConnectToView must unsubscribe the old view and subscribe the new view now.
    Trace(L"EFN xaml_connect");
    HRESULT hr = changedOriginal(self);
    connected = SUCCEEDED(hr) ? self : nullptr;
    return hr;
}
HRESULT __fastcall NavigatedHook(void* self) {
    if (Pending() && self == connected && RibbonWork::Enqueue(
            reinterpret_cast<IUnknown*>(self), [self] {
                Run(navigatedOriginal, self, L"EFN xaml_navigated_run");
            })) { Trace(L"EFN xaml_navigated_queued"); return S_OK; }
    return navigatedOriginal(self);
}
HRESULT __fastcall InvalidatedHook(void* self) {
    if (Pending() && RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self] {
            Run(invalidatedOriginal, self, L"EFN xaml_invalidated_run");
        })) { Trace(L"EFN xaml_invalidated_queued"); return S_OK; }
    return invalidatedOriginal(self);
}
HRESULT __fastcall StateHook(void* self, ULONG flags) {
    // This adapter uses bit 4 to refresh location and commands; other bits are
    // native no-ops. Preserve each notification's flags and FIFO ordering.
    if ((flags & 4) && Pending() && RibbonWork::Enqueue(
            reinterpret_cast<IUnknown*>(self), [self, flags] {
                Trace(L"EFN xaml_location_run");
                HRESULT hr = stateOriginal(self, flags);
                if (FAILED(hr)) Wh_Log(L"Deferred XAML location failed: 0x%08X", hr);
            })) { Trace(L"EFN xaml_location_queued"); return S_OK; }
    return stateOriginal(self, flags);
}
HRESULT __fastcall InvalidateHook(void* self, ULONG commands) {
    if (Pending() && RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self), [self, commands] {
            Trace(L"EFN xaml_commands_run");
            invalidateOriginal(self, commands);
        })) { Trace(L"EFN xaml_commands_queued"); return S_OK; }
    return invalidateOriginal(self, commands);
}
HRESULT __fastcall InvokeHook(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
        DISPPARAMS* args, VARIANT* result, EXCEPINFO* exception, UINT* argError) {
    // Inspected argument-free invalidations. DISPID 213 consumes arguments and
    // handles Ctrl+wheel; it must always execute synchronously with native args.
    bool invalidation = id == 200 || id == 201 || id == 205 || id == 207 ||
        id == 211 || id == 212 || id == 215 || id == 220;
    HRESULT hr;
    if (invalidation && CanDeferInvoke(iid, flags, args) && Pending() && RibbonWork::Enqueue(
            reinterpret_cast<IUnknown*>(self), [self, id, iid, locale, flags] {
                Trace(L"EFN xaml_event_run");
                DISPPARAMS empty{};
                invokeOriginal(self, id, iid, locale, flags, &empty, nullptr, nullptr, nullptr);
            })) {
        Trace(L"EFN xaml_event_queued");
        if (result) *result = {};
        hr = S_OK;
    } else {
        hr = invokeOriginal(self, id, iid, locale, flags, args, result, exception, argError);
    }
    if (id == DISPID_FILELISTENUMDONE) FileTransition::EnumerationDone();
    return hr;
}
HRESULT __fastcall CleanupHook(void* self) {
    // Cleanup disconnects the view and site even if queued COM refs keep the
    // allocation alive. Never replay presentation work after that disconnect.
    if (RibbonWork::queue && RibbonWork::queue->draining)
        RibbonWork::queue->closing = true;
    RibbonWork::Flush();
    connected = nullptr;
    return cleanupOriginal(self);
}
void Initialize(const WH_HOOK_SYMBOLS_OPTIONS* options) {
    module = LoadLibraryExW(L"Windows.UI.FileExplorer.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) { Wh_Log(L"Modern XAML adapter unavailable; classic hooks remain active"); return; }
    // Windows.UI.FileExplorer.dll
    const WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"public: virtual long __cdecl CommandBarViewAdapter::OnBrowserNavigated(void)"}, &navigatedOriginal, NavigatedHook, true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::OnShellViewChanged(void)"}, &changedOriginal, ChangedHook, true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::OnCommandStateInvalidated(void)"}, &invalidatedOriginal, InvalidatedHook, true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::SetNavigationState(unsigned long)"}, &stateOriginal, StateHook, true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::InvalidateCommands(enum COMMAND_SET)"}, &invalidateOriginal, InvalidateHook, true},
        {{L"public: virtual long __cdecl CommandBarViewAdapter::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *)"}, &invokeOriginal, InvokeHook, true},
        {{L"public: long __cdecl CommandBarViewAdapter::Cleanup(void)"}, &cleanupOriginal, CleanupHook, true},
    };
    bool resolved = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks), options);
    enabled = resolved && navigatedOriginal && changedOriginal && invalidatedOriginal &&
        stateOriginal && invalidateOriginal && invokeOriginal && cleanupOriginal;
    Wh_Log(L"Modern XAML command/location deferral=%d", enabled);
}
void Uninit() { if (module) FreeLibrary(module); module = nullptr; }
} // namespace XamlWork

using Callback = void(*)(void*);
struct Action { // GMA_ACTION ABI; checked before reading callback/parameters.
    DWORD size;
    float startDelay, duration, repeatDelay;
    int repeatCount, timeSlice;
    Callback callback;
    void* context;
};
static_assert(sizeof(Action) == 40 && offsetof(Action, callback) == 24);
using CreateAction = void*(*)(const Action*);

Callback batchCallback;
using RedrawFrame = void(__fastcall*)(void*);
using RenderSizer = HRESULT(__fastcall*)(void*);
RedrawFrame redrawFrameOriginal;
RenderSizer renderSizerOriginal;
thread_local unsigned redrawFrameDepth, renderSizerDepth;
struct PaintScope {
    unsigned& depth;
    explicit PaintScope(unsigned& depth) : depth(depth) { ++depth; }
    PaintScope(const PaintScope&) = delete;
    ~PaintScope() { --depth; }
};
void __fastcall RedrawFrameHook(void* self) {
    PaintScope scope(redrawFrameDepth);
    redrawFrameOriginal(self);
}
HRESULT __fastcall RenderSizerHook(void* self) {
    PaintScope scope(renderSizerDepth);
    return renderSizerOriginal(self);
}
CreateAction createActionOriginal;
decltype(&UpdateWindow) updateWindowOriginal;
decltype(&RedrawWindow) redrawWindowOriginal;
decltype(&SetWindowPos) setWindowPosOriginal;
BOOL WINAPI SetWindowPosHook(HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags) {
    bool defer = FileTransition::navigationDepth && !FileTransition::NativeLoadingAllowed() &&
        !(flags & SWP_NOREDRAW) && GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId() &&
        (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD);
    if (defer) {
        wchar_t cls[80];
        if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) || wcscmp(cls, FileTransition::WindowClass) == 0)
            defer = false;
    }
    BOOL result = setWindowPosOriginal(hwnd, after, x, y, cx, cy, defer ? flags | SWP_NOREDRAW : flags);
    if (defer && result) {
        // Geometry and notifications run normally. Queue the complete affected
        // parent region because moving a child also uncovers its old rectangle.
        HWND parent = GetParent(hwnd);
        RedrawWindow(parent ? parent : hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
    return result;
}
BOOL WINAPI UpdateWindowHook(HWND hwnd) {
    // Scope survives intervening API hooks installed by other mods. Suppress
    // only while this thread is navigating inside the relevant shell32 host.
    return FileTransition::navigationDepth && redrawFrameDepth && !FileTransition::NativeLoadingAllowed()
        ? TRUE : updateWindowOriginal(hwnd);
}
BOOL WINAPI RedrawWindowHook(HWND hwnd, const RECT* rect, HRGN region, UINT flags) {
    if (FileTransition::navigationDepth && renderSizerDepth && !FileTransition::NativeLoadingAllowed()) {
        // _Render flushes every child during navigation layout initialization.
        // Leave invalidation and geometry alone; remove only the immediate flush.
        flags &= ~(RDW_UPDATENOW | RDW_ERASENOW);
    }
    return redrawWindowOriginal(hwnd, rect, region, flags);
}
void* CreateActionHook(const Action* action) {
    if (action && action->size == sizeof(Action) && action->callback == batchCallback &&
        std::isfinite(action->duration) && action->duration >= 0) {
        Action faster = *action;
        if (faster.duration > 0.005f) faster.duration = 0.005f;
        if (faster.timeSlice == 0 || faster.timeSlice > 5) faster.timeSlice = 5;
        return createActionOriginal(&faster);
    }
    return createActionOriginal(action);
}
template<class T> bool Hook(T& original, T replacement) {
    return original && Wh_SetFunctionHook(reinterpret_cast<void*>(original),
        reinterpret_cast<void*>(replacement), reinterpret_cast<void**>(&original));
}

HMODULE frame, shell, duser;
void ReleaseModules() {
    XamlWork::Uninit();
    if (duser) FreeLibrary(duser);
    if (shell) FreeLibrary(shell);
    if (frame) FreeLibrary(frame);
    duser = shell = frame = nullptr;
}

BOOL InitializeMod() {
#ifdef EFN_DIAGNOSTICS
    GUID provider{0xd1675027, 0xf8d0, 0x4c43, {0x9b, 0x6f, 0x4b, 0x39, 0x0c, 0xe6, 0x46, 0xed}};
    EventRegister(&provider, nullptr, nullptr, &diagnosticProvider);
#endif
    frame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    shell = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    duser = LoadLibraryExW(L"DUser.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    // ExplorerFrame.dll
    const WindhawkUtils::SYMBOL_HOOK frameSymbols[] = {
        {{L"private: static void __cdecl UIItemsView::s_BatchTimerCallback(struct GMA_ACTIONINFO *)"}, &batchCallback, nullptr, true},
        {{L"private: long __cdecl CShellBrowser::_NavigateToPidl(struct _ITEMIDLIST_ABSOLUTE const *,unsigned long,unsigned long)"}, &FileTransition::navigateOriginal, FileTransition::NavigateHook},
        {{L"public: void __cdecl UIItemsView::BlockRedrawWithTimeout(float)"}, &FileTransition::blockOriginal, FileTransition::BlockHook},
        {{L"public: void __cdecl UIItemsView::UnblockRedraw(void)"}, &FileTransition::unblockRedraw},
        {{L"private: long __cdecl UIItemsView::_ResetRoot(struct IItem *,struct IItemCollection *)"}, &FileTransition::resetOriginal, FileTransition::ResetHook},
        {{L"private: long __cdecl UIItemsView::_EnsureBatching(void)"}, &FileTransition::ensureOriginal, FileTransition::EnsureHook},
        {{L"private: void __cdecl UIItemsView::_OnBatchTimer(struct GMA_ACTIONINFO *)"}, &FileTransition::batchOriginal, FileTransition::BatchHook},
        {{L"private: bool __cdecl UIItemsView::DeleteBatchTimer(void)"}, &FileTransition::deleteOriginal, FileTransition::DeleteHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *)"}, &FileTransition::invokeOriginal, FileTransition::InvokeHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::OnBrowserNavigated(void)"}, &RibbonWork::navigatedOriginal, RibbonWork::NavigatedHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::OnShellViewChanged(void)"}, &RibbonWork::changedOriginal, RibbonWork::ChangedHook},
        {{L"public: virtual long __cdecl CExplorerRibbon::DestroyRibbonUI(int)"}, &RibbonWork::destroyOriginal, RibbonWork::DestroyHook},
        {{L"public: virtual long __cdecl CNavBar::SetNavigationState(unsigned long)"}, &RibbonWork::navStateOriginal, RibbonWork::NavStateHook},
    };
    // shell32.dll
    const WindhawkUtils::SYMBOL_HOOK shellSymbols[] = {
        {{L"private: void __cdecl CDUIViewFrame::_RedrawFrame(void)"}, &redrawFrameOriginal, RedrawFrameHook},
        {{L"private: long __cdecl CDUISizerElement::_Render(void)"}, &renderSizerOriginal, RenderSizerHook},
    };
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.onlineCacheUrl = L"";
    bool frameSymbolsReady = frame && WindhawkUtils::HookSymbols(frame, frameSymbols, ARRAYSIZE(frameSymbols), &options);
    if (!frameSymbolsReady) {
        Wh_Log(L"Required ExplorerFrame symbols unavailable");
        return FALSE;
    }
    bool shellSymbolsReady = shell && WindhawkUtils::HookSymbols(shell, shellSymbols, ARRAYSIZE(shellSymbols), &options);
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
        createActionOriginal = reinterpret_cast<CreateAction>(GetProcAddress(duser, "CreateAction"));
        batches = Hook(createActionOriginal, CreateActionHook);
    }
    Wh_Log(L"Ribbon preserved. Hooks: batching=%d, activation paint=%d, layout paint=%d",
           batches, paint, layoutPaint);
    return TRUE;
}
BOOL Wh_ModInit() {
    if (InitializeMod()) return TRUE;
    // Windhawk rolls back hooks on failure, but doesn't call Wh_ModUninit.
    FileTransition::Uninit(); RibbonWork::Uninit();
    ReleaseModules();
#ifdef EFN_DIAGNOSTICS
    EventUnregister(diagnosticProvider);
#endif
    return FALSE;
}
void Wh_ModBeforeUninit() {
    FileTransition::lifetime.Stop();
    RibbonWork::lifetime.Stop();
    FileTransition::BeforeUninit(); RibbonWork::BeforeUninit();
}
void Wh_ModUninit() {
    FileTransition::Uninit(); RibbonWork::Uninit();
    ReleaseModules();
#ifdef EFN_DIAGNOSTICS
    EventUnregister(diagnosticProvider);
#endif
}
#endif // EXPLORER_NAVIGATION_POLICY_TEST
