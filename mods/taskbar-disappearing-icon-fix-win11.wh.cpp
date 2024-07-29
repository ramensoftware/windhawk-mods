// ==WindhawkMod==
// @id              taskbar-disappearing-icon-fix-win11
// @name            Taskbar Disappearing Icon Fix for Windows 11
// @description     Tries to fix disappearing icon in Windows 11 taskbar while switching virtual desktops.
// @version         1.0
// @author          yezhiyi9670
// @github          https://github.com/yezhiyi9670
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0605 -loleaut32 -lole32 -lruntimeobject -lwininet
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Framework based on taskbar-labels mod by @m417z

// ==WindhawkModReadme==
/*
# Taskbar Disappearing Icon Fix for Windows 11

Tries to fix disappearing icon bug in Windows 11 taskbar while switching virtual desktops.

Since I cannot reproduce this bug very reliably, this mod may not have fixed the bug entirely. Let me know if it still persists.

Takeaway message:

- It is observed that hiding (`Visibility: Collapsed`) the invisible icon and then making it visible after a while will fix it.
- However, hiding the icon for a while results in flickering.
- The mod tries to flicker the icon only when the system tries to change its display name, typically while switching virtual desktops, making the flicker hardly visible.

*/
// ==/WindhawkModReadme==

// #==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings

*/
// #==/WindhawkModSettings==


#undef GetCurrentTime

#include <initguid.h>  // must come before knownfolders.h

#include <inspectable.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <wininet.h>

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <string>
#include <vector>
#include <windhawk_utils.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::Foundation::Numerics;

// ===========================================================

WCHAR g_taskbarViewDllPath[MAX_PATH];
std::atomic<bool> g_unloading = false;

// ===========================================================

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

bool GetTaskbarViewDllPath(WCHAR path[MAX_PATH]) {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    // Windows 11 version 22H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    // Windows 11 version 21H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    return false;
}

// ===========================================================

void RefreshTaskbar() {
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, 0, 0);
    Sleep(400);
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, 0, 0);
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (child.Name() == name) {
            return child;
        }
    }

    return nullptr;
}

// ===========================================================

void HideAndShowIcon(FrameworkElement taskListButtonElement) {
    if(g_unloading) {
        return;
    }
    
    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
    if (!iconPanelElement) {
        return;
    }

    auto iconElement =
        FindChildByName(iconPanelElement, L"Icon")
            .as<Controls::Image>();

    if(iconElement) {
        iconElement.Visibility(Visibility::Collapsed);
        iconElement.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [iconElement](){
            iconElement.Visibility(Visibility::Visible);
        });
    }
}

using TaskListButton_Hook2_t = void(WINAPI*)(void* pThis, struct winrt::hstring w);
TaskListButton_Hook2_t TaskListButton_Hook2_Original;
void WINAPI TaskListButton_Hook2_Hook(void* pThis, struct winrt::hstring w) {
    TaskListButton_Hook2_Original(pThis, w);

    Wh_Log(L">Hook2");

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    HideAndShowIcon(taskListButtonElement);
}

// ===========================================================

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // taskbar.view.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        // {
        //     {
        //         LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))",
        //         LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void) __ptr64)",
        //     },
        //     (void**)&TaskListButton_Hook2_Original,
        //     (void*)TaskListButton_Hook2_Hook,
        // },
        // {
        //     {
        //         LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream))",
        //         LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon(struct winrt::Windows::Storage::Streams::IRandomAccessStream) __ptr64)",
        //     },
        //     (void**)&TaskListButton_Hook2_Original,
        //     (void*)TaskListButton_Hook2_Hook,
        // },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::DisplayName(struct winrt::hstring))",
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::DisplayName(struct winrt::hstring) __ptr64)",
            },
            (void**)&TaskListButton_Hook2_Original,
            (void*)TaskListButton_Hook2_Hook,
        },
    };

    return WindhawkUtils::HookSymbols(module, symbolHooks,
                                      ARRAYSIZE(symbolHooks));
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule) {
    if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
        return FALSE;
    }

    return TRUE;
}

// ===========================================================

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!GetTaskbarViewDllPath(g_taskbarViewDllPath)) {
        Wh_Log(L"Taskbar view module not found");
        return FALSE;
    }

    HMODULE taskbarViewModule = LoadLibraryEx(g_taskbarViewDllPath, nullptr,
                                              LOAD_WITH_ALTERED_SEARCH_PATH);
    if (taskbarViewModule) {
        return ModInitWithTaskbarView(taskbarViewModule);
    } else {
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    RefreshTaskbar();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    RefreshTaskbar();

    // This is required to give time for taskbar buttons of UWP apps to
    // update the layout.
    Sleep(400);
}
