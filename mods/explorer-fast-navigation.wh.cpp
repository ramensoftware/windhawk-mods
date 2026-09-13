// ==WindhawkMod==
// @id              explorer-fast-navigation
// @name            Explorer Fast Navigation
// @description     Improves explorer navigation latency
// @version         0.8.0
// @author          GPT6-Astra w/ Vivy
// @github          https://github.com/enginelesscc
// @twitter         https://x.com/VivyVCCS
// @include         explorer.exe
// @architecture    amd64
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
#include <atomic>
#include <algorithm>
#include <mutex>
#include <new>
#include <vector>
#include <functional>
#include <deque>

#ifdef EFN_DIAGNOSTICS
#include <evntprov.h>
REGHANDLE diagnosticProvider;
void Trace(const wchar_t* message) { EventWriteString(diagnosticProvider, 4, 1, message); }
#else
void Trace(const wchar_t*) {}
#endif

namespace RibbonWork {
bool Initialize(HMODULE, const WH_HOOK_SYMBOLS_OPTIONS*);
void BeforeUninit();
void Uninit();
void Flush();
bool DeferInvoke(void*, DISPID);
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
    bool pending = false, enumerated = false, ready = false, finishing = false;
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
std::atomic<bool> stopping;
std::mutex windowsMutex;
std::vector<HWND> windows;
HINSTANCE instance;

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
void QueuePresent(State* state) {
    if (state && (state->ready || (state->enumerated && !state->pending)))
        PostMessageW(state->overlay, PresentMessage, state->generation, 0);
}
void Finish(State* state) {
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
        if (state->generation == generation) { DestroyWindow(overlay); Trace(L"EFN present_done"); }
        else state->finishing = false;
    }
}
LRESULT CALLBACK RootSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR data) {
    auto state = reinterpret_cast<State*>(data);
    if (msg == WM_SIZE || msg == WM_DPICHANGED || msg == WM_NCDESTROY)
        DestroyWindow(state->overlay);
    return DefSubclassProc(hwnd, msg, wp, lp);
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto state = reinterpret_cast<State*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (!state) return DefWindowProcW(hwnd, msg, wp, lp);
    switch (msg) {
    case PresentMessage:
        if (wp == state->generation &&
                (state->ready || (state->enumerated && !state->pending))) Finish(state);
        return 0;
    case WM_TIMER:
        if (wp == 1) {
            // Ignore stale timer messages left queued by an earlier navigation.
            UINT remaining = NavigationPolicy::RemainingCoverMs(false, state->started, GetTickCount64());
            if (!remaining) Finish(state);
            else if (!SetTimer(hwnd, 1, remaining, nullptr)) Finish(state);
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
        {
            std::lock_guard lock(windowsMutex);
            std::erase(windows, hwnd);
        }
        SelectObject(state->dc, state->oldBitmap);
        DeleteObject(state->bitmap);
        DeleteDC(state->dc);
        delete state;
        break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
void Begin(void* itemsView) {
    if (stopping) return;
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
        active->pending = active->enumerated = active->ready = false;
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
    {
        std::lock_guard lock(windowsMutex);
        windows.push_back(overlay);
    }
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
    UINT remaining = NavigationPolicy::RemainingCoverMs(navigation.remote,
        navigation.started ? navigation.started : GetTickCount64(), GetTickCount64());
    if (!remaining) {
        unblockRedraw(self);
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
        active->pending = active->enumerated = active->ready = false;
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
        if (active) Finish(active);
    }
    return result;
}

HRESULT __fastcall ResetHook(void* self, void* item, void* collection) {
    Begin(self);
    HRESULT result = resetOriginal(self, item, collection);
    if (FAILED(result) && active && active->itemsView == self) Finish(active);
    return result;
}
HRESULT __fastcall EnsureHook(void* self) {
    HRESULT result = ensureOriginal(self);
    if (SUCCEEDED(result) && active && active->itemsView == self) {
        active->pending = true;
        active->ready = false;
    }
    return result;
}
bool __fastcall DeleteHook(void* self) {
    bool result = deleteOriginal(self);
    if (result && processingBatch == self) RibbonWork::ViewReady();
    if (result && processingBatch == self && active && active->itemsView == self) {
        active->pending = false;
        active->ready = true;
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
    if (RibbonWork::DeferInvoke(self, id)) {
        if (result) *result = {};
        hr = S_OK;
    } else {
        hr = invokeOriginal(self, id, iid, locale, flags, args, result, exception, argError);
    }
    if (id == DISPID_FILELISTENUMDONE) EnumerationDone();
    return hr;
}
bool Initialize(HMODULE frame, const WH_HOOK_SYMBOLS_OPTIONS* options) {
    const WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"?_NavigateToPidl@CShellBrowser@@AEAAJPEBU_ITEMIDLIST_ABSOLUTE@@KK@Z"}, &navigateOriginal, NavigateHook},
        {{L"?BlockRedrawWithTimeout@UIItemsView@@QEAAXM@Z"}, &blockOriginal, BlockHook},
        {{L"?UnblockRedraw@UIItemsView@@QEAAXXZ"}, &unblockRedraw},
        {{L"?_ResetRoot@UIItemsView@@AEAAJPEAUIItem@@PEAUIItemCollection@@@Z"}, &resetOriginal, ResetHook},
        {{L"?_EnsureBatching@UIItemsView@@AEAAJXZ"}, &ensureOriginal, EnsureHook},
        {{L"?_OnBatchTimer@UIItemsView@@AEAAXPEAUGMA_ACTIONINFO@@@Z"}, &batchOriginal, BatchHook},
        {{L"?DeleteBatchTimer@UIItemsView@@AEAA_NXZ"}, &deleteOriginal, DeleteHook},
        {{L"?Invoke@CExplorerRibbon@@UEAAJJAEBU_GUID@@KGPEAUtagDISPPARAMS@@PEAUtagVARIANT@@PEAUtagEXCEPINFO@@PEAI@Z"}, &invokeOriginal, InvokeHook},
    };
    if (!WindhawkUtils::HookSymbols(frame, symbols, ARRAYSIZE(symbols), options)) return false;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(WindowProc), &instance);
    WNDCLASSW cls{};
    cls.lpfnWndProc = WindowProc; cls.hInstance = instance; cls.lpszClassName = WindowClass;
    return RegisterClassW(&cls) != 0;
}
void BeforeUninit() {
    stopping = true;
    std::vector<HWND> copy;
    { std::lock_guard lock(windowsMutex); copy = windows; }
    for (HWND hwnd : copy) SendMessageW(hwnd, WM_CLOSE, 0, 0);
}
void Uninit() { UnregisterClassW(WindowClass, instance); }
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
std::atomic<bool> stopping;
std::mutex mutex;
std::vector<HWND> windows;
HINSTANCE instance;
using Notification = HRESULT(__fastcall*)(void*);
using Destroy = HRESULT(__fastcall*)(void*, BOOL);
using NavState = HRESULT(__fastcall*)(void*, ULONG);
Notification navigatedOriginal, changedOriginal;
Destroy destroyOriginal;
NavState navStateOriginal;

