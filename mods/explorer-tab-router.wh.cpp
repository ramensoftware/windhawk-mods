// ==WindhawkMod==
// @id              explorer-tab-router
// @name            Explorer Tab Router
// @name:zh-CN      资源管理器页签路由
// @description     Reuse and activate Explorer folder tabs, preserve file selection and explicit new windows
// @description:zh-CN 复用并切换资源管理器页签，保留文件选中状态和明确的新窗口操作
// @version         1.1.0
// @author          afate123
// @github          https://github.com/afate123
// @license         MIT
// @architecture    x86-64
// @include         explorer.exe
// @include         rundll32.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luuid -lbcrypt
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
- **Open in new tab from a folder context menu:** create a new tab and switch to it automatically, including when that folder already has another tab. Middle-click, background tab creation, and restored tabs keep their native activation behavior.
- **Open in new window:** keep the explicitly requested window separate.
- **Hold Shift while opening a folder:** bypass ordinary window-to-tab routing and keep a separate window.

Enable the mod in Windhawk, then use File Explorer and other applications normally. Folder routing and tab activation do not require a separate application list. Control Panel and unsupported shell locations are allowed to remain in their own windows.

### Settings

Window routing, matching-tab reuse during routing, folder-menu new-tab activation, and ShellWindows lookup activation can be enabled independently. All four are enabled by default.

The lookup compatibility option is needed by some external folder-opening and file-reveal requests. The ShellWindows query interface does not say whether a caller intends to open a folder or only inspect it. With this option enabled, a successful read-only query can also switch tabs. Disable it if query-triggered activation is undesirable; some external requests will then no longer switch an existing tab. Delayed lookup activation is canceled if the user switches to another foreground window or selects a different tab before it runs.

### Source and conflicting mods

