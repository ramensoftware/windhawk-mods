// ==WindhawkMod==
// @id start-menu-hide-empty-categories
// @name Start Menu Hide Empty Categories
// @name:pt-BR Ocultar Categorias Vazias do Menu Iniciar
// @description Hides the empty category tiles of the Windows 11 Start menu's "All apps" category view and closes the gaps they leave.
// @description:pt-BR Oculta os blocos de categoria vazios da visualização por categorias de "Todos os aplicativos" do Menu Iniciar do Windows 11 e fecha os espaços que eles deixam.
// @version 1.0.0
// @author Feuer
// @github https://github.com/feuerdraws
// @twitter https://x.com/feuerdraws
// @include explorer.exe
// @include StartMenuExperienceHost.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lruntimeobject -luuid -luser32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Hide Empty Categories

> **Disclaimer:** this mod is vibe coded. It was written with an AI assistant and tested only by its author on
> one setup. It hooks private parts of the Start menu and restarts the Start menu process, so use it at your own risk.

In the Windows 11 Start menu's category view of **All apps**, a heading that has no apps still shows up as an empty
tile. This mod hides those tiles and shows them again as soon as an app is assigned to the heading. The tiles after
them move up to fill the gap. A tile counts as empty when its category holds no apps; pinned items and other tiles
are never touched.

**Before:**

![Before](https://i.imgur.com/ZrksjvR.png)

**After:**

![After](https://i.imgur.com/MyVa7UT.png)

This mod does not change any category, does not touch the registry, and has no settings. It is independent of
**Start Menu Category Manager** and works with it or without it.

When the mod is loaded (installed, updated or switched on again), it restarts the Start menu process if that process
does not have this copy of the mod yet. The new process loads with the mod from its start, so empty tiles are hidden
from the first time the menu is opened. A Start process that already has the mod (for example after Explorer
restarted) is left alone, and so is one that started within the last second. When the mod is switched off or removed,
the Start menu process is restarted once more so the tiles it had hidden come back. Start can flash or close for a
moment each time.

**The *Other* category.** If *Other* is empty, Windows 11 still shows an empty *Other* container. This mod hides it
too, but only while the mod is loaded.

## Tested on

Only Windows 11 build 26300, on one computer, with Windows in Brazilian Portuguese. On any other build the mod may
fail to attach; it then switches only the affected part off (see below).

**Languages.** Empty tiles are recognised by what they hold (a category with no apps), not by their heading, so the
mod does not depend on the display language. It has only been tried with Windows in Brazilian Portuguese.

## If a Windows update breaks the mod

The mod depends on private parts of the Start menu that Microsoft can change without notice. It was written and tested
on Windows 11 build 26300. When something stops matching, the mod keeps running and switches only the affected part
off.

- **Empty category tiles stay visible.** The required hook did not attach. The Windhawk log (lines starting with
  `[start]`) says so.
- **Empty tiles are hidden, but the other tiles do not move up.** Only the optional `ReplaceAll` hook was not found.

Where to look: the Windhawk log. The functions the mod hooks, all inside the Start menu DLL (`StartMenu.dll`, or
`StartUI.dll` on some builds), are:

1. `CategoryProvider::PruneAndCombineCategories`, which tells the mod that Start rebuilt its categories. This one is
   required. `StartBlendedFlexFrame::OnContextRequested` is an optional extra trigger.
2. The `ReplaceAll` function of the category list (`AllAppsGroupViewModel`), which is optional and only makes the
   tiles close up.

To find the new names after an update, search the Start menu DLL's symbols in Windhawk for those class and function
names.
*/
// ==/WindhawkModReadme==

#include <commctrl.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <cwchar>
#include <string>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxd = winrt::Windows::UI::Xaml::Data;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

namespace {

using IInspectableList = wf::Collections::IVector<wf::IInspectable>;

std::atomic<bool> g_shuttingDown{false};
std::atomic<bool> g_startModuleHooked = false;
bool g_isStartHost = false;  // True inside StartMenuExperienceHost.exe, false inside explorer.exe.

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

std::wstring WindowsBuildString() {
    typedef LONG(WINAPI * RtlGetVersion_t)(OSVERSIONINFOW*);
    OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof(info);
    if (HMODULE ntdll = GetModuleHandleW(L"ntdll.dll")) {
        auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion"));
        if (rtlGetVersion) rtlGetVersion(&info);
    }
    return std::to_wstring(info.dwBuildNumber);
}

// This mod's own module handle.
HMODULE ModuleInstance() {
    static HMODULE module = nullptr;
    if (!module) {
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ModuleInstance), &module);
    }
    return module;
}

enum class StartModState { Absent, Late, FromStart };

// Whether the process has this copy of the mod, and whether it has had it since it started. A copy loaded into a Start
// process that was already running comes after StartMenu.dll (or StartUI.dll) in the module list, which is in load
// order; Start has built its menus by then, so such a copy cannot add items to them. The DLL's file name carries the
// version, so an updated mod is a different file. If the module list cannot be read the answer is Absent.
StartModState ProcessModState(DWORD pid) {
    wchar_t selfPath[MAX_PATH]{};
    if (!GetModuleFileNameW(ModuleInstance(), selfPath, ARRAYSIZE(selfPath))) return StartModState::Absent;
    const wchar_t* selfName = PathFindFileNameW(selfPath);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snapshot == INVALID_HANDLE_VALUE) return StartModState::Absent;
    StartModState state = StartModState::Absent;
    bool sawStartMenuDll = false;
    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (Module32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szModule, L"StartMenu.dll") == 0 || _wcsicmp(entry.szModule, L"StartUI.dll") == 0) {
                sawStartMenuDll = true;
            } else if (_wcsicmp(entry.szModule, selfName) == 0) {
                state = sawStartMenuDll ? StartModState::Late : StartModState::FromStart;
                break;
            }
        } while (Module32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return state;
}

