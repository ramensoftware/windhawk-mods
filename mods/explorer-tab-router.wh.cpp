// ==WindhawkMod==
// @id              explorer-tab-router
// @name            Explorer Tab Router
// @name:zh-CN      资源管理器页签路由
// @description     Reuse and activate Explorer folder tabs, preserve file selection and explicit new windows
// @description:zh-CN 复用并切换资源管理器页签，保留文件选中状态和明确的新窗口操作
// @version         1.0.3
// @author          afate123
// @github          https://github.com/afate123
// @license         MIT
// @architecture    x86-64
// @include         explorer.exe
// @include         rundll32.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luuid -lcomctl32
// ==/WindhawkMod==

/*
MIT License

Copyright (c) ALMAS CP
Copyright (c) Explorer Tab Router contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

// clang-format off
// ==WindhawkModReadme==
/*
# Explorer Tab Router / 资源管理器页签路由

![External folder requests become tabs; reopening reuses a tab / 外部打开转为页签，重复请求复用页签](https://raw.githubusercontent.com/afate123/windhawk-mods/d3c374a386345b893a2318a483ff1adb0690dd2f/assets/explorer-tab-router.gif)

## English

Explorer Tab Router keeps ordinary folder-opening requests together in Windows 11 File Explorer while preserving explicit requests for a separate window.

### Behavior

- **Open a folder externally:** reuse and activate an existing tab for that folder. If the folder is not open, route the new folder window into a tab in an existing Explorer window.
- **Show a file in its folder:** activate the matching folder tab and preserve the requested file selection.
- **Open in new tab:** create a new tab and switch to it automatically, including when that folder already has another tab.
- **Open in new window:** keep the explicitly requested window separate.
- **Hold Shift while opening a folder:** bypass ordinary window-to-tab routing and keep a separate window.

Enable the mod in Windhawk, then use File Explorer and other applications normally. Folder routing and tab activation do not require a separate application list. Control Panel and unsupported shell locations are allowed to remain in their own windows.

### Source and conflicting mods

This mod builds on the routing approach introduced by [Explorer Single Window Tabs](https://windhawk.net/mods/explorer-single-window-tabs) by [ALMAS CP](https://github.com/almas-cp). It adds existing-tab reuse and activation, file-selection preservation, explicit-window handling, automatic new-tab activation, and managed presentation recovery.

**Do not enable Explorer Single Window Tabs and Explorer Tab Router together. Disable the other routing mod first.**

### Compatibility and limitations

Windows 11 x64 with native File Explorer tabs is required. This mod uses internal shell symbols, which are checked when it loads. A future Windows update can require a compatibility update.

Some requests are handled through a temporary transparent Explorer window before routing completes. This is not a guarantee of entirely window-free navigation. If a request cannot be completed safely, the temporary window is restored instead of discarding the request. Window presentation state, including existing layered transparency, is restored on recovery.

The mod is distributed under the MIT license.

## 简体中文

资源管理器页签路由将普通的文件夹打开请求集中到 Windows 11 资源管理器页签中，同时保留明确要求打开独立窗口的操作。

### 使用行为

- **从外部应用打开文件夹：** 已有相同目录页签时，复用并切换过去；尚未打开时，将新目录窗口转入现有资源管理器的页签。
- **在文件夹中显示文件：** 切换到对应目录页签，并保留目标文件的选中状态。
- **在新标签页中打开：** 新建页签并自动切换，即使相同目录已有其他页签。
- **在新窗口中打开：** 保留明确请求的独立窗口。
- **按住 Shift 打开文件夹：** 绕过普通窗口转页签行为，保留独立窗口。

在 Windhawk 中启用后即可正常使用，不需要逐个添加应用名单。控制面板及不支持的 Shell 位置可以保持独立窗口。

### 来源与冲突提示

本 mod 基于 [ALMAS CP](https://github.com/almas-cp) 的 [Explorer Single Window Tabs](https://windhawk.net/mods/explorer-single-window-tabs) 路由思路扩展，增加已有页签复用与切换、文件选中保留、明确的新窗口请求识别、新页签自动激活，以及窗口状态恢复管理。

**请勿同时启用 Explorer Single Window Tabs 与 Explorer Tab Router；启用前先停用另一个路由 mod。**

### 兼容性与限制

需要 Windows 11 x64 以及支持原生页签的资源管理器。加载时会检查所需的系统内部符号，未来 Windows 更新可能需要同步更新兼容实现。

部分请求会先经过临时透明窗口，再完成页签路由，因此不保证整个过程完全不创建窗口。无法安全完成时会恢复临时窗口，避免丢弃打开请求；恢复时会还原原有窗口显示状态，包括已有的分层透明属性。

本 mod 使用 MIT 许可。

*/
// ==/WindhawkModReadme==
// clang-format on

#include <exdisp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <string>

#include <exdisp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
#include <wrl/client.h>
#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

#include <array>
#include <new>
#include <string_view>

namespace etr {
enum class DestinationKind { Unavailable, ShellAlias, ControlPanel, Folder };

constexpr wchar_t FoldShellAscii(wchar_t c) noexcept {
    return c >= L'A' && c <= L'Z' ? c + (L'a' - L'A') : c;
}
constexpr bool ShellPrefixEqual(std::wstring_view value,
                                std::wstring_view prefix) noexcept {
    if (value.size() < prefix.size())
        return false;
    for (size_t i = 0; i < prefix.size(); ++i)
        if (FoldShellAscii(value[i]) != FoldShellAscii(prefix[i]))
            return false;
    return true;
}
constexpr DestinationKind ClassifyShellDestination(
    std::wstring_view address) noexcept {
    if (address.empty())
        return DestinationKind::Unavailable;
    if (ShellPrefixEqual(address, L"shell:::{"))
        return DestinationKind::ShellAlias;
    // Interpret a namespace root, never a GUID embedded in a filesystem name.
    constexpr std::array roots = {
        std::wstring_view{L"::{26EE0668-A00A-44D7-9371-BEB064C98683}"},
        std::wstring_view{L"::{21EC2020-3AEA-1069-A2DD-08002B30309D}"},
        std::wstring_view{L"::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}"}};
    if (ShellPrefixEqual(address, L"::{")) {
        for (size_t offset = 0; offset < address.size();) {
            auto component = address.substr(offset);
            for (auto root : roots)
                if (ShellPrefixEqual(component, root) &&
                    (component.size() == root.size() ||
                     component[root.size()] == L'\\' ||
                     component[root.size()] == L'/'))
                    return DestinationKind::ControlPanel;
            size_t next = address.find_first_of(L"\\/", offset);
            if (next == address.npos)
                break;
            offset = next + 1;
        }
    }
    return DestinationKind::Folder;
}
}  // namespace etr

#include <exdisp.h>
#include <shlobj.h>
#include <windows.h>
#include <wrl/client.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>

namespace etr {
using Microsoft::WRL::ComPtr;
struct ShellMemoryDeleter {
    void operator()(void* p) const noexcept { CoTaskMemFree(p); }
};
using OwnedPidl = std::unique_ptr<ITEMIDLIST, ShellMemoryDeleter>;
using OwnedShellText = std::unique_ptr<wchar_t, ShellMemoryDeleter>;
struct FolderLocation {
    HWND frame = nullptr, tab = nullptr;
    ComPtr<IShellView> view;
    OwnedPidl identity;
    std::wstring address;
};

// One apartment-local ShellWindows connection per routing request; navigation
// results remain fresh and are never cached across polling attempts.
class ShellTabCatalog {
    ComPtr<IShellWindows> windows_;
    ULONGLONG nextConnectAttempt_ = 0;