This mod builds on the routing approach introduced by [Explorer Single Window Tabs](https://windhawk.net/mods/explorer-single-window-tabs) by [ALMAS CP](https://github.com/almas-cp). It adds existing-tab reuse and activation, file-selection preservation, explicit-window handling, automatic new-tab activation, and managed presentation recovery.

**Do not enable Explorer Single Window Tabs and Explorer Tab Router together. Disable the other routing mod first.**

### Compatibility and limitations

Windows 11 x64 with native File Explorer tabs is required. This mod uses internal shell symbols, which are checked when it loads. A future Windows update can require a compatibility update.

Ctrl+N and dragging a tab into a separate window can create frames without the explicit folder-menu intent this mod recognizes. Those frames may be routed back into tabs. Disable window routing when you need to retain them as separate windows.

Some requests are handled through a temporary transparent Explorer window before routing completes. This is not a guarantee of entirely window-free navigation. If a request cannot be completed safely, the temporary window is restored instead of discarding the request. Window presentation state, including existing layered transparency, is restored on recovery.

Tab activation uses verified position hints where available. A first activation or a tab reorder can still require intermediate tab switches. Activation has an overall time limit; a routing failure restores the original window. Navigation completion uses browser events with bounded direct-view checks for providers that omit events.

The mod is distributed under the MIT license.

## 简体中文

资源管理器页签路由将普通的文件夹打开请求集中到 Windows 11 资源管理器页签中，同时保留明确要求打开独立窗口的操作。

### 使用行为

- **从外部应用打开文件夹：** 已有相同目录页签时，复用并切换过去；尚未打开时，将新目录窗口转入现有资源管理器的页签。
- **在文件夹中显示文件：** 切换到对应目录页签，并保留目标文件的选中状态。
- **文件夹右键菜单中的“在新标签页中打开”：** 新建页签并自动切换，即使相同目录已有其他页签。中键、其他后台创建及启动恢复的页签保持系统原有的激活行为。
- **在新窗口中打开：** 保留明确请求的独立窗口。
- **按住 Shift 打开文件夹：** 绕过普通窗口转页签行为，保留独立窗口。

在 Windhawk 中启用后即可正常使用，不需要逐个添加应用名单。控制面板及不支持的 Shell 位置可以保持独立窗口。

### 设置

窗口转页签、路由时复用相同目录、文件夹菜单新页签自动激活、ShellWindows 查询命中后激活，可分别开关；默认全部启用。

查询兼容开关用于部分外部打开目录和文件定位请求。ShellWindows 查询接口不区分“打开文件夹”和“只检查是否已打开”，因此启用后，成功的纯查询也可能切换页签。若不希望查询触发切换，可关闭此项；部分外部请求也会因此不再自动切换已有页签。延迟执行查询激活前，若用户已转到其他前台窗口或切换了不同页签，会取消这次延迟激活。

### 来源与冲突提示

本 mod 基于 [ALMAS CP](https://github.com/almas-cp) 的 [Explorer Single Window Tabs](https://windhawk.net/mods/explorer-single-window-tabs) 路由思路扩展，增加已有页签复用与切换、文件选中保留、明确的新窗口请求识别、新页签自动激活，以及窗口状态恢复管理。

**请勿同时启用 Explorer Single Window Tabs 与 Explorer Tab Router；启用前先停用另一个路由 mod。**

### 兼容性与限制

需要 Windows 11 x64 以及支持原生页签的资源管理器。加载时会检查所需的系统内部符号，未来 Windows 更新可能需要同步更新兼容实现。

Ctrl+N 和将页签拖出为独立窗口，可能不携带本 mod 能识别的文件夹菜单意图，因此产生的窗口仍可能被合并回页签；需要保留这些独立窗口时，可关闭窗口转页签设置。

部分请求会先经过临时透明窗口，再完成页签路由，因此不保证整个过程完全不创建窗口。无法安全完成时会恢复临时窗口，避免丢弃打开请求；恢复时会还原原有窗口显示状态，包括已有的分层透明属性。

页签切换会复用经过验证的位置记录；首次切换或重新排序后仍可能短暂经过其他页签。激活设有总超时限制，路由失败时恢复原窗口。导航完成优先使用浏览器事件；缺少事件时只进行有限次直接视图检查。

本 mod 使用 MIT 许可。

*/
// ==/WindhawkModReadme==
// clang-format on

// clang-format off
// ==WindhawkModSettings==
/*
- mergeWindows: true
  $name: Route ordinary folder windows into tabs
  $name:zh-CN: 将普通文件夹窗口转入页签
- reuseTabs: true
  $name: Reuse matching tabs while routing
  $name:zh-CN: 路由时复用已有相同目录页签
- activateNewTabs: true
  $name: Activate new tabs opened from folder context menus
  $name:zh-CN: 自动切换文件夹右键菜单打开的新页签
- activateShellLookups: true
  $name: Activate shell lookup matches (compatibility)
  $name:zh-CN: 激活 Shell 查询命中的页签（兼容功能）
  $description: Needed by some external open/reveal requests. Shell lookups do not identify user intent, so a read-only query can also activate a matching tab. Disable to prevent lookup-triggered activation.
  $description:zh-CN: 部分外部打开和文件定位请求需要此功能。Shell 查询不提供用户意图，因此纯查询也可能激活对应页签；关闭后不再由查询触发切换。
*/
// ==/WindhawkModSettings==
// clang-format on

#include <bcrypt.h>
#include <exdisp.h>
#include <exdispid.h>
#include <ocidl.h>
#include <olectl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <wrl/client.h>
#include <cwchar>
#include <iterator>

#include <algorithm>
#include <array>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct RouterSettings {
    std::atomic<bool> mergeWindows{true};
    std::atomic<bool> reuseTabs{true};
    std::atomic<bool> activateNewTabs{true};
    std::atomic<bool> activateShellLookups{true};
};
static RouterSettings g_settings;
static void LoadSettings() {
    g_settings.mergeWindows = Wh_GetIntSetting(L"mergeWindows") != 0;
    g_settings.reuseTabs = Wh_GetIntSetting(L"reuseTabs") != 0;
    g_settings.activateNewTabs = Wh_GetIntSetting(L"activateNewTabs") != 0;
    g_settings.activateShellLookups =
        Wh_GetIntSetting(L"activateShellLookups") != 0;
}

namespace etr {
inline bool HasWindowClass(HWND window, PCWSTR expected) noexcept {
    wchar_t name[64]{};
    return GetClassNameW(window, name, static_cast<int>(std::size(name))) &&
           wcscmp(name, expected) == 0;
}
inline bool IsStagingFrame(HWND window) noexcept {
    if (GetPropW(window, L"ETR.Redirecting"))
        return true;
    BYTE alpha = 255;
    DWORD flags = 0;
    return GetLayeredWindowAttributes(window, nullptr, &alpha, &flags) &&
           (flags & LWA_ALPHA) && alpha == 0;
}
inline bool CanHostFrame(HWND frame, HWND excluded = nullptr) noexcept {
    return frame && frame != GetAncestor(excluded, GA_ROOT) &&
           HasWindowClass(frame, L"CabinetWClass") && IsWindowVisible(frame) &&
           !IsStagingFrame(frame);
}
}  // namespace etr

// Link bcrypt, ole32 and shell32. Win11 x64, same desktop / integrity level.
// Initialize once before publishing the instance to other threads. Publish
// anchors must be top-level windows. Explicitly call Close before destroying
// the instance; its destructor deliberately performs no Win32 cleanup. Caller
// must join users before destruction. The 128 property cap is a soft concurrent
// limit: racing publishers can transiently exceed it by the number of in-flight
// publishers.
namespace etr {
enum class IntentPropertyKind { Window, Tab };

class IntentPropertyChannel {
    static constexpr wchar_t kNamespace[] = L"ETR.Intent.1.";
    static constexpr ULONGLONG kLifetime = 10000;
    static constexpr size_t kCapacity = 128;
    std::atomic<bool> ready_{false};
    std::atomic<bool> closing_{false};
    std::atomic<ULONGLONG> sequence_{0};
    wchar_t
        origin_[40]{};  // Immutable after Initialize; Close never clears it.

    struct Candidate {
        std::wstring name;
        ULONGLONG expiry;
    };
    struct Snapshot {
        std::vector<Candidate> items;
        const wchar_t* prefix = kNamespace;
        size_t prefixLength = std::size(kNamespace) - 1;
        bool failed = false;
    };
    static BOOL CALLBACK Collect(HWND,
                                 LPWSTR name,
                                 HANDLE value,
                                 ULONG_PTR data) noexcept {
        auto& snapshot = *reinterpret_cast<Snapshot*>(data);
        if (IS_INTRESOURCE(name) ||
            std::wcsncmp(name, snapshot.prefix, snapshot.prefixLength))
            return TRUE;
        try {
            snapshot.items.push_back(
                {name,
                 static_cast<ULONGLONG>(reinterpret_cast<ULONG_PTR>(value))});
        } catch (...) {
            snapshot.failed = true;
            return FALSE;
        }
        return TRUE;
    }
    static bool Read(HWND anchor, Snapshot& snapshot) noexcept {
        // EnumPropsEx returns -1 for a window with no properties as well as
        // some errors; SetProp/RemoveProp remain the authority for those cases.
        EnumPropsExW(anchor, Collect, reinterpret_cast<LPARAM>(&snapshot));
        return !snapshot.failed;
    }
    static bool ValidKind(IntentPropertyKind kind) noexcept {
        return kind == IntentPropertyKind::Window ||
               kind == IntentPropertyKind::Tab;
    }
    static wchar_t KindLetter(IntentPropertyKind kind) noexcept {
        return kind == IntentPropertyKind::Window ? L'W' : L'T';
    }
    static bool Hash(PCIDLIST_ABSOLUTE pidl, wchar_t (&hex)[65]) noexcept {
        if (!pidl)
            return false;
        // System-provided, valid PIDLs are required. Include the terminating
        // item.
        const UINT bytes = ILGetSize(pidl);
        if (bytes < sizeof(USHORT))
            return false;
        BCRYPT_ALG_HANDLE algorithm = nullptr;
        BCRYPT_HASH_HANDLE hash = nullptr;
        unsigned char digest[32]{};
        NTSTATUS status = BCryptOpenAlgorithmProvider(
            &algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
        if (status >= 0)
            status =
                BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0);
        if (status >= 0)
            status = BCryptHashData(
                hash, reinterpret_cast<PUCHAR>(const_cast<ITEMIDLIST*>(pidl)),
                bytes, 0);
        if (status >= 0)
            status = BCryptFinishHash(hash, digest, sizeof(digest), 0);
        if (hash)
            BCryptDestroyHash(hash);
        if (algorithm)
            BCryptCloseAlgorithmProvider(algorithm, 0);
        if (status < 0)
            return false;
        constexpr wchar_t digits[] = L"0123456789abcdef";
        for (size_t i = 0; i < sizeof(digest); ++i) {
            hex[2 * i] = digits[digest[i] >> 4];
            hex[2 * i + 1] = digits[digest[i] & 15];
        }
        hex[64] = L'\0';
        return true;
    }
    static std::wstring Prefix(IntentPropertyKind kind, const wchar_t* hash) {
        std::wstring name = kNamespace;
        name += KindLetter(kind);
        name += L'.';
        name += hash;
        name += L'.';
        return name;
    }
    ULONGLONG Next() noexcept {
        ULONGLONG previous = sequence_.load(std::memory_order_relaxed);
        do {
            if (previous == std::numeric_limits<ULONGLONG>::max())
                return 0;
        } while (!sequence_.compare_exchange_weak(previous, previous + 1,
                                                  std::memory_order_relaxed));
        return previous + 1;
    }
    static size_t Prune(HWND anchor,
                        Snapshot& snapshot,
                        ULONGLONG now) noexcept {
        size_t active = 0;
        for (const auto& item : snapshot.items) {
            if (!item.expiry || item.expiry <= now)
                RemovePropW(anchor, item.name.c_str());
            else
                ++active;
        }
        return active;
    }
    struct Scan {
        IntentPropertyChannel* self;
        const wchar_t* prefix;
        size_t prefixLength;
        ULONGLONG now;
        bool consumed = false;
    };
    static BOOL CALLBACK ConsumeWindow(HWND window, LPARAM data) noexcept {
        auto& scan = *reinterpret_cast<Scan*>(data);
        if (HasWindowClass(window, L"CabinetWClass"))
            scan.consumed = scan.self->ConsumePrefix(
                window, scan.prefix, scan.prefixLength, scan.now);
        return !scan.consumed;
    }
    bool ConsumePrefix(HWND anchor,
                       const wchar_t* prefix,
                       size_t length,
                       ULONGLONG now) noexcept {
        if (Closing())
            return false;
        Snapshot snapshot;
        snapshot.prefix = prefix;
        snapshot.prefixLength = length;
        if (!Read(anchor, snapshot))
            return false;
        for (const auto& item : snapshot.items) {
            if (Closing())
                return false;
            // RemoveProp atomically returns the exact unique-name ticket.
            const auto expiry =
                static_cast<ULONGLONG>(reinterpret_cast<ULONG_PTR>(
                    RemovePropW(anchor, item.name.c_str())));
            if (expiry > now)
                return true;
        }
        return false;
    }
    struct Cleanup {
        const IntentPropertyChannel* self;
        bool all;
        IntentPropertyKind kind;
        ULONGLONG token;
    };
    static BOOL CALLBACK CleanupWindow(HWND window, LPARAM data) noexcept {
        auto& cleanup = *reinterpret_cast<Cleanup*>(data);
        // The callback catches every allocation failure. No property is changed
        // during EnumPropsEx itself; partial snapshots are safe to clean too.
        try {
            Snapshot snapshot;
            Read(window, snapshot);
            if (snapshot.items.empty())
                return TRUE;
            std::wstring suffix = L".";
            suffix += cleanup.self->origin_;
            suffix += L'.';
            if (!cleanup.all)
                suffix += std::to_wstring(cleanup.token);
            for (const auto& item : snapshot.items) {
                constexpr size_t kindOffset = std::size(kNamespace) - 1;
                constexpr size_t originOffset = kindOffset + 2 + 64;
                if (item.name.size() <= originOffset ||
                    item.name[originOffset] != L'.')
                    continue;
                if (cleanup.all) {
                    if (item.name.compare(originOffset, suffix.size(),
                                          suffix) == 0)
                        RemovePropW(window, item.name.c_str());
                } else if (item.name[kindOffset] == KindLetter(cleanup.kind) &&
                           item.name.compare(originOffset, std::wstring::npos,
                                             suffix) == 0)
                    RemovePropW(window, item.name.c_str());
            }
        } catch (...) { /* Best effort; tickets expire independently in 10s. */
        }
        return TRUE;
    }

   public:
    IntentPropertyChannel() noexcept = default;
    ~IntentPropertyChannel() = default;  // Never perform Win32 calls at unload.
    IntentPropertyChannel(const IntentPropertyChannel&) = delete;
    IntentPropertyChannel& operator=(const IntentPropertyChannel&) = delete;

    bool Initialize() noexcept {
        static_assert(sizeof(ULONG_PTR) == sizeof(ULONGLONG),
                      "Intent channel requires x64");
        if (Closing())
            return false;
        if (Ready())
            return true;
        GUID guid{};
        if (FAILED(CoCreateGuid(&guid)) ||
            !StringFromGUID2(guid, origin_,
                             static_cast<int>(std::size(origin_))))
            return false;
        ready_.store(true, std::memory_order_release);
        return true;
    }
    bool Ready() const noexcept {
        return ready_.load(std::memory_order_acquire) && !Closing();
    }
    bool Closing() const noexcept {
        return closing_.load(std::memory_order_acquire);
    }

    ULONGLONG Publish(HWND anchor,
                      IntentPropertyKind kind,
                      PCIDLIST_ABSOLUTE pidl,
                      ULONGLONG now) noexcept {
        if (!Ready() || !anchor || !ValidKind(kind) ||
            now > std::numeric_limits<ULONGLONG>::max() - kLifetime)
            return 0;
        wchar_t hash[65];
        if (!Hash(pidl, hash))
            return 0;
        try {
            Snapshot before;
            if (!Read(anchor, before) ||
                Prune(anchor, before, now) >= kCapacity || Closing())
                return 0;
            const ULONGLONG token = Next();
            if (!token)
                return 0;
            std::wstring name = Prefix(kind, hash);
            name += origin_;
            name += L'.';
            name += std::to_wstring(token);
            if (Closing() ||
                !SetPropW(anchor, name.c_str(),
                          reinterpret_cast<HANDLE>(
                              static_cast<ULONG_PTR>(now + kLifetime))))
                return 0;
            // Always undo a late insertion if Close raced the enumeration.
            if (Closing()) {
                RemovePropW(anchor, name.c_str());
                return 0;
            }
            Snapshot after;
            if (!Read(anchor, after) || Prune(anchor, after, now) > kCapacity ||
                Closing()) {
                RemovePropW(anchor, name.c_str());
                return 0;
            }
            return token;
        } catch (...) {
            return 0;
        }
    }
    bool ConsumeAt(HWND anchor,
                   IntentPropertyKind kind,
                   PCIDLIST_ABSOLUTE pidl,
                   ULONGLONG now) noexcept {
        if (!Ready() || !anchor || !ValidKind(kind))
            return false;
        wchar_t hash[65];
        if (!Hash(pidl, hash))
            return false;
        try {
            std::wstring prefix = Prefix(kind, hash);
            return ConsumePrefix(anchor, prefix.c_str(), prefix.size(), now);
        } catch (...) {
        }
        return false;
    }
    bool Consume(IntentPropertyKind kind,
                 PCIDLIST_ABSOLUTE pidl,
                 ULONGLONG now) noexcept {
        if (!Ready() || !pidl || !ValidKind(kind))
            return false;
        wchar_t hash[65];
        if (!Hash(pidl, hash))
            return false;
        try {
            std::wstring prefix = Prefix(kind, hash);
            Scan scan{this, prefix.c_str(), prefix.size(), now};
            EnumWindows(ConsumeWindow, reinterpret_cast<LPARAM>(&scan));
            return scan.consumed;
        } catch (...) {
            return false;
        }
    }
    void Cancel(IntentPropertyKind kind, ULONGLONG token) noexcept {
        if (!ready_.load(std::memory_order_acquire) || !token ||
            !ValidKind(kind))
            return;
        Cleanup cleanup{this, false, kind, token};
        EnumWindows(CleanupWindow, reinterpret_cast<LPARAM>(&cleanup));
    }
    void Close() noexcept {
        closing_.store(true, std::memory_order_release);
        if (!ready_.load(std::memory_order_acquire))
            return;
        Cleanup cleanup{this, true, IntentPropertyKind::Window, 0};
        EnumWindows(CleanupWindow, reinterpret_cast<LPARAM>(&cleanup));
    }
};
}  // namespace etr

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

namespace etr {
using Microsoft::WRL::ComPtr;
struct ShellMemoryDeleter {
    void operator()(void* p) const noexcept { CoTaskMemFree(p); }
};
using OwnedPidl = std::unique_ptr<ITEMIDLIST, ShellMemoryDeleter>;
using OwnedShellText = std::unique_ptr<wchar_t, ShellMemoryDeleter>;
inline ComPtr<IShellBrowser> AcquireShellBrowser(IDispatch* dispatch) {
    ComPtr<IServiceProvider> services;
    ComPtr<IShellBrowser> browser;
    if (dispatch &&
        SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS(&services))))
        services->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser));
    return browser;
}
struct FolderLocation {
    ComPtr<IShellBrowser> browser;
    HWND tab = nullptr;
    ComPtr<IShellView> view;
    OwnedPidl identity;
    std::wstring address;
};

