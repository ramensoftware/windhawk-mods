// ==WindhawkMod==
// @id              taskbar-open-window-underline
// @name            Taskbar Open Window Underline
// @description     Underlines the labels of taskbar buttons whose windows are shown on the desktop, so open and minimized windows can be told apart at a glance
// @version         1.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0A00 -lole32 -loleaut32 -lruntimeobject -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Open Window Underline

Underlines the label of every Windows 11 taskbar button that has a window
currently shown on the desktop. Buttons whose windows are all minimized keep a
plain label, so you can tell at a glance which apps actually have something
open in front of you.

Requirements:

* Windows 11 with the XAML taskbar (22H2 or newer).
* Taskbar labels must be visible: Settings → Personalization → Taskbar →
  Taskbar behaviors → "Combine taskbar buttons and hide labels" set to
  **Never** (or **When taskbar is full**). The "Show labels, combine taskbar
  buttons" mode of the Taskbar Labels mod is supported as well; a combined
  group is underlined when any (or, optionally, every) one of its windows is
  shown.

How it works:

* The taskbar's buttons live in an `ItemsRepeater`; each button's item is its
  view model (`TaskListWindowViewModel` for one window, `TaskListGroupViewModel`
  for a group), and the view model knows its windows through the shell's
  `TaskItem` / `TaskGroup` objects. The mod resolves every button's windows
  that way and judges them with `IsIconic` (plus DWM cloaking for windows on
  other virtual desktops). Win+D counts as minimized.
* A WinEvent hook (minimize / restore / show / hide / destroy / cloak / name
  change / foreground) invalidates the buttons; events are coalesced and every
  button is re-evaluated on its own UI thread, on every taskbar.
* The underline is the native `TextBlock` underline, so it follows the text
  color, trimming and theme. It can cover the whole label or just its first
  letter. It is removed when the mod is disabled.
* On builds where the view-model symbols can't be resolved, the mod falls back
  to matching label text against window titles, which is exact for "Never
  combine" but blind to combined groups.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: open
  $name: What to underline
  $options:
  - open: Buttons with at least one window shown on the desktop
  - minimized: Buttons whose windows are all minimized (inverse)
- underlineStyle: full
  $name: Underline style
  $options:
  - full: The whole label
  - firstLetter: The first letter only
- includeActive: true
  $name: Count the foreground window as shown
  $description: >-
    When disabled, the active (foreground) window alone does not earn an
    underline, since its button already has the active indicator.
- groupRule: any
  $name: Buttons with several windows
  $options:
  - any: Shown if any of the windows is shown
  - all: Shown only if every window is shown
- cloakedIsHidden: true
  $name: Treat cloaked windows as not shown
  $description: >-
    Cloaked windows include windows on other virtual desktops and suspended
    Store apps.
- refreshDelayMs: 60
  $name: Refresh delay (ms)
  $description: >-
    Window-state events are coalesced for this long before the buttons are
    re-evaluated. Win+M and Win+D fire dozens of events at once.
- debugLog: false
  $name: Debug - log every decision
  $description: >-
    Writes one line per button on every refresh (label, windows, state,
    decision) to the mod log. Development aid only; noisy.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <inspectable.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

// The taskbar items live in a WinUI 2 ItemsRepeater built on system XAML;
// its projection is needed to ask the repeater which item a button shows.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace Xaml = winrt::Windows::UI::Xaml;
using winrt::Windows::System::DispatcherQueue;
using winrt::Windows::System::DispatcherQueuePriority;
using winrt::Windows::UI::Text::TextDecorations;

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

// Posted to the event thread by the UI-thread hooks to request a deferred,
// coalesced re-check (the hook often fires before WS_MINIMIZE is set).
#define WM_APP_REQUEST_REFRESH (WM_APP + 1)

// Delay for the second pass that follows every coalesced refresh, to catch
// window-state transitions that were still in flight during the first pass.
#define FOLLOW_UP_DELAY_MS 300

// ============================================================================
// Settings
// ============================================================================

enum class Mode { kOpen, kMinimized };
enum class GroupRule { kAny, kAll };
enum class UnderlineStyle { kFull, kFirstLetter };

struct {
    Mode mode;
    UnderlineStyle underlineStyle;
    bool includeActive;
    GroupRule groupRule;
    bool cloakedIsHidden;
    int refreshDelayMs;
    bool debugLog;
} g_settings;

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode =
        wcscmp(mode, L"minimized") == 0 ? Mode::kMinimized : Mode::kOpen;
    Wh_FreeStringSetting(mode);

    PCWSTR underlineStyle = Wh_GetStringSetting(L"underlineStyle");
    g_settings.underlineStyle = wcscmp(underlineStyle, L"firstLetter") == 0
                                    ? UnderlineStyle::kFirstLetter
                                    : UnderlineStyle::kFull;
    Wh_FreeStringSetting(underlineStyle);

    g_settings.includeActive = Wh_GetIntSetting(L"includeActive") != 0;

    PCWSTR groupRule = Wh_GetStringSetting(L"groupRule");
    g_settings.groupRule =
        wcscmp(groupRule, L"all") == 0 ? GroupRule::kAll : GroupRule::kAny;
    Wh_FreeStringSetting(groupRule);

    g_settings.cloakedIsHidden = Wh_GetIntSetting(L"cloakedIsHidden") != 0;

    int delay = Wh_GetIntSetting(L"refreshDelayMs");
    if (delay < 10) {
        delay = 10;
    } else if (delay > 2000) {
        delay = 2000;
    }
    g_settings.refreshDelayMs = delay;

    g_settings.debugLog = Wh_GetIntSetting(L"debugLog") != 0;
}

// ============================================================================
// Global state
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

// Guards g_roots and g_labels. Never call into XAML while holding it.
std::mutex g_mutex;

// One entry per XAML tree we have seen a TaskListButton in (one tree per
// taskbar, i.e. per monitor). Every refresh pass enumerates the buttons from
// the root, so recycled or newly created buttons are always found.
struct RootEntry {
    void* key;
    winrt::weak_ref<Xaml::DependencyObject> root;
    DispatcherQueue queue;
};
std::vector<RootEntry> g_roots;

// Every LabelControl TextBlock we have touched, with what it looked like
// before we did, so Wh_ModBeforeUninit can put it back exactly.
struct LabelEntry {
    winrt::weak_ref<Xaml::Controls::TextBlock> label;
    DispatcherQueue queue;
    TextDecorations original;
    int64_t textChangedToken;
};
std::unordered_map<void*, LabelEntry> g_labels;

// ============================================================================
// Window-state source
// ============================================================================

struct WindowSnapshot {
    // Title -> top-level windows carrying that title, taskbar-eligible only.
    std::unordered_map<std::wstring, std::vector<HWND>> byTitle;
    // The same for every visible top-level window, used when the strict
    // map has no match (elevated windows such as Task Manager, say).
    std::unordered_map<std::wstring, std::vector<HWND>> byTitleAll;
};

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked)))) {
        return cloaked != 0;
    }
    return false;
}

