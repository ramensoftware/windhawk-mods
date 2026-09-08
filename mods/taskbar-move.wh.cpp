// ==WindhawkMod==
// @id              taskbar-move
// @name            Taskbar Move
// @description     Adds taskbar position and item visibility controls to the native Windows 11 context menu.
// @version         1.0.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ladvapi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds **Move taskbar** to the native Windows 11 empty-taskbar context menu.
Its submenu contains Top, Bottom, Left, and Right, excluding the current edge
so there are exactly three choices.

Below a separator, **Taskbar items** provides checked toggles for **Search**,
**Task View**, and **Widgets**. Search remembers its previous visible style;
if none is saved, it uses the search icon.

All controls use the native Windows Settings handlers. Managed or unavailable
options are disabled. Search restores its previous visible choice when that
choice is still available, otherwise it falls back to the search icon.

Requires the native Windows 11 taskbar context menu. **Move taskbar** additionally
requires the native **Taskbar position** setting to be available. Tested movement
on build 26200.9278. This does not add positioning support to older Windows
versions. If movement is unavailable, **Taskbar items** is still shown.

Changes the same persistent, user-wide settings as Windows Settings, and tells
Explorer to apply it immediately. No Explorer restart is needed. Unloading the
mod leaves the chosen position in place.

Position choices reflect the taskbar's actual edge, including positions changed
by other mods, rather than just its saved Windows preference.

