// ==WindhawkMod==
// @id              taskbar-autohide-in-context
// @name            Windows 11 Taskbar Auto-Hide in Context
// @description     Adds an option to toggle taskbar auto-hide to the native Windows 11 taskbar context menu.
// @version         1.0
// @author          oyrslyng
// @github          https://github.com/oyrslyng
// @donateUrl       https://ko-fi.com/oyrslyng
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Taskbar Auto-Hide in Context

Adds an "Auto-hide Taskbar" toggle to the native Windows 11 empty-taskbar context menu.

![Taskbar Auto-Hide demo](https://i.imgur.com/CRooPg6.gif)

### Features
- **Native UI Integration**: Injects seamlessly into the native Windows 11 XAML `MenuFlyout`.
- **Immediate Auto-Hide**: Automatically releases taskbar focus upon enabling auto-hide, ensuring the taskbar hides immediately.
- **Customizable**: Fully customizable label, and choose between classic checkmark or checkbox indicator styles.
- **Mod Compatibility**: Fully compatible with the "Taskbar Restart Explorer" mod.

### Compatibility
- Windows 11 (Tested on 26H2).

### Credits
- Inspired by the menu injection technique from [Taskbar Restart
Explorer](https://windhawk.net/mods/taskbar-restart-explorer) by **Mgrmjp**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- menuItemText: Auto-hide Taskbar
  $name: Menu item text
  $description: The text displayed for the auto-hide toggle option in the taskbar context menu.
- iconStyle: checkmark
  $name: Indicator style
  $description: The visual indicator style in the icon column.
  $options:
    - checkmark: Checkmark when enabled, empty space when disabled (classic Windows style)
    - checkbox: Checked box when enabled, empty box when disabled
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#include <atomic>
#include <mutex>
#include <string>
#include <string_view>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

static constexpr wchar_t kDefaultMenuItemText[] = L"Auto-hide Taskbar";
static constexpr wchar_t kItemName[] = L"WindhawkToggleAutohideItem";
static constexpr wchar_t kSeparatorName[] = L"WindhawkToggleAutohideSeparator";

// Known identifier for the "Taskbar Restart Explorer" mod
static constexpr wchar_t kRestartExplorerItemName[] = L"WindhawkRestartExplorerItem";

enum class IconStyle {
    Checkmark,
    Checkbox,
};

struct {
    std::wstring menuItemText;
    IconStyle iconStyle;
} g_settings;

std::atomic<bool> g_taskbarViewModuleHooked = false;
std::atomic<HWND> g_hPrevForegroundWnd = nullptr;

std::mutex g_hideWorkerMutex;
HANDLE g_hideWorkerThread = nullptr;

// Call with g_hideWorkerMutex held.
void WaitForHideWorker() {
    if (g_hideWorkerThread) {
        WaitForSingleObject(g_hideWorkerThread, INFINITE);
        CloseHandle(g_hideWorkerThread);
        g_hideWorkerThread = nullptr;
    }
}

thread_local int g_taskbarSettingsMenuDepth = 0;
thread_local bool g_currentMenuInjected = false;

void LoadSettings() {
    PCWSTR text = Wh_GetStringSetting(L"menuItemText");
    g_settings.menuItemText = (text && *text) ? text : kDefaultMenuItemText;
    Wh_FreeStringSetting(text);

    PCWSTR iconStyleStr = Wh_GetStringSetting(L"iconStyle");
    g_settings.iconStyle = (iconStyleStr && wcscmp(iconStyleStr, L"checkbox") == 0)
            ? IconStyle::Checkbox
            : IconStyle::Checkmark;
    Wh_FreeStringSetting(iconStyleStr);
}

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

bool GetTaskbarAutohideState() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) {
        Wh_Log(L"Shell_TrayWnd not found");
        return false;
    }

    APPBARDATA abd = {};
    abd.cbSize = sizeof(abd);
    abd.hWnd = hTaskbar;

    LPARAM state = SHAppBarMessage(ABM_GETSTATE, &abd);
    return (state & ABS_AUTOHIDE) != 0;
}

DWORD WINAPI HideTaskbarWorker(LPVOID lpParam) {
    HWND hPrev = static_cast<HWND>(lpParam);

    // Give the XAML context menu flyout time to complete its dismissal animation
    Sleep(80);

    // Release focus from the taskbar so that PermitAutoHide() returns TRUE.
    // If a previous application window was active, restore focus to it.
    if (hPrev && IsWindow(hPrev) && IsWindowVisible(hPrev)) {
        SetForegroundWindow(hPrev);
    } else {
        HWND hProgman = FindWindowW(L"Progman", nullptr);
        if (hProgman) {
            SetForegroundWindow(hProgman);
        }
    }

    return 0;
}