// One apartment-local ShellWindows connection per routing request; navigation
// results remain fresh and are never cached across polling attempts.
class ShellTabCatalog {
    ComPtr<IShellWindows> windows_;
    ULONGLONG nextConnectAttempt_ = 0;
    const DWORD apartmentThread_ = GetCurrentThreadId();

    bool EnsureConnection() {
        if (GetCurrentThreadId() != apartmentThread_)
            return false;
        if (windows_)
            return true;
        if (GetTickCount64() < nextConnectAttempt_)
            return false;
        nextConnectAttempt_ = GetTickCount64() + 100;
        CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(&windows_));
        return windows_ != nullptr;
    }

    struct TabAccess {
        HWND frame, tab;
        ComPtr<IDispatch> dispatch;
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
            TabAccess access{};
            access.dispatch = dispatch;
            access.browser = AcquireShellBrowser(dispatch.Get());
            if (!access.browser ||
                FAILED(access.browser->GetWindow(&access.tab)) ||
                !HasWindowClass(access.tab, L"ShellTabWindowClass"))
                continue;
            access.frame = GetAncestor(access.tab, GA_ROOT);
            if (!HasWindowClass(access.frame, L"CabinetWClass"))
                continue;
            if (visitor(access))
                return;
        }
    }

   private:
    static std::optional<FolderLocation> Inspect(const TabAccess& access) {
        FolderLocation result;
        result.browser = access.browser;
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
        if (SUCCEEDED(hr) && ownedText && *ownedText)
            result.address = ownedText.get();
        return result;
    }

   public:
    ShellTabCatalog() { EnsureConnection(); }
    ShellTabCatalog(const ShellTabCatalog&) = delete;
    ShellTabCatalog& operator=(const ShellTabCatalog&) = delete;
    std::optional<TabAccess> FindTab(HWND wanted) {
        std::optional<TabAccess> result;
        VisitTabs([&](const TabAccess& access) {
            if (access.tab != wanted)
                return false;
            result = access;
            return true;
        });
        return result;
    }
    std::optional<FolderLocation> FindFolder(PCIDLIST_ABSOLUTE requested,
                                             HWND excluded = nullptr) {
        std::optional<FolderLocation> result;
        if (!requested)
            return result;
        VisitTabs([&](const TabAccess& access) {
            if (!CanHostFrame(access.frame, excluded))
                return false;
            auto location = Inspect(access);
            if (!location || !ILIsEqual(location->identity.get(), requested))
                return false;
            result = std::move(location);
            return true;
        });
        return result;
    }
    std::optional<FolderLocation> ReadFrameDestination(HWND target) {
        // An explicit source lookup must accept a transparent staging window.
        if (!IsWindow(target))
            return std::nullopt;
        HWND frame = HasWindowClass(target, L"ShellTabWindowClass")
                         ? GetAncestor(target, GA_ROOT)
                         : target;
        if (!HasWindowClass(frame, L"CabinetWClass"))
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
            if (result && result->address.empty())
                result.reset();
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
            if (!CanHostFrame(access.frame, excluded) ||
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
    return etr::AcquireShellBrowser(dispatch);
}

static bool SameFolder(IShellView* view, PCIDLIST_ABSOLUTE requested) {
    if (!view || !requested)
        return false;
    ComPtr<IFolderView> folderView;
    ComPtr<IPersistFolder2> folder;
    PIDLIST_ABSOLUTE raw = nullptr;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(&folderView))) ||
        FAILED(folderView->GetFolder(IID_PPV_ARGS(&folder))))
        return false;
    const HRESULT hr = folder->GetCurFolder(&raw);
    etr::OwnedPidl actual(raw);
    // PIDLs are the identity. Names and case-folded filesystem paths cannot
    // prove equality (including on case-sensitive filesystem directories).
    return SUCCEEDED(hr) && actual && ILIsEqual(actual.get(), requested);
}

namespace etr {
struct SelectionSnapshot {
    OwnedPidl parent;
    // IEnumIDList yields children relative to this exact parent view.
    std::vector<OwnedPidl> children;
};
}  // namespace etr

static bool ApplySelection(IShellView* view,
                           PCIDLIST_ABSOLUTE expected,
                           const etr::SelectionSnapshot& selection) {
    if (!view || !expected || !selection.parent ||
        !ILIsEqual(selection.parent.get(), expected) ||
        !SameFolder(view, expected))
        return false;
    for (size_t i = 0; i < selection.children.size(); ++i) {
        if (!selection.children[i])
            return false;
        UINT select = SVSI_SELECT | SVSI_ENSUREVISIBLE;
        if (i == 0)
            select |= SVSI_DESELECTOTHERS | SVSI_FOCUSED;
        if (FAILED(view->SelectItem(selection.children[i].get(), select)))
            return false;
    }
    return SameFolder(view, expected);
}

// ExplorerTabUtility documents the existing 0xA221 command. Its sequential
// search supplies no stable tab-order API. Cache only ordinals actually
// observed after this command, and always verify the resulting active HWND.
static bool ActivateShellTab(HWND tab) {
    if (!IsWindow(tab))
        return false;
    HWND frame = GetAncestor(tab, GA_ROOT);
    if (!etr::CanHostFrame(frame))
        return false;
    if (IsIconic(frame))
        ShowWindowAsync(frame, SW_RESTORE);

    struct FrameHints {
        std::vector<HWND> membership;
        std::unordered_map<HWND, int> ordinal;
    };
    // HWND/int metadata only: never an apartment-bound COM pointer.
    static thread_local std::unordered_map<HWND, FrameHints> hints;
    for (auto it = hints.begin(); it != hints.end();) {
        if (!IsWindow(it->first))
            it = hints.erase(it);
        else
            ++it;
    }
    if (hints.size() > 64)
        hints.clear();
    auto active = [&] {
        return FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
    };
    if (active() != tab) {
        std::vector<HWND> membership;
        for (HWND child = nullptr;
             (child = FindWindowExW(frame, child, L"ShellTabWindowClass",
                                    nullptr));)
            membership.push_back(child);
        std::sort(membership.begin(), membership.end(), std::less<HWND>{});
        if (!std::binary_search(membership.begin(), membership.end(), tab,
                                std::less<HWND>{}))
            return false;
        auto& learned = hints[frame];
        if (learned.membership != membership) {
            learned.membership = membership;
            learned.ordinal.clear();
        }
        const ULONGLONG deadline = GetTickCount64() + 1200;
        auto selectOrdinal = [&](int index) {
            const ULONGLONG now = GetTickCount64();
            if (now >= deadline || !IsWindow(tab) ||
                GetAncestor(tab, GA_ROOT) != frame)
                return false;
            DWORD_PTR result = 0;
            if (!SendMessageTimeoutW(
                    frame, WM_COMMAND, 0xA221, index, SMTO_ABORTIFHUNG,
                    static_cast<UINT>(std::min<ULONGLONG>(300, deadline - now)),
                    &result))
                return false;
            HWND observed = active();
            if (observed)
                learned.ordinal[observed] = index;
            return GetTickCount64() <= deadline;
        };
        auto hint = learned.ordinal.find(tab);
        if (hint != learned.ordinal.end()) {
            const int index = hint->second;
            if (!selectOrdinal(index))
                return false;
            if (active() != tab)
                learned.ordinal.clear();  // Reordered tabs: discard every hint.
        }
        if (active() != tab) {
            for (size_t index = 1; index <= membership.size(); ++index) {
                if (!selectOrdinal(static_cast<int>(index)))
                    return false;
                if (active() == tab)
                    break;
            }
        }
        if (active() != tab)
            return false;
    }
    SetForegroundWindow(frame);
    return true;
}

enum class ReuseResult { NoMatch, Completed, FailedAfterMatch };

template <class Catalog>
static ReuseResult ReuseFolderTab(Catalog& catalog,
                                  PCIDLIST_ABSOLUTE folder,
                                  const etr::SelectionSnapshot& selection,
                                  HWND exclude = nullptr) {
    if (!folder)
        return ReuseResult::NoMatch;
    auto location = catalog.FindFolder(folder, exclude);
    if (!location)
        return ReuseResult::NoMatch;
    // Once an exact match exists, a failed activation or selection must leave
    // the source recoverable. It must never request a duplicate folder tab.
    if (!ActivateShellTab(location->tab))
        return ReuseResult::FailedAfterMatch;
    // Activation can reenter Explorer: acquire a fresh view and identity.
    ComPtr<IShellView> view;
    return location->browser &&
                   SUCCEEDED(location->browser->QueryActiveShellView(&view)) &&
                   view && ApplySelection(view.Get(), folder, selection)
               ? ReuseResult::Completed
               : ReuseResult::FailedAfterMatch;
}

// BEGIN INTENT POLICY
namespace etr {
enum class FolderIntent { Ordinary, NewTab, NewWindow };
struct FolderCommand {
    FolderIntent intent = FolderIntent::Ordinary;
    bool foreground = false;
    bool tabSelectionPrepared = false;
    ULONGLONG revision = 0;
};
inline FolderCommand ClassifyFolderCommand(PCWSTR name,
                                           DWORD flags,
                                           bool initialized) {
    if (!initialized)
        return {};
    if ((name && _wcsicmp(name, L"opennewwindow") == 0) || (flags & 1))
        return {FolderIntent::NewWindow, false};
    if (name && _wcsicmp(name, L"opennewtab") == 0)
        return {FolderIntent::NewTab, true};
    return {};
}
template <typename T>
class ScopedValue {
    T& target_;
    T previous_;

   public:
    ScopedValue(T& target, T value) : target_(target), previous_(target) {
        target_ = value;
    }
    ~ScopedValue() { target_ = previous_; }
    ScopedValue(const ScopedValue&) = delete;
    ScopedValue& operator=(const ScopedValue&) = delete;
};
class FolderCommandRegistry {
    struct Record {
        FolderCommand command;
        ULONGLONG expires;
    };
    std::unordered_map<void*, Record> commands_;
    ULONGLONG nextRevision_ = 0;
    void Prune(ULONGLONG now) {
        std::erase_if(commands_, [=](const auto& item) {
            return now >= item.second.expires;
        });
    }

