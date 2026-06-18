// ==WindhawkMod==
// @id              vertical-omnibutton
// @name            Vertical OmniButton
// @description     Stacks Windows 11 wifi/volume/battery OmniButton vertically
// @version         1.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman/Windhawk-Vertical-OmniButton
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vertical OmniButton

Rearranges the Windows 11 system tray OmniButton (wifi, volume/sound, battery) from
horizontal layout to clean vertical stacking. 

This mod also includes support for the clock too. 
You gain granular control over the X-Y pixel location the items. 

## Screenshots

**Stacked mode** — battery percentage as a 4th row below the battery icon:

![Stacked mode](https://raw.githubusercontent.com/sb4ssman/Windhawk-Vertical-OmniButton/main/screenshot-stacked.png)

**Inline mode** — percentage shown within the battery icon slot:

![Inline mode](https://raw.githubusercontent.com/sb4ssman/Windhawk-Vertical-OmniButton/main/screenshot-inline.png)

**Off mode** — battery icon only, clean three-icon stack:

![Off mode](https://raw.githubusercontent.com/sb4ssman/Windhawk-Vertical-OmniButton/main/screenshot-off.png)

## How it works

Uses the Windows XAML Diagnostics API to watch the live XAML visual tree
(the same mechanism used by the Windows 11 Taskbar Styler). When OmniButton
elements appear, the mod forces `Orientation=Vertical` on the inner StackPanel
and positions each icon slot according to your settings.

## Usage

After enabling the mod, **restart explorer.exe**! You can do so easily using the built-in Restart
toggle in settings, or via Task Manager → Restart explorer.exe.

Enable **debug logging** to trace which XAML elements are being checked.

## Settings

- **Enable vertical arrangement** — master toggle for the vertical stack
- **Battery percentage** — Off / Inline / Stacked. Changing modes requires
  restarting explorer.exe to take effect.
- **Icon offsets** — each battery mode (Off / Inline / Stacked) has its own
  X/Y offsets for wifi, volume, and battery. Settings are labeled by mode.
- **Vertical clock** — splits the clock into three rows: time / day / date
- **Debug logging** — log XAML elements as they are added to the visual tree

## Windows 11 Taskbar Styler compatibility

This mod works alongside [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler).
If you have an existing `style.yaml` or theme for the Taskbar Styler, you can continue using it — the two mods
operate on different parts of the XAML tree and do not conflict.

## Related mods

These mods inspired this one and combine well with it for a fully customized taskbar:

- [Taskbar height and icon size](https://windhawk.net/mods/taskbar-icon-size) — resize the taskbar to give the vertical stack room to breathe
- [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization) — rich clock formatting options that complement the vertical layout
- [Multirow taskbar for Windows 11](https://windhawk.net/mods/taskbar-multirow) — span taskbar items across multiple rows
- [Taskbar tray icon spacing and grid](https://windhawk.net/mods/taskbar-notification-icon-spacing) — control spacing and grid layout of system tray icons
- [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler) — full XAML-level taskbar theming; existing style.yaml configs work alongside this mod

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- restartExplorer: false
  $name: Restart explorer.exe
  $description: "Save with this ON to restart explorer.exe immediately. A restart will fire on any save while this is ON, with a 10-second cooldown. Warning: closes all open File Explorer windows."
- enableVertical: true
  $name: Enable vertical arrangement
  $description: Enable/disable vertical stacking of wifi, volume, and battery icons
- batteryMode: "stacked"
  $name: Battery percentage
  $description: "Off: battery icon only.\nInline: percentage shown in the battery icon slot (3rd row).\nStacked: percentage as a separate 4th row below the battery icon.\nAll three modes require restarting explorer.exe after changing."
  $options:
    - "off": "Off — battery icon only"
    - "inline": "Inline — percentage in battery slot (3rd row)"
    - "stacked": "Stacked — percentage as 4th row below battery"
- verticalClock: true
  $name: Vertical clock (three rows)
  $description: Split the clock into three rows — time, day of week, date
- clockLineSpacing: 0
  $name: Clock row spacing
  $description: Extra vertical space between clock rows in pixels (minimum 0, maximum 20)
- clockAlignment: "center"
  $name: Clock alignment
  $description: "Aligns time, day, and date text. Only applies when Vertical clock is enabled."
  $options:
    - "left": "Left"
    - "center": "Center"
    - "right": "Right"
- wifiOffX: -2
  $name: "Off mode: Wifi X"
  $description: "Wifi horizontal offset when battery % is Off. Negative = left, positive = right."
- wifiOffY: 0
  $name: "Off mode: Wifi Y"
  $description: "Wifi vertical offset when battery % is Off. Negative = up, positive = down."
- volumeOffX: 0
  $name: "Off mode: Volume X"
  $description: "Volume horizontal offset when battery % is Off. Negative = left, positive = right."
- volumeOffY: 0
  $name: "Off mode: Volume Y"
  $description: "Volume vertical offset when battery % is Off. Negative = up, positive = down."
- batteryOffX: 2
  $name: "Off mode: Battery X"
  $description: "Battery icon horizontal offset when battery % is Off. Negative = left, positive = right."
- batteryOffY: 0
  $name: "Off mode: Battery Y"
  $description: "Battery icon vertical offset when battery % is Off. Negative = up, positive = down."
- wifiInlineX: -2
  $name: "Inline mode: Wifi X"
  $description: "Wifi horizontal offset in Inline mode. Negative = left, positive = right."
- wifiInlineY: 0
  $name: "Inline mode: Wifi Y"
  $description: "Wifi vertical offset in Inline mode. Negative = up, positive = down."
- volumeInlineX: 0
  $name: "Inline mode: Volume X"
  $description: "Volume horizontal offset in Inline mode. Negative = left, positive = right."
- volumeInlineY: 0
  $name: "Inline mode: Volume Y"
  $description: "Volume vertical offset in Inline mode. Negative = up, positive = down."
- batteryInlineX: 2
  $name: "Inline mode: Battery X"
  $description: "Battery slot horizontal offset in Inline mode. Negative = left, positive = right. Default: 2."
- batteryInlineY: 0
  $name: "Inline mode: Battery Y"
  $description: "Battery slot vertical offset in Inline mode. Negative = up, positive = down."
- batteryInlinePercentX: 0
  $name: "Inline mode: Battery percent X"
  $description: "Percentage text horizontal offset within the inline battery slot. Negative = left, positive = right."
- batteryInlinePercentY: 0
  $name: "Inline mode: Battery percent Y"
  $description: "Percentage text vertical offset within the inline battery slot. Negative = up, positive = down."
- wifiStackedX: -2
  $name: "Stacked mode: Wifi X"
  $description: "Wifi horizontal offset in Stacked mode. Negative = left, positive = right."
- wifiStackedY: 7
  $name: "Stacked mode: Wifi Y"
  $description: "Wifi vertical offset in Stacked mode. Negative = up, positive = down."
- volumeStackedX: 0
  $name: "Stacked mode: Volume X"
  $description: "Volume horizontal offset in Stacked mode. Negative = left, positive = right."
- volumeStackedY: 0
  $name: "Stacked mode: Volume Y"
  $description: "Volume vertical offset in Stacked mode. Negative = up, positive = down."
- batteryGlyphX: 8
  $name: "Stacked mode: Battery glyph X"
  $description: "Battery icon row horizontal offset in Stacked mode. Negative = left, positive = right. Default: 8."
- batteryGlyphY: -6
  $name: "Stacked mode: Battery glyph Y"
  $description: "Battery icon row vertical offset in Stacked mode. Negative = up, positive = down. Default: -6."
- batteryPercentX: 2
  $name: "Stacked mode: Battery percent X"
  $description: "Percentage row horizontal offset in Stacked mode. Negative = left, positive = right. Default: 2."
- batteryPercentY: -11
  $name: "Stacked mode: Battery percent Y"
  $description: "Percentage row vertical offset in Stacked mode. Negative = up, positive = down. Default: -11."
- debugLogging: false
  $name: Enable debug logging
  $description: Log XAML element types as they are added to the visual tree
*/
// ==/WindhawkModSettings==

#include <limits>
#include <unknwn.h>
#include <winrt/base.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>
#include <objidl.h>   // IObjectWithSite
#include <xamlom.h>   // IXamlDiagnostics, IVisualTreeService3, XAML diagnostics types

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ── Settings ───────────────────────────────────────────────────────────────

struct {
    bool enableVertical;
    bool verticalClock;
    int batteryMode;          // 0=off, 1=inline (3rd row), 2=stacked (4th row)
    int clockAlignment;       // 0=Left 1=Center 2=Right
    int clockLineSpacing;
    int wifiOffX,    wifiOffY;      // wifi CP offset in off mode
    int wifiInlineX, wifiInlineY;   // wifi CP offset in inline mode
    int wifiStackedX,wifiStackedY;  // wifi CP offset in stacked mode
    int volumeOffX,    volumeOffY;
    int volumeInlineX, volumeInlineY;
    int volumeStackedX,volumeStackedY;
    int batteryOffX,     batteryOffY;           // off: battery icon offset
    int batteryGlyphX,   batteryGlyphY;        // stacked: glyph row offset
    int batteryPercentX, batteryPercentY;      // stacked: % text row offset
    int batteryInlineX,  batteryInlineY;       // inline: battery CP offset
    int batteryInlinePercentX, batteryInlinePercentY; // inline: % text offset within CP
    bool debugLogging;
} g_settings;

bool g_unloading  = false;
bool g_ixdeStarted = false; // prevent double-injection from Wh_ModAfterInit
static DWORD g_lastRestartTick = 0; // tick-count when explorer was last restarted (0 = never)

void LoadSettings() {
    g_settings.enableVertical = Wh_GetIntSetting(L"enableVertical") != 0;
    g_settings.verticalClock  = Wh_GetIntSetting(L"verticalClock") != 0;
    {
        auto* bm = Wh_GetStringSetting(L"batteryMode");
        if (bm) {
            if (wcscmp(bm, L"inline") == 0)       g_settings.batteryMode = 1;
            else if (wcscmp(bm, L"stacked") == 0) g_settings.batteryMode = 2;
            else                                   g_settings.batteryMode = 0;
            Wh_FreeStringSetting(bm);
        } else {
            g_settings.batteryMode = 0;
        }
    }
    {
        auto* ca = Wh_GetStringSetting(L"clockAlignment");
        if (ca) {
            if (wcscmp(ca, L"left") == 0)        g_settings.clockAlignment = 0;
            else if (wcscmp(ca, L"right") == 0)  g_settings.clockAlignment = 2;
            else                                  g_settings.clockAlignment = 1;
            Wh_FreeStringSetting(ca);
        } else {
            g_settings.clockAlignment = 1;
        }
    }
    g_settings.clockLineSpacing   = Wh_GetIntSetting(L"clockLineSpacing");
    if (g_settings.clockLineSpacing < 0)  g_settings.clockLineSpacing = 0;
    if (g_settings.clockLineSpacing > 20) g_settings.clockLineSpacing = 20;
    auto clampOffset = [](int v) { return v < -20 ? -20 : v > 20 ? 20 : v; };
    g_settings.wifiOffX      = clampOffset(Wh_GetIntSetting(L"wifiOffX"));
    g_settings.wifiOffY      = clampOffset(Wh_GetIntSetting(L"wifiOffY"));
    g_settings.wifiInlineX   = clampOffset(Wh_GetIntSetting(L"wifiInlineX"));
    g_settings.wifiInlineY   = clampOffset(Wh_GetIntSetting(L"wifiInlineY"));
    g_settings.wifiStackedX  = clampOffset(Wh_GetIntSetting(L"wifiStackedX"));
    g_settings.wifiStackedY  = clampOffset(Wh_GetIntSetting(L"wifiStackedY"));
    g_settings.volumeOffX    = clampOffset(Wh_GetIntSetting(L"volumeOffX"));
    g_settings.volumeOffY    = clampOffset(Wh_GetIntSetting(L"volumeOffY"));
    g_settings.volumeInlineX = clampOffset(Wh_GetIntSetting(L"volumeInlineX"));
    g_settings.volumeInlineY = clampOffset(Wh_GetIntSetting(L"volumeInlineY"));
    g_settings.volumeStackedX= clampOffset(Wh_GetIntSetting(L"volumeStackedX"));
    g_settings.volumeStackedY= clampOffset(Wh_GetIntSetting(L"volumeStackedY"));
    g_settings.batteryOffX     = clampOffset(Wh_GetIntSetting(L"batteryOffX"));
    g_settings.batteryOffY     = clampOffset(Wh_GetIntSetting(L"batteryOffY"));
    g_settings.batteryGlyphX   = clampOffset(Wh_GetIntSetting(L"batteryGlyphX"));
    g_settings.batteryGlyphY   = clampOffset(Wh_GetIntSetting(L"batteryGlyphY"));
    g_settings.batteryPercentX = clampOffset(Wh_GetIntSetting(L"batteryPercentX"));
    g_settings.batteryPercentY = clampOffset(Wh_GetIntSetting(L"batteryPercentY"));
    g_settings.batteryInlineX  = clampOffset(Wh_GetIntSetting(L"batteryInlineX"));
    g_settings.batteryInlineY  = clampOffset(Wh_GetIntSetting(L"batteryInlineY"));
    g_settings.batteryInlinePercentX = clampOffset(Wh_GetIntSetting(L"batteryInlinePercentX"));
    g_settings.batteryInlinePercentY = clampOffset(Wh_GetIntSetting(L"batteryInlinePercentY"));
    g_settings.debugLogging    = Wh_GetIntSetting(L"debugLogging") != 0;
}

// Return the wifi/volume offsets for the currently active battery mode.
static int WifiX()   { return g_settings.batteryMode==1 ? g_settings.wifiInlineX   : g_settings.batteryMode==2 ? g_settings.wifiStackedX   : g_settings.wifiOffX;   }
static int WifiY()   { return g_settings.batteryMode==1 ? g_settings.wifiInlineY   : g_settings.batteryMode==2 ? g_settings.wifiStackedY   : g_settings.wifiOffY;   }
static int VolumeX() { return g_settings.batteryMode==1 ? g_settings.volumeInlineX : g_settings.batteryMode==2 ? g_settings.volumeStackedX : g_settings.volumeOffX; }
static int VolumeY() { return g_settings.batteryMode==1 ? g_settings.volumeInlineY : g_settings.batteryMode==2 ? g_settings.volumeStackedY : g_settings.volumeOffY; }

// ── CLSID for our TAP ─────────────────────────────────────────────────────
// {7E6D9C3A-2F1B-4A58-B3E7-1D5C8F9A2B6E}

static const CLSID CLSID_OmniButtonTAP =
    { 0x7E6D9C3A, 0x2F1B, 0x4A58, { 0xB3, 0xE7, 0x1D, 0x5C, 0x8F, 0x9A, 0x2B, 0x6E } };

// ── Cached element references for cleanup on unload ───────────────────────

static StackPanel       g_omniStackPanel{ nullptr };
static FrameworkElement g_omniButton{ nullptr };
static FrameworkElement g_wifiPresenter{ nullptr };     // slot 0
static FrameworkElement g_volumePresenter{ nullptr };   // slot 1
static FrameworkElement g_batteryPresenter{ nullptr };  // slot 2
static StackPanel       g_batteryInnerPanel{ nullptr };    // inner panel flipped to Vertical for 4th row
static FrameworkElement g_batteryInlinePercentFE{ nullptr }; // % TextBlock inside inline battery slot

static StackPanel       g_clockDayDatePanel{ nullptr };
static FrameworkElement g_clockButton{ nullptr };
static TextBlock        g_clockTimeTextBlock{ nullptr };
static TextBlock        g_clockDateTextBlock{ nullptr };

// ── Explorer restart ──────────────────────────────────────────────────────

static void RestartExplorer() {
    // Use CreateProcess with CREATE_NEW_PROCESS_GROUP so the cmd.exe is NOT a child
    // of explorer.exe and survives when explorer is killed.
    // ShellExecuteW inherits the process group and can be orphaned when explorer dies.
    WCHAR cmd[] =
        L"cmd.exe /c taskkill /f /im explorer.exe & timeout /t 2 /nobreak >nul & start explorer.exe";
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {};
    if (CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE,
                       CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        Wh_Log(L"[Restart] Explorer restart initiated");
    } else {
        Wh_Log(L"[Restart] CreateProcess failed (%lu)", GetLastError());
    }
}

// ── Battery XAML helpers ──────────────────────────────────────────────────

static bool HasBatteryDescendant(DependencyObject const& node, int depth = 0) {
    if (depth > 3) return false;
    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(node, i);
        if (!child) continue;
        std::wstring cls(winrt::get_class_name(child).c_str());
        if (cls.find(L"Battery") != std::wstring::npos) return true;
        if (HasBatteryDescendant(child, depth + 1)) return true;
    }
    return false;
}

static bool WalkBatteryTree(DependencyObject const& node, int depth) {
    if (depth > 5) return false;
    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(node, i);
        if (!child) continue;
        if (g_settings.debugLogging) {
            auto fe = child.try_as<FrameworkElement>();
            auto tb = child.try_as<TextBlock>();
            if (fe) Wh_Log(L"[Battery4] [%d/%d] %s \"%s\"%s",
                depth, i, winrt::get_class_name(fe).c_str(), fe.Name().c_str(),
                tb ? (std::wstring(L" text=") + tb.Text().c_str()).c_str() : L"");
        }
        auto sp = child.try_as<StackPanel>();
        if (sp && !sp.IsItemsHost() && sp.Orientation() == Orientation::Horizontal) {
            sp.Orientation(Orientation::Vertical);
            g_batteryInnerPanel = sp;
            Wh_Log(L"[Battery4] Flipped inner StackPanel at depth %d", depth);
            return true;
        }
        if (WalkBatteryTree(child, depth + 1)) return true;
    }
    return false;
}

// Walk the battery ContentPresenter's subtree. When showBatteryPercent is on,
// try to find a horizontal StackPanel (the icon+% layout) and flip it Vertical
// so the % appears below the battery icon glyph.
static void FlipBatteryLayout(FrameworkElement const& batteryCP) {
    if (!WalkBatteryTree(batteryCP, 0))
        Wh_Log(L"[Battery4] No horizontal StackPanel found — enable debug logging to see tree");
}

static void ApplyOffset(FrameworkElement const& fe, int x, int y) {
    if (x != 0 || y != 0) {
        TranslateTransform tt;
        tt.X(static_cast<double>(x));
        tt.Y(static_cast<double>(y));
        fe.RenderTransform(tt);
    } else {
        fe.ClearValue(UIElement::RenderTransformProperty());
    }
}

// Walk the battery CP subtree to find the inner SP's % child (child[1]) for inline mode.
// Does NOT flip orientation. Stores result in g_batteryInlinePercentFE and applies offset.
static bool WalkFindInlinePercent(DependencyObject const& node, int depth = 0) {
    if (depth > 5) return false;
    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(node, i);
        if (!child) continue;
        auto sp = child.try_as<StackPanel>();
        if (sp && !sp.IsItemsHost()) {
            if (VisualTreeHelper::GetChildrenCount(sp) >= 2) {
                auto pct = VisualTreeHelper::GetChild(sp, 1).try_as<FrameworkElement>();
                if (pct) {
                    g_batteryInlinePercentFE = pct;
                    ApplyOffset(pct, g_settings.batteryInlinePercentX, g_settings.batteryInlinePercentY);
                    Wh_Log(L"[Battery] Inline percent FE found and offset applied");
                }
            }
            return true; // SP found (children may still be empty; offset applied if present)
        }
        if (WalkFindInlinePercent(child, depth + 1)) return true;
    }
    return false;
}

// After flipping the battery inner StackPanel to Vertical, center each row and
// apply the user's X/Y offsets via TranslateTransform (post-layout, non-destructive).
static void SizeStackedBatteryRows(StackPanel const& innerSP) {
    int n = VisualTreeHelper::GetChildrenCount(innerSP);
    if (n >= 1) {
        auto glyph = VisualTreeHelper::GetChild(innerSP, 0).try_as<FrameworkElement>();
        if (glyph) {
            glyph.Width(32.0);
            glyph.Height(28.0);
            glyph.HorizontalAlignment(HorizontalAlignment::Center);
            ApplyOffset(glyph, g_settings.batteryGlyphX, g_settings.batteryGlyphY);
        }
    }
    if (n >= 2) {
        auto text = VisualTreeHelper::GetChild(innerSP, 1).try_as<FrameworkElement>();
        if (text) {
            text.HorizontalAlignment(HorizontalAlignment::Center);
            text.ClearValue(FrameworkElement::MarginProperty());
            ApplyOffset(text, g_settings.batteryPercentX, g_settings.batteryPercentY);
        }
    }
}

// Walk DOWN from node and set Height = NaN (XAML "Auto") on every FrameworkElement.
// Using NaN as a LOCAL value overrides style/template constraints (ClearValue would
// only remove our local value and revert to the template value, which may still be 28px).
static void ClearHeightDescendants(DependencyObject const& node, int depth = 0) {
    if (depth > 8) return;
    auto fe = node.try_as<FrameworkElement>();
    if (fe) fe.Height(std::numeric_limits<double>::quiet_NaN());
    int n = VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(node, i);
        if (child) ClearHeightDescendants(child, depth + 1);
    }
}

// ── Battery percentage (Windows registry toggle) ──────────────────────────

static DWORD g_originalBatteryPercent = MAXDWORD; // MAXDWORD = not yet saved

static constexpr LPCWSTR kAdvancedKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

static void ApplyBatteryPercent(int mode) {
    bool show = (mode > 0);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kAdvancedKey, 0,
                      KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        Wh_Log(L"[Battery] Failed to open registry key");
        return;
    }
    // Save original value the first time we touch the key.
    if (g_originalBatteryPercent == MAXDWORD) {
        DWORD val = 0, sz = sizeof(val), type = 0;
        if (RegQueryValueExW(hKey, L"TaskbarBatteryPercent", nullptr, &type,
                             reinterpret_cast<BYTE*>(&val), &sz) == ERROR_SUCCESS)
            g_originalBatteryPercent = val;
        else
            g_originalBatteryPercent = 0;
    }
    DWORD val = show ? 1 : 0;
    RegSetValueExW(hKey, L"TaskbarBatteryPercent", 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&val), sizeof(val));
    RegCloseKey(hKey);
    // Broadcast so explorer picks up the change on next restart.
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                       reinterpret_cast<LPARAM>(kAdvancedKey));
    Wh_Log(L"[Battery] TaskbarBatteryPercent set to %d (requires explorer restart)", val);
}

static void RestoreBatteryPercent() {
    if (g_originalBatteryPercent == MAXDWORD) return;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kAdvancedKey, 0,
                      KEY_WRITE, &hKey) != ERROR_SUCCESS) return;
    RegSetValueExW(hKey, L"TaskbarBatteryPercent", 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&g_originalBatteryPercent),
                   sizeof(g_originalBatteryPercent));
    RegCloseKey(hKey);
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                       reinterpret_cast<LPARAM>(kAdvancedKey));
    Wh_Log(L"[Battery] Restored TaskbarBatteryPercent to %lu", g_originalBatteryPercent);
    g_originalBatteryPercent = MAXDWORD;
}

// ── OmniButton height helpers ─────────────────────────────────────────────

// Free the OmniButton's height so it can grow to fit a 4th row (stacked mode),
// then force the taskbar layout panel to reallocate space for the larger button.
static void FreeOmniButtonHeight() {
    if (!g_omniButton) return;
    g_omniButton.Height(std::numeric_limits<double>::quiet_NaN());
    g_omniButton.InvalidateMeasure();
    // Invalidate the immediate parent so the taskbar slot expands.
    auto parent = VisualTreeHelper::GetParent(g_omniButton);
    if (parent) {
        auto parentUI = parent.try_as<UIElement>();
        if (parentUI) parentUI.InvalidateMeasure();
        // One more level up — the taskbar item container.
        auto grandparent = VisualTreeHelper::GetParent(parent);
        if (grandparent) {
            auto gpUI = grandparent.try_as<UIElement>();
            if (gpUI) gpUI.InvalidateMeasure();
        }
    }
}

static void RestoreOmniButtonHeight() {
    if (!g_omniButton) return;
    g_omniButton.ClearValue(FrameworkElement::HeightProperty());
    g_omniButton.InvalidateMeasure();
}

// ── Layout application ────────────────────────────────────────────────────

static TextAlignment ClockTextAlignment() {
    switch (g_settings.clockAlignment) {
        case 0:  return TextAlignment::Left;
        case 2:  return TextAlignment::Right;
        default: return TextAlignment::Center;
    }
}

static HorizontalAlignment ClockHorizontalAlignment() {
    switch (g_settings.clockAlignment) {
        case 0:  return HorizontalAlignment::Left;
        case 2:  return HorizontalAlignment::Right;
        default: return HorizontalAlignment::Center;
    }
}

static bool IsOmniButtonParent(DependencyObject const& start) {
    DependencyObject cur = start;
    for (int depth = 0; depth < 12; depth++) {
        auto p = VisualTreeHelper::GetParent(cur);
        if (!p) break;
        auto pfe = p.try_as<FrameworkElement>();
        if (!pfe) break;
        auto cls = winrt::get_class_name(pfe);
        auto nm  = pfe.Name();
        if (g_settings.debugLogging) {
            Wh_Log(L"[Layout] Parent[%d] class=%s name=%s", depth, cls.c_str(), nm.c_str());
        }
        std::wstring clsStr(cls.c_str());
        if (clsStr.find(L"OmniButton") != std::wstring::npos ||
            nm == L"ControlCenterButton") {
            g_omniButton = pfe;
            return true;
        }
        cur = p;
    }
    return false;
}

static void ApplyLayout(StackPanel const& sp) {
    if (g_omniStackPanel) return;  // already found the right one, ignore others
    if (!sp.IsItemsHost()) return;
    if (!IsOmniButtonParent(sp)) return;

    g_omniStackPanel = sp;
    sp.Orientation(Orientation::Vertical);
    sp.VerticalAlignment(VerticalAlignment::Center);
    sp.Spacing(0);

    if (g_omniButton) {
        auto ctrl = g_omniButton.try_as<Control>();
        if (ctrl) {
            ctrl.HorizontalContentAlignment(HorizontalAlignment::Center);
            ctrl.VerticalContentAlignment(VerticalAlignment::Center);
        }
        // In stacked mode, the 4th row pushes past the button's template height.
        if (g_settings.batteryMode == 2)
            FreeOmniButtonHeight();
    }

    // Size each icon slot explicitly so they render correctly in vertical layout.
    {
        int n = VisualTreeHelper::GetChildrenCount(sp);
        for (int i = 0; i < n; i++) {
            auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
            if (child) {
                child.Width(32.0);
                child.Height(28.0);
                child.HorizontalAlignment(HorizontalAlignment::Center);
                auto cp = child.try_as<ContentPresenter>();
                if (cp) cp.HorizontalContentAlignment(HorizontalAlignment::Center);
                if (i == 0) { g_wifiPresenter  = child; ApplyOffset(child, WifiX(),   WifiY());   }
                if (i == 1) { g_volumePresenter = child; ApplyOffset(child, VolumeX(), VolumeY()); }
            }
        }
    }

    // Always find battery presenter so settings changes can update its height later.
    {
        int n = VisualTreeHelper::GetChildrenCount(sp);
        for (int i = 0; i < n; i++) {
            auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
            if (!child) continue;
            if (HasBatteryDescendant(child)) {
                g_batteryPresenter = child;
                Wh_Log(L"[Battery] Battery slot at index %d (mode=%d)", i, g_settings.batteryMode);
                if (g_settings.batteryMode == 1) {
                    // Inline: same height as other slots; width auto to fit glyph+text.
                    child.Width(std::numeric_limits<double>::quiet_NaN());
                    child.Height(28.0);
                    ApplyOffset(child, g_settings.batteryInlineX, g_settings.batteryInlineY);
                    WalkFindInlinePercent(child);
                } else if (g_settings.batteryMode == 2) {
                    // Stacked: width auto (% text can exceed 32px); height auto for both rows.
                    child.Width(std::numeric_limits<double>::quiet_NaN());
                    // NaN every height in the subtree so the CP can grow past 28px.
                    ClearHeightDescendants(child);
                    FlipBatteryLayout(child);
                    if (g_batteryInnerPanel) {
                        g_batteryInnerPanel.Spacing(0.0);
                        SizeStackedBatteryRows(g_batteryInnerPanel);
                    }
                } else {
                    // Off mode: battery icon only; apply offset from settings.
                    ApplyOffset(child, g_settings.batteryOffX, g_settings.batteryOffY);
                }
                break;
            }
        }
        if (!g_batteryPresenter)
            Wh_Log(L"[Battery] No battery slot found (desktop without battery?)");
    }

    Wh_Log(L"[Layout] Applied vertical layout (children=%d)", VisualTreeHelper::GetChildrenCount(sp));
}

// ── Clock three-row layout ────────────────────────────────────────────────

static bool IsClockParent(DependencyObject const& start) {
    DependencyObject cur = start;
    for (int depth = 0; depth < 16; depth++) {
        auto p = VisualTreeHelper::GetParent(cur);
        if (!p) break;
        auto pfe = p.try_as<FrameworkElement>();
        if (!pfe) { cur = p; continue; }
        auto cls = winrt::get_class_name(pfe);
        if (g_settings.debugLogging) {
            Wh_Log(L"[Clock] Parent[%d] class=%s", depth, cls.c_str());
        }
        std::wstring clsStr(cls.c_str());
        if (clsStr.find(L"Clock")    != std::wstring::npos ||
            clsStr.find(L"DateTime") != std::wstring::npos) {
            g_clockButton = pfe;
            return true;
        }
        cur = p;
    }
    return false;
}

// The outer clock StackPanel is already Vertical (time on row 1, "Day Date" on row 2).
// We make it three rows by forcing the date TextBlock (last child) to wrap at word
// boundaries with a narrow MaxWidth, splitting "Sunday 04/19/2026" onto two lines.
static void ApplyClockLayout(StackPanel const& sp) {
    if (g_clockDayDatePanel) return;
    if (sp.IsItemsHost()) return;
    int n = VisualTreeHelper::GetChildrenCount(sp);
    if (n < 2 || n > 4) return;
    if (!IsClockParent(sp)) return;

    g_clockDayDatePanel = sp;

    sp.Spacing(g_settings.clockLineSpacing);

    // First child is the time TextBlock ("3:39 PM").
    auto firstChild = VisualTreeHelper::GetChild(sp, 0).try_as<TextBlock>();
    if (firstChild) {
        firstChild.HorizontalAlignment(ClockHorizontalAlignment());
        firstChild.TextAlignment(ClockTextAlignment());
        g_clockTimeTextBlock = firstChild;
    }

    // Last child is the "DayOfWeek Date" TextBlock.
    auto lastChild = VisualTreeHelper::GetChild(sp, n - 1);
    auto dateTB = lastChild.try_as<TextBlock>();
    if (dateTB) {
        dateTB.TextWrapping(TextWrapping::WrapWholeWords);
        dateTB.MaxWidth(80.0);
        dateTB.HorizontalAlignment(ClockHorizontalAlignment());
        dateTB.TextAlignment(ClockTextAlignment());
        // NOTE: StackPanel::Spacing (time↔day) and TextBlock::LineHeight (day↔date)
        // use different measurement models. Spacing is pixel-exact; LineHeight includes
        // the font's internal leading/line-gap, so the same numeric value will look
        // visually tighter between day↔date than between time↔day. This is fundamental
        // and cannot be made perfectly uniform with these two properties.
        if (g_settings.clockLineSpacing > 0)
            dateTB.LineHeight(dateTB.FontSize() + g_settings.clockLineSpacing);
        else
            dateTB.ClearValue(TextBlock::LineHeightProperty());
        g_clockDateTextBlock = dateTB;
        Wh_Log(L"[Clock] Wrapping date TextBlock for three-row layout");
    } else {
        Wh_Log(L"[Clock] Last clock child is not a TextBlock (class=%s)",
               winrt::get_class_name(lastChild).c_str());
    }
}

// ── XAML diagnostics — VisualTreeWatcher ──────────────────────────────────

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
{
public:
    explicit VisualTreeWatcher(winrt::com_ptr<IUnknown> const& site)
        : m_diagnostics(site.as<IXamlDiagnostics>())
    {
        Wh_Log(L"[TAP] VisualTreeWatcher created");
        AddRef();
        HANDLE t = CreateThread(nullptr, 0, [](LPVOID p) -> DWORD {
            auto self = reinterpret_cast<VisualTreeWatcher*>(p);
            HRESULT hr = self->m_diagnostics.as<IVisualTreeService3>()
                             ->AdviseVisualTreeChange(self);
            Wh_Log(L"[TAP] AdviseVisualTreeChange: %08X", hr);
            self->Release();
            return 0;
        }, this, 0, nullptr);
        if (t) CloseHandle(t);
        else    Release();
    }

    void Unadvise() {
        m_diagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    }

private:
    winrt::com_ptr<IXamlDiagnostics> m_diagnostics;

    winrt::Windows::Foundation::IInspectable FromHandle(InstanceHandle handle) {
        winrt::Windows::Foundation::IInspectable obj;
        m_diagnostics->GetIInspectableFromHandle(
            handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj)));
        return obj;
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation,
        VisualElement element,
        VisualMutationType mutationType) override try
    {
        if (mutationType != VisualMutationType::Add) return S_OK;
        if (g_unloading) return S_OK;
        if (!g_settings.enableVertical && !g_settings.verticalClock) return S_OK;

        if (g_settings.debugLogging && element.Type) {
            Wh_Log(L"[TAP] Add: %s", element.Type);
        }

        auto insp = FromHandle(element.Handle);
        if (!insp) return S_OK;

        try {
            auto sp = insp.try_as<StackPanel>();
            if (sp) {
                if (!g_omniStackPanel) ApplyLayout(sp);
                if (!g_clockDayDatePanel && g_settings.verticalClock) ApplyClockLayout(sp);

                // Deferred stacked battery flip: the inner StackPanel is often added to
                // the tree AFTER ApplyLayout runs (XAML builds the tree incrementally).
                // When it arrives here, parents ARE wired so GetParent works correctly.
                // We also handle the case where g_batteryPresenter was never set because
                // the IsItemsHost StackPanel had no children when ApplyLayout ran.
                if (g_settings.batteryMode == 2 && !sp.IsItemsHost() && !g_batteryInnerPanel
                    && g_omniStackPanel) {
                    // Walk up from sp to find its direct-child ancestor of g_omniStackPanel.
                    DependencyObject cur = sp;
                    FrameworkElement directChild{ nullptr };
                    for (int d = 0; d < 10; d++) {
                        auto p = VisualTreeHelper::GetParent(cur);
                        if (!p) break;
                        auto parentSP = p.try_as<StackPanel>();
                        if (parentSP && parentSP == g_omniStackPanel) {
                            directChild = cur.try_as<FrameworkElement>();
                            break;
                        }
                        // Also short-circuit if we already know the battery presenter.
                        if (g_batteryPresenter) {
                            auto pfe = p.try_as<FrameworkElement>();
                            if (pfe && pfe == g_batteryPresenter) {
                                directChild = g_batteryPresenter;
                                break;
                            }
                        }
                        cur = p;
                    }
                    if (directChild) {
                        if (!g_batteryPresenter) {
                            g_batteryPresenter = directChild;
                            Wh_Log(L"[Battery4] Battery presenter inferred via deferred walk");
                        }
                        g_batteryPresenter.Width(std::numeric_limits<double>::quiet_NaN());
                        sp.Orientation(Orientation::Vertical);
                        g_batteryInnerPanel = sp;
                        ClearHeightDescendants(g_batteryPresenter);
                        sp.Spacing(0.0);
                        SizeStackedBatteryRows(sp);
                        FreeOmniButtonHeight();
                        Wh_Log(L"[Battery4] Deferred flip of battery inner StackPanel");
                    }
                }

                return S_OK;
            }

            // Size any new FrameworkElement added directly to the OmniButton StackPanel.
            // This fires when battery/wifi/volume CPs arrive late (SP had 0 children at ApplyLayout).
            if (g_omniStackPanel && g_settings.enableVertical) {
                auto fe = insp.try_as<FrameworkElement>();
                if (fe) {
                    auto parent = VisualTreeHelper::GetParent(fe);
                    if (parent) {
                        auto parentSP = parent.try_as<StackPanel>();
                        if (parentSP && parentSP == g_omniStackPanel) {
                            // Find slot index to identify wifi(0)/volume(1)/battery(2).
                            int slotIdx = -1;
                            int nc = VisualTreeHelper::GetChildrenCount(parentSP);
                            for (int j = 0; j < nc; j++) {
                                if (VisualTreeHelper::GetChild(parentSP, j) == fe) { slotIdx = j; break; }
                            }

                            bool isBattery = (g_batteryPresenter && fe == g_batteryPresenter)
                                          || (!g_batteryPresenter && HasBatteryDescendant(fe));
                            if (isBattery && !g_batteryPresenter) g_batteryPresenter = fe;

                            fe.HorizontalAlignment(HorizontalAlignment::Center);
                            auto cp = fe.try_as<ContentPresenter>();
                            if (cp) cp.HorizontalContentAlignment(HorizontalAlignment::Center);

                            if (isBattery && g_settings.batteryMode == 1) {
                                fe.Width(std::numeric_limits<double>::quiet_NaN());
                                fe.Height(28.0);
                                ApplyOffset(fe, g_settings.batteryInlineX, g_settings.batteryInlineY);
                                WalkFindInlinePercent(fe);
                            } else if (isBattery && g_settings.batteryMode == 2) {
                                fe.Width(std::numeric_limits<double>::quiet_NaN());
                                fe.Height(std::numeric_limits<double>::quiet_NaN());
                            } else {
                                fe.Width(32.0);
                                fe.Height(28.0);
                                if (slotIdx == 0) { g_wifiPresenter  = fe; ApplyOffset(fe, WifiX(),   WifiY());   }
                                if (slotIdx == 1) { g_volumePresenter = fe; ApplyOffset(fe, VolumeX(), VolumeY()); }
                                if (isBattery)    { ApplyOffset(fe, g_settings.batteryOffX, g_settings.batteryOffY); }
                            }
                            Wh_Log(L"[Layout] Sized new OmniButton child: %s battery=%d mode=%d",
                                   winrt::get_class_name(fe).c_str(),
                                   (int)isBattery, g_settings.batteryMode);
                        }
                    }
                }
            }

        } catch (...) {}

        return S_OK;
    }
    catch (...) { return S_OK; }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle, VisualElementState, LPCWSTR) noexcept override
    { return S_OK; }
};

// ── XAML diagnostics — OmniButtonTAP (IObjectWithSite) ────────────────────

static winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

class OmniButtonTAP
    : public winrt::implements<OmniButtonTAP, IObjectWithSite, winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override try {
        if (g_visualTreeWatcher) {
            g_visualTreeWatcher->Unadvise();
            g_visualTreeWatcher = nullptr;
        }
        if (pUnkSite) {
            winrt::com_ptr<IUnknown> site;
            site.copy_from(pUnkSite);
            g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
        }
        // Balance the LoadLibrary that InitializeXamlDiagnosticsEx does on our DLL.
        FreeLibrary(GetCurrentModuleHandleHelper());
        return S_OK;
    }
    catch (...) { return winrt::to_hresult(); }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) noexcept override
    { return E_NOTIMPL; }

private:
    static HMODULE GetCurrentModuleHandleHelper() {
        HMODULE h = nullptr;
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandleHelper), &h);
        return h;
    }
};

// ── IClassFactory for OmniButtonTAP ───────────────────────────────────────

struct OmniButtonTAPFactory
    : winrt::implements<OmniButtonTAPFactory, IClassFactory>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(
        IUnknown* pOuter, REFIID riid, void** ppv) noexcept override
    {
        if (pOuter) return CLASS_E_NOAGGREGATION;
        try {
            auto tap = winrt::make_self<OmniButtonTAP>();
            return tap->QueryInterface(riid, ppv);
        } catch (...) { return winrt::to_hresult(); }
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

// InitializeXamlDiagnosticsEx loads our DLL and calls DllGetClassObject to
// get the factory for CLSID_OmniButtonTAP.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
extern "C" __declspec(dllexport)
HRESULT WINAPI DllGetClassObject(GUID const& clsid, GUID const& iid, void** ppv) {
    Wh_Log(L"[TAP] DllGetClassObject called");
    if (!IsEqualGUID(clsid, CLSID_OmniButtonTAP))
        return CLASS_E_CLASSNOTAVAILABLE;
    try {
        return winrt::make_self<OmniButtonTAPFactory>()->QueryInterface(iid, ppv);
    } catch (...) { return winrt::to_hresult(); }
}
#pragma clang diagnostic pop

// ── TAP injection ─────────────────────────────────────────────────────────

static HMODULE GetCurrentModuleHandle() {
    HMODULE h = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &h);
    return h;
}

using PFN_IXDE = HRESULT(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, CLSID, LPCWSTR);

static void InjectOmniButtonTAP() {
    HMODULE hXaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
    if (!hXaml) { Wh_Log(L"[TAP] Windows.UI.Xaml.dll not loaded"); return; }

    auto ixde = reinterpret_cast<PFN_IXDE>(
        GetProcAddress(hXaml, "InitializeXamlDiagnosticsEx"));
    if (!ixde) { Wh_Log(L"[TAP] InitializeXamlDiagnosticsEx not found"); return; }

    HMODULE hSelf = GetCurrentModuleHandle();
    WCHAR selfPath[MAX_PATH];
    if (!GetModuleFileNameW(hSelf, selfPath, MAX_PATH)) {
        Wh_Log(L"[TAP] GetModuleFileName failed (%lu)", GetLastError());
        return;
    }

    Wh_Log(L"[TAP] Injecting from: %s", selfPath);

    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (int i = 1; i <= 10000 && hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND); i++) {
        WCHAR conn[64];
        wsprintf(conn, L"VisualDiagConnection%d", i);
        hr = ixde(conn, GetCurrentProcessId(), L"", selfPath,
                  CLSID_OmniButtonTAP, nullptr);
    }
    Wh_Log(L"[TAP] InitializeXamlDiagnosticsEx result: %08X", hr);
    if (SUCCEEDED(hr)) g_ixdeStarted = true;
}

// ── LoadLibraryExW hook ────────────────────────────────────────────────────

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (hModule && !g_visualTreeWatcher && lpLibFileName) {
        const wchar_t* name = wcsrchr(lpLibFileName, L'\\');
        name = name ? (name + 1) : lpLibFileName;
        if (_wcsicmp(name, L"Windows.UI.Xaml.dll") == 0) {
            Wh_Log(L"[LoadLib] Windows.UI.Xaml.dll loaded — injecting TAP");
            InjectOmniButtonTAP();
        }
    }

    return hModule;
}

// ── Windhawk lifecycle ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Vertical OmniButton v1.51.0");
    // If restartExplorer is already true when we load, "consume" it so the first
    // settings save doesn't immediately re-fire.  After 30 s the user can save
    // again to trigger a fresh restart without having to toggle the setting off first.
    if (Wh_GetIntSetting(L"restartExplorer") != 0)
        g_lastRestartTick = GetTickCount();
    LoadSettings();
    ApplyBatteryPercent(g_settings.batteryMode);

    HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
    auto pLoadLibraryExW = kernelbase
        ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelbase, "LoadLibraryExW"))
        : nullptr;

    if (pLoadLibraryExW) {
        Wh_SetFunctionHook((void*)pLoadLibraryExW,
                            (void*)LoadLibraryExW_Hook,
                            (void**)&LoadLibraryExW_Original);
        Wh_Log(L"[Init] LoadLibraryExW hook queued");
    }

    HMODULE hXaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
    Wh_Log(L"[Init] Windows.UI.Xaml.dll at init: %p", (void*)hXaml);
    if (hXaml) {
        InjectOmniButtonTAP();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // Only retry if IXDE was never successfully initiated (e.g., DLL wasn't loaded yet at init).
    if (!g_ixdeStarted) {
        HMODULE hXaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
        if (hXaml) {
            Wh_Log(L"[AfterInit] Retrying inject...");
            InjectOmniButtonTAP();
        }
    }
    Wh_Log(L"[AfterInit] ixdeStarted=%d watcher=%p",
           (int)g_ixdeStarted, (void*)g_visualTreeWatcher.get());
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->Unadvise();
        g_visualTreeWatcher = nullptr;
    }

    // NOTE: intentionally NOT calling RestoreBatteryPercent() here.
    // Windows reads TaskbarBatteryPercent at explorer STARTUP. If we reset the
    // registry to 0 in Wh_ModUninit, the new explorer process reads 0 before
    // Wh_ModInit has a chance to re-apply it, permanently clearing the % display.
    // The registry is left as-is; Windhawk will re-apply the correct value on
    // the next Wh_ModInit call.

    auto sp      = g_omniStackPanel;
    auto btn     = g_omniButton;
    auto wifi    = g_wifiPresenter;
    auto vol     = g_volumePresenter;
    auto bp      = g_batteryPresenter;
    auto bip     = g_batteryInnerPanel;
    auto bipct   = g_batteryInlinePercentFE;
    auto cdp     = g_clockDayDatePanel;
    auto timeTB  = g_clockTimeTextBlock;
    auto dateTB  = g_clockDateTextBlock;
    g_omniStackPanel          = nullptr;
    g_omniButton              = nullptr;
    g_wifiPresenter           = nullptr;
    g_volumePresenter         = nullptr;
    g_batteryPresenter        = nullptr;
    g_batteryInnerPanel       = nullptr;
    g_batteryInlinePercentFE  = nullptr;
    g_clockDayDatePanel       = nullptr;
    g_clockButton             = nullptr;
    g_clockTimeTextBlock      = nullptr;
    g_clockDateTextBlock      = nullptr;

    // Helper lambda that does the actual XAML restoration.
    auto doCleanup = [sp, btn, wifi, vol, bp, bip, bipct, cdp, timeTB, dateTB]() {
        try {
            if (sp) {
                sp.ClearValue(StackPanel::OrientationProperty());
                sp.ClearValue(StackPanel::SpacingProperty());
                sp.ClearValue(FrameworkElement::VerticalAlignmentProperty());
                int n = VisualTreeHelper::GetChildrenCount(sp);
                for (int i = 0; i < n; i++) {
                    auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
                    if (child) {
                        child.ClearValue(FrameworkElement::WidthProperty());
                        child.ClearValue(FrameworkElement::HeightProperty());
                        child.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                        auto cp = child.try_as<ContentPresenter>();
                        if (cp) cp.ClearValue(ContentPresenter::HorizontalContentAlignmentProperty());
                    }
                }
            }
        } catch (...) {}
        try {
            if (btn) {
                btn.ClearValue(FrameworkElement::WidthProperty());
                RestoreOmniButtonHeight();
                auto ctrl = btn.try_as<Control>();
                if (ctrl) {
                    ctrl.ClearValue(Control::HorizontalContentAlignmentProperty());
                    ctrl.ClearValue(Control::VerticalContentAlignmentProperty());
                }
            }
        } catch (...) {}
        try { if (wifi) wifi.ClearValue(UIElement::RenderTransformProperty()); } catch (...) {}
        try { if (vol)  vol.ClearValue(UIElement::RenderTransformProperty());  } catch (...) {}
        try {
            if (bp) {
                bp.ClearValue(FrameworkElement::HeightProperty());
                bp.ClearValue(UIElement::RenderTransformProperty());
            }
        } catch (...) {}
        try { if (bipct) bipct.ClearValue(UIElement::RenderTransformProperty()); } catch (...) {}
        try {
            if (bip) {
                bip.ClearValue(StackPanel::OrientationProperty());
                bip.ClearValue(StackPanel::SpacingProperty());
                int bipN = VisualTreeHelper::GetChildrenCount(bip);
                for (int i = 0; i < bipN; i++) {
                    auto fe = VisualTreeHelper::GetChild(bip, i).try_as<FrameworkElement>();
                    if (fe) {
                        fe.ClearValue(FrameworkElement::WidthProperty());
                        fe.ClearValue(FrameworkElement::HeightProperty());
                        fe.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                        fe.ClearValue(FrameworkElement::MarginProperty());
                        fe.ClearValue(UIElement::RenderTransformProperty());
                    }
                }
            }
        } catch (...) {}
        try { if (cdp) cdp.ClearValue(StackPanel::SpacingProperty()); } catch (...) {}
        try {
            if (timeTB) {
                timeTB.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                timeTB.ClearValue(TextBlock::TextAlignmentProperty());
            }
        } catch (...) {}
        try {
            if (dateTB) {
                dateTB.ClearValue(TextBlock::TextWrappingProperty());
                dateTB.ClearValue(FrameworkElement::MaxWidthProperty());
                dateTB.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                dateTB.ClearValue(TextBlock::TextAlignmentProperty());
                dateTB.ClearValue(TextBlock::LineHeightProperty());
            }
        } catch (...) {}
    };

    // Try synchronous first (works if uninit is already on the UI thread).
    doCleanup();

    // Also schedule async dispatch as belt-and-suspenders for wrong-thread cases.
    // Bump DLL refcount so the lambda is safe after Windhawk's FreeLibrary.
    // Need any XAML element to get the dispatcher.
    auto dispSrc = sp    ? sp.try_as<FrameworkElement>()
                        : (dateTB ? dateTB.try_as<FrameworkElement>() : nullptr);
    if (!dispSrc) return;
    auto disp = dispSrc.Dispatcher();
    if (!disp) return;

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(&Wh_ModUninit), &hSelf);

    try {
        auto _ = disp.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
            [doCleanup, hSelf]() {
                Wh_Log(L"[Uninit] Async cleanup running");
                doCleanup();
                Wh_Log(L"[Uninit] Async cleanup done");
                if (hSelf) FreeLibrary(hSelf);
            });
    } catch (...) {
        if (hSelf) FreeLibrary(hSelf);
    }
}

void Wh_ModSettingsChanged() {
    // Restart explorer whenever restartExplorer=true, debounced to 10 s.
    if (Wh_GetIntSetting(L"restartExplorer") != 0) {
        DWORD now = GetTickCount();
        if (now - g_lastRestartTick > 10000) {
            g_lastRestartTick = now;
            // CRITICAL: load settings and write the battery registry value BEFORE
            // killing explorer.  The new process reads TaskbarBatteryPercent at startup;
            // if we set it after the fact (in Wh_ModInit) it is already too late.
            LoadSettings();
            ApplyBatteryPercent(g_settings.batteryMode);
            RestartExplorer();
            return; // explorer is restarting; no point updating XAML
        }
    }

    LoadSettings();
    Wh_Log(L"[Settings] Updated");

    // Battery % is a registry toggle — apply immediately on any thread.
    ApplyBatteryPercent(g_settings.batteryMode);

    // XAML properties must be set on the UI thread. Get a dispatcher from any live element.
    FrameworkElement dispSrc{ nullptr };
    if (g_omniStackPanel)     dispSrc = g_omniStackPanel.try_as<FrameworkElement>();
    if (!dispSrc && g_clockDateTextBlock) dispSrc = g_clockDateTextBlock.try_as<FrameworkElement>();
    if (!dispSrc) { Wh_Log(L"[Settings] No XAML elements yet — restart explorer to apply"); return; }
    auto disp = dispSrc.Dispatcher();
    if (!disp) return;

    // Capture all state needed on the UI thread.
    auto sp     = g_omniStackPanel;
    auto btn    = g_omniButton;
    auto bp     = g_batteryPresenter;
    auto bip    = g_batteryInnerPanel;
    auto cdp    = g_clockDayDatePanel;
    auto timeTB = g_clockTimeTextBlock;
    auto dateTB = g_clockDateTextBlock;
    bool enableV      = g_settings.enableVertical;
    bool enableC      = g_settings.verticalClock;
    int  batteryMode  = g_settings.batteryMode;
    int  clockAlign   = g_settings.clockAlignment;
    int  clockSpacing = g_settings.clockLineSpacing;

    try {
        disp.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
        [sp, btn, bp, bip, cdp, timeTB, dateTB,
         enableV, enableC, batteryMode, clockAlign, clockSpacing]() mutable
        {
            // OmniButton layout
            try {
                if (sp) {
                    if (enableV) {
                        sp.Orientation(Orientation::Vertical);
                        sp.VerticalAlignment(VerticalAlignment::Center);
                        sp.Spacing(0);
                        if (btn) {
                            auto ctrl = btn.try_as<Control>();
                            if (ctrl) {
                                ctrl.HorizontalContentAlignment(HorizontalAlignment::Center);
                                ctrl.VerticalContentAlignment(VerticalAlignment::Center);
                            }
                            // Free button height in stacked mode; restore otherwise.
                            if (batteryMode == 2)
                                FreeOmniButtonHeight();
                            else
                                RestoreOmniButtonHeight();
                        }
                        {
                            int n = VisualTreeHelper::GetChildrenCount(sp);
                            for (int i = 0; i < n; i++) {
                                auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
                                if (child) {
                                    child.Width(32.0);
                                    child.Height(28.0);
                                    child.HorizontalAlignment(HorizontalAlignment::Center);
                                    auto cp = child.try_as<ContentPresenter>();
                                    if (cp) cp.HorizontalContentAlignment(HorizontalAlignment::Center);
                                    if (i == 0) ApplyOffset(child, WifiX(),   WifiY());
                                    if (i == 1) ApplyOffset(child, VolumeX(), VolumeY());
                                }
                            }
                        }
                        // Find battery slot if needed.
                        if (!bp && sp) {
                            int nc = VisualTreeHelper::GetChildrenCount(sp);
                            for (int i = 0; i < nc; i++) {
                                auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
                                if (child && HasBatteryDescendant(child)) {
                                    bp = child;
                                    g_batteryPresenter = child;
                                    break;
                                }
                            }
                        }
                        if (bp) {
                            if (batteryMode == 1) {
                                bp.Width(std::numeric_limits<double>::quiet_NaN());
                                bp.Height(28.0);
                                ApplyOffset(bp, g_settings.batteryInlineX, g_settings.batteryInlineY);
                                if (!g_batteryInlinePercentFE) WalkFindInlinePercent(bp);
                                else ApplyOffset(g_batteryInlinePercentFE,
                                                 g_settings.batteryInlinePercentX,
                                                 g_settings.batteryInlinePercentY);
                            } else if (batteryMode == 2) {
                                bp.Width(std::numeric_limits<double>::quiet_NaN());
                                ClearHeightDescendants(bp);
                                if (!g_batteryInnerPanel) {
                                    FlipBatteryLayout(bp);
                                    bip = g_batteryInnerPanel;
                                }
                                auto innerPanel = g_batteryInnerPanel;
                                if (innerPanel) {
                                    innerPanel.Spacing(0.0);
                                    SizeStackedBatteryRows(innerPanel);
                                }
                            } else {
                                // Off mode: sizing loop above applied Width=32/Height=28.
                                // Do NOT ClearValue here (would let template render % if registry=1).
                                ApplyOffset(bp, g_settings.batteryOffX, g_settings.batteryOffY);
                            }
                        }
                        if (bip && batteryMode != 2) {
                            bip.ClearValue(StackPanel::OrientationProperty());
                            bip.ClearValue(StackPanel::SpacingProperty());
                            int bipN = VisualTreeHelper::GetChildrenCount(bip);
                            for (int i = 0; i < bipN; i++) {
                                auto fe = VisualTreeHelper::GetChild(bip, i).try_as<FrameworkElement>();
                                if (fe) {
                                    fe.ClearValue(FrameworkElement::HeightProperty());
                                    fe.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                                    fe.ClearValue(FrameworkElement::MarginProperty());
                                }
                            }
                        }
                    } else {
                        sp.ClearValue(StackPanel::OrientationProperty());
                        sp.ClearValue(StackPanel::SpacingProperty());
                        sp.ClearValue(FrameworkElement::VerticalAlignmentProperty());
                        {
                            int n = VisualTreeHelper::GetChildrenCount(sp);
                            for (int i = 0; i < n; i++) {
                                auto child = VisualTreeHelper::GetChild(sp, i).try_as<FrameworkElement>();
                                if (child) {
                                    child.ClearValue(FrameworkElement::WidthProperty());
                                    child.ClearValue(FrameworkElement::HeightProperty());
                                    child.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                                    child.ClearValue(UIElement::RenderTransformProperty());
                                    auto cp = child.try_as<ContentPresenter>();
                                    if (cp) cp.ClearValue(ContentPresenter::HorizontalContentAlignmentProperty());
                                }
                            }
                        }
                        if (btn) {
                            auto ctrl = btn.try_as<Control>();
                            if (ctrl) {
                                ctrl.ClearValue(Control::HorizontalContentAlignmentProperty());
                                ctrl.ClearValue(Control::VerticalContentAlignmentProperty());
                            }
                            btn.ClearValue(FrameworkElement::WidthProperty());
                            RestoreOmniButtonHeight();
                        }
                        if (bp) {
                            bp.ClearValue(FrameworkElement::HeightProperty());
                            bp.ClearValue(UIElement::RenderTransformProperty());
                        }
                        if (bip) bip.ClearValue(StackPanel::OrientationProperty());
                    }
                }
            } catch (...) {}

            // Clock layout
            auto clockTA = clockAlign == 0 ? TextAlignment::Left
                         : clockAlign == 2 ? TextAlignment::Right
                         : TextAlignment::Center;
            auto clockHA = clockAlign == 0 ? HorizontalAlignment::Left
                         : clockAlign == 2 ? HorizontalAlignment::Right
                         : HorizontalAlignment::Center;
            try {
                if (enableC) {
                    if (cdp)    cdp.Spacing(clockSpacing);
                    if (timeTB) { timeTB.HorizontalAlignment(clockHA); timeTB.TextAlignment(clockTA); }
                    if (dateTB) {
                        dateTB.TextWrapping(TextWrapping::WrapWholeWords);
                        dateTB.MaxWidth(80.0);
                        dateTB.HorizontalAlignment(clockHA);
                        dateTB.TextAlignment(clockTA);
                        if (clockSpacing > 0)
                            dateTB.LineHeight(dateTB.FontSize() + clockSpacing);
                        else
                            dateTB.ClearValue(TextBlock::LineHeightProperty());
                    }
                } else {
                    if (cdp)    cdp.ClearValue(StackPanel::SpacingProperty());
                    if (timeTB) { timeTB.ClearValue(FrameworkElement::HorizontalAlignmentProperty()); timeTB.ClearValue(TextBlock::TextAlignmentProperty()); }
                    if (dateTB) {
                        dateTB.ClearValue(TextBlock::TextWrappingProperty());
                        dateTB.ClearValue(FrameworkElement::MaxWidthProperty());
                        dateTB.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                        dateTB.ClearValue(TextBlock::TextAlignmentProperty());
                        dateTB.ClearValue(TextBlock::LineHeightProperty());
                    }
                }
            } catch (...) {}
        });
    } catch (...) {}
}
