// ==WindhawkMod==
// @id              taskbar-system-utilities
// @name            Taskbar System Utilities
// @description     Native XAML taskbar tools (Restart Explorer, Clean Temp, Reduce Memory, Reset GPU, Get IP, Cache, DNS) and custom launch items (programs, folders, URLs, settings).
// @version         1.3
// @author          Marco Petrucci
// @github          https://github.com/marco00petrucci
// @donateUrl       https://www.paypal.me/marco00petrucci
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -lshell32 -lws2_32 -liphlpapi -lwininet -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar System Utilities & Custom Items

System utilities and custom launch items integrated natively into the Windows 11
Taskbar XAML context menu with **native XAML flyout cards**, for built-in tools, and fully customizable user shortcuts.

![Taskbar System Utilities Screenshot](https://i.imgur.com/kOz6nZ9.gif)

## Features

### Built-in System Utilities
- **Restart Explorer**: Instantly restarts the `explorer.exe` shell process from
  the taskbar context menu.
- **Clean Temp**: Comprehensive temporary file cleaner for `%TEMP%` and
  accessible files in `%WINDIR%\Temp`. Deletes all unlocked temporary files
  regardless of age (files currently locked by active processes are skipped) and
  displays a XAML summary card.
- **Reduce Memory**: Trims the working sets of your own non-elevated running
  processes via `EmptyWorkingSet` to reclaim RAM, displaying a XAML summary card
  with the number of processes optimized and memory freed.
- **Restart Graphics Driver**: Triggers the `Win + Ctrl + Shift + B` display
  driver reset shortcut.
- **Get IP Addresses**: Lists active local IPv4 and IPv6 addresses across
  network adapters. Optionally queries public IP via `api.ipify.org` if enabled
  in settings (disabled by default for privacy). Includes clipboard copy.
- **Rebuild Icon & Thumbnail Cache**: Deletes unlocked thumbnail cache files
  from `%LOCALAPPDATA%\Microsoft\Windows\Explorer\thumbcache_*.db` and rebuilds
  the Windows icon cache via `ie4uinit.exe -show`, without restarting Explorer.
- **Clean DNS Cache**: Flushes the Windows DNS resolver cache via
  `dnsapi.dll!DnsFlushResolverCache`.
- **System Utilities Submenu**: Utilities can be grouped into a cascading
  submenu with a custom label, Segoe Fluent Icons glyph, and top/bottom
  placement. Each utility can individually be placed inside the submenu or at
  the root of the taskbar menu.

### Custom Menu Items
Add your own custom programs, folders, scripts, URLs, and settings pages to the
taskbar right-click menu:
- **Label**: Text shown in the menu. Use a single dash (`-`) to insert a
  separator line. A label with an empty command acts as a disabled section
  heading.
- **Command**: What to open: an executable (`wt.exe`, `notepad.exe`), a full
  path to a file or script, a directory, a URL (`https://example.com`), a
  settings URI (`ms-settings:display`), or a shell folder (`shell:Downloads`).
  Environment variables such as `%USERPROFILE%` are expanded automatically.
- **Arguments**: Optional command-line arguments for the command.
- **Icon**: Optional Segoe Fluent Icons glyph code (e.g. `E756`, `0xE756`,
  `\uE756`) or a single character / emoji.
- **Run as administrator**: Starts the command elevated via the Windows UAC
  prompt.
- **Hide window**: Starts the command without creating a visible window, useful
  for scripts and console commands.
- **Placement**: Place custom items below the built-in items (`bottom`), above
  them (`top`), or directly inside the System Utilities submenu (`submenu`).
- **Custom Submenu**: Optionally group custom items into their own dedicated
  cascading submenu with a custom name and icon.

## Credits & Licensing
- Taskbar context menu injection hooks adapted from `taskbar-restart-explorer`
  by **Mgrmjp** and `taskbar-autohide-in-context` by **oyrslyng**.
- Custom items management, glyph conversion, and launch thread handling adapted
  from `taskbar-context-menu-custom-items` by **jcyrio**.
- Distributed under the MIT license.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- submenu:
  - enabled: true
    $name: Group into submenu
    $description: Group the selected utilities inside a cascading submenu.
  - label: System Utilities
    $name: Submenu label
  - glyph: E712
    $name: Submenu icon glyph (Segoe Fluent Icons)
    $description: "Hex code (E712, 0xE712, \\uE712) or a single character/emoji."
  - placement: top
    $name: Utilities placement
    $description: Where the system utilities appear in the taskbar context menu.
    $options:
    - top: Above built-in items
    - bottom: Below built-in items
  $name: Submenu
  $description: Configure the cascading submenu and placement for system utilities.
- restartExplorer:
  - enabled: true
    $name: Enable in taskbar menu
  - inSubmenu: true
    $name: Place inside submenu
    $description: Show this item in the submenu (when grouping is on).
  - label: Restart Explorer
    $name: Menu item label
  $name: Restart Explorer
  $description: Configure the Restart Explorer shortcut.
- cleanTemp:
  - enabled: true
    $name: Enable in taskbar menu
  - inSubmenu: true
    $name: Place inside submenu
    $description: Show this item in the submenu (when grouping is on).
  - label: Clean Temp
    $name: Menu item label
  - cleanRecycleBin: false
    $name: Empty Recycle Bin
    $description: Permanently empty Recycle Bin on all drives.
  - showCard: true
    $name: Show summary card
    $description: Display XAML card with cleanup statistics.
  $name: Clean Temp
  $description: Configure the temporary files cleaner.
- reduceMemory:
  - enabled: true
    $name: Enable in taskbar menu
  - inSubmenu: true
    $name: Place inside submenu
    $description: Show this item in the submenu (when grouping is on).
  - label: Reduce Memory
    $name: Menu item label
  - showCard: true
    $name: Show summary card
    $description: Display XAML card with freed RAM statistics.
  $name: Reduce Memory
  $description: Trim working sets of running processes to free up RAM.
- restartGpu:
  - enabled: true
    $name: Enable in taskbar menu
  - inSubmenu: true
    $name: Place inside submenu
    $description: Show this item in the submenu (when grouping is on).
  - label: Restart Graphics Driver
    $name: Menu item label
  $name: Restart Graphics Driver
  $description: Configure the graphics subsystem reset shortcut.
- getIp:
  - enabled: true
    $name: Enable in taskbar menu
  - inSubmenu: true
    $name: Place inside submenu
    $description: Show this item in the submenu (when grouping is on).
  - label: Get IP Address
    $name: Menu item label
  - queryPublicIp: false
    $name: Query Public IP (External)
    $description: Contacts api.ipify.org via HTTPS. Off by default.
  - copyToClipboard: false
    $name: Auto-copy to clipboard
    $description: Automatically copy detected IPs to clipboard.
  $name: Get IP Address
  $description: Configure local and public IP detection.
- rebuildCache:
  - enabled: true
    $name: Enable in taskbar menu
  - inSubmenu: true
    $name: Place inside submenu
    $description: Show this item in the submenu (when grouping is on).
  - label: Rebuild Icon & Thumbnail Cache
    $name: Menu item label
  - showCard: true
    $name: Show summary card
    $description: Display XAML card with cache rebuild statistics.
  $name: Rebuild Icon & Thumbnail Cache
  $description: Delete thumbnail cache and rebuild Windows icon cache.
- flushDns:
  - enabled: true
    $name: Enable in taskbar menu
  - inSubmenu: true
    $name: Place inside submenu
    $description: Show this item in the submenu (when grouping is on).
  - label: Clean DNS Cache
    $name: Menu item label
  - showCard: true
    $name: Show confirmation card
    $description: Display XAML card when DNS cache is flushed.
  $name: Clean DNS Cache
  $description: Configure the DNS resolver cache cleaner.
- items:
  - - name: Terminal
      $name: Label
      $description: "Text shown in the menu. Use a single dash (-) to add a separator line. Leave command empty for a disabled section heading."
    - command: wt.exe
      $name: Command
      $description: "Program, file, folder, URL, ms-settings page or shell location to open, without arguments. Environment variables such as %USERPROFILE% are expanded."
    - arguments: ""
      $name: Arguments
      $description: Optional command-line arguments for the command.
    - icon: E756
      $name: Icon
      $description: "Optional hex code of a Segoe Fluent Icons glyph (e.g. E756) or a single character such as an emoji."
    - runAsAdmin: false
      $name: Run as administrator
      $description: Start the command elevated with administrative privileges (triggers UAC).
    - hideWindow: false
      $name: Hide window
      $description: Start the command without a visible window, useful for background scripts and console commands.
  - - name: Control Panel
    - command: control.exe
    - arguments: ""
    - icon: E770
    - runAsAdmin: false
    - hideWindow: false
  $name: Custom Menu Items
  $description: "Add your own custom items to the taskbar right-click menu: programs, folders, scripts, URLs and settings pages."
- placement: bottom
  $name: Custom items placement
  $description: Where the custom items appear in the taskbar context menu.
  $options:
  - bottom: Below built-in items
  - top: Above built-in items
  - submenu: Inside System Utilities submenu
- separator: true
  $name: Custom items separator line
  $description: Draw a separator line between the custom items and adjacent menu items.
- submenuName: ""
  $name: Custom items submenu name
  $description: If set, custom items are grouped into a dedicated submenu with this name instead of being listed directly in the menu.
- submenuIcon: E71D
  $name: Custom items submenu icon
  $description: "Optional hex code of a Segoe Fluent Icons glyph (e.g. E71D) or a single character/emoji for the custom items submenu."
*/
// ==/WindhawkModSettings==

// clang-format off
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <psapi.h>
#include <shellapi.h>
#include <wininet.h>
// clang-format on

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

#include <atomic>
#include <cctype>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wu = winrt::Windows::UI;
namespace wut = winrt::Windows::UI::Text;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

using MenuItemsVector = wfc::IVector<wuxc::MenuFlyoutItemBase>;

// ============================================================================
// Settings & Configuration Structures
// ============================================================================

enum class UtilitiesPlacement {
    Top,
    Bottom,
};

enum class CustomPlacement {
    Bottom,
    Top,
    Submenu,
};

struct CustomMenuItemSettings {
    std::wstring name;
    std::wstring command;
    std::wstring arguments;
    std::wstring icon;
    bool runAsAdmin = false;
    bool hideWindow = false;

    // Returns the text displayed in the menu and log messages.
    // An item without an explicit label displays its command string.
    const std::wstring& DisplayName() const {
        return name.empty() ? command : name;
    }
};

struct ModSettings {
    // System Utilities Submenu & Placement
    bool enableSubmenu = true;
    std::wstring textSubmenu = L"System Utilities";
    std::wstring glyphSubmenu = L"\xE712";
    UtilitiesPlacement utilitiesPlacement = UtilitiesPlacement::Top;

    // 1. Restart Explorer
    bool showRestartExplorer = true;
    bool inSubmenuRestartExplorer = true;
    std::wstring textRestartExplorer = L"Restart Explorer";

    // 2. Clean Temp
    bool showCleanTemp = true;
    bool inSubmenuCleanTemp = true;
    std::wstring textCleanTemp = L"Clean Temp";
    bool cleanRecycleBin = false;
    bool notifyCleanTemp = true;

    // 3. Reduce Memory
    bool showReduceMemory = true;
    bool inSubmenuReduceMemory = true;
    std::wstring textReduceMemory = L"Reduce Memory";
    bool notifyReduceMemory = true;

    // 4. Restart Graphics Driver
    bool showRestartGpu = true;
    bool inSubmenuRestartGpu = true;
    std::wstring textRestartGpu = L"Restart Graphics Driver";

    // 5. Get IP Address
    bool showGetIp = true;
    bool inSubmenuGetIp = true;
    std::wstring textGetIp = L"Get IP Address";
    bool queryPublicIp = false;
    bool copyIpToClipboard = false;

    // 6. Rebuild Icon & Thumbnail Cache
    bool showRebuildCache = true;
    bool inSubmenuRebuildCache = true;
    std::wstring textRebuildCache = L"Rebuild Icon & Thumbnail Cache";
    bool notifyRebuildCache = true;

    // 7. Clean DNS Cache
    bool showFlushDns = true;
    bool inSubmenuFlushDns = true;
    std::wstring textFlushDns = L"Clean DNS Cache";
    bool notifyFlushDns = true;

    // Custom Menu Items
    std::vector<CustomMenuItemSettings> customItems;
    CustomPlacement customPlacement = CustomPlacement::Bottom;
    bool customSeparator = true;
    std::wstring customSubmenuName;
    std::wstring customSubmenuIcon = L"E71D";
};

// Settings are loaded on a Windhawk thread and read on the taskbar UI thread,
// so they are published as an immutable shared_ptr snapshot.
std::mutex g_settingsMutex;
std::shared_ptr<const ModSettings> g_settings;

std::shared_ptr<const ModSettings> GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// ============================================================================
// Global State & Thread / Window Synchronization
// ============================================================================

// Identifier assigned to every element injected by the mod.
static constexpr wchar_t kInjectedElementName[] =
    L"WindhawkCustomTaskbarMenuItem";

std::atomic<bool> g_taskbarViewModuleHooked{false};
std::atomic<bool> g_restartStarted{false};
std::atomic<bool> g_unloading{false};

// Weak references to XAML UI objects, accessed exclusively on the taskbar UI
// thread.
winrt::weak_ref<wux::FrameworkElement> g_lastTargetWeak;
winrt::weak_ref<wuxc::Flyout> g_activeFlyoutWeak;
winrt::weak_ref<wuxc::Button> g_activeOkButtonWeak;
winrt::event_token g_activeOkClickToken{};

// Background task handles for built-in system utilities.
std::mutex g_threadsMutex;
std::vector<HANDLE> g_threads;
std::mutex g_dispatchMutex;

// Background thread handles for custom item command execution.
std::mutex g_launchThreadsMutex;
std::vector<HANDLE> g_launchThreads;

// Finds the primary taskbar window (Shell_TrayWnd) belonging to the current
// process.
HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId = 0;
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId()) {
                WCHAR szClassName[32];
                if (GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) &&
                    _wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                    *reinterpret_cast<HWND*>(lParam) = hWnd;
                    return FALSE;
                }
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

