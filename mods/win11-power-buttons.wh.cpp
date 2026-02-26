// ==WindhawkMod==
// @id              win11-power-buttons
// @name            Windows 11 Start Menu Power Buttons
// @name:zh-CN      Windows 11 开始菜单一键电源按钮
// @description     Adds customizable one-click Shutdown, Restart, Sign out, Sleep, and Hibernate buttons to the Windows 11 Start menu, replacing the default power flyout.
// @description:zh-CN 添加可配置的一键关机/重启/注销/睡眠/休眠按钮到 Windows 11 开始菜单，替换默认的电源按钮二级菜单。
// @version         1.0.0
// @author          Hakuuyosei
// @author:zh-CN    灵弦
// @github          https://github.com/ahzvenol
// @include         StartMenuExperienceHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols -luser32 -lwtsapi32 -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Start Menu Power Buttons

Adds customizable one-click Shutdown, Restart, Sign out, Sleep, and Hibernate buttons to the Windows 11 Start menu, replacing the default power flyout.

![](https://i.imgur.com/M7xcCgb.png)

---

添加可配置的一键关机/重启/注销/睡眠/休眠按钮到 Windows 11 开始菜单，替换默认的电源按钮二级菜单。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- alignment: right
  $name: Alignment
  $name:zh-CN: 对齐方式
  $description: Aligns the buttons within the NavigationPane.
  $description:zh-CN: 设置按钮组在导航窗格中的对齐方式。
  $options:
    - left: Left
    - center: Center
    - right: Right
  $options:zh-CN:
    - left: 左对齐
    - center: 居中
    - right: 右对齐

- buttons: [sleep, signout, restart, shutdown]
  $name:zh-CN: 按钮与排序
  $name: Buttons & Order
  $description: Select which buttons to display and their order. Duplicates are ignored.
  $description:zh-CN: 选择要显示的按钮以及它们的顺序，重复项会被忽略。
  $options:
    - shutdown: Shutdown
    - restart: Restart
    - signout: Sign out
    - sleep: Sleep
    - hibernate: Hibernate
  $options:zh-CN:
    - shutdown: 关机
    - restart: 重启
    - signout: 注销
    - sleep: 睡眠
    - hibernate: 休眠
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <set>
#include <cwchar>
#include <cwctype>

#include <windows.h>
#include <powrprof.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.System.h>

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wu = winrt::Windows::UI;
namespace wuc = winrt::Windows::UI::Core;

// Global state flags
static std::atomic<bool> g_initialized = false;
static std::atomic<bool> g_unloading = false;
static std::atomic<bool> g_buttonsInjected = false;
static std::atomic<bool> g_monitoringActive = false;
static std::atomic<bool> g_forceRebuild = false;

static std::mutex g_settingsMutex;

// ============================================================================
// Settings Management
// ============================================================================

struct Settings {
    wux::HorizontalAlignment alignment = wux::HorizontalAlignment::Right;
    std::vector<std::wstring> buttonKeywords;
};

static Settings g_settings;

static std::wstring JoinStrings(const std::vector<std::wstring>& v) {
    std::wstring out;
    for (size_t i = 0; i < v.size(); i++) {
        if (i) out += L",";
        out += v[i];
    }
    return out;
}

// Parses Windhawk settings into internal structures
static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_settings.alignment = wux::HorizontalAlignment::Right;
    PCWSTR alignmentStr = Wh_GetStringSetting(L"alignment");

    if (_wcsicmp(alignmentStr, L"left") == 0) {
        g_settings.alignment = wux::HorizontalAlignment::Left;
    } else if (_wcsicmp(alignmentStr, L"center") == 0) {
        g_settings.alignment = wux::HorizontalAlignment::Center;
    }

    Wh_FreeStringSetting(alignmentStr);

    g_settings.buttonKeywords.clear();
    // 'buttons' is defined as an array of strings in settings.
    // Iterates up to a reasonable limit or until an empty/missing entry is encountered.
    for (int i = 0; i < 32; i++) {
        PCWSTR itemStr = Wh_GetStringSetting(L"buttons[%d]", i);
        if (!itemStr || !*itemStr) {
            Wh_FreeStringSetting(itemStr);
            break;
        }

        std::wstring item(itemStr);
        Wh_FreeStringSetting(itemStr);

        if (!item.empty())
            g_settings.buttonKeywords.push_back(std::move(item));
    }

    Wh_Log(L"Settings loaded: alignment=%d, buttons=%s",
           static_cast<int>(g_settings.alignment),
           JoinStrings(g_settings.buttonKeywords).c_str());
}

// ============================================================================
// Button Configuration
// ============================================================================

