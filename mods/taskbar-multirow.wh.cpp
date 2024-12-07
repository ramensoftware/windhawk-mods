// ==WindhawkMod==
// @id              taskbar-multirow
// @name            Multirow taskbar for Windows 11
// @description     Span taskbar items across multiple rows, just like it was possible before Windows 11
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
# Multirow taskbar for Windows 11

Span taskbar items across multiple rows, just like it was possible before
Windows 11.

## Notes

* The mod doesn't change the taskbar height, it only makes the task list span
  across multiple rows. To change the taskbar height, use the [Taskbar height
  and icon size](https://windhawk.net/mods/taskbar-icon-size) mod.
* The mod works best with "Taskbar alignment" set to "Left" in the taskbar
  settings in Windows.

![Screenshot](https://i.imgur.com/ZpzoDXy.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- rows: 2
  $name: Rows
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <functional>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int rows;
} g_settings;

std::atomic<bool> g_unloading;

struct TaskbarState {
    winrt::weak_ref<XamlRoot> xamlRoot;
    std::vector<float> rowOffsetAdjustment;
};

std::unordered_map<void*, TaskbarState> g_taskbarState;

HWND GetTaskbarWnd() {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);

    DWORD processId = 0;
    if (!hTaskbarWnd || !GetWindowThreadProcessId(hTaskbarWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return nullptr;
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

TaskbarState* GetTaskbarState(XamlRoot xamlRoot) {
    void* xamlRootAbi = winrt::get_abi(xamlRoot);

    auto [it, inserted] = g_taskbarState.insert(
        {xamlRootAbi, TaskbarState{
                          .xamlRoot = winrt::make_weak(xamlRoot),
                      }});

    if (!inserted && !it->second.xamlRoot.get()) {
        it->second = TaskbarState{
            .xamlRoot = winrt::make_weak(xamlRoot),
        };
    }

    // Update size in case it was just created as an empty vector or in case the
    // settings changed.
    it->second.rowOffsetAdjustment.resize(g_settings.rows);

    return &it->second;
}

void UpdateTaskbarFrameRepeaterMargin(FrameworkElement taskbarFrameRepeater,
                                      TaskbarState* taskbarState,
                                      double widthWithoutExtent) {
    double desiredMargin = 0;

    if (!g_unloading) {
        desiredMargin = -widthWithoutExtent * (g_settings.rows - 1);

        for (const auto f : taskbarState->rowOffsetAdjustment) {
            desiredMargin += f;
        }

        if (desiredMargin > 0) {
            desiredMargin = 0;
        }
    }

    auto margin = taskbarFrameRepeater.Margin();
    if (margin.Right != desiredMargin) {
        Wh_Log(L"Setting margin.Right=%f (widthWithoutExtent=%f)",
               desiredMargin, widthWithoutExtent);
        margin.Right = desiredMargin;
        taskbarFrameRepeater.Margin(margin);
    }
}

bool ApplyStyle(XamlRoot xamlRoot) {
    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

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

    auto taskbarFrameElement =
        FindChildByName(xamlRootContent, L"TaskbarFrame");
    if (!taskbarFrameElement) {
        return false;
    }

    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        return false;
    }

    double systemTrayFrameWidth = systemTrayFrame.ActualWidth();

    double widthWithoutExtent =
        taskbarFrameElement.Width() - systemTrayFrameWidth;

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     widthWithoutExtent);

    return true;
}

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
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

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyles failed");
                return TRUE;
            }

            return TRUE;
        },
        0);
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) WINAPI -> void { ApplySettingsFromTaskbarThread(); },
        0);
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, const winrt::Windows::Foundation::Rect* rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI
IUIElement_Arrange_Hook(void* pThis,
                        const winrt::Windows::Foundation::Rect* rect) {
    Wh_Log(L">");

    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (g_unloading) {
        return original();
    }

    FrameworkElement element = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();
    if (!taskbarFrameRepeater ||
        taskbarFrameRepeater.Name() != L"TaskbarFrameRepeater") {
        return original();
    }

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

    auto taskbarFrameElement =
        FindChildByName(xamlRootContent, L"TaskbarFrame");
    if (!taskbarFrameElement) {
        return original();
    }

    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        return original();
    }

    double systemTrayFrameWidth = systemTrayFrame.ActualWidth();

    double widthWithoutExtent =
        taskbarFrameElement.Width() - systemTrayFrameWidth;

    winrt::Windows::Foundation::Rect newRect = *rect;
    newRect.Height /= g_settings.rows;
    for (int i = 0; i < g_settings.rows - 1 &&
                    newRect.X + newRect.Width > widthWithoutExtent;
         i++) {
        newRect.X -= widthWithoutExtent;
        if (newRect.X <= 0) {
            taskbarState->rowOffsetAdjustment[i] = -newRect.X;
            newRect.X = 0;
        } else {
            newRect.X += taskbarState->rowOffsetAdjustment[i];
        }

        newRect.Y += newRect.Height;
    }

    if (newRect.X + newRect.Width > widthWithoutExtent) {
        UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                         widthWithoutExtent);
    }

    return IUIElement_Arrange_Original(pThis, &newRect);
}

using TaskbarFrame_SystemTrayExtent_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_SystemTrayExtent_t TaskbarFrame_SystemTrayExtent_Original;
void WINAPI TaskbarFrame_SystemTrayExtent_Hook(void* pThis, double value) {
    Wh_Log(L"> %f", value);

    TaskbarFrame_SystemTrayExtent_Original(pThis, value);

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        return;
    }

    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = taskbarFrameElement;
    if ((child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }

    if (!taskbarFrameRepeater) {
        return;
    }

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    double widthWithoutExtent = taskbarFrameElement.Width() - value;

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     widthWithoutExtent);
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
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
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols() {
    WCHAR dllPath[MAX_PATH];
    if (!GetWindowsDirectory(dllPath, ARRAYSIZE(dllPath))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    wcscat_s(
        dllPath, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");

    HMODULE module =
        LoadLibraryEx(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module) {
        Wh_Log(L"Taskbar view module couldn't be loaded");
        return false;
    }

    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement<struct winrt::Windows::UI::Xaml::IUIElement>::Arrange(struct winrt::Windows::Foundation::Rect const &)const )"},
            &IUIElement_Arrange_Original,
            IUIElement_Arrange_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SystemTrayExtent(double))"},
            &TaskbarFrame_SystemTrayExtent_Original,
            TaskbarFrame_SystemTrayExtent_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    g_settings.rows = Wh_GetIntSetting(L"rows");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (!HookTaskbarViewDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = GetTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hTaskbarWnd = GetTaskbarWnd();
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

    HWND hTaskbarWnd = GetTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}