// Approximates the shell's own rules for which top-level windows get a
// taskbar button.
bool IsTaskbarCandidate(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_APPWINDOW) {
        return true;
    }

    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER)) {
        return false;
    }

    return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* snapshot = reinterpret_cast<WindowSnapshot*>(lParam);

    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    // GetWindowText doesn't message foreign windows, so this cannot hang on a
    // stuck app.
    int length = GetWindowTextLengthW(hwnd);
    if (length <= 0) {
        return TRUE;
    }

    std::wstring title(static_cast<size_t>(length) + 1, L'\0');
    length = GetWindowTextW(hwnd, title.data(), length + 1);
    if (length <= 0) {
        return TRUE;
    }

    title.resize(length);
    snapshot->byTitleAll[title].push_back(hwnd);
    if (IsTaskbarCandidate(hwnd)) {
        snapshot->byTitle[title].push_back(hwnd);
    }
    return TRUE;
}

WindowSnapshot BuildWindowSnapshot() {
    WindowSnapshot snapshot;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&snapshot));
    return snapshot;
}

bool IsWindowShown(HWND hwnd) {
    // IsIconic is the source of truth. Win+D minimizes windows for real, so
    // it is covered here without special handling.
    if (IsIconic(hwnd)) {
        return false;
    }

    if (g_settings.cloakedIsHidden && IsWindowCloaked(hwnd)) {
        return false;
    }

    return true;
}

// ============================================================================
// Decision logic
// ============================================================================

// Given the windows behind one button, decides whether its label should be
// underlined under the current settings.
bool ShouldUnderline(const std::vector<HWND>& windows) {
    if (windows.empty()) {
        return false;
    }

    HWND foreground = g_settings.includeActive ? nullptr : GetForegroundWindow();

    size_t shownCount = 0;
    bool foregroundSkipped = false;
    for (HWND hwnd : windows) {
        if (!IsWindowShown(hwnd)) {
            continue;
        }

        if (hwnd == foreground) {
            // Shown, but the user doesn't want it to count on its own.
            foregroundSkipped = true;
            continue;
        }

        shownCount++;
    }

    bool shown;
    if (g_settings.groupRule == GroupRule::kAll) {
        // For "all", the foreground window still counts as shown; it only
        // stops counting as the *sole* reason under the "any" rule.
        shown = shownCount + (foregroundSkipped ? 1 : 0) == windows.size();
    } else {
        shown = shownCount > 0;
    }

    return g_settings.mode == Mode::kOpen ? shown : !shown;
}

// Builds the title snapshot on first use only; the view-model bridge below
// doesn't need it at all.
struct LazyWindowSnapshot {
    const WindowSnapshot& get() {
        if (!snapshot) {
            snapshot.emplace(BuildWindowSnapshot());
        }
        return *snapshot;
    }

   private:
    std::optional<WindowSnapshot> snapshot;
};

// ============================================================================
// Button -> windows resolver, part 1: the taskbar's own view model
// ============================================================================
//
// Every TaskListButton carries a ViewModel: a TaskListWindowViewModel for one
// window, or a TaskListGroupViewModel for a combined group (always the case
// with Taskbar Labels' "Show labels, combine taskbar buttons" mode). Both are
// backed by WindowsUdk.UI.Shell objects that know their HWNDs. The symbols
// and interface layouts below are the ones the Taskbar Labels mod uses.

// WindowsUdk.UI.Shell interfaces, declared up to the last used method. A WinRT
// interface doesn't change once published, so a successful query by IID
// guarantees the layout.

// {b081d9d6-9b45-5363-8a4d-854add6abe7e}
constexpr winrt::guid IID_ITaskItem{
    0xb081d9d6,
    0x9b45,
    0x5363,
    {0x8a, 0x4d, 0x85, 0x4a, 0xdd, 0x6a, 0xbe, 0x7e}};

struct ITaskItem : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowId(HWND* value) = 0;
};

// {a152e779-93df-5be2-af2c-d342452b0ce0}
constexpr winrt::guid IID_ITaskGroup{
    0xa152e779,
    0x93df,
    0x5be2,
    {0xaf, 0x2c, 0xd3, 0x42, 0x45, 0x2b, 0x0c, 0xe0}};

struct ITaskGroup : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessibleName() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Icon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIcon() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIconDescription() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Badge() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRequestingAttention() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Progress() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_VisualState() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Bounds() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Items(void** value) = 0;
};

// Taskbar.View.dll symbols, all optional: without them the title fallback
// below is used.

void* TaskListWindowViewModel_ITaskListWindowViewModel_vftable;
void* TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable;
void* TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable;

// produce<TaskListWindowViewModel, ITaskListWindowViewModel>::get_TaskItem
using ITaskListWindowViewModel_get_TaskItem_t = HRESULT(WINAPI*)(void* pThis,
                                                                 void** taskItem);
ITaskListWindowViewModel_get_TaskItem_t ITaskListWindowViewModel_get_TaskItem;

// produce<TaskListGroupViewModel, ITaskbarAppItemViewModel>::get_TaskGroup
using TaskListGroupViewModel_get_TaskGroup_t = HRESULT(WINAPI*)(void* pThis,
                                                                void** taskGroup);
TaskListGroupViewModel_get_TaskGroup_t TaskListGroupViewModel_get_TaskGroup;

// The group view model's default interface; optional, only widens the search.
void* TaskListGroupViewModel_ITaskListGroupViewModel_vftable;



bool IsViewModelBridgeAvailable() {
    return TaskListWindowViewModel_ITaskListWindowViewModel_vftable &&
           ITaskListWindowViewModel_get_TaskItem &&
           TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable &&
           TaskListGroupViewModel_get_TaskGroup;
}

HWND GetTaskItemWindow(IUnknown* taskItemUnknown) {
    winrt::com_ptr<ITaskItem> taskItem;
    HRESULT hr =
        taskItemUnknown->QueryInterface(IID_ITaskItem, taskItem.put_void());
    if (FAILED(hr)) {
        return nullptr;
    }

    HWND hWnd = nullptr;
    if (FAILED(taskItem->get_WindowId(&hWnd)) || !hWnd) {
        return nullptr;
    }

    // The taskbar may hand back a non-root window (a UWP core window inside
    // its frame, say); the minimized state lives on the top-level window.
    if (HWND root = GetAncestor(hWnd, GA_ROOT)) {
        hWnd = root;
    }

    return hWnd;
}

// A C++/WinRT implementation object has one vtable pointer per implemented
// interface, laid out consecutively. Given any interface pointer into the
// object, this finds the sub-object whose vtable is `vftable`, scanning a
// bounded window of slots around it (the Taskbar Labels mod does the same,
// unbounded, to get from ITaskbarAppItemViewModel to
// ITaskListWindowViewModel).
void* FindInterfaceByVtable(void* anyInterface, void* vftable) {
    if (!anyInterface || !vftable) {
        return nullptr;
    }

    void** slots = static_cast<void**>(anyInterface);
    for (int i = 0; i < 16; i++) {
        if (slots[i] == vftable) {
            return &slots[i];
        }
    }
    for (int i = 1; i < 16; i++) {
        if (slots[-i] == vftable) {
            return &slots[-i];
        }
    }

    return nullptr;
}

// Reads one pointer-sized value without faulting on a bad address.
bool SafeReadPointer(const void* address, void** value) {
    SIZE_T read = 0;
    return ReadProcessMemory(GetCurrentProcess(), address, value, sizeof(*value),
                             &read) &&
           read == sizeof(*value);
}

bool IsKnownViewModelVtable(void* vftable) {
    return vftable &&
           (vftable == TaskListWindowViewModel_ITaskListWindowViewModel_vftable ||
            vftable == TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable ||
            vftable == TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable ||
            vftable == TaskListGroupViewModel_ITaskListGroupViewModel_vftable);
}