    bool EnsureConnection() {
        if (windows_)
            return true;
        if (GetTickCount64() < nextConnectAttempt_)
            return false;
        nextConnectAttempt_ = GetTickCount64() + 100;
        CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(&windows_));
        return windows_ != nullptr;
    }

    static bool HasClass(HWND window, const wchar_t* expected) noexcept {
        wchar_t name[64]{};
        return GetClassNameW(window, name, static_cast<int>(std::size(name))) &&
               wcscmp(name, expected) == 0;
    }
    struct TabAccess {
        HWND frame, tab;
        ComPtr<IShellBrowser> browser;
    };
    template <class Visitor>
    void VisitTabs(Visitor visitor) {
        if (!EnsureConnection())
            return;
        long count = 0;
        if (FAILED(windows_->get_Count(&count))) {
            windows_.Reset();
            return;
        }
        for (long index = 0; index < count; ++index) {
            VARIANT key{};
            key.vt = VT_I4;
            key.lVal = index;
            ComPtr<IDispatch> dispatch;
            if (FAILED(windows_->Item(key, &dispatch)) || !dispatch)
                continue;
            ComPtr<IServiceProvider> services;
            TabAccess access{};
            if (FAILED(dispatch.As(&services)) ||
                FAILED(services->QueryService(SID_STopLevelBrowser,
                                              IID_PPV_ARGS(&access.browser))) ||
                FAILED(access.browser->GetWindow(&access.tab)) ||
                !HasClass(access.tab, L"ShellTabWindowClass"))
                continue;
            access.frame = GetAncestor(access.tab, GA_ROOT);
            if (!HasClass(access.frame, L"CabinetWClass"))
                continue;
            if (visitor(access))
                return;
        }
    }
    static std::optional<FolderLocation> Inspect(const TabAccess& access) {
        FolderLocation result;
        result.frame = access.frame;
        result.tab = access.tab;
        ComPtr<IFolderView> folderView;
        ComPtr<IPersistFolder2> folder;
        if (FAILED(access.browser->QueryActiveShellView(&result.view)) ||
            !result.view || FAILED(result.view.As(&folderView)) ||
            FAILED(folderView->GetFolder(IID_PPV_ARGS(&folder))))
            return std::nullopt;
        PIDLIST_ABSOLUTE pidl = nullptr;
        HRESULT hr = folder->GetCurFolder(&pidl);
        result.identity.reset(pidl);
        if (FAILED(hr) || !result.identity)
            return std::nullopt;
        PWSTR text = nullptr;
        hr = SHGetNameFromIDList(result.identity.get(),
                                 SIGDN_DESKTOPABSOLUTEPARSING, &text);
        OwnedShellText ownedText(text);
        if (FAILED(hr) || !ownedText || !*ownedText)
            return std::nullopt;
        result.address = ownedText.get();
        return result;
    }
    static bool CanHostTabs(HWND frame, HWND excluded) noexcept {
        if (frame == excluded || !IsWindowVisible(frame) ||
            GetPropW(frame, L"ETR.Redirecting"))
            return false;
        BYTE alpha = 255;
        DWORD flags = 0;
        return !(GetLayeredWindowAttributes(frame, nullptr, &alpha, &flags) &&
                 (flags & LWA_ALPHA) && alpha == 0);
    }

   public:
    ShellTabCatalog() { EnsureConnection(); }
    std::optional<FolderLocation> ReadFrameDestination(HWND target) {
        // An explicit source lookup must accept a transparent staging window.
        if (!IsWindow(target))
            return std::nullopt;
        HWND frame = HasClass(target, L"ShellTabWindowClass")
                         ? GetAncestor(target, GA_ROOT)
                         : target;
        if (!HasClass(frame, L"CabinetWClass"))
            return std::nullopt;
        HWND wanted =
            target == frame
                ? FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr)
                : target;
        if (!wanted)
            return std::nullopt;
        std::optional<FolderLocation> result;
        VisitTabs([&](const TabAccess& access) {
            if (access.frame != frame || access.tab != wanted)
                return false;
            result = Inspect(access);
            return true;
        });
        return result;
    }
    HWND ChooseRoutingHost(HWND excluded, HWND preferred = nullptr) {
        if (excluded)
            excluded = GetAncestor(excluded, GA_ROOT);
        if (!preferred)
            preferred = GetAncestor(GetForegroundWindow(), GA_ROOT);
        HWND fallback = nullptr;
        std::unordered_set<HWND> inspected;
        VisitTabs([&](const TabAccess& access) {
            if (!CanHostTabs(access.frame, excluded) ||
                access.tab != FindWindowExW(access.frame, nullptr,
                                            L"ShellTabWindowClass", nullptr) ||
                !inspected.insert(access.frame).second)
                return false;
            auto location = Inspect(access);
            if (!location || ClassifyShellDestination(location->address) !=
                                 DestinationKind::Folder)
                return false;
            if (!fallback)
                fallback = access.frame;
            if (access.frame != preferred)
                return false;
            fallback = access.frame;
            return true;
        });
        return fallback;
    }
};
}  // namespace etr

#include <shellapi.h>
#include <windows.h>
#include <string_view>

namespace etr {
inline bool IsShellWindowsServiceCommand(const wchar_t* command) {
    int count = 0;
    wchar_t** arguments = CommandLineToArgvW(command, &count);
    if (!arguments)
        return false;
    bool matches = false;
    if (count >= 3) {
        std::wstring_view library = arguments[1];
        size_t separator = library.find_last_of(L"\\/");
        if (separator != library.npos)
            library.remove_prefix(separator + 1);
        matches = _wcsicmp(library.data(),
                           L"shell32.dll,SHCreateLocalServerRunDll") == 0 &&
                  _wcsicmp(arguments[2],
                           L"{9BA05972-F6A8-11CF-A442-00A0C90A8F39}") == 0;
    }
    LocalFree(arguments);
    return matches;
}
}  // namespace etr

using Microsoft::WRL::ComPtr;

static ComPtr<IShellBrowser> GetTabBrowser(IDispatch* dispatch) {
    ComPtr<IServiceProvider> service;
    ComPtr<IShellBrowser> browser;
    if (SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS(&service))))
        service->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser));
    return browser;
}

static bool SameFolder(IShellView* view, PCIDLIST_ABSOLUTE requested) {
    ComPtr<IFolderView> fv;
    ComPtr<IPersistFolder2> folder;
    PIDLIST_ABSOLUTE actual = nullptr;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(&fv))) ||
        FAILED(fv->GetFolder(IID_PPV_ARGS(&folder))) ||
        FAILED(folder->GetCurFolder(&actual)) || !actual)
        return false;
    bool equal = ILIsEqual(actual, requested);
    if (!equal) {
        PWSTR a = nullptr, b = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(actual, SIGDN_FILESYSPATH, &a)) &&
            SUCCEEDED(SHGetNameFromIDList(requested, SIGDN_FILESYSPATH, &b)))
            equal = CompareStringOrdinal(a, -1, b, -1, TRUE) == CSTR_EQUAL;
        CoTaskMemFree(a);
        CoTaskMemFree(b);
    }
    CoTaskMemFree(actual);
    return equal;
}

