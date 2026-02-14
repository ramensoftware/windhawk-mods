// ==WindhawkMod==
// @id              start-menu-size
// @name            Start Menu Size
// @description     Set a custom size for the Start menu on Windows 11
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Start Menu Size

Set a custom size for the Start menu on Windows 11.

Allows you to override the default width and height of the Start menu.

The classic menu width is 642, and the height is 726. [The redesigned Start
menu](https://microsoft.design/articles/start-fresh-redesigning-windows-start-menu/)
width is 834, and the height occupies the full screen height. The sizes might
vary for smaller screen sizes.

![Screenshot](https://i.imgur.com/FoFSFOV.png) \
_Example for making the Start menu smaller_
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 0
  $name: Width
  $description: >-
    A custom width for the Start menu in pixels. Set to 0 to use the default
    system value.
- height: 0
  $name: Height
  $description: >-
    A custom height for the Start menu in pixels. Set to 0 to use the default
    system value.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <roapi.h>
#include <winrt/base.h>
#include <winstring.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <cmath>
#include <functional>
#include <optional>

using namespace winrt::Windows::UI::Xaml;

struct {
    int width;
    int height;
} g_settings;

std::atomic<bool> g_unloading;

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

namespace StartMenuUI {

struct OriginalWidthParams {
    std::optional<double> width;
    std::optional<double> minWidth;
    std::optional<double> maxWidth;
};

struct OriginalHeightParams {
    std::optional<double> height;
    std::optional<double> minHeight;
    std::optional<double> maxHeight;
};

bool g_applyStylePending;
winrt::event_token g_layoutUpdatedToken;
winrt::event_token g_visibilityChangedToken;

// Classic start menu: single element for both width and height.
std::optional<OriginalWidthParams> g_originalClassicWidth;
std::optional<OriginalWidthParams> g_originalClassicRootGridWidth;
std::optional<OriginalHeightParams> g_originalClassicHeight;

// Redesigned start menu: mainMenu for width, frameRoot for height.
std::optional<OriginalWidthParams> g_originalMainMenuWidth;
std::optional<OriginalHeightParams> g_originalFrameRootHeight;

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

void ApplyStyle();

void SaveWidth(FrameworkElement element, OriginalWidthParams& out) {
    double width = element.Width();
    out.width = std::isnan(width) ? std::nullopt : std::optional(width);

    double minWidth = element.MinWidth();
    out.minWidth = minWidth == 0 ? std::nullopt : std::optional(minWidth);

    double maxWidth = element.MaxWidth();
    out.maxWidth =
        std::isinf(maxWidth) ? std::nullopt : std::optional(maxWidth);
}

void RestoreWidth(FrameworkElement element,
                  const OriginalWidthParams& original) {
    auto dep = element.as<DependencyObject>();

    if (original.width) {
        element.Width(*original.width);
    } else {
        dep.ClearValue(FrameworkElement::WidthProperty());
    }

    if (original.minWidth) {
        element.MinWidth(*original.minWidth);
    } else {
        dep.ClearValue(FrameworkElement::MinWidthProperty());
    }

    if (original.maxWidth) {
        element.MaxWidth(*original.maxWidth);
    } else {
        dep.ClearValue(FrameworkElement::MaxWidthProperty());
    }
}

void SaveHeight(FrameworkElement element, OriginalHeightParams& out) {
    double height = element.Height();
    out.height = std::isnan(height) ? std::nullopt : std::optional(height);

    double minHeight = element.MinHeight();
    out.minHeight = minHeight == 0 ? std::nullopt : std::optional(minHeight);

    double maxHeight = element.MaxHeight();
    out.maxHeight =
        std::isinf(maxHeight) ? std::nullopt : std::optional(maxHeight);
}

void RestoreHeight(FrameworkElement element,
                   const OriginalHeightParams& original) {
    auto dep = element.as<DependencyObject>();

    if (original.height) {
        element.Height(*original.height);
    } else {
        dep.ClearValue(FrameworkElement::HeightProperty());
    }

    if (original.minHeight) {
        element.MinHeight(*original.minHeight);
    } else {
        dep.ClearValue(FrameworkElement::MinHeightProperty());
    }

    if (original.maxHeight) {
        element.MaxHeight(*original.maxHeight);
    } else {
        dep.ClearValue(FrameworkElement::MaxHeightProperty());
    }
}

void ApplyStyleClassicStartMenu(FrameworkElement content) {
    FrameworkElement startSizingFrame =
        FindChildByClassName(content, L"StartDocked.StartSizingFrame");
    if (!startSizingFrame) {
        Wh_Log(L"Failed to find StartDocked.StartSizingFrame");
        return;
    }

    // Navigate to RootGrid and RootContent to remove MinWidth constraints.
    FrameworkElement rootGrid = nullptr;
    FrameworkElement rootContent = nullptr;
    bool isNewerVersion = false;

    FrameworkElement child = startSizingFrame;
    if ((child = FindChildByClassName(child,
                                      L"StartDocked.StartSizingFramePanel")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ContentPresenter")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Frame")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ContentPresenter")) &&
        (child = FindChildByClassName(child, L"StartDocked.LauncherFrame"))) {
        // Newer versions have RootPanel > RootGrid > RootContent, older
        // versions have RootGrid > RootContent.
        FrameworkElement rootPanel = FindChildByName(child, L"RootPanel");
        isNewerVersion = !!rootPanel;
        rootGrid = FindChildByName(rootPanel ? rootPanel : child, L"RootGrid");
        if (rootGrid) {
            rootContent = FindChildByName(rootGrid, L"RootContent");
        }
    }

    if (!rootContent) {
        Wh_Log(L"Failed to find RootContent");
    }

    Wh_Log(L"Invalidating measure");
    startSizingFrame.InvalidateMeasure();

    Wh_Log(L"Setting size: %dx%d", g_settings.width, g_settings.height);

    if (g_unloading) {
        if (g_originalClassicWidth) {
            RestoreWidth(startSizingFrame, *g_originalClassicWidth);
            g_originalClassicWidth.reset();
        }
        if (g_originalClassicRootGridWidth) {
            if (rootGrid) {
                RestoreWidth(rootGrid, *g_originalClassicRootGridWidth);
            }
            g_originalClassicRootGridWidth.reset();
        }
        if (g_originalClassicHeight) {
            RestoreHeight(startSizingFrame, *g_originalClassicHeight);
            g_originalClassicHeight.reset();
        }
    } else {
        if (g_settings.width > 0) {
            if (!g_originalClassicWidth) {
                g_originalClassicWidth.emplace();
                SaveWidth(startSizingFrame, *g_originalClassicWidth);
            }
            double width = static_cast<double>(g_settings.width);
            startSizingFrame.Width(width);
            startSizingFrame.MinWidth(width);
            startSizingFrame.MaxWidth(width);

            if (isNewerVersion && rootGrid) {
                if (!g_originalClassicRootGridWidth) {
                    g_originalClassicRootGridWidth.emplace();
                    SaveWidth(rootGrid, *g_originalClassicRootGridWidth);
                }
                rootGrid.Width(width);
                rootGrid.MinWidth(width);
                rootGrid.MaxWidth(width);
            }

            if (rootContent) {
                rootContent.as<DependencyObject>().ClearValue(
                    FrameworkElement::MinWidthProperty());
            }
        } else if (g_originalClassicWidth) {
            RestoreWidth(startSizingFrame, *g_originalClassicWidth);
            g_originalClassicWidth.reset();
            if (g_originalClassicRootGridWidth) {
                if (rootGrid) {
                    RestoreWidth(rootGrid, *g_originalClassicRootGridWidth);
                }
                g_originalClassicRootGridWidth.reset();
            }
        }

        if (g_settings.height > 0) {
            if (!g_originalClassicHeight) {
                g_originalClassicHeight.emplace();
                SaveHeight(startSizingFrame, *g_originalClassicHeight);
            }
            double height = static_cast<double>(g_settings.height);
            startSizingFrame.Height(height);
            startSizingFrame.MinHeight(height);
            startSizingFrame.MaxHeight(height);
        } else if (g_originalClassicHeight) {
            RestoreHeight(startSizingFrame, *g_originalClassicHeight);
            g_originalClassicHeight.reset();
        }
    }
}

void ApplyStyleRedesignedStartMenu(FrameworkElement content) {
    // Navigate: FrameRoot > AnimationRoot > MainMenu.
    FrameworkElement frameRoot = FindChildByName(content, L"FrameRoot");
    if (!frameRoot) {
        Wh_Log(L"Failed to find FrameRoot");
        return;
    }

    FrameworkElement animationRoot =
        FindChildByName(frameRoot, L"AnimationRoot");
    if (!animationRoot) {
        Wh_Log(L"Failed to find AnimationRoot");
        return;
    }

    FrameworkElement mainMenu = FindChildByName(animationRoot, L"MainMenu");
    if (!mainMenu) {
        Wh_Log(L"Failed to find MainMenu");
        return;
    }

    Wh_Log(L"Setting size: %dx%d", g_settings.width, g_settings.height);

    if (g_unloading) {
        if (g_originalMainMenuWidth) {
            RestoreWidth(mainMenu, *g_originalMainMenuWidth);
            g_originalMainMenuWidth.reset();
        }
        if (g_originalFrameRootHeight) {
            RestoreHeight(frameRoot, *g_originalFrameRootHeight);
            g_originalFrameRootHeight.reset();
        }
    } else {
        constexpr int kMinWidth = 270;

        if (g_settings.width > 0) {
            if (!g_originalMainMenuWidth) {
                g_originalMainMenuWidth.emplace();
                SaveWidth(mainMenu, *g_originalMainMenuWidth);
            }
            double width =
                static_cast<double>(std::fmax(g_settings.width, kMinWidth));
            mainMenu.Width(width);
            mainMenu.MinWidth(width);
            mainMenu.MaxWidth(width);
        } else if (g_originalMainMenuWidth) {
            RestoreWidth(mainMenu, *g_originalMainMenuWidth);
            g_originalMainMenuWidth.reset();
        }

        if (g_settings.height > 0) {
            if (!g_originalFrameRootHeight) {
                g_originalFrameRootHeight.emplace();
                SaveHeight(frameRoot, *g_originalFrameRootHeight);
            }
            double height = static_cast<double>(g_settings.height);
            frameRoot.Height(height);
            frameRoot.MinHeight(height);
            frameRoot.MaxHeight(height);
        } else if (g_originalFrameRootHeight) {
            RestoreHeight(frameRoot, *g_originalFrameRootHeight);
            g_originalFrameRootHeight.reset();
        }
    }
}

void ApplyStyle() {
    Wh_Log(L"Applying Start menu size");

    auto window = Window::Current();
    FrameworkElement content = window.Content().as<FrameworkElement>();

    winrt::hstring contentClassName = winrt::get_class_name(content);
    Wh_Log(L"Start menu content class name: %s", contentClassName.c_str());

    if (contentClassName == L"Windows.UI.Xaml.Controls.Canvas") {
        ApplyStyleClassicStartMenu(content);
    } else if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleRedesignedStartMenu(content);
    } else {
        Wh_Log(L"Error: Unsupported Start menu content class name");
    }
}

void Init() {
    if (g_layoutUpdatedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    if (!g_visibilityChangedToken) {
        g_visibilityChangedToken = window.VisibilityChanged(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               winrt::Windows::UI::Core::VisibilityChangedEventArgs const&
                   args) {
                Wh_Log(L"Window visibility changed: %d", args.Visible());
                if (args.Visible()) {
                    g_applyStylePending = true;
                }
            });
    }

    auto contentUI = window.Content();
    if (!contentUI) {
        return;
    }

    auto content = contentUI.as<FrameworkElement>();
    g_layoutUpdatedToken = content.LayoutUpdated(
        [](winrt::Windows::Foundation::IInspectable const&,
           winrt::Windows::Foundation::IInspectable const&) {
            if (g_applyStylePending) {
                g_applyStylePending = false;
                ApplyStyle();
            }
        });

    ApplyStyle();
}

void Uninit() {
    if (!g_layoutUpdatedToken) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        return;
    }

    if (g_visibilityChangedToken) {
        window.VisibilityChanged(g_visibilityChangedToken);
        g_visibilityChangedToken = {};
    }

    auto contentUI = window.Content();
    if (!contentUI) {
        return;
    }

    auto content = contentUI.as<FrameworkElement>();
    content.LayoutUpdated(g_layoutUpdatedToken);
    g_layoutUpdatedToken = {};

    ApplyStyle();
}

void SettingsChanged() {
    ApplyStyle();
}

using RoGetActivationFactory_t = decltype(&RoGetActivationFactory);
RoGetActivationFactory_t RoGetActivationFactory_Original;
HRESULT WINAPI RoGetActivationFactory_Hook(HSTRING activatableClassId,
                                           REFIID iid,
                                           void** factory) {
    thread_local static bool isInHook;

    if (isInHook) {
        return RoGetActivationFactory_Original(activatableClassId, iid,
                                               factory);
    }

    isInHook = true;

    if (wcscmp(WindowsGetStringRawBuffer(activatableClassId, nullptr),
               L"Windows.UI.Xaml.Hosting.XamlIsland") == 0) {
        try {
            Init();
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }

    HRESULT ret =
        RoGetActivationFactory_Original(activatableClassId, iid, factory);

    isInHook = false;

    return ret;
}

}  // namespace StartMenuUI

void LoadSettings() {
    g_settings.width = Wh_GetIntSetting(L"width");
    g_settings.height = Wh_GetIntSetting(L"height");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE winrtModule = GetModuleHandle(L"api-ms-win-core-winrt-l1-1-0.dll");
    auto pRoGetActivationFactory =
        (decltype(&RoGetActivationFactory))GetProcAddress(
            winrtModule, "RoGetActivationFactory");
    WindhawkUtils::SetFunctionHook(
        pRoGetActivationFactory, StartMenuUI::RoGetActivationFactory_Hook,
        &StartMenuUI::RoGetActivationFactory_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hCoreWnd = StartMenuUI::GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Initializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { StartMenuUI::Init(); }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hCoreWnd = StartMenuUI::GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Uninitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { StartMenuUI::Uninit(); }, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hCoreWnd = StartMenuUI::GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Applying settings - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { StartMenuUI::SettingsChanged(); }, nullptr);
    }
}
