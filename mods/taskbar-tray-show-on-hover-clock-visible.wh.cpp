// ==WindhawkMod==
// @id              taskbar-tray-show-on-hover-clock-visible
// @name            Taskbar Tray Auto-Hide (Keep Clock + Show Desktop Visible)
// @description     Auto-hide tray icons except the clock and the Show Desktop button. The tray background shrinks to fit only those two items when hidden. Hover over the original tray area to reveal hidden icons.
// @version         1.3
// @author          paracat
// @github          https://github.com/AngellNeverBe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Tray Auto-Hide (Keep Clock + Show Desktop Visible)

This mod automatically hides most tray icons and controls when not in use,
leaving **only the clock and the Show Desktop button** visible. The tray
background shrinks to fit just those two items. Move the mouse over the
original tray area to reveal all icons.

## Settings
- **Hidden opacity**: Opacity of hidden tray items (usually 0).
- **Hide delay (ms)**: Delay before hiding after the mouse leaves.
- **Fade duration (ms)**: Duration of the fade animation.

Based on the original `Taskbar tray auto-hide (show on hover)` mod by m417z.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hiddenOpacity: 0
  $name: Hidden opacity
  $description: Opacity of the tray area when hidden (0-100)
- hideDelay: 3000
  $name: Hide delay (ms)
  $description: Delay before hiding the tray area after the mouse leaves
- fadeDuration: 200
  $name: Fade duration (ms)
  $description: Duration of the fade in/out animation
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <vector>
#include <unordered_set>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    double hiddenOpacity;
    int hideDelay;
    int fadeDuration;
} g_settings;

std::atomic<bool> g_unloading;

// Track which tray frames have had their MinWidth locked for cleanup
std::unordered_set<winrt::Windows::Foundation::IInspectable> g_lockedTrayFrames;

struct TrayEventSubscription {
    winrt::weak_ref<FrameworkElement> hoverArea;            // whole tray for hit testing
    std::vector<winrt::weak_ref<FrameworkElement>> fadeElements; // elements to hide (clock & show desktop excluded)
    winrt::event_token pointerMovedToken;
    DispatcherTimer timer{nullptr};
    winrt::event_token timerTickToken;
    DispatcherTimer collapseTimer{nullptr};   // delayed collapse after animation
    winrt::event_token collapseTimerTickToken;
    bool isHovering = false;
};

std::vector<TrayEventSubscription> g_traySubscriptions;

// ─── Helper functions (unchanged) ──────────────────────────────────────────

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

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

void AnimateOpacity(FrameworkElement element, double to) {
    Media::Animation::DoubleAnimation animation;
    animation.To(to);
    animation.Duration(
        DurationHelper::FromTimeSpan(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(g_settings.fadeDuration)}));

    Media::Animation::Storyboard storyboard;
    storyboard.Children().Append(animation);
    Media::Animation::Storyboard::SetTarget(animation, element);
    Media::Animation::Storyboard::SetTargetProperty(animation, L"Opacity");
    storyboard.Begin();
}

bool IsCursorOnElement(FrameworkElement element, HWND hWnd) {
    POINT pt;
    if (!GetCursorPos(&pt) || !ScreenToClient(hWnd, &pt)) {
        return false;
    }
    UINT dpi = GetDpiForWindow(hWnd);
    int logicalX = MulDiv(pt.x, 96, dpi);
    int logicalY = MulDiv(pt.y, 96, dpi);
    auto transform = element.TransformToVisual(nullptr);
    auto topLeft = transform.TransformPoint({0, 0});
    float width = (float)element.ActualWidth();
    float height = (float)element.ActualHeight();
    return logicalX >= topLeft.X && logicalX < topLeft.X + width &&
           logicalY >= topLeft.Y && logicalY < topLeft.Y + height;
}

// ─── Element collection (exclude clock and show desktop by name) ───────────

std::vector<winrt::weak_ref<FrameworkElement>> CollectFadeElements(FrameworkElement systemTrayFrame) {
    std::vector<winrt::weak_ref<FrameworkElement>> elements;
    auto systemTrayGrid = FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    if (!systemTrayGrid) {
        elements.push_back(winrt::make_weak(systemTrayFrame));
        return elements;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(systemTrayGrid);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(systemTrayGrid, i)
                         .try_as<FrameworkElement>();
        if (child) {
            // Keep clock and show desktop button always visible
            if (child.Name() == L"NotificationCenterButton" ||
                child.Name() == L"ShowDesktopStack") {
                continue;
            }
            elements.push_back(winrt::make_weak(child));
        }
    }
    return elements;
}

// ─── Lock tray frame width so hover area doesn't shrink ────────────────────

void LockTrayFrameWidth(FrameworkElement systemTrayFrame) {
    auto inspectable = systemTrayFrame.as<winrt::Windows::Foundation::IInspectable>();
    if (g_lockedTrayFrames.count(inspectable)) return;
    if (systemTrayFrame.ActualWidth() > 0) {
        systemTrayFrame.MinWidth(systemTrayFrame.ActualWidth());
        g_lockedTrayFrames.insert(inspectable);
    }
}