// ExplorerTabUtility uses 0xA221 (Ctrl+1..n). Child z-order is NOT tab order:
// validate the active HWND after each command instead of guessing an index.
static bool ActivateShellTab(HWND tab) {
    if (!IsWindow(tab))
        return false;
    HWND frame = GetAncestor(tab, GA_ROOT);
    WCHAR cls[64]{};
    GetClassNameW(frame, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"CabinetWClass") != 0)
        return false;
    // Redirect placeholders are transparent but still WS_VISIBLE. Never
    // activate one while Explorer is resolving a new folder request.
    if (!IsWindowVisible(frame))
        return false;
    if (GetWindowLongW(frame, GWL_EXSTYLE) & WS_EX_LAYERED) {
        BYTE alpha = 255;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(frame, nullptr, &alpha, &flags) &&
            (flags & LWA_ALPHA) && alpha == 0)
            return false;
    }
    if (IsIconic(frame))
        ShowWindowAsync(frame, SW_RESTORE);
    if (FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr) != tab) {
        int count = 0;
        for (HWND h = nullptr;
             (h = FindWindowExW(frame, h, L"ShellTabWindowClass", nullptr));)
            ++count;
        bool activated = false;
        for (int i = 1; i <= count; ++i) {
            DWORD_PTR result = 0;
            if (!SendMessageTimeoutW(frame, WM_COMMAND, 0xA221, i,
                                     SMTO_ABORTIFHUNG, 300, &result))
                return false;
            if (FindWindowExW(frame, nullptr, L"ShellTabWindowClass",
                              nullptr) == tab) {
                activated = true;
                break;
            }
        }
        if (!activated)
            return false;
    }
    SetForegroundWindow(frame);
    return true;
}

static bool ReuseFolderTab(PCIDLIST_ABSOLUTE folder,
                           UINT count,
                           PCUITEMID_CHILD_ARRAY items,
                           DWORD flags,
                           HWND exclude = nullptr) {
    if (!folder || (flags & ~DWORD(OFASI_EDIT | OFASI_OPENDESKTOP)))
        return false;
    if (flags & OFASI_OPENDESKTOP)
        return false;
    ComPtr<IShellWindows> windows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&windows))))
        return false;
    long n = 0;
    windows->get_Count(&n);
    for (long i = 0; i < n; ++i) {
        VARIANT idx{};
        idx.vt = VT_I4;
        idx.lVal = i;
        ComPtr<IDispatch> dispatch;
        if (FAILED(windows->Item(idx, &dispatch)) || !dispatch)
            continue;
        auto browser = GetTabBrowser(dispatch.Get());
        if (!browser)
            continue;
        HWND tab = nullptr;
        if (FAILED(browser->GetWindow(&tab)) || !tab)
            continue;
        HWND frame = GetAncestor(tab, GA_ROOT);
        if (frame == exclude || !IsWindowVisible(frame))
            continue;
        ComPtr<IShellView> view;
        if (FAILED(browser->QueryActiveShellView(&view)) || !view ||
            !SameFolder(view.Get(), folder))
            continue;
        if (!ActivateShellTab(tab))
            continue;
        for (UINT j = 0; j < count; ++j) {
            UINT select = SVSI_SELECT | SVSI_ENSUREVISIBLE;
            if (j == 0)
                select |= SVSI_DESELECTOTHERS | SVSI_FOCUSED;
            if ((flags & OFASI_EDIT) && count == 1)
                select |= SVSI_EDIT;
            if (FAILED(view->SelectItem(items[j], select)))
                return false;
        }
        return true;
    }
    return false;
}

#include <mutex>
#include <unordered_map>
static std::mutex g_explicitMutex;
static std::unordered_map<void*, bool> g_folderVerbs;
struct ExplicitWindowRequest {
    std::wstring path;
    ULONGLONG expires;
};
static std::vector<ExplicitWindowRequest> g_explicitWindows;
static thread_local bool g_explicitWindowCall = false;
static thread_local bool g_explicitWindowCreated = false;

static void* CommandIdentity(void* self) {
    ComPtr<IUnknown> identity;
    reinterpret_cast<IUnknown*>(self)->QueryInterface(IID_PPV_ARGS(&identity));
    return identity.Get();
}
static void QueueFocus(HWND tab);
static void ScheduleNewTab(HWND tab) {
    HWND frame = GetAncestor(tab, GA_ROOT);
    int count = 0;
    for (HWND t = nullptr;
         (t = FindWindowExW(frame, t, L"ShellTabWindowClass", nullptr));)
        ++count;
    if (count < 2)
        return;
    QueueFocus(tab);
}
static std::wstring PidlPath(PCIDLIST_ABSOLUTE pidl) {
    PWSTR value = nullptr;
    std::wstring result;
    if (pidl && SUCCEEDED(SHGetNameFromIDList(
                    pidl, SIGDN_DESKTOPABSOLUTEPARSING, &value))) {
        result = value;
        CoTaskMemFree(value);
        while (result.size() > 3 &&
               (result.back() == L'\\' || result.back() == L'/'))
            result.pop_back();
    }
    return result;
}
static bool ConsumeExplicitWindow(const std::wstring& raw) {
    auto path = raw;
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/'))
        path.pop_back();
    std::lock_guard<std::mutex> lock(g_explicitMutex);
    ULONGLONG now = GetTickCount64();
    for (auto i = g_explicitWindows.begin(); i != g_explicitWindows.end();) {
        if (now > i->expires) {
            i = g_explicitWindows.erase(i);
            continue;
        }
        if (CompareStringOrdinal(path.c_str(), -1, i->path.c_str(), -1, TRUE) ==
            CSTR_EQUAL) {
            g_explicitWindows.erase(i);
            return true;
        }
        ++i;
    }
    return false;
}
using FolderInitialize_t = HRESULT (*)(void*, PCWSTR, IPropertyBag*);
using FolderShow_t = HRESULT (*)(void*, PCIDLIST_ABSOLUTE, bool);
static FolderInitialize_t FolderInitialize_Original;
static FolderShow_t FolderShow_Original;
static HRESULT FolderInitialize_Hook(void* self,
                                     PCWSTR name,
                                     IPropertyBag* bag) {
    HRESULT hr = FolderInitialize_Original(self, name, bag);
    VARIANT flags{};
    if (bag)
        bag->Read(L"LaunchExplorerFlags", &flags, nullptr);
    DWORD value = flags.vt == VT_UI4  ? flags.ulVal
                  : flags.vt == VT_I4 ? flags.lVal
                                      : 0;
    void* identity = CommandIdentity(self);
    Wh_Log(L"Initialize self=%p identity=%p name=%s flags=%lu vt=%d", self,
           identity, name ? name : L"", value, flags.vt);
    std::lock_guard<std::mutex> lock(g_explicitMutex);
    // Each initialization overwrites the previous value if a COM address is
    // reused.
    if (g_folderVerbs.size() >= 256)
        g_folderVerbs.clear();
    g_folderVerbs[identity] =
        SUCCEEDED(hr) &&
        ((name && _wcsicmp(name, L"opennewwindow") == 0) || (value & 1));
    VariantClear(&flags);
    return hr;
}
static HRESULT FolderShow_Hook(void* self, PCIDLIST_ABSOLUTE pidl, bool flag) {
    bool separate = false;
    auto path = PidlPath(pidl);
    void* identity = CommandIdentity(self);
    {
        std::lock_guard<std::mutex> lock(g_explicitMutex);
        auto i = g_folderVerbs.find(identity);
        separate = i != g_folderVerbs.end() && i->second;
        if (separate && !path.empty())
            g_explicitWindows.push_back({path, GetTickCount64() + 10000});
    }
    Wh_Log(L"ShowFolder self=%p identity=%p separate=%d flag=%d path=%s", self,
           identity, separate, flag, path.c_str());
    bool previous = g_explicitWindowCall,
         previousCreated = g_explicitWindowCreated;
    g_explicitWindowCall = previous || separate;
    g_explicitWindowCreated = false;
    HRESULT hr = FolderShow_Original(self, pidl, flag);
    if (separate && (g_explicitWindowCreated || FAILED(hr)))
        ConsumeExplicitWindow(path);
    g_explicitWindowCall = previous;
    g_explicitWindowCreated = previousCreated;
    return hr;
}

