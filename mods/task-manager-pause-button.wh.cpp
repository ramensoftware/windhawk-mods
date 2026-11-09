// ==WindhawkMod==
// @id              task-manager-pause-button
// @name            Task Manager Pause Button
// @description     Add a native Pause/Resume button to the Processes and Details pages of Windows 11 Task Manager.
// @version         1.0.0
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/task-manager-pause-button
// @include         Taskmgr.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols
// @license         Apache-2.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Pause Button

Add a native **Pause / Resume** button next to **Run new task** in the
Windows 11 Task Manager. The button uses the same style as other Task Manager
commands.

![Task Manager Pause Button](https://raw.githubusercontent.com/luizbizzio/task-manager-pause-button/main/assets/task-manager-pause-button-cover.png)

## How to use

1. Open Task Manager.
2. Go to **Processes** or **Details**.
3. Click **Pause** to stop the list from updating.
4. Click **Resume** to start updating the list again.

The Pause / Resume state stays in sync when you switch between the two pages.
The mod does not pause apps or other Windows processes. It only affects the
Task Manager view.

## Limitations

- Uses Task Manager's built-in Ctrl-key refresh hold. It does not change the
  **Real time update speed** setting.
- Some Ctrl-based actions inside Task Manager may act differently while
  the button is set to **Pause**.
- Currently detects the command bar in English and Portuguese.
- Uses internal Windows XAML interfaces. A Windows update may change the
  Task Manager interface and require an update to this mod.
- Does not change the Windows Registry or install a separate program.

## Troubleshooting

If the button does not appear, make sure the mod is enabled, close and reopen
Task Manager, and open **Processes** or **Details**. Check the Windhawk log for
XAML-related errors if the problem continues.

## Project

Find the source code, report bugs, or suggest changes on
[GitHub](https://github.com/luizbizzio/task-manager-pause-button).

Author: **Luiz Bizzio**. Licensed under **Apache 2.0**.
*/
// ==/WindhawkModReadme==

// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Luiz Bizzio
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Uses Windows.UI.Xaml visual-tree diagnostics (an internal Windows interface).
// No registry writes, no settings.json writes, no separate overlay window.
// Task Manager's private XAML tree can change between Windows versions.
// The Pause button requests the Task Manager Ctrl-key refresh hold.
// It does NOT change the native Real time update speed setting, and does
// NOT suspend or pause any operating system processes.
// The internal refresh mechanism may differ between Windows builds.

#include <windows.h>
#include <ocidl.h>
#include <xamlom.h>
// The system XAML WinRT projection declares a GetCurrentTime method.
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
#include <winrt/Windows.UI.Core.h>
#include <atomic>
#include <vector>
#include <mutex>
#include <algorithm>
#include <cwchar>

namespace mux = winrt::Windows::UI::Xaml;
namespace muxc = winrt::Windows::UI::Xaml::Controls;
namespace muxm = winrt::Windows::UI::Xaml::Media;
namespace wuc = winrt::Windows::UI::Core;
namespace wf = winrt::Windows::Foundation;

namespace {
constexpr GUID kTapClsid = {0xED8B42B2, 0x71A1, 0x4B8D,
                            {0xB7, 0x31, 0xF4, 0x1B, 0xAC, 0x6A, 0x41, 0x92}};
constexpr wchar_t kPauseButtonName[] = L"WindhawkTaskManagerPauseButton";
std::atomic<bool> g_stopping{false};
std::atomic<bool> g_paused{false};

using GetKeyState_t = SHORT(WINAPI*)(int);
using GetAsyncKeyState_t = SHORT(WINAPI*)(int);
using GetKeyboardState_t = BOOL(WINAPI*)(PBYTE);
GetKeyState_t g_originalGetKeyState = nullptr;
GetAsyncKeyState_t g_originalGetAsyncKeyState = nullptr;
GetKeyboardState_t g_originalGetKeyboardState = nullptr;

bool IsCtrl(int vk) {
    return vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL;
}

SHORT WINAPI GetKeyState_Hook(int vk) {
    SHORT result = g_originalGetKeyState(vk);
    if (!g_stopping.load() && g_paused.load() && IsCtrl(vk))
        result = static_cast<SHORT>(result | 0x8000);
    return result;
}

SHORT WINAPI GetAsyncKeyState_Hook(int vk) {
    SHORT result = g_originalGetAsyncKeyState(vk);
    if (!g_stopping.load() && g_paused.load() && IsCtrl(vk))
        result = static_cast<SHORT>(result | 0x8000);
    return result;
}

BOOL WINAPI GetKeyboardState_Hook(PBYTE state) {
    BOOL result = g_originalGetKeyboardState(state);
    if (result && state && !g_stopping.load() && g_paused.load()) {
        state[VK_CONTROL] |= 0x80;
        state[VK_LCONTROL] |= 0x80;
    }
    return result;
}

struct Entry {
    winrt::weak_ref<muxc::CommandBar> bar;
    winrt::weak_ref<muxc::AppBarButton> button;
    wuc::CoreDispatcher dispatcher{nullptr};
    winrt::event_token token{};
    DWORD threadId{};
};
std::mutex g_entriesMutex;
std::vector<Entry> g_entries;

// WinRT XAML elements must be updated on the UI thread that owns them.
// Entries contain weak references so old pages can be collected normally.
void UpdateButton(muxc::AppBarButton const& button) {
    const bool paused = g_paused.load();
    const wchar_t* label = paused ? L"Resume" : L"Pause";
    if (button.Label() == label) return;
    button.Label(label);
    button.Icon(muxc::SymbolIcon(paused ? muxc::Symbol::Play
                                        : muxc::Symbol::Pause));
}

void PruneDeadEntriesOnCurrentThread() {
    const DWORD threadId = GetCurrentThreadId();
    std::lock_guard<std::mutex> lock(g_entriesMutex);
    g_entries.erase(std::remove_if(g_entries.begin(), g_entries.end(),
        [threadId](Entry const& entry) {
            if (entry.threadId != threadId) return false;
            try {
                return !entry.button.get() || !entry.bar.get();
            } catch (...) {
                return true;
            }
        }), g_entries.end());
}

void SyncAllButtons() {
    std::vector<Entry> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        snapshot = g_entries;
    }
    const DWORD currentThreadId = GetCurrentThreadId();
    for (auto const& entry : snapshot) {
        if (entry.threadId == currentThreadId) {
            try {
                if (auto button = entry.button.get()) UpdateButton(button);
            } catch (...) {
                // An old page may have been destroyed between snapshots.
            }
        } else if (entry.dispatcher) {
            try {
                auto weakButton = entry.button;
                entry.dispatcher.RunAsync(wuc::CoreDispatcherPriority::Normal,
                    [weakButton] {
                        if (g_stopping.load()) return;
                        try {
                            if (auto button = weakButton.get()) UpdateButton(button);
                        } catch (...) {
                            // The UI element no longer exists.
                        }
                    });
            } catch (...) {
                // Dispatcher is shutting down.
            }
        }
    }
}

void TogglePause() {
    bool expected = g_paused.load();
    while (!g_paused.compare_exchange_weak(expected, !expected)) {
    }
    SyncAllButtons();
}

void RemoveEntry(Entry const& entry) {
    try {
        auto button = entry.button.get();
        if (!button) return;
        button.Click(entry.token);
        auto bar = entry.bar.get();
        if (!bar) return;
        auto commands = bar.PrimaryCommands();
        for (uint32_t i = 0; i < commands.Size(); ++i) {
            auto item = commands.GetAt(i).try_as<muxc::AppBarButton>();
            if (item == button) {
                commands.RemoveAt(i);
                break;
            }
        }
    } catch (...) {
        static std::atomic<unsigned> failures{0};
        if (failures.fetch_add(1) < 3)
            Wh_Log(L"XAML Pause button cleanup failed: 0x%08X", winrt::to_hresult());
    }
}

void InsertPauseButton(muxc::CommandBar const& bar) {
    if (g_stopping.load()) return;
    auto commands = bar.PrimaryCommands();
    bool hasRun = false;
    bool hasEnd = false;
    uint32_t insertion = 0;
    for (uint32_t i = 0; i < commands.Size(); ++i) {
        auto current = commands.GetAt(i).try_as<muxc::AppBarButton>();
        if (!current) continue;
        if (current.Name() == kPauseButtonName) {
            // This bar already contains our button. A visual-tree notification
            // can run many times for the same command bar; do not insert twice.
            UpdateButton(current);
            return;
        }
        const auto label = current.Label();
        if (label == L"Run new task" || label == L"Executar nova tarefa") {
            hasRun = true;
            insertion = i + 1;
        }
        if (label == L"End task" || label == L"Finalizar tarefa") hasEnd = true;
    }
    // The Processes and Details bars have both commands. Ignore other pages.
    if (!hasRun || !hasEnd) return;

    PruneDeadEntriesOnCurrentThread();

    auto button = muxc::AppBarButton();
    button.Name(kPauseButtonName);
    UpdateButton(button);
    auto clickToken = button.Click([](auto const&, auto const&) {
        if (!g_stopping.load()) TogglePause();
    });
    // Insert directly after Run new task. No Win32 overlay or registry edits.
    commands.InsertAt(insertion, button);
    Entry entry;
    entry.bar = winrt::make_weak(bar);
    entry.button = winrt::make_weak(button);
    entry.token = clickToken;
    entry.dispatcher = button.Dispatcher();
    entry.threadId = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        g_entries.push_back(std::move(entry));
    }
    static std::atomic<bool> firstInsertion{false};
    if (!firstInsertion.exchange(true))
        Wh_Log(L"Native XAML Pause/Resume button ready in Task Manager");
}

class TreeWatcher : public winrt::implements<TreeWatcher, IVisualTreeServiceCallback2,
                                               winrt::non_agile> {
    winrt::com_ptr<IXamlDiagnostics> diagnostics;
public:
    explicit TreeWatcher(winrt::com_ptr<IXamlDiagnostics> const& diag)
        : diagnostics(diag) {}

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation,
                                                 VisualElement element,
                                                 VisualMutationType mutation) noexcept override {
        if (mutation != Add || g_stopping.load() || !element.Type) return S_OK;
        // CommandBar may be announced before it receives its built-in buttons.
        // Also look when an AppBarButton enters the visual tree.
        if (!wcsstr(element.Type, L"CommandBar") &&
            !wcsstr(element.Type, L"AppBarButton")) return S_OK;
        try {
            winrt::com_ptr<::IInspectable> native;
            if (FAILED(diagnostics->GetIInspectableFromHandle(element.Handle, native.put())) ||
                !native) return S_OK;
            wf::IInspectable object{nullptr};
            // IInspectable projection owns a separate AddRef.
            // Projected WinRT objects do not have a .attach() member.
            winrt::copy_from_abi(object, native.get());
            if (auto bar = object.try_as<muxc::CommandBar>()) {
                InsertPauseButton(bar);
            } else if (auto child = object.try_as<mux::DependencyObject>()) {
                for (int depth = 0; depth < 32 && child; ++depth) {
                    if (auto bar = child.try_as<muxc::CommandBar>()) {
                        InsertPauseButton(bar);
                        break;
                    }
                    child = muxm::VisualTreeHelper::GetParent(child);
                }
            }
        } catch (...) {
            static std::atomic<unsigned> failures{0};
            if (failures.fetch_add(1) < 3)
                Wh_Log(L"XAML visual-tree callback failed: 0x%08X", winrt::to_hresult());
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle,
                                                     VisualElementState,
                                                     LPCWSTR) noexcept override {
        return S_OK;
    }
};

class Tap : public winrt::implements<Tap, IObjectWithSite, winrt::non_agile> {
    winrt::com_ptr<IVisualTreeService3> service;
    winrt::com_ptr<TreeWatcher> watcher;
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) noexcept override {
        try {
            if (service && watcher) {
                service->UnadviseVisualTreeChange(watcher.get());
                watcher = nullptr;
                service = nullptr;
            }
            if (!site || g_stopping.load()) return S_OK;
            if (FAILED(site->QueryInterface(__uuidof(IVisualTreeService3),
                                           service.put_void()))) return E_NOINTERFACE;
            watcher = winrt::make_self<TreeWatcher>(service.as<IXamlDiagnostics>());
            struct Work {
                winrt::com_ptr<IVisualTreeService3> service;
                winrt::com_ptr<TreeWatcher> watcher;
            };
            auto work = new Work{service, watcher};
            HANDLE thread = CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
                auto work = static_cast<Work*>(param);
                HRESULT hr = work->service->AdviseVisualTreeChange(work->watcher.get());
                if (FAILED(hr))
                    Wh_Log(L"AdviseVisualTreeChange failed: 0x%08X", hr);
                delete work;
                return 0;
            }, work, 0, nullptr);
            if (!thread) {
                delete work;
                return HRESULT_FROM_WIN32(GetLastError());
            }
            CloseHandle(thread);
            return S_OK;
        } catch (...) { return winrt::to_hresult(); }
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void** result) noexcept override {
        if (!result) return E_POINTER;
        *result = nullptr;
        return service ? service->QueryInterface(iid, result) : E_FAIL;
    }
};

