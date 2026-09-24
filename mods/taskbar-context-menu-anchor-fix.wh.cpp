// ==WindhawkMod==
// @id              taskbar-context-menu-anchor-fix
// @name            Taskbar context menu anchor fix
// @description     Repositions taskbar tray icon context menus (e.g. Notification/Action Center) to open near the click point instead of a wrong fixed position
// @version         8.0.0
// @author          kuba
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar context menu anchor fix

EXPERIMENTAL. Fixes the right-click context menu on certain taskbar tray
icons (for example the Notification Center / clock area, "Adjust date and
time" / "Notification settings") appearing far away from where you actually
clicked.

## How it works (v8)

The taskbar's tray context menus are XAML `MenuFlyout`s. Earlier versions
moved the finished popup window (`Xaml_WindowedPopupClass`) with
`SetWindowPos`. That put the menu in the right place visually, but XAML
still thought the popup was at its original position, so mouse input was
hit-tested against the wrong place: the menu items didn't highlight on
hover and couldn't be clicked.

This version instead hooks `MenuFlyout::ShowAt` in the taskbar's own code and
changes the placement it asks XAML for: the menu is centered horizontally on
the click and opens just above the taskbar's top edge, with a configurable
gap. XAML positions the popup itself, so the position and the input stay in
sync.

Only menus opened while the cursor is within a configurable distance of the
taskbar's right edge ("status bar zone" - system tray, clock,
notification/action center) are changed. Everything else (Start button,
pinned apps, task list, etc.) is left completely untouched.

Note: XAML keeps popups inside the monitor's work area, so a negative gap
might not be able to push the menu over the taskbar.

## If something looks wrong

Turn on logging for this mod and reproduce the issue; the log shows the
decision made for every menu flyout the taskbar opens.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- statusBarZoneWidth: 700
  $name: Status bar zone width (px)
  $description: >-
    Only clicks within this many pixels of the taskbar's right edge are
    treated as "status bar" clicks (system tray, clock, notification/action
    center). Clicks further left (Start button, pinned apps, task list) are
    left completely untouched.
- menuGapFromTaskbar: 6
  $name: Gap above taskbar (px)
  $description: >-
    Vertical gap between the bottom of the repositioned menu and the top
    edge of the taskbar.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>

using namespace winrt::Windows::UI::Xaml;

struct {
    int statusBarZoneWidth;
    int menuGapFromTaskbar;
} g_settings;

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

constexpr WCHAR kContentBridgeClassName[] =
    L"Windows.UI.Composition.DesktopWindowContentBridge";

// Adjusts the flyout show options so that the menu opens centered on the
// click, just above the taskbar. Returns false if the menu should be left
// untouched.
bool AdjustShowOptions(DependencyObject* placementTarget,
                       Controls::Primitives::FlyoutShowOptions* showOptions) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    HWND hTaskbarWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);
    if (!hTaskbarWnd || !IsTaskbarWindow(hTaskbarWnd)) {
        Wh_Log(L"Cursor (%d,%d) isn't over a taskbar, skipping", pt.x, pt.y);
        return false;
    }

    RECT taskbarRc{};
    if (!GetWindowRect(hTaskbarWnd, &taskbarRc)) {
        return false;
    }

    bool inStatusBarZone =
        pt.x >= taskbarRc.right - g_settings.statusBarZoneWidth;

    Wh_Log(L"Menu flyout, click at (%d,%d), taskbar (%d,%d)-(%d,%d), "
           L"inStatusBarZone=%d",
           pt.x, pt.y, (int)taskbarRc.left, (int)taskbarRc.top,
           (int)taskbarRc.right, (int)taskbarRc.bottom, inStatusBarZone);

    if (!inStatusBarZone) {
        return false;
    }

    UIElement targetElement = placementTarget->try_as<UIElement>();
    if (!targetElement) {
        Wh_Log(L"Placement target isn't a UIElement");
        return false;
    }

    auto xamlRoot = targetElement.XamlRoot();
    if (!xamlRoot) {
        Wh_Log(L"No XamlRoot");
        return false;
    }

    double scale = xamlRoot.RasterizationScale();
    if (scale <= 0) {
        scale = 1;
    }

    // The XAML island's origin on screen. XAML coordinates relative to the
    // root are relative to this window.
    POINT islandOrigin{};
    HWND hBridgeWnd =
        FindWindowEx(hTaskbarWnd, nullptr, kContentBridgeClassName, nullptr);
    RECT bridgeRc{};
    if (hBridgeWnd && GetWindowRect(hBridgeWnd, &bridgeRc)) {
        islandOrigin = {bridgeRc.left, bridgeRc.top};
    } else {
        Wh_Log(L"No DesktopWindowContentBridge, using the taskbar client area");
        ClientToScreen(hTaskbarWnd, &islandOrigin);
    }

    // The target's top-left corner, relative to the island, in DIPs.
    winrt::Windows::Foundation::Point targetOrigin =
        targetElement.TransformToVisual(nullptr).TransformPoint(
            winrt::Windows::Foundation::Point{0, 0});

    // The anchor point in screen pixels: horizontally at the click,
    // vertically at the taskbar's top edge minus the gap. With the Top
    // placement, XAML opens the menu above this point, centered on it.
    int anchorX = pt.x;
    int anchorY = taskbarRc.top - g_settings.menuGapFromTaskbar;

    winrt::Windows::Foundation::Point position{
        static_cast<float>((anchorX - islandOrigin.x) / scale -
                           targetOrigin.X),
        static_cast<float>((anchorY - islandOrigin.y) / scale -
                           targetOrigin.Y),
    };

    Wh_Log(L"Island origin (%d,%d), scale %f, target origin (%f,%f), "
           L"placement %d -> Top, position (%f,%f)",
           islandOrigin.x, islandOrigin.y, scale, targetOrigin.X,
           targetOrigin.Y, (int)showOptions->Placement(), position.X,
           position.Y);

    showOptions->Placement(Controls::Primitives::FlyoutPlacementMode::Top);
    showOptions->Position(position);

    return true;
}