// Constants
#define WC_CABINET L"CabinetWClass"
#define WC_SHELLTAB L"ShellTabWindowClass"
#define CMD_NEW_TAB 0xA21B  // Internal Explorer "new tab" command

#include <windows.h>

#include <atomic>
#include <condition_variable>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace etr {

// Owns presentation changes only while the window still carries this entry's
// token. No window procedure, timer, hook, or per-window thread is installed.
class WindowPresentationStore final {
   public:
    static constexpr const wchar_t* PropertyName = L"ETR.Redirecting";

    WindowPresentationStore() = default;
    WindowPresentationStore(const WindowPresentationStore&) = delete;
    WindowPresentationStore& operator=(const WindowPresentationStore&) = delete;

    ~WindowPresentationStore() = default;

    [[nodiscard]] std::optional<ULONG_PTR> Stage(HWND window) {
        if (!::IsWindow(window) ||
            ::GetPropW(window, PropertyName) != nullptr) {
            ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return std::nullopt;
        }

        const auto token = AllocateToken();
        if (!token) {
            ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return std::nullopt;
        }

        std::shared_ptr<Entry> entry;
        std::shared_ptr<Entry> previous;
        try {
            entry = std::make_shared<Entry>(window, *token);
            {
                std::lock_guard lock(entriesMutex_);
                if (stopping_) {
                    ::SetLastError(ERROR_OPERATION_ABORTED);
                    return std::nullopt;
                }
                if (const auto old = entries_.find(window);
                    old != entries_.end()) {
                    std::lock_guard oldLock(old->second->mutex);
                    if (old->second->state != State::Staged) {
                        ::SetLastError(ERROR_BUSY);
                        return std::nullopt;
                    }
                    old->second->state = State::Inspecting;
                    previous = old->second;
                } else {
                    entries_.emplace(window, entry);
                }
            }
            if (previous) {
                // Check after reserving the old record: the initial GetProp
                // snapshot may have raced another completed Stage operation.
                if (HasToken(*previous)) {
                    SetState(previous, State::Staged);
                    ::SetLastError(ERROR_ALREADY_EXISTS);
                    return std::nullopt;
                }
                Finish(previous);
                std::lock_guard lock(entriesMutex_);
                if (stopping_ || !entries_.emplace(window, entry).second) {
                    ::SetLastError(stopping_ ? ERROR_OPERATION_ABORTED
                                             : ERROR_BUSY);
                    return std::nullopt;
                }
            }
        } catch (...) {
            ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return std::nullopt;
        }

        entry->threadId = ::GetWindowThreadProcessId(window, &entry->processId);
        if (entry->threadId == 0) {
            return StageFailed(entry, ERROR_INVALID_WINDOW_HANDLE);
        }
        if (!ReadExStyle(window, entry->exStyle)) {
            return StageFailed(entry, LastErrorOr(ERROR_INVALID_WINDOW_HANDLE));
        }
        entry->wasLayered = (entry->exStyle & WS_EX_LAYERED) != 0;
        if (entry->wasLayered && !::GetLayeredWindowAttributes(
                                     window, &entry->colorKey, &entry->alpha,
                                     &entry->layeredFlags)) {
            // UpdateLayeredWindow-backed windows need not expose attributes.
            // Refuse them before making any changes that cannot be undone.
            return StageFailed(entry, LastErrorOr(ERROR_NOT_SUPPORTED));
        }
        entry->captured = true;

        if (!SameWindow(*entry) ||
            ::GetPropW(window, PropertyName) != nullptr) {
            return StageFailed(entry, ERROR_INVALID_WINDOW_HANDLE);
        }
        if (!::SetPropW(window, PropertyName, TokenHandle(entry->token))) {
            return StageFailed(entry, LastErrorOr(ERROR_ACCESS_DENIED));
        }
        entry->propertyInstalled = true;
        if (!HasToken(*entry)) {
            return StageFailed(entry, ERROR_INVALID_WINDOW_HANDLE);
        }

        entry->presentationTouched = true;
        if (!WriteExStyle(window, entry->exStyle | WS_EX_LAYERED)) {
            return StageFailed(entry, LastErrorOr(ERROR_ACCESS_DENIED));
        }
        if (!HasToken(*entry) ||
            !::SetLayeredWindowAttributes(window, 0, 0, LWA_ALPHA)) {
            return StageFailed(entry, LastErrorOr(ERROR_INVALID_WINDOW_HANDLE));
        }
        if (!HasToken(*entry)) {
            return StageFailed(entry, ERROR_INVALID_WINDOW_HANDLE);
        }

        SetState(entry, State::Staged);
        return entry->token;
    }

    [[nodiscard]] bool Owns(HWND window, ULONG_PTR token) const {
        const auto entry = Find(window, token);
        if (!entry) {
            return false;
        }
        {
            std::lock_guard lock(entry->mutex);
            if (entry->state == State::Staging ||
                entry->state == State::Finished) {
                return false;
            }
        }
        return HasToken(*entry);
    }

    [[nodiscard]] bool Reveal(HWND window, ULONG_PTR token) {
        const auto entry = Find(window, token);
        if (!entry || !Begin(entry, State::Restoring)) {
            return false;
        }
        return Restore(entry, true);
    }

    [[nodiscard]] bool Retire(HWND window, ULONG_PTR token) {
        const auto entry = Find(window, token);
        if (!entry || !Begin(entry, State::Retiring)) {
            return false;
        }
        if (!HasToken(*entry)) {
            Finish(entry);
            ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return false;
        }
        if (!::PostMessageW(window, WM_CLOSE, 0, 0)) {
            const DWORD error = LastErrorOr(ERROR_ACCESS_DENIED);
            // A failed close must not strand an invisible Explorer window.
            (void)Restore(entry, true);
            ::SetLastError(error);
            return false;
        }

        // Posting the close commits retirement. It must not subsequently be
        // shown by RestoreAll while WM_CLOSE is still waiting in the queue.
        (void)RemoveOwnedProperty(*entry);
        Finish(entry);
        return true;
    }

    // The owner must stop/join its worker before shutdown. This also waits for
    // any already reserved operation; all window calls run without our locks.
    void RestoreAll() {
        if (restoreSweepActive_.test_and_set(std::memory_order_acquire)) {
            return;
        }
        struct EndSweep final {
            std::atomic_flag& active;
            ~EndSweep() { active.clear(std::memory_order_release); }
        } endSweep{restoreSweepActive_};
        {
            std::lock_guard lock(entriesMutex_);
            stopping_ = true;
            for (const auto& item : entries_) {
                std::lock_guard entryLock(item.second->mutex);
                item.second->restoreVisited = false;
            }
        }
        for (;;) {
            std::shared_ptr<Entry> entry;
            {
                std::lock_guard lock(entriesMutex_);
                for (const auto& item : entries_) {
                    std::lock_guard entryLock(item.second->mutex);
                    if (!item.second->restoreVisited) {
                        item.second->restoreVisited = true;
                        entry = item.second;
                        break;
                    }
                }
            }
            if (!entry) {
                return;
            }
            {
                std::unique_lock lock(entry->mutex);
                entry->changed.wait(lock, [&] {
                    return entry->state == State::Staged ||
                           entry->state == State::Finished;
                });
                if (entry->state == State::Finished) {
                    lock.unlock();
                    Erase(entry);
                    continue;
                }
                entry->state = State::Restoring;
            }
            // Attempt every entry even if one live window refuses restoration.
            // A failed live record remains available to a later RestoreAll
            // call.
            (void)Restore(entry, true);
        }
    }