enum class PowerAction {
    Shutdown = 0,
    Restart = 1,
    SignOut = 2,
    Sleep = 3,
    Hibernate = 4,
};

struct ButtonDefinition {
    std::wstring keyword;
    PowerAction action;
    const wchar_t* glyph;
};

// Glyphs are from Segoe Fluent Icons.
// Hibernate shares the Sleep icon as there isn't a dedicated standard glyph.
static const std::vector<ButtonDefinition> g_buttonDefinitions = {
    {L"shutdown",  PowerAction::Shutdown,  L"\uE7E8"},
    {L"restart",   PowerAction::Restart,   L"\uE777"},
    {L"signout",   PowerAction::SignOut,   L"\uF3B1"},
    {L"sleep",     PowerAction::Sleep,     L"\uE708"},
    {L"hibernate", PowerAction::Hibernate, L"\uE708"},
};

struct ButtonConfig {
    PowerAction action;
    const wchar_t* glyph;
};

static std::vector<ButtonConfig> g_buttons;

// Maps configuration keywords to actionable button definitions, ensuring uniqueness
static void BuildButtons() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_buttons.clear();
    std::set<PowerAction> seen;

    for (const auto& keyword : g_settings.buttonKeywords) {
        for (const auto& def : g_buttonDefinitions) {
            if (def.keyword == keyword && seen.insert(def.action).second) {
                g_buttons.push_back({def.action, def.glyph});
                break;
            }
        }
    }
}

// ============================================================================
// Cross-Process IPC (Explorer Proxy)
//
// StartMenuExperienceHost.exe runs within a restricted AppContainer sandbox
// and lacks privileges to execute ExitWindowsEx or SetSuspendState.
// To perform these privileged commands, a hidden message-only window is
// injected into Explorer.exe (running with user privileges) to act as a proxy.
// ============================================================================

static const wchar_t* PROXY_WINDOW_CLASS = L"PowerActionProxy_Class";
static const wchar_t* PROXY_WINDOW_NAME  = L"PowerActionProxy_Window";
static const wchar_t* PROXY_MESSAGE_NAME = L"PowerActionProxy_Execute";

static UINT g_proxyMessage = 0;
static HWND g_proxyWindow = NULL;
static HANDLE g_proxyThread = NULL;

static void InitProxyMessage() {
    if (g_proxyMessage == 0) {
        g_proxyMessage = RegisterWindowMessageW(PROXY_MESSAGE_NAME);
    }
}

// Executes the requested power action, acquiring necessary privileges if required
static void PerformPowerAction(PowerAction action) {
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tkp{};
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
        }
        CloseHandle(hToken);
    }

    switch (action) {
        case PowerAction::Shutdown:
            ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case PowerAction::Restart:
            ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case PowerAction::SignOut:
            ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);
            break;
        case PowerAction::Sleep:
            SetSuspendState(FALSE, FALSE, FALSE);
            break;
        case PowerAction::Hibernate:
            SetSuspendState(TRUE, FALSE, FALSE);
            break;
    }
}

static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == g_proxyMessage && g_proxyMessage != 0) {
        PerformPowerAction((PowerAction)wParam);
        return 0;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

// Thread function for the hidden proxy window
static DWORD WINAPI ProxyWindowThread(LPVOID) {
    InitProxyMessage();

    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.lpfnWndProc = ProxyWndProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.lpszClassName = PROXY_WINDOW_CLASS;

    if (!RegisterClassExW(&wcex)) {
        Wh_Log(L"Proxy: RegisterClassEx failed, error %lu", GetLastError());
        return 1;
    }

    g_proxyWindow = CreateWindowExW(
        0, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME,
        0, 0, 0, 0, 0, HWND_MESSAGE,
        NULL, GetModuleHandle(NULL), NULL
    );

    if (!g_proxyWindow) {
        Wh_Log(L"Proxy: Failed to create window, error %lu", GetLastError());
        UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandle(NULL));
        return 1;
    }

    // Bypass UIPI to allow messages from lower-integrity processes
    ChangeWindowMessageFilterEx(g_proxyWindow, g_proxyMessage, MSGFLT_ALLOW, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindow(g_proxyWindow);
    g_proxyWindow = NULL;
    UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandle(NULL));

    return 0;
}

static void StartProxyThread() {
    g_proxyThread = CreateThread(NULL, 0, ProxyWindowThread, NULL, 0, NULL);
    if (!g_proxyThread) {
        Wh_Log(L"Proxy: Failed to create thread, error %lu", GetLastError());
    }
}