// The ShowAt call is a C++/WinRT template instantiated separately in every
// module that uses it, so it may need to be hooked in several modules. Each
// hooked instance needs its own original function pointer, hence the
// template.
using ShowAt_t =
    void(WINAPI*)(void* pThis,
                  DependencyObject* placementTarget,
                  Controls::Primitives::FlyoutShowOptions* showOptions);

template <int N>
struct ShowAtHook {
    static inline ShowAt_t original;

    static void WINAPI Hook(
        void* pThis,
        DependencyObject* placementTarget,
        Controls::Primitives::FlyoutShowOptions* showOptions) {
        Wh_Log(L">");

        if (placementTarget && showOptions) {
            try {
                AdjustShowOptions(placementTarget, showOptions);
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Error %08X", hr);
            }
        }

        original(pThis, placementTarget, showOptions);
    }
};

constexpr WCHAR kMenuFlyoutShowAtSymbol[] =
    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_Controls_Primitives_IFlyoutBase5<struct winrt::Windows::UI::Xaml::Controls::MenuFlyout>::ShowAt(struct winrt::Windows::UI::Xaml::DependencyObject const &,struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutShowOptions const &)const )";
constexpr WCHAR kFlyoutBaseShowAtSymbol[] =
    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_Controls_Primitives_IFlyoutBase5<struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutBase>::ShowAt(struct winrt::Windows::UI::Xaml::DependencyObject const &,struct winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutShowOptions const &)const )";

// The modules that host the taskbar's XAML code, depending on the Windows
// version.
constexpr PCWSTR kModuleNames[] = {
    L"SystemTray.dll",
    L"Taskbar.View.dll",
    L"ExplorerExtensions.dll",
};

std::atomic<bool> g_moduleHooked[ARRAYSIZE(kModuleNames)];

template <int N>
bool HookModuleSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {kMenuFlyoutShowAtSymbol},
            &ShowAtHook<N * 2>::original,
            ShowAtHook<N * 2>::Hook,
            true,
        },
        {
            {kFlyoutBaseShowAtSymbol},
            &ShowAtHook<N * 2 + 1>::original,
            ShowAtHook<N * 2 + 1>::Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, symbolHooks,
                                      ARRAYSIZE(symbolHooks));
}

bool HookModuleIfNeeded(HMODULE module) {
    bool hooked = false;

    for (int i = 0; i < ARRAYSIZE(kModuleNames); i++) {
        if (g_moduleHooked[i] || GetModuleHandle(kModuleNames[i]) != module ||
            g_moduleHooked[i].exchange(true)) {
            continue;
        }

        Wh_Log(L"Hooking %s", kModuleNames[i]);

        bool result = false;
        switch (i) {
            case 0:
                result = HookModuleSymbols<0>(module);
                break;
            case 1:
                result = HookModuleSymbols<1>(module);
                break;
            case 2:
                result = HookModuleSymbols<2>(module);
                break;
        }

        if (!result) {
            Wh_Log(L"HookSymbols failed for %s", kModuleNames[i]);
        }

        hooked = hooked || result;
    }

    return hooked;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && HookModuleIfNeeded(module)) {
        Wh_ApplyHookOperations();
    }

    return module;
}

void LoadSettings() {
    g_settings.statusBarZoneWidth = Wh_GetIntSetting(L"statusBarZoneWidth");
    g_settings.menuGapFromTaskbar = Wh_GetIntSetting(L"menuGapFromTaskbar");
    Wh_Log(L"Settings loaded: statusBarZoneWidth=%d menuGapFromTaskbar=%d",
           g_settings.statusBarZoneWidth, g_settings.menuGapFromTaskbar);
}

BOOL Wh_ModInit(void) {
    Wh_Log(L"Init");

    LoadSettings();

    for (PCWSTR moduleName : kModuleNames) {
        if (HMODULE module = GetModuleHandle(moduleName)) {
            HookModuleIfNeeded(module);
        }
    }

    // Some of the modules may load later.
    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW =
        (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                  "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit(void) {
    // Handle modules that were loaded while the mod was initializing.
    bool hooked = false;
    for (PCWSTR moduleName : kModuleNames) {
        if (HMODULE module = GetModuleHandle(moduleName)) {
            hooked = HookModuleIfNeeded(module) || hooked;
        }
    }

    if (hooked) {
        Wh_ApplyHookOperations();
    }
}

void Wh_ModUninit(void) {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
}