   public:
    void Set(void* identity, FolderCommand command, ULONGLONG now) {
        Prune(now);
        if (!identity)
            return;
        commands_.erase(identity);
        if (command.intent == FolderIntent::Ordinary)
            return;
        if (commands_.size() >= 256) {
            auto oldest =
                std::min_element(commands_.begin(), commands_.end(),
                                 [](const auto& a, const auto& b) {
                                     return a.second.expires < b.second.expires;
                                 });
            commands_.erase(oldest);
        }
        if (++nextRevision_ == 0)
            ++nextRevision_;
        command.revision = nextRevision_;
        command.tabSelectionPrepared = false;
        commands_.emplace(identity, Record{command, now + 10000});
    }
    FolderCommand Get(void* identity, ULONGLONG now) {
        Prune(now);
        auto found = commands_.find(identity);
        if (found == commands_.end())
            return {};
        return found->second.command;
    }
    bool SetPrepared(void* identity,
                     ULONGLONG revision,
                     bool prepared,
                     ULONGLONG now) {
        Prune(now);
        auto found = commands_.find(identity);
        if (found == commands_.end() ||
            found->second.command.revision != revision ||
            found->second.command.intent != FolderIntent::NewTab ||
            !found->second.command.foreground)
            return false;
        found->second.command.tabSelectionPrepared = prepared;
        return true;
    }
    bool MarkPrepared(void* identity, ULONGLONG revision, ULONGLONG now) {
        return SetPrepared(identity, revision, true, now);
    }
    void ClearPrepared(void* identity, ULONGLONG revision, ULONGLONG now) {
        (void)SetPrepared(identity, revision, false, now);
    }
    size_t Size() const noexcept { return commands_.size(); }
};
class PidlIntentRegistry {
    struct Request {
        OwnedPidl identity;
        ULONGLONG expires;
        ULONGLONG token;
    };
    std::vector<Request> requests_;
    ULONGLONG nextToken_ = 0;
    void Prune(ULONGLONG now) {
        std::erase_if(requests_, [=](const auto& request) {
            return now >= request.expires;
        });
    }

   public:
    ULONGLONG Add(PCIDLIST_ABSOLUTE pidl, ULONGLONG now) {
        Prune(now);
        if (!pidl)
            return 0;
        OwnedPidl identity(ILCloneFull(pidl));
        if (!identity)
            return 0;
        if (requests_.size() >= 128)
            requests_.erase(requests_.begin());
        if (++nextToken_ == 0)
            ++nextToken_;
        requests_.push_back({std::move(identity), now + 10000, nextToken_});
        return nextToken_;
    }
    bool Consume(PCIDLIST_ABSOLUTE pidl, ULONGLONG now) {
        Prune(now);
        if (!pidl)
            return false;
        for (auto i = requests_.begin(); i != requests_.end(); ++i) {
            if (ILIsEqual(i->identity.get(), pidl)) {
                requests_.erase(i);
                return true;
            }
        }
        return false;
    }
    void Cancel(ULONGLONG token) {
        std::erase_if(requests_, [=](const auto& request) {
            return request.token == token;
        });
    }
    size_t Size() const noexcept { return requests_.size(); }
};
}  // namespace etr
static thread_local bool g_foregroundNewTabCall = false;
// END INTENT POLICY
static std::mutex g_explicitMutex;
static etr::FolderCommandRegistry g_folderVerbs;
static etr::PidlIntentRegistry g_explicitWindows;
static etr::PidlIntentRegistry g_explicitNewTabs;
static etr::IntentPropertyChannel g_intentProperties;
static HWND FastPrimary(HWND exclude);
static constexpr ULONGLONG kSharedIntent = ULONGLONG{1} << 63;

static etr::PidlIntentRegistry& LocalIntentRegistry(
    etr::IntentPropertyKind kind) {
    return kind == etr::IntentPropertyKind::Window ? g_explicitWindows
                                                   : g_explicitNewTabs;
}
static ULONGLONG RegisterFolderIntent(etr::IntentPropertyKind kind,
                                      PCIDLIST_ABSOLUTE pidl,
                                      ULONGLONG now) {
    if (g_intentProperties.Closing())
        return 0;
    if (g_intentProperties.Ready()) {
        const auto token =
            g_intentProperties.Publish(FastPrimary(nullptr), kind, pidl, now);
        if (token && !(token & kSharedIntent))
            return token | kSharedIntent;
        if (token)
            g_intentProperties.Cancel(kind, token);
        static std::atomic<bool> warned{false};
        if (!warned.exchange(true))
            Wh_Log(
                L"Cross-process intent forwarding unavailable; using local "
                L"fallback");
    }
    std::lock_guard lock(g_explicitMutex);
    return LocalIntentRegistry(kind).Add(pidl, now);
}
static void CancelFolderIntent(etr::IntentPropertyKind kind, ULONGLONG token) {
    if (!token)
        return;
    if (token & kSharedIntent) {
        g_intentProperties.Cancel(kind, token & ~kSharedIntent);
        return;
    }
    std::lock_guard lock(g_explicitMutex);
    LocalIntentRegistry(kind).Cancel(token);
}
static bool ConsumeFolderIntent(etr::IntentPropertyKind kind,
                                PCIDLIST_ABSOLUTE pidl,
                                ULONGLONG now) {
    if (g_intentProperties.Ready() &&
        g_intentProperties.Consume(kind, pidl, now))
        return true;
    std::lock_guard lock(g_explicitMutex);
    return LocalIntentRegistry(kind).Consume(pidl, now);
}

static thread_local bool g_explicitWindowCall = false;
static thread_local bool g_explicitWindowCreated = false;

// Only a folder-menu invocation establishes foreground intent. Middle-click
// uses the same opennewtab verb and flags, but never enters this scope.
static thread_local bool g_foregroundTabCommand = false;
using InvokeFolderMenu_t = HRESULT (*)(void*,
                                       IContextMenu*,
                                       CMINVOKECOMMANDINFOEX*);
static InvokeFolderMenu_t InvokeFolderMenu_Original;
static HRESULT InvokeFolderMenu_Hook(void* self,
                                     IContextMenu* menu,
                                     CMINVOKECOMMANDINFOEX* request) {
    etr::ScopedValue invoking(g_foregroundTabCommand, true);
    return InvokeFolderMenu_Original(self, menu, request);
}

static void* CommandIdentity(void* self) {
    ComPtr<IUnknown> identity;
    if (!self || FAILED(reinterpret_cast<IUnknown*>(self)->QueryInterface(
                     IID_PPV_ARGS(&identity))))
        return nullptr;
    return identity.Get();
}
static void QueueFocus(HWND tab, bool guardInput = true);
static void ScheduleNewTab(HWND tab) {
    if (!g_settings.activateNewTabs || !g_foregroundNewTabCall)
        return;
    HWND frame = GetAncestor(tab, GA_ROOT);
    int count = 0;
    for (HWND t = nullptr;
         (t = FindWindowExW(frame, t, L"ShellTabWindowClass", nullptr));)
        ++count;
    if (count < 2)
        return;
    QueueFocus(tab, false);
}
static bool ConsumeExplicitWindow(PCIDLIST_ABSOLUTE pidl) {
    return ConsumeFolderIntent(etr::IntentPropertyKind::Window, pidl,
                               GetTickCount64());
}
using FolderInitialize_t = HRESULT (*)(void*, PCWSTR, IPropertyBag*);
using FolderExecute_t = HRESULT (*)(void*);
using FolderShow_t = HRESULT (*)(void*, PCIDLIST_ABSOLUTE, bool);
static FolderInitialize_t FolderInitialize_Original;
static FolderExecute_t FolderExecute_Original;
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
    VariantClear(&flags);
    auto command = etr::ClassifyFolderCommand(name, value, SUCCEEDED(hr));
    if (command.intent == etr::FolderIntent::NewTab)
        command.foreground = g_foregroundTabCommand;
    // Conservative suppression at both observed command boundaries. This
    // does not claim to identify every possible asynchronous background input.
    if (GetKeyState(VK_MBUTTON) & 0x8000)
        command.foreground = false;
    {
        std::lock_guard<std::mutex> lock(g_explicitMutex);
        g_folderVerbs.Set(identity, command, GetTickCount64());
    }
    return hr;
}
static HRESULT FolderShow_Hook(void* self, PCIDLIST_ABSOLUTE pidl, bool flag) {
    etr::FolderCommand command;
    void* identity = CommandIdentity(self);
    {
        std::lock_guard<std::mutex> lock(g_explicitMutex);
        // ShowFolder can arrive later or on another thread, and one command
        // can show several selected folders. Look up its canonical identity
        // without consuming it; Initialize overwrites and expiry bounds it.
        command = g_folderVerbs.Get(identity, GetTickCount64());
    }
    if (GetKeyState(VK_MBUTTON) & 0x8000)
        command.foreground = false;
    const bool separate = command.intent == etr::FolderIntent::NewWindow;
    const bool foregroundTab =
        command.intent == etr::FolderIntent::NewTab && command.foreground;
    ULONGLONG windowToken = 0, tabToken = 0;
    if (separate && !g_explicitWindowCall)
        windowToken = RegisterFolderIntent(etr::IntentPropertyKind::Window,
                                           pidl, GetTickCount64());
    if (foregroundTab && !command.tabSelectionPrepared)
        tabToken = RegisterFolderIntent(etr::IntentPropertyKind::Tab, pidl,
                                        GetTickCount64());
    Wh_Log(L"ShowFolder self=%p identity=%p separate=%d flag=%d pidl=%p", self,
           identity, separate, flag, pidl);
    bool previous = g_explicitWindowCall,
         previousCreated = g_explicitWindowCreated;
    g_explicitWindowCall = previous || separate;
    g_explicitWindowCreated = false;
    HRESULT hr = FolderShow_Original(self, pidl, flag);
    if (g_explicitWindowCreated || FAILED(hr))
        CancelFolderIntent(etr::IntentPropertyKind::Window, windowToken);
    if (FAILED(hr))
        CancelFolderIntent(etr::IntentPropertyKind::Tab, tabToken);
    const bool createdSynchronously = g_explicitWindowCreated;
    g_explicitWindowCall = previous;
    // A nested explicit scope must report creation to the scope which owns
    // the pending request. A standalone call restores its caller's TLS.
    g_explicitWindowCreated =
        previousCreated || (previous && createdSynchronously);
    return hr;
}

static bool ReadFolderCommandSelection(
    void* self,
    std::vector<etr::OwnedPidl>& destinations) {
    destinations.clear();
    ComPtr<IObjectWithSelection> object;
    ComPtr<IShellItemArray> selection;
    DWORD count = 0;
    if (!self ||
        FAILED(reinterpret_cast<IUnknown*>(self)->QueryInterface(
            IID_PPV_ARGS(&object))) ||
        !object || FAILED(object->GetSelection(IID_PPV_ARGS(&selection))) ||
        !selection || FAILED(selection->GetCount(&count)) || count == 0)
        return false;
    try {
        destinations.reserve(count);
        for (DWORD i = 0; i < count; ++i) {
            ComPtr<IShellItem> item;
            if (FAILED(selection->GetItemAt(i, &item)) || !item) {
                destinations.clear();
                return false;
            }
            PIDLIST_ABSOLUTE pidl = nullptr;
            const HRESULT hr = SHGetIDListFromObject(item.Get(), &pidl);
            etr::OwnedPidl owned(pidl);
            if (FAILED(hr) || !owned) {
                destinations.clear();
                return false;
            }
            destinations.push_back(std::move(owned));
        }
    } catch (...) {
        destinations.clear();
        return false;
    }
    return true;
}

