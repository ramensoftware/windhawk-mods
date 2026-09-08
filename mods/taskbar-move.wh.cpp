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

Item availability follows Windows Settings. Managed or unavailable
options are disabled. Task View and Widgets use the native Settings handlers;
Search uses its native registry value and taskbar notification to preserve
the visible style independently of Settings' changing dropdown indexes.

Requires Windows 11 with the native **Taskbar position** setting enabled.
Tested movement on build 26200.9278. This does not add positioning support to
older Windows versions. The entry is hidden if the native TaskbarLocation
setting or the current taskbar edge cannot be read.

Changes the same persistent, user-wide settings as Windows Settings, and tells
Explorer to apply it immediately. No Explorer restart is needed. Unloading the
mod leaves the chosen position in place.

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
#include <string_view>

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

static constexpr wchar_t kMoveTaskbarText[] = L"Move taskbar";
static constexpr wchar_t kItemName[] = L"WindhawkMoveTaskbarItem";
static constexpr wchar_t kSeparatorName[] = L"WindhawkMoveTaskbarSeparator";

std::atomic<bool> g_taskbarViewModuleHooked = false;

thread_local int g_taskbarSettingsMenuDepth = 0;
thread_local bool g_currentMenuInjected = false;

bool HStringEquals(winrt::hstring const& value, const wchar_t* text) {
    return std::wstring_view(value.c_str(), value.size()) == text;
}

bool IsNamedItem(wuxc::MenuFlyoutItemBase const& baseItem,
                 const wchar_t* name) {
    try {
        if (auto frameworkElement = baseItem.try_as<wux::FrameworkElement>()) {
            return HStringEquals(frameworkElement.Name(), name);
        }
    } catch (...) {
    }

    return false;
}

bool IsSeparator(wuxc::MenuFlyoutItemBase const& baseItem) {
    try {
        return !!baseItem.try_as<wuxc::MenuFlyoutSeparator>();
    } catch (...) {
    }

    return false;
}

// SettingsHandlers_DesktopTaskbar.dll, DesktopTaskbarSettingsSingleton::Location
// on 26200.9278 writes this DWORD, then sends Shell_TrayWnd 0x5CA, 6, edge.
// The values match ABE_LEFT/TOP/RIGHT/BOTTOM (0/1/2/3).
constexpr wchar_t kAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr UINT kTaskbarSettingChanged = WM_USER + 0x1CA;
constexpr WPARAM kLocationSetting = 6;

winrt::com_ptr<NativeSettingItem> OpenNativeSetting(const TaskbarOption& option) {
    winrt::hstring name{option.runtimeClass};
    winrt::com_ptr<IInspectable> object;
    winrt::check_hresult(RoActivateInstance(
        reinterpret_cast<HSTRING>(winrt::get_abi(name)), object.put()));
    return object.as<NativeSettingItem>();
}

bool ReadOption(const TaskbarOption& option, DWORD* value) {
    DWORD bytes = sizeof(*value);
    return RegGetValueW(HKEY_CURRENT_USER,
                        option.search ? kSearchKey : kAdvancedKey,
                        option.registryName, RRF_RT_REG_DWORD, nullptr,
                        value, &bytes) == ERROR_SUCCESS;
}

bool CanChangeOption(NativeSettingItem* setting) {
    boolean enabled = false, applicable = false, managed = true;
    return SUCCEEDED(setting->get_IsEnabled(&enabled)) && enabled &&
           SUCCEEDED(setting->get_IsApplicable(&applicable)) && applicable &&
           SUCCEEDED(setting->get_IsSetByGroupPolicy(&managed)) && !managed;
}