// Whether `candidate` points into a TaskListWindowViewModel or
// TaskListGroupViewModel. A C++/WinRT object starts with one vtable pointer
// per implemented interface. For a pointer known to be an object's identity
// (a resolved weak reference, an IInspectable) the known vtables are among
// its first slots whichever interface comes first, so several are checked;
// for a raw field pointer, which may point mid-object, only its own slot is.
// A vtable address can only ever appear as an object's own slot, so a match
// identifies the object type without ambiguity.
bool IsKnownViewModelPointer(void* candidate, bool isObjectIdentity = false) {
    auto address = reinterpret_cast<uintptr_t>(candidate);
    if (!address || (address & 7) || address > 0x00007FFFFFFFFFFFull) {
        return false;
    }

    auto slots = static_cast<void**>(candidate);
    int limit = isObjectIdentity ? 12 : 1;
    for (int i = 0; i < limit; i++) {
        void* vftable = nullptr;
        if (!SafeReadPointer(&slots[i], &vftable)) {
            return false;
        }
        if (IsKnownViewModelVtable(vftable)) {
            return true;
        }
    }

    return false;
}






// The ItemsRepeater hosting a button, or null when its parent isn't one.
winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater GetHostingRepeater(
    const Xaml::FrameworkElement& button) {
    auto parent = Xaml::Media::VisualTreeHelper::GetParent(button);
    if (!parent) {
        return nullptr;
    }
    return parent.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
}

// The index of the item a realized button shows, or -1 if the repeater
// doesn't currently associate the button with any item: an ItemsRepeater
// keeps recycled elements in its visual tree for reuse, and those carry
// stale labels but aren't shown.
int GetRepeaterItemIndex(
    const winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater& repeater,
    const Xaml::FrameworkElement& button,
    int count) {
    int index = repeater.GetElementIndex(button);
    if (index >= 0) {
        return index < count ? index : -1;
    }

    // GetElementIndex can decline for elements it does hold; the inverse
    // query is authoritative. Compare COM identities, since the handles may
    // hold different interfaces of the same object.
    void* buttonIdentity =
        winrt::get_abi(button.as<winrt::Windows::Foundation::IUnknown>());
    for (int i = 0; i < count; i++) {
        auto element = repeater.TryGetElement(i);
        if (element &&
            winrt::get_abi(element.as<winrt::Windows::Foundation::IUnknown>()) ==
                buttonIdentity) {
            return i;
        }
    }

    return -1;
}

// True for a button that sits in a repeater's tree without being one of its
// realized items. Such a button isn't shown and has nothing to represent.
bool IsUnrealizedRepeaterChild(const Xaml::FrameworkElement& button) {
    try {
        auto repeater = GetHostingRepeater(button);
        if (!repeater) {
            return false;
        }

        auto itemsSourceView = repeater.ItemsSourceView();
        int count = itemsSourceView ? itemsSourceView.Count() : 0;
        return GetRepeaterItemIndex(repeater, button, count) < 0;
    } catch (...) {
        return false;
    }
}

// The item the ItemsRepeater hosting the button created it for: the
// repeater owns the data, and with x:Bind templates the button itself keeps
// no reference to it. Covers the taskbar's own repeater and the overflow
// flyout's.
winrt::com_ptr<IUnknown> ViewModelFromRepeater(
    const Xaml::FrameworkElement& button) {
    winrt::com_ptr<IUnknown> viewModel;

    try {
        auto repeater = GetHostingRepeater(button);
        if (!repeater) {
            static std::atomic<bool> logged;
            if (!logged.exchange(true)) {
                auto parent = Xaml::Media::VisualTreeHelper::GetParent(button);
                Wh_Log(L"Button parent is not an ItemsRepeater: %s",
                       parent ? winrt::get_class_name(parent).c_str()
                              : L"(none)");
            }
            return viewModel;
        }

        auto itemsSourceView = repeater.ItemsSourceView();
        int count = itemsSourceView ? itemsSourceView.Count() : 0;
        int index = GetRepeaterItemIndex(repeater, button, count);
        if (index < 0) {
            return viewModel;
        }

        auto item = itemsSourceView.GetAt(index);

        // The view can return null for sources it reaches only through the
        // bindable interfaces; read the source itself in that case.
        if (!item) {
            auto source = repeater.ItemsSource();
            static std::atomic<bool> logged;
            if (!logged.exchange(true)) {
                Wh_Log(L"ItemsSourceView.GetAt(%d) is null; source class: %s",
                       index,
                       source ? winrt::get_class_name(source).c_str()
                              : L"(none)");
            }

            if (source) {
                if (auto vector = source.try_as<
                        winrt::Windows::Foundation::Collections::IVector<
                            winrt::Windows::Foundation::IInspectable>>()) {
                    if (index < static_cast<int>(vector.Size())) {
                        item = vector.GetAt(index);
                    }
                } else if (auto bindable = source.try_as<
                               winrt::Windows::UI::Xaml::Interop::
                                   IBindableVector>()) {
                    if (index < static_cast<int>(bindable.Size())) {
                        item = bindable.GetAt(index);
                    }
                }
            }
        }

        if (!item) {
            static std::atomic<bool> logged;
            if (!logged.exchange(true)) {
                Wh_Log(L"Repeater item %d unavailable from the source too",
                       index);
            }
            return viewModel;
        }

        if (IsKnownViewModelPointer(winrt::get_abi(item),
                                    /*isObjectIdentity=*/true)) {
            viewModel.copy_from(static_cast<IUnknown*>(winrt::get_abi(item)));
            return viewModel;
        }

        // Not a view model we know. Say what it is, once.
        static std::atomic<bool> logged;
        if (!logged.exchange(true)) {
            Wh_Log(L"Repeater item not recognized: index=%d class=%s", index,
                   winrt::get_class_name(item).c_str());
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Repeater lookup error: %08X",
               static_cast<unsigned int>(e.code()));
    }

    return viewModel;
}

// The button's view model, or null.
winrt::com_ptr<IUnknown> GetButtonViewModel(const Xaml::FrameworkElement& button) {
    return ViewModelFromRepeater(button);
}

// The windows behind a button, from its view model. nullopt means the bridge
// couldn't be used for this button and the caller should fall back; an empty
// vector means the button has no windows (pinned, not running).
std::optional<std::vector<HWND>> ResolveWindowsViaViewModel(
    const Xaml::FrameworkElement& button) {
    if (!IsViewModelBridgeAvailable()) {
        return std::nullopt;
    }

    auto viewModel = GetButtonViewModel(button);
    if (!viewModel) {
        return std::nullopt;
    }

    std::vector<HWND> windows;

    // One window.
    if (void* windowViewModel = FindInterfaceByVtable(
            viewModel.get(),
            TaskListWindowViewModel_ITaskListWindowViewModel_vftable)) {
        winrt::com_ptr<IUnknown> taskItem;
        HRESULT hr = ITaskListWindowViewModel_get_TaskItem(windowViewModel,
                                                           taskItem.put_void());
        if (SUCCEEDED(hr) && taskItem) {
            if (HWND hWnd = GetTaskItemWindow(taskItem.get())) {
                windows.push_back(hWnd);
            }
        }
        return windows;
    }

    // A group: all of its windows.
    if (void* groupViewModel = FindInterfaceByVtable(
            viewModel.get(),
            TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable)) {
        winrt::com_ptr<IUnknown> taskGroupUnknown;
        HRESULT hr = TaskListGroupViewModel_get_TaskGroup(
            groupViewModel, taskGroupUnknown.put_void());
        if (FAILED(hr) || !taskGroupUnknown) {
            return windows;
        }

        winrt::com_ptr<ITaskGroup> taskGroup;
        hr = taskGroupUnknown->QueryInterface(IID_ITaskGroup,
                                              taskGroup.put_void());
        if (FAILED(hr)) {
            return windows;
        }

        // An IVectorView<TaskItem>, which has the same ABI.
        winrt::Windows::Foundation::Collections::IVectorView<
            winrt::Windows::Foundation::IInspectable>
            items;
        hr = taskGroup->get_Items(winrt::put_abi(items));
        if (SUCCEEDED(hr) && items) {
            uint32_t count = items.Size();
            for (uint32_t i = 0; i < count; i++) {
                auto item = items.GetAt(i);
                if (HWND hWnd = GetTaskItemWindow(
                        static_cast<IUnknown*>(winrt::get_abi(item)))) {
                    windows.push_back(hWnd);
                }
            }
        }
        return windows;
    }

    // Neither known view model (new build?): let the caller fall back.
    return std::nullopt;
}