// Sends a message to the explorer proxy window to execute the command
static void SendPowerAction(PowerAction action) {
    InitProxyMessage();

    HWND hProxy = FindWindowW(PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME);
    if (!hProxy) {
        Wh_Log(L"Proxy: Window not found. Is explorer.exe hooked?");
        return;
    }

    PostMessageW(hProxy, g_proxyMessage, (WPARAM)action, 0);
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring p = path;
    for (auto& c : p) c = towlower(c);
    return p.find(L"explorer.exe") != std::wstring::npos;
}

// ============================================================================
// Start Menu UI Injection
// ============================================================================

static const wchar_t* CONTAINER_TAG  = L"PowerButtons_Container";

// Weak references to key UI elements to avoid repeated VisualTree traversal
static winrt::weak_ref<wuxc::Panel> g_parentPanel{ nullptr };
static winrt::weak_ref<wux::FrameworkElement> g_originalPowerButton{ nullptr };
static winrt::weak_ref<wuxc::StackPanel> g_buttonContainer{ nullptr };

static void InjectButtons(wuxc::Panel parentPanel, wux::FrameworkElement originalPowerButton) {
    try {
        Settings currentSettings;
        std::vector<ButtonConfig> currentButtons;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            currentSettings = g_settings;
            currentButtons = g_buttons;
        }

        // Ensure the native power button remains hidden
        if (originalPowerButton && originalPowerButton.Visibility() != wux::Visibility::Collapsed) {
            originalPowerButton.Visibility(wux::Visibility::Collapsed);
        }

        wuxc::StackPanel container = nullptr;

        // Attempt to retrieve the container from cache
        container = g_buttonContainer.get();
        if (container) {
            // Validate that the cached container is still attached to the target parent
            try {
                auto containerParent = wuxm::VisualTreeHelper::GetParent(container);
                if (!containerParent || containerParent != parentPanel) {
                    container = nullptr;
                    g_buttonContainer = nullptr;
                }
            } catch (...) {
                container = nullptr;
                g_buttonContainer = nullptr;
            }
        }

        // Cache miss: scan children to locate the tagged container
        if (!container) {
            auto children = parentPanel.Children();
            for (uint32_t i = 0; i < children.Size(); i++) {
                auto child = children.GetAt(i);
                if (auto panel = child.try_as<wuxc::StackPanel>()) {
                    auto tag = panel.Tag();
                    if (tag && winrt::unbox_value_or<winrt::hstring>(tag, L"") == CONTAINER_TAG) {
                        container = panel;
                        g_buttonContainer = container;
                        break;
                    }
                }
            }
        }

        // If still not found, create and insert a new container
        if (!container) {
            container = wuxc::StackPanel();
            container.Tag(winrt::box_value(CONTAINER_TAG));
            container.Orientation(wuxc::Orientation::Horizontal);
            container.VerticalAlignment(wux::VerticalAlignment::Center);
            parentPanel.Children().Append(container);
            g_buttonContainer = container;
        }

        // Fast path: if the button count matches and no rebuild is forced,
        // only update layout properties to handle potential style changes.
        if (!g_forceRebuild.exchange(false) &&
            container.Children().Size() == currentButtons.size()) {

            if (container.Visibility() != wux::Visibility::Visible)
                container.Visibility(wux::Visibility::Visible);

            if (container.HorizontalAlignment() != currentSettings.alignment)
                container.HorizontalAlignment(currentSettings.alignment);

            container.Margin({ 0, 0, 0, 0 });
            return;
        }

        // Full rebuild path
        if (container.Visibility() != wux::Visibility::Visible)
            container.Visibility(wux::Visibility::Visible);

        container.HorizontalAlignment(currentSettings.alignment);

        container.Margin({ 0, 0, 0, 0 });

        container.Children().Clear();

        for (size_t i = 0; i < currentButtons.size(); i++) {
            const auto& cfg = currentButtons[i];

            wuxc::Button btn;
            btn.Width(40);
            btn.Height(40);

            // Apply margin only between buttons, not on outer edges
            if (i == currentButtons.size() - 1) {
                btn.Margin({ 0, 0, 0, 0 });
            } else {
                btn.Margin({ 0, 0, 4, 0 });
            }

            // Style the button to be transparent and blend with the Start Menu
            btn.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
            btn.BorderThickness({ 0, 0, 0, 0 });
            btn.CornerRadius({ 4, 4, 4, 4 });

            wuxc::FontIcon icon;
            icon.Glyph(cfg.glyph);
            icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
            icon.FontSize(16);
            btn.Content(icon);

            PowerAction action = cfg.action;
            btn.Click([action](auto&&, auto&&) {
                SendPowerAction(action);
            });

            container.Children().Append(btn);
        }

    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Inject: WinRT error 0x%08X", (unsigned)ex.code());
    } catch (...) {
        Wh_Log(L"Inject: Unknown error");
    }
}