void ToggleTaskbarAutohide() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) {
        Wh_Log(L"Shell_TrayWnd not found; cannot toggle auto-hide");
        return;
    }

    APPBARDATA abd = {};
    abd.cbSize = sizeof(abd);
    abd.hWnd = hTaskbar;

    LPARAM state = SHAppBarMessage(ABM_GETSTATE, &abd);
    bool isCurrentlyAutoHide = (state & ABS_AUTOHIDE) != 0;
    bool newAutoHide = !isCurrentlyAutoHide;

    abd.lParam = newAutoHide ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &abd);

    Wh_Log(L"Taskbar auto-hide toggled from %s to %s",
           isCurrentlyAutoHide ? L"enabled" : L"disabled",
           newAutoHide ? L"enabled" : L"disabled");

    // If auto-hide was just enabled, dispatch background worker to release
    // focus so that Windows Shell can hide the taskbar naturally
    if (newAutoHide) {
        std::lock_guard<std::mutex> lock(g_hideWorkerMutex);
        WaitForHideWorker();
        g_hideWorkerThread =
            CreateThread(nullptr, 0, HideTaskbarWorker, g_hPrevForegroundWnd.load(), 0, nullptr);
    }
}

void OnToggleAutohideClick(wf::IInspectable const&,
                           wux::RoutedEventArgs const&) {
    ToggleTaskbarAutohide();
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

wuxc::MenuFlyoutItem CreateAutohideMenuItem() {
    bool isAutoHide = GetTaskbarAutohideState();

    wuxc::MenuFlyoutItem item;
    item.Name(kItemName);
    item.Tag(winrt::box_value(winrt::hstring{kItemName}));
    item.Text(g_settings.menuItemText.c_str());

    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.FontSize(16);

    if (g_settings.iconStyle == IconStyle::Checkbox) {
        icon.Glyph(isAutoHide ? L"\xE73A" : L"\xE739");
        icon.Opacity(1.0);
    } else {
        // Classic checkmark style: visible checkmark when enabled, transparent
        // placeholder when disabled
        icon.Glyph(L"\xE73E");
        icon.Opacity(isAutoHide ? 1.0 : 0.0);
    }

    item.Icon(icon);
    item.Click(wux::RoutedEventHandler{&OnToggleAutohideClick});
    return item;
}

wuxc::MenuFlyoutSeparator CreateAutohideSeparator() {
    wuxc::MenuFlyoutSeparator separator;
    separator.Name(kSeparatorName);
    separator.Tag(winrt::box_value(winrt::hstring{kSeparatorName}));
    return separator;
}

void InsertInjectedItems(void* vectorThis) {
    auto* vectorPtr =
        reinterpret_cast<wf::Collections::IVector<wuxc::MenuFlyoutItemBase>*>(
            vectorThis);
    auto item = CreateAutohideMenuItem();
    auto separator = CreateAutohideSeparator();
    vectorPtr->InsertAt(0, separator);
    vectorPtr->InsertAt(0, item);

    g_currentMenuInjected = true;
    Wh_Log(L"Injected Toggle Autohide + Separator at index 0 via InsertAt");
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    // Let the target item be appended to the vector first
    MenuFlyoutItemBaseVector_Append_Original(pThis, item);

    // After the first non-separator, non-Windhawk item is appended (typically
    // "Taskbar settings"), insert Auto-Hide + Separator at index 0. This
    // guarantees Auto-Hide sits at the very top regardless of whether other
    // mods (like Restart Explorer) were loaded before or after.
    if (g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (!IsSeparator(item) && !IsNamedItem(item, kItemName) &&
                !IsNamedItem(item, kSeparatorName) &&
                !IsNamedItem(item, kRestartExplorerItemName)) {
                InsertInjectedItems(pThis);
            }
        } catch (...) {
            Wh_Log(L"Taskbar menu insert failed");
        }
    }
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

    // Capture the active foreground window before the context menu grabs focus,
    // so it can be restored when auto-hide is toggled.
    HWND hForeground = GetForegroundWindow();
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hForeground && hForeground != hTaskbar) {
        g_hPrevForegroundWnd = hForeground;
    }

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

    OSVERSIONINFOW versionInfo = {sizeof(versionInfo)};
    if (rtlGetVersion(&versionInfo) != 0) {
        return 0;
    }

    Wh_Log(L"Windows build: %lu", versionInfo.dwBuildNumber);
    return versionInfo.dwBuildNumber;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(module, path, ARRAYSIZE(path));
    Wh_Log(L"Resolving taskbar symbols: %s", path);

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

    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
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

    LoadSettings();

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
                             GetProcAddress(kernelBase, "LoadLibraryExW"))
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

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    std::lock_guard<std::mutex> lock(g_hideWorkerMutex);
    WaitForHideWorker();
}
