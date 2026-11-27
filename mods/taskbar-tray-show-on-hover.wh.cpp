// ==WindhawkMod==
// @id              taskbar-tray-show-on-hover
// @name            Taskbar tray auto-hide (show on hover)
// @description     Hide the taskbar tray area when not in use, and show it when hovering the mouse over it
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
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
# Taskbar tray auto-hide (show on hover)

Hide the taskbar tray area when not in use, and show it when hovering the mouse
over it.

![Demonstration](https://i.imgur.com/uGReVFB.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hiddenOpacity: 10
  $name: Hidden opacity
  $description: >-
    The opacity of the tray area when hidden, from 0 (fully transparent) to 100
    (fully visible)
- hideDelay: 500
  $name: Hide delay
  $description: >-
    Time in milliseconds to wait after the mouse leaves the tray area before
    hiding it
- fadeDuration: 200
  $name: Fade duration
  $description: >-
    Duration of the fade animation in milliseconds, set to 0 for instant
    transitions
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
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

struct TrayEventSubscription {
    winrt::weak_ref<FrameworkElement> element;
    std::shared_ptr<bool> pHovering;
    winrt::event_token pointerMovedToken;
    DispatcherTimer timer{nullptr};
    winrt::event_token timerTickToken;
};

// Only accessed from the taskbar thread, which is also where it's released on
// unload, rather than by the destructor at process exit.
[[clang::no_destroy]] std::optional<std::vector<TrayEventSubscription>>
    g_traySubscriptions{std::in_place};

struct TrayAddedWait {
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    FrameworkElement::LayoutUpdated_revoker layoutUpdatedRevoker;
};

// The pending waits of ApplyStyleWhenTrayAdded, released like
// g_traySubscriptions.
[[clang::no_destroy]] std::optional<std::list<TrayAddedWait>> g_trayAddedWaits{
    std::in_place};

// On newer builds, the XAML root content is the taskbar frame, and the tray
// frame is an item of its repeater.
FrameworkElement FindSystemTrayFrame(FrameworkElement xamlRootContent) {
    if (winrt::get_class_name(xamlRootContent) != L"Taskbar.TaskbarFrame") {
        return FindChildByClassName(xamlRootContent,
                                    L"SystemTray.SystemTrayFrame");
    }

    FrameworkElement child = xamlRootContent;
    if (!(child = FindChildByName(child, L"RootGrid")) ||
        !(child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return nullptr;
    }

    FrameworkElement systemTrayFrame = nullptr;
    EnumChildElements(child, [&systemTrayFrame](FrameworkElement item) {
        if (winrt::get_class_name(item) != L"Taskbar.TaskbarExtensionElement") {
            return false;
        }

        auto extensionElement = item.try_as<Controls::ContentControl>();
        auto content =
            extensionElement
                ? extensionElement.Content().try_as<FrameworkElement>()
                : nullptr;
        if (!content ||
            winrt::get_class_name(content) != L"SystemTray.SystemTrayFrame") {
            return false;
        }

        systemTrayFrame = content;
        return true;
    });

    return systemTrayFrame;
}

bool ApplyStyle(XamlRoot xamlRoot, HWND hWnd) {
    if (!g_traySubscriptions) {
        return false;
    }

    // Drop the subscriptions of destroyed trays. The Tick handler holds the
    // timer, so it's removed for the two to be released.
    std::erase_if(*g_traySubscriptions, [](const TrayEventSubscription& sub) {
        if (sub.element.get()) {
            return false;
        }

        sub.timer.Tick(sub.timerTickToken);
        sub.timer.Stop();
        return true;
    });

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

    auto systemTrayFrame = FindSystemTrayFrame(xamlRootContent);
    if (!systemTrayFrame) {
        Wh_Log(L"Failed to find SystemTrayFrame");
        return false;
    }

    auto it =
        std::find_if(g_traySubscriptions->begin(), g_traySubscriptions->end(),
                     [&](const TrayEventSubscription& sub) {
                         auto element = sub.element.get();
                         return element && element == systemTrayFrame;
                     });

    winrt::Windows::Foundation::TimeSpan timerInterval{
        std::chrono::milliseconds(g_settings.hideDelay > 200 ? 100 : 20)};

    if (it != g_traySubscriptions->end()) {
        it->timer.Interval(timerInterval);
        if (!*it->pHovering) {
            AnimateOpacity(systemTrayFrame, g_settings.hiddenOpacity);
        }
    } else {
        bool isHovering = IsCursorOnElement(systemTrayFrame, hWnd);

        systemTrayFrame.Opacity(isHovering ? 1.0 : g_settings.hiddenOpacity);

        TrayEventSubscription sub;
        sub.element = systemTrayFrame;
        sub.pHovering = std::make_shared<bool>(isHovering);

        sub.timer = DispatcherTimer();
        sub.timer.Interval(timerInterval);

        auto timer = sub.timer;
        auto pHovering = sub.pHovering;
        auto pMouseLeaveTime = std::make_shared<ULONGLONG>(0);
        sub.pointerMovedToken = systemTrayFrame.PointerMoved(
            [timer, pHovering, pMouseLeaveTime](
                winrt::Windows::Foundation::IInspectable const& sender,
                Input::PointerRoutedEventArgs const&) {
                auto element = sender.as<FrameworkElement>();
                *pMouseLeaveTime = 0;
                if (!*pHovering) {
                    *pHovering = true;
                    AnimateOpacity(element, 1.0);
                }
                timer.Start();
            });

        sub.timerTickToken = sub.timer.Tick(
            [timer, pHovering, pMouseLeaveTime,
             weakElement = winrt::make_weak(systemTrayFrame),
             hWnd](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Windows::Foundation::IInspectable const&) {
                auto element = weakElement.get();
                if (!element) {
                    timer.Stop();
                    return;
                }

                if (IsCursorOnElement(element, hWnd)) {
                    *pMouseLeaveTime = 0;
                } else if (*pMouseLeaveTime == 0) {
                    *pMouseLeaveTime = GetTickCount64();
                } else if (GetTickCount64() - *pMouseLeaveTime >=
                           (ULONGLONG)g_settings.hideDelay) {
                    *pHovering = false;
                    AnimateOpacity(element, g_settings.hiddenOpacity);
                    timer.Stop();
                }
            });

        if (isHovering) {
            sub.timer.Start();
        }

        g_traySubscriptions->push_back(std::move(sub));
    }

    return true;
}

// On newer builds, the tray is an item of the taskbar frame repeater, which is
// added after the taskbar is started.
void ApplyStyleWhenTrayAdded(XamlRoot xamlRoot, HWND hWnd) {
    if (g_unloading) {
        return;
    }

    auto taskbarFrame = xamlRoot.Content().try_as<FrameworkElement>();
    if (!taskbarFrame ||
        winrt::get_class_name(taskbarFrame) != L"Taskbar.TaskbarFrame") {
        return;
    }

    std::erase_if(*g_trayAddedWaits, [](const TrayAddedWait& wait) {
        return !wait.taskbarFrame.get();
    });

    if (std::any_of(g_trayAddedWaits->begin(), g_trayAddedWaits->end(),
                    [&](const TrayAddedWait& wait) {
                        return wait.taskbarFrame.get() == taskbarFrame;
                    })) {
        return;
    }

    auto it = g_trayAddedWaits->emplace(g_trayAddedWaits->end());
    it->taskbarFrame = taskbarFrame;
    it->layoutUpdatedRevoker = taskbarFrame.LayoutUpdated(
        winrt::auto_revoke,
        [it, hWnd](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Windows::Foundation::IInspectable const&) {
            auto taskbarFrame = it->taskbarFrame.get();
            if (taskbarFrame && !FindSystemTrayFrame(taskbarFrame)) {
                return;
            }

            HWND hTaskbarWnd = hWnd;
            g_trayAddedWaits->erase(it);

            if (!taskbarFrame) {
                return;
            }

            try {
                ApplyStyle(taskbarFrame.XamlRoot(), hTaskbarWnd);
            } catch (...) {
                HRESULT hr = winrt::to_hresult();
                Wh_Log(L"Error %08X", hr);
            }
        });
}

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

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

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

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
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

void ApplySettingsFromTaskbarThread(bool mainTaskbarOnly = false) {
    Wh_Log(L"Applying settings");

    if (g_unloading) {
        g_trayAddedWaits.reset();

        if (g_traySubscriptions) {
            for (auto& sub : *g_traySubscriptions) {
                if (auto element = sub.element.get()) {
                    element.PointerMoved(sub.pointerMovedToken);
                    element.Opacity(1.0);
                }
                sub.timer.Tick(sub.timerTickToken);
                sub.timer.Stop();
            }
            g_traySubscriptions.reset();
        }

        return;
    }

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            bool mainTaskbarOnly = lParam != 0;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (!mainTaskbarOnly &&
                       _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }

            if (!ApplyStyle(xamlRoot, hWnd)) {
                Wh_Log(L"ApplyStyle failed");
                ApplyStyleWhenTrayAdded(xamlRoot, hWnd);
                return TRUE;
            }

            return TRUE;
        },
        (LPARAM)mainTaskbarOnly);
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;
void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L">");

    TrayUI_StartTaskbar_Original(pThis);

    ApplySettingsFromTaskbarThread(/*mainTaskbarOnly=*/true);
}