    [[nodiscard]] bool IsStaged(HWND window) const {
        const auto entry = Find(window);
        if (!entry) {
            return ::GetPropW(window, PropertyName) != nullptr;
        }
        bool suppressShow = true;
        bool staging = false;
        {
            std::lock_guard lock(entry->mutex);
            suppressShow = entry->state != State::Presenting &&
                           entry->state != State::Finished;
            staging = entry->state == State::Staging;
        }
        const HANDLE property = ::GetPropW(window, PropertyName);
        if (property == TokenHandle(entry->token)) {
            return suppressShow;
        }
        // Before Stage installs the property, reserve the window against a
        // synchronous ShowWindow interception as well.
        return property != nullptr || (suppressShow && staging);
    }

    [[nodiscard]] static HWND Root(HWND window) noexcept {
        return window == nullptr ? nullptr : ::GetAncestor(window, GA_ROOT);
    }

    [[nodiscard]] static bool IsExplorerFrame(HWND window) noexcept {
        wchar_t name[64]{};
        return ::GetClassNameW(window, name, 64) != 0 &&
               ::lstrcmpiW(name, L"CabinetWClass") == 0;
    }

    [[nodiscard]] static bool IsShellTab(HWND window) noexcept {
        wchar_t name[64]{};
        return ::GetClassNameW(window, name, 64) != 0 &&
               ::lstrcmpiW(name, L"ShellTabWindowClass") == 0;
    }

    [[nodiscard]] static bool IsEligibleHost(HWND window,
                                             HWND exclude = nullptr) noexcept {
        if (window == nullptr || window == Root(exclude) ||
            Root(window) != window || !::IsWindowVisible(window) ||
            !IsExplorerFrame(window) ||
            ::GetPropW(window, PropertyName) != nullptr) {
            return false;
        }
        LONG_PTR style{};
        if (!ReadExStyle(window, style)) {
            return false;
        }
        if ((style & WS_EX_LAYERED) != 0) {
            COLORREF key{};
            BYTE alpha{};
            DWORD flags{};
            if (!::GetLayeredWindowAttributes(window, &key, &alpha, &flags) ||
                ((flags & LWA_ALPHA) != 0 && alpha == 0)) {
                return false;
            }
        }
        return true;
    }

   private:
    enum class State {
        Staging,
        Staged,
        Inspecting,
        Restoring,
        Retiring,
        Presenting,
        Finished
    };

    struct Entry final {
        Entry(HWND value, ULONG_PTR identity)
            : window(value), token(identity) {}

        const HWND window;
        const ULONG_PTR token;
        DWORD threadId{};
        DWORD processId{};
        LONG_PTR exStyle{};
        COLORREF colorKey{};
        BYTE alpha{255};
        DWORD layeredFlags{};
        bool wasLayered{};
        bool captured{};
        bool propertyInstalled{};
        bool presentationTouched{};
        bool restoreVisited{};
        std::mutex mutex;
        std::condition_variable changed;
        State state{State::Staging};
    };

    [[nodiscard]] static HANDLE TokenHandle(ULONG_PTR token) noexcept {
        return reinterpret_cast<HANDLE>(token);
    }

