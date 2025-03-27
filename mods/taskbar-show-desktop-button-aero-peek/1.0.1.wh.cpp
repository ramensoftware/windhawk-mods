// ==WindhawkMod==
// @id              taskbar-show-desktop-button-aero-peek
// @name            Aero Peek on "Show desktop" button hover
// @description     Enable Aero Peek when hovering over the "Show Desktop" button, like it was possible before Windows 11
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
# Aero Peek on "Show desktop" button hover

Enable Aero Peek when hovering over the "Show Desktop" button, like it was
possible before Windows 11.

Note that Aero Peek can also be activated with the `Win+,` (Win key, then the
comma key) keyboard shortcut.

Only Windows 11 is supported. This mod isn't needed for Windows 7 to 10, where
this behavior is already present.

![Demonstration](https://i.imgur.com/TsJX013.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hoverDelay: 500
  $name: Hover delay
  $description: >-
    The delay, in milliseconds, before activating the aero peek
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <functional>
#include <optional>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int hoverDelay;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;

std::optional<winrt::Windows::Foundation::Point> g_startPointerPos;
UINT g_hoverTimer;
BOOL g_aeroPeekOn;

using DwmpActivateLivePreview_t = HRESULT(WINAPI*)(BOOL peekOn,
                                                   HWND hPeekWindow,
                                                   HWND hTopmostWindow,
                                                   UINT peekType,
                                                   void* param5);
DwmpActivateLivePreview_t pDwmpActivateLivePreview;

bool IsDragDistance(winrt::Windows::Foundation::Point p1,
                    winrt::Windows::Foundation::Point p2) {
    return std::abs(p1.X - p2.X) > GetSystemMetrics(SM_CXDRAG) ||
           std::abs(p1.Y - p2.Y) > GetSystemMetrics(SM_CYDRAG);
}

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByName(FrameworkElement element, PCWSTR name) {
    return EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
}

FrameworkElement GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    return EnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

bool IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    return !!GetParentElementByClassName(element, className);
}

void AeroPeekOnHoverStart() {
    if (!g_aeroPeekOn) {
        pDwmpActivateLivePreview(true, nullptr, nullptr, 3, nullptr);
        g_aeroPeekOn = true;
    }
}

void AeroPeekOnHoverStartTimer() {
    g_hoverTimer =
        SetTimer(nullptr, g_hoverTimer, g_settings.hoverDelay,
                 [](HWND hwnd,         // handle of window for timer messages
                    UINT uMsg,         // WM_TIMER message
                    UINT_PTR idEvent,  // timer identifier
                    DWORD dwTime       // current system time
                 ) {
                     Wh_Log(L">");

                     KillTimer(nullptr, g_hoverTimer);
                     g_hoverTimer = 0;

                     AeroPeekOnHoverStart();
                 });
}

void AeroPeekOnHoverStop() {
    if (g_hoverTimer) {
        KillTimer(nullptr, g_hoverTimer);
        g_hoverTimer = 0;
    }

    if (g_aeroPeekOn) {
        pDwmpActivateLivePreview(false, nullptr, nullptr, 3, nullptr);
        g_aeroPeekOn = false;
    }
}

using IconView_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
IconView_OnPointerMoved_t IconView_OnPointerMoved_Original;
int WINAPI IconView_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return IconView_OnPointerMoved_Original(pThis, pArgs);
    };

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);

    if (className != L"SystemTray.IconView" ||
        !IsChildOfElementByName(element, L"ShowDesktopStack")) {
        return original();
    }

    Wh_Log(L"> %s", className.c_str());

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    winrt::Windows::Foundation::Point pointerPos =
        args.GetCurrentPoint(element).Position();

    if (!g_startPointerPos) {
        g_startPointerPos = pointerPos;
        AeroPeekOnHoverStartTimer();
    } else if (!g_aeroPeekOn &&
               IsDragDistance(*g_startPointerPos, pointerPos)) {
        AeroPeekOnHoverStop();
        g_startPointerPos = pointerPos;
        AeroPeekOnHoverStartTimer();
    }

    return original();
}

using IconView_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
IconView_OnPointerExited_t IconView_OnPointerExited_Original;
int WINAPI IconView_OnPointerExited_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return IconView_OnPointerExited_Original(pThis, pArgs);
    };

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));

    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);

    if (className != L"SystemTray.IconView" ||
        !IsChildOfElementByName(element, L"ShowDesktopStack")) {
        return original();
    }

    Wh_Log(L"> %s", className.c_str());

    AeroPeekOnHoverStop();
    g_startPointerPos.reset();

    return original();
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &IconView_OnPointerMoved_Original,
            IconView_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &IconView_OnPointerExited_Original,
            IconView_OnPointerExited_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
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

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    g_settings.hoverDelay = Wh_GetIntSetting(L"hoverDelay");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE dwmapiModule = LoadLibrary(L"dwmapi.dll");
    if (dwmapiModule) {
        pDwmpActivateLivePreview =
            (DwmpActivateLivePreview_t)GetProcAddress(dwmapiModule, (PCSTR)113);
    }

    if (!pDwmpActivateLivePreview) {
        Wh_Log(L"Couldn't get DwmpActivateLivePreview");
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