The native menu injection is based on
[Taskbar Restart Explorer](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-restart-explorer.wh.cpp)
by [Mgrmjp](https://github.com/Mgrmjp).
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>
#include <winver.h>
#include <shellapi.h>
#include <roapi.h>
#include <inspectable.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <atomic>
#include <algorithm>
#include <mutex>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxi = winrt::Windows::UI::Xaml::Input;

// Native Settings ISettingItem ABI, verified against the Windows 11 public
// symbols. QueryInterface validates the contract before any methods are used.
struct NativeSettingItem : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Id(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Type(INT32*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsSetByGroupPolicy(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsEnabled(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsApplicable(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsUpdating(boolean*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValue(HSTRING, IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetValue(HSTRING, IInspectable*) = 0;
};
__CRT_UUID_DECL(NativeSettingItem, 0x40c037cc, 0xd8bf, 0x489e,
                0x86, 0x97, 0xd6, 0x6b, 0xaa, 0x32, 0x21, 0xbf)

constexpr wchar_t kSearchKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Search";
struct TaskbarOption {
    const wchar_t* text;
    const wchar_t* runtimeClass;
    const wchar_t* registryName;
    bool search;
    bool booleanValue;
};
constexpr TaskbarOption kItemOptions[] = {
    {L"Search", L"SystemSettings.Desktop.Taskbar.DesktopTaskbarSearchSetting",
     L"SearchboxTaskbarMode", true, false},
    {L"Task View", L"SystemSettings.Desktop.Taskbar.DesktopTaskbarTaskViewSetting",
     L"ShowTaskViewButton", false, true},
    {L"Widgets", L"SystemSettings.Desktop.Taskbar.DesktopTaskbarDaSetting",
     L"TaskbarDa", false, true},
};
constexpr TaskbarOption kLocationOption{
    L"Move taskbar", L"SystemSettings.Desktop.Taskbar.DesktopTaskbarLoSetting",
    L"TaskbarLocation", false, false};

static constexpr wchar_t kMoveTaskbarText[] = L"Move taskbar";
static constexpr wchar_t kItemName[] = L"WindhawkMoveTaskbarItem";
static constexpr wchar_t kSeparatorName[] = L"WindhawkMoveTaskbarSeparator";

std::atomic<bool> g_taskbarViewModuleHooked = false;
std::atomic<bool> g_unloading = false;

thread_local int g_taskbarSettingsMenuDepth = 0;
thread_local bool g_currentMenuInjected = false;

bool IsSeparator(wuxc::MenuFlyoutItemBase const& baseItem) {
    return !!baseItem.try_as<wuxc::MenuFlyoutSeparator>();
}

constexpr wchar_t kAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

winrt::com_ptr<NativeSettingItem> OpenNativeSetting(const TaskbarOption& option) {
    winrt::hstring name{option.runtimeClass};
    winrt::com_ptr<IInspectable> object;
    winrt::check_hresult(RoActivateInstance(
        reinterpret_cast<HSTRING>(winrt::get_abi(name)), object.put()));
    return object.as<NativeSettingItem>();
}

bool ReadPersistedOption(const TaskbarOption& option, DWORD* value) {
    DWORD bytes = sizeof(*value);
    return RegGetValueW(HKEY_CURRENT_USER,
                        option.search ? kSearchKey : kAdvancedKey,
                        option.registryName, RRF_RT_REG_DWORD, nullptr,
                        value, &bytes) == ERROR_SUCCESS;
}

DWORD ReadOption(NativeSettingItem* setting, const TaskbarOption& option) {
    winrt::hstring property{L"Value"};
    wf::IInspectable value{nullptr};
    winrt::check_hresult(setting->GetValue(
        reinterpret_cast<HSTRING>(winrt::get_abi(property)),
        reinterpret_cast<IInspectable**>(winrt::put_abi(value))));
    return option.booleanValue ? winrt::unbox_value<bool>(value)
                               : winrt::unbox_value<int32_t>(value);
}

// DesktopTaskbarLoSetting uses the Settings tile order, not ABE_* order.
// Verified against LocationModeFromSettingIndex on 26200.9278.
constexpr DWORD kLocationEdges[] = {ABE_BOTTOM, ABE_TOP, ABE_LEFT, ABE_RIGHT};

bool CanChangeOption(NativeSettingItem* setting) {
    boolean enabled = false, applicable = false, managed = true;
    return SUCCEEDED(setting->get_IsEnabled(&enabled)) && enabled &&
           SUCCEEDED(setting->get_IsApplicable(&applicable)) && applicable &&
           SUCCEEDED(setting->get_IsSetByGroupPolicy(&managed)) && !managed;
}

bool SetOption(const TaskbarOption& option, DWORD value,
               NativeSettingItem* existingSetting = nullptr) {
    try {
        if (g_unloading) {
            return false;
        }
        auto setting = existingSetting ? winrt::com_ptr<NativeSettingItem>{}
                                       : OpenNativeSetting(option);
        if (existingSetting) {
            setting.copy_from(existingSetting);
        }
        if (!CanChangeOption(setting.get())) {
            Wh_Log(L"%s is unavailable or managed by Windows", option.text);
            return false;
        }
        winrt::hstring property{L"Value"};
        auto boxed = option.booleanValue
            ? winrt::box_value(value != 0)
            : winrt::box_value(static_cast<int32_t>(value));
        winrt::check_hresult(setting->SetValue(
            reinterpret_cast<HSTRING>(winrt::get_abi(property)),
            reinterpret_cast<IInspectable*>(winrt::get_abi(boxed))));
        boolean updating = false;
        winrt::check_hresult(setting->get_IsUpdating(&updating));
        if (updating) {
            // Don't block Explorer's UI thread waiting for work that may need
            // that thread. The next menu construction reads the effective state.
            Wh_Log(L"%s: Windows accepted the change; update pending", option.text);
            return true;
        }
        // The handlers on the tested build are synchronous. Some swallow a
        // failed registry write, so verify completed writes as well as the ABI.
        DWORD saved;
        bool matches = ReadPersistedOption(option, &saved);
        if (option.search) {
            matches = matches && ((saved != 0) == (value != 0));
        } else if (!option.booleanValue) {
            matches = matches && value < ARRAYSIZE(kLocationEdges) &&
                      saved == kLocationEdges[value];
        } else {
            matches = matches && saved == value;
        }
        if (!matches) {
            Wh_Log(L"%s: Windows did not save the requested value %lu",
                   option.text, value);
            return false;
        }
        Wh_Log(L"%s changed to %lu", option.text, value);
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"%s failed: %08X", option.text, static_cast<unsigned>(e.code().value));
    } catch (...) {
        Wh_Log(L"%s failed", option.text);
    }
    return false;
}

bool GetCurrentTaskbarEdge(DWORD* edge) {
    // Use the actual edge rather than the saved preference: another taskbar
    // mod can move the bar without changing the native registry setting.
    APPBARDATA data = {};
    data.cbSize = sizeof(data);
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &data) || data.uEdge > ABE_BOTTOM) {
        return false;
    }
    *edge = data.uEdge;
    return true;
}

void MoveTaskbar(DWORD edge) {
    if (edge > ABE_BOTTOM) {
        return;
    }
    DWORD currentEdge;
    if (!GetCurrentTaskbarEdge(&currentEdge) || currentEdge == edge) {
        return;
    }
    for (DWORD index = 0; index < ARRAYSIZE(kLocationEdges); index++) {
        if (kLocationEdges[index] == edge) {
            SetOption(kLocationOption, index);
            break;
        }
    }
}

struct ClickRegistration {
    DWORD threadId;
    winrt::weak_ref<wuxc::MenuFlyoutItem> item;
    winrt::event_token token;
};
std::mutex g_clickMutex;
std::vector<ClickRegistration> g_clickRegistrations;
std::atomic<unsigned> g_activeClicks = 0;

struct ActiveClick {
    ActiveClick() { ++g_activeClicks; }
    ~ActiveClick() { --g_activeClicks; }
};

template <typename Item, typename Handler>
void TrackClick(const Item& item, Handler handler) {
    std::lock_guard lock(g_clickMutex);
    if (g_unloading) {
        return;
    }
    DWORD threadId = GetCurrentThreadId();
    // Resolve XAML weak references only on their owning UI thread.
    std::erase_if(g_clickRegistrations, [threadId](const auto& registration) {
        return registration.threadId == threadId && !registration.item.get();
    });
    // Allocate tracking storage before attaching anything to a XAML object.
    g_clickRegistrations.push_back({threadId,
        winrt::make_weak(item.template as<wuxc::MenuFlyoutItem>()), {}});
    try {
        g_clickRegistrations.back().token = item.Click(
            [handler](wf::IInspectable const& sender, wux::RoutedEventArgs const& args) {
                ActiveClick active;
                if (!g_unloading) {
                    try {
                        handler(sender, args);
                    } catch (...) {
                        Wh_Log(L"Taskbar menu action failed");
                    }
                }
            });
    } catch (...) {
        g_clickRegistrations.pop_back();
        throw;
    }
}

void RevokeClicksOnCurrentThread() {
    std::lock_guard lock(g_clickMutex);
    DWORD threadId = GetCurrentThreadId();
    for (auto it = g_clickRegistrations.begin(); it != g_clickRegistrations.end();) {
        if (it->threadId != threadId) {
            ++it;
            continue;
        }
        try {
            if (auto item = it->item.get()) {
                // ToggleMenuFlyoutItem also implements IMenuFlyoutItem.
                // Check the ABI HRESULT: the projected Click(token) removal
                // discards errors, which would hide a failed revocation.
                auto clickInterface = item.as<wuxc::IMenuFlyoutItem>();
                auto abi = static_cast<winrt::impl::abi_t<wuxc::IMenuFlyoutItem>*>(
                    winrt::get_abi(clickInterface));
                winrt::check_hresult(abi->remove_Click(it->token));
                try {
                    item.IsEnabled(false);
                } catch (...) {
                    // Cosmetic: the callback has already been revoked.
                }
            }
            it = g_clickRegistrations.erase(it);
        } catch (...) {
            // Keep failed revocations tracked so the unload fallback can
            // retain the module instead of leaving a dangling delegate.
            ++it;
        }
    }
}

UINT g_revokeMessage;
LRESULT CALLBACK RevokeClicksHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && g_unloading &&
        reinterpret_cast<CWPSTRUCT*>(lp)->message == g_revokeMessage) {
        RevokeClicksOnCurrentThread();
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

void RevokeAllClicks() {
    std::vector<DWORD> threads;
    {
        std::lock_guard lock(g_clickMutex);
        for (const auto& registration : g_clickRegistrations) {
            if (std::find(threads.begin(), threads.end(), registration.threadId) == threads.end()) {
                threads.push_back(registration.threadId);
            }
        }
    }
    for (DWORD threadId : threads) {
        if (threadId == GetCurrentThreadId()) {
            RevokeClicksOnCurrentThread();
            continue;
        }
        HWND window = nullptr;
        EnumThreadWindows(threadId, [](HWND hwnd, LPARAM data) -> BOOL {
            *reinterpret_cast<HWND*>(data) = hwnd;
            return FALSE;
        }, reinterpret_cast<LPARAM>(&window));
        if (!window) {
            continue;
        }
        HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RevokeClicksHook, nullptr, threadId);
        if (hook) {
            // Synchronous: the hook callback must finish before it is unhooked
            // and before Windhawk is allowed to unload this module.
            SendMessageW(window, g_revokeMessage, 0, 0);
            UnhookWindowsHookEx(hook);
        }
    }
    while (g_activeClicks.load() != 0) {
        Sleep(1);
    }
    std::lock_guard lock(g_clickMutex);
    if (!g_clickRegistrations.empty()) {
        // A vanished UI thread or failed revocation must never turn a stale
        // XAML delegate into a call to unmapped code. Retain code only on error.
        HMODULE module;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_PIN,
                              reinterpret_cast<LPCWSTR>(&RevokeAllClicks), &module)) {
            Wh_Log(L"Callback cleanup incomplete; module retained until Explorer exits");
        }
    }
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

void AppendInjectedItems(void* vectorThis) {
    // Claim this build before adding our block; do not inject twice.
    g_currentMenuInjected = true;
    bool addedPositionMenu = false;
    try {
        auto location = OpenNativeSetting(kLocationOption);
        DWORD currentEdge;
        if (CanChangeOption(location.get()) && GetCurrentTaskbarEdge(&currentEdge)) {
            wuxc::MenuFlyoutSubItem item;
            item.Name(kItemName);
            item.Text(kMoveTaskbarText);
            struct Position {
                const wchar_t* text;
                DWORD edge;
                const wchar_t* glyph;
            };
            constexpr Position positions[] = {
                {L"Top", ABE_TOP, L"\xE74A"},
                {L"Bottom", ABE_BOTTOM, L"\xE74B"},
                {L"Left", ABE_LEFT, L"\xE72B"},
                {L"Right", ABE_RIGHT, L"\xE72A"},
            };
            for (const auto& position : positions) {
                if (position.edge == currentEdge) {
                    continue;
                }
                wuxc::MenuFlyoutItem child;
                child.Text(position.text);
                wuxc::FontIcon icon;
                icon.FontFamily(wux::Media::FontFamily(L"Segoe Fluent Icons"));
                icon.Glyph(position.glyph);
                icon.FontSize(16);
                child.Icon(icon);
                TrackClick(child, [edge = position.edge](wf::IInspectable const&,
                                                        wux::RoutedEventArgs const&) {
                    MoveTaskbar(edge);
                });
                item.Items().Append(child);
            }
            MenuFlyoutItemBaseVector_Append_Original(vectorThis, item);
            addedPositionMenu = true;
        }
    } catch (...) {
        Wh_Log(L"Move taskbar unavailable; keeping Taskbar items");
    }

    if (addedPositionMenu) {
        wuxc::MenuFlyoutSeparator separator;
        separator.Name(L"WindhawkTaskbarItemsSeparator");
        MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);
    }

    wuxc::MenuFlyoutSubItem itemsMenu;
    itemsMenu.Name(L"WindhawkTaskbarItemsMenu");
    itemsMenu.Text(L"Taskbar items");
    for (const auto& option : kItemOptions) {
        wuxc::ToggleMenuFlyoutItem toggle;
        toggle.Text(option.text);
        toggle.IsEnabled(false);
        try {
            // One activation per option, shared by value and availability reads.
            // GetValue also supplies Windows' effective default when no registry
            // value exists. All access stays on this item's owning UI thread.
            auto setting = OpenNativeSetting(option);
            DWORD value = ReadOption(setting.get(), option);
            toggle.IsChecked(value != 0);
            toggle.IsEnabled(CanChangeOption(setting.get()));
            TrackClick(toggle, [option, setting](wf::IInspectable const& sender,
                                                wux::RoutedEventArgs const&) {
                auto toggle = sender.as<wuxc::ToggleMenuFlyoutItem>();
                DWORD current = ReadOption(setting.get(), option);
                DWORD next = toggle.IsChecked() ? 1 : 0;
                if (option.search) {
                    if (current >= 1 && current <= 3) {
                        Wh_SetIntValue(L"LastVisibleSearchSelection", current);
                    }
                    if (next != 0) {
                        auto previous = Wh_GetIntValue(L"LastVisibleSearchSelection", 1);
                        next = previous >= 1 && previous <= 3 ? previous : 1;
                    }
                }
                bool changed = SetOption(option, next, setting.get());
                if (!changed && option.search && next > 1) {
                    // Available search styles can change with Windows/build or
                    // layout. Fall back to the first visible choice if needed.
                    changed = SetOption(option, 1, setting.get());
                }
                if (!changed) {
                    toggle.IsChecked(current != 0);
                }
            });
        } catch (...) {
            Wh_Log(L"%s setting unavailable", option.text);
        }
        itemsMenu.Items().Append(toggle);
    }
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, itemsMenu);

    wuxc::MenuFlyoutSeparator separator;
    separator.Name(kSeparatorName);
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);
    Wh_Log(L"Injected taskbar menu controls");
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    if (!g_unloading && g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (!IsSeparator(item)) {
                AppendInjectedItems(pThis);
            }
        } catch (...) {
            Wh_Log(L"Taskbar menu append inspection failed; skipping injection");
        }
    }

    MenuFlyoutItemBaseVector_Append_Original(pThis, item);
}