    [[nodiscard]] static std::optional<ULONG_PTR> AllocateToken() noexcept {
        // Never recycle a token, including on unsigned wraparound.
        static std::atomic<ULONG_PTR> next{1};
        ULONG_PTR value = next.load(std::memory_order_relaxed);
        while (value != (std::numeric_limits<ULONG_PTR>::max)()) {
            if (next.compare_exchange_weak(value, value + 1,
                                           std::memory_order_relaxed)) {
                return value;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] static DWORD LastErrorOr(DWORD fallback) noexcept {
        const DWORD error = ::GetLastError();
        return error == ERROR_SUCCESS ? fallback : error;
    }

    [[nodiscard]] static bool ReadExStyle(HWND window,
                                          LONG_PTR& value) noexcept {
        const DWORD savedError = ::GetLastError();
        ::SetLastError(ERROR_SUCCESS);
        value = ::GetWindowLongPtrW(window, GWL_EXSTYLE);
        const DWORD error = ::GetLastError();
        if (value == 0 && error != ERROR_SUCCESS) {
            ::SetLastError(error);
            return false;
        }
        ::SetLastError(savedError);
        return true;
    }

    [[nodiscard]] static bool WriteExStyle(HWND window,
                                           LONG_PTR value) noexcept {
        const DWORD savedError = ::GetLastError();
        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR previous =
            ::SetWindowLongPtrW(window, GWL_EXSTYLE, value);
        const DWORD error = ::GetLastError();
        if (previous == 0 && error != ERROR_SUCCESS) {
            ::SetLastError(error);
            return false;
        }
        ::SetLastError(savedError);
        return true;
    }

    [[nodiscard]] static bool SameWindow(const Entry& entry) noexcept {
        DWORD process{};
        const DWORD thread = ::GetWindowThreadProcessId(entry.window, &process);
        return thread != 0 && thread == entry.threadId &&
               process == entry.processId;
    }

    [[nodiscard]] static bool HasToken(const Entry& entry) noexcept {
        return SameWindow(entry) && ::GetPropW(entry.window, PropertyName) ==
                                        TokenHandle(entry.token);
    }

    [[nodiscard]] static bool RemoveOwnedProperty(const Entry& entry) noexcept {
        return HasToken(entry) && ::RemovePropW(entry.window, PropertyName) ==
                                      TokenHandle(entry.token);
    }

    [[nodiscard]] std::shared_ptr<Entry> Find(HWND window) const {
        std::lock_guard lock(entriesMutex_);
        const auto found = entries_.find(window);
        if (found == entries_.end()) {
            return {};
        }
        return found->second;
    }

    [[nodiscard]] std::shared_ptr<Entry> Find(HWND window,
                                              ULONG_PTR token) const {
        const auto entry = Find(window);
        return entry && entry->token == token ? entry
                                              : std::shared_ptr<Entry>{};
    }

    [[nodiscard]] static bool Begin(const std::shared_ptr<Entry>& entry,
                                    State operation) {
        std::lock_guard lock(entry->mutex);
        if (entry->state != State::Staged) {
            ::SetLastError(ERROR_BUSY);
            return false;
        }
        entry->state = operation;
        return true;
    }

    static void SetState(const std::shared_ptr<Entry>& entry, State state) {
        {
            std::lock_guard lock(entry->mutex);
            entry->state = state;
        }
        entry->changed.notify_all();
    }

    void Erase(const std::shared_ptr<Entry>& entry) {
        std::lock_guard lock(entriesMutex_);
        const auto found = entries_.find(entry->window);
        if (found != entries_.end() && found->second == entry) {
            entries_.erase(found);
        }
    }

    void Finish(const std::shared_ptr<Entry>& entry) {
        SetState(entry, State::Finished);
        Erase(entry);
    }

    [[nodiscard]] std::optional<ULONG_PTR> StageFailed(
        const std::shared_ptr<Entry>& entry,
        DWORD error) {
        if (entry->propertyInstalled && entry->presentationTouched) {
            (void)Restore(entry, false);
        } else {
            if (entry->propertyInstalled) {
                (void)RemoveOwnedProperty(*entry);
            }
            Finish(entry);
        }
        ::SetLastError(error);
        return std::nullopt;
    }

    [[nodiscard]] bool Restore(const std::shared_ptr<Entry>& entry, bool show) {
        if (!HasToken(*entry)) {
            Finish(entry);
            ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return false;
        }
        if (entry->captured && entry->presentationTouched) {
            if (!WriteExStyle(entry->window, entry->exStyle)) {
                SetState(entry, State::Staged);
                return false;
            }
            if (!HasToken(*entry)) {
                Finish(entry);
                ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
                return false;
            }
            if (entry->wasLayered && !::SetLayeredWindowAttributes(
                                         entry->window, entry->colorKey,
                                         entry->alpha, entry->layeredFlags)) {
                SetState(entry, State::Staged);
                return false;
            }
            if (!HasToken(*entry)) {
                Finish(entry);
                ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
                return false;
            }
            if (!::SetWindowPos(entry->window, nullptr, 0, 0, 0, 0,
                                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                    SWP_NOACTIVATE | SWP_FRAMECHANGED)) {
                SetState(entry, State::Staged);
                return false;
            }
        }

        // Intercepted ShowWindow calls must be allowed at this point. Keep the
        // token until after ShowWindow so a recycled HWND cannot be mistaken
        // for this window between the restore and the presentation call.
        SetState(entry, State::Presenting);
        if (HasToken(*entry)) {
            (void)::RedrawWindow(entry->window, nullptr, nullptr,
                                 RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
            if (show && HasToken(*entry)) {
                (void)::ShowWindow(entry->window, SW_SHOWNOACTIVATE);
            }
            (void)RemoveOwnedProperty(*entry);
        }
        Finish(entry);
        return true;
    }

    mutable std::mutex entriesMutex_;
    std::unordered_map<HWND, std::shared_ptr<Entry>> entries_;
    bool stopping_{};
    std::atomic_flag restoreSweepActive_ = ATOMIC_FLAG_INIT;
};

}  // namespace etr

static etr::WindowPresentationStore g_presentations;
struct MergeRequest {
    HWND stagingFrame;
    HWND destinationHint;
    ULONGLONG deadline = 0;
    ULONG_PTR lease = 0;
};

// Routing host and location queries are implemented by etr::ShellTabCatalog
// above.

#include <atomic>
#include <deque>
static std::atomic<bool> g_stop{false};
static HANDLE g_stopEvent = nullptr, g_wakeEvent = nullptr,
              g_readyEvent = nullptr, g_worker = nullptr;
static std::atomic<bool> g_workerReady{false};
static DWORD g_workerId = 0;
static std::mutex g_queueMutex;
static std::deque<MergeRequest*> g_redirectQueue;
struct FocusTask {
    HWND tab;
    ULONGLONG due;
};
static std::unordered_map<HWND, FocusTask> g_focusQueue;
static DWORD WINAPI ExecuteMergeRequest(LPVOID);
static void PumpFocus() {
    std::vector<FocusTask> ready;
    ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        for (auto i = g_focusQueue.begin(); i != g_focusQueue.end();) {
            if (!IsWindow(i->first) || !IsWindow(i->second.tab)) {
                i = g_focusQueue.erase(i);
                continue;
            }
            if (i->second.due <= now) {
                ready.push_back(i->second);
                i = g_focusQueue.erase(i);
            } else
                ++i;
        }
    }
    for (auto f : ready)
        if (!g_stop)
            ActivateShellTab(f.tab);
}
static bool CooperativeWait(DWORD ms) {
    ULONGLONG until = GetTickCount64() + ms;
    do {
        if (g_stop)
            return false;
        PumpFocus();
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        DWORD left = static_cast<DWORD>(std::min<ULONGLONG>(
            25, until > GetTickCount64() ? until - GetTickCount64() : 0));
        if (WaitForSingleObject(g_stopEvent, left) == WAIT_OBJECT_0)
            return false;
    } while (GetTickCount64() < until);
    return !g_stop;
}
static DWORD WINAPI RouterWorker(void*) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_workerReady = SUCCEEDED(hr);
    SetEvent(g_readyEvent);
    if (FAILED(hr))
        return 1;
    CoEnableCallCancellation(nullptr);
    while (!g_stop) {
        MergeRequest* next = nullptr;
        bool focusPending = false;
        {
            std::lock_guard<std::mutex> lock(g_queueMutex);
            if (!g_redirectQueue.empty()) {
                next = g_redirectQueue.front();
                g_redirectQueue.pop_front();
            }
            focusPending = !g_focusQueue.empty();
        }
        if (next) {
            if (!g_presentations.Owns(next->stagingFrame, next->lease)) {
                delete next;
                continue;
            }
            if (GetTickCount64() > next->deadline) {
                (void)g_presentations.Reveal(next->stagingFrame, next->lease);
                delete next;
            } else
                ExecuteMergeRequest(next);
            continue;
        }
        PumpFocus();
        HANDLE handles[] = {g_stopEvent, g_wakeEvent};
        if (MsgWaitForMultipleObjects(2, handles, FALSE,
                                      focusPending ? 25 : INFINITE,
                                      QS_ALLINPUT) == WAIT_OBJECT_0)
            break;
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    std::deque<MergeRequest*> pending;
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        pending.swap(g_redirectQueue);
        g_focusQueue.clear();
    }
    for (auto* p : pending) {
        if (g_presentations.Owns(p->stagingFrame, p->lease))
            (void)g_presentations.Reveal(p->stagingFrame, p->lease);
        delete p;
    }
    CoDisableCallCancellation(nullptr);
    CoUninitialize();
    return 0;
}
static bool StartRouter() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent && g_wakeEvent && g_readyEvent)
        g_worker =
            CreateThread(nullptr, 0, RouterWorker, nullptr, 0, &g_workerId);
    if (!g_worker)
        return false;
    WaitForSingleObject(g_readyEvent, INFINITE);
    return g_workerReady;
}
static void StopRouter() {
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        g_stop = true;
    }
    if (g_stopEvent)
        SetEvent(g_stopEvent);
    if (g_worker) {
        CoCancelCall(g_workerId, 0);
        WaitForSingleObject(g_worker, INFINITE);
    }
    g_presentations.RestoreAll();
}
static void CloseRouter() {
    if (g_worker)
        CloseHandle(g_worker);
    if (g_stopEvent)
        CloseHandle(g_stopEvent);
    if (g_wakeEvent)
        CloseHandle(g_wakeEvent);
    if (g_readyEvent)
        CloseHandle(g_readyEvent);
}
static void QueueRedirect(MergeRequest* info) {
    bool queued = false;
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        if (!g_stop && g_redirectQueue.size() < 16) {
            info->deadline = GetTickCount64() + 8000;
            g_redirectQueue.push_back(info);
            queued = true;
            SetEvent(g_wakeEvent);
        }
    }
    if (!queued) {
        (void)g_presentations.Reveal(info->stagingFrame, info->lease);
        delete info;
    }
}
static HWND FastPrimary(HWND exclude) {
    struct Search {
        HWND exclude, result;
        DWORD pid;
    } search{exclude, nullptr, GetCurrentProcessId()};
    EnumWindows(
        [](HWND h, LPARAM p) -> BOOL {
            auto* s = reinterpret_cast<Search*>(p);
            WCHAR cls[64]{};
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            BYTE alpha = 255;
            DWORD flags = 0;
            bool hidden =
                GetLayeredWindowAttributes(h, nullptr, &alpha, &flags) &&
                (flags & LWA_ALPHA) && alpha == 0;
            if (h != s->exclude && pid == s->pid && !hidden &&
                IsWindowVisible(h) && GetClassNameW(h, cls, 64) &&
                !wcscmp(cls, L"CabinetWClass")) {
                s->result = h;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));
    return search.result;
}

static void QueueFocus(HWND tab) {
    if (g_stop)
        return;
    HWND frame = GetAncestor(tab, GA_ROOT);
    std::lock_guard<std::mutex> lock(g_queueMutex);
    if (!g_stop) {
        g_focusQueue[frame] = {tab, GetTickCount64() + 120};
        SetEvent(g_wakeEvent);
    }
}

// Serialize redirects and identify the newly created tab by HWND, never by
// global IShellWindows counts (which change while other requests are arriving).
#include <mutex>

static ComPtr<IWebBrowser2> BrowserForTab(HWND wanted) {
    ComPtr<IShellWindows> windows;
    CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                     IID_PPV_ARGS(&windows));
    long count = 0;
    if (windows)
        windows->get_Count(&count);
    for (long i = 0; i < count; ++i) {
        VARIANT index{};
        index.vt = VT_I4;
        index.lVal = i;
        ComPtr<IDispatch> dispatch;
        windows->Item(index, &dispatch);
        if (!dispatch)
            continue;
        auto browser = GetTabBrowser(dispatch.Get());
        HWND tab = nullptr;
        if (browser && SUCCEEDED(browser->GetWindow(&tab)) && tab == wanted) {
            ComPtr<IWebBrowser2> result;
            dispatch.As(&result);
            return result;
        }
    }
    return nullptr;
}
static std::vector<PIDLIST_ABSOLUTE> CaptureSelection(HWND frame) {
    std::vector<PIDLIST_ABSOLUTE> result;
    HWND tab = FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
    auto wb = BrowserForTab(tab);
    if (!wb)
        return result;
    auto browser = GetTabBrowser(wb.Get());
    ComPtr<IShellView> view;
    if (!browser || FAILED(browser->QueryActiveShellView(&view)))
        return result;
    ComPtr<IFolderView> fv;
    view.As(&fv);
    ComPtr<IShellItemArray> items;
    if (!fv || FAILED(fv->Items(SVGIO_SELECTION, IID_PPV_ARGS(&items))))
        return result;
    DWORD count = 0;
    items->GetCount(&count);
    for (DWORD i = 0; i < count; ++i) {
        ComPtr<IShellItem> item;
        items->GetItemAt(i, &item);
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (item && SUCCEEDED(SHGetIDListFromObject(item.Get(), &pidl)) && pidl)
            result.push_back(pidl);
    }
    return result;
}
static void ApplySelection(HWND tab,
                           const std::vector<PIDLIST_ABSOLUTE>& selection) {
    auto wb = BrowserForTab(tab);
    if (!wb)
        return;
    auto browser = GetTabBrowser(wb.Get());
    ComPtr<IShellView> view;
    if (!browser || FAILED(browser->QueryActiveShellView(&view)))
        return;
    for (size_t i = 0; i < selection.size(); ++i)
        view->SelectItem(
            ILFindLastID(selection[i]),
            SVSI_SELECT | SVSI_ENSUREVISIBLE |
                (i == 0 ? (SVSI_DESELECTOTHERS | SVSI_FOCUSED) : 0));
}

