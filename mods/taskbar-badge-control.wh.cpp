// ==WindhawkMod==
// @id              taskbar-badge-control
// @name            Taskbar Badge Control
// @description     Show or hide taskbar badges per application (Windows 11).
// @version         1.0.0
// @author          mobeigi
// @github          https://github.com/mobeigi
// @homepage        https://mobeigi.com/
// @license         GPL-3.0-only
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
// Based in part on m417z's GPL-3.0 taskbar-labels, taskbar-icon-size,
// taskbar-start-button-position, and taskbar-reorder-right-drag mods:
// https://github.com/ramensoftware/windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar Badge Control

This mod lets you show or hide taskbar badges per application, instead of
turning them all on or off through Windows. It's useful for apps that don't
offer their own setting to disable taskbar badges.

![Taskbar before and after selectively hiding application badges](https://i.imgur.com/x45aKkt.jpeg)

## Usage

Use the filtering mode to **show badges by default** or **hide badges by
default**, then populate **Applications** with the apps that should behave
differently.

Add process names (`process.exe`), full executable paths, or AppUserModelIDs
under **Applications**. Entries are case-insensitive exact matches; wildcards
are not supported.

## Required Windows setting

Enable **Show badges on taskbar apps** under **Settings → Personalization →
Taskbar → Taskbar behaviors**.

If this setting is disabled, Windows hides all taskbar badges regardless of the
mod's rules. The mod cannot show badges for allowed applications while the
global setting is off.

![Windows taskbar settings with Show badges on taskbar apps enabled](https://i.imgur.com/dBVwbWE.jpeg)

## Limits

- Only the native Windows 11 x64 taskbar is supported.
- Badges drawn into an application's base icon cannot be removed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: blocklist
  $name: Filtering mode
  $options:
  - blocklist: Show badges by default, except for applications listed below
  - allowlist: Hide badges by default, except for applications listed below
- applications: [""]
  $name: Applications
  $description: >-
    Case-insensitive exact matches against process names (process.exe),
    full executable paths, or AppUserModelIDs. Wildcards are not supported.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <inspectable.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

// The native taskbar's repeaters use WinUI 2 on system XAML.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
using Inspectable = winrt::Windows::Foundation::IInspectable;

struct Settings {
    bool allowlist = false;
    bool needsProcessPaths = false;
    std::set<std::wstring> applications;
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_moduleAttempted = false;
std::atomic<bool> g_hooksReady = false;
std::atomic<bool> g_filteringActive = false;
// Serializes XAML work and lets unload drain an in-flight observation before
// taking its state snapshot. Recursive because XAML writes can re-enter hooks.
std::recursive_mutex g_visualMutex;
UINT g_controlMessage;
UINT g_bootstrapMessage;

// Resolve existing roots directly, without waiting for an input/layout event.
void* g_primaryBandVtable;
void* g_secondaryBandVtable;
using GetNativeHost = void*(WINAPI*)(void*, void**);
GetNativeHost g_getPrimaryHost;
GetNativeHost g_getSecondaryHost;
void* g_hostFrameHeight;
using ReleaseNativeHost = void(WINAPI*)(void*);
ReleaseNativeHost g_releaseNativeHost;
size_t g_hostElementOffset = 0;

std::optional<size_t> DecodeHostElementOffset(BYTE const* code, size_t length) {
    // Known x64 TaskbarHost::FrameHeight prologue:
    // sub rsp,28h; add rcx,<hosted-element offset>.
    // Never guess an offset on an unknown build.
    if (length >= 8 && code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[3] == 0x28 && code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] >= sizeof(void*) && code[7] <= 0x78 && code[7] % sizeof(void*) == 0) {
        return code[7];
    }
    return std::nullopt;
}

bool ReadLocalMemory(void const* address, void* output, size_t size) {
    SIZE_T read = 0;
    return address && ReadProcessMemory(GetCurrentProcess(), address, output, size, &read) && read == size;
}

struct NativeHostReference {
    void* value[2]{};  // MSVC std::shared_ptr: object and control block.
    NativeHostReference() = default;
    NativeHostReference(NativeHostReference const&) = delete;
    ~NativeHostReference() {
        if (value[1]) g_releaseNativeHost(value[1]);
    }
};

XamlRoot ReadNativeTaskbarRoot(HWND taskbar) {
    DWORD process = 0;
    DWORD thread = GetWindowThreadProcessId(taskbar, &process);
    if (process != GetCurrentProcessId() || thread != GetCurrentThreadId()) return nullptr;
    wchar_t className[64];
    if (!GetClassNameW(taskbar, className, ARRAYSIZE(className))) return nullptr;
    bool secondary = wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
    if (!secondary && wcscmp(className, L"Shell_TrayWnd") != 0) return nullptr;
    HWND bandWindow = secondary ? FindWindowExW(taskbar, nullptr, L"WorkerW", nullptr) :
        reinterpret_cast<HWND>(GetPropW(taskbar, L"TaskbandHWND"));
    if (!bandWindow) return nullptr;
    auto band = reinterpret_cast<BYTE*>(GetWindowLongPtrW(bandWindow, 0));
    if (!band) return nullptr;
    void* expectedVtable = secondary ? g_secondaryBandVtable : g_primaryBandVtable;
    auto getter = secondary ? g_getSecondaryHost : g_getPrimaryHost;
    if (!expectedVtable || !getter || !g_hostElementOffset || !g_releaseNativeHost) return nullptr;

    void* site = nullptr;
    for (size_t i = 0; i <= 20; ++i) {
        void* vtable = nullptr;
        void* candidate = band + i * sizeof(void*);
        if (!ReadLocalMemory(candidate, &vtable, sizeof(vtable))) break;
        if (vtable == expectedVtable) { site = candidate; break; }
    }
    if (!site) return nullptr;
    NativeHostReference host;
    getter(site, host.value);
    if (!host.value[0] || !host.value[1]) return nullptr;
    IUnknown* element = nullptr;
    if (!ReadLocalMemory(static_cast<BYTE*>(host.value[0]) + g_hostElementOffset,
                         &element, sizeof(element)) || !element) return nullptr;
    FrameworkElement frame{nullptr};
    if (FAILED(element->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame)))) {
        return nullptr;
    }
    return frame ? frame.XamlRoot() : nullptr;
}