// Terminates the Start menu process so Windows starts a fresh one. A process that started within the last second is
// left alone. Only the current session's Start process is touched. With onlyIfWithoutMod, a process that already has
// this copy of the mod is left alone too. Calls from different threads run one after the other.
void RestartStartMenuHost(bool onlyIfWithoutMod = false) {
    static SRWLOCK s_lock = SRWLOCK_INIT;
    AcquireSRWLockExclusive(&s_lock);
    DWORD ownSession = 0;
    const bool haveSession = ProcessIdToSessionId(GetCurrentProcessId(), &ownSession) != 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        FILETIME nowFt{};
        GetSystemTimeAsFileTime(&nowFt);
        const ULONGLONG now = (static_cast<ULONGLONG>(nowFt.dwHighDateTime) << 32) | nowFt.dwLowDateTime;
        constexpr ULONGLONG kMinAge = 10000000ull;  // 1 s in 100 ns units.

        PROCESSENTRY32W entry{};
        entry.dwSize = sizeof(entry);
        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (_wcsicmp(entry.szExeFile, L"StartMenuExperienceHost.exe") != 0) continue;
                DWORD session = 0;
                if (haveSession && (!ProcessIdToSessionId(entry.th32ProcessID, &session) || session != ownSession)) continue;
                if (onlyIfWithoutMod) {
                    const StartModState modState = ProcessModState(entry.th32ProcessID);
                    if (modState == StartModState::FromStart) {
                        Wh_Log(L"Start (process %lu) has had this mod since it started; leaving it alone", entry.th32ProcessID);
                        continue;
                    }
                    Wh_Log(L"Start (process %lu) %ls; restarting it", entry.th32ProcessID,
                           modState == StartModState::Late ? L"got this mod only after it had started"
                                                           : L"does not have this mod");
                }
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                              entry.th32ProcessID);
                if (!hProcess) continue;
                FILETIME created{}, exited{}, kernel{}, user{};
                bool tooYoung = false;
                if (GetProcessTimes(hProcess, &created, &exited, &kernel, &user)) {
                    const ULONGLONG start = (static_cast<ULONGLONG>(created.dwHighDateTime) << 32) | created.dwLowDateTime;
                    tooYoung = now > start && now - start < kMinAge;
                }
                if (!tooYoung) TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            } while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
    ReleaseSRWLockExclusive(&s_lock);
}

// ---------------------------------------------------------------------------
// Hiding empty category tiles (runs inside StartMenuExperienceHost.exe)
// ---------------------------------------------------------------------------

// How many times Start has rebuilt its categories since this mod loaded, and whether the first rebuild was the one
// Start does when it starts up (the process was young), before anyone opened the menu.
std::atomic<int> g_rebuildCount{0};
bool g_startupCleanupAllowed = false;

