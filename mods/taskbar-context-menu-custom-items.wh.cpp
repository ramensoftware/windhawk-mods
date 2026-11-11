// ==WindhawkMod==
// @id              taskbar-context-menu-custom-items
// @name            Taskbar Context Menu Custom Items
// @description     Add your own items to the Windows 11 taskbar right-click menu: programs, folders, scripts, URLs and settings pages
// @version         1.0
// @author          jcyrio
// @github          https://github.com/jcyrio
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Context Menu Custom Items

The Windows 11 taskbar right-click menu only offers "Taskbar settings" and
"Task Manager". This mod lets you add your own items to that menu. Each item
has a label, an optional icon, and a command that runs when you click it.

![The taskbar menu with three custom items](https://raw.githubusercontent.com/jcyrio/windhawk-taskbar-context-menu-custom-items/main/screenshot.png)

The items are added to the native menu itself, so they look and behave like
the built-in ones.

## Menu items

Add as many items as you like in the mod settings. Each item has:

* **Label**: the text shown in the menu. A label that is a single dash (`-`)
  adds a separator line instead of an item.
* **Command**: what to open. Anything that works in the Run dialog (Win+R)
  works here: a program (`notepad.exe`), a full path to a file or script, a
  folder, a URL (`https://example.com`), a settings page
  (`ms-settings:display`) or a shell location (`shell:Downloads`). Environment
  variables such as `%USERPROFILE%` are expanded. Quotes around a path with
  spaces are not needed.
* **Arguments**: optional command-line arguments for the command.
* **Icon**: optional. The hex code of a glyph in the Segoe Fluent Icons font,
  for example `E756` for a command prompt. The full list is in Microsoft's
  [Segoe Fluent Icons reference](https://learn.microsoft.com/en-us/windows/apps/design/style/segoe-fluent-icons-font).
  You can also paste a single character such as an emoji.
* **Run as administrator**: starts the command elevated. Windows shows the
  usual UAC prompt.
* **Hide window**: starts the command without a visible window. Useful for
  scripts and console commands.

An item with a label but no command is shown greyed out, which can be used as
a heading.

### Examples

| Label | Command | Arguments |
| --- | --- | --- |
| Terminal | `wt.exe` | |
| Downloads | `shell:Downloads` | |
| Display settings | `ms-settings:display` | |
| Network connections | `ncpa.cpl` | |
| My script | `powershell.exe` | `-NoProfile -File "C:\Scripts\my script.ps1"` |
| Restart Explorer | `cmd.exe` | `/c taskkill /f /im explorer.exe & start explorer.exe` |

## Other settings

* **Placement**: whether the custom items appear above or below the built-in
  items.
* **Separator line**: draws a line between the custom items and the built-in
  items.
* **Submenu name**: if set, the custom items are grouped in a submenu with
  this name instead of being listed directly in the menu.

Settings take effect the next time the menu is opened.

## Notes

* Windows 11 only. The mod extends the modern taskbar menu and has no effect
  on the Windows 10 taskbar or on taskbar replacements.
* The menu that opens when right-clicking an app button, the Start button or
  the system tray is not changed, only the menu of the taskbar itself.
* Commands run with the same permissions as File Explorer, unless "Run as
  administrator" is turned on.
* The mod relies on debug symbols for the taskbar. After a Windows update,
  Windhawk may need a moment to download new symbols before the items show up
  again. If the items are missing, enable logging for the mod to see why.

## Credits

The menu injection technique is based on
[Taskbar Restart Explorer](https://windhawk.net/mods/taskbar-restart-explorer)
by Mgrmjp and
[Windows 11 Taskbar Auto-Hide in Context](https://windhawk.net/mods/taskbar-autohide-in-context)
by oyrslyng.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- items:
  - - name: Terminal
      $name: Label
      $description: Text shown in the menu. Use a single dash (-) to add a separator line.
    - command: wt.exe
      $name: Command
      $description: Program, file, folder, URL, ms-settings page or shell location to open. Environment variables such as %USERPROFILE% are expanded.
    - arguments: ""
      $name: Arguments
      $description: Optional command-line arguments
    - icon: E756
      $name: Icon
      $description: Optional hex code of a Segoe Fluent Icons glyph, for example E756, or a single character such as an emoji
    - runAsAdmin: false
      $name: Run as administrator
    - hideWindow: false
      $name: Hide window
      $description: Start the command without a visible window, useful for scripts and console commands
  - - name: Control Panel
    - command: control.exe
    - arguments: ""
    - icon: E770
    - runAsAdmin: false
    - hideWindow: false
  - - name: Restart Explorer
    - command: cmd.exe
    - arguments: /c taskkill /f /im explorer.exe & start explorer.exe
    - icon: E72C
    - runAsAdmin: false
    - hideWindow: true
  $name: Menu items
- placement: bottom
  $name: Placement
  $description: Where the custom items appear in the menu
  $options:
  - top: Above the built-in items
  - bottom: Below the built-in items
- separator: true
  $name: Separator line
  $description: Draw a line between the custom items and the built-in items
- submenuName: ""
  $name: Submenu name
  $description: If set, the custom items are grouped in a submenu with this name instead of being listed directly in the menu
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <shellapi.h>
#include <shlobj.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

using MenuItemsVector = wfc::IVector<wuxc::MenuFlyoutItemBase>;

////////////////////////////////////////////////////////////////////////////////
// Settings

struct MenuItemSettings {
    std::wstring name;
    std::wstring command;
    std::wstring arguments;
    std::wstring icon;
    bool runAsAdmin = false;
    bool hideWindow = false;

    // The text shown in the menu and in the log. An item without a label is
    // shown as its command.
    const std::wstring& DisplayName() const {
        return name.empty() ? command : name;
    }
};

enum class Placement {
    top,
    bottom,
};

struct Settings {
    std::vector<MenuItemSettings> items;
    Placement placement = Placement::bottom;
    bool separator = true;
    std::wstring submenuName;
};

// The settings are loaded on a Windhawk thread and read on the taskbar UI
// thread, so they are published as an immutable snapshot.
std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings;

std::shared_ptr<const Settings> GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

std::wstring Trim(std::wstring_view value) {
    const wchar_t* whitespace = L" \t\r\n";
    size_t first = value.find_first_not_of(whitespace);
    if (first == std::wstring_view::npos) {
        return {};
    }

    size_t last = value.find_last_not_of(whitespace);
    return std::wstring(value.substr(first, last - first + 1));
}

template <typename... Args>
std::wstring GetStringSetting(PCWSTR valueName, Args... args) {
    auto value = WindhawkUtils::StringSetting::make(valueName, args...);
    return value.get() ? Trim(value.get()) : std::wstring();
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();

    for (int i = 0;; i++) {
        MenuItemSettings item;
        item.name = GetStringSetting(L"items[%d].name", i);
        item.command = GetStringSetting(L"items[%d].command", i);

        // The list ends at the first entry that has neither a label nor a
        // command.
        if (item.name.empty() && item.command.empty()) {
            break;
        }

        item.arguments = GetStringSetting(L"items[%d].arguments", i);
        item.icon = GetStringSetting(L"items[%d].icon", i);
        item.runAsAdmin = Wh_GetIntSetting(L"items[%d].runAsAdmin", i) != 0;
        item.hideWindow = Wh_GetIntSetting(L"items[%d].hideWindow", i) != 0;

        settings->items.push_back(std::move(item));
    }

    settings->placement = GetStringSetting(L"placement") == L"top"
                              ? Placement::top
                              : Placement::bottom;
    settings->separator = Wh_GetIntSetting(L"separator") != 0;
    settings->submenuName = GetStringSetting(L"submenuName");

    Wh_Log(L"Loaded %d menu item(s)", static_cast<int>(settings->items.size()));

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(settings);
}

////////////////////////////////////////////////////////////////////////////////
// Running commands

// Set when the mod is being unloaded. A launch thread can outlive the mod's
// registration with Windhawk, and must not log after that point.
std::atomic<bool> g_unloading;

struct LaunchRequest {
    HMODULE module = nullptr;
    std::wstring name;
    std::wstring command;
    std::wstring arguments;
    bool runAsAdmin = false;
    bool hideWindow = false;
};

std::wstring ExpandEnvironment(const std::wstring& value) {
    if (value.empty()) {
        return value;
    }

    DWORD size = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
    if (!size) {
        return value;
    }

    std::wstring expanded(size, L'\0');
    DWORD written =
        ExpandEnvironmentStringsW(value.c_str(), expanded.data(), size);
    if (!written || written > size) {
        return value;
    }

    // The returned size includes the terminating null character.
    expanded.resize(written - 1);
    return expanded;
}

// A path pasted from File Explorer's "Copy as path" comes wrapped in quotes,
// which ShellExecuteEx doesn't accept for the file to open.
std::wstring StripSurroundingQuotes(const std::wstring& value) {
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"' &&
        value.find(L'"', 1) == value.size() - 1) {
        return value.substr(1, value.size() - 2);
    }

    return value;
}

DWORD WINAPI LaunchThread(LPVOID param) {
    std::unique_ptr<LaunchRequest> request(static_cast<LaunchRequest*>(param));
    HMODULE module = request->module;

    {
        HRESULT hrCoInit = CoInitializeEx(
            nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        std::wstring command =
            StripSurroundingQuotes(ExpandEnvironment(request->command));
        std::wstring arguments = ExpandEnvironment(request->arguments);

        // Explorer's own working directory is System32. Start commands in the
        // user's profile folder instead, like a terminal would.
        std::wstring directory = ExpandEnvironment(L"%USERPROFILE%");

        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        // The thread exits right after the call, so the launch has to complete
        // before ShellExecuteEx returns. Error UI is left enabled so that a
        // mistyped command is reported to the user.
        sei.fMask = SEE_MASK_NOASYNC;
        sei.lpVerb = request->runAsAdmin ? L"runas" : nullptr;
        sei.lpFile = command.c_str();
        sei.lpParameters = arguments.empty() ? nullptr : arguments.c_str();
        sei.lpDirectory = directory.empty() ? nullptr : directory.c_str();
        sei.nShow = request->hideWindow ? SW_HIDE : SW_SHOWNORMAL;

        BOOL started = ShellExecuteExW(&sei);
        DWORD error = started ? ERROR_SUCCESS : GetLastError();

        if (g_unloading) {
            // No logging, see above.
        } else if (started) {
            Wh_Log(L"Started \"%s\"", request->name.c_str());
        } else {
            if (error == ERROR_CANCELLED) {
                Wh_Log(L"Starting \"%s\" was cancelled by the user",
                       request->name.c_str());
            } else {
                Wh_Log(L"Failed to start \"%s\" (%s), error %lu",
                       request->name.c_str(), command.c_str(), error);
            }
        }

        if (SUCCEEDED(hrCoInit)) {
            CoUninitialize();
        }
    }

    request.reset();

    // Release the reference that kept the mod loaded while the thread ran.
    FreeLibraryAndExitThread(module, 0);
    return 0;
}

// Commands are started on a separate thread so that a slow launch or a UAC
// prompt can't block the taskbar.
void Launch(const MenuItemSettings& item) {
    auto request = std::make_unique<LaunchRequest>();
    request->name = item.DisplayName();
    request->command = item.command;
    request->arguments = item.arguments;
    request->runAsAdmin = item.runAsAdmin;
    request->hideWindow = item.hideWindow;

    // Keep the mod loaded until the thread exits, even if the mod is disabled
    // in the meantime.
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            reinterpret_cast<LPCWSTR>(&LaunchThread),
                            &request->module)) {
        Wh_Log(L"GetModuleHandleExW failed, error %lu", GetLastError());
        return;
    }

    HMODULE module = request->module;
    HANDLE thread =
        CreateThread(nullptr, 0, LaunchThread, request.get(), 0, nullptr);
    if (!thread) {
        Wh_Log(L"CreateThread failed, error %lu", GetLastError());
        FreeLibrary(module);
        return;
    }

    // The thread owns the request now.
    request.release();
    CloseHandle(thread);
}

////////////////////////////////////////////////////////////////////////////////
// Building the menu items

// Name given to every element the mod adds, to tell them apart from the
// built-in items and from items added by other mods.
constexpr wchar_t kInjectedElementName[] = L"WindhawkCustomTaskbarMenuItem";

bool IsHexDigit(wchar_t ch) {
    return (ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f') ||
           (ch >= L'A' && ch <= L'F');
}

// Converts the icon setting to the text of the glyph. Accepts a hex code point
// such as "E756", "0xE756", "U+E756" or "\uE756". Anything else is used as is,
// which allows pasting a character directly.
std::wstring GlyphFromIconSetting(const std::wstring& icon, bool* isCodePoint) {
    *isCodePoint = false;

    std::wstring_view hex = icon;
    if (hex.size() > 2 &&
        (hex.starts_with(L"0x") || hex.starts_with(L"0X") ||
         hex.starts_with(L"U+") || hex.starts_with(L"u+") ||
         hex.starts_with(L"\\u") || hex.starts_with(L"\\U"))) {
        hex.remove_prefix(2);
    }

    if (hex.size() < 4 || hex.size() > 6) {
        return icon;
    }

    for (wchar_t ch : hex) {
        if (!IsHexDigit(ch)) {
            return icon;
        }
    }

    unsigned long codePoint = wcstoul(std::wstring(hex).c_str(), nullptr, 16);
    if (codePoint == 0 || codePoint > 0x10FFFF ||
        (codePoint >= 0xD800 && codePoint <= 0xDFFF)) {
        return icon;
    }

    *isCodePoint = true;

    std::wstring glyph;
    if (codePoint < 0x10000) {
        glyph.push_back(static_cast<wchar_t>(codePoint));
    } else {
        codePoint -= 0x10000;
        glyph.push_back(static_cast<wchar_t>(0xD800 + (codePoint >> 10)));
        glyph.push_back(static_cast<wchar_t>(0xDC00 + (codePoint & 0x3FF)));
    }

    return glyph;
}

// An element the mod added to a taskbar menu. The click handler is the only
// object in a menu that is implemented by the mod, so it has to be removed
// before the mod is unloaded. The elements are tracked with weak references so
// that tracking doesn't keep closed menus alive.
struct InjectedElement {
    winrt::weak_ref<wuxc::MenuFlyoutItemBase> element;
    winrt::event_token clickToken{};
    bool hasClickHandler = false;
};

std::mutex g_injectedElementsMutex;
std::vector<InjectedElement> g_injectedElements;

void TrackInjectedElement(InjectedElement&& element) {
    std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
    g_injectedElements.push_back(std::move(element));
}

// Forgets the elements of menus that no longer exist.
void PruneInjectedElements() {
    std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
    std::erase_if(g_injectedElements, [](const InjectedElement& entry) {
        return !entry.element.get();
    });
}

// Detaches the mod from the menus it added elements to: removes the click
// handlers and hides the elements, in case a menu is open or is shown again.
// Must be called from the taskbar thread.
void ReleaseInjectedElements() {
    std::vector<InjectedElement> elements;
    {
        std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
        elements.swap(g_injectedElements);
    }

    int released = 0;
    for (const auto& entry : elements) {
        try {
            auto element = entry.element.get();
            if (!element) {
                continue;
            }

            if (entry.hasClickHandler) {
                if (auto item = element.try_as<wuxc::MenuFlyoutItem>()) {
                    item.Click(entry.clickToken);
                }
            }

            element.Visibility(wux::Visibility::Collapsed);
            released++;
        } catch (...) {
        }
    }

    Wh_Log(L"Released %d menu element(s)", released);
}

wuxc::MenuFlyoutSeparator CreateSeparator() {
    wuxc::MenuFlyoutSeparator separator;
    separator.Name(kInjectedElementName);

    InjectedElement tracked;
    tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(separator);
    TrackInjectedElement(std::move(tracked));

    return separator;
}

wuxc::MenuFlyoutItemBase CreateMenuItem(const MenuItemSettings& settings) {
    if (settings.name == L"-") {
        return CreateSeparator();
    }

    wuxc::MenuFlyoutItem item;
    item.Name(kInjectedElementName);
    item.Text(settings.DisplayName());

    if (!settings.icon.empty()) {
        bool isCodePoint;
        std::wstring glyph = GlyphFromIconSetting(settings.icon, &isCodePoint);

        wuxc::FontIcon icon;
        if (isCodePoint) {
            icon.FontFamily(
                wuxm::FontFamily(L"Segoe Fluent Icons,Segoe MDL2 Assets"));
        } else {
            icon.FontFamily(wuxm::FontFamily(L"Segoe UI Emoji,Segoe UI"));
        }
        icon.FontSize(16);
        icon.Glyph(glyph);
        item.Icon(icon);
    }

    // The weak reference is created before the click handler is added, so that
    // a handler is never added to an element that can't be tracked.
    InjectedElement tracked;
    tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(item);

    if (settings.command.empty()) {
        // A label without a command acts as a heading.
        item.IsEnabled(false);
    } else {
        tracked.clickToken =
            item.Click([settings](wf::IInspectable const&,
                                  wux::RoutedEventArgs const&) {
                Wh_Log(L"Clicked \"%s\"", settings.DisplayName().c_str());
                Launch(settings);
            });
        tracked.hasClickHandler = true;
    }

    TrackInjectedElement(std::move(tracked));

    return item;
}

// Creates the elements to add to the taskbar menu, in display order and without
// the separator that divides them from the built-in items.
std::vector<wuxc::MenuFlyoutItemBase> CreateCustomElements(
    const Settings& settings) {
    std::vector<wuxc::MenuFlyoutItemBase> elements;

    if (settings.items.empty()) {
        return elements;
    }

    if (!settings.submenuName.empty()) {
        wuxc::MenuFlyoutSubItem subItem;
        subItem.Name(kInjectedElementName);
        subItem.Text(settings.submenuName);

        InjectedElement tracked;
        tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(subItem);
        TrackInjectedElement(std::move(tracked));

        auto subItems = subItem.Items();
        for (const auto& item : settings.items) {
            subItems.Append(CreateMenuItem(item));
        }

        elements.push_back(subItem);
    } else {
        for (const auto& item : settings.items) {
            elements.push_back(CreateMenuItem(item));
        }
    }

    return elements;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar menu hooks

// State of the taskbar menu that is currently being built on this thread. It
// lives on the stack of the ShowTaskbarSettingsContextMenu hook.
struct MenuBuildState {
    std::shared_ptr<const Settings> settings;
    // Identity of the item collection of the menu.
    void* itemsIdentity = nullptr;
    // Elements added by the mod, in display order.
    std::vector<wuxc::MenuFlyoutItemBase> injected;
    // Set while the mod itself modifies the item collection.
    bool modifying = false;
};

thread_local MenuBuildState* g_menuBuildState;

void* GetIdentity(MenuItemsVector const& items) {
    return winrt::get_abi(items.as<wf::IUnknown>());
}

// Adds the custom elements to the menu, right after the first built-in item
// was added to it.
void InjectCustomElements(MenuItemsVector const& items, MenuBuildState& state) {
    const Settings& settings = *state.settings;

    PruneInjectedElements();

    auto elements = CreateCustomElements(settings);
    if (elements.empty()) {
        return;
    }

    if (settings.placement == Placement::top) {
        if (settings.separator) {
            elements.push_back(CreateSeparator());
        }

        uint32_t index = 0;
        for (const auto& element : elements) {
            items.InsertAt(index++, element);
        }
    } else {
        if (settings.separator) {
            elements.insert(elements.begin(), CreateSeparator());
        }

        for (const auto& element : elements) {
            items.Append(element);
        }
    }

    state.injected = std::move(elements);

    Wh_Log(L"Added %d element(s) to the taskbar menu",
           static_cast<int>(state.injected.size()));
}

// Moves the custom elements back to the end of the menu after another item was
// appended behind them.
void MoveCustomElementsToEnd(MenuItemsVector const& items,
                             MenuBuildState& state) {
    for (const auto& element : state.injected) {
        uint32_t index;
        if (items.IndexOf(element, index)) {
            items.RemoveAt(index);
        }
    }

    for (const auto& element : state.injected) {
        items.Append(element);
    }
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    MenuFlyoutItemBaseVector_Append_Original(pThis, item);

    MenuBuildState* state = g_menuBuildState;
    if (!state || state->modifying) {
        return;
    }

    state->modifying = true;

    try {
        // pThis points to the projected IVector<MenuFlyoutItemBase> object.
        auto const& items = *reinterpret_cast<MenuItemsVector const*>(pThis);
        void* identity = GetIdentity(items);

        if (!state->itemsIdentity) {
            // The first item appended while the taskbar menu is being built
            // identifies the item collection of the menu.
            state->itemsIdentity = identity;
            InjectCustomElements(items, *state);
        } else if (state->itemsIdentity == identity &&
                   !state->injected.empty() &&
                   state->settings->placement == Placement::bottom) {
            MoveCustomElementsToEnd(items, *state);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to update the taskbar menu: %08X %s",
               static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to update the taskbar menu");
    }

    state->modifying = false;
}

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
    if (g_menuBuildState) {
        // Nested call, the outer call already tracks the menu.
        ContextMenus_ShowTaskbarSettingsContextMenu_Original(
            target, taskbarSettings, args, options);
        return;
    }

    MenuBuildState state;
    state.settings = GetSettings();

    // Resets the thread state even if the original function throws.
    struct ScopedMenuBuild {
        explicit ScopedMenuBuild(MenuBuildState* state) {
            g_menuBuildState = state;
        }
        ~ScopedMenuBuild() { g_menuBuildState = nullptr; }
    };

    if (state.settings && !state.settings->items.empty() && !g_unloading) {
        ScopedMenuBuild scopedMenuBuild(&state);

        ContextMenus_ShowTaskbarSettingsContextMenu_Original(
            target, taskbarSettings, args, options);

        if (!state.itemsIdentity) {
            Wh_Log(L"The taskbar menu was shown without adding items to it");
        }
    } else {
        ContextMenus_ShowTaskbarSettingsContextMenu_Original(
            target, taskbarSettings, args, options);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Hook setup

std::atomic<bool> g_taskbarViewDllHooked;

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))"},
            &ContextMenus_ShowTaskbarSettingsContextMenu_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )"},
            &MenuFlyoutItemBaseVector_Append_Original,
            MenuFlyoutItemBaseVector_Append_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Failed to hook the taskbar menu, the required symbols are "
               L"missing");
        return false;
    }

    Wh_Log(L"Hooked the taskbar menu");
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }

    if (!g_taskbarViewDllHooked && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllHooked.exchange(true)) {
        Wh_Log(L"Taskbar view module loaded: %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWnd));

    return taskbarWnd;
}

using RunFromWindowThreadProc_t = void (*)(void* parameter);

// Runs a function on the thread of the given window and waits for it to
// complete.
bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                            cwp->lParam);
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0,
                 reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllHooked = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    // The taskbar view module is loaded after Explorer starts. Watch for it in
    // case the mod is loaded first.
    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW =
        kernelBaseModule
            ? reinterpret_cast<LoadLibraryExW_t>(
                  GetProcAddress(kernelBaseModule, "LoadLibraryExW"))
            : nullptr;
    if (pKernelBaseLoadLibraryExW) {
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    } else {
        Wh_Log(L"LoadLibraryExW not found in kernelbase.dll");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The module may have been loaded between Wh_ModInit and the moment the
    // LoadLibraryExW hook became active.
    if (!g_taskbarViewDllHooked) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllHooked.exchange(true)) {
                Wh_Log(L"Taskbar view module found after init");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

// Called before the hooks are removed. From here on, no elements are added to
// new menus.
void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
}

// Called after the hooks are removed.
void Wh_ModUninit() {
    Wh_Log(L">");

    bool hasInjectedElements;
    {
        std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
        hasInjectedElements = !g_injectedElements.empty();
    }

    // The menu elements belong to the taskbar thread and can only be accessed
    // from it.
    if (hasInjectedElements) {
        HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!taskbarWnd ||
            !RunFromWindowThread(
                taskbarWnd, [](void*) { ReleaseInjectedElements(); },
                nullptr)) {
            Wh_Log(L"Failed to release the menu elements");
        }
    }
}