class TapFactory : public winrt::implements<TapFactory, IClassFactory,
                                              winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID iid,
                                             void** result) noexcept override {
        if (!result) return E_POINTER;
        *result = nullptr;
        if (outer) return CLASS_E_NOAGGREGATION;
        try { return winrt::make<Tap>().as(iid, result); }
        catch (...) { return winrt::to_hresult(); }
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};
} // namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
extern "C" __declspec(dllexport) HRESULT WINAPI DllGetClassObject(
    REFCLSID clsid, REFIID iid, void** result) {
    if (!result) return E_POINTER;
    *result = nullptr;
    if (clsid != kTapClsid) return CLASS_E_CLASSNOTAVAILABLE;
    try { return winrt::make<TapFactory>().as(iid, result); }
    catch (...) { return winrt::to_hresult(); }
}

extern "C" __declspec(dllexport) HRESULT WINAPI DllCanUnloadNow() {
    return S_FALSE;
}
#pragma clang diagnostic pop

namespace {
using InitDiagnostics_t = HRESULT(WINAPI*)(LPCWSTR, DWORD, LPCWSTR,
                                           LPCWSTR, CLSID, LPCWSTR);

bool StartDiagnostics() {
    HMODULE xaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
    if (!xaml) {
        Wh_Log(L"System XAML is not loaded; skipping native system-XAML injection");
        return false;
    }
    auto initialize = reinterpret_cast<InitDiagnostics_t>(
        GetProcAddress(xaml, "InitializeXamlDiagnosticsEx"));
    if (!initialize) {
        Wh_Log(L"Windows.UI.Xaml.dll has no InitializeXamlDiagnosticsEx export");
        return false;
    }

    HMODULE self = nullptr;
    if (!GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&DllGetClassObject), &self)) {
        Wh_Log(L"Cannot pin the XAML tap module: %lu", GetLastError());
        return false;
    }
    // XAML diagnostics may keep native callback pointers for the lifetime of
    // Taskmgr.exe. Pinning avoids use-after-free after mod recompilation.
    WCHAR tapPath[MAX_PATH]{};
    if (!GetModuleFileNameW(self, tapPath, ARRAYSIZE(tapPath))) return false;

    HRESULT hr = E_FAIL;
    for (int i = 1; i <= 512 && !g_stopping.load(); ++i) {
        WCHAR endpoint[80]{};
        swprintf(endpoint, ARRAYSIZE(endpoint), L"VisualDiagConnection%d", i);
        // For system XAML, use a null diagnostics DLL path, as in the
        // original Windhawk XAML visual tree monitoring reference.
        hr = initialize(endpoint, GetCurrentProcessId(), nullptr,
                        tapPath, kTapClsid, nullptr);
        if (SUCCEEDED(hr)) {
            Wh_Log(L"System XAML diagnostics connected: %s", endpoint);
            return true;
        }
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) break;
    }
    Wh_Log(L"System XAML diagnostics connection failed: 0x%08X", hr);
    return false;
}

