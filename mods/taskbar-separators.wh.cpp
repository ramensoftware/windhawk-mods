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

Add clean, customizable visual separators between application buttons on the
Windows 11 taskbar.

Unlike placeholder applications or pinned shortcuts, these separators are
visual, non-clickable elements. They do not launch programs or occupy normal
application slots.

## Preview

![Taskbar Separators preview](https://raw.githubusercontent.com/digart11/taskbar-separators/master/images/taskbar-separators-preview.jpg)

## Features

- Add multiple separators at configurable taskbar positions
- Optional separator before the first application button
- Live position and appearance updates
- Five configurable visual styles:
  - Fade
  - Solid
  - Double
  - Rounded
  - Glow
- Adjustable thickness, length, opacity, color, and effect settings
- Automatic horizontal and vertical taskbar orientation
- Optional animation compatibility mode
- Clean removal when the mod is disabled
- No changes to taskbar button margins, padding, or application behavior

## Getting started

1. Open the mod's **Settings** tab.
2. Add separator positions to the **Separators** list.
3. A position of `3` places a separator after the third application button.
4. Select a style and adjust its appearance.

## Position behavior

Numbered positions place separators after application buttons:

- Position `1` places a separator after the first application button
- Position `2` places a separator after the second application button
- Position `3` places a separator after the third application button

Enable **Separator before first app** to place a separator before the first
application button.

Start, Search, Widgets, Task View, and other system buttons are not counted as
application buttons.

Positions follow the current visual order of taskbar application buttons.
Opening, closing, pinning, unpinning, or rearranging applications can change
which icons appear beside a configured separator.

## Settings

![Taskbar Separators settings](https://raw.githubusercontent.com/digart11/taskbar-separators/master/images/taskbar-separators-settings.jpg)

## Alternate setups

![Taskbar Separators alternate setups](https://raw.githubusercontent.com/digart11/taskbar-separators/master/images/taskbar-separators-alt.jpg)

## Animation compatibility

Static taskbars are fully supported.

The mod can follow icons animated by other taskbar mods, but very fast animation
may not remain perfectly synchronized because both mods update their visual
elements independently.

This affects animation appearance only and does not affect normal static
separator positioning.

## Compatibility

- Windows 11 horizontal taskbars
- Vertical taskbars via Vertical Taskbar for Windows 11
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
- animationCompatibility: "off"
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
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

namespace
{

    enum class ReconcileResult
    {
        succeeded,
        succeededPartial,
        temporarilyNotReady,
        noValidSeparators,
    };

    enum class TaskbarOrientation
    {
        horizontal,
        vertical,
    };

    enum class OrientationSetting
    {
        automatic,
        horizontal,
        vertical,
    };

    enum class DividerStyle
    {
        solid,
        rounded,
        fade,
        glow,
        doubleLine,
    };

    struct SeparatorSettings
    {
        size_t settingsIndex = 0;
        int position = 3;
    };

    struct Settings
    {
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
        bool animationCompatibility = false;
        bool separatorBeforeFirstApp = false;
        std::vector<SeparatorSettings> separators;
    };

    std::mutex g_settingsMutex;
    Settings g_settings;
    std::atomic<unsigned int> g_settingsGeneration{0};

    std::atomic<bool> g_taskbarViewDllLoaded{false};
    std::atomic<bool> g_unloading{false};
    std::atomic<bool> g_unloadSuppressionLogged{false};

    bool SuppressEnabledWorkDuringUnload(PCWSTR path)
    {
        if (!g_unloading.load(std::memory_order_acquire))
        {
            return false;
        }

        if (!g_unloadSuppressionLogged.exchange(true))
        {
            Wh_Log(L"RECONCILE: suppressed during unload (%s)", path);
        }
        return true;
    }

    struct AnimationDividerCache
    {
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

    using AnimationClock = std::chrono::steady_clock;
    constexpr auto kAnimationTrackingTimeout = std::chrono::seconds(3);
    constexpr auto kNativeSettlingTimeout = std::chrono::seconds(1);
    constexpr int kGeometryStableFrameThreshold = 12;

    struct ReconciledButtonSignature
    {
        int itemIndex = -1;
        winrt::weak_ref<FrameworkElement> button;
        winrt::weak_ref<FrameworkElement> iconPanel;
        winrt::weak_ref<FrameworkElement> icon;
        bool iconResolved = false;
        double actualWidth = 0;
        double actualHeight = 0;
        Thickness margin{};
        Visibility visibility = Visibility::Visible;
    };

    struct ReconciledSeparatorVisual
    {
        std::wstring name;
        winrt::weak_ref<Controls::Canvas> host;
    };

    struct TrackedTaskbarState
    {
        size_t id = 0;
        winrt::weak_ref<FrameworkElement> repeater;
        winrt::weak_ref<Controls::Grid> rootGrid;
        winrt::weak_ref<Controls::Canvas> overlayCanvas;
        unsigned int appliedSettingsGeneration = 0;
        size_t lastActiveDividerCount = static_cast<size_t>(-1);

        bool reconciliationSignatureValid = false;
        ReconcileResult cachedReconcileResult =
            ReconcileResult::temporarilyNotReady;
        winrt::weak_ref<FrameworkElement> reconciledRepeater;
        winrt::weak_ref<Controls::Grid> reconciledRootGrid;
        double reconciledRootWidth = 0;
        double reconciledRootHeight = 0;
        winrt::Windows::Foundation::Rect reconciledRepeaterBounds{};
        std::vector<ReconciledButtonSignature> reconciledButtons;
        bool reconciledOverlayExpected = false;
        uint32_t reconciledOverlayChildCount = 0;
        bool reconciledAnimationCompatibility = false;
        std::vector<ReconciledSeparatorVisual> reconciledSeparators;

        std::vector<AnimationDividerCache> animationDividers;
        winrt::weak_ref<Controls::Canvas> animationOverlayCanvas;
        winrt::weak_ref<FrameworkElement> animationPointerSource;
        winrt::Windows::Foundation::IInspectable
            animationPointerMovedHandler{nullptr};
        winrt::event_token animationPointerExitedToken{};
        winrt::event_token animationRenderingToken{};
        bool animationPointerHandlersAttached = false;
        bool animationPointerExitedHandlerAttached = false;
        bool animationRenderingSubscribed = false;
        bool animationPointerInside = false;
        bool animationRenderingCallbackActive = false;
        int animationStableFrames = 0;
        AnimationClock::time_point animationLastActivity{};
        bool nativeSettlingActive = false;
        int nativeSettlingStableFrames = 0;
        AnimationClock::time_point nativeSettlingStarted{};
    };

    using TrackedTaskbarCollection = std::vector<TrackedTaskbarState>;

    // Accessed only from taskbar XAML/UI-thread callbacks. Keep the TLS object
    // itself trivially destructible: normal unload cleanup explicitly destroys
    // the collection on the taskbar UI thread, and no TLS destructor can run
    // after this mod DLL has been unloaded.
    thread_local TrackedTaskbarCollection *g_trackedTaskbars = nullptr;
    thread_local size_t g_nextTrackedTaskbarId = 1;
    thread_local bool g_reconcilingTaskbars = false;

    struct InitialReconcileTimerState
    {
        DispatcherTimer timer{nullptr};
    };

    thread_local InitialReconcileTimerState *g_initialReconcileTimer = nullptr;
    thread_local winrt::event_token g_initialReconcileTimerToken{};
    thread_local int g_initialReconcileAttempts = 0;

    void CancelInitialReconcileRetry();
    Controls::Grid DiscoverPrimaryTaskbarRootGrid();

    TrackedTaskbarCollection &GetTrackedTaskbars()
    {
        if (!g_trackedTaskbars)
        {
            g_trackedTaskbars = new TrackedTaskbarCollection;
        }

        return *g_trackedTaskbars;
    }

    void DestroyTrackedTaskbars()
    {
        auto *trackedTaskbars = g_trackedTaskbars;
        g_trackedTaskbars = nullptr;
        delete trackedTaskbars;
    }

    int ClampSetting(PCWSTR, int value, int minimum, int maximum)
    {
        return std::clamp(value, minimum, maximum);
    }

    int HexDigitValue(wchar_t character)
    {
        if (character >= L'0' && character <= L'9')
        {
            return character - L'0';
        }

        if (character >= L'a' && character <= L'f')
        {
            return character - L'a' + 10;
        }

        if (character >= L'A' && character <= L'F')
        {
            return character - L'A' + 10;
        }

        return -1;
    }

    bool ParseHexByte(PCWSTR text, uint8_t *result)
    {
        int high = HexDigitValue(text[0]);
        int low = HexDigitValue(text[1]);
        if (high < 0 || low < 0)
        {
            return false;
        }

        *result = static_cast<uint8_t>((high << 4) | low);
        return true;
    }

    bool ParseColor(PCWSTR text, winrt::Windows::UI::Color *result)
    {
        if (!text || text[0] != L'#')
        {
            return false;
        }

        size_t length = wcslen(text);
        if (length != 7 && length != 9)
        {
            return false;
        }

        winrt::Windows::UI::Color color{255, 255, 255, 255};
        size_t rgbOffset = 1;

        if (length == 9)
        {
            if (!ParseHexByte(text + 1, &color.A))
            {
                return false;
            }
            rgbOffset = 3;
        }

        if (!ParseHexByte(text + rgbOffset, &color.R) ||
            !ParseHexByte(text + rgbOffset + 2, &color.G) ||
            !ParseHexByte(text + rgbOffset + 4, &color.B))
        {
            return false;
        }

        *result = color;
        return true;
    }

    DividerStyle ParseDividerStyle(PCWSTR text)
    {
        if (text && wcscmp(text, L"solid") == 0)
        {
            return DividerStyle::solid;
        }
        if (text && wcscmp(text, L"rounded") == 0)
        {
            return DividerStyle::rounded;
        }
        if (text && wcscmp(text, L"fade") == 0)
        {
            return DividerStyle::fade;
        }
        if (text && wcscmp(text, L"glow") == 0)
        {
            return DividerStyle::glow;
        }
        if (text && wcscmp(text, L"double") == 0)
        {
            return DividerStyle::doubleLine;
        }
        return DividerStyle::fade;
    }

    void LoadSettings()
    {
        Settings settings;

        settings.width =
            ClampSetting(L"width", Wh_GetIntSetting(L"width"), 1, 8);
        settings.height =
            ClampSetting(L"height", Wh_GetIntSetting(L"height"), 4, 48);
        settings.opacityPercent =
            ClampSetting(L"opacity", Wh_GetIntSetting(L"opacity"), 0, 100);

        PCWSTR colorText = Wh_GetStringSetting(L"color");
        if (!ParseColor(colorText, &settings.color))
        {
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
        if (orientationText && wcscmp(orientationText, L"horizontal") == 0)
        {
            settings.orientation = OrientationSetting::horizontal;
        }
        else if (orientationText &&
                 wcscmp(orientationText, L"vertical") == 0)
        {
            settings.orientation = OrientationSetting::vertical;
        }
        Wh_FreeStringSetting(orientationText);

        PCWSTR animationCompatibilityText =
            Wh_GetStringSetting(L"animationCompatibility");
        settings.animationCompatibility =
            animationCompatibilityText &&
            wcscmp(animationCompatibilityText, L"on") == 0;
        Wh_FreeStringSetting(animationCompatibilityText);
        settings.separatorBeforeFirstApp =
            Wh_GetIntSetting(L"separatorBeforeFirstApp") != 0;

        auto appendSeparator = [&](size_t settingsIndex, int position)
        {
            SeparatorSettings separator;
            separator.settingsIndex = settingsIndex;
            separator.position = std::max(position, 1);
            settings.separators.push_back(separator);
        };

        for (int index = 0; index < 128; index++)
        {
            int position = Wh_GetIntSetting(L"separators[%d]", index);
            if (position == 0)
            {
                break;
            }
            appendSeparator(static_cast<size_t>(index), position);
        }

        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            g_settings = settings;
        }

        g_settingsGeneration.fetch_add(1, std::memory_order_release);
        Wh_Log(L"SETTINGS: loaded %zu dividers", settings.separators.size());
    }

    Settings GetSettingsSnapshot()
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        return g_settings;
    }

    bool ColorsEqual(winrt::Windows::UI::Color const &left,
                     winrt::Windows::UI::Color const &right)
    {
        return left.A == right.A && left.R == right.R && left.G == right.G &&
               left.B == right.B;
    }

    std::vector<HWND> EnumerateCurrentProcessTaskbarWindows()
    {
        struct TaskbarWindows
        {
            std::vector<HWND> primary;
            std::vector<HWND> secondary;
        } taskbarWindows;

        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL
            {
                DWORD processId = 0;
                WCHAR className[32]{};

                if (GetWindowThreadProcessId(hWnd, &processId) &&
                    processId == GetCurrentProcessId() &&
                    GetClassName(hWnd, className, ARRAYSIZE(className)))
                {
                    auto *taskbarWindows =
                        reinterpret_cast<TaskbarWindows *>(lParam);
                    if (_wcsicmp(className, L"Shell_TrayWnd") == 0)
                    {
                        taskbarWindows->primary.push_back(hWnd);
                    }
                    else if (_wcsicmp(className,
                                      L"Shell_SecondaryTrayWnd") == 0)
                    {
                        taskbarWindows->secondary.push_back(hWnd);
                    }
                }

                return TRUE;
            },
            reinterpret_cast<LPARAM>(&taskbarWindows));

        std::vector<HWND> result;
        result.reserve(taskbarWindows.primary.size() +
                       taskbarWindows.secondary.size());
        result.insert(result.end(), taskbarWindows.primary.begin(),
                      taskbarWindows.primary.end());
        result.insert(result.end(), taskbarWindows.secondary.begin(),
                      taskbarWindows.secondary.end());
        return result;
    }

    HWND FindCurrentProcessTaskbarWnd()
    {
        for (HWND taskbarWnd : EnumerateCurrentProcessTaskbarWindows())
        {
            WCHAR className[32]{};
            if (GetClassName(taskbarWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                return taskbarWnd;
            }
        }

        return nullptr;
    }

    HWND GetTaskbarDispatchWindow(HWND taskbarWnd)
    {
        if (!taskbarWnd)
        {
            return nullptr;
        }

        HWND taskbarUiWnd = FindWindowEx(
            taskbarWnd, nullptr,
            L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        return taskbarUiWnd ? taskbarUiWnd : taskbarWnd;
    }

    HWND GetTaskbarUiWnd()
    {
        auto taskbarWindows = EnumerateCurrentProcessTaskbarWindows();
        return taskbarWindows.empty()
                   ? nullptr
                   : GetTaskbarDispatchWindow(taskbarWindows.front());
    }

    using RunFromWindowThreadProc_t = void(WINAPI *)(PVOID parameter);

    // Adapted from the official Windows 11 Taskbar Styler mod. SendMessage is
    // synchronous; a true return means the hook observed the message and the
    // callback returned.
    bool RunFromWindowThread(HWND hWnd,
                             RunFromWindowThreadProc_t proc,
                             PVOID procParam)
    {
        static const UINT runFromWindowThreadRegisteredMsg =
            RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

        struct RUN_FROM_WINDOW_THREAD_PARAM
        {
            RunFromWindowThreadProc_t proc;
            PVOID procParam;
            bool callbackRan;
        };

        DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (threadId == 0)
        {
            return false;
        }

        if (threadId == GetCurrentThreadId())
        {
            proc(procParam);
            return true;
        }

        HHOOK hook = SetWindowsHookEx(
            WH_CALLWNDPROC,
            [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
            {
                if (nCode == HC_ACTION)
                {
                    const CWPSTRUCT *cwp = reinterpret_cast<const CWPSTRUCT *>(lParam);
                    if (cwp->message == runFromWindowThreadRegisteredMsg)
                    {
                        auto *param = reinterpret_cast<
                            RUN_FROM_WINDOW_THREAD_PARAM *>(cwp->lParam);
                        param->callbackRan = true;
                        param->proc(param->procParam);
                    }
                }

                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            },
            nullptr, threadId);
        if (!hook)
        {
            return false;
        }

        RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam, false};
        SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                    reinterpret_cast<LPARAM>(&param));

        UnhookWindowsHookEx(hook);
        return param.callbackRan;
    }

    void *CTaskBand_ITaskListWndSite_vftable;
    void *CSecondaryTaskBand_ITaskListWndSite_vftable;

    using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
    CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

    using CSecondaryTaskBand_GetTaskbarHost_t =
        void *(WINAPI *)(void *pThis, void **result);
    CSecondaryTaskBand_GetTaskbarHost_t
        CSecondaryTaskBand_GetTaskbarHost_Original;

    void *TaskbarHost_FrameHeight_Original;

    using std__Ref_count_base__Decref_t = void(WINAPI *)(void *pThis);
    std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

    // Adapted from the official Taskbar Multirow mod. This obtains the existing
    // taskbar XamlRoot without installing another XAML diagnostics client, which
    // keeps this mod compatible with Taskbar Styler.
    XamlRoot XamlRootFromTaskbarHostSharedPtr(void *taskbarHostSharedPtr[2])
    {
        if (!taskbarHostSharedPtr[0])
        {
            if (taskbarHostSharedPtr[1])
            {
                std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
            }
            return nullptr;
        }

        size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
        const BYTE *code = static_cast<const BYTE *>(TaskbarHost_FrameHeight_Original);
        if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
            code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
            code[7] <= 0x7F)
        {
            taskbarElementIUnknownOffset = code[7];
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight prologue");
        }
#elif defined(_M_ARM64)
        // Keep the default 0x48 offset.
#else
#error "Unsupported architecture"
#endif

        auto *taskbarElementIUnknown =
            *reinterpret_cast<IUnknown **>(
                static_cast<BYTE *>(taskbarHostSharedPtr[0]) +
                taskbarElementIUnknownOffset);

        FrameworkElement taskbarElement = nullptr;
        if (taskbarElementIUnknown)
        {
            taskbarElementIUnknown->QueryInterface(
                winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
        }

        XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

        if (taskbarHostSharedPtr[1])
        {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        }

        return result;
    }

    XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd)
    {
        HWND taskSwitchWnd =
            reinterpret_cast<HWND>(GetProp(taskbarWnd, L"TaskbandHWND"));
        if (!taskSwitchWnd)
        {
            return nullptr;
        }

        void *taskBand =
            reinterpret_cast<void *>(GetWindowLongPtr(taskSwitchWnd, 0));
        if (!taskBand)
        {
            return nullptr;
        }

        void *taskBandForTaskListWndSite = taskBand;
        for (int i = 0;
             *reinterpret_cast<void **>(taskBandForTaskListWndSite) !=
             CTaskBand_ITaskListWndSite_vftable;
             i++)
        {
            if (i == 20)
            {
                return nullptr;
            }

            taskBandForTaskListWndSite =
                reinterpret_cast<void **>(taskBandForTaskListWndSite) + 1;
        }
        void *taskbarHostSharedPtr[2]{};
        CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                          taskbarHostSharedPtr);
        return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
    }

    // Established taskbar-multirow pattern for Shell_SecondaryTrayWnd: the
    // secondary task-band object is stored on its WorkerW child, not in the
    // primary-only TaskbandHWND property.
    XamlRoot GetSecondaryTaskbarXamlRoot(HWND secondaryTaskbarWnd)
    {
        HWND taskSwitchWnd = FindWindowEx(
            secondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
        if (!taskSwitchWnd)
        {
            return nullptr;
        }

        void *taskBand =
            reinterpret_cast<void *>(GetWindowLongPtr(taskSwitchWnd, 0));
        if (!taskBand)
        {
            return nullptr;
        }

        void *taskBandForTaskListWndSite = taskBand;
        for (int i = 0;
             *reinterpret_cast<void **>(taskBandForTaskListWndSite) !=
             CSecondaryTaskBand_ITaskListWndSite_vftable;
             i++)
        {
            if (i == 20)
            {
                return nullptr;
            }

            taskBandForTaskListWndSite =
                reinterpret_cast<void **>(taskBandForTaskListWndSite) + 1;
        }
        void *taskbarHostSharedPtr[2]{};
        CSecondaryTaskBand_GetTaskbarHost_Original(
            taskBandForTaskListWndSite, taskbarHostSharedPtr);
        return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
    }

    XamlRoot GetTaskbarXamlRootForWindow(HWND taskbarWnd)
    {
        WCHAR className[32]{};
        if (!taskbarWnd ||
            !GetClassName(taskbarWnd, className, ARRAYSIZE(className)))
        {
            return nullptr;
        }

        if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)
        {
            return GetSecondaryTaskbarXamlRoot(taskbarWnd);
        }
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0)
        {
            return GetTaskbarXamlRoot(taskbarWnd);
        }
        return nullptr;
    }

    // {0BD894F2-EDFC-5DDF-A166-2DB14BBFDF35}
    constexpr winrt::guid IItemsRepeater{
        0x0BD894F2,
        0xEDFC,
        0x5DDF,
        {0xA1, 0x66, 0x2D, 0xB1, 0x4B, 0xBF, 0xDF, 0x35}};

    int ItemsRepeater_GetElementIndex(FrameworkElement repeater,
                                      UIElement element)
    {
        winrt::Windows::Foundation::IUnknown repeaterUnknown = nullptr;
        repeater.as(IItemsRepeater, winrt::put_abi(repeaterUnknown));

        using GetElementIndex_t =
            HRESULT(WINAPI *)(void *pThis, void *element, void *index);

        void **vtable = *(void ***)winrt::get_abi(repeaterUnknown);
        auto getElementIndex = (GetElementIndex_t)vtable[19];

        int index = -1;
        getElementIndex(winrt::get_abi(repeaterUnknown), winrt::get_abi(element),
                        &index);
        return index;
    }

    FrameworkElement FindRepeaterAncestor(FrameworkElement element)
    {
        auto current = element;

        for (int depth = 0; depth < 16 && current; depth++)
        {
            auto parentObject = Media::VisualTreeHelper::GetParent(current);
            auto parent = parentObject.try_as<FrameworkElement>();
            if (!parent)
            {
                return nullptr;
            }

            if (parent.Name() == L"TaskbarFrameRepeater")
            {
                return parent;
            }

            current = parent;
        }

        return nullptr;
    }

    FrameworkElement FindDescendantByName(FrameworkElement root,
                                          PCWSTR name,
                                          int depth = 0)
    {
        if (!root || depth > 12)
        {
            return nullptr;
        }

        int childCount = Media::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < childCount; i++)
        {
            auto child = Media::VisualTreeHelper::GetChild(root, i)
                             .try_as<FrameworkElement>();
            if (!child)
            {
                continue;
            }

            if (child.Name() == name)
            {
                return child;
            }

            auto found = FindDescendantByName(child, name, depth + 1);
            if (found)
            {
                return found;
            }
        }

        return nullptr;
    }

    Controls::Grid FindRootGridAncestor(FrameworkElement element)
    {
        auto current = element;

        for (int depth = 0; depth < 16 && current; depth++)
        {
            auto parent = Media::VisualTreeHelper::GetParent(current)
                              .try_as<FrameworkElement>();
            if (!parent)
            {
                return nullptr;
            }

            if (parent.Name() == L"RootGrid")
            {
                return parent.try_as<Controls::Grid>();
            }

            current = parent;
        }

        return nullptr;
    }

    struct DividerGeometry
    {
        winrt::Windows::Foundation::Rect targetBounds{};
        winrt::Windows::Foundation::Rect nextBounds{};
        double center = 0;
        double left = 0;
        double top = 0;
    };

    bool TryGetIconBounds(
        Controls::Canvas const &overlayCanvas,
        FrameworkElement const &icon,
        winrt::Windows::Foundation::Rect *bounds)
    {
        if (!overlayCanvas || !icon || !bounds)
        {
            return false;
        }

        double width = icon.ActualWidth();
        double height = icon.ActualHeight();
        if (!std::isfinite(width) || width <= 0 || !std::isfinite(height) ||
            height <= 0)
        {
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
            transformedBounds.Width <= 0 || transformedBounds.Height <= 0)
        {
            return false;
        }

        *bounds = transformedBounds;
        return true;
    }

    double PrimaryStart(winrt::Windows::Foundation::Rect const &bounds,
                        TaskbarOrientation orientation)
    {
        return orientation == TaskbarOrientation::horizontal ? bounds.X
                                                             : bounds.Y;
    }

    double PrimarySize(winrt::Windows::Foundation::Rect const &bounds,
                       TaskbarOrientation orientation)
    {
        return orientation == TaskbarOrientation::horizontal ? bounds.Width
                                                             : bounds.Height;
    }

    double PrimaryCenter(winrt::Windows::Foundation::Rect const &bounds,
                         TaskbarOrientation orientation)
    {
        return PrimaryStart(bounds, orientation) +
               PrimarySize(bounds, orientation) / 2.0;
    }

    bool TryGetDividerGeometry(
        Controls::Canvas const &overlayCanvas,
        FrameworkElement const &previousIcon,
        FrameworkElement const &targetIcon,
        FrameworkElement const &nextIcon,
        TaskbarOrientation orientation,
        double rectangleWidth,
        double rectangleHeight,
        DividerGeometry *geometry)
    {
        if (!overlayCanvas || !targetIcon || !geometry ||
            !std::isfinite(rectangleWidth) || rectangleWidth <= 0 ||
            !std::isfinite(rectangleHeight) || rectangleHeight <= 0)
        {
            return false;
        }

        winrt::Windows::Foundation::Rect targetBounds{};
        winrt::Windows::Foundation::Rect nextBounds{};
        if (!TryGetIconBounds(overlayCanvas, targetIcon, &targetBounds))
        {
            return false;
        }

        if (nextIcon)
        {
            if (!TryGetIconBounds(overlayCanvas, nextIcon, &nextBounds))
            {
                return false;
            }
        }
        else
        {
            winrt::Windows::Foundation::Rect previousBounds{};
            if (!previousIcon ||
                !TryGetIconBounds(overlayCanvas, previousIcon,
                                  &previousBounds))
            {
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
                std::fabs(spacing) <= std::fabs(crossSpacing))
            {
                return false;
            }

            nextBounds = targetBounds;
            if (orientation == TaskbarOrientation::horizontal)
            {
                nextBounds.X = static_cast<float>(targetBounds.X + spacing);
            }
            else
            {
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
        if (nextCenter >= targetCenter)
        {
            double targetEnd = targetStart + targetSize;
            center = nextStart >= targetEnd
                         ? (targetEnd + nextStart) / 2.0
                         : (targetCenter + nextCenter) / 2.0;
        }
        else
        {
            double nextEnd = nextStart + nextSize;
            center = nextEnd <= targetStart
                         ? (targetStart + nextEnd) / 2.0
                         : (targetCenter + nextCenter) / 2.0;
        }

        double overlayWidth = overlayCanvas.ActualWidth();
        double overlayHeight = overlayCanvas.ActualHeight();
        if (!std::isfinite(overlayWidth) || overlayWidth <= 0 ||
            !std::isfinite(overlayHeight) || overlayHeight <= 0)
        {
            return false;
        }

        double left;
        double top;
        if (orientation == TaskbarOrientation::horizontal)
        {
            left = center - rectangleWidth / 2.0;
            top = (overlayHeight - rectangleHeight) / 2.0;
        }
        else
        {
            left = (overlayWidth - rectangleWidth) / 2.0;
            top = center - rectangleHeight / 2.0;
        }
        if (!std::isfinite(center) || !std::isfinite(left) ||
            !std::isfinite(top))
        {
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
        Controls::Canvas const &overlayCanvas,
        FrameworkElement const &firstIcon,
        FrameworkElement const &secondIcon,
        TaskbarOrientation orientation,
        double rectangleWidth,
        double rectangleHeight,
        DividerGeometry *geometry)
    {
        if (!overlayCanvas || !firstIcon || !geometry ||
            !std::isfinite(rectangleWidth) || rectangleWidth <= 0 ||
            !std::isfinite(rectangleHeight) || rectangleHeight <= 0)
        {
            return false;
        }

        winrt::Windows::Foundation::Rect firstBounds{};
        winrt::Windows::Foundation::Rect secondBounds{};
        if (!TryGetIconBounds(overlayCanvas, firstIcon, &firstBounds))
        {
            return false;
        }

        double firstCenter = PrimaryCenter(firstBounds, orientation);
        double firstCrossCenter =
            orientation == TaskbarOrientation::horizontal
                ? firstBounds.Y + firstBounds.Height / 2.0
                : firstBounds.X + firstBounds.Width / 2.0;
        double pitch = PrimarySize(firstBounds, orientation);
        double crossPitch = 0;
        if (secondIcon)
        {
            if (!TryGetIconBounds(overlayCanvas, secondIcon, &secondBounds))
            {
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
                std::fabs(pitch) <= std::fabs(crossPitch))
            {
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
        if (orientation == TaskbarOrientation::horizontal)
        {
            left = center - rectangleWidth / 2.0;
            top = crossCenter - rectangleHeight / 2.0;
        }
        else
        {
            left = crossCenter - rectangleWidth / 2.0;
            top = center - rectangleHeight / 2.0;
        }
        if (!std::isfinite(center) || !std::isfinite(left) ||
            !std::isfinite(top))
        {
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
        Controls::Canvas const &overlayCanvas,
        FrameworkElement const &previousIcon,
        FrameworkElement const &targetIcon,
        FrameworkElement const &nextIcon,
        TaskbarOrientation orientation,
        double rectangleWidth,
        double rectangleHeight,
        DividerGeometry *geometry)
    {
        if (!TryGetDividerGeometry(
                overlayCanvas, previousIcon, targetIcon, nextIcon, orientation,
                rectangleWidth, rectangleHeight, geometry))
        {
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

        if (orientation == TaskbarOrientation::horizontal)
        {
            double separatorCenterY = (targetCenterY + nextCenterY) / 2.0;
            geometry->top = separatorCenterY - rectangleHeight / 2.0;
        }
        else
        {
            double separatorCenterX = (targetCenterX + nextCenterX) / 2.0;
            geometry->left = separatorCenterX - rectangleWidth / 2.0;
        }

        return std::isfinite(geometry->left) && std::isfinite(geometry->top);
    }

    TrackedTaskbarState *FindTrackedTaskbarById(size_t taskbarId)
    {
        if (!g_trackedTaskbars)
        {
            return nullptr;
        }

        auto &trackedTaskbars = *g_trackedTaskbars;
        auto found = std::find_if(
            trackedTaskbars.begin(), trackedTaskbars.end(),
            [&](TrackedTaskbarState const &taskbar)
            {
                return taskbar.id == taskbarId;
            });
        return found != trackedTaskbars.end() ? &*found : nullptr;
    }

    void ClearAnimationElementCache(TrackedTaskbarState &taskbar)
    {
        taskbar.animationOverlayCanvas = {};
        taskbar.animationDividers.clear();
    }

    void InvalidateReconciliationSignature(TrackedTaskbarState &taskbar)
    {
        taskbar.nativeSettlingActive = false;
        taskbar.nativeSettlingStableFrames = 0;
        taskbar.nativeSettlingStarted = {};
        taskbar.reconciliationSignatureValid = false;
        taskbar.cachedReconcileResult =
            ReconcileResult::temporarilyNotReady;
        taskbar.reconciledRepeater = {};
        taskbar.reconciledRootGrid = {};
        taskbar.reconciledButtons.clear();
        taskbar.reconciledOverlayExpected = false;
        taskbar.reconciledOverlayChildCount = 0;
        taskbar.reconciledAnimationCompatibility = false;
        taskbar.reconciledSeparators.clear();
    }

    void UnsubscribeAnimationRendering(TrackedTaskbarState &taskbar,
                                       bool logStopped)
    {
        if (!taskbar.animationRenderingSubscribed)
        {
            return;
        }

        try
        {
            Media::CompositionTarget::Rendering(taskbar.animationRenderingToken);
        }
        catch (...)
        {
        }
        taskbar.animationRenderingToken = {};
        taskbar.animationRenderingSubscribed = false;
        for (auto &cache : taskbar.animationDividers)
        {
            cache.hasLastPosition = false;
        }
        taskbar.animationStableFrames = 0;
        taskbar.nativeSettlingStableFrames = 0;
        if (logStopped)
        {
            Wh_Log(L"GEOMETRY TRACKING: stopped");
        }
    }

    void StopAnimationTracking(TrackedTaskbarState &taskbar)
    {
        taskbar.animationLastActivity = {};
        taskbar.animationStableFrames = 0;
        if (!taskbar.nativeSettlingActive)
        {
            UnsubscribeAnimationRendering(taskbar, true);
        }
    }

    void StopNativeSettlingTracking(TrackedTaskbarState &taskbar)
    {
        taskbar.nativeSettlingActive = false;
        taskbar.nativeSettlingStableFrames = 0;
        taskbar.nativeSettlingStarted = {};
        if (taskbar.animationLastActivity == AnimationClock::time_point{})
        {
            UnsubscribeAnimationRendering(taskbar, true);
        }
    }

    void StopAllGeometryTracking(TrackedTaskbarState &taskbar)
    {
        taskbar.animationLastActivity = {};
        taskbar.animationStableFrames = 0;
        taskbar.nativeSettlingActive = false;
        taskbar.nativeSettlingStableFrames = 0;
        taskbar.nativeSettlingStarted = {};
        UnsubscribeAnimationRendering(taskbar, true);
    }

    bool RefreshCachedDividerGeometry(TrackedTaskbarState &taskbar,
                                      bool animatedCrossAxis,
                                      bool *allStable)
    {
        auto overlayCanvas = taskbar.animationOverlayCanvas.get();
        if (!overlayCanvas || taskbar.animationDividers.empty())
        {
            return false;
        }

        bool stable = true;
        for (auto &cache : taskbar.animationDividers)
        {
            auto host = cache.host.get();
            auto previousIcon = cache.previousIcon.get();
            auto targetIcon = cache.targetIcon.get();
            auto nextIcon = cache.nextIcon.get();
            if (!host || !targetIcon ||
                (!cache.beforeFirst && !nextIcon && !previousIcon))
            {
                return false;
            }

            DividerGeometry geometry;
            bool geometryValid =
                cache.beforeFirst
                    ? TryGetBeforeFirstDividerGeometry(
                          overlayCanvas, targetIcon, nextIcon,
                          cache.orientation, host.Width(), host.Height(),
                          &geometry)
                    : animatedCrossAxis
                          ? TryGetAnimatedDividerGeometry(
                                overlayCanvas, previousIcon, targetIcon,
                                nextIcon, cache.orientation, host.Width(),
                                host.Height(), &geometry)
                          : TryGetDividerGeometry(
                                overlayCanvas, previousIcon, targetIcon,
                                nextIcon, cache.orientation, host.Width(),
                                host.Height(), &geometry);
            if (!geometryValid)
            {
                return false;
            }

            constexpr double kWriteEpsilon = 0.01;
            double currentLeft = Controls::Canvas::GetLeft(host);
            double currentTop = Controls::Canvas::GetTop(host);
            if (!std::isfinite(currentLeft) ||
                std::fabs(currentLeft - geometry.left) > kWriteEpsilon)
            {
                Controls::Canvas::SetLeft(host, geometry.left);
            }
            if (!std::isfinite(currentTop) ||
                std::fabs(currentTop - geometry.top) > kWriteEpsilon)
            {
                Controls::Canvas::SetTop(host, geometry.top);
            }

            if (!cache.hasLastPosition ||
                std::fabs(cache.lastLeft - geometry.left) > kWriteEpsilon ||
                std::fabs(cache.lastTop - geometry.top) > kWriteEpsilon)
            {
                stable = false;
            }
            cache.lastLeft = geometry.left;
            cache.lastTop = geometry.top;
            cache.hasLastPosition = true;
        }

        if (allStable)
        {
            *allStable = stable;
        }
        return true;
    }

    void OnAnimationRendering(size_t taskbarId,
                              winrt::Windows::Foundation::IInspectable const &,
                              winrt::Windows::Foundation::IInspectable const &)
    {
        auto *taskbarState = FindTrackedTaskbarById(taskbarId);
        if (!taskbarState)
        {
            return;
        }
        auto &taskbar = *taskbarState;

        if (g_unloading)
        {
            StopAllGeometryTracking(taskbar);
            return;
        }

        auto now = AnimationClock::now();
        if (taskbar.animationLastActivity != AnimationClock::time_point{} &&
            now - taskbar.animationLastActivity >= kAnimationTrackingTimeout)
        {
            StopAnimationTracking(taskbar);
        }
        if (taskbar.nativeSettlingActive &&
            now - taskbar.nativeSettlingStarted >= kNativeSettlingTimeout)
        {
            StopNativeSettlingTracking(taskbar);
        }
        if (!taskbar.animationRenderingSubscribed)
        {
            return;
        }

        if (taskbar.animationRenderingCallbackActive)
        {
            return;
        }

        taskbar.animationRenderingCallbackActive = true;
        struct CallbackGuard
        {
            TrackedTaskbarState *taskbar;
            ~CallbackGuard()
            {
                taskbar->animationRenderingCallbackActive = false;
            }
        } callbackGuard{&taskbar};

        try
        {
            bool allStable = false;
            if (!RefreshCachedDividerGeometry(taskbar, true, &allStable))
            {
                Wh_Log(L"GEOMETRY TRACKING: cache invalid");
                ClearAnimationElementCache(taskbar);
                StopAllGeometryTracking(taskbar);
                return;
            }

            if (taskbar.nativeSettlingActive)
            {
                taskbar.nativeSettlingStableFrames =
                    allStable ? taskbar.nativeSettlingStableFrames + 1 : 0;
                if (taskbar.nativeSettlingStableFrames >=
                    kGeometryStableFrameThreshold)
                {
                    StopNativeSettlingTracking(taskbar);
                }
            }

            if (taskbar.animationLastActivity != AnimationClock::time_point{} &&
                taskbar.animationPointerInside)
            {
                taskbar.animationStableFrames = 0;
            }
            else if (taskbar.animationLastActivity !=
                     AnimationClock::time_point{})
            {
                taskbar.animationStableFrames =
                    allStable ? taskbar.animationStableFrames + 1 : 0;
                if (taskbar.animationStableFrames >=
                    kGeometryStableFrameThreshold)
                {
                    StopAnimationTracking(taskbar);
                }
            }
        }
        catch (...)
        {
            Wh_Log(L"GEOMETRY TRACKING: cache invalid");
            ClearAnimationElementCache(taskbar);
            StopAllGeometryTracking(taskbar);
        }
    }

    void EnsureGeometryRenderingSubscribed(TrackedTaskbarState &taskbar)
    {
        if (taskbar.animationRenderingSubscribed)
        {
            return;
        }

        size_t taskbarId = taskbar.id;
        taskbar.animationRenderingToken =
            Media::CompositionTarget::Rendering(
                [taskbarId](
                    winrt::Windows::Foundation::IInspectable const &sender,
                    winrt::Windows::Foundation::IInspectable const &args)
                {
                    OnAnimationRendering(taskbarId, sender, args);
                });
        taskbar.animationRenderingSubscribed = true;
        Wh_Log(L"GEOMETRY TRACKING: started");
    }

    void StartNativeSettlingTracking(TrackedTaskbarState &taskbar)
    {
        if (g_unloading || !taskbar.animationOverlayCanvas.get() ||
            taskbar.animationDividers.empty())
        {
            return;
        }

        taskbar.nativeSettlingActive = true;
        taskbar.nativeSettlingStarted = AnimationClock::now();
        taskbar.nativeSettlingStableFrames = 0;
        for (auto &cache : taskbar.animationDividers)
        {
            cache.hasLastPosition = false;
        }
        EnsureGeometryRenderingSubscribed(taskbar);
    }

    void StartAnimationTracking(TrackedTaskbarState &taskbar)
    {
        if (g_unloading || !taskbar.animationOverlayCanvas.get() ||
            taskbar.animationDividers.empty())
        {
            return;
        }

        taskbar.animationLastActivity = AnimationClock::now();

        for (auto &cache : taskbar.animationDividers)
        {
            cache.hasLastPosition = false;
        }
        taskbar.animationStableFrames = 0;
        EnsureGeometryRenderingSubscribed(taskbar);
    }

    void OnAnimationPointerMoved(
        size_t taskbarId,
        winrt::Windows::Foundation::IInspectable const &,
        Input::PointerRoutedEventArgs const &)
    {
        auto *taskbarState = FindTrackedTaskbarById(taskbarId);
        if (!taskbarState)
        {
            return;
        }
        auto &taskbar = *taskbarState;

        if (g_unloading)
        {
            return;
        }

        taskbar.animationPointerInside = true;
        taskbar.animationStableFrames = 0;
        StartAnimationTracking(taskbar);
    }

    void OnAnimationPointerExited(
        size_t taskbarId,
        winrt::Windows::Foundation::IInspectable const &,
        Input::PointerRoutedEventArgs const &)
    {
        auto *taskbarState = FindTrackedTaskbarById(taskbarId);
        if (!taskbarState)
        {
            return;
        }
        auto &taskbar = *taskbarState;

        if (g_unloading)
        {
            return;
        }

        taskbar.animationPointerInside = false;
        taskbar.animationStableFrames = 0;
        taskbar.animationLastActivity = AnimationClock::now();
    }

    void DetachAnimationPointerHandlers(TrackedTaskbarState &taskbar)
    {
        StopAnimationTracking(taskbar);

        if (taskbar.animationPointerHandlersAttached)
        {
            if (auto source = taskbar.animationPointerSource.get())
            {
                if (taskbar.animationPointerMovedHandler)
                {
                    try
                    {
                        source.RemoveHandler(UIElement::PointerMovedEvent(),
                                             taskbar.animationPointerMovedHandler);
                    }
                    catch (...)
                    {
                        // The source can be disconnected while Explorer rebuilds
                        // the taskbar. Clearing our delegate is sufficient then.
                    }
                }
                if (taskbar.animationPointerExitedHandlerAttached)
                {
                    try
                    {
                        source.PointerExited(taskbar.animationPointerExitedToken);
                    }
                    catch (...)
                    {
                        // The source can be disconnected while Explorer rebuilds
                        // the taskbar. Clearing our token is sufficient then.
                    }
                }
            }
        }

        taskbar.animationPointerMovedHandler = nullptr;
        taskbar.animationPointerExitedToken = {};
        taskbar.animationPointerSource = {};
        taskbar.animationPointerHandlersAttached = false;
        taskbar.animationPointerExitedHandlerAttached = false;
        taskbar.animationPointerInside = false;
        taskbar.animationLastActivity = {};
    }

    void AttachAnimationPointerHandlers(TrackedTaskbarState &taskbar,
                                        FrameworkElement const &source)
    {
        if (!source || g_unloading)
        {
            return;
        }

        if (taskbar.animationPointerHandlersAttached)
        {
            auto currentSource = taskbar.animationPointerSource.get();
            if (currentSource && winrt::get_abi(currentSource) ==
                                     winrt::get_abi(source))
            {
                return;
            }
            DetachAnimationPointerHandlers(taskbar);
        }

        try
        {
            taskbar.animationPointerSource = winrt::make_weak(source);
            size_t taskbarId = taskbar.id;
            taskbar.animationPointerMovedHandler = winrt::box_value(
                Input::PointerEventHandler{
                    [taskbarId](
                        winrt::Windows::Foundation::IInspectable const &sender,
                        Input::PointerRoutedEventArgs const &args)
                    {
                        OnAnimationPointerMoved(taskbarId, sender, args);
                    }});
            source.AddHandler(UIElement::PointerMovedEvent(),
                              taskbar.animationPointerMovedHandler, true);
            taskbar.animationPointerHandlersAttached = true;
            taskbar.animationPointerExitedToken = source.PointerExited(
                Input::PointerEventHandler{
                    [taskbarId](
                        winrt::Windows::Foundation::IInspectable const &sender,
                        Input::PointerRoutedEventArgs const &args)
                    {
                        OnAnimationPointerExited(taskbarId, sender, args);
                    }});
            taskbar.animationPointerExitedHandlerAttached = true;
        }
        catch (...)
        {
            DetachAnimationPointerHandlers(taskbar);
        }
    }

    void ClearAnimationTrackingForUnload()
    {
        try
        {
            CancelInitialReconcileRetry();
        }
        catch (...)
        {
        }

        if (!g_trackedTaskbars)
        {
            return;
        }

        for (auto &taskbar : *g_trackedTaskbars)
        {
            StopAllGeometryTracking(taskbar);
            try
            {
                DetachAnimationPointerHandlers(taskbar);
            }
            catch (...)
            {
            }
            ClearAnimationElementCache(taskbar);
        }
    }

    std::wstring GetDividerName(size_t settingsIndex)
    {
        return L"WindhawkTaskbarSeparator_" +
               std::to_wstring(settingsIndex);
    }

    std::wstring GetBeforeFirstDividerName()
    {
        return L"WindhawkTaskbarSeparator_BeforeFirst";
    }

    bool IsOwnedDividerName(std::wstring const &name)
    {
        constexpr std::wstring_view prefix = L"WindhawkTaskbarSeparator_";
        return name.size() >= prefix.size() &&
               name.compare(0, prefix.size(), prefix) == 0;
    }

    struct CleanupSweepResult
    {
        size_t overlaysRemoved = 0;
        size_t dividersRemoved = 0;
    };

    void SweepOwnedElementsFromPanel(Controls::Panel const &panel,
                                     CleanupSweepResult &result,
                                     int depth = 0)
    {
        if (!panel || depth > 24)
        {
            return;
        }

        try
        {
            auto children = panel.Children();
            for (uint32_t childIndex = 0; childIndex < children.Size();)
            {
                FrameworkElement child = nullptr;
                try
                {
                    child = children.GetAt(childIndex)
                                .try_as<FrameworkElement>();
                }
                catch (...)
                {
                    childIndex++;
                    continue;
                }

                if (!child)
                {
                    childIndex++;
                    continue;
                }

                bool ownedOverlay = false;
                bool ownedDivider = false;
                try
                {
                    std::wstring childName{child.Name()};
                    ownedOverlay =
                        childName == L"WindhawkTaskbarSeparatorOverlay";
                    ownedDivider = IsOwnedDividerName(childName);
                }
                catch (...)
                {
                    childIndex++;
                    continue;
                }

                if (ownedOverlay || ownedDivider)
                {
                    if (auto childPanel = child.try_as<Controls::Panel>())
                    {
                        // Release any owned divider hosts inside an overlay before
                        // detaching the overlay itself.
                        SweepOwnedElementsFromPanel(childPanel, result,
                                                    depth + 1);
                    }

                    try
                    {
                        children.RemoveAt(childIndex);
                        if (ownedOverlay)
                        {
                            result.overlaysRemoved++;
                        }
                        else
                        {
                            result.dividersRemoved++;
                        }
                        continue;
                    }
                    catch (...)
                    {
                        childIndex++;
                        continue;
                    }
                }

                if (auto childPanel = child.try_as<Controls::Panel>())
                {
                    SweepOwnedElementsFromPanel(childPanel, result, depth + 1);
                }
                childIndex++;
            }
        }
        catch (...)
        {
        }
    }

    Controls::Grid ResolveTrackedTaskbarRoot(TrackedTaskbarState &taskbar)
    {
        try
        {
            if (auto rootGrid = taskbar.rootGrid.get())
            {
                return rootGrid;
            }
        }
        catch (...)
        {
        }

        try
        {
            if (auto repeater = taskbar.repeater.get())
            {
                if (auto rootGrid = FindRootGridAncestor(repeater))
                {
                    return rootGrid;
                }
            }
        }
        catch (...)
        {
        }

        try
        {
            if (auto overlayCanvas = taskbar.overlayCanvas.get())
            {
                return Media::VisualTreeHelper::GetParent(overlayCanvas)
                    .try_as<Controls::Grid>();
            }
        }
        catch (...)
        {
        }

        return nullptr;
    }

    void RemoveTrackedOverlayByExactName(TrackedTaskbarState &taskbar,
                                         CleanupSweepResult &result)
    {
        try
        {
            auto overlayCanvas = taskbar.overlayCanvas.get();
            if (!overlayCanvas ||
                overlayCanvas.Name() != L"WindhawkTaskbarSeparatorOverlay")
            {
                return;
            }

            auto overlayParent =
                Media::VisualTreeHelper::GetParent(overlayCanvas)
                    .try_as<Controls::Panel>();
            if (!overlayParent)
            {
                return;
            }

            SweepOwnedElementsFromPanel(overlayCanvas, result);
            uint32_t overlayIndex = 0;
            auto parentChildren = overlayParent.Children();
            if (parentChildren.IndexOf(overlayCanvas, overlayIndex))
            {
                parentChildren.RemoveAt(overlayIndex);
                result.overlaysRemoved++;
            }
        }
        catch (...)
        {
        }
    }

    void RemoveTrackedTaskbarElements(TrackedTaskbarState &taskbar)
    {
        InvalidateReconciliationSignature(taskbar);
        StopAllGeometryTracking(taskbar);
        try
        {
            DetachAnimationPointerHandlers(taskbar);
        }
        catch (...)
        {
        }
        ClearAnimationElementCache(taskbar);

        CleanupSweepResult result;
        if (auto rootGrid = ResolveTrackedTaskbarRoot(taskbar))
        {
            SweepOwnedElementsFromPanel(rootGrid, result);
        }
        RemoveTrackedOverlayByExactName(taskbar, result);

        taskbar.overlayCanvas = {};
        taskbar.rootGrid = {};
    }

    void PruneExpiredTrackedTaskbars()
    {
        if (!g_trackedTaskbars)
        {
            return;
        }

        auto &trackedTaskbars = *g_trackedTaskbars;
        for (size_t index = 0; index < trackedTaskbars.size();)
        {
            bool taskbarIsLive = false;
            try
            {
                auto repeater = trackedTaskbars[index].repeater.get();
                taskbarIsLive = repeater && FindRootGridAncestor(repeater);
            }
            catch (...)
            {
            }

            if (taskbarIsLive)
            {
                index++;
                continue;
            }

            RemoveTrackedTaskbarElements(trackedTaskbars[index]);
            trackedTaskbars.erase(trackedTaskbars.begin() + index);
        }
    }

    TrackedTaskbarState *TrackTaskbarRepeater(
        FrameworkElement const &repeater)
    {
        if (!repeater)
        {
            return nullptr;
        }

        PruneExpiredTrackedTaskbars();
        auto &trackedTaskbars = GetTrackedTaskbars();
        for (auto &taskbar : trackedTaskbars)
        {
            auto trackedRepeater = taskbar.repeater.get();
            if (trackedRepeater &&
                winrt::get_abi(trackedRepeater) == winrt::get_abi(repeater))
            {
                return &taskbar;
            }
        }

        auto rootGrid = FindRootGridAncestor(repeater);
        if (rootGrid)
        {
            for (auto &taskbar : trackedTaskbars)
            {
                auto trackedRootGrid = taskbar.rootGrid.get();
                if (trackedRootGrid &&
                    winrt::get_abi(trackedRootGrid) ==
                        winrt::get_abi(rootGrid))
                {
                    auto trackedRepeater = taskbar.repeater.get();
                    if (!trackedRepeater ||
                        winrt::get_abi(trackedRepeater) !=
                            winrt::get_abi(repeater))
                    {
                        InvalidateReconciliationSignature(taskbar);
                    }
                    taskbar.repeater = winrt::make_weak(repeater);
                    return &taskbar;
                }
            }
        }

        TrackedTaskbarState taskbar;
        taskbar.id = g_nextTrackedTaskbarId++;
        taskbar.repeater = winrt::make_weak(repeater);
        if (rootGrid)
        {
            taskbar.rootGrid = winrt::make_weak(rootGrid);
        }
        trackedTaskbars.push_back(std::move(taskbar));
        return &trackedTaskbars.back();
    }

    size_t StyleChildCount(DividerStyle style)
    {
        if (style == DividerStyle::fade)
        {
            return 3;
        }
        return style == DividerStyle::glow ||
                       style == DividerStyle::doubleLine
                   ? 2
                   : 1;
    }

    PCWSTR ExpectedStyleChildName(DividerStyle style, uint32_t index)
    {
        if (style == DividerStyle::fade)
        {
            constexpr PCWSTR names[] = {
                L"DividerFadeOuter", L"DividerFadeMiddle", L"DividerFadeCore"};
            return names[index];
        }
        if (style == DividerStyle::glow)
        {
            return index == 0 ? L"DividerGlow" : L"DividerMain";
        }
        if (style == DividerStyle::doubleLine)
        {
            return index == 0 ? L"DividerLine1" : L"DividerLine2";
        }
        return L"DividerMain";
    }

    Shapes::Rectangle AppendStyleRectangle(Controls::Canvas const &host,
                                           PCWSTR name,
                                           int zIndex)
    {
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

    void RebuildStyleChildren(Controls::Canvas const &host,
                              DividerStyle style)
    {
        host.Children().Clear();
        switch (style)
        {
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

    bool StyleChildrenValid(Controls::Canvas const &host,
                            DividerStyle style)
    {
        auto children = host.Children();
        if (children.Size() != StyleChildCount(style))
        {
            return false;
        }

        for (uint32_t index = 0; index < children.Size(); index++)
        {
            auto rectangle =
                children.GetAt(index).try_as<Shapes::Rectangle>();
            if (!rectangle ||
                rectangle.Name() != ExpectedStyleChildName(style, index))
            {
                return false;
            }
        }

        return winrt::unbox_value_or<int32_t>(
                   host.Tag(), -1) == static_cast<int32_t>(style);
    }

    void SetRectangleBounds(Shapes::Rectangle const &rectangle,
                            double left,
                            double top,
                            double width,
                            double height)
    {
        if (rectangle.IsHitTestVisible())
        {
            rectangle.IsHitTestVisible(false);
        }
        if (!rectangle.UseLayoutRounding())
        {
            rectangle.UseLayoutRounding(true);
        }
        if (rectangle.Width() != width)
        {
            rectangle.Width(width);
        }
        if (rectangle.Height() != height)
        {
            rectangle.Height(height);
        }
        if (Controls::Canvas::GetLeft(rectangle) != left)
        {
            Controls::Canvas::SetLeft(rectangle, left);
        }
        if (Controls::Canvas::GetTop(rectangle) != top)
        {
            Controls::Canvas::SetTop(rectangle, top);
        }
        if (rectangle.Opacity() != 1)
        {
            rectangle.Opacity(1);
        }
    }

    void SetRectangleRadius(Shapes::Rectangle const &rectangle, double radius)
    {
        if (rectangle.RadiusX() != radius)
        {
            rectangle.RadiusX(radius);
        }
        if (rectangle.RadiusY() != radius)
        {
            rectangle.RadiusY(radius);
        }
    }

    void SetSolidFill(Shapes::Rectangle const &rectangle,
                      winrt::Windows::UI::Color color)
    {
        auto brush = rectangle.Fill().try_as<Media::SolidColorBrush>();
        if (!brush)
        {
            brush = Media::SolidColorBrush();
            rectangle.Fill(brush);
        }
        if (!ColorsEqual(brush.Color(), color))
        {
            brush.Color(color);
        }
    }

    enum class FadeLayer
    {
        outer,
        middle,
        core,
    };

    void SetFadeFill(Shapes::Rectangle const &rectangle,
                     winrt::Windows::UI::Color color,
                     int fadeAmount,
                     TaskbarOrientation orientation,
                     FadeLayer layer)
    {
        auto brush = rectangle.Fill().try_as<Media::LinearGradientBrush>();
        if (!brush)
        {
            brush = Media::LinearGradientBrush();
            rectangle.Fill(brush);
        }

        if (orientation == TaskbarOrientation::horizontal)
        {
            brush.StartPoint({0.5f, 0.0f});
            brush.EndPoint({0.5f, 1.0f});
        }
        else
        {
            brush.StartPoint({0.0f, 0.5f});
            brush.EndPoint({1.0f, 0.5f});
        }

        auto stops = brush.GradientStops();
        constexpr uint32_t kFadeStopCount = 7;
        if (stops.Size() != kFadeStopCount)
        {
            stops.Clear();
            for (uint32_t index = 0; index < kFadeStopCount; index++)
            {
                Media::GradientStop stop;
                stops.Append(stop);
            }
        }

        double lowOpacityBaseOffset = 0.10;
        double highOpacityBaseOffset = 0.25;
        if (layer == FadeLayer::middle)
        {
            lowOpacityBaseOffset = 0.18;
            highOpacityBaseOffset = 0.36;
        }
        else if (layer == FadeLayer::outer)
        {
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

        for (uint32_t index = 0; index < kFadeStopCount; index++)
        {
            auto stopColor = color;
            stopColor.A = static_cast<uint8_t>(std::lround(
                color.A * opacityFactors[index]));
            auto stop = stops.GetAt(index);
            if (!ColorsEqual(stop.Color(), stopColor))
            {
                stop.Color(stopColor);
            }
            if (stop.Offset() != offsets[index])
            {
                stop.Offset(offsets[index]);
            }
        }
    }

    void GetStyleHostSize(Settings const &settings,
                          TaskbarOrientation orientation,
                          double *width,
                          double *height)
    {
        double baseWidth = orientation == TaskbarOrientation::horizontal
                               ? settings.width
                               : settings.height;
        double baseHeight = orientation == TaskbarOrientation::horizontal
                                ? settings.height
                                : settings.width;
        *width = baseWidth;
        *height = baseHeight;

        if (settings.style == DividerStyle::fade)
        {
            double outerThickness =
                std::max<double>(settings.width + 4, 5);
            if (orientation == TaskbarOrientation::horizontal)
            {
                *width = outerThickness;
            }
            else
            {
                *height = outerThickness;
            }
        }
        else if (settings.style == DividerStyle::glow)
        {
            *width += settings.glowSize * 2.0;
            *height += settings.glowSize * 2.0;
        }
        else if (settings.style == DividerStyle::doubleLine)
        {
            double doubleThickness = settings.width * 2.0 + settings.doubleGap;
            if (orientation == TaskbarOrientation::horizontal)
            {
                *width = doubleThickness;
            }
            else
            {
                *height = doubleThickness;
            }
        }
    }

    void ConfigureStyleHost(Controls::Canvas const &host,
                            Settings const &settings,
                            TaskbarOrientation orientation,
                            double hostWidth,
                            double hostHeight)
    {
        if (!StyleChildrenValid(host, settings.style))
        {
            RebuildStyleChildren(host, settings.style);
        }

        auto children = host.Children();
        double baseWidth = orientation == TaskbarOrientation::horizontal
                               ? settings.width
                               : settings.height;
        double baseHeight = orientation == TaskbarOrientation::horizontal
                                ? settings.height
                                : settings.width;

        if (settings.style == DividerStyle::fade)
        {
            auto outer = children.GetAt(0).as<Shapes::Rectangle>();
            auto middle = children.GetAt(1).as<Shapes::Rectangle>();
            auto core = children.GetAt(2).as<Shapes::Rectangle>();
            double outerThickness =
                std::max<double>(settings.width + 4, 5);
            double middleThickness =
                std::max<double>(settings.width + 2, 3);

            if (orientation == TaskbarOrientation::horizontal)
            {
                SetRectangleBounds(outer,
                                   (hostWidth - outerThickness) / 2.0, 0,
                                   outerThickness, baseHeight);
                SetRectangleBounds(middle,
                                   (hostWidth - middleThickness) / 2.0, 0,
                                   middleThickness, baseHeight);
                SetRectangleBounds(core,
                                   (hostWidth - settings.width) / 2.0, 0,
                                   settings.width, baseHeight);
            }
            else
            {
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

        if (settings.style == DividerStyle::glow)
        {
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

        if (settings.style == DividerStyle::doubleLine)
        {
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
        Controls::Canvas const &overlayCanvas,
        std::vector<FrameworkElement> const &icons,
        TaskbarOrientation *orientation)
    {
        if (!orientation)
        {
            return false;
        }

        if (orientationSetting == OrientationSetting::horizontal)
        {
            *orientation = TaskbarOrientation::horizontal;
            return true;
        }
        if (orientationSetting == OrientationSetting::vertical)
        {
            *orientation = TaskbarOrientation::vertical;
            return true;
        }

        double totalHorizontalMovement = 0;
        double totalVerticalMovement = 0;
        bool foundPair = false;
        for (size_t index = 1; index < icons.size(); index++)
        {
            winrt::Windows::Foundation::Rect previousBounds{};
            winrt::Windows::Foundation::Rect currentBounds{};
            if (!icons[index - 1] || !icons[index] ||
                !TryGetIconBounds(overlayCanvas, icons[index - 1],
                                  &previousBounds) ||
                !TryGetIconBounds(overlayCanvas, icons[index],
                                  &currentBounds))
            {
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

        if (!foundPair)
        {
            double width = overlayCanvas.ActualWidth();
            double height = overlayCanvas.ActualHeight();
            if (!std::isfinite(width) || !std::isfinite(height) || width < 0 ||
                height < 0)
            {
                return false;
            }

            *orientation = width >= height ? TaskbarOrientation::horizontal
                                           : TaskbarOrientation::vertical;
            return true;
        }

        if (std::fabs(totalHorizontalMovement - totalVerticalMovement) <= 0.1)
        {
            return false;
        }

        *orientation = totalHorizontalMovement > totalVerticalMovement
                           ? TaskbarOrientation::horizontal
                           : TaskbarOrientation::vertical;
        return true;
    }

    struct RealizedButtonSnapshot
    {
        int itemIndex = -1;
        FrameworkElement button{nullptr};
        FrameworkElement iconPanel{nullptr};
        FrameworkElement icon{nullptr};
        double actualWidth = 0;
        double actualHeight = 0;
        Thickness margin{};
        Visibility visibility = Visibility::Visible;
    };

    struct TaskbarReconciliationSnapshot
    {
        bool signatureValid = false;
        Controls::Grid rootGrid{nullptr};
        double rootWidth = 0;
        double rootHeight = 0;
        winrt::Windows::Foundation::Rect repeaterBounds{};
        std::vector<RealizedButtonSnapshot> buttons;
    };

    bool IsFiniteThickness(Thickness const &thickness)
    {
        return std::isfinite(thickness.Left) &&
               std::isfinite(thickness.Top) &&
               std::isfinite(thickness.Right) &&
               std::isfinite(thickness.Bottom);
    }

    bool LayoutScalarMatches(double left, double right)
    {
        constexpr double kLayoutEpsilon = 0.01;
        return std::isfinite(left) && std::isfinite(right) &&
               std::fabs(left - right) <= kLayoutEpsilon;
    }

    bool LayoutRectMatches(
        winrt::Windows::Foundation::Rect const &left,
        winrt::Windows::Foundation::Rect const &right)
    {
        return LayoutScalarMatches(left.X, right.X) &&
               LayoutScalarMatches(left.Y, right.Y) &&
               LayoutScalarMatches(left.Width, right.Width) &&
               LayoutScalarMatches(left.Height, right.Height);
    }

    bool LayoutThicknessMatches(Thickness const &left,
                                Thickness const &right)
    {
        return LayoutScalarMatches(left.Left, right.Left) &&
               LayoutScalarMatches(left.Top, right.Top) &&
               LayoutScalarMatches(left.Right, right.Right) &&
               LayoutScalarMatches(left.Bottom, right.Bottom);
    }

    TaskbarReconciliationSnapshot CaptureTaskbarReconciliationSnapshot(
        FrameworkElement const &repeater)
    {
        TaskbarReconciliationSnapshot snapshot;
        auto repeaterPanel = repeater.try_as<Controls::Panel>();
        if (!repeaterPanel)
        {
            return snapshot;
        }

        struct RealizedElement
        {
            int itemIndex;
            FrameworkElement element;
        };

        std::vector<RealizedElement> realizedElements;
        for (auto const &panelChild : repeaterPanel.Children())
        {
            auto child = panelChild.try_as<FrameworkElement>();
            if (!child)
            {
                continue;
            }

            int itemIndex = ItemsRepeater_GetElementIndex(repeater, child);
            if (itemIndex >= 0)
            {
                realizedElements.push_back({itemIndex, child});
            }
        }

        std::sort(realizedElements.begin(), realizedElements.end(),
                  [](RealizedElement const &left,
                     RealizedElement const &right)
                  {
                      return left.itemIndex < right.itemIndex;
                  });

        bool buttonLayoutValid = true;
        for (auto const &realized : realizedElements)
        {
            if (realized.element.Name() != L"TaskListButton")
            {
                continue;
            }

            RealizedButtonSnapshot button;
            button.itemIndex = realized.itemIndex;
            button.button = realized.element;
            button.actualWidth = realized.element.ActualWidth();
            button.actualHeight = realized.element.ActualHeight();
            button.margin = realized.element.Margin();
            button.visibility = realized.element.Visibility();
            if (!std::isfinite(button.actualWidth) ||
                !std::isfinite(button.actualHeight) ||
                button.actualWidth < 0 || button.actualHeight < 0 ||
                !IsFiniteThickness(button.margin))
            {
                buttonLayoutValid = false;
            }
            snapshot.buttons.push_back(std::move(button));
        }

        snapshot.rootGrid = FindRootGridAncestor(repeater);
        if (!snapshot.rootGrid)
        {
            return snapshot;
        }

        snapshot.rootWidth = snapshot.rootGrid.ActualWidth();
        snapshot.rootHeight = snapshot.rootGrid.ActualHeight();
        double repeaterWidth = repeater.ActualWidth();
        double repeaterHeight = repeater.ActualHeight();
        if (!buttonLayoutValid || !std::isfinite(snapshot.rootWidth) ||
            !std::isfinite(snapshot.rootHeight) ||
            !std::isfinite(repeaterWidth) ||
            !std::isfinite(repeaterHeight) || snapshot.rootWidth <= 0 ||
            snapshot.rootHeight <= 0 || repeaterWidth < 0 ||
            repeaterHeight < 0)
        {
            return snapshot;
        }

        snapshot.repeaterBounds =
            repeater.TransformToVisual(snapshot.rootGrid)
                .TransformBounds(winrt::Windows::Foundation::Rect{
                    0, 0, static_cast<float>(repeaterWidth),
                    static_cast<float>(repeaterHeight)});
        if (!std::isfinite(snapshot.repeaterBounds.X) ||
            !std::isfinite(snapshot.repeaterBounds.Y) ||
            !std::isfinite(snapshot.repeaterBounds.Width) ||
            !std::isfinite(snapshot.repeaterBounds.Height))
        {
            return snapshot;
        }

        snapshot.signatureValid = true;
        return snapshot;
    }

    bool CachedSeparatorVisualsAreValid(
        TrackedTaskbarState &taskbar,
        Controls::Grid const &rootGrid)
    {
        if (!taskbar.reconciledOverlayExpected)
        {
            return taskbar.reconciledSeparators.empty() &&
                   !taskbar.overlayCanvas.get();
        }

        if (taskbar.reconciledAnimationCompatibility)
        {
            auto pointerSource = taskbar.animationPointerSource.get();
            if (!taskbar.animationPointerHandlersAttached ||
                !taskbar.animationPointerExitedHandlerAttached ||
                !taskbar.animationPointerMovedHandler || !pointerSource ||
                winrt::get_abi(pointerSource) != winrt::get_abi(rootGrid))
            {
                return false;
            }
        }

        auto overlayCanvas = taskbar.overlayCanvas.get();
        if (!overlayCanvas ||
            overlayCanvas.Name() != L"WindhawkTaskbarSeparatorOverlay" ||
            overlayCanvas.Children().Size() !=
                taskbar.reconciledOverlayChildCount)
        {
            return false;
        }

        auto overlayParent =
            Media::VisualTreeHelper::GetParent(overlayCanvas)
                .try_as<Controls::Grid>();
        if (!overlayParent ||
            winrt::get_abi(overlayParent) != winrt::get_abi(rootGrid))
        {
            return false;
        }

        if (taskbar.reconciledOverlayExpected)
        {
            auto animationOverlay = taskbar.animationOverlayCanvas.get();
            if (!animationOverlay ||
                winrt::get_abi(animationOverlay) !=
                    winrt::get_abi(overlayCanvas) ||
                taskbar.animationDividers.size() !=
                    taskbar.reconciledSeparators.size())
            {
                return false;
            }

            for (auto const &animationDivider : taskbar.animationDividers)
            {
                if (!animationDivider.host.get() ||
                    !animationDivider.targetIcon.get() ||
                    (!animationDivider.beforeFirst &&
                     !animationDivider.previousIcon.get() &&
                     !animationDivider.nextIcon.get()))
                {
                    return false;
                }
            }
        }

        for (auto const &separator : taskbar.reconciledSeparators)
        {
            auto host = separator.host.get();
            if (!host || host.Name() != separator.name)
            {
                return false;
            }

            auto hostParent = Media::VisualTreeHelper::GetParent(host)
                                  .try_as<Controls::Canvas>();
            if (!hostParent ||
                winrt::get_abi(hostParent) != winrt::get_abi(overlayCanvas))
            {
                return false;
            }
        }

        return true;
    }

    bool IsVisualDescendantOf(FrameworkElement const &descendant,
                              FrameworkElement const &ancestor)
    {
        auto current = descendant;
        for (int depth = 0; depth < 16 && current; depth++)
        {
            if (winrt::get_abi(current) == winrt::get_abi(ancestor))
            {
                return true;
            }
            current = Media::VisualTreeHelper::GetParent(current)
                          .try_as<FrameworkElement>();
        }
        return false;
    }

    bool CanReuseReconciledTaskbar(
        TrackedTaskbarState &taskbar,
        FrameworkElement const &repeater,
        TaskbarReconciliationSnapshot const &snapshot,
        unsigned int settingsGeneration)
    {
        if (!taskbar.reconciliationSignatureValid ||
            !snapshot.signatureValid ||
            taskbar.appliedSettingsGeneration != settingsGeneration)
        {
            return false;
        }

        auto reconciledRepeater = taskbar.reconciledRepeater.get();
        auto reconciledRootGrid = taskbar.reconciledRootGrid.get();
        if (!reconciledRepeater || !reconciledRootGrid ||
            winrt::get_abi(reconciledRepeater) != winrt::get_abi(repeater) ||
            winrt::get_abi(reconciledRootGrid) !=
                winrt::get_abi(snapshot.rootGrid) ||
            !LayoutScalarMatches(taskbar.reconciledRootWidth,
                                 snapshot.rootWidth) ||
            !LayoutScalarMatches(taskbar.reconciledRootHeight,
                                 snapshot.rootHeight) ||
            !LayoutRectMatches(taskbar.reconciledRepeaterBounds,
                               snapshot.repeaterBounds) ||
            taskbar.reconciledButtons.size() != snapshot.buttons.size())
        {
            return false;
        }

        for (size_t index = 0; index < snapshot.buttons.size(); index++)
        {
            auto const &cached = taskbar.reconciledButtons[index];
            auto const &current = snapshot.buttons[index];
            auto cachedButton = cached.button.get();
            if (!cachedButton ||
                winrt::get_abi(cachedButton) !=
                    winrt::get_abi(current.button) ||
                cached.itemIndex != current.itemIndex ||
                !LayoutScalarMatches(cached.actualWidth,
                                     current.actualWidth) ||
                !LayoutScalarMatches(cached.actualHeight,
                                     current.actualHeight) ||
                !LayoutThicknessMatches(cached.margin, current.margin) ||
                cached.visibility != current.visibility)
            {
                return false;
            }

            if (cached.iconResolved)
            {
                auto cachedIconPanel = cached.iconPanel.get();
                auto cachedIcon = cached.icon.get();
                if (!cachedIconPanel || !cachedIcon ||
                    cachedIconPanel.Name() != L"IconPanel" ||
                    cachedIcon.Name() != L"Icon" ||
                    !IsVisualDescendantOf(cachedIconPanel, current.button) ||
                    !IsVisualDescendantOf(cachedIcon, cachedIconPanel))
                {
                    return false;
                }
            }
            else
            {
                auto currentIconPanel =
                    FindDescendantByName(current.button, L"IconPanel")
                        .try_as<Controls::Panel>();
                if (currentIconPanel &&
                    FindDescendantByName(currentIconPanel, L"Icon"))
                {
                    // A previously unrealized button is now placeable.
                    return false;
                }
            }
        }

        return CachedSeparatorVisualsAreValid(taskbar, snapshot.rootGrid);
    }

    void CommitReconciledTaskbar(
        TrackedTaskbarState &taskbar,
        FrameworkElement const &repeater,
        TaskbarReconciliationSnapshot const &snapshot,
        ReconcileResult result,
        bool overlayExpected,
        uint32_t overlayChildCount,
        bool animationCompatibility,
        std::vector<ReconciledSeparatorVisual> &&separators)
    {
        if (!snapshot.signatureValid ||
            result == ReconcileResult::temporarilyNotReady)
        {
            return;
        }

        taskbar.cachedReconcileResult = result;
        taskbar.reconciledRepeater = winrt::make_weak(repeater);
        taskbar.reconciledRootGrid = winrt::make_weak(snapshot.rootGrid);
        taskbar.reconciledRootWidth = snapshot.rootWidth;
        taskbar.reconciledRootHeight = snapshot.rootHeight;
        taskbar.reconciledRepeaterBounds = snapshot.repeaterBounds;
        taskbar.reconciledButtons.clear();
        taskbar.reconciledButtons.reserve(snapshot.buttons.size());
        for (auto const &button : snapshot.buttons)
        {
            bool iconResolved = button.iconPanel && button.icon;
            taskbar.reconciledButtons.push_back(
                {button.itemIndex, winrt::make_weak(button.button),
                 button.iconPanel ? winrt::make_weak(button.iconPanel)
                                  : winrt::weak_ref<FrameworkElement>{},
                 button.icon ? winrt::make_weak(button.icon)
                             : winrt::weak_ref<FrameworkElement>{},
                 iconResolved, button.actualWidth,
                 button.actualHeight, button.margin, button.visibility});
        }
        taskbar.reconciledOverlayExpected = overlayExpected;
        taskbar.reconciledOverlayChildCount = overlayChildCount;
        taskbar.reconciledAnimationCompatibility = animationCompatibility;
        taskbar.reconciledSeparators = std::move(separators);
        taskbar.reconciliationSignatureValid = true;
    }

    // This is the only overlay function that mutates the taskbar XAML visual tree.
    // Callers must already be running on the taskbar XAML/UI thread.
    ReconcileResult ReconcileTrackedTaskbar(
        TrackedTaskbarState &taskbar,
        FrameworkElement const &repeater,
        bool forceStructuralReconcile)
    {
        ReconcileResult result = ReconcileResult::temporarilyNotReady;

        try
        {
            if (repeater && repeater.Dispatcher().HasThreadAccess())
            {
                if (forceStructuralReconcile)
                {
                    InvalidateReconciliationSignature(taskbar);
                }

                auto snapshot =
                    CaptureTaskbarReconciliationSnapshot(repeater);
                unsigned int currentSettingsGeneration =
                    g_settingsGeneration.load(std::memory_order_acquire);
                // UpdateVisualStates is a hot path. A matching weak/scalar
                // snapshot proves that structural work is unchanged; active
                // animation geometry remains the Rendering callback's job.
                if (!forceStructuralReconcile &&
                    CanReuseReconciledTaskbar(
                        taskbar, repeater, snapshot,
                        currentSettingsGeneration))
                {
                    if (taskbar.reconciledAnimationCompatibility &&
                        taskbar.reconciledOverlayExpected)
                    {
                        // UpdateVisualStates can be raised by non-pointer
                        // composition changes. Reuse the existing bounded
                        // Rendering refresh without repeating structural work.
                        StartAnimationTracking(taskbar);
                    }
                    return taskbar.cachedReconcileResult;
                }
                InvalidateReconciliationSignature(taskbar);

                Settings settings = GetSettingsSnapshot();
                unsigned int settingsGeneration =
                    g_settingsGeneration.load(std::memory_order_acquire);
                bool settingsChanged =
                    taskbar.appliedSettingsGeneration != settingsGeneration;

                if (!settings.animationCompatibility)
                {
                    StopAnimationTracking(taskbar);
                    DetachAnimationPointerHandlers(taskbar);
                    ClearAnimationElementCache(taskbar);
                }

                std::vector<FrameworkElement> appButtons;
                appButtons.reserve(snapshot.buttons.size());
                for (auto const &button : snapshot.buttons)
                {
                    appButtons.push_back(button.button);
                }

                std::vector<FrameworkElement> appIcons;
                appIcons.reserve(appButtons.size());
                for (size_t index = 0; index < appButtons.size(); index++)
                {
                    appIcons.push_back(nullptr);
                }
                for (size_t buttonIndex = 0; buttonIndex < appButtons.size();
                     buttonIndex++)
                {
                    auto iconPanel =
                        FindDescendantByName(appButtons[buttonIndex], L"IconPanel")
                            .try_as<Controls::Panel>();
                    if (!iconPanel)
                    {
                        continue;
                    }

                    snapshot.buttons[buttonIndex].iconPanel = iconPanel;
                    appIcons[buttonIndex] =
                        FindDescendantByName(iconPanel, L"Icon");
                    snapshot.buttons[buttonIndex].icon =
                        appIcons[buttonIndex];
                }

                struct ActiveSeparator
                {
                    SeparatorSettings settings;
                    std::wstring name;
                    bool beforeFirst = false;
                };

                std::vector<ActiveSeparator> activeSeparators;
                std::vector<int> usedPositions;
                if (settings.separatorBeforeFirstApp)
                {
                    activeSeparators.push_back(
                        {SeparatorSettings{}, GetBeforeFirstDividerName(),
                         true});
                }
                for (auto const &separator : settings.separators)
                {
                    if (std::find(usedPositions.begin(), usedPositions.end(),
                                  separator.position) != usedPositions.end())
                    {
                        continue;
                    }

                    usedPositions.push_back(separator.position);
                    activeSeparators.push_back(
                        {separator,
                         GetDividerName(separator.settingsIndex), false});
                }

                auto rootGrid = snapshot.rootGrid;
                auto trackedRootGrid = taskbar.rootGrid.get();
                if (trackedRootGrid &&
                    (!rootGrid ||
                     winrt::get_abi(trackedRootGrid) !=
                         winrt::get_abi(rootGrid)))
                {
                    RemoveTrackedTaskbarElements(taskbar);
                }
                if (rootGrid)
                {
                    taskbar.rootGrid = winrt::make_weak(rootGrid);
                }

                Controls::Canvas overlayCanvas = nullptr;
                bool needOverlay = !activeSeparators.empty();

                if (rootGrid)
                {
                    auto rootChildren = rootGrid.Children();
                    for (uint32_t childIndex = 0;
                         childIndex < rootChildren.Size();)
                    {
                        auto child = rootChildren.GetAt(childIndex)
                                         .try_as<FrameworkElement>();
                        if (!child)
                        {
                            childIndex++;
                            continue;
                        }

                        if (child.Name() ==
                            L"WindhawkTaskbarSeparatorOverlay")
                        {
                            auto canvas = child.try_as<Controls::Canvas>();
                            if (needOverlay && canvas && !overlayCanvas)
                            {
                                overlayCanvas = canvas;
                                childIndex++;
                            }
                            else
                            {
                                rootChildren.RemoveAt(childIndex);
                            }
                            continue;
                        }

                        std::wstring childName{child.Name()};
                        if (IsOwnedDividerName(childName))
                        {
                            rootChildren.RemoveAt(childIndex);
                            continue;
                        }

                        childIndex++;
                    }

                    if (needOverlay && !overlayCanvas)
                    {
                        Controls::Canvas overlay;
                        overlay.Name(L"WindhawkTaskbarSeparatorOverlay");
                        overlay.HorizontalAlignment(HorizontalAlignment::Stretch);
                        overlay.VerticalAlignment(VerticalAlignment::Stretch);
                        overlay.IsHitTestVisible(false);
                        Controls::Canvas::SetZIndex(overlay, 1000);
                        rootChildren.Append(overlay);
                        overlayCanvas = overlay;
                    }

                    if (overlayCanvas)
                    {
                        if (overlayCanvas.HorizontalAlignment() !=
                            HorizontalAlignment::Stretch)
                        {
                            overlayCanvas.HorizontalAlignment(
                                HorizontalAlignment::Stretch);
                        }
                        if (overlayCanvas.VerticalAlignment() !=
                            VerticalAlignment::Stretch)
                        {
                            overlayCanvas.VerticalAlignment(
                                VerticalAlignment::Stretch);
                        }
                        if (overlayCanvas.IsHitTestVisible())
                        {
                            overlayCanvas.IsHitTestVisible(false);
                        }
                        if (Controls::Canvas::GetZIndex(overlayCanvas) != 1000)
                        {
                            Controls::Canvas::SetZIndex(overlayCanvas, 1000);
                        }
                    }
                }

                if (overlayCanvas)
                {
                    taskbar.overlayCanvas = winrt::make_weak(overlayCanvas);
                }
                else
                {
                    taskbar.overlayCanvas = {};
                }

                if (settings.animationCompatibility && needOverlay && rootGrid)
                {
                    AttachAnimationPointerHandlers(taskbar, rootGrid);
                }
                else
                {
                    DetachAnimationPointerHandlers(taskbar);
                }

                std::vector<Controls::Canvas> existingHosts;
                existingHosts.reserve(activeSeparators.size());
                for (size_t index = 0; index < activeSeparators.size(); index++)
                {
                    existingHosts.push_back(nullptr);
                }
                if (overlayCanvas)
                {
                    auto overlayChildren = overlayCanvas.Children();
                    for (uint32_t childIndex = 0;
                         childIndex < overlayChildren.Size();)
                    {
                        auto child = overlayChildren.GetAt(childIndex)
                                         .try_as<FrameworkElement>();
                        if (!child)
                        {
                            childIndex++;
                            continue;
                        }

                        std::wstring childName{child.Name()};
                        if (!IsOwnedDividerName(childName))
                        {
                            childIndex++;
                            continue;
                        }

                        auto desired = std::find_if(
                            activeSeparators.begin(), activeSeparators.end(),
                            [&](ActiveSeparator const &separator)
                            {
                                return separator.name == childName;
                            });
                        auto host = child.try_as<Controls::Canvas>();
                        if (desired == activeSeparators.end() || !host)
                        {
                            overlayChildren.RemoveAt(childIndex);
                            continue;
                        }

                        size_t desiredIndex =
                            static_cast<size_t>(
                                desired - activeSeparators.begin());
                        if (existingHosts[desiredIndex])
                        {
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
                std::vector<ReconciledSeparatorVisual>
                    reconciledSeparators;
                reconciledSeparators.reserve(activeSeparators.size());
                size_t activeDividerCount = 0;
                bool geometryFailureWasTransient = false;
                for (size_t activeIndex = 0;
                     activeIndex < activeSeparators.size(); activeIndex++)
                {
                    auto const &activeSeparator =
                        activeSeparators[activeIndex];
                    auto const &separator = activeSeparator.settings;

                    FrameworkElement previousIcon = nullptr;
                    FrameworkElement targetIcon = nullptr;
                    FrameworkElement nextIcon = nullptr;
                    if (activeSeparator.beforeFirst)
                    {
                        if (!appIcons.empty())
                        {
                            targetIcon = appIcons[0];
                            if (appIcons.size() >= 2)
                            {
                                nextIcon = appIcons[1];
                            }
                        }
                    }
                    else
                    {
                        size_t buttonIndex =
                            static_cast<size_t>(separator.position - 1);
                        if (buttonIndex < appIcons.size())
                        {
                            targetIcon = appIcons[buttonIndex];
                            if (buttonIndex > 0)
                            {
                                previousIcon = appIcons[buttonIndex - 1];
                            }
                            if (buttonIndex + 1 < appIcons.size())
                            {
                                nextIcon = appIcons[buttonIndex + 1];
                            }
                        }
                    }

                    double hostWidth = 0;
                    double hostHeight = 0;
                    GetStyleHostSize(settings, taskbarOrientation, &hostWidth,
                                     &hostHeight);
                    bool geometryExpectedFromCurrentButtons =
                        activeSeparator.beforeFirst
                            ? targetIcon && nextIcon
                            : targetIcon && (nextIcon || previousIcon);
                    DividerGeometry geometry;
                    bool geometryValid = false;
                    if (orientationValid && targetIcon)
                    {
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
                    if (!geometryValid)
                    {
                        if (geometryExpectedFromCurrentButtons)
                        {
                            // The required realized icons exist, so a failed
                            // orientation/transform indicates a rebuilding or
                            // otherwise transient visual tree.
                            geometryFailureWasTransient = true;
                        }
                        auto staleHost = existingHosts[activeIndex];
                        if (staleHost && overlayCanvas)
                        {
                            uint32_t staleIndex = 0;
                            auto overlayChildren = overlayCanvas.Children();
                            if (overlayChildren.IndexOf(staleHost,
                                                        staleIndex))
                            {
                                overlayChildren.RemoveAt(staleIndex);
                            }
                        }
                        continue;
                    }

                    auto host = existingHosts[activeIndex];
                    if (!host)
                    {
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
                        HorizontalAlignment::Left)
                    {
                        host.HorizontalAlignment(HorizontalAlignment::Left);
                    }
                    if (host.VerticalAlignment() !=
                        VerticalAlignment::Top)
                    {
                        host.VerticalAlignment(VerticalAlignment::Top);
                    }
                    if (host.IsHitTestVisible())
                    {
                        host.IsHitTestVisible(false);
                    }
                    if (!host.UseLayoutRounding())
                    {
                        host.UseLayoutRounding(true);
                    }
                    if (Controls::Canvas::GetZIndex(host) != 1000)
                    {
                        Controls::Canvas::SetZIndex(host, 1000);
                    }
                    if (host.Width() != hostWidth)
                    {
                        host.Width(hostWidth);
                    }
                    if (host.Height() != hostHeight)
                    {
                        host.Height(hostHeight);
                    }
                    if (Controls::Canvas::GetLeft(host) != geometry.left)
                    {
                        Controls::Canvas::SetLeft(host, geometry.left);
                    }
                    if (Controls::Canvas::GetTop(host) != geometry.top)
                    {
                        Controls::Canvas::SetTop(host, geometry.top);
                    }

                    double opacity = settings.opacityPercent / 100.0;
                    if (host.Opacity() != opacity)
                    {
                        host.Opacity(opacity);
                    }
                    ConfigureStyleHost(host, settings, taskbarOrientation,
                                       hostWidth, hostHeight);

                    reconciledSeparators.push_back(
                        {activeSeparator.name, winrt::make_weak(host)});

                    AnimationDividerCache cache;
                    cache.host = winrt::make_weak(host);
                    if (previousIcon)
                    {
                        cache.previousIcon =
                            winrt::make_weak(previousIcon);
                    }
                    cache.targetIcon = winrt::make_weak(targetIcon);
                    if (nextIcon)
                    {
                        cache.nextIcon = winrt::make_weak(nextIcon);
                    }
                    cache.orientation = taskbarOrientation;
                    cache.beforeFirst = activeSeparator.beforeFirst;
                    animationDividers.push_back(std::move(cache));
                    activeDividerCount++;
                }

                if (activeDividerCount == 0 && overlayCanvas && rootGrid)
                {
                    uint32_t overlayIndex = 0;
                    auto rootChildren = rootGrid.Children();
                    if (rootChildren.IndexOf(overlayCanvas, overlayIndex))
                    {
                        rootChildren.RemoveAt(overlayIndex);
                        overlayCanvas = nullptr;
                        taskbar.overlayCanvas = {};
                        needOverlay = false;
                    }
                    else
                    {
                        geometryFailureWasTransient = true;
                    }
                }

                taskbar.animationDividers = std::move(animationDividers);
                if (!taskbar.animationDividers.empty() && overlayCanvas)
                {
                    taskbar.animationOverlayCanvas =
                        winrt::make_weak(overlayCanvas);
                }
                else
                {
                    StopAllGeometryTracking(taskbar);
                    ClearAnimationElementCache(taskbar);
                    DetachAnimationPointerHandlers(taskbar);
                }

                if (activeSeparators.empty())
                {
                    result = ReconcileResult::noValidSeparators;
                }
                else if (geometryFailureWasTransient)
                {
                    result = ReconcileResult::temporarilyNotReady;
                }
                else if (activeDividerCount == activeSeparators.size())
                {
                    result = ReconcileResult::succeeded;
                }
                else
                {
                    result = ReconcileResult::succeededPartial;
                }

                if (settingsChanged ||
                    taskbar.lastActiveDividerCount != activeDividerCount)
                {
                    if (result == ReconcileResult::temporarilyNotReady)
                    {
                        Wh_Log(L"RECONCILE: failed: active %zu of %zu dividers",
                               activeDividerCount, activeSeparators.size());
                    }
                    else if (result == ReconcileResult::succeededPartial)
                    {
                        Wh_Log(L"RECONCILE: partial success: active %zu of %zu "
                               L"dividers",
                               activeDividerCount, activeSeparators.size());
                    }
                    else
                    {
                        Wh_Log(L"RECONCILE: succeeded with %zu active dividers",
                               activeDividerCount);
                    }
                    taskbar.lastActiveDividerCount = activeDividerCount;
                }

                if (result != ReconcileResult::temporarilyNotReady)
                {
                    taskbar.appliedSettingsGeneration = settingsGeneration;
                    CommitReconciledTaskbar(
                        taskbar, repeater, snapshot, result, needOverlay,
                        overlayCanvas ? overlayCanvas.Children().Size() : 0,
                        settings.animationCompatibility,
                        std::move(reconciledSeparators));
                    if (taskbar.reconciliationSignatureValid &&
                        (result == ReconcileResult::succeeded ||
                         result == ReconcileResult::succeededPartial))
                    {
                        StartNativeSettlingTracking(taskbar);
                        if (settings.animationCompatibility)
                        {
                            StartAnimationTracking(taskbar);
                        }
                    }
                }
            }
        }
        catch (winrt::hresult_error const &e)
        {
            InvalidateReconciliationSignature(taskbar);
            Wh_Log(L"RECONCILE: failed: 0x%08X %s",
                   static_cast<unsigned int>(e.code().value), e.message().c_str());
        }
        catch (...)
        {
            InvalidateReconciliationSignature(taskbar);
            Wh_Log(L"RECONCILE: failed with an unknown exception");
        }

        return result;
    }

    FrameworkElement DiscoverTaskbarRepeater(HWND taskbarWnd)
    {
        XamlRoot xamlRoot = GetTaskbarXamlRootForWindow(taskbarWnd);
        auto root = xamlRoot
                        ? xamlRoot.Content().try_as<FrameworkElement>()
                        : nullptr;
        return FindDescendantByName(root, L"TaskbarFrameRepeater");
    }

    struct TaskbarDiscoveryResult
    {
        size_t taskbarWindowCount = 0;
        size_t resolvedRepeaterCount = 0;
        std::vector<FrameworkElement> repeaters;
    };

    TaskbarDiscoveryResult DiscoverCurrentThreadTaskbars()
    {
        TaskbarDiscoveryResult result;
        std::vector<HWND> taskbarWindows;
        EnumThreadWindows(
            GetCurrentThreadId(),
            [](HWND hWnd, LPARAM lParam) -> BOOL
            {
                WCHAR className[32];
                if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                    (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
                     _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0))
                {
                    reinterpret_cast<std::vector<HWND> *>(lParam)
                        ->push_back(hWnd);
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&taskbarWindows));

        result.taskbarWindowCount = taskbarWindows.size();
        for (HWND taskbarWnd : taskbarWindows)
        {
            try
            {
                auto repeater = DiscoverTaskbarRepeater(taskbarWnd);
                if (!repeater)
                {
                    continue;
                }

                result.resolvedRepeaterCount++;
                auto existing = std::find_if(
                    result.repeaters.begin(), result.repeaters.end(),
                    [&](FrameworkElement const &existingRepeater)
                    {
                        return winrt::get_abi(existingRepeater) ==
                               winrt::get_abi(repeater);
                    });
                if (existing == result.repeaters.end())
                {
                    result.repeaters.push_back(repeater);
                }
            }
            catch (...)
            {
                // A taskbar can be rebuilding while the bounded initial retry is
                // running. Failure of one window must not skip the others.
            }
        }

        return result;
    }

    Controls::Grid DiscoverPrimaryTaskbarRootGrid()
    {
        HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
        XamlRoot xamlRoot =
            taskbarWnd ? GetTaskbarXamlRoot(taskbarWnd) : nullptr;
        auto root = xamlRoot
                        ? xamlRoot.Content().try_as<FrameworkElement>()
                        : nullptr;
        if (!root)
        {
            return nullptr;
        }

        if (root.Name() == L"RootGrid")
        {
            return root.try_as<Controls::Grid>();
        }

        return FindDescendantByName(root, L"RootGrid")
            .try_as<Controls::Grid>();
    }

    void CleanupAllTaskbarsForUnload()
    {
        // Stop every callback source before touching the visual tree. A queued
        // callback is still harmless because every asynchronous reconcile entry
        // checks g_unloading.
        ClearAnimationTrackingForUnload();

        size_t trackedCount =
            g_trackedTaskbars ? g_trackedTaskbars->size() : 0;
        Wh_Log(L"CLEANUP: tracked taskbars=%zu", trackedCount);

        std::vector<Controls::Grid> rootsToSweep;
        auto addUniqueRoot = [&](Controls::Grid const &rootGrid)
        {
            if (!rootGrid)
            {
                return;
            }

            auto found = std::find_if(
                rootsToSweep.begin(), rootsToSweep.end(),
                [&](Controls::Grid const &existing)
                {
                    return winrt::get_abi(existing) ==
                           winrt::get_abi(rootGrid);
                });
            if (found == rootsToSweep.end())
            {
                rootsToSweep.push_back(rootGrid);
            }
        };

        Controls::Grid primaryRoot = nullptr;
        try
        {
            primaryRoot = DiscoverPrimaryTaskbarRootGrid();
            addUniqueRoot(primaryRoot);
        }
        catch (...)
        {
        }
        Wh_Log(L"CLEANUP: current primary root live=%d",
               primaryRoot ? 1 : 0);

        if (g_trackedTaskbars)
        {
            for (auto &taskbar : *g_trackedTaskbars)
            {
                bool trackedRootLive = false;
                bool repeaterRootLive = false;
                bool trackedOverlayLive = false;
                bool trackedOverlayAttached = false;

                try
                {
                    auto rootGrid = taskbar.rootGrid.get();
                    trackedRootLive = static_cast<bool>(rootGrid);
                    addUniqueRoot(rootGrid);
                }
                catch (...)
                {
                }

                try
                {
                    auto repeater = taskbar.repeater.get();
                    auto rootGrid =
                        repeater ? FindRootGridAncestor(repeater) : nullptr;
                    repeaterRootLive = static_cast<bool>(rootGrid);
                    addUniqueRoot(rootGrid);
                }
                catch (...)
                {
                }

                try
                {
                    auto overlayCanvas = taskbar.overlayCanvas.get();
                    trackedOverlayLive = static_cast<bool>(overlayCanvas);
                    if (overlayCanvas && overlayCanvas.Name() ==
                                             L"WindhawkTaskbarSeparatorOverlay")
                    {
                        auto overlayParent =
                            Media::VisualTreeHelper::GetParent(overlayCanvas)
                                .try_as<Controls::Grid>();
                        trackedOverlayAttached =
                            static_cast<bool>(overlayParent);
                        addUniqueRoot(overlayParent);
                    }
                }
                catch (...)
                {
                }

                Wh_Log(
                    L"CLEANUP: taskbar %zu tracked root live=%d, repeater "
                    L"root live=%d, tracked overlay live=%d, stale overlay=%d",
                    taskbar.id, trackedRootLive ? 1 : 0,
                    repeaterRootLive ? 1 : 0, trackedOverlayLive ? 1 : 0,
                    trackedOverlayAttached ? 0 : 1);
            }
        }

        for (size_t rootIndex = 0; rootIndex < rootsToSweep.size(); rootIndex++)
        {
            CleanupSweepResult result;
            try
            {
                SweepOwnedElementsFromPanel(rootsToSweep[rootIndex], result);
            }
            catch (...)
            {
            }
            Wh_Log(L"CLEANUP: root %zu live=1, overlays removed=%zu, "
                   L"dividers removed=%zu",
                   rootIndex + 1, result.overlaysRemoved,
                   result.dividersRemoved);
        }

        // Normally the root sweeps removed these. This exact-name fallback also
        // covers a live tracked overlay whose parent isn't the stored root grid.
        if (g_trackedTaskbars)
        {
            for (auto &taskbar : *g_trackedTaskbars)
            {
                CleanupSweepResult result;
                RemoveTrackedOverlayByExactName(taskbar, result);
                if (result.overlaysRemoved || result.dividersRemoved)
                {
                    Wh_Log(L"CLEANUP: taskbar %zu fallback overlays removed=%zu, "
                           L"dividers removed=%zu",
                           taskbar.id, result.overlaysRemoved,
                           result.dividersRemoved);
                }
            }

            // Do not release any tracked weak references or delegates until all
            // discoverable primary and secondary visual trees have been swept.
            for (auto &taskbar : *g_trackedTaskbars)
            {
                taskbar.repeater = {};
                taskbar.rootGrid = {};
                taskbar.overlayCanvas = {};
                taskbar.animationPointerSource = {};
                taskbar.animationPointerMovedHandler = nullptr;
                ClearAnimationElementCache(taskbar);
            }
            g_trackedTaskbars->clear();
            DestroyTrackedTaskbars();
        }
    }

    struct ReconcileGuard
    {
        ~ReconcileGuard()
        {
            g_reconcilingTaskbars = false;
        }
    };

    ReconcileResult ReconcileTaskbarRepeater(
        FrameworkElement const &repeater,
        bool forceStructuralReconcile)
    {
        if (g_reconcilingTaskbars)
        {
            return ReconcileResult::temporarilyNotReady;
        }

        g_reconcilingTaskbars = true;
        ReconcileGuard guard;
        try
        {
            if (!repeater || repeater.Name() != L"TaskbarFrameRepeater" ||
                !repeater.Dispatcher().HasThreadAccess())
            {
                return ReconcileResult::temporarilyNotReady;
            }

            auto *taskbar = TrackTaskbarRepeater(repeater);
            if (!taskbar)
            {
                return ReconcileResult::temporarilyNotReady;
            }

            return ReconcileTrackedTaskbar(
                *taskbar, repeater, forceStructuralReconcile);
        }
        catch (winrt::hresult_error const &e)
        {
            Wh_Log(L"RECONCILE: failed: 0x%08X %s",
                   static_cast<unsigned int>(e.code().value), e.message().c_str());
        }
        catch (...)
        {
            Wh_Log(L"RECONCILE: failed with an unknown exception");
        }

        return ReconcileResult::temporarilyNotReady;
    }

    ReconcileResult ReconcileDividers(bool forceStructuralReconcile)
    {
        if (g_reconcilingTaskbars)
        {
            return ReconcileResult::temporarilyNotReady;
        }

        g_reconcilingTaskbars = true;
        ReconcileGuard guard;
        ReconcileResult result = ReconcileResult::noValidSeparators;

        try
        {
            PruneExpiredTrackedTaskbars();
            auto discovery = DiscoverCurrentThreadTaskbars();
            bool discoveryComplete =
                discovery.taskbarWindowCount > 0 &&
                discovery.resolvedRepeaterCount ==
                    discovery.taskbarWindowCount;
            for (auto const &repeater : discovery.repeaters)
            {
                if (!TrackTaskbarRepeater(repeater))
                {
                    discoveryComplete = false;
                }
            }

            size_t reconciledTaskbarCount = 0;
            size_t succeededTaskbarCount = 0;
            size_t partiallySucceededTaskbarCount = 0;
            size_t noValidSeparatorTaskbarCount = 0;
            bool anyTemporarilyNotReady = !discoveryComplete;
            if (g_trackedTaskbars)
            {
                for (auto &taskbar : *g_trackedTaskbars)
                {
                    try
                    {
                        auto repeater = taskbar.repeater.get();
                        if (!repeater)
                        {
                            anyTemporarilyNotReady = true;
                            continue;
                        }

                        reconciledTaskbarCount++;
                        if (!repeater.Dispatcher().HasThreadAccess())
                        {
                            anyTemporarilyNotReady = true;
                            continue;
                        }

                        ReconcileResult taskbarResult =
                            ReconcileTrackedTaskbar(
                                taskbar, repeater, forceStructuralReconcile);
                        if (taskbarResult == ReconcileResult::succeeded)
                        {
                            succeededTaskbarCount++;
                        }
                        else if (taskbarResult ==
                                 ReconcileResult::succeededPartial)
                        {
                            partiallySucceededTaskbarCount++;
                        }
                        else if (taskbarResult ==
                                 ReconcileResult::temporarilyNotReady)
                        {
                            anyTemporarilyNotReady = true;
                        }
                        else
                        {
                            noValidSeparatorTaskbarCount++;
                        }
                    }
                    catch (...)
                    {
                        anyTemporarilyNotReady = true;
                    }
                }
            }

            if (reconciledTaskbarCount == 0)
            {
                result = ReconcileResult::temporarilyNotReady;
            }
            else if (!discoveryComplete)
            {
                result = ReconcileResult::temporarilyNotReady;
            }
            else if (anyTemporarilyNotReady)
            {
                result = ReconcileResult::temporarilyNotReady;
            }
            else if (succeededTaskbarCount == reconciledTaskbarCount)
            {
                result = ReconcileResult::succeeded;
            }
            else if (noValidSeparatorTaskbarCount == reconciledTaskbarCount)
            {
                result = ReconcileResult::noValidSeparators;
            }
            else if (succeededTaskbarCount +
                         partiallySucceededTaskbarCount +
                         noValidSeparatorTaskbarCount ==
                     reconciledTaskbarCount)
            {
                result = ReconcileResult::succeededPartial;
            }
            else
            {
                result = ReconcileResult::temporarilyNotReady;
            }
        }
        catch (winrt::hresult_error const &e)
        {
            Wh_Log(L"RECONCILE: failed: 0x%08X %s",
                   static_cast<unsigned int>(e.code().value), e.message().c_str());
            result = ReconcileResult::temporarilyNotReady;
        }
        catch (...)
        {
            Wh_Log(L"RECONCILE: failed with an unknown exception");
            result = ReconcileResult::temporarilyNotReady;
        }

        return result;
    }

    void CancelInitialReconcileRetry()
    {
        DispatcherTimer timer = nullptr;
        if (g_initialReconcileTimer)
        {
            timer = std::move(g_initialReconcileTimer->timer);
            delete g_initialReconcileTimer;
            g_initialReconcileTimer = nullptr;
        }

        auto timerToken = g_initialReconcileTimerToken;
        g_initialReconcileTimerToken = {};
        g_initialReconcileAttempts = 0;

        if (timer)
        {
            timer.Stop();
            timer.Tick(timerToken);
        }
    }

    void OnInitialReconcileTimerTick(
        winrt::Windows::Foundation::IInspectable const &,
        winrt::Windows::Foundation::IInspectable const &)
    {
        if (SuppressEnabledWorkDuringUnload(L"initial reconcile timer"))
        {
            CancelInitialReconcileRetry();
            return;
        }

        constexpr int kMaximumAttempts = 20;
        g_initialReconcileAttempts++;
        ReconcileResult result = ReconcileDividers(false);
        if (result == ReconcileResult::succeeded ||
            result == ReconcileResult::succeededPartial)
        {
            CancelInitialReconcileRetry();
        }
        else if (result == ReconcileResult::noValidSeparators)
        {
            CancelInitialReconcileRetry();
        }
        else if (g_initialReconcileAttempts >= kMaximumAttempts)
        {
            int attempts = g_initialReconcileAttempts;
            CancelInitialReconcileRetry();
            Wh_Log(L"RECONCILE: initialization failed after %d attempts",
                   attempts);
        }
    }

    void BeginInitialReconcileOnUiThread()
    {
        CancelInitialReconcileRetry();

        if (SuppressEnabledWorkDuringUnload(L"initial reconcile start"))
        {
            return;
        }

        g_initialReconcileAttempts = 1;
        ReconcileResult result = ReconcileDividers(true);
        if (result == ReconcileResult::succeeded ||
            result == ReconcileResult::succeededPartial)
        {
            g_initialReconcileAttempts = 0;
            return;
        }
        if (result == ReconcileResult::noValidSeparators)
        {
            g_initialReconcileAttempts = 0;
            return;
        }

        DispatcherTimer timer;
        timer.Interval(std::chrono::milliseconds(100));
        g_initialReconcileTimerToken = timer.Tick(
            winrt::Windows::Foundation::EventHandler<
                winrt::Windows::Foundation::IInspectable>{
                OnInitialReconcileTimerTick});
        g_initialReconcileTimer = new InitialReconcileTimerState{timer};
        timer.Start();
    }

    using TaskListButton_UpdateVisualStates_t = void(WINAPI *)(void *pThis);
    TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

    void WINAPI TaskListButton_UpdateVisualStates_Hook(void *pThis)
    {
        TaskListButton_UpdateVisualStates_Original(pThis);

        if (SuppressEnabledWorkDuringUnload(
                L"TaskListButton_UpdateVisualStates_Hook"))
        {
            return;
        }

        try
        {
            void *taskListButtonIUnknownPtr = (void **)pThis + 3;
            winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
            winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

            auto taskListButton = taskListButtonIUnknown.try_as<FrameworkElement>();
            if (taskListButton)
            {
                auto repeater = FindRepeaterAncestor(taskListButton);
                if (repeater)
                {
                    ReconcileTaskbarRepeater(repeater, false);
                }
            }
        }
        catch (...)
        {
        }
    }

    bool RunReconcileOnTaskbarThread(bool enabled,
                                     bool beginBoundedRetry = false)
    {
        if (enabled &&
            SuppressEnabledWorkDuringUnload(L"window-thread dispatch"))
        {
            return true;
        }

        struct RECONCILE_REQUEST
        {
            bool enabled;
            bool beginBoundedRetry;
            bool completed;
        };

        std::vector<HWND> taskbarUiWindows;
        if (enabled)
        {
            if (HWND taskbarUiWnd = GetTaskbarUiWnd())
            {
                taskbarUiWindows.push_back(taskbarUiWnd);
            }
        }
        else
        {
            for (HWND taskbarWnd : EnumerateCurrentProcessTaskbarWindows())
            {
                HWND taskbarUiWnd = GetTaskbarDispatchWindow(taskbarWnd);
                DWORD processId = 0;
                DWORD threadId = taskbarUiWnd
                                     ? GetWindowThreadProcessId(taskbarUiWnd,
                                                                &processId)
                                     : 0;
                if (threadId != 0 && processId == GetCurrentProcessId())
                {
                    taskbarUiWindows.push_back(taskbarUiWnd);
                }
            }
        }

        if (taskbarUiWindows.empty())
        {
            if (enabled)
            {
                Wh_Log(L"RECONCILE: taskbar UI window not found");
            }
            return false;
        }

        auto reconcileProc = [](PVOID parameter)
        {
            try
            {
                auto *request =
                    static_cast<RECONCILE_REQUEST *>(parameter);
                if (request->enabled && SuppressEnabledWorkDuringUnload(
                                            L"queued window callback"))
                {
                    request->completed = true;
                    return;
                }
                if (!request->enabled)
                {
                    CleanupAllTaskbarsForUnload();
                }
                else if (request->beginBoundedRetry)
                {
                    BeginInitialReconcileOnUiThread();
                }
                else
                {
                    ReconcileDividers(true);
                }
                request->completed = true;
            }
            catch (...)
            {
            }
        };

        for (HWND taskbarUiWnd : taskbarUiWindows)
        {
            RECONCILE_REQUEST request{enabled, beginBoundedRetry, false};
            bool callbackRan = RunFromWindowThread(
                taskbarUiWnd, reconcileProc, &request);
            if (callbackRan && request.completed)
            {
                return true;
            }
        }

        if (enabled)
        {
            Wh_Log(L"RECONCILE: failed to run synchronously on the taskbar UI "
                   L"thread");
        }
        return false;
    }

    bool HookTaskbarDllSymbols()
    {
        HMODULE module =
            LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module)
        {
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
                {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
                &CSecondaryTaskBand_GetTaskbarHost_Original,
            },
            {
                {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
                &std__Ref_count_base__Decref_Original,
            },
        };

        if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)))
        {
            Wh_Log(L"HookSymbols for taskbar.dll failed");
            return false;
        }

        return true;
    }

    bool HookTaskbarViewDllSymbols(HMODULE module)
    {
        // Taskbar.View.dll, ExplorerExtensions.dll
        WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
            },
        };

        if (!HookSymbols(module, taskbarViewHooks,
                         ARRAYSIZE(taskbarViewHooks)))
        {
            Wh_Log(L"HookSymbols failed");
            return false;
        }

        return true;
    }

    HMODULE GetTaskbarViewModuleHandle()
    {
        HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
        if (!module)
        {
            module = GetModuleHandle(L"ExplorerExtensions.dll");
        }
        return module;
    }

    BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule)
    {
        return HookTaskbarViewDllSymbols(taskbarViewModule) ? TRUE : FALSE;
    }

    void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR fileName)
    {
        if (g_unloading.load(std::memory_order_acquire))
        {
            return;
        }

        if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
            !g_taskbarViewDllLoaded.exchange(true))
        {
            if (ModInitWithTaskbarView(module))
            {
                Wh_ApplyHookOperations();
            }
        }
    }

    using LoadLibraryExW_t = decltype(&LoadLibraryExW);
    LoadLibraryExW_t LoadLibraryExW_Original;

    HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                       HANDLE file,
                                       DWORD flags)
    {
        HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
        if (module)
        {
            HandleLoadedModuleIfTaskbarView(module, fileName);
        }
        return module;
    }

} // namespace

BOOL Wh_ModInit()
{
    LoadSettings();

    if (!HookTaskbarDllSymbols())
    {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle())
    {
        g_taskbarViewDllLoaded = true;
        if (!ModInitWithTaskbarView(taskbarViewModule))
        {
            return FALSE;
        }
    }
    else
    {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto loadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
        if (!loadLibraryExW)
        {
            Wh_Log(L"Failed to resolve LoadLibraryExW");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (g_unloading.load(std::memory_order_acquire))
    {
        return;
    }

    if (!g_taskbarViewDllLoaded)
    {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle())
        {
            if (!g_taskbarViewDllLoaded.exchange(true))
            {
                if (ModInitWithTaskbarView(taskbarViewModule))
                {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (g_taskbarViewDllLoaded)
    {
        RunReconcileOnTaskbarThread(true, true);
    }
}

void Wh_ModBeforeUninit()
{
    Wh_Log(L"Taskbar Separators cleanup starting");

    g_unloading.store(true, std::memory_order_release);

    if (g_taskbarViewDllLoaded)
    {
        // On success, SendMessage keeps this synchronous until all tracked
        // taskbars have been cleaned on the taskbar UI thread.
        if (!RunReconcileOnTaskbarThread(false))
        {
            Wh_Log(L"RECONCILE: unload cleanup did not reach the taskbar UI "
                   L"thread");
        }
    }
}

void Wh_ModUninit()
{
    Wh_Log(L"Taskbar Separators cleanup complete");
}

void Wh_ModSettingsChanged()
{
    LoadSettings();

    if (g_taskbarViewDllLoaded && !g_unloading)
    {
        RunReconcileOnTaskbarThread(true, true);
    }
}