static DWORD WINAPI ExecuteMergeRequest(LPVOID parameter) {
    auto* info = static_cast<MergeRequest*>(parameter);
    HWND source = info->stagingFrame, primary = info->destinationHint;
    ULONG_PTR token = info->lease;
    delete info;
    if (!g_presentations.Owns(source, token))
        return 0;
    auto AbortOwned = [&]() {
        if (g_presentations.Owns(source, token))
            (void)g_presentations.Reveal(source, token);
    };

    if (g_stop) {
        AbortOwned();
        return 0;
    }
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(init)) {
        AbortOwned();
        return 1;
    }
    etr::ShellTabCatalog catalog;
    std::optional<etr::FolderLocation> destination;
    for (int attempt = 0; attempt < 100 && g_presentations.Owns(source, token);
         ++attempt) {
        if (!CooperativeWait(50)) {
            AbortOwned();
            CoUninitialize();
            return 0;
        }
        destination = catalog.ReadFrameDestination(source);
        if (destination)
            break;
    }
    primary = catalog.ChooseRoutingHost(source, primary);
    if (!destination || !primary || primary == source ||
        etr::ClassifyShellDestination(destination->address) !=
            etr::DestinationKind::Folder) {
        AbortOwned();
        CoUninitialize();
        return 0;
    }
    if (ConsumeExplicitWindow(destination->address)) {
        AbortOwned();
        CoUninitialize();
        return 0;
    }
    // Preserve the Shell identity from the view; do not stringify and reparse
    // it.
    PIDLIST_ABSOLUTE folder = destination->identity.release();
    struct FolderGuard {
        PIDLIST_ABSOLUTE& p;
        ~FolderGuard() {
            if (p)
                CoTaskMemFree(p);
        }
    } folderGuard{folder};
    // Allow the originating reveal request to deliver its selection.
    if (!CooperativeWait(10)) {
        AbortOwned();
        CoUninitialize();
        return 0;
    }
    auto selection = CaptureSelection(source);
    struct SelectionGuard {
        std::vector<PIDLIST_ABSOLUTE>& s;
        ~SelectionGuard() {
            for (auto p : s)
                CoTaskMemFree(p);
        }
    } selectionGuard{selection};
    std::vector<PCUITEMID_CHILD> children;
    for (auto pidl : selection)
        children.push_back(ILFindLastID(pidl));
    bool complete = ReuseFolderTab(folder, static_cast<UINT>(children.size()),
                                   children.data(), 0, source);
    if (!complete) {
        std::vector<HWND> before;
        for (HWND t = nullptr;
             (t = FindWindowExW(primary, t, L"ShellTabWindowClass", nullptr));)
            before.push_back(t);
        HWND active =
            FindWindowExW(primary, nullptr, L"ShellTabWindowClass", nullptr);
        HWND created = nullptr;
        if (active)
            PostMessageW(active, WM_COMMAND, CMD_NEW_TAB, 0);
        ComPtr<IWebBrowser2> wb;
        for (int i = 0; i < 60 && IsWindow(primary); ++i) {
            if (!CooperativeWait(50)) {
                AbortOwned();
                CoUninitialize();
                return 0;
            }
            for (HWND t = nullptr;
                 (t = FindWindowExW(primary, t, L"ShellTabWindowClass",
                                    nullptr));) {
                if (std::find(before.begin(), before.end(), t) ==
                    before.end()) {
                    created = t;
                    break;
                }
            }
            if (created) {
                wb = BrowserForTab(created);
                if (wb)
                    break;
            }
        }
        if (wb) {
            VARIANT target{}, empty{};
            struct VariantGuard {
                VARIANT* v;
                ~VariantGuard() { VariantClear(v); }
            } variantGuard{&target};
            target.vt = VT_ARRAY | VT_UI1;
            target.parray = SafeArrayCreateVector(VT_UI1, 0, ILGetSize(folder));
            if (target.parray) {
                void* data = nullptr;
                if (SUCCEEDED(SafeArrayAccessData(target.parray, &data))) {
                    memcpy(data, folder, ILGetSize(folder));
                    SafeArrayUnaccessData(target.parray);
                    HRESULT hr =
                        wb->Navigate2(&target, &empty, &empty, &empty, &empty);
                    if (SUCCEEDED(hr))
                        for (int i = 0; i < 80; ++i) {
                            if (!CooperativeWait(50)) {
                                AbortOwned();
                                CoUninitialize();
                                return 0;
                            }
                            auto b = GetTabBrowser(wb.Get());
                            ComPtr<IShellView> v;
                            if (b && SUCCEEDED(b->QueryActiveShellView(&v)) &&
                                v && SameFolder(v.Get(), folder)) {
                                // Capture again after navigation, before
                                // retiring the source.
                                auto late = CaptureSelection(source);
                                if (!late.empty()) {
                                    for (auto p : selection)
                                        CoTaskMemFree(p);
                                    selection = std::move(late);
                                }
                                ApplySelection(created, selection);
                                complete = ActivateShellTab(created);
                                break;
                            }
                        }
                }
                VariantClear(&target);
            }
        }
    }
    for (auto p : selection)
        CoTaskMemFree(p);
    selection.clear();
    CoTaskMemFree(folder);
    folder = nullptr;
    if (complete)
        (void)g_presentations.Retire(source, token);
    else if (g_presentations.Owns(source, token))
        AbortOwned();
    CoUninitialize();
    return 0;
}

