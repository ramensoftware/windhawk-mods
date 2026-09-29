// ==WindhawkMod==
// @id              explorer-properties-button
// @name            Explorer Properties Button
// @name:zh-CN      资源管理器属性按钮
// @description     Add a Properties button to the Windows 11 File Explorer command bar, between Delete and Sort, opening the properties dialog of the current selection
// @description:zh-CN 在 Windows 11 文件资源管理器命令栏（"删除"与"排序"按钮之间）添加属性按钮，点击打开当前选中项目的属性对话框
// @version         1.0.0
// @author          AnnAngela
// @github          https://github.com/AnnAngela
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Properties Button

![Explorer Properties Button](https://raw.githubusercontent.com/AnnAngela/windhawk-mods/main/explorer-properties-button/screenshot.png)

Adds a **Properties** button to the Windows 11 File Explorer command bar,
between the **Delete** and **Sort** buttons. Clicking it opens the properties
dialog of the current selection:

- **Multiple selection** - opens the combined properties dialog, just like
  the context menu's Properties entry.
- **Virtual locations** - This PC, Recycle Bin, Quick Access and other items
  without a filesystem path work too.
- **No selection** (configurable) - shows the properties of the current
  folder, or does nothing.

The dialog opens via fast shell APIs (SHObjectProperties /
SHMultiFileProperties) without building the shell context menu, so it appears
without the delay that loading shell extensions would add. The context-menu
properties verb is only used as a fallback for exotic locations.

# System requirements

Windows 11 24H2 / 25H2 or newer, with the WinAppSDK (WinUI 3) File Explorer
command bar. Older versions (Windows 10, Windows 11 23H2 and earlier) use a
completely different command bar; the mod stays silently disabled there.

# How it works

The mod hooks a couple of functions of File Explorer's own WinUI 3 code
(FileExplorerExtensions.dll), gets the command bar from there, and inserts a
WinRT-constructed AppBarButton into its PrimaryCommands. The button survives
tab switches, navigation and new windows, and is cleanly removed when the mod
is disabled. Clicks are handled on a worker thread, so the Explorer UI never
blocks.

Note: close open properties dialogs before disabling the mod - unloading
waits for the worker thread that shows the dialog.

Implementation follows the Explorer Command Bar mod by DanRotaru from the
official windhawk-mods repository.

---

# 资源管理器属性按钮（中文说明）

在 Windows 11 文件资源管理器的命令栏（工具栏）上，"删除"和"排序"按钮之间
添加一个"属性"按钮，点击后打开当前选中项目的属性对话框：

- 支持多选：多选时与右键菜单里的"属性"一样，打开合并的属性对话框。
- 支持虚拟位置：此电脑、回收站、Quick Access 等没有文件系统路径的项目
  也能正确打开属性。
- 没有选中项时（可配置）：显示当前文件夹的属性，或什么都不做。

属性对话框通过快速 Shell API（SHObjectProperties / SHMultiFileProperties）
直接打开，不构建右键菜单，避免了加载 shell 扩展带来的延迟；右键菜单的
"属性"动词仅作为特殊位置的兜底。

# 系统要求

适用于 Windows 11 24H2 / 25H2 及以上使用 WinAppSDK (WinUI 3) 命令栏的
文件资源管理器。旧版本（Windows 10、Windows 11 23H2 及更早）使用完全
不同的命令栏实现，本 mod 会在其中安静地保持不启用状态。

# 工作原理

本 mod hook 了文件资源管理器自身的 WinUI 3 代码（FileExplorerExtensions.dll）
中的若干函数，从那里拿到命令栏，并把一个用 WinRT 构造的 AppBarButton 插入
PrimaryCommands；按钮在切换标签页、导航、新窗口后自动恢复，禁用 mod 时
被干净移除。点击在独立工作线程上处理，不会阻塞资源管理器 UI 线程。

注意：禁用 mod 时请先关闭已打开的属性对话框——卸载流程会等待正在显示
属性对话框的工作线程结束。

实现参考了官方仓库的 Explorer Command Bar mod（DanRotaru），在此致谢。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- buttonLabel: "属性"
  $name: Button label
  $name:zh-CN: 按钮文字
  $description: >-
    The tooltip shown when hovering the button; with "Show label" enabled, it
    is also displayed next to the icon.
  $description:zh-CN: >-
    鼠标悬停在按钮上时显示的提示文字；勾选"显示文字"后也会显示在图标旁。
- showLabel: false
  $name: Show label
  $name:zh-CN: 显示文字
  $description: >-
    Show the label text next to the icon. When disabled, only the icon is
    shown, matching the neighboring Delete button.
  $description:zh-CN: >-
    在图标旁显示按钮文字。不勾选则仅显示图标，与相邻的"删除"按钮一致。
- iconGlyph: "E946"
  $name: Icon glyph
  $name:zh-CN: 图标字形
  $description: >-
    Hex code of a Segoe Fluent Icons glyph. Default: E946 (circled i, matching
    the properties dialog icon).
  $description:zh-CN: >-
    Segoe Fluent Icons 字形的十六进制编码。默认 E946（圆圈 i，与属性对话框
    的图标一致）。
- noSelectionBehavior: folder
  $name: Behavior with no selection
  $name:zh-CN: 无选中项时的行为
  $description: >-
    What the button does when nothing is selected.
  $description:zh-CN: >-
    没有选中任何项目时按钮的行为。
  $options:
    - folder: Show properties of the current folder
    - none: Do nothing
  $options:zh-CN:
    - folder: 显示当前文件夹的属性
    - none: 不执行任何操作
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

#include <exdisp.h>
#include <servprov.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// clang-format off

#pragma region winrt_hpp

#include <Unknwn.h>

// Conflicts with a winrt method of the same name.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;

#pragma endregion  // winrt_hpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

// Our button is recognized by its name, so it's only ever inserted once and is
// found again on removal.
constexpr PCWSTR kPropertiesButtonName = L"WindhawkPropertiesButton";

// The primary command bar is where all the visible buttons live; the secondary
// one only holds the Details pane toggle.
constexpr std::wstring_view kPrimaryCommandBarName = L"FileExplorerCommandBar";
constexpr std::wstring_view kSecondaryCommandBarName =
    L"FileExplorerSecondaryCommandBar";

// Built-in buttons we anchor to, identified by their SVG icon file name, which
// is stable across languages. The delete button's icon is where our button
// goes: right after it, before Sort.
constexpr std::wstring_view kDeleteButtonSvg = L"windows.ribbondelete.svg";
constexpr std::wstring_view kSortButtonSvg = L"sortby.svg";
constexpr wchar_t kSortButtonAutomationId[] = L"SortAndGroupButton";

// {9BA05972-F6A8-11CF-A442-00A0C90A8F39} CLSID_ShellWindows
constexpr GUID kCLSID_ShellWindows = {
    0x9BA05972, 0xF6A8, 0x11CF,
    {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};

// {4C96BE40-915C-11CF-99D3-00AA004AE837} SID_STopLevelBrowser
constexpr GUID kSID_STopLevelBrowser = {
    0x4C96BE40, 0x915C, 0x11CF,
    {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

struct {
    std::mutex mutex;
    std::wstring buttonLabel = L"属性";
    bool showLabel = false;
    std::wstring iconGlyph = L"E946";
    bool noSelectionFolder = true;  // folder = true, none = false.
} g_settings;

// Set as early as possible before unload, so the XAML delegates - which stay
// live until unregistered - stop acting on behalf of a dying mod.
std::atomic<bool> g_unloading;

std::atomic<bool> g_symbolsHooked;

std::wstring ToLower(std::wstring str) {
    for (auto& c : str) {
        c = towlower(c);
    }

    return str;
}

////////////////////////////////////////////////////////////////////////////////
// Per-UI-thread state. Every XAML object in it belongs to the thread that
// created it: a weak reference to a live non-agile XAML element can only be
// resolved from its own thread, and its event handlers can only be
// unregistered there. Wh_ModUninit and Wh_ModSettingsChanged reach every
// thread through RunFromWindowThread.

struct CommandBarEntry {
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::event_token vectorChangedToken{};
};

thread_local std::vector<CommandBarEntry> g_entries;

// Set once a scan of this thread's XAML island found its command bars, so the
// focus hook can skip the tree walk while they're still around.
thread_local bool g_threadScanned;

// Every event handler the mod registers on the elements it creates has to be
// revoked before the mod is unloaded: the delegate object lives in this DLL,
// and XAML releasing it after the module was unmapped would crash Explorer.
// Handlers are registered with winrt::auto_revoke and the revokers are kept
// here until the buttons are taken down.
struct TrackedRevoker {
    winrt::weak_ref<wf::IInspectable> source;
    std::function<void()> revoke;
};

thread_local std::vector<TrackedRevoker> g_revokers;

template <typename T, typename Revoker>
void TrackRevoker(T const& source, Revoker&& revoker) {
    auto held =
        std::make_shared<std::decay_t<Revoker>>(std::forward<Revoker>(revoker));
    g_revokers.push_back({winrt::weak_ref<wf::IInspectable>{source},
                          [held]() { held->revoke(); }});
}

void RevokeHandlersForCurrentThread() {
    std::vector<TrackedRevoker> taken;
    taken.swap(g_revokers);

    for (auto const& tracked : taken) {
        try {
            tracked.revoke();
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

// Command bar updates deferred out of the PrimaryCommands change notification
// (mutating the vector from within its own notification isn't allowed), keyed
// by address and coalesced: our own insertions raise one notification each.
// The queued work always removes its own entry, so an entry can't outlive the
// command bar it belongs to and be matched against a later one at the same
// address.
thread_local std::unordered_set<void*> g_pendingUpdates;

////////////////////////////////////////////////////////////////////////////////
// Worker threads for shell work. Everything which must not happen on the
// Explorer UI thread - building a context menu can load every registered shell
// extension, and the properties dialog runs a modal loop - happens on a thread
// of its own. The shell objects involved are owned by the Explorer UI thread,
// so the calls marshal back to it; only the waiting happens elsewhere.

std::mutex g_launchThreadsMutex;
std::vector<HANDLE> g_launchThreads;

void TrackLaunchThread(HANDLE thread) {
    std::lock_guard<std::mutex> lock(g_launchThreadsMutex);

    // Opportunistic cleanup of finished threads.
    std::erase_if(g_launchThreads, [](HANDLE thread) {
        return WaitForSingleObject(thread, 0) == WAIT_OBJECT_0;
    });

    g_launchThreads.push_back(thread);
}

void WaitForLaunchThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        threads.swap(g_launchThreads);
    }

    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

// Initializes the calling thread's apartment for as long as the scope lives.
// The pairing is what matters: every CoInitializeEx which succeeded has to be
// undone by a CoUninitialize, and that includes S_FALSE. Only RPC_E_CHANGED_MODE
// leaves nothing to undo. COM objects created inside the scope have to be
// released before it ends, which declaring them after it takes care of.
class ComApartmentScope {
   public:
    explicit ComApartmentScope(DWORD coInit)
        : m_hr(CoInitializeEx(nullptr, coInit)) {}

    ~ComApartmentScope() {
        if (SUCCEEDED(m_hr)) {
            CoUninitialize();
        }
    }

    ComApartmentScope(const ComApartmentScope&) = delete;
    ComApartmentScope& operator=(const ComApartmentScope&) = delete;

   private:
    HRESULT m_hr;
};

void RunShellWorkOnWorkerThread(std::function<void()> work) {
    auto params = std::make_unique<std::function<void()>>(std::move(work));
    LPVOID param = params.release();  // Owned by the thread, reclaimed below
                                      // if the thread never came to be.

    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            std::unique_ptr<std::function<void()>> work(
                reinterpret_cast<std::function<void()>*>(lpParam));

            ComApartmentScope comScope(COINIT_APARTMENTTHREADED |
                                       COINIT_DISABLE_OLE1DDE);
            try {
                (*work)();
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }

            return 0;
        },
        param, 0, nullptr);
    if (thread) {
        // The handle is closed once the thread finished, either here on a
        // later call or in Wh_ModUninit, which waits for it.
        TrackLaunchThread(thread);
    } else {
        delete reinterpret_cast<std::function<void()>*>(param);
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Shell: finding the active tab's shell view, and opening the properties
// dialog of its selection.

// The shell view of the given File Explorer window's active tab, or, when
// hExplorerWnd is null, of whichever Explorer window is around.
winrt::com_ptr<IShellView> GetActiveShellView(HWND hExplorerWnd) {
    winrt::com_ptr<IShellWindows> shellWindows;
    HRESULT hr = CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(shellWindows.put()));
    if (FAILED(hr) || !shellWindows) {
        Wh_Log(L"CoCreateInstance(ShellWindows) failed: %08X", hr);
        return nullptr;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    // The active tab's ShellTabWindowClass window is the first one in the
    // Z-order of the CabinetWClass window's children.
    HWND hActiveTabWnd =
        hExplorerWnd ? FindWindowExW(hExplorerWnd, nullptr,
                                     L"ShellTabWindowClass", nullptr)
                     : nullptr;

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(index, dispatch.put())) || !dispatch) {
            continue;
        }

        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        if (!webBrowser) {
            continue;
        }

        SHANDLE_PTR hWndRaw = 0;
        if (FAILED(webBrowser->get_HWND(&hWndRaw))) {
            continue;
        }

        if (hExplorerWnd && (HWND)hWndRaw != hExplorerWnd) {
            continue;
        }

        auto serviceProvider = dispatch.try_as<IServiceProvider>();
        if (!serviceProvider) {
            continue;
        }

        winrt::com_ptr<IShellBrowser> shellBrowser;
        if (FAILED(serviceProvider->QueryService(
                kSID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))) ||
            !shellBrowser) {
            continue;
        }

        // With tabs, every tab is a separate ShellWindows entry with the same
        // top-level window. Skip the entries of inactive tabs.
        HWND hTabWnd = nullptr;
        if (SUCCEEDED(shellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
            if (hActiveTabWnd) {
                if (hTabWnd != hActiveTabWnd) {
                    continue;
                }
            } else if (!IsWindowVisible(hTabWnd)) {
                continue;
            }
        }

        winrt::com_ptr<IShellView> shellView;
        if (SUCCEEDED(shellBrowser->QueryActiveShellView(shellView.put())) &&
            shellView) {
            return shellView;
        }
    }

    return nullptr;
}

bool ShellViewHasSelection(winrt::com_ptr<IShellView> const& shellView) {
    auto folderView = shellView.try_as<IFolderView>();
    if (!folderView) {
        return false;
    }

    int count = 0;
    return SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &count)) &&
           count > 0;
}

// The filesystem path of the browsed folder, if any, used by the no-selection
// fast path below.
std::wstring GetFolderPath(winrt::com_ptr<IShellView> const& shellView) {
    auto folderView = shellView.try_as<IFolderView>();
    if (!folderView) {
        return std::wstring();
    }

    winrt::com_ptr<IPersistFolder2> persistFolder;
    LPITEMIDLIST pidl = nullptr;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) ||
        !persistFolder || FAILED(persistFolder->GetCurFolder(&pidl)) || !pidl) {
        return std::wstring();
    }

    WCHAR path[MAX_PATH];
    BOOL ok = SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path), GPFIDL_DEFAULT);
    CoTaskMemFree(pidl);

    return ok ? path : std::wstring();
}