// Searches the Visual Tree recursively for the element named "PowerButton"
static wux::FrameworkElement FindOriginalPowerButton(wux::DependencyObject parent) {
    if (!parent) return nullptr;

    int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);

        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == L"PowerButton")
                return fe;
        }

        auto found = FindOriginalPowerButton(child);
        if (found) return found;
    }

    return nullptr;
}

static bool TryInjectButtons() {
    if (g_unloading) return false;

    try {
        auto window = wux::Window::Current();
        if (!window) return false;

        auto content = window.Content();
        if (!content) return false;

        auto powerButton = FindOriginalPowerButton(content);
        if (!powerButton) return false;

        auto parentObj = wuxm::VisualTreeHelper::GetParent(powerButton);
        if (!parentObj) return false;

        auto parentPanel = parentObj.try_as<wuxc::Panel>();
        if (!parentPanel) return false;

        g_parentPanel = parentPanel;
        g_originalPowerButton = powerButton;

        InjectButtons(parentPanel, powerButton);
        g_buttonsInjected = true;

        return true;

    } catch (...) {
        Wh_Log(L"Inject: Exception during TryInjectButtons");
        return false;
    }
}

// ============================================================================
// Visibility Monitoring
// Hooks Start Menu window events to trigger injection when UI becomes visible.
// ============================================================================

static winrt::event_token g_visibilityToken{};
static winrt::event_token g_activatedToken{};

static void OnWindowVisible() {
    if (g_unloading) return;

    // Skip if already injected and no setting changes occurred
    if (g_buttonsInjected && !g_forceRebuild) {
        return;
    }

    // Schedule injection on the UI thread at low priority to avoid blocking UI startup
    auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (dq) {
        dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
            if (!g_unloading) {
                TryInjectButtons();
            }
        });
    }
}

static bool StartVisibilityMonitoring() {
    if (g_monitoringActive) return true;

    try {
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) return false;

        g_visibilityToken =
            coreWindow.VisibilityChanged([](auto&&, auto&& args) {
                if (args.Visible()) OnWindowVisible();
            });

        g_activatedToken =
            coreWindow.Activated([](auto&&, auto&& args) {
                if (args.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated)
                    OnWindowVisible();
            });

        g_monitoringActive = true;

        // If Start Menu is already visible, trigger immediately
        if (coreWindow.Visible())
            OnWindowVisible();

        return true;
    } catch (...) {
        return false;
    }
}

static void StopVisibilityMonitoring() {
    try {
        // Unregister event handlers
        wuc::CoreWindow coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (coreWindow) {
            if (g_visibilityToken) {
                coreWindow.VisibilityChanged(g_visibilityToken);
                g_visibilityToken = {};
            }
            if (g_activatedToken) {
                coreWindow.Activated(g_activatedToken);
                g_activatedToken = {};
            }
        }
    } catch (...) {}

    // Cleanup UI: Remove custom buttons and restore original state
    try {
        auto panel = g_parentPanel.get();
        auto container = g_buttonContainer.get();
        auto originalBtn = g_originalPowerButton.get();

        if (panel) {
            // Remove the injected container from the visual tree
            if (container) {
                uint32_t index;
                if (panel.Children().IndexOf(container, index)) {
                    panel.Children().RemoveAt(index);
                }
            }
            // Clear tag/marker if used
            panel.Tag(nullptr);
        }

        // Restore original power button visibility
        if (originalBtn) {
            originalBtn.Visibility(wux::Visibility::Visible);
        }

    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Cleanup: WinRT error 0x%08X", (unsigned)ex.code());
    } catch (...) {
        Wh_Log(L"Cleanup: Unknown error");
    }

    // Clear references
    g_parentPanel = nullptr;
    g_originalPowerButton = nullptr;
    g_buttonContainer = nullptr;
    g_monitoringActive = false;
}

// ============================================================================
// Initialization Hooks
// Intercepts window creation to detect the Start Menu's CoreWindow.
// ============================================================================

static const wchar_t* THREAD_CALL_MSG = L"PowerButtons_ThreadCall";

static void InitWithRetry(HWND hwnd);

static void CALLBACK RetryTimerProc(HWND hwnd, UINT, UINT_PTR idEvent, DWORD) {
    KillTimer(hwnd, idEvent);
    InitWithRetry(hwnd);
}