// Executes a callback synchronously on the thread owning the specified window.
bool RunFromWindowThread(HWND hWnd, std::function<void()> proc) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!dwThreadId) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc();
        return true;
    }

    static const UINT runMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct CallWndProcParam {
        std::function<void()>* proc;
    };

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runMsg) {
                    auto* param =
                        reinterpret_cast<CallWndProcParam*>(cwp->lParam);
                    (*param->proc)();
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    CallWndProcParam param{&proc};
    SendMessageW(hWnd, runMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

// Runs a background task on a tracked worker thread so Wh_ModUninit can wait
// for completion before unloading the mod DLL.
template <typename F>
void RunAsyncTracked(F&& fn) {
    std::lock_guard<std::mutex> lock(g_threadsMutex);
    if (g_unloading.load()) {
        return;
    }

    for (auto it = g_threads.begin(); it != g_threads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_threads.erase(it);
        } else {
            ++it;
        }
    }

    auto task = std::make_unique<std::function<void()>>(std::forward<F>(fn));
    auto* rawTask = task.release();

    HANDLE thread = CreateThread(
        nullptr, 0,
        [](void* p) -> DWORD {
            std::unique_ptr<std::function<void()>> taskPtr(
                static_cast<std::function<void()>*>(p));
            try {
                (*taskPtr)();
            } catch (...) {
                Wh_Log(L"Background task threw an exception");
            }
            return 0;
        },
        rawTask, 0, nullptr);

    if (thread) {
        g_threads.push_back(thread);
    } else {
        delete rawTask;
    }
}

// ============================================================================
// Icon & Glyph Helpers
// ============================================================================

bool IsHexDigit(wchar_t ch) {
    return (ch >= L'0' && ch <= L'9') || (ch >= L'a' && ch <= L'f') ||
           (ch >= L'A' && ch <= L'F');
}

// Converts an icon configuration string into a UTF-16 glyph string.
// Accepts hex code points ("E756", "0xE756", "U+E756", "\uE756", "\xE756")
// as well as literal characters or emojis.
std::wstring GlyphFromIconSetting(const std::wstring& icon, bool* isCodePoint) {
    *isCodePoint = false;

    std::wstring_view hex = icon;
    if (hex.size() > 2 &&
        (hex.starts_with(L"0x") || hex.starts_with(L"0X") ||
         hex.starts_with(L"U+") || hex.starts_with(L"u+") ||
         hex.starts_with(L"\\u") || hex.starts_with(L"\\U") ||
         hex.starts_with(L"\\x") || hex.starts_with(L"\\X"))) {
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

wuxc::FontIcon CreateFluentIcon(const wchar_t* glyph, double size = 16.0) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons,Segoe MDL2 Assets"));
    icon.Glyph(glyph);
    icon.FontSize(size);
    return icon;
}

wuxc::FontIcon CreateIconFromSetting(const std::wstring& iconSetting,
                                     double size = 16.0) {
    bool isCodePoint = false;
    std::wstring glyph = GlyphFromIconSetting(iconSetting, &isCodePoint);

    wuxc::FontIcon icon;
    if (isCodePoint) {
        icon.FontFamily(
            wuxm::FontFamily(L"Segoe Fluent Icons,Segoe MDL2 Assets"));
    } else {
        icon.FontFamily(wuxm::FontFamily(L"Segoe UI Emoji,Segoe UI"));
    }
    icon.FontSize(size);
    icon.Glyph(glyph);
    return icon;
}

// ============================================================================
// Custom Items Process Launcher
// ============================================================================

struct LaunchRequest {
    std::wstring name;
    std::wstring command;
    std::wstring arguments;
    bool runAsAdmin = false;
    bool hideWindow = false;
};

// Expands environment variable references such as %USERPROFILE% or %WINDIR%.
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

    expanded.resize(written - 1);
    return expanded;
}

// Removes surrounding quotes from paths copied via File Explorer's "Copy as
// path".
std::wstring StripSurroundingQuotes(const std::wstring& value) {
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"' &&
        value.find(L'"', 1) == value.size() - 1) {
        return value.substr(1, value.size() - 2);
    }

    return value;
}