// Reads through ReadProcessMemory instead of dereferencing after a VirtualQuery check. A check and a read cannot be
// made atomic, so memory that is freed in between would still fault. ReadProcessMemory on this process copies inside
// the kernel and reports bad memory as a failure instead of raising an exception.
bool ReadWordAt(const void* base, size_t offset, unsigned long long* out) {
    if (!base) return false;
    const void* p = static_cast<const unsigned char*>(base) + offset;
    unsigned long long value = 0;
    SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), p, &value, sizeof(value), &got) || got != sizeof(value)) return false;
    *out = value;
    return true;
}

// A category with no apps is a group whose own list of items is empty. This does not depend on the display language,
// and it never matches a pinned app or folder, because those are not groups.
bool IsEmptyGroup(wf::IInspectable const& entry) {
    if (!entry) return false;
    auto group = entry.try_as<wuxd::ICollectionViewGroup>();
    return group && group.GroupItems().Size() == 0;
}

// Removes the empty groups from a list of groups.
void RemoveEmptyGroups(IInspectableList const& items) {
    for (uint32_t i = items.Size(); i-- > 0;) {
        if (IsEmptyGroup(items.GetAt(i))) items.RemoveAt(i);
    }
}

// The tiles this mod has collapsed, so that only those are ever shown again or reset. Only used on Start's UI thread.
[[clang::no_destroy]] std::vector<winrt::weak_ref<wux::FrameworkElement>> g_collapsedTiles;

// True if the tile was one this mod collapsed (and forgets it).
bool TakeCollapsedTile(wux::FrameworkElement const& element) {
    bool found = false;
    for (size_t i = 0; i < g_collapsedTiles.size();) {
        auto tile = g_collapsedTiles[i].get();
        if (!tile) {
            g_collapsedTiles.erase(g_collapsedTiles.begin() + i);
            continue;
        }
        if (!found && winrt::get_abi(tile) == winrt::get_abi(element)) {
            found = true;
            g_collapsedTiles.erase(g_collapsedTiles.begin() + i);
            continue;
        }
        ++i;
    }
    return found;
}

// Shows again every tile this mod collapsed. Runs on Start's UI thread when the mod unloads.
void RestoreCollapsedTiles() {
    std::vector<winrt::weak_ref<wux::FrameworkElement>> tiles;
    tiles.swap(g_collapsedTiles);
    for (auto const& weakTile : tiles) {
        try {
            if (auto tile = weakTile.get()) tile.Visibility(wux::Visibility::Visible);
        } catch (...) {
        }
    }
}

// The GridView that shows the items of this panel, or null.
wuxc::GridView FindOwningGridView(wux::DependencyObject const& panel) {
    wux::DependencyObject walker = panel;
    for (int i = 0; i < 10; ++i) {
        walker = wuxm::VisualTreeHelper::GetParent(walker);
        if (!walker) return nullptr;
        if (auto gridView = walker.try_as<wuxc::GridView>()) return gridView;
    }
    return nullptr;
}

// The list the category view is bound to, or null when this GridView shows something else. The category list is a list
// of groups; the lists of pinned and recommended items are not.
IInspectableList CategoryListOf(wuxc::GridView const& gridView) {
    auto source = gridView.ItemsSource();
    if (!source) return nullptr;
    auto list = source.try_as<IInspectableList>();
    if (!list || list.Size() == 0) return nullptr;
    auto first = list.GetAt(0);
    if (!first || !first.try_as<wuxd::ICollectionViewGroup>()) return nullptr;
    return list;
}

// Hides the tile of every empty category, and shows again the tiles this mod hid that now hold apps.
void ApplyCategoryTiles(wux::DependencyObject const& panel) {
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(panel);
    for (int i = 0; i < count; ++i) {
        auto item = wuxm::VisualTreeHelper::GetChild(panel, i);
        if (!item.try_as<wuxc::GridViewItem>()) continue;
        auto element = item.try_as<wux::FrameworkElement>();
        if (!element) continue;

        if (IsEmptyGroup(element.DataContext())) {
            if (element.Visibility() != wux::Visibility::Collapsed) {
                element.Visibility(wux::Visibility::Collapsed);
                g_collapsedTiles.push_back(winrt::make_weak(element));
            }
        } else if (TakeCollapsedTile(element)) {
            element.Visibility(wux::Visibility::Visible);
        }
    }
}