void Flush() {
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
    if (msg == WM_DESTROY) {
        auto q = reinterpret_cast<Queue*>(data);
        q->closing = true;
        Flush(); // Drop pending presentation work; release COM refs on this thread.
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}
LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TIMER && wp == 1) {
        // WM_TIMER has lower priority than paint. A bounded fallback also handles
        // empty, slow, remote and failed navigations with no batching callback.
        if ((ready && !FileTransition::active) || GetTickCount64() - started >= 200)
            Flush();
        return 0;
    }
    if (msg == WM_CLOSE) {
        if (queue && queue->draining) { queue->closing = true; return 0; }
        Flush(); DestroyWindow(hwnd); return 0;
    }
    if (msg == WM_NCDESTROY) {
        Queue* q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (q && q->inputHook) UnhookWindowsHookEx(q->inputHook);
        if (q && q->root) WindhawkUtils::RemoveWindowSubclassFromAnyThread(q->root, OwnerSubclass);
        if (queue == q) queue = nullptr;
        { std::lock_guard lock(mutex); std::erase(windows, hwnd); }
        delete q;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
bool Enqueue(IUnknown* object, std::function<void()> run) {
    if (stopping || (queue && queue->draining)) return false;
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
        { std::lock_guard lock(mutex); windows.push_back(q->hwnd); }
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
bool DeferInvoke(void* self, DISPID id) {
    // These native branches use no DISPPARAMS and always return S_OK. All
    // argument-bearing events (especially mouse-wheel handling) stay synchronous.
    if (!Pending() || (id != 200 && id != 201 && id != 205 && id != 207 &&
            id != 212 && id != 215 && id != 220)) return false;
    return Enqueue(reinterpret_cast<IUnknown*>(self), [self, id] {
        DISPPARAMS args{};
        const GUID iid{};
        FileTransition::invokeOriginal(self, id, iid, 0, DISPATCH_METHOD,
            &args, nullptr, nullptr, nullptr);
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
bool Initialize(HMODULE frame, const WH_HOOK_SYMBOLS_OPTIONS* options) {
    const WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"?OnBrowserNavigated@CExplorerRibbon@@UEAAJXZ"}, &navigatedOriginal, NavigatedHook},
        {{L"?OnShellViewChanged@CExplorerRibbon@@UEAAJXZ"}, &changedOriginal, ChangedHook},
        {{L"?DestroyRibbonUI@CExplorerRibbon@@UEAAJH@Z"}, &destroyOriginal, DestroyHook},
        {{L"?SetNavigationState@CNavBar@@UEAAJK@Z"}, &navStateOriginal, NavStateHook},
    };
    if (!WindhawkUtils::HookSymbols(frame, symbols, ARRAYSIZE(symbols), options)) return false;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(Proc), &instance);
    WNDCLASSW cls{};
    cls.lpfnWndProc = Proc; cls.hInstance = instance; cls.lpszClassName = ClassName;
    return RegisterClassW(&cls) != 0;
}
void BeforeUninit() {
    stopping = true;
    std::vector<HWND> copy;
    { std::lock_guard lock(mutex); copy = windows; }
    for (HWND hwnd : copy) SendMessageW(hwnd, WM_CLOSE, 0, 0);
}
void Uninit() { UnregisterClassW(ClassName, instance); }
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
    if (invalidation && Pending() && RibbonWork::Enqueue(
            reinterpret_cast<IUnknown*>(self), [self, id] {
                Trace(L"EFN xaml_event_run");
                DISPPARAMS empty{};
                const GUID iid{};
                invokeOriginal(self, id, iid, 0, DISPATCH_METHOD, &empty, nullptr, nullptr, nullptr);
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
    const WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{L"?OnBrowserNavigated@CommandBarViewAdapter@@UEAAJXZ"}, &navigatedOriginal, NavigatedHook, true},
        {{L"?OnShellViewChanged@CommandBarViewAdapter@@UEAAJXZ"}, &changedOriginal, ChangedHook, true},
        {{L"?OnCommandStateInvalidated@CommandBarViewAdapter@@UEAAJXZ"}, &invalidatedOriginal, InvalidatedHook, true},
        {{L"?SetNavigationState@CommandBarViewAdapter@@UEAAJK@Z"}, &stateOriginal, StateHook, true},
        {{L"?InvalidateCommands@CommandBarViewAdapter@@UEAAJW4COMMAND_SET@@@Z"}, &invalidateOriginal, InvalidateHook, true},
        {{L"?Invoke@CommandBarViewAdapter@@UEAAJJAEBU_GUID@@KGPEAUtagDISPPARAMS@@PEAUtagVARIANT@@PEAUtagEXCEPINFO@@PEAI@Z"}, &invokeOriginal, InvokeHook, true},
        {{L"?Cleanup@CommandBarViewAdapter@@QEAAJXZ"}, &cleanupOriginal, CleanupHook, true},
    };
    bool resolved = WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols), options);
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
void *redrawFrame, *renderSizer;
struct FunctionRange {
    ULONG_PTR begin = 0, end = 0;
    bool ContainsReturnAddress(ULONG_PTR caller) const {
        return caller > begin && caller <= end;
    }
    bool Resolve(void* function) {
        DWORD64 imageBase = 0;
        auto address = reinterpret_cast<DWORD64>(function);
        auto entry = address ? RtlLookupFunctionEntry(address, &imageBase, nullptr) : nullptr;
        if (!entry || imageBase + entry->BeginAddress != address ||
            entry->EndAddress <= entry->BeginAddress) return false;
        begin = address;
        end = imageBase + entry->EndAddress;
        return true;
    }
};
FunctionRange activationRange, sizerRange;
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
    auto caller = reinterpret_cast<ULONG_PTR>(__builtin_return_address(0));
    // The return address may equal the exclusive end of the calling function.
    return activationRange.ContainsReturnAddress(caller) && !FileTransition::NativeLoadingAllowed()
        ? TRUE : updateWindowOriginal(hwnd);
}
BOOL WINAPI RedrawWindowHook(HWND hwnd, const RECT* rect, HRGN region, UINT flags) {
    auto caller = reinterpret_cast<ULONG_PTR>(__builtin_return_address(0));
    if (sizerRange.ContainsReturnAddress(caller) && !FileTransition::NativeLoadingAllowed()) {
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

BOOL Wh_ModInit() {
#ifdef EFN_DIAGNOSTICS
    GUID provider{0xd1675027, 0xf8d0, 0x4c43, {0x9b, 0x6f, 0x4b, 0x39, 0x0c, 0xe6, 0x46, 0xed}};
    EventRegister(&provider, nullptr, nullptr, &diagnosticProvider);
#endif
    auto frame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto shell = GetModuleHandleW(L"shell32.dll");
    auto duser = GetModuleHandleW(L"DUser.dll");
    const WindhawkUtils::SYMBOL_HOOK frameSymbols[] = {
        {{L"?s_BatchTimerCallback@UIItemsView@@CAXPEAUGMA_ACTIONINFO@@@Z"}, &batchCallback, nullptr, true},
    };
    const WindhawkUtils::SYMBOL_HOOK shellSymbols[] = {
        {{L"?_RedrawFrame@CDUIViewFrame@@AEAAXXZ"}, &redrawFrame, nullptr, true},
        {{L"?_Render@CDUISizerElement@@AEAAJXZ"}, &renderSizer, nullptr, true},
    };
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    options.onlineCacheUrl = L"";
    bool frameSymbolsReady = frame && WindhawkUtils::HookSymbols(frame, frameSymbols, ARRAYSIZE(frameSymbols), &options);
    bool shellSymbolsReady = shell && WindhawkUtils::HookSymbols(shell, shellSymbols, ARRAYSIZE(shellSymbols), &options);
    if (!shellSymbolsReady || !activationRange.Resolve(redrawFrame) ||
        !sizerRange.Resolve(renderSizer)) {
        Wh_Log(L"Required activation/layout paint symbols or function metadata missing");
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
    if (!FileTransition::Initialize(frame, &options)) {
        Wh_Log(L"Required file-view transition hooks unavailable");
        return FALSE;
    }
    if (!RibbonWork::Initialize(frame, &options)) {
        FileTransition::Uninit();
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
void Wh_ModBeforeUninit() { FileTransition::BeforeUninit(); RibbonWork::BeforeUninit(); }
void Wh_ModUninit() {
    FileTransition::Uninit(); RibbonWork::Uninit();
    XamlWork::Uninit();
#ifdef EFN_DIAGNOSTICS
    EventUnregister(diagnosticProvider);
#endif
}
#endif // EXPLORER_NAVIGATION_POLICY_TEST