bool InstallNativeRootSymbols() {
    HMODULE module = GetModuleHandleW(L"taskbar.dll");
    if (!module) module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_primaryBandVtable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_secondaryBandVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_getPrimaryHost},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"}, &g_getSecondaryHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_hostFrameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_releaseNativeHost},
    };
    if (!WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols))) return false;
    BYTE prologue[8];
    if (!ReadLocalMemory(g_hostFrameHeight, prologue, sizeof(prologue))) return false;
    auto offset = DecodeHostElementOffset(prologue, sizeof(prologue));
    if (!offset) {
        Wh_Log(L"Unrecognized TaskbarHost layout");
        return false;
    }
    g_hostElementOffset = *offset;
    Wh_Log(L"TaskbarHost element offset: %zu", *offset);
    return true;
}

std::wstring Normalize(std::wstring value) {
    const auto first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return {};
    value = value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1);
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t c) { return std::towupper(c); });
    return value;
}

std::shared_ptr<const Settings> GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

bool NeedsProcessPath(std::wstring_view rule) {
    return rule.ends_with(L".EXE") || rule.find_first_of(L"\\/") != std::wstring_view::npos;
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();
    auto mode = Wh_GetStringSetting(L"mode");
    settings->allowlist = mode && wcscmp(mode, L"allowlist") == 0;
    Wh_FreeStringSetting(mode);
    for (int i = 0; i < 1024; ++i) {
        auto entry = Wh_GetStringSetting(L"applications[%d]", i);
        bool end = !entry || !*entry;
        if (!end) {
            auto normalized = Normalize(entry);
            if (!normalized.empty()) {
                settings->needsProcessPaths |= NeedsProcessPath(normalized);
                settings->applications.insert(std::move(normalized));
            }
        }
        Wh_FreeStringSetting(entry);
        if (end) break;
    }
    {
        std::lock_guard lock(g_settingsMutex);
        g_settings = settings;
    }
    g_filteringActive = settings->allowlist || !settings->applications.empty();
    Wh_Log(L"Policy: %s, %zu rules",
           settings->allowlist ? L"allowlist" : L"blocklist",
           settings->applications.size());
}

// These WinRT interfaces are queried by IID, never cast from an arbitrary
// implementation pointer. Unused ABI entries preserve the published slots.
constexpr winrt::guid kTaskItemIid{
    0xb081d9d6, 0x9b45, 0x5363, {0x8a, 0x4d, 0x85, 0x4a, 0xdd, 0x6a, 0xbe, 0x7e}};
struct TaskItemAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowId(HWND*) = 0;
};

constexpr winrt::guid kTaskGroupIid{
    0xa152e779, 0x93df, 0x5be2, {0xaf, 0x2c, 0xd3, 0x42, 0x45, 0x2b, 0x0c, 0xe0}};
struct TaskGroupAbi : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_DisplayName() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_AccessibleName() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_Icon() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_OverlayIcon() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_OverlayDescription() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_Badge() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_Attention() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_Progress() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_VisualStateGet() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_VisualStatePut() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_BoundsGet() = 0;
    virtual HRESULT STDMETHODCALLTYPE unused_BoundsPut() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Items(void**) = 0;
};

void* g_groupModelVtable;
std::optional<IID> g_groupModelIid;
void* g_buttonVtable;
using GetModelObject = HRESULT(WINAPI*)(void*, void**);
GetModelObject g_getTaskItem;
GetModelObject g_getTaskGroup;
using TryGetContainerModel = void*(WINAPI*)(void**, UIElement*);
TryGetContainerModel g_tryGetWindowModel;
TryGetContainerModel g_tryGetGroupModel;

// Resolve a model interface by querying its reported IIDs and comparing its
// symbol-resolved vtable. This avoids scanning adjacent implementation memory.
winrt::com_ptr<IUnknown> QueryByVtable(IInspectable* object, void* vtable) {
    winrt::com_ptr<IUnknown> result;
    if (!vtable) return result;
    if (g_groupModelIid && SUCCEEDED(object->QueryInterface(*g_groupModelIid, result.put_void())) &&
        result && *reinterpret_cast<void**>(result.get()) == vtable) return result;
    result = nullptr;
    ULONG count = 0;
    IID* iids = nullptr;
    if (FAILED(object->GetIids(&count, &iids))) return result;
    std::unique_ptr<IID, decltype(&CoTaskMemFree)> cleanup(iids, CoTaskMemFree);
    for (ULONG i = 0; i < count && i < 128; ++i) {
        winrt::com_ptr<IUnknown> candidate;
        if (SUCCEEDED(object->QueryInterface(iids[i], candidate.put_void())) &&
            candidate && *reinterpret_cast<void**>(candidate.get()) == vtable) {
            g_groupModelIid = iids[i];
            return candidate;
        }
    }
    return result;
}

struct WindowIdentity {
    HWND window = nullptr;
    DWORD process = 0;
    bool operator==(WindowIdentity const&) const = default;
};

struct Identity {
    std::wstring appId;
    std::vector<std::wstring> paths;
    std::vector<WindowIdentity> windows;
    bool known() const { return !appId.empty() || !paths.empty(); }
};

bool SameIdentitySnapshot(Identity const& first, Identity const& second) {
    return first.appId == second.appId && (first.windows == second.windows ||
        std::is_permutation(first.windows.begin(), first.windows.end(),
                            second.windows.begin(), second.windows.end()));
}

void AddWindowIdentity(Identity& identity, HWND window) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    WindowIdentity item{window, pid};
    if (pid && std::find(identity.windows.begin(), identity.windows.end(), item) == identity.windows.end()) {
        identity.windows.push_back(item);
    }
}

std::wstring GetProcessPath(WindowIdentity const& window) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window.window, &pid);
    if (pid != window.process) return {};
    std::unique_ptr<void, decltype(&CloseHandle)> process(
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid), CloseHandle);
    if (!process) return {};
    wchar_t buffer[512];
    DWORD length = ARRAYSIZE(buffer);
    if (QueryFullProcessImageNameW(process.get(), 0, buffer, &length)) return {buffer, length};
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return {};
    std::wstring path(32768, L'\0');
    length = static_cast<DWORD>(path.size());
    if (!QueryFullProcessImageNameW(process.get(), 0, path.data(), &length)) return {};
    path.resize(length);
    return path;
}

