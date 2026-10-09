// ==WindhawkMod==
// @id              taskbar-thumbnail-size
// @name            Taskbar Thumbnail Size
// @description     Customize the size of the new taskbar thumbnails in Windows 11
// @version         1.2.1
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
# Taskbar Thumbnail Size

Customize the size of the new taskbar thumbnails in Windows 11.

By default, this mod uses simple percentage-based scaling. You can optionally
enable absolute sizing mode to set specific pixel dimensions for thumbnails.

For older Windows versions, the size of taskbar thumbnails can be changed via
the registry:

* [Change Size of Taskbar Thumbnail Previews in Windows
  11](https://www.elevenforum.com/t/change-size-of-taskbar-thumbnail-previews-in-windows-11.6340/)
  (before Windows 11 version 24H2)
* [How to Change the Size of Taskbar Thumbnails in Windows
  10](https://www.tenforums.com/tutorials/26105-change-size-taskbar-thumbnails-windows-10-a.html)

![Demonstration](https://i.imgur.com/nGxrBYG.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- size: 150
  $name: Thumbnail size (percentage)
  $description: >-
    Percentage of the original size. Used when absolute sizing is disabled.
- useAbsoluteSize: false
  $name: Use absolute sizing
  $description: >-
    When enabled, uses the min/max pixel constraints below instead of
    percentage-based scaling.
- minWidth: 180
  $name: Minimum width (pixels)
  $description: >-
    Minimum thumbnail width in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
- minHeight: 120
  $name: Minimum height (pixels)
  $description: >-
    Minimum thumbnail height in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
- maxWidth: 180
  $name: Maximum width (pixels)
  $description: >-
    Maximum thumbnail width in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
- maxHeight: 120
  $name: Maximum height (pixels)
  $description: >-
    Maximum thumbnail height in pixels. Only used when absolute sizing is
    enabled. Set to 0 to disable.
- preserveAspectRatio: true
  $name: Preserve aspect ratio
  $description: >-
    Forcibly preserves thumbnail aspect ratios while fitting Maximum
    constraints. Fits Minimum constraints as best as possible.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <cmath>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int size;
    bool useAbsoluteSize;
    int minWidth;
    int maxWidth;
    int minHeight;
    int maxHeight;
    bool preserveAspectRatio;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;

using ThumbnailHelpers_GetScaledThumbnailSize_t =
    winrt::Windows::Foundation::Size*(
        WINAPI*)(winrt::Windows::Foundation::Size* resultBuffer,
                 winrt::Windows::Foundation::Size size,
                 float scale);
ThumbnailHelpers_GetScaledThumbnailSize_t
    ThumbnailHelpers_GetScaledThumbnailSize_Original;
winrt::Windows::Foundation::Size* WINAPI
ThumbnailHelpers_GetScaledThumbnailSize_Hook(
    winrt::Windows::Foundation::Size* resultBuffer,
    winrt::Windows::Foundation::Size size,
    float scale) {
    Wh_Log(L"> %fx%f %f", size.Width, size.Height, scale);

    if (!g_settings.useAbsoluteSize) {
        // Original percentage-based behavior
        winrt::Windows::Foundation::Size* ret =
            ThumbnailHelpers_GetScaledThumbnailSize_Original(
                resultBuffer, size, scale * g_settings.size / 100.0);

        Wh_Log(L"%fx%f", ret->Width, ret->Height);
        return ret;
    }

    // Absolute sizing mode
    winrt::Windows::Foundation::Size* result =
        ThumbnailHelpers_GetScaledThumbnailSize_Original(resultBuffer, size,
                                                         scale);

    float currentWidth = result->Width;
    float currentHeight = result->Height;

    if (currentWidth <= 0 || currentHeight <= 0) {
        // Avoid division by zero and invalid sizes
        Wh_Log(L"Invalid original size %fx%f, skipping", currentWidth,
               currentHeight);
        return result;
    }

    if (g_settings.preserveAspectRatio) {
        // Calculate the scale factors needed to satisfy each constraint
        // A scale > 1 means we need to enlarge, < 1 means shrink
        float minScaleForWidth = 1.0f;
        float minScaleForHeight = 1.0f;
        float maxScaleForWidth = std::numeric_limits<float>::infinity();
        float maxScaleForHeight = std::numeric_limits<float>::infinity();

        if (g_settings.minWidth > 0) {
            minScaleForWidth = g_settings.minWidth / currentWidth;
        }
        if (g_settings.minHeight > 0) {
            minScaleForHeight = g_settings.minHeight / currentHeight;
        }
        if (g_settings.maxWidth > 0) {
            maxScaleForWidth = g_settings.maxWidth / currentWidth;
        }
        if (g_settings.maxHeight > 0) {
            maxScaleForHeight = g_settings.maxHeight / currentHeight;
        }

        // The minimum scale we must apply (to satisfy min constraints)
        float minRequiredScale = std::fmax(minScaleForWidth, minScaleForHeight);
        // The maximum scale we can apply (to satisfy max constraints)
        float maxAllowedScale = std::fmin(maxScaleForWidth, maxScaleForHeight);

        // Choose the scale that results in minimal change from original
        float finalScale = 1.0f;

        if (maxAllowedScale >= minRequiredScale) {
            // Constraints are satisfiable - pick scale closest to 1.0
            if (minRequiredScale > 1.0f) {
                // Need to enlarge to meet minimum constraints
                finalScale = minRequiredScale;
            } else if (maxAllowedScale < 1.0f) {
                // Need to shrink to meet maximum constraints
                finalScale = maxAllowedScale;
            }
            // else: current size already satisfies all constraints, keep
            // scale = 1.0
        } else {
            // Constraints conflict - prioritize max constraints (fit within
            // bounds)
            finalScale = maxAllowedScale;
        }

        winrt::Windows::Foundation::Size* ret =
            finalScale == 1.0f
                ? result
                : ThumbnailHelpers_GetScaledThumbnailSize_Original(
                      resultBuffer, size, scale * finalScale);
        Wh_Log(L"%fx%f", ret->Width, ret->Height);
        return ret;
    }

    float targetWidth = currentWidth;
    float targetHeight = currentHeight;

    // Apply constraints without regard to aspect ratio
    if (g_settings.minWidth > 0 && targetWidth < g_settings.minWidth) {
        targetWidth = g_settings.minWidth;
    }
    if (g_settings.maxWidth > 0 && targetWidth > g_settings.maxWidth) {
        targetWidth = g_settings.maxWidth;
    }
    if (g_settings.minHeight > 0 && targetHeight < g_settings.minHeight) {
        targetHeight = g_settings.minHeight;
    }
    if (g_settings.maxHeight > 0 && targetHeight > g_settings.maxHeight) {
        targetHeight = g_settings.maxHeight;
    }

    result->Width = targetWidth;
    result->Height = targetHeight;

    Wh_Log(L"%fx%f", result->Width, result->Height);
    return result;
}

using TaskItemThumbnailView_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
TaskItemThumbnailView_OnApplyTemplate_t
    TaskItemThumbnailView_OnApplyTemplate_Original;
void WINAPI TaskItemThumbnailView_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");

    TaskItemThumbnailView_OnApplyTemplate_Original(pThis);

    IUnknown* unknownPtr = *((IUnknown**)pThis + 1);
    if (!unknownPtr) {
        return;
    }

    FrameworkElement element = nullptr;
    unknownPtr->QueryInterface(winrt::guid_of<FrameworkElement>(),
                               winrt::put_abi(element));
    if (!element) {
        return;
    }

    try {
        Wh_Log(L"maxWidth=%f", element.MaxWidth());
        element.MaxWidth(std::numeric_limits<double>::infinity());
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(struct winrt::Windows::Foundation::Size __cdecl winrt::Taskbar::implementation::ThumbnailHelpers::GetScaledThumbnailSize(struct winrt::Windows::Foundation::Size,float))"},
            &ThumbnailHelpers_GetScaledThumbnailSize_Original,
            ThumbnailHelpers_GetScaledThumbnailSize_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskItemThumbnailView::OnApplyTemplate(void))"},
            &TaskItemThumbnailView_OnApplyTemplate_Original,
            TaskItemThumbnailView_OnApplyTemplate_Hook,
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
    g_settings.size = Wh_GetIntSetting(L"size");
    g_settings.useAbsoluteSize = Wh_GetIntSetting(L"useAbsoluteSize");
    g_settings.minWidth = Wh_GetIntSetting(L"minWidth");
    g_settings.minHeight = Wh_GetIntSetting(L"minHeight");
    g_settings.maxWidth = Wh_GetIntSetting(L"maxWidth");
    g_settings.maxHeight = Wh_GetIntSetting(L"maxHeight");
    g_settings.preserveAspectRatio = Wh_GetIntSetting(L"preserveAspectRatio");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

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