static HRESULT FolderExecute_Hook(void* self) {
    void* identity = CommandIdentity(self);
    etr::FolderCommand command;
    {
        std::lock_guard<std::mutex> lock(g_explicitMutex);
        command = g_folderVerbs.Get(identity, GetTickCount64());
    }
    std::vector<ULONGLONG> tokens;
    bool prepared = false;
    auto rollback = [&] {
        for (ULONGLONG token : tokens)
            CancelFolderIntent(etr::IntentPropertyKind::Tab, token);
        tokens.clear();
        if (prepared) {
            std::lock_guard<std::mutex> lock(g_explicitMutex);
            g_folderVerbs.ClearPrepared(identity, command.revision,
                                        GetTickCount64());
            prepared = false;
        }
    };
    if (command.intent == etr::FolderIntent::NewTab && command.foreground) {
        std::vector<etr::OwnedPidl> destinations;
        if (ReadFolderCommandSelection(self, destinations)) {
            try {
                tokens.reserve(destinations.size());
                {
                    std::lock_guard<std::mutex> lock(g_explicitMutex);
                    prepared = g_folderVerbs.MarkPrepared(
                        identity, command.revision, GetTickCount64());
                }
                if (prepared) {
                    for (const auto& pidl : destinations) {
                        const ULONGLONG token =
                            RegisterFolderIntent(etr::IntentPropertyKind::Tab,
                                                 pidl.get(), GetTickCount64());
                        if (!token) {
                            rollback();
                            break;
                        }
                        tokens.push_back(token);
                    }
                }
            } catch (...) {
                rollback();
            }
        } else {
            std::lock_guard<std::mutex> lock(g_explicitMutex);
            g_folderVerbs.ClearPrepared(identity, command.revision,
                                        GetTickCount64());
        }
    }
    const HRESULT hr = FolderExecute_Original(self);
    if (FAILED(hr))
        rollback();
    return hr;
}

using AddTabAtIndex_t = HRESULT (*)(void*, PCIDLIST_ABSOLUTE, int, int, GUID*);
static AddTabAtIndex_t AddTabAtIndex_Original;
static HRESULT AddTabAtIndex_Hook(void* self,
                                  PCIDLIST_ABSOLUTE pidl,
                                  int entryPoint,
                                  int index,
                                  GUID* id) {
    const bool foreground = ConsumeFolderIntent(etr::IntentPropertyKind::Tab,
                                                pidl, GetTickCount64());
    Wh_Log(L"AddTabAtIndex self=%p entryPoint=%d index=%d foreground=%d", self,
           entryPoint, index, foreground);
    // Scope also clears an outer foreground call during unrelated/reentrant
    // AddTabAtIndex operations. Never infer intent from the entry-point enum.
    etr::ScopedValue creating(g_foregroundNewTabCall, foreground);
    return AddTabAtIndex_Original(self, pidl, entryPoint, index, id);
}
static constexpr UINT kNewTabCommand = 0xA21B;

namespace etr {

// Owns presentation changes only while the window still carries this entry's
// token. All bookkeeping uses one mutex; every window API runs outside it.
class WindowPresentationStore final {
   public:
    static constexpr const wchar_t* PropertyName = L"ETR.Redirecting";
    static constexpr const wchar_t* ConstructionPropertyName =
        L"ETR.ConstructionShow";

    WindowPresentationStore() = default;
    WindowPresentationStore(const WindowPresentationStore&) = delete;
    WindowPresentationStore& operator=(const WindowPresentationStore&) = delete;
    ~WindowPresentationStore() = default;

    [[nodiscard]] std::optional<ULONG_PTR> Stage(
        HWND window,
        std::vector<int>* deferredShows = nullptr,
        HANDLE constructionToken = nullptr) {
        if (!::IsWindow(window) ||
            ::GetPropW(window, PropertyName) != nullptr ||
            (constructionToken &&
             ::GetPropW(window, ConstructionPropertyName) !=
                 constructionToken)) {
            ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return std::nullopt;
        }
        const auto token = AllocateToken();
        if (!token) {
            ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return std::nullopt;
        }
        std::shared_ptr<Entry> entry;
        try {
            entry = std::make_shared<Entry>(window, *token);
        } catch (...) {
            ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return std::nullopt;
        }
        // Publish the identity and original presentation together. Hook
        // reentry can read these immutable fields as soon as we reserve it.
        entry->threadId = ::GetWindowThreadProcessId(window, &entry->processId);
        if (!entry->threadId || !ReadExStyle(window, entry->exStyle)) {
            ::SetLastError(LastErrorOr(ERROR_INVALID_WINDOW_HANDLE));
            return std::nullopt;
        }
        if ((entry->exStyle & WS_EX_LAYERED) &&
            !::GetLayeredWindowAttributes(window, &entry->colorKey,
                                          &entry->alpha,
                                          &entry->layeredFlags)) {
            ::SetLastError(LastErrorOr(ERROR_NOT_SUPPORTED));
            return std::nullopt;
        }
        std::shared_ptr<Entry> previous;
        DWORD error = ERROR_SUCCESS;
        try {
            {
                std::lock_guard lock(mutex_);
                if (stopping_) {
                    error = ERROR_OPERATION_ABORTED;
                } else if (const auto old = entries_.find(window);
                           old != entries_.end()) {
                    if (old->second->state != State::Staged) {
                        error = ERROR_BUSY;
                    } else {
                        previous = old->second;
                        previous->state = State::Operating;
                    }
                } else {
                    entries_.emplace(window, entry);
                    if (deferredShows)
                        entry->deferredShows.swap(*deferredShows);
                }
            }
            if (error) {
                ::SetLastError(error);
                return std::nullopt;
            }
            if (previous) {
                if (HasToken(*previous)) {
                    SetState(previous, State::Staged);
                    ::SetLastError(ERROR_ALREADY_EXISTS);
                    return std::nullopt;
                }
                Finish(previous);
                {
                    std::lock_guard lock(mutex_);
                    if (stopping_)
                        error = ERROR_OPERATION_ABORTED;
                    else if (!entries_.emplace(window, entry).second)
                        error = ERROR_BUSY;
                    else if (deferredShows)
                        entry->deferredShows.swap(*deferredShows);
                }
                if (error) {
                    ::SetLastError(error);
                    return std::nullopt;
                }
            }
        } catch (...) {
            ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return std::nullopt;
        }

        bool installed = false, touched = false;
        const auto fail = [&](DWORD failure) -> std::optional<ULONG_PTR> {
            if (installed && touched)
                (void)Restore(entry, true);
            else {
                if (installed)
                    (void)RemoveOwnedProperty(*entry);
                if (deferredShows) {
                    std::lock_guard lock(mutex_);
                    deferredShows->swap(entry->deferredShows);
                }
                Finish(entry);
            }
            ::SetLastError(failure);
            return std::nullopt;
        };
        if (!SameWindow(*entry) ||
            ::GetPropW(window, PropertyName) != nullptr ||
            (constructionToken &&
             ::GetPropW(window, ConstructionPropertyName) != constructionToken))
            return fail(ERROR_INVALID_WINDOW_HANDLE);
        if (!::SetPropW(window, PropertyName, TokenHandle(entry->token)))
            return fail(LastErrorOr(ERROR_ACCESS_DENIED));
        installed = true;
        if (!HasToken(*entry))
            return fail(ERROR_INVALID_WINDOW_HANDLE);
        touched = true;
        if (!WriteExStyle(window, entry->exStyle | WS_EX_LAYERED))
            return fail(LastErrorOr(ERROR_ACCESS_DENIED));
        if (!HasToken(*entry) ||
            !::SetLayeredWindowAttributes(window, 0, 0, LWA_ALPHA))
            return fail(LastErrorOr(ERROR_INVALID_WINDOW_HANDLE));
        if (!HasToken(*entry))
            return fail(ERROR_INVALID_WINDOW_HANDLE);
        SetState(entry, State::Staged);
        return entry->token;
    }

    [[nodiscard]] bool Owns(HWND window, ULONG_PTR token) const {
        const auto entry = Find(window, token);
        if (!entry)
            return false;
        {
            std::lock_guard lock(mutex_);
            if (entry->state == State::Staging ||
                entry->state == State::Finished)
                return false;
        }
        return HasToken(*entry);
    }

    // Return true only after retaining the exact intercepted command. Keeping
    // the sequence preserves e.g. maximize followed by show-without-activation.
    [[nodiscard]] bool DeferShow(HWND window, int command) {
        const auto entry = Find(window);
        if (!entry || !SameWindow(*entry))
            return false;
        const HANDLE property = ::GetPropW(window, PropertyName);
        std::lock_guard lock(mutex_);
        if (entry->state == State::Presenting ||
            entry->state == State::Finished ||
            (property != TokenHandle(entry->token) &&
             !(entry->state == State::Staging && property == nullptr)))
            return false;
        try {
            entry->deferredShows.push_back(command);
            return true;
        } catch (...) {
            // A request we cannot retain must reach the native API.
            return false;
        }
    }

    [[nodiscard]] bool Reveal(HWND window, ULONG_PTR token) {
        const auto entry = Find(window, token);
        return entry && Begin(entry) && Restore(entry, true);
    }

    [[nodiscard]] bool Retire(HWND window, ULONG_PTR token) {
        const auto entry = Find(window, token);
        if (!entry || !Begin(entry))
            return false;
        if (!HasToken(*entry)) {
            Finish(entry);
            ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return false;
        }
        if (!::PostMessageW(window, WM_CLOSE, 0, 0)) {
            const DWORD error = LastErrorOr(ERROR_ACCESS_DENIED);
            (void)Restore(entry, true);
            ::SetLastError(error);
            return false;
        }
        // A committed close must not later be shown by RestoreAll.
        (void)RemoveOwnedProperty(*entry);
        Finish(entry);
        return true;
    }