void OpenPropertiesForWindow(HWND hExplorerWnd) {
    if (g_unloading) {
        return;
    }

    // Elapsed-since-entry for the log lines below. The properties APIs are
    // modal, so "returned" includes however long the dialog stayed open;
    // "invoked" is the click-to-open measurement. Steady clock so the
    // numbers aren't affected by wall-clock changes.
    auto startTime = std::chrono::steady_clock::now();
    auto elapsedMs = [startTime]() {
        return (int)std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now() - startTime)
                   .count();
    };

    bool noSelectionFolder;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        noSelectionFolder = g_settings.noSelectionFolder;
    }

    auto shellView = GetActiveShellView(hExplorerWnd);
    if (!shellView) {
        Wh_Log(L"No shell view for window %08X", (DWORD)(ULONG_PTR)hExplorerWnd);
        return;
    }

    // With a selection, it's the selection's properties; without one, the
    // current folder's - exactly what the context menu's Properties entry
    // shows for a selection vs. for the folder background.
    bool hasSelection = ShellViewHasSelection(shellView);
    Wh_Log(L"Selection: %s (%d ms)", hasSelection ? L"yes" : L"no",
           elapsedMs());
    if (!hasSelection && !noSelectionFolder) {
        Wh_Log(L"No selection, doing nothing per settings");
        return;
    }

    // Fast paths first. Building the shell context menu - what this used to
    // do - constructs and initializes every registered context-menu shell
    // extension, which can take a second or more on a loaded system and was
    // most of the delay the button used to have. SHObjectProperties and
    // SHMultiFileProperties open the very same dialogs without it.
    if (hasSelection) {
        // A single filesystem item goes straight to its property sheet.
        winrt::com_ptr<IShellItemArray> selection;
        if (SUCCEEDED(shellView->GetItemObject(
                SVGIO_SELECTION, IID_PPV_ARGS(selection.put()))) &&
            selection) {
            DWORD count = 0;
            winrt::com_ptr<IShellItem> shellItem;
            if (SUCCEEDED(selection->GetCount(&count)) && count == 1 &&
                SUCCEEDED(selection->GetItemAt(0, shellItem.put())) &&
                shellItem) {
                PWSTR path = nullptr;
                if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_FILESYSPATH,
                                                        &path)) &&
                    path) {
                    int invokedAtMs = elapsedMs();
                    BOOL opened = SHObjectProperties(hExplorerWnd,
                                                     SHOP_FILEPATH, path,
                                                     nullptr);
                    Wh_Log(L"SHObjectProperties(%s): %u (invoked +%d ms, "
                           L"returned +%d ms)",
                           path, opened, invokedAtMs, elapsedMs());
                    CoTaskMemFree(path);
                    if (opened) {
                        return;
                    }
                }
            }
        }

        // Multiple selection - the combined properties dialog - or a single
        // virtual item: straight from the selection's data object.
        winrt::com_ptr<IDataObject> dataObject;
        HRESULT hr = shellView->GetItemObject(
            SVGIO_SELECTION, IID_PPV_ARGS(dataObject.put()));
        if (SUCCEEDED(hr) && dataObject) {
            int invokedAtMs = elapsedMs();
            hr = SHMultiFileProperties(dataObject.get(), 0);
            Wh_Log(L"SHMultiFileProperties: %08X (invoked +%d ms, "
                   L"returned +%d ms)",
                   hr, invokedAtMs, elapsedMs());
            if (SUCCEEDED(hr)) {
                return;
            }
        } else {
            Wh_Log(L"GetItemObject(SVGIO_SELECTION, IDataObject) failed: %08X",
                   hr);
        }
    } else {
        // The current folder: a filesystem folder goes straight to its
        // property sheet.
        std::wstring folderPath = GetFolderPath(shellView);
        if (!folderPath.empty()) {
            int invokedAtMs = elapsedMs();
            BOOL opened = SHObjectProperties(hExplorerWnd, SHOP_FILEPATH,
                                             folderPath.c_str(), nullptr);
            Wh_Log(L"SHObjectProperties(%s): %u (invoked +%d ms, "
                   L"returned +%d ms)",
                   folderPath.c_str(), opened, invokedAtMs, elapsedMs());
            if (opened) {
                return;
            }
        }
    }

    // Slow fallback, for virtual locations only (This PC, Recycle Bin, ...):
    // the "properties" verb on the shell context menu of the selection or of
    // the folder background. Building the menu loads every registered shell
    // extension, which is why it comes last.
    UINT viewObject = hasSelection ? SVGIO_SELECTION : SVGIO_BACKGROUND;

    winrt::com_ptr<IContextMenu> contextMenu;
    HRESULT hr = shellView->GetItemObject(viewObject, __uuidof(IContextMenu),
                                          contextMenu.put_void());
    if (SUCCEEDED(hr) && contextMenu) {
        HMENU hMenu = CreatePopupMenu();
        if (hMenu) {
            // Some context menu handlers only accept InvokeCommand after
            // QueryContextMenu initialized them.
            if (SUCCEEDED(contextMenu->QueryContextMenu(
                    hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {
                CMINVOKECOMMANDINFOEX info{};
                info.cbSize = sizeof(info);
                info.fMask = CMIC_MASK_UNICODE;
                info.hwnd = hExplorerWnd;
                info.lpVerb = "properties";
                info.lpVerbW = L"properties";
                info.nShow = SW_SHOWNORMAL;
                int invokedAtMs = elapsedMs();
                hr = contextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&info);
                Wh_Log(L"InvokeCommand(properties): %08X (invoked +%d ms, "
                       L"returned +%d ms)",
                       hr, invokedAtMs, elapsedMs());
                if (SUCCEEDED(hr)) {
                    DestroyMenu(hMenu);
                    return;
                }
            } else {
                Wh_Log(L"QueryContextMenu failed: %08X", hr);
            }

            DestroyMenu(hMenu);
        }
    } else {
        Wh_Log(L"GetItemObject(%u, IContextMenu) failed: %08X", viewObject, hr);
    }
}

////////////////////////////////////////////////////////////////////////////////
// XAML: the button itself.

// The File Explorer window an element of ours belongs to.
HWND GetExplorerWindowForElement(mux::FrameworkElement const& element) {
    HWND hWnd = nullptr;

    try {
        if (auto xamlRoot = element.XamlRoot()) {
            if (auto environment = xamlRoot.ContentIslandEnvironment()) {
                hWnd = (HWND)(uintptr_t)environment.AppWindowId().Value;
            }
        }
    } catch (...) {
        Wh_Log(L"Failed to get window from XamlRoot: %08X",
               winrt::to_hresult().value);
    }

    if (!hWnd) {
        hWnd = GetActiveWindow();
    }

    if (!hWnd) {
        // Last resort, and the only candidate which isn't ours by
        // construction: the foreground window can belong to another process.
        HWND hForegroundWnd = GetForegroundWindow();
        DWORD processId = 0;
        if (hForegroundWnd &&
            GetWindowThreadProcessId(hForegroundWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            hWnd = hForegroundWnd;
        }
    }

    if (hWnd) {
        hWnd = GetAncestor(hWnd, GA_ROOT);
    }

    return hWnd;
}

void OnPropertiesInvoked(mux::FrameworkElement const& element) {
    if (g_unloading) {
        return;
    }

    HWND hWnd = GetExplorerWindowForElement(element);
    Wh_Log(L"Properties button clicked, window %08X", (DWORD)(ULONG_PTR)hWnd);

    // Off the UI thread: the context menu construction and the modal
    // properties dialog must not block the Explorer UI thread.
    RunShellWorkOnWorkerThread(
        [hWnd]() { OpenPropertiesForWindow(hWnd); });
}

muxc::AppBarButton CreatePropertiesButton() {
    std::wstring label;
    std::wstring glyph;
    bool showLabel;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        label = g_settings.buttonLabel;
        glyph = g_settings.iconGlyph;
        showLabel = g_settings.showLabel;
    }

    muxc::AppBarButton button;
    button.Name(kPropertiesButtonName);
    button.Label(label.c_str());
    if (!showLabel) {
        // Icon-only, matching the neighboring built-in buttons.
        button.LabelPosition(muxc::CommandBarLabelPosition::Collapsed);
    }

    wchar_t glyphChar = (wchar_t)wcstoul(glyph.c_str(), nullptr, 16);
    if (glyphChar) {
        muxc::FontIcon icon;
        icon.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
        std::wstring glyphString(1, glyphChar);
        icon.Glyph(glyphString.c_str());
        button.Icon(icon);
    }

    if (!label.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{label}));
    }

    TrackRevoker(
        button,
        button.Click(winrt::auto_revoke,
                     [](wf::IInspectable const& sender,
                        mux::RoutedEventArgs const&) {
                         if (auto element =
                                 sender.try_as<mux::FrameworkElement>()) {
                             OnPropertiesInvoked(element);
                         }
                     }));

    return button;
}

