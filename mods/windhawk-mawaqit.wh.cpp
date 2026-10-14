// ==WindhawkMod==
// @id windhawk-mawaqit
// @name Mawaqit Prayer Clock
// @description Mawaqit prayer clock with live countdown, prayer schedule popup, and customizable themes.
// @version 0.6.9-rc1
// @author HHeitlager
// @github https://github.com/hheitlager
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -luuid -lgdi32 -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mawaqit Prayer Clock
Mawaqit prayer clock with live countdown, prayer schedule popup, and customizable themes.


## Features
- Upcoming prayer name, scheduled time, and live countdown
- Clickable prayer-schedule popup with optional auto-close timer
- 13 appearance themes, including Windows / System, Traditional Islamic, Catppuccin Mocha, Emberveil, Dracula, Nord, Gruvbox Dark, Tokyo Night, Solarized Dark and Light, and Custom
- Independent custom colors for the pill and popup
- Visibility switches for the prayer label, countdown, and popup
- Five selectable taskbar positions, including experimental dynamic centering
- Native XAML taskbar integration with reserved layout space for anchored positions
- Automatic or manual clock height, four-sided margins and padding, tray gap, corner radius, and font size
- Background schedule updates through native WinHTTP, with configurable refresh interval

## Network access and cached schedules
This mod connects to **https://mawaqit.net** over HTTPS to retrieve the public
prayer schedule for the mosque identifier configured in Settings. The request
is an HTTP GET for `https://mawaqit.net/<language>/<mosque-identifier>`;
no account or authentication is required. The server can observe the request
and normal connection information such as your IP address.

The schedule is stored in Windhawk's mod-specific storage. A valid cached
schedule is reused when Explorer restarts, until the configured refresh interval
expires (24 hours by default; adjustable from 1 to 168 hours). The mod also
requests a fresh schedule when the mosque or refresh interval changes.
If a download fails, it retries after one hour. An empty, expired, mismatched,
or unusable cache causes a download at startup. The mod reads schedule data
from the public Mawaqit webpage, so changes to that page can interrupt updates.

- Live settings updates without restarting Explorer

## Screenshots
Mawaqit Prayer Clock supports multiple appearance themes to match your Windows 11 desktop.

Windows System Default | Traditional Islamic |
|:---:|:---:|
![Windows](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Windows-System-Default.png) | ![Traditional](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Traditional-Islamic.png) |

| Catppucchin Mocha | Emberveil |
|:---:|:---:|
| ![Catppucchin](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Catppucchin-Moca.png) | ![Emberveil](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/Mawaqit-Emberveil.png) |

## Desktop Integration
See how Mawaqit Prayer Clock integrates into a fully customized Windows 11 desktop.
![Desktop](https://raw.githubusercontent.com/hheitlager/windhawk-mawaqit/main/screenshots/My-Desktop.png)

## Configuration
Choose a mosque identifier or Mawaqit URL in Settings.
Select an appearance theme and customize the widget as desired.
Custom color settings only apply when the Custom theme is selected.
Set popup auto-close to 0 to keep the popup open until dismissed.

## More information
For installation instructions, additional screenshots,
troubleshooting, and release information, visit:

[GitHub Project](https://github.com/hheitlager/windhawk-mawaqit)

## License
MIT License.

## Disclaimer
This is an unofficial community project and is not affiliated
with or endorsed by Mawaqit.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- General:
  - Mosque: al-haram-makkah-saudi-arabia
    $name: Mawaqit mosque identifier or URL
  - Theme: system
    $name: Theme
    $options:
    - system: Windows / System
    - traditional: Traditional Islamic
    - catppuccin: Catppuccin Mocha
    - emberveil: Emberveil
    - emberveil-midnight: Emberveil Midnight
    - emerald-mosque: Emerald Mosque
    - dracula: Dracula
    - nord: Nord
    - gruvbox: Gruvbox Dark
    - tokyo-night: Tokyo Night
    - solarized-dark: Solarized Dark
    - solarized-light: Solarized Light
    - custom: Custom
  - Position: before-tray
    $name: Taskbar position
    $description: Choose where the clock appears on the taskbar. Dynamic center may overlap applications when the taskbar is full.
    $options:
    - before-tray: Before system tray
    - after-tray: After system tray
    - before-icons: Before taskbar icons
    - after-icons: After taskbar icons
    - center: Dynamic center (experimental)
  $name: General
- Display:
  - ShowNextPrayer: true
    $name: Show next prayer
  - ShowCountdown: true
    $name: Show countdown
  - EnablePopup: true
    $name: Show popup
  - PopupAutoCloseSeconds: 5
    $name: Popup auto-close (seconds, 0 = disabled)
    $description: Automatically close popup after 1–300 seconds. Opening it again resets the timer.
  $name: Popup settings
- AdvancedCustomization:
  - ModSettingsAdvanced:
    - RefreshHours: 24
      $name: Refresh interval (hours)
      $description: Choose how often the prayer schedule is updated from Mawaqit (1–168 hours).
    $name: Mod settings
  - LayoutSettings:
    - Width: 0
      $name: Width
      $description: Choose how wide the clock appears (180–400 px). Values outside this range use 200 px.
    - PillHeight: 0
      $name: Height
      $description: Set to 0 to adjust automatically to the taskbar, or choose a height between 1 and 100 px.
    - Margins: "0, 0, 0, 0"
      $name: Margin (left, top, right, bottom)
      $description: Move the clock left, right, up, or down (−100 to 100 px per side).
    - Padding: "0, 0, 0, 0"
      $name: Padding (left, top, right, bottom)
      $description: Adjust the space between the clock edges and its text (0–100 px per side).
    - TrayGap: 5
      $name: Gap before system tray
      $description: Choose how much space appears between the clock and system tray (0–100 px).
    - CornerRadius: 13
      $name: Corner radius
      $description: Choose how rounded the clock's corners appear (0–30 px). Set to 0 for square corners; higher values create rounder corners.
    - FontSize: 15
      $name: Fontsize
      $description: Adjust the clock's text size (10–24 px). This does not affect the popup.
    $name: Layout settings
  - CustomThemeSettings:
    - CustomPillBackground: "#211E32"
      $name: Pill background
    - CustomPillText: "#F5E6C8"
      $name: Pill prayer text
    - CustomPillSeparator: "#B8A98A"
      $name: Pill separator
    - CustomPillCountdown: "#FFBC5E"
      $name: Pill countdown
    - CustomPopupBackground: "#1D1D2D"
      $name: Popup background
    - CustomPopupBorder: "#3B3B58"
      $name: Popup border
    - CustomPopupText: "#D3D0E8"
      $name: Popup main text
    - CustomPopupMuted: "#B8B3D1"
      $name: Popup secondary text
    - CustomPopupRule: "#424158"
      $name: Popup dividers
    - CustomPopupFooter: "#A29DBB"
      $name: Popup footer
    $name: Custom theme settings
  $name: Advanced customization
*/
// ==/WindhawkModSettings==

#include <windows.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <windhawk_utils.h>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstdint>
#include <winhttp.h>
#include <map>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cwchar>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <cstdio>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

namespace folder_menus_slot_lease {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::GridUnitType;
using winrt::Windows::UI::Xaml::Controls::ColumnDefinition;
using winrt::Windows::UI::Xaml::Controls::Grid;
using winrt::Windows::UI::Xaml::Controls::Panel;
using winrt::Windows::UI::Xaml::Controls::StackPanel;

enum class Anchor {
    BeforeIcons,
    AfterShowDesktop,
};

// Which layout contract the live tray panel follows.
enum class Kind {
    Unsupported,  // some other Panel: do not guess its layout semantics.
    Columns,      // Grid. A slot is a column index.
    Order,        // StackPanel. A slot is a child index.
};

struct Lease {
    std::wstring markerName;
    int slot = -1;
    Kind kind = Kind::Unsupported;
    bool createdBaseColumn = false;
};

inline Kind Classify(FrameworkElement const& parent) {
    if (!parent)
        return Kind::Unsupported;
    if (parent.try_as<Grid>())
        return Kind::Columns;
    if (parent.try_as<StackPanel>())
        return Kind::Order;
    return Kind::Unsupported;
}

// Class name of an unexpected panel, so a mod can log what it actually got
// instead of reporting a bare "not found" for an element that is right there.
inline std::wstring ClassName(FrameworkElement const& element) {
    if (!element)
        return L"(null)";
    try {
        return std::wstring(winrt::get_class_name(element));
    } catch (...) {
        return L"(unknown)";
    }
}

// Named direct children, for logging when an anchor cannot be resolved. A tray
// restructure shows up here as missing or renamed names, which is the one thing
// a user's debug log otherwise cannot tell us.
inline std::wstring DescribeChildren(Panel const& parent) {
    if (!parent)
        return L"(none)";
    std::wstring names;
    try {
        for (auto const& child : parent.Children()) {
            auto element = child.try_as<FrameworkElement>();
            if (!element || element.Name().empty())
                continue;
            if (!names.empty())
                names += L", ";
            names += element.Name();
        }
    } catch (...) {
        return L"(unreadable)";
    }
    return names.empty() ? L"(no named children)" : names;
}

inline FrameworkElement FindDirectChild(Panel const& parent,
                                        wchar_t const* name) {
    if (!parent || !name)
        return nullptr;
    for (auto const& child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name)
            return element;
    }
    return nullptr;
}

inline int IndexOfChild(Panel const& parent, FrameworkElement const& child) {
    if (!parent || !child)
        return -1;
    for (uint32_t i = 0; i < parent.Children().Size(); ++i) {
        if (parent.Children().GetAt(i).try_as<FrameworkElement>() == child)
            return static_cast<int>(i);
    }
    return -1;
}

inline bool ResolveSlot(Panel const& parent, Anchor anchor, int& slot) {
    Kind kind = Classify(parent);
    if (kind == Kind::Unsupported)
        return false;

    if (anchor == Anchor::BeforeIcons) {
        slot = 0;
        return true;
    }

    wchar_t const* referenceName = nullptr;
    bool after = false;
    switch (anchor) {
        case Anchor::AfterShowDesktop:
            referenceName = L"ShowDesktopStack";
            after = true;
            break;
        case Anchor::BeforeIcons:
            break;
    }

    auto reference = FindDirectChild(parent, referenceName);
    if (!reference)
        return false; // Never silently turn an unavailable anchor into slot 0.

    if (kind == Kind::Columns) {
        slot = Grid::GetColumn(reference) +
               (after ? std::max(1, Grid::GetColumnSpan(reference)) : 0);
        return true;
    }

    // Order: the reference's own child index is the slot. There is no span to
    // step over -- a StackPanel child occupies exactly one position.
    int index = IndexOfChild(parent, reference);
    if (index < 0)
        return false;
    slot = index + (after ? 1 : 0);
    return true;
}

inline bool Release(Panel const& parent, Lease& lease);

inline bool AcquireAt(Panel const& parent, int slot,
                      std::wstring const& markerName, Lease& lease) {
    Kind kind = Classify(parent);
    if (kind == Kind::Unsupported || slot < 0 || markerName.empty())
        return false;

    // A marker with this name already in the tray is either this caller's own
    // live lease — refuse, acquiring twice would strand the first slot — or a
    // leftover from an instance whose teardown never reached the UI thread.
    // Refusing the leftover would block every later apply until Explorer
    // restarts, so release it and take the slot fresh, correcting the
    // requested slot if the released one sat before it.
    if (auto stale = FindDirectChild(parent, markerName.c_str())) {
        if (lease.markerName == markerName)
            return false;
        int staleSlot = kind == Kind::Columns
                            ? Grid::GetColumn(stale)
                            : IndexOfChild(parent, stale);
        Lease leftover{markerName, staleSlot, kind};
        if (!Release(parent, leftover))
            return false;
        if (staleSlot >= 0 && staleSlot < slot)
            --slot;
    }

    Grid marker;
    marker.Name(markerName);
    marker.Width(0.0);
    marker.Height(0.0);
    marker.IsHitTestVisible(false);

    if (kind == Kind::Columns) {
        auto grid = parent.try_as<Grid>();
        if (!grid)
            return false;
        // A Grid without ColumnDefinitions uses one implicit full-width column.
        // Materialize that implicit column before inserting a lease column;
        // otherwise the repeater is shifted to column 1 in a ONE-column grid.
        bool createdBaseColumn = grid.ColumnDefinitions().Size() == 0;
        if (createdBaseColumn) {
            ColumnDefinition base;
            base.Width({1.0, GridUnitType::Star});
            grid.ColumnDefinitions().Append(base);
        }
        ColumnDefinition definition;
        definition.Width({1.0, GridUnitType::Auto});
        slot = std::min(slot, static_cast<int>(grid.ColumnDefinitions().Size()));
        if (static_cast<uint32_t>(slot) < grid.ColumnDefinitions().Size())
            grid.ColumnDefinitions().InsertAt(slot, definition);
        else
            grid.ColumnDefinitions().Append(definition);

        for (auto const& child : grid.Children()) {
            auto element = child.try_as<FrameworkElement>();
            if (!element) continue;
            int start = Grid::GetColumn(element);
            int span = Grid::GetColumnSpan(element);
            if (start >= slot)
                Grid::SetColumn(element, start + 1);
            else if (start + span > slot)
                Grid::SetColumnSpan(element, span + 1);
        }

        Grid::SetColumn(marker, slot);
        grid.Children().Append(marker);
        lease.createdBaseColumn = createdBaseColumn;
    } else {
        // Order: no column is created or owned. The marker simply holds the
        // position, and PlaceChild drops the content next to it.
        uint32_t index = std::min(static_cast<uint32_t>(slot),
                                  parent.Children().Size());
        parent.Children().InsertAt(index, marker);
        slot = static_cast<int>(index);
    }

    lease = {markerName, slot, kind, lease.createdBaseColumn};
    return true;
}

// Live index of the lease marker. Other mods inject and remove siblings around
// us, so the acquire-time index is a hint, never the truth at removal time.
inline bool FindMarker(Panel const& parent, Lease const& lease,
                       uint32_t& index) {
    if (!parent || lease.markerName.empty())
        return false;
    for (uint32_t i = 0; i < parent.Children().Size(); ++i) {
        auto element = parent.Children().GetAt(i).try_as<FrameworkElement>();
        if (element && element.Name() == lease.markerName) {
            index = i;
            return true;
        }
    }
    return false;
}

// Put mod content into the leased slot. On a Grid the content joins the leased
// column; on a StackPanel it is inserted directly after the marker, so the
// marker's position is the content's position.
inline bool PlaceChild(Panel const& parent, Lease const& lease,
                       FrameworkElement const& content) {
    if (!parent || !content || lease.kind == Kind::Unsupported)
        return false;

    uint32_t markerIndex = 0;
    if (!FindMarker(parent, lease, markerIndex))
        return false;

    if (lease.kind == Kind::Columns) {
        auto marker = parent.Children().GetAt(markerIndex)
                          .try_as<FrameworkElement>();
        Grid::SetColumn(content, marker ? Grid::GetColumn(marker) : lease.slot);
        parent.Children().Append(content);
        return true;
    }

    parent.Children().InsertAt(markerIndex + 1, content);
    return true;
}

inline bool Release(Panel const& parent, Lease& lease) {
    if (!parent || lease.markerName.empty())
        return false;

    uint32_t markerIndex = 0;
    if (!FindMarker(parent, lease, markerIndex))
        return false;

    if (lease.kind == Kind::Order) {
        parent.Children().RemoveAt(markerIndex);
        lease = {};
        return true;
    }

    auto grid = parent.try_as<Grid>();
    auto marker =
        parent.Children().GetAt(markerIndex).try_as<FrameworkElement>();
    int liveColumn = marker ? Grid::GetColumn(marker) : lease.slot;
    if (!grid || liveColumn < 0)
        return false;

    grid.Children().RemoveAt(markerIndex);
    if (static_cast<uint32_t>(liveColumn) < grid.ColumnDefinitions().Size())
        grid.ColumnDefinitions().RemoveAt(liveColumn);

    for (auto const& child : grid.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element) continue;
        int start = Grid::GetColumn(element);
        int span = Grid::GetColumnSpan(element);
        if (start > liveColumn)
            Grid::SetColumn(element, start - 1);
        else if (start < liveColumn && start + span > liveColumn)
            Grid::SetColumnSpan(element, std::max(1, span - 1));
    }

    // Restore the original implicit-column layout only for columns we created.
    if (lease.createdBaseColumn && grid.ColumnDefinitions().Size() == 1)
        grid.ColumnDefinitions().Clear();
    lease = {};
    return true;
}

}  // namespace folder_menus_slot_lease
namespace folder_menus_taskbar_window {

// ---- Window discovery -------------------------------------------------------

inline HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(window, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(window, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(parameter) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

// Shell_TrayWnd can be recreated inside Explorer. A cache is useful only while
// it names a live window; otherwise rediscover before dispatch or teardown.
inline HWND ResolveTaskbarWnd(HWND cached) {
    if (cached && IsWindow(cached))
        return cached;
    return FindCurrentProcessTaskbarWnd();
}

}  // namespace folder_menus_taskbar_window
namespace folder_menus_dispatch {

// ---- UI-thread marshalling --------------------------------------------------
//
// XAML may only be touched from the thread that owns it. This posts work onto
// the taskbar's thread with a CALLWNDPROC hook and a private registered
// message, and reports whether the callback actually ran — a caller that
// assumes it did will corrupt its own state when the dispatch failed.

using ThreadProc = void (*)(void*);
using ExceptionLogFn = void (*)(PCWSTR context);

inline ExceptionLogFn g_logException = nullptr;

// Point this at the mod's logger once in Wh_ModInit so failures inside a UI
// callback are reported in the mod's own voice.
inline void SetExceptionLogger(ExceptionLogFn logger) {
    g_logException = logger;
}

inline bool Invoke(ThreadProc proc, void* parameter) {
    try {
        proc(parameter);
        return true;
    } catch (...) {
        if (g_logException) g_logException(L"UI callback");
    }
    return false;
}

struct Dispatch {
    ThreadProc proc;
    void* parameter;
    bool succeeded = false;
    // Every concurrent caller installs its own hook with this same proc, and
    // each hook instance sees every message equal to g_dispatchMessage. With
    // two dispatches in flight, both hooks are in the chain when either
    // message arrives, so without this each callback would run twice. The
    // hooks run one after another on the UI thread, so a plain flag suffices.
    bool ran = false;
};

// The private message this mod dispatches on. Set before the hook is
// installed, and read by the hook proc to recognise its own message.
//
// A CALLWNDPROC HOOK SEES EVERY MESSAGE SENT TO EVERY WINDOW ON THE TASKBAR'S
// UI THREAD. `lParam` for all of those is arbitrary — an integer, a flag, a
// pointer to something else entirely. So the message MUST be checked first,
// against a value that does not come from lParam, and only then may lParam be
// treated as a Dispatch*. Reading anything out of lParam before that check
// dereferences whatever happened to be in the message and takes Explorer down
// with it.
//
// Atomic because the caller may be the retry thread while the hook
// proc runs on the taskbar's UI thread. RegisterWindowMessageW returns the
// same value for the same string for the lifetime of the session, so this
// settles on one value immediately and never changes again.
inline std::atomic<UINT> g_dispatchMessage{0};

// messageName must embed WH_MOD_ID, so two mods cannot collide on the message.
inline bool RunFromWindowThread(HWND window, ThreadProc proc, void* parameter,
                                PCWSTR messageName) {
    UINT message = RegisterWindowMessageW(messageName);
    if (!message) return false;

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) return Invoke(proc, parameter);

    g_dispatchMessage.store(message, std::memory_order_release);

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto const* call = reinterpret_cast<CWPSTRUCT const*>(lParam);
                // Message first. Only our own private message carries a
                // Dispatch* in lParam; everything else carries something we
                // must not touch.
                UINT expected =
                    g_dispatchMessage.load(std::memory_order_acquire);
                if (expected && call->message == expected) {
                    if (auto* dispatch =
                            reinterpret_cast<Dispatch*>(call->lParam);
                        dispatch && !dispatch->ran) {
                        dispatch->ran = true;
                        dispatch->succeeded =
                            Invoke(dispatch->proc, dispatch->parameter);
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Dispatch dispatch{proc, parameter};
    SendMessageW(window, message, 0, reinterpret_cast<LPARAM>(&dispatch));
    UnhookWindowsHookEx(hook);
    return dispatch.succeeded;
}

}  // namespace folder_menus_dispatch
namespace folder_menus_taskbar_xaml {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::XamlRoot;

// ---- XamlRoot ---------------------------------------------------------------

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
using Ref_count_base_Decref_t = void(WINAPI*)(void*);
using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);

inline CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
inline TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
inline Ref_count_base_Decref_t Ref_count_base_Decref_Original = nullptr;
inline TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
inline void* CTaskBand_ITaskListWndSite_vftable = nullptr;

// The mod's rebuild callback, invoked after Explorer rebuilds the taskbar.
inline void (*g_onTaskbarRebuilt)() = nullptr;

inline void WINAPI TrayUI_StartTaskbar_Hook(void* self) {
    TrayUI_StartTaskbar_Original(self);
    try {
        if (g_onTaskbarRebuilt) g_onTaskbarRebuilt();
    } catch (...) {
        if (folder_menus_dispatch::g_logException)
            folder_menus_dispatch::g_logException(L"TrayUI::StartTaskbar hook");
    }
}

inline bool HookTaskbarSymbols(void (*onTaskbarRebuilt)()) {
    g_onTaskbarRebuilt = onTaskbarRebuilt;
    HMODULE taskbar = LoadLibraryExW(L"taskbar.dll", nullptr,
                                     LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbar) return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Ref_count_base_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
    };
    return WindhawkUtils::HookSymbols(taskbar, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

// The FrameworkElement lives at an offset inside TaskbarHost that MOVES
// between Windows builds, so it is read out of TaskbarHost::FrameHeight's
// prologue at runtime rather than hardcoded.
inline size_t FrameworkElementOffset() {
    size_t offset = 0x10;
#if defined(_M_X64)
    BYTE const* code =
        reinterpret_cast<BYTE const*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        offset = code[7];
    }
#elif defined(_M_ARM64)
    DWORD const* code =
        reinterpret_cast<DWORD const*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0xD503237F && (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD && (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        offset = (code[3] >> 12) & 0xFF;
    }
#else
#error "Unsupported architecture"
#endif
    return offset;
}

inline XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original || !Ref_count_base_Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable)
        return nullptr;

    HWND taskSwWnd = (HWND)GetProp(taskbarWnd, L"TaskbandHWND");
    if (!taskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(taskSwWnd, 0);
    if (!taskBand) return nullptr;

    void* site = taskBand;
    for (int i = 0; *(void**)site != CTaskBand_ITaskListWndSite_vftable; ++i) {
        if (i == 20) return nullptr;
        site = (void**)site + 1;
    }

    void* host[2]{};
    CTaskBand_GetTaskbarHost_Original(site, host);
    if (!host[0] || !host[1]) {
        if (host[1]) Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }

    auto* unknown =
        *(IUnknown**)((BYTE*)host[0] + FrameworkElementOffset());
    if (!unknown) {
        Ref_count_base_Decref_Original(host[1]);
        return nullptr;
    }
    FrameworkElement element = nullptr;
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(element));
    auto result = element ? element.XamlRoot() : nullptr;
    Ref_count_base_Decref_Original(host[1]);
    return result;
}

}  // namespace folder_menus_taskbar_xaml

// Windhawk-managed per-mod cache, removed when the mod is uninstalled.
static std::wstring CacheDir() {
    WCHAR path[MAX_PATH]{};
    if (!Wh_GetModStoragePath(path, ARRAYSIZE(path))) return L"";
    return path;
}

namespace NativeUpdater {

static void fail(const char* s){throw std::runtime_error(s);}
static std::string narrow(const std::wstring& s){if(s.empty())return {};int n=WideCharToMultiByte(CP_UTF8,0,s.data(),(int)s.size(),nullptr,0,nullptr,nullptr);std::string r(n,0);WideCharToMultiByte(CP_UTF8,0,s.data(),(int)s.size(),r.data(),n,nullptr,nullptr);return r;}
static std::string quote(const std::string& s){std::string o="\"";for(unsigned char c:s){switch(c){case '"':o+="\\\"";break;case '\\':o+="\\\\";break;case '\n':o+="\\n";break;case '\r':o+="\\r";break;case '\t':o+="\\t";break;default:if(c<32){char b[7];snprintf(b,sizeof(b),"\\u%04x",c);o+=b;}else o+=(char)c;}}return o+"\"";}

struct J {
 enum Kind{Null,Str,Arr,Obj,Other} kind=Null;
 std::string s;std::vector<J> a;std::map<std::string,J> o;
 const J& key(const std::string& k)const{auto it=o.find(k);if(it==o.end())fail("Missing JSON key");return it->second;}
 const J& idx(size_t i)const{if(i>=a.size())fail("JSON array index missing");return a[i];}
};

struct Parser {
 const std::string& s;size_t p=0;int depth=0;
 void ws(){while(p<s.size()&&(s[p]==' '||s[p]=='\r'||s[p]=='\n'||s[p]=='\t'))p++;}
 void expect(char c){ws();if(p>=s.size()||s[p++]!=c)fail("Malformed JSON");}
 static void utf8(std::string& out,unsigned cp){if(cp<128)out+=(char)cp;else if(cp<2048){out+=(char)(0xC0|(cp>>6));out+=(char)(0x80|(cp&63));}else if(cp<65536){out+=(char)(0xE0|(cp>>12));out+=(char)(0x80|((cp>>6)&63));out+=(char)(0x80|(cp&63));}else{out+=(char)(0xF0|(cp>>18));out+=(char)(0x80|((cp>>12)&63));out+=(char)(0x80|((cp>>6)&63));out+=(char)(0x80|(cp&63));}}
 unsigned hex4(){if(p+4>s.size())fail("Invalid Unicode escape");unsigned v=0;for(int i=0;i<4;i++){char c=s[p++];v<<=4;if(c>='0'&&c<='9')v+=c-'0';else if(c>='a'&&c<='f')v+=c-'a'+10;else if(c>='A'&&c<='F')v+=c-'A'+10;else fail("Invalid Unicode hex");}return v;}
 std::string str(){expect('"');std::string o;bool done=false;while(p<s.size()){unsigned char c=s[p++];if(c=='"'){done=true;break;}if(c<32)fail("Control char in JSON string");if(c!='\\'){o+=(char)c;continue;}if(p>=s.size())fail("Invalid JSON escape");char e=s[p++];switch(e){case '"':case '\\':case '/':o+=e;break;case 'n':o+='\n';break;case 'r':o+='\r';break;case 't':o+='\t';break;case 'b':o+='\b';break;case 'f':o+='\f';break;case 'u':{unsigned cp=hex4();if(cp>=0xD800&&cp<=0xDBFF){if(p+2>s.size()||s[p++]!='\\'||s[p++]!='u')fail("Bad surrogate");unsigned low=hex4();if(low<0xDC00||low>0xDFFF)fail("Bad low surrogate");cp=0x10000+((cp-0xD800)<<10)+(low-0xDC00);}else if(cp>=0xDC00&&cp<=0xDFFF)fail("Bad surrogate");utf8(o,cp);break;}default:fail("Unknown JSON escape");}}if(!done)fail("Unterminated JSON string");return o;}
 J value(){ws();if(++depth>80)fail("JSON nesting too deep");if(p>=s.size())fail("Truncated JSON");J v;char c=s[p];if(c=='"'){v.kind=J::Str;v.s=str();}else if(c=='{'){v.kind=J::Obj;p++;ws();if(p<s.size()&&s[p]=='}')p++;else for(;;){std::string k=str();expect(':');v.o.emplace(std::move(k),value());ws();if(p>=s.size())fail("Truncated object");char t=s[p++];if(t=='}')break;if(t!=',')fail("Invalid object separator");}}else if(c=='['){v.kind=J::Arr;p++;ws();if(p<s.size()&&s[p]==']')p++;else for(;;){v.a.push_back(value());ws();if(p>=s.size())fail("Truncated array");char t=s[p++];if(t==']')break;if(t!=',')fail("Invalid array separator");}}else{v.kind=J::Other;size_t b=p;while(p<s.size()&&s[p]!=','&&s[p]!=']'&&s[p]!='}'&&s[p]!=' '&&s[p]!='\r'&&s[p]!='\n')p++;if(b==p)fail("Invalid JSON token");v.s=s.substr(b,p-b);}depth--;return v;}
};

struct Http {HINTERNET h=nullptr;explicit Http(HINTERNET x):h(x){if(!h)fail("WinHTTP failed");}~Http(){if(h)WinHttpCloseHandle(h);}Http(const Http&)=delete;Http& operator=(const Http&)=delete;};

static std::string fetch(const std::wstring& path){
 Http session(WinHttpOpen(L"EmberveilMawaqit/0.5",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0));
 WinHttpSetTimeouts(session.h,5000,5000,15000,15000);
 Http conn(WinHttpConnect(session.h,L"mawaqit.net",INTERNET_DEFAULT_HTTPS_PORT,0));
 Http req(WinHttpOpenRequest(conn.h,L"GET",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE));
 DWORD redirects=WINHTTP_OPTION_REDIRECT_POLICY_NEVER;WinHttpSetOption(req.h,WINHTTP_OPTION_REDIRECT_POLICY,&redirects,sizeof(redirects));
 if(!WinHttpSendRequest(req.h,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0)||!WinHttpReceiveResponse(req.h,nullptr))fail("HTTP request failed");
 DWORD status=0,len=sizeof(status);if(!WinHttpQueryHeaders(req.h,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,WINHTTP_HEADER_NAME_BY_INDEX,&status,&len,WINHTTP_NO_HEADER_INDEX)||status!=200)fail("Mawaqit did not return HTTP 200");
 std::string result;char b[8192];for(;;){DWORD got=0;if(!WinHttpReadData(req.h,b,sizeof(b),&got))fail("HTTP read failed");if(!got)break;if(result.size()+got>8*1024*1024)fail("HTML too large");result.append(b,got);}return result;
}

static std::string extract(const std::string& html){
 size_t pos=html.find("let confData");if(pos==std::string::npos)fail("Mawaqit confData not found");pos=html.find('=',pos);if(pos==std::string::npos)fail("confData assignment missing");pos=html.find('{',pos);if(pos==std::string::npos)fail("confData object missing");
 // The parser consumes exactly one JSON object, so nested braces and quoted strings are safe.
 std::string rest=html.substr(pos);Parser p{rest};J root=p.value();if(root.kind!=J::Obj)fail("confData is not an object");return rest.substr(0,p.p);
}

static bool leap(int y){return y%4==0&&(y%100!=0||y%400==0);}
static int days(int y,int m){static const int d[]={31,28,31,30,31,30,31,31,30,31,30,31};return d[m-1]+(m==2&&leap(y));}
static std::string date(int y,int m,int d){char b[16];snprintf(b,sizeof(b),"%04d-%02d-%02d",y,m,d);return b;}
static bool validTime(const std::string& t){return t.size()==5&&t[2]==':'&&t[0]>='0'&&t[0]<='2'&&t[1]>='0'&&t[1]<='9'&&t[3]>='0'&&t[3]<='5'&&t[4]>='0'&&t[4]<='9'&&((t[0]-'0')*10+t[1]-'0')<24;}
static void writeFile(const std::wstring& path,const std::string& content){HANDLE h=CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);if(h==INVALID_HANDLE_VALUE)fail("Cannot create temporary cache file");DWORD written=0;BOOL ok=WriteFile(h,content.data(),(DWORD)content.size(),&written,nullptr);FlushFileBuffers(h);CloseHandle(h);if(!ok||written!=content.size())fail("Cache write failed");}
static void replace(const std::wstring& from,const std::wstring& to){if(!MoveFileExW(from.c_str(),to.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))fail("Cache replace failed");}

static std::wstring normalize(std::wstring input,std::wstring& slug){
 const std::wstring prefix=L"https://mawaqit.net/";if(input.rfind(prefix,0)==0)input=input.substr(prefix.size());
 if(input.empty()||input.find_first_of(L"?#\\@")!=std::wstring::npos||input.find(L"://")!=std::wstring::npos)fail("Invalid mosque URL/identifier");
 if(input.back()==L'/')input.pop_back();std::wstring lang=L"nl";size_t slash=input.find(L'/');if(slash!=std::wstring::npos){lang=input.substr(0,slash);slug=input.substr(slash+1);}else slug=input;
 auto valid=[](wchar_t c){return(c>=L'a'&&c<=L'z')||(c>=L'A'&&c<=L'Z')||(c>=L'0'&&c<=L'9')||c==L'_'||c==L'-';};
 if(lang.size()!=2||!std::all_of(lang.begin(),lang.end(),[](wchar_t c){return c>=L'a'&&c<=L'z';})||slug.empty()||slug.size()>120||!std::all_of(slug.begin(),slug.end(),valid))fail("Invalid mosque slug or language");
 return L"/"+lang+L"/"+slug;
}

static bool Run(const std::wstring& mosque) {
 std::wstring slug;std::wstring path=normalize(mosque,slug);std::string html=fetch(path);std::string json=extract(html);Parser parser{json};J root=parser.value();
 const J& cal=root.key("calendar");if(cal.kind!=J::Arr||cal.a.size()!=12)fail("Calendar must have 12 months");
 const J& nameVal=root.key("name");const J& tzVal=root.key("timezone");if(tzVal.kind!=J::Str||tzVal.s.empty())fail("Missing timezone");std::string name=nameVal.kind==J::Str&&!nameVal.s.empty()?nameVal.s:narrow(slug);
 // Use the local Windows date for the cached schedule year.
 SYSTEMTIME now;GetLocalTime(&now);int year=now.wYear;std::ostringstream sched;sched<<"# Emberveil Mawaqit schedule v1\n# Source: https://mawaqit.net"<<narrow(path)<<"\n# Year: "<<year<<"\n";
 for(int m=1;m<=12;m++){const J& month=cal.idx(m-1);if(month.kind!=J::Obj)fail("Month is not an object");for(int d=1;d<=days(year,m);d++){
  const J& entry=month.key(std::to_string(d));if(entry.kind!=J::Arr||entry.a.size()!=6)fail("Invalid calendar day");
  sched<<date(year,m,d);for(int i:{0,2,3,4,5}){const J& t=entry.idx(i);if(t.kind!=J::Str||!validTime(t.s))fail("Invalid prayer time");sched<<'|'<<t.s;}sched<<'\n';
 }}
 std::wstring dir=CacheDir();if(dir.empty())fail("Windhawk mod storage unavailable");if(!CreateDirectoryW(dir.c_str(),nullptr)&&GetLastError()!=ERROR_ALREADY_EXISTS)fail("Cannot create cache folder");
 // Retain existing Hijri date metadata only if it is from the same mosque and day.
 std::string hijri="",hijriSource="unavailable";std::wstring old=dir+L"\\metadata.json";
 {HANDLE h=CreateFileW(old.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);if(h!=INVALID_HANDLE_VALUE){DWORD sz=GetFileSize(h,nullptr);if(sz<65536){std::string data(sz,0);DWORD got=0;if(ReadFile(h,data.data(),sz,&got,nullptr)){try{Parser mp{data};J prev=mp.value();if(prev.key("mosqueIdentifier").s==narrow(slug)&&prev.key("localDate").s==date(now.wYear,now.wMonth,now.wDay)){hijri=prev.key("hijriDate").s;hijriSource=prev.key("hijriDateSource").s;}}catch(...){}}}CloseHandle(h);}}
 FILETIME ft;GetSystemTimeAsFileTime(&ft);SYSTEMTIME utc;GetSystemTime(&utc);char stamp[40];snprintf(stamp,sizeof(stamp),"%04u-%02u-%02uT%02u:%02u:%02uZ",utc.wYear,utc.wMonth,utc.wDay,utc.wHour,utc.wMinute,utc.wSecond);
 std::ostringstream meta;meta<<"{\n  \"schemaVersion\": 1,\n  \"mosqueName\": "<<quote(name)<<",\n  \"mosqueIdentifier\": "<<quote(narrow(slug))<<",\n  \"mosqueUrl\": "<<quote("https://mawaqit.net"+narrow(path))<<",\n  \"timezone\": "<<quote(tzVal.s)<<",\n  \"scheduleYear\": "<<year<<",\n  \"localDate\": "<<quote(date(now.wYear,now.wMonth,now.wDay))<<",\n  \"hijriDate\": "<<quote(hijri)<<",\n  \"hijriDateSource\": "<<quote(hijriSource)<<",\n  \"updatedAtUtc\": "<<quote(stamp)<<"\n}\n";
 std::wstring tmp1=dir+L"\\schedule.native.tmp",tmp2=dir+L"\\metadata.native.tmp";DeleteFileW(tmp1.c_str());DeleteFileW(tmp2.c_str());
 try{writeFile(tmp1,sched.str());writeFile(tmp2,meta.str());replace(tmp1,dir+L"\\schedule.txt");replace(tmp2,dir+L"\\metadata.json");}catch(...){DeleteFileW(tmp1.c_str());DeleteFileW(tmp2.c_str());throw;}
 return true;
}
}


static std::wstring g_mosqueName=L"Mosque",g_hijriDate=L"";
struct Prayer { std::wstring name; int minute; };
static const wchar_t* kNames[]={L"Fajr",L"Dhuhr",L"Asr",L"Maghrib",L"Isha"};
static std::wstring g_label=L"Loading schedule",g_countdown=L"--:--";
static std::wstring g_loadedDate;
static std::vector<Prayer> g_today,g_tomorrow;

static std::wstring DateKey(const SYSTEMTIME& t){wchar_t b[16];swprintf(b,16,L"%04u-%02u-%02u",t.wYear,t.wMonth,t.wDay);return b;}

static std::wstring CachePath(){
 std::wstring dir=CacheDir();
 return dir.empty()?L"":dir+L"\\schedule.txt";
}

static std::wstring Utf8ToWide(const std::string& s) {
 int n=MultiByteToWideChar(CP_UTF8,0,s.data(),(int)s.size(),nullptr,0);
 if(n<=0)return L"";
 std::wstring result(n,L'\0');
 MultiByteToWideChar(CP_UTF8,0,s.data(),(int)s.size(),&result[0],n);
 return result;
}
// Minimal JSON string reader for the two fields written by our trusted updater.
// Handles standard JSON escapes and Unicode escapes, including surrogate pairs.

static std::wstring JsonField(const std::wstring& json,const wchar_t* key){
 std::wstring token=L"\""+std::wstring(key)+L"\"";
 size_t at=json.find(token);if(at==std::wstring::npos)return L"";
 at=json.find(L':',at+token.size());if(at==std::wstring::npos)return L"";
 at=json.find_first_not_of(L" \r\n\t",at+1);
 if(at==std::wstring::npos||json[at]!=L'"')return L"";
 std::wstring out;
 for(size_t i=at+1;i<json.size();i++){
  wchar_t c=json[i];if(c==L'"')break;
  if(c!=L'\\'){out+=c;continue;}
  if(++i>=json.size())break;
  c=json[i];
  if(c==L'u' && i+4<json.size()){
   unsigned code=0;bool valid=true;
   for(int j=0;j<4;j++){wchar_t h=json[i+1+j];unsigned v;
    if(h>=L'0'&&h<=L'9')v=h-L'0';
    else if(h>=L'a'&&h<=L'f')v=h-L'a'+10;
    else if(h>=L'A'&&h<=L'F')v=h-L'A'+10;
    else {valid=false;break;}code=code*16+v;
   }
   if(valid){out+=(wchar_t)code;i+=4;}continue;
  }
  switch(c){case L'n':out+=L'\n';break;case L'r':out+=L'\r';break;
   case L't':out+=L'\t';break;default:out+=c;break;}
 }
 return out;
}

static void LoadMetadata(){
 g_mosqueName=L"Mosque";g_hijriDate=L"";
 std::wstring path=CachePath();if(path.empty())return;
 size_t slash=path.find_last_of(L'\\');if(slash==std::wstring::npos)return;
 path=path.substr(0,slash+1)+L"metadata.json";
 std::ifstream f(path.c_str(),std::ios::binary);
 if(!f)return;
 std::string bytes((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
 if(bytes.size()>65536)return;
 std::wstring json=Utf8ToWide(bytes);
 std::wstring name=JsonField(json,L"mosqueName");
 if(!name.empty())g_mosqueName=name;
 std::wstring hijri=JsonField(json,L"hijriDate");
 if(hijri.size()==10 && hijri[4]==L'-' && hijri[7]==L'-'){
  static const wchar_t* months[]={L"Muharram",L"Safar",L"Rabi' al-Awwal",L"Rabi' al-Thani",L"Jumada al-Ula",L"Jumada al-Akhirah",L"Rajab",L"Sha'ban",L"Ramadan",L"Shawwal",L"Dhu al-Qi'dah",L"Dhu al-Hijjah"};
  int month=_wtoi(hijri.substr(5,2).c_str()),day=_wtoi(hijri.substr(8,2).c_str());
  if(month>=1&&month<=12&&day>=1&&day<=30){
   wchar_t text[120];swprintf(text,120,L"%d %ls %ls AH",day,months[month-1],hijri.substr(0,4).c_str());g_hijriDate=text;
  }
 }
}

static bool ParseDay(const std::wstring& line,const std::wstring& date,std::vector<Prayer>& out){
 if(line.size()<date.size()+1||line.compare(0,date.size(),date)!=0||line[date.size()]!=L'|')return false;
 std::wistringstream s(line.substr(date.size()+1));std::wstring item;std::vector<Prayer> result;
 for(int i=0;i<5;i++){
  if(!std::getline(s,item,L'|')||item.size()!=5||item[2]!=L':'||item[0]<L'0'||item[0]>L'2'||item[1]<L'0'||item[1]>L'9'||item[3]<L'0'||item[3]>L'5'||item[4]<L'0'||item[4]>L'9')return false;
  int hour=(item[0]-L'0')*10+item[1]-L'0';int minute=(item[3]-L'0')*10+item[4]-L'0';if(hour>23)return false;
  result.push_back({kNames[i],hour*60+minute});
 }
 if(std::getline(s,item,L'|'))return false;
 out=result;return true;
}

static void LoadSchedule(const SYSTEMTIME& now){
 std::wstring today=DateKey(now);if(today==g_loadedDate)return;
 SYSTEMTIME next=now;
 FILETIME ft;SystemTimeToFileTime(&next,&ft);
 ULARGE_INTEGER v;v.LowPart=ft.dwLowDateTime;v.HighPart=ft.dwHighDateTime;v.QuadPart+=864000000000ULL;
 ft.dwLowDateTime=v.LowPart;ft.dwHighDateTime=v.HighPart;FileTimeToSystemTime(&ft,&next);
 std::wstring tomorrow=DateKey(next);
 std::wstring path=CachePath();
 std::wifstream file(path.c_str());if(!file)return;
 std::wstring line;std::vector<Prayer> a,b;
 while(std::getline(file,line)){
  if(a.empty())ParseDay(line,today,a);
  if(b.empty())ParseDay(line,tomorrow,b);
  if(!a.empty()&&!b.empty())break;
 }
 if(a.size()!=5)return;
 g_today=a;g_tomorrow=b;g_loadedDate=today;
}

static void UpdateCountdown(){
 SYSTEMTIME now;GetLocalTime(&now);LoadSchedule(now);
 if(g_today.empty()){g_label=L"Schedule unavailable";g_countdown=L"--:--";return;}
 int nowSec=now.wHour*3600+now.wMinute*60+now.wSecond;
 for(const auto& p:g_today){int sec=p.minute*60-nowSec;if(sec>0){
  wchar_t label[80],count[32];swprintf(label,80,L"%ls  %02d:%02d",p.name.c_str(),p.minute/60,p.minute%60);
  swprintf(count,32,L"%02d:%02d:%02d",sec/3600,(sec/60)%60,sec%60);
  g_label=label;g_countdown=count;return;
 }}
 if(!g_tomorrow.empty()){
  const auto& p=g_tomorrow.front();int sec=86400-nowSec+p.minute*60;
  wchar_t label[80],count[32];swprintf(label,80,L"%ls  %02d:%02d",p.name.c_str(),p.minute/60,p.minute%60);
  swprintf(count,32,L"%02d:%02d:%02d",sec/3600,(sec/60)%60,sec%60);
  g_label=label;g_countdown=count;
 }else{g_label=L"Tomorrow unavailable";g_countdown=L"--:--";}
}



namespace slot = folder_menus_slot_lease;
namespace taskwin = folder_menus_taskbar_window;
namespace taskxaml = folder_menus_taskbar_xaml;

static std::atomic<bool> g_stopping{false};
// TrayUI::StartTaskbar can recreate XAML without restarting explorer.exe.
static std::atomic<bool> g_taskbarRebuildPending{false};

static slot::Lease g_lease{};
static winrt::weak_ref<FrameworkElement> g_placeholder;
static winrt::weak_ref<Panel> g_parent;
struct Insets {int left=0,top=0,right=0,bottom=0;};
static Insets ParseInsets(const std::wstring& value,int minVal,int maxVal){
 Insets result{};int values[4]{};
 if(swscanf_s(value.c_str(),L" %d , %d , %d , %d ",&values[0],&values[1],&values[2],&values[3])==4){
  for(int& v:values)v=std::clamp(v,minVal,maxVal);
  result={values[0],values[1],values[2],values[3]};
 }
 return result;
}
static int g_width=200,g_pillHeight=0,g_trayGap=5,g_cornerRadius=13,g_fontSize=15;
static Insets g_margins{},g_padding{};
static std::wstring g_theme=L"system";
static std::wstring g_position=L"before-tray";
struct PillPalette { winrt::Windows::UI::Color background,foreground,countdown,separator; };
static winrt::Windows::UI::Color Rgb(unsigned c) {
 return winrt::Windows::UI::Color{255,(uint8_t)(c>>16),(uint8_t)(c>>8),(uint8_t)c};
}
static COLORREF ParseThemeHex(const std::wstring& input,COLORREF fallback) {
    if(input.size()!=7 || input[0]!=L'#')return fallback;
    unsigned value=0;
    for(size_t i=1;i<7;i++){
        wchar_t c=input[i];
        unsigned digit=(c>=L'0'&&c<=L'9')?(unsigned)(c-L'0'):
                       (c>=L'a'&&c<=L'f')?(unsigned)(c-L'a'+10):
                       (c>=L'A'&&c<=L'F')?(unsigned)(c-L'A'+10):16;
        if(digit>15)return fallback;
        value=(value<<4)|digit;
    }
    return RGB((value>>16)&255,(value>>8)&255,value&255);
}
static std::wstring ReadSetting(PCWSTR key);
static unsigned ReadCustomHex(PCWSTR key,unsigned fallback) {
    COLORREF c=ParseThemeHex(ReadSetting(key),RGB((fallback>>16)&255,(fallback>>8)&255,fallback&255));
    return (unsigned)GetRValue(c)<<16 | (unsigned)GetGValue(c)<<8 | GetBValue(c);
}
static bool WindowsLightMode() {
 HKEY key=nullptr;
 if(RegOpenKeyExW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS)return false;
 DWORD value=0,bytes=sizeof(value);
 LONG result=RegQueryValueExW(key,L"AppsUseLightTheme",nullptr,nullptr,reinterpret_cast<BYTE*>(&value),&bytes);
 RegCloseKey(key);return result==ERROR_SUCCESS && value!=0;
}
static PillPalette ResolvePillPalette(const std::wstring& theme) {
 if(theme==L"traditional")return {Rgb(0x123D34),Rgb(0xF5EAD3),Rgb(0xE4C784),Rgb(0xC9B789)};
 if(theme==L"catppuccin")return {Rgb(0x211E32),Rgb(0xCDD6F4),Rgb(0xCFA1FF),Rgb(0xA6ADC8)};
 if(theme==L"emberveil")return {Rgb(0x211E32),Rgb(0xF5E6C8),Rgb(0xFFBC5E),Rgb(0xB8A98A)};
 if(theme==L"emberveil-midnight")return {Rgb(0x0A0C0E),Rgb(0xF5E6C8),Rgb(0xFFBC5E),Rgb(0xB8A98A)};
 if(theme==L"emerald-mosque")return {Rgb(0x102B27),Rgb(0xF3E8CF),Rgb(0xD4AF70),Rgb(0xB6C9BD)};
 if(theme==L"dracula")return {Rgb(0x282A36),Rgb(0xF8F8F2),Rgb(0xBD93F9),Rgb(0x6272A4)};
 if(theme==L"nord")return {Rgb(0x2E3440),Rgb(0xECEFF4),Rgb(0x88C0D0),Rgb(0x81A1C1)};
 if(theme==L"gruvbox")return {Rgb(0x282828),Rgb(0xEBDBB2),Rgb(0xFABD2F),Rgb(0x928374)};
 if(theme==L"tokyo-night")return {Rgb(0x1A1B26),Rgb(0xC0CAF5),Rgb(0x7AA2F7),Rgb(0x565F89)};
 if(theme==L"solarized-dark")return {Rgb(0x002B36),Rgb(0x93A1A1),Rgb(0xB58900),Rgb(0x586E75)};
 if(theme==L"solarized-light")return {Rgb(0xFDF6E3),Rgb(0x586E75),Rgb(0x268BD2),Rgb(0x93A1A1)};
 if(theme==L"custom")return {
     Rgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPillBackground",0x211E32)),
     Rgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPillText",0xF5E6C8)),
     Rgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPillCountdown",0xFFBC5E)),
     Rgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPillSeparator",0xB8A98A))};
 COLORREF c=GetSysColor(COLOR_HIGHLIGHT);
 auto accent=winrt::Windows::UI::Color{255,GetRValue(c),GetGValue(c),GetBValue(c)};
 if(WindowsLightMode())return {Rgb(0xF2F2F2),Rgb(0x202020),accent,Rgb(0x666666)};
 return {Rgb(0x292929),Rgb(0xF4F4F4),accent,Rgb(0xB8B8B8)};
}
static bool g_showNext=true,g_showCountdown=true,g_enablePopup=true;
static std::atomic<int> g_popupAutoCloseSeconds{5};
static ULONGLONG g_popupAutoCloseDeadline=0;
static std::wstring g_mosque=L"al-haram-makkah-saudi-arabia";
static int g_refreshHours=24;
static SRWLOCK g_settingsLock=SRWLOCK_INIT;
static HANDLE g_stopEvent=nullptr,g_refreshEvent=nullptr;
static HANDLE g_updateWorker=nullptr;
static HANDLE g_countdownWorker=nullptr;
static void UpdateClockOnUiThread();
[[clang::no_destroy]] static TextBlock g_prayerText{nullptr};
[[clang::no_destroy]] static TextBlock g_countdownText{nullptr};
[[clang::no_destroy]] static Grid g_pill{nullptr};
static HWND g_popup=nullptr;
static winrt::event_token g_tapToken{};
static bool g_tapRegistered=false;


// Popup is a small independent Win32 window owned by the taskbar UI thread.
// The native taskbar slot remains XAML; this preserves the original v0.6.6
// popup behavior without introducing another XAML layout lease.
static COLORREF PopupRgb(unsigned v){return RGB((v>>16)&255,(v>>8)&255,v&255);}
struct PopupColors {COLORREF bg,border,text,muted,rule,footer;};
static PopupColors PopupTheme(){
 if(g_theme==L"custom")return {
     PopupRgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPopupBackground",0x1D1D2D)),
     PopupRgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPopupBorder",0x3B3B58)),
     PopupRgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPopupText",0xD3D0E8)),
     PopupRgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPopupMuted",0xB8B3D1)),
     PopupRgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPopupRule",0x424158)),
     PopupRgb(ReadCustomHex(L"AdvancedCustomization.CustomThemeSettings.CustomPopupFooter",0xA29DBB))};
 if(g_theme==L"traditional")return {PopupRgb(0x102F29),PopupRgb(0x527361),PopupRgb(0xF5EAD3),PopupRgb(0xC9B789),PopupRgb(0x527361),PopupRgb(0xE4C784)};
 if(g_theme==L"catppuccin")return {PopupRgb(0x302D47),PopupRgb(0x58536F),PopupRgb(0xCDD6F4),PopupRgb(0xA6ADC8),PopupRgb(0x58536F),PopupRgb(0xB4BEFE)};
 if(g_theme==L"emberveil")return {PopupRgb(0x1D1D2D),PopupRgb(0x3B3B58),PopupRgb(0xD3D0E8),PopupRgb(0xB8B3D1),PopupRgb(0x424158),PopupRgb(0xA29DBB)};
 if(g_theme==L"emberveil-midnight")return {PopupRgb(0x101418),PopupRgb(0x353A3E),PopupRgb(0xF5E6C8),PopupRgb(0xB8A98A),PopupRgb(0x353A3E),PopupRgb(0xFFBC5E)};
 if(g_theme==L"emerald-mosque")return {PopupRgb(0x102B27),PopupRgb(0x386057),PopupRgb(0xF3E8CF),PopupRgb(0xB6C9BD),PopupRgb(0x386057),PopupRgb(0xD4AF70)};
 if(g_theme==L"dracula")return {PopupRgb(0x282A36),PopupRgb(0x44475A),PopupRgb(0xF8F8F2),PopupRgb(0xBFBFD0),PopupRgb(0x44475A),PopupRgb(0xBD93F9)};
 if(g_theme==L"nord")return {PopupRgb(0x2E3440),PopupRgb(0x4C566A),PopupRgb(0xECEFF4),PopupRgb(0xD8DEE9),PopupRgb(0x4C566A),PopupRgb(0x88C0D0)};
 if(g_theme==L"gruvbox")return {PopupRgb(0x282828),PopupRgb(0x504945),PopupRgb(0xEBDBB2),PopupRgb(0xA89984),PopupRgb(0x504945),PopupRgb(0xFABD2F)};
 if(g_theme==L"tokyo-night")return {PopupRgb(0x1A1B26),PopupRgb(0x414868),PopupRgb(0xC0CAF5),PopupRgb(0xA9B1D6),PopupRgb(0x414868),PopupRgb(0x7AA2F7)};
 if(g_theme==L"solarized-dark")return {PopupRgb(0x002B36),PopupRgb(0x586E75),PopupRgb(0x93A1A1),PopupRgb(0x839496),PopupRgb(0x586E75),PopupRgb(0xB58900)};
 if(g_theme==L"solarized-light")return {PopupRgb(0xFDF6E3),PopupRgb(0xEEE8D5),PopupRgb(0x586E75),PopupRgb(0x657B83),PopupRgb(0x93A1A1),PopupRgb(0x268BD2)};
 if(WindowsLightMode())return {PopupRgb(0xFAFAFA),PopupRgb(0xD1D1D1),PopupRgb(0x202020),PopupRgb(0x666666),PopupRgb(0xD1D1D1),GetSysColor(COLOR_HIGHLIGHT)};
 return {PopupRgb(0x202020),PopupRgb(0x505050),PopupRgb(0xF4F4F4),PopupRgb(0xB8B8B8),PopupRgb(0x505050),GetSysColor(COLOR_HIGHLIGHT)};
}
static void PaintPrayerPopup(HWND hwnd){
 PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);if(!dc)return;
 // Draw in a fixed 270x252 logical coordinate space. The GDI viewport
 // scales the entire popup, including text, consistently with its DPI size.
 RECT client{};GetClientRect(hwnd,&client);
 int savedDc=SaveDC(dc);
 SetMapMode(dc,MM_ANISOTROPIC);
 SetWindowExtEx(dc,270,252,nullptr);
 SetViewportExtEx(dc,client.right-client.left,client.bottom-client.top,nullptr);
 RECT r{0,0,270,252};auto colors=PopupTheme();
 HBRUSH bg=CreateSolidBrush(colors.bg);FillRect(dc,&r,bg);DeleteObject(bg);
 HPEN border=CreatePen(PS_SOLID,1,colors.border);HGDIOBJ prevPen=SelectObject(dc,border);
 HGDIOBJ oldBrush=SelectObject(dc,GetStockObject(NULL_BRUSH));Rectangle(dc,0,0,r.right,r.bottom);
 SelectObject(dc,oldBrush);SelectObject(dc,prevPen);DeleteObject(border);
 SetBkMode(dc,TRANSPARENT);
 HFONT font=CreateFontW(-15,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
 HGDIOBJ oldFont=SelectObject(dc,font);
 auto line=[&](const std::wstring& value,int left,int top,int right,int height,COLORREF color,UINT flags){
   RECT rect{left,top,right,top+height};SetTextColor(dc,color);
   DrawTextW(dc,value.c_str(),-1,&rect,flags|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS);
 };
 RECT nameRect{14,9,r.right-14,53};SetTextColor(dc,colors.text);
 DrawTextW(dc,g_mosqueName.c_str(),-1,&nameRect,DT_CENTER|DT_WORDBREAK|DT_NOPREFIX|DT_END_ELLIPSIS);
 bool hijri=!g_hijriDate.empty();if(hijri)line(g_hijriDate,14,54,r.right-14,19,colors.muted,DT_CENTER);
 int topRule=hijri?85:65,bottomRule=r.bottom-34;
 HPEN rule=CreatePen(PS_SOLID,1,colors.rule);prevPen=SelectObject(dc,rule);
 MoveToEx(dc,12,topRule,nullptr);LineTo(dc,r.right-12,topRule);
 MoveToEx(dc,12,bottomRule,nullptr);LineTo(dc,r.right-12,bottomRule);
 SelectObject(dc,prevPen);DeleteObject(rule);
 int firstRow=topRule+12+((bottomRule-12)-(topRule+12)-5*25)/2;
 for(int i=0;i<5;i++){
   std::wstring time=L"--:--";
   if(i<(int)g_today.size()){
     wchar_t b[16];swprintf(b,16,L"%02d:%02d",g_today[i].minute/60,g_today[i].minute%60);time=b;
   }
   int y=firstRow+i*25;
   line(kNames[i],20,y,r.right-20,23,colors.text,DT_LEFT);
   line(time,20,y,r.right-20,23,colors.text,DT_RIGHT);
 }
 line(g_label+L" - in "+g_countdown,12,bottomRule+5,r.right-12,24,colors.footer,DT_CENTER);
 SelectObject(dc,oldFont);DeleteObject(font);RestoreDC(dc,savedDc);EndPaint(hwnd,&ps);
}
// The popup is an activatable flyout: Windows notifies it when focus leaves.
// No session-wide mouse hook or cross-application Escape polling is needed.
static ULONGLONG g_popupLastDeactivated=0;
static void HidePrayerPopup(bool deactivated=false){
    g_popupAutoCloseDeadline=0;
    if(g_popup && IsWindow(g_popup)){
        KillTimer(g_popup,1);
        ShowWindow(g_popup,SW_HIDE);
    }
    if(deactivated)g_popupLastDeactivated=GetTickCount64();
}
static LRESULT CALLBACK PrayerPopupProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 switch(msg){
 case WM_ACTIVATE:
   if(LOWORD(wp)==WA_INACTIVE && IsWindowVisible(hwnd))
       HidePrayerPopup(true);
   return 0;
 case WM_KEYDOWN:
   if(wp==VK_ESCAPE){HidePrayerPopup();return 0;}
   break;
 case WM_PAINT: PaintPrayerPopup(hwnd); return 0;
 case WM_ERASEBKGND:return 1;
 case WM_CLOSE:HidePrayerPopup();return 0;
 case WM_TIMER:{
   if(wp==1){
     if(g_popupAutoCloseDeadline && GetTickCount64()>=g_popupAutoCloseDeadline){
       Wh_Log(L"[Mawaqit] Popup auto-close timeout reached");
       HidePrayerPopup();return 0;
     }
     InvalidateRect(hwnd,nullptr,FALSE);
   }return 0;
 }
 case WM_DESTROY:KillTimer(hwnd,1);if(g_popup==hwnd)g_popup=nullptr;return 0;
 }
 return DefWindowProcW(hwnd,msg,wp,lp);
}
static wchar_t g_popupClassName[128]{};
static HINSTANCE g_popupClassInstance=nullptr;
static bool g_popupClassRegistered=false;
static void DestroyPrayerPopup(){
    if(g_popup){DestroyWindow(g_popup);g_popup=nullptr;}
    if(g_popupClassRegistered){
        if(!UnregisterClassW(g_popupClassName,g_popupClassInstance))
            Wh_Log(L"[Mawaqit] Popup class unregister failed: %lu",GetLastError());
        else g_popupClassRegistered=false;
    }
}
[[maybe_unused]] static void TogglePrayerPopup(){

 if(!g_enablePopup){Wh_Log(L"[Mawaqit] Popup click ignored: disabled by settings");return;}
 // A pill click may have just deactivated the flyout. Do not reopen it.
 if(g_popupLastDeactivated && GetTickCount64()-g_popupLastDeactivated<300)return;
 if(g_popup && IsWindowVisible(g_popup)){HidePrayerPopup();return;}
 if(!g_popup){

   // Register a unique class per loaded mod instance. A previous Windhawk
   // hot-reload can leave a process-wide class pointing at unloaded code.
   if(!g_popupClassName[0]){
       LARGE_INTEGER stamp{};QueryPerformanceCounter(&stamp);
       swprintf_s(g_popupClassName,L"MawaqitPopup_%lu_%llX_%p",
                  GetCurrentProcessId(),(unsigned long long)stamp.QuadPart,
                  reinterpret_cast<void*>(&PrayerPopupProc));
   }
   if(!g_popupClassInstance){
       HMODULE module=nullptr;
       if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                             GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                             reinterpret_cast<LPCWSTR>(&PrayerPopupProc),&module)){
           Wh_Log(L"[Mawaqit] Popup module lookup failed: %lu",GetLastError());
           return;
       }
       g_popupClassInstance=module;
   }
   WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.lpfnWndProc=PrayerPopupProc;
   wc.hInstance=g_popupClassInstance;
   wc.lpszClassName=g_popupClassName;wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
   if(!g_popupClassRegistered){
       ATOM atom=RegisterClassExW(&wc);
       Wh_Log(L"[Mawaqit] class-register result=%u error=%lu",(unsigned)atom,atom?0:GetLastError());
       if(!atom)return;
       g_popupClassRegistered=true;
   }

   g_popup=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,
       g_popupClassName,L"Mawaqit Prayer Times",WS_POPUP,0,0,270,252,nullptr,nullptr,wc.hInstance,nullptr);

   if(!g_popup){Wh_Log(L"[Mawaqit] Popup creation failed: %lu",GetLastError());return;}


 }

 LoadMetadata();

 UpdateCountdown();

 HWND taskbar=taskwin::FindCurrentProcessTaskbarWnd();
 RECT bar{};if(taskbar)GetWindowRect(taskbar,&bar);
 RECT monitor{};MONITORINFO mi{};mi.cbSize=sizeof(mi);
 HMONITOR mon=MonitorFromWindow(taskbar?taskbar:g_popup,MONITOR_DEFAULTTONEAREST);
 if(GetMonitorInfoW(mon,&mi))monitor=mi.rcMonitor;
 // Win32 window rectangles are physical pixels; XAML coordinates are DIPs.
 UINT dpi=taskbar?GetDpiForWindow(taskbar):96;
 if(!dpi)dpi=96;
 const double scale=static_cast<double>(dpi)/96.0;
 const int w=MulDiv(270,dpi,96),h=MulDiv(252,dpi,96);
 const int edgeGap=MulDiv(5,dpi,96);
 const int screenMargin=MulDiv(4,dpi,96);
 bool top=bar.top<=monitor.top+10;
 int x=bar.left+(bar.right-bar.left-w)/2;
 // Center below the XAML pill if possible, otherwise use taskbar center.
 if(g_pill){
   try{auto transform=g_pill.TransformToVisual(nullptr);auto pt=transform.TransformPoint({0,0});
       x=bar.left+(int)std::lround((pt.X+g_pill.ActualWidth()/2.0)*scale)-w/2;}catch(...){}
 }
 int y=top?bar.bottom+edgeGap:bar.top-h-edgeGap;
 x=std::max<int>(monitor.left+screenMargin,std::min<int>(x,monitor.right-w-screenMargin));
 y=std::max<int>(monitor.top+screenMargin,std::min<int>(y,monitor.bottom-h-screenMargin));
 // Clip the Win32 popup itself, not only its painted background.
 // The region is owned by the window after a successful SetWindowRgn call.

 HRGN rounded=CreateRoundRectRgn(0,0,w+1,h+1,MulDiv(18,dpi,96),MulDiv(18,dpi,96));
 if(rounded && !SetWindowRgn(g_popup,rounded,FALSE))DeleteObject(rounded);

 SetWindowPos(g_popup,HWND_TOPMOST,x,y,w,h,SWP_SHOWWINDOW);
 SetForegroundWindow(g_popup);
 SetFocus(g_popup);
 SetTimer(g_popup,1,150,nullptr);
 // Arm only after the popup becomes visible; a subsequent opening re-arms.
 int autoClose=g_popupAutoCloseSeconds.load();
 g_popupAutoCloseDeadline=autoClose>0?GetTickCount64()+static_cast<ULONGLONG>(autoClose)*1000:0;
 Wh_Log(L"[Mawaqit] Popup opened: auto-close=%d seconds",autoClose);


 InvalidateRect(g_popup,nullptr,FALSE);

}

static constexpr wchar_t kSlotName[]=L"MawaqitTaskbarSlot";
static constexpr wchar_t kLeaseName[]=L"MawaqitTaskbarLease";

static FrameworkElement FindNamed(FrameworkElement const& root, wchar_t const* name, int depth=0) {
    if(!root || depth>24) return nullptr;
    if(root.Name()==name) return root;
    int count=VisualTreeHelper::GetChildrenCount(root);
    for(int i=0;i<count;i++) {
        auto child=VisualTreeHelper::GetChild(root,i).try_as<FrameworkElement>();
        if(auto found=FindNamed(child,name,depth+1)) return found;
    }
    return nullptr;
}

static FrameworkElement LiveTaskbarRoot() {
    HWND wnd=taskwin::FindCurrentProcessTaskbarWnd();
    if(!wnd) return nullptr;
    auto xamlRoot=taskxaml::GetTaskbarXamlRoot(wnd);
    return xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
}

static Panel LiveTray() {
    auto root=LiveTaskbarRoot();
    return root ? FindNamed(root,L"SystemTrayFrameGrid").try_as<Panel>() : nullptr;
}

// Locate the application repeater's actual parent for icon-area insertion.
// Do not substitute a tray slot when the XAML hierarchy differs.
static bool FindApplicationRepeater(FrameworkElement const& root,
                                    Panel& parent, FrameworkElement& repeater,
                                    int depth=0) {
    if(!root || depth>24) return false;
    // Windows 11 taskbar applications are hosted by TaskbarFrameRepeater.
    // Only lease beside a direct child of a supported panel.
    if(auto panel=root.try_as<Panel>()) {
        for(auto const& child:panel.Children()) {
            auto element=child.try_as<FrameworkElement>();
            if(element && element.Name()==L"TaskbarFrameRepeater" &&
               slot::Classify(panel)!=slot::Kind::Unsupported) {
                parent=panel;
                repeater=element;
                return true;
            }
        }
    }
    int count=VisualTreeHelper::GetChildrenCount(root);
    for(int i=0;i<count;i++) {
        auto child=VisualTreeHelper::GetChild(root,i).try_as<FrameworkElement>();
        if(child && FindApplicationRepeater(child,parent,repeater,depth+1))
            return true;
    }
    return false;
}

// Center uses a full-width taskbar root Grid overlay rather than a leased
// application/tray slot. Only accept a root that actually spans the taskbar;
// otherwise skip rather than accidentally center inside a small subpanel.
static Panel ResolveCenterParent() {
    auto root=LiveTaskbarRoot();
    auto grid=root.try_as<Grid>();
    HWND wnd=taskwin::FindCurrentProcessTaskbarWnd();
    RECT rect{};
    if(!grid || !wnd || !GetWindowRect(wnd,&rect)) return nullptr;
    double dpi=(double)GetDpiForWindow(wnd);
    double taskbarWidth=(rect.right-rect.left)*96.0/dpi;
    if(taskbarWidth<=0 || grid.ActualWidth()<taskbarWidth*0.75) {
        Wh_Log(L"[Mawaqit] Center skipped: root Grid width %.1f vs taskbar %.1f DIP",
               grid.ActualWidth(),taskbarWidth);
        return nullptr;
    }
    Panel iconsParent{nullptr};
    FrameworkElement repeater{nullptr};
    auto tray=FindNamed(root,L"SystemTrayFrameGrid");
    if(!tray || tray.ActualWidth()<=0 ||
       !FindApplicationRepeater(root,iconsParent,repeater) ||
       !repeater || repeater.ActualWidth()<=0) {
        Wh_Log(L"[Mawaqit] Center waiting for application/tray layout");
        return nullptr;
    }
    return grid;
}

static Panel ResolvePositionParent(int& slotIndex) {
    slotIndex=-1;
    if(g_position==L"after-icons") {
        // Unlike before-icons, the repeater's column spans nearly the whole
        // taskbar. Use a non-layout-affecting overlay anchored to realized icons.
        auto root=LiveTaskbarRoot();
        auto grid=root.try_as<Grid>();
        Panel iconsParent{nullptr}; FrameworkElement repeater{nullptr};
        if(grid && grid.ActualWidth()>0 &&
           FindApplicationRepeater(root,iconsParent,repeater) &&
           repeater && repeater.ActualWidth()>0) {
            slotIndex=0;
            return grid;
        }
        return nullptr;
    }
    if(g_position==L"center") {
        auto center=ResolveCenterParent();
        if(center) slotIndex=0;
        return center;
    }
    if(g_position==L"before-tray") {
        auto tray=LiveTray();
        if(tray && slot::ResolveSlot(tray,slot::Anchor::BeforeIcons,slotIndex)) return tray;
        return nullptr;
    }
    if(g_position==L"after-tray") {
        auto tray=LiveTray();
        if(!tray) return nullptr;
        if(slot::ResolveSlot(tray,slot::Anchor::AfterShowDesktop,slotIndex)) return tray;
        Wh_Log(L"[Mawaqit] After-tray anchor unavailable; named children: %s",
               slot::DescribeChildren(tray).c_str());
        return nullptr;
    }
    auto root=LiveTaskbarRoot();
    Panel parent{nullptr};
    FrameworkElement repeater{nullptr};
    if(root && FindApplicationRepeater(root,parent,repeater)) {
        // During Explorer startup the repeater can exist before its first
        // layout pass. Inserting at that point can lease column zero and
        // overlay the Start area. Let the startup worker retry instead.
        if(root.ActualWidth()<=0 || parent.ActualWidth()<=0 ||
           repeater.ActualWidth()<=0 || repeater.ActualHeight()<=0) {
            Wh_Log(L"[Mawaqit] Icon anchor waiting for layout: root=%.1f parent=%.1f repeater=%.1fx%.1f",
                   root.ActualWidth(),parent.ActualWidth(),
                   repeater.ActualWidth(),repeater.ActualHeight());
            return nullptr;
        }
        try {
            auto point=repeater.TransformToVisual(parent).TransformPoint({0,0});
            if(!std::isfinite(point.X) || !std::isfinite(point.Y) ||
               point.X < -1.0 || point.X > parent.ActualWidth()+1.0) {
                Wh_Log(L"[Mawaqit] Icon anchor waiting for stable coordinates: x=%.1f",point.X);
                return nullptr;
            }
        } catch(...) {
            Wh_Log(L"[Mawaqit] Icon anchor transform unavailable; retrying");
            return nullptr;
        }
        auto kind=slot::Classify(parent);
        if(kind==slot::Kind::Columns) {
            int column=Grid::GetColumn(repeater);
            int span=std::max(1,Grid::GetColumnSpan(repeater));
            slotIndex=column+(g_position==L"after-icons"?span:0);
        } else if(kind==slot::Kind::Order) {
            int index=slot::IndexOfChild(parent,repeater);
            if(index>=0) slotIndex=index+(g_position==L"after-icons"?1:0);
        }
        Wh_Log(L"[Mawaqit] Icon anchor: parent=%s repeater=%s slot=%d children=%s",
               slot::ClassName(parent).c_str(),slot::ClassName(repeater).c_str(),
               slotIndex,slot::DescribeChildren(parent).c_str());
        if(slotIndex>=0) return parent;
    }
    Wh_Log(L"[Mawaqit] Icon position '%s' unavailable: no supported TaskbarFrameRepeater parent; insertion skipped",
           g_position.c_str());
    return nullptr;
}

// Keep the overlay centered until the live application repeater reaches it,
// then translate right, stopping before the tray. Windows icon layout and
// overflow behavior remain unchanged.
static int g_centerDiagnosticCount=0;
static void UpdateDynamicCenterOnUiThread() {
    if(g_position!=L"center" || !g_pill) return;
    auto root=LiveTaskbarRoot();
    auto grid=root.try_as<Grid>();
    auto tray=LiveTray();
    if(!grid || !tray || grid.ActualWidth()<=0) return;
    Panel iconsParent{nullptr};
    FrameworkElement repeater{nullptr};
    if(!FindApplicationRepeater(root,iconsParent,repeater) || !repeater) return;
    try {
        auto point=winrt::Windows::Foundation::Point{0,0};
        auto appOrigin=repeater.TransformToVisual(grid).TransformPoint(point);
        auto trayOrigin=tray.TransformToVisual(grid).TransformPoint(point);
        double pillWidth=g_pill.ActualWidth();
        if(pillWidth<=0) pillWidth=g_pill.Width();
        double centeredLeft=(grid.ActualWidth()-pillWidth)/2.0;
        // ItemsRepeater can occupy the entire taskbar even when its realized
        // buttons use only a small portion. Measure the realized children.
        double appRight=appOrigin.X;
        unsigned measuredButtons=0;
        int count=VisualTreeHelper::GetChildrenCount(repeater);
        for(int i=0;i<count;++i) {
            auto child=VisualTreeHelper::GetChild(repeater,i).try_as<FrameworkElement>();
            if(!child || child.Visibility()!=Visibility::Visible ||
               child.ActualWidth()<=0 || child.ActualWidth()>400.0) continue;
            auto pos=child.TransformToVisual(grid).TransformPoint({0,0});
            if(std::isfinite(pos.X) && pos.X>=0 &&
               pos.X+child.ActualWidth()<=grid.ActualWidth()+1.0) {
                appRight=std::max(appRight,pos.X+child.ActualWidth());
                ++measuredButtons;
            }
        }
        // If buttons are not realized yet, do not mistake the full-width
        // repeater for occupied space; defer visibility until the next tick.
        if(measuredButtons==0) {
            Wh_Log(L"[Mawaqit] Center awaiting realized buttons: repeaterW=%.1f children=%d",repeater.ActualWidth(),count);
            g_pill.Visibility(Visibility::Collapsed);
            return;
        }
        double gap=5.0;
        double maximumLeft=trayOrigin.X-pillWidth-gap;
        double wantedLeft=std::max(centeredLeft,appRight+gap);
        // If the available region is exhausted, hide rather than overlap the
        // system tray or the application buttons. This does not force overflow.
        bool room=maximumLeft>=centeredLeft && wantedLeft<=maximumLeft;
        if(g_centerDiagnosticCount<12) {
            Wh_Log(L"[Mawaqit] Center layout: root=%.1f repeaterX=%.1f repeaterW=%.1f appRight=%.1f trayX=%.1f pillW=%.1f centered=%.1f wanted=%.1f max=%.1f room=%d",
                   grid.ActualWidth(),appOrigin.X,repeater.ActualWidth(),appRight,
                   trayOrigin.X,pillWidth,centeredLeft,wantedLeft,maximumLeft,room?1:0);
            ++g_centerDiagnosticCount;
        }
        g_pill.Visibility(room?Visibility::Visible:Visibility::Collapsed);
        double left=std::min(wantedLeft,maximumLeft);
        if(room) {
            auto translation=g_pill.RenderTransform().try_as<TranslateTransform>();
            if(!translation) {
                translation=TranslateTransform();
                g_pill.RenderTransform(translation);
            }
            translation.X(left-centeredLeft);
        }
    } catch(...) {
        // Do not disturb the stable anchored positions on unsupported trees.
        Wh_Log(L"[Mawaqit] Dynamic center bounds unavailable; retaining last placement");
    }
}

// After-icons is visually anchored to the last realized taskbar button.
// Never resize or re-column the taskbar's full-width ItemsRepeater.
static int g_afterIconsDiagnosticCount=0;
static void UpdateAfterIconsOnUiThread() {
    if(g_position!=L"after-icons" || !g_pill) return;
    auto root=LiveTaskbarRoot();
    auto grid=root.try_as<Grid>();
    if(!grid || grid.ActualWidth()<=0) return;
    Panel iconsParent{nullptr}; FrameworkElement repeater{nullptr};
    if(!FindApplicationRepeater(root,iconsParent,repeater) || !repeater) return;
    try {
        auto origin=repeater.TransformToVisual(grid).TransformPoint({0,0});
        double appRight=origin.X;
        unsigned measured=0;
        int count=VisualTreeHelper::GetChildrenCount(repeater);
        for(int i=0;i<count;++i) {
            auto child=VisualTreeHelper::GetChild(repeater,i).try_as<FrameworkElement>();
            if(!child || child.Visibility()!=Visibility::Visible ||
               child.ActualWidth()<=0 || child.ActualWidth()>400.0) continue;
            auto pos=child.TransformToVisual(grid).TransformPoint({0,0});
            if(std::isfinite(pos.X) && pos.X>=0 &&
               pos.X+child.ActualWidth()<=grid.ActualWidth()+1.0) {
                appRight=std::max(appRight,pos.X+child.ActualWidth());
                ++measured;
            }
        }
        double pillWidth=g_pill.ActualWidth();
        if(pillWidth<=0) pillWidth=g_pill.Width();
        double centeredLeft=(grid.ActualWidth()-pillWidth)/2.0;
        double desiredLeft=appRight+5.0;
        double rightLimit=grid.ActualWidth();
        if(auto tray=LiveTray()) {
            auto trayPos=tray.TransformToVisual(grid).TransformPoint({0,0});
            rightLimit=trayPos.X;
        }
        bool room=measured>0 && desiredLeft+pillWidth+5.0<=rightLimit;
        g_pill.Visibility(room?Visibility::Visible:Visibility::Collapsed);
        if(room) {
            auto transform=g_pill.RenderTransform().try_as<TranslateTransform>();
            if(!transform) {transform=TranslateTransform();g_pill.RenderTransform(transform);}
            transform.X(desiredLeft-centeredLeft);
        }
        if(g_afterIconsDiagnosticCount<15) {
            Wh_Log(L"[Mawaqit] After-icons: repeaterW=%.1f measured=%u appRight=%.1f desired=%.1f trayLeft=%.1f pillW=%.1f room=%d",
                   repeater.ActualWidth(),measured,appRight,desiredLeft,rightLimit,pillWidth,room?1:0);
            ++g_afterIconsDiagnosticCount;
        }
    } catch(...) {
        Wh_Log(L"[Mawaqit] After-icons measurement unavailable; retrying");
    }
}

static void RemoveOnUiThread() {
    g_centerDiagnosticCount=0;
    g_afterIconsDiagnosticCount=0;
    DestroyPrayerPopup();
    if(g_pill && g_tapRegistered){try{g_pill.Tapped(g_tapToken);}catch(...){}g_tapRegistered=false;}
    auto parent=g_parent.get();
    if(!parent) parent=LiveTray();
    if(parent) {
        for(int i=(int)parent.Children().Size()-1;i>=0;--i) {
            auto fe=parent.Children().GetAt(i).try_as<FrameworkElement>();
            if(fe && fe.Name()==kSlotName) parent.Children().RemoveAt(i);
        }
        if(!g_lease.markerName.empty()) {
            if(!slot::Release(parent,g_lease))
                Wh_Log(L"[Mawaqit] Could not release old lease; taskbar may have rebuilt");
        }
    }
    g_placeholder={};g_parent={};g_lease={};
    g_prayerText=nullptr;g_countdownText=nullptr;g_pill=nullptr;
}

static void InsertOnUiThread() {
    if(g_stopping.load()) return;
    int slotIndex=-1;
    auto parent=ResolvePositionParent(slotIndex);
    if(!parent){Wh_Log(L"[Mawaqit] Position parent unavailable: %s",g_position.c_str());return;}
    if(slot::Classify(parent)==slot::Kind::Unsupported) {
        Wh_Log(L"[Mawaqit] Unsupported tray panel: %s",slot::ClassName(parent).c_str());return;
    }
    // Existing slot? This prevents duplicates during repeated initialization.
    for(auto child:parent.Children()) {
        auto fe=child.try_as<FrameworkElement>();
        if(fe && fe.Name()==kSlotName) {Wh_Log(L"[Mawaqit] Slot already exists");return;}
    }
    // Slot is resolved from the selected position before creating XAML content.
    // Layout settings are DIPs, so XAML performs monitor DPI scaling.
    double dpi=96.0;
    if(auto wnd=taskwin::FindCurrentProcessTaskbarWnd()) dpi=(double)GetDpiForWindow(wnd);
    double widthDip=(double)g_width;
    Grid box;
    box.Name(kSlotName);
    box.Width(widthDip+(double)g_trayGap);
    box.Height(g_pillHeight>0?(double)g_pillHeight:30.0);
    box.HorizontalAlignment(HorizontalAlignment::Center);
    box.VerticalAlignment(VerticalAlignment::Center);
    auto palette=ResolvePillPalette(g_theme);
    SolidColorBrush background;
    background.Color(palette.background);
    Border pillSurface;
    pillSurface.Background(background);
    double radius=(double)g_cornerRadius;
    pillSurface.CornerRadius({radius,radius,radius,radius});
    pillSurface.Margin({0,0,(double)g_trayGap,0});
    // Position offsets are visual only; the native slot remains reserved.
    TranslateTransform offset;
    offset.X((double)(g_margins.left-g_margins.right));
    offset.Y((double)(g_margins.top-g_margins.bottom));
    pillSurface.RenderTransform(offset);
    pillSurface.Padding({(double)g_padding.left,
                         (double)g_padding.top,
                         (double)g_padding.right,
                         (double)g_padding.bottom});
    pillSurface.HorizontalAlignment(HorizontalAlignment::Stretch);
    pillSurface.VerticalAlignment(VerticalAlignment::Stretch);
    // XAML text controls in the native taskbar layout.
    StackPanel content;
    content.Orientation(Orientation::Horizontal);
    content.HorizontalAlignment(HorizontalAlignment::Center);
    content.VerticalAlignment(VerticalAlignment::Center);
    TextBlock prayer;
    prayer.FontSize((double)g_fontSize);
    prayer.VerticalAlignment(VerticalAlignment::Center);
    prayer.Margin({5,0,4,0});
    SolidColorBrush foreground;
    foreground.Color(palette.foreground);
    prayer.Foreground(foreground);
    TextBlock countdown;
    countdown.FontSize((double)g_fontSize);
    countdown.VerticalAlignment(VerticalAlignment::Center);
    countdown.Margin({4,0,5,0});
    SolidColorBrush accent;
    accent.Color(palette.countdown);
    countdown.Foreground(accent);
    prayer.Text(L"Loading...");
    countdown.Text(L"--:--");
    content.Children().Append(prayer);
    if(g_theme==L"custom") {
        TextBlock separator;
        separator.Text(L"|");
        separator.FontSize((double)g_fontSize);
        separator.VerticalAlignment(VerticalAlignment::Center);
        separator.Margin({0,0,0,0});
        SolidColorBrush separatorBrush;
        separatorBrush.Color(palette.separator);
        separator.Foreground(separatorBrush);
        content.Children().Append(separator);
    }
    content.Children().Append(countdown);
    pillSurface.Child(content);
    box.Children().Append(pillSurface);
    if(g_position==L"center" || g_position==L"after-icons") {
        // A full-width Grid child is horizontally centered, then dynamically shifted.
        // It owns no grid column and must not change existing taskbar layout.
        // The pill remains the only hit-testable part of this overlay.
        auto grid=parent.try_as<Grid>();
        if(!grid) return;
        Grid::SetColumn(box,0);
        Grid::SetColumnSpan(box,std::max(1,(int)grid.ColumnDefinitions().Size()));
        Grid::SetRow(box,0);
        Grid::SetRowSpan(box,std::max(1,(int)grid.RowDefinitions().Size()));
        box.HorizontalAlignment(HorizontalAlignment::Center);
        box.VerticalAlignment(VerticalAlignment::Center);
        Canvas::SetZIndex(box,100);
        grid.Children().Append(box);
    } else {
        if(!slot::AcquireAt(parent,slotIndex,kLeaseName,g_lease)) {
            Wh_Log(L"[Mawaqit] Lease acquisition failed");return;
        }
        bool placed=false;
        try {placed=slot::PlaceChild(parent,g_lease,box);} catch(...) {
            slot::Release(parent,g_lease);throw;
        }
        if(!placed) {slot::Release(parent,g_lease);return;}
    }
    g_placeholder=box;
    g_pill=box;
    g_tapToken=box.Tapped([](auto const&,auto const&){TogglePrayerPopup();});
    g_tapRegistered=true;
    g_prayerText=prayer;
    g_countdownText=countdown;
    Wh_Log(L"[Mawaqit] Text controls attached, prayer=%d countdown=%d",
        g_prayerText?1:0,g_countdownText?1:0);
    g_parent=parent;
    if(g_position==L"before-icons" || g_position==L"after-icons") {
        auto grid=parent.try_as<Grid>();
        Panel iconsParent{nullptr}; FrameworkElement repeater{nullptr};
        if(grid && FindApplicationRepeater(LiveTaskbarRoot(),iconsParent,repeater)) {
            Wh_Log(L"[Mawaqit] Icon layout: requested=%s columns=%u repeaterColumn=%d repeaterSpan=%d boxColumn=%d boxSpan=%d parentW=%.1f repeaterW=%.1f boxW=%.1f",
                   g_position.c_str(),grid.ColumnDefinitions().Size(),
                   Grid::GetColumn(repeater),Grid::GetColumnSpan(repeater),
                   Grid::GetColumn(box),Grid::GetColumnSpan(box),
                   grid.ActualWidth(),repeater.ActualWidth(),box.ActualWidth());
        }
    }
    UpdateDynamicCenterOnUiThread();
    UpdateAfterIconsOnUiThread();
    UpdateCountdown();
    prayer.Text(g_showNext?g_label:L"");
    countdown.Text(g_showCountdown?g_countdown:L"");
    Wh_Log(L"[Mawaqit] Native XAML position %s inserted: %d physical px, %.1f DIP, DPI %.0f",g_position.c_str(),g_width,widthDip,dpi);
}

static void LogException(PCWSTR context) {
    try {throw;} catch(winrt::hresult_error const& e) {
        Wh_Log(L"[Mawaqit] %s failed HRESULT 0x%08X: %s",context,(unsigned)e.code().value,e.message().c_str());
    } catch(...) {Wh_Log(L"[Mawaqit] %s failed",context);}
}

static bool OnTaskbarThread(folder_menus_dispatch::ThreadProc proc) {
    HWND wnd=taskwin::FindCurrentProcessTaskbarWnd();
    return wnd && folder_menus_dispatch::RunFromWindowThread(wnd,proc,nullptr,
        L"Windhawk_Mawaqit_TaskbarDispatch");
}

static DWORD WINAPI StartWorker(void*) {
    // Explorer may create the taskbar's XAML tree after Windhawk initialization.
    for(int i=0;i<40 && !g_stopping.load();++i) {
        bool inserted=false;
        HWND wnd=taskwin::FindCurrentProcessTaskbarWnd();
        if(wnd) inserted=OnTaskbarThread([](void*) {InsertOnUiThread();});
        if(inserted && g_placeholder.get()) return 0;
        Sleep(1500);
    }
    return 0;
}
static HANDLE g_worker=nullptr;

// Runs only on the taskbar XAML thread. Re-resolve the live parent rather
// than trusting weak references to the previous taskbar visual tree.
static void EnsureTaskbarSlotOnUiThread() {
    if (g_stopping.load()) return;
    int index = -1;
    auto parent = ResolvePositionParent(index);
    if (!parent) return; // New taskbar tree may still be initializing.
    bool present = false;
    for (auto child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == kSlotName) {
            present = true;
            break;
        }
    }
    if (present) {
        g_taskbarRebuildPending.store(false, std::memory_order_release);
        return;
    }
    Wh_Log(L"[Mawaqit] Taskbar slot missing after recreation; restoring");
    RemoveOnUiThread();
    InsertOnUiThread();
    // Keep checking if insertion was deferred because the tree is incomplete.
    g_taskbarRebuildPending.store(false, std::memory_order_release);
}

static void UpdateClockOnUiThread() {
    if(g_stopping.load())return;
    // Check occasionally even without a StartTaskbar notification: some
    // Windows shell changes replace the XAML tree through other paths.
    static unsigned healthTick = 0;
    if (g_taskbarRebuildPending.exchange(false, std::memory_order_acq_rel) ||
        ++healthTick >= 5) {
        healthTick = 0;
        EnsureTaskbarSlotOnUiThread();
    }
    UpdateCountdown();
    auto prayer=g_prayerText;
    auto countdown=g_countdownText;
    if(prayer)prayer.Text(g_showNext?g_label:L"");
    if(countdown)countdown.Text(g_showCountdown?g_countdown:L"");
    UpdateDynamicCenterOnUiThread();
    UpdateAfterIconsOnUiThread();
}

// XAML DispatcherTimer did not tick in the taskbar's hosting context on the
// tested Windows build. A Win32 waitable worker supplies the cadence; all
// TextBlock writes still execute on the owning taskbar/XAML thread.
static DWORD WINAPI CountdownWorker(void*) {
    while(!g_stopping.load()) {
        DWORD wait=WaitForSingleObject(g_stopEvent,1000);
        if(wait!=WAIT_TIMEOUT) break;
        if(g_stopping.load()) break;
        if(!OnTaskbarThread([](void*){UpdateClockOnUiThread();})) {
            static unsigned failures=0;
            if(++failures<=3 || failures%60==0)
                Wh_Log(L"[Mawaqit] Countdown UI dispatch failed (%u)",failures);
        }
    }
    return 0;
}

// Return remaining cache lifetime only when it belongs to the selected mosque,
// includes today's date, and both cache files are present.
static DWORD FreshScheduleCacheRemainingMs(const std::wstring& mosque, int hours) {
    try {
        std::wstring dir=CacheDir();if(dir.empty())return 0;
        std::wstring slug;NativeUpdater::normalize(mosque,slug);
        std::ifstream metadata((dir+L"\\metadata.json").c_str(),std::ios::binary);
        if(!metadata)return 0;
        std::string json((std::istreambuf_iterator<char>(metadata)),std::istreambuf_iterator<char>());
        if(json.size()>65536)return 0;
        NativeUpdater::Parser parser{json};auto root=parser.value();
        if(root.key("mosqueIdentifier").s!=NativeUpdater::narrow(slug))return 0;
        SYSTEMTIME now;GetLocalTime(&now);
        if(root.key("scheduleYear").s!=std::to_string(now.wYear))return 0;
        std::wstring schedulePath=dir+L"\\schedule.txt";
        std::ifstream schedule(schedulePath.c_str(),std::ios::binary);
        if(!schedule)return 0;
        std::string today=NativeUpdater::date(now.wYear,now.wMonth,now.wDay)+"|";
        bool found=false;std::string line;
        while(std::getline(schedule,line))if(line.compare(0,today.size(),today)==0){found=true;break;}
        if(!found)return 0;
        WIN32_FILE_ATTRIBUTE_DATA attrs{};
        if(!GetFileAttributesExW((dir+L"\\metadata.json").c_str(),GetFileExInfoStandard,&attrs))return 0;
        FILETIME current;GetSystemTimeAsFileTime(&current);
        ULARGE_INTEGER updated{},nowTicks{};
        updated.LowPart=attrs.ftLastWriteTime.dwLowDateTime;updated.HighPart=attrs.ftLastWriteTime.dwHighDateTime;
        nowTicks.LowPart=current.dwLowDateTime;nowTicks.HighPart=current.dwHighDateTime;
        if(nowTicks.QuadPart<updated.QuadPart)return 0;
        ULONGLONG elapsedMs=(nowTicks.QuadPart-updated.QuadPart)/10000ULL;
        ULONGLONG lifetimeMs=(ULONGLONG)hours*60*60*1000;
        return elapsedMs<lifetimeMs?(DWORD)(lifetimeMs-elapsedMs):0;
    }catch(...){return 0;}
}

// A schedule covers one calendar year. Never sleep past local New Year's
// midnight with the previous year's data, even when the refresh interval
// (or its remaining cached lifetime) would otherwise cross that boundary.
// A short December 31 check also accommodates clock adjustments and sleep.
static DWORD LimitRefreshDelayAtYearEnd(DWORD delay) {
    SYSTEMTIME now{};
    GetLocalTime(&now);
    if(now.wMonth!=12 || now.wDay!=31)return delay;
    ULONGLONG secondsLeft=(23u-now.wHour)*3600ULL+
                           (59u-now.wMinute)*60ULL+(60u-now.wSecond);
    // Wake just after midnight, allowing Windows to advance the local date.
    ULONGLONG untilNewYearMs=(secondsLeft+2ULL)*1000ULL;
    DWORD capped=(DWORD)std::min<ULONGLONG>(untilNewYearMs,3600000ULL);
    return std::min(delay,capped);
}

static DWORD WINAPI ScheduleWorker(void*) {
    std::wstring initialMosque;int initialHours;
    AcquireSRWLockShared(&g_settingsLock);
    initialMosque=g_mosque;initialHours=g_refreshHours;
    ReleaseSRWLockShared(&g_settingsLock);
    DWORD delay=FreshScheduleCacheRemainingMs(initialMosque,initialHours);
    if(delay)Wh_Log(L"[Mawaqit] Fresh cached schedule; next download in %lu ms",(unsigned long)delay);
    while(!g_stopping.load()) {
        HANDLE events[]={g_stopEvent,g_refreshEvent};
        DWORD result=WaitForMultipleObjects(2,events,FALSE,LimitRefreshDelayAtYearEnd(delay));
        if(result==WAIT_OBJECT_0 || result==WAIT_FAILED)return 0;
        // A capped wait may expire before midnight. Recheck the cache and
        // retain its remaining lifetime instead of downloading every hour.
        if(result==WAIT_TIMEOUT && delay>3600000u) {
            SYSTEMTIME current{};GetLocalTime(&current);
            if(current.wMonth==12 && current.wDay==31) {
                std::wstring cachedMosque;int cachedHours;
                AcquireSRWLockShared(&g_settingsLock);
                cachedMosque=g_mosque;cachedHours=g_refreshHours;
                ReleaseSRWLockShared(&g_settingsLock);
                DWORD remaining=FreshScheduleCacheRemainingMs(cachedMosque,cachedHours);
                if(remaining) {delay=remaining;continue;}
            }
        }
        std::wstring mosque;int hours;
        AcquireSRWLockShared(&g_settingsLock);
        mosque=g_mosque;hours=g_refreshHours;
        ReleaseSRWLockShared(&g_settingsLock);
        Wh_Log(L"[Mawaqit] Refresh started: mosque=%s interval=%d hours",mosque.c_str(),hours);
        bool ok=false;
        try {ok=NativeUpdater::Run(mosque);}
        catch(const std::exception& e){Wh_Log(L"[Mawaqit] Schedule update: %hs",e.what());}
        catch(...){Wh_Log(L"[Mawaqit] Schedule update failed");}
        if(g_stopping.load())return 0;
        if(ok) {
            Wh_Log(L"[Mawaqit] Schedule updated");
            OnTaskbarThread([](void*){
                g_loadedDate.clear();g_today.clear();g_tomorrow.clear();
                LoadMetadata();UpdateClockOnUiThread();
            });
        }
        delay=(DWORD)(ok?hours:1)*60u*60u*1000u;
        Wh_Log(L"[Mawaqit] Refresh finished: success=%d next-delay=%lu ms",ok?1:0,(unsigned long)delay);
    }
    return 0;
}

static std::wstring ReadSetting(PCWSTR key) {
    PCWSTR raw=Wh_GetStringSetting(key);
    std::wstring result=raw?raw:L"";
    if(raw)Wh_FreeStringSetting(raw);
    return result;
}
static void LoadModSettings() {
    std::wstring position=ReadSetting(L"General.Position");
    if(position!=L"after-tray" && position!=L"before-icons" &&
       position!=L"after-icons" && position!=L"center") position=L"before-tray";
    g_position=position;
    auto theme=ReadSetting(L"General.Theme");
    if(theme!=L"traditional" && theme!=L"catppuccin" && theme!=L"emberveil" &&
       theme!=L"emberveil-midnight" && theme!=L"emerald-mosque" &&
       theme!=L"dracula" && theme!=L"nord" && theme!=L"gruvbox" &&
       theme!=L"tokyo-night" && theme!=L"solarized-dark" &&
       theme!=L"solarized-light" && theme!=L"custom")theme=L"system";
    g_theme=theme;
    int width=Wh_GetIntSetting(L"AdvancedCustomization.LayoutSettings.Width");
    g_width=(width>=180&&width<=400)?width:200;
    int height=Wh_GetIntSetting(L"AdvancedCustomization.LayoutSettings.PillHeight");
    g_pillHeight=(height>=0&&height<=100)?height:0;
    int gap=Wh_GetIntSetting(L"AdvancedCustomization.LayoutSettings.TrayGap");
    g_trayGap=(gap>=0&&gap<=100)?gap:5;
    int radius=Wh_GetIntSetting(L"AdvancedCustomization.LayoutSettings.CornerRadius");
    g_cornerRadius=(radius>=0&&radius<=30)?radius:13;
    int font=Wh_GetIntSetting(L"AdvancedCustomization.LayoutSettings.FontSize");
    g_fontSize=(font>=10&&font<=24)?font:15;
    PCWSTR marginSetting=Wh_GetStringSetting(L"AdvancedCustomization.LayoutSettings.Margins");
    if(marginSetting){g_margins=ParseInsets(marginSetting,-100,100);Wh_FreeStringSetting(marginSetting);}
    PCWSTR paddingSetting=Wh_GetStringSetting(L"AdvancedCustomization.LayoutSettings.Padding");
    if(paddingSetting){g_padding=ParseInsets(paddingSetting,0,100);Wh_FreeStringSetting(paddingSetting);}
    g_showNext=Wh_GetIntSetting(L"Display.ShowNextPrayer")!=0;
    g_showCountdown=Wh_GetIntSetting(L"Display.ShowCountdown")!=0;
    g_enablePopup=Wh_GetIntSetting(L"Display.EnablePopup")!=0;
    int autoClose=std::clamp(Wh_GetIntSetting(L"Display.PopupAutoCloseSeconds"),0,300);
    g_popupAutoCloseSeconds.store(autoClose);
    Wh_Log(L"[Mawaqit] Popup auto-close configured: %d seconds",autoClose);
    std::wstring mosque=ReadSetting(L"General.Mosque");
    if(mosque.empty())mosque=L"al-haram-makkah-saudi-arabia";
    int hours=Wh_GetIntSetting(L"AdvancedCustomization.ModSettingsAdvanced.RefreshHours");
    if(hours<1||hours>168)hours=24;
    AcquireSRWLockExclusive(&g_settingsLock);
    bool changed=g_mosque!=mosque || g_refreshHours!=hours;
    g_mosque=mosque;g_refreshHours=hours;
    ReleaseSRWLockExclusive(&g_settingsLock);
    Wh_Log(L"[Mawaqit] Settings: next=%d countdown=%d popup=%d theme=%s mosque=%s refresh=%d h changed=%d",
           g_showNext?1:0,g_showCountdown?1:0,g_enablePopup?1:0,g_theme.c_str(),mosque.c_str(),hours,changed?1:0);
    if(changed && g_refreshEvent){
        Wh_Log(L"[Mawaqit] Requesting schedule refresh after settings change");
        SetEvent(g_refreshEvent);
    }
}

BOOL Wh_ModInit() {
    g_stopping=false;
    LoadModSettings();
    g_stopEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    g_refreshEvent=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if(!g_stopEvent||!g_refreshEvent) return FALSE;
    folder_menus_dispatch::SetExceptionLogger(LogException);
    if(!taskxaml::HookTaskbarSymbols([]() {
        // Hook may fire while Explorer is constructing the new taskbar.
        // Defer all XAML access to the regular UI-thread countdown tick.
        g_taskbarRebuildPending.store(true, std::memory_order_release);
    })) {
        Wh_Log(L"[Mawaqit] Failed to hook taskbar symbols");return FALSE;
    }
    return TRUE;
}
void Wh_ModAfterInit() {
    g_worker=CreateThread(nullptr,0,StartWorker,nullptr,0,nullptr);
    g_updateWorker=CreateThread(nullptr,0,ScheduleWorker,nullptr,0,nullptr);
    g_countdownWorker=CreateThread(nullptr,0,CountdownWorker,nullptr,0,nullptr);
}
void Wh_ModSettingsChanged() {
    Wh_Log(L"[Mawaqit] Settings change received; rebuilding native slot");
    // Keep UI-owned settings and XAML rebuild on the same taskbar thread.
    // Previously LoadModSettings() ran on the Windhawk settings callback thread,
    // racing with UI reads of g_theme, g_position and layout values.
    // The schedule worker independently snapshots mosque/interval under g_settingsLock.
    bool dispatched=OnTaskbarThread([](void*){
        LoadModSettings();
        if(g_popup && IsWindowVisible(g_popup)){
            if(!g_enablePopup) HidePrayerPopup();
            else {
                int seconds=g_popupAutoCloseSeconds.load();
                g_popupAutoCloseDeadline=seconds>0?GetTickCount64()+static_cast<ULONGLONG>(seconds)*1000:0;
            }
        }
        RemoveOnUiThread();InsertOnUiThread();
    });
    Wh_Log(L"[Mawaqit] Settings rebuild dispatched=%d",dispatched?1:0);
}
void Wh_ModUninit() {
    g_stopping=true;
    if(g_stopEvent)SetEvent(g_stopEvent);
    if(g_refreshEvent)SetEvent(g_refreshEvent);
    if(g_worker){WaitForSingleObject(g_worker,INFINITE);CloseHandle(g_worker);g_worker=nullptr;}
    if(g_updateWorker){WaitForSingleObject(g_updateWorker,INFINITE);CloseHandle(g_updateWorker);g_updateWorker=nullptr;}
    if(g_countdownWorker){WaitForSingleObject(g_countdownWorker,INFINITE);CloseHandle(g_countdownWorker);g_countdownWorker=nullptr;}
    if(!OnTaskbarThread([](void*){RemoveOnUiThread();}))
        Wh_Log(L"[Mawaqit] Teardown could not reach taskbar thread");
    if(g_refreshEvent){CloseHandle(g_refreshEvent);g_refreshEvent=nullptr;}
    if(g_stopEvent){CloseHandle(g_stopEvent);g_stopEvent=nullptr;}
}