template <typename Resolver>
bool ResolveIdentityPaths(Identity& identity, Identity const& previous,
                          bool previouslyResolved, Resolver resolver) {
    if (previouslyResolved && SameIdentitySnapshot(identity, previous)) {
        identity.paths = previous.paths;
        return true;
    }
    std::set<DWORD> processes;
    bool resolved = true;
    for (auto const& window : identity.windows) {
        if (!processes.insert(window.process).second) continue;
        auto path = resolver(window);
        if (path.empty()) resolved = false;
        if (!path.empty() && std::find(identity.paths.begin(), identity.paths.end(), path) == identity.paths.end()) {
            identity.paths.push_back(std::move(path));
        }
    }
    return resolved;
}

bool ReadTaskItem(IUnknown* object, Identity& identity, bool readAppId = true) {
    winrt::com_ptr<TaskItemAbi> item;
    if (FAILED(object->QueryInterface(kTaskItemIid, item.put_void()))) return false;
    if (readAppId) {
        winrt::hstring appId;
        if (SUCCEEDED(item->get_AppId(winrt::put_abi(appId))) && !appId.empty()) identity.appId = appId;
    }
    HWND window = nullptr;
    if (SUCCEEDED(item->get_WindowId(&window)) && window) AddWindowIdentity(identity, window);
    return identity.known() || !identity.windows.empty();
}

bool ReadTaskGroup(IUnknown* object, Identity& identity) {
    winrt::com_ptr<TaskGroupAbi> group;
    if (FAILED(object->QueryInterface(kTaskGroupIid, group.put_void()))) return false;
    winrt::hstring appId;
    if (SUCCEEDED(group->get_AppId(winrt::put_abi(appId))) && !appId.empty()) {
        identity.appId = appId;
    }
    winrt::Windows::Foundation::Collections::IVectorView<Inspectable> items{nullptr};
    if (SUCCEEDED(group->get_Items(winrt::put_abi(items))) && items) {
        const auto count = std::min(items.Size(), 128u);
        for (unsigned i = 0; i < count; ++i) {
            auto item = items.GetAt(i);
            Identity itemIdentity;
            if (ReadTaskItem(static_cast<IUnknown*>(winrt::get_abi(item)), itemIdentity, false)) {
                for (auto const& window : itemIdentity.windows) {
                    if (std::find(identity.windows.begin(), identity.windows.end(), window) == identity.windows.end()) {
                        identity.windows.push_back(window);
                    }
                }
            }
        }
    }
    return identity.known() || !identity.windows.empty();
}

bool ReadModel(Inspectable const& model, Identity& identity) {
    if (!model) return false;
    auto object = static_cast<IInspectable*>(winrt::get_abi(model));
    if (auto groupModel = QueryByVtable(object, g_groupModelVtable)) {
        winrt::com_ptr<IUnknown> group;
        if (SUCCEEDED(g_getTaskGroup(groupModel.get(), group.put_void())) && group) {
            return ReadTaskGroup(group.get(), identity);
        }
    }
    return false;
}

bool ReadNativeContainer(FrameworkElement const& button, Identity& identity) {
    UIElement element = button;
    if (g_tryGetGroupModel) {
        winrt::com_ptr<IUnknown> groupModel;
        g_tryGetGroupModel(groupModel.put_void(), &element);
        if (groupModel && ReadModel(groupModel.as<Inspectable>(), identity)) return true;
    }
    if (g_tryGetWindowModel) {
        winrt::com_ptr<IUnknown> windowModel;
        g_tryGetWindowModel(windowModel.put_void(), &element);
        if (windowModel) {
            // The helper returns the default ITaskListWindowViewModel interface.
            winrt::com_ptr<IUnknown> item;
            if (SUCCEEDED(g_getTaskItem(windowModel.get(), item.put_void())) && item &&
                ReadTaskItem(item.get(), identity)) return true;
        }
    }
    return false;
}

bool IsListed(Identity const& identity, Settings const& settings) {
    if (settings.applications.contains(Normalize(identity.appId))) return true;
    for (auto const& path : identity.paths) {
        if (settings.applications.contains(Normalize(path))) return true;
        auto slash = path.find_last_of(L"\\/");
        if (settings.applications.contains(Normalize(path.substr(
                slash == std::wstring::npos ? 0 : slash + 1)))) return true;
    }
    return false;
}

bool ShouldHide(Identity const& identity, Settings const& settings) {
    if (!identity.known()) return false;
    bool listed = IsListed(identity, settings);
    return settings.allowlist ? !listed : listed;
}

bool NeedsIdentityPaths(Identity const& identity, Settings const& settings) {
    if (identity.appId.empty()) return true;
    return settings.needsProcessPaths && !settings.applications.contains(Normalize(identity.appId));
}

struct Mask {
    winrt::weak_ref<FrameworkElement> element;
    Inspectable originalLocal{nullptr};
    Data::Binding originalBinding{nullptr};
    std::optional<int64_t> opacityToken;
    bool changing = false;
};

struct ButtonState {
    winrt::weak_ref<FrameworkElement> element;
    std::vector<winrt::weak_ref<FrameworkElement>> badges;
    Identity identity;
    bool pathsResolved = false;
    bool visualsDirty = true;
    std::optional<bool> lastDecision;
    winrt::event_token loaded{}, unloaded{}, layout{};
    std::optional<int64_t> templateToken;
    FrameworkElement layoutElement{nullptr};
};

using Repeater = winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater;

struct RootEvents {
    winrt::weak_ref<FrameworkElement> element;
    winrt::event_token loaded{}, unloaded{}, themeChanged{};
};

struct RepeaterEvents {
    winrt::weak_ref<Repeater> element;
    winrt::event_token prepared{}, clearing{}, indexChanged{};
};

struct PendingWork {
    bool posted = false;
    bool discovery = false;
    bool all = false;
    std::set<void*> buttons;
    std::map<void*, winrt::weak_ref<FrameworkElement>> subtrees;
    bool HasWork() const {
        return discovery || all || !buttons.empty() || !subtrees.empty();
    }
};

struct ThreadState : std::enable_shared_from_this<ThreadState> {
    std::atomic<HWND> window = nullptr;
    std::set<HWND> windows;
    std::vector<RootEvents> roots;
    std::vector<RepeaterEvents> repeaters;
    std::map<void*, std::unique_ptr<ButtonState>> buttons;
    std::vector<std::shared_ptr<Mask>> masks;
    // Preserve a known executable match while an app has no taskbar windows.
    std::map<std::wstring, std::vector<std::wstring>> identityPaths;
    PendingWork pending;
    unsigned retryCount = 0;
    bool retryArmed = false;
    bool stopping = false;
    bool walking = false;
};