bool IsOurElement(muxc::ICommandBarElement const& command) {
    auto element = command.try_as<mux::FrameworkElement>();
    if (!element) {
        return false;
    }

    return element.Name() == kPropertiesButtonName;
}

std::wstring GetButtonIconUri(muxc::AppBarButton const& button) try {
    auto icon = button.Icon();
    if (!icon) {
        return std::wstring();
    }

    wf::Uri uri{nullptr};
    if (auto imageIcon = icon.try_as<muxc::ImageIcon>()) {
        if (auto source = imageIcon.Source()) {
            if (auto svgSource = source.try_as<muxm::Imaging::SvgImageSource>()) {
                uri = svgSource.UriSource();
            }
        }
    } else if (auto iconSourceElement = icon.try_as<muxc::IconSourceElement>()) {
        if (auto iconSource = iconSourceElement.IconSource()) {
            if (auto imageIconSource =
                    iconSource.try_as<muxc::ImageIconSource>()) {
                if (auto source = imageIconSource.ImageSource()) {
                    if (auto svgSource =
                            source.try_as<muxm::Imaging::SvgImageSource>()) {
                        uri = svgSource.UriSource();
                    }
                }
            }
        }
    } else if (auto bitmapIcon = icon.try_as<muxc::BitmapIcon>()) {
        uri = bitmapIcon.UriSource();
    }

    if (!uri) {
        return std::wstring();
    }

    return std::wstring{uri.AbsoluteUri()};
} catch (...) {
    return std::wstring();
}