// Handles the tiles of the category view: hides the empty ones, and, once, removes the empty groups from the list the
// view is bound to so the other tiles move up. The removal is only done for the list Start builds when it starts up,
// while nobody has opened the menu: the same removal from a timer right after the rebuild that happens when the menu
// opens crashed Start (access violation inside StartMenu.dll, 30 to 50 ms later). Those later lists are cleaned inside
// Start's own ReplaceAll call instead (see ReplaceAll_Hook).
void ProcessCategoryGrid(wux::DependencyObject const& panel) {
    auto gridView = FindOwningGridView(panel);
    if (!gridView) return;
    auto list = CategoryListOf(gridView);
    if (!list) return;

    ApplyCategoryTiles(panel);

    if (!g_startupCleanupAllowed || g_rebuildCount != 1) return;
    try {
        RemoveEmptyGroups(list);
    } catch (...) {
        static bool loggedFailure = false;
        if (!loggedFailure) {
            loggedFailure = true;
            Wh_Log(L"[start] Could not remove the empty category from the grid's list");
        }
    }
}

void FindItemGrids(wux::DependencyObject const& obj, int depth) {
    if (depth > 60) return;
    if (obj.try_as<wuxc::ItemsWrapGrid>()) {
        ProcessCategoryGrid(obj);
        return;  // The tiles inside are not searched.
    }
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(obj);
    for (int i = 0; i < count; ++i) {
        FindItemGrids(wuxm::VisualTreeHelper::GetChild(obj, i), depth + 1);
    }
}

// A weak reference: a strong global would be released on the wrong thread when the mod unloads.
[[clang::no_destroy]] winrt::weak_ref<wux::UIElement> g_startRoot;

void UpdateCategoryTiles() {
    try {
        wux::DependencyObject root{nullptr};
        if (auto window = wux::Window::Current()) {
            if (auto content = window.Content()) root = content;
        }
        if (!root) {
            if (auto saved = g_startRoot.get()) root = saved;  // Fallback: the frame from the last right-click.
        }
        if (!root) return;
        FindItemGrids(root, 0);
    } catch (...) {
        g_startRoot = nullptr;  // The tree was torn down; look for a fresh root next time.
    }
}

// The check runs on a timer inside Start's own window, so it also works when the menu was already open before the mod
// loaded. The timer only runs for a few seconds after something shows that Start is in use (the mod loading, a
// right-click, Start rebuilding its categories); it stops itself after that and is started again by the next sign.
// The window is subclassed with WindhawkUtils, so other mods' subclasses of the same window keep working.
constexpr UINT_PTR kTileTimerId = 0x5448;
constexpr UINT kTileTimerMs = 250;
constexpr ULONGLONG kTileActiveMs = 5000;
HWND g_tileWindow = nullptr;
std::atomic<ULONGLONG> g_tileActiveUntil{0};

// SetTimer needs the window's own thread, and some callers run on another one. In that case a private message makes
// the window's thread start (or stop) the timer itself.
UINT TileStartMessage() {
    static const UINT message = RegisterWindowMessageW(L"WindhawkStartHideEmptyCategories.TileTimerStart");
    return message;
}

UINT TileStopMessage() {
    static const UINT message = RegisterWindowMessageW(L"WindhawkStartHideEmptyCategories.TileTimerStop");
    return message;
}

void StartTileTimer(HWND hwnd) {
    if (!SetTimer(hwnd, kTileTimerId, kTileTimerMs, nullptr)) {
        Wh_Log(L"[start] Could not start the tile timer (error %lu)", GetLastError());
    }
}

// Keeps the timer running for a few more seconds.
void ArmTileTimer() {
    g_tileActiveUntil = GetTickCount64() + kTileActiveMs;
    HWND hwnd = g_tileWindow;
    if (!hwnd) return;
    if (GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId()) {
        StartTileTimer(hwnd);
    } else {
        PostMessageW(hwnd, TileStartMessage(), 0, 0);
    }
}

bool IsStartCoreWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;
    wchar_t cls[128]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

LRESULT CALLBACK TileSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (msg == WM_TIMER && wParam == kTileTimerId) {
        if (g_shuttingDown || GetTickCount64() >= g_tileActiveUntil) {
            KillTimer(hwnd, kTileTimerId);
        } else {
            UpdateCategoryTiles();  // Runs on the window's own thread, which may use the UI.
        }
        return 0;
    }
    if (msg == TileStartMessage()) {
        if (!g_shuttingDown) StartTileTimer(hwnd);
        return 0;
    }
    if (msg == TileStopMessage()) {
        KillTimer(hwnd, kTileTimerId);
        RestoreCollapsedTiles();  // On the window's own thread, which owns the tiles.
        return 0;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK AttachTileSubclass(HWND hwnd, LPARAM) {
    if (g_tileWindow || !IsStartCoreWindow(hwnd)) return TRUE;
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TileSubclassProc, 0)) g_tileWindow = hwnd;
    return TRUE;
}

