// ==WindhawkMod==
// @id              taskbar-start-button-position
// @name            Start button always on the left
// @description     Forces the start button to be on the left of the taskbar, even when taskbar icons are centered (Windows 11 only)
// @version         1.0
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
# Start button always on the left

Forces the start button to be on the left of the taskbar, even when taskbar
icons are centered.

Only Windows 11 is supported.

Known limitations:
* There's a jumpy animation when a centered taskbar is animated.
* Secondary monitors aren't supported.

![Screenshot](https://i.imgur.com/bEqvfOE.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <functional>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

std::atomic<bool> g_appliedSettings;
std::atomic<bool> g_unloading;

FrameworkElement g_startButtonElement = nullptr;
winrt::event_token g_taskbarFrameRepeaterLayoutUpdatedToken;
UINT_PTR g_updateStartButtonPositionTimer;
int g_updateStartButtonPositionTimerCounter;

HWND GetTaskbarWnd() {
    static HWND hTaskbarWnd;

    if (!hTaskbarWnd) {
        HWND hWnd = FindWindow(L"Shell_TrayWnd", nullptr);

        DWORD processId = 0;
        if (hWnd && GetWindowThreadProcessId(hWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            hTaskbarWnd = hWnd;
        }
    }

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

void UpdateStartButtonPosition(FrameworkElement startButton) {
    Wh_Log(L">");

    FrameworkElement xamlRootContent =
        startButton.XamlRoot().Content().try_as<FrameworkElement>();

    auto pt = startButton.TransformToVisual(xamlRootContent)
                  .TransformPoint(winrt::Windows::Foundation::Point{0, 0});
    Wh_Log(L"%f, %f", pt.X, pt.Y);

    if (pt.X == 0) {
        return;
    }

    Media::TranslateTransform transform;
    if (auto prevTransform =
            startButton.RenderTransform().try_as<Media::TranslateTransform>()) {
        transform = prevTransform;
    }

    transform.X(transform.X() - pt.X);
    startButton.RenderTransform(transform);
}

void ResetStartButtonPosition(FrameworkElement startButton) {
    Wh_Log(L">");

    Media::TranslateTransform transform;
    if (auto prevTransform =
            startButton.RenderTransform().try_as<Media::TranslateTransform>()) {
        transform = prevTransform;
    }

    transform.X(0);
    startButton.RenderTransform(transform);
}

void ScheduleUpdateStartButtonPosition() {
    g_updateStartButtonPositionTimerCounter = 0;
    g_updateStartButtonPositionTimer =
        SetTimer(nullptr, g_updateStartButtonPositionTimer, 100,
                 [](HWND hwnd,         // handle of window for timer messages
                    UINT uMsg,         // WM_TIMER message
                    UINT_PTR idEvent,  // timer identifier
                    DWORD dwTime       // current system time
                    ) WINAPI {
                     g_updateStartButtonPositionTimerCounter++;
                     if (g_updateStartButtonPositionTimerCounter >= 10) {
                         KillTimer(nullptr, g_updateStartButtonPositionTimer);
                         g_updateStartButtonPositionTimer = 0;
                     }

                     UpdateStartButtonPosition(g_startButtonElement);
                 });
}

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement xamlRootContent =
        xamlRoot.Content().try_as<FrameworkElement>();

    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }

    if (!taskbarFrameRepeater) {
        return false;
    }

    auto startButton =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.ExperienceToggleButton") {
                return false;
            }

            auto automationId =
                Automation::AutomationProperties::GetAutomationId(child);
            return automationId == L"StartButton";
        });
    if (!startButton) {
        return false;
    }

    double startButtonWidth = startButton.ActualWidth();

    Thickness taskbarFrameRepeaterMargin = taskbarFrameRepeater.Margin();
    taskbarFrameRepeaterMargin.Left = g_unloading ? 0 : startButtonWidth;
    taskbarFrameRepeater.Margin(taskbarFrameRepeaterMargin);

    Thickness startButtonMargin = startButton.Margin();
    startButtonMargin.Left = g_unloading ? 0 : -startButtonWidth;
    startButton.Margin(startButtonMargin);

    if (!g_unloading) {
        g_startButtonElement = startButton;

        ScheduleUpdateStartButtonPosition();

        if (!g_taskbarFrameRepeaterLayoutUpdatedToken) {
            g_taskbarFrameRepeaterLayoutUpdatedToken =
                taskbarFrameRepeater.LayoutUpdated([](auto&&, auto&& args) {
                    ScheduleUpdateStartButtonPosition();
                });
        }
    } else {
        if (g_taskbarFrameRepeaterLayoutUpdatedToken) {
            taskbarFrameRepeater.LayoutUpdated(
                g_taskbarFrameRepeaterLayoutUpdatedToken);
            g_taskbarFrameRepeaterLayoutUpdatedToken = winrt::event_token{};
        }

        if (g_updateStartButtonPositionTimer) {
            KillTimer(nullptr, g_updateStartButtonPositionTimer);
            g_updateStartButtonPositionTimer = 0;
        }

        ResetStartButtonPosition(startButton);
        g_startButtonElement = nullptr;
    }

    return true;
}

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = PVOID(WINAPI*)(PVOID pThis, PVOID* result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(PVOID pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    PVOID taskBand = (PVOID)GetWindowLongPtr(hTaskSwWnd, 0);
    PVOID taskBandForTaskListWndSite = taskBand;
    while (*(PVOID*)taskBandForTaskListWndSite !=
           CTaskBand_ITaskListWndSite_vftable) {
        taskBandForTaskListWndSite = (PVOID*)taskBandForTaskListWndSite + 1;
    }

    PVOID taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    // Reference: TaskbarHost::FrameHeight
    constexpr size_t kTaskbarElementIUnknownOffset = 0x40;

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      kTaskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
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
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
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

void ApplySettings(HWND hTaskbarWnd) {
    Wh_Log(L"Applying settings");

    struct ApplySettingsParam {
        HWND hTaskbarWnd;
    };

    ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
    };

    RunFromWindowThread(
        hTaskbarWnd,
        [](PVOID pParam) WINAPI {
            ApplySettingsParam& param = *(ApplySettingsParam*)pParam;

            auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyles failed");
            }
        },
        &param);
}

using CPearl_SetBounds_t = HRESULT(WINAPI*)(void* pThis, void* param1);
CPearl_SetBounds_t CPearl_SetBounds_Original;
HRESULT WINAPI CPearl_SetBounds_Hook(void* pThis, void* param1) {
    Wh_Log(L">");

    if (!g_appliedSettings) {
        HWND hTaskbarWnd = GetTaskbarWnd();
        if (hTaskbarWnd) {
            g_appliedSettings = true;
            ApplySettings(hTaskbarWnd);
        }
    }

    return CPearl_SetBounds_Original(pThis, param1);
}

BOOL HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: long __cdecl CPearl::SetBounds(struct tagRECT const &))"},
            (void**)&CPearl_SetBounds_Original,
            (void*)CPearl_SetBounds_Hook,
        },
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            (void**)&CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            (void**)&CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            (void**)&std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = GetTaskbarWnd();
    if (hTaskbarWnd) {
        g_appliedSettings = true;
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_appliedSettings) {
        HWND hTaskbarWnd = GetTaskbarWnd();
        if (hTaskbarWnd) {
            ApplySettings(hTaskbarWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