enum class AnchorButton {
    None,
    Delete,
    Sort,
};

// Which built-in button this is, identified by SVG icon file name (stable
// across languages), with the automation id as a second chance for Sort.
AnchorButton IdentifyDefaultButton(muxc::AppBarButton const& button) {
    std::wstring uri = ToLower(GetButtonIconUri(button));
    if (!uri.empty()) {
        size_t slash = uri.find_last_of(L'/');
        std::wstring_view fileName =
            slash == std::wstring::npos
                ? std::wstring_view(uri)
                : std::wstring_view(uri).substr(slash + 1);

        if (fileName == kDeleteButtonSvg) {
            return AnchorButton::Delete;
        }
        if (fileName == kSortButtonSvg) {
            return AnchorButton::Sort;
        }
    }

    try {
        if (mux::Automation::AutomationProperties::GetAutomationId(button) ==
            kSortButtonAutomationId) {
            return AnchorButton::Sort;
        }
    } catch (...) {
    }

    return AnchorButton::None;
}

////////////////////////////////////////////////////////////////////////////////
// XAML: putting the button where it belongs, and keeping it there.

void UpdateCommandBar(muxc::CommandBar const& commandBar) {
    if (g_unloading) {
        return;
    }

    // Our button only goes in the primary command bar.
    if (commandBar.Name() != kPrimaryCommandBarName) {
        return;
    }

    for (auto const& command : commandBar.PrimaryCommands()) {
        if (IsOurElement(command)) {
            return;  // Already there.
        }
    }

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    // The button goes right after Delete, i.e. between Delete and Sort. If
    // this build has no Delete button, before Sort is the closest spot.
    uint32_t insertIndex = count;
    for (uint32_t i = 0; i < count; i++) {
        auto command = commands.GetAt(i);
        if (IsOurElement(command)) {
            continue;
        }

        auto button = command.try_as<muxc::AppBarButton>();
        if (!button) {
            continue;
        }

        AnchorButton anchor = IdentifyDefaultButton(button);
        if (anchor == AnchorButton::Delete) {
            insertIndex = i + 1;
            break;
        }
        if (anchor == AnchorButton::Sort) {
            insertIndex = i;
            break;
        }
    }

    if (insertIndex == count) {
        // Explorer hasn't populated the command bar yet, or this build's
        // buttons aren't recognizable. Appending would put the button
        // somewhere it doesn't belong, and it would stay there, so leave it
        // to the update which follows Explorer's own commands being added.
        Wh_Log(L"Delete/Sort buttons not found in the command bar (yet)");
        return;
    }

    Wh_Log(L"Inserting the properties button at index %u", insertIndex);
    commands.InsertAt(insertIndex, CreatePropertiesButton());
}