bool SetOption(const TaskbarOption& option, DWORD value) {
    try {
        auto setting = OpenNativeSetting(option);
        if (!CanChangeOption(setting.get())) {
            Wh_Log(L"%s is unavailable or managed by Windows", option.text);
            return false;
        }
        if (option.search) {
            // The native Search setting's Value is a dropdown index, whose
            // mapping varies with the available search styles. Use the actual
            // persisted search mode with the Settings handler's notification.
            HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
            if (!taskbar || value > 3) {
                return false;
            }
            auto status = RegSetKeyValueW(HKEY_CURRENT_USER, kSearchKey,
                                          option.registryName, REG_DWORD,
                                          &value, sizeof(value));
            winrt::check_win32(status);
            DWORD_PTR result;
            if (!SendMessageTimeoutW(taskbar, kTaskbarSettingChanged, 102,
                                     value, SMTO_ABORTIFHUNG, 2000, &result)) {
                Wh_Log(L"Search saved, but taskbar notification failed: %lu",
                       GetLastError());
                return false;
            }
        } else {
            winrt::hstring property{L"Value"};
            auto boxed = option.booleanValue
                ? winrt::box_value(value != 0)
                : winrt::box_value(static_cast<int32_t>(value));
            winrt::check_hresult(setting->SetValue(
                reinterpret_cast<HSTRING>(winrt::get_abi(property)),
                reinterpret_cast<IInspectable*>(winrt::get_abi(boxed))));
        }
        // Some Settings handlers swallow a failed registry write. Verify it.
        DWORD saved;
        if (!ReadOption(option, &saved) || saved != value) {
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

bool IsOptionAvailable(const TaskbarOption& option) {
    try {
        return CanChangeOption(OpenNativeSetting(option).get());
    } catch (...) {
        return false;
    }
}

bool GetCurrentTaskbarEdge(DWORD* edge) {
    DWORD configuredEdge = 0;
    DWORD size = sizeof(configuredEdge);
    if (RegGetValueW(HKEY_CURRENT_USER, kAdvancedKey, L"TaskbarLocation",
                     RRF_RT_REG_DWORD, nullptr, &configuredEdge, &size) !=
            ERROR_SUCCESS || configuredEdge > ABE_BOTTOM) {
        return false;
    }

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
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD currentEdge;
    if (!taskbar || !GetCurrentTaskbarEdge(&currentEdge) || currentEdge == edge) {
        return;
    }
    LSTATUS status = RegSetKeyValueW(HKEY_CURRENT_USER, kAdvancedKey,
                                    L"TaskbarLocation", REG_DWORD,
                                    &edge, sizeof(edge));
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"TaskbarLocation write failed: %ld", status);
        return;
    }
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(taskbar, kTaskbarSettingChanged, kLocationSetting,
                             edge, SMTO_ABORTIFHUNG, 2000, &result)) {
        Wh_Log(L"Position saved, but Explorer notification failed: %lu",
               GetLastError());
        return;
    }
    Wh_Log(L"Move taskbar: %lu -> %lu", currentEdge, edge);
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

void AppendInjectedItems(void* vectorThis) {
    // Submenu Items().Append can hit this same hook. Guard before constructing it.
    g_currentMenuInjected = true;
    DWORD currentEdge;
    if (!GetCurrentTaskbarEdge(&currentEdge)) {
        Wh_Log(L"Skipping Move taskbar: native position setting unavailable");
        return;
    }

    wuxc::MenuFlyoutSubItem item;
    item.Name(kItemName);
    item.Tag(winrt::box_value(winrt::hstring{kItemName}));
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
        child.Click([edge = position.edge](wf::IInspectable const&,
                                           wux::RoutedEventArgs const&) {
            MoveTaskbar(edge);
        });
        item.Items().Append(child);
    }

    wuxc::MenuFlyoutSeparator separator;
    separator.Name(kSeparatorName);
    separator.Tag(winrt::box_value(winrt::hstring{kSeparatorName}));
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, item);

    wuxc::MenuFlyoutSeparator itemsSeparator;
    itemsSeparator.Name(L"WindhawkTaskbarItemsSeparator");
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, itemsSeparator);

    wuxc::MenuFlyoutSubItem itemsMenu;
    itemsMenu.Name(L"WindhawkTaskbarItemsMenu");
    itemsMenu.Text(L"Taskbar items");
    for (const auto& option : kItemOptions) {
        DWORD value = 0;
        bool readable = ReadOption(option, &value);
        wuxc::ToggleMenuFlyoutItem toggle;
        toggle.Text(option.text);
        toggle.IsChecked(readable && value != 0);
        toggle.IsEnabled(readable && IsOptionAvailable(option));
        toggle.Click([option](wf::IInspectable const& sender,
                              wux::RoutedEventArgs const&) {
            auto toggle = sender.as<wuxc::ToggleMenuFlyoutItem>();
            DWORD current;
            if (!ReadOption(option, &current)) {
                toggle.IsChecked(!toggle.IsChecked());
                return;
            }
            DWORD next = toggle.IsChecked() ? 1 : 0;
            if (option.search) {
                if (current >= 1 && current <= 3) {
                    Wh_SetIntValue(L"LastVisibleSearchMode", current);
                }
                if (next != 0) {
                    auto previous = Wh_GetIntValue(L"LastVisibleSearchMode", 1);
                    next = previous >= 1 && previous <= 3 ? previous : 1;
                }
            }
            if (!SetOption(option, next)) {
                toggle.IsChecked(current != 0);
            }
        });
        itemsMenu.Items().Append(toggle);
    }
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, itemsMenu);
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);
    Wh_Log(L"Injected Move taskbar submenu; current edge=%lu", currentEdge);
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    if (g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (!IsSeparator(item) &&
                !IsNamedItem(item, kItemName) &&
                !IsNamedItem(item, kSeparatorName)) {
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
    if (!module || g_taskbarViewModuleHooked) {
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

    DWORD build = GetWindowsBuild();
    if (build && build < 22000) {
        Wh_Log(L"Skipping injection: unsupported build %lu", build);
        return TRUE;
    }

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

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