// ============================================================================
// Button -> windows resolver, part 2: title matching (fallback)
// ============================================================================
//
// Used only when the view-model symbols didn't resolve. Exact for "Never
// combine", where the label is the window title; blind to combined groups.

const std::vector<HWND>* ResolveWindowsForLabel(
    const Xaml::Controls::TextBlock& label,
    const WindowSnapshot& snapshot) {
    winrt::hstring text = label.Text();
    if (text.empty()) {
        return nullptr;
    }

    // TextBlock.Text returns the full string even when the visual is
    // trimmed with an ellipsis, so this compares against the real title.
    std::wstring title(text.c_str(), text.size());
    auto it = snapshot.byTitle.find(title);
    if (it != snapshot.byTitle.end()) {
        return &it->second;
    }

    it = snapshot.byTitleAll.find(title);
    if (it != snapshot.byTitleAll.end()) {
        return &it->second;
    }

    return nullptr;
}

// ============================================================================
// XAML helpers (UI thread only)
// ============================================================================

// The taskbar runs in a XAML island, where DependencyObject.Dispatcher is
// null. The thread's DispatcherQueue is the island-correct way to get back
// onto the UI thread; it is agile, so it can be used from any thread later
// on. This is only ever called on the UI thread (from the hooks or from a
// refresh pass already running there), so "current thread" is the right one.
DispatcherQueue GetCurrentUiQueue() {
    DispatcherQueue queue = nullptr;
    try {
        queue = DispatcherQueue::GetForCurrentThread();
    } catch (...) {
    }
    return queue;
}

bool IsTaskListButton(const Xaml::DependencyObject& element) {
    return winrt::get_class_name(element) == L"Taskbar.TaskListButton";
}

// Walks up to the top of the visual tree. Returns null if the element isn't
// in a tree yet (it will be registered on a later refresh).
Xaml::DependencyObject FindTreeRoot(const Xaml::DependencyObject& element) {
    Xaml::DependencyObject current = element;
    bool movedUp = false;

    for (int i = 0; i < 200; i++) {
        auto parent = Xaml::Media::VisualTreeHelper::GetParent(current);
        if (!parent) {
            break;
        }

        current = parent;
        movedUp = true;
    }

    return movedUp ? current : nullptr;
}

// Collects every TaskListButton under `root` without descending into the
// buttons themselves.
void CollectTaskListButtons(const Xaml::DependencyObject& root,
                            std::vector<Xaml::FrameworkElement>& result,
                            int depth = 0) {
    if (depth > 40) {
        return;
    }

    int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Xaml::Media::VisualTreeHelper::GetChild(root, i);

        if (IsTaskListButton(child)) {
            if (auto element = child.try_as<Xaml::FrameworkElement>()) {
                result.push_back(element);
            }
            continue;
        }

        CollectTaskListButtons(child, result, depth + 1);
    }
}

