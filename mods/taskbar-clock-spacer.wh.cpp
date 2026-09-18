// ==WindhawkMod==
// @id              taskbar-clock-spacer
// @name            Taskbar Clock Spacer
// @description     Companion for Taskbar Clock Customization: adds a %s% elastic spacer token that distributes leftover clock width between items. Windows 11 only.
// @version         1.1
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Clock Spacer

Adds a `%s%` elastic spacer token to the Windows 11 taskbar clock, so clock items
can be pushed apart to fill a fixed width instead of bunching together.

![Clock Spacer distributing a custom clock across multiple rows](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-clock-spacer/assets/clock-spacer-working.png)
*User-confirmed working configuration, September 9, 2026, with system stats,
time, date, and weather arranged across a fixed-width clock.*

## Two requirements — please read before installing

**1. This mod does nothing on its own.** It is a companion for
[Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization).
That mod produces the clock text; this mod only rearranges it. Install and
configure that mod first.

**2. The clock needs a fixed width.** An elastic spacer distributes *leftover*
width. If the clock sizes itself to its own text there is no leftover width,
every gap computes to zero, and the result looks exactly as if the mod were not
installed. Set a fixed width using either:

- **Max width** in Taskbar Clock Customization's settings, or
- **Max clock width** in this mod's settings.

Either one works. 120 px is a reasonable starting point.

Windows 11 only. This mod does not work on Windows 10.

## What it does

Put `%s%` between items in the clock's Top Line or Bottom Line format. Each `%s%`
becomes a gap, and all leftover width is shared out evenly between the gaps.

| Format | Result |
| --- | --- |
| `%time%%s%%date%` | time hugs the left edge, date hugs the right, gap fills the middle |
| `%time%%s%%date%%s%%weekday%` | three items, two equal gaps |
| `%time%%s%%date%%s%%s%%weekday%` | Double-spacer: more space is weighted between date and weekday |

The first item always hugs the left edge and the last always hugs the right edge,
so the line stays anchored as the text changes width.

### Spacers inside the weather

The weather service substitutes `%s` as its sunset token, so `%s%` cannot be
written inside Taskbar Clock Customization's **Weather format**. Write
`{spacer}` there instead, for example:

```
%c{spacer}🌡️%t{spacer}🌬️%w
```

`{spacer}` passes through the weather service verbatim, arrives in the clock
line, and becomes the same elastic gap as `%s%` — so weather items justify
with the rest of the clock.

## Setup

1. Install **Taskbar Clock Customization** and set up your clock format.
2. Set a **Max width** in its settings, for example `120`.
3. Install this mod.
4. Edit the clock mod's **Top line** or **Bottom line** to put `%s%` between
   items, for example `%time%%s%%date%`.

The `%s%` token passes through Taskbar Clock Customization untouched and is
interpreted here at display time.

## Troubleshooting

**`%s%` disappears and nothing moves.** This is the fixed-width problem in
requirement 2 above. Set a **Max width** in Taskbar Clock Customization, or a
**Max clock width** here. The mod also writes a one-line explanation to the
Windhawk log the first time it detects this.

**Nothing happens at all.** Confirm Taskbar Clock Customization is installed and
enabled, and that `%s%` is in its **Top line** or **Bottom line** setting — not in
the tooltip, the middle line, or the weather format.

**The spacer works but the clock is the wrong width.** Adjust the same Max width
value. Use **Line width override** only if the automatic width is being read
incorrectly.

## Settings

- **Line width override** — explicit width for the spacer grid. Usually `0`
  (automatic) is correct; the width is inherited from the clock's Max width.
- **Max clock width** — fixed width for the generated spacer rows. Equivalent
  to setting Max width in Taskbar Clock Customization; that mod's own Max width
  is respected automatically when this is `0`.
- **Minimum spacer width** — a floor, in pixels, for every gap. `0` (the default)
  leaves gaps fully elastic. A small value such as `8` guarantees a visible gap
  even before a fixed clock width is configured.

## Limitations

- `%s%` is interpreted after Taskbar Clock Customization expands its format
  tokens, so it works in the top and bottom line formats. Inside the composite
  weather segment use `{spacer}` instead — the weather service would consume
  `%s%` as its sunset token.
- Lines without `%s%` are left completely alone — the mod is a no-op for them.
- Font, size, and color of the spaced segments follow the original clock text's
  current style, so the clock mod's style settings continue to apply.

## How it works

The mod hooks `DateTimeIconContent::OnApplyTemplate` in the system tray and
watches the clock's time and date text blocks. When a line contains `%s%`, the
source text block is collapsed and a generated panel is inserted in its place:
each line becomes a Grid whose text segments sit in `Auto` columns separated by
`Star` columns, and the star columns absorb the leftover width. When only the
text changes — which happens every second — the existing segments are rewritten
in place rather than rebuilt, so the visual tree stays stable.

## Relationship to Taskbar Clock Customization

The spacer was first offered as a patch to Taskbar Clock Customization itself
([m417z/my-windhawk-mods#68](https://github.com/m417z/my-windhawk-mods/pull/68)).
The maintainer preferred an approach that does not add generated layout elements,
so this companion mod carries the feature separately and leaves that mod
untouched.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- lineWidth: 0
  $name: Line width override (px, 0 = auto)
  $description: >-
    Explicit width for the spacer grid. Usually 0 is correct — the clock area
    inherits its width from the Max width set in Taskbar Clock Customization.
    Set this only if the spacer doesn't expand as expected.

- maxWidth: 0
  $name: Max clock width (px, 0 = off)
  $description: >-
    Fixed width for the generated spacer rows. Equivalent to setting Max width
    in Taskbar Clock Customization — use whichever you prefer; that mod's own
    Max width is respected automatically when this is 0.

- minSpacerWidth: 0
  $name: Minimum spacer width (px, 0 = off)
  $description: >-
    A floor for every gap. 0 keeps gaps fully elastic. A small value such as 8
    guarantees a visible gap even before a fixed clock width is configured.
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <cmath>
#include <functional>
#include <string>
#include <vector>

#include <windhawk_utils.h>
#include <winver.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ============================================================
// Visual tree walk
// Template block: _templates/visual-tree-walk.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

namespace windhawk_mod_templates::visual_tree_walk {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::Controls::StackPanel;
using winrt::Windows::UI::Xaml::Media::VisualTreeHelper;

// Depth-first visit of every FrameworkElement descendant (root excluded).
// The visitor returns true to stop the walk early.
inline bool ForEachDescendant(
    FrameworkElement const& root, int maxDepth,
    std::function<bool(FrameworkElement const&, int)> const& visit,
    int depth = 0) {
    if (!root || depth >= maxDepth)
        return false;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child =
            VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child)
            continue;
        if (visit(child, depth + 1))
            return true;
        if (ForEachDescendant(child, maxDepth, visit, depth + 1))
            return true;
    }
    return false;
}

// First descendant matching the predicate, depth-first document order.
inline FrameworkElement FindDescendant(
    FrameworkElement const& root, int maxDepth,
    std::function<bool(FrameworkElement const&)> const& predicate) {
    FrameworkElement found = nullptr;
    ForEachDescendant(root, maxDepth,
                      [&](FrameworkElement const& element, int) {
                          if (predicate(element)) {
                              found = element;
                              return true;
                          }
                          return false;
                      });
    return found;
}

// Every descendant matching the predicate, in depth-first document order —
// which is also visual order for the tray's horizontal stacks.
inline void CollectDescendants(
    FrameworkElement const& root, int maxDepth,
    std::function<bool(FrameworkElement const&)> const& predicate,
    std::vector<FrameworkElement>& out) {
    ForEachDescendant(root, maxDepth,
                      [&](FrameworkElement const& element, int) {
                          if (predicate(element))
                              out.push_back(element);
                          return false;
                      });
}

// The OmniButton battery walk: the first non-items-host StackPanel
// descendant — the inner panel whose children are the individually
// addressable native elements (glyph, percent, per-icon views).
inline StackPanel FindInnerStackPanel(FrameworkElement const& root,
                                      int maxDepth) {
    StackPanel found = nullptr;
    ForEachDescendant(root, maxDepth,
                      [&](FrameworkElement const& element, int) {
                          auto panel = element.try_as<StackPanel>();
                          if (panel && !panel.IsItemsHost()) {
                              found = panel;
                              return true;
                          }
                          return false;
                      });
    return found;
}

}  // namespace windhawk_mod_templates::visual_tree_walk

namespace vtw = windhawk_mod_templates::visual_tree_walk;

// ============================================================
// Settings IO
// Template block: _templates/settings-io.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

namespace windhawk_mod_templates::settings_io {

inline int Clamp(int value, int low, int high) {
    return std::max(low, std::min(high, value));
}

// Frees on every path, including the ones a hand-written loader forgets.
class StringSetting {
public:
    explicit StringSetting(PCWSTR key) : value_(Wh_GetStringSetting(key)) {}
    ~StringSetting() {
        if (value_) Wh_FreeStringSetting(value_);
    }
    StringSetting(StringSetting const&) = delete;
    StringSetting& operator=(StringSetting const&) = delete;

    // Never nullptr in practice, but do not rely on that at the call site.
    PCWSTR Get() const { return value_ ? value_ : L""; }
    bool Empty() const { return !value_ || !value_[0]; }

private:
    PCWSTR value_ = nullptr;
};

// Copy a string setting into a fixed buffer, always NUL-terminated. Fixed
// buffers rather than std::wstring because a namespace-scope settings struct
// must not own heap — see the exit-time destructor audit.
template <size_t N>
inline void LoadString(PCWSTR key, wchar_t (&buffer)[N]) {
    StringSetting setting(key);
    if (setting.Empty()) {
        buffer[0] = L'\0';
        return;
    }
    wcsncpy(buffer, setting.Get(), N - 1);
    buffer[N - 1] = L'\0';
}

// Same, but substitutes `fallback` when the setting is empty.
template <size_t N>
inline void LoadString(PCWSTR key, wchar_t (&buffer)[N], PCWSTR fallback) {
    LoadString(key, buffer);
    if (!buffer[0] && fallback) {
        wcsncpy(buffer, fallback, N - 1);
        buffer[N - 1] = L'\0';
    }
}

inline int LoadInt(PCWSTR key, int low, int high) {
    return Clamp(Wh_GetIntSetting(key), low, high);
}

inline bool LoadBool(PCWSTR key) {
    return Wh_GetIntSetting(key) != 0;
}

// A $options choice, matched case-insensitively against a table of tokens.
// Returns the matching entry's value, or `fallback` when nothing matches —
// which also covers the unset case, since an unset string is empty.
//
// Use this rather than a chain of _wcsicmp: after ANY option is renamed, a
// stale literal in a hand-written chain fails silently and the mod quietly
// falls back. That cost this lab a release (Indicator symbols reverted to
// numbers because `labelFormat == L"dot"` was never true again).
template <typename T>
struct Choice {
    wchar_t const* token;
    T value;
};

template <typename T, size_t N>
inline T LoadChoice(PCWSTR key, Choice<T> const (&choices)[N], T fallback) {
    StringSetting setting(key);
    if (setting.Empty()) return fallback;
    for (auto const& choice : choices) {
        if (_wcsicmp(setting.Get(), choice.token) == 0) return choice.value;
    }
    return fallback;
}

}  // namespace windhawk_mod_templates::settings_io

namespace sio = windhawk_mod_templates::settings_io;

// ============================================================
// Taskbar host
// Template block: _templates/taskbar-host.h (verbatim copy —
// keep in sync with the template; Windhawk mods are single-file).
// ============================================================

namespace windhawk_mod_templates::taskbar_host {

using winrt::Windows::UI::Xaml::FrameworkElement;
using winrt::Windows::UI::Xaml::XamlRoot;

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
// with it — which is exactly what an earlier revision of this template did.
// Atomic because the caller may be the retry thread while the hook proc runs
// on the taskbar's UI thread. RegisterWindowMessageW returns the same value
// for the same string for the lifetime of the session, so this settles on one
// value immediately and never changes again — the pre-template code got the
// same property from a function-local `static UINT` magic static, which a
// parameterised template cannot use.
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
                            reinterpret_cast<Dispatch*>(call->lParam)) {
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
        if (g_logException) g_logException(L"TrayUI::StartTaskbar hook");
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

// ---- Taskbar metrics and orientation ----------------------------------------
//
// WHERE THE TASKBAR IS, AND WHETHER THIS FAMILY CAN WORK THERE.
//
// Windows 11 itself only puts the taskbar at the bottom. Two mods by m417z
// move it, and both are first-class parts of the ecosystem these mods have to
// live in:
//
//   taskbar-on-top       — bottom -> top. FINE for this family. Everything
//                          here is positioned relative to the taskbar's own
//                          XAML tree, never to screen coordinates, so a top
//                          taskbar is the same tree at a different y.
//
//   taskbar-vertical     — bottom -> left/right. NOT COMPATIBLE, and not for
//                          a reason cooperation can fix. It walks the very
//                          same path this family walks
//                          (ControlCenterButton > Grid > ContentPresenter >
//                          ItemsPresenter > StackPanel) and applies a
//                          RotateTransform to `RenderTransform` on those
//                          children. Positioning here sets a
//                          TranslateTransform on the SAME property of the SAME
//                          elements. One dependency property, two owners, last
//                          writer wins — there is no version of this where
//                          both mods are correct. m417z documents the same
//                          class of conflict for taskbar-multirow.
//
// So: DETECT AND STAND DOWN, loudly, rather than fight and paint garbage. The
// detection is the taskbar's own rect aspect, not a check for a specific mod —
// it is the condition that matters, and it stays true however the taskbar got
// that way.
//
// The rect is in PHYSICAL pixels and every XAML size is a DIP, so the DIP
// conversion lives here too rather than being re-derived per mod. That is the
// bug that was blocking on PR #4855 and #4843.

enum class Orientation { Horizontal, Vertical };

struct Metrics {
    bool valid = false;
    RECT rect{};
    UINT dpi = 96;
    Orientation orientation = Orientation::Horizontal;
    // The extent this family's grid has to fit INTO: the taskbar's height when
    // it runs across the screen, its width when it runs down the side.
    double constrainedDip = 0.0;
    // The extent it can run ALONG.
    double alongDip = 0.0;
};

inline Metrics GetMetrics(HWND taskbarWnd) {
    Metrics metrics;
    if (!taskbarWnd || !GetWindowRect(taskbarWnd, &metrics.rect))
        return metrics;

    metrics.valid = true;
    metrics.dpi = GetDpiForWindow(taskbarWnd);
    if (!metrics.dpi) metrics.dpi = 96;

    double width = (double)(metrics.rect.right - metrics.rect.left);
    double height = (double)(metrics.rect.bottom - metrics.rect.top);
    double scale = 96.0 / (double)metrics.dpi;

    // Taller than wide means it runs down a side. Nothing else can produce
    // that shape, so this needs no cooperation from whatever moved it.
    metrics.orientation =
        height > width ? Orientation::Vertical : Orientation::Horizontal;
    if (metrics.orientation == Orientation::Horizontal) {
        metrics.constrainedDip = height * scale;
        metrics.alongDip = width * scale;
    } else {
        metrics.constrainedDip = width * scale;
        metrics.alongDip = height * scale;
    }
    return metrics;
}

// Whether this family's layout model applies at all. A mod must check this
// BEFORE touching anything and stand down cleanly if it is false — leaving the
// taskbar exactly as it found it — rather than arranging into a coordinate
// space someone else is rotating.
inline bool LayoutModelApplies(Metrics const& metrics) {
    return metrics.valid && metrics.orientation == Orientation::Horizontal;
}

inline wchar_t const* OrientationName(Orientation orientation) {
    return orientation == Orientation::Vertical ? L"vertical" : L"horizontal";
}

// ---- Bounded retry ----------------------------------------------------------
//
// Stoppable and WAITED during unload. A detached thread that outlives
// Wh_ModUninit runs mod code out of an unloaded DLL.

class RetryLoop {
public:
    // applied: has the work finished? unloading: stop immediately.
    using AppliedFn = bool (*)();
    using AttemptFn = void (*)();

    void Start(AttemptFn attempt, AppliedFn applied,
               std::atomic<bool> const& unloading, int attempts = 5,
               DWORD intervalMs = 2000) {
        Stop();
        if (unloading) return;
        attempt_ = attempt;
        applied_ = applied;
        unloading_ = &unloading;
        attempts_ = attempts;
        intervalMs_ = intervalMs;
        stopEvent_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stopEvent_) return;
        thread_ = CreateThread(
            nullptr, 0,
            [](void* parameter) -> DWORD {
                auto* self = static_cast<RetryLoop*>(parameter);
                for (int i = 0; i < self->attempts_ && !*self->unloading_;
                     ++i) {
                    if (self->applied_ && self->applied_()) break;
                    if (i && WaitForSingleObject(self->stopEvent_,
                                                 self->intervalMs_) !=
                                 WAIT_TIMEOUT)
                        break;
                    if (self->attempt_) self->attempt_();
                }
                return 0;
            },
            this, 0, nullptr);
        if (!thread_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
    }

    // Pumps sent messages while waiting: the retry thread marshals onto the UI
    // thread with SendMessage, so a plain wait from that same UI thread would
    // deadlock against the thread it is waiting for.
    void Stop() {
        if (stopEvent_) SetEvent(stopEvent_);
        if (thread_) {
            DWORD result;
            do {
                result = MsgWaitForMultipleObjects(1, &thread_, FALSE, INFINITE,
                                                   QS_SENDMESSAGE);
                if (result == WAIT_OBJECT_0 + 1) {
                    MSG message;
                    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
                }
            } while (result == WAIT_OBJECT_0 + 1);
            CloseHandle(thread_);
            thread_ = nullptr;
        }
        if (stopEvent_) {
            CloseHandle(stopEvent_);
            stopEvent_ = nullptr;
        }
    }

private:
    HANDLE thread_ = nullptr;
    HANDLE stopEvent_ = nullptr;
    AttemptFn attempt_ = nullptr;
    AppliedFn applied_ = nullptr;
    std::atomic<bool> const* unloading_ = nullptr;
    int attempts_ = 5;
    DWORD intervalMs_ = 2000;
};

}  // namespace windhawk_mod_templates::taskbar_host

namespace tbh = windhawk_mod_templates::taskbar_host;

// ============================================================
// Settings
// ============================================================

struct ModSettings {
    int lineWidth = 0;
    int maxWidth = 0;
    int minSpacerWidth = 0;
};
static ModSettings g_settings;

static void LoadSettings() {
    // sio::LoadInt reads and clamps in one step, so a negative value can never
    // reach the layout code even if a clamp line is later edited away.
    g_settings.lineWidth = sio::LoadInt(L"lineWidth", 0, INT_MAX);
    g_settings.maxWidth = sio::LoadInt(L"maxWidth", 0, INT_MAX);
    g_settings.minSpacerWidth = sio::LoadInt(L"minSpacerWidth", 0, INT_MAX);
}

// Generated subtrees are reused across clock ticks. They must be rebuilt when a
// setting that changes their *shape* changes, so the layout settings are folded
// into a key that is stored alongside each generated panel.
static uint64_t CurrentLayoutKey() {
    return (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.maxWidth))) |
           (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.lineWidth)) << 20) |
           (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.minSpacerWidth)) << 40);
}

// ============================================================
// GetTaskbarXamlRoot
// ============================================================

// The CTaskBand walk, the runtime-disassembled FrameworkElement offset and the
// taskbar.dll symbol hooks are all _templates/taskbar-host.h now.
static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    return tbh::GetTaskbarXamlRoot(hTaskbarWnd);
}

// ============================================================
// Globals
// ============================================================

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_scanDone{false};
static std::atomic<bool> g_systemTrayModuleHooked{false};
static std::atomic<bool> g_warnedNoElasticRoom{false};
static HANDLE g_scanThread = nullptr;
static HANDLE g_scanStopEvent = nullptr;

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName);
static void StartInitialScan();

static constexpr PCWSTR kSpacerToken    = L"%s%";
static constexpr size_t kSpacerTokenLen = 3;
// Weather-format spacer: wttr.in substitutes %s (sunset), so %s% cannot be
// written inside Taskbar Clock Customization's Weather format. A literal
// {spacer} instead rides through the wttr.in request untouched and arrives in
// the clock line text, where it splits exactly like %s%.
static constexpr PCWSTR kWeatherSpacerToken    = L"{spacer}";
static constexpr size_t kWeatherSpacerTokenLen = 8;
static constexpr PCWSTR kDateBlock      = L"DateInnerTextBlock";
static constexpr PCWSTR kTimeBlock      = L"TimeInnerTextBlock";

struct SpacerState {
    winrt::weak_ref<TextBlock>  originalRef;
    winrt::weak_ref<StackPanel> parentRef;
    winrt::weak_ref<StackPanel> generatedRef;
    uint64_t                    generatedLayoutKey = 0;
    int64_t                     textToken = 0;
};

// Wh_ModUninit is not called when Explorer terminates. Without this the vector's
// destructor runs on the shutdown thread and releases XAML weak references off
// their UI thread.
// SpacerState holds only winrt::weak_ref and integers; weak_ref release is an
// in-process refcount decrement, safe from any thread at shutdown, so the normal
// destructor is correct and leak-free. A bare no_destroy here would only leak
// the vector buffer on every unload (m417z, #4443).
static std::vector<SpacerState> g_states;  // exit-time-safe: heap-only

// ============================================================
// XAML helpers
// ============================================================

// Kept as a thin wrapper: the signature takes its predicate by value and the
// call sites rely on that. The walk itself is now the template's.
static FrameworkElement FindChildRecursive(FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& cb, int maxDepth = 20) {
    return vtw::FindDescendant(
        element, maxDepth,
        [&cb](FrameworkElement const& child) { return cb(child); });
}

// ============================================================
// Spacer geometry
// ============================================================

static size_t FindNextSpacer(std::wstring_view text, size_t pos,
                             size_t* tokenLen) {
    size_t plain = text.find(kSpacerToken, pos);
    size_t weather = text.find(kWeatherSpacerToken, pos);
    if (weather < plain) {
        *tokenLen = kWeatherSpacerTokenLen;
        return weather;
    }
    *tokenLen = kSpacerTokenLen;
    return plain;
}

static bool HasSpacerToken(std::wstring_view text) {
    size_t tokenLen;
    return FindNextSpacer(text, 0, &tokenLen) != std::wstring_view::npos;
}

static std::vector<std::wstring> SplitOnSpacer(std::wstring_view text) {
    std::vector<std::wstring> segments;
    size_t pos = 0;
    while (true) {
        size_t tokenLen;
        size_t found = FindNextSpacer(text, pos, &tokenLen);
        if (found == std::wstring_view::npos) {
            segments.emplace_back(text.substr(pos));
            break;
        }
        segments.emplace_back(text.substr(pos, found - pos));
        pos = found + tokenLen;
    }
    return segments;
}

static std::vector<std::wstring> SplitLines(std::wstring_view text) {
    std::vector<std::wstring> lines;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t found = text.find(L'\n', pos);
        if (found == std::wstring_view::npos) {
            lines.emplace_back(text.substr(pos));
            break;
        }
        size_t end = found;
        if (end > pos && text[end - 1] == L'\r') end--;
        lines.emplace_back(text.substr(pos, end - pos));
        pos = found + 1;
    }
    return lines;
}

static void CopyTextStyle(TextBlock src, TextBlock dst) {
    dst.FontSize(src.FontSize());
    dst.FontFamily(src.FontFamily());
    dst.FontWeight(src.FontWeight());
    dst.FontStyle(src.FontStyle());
    dst.FontStretch(src.FontStretch());
    dst.CharacterSpacing(src.CharacterSpacing());
    dst.Foreground(src.Foreground());
    dst.TextAlignment(src.TextAlignment());
    dst.TextWrapping(TextWrapping::NoWrap);
}

// The first and last segments must hug the fixed clock edges, otherwise each
// Auto column centers its text and the gaps look uneven.
static void ApplySegmentAlignment(TextBlock textBlock, int index, int count) {
    if (count <= 1) {
        textBlock.HorizontalAlignment(HorizontalAlignment::Stretch);
        return;
    }
    if (index == 0) {
        textBlock.HorizontalAlignment(HorizontalAlignment::Left);
        textBlock.TextAlignment(TextAlignment::Left);
    } else if (index == count - 1) {
        textBlock.HorizontalAlignment(HorizontalAlignment::Right);
        textBlock.TextAlignment(TextAlignment::Right);
    } else {
        textBlock.HorizontalAlignment(HorizontalAlignment::Center);
        textBlock.TextAlignment(TextAlignment::Center);
    }
}

static double EffectiveLineWidth(TextBlock original, StackPanel parent) {
    if (g_settings.lineWidth > 0)
        return (double)g_settings.lineWidth;
    if (g_settings.maxWidth > 0)
        return (double)g_settings.maxWidth;
    // Taskbar Clock Customization applies its "Max width" setting as MaxWidth
    // on this same StackPanel, so a finite value there is the fixed clock
    // width to fill. It must be read as a *setting*, never measured:
    // deriving the width from ActualWidth and then setting Width feeds the
    // next measurement, and the clock ratchets permanently wider every time
    // any line's text gets longer (live-observed as multiplying gaps).
    if (parent) {
        double parentMax = parent.MaxWidth();
        if (std::isfinite(parentMax) && parentMax > 1.0)
            return parentMax;
    }
    return 0.0;
}

// The generated panel is pinned to exactly the effective width (min AND max).
// The values are constants from settings, never measurements, so there is no
// feedback ratchet. Both bounds matter: MinWidth expands short content to the
// fixed clock width; MaxWidth stops a naturally wider line from dragging the
// panel past it — a StackPanel arranges a child at max(slot, desired), so an
// uncapped panel would exceed Taskbar Clock Customization's Max width and
// stretch every spaced row with it.
static void ApplyPanelWidthConstraint(FrameworkElement element, double width) {
    if (!element) return;
    if (width > 1.0) {
        element.MinWidth(width);
        element.MaxWidth(width);
    } else {
        element.ClearValue(FrameworkElement::MinWidthProperty());
        element.ClearValue(FrameworkElement::MaxWidthProperty());
    }
}

// Rows only get the cap. They stretch to the pinned panel width, and an
// unspaced over-long line (for example the weather line) clips at the fixed
// width exactly like the native text block does under TCC's Max width.
static void ApplyRowWidthCap(FrameworkElement element, double width) {
    if (!element) return;
    if (width > 1.0)
        element.MaxWidth(width);
    else
        element.ClearValue(FrameworkElement::MaxWidthProperty());
}

static void WarnIfNoElasticRoom(bool hasElasticRoom) {
    if (hasElasticRoom || g_warnedNoElasticRoom.exchange(true))
        return;
    Wh_Log(L"[Spacer] No spare width to distribute, so %%s%% produces no visible "
           L"gap. Set 'Max width' in Taskbar Clock Customization, or 'Max clock "
           L"width' in this mod, to give the spacer room to expand.");
}

// ============================================================
// Spacer grid construction
// ============================================================

// Layout: [Auto text] [* gap] [Auto text] [* gap] ... [Auto text]
static Grid BuildSpacerGrid(winrt::hstring const& name,
                            const std::vector<std::wstring>& segments,
                            TextBlock styleSource,
                            double width) {
    Grid grid;
    grid.Name(name + L"_Spacer");
    grid.HorizontalAlignment(HorizontalAlignment::Stretch);
    grid.VerticalAlignment(VerticalAlignment::Center);
    ApplyRowWidthCap(grid, width);

    double minSpacer = (double)g_settings.minSpacerWidth;

    int segmentCount = (int)segments.size();
    for (int i = 0; i < segmentCount; i++) {
        ColumnDefinition textColumn;
        textColumn.Width({1.0, GridUnitType::Auto});
        grid.ColumnDefinitions().Append(textColumn);

        if (i + 1 < segmentCount) {
            ColumnDefinition spacerColumn;
            spacerColumn.Width({1.0, GridUnitType::Star});
            if (minSpacer > 0.0)
                spacerColumn.MinWidth(minSpacer);
            grid.ColumnDefinitions().Append(spacerColumn);
        }
    }

    int gridColumn = 0;
    for (int i = 0; i < segmentCount; i++) {
        TextBlock textBlock;
        textBlock.Text(segments[i]);
        textBlock.VerticalAlignment(VerticalAlignment::Center);
        CopyTextStyle(styleSource, textBlock);
        ApplySegmentAlignment(textBlock, i, segmentCount);
        Grid::SetColumn(textBlock, gridColumn);
        grid.Children().Append(textBlock);
        gridColumn += 2;
    }

    return grid;
}

static FrameworkElement BuildLineElement(winrt::hstring const& baseName,
                                         std::wstring const& line,
                                         TextBlock styleSource,
                                         StackPanel parent,
                                         int lineIndex) {
    auto segments = SplitOnSpacer(line);
    double width = EffectiveLineWidth(styleSource, parent);

    if (segments.size() > 1)
        return BuildSpacerGrid(baseName + L"_Line" + winrt::to_hstring(lineIndex),
                               segments, styleSource, width);

    TextBlock textBlock;
    textBlock.Name(baseName + L"_Line" + winrt::to_hstring(lineIndex));
    textBlock.Text(line);
    textBlock.VerticalAlignment(VerticalAlignment::Center);
    CopyTextStyle(styleSource, textBlock);
    ApplyRowWidthCap(textBlock, width);
    return textBlock;
}

// ============================================================
// In-place update
//
// The clock text changes every second. Rebuilding the generated subtree each
// tick thrashes layout and makes the inspected visual tree unstable, so when the
// shape is unchanged only the text is rewritten.
// ============================================================

static bool UpdateLineElementText(FrameworkElement lineElement,
                                  std::wstring const& line,
                                  double width) {
    if (!lineElement) return false;
    auto segments = SplitOnSpacer(line);
    // Reapplied on the fast path: a TCC Max width change alters the effective
    // width without changing this mod's settings (the layout key).
    ApplyRowWidthCap(lineElement, width);

    if (segments.size() > 1) {
        auto grid = lineElement.try_as<Grid>();
        if (!grid) return false;
        if (grid.Children().Size() != (uint32_t)segments.size()) return false;
        for (uint32_t i = 0; i < (uint32_t)segments.size(); i++) {
            auto textBlock = grid.Children().GetAt(i).try_as<TextBlock>();
            if (!textBlock) return false;
            textBlock.Text(segments[i]);
        }
        return true;
    }

    auto textBlock = lineElement.try_as<TextBlock>();
    if (!textBlock) return false;
    textBlock.Text(line);
    return true;
}

static bool UpdateGeneratedPanelText(StackPanel generatedPanel,
                                     std::vector<std::wstring> const& lines,
                                     double width) {
    if (!generatedPanel ||
        generatedPanel.Children().Size() != (uint32_t)lines.size())
        return false;

    ApplyPanelWidthConstraint(generatedPanel, width);

    for (uint32_t i = 0; i < (uint32_t)lines.size(); i++) {
        auto lineElement = generatedPanel.Children().GetAt(i).try_as<FrameworkElement>();
        if (!lineElement || !UpdateLineElementText(lineElement, lines[i], width))
            return false;
    }
    return true;
}

// ============================================================
// Source text block visibility
// ============================================================

// Zero both axes: Taskbar Clock Customization re-sets Visibility on its own
// schedule, and a nonzero-width collapsed block would still widen the shared
// StackPanel past the generated rows.
static void CollapseSourceTextBlock(TextBlock original) {
    if (!original) return;
    original.Height(0.0);
    original.MinHeight(0.0);
    original.MaxHeight(0.0);
    original.Width(0.0);
    original.MinWidth(0.0);
    original.Visibility(Visibility::Collapsed);
}

static void RestoreSourceTextBlock(TextBlock original) {
    if (!original) return;
    original.ClearValue(FrameworkElement::HeightProperty());
    original.ClearValue(FrameworkElement::MinHeightProperty());
    original.ClearValue(FrameworkElement::MaxHeightProperty());
    original.ClearValue(FrameworkElement::WidthProperty());
    original.ClearValue(FrameworkElement::MinWidthProperty());
    original.Visibility(Visibility::Visible);
}

static void RemoveGeneratedPanel(SpacerState& state) {
    auto parent = state.parentRef.get();
    auto generated = state.generatedRef.get();
    if (parent && generated) {
        uint32_t index;
        if (parent.Children().IndexOf(generated, index))
            parent.Children().RemoveAt(index);
    }
    state.generatedRef = {};
    state.generatedLayoutKey = 0;
}

// ============================================================
// Per-line update
// ============================================================

static void UpdateSpacerLine(SpacerState& state) {
    auto original = state.originalRef.get();
    auto parent   = state.parentRef.get();
    if (!original || !parent) return;

    // The parent StackPanel is deliberately never resized here. Taskbar Clock
    // Customization owns its MaxWidth (clearing it erased the user's fixed
    // clock width), and the panel is auto-width, so it follows the generated
    // rows on its own once the source block collapses to zero size.

    winrt::hstring textHString = original.Text();
    std::wstring fullText{textHString.c_str(), textHString.size()};

    if (!HasSpacerToken(fullText)) {
        RemoveGeneratedPanel(state);
        RestoreSourceTextBlock(original);
        return;
    }

    double width = EffectiveLineWidth(original, parent);
    WarnIfNoElasticRoom(width > 1.0);
    auto lines = SplitLines(fullText);
    uint64_t layoutKey = CurrentLayoutKey();

    // Fast path: same shape, same settings — rewrite text only.
    if (auto generated = state.generatedRef.get();
        generated && state.generatedLayoutKey == layoutKey &&
        UpdateGeneratedPanelText(generated, lines, width)) {
        CollapseSourceTextBlock(original);
        return;
    }

    RemoveGeneratedPanel(state);

    StackPanel generated;
    generated.Name(original.Name() + L"_SpacerPanel");
    generated.Orientation(Orientation::Vertical);
    generated.HorizontalAlignment(HorizontalAlignment::Stretch);
    generated.VerticalAlignment(VerticalAlignment::Center);
    ApplyPanelWidthConstraint(generated, width);

    for (int i = 0; i < (int)lines.size(); i++)
        generated.Children().Append(
            BuildLineElement(original.Name(), lines[i], original, parent, i));

    uint32_t originalIndex = 0;
    if (parent.Children().IndexOf(original, originalIndex))
        parent.Children().InsertAt(originalIndex, generated);
    else
        parent.Children().Append(generated);

    state.generatedRef = winrt::make_weak(generated);
    state.generatedLayoutKey = layoutKey;
    CollapseSourceTextBlock(original);
    generated.Visibility(Visibility::Visible);
}

// ============================================================
// Registration
// ============================================================

static void SetupSpacerForTextBlock(StackPanel parent, TextBlock textBlock) {
    if (!parent || !textBlock) return;

    for (auto& state : g_states)
        if (state.originalRef.get() == textBlock) return;

    SpacerState state;
    state.originalRef = winrt::make_weak(textBlock);
    state.parentRef   = winrt::make_weak(parent);
    UpdateSpacerLine(state);

    g_states.push_back(std::move(state));

    g_states.back().textToken = textBlock.RegisterPropertyChangedCallback(
        TextBlock::TextProperty(),
        [](DependencyObject sender, DependencyProperty) {
            if (g_unloading) return;
            auto changed = sender.try_as<TextBlock>();
            if (!changed) return;
            for (auto& state : g_states) {
                if (state.originalRef.get() == changed) {
                    UpdateSpacerLine(state);
                    return;
                }
            }
        });

    Wh_Log(L"[Spacer] Registered '%s'", textBlock.Name().c_str());
}

static void ApplySpacerToDateTimeContent(FrameworkElement element) {
    PCWSTR blockNames[] = {kTimeBlock, kDateBlock};
    int found = 0;
    for (PCWSTR blockName : blockNames) {
        auto textBlockElement = FindChildRecursive(element, [blockName](FrameworkElement fe) {
            return fe.Name() == blockName;
        });
        if (!textBlockElement) { Wh_Log(L"[Spacer] '%s' not found", blockName); continue; }
        auto textBlock = textBlockElement.try_as<TextBlock>();
        if (!textBlock) continue;
        auto parentDep = VisualTreeHelper::GetParent(textBlock);
        if (!parentDep) continue;
        auto parent = parentDep.try_as<StackPanel>();
        if (!parent) { Wh_Log(L"[Spacer] parent of '%s' not a StackPanel", blockName); continue; }
        SetupSpacerForTextBlock(parent, textBlock);
        found++;
    }
    if (!found) Wh_Log(L"[Spacer] No text blocks found in DateTimeIconContent");
}

// ============================================================
// Initial scan (for elements rendered before mod load)
// ============================================================

static void ScanForSpacerTargets(FrameworkElement root) {
    if (!root) return;
    // ContainerGrid appears throughout the system tray, so the class name is the
    // only reliable way to identify DateTimeIconContent specifically.
    try {
        if (winrt::get_class_name(root) == L"SystemTray.DateTimeIconContent") {
            ApplySpacerToDateTimeContent(root);
            return;
        }
    } catch (...) {}
    int n = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (child) ScanForSpacerTargets(child);
    }
}

// A WH_CALLWNDPROC hook sees every message sent to every window on the
// taskbar's UI thread, so the message must be compared BEFORE lParam is
// treated as the dispatch record. This mod already got that right; the
// template states the rule in capitals because reordering it once took
// Explorer down in OmniButton.
using RunFromWindowThreadProc_t = tbh::ThreadProc;

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc,
                                void* procParam) {
    return tbh::RunFromWindowThread(
        hWnd, proc, procParam,
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
}

static HWND FindCurrentProcessTaskbarWnd() {
    return tbh::FindCurrentProcessTaskbarWnd();
}

// ============================================================
// Hooks
// ============================================================

using DateTimeIconContent_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
DateTimeIconContent_OnApplyTemplate_t DateTimeIconContent_OnApplyTemplate_Original;

void WINAPI DateTimeIconContent_OnApplyTemplate_Hook(void* pThis) {
    DateTimeIconContent_OnApplyTemplate_Original(pThis);
    if (g_unloading) return;

    auto* iunk = *((IUnknown**)pThis + 1);
    if (!iunk) return;
    FrameworkElement element = nullptr;
    iunk->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element) return;

    try {
        ApplySpacerToDateTimeContent(element);
    } catch (...) {
        Wh_Log(L"[Spacer] Exception in OnApplyTemplate hook");
    }
}

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;
    HRSRC hResource = FindResourceW(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen) || !uPtrLen)
                    pFixedFileInfo = nullptr;
            }
        }
    }
    if (puPtrLen) *puPtrLen = uPtrLen;
    return static_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

// Order matters: SystemTray.dll is the new home (Win11 Insider 26200+);
// older builds have the symbols in Taskbar.View.dll.
static HMODULE GetSystemTrayModuleHandle() {
    if (HMODULE h = GetModuleHandleW(L"SystemTray.dll")) return h;
    if (HMODULE h = GetModuleHandleW(L"Taskbar.View.dll")) {
        // Starting with Taskbar.View.dll 2604.x, the SystemTray types moved out
        // into SystemTray.dll — don't hook this version.
        VS_FIXEDFILEINFO* fi = GetModuleVersionInfo(h, nullptr);
        WORD moduleMajor = fi ? HIWORD(fi->dwFileVersionMS) : 0;
        if (!moduleMajor || moduleMajor >= 2604) return nullptr;
        return h;
    }
    if (HMODULE h = GetModuleHandleW(L"ExplorerExtensions.dll")) return h;
    return nullptr;
}

static bool HookSystemTraySymbols(HMODULE h) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayModuleHooks[] = {{
        {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::OnApplyTemplate(void))"},
        &DateTimeIconContent_OnApplyTemplate_Original,
        DateTimeIconContent_OnApplyTemplate_Hook,
        true,
    }};
    return WindhawkUtils::HookSymbols(h, systemTrayModuleHooks,
                                      ARRAYSIZE(systemTrayModuleHooks));
}

static void TryHookSystemTrayModule(PCWSTR reason) {
    if (g_systemTrayModuleHooked) return;
    HMODULE h = GetSystemTrayModuleHandle();
    if (!h) return;
    if (g_systemTrayModuleHooked.exchange(true)) return;
    Wh_Log(L"[Hooks] System tray module found (%s) — hooking symbols", reason);
    if (HookSystemTraySymbols(h))
        Wh_ApplyHookOperations();
    else
        Wh_Log(L"[Hooks] System tray symbol hooks failed");
}

// Preferred wait-for-module path: TrayUI::StartTaskbar runs once the taskbar is
// actually starting, by which point the system tray module is loaded. This
// replaces watching every DLL load in the process.

// Fallback only, used when the TrayUI::StartTaskbar symbol cannot be resolved.
using LoadLibraryExW_t = HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (hModule && lpLibFileName)
        HandleLoadedModuleIfSystemTray(hModule, lpLibFileName);
    return hModule;
}

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == hModule) {
        Wh_Log(L"[LoadLib] %s", lpLibFileName);
        TryHookSystemTrayModule(L"LoadLibraryExW");
    }
}

static bool g_trayUiStartTaskbarHooked = false;

// Explorer rebuilt the taskbar. Same work the mod's own StartTaskbar hook did,
// handed to the template as its rebuild callback.
static void OnTaskbarRebuilt() {
    if (g_unloading) return;
    TryHookSystemTrayModule(L"TrayUI::StartTaskbar");
    StartInitialScan();
}

static bool HookTaskbarDllSymbols() {
    // The template hooks StartTaskbar alongside the four XamlRoot symbols, so
    // a failure here is a failure of the whole set - the LoadLibraryExW
    // watcher remains the fallback exactly as before.
    g_trayUiStartTaskbarHooked = tbh::HookTaskbarSymbols(OnTaskbarRebuilt);
    return g_trayUiStartTaskbarHooked;
}

// ============================================================
// Initial scan
// ============================================================

static void WaitForThreadWithSentMessagePump(HANDLE thread) {
    DWORD result;
    do {
        result = MsgWaitForMultipleObjects(1, &thread, FALSE, INFINITE, QS_SENDMESSAGE);
        if (result == WAIT_OBJECT_0 + 1) {
            MSG msg;
            PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
        }
    } while (result == WAIT_OBJECT_0 + 1);
}

static void StartInitialScan() {
    if (g_unloading || g_scanThread || g_scanDone) return;

    g_scanStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_scanStopEvent) {
        Wh_Log(L"[Spacer] Failed to create scan stop event");
        return;
    }

    g_scanThread = CreateThread(nullptr, 0, [](void* param) -> DWORD {
        HANDLE stopEvent = static_cast<HANDLE>(param);
        for (int i = 0; i < 5 && !g_unloading && !g_scanDone; i++) {
            if (i > 0 && WaitForSingleObject(stopEvent, 2000) != WAIT_TIMEOUT)
                break;
            HWND hWnd = FindCurrentProcessTaskbarWnd();
            if (!hWnd) continue;
            RunFromWindowThread(hWnd, [](void* param) {
                HWND h = (HWND)param;
                auto xamlRoot = GetTaskbarXamlRoot(h);
                if (!xamlRoot) return;
                auto root = xamlRoot.Content().try_as<FrameworkElement>();
                if (!root) return;
                g_scanDone = true;
                ScanForSpacerTargets(root);
                Wh_Log(L"[Spacer] Scan done, states=%d", (int)g_states.size());
            }, hWnd);
            if (g_scanDone) break;
        }
        return 0;
    }, g_scanStopEvent, 0, nullptr);

    if (!g_scanThread) {
        CloseHandle(g_scanStopEvent);
        g_scanStopEvent = nullptr;
        Wh_Log(L"[Spacer] Failed to create scan thread");
    }
}

// ============================================================
// Uninit
// ============================================================

static void ClearSpacerStates() {
    for (auto& state : g_states) {
        if (auto textBlock = state.originalRef.get()) {
            if (state.textToken)
                textBlock.UnregisterPropertyChangedCallback(
                    TextBlock::TextProperty(), state.textToken);
            textBlock.ClearValue(FrameworkElement::MaxWidthProperty());
            RestoreSourceTextBlock(textBlock);
        }
        // The parent StackPanel is intentionally untouched: this mod no longer
        // sets anything on it, and clearing MaxWidth here would erase Taskbar
        // Clock Customization's fixed clock width.
        RemoveGeneratedPanel(state);
    }
    g_states.clear();
}

// ============================================================
// Windhawk lifecycle
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Clock Spacer v1.1");
    LoadSettings();

    // GetTaskbarXamlRoot depends on every one of these symbols, and the initial
    // scan depends on GetTaskbarXamlRoot. Continuing without them leaves the mod
    // unable to do its job, so fail loudly here instead of later.
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] taskbar.dll symbol hooks failed");
        return FALSE;
    }

    if (HMODULE hSystemTray = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(hSystemTray)) {
            Wh_Log(L"[Init] System tray symbol hooks failed");
            return FALSE;
        }
    } else if (!g_trayUiStartTaskbarHooked) {
        Wh_Log(L"[Init] System tray module not loaded and TrayUI::StartTaskbar "
               L"unavailable — falling back to LoadLibraryExW");
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW = kernelbase
            ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelbase, "LoadLibraryExW"))
            : nullptr;
        if (!pLoadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(pLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original)) {
            Wh_Log(L"[Init] LoadLibraryExW hook unavailable");
            return FALSE;
        }
    } else {
        Wh_Log(L"[Init] System tray module not loaded — waiting for "
               L"TrayUI::StartTaskbar");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    TryHookSystemTrayModule(L"Wh_ModAfterInit");
    Wh_Log(L"[AfterInit] hooked=%d startTaskbarHook=%d",
           (int)g_systemTrayModuleHooked.load(), (int)g_trayUiStartTaskbarHooked);

    // If the taskbar already exists, scan now. Otherwise TrayUI::StartTaskbar
    // (or the LoadLibraryExW fallback plus this call on a later reload) covers it.
    if (g_systemTrayModuleHooked)
        StartInitialScan();
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");

    if (g_scanStopEvent)
        SetEvent(g_scanStopEvent);
    if (g_scanThread) {
        WaitForThreadWithSentMessagePump(g_scanThread);
        CloseHandle(g_scanThread);
        g_scanThread = nullptr;
    }
    if (g_scanStopEvent) {
        CloseHandle(g_scanStopEvent);
        g_scanStopEvent = nullptr;
    }

    // ClearSpacerStates touches WinRT objects — must run on the UI thread.
    if (HWND hWnd = FindCurrentProcessTaskbarWnd())
        RunFromWindowThread(hWnd, [](void*) { ClearSpacerStates(); }, nullptr);
    else
        ClearSpacerStates();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_warnedNoElasticRoom.store(false);
    Wh_Log(L"[Settings] lineWidth=%d maxWidth=%d minSpacerWidth=%d",
           g_settings.lineWidth, g_settings.maxWidth, g_settings.minSpacerWidth);

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"[Settings] No taskbar window found");
        return;
    }

    RunFromWindowThread(hWnd, [](void*) {
        for (auto& state : g_states)
            UpdateSpacerLine(state);
    }, nullptr);
}