void UnlockTrayFrameWidth(FrameworkElement systemTrayFrame) {
    auto inspectable = systemTrayFrame.as<winrt::Windows::Foundation::IInspectable>();
    if (g_lockedTrayFrames.erase(inspectable)) {
        systemTrayFrame.ClearValue(FrameworkElement::MinWidthProperty());
    }
}

// ─── Collapse / expand helpers (const reference) ───────────────────────────

void CollapseFadeElements(const std::vector<winrt::weak_ref<FrameworkElement>>& weakElements) {
    for (const auto& weak : weakElements) {
        if (auto elem = weak.get()) {
            elem.Visibility(Visibility::Collapsed);
        }
    }
}

void ExpandFadeElements(const std::vector<winrt::weak_ref<FrameworkElement>>& weakElements) {
    for (const auto& weak : weakElements) {
        if (auto elem = weak.get()) {
            elem.Visibility(Visibility::Visible);
        }
    }
}

// ─── Core ApplyStyle with layout shrinking and right alignment ─────────────

bool ApplyStyle(XamlRoot xamlRoot, HWND hWnd) {
    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();
    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        Wh_Log(L"Failed to find SystemTrayFrame");
        return false;
    }

    // Ensure the tray frame width stays unchanged for mouse detection
    LockTrayFrameWidth(systemTrayFrame);

    // Fix the grid alignment to prevent the clock from being pulled left when other items collapse
    auto systemTrayGrid = FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    if (systemTrayGrid) {
        systemTrayGrid.HorizontalAlignment(HorizontalAlignment::Right);
    }

    auto it = std::find_if(g_traySubscriptions.begin(), g_traySubscriptions.end(),
        [&](const TrayEventSubscription& sub) {
            auto area = sub.hoverArea.get();
            return area && area == systemTrayFrame;
        });

    if (g_unloading) {
        if (it != g_traySubscriptions.end()) {
            systemTrayFrame.PointerMoved(it->pointerMovedToken);
            it->timer.Tick(it->timerTickToken);
            it->timer.Stop();
            if (it->collapseTimer) {
                it->collapseTimer.Tick(it->collapseTimerTickToken);
                it->collapseTimer.Stop();
            }
            // Restore full opacity and visibility on all fade elements
            for (auto& weak : it->fadeElements) {
                if (auto elem = weak.get()) {
                    elem.Opacity(1.0);
                    elem.Visibility(Visibility::Visible);
                }
            }
            g_traySubscriptions.erase(it);
        }
        // Reset alignment to default (Stretch) when unloading
        if (systemTrayGrid) {
            systemTrayGrid.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
        }
        UnlockTrayFrameWidth(systemTrayFrame);
        return true;
    }

    if (it == g_traySubscriptions.end()) {
        auto fadeElements = CollectFadeElements(systemTrayFrame);
        bool initialHover = IsCursorOnElement(systemTrayFrame, hWnd);

        // Set initial visibility state
        for (auto& weak : fadeElements) {
            if (auto elem = weak.get()) {
                if (initialHover) {
                    elem.Opacity(1.0);
                    elem.Visibility(Visibility::Visible);
                } else {
                    elem.Opacity(0);                    // start fully transparent
                    elem.Visibility(Visibility::Collapsed); // start collapsed
                }
            }
        }

        TrayEventSubscription sub;
        sub.hoverArea = systemTrayFrame;
        sub.fadeElements = std::move(fadeElements);
        sub.isHovering = initialHover;

        sub.timer = DispatcherTimer();
        sub.timer.Interval(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds(g_settings.hideDelay > 200 ? 100 : 20)});

        auto pMouseLeaveTime = std::make_shared<ULONGLONG>(0);

        sub.pointerMovedToken = systemTrayFrame.PointerMoved(
            [timer = sub.timer, pMouseLeaveTime,
             weakArea = winrt::make_weak(systemTrayFrame),
             weakElements = sub.fadeElements](
                winrt::Windows::Foundation::IInspectable const& sender,
                Input::PointerRoutedEventArgs const&) {
                auto area = weakArea.get();
                if (!area) return;
                *pMouseLeaveTime = 0;
                for (auto& s : g_traySubscriptions) {
                    if (s.hoverArea.get() == area) {
                        s.isHovering = true;
                        // Cancel any pending collapse
                        if (s.collapseTimer) {
                            s.collapseTimer.Stop();
                        }
                        break;
                    }
                }
                // Expand and fade in
                ExpandFadeElements(weakElements);
                for (auto& weak : weakElements) {
                    if (auto elem = weak.get()) {
                        AnimateOpacity(elem, 1.0);
                    }
                }
                timer.Start();
            });

        sub.timerTickToken = sub.timer.Tick(
            [timer = sub.timer, pMouseLeaveTime,
             weakArea = winrt::make_weak(systemTrayFrame),
             weakElements = sub.fadeElements, hWnd](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) {
                auto area = weakArea.get();
                if (!area) {
                    timer.Stop();
                    return;
                }
                if (IsCursorOnElement(area, hWnd)) {
                    *pMouseLeaveTime = 0;
                } else if (*pMouseLeaveTime == 0) {
                    *pMouseLeaveTime = GetTickCount64();
                } else if (GetTickCount64() - *pMouseLeaveTime >=
                           (ULONGLONG)g_settings.hideDelay) {
                    for (auto& s : g_traySubscriptions) {
                        if (s.hoverArea.get() == area) {
                            s.isHovering = false;
                            // Fade out first
                            for (auto& weak : weakElements) {
                                if (auto elem = weak.get()) {
                                    AnimateOpacity(elem, g_settings.hiddenOpacity);
                                }
                            }
                            // Schedule collapse after animation duration
                            if (!s.collapseTimer) {
                                s.collapseTimer = DispatcherTimer();
                                s.collapseTimerTickToken = s.collapseTimer.Tick(
                                    [weakElements](auto&&, auto&&) {
                                        CollapseFadeElements(weakElements);
                                        // Stop the timer
                                        for (auto& sub : g_traySubscriptions) {
                                            if (sub.collapseTimer) {
                                                sub.collapseTimer.Stop();
                                                break;
                                            }
                                        }
                                    });
                                s.collapseTimer.Interval(
                                    winrt::Windows::Foundation::TimeSpan{
                                        std::chrono::milliseconds(g_settings.fadeDuration)});
                            }
                            s.collapseTimer.Start();
                            break;
                        }
                    }
                    timer.Stop();
                }
            });

        if (initialHover) {
            sub.timer.Start();
        }

        g_traySubscriptions.push_back(std::move(sub));
    }

    return true;
}