Xaml::Controls::TextBlock FindLabelControl(const Xaml::DependencyObject& root,
                                           int depth = 0) {
    if (depth > 12) {
        return nullptr;
    }

    int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = Xaml::Media::VisualTreeHelper::GetChild(root, i);

        if (auto element = child.try_as<Xaml::FrameworkElement>()) {
            if (element.Name() == L"LabelControl") {
                if (auto textBlock = element.try_as<Xaml::Controls::TextBlock>()) {
                    return textBlock;
                }
            }
        }

        if (auto found = FindLabelControl(child, depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

// ============================================================================
// Renderer (UI thread only)
// ============================================================================

void ApplyToButton(const Xaml::FrameworkElement& button,
                   LazyWindowSnapshot& snapshot);

// The TaskListButton that owns a LabelControl, or null.
Xaml::FrameworkElement FindOwningTaskListButton(
    const Xaml::DependencyObject& element) {
    Xaml::DependencyObject current = element;
    for (int i = 0; i < 12 && current; i++) {
        if (IsTaskListButton(current)) {
            return current.try_as<Xaml::FrameworkElement>();
        }
        current = Xaml::Media::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

// Records the label's original decoration and subscribes to its Text
// property, so a title change re-evaluates the button even when the taskbar
// doesn't refresh its visual states. Returns the original decoration.
// Set while a label's text is re-set to rebuild its layout, so the Text
// callback below ignores that write.
thread_local bool t_rebuildingLabel = false;

// Returns true if the label was not tracked before.
bool EnsureLabelTracked(const Xaml::Controls::TextBlock& label) {
    void* key = winrt::get_abi(label);

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_labels.find(key);
        if (it != g_labels.end()) {
            auto existing = it->second.label.get();
            if (existing && existing == label) {
                return false;
            }

            // Stale entry for a dead TextBlock whose address got reused.
            g_labels.erase(it);
        }
    }

    TextDecorations original = label.TextDecorations();
    DispatcherQueue queue = GetCurrentUiQueue();

    int64_t token = label.RegisterPropertyChangedCallback(
        Xaml::Controls::TextBlock::TextProperty(),
        [](const Xaml::DependencyObject& sender, const Xaml::DependencyProperty&) {
            if (g_unloading || t_rebuildingLabel) {
                return;
            }

            try {
                if (auto button = FindOwningTaskListButton(sender)) {
                    LazyWindowSnapshot snapshot;
                    ApplyToButton(button, snapshot);
                }
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"Text change handler error: %08X",
                       static_cast<unsigned int>(e.code()));
            } catch (...) {
                Wh_Log(L"Text change handler error");
            }
        });

    std::lock_guard<std::mutex> lock(g_mutex);
    g_labels.emplace(key, LabelEntry{winrt::make_weak(label), queue, original,
                                     token});
    return true;
}

TextDecorations WithUnderlineBit(TextDecorations value, bool underline) {
    auto bits = static_cast<uint32_t>(value);
    auto bit = static_cast<uint32_t>(TextDecorations::Underline);
    return static_cast<TextDecorations>(underline ? (bits | bit)
                                                  : (bits & ~bit));
}

bool HasUnderlineBit(TextDecorations value) {
    return (static_cast<uint32_t>(value) &
            static_cast<uint32_t>(TextDecorations::Underline)) != 0;
}

// Sets or clears the Underline bit on the label, and nothing else, so a
// theme's strikethrough survives.
//
// Setting works through the TextBlock property. Clearing does not: XAML
// keeps drawing a decoration that was once applied to a TextBlock even after
// the property reads back as None, because a "no decoration" value doesn't
// rebuild the text layout. So clearing also rebuilds the inline content by
// re-setting the text, which creates a fresh Run with no decoration, and
// clears the bit on any Run that carries it.
// Rebuilds the label's inline content from its text, which replaces any
// Runs (and any cached text layout) with a single plain Run. Setting the
// same string is a no-op, so the text is cleared first; both writes happen
// before the next layout pass, so nothing flickers.
void RebuildLabelText(const Xaml::Controls::TextBlock& label) {
    winrt::hstring text = label.Text();
    t_rebuildingLabel = true;
    label.Text(L"");
    label.Text(text);
    t_rebuildingLabel = false;
    label.InvalidateMeasure();
}

bool IsCombiningMark(wchar_t c) {
    return (c >= 0x0300 && c <= 0x036F) ||  // combining diacriticals
           (c >= 0x0591 && c <= 0x05C7) ||  // Hebrew points
           (c >= 0x0610 && c <= 0x061A) ||  // Arabic marks
           (c >= 0x064B && c <= 0x065F) ||
           (c >= 0x0670 && c <= 0x0670) ||
           (c >= 0x06D6 && c <= 0x06ED) ||
           (c >= 0x1AB0 && c <= 0x1AFF) ||
           (c >= 0x1DC0 && c <= 0x1DFF) ||
           (c >= 0x20D0 && c <= 0x20FF) ||
           (c >= 0xFE00 && c <= 0xFE0F) ||  // variation selectors
           (c >= 0xFE20 && c <= 0xFE2F);
}

// Number of UTF-16 code units making up the first visible letter: leading
// whitespace is skipped, a surrogate pair stays together, and combining
// marks (Hebrew points, Arabic harakat, accents) stay with their base.
// Returns the start offset through `start`.
size_t FirstLetterSpan(std::wstring_view text, size_t* start) {
    size_t i = 0;
    while (i < text.size() && iswspace(text[i])) {
        i++;
    }
    *start = i;
    if (i >= text.size()) {
        return 0;
    }

    size_t n = 1;
    if (IS_HIGH_SURROGATE(text[i]) && i + 1 < text.size() &&
        IS_LOW_SURROGATE(text[i + 1])) {
        n = 2;
    }
    while (i + n < text.size() && IsCombiningMark(text[i + n])) {
        n++;
    }
    return n;
}

// Puts the underline on the first letter only, by splitting the text into
// Runs. Idempotent: a label already split the right way is left alone.
void ApplyFirstLetterUnderline(const Xaml::Controls::TextBlock& label) {
    winrt::hstring text = label.Text();
    std::wstring_view view(text.c_str(), text.size());

    size_t start = 0;
    size_t length = FirstLetterSpan(view, &start);
    if (length == 0) {
        return;
    }

    std::wstring_view head = view.substr(0, start);
    std::wstring_view letter = view.substr(start, length);
    std::wstring_view tail = view.substr(start + length);

    auto inlines = label.Inlines();
    if (!inlines) {
        return;
    }

    // Already in the wanted shape?
    uint32_t wanted = (head.empty() ? 0u : 1u) + 1u + (tail.empty() ? 0u : 1u);
    if (inlines.Size() == wanted) {
        bool matches = true;
        uint32_t index = 0;
        auto check = [&](std::wstring_view expected, bool underlined) {
            if (!matches) {
                return;
            }
            auto run = inlines.GetAt(index++).try_as<Xaml::Documents::Run>();
            if (!run || run.Text() != expected ||
                HasUnderlineBit(run.TextDecorations()) != underlined) {
                matches = false;
            }
        };
        if (!head.empty()) {
            check(head, false);
        }
        check(letter, true);
        if (!tail.empty()) {
            check(tail, false);
        }
        if (matches) {
            return;
        }
    }

    auto makeRun = [](std::wstring_view part, bool underlined) {
        Xaml::Documents::Run run;
        run.Text(winrt::hstring(part));
        if (underlined) {
            run.TextDecorations(TextDecorations::Underline);
        }
        return run;
    };

    t_rebuildingLabel = true;
    inlines.Clear();
    if (!head.empty()) {
        inlines.Append(makeRun(head, false));
    }
    inlines.Append(makeRun(letter, true));
    if (!tail.empty()) {
        inlines.Append(makeRun(tail, false));
    }
    t_rebuildingLabel = false;
    label.InvalidateMeasure();
}

// Sets or clears the underline on the label, and nothing else, so a theme's
// strikethrough survives.
//
// Full style: the TextBlock's Underline bit. First-letter style: the bit on
// a Run holding just the first letter. Clearing does not rely on the
// TextBlock property alone: XAML keeps drawing a decoration that was once
// applied even after the property reads back as None, because a "no
// decoration" value doesn't rebuild the text layout; so clearing also
// rebuilds the inline content from the text.
void SetLabelUnderline(const Xaml::Controls::TextBlock& label,
                       bool underline,
                       bool forceRebuild = false) {
    TextDecorations current = label.TextDecorations();

    bool anyRunUnderlined = false;
    uint32_t inlineCount = 0;
    try {
        auto inlines = label.Inlines();
        inlineCount = inlines ? inlines.Size() : 0;
        for (uint32_t i = 0; i < inlineCount; i++) {
            if (HasUnderlineBit(inlines.GetAt(i).TextDecorations())) {
                anyRunUnderlined = true;
                break;
            }
        }
    } catch (...) {
    }

    if (underline) {
        if (g_settings.underlineStyle == UnderlineStyle::kFirstLetter) {
            if (HasUnderlineBit(current)) {
                label.TextDecorations(WithUnderlineBit(current, false));
            }
            try {
                ApplyFirstLetterUnderline(label);
            } catch (...) {
            }
            return;
        }

        // Full style. A label split for the first-letter style is first put
        // back together.
        if (inlineCount > 1 || anyRunUnderlined) {
            RebuildLabelText(label);
        }
        if (!HasUnderlineBit(current)) {
            label.TextDecorations(WithUnderlineBit(current, true));
        }
        return;
    }

    if (!HasUnderlineBit(current) && !anyRunUnderlined && inlineCount <= 1 &&
        !forceRebuild) {
        return;
    }

    if (HasUnderlineBit(current)) {
        label.TextDecorations(WithUnderlineBit(current, false));
    }

    RebuildLabelText(label);
}

// "Plain" clears the Underline bit rather than restoring a remembered value,
// so a stale capture (for example after a reload that couldn't restore) can
// never leave an underline behind.
void ApplyUnderline(const Xaml::Controls::TextBlock& label, bool underline) {
    // A label seen for the first time may still carry a decoration drawn by
    // an earlier session that the property no longer reports; rebuild it
    // once when it is meant to be plain.
    bool firstSeen = EnsureLabelTracked(label);
    SetLabelUnderline(label, underline, /*forceRebuild=*/firstSeen);
}

void ApplyToButton(const Xaml::FrameworkElement& button,
                   LazyWindowSnapshot& snapshot) {
    if (g_unloading) {
        return;
    }

    // The template may not be applied yet (fresh button) or the label may
    // have been collapsed/recreated (combine setting toggled); re-finding it
    // every time keeps us honest.
    auto label = FindLabelControl(button);
    if (!label) {
        return;
    }

    // A recycled button isn't shown; leave it alone until the repeater reuses
    // it (its text changes then, which re-evaluates it).
    if (IsUnrealizedRepeaterChild(button)) {
        if (g_settings.debugLog) {
            Wh_Log(L"[%s] skipped: not a realized repeater item",
                   label.Text().c_str());
        }
        return;
    }

    bool underline = false;

    auto windows = ResolveWindowsViaViewModel(button);
    const std::vector<HWND>* byTitle = nullptr;
    if (windows) {
        underline = ShouldUnderline(*windows);
    } else if ((byTitle = ResolveWindowsForLabel(label, snapshot.get()))) {
        underline = ShouldUnderline(*byTitle);
    }

    if (g_settings.debugLog) {
        const std::vector<HWND>* list = windows ? &*windows : byTitle;
        std::wstring detail;
        if (list) {
            for (HWND hWnd : *list) {
                WCHAR part[64];
                swprintf_s(part, L" %p[%s%s%s]", hWnd,
                           IsWindow(hWnd) ? L"" : L"dead,",
                           IsIconic(hWnd) ? L"iconic" : L"shown",
                           IsWindowCloaked(hWnd) ? L",cloaked" : L"");
                detail += part;
            }
        }
        Wh_Log(L"[%s] via %s:%s -> %s (td=%u before)", label.Text().c_str(),
               windows ? L"vm" : (byTitle ? L"title" : L"none"),
               detail.c_str(), underline ? L"underline" : L"plain",
               static_cast<unsigned int>(label.TextDecorations()));
    }

    // Logged once per outcome, since the first button evaluated can run
    // before the bridge is fully set up.
    static std::atomic<bool> loggedViewModel, loggedFallback;
    if (windows) {
        if (!loggedViewModel.exchange(true)) {
            Wh_Log(L"Resolver in use: view model (%zu window(s) for this button)",
                   windows->size());
        }
    } else if (!loggedFallback.exchange(true)) {
        Wh_Log(L"Resolver in use: title matching (fallback)");
    }

    ApplyUnderline(label, underline);
}

// ============================================================================
// Root registry and refresh passes
// ============================================================================

void StartEventThreadIfNeeded();
void RequestDeferredRefresh();

// Registers the tree containing `element`. Returns true if the tree is new.
bool RegisterRootOf(const Xaml::DependencyObject& element) {
    auto root = FindTreeRoot(element);
    if (!root) {
        return false;
    }

    void* key = winrt::get_abi(root);
    DispatcherQueue queue = GetCurrentUiQueue();
    if (!queue) {
        Wh_Log(L"No DispatcherQueue for this UI thread");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (auto it = g_roots.begin(); it != g_roots.end();) {
            if (it->key == key) {
                auto existing = it->root.get();
                if (existing && existing == root) {
                    return false;
                }

                // Dead object at a reused address.
                it = g_roots.erase(it);
                continue;
            }
            ++it;
        }

        g_roots.push_back(RootEntry{key, winrt::make_weak(root), queue});
    }

    Wh_Log(L"Registered taskbar tree root: %s",
           winrt::get_class_name(root).c_str());
    StartEventThreadIfNeeded();
    return true;
}

std::vector<DispatcherQueue> CollectQueues() {
    std::vector<DispatcherQueue> result;

    auto add = [&result](const DispatcherQueue& queue) {
        if (!queue) {
            return;
        }
        for (const auto& existing : result) {
            if (existing == queue) {
                return;
            }
        }
        result.push_back(queue);
    };

    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto& entry : g_roots) {
        add(entry.queue);
    }
    for (const auto& [key, entry] : g_labels) {
        add(entry.queue);
    }

    return result;
}

// Re-evaluates every button in every tree that lives on the calling UI thread
// and prunes entries whose objects are gone.
void RefreshTreesOnCurrentThread() {
    if (g_unloading) {
        return;
    }

    LazyWindowSnapshot snapshot;

    std::vector<std::pair<void*, winrt::weak_ref<Xaml::DependencyObject>>> roots;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& entry : g_roots) {
            if (entry.queue.HasThreadAccess()) {
                roots.emplace_back(entry.key, entry.root);
            }
        }
    }

    std::vector<void*> deadRoots;
    for (const auto& [key, weakRoot] : roots) {
        auto root = weakRoot.get();
        if (!root) {
            deadRoots.push_back(key);
            continue;
        }

        try {
            std::vector<Xaml::FrameworkElement> buttons;
            CollectTaskListButtons(root, buttons);
            for (const auto& button : buttons) {
                ApplyToButton(button, snapshot);
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Refresh error: %08X", static_cast<unsigned int>(e.code()));
        } catch (...) {
            Wh_Log(L"Refresh error");
        }
    }

    // Prune labels owned by this thread whose TextBlock is gone.
    std::vector<std::pair<void*, winrt::weak_ref<Xaml::Controls::TextBlock>>> labels;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& [key, entry] : g_labels) {
            if (entry.queue.HasThreadAccess()) {
                labels.emplace_back(key, entry.label);
            }
        }
    }

    std::vector<void*> deadLabels;
    for (const auto& [key, weakLabel] : labels) {
        if (!weakLabel.get()) {
            deadLabels.push_back(key);
        }
    }

    if (!deadRoots.empty() || !deadLabels.empty()) {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (void* key : deadRoots) {
            for (auto it = g_roots.begin(); it != g_roots.end(); ++it) {
                if (it->key == key) {
                    g_roots.erase(it);
                    break;
                }
            }
        }
        for (void* key : deadLabels) {
            g_labels.erase(key);
        }
    }
}