struct ScopedTaskbarSettingsMenuBuild {
    bool outer = false;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_currentMenuInjected = false;
        }
    }

    ~ScopedTaskbarSettingsMenuBuild() {
        if (outer && !g_currentMenuInjected) {
            Wh_Log(L"Skipping injection: no menu item append observed");
        }

        g_taskbarSettingsMenuDepth--;
    }
};

using ContextMenus_ShowTaskbarSettingsContextMenu_t =
    void(__cdecl*)(wux::FrameworkElement const& target,
                   void* taskbarSettings,
                   wuxi::ContextRequestedEventArgs const& args,
                   unsigned long long options);
ContextMenus_ShowTaskbarSettingsContextMenu_t
    ContextMenus_ShowTaskbarSettingsContextMenu_Original;

void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(
    wux::FrameworkElement const& target,
    void* taskbarSettings,
    wuxi::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    ScopedTaskbarSettingsMenuBuild scopedBuild;

    ContextMenus_ShowTaskbarSettingsContextMenu_Original(
        target, taskbarSettings, args, options);
}

using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW* versionInfo);

DWORD GetWindowsBuild() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto rtlGetVersion = ntdll ? reinterpret_cast<RtlGetVersion_t>(
                                     GetProcAddress(ntdll, "RtlGetVersion"))
                               : nullptr;
    if (!rtlGetVersion) {
        return 0;
    }

    OSVERSIONINFOW versionInfo = {};
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

    if (rtlGetVersion(&versionInfo) != 0) {
        return 0;
    }

    Wh_Log(L"Windows version: %lu.%lu.%lu", versionInfo.dwMajorVersion,
           versionInfo.dwMinorVersion, versionInfo.dwBuildNumber);

    return versionInfo.dwBuildNumber;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    void* fixedFileInfo = nullptr;
    UINT len = 0;

    HRSRC resource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (resource) {
        HGLOBAL global = LoadResource(module, resource);
        if (global) {
            void* data = LockResource(global);
            if (data) {
                if (!VerQueryValueW(data, L"\\", &fixedFileInfo, &len) ||
                    len == 0) {
                    fixedFileInfo = nullptr;
                }
            }
        }
    }

    return static_cast<VS_FIXEDFILEINFO*>(fixedFileInfo);
}