DWORD WINAPI LaunchThread(LPVOID param) {
    std::unique_ptr<LaunchRequest> request(static_cast<LaunchRequest*>(param));

    HRESULT hrCoInit = CoInitializeEx(
        nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    std::wstring command =
        StripSurroundingQuotes(ExpandEnvironment(request->command));
    std::wstring arguments = ExpandEnvironment(request->arguments);

    // Use the user's profile directory as the working directory rather than
    // System32.
    std::wstring directory = ExpandEnvironment(L"%USERPROFILE%");

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOASYNC;
    sei.lpVerb = request->runAsAdmin ? L"runas" : nullptr;
    sei.lpFile = command.c_str();
    sei.lpParameters = arguments.empty() ? nullptr : arguments.c_str();
    sei.lpDirectory = directory.empty() ? nullptr : directory.c_str();
    sei.nShow = request->hideWindow ? SW_HIDE : SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei)) {
        Wh_Log(L"Started \"%s\"", request->name.c_str());
    } else {
        DWORD error = GetLastError();
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

    return 0;
}

// Launches a custom item command on a separate thread so slow startups or UAC
// prompts do not block the taskbar UI thread.
void Launch(const CustomMenuItemSettings& item) {
    auto request = std::make_unique<LaunchRequest>();
    request->name = item.DisplayName();
    request->command = item.command;
    request->arguments = item.arguments;
    request->runAsAdmin = item.runAsAdmin;
    request->hideWindow = item.hideWindow;

    std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
    if (g_unloading.load()) {
        return;
    }

    std::erase_if(g_launchThreads, [](HANDLE thread) {
        if (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0) {
            return false;
        }

        CloseHandle(thread);
        return true;
    });

    auto* rawRequest = request.release();
    HANDLE thread =
        CreateThread(nullptr, 0, LaunchThread, rawRequest, 0, nullptr);
    if (!thread) {
        delete rawRequest;
        Wh_Log(L"CreateThread failed, error %lu", GetLastError());
        return;
    }

    g_launchThreads.push_back(thread);
}

void WaitForLaunchThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        threads.swap(g_launchThreads);
    }

    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

// ============================================================================
// Injected Menu Elements Lifetime & Unload Detachment
// ============================================================================

// Tracks menu elements injected by the mod using weak references so that closed
// menus can be garbage-collected normally while remaining detachable on unload.
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

void PruneInjectedElements() {
    std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
    std::erase_if(g_injectedElements, [](const InjectedElement& entry) {
        return !entry.element.get();
    });
}

// Removes click handlers and collapses injected elements on the taskbar UI
// thread when the mod is being unloaded.
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

wuxc::MenuFlyoutItemBase CreateCustomMenuItem(
    const CustomMenuItemSettings& settings) {
    if (settings.name == L"-") {
        return CreateSeparator();
    }

    wuxc::MenuFlyoutItem item;
    item.Name(kInjectedElementName);
    item.Text(settings.DisplayName());

    if (!settings.icon.empty()) {
        item.Icon(CreateIconFromSetting(settings.icon, 16.0));
    }

    InjectedElement tracked;
    tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(item);

    if (settings.command.empty()) {
        // An item with a label and an empty command acts as a disabled heading.
        item.IsEnabled(false);
    } else {
        tracked.clickToken = item.Click(
            [settings](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                Wh_Log(L"Clicked \"%s\"", settings.DisplayName().c_str());
                Launch(settings);
            });
        tracked.hasClickHandler = true;
    }

    TrackInjectedElement(std::move(tracked));
    return item;
}

// ============================================================================
// Native XAML Flyout Card Builder (Theme-Aware & Unload-Safe)
// ============================================================================

using DialogRow = std::pair<std::wstring, std::wstring>;

wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(a, r, g, b));
}

void CloseActiveFlyoutOnUIThread() {
    try {
        if (auto btn = g_activeOkButtonWeak.get()) {
            if (g_activeOkClickToken) {
                btn.Click(g_activeOkClickToken);
            }
        }
    } catch (...) {
    }
    g_activeOkButtonWeak = nullptr;
    g_activeOkClickToken = {};

    try {
        if (auto fl = g_activeFlyoutWeak.get()) {
            fl.Hide();
        }
    } catch (...) {
    }
    g_activeFlyoutWeak = nullptr;
}