// Called from any thread. Posts one refresh pass per UI thread.
void ScheduleRefreshAll() {
    if (g_unloading) {
        return;
    }

    for (const auto& queue : CollectQueues()) {
        try {
            bool enqueued = queue.TryEnqueue(DispatcherQueuePriority::Normal, [] {
                if (!g_unloading) {
                    RefreshTreesOnCurrentThread();
                }
            });
            if (!enqueued) {
                Wh_Log(L"TryEnqueue returned false");
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"TryEnqueue error: %08X", static_cast<unsigned int>(e.code()));
        } catch (...) {
            Wh_Log(L"TryEnqueue error");
        }
    }
}

// Restores every label owned by the calling UI thread and drops it from the
// registry. Used on unload.
void RestoreLabelsOnCurrentThread() {
    std::vector<std::pair<void*, LabelEntry>> entries;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& [key, entry] : g_labels) {
            if (entry.queue.HasThreadAccess()) {
                entries.emplace_back(key, entry);
            }
        }
    }

    for (const auto& [key, entry] : entries) {
        try {
            if (auto label = entry.label.get()) {
                label.UnregisterPropertyChangedCallback(
                    Xaml::Controls::TextBlock::TextProperty(),
                    entry.textChangedToken);
                SetLabelUnderline(label, false, /*forceRebuild=*/true);
            }
        } catch (...) {
            Wh_Log(L"Restore error");
        }

        std::lock_guard<std::mutex> lock(g_mutex);
        g_labels.erase(key);
    }
}

// ============================================================================
// WinEvent thread: invalidation signals, coalesced into refresh passes
// ============================================================================

std::atomic<bool> g_eventThreadStarted;
HANDLE g_eventThread;
HANDLE g_eventThreadReady;
DWORD g_eventThreadId;

// Only touched on the event thread.
UINT_PTR g_refreshTimer;
UINT_PTR g_followUpTimer;

void CALLBACK RefreshTimerProc(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);

    if (id == g_refreshTimer) {
        g_refreshTimer = 0;
        ScheduleRefreshAll();

        // Second pass a bit later: minimize/restore state can still be
        // changing during the first one.
        if (!g_followUpTimer) {
            g_followUpTimer = SetTimer(nullptr, 0, FOLLOW_UP_DELAY_MS,
                                       RefreshTimerProc);
        }
    } else if (id == g_followUpTimer) {
        g_followUpTimer = 0;
        ScheduleRefreshAll();
    }
}