void StartCategoryTileWatch() {
    if (!g_tileWindow) EnumWindows(AttachTileSubclass, 0);
    ArmTileTimer();
}

void StopCategoryTileWatch() {
    HWND hwnd = g_tileWindow;
    if (!hwnd) return;
    g_tileWindow = nullptr;
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(hwnd, TileStopMessage(), 0, 0, SMTO_ABORTIFHUNG, 2000, &ignored);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TileSubclassProc);
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

using StartFrameContextRequested_t = void(__cdecl*)(void*, void*, void*);
StartFrameContextRequested_t StartFrameContextRequested_Original = nullptr;

// Start's right-click handler runs on the UI thread, so it also starts the tile watch.
void __cdecl StartFrameContextRequested_Hook(void* self, void* sender, void* args) {
    try {
        auto const& element = *static_cast<wux::UIElement const*>(sender);
        g_startRoot = winrt::make_weak(element);
    } catch (...) {
    }
    StartCategoryTileWatch();
    StartFrameContextRequested_Original(self, sender, args);
}

using PruneAndCombine_t = void(__cdecl*)(void*, void*);
PruneAndCombine_t PruneAndCombine_Original = nullptr;

// Start rebuilds its categories when it opens, on the UI thread, so this is where the tile check
// starts without a right-click.
void __cdecl PruneAndCombine_Hook(void* self, void* map) {
    if (++g_rebuildCount == 1) {
        FILETIME created{}, exited{}, kernel{}, user{}, now{};
        GetSystemTimeAsFileTime(&now);
        if (GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
            const ULONGLONG start = (static_cast<ULONGLONG>(created.dwHighDateTime) << 32) | created.dwLowDateTime;
            const ULONGLONG current = (static_cast<ULONGLONG>(now.dwHighDateTime) << 32) | now.dwLowDateTime;
            const ULONGLONG ageMs = current > start ? (current - start) / 10000 : 0;
            g_startupCleanupAllowed = ageMs < 10000;
        }
    }
    StartCategoryTileWatch();
    PruneAndCombine_Original(self, map);
    UpdateCategoryTiles();
}

// Removing entries from Start's list from a timer right after the rebuild that happens when the menu opens crashes
// Start: it faults inside StartMenu.dll 30 to 50 ms later. Removing them inside Start's own ReplaceAll call, before it
// does anything else with the list, does not. So that is where empty entries are removed from those lists. The one
// list that never goes through such a call here is the one built when Start starts up (see ProcessCategoryGrid).

// Removes empty category groups from the list that Start has just filled, before the menu draws it.
// `abi` is the ABI pointer of the list.
void RemoveEmptyGroupsFromAbi(void* abi) {
    try {
        unsigned long long vtable = 0;
        if (!ReadWordAt(abi, 0, &vtable)) return;
        wf::IInspectable inspectable{nullptr};
        winrt::copy_from_abi(inspectable, abi);
        auto items = inspectable.try_as<IInspectableList>();
        if (!items) {
            static bool loggedNotList = false;
            if (!loggedNotList) {
                loggedNotList = true;
                Wh_Log(L"[start] ReplaceAll was given something that is not a list; the hook's layout assumption may be wrong for this build");
            }
            return;
        }
        RemoveEmptyGroups(items);
    } catch (...) {
        Wh_Log(L"[start] Removing the empty groups failed with an exception");
    }
}

// The C++ wrapper of the category list's ReplaceAll. It covers the callers that use the out-of-line copy of the
// wrapper, which is how Start fills the category list on the build this mod was tested on. `self` is the address of
// the vector wrapper, whose first member is the ABI pointer; this layout is an assumption about Start's build, so it
// is read safely.
using ReplaceAll_t = void(__cdecl*)(void*, void*);
ReplaceAll_t ReplaceAll_Original = nullptr;

void __cdecl ReplaceAll_Hook(void* self, void* items) {
    ReplaceAll_Original(self, items);
    unsigned long long abi = 0;
    if (ReadWordAt(self, 0, &abi)) RemoveEmptyGroupsFromAbi(reinterpret_cast<void*>(abi));
}