void CancelPendingLayout(ButtonState& record) {
    // LayoutUpdated is thread-wide. Keep its source alive until the one-shot
    // subscription is revoked, including when a container is unloaded first.
    if (record.layout.value && record.layoutElement) record.layoutElement.LayoutUpdated(record.layout);
    record.layout = {};
    record.layoutElement = nullptr;
}

void RememberIdentityPaths(ThreadState& state, Identity& identity) {
    if (identity.appId.empty()) return;
    auto key = Normalize(identity.appId);
    if (!identity.paths.empty()) {
        if (state.identityPaths.contains(key) || state.identityPaths.size() < 512) {
            state.identityPaths[key] = identity.paths;
        }
    } else if (auto it = state.identityPaths.find(key); it != state.identityPaths.end()) {
        identity.paths = it->second;
    }
}

std::mutex g_statesMutex;
std::map<DWORD, std::shared_ptr<ThreadState>> g_states;
thread_local bool g_observing = false;

void CaptureMask(Mask& mask, FrameworkElement const& element) {
    auto property = UIElement::OpacityProperty();
    mask.originalLocal = element.ReadLocalValue(property);
    mask.originalBinding = nullptr;
    if (auto expression = element.GetBindingExpression(property)) {
        mask.originalBinding = expression.ParentBinding();
    }
}

void RestoreMask(Mask& mask, FrameworkElement const& element) {
    auto property = UIElement::OpacityProperty();
    if (mask.opacityToken) {
        element.UnregisterPropertyChangedCallback(property, *mask.opacityToken);
        mask.opacityToken.reset();
    }
    if (mask.originalBinding) {
        element.SetBinding(property, mask.originalBinding);
    } else if (mask.originalLocal == DependencyProperty::UnsetValue()) {
        element.ClearValue(property);
    } else {
        element.SetValue(property, mask.originalLocal);
    }
}

void SetMasked(ThreadState& state, FrameworkElement const& element, bool hide) {
    auto found = std::find_if(state.masks.begin(), state.masks.end(),
        [&](auto const& mask) { return mask->element.get() == element; });
    if (hide) {
        if (found == state.masks.end()) {
            auto mask = std::make_shared<Mask>();
            mask->element = winrt::make_weak(element);
            CaptureMask(*mask, element);
            state.masks.push_back(mask);
            std::weak_ptr<Mask> weak = mask;
            try { mask->opacityToken = element.RegisterPropertyChangedCallback(
                UIElement::OpacityProperty(), [weak](auto const& sender, auto const&) {
                    std::lock_guard lock(g_visualMutex);
                    auto mask = weak.lock();
                    if (!mask || mask->changing || g_unloading) return;
                    try {
                        auto element = sender.template as<FrameworkElement>();
                        CaptureMask(*mask, element);
                        mask->changing = true;
                        element.Opacity(0.0);
                    } catch (...) {
                        Wh_Log(L"Could not reapply badge opacity");
                    }
                    mask->changing = false;
                }); }
            catch (...) { state.masks.pop_back(); throw; }
            found = std::prev(state.masks.end());
        }
        auto mask = *found;
        mask->changing = true;
        try { if (element.Opacity() != 0.0) element.Opacity(0.0); }
        catch (...) { mask->changing = false; throw; }
        mask->changing = false;
    } else if (found != state.masks.end()) {
        RestoreMask(**found, element);
        state.masks.erase(found);
    }
}

bool IsBadgeElementName(std::wstring_view name) {
    return name == L"OverlayIcon" || name == L"BadgeControl";
}

void CollectBadges(DependencyObject const& node,
                   std::vector<winrt::weak_ref<FrameworkElement>>& badges, int depth = 0) {
    if (depth > 16) return;
    if (auto element = node.try_as<FrameworkElement>()) {
        auto name = element.Name();
        if (IsBadgeElementName(name)) {
            badges.push_back(winrt::make_weak(element));
            return;
        }
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        CollectBadges(Media::VisualTreeHelper::GetChild(node, i), badges, depth + 1);
    }
}

void ClearButtonMasks(ThreadState& state, ButtonState& button) {
    for (auto const& weak : button.badges) {
        if (auto badge = weak.get()) SetMasked(state, badge, false);
    }
}

void FilterButton(ThreadState& state, ButtonState& record, Settings const& settings) {
    auto button = record.element.get();
    if (!button) return;
    if (!button.IsLoaded()) {
        ClearButtonMasks(state, record);
        record.identity = {};
        record.pathsResolved = false;
        return;
    }
    if (!settings.allowlist && settings.applications.empty()) {
        ClearButtonMasks(state, record);
        record.lastDecision = false;
        return;
    }
    Identity identity;
    ReadNativeContainer(button, identity);
    bool same = SameIdentitySnapshot(identity, record.identity);
    if (NeedsIdentityPaths(identity, settings)) {
        record.pathsResolved = ResolveIdentityPaths(identity, record.identity, record.pathsResolved, GetProcessPath);
        RememberIdentityPaths(state, identity);
    } else if (same) {
        identity.paths = record.identity.paths;
    } else {
        record.pathsResolved = false;
    }
    if (!identity.paths.empty() && (record.identity.paths.empty() || record.identity.appId != identity.appId)) {
        // Another monitor can expose the same app without any of its windows.
        // Revisit an unresolved peer once a usable executable identity arrives.
        for (auto const& [key, peer] : state.buttons) {
            if (peer.get() != &record && peer->identity.appId == identity.appId &&
                peer->identity.paths.empty() && peer->identity.windows.empty()) {
                state.pending.buttons.insert(key);
            }
        }
    }
    bool hide = ShouldHide(identity, settings);
    if (!record.lastDecision || *record.lastDecision != hide || identity.appId != record.identity.appId) {
        Wh_Log(L"App %s: %s", identity.appId.c_str(), hide ? L"hide" : L"show");
    }
    record.identity = std::move(identity);
    record.lastDecision = hide;
    if (record.visualsDirty) {
        std::vector<winrt::weak_ref<FrameworkElement>> badges;
        CollectBadges(button, badges);
        for (auto const& old : record.badges) {
            if (auto badge = old.get(); badge && std::none_of(badges.begin(), badges.end(),
                    [&](auto const& weak) { return weak.get() == badge; })) SetMasked(state, badge, false);
        }
        record.badges = std::move(badges);
        record.visualsDirty = false;
    }
    for (auto const& weak : record.badges) {
        if (auto badge = weak.get()) SetMasked(state, badge, hide);
    }
}