void ShowModernXAMLCardOnUIThread(const std::wstring& titleText,
                                  const wchar_t* glyphIcon,
                                  const std::vector<DialogRow>& rows,
                                  const std::wstring& footerNote = L"") {
    if (g_unloading.load()) {
        return;
    }

    try {
        CloseActiveFlyoutOnUIThread();

        auto target = g_lastTargetWeak.get();
        if (!target || !target.XamlRoot()) {
            Wh_Log(L"Cannot display XAML card: target or XamlRoot unavailable");
            return;
        }

        bool isLight = (target.ActualTheme() == wux::ElementTheme::Light);

        wuxc::Flyout flyout;
        flyout.ShouldConstrainToRootBounds(false);
        flyout.Placement(wuxcp::FlyoutPlacementMode::Top);
        flyout.XamlRoot(target.XamlRoot());

        // Remove default FlyoutPresenter padding and borders for a clean card
        // layout.
        wux::Interop::TypeName presenterType{
            winrt::hstring{winrt::name_of<wuxc::FlyoutPresenter>()},
            wux::Interop::TypeKind::Metadata};
        wux::Style presenterStyle(presenterType);
        presenterStyle.Setters().Append(
            wux::Setter(wuxc::Control::PaddingProperty(),
                        winrt::box_value(wux::Thickness{0, 0, 0, 0})));
        presenterStyle.Setters().Append(
            wux::Setter(wuxc::Control::BorderThicknessProperty(),
                        winrt::box_value(wux::Thickness{0, 0, 0, 0})));
        presenterStyle.Setters().Append(wux::Setter(
            wuxc::Control::BackgroundProperty(), MakeBrush(0, 0, 0, 0)));
        flyout.FlyoutPresenterStyle(presenterStyle);

        wuxc::Border card;
        card.Background(isLight ? MakeBrush(255, 249, 249, 249)
                                : MakeBrush(255, 32, 32, 32));
        card.BorderBrush(isLight ? MakeBrush(255, 218, 218, 218)
                                 : MakeBrush(255, 58, 58, 58));
        card.BorderThickness({1, 1, 1, 1});
        card.CornerRadius({12, 12, 12, 12});
        card.Padding({22, 20, 22, 18});
        card.Width(360);

        wuxc::StackPanel mainStack;
        mainStack.Spacing(14);

        // 1. Header Row (Accent Icon Badge + Title)
        wuxc::StackPanel headerStack;
        headerStack.Orientation(wuxc::Orientation::Horizontal);
        headerStack.Spacing(12);

        wuxc::Border iconBadge;
        iconBadge.Background(isLight ? MakeBrush(28, 0, 103, 192)
                                     : MakeBrush(35, 96, 205, 255));
        iconBadge.CornerRadius({8, 8, 8, 8});
        iconBadge.Width(36);
        iconBadge.Height(36);

        auto headerIcon = CreateFluentIcon(glyphIcon, 18.0);
        headerIcon.Foreground(isLight ? MakeBrush(255, 0, 103, 192)
                                      : MakeBrush(255, 96, 205, 255));
        headerIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        headerIcon.VerticalAlignment(wux::VerticalAlignment::Center);
        iconBadge.Child(headerIcon);
        headerStack.Children().Append(iconBadge);

        wuxc::TextBlock titleBlock;
        titleBlock.Text(titleText);
        titleBlock.FontWeight(wut::FontWeights::SemiBold());
        titleBlock.FontSize(16);
        titleBlock.Foreground(isLight ? MakeBrush(255, 26, 26, 26)
                                      : MakeBrush(255, 255, 255, 255));
        titleBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        headerStack.Children().Append(titleBlock);

        mainStack.Children().Append(headerStack);

        // 2. Metrics Box (Key-Value Rows)
        wuxc::Border metricsBox;
        metricsBox.Background(isLight ? MakeBrush(255, 240, 240, 240)
                                      : MakeBrush(255, 43, 43, 43));
        metricsBox.BorderBrush(isLight ? MakeBrush(255, 226, 226, 226)
                                       : MakeBrush(255, 53, 53, 53));
        metricsBox.BorderThickness({1, 1, 1, 1});
        metricsBox.CornerRadius({8, 8, 8, 8});
        metricsBox.Padding({14, 12, 14, 12});

        wuxc::StackPanel rowsStack;
        rowsStack.Spacing(8);

        for (const auto& [label, val] : rows) {
            wuxc::Grid rowGrid;
            wuxc::ColumnDefinition col0;
            col0.Width(wux::GridLength{1.0, wux::GridUnitType::Auto});
            wuxc::ColumnDefinition col1;
            col1.Width(wux::GridLength{1.0, wux::GridUnitType::Star});
            rowGrid.ColumnDefinitions().Append(col0);
            rowGrid.ColumnDefinitions().Append(col1);

            wuxc::TextBlock lblBlock;
            lblBlock.Text(label);
            lblBlock.FontSize(13);
            lblBlock.Foreground(isLight ? MakeBrush(255, 92, 92, 92)
                                        : MakeBrush(255, 175, 175, 175));
            lblBlock.VerticalAlignment(wux::VerticalAlignment::Center);
            lblBlock.Margin({0, 0, 16, 0});
            rowGrid.Children().Append(lblBlock);
            wuxc::Grid::SetColumn(lblBlock, 0);

            wuxc::TextBlock valBlock;
            valBlock.Text(val);
            valBlock.FontSize(13);
            valBlock.FontWeight(wut::FontWeights::SemiBold());
            valBlock.Foreground(isLight ? MakeBrush(255, 26, 26, 26)
                                        : MakeBrush(255, 245, 245, 245));
            valBlock.HorizontalAlignment(wux::HorizontalAlignment::Right);
            valBlock.VerticalAlignment(wux::VerticalAlignment::Center);
            valBlock.TextAlignment(wux::TextAlignment::Right);
            valBlock.TextWrapping(wux::TextWrapping::Wrap);
            valBlock.IsTextSelectionEnabled(true);
            rowGrid.Children().Append(valBlock);
            wuxc::Grid::SetColumn(valBlock, 1);

            rowsStack.Children().Append(rowGrid);
        }

        metricsBox.Child(rowsStack);
        mainStack.Children().Append(metricsBox);

        // 3. Footer Bar (Status Note + OK Button)
        wuxc::Grid footerGrid;

        if (!footerNote.empty()) {
            wuxc::StackPanel noteStack;
            noteStack.Orientation(wuxc::Orientation::Horizontal);
            noteStack.Spacing(6);
            noteStack.VerticalAlignment(wux::VerticalAlignment::Center);

            auto checkIcon = CreateFluentIcon(L"\xE73E", 13.0);
            checkIcon.Foreground(isLight ? MakeBrush(255, 15, 123, 15)
                                         : MakeBrush(255, 108, 203, 95));
            noteStack.Children().Append(checkIcon);

            wuxc::TextBlock noteBlock;
            noteBlock.Text(footerNote);
            noteBlock.FontSize(12);
            noteBlock.Foreground(isLight ? MakeBrush(255, 100, 100, 100)
                                         : MakeBrush(255, 160, 160, 160));
            noteStack.Children().Append(noteBlock);

            footerGrid.Children().Append(noteStack);
        }

        wuxc::Button okButton;
        okButton.Content(winrt::box_value(L"OK"));
        okButton.Width(76);
        okButton.Height(30);
        okButton.CornerRadius({6, 6, 6, 6});
        okButton.Background(MakeBrush(255, 0, 120, 212));
        okButton.Foreground(MakeBrush(255, 255, 255, 255));
        okButton.BorderThickness({0, 0, 0, 0});
        okButton.HorizontalAlignment(wux::HorizontalAlignment::Right);

        winrt::weak_ref<wuxc::Flyout> weakFlyout(flyout);
        g_activeFlyoutWeak = flyout;
        g_activeOkButtonWeak = okButton;
        g_activeOkClickToken = okButton.Click(
            [weakFlyout](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (auto fl = weakFlyout.get()) {
                    fl.Hide();
                }
            });

        footerGrid.Children().Append(okButton);
        mainStack.Children().Append(footerGrid);

        card.Child(mainStack);
        flyout.Content(card);
        flyout.ShowAt(target);
    } catch (...) {
        Wh_Log(L"Failed to display XAML Flyout card");
    }
}

void DispatchXAMLCardSync(const std::wstring& titleText,
                          const wchar_t* glyphIcon,
                          const std::vector<DialogRow>& rows,
                          const std::wstring& footerNote = L"") {
    std::lock_guard<std::mutex> lock(g_dispatchMutex);
    if (g_unloading.load()) {
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    RunFromWindowThread(hTaskbarWnd, [&]() {
        if (!g_unloading.load()) {
            ShowModernXAMLCardOnUIThread(titleText, glyphIcon, rows,
                                         footerNote);
        }
    });
}

bool CopyTextToClipboard(const std::wstring& text) {
    HWND tempOwner = CreateWindowExW(0, L"STATIC", nullptr, 0, 0, 0, 0, 0,
                                     HWND_MESSAGE, nullptr, nullptr, nullptr);
    HWND owner = tempOwner ? tempOwner : GetShellWindow();

    bool opened = false;
    for (int i = 0; i < 6 && !opened; ++i) {
        opened = OpenClipboard(owner) != FALSE;
        if (!opened) {
            Sleep(25);
        }
    }

    bool copied = false;
    if (opened) {
        EmptyClipboard();
        size_t byteSize = (text.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, byteSize);
        if (hMem) {
            if (void* ptr = GlobalLock(hMem)) {
                memcpy(ptr, text.c_str(), byteSize);
                GlobalUnlock(hMem);
                if (SetClipboardData(CF_UNICODETEXT, hMem)) {
                    copied = true;
                } else {
                    GlobalFree(hMem);
                }
            } else {
                GlobalFree(hMem);
            }
        }
        CloseClipboard();
    }

    if (tempOwner) {
        DestroyWindow(tempOwner);
    }
    return copied;
}

// ============================================================================
// Native Action Implementations (System Utilities)
// ============================================================================

// 1. Restart Explorer
void OnRestartExplorerClick(wf::IInspectable const&,
                            wux::RoutedEventArgs const&) {
    if (g_restartStarted.exchange(true)) {
        return;
    }

    wchar_t sysDir[MAX_PATH] = {};
    if (GetSystemDirectoryW(sysDir, MAX_PATH) == 0) {
        g_restartStarted = false;
        return;
    }

    std::wstring cmdPath = std::wstring(sysDir) + L"\\cmd.exe";
    wchar_t cmdArgs[] =
        L"/d /c \"taskkill /f /im explorer.exe >nul 2>nul & start \"\" "
        L"explorer.exe\"";

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {};

    if (CreateProcessW(cmdPath.c_str(), cmdArgs, nullptr, nullptr, FALSE,
                       CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, nullptr,
                       nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        g_restartStarted = false;
    }
}

// 2. Clean Temp
struct CleanStats {
    DWORD filesDeleted = 0;
    DWORD filesSkipped = 0;
    ULONGLONG bytesFreed = 0;
};

std::wstring ToExtendedPath(const std::wstring& path) {
    if (path.rfind(L"\\\\?\\", 0) == 0) {
        return path;
    }
    return L"\\\\?\\" + path;
}

bool IsSafeCleanTarget(const std::wstring& path) {
    if (path.size() < 4) {
        return false;
    }

    auto equalsEnv = [&](const wchar_t* var) {
        wchar_t buf[MAX_PATH] = {};
        DWORD n = GetEnvironmentVariableW(var, buf, MAX_PATH);
        if (n == 0 || n >= MAX_PATH) {
            return false;
        }
        std::wstring v(buf);
        while (!v.empty() && v.back() == L'\\') {
            v.pop_back();
        }
        return _wcsicmp(v.c_str(), path.c_str()) == 0;
    };

    return !(equalsEnv(L"USERPROFILE") || equalsEnv(L"SystemRoot") ||
             equalsEnv(L"SystemDrive"));
}

void CleanDirectoryRecursive(const std::wstring& dirPath,
                             bool removeRoot,
                             CleanStats& stats) {
    if (g_unloading.load()) {
        return;
    }

    std::wstring searchMask = dirPath + L"\\*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchMask.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }

        std::wstring fullPath = dirPath + L"\\" + fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                CleanDirectoryRecursive(fullPath, true, stats);
            }
        } else {
            ULONGLONG fileSize =
                (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) |
                fd.nFileSizeLow;
            if (DeleteFileW(fullPath.c_str())) {
                stats.filesDeleted++;
                stats.bytesFreed += fileSize;
            } else {
                stats.filesSkipped++;
            }
        }
    } while (!g_unloading.load() && FindNextFileW(hFind, &fd));

    FindClose(hFind);

    if (removeRoot && !g_unloading.load()) {
        RemoveDirectoryW(dirPath.c_str());
    }
}

void OnCleanTempClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();
        CleanStats stats;

        wchar_t tempPath[MAX_PATH] = {};
        if (GetTempPathW(MAX_PATH, tempPath) > 0) {
            std::wstring userTemp(tempPath);
            while (!userTemp.empty() && userTemp.back() == L'\\') {
                userTemp.pop_back();
            }
            if (IsSafeCleanTarget(userTemp)) {
                CleanDirectoryRecursive(ToExtendedPath(userTemp), false, stats);
            } else {
                Wh_Log(L"Temp path rejected by safety check: %s",
                       userTemp.c_str());
            }
        }

        wchar_t winDir[MAX_PATH] = {};
        if (GetWindowsDirectoryW(winDir, MAX_PATH) >= 3) {
            std::wstring sysTemp = std::wstring(winDir) + L"\\Temp";
            CleanDirectoryRecursive(ToExtendedPath(sysTemp), false, stats);
        }

        if (cfg->cleanRecycleBin && !g_unloading.load()) {
            SHEmptyRecycleBinW(
                nullptr, nullptr,
                SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
        }

        if (cfg->notifyCleanTemp && !g_unloading.load()) {
            double mbFreed =
                static_cast<double>(stats.bytesFreed) / (1024.0 * 1024.0);
            wchar_t mbBuf[64] = {};
            swprintf_s(mbBuf, L"%.2f MB", mbFreed);

            std::vector<DialogRow> rows = {
                {L"Deleted files", std::to_wstring(stats.filesDeleted)},
                {L"Locked / skipped", std::to_wstring(stats.filesSkipped)},
                {L"Space recovered", mbBuf},
            };

            std::wstring footer =
                (stats.filesDeleted == 0 && stats.filesSkipped == 0)
                    ? L"No removable temp files found"
                    : L"Cleanup completed";

            DispatchXAMLCardSync(L"Temporary Files Cleaned", L"\xE74D", rows,
                                 footer);
        }
    });
}

// 3. Reduce Memory
void OnReduceMemoryClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();

        MEMORYSTATUSEX memBefore = {sizeof(memBefore)};
        GlobalMemoryStatusEx(&memBefore);

        std::vector<DWORD> pids(1024);
        DWORD bytesReturned = 0;
        while (K32EnumProcesses(pids.data(),
                                static_cast<DWORD>(pids.size() * sizeof(DWORD)),
                                &bytesReturned)) {
            if (bytesReturned < pids.size() * sizeof(DWORD)) {
                break;
            }
            pids.resize(pids.size() * 2);
        }

        DWORD currentPid = GetCurrentProcessId();
        DWORD count = bytesReturned / sizeof(DWORD);
        DWORD trimmedCount = 0;
        DWORD skippedCount = 0;

        for (DWORD i = 0; i < count && !g_unloading.load(); ++i) {
            if (pids[i] == 0 || pids[i] == 4 || pids[i] == currentPid) {
                continue;
            }

            HANDLE hProc = OpenProcess(
                PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_QUOTA, FALSE,
                pids[i]);
            if (hProc) {
                if (K32EmptyWorkingSet(hProc)) {
                    trimmedCount++;
                } else {
                    skippedCount++;
                }
                CloseHandle(hProc);
            } else {
                skippedCount++;
            }
        }

        if (cfg->notifyReduceMemory && !g_unloading.load()) {
            MEMORYSTATUSEX memAfter = {sizeof(memAfter)};
            GlobalMemoryStatusEx(&memAfter);

            double freedMb = 0.0;
            if (memAfter.ullAvailPhys > memBefore.ullAvailPhys) {
                freedMb = static_cast<double>(memAfter.ullAvailPhys -
                                              memBefore.ullAvailPhys) /
                          (1024.0 * 1024.0);
            }
            double availGb = static_cast<double>(memAfter.ullAvailPhys) /
                             (1024.0 * 1024.0 * 1024.0);

            wchar_t freedBuf[64] = {};
            swprintf_s(freedBuf, L"%.1f MB", freedMb);

            wchar_t availBuf[64] = {};
            swprintf_s(availBuf, L"%.2f GB (%lu%% used)", availGb,
                       memAfter.dwMemoryLoad);

            std::vector<DialogRow> rows = {
                {L"Processes optimized", std::to_wstring(trimmedCount)},
                {L"Processes skipped", std::to_wstring(skippedCount)},
                {L"RAM recovered", freedBuf},
                {L"Available RAM", availBuf},
            };

            DispatchXAMLCardSync(L"Memory Optimized", L"\xE950", rows,
                                 L"Working sets trimmed");
        }
    });
}

// 4. Restart Graphics Driver
void OnRestartGpuClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        Sleep(150);
        if (g_unloading.load()) {
            return;
        }

        static constexpr WORD kKeys[] = {VK_LWIN, VK_CONTROL, VK_SHIFT, 'B'};
        INPUT inputs[8] = {};

        for (size_t i = 0; i < 4; ++i) {
            inputs[i].type = INPUT_KEYBOARD;
            inputs[i].ki.wVk = kKeys[i];

            inputs[i + 4].type = INPUT_KEYBOARD;
            inputs[i + 4].ki.wVk = kKeys[3 - i];
            inputs[i + 4].ki.dwFlags = KEYEVENTF_KEYUP;
        }

        if (SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT)) !=
            ARRAYSIZE(inputs)) {
            Wh_Log(L"SendInput failed: %lu", GetLastError());
        }
    });
}

// 5. Get IP (Preferred Local Adapters + Optional Public IP)
std::vector<DialogRow> GetLocalAdapterIPs(std::wstring& clipboardText) {
    std::vector<DialogRow> rows;
    clipboardText.clear();

    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                  GAA_FLAG_SKIP_DNS_SERVER;
    ULONG outBufLen = 15000;
    std::vector<BYTE> buffer(outBufLen);

    DWORD dwRetVal = GetAdaptersAddresses(
        AF_UNSPEC, flags, nullptr,
        reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(outBufLen);
        dwRetVal = GetAdaptersAddresses(
            AF_UNSPEC, flags, nullptr,
            reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data()), &outBufLen);
    }

    if (dwRetVal != NO_ERROR) {
        rows.emplace_back(L"Status", L"Failed to query adapters");
        return rows;
    }

    for (auto* adapter = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
         adapter != nullptr; adapter = adapter->Next) {
        if (adapter->OperStatus != IfOperStatusUp ||
            adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK) {
            continue;
        }

        bool hasIpv4 = false;
        bool hasIpv6 = false;

        for (auto* unicast = adapter->FirstUnicastAddress; unicast != nullptr;
             unicast = unicast->Next) {
            if (unicast->DadState != IpDadStatePreferred) {
                continue;
            }

            SOCKADDR* addr = unicast->Address.lpSockaddr;
            if (!addr) {
                continue;
            }

            wchar_t ipStr[INET6_ADDRSTRLEN] = {};
            if (addr->sa_family == AF_INET && !hasIpv4) {
                auto* sa4 = reinterpret_cast<sockaddr_in*>(addr);
                const auto* bytes =
                    reinterpret_cast<const uint8_t*>(&sa4->sin_addr);
                // Ignore link-local APIPA addresses (169.254.0.0/16).
                if (bytes[0] == 169 && bytes[1] == 254) {
                    continue;
                }
                if (InetNtopW(AF_INET, &sa4->sin_addr, ipStr,
                              ARRAYSIZE(ipStr))) {
                    hasIpv4 = true;
                }
            } else if (addr->sa_family == AF_INET6 && !hasIpv6) {
                auto* sa6 = reinterpret_cast<sockaddr_in6*>(addr);
                // Ignore link-local IPv6 addresses (fe80::/10).
                if (IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
                    continue;
                }
                if (InetNtopW(AF_INET6, &sa6->sin6_addr, ipStr,
                              ARRAYSIZE(ipStr))) {
                    hasIpv6 = true;
                }
            }

            if (ipStr[0] != L'\0') {
                std::wstring label =
                    adapter->FriendlyName ? adapter->FriendlyName : L"Adapter";
                if (addr->sa_family == AF_INET6) {
                    label += L" (IPv6)";
                }
                rows.emplace_back(label, ipStr);
                if (!clipboardText.empty()) {
                    clipboardText += L"\r\n";
                }
                clipboardText += label + L": " + ipStr;
            }
        }
    }

    if (rows.empty()) {
        rows.emplace_back(L"Network", L"Disconnected");
    }

    return rows;
}

