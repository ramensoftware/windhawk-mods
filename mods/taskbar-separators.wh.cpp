// ==WindhawkMod==
// @id              taskbar-separators
// @name            Taskbar Separators
// @description     Add customizable visual separators between Windows 11 taskbar application buttons.
// @version         1.0.0
// @author          digART
// @github          https://github.com/digart11
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Copyright (C) 2026 digART
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// Taskbar hook and UI-thread infrastructure includes code adapted from
// Windhawk mods by Michael Maltsev (m417z), including Taskbar Labels
// for Windows 11.


// ==WindhawkModReadme==
/*
# Taskbar Separators

Add clean, customizable separators between application buttons on the
Windows 11 taskbar.

Unlike placeholder applications or pinned shortcuts, these separators are
visual, non-clickable elements and do not launch programs or occupy normal
application slots.

## Features

- Add multiple separators at configurable taskbar positions
- Live position and appearance updates
- Five configurable visual styles:
  - Fade
  - Solid
  - Double
  - Rounded
  - Glow
- Adjustable thickness, height, opacity, color, and effect settings
- Automatic horizontal and vertical taskbar orientation
- Clean removal when the mod is disabled
- No changes to taskbar button margins, padding, or application behavior

## Getting started

1. Open the mod's **Settings** tab.
2. Add separator positions to the **Separators** list.
3. A position of `3` places a separator after the third application button.
4. Select a style and adjust its appearance.

## Position behavior

Positions refer to the current visual order of taskbar application buttons.

Opening, closing, pinning, unpinning, or rearranging applications can change
which icons are beside a configured separator position. The separator remains
attached to its numeric position in the taskbar order.

## Animation compatibility

Static taskbars are fully supported.

The mod can follow icons animated by other taskbar mods, but very fast animation
may not remain perfectly synchronized because both mods update their visual
elements independently.

This affects animation appearance only and does not affect normal static
separator positioning.

## Compatibility

- Windows 11
- Standard horizontal taskbars
- Vertical taskbars provided by compatible customization mods (not tested)
- Compatible with Windows 11 Taskbar Styler in normal configurations

## License and attribution

Licensed under the GNU General Public License v3.0.

Taskbar hook and UI-thread infrastructure includes code adapted from Windhawk
mods by Michael Maltsev (m417z).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 2
  $name: Thickness
  $description: Divider thickness in pixels, from 1 to 8.
- height: 22
  $name: Length
  $description: Divider length in pixels, from 4 to 48.
- opacity: 100
  $name: Opacity
  $description: Divider opacity as a percentage, from 0 to 100.
- color: "#FFFF00"
  $name: Color
  $description: "Divider color in #RRGGBB or #AARRGGBB format."
- style: fade
  $name: Style
  $description: "Solid: Plain crisp separator line. Fade: Soft layered separator with faded ends. Double: Two parallel separator lines. Rounded: Separator with rounded ends. Glow: Separator with a soft surrounding glow."
  $options:
  - solid: Solid
  - fade: Fade
  - double: Double
  - rounded: Rounded
  - glow: Glow
- cornerRadius: 2
  $name: Corner radius
  $description: Rounded style corner radius in pixels, from 0 to 12.
- fadeAmount: 70
  $name: Fade amount
  $description: Fade style end-region size as a percentage, from 0 to 100.
- glowSize: 4
  $name: Glow size
  $description: Glow style expansion in pixels, from 0 to 16.
- glowOpacity: 30
  $name: Glow opacity
  $description: Glow layer opacity as a percentage, from 0 to 100.
- doubleGap: 3
  $name: Double gap
  $description: Distance between double-line centers in pixels, from 1 to 12.
- orientation: auto
  $name: Taskbar orientation
  $description: Automatic uses the direction between adjacent realized icons.
  $options:
  - auto: Automatic
  - horizontal: Horizontal taskbar
  - vertical: Vertical taskbar
- animationCompatibility: "on"
  $name: Animation compatibility
  $description: Track animated taskbar icons. Turn off for static separators with no animation-tracking overhead.
  $options:
  - "on": On
  - "off": Off
- separatorBeforeFirstApp: false
  $name: Separator before first app
  $description: Show a separator before the first taskbar application button.
- separators:
  - 3
  $name: Separators
  $description: Application button positions after which to place dividers.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
#include <winrt/Windows.UI.Xaml.Automation.h>
#endif
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

namespace {

enum class SeparatorMode {
    Overlay,
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
    NativePlaceholderExperimental,
#endif
};

#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
constexpr SeparatorMode kSeparatorMode =
    SeparatorMode::NativePlaceholderExperimental;
#else
constexpr SeparatorMode kSeparatorMode = SeparatorMode::Overlay;
#endif
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
constexpr PCWSTR kNativePlaceholderAutomationName =
    L"Windhawk Separator Placeholder";
#endif

enum class ReconcileResult {
    succeeded,
    temporarilyNotReady,
    noValidSeparators,
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
    placeholderNotFound,
#endif
};

enum class TaskbarOrientation {
    horizontal,
    vertical,
};

enum class OrientationSetting {
    automatic,
    horizontal,
    vertical,
};

enum class DividerStyle {
    solid,
    rounded,
    fade,
    glow,
    doubleLine,
};

struct SeparatorSettings {
    size_t settingsIndex = 0;
    int position = 3;
};

struct Settings {
    int width = 2;
    int height = 22;
    int opacityPercent = 100;
    winrt::Windows::UI::Color color{255, 255, 255, 0};
    DividerStyle style = DividerStyle::fade;
    int cornerRadius = 2;
    int fadeAmount = 70;
    int glowSize = 4;
    int glowOpacityPercent = 30;
    int doubleGap = 3;
    OrientationSetting orientation = OrientationSetting::automatic;
    bool animationCompatibility = true;
    bool separatorBeforeFirstApp = false;
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
    int nativeTestButtonIndex = 0;
#endif
    std::vector<SeparatorSettings> separators;
};

#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
struct NativeHiddenElementState {
    winrt::weak_ref<FrameworkElement> element;
    double opacity = 1;
    bool isHitTestVisible = true;
};

struct NativeForcedAncestorState {
    winrt::weak_ref<FrameworkElement> element;
    Visibility visibility = Visibility::Visible;
    double opacity = 1;
};

struct NativePlaceholderState {
    winrt::weak_ref<FrameworkElement> button;
    winrt::weak_ref<Controls::Panel> separatorHost;
    winrt::weak_ref<Shapes::Rectangle> separator;
    winrt::weak_ref<Controls::Panel> nativeHostParent;
    winrt::weak_ref<Controls::Grid> nativeHost;
    double width = 0;
    double minWidth = 0;
    double maxWidth = 0;
    Thickness margin{};
    bool hasControlPadding = false;
    Thickness controlPadding{};
    winrt::Windows::Foundation::IInspectable toolTip{nullptr};
    std::vector<NativeHiddenElementState> hiddenElements;
    std::vector<NativeForcedAncestorState> forcedAncestors;
    bool dividerVisibilityLogged = false;
    bool dividerZeroSizeLogged = false;
    bool diagnosticsLogged = false;
    bool invalidHostLogged = false;
    bool hostReadyLogged = false;
    bool transformGroupLogged = false;
};

enum class NativePlaceholderLogState {
    unknown,
    found,
    notFound,
};
#endif

std::mutex g_settingsMutex;
Settings g_settings;
std::atomic<unsigned int> g_settingsGeneration{0};

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_unloading{false};

thread_local winrt::weak_ref<FrameworkElement>
    g_taskbarFrameRepeaterForCurrentThread;
thread_local unsigned int g_appliedSettingsGeneration = 0;

thread_local winrt::weak_ref<Controls::Canvas> g_animationOverlayCanvas;
struct AnimationDividerCache {
    winrt::weak_ref<Controls::Canvas> host;
    winrt::weak_ref<FrameworkElement> previousIcon;
    winrt::weak_ref<FrameworkElement> targetIcon;
    winrt::weak_ref<FrameworkElement> nextIcon;
    TaskbarOrientation orientation = TaskbarOrientation::horizontal;
    bool beforeFirst = false;
    bool hasLastPosition = false;
    double lastLeft = 0;
    double lastTop = 0;
};
thread_local std::vector<AnimationDividerCache> g_animationDividers;
thread_local winrt::weak_ref<FrameworkElement> g_animationPointerSource;
thread_local winrt::Windows::Foundation::IInspectable
    g_animationPointerMovedHandler{nullptr};
thread_local winrt::event_token g_animationPointerExitedToken{};
thread_local winrt::event_token g_animationRenderingToken{};
thread_local bool g_animationPointerHandlersAttached = false;
thread_local bool g_animationPointerExitedHandlerAttached = false;
thread_local bool g_animationRenderingSubscribed = false;
thread_local bool g_animationPointerInside = false;
thread_local bool g_animationRenderingCallbackActive = false;
thread_local bool g_animationSubscriptionOrderRefreshed = false;
thread_local int g_animationStableFrames = 0;
using AnimationClock = std::chrono::steady_clock;
thread_local AnimationClock::time_point g_animationLastActivity{};
constexpr auto kAnimationTrackingTimeout = std::chrono::seconds(3);

thread_local DispatcherTimer g_initialReconcileTimer{nullptr};
thread_local winrt::event_token g_initialReconcileTimerToken{};
thread_local int g_initialReconcileAttempts = 0;

#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
thread_local NativePlaceholderState g_nativePlaceholderState;
thread_local NativePlaceholderLogState g_nativePlaceholderLogState =
    NativePlaceholderLogState::unknown;
thread_local int g_nativeLoggedSelectedButtonIndex = 0;
thread_local int g_nativeLoggedRequestedButtonIndex = -1;
thread_local winrt::weak_ref<FrameworkElement> g_nativeTreeDumpedButton;
thread_local int g_nativeTreeDumpedRequestedButtonIndex = -1;
#endif

void CancelInitialReconcileRetry();

int ClampSetting(PCWSTR, int value, int minimum, int maximum) {
    return std::clamp(value, minimum, maximum);
}

int HexDigitValue(wchar_t character) {
    if (character >= L'0' && character <= L'9') {
        return character - L'0';
    }

    if (character >= L'a' && character <= L'f') {
        return character - L'a' + 10;
    }

    if (character >= L'A' && character <= L'F') {
        return character - L'A' + 10;
    }

    return -1;
}

bool ParseHexByte(PCWSTR text, uint8_t* result) {
    int high = HexDigitValue(text[0]);
    int low = HexDigitValue(text[1]);
    if (high < 0 || low < 0) {
        return false;
    }

    *result = static_cast<uint8_t>((high << 4) | low);
    return true;
}

bool ParseColor(PCWSTR text, winrt::Windows::UI::Color* result) {
    if (!text || text[0] != L'#') {
        return false;
    }

    size_t length = wcslen(text);
    if (length != 7 && length != 9) {
        return false;
    }

    winrt::Windows::UI::Color color{255, 255, 255, 255};
    size_t rgbOffset = 1;

    if (length == 9) {
        if (!ParseHexByte(text + 1, &color.A)) {
            return false;
        }
        rgbOffset = 3;
    }

    if (!ParseHexByte(text + rgbOffset, &color.R) ||
        !ParseHexByte(text + rgbOffset + 2, &color.G) ||
        !ParseHexByte(text + rgbOffset + 4, &color.B)) {
        return false;
    }

    *result = color;
    return true;
}

DividerStyle ParseDividerStyle(PCWSTR text) {
    if (text && wcscmp(text, L"solid") == 0) {
        return DividerStyle::solid;
    }
    if (text && wcscmp(text, L"rounded") == 0) {
        return DividerStyle::rounded;
    }
    if (text && wcscmp(text, L"fade") == 0) {
        return DividerStyle::fade;
    }
    if (text && wcscmp(text, L"glow") == 0) {
        return DividerStyle::glow;
    }
    if (text && wcscmp(text, L"double") == 0) {
        return DividerStyle::doubleLine;
    }
    return DividerStyle::fade;
}

void LoadSettings(bool settingsWereChanged = false) {
    Settings settings;

    settings.width =
        ClampSetting(L"width", Wh_GetIntSetting(L"width"), 1, 8);
    settings.height =
        ClampSetting(L"height", Wh_GetIntSetting(L"height"), 4, 48);
    settings.opacityPercent =
        ClampSetting(L"opacity", Wh_GetIntSetting(L"opacity"), 0, 100);

    PCWSTR colorText = Wh_GetStringSetting(L"color");
    if (!ParseColor(colorText, &settings.color)) {
        Wh_Log(L"SETTINGS: invalid color; using #FFFF00");
        settings.color =
            winrt::Windows::UI::Color{255, 255, 255, 0};
    }
    Wh_FreeStringSetting(colorText);

    PCWSTR styleText = Wh_GetStringSetting(L"style");
    settings.style = ParseDividerStyle(styleText);
    Wh_FreeStringSetting(styleText);
    settings.cornerRadius = ClampSetting(
        L"cornerRadius", Wh_GetIntSetting(L"cornerRadius"), 0, 12);
    settings.fadeAmount = ClampSetting(
        L"fadeAmount", Wh_GetIntSetting(L"fadeAmount"), 0, 100);
    settings.glowSize = ClampSetting(
        L"glowSize", Wh_GetIntSetting(L"glowSize"), 0, 16);
    settings.glowOpacityPercent = ClampSetting(
        L"glowOpacity", Wh_GetIntSetting(L"glowOpacity"), 0, 100);
    settings.doubleGap = ClampSetting(
        L"doubleGap", Wh_GetIntSetting(L"doubleGap"), 1, 12);

    PCWSTR orientationText = Wh_GetStringSetting(L"orientation");
    if (orientationText && wcscmp(orientationText, L"horizontal") == 0) {
        settings.orientation = OrientationSetting::horizontal;
    } else if (orientationText &&
               wcscmp(orientationText, L"vertical") == 0) {
        settings.orientation = OrientationSetting::vertical;
    }
    Wh_FreeStringSetting(orientationText);

#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
    settings.nativeTestButtonIndex =
        Wh_GetIntSetting(L"nativeTestButtonIndex");
    if (settings.nativeTestButtonIndex < 0) {
        settings.nativeTestButtonIndex = 0;
    }
#endif

    PCWSTR animationCompatibilityText =
        Wh_GetStringSetting(L"animationCompatibility");
    settings.animationCompatibility =
        !animationCompatibilityText ||
        wcscmp(animationCompatibilityText, L"off") != 0;
    Wh_FreeStringSetting(animationCompatibilityText);
    settings.separatorBeforeFirstApp =
        Wh_GetIntSetting(L"separatorBeforeFirstApp") != 0;

    constexpr PCWSTR kSettingsFormatValue =
        L"taskbar-separators-settings-format";
    int settingsFormat = Wh_GetIntValue(kSettingsFormatValue, 0);

    auto appendSeparator = [&](size_t settingsIndex, int position) {
        SeparatorSettings separator;
        separator.settingsIndex = settingsIndex;
        separator.position = std::max(position, 1);
        settings.separators.push_back(separator);
    };

    if (!settingsWereChanged && settingsFormat == 2) {
        // Interpret the previous array-of-objects format until the user saves
        // the new compact settings schema.
        for (int index = 0; index < 128; index++) {
            int position =
                Wh_GetIntSetting(L"separators[%d].position", index);
            if (position == 0) {
                break;
            }
            appendSeparator(static_cast<size_t>(index), position);
        }
    } else if (!settingsWereChanged && settingsFormat == 1) {
        // Interpret the original single-divider settings as index zero.
        int position = Wh_GetIntSetting(L"position");
        appendSeparator(0, position != 0 ? position : 3);
    } else {
        for (int index = 0; index < 128; index++) {
            int position = Wh_GetIntSetting(L"separators[%d]", index);
            if (position == 0) {
                break;
            }
            appendSeparator(static_cast<size_t>(index), position);
        }
        Wh_SetIntValue(kSettingsFormatValue, 3);
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = settings;
    }

    g_settingsGeneration.fetch_add(1, std::memory_order_release);
    Wh_Log(L"SETTINGS: loaded %zu dividers", settings.separators.size());
}

Settings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

bool ColorsEqual(winrt::Windows::UI::Color const& left,
                 winrt::Windows::UI::Color const& right) {
    return left.A == right.A && left.R == right.R && left.G == right.G &&
           left.B == right.B;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];

            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWnd));

    return taskbarWnd;
}

HWND GetTaskbarUiWnd() {
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        return nullptr;
    }

    return FindWindowEx(
        taskbarWnd, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

// Adapted from the official Windows 11 Taskbar Styler mod. SendMessage is
// synchronous, so the callback has completed before this function returns.
bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = reinterpret_cast<
                        RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

// Adapted from the official Taskbar Multirow mod. This obtains the existing
// taskbar XamlRoot without installing another XAML diagnostics client, which
// keeps this mod compatible with Taskbar Styler.
XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1]) {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

#if defined(_M_X64)
    size_t taskbarElementIUnknownOffset = 0x48;

    const BYTE* code = static_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        taskbarElementIUnknownOffset = code[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight prologue");
    }

    auto* taskbarElementIUnknown =
        *reinterpret_cast<IUnknown**>(
            static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
            taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    }

    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
#else
    if (taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }
    return nullptr;
#endif
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    HWND taskSwitchWnd =
        reinterpret_cast<HWND>(GetProp(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwitchWnd) {
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtr(taskSwitchWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0;
         *reinterpret_cast<void**>(taskBandForTaskListWndSite) !=
         CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite =
            reinterpret_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// {0BD894F2-EDFC-5DDF-A166-2DB14BBFDF35}
constexpr winrt::guid IItemsRepeater{
    0x0BD894F2,
    0xEDFC,
    0x5DDF,
    {0xA1, 0x66, 0x2D, 0xB1, 0x4B, 0xBF, 0xDF, 0x35}};

int ItemsRepeater_GetElementIndex(FrameworkElement repeater,
                                  UIElement element) {
    winrt::Windows::Foundation::IUnknown repeaterUnknown = nullptr;
    repeater.as(IItemsRepeater, winrt::put_abi(repeaterUnknown));

    using GetElementIndex_t =
        HRESULT(WINAPI*)(void* pThis, void* element, void* index);

    void** vtable = *(void***)winrt::get_abi(repeaterUnknown);
    auto getElementIndex = (GetElementIndex_t)vtable[19];

    int index = -1;
    getElementIndex(winrt::get_abi(repeaterUnknown), winrt::get_abi(element),
                    &index);
    return index;
}

FrameworkElement ItemsRepeater_TryGetElement(FrameworkElement repeater,
                                             int index) {
    winrt::Windows::Foundation::IUnknown repeaterUnknown = nullptr;
    repeater.as(IItemsRepeater, winrt::put_abi(repeaterUnknown));

    using TryGetElement_t =
        HRESULT(WINAPI*)(void* pThis, int index, void** uiElement);

    void** vtable = *(void***)winrt::get_abi(repeaterUnknown);
    auto tryGetElement = (TryGetElement_t)vtable[20];

    void* uiElement = nullptr;
    tryGetElement(winrt::get_abi(repeaterUnknown), index, &uiElement);

    if (!uiElement) {
        return nullptr;
    }

    return UIElement{uiElement, winrt::take_ownership_from_abi}
        .try_as<FrameworkElement>();
}

FrameworkElement FindRepeaterAncestor(FrameworkElement element) {
    auto current = element;

    for (int depth = 0; depth < 16 && current; depth++) {
        auto parentObject = Media::VisualTreeHelper::GetParent(current);
        auto parent = parentObject.try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (parent.Name() == L"TaskbarFrameRepeater") {
            return parent;
        }

        current = parent;
    }

    return nullptr;
}

FrameworkElement FindDescendantByName(FrameworkElement root,
                                      PCWSTR name,
                                      int depth = 0) {
    if (!root || depth > 12) {
        return nullptr;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < childCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(root, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (child.Name() == name) {
            return child;
        }

        auto found = FindDescendantByName(child, name, depth + 1);
        if (found) {
            return found;
        }
    }

    return nullptr;
}

Controls::Grid FindRootGridAncestor(FrameworkElement element) {
    auto current = element;

    for (int depth = 0; depth < 16 && current; depth++) {
        auto parent = Media::VisualTreeHelper::GetParent(current)
                          .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (parent.Name() == L"RootGrid") {
            return parent.try_as<Controls::Grid>();
        }

        current = parent;
    }

    return nullptr;
}

struct DividerGeometry {
    winrt::Windows::Foundation::Rect targetBounds{};
    winrt::Windows::Foundation::Rect nextBounds{};
    double center = 0;
    double left = 0;
    double top = 0;
};

bool TryGetIconBounds(
    Controls::Canvas const& overlayCanvas,
    FrameworkElement const& icon,
    winrt::Windows::Foundation::Rect* bounds) {
    if (!overlayCanvas || !icon || !bounds) {
        return false;
    }

    double width = icon.ActualWidth();
    double height = icon.ActualHeight();
    if (!std::isfinite(width) || width <= 0 || !std::isfinite(height) ||
        height <= 0) {
        return false;
    }

    auto transformedBounds =
        icon.TransformToVisual(overlayCanvas)
            .TransformBounds(winrt::Windows::Foundation::Rect{
                0, 0, static_cast<float>(width),
                static_cast<float>(height)});
    if (!std::isfinite(transformedBounds.X) ||
        !std::isfinite(transformedBounds.Y) ||
        !std::isfinite(transformedBounds.Width) ||
        !std::isfinite(transformedBounds.Height) ||
        transformedBounds.Width <= 0 || transformedBounds.Height <= 0) {
        return false;
    }

    *bounds = transformedBounds;
    return true;
}

double PrimaryStart(winrt::Windows::Foundation::Rect const& bounds,
                    TaskbarOrientation orientation) {
    return orientation == TaskbarOrientation::horizontal ? bounds.X
                                                         : bounds.Y;
}

double PrimarySize(winrt::Windows::Foundation::Rect const& bounds,
                   TaskbarOrientation orientation) {
    return orientation == TaskbarOrientation::horizontal ? bounds.Width
                                                         : bounds.Height;
}

double PrimaryCenter(winrt::Windows::Foundation::Rect const& bounds,
                     TaskbarOrientation orientation) {
    return PrimaryStart(bounds, orientation) +
           PrimarySize(bounds, orientation) / 2.0;
}

bool TryGetDividerGeometry(
    Controls::Canvas const& overlayCanvas,
    FrameworkElement const& previousIcon,
    FrameworkElement const& targetIcon,
    FrameworkElement const& nextIcon,
    TaskbarOrientation orientation,
    double rectangleWidth,
    double rectangleHeight,
    DividerGeometry* geometry) {
    if (!overlayCanvas || !targetIcon || !geometry ||
        !std::isfinite(rectangleWidth) || rectangleWidth <= 0 ||
        !std::isfinite(rectangleHeight) || rectangleHeight <= 0) {
        return false;
    }

    winrt::Windows::Foundation::Rect targetBounds{};
    winrt::Windows::Foundation::Rect nextBounds{};
    if (!TryGetIconBounds(overlayCanvas, targetIcon, &targetBounds)) {
        return false;
    }

    if (nextIcon) {
        if (!TryGetIconBounds(overlayCanvas, nextIcon, &nextBounds)) {
            return false;
        }
    } else {
        winrt::Windows::Foundation::Rect previousBounds{};
        if (!previousIcon ||
            !TryGetIconBounds(overlayCanvas, previousIcon,
                              &previousBounds)) {
            return false;
        }

        double previousCenter =
            PrimaryCenter(previousBounds, orientation);
        double targetCenter = PrimaryCenter(targetBounds, orientation);
        double spacing = targetCenter - previousCenter;
        double previousCrossCenter =
            orientation == TaskbarOrientation::horizontal
                ? previousBounds.Y + previousBounds.Height / 2.0
                : previousBounds.X + previousBounds.Width / 2.0;
        double targetCrossCenter =
            orientation == TaskbarOrientation::horizontal
                ? targetBounds.Y + targetBounds.Height / 2.0
                : targetBounds.X + targetBounds.Width / 2.0;
        double crossSpacing = targetCrossCenter - previousCrossCenter;
        if (!std::isfinite(spacing) || !std::isfinite(crossSpacing) ||
            std::fabs(spacing) <= 0.1 ||
            std::fabs(spacing) <= std::fabs(crossSpacing)) {
            return false;
        }

        nextBounds = targetBounds;
        if (orientation == TaskbarOrientation::horizontal) {
            nextBounds.X = static_cast<float>(targetBounds.X + spacing);
        } else {
            nextBounds.Y = static_cast<float>(targetBounds.Y + spacing);
        }
    }

    double targetStart = PrimaryStart(targetBounds, orientation);
    double targetSize = PrimarySize(targetBounds, orientation);
    double targetCenter = targetStart + targetSize / 2.0;
    double nextStart = PrimaryStart(nextBounds, orientation);
    double nextSize = PrimarySize(nextBounds, orientation);
    double nextCenter = nextStart + nextSize / 2.0;
    double center;
    if (nextCenter >= targetCenter) {
        double targetEnd = targetStart + targetSize;
        center = nextStart >= targetEnd
                     ? (targetEnd + nextStart) / 2.0
                     : (targetCenter + nextCenter) / 2.0;
    } else {
        double nextEnd = nextStart + nextSize;
        center = nextEnd <= targetStart
                     ? (targetStart + nextEnd) / 2.0
                     : (targetCenter + nextCenter) / 2.0;
    }

    double overlayWidth = overlayCanvas.ActualWidth();
    double overlayHeight = overlayCanvas.ActualHeight();
    if (!std::isfinite(overlayWidth) || overlayWidth <= 0 ||
        !std::isfinite(overlayHeight) || overlayHeight <= 0) {
        return false;
    }

    double left;
    double top;
    if (orientation == TaskbarOrientation::horizontal) {
        left = center - rectangleWidth / 2.0;
        top = (overlayHeight - rectangleHeight) / 2.0;
    } else {
        left = (overlayWidth - rectangleWidth) / 2.0;
        top = center - rectangleHeight / 2.0;
    }
    if (!std::isfinite(center) || !std::isfinite(left) ||
        !std::isfinite(top)) {
        return false;
    }

    geometry->targetBounds = targetBounds;
    geometry->nextBounds = nextBounds;
    geometry->center = center;
    geometry->left = left;
    geometry->top = top;
    return true;
}

bool TryGetBeforeFirstDividerGeometry(
    Controls::Canvas const& overlayCanvas,
    FrameworkElement const& firstIcon,
    FrameworkElement const& secondIcon,
    TaskbarOrientation orientation,
    double rectangleWidth,
    double rectangleHeight,
    DividerGeometry* geometry) {
    if (!overlayCanvas || !firstIcon || !geometry ||
        !std::isfinite(rectangleWidth) || rectangleWidth <= 0 ||
        !std::isfinite(rectangleHeight) || rectangleHeight <= 0) {
        return false;
    }

    winrt::Windows::Foundation::Rect firstBounds{};
    winrt::Windows::Foundation::Rect secondBounds{};
    if (!TryGetIconBounds(overlayCanvas, firstIcon, &firstBounds)) {
        return false;
    }

    double firstCenter = PrimaryCenter(firstBounds, orientation);
    double firstCrossCenter =
        orientation == TaskbarOrientation::horizontal
            ? firstBounds.Y + firstBounds.Height / 2.0
            : firstBounds.X + firstBounds.Width / 2.0;
    double pitch = PrimarySize(firstBounds, orientation);
    double crossPitch = 0;
    if (secondIcon) {
        if (!TryGetIconBounds(overlayCanvas, secondIcon, &secondBounds)) {
            return false;
        }

        double secondCenter = PrimaryCenter(secondBounds, orientation);
        pitch = secondCenter - firstCenter;
        double secondCrossCenter =
            orientation == TaskbarOrientation::horizontal
                ? secondBounds.Y + secondBounds.Height / 2.0
                : secondBounds.X + secondBounds.Width / 2.0;
        crossPitch = secondCrossCenter - firstCrossCenter;
        if (!std::isfinite(pitch) || !std::isfinite(crossPitch) ||
            std::fabs(pitch) <= 0.1 ||
            std::fabs(pitch) <= std::fabs(crossPitch)) {
            return false;
        }
    }

    double virtualPreviousCenter = firstCenter - pitch;
    double center = (virtualPreviousCenter + firstCenter) / 2.0;
    double virtualPreviousCrossCenter = firstCrossCenter - crossPitch;
    double crossCenter =
        (virtualPreviousCrossCenter + firstCrossCenter) / 2.0;
    double left;
    double top;
    if (orientation == TaskbarOrientation::horizontal) {
        left = center - rectangleWidth / 2.0;
        top = crossCenter - rectangleHeight / 2.0;
    } else {
        left = crossCenter - rectangleWidth / 2.0;
        top = center - rectangleHeight / 2.0;
    }
    if (!std::isfinite(center) || !std::isfinite(left) ||
        !std::isfinite(top)) {
        return false;
    }

    geometry->targetBounds = firstBounds;
    geometry->nextBounds = secondIcon ? secondBounds : firstBounds;
    geometry->center = center;
    geometry->left = left;
    geometry->top = top;
    return true;
}

bool TryGetAnimatedDividerGeometry(
    Controls::Canvas const& overlayCanvas,
    FrameworkElement const& previousIcon,
    FrameworkElement const& targetIcon,
    FrameworkElement const& nextIcon,
    TaskbarOrientation orientation,
    double rectangleWidth,
    double rectangleHeight,
    DividerGeometry* geometry) {
    if (!TryGetDividerGeometry(
            overlayCanvas, previousIcon, targetIcon, nextIcon, orientation,
            rectangleWidth, rectangleHeight, geometry)) {
        return false;
    }

    double targetCenterX =
        geometry->targetBounds.X + geometry->targetBounds.Width / 2.0;
    double targetCenterY =
        geometry->targetBounds.Y + geometry->targetBounds.Height / 2.0;
    double nextCenterX =
        geometry->nextBounds.X + geometry->nextBounds.Width / 2.0;
    double nextCenterY =
        geometry->nextBounds.Y + geometry->nextBounds.Height / 2.0;

    if (orientation == TaskbarOrientation::horizontal) {
        double separatorCenterY = (targetCenterY + nextCenterY) / 2.0;
        geometry->top = separatorCenterY - rectangleHeight / 2.0;
    } else {
        double separatorCenterX = (targetCenterX + nextCenterX) / 2.0;
        geometry->left = separatorCenterX - rectangleWidth / 2.0;
    }

    return std::isfinite(geometry->left) && std::isfinite(geometry->top);
}

void ClearAnimationElementCache() {
    g_animationOverlayCanvas = {};
    g_animationDividers.clear();
}

void UnsubscribeAnimationRendering(bool logStopped) {
    if (!g_animationRenderingSubscribed) {
        return;
    }

    Media::CompositionTarget::Rendering(g_animationRenderingToken);
    g_animationRenderingToken = {};
    g_animationRenderingSubscribed = false;
    for (auto& cache : g_animationDividers) {
        cache.hasLastPosition = false;
    }
    g_animationSubscriptionOrderRefreshed = false;
    g_animationStableFrames = 0;
    if (logStopped) {
        Wh_Log(L"ANIMATION TRACKING: stopped");
    }
}

void StopAnimationTracking() {
    UnsubscribeAnimationRendering(true);
    g_animationLastActivity = {};
}

void OnAnimationRendering(winrt::Windows::Foundation::IInspectable const&,
                          winrt::Windows::Foundation::IInspectable const&) {
    if (g_unloading) {
        StopAnimationTracking();
        return;
    }

    auto now = AnimationClock::now();
    if (g_animationLastActivity == AnimationClock::time_point{} ||
        now - g_animationLastActivity >= kAnimationTrackingTimeout) {
        StopAnimationTracking();
        return;
    }

    if (g_animationRenderingCallbackActive) {
        return;
    }

    g_animationRenderingCallbackActive = true;
    struct CallbackGuard {
        ~CallbackGuard() {
            g_animationRenderingCallbackActive = false;
        }
    } callbackGuard;

    try {
        auto overlayCanvas = g_animationOverlayCanvas.get();
        if (!overlayCanvas || g_animationDividers.empty()) {
            Wh_Log(L"ANIMATION TRACKING: cache invalid");
            ClearAnimationElementCache();
            StopAnimationTracking();
            return;
        }

        bool allStable = true;
        for (auto& cache : g_animationDividers) {
            auto host = cache.host.get();
            auto previousIcon = cache.previousIcon.get();
            auto targetIcon = cache.targetIcon.get();
            auto nextIcon = cache.nextIcon.get();
            if (!host || !targetIcon ||
                (!cache.beforeFirst && !nextIcon && !previousIcon)) {
                Wh_Log(L"ANIMATION TRACKING: cache invalid");
                ClearAnimationElementCache();
                StopAnimationTracking();
                return;
            }

            DividerGeometry geometry;
            bool geometryValid =
                cache.beforeFirst
                    ? TryGetBeforeFirstDividerGeometry(
                          overlayCanvas, targetIcon, nextIcon,
                          cache.orientation, host.Width(), host.Height(),
                          &geometry)
                    : TryGetAnimatedDividerGeometry(
                          overlayCanvas, previousIcon, targetIcon, nextIcon,
                          cache.orientation, host.Width(), host.Height(),
                          &geometry);
            if (!geometryValid) {
                Wh_Log(L"ANIMATION TRACKING: cache invalid");
                ClearAnimationElementCache();
                StopAnimationTracking();
                return;
            }

            constexpr double kWriteEpsilon = 0.01;
            double currentLeft = Controls::Canvas::GetLeft(host);
            double currentTop = Controls::Canvas::GetTop(host);
            if (!std::isfinite(currentLeft) ||
                std::fabs(currentLeft - geometry.left) > kWriteEpsilon) {
                Controls::Canvas::SetLeft(host, geometry.left);
            }
            if (!std::isfinite(currentTop) ||
                std::fabs(currentTop - geometry.top) > kWriteEpsilon) {
                Controls::Canvas::SetTop(host, geometry.top);
            }

            if (!cache.hasLastPosition ||
                std::fabs(cache.lastLeft - geometry.left) >
                    kWriteEpsilon ||
                std::fabs(cache.lastTop - geometry.top) >
                    kWriteEpsilon) {
                allStable = false;
            }
            cache.lastLeft = geometry.left;
            cache.lastTop = geometry.top;
            cache.hasLastPosition = true;
        }

        if (g_animationPointerInside) {
            g_animationStableFrames = 0;
        } else {
            g_animationStableFrames =
                allStable ? g_animationStableFrames + 1 : 0;
            if (g_animationStableFrames >= 12) {
                StopAnimationTracking();
            }
        }
    } catch (...) {
        Wh_Log(L"ANIMATION TRACKING: cache invalid");
        ClearAnimationElementCache();
        StopAnimationTracking();
    }
}

void StartAnimationTracking(bool refreshSubscription = false) {
    if (g_unloading || !g_animationOverlayCanvas.get() ||
        g_animationDividers.empty()) {
        return;
    }

    g_animationLastActivity = AnimationClock::now();

    bool wasSubscribed = g_animationRenderingSubscribed;
    if (wasSubscribed && !refreshSubscription) {
        return;
    }
    if (wasSubscribed) {
        // Re-register once after pointer tracking begins. If another mod
        // subscribed during the first pointer event, this normally places our
        // callback later without relying on that ordering for correctness.
        UnsubscribeAnimationRendering(false);
    }

    for (auto& cache : g_animationDividers) {
        cache.hasLastPosition = false;
    }
    g_animationStableFrames = 0;
    g_animationRenderingToken =
        Media::CompositionTarget::Rendering(OnAnimationRendering);
    g_animationRenderingSubscribed = true;
    if (!wasSubscribed) {
        Wh_Log(L"ANIMATION TRACKING: started");
    }
}

void OnAnimationPointerMoved(
    winrt::Windows::Foundation::IInspectable const&,
    Input::PointerRoutedEventArgs const&) {
    if (g_unloading) {
        return;
    }

    bool wasPointerInside = g_animationPointerInside;
    g_animationPointerInside = true;
    g_animationStableFrames = 0;
    if (!wasPointerInside) {
        bool refreshExistingSubscription =
            g_animationRenderingSubscribed;
        StartAnimationTracking(refreshExistingSubscription);
        g_animationSubscriptionOrderRefreshed =
            refreshExistingSubscription;
    } else if (g_animationRenderingSubscribed &&
               !g_animationSubscriptionOrderRefreshed) {
        StartAnimationTracking(true);
        g_animationSubscriptionOrderRefreshed = true;
    } else {
        StartAnimationTracking();
    }
}

void OnAnimationPointerExited(
    winrt::Windows::Foundation::IInspectable const&,
    Input::PointerRoutedEventArgs const&) {
    if (g_unloading) {
        return;
    }

    g_animationPointerInside = false;
    g_animationStableFrames = 0;
    g_animationLastActivity = AnimationClock::now();
}

void DetachAnimationPointerHandlers() {
    StopAnimationTracking();

    if (g_animationPointerHandlersAttached) {
        if (auto source = g_animationPointerSource.get()) {
            if (g_animationPointerMovedHandler) {
                try {
                    source.RemoveHandler(UIElement::PointerMovedEvent(),
                                         g_animationPointerMovedHandler);
                } catch (...) {
                    // The source can be disconnected while Explorer rebuilds
                    // the taskbar. Clearing our delegate is sufficient then.
                }
            }
            if (g_animationPointerExitedHandlerAttached) {
                try {
                    source.PointerExited(g_animationPointerExitedToken);
                } catch (...) {
                    // The source can be disconnected while Explorer rebuilds
                    // the taskbar. Clearing our token is sufficient then.
                }
            }
        }
    }

    g_animationPointerMovedHandler = nullptr;
    g_animationPointerExitedToken = {};
    g_animationPointerSource = {};
    g_animationPointerHandlersAttached = false;
    g_animationPointerExitedHandlerAttached = false;
    g_animationPointerInside = false;
    g_animationLastActivity = {};
}

void AttachAnimationPointerHandlers(FrameworkElement const& source) {
    if (!source || g_unloading) {
        return;
    }

    if (g_animationPointerHandlersAttached) {
        auto currentSource = g_animationPointerSource.get();
        if (currentSource && winrt::get_abi(currentSource) ==
                                 winrt::get_abi(source)) {
            return;
        }
        DetachAnimationPointerHandlers();
    }

    try {
        g_animationPointerSource = winrt::make_weak(source);
        g_animationPointerMovedHandler = winrt::box_value(
            Input::PointerEventHandler{OnAnimationPointerMoved});
        source.AddHandler(UIElement::PointerMovedEvent(),
                          g_animationPointerMovedHandler, true);
        g_animationPointerHandlersAttached = true;
        g_animationPointerExitedToken = source.PointerExited(
            Input::PointerEventHandler{OnAnimationPointerExited});
        g_animationPointerExitedHandlerAttached = true;
    } catch (...) {
        DetachAnimationPointerHandlers();
    }
}

void ClearAnimationTrackingForUnload() {
    CancelInitialReconcileRetry();
    StopAnimationTracking();
    DetachAnimationPointerHandlers();
    ClearAnimationElementCache();
}

std::wstring GetDividerName(size_t settingsIndex) {
    return L"WindhawkTaskbarSeparator_" +
           std::to_wstring(settingsIndex);
}

std::wstring GetBeforeFirstDividerName() {
    return L"WindhawkTaskbarSeparator_BeforeFirst";
}

bool IsOwnedDividerName(std::wstring const& name) {
    constexpr std::wstring_view prefix = L"WindhawkTaskbarSeparator_";
    return name == L"WindhawkTaskbarSeparator" ||
            (name.size() >= prefix.size() &&
             name.compare(0, prefix.size(), prefix) == 0);
}

size_t StyleChildCount(DividerStyle style) {
    if (style == DividerStyle::fade) {
        return 3;
    }
    return style == DividerStyle::glow ||
                   style == DividerStyle::doubleLine
               ? 2
               : 1;
}

PCWSTR ExpectedStyleChildName(DividerStyle style, uint32_t index) {
    if (style == DividerStyle::fade) {
        constexpr PCWSTR names[] = {
            L"DividerFadeOuter", L"DividerFadeMiddle", L"DividerFadeCore"};
        return names[index];
    }
    if (style == DividerStyle::glow) {
        return index == 0 ? L"DividerGlow" : L"DividerMain";
    }
    if (style == DividerStyle::doubleLine) {
        return index == 0 ? L"DividerLine1" : L"DividerLine2";
    }
    return L"DividerMain";
}

Shapes::Rectangle AppendStyleRectangle(Controls::Canvas const& host,
                                       PCWSTR name,
                                       int zIndex) {
    Shapes::Rectangle rectangle;
    rectangle.Name(name);
    rectangle.HorizontalAlignment(HorizontalAlignment::Left);
    rectangle.VerticalAlignment(VerticalAlignment::Top);
    rectangle.IsHitTestVisible(false);
    rectangle.UseLayoutRounding(true);
    Controls::Canvas::SetZIndex(rectangle, zIndex);
    host.Children().Append(rectangle);
    return rectangle;
}

void RebuildStyleChildren(Controls::Canvas const& host,
                          DividerStyle style) {
    host.Children().Clear();
    switch (style) {
        case DividerStyle::fade:
            AppendStyleRectangle(host, L"DividerFadeOuter", 0);
            AppendStyleRectangle(host, L"DividerFadeMiddle", 1);
            AppendStyleRectangle(host, L"DividerFadeCore", 2);
            break;
        case DividerStyle::glow:
            AppendStyleRectangle(host, L"DividerGlow", 0);
            AppendStyleRectangle(host, L"DividerMain", 1);
            break;
        case DividerStyle::doubleLine:
            AppendStyleRectangle(host, L"DividerLine1", 0);
            AppendStyleRectangle(host, L"DividerLine2", 1);
            break;
        default:
            AppendStyleRectangle(host, L"DividerMain", 0);
            break;
    }
    host.Tag(winrt::box_value(static_cast<int32_t>(style)));
}

bool StyleChildrenValid(Controls::Canvas const& host,
                        DividerStyle style) {
    auto children = host.Children();
    if (children.Size() != StyleChildCount(style)) {
        return false;
    }

    for (uint32_t index = 0; index < children.Size(); index++) {
        auto rectangle =
            children.GetAt(index).try_as<Shapes::Rectangle>();
        if (!rectangle ||
            rectangle.Name() != ExpectedStyleChildName(style, index)) {
            return false;
        }
    }

    return winrt::unbox_value_or<int32_t>(
               host.Tag(), -1) == static_cast<int32_t>(style);
}

void SetRectangleBounds(Shapes::Rectangle const& rectangle,
                        double left,
                        double top,
                        double width,
                        double height) {
    if (rectangle.IsHitTestVisible()) {
        rectangle.IsHitTestVisible(false);
    }
    if (!rectangle.UseLayoutRounding()) {
        rectangle.UseLayoutRounding(true);
    }
    if (rectangle.Width() != width) {
        rectangle.Width(width);
    }
    if (rectangle.Height() != height) {
        rectangle.Height(height);
    }
    if (Controls::Canvas::GetLeft(rectangle) != left) {
        Controls::Canvas::SetLeft(rectangle, left);
    }
    if (Controls::Canvas::GetTop(rectangle) != top) {
        Controls::Canvas::SetTop(rectangle, top);
    }
    if (rectangle.Opacity() != 1) {
        rectangle.Opacity(1);
    }
}

void SetRectangleRadius(Shapes::Rectangle const& rectangle, double radius) {
    if (rectangle.RadiusX() != radius) {
        rectangle.RadiusX(radius);
    }
    if (rectangle.RadiusY() != radius) {
        rectangle.RadiusY(radius);
    }
}

void SetSolidFill(Shapes::Rectangle const& rectangle,
                  winrt::Windows::UI::Color color) {
    auto brush = rectangle.Fill().try_as<Media::SolidColorBrush>();
    if (!brush) {
        brush = Media::SolidColorBrush();
        rectangle.Fill(brush);
    }
    if (!ColorsEqual(brush.Color(), color)) {
        brush.Color(color);
    }
}

enum class FadeLayer {
    outer,
    middle,
    core,
};

void SetFadeFill(Shapes::Rectangle const& rectangle,
                 winrt::Windows::UI::Color color,
                 int fadeAmount,
                 TaskbarOrientation orientation,
                 FadeLayer layer) {
    auto brush = rectangle.Fill().try_as<Media::LinearGradientBrush>();
    if (!brush) {
        brush = Media::LinearGradientBrush();
        rectangle.Fill(brush);
    }

    if (orientation == TaskbarOrientation::horizontal) {
        brush.StartPoint({0.5f, 0.0f});
        brush.EndPoint({0.5f, 1.0f});
    } else {
        brush.StartPoint({0.0f, 0.5f});
        brush.EndPoint({1.0f, 0.5f});
    }

    auto stops = brush.GradientStops();
    constexpr uint32_t kFadeStopCount = 7;
    if (stops.Size() != kFadeStopCount) {
        stops.Clear();
        for (uint32_t index = 0; index < kFadeStopCount; index++) {
            Media::GradientStop stop;
            stops.Append(stop);
        }
    }

    double lowOpacityBaseOffset = 0.10;
    double highOpacityBaseOffset = 0.25;
    if (layer == FadeLayer::middle) {
        lowOpacityBaseOffset = 0.18;
        highOpacityBaseOffset = 0.36;
    } else if (layer == FadeLayer::outer) {
        lowOpacityBaseOffset = 0.28;
        highOpacityBaseOffset = 0.44;
    }

    // fadeAmount 70 uses each layer's default profile. Lower values compress
    // the fades toward the ends; higher values move them toward the midpoint.
    double softnessScale = std::clamp(fadeAmount / 70.0, 0.0, 100.0 / 70.0);
    double lowOpacityOffset =
        std::clamp(lowOpacityBaseOffset * softnessScale, 0.0, 0.5);
    double highOpacityOffset =
        std::clamp(highOpacityBaseOffset * softnessScale,
                   lowOpacityOffset, 0.5);
    double mirroredHighOpacityOffset =
        std::clamp(1.0 - highOpacityOffset, 0.5, 1.0);
    double mirroredLowOpacityOffset =
        std::clamp(1.0 - lowOpacityOffset,
                   mirroredHighOpacityOffset, 1.0);
    double offsets[kFadeStopCount] = {
        0.0,
        lowOpacityOffset,
        highOpacityOffset,
        0.5,
        mirroredHighOpacityOffset,
        mirroredLowOpacityOffset,
        1.0,
    };
    double opacityFactors[kFadeStopCount] = {
        0.0, 0.45, 0.85, 1.0, 0.85, 0.45, 0.0};

    for (uint32_t index = 0; index < kFadeStopCount; index++) {
        auto stopColor = color;
        stopColor.A = static_cast<uint8_t>(std::lround(
            color.A * opacityFactors[index]));
        auto stop = stops.GetAt(index);
        if (!ColorsEqual(stop.Color(), stopColor)) {
            stop.Color(stopColor);
        }
        if (stop.Offset() != offsets[index]) {
            stop.Offset(offsets[index]);
        }
    }
}

void GetStyleHostSize(Settings const& settings,
                      TaskbarOrientation orientation,
                      double* width,
                      double* height) {
    double baseWidth = orientation == TaskbarOrientation::horizontal
                           ? settings.width
                           : settings.height;
    double baseHeight = orientation == TaskbarOrientation::horizontal
                            ? settings.height
                            : settings.width;
    *width = baseWidth;
    *height = baseHeight;

    if (settings.style == DividerStyle::fade) {
        double outerThickness =
            std::max<double>(settings.width + 4, 5);
        if (orientation == TaskbarOrientation::horizontal) {
            *width = outerThickness;
        } else {
            *height = outerThickness;
        }
    } else if (settings.style == DividerStyle::glow) {
        *width += settings.glowSize * 2.0;
        *height += settings.glowSize * 2.0;
    } else if (settings.style == DividerStyle::doubleLine) {
        double doubleThickness = settings.width * 2.0 + settings.doubleGap;
        if (orientation == TaskbarOrientation::horizontal) {
            *width = doubleThickness;
        } else {
            *height = doubleThickness;
        }
    }
}

void ConfigureStyleHost(Controls::Canvas const& host,
                        Settings const& settings,
                        TaskbarOrientation orientation,
                        double hostWidth,
                        double hostHeight) {
    if (!StyleChildrenValid(host, settings.style)) {
        RebuildStyleChildren(host, settings.style);
    }

    auto children = host.Children();
    double baseWidth = orientation == TaskbarOrientation::horizontal
                           ? settings.width
                           : settings.height;
    double baseHeight = orientation == TaskbarOrientation::horizontal
                            ? settings.height
                            : settings.width;

    if (settings.style == DividerStyle::fade) {
        auto outer = children.GetAt(0).as<Shapes::Rectangle>();
        auto middle = children.GetAt(1).as<Shapes::Rectangle>();
        auto core = children.GetAt(2).as<Shapes::Rectangle>();
        double outerThickness =
            std::max<double>(settings.width + 4, 5);
        double middleThickness =
            std::max<double>(settings.width + 2, 3);

        if (orientation == TaskbarOrientation::horizontal) {
            SetRectangleBounds(outer,
                               (hostWidth - outerThickness) / 2.0, 0,
                               outerThickness, baseHeight);
            SetRectangleBounds(middle,
                               (hostWidth - middleThickness) / 2.0, 0,
                               middleThickness, baseHeight);
            SetRectangleBounds(core,
                               (hostWidth - settings.width) / 2.0, 0,
                               settings.width, baseHeight);
        } else {
            SetRectangleBounds(outer, 0,
                               (hostHeight - outerThickness) / 2.0,
                               baseWidth, outerThickness);
            SetRectangleBounds(middle, 0,
                               (hostHeight - middleThickness) / 2.0,
                               baseWidth, middleThickness);
            SetRectangleBounds(core, 0,
                               (hostHeight - settings.width) / 2.0,
                               baseWidth, settings.width);
        }

        SetRectangleRadius(outer, 0);
        SetRectangleRadius(middle, 0);
        SetRectangleRadius(core, 0);
        SetFadeFill(outer, settings.color, settings.fadeAmount, orientation,
                    FadeLayer::outer);
        SetFadeFill(middle, settings.color, settings.fadeAmount, orientation,
                    FadeLayer::middle);
        SetFadeFill(core, settings.color, settings.fadeAmount, orientation,
                    FadeLayer::core);
        outer.Opacity(0.12);
        middle.Opacity(0.30);
        core.Opacity(1.0);
        return;
    }

    if (settings.style == DividerStyle::glow) {
        auto glow = children.GetAt(0).as<Shapes::Rectangle>();
        auto main = children.GetAt(1).as<Shapes::Rectangle>();
        SetRectangleBounds(glow, 0, 0, hostWidth, hostHeight);
        SetRectangleRadius(glow, 0);
        SetSolidFill(glow, settings.color);
        glow.Opacity(settings.glowOpacityPercent / 100.0);
        SetRectangleBounds(main, settings.glowSize, settings.glowSize,
                           baseWidth, baseHeight);
        SetRectangleRadius(main, 0);
        SetSolidFill(main, settings.color);
        return;
    }

    if (settings.style == DividerStyle::doubleLine) {
        auto first = children.GetAt(0).as<Shapes::Rectangle>();
        auto second = children.GetAt(1).as<Shapes::Rectangle>();
        double secondLeft = orientation == TaskbarOrientation::horizontal
                                 ? settings.width + settings.doubleGap
                                 : 0;
        double secondTop = orientation == TaskbarOrientation::vertical
                                ? settings.width + settings.doubleGap
                                : 0;
        SetRectangleBounds(first, 0, 0, baseWidth, baseHeight);
        SetRectangleBounds(second, secondLeft, secondTop, baseWidth,
                           baseHeight);
        SetRectangleRadius(first, 0);
        SetRectangleRadius(second, 0);
        SetSolidFill(first, settings.color);
        SetSolidFill(second, settings.color);
        first.Opacity(1.0);
        second.Opacity(1.0);
        return;
    }

    auto main = children.GetAt(0).as<Shapes::Rectangle>();
    SetRectangleBounds(main, 0, 0, baseWidth, baseHeight);
    double radius = settings.style == DividerStyle::rounded
                        ? std::min<double>(settings.cornerRadius,
                                           settings.width / 2.0)
                        : 0;
    SetRectangleRadius(main, radius);
    SetSolidFill(main, settings.color);
}

bool TryGetTaskbarOrientation(
    OrientationSetting orientationSetting,
    Controls::Canvas const& overlayCanvas,
    std::vector<FrameworkElement> const& icons,
    TaskbarOrientation* orientation) {
    if (!orientation) {
        return false;
    }

    if (orientationSetting == OrientationSetting::horizontal) {
        *orientation = TaskbarOrientation::horizontal;
        return true;
    }
    if (orientationSetting == OrientationSetting::vertical) {
        *orientation = TaskbarOrientation::vertical;
        return true;
    }

    size_t validIconCount = 0;
    for (auto const& icon : icons) {
        winrt::Windows::Foundation::Rect bounds{};
        if (icon && TryGetIconBounds(overlayCanvas, icon, &bounds)) {
            validIconCount++;
        }
    }

    double totalHorizontalMovement = 0;
    double totalVerticalMovement = 0;
    bool foundPair = false;
    for (size_t index = 1; index < icons.size(); index++) {
        winrt::Windows::Foundation::Rect previousBounds{};
        winrt::Windows::Foundation::Rect currentBounds{};
        if (!icons[index - 1] || !icons[index] ||
            !TryGetIconBounds(overlayCanvas, icons[index - 1],
                              &previousBounds) ||
            !TryGetIconBounds(overlayCanvas, icons[index],
                              &currentBounds)) {
            continue;
        }

        double previousCenterX =
            previousBounds.X + previousBounds.Width / 2.0;
        double previousCenterY =
            previousBounds.Y + previousBounds.Height / 2.0;
        double currentCenterX = currentBounds.X + currentBounds.Width / 2.0;
        double currentCenterY =
            currentBounds.Y + currentBounds.Height / 2.0;
        totalHorizontalMovement +=
            std::fabs(currentCenterX - previousCenterX);
        totalVerticalMovement +=
            std::fabs(currentCenterY - previousCenterY);
        foundPair = true;
    }

    if (!foundPair && validIconCount < 2) {
        double width = overlayCanvas.ActualWidth();
        double height = overlayCanvas.ActualHeight();
        if (!std::isfinite(width) || !std::isfinite(height) || width < 0 ||
            height < 0) {
            return false;
        }

        *orientation = width >= height ? TaskbarOrientation::horizontal
                                       : TaskbarOrientation::vertical;
        return true;
    }

    if (!foundPair ||
        std::fabs(totalHorizontalMovement - totalVerticalMovement) <= 0.1) {
        return false;
    }

    *orientation = totalHorizontalMovement > totalVerticalMovement
                       ? TaskbarOrientation::horizontal
                       : TaskbarOrientation::vertical;
    return true;
}

#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
bool HasNativePlaceholderMarker(FrameworkElement const& element,
                                int depth = 0) {
    if (!element || depth > 10) {
        return false;
    }

    auto automationName =
        Automation::AutomationProperties::GetName(element);
    if (automationName == kNativePlaceholderAutomationName) {
        return true;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int index = 0; index < childCount; index++) {
        auto child = Media::VisualTreeHelper::GetChild(element, index)
                         .try_as<FrameworkElement>();
        if (child && HasNativePlaceholderMarker(child, depth + 1)) {
            return true;
        }
    }

    return false;
}

bool ShouldHideNativePlaceholderElement(FrameworkElement const& element) {
    if (!element) {
        return false;
    }

    std::wstring name{element.Name()};
    std::transform(name.begin(), name.end(), name.begin(),
                   [](wchar_t character) {
                       return static_cast<wchar_t>(std::towlower(character));
                   });

    if (name.empty() || name == L"iconpanel" ||
        name == L"windhawknativetaskbarseparator") {
        return false;
    }

    return name == L"icon" || name.find(L"overlayicon") != std::wstring::npos ||
           name.find(L"background") != std::wstring::npos ||
           name.find(L"runningindicator") != std::wstring::npos ||
           name.find(L"badge") != std::wstring::npos ||
           name.find(L"progress") != std::wstring::npos ||
           name.find(L"hover") != std::wstring::npos;
}

bool IsVisualAncestorOrSelf(FrameworkElement const& candidate,
                            FrameworkElement const& descendant) {
    if (!candidate || !descendant) {
        return false;
    }

    DependencyObject current = descendant;
    while (current) {
        if (winrt::get_abi(current) == winrt::get_abi(candidate)) {
            return true;
        }
        current = Media::VisualTreeHelper::GetParent(current);
    }

    return false;
}

void HideNativePlaceholderElements(FrameworkElement const& root,
                                   FrameworkElement const& protectedHost,
                                   int depth = 0) {
    if (!root || depth > 12) {
        return;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int index = 0; index < childCount; index++) {
        auto child = Media::VisualTreeHelper::GetChild(root, index)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (ShouldHideNativePlaceholderElement(child) &&
            !IsVisualAncestorOrSelf(child, protectedHost)) {
            auto existing = std::find_if(
                g_nativePlaceholderState.hiddenElements.begin(),
                g_nativePlaceholderState.hiddenElements.end(),
                [&](NativeHiddenElementState const& state) {
                    auto stateElement = state.element.get();
                    return stateElement &&
                           winrt::get_abi(stateElement) ==
                               winrt::get_abi(child);
                });
            if (existing ==
                g_nativePlaceholderState.hiddenElements.end()) {
                NativeHiddenElementState state;
                state.element = winrt::make_weak(child);
                state.opacity = child.Opacity();
                state.isHitTestVisible = child.IsHitTestVisible();
                g_nativePlaceholderState.hiddenElements.push_back(state);
            }

            if (child.Opacity() != 0) {
                child.Opacity(0);
            }
            if (child.IsHitTestVisible()) {
                child.IsHitTestVisible(false);
            }
        }

        HideNativePlaceholderElements(child, protectedHost, depth + 1);
    }
}

void RestoreNativeProtectedHostState(FrameworkElement const& protectedHost) {
    auto& hiddenElements = g_nativePlaceholderState.hiddenElements;
    hiddenElements.erase(
        std::remove_if(
            hiddenElements.begin(), hiddenElements.end(),
            [&](NativeHiddenElementState const& state) {
                auto element = state.element.get();
                if (!element ||
                    !IsVisualAncestorOrSelf(element, protectedHost)) {
                    return !element;
                }

                element.Opacity(state.opacity);
                element.IsHitTestVisible(state.isHitTestVisible);
                return true;
            }),
        hiddenElements.end());
}

bool IsHiddenByNativePrototype(FrameworkElement const& element) {
    return std::any_of(
        g_nativePlaceholderState.hiddenElements.begin(),
        g_nativePlaceholderState.hiddenElements.end(),
        [&](NativeHiddenElementState const& state) {
            auto hiddenElement = state.element.get();
            return hiddenElement &&
                   winrt::get_abi(hiddenElement) == winrt::get_abi(element);
        });
}

void ForceNativeSeparatorAncestorsVisible(
    Shapes::Rectangle const& separator,
    FrameworkElement const& iconPanel,
    FrameworkElement const& taskListButton) {
    DependencyObject current = Media::VisualTreeHelper::GetParent(separator);
    while (current) {
        auto element = current.try_as<FrameworkElement>();
        if (!element || winrt::get_abi(element) ==
                            winrt::get_abi(taskListButton)) {
            break;
        }

        auto existing = std::find_if(
            g_nativePlaceholderState.forcedAncestors.begin(),
            g_nativePlaceholderState.forcedAncestors.end(),
            [&](NativeForcedAncestorState const& state) {
                auto stateElement = state.element.get();
                return stateElement &&
                       winrt::get_abi(stateElement) ==
                           winrt::get_abi(element);
            });
        if (existing ==
            g_nativePlaceholderState.forcedAncestors.end()) {
            g_nativePlaceholderState.forcedAncestors.push_back(
                {winrt::make_weak(element), element.Visibility(),
                 element.Opacity()});
        }

        element.Visibility(Visibility::Visible);
        element.Opacity(1);
        if (winrt::get_abi(element) == winrt::get_abi(iconPanel)) {
            break;
        }
        current = Media::VisualTreeHelper::GetParent(current);
    }
}

bool IsGenuineTaskListButton(FrameworkElement const& element) {
    if (!element) {
        return false;
    }
    std::wstring runtimeClass{winrt::get_class_name(element)};
    return runtimeClass.find(L"Taskbar.TaskListButton") !=
           std::wstring::npos;
}

void DumpNativeTaskListButtonTree(
    DependencyObject const& object,
    DependencyObject const& parent,
    int depth = 0) {
    if (!object || depth > 32) {
        return;
    }

    auto element = object.try_as<FrameworkElement>();
    auto panel = object.try_as<Controls::Panel>();
    auto grid = object.try_as<Controls::Grid>();
    auto runtimeClass = winrt::get_class_name(object);
    winrt::hstring name = element ? element.Name() : L"(not FrameworkElement)";
    bool hasChildrenCollection = static_cast<bool>(panel);

    if (parent) {
        auto parentElement = parent.try_as<FrameworkElement>();
        auto parentClass = winrt::get_class_name(parent);
        winrt::hstring parentName =
            parentElement ? parentElement.Name()
                          : L"(not FrameworkElement)";
        Wh_Log(
            L"NATIVE TREE EDGE: depth=%d parent=%s#%s child=%s#%s",
            depth, parentClass.c_str(), parentName.c_str(),
            runtimeClass.c_str(), name.c_str());
    }

    Wh_Log(
        L"NATIVE TREE NODE: depth=%d class=%s name=%s panel=%d grid=%d "
        L"childrenCollection=%d actual=%.2fx%.2f visibility=%d "
        L"opacity=%.3f clip=%d hitTest=%d",
        depth, runtimeClass.c_str(), name.c_str(), static_cast<bool>(panel),
        static_cast<bool>(grid), hasChildrenCollection,
        element ? element.ActualWidth() : -1.0,
        element ? element.ActualHeight() : -1.0,
        element ? static_cast<int>(element.Visibility()) : -1,
        element ? element.Opacity() : -1.0,
        element ? static_cast<bool>(element.Clip()) : false,
        element ? element.IsHitTestVisible() : false);

    int childCount = Media::VisualTreeHelper::GetChildrenCount(object);
    for (int index = 0; index < childCount; index++) {
        auto child = Media::VisualTreeHelper::GetChild(object, index);
        DumpNativeTaskListButtonTree(child, object, depth + 1);
    }
}

bool AncestorChainContains(DependencyObject const& descendant,
                           FrameworkElement const& expectedAncestor) {
    DependencyObject current = descendant;
    while (current) {
        if (winrt::get_abi(current) == winrt::get_abi(expectedAncestor)) {
            return true;
        }
        current = Media::VisualTreeHelper::GetParent(current);
    }
    return false;
}

void RemoveInvalidNativeHosts(FrameworkElement const& root,
                              FrameworkElement const& taskListButton,
                              int depth = 0) {
    if (!root || depth > 14) {
        return;
    }

    if (auto panel = root.try_as<Controls::Panel>()) {
        auto children = panel.Children();
        for (uint32_t index = 0; index < children.Size();) {
            auto child = children.GetAt(index).try_as<FrameworkElement>();
            if (!child) {
                index++;
                continue;
            }

            bool isLegacyHost =
                child.Name() == L"WindhawkNativeTaskbarSeparatorHost";
            bool isOutsideHost =
                child.Name() == L"WindhawkNativeSeparatorHost" &&
                !AncestorChainContains(child, taskListButton);
            if (isLegacyHost || isOutsideHost) {
                children.RemoveAt(index);
                if (isOutsideHost) {
                    Wh_Log(L"NATIVE TEST: invalid host outside TaskListButton");
                }
                continue;
            }

            RemoveInvalidNativeHosts(child, taskListButton, depth + 1);
            index++;
        }
        return;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int index = 0; index < childCount; index++) {
        auto child = Media::VisualTreeHelper::GetChild(root, index)
                         .try_as<FrameworkElement>();
        if (child) {
            RemoveInvalidNativeHosts(child, taskListButton, depth + 1);
        }
    }
}

bool IsUsableNativeHostPanel(Controls::Panel const& panel) {
    if (!panel || panel.Visibility() != Visibility::Visible ||
        panel.Opacity() <= 0 || !std::isfinite(panel.ActualWidth()) ||
        !std::isfinite(panel.ActualHeight()) || panel.ActualWidth() <= 0 ||
        panel.ActualHeight() <= 0 || IsHiddenByNativePrototype(panel)) {
        return false;
    }

    if (auto clip = panel.Clip()) {
        auto clipRect = clip.Rect();
        if (clipRect.Width <= 0 || clipRect.Height <= 0) {
            return false;
        }
    }
    return true;
}

struct NativeHostCandidate {
    Controls::Panel panel{nullptr};
    int depth = 0;
    bool aboveIconPanel = false;
    bool isGrid = false;
};

void CollectNativeHostCandidates(
    FrameworkElement const& root,
    FrameworkElement const& iconPanel,
    int depth,
    std::vector<NativeHostCandidate>* candidates) {
    if (!root || !candidates || depth > 12) {
        return;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int index = 0; index < childCount; index++) {
        auto child = Media::VisualTreeHelper::GetChild(root, index)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (auto panel = child.try_as<Controls::Panel>();
            panel && child.Name() != L"IconPanel" &&
            child.Name() != L"WindhawkNativeSeparatorHost" &&
            IsUsableNativeHostPanel(panel)) {
            candidates->push_back(
                {panel, depth,
                 iconPanel && IsVisualAncestorOrSelf(child, iconPanel),
                 static_cast<bool>(child.try_as<Controls::Grid>())});
        }
        CollectNativeHostCandidates(child, iconPanel, depth + 1,
                                    candidates);
    }
}

Controls::Panel FindNativeHostPanel(
    FrameworkElement const& taskListButton,
    FrameworkElement const& iconPanel) {
    std::vector<NativeHostCandidate> candidates;
    CollectNativeHostCandidates(taskListButton, iconPanel, 1,
                                &candidates);
    std::stable_sort(
        candidates.begin(), candidates.end(),
        [](NativeHostCandidate const& left,
           NativeHostCandidate const& right) {
            if (left.aboveIconPanel != right.aboveIconPanel) {
                return left.aboveIconPanel > right.aboveIconPanel;
            }
            if (left.depth != right.depth) {
                return left.depth < right.depth;
            }
            return left.isGrid > right.isGrid;
        });
    return candidates.empty() ? nullptr : candidates.front().panel;
}

Controls::Grid CreateNativeInsideButtonHost(
    Controls::Panel const& parent,
    FrameworkElement const& taskListButton) {
    if (!parent || !AncestorChainContains(parent, taskListButton)) {
        return nullptr;
    }

    Controls::Grid host;
    host.Name(L"WindhawkNativeSeparatorHost");
    host.HorizontalAlignment(HorizontalAlignment::Stretch);
    host.VerticalAlignment(VerticalAlignment::Stretch);
    host.IsHitTestVisible(false);
    Controls::Canvas::SetZIndex(host, 9999);
    parent.Children().Append(host);

    if (auto parentGrid = parent.try_as<Controls::Grid>()) {
        Controls::Grid::SetRowSpan(
            host,
            std::max(1, static_cast<int>(parentGrid.RowDefinitions().Size())));
        Controls::Grid::SetColumnSpan(
            host, std::max(
                      1, static_cast<int>(
                             parentGrid.ColumnDefinitions().Size())));
    }

    if (!AncestorChainContains(host, taskListButton)) {
        uint32_t hostIndex = 0;
        auto children = parent.Children();
        if (children.IndexOf(host, hostIndex)) {
            children.RemoveAt(hostIndex);
        }
        return nullptr;
    }

    g_nativePlaceholderState.nativeHostParent = winrt::make_weak(parent);
    g_nativePlaceholderState.nativeHost = winrt::make_weak(host);
    return host;
}

void LogNativeSeparatorDiagnostics(
    Shapes::Rectangle const& separator,
    Controls::Panel const& iconPanel,
    FrameworkElement const& taskListButton) {
    auto fill = separator.Fill();
    auto fillClass = fill ? winrt::get_class_name(fill) : L"(null)";
    auto solidFill = fill.try_as<Media::SolidColorBrush>();
    winrt::Windows::UI::Color fillColor{};
    if (solidFill) {
        fillColor = solidFill.Color();
    }
    Wh_Log(
        L"NATIVE RECT: visibility=%d opacity=%.3f actual=%.2fx%.2f "
        L"size=%.2fx%.2f fill=%s ARGB=%02X%02X%02X%02X z=%d "
        L"hAlign=%d vAlign=%d hitTest=%d",
        static_cast<int>(separator.Visibility()), separator.Opacity(),
        separator.ActualWidth(), separator.ActualHeight(), separator.Width(),
        separator.Height(), fillClass.c_str(), fillColor.A, fillColor.R,
        fillColor.G, fillColor.B, Controls::Canvas::GetZIndex(separator),
        static_cast<int>(separator.HorizontalAlignment()),
        static_cast<int>(separator.VerticalAlignment()),
        separator.IsHitTestVisible());

    DependencyObject current = separator;
    while (current) {
        auto element = current.try_as<FrameworkElement>();
        if (!element) {
            break;
        }
        auto runtimeClass = winrt::get_class_name(element);
        auto clip = element.Clip();
        auto transform = element.RenderTransform();
        auto transformClass =
            transform ? winrt::get_class_name(transform) : L"(null)";
        Wh_Log(
            L"NATIVE CHAIN: class=%s name=%s visibility=%d "
            L"opacity=%.3f actual=%.2fx%.2f clip=%d transform=%s hidden=%d",
            runtimeClass.c_str(), element.Name().c_str(),
            static_cast<int>(element.Visibility()), element.Opacity(),
            element.ActualWidth(), element.ActualHeight(), clip != nullptr,
            transformClass.c_str(), IsHiddenByNativePrototype(element));
        if (winrt::get_abi(element) == winrt::get_abi(taskListButton)) {
            break;
        }
        current = Media::VisualTreeHelper::GetParent(current);
    }

    auto iconChildren = iconPanel.Children();
    for (uint32_t index = 0; index < iconChildren.Size(); index++) {
        auto child = iconChildren.GetAt(index).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        auto runtimeClass = winrt::get_class_name(child);
        Wh_Log(
            L"NATIVE ICONPANEL CHILD: index=%u name=%s class=%s "
            L"opacity=%.3f visibility=%d z=%d",
            index, child.Name().c_str(), runtimeClass.c_str(),
            child.Opacity(), static_cast<int>(child.Visibility()),
            Controls::Canvas::GetZIndex(child));
    }

    uint32_t childIndex = 0;
    bool isIconPanelChild = iconChildren.IndexOf(separator, childIndex);
    Wh_Log(
        L"NATIVE TEST: divider child of visible IconPanel=%d "
        L"IconPanel visibility=%d opacity=%.3f",
        isIconPanelChild, static_cast<int>(iconPanel.Visibility()),
        iconPanel.Opacity());

    try {
        auto bounds = separator.TransformToVisual(taskListButton)
                          .TransformBounds(
                              {0, 0,
                               static_cast<float>(separator.ActualWidth()),
                               static_cast<float>(separator.ActualHeight())});
        Wh_Log(
            L"NATIVE TEST: visible bounds in TaskListButton coordinates = "
            L"(%.2f, %.2f, %.2f, %.2f)",
            bounds.X, bounds.Y, bounds.Width, bounds.Height);
    } catch (...) {
        Wh_Log(L"NATIVE TEST: visible bounds in TaskListButton coordinates = invalid");
    }
}

void RestoreNativePlaceholder() {
    bool hadState = !g_nativePlaceholderState.hiddenElements.empty() ||
                    g_nativePlaceholderState.button.get() ||
                    g_nativePlaceholderState.separator.get();

    if (auto host = g_nativePlaceholderState.separatorHost.get()) {
        if (auto separator = g_nativePlaceholderState.separator.get()) {
            uint32_t index = 0;
            auto children = host.Children();
            if (children.IndexOf(separator, index)) {
                children.RemoveAt(index);
            }
        }
    }

    if (auto parent = g_nativePlaceholderState.nativeHostParent.get()) {
        if (auto host = g_nativePlaceholderState.nativeHost.get()) {
            uint32_t index = 0;
            auto children = parent.Children();
            if (children.IndexOf(host, index)) {
                children.RemoveAt(index);
            }
        }
    }

    for (auto const& forcedState :
         g_nativePlaceholderState.forcedAncestors) {
        if (auto element = forcedState.element.get()) {
            element.Visibility(forcedState.visibility);
            element.Opacity(forcedState.opacity);
        }
    }

    for (auto const& hiddenState :
         g_nativePlaceholderState.hiddenElements) {
        if (auto element = hiddenState.element.get()) {
            element.Opacity(hiddenState.opacity);
            element.IsHitTestVisible(hiddenState.isHitTestVisible);
        }
    }

    if (auto button = g_nativePlaceholderState.button.get()) {
        button.Width(g_nativePlaceholderState.width);
        button.MinWidth(g_nativePlaceholderState.minWidth);
        button.MaxWidth(g_nativePlaceholderState.maxWidth);
        button.Margin(g_nativePlaceholderState.margin);
        if (g_nativePlaceholderState.hasControlPadding) {
            if (auto control = button.try_as<Controls::Control>()) {
                control.Padding(g_nativePlaceholderState.controlPadding);
            }
        }
        Controls::ToolTipService::SetToolTip(
            button, g_nativePlaceholderState.toolTip);
    }

    g_nativePlaceholderState = NativePlaceholderState{};
    if (hadState) {
        Wh_Log(L"NATIVE TEST: restored");
    }
}

[[maybe_unused]] ReconcileResult ReconcileNativePlaceholderExperimental(
    bool enabled) {
    if (!enabled) {
        RestoreNativePlaceholder();
        g_nativePlaceholderLogState = NativePlaceholderLogState::unknown;
        g_nativeLoggedSelectedButtonIndex = 0;
        g_nativeLoggedRequestedButtonIndex = -1;
        g_nativeTreeDumpedButton = {};
        g_nativeTreeDumpedRequestedButtonIndex = -1;
        return ReconcileResult::noValidSeparators;
    }

    FrameworkElement repeater =
        g_taskbarFrameRepeaterForCurrentThread.get();
    if (!repeater || repeater.Name() != L"TaskbarFrameRepeater") {
        HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
        XamlRoot xamlRoot =
            taskbarWnd ? GetTaskbarXamlRoot(taskbarWnd) : nullptr;
        auto root =
            xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>()
                     : nullptr;
        repeater = FindDescendantByName(root, L"TaskbarFrameRepeater");
        if (repeater) {
            g_taskbarFrameRepeaterForCurrentThread =
                winrt::make_weak(repeater);
        }
    }

    if (!repeater || !repeater.Dispatcher().HasThreadAccess()) {
        return ReconcileResult::temporarilyNotReady;
    }

    struct RealizedElement {
        int index;
        FrameworkElement element;
    };
    std::vector<RealizedElement> realizedElements;
    if (auto repeaterPanel = repeater.try_as<Controls::Panel>()) {
        for (auto const& panelChild : repeaterPanel.Children()) {
            auto child = panelChild.try_as<FrameworkElement>();
            if (!child) {
                continue;
            }
            int index = ItemsRepeater_GetElementIndex(repeater, child);
            if (index < 0) {
                continue;
            }
            if (auto realized =
                    ItemsRepeater_TryGetElement(repeater, index)) {
                realizedElements.push_back({index, realized});
            }
        }
    }
    std::sort(realizedElements.begin(), realizedElements.end(),
              [](RealizedElement const& left,
                 RealizedElement const& right) {
                  return left.index < right.index;
              });

    std::vector<FrameworkElement> taskListButtons;
    for (auto const& realized : realizedElements) {
        if (realized.element.Name() != L"TaskListButton") {
            continue;
        }

        taskListButtons.push_back(realized.element);
    }

    Settings settings = GetSettingsSnapshot();
    FrameworkElement placeholderButton = nullptr;
    int selectedButtonIndex = 0;
    if (settings.nativeTestButtonIndex >= 1) {
        size_t requestedIndex =
            static_cast<size_t>(settings.nativeTestButtonIndex - 1);
        if (requestedIndex < taskListButtons.size()) {
            placeholderButton = taskListButtons[requestedIndex];
            selectedButtonIndex = settings.nativeTestButtonIndex;
        }
    } else {
        for (size_t index = 0; index < taskListButtons.size(); index++) {
            if (HasNativePlaceholderMarker(taskListButtons[index])) {
                placeholderButton = taskListButtons[index];
                selectedButtonIndex = static_cast<int>(index + 1);
                break;
            }
        }
    }

    if (!placeholderButton) {
        RestoreNativePlaceholder();
        g_nativeLoggedSelectedButtonIndex = 0;
        g_nativeLoggedRequestedButtonIndex =
            settings.nativeTestButtonIndex;
        if (g_nativePlaceholderLogState !=
            NativePlaceholderLogState::notFound) {
            Wh_Log(L"NATIVE TEST: placeholder not found");
            g_nativePlaceholderLogState =
                NativePlaceholderLogState::notFound;
        }
        return ReconcileResult::placeholderNotFound;
    }

    if (!IsGenuineTaskListButton(placeholderButton)) {
        DependencyObject current = placeholderButton;
        FrameworkElement genuineTaskListButton = nullptr;
        while (current) {
            auto element = current.try_as<FrameworkElement>();
            if (element && IsGenuineTaskListButton(element)) {
                genuineTaskListButton = element;
                break;
            }
            current = Media::VisualTreeHelper::GetParent(current);
        }
        if (!genuineTaskListButton) {
            RestoreNativePlaceholder();
            Wh_Log(L"NATIVE TEST: invalid host outside TaskListButton");
            return ReconcileResult::placeholderNotFound;
        }
        placeholderButton = genuineTaskListButton;
    }

    if (g_nativePlaceholderLogState != NativePlaceholderLogState::found) {
        Wh_Log(L"NATIVE TEST: placeholder found");
        g_nativePlaceholderLogState = NativePlaceholderLogState::found;
    }
    if (g_nativeLoggedSelectedButtonIndex != selectedButtonIndex ||
        g_nativeLoggedRequestedButtonIndex !=
            settings.nativeTestButtonIndex) {
        if (settings.nativeTestButtonIndex >= 1) {
            Wh_Log(L"NATIVE TEST: selected app button %d by index",
                   selectedButtonIndex);
        } else {
            Wh_Log(L"NATIVE TEST: selected app button %d by marker",
                   selectedButtonIndex);
        }
        g_nativeLoggedSelectedButtonIndex = selectedButtonIndex;
        g_nativeLoggedRequestedButtonIndex =
            settings.nativeTestButtonIndex;
    }

    auto dumpedButton = g_nativeTreeDumpedButton.get();
    bool alreadyDumped =
        dumpedButton &&
        winrt::get_abi(dumpedButton) == winrt::get_abi(placeholderButton) &&
        g_nativeTreeDumpedRequestedButtonIndex ==
            settings.nativeTestButtonIndex;
    if (!alreadyDumped) {
        auto runtimeClass = winrt::get_class_name(placeholderButton);
        Wh_Log(
            L"NATIVE TREE: selected index=%d class=%s name=%s",
            selectedButtonIndex, runtimeClass.c_str(),
            placeholderButton.Name().c_str());
        DumpNativeTaskListButtonTree(placeholderButton, nullptr);
        g_nativeTreeDumpedButton = winrt::make_weak(placeholderButton);
        g_nativeTreeDumpedRequestedButtonIndex =
            settings.nativeTestButtonIndex;
    }

    // Native diagnostic build: selection and visual-tree logging only.
    // Do not narrow, hide, host, or create any visual element.
    return ReconcileResult::noValidSeparators;

    auto currentButton = g_nativePlaceholderState.button.get();
    if (!currentButton ||
        winrt::get_abi(currentButton) != winrt::get_abi(placeholderButton)) {
        RestoreNativePlaceholder();

        g_nativePlaceholderState.button =
            winrt::make_weak(placeholderButton);
        g_nativePlaceholderState.width = placeholderButton.Width();
        g_nativePlaceholderState.minWidth = placeholderButton.MinWidth();
        g_nativePlaceholderState.maxWidth = placeholderButton.MaxWidth();
        g_nativePlaceholderState.margin = placeholderButton.Margin();
        if (auto control =
                placeholderButton.try_as<Controls::Control>()) {
            g_nativePlaceholderState.hasControlPadding = true;
            g_nativePlaceholderState.controlPadding = control.Padding();
        }
        g_nativePlaceholderState.toolTip =
            Controls::ToolTipService::GetToolTip(placeholderButton);
    }

    constexpr double kPrototypeButtonWidth = 12;
    placeholderButton.MinWidth(0);
    placeholderButton.MaxWidth(kPrototypeButtonWidth);
    placeholderButton.Width(kPrototypeButtonWidth);
    placeholderButton.Margin(Thickness{0, 0, 0, 0});
    if (auto control = placeholderButton.try_as<Controls::Control>()) {
        control.Padding(Thickness{0, 0, 0, 0});
    }
    Controls::ToolTipService::SetToolTip(placeholderButton, nullptr);

    auto iconPanel =
        FindDescendantByName(placeholderButton, L"IconPanel")
            .try_as<Controls::Panel>();
    if (!iconPanel) {
        return ReconcileResult::temporarilyNotReady;
    }

    g_nativePlaceholderState.hiddenElements.erase(
        std::remove_if(
            g_nativePlaceholderState.hiddenElements.begin(),
            g_nativePlaceholderState.hiddenElements.end(),
            [](NativeHiddenElementState const& state) {
                return !state.element.get();
            }),
        g_nativePlaceholderState.hiddenElements.end());
    RestoreNativeProtectedHostState(iconPanel);
    HideNativePlaceholderElements(placeholderButton, iconPanel);

    auto hostParent = FindNativeHostPanel(placeholderButton, iconPanel);
    if (!hostParent ||
        !AncestorChainContains(hostParent, placeholderButton)) {
        if (!g_nativePlaceholderState.invalidHostLogged) {
            Wh_Log(L"NATIVE TEST: no suitable visible host inside TaskListButton");
            g_nativePlaceholderState.invalidHostLogged = true;
        }
        return placeholderButton.ActualWidth() > 0 &&
                       placeholderButton.ActualHeight() > 0
                   ? ReconcileResult::noValidSeparators
                   : ReconcileResult::temporarilyNotReady;
    }

    Controls::Grid nativeHost = g_nativePlaceholderState.nativeHost.get();
    auto existingHostParent = nativeHost
                                  ? Media::VisualTreeHelper::GetParent(
                                        nativeHost)
                                        .try_as<Controls::Panel>()
                                  : nullptr;
    if (nativeHost &&
        (!existingHostParent ||
         winrt::get_abi(existingHostParent) != winrt::get_abi(hostParent) ||
         !AncestorChainContains(nativeHost, placeholderButton))) {
        if (existingHostParent) {
            uint32_t hostIndex = 0;
            auto hostSiblings = existingHostParent.Children();
            if (hostSiblings.IndexOf(nativeHost, hostIndex)) {
                hostSiblings.RemoveAt(hostIndex);
            }
        }
        nativeHost = nullptr;
        g_nativePlaceholderState.nativeHost = {};
        g_nativePlaceholderState.nativeHostParent = {};
        Wh_Log(L"NATIVE TEST: invalid host outside TaskListButton");
    }

    if (!nativeHost) {
        nativeHost =
            CreateNativeInsideButtonHost(hostParent, placeholderButton);
    }
    if (!nativeHost ||
        !AncestorChainContains(nativeHost, placeholderButton)) {
        Wh_Log(L"NATIVE TEST: invalid host outside TaskListButton");
        return ReconcileResult::noValidSeparators;
    }

    nativeHost.Name(L"WindhawkNativeSeparatorHost");
    nativeHost.Visibility(Visibility::Visible);
    nativeHost.Opacity(1);
    nativeHost.HorizontalAlignment(HorizontalAlignment::Stretch);
    nativeHost.VerticalAlignment(VerticalAlignment::Stretch);
    nativeHost.IsHitTestVisible(false);
    Controls::Canvas::SetZIndex(nativeHost, 9999);

    Shapes::Rectangle separator = nullptr;
    std::vector<Controls::Panel> candidateHosts{
        iconPanel, nativeHost.as<Controls::Panel>()};
    for (auto const& candidateHost : candidateHosts) {
        auto candidateChildren = candidateHost.Children();
        for (uint32_t index = 0; index < candidateChildren.Size();) {
            auto child = candidateChildren.GetAt(index)
                             .try_as<FrameworkElement>();
            if (!child ||
                child.Name() != L"WindhawkNativeTaskbarSeparator") {
                index++;
                continue;
            }

            auto rectangle = child.try_as<Shapes::Rectangle>();
            if (rectangle && !separator) {
                separator = rectangle;
            }
            candidateChildren.RemoveAt(index);
        }
    }

    bool separatorCreated = false;
    if (!separator) {
        Shapes::Rectangle newSeparator;
        separator = newSeparator;
        separatorCreated = true;
    }

    constexpr double kDebugWidth = 6;
    constexpr double kDebugHeight = 30;
    constexpr double kDebugOpacity = 1.0;
    constexpr winrt::Windows::UI::Color kDebugColor{255, 255, 255, 0};
    constexpr int kDebugZIndex = 10000;

    separator.Name(L"WindhawkNativeTaskbarSeparator");
    separator.Visibility(Visibility::Visible);
    separator.IsHitTestVisible(false);
    separator.HorizontalAlignment(HorizontalAlignment::Center);
    separator.VerticalAlignment(VerticalAlignment::Center);
    separator.Width(kDebugWidth);
    separator.Height(kDebugHeight);
    separator.Opacity(kDebugOpacity);
    separator.Fill(Media::SolidColorBrush{kDebugColor});
    Controls::Canvas::SetZIndex(separator, kDebugZIndex);

    Controls::Panel separatorHost = nativeHost.as<Controls::Panel>();
    // Re-append even an existing separator so it is always the final child.
    separatorHost.Children().Append(separator);
    ForceNativeSeparatorAncestorsVisible(
        separator, iconPanel, placeholderButton);

    g_nativePlaceholderState.separatorHost = winrt::make_weak(separatorHost);
    g_nativePlaceholderState.separator = winrt::make_weak(separator);

    if (separatorCreated &&
        !g_nativePlaceholderState.dividerVisibilityLogged) {
        auto parentClass = winrt::get_class_name(nativeHost);
        Wh_Log(
            L"NATIVE TEST: divider visible\n"
            L"parent name/class=%s/%s\n"
            L"actual width/height=%.2f/%.2f\n"
            L"configured color=#FFFF00\n"
            L"effective opacity=%.2f\n"
            L"z-index=%d",
            nativeHost.Name().c_str(), parentClass.c_str(),
            separator.ActualWidth(), separator.ActualHeight(),
            separator.Opacity(), Controls::Canvas::GetZIndex(separator));
        g_nativePlaceholderState.dividerVisibilityLogged = true;
    }
    if ((separator.ActualWidth() <= 0 ||
         separator.ActualHeight() <= 0) &&
        !g_nativePlaceholderState.dividerZeroSizeLogged) {
        Wh_Log(L"NATIVE TEST: divider ActualWidth or ActualHeight is zero");
        g_nativePlaceholderState.dividerZeroSizeLogged = true;
    }
    if (!g_nativePlaceholderState.diagnosticsLogged) {
        LogNativeSeparatorDiagnostics(separator, iconPanel,
                                      placeholderButton);
        g_nativePlaceholderState.diagnosticsLogged = true;
    }

    bool chainIsValid =
        AncestorChainContains(separator, placeholderButton);
    if (!chainIsValid) {
        Wh_Log(L"NATIVE TEST: invalid host outside TaskListButton");
        return ReconcileResult::noValidSeparators;
    }

    bool dimensionsReady =
        nativeHost.ActualWidth() > 0 && nativeHost.ActualHeight() > 0 &&
        separator.ActualWidth() > 0 && separator.ActualHeight() > 0;
    if (!g_nativePlaceholderState.hostReadyLogged && dimensionsReady) {
        Wh_Log(
            L"NATIVE TEST: host inside selected TaskListButton "
            L"host=%.2fx%.2f rectangle=%.2fx%.2f",
            nativeHost.ActualWidth(), nativeHost.ActualHeight(),
            separator.ActualWidth(), separator.ActualHeight());
        g_nativePlaceholderState.hostReadyLogged = true;
    }

    auto taskbarTransform = placeholderButton.RenderTransform();
    auto transformClass = taskbarTransform
                              ? winrt::get_class_name(taskbarTransform)
                              : L"(null)";
    bool hasTransformGroup = static_cast<bool>(
        taskbarTransform.try_as<Media::TransformGroup>());
    bool separatorHasOwnTransform =
        static_cast<bool>(separator.RenderTransform());
    if (hasTransformGroup && !separatorHasOwnTransform &&
        !g_nativePlaceholderState.transformGroupLogged) {
        Wh_Log(
            L"NATIVE TEST: inherited TaskListButton transform "
            L"class=%s TransformGroup=%d separatorOwnTransform=%d",
            transformClass.c_str(), hasTransformGroup,
            separatorHasOwnTransform);
        g_nativePlaceholderState.transformGroupLogged = true;
    }

    if (nativeHost.ActualWidth() <= 0 || nativeHost.ActualHeight() <= 0 ||
        separator.ActualWidth() <= 0 || separator.ActualHeight() <= 0) {
        return ReconcileResult::temporarilyNotReady;
    }

    return ReconcileResult::succeeded;
}
#endif

// This is the only overlay function that mutates the taskbar XAML visual tree.
// Callers must already be running on the taskbar XAML/UI thread.
ReconcileResult ReconcileDividers(bool enabled) {
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
    if constexpr (kSeparatorMode ==
                  SeparatorMode::NativePlaceholderExperimental) {
        static thread_local bool nativeReconciling = false;
        if (nativeReconciling) {
            return ReconcileResult::temporarilyNotReady;
        }

        nativeReconciling = true;
        struct NativeReconcileGuard {
            bool* active;
            ~NativeReconcileGuard() {
                *active = false;
            }
        } guard{&nativeReconciling};

        try {
            return ReconcileNativePlaceholderExperimental(enabled);
        } catch (...) {
            return ReconcileResult::temporarilyNotReady;
        }
    }
#endif

    static thread_local bool reconciling = false;
    static thread_local size_t lastActiveDividerCount =
        static_cast<size_t>(-1);
    if (reconciling) {
        return ReconcileResult::temporarilyNotReady;
    }

    reconciling = true;
    ReconcileResult result = ReconcileResult::temporarilyNotReady;

    try {
        Settings settings = GetSettingsSnapshot();
        unsigned int settingsGeneration =
            g_settingsGeneration.load(std::memory_order_acquire);
        bool settingsChanged =
            g_appliedSettingsGeneration != settingsGeneration;

        if (!settings.animationCompatibility) {
            StopAnimationTracking();
            DetachAnimationPointerHandlers();
            ClearAnimationElementCache();
        }

        FrameworkElement repeater =
            g_taskbarFrameRepeaterForCurrentThread.get();

        if (!repeater || repeater.Name() != L"TaskbarFrameRepeater") {
            HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
            XamlRoot xamlRoot =
                taskbarWnd ? GetTaskbarXamlRoot(taskbarWnd) : nullptr;
            auto root =
                xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>()
                         : nullptr;

            repeater =
                FindDescendantByName(root, L"TaskbarFrameRepeater");
            if (repeater) {
                g_taskbarFrameRepeaterForCurrentThread =
                    winrt::make_weak(repeater);
            }
        }

        if (repeater && repeater.Dispatcher().HasThreadAccess()) {
            struct RealizedElement {
                int index;
                FrameworkElement element;
            };

            std::vector<RealizedElement> realizedElements;
            auto repeaterPanel = repeater.try_as<Controls::Panel>();
            if (repeaterPanel) {
                for (auto const& panelChild : repeaterPanel.Children()) {
                    auto child = panelChild.try_as<FrameworkElement>();
                    if (!child) {
                        continue;
                    }

                    int index =
                        ItemsRepeater_GetElementIndex(repeater, child);
                    if (index < 0) {
                        continue;
                    }

                    auto realized =
                        ItemsRepeater_TryGetElement(repeater, index);
                    if (realized) {
                        realizedElements.push_back({index, realized});
                    }
                }
            }

            std::sort(realizedElements.begin(), realizedElements.end(),
                      [](RealizedElement const& left,
                         RealizedElement const& right) {
                          return left.index < right.index;
                      });

            std::vector<FrameworkElement> appButtons;
            for (auto const& realized : realizedElements) {
                if (realized.element.Name() == L"TaskListButton") {
                    appButtons.push_back(realized.element);
                }
            }

            std::vector<FrameworkElement> appIcons;
            appIcons.reserve(appButtons.size());
            for (size_t index = 0; index < appButtons.size(); index++) {
                appIcons.push_back(nullptr);
            }
            for (size_t buttonIndex = 0; buttonIndex < appButtons.size();
                 buttonIndex++) {
                auto iconPanel =
                    FindDescendantByName(appButtons[buttonIndex], L"IconPanel")
                        .try_as<Controls::Panel>();
                if (!iconPanel) {
                    continue;
                }

                auto children = iconPanel.Children();
                for (uint32_t childIndex = 0;
                     childIndex < children.Size();) {
                    auto child = children.GetAt(childIndex)
                                     .try_as<FrameworkElement>();
                    if (!child ||
                        child.Name() != L"WindhawkTaskbarSeparator") {
                        childIndex++;
                        continue;
                    }

                    children.RemoveAt(childIndex);
                }

                appIcons[buttonIndex] =
                    FindDescendantByName(iconPanel, L"Icon");
            }

            struct ActiveSeparator {
                SeparatorSettings settings;
                std::wstring name;
                bool beforeFirst = false;
            };

            std::vector<ActiveSeparator> activeSeparators;
            std::vector<int> usedPositions;
            if (enabled) {
                if (settings.separatorBeforeFirstApp) {
                    activeSeparators.push_back(
                        {SeparatorSettings{}, GetBeforeFirstDividerName(),
                         true});
                }
                for (auto const& separator : settings.separators) {
                    if (std::find(usedPositions.begin(), usedPositions.end(),
                                  separator.position) !=
                        usedPositions.end()) {
                        continue;
                    }

                    usedPositions.push_back(separator.position);
                    activeSeparators.push_back(
                        {separator,
                         GetDividerName(separator.settingsIndex), false});
                }
            }

            auto rootGrid = FindRootGridAncestor(repeater);
            Controls::Canvas overlayCanvas = nullptr;
            bool needOverlay = enabled && !activeSeparators.empty();

            if (rootGrid) {
                auto rootChildren = rootGrid.Children();
                for (uint32_t childIndex = 0;
                     childIndex < rootChildren.Size();) {
                    auto child = rootChildren.GetAt(childIndex)
                                     .try_as<FrameworkElement>();
                    if (!child) {
                        childIndex++;
                        continue;
                    }

                    if (child.Name() ==
                        L"WindhawkTaskbarSeparatorOverlay") {
                        auto canvas = child.try_as<Controls::Canvas>();
                        if (needOverlay && canvas && !overlayCanvas) {
                            overlayCanvas = canvas;
                            childIndex++;
                        } else {
                            rootChildren.RemoveAt(childIndex);
                        }
                        continue;
                    }

                    std::wstring childName{child.Name()};
                    if (IsOwnedDividerName(childName)) {
                        rootChildren.RemoveAt(childIndex);
                        continue;
                    }

                    childIndex++;
                }

                if (needOverlay && !overlayCanvas) {
                    Controls::Canvas overlay;
                    overlay.Name(L"WindhawkTaskbarSeparatorOverlay");
                    overlay.HorizontalAlignment(HorizontalAlignment::Stretch);
                    overlay.VerticalAlignment(VerticalAlignment::Stretch);
                    overlay.IsHitTestVisible(false);
                    Controls::Canvas::SetZIndex(overlay, 1000);
                    rootChildren.Append(overlay);
                    overlayCanvas = overlay;
                }

                if (overlayCanvas) {
                    if (overlayCanvas.HorizontalAlignment() !=
                        HorizontalAlignment::Stretch) {
                        overlayCanvas.HorizontalAlignment(
                            HorizontalAlignment::Stretch);
                    }
                    if (overlayCanvas.VerticalAlignment() !=
                        VerticalAlignment::Stretch) {
                        overlayCanvas.VerticalAlignment(
                            VerticalAlignment::Stretch);
                    }
                    if (overlayCanvas.IsHitTestVisible()) {
                        overlayCanvas.IsHitTestVisible(false);
                    }
                    if (Controls::Canvas::GetZIndex(overlayCanvas) != 1000) {
                        Controls::Canvas::SetZIndex(overlayCanvas, 1000);
                    }
                }
            }

            if (settings.animationCompatibility && needOverlay && rootGrid) {
                AttachAnimationPointerHandlers(rootGrid);
            } else if (enabled) {
                DetachAnimationPointerHandlers();
            }

            std::vector<Controls::Canvas> existingHosts;
            existingHosts.reserve(activeSeparators.size());
            for (size_t index = 0; index < activeSeparators.size(); index++) {
                existingHosts.push_back(nullptr);
            }
            if (overlayCanvas) {
                auto overlayChildren = overlayCanvas.Children();
                for (uint32_t childIndex = 0;
                     childIndex < overlayChildren.Size();) {
                    auto child = overlayChildren.GetAt(childIndex)
                                     .try_as<FrameworkElement>();
                    if (!child) {
                        childIndex++;
                        continue;
                    }

                    std::wstring childName{child.Name()};
                    if (!IsOwnedDividerName(childName)) {
                        childIndex++;
                        continue;
                    }

                    auto desired = std::find_if(
                        activeSeparators.begin(), activeSeparators.end(),
                        [&](ActiveSeparator const& separator) {
                            return separator.name == childName;
                        });
                    auto host = child.try_as<Controls::Canvas>();
                    if (desired == activeSeparators.end() || !host) {
                        overlayChildren.RemoveAt(childIndex);
                        continue;
                    }

                    size_t desiredIndex =
                        static_cast<size_t>(
                            desired - activeSeparators.begin());
                    if (existingHosts[desiredIndex]) {
                        overlayChildren.RemoveAt(childIndex);
                        continue;
                    }

                    existingHosts[desiredIndex] = host;
                    childIndex++;
                }
            }

            TaskbarOrientation taskbarOrientation =
                TaskbarOrientation::horizontal;
            bool orientationValid =
                overlayCanvas &&
                TryGetTaskbarOrientation(
                    settings.orientation, overlayCanvas, appIcons,
                    &taskbarOrientation);

            std::vector<AnimationDividerCache> animationDividers;
            size_t activeDividerCount = 0;
            for (size_t activeIndex = 0;
                 activeIndex < activeSeparators.size(); activeIndex++) {
                auto const& activeSeparator =
                    activeSeparators[activeIndex];
                auto const& separator = activeSeparator.settings;

                FrameworkElement previousIcon = nullptr;
                FrameworkElement targetIcon = nullptr;
                FrameworkElement nextIcon = nullptr;
                if (activeSeparator.beforeFirst) {
                    if (!appIcons.empty()) {
                        targetIcon = appIcons[0];
                        if (appIcons.size() >= 2) {
                            nextIcon = appIcons[1];
                        }
                    }
                } else {
                    size_t buttonIndex =
                        static_cast<size_t>(separator.position - 1);
                    if (buttonIndex < appIcons.size()) {
                        targetIcon = appIcons[buttonIndex];
                        if (buttonIndex > 0) {
                            previousIcon = appIcons[buttonIndex - 1];
                        }
                        if (buttonIndex + 1 < appIcons.size()) {
                            nextIcon = appIcons[buttonIndex + 1];
                        }
                    }
                }

                double hostWidth = 0;
                double hostHeight = 0;
                GetStyleHostSize(settings, taskbarOrientation, &hostWidth,
                                 &hostHeight);
                DividerGeometry geometry;
                bool geometryValid = false;
                if (orientationValid && targetIcon) {
                    geometryValid = activeSeparator.beforeFirst
                                        ? TryGetBeforeFirstDividerGeometry(
                                                  overlayCanvas, targetIcon,
                                                  nextIcon,
                                                  taskbarOrientation,
                                                  hostWidth, hostHeight,
                                                  &geometry)
                                        : (nextIcon || previousIcon) &&
                                              TryGetDividerGeometry(
                                                  overlayCanvas, previousIcon,
                                                  targetIcon, nextIcon,
                                                  taskbarOrientation,
                                                  hostWidth, hostHeight,
                                                  &geometry);
                }
                if (!geometryValid) {
                    auto staleHost = existingHosts[activeIndex];
                    if (staleHost && overlayCanvas) {
                        uint32_t staleIndex = 0;
                        auto overlayChildren = overlayCanvas.Children();
                        if (overlayChildren.IndexOf(staleHost,
                                                    staleIndex)) {
                            overlayChildren.RemoveAt(staleIndex);
                        }
                    }
                    continue;
                }

                auto host = existingHosts[activeIndex];
                if (!host) {
                    Controls::Canvas newHost;
                    newHost.Name(activeSeparator.name);
                    newHost.HorizontalAlignment(
                        HorizontalAlignment::Left);
                    newHost.VerticalAlignment(VerticalAlignment::Top);
                    newHost.IsHitTestVisible(false);
                    newHost.UseLayoutRounding(true);
                    Controls::Canvas::SetZIndex(newHost, 1000);
                    overlayCanvas.Children().Append(newHost);
                    host = newHost;
                }

                if (host.HorizontalAlignment() !=
                    HorizontalAlignment::Left) {
                    host.HorizontalAlignment(HorizontalAlignment::Left);
                }
                if (host.VerticalAlignment() !=
                    VerticalAlignment::Top) {
                    host.VerticalAlignment(VerticalAlignment::Top);
                }
                if (host.IsHitTestVisible()) {
                    host.IsHitTestVisible(false);
                }
                if (!host.UseLayoutRounding()) {
                    host.UseLayoutRounding(true);
                }
                if (Controls::Canvas::GetZIndex(host) != 1000) {
                    Controls::Canvas::SetZIndex(host, 1000);
                }
                if (host.Width() != hostWidth) {
                    host.Width(hostWidth);
                }
                if (host.Height() != hostHeight) {
                    host.Height(hostHeight);
                }
                if (Controls::Canvas::GetLeft(host) != geometry.left) {
                    Controls::Canvas::SetLeft(host, geometry.left);
                }
                if (Controls::Canvas::GetTop(host) != geometry.top) {
                    Controls::Canvas::SetTop(host, geometry.top);
                }

                double opacity = settings.opacityPercent / 100.0;
                if (host.Opacity() != opacity) {
                    host.Opacity(opacity);
                }
                ConfigureStyleHost(host, settings, taskbarOrientation,
                                   hostWidth, hostHeight);

                if (settings.animationCompatibility) {
                    AnimationDividerCache cache;
                    cache.host = winrt::make_weak(host);
                    if (previousIcon) {
                        cache.previousIcon =
                            winrt::make_weak(previousIcon);
                    }
                    cache.targetIcon = winrt::make_weak(targetIcon);
                    if (nextIcon) {
                        cache.nextIcon = winrt::make_weak(nextIcon);
                    }
                    cache.orientation = taskbarOrientation;
                    cache.beforeFirst = activeSeparator.beforeFirst;
                    animationDividers.push_back(std::move(cache));
                }
                activeDividerCount++;
            }

            g_animationDividers = std::move(animationDividers);
            if (settings.animationCompatibility &&
                !g_animationDividers.empty() && overlayCanvas) {
                g_animationOverlayCanvas =
                    winrt::make_weak(overlayCanvas);
                if (g_animationPointerInside) {
                    StartAnimationTracking();
                }
            } else {
                StopAnimationTracking();
                ClearAnimationElementCache();
                if (enabled) {
                    DetachAnimationPointerHandlers();
                }
            }

            if (!enabled || activeSeparators.empty()) {
                result = ReconcileResult::noValidSeparators;
            } else if (activeDividerCount == activeSeparators.size()) {
                result = ReconcileResult::succeeded;
            }

            if (settingsChanged ||
                lastActiveDividerCount != activeDividerCount) {
                if (result == ReconcileResult::temporarilyNotReady) {
                    Wh_Log(L"RECONCILE: failed: active %zu of %zu dividers",
                           activeDividerCount, activeSeparators.size());
                } else {
                    Wh_Log(L"RECONCILE: succeeded with %zu active dividers",
                           activeDividerCount);
                }
                lastActiveDividerCount = activeDividerCount;
            }

            g_appliedSettingsGeneration = settingsGeneration;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"RECONCILE: failed: 0x%08X %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
    } catch (...) {
        Wh_Log(L"RECONCILE: failed with an unknown exception");
    }

    reconciling = false;
    return result;
}

void CancelInitialReconcileRetry() {
    if (g_initialReconcileTimer) {
        g_initialReconcileTimer.Stop();
        g_initialReconcileTimer.Tick(g_initialReconcileTimerToken);
    }

    g_initialReconcileTimer = nullptr;
    g_initialReconcileTimerToken = {};
    g_initialReconcileAttempts = 0;
}

void OnInitialReconcileTimerTick(
    winrt::Windows::Foundation::IInspectable const&,
    winrt::Windows::Foundation::IInspectable const&) {
    if (g_unloading) {
        CancelInitialReconcileRetry();
        return;
    }

    constexpr int kMaximumAttempts = 20;
    g_initialReconcileAttempts++;
    ReconcileResult result = ReconcileDividers(true);
    if (result == ReconcileResult::succeeded) {
        CancelInitialReconcileRetry();
    } else if (result == ReconcileResult::noValidSeparators
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
               || result == ReconcileResult::placeholderNotFound
#endif
    ) {
        CancelInitialReconcileRetry();
    } else if (g_initialReconcileAttempts >= kMaximumAttempts) {
        int attempts = g_initialReconcileAttempts;
        CancelInitialReconcileRetry();
        Wh_Log(L"RECONCILE: initialization failed after %d attempts",
               attempts);
    }
}

void BeginInitialReconcileOnUiThread() {
    CancelInitialReconcileRetry();

    g_initialReconcileAttempts = 1;
    ReconcileResult result = ReconcileDividers(true);
    if (result == ReconcileResult::succeeded) {
        g_initialReconcileAttempts = 0;
        return;
    }
    if (result == ReconcileResult::noValidSeparators
#if defined(TASKBAR_SEPARATORS_NATIVE_PLACEHOLDER_TEST)
        || result == ReconcileResult::placeholderNotFound
#endif
    ) {
        g_initialReconcileAttempts = 0;
        return;
    }

    DispatcherTimer timer;
    timer.Interval(std::chrono::milliseconds(100));
    g_initialReconcileTimerToken = timer.Tick(
        winrt::Windows::Foundation::EventHandler<
            winrt::Windows::Foundation::IInspectable>{
            OnInitialReconcileTimerTick});
    g_initialReconcileTimer = timer;
    timer.Start();
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButton = taskListButtonIUnknown.try_as<FrameworkElement>();
    if (taskListButton) {
        auto repeater = FindRepeaterAncestor(taskListButton);
        if (repeater) {
            g_taskbarFrameRepeaterForCurrentThread =
                winrt::make_weak(repeater);
            ReconcileDividers(!g_unloading.load());
        }
    }
}

bool RunReconcileOnTaskbarThread(bool enabled,
                                 bool beginBoundedRetry = false) {
    struct RECONCILE_REQUEST {
        bool enabled;
        bool beginBoundedRetry;
        bool completed;
    };

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        Wh_Log(L"RECONCILE: Shell_TrayWnd not found");
        return false;
    }

    HWND taskbarUiWnd = GetTaskbarUiWnd();
    if (!taskbarUiWnd) {
        // Shell_TrayWnd is the fallback used by other official taskbar mods.
        // On current Windows 11 builds it belongs to the same taskbar UI thread.
        taskbarUiWnd = taskbarWnd;
    }

    RECONCILE_REQUEST request{enabled, beginBoundedRetry, false};
    bool dispatched = RunFromWindowThread(
        taskbarUiWnd,
        [](PVOID parameter) {
            auto* request =
                static_cast<RECONCILE_REQUEST*>(parameter);
            if (!request->enabled) {
                ClearAnimationTrackingForUnload();
                ReconcileDividers(false);
            } else if (request->beginBoundedRetry) {
                BeginInitialReconcileOnUiThread();
            } else {
                ReconcileDividers(true);
            }
            request->completed = true;
        },
        &request);

    if (!dispatched || !request.completed) {
        Wh_Log(L"RECONCILE: failed to run synchronously on the taskbar UI "
               L"thread");
        return false;
    }

    return true;
}

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
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
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

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols for taskbar.dll failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
    };

    if (!HookSymbols(module, taskbarViewHooks,
                     ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule) {
    return HookTaskbarViewDllSymbols(taskbarViewModule) ? TRUE : FALSE;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR fileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        if (ModInitWithTaskbarView(module)) {
            Wh_ApplyHookOperations();
            RunReconcileOnTaskbarThread(true, true);
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, fileName);
    }
    return module;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!ModInitWithTaskbarView(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto loadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
        if (!loadLibraryExW) {
            Wh_Log(L"Failed to resolve LoadLibraryExW");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (ModInitWithTaskbarView(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (g_taskbarViewDllLoaded) {
        RunReconcileOnTaskbarThread(true, true);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Taskbar Separators cleanup starting");

    g_unloading = true;

    if (g_taskbarViewDllLoaded) {
        // RunFromWindowThread uses SendMessage, so this doesn't return until
        // the retry timer is stopped, rendering and pointer callbacks are
        // detached, their weak references are cleared, and
        // ReconcileDividers(false) has finished on the taskbar UI thread.
        RunReconcileOnTaskbarThread(false);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Taskbar Separators cleanup complete");
}

void Wh_ModSettingsChanged() {
    LoadSettings(true);

    if (g_taskbarViewDllLoaded && !g_unloading) {
        RunReconcileOnTaskbarThread(true, true);
    }
}