void PostWork(ThreadState& state);
void QueueSubtree(ThreadState& state, FrameworkElement const& element);
void QueueButton(ThreadState& state, FrameworkElement const& element, bool visualsDirty, bool waitForLayout = false);
void TrackRepeater(ThreadState& state, Repeater const& repeater);
void TrackButton(ThreadState& state, FrameworkElement const& button);

void WalkButtons(ThreadState& state, DependencyObject const& node, int depth = 0) {
    if (depth > 48) return;
    if (auto element = node.try_as<FrameworkElement>()) {
        if (auto repeater = element.try_as<Repeater>()) TrackRepeater(state, repeater);
        if (winrt::get_class_name(element) == L"Taskbar.TaskListButton") {
            TrackButton(state, element);
            QueueButton(state, element, true);
            return;
        }
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        WalkButtons(state, Media::VisualTreeHelper::GetChild(node, i), depth + 1);
    }
}

bool TrackRoot(ThreadState& state, FrameworkElement const& element, PCWSTR reason);
void RestoreAll(ThreadState& state);
LRESULT CALLBACK TaskbarSubclass(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

bool AttachTaskbarWindow(ThreadState& state, HWND window) {
    if (state.windows.contains(window)) return true;
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            window, TaskbarSubclass, reinterpret_cast<DWORD_PTR>(&state))) return false;
    state.windows.insert(window);
    return true;
}

template <typename Callback>
void WithState(std::weak_ptr<ThreadState> const& weak, Callback callback) {
    if (g_unloading) return;
    std::lock_guard lock(g_visualMutex);
    if (auto state = weak.lock(); state && !state->stopping && !g_unloading) {
        try { callback(*state); }
        catch (winrt::hresult_error const& error) { Wh_Log(L"Taskbar event: %08X", static_cast<unsigned>(error.code().value)); }
        catch (...) { Wh_Log(L"Taskbar event failed"); }
    }
}

void PostWork(ThreadState& state) {
    if (g_unloading || state.stopping || state.walking || state.pending.posted || !state.pending.HasWork()) return;
    state.pending.posted = PostMessageW(state.window, g_controlMessage, 0, 0) != FALSE;
}

void QueueSubtree(ThreadState& state, FrameworkElement const& element) {
    state.pending.subtrees[winrt::get_abi(element)] = winrt::make_weak(element);
    PostWork(state);
}

void QueueButton(ThreadState& state, FrameworkElement const& element, bool visualsDirty, bool waitForLayout) {
    auto key = winrt::get_abi(element);
    auto it = state.buttons.find(key);
    if (it == state.buttons.end()) {
        state.pending.discovery = true;
        QueueSubtree(state, element);
        return;
    }
    auto& record = *it->second;
    record.visualsDirty |= visualsDirty;
    state.pending.buttons.insert(key);
    // A source update can precede asynchronous template/layout work. Watch only
    // the next layout, not every layout for the lifetime of the taskbar.
    if (waitForLayout && !record.layout.value) {
        std::weak_ptr<ThreadState> weak = state.shared_from_this();
        record.layoutElement = element;
        try { record.layout = element.LayoutUpdated([weak, key](auto const&, auto const&) {
            WithState(weak, [key](ThreadState& state) {
                auto it = state.buttons.find(key);
                if (it == state.buttons.end()) return;
                auto& record = *it->second;
                if (auto button = record.element.get()) {
                    CancelPendingLayout(record);
                    QueueButton(state, button, true);
                }
            });
        }); }
        catch (...) { record.layoutElement = nullptr; throw; }
    }
    PostWork(state);
}

void TrackButton(ThreadState& state, FrameworkElement const& button) {
    auto key = winrt::get_abi(button);
    auto it = state.buttons.find(key);
    if (it != state.buttons.end() && it->second->element.get() == button) {
        it->second->pathsResolved = false;  // A realized container may have been rebound.
        return;
    }
    auto record = std::make_unique<ButtonState>();
    record->element = winrt::make_weak(button);
    auto& item = *record;
    state.buttons[key] = std::move(record);
    std::weak_ptr<ThreadState> weak = state.shared_from_this();
    item.loaded = button.Loaded([weak](auto const& sender, auto const&) {
        WithState(weak, [&](ThreadState& state) { QueueButton(state, sender.template as<FrameworkElement>(), true, true); });
    });
    item.unloaded = button.Unloaded([weak](auto const& sender, auto const&) {
        WithState(weak, [&](ThreadState& state) {
            auto element = sender.template as<FrameworkElement>();
            auto it = state.buttons.find(winrt::get_abi(element));
            if (it != state.buttons.end()) CancelPendingLayout(*it->second);
            QueueButton(state, element, false);
        });
    });
    item.templateToken = button.RegisterPropertyChangedCallback(Controls::Control::TemplateProperty(),
        [weak](auto const& sender, auto const&) {
            WithState(weak, [&](ThreadState& state) { QueueButton(state, sender.template as<FrameworkElement>(), true, true); });
        });
}

void TrackRepeater(ThreadState& state, Repeater const& repeater) {
    if (std::any_of(state.repeaters.begin(), state.repeaters.end(),
                    [&](auto const& item) { return item.element.get() == repeater; })) return;
    state.repeaters.push_back({winrt::make_weak(repeater)});
    auto& events = state.repeaters.back();
    std::weak_ptr<ThreadState> weak = state.shared_from_this();
    auto changed = [weak](auto const&, auto const& args) {
        WithState(weak, [&](ThreadState& state) {
            if (auto element = args.Element().template try_as<FrameworkElement>()) QueueSubtree(state, element);
        });
    };
    events.prepared = repeater.ElementPrepared(changed);
    events.clearing = repeater.ElementClearing(changed);
    events.indexChanged = repeater.ElementIndexChanged(changed);
}