std::mutex g_inetMutex;
HINTERNET g_activeInternet = nullptr;

void CancelActivePublicIpRequest() {
    std::lock_guard<std::mutex> lock(g_inetMutex);
    if (g_activeInternet) {
        InternetCloseHandle(g_activeInternet);
        g_activeInternet = nullptr;
    }
}

std::wstring GetPublicIP() {
    std::wstring result = L"Unavailable";
    HINTERNET hInternet =
        InternetOpenW(L"WindhawkSystemUtilities/" WH_MOD_VERSION,
                      INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) {
        return result;
    }

    {
        std::lock_guard<std::mutex> lock(g_inetMutex);
        if (g_unloading.load()) {
            InternetCloseHandle(hInternet);
            return result;
        }
        g_activeInternet = hInternet;
    }

    DWORD timeout = 2500;
    InternetSetOptionW(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                       sizeof(timeout));
    InternetSetOptionW(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                       sizeof(timeout));

    HINTERNET hConnect =
        InternetOpenUrlW(hInternet, L"https://api.ipify.org", nullptr, 0,
                         INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
                             INTERNET_FLAG_SECURE,
                         0);
    if (hConnect) {
        char buffer[64] = {};
        DWORD bytesRead = 0;
        if (!g_unloading.load() &&
            InternetReadFile(hConnect, buffer, sizeof(buffer) - 1,
                             &bytesRead) &&
            bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string raw(buffer);
            while (!raw.empty() &&
                   std::isspace(static_cast<unsigned char>(raw.back()))) {
                raw.pop_back();
            }

            in_addr addr4;
            in6_addr addr6;
            if (InetPtonA(AF_INET, raw.c_str(), &addr4) == 1 ||
                InetPtonA(AF_INET6, raw.c_str(), &addr6) == 1) {
                wchar_t wbuffer[64] = {};
                MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), -1, wbuffer, 64);
                result = wbuffer;
            }
        }
        InternetCloseHandle(hConnect);
    }

    {
        std::lock_guard<std::mutex> lock(g_inetMutex);
        if (g_activeInternet == hInternet) {
            g_activeInternet = nullptr;
            InternetCloseHandle(hInternet);
        }
    }
    return result;
}

void OnGetIpClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();

        std::wstring clipboardText;
        std::vector<DialogRow> rows = GetLocalAdapterIPs(clipboardText);

        if (cfg->queryPublicIp && !g_unloading.load()) {
            std::wstring publicIp = GetPublicIP();
            rows.insert(rows.begin(), {L"Public IP", publicIp});
            if (!clipboardText.empty()) {
                clipboardText =
                    L"Public IP: " + publicIp + L"\r\n" + clipboardText;
            } else {
                clipboardText = L"Public IP: " + publicIp;
            }
        }

        if (g_unloading.load()) {
            return;
        }

        std::wstring footer;
        if (cfg->copyIpToClipboard && !clipboardText.empty()) {
            if (CopyTextToClipboard(clipboardText)) {
                footer = L"Copied to clipboard";
            }
        }

        DispatchXAMLCardSync(L"Network IP Addresses", L"\xE968", rows, footer);
    });
}

// 6. Rebuild Icon & Thumbnail Cache
void OnRebuildCacheClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        auto cfg = GetSettingsSnapshot();

        DWORD thumbsDeleted = 0;
        DWORD thumbsSkipped = 0;
        ULONGLONG bytesFreed = 0;

        wchar_t localAppData[MAX_PATH] = {};
        DWORD localLen =
            GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, MAX_PATH);
        if (localLen > 0 && localLen < MAX_PATH) {
            std::wstring explorerDir =
                std::wstring(localAppData) + L"\\Microsoft\\Windows\\Explorer";

            std::wstring searchMask = explorerDir + L"\\thumbcache_*.db";
            WIN32_FIND_DATAW fd;
            HANDLE hFind = FindFirstFileW(searchMask.c_str(), &fd);
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    if (g_unloading.load()) {
                        break;
                    }
                    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        continue;
                    }

                    std::wstring fullPath = explorerDir + L"\\" + fd.cFileName;
                    ULONGLONG fileSize =
                        (static_cast<ULONGLONG>(fd.nFileSizeHigh) << 32) |
                        fd.nFileSizeLow;

                    if (DeleteFileW(ToExtendedPath(fullPath).c_str())) {
                        thumbsDeleted++;
                        bytesFreed += fileSize;
                    } else {
                        thumbsSkipped++;
                    }
                } while (FindNextFileW(hFind, &fd));
                FindClose(hFind);
            }
        }

        // Rebuild icon cache without restarting Explorer via ie4uinit.exe
        // -show.
        bool iconsRebuilt = false;
        if (!g_unloading.load()) {
            wchar_t sysDir[MAX_PATH] = {};
            DWORD sysLen = GetSystemDirectoryW(sysDir, MAX_PATH);
            if (sysLen > 0 && sysLen < MAX_PATH) {
                std::wstring exePath = std::wstring(sysDir) + L"\\ie4uinit.exe";

                STARTUPINFOW si = {};
                si.cb = sizeof(si);
                si.dwFlags = STARTF_USESHOWWINDOW;
                si.wShowWindow = SW_HIDE;
                PROCESS_INFORMATION pi = {};

                wchar_t cmdLine[] = L"ie4uinit.exe -show";
                if (CreateProcessW(exePath.c_str(), cmdLine, nullptr, nullptr,
                                   FALSE, CREATE_NO_WINDOW, nullptr, nullptr,
                                   &si, &pi)) {
                    DWORD waitRes = WaitForSingleObject(pi.hProcess, 5000);
                    DWORD exitCode = 1;
                    GetExitCodeProcess(pi.hProcess, &exitCode);
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                    iconsRebuilt = (waitRes == WAIT_OBJECT_0 && exitCode == 0);
                } else {
                    Wh_Log(L"ie4uinit.exe launch failed: %lu", GetLastError());
                }
            }
        }

        if (cfg->notifyRebuildCache && !g_unloading.load()) {
            double mbFreed =
                static_cast<double>(bytesFreed) / (1024.0 * 1024.0);
            wchar_t mbBuf[64] = {};
            swprintf_s(mbBuf, L"%.2f MB", mbFreed);

            std::vector<DialogRow> rows = {
                {L"Thumbnail files deleted", std::to_wstring(thumbsDeleted)},
                {L"Locked / skipped", std::to_wstring(thumbsSkipped)},
                {L"Space recovered", mbBuf},
                {L"Icon cache", iconsRebuilt ? L"Rebuilt" : L"Failed"},
            };

            std::wstring footer;
            if (!iconsRebuilt) {
                footer = L"Icon rebuild failed";
            } else if (thumbsSkipped > 0) {
                footer = L"Some thumbnails locked by Explorer";
            } else {
                footer = L"Cache rebuilt successfully";
            }

            DispatchXAMLCardSync(L"Cache Rebuilt", L"\xE8B9", rows, footer);
        }
    });
}

// 7. Clean DNS Cache
using DnsFlushResolverCache_t = BOOL(WINAPI*)();

void OnFlushDnsClick(wf::IInspectable const&, wux::RoutedEventArgs const&) {
    RunAsyncTracked([]() {
        BOOL flushed = FALSE;
        if (HMODULE hDnsApi = LoadLibraryExW(L"dnsapi.dll", nullptr,
                                             LOAD_LIBRARY_SEARCH_SYSTEM32)) {
            auto pFlush = reinterpret_cast<DnsFlushResolverCache_t>(
                GetProcAddress(hDnsApi, "DnsFlushResolverCache"));
            if (pFlush) {
                flushed = pFlush();
            }
            FreeLibrary(hDnsApi);
        }

        auto cfg = GetSettingsSnapshot();
        if (cfg->notifyFlushDns && !g_unloading.load()) {
            std::vector<DialogRow> rows = {
                {L"Resolver Cache", flushed ? L"Flushed" : L"Failed"},
                {L"Method", L"dnsapi.dll (In-Memory)"},
            };
            DispatchXAMLCardSync(L"DNS Cache", L"\xE774", rows,
                                 flushed ? L"Cache cleared successfully"
                                         : L"Error flushing cache");
        }
    });
}

// ============================================================================
// Taskbar Context Menu Injection Engine
// ============================================================================

struct MenuItemSpec {
    bool enabled;
    bool inSubmenu;
    std::wstring text;
    const wchar_t* glyph;
    void (*handler)(wf::IInspectable const&, wux::RoutedEventArgs const&);
};