void ArmRefreshTimer() {
    if (!g_refreshTimer) {
        g_refreshTimer = SetTimer(nullptr, 0, g_settings.refreshDelayMs,
                                  RefreshTimerProc);
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    // Destroy events can't be checked for top-level-ness (the window is
    // already gone); everything else is filtered to top-level windows.
    if (event != EVENT_OBJECT_DESTROY && GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return;
    }

    ArmRefreshTimer();
}

DWORD WINAPI EventThreadProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // Force the message queue into existence before signaling readiness, so
    // PostThreadMessage from other threads can't be lost.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_eventThreadReady);

    const struct {
        DWORD min;
        DWORD max;
    } ranges[] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},  // DESTROY, SHOW, HIDE
        {EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };

    HWINEVENTHOOK hooks[ARRAYSIZE(ranges)] = {};
    for (size_t i = 0; i < ARRAYSIZE(ranges); i++) {
        hooks[i] = SetWinEventHook(ranges[i].min, ranges[i].max, nullptr,
                                   WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (!hooks[i]) {
            Wh_Log(L"SetWinEventHook failed for range %04X-%04X",
                   ranges[i].min, ranges[i].max);
        }
    }

    // Explorer was most likely already running; evaluate everything known so
    // far once right away.
    ScheduleRefreshAll();

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == WM_APP_REQUEST_REFRESH) {
            ArmRefreshTimer();
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_refreshTimer) {
        KillTimer(nullptr, g_refreshTimer);
        g_refreshTimer = 0;
    }
    if (g_followUpTimer) {
        KillTimer(nullptr, g_followUpTimer);
        g_followUpTimer = 0;
    }

    for (HWINEVENTHOOK hook : hooks) {
        if (hook) {
            UnhookWinEvent(hook);
        }
    }

    CoUninitialize();
    return 0;
}