bool DiscoverRootsDirectly(ThreadState& state) {
    struct Context { ThreadState* state; bool missing = false; } context{&state};
    EnumThreadWindows(GetCurrentThreadId(), [](HWND window, LPARAM parameter) -> BOOL {
        auto& context = *reinterpret_cast<Context*>(parameter);
        auto& state = *context.state;
        wchar_t name[64];
        if (!GetClassNameW(window, name, ARRAYSIZE(name)) ||
            (wcscmp(name, L"Shell_TrayWnd") != 0 && wcscmp(name, L"Shell_SecondaryTrayWnd") != 0)) {
            return TRUE;
        }
        try {
            if (!AttachTaskbarWindow(state, window)) context.missing = true;
            auto root = ReadNativeTaskbarRoot(window);
            if (root) {
                if (auto content = root.Content().try_as<FrameworkElement>()) {
                    TrackRoot(state, content, L"direct native host lookup");
                } else {
                    context.missing = true;
                }
            } else {
                context.missing = true;
            }
        } catch (winrt::hresult_error const& error) {
            context.missing = true;
            Wh_Log(L"Root lookup: %08X", static_cast<unsigned>(error.code().value));
        } catch (...) {
            context.missing = true;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&context));
    return !context.missing && !state.roots.empty();
}

void Refresh(ThreadState& state) {
    std::lock_guard visualLock(g_visualMutex);
    if (state.walking || state.stopping || g_unloading) return;
    state.walking = true;
    try {
        auto settings = GetSettings();
        if (std::exchange(state.pending.discovery, false)) {
            bool ready = DiscoverRootsDirectly(state);
            if (ready && state.retryArmed) {
                KillTimer(state.window, g_controlMessage);
                state.retryArmed = false;
            } else if (!ready && !state.retryArmed && state.retryCount < 8) {
                // SetTimer is periodic at the API level; WM_TIMER kills it
                // before doing any work. At most eight startup retries are made.
                state.retryArmed = SetTimer(state.window, g_controlMessage, 250, nullptr) != 0;
            }
        }
        auto subtrees = std::move(state.pending.subtrees);
        state.pending.subtrees.clear();
        for (auto const& [key, weak] : subtrees) {
            if (state.stopping) break;
            if (auto element = weak.get()) WalkButtons(state, element);
        }
        if (std::exchange(state.pending.all, false)) {
            for (auto const& [key, button] : state.buttons) state.pending.buttons.insert(key);
        }
        auto dirty = std::move(state.pending.buttons);
        state.pending.buttons.clear();
        for (void* key : dirty) {
            if (state.stopping) break;
            auto it = state.buttons.find(key);
            if (it == state.buttons.end()) continue;
            try { FilterButton(state, *it->second, *settings); }
            catch (winrt::hresult_error const& error) { Wh_Log(L"Badge update: %08X", static_cast<unsigned>(error.code().value)); }
        }
        std::erase_if(state.buttons, [](auto const& entry) { return !entry.second->element.get(); });
        std::erase_if(state.roots, [](auto const& item) { return !item.element.get(); });
        std::erase_if(state.repeaters, [](auto const& item) { return !item.element.get(); });
        std::erase_if(state.masks, [](auto const& mask) { return !mask->element.get(); });
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"Taskbar update: %08X", static_cast<unsigned>(error.code().value));
    } catch (...) {
        Wh_Log(L"Unexpected refresh exception");
    }
    state.walking = false;
    if (state.stopping) {
        RestoreAll(state);
        return;
    }
    PostWork(state);
}

void RestoreAll(ThreadState& state) {
    state.stopping = true;
    KillTimer(state.window, g_controlMessage);
    state.retryArmed = false;
    for (auto const& item : state.roots) {
        if (auto element = item.element.get()) {
            if (item.loaded.value) element.Loaded(item.loaded);
            if (item.unloaded.value) element.Unloaded(item.unloaded);
            if (item.themeChanged.value) element.ActualThemeChanged(item.themeChanged);
        }
    }
    for (auto const& item : state.repeaters) {
        if (auto element = item.element.get()) {
            if (item.prepared.value) element.ElementPrepared(item.prepared);
            if (item.clearing.value) element.ElementClearing(item.clearing);
            if (item.indexChanged.value) element.ElementIndexChanged(item.indexChanged);
        }
    }
    for (auto const& [key, item] : state.buttons) {
        if (auto element = item->element.get()) {
            if (item->loaded.value) element.Loaded(item->loaded);
            if (item->unloaded.value) element.Unloaded(item->unloaded);
            CancelPendingLayout(*item);
            if (item->templateToken) element.UnregisterPropertyChangedCallback(
                Controls::Control::TemplateProperty(), *item->templateToken);
        }
    }
    for (auto& mask : state.masks) {
        try {
            if (auto element = mask->element.get()) RestoreMask(*mask, element);
        } catch (...) {
            Wh_Log(L"Could not restore a detached badge visual");
        }
    }
    state.masks.clear();
    state.roots.clear();
    state.repeaters.clear();
    state.buttons.clear();
    state.pending = {};
    state.identityPaths.clear();
    for (HWND window : state.windows) {
        if (IsWindow(window)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, TaskbarSubclass);
    }
    state.windows.clear();
}

LRESULT CALLBACK TaskbarSubclass(HWND window, UINT message, WPARAM wParam,
                                 LPARAM lParam, DWORD_PTR reference) {
    auto& state = *reinterpret_cast<ThreadState*>(reference);
    auto keepAlive = state.shared_from_this();
    if (message == WM_TIMER && wParam == g_controlMessage) {
        KillTimer(window, g_controlMessage);
        state.retryArmed = false;
        ++state.retryCount;
        state.pending.discovery = true;
        Refresh(state);
        return 0;
    }
    if (message == g_controlMessage) {
        if (wParam == 1) {
            RestoreAll(state);
        } else {
            state.pending.posted = false;
            if (wParam == 2) state.pending.all = true;
            if (wParam == 3) {
                state.retryCount = 0;
                state.pending.discovery = true;
            }
            Refresh(state);
        }
        return 0;
    }
    if (message == WM_NCDESTROY) {
        state.windows.erase(window);
        if (state.window == window) {
            KillTimer(window, g_controlMessage);
            state.retryArmed = false;
            state.pending.posted = false;
            auto replacement = std::find_if(state.windows.begin(), state.windows.end(),
                                             [](HWND candidate) { return IsWindow(candidate); });
            state.window = replacement == state.windows.end() ? nullptr : *replacement;
        }
        if (!state.window) {
            state.stopping = true;
            if (!state.walking) RestoreAll(state);
        } else {
            state.pending.discovery = true;
            state.pending.all = true;
        }
    }
    auto result = DefSubclassProc(window, message, wParam, lParam);
    if (message == WM_NCDESTROY && !state.stopping) PostWork(state);
    if (!g_unloading && !state.stopping && (message == WM_DISPLAYCHANGE ||
        message == WM_SETTINGCHANGE || message == WM_DPICHANGED)) {
        state.retryCount = 0;
        state.pending.discovery = true;
        PostWork(state);
    }
    return result;
}