bool HookStartMenuDllSymbols(HMODULE module) {
    // StartMenu.dll, StartUI.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            // Optional. Only an extra trigger for the tile check (a right-click on the menu).
            {
                LR"(public: void __cdecl winrt::StartMenu::implementation::StartBlendedFlexFrame::OnContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))",
            },
            &StartFrameContextRequested_Original,
            StartFrameContextRequested_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::StartMenu::implementation::CategoryProvider::PruneAndCombineCategories(class std::unordered_map<enum winrt::StartMenu::AppCategory,class std::vector<struct winrt::StartMenu::IAllAppsItemViewModel,class std::allocator<struct winrt::StartMenu::IAllAppsItemViewModel> >,struct std::hash<enum winrt::StartMenu::AppCategory>,struct std::equal_to<enum winrt::StartMenu::AppCategory>,class std::allocator<struct std::pair<enum winrt::StartMenu::AppCategory const ,class std::vector<struct winrt::StartMenu::IAllAppsItemViewModel,class std::allocator<struct winrt::StartMenu::IAllAppsItemViewModel> > > > > &))",
            },
            &PruneAndCombine_Original,
            PruneAndCombine_Hook,
        },
        {
            // Optional. If a Windows update renames it, the other hook keeps working and only the gaps return.
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IObservableVector<struct winrt::StartMenu::AllAppsGroupViewModel>,struct winrt::StartMenu::AllAppsGroupViewModel>::ReplaceAll(struct winrt::array_view<struct winrt::StartMenu::AllAppsGroupViewModel const >)const )",
            },
            &ReplaceAll_Original,
            ReplaceAll_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"[start] Windows build %ls: could not hook the Start menu; empty category tiles will not be hidden",
               WindowsBuildString().c_str());
        return false;
    }
    Wh_Log(L"[start] Windows build %ls: empty-tile hooks ok", WindowsBuildString().c_str());
    if (!ReplaceAll_Original) {
        Wh_Log(L"[start] The category list's ReplaceAll was not found; empty tiles will be hidden but may leave gaps");
    }
    return true;
}

HMODULE GetStartMenuModuleHandle() {
    HMODULE module = GetModuleHandleW(L"StartMenu.dll");
    if (!module) module = GetModuleHandleW(L"StartUI.dll");
    return module;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

// Start's menu DLL may load after the mod, so watch for it (Start host only).
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_startModuleHooked && GetStartMenuModuleHandle() == module) {
        if (!g_startModuleHooked.exchange(true)) {
            if (HookStartMenuDllSymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }
    return module;
}

}  // namespace

// ---------------------------------------------------------------------------
// Wh_ModInit and friends
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    wchar_t exePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    g_isStartHost = _wcsicmp(PathFindFileNameW(exePath), L"StartMenuExperienceHost.exe") == 0;

    if (!g_isStartHost) {
        // Explorer can run folder windows in separate processes, and each of them loads the mod. Only the shell process
        // (the one that owns the desktop) does the work.
        DWORD shellProcess = 0;
        if (HWND shell = GetShellWindow()) GetWindowThreadProcessId(shell, &shellProcess);
        if (shellProcess != 0 && shellProcess != GetCurrentProcessId()) {
            Wh_Log(L"Not the shell Explorer process; the mod does nothing here");
            return FALSE;
        }

        // explorer.exe only restarts the Start menu process, so it loads with the hooks from its start. A Start menu
        // process that was already running built its first category list before any hook was in place, so its empty
        // tiles would leave a gap until the menu was opened twice. One that already has this copy of the mod is left
        // alone.
        Wh_Log(L"Restarting the Start menu process if it does not have this mod yet");
        RestartStartMenuHost(true);
        return TRUE;
    }

    Wh_Log(L"[start] Windows build %ls", WindowsBuildString().c_str());
    if (HMODULE startModule = GetStartMenuModuleHandle()) {
        g_startModuleHooked = true;
        HookStartMenuDllSymbols(startModule);
    }

    StartCategoryTileWatch();

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase
        ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"))
        : nullptr;
    if (loadLibraryExW) {
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isStartHost || g_startModuleHooked) return;

    if (HMODULE startModule = GetStartMenuModuleHandle()) {
        if (!g_startModuleHooked.exchange(true)) {
            if (HookStartMenuDllSymbols(startModule)) {
                Wh_ApplyHookOperations();
            }
        }
    }
}

void Wh_ModUninit() {
    g_shuttingDown = true;
    if (g_isStartHost) {
        StopCategoryTileWatch();
        return;  // Nothing else but the hooks lives in the Start menu process.
    }

    // Explorer: the tiles hidden so far, and the empty groups removed from Start's list, stay that way until Start
    // rebuilds them. Restarting the Start menu process brings everything back the way Windows made it.
    RestartStartMenuHost();

}