// Hook: CreateWindowExW

#include <windows.h>
#include <string_view>

namespace etr {
enum class CreatedWindowRole { Other, ExplorerFrame, ExplorerTab };

inline CreatedWindowRole DescribeWindowClass(LPCWSTR name) noexcept {
    if (!name || IS_INTRESOURCE(name))
        return CreatedWindowRole::Other;
    const std::wstring_view value{name};
    if (value == L"CabinetWClass")
        return CreatedWindowRole::ExplorerFrame;
    if (value == L"ShellTabWindowClass")
        return CreatedWindowRole::ExplorerTab;
    return CreatedWindowRole::Other;
}

class ConstructionVisibilityScope {
    inline static thread_local unsigned pending_ = 0;
    bool active_;

   public:
    explicit ConstructionVisibilityScope(bool active) noexcept
        : active_(active) {
        if (active_)
            ++pending_;
    }
    ~ConstructionVisibilityScope() {
        if (active_)
            --pending_;
    }
    static bool Pending() noexcept { return pending_ != 0; }
    ConstructionVisibilityScope(const ConstructionVisibilityScope&) = delete;
    ConstructionVisibilityScope& operator=(const ConstructionVisibilityScope&) =
        delete;
};
}  // namespace etr

static decltype(&CreateWindowExW) g_nextWindowBuild = nullptr;
static decltype(&ShowWindow) g_nextVisibilityChange = nullptr;

static HWND WINAPI RouteWindowCreation(DWORD extendedStyle,
                                       LPCWSTR classIdentifier,
                                       LPCWSTR caption,
                                       DWORD style,
                                       int left,
                                       int top,
                                       int width,
                                       int height,
                                       HWND owner,
                                       HMENU menu,
                                       HINSTANCE module,
                                       LPVOID context) {
    const auto role = etr::DescribeWindowClass(classIdentifier);
    const bool explicitFrame =
        role == etr::CreatedWindowRole::ExplorerFrame && g_explicitWindowCall;
    const bool permitRouting = role == etr::CreatedWindowRole::ExplorerFrame &&
                               !g_stop.load() && !explicitFrame &&
                               (GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0;
    HWND destinationHint = permitRouting ? FastPrimary(nullptr) : nullptr;
    HWND result = nullptr;
    DWORD nativeError = ERROR_SUCCESS;
    {
        etr::ConstructionVisibilityScope constructing(destinationHint !=
                                                      nullptr);
        result = g_nextWindowBuild(extendedStyle, classIdentifier, caption,
                                   style, left, top, width, height, owner, menu,
                                   module, context);
        nativeError = GetLastError();
    }
    if (result) {
        if (explicitFrame)
            g_explicitWindowCreated = true;
        if (role == etr::CreatedWindowRole::ExplorerTab && !g_stop.load())
            ScheduleNewTab(result);
        if (destinationHint) {
            auto lease =
                !g_stop.load() ? g_presentations.Stage(result) : std::nullopt;
            if (lease) {
                auto* request = new (std::nothrow)
                    MergeRequest{result, destinationHint, 0, *lease};
                if (request)
                    QueueRedirect(request);
                else
                    (void)g_presentations.Reveal(result, *lease);
            } else if (IsWindow(result)) {
                // Construction may have deferred a native show request. A
                // staging refusal must leave the newly built frame usable.
                g_nextVisibilityChange(result, SW_SHOWNOACTIVATE);
            }
        }
    }
    SetLastError(nativeError);
    return result;
}

static BOOL WINAPI RouteWindowVisibility(HWND window, int request) {
    if (!g_stop.load() &&
        etr::WindowPresentationStore::IsExplorerFrame(window) &&
        (etr::ConstructionVisibilityScope::Pending() ||
         g_presentations.IsStaged(window)))
        return TRUE;
    return g_nextVisibilityChange(window, request);
}

// Mod lifecycle

using FindWindowSW_t = HRESULT(STDMETHODCALLTYPE*)(void*,
                                                   VARIANT*,
                                                   VARIANT*,
                                                   int,
                                                   LONG*,
                                                   int,
                                                   IDispatch**);
static FindWindowSW_t FindWindowSW_Original;
static HRESULT STDMETHODCALLTYPE FindWindowSW_Hook(void* self,
                                                   VARIANT* loc,
                                                   VARIANT* root,
                                                   int cls,
                                                   LONG* frame,
                                                   int options,
                                                   IDispatch** out) {
    HRESULT hr =
        FindWindowSW_Original(self, loc, root, cls, frame, options, out);
    Wh_Log(L"FindWindowSW cls=%d options=%d vt=%d hr=%08lx out=%d", cls,
           options, loc ? loc->vt : 0, hr, out && *out ? 1 : 0);
    if (SUCCEEDED(hr) && hr != S_FALSE && out && *out && cls != SWC_DESKTOP &&
        (options & SWFO_NEEDDISPATCH) &&
        !(GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
        auto browser = GetTabBrowser(*out);
        HWND tab = nullptr;
        if (browser && SUCCEEDED(browser->GetWindow(&tab)) && tab) {
            // Return from the ShellWindows RPC before issuing frame commands;
            // the UI command can itself need a Shell/WinRT apartment callback.
            QueueFocus(tab);
            Wh_Log(L"Queued activation for tab=%p", tab);
        }
    }
    return hr;
}

static bool InstallExplorerFrameHooks(bool shellService) {
    HMODULE module = LoadLibraryExW(L"ExplorerFrame.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return false;
    }
    // ExplorerFrame.dll
    WindhawkUtils::SYMBOL_HOOK shellHooks[] = {
        {{L"public: virtual long __cdecl CSDWindows::FindWindowSW(struct "
          L"tagVARIANT *,struct tagVARIANT *,int,long *,int,struct IDispatch * "
          L"*)"},
         &FindWindowSW_Original,
         FindWindowSW_Hook},
        {{L"public: virtual long __cdecl CExecuteFolder::Initialize(unsigned "
          L"short const *,struct IPropertyBag *)"},
         &FolderInitialize_Original,
         FolderInitialize_Hook},
        {{L"protected: long __cdecl CExecuteFolder::ShowFolder(struct "
          L"_ITEMIDLIST_ABSOLUTE const *,bool)"},
         &FolderShow_Original,
         FolderShow_Hook},
    };
    const size_t count = shellService ? 1 : std::size(shellHooks);
    return WindhawkUtils::HookSymbols(module, shellHooks, count);
}

BOOL Wh_ModInit() {
    wchar_t image[MAX_PATH]{};
    GetModuleFileNameW(nullptr, image, static_cast<DWORD>(std::size(image)));
    const bool shellService =
        _wcsicmp(PathFindFileNameW(image), L"rundll32.exe") == 0;
    if (shellService) {
        if (!etr::IsShellWindowsServiceCommand(GetCommandLineW()))
            return FALSE;
        if (!InstallExplorerFrameHooks(true))
            return FALSE;
        if (!StartRouter()) {
            StopRouter();
            CloseRouter();
            return FALSE;
        }
        return TRUE;
    }
    if (!InstallExplorerFrameHooks(false))
        return FALSE;
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                            reinterpret_cast<void*>(RouteWindowCreation),
                            reinterpret_cast<void**>(&g_nextWindowBuild)))
        return FALSE;
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(ShowWindow),
                            reinterpret_cast<void*>(RouteWindowVisibility),
                            reinterpret_cast<void**>(&g_nextVisibilityChange)))
        return FALSE;
    if (!StartRouter()) {
        StopRouter();
        CloseRouter();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    StopRouter();
}
void Wh_ModUninit() {
    CloseRouter();
}