// Attempts to initialize UWP hooks. Retries if the environment isn't ready.
static void InitWithRetry(HWND hwnd) {
    if (!g_initialized.exchange(true)) {
        LoadSettings();
        BuildButtons();
    }

    if (!StartVisibilityMonitoring()) {
        SetTimer(hwnd, 1772058423, 100, RetryTimerProc);
    }
}

// Hook CreateWindowInBand to catch the Windows.UI.Core.CoreWindow creation
using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD);
static CreateWindowInBand_t CreateWindowInBand_Original;

static HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                           int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                           HINSTANCE hInstance, PVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                           X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) return hWnd;

    // Check for CoreWindow class name efficiently
    if (lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        InitWithRetry(hWnd);
    }

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
static CreateWindowInBandEx_t CreateWindowInBandEx_Original;

static HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                             int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                             HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                             X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) return hWnd;

    if (lpClassName && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0 &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        InitWithRetry(hWnd);
    }

    return hWnd;
}

// Helper to run a lambda on the target window's thread
using ThreadCallProc_t = void(WINAPI*)(PVOID);

static bool CallOnWindowThread(HWND hWnd, ThreadCallProc_t proc, PVOID param) {
    static const UINT msg = RegisterWindowMessage(THREAD_CALL_MSG);
    struct CallParam { ThreadCallProc_t proc; PVOID param; };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) return false;

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == RegisterWindowMessage(THREAD_CALL_MSG)) {
                    CallParam* p = (CallParam*)cwp->lParam;
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);

    if (!hook) return false;

    CallParam p = { proc, param };
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindCoreWindow() {
    struct EnumParam { HWND* result; };
    HWND hWnd = nullptr;
    EnumParam param = { &hWnd };

    EnumWindows([](HWND w, LPARAM lParam) -> BOOL {
        EnumParam& p = *(EnumParam*)lParam;

        DWORD pid = 0;
        if (!GetWindowThreadProcessId(w, &pid) || pid != GetCurrentProcessId())
            return TRUE;

        WCHAR cls[64]{};
        if (GetClassName(w, cls, 64) == 0)
            return TRUE;

        if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            *p.result = w;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&param);

    return hWnd;
}

// ============================================================================
// Mod Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    if (IsExplorerProcess()) {
        StartProxyThread();
        return TRUE;
    }

    HMODULE user32 = LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!user32) {
        Wh_Log(L"Init: Failed to load user32.dll");
        return TRUE;
    }

    void* pCreateWindowInBand = (void*)GetProcAddress(user32, "CreateWindowInBand");
    if (pCreateWindowInBand) {
        Wh_SetFunctionHook(pCreateWindowInBand, (void*)CreateWindowInBand_Hook, (void**)&CreateWindowInBand_Original);
    } else {
        Wh_Log(L"Init: CreateWindowInBand not found");
    }

    void* pCreateWindowInBandEx = (void*)GetProcAddress(user32, "CreateWindowInBandEx");
    if (pCreateWindowInBandEx) {
        Wh_SetFunctionHook(pCreateWindowInBandEx, (void*)CreateWindowInBandEx_Hook, (void**)&CreateWindowInBandEx_Original);
    } else {
        Wh_Log(L"Init: CreateWindowInBandEx not found");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (IsExplorerProcess()) return;

    HWND hCoreWnd = FindCoreWindow();
    if (!hCoreWnd) {
        // Start Menu process started but no window yet. Hook will catch it.
        return;
    }

    CallOnWindowThread(hCoreWnd, [](PVOID hwnd) {
        InitWithRetry((HWND)hwnd);
    }, hCoreWnd);
}

void Wh_ModUninit() {
    g_unloading = true;

    if (g_proxyWindow) {
        PostMessage(g_proxyWindow, WM_QUIT, 0, 0);
    }

    if (g_proxyThread) {
        WaitForSingleObject(g_proxyThread, 5000); 
        CloseHandle(g_proxyThread);
        g_proxyThread = NULL;
    }
    g_proxyWindow = NULL;

    if (!IsExplorerProcess()) {
        HWND hCoreWnd = FindCoreWindow();
        if (hCoreWnd) {
            CallOnWindowThread(hCoreWnd, [](PVOID) {
                StopVisibilityMonitoring();
            }, nullptr);
        }
    }
}

void Wh_ModSettingsChanged() {
    if (IsExplorerProcess()) return;

    g_forceRebuild = true;
    g_buttonsInjected = false;
    g_buttonContainer = nullptr;

    HWND hCoreWnd = FindCoreWindow();
    if (!hCoreWnd) return;

    CallOnWindowThread(hCoreWnd, [](PVOID) {
        LoadSettings();
        BuildButtons();
        TryInjectButtons();
    }, nullptr);
}