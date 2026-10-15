// ==WindhawkMod==
// @id              taskbar-flyout-anchor
// @name            Taskbar Flyout Anchor
// @description     Opens the system tray flyouts (hidden icons, Quick Settings, Notification Center, input switcher) right above the button that opened them
// @version         1.0.0
// @author          Mário Barbosa
// @github          https://github.com/mariogomesbarbosa
// @include         explorer.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Flyout Anchor

Makes the system tray flyouts open right above the taskbar button that opened
them, instead of at a fixed position. Useful with custom taskbar layouts (for
example a centered dock made with Windows 11 Taskbar Styler) where the system
tray is no longer at the right edge of the screen.

![Notification Center and Quick Settings anchored above their taskbar buttons](https://raw.githubusercontent.com/mariogomesbarbosa/windhawk-taskbar-flyout-anchor/main/assets/taskbar-flyout-anchor-example.png)

Covered flyouts:

* Hidden icons (the tray overflow, opened by the chevron).
* Quick Settings (Wi-Fi, Bluetooth, volume), anchored to the network/volume
  button group.
* Notification Center and calendar, anchored to the clock/notifications button
  group.
* Input switcher (keyboard layout), anchored to the input indicator.

Flyouts opened with keyboard shortcuts (Win+A, Win+N, Win+Space) are anchored
the same way.

The button positions are read every time a flyout opens, so the flyouts follow
the taskbar when its width changes. If a button position isn't available, the
flyout keeps the default Windows position.

Each flyout has its own settings: alignment (centered on its button, or
right-aligned with the system tray), vertical spacing and minimum screen
margin.

## Notes

* Only Windows 11 with the taskbar at the bottom of the screen is supported.
  With a vertical, top or auto-hidden taskbar, the flyouts keep their default
  position.
* Tested on Windows 11 25H2 (build 26200).
* If the input indicator can't be identified on your Windows build, the input
  switcher is anchored after the indicator is clicked once; until then,
  Win+Space opens it at the default position.
* Mods that also move these flyouts (such as Shell Flyout Positions) shouldn't
  be enabled at the same time.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- overflow:
  - enabled: true
    $name: Anchor this flyout
    $description: Open the hidden icons flyout above the chevron button.
  - alignment: center
    $name: Alignment
    $description: Horizontal alignment of the flyout.
    $options:
    - center: Centered on the button
    - dockRight: Right edge aligned with the right edge of the system tray
  - verticalGap: 8
    $name: Vertical spacing
    $description: >-
      Distance in pixels (at 100% scaling) between the top of the taskbar
      buttons and the bottom of the visible flyout panel. Negative values make
      the flyout overlap the taskbar.
  - screenMargin: 8
    $name: Minimum screen margin
    $description: >-
      Minimum distance in pixels (at 100% scaling, 0 or more) between the
      flyout and the edges of the monitor work area.
  $name: Hidden icons
- quickSettings:
  - enabled: true
    $name: Anchor this flyout
    $description: Open Quick Settings above the network/volume button group.
  - alignment: dockRight
    $name: Alignment
    $description: Horizontal alignment of the flyout.
    $options:
    - center: Centered on the button
    - dockRight: Right edge aligned with the right edge of the system tray
  - verticalGap: 8
    $name: Vertical spacing
    $description: >-
      Distance in pixels (at 100% scaling) between the top of the taskbar
      buttons and the bottom of the visible flyout panel. Negative values make
      the flyout overlap the taskbar.
  - screenMargin: 8
    $name: Minimum screen margin
    $description: >-
      Minimum distance in pixels (at 100% scaling, 0 or more) between the
      flyout and the edges of the monitor work area.
  $name: Quick Settings
- notificationCenter:
  - enabled: true
    $name: Anchor this flyout
    $description: Open the Notification Center and calendar above the clock/notifications button group.
  - alignment: dockRight
    $name: Alignment
    $description: Horizontal alignment of the flyout.
    $options:
    - center: Centered on the button
    - dockRight: Right edge aligned with the right edge of the system tray
  - verticalGap: 8
    $name: Vertical spacing
    $description: >-
      Distance in pixels (at 100% scaling) between the top of the taskbar
      buttons and the bottom of the visible flyout panel. Negative values make
      the flyout overlap the taskbar.
  - screenMargin: 8
    $name: Minimum screen margin
    $description: >-
      Minimum distance in pixels (at 100% scaling, 0 or more) between the
      flyout and the edges of the monitor work area.
  $name: Notification Center
- inputSwitcher:
  - enabled: true
    $name: Anchor this flyout
    $description: Open the keyboard layout flyout above the input indicator.
  - alignment: center
    $name: Alignment
    $description: Horizontal alignment of the flyout.
    $options:
    - center: Centered on the button
    - dockRight: Right edge aligned with the right edge of the system tray
  - verticalGap: 8
    $name: Vertical spacing
    $description: >-
      Distance in pixels (at 100% scaling) between the top of the taskbar
      buttons and the bottom of the visible flyout panel. Negative values make
      the flyout overlap the taskbar.
  - screenMargin: 8
    $name: Minimum screen margin
    $description: >-
      Minimum distance in pixels (at 100% scaling, 0 or more) between the
      flyout and the edges of the monitor work area.
  $name: Input switcher
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

enum class Alignment {
    center,
    dockRight,
};

struct FlyoutSettings {
    bool enabled;
    Alignment alignment;
    int verticalGap;
    int screenMargin;
};

enum class Target {
    Explorer,
    ShellHost,
};

Target g_target;

std::atomic<bool> g_unloading;

// The flyouts handled by the mod.
enum class Flyout {
    overflow,
    quickSettings,
    notificationCenter,
    inputSwitcher,
};

constexpr int kFlyoutCount = 4;

// Indexed by Flyout, each flyout has its own settings group.
FlyoutSettings g_settings[kFlyoutCount];

const FlyoutSettings& GetFlyoutSettings(Flyout flyout) {
    return g_settings[(int)flyout];
}

// The taskbar buttons the flyouts are anchored to.
enum class TrayButton : WPARAM {
    chevron = 1,
    controlCenter,
    notificationCenter,
    inputIndicator,
};

struct FlyoutTraits {
    TrayButton button;

    // The flyout windows are larger than the visible panel: they include a
    // transparent area for the shadow. These are the distances, in DIPs,
    // between each window edge and the visible panel edge (left, top, right,
    // bottom), measured on Windows 11 25H2.
    RECT inset;

    // Quick Settings and the Notification Center are hosted in windows that
    // span the whole work area height, with the panel drawn at the bottom.
    // Their top area is empty, so it may be pushed off-screen.
    bool fullHeight;
};

FlyoutTraits GetFlyoutTraits(Flyout flyout) {
    switch (flyout) {
        case Flyout::overflow:
            return {TrayButton::chevron, {13, 7, 13, 21}, false};
        case Flyout::quickSettings:
            return {TrayButton::controlCenter, {12, 0, 12, 11}, true};
        case Flyout::notificationCenter:
            return {TrayButton::notificationCenter, {12, 0, 12, 15}, true};
        case Flyout::inputSwitcher:
            return {TrayButton::inputIndicator, {12, 12, 12, 14}, false};
    }

    return {};
}

bool IsFlyoutEnabled(Flyout flyout) {
    return GetFlyoutSettings(flyout).enabled;
}

////////////////////////////////////////////////////////////////////////////////
// Data shared between explorer.exe and ShellHost.exe.
//
// ShellHost asks explorer for a button position with a registered window
// message, and explorer writes the answer here. The section is shared by all
// processes that load this mod build, and indexed by session id.

#define SHARED_SECTION __attribute__((section(".shared")))
asm(".section .shared,\"dws\"\n");

struct SharedButtonGeometry {
    LONG requestCounter;
    LONG requestId;  // Written last, identifies the answer below.
    LONG ok;
    RECT button;
    RECT tray;
    LONG scalePercent;
};

SharedButtonGeometry g_sharedGeometry[1024] SHARED_SECTION = {};

SharedButtonGeometry* g_sessionSharedGeometry;

UINT g_queryButtonMsg;

////////////////////////////////////////////////////////////////////////////////
// Taskbar XAML access (explorer.exe only).
//
// There's no public API to get the XamlRoot of a taskbar window. Like other
// taskbar mods, we follow explorer's own objects: the taskbar window's
// CTaskBand returns a shared_ptr<TaskbarHost>, which holds the taskbar XAML
// element. The offset of that element is decoded from the prologue of
// TaskbarHost::FrameHeight. All of these come from taskbar.dll symbols.

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        taskbarElementIUnknownOffset = b[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

void* FindTaskListWndSite(HWND hTaskSwWnd, void* vftable) {
    if (!hTaskSwWnd || !vftable) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != vftable; i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    return taskBandForTaskListWndSite;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    void* site =
        FindTaskListWndSite(hTaskSwWnd, CTaskBand_ITaskListWndSite_vftable);
    if (!site) {
        return nullptr;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(site, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    if (!CSecondaryTaskBand_GetTaskbarHost_Original) {
        return nullptr;
    }

    HWND hTaskSwWnd =
        FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    void* site = FindTaskListWndSite(
        hTaskSwWnd, CSecondaryTaskBand_ITaskListWndSite_vftable);
    if (!site) {
        return nullptr;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(site, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child && enumCallback(child)) {
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

// Depth-limited search for the first descendant matching a predicate.
FrameworkElement FindDescendant(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& predicate,
    int maxDepth) {
    if (maxDepth <= 0) {
        return nullptr;
    }

    FrameworkElement result = nullptr;
    EnumChildElements(element, [&](FrameworkElement child) {
        if (predicate(child)) {
            result = child;
            return true;
        }

        result = FindDescendant(child, predicate, maxDepth - 1);
        return !!result;
    });

    return result;
}

FrameworkElement FindDescendantByClassName(FrameworkElement element,
                                           PCWSTR className,
                                           int maxDepth) {
    return FindDescendant(
        element,
        [className](FrameworkElement child) {
            return winrt::get_class_name(child) == className;
        },
        maxDepth);
}

// The input indicator is a generic SystemTray.IconView, so it's identified by
// its content or data model class instead of its localized name.
bool IsInputIndicatorElement(FrameworkElement element) {
    static constexpr PCWSTR kIdentities[] = {
        L"SystemTray.LanguageSystemTrayIconDataModel",
        L"SystemTray.ImeSystemTrayIconDataModel",
        L"SystemTray.LanguageTextIconContent",
        L"SystemTray.LanguageImageIconContent",
        L"SystemTray.IMEButton",
        L"SystemTray.InputIndicatorButton",
    };

    auto className = winrt::get_class_name(element);
    winrt::hstring dataContextClassName;
    if (auto dataContext = element.DataContext()) {
        dataContextClassName = winrt::get_class_name(dataContext);
    }

    for (PCWSTR identity : kIdentities) {
        if (className == identity || dataContextClassName == identity) {
            return true;
        }
    }

    return false;
}

bool IsEffectivelyVisible(FrameworkElement element, FrameworkElement stopAt) {
    DependencyObject current = element;
    while (current) {
        if (auto fe = current.try_as<UIElement>()) {
            if (fe.Visibility() != Visibility::Visible) {
                return false;
            }
        }

        if (current == stopAt) {
            return true;
        }

        current = Media::VisualTreeHelper::GetParent(current);
    }

    return true;
}

bool IsTrayIconHost(FrameworkElement element) {
    auto className = winrt::get_class_name(element);
    return className == L"SystemTray.IconView" ||
           className == L"SystemTray.NormalButton";
}

// The children of SystemTrayFrameGrid that host other buttons, which can't be
// the input indicator.
bool IsNonInputIndicatorContainer(FrameworkElement element) {
    auto name = element.Name();
    return name == L"NotifyIconStack" || name == L"NotificationAreaIcons" ||
           name == L"ControlCenterButton" ||
           name == L"NotificationCenterButton" || name == L"ShowDesktopStack";
}

// Calls the callback for each visible tray icon host that may be the input
// indicator, stopping when it returns true.
void EnumInputIndicatorCandidates(
    FrameworkElement systemTrayFrameGrid,
    const std::function<bool(FrameworkElement)>& callback) {
    EnumChildElements(systemTrayFrameGrid, [&](FrameworkElement container) {
        if (IsNonInputIndicatorContainer(container) ||
            !IsEffectivelyVisible(container, systemTrayFrameGrid)) {
            return false;
        }

        FrameworkElement found = FindDescendant(
            container,
            [&](FrameworkElement element) {
                return IsTrayIconHost(element) &&
                       IsEffectivelyVisible(element, systemTrayFrameGrid) &&
                       callback(element);
            },
            8);
        return !!found;
    });
}

// Logs the structure of the tray containers that may host the input
// indicator, to help identifying it on new Windows builds.
void LogInputIndicatorCandidates(FrameworkElement systemTrayFrameGrid) {
    std::function<void(FrameworkElement, int)> dump =
        [&](FrameworkElement element, int depth) {
            winrt::hstring dataContextClassName;
            if (auto dataContext = element.DataContext()) {
                dataContextClassName = winrt::get_class_name(dataContext);
            }

            Wh_Log(L"%*s%s #%s [%s]", depth * 2, L"",
                   winrt::get_class_name(element).c_str(),
                   element.Name().c_str(), dataContextClassName.c_str());

            if (depth < 6) {
                EnumChildElements(element, [&](FrameworkElement child) {
                    dump(child, depth + 1);
                    return false;
                });
            }
        };

    EnumChildElements(systemTrayFrameGrid, [&](FrameworkElement container) {
        if (!IsNonInputIndicatorContainer(container)) {
            dump(container, 0);
        }
        return false;
    });
}

// Remembers the input indicator found by a click, for openings with the
// keyboard (Win+Space). Only accessed from the taskbar thread.
winrt::weak_ref<FrameworkElement> g_lastInputIndicator;

std::optional<RECT> GetElementScreenRect(FrameworkElement element,
                                         HWND hIslandWnd,
                                         double scale);

FrameworkElement FindInputIndicator(FrameworkElement systemTrayFrameGrid,
                                    HWND hIslandWnd,
                                    double scale) {
    FrameworkElement result = nullptr;

    // 1. By the class of its content or data model.
    EnumInputIndicatorCandidates(
        systemTrayFrameGrid, [&](FrameworkElement element) {
            if (IsInputIndicatorElement(element) ||
                FindDescendant(element, IsInputIndicatorElement, 12)) {
                result = element;
                return true;
            }
            return false;
        });
    if (result) {
        return result;
    }

    // 2. The tray icon under the cursor, when the flyout is opened by a
    // click.
    POINT pt;
    if (GetCursorPos(&pt)) {
        EnumInputIndicatorCandidates(
            systemTrayFrameGrid, [&](FrameworkElement element) {
                auto rect = GetElementScreenRect(element, hIslandWnd, scale);
                if (rect && PtInRect(&*rect, pt)) {
                    result = element;
                    return true;
                }
                return false;
            });
        if (result) {
            g_lastInputIndicator = winrt::make_weak(result);
            return result;
        }
    }

    // 3. The icon found by the last click, if it's still on this taskbar.
    if (auto last = g_lastInputIndicator.get()) {
        if (last.XamlRoot() == systemTrayFrameGrid.XamlRoot() &&
            IsEffectivelyVisible(last, systemTrayFrameGrid)) {
            return last;
        }
    }

    static bool logged = false;
    if (!logged) {
        logged = true;
        Wh_Log(L"Input indicator not identified, tray structure:");
        LogInputIndicatorCandidates(systemTrayFrameGrid);
    }

    return nullptr;
}

struct ButtonGeometry {
    RECT button;   // Screen rect of the button, in physical pixels.
    RECT tray;     // Screen rect of the system tray frame, in physical pixels.
    double scale;  // XAML rasterization scale (1.0 at 96 DPI).
};

// Converts an element's bounds to screen coordinates. The XAML island fills
// the taskbar window, so island coordinates are offset by the island window.
std::optional<RECT> GetElementScreenRect(FrameworkElement element,
                                         HWND hIslandWnd,
                                         double scale) {
    double width = element.ActualWidth();
    double height = element.ActualHeight();
    if (width <= 0 || height <= 0 ||
        element.Visibility() != Visibility::Visible) {
        return std::nullopt;
    }

    auto bounds = element.TransformToVisual(nullptr).TransformBounds(
        winrt::Windows::Foundation::Rect{0, 0, (float)width, (float)height});

    RECT islandRect;
    if (!GetWindowRect(hIslandWnd, &islandRect)) {
        return std::nullopt;
    }

    return RECT{
        islandRect.left + (LONG)lround(bounds.X * scale),
        islandRect.top + (LONG)lround(bounds.Y * scale),
        islandRect.left + (LONG)lround((bounds.X + bounds.Width) * scale),
        islandRect.top + (LONG)lround((bounds.Y + bounds.Height) * scale),
    };
}

// Must run on the taskbar thread of explorer.exe.
std::optional<ButtonGeometry> GetButtonGeometryFromTaskbarThread(
    HWND hTaskbarWnd,
    TrayButton button) {
    try {
        XamlRoot xamlRoot = nullptr;

        WCHAR className[32];
        GetClassName(hTaskbarWnd, className, ARRAYSIZE(className));
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
            xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
        } else {
            xamlRoot = GetSecondaryTaskbarXamlRoot(hTaskbarWnd);
        }

        if (!xamlRoot) {
            Wh_Log(L"Failed to get XamlRoot");
            return std::nullopt;
        }

        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content) {
            return std::nullopt;
        }

        // Depth-limited search, since custom themes (e.g. Taskbar Styler)
        // may change the layout around the system tray frame.
        FrameworkElement systemTrayFrame = FindDescendantByClassName(
            content, L"SystemTray.SystemTrayFrame", 8);
        if (!systemTrayFrame) {
            Wh_Log(L"SystemTray.SystemTrayFrame not found");
            return std::nullopt;
        }

        FrameworkElement systemTrayFrameGrid =
            FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
        if (!systemTrayFrameGrid) {
            Wh_Log(L"SystemTrayFrameGrid not found");
            return std::nullopt;
        }

        HWND hIslandWnd = FindWindowEx(
            hTaskbarWnd, nullptr,
            L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (!hIslandWnd) {
            Wh_Log(L"XAML island window not found");
            return std::nullopt;
        }

        double scale = xamlRoot.RasterizationScale();

        FrameworkElement buttonElement = nullptr;
        switch (button) {
            case TrayButton::chevron:
                buttonElement =
                    FindChildByName(systemTrayFrameGrid, L"NotifyIconStack");
                break;
            case TrayButton::controlCenter:
                buttonElement = FindChildByName(systemTrayFrameGrid,
                                                L"ControlCenterButton");
                break;
            case TrayButton::notificationCenter:
                buttonElement = FindChildByName(systemTrayFrameGrid,
                                                L"NotificationCenterButton");
                break;
            case TrayButton::inputIndicator:
                buttonElement =
                    FindInputIndicator(systemTrayFrameGrid, hIslandWnd, scale);
                break;
        }

        if (!buttonElement) {
            Wh_Log(L"Button element %d not found", (int)button);
            return std::nullopt;
        }

        auto buttonRect = GetElementScreenRect(buttonElement, hIslandWnd, scale);
        auto trayRect = GetElementScreenRect(systemTrayFrame, hIslandWnd, scale);
        if (!buttonRect || !trayRect) {
            Wh_Log(L"Button %d or tray isn't visible", (int)button);
            return std::nullopt;
        }

        return ButtonGeometry{*buttonRect, *trayRect, scale};
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return std::nullopt;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Button position queries.
//
// The taskbar windows of explorer.exe are subclassed to answer
// g_queryButtonMsg (wParam = button, lParam = request id) from the taskbar
// thread, which is the only thread allowed to access the taskbar XAML.

std::mutex g_subclassedTaskbarsMutex;
std::vector<HWND> g_subclassedTaskbars;

LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR dwRefData) {
    if (uMsg == g_queryButtonMsg && g_queryButtonMsg) {
        auto geometry = GetButtonGeometryFromTaskbarThread(hWnd,
                                                           (TrayButton)wParam);
        SharedButtonGeometry* shared = g_sessionSharedGeometry;
        if (!shared) {
            return 0;
        }

        shared->ok = !!geometry;
        if (geometry) {
            shared->button = geometry->button;
            shared->tray = geometry->tray;
            shared->scalePercent = (LONG)lround(geometry->scale * 100);
        }

        InterlockedExchange(&shared->requestId, (LONG)lParam);
        return geometry ? 1 : 0;
    }

    if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> guard(g_subclassedTaskbarsMutex);
        std::erase(g_subclassedTaskbars, hWnd);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                         TaskbarSubclassProc);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool IsTaskbarWindowClass(HWND hWnd) {
    WCHAR className[32];
    return GetClassName(hWnd, className, ARRAYSIZE(className)) &&
           (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0);
}

void SubclassTaskbarIfNeeded(HWND hWnd) {
    if (g_unloading) {
        return;
    }

    {
        std::lock_guard<std::mutex> guard(g_subclassedTaskbarsMutex);
        if (std::find(g_subclassedTaskbars.begin(), g_subclassedTaskbars.end(),
                      hWnd) != g_subclassedTaskbars.end()) {
            return;
        }

        g_subclassedTaskbars.push_back(hWnd);
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, TaskbarSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass taskbar %p", hWnd);
        std::lock_guard<std::mutex> guard(g_subclassedTaskbarsMutex);
        std::erase(g_subclassedTaskbars, hWnd);
    }
}

void SubclassExistingTaskbars() {
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                IsTaskbarWindowClass(hWnd)) {
                SubclassTaskbarIfNeeded(hWnd);
            }
            return TRUE;
        },
        0);
}

void UnsubclassTaskbars() {
    std::vector<HWND> taskbars;
    {
        std::lock_guard<std::mutex> guard(g_subclassedTaskbarsMutex);
        taskbars.swap(g_subclassedTaskbars);
    }

    for (HWND hWnd : taskbars) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                         TaskbarSubclassProc);
    }
}

thread_local bool g_inButtonQuery;

// Gets the geometry of a button of the given taskbar. Works from any thread
// of explorer.exe or ShellHost.exe.
std::optional<ButtonGeometry> QueryButtonGeometry(HWND hTaskbarWnd,
                                                  TrayButton button) {
    if (g_target == Target::Explorer &&
        GetWindowThreadProcessId(hTaskbarWnd, nullptr) ==
            GetCurrentThreadId()) {
        return GetButtonGeometryFromTaskbarThread(hTaskbarWnd, button);
    }

    SharedButtonGeometry* shared = g_sessionSharedGeometry;
    if (!shared || !g_queryButtonMsg || g_inButtonQuery) {
        return std::nullopt;
    }

    LONG requestId = InterlockedIncrement(&shared->requestCounter);

    // A timeout prevents a deadlock if the taskbar thread is waiting for us.
    // Sent messages may be dispatched while waiting, hence the reentrancy
    // guard.
    g_inButtonQuery = true;
    DWORD_PTR result = 0;
    LRESULT sent = SendMessageTimeout(hTaskbarWnd, g_queryButtonMsg,
                                      (WPARAM)button, requestId,
                                      SMTO_NORMAL | SMTO_ABORTIFHUNG, 300,
                                      &result);
    g_inButtonQuery = false;

    if (!sent || result != 1) {
        Wh_Log(L"Button %d query failed (sent=%d, result=%d)", (int)button,
               (int)sent, (int)result);
        return std::nullopt;
    }

    ButtonGeometry geometry{shared->button, shared->tray,
                            shared->scalePercent / 100.0};
    MemoryBarrier();

    // Another request might have overwritten the answer in the meantime.
    if (InterlockedCompareExchange(&shared->requestId, 0, 0) != requestId ||
        !shared->ok || geometry.scale <= 0) {
        return std::nullopt;
    }

    return geometry;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar windows.

struct FindTaskbarParam {
    HMONITOR monitor;
    HWND primary;
    HWND onMonitor;
};

// Returns the taskbar on the given monitor, or nullptr.
HWND FindTaskbarOnMonitor(HMONITOR monitor, HWND* primaryTaskbar) {
    FindTaskbarParam param{monitor};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* param = (FindTaskbarParam*)lParam;

            WCHAR className[32];
            if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            bool primary = _wcsicmp(className, L"Shell_TrayWnd") == 0;
            if (!primary &&
                _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
                return TRUE;
            }

            if (primary) {
                param->primary = hWnd;
            }

            if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) ==
                param->monitor) {
                param->onMonitor = hWnd;
            }

            return TRUE;
        },
        (LPARAM)&param);

    if (primaryTaskbar) {
        *primaryTaskbar = param.primary;
    }

    return param.onMonitor;
}

// The mod only handles a visible horizontal taskbar at the bottom of its
// monitor. Anything else (vertical/top taskbar, auto-hidden taskbar) keeps the
// default Windows behavior.
bool IsBottomTaskbarVisible(HWND hTaskbarWnd, const MONITORINFO& monitorInfo) {
    RECT rc;
    if (!GetWindowRect(hTaskbarWnd, &rc)) {
        return false;
    }

    const RECT& mon = monitorInfo.rcMonitor;
    if (rc.right - rc.left <= rc.bottom - rc.top) {
        return false;  // Vertical.
    }

    if (rc.top < mon.top + (mon.bottom - mon.top) / 2) {
        return false;  // Top of the screen.
    }

    if (rc.top >= mon.bottom - 2) {
        return false;  // Hidden (auto-hide).
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Flyout positioning.

// Calculates the anchored rect of a flyout window, given the rect Windows
// requested for it. Returns nothing to keep the default position.
std::optional<RECT> CalculateAnchoredRect(Flyout flyout,
                                          HWND hTaskbarWnd,
                                          const RECT& requested) {
    FlyoutTraits traits = GetFlyoutTraits(flyout);

    auto geometry = QueryButtonGeometry(hTaskbarWnd, traits.button);
    if (!geometry) {
        return std::nullopt;
    }

    const RECT& buttonRect = geometry->button;
    const RECT& trayRect = geometry->tray;

    HMONITOR monitor = MonitorFromRect(&buttonRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return std::nullopt;
    }

    if (!IsBottomTaskbarVisible(hTaskbarWnd, monitorInfo)) {
        return std::nullopt;
    }

    const FlyoutSettings& settings = GetFlyoutSettings(flyout);
    double scale = geometry->scale;
    int gap = (int)lround(settings.verticalGap * scale);
    int margin = (int)lround(settings.screenMargin * scale);
    RECT inset{
        (LONG)lround(traits.inset.left * scale),
        (LONG)lround(traits.inset.top * scale),
        (LONG)lround(traits.inset.right * scale),
        (LONG)lround(traits.inset.bottom * scale),
    };

    int cx = requested.right - requested.left;
    int cy = requested.bottom - requested.top;

    // All the math below is done on the visible part of the flyout, so the
    // settings match what the user sees regardless of the shadow area.
    // Windows sometimes sizes a flyout window to the whole monitor before
    // showing it (e.g. the input switcher), such rects are left untouched.
    const RECT& work = monitorInfo.rcWork;
    int visibleWidth = cx - inset.left - inset.right;
    if (visibleWidth <= 0 || visibleWidth > work.right - work.left - 2 * margin ||
        (!traits.fullHeight && cy >= work.bottom - work.top)) {
        return std::nullopt;
    }

    int visibleLeft;
    switch (settings.alignment) {
        case Alignment::dockRight:
            visibleLeft = trayRect.right - visibleWidth;
            break;

        case Alignment::center:
        default:
            visibleLeft = buttonRect.left +
                          (buttonRect.right - buttonRect.left) / 2 -
                          visibleWidth / 2;
            break;
    }

    // Keep the flyout inside the work area horizontally, never cut off.
    visibleLeft =
        std::min(visibleLeft, (int)work.right - margin - visibleWidth);
    visibleLeft = std::max(visibleLeft, (int)work.left + margin);

    int visibleBottom = buttonRect.top - gap;

    RECT result;
    result.left = visibleLeft - inset.left;
    result.right = result.left + cx;
    result.bottom = visibleBottom + inset.bottom;

    // The whole window is moved, keeping its size: resizing the full-height
    // windows from outside doesn't move the panel drawn inside them.
    result.top = result.bottom - cy;

    if (!traits.fullHeight) {
        // Don't let the panel go above the work area. Full-height windows
        // only have their (empty) top area pushed off-screen.
        int minTop = (int)work.top + margin - inset.top;
        if (result.top < minTop) {
            result.top = minTop;
            result.bottom = result.top + cy;
        }
    }

    Wh_Log(L"Flyout %d: button (%d,%d)-(%d,%d), tray (%d,%d)-(%d,%d), "
           L"requested (%d,%d)-(%d,%d) -> (%d,%d)-(%d,%d)",
           (int)flyout, buttonRect.left, buttonRect.top, buttonRect.right,
           buttonRect.bottom, trayRect.left, trayRect.top, trayRect.right,
           trayRect.bottom, requested.left, requested.top, requested.right,
           requested.bottom, result.left, result.top, result.right,
           result.bottom);

    return result;
}

// Picks the taskbar the flyout belongs to and calculates its anchored rect.
// Flyouts open on the monitor of the taskbar that opened them; secondary
// taskbars usually lack some buttons, in which case the default position is
// kept.
std::optional<RECT> AnchorFlyout(Flyout flyout,
                                 const RECT& requested,
                                 std::optional<POINT> monitorHint) {
    if (g_unloading || !IsFlyoutEnabled(flyout)) {
        return std::nullopt;
    }

    HMONITOR monitor =
        monitorHint ? MonitorFromPoint(*monitorHint, MONITOR_DEFAULTTONEAREST)
                    : MonitorFromRect(&requested, MONITOR_DEFAULTTONEAREST);

    HWND hPrimaryTaskbarWnd = nullptr;
    HWND hTaskbarWnd = FindTaskbarOnMonitor(monitor, &hPrimaryTaskbarWnd);

    if (flyout == Flyout::overflow && !hTaskbarWnd) {
        // The chevron is normally only on the primary taskbar.
        hTaskbarWnd = hPrimaryTaskbarWnd;
    }

    if (!hTaskbarWnd) {
        return std::nullopt;
    }

    auto result = CalculateAnchoredRect(flyout, hTaskbarWnd, requested);
    if (!result && flyout == Flyout::overflow &&
        hTaskbarWnd != hPrimaryTaskbarWnd && hPrimaryTaskbarWnd) {
        result = CalculateAnchoredRect(flyout, hPrimaryTaskbarWnd, requested);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Flyout window identification.

std::wstring GetWindowClassName(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return {};
    }

    return className;
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return {};
    }

    WCHAR processPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(processPath);
    bool succeeded = QueryFullProcessImageName(hProcess, 0, processPath, &dwSize);
    CloseHandle(hProcess);
    if (!succeeded) {
        return {};
    }

    PCWSTR fileName = wcsrchr(processPath, L'\\');
    return fileName ? fileName + 1 : processPath;
}

std::wstring GetThreadDescriptionById(DWORD dwThreadId) {
    HANDLE hThread =
        OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, dwThreadId);
    if (!hThread) {
        return {};
    }

    std::wstring result;
    PWSTR description = nullptr;
    if (SUCCEEDED(GetThreadDescription(hThread, &description)) &&
        description) {
        result = description;
        LocalFree(description);
    }

    CloseHandle(hThread);
    return result;
}

std::optional<Flyout> IdentifyExplorerFlyout(HWND hWnd) {
    DWORD dwProcessId;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return std::nullopt;
    }

    std::wstring className = GetWindowClassName(hWnd);
    if (_wcsicmp(className.c_str(), L"TopLevelWindowForOverflowXamlIsland") ==
        0) {
        return Flyout::overflow;
    }

    if (_wcsicmp(className.c_str(), L"Shell_InputSwitchTopLevelWindow") == 0) {
        return Flyout::inputSwitcher;
    }

    return std::nullopt;
}

// The Notification Center is a CoreWindow of ShellExperienceHost.exe, created
// on a thread named "ActionCenter". Explorer uncloaks it when it opens.
std::atomic<HWND> g_notificationCenterWnd;

bool IsNotificationCenterWindow(HWND hWnd) {
    if (hWnd == g_notificationCenterWnd) {
        return true;
    }

    if (_wcsicmp(GetWindowClassName(hWnd).c_str(),
                 L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }

    DWORD dwProcessId;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId ||
        _wcsicmp(GetProcessFileName(dwProcessId).c_str(),
                 L"ShellExperienceHost.exe") != 0 ||
        GetThreadDescriptionById(dwThreadId) != L"ActionCenter") {
        return false;
    }

    g_notificationCenterWnd = hWnd;
    return true;
}

bool IsQuickSettingsWindow(HWND hWnd) {
    DWORD dwProcessId;
    return GetWindowThreadProcessId(hWnd, &dwProcessId) &&
           dwProcessId == GetCurrentProcessId() &&
           _wcsicmp(GetWindowClassName(hWnd).c_str(), L"ControlCenterWindow") ==
               0;
}

// The default rects of the windows that persist between openings, to restore
// them when the mod is disabled.
struct OriginalRect {
    std::mutex mutex;
    HWND hWnd = nullptr;
    RECT rect{};
};

OriginalRect g_notificationCenterOriginal;
OriginalRect g_quickSettingsOriginal;

void RememberOriginalRect(OriginalRect& original, HWND hWnd, const RECT& rc) {
    std::lock_guard<std::mutex> guard(original.mutex);
    original.hWnd = hWnd;
    original.rect = rc;
}

////////////////////////////////////////////////////////////////////////////////
// Hooks.

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

// Applies an anchored rect to SetWindowPos arguments. Missing position or size
// are taken from the current window rect.
bool AdjustSetWindowPosArgs(HWND hWnd,
                            Flyout flyout,
                            int* X,
                            int* Y,
                            int* cx,
                            int* cy,
                            UINT* uFlags,
                            OriginalRect* original) {
    if ((*uFlags & SWP_NOMOVE) && (*uFlags & SWP_NOSIZE)) {
        return false;
    }

    RECT current;
    if (!GetWindowRect(hWnd, &current)) {
        return false;
    }

    int x = (*uFlags & SWP_NOMOVE) ? current.left : *X;
    int y = (*uFlags & SWP_NOMOVE) ? current.top : *Y;
    int width = (*uFlags & SWP_NOSIZE) ? current.right - current.left : *cx;
    int height = (*uFlags & SWP_NOSIZE) ? current.bottom - current.top : *cy;
    RECT requested{x, y, x + width, y + height};

    // The size is kept as requested, only the position changes.
    auto anchored = AnchorFlyout(flyout, requested, std::nullopt);
    if (!anchored) {
        return false;
    }

    // Without an explicit position, the current rect may already be an
    // anchored one, so only explicit positions are remembered.
    if (original && !(*uFlags & SWP_NOMOVE)) {
        RememberOriginalRect(*original, hWnd, requested);
    }

    *X = anchored->left;
    *Y = anchored->top;
    *cx = anchored->right - anchored->left;
    *cy = anchored->bottom - anchored->top;
    *uFlags &= ~(SWP_NOMOVE | SWP_NOSIZE);
    return true;
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    auto original = [&] {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    };

    if (g_target == Target::ShellHost) {
        if (!IsQuickSettingsWindow(hWnd)) {
            return original();
        }

        Wh_Log(L"Quick Settings SetWindowPos (%d,%d) %dx%d flags=0x%X", X, Y,
               cx, cy, uFlags);
        AdjustSetWindowPosArgs(hWnd, Flyout::quickSettings, &X, &Y, &cx, &cy,
                               &uFlags, &g_quickSettingsOriginal);
        return original();
    }

    // explorer.exe
    if (IsTaskbarWindowClass(hWnd)) {
        // Taskbars are subclassed lazily, which also covers taskbars created
        // after the mod was loaded (e.g. after an explorer restart).
        DWORD dwProcessId;
        if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
            dwProcessId == GetCurrentProcessId()) {
            SubclassTaskbarIfNeeded(hWnd);
        }
        return original();
    }

    if (auto flyout = IdentifyExplorerFlyout(hWnd)) {
        Wh_Log(L"Flyout %d SetWindowPos (%d,%d) %dx%d flags=0x%X",
               (int)*flyout, X, Y, cx, cy, uFlags);

        if (*flyout == Flyout::overflow && !(uFlags & SWP_NOMOVE)) {
            // The overflow flyout belongs to the taskbar it was opened from,
            // which is normally the one under the last input.
            DWORD messagePos = GetMessagePos();
            POINT pt{GET_X_LPARAM(messagePos), GET_Y_LPARAM(messagePos)};
            RECT current;
            GetWindowRect(hWnd, &current);
            int width = (uFlags & SWP_NOSIZE) ? current.right - current.left
                                              : cx;
            int height = (uFlags & SWP_NOSIZE) ? current.bottom - current.top
                                               : cy;
            RECT requested{X, Y, X + width, Y + height};
            if (auto anchored = AnchorFlyout(*flyout, requested, pt)) {
                X = anchored->left;
                Y = anchored->top;
            }
            return original();
        }

        AdjustSetWindowPosArgs(hWnd, *flyout, &X, &Y, &cx, &cy, &uFlags,
                               nullptr);
        return original();
    }

    if (!(uFlags & SWP_NOMOVE) && IsNotificationCenterWindow(hWnd)) {
        Wh_Log(L"Notification Center SetWindowPos (%d,%d) %dx%d flags=0x%X",
               X, Y, cx, cy, uFlags);
        AdjustSetWindowPosArgs(hWnd, Flyout::notificationCenter, &X, &Y, &cx,
                               &cy, &uFlags, &g_notificationCenterOriginal);
    }

    return original();
}

using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;
BOOL WINAPI MoveWindow_Hook(HWND hWnd,
                            int X,
                            int Y,
                            int nWidth,
                            int nHeight,
                            BOOL bRepaint) {
    if (g_target == Target::Explorer) {
        if (auto flyout = IdentifyExplorerFlyout(hWnd)) {
            Wh_Log(L"Flyout %d MoveWindow (%d,%d) %dx%d", (int)*flyout, X, Y,
                   nWidth, nHeight);

            RECT requested{X, Y, X + nWidth, Y + nHeight};
            std::optional<POINT> hint;
            if (*flyout == Flyout::overflow) {
                DWORD messagePos = GetMessagePos();
                hint = POINT{GET_X_LPARAM(messagePos),
                             GET_Y_LPARAM(messagePos)};
            }

            if (auto anchored = AnchorFlyout(*flyout, requested, hint)) {
                X = anchored->left;
                Y = anchored->top;
                nWidth = anchored->right - anchored->left;
                nHeight = anchored->bottom - anchored->top;
            }
        }
    }

    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

// Explorer shows the Notification Center by uncloaking it. Moving it right
// before that makes it appear directly at the anchored position.
using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [&] {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };

    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL) ||
        !pvAttribute || *(const BOOL*)pvAttribute) {
        return original();
    }

    if (!IsNotificationCenterWindow(hwnd)) {
        return original();
    }

    RECT current;
    if (GetWindowRect(hwnd, &current)) {
        // The window keeps the anchored rect between openings, so the default
        // rect is only remembered the first time.
        RECT requested = current;
        {
            std::lock_guard<std::mutex> guard(
                g_notificationCenterOriginal.mutex);
            if (g_notificationCenterOriginal.hWnd == hwnd) {
                requested.left = current.left;
                requested.right = current.right;
                requested.top = g_notificationCenterOriginal.rect.top;
                requested.bottom = g_notificationCenterOriginal.rect.bottom;
            }
        }

        Wh_Log(L"Notification Center uncloak (%d,%d)-(%d,%d)", current.left,
               current.top, current.right, current.bottom);

        if (auto anchored =
                AnchorFlyout(Flyout::notificationCenter, requested,
                             std::nullopt)) {
            {
                std::lock_guard<std::mutex> guard(
                    g_notificationCenterOriginal.mutex);
                if (g_notificationCenterOriginal.hWnd != hwnd) {
                    g_notificationCenterOriginal.hWnd = hwnd;
                    g_notificationCenterOriginal.rect = current;
                }
            }

            SetWindowPos_Original(hwnd, nullptr, anchored->left, anchored->top,
                                  anchored->right - anchored->left,
                                  anchored->bottom - anchored->top,
                                  SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    return original();
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle.

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

void LoadSettings() {
    // Settings group names, indexed by Flyout.
    static constexpr PCWSTR kGroups[kFlyoutCount] = {
        L"overflow",
        L"quickSettings",
        L"notificationCenter",
        L"inputSwitcher",
    };

    for (int i = 0; i < kFlyoutCount; i++) {
        FlyoutSettings& settings = g_settings[i];
        PCWSTR group = kGroups[i];

        settings.enabled = Wh_GetIntSetting(L"%s.enabled", group);

        PCWSTR alignment = Wh_GetStringSetting(L"%s.alignment", group);
        settings.alignment = wcscmp(alignment, L"dockRight") == 0
                                 ? Alignment::dockRight
                                 : Alignment::center;
        Wh_FreeStringSetting(alignment);

        settings.verticalGap =
            std::clamp(Wh_GetIntSetting(L"%s.verticalGap", group), -100, 200);
        settings.screenMargin =
            std::clamp(Wh_GetIntSetting(L"%s.screenMargin", group), 0, 200);
    }
}

// Moves the persistent flyout windows back to their default rect.
void RestoreOriginalRect(OriginalRect& original) {
    HWND hWnd;
    RECT rc;
    {
        std::lock_guard<std::mutex> guard(original.mutex);
        hWnd = original.hWnd;
        rc = original.rect;
        original.hWnd = nullptr;
    }

    if (hWnd && IsWindow(hWnd)) {
        SetWindowPos_Original(hWnd, nullptr, rc.left, rc.top,
                              rc.right - rc.left, rc.bottom - rc.top,
                              SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WCHAR moduleFilePath[MAX_PATH];
    DWORD len =
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath));
    if (len == 0 || len == ARRAYSIZE(moduleFilePath)) {
        Wh_Log(L"GetModuleFileName failed");
        return FALSE;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    moduleFileName = moduleFileName ? moduleFileName + 1 : moduleFilePath;
    g_target = _wcsicmp(moduleFileName, L"ShellHost.exe") == 0
                   ? Target::ShellHost
                   : Target::Explorer;

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId < ARRAYSIZE(g_sharedGeometry)) {
        g_sessionSharedGeometry = &g_sharedGeometry[sessionId];
    }

    g_queryButtonMsg =
        RegisterWindowMessage(L"TaskbarFlyoutAnchor_QueryButton_" WH_MOD_ID);

    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);

    if (g_target == Target::ShellHost) {
        return TRUE;
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Failed to hook taskbar.dll symbols");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(MoveWindow, MoveWindow_Hook,
                                   &MoveWindow_Original);

    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (DwmSetWindowAttribute_t)GetProcAddress(dwmapiModule,
                                                    "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::SetFunctionHook(pDwmSetWindowAttribute,
                                           DwmSetWindowAttribute_Hook,
                                           &DwmSetWindowAttribute_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::Explorer) {
        SubclassExistingTaskbars();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        UnsubclassTaskbars();
        RestoreOriginalRect(g_notificationCenterOriginal);
    } else {
        RestoreOriginalRect(g_quickSettingsOriginal);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    // Restore the persistent flyouts, they're anchored again with the new
    // settings the next time they open.
    if (g_target == Target::Explorer) {
        if (!IsFlyoutEnabled(Flyout::notificationCenter)) {
            RestoreOriginalRect(g_notificationCenterOriginal);
        }
    } else {
        if (!IsFlyoutEnabled(Flyout::quickSettings)) {
            RestoreOriginalRect(g_quickSettingsOriginal);
        }
    }
}