void StartEventThreadIfNeeded() {
    if (g_unloading || g_eventThreadStarted.exchange(true)) {
        return;
    }

    g_eventThreadReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_eventThread =
        CreateThread(nullptr, 0, EventThreadProc, nullptr, 0, &g_eventThreadId);
    if (!g_eventThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

// Asks the event thread for a coalesced re-check of everything. Safe to call
// from the UI thread hooks; a no-op before the thread exists (its startup
// does a full pass anyway).
void RequestDeferredRefresh() {
    if (g_eventThreadId && !g_unloading) {
        PostThreadMessage(g_eventThreadId, WM_APP_REQUEST_REFRESH, 0, 0);
    }
}

void StopEventThread() {
    if (!g_eventThread) {
        return;
    }

    if (g_eventThreadReady) {
        WaitForSingleObject(g_eventThreadReady, 2000);
    }

    PostThreadMessage(g_eventThreadId, WM_QUIT, 0, 0);
    if (WaitForSingleObject(g_eventThread, 3000) != WAIT_OBJECT_0) {
        Wh_Log(L"Event thread did not exit in time");
    }

    CloseHandle(g_eventThread);
    g_eventThread = nullptr;
    g_eventThreadId = 0;

    if (g_eventThreadReady) {
        CloseHandle(g_eventThreadReady);
        g_eventThreadReady = nullptr;
    }
}

// ============================================================================
// Taskbar.View.dll hooks
// ============================================================================

std::atomic<bool> g_elementVerified;
std::atomic<bool> g_elementBroken;

// Gets the XAML element from the TaskListButton implementation object. The
// IInspectable of the composed XAML control sits at this offset in the
// C++/WinRT implementation layout; the same technique is used by the
// Taskbar Labels and Taskbar Icon Size mods. The class name is verified once
// so an offset mismatch on a new build degrades to "mod does nothing" rather
// than a crash.
Xaml::FrameworkElement TaskListButtonFromThis(void* pThis) {
    if (g_elementBroken) {
        return nullptr;
    }

    winrt::Windows::Foundation::IInspectable inspectable = nullptr;
    winrt::copy_from_abi(inspectable,
                         static_cast<void*>(static_cast<void**>(pThis) + 3));

    auto element = inspectable.try_as<Xaml::FrameworkElement>();
    if (!element) {
        return nullptr;
    }

    if (!g_elementVerified.exchange(true)) {
        winrt::hstring className = winrt::get_class_name(element);
        Wh_Log(L"TaskListButton element class: %s", className.c_str());
        if (className != L"Taskbar.TaskListButton") {
            Wh_Log(L"Unexpected element class, disabling the mod's effect");
            g_elementBroken = true;
            return nullptr;
        }
    }

    return element;
}

void OnTaskListButtonUpdated(void* pThis) {
    if (g_unloading) {
        return;
    }

    try {
        auto element = TaskListButtonFromThis(pThis);
        if (!element) {
            return;
        }

        if (RegisterRootOf(element)) {
            // First time we see this tree: decorate everything in it, which
            // covers all the buttons that existed before the mod loaded.
            RefreshTreesOnCurrentThread();
        } else {
            LazyWindowSnapshot snapshot;
            ApplyToButton(element, snapshot);
        }

        // The taskbar often refreshes a button a moment before the window's
        // minimized state is actually committed; re-check shortly after.
        RequestDeferredRefresh();
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"TaskListButton hook error: %08X",
               static_cast<unsigned int>(e.code()));
    } catch (...) {
        Wh_Log(L"TaskListButton hook error");
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    TaskListButton_UpdateButtonPadding_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

using TaskListButton_UpdateIconColumnDefinition_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateIconColumnDefinition_t
    TaskListButton_UpdateIconColumnDefinition_Original;
void WINAPI TaskListButton_UpdateIconColumnDefinition_Hook(void* pThis) {
    TaskListButton_UpdateIconColumnDefinition_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK taskbarViewDllHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        // The two below are optional convenience triggers: they run for every
        // button on layout/settings changes, which is how buttons that
        // existed before the mod loaded get seen right after enabling.
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
            &TaskListButton_UpdateIconColumnDefinition_Original,
            TaskListButton_UpdateIconColumnDefinition_Hook,
            true,
        },
        // View-model bridge (addresses only, nothing is hooked). All
        // optional: without them the title-matching fallback is used.
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::`vftable')"},
            &TaskListWindowViewModel_ITaskListWindowViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
            &TaskListWindowViewModel_ITaskbarAppItemViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &ITaskListWindowViewModel_get_TaskItem,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::`vftable')"},
            &TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_TaskGroup(void * *))"},
            &TaskListGroupViewModel_get_TaskGroup,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskListGroupViewModel>::`vftable')"},
            &TaskListGroupViewModel_ITaskListGroupViewModel_vftable,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarViewDllHooks,
                                    ARRAYSIZE(taskbarViewDllHooks))) {
        Wh_Log(L"HookSymbols failed for the taskbar view module");
        return false;
    }

    Wh_Log(L"Optional hooks: UpdateButtonPadding=%s UpdateIconColumnDefinition=%s",
           TaskListButton_UpdateButtonPadding_Original ? L"yes" : L"no",
           TaskListButton_UpdateIconColumnDefinition_Original ? L"yes" : L"no");
    Wh_Log(L"View-model bridge: windowVM=%s get_TaskItem=%s groupVM=%s "
           L"get_TaskGroup=%s",
           TaskListWindowViewModel_ITaskListWindowViewModel_vftable ? L"yes"
                                                                    : L"no",
           ITaskListWindowViewModel_get_TaskItem ? L"yes" : L"no",
           TaskListGroupViewModel_ITaskbarAppItemViewModel_vftable ? L"yes"
                                                                   : L"no",
           TaskListGroupViewModel_get_TaskGroup ? L"yes" : L"no");
    Wh_Log(L"Group VM default-interface vtable: %s",
           TaskListGroupViewModel_ITaskListGroupViewModel_vftable ? L"yes"
                                                                  : L"no");

    return true;
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
        Wh_Log(L"Taskbar view module loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

// ============================================================================
// Helpers
// ============================================================================

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd) {
        return nullptr;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hTaskbarWnd, &processId);
    return processId == GetCurrentProcessId() ? hTaskbarWnd : nullptr;
}

// ----------------------------------------------------------------------------
// Enable-time trigger
// ----------------------------------------------------------------------------
//
// Buttons that existed before the mod loaded are only seen once one of the
// hooked TaskListButton paths runs for any button; from that button the whole
// tree is registered and decorated. The thing guaranteed to run
// UpdateVisualStates is a change of a button's active state, so the probe
// below flips the active button to another window and straight back. Both
// changes are processed on the UI thread before the next composition frame,
// so nothing visibly moves and the taskbar ends in the state it started in.
// Secondary taskbars are fed from the same tray, so their trees register in
// the same pass.
//
// Three stages, each only if the previous one didn't register a tree:
//   1. ITaskbarList::ActivateTab - the documented COM path into
//      CTaskBand/CTaskListWnd, independent of how the tray listens for real
//      activations.
//   2. The shell hook message (HSHELL_WINDOWACTIVATED) on Shell_TrayWnd.
//   3. ITaskbarList::DeleteTab + AddTab on one window, which recreates its
//      button and runs the button-creation path. Visible as a brief blip of
//      that button, so it is the last resort.

bool IsTreeRegistered() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return !g_roots.empty();
}

struct ProbeWindows {
    HWND foreground;  // Foreground window, if it has a taskbar button.
    HWND other;       // Some other taskbar window, if any.
    HWND desktop;
};

ProbeWindows PickProbeWindows() {
    ProbeWindows result{};

    result.desktop = GetShellWindow();
    if (!result.desktop) {
        result.desktop = GetDesktopWindow();
    }

    HWND foreground = GetForegroundWindow();
    if (foreground && IsTaskbarCandidate(foreground)) {
        result.foreground = foreground;
    }

    WindowSnapshot snapshot = BuildWindowSnapshot();
    for (const auto& [title, windows] : snapshot.byTitle) {
        for (HWND hwnd : windows) {
            if (hwnd != foreground) {
                result.other = hwnd;
                break;
            }
        }
        if (result.other) {
            break;
        }
    }

    return result;
}

winrt::com_ptr<ITaskbarList> CreateTaskbarList() {
    winrt::com_ptr<ITaskbarList> list;
    HRESULT hr = CoCreateInstance(__uuidof(TaskbarList), nullptr,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(list.put()));
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance(TaskbarList) failed: %08X",
               static_cast<unsigned int>(hr));
        return nullptr;
    }

    hr = list->HrInit();
    if (FAILED(hr)) {
        Wh_Log(L"ITaskbarList::HrInit failed: %08X",
               static_cast<unsigned int>(hr));
        return nullptr;
    }

    return list;
}

// Stage 1.
void ProbeViaActivateTab(const ProbeWindows& windows) {
    if (!windows.other || !windows.foreground) {
        Wh_Log(L"ActivateTab probe skipped (need a foreground window with a "
               L"button plus one other taskbar window)");
        return;
    }

    auto list = CreateTaskbarList();
    if (!list) {
        return;
    }

    list->ActivateTab(windows.other);
    list->ActivateTab(windows.foreground);
    Wh_Log(L"ActivateTab probe sent, tree registered: %s",
           IsTreeRegistered() ? L"yes" : L"no");
}

// Stage 2.
void ProbeViaShellHook(HWND hTaskbarWnd, const ProbeWindows& windows) {
    UINT shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    if (!shellHookMsg) {
        return;
    }

    HWND probe = windows.other ? windows.other : windows.desktop;
    HWND restore = windows.foreground ? windows.foreground : windows.desktop;
    if (probe == restore) {
        Wh_Log(L"Shell hook probe skipped (nothing to flip)");
        return;
    }

    SendMessage(hTaskbarWnd, shellHookMsg, HSHELL_WINDOWACTIVATED,
                reinterpret_cast<LPARAM>(probe));
    SendMessage(hTaskbarWnd, shellHookMsg, HSHELL_WINDOWACTIVATED,
                reinterpret_cast<LPARAM>(restore));
    Wh_Log(L"Shell hook probe sent, tree registered: %s",
           IsTreeRegistered() ? L"yes" : L"no");
}

// Stage 3.
void ProbeViaRecreateButton(const ProbeWindows& windows) {
    HWND target = windows.other ? windows.other : windows.foreground;
    if (!target) {
        Wh_Log(L"Recreate probe skipped (no taskbar window)");
        return;
    }

    auto list = CreateTaskbarList();
    if (!list) {
        return;
    }

    list->DeleteTab(target);
    list->AddTab(target);
    if (target == windows.foreground) {
        list->ActivateTab(target);
    }
    Wh_Log(L"Recreate probe sent, tree registered: %s",
           IsTreeRegistered() ? L"yes" : L"no");
}

void NudgeTaskbar() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    if (IsTreeRegistered()) {
        return;
    }

    // ITaskbarList is a plain in-process COM object; make sure this thread
    // can create it.
    HRESULT coInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    ProbeWindows windows = PickProbeWindows();
    Wh_Log(L"Probe windows: foreground=%p other=%p", windows.foreground,
           windows.other);

    ProbeViaActivateTab(windows);
    if (!IsTreeRegistered()) {
        ProbeViaShellHook(hTaskbarWnd, windows);
    }
    if (!IsTreeRegistered()) {
        ProbeViaRecreateButton(windows);
    }
    if (!IsTreeRegistered()) {
        Wh_Log(L"No probe reached a button; existing buttons will be picked "
               L"up on their next refresh (hover or activation)");
    }

    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }
}

// ============================================================================
// Mod entry points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(module)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet, hooking LoadLibraryExW");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The module may have been loaded between Wh_ModInit and the hooks being
    // applied; catch that case.
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE module = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (HookTaskbarViewDllSymbols(module)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (g_taskbarViewDllLoaded) {
        NudgeTaskbar();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
    StopEventThread();

    // Put every label back on its own UI thread and wait for that to finish,
    // because after this returns the mod's code is going away.
    for (const auto& queue : CollectQueues()) {
        HANDLE done = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (!done) {
            continue;
        }

        bool enqueued = false;
        try {
            enqueued = queue.TryEnqueue(DispatcherQueuePriority::High, [done] {
                RestoreLabelsOnCurrentThread();
                SetEvent(done);
            });
        } catch (...) {
        }

        if (!enqueued) {
            Wh_Log(L"Label restore could not be dispatched");
        } else if (WaitForSingleObject(done, 3000) != WAIT_OBJECT_0) {
            Wh_Log(L"Label restore timed out on one UI thread");
        }

        CloseHandle(done);
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    g_roots.clear();
    g_labels.clear();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    ScheduleRefreshAll();
}
