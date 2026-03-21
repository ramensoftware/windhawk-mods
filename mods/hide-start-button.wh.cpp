// ==WindhawkMod==
// @id              hide-start-button
// @name            Hide Start Button
// @description     Hides the start button from the taskbar (Windows 11 only). The start menu can still be opened via keyboard or touchscreen.
// @version         1.0
// @author          ptrkhh
// @github          https://github.com/ptrkhh
// @homepage        https://www.linkedin.com/in/patrick-hermawan/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Start Button

Hides the start button from the taskbar. The start menu can still be opened
via Win key, Ctrl+Esc key or touchscreen gestures.

Only Windows 11 is supported.

Based on "Start button always on the left" (taskbar-start-button-position) mod.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <atomic>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;
thread_local bool g_inArrangeOverride;

template <typename F>
FrameworkElement EnumChildElements(FrameworkElement element, F callback) {
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child && callback(child))
            return child;
    }
    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement child =
        xamlRoot.Content().try_as<FrameworkElement>();

    if (!child ||
        !(child = EnumChildElements(child, [](FrameworkElement c) {
            return winrt::get_class_name(c) == L"Taskbar.TaskbarFrame";
        })) ||
        !(child = FindChildByName(child, L"RootGrid")) ||
        !(child = FindChildByName(child, L"TaskbarFrameRepeater")))
        return false;

    auto startButton = EnumChildElements(child, [](FrameworkElement c) {
        return winrt::get_class_name(c) ==
                   L"Taskbar.ExperienceToggleButton" &&
               Automation::AutomationProperties::GetAutomationId(c) ==
                   L"StartButton";
    });

    if (startButton)
        startButton.Visibility(g_unloading ? Visibility::Visible
                                           : Visibility::Collapsed);

    return true;
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
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1])
        return nullptr;

    size_t offset = 0x48;
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F)
        offset = b[7];

    auto* unk = *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + offset);
    FrameworkElement fe = nullptr;
    unk->QueryInterface(winrt::guid_of<FrameworkElement>(),
                        winrt::put_abi(fe));

    auto result = fe ? fe.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hWnd, bool isSecondary) {
    HWND hTaskSwWnd = isSecondary
        ? (HWND)FindWindowEx(hWnd, nullptr, L"WorkerW", nullptr)
        : (HWND)GetProp(hWnd, L"TaskbandHWND");
    if (!hTaskSwWnd)
        return nullptr;

    void* vftable = isSecondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                                : CTaskBand_ITaskListWndSite_vftable;
    void* p = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    for (int i = 0; *(void**)p != vftable; i++) {
        if (i == 20)
            return nullptr;
        p = (void**)p + 1;
    }

    void* sharedPtr[2]{};
    if (isSecondary)
        CSecondaryTaskBand_GetTaskbarHost_Original(p, sharedPtr);
    else
        CTaskBand_GetTaskbarHost_Original(p, sharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(sharedPtr);
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hWnd = nullptr;
    while ((hWnd = FindWindowEx(nullptr, hWnd, L"Shell_TrayWnd", nullptr))) {
        DWORD pid;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == GetCurrentProcessId())
            return hWnd;
    }
    return nullptr;
}

void ApplySettingsFromTaskbarThread() {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
            WCHAR cls[32];
            if (GetClassName(hWnd, cls, ARRAYSIZE(cls)) == 0)
                return TRUE;

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(cls, L"Shell_TrayWnd") == 0)
                xamlRoot = GetTaskbarXamlRoot(hWnd, false);
            else if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0)
                xamlRoot = GetTaskbarXamlRoot(hWnd, true);

            if (xamlRoot)
                ApplyStyle(xamlRoot);
            return TRUE;
        },
        0);
}

void ApplySettings(HWND hTaskbarWnd) {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    DWORD threadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!threadId)
        return;

    if (threadId == GetCurrentThreadId()) {
        ApplySettingsFromTaskbarThread();
        return;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION &&
                ((const CWPSTRUCT*)lParam)->message == msg)
                ApplySettingsFromTaskbarThread();
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook)
        return;

    SendMessage(hTaskbarWnd, msg, 0, 0);
    UnhookWindowsHookEx(hook);
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (!g_inArrangeOverride || g_unloading)
        return original();

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element)
        return original();

    if (winrt::get_class_name(element) != L"Taskbar.ExperienceToggleButton" ||
        Automation::AutomationProperties::GetAutomationId(element) !=
            L"StartButton")
        return original();

    element.Visibility(Visibility::Collapsed);
    return IUIElement_Arrange_Original(pThis, {0, 0, 0, rect.Height});
}

using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    [[maybe_unused]] static bool hooked = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;
        void** vtable = *(void***)winrt::get_abi(element);
        WindhawkUtils::SetFunctionHook((IUIElement_Arrange_t)vtable[92],
                                       IUIElement_Arrange_Hook,
                                       &IUIElement_Arrange_Original);
        Wh_ApplyHookOperations();
        return true;
    }();

    g_inArrangeOverride = true;
    HRESULT ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);
    g_inArrangeOverride = false;
    return ret;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module)
        return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &std__Ref_count_base__Decref_Original},
    };
    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
         &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
         TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook},
    };
    return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        if (HookTaskbarViewDllSymbols(module))
            Wh_ApplyHookOperations();
    }
    return module;
}

BOOL Wh_ModInit() {
    if (!HookTaskbarDllSymbols())
        return FALSE;

    if (HMODULE m = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(m))
            return FALSE;
    } else {
        HMODULE kb = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLib =
            (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pLoadLib, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE m = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(m))
                    Wh_ApplyHookOperations();
            }
        }
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
        ApplySettings(hTaskbarWnd);
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd)
        ApplySettings(hTaskbarWnd);
}

void Wh_ModUninit() {}
