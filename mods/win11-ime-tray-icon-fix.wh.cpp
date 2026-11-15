// ==WindhawkMod==
// @id              win11-ime-tray-icon-fix
// @name            Windows 11 IME Tray Icon Fix
// @description     Moves the input indicator to the left of the notification area so its width change cannot shift the tray icons.
// @version         1.0
// @author          yumeyao
// @github          https://github.com/yumeyao
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 IME Tray Icon Fix

## Default Stock Layout And The Bug

![Before Fix](https://raw.githubusercontent.com/yumeyao/windhawk-ime-tray-fix/refs/heads/master/doc/before-fix.gif)

```text
  Screen Left  <-----------------------------------------------------  Screen Right Edge
  [Chevron ^]  |  [SysPager (Tray Icons)]  |  [IME Indicator]  |  [Clock]  |  [Show Desktop]
```

The notification area (`TrayNotifyWnd`) is **anchored to the right edge** of the screen, while child windows are laid out left-to-right. Notice that the **IME Indicator sits right in the middle**, between the third-party tray icons (`SysPager`) and the clock.

When **"Let me set a different input method for each app window"** is enabled in a multilingual setup (e.g., English, Chinese, Japanese):
1. While typing in an app, the indicator often displays **2 glyphs** (e.g., `CH` + `中` or `JP` + `あ`), occupying a wider slot.
2. When you click a tray icon (such as Steam or WiFi), `WM_LBUTTONDOWN` immediately shifts focus to `explorer.exe`.
3. The shell defaults to English, shrinking the indicator down to **1 glyph** (`ENG`).
4. Because the clock on the right is pinned to the right edge, the shrunk width forces **everything to the left of the indicator (`SysPager` and `Chevron ^`) to slide rightward**.
5. When `WM_LBUTTONUP` fires, the icon you aimed at has moved, and your click lands on whichever neighboring icon slid under your cursor.

## The Fix

![After Fix](https://raw.githubusercontent.com/yumeyao/windhawk-ime-tray-fix/refs/heads/master/doc/after-fix.gif)

```text
[IME Indicator]  |  [Chevron ^]  |  [SysPager (Tray Icons)]  |  [Clock]  |  [Show Desktop]
```

The indicator takes the first slot and everything that used to sit left of it moves right by the indicator's width. Thus, every band is now to the right of the indicator, sits at a fixed distance from the right edge and never moves.

##### Mimicking Docked Language Bar?

Yes. But the docked language bar is broken in win10 (unstable, and won't work in remote desktop - even if you enable this option in both sides, and the latest MSPY doesn't support it unless you turn on "Use the old version of MSPY") and is completely removed in win11.

## Notes and known limitations

- This mod is separated from the Win 10 fix because the UI technology (XAML vs Win32) is completely different. So they share no common code.
- No alternative modes. You can adjust width / hide icon using the offcial (m417z) mods "Taskbar height and icon size" and "Taskbar tray system icon tweaks".
- Source code and reverse engineering credit goes to AI bot (hy4). Heavily guided to follow the framework of the official mods above, with AI-hallucinated overhead (it added a repeating timer) dropped. But not fully revised (don't want to learn XAML 🙂).
- Verified on Windows 11 24H2 (OS build 26100), where `SystemTrayFrameGrid` is a `Grid` with one Auto column per tray stack. On that template the mod rewrites `Grid.Column`.
- Some newer Windows 11 builds use a horizontal `StackPanel` named `SystemTrayFrameGrid` instead. Not tested yet.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <atomic>
#include <cwchar>
#include <list>
#include <optional>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

namespace wf = winrt::Windows::Foundation;
namespace xamlcontrols = winrt::Windows::UI::Xaml::Controls;
std::atomic<bool> g_unloading;
std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_columnsApplied;
std::atomic<int> g_originalColumn{3};
std::atomic<bool> g_panelReorderApplied;
std::atomic<uint32_t> g_originalPanelIndex{3};
std::atomic<bool> g_taskbarDllSymbolsHooked;

// Addresses looked up in taskbar.dll by symbol name. These are not hooks:
// GetTaskbarXamlRoot only calls them, on the taskbar UI thread.
void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

// Only accessed on the XAML UI thread.
std::optional<winrt::weak_ref<FrameworkElement>> g_cachedGrid;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return static_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

int ChildCount(FrameworkElement element) {
    try {
        return winrt::Windows::UI::Xaml::Media::VisualTreeHelper::
            GetChildrenCount(element);
    } catch (...) {
        return 0;
    }
}

FrameworkElement ChildAt(FrameworkElement element, int index) {
    try {
        return winrt::Windows::UI::Xaml::Media::VisualTreeHelper::
            GetChild(element, index)
                .try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int count = ChildCount(element);
    for (int i = 0; i < count; ++i) {
        FrameworkElement child = ChildAt(element, i);
        if (!child) {
            continue;
        }
        try {
            if (child.Name() == name) {
                return child;
            }
        } catch (...) {
        }
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                     PCWSTR className) {
    int count = ChildCount(element);
    for (int i = 0; i < count; ++i) {
        FrameworkElement child = ChildAt(element, i);
        if (!child) {
            continue;
        }
        try {
            if (winrt::get_class_name(child) == className) {
                return child;
            }
        } catch (...) {
        }
    }
    return nullptr;
}

// The same walk the official mods do in ApplyStyle, but starting from an
// arbitrary XamlRoot instead of from an IconView.
FrameworkElement FrameGridFromXamlRoot(XamlRoot xamlRoot) {
    if (!xamlRoot) {
        return nullptr;
    }
    try {
        FrameworkElement content =
            xamlRoot.Content().try_as<FrameworkElement>();
        if (!content) {
            return nullptr;
        }
        FrameworkElement frame = FindChildByClassName(
            content, L"SystemTray.SystemTrayFrame");
        if (!frame) {
            return nullptr;
        }
        return FindChildByName(frame, L"SystemTrayFrameGrid");
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement FindFrameGrid(FrameworkElement seed) {
    if (!seed) {
        return nullptr;
    }
    try {
        return FrameGridFromXamlRoot(seed.XamlRoot());
    } catch (...) {
        return nullptr;
    }
}

// SystemTrayFrameGrid comes in two shapes across Windows 11 builds: a Grid
// with one auto-sized column per named stack (classic template), or a
// horizontal StackPanel (newer template). Taskbar Styler themes target
// "StackPanel#SystemTrayFrameGrid, Grid#SystemTrayFrameGrid" for that reason,
// and taskbar-multi-tray swaps tray children with the same two branches.
// Grid.Column is inert on a StackPanel child and reads back as 0, so the panel
// case has to move the child inside Children() instead.
//
// In both cases: moving the language stack to the first slot and pushing the
// stacks that used to be to its left is enough. The tray is right-aligned, so
// after the move a width change of the language stack only stretches its own
// left edge.
void ApplyGridColumnReorder(FrameworkElement grid,
                            FrameworkElement languageStack,
                            bool enable) {
    int current = xamlcontrols::Grid::GetColumn(languageStack);
    int count = ChildCount(grid);

    if (enable) {
        // Already first: do nothing and do not claim we changed it. A theme
        // can legitimately put the stack at column 0 on its own.
        if (current == 0) {
            return;
        }
        if (!g_columnsApplied.load()) {
            g_originalColumn = current;
        }

        // Collect first, then write. Changing a column changes the layout, so
        // reading columns while writing them would be unreliable.
        std::vector<std::pair<FrameworkElement, int>> toShift;
        for (int i = 0; i < count; ++i) {
            FrameworkElement child = ChildAt(grid, i);
            if (!child) {
                continue;
            }
            int column = xamlcontrols::Grid::GetColumn(child);
            if (column >= 0 && column < current) {
                toShift.emplace_back(child, column);
            }
        }
        for (auto& item : toShift) {
            xamlcontrols::Grid::SetColumn(item.first, item.second + 1);
        }
        xamlcontrols::Grid::SetColumn(languageStack, 0);
        g_columnsApplied = true;
        Wh_Log(L"Moved NonActivatableStack from column %d to 0", current);
    } else {
        // Only undo a move we actually made, so a theme that already places
        // the stack first is not rearranged on unload.
        if (!g_columnsApplied.load()) {
            return;
        }
        if (current != 0) {
            g_columnsApplied = false;
            return;
        }
        int target = g_originalColumn.load();
        if (target <= 0) {
            target = 3;
        }

        std::vector<std::pair<FrameworkElement, int>> toShift;
        for (int i = 0; i < count; ++i) {
            FrameworkElement child = ChildAt(grid, i);
            if (!child) {
                continue;
            }
            int column = xamlcontrols::Grid::GetColumn(child);
            if (column >= 1 && column <= target) {
                toShift.emplace_back(child, column);
            }
        }
        for (auto& item : toShift) {
            xamlcontrols::Grid::SetColumn(item.first, item.second - 1);
        }
        xamlcontrols::Grid::SetColumn(languageStack, target);
        g_columnsApplied = false;
        Wh_Log(L"Restored NonActivatableStack to column %d", target);
    }
}

// StackPanel (and any other Panel) template: order is the child index, not an
// attached property. Move() shifts every child between the two indices, so
// moving the language stack to 0 pushes the stacks that were left of it one
// slot to the right, exactly like the column shift above. The runtime index is
// saved because Taskbar Styler themes and other mods can change it too.
void ApplyPanelChildReorder(xamlcontrols::Panel panel,
                            FrameworkElement languageStack,
                            bool enable) {
    auto children = panel.Children();
    uint32_t index = 0;
    if (!children.IndexOf(languageStack, index)) {
        Wh_Log(L"NonActivatableStack is not a child of SystemTrayFrameGrid");
        return;
    }
    uint32_t size = children.Size();

    if (enable) {
        if (index == 0) {
            return;
        }
        if (!g_panelReorderApplied.load()) {
            g_originalPanelIndex = index;
        }
        children.Move(index, 0);
        g_panelReorderApplied = true;
        Wh_Log(L"Moved NonActivatableStack from child index %u to 0 (Panel)",
               index);
    } else {
        if (!g_panelReorderApplied.load()) {
            return;
        }
        if (index != 0) {
            g_panelReorderApplied = false;
            return;
        }
        uint32_t target = g_originalPanelIndex.load();
        if (target == 0 || target >= size) {
            // Template order: NotifyIconStack, NotificationAreaIcons,
            // MainStack, NonActivatableStack, ...
            target = 3;
            if (target >= size) {
                target = size - 1;
            }
        }
        children.Move(0, target);
        g_panelReorderApplied = false;
        Wh_Log(L"Restored NonActivatableStack to child index %u", target);
    }
}

void ApplyColumnReorder(FrameworkElement grid, bool enable) {
    FrameworkElement languageStack =
        FindChildByName(grid, L"NonActivatableStack");
    if (!languageStack) {
        Wh_Log(L"NonActivatableStack not found in SystemTrayFrameGrid");
        return;
    }

    // Grid derives from Panel, so the Grid check must come first.
    if (grid.try_as<xamlcontrols::Grid>()) {
        ApplyGridColumnReorder(grid, languageStack, enable);
    } else if (auto panel = grid.try_as<xamlcontrols::Panel>()) {
        ApplyPanelChildReorder(panel, languageStack, enable);
    } else {
        Wh_Log(L"SystemTrayFrameGrid is neither a Grid nor a Panel");
    }
}

void ApplyAll(FrameworkElement grid, bool restoring = false) {
    if (!grid || (!restoring && g_unloading.load())) {
        return;
    }
    try {
        ApplyColumnReorder(grid, !restoring);
    } catch (...) {
        Wh_Log(L"ApplyAll threw an exception");
    }
}

// Copied from taskbar-tray-system-icon-tweaks (m417z), GPLv3. Unlike the
// official mod this one does not need taskbar.dll symbols to find the taskbar
// window: Shell_TrayWnd is enough to marshal a call onto the XAML UI thread.
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }
    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

HWND GetTaskbarWnd() {
    // Match the official mods: Shell_TrayWnd must belong to this process.
    // FindWindowW can return a window from another explorer instance.
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

// Copied from taskbar-tray-system-icon-tweaks (m417z), GPLv3, and
// taskbar-notification-icon-spacing. This reaches the XAML root of the tray
// that already exists, without waiting for a new IconView to be constructed.
// All four addresses come from PDB symbol names. The field offset below is
// read from the instruction stream of TaskbarHost::FrameHeight, not from a
// hardcoded RVA, so Windows updates that move code around remain safe.
XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!g_taskbarDllSymbolsHooked.load() ||
        !CTaskBand_ITaskListWndSite_vftable ||
        !CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        return nullptr;
    }

    HWND hTaskSwWnd =
        reinterpret_cast<HWND>(GetProp(hTaskbarWnd, L"TaskbandHWND"));
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtr(hTaskSwWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *static_cast<void**>(taskBandForTaskListWndSite) !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite =
            static_cast<void**>(taskBandForTaskListWndSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b =
            static_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }

    auto* taskbarElementIUnknown = *reinterpret_cast<IUnknown**>(
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
        taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));

    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols for taskbar.dll FAILED");
        return false;
    }
    g_taskbarDllSymbolsHooked = true;
    Wh_Log(L"taskbar.dll symbols OK");
    return true;
}

// All XAML work, including destroying event revokers, has to run on the
// taskbar UI thread. Both official SystemTray mods follow this pattern:
// Wh_ModBeforeUninit only sets g_unloading and calls ApplySettings, which
// marshals the cleanup onto the UI thread with RunFromWindowThread.
struct ApplySettingsParam {
    bool unloading;
    HWND hTaskbarWnd;
};

void WINAPI ApplySettingsOnUiThread(void* pParam) {
    ApplySettingsParam& param = *static_cast<ApplySettingsParam*>(pParam);
    bool unloading = param.unloading;

    // Both official mods clear the Loaded revoker list on every ApplySettings,
    // then re-apply to the existing tree wholesale. Doing the same here keeps
    // the event paths identical and avoids stale one-shot subscriptions.
    g_autoRevokerList->clear();

    // Primary path, copied from the official SystemTray mods: walk from
    // Shell_TrayWnd to the XAML root. This works even when the tray was built
    // long before the mod was enabled and no IconView::Loaded will ever fire.
    FrameworkElement grid =
        FrameGridFromXamlRoot(GetTaskbarXamlRoot(param.hTaskbarWnd));
    bool fromXamlRoot = grid != nullptr;
    if (!grid && g_cachedGrid) {
        try {
            grid = g_cachedGrid->get();
        } catch (...) {
        }
    }

    Wh_Log(L"ApplySettings: unloading=%d xamlRootGrid=%d gridCached=%d",
           static_cast<int>(unloading), static_cast<int>(fromXamlRoot),
           static_cast<int>(grid != nullptr));

    if (grid) {
        g_cachedGrid = winrt::make_weak(grid);
    }

    if (unloading) {
        if (grid) {
            ApplyAll(grid, true);
        }
        g_cachedGrid.reset();
        Wh_Log(L"Restored on the UI thread");
        return;
    }

    if (grid) {
        ApplyAll(grid);
    }
}

void ApplySettings(bool unloading) {
    HWND taskbar = GetTaskbarWnd();
    if (!taskbar) {
        Wh_Log(L"No taskbar window found");
        return;
    }
    ApplySettingsParam param{unloading, taskbar};
    if (!RunFromWindowThread(taskbar, ApplySettingsOnUiThread, &param)) {
        // Do not fall back to clearing the revokers on this thread. A failed
        // marshal only leaks XAML subscriptions until explorer rebuilds the
        // tray; trying to clean them up off-thread is what used to crash.
        Wh_Log(L"RunFromWindowThread failed, unloading=%d",
               static_cast<int>(unloading));
    }
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);
    if (g_unloading.load()) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](wf::IInspectable const& sender,
                        RoutedEventArgs const& /*e*/) {
            // The unload path clears the whole revoker list on the UI thread.
            // Bail out before touching a possibly erased iterator.
            if (g_unloading.load()) {
                return;
            }
            g_autoRevokerList->erase(autoRevokerIt);
            try {
                auto element = sender.try_as<FrameworkElement>();
                if (!element) {
                    return;
                }
                FrameworkElement grid = FindFrameGrid(element);
                if (!grid) {
                    return;
                }

                // Grid.Column is a static XAML property. It only needs to be
                // written again if explorer rebuilt the frame grid.
                FrameworkElement cached = nullptr;
                if (g_cachedGrid) {
                    try {
                        cached = g_cachedGrid->get();
                    } catch (...) {
                    }
                }
                if (cached != grid) {
                    g_cachedGrid = winrt::make_weak(grid);
                    ApplyColumnReorder(grid, true);
                }
            } catch (...) {
                Wh_Log(L"Loaded handler threw an exception");
            }
        });

    return ret;
}

bool HookSystemTraySymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols FAILED");
        return false;
    }
    Wh_Log(L"HookSymbols OK");
    return true;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    // LoadLibraryExW is a very hot hook. Do not install new symbol hooks or
    // call back into the engine while the mod is being unloaded.
    if (g_unloading.load()) {
        return;
    }
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookSystemTraySymbols(module)) {
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
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    // Not fatal. If taskbar.dll symbols are missing on a future build, the
    // mod still works through the IconView::Loaded fallback, just without an
    // immediate apply on enable.
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"XamlRoot lookup unavailable, using Loaded fallback");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module after init");
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings(false);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings(true);
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    ApplySettings(false);
}