void LogModuleInfo(HMODULE module, const wchar_t* reason) {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(module, path, ARRAYSIZE(path));

    const wchar_t* moduleName = wcsrchr(path, L'\\');
    moduleName = moduleName ? moduleName + 1 : path;

    VS_FIXEDFILEINFO* versionInfo = GetModuleVersionInfo(module);
    if (versionInfo) {
        Wh_Log(L"%s module=%s path=%s version=%u.%u.%u.%u", reason,
               moduleName, path, HIWORD(versionInfo->dwFileVersionMS),
               LOWORD(versionInfo->dwFileVersionMS),
               HIWORD(versionInfo->dwFileVersionLS),
               LOWORD(versionInfo->dwFileVersionLS));
    } else {
        Wh_Log(L"%s module=%s path=%s version=<unavailable>", reason,
               moduleName, path);
    }
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    LogModuleInfo(module, L"Resolving taskbar symbols");

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
            },
            &ContextMenus_ShowTaskbarSettingsContextMenu_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )",
            },
            &MenuFlyoutItemBaseVector_Append_Original,
            MenuFlyoutItemBaseVector_Append_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Skipping injection: missing symbols");
        return false;
    }

    Wh_Log(L"Hook installed: ContextMenus::ShowTaskbarSettingsContextMenu");
    Wh_Log(L"Hook installed: IVector<MenuFlyoutItemBase>::Append");

    return true;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module,
                                     LPCWSTR loadedFileName,
                                     bool applyHookOperations) {
    if (g_unloading || !module || g_taskbarViewModuleHooked) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewModuleHooked.exchange(true)) {
        return;
    }

    Wh_Log(L"Taskbar module loaded via LoadLibraryExW: %s",
           loadedFileName ? loadedFileName : L"<null>");

    if (HookTaskbarViewDllSymbols(module)) {
        if (applyHookOperations) {
            Wh_ApplyHookOperations();
        }
    } else {
        Wh_Log(L"Skipping injection: unsupported build or missing symbols");
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName, true);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_revokeMessage = RegisterWindowMessageW(L"Windhawk.TaskbarMove.RevokeClicks");
    if (!g_revokeMessage) {
        return FALSE;
    }

    // Log the version for diagnostics; actual support is determined by the
    // menu symbols and each native setting's availability, not a build guess.
    GetWindowsBuild();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewModuleHooked = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Skipping injection: missing symbols");
            return TRUE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                                          GetProcAddress(kernelBase,
                                                         "LoadLibraryExW"))
                                    : nullptr;
    if (!loadLibraryExW) {
        Wh_Log(L"LoadLibraryExW not found; hook not installed");
        return TRUE;
    }

    if (WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original)) {
        Wh_Log(L"Hook installed: LoadLibraryExW");
    } else {
        Wh_Log(L"LoadLibraryExW hook install failed");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");

    if (!g_taskbarViewModuleHooked) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewModuleHooked.exchange(true)) {
                Wh_Log(L"Taskbar view module found after init");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                } else {
                    Wh_Log(L"Skipping injection: missing symbols");
                }
            }
        }
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    RevokeAllClicks();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}