    // The owner stops/joins its worker before shutdown. Retry failed live
    // records on a later explicit sweep; the destructor never calls this.
    void RestoreAll() {
        {
            std::lock_guard lock(mutex_);
            if (restoreSweepActive_)
                return;
            restoreSweepActive_ = true;
            stopping_ = true;
        }
        struct EndSweep final {
            WindowPresentationStore& store;
            ~EndSweep() {
                std::lock_guard lock(store.mutex_);
                store.restoreSweepActive_ = false;
            }
        } endSweep{*this};
        // Tokens never repeat and stopping_ prevents insertion. Walking their
        // order needs neither a throwing snapshot allocation nor visit flags.
        ULONG_PTR lastVisited = 0;
        for (;;) {
            std::shared_ptr<Entry> entry;
            {
                std::unique_lock lock(mutex_);
                for (const auto& item : entries_) {
                    if (item.second->token > lastVisited &&
                        (!entry || item.second->token < entry->token))
                        entry = item.second;
                }
                if (!entry)
                    return;
                lastVisited = entry->token;
                changed_.wait(lock, [&] {
                    return entry->state == State::Staged ||
                           entry->state == State::Finished;
                });
                if (entry->state == State::Finished)
                    continue;
                entry->state = State::Operating;
            }
            (void)Restore(entry, true);
        }
    }

    [[nodiscard]] bool IsStaged(HWND window) const {
        const auto entry = Find(window);
        if (!entry)
            return ::GetPropW(window, PropertyName) != nullptr;
        const HANDLE property = ::GetPropW(window, PropertyName);
        std::lock_guard lock(mutex_);
        return entry->state != State::Presenting &&
               entry->state != State::Finished &&
               (property == TokenHandle(entry->token) || property != nullptr ||
                entry->state == State::Staging);
    }

    [[nodiscard]] static HWND Root(HWND window) noexcept {
        return window == nullptr ? nullptr : ::GetAncestor(window, GA_ROOT);
    }
    [[nodiscard]] static bool IsExplorerFrame(HWND window) noexcept {
        return HasWindowClass(window, L"CabinetWClass");
    }

   private:
    enum class State { Staging, Staged, Operating, Presenting, Finished };
    struct Entry final {
        Entry(HWND value, ULONG_PTR identity)
            : window(value), token(identity) {}
        const HWND window;
        const ULONG_PTR token;
        DWORD threadId{}, processId{};
        LONG_PTR exStyle{};
        COLORREF colorKey{};
        BYTE alpha{255};
        DWORD layeredFlags{};
        State state{State::Staging};
        std::vector<int> deferredShows;
    };
    [[nodiscard]] static HANDLE TokenHandle(ULONG_PTR token) noexcept {
        return reinterpret_cast<HANDLE>(token);
    }
    [[nodiscard]] static std::optional<ULONG_PTR> AllocateToken() noexcept {
        static std::atomic<ULONG_PTR> next{1};
        ULONG_PTR value = next.load(std::memory_order_relaxed);
        while (value != (std::numeric_limits<ULONG_PTR>::max)()) {
            if (next.compare_exchange_weak(value, value + 1,
                                           std::memory_order_relaxed))
                return value;
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
        std::lock_guard lock(mutex_);
        const auto found = entries_.find(window);
        return found == entries_.end() ? std::shared_ptr<Entry>{}
                                       : found->second;
    }
    [[nodiscard]] std::shared_ptr<Entry> Find(HWND window,
                                              ULONG_PTR token) const {
        const auto entry = Find(window);
        return entry && entry->token == token ? entry
                                              : std::shared_ptr<Entry>{};
    }
    [[nodiscard]] bool Begin(const std::shared_ptr<Entry>& entry) {
        bool reserved = false;
        {
            std::lock_guard lock(mutex_);
            if (entry->state == State::Staged) {
                entry->state = State::Operating;
                reserved = true;
            }
        }
        if (!reserved)
            ::SetLastError(ERROR_BUSY);
        return reserved;
    }
    void SetState(const std::shared_ptr<Entry>& entry, State state) {
        {
            std::lock_guard lock(mutex_);
            entry->state = state;
        }
        changed_.notify_all();
    }
    void Finish(const std::shared_ptr<Entry>& entry) {
        {
            std::lock_guard lock(mutex_);
            entry->state = State::Finished;
            const auto found = entries_.find(entry->window);
            if (found != entries_.end() && found->second == entry)
                entries_.erase(found);
        }
        changed_.notify_all();
    }
    [[nodiscard]] bool Restore(const std::shared_ptr<Entry>& entry, bool show) {
        if (!HasToken(*entry)) {
            Finish(entry);
            ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return false;
        }
        if (!WriteExStyle(entry->window, entry->exStyle)) {
            SetState(entry, State::Staged);
            return false;
        }
        if (!HasToken(*entry)) {
            Finish(entry);
            ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return false;
        }
        if ((entry->exStyle & WS_EX_LAYERED) &&
            !::SetLayeredWindowAttributes(entry->window, entry->colorKey,
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
        std::vector<int> deferredShows;
        {
            std::lock_guard lock(mutex_);
            entry->state = State::Presenting;
            deferredShows.swap(entry->deferredShows);
        }
        // Keep the identity property through redraw and the full replay. A
        // ShowWindow hook can reenter the store and must observe Presenting.
        if (HasToken(*entry)) {
            (void)::RedrawWindow(entry->window, nullptr, nullptr,
                                 RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
            if (show) {
                if (deferredShows.empty()) {
                    if (HasToken(*entry))
                        (void)::ShowWindow(entry->window, SW_SHOWNOACTIVATE);
                } else {
                    for (int command : deferredShows) {
                        if (!HasToken(*entry))
                            break;
                        (void)::ShowWindow(entry->window, command);
                    }
                }
            }
            (void)RemoveOwnedProperty(*entry);
        }
        Finish(entry);
        return true;
    }

    mutable std::mutex mutex_;
    std::condition_variable changed_;
    std::unordered_map<HWND, std::shared_ptr<Entry>> entries_;
    bool stopping_{};
    bool restoreSweepActive_{};
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
    HWND foreground;
    HWND activeBefore;
    DWORD inputTime;
    bool guardInput;
};
static std::unordered_map<HWND, FocusTask> g_focusQueue;
static void ExecuteMergeRequest(MergeRequest*);
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
    for (auto f : ready) {
        if (g_stop)
            break;
        if ((f.guardInput && !g_settings.activateShellLookups) ||
            (!f.guardInput && !g_settings.activateNewTabs))
            continue;
        const HWND current = GetForegroundWindow();
        const HWND frame = GetAncestor(f.tab, GA_ROOT);
        LASTINPUTINFO input{sizeof(input), 0};
        const bool inputChanged = f.guardInput && f.inputTime &&
                                  GetLastInputInfo(&input) &&
                                  input.dwTime != f.inputTime;
        const HWND active =
            FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
        const bool changedTab = inputChanged && current == frame &&
                                active != f.activeBefore && active != f.tab;
        if (!changedTab && (current == f.foreground || current == frame))
            ActivateShellTab(f.tab);
    }
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
    const HWND preferred = GetAncestor(GetForegroundWindow(), GA_ROOT);
    if (etr::CanHostFrame(preferred, exclude))
        return preferred;
    struct Search {
        HWND exclude, result;
    } search{exclude, nullptr};
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            auto* state = reinterpret_cast<Search*>(parameter);
            if (etr::CanHostFrame(window, state->exclude)) {
                state->result = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));
    return search.result;
}
static void QueueFocus(HWND tab, bool guardInput) {
    if (g_stop)
        return;
    HWND frame = GetAncestor(tab, GA_ROOT);
    LASTINPUTINFO input{sizeof(input), 0};
    const DWORD inputTime = GetLastInputInfo(&input) ? input.dwTime : 0;
    const HWND foreground = GetForegroundWindow();
    const HWND activeBefore =
        FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
    std::lock_guard<std::mutex> lock(g_queueMutex);
    if (!g_stop) {
        g_focusQueue[frame] = {tab,        GetTickCount64() + 120,
                               foreground, activeBefore,
                               inputTime,  guardInput};
        SetEvent(g_wakeEvent);
    }
}

// Serialize redirects and identify the newly created tab by HWND, never by
// global IShellWindows counts (which change while other requests are arriving).

namespace etr {
struct NavigationState {
    HANDLE event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    std::atomic<bool> live{true}, dirty{false}, quit{false};
    ~NavigationState() {
        if (event)
            CloseHandle(event);
    }
};

class BrowserEventSink final : public IDispatch {
    std::atomic<ULONG> references_{1};
    std::shared_ptr<NavigationState> state_;

   public:
    explicit BrowserEventSink(std::shared_ptr<NavigationState> state)
        : state_(std::move(state)) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        if (!out)
            return E_POINTER;
        *out = nullptr;
        if (iid != IID_IUnknown && iid != IID_IDispatch &&
            iid != DIID_DWebBrowserEvents2)
            return E_NOINTERFACE;
        *out = static_cast<IDispatch*>(this);
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++references_; }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG left = --references_;
        if (!left)
            delete this;
        return left;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) override {
        if (!count)
            return E_POINTER;
        *count = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE
    GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) override {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID id,
                                     REFIID,
                                     LCID,
                                     WORD,
                                     DISPPARAMS*,
                                     VARIANT*,
                                     EXCEPINFO*,
                                     UINT*) override {
        // No browser calls, request pointers, selection or unsubscribe from
        // this callback. The STA waiter verifies the actual PIDL afterwards.
        if (!state_->live.load())
            return S_OK;
        if (id == DISPID_NAVIGATECOMPLETE2 || id == DISPID_DOCUMENTCOMPLETE ||
            id == DISPID_NAVIGATEERROR || id == DISPID_ONQUIT) {
            if (id == DISPID_ONQUIT)
                state_->quit = true;
            state_->dirty = true;
            if (state_->event)
                SetEvent(state_->event);
        }
        return S_OK;
    }
};

class NavigationSubscription {
    ComPtr<IConnectionPoint> point_;
    ComPtr<BrowserEventSink> sink_;
    std::shared_ptr<NavigationState> state_;
    DWORD cookie_ = 0;