// ─── Hooks, XAML root access, threading (unchanged) ────────────────────────

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;
#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + taskbarElementIUnknownOffset);
    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) return nullptr;
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CSecondaryTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) return nullptr;
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM { RunFromWindowThreadProc_t proc; void* procParam; };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;
    if (dwThreadId == GetCurrentThreadId()) { proc(procParam); return true; }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }, nullptr, dwThreadId);
    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

void ApplySettingsFromTaskbarThread(bool mainTaskbarOnly = false) {
    Wh_Log(L"Applying settings");
    EnumThreadWindows(GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            bool mainOnly = lParam != 0;
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) return TRUE;
            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0)
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            else if (!mainOnly && _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0)
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            else return TRUE;
            if (!xamlRoot) { Wh_Log(L"Getting XamlRoot failed"); return TRUE; }
            if (!ApplyStyle(xamlRoot, hWnd)) { Wh_Log(L"ApplyStyle failed"); return TRUE; }
            return TRUE;
        }, (LPARAM)mainTaskbarOnly);
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;
void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    ApplySettingsFromTaskbarThread(true);
}

using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void* pThis);
CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original;
using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis, void* taskbarModel);
CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original;
void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis, void* taskbarModel) {
    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);
    HWND taskbarWnd = CSecondaryTray_GetTrayWindow_Original(pThis);
    auto xamlRoot = GetSecondaryTaskbarXamlRoot(taskbarWnd);
    if (xamlRoot) ApplyStyle(xamlRoot, taskbarWnd);
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(hTaskbarWnd, [](void*) { ApplySettingsFromTaskbarThread(); }, nullptr);
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &TaskbarHost_FrameHeight_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"}, &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &std__Ref_count_base__Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"}, &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
        {{LR"(public: virtual struct HWND__ * __cdecl CSecondaryTray::GetTrayWindow(void))"}, &CSecondaryTray_GetTrayWindow_Original},
        {{LR"(public: virtual void __cdecl CSecondaryTray::InitModelAndHost(struct winrt::WindowsUdk::UI::Shell::TaskbarModel))"}, &CSecondaryTray_InitModelAndHost_Original, CSecondaryTray_InitModelAndHost_Hook},
    };
    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}

void LoadSettings() {
    g_settings.hiddenOpacity = std::clamp(Wh_GetIntSetting(L"hiddenOpacity"), 0, 100) / 100.0;
    g_settings.hideDelay = std::max(Wh_GetIntSetting(L"hideDelay"), 0);
    g_settings.fadeDuration = std::max(Wh_GetIntSetting(L"fadeDuration"), 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    return HookTaskbarDllSymbols() ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");
    if (HWND hTaskbar = FindCurrentProcessTaskbarWnd())
        ApplySettings(hTaskbar);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");
    g_unloading = true;
    if (HWND hTaskbar = FindCurrentProcessTaskbarWnd())
        ApplySettings(hTaskbar);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    if (HWND hTaskbar = FindCurrentProcessTaskbarWnd())
        ApplySettings(hTaskbar);
}
