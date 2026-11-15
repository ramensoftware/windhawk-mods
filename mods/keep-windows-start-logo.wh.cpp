// ==WindhawkMod==
// @id              keep-windows-start-logo
// @name            Keep Windows Start Logo Visible
// @description     Prevents macOS dock mods and taskbar themes from hiding the Windows 11 Start button logo.
// @version         1.0.0
// @author          Srihith
// @github          https://github.com/srihithsreddy-bit
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keep Windows Start Logo Visible

This mod hooks into the Windows 11 XAML taskbar (`Taskbar.ExperienceToggleButton#StartButton`) and ensures the Windows Start logo remains visible even when using macOS-style dock mods or custom taskbar themes that attempt to collapse it.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <atomic>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;

std::atomic<bool> g_unloading{false};

// Recursively searches the XAML visual tree for StartButton and forces Visibility::Visible
void EnforceStartButtonVisibility(DependencyObject const& parent) {
    if (!parent) return;

    int childrenCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childrenCount; i++) {
        DependencyObject child = VisualTreeHelper::GetChild(parent, i);
        if (!child) continue;

        if (auto elem = child.try_as<FrameworkElement>()) {
            if (elem.Name() == L"StartButton") {
                if (elem.Visibility() != Visibility::Visible) {
                    elem.Visibility(Visibility::Visible);
                    Wh_Log(L"Restored StartButton visibility to Visible.");
                }
                return;
            }
        }
        EnforceStartButtonVisibility(child);
    }
}

using UIElement_put_Visibility_t = HRESULT(WINAPI*)(void* pThis, Visibility value);
UIElement_put_Visibility_t UIElement_put_Visibility_Original;

HRESULT WINAPI UIElement_put_Visibility_Hook(void* pThis, Visibility value) {
    if (!g_unloading && value == Visibility::Collapsed && pThis) {
        try {
            winrt::Windows::Foundation::IInspectable inspectable{nullptr};
            winrt::copy_from_abi(inspectable, pThis);

            if (auto elem = inspectable.try_as<FrameworkElement>()) {
                if (elem.Name() == L"StartButton") {
                    Wh_Log(L"Blocked attempt to hide StartButton; keeping Visible.");
                    return UIElement_put_Visibility_Original(pThis, Visibility::Visible);
                }
            }
        } catch (...) {
            // Safely ignore non-FrameworkElement COM calls
        }
    }
    return UIElement_put_Visibility_Original(pThis, value);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Keep Windows Start Logo Visible...");

    HMODULE hXaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
    if (!hXaml) {
        hXaml = LoadLibraryW(L"Windows.UI.Xaml.dll");
    }

    if (!hXaml) {
        Wh_Log(L"Failed to locate Windows.UI.Xaml.dll");
        return FALSE;
    }

    // Hook UIElement::put_Visibility via symbol lookup
    WindhawkUtils::SYMBOL_HOOK xamlHooks[] = {
        {
            {L"public: virtual long __cdecl DirectUI::UIElement::put_Visibility(enum ABI::Windows::UI::Xaml::Visibility)"},
            &UIElement_put_Visibility_Original