   public:
    NavigationSubscription() = default;
    NavigationSubscription(const NavigationSubscription&) = delete;
    NavigationSubscription& operator=(const NavigationSubscription&) = delete;
    ~NavigationSubscription() { Close(); }
    const std::shared_ptr<NavigationState>& State() const { return state_; }
    HRESULT Attach(IConnectionPoint* point) {
        Close();
        if (!point)
            return E_POINTER;
        state_ = std::make_shared<NavigationState>();
        if (!state_->event)
            return HRESULT_FROM_WIN32(GetLastError());
        sink_.Attach(new (std::nothrow) BrowserEventSink(state_));
        if (!sink_)
            return E_OUTOFMEMORY;
        point_ = point;
        const HRESULT hr = point_->Advise(sink_.Get(), &cookie_);
        if (FAILED(hr))
            Close();
        return hr;
    }
    HRESULT Open(IWebBrowser2* browser) {
        ComPtr<IConnectionPointContainer> container;
        ComPtr<IConnectionPoint> point;
        if (!browser)
            return E_POINTER;
        HRESULT hr = browser->QueryInterface(IID_PPV_ARGS(&container));
        if (SUCCEEDED(hr))
            hr =
                container->FindConnectionPoint(DIID_DWebBrowserEvents2, &point);
        return SUCCEEDED(hr) ? Attach(point.Get()) : hr;
    }
    void Close() {
        if (state_) {
            state_->live = false;
            state_->dirty = false;
        }
        if (point_ && cookie_) {
            const HRESULT hr = point_->Unadvise(cookie_);
            cookie_ = 0;
            if (FAILED(hr) && hr != CONNECT_E_NOCONNECTION) {
                // A provider can retain a direct sink reference after a
                // failed Unadvise. CoDisconnectObject only disconnects COM
                // marshaling; it cannot revoke an arbitrary raw reference.
                // Pin this image only on that exceptional failure path so
                // late Invoke/Release can never enter an unloaded vtable.
                // The disabled sink owns only its inert shared state.
                HMODULE pinned = nullptr;
                const BOOL kept =
                    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                           GET_MODULE_HANDLE_EX_FLAG_PIN,
                                       reinterpret_cast<LPCWSTR>(
                                           &NavigationSubscription::PinAnchor),
                                       &pinned);
                Wh_Log(
                    L"Navigation Unadvise failed hr=%08lx; image pin=%d; "
                    L"disabled sink retained safely until provider release",
                    hr, kept);
                CoDisconnectObject(sink_.Get(), 0);
            }
        }
        point_.Reset();
        sink_.Reset();
        state_.reset();
    }

   private:
    static void PinAnchor() {}
};

class NavigationCheckSchedule {
    ULONGLONG start_, deadline_;
    size_t next_ = 0;
    static constexpr ULONGLONG offsets_[] = {0,    100,  250, 500,
                                             1000, 2000, 3500};

   public:
    NavigationCheckSchedule(ULONGLONG start, ULONGLONG deadline)
        : start_(start), deadline_(deadline) {}
    bool ShouldInspect(ULONGLONG now, bool notice) {
        bool due = false;
        while (next_ < std::size(offsets_) && now >= start_ + offsets_[next_]) {
            ++next_;
            due = true;
        }
        return now < deadline_ && (notice || due);
    }
    DWORD NextWait(ULONGLONG now) const {
        ULONGLONG wake = deadline_;
        if (next_ < std::size(offsets_))
            wake = std::min(wake, start_ + offsets_[next_]);
        return static_cast<DWORD>(
            wake > now ? std::min<ULONGLONG>(25, wake - now) : 0);
    }
};

class OwnedVariant {
   public:
    VARIANT value{};
    ~OwnedVariant() { VariantClear(&value); }
    OwnedVariant() = default;
    OwnedVariant(const OwnedVariant&) = delete;
    OwnedVariant& operator=(const OwnedVariant&) = delete;
};
}  // namespace etr

static ComPtr<IWebBrowser2> BrowserForTab(
    etr::ShellTabCatalog& catalog,
    HWND wanted,
    ComPtr<IShellBrowser>* shell = nullptr) {
    auto access = catalog.FindTab(wanted);
    ComPtr<IWebBrowser2> browser;
    if (access) {
        access->dispatch.As(&browser);
        if (shell)
            *shell = access->browser;
    }
    return browser;
}

template <class Catalog>
static std::optional<etr::SelectionSnapshot>
CaptureSelection(Catalog& catalog, HWND frame, PCIDLIST_ABSOLUTE expected) {
    auto location = catalog.ReadFrameDestination(frame);
    if (!location || !expected ||
        !ILIsEqual(location->identity.get(), expected))
        return std::nullopt;
    ComPtr<IFolderView> folderView;
    ComPtr<IEnumIDList> items;
    if (FAILED(location->view.As(&folderView)))
        return std::nullopt;
    etr::SelectionSnapshot result;
    result.parent = std::move(location->identity);
    int selected = -1;
    if (folderView->ItemCount(SVGIO_SELECTION, &selected) == S_OK &&
        selected == 0) {
        // Explorer returns ERROR_NOT_FOUND from Items for an empty selection.
        // A verified zero count is a valid snapshot, not a routing failure.
        return SameFolder(location->view.Get(), expected)
                   ? std::optional<etr::SelectionSnapshot>(std::move(result))
                   : std::nullopt;
    }
    if (FAILED(folderView->Items(SVGIO_SELECTION, IID_PPV_ARGS(&items))) ||
        !items)
        return std::nullopt;
    for (;;) {
        PITEMID_CHILD raw = nullptr;
        ULONG fetched = 0;
        const HRESULT hr = items->Next(1, &raw, &fetched);
        etr::OwnedPidl child(raw);
        if (hr == S_FALSE && !fetched)
            break;
        if (FAILED(hr) || fetched != 1 || !child)
            return std::nullopt;
        result.children.push_back(std::move(child));
    }
    if (!SameFolder(location->view.Get(), expected))
        return std::nullopt;
    return result;
}

static HRESULT NavigateToPidl(IWebBrowser2* browser, PCIDLIST_ABSOLUTE folder) {
    if (!browser || !folder)
        return E_POINTER;
    etr::OwnedVariant target;
    target.value.vt = VT_ARRAY | VT_UI1;
    target.value.parray = SafeArrayCreateVector(VT_UI1, 0, ILGetSize(folder));
    if (!target.value.parray)
        return E_OUTOFMEMORY;
    void* data = nullptr;
    HRESULT hr = SafeArrayAccessData(target.value.parray, &data);
    if (FAILED(hr))
        return hr;
    memcpy(data, folder, ILGetSize(folder));
    hr = SafeArrayUnaccessData(target.value.parray);
    if (FAILED(hr))
        return hr;
    VARIANT empty{};
    return browser->Navigate2(&target.value, &empty, &empty, &empty, &empty);
}

