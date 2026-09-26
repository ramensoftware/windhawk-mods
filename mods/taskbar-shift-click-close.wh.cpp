// ==WindhawkMod==
// @id taskbar-shift-click-close
// @name Taskbar Shift Click Close
// @description Close a running taskbar application with Shift + left click
// @version 1.1.0
// @author Arkadiusz
// @github https://github.com/Artllex
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Shift Click Close 1.1.0

Hold Shift and left-click a running application on the Windows 11 taskbar to
send the standard close command to its matching top-level window.
Hold Ctrl+Shift and left-click to close all matching windows in the group.

Normal clicks, right-click, dragging, grouping, thumbnails and hover remain
native. The mod creates no visual elements, animations, popups or timers.

This changes the standard Windows Shift+click action, which normally opens a
new application instance. A proposal to integrate configurable Shift+click
actions into the existing middle-click mod is tracked in
[windhawk-mods issue #5586](https://github.com/ramensoftware/windhawk-mods/issues/5586).

Written from scratch. No implementation copied from GPL mods.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <atomic>
#include <algorithm>
#include <string>
#include <vector>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace {
std::atomic<bool> stopping{false};
std::atomic<bool> hooked{false};
using PointerAbi = HRESULT(WINAPI*)(void*, void*);
PointerAbi pressedOriginal = nullptr;
using RunningAbi = HRESULT(WINAPI*)(void*, bool*);
RunningAbi runningOriginal = nullptr;
decltype(&LoadLibraryExW) loadOriginal = nullptr;

bool ContainsInsensitive(PCWSTR text, const std::wstring& needle) {
    if (!text || needle.empty()) return false;
    for (const wchar_t* p = text; *p; ++p) {
        if (_wcsnicmp(p, needle.c_str(), needle.size()) == 0) return true;
    }
    return false;
}

bool WindowMatches(HWND hwnd, const std::wstring& name) {
    WCHAR title[512]{};
    if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) > 0 &&
        ContainsInsensitive(title, name)) return true;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE process = pid ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                       FALSE, pid) : nullptr;
    if (!process) return false;
    WCHAR path[32768]{};
    DWORD length = ARRAYSIZE(path);
    bool found = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    if (!found) return false;
    const wchar_t* fileName = wcsrchr(path, L'\\');
    return ContainsInsensitive(fileName ? fileName + 1 : path, name);
}

std::vector<HWND> FindWindowsForTaskName(const std::wstring& taskName) {
    if (taskName.empty()) return {};
    const std::wstring name = taskName.substr(0, taskName.find(L" — "));
    struct Search {
        const std::wstring& name;
        std::vector<HWND> results;
    } search{name};
    EnumWindows([](HWND hwnd, LPARAM data) -> BOOL {
        auto& search = *reinterpret_cast<Search*>(data);
        if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER)) return TRUE;
        if (WindowMatches(hwnd, search.name)) {
            search.results.push_back(hwnd);
            return TRUE;
        }
        EnumChildWindows(hwnd, [](HWND child, LPARAM data) -> BOOL {
            auto& search = *reinterpret_cast<Search*>(data);
            if (WindowMatches(child, search.name)) {
                HWND root = GetAncestor(child, GA_ROOT);
                if (root && std::find(search.results.begin(), search.results.end(),
                                      root) == search.results.end()) {
                    search.results.push_back(root);
                }
                return FALSE;
            }
            return TRUE;
        }, data);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&search));
    return search.results;
}

FrameworkElement ElementFromAbi(void* object) {
    FrameworkElement result{nullptr};
    check_hresult(static_cast<::IUnknown*>(object)->QueryInterface(
        guid_of<FrameworkElement>(), put_abi(result)));
    return result;
}

bool IsRunning(const FrameworkElement& button) {
    auto identity = button.as<Windows::Foundation::IUnknown>();
    bool running = false;
    return SUCCEEDED(runningOriginal(get_abi(identity), &running)) && running;
}

HRESULT WINAPI PointerPressed(void* object, void* eventAbi) {
    if (!stopping && (GetKeyState(VK_SHIFT) & 0x8000)) {
        try {
            auto button = ElementFromAbi(object);
            if (get_class_name(button) == L"Taskbar.TaskListButton" &&
                button.IsLoaded() && IsRunning(button)) {
                Input::PointerRoutedEventArgs args{nullptr};
                copy_from_abi(args, eventAbi);
                if (args.GetCurrentPoint(button).Properties().IsLeftButtonPressed()) {
                    args.Handled(true);
                    std::wstring taskName =
                        Automation::AutomationProperties::GetName(button).c_str();
                    Wh_Log(L"Shift+click received; task name: %s", taskName.c_str());
                    auto targets = FindWindowsForTaskName(taskName);
                    const bool closeAll = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                    if (!targets.empty()) {
                        const size_t count = closeAll ? targets.size() : 1;
                        for (size_t i = 0; i < count; ++i) {
                            HWND root = GetAncestor(targets[i], GA_ROOT);
                            if (!root) root = targets[i];
                            PostMessageW(root, WM_SYSCOMMAND, SC_CLOSE, 0);
                            Wh_Log(L"SC_CLOSE sent to root %p", root);
                        }
                        Wh_Log(L"Close mode: %s; windows: %zu",
                               closeAll ? L"all" : L"single", count);
                    } else {
                        Wh_Log(L"No matching HWND");
                    }
                    return S_OK;
                }
            }
        } catch (...) {
            Wh_Log(L"Shift+click handling failed; using native action");
        }
    }
    return pressedOriginal(object, eventAbi);
}

bool HookTaskbar(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *)",
          L"public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void * __ptr64) __ptr64"},
         &pressedOriginal, PointerPressed},
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *)",
          L"public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool * __ptr64) __ptr64"},
         &runningOriginal},
    };
    bool ok = WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                         ARRAYSIZE(taskbarViewHooks));
    Wh_Log(L"TaskListButton Shift+click hooks: %s", ok ? L"ready" : L"unavailable");
    return ok;
}

HMODULE WINAPI Load(PCWSTR name, HANDLE file, DWORD flags) {
    HMODULE result = loadOriginal(name, file, flags);
    if (result && !stopping && !hooked &&
        result == GetModuleHandleW(L"Taskbar.View.dll")) {
        bool expected = false;
        if (hooked.compare_exchange_strong(expected, true) && HookTaskbar(result))
            Wh_ApplyHookOperations();
    }
    return result;
}
}

BOOL Wh_ModInit() {
    if (auto module = GetModuleHandleW(L"Taskbar.View.dll")) {
        hooked = true;
        return HookTaskbar(module);
    }
    Wh_Log(L"Waiting for Taskbar.View.dll");
    return Wh_SetFunctionHook(reinterpret_cast<void*>(&LoadLibraryExW),
                              reinterpret_cast<void*>(&Load),
                              reinterpret_cast<void**>(&loadOriginal));
}

void Wh_ModAfterInit() {
    if (auto module = GetModuleHandleW(L"Taskbar.View.dll")) {
        bool expected = false;
        if (hooked.compare_exchange_strong(expected, true) && HookTaskbar(module))
            Wh_ApplyHookOperations();
    }
}

void Wh_ModBeforeUninit() { stopping = true; }
void Wh_ModUninit() {}