HWND FindTaskbarOnCurrentThread() {
    HWND result = nullptr;
    EnumThreadWindows(GetCurrentThreadId(), [](HWND window, LPARAM parameter) -> BOOL {
        wchar_t name[64];
        if (GetClassNameW(window, name, ARRAYSIZE(name)) &&
            (wcscmp(name, L"Shell_TrayWnd") == 0 ||
              wcscmp(name, L"Shell_SecondaryTrayWnd") == 0)) {
            auto& result = *reinterpret_cast<HWND*>(parameter);
            if (!result || wcscmp(name, L"Shell_TrayWnd") == 0) result = window;
            if (wcscmp(name, L"Shell_TrayWnd") == 0) return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

std::shared_ptr<ThreadState> GetThreadState() {
    DWORD thread = GetCurrentThreadId();
    {
        std::lock_guard lock(g_statesMutex);
        auto it = g_states.find(thread);
        if (it != g_states.end() && !it->second->stopping && IsWindow(it->second->window)) return it->second;
    }
    HWND window = FindTaskbarOnCurrentThread();
    if (!window) return {};  // Do not manipulate XAML on an unrecognized UI thread.
    auto state = std::make_shared<ThreadState>();
    state->window = window;
    if (!AttachTaskbarWindow(*state, window)) return {};
    {
        std::lock_guard lock(g_statesMutex);
        g_states[thread] = state;
    }
    Wh_Log(L"Tracking taskbar UI thread %lu", thread);
    return state;
}

bool TrackRoot(ThreadState& state, FrameworkElement const& element, PCWSTR reason) {
    FrameworkElement root = element;
    if (auto xamlRoot = element.XamlRoot()) root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return false;
    if (std::none_of(state.roots.begin(), state.roots.end(),
                    [&](auto const& item) { return item.element.get() == root; })) {
        state.roots.push_back({winrt::make_weak(root)});
        auto& events = state.roots.back();
        std::weak_ptr<ThreadState> weak = state.shared_from_this();
        events.loaded = root.Loaded([weak](auto const& sender, auto const&) {
            WithState(weak, [&](ThreadState& state) {
                QueueSubtree(state, sender.template as<FrameworkElement>());
            });
        });
        events.unloaded = root.Unloaded([weak](auto const&, auto const&) {
            WithState(weak, [](ThreadState& state) {
                state.pending.discovery = true;
                state.pending.all = true;
                state.retryCount = 0;
                PostWork(state);
            });
        });
        events.themeChanged = root.ActualThemeChanged([weak](auto const& sender, auto const&) {
            WithState(weak, [&](ThreadState& state) { QueueSubtree(state, sender.template as<FrameworkElement>()); });
        });
        Wh_Log(L"Discovered taskbar XAML root via %s", reason);
        WalkButtons(state, root);
    }
    return true;
}

// Execute discovery setup on each taskbar's own UI thread, even when no button
// has received an update since the mod was enabled. The temporary hook is
// removed before bootstrap returns; no synthetic mouse events are used.
LRESULT CALLBACK BootstrapThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && !g_unloading) {
        auto message = reinterpret_cast<CWPSTRUCT const*>(lParam);
        if (message->message == g_bootstrapMessage) {
            try {
                if (auto state = GetThreadState()) {
                    state->pending.discovery = true;
                    Refresh(*state);
                    Wh_Log(L"Startup scan: %zu roots, %zu buttons", state->roots.size(), state->buttons.size());
                }
            } catch (...) {
                Wh_Log(L"Taskbar bootstrap failed on UI thread");
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void BootstrapExistingTaskbars() {
    std::vector<HWND> windows;
    EnumWindows([](HWND window, LPARAM parameter) -> BOOL {
        DWORD process = 0;
        GetWindowThreadProcessId(window, &process);
        wchar_t name[64];
        if (process == GetCurrentProcessId() && GetClassNameW(window, name, ARRAYSIZE(name)) &&
            (wcscmp(name, L"Shell_TrayWnd") == 0 || wcscmp(name, L"Shell_SecondaryTrayWnd") == 0)) {
            reinterpret_cast<std::vector<HWND>*>(parameter)->push_back(window);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));

    for (HWND window : windows) {
        DWORD thread = GetWindowThreadProcessId(window, nullptr);
        if (!thread || g_unloading) continue;
        HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, BootstrapThreadHook, nullptr, thread);
        if (!hook) {
            Wh_Log(L"Could not install taskbar UI-thread hook");
            continue;
        }
        SendMessageW(window, g_bootstrapMessage, 0, 0);
        UnhookWindowsHookEx(hook);
    }
}

void ObserveButton(void* implementation, bool visualsDirty) {
    if (g_unloading || !g_hooksReady || !g_filteringActive || g_observing) return;
    std::lock_guard visualLock(g_visualMutex);
    if (g_unloading) return;
    g_observing = true;
    try {
        // Check the C++/WinRT implementation layout before calling its interface.
        auto unknown = reinterpret_cast<IUnknown*>(static_cast<void**>(implementation) + 3);
        if (*reinterpret_cast<void**>(unknown) != g_buttonVtable) {
            static std::atomic<bool> logged = false;
            if (!logged.exchange(true)) {
                Wh_Log(L"TaskListButton layout differs from expected vtable; leaving badges unchanged");
            }
            g_observing = false;
            return;
        }
        FrameworkElement button{nullptr};
        winrt::check_hresult(unknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(button)));
        if (auto state = GetThreadState()) {
            if (!state->walking) {
                QueueButton(*state, button, visualsDirty, visualsDirty);
            }
        }
    } catch (winrt::hresult_error const& error) {
        static std::atomic<bool> logged = false;
        if (!logged.exchange(true)) {
            Wh_Log(L"Button observation failed: %08X %s",
                   static_cast<unsigned>(error.code().value), error.message().c_str());
        }
    } catch (...) {
        Wh_Log(L"Unexpected button observation exception");
    }
    g_observing = false;
}

using UpdateButton = void(WINAPI*)(void*);
UpdateButton g_updateStates;
UpdateButton g_updateBadge;
UpdateButton g_updateBadgeSize;
using SetOverlay = void(WINAPI*)(void*, void*);
SetOverlay g_setOverlay;
SetOverlay g_setIcon;
void WINAPI UpdateStatesHook(void* self) { g_updateStates(self); ObserveButton(self, false); }
void WINAPI UpdateBadgeHook(void* self) { g_updateBadge(self); ObserveButton(self, true); }
void WINAPI UpdateBadgeSizeHook(void* self) { g_updateBadgeSize(self); ObserveButton(self, true); }
void WINAPI SetOverlayHook(void* self, void* value) { g_setOverlay(self, value); ObserveButton(self, true); }
void WINAPI SetIconHook(void* self, void* value) { g_setIcon(self, value); ObserveButton(self, true); }

decltype(&CreateWindowExW) g_createWindow;
HWND WINAPI CreateWindowHook(DWORD exStyle, LPCWSTR className, LPCWSTR title, DWORD style,
                            int x, int y, int width, int height, HWND parent, HMENU menu,
                            HINSTANCE instance, LPVOID parameter) {
    HWND window = g_createWindow(exStyle, className, title, style, x, y, width, height,
                                 parent, menu, instance, parameter);
    if (window && !g_unloading && g_hooksReady && className && !IS_INTRESOURCE(className) &&
        (wcscmp(className, L"Shell_TrayWnd") == 0 || wcscmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
        std::lock_guard lock(g_visualMutex);
        if (!g_unloading) {
            try {
                if (auto state = GetThreadState()) {
                    state->retryCount = 0;
                    state->pending.discovery = true;
                    PostWork(*state);
                }
            } catch (...) { Wh_Log(L"Could not schedule new taskbar discovery"); }
        }
    }
    return window;
}

bool InstallTaskbarHooks(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::`vftable')"},
         &g_buttonVtable},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
         &g_updateStates, UpdateStatesHook},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
         &g_updateBadge, UpdateBadgeHook},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadgeSize(void))"},
         &g_updateBadgeSize, UpdateBadgeSizeHook},
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream))",
          LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
         &g_setIcon, SetIconHook, true},
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))",
          LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream))"},
         &g_setOverlay, SetOverlayHook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
         &g_getTaskItem},
        {{LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
         &g_groupModelVtable},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_TaskGroup(void * *))"},
         &g_getTaskGroup},
        {{LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
         &g_tryGetWindowModel},
        {{LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
         &g_tryGetGroupModel},
    };
    if (!WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols))) {
        Wh_Log(L"Required taskbar symbols unavailable. This build isn't supported yet.");
        return false;
    }
    g_hooksReady = true;
    Wh_Log(L"Taskbar hooks ready");
    return true;
}

HMODULE TaskbarModule() {
    auto module = GetModuleHandleW(L"Taskbar.View.dll");
    return module ? module : GetModuleHandleW(L"ExplorerExtensions.dll");
}

decltype(&LoadLibraryExW) g_loadLibrary;
HMODULE WINAPI LoadLibraryHook(LPCWSTR path, HANDLE file, DWORD flags) {
    auto module = g_loadLibrary(path, file, flags);
    if (module && !g_unloading && !g_moduleAttempted && module == TaskbarModule() &&
        !g_moduleAttempted.exchange(true)) {
        if (InstallTaskbarHooks(module)) Wh_ApplyHookOperations();
    }
    return module;
}

std::vector<std::shared_ptr<ThreadState>> StatesSnapshot() {
    std::lock_guard lock(g_statesMutex);
    std::vector<std::shared_ptr<ThreadState>> states;
    for (auto const& [thread, state] : g_states) states.push_back(state);
    return states;
}

BOOL Wh_ModInit() {
    g_controlMessage = RegisterWindowMessageW(
        L"Windhawk.TaskbarBadgeControl.9C994C4A-50DA-470B-95DA-E6975C452979");
    g_bootstrapMessage = RegisterWindowMessageW(
        L"Windhawk.TaskbarBadgeControl.Bootstrap.6627C859-C99B-4017-9E01-34B325B228A0");
    if (!g_controlMessage || !g_bootstrapMessage) return FALSE;
    LoadSettings();
    if (!InstallNativeRootSymbols()) {
        Wh_Log(L"Direct taskbar root lookup is unavailable; initialization aborted");
        return FALSE;
    }
    auto createWindow = reinterpret_cast<decltype(&CreateWindowExW)>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExW"));
    if (!createWindow || !WindhawkUtils::SetFunctionHook(createWindow, CreateWindowHook, &g_createWindow)) return FALSE;
    if (auto module = TaskbarModule()) {
        g_moduleAttempted = true;
        return InstallTaskbarHooks(module);
    }
    Wh_Log(L"Waiting for taskbar view module");
    auto target = reinterpret_cast<decltype(&LoadLibraryExW)>(GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"), "LoadLibraryExW"));
    return target && WindhawkUtils::SetFunctionHook(target, LoadLibraryHook, &g_loadLibrary);
}

void Wh_ModAfterInit() {
    // Close the gap between the initial module check and installing the loader hook.
    if (auto module = TaskbarModule(); module && !g_moduleAttempted.exchange(true)) {
        if (InstallTaskbarHooks(module)) Wh_ApplyHookOperations();
    }
    if (g_hooksReady) BootstrapExistingTaskbars();
    Wh_Log(L"Ready. Direct taskbar root discovery initialized; no input event is needed.");
}

BOOL Wh_ModSettingsChanged(BOOL*) {
    LoadSettings();
    for (auto const& state : StatesSnapshot()) {
        if (IsWindow(state->window)) PostMessageW(state->window, g_controlMessage, 2, 0);
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    { std::lock_guard drain(g_visualMutex); }
    // Synchronous UI-thread cleanup: restore local values/bindings, kill timers,
    // remove subclasses, and release XAML references before unloading our code.
    for (auto const& state : StatesSnapshot()) {
        if (IsWindow(state->window)) {
            SendMessageW(state->window, g_controlMessage, 1, 0);
        }
    }
    std::lock_guard lock(g_statesMutex);
    g_states.clear();
    Wh_Log(L"Badge visuals restored; taskbar hooks unloading");
}