static ComPtr<IShellView> WaitForNavigation(IShellBrowser* browser,
                                            PCIDLIST_ABSOLUTE expected,
                                            etr::NavigationSubscription& events,
                                            ULONGLONG deadline) {
    etr::NavigationCheckSchedule schedule(GetTickCount64(), deadline);
    const auto state = events.State();
    while (!g_stop && GetTickCount64() < deadline) {
        const bool notice = state && state->dirty.exchange(false);
        if (schedule.ShouldInspect(GetTickCount64(), notice)) {
            ComPtr<IShellView> view;
            if (browser && SUCCEEDED(browser->QueryActiveShellView(&view)) &&
                view && SameFolder(view.Get(), expected))
                return view;
        }
        if (state && state->quit)
            break;
        PumpFocus();
        HANDLE handles[3] = {g_stopEvent, g_wakeEvent,
                             state ? state->event : nullptr};
        const DWORD count = handles[2] ? 3 : 2;
        if (MsgWaitForMultipleObjectsEx(
                count, handles, schedule.NextWait(GetTickCount64()),
                QS_ALLINPUT, MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0)
            break;
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    // A provider with missing events may finish between the last scheduled
    // fallback and the deadline. Give that completion one final direct-view
    // check; this still performs no ShellWindows reenumeration.
    if (!g_stop && browser) {
        ComPtr<IShellView> view;
        if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view &&
            SameFolder(view.Get(), expected))
            return view;
    }
    return nullptr;
}

// Called only by RouterWorker, whose STA owns every COM reference below.
// No nested CoInitializeEx/CoUninitialize and no thread-entry-point disguise.
static void ExecuteMergeRequest(MergeRequest* parameter) {
    std::unique_ptr<MergeRequest> info(parameter);
    const HWND source = info->stagingFrame;
    const ULONG_PTR token = info->lease;
    if (!g_presentations.Owns(source, token))
        return;
    struct RecoverPresentation {
        HWND source;
        ULONG_PTR token;
        ~RecoverPresentation() {
            if (g_presentations.Owns(source, token))
                (void)g_presentations.Reveal(source, token);
        }
    } recovery{source, token};
    if (g_stop || !g_settings.mergeWindows)
        return;
    const ULONGLONG deadline =
        info->deadline ? info->deadline : GetTickCount64() + 8000;
    auto waitWithinDeadline = [&](DWORD delay) {
        const ULONGLONG now = GetTickCount64();
        return now < deadline && g_presentations.Owns(source, token) &&
               CooperativeWait(static_cast<DWORD>(
                   std::min<ULONGLONG>(delay, deadline - now)));
    };
    etr::ShellTabCatalog catalog;
    std::optional<etr::FolderLocation> destination;
    const ULONGLONG sourceDeadline =
        std::min(deadline, GetTickCount64() + 5000);
    DWORD sourceDelay = 50;
    while (!g_stop && GetTickCount64() < sourceDeadline &&
           g_presentations.Owns(source, token)) {
        destination = catalog.ReadFrameDestination(source);
        if (destination)
            break;
        if (!waitWithinDeadline(sourceDelay))
            return;
        sourceDelay = std::min<DWORD>(500, sourceDelay * 2);
    }
    HWND primary = catalog.ChooseRoutingHost(source, info->destinationHint);
    if (!destination || !primary || primary == source ||
        etr::ClassifyShellDestination(destination->address) !=
            etr::DestinationKind::Folder ||
        ConsumeExplicitWindow(destination->identity.get()))
        return;
    // Keep the original PIDL from the source view through the entire request.
    etr::OwnedPidl folder = std::move(destination->identity);
    if (!waitWithinDeadline(10))
        return;
    auto selection = CaptureSelection(catalog, source, folder.get());
    if (!selection)
        return;
    const auto reuse =
        g_settings.reuseTabs
            ? ReuseFolderTab(catalog, folder.get(), *selection, source)
            : ReuseResult::NoMatch;
    if (reuse == ReuseResult::FailedAfterMatch)
        return;
    bool complete = reuse == ReuseResult::Completed;
    if (reuse == ReuseResult::NoMatch) {
        std::vector<HWND> before;
        for (HWND tab = nullptr;
             (tab = FindWindowExW(primary, tab, L"ShellTabWindowClass",
                                  nullptr));)
            before.push_back(tab);
        HWND active =
            FindWindowExW(primary, nullptr, L"ShellTabWindowClass", nullptr);
        if (!active || !PostMessageW(active, WM_COMMAND, kNewTabCommand, 0))
            return;
        HWND created = nullptr;
        ComPtr<IWebBrowser2> browser;
        ComPtr<IShellBrowser> shell;
        const ULONGLONG creationDeadline =
            std::min(deadline, GetTickCount64() + 3000);
        ULONGLONG nextBrowserProbe = 0;
        DWORD browserProbeDelay = 100;
        while (!g_stop && GetTickCount64() < creationDeadline &&
               IsWindow(primary)) {
            for (HWND tab = nullptr;
                 !created &&
                 (tab = FindWindowExW(primary, tab, L"ShellTabWindowClass",
                                      nullptr));)
                if (std::find(before.begin(), before.end(), tab) ==
                    before.end())
                    created = tab;
            if (created && GetTickCount64() >= nextBrowserProbe) {
                browser = BrowserForTab(catalog, created, &shell);
                if (browser && shell)
                    break;
                nextBrowserProbe = GetTickCount64() + browserProbeDelay;
                browserProbeDelay = std::min<DWORD>(500, browserProbeDelay * 2);
            }
            if (!waitWithinDeadline(50))
                return;
        }
        if (!browser || !shell)
            return;
        etr::NavigationSubscription events;
        // Some Explorer builds expose no events. Missing events remain safe:
        // at most seven scheduled direct-view checks, without reenumeration.
        (void)events.Open(browser.Get());
        if (FAILED(NavigateToPidl(browser.Get(), folder.get())))
            return;
        auto view =
            WaitForNavigation(shell.Get(), folder.get(), events,
                              std::min(deadline, GetTickCount64() + 4000));
        events.Close();
        if (!view || !g_presentations.Owns(source, token))
            return;
        auto late = CaptureSelection(catalog, source, folder.get());
        if (!late)
            return;
        if (!late->children.empty())
            selection = std::move(late);
        complete = ApplySelection(view.Get(), folder.get(), *selection) &&
                   ActivateShellTab(created) &&
                   SameFolder(view.Get(), folder.get());
    }
    if (complete && !g_stop && g_presentations.Owns(source, token))
        (void)g_presentations.Retire(source, token);
}

// Hook: CreateWindowExW

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
   public:
    struct DeferredShows final {
        HWND window{};
        DWORD threadId{}, processId{};
        ULONG_PTR token{};
        std::vector<int> commands;

        [[nodiscard]] HANDLE Token() const noexcept {
            return reinterpret_cast<HANDLE>(token);
        }
        [[nodiscard]] bool Owns() const noexcept {
            DWORD process{};
            const DWORD thread = ::GetWindowThreadProcessId(window, &process);
            return thread != 0 && thread == threadId && process == processId &&
                   ::GetPropW(
                       window,
                       WindowPresentationStore::ConstructionPropertyName) ==
                       Token();
        }
        void Release() const noexcept {
            if (Owns())
                (void)::RemovePropW(
                    window, WindowPresentationStore::ConstructionPropertyName);
        }
        void Replay(decltype(&ShowWindow) show) {
            for (int command : commands) {
                if (!Owns())
                    break;
                (void)show(window, command);
            }
            commands.clear();
            Release();
        }
    };

   private:
    inline static thread_local ConstructionVisibilityScope* current_ = nullptr;
    inline static thread_local unsigned pending_ = 0;
    inline static std::atomic<ULONG_PTR> nextToken_{1};
    ConstructionVisibilityScope* previous_;
    bool active_;
    std::unordered_map<HWND, DeferredShows> deferred_;

   public:
    explicit ConstructionVisibilityScope(bool active) noexcept
        : previous_(current_), active_(active) {
        current_ = this;
        if (active_)
            ++pending_;
    }
    // Only restores TLS bookkeeping. The creation hook explicitly transfers
    // or replays requests before leaving the scope; no window calls here.
    ~ConstructionVisibilityScope() {
        if (active_)
            --pending_;
        current_ = previous_;
    }
    static bool Pending() noexcept { return pending_ != 0; }
    static bool DeferShow(HWND window, int command) {
        auto* scope = current_;
        // A nested explicit/bypassed creation must not inherit suppression.
        if (!scope || !scope->active_)
            return false;
        auto existing = scope->deferred_.find(window);
        if (existing != scope->deferred_.end()) {
            if (existing->second.Owns()) {
                try {
                    existing->second.commands.push_back(command);
                    return true;
                } catch (...) {
                    return false;
                }
            }
            scope->deferred_.erase(existing);
        }
        if (::GetPropW(window,
                       WindowPresentationStore::ConstructionPropertyName))
            return false;
        DeferredShows record;
        record.window = window;
        record.threadId = ::GetWindowThreadProcessId(window, &record.processId);
        if (!record.threadId)
            return false;
        ULONG_PTR value = nextToken_.load(std::memory_order_relaxed);
        while (value != (std::numeric_limits<ULONG_PTR>::max)()) {
            if (nextToken_.compare_exchange_weak(value, value + 1,
                                                 std::memory_order_relaxed))
                break;
        }
        if (value == (std::numeric_limits<ULONG_PTR>::max)())
            return false;
        record.token = value;
        try {
            record.commands.push_back(command);
            auto [found, inserted] =
                scope->deferred_.emplace(window, std::move(record));
            if (!inserted)
                return false;
            if (!::SetPropW(window,
                            WindowPresentationStore::ConstructionPropertyName,
                            found->second.Token()) ||
                !found->second.Owns()) {
                found->second.Release();
                scope->deferred_.erase(found);
                return false;
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    [[nodiscard]] DeferredShows* PendingFor(HWND window) {
        const auto found = deferred_.find(window);
        return found == deferred_.end() ? nullptr : &found->second;
    }
    void ReplayRemaining(decltype(&ShowWindow) show) {
        if (active_) {
            active_ = false;
            --pending_;
        }
        // Calls concerning other windows during construction are delayed only
        // until its return, then replayed rather than discarded.
        for (auto& item : deferred_)
            item.second.Replay(show);
        deferred_.clear();
    }
    ConstructionVisibilityScope(const ConstructionVisibilityScope&) = delete;
    ConstructionVisibilityScope& operator=(const ConstructionVisibilityScope&) =
        delete;
};
}  // namespace etr

static decltype(&CreateWindowExW) g_nextWindowBuild = nullptr;
static decltype(&ShowWindow) g_nextVisibilityChange = nullptr;
static BOOL WINAPI RouteWindowVisibility(HWND window, int request);

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
    const bool permitRouting = g_settings.mergeWindows &&
                               role == etr::CreatedWindowRole::ExplorerFrame &&
                               !g_stop.load() && !explicitFrame &&
                               (GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0;
    HWND destinationHint = permitRouting ? FastPrimary(nullptr) : nullptr;
    etr::ConstructionVisibilityScope constructing(destinationHint != nullptr);
    HWND result =
        g_nextWindowBuild(extendedStyle, classIdentifier, caption, style, left,
                          top, width, height, owner, menu, module, context);
    const DWORD nativeError = GetLastError();
    if (result) {
        if (explicitFrame)
            g_explicitWindowCreated = true;
        if (role == etr::CreatedWindowRole::ExplorerTab && !g_stop.load())
            ScheduleNewTab(result);
        if (destinationHint) {
            auto* deferred = constructing.PendingFor(result);
            std::vector<int> noShows;
            auto lease =
                !g_stop.load() && (!deferred || deferred->Owns())
                    ? g_presentations.Stage(
                          result, deferred ? &deferred->commands : &noShows,
                          deferred ? deferred->Token() : nullptr)
                    : std::nullopt;
            if (lease) {
                if (deferred)
                    deferred->Release();
                // Disable scope capture before any native replay, including
                // synchronous ShowWindow calls from a native window callback.
                constructing.ReplayRemaining(g_nextVisibilityChange);
                auto* request = new (std::nothrow)
                    MergeRequest{result, destinationHint, 0, *lease};
                if (request)
                    QueueRedirect(request);
                else
                    (void)g_presentations.Reveal(result, *lease);
            }
        }
    }
    // Failure uses the same disabled-scope replay as success. Keep each
    // construction token through replay so a recycled HWND is never targeted.
    constructing.ReplayRemaining(g_nextVisibilityChange);
    SetLastError(nativeError);
    return result;
}

static BOOL WINAPI RouteWindowVisibility(HWND window, int request) {
    if (!g_stop.load() &&
        etr::WindowPresentationStore::IsExplorerFrame(window) &&
        (g_presentations.DeferShow(window, request) ||
         etr::ConstructionVisibilityScope::DeferShow(window, request)))
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
    if (g_settings.activateShellLookups && SUCCEEDED(hr) && hr != S_FALSE &&
        out && *out && cls != SWC_DESKTOP && (options & SWFO_NEEDDISPATCH) &&
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
    if (!shellService) {
        HMODULE shell = LoadLibraryExW(L"shell32.dll", nullptr,
                                       LOAD_LIBRARY_SEARCH_SYSTEM32);
        WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
            {{L"private: long __cdecl CDefView::_InvokeContextMenu(struct "
              L"IContextMenu *,struct _CMINVOKECOMMANDINFOEX *)"},
             &InvokeFolderMenu_Original,
             InvokeFolderMenu_Hook,
             true},
        };
        if (!shell || !WindhawkUtils::HookSymbols(shell, shell32DllHooks,
                                                  std::size(shell32DllHooks)))
            return false;
        if (!InvokeFolderMenu_Original)
            Wh_Log(
                L"Folder-menu activation unavailable: exact menu symbol "
                L"not found");
    }

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
        {{L"public: virtual long __cdecl CExecuteFolder::Execute(void)"},
         &FolderExecute_Original,
         FolderExecute_Hook,
         true},
        {{L"public: virtual long __cdecl CExplorerFrame::AddTabAtIndex(struct "
          L"_ITEMIDLIST_ABSOLUTE const *,enum NewTabEntryPoint,int,struct "
          L"_GUID *)"},
         &AddTabAtIndex_Original,
         AddTabAtIndex_Hook,
         true},
    };
    const size_t count = shellService ? 1 : std::size(shellHooks);
    if (!WindhawkUtils::HookSymbols(module, shellHooks, count))
        return false;
    if (!shellService && !AddTabAtIndex_Original)
        Wh_Log(
            L"Explicit new-tab activation unavailable: exact AddTabAtIndex "
            L"symbol not found");
    return true;
}

BOOL Wh_ModInit() {
    LoadSettings();
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
    if (!g_intentProperties.Initialize())
        Wh_Log(
            L"Cross-process intent channel initialization failed; local "
            L"fallback only");
    if (!InstallExplorerFrameHooks(false))
        return FALSE;
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, RouteWindowCreation,
                                        &g_nextWindowBuild))
        return FALSE;
    if (!WindhawkUtils::SetFunctionHook(ShowWindow, RouteWindowVisibility,
                                        &g_nextVisibilityChange))
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
    g_intentProperties.Close();
}
void Wh_ModUninit() {
    CloseRouter();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