struct MenuBuildState {
    std::shared_ptr<const ModSettings> settings;
    // ABI identity of the taskbar menu's IVector<MenuFlyoutItemBase>.
    void* itemsIdentity = nullptr;
    // Elements appended to the bottom of the menu, kept in display order.
    std::vector<wuxc::MenuFlyoutItemBase> bottomInjected;
    // Re-entrancy guard while the mod modifies the item collection.
    bool modifying = false;
};

thread_local MenuBuildState* g_menuBuildState = nullptr;

void* GetIdentity(MenuItemsVector const& items) {
    return winrt::get_abi(items.as<wf::IUnknown>());
}

// Creates custom menu elements, either listed directly or grouped into a
// dedicated cascading submenu with an optional icon.
std::vector<wuxc::MenuFlyoutItemBase> CreateCustomElements(
    const std::vector<CustomMenuItemSettings>& items,
    const std::wstring& submenuName,
    const std::wstring& submenuIcon) {
    std::vector<wuxc::MenuFlyoutItemBase> elements;
    if (items.empty()) {
        return elements;
    }

    if (!submenuName.empty()) {
        wuxc::MenuFlyoutSubItem subItem;
        subItem.Name(kInjectedElementName);
        subItem.Text(submenuName);

        if (!submenuIcon.empty()) {
            subItem.Icon(CreateIconFromSetting(submenuIcon, 16.0));
        }

        InjectedElement tracked;
        tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(subItem);
        TrackInjectedElement(std::move(tracked));

        auto subItems = subItem.Items();
        for (const auto& item : items) {
            subItems.Append(CreateCustomMenuItem(item));
        }

        elements.push_back(subItem);
    } else {
        for (const auto& item : items) {
            elements.push_back(CreateCustomMenuItem(item));
        }
    }

    return elements;
}

// Builds the system utilities items (and any custom items configured to appear
// inside the System Utilities submenu).
std::vector<wuxc::MenuFlyoutItemBase> CreateUtilityElements(
    const ModSettings& cfg) {
    std::vector<wuxc::MenuFlyoutItemBase> utilityElements;
    wuxc::MenuFlyoutSubItem subItem{nullptr};

    const MenuItemSpec specs[] = {
        {cfg.showRestartExplorer, cfg.inSubmenuRestartExplorer,
         cfg.textRestartExplorer, L"\xE72C", &OnRestartExplorerClick},
        {cfg.showCleanTemp, cfg.inSubmenuCleanTemp, cfg.textCleanTemp,
         L"\xE74D", &OnCleanTempClick},
        {cfg.showReduceMemory, cfg.inSubmenuReduceMemory, cfg.textReduceMemory,
         L"\xE950", &OnReduceMemoryClick},
        {cfg.showRestartGpu, cfg.inSubmenuRestartGpu, cfg.textRestartGpu,
         L"\xE7F4", &OnRestartGpuClick},
        {cfg.showGetIp, cfg.inSubmenuGetIp, cfg.textGetIp, L"\xE968",
         &OnGetIpClick},
        {cfg.showRebuildCache, cfg.inSubmenuRebuildCache, cfg.textRebuildCache,
         L"\xE8B9", &OnRebuildCacheClick},
        {cfg.showFlushDns, cfg.inSubmenuFlushDns, cfg.textFlushDns, L"\xE774",
         &OnFlushDnsClick},
    };

    for (const auto& spec : specs) {
        if (!spec.enabled) {
            continue;
        }

        wuxc::MenuFlyoutItem menuItem;
        menuItem.Name(kInjectedElementName);
        menuItem.Text(spec.text);
        menuItem.Icon(CreateFluentIcon(spec.glyph));

        InjectedElement tracked;
        tracked.element = winrt::make_weak<wuxc::MenuFlyoutItemBase>(menuItem);
        tracked.clickToken =
            menuItem.Click(wux::RoutedEventHandler{spec.handler});
        tracked.hasClickHandler = true;
        TrackInjectedElement(std::move(tracked));

        if (cfg.enableSubmenu && spec.inSubmenu) {
            if (!subItem) {
                subItem = wuxc::MenuFlyoutSubItem();
                subItem.Name(kInjectedElementName);
                subItem.Text(cfg.textSubmenu);
                if (!cfg.glyphSubmenu.empty()) {
                    subItem.Icon(CreateIconFromSetting(cfg.glyphSubmenu, 16.0));
                }

                InjectedElement trackedSub;
                trackedSub.element =
                    winrt::make_weak<wuxc::MenuFlyoutItemBase>(subItem);
                TrackInjectedElement(std::move(trackedSub));
            }
            subItem.Items().Append(menuItem);
        } else {
            utilityElements.push_back(menuItem);
        }
    }

    // If custom items are configured to appear inside the System Utilities
    // group:
    if (cfg.customPlacement == CustomPlacement::Submenu &&
        !cfg.customItems.empty()) {
        auto customElements = CreateCustomElements(
            cfg.customItems, cfg.customSubmenuName, cfg.customSubmenuIcon);

        if (cfg.enableSubmenu) {
            if (!subItem) {
                subItem = wuxc::MenuFlyoutSubItem();
                subItem.Name(kInjectedElementName);
                subItem.Text(cfg.textSubmenu);
                if (!cfg.glyphSubmenu.empty()) {
                    subItem.Icon(CreateIconFromSetting(cfg.glyphSubmenu, 16.0));
                }

                InjectedElement trackedSub;
                trackedSub.element =
                    winrt::make_weak<wuxc::MenuFlyoutItemBase>(subItem);
                TrackInjectedElement(std::move(trackedSub));
            }

            if (subItem.Items().Size() > 0 && cfg.customSeparator) {
                subItem.Items().Append(CreateSeparator());
            }

            for (const auto& el : customElements) {
                subItem.Items().Append(el);
            }
        } else {
            if (!utilityElements.empty() && cfg.customSeparator) {
                utilityElements.push_back(CreateSeparator());
            }
            for (const auto& el : customElements) {
                utilityElements.push_back(el);
            }
        }
    }

    if (subItem && subItem.Items().Size() > 0) {
        utilityElements.push_back(subItem);
    }

    return utilityElements;
}

void InjectElements(MenuItemsVector const& items, MenuBuildState& state) {
    PruneInjectedElements();

    const auto& cfg = *state.settings;

    std::vector<wuxc::MenuFlyoutItemBase> topElements;
    std::vector<wuxc::MenuFlyoutItemBase> bottomElements;

    // 1. Place System Utilities (Top or Bottom)
    auto utilityElements = CreateUtilityElements(cfg);
    if (!utilityElements.empty()) {
        if (cfg.utilitiesPlacement == UtilitiesPlacement::Top) {
            for (const auto& el : utilityElements) {
                topElements.push_back(el);
            }
            topElements.push_back(CreateSeparator());
        } else {
            bottomElements.push_back(CreateSeparator());
            for (const auto& el : utilityElements) {
                bottomElements.push_back(el);
            }
        }
    }

    // 2. Place Custom Items when configured at Top or Bottom
    if (!cfg.customItems.empty() &&
        cfg.customPlacement != CustomPlacement::Submenu) {
        auto customElements = CreateCustomElements(
            cfg.customItems, cfg.customSubmenuName, cfg.customSubmenuIcon);

        if (cfg.customPlacement == CustomPlacement::Top) {
            for (const auto& el : customElements) {
                topElements.push_back(el);
            }
            if (cfg.customSeparator) {
                topElements.push_back(CreateSeparator());
            }
        } else if (cfg.customPlacement == CustomPlacement::Bottom) {
            if (cfg.customSeparator) {
                bottomElements.push_back(CreateSeparator());
            }
            for (const auto& el : customElements) {
                bottomElements.push_back(el);
            }
        }
    }

    // Insert top elements before the built-in taskbar menu items
    if (!topElements.empty()) {
        uint32_t index = 0;
        for (const auto& element : topElements) {
            items.InsertAt(index++, element);
        }
    }

    // Append bottom elements after the built-in taskbar menu items
    if (!bottomElements.empty()) {
        for (const auto& element : bottomElements) {
            items.Append(element);
        }
        state.bottomInjected = std::move(bottomElements);
    }
}