void QueueCommandBarUpdate(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar) {
    auto commandBar = weakCommandBar.get();
    if (!commandBar) {
        return;
    }

    void* key = winrt::get_abi(commandBar);
    if (!g_pendingUpdates.insert(key).second) {
        return;  // Already queued.
    }

    auto takePending = [key]() {
        g_pendingUpdates.erase(key);
    };

    // Runs on this same thread, either from the dispatcher queue or right
    // below if queueing failed.
    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::
            GetForCurrentThread();
    if (!dispatcherQueue) {
        takePending();
        return;
    }

    if (!dispatcherQueue.TryEnqueue([weakCommandBar, takePending]() {
            takePending();

            if (g_unloading) {
                return;
            }

            if (auto commandBar = weakCommandBar.get()) {
                try {
                    UpdateCommandBar(commandBar);
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        })) {
        takePending();
    }
}

void OnCommandBarAdded(muxc::CommandBar const& commandBar) {
    // Prune entries whose command bar is gone. Only this thread's, since
    // g_entries is thread_local.
    for (auto it = g_entries.begin(); it != g_entries.end();) {
        if (!it->commandBar.get()) {
            it = g_entries.erase(it);
        } else {
            ++it;
        }
    }

    for (auto const& entry : g_entries) {
        if (entry.commandBar.get() == commandBar) {
            return;  // Already tracked (e.g. re-added to the tree).
        }
    }

    Wh_Log(L"Command bar found: %s, thread %u", commandBar.Name().c_str(),
           GetCurrentThreadId());

    CommandBarEntry entry;
    entry.commandBar = winrt::make_weak(commandBar);

    // Re-add the button whenever the command bar reloads.
    entry.loadedToken = commandBar.Loaded(
        [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
            if (auto commandBar = sender.try_as<muxc::CommandBar>()) {
                try {
                    UpdateCommandBar(commandBar);
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        });

    // Re-add the button if Explorer ever rebuilds the command list.
    entry.vectorChangedToken = commandBar.PrimaryCommands().VectorChanged(
        [weakCommandBar = winrt::make_weak(commandBar)](
            wfc::IObservableVector<muxc::ICommandBarElement> const&,
            wfc::IVectorChangedEventArgs const&) {
            if (g_unloading) {
                return;
            }

            // Defer the update; mutating the vector from within its own
            // change notification isn't allowed. Coalesced, since adding our
            // own button raises a notification too.
            QueueCommandBarUpdate(weakCommandBar);
        });

    g_entries.push_back(std::move(entry));

    try {
        UpdateCommandBar(commandBar);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void RemoveButtonsForCurrentThread() {
    // While the elements the handlers are registered on are still around.
    RevokeHandlersForCurrentThread();

    std::vector<CommandBarEntry> taken;
    taken.swap(g_entries);

    for (auto& entry : taken) {
        auto commandBar = entry.commandBar.get();
        if (!commandBar) {
            continue;
        }

        try {
            commandBar.Loaded(entry.loadedToken);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }

        try {
            commandBar.PrimaryCommands().VectorChanged(entry.vectorChangedToken);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }

        auto commands = commandBar.PrimaryCommands();
        for (uint32_t i = commands.Size(); i > 0; i--) {
            if (IsOurElement(commands.GetAt(i - 1))) {
                commands.RemoveAt(i - 1);
            }
        }
    }

    g_pendingUpdates.clear();
    g_threadScanned = false;
}

// Settings changed: take the buttons down and put them back with the new
// label/icon. The command bars are kept around: after removal there may be no
// element left to re-find them from on an unfocused window.
void RefreshButtonsForCurrentThread() {
    std::vector<muxc::CommandBar> commandBars;
    for (auto& entry : g_entries) {
        if (auto commandBar = entry.commandBar.get()) {
            commandBars.push_back(commandBar);
        }
    }

    RemoveButtonsForCurrentThread();

    for (auto const& commandBar : commandBars) {
        OnCommandBarAdded(commandBar);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Command bar discovery.
//
// The command bars are found by hooking a couple of functions of File
// Explorer's own WinUI 3 code (FileExplorerExtensions.dll) and by walking the
// XAML tree from there with the public VisualTreeHelper API.
//
// Note: XAML Diagnostics (InitializeXamlDiagnosticsEx) would be an easier way
// to watch for the command bar, but only one XAML diagnostics consumer can be
// active per process, which makes it conflict with other tools and mods, such
// as Windows 11 File Explorer Styler. That's why it's not used here.

void CollectCommandBars(mux::DependencyObject const& root,
                        int depth,
                        std::vector<muxc::CommandBar>* commandBars) {
    if (depth > 64) {
        return;
    }

    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto commandBar = child.try_as<muxc::CommandBar>();
            commandBar &&
            (commandBar.Name() == kPrimaryCommandBarName ||
             commandBar.Name() == kSecondaryCommandBarName)) {
            commandBars->push_back(std::move(commandBar));
            // No need to descend into a command bar we already found.
            continue;
        }

        CollectCommandBars(child, depth + 1, commandBars);
    }
}

muxc::CommandBar GetKnownCommandBarForCurrentThread() {
    for (auto const& entry : g_entries) {
        if (auto commandBar = entry.commandBar.get()) {
            return commandBar;
        }
    }

    return nullptr;
}

// Finds the command bars of the XAML island the given element belongs to.
// Must run on the element's UI thread.
void ScanXamlRootForCommandBars(mux::UIElement const& element) try {
    if (g_unloading || !element) {
        return;
    }

    auto xamlRoot = element.XamlRoot();
    if (!xamlRoot) {
        return;
    }

    auto content = xamlRoot.Content();
    if (!content) {
        return;
    }

    std::vector<muxc::CommandBar> commandBars;
    CollectCommandBars(content, 0, &commandBars);
    for (auto const& commandBar : commandBars) {
        OnCommandBarAdded(commandBar);
    }

    if (!commandBars.empty()) {
        // The island is built and its command bars are tracked; the focus
        // hook doesn't have to keep looking.
        g_threadScanned = true;
    }
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Same as above, but deferred, for the cases where the command bar isn't in
// the tree yet by the time our hook runs.
void ScheduleXamlRootScan(mux::UIElement const& element) try {
    if (g_unloading || !element) {
        return;
    }

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanXamlRootForCommandBars(element);
        return;
    }

    dispatcherQueue.TryEnqueue([weakElement = winrt::make_weak(element)]() {
        if (auto element = weakElement.get()) {
            ScanXamlRootForCommandBars(element);
        }
    });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Looks for the command bars of the current thread's XAML island without an
// element to start from: either from a command bar which is already known for
// this thread, or from the focused element. Must run on the UI thread.
void ScanCurrentThreadForCommandBars() try {
    if (g_unloading) {
        return;
    }

    if (auto knownCommandBar = GetKnownCommandBarForCurrentThread()) {
        ScanXamlRootForCommandBars(knownCommandBar);
        return;
    }

    auto focused = mux::Input::FocusManager::GetFocusedElement();
    auto element = focused ? focused.try_as<mux::UIElement>() : nullptr;
    if (!element) {
        Wh_Log(L"No XAML element to start from on thread %u",
               GetCurrentThreadId());
        return;
    }

    ScanXamlRootForCommandBars(element);
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Same as above, deferred to after the current layout pass.
void ScheduleCurrentThreadScan() try {
    if (g_unloading) {
        return;
    }

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanCurrentThreadForCommandBars();
        return;
    }

    dispatcherQueue.TryEnqueue([]() { ScanCurrentThreadForCommandBars(); });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks of File Explorer's WinUI 3 code.

// void CommandBarManager::CommandBar(muxc::CommandBar const& value)
//
// Called with the command bar element itself, which is the most direct way to
// get hold of it.
using CommandBarManager_CommandBar_t = void(WINAPI*)(void* pThis,
                                                     void* commandBar);
CommandBarManager_CommandBar_t CommandBarManager_CommandBar_Original;
void WINAPI CommandBarManager_CommandBar_Hook(void* pThis, void* commandBar) {
    Wh_Log(L">");

    CommandBarManager_CommandBar_Original(pThis, commandBar);

    if (g_unloading || !commandBar) {
        return;
    }

    try {
        auto const& element =
            *reinterpret_cast<muxc::CommandBar const*>(commandBar);
        if (!element) {
            return;
        }

        OnCommandBarAdded(element);

        // The command bar isn't necessarily attached to the tree yet, so also
        // scan the island once the current layout pass is done; that's where
        // a second command bar of the same island (the secondary one) is
        // found.
        ScheduleXamlRootScan(element);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

// void CommandBarControl::OnApplyTemplate()
//
// Runs when the control which hosts the command bar builds its contents, both
// for new windows and tabs and when Explorer rebuilds it. Note: the `this`
// pointer our hooks get is a C++/WinRT implementation object, not a XAML
// object, and there's no supported way to turn one into the other, so these
// hooks are only used as a signal to rescan the thread.
using CommandBarControl_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
CommandBarControl_OnApplyTemplate_t CommandBarControl_OnApplyTemplate_Original;
CommandBarControl_OnApplyTemplate_t
    CommandBarControl_Wave1_OnApplyTemplate_Original;

void WINAPI CommandBarControl_OnApplyTemplate_Hook(void* pThis) {
    CommandBarControl_OnApplyTemplate_Original(pThis);
    if (!g_unloading) {
        ScheduleCurrentThreadScan();
    }
}

void WINAPI CommandBarControl_Wave1_OnApplyTemplate_Hook(void* pThis) {
    CommandBarControl_Wave1_OnApplyTemplate_Original(pThis);
    if (!g_unloading) {
        ScheduleCurrentThreadScan();
    }
}

// void CommandBarControl::CommandBarControlGotFocusHandler(
//     IInspectable const& sender, RoutedEventArgs const&)
//
// A cheap extra chance to pick up a command bar we haven't seen yet, e.g. in
// a window which was already open when the mod was loaded.
using CommandBarControl_GotFocusHandler_t = void(WINAPI*)(void* pThis,
                                                          void* sender,
                                                          void* args);
CommandBarControl_GotFocusHandler_t CommandBarControl_GotFocusHandler_Original;
CommandBarControl_GotFocusHandler_t
    CommandBarControl_Wave1_GotFocusHandler_Original;

void HandleCommandBarControlGotFocus(void* sender) {
    if (g_unloading || !sender) {
        return;
    }

    // This runs on every focus change, and the scan walks the whole visual
    // tree, so skip it once this thread's command bars are known. A rebuilt
    // command bar comes back through OnApplyTemplate / Loaded instead.
    if (g_threadScanned && GetKnownCommandBarForCurrentThread()) {
        return;
    }

    try {
        auto const& inspectable =
            *reinterpret_cast<wf::IInspectable const*>(sender);
        if (auto element = inspectable ? inspectable.try_as<mux::UIElement>()
                                       : nullptr) {
            ScanXamlRootForCommandBars(element);
        }
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void WINAPI CommandBarControl_GotFocusHandler_Hook(void* pThis,
                                                   void* sender,
                                                   void* args) {
    CommandBarControl_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

void WINAPI CommandBarControl_Wave1_GotFocusHandler_Hook(void* pThis,
                                                         void* sender,
                                                         void* args) {
    CommandBarControl_Wave1_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

////////////////////////////////////////////////////////////////////////////////
// Hook installation. FileExplorerExtensions.dll may or may not be loaded when
// the mod initializes, so its load is caught with a LoadLibraryExW hook.

enum class SymbolHookResult {
    Success,
    // Symbol resolution itself failed, e.g. the PDB couldn't be downloaded.
    // Transient, so trying again later can succeed.
    ResolutionFailed,
    // The symbols resolved, but this build has none of the functions the mod
    // needs. Permanent for this Explorer, so it must not be retried: each
    // HookSymbols call for a module invalidates its symbol cache and forces a
    // re-resolution.
    NoSymbolFound,
};

SymbolHookResult HookFileExplorerExtensionsSymbols(HMODULE module) {
    // All hooks are optional, since the set of functions differs between
    // Windows builds, but at least one of the discovery hooks must be found.
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const & __ptr64) __ptr64)",
            },
            &CommandBarManager_CommandBar_Original,
            CommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void) __ptr64)",
            },
            &CommandBarControl_OnApplyTemplate_Original,
            CommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void) __ptr64)",
            },
            &CommandBarControl_Wave1_OnApplyTemplate_Original,
            CommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &CommandBarControl_GotFocusHandler_Original,
            CommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &CommandBarControl_Wave1_GotFocusHandler_Original,
            CommandBarControl_Wave1_GotFocusHandler_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, fileExplorerExtensionsDllHooks,
                                    ARRAYSIZE(fileExplorerExtensionsDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return SymbolHookResult::ResolutionFailed;
    }

    if (!CommandBarManager_CommandBar_Original &&
        !CommandBarControl_OnApplyTemplate_Original &&
        !CommandBarControl_Wave1_OnApplyTemplate_Original) {
        Wh_Log(L"No command bar symbol was found");
        return SymbolHookResult::NoSymbolFound;
    }

    return SymbolHookResult::Success;
}

HMODULE GetFileExplorerExtensionsModuleHandle() {
    return GetModuleHandle(L"FileExplorerExtensions.dll");
}

// Returns false only if the module is loaded but hooking it failed.
bool HookFileExplorerExtensionsIfLoaded(bool applyHooks) {
    if (g_symbolsHooked) {
        return true;
    }

    HMODULE module = GetFileExplorerExtensionsModuleHandle();
    if (!module) {
        return true;
    }

    if (g_symbolsHooked.exchange(true)) {
        return true;
    }

    Wh_Log(L"Hooking FileExplorerExtensions.dll");

    switch (HookFileExplorerExtensionsSymbols(module)) {
        case SymbolHookResult::Success:
            break;

        case SymbolHookResult::ResolutionFailed:
            // Transient, so let a later attempt try again instead of leaving
            // the mod disabled for this process.
            g_symbolsHooked = false;
            return false;

        case SymbolHookResult::NoSymbolFound:
            // Keep the flag set: this build doesn't have what the mod needs,
            // and re-resolving the symbols on every module load would only
            // make Explorer slow for the rest of the session.
            return false;
    }

    if (applyHooks) {
        Wh_ApplyHookOperations();
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading || !lpLibFileName) {
        return module;
    }

    // Explorer loads modules constantly (shell extensions, thumbnail and
    // preview handlers), and this runs inline on whatever thread did the load,
    // so only look at the one module the mod cares about.
    PCWSTR fileName = lpLibFileName;
    for (PCWSTR p = lpLibFileName; *p; p++) {
        if (*p == L'\\' || *p == L'/') {
            fileName = p + 1;
        }
    }

    // LoadLibraryEx appends the default extension itself, so the caller may
    // have left it out.
    if (_wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0 ||
        _wcsicmp(fileName, L"FileExplorerExtensions") == 0) {
        HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/true);
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Reaching a window's UI thread, for button setup, refresh and removal.

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
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
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

std::vector<HWND> GetFileExplorerWnds() {
    std::vector<HWND> hWnds;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& hWnds = *(std::vector<HWND>*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[64];
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                hWnds.push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&hWnds);

    return hWnds;
}

////////////////////////////////////////////////////////////////////////////////
// Initialization plumbing.

void LoadSettings() {
    PCWSTR buttonLabel = Wh_GetStringSetting(L"buttonLabel");
    PCWSTR iconGlyph = Wh_GetStringSetting(L"iconGlyph");
    PCWSTR noSelectionBehavior = Wh_GetStringSetting(L"noSelectionBehavior");
    bool showLabel = Wh_GetIntSetting(L"showLabel");

    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        g_settings.buttonLabel = buttonLabel;
        g_settings.iconGlyph = iconGlyph;
        g_settings.showLabel = showLabel;
        g_settings.noSelectionFolder =
            noSelectionBehavior && _wcsicmp(noSelectionBehavior, L"none") != 0;
    }

    Wh_FreeStringSetting(buttonLabel);
    Wh_FreeStringSetting(iconGlyph);
    Wh_FreeStringSetting(noSelectionBehavior);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (GetFileExplorerExtensionsModuleHandle()) {
        if (!HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/false)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"FileExplorerExtensions.dll isn't loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW) {
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/true);

    // Windows which were already open when the mod was loaded won't
    // necessarily rebuild their command bar, so look for it explicitly.
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { ScanCurrentThreadForCommandBars(); }, nullptr);
    }
}

// Our function hooks are gone by the time Wh_ModUninit runs, but the XAML
// delegates aren't - they stay live until they're unregistered below. Setting
// the flag one callback earlier shortens the window in which they still act.
void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    for (HWND hWnd : GetFileExplorerWnds()) {
        Wh_Log(L"Removing the button for window %08X", (DWORD)(ULONG_PTR)hWnd);
        if (!RunFromWindowThread(
                hWnd, [](PVOID) { RemoveButtonsForCurrentThread(); },
                nullptr)) {
            Wh_Log(L"Couldn't reach the thread of window %08X",
                   (DWORD)(ULONG_PTR)hWnd);
        }
    }

    // The DLL can't be unmapped while a worker thread is still running our
    // code. The wait has no timeout, and a thread can be inside a modal
    // properties dialog, which blocks it until the user closes the dialog -
    // hence the note in the readme about closing those before disabling.
    WaitForLaunchThreads();
}

// The mod settings were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { RefreshButtonsForCurrentThread(); }, nullptr);
    }
}