DWORD WINAPI InitThread(LPVOID) {
    // Task Manager can load XAML after injection, so retry briefly.
    for (int i = 0; i < 60 && !g_stopping.load(); ++i) {
        if (GetModuleHandleW(L"Windows.UI.Xaml.dll")) {
            StartDiagnostics();
            return 0;
        }
        Sleep(250);
    }
    if (!g_stopping.load()) {
        Wh_Log(L"No Windows.UI.Xaml.dll after 15s; system-XAML integration unavailable on this Task Manager build");
    }
    return 0;
}
} // namespace

BOOL Wh_ModInit() {
    g_stopping = false;
    g_paused = false;
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(GetKeyState),
                            reinterpret_cast<void*>(GetKeyState_Hook),
                            reinterpret_cast<void**>(&g_originalGetKeyState))) return FALSE;
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(GetAsyncKeyState),
                            reinterpret_cast<void*>(GetAsyncKeyState_Hook),
                            reinterpret_cast<void**>(&g_originalGetAsyncKeyState))) return FALSE;
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(GetKeyboardState),
                            reinterpret_cast<void*>(GetKeyboardState_Hook),
                            reinterpret_cast<void**>(&g_originalGetKeyboardState))) return FALSE;
    HANDLE thread = CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
    if (thread) CloseHandle(thread);
    return thread != nullptr;
}

void Wh_ModUninit() {
    g_stopping = true;
    g_paused = false;
    std::vector<Entry> entries;
    {
        std::lock_guard<std::mutex> lock(g_entriesMutex);
        entries.swap(g_entries);
    }
    for (auto const& entry : entries) {
        if (entry.threadId == GetCurrentThreadId()) {
            RemoveEntry(entry);
        } else if (entry.dispatcher) {
            try {
                // The tap DLL remains pinned while a diagnostics watcher exists.
                // Do not block the unload thread waiting for the XAML UI loop.
                entry.dispatcher.RunAsync(wuc::CoreDispatcherPriority::Normal,
                    [entry] { RemoveEntry(entry); });
            } catch (...) {
                // The Task Manager window may already be closing.
            }
        }
    }
}