void MoveBottomElementsToEnd(MenuItemsVector const& items,
                             MenuBuildState& state) {
    for (const auto& element : state.bottomInjected) {
        uint32_t index;
        if (items.IndexOf(element, index)) {
            items.RemoveAt(index);
        }
    }

    for (const auto& element : state.bottomInjected) {
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
    if (!state || state->modifying || g_unloading.load()) {
        return;
    }

    state->modifying = true;

    try {
        auto const& items = *reinterpret_cast<MenuItemsVector const*>(pThis);
        void* identity = GetIdentity(items);

        if (!state->itemsIdentity) {
            if (!items.try_as<MenuItemsVector>()) {
                state->modifying = false;
                return;
            }

            state->itemsIdentity = identity;
            InjectElements(items, *state);
        } else if (state->itemsIdentity == identity &&
                   !state->bottomInjected.empty()) {
            // Keep bottom-placed items at the end of the menu when Explorer
            // appends additional built-in items.
            MoveBottomElementsToEnd(items, *state);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to update taskbar menu: %08X %s",
               static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to update taskbar menu");
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
        ContextMenus_ShowTaskbarSettingsContextMenu_Original(
            target, taskbarSettings, args, options);
        return;
    }

    try {
        if (target && target.XamlRoot()) {
            g_lastTargetWeak = target;
        }
    } catch (...) {
    }

    MenuBuildState state;
    state.settings = GetSettingsSnapshot();

    struct ScopedMenuBuild {
        explicit ScopedMenuBuild(MenuBuildState* state) {
            g_menuBuildState = state;
        }
        ~ScopedMenuBuild() { g_menuBuildState = nullptr; }
    } scopedMenuBuild(&state);

    ContextMenus_ShowTaskbarSettingsContextMenu_Original(
        target, taskbarSettings, args, options);
}

// ============================================================================
// Symbol Hooking & Mod Lifecycle
// ============================================================================

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

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(
            L"Failed to hook Taskbar.View symbols, required symbols are "
            L"missing");
        return false;
    }

    Wh_Log(L"Hooked the taskbar menu");
    return true;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, bool applyHookOperations) {
    if (!module || g_taskbarViewModuleHooked.load()) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewModuleHooked.exchange(true)) {
        return;
    }

    if (HookTaskbarViewDllSymbols(module)) {
        if (applyHookOperations) {
            Wh_ApplyHookOperations();
        }
    } else {
        Wh_Log(L"Failed to hook Taskbar.View symbols");
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, true);
    }
    return module;
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
    return Trim(value.get());
}

std::wstring LoadStringSetting(const wchar_t* name,
                               const wchar_t* defaultValue) {
    std::wstring trimmed = GetStringSetting(name);
    return !trimmed.empty() ? trimmed : defaultValue;
}

void LoadSettings() {
    auto fresh = std::make_shared<ModSettings>();

    // 1. System Utilities Submenu & Placement
    fresh->enableSubmenu = Wh_GetIntSetting(L"submenu.enabled") != 0;
    fresh->textSubmenu =
        LoadStringSetting(L"submenu.label", L"System Utilities");
    fresh->glyphSubmenu = LoadStringSetting(L"submenu.glyph", L"E712");

    std::wstring utilPlacementStr = GetStringSetting(L"submenu.placement");
    fresh->utilitiesPlacement = (utilPlacementStr == L"bottom")
                                    ? UtilitiesPlacement::Bottom
                                    : UtilitiesPlacement::Top;

    // 2. Restart Explorer
    fresh->showRestartExplorer =
        Wh_GetIntSetting(L"restartExplorer.enabled") != 0;
    fresh->inSubmenuRestartExplorer =
        Wh_GetIntSetting(L"restartExplorer.inSubmenu") != 0;
    fresh->textRestartExplorer =
        LoadStringSetting(L"restartExplorer.label", L"Restart Explorer");

    // 3. Clean Temp
    fresh->showCleanTemp = Wh_GetIntSetting(L"cleanTemp.enabled") != 0;
    fresh->inSubmenuCleanTemp = Wh_GetIntSetting(L"cleanTemp.inSubmenu") != 0;
    fresh->textCleanTemp = LoadStringSetting(L"cleanTemp.label", L"Clean Temp");
    fresh->cleanRecycleBin =
        Wh_GetIntSetting(L"cleanTemp.cleanRecycleBin") != 0;
    fresh->notifyCleanTemp = Wh_GetIntSetting(L"cleanTemp.showCard") != 0;

    // 4. Reduce Memory
    fresh->showReduceMemory = Wh_GetIntSetting(L"reduceMemory.enabled") != 0;
    fresh->inSubmenuReduceMemory =
        Wh_GetIntSetting(L"reduceMemory.inSubmenu") != 0;
    fresh->textReduceMemory =
        LoadStringSetting(L"reduceMemory.label", L"Reduce Memory");
    fresh->notifyReduceMemory = Wh_GetIntSetting(L"reduceMemory.showCard") != 0;

    // 5. Restart Graphics Driver
    fresh->showRestartGpu = Wh_GetIntSetting(L"restartGpu.enabled") != 0;
    fresh->inSubmenuRestartGpu = Wh_GetIntSetting(L"restartGpu.inSubmenu") != 0;
    fresh->textRestartGpu =
        LoadStringSetting(L"restartGpu.label", L"Restart Graphics Driver");

    // 6. Get IP Address
    fresh->showGetIp = Wh_GetIntSetting(L"getIp.enabled") != 0;
    fresh->inSubmenuGetIp = Wh_GetIntSetting(L"getIp.inSubmenu") != 0;
    fresh->textGetIp = LoadStringSetting(L"getIp.label", L"Get IP Address");
    fresh->queryPublicIp = Wh_GetIntSetting(L"getIp.queryPublicIp") != 0;
    fresh->copyIpToClipboard = Wh_GetIntSetting(L"getIp.copyToClipboard") != 0;

    // 7. Rebuild Icon & Thumbnail Cache
    fresh->showRebuildCache = Wh_GetIntSetting(L"rebuildCache.enabled") != 0;
    fresh->inSubmenuRebuildCache =
        Wh_GetIntSetting(L"rebuildCache.inSubmenu") != 0;
    fresh->textRebuildCache = LoadStringSetting(
        L"rebuildCache.label", L"Rebuild Icon & Thumbnail Cache");
    fresh->notifyRebuildCache = Wh_GetIntSetting(L"rebuildCache.showCard") != 0;

    // 8. Clean DNS Cache
    fresh->showFlushDns = Wh_GetIntSetting(L"flushDns.enabled") != 0;
    fresh->inSubmenuFlushDns = Wh_GetIntSetting(L"flushDns.inSubmenu") != 0;
    fresh->textFlushDns =
        LoadStringSetting(L"flushDns.label", L"Clean DNS Cache");
    fresh->notifyFlushDns = Wh_GetIntSetting(L"flushDns.showCard") != 0;

    // 9. Custom Menu Items
    for (int i = 0;; i++) {
        CustomMenuItemSettings item;
        item.name = GetStringSetting(L"items[%d].name", i);
        item.command = GetStringSetting(L"items[%d].command", i);

        // Stop reading at the first entry that has neither a label nor a
        // command.
        if (item.name.empty() && item.command.empty()) {
            break;
        }

        item.arguments = GetStringSetting(L"items[%d].arguments", i);
        item.icon = GetStringSetting(L"items[%d].icon", i);
        item.runAsAdmin = Wh_GetIntSetting(L"items[%d].runAsAdmin", i) != 0;
        item.hideWindow = Wh_GetIntSetting(L"items[%d].hideWindow", i) != 0;

        fresh->customItems.push_back(std::move(item));
    }

    // 10. Custom items placement, separator, and submenu settings
    std::wstring placementStr = GetStringSetting(L"placement");
    if (placementStr == L"top") {
        fresh->customPlacement = CustomPlacement::Top;
    } else if (placementStr == L"submenu") {
        fresh->customPlacement = CustomPlacement::Submenu;
    } else {
        fresh->customPlacement = CustomPlacement::Bottom;
    }

    fresh->customSeparator = Wh_GetIntSetting(L"separator") != 0;
    fresh->customSubmenuName = GetStringSetting(L"submenuName");
    fresh->customSubmenuIcon = GetStringSetting(L"submenuIcon");

    Wh_Log(L"Loaded settings: %zu custom item(s)", fresh->customItems.size());

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(fresh);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_unloading.store(false);
    LoadSettings();

    DWORD build = GetWindowsBuild();
    if (build && build < 22000) {
        Wh_Log(L"Unsupported Windows build %lu", build);
        return TRUE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewModuleHooked.store(true);
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Failed to hook Taskbar.View symbols during init");
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW =
        kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;
    if (loadLibraryExW) {
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    } else {
        Wh_Log(L"LoadLibraryExW not found in kernelbase.dll");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewModuleHooked.load()) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            HandleLoadedModuleIfTaskbarView(taskbarViewModule, true);
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading.store(true);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading.store(true);

    CancelActivePublicIpRequest();

    bool hasInjectedElements;
    {
        std::lock_guard<std::mutex> lock(g_injectedElementsMutex);
        hasInjectedElements = !g_injectedElements.empty();
    }

    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(hTaskbarWnd, [hasInjectedElements]() {
            CloseActiveFlyoutOnUIThread();
            if (hasInjectedElements) {
                ReleaseInjectedElements();
            }
        });
    }

    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        threads.swap(g_threads);
    }

    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }

    WaitForLaunchThreads();
}