// ==WindhawkMod==
// @id              mod-tool-taskbar-tree-dump
// @name            Windhawk-Mod-Lab Tool: Taskbar Tree Dump
// @description     For mod authors: writes the Windows 11 taskbar's XAML element tree to a text or JSON file - on load, whenever the taskbar moves, resizes or is rebuilt, and on demand. Read-only.
// @version         1.0
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windhawk-Mod-Lab Tool: Taskbar Tree Dump

A tool for **mod authors**. It writes the Windows 11 taskbar's XAML element
tree to a file you can read, search, diff, or hand to someone helping you —
one line (or one JSON object) per element, with the layout facts that matter
when a mod has to find or move something.

It changes nothing on the taskbar. It only reads.

## Why

A live inspector shows one element at a time. A dump shows the whole tree at
once, and two dumps can be diffed: bottom against side taskbar, before and
after a Windows update, with and without another mod. That is how the Mod Lab
found out that moving the taskbar between edges re-lays out the existing tree
instead of rebuilding it, and that Windows announces the edge in a visual state.

## When it writes a dump

- About two seconds after it loads (**Dump on load**).
- Whenever the taskbar settles after a change — moved to another edge, made
  thicker or thinner, or rebuilt by Explorer (**Dump on change**). It waits
  for the taskbar to stop moving, so a dump is never taken mid-animation.
- Whenever you change any of its settings. To take a dump on demand, type a
  word into **Label**; it is added to the file name, which makes the files easy
  to tell apart (`left`, `before-update`, `with-styler`).

Disable the mod when you are done; it polls the taskbar once a second while
it is enabled.

## What each element records

- Type (runtime class) and `#Name`
- Actual size `[W x H]` and position `@x,y` relative to the dumped root
- Explicit `W`/`H`, `minW`/`minH`, `maxW`/`maxH`, margin `m`, padding `pad`,
  and non-stretch alignment `ha`/`va`
- Panel facts: StackPanel orientation and spacing, Grid row and column
  definitions (`A` = Auto, `*` = Star), a child's Grid cell and span,
  WrapGrid / ItemsWrapGrid / VariableSizedWrapGrid item size and row limit,
  Canvas position
- `RenderTransform` (translate, rotate, scale, composite, group, matrix)
- `Visibility` collapsed and `Opacity` below 1
- Every visual state group and its current state — this is where Windows
  states things like the taskbar's edge (`DockingStates` on `RootGrid`).
  Groups the template left unnamed are listed as `(unnamed 1)`, `(unnamed 2)`
- Text content, only if **Include text content** is on (see Privacy)

The file header records the time, the reason for the dump, the Windows
build, the taskbar position setting, each taskbar window's size and DPI, and
whether the taskbar was rebuilt since the previous dump. Session-specific
values such as window handles and object addresses are left out, so two dumps
diff cleanly.

## Privacy

Text in the taskbar includes window titles on task buttons, the clock and
anything other mods display. **Include text content** is off by default, and
each text element then records only its character count. Turn it on when you
need the text and are not sharing the file.

## Settings

| Setting | Default | |
|---|---|---|
| Output folder | `%USERPROFILE%\Documents\Taskbar Tree Dumps` | Environment variables are expanded; missing folders are created |
| Format | Text | Text (indented, one line per element) or JSON (nested) |
| Subtree | *(whole taskbar)* | Name of one element to dump instead, e.g. `SystemTrayFrameGrid` |
| Include text content | Off | See Privacy |
| Label | *(empty)* | Added to the next file name; changing it writes a dump |
| Dump on load | On | |
| Dump on change | On | |
| Maximum depth | 80 | Levels below the root |

File names are `taskbar-<date>-<time>-<edge>-<reason>[-<label>].txt` (or
`.json`), where the edge comes from the Windows taskbar position setting.

## Limitations

- The primary taskbar's tree only. Secondary-monitor taskbars are listed with
  their window size, but their XAML is not reached.
- The taskbar's own tree only: Start, Quick Settings and the notification
  center live in other processes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- OutputFolder: "%USERPROFILE%\\Documents\\Taskbar Tree Dumps"
  $name: Output folder
  $description: >-
    Environment variables such as %USERPROFILE% are expanded, and missing
    folders are created.
- Format: text
  $name: Format
  $options:
  - text: Text (indented, one line per element)
  - json: JSON (nested objects)
- Subtree: ""
  $name: Subtree
  $description: >-
    Name of one element to dump instead of the whole taskbar, for example
    SystemTrayFrameGrid or ControlCenterButton. Empty dumps everything. If the
    name is not found, the whole taskbar is dumped and the header says so.
- IncludeText: false
  $name: Include text content
  $description: >-
    Off records only how many characters each text element holds. Taskbar
    text includes window titles and the clock, so leave this off for dumps
    you share.
- Label: ""
  $name: Label
  $description: >-
    Added to the next file name. Changing any setting writes a dump at once,
    so typing a label is the way to take one on demand.
- DumpOnLoad: true
  $name: Dump on load
- DumpOnChange: true
  $name: Dump on change
  $description: >-
    When the taskbar settles after moving to another edge, changing
    thickness, or being rebuilt by Explorer.
- MaxDepth: 80
  $name: Maximum depth
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cwctype>
#include <string>
#include <utility>
#include <vector>
#include <windhawk_utils.h>
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ==ModComponents==
// Self-contained building blocks this mod is built from. Each
// section below is one contract in its own namespace; the mod's own
// code begins after them.

// -- Taskbar window discovery -----------------------------------------------
// Find this process's Shell_TrayWnd, and validate a cached handle before
// preferring it.
namespace tree_dump_taskbar_window {

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

}  // namespace tree_dump_taskbar_window

// -- UI-thread dispatch -----------------------------------------------------
// Marshal a callback onto the taskbar's UI thread with a CALLWNDPROC hook
// and a private registered message, reporting whether it actually ran.
namespace tree_dump_dispatch {

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

}  // namespace tree_dump_dispatch

// -- Taskbar XamlRoot -------------------------------------------------------
// Hook the taskbar.dll symbols, reach the taskbar's XamlRoot, and call back
// when Explorer rebuilds the taskbar in place.
namespace dispatch = tree_dump_dispatch;
namespace tree_dump_taskbar_xaml {

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
        if (dispatch::g_logException)
            dispatch::g_logException(L"TrayUI::StartTaskbar hook");
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

}  // namespace tree_dump_taskbar_xaml

// ==/ModComponents==

namespace taskbar_window = tree_dump_taskbar_window;
namespace taskbar_xaml = tree_dump_taskbar_xaml;

// ---- Settings ---------------------------------------------------------------

enum class Format { Text, Json };

struct Settings {
    WCHAR outputFolder[MAX_PATH];
    Format format;
    WCHAR subtree[128];
    bool includeText;
    WCHAR label[64];
    bool dumpOnLoad;
    bool dumpOnChange;
    int maxDepth;
};
// Fixed buffers only, so nothing here needs an exit-time destructor. Written
// on Windhawk's thread, read by the worker and (during a dump) the taskbar's
// thread: the lock keeps a dump from seeing a half-written change.
static Settings g_settings{};
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

static void CopySetting(PCWSTR key, WCHAR* buffer, size_t size) {
    auto value = WindhawkUtils::StringSetting::make(key);
    wcsncpy_s(buffer, size, value.get(), _TRUNCATE);
}

static void LoadSettings() {
    Settings next{};
    WCHAR raw[MAX_PATH];
    CopySetting(L"OutputFolder", raw, ARRAYSIZE(raw));
    if (!raw[0])
        wcscpy_s(raw, L"%USERPROFILE%\\Documents\\Taskbar Tree Dumps");
    if (!ExpandEnvironmentStringsW(raw, next.outputFolder,
                                   ARRAYSIZE(next.outputFolder)))
        wcscpy_s(next.outputFolder, raw);
    size_t len = wcslen(next.outputFolder);
    while (len && (next.outputFolder[len - 1] == L'\\' ||
                   next.outputFolder[len - 1] == L'/'))
        next.outputFolder[--len] = 0;

    auto format = WindhawkUtils::StringSetting::make(L"Format");
    next.format =
        _wcsicmp(format.get(), L"json") == 0 ? Format::Json : Format::Text;

    CopySetting(L"Subtree", next.subtree, ARRAYSIZE(next.subtree));
    next.includeText = Wh_GetIntSetting(L"IncludeText") != 0;

    // File-name safe: letters, digits, '-' and '_'; spaces become '_'.
    WCHAR label[64];
    CopySetting(L"Label", label, ARRAYSIZE(label));
    size_t out = 0;
    for (PCWSTR p = label; *p && out + 1 < ARRAYSIZE(next.label); ++p) {
        if (iswalnum(*p) || *p == L'-' || *p == L'_')
            next.label[out++] = *p;
        else if (*p == L' ')
            next.label[out++] = L'_';
    }
    next.label[out] = 0;

    next.dumpOnLoad = Wh_GetIntSetting(L"DumpOnLoad") != 0;
    next.dumpOnChange = Wh_GetIntSetting(L"DumpOnChange") != 0;
    next.maxDepth = std::clamp(Wh_GetIntSetting(L"MaxDepth"), 1, 200);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = next;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ---- State ------------------------------------------------------------------

static std::atomic<bool> g_unloading{false};
static std::atomic<int> g_rebuildCount{0};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_dumpNowEvent = nullptr;

static constexpr int kMaxElements = 60000;

// ---- Formatting helpers -----------------------------------------------------

static void Appendf(std::wstring& out, PCWSTR format, ...) {
    WCHAR buffer[1024];
    va_list args;
    va_start(args, format);
    int n = _vsnwprintf_s(buffer, ARRAYSIZE(buffer), _TRUNCATE, format, args);
    va_end(args);
    if (n < 0) n = (int)wcslen(buffer);
    out.append(buffer, n);
}

static std::wstring Fmt(PCWSTR format, ...) {
    WCHAR buffer[512];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(buffer, ARRAYSIZE(buffer), _TRUNCATE, format, args);
    va_end(args);
    return buffer;
}

static std::wstring JsonString(std::wstring const& text) {
    std::wstring out = L"\"";
    for (wchar_t c : text) {
        switch (c) {
            case L'"': out += L"\\\""; break;
            case L'\\': out += L"\\\\"; break;
            case L'\n': out += L"\\n"; break;
            case L'\r': out += L"\\r"; break;
            case L'\t': out += L"\\t"; break;
            default:
                if (c < 0x20)
                    out += Fmt(L"\\u%04x", (unsigned)c);
                else
                    out += c;
        }
    }
    return out + L"\"";
}

static PCWSTR OrientationText(Orientation o) {
    return o == Orientation::Vertical ? L"V" : L"H";
}

static std::wstring GridLengthText(GridLength const& g) {
    switch (g.GridUnitType) {
        case GridUnitType::Auto: return L"A";
        case GridUnitType::Star:
            return g.Value == 1.0 ? L"*" : Fmt(L"%g*", g.Value);
        default: return Fmt(L"%g", g.Value);
    }
}

static std::wstring ThicknessText(Thickness const& t) {
    return Fmt(L"%g,%g,%g,%g", t.Left, t.Top, t.Right, t.Bottom);
}

static bool NonZero(Thickness const& t) {
    return t.Left || t.Top || t.Right || t.Bottom;
}

// Item sizes are NaN when the panel sizes items from the first one; %g would
// print that as "-1.#IND".
static std::wstring ItemSizeText(double value) {
    return std::isnan(value) ? L"auto" : Fmt(L"%g", value);
}

// ---- One element ------------------------------------------------------------

using Pairs = std::vector<std::pair<std::wstring, std::wstring>>;

// Everything recorded for one element, kept separate from how it is written so
// the text and JSON outputs cannot disagree.
struct Item {
    int depth = 0;
    std::wstring type;
    std::wstring name;
    double width = 0, height = 0;
    bool positioned = false;
    double x = 0, y = 0;
    Pairs props;
    Pairs states;
};

static std::wstring TransformText(Transform const& t) {
    if (!t) return {};
    if (auto tt = t.try_as<TranslateTransform>())
        return tt.X() || tt.Y() ? Fmt(L"translate(%g,%g)", tt.X(), tt.Y())
                                : std::wstring{};
    if (auto rt = t.try_as<RotateTransform>())
        return Fmt(L"rotate(%g @%g,%g)", rt.Angle(), rt.CenterX(), rt.CenterY());
    if (auto st = t.try_as<ScaleTransform>())
        return st.ScaleX() != 1 || st.ScaleY() != 1
                   ? Fmt(L"scale(%g,%g)", st.ScaleX(), st.ScaleY())
                   : std::wstring{};
    if (auto ct = t.try_as<CompositeTransform>())
        return Fmt(L"composite(t=%g,%g r=%g s=%g,%g)", ct.TranslateX(),
                   ct.TranslateY(), ct.Rotation(), ct.ScaleX(), ct.ScaleY());
    if (auto gt = t.try_as<TransformGroup>()) {
        std::wstring out = L"group[";
        for (auto child : gt.Children()) {
            auto inner = TransformText(child);
            if (!inner.empty()) out += inner + L" ";
        }
        return out + L"]";
    }
    if (auto mt = t.try_as<MatrixTransform>()) {
        auto m = mt.Matrix();
        if (m.M11 != 1 || m.M12 || m.M21 || m.M22 != 1 || m.OffsetX || m.OffsetY)
            return Fmt(L"matrix(%g,%g,%g,%g,%g,%g)", m.M11, m.M12, m.M21,
                       m.M22, m.OffsetX, m.OffsetY);
        return {};
    }
    return winrt::get_class_name(t).c_str();
}

static void AddPanelFacts(FrameworkElement const& e, Pairs& props) {
    if (auto sp = e.try_as<StackPanel>()) {
        props.emplace_back(L"stack", OrientationText(sp.Orientation()));
        if (sp.Spacing()) props.emplace_back(L"spacing", Fmt(L"%g", sp.Spacing()));
        if (NonZero(sp.Padding()))
            props.emplace_back(L"pad", ThicknessText(sp.Padding()));
    } else if (auto g = e.try_as<Grid>()) {
        std::wstring rows, cols;
        for (auto r : g.RowDefinitions())
            rows += (rows.empty() ? L"" : L" ") + GridLengthText(r.Height());
        for (auto c : g.ColumnDefinitions())
            cols += (cols.empty() ? L"" : L" ") + GridLengthText(c.Width());
        if (!rows.empty()) props.emplace_back(L"rows", rows);
        if (!cols.empty()) props.emplace_back(L"cols", cols);
        if (NonZero(g.Padding()))
            props.emplace_back(L"pad", ThicknessText(g.Padding()));
    } else if (auto wg = e.try_as<WrapGrid>()) {
        props.emplace_back(L"wrapgrid", OrientationText(wg.Orientation()));
        props.emplace_back(L"itemW", ItemSizeText(wg.ItemWidth()));
        props.emplace_back(L"itemH", ItemSizeText(wg.ItemHeight()));
        props.emplace_back(L"maxRC", Fmt(L"%d", wg.MaximumRowsOrColumns()));
    } else if (auto iwg = e.try_as<ItemsWrapGrid>()) {
        props.emplace_back(L"itemswrapgrid", OrientationText(iwg.Orientation()));
        props.emplace_back(L"itemW", ItemSizeText(iwg.ItemWidth()));
        props.emplace_back(L"itemH", ItemSizeText(iwg.ItemHeight()));
        props.emplace_back(L"maxRC", Fmt(L"%d", iwg.MaximumRowsOrColumns()));
    } else if (auto vsw = e.try_as<VariableSizedWrapGrid>()) {
        props.emplace_back(L"vswrapgrid", OrientationText(vsw.Orientation()));
        props.emplace_back(L"itemW", ItemSizeText(vsw.ItemWidth()));
        props.emplace_back(L"itemH", ItemSizeText(vsw.ItemHeight()));
        props.emplace_back(L"maxRC", Fmt(L"%d", vsw.MaximumRowsOrColumns()));
    } else if (auto isp = e.try_as<ItemsStackPanel>()) {
        props.emplace_back(L"itemsstack", OrientationText(isp.Orientation()));
    }
}

static Item Describe(FrameworkElement const& e, FrameworkElement const& root,
                     int depth) {
    Item item;
    item.depth = depth;
    item.type = winrt::get_class_name(e).c_str();
    item.name = e.Name().c_str();
    item.width = e.ActualWidth();
    item.height = e.ActualHeight();
    try {
        auto point = e.TransformToVisual(root).TransformPoint({0, 0});
        item.x = point.X;
        item.y = point.Y;
        item.positioned = true;
    } catch (...) {
    }

    auto& p = item.props;
    if (e.Visibility() == Visibility::Collapsed) p.emplace_back(L"collapsed", L"1");
    if (e.Opacity() < 1.0) p.emplace_back(L"op", Fmt(L"%g", e.Opacity()));
    if (!std::isnan(e.Width())) p.emplace_back(L"W", Fmt(L"%g", e.Width()));
    if (!std::isnan(e.Height())) p.emplace_back(L"H", Fmt(L"%g", e.Height()));
    if (e.MinWidth() > 0) p.emplace_back(L"minW", Fmt(L"%g", e.MinWidth()));
    if (e.MinHeight() > 0) p.emplace_back(L"minH", Fmt(L"%g", e.MinHeight()));
    if (std::isfinite(e.MaxWidth())) p.emplace_back(L"maxW", Fmt(L"%g", e.MaxWidth()));
    if (std::isfinite(e.MaxHeight())) p.emplace_back(L"maxH", Fmt(L"%g", e.MaxHeight()));
    if (NonZero(e.Margin())) p.emplace_back(L"m", ThicknessText(e.Margin()));

    static PCWSTR const kHa[] = {L"L", L"C", L"R"};
    static PCWSTR const kVa[] = {L"T", L"C", L"B"};
    int ha = (int)e.HorizontalAlignment();
    int va = (int)e.VerticalAlignment();
    if (ha >= 0 && ha < 3) p.emplace_back(L"ha", kHa[ha]);
    if (va >= 0 && va < 3) p.emplace_back(L"va", kVa[va]);

    if (auto c = e.try_as<Control>(); c && NonZero(c.Padding()))
        p.emplace_back(L"pad", ThicknessText(c.Padding()));
    if (auto b = e.try_as<Border>(); b && NonZero(b.Padding()))
        p.emplace_back(L"pad", ThicknessText(b.Padding()));
    AddPanelFacts(e, p);

    if (auto parent = VisualTreeHelper::GetParent(e)) {
        if (parent.try_as<Grid>()) {
            int r = Grid::GetRow(e), c = Grid::GetColumn(e);
            int rs = Grid::GetRowSpan(e), cs = Grid::GetColumnSpan(e);
            if (r || c || rs != 1 || cs != 1)
                p.emplace_back(L"cell", Fmt(L"r%d,c%d span %d,%d", r, c, rs, cs));
        } else if (parent.try_as<Canvas>()) {
            p.emplace_back(L"canvas",
                           Fmt(L"%g,%g", Canvas::GetLeft(e), Canvas::GetTop(e)));
        }
    }

    auto transform = TransformText(e.RenderTransform());
    if (!transform.empty()) p.emplace_back(L"rt", transform);

    if (auto tb = e.try_as<TextBlock>()) {
        auto text = tb.Text();
        if (g_settings.includeText) {
            std::wstring value(text.c_str(), text.size());
            for (auto& ch : value)
                if (ch == L'\r' || ch == L'\n') ch = L'|';
            if (value.size() > 120) value = value.substr(0, 120) + L"...";
            p.emplace_back(L"text", value);
        } else {
            p.emplace_back(L"textChars", Fmt(L"%u", (unsigned)text.size()));
        }
        p.emplace_back(L"fs", Fmt(L"%g", tb.FontSize()));
    }

    // Templates can leave a group unnamed, and two of them would share the key
    // "" - which a JSON reader collapses to one. Number them instead.
    int unnamedGroups = 0;
    for (auto const& group : VisualStateManager::GetVisualStateGroups(e)) {
        auto current = group.CurrentState();
        std::wstring name = group.Name().c_str();
        if (name.empty()) name = Fmt(L"(unnamed %d)", ++unnamedGroups);
        item.states.emplace_back(name,
                                 current ? current.Name().c_str() : L"-");
    }
    return item;
}

static FrameworkElement FindByName(FrameworkElement const& root,
                                   std::wstring const& name, int depth = 0) {
    if (!root || depth > 200) return nullptr;
    if (root.Name() == name) return root;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child =
            VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (auto found = FindByName(child, name, depth + 1)) return found;
    }
    return nullptr;
}

static void Collect(FrameworkElement const& element,
                    FrameworkElement const& root, int depth,
                    std::vector<Item>& items) {
    if ((int)items.size() >= kMaxElements) return;
    try {
        items.push_back(Describe(element, root, depth));
    } catch (...) {
        Item failed;
        failed.depth = depth;
        failed.type = L"(element could not be read)";
        items.push_back(failed);
        return;
    }
    if (depth >= g_settings.maxDepth) return;
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        auto child =
            VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (child) Collect(child, root, depth + 1, items);
    }
}

// ---- Rendering --------------------------------------------------------------

static std::wstring ItemLine(Item const& item) {
    std::wstring line(item.depth * 2, L' ');
    line += item.type;
    if (!item.name.empty()) line += L" #" + item.name;
    Appendf(line, L"  [%gx%g]", item.width, item.height);
    if (item.positioned) Appendf(line, L" @%g,%g", item.x, item.y);
    for (auto const& [key, value] : item.props) {
        if (key == L"text")
            line += L" text=\"" + value + L"\"";
        else if (key == L"collapsed")
            line += L" COLLAPSED";
        else
            line += L" " + key + L"=" + value;
    }
    if (!item.states.empty()) {
        line += L" states{";
        for (size_t i = 0; i < item.states.size(); ++i)
            line += (i ? L", " : L"") + item.states[i].first + L"=" +
                    item.states[i].second;
        line += L"}";
    }
    return line + L"\n";
}

static std::wstring ItemJson(Item const& item) {
    std::wstring out = L"{\"type\":" + JsonString(item.type);
    if (!item.name.empty()) out += L",\"name\":" + JsonString(item.name);
    out += Fmt(L",\"size\":[%g,%g]", item.width, item.height);
    if (item.positioned) out += Fmt(L",\"pos\":[%g,%g]", item.x, item.y);
    if (!item.props.empty()) {
        out += L",\"props\":{";
        for (size_t i = 0; i < item.props.size(); ++i)
            out += (i ? L"," : L"") + JsonString(item.props[i].first) + L":" +
                   JsonString(item.props[i].second);
        out += L"}";
    }
    if (!item.states.empty()) {
        out += L",\"states\":{";
        for (size_t i = 0; i < item.states.size(); ++i)
            out += (i ? L"," : L"") + JsonString(item.states[i].first) + L":" +
                   JsonString(item.states[i].second);
        out += L"}";
    }
    return out;  // left open for "children"
}

// Depth-first items with depths -> nested JSON objects.
static std::wstring TreeJson(std::vector<Item> const& items) {
    std::wstring out;
    std::vector<int> open;  // depths of objects whose children array is open
    for (size_t i = 0; i < items.size(); ++i) {
        int depth = items[i].depth;
        while (!open.empty() && open.back() >= depth) {
            out += L"]}";
            open.pop_back();
        }
        if (i && !open.empty() && out.back() != L'[') out += L",";
        out += ItemJson(items[i]) + L",\"children\":[";
        open.push_back(depth);
    }
    while (!open.empty()) {
        out += L"]}";
        open.pop_back();
    }
    return out.empty() ? L"null" : out;
}

// ---- Taskbar windows --------------------------------------------------------

static std::vector<HWND> FindTaskbars() {
    std::vector<HWND> result;
    if (HWND primary = taskbar_window::FindCurrentProcessTaskbarWnd())
        result.push_back(primary);
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            DWORD processId = 0;
            WCHAR className[64];
            if (GetWindowThreadProcessId(window, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassNameW(window, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)
                reinterpret_cast<std::vector<HWND>*>(parameter)->push_back(window);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

static PCWSTR EdgeByRect(RECT const& r, RECT const& mon) {
    int w = r.right - r.left, h = r.bottom - r.top;
    if (w >= h)
        return std::abs(r.top - mon.top) <= std::abs(mon.bottom - r.bottom)
                   ? L"top"
                   : L"bottom";
    return std::abs(r.left - mon.left) <= std::abs(mon.right - r.right)
               ? L"left"
               : L"right";
}

static int ReadTaskbarLocation() {
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"TaskbarLocation", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS)
        return -1;
    return (int)value;
}

static PCWSTR LocationName(int location) {
    static PCWSTR const kAbe[] = {L"left", L"top", L"right", L"bottom"};
    return location >= 0 && location < 4 ? kAbe[location] : L"unset";
}

static std::wstring WindowsBuild() {
    WCHAR build[32]{};
    DWORD size = sizeof(build);
    RegGetValueW(HKEY_LOCAL_MACHINE,
                 L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                 L"CurrentBuild", RRF_RT_REG_SZ, nullptr, build, &size);
    DWORD ubr = 0;
    size = sizeof(ubr);
    RegGetValueW(HKEY_LOCAL_MACHINE,
                 L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"UBR",
                 RRF_RT_REG_DWORD, nullptr, &ubr, &size);
    return Fmt(L"%s.%lu", build[0] ? build : L"?", ubr);
}

// ---- UI-thread work ---------------------------------------------------------

static PCWSTR const kDispatchMessage =
    L"Windhawk_RunFromWindowThread_" WH_MOD_ID;

struct ProbeContext {
    HWND hwnd = nullptr;
    void* rootAbi = nullptr;
    double width = 0, height = 0;
};

// Cheap: the root's identity and size, for change detection only.
static void ProbeOnUiThread(void* parameter) {
    auto* ctx = static_cast<ProbeContext*>(parameter);
    auto xamlRoot = taskbar_xaml::GetTaskbarXamlRoot(ctx->hwnd);
    if (!xamlRoot) return;
    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return;
    ctx->rootAbi = winrt::get_abi(root);
    ctx->width = root.ActualWidth();
    ctx->height = root.ActualHeight();
}

struct DumpContext {
    HWND hwnd = nullptr;
    std::vector<Item> items;
    void* rootAbi = nullptr;
    bool found = false;
    bool subtreeMissing = false;
};

static void DumpOnUiThread(void* parameter) {
    auto* ctx = static_cast<DumpContext*>(parameter);
    auto xamlRoot = taskbar_xaml::GetTaskbarXamlRoot(ctx->hwnd);
    if (!xamlRoot) return;
    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return;
    ctx->found = true;
    ctx->rootAbi = winrt::get_abi(root);
    FrameworkElement start = root;
    if (g_settings.subtree[0]) {
        start = FindByName(root, g_settings.subtree);
        if (!start) {
            ctx->subtreeMissing = true;
            start = root;
        }
    }
    Collect(start, start, 0, ctx->items);
}

// ---- Change detection and output --------------------------------------------

struct KnownRoot {
    HWND hwnd;
    void* rootAbi;
};

static std::wstring ComputeSignature() {
    std::wstring sig;
    Appendf(sig, L"loc=%d rebuilds=%d;", ReadTaskbarLocation(),
            g_rebuildCount.load());
    for (HWND hwnd : FindTaskbars()) {
        RECT r{};
        GetWindowRect(hwnd, &r);
        ProbeContext probe;
        probe.hwnd = hwnd;
        tree_dump_dispatch::RunFromWindowThread(hwnd, ProbeOnUiThread, &probe,
                                                kDispatchMessage);
        Appendf(sig, L"%p:%ld,%ld,%ld,%ld:%p:%.0fx%.0f;", hwnd, r.left, r.top,
                r.right, r.bottom, probe.rootAbi, probe.width, probe.height);
    }
    return sig;
}

static bool IsFolder(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

// Creates every missing folder along the path. Prefixes that cannot be
// created (a drive root, a UNC server or share) are skipped; only the final
// folder has to exist.
static bool EnsureFolder(std::wstring const& path) {
    for (size_t i = 0; i <= path.size(); ++i) {
        if (i < path.size() && path[i] != L'\\' && path[i] != L'/') continue;
        std::wstring prefix = path.substr(0, i);
        if (!prefix.empty() && !IsFolder(prefix))
            CreateDirectoryW(prefix.c_str(), nullptr);
    }
    return IsFolder(path);
}

static bool WriteUtf8File(std::wstring const& path, std::wstring const& text) {
    int bytes = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(),
                                    nullptr, 0, nullptr, nullptr);
    std::string utf8(bytes, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), utf8.data(),
                        bytes, nullptr, nullptr);
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool ok = WriteFile(file, utf8.data(), (DWORD)utf8.size(), &written,
                        nullptr) &&
              written == utf8.size();
    CloseHandle(file);
    return ok;
}

struct TaskbarDump {
    std::wstring windowClass;
    RECT window{};
    RECT monitor{};
    UINT dpi = 96;
    PCWSTR edge = L"";
    PCWSTR rootStatus = L"";
    bool reached = false;
    bool subtreeMissing = false;
    std::vector<Item> items;
};

static PCWSTR const kLegend =
    L"Legend: [actual WxH] @x,y relative to the dumped root; W/H, minW/minH, "
    L"maxW/maxH = explicit values; m = margin, pad = padding; ha/va = "
    L"non-stretch alignment; stack = StackPanel H|V; rows/cols = Grid "
    L"definitions (A = Auto, * = Star); cell = Grid row,col; rt = "
    L"RenderTransform; textChars = characters of text (content hidden); "
    L"states{group=current}";

static std::wstring RenderText(PCWSTR reason, SYSTEMTIME const& st,
                               std::wstring const& build, int location,
                               std::vector<TaskbarDump> const& dumps) {
    std::wstring text;
    Appendf(text, L"Windhawk-Mod-Lab Tool: Taskbar Tree Dump v%s\n", WH_MOD_VERSION);
    Appendf(text, L"Time: %04d-%02d-%02d %02d:%02d:%02d\n", st.wYear, st.wMonth,
            st.wDay, st.wHour, st.wMinute, st.wSecond);
    Appendf(text, L"Reason: %s\n", reason);
    Appendf(text, L"Label: %s\n", g_settings.label[0] ? g_settings.label : L"-");
    Appendf(text, L"Windows build: %s\n", build.c_str());
    Appendf(text, L"Taskbar position setting (TaskbarLocation): %s\n",
            LocationName(location));
    Appendf(text, L"Taskbar rebuilds seen since load: %d\n",
            g_rebuildCount.load());
    Appendf(text, L"Subtree: %s\n",
            g_settings.subtree[0] ? g_settings.subtree : L"(whole taskbar)");
    Appendf(text, L"Text content: %s\n\n",
            g_settings.includeText ? L"included" : L"hidden (character counts only)");

    for (size_t i = 0; i < dumps.size(); ++i) {
        auto const& d = dumps[i];
        Appendf(text, L"=== Taskbar %zu of %zu: %s\n", i + 1, dumps.size(),
                d.windowClass.c_str());
        Appendf(text, L"window: %ldx%ld at %ld,%ld; monitor %ldx%ld; dpi %u (%.0f%%)\n",
                d.window.right - d.window.left, d.window.bottom - d.window.top,
                d.window.left, d.window.top, d.monitor.right - d.monitor.left,
                d.monitor.bottom - d.monitor.top, d.dpi, d.dpi * 100.0 / 96);
        Appendf(text, L"edge (from window position): %s\n", d.edge);
        if (!d.reached) {
            text += L"XAML tree: not reached\n\n";
            continue;
        }
        Appendf(text, L"root element: %s\n", d.rootStatus);
        if (d.subtreeMissing)
            Appendf(text, L"subtree '%s' not found; whole taskbar dumped\n",
                    g_settings.subtree);
        Appendf(text, L"elements: %zu\n\n", d.items.size());
        text += kLegend;
        text += L"\n\n";
        for (auto const& item : d.items) text += ItemLine(item);
        text += L"\n";
    }
    return text;
}

static std::wstring RectJson(RECT const& r) {
    return Fmt(L"{\"x\":%ld,\"y\":%ld,\"width\":%ld,\"height\":%ld}", r.left,
               r.top, r.right - r.left, r.bottom - r.top);
}

static std::wstring RenderJson(PCWSTR reason, SYSTEMTIME const& st,
                               std::wstring const& build, int location,
                               std::vector<TaskbarDump> const& dumps) {
    std::wstring out = L"{";
    out += L"\"tool\":\"Windhawk-Mod-Lab Tool: Taskbar Tree Dump\"";
    out += L",\"version\":" + JsonString(WH_MOD_VERSION);
    out += Fmt(L",\"time\":\"%04d-%02d-%02dT%02d:%02d:%02d\"", st.wYear,
               st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    out += L",\"reason\":" + JsonString(reason);
    out += L",\"label\":" + JsonString(g_settings.label);
    out += L",\"windowsBuild\":" + JsonString(build);
    out += L",\"taskbarLocation\":" + JsonString(LocationName(location));
    out += Fmt(L",\"rebuildsSinceLoad\":%d", g_rebuildCount.load());
    out += L",\"subtree\":" + JsonString(g_settings.subtree);
    out += g_settings.includeText ? L",\"includeText\":true"
                                  : L",\"includeText\":false";
    out += L",\"taskbars\":[";
    for (size_t i = 0; i < dumps.size(); ++i) {
        auto const& d = dumps[i];
        if (i) out += L",";
        out += L"{\"class\":" + JsonString(d.windowClass);
        out += L",\"window\":" + RectJson(d.window);
        out += L",\"monitor\":" + RectJson(d.monitor);
        out += Fmt(L",\"dpi\":%u", d.dpi);
        out += L",\"edgeFromWindow\":" + JsonString(d.edge);
        out += d.reached ? L",\"reached\":true" : L",\"reached\":false";
        if (d.reached) {
            out += L",\"rootElement\":" + JsonString(d.rootStatus);
            out += d.subtreeMissing ? L",\"subtreeFound\":false"
                                    : L",\"subtreeFound\":true";
            out += Fmt(L",\"elements\":%zu", d.items.size());
            out += L",\"tree\":" + TreeJson(d.items);
        }
        out += L"}";
    }
    return out + L"]}\n";
}

static void Dump(PCWSTR reason, std::vector<KnownRoot>& known) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    int location = ReadTaskbarLocation();
    std::wstring build = WindowsBuild();

    std::vector<TaskbarDump> dumps;
    for (HWND hwnd : FindTaskbars()) {
        TaskbarDump d;
        WCHAR className[64]{};
        GetClassNameW(hwnd, className, ARRAYSIZE(className));
        d.windowClass = className;
        GetWindowRect(hwnd, &d.window);
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
        d.monitor = mi.rcMonitor;
        d.dpi = GetDpiForWindow(hwnd);
        d.edge = EdgeByRect(d.window, d.monitor);

        DumpContext ctx;
        ctx.hwnd = hwnd;
        bool ran = tree_dump_dispatch::RunFromWindowThread(
            hwnd, DumpOnUiThread, &ctx, kDispatchMessage);
        d.reached = ran && ctx.found;
        d.subtreeMissing = ctx.subtreeMissing;
        d.items = std::move(ctx.items);

        d.rootStatus = L"first dump since load";
        bool seen = false;
        for (auto& k : known) {
            if (k.hwnd != hwnd) continue;
            seen = true;
            d.rootStatus = k.rootAbi == ctx.rootAbi
                               ? L"same element as the previous dump"
                               : L"changed since the previous dump (rebuilt)";
            k.rootAbi = ctx.rootAbi;
        }
        if (!seen) known.push_back({hwnd, ctx.rootAbi});
        dumps.push_back(std::move(d));
    }

    bool json = g_settings.format == Format::Json;
    std::wstring body = json ? RenderJson(reason, st, build, location, dumps)
                             : RenderText(reason, st, build, location, dumps);

    std::wstring folder = g_settings.outputFolder;
    std::wstring path = Fmt(
        L"%s\\taskbar-%04d%02d%02d-%02d%02d%02d-%s-%s%s%s.%s", folder.c_str(),
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
        LocationName(location), reason, g_settings.label[0] ? L"-" : L"",
        g_settings.label, json ? L"json" : L"txt");
    if (!EnsureFolder(folder) || !WriteUtf8File(path, body)) {
        Wh_Log(L"Could not write %s (error %lu)", path.c_str(), GetLastError());
        return;
    }
    Wh_Log(L"Dump written (%s): %s", reason, path.c_str());
}

static void WorkerIteration(bool forced, std::vector<KnownRoot>& known,
                            std::wstring& lastSeen, std::wstring& pending,
                            int& stable, bool& first);

// Polls once a second. A change is dumped only after the signature has held
// steady for two polls, so a move is captured settled, not mid-animation.
static DWORD WINAPI WorkerThread(void*) {
    std::vector<KnownRoot> known;
    std::wstring lastSeen, pending;
    int stable = 0;
    bool first = true;
    HANDLE waits[] = {g_stopEvent, g_dumpNowEvent};
    while (!g_unloading) {
        DWORD wait = WaitForMultipleObjects(2, waits, FALSE, 1000);
        if (wait == WAIT_OBJECT_0 || g_unloading) break;
        // Shared for the whole iteration: a dump reads the settings on this
        // thread and, through the dispatch, on the taskbar's.
        AcquireSRWLockShared(&g_settingsLock);
        try {
            WorkerIteration(wait == WAIT_OBJECT_0 + 1, known, lastSeen, pending,
                            stable, first);
        } catch (...) {
            Wh_Log(L"Worker iteration failed");
        }
        ReleaseSRWLockShared(&g_settingsLock);
    }
    return 0;
}

static void WorkerIteration(bool forced, std::vector<KnownRoot>& known,
                            std::wstring& lastSeen, std::wstring& pending,
                            int& stable, bool& first) {
    {
            std::wstring sig = ComputeSignature();
            if (forced) {
                Dump(L"settings", known);
                lastSeen = sig;
                first = false;
                pending.clear();
                return;
            }
            if (sig == lastSeen) {
                pending.clear();
                return;
            }
            if (sig != pending) {
                pending = sig;
                stable = 1;
                return;
            }
            if (++stable < 2) return;
            bool wanted = first ? g_settings.dumpOnLoad : g_settings.dumpOnChange;
            if (wanted) Dump(first ? L"load" : L"change", known);
            first = false;
            lastSeen = sig;
            pending.clear();
    }
}

// ---- Windhawk lifecycle -----------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Windhawk-Mod-Lab Tool: Taskbar Tree Dump v%s", WH_MOD_VERSION);
    LoadSettings();
    tree_dump_dispatch::SetExceptionLogger(
        [](PCWSTR context) { Wh_Log(L"Exception in %s", context); });
    if (!taskbar_xaml::HookTaskbarSymbols([] { ++g_rebuildCount; })) {
        Wh_Log(L"taskbar.dll symbols unavailable");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_dumpNowEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stopEvent && g_dumpNowEvent)
        g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
}

void Wh_ModUninit() {
    g_unloading = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_thread) {
        // The worker marshals onto the taskbar's thread with SendMessage, so
        // pump sent messages while waiting rather than block that thread.
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(1, &g_thread, FALSE, INFINITE,
                                               QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG message;
                PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) CloseHandle(g_stopEvent);
    if (g_dumpNowEvent) CloseHandle(g_dumpNowEvent);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_dumpNowEvent) SetEvent(g_dumpNowEvent);
}