using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void* pThis);
CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original;

using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis,
                                                        void* taskbarModel);
CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original;
void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis,
                                                 void* taskbarModel) {
    Wh_Log(L">");

    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);

    HWND taskbarWnd = CSecondaryTray_GetTrayWindow_Original(pThis);

    auto xamlRoot = GetSecondaryTaskbarXamlRoot(taskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed");
        return;
    }

    if (!ApplyStyle(xamlRoot, taskbarWnd)) {
        ApplyStyleWhenTrayAdded(xamlRoot, taskbarWnd);
    }
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) -> void { ApplySettingsFromTaskbarThread(); }, 0);
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
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CSecondaryTray::GetTrayWindow(void))"},
            &CSecondaryTray_GetTrayWindow_Original,
        },
        {
            {LR"(public: virtual void __cdecl CSecondaryTray::InitModelAndHost(struct winrt::WindowsUdk::UI::Shell::TaskbarModel))"},
            &CSecondaryTray_InitModelAndHost_Original,
            CSecondaryTray_InitModelAndHost_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    g_settings.hiddenOpacity =
        std::clamp(Wh_GetIntSetting(L"hiddenOpacity"), 0, 100) / 100.0;
    g_settings.hideDelay = std::max(Wh_GetIntSetting(L"hideDelay"), 0);
    g_settings.fadeDuration = std::max(Wh_GetIntSetting(L"fadeDuration"), 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}
