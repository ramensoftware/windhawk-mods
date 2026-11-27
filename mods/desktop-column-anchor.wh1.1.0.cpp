// ==WindhawkMod==
// @id              desktop-column-anchor
// @name            DesktopColumnAnchor
// @name:zh-CN      桌面列锚
// @description     Anchors newly created desktop files from external apps to a customizable column with overflow direction control, while leaving desktop right-click "New" and drag-and-drop placements untouched.
// @description:zh-CN 当其他软件向桌面新增文件时，自动将其锚定到自定义列位置（支持正向/反向列计数与列满扩展方向），同时保留桌面右键“新建”和拖拽到桌面的原始落点。
// @version         1.1.0
// @github          https://github.com/chaochenzhudian
// @author          chaochenzhudian
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DesktopColumnAnchor（桌面列锚）

Anchor newly added desktop files (saved, downloaded, copied, or created by
external software) to a specific desktop column, without interfering with items
created via the desktop right-click **New** menu or files dragged and dropped
onto the desktop.

当外部软件向桌面保存、下载、复制或创建新文件时，自动将其锚定到指定的桌面列，同时不干扰桌面右键 **新建** 菜单创建的项目以及直接拖拽到桌面的文件。

## Features / 功能特性

- **Customizable target column / 自定义目标列位置**：
  - Positive values count from left to right (`1` = leftmost column, `3` = 3rd
    column from the left).

    正数表示从左向右第 N 列（`1` = 最左第 1 列，`3` = 从左数第 3 列）。
  - Negative values count from right to left (`-1` = rightmost column, `-3` =
    3rd column from the right).

    负数表示从右向左第 N 列（`-1` = 最右第 1 列，`-3` = 从右数第 3 列）。
- **Column capacity & overflow direction / 列容量与相邻列扩展方向**：
  - Configure maximum icons per column (`0` = automatically fit desktop rows).

    可自定义每列最多容纳的图标数量（`0` = 自动适配当前桌面单列最大行数）。
  - Configure overflow direction when the target column is full (search
    rightward or leftward).

    可自定义目标列已满时向相邻列寻找空位的方向（向右或向左寻找可用列）。
  - Edge column rule: when targeting the leftmost column, overflow defaults to
    rightward; when targeting the rightmost column, overflow defaults to
    leftward. If the primary direction reaches the screen edge without free
    slots, the mod falls back to the opposite direction.

    边缘列规则：当目标列为从左数第 1 列时默认向右寻找；当目标列为从右数第 1 列时默认向左寻找。若首选方向直到屏幕边缘都已满，将自动回退向反方向寻找。
- **First-load Auto Arrange disable / 首次加载自动关闭“自动排列图标”**：
  - Automatically clears `FWF_AUTOARRANGE` on the desktop folder view upon first
    load so custom column placement takes effect, while preserving Snap to Grid.

    首次加载时自动清除桌面文件夹视图的 `FWF_AUTOARRANGE` 标志以关闭“自动排列图标”，使自定义列定位生效，同时保留“将图标与网格对齐”。
- **Smart source filtering / 精准排除右键新建与拖拽**：
  - Leaves desktop right-click **New** items and files dragged/dropped onto the
    desktop at their native positions.

    桌面右键 **新建** 的项目及直接拖拽到桌面的文件均保留在原落点，不受干预。
  - Ignores common temporary download files (`.crdownload`, `.part`, `.tmp`,
    etc.) until renamed to their final filename.

    自动忽略常见临时下载文件（`.crdownload`、`.part`、`.tmp` 等），待其重命名为正式文件后再执行列锚定。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- anchorColumn: -1
  $name: Anchor column
  $name:zh-CN: 目标列位置
  $description: >-
    Positive numbers count from left to right (e.g. 1 = leftmost column, 3 =
    3rd column from left). Negative numbers count from right to left (e.g. -1 =
    rightmost column, -3 = 3rd column from right). 0 is treated as 1.
  $description:zh-CN: >-
    正数表示从左向右第 N 列（例如 1 为最左第 1 列，3 为从左数第 3 列）；
    负数表示从右向左第 N 列（例如 -1 为最右第 1 列，-3 为从右数第 3 列）。
    填 0 时按 1 处理。
- overflowDirection: "-1"
  $name: Overflow direction when column is full
  $name:zh-CN: 目标列已满时的相邻列扩展方向
  $description: >-
    Direction to search for the next available column when the target column is
    full. Note: when targeting the leftmost column, it defaults to rightward;
    when targeting the rightmost column, it defaults to leftward.
  $description:zh-CN: >-
    当设定的目标列已满时，向相邻列寻找可用空位的方向。
    注意：当目标列为从左数第 1 列时默认向右寻找；当目标列为从右数第 1 列时默认向左寻找。
    若首选方向直到屏幕边缘都已满，将自动回退向反方向寻找。
  $options:
  - "1": Rightward
  - "-1": Leftward
  $options:zh-CN:
  - "1": 向右寻找可用列
  - "-1": 向左寻找可用列
- columnCapacity: 0
  $name: Column capacity (0 = auto)
  $name:zh-CN: 单列容量（0 表示自动）
  $description: >-
    Maximum number of icons allowed in each column before overflowing to an
    adjacent column. Set to 0 to automatically use the full row count of the
    current desktop grid.
  $description:zh-CN: >-
    每列最多容纳的图标数量，超过后向相邻列继续新增。
    设为 0 表示自动适配当前桌面网格单列最大行数；设为正整数时不超过桌面实际最大行数。
- anchorDelayMs: 350
  $name: Context menu disambiguation delay (ms)
  $name:zh-CN: 右键菜单消歧延迟（毫秒）
  $description: >-
    Delay in milliseconds applied only right after closing the desktop
    right-click menu, to avoid racing with right-click "New" inline rename.
    External app file creations are anchored immediately before the first paint
    without waiting for this delay. Range: 100-3000.
  $description:zh-CN: >-
    仅在桌面右键菜单刚关闭后的短暂窗口内生效的消歧等待时间（范围 100-3000，默认 350），
    用于避免与桌面右键“新建”进入重命名状态竞争；其他软件新增文件会在首次绘制前直接同步锚定，
    不受此延迟阻塞。
- ignoreTempFiles: true
  $name: Ignore temporary download files
  $name:zh-CN: 忽略临时下载文件
  $description: >-
    Skip anchoring temporary files (e.g. .tmp, .crdownload, .part, .partial,
    .download, ~$*) until they are renamed to their final file name.
  $description:zh-CN: >-
    忽略常见下载或编辑临时文件（如 .tmp、.crdownload、.part、.partial、.download、~$ 开头文件），
    待其下载完成并重命名为正式文件后再执行列锚定。
- disableAutoArrangeOnEveryLoad: true
  $name: Disable "Auto arrange icons" on every load
  $name:zh-CN: 每次加载时均关闭“自动排列图标”
  $description: >-
    By default, the mod disables desktop "Auto arrange icons" once on first
    load. Enable this option to also disable it on every subsequent mod reload
    or Explorer restart. (Precisely positioning desktop icons with this mod
    generally only works when "Auto arrange icons" is disabled.)
  $description:zh-CN: >-
    默认仅在 mod 首次加载时自动关闭桌面“自动排列图标”。
    开启此项后，每次重新加载 mod 或重启资源管理器时都会再次检查并关闭该选项。（通过这个mod来精确设置桌面图标的位置，通常只有在“自动排列图标”被禁用的情况下才能生效）
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <exdisp.h>
#include <knownfolders.h>
#include <ole2.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

// ============================================================================
// Constants & Settings
// ============================================================================

constexpr PCWSTR kMsgWindowClassName = L"Windhawk_DesktopColumnAnchor_MsgWnd";
constexpr PCWSTR kFirstLoadValueName = L"AutoArrangeDisabledOnce";

constexpr UINT_PTR kTimerSyncDesktop = 1;
constexpr UINT_PTR kTimerRetryInit = 2;
constexpr UINT_PTR kTimerPendingFlush = 3;

constexpr ULONGLONG kDropExcludeTtlMs = 60000;
constexpr ULONGLONG kDropBurstWindowMs = 3000;
constexpr ULONGLONG kShellExplicitExcludeTtlMs = 20000;
constexpr ULONGLONG kContextMenuWindowMs = 2500;
constexpr ULONGLONG kContextMenuOpenMaxMs = 60000;
constexpr ULONGLONG kLabelEditExcludeTtlMs = 20000;
constexpr ULONGLONG kPendingViewItemTtlMs = 2000;
constexpr ULONGLONG kRecentRemovedKnownTtlMs = 350;
constexpr ULONGLONG kAnchoredGuardTtlMs = 1500;
constexpr UINT kPendingFlushIntervalMs = 25;
constexpr int kAnchoredGuardMaxReasserts = 4;

constexpr UINT WM_APP_SHCHANGE = WM_APP + 101;
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 103;
constexpr UINT WM_APP_CLEANUP = WM_APP + 104;
constexpr UINT WM_APP_SYNC_NOW = WM_APP + 105;

// Verified COM vtable indices (0-based) against Windows SDK / MinGW headers:
// - IDropTarget (oleidl.h): QueryInterface(0), AddRef(1), Release(2),
//   DragEnter(3), DragOver(4), DragLeave(5), Drop(6)
constexpr size_t kVtblIdx_IDropTarget_Drop = 6;
// - IShellView2 (shobjidl.h): IUnknown(0..2), IOleWindow(3..4),
//   IShellView(5..15), GetView(16), CreateViewWindow2(17), HandleRename(18),
//   SelectAndPositionItem(19)
constexpr size_t kVtblIdx_IShellView2_SelectAndPositionItem = 19;
// - IFolderView (shobjidl.h): IUnknown(0..2), GetCurrentViewMode(3)..
//   SelectItem(15), SelectAndPositionItems(16)
constexpr size_t kVtblIdx_IFolderView_SelectAndPositionItems = 16;

struct ModSettings {
    int anchorColumn = -1;
    int overflowDirection = -1;  // 1 = right, -1 = left
    int columnCapacity = 0;     // 0 = auto (gridRows)
    int anchorDelayMs = 350;
    bool ignoreTempFiles = true;
    bool disableAutoArrangeOnEveryLoad = true;
};

struct ExcludeEntry {
    ULONGLONG expireTick = 0;
    std::wstring reason;
};

struct DropStemEntry {
    std::wstring stemLower;
    ULONGLONG expireTick = 0;
};

enum class FsEventType {
    Added,
    Removed,
    Renamed,
};

struct FsEvent {
    FsEventType type = FsEventType::Added;
    std::wstring name1;  // Lowercase filename (or old name for Renamed)
    std::wstring name2;  // Lowercase new filename for Renamed
    ULONGLONG tick = 0;
};

struct AnchoredGuard {
    std::wstring key;
    PITEMID_CHILD pidl = nullptr;  // Owned via ILCloneChild, freed via CoTaskMemFree
    POINT targetPos{};
    int tolX = 24;
    int tolY = 24;
    ULONGLONG expireTick = 0;
    int reassertCount = 0;
};

struct DesktopItemInfo {
    std::wstring key;         // Normalized lowercase filename (with extension)
    std::wstring displayName; // Raw display/parsing name for logging
    POINT pos{};
    bool hasPos = false;
    PITEMID_CHILD pidl = nullptr;  // Owned, freed via CoTaskMemFree
};

struct DesktopGridMetrics {
    RECT workArea{};
    int cellW = 0;
    int cellH = 0;
    int originX = 0;
    int originY = 0;
    int totalCols = 1;
    int gridRows = 1;
    bool valid = false;
};

// ============================================================================
// Minimal RAII Helpers (No ATL dependency)
// ============================================================================

template <typename T>
class ComPtr {
   public:
    ComPtr() = default;
    ~ComPtr() { Reset(); }

    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;

    ComPtr(ComPtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* Get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

    T** Put() {
        Reset();
        return &ptr_;
    }

    void** PutVoid() { return reinterpret_cast<void**>(Put()); }

    void Reset() {
        if (ptr_) {
            ptr_->Release();
            ptr_ = nullptr;
        }
    }

   private:
    T* ptr_ = nullptr;
};

template <typename T>
class CoTaskMemPtr {
   public:
    CoTaskMemPtr() = default;
    ~CoTaskMemPtr() { Reset(); }

    CoTaskMemPtr(const CoTaskMemPtr&) = delete;
    CoTaskMemPtr& operator=(const CoTaskMemPtr&) = delete;

    T* Get() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

    T** Put() {
        Reset();
        return &ptr_;
    }

    void Reset() {
        if (ptr_) {
            CoTaskMemFree(ptr_);
            ptr_ = nullptr;
        }
    }

   private:
    T* ptr_ = nullptr;
};

// ============================================================================
// Global State
// ============================================================================

std::mutex g_settingsMutex;
ModSettings g_settings;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_desktopReady{false};
std::atomic<bool> g_comHooksInstalled{false};
std::atomic<bool> g_baselineLoaded{false};

std::atomic<HWND> g_hDefView{nullptr};
std::atomic<HWND> g_hListView{nullptr};
std::atomic<HWND> g_hMsgWnd{nullptr};
std::atomic<DWORD> g_desktopThreadId{0};
std::atomic<void*> g_desktopDropTargetPtr{nullptr};
std::atomic<void*> g_lvDropTargetPtr{nullptr};
std::atomic<void*> g_defViewDropTargetPtr{nullptr};

HANDLE g_initThread = nullptr;
HANDLE g_fsWatchThread = nullptr;
HANDLE g_stopEvent = nullptr;

// Lock-free fast-path hint checked at WM_PAINT entry before drawing.
std::atomic<bool> g_pendingAnchorCheck{false};

// File-system watcher queue (written by g_fsWatchThread, drained on desktop UI thread).
std::mutex g_fsMutex;
std::vector<FsEvent> g_fsEvents;

// Only accessed on the desktop UI thread.
ULONG g_shNotifyUser = 0;
ULONG g_shNotifyPublic = 0;
bool g_defViewSubclassed = false;
bool g_listViewSubclassed = false;
int g_lastObservedLvCount = -1;
bool g_pendingFlushTimerActive = false;
ComPtr<IFolderView2> g_cachedFolderView2;
ComPtr<IShellFolder> g_cachedDesktopFolder;
std::unordered_set<std::wstring> g_knownItems;
std::unordered_map<std::wstring, ULONGLONG> g_pendingViewItems;
std::unordered_map<std::wstring, ULONGLONG> g_recentlyRemovedKnown;
std::vector<AnchoredGuard> g_recentAnchoredGuards;
std::wstring g_desktopUserDirLower;
std::wstring g_desktopPublicDirLower;

// Exclusion tracking (protected by g_excludeMutex for safety across hooks).
std::mutex g_excludeMutex;
std::unordered_map<std::wstring, ExcludeEntry> g_excludedNames;
std::unordered_map<std::wstring, int> g_inFlightDragNames;
std::vector<DropStemEntry> g_recentDropStems;
ULONGLONG g_lastDropTick = 0;
ULONGLONG g_lastContextMenuTick = 0;
POINT g_lastContextMenuClientPt{0, 0};
bool g_hasContextMenuPt = false;
bool g_inDesktopContextMenu = false;
bool g_contextMenuPendingClose = false;

thread_local bool g_inSelfPositioning = false;
thread_local bool g_inPrePaintSync = false;

// Original function pointers for Windhawk hooks.
using DropFn = HRESULT(STDMETHODCALLTYPE*)(IDropTarget* pThis,
                                           IDataObject* pDataObj,
                                           DWORD grfKeyState,
                                           POINTL pt,
                                           DWORD* pdwEffect);
using SelectAndPositionItemsFn =
    HRESULT(STDMETHODCALLTYPE*)(IFolderView* pThis,
                                UINT cidl,
                                PCUITEMID_CHILD_ARRAY apidl,
                                POINT* apt,
                                DWORD dwFlags);
using SelectAndPositionItemFn =
    HRESULT(STDMETHODCALLTYPE*)(IShellView2* pThis,
                                PCUITEMID_CHILD pidlItem,
                                UINT uFlags,
                                POINT* ppt);
using DoDragDropFn = HRESULT(WINAPI*)(IDataObject* pDataObj,
                                      IDropSource* pDropSource,
                                      DWORD dwOKEffects,
                                      DWORD* pdwEffect);
using SHDoDragDropFn = HRESULT(WINAPI*)(HWND hwnd,
                                        IDataObject* pdtobj,
                                        IDropSource* pdsrc,
                                        DWORD dwEffect,
                                        DWORD* pdwEffect);

constexpr size_t kMaxDropHookSlots = 4;
DropFn g_origDropSlots[kMaxDropHookSlots] = {};
void* g_hookedDropAddrs[kMaxDropHookSlots] = {};
size_t g_dropHookSlotCount = 0;

SelectAndPositionItemsFn g_origSelectAndPositionItems = nullptr;
SelectAndPositionItemFn g_origSelectAndPositionItem = nullptr;
DoDragDropFn g_origDoDragDrop = nullptr;
SHDoDragDropFn g_origSHDoDragDrop = nullptr;

// ============================================================================
// String & Path Helpers
// ============================================================================

std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) {
        ch = static_cast<wchar_t>(std::towlower(ch));
    }
    return s;
}

std::wstring ExtractFileNameFromPath(const std::wstring& path) {
    if (path.empty()) {
        return {};
    }
    size_t end = path.size();
    while (end > 0 && (path[end - 1] == L'\\' || path[end - 1] == L'/')) {
        --end;
    }
    if (end == 0) {
        return {};
    }
    size_t pos = path.find_last_of(L"\\/", end - 1);
    if (pos == std::wstring::npos) {
        return path.substr(0, end);
    }
    return path.substr(pos + 1, end - pos - 1);
}

std::wstring ExtractStemLower(const std::wstring& fileNameLower) {
    size_t dot = fileNameLower.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) {
        return fileNameLower;
    }
    return fileNameLower.substr(0, dot);
}

bool EndsWith(const std::wstring& value, const std::wstring& suffix) {
    if (suffix.size() > value.size()) {
        return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

bool IsTemporaryFileName(const std::wstring& nameLower) {
    if (nameLower.empty()) {
        return true;
    }
    // Office / WPS lock & temp files
    if (nameLower.rfind(L"~$", 0) == 0 || nameLower.rfind(L".~", 0) == 0) {
        return true;
    }
    static const wchar_t* const kTempSuffixes[] = {
        L".tmp",
        L".temp",
        L".crdownload",
        L".part",
        L".partial",
        L".download",
        L".opdownload",
        L".!ut",
        L".!qb",
        L".filepart",
        L".aria2",
    };
    for (const wchar_t* suffix : kTempSuffixes) {
        if (EndsWith(nameLower, suffix)) {
            return true;
        }
    }
    return false;
}

std::wstring GetDesktopKnownFolderLower(REFKNOWNFOLDERID rfid) {
    CoTaskMemPtr<WCHAR> path;
    if (FAILED(SHGetKnownFolderPath(rfid, 0, nullptr, path.Put())) || !path) {
        return {};
    }
    std::wstring result = ToLower(path.Get());
    while (!result.empty() && result.back() == L'\\') {
        result.pop_back();
    }
    return result;
}

bool IsDirectChildOfDesktop(const std::wstring& fullPathLower,
                            std::wstring* outFileNameLower) {
    if (fullPathLower.empty()) {
        return false;
    }
    size_t slash = fullPathLower.find_last_of(L'\\');
    if (slash == std::wstring::npos || slash == 0 ||
        slash + 1 >= fullPathLower.size()) {
        return false;
    }
    std::wstring parent = fullPathLower.substr(0, slash);
    if ((!g_desktopUserDirLower.empty() && parent == g_desktopUserDirLower) ||
        (!g_desktopPublicDirLower.empty() &&
         parent == g_desktopPublicDirLower)) {
        if (outFileNameLower) {
            *outFileNameLower = fullPathLower.substr(slash + 1);
        }
        return true;
    }
    return false;
}

// ============================================================================
// Settings Loading
// ============================================================================

ModSettings GetSettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

void LoadSettings() {
    ModSettings loaded;

    loaded.anchorColumn = Wh_GetIntSetting(L"anchorColumn");
    if (loaded.anchorColumn == 0) {
        loaded.anchorColumn = 1;
    }

    auto overflowStr = WindhawkUtils::StringSetting::make(L"overflowDirection");
    if (overflowStr && overflowStr[0] != L'\0') {
        int parsed = _wtoi(overflowStr.get());
        loaded.overflowDirection = (parsed > 0) ? 1 : -1;
    } else {
        int raw = Wh_GetIntSetting(L"overflowDirection");
        loaded.overflowDirection = (raw > 0) ? 1 : -1;
    }

    loaded.columnCapacity = Wh_GetIntSetting(L"columnCapacity");
    if (loaded.columnCapacity < 0) {
        loaded.columnCapacity = 0;
    }

    loaded.anchorDelayMs = Wh_GetIntSetting(L"anchorDelayMs");
    if (loaded.anchorDelayMs < 100) {
        loaded.anchorDelayMs = 100;
    } else if (loaded.anchorDelayMs > 3000) {
        loaded.anchorDelayMs = 3000;
    }

    loaded.ignoreTempFiles = Wh_GetIntSetting(L"ignoreTempFiles") != 0;
    loaded.disableAutoArrangeOnEveryLoad =
        Wh_GetIntSetting(L"disableAutoArrangeOnEveryLoad") != 0;

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = loaded;
    }

    Wh_Log(
        L"Settings loaded: anchorColumn=%d, overflowDirection=%d, "
        L"columnCapacity=%d, anchorDelayMs=%d, ignoreTempFiles=%d, "
        L"disableAutoArrangeOnEveryLoad=%d",
        loaded.anchorColumn, loaded.overflowDirection, loaded.columnCapacity,
        loaded.anchorDelayMs, loaded.ignoreTempFiles ? 1 : 0,
        loaded.disableAutoArrangeOnEveryLoad ? 1 : 0);
}

// ============================================================================
// Exclusion Tracking (Right-Click New & Drag-and-Drop)
// ============================================================================

void PruneExclusionsLocked(ULONGLONG now) {
    for (auto it = g_excludedNames.begin(); it != g_excludedNames.end();) {
        if (it->second.expireTick <= now) {
            it = g_excludedNames.erase(it);
        } else {
            ++it;
        }
    }
    g_recentDropStems.erase(
        std::remove_if(g_recentDropStems.begin(), g_recentDropStems.end(),
                       [now](const DropStemEntry& e) {
                           return e.expireTick <= now;
                       }),
        g_recentDropStems.end());
}

void MarkNameExcluded(const std::wstring& rawName,
                      ULONGLONG ttlMs,
                      PCWSTR reason,
                      bool alsoTrackDropStem = false) {
    std::wstring name = ExtractFileNameFromPath(rawName);
    if (name.empty()) {
        return;
    }
    std::wstring key = ToLower(name);
    ULONGLONG now = GetTickCount64();
    ULONGLONG expire = now + ttlMs;

    std::lock_guard<std::mutex> lock(g_excludeMutex);
    PruneExclusionsLocked(now);
    auto it = g_excludedNames.find(key);
    if (it == g_excludedNames.end() || it->second.expireTick < expire) {
        g_excludedNames[key] = ExcludeEntry{expire, reason ? reason : L""};
    }

    if (alsoTrackDropStem) {
        std::wstring stem = ExtractStemLower(key);
        if (!stem.empty()) {
            g_recentDropStems.push_back(DropStemEntry{stem, expire});
        }
    }

    Wh_Log(L"Excluded [%ls] (%ls, ttl=%llums)", key.c_str(),
           reason ? reason : L"", ttlMs);
}

bool IsItemExcluded(const std::wstring& keyLower,
                    const POINT& currentPos,
                    bool isSelectedOrEditing,
                    const DesktopGridMetrics& grid,
                    std::wstring* outReason) {
    ULONGLONG now = GetTickCount64();

    std::lock_guard<std::mutex> lock(g_excludeMutex);
    PruneExclusionsLocked(now);

    // 1. Direct filename match (Drop, SelectAndPositionItem(s), EditLabel).
    auto it = g_excludedNames.find(keyLower);
    if (it != g_excludedNames.end()) {
        if (outReason) {
            *outReason = it->second.reason;
        }
        return true;
    }

    // 1b. Active in-flight Explorer drag-and-drop payload (covers immediate
    //     file creation before DoDragDrop returns, e.g. replace dialogs).
    if (g_inFlightDragNames.find(keyLower) != g_inFlightDragNames.end()) {
        if (outReason) {
            *outReason = L"InFlightDragDrop";
        }
        return true;
    }

    // 2. Drop duplicate-copy naming fallback (e.g., "foo - Copy.txt",
    //    "foo - 副本.txt", "foo (2).txt") while drop stems are still active.
    if (!g_recentDropStems.empty()) {
        std::wstring stem = ExtractStemLower(keyLower);
        for (const auto& entry : g_recentDropStems) {
            if (stem == entry.stemLower) {
                if (outReason) {
                    *outReason = L"DropStemExact";
                }
                return true;
            }
            if (stem.size() > entry.stemLower.size() + 2 &&
                stem.rfind(entry.stemLower, 0) == 0) {
                wchar_t nextCh = stem[entry.stemLower.size()];
                if (nextCh == L' ' || nextCh == L'(' || nextCh == L'-') {
                    if (outReason) {
                        *outReason = L"DropStemDuplicate";
                    }
                    return true;
                }
            }
        }
    }

    // 3. Short burst window immediately after a desktop drop (handles virtual
    //    or renamed drop payloads without CF_HDROP).
    if (g_lastDropTick != 0 && now >= g_lastDropTick &&
        (now - g_lastDropTick) <= kDropBurstWindowMs) {
        if (outReason) {
            *outReason = L"RecentDropBurst";
        }
        return true;
    }

    // 4. Recent desktop right-click menu placement near the right-click point
    //    (extra safeguard for Win11 XAML context menu / New -> Shortcut).
    if (g_hasContextMenuPt && g_lastContextMenuTick != 0 &&
        now >= g_lastContextMenuTick &&
        (now - g_lastContextMenuTick) <= kContextMenuWindowMs &&
        grid.valid && isSelectedOrEditing) {
        int dx = std::abs(currentPos.x - g_lastContextMenuClientPt.x);
        int dy = std::abs(currentPos.y - g_lastContextMenuClientPt.y);
        int threshX = std::max(grid.cellW * 2, 160);
        int threshY = std::max(grid.cellH * 2, 160);
        if (dx <= threshX && dy <= threshY) {
            if (outReason) {
                *outReason = L"RecentContextMenuNearby";
            }
            return true;
        }
    }

    return false;
}

bool IsForegroundExternalApp() {
    HWND hFg = GetForegroundWindow();
    if (!hFg) {
        return false;
    }
    DWORD fgPid = 0;
    GetWindowThreadProcessId(hFg, &fgPid);
    if (fgPid == 0 || fgPid == GetCurrentProcessId()) {
        return false;
    }
    WCHAR cls[64] = {};
    if (GetClassNameW(hFg, cls, ARRAYSIZE(cls))) {
        if (_wcsicmp(cls, L"#32768") == 0 ||
            _wcsicmp(cls, L"Xaml_WindowedPopupClass") == 0) {
            return false;
        }
    }
    return true;
}

bool IsDesktopContextMenuGraceActive(ULONGLONG now, ULONGLONG graceMs) {
    bool externalFg = IsForegroundExternalApp();
    std::lock_guard<std::mutex> lock(g_excludeMutex);

    if (g_inDesktopContextMenu) {
        return true;
    }

    if (g_contextMenuPendingClose) {
        if (externalFg) {
            g_contextMenuPendingClose = false;
            return false;
        }
        if (g_lastContextMenuTick != 0 && now >= g_lastContextMenuTick &&
            (now - g_lastContextMenuTick) <= kContextMenuOpenMaxMs) {
            return true;
        }
        g_contextMenuPendingClose = false;
    }

    if (!externalFg && g_lastContextMenuTick != 0 &&
        now >= g_lastContextMenuTick &&
        (now - g_lastContextMenuTick) <= graceMs) {
        return true;
    }

    return false;
}

// ============================================================================
// Desktop Window & Shell COM Helpers
// ============================================================================

HWND GetOwnProcessProgman() {
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (!hProgman) {
        return nullptr;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hProgman, &pid);
    if (pid != GetCurrentProcessId()) {
        return nullptr;
    }
    return hProgman;
}

struct EnumDefViewData {
    DWORD currentPid = 0;
    HWND hDefView = nullptr;
};

BOOL CALLBACK EnumWorkerWForDefView(HWND hWnd, LPARAM lParam) {
    auto* data = reinterpret_cast<EnumDefViewData*>(lParam);
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != data->currentPid) {
        return TRUE;
    }
    WCHAR cls[32] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, L"WorkerW") != 0) {
        return TRUE;
    }
    HWND hDef = FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
    if (hDef) {
        data->hDefView = hDef;
        return FALSE;
    }
    return TRUE;
}

bool FindDesktopWindows(HWND* outDefView, HWND* outListView) {
    HWND hProgman = GetOwnProcessProgman();
    if (!hProgman) {
        return false;
    }

    HWND hDefView =
        FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!hDefView) {
        EnumDefViewData data{GetCurrentProcessId(), nullptr};
        EnumWindows(EnumWorkerWForDefView, reinterpret_cast<LPARAM>(&data));
        hDefView = data.hDefView;
    }

    if (!hDefView) {
        return false;
    }

    HWND hListView = FindWindowExW(hDefView, nullptr, L"SysListView32", nullptr);
    if (!hListView) {
        return false;
    }

    if (outDefView) {
        *outDefView = hDefView;
    }
    if (outListView) {
        *outListView = hListView;
    }
    return true;
}

HRESULT AcquireDesktopShellView(ComPtr<IShellView>& outView) {
    ComPtr<IShellWindows> spShellWindows;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(spShellWindows.Put()));
    if (FAILED(hr) || !spShellWindows) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    VARIANT vtLoc{};
    vtLoc.vt = VT_I4;
    vtLoc.lVal = CSIDL_DESKTOP;
    VARIANT vtEmpty{};
    long lhwnd = 0;
    ComPtr<IDispatch> spDisp;

    hr = spShellWindows->FindWindowSW(&vtLoc, &vtEmpty, SWC_DESKTOP, &lhwnd,
                                      SWFO_NEEDDISPATCH, spDisp.Put());
    if (FAILED(hr) || !spDisp) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ComPtr<IServiceProvider> spProv;
    hr = spDisp->QueryInterface(IID_PPV_ARGS(spProv.Put()));
    if (FAILED(hr) || !spProv) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ComPtr<IShellBrowser> spBrowser;
    hr = spProv->QueryService(SID_STopLevelBrowser,
                              IID_PPV_ARGS(spBrowser.Put()));
    if (FAILED(hr) || !spBrowser) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    return spBrowser->QueryActiveShellView(outView.Put());
}

HRESULT AcquireDesktopFolderView2(ComPtr<IFolderView2>& outFolderView2) {
    ComPtr<IShellView> spView;
    HRESULT hr = AcquireDesktopShellView(spView);
    if (FAILED(hr) || !spView) {
        return FAILED(hr) ? hr : E_FAIL;
    }
    return spView->QueryInterface(IID_PPV_ARGS(outFolderView2.Put()));
}

void ResetDesktopComCache() {
    g_cachedDesktopFolder.Reset();
    g_cachedFolderView2.Reset();
}

IFolderView2* GetDesktopFolderView2Cached() {
    if (g_cachedFolderView2) {
        return g_cachedFolderView2.Get();
    }
    ComPtr<IFolderView2> spFv2;
    if (SUCCEEDED(AcquireDesktopFolderView2(spFv2)) && spFv2) {
        g_cachedFolderView2 = std::move(spFv2);
        g_cachedDesktopFolder.Reset();
        g_cachedFolderView2->GetFolder(
            IID_PPV_ARGS(g_cachedDesktopFolder.Put()));
        return g_cachedFolderView2.Get();
    }
    return nullptr;
}

std::wstring GetPidlItemName(IShellFolder* pFolder, PCUITEMID_CHILD pidl) {
    if (!pFolder || !pidl) {
        return {};
    }
    STRRET sr{};
    if (SUCCEEDED(pFolder->GetDisplayNameOf(
            pidl, SHGDN_INFOLDER | SHGDN_FORPARSING, &sr))) {
        WCHAR buf[MAX_PATH * 2] = {};
        if (SUCCEEDED(StrRetToBufW(&sr, pidl, buf, ARRAYSIZE(buf))) &&
            buf[0] != L'\0') {
            return ExtractFileNameFromPath(buf);
        }
    }
    STRRET srNormal{};
    if (SUCCEEDED(pFolder->GetDisplayNameOf(pidl, SHGDN_NORMAL, &srNormal))) {
        WCHAR buf[MAX_PATH * 2] = {};
        if (SUCCEEDED(StrRetToBufW(&srNormal, pidl, buf, ARRAYSIZE(buf))) &&
            buf[0] != L'\0') {
            return buf;
        }
    }
    return {};
}

bool IsDesktopComObject(IUnknown* pUnk) {
    if (!pUnk) {
        return false;
    }
    HWND hDefView = g_hDefView.load();
    if (!hDefView) {
        return false;
    }
    DWORD desktopTid = g_desktopThreadId.load();
    if (desktopTid != 0 && GetCurrentThreadId() != desktopTid) {
        return false;
    }

    ComPtr<IShellView> spView;
    if (SUCCEEDED(pUnk->QueryInterface(IID_PPV_ARGS(spView.Put()))) && spView) {
        HWND hWnd = nullptr;
        if (SUCCEEDED(spView->GetWindow(&hWnd)) && hWnd == hDefView) {
            return true;
        }
        return false;
    }
    return pUnk == g_desktopDropTargetPtr.load();
}

bool IsPointOverDesktop(POINT screenPt) {
    HWND hUnder = WindowFromPoint(screenPt);
    if (!hUnder) {
        return false;
    }
    HWND hLV = g_hListView.load();
    HWND hDef = g_hDefView.load();
    for (HWND cur = hUnder; cur != nullptr; cur = GetParent(cur)) {
        if ((hLV && cur == hLV) || (hDef && cur == hDef)) {
            return true;
        }
        WCHAR cls[64] = {};
        if (GetClassNameW(cur, cls, ARRAYSIZE(cls))) {
            if (_wcsicmp(cls, L"Progman") == 0) {
                return true;
            }
            if (_wcsicmp(cls, L"WorkerW") == 0 && hDef &&
                IsChild(cur, hDef)) {
                return true;
            }
        }
    }
    return false;
}

bool IsAddressInComProxyModule(void* addr, std::wstring* outModuleName = nullptr) {
    HMODULE hMod = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(addr), &hMod) ||
        !hMod) {
        return false;
    }
    WCHAR modPath[MAX_PATH] = {};
    if (GetModuleFileNameW(hMod, modPath, ARRAYSIZE(modPath)) == 0) {
        return false;
    }
    std::wstring name = ToLower(ExtractFileNameFromPath(modPath));
    if (outModuleName) {
        *outModuleName = name;
    }
    return name == L"combase.dll" || name == L"rpcrt4.dll";
}

bool IsDesktopDropInvocation(IDropTarget* pThis, POINTL pt) {
    if (!pThis) {
        return false;
    }
    if (pThis == g_lvDropTargetPtr.load() ||
        pThis == g_defViewDropTargetPtr.load() ||
        pThis == g_desktopDropTargetPtr.load()) {
        return true;
    }
    DWORD desktopTid = g_desktopThreadId.load();
    if (desktopTid != 0 && GetCurrentThreadId() == desktopTid) {
        return true;
    }
    if (IsPointOverDesktop(POINT{pt.x, pt.y})) {
        return true;
    }
    return IsDesktopComObject(pThis);
}

// ============================================================================
// COM & OLE Drag-and-Drop Hooks
// ============================================================================

void ExtractNamesFromDataObject(IDataObject* pDataObj,
                                std::vector<std::wstring>& outNames) {
    outNames.clear();
    if (!pDataObj) {
        return;
    }

    auto addName = [&outNames](const std::wstring& raw) {
        std::wstring name = ExtractFileNameFromPath(raw);
        if (!name.empty()) {
            outNames.push_back(std::move(name));
        }
    };

    // 1. Standard CF_HDROP (Explorer folder drag & drop).
    FORMATETC fmtHdrop{};
    fmtHdrop.cfFormat = CF_HDROP;
    fmtHdrop.ptd = nullptr;
    fmtHdrop.dwAspect = DVASPECT_CONTENT;
    fmtHdrop.lindex = -1;
    fmtHdrop.tymed = TYMED_HGLOBAL;

    STGMEDIUM stg{};
    if (SUCCEEDED(pDataObj->GetData(&fmtHdrop, &stg))) {
        if (stg.tymed == TYMED_HGLOBAL && stg.hGlobal) {
            auto hDrop = static_cast<HDROP>(stg.hGlobal);
            UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            for (UINT i = 0; i < count; ++i) {
                UINT len = DragQueryFileW(hDrop, i, nullptr, 0);
                if (len > 0) {
                    std::wstring path(len + 1, L'\0');
                    if (DragQueryFileW(hDrop, i, path.data(), len + 1) > 0) {
                        path.resize(len);
                        addName(path);
                    }
                }
            }
        }
        ReleaseStgMedium(&stg);
    }

    // 2. CFSTR_SHELLIDLIST (HIDA - native Explorer shell item list).
    static UINT cfShellIdList = RegisterClipboardFormatW(CFSTR_SHELLIDLIST);
    if (cfShellIdList != 0) {
        FORMATETC fmtIda{};
        fmtIda.cfFormat = static_cast<CLIPFORMAT>(cfShellIdList);
        fmtIda.ptd = nullptr;
        fmtIda.dwAspect = DVASPECT_CONTENT;
        fmtIda.lindex = -1;
        fmtIda.tymed = TYMED_HGLOBAL;

        STGMEDIUM stgIda{};
        if (SUCCEEDED(pDataObj->GetData(&fmtIda, &stgIda))) {
            if (stgIda.tymed == TYMED_HGLOBAL && stgIda.hGlobal) {
                auto* cida =
                    static_cast<const CIDA*>(GlobalLock(stgIda.hGlobal));
                if (cida && cida->cidl > 0) {
                    const auto* base = reinterpret_cast<const BYTE*>(cida);
                    auto parentPidl = reinterpret_cast<PCIDLIST_ABSOLUTE>(
                        base + cida->aoffset[0]);
                    for (UINT i = 1; i <= cida->cidl; ++i) {
                        auto childPidl = reinterpret_cast<PCUIDLIST_RELATIVE>(
                            base + cida->aoffset[i]);
                        CoTaskMemPtr<ITEMIDLIST_ABSOLUTE> fullPidl;
                        *fullPidl.Put() = ILCombine(parentPidl, childPidl);
                        if (fullPidl) {
                            WCHAR pathBuf[MAX_PATH * 2] = {};
                            if (SHGetPathFromIDListW(fullPidl.Get(), pathBuf) &&
                                pathBuf[0] != L'\0') {
                                addName(pathBuf);
                            } else {
                                CoTaskMemPtr<WCHAR> parsingName;
                                if (SUCCEEDED(SHGetNameFromIDList(
                                        fullPidl.Get(),
                                        SIGDN_PARENTRELATIVEPARSING,
                                        parsingName.Put())) &&
                                    parsingName && parsingName.Get()[0] != L'\0') {
                                    addName(parsingName.Get());
                                }
                            }
                        }
                    }
                }
                if (cida) {
                    GlobalUnlock(stgIda.hGlobal);
                }
            }
            ReleaseStgMedium(&stgIda);
        }
    }

    // 3. CFSTR_FILEDESCRIPTORW (drag & drop from archives / virtual folders).
    static UINT cfFileDescW = RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);
    if (cfFileDescW != 0) {
        FORMATETC fmtDesc{};
        fmtDesc.cfFormat = static_cast<CLIPFORMAT>(cfFileDescW);
        fmtDesc.ptd = nullptr;
        fmtDesc.dwAspect = DVASPECT_CONTENT;
        fmtDesc.lindex = -1;
        fmtDesc.tymed = TYMED_HGLOBAL;

        STGMEDIUM stgDesc{};
        if (SUCCEEDED(pDataObj->GetData(&fmtDesc, &stgDesc))) {
            if (stgDesc.tymed == TYMED_HGLOBAL && stgDesc.hGlobal) {
                auto* fgd = static_cast<FILEGROUPDESCRIPTORW*>(
                    GlobalLock(stgDesc.hGlobal));
                if (fgd) {
                    for (UINT i = 0; i < fgd->cItems; ++i) {
                        if (fgd->fgd[i].cFileName[0] != L'\0') {
                            addName(fgd->fgd[i].cFileName);
                        }
                    }
                    GlobalUnlock(stgDesc.hGlobal);
                }
            }
            ReleaseStgMedium(&stgDesc);
        }
    }
}

void RecordDroppedDataObject(IDataObject* pDataObj,
                             bool updateBurstWindow,
                             ULONGLONG ttlMs,
                             PCWSTR reason) {
    if (updateBurstWindow) {
        std::lock_guard<std::mutex> lock(g_excludeMutex);
        g_lastDropTick = GetTickCount64();
    }

    std::vector<std::wstring> names;
    ExtractNamesFromDataObject(pDataObj, names);
    for (const auto& name : names) {
        MarkNameExcluded(name, ttlMs, reason, true);
    }
}

HRESULT HandleDropHookSlot(size_t slot,
                           IDropTarget* pThis,
                           IDataObject* pDataObj,
                           DWORD grfKeyState,
                           POINTL pt,
                           DWORD* pdwEffect) {
    if (!g_unloading.load() && IsDesktopDropInvocation(pThis, pt)) {
        Wh_Log(L"Desktop IDropTarget::Drop[slot %zu] at (%ld, %ld)", slot, pt.x,
               pt.y);
        RecordDroppedDataObject(pDataObj, /*updateBurstWindow=*/true,
                                kDropExcludeTtlMs, L"IDropTarget::Drop");
    }
    return g_origDropSlots[slot](pThis, pDataObj, grfKeyState, pt, pdwEffect);
}

template <size_t Slot>
HRESULT STDMETHODCALLTYPE Drop_HookSlot(IDropTarget* pThis,
                                        IDataObject* pDataObj,
                                        DWORD grfKeyState,
                                        POINTL pt,
                                        DWORD* pdwEffect) {
    return HandleDropHookSlot(Slot, pThis, pDataObj, grfKeyState, pt,
                              pdwEffect);
}

constexpr DropFn kDropHookThunks[kMaxDropHookSlots] = {
    Drop_HookSlot<0>,
    Drop_HookSlot<1>,
    Drop_HookSlot<2>,
    Drop_HookSlot<3>,
};

bool RegisterUniqueDropHook(IDropTarget* pDropTarget, PCWSTR label) {
    if (!pDropTarget || g_dropHookSlotCount >= kMaxDropHookSlots) {
        return false;
    }
    void** vtbl = *reinterpret_cast<void***>(pDropTarget);
    if (!vtbl) {
        return false;
    }
    void* targetAddr = vtbl[kVtblIdx_IDropTarget_Drop];
    if (!targetAddr) {
        return false;
    }

    std::wstring modName;
    if (IsAddressInComProxyModule(targetAddr, &modName)) {
        Wh_Log(L"Skip hooking COM proxy Drop for [%ls] in %ls (%p)", label,
               modName.c_str(), targetAddr);
        return false;
    }

    for (size_t i = 0; i < g_dropHookSlotCount; ++i) {
        if (g_hookedDropAddrs[i] == targetAddr) {
            Wh_Log(L"Drop for [%ls] shares already-hooked addr %p (slot %zu)",
                   label, targetAddr, i);
            return false;
        }
    }

    size_t slot = g_dropHookSlotCount;
    auto targetDrop = reinterpret_cast<DropFn>(targetAddr);
    if (WindhawkUtils::SetFunctionHook(targetDrop, kDropHookThunks[slot],
                                       &g_origDropSlots[slot])) {
        g_hookedDropAddrs[slot] = targetAddr;
        g_dropHookSlotCount++;
        Wh_Log(L"Hooked IDropTarget::Drop [%ls] in %ls at %p (slot %zu)", label,
               modName.c_str(), targetAddr, slot);
        return true;
    }
    return false;
}

class InFlightDragScope {
   public:
    explicit InFlightDragScope(IDataObject* pDataObj) {
        if (g_unloading.load() || !pDataObj) {
            return;
        }
        ExtractNamesFromDataObject(pDataObj, rawNames_);
        if (rawNames_.empty()) {
            return;
        }
        keys_.reserve(rawNames_.size());
        std::lock_guard<std::mutex> lock(g_excludeMutex);
        for (const auto& name : rawNames_) {
            std::wstring key = ToLower(name);
            if (!key.empty()) {
                g_inFlightDragNames[key]++;
                keys_.push_back(std::move(key));
            }
        }
    }

    ~InFlightDragScope() {
        if (keys_.empty()) {
            return;
        }
        std::lock_guard<std::mutex> lock(g_excludeMutex);
        for (const auto& key : keys_) {
            auto it = g_inFlightDragNames.find(key);
            if (it != g_inFlightDragNames.end()) {
                if (--it->second <= 0) {
                    g_inFlightDragNames.erase(it);
                }
            }
        }
    }

    void CommitDropped(bool droppedOnDesktop, PCWSTR sourceLabel) const {
        if (droppedOnDesktop) {
            std::lock_guard<std::mutex> lock(g_excludeMutex);
            g_lastDropTick = GetTickCount64();
        }
        ULONGLONG ttl = droppedOnDesktop ? kDropExcludeTtlMs : 8000;
        for (const auto& name : rawNames_) {
            MarkNameExcluded(name, ttl, sourceLabel, true);
        }
    }

   private:
    std::vector<std::wstring> rawNames_;
    std::vector<std::wstring> keys_;
};

HRESULT WINAPI DoDragDrop_Hook(IDataObject* pDataObj,
                               IDropSource* pDropSource,
                               DWORD dwOKEffects,
                               DWORD* pdwEffect) {
    InFlightDragScope scope(pDataObj);
    HRESULT hr = g_origDoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);
    if (!g_unloading.load() && hr == DRAGDROP_S_DROP &&
        (!pdwEffect || *pdwEffect != DROPEFFECT_NONE)) {
        POINT pt{};
        GetCursorPos(&pt);
        bool onDesktop = IsPointOverDesktop(pt);
        Wh_Log(L"DoDragDrop completed (DRAGDROP_S_DROP, onDesktop=%d, pt=%ld,%ld)",
               onDesktop ? 1 : 0, pt.x, pt.y);
        scope.CommitDropped(onDesktop, L"DoDragDrop");
    }
    return hr;
}

HRESULT WINAPI SHDoDragDrop_Hook(HWND hwnd,
                                 IDataObject* pdtobj,
                                 IDropSource* pdsrc,
                                 DWORD dwEffect,
                                 DWORD* pdwEffect) {
    InFlightDragScope scope(pdtobj);
    HRESULT hr = g_origSHDoDragDrop(hwnd, pdtobj, pdsrc, dwEffect, pdwEffect);
    if (!g_unloading.load() && hr == DRAGDROP_S_DROP &&
        (!pdwEffect || *pdwEffect != DROPEFFECT_NONE)) {
        POINT pt{};
        GetCursorPos(&pt);
        bool onDesktop = IsPointOverDesktop(pt);
        Wh_Log(
            L"SHDoDragDrop completed (DRAGDROP_S_DROP, onDesktop=%d, pt=%ld,%ld)",
            onDesktop ? 1 : 0, pt.x, pt.y);
        scope.CommitDropped(onDesktop, L"SHDoDragDrop");
    }
    return hr;
}

void InstallProcessDragDropHooks() {
    if (HMODULE hOle32 = GetModuleHandleW(L"ole32.dll")) {
        auto pDoDragDrop = reinterpret_cast<DoDragDropFn>(
            GetProcAddress(hOle32, "DoDragDrop"));
        if (pDoDragDrop &&
            WindhawkUtils::SetFunctionHook(pDoDragDrop, DoDragDrop_Hook,
                                           &g_origDoDragDrop)) {
            Wh_Log(L"Hooked ole32!DoDragDrop at %p", pDoDragDrop);
        }
    }
    if (HMODULE hShell32 = GetModuleHandleW(L"shell32.dll")) {
        auto pSHDoDragDrop = reinterpret_cast<SHDoDragDropFn>(
            GetProcAddress(hShell32, "SHDoDragDrop"));
        if (pSHDoDragDrop &&
            WindhawkUtils::SetFunctionHook(pSHDoDragDrop, SHDoDragDrop_Hook,
                                           &g_origSHDoDragDrop)) {
            Wh_Log(L"Hooked shell32!SHDoDragDrop at %p", pSHDoDragDrop);
        }
    }
}

HRESULT STDMETHODCALLTYPE SelectAndPositionItems_Hook(
    IFolderView* pThis,
    UINT cidl,
    PCUITEMID_CHILD_ARRAY apidl,
    POINT* apt,
    DWORD dwFlags) {
    if (!g_unloading.load() && !g_inSelfPositioning &&
        IsDesktopComObject(pThis)) {
        bool isShellEdit = (dwFlags & SVSI_EDIT) != 0;
        bool isExplicitPos =
            ((dwFlags & SVSI_POSITIONITEM) != 0) && (apt != nullptr);
        if ((isShellEdit || isExplicitPos) && cidl > 0 && apidl != nullptr) {
            ComPtr<IShellFolder> spFolder;
            if (SUCCEEDED(pThis->GetFolder(IID_PPV_ARGS(spFolder.Put()))) &&
                spFolder) {
                for (UINT i = 0; i < cidl; ++i) {
                    std::wstring name = GetPidlItemName(spFolder.Get(), apidl[i]);
                    if (!name.empty()) {
                        MarkNameExcluded(
                            name, kShellExplicitExcludeTtlMs,
                            isShellEdit ? L"SelectAndPositionItems(SVSI_EDIT)"
                                        : L"SelectAndPositionItems(ExplicitPos)");
                    }
                }
            }
        }
    }
    return g_origSelectAndPositionItems(pThis, cidl, apidl, apt, dwFlags);
}

HRESULT STDMETHODCALLTYPE SelectAndPositionItem_Hook(IShellView2* pThis,
                                                     PCUITEMID_CHILD pidlItem,
                                                     UINT uFlags,
                                                     POINT* ppt) {
    if (!g_unloading.load() && !g_inSelfPositioning &&
        IsDesktopComObject(pThis)) {
        bool isShellEdit = (uFlags & SVSI_EDIT) != 0;
        bool isExplicitPos =
            ((uFlags & SVSI_POSITIONITEM) != 0) && (ppt != nullptr);
        if ((isShellEdit || isExplicitPos) && pidlItem != nullptr) {
            ComPtr<IFolderView> spFv;
            ComPtr<IShellFolder> spFolder;
            if (SUCCEEDED(pThis->QueryInterface(IID_PPV_ARGS(spFv.Put()))) &&
                spFv &&
                SUCCEEDED(spFv->GetFolder(IID_PPV_ARGS(spFolder.Put()))) &&
                spFolder) {
                std::wstring name = GetPidlItemName(spFolder.Get(), pidlItem);
                if (!name.empty()) {
                    MarkNameExcluded(
                        name, kShellExplicitExcludeTtlMs,
                        isShellEdit ? L"SelectAndPositionItem(SVSI_EDIT)"
                                    : L"SelectAndPositionItem(ExplicitPos)");
                }
            }
        }
    }
    return g_origSelectAndPositionItem(pThis, pidlItem, uFlags, ppt);
}

bool InstallDesktopComHooks(IShellView* pView, HWND hDefView, HWND hListView) {
    if (g_comHooksInstalled.load()) {
        return true;
    }

    bool anyHookRegistered = false;

    // 1. Direct in-process IDropTarget registered on desktop SysListView32
    //    (e.g. CListViewHost) and SHELLDLL_DefView via RegisterDragDrop.
    if (hListView) {
        auto* pLvDrop = reinterpret_cast<IDropTarget*>(
            GetPropW(hListView, L"OleDropTargetInterface"));
        if (pLvDrop) {
            g_lvDropTargetPtr.store(pLvDrop);
            if (RegisterUniqueDropHook(pLvDrop, L"SysListView32.OleDropTarget")) {
                anyHookRegistered = true;
            }
        }
    }
    if (hDefView) {
        auto* pDefDrop = reinterpret_cast<IDropTarget*>(
            GetPropW(hDefView, L"OleDropTargetInterface"));
        if (pDefDrop) {
            g_defViewDropTargetPtr.store(pDefDrop);
            if (RegisterUniqueDropHook(pDefDrop, L"DefView.OleDropTarget")) {
                anyHookRegistered = true;
            }
        }
    }

    // 2. Direct in-process Desktop ShellFolder drop target (CDesktopFolder /
    //    CFSDropTarget) via SHGetDesktopFolder()->CreateViewObject.
    ComPtr<IShellFolder> spDeskFolder;
    if (SUCCEEDED(SHGetDesktopFolder(spDeskFolder.Put())) && spDeskFolder) {
        ComPtr<IDropTarget> spFolderDrop;
        if (SUCCEEDED(spDeskFolder->CreateViewObject(
                hDefView, IID_PPV_ARGS(spFolderDrop.Put()))) &&
            spFolderDrop) {
            if (RegisterUniqueDropHook(spFolderDrop.Get(),
                                       L"DesktopFolder.CreateViewObject")) {
                anyHookRegistered = true;
            }
        }
    }

    if (pView) {
        // 3. IShellView::QueryInterface(IID_IDropTarget) if not a COM proxy.
        ComPtr<IDropTarget> spDropTarget;
        if (SUCCEEDED(pView->QueryInterface(IID_PPV_ARGS(spDropTarget.Put()))) &&
            spDropTarget) {
            g_desktopDropTargetPtr.store(spDropTarget.Get());
            if (RegisterUniqueDropHook(spDropTarget.Get(),
                                       L"DesktopView.IDropTarget")) {
                anyHookRegistered = true;
            }
        }

        // 4. IFolderView::SelectAndPositionItems (skip COM proxy stubs).
        ComPtr<IFolderView> spFolderView;
        if (SUCCEEDED(pView->QueryInterface(IID_PPV_ARGS(spFolderView.Put()))) &&
            spFolderView) {
            void** vtbl = *reinterpret_cast<void***>(spFolderView.Get());
            void* targetAddr = vtbl[kVtblIdx_IFolderView_SelectAndPositionItems];
            std::wstring modName;
            if (targetAddr && !IsAddressInComProxyModule(targetAddr, &modName)) {
                auto targetPosItems =
                    reinterpret_cast<SelectAndPositionItemsFn>(targetAddr);
                if (WindhawkUtils::SetFunctionHook(
                        targetPosItems, SelectAndPositionItems_Hook,
                        &g_origSelectAndPositionItems)) {
                    Wh_Log(
                        L"Hooked IFolderView::SelectAndPositionItems in %ls at "
                        L"%p",
                        modName.c_str(), targetAddr);
                    anyHookRegistered = true;
                }
            }
        }

        // 5. IShellView2::SelectAndPositionItem (skip COM proxy stubs).
        ComPtr<IShellView2> spShellView2;
        if (SUCCEEDED(pView->QueryInterface(IID_PPV_ARGS(spShellView2.Put()))) &&
            spShellView2) {
            void** vtbl = *reinterpret_cast<void***>(spShellView2.Get());
            void* targetAddr = vtbl[kVtblIdx_IShellView2_SelectAndPositionItem];
            std::wstring modName;
            if (targetAddr && !IsAddressInComProxyModule(targetAddr, &modName)) {
                auto targetPosItem =
                    reinterpret_cast<SelectAndPositionItemFn>(targetAddr);
                if (WindhawkUtils::SetFunctionHook(
                        targetPosItem, SelectAndPositionItem_Hook,
                        &g_origSelectAndPositionItem)) {
                    Wh_Log(
                        L"Hooked IShellView2::SelectAndPositionItem in %ls at "
                        L"%p",
                        modName.c_str(), targetAddr);
                    anyHookRegistered = true;
                }
            }
        }
    }

    g_comHooksInstalled.store(anyHookRegistered);
    return anyHookRegistered;
}

// ============================================================================
// Auto-Arrange Control (Requirement 4)
// ============================================================================

void EnsureAutoArrangeDisabledIfNeeded(IFolderView2* pFolderView2) {
    if (!pFolderView2) {
        return;
    }
    ModSettings settings = GetSettingsCopy();
    int alreadyDone = Wh_GetIntValue(kFirstLoadValueName, 0);
    if (alreadyDone != 0 && !settings.disableAutoArrangeOnEveryLoad) {
        return;
    }

    DWORD flags = 0;
    HRESULT hr = pFolderView2->GetCurrentFolderFlags(&flags);
    if (FAILED(hr)) {
        Wh_Log(L"GetCurrentFolderFlags failed: 0x%08lX", hr);
        return;
    }

    if ((flags & FWF_AUTOARRANGE) != 0) {
        hr = pFolderView2->SetCurrentFolderFlags(FWF_AUTOARRANGE, 0);
        if (SUCCEEDED(hr)) {
            Wh_Log(L"Disabled desktop Auto Arrange (FWF_AUTOARRANGE cleared)");
            Wh_SetIntValue(kFirstLoadValueName, 1);
        } else {
            Wh_Log(L"SetCurrentFolderFlags(FWF_AUTOARRANGE, 0) failed: 0x%08lX",
                   hr);
        }
    } else {
        Wh_Log(L"Desktop Auto Arrange is already disabled");
        if (alreadyDone == 0) {
            Wh_SetIntValue(kFirstLoadValueName, 1);
        }
    }
}

// ============================================================================
// Desktop Grid Geometry & Column Anchor Layout Engine
// ============================================================================

void FreeDesktopItems(std::vector<DesktopItemInfo>& items) {
    for (auto& item : items) {
        if (item.pidl) {
            CoTaskMemFree(item.pidl);
            item.pidl = nullptr;
        }
    }
    items.clear();
}

bool EnumerateDesktopItems(IFolderView2* pFolderView2,
                           std::vector<DesktopItemInfo>& outItems) {
    FreeDesktopItems(outItems);
    if (!pFolderView2) {
        return false;
    }

    IShellFolder* pFolder = nullptr;
    ComPtr<IShellFolder> spFolderLocal;
    if (pFolderView2 == g_cachedFolderView2.Get() && g_cachedDesktopFolder) {
        pFolder = g_cachedDesktopFolder.Get();
    } else {
        if (FAILED(pFolderView2->GetFolder(IID_PPV_ARGS(spFolderLocal.Put()))) ||
            !spFolderLocal) {
            return false;
        }
        pFolder = spFolderLocal.Get();
    }

    int count = 0;
    if (FAILED(pFolderView2->ItemCount(SVGIO_ALLVIEW, &count)) || count < 0) {
        return false;
    }

    outItems.reserve(static_cast<size_t>(count));
    for (int i = 0; i < count; ++i) {
        PITEMID_CHILD pidl = nullptr;
        if (FAILED(pFolderView2->Item(i, &pidl)) || !pidl) {
            continue;
        }
        std::wstring name = GetPidlItemName(pFolder, pidl);
        if (name.empty()) {
            CoTaskMemFree(pidl);
            continue;
        }
        POINT pt{};
        bool hasPos = SUCCEEDED(pFolderView2->GetItemPosition(pidl, &pt));
        DesktopItemInfo info;
        info.key = ToLower(name);
        info.displayName = std::move(name);
        info.pos = pt;
        info.hasPos = hasPos;
        info.pidl = pidl;
        outItems.push_back(std::move(info));
    }
    return true;
}

RECT GetPrimaryDesktopWorkAreaInListViewCoords(HWND hListView) {
    // 1. Try SysListView32 work areas directly (already in ListView client
    //    coordinates and already adjusted for the taskbar).
    if (hListView && IsWindow(hListView)) {
        UINT nWork = 0;
        SendMessageW(hListView, LVM_GETNUMBEROFWORKAREAS, 0,
                     reinterpret_cast<LPARAM>(&nWork));
        if (nWork > 0 && nWork <= 16) {
            std::vector<RECT> areas(nWork);
            SendMessageW(hListView, LVM_GETWORKAREAS, nWork,
                         reinterpret_cast<LPARAM>(areas.data()));
            // Prefer the work area corresponding to the primary monitor.
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            HMONITOR hPrimary = MonitorFromPoint(POINT{0, 0},
                                                 MONITOR_DEFAULTTOPRIMARY);
            RECT primaryClient{};
            bool hasPrimary = false;
            if (hPrimary && GetMonitorInfoW(hPrimary, &mi)) {
                primaryClient = mi.rcWork;
                MapWindowPoints(nullptr, hListView,
                                reinterpret_cast<LPPOINT>(&primaryClient), 2);
                hasPrimary = true;
            }
            if (hasPrimary) {
                for (const RECT& rc : areas) {
                    if (rc.right > rc.left && rc.bottom > rc.top) {
                        RECT inter{};
                        if (IntersectRect(&inter, &rc, &primaryClient)) {
                            return rc;
                        }
                    }
                }
            }
            for (const RECT& rc : areas) {
                if (rc.right > rc.left && rc.bottom > rc.top) {
                    return rc;
                }
            }
        }
    }

    // 2. Fallback to primary monitor work area mapped into ListView client.
    RECT rcWork{};
    HMONITOR hPrimary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (hPrimary && GetMonitorInfoW(hPrimary, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    if (hListView && IsWindow(hListView)) {
        MapWindowPoints(nullptr, hListView, reinterpret_cast<LPPOINT>(&rcWork),
                        2);
    }
    return rcWork;
}

DesktopGridMetrics ComputeDesktopGridMetrics(
    IFolderView2* pFolderView2,
    HWND hListView,
    const std::vector<DesktopItemInfo>& existingItems) {
    DesktopGridMetrics grid{};
    if (!pFolderView2) {
        return grid;
    }

    POINT spacing{};
    if (FAILED(pFolderView2->GetSpacing(&spacing)) || spacing.x <= 0 ||
        spacing.y <= 0) {
        if (hListView && IsWindow(hListView)) {
            DWORD lvSpacing = static_cast<DWORD>(
                SendMessageW(hListView, LVM_GETITEMSPACING, FALSE, 0));
            spacing.x = LOWORD(lvSpacing);
            spacing.y = HIWORD(lvSpacing);
        }
        if (spacing.x <= 0 || spacing.y <= 0) {
            pFolderView2->GetDefaultSpacing(&spacing);
        }
    }

    if (spacing.x <= 0 || spacing.y <= 0) {
        return grid;
    }

    grid.cellW = spacing.x;
    grid.cellH = spacing.y;
    grid.workArea = GetPrimaryDesktopWorkAreaInListViewCoords(hListView);

    long workW = grid.workArea.right - grid.workArea.left;
    long workH = grid.workArea.bottom - grid.workArea.top;
    if (workW < grid.cellW || workH < grid.cellH) {
        return grid;
    }

    int rawCols = std::max(1, static_cast<int>(workW / grid.cellW));
    int rawRows = std::max(1, static_cast<int>(workH / grid.cellH));

    // Calibrate grid origin (originX, originY) from existing desktop icons
    // inside this work area using modal modulo offset.
    std::map<int, int> remXFreq;
    std::map<int, int> remYFreq;
    for (const auto& item : existingItems) {
        if (!item.hasPos || item.pos.x < grid.workArea.left ||
            item.pos.x >= grid.workArea.right ||
            item.pos.y < grid.workArea.top ||
            item.pos.y >= grid.workArea.bottom) {
            continue;
        }
        int rx = static_cast<int>(item.pos.x - grid.workArea.left) % grid.cellW;
        int ry = static_cast<int>(item.pos.y - grid.workArea.top) % grid.cellH;
        if (rx < 0) rx += grid.cellW;
        if (ry < 0) ry += grid.cellH;
        remXFreq[rx]++;
        remYFreq[ry]++;
    }

    auto pickMode = [](const std::map<int, int>& freq, int fallback) -> int {
        int bestKey = fallback;
        int bestCount = 0;
        for (const auto& [k, c] : freq) {
            if (c > bestCount) {
                bestCount = c;
                bestKey = k;
            }
        }
        return bestKey;
    };

    int defaultMarginX =
        static_cast<int>((workW - static_cast<long>(rawCols) * grid.cellW) / 2);
    int defaultMarginY =
        static_cast<int>((workH - static_cast<long>(rawRows) * grid.cellH) / 2);
    defaultMarginX = std::clamp(defaultMarginX, 0, grid.cellW - 1);
    defaultMarginY = std::clamp(defaultMarginY, 0, grid.cellH - 1);

    int offsetX = pickMode(remXFreq, defaultMarginX);
    int offsetY = pickMode(remYFreq, defaultMarginY);

    grid.originX = static_cast<int>(grid.workArea.left) + offsetX;
    grid.originY = static_cast<int>(grid.workArea.top) + offsetY;

    // Recompute exact fitting column and row counts from the calibrated origin.
    long availW = grid.workArea.right - grid.originX;
    long availH = grid.workArea.bottom - grid.originY;
    grid.totalCols = std::max(1, static_cast<int>((availW + 4) / grid.cellW));
    grid.gridRows = std::max(1, static_cast<int>((availH + 4) / grid.cellH));

    // Ensure the rightmost column's center stays inside the work area.
    while (grid.totalCols > 1 &&
           grid.originX + (grid.totalCols - 1) * grid.cellW + grid.cellW / 2 >
               grid.workArea.right) {
        grid.totalCols--;
    }
    while (grid.gridRows > 1 &&
           grid.originY + (grid.gridRows - 1) * grid.cellH + grid.cellH / 2 >
               grid.workArea.bottom) {
        grid.gridRows--;
    }

    grid.valid = true;
    return grid;
}

std::vector<int> BuildColumnSearchOrder(int anchorColumn,
                                        int overflowDirection,
                                        int totalCols) {
    int normalizedCol = (anchorColumn == 0) ? 1 : anchorColumn;
    int targetIdx = 0;
    if (normalizedCol > 0) {
        targetIdx = std::clamp(normalizedCol - 1, 0, totalCols - 1);
    } else {
        targetIdx = std::clamp(totalCols + normalizedCol, 0, totalCols - 1);
    }

    // Requirement 3:
    // - Leftmost column (1 or index 0): default rightward (+1)
    // - Rightmost column (-1 or index totalCols-1): default leftward (-1)
    // - Otherwise follow user setting (1 = right, -1 = left)
    int primaryDir = (overflowDirection < 0) ? -1 : 1;
    if (normalizedCol == 1 || targetIdx == 0) {
        primaryDir = 1;
    } else if (normalizedCol == -1 || targetIdx == totalCols - 1) {
        primaryDir = -1;
    }

    std::vector<int> order;
    order.reserve(static_cast<size_t>(totalCols));
    order.push_back(targetIdx);

    // Search primary direction to the desktop edge.
    for (int c = targetIdx + primaryDir; c >= 0 && c < totalCols;
         c += primaryDir) {
        order.push_back(c);
    }

    // Fallback to the opposite direction if primary side is completely full.
    for (int c = targetIdx - primaryDir; c >= 0 && c < totalCols;
         c -= primaryDir) {
        order.push_back(c);
    }

    return order;
}

std::optional<POINT> FindNextAvailableSlot(
    const DesktopGridMetrics& grid,
    const ModSettings& settings,
    const std::vector<POINT>& occupiedPositions) {
    if (!grid.valid) {
        return std::nullopt;
    }

    int effectiveRows = grid.gridRows;
    if (settings.columnCapacity > 0) {
        effectiveRows = std::min(settings.columnCapacity, grid.gridRows);
    }
    if (effectiveRows < 1) {
        effectiveRows = 1;
    }

    std::vector<int> colOrder = BuildColumnSearchOrder(
        settings.anchorColumn, settings.overflowDirection, grid.totalCols);

    int collideThreshX = std::max(1, (grid.cellW * 6) / 10);
    int collideThreshY = std::max(1, (grid.cellH * 6) / 10);

    for (int col : colOrder) {
        int slotX = grid.originX + col * grid.cellW;
        for (int row = 0; row < effectiveRows; ++row) {
            int slotY = grid.originY + row * grid.cellH;
            bool occupied = false;
            for (const POINT& pt : occupiedPositions) {
                if (std::abs(pt.x - slotX) < collideThreshX &&
                    std::abs(pt.y - slotY) < collideThreshY) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                return POINT{slotX, slotY};
            }
        }
    }

    return std::nullopt;
}

// ============================================================================
// Desktop Sync & Anchoring Logic (Runs strictly on Desktop UI Thread)
// ============================================================================

void ScheduleDesktopSync(UINT delayMs) {
    HWND hMsg = g_hMsgWnd.load();
    if (!hMsg || g_unloading.load()) {
        return;
    }
    SetTimer(hMsg, kTimerSyncDesktop, delayMs, nullptr);
}

void PostImmediateDesktopSync() {
    g_pendingAnchorCheck.store(true, std::memory_order_release);
    HWND hMsg = g_hMsgWnd.load();
    if (!hMsg || g_unloading.load()) {
        return;
    }
    PostMessageW(hMsg, WM_APP_SYNC_NOW, 0, 0);
}

void SetPendingFlushTimer(bool enable) {
    HWND hMsg = g_hMsgWnd.load();
    if (!hMsg || g_unloading.load()) {
        g_pendingFlushTimerActive = false;
        return;
    }
    if (enable) {
        if (!g_pendingFlushTimerActive) {
            SetTimer(hMsg, kTimerPendingFlush, kPendingFlushIntervalMs, nullptr);
            g_pendingFlushTimerActive = true;
        }
    } else {
        if (g_pendingFlushTimerActive) {
            KillTimer(hMsg, kTimerPendingFlush);
            g_pendingFlushTimerActive = false;
        }
    }
}

void ClearAnchoredGuards() {
    for (auto& g : g_recentAnchoredGuards) {
        if (g.pidl) {
            CoTaskMemFree(g.pidl);
            g.pidl = nullptr;
        }
    }
    g_recentAnchoredGuards.clear();
}

void AddAnchoredGuard(const std::wstring& key,
                      PCUITEMID_CHILD pidl,
                      POINT targetPos,
                      const DesktopGridMetrics& grid,
                      ULONGLONG now) {
    if (!pidl) {
        return;
    }
    PITEMID_CHILD cloned = ILCloneChild(pidl);
    if (!cloned) {
        return;
    }

    int tolX = grid.valid ? std::max(16, grid.cellW / 3) : 24;
    int tolY = grid.valid ? std::max(16, grid.cellH / 3) : 24;

    for (auto& existing : g_recentAnchoredGuards) {
        if (existing.key == key) {
            if (existing.pidl) {
                CoTaskMemFree(existing.pidl);
            }
            existing.pidl = cloned;
            existing.targetPos = targetPos;
            existing.tolX = tolX;
            existing.tolY = tolY;
            existing.expireTick = now + kAnchoredGuardTtlMs;
            existing.reassertCount = 0;
            return;
        }
    }

    AnchoredGuard guard;
    guard.key = key;
    guard.pidl = cloned;
    guard.targetPos = targetPos;
    guard.tolX = tolX;
    guard.tolY = tolY;
    guard.expireTick = now + kAnchoredGuardTtlMs;
    guard.reassertCount = 0;
    g_recentAnchoredGuards.push_back(std::move(guard));
}

void InvalidateIconSlotRect(HWND hListView,
                            POINT pos,
                            int cellW,
                            int cellH) {
    if (!hListView || !IsWindow(hListView)) {
        return;
    }
    int w = std::max(cellW, 80);
    int h = std::max(cellH, 80);
    RECT rc{pos.x - 8, pos.y - 8, pos.x + w + 8, pos.y + h + 8};
    InvalidateRect(hListView, &rc, TRUE);
}

void CheckAnchoredGuardsBeforePaint(HWND hListView, IFolderView2* pFolderView2) {
    if (g_recentAnchoredGuards.empty() || !pFolderView2 ||
        g_inSelfPositioning) {
        return;
    }

    ULONGLONG now = GetTickCount64();

    // If user enabled Auto Arrange, release all guards immediately.
    DWORD folderFlags = 0;
    if (SUCCEEDED(pFolderView2->GetCurrentFolderFlags(&folderFlags)) &&
        ((folderFlags & FWF_AUTOARRANGE) != 0)) {
        ClearAnchoredGuards();
        return;
    }

    std::vector<PCUITEMID_CHILD> fixPidls;
    std::vector<POINT> fixPts;
    std::vector<size_t> fixIndices;

    for (size_t i = 0; i < g_recentAnchoredGuards.size();) {
        auto& g = g_recentAnchoredGuards[i];
        if (g.expireTick <= now || g.reassertCount >= kAnchoredGuardMaxReasserts ||
            !g.pidl) {
            if (g.pidl) {
                CoTaskMemFree(g.pidl);
            }
            g_recentAnchoredGuards.erase(g_recentAnchoredGuards.begin() +
                                         static_cast<ptrdiff_t>(i));
            continue;
        }

        POINT curPos{};
        if (FAILED(pFolderView2->GetItemPosition(g.pidl, &curPos))) {
            CoTaskMemFree(g.pidl);
            g_recentAnchoredGuards.erase(g_recentAnchoredGuards.begin() +
                                         static_cast<ptrdiff_t>(i));
            continue;
        }

        if (std::abs(curPos.x - g.targetPos.x) > g.tolX ||
            std::abs(curPos.y - g.targetPos.y) > g.tolY) {
            fixPidls.push_back(g.pidl);
            fixPts.push_back(g.targetPos);
            fixIndices.push_back(i);
        }
        ++i;
    }

    if (fixPidls.empty()) {
        return;
    }

    g_inSelfPositioning = true;
    HRESULT hr = pFolderView2->SelectAndPositionItems(
        static_cast<UINT>(fixPidls.size()), fixPidls.data(), fixPts.data(),
        SVSI_POSITIONITEM);
    g_inSelfPositioning = false;

    if (SUCCEEDED(hr)) {
        for (size_t idx : fixIndices) {
            auto& g = g_recentAnchoredGuards[idx];
            g.reassertCount++;
            InvalidateIconSlotRect(hListView, g.targetPos, g.tolX * 3,
                                   g.tolY * 3);
            Wh_Log(L"Re-asserted anchored [%ls] -> (%ld,%ld) before paint (#%d)",
                   g.key.c_str(), g.targetPos.x, g.targetPos.y,
                   g.reassertCount);
        }
    }
}

void PruneDesktopTrackingMaps(ULONGLONG now) {
    for (auto it = g_pendingViewItems.begin();
         it != g_pendingViewItems.end();) {
        if (it->second <= now) {
            it = g_pendingViewItems.erase(it);
        } else {
            ++it;
        }
    }
    for (auto it = g_recentlyRemovedKnown.begin();
         it != g_recentlyRemovedKnown.end();) {
        if (it->second <= now) {
            it = g_recentlyRemovedKnown.erase(it);
        } else {
            ++it;
        }
    }
}

void ConsumeFsEventsOnDesktopThread(const ModSettings& settings, ULONGLONG now) {
    std::vector<FsEvent> drained;
    {
        std::lock_guard<std::mutex> lock(g_fsMutex);
        if (g_fsEvents.empty()) {
            return;
        }
        drained.swap(g_fsEvents);
    }

    for (const auto& ev : drained) {
        switch (ev.type) {
            case FsEventType::Removed: {
                if (!ev.name1.empty()) {
                    g_pendingViewItems.erase(ev.name1);
                    if (g_knownItems.find(ev.name1) != g_knownItems.end()) {
                        g_recentlyRemovedKnown[ev.name1] =
                            now + kRecentRemovedKnownTtlMs;
                    }
                }
                break;
            }
            case FsEventType::Renamed: {
                if (ev.name1.empty() || ev.name2.empty()) {
                    break;
                }
                bool oldWasKnown =
                    (g_knownItems.find(ev.name1) != g_knownItems.end());
                if (oldWasKnown) {
                    g_knownItems.erase(ev.name1);
                    if (settings.ignoreTempFiles &&
                        IsTemporaryFileName(ev.name2)) {
                        // Likely atomic ReplaceFile moving original to temp.
                        g_recentlyRemovedKnown[ev.name1] =
                            now + kRecentRemovedKnownTtlMs;
                    } else {
                        g_knownItems.insert(ev.name2);
                        g_pendingViewItems.erase(ev.name2);
                        Wh_Log(L"FS rename tracked (existing): [%ls] -> [%ls]",
                               ev.name1.c_str(), ev.name2.c_str());
                    }
                } else {
                    // Previously untracked (e.g. temp download file) renamed
                    // to final filename.
                    auto remIt = g_recentlyRemovedKnown.find(ev.name2);
                    if (remIt != g_recentlyRemovedKnown.end() &&
                        remIt->second > now) {
                        // Atomic overwrite of an existing desktop file.
                        g_knownItems.insert(ev.name2);
                        g_recentlyRemovedKnown.erase(remIt);
                        g_pendingViewItems.erase(ev.name2);
                        Wh_Log(L"Atomic overwrite preserved: [%ls]",
                               ev.name2.c_str());
                    } else if (!settings.ignoreTempFiles ||
                               !IsTemporaryFileName(ev.name2)) {
                        g_pendingViewItems[ev.name2] =
                            now + kPendingViewItemTtlMs;
                    }
                }
                break;
            }
            case FsEventType::Added: {
                if (ev.name1.empty()) {
                    break;
                }
                if (settings.ignoreTempFiles && IsTemporaryFileName(ev.name1)) {
                    break;
                }
                auto remIt = g_recentlyRemovedKnown.find(ev.name1);
                if (remIt != g_recentlyRemovedKnown.end() &&
                    remIt->second > now) {
                    // Re-created within atomic overwrite window.
                    g_knownItems.insert(ev.name1);
                    g_recentlyRemovedKnown.erase(remIt);
                    break;
                }
                if (g_knownItems.find(ev.name1) == g_knownItems.end()) {
                    g_pendingViewItems[ev.name1] = now + kPendingViewItemTtlMs;
                }
                break;
            }
        }
    }
}

bool IsListViewItemSelectedOrEditing(HWND hListView,
                                     IFolderView2* pFolderView2,
                                     const std::wstring& keyLower) {
    // Check if the desktop ListView is currently in inline rename edit mode.
    if (hListView && IsWindow(hListView)) {
        if (ListView_GetEditControl(hListView) != nullptr) {
            int focusedIdx = -1;
            if (pFolderView2 &&
                SUCCEEDED(pFolderView2->GetFocusedItem(&focusedIdx)) &&
                focusedIdx >= 0) {
                PITEMID_CHILD pidl = nullptr;
                ComPtr<IShellFolder> spFolder;
                if (SUCCEEDED(pFolderView2->GetFolder(
                        IID_PPV_ARGS(spFolder.Put()))) &&
                    spFolder &&
                    SUCCEEDED(pFolderView2->Item(focusedIdx, &pidl)) && pidl) {
                    std::wstring focusedName =
                        ToLower(GetPidlItemName(spFolder.Get(), pidl));
                    CoTaskMemFree(pidl);
                    if (focusedName == keyLower) {
                        return true;
                    }
                }
            }
        }
    }

    // Check whether the item is currently selected in the view.
    if (pFolderView2) {
        ComPtr<IShellFolder> spFolder;
        ComPtr<IEnumIDList> spSelEnum;
        if (SUCCEEDED(pFolderView2->GetFolder(IID_PPV_ARGS(spFolder.Put()))) &&
            spFolder &&
            SUCCEEDED(pFolderView2->Items(SVGIO_SELECTION,
                                          IID_PPV_ARGS(spSelEnum.Put()))) &&
            spSelEnum) {
            PITEMID_CHILD pidl = nullptr;
            while (spSelEnum->Next(1, &pidl, nullptr) == S_OK && pidl) {
                std::wstring selName =
                    ToLower(GetPidlItemName(spFolder.Get(), pidl));
                CoTaskMemFree(pidl);
                pidl = nullptr;
                if (selName == keyLower) {
                    return true;
                }
            }
        }
    }

    return false;
}

void SyncDesktopAndAnchorNewItems(bool fromPrePaint = false) {
    if (g_unloading.load()) {
        return;
    }

    HWND hListView = g_hListView.load();
    IFolderView2* pFolderView2 = GetDesktopFolderView2Cached();
    if (!pFolderView2) {
        Wh_Log(L"GetDesktopFolderView2Cached failed");
        return;
    }

    if (!g_baselineLoaded.load()) {
        EnsureAutoArrangeDisabledIfNeeded(pFolderView2);
    }

    ModSettings settings = GetSettingsCopy();
    ULONGLONG now = GetTickCount64();

    // Drain kernel-level directory change events before inspecting the view
    // so existing-file renames and atomic overwrites are recognized even
    // before SHChangeNotify arrives.
    PruneDesktopTrackingMaps(now);
    ConsumeFsEventsOnDesktopThread(settings, now);

    std::vector<DesktopItemInfo> allItems;
    if (!EnumerateDesktopItems(pFolderView2, allItems)) {
        ResetDesktopComCache();
        pFolderView2 = GetDesktopFolderView2Cached();
        if (!pFolderView2 || !EnumerateDesktopItems(pFolderView2, allItems)) {
            return;
        }
    }

    int curLvCount = -1;
    if (hListView && IsWindow(hListView)) {
        curLvCount = static_cast<int>(
            SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0));
    }

    // Initial baseline snapshot: record existing items without moving anything.
    if (!g_baselineLoaded.load()) {
        g_knownItems.clear();
        for (const auto& item : allItems) {
            g_knownItems.insert(item.key);
        }
        g_lastObservedLvCount =
            (curLvCount >= 0) ? curLvCount : static_cast<int>(allItems.size());
        g_pendingViewItems.clear();
        g_pendingAnchorCheck.store(false, std::memory_order_release);
        g_baselineLoaded.store(true);
        Wh_Log(L"Initial desktop baseline captured: %zu items",
               g_knownItems.size());
        FreeDesktopItems(allItems);
        return;
    }

    // Build current key set and detect newly appeared non-temporary items.
    std::unordered_set<std::wstring> currentKeys;
    currentKeys.reserve(allItems.size());

    std::vector<size_t> newItemIndices;
    for (size_t i = 0; i < allItems.size(); ++i) {
        const auto& item = allItems[i];
        currentKeys.insert(item.key);
        g_pendingViewItems.erase(item.key);

        if (g_knownItems.find(item.key) == g_knownItems.end()) {
            if (settings.ignoreTempFiles && IsTemporaryFileName(item.key)) {
                Wh_Log(L"Skipping temp file [%ls] until finalized",
                       item.displayName.c_str());
                continue;
            }
            newItemIndices.push_back(i);
        }
    }

    // Remove deleted items from g_knownItems so recreating a deleted filename
    // will be treated as a new file, while keeping a short atomic-overwrite
    // memory window.
    for (auto it = g_knownItems.begin(); it != g_knownItems.end();) {
        if (currentKeys.find(*it) == currentKeys.end()) {
            g_recentlyRemovedKnown[*it] = now + kRecentRemovedKnownTtlMs;
            it = g_knownItems.erase(it);
        } else {
            ++it;
        }
    }

    if (curLvCount >= 0) {
        g_lastObservedLvCount = curLvCount;
    }

    if (newItemIndices.empty()) {
        // Still verify any recently anchored items in case the shell's async
        // layout engine tried to snap them back before painting.
        CheckAnchoredGuardsBeforePaint(hListView, pFolderView2);

        if (g_pendingViewItems.empty()) {
            g_pendingAnchorCheck.store(false, std::memory_order_release);
            SetPendingFlushTimer(false);
        } else {
            // Disk file was created/renamed, but CDefView has not inserted it
            // into IFolderView2 yet: keep pre-paint hook armed and poll
            // briefly as a fallback in case the desktop is occluded.
            g_pendingAnchorCheck.store(true, std::memory_order_release);
            SetPendingFlushTimer(true);
        }
        FreeDesktopItems(allItems);
        return;
    }

    // Check whether Auto Arrange was manually re-enabled by the user.
    DWORD folderFlags = 0;
    bool autoArrangeOn =
        SUCCEEDED(pFolderView2->GetCurrentFolderFlags(&folderFlags)) &&
        ((folderFlags & FWF_AUTOARRANGE) != 0);

    // Collect existing (already known) items for grid calibration and occupancy.
    std::unordered_set<size_t> newIndexSet(newItemIndices.begin(),
                                           newItemIndices.end());
    std::vector<DesktopItemInfo> calibrationItems;
    std::vector<POINT> occupiedPositions;
    occupiedPositions.reserve(allItems.size());

    for (size_t i = 0; i < allItems.size(); ++i) {
        if (newIndexSet.find(i) == newIndexSet.end() && allItems[i].hasPos) {
            calibrationItems.push_back(allItems[i]);  // Non-owning copy of pos
            occupiedPositions.push_back(allItems[i].pos);
        }
    }

    DesktopGridMetrics grid = ComputeDesktopGridMetrics(
        pFolderView2, hListView,
        calibrationItems.empty() ? allItems : calibrationItems);

    if (grid.valid) {
        Wh_Log(
            L"Grid metrics (%ls): work=(%ld,%ld)-(%ld,%ld) cell=(%d,%d) "
            L"origin=(%d,%d) cols=%d rows=%d",
            fromPrePaint ? L"pre-paint" : L"sync", grid.workArea.left,
            grid.workArea.top, grid.workArea.right, grid.workArea.bottom,
            grid.cellW, grid.cellH, grid.originX, grid.originY, grid.totalCols,
            grid.gridRows);
    }

    ULONGLONG contextGraceMs =
        static_cast<ULONGLONG>(std::max(settings.anchorDelayMs, 250));
    bool inContextMenuGrace =
        IsDesktopContextMenuGraceActive(now, contextGraceMs);

    bool deferredAny = false;
    bool retryAny = false;

    struct PendingAnchorOp {
        size_t itemIndex = 0;
        POINT targetPt{};
    };
    std::vector<PendingAnchorOp> batchOps;
    std::vector<PCUITEMID_CHILD> batchPidls;
    std::vector<POINT> batchPts;
    batchOps.reserve(newItemIndices.size());
    batchPidls.reserve(newItemIndices.size());
    batchPts.reserve(newItemIndices.size());

    for (size_t idx : newItemIndices) {
        const auto& item = allItems[idx];

        if (autoArrangeOn) {
            g_knownItems.insert(item.key);
            Wh_Log(L"Skip [%ls]: Auto Arrange is currently enabled",
                   item.displayName.c_str());
            if (item.hasPos) {
                occupiedPositions.push_back(item.pos);
            }
            continue;
        }

        // Check if currently in inline label editing for this item.
        bool isEditingThis = false;
        if (hListView && IsWindow(hListView) &&
            ListView_GetEditControl(hListView) != nullptr) {
            int focusedIdx = -1;
            if (SUCCEEDED(pFolderView2->GetFocusedItem(&focusedIdx)) &&
                focusedIdx >= 0 &&
                static_cast<size_t>(focusedIdx) < allItems.size()) {
                if (allItems[static_cast<size_t>(focusedIdx)].key == item.key) {
                    isEditingThis = true;
                }
            }
        }

        if (isEditingThis) {
            g_knownItems.insert(item.key);
            Wh_Log(L"Skip [%ls]: item is in inline label edit mode",
                   item.displayName.c_str());
            if (item.hasPos) {
                occupiedPositions.push_back(item.pos);
            }
            continue;
        }

        bool isSelected = IsListViewItemSelectedOrEditing(
            hListView, pFolderView2, item.key);

        std::wstring excludeReason;
        if (IsItemExcluded(item.key, item.pos, isSelected, grid,
                           &excludeReason)) {
            g_knownItems.insert(item.key);
            Wh_Log(L"Skip [%ls]: excluded by [%ls]", item.displayName.c_str(),
                   excludeReason.c_str());
            if (item.hasPos) {
                occupiedPositions.push_back(item.pos);
            }
            continue;
        }

        // Right after closing the desktop context menu, defer briefly so
        // right-click "New" has time to invoke SelectAndPositionItem(SVSI_EDIT)
        // and enter inline rename without racing. External app creations never
        // hit this branch and anchor synchronously before the first paint.
        if (inContextMenuGrace) {
            deferredAny = true;
            Wh_Log(L"Defer [%ls]: desktop context menu grace active",
                   item.displayName.c_str());
            if (item.hasPos) {
                occupiedPositions.push_back(item.pos);
            }
            continue;
        }

        auto slot = FindNextAvailableSlot(grid, settings, occupiedPositions);
        if (!slot.has_value()) {
            g_knownItems.insert(item.key);
            Wh_Log(L"Skip [%ls]: no available grid slot on desktop",
                   item.displayName.c_str());
            if (item.hasPos) {
                occupiedPositions.push_back(item.pos);
            }
            continue;
        }

        batchOps.push_back(PendingAnchorOp{idx, *slot});
        batchPidls.push_back(item.pidl);
        batchPts.push_back(*slot);
        occupiedPositions.push_back(*slot);
    }

    if (!batchOps.empty()) {
        g_inSelfPositioning = true;
        HRESULT batchHr = pFolderView2->SelectAndPositionItems(
            static_cast<UINT>(batchPidls.size()), batchPidls.data(),
            batchPts.data(), SVSI_POSITIONITEM);
        g_inSelfPositioning = false;

        if (SUCCEEDED(batchHr)) {
            for (const auto& op : batchOps) {
                const auto& item = allItems[op.itemIndex];
                g_knownItems.insert(item.key);
                AddAnchoredGuard(item.key, item.pidl, op.targetPt, grid, now);
                if (fromPrePaint && grid.valid) {
                    if (item.hasPos) {
                        InvalidateIconSlotRect(hListView, item.pos, grid.cellW,
                                               grid.cellH);
                    }
                    InvalidateIconSlotRect(hListView, op.targetPt, grid.cellW,
                                           grid.cellH);
                }
                Wh_Log(L"Anchored [%ls] (%ls) from (%ld,%ld) -> (%ld,%ld)",
                       item.displayName.c_str(),
                       fromPrePaint ? L"pre-paint" : L"sync", item.pos.x,
                       item.pos.y, op.targetPt.x, op.targetPt.y);
            }
        } else {
            // Fallback to per-item positioning if batch call fails.
            for (const auto& op : batchOps) {
                const auto& item = allItems[op.itemIndex];
                PCUITEMID_CHILD onePidl[1] = {item.pidl};
                POINT onePt = op.targetPt;
                g_inSelfPositioning = true;
                HRESULT posHr = pFolderView2->SelectAndPositionItems(
                    1, onePidl, &onePt, SVSI_POSITIONITEM);
                g_inSelfPositioning = false;

                if (SUCCEEDED(posHr)) {
                    g_knownItems.insert(item.key);
                    AddAnchoredGuard(item.key, item.pidl, onePt, grid, now);
                    if (fromPrePaint && grid.valid) {
                        if (item.hasPos) {
                            InvalidateIconSlotRect(hListView, item.pos,
                                                   grid.cellW, grid.cellH);
                        }
                        InvalidateIconSlotRect(hListView, onePt, grid.cellW,
                                               grid.cellH);
                    }
                    Wh_Log(L"Anchored [%ls] (%ls) from (%ld,%ld) -> (%ld,%ld)",
                           item.displayName.c_str(),
                           fromPrePaint ? L"pre-paint" : L"sync", item.pos.x,
                           item.pos.y, onePt.x, onePt.y);
                } else {
                    Wh_Log(L"SelectAndPositionItems failed for [%ls]: 0x%08lX",
                           item.displayName.c_str(), posHr);
                    retryAny = true;
                }
            }
        }
    }

    // Re-check guards for previously anchored items if any are still active.
    CheckAnchoredGuardsBeforePaint(hListView, pFolderView2);

    if (deferredAny) {
        ScheduleDesktopSync(static_cast<UINT>(settings.anchorDelayMs));
    }

    if (retryAny || !g_pendingViewItems.empty()) {
        g_pendingAnchorCheck.store(true, std::memory_order_release);
        SetPendingFlushTimer(true);
    } else {
        g_pendingAnchorCheck.store(false, std::memory_order_release);
        SetPendingFlushTimer(false);
    }

    // Clear non-owning pidl copies in calibrationItems before freeing allItems.
    for (auto& c : calibrationItems) {
        c.pidl = nullptr;
    }
    FreeDesktopItems(allItems);
}

// ============================================================================
// Desktop Window Subclasses (SHELLDLL_DefView & SysListView32)
// ============================================================================

void RecordLabelEditByListViewIndex(HWND hListView, int iItem) {
    if (!hListView || iItem < 0) {
        return;
    }
    WCHAR text[MAX_PATH] = {};
    LVITEMW lvi{};
    lvi.mask = LVIF_TEXT;
    lvi.iItem = iItem;
    lvi.pszText = text;
    lvi.cchTextMax = ARRAYSIZE(text);
    if (SendMessageW(hListView, LVM_GETITEMTEXTW, static_cast<WPARAM>(iItem),
                     reinterpret_cast<LPARAM>(&lvi)) > 0 &&
        text[0] != L'\0') {
        MarkNameExcluded(text, kLabelEditExcludeTtlMs, L"LabelEdit");
    }

    // Also resolve exact parsing name (including extension) via IFolderView2.
    ComPtr<IFolderView2> spFv2;
    ComPtr<IShellFolder> spFolder;
    if (SUCCEEDED(AcquireDesktopFolderView2(spFv2)) && spFv2 &&
        SUCCEEDED(spFv2->GetFolder(IID_PPV_ARGS(spFolder.Put()))) && spFolder) {
        PITEMID_CHILD pidl = nullptr;
        if (SUCCEEDED(spFv2->Item(iItem, &pidl)) && pidl) {
            std::wstring fullName = GetPidlItemName(spFolder.Get(), pidl);
            CoTaskMemFree(pidl);
            if (!fullName.empty()) {
                MarkNameExcluded(fullName, kLabelEditExcludeTtlMs,
                                 L"LabelEdit(ParsingName)");
            }
        }
    }
}

void RecordContextMenuPoint(HWND hListView, POINT screenPt) {
    if (!hListView || !IsWindow(hListView)) {
        return;
    }
    POINT clientPt = screenPt;
    ScreenToClient(hListView, &clientPt);
    std::lock_guard<std::mutex> lock(g_excludeMutex);
    g_lastContextMenuClientPt = clientPt;
    g_lastContextMenuTick = GetTickCount64();
    g_hasContextMenuPt = true;
    g_contextMenuPendingClose = true;
}

void RecordEndLabelEditName(PCWSTR newText) {
    if (!newText || newText[0] == L'\0') {
        return;
    }
    MarkNameExcluded(newText, kLabelEditExcludeTtlMs, L"EndLabelEdit");
    // Also record stem for cases where Windows hides known file extensions.
    std::wstring lower = ToLower(ExtractFileNameFromPath(newText));
    if (!lower.empty()) {
        std::lock_guard<std::mutex> lock(g_excludeMutex);
        g_recentDropStems.push_back(
            DropStemEntry{ExtractStemLower(lower), GetTickCount64() + 5000});
    }
}

void PrePaintSyncDesktopIfNeeded(HWND hListView) {
    if (g_unloading.load() || g_inPrePaintSync || g_inSelfPositioning ||
        !g_baselineLoaded.load()) {
        return;
    }

    bool dirty = g_pendingAnchorCheck.load(std::memory_order_acquire);
    int curCount = static_cast<int>(
        DefSubclassProc(hListView, LVM_GETITEMCOUNT, 0, 0));
    bool countChanged =
        (g_lastObservedLvCount >= 0 && curCount != g_lastObservedLvCount);
    bool hasGuards = !g_recentAnchoredGuards.empty();

    if (!dirty && !countChanged && !hasGuards) {
        return;
    }

    struct ScopedPrePaintGuard {
        ScopedPrePaintGuard() { g_inPrePaintSync = true; }
        ~ScopedPrePaintGuard() { g_inPrePaintSync = false; }
    } guard;

    if (dirty || countChanged) {
        SyncDesktopAndAnchorNewItems(/*fromPrePaint=*/true);
    } else if (hasGuards) {
        IFolderView2* pFv2 = GetDesktopFolderView2Cached();
        if (pFv2) {
            CheckAnchoredGuardsBeforePaint(hListView, pFv2);
        }
    }
}

LRESULT CALLBACK DesktopListViewSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             DWORD_PTR dwRefData) {
    if (!g_unloading.load()) {
        switch (uMsg) {
            case WM_PAINT:
            case WM_PRINTCLIENT: {
                PrePaintSyncDesktopIfNeeded(hWnd);
                break;
            }
            case LVM_SETITEMCOUNT:
            case LVM_INSERTITEMW:
            case LVM_INSERTITEMA:
            case LVM_DELETEITEM: {
                LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                PostImmediateDesktopSync();
                return res;
            }
            case LVM_EDITLABELW:
            case LVM_EDITLABELA: {
                ClearAnchoredGuards();
                int iItem = static_cast<int>(wParam);
                if (iItem >= 0) {
                    RecordLabelEditByListViewIndex(hWnd, iItem);
                }
                break;
            }
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_KEYDOWN: {
                ClearAnchoredGuards();
                break;
            }
            case WM_RBUTTONDOWN: {
                ClearAnchoredGuards();
                break;
            }
            case WM_RBUTTONUP: {
                ClearAnchoredGuards();
                POINT pt{};
                GetCursorPos(&pt);
                RecordContextMenuPoint(hWnd, pt);
                break;
            }
            case WM_CONTEXTMENU: {
                ClearAnchoredGuards();
                POINT pt{};
                if (lParam != static_cast<LPARAM>(-1)) {
                    pt.x = static_cast<short>(LOWORD(lParam));
                    pt.y = static_cast<short>(HIWORD(lParam));
                } else {
                    GetCursorPos(&pt);
                }
                RecordContextMenuPoint(hWnd, pt);
                break;
            }
            case WM_SETFOCUS: {
                ULONGLONG now = GetTickCount64();
                std::lock_guard<std::mutex> lock(g_excludeMutex);
                if (g_contextMenuPendingClose) {
                    if (g_lastContextMenuTick != 0 &&
                        now >= g_lastContextMenuTick &&
                        (now - g_lastContextMenuTick) <= kContextMenuOpenMaxMs) {
                        g_lastContextMenuTick = now;
                    }
                    g_contextMenuPendingClose = false;
                }
                break;
            }
            case WM_NCDESTROY: {
                ClearAnchoredGuards();
                ResetDesktopComCache();
                g_listViewSubclassed = false;
                g_hListView.store(nullptr);
                break;
            }
            default:
                break;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DesktopDefViewSubclassProc(HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            DWORD_PTR dwRefData) {
    if (!g_unloading.load()) {
        switch (uMsg) {
            case WM_ENTERMENULOOP: {
                ClearAnchoredGuards();
                std::lock_guard<std::mutex> lock(g_excludeMutex);
                g_inDesktopContextMenu = true;
                g_contextMenuPendingClose = false;
                g_lastContextMenuTick = GetTickCount64();
                break;
            }
            case WM_EXITMENULOOP: {
                std::lock_guard<std::mutex> lock(g_excludeMutex);
                g_inDesktopContextMenu = false;
                g_contextMenuPendingClose = false;
                g_lastContextMenuTick = GetTickCount64();
                break;
            }
            case WM_CONTEXTMENU: {
                ClearAnchoredGuards();
                HWND hLV = g_hListView.load();
                POINT pt{};
                if (lParam != static_cast<LPARAM>(-1)) {
                    pt.x = static_cast<short>(LOWORD(lParam));
                    pt.y = static_cast<short>(HIWORD(lParam));
                } else {
                    GetCursorPos(&pt);
                }
                RecordContextMenuPoint(hLV, pt);
                break;
            }
            case WM_NOTIFY: {
                auto* nmhdr = reinterpret_cast<const NMHDR*>(lParam);
                if (nmhdr && nmhdr->hwndFrom == g_hListView.load()) {
                    if (nmhdr->code == LVN_BEGINLABELEDITW ||
                        nmhdr->code == LVN_BEGINLABELEDITA) {
                        ClearAnchoredGuards();
                        auto* disp =
                            reinterpret_cast<const NMLVDISPINFOW*>(lParam);
                        RecordLabelEditByListViewIndex(nmhdr->hwndFrom,
                                                       disp->item.iItem);
                    } else if (nmhdr->code == LVN_ENDLABELEDITW) {
                        auto* disp =
                            reinterpret_cast<const NMLVDISPINFOW*>(lParam);
                        if (disp && disp->item.pszText) {
                            RecordEndLabelEditName(disp->item.pszText);
                        }
                    } else if (nmhdr->code == LVN_ENDLABELEDITA) {
                        auto* disp =
                            reinterpret_cast<const NMLVDISPINFOA*>(lParam);
                        if (disp && disp->item.pszText &&
                            disp->item.pszText[0] != '\0') {
                            WCHAR wideBuf[MAX_PATH] = {};
                            if (MultiByteToWideChar(
                                    CP_ACP, 0, disp->item.pszText, -1, wideBuf,
                                    ARRAYSIZE(wideBuf)) > 0) {
                                RecordEndLabelEditName(wideBuf);
                            }
                        }
                    } else if (nmhdr->code == LVN_BEGINDRAG ||
                               nmhdr->code == LVN_BEGINRDRAG) {
                        ClearAnchoredGuards();
                    }
                }
                break;
            }
            case WM_NCDESTROY: {
                ClearAnchoredGuards();
                ResetDesktopComCache();
                g_defViewSubclassed = false;
                g_hDefView.store(nullptr);
                break;
            }
            default:
                break;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ============================================================================
// SHChangeNotify Handling & Desktop-Thread Message Window
// ============================================================================

std::wstring GetPathFromPidl(PCIDLIST_ABSOLUTE pidl) {
    if (!pidl) {
        return {};
    }
    WCHAR path[MAX_PATH * 2] = {};
    if (!SHGetPathFromIDListW(pidl, path) || path[0] == L'\0') {
        return {};
    }
    return ToLower(path);
}

void HandleShellChangeNotify(WPARAM wParam, LPARAM lParam) {
    PIDLIST_ABSOLUTE* ppidl = nullptr;
    LONG event = 0;
    HANDLE hLock = SHChangeNotification_Lock(
        reinterpret_cast<HANDLE>(wParam), static_cast<DWORD>(lParam), &ppidl,
        &event);
    if (!hLock) {
        return;
    }

    std::wstring path1 = (ppidl && ppidl[0]) ? GetPathFromPidl(ppidl[0]) : L"";
    std::wstring path2 = (ppidl && ppidl[1]) ? GetPathFromPidl(ppidl[1]) : L"";
    SHChangeNotification_Unlock(hLock);

    bool needSync = false;

    if (event & (SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER)) {
        std::wstring oldNameLower;
        std::wstring newNameLower;
        bool oldOnDesktop = IsDirectChildOfDesktop(path1, &oldNameLower);
        bool newOnDesktop = IsDirectChildOfDesktop(path2, &newNameLower);

        if (oldOnDesktop && newOnDesktop) {
            if (g_knownItems.find(oldNameLower) != g_knownItems.end()) {
                // Existing desktop item renamed by user (e.g. F2 or finishing
                // right-click New rename): update baseline without moving it.
                g_knownItems.erase(oldNameLower);
                g_knownItems.insert(newNameLower);
                Wh_Log(L"Desktop rename tracked: [%ls] -> [%ls]",
                       oldNameLower.c_str(), newNameLower.c_str());
            } else if (g_knownItems.find(newNameLower) != g_knownItems.end()) {
                // Already handled via low-latency FS watcher / pre-paint sync.
            } else {
                // Previously untracked (e.g. ignored temp file like .crdownload)
                // renamed to final filename: trigger anchor check!
                Wh_Log(L"Temp/untracked file finalized on desktop: [%ls] -> [%ls]",
                       oldNameLower.c_str(), newNameLower.c_str());
                needSync = true;
            }
        } else if (newOnDesktop) {
            // Moved into desktop from another folder.
            if (g_knownItems.find(newNameLower) == g_knownItems.end()) {
                needSync = true;
            }
        } else if (oldOnDesktop) {
            g_knownItems.erase(oldNameLower);
        }
    } else if (event & (SHCNE_CREATE | SHCNE_MKDIR)) {
        std::wstring nameLower;
        if (IsDirectChildOfDesktop(path1, &nameLower) &&
            g_knownItems.find(nameLower) == g_knownItems.end()) {
            needSync = true;
        }
    } else if (event & (SHCNE_DELETE | SHCNE_RMDIR)) {
        std::wstring nameLower;
        if (IsDirectChildOfDesktop(path1, &nameLower)) {
            if (g_knownItems.find(nameLower) != g_knownItems.end()) {
                g_recentlyRemovedKnown[nameLower] =
                    GetTickCount64() + kRecentRemovedKnownTtlMs;
            }
            g_knownItems.erase(nameLower);
            g_pendingViewItems.erase(nameLower);
        }
    }

    if (needSync) {
        PostImmediateDesktopSync();
    }
}

// ============================================================================
// Low-Latency Desktop Directory Watcher (ReadDirectoryChangesW, Overlapped)
// ============================================================================

struct DirWatchSlot {
    std::wstring dirPath;
    HANDLE hDir = INVALID_HANDLE_VALUE;
    HANDLE hEvent = nullptr;
    OVERLAPPED ov{};
    alignas(DWORD) BYTE buffer[16384]{};
    bool pendingIo = false;
    std::wstring pendingRenameOldLower;
};

void CloseDirWatchSlot(DirWatchSlot& slot) {
    if (slot.hDir != INVALID_HANDLE_VALUE) {
        if (slot.pendingIo) {
            CancelIoEx(slot.hDir, &slot.ov);
            DWORD dummy = 0;
            GetOverlappedResult(slot.hDir, &slot.ov, &dummy, TRUE);
            slot.pendingIo = false;
        }
        CloseHandle(slot.hDir);
        slot.hDir = INVALID_HANDLE_VALUE;
    }
    if (slot.hEvent) {
        CloseHandle(slot.hEvent);
        slot.hEvent = nullptr;
    }
}

bool ArmDirWatchSlot(DirWatchSlot& slot) {
    if (slot.hDir == INVALID_HANDLE_VALUE || !slot.hEvent) {
        return false;
    }
    ResetEvent(slot.hEvent);
    ZeroMemory(&slot.ov, sizeof(slot.ov));
    slot.ov.hEvent = slot.hEvent;
    BOOL ok = ReadDirectoryChangesW(
        slot.hDir, slot.buffer, static_cast<DWORD>(sizeof(slot.buffer)),
        /*bWatchSubtree=*/FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME, nullptr,
        &slot.ov, nullptr);
    if (!ok && GetLastError() != ERROR_IO_PENDING) {
        slot.pendingIo = false;
        return false;
    }
    slot.pendingIo = true;
    return true;
}

void ProcessDirWatchBuffer(DirWatchSlot& slot, DWORD bytesTransferred) {
    if (bytesTransferred == 0) {
        // Buffer overflow fallback: trigger a full sync check.
        PostImmediateDesktopSync();
        return;
    }

    ULONGLONG now = GetTickCount64();
    std::vector<FsEvent> localEvents;
    bool shouldNotify = false;

    const BYTE* ptr = slot.buffer;
    while (true) {
        const auto* fni =
            reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(ptr);
        size_t chars = fni->FileNameLength / sizeof(WCHAR);
        if (chars > 0) {
            std::wstring rawName(fni->FileName, chars);
            // Only care about direct children of Desktop (no path separators).
            if (rawName.find_first_of(L"\\/") == std::wstring::npos &&
                _wcsicmp(rawName.c_str(), L"desktop.ini") != 0) {
                std::wstring lower = ToLower(rawName);
                auto isHiddenOrSystem = [&slot, &rawName]() -> bool {
                    std::wstring fullPath = slot.dirPath + L"\\" + rawName;
                    DWORD attrs = GetFileAttributesW(fullPath.c_str());
                    return attrs != INVALID_FILE_ATTRIBUTES &&
                           (attrs &
                            (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) !=
                               0;
                };
                switch (fni->Action) {
                    case FILE_ACTION_ADDED: {
                        if (!isHiddenOrSystem()) {
                            localEvents.push_back(
                                FsEvent{FsEventType::Added, lower, {}, now});
                            shouldNotify = true;
                        }
                        break;
                    }
                    case FILE_ACTION_REMOVED: {
                        localEvents.push_back(
                            FsEvent{FsEventType::Removed, lower, {}, now});
                        break;
                    }
                    case FILE_ACTION_RENAMED_OLD_NAME: {
                        slot.pendingRenameOldLower = std::move(lower);
                        break;
                    }
                    case FILE_ACTION_RENAMED_NEW_NAME: {
                        if (!slot.pendingRenameOldLower.empty()) {
                            localEvents.push_back(
                                FsEvent{FsEventType::Renamed,
                                        std::move(slot.pendingRenameOldLower),
                                        lower, now});
                            slot.pendingRenameOldLower.clear();
                            if (!isHiddenOrSystem()) {
                                shouldNotify = true;
                            }
                        } else if (!isHiddenOrSystem()) {
                            localEvents.push_back(
                                FsEvent{FsEventType::Added, lower, {}, now});
                            shouldNotify = true;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        if (fni->NextEntryOffset == 0) {
            break;
        }
        if (static_cast<size_t>((ptr - slot.buffer) + fni->NextEntryOffset) >=
            bytesTransferred) {
            break;
        }
        ptr += fni->NextEntryOffset;
    }

    if (!localEvents.empty()) {
        std::lock_guard<std::mutex> lock(g_fsMutex);
        g_fsEvents.insert(g_fsEvents.end(),
                          std::make_move_iterator(localEvents.begin()),
                          std::make_move_iterator(localEvents.end()));
    }

    if (shouldNotify) {
        // Arm the lock-free pre-paint flag immediately (within <1ms of disk
        // creation) and also post an immediate sync message as fallback.
        PostImmediateDesktopSync();
    }
}

DWORD WINAPI DesktopFsWatchThreadProc(LPVOID) {
    std::vector<std::wstring> dirs;
    std::wstring userDir = GetDesktopKnownFolderLower(FOLDERID_Desktop);
    std::wstring publicDir = GetDesktopKnownFolderLower(FOLDERID_PublicDesktop);
    if (!userDir.empty()) {
        dirs.push_back(std::move(userDir));
    }
    if (!publicDir.empty() &&
        (dirs.empty() || dirs[0] != publicDir)) {
        dirs.push_back(std::move(publicDir));
    }

    std::vector<DirWatchSlot> slots(dirs.size());
    for (size_t i = 0; i < dirs.size(); ++i) {
        slots[i].dirPath = dirs[i];
        slots[i].hDir = CreateFileW(
            dirs[i].c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr);
        if (slots[i].hDir == INVALID_HANDLE_VALUE) {
            Wh_Log(L"FS watcher failed to open [%ls]: %lu", dirs[i].c_str(),
                   GetLastError());
            continue;
        }
        slots[i].hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!slots[i].hEvent || !ArmDirWatchSlot(slots[i])) {
            CloseDirWatchSlot(slots[i]);
        } else {
            Wh_Log(L"FS watcher active on [%ls]", dirs[i].c_str());
        }
    }

    while (!g_unloading.load()) {
        HANDLE waitHandles[3] = {};
        size_t slotMap[3] = {};
        DWORD waitCount = 0;

        if (g_stopEvent) {
            waitHandles[waitCount++] = g_stopEvent;
        }
        for (size_t i = 0; i < slots.size() && waitCount < 3; ++i) {
            if (slots[i].pendingIo && slots[i].hEvent) {
                slotMap[waitCount] = i;
                waitHandles[waitCount++] = slots[i].hEvent;
            }
        }

        if (waitCount <= (g_stopEvent ? 1u : 0u)) {
            break;
        }

        DWORD waitRes =
            WaitForMultipleObjects(waitCount, waitHandles, FALSE, INFINITE);
        if (g_unloading.load()) {
            break;
        }
        if (waitRes == WAIT_OBJECT_0 && g_stopEvent) {
            break;
        }
        if (waitRes > WAIT_OBJECT_0 &&
            waitRes < WAIT_OBJECT_0 + waitCount) {
            size_t idx = slotMap[waitRes - WAIT_OBJECT_0];
            auto& slot = slots[idx];
            slot.pendingIo = false;
            DWORD bytes = 0;
            if (GetOverlappedResult(slot.hDir, &slot.ov, &bytes, FALSE)) {
                ProcessDirWatchBuffer(slot, bytes);
            } else {
                DWORD err = GetLastError();
                if (err == ERROR_NOTIFY_ENUM_DIR) {
                    PostImmediateDesktopSync();
                } else if (err == ERROR_OPERATION_ABORTED) {
                    break;
                }
            }
            if (!g_unloading.load()) {
                ArmDirWatchSlot(slot);
            }
        } else {
            break;
        }
    }

    for (auto& slot : slots) {
        CloseDirWatchSlot(slot);
    }
    Wh_Log(L"FS watcher thread exited");
    return 0;
}

void StartDesktopFsWatcher() {
    if (g_fsWatchThread || g_unloading.load()) {
        return;
    }
    g_fsWatchThread = CreateThread(nullptr, 0, DesktopFsWatchThreadProc,
                                   nullptr, 0, nullptr);
}

ULONG RegisterDesktopFolderNotify(HWND hMsgWnd, REFKNOWNFOLDERID rfid) {
    CoTaskMemPtr<ITEMIDLIST> pidl;
    if (FAILED(SHGetKnownFolderIDList(rfid, 0, nullptr, pidl.Put())) || !pidl) {
        return 0;
    }

    SHChangeNotifyEntry entry{};
    entry.pidl = pidl.Get();
    entry.fRecursive = FALSE;

    return SHChangeNotifyRegister(
        hMsgWnd,
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_CREATE | SHCNE_MKDIR | SHCNE_DELETE | SHCNE_RMDIR |
            SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER,
        WM_APP_SHCHANGE, 1, &entry);
}

void CleanupOnDesktopThread(HWND hMsgWnd) {
    if (hMsgWnd) {
        KillTimer(hMsgWnd, kTimerSyncDesktop);
        KillTimer(hMsgWnd, kTimerRetryInit);
        KillTimer(hMsgWnd, kTimerPendingFlush);
    }
    g_pendingFlushTimerActive = false;

    if (g_shNotifyUser) {
        SHChangeNotifyDeregister(g_shNotifyUser);
        g_shNotifyUser = 0;
    }
    if (g_shNotifyPublic) {
        SHChangeNotifyDeregister(g_shNotifyPublic);
        g_shNotifyPublic = 0;
    }

    HWND hLV = g_hListView.load();
    if (g_listViewSubclassed && hLV && IsWindow(hLV)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hLV, DesktopListViewSubclassProc);
    }
    g_listViewSubclassed = false;

    HWND hDef = g_hDefView.load();
    if (g_defViewSubclassed && hDef && IsWindow(hDef)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hDef, DesktopDefViewSubclassProc);
    }
    g_defViewSubclassed = false;

    ClearAnchoredGuards();
    ResetDesktopComCache();

    g_knownItems.clear();
    g_pendingViewItems.clear();
    g_recentlyRemovedKnown.clear();
    g_lastObservedLvCount = -1;
    g_pendingAnchorCheck.store(false, std::memory_order_release);
    g_baselineLoaded.store(false);
    g_desktopReady.store(false);

    if (hMsgWnd) {
        DestroyWindow(hMsgWnd);
    }
    g_hMsgWnd.store(nullptr);
}

LRESULT CALLBACK MsgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_APP_SHCHANGE:
            if (!g_unloading.load()) {
                HandleShellChangeNotify(wParam, lParam);
            }
            return 0;

        case WM_APP_SYNC_NOW:
            if (!g_unloading.load()) {
                SyncDesktopAndAnchorNewItems(/*fromPrePaint=*/false);
            }
            return 0;

        case WM_APP_SETTINGS_CHANGED:
            if (!g_unloading.load()) {
                ClearAnchoredGuards();
                LoadSettings();
                IFolderView2* pFv2 = GetDesktopFolderView2Cached();
                if (pFv2) {
                    EnsureAutoArrangeDisabledIfNeeded(pFv2);
                }
            }
            return 0;

        case WM_APP_CLEANUP:
            CleanupOnDesktopThread(hWnd);
            return 0;

        case WM_TIMER:
            if (wParam == kTimerSyncDesktop) {
                KillTimer(hWnd, kTimerSyncDesktop);
                if (!g_unloading.load()) {
                    SyncDesktopAndAnchorNewItems(/*fromPrePaint=*/false);
                }
                return 0;
            }
            if (wParam == kTimerPendingFlush) {
                if (g_unloading.load()) {
                    SetPendingFlushTimer(false);
                    return 0;
                }
                SyncDesktopAndAnchorNewItems(/*fromPrePaint=*/false);
                return 0;
            }
            if (wParam == kTimerRetryInit) {
                if (g_unloading.load()) {
                    KillTimer(hWnd, kTimerRetryInit);
                    return 0;
                }
                SyncDesktopAndAnchorNewItems(/*fromPrePaint=*/false);
                if (g_baselineLoaded.load()) {
                    KillTimer(hWnd, kTimerRetryInit);
                }
                return 0;
            }
            break;

        default:
            break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// Execute a callback synchronously on the thread owning targetWnd.
struct RunOnThreadPayload {
    void (*fn)(void*);
    void* param;
};

UINT GetRunOnThreadMsg() {
    static UINT msg =
        RegisterWindowMessageW(L"Windhawk_DesktopColumnAnchor_RunOnThread");
    return msg;
}

LRESULT CALLBACK RunOnThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* cwp = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (cwp && cwp->message == GetRunOnThreadMsg()) {
            auto* payload = reinterpret_cast<RunOnThreadPayload*>(cwp->lParam);
            if (payload && payload->fn) {
                payload->fn(payload->param);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool RunOnWindowThread(HWND hTargetWnd, void (*fn)(void*), void* param) {
    if (!hTargetWnd || !IsWindow(hTargetWnd) || !fn) {
        return false;
    }
    DWORD targetTid = GetWindowThreadProcessId(hTargetWnd, nullptr);
    if (targetTid == 0) {
        return false;
    }
    if (targetTid == GetCurrentThreadId()) {
        fn(param);
        return true;
    }

    HHOOK hHook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnThreadHookProc,
                                    nullptr, targetTid);
    if (!hHook) {
        return false;
    }

    RunOnThreadPayload payload{fn, param};
    SendMessageW(hTargetWnd, GetRunOnThreadMsg(), 0,
                 reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hHook);
    return true;
}

// ============================================================================
// Desktop Attachment & Initialization
// ============================================================================

struct AttachContext {
    bool calledFromModInit = false;
    bool hooksNeedApply = false;
    bool succeeded = false;
};

void AttachOnDesktopThread(void* rawCtx) {
    auto* ctx = static_cast<AttachContext*>(rawCtx);
    if (g_unloading.load()) {
        return;
    }

    HWND hDefView = nullptr;
    HWND hListView = nullptr;
    if (!FindDesktopWindows(&hDefView, &hListView)) {
        return;
    }

    g_hDefView.store(hDefView);
    g_hListView.store(hListView);
    g_desktopThreadId.store(GetCurrentThreadId());

    g_desktopUserDirLower = GetDesktopKnownFolderLower(FOLDERID_Desktop);
    g_desktopPublicDirLower = GetDesktopKnownFolderLower(FOLDERID_PublicDesktop);

    // Register message-only window owned by the desktop thread.
    WNDCLASSW wc{};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kMsgWindowClassName;
    RegisterClassW(&wc);

    HWND hMsg = CreateWindowExW(0, kMsgWindowClassName, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr,
                                GetModuleHandleW(nullptr), nullptr);
    if (!hMsg) {
        Wh_Log(L"CreateWindowExW for message window failed: %lu",
               GetLastError());
        return;
    }
    g_hMsgWnd.store(hMsg);

    // Install COM hooks on desktop drop targets and IShellView.
    ComPtr<IShellView> spView;
    AcquireDesktopShellView(spView);
    bool wasInstalled = g_comHooksInstalled.load();
    if (InstallDesktopComHooks(spView.Get(), hDefView, hListView) &&
        !wasInstalled && !ctx->calledFromModInit) {
        ctx->hooksNeedApply = true;
    }

    // Subclass SHELLDLL_DefView and SysListView32.
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hDefView, DesktopDefViewSubclassProc, 0)) {
        g_defViewSubclassed = true;
    }
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hListView, DesktopListViewSubclassProc, 0)) {
        g_listViewSubclassed = true;
    }

    // Register shell change notifications for User Desktop & Public Desktop.
    g_shNotifyUser = RegisterDesktopFolderNotify(hMsg, FOLDERID_Desktop);
    g_shNotifyPublic = RegisterDesktopFolderNotify(hMsg, FOLDERID_PublicDesktop);
    Wh_Log(L"SHChangeNotifyRegister user=%lu public=%lu", g_shNotifyUser,
           g_shNotifyPublic);

    // Capture initial desktop baseline & disable Auto Arrange on first load.
    SyncDesktopAndAnchorNewItems();
    if (!g_baselineLoaded.load()) {
        SetTimer(hMsg, kTimerRetryInit, 1000, nullptr);
    }

    g_desktopReady.store(true);
    ctx->succeeded = true;
}

bool TryAttachDesktop(bool calledFromModInit) {
    HWND hDefView = nullptr;
    HWND hListView = nullptr;
    if (!FindDesktopWindows(&hDefView, &hListView)) {
        return false;
    }

    AttachContext ctx{};
    ctx.calledFromModInit = calledFromModInit;
    if (!RunOnWindowThread(hDefView, AttachOnDesktopThread, &ctx)) {
        return false;
    }

    if (ctx.hooksNeedApply && !g_unloading.load()) {
        Wh_ApplyHookOperations();
    }

    if (ctx.succeeded && !g_unloading.load()) {
        StartDesktopFsWatcher();
    }

    return ctx.succeeded;
}

DWORD WINAPI DelayedInitThreadProc(LPVOID) {
    Wh_Log(L"Desktop not ready during Wh_ModInit; waiting for desktop view...");
    for (int attempt = 0; attempt < 60 && !g_unloading.load(); ++attempt) {
        if (TryAttachDesktop(/*calledFromModInit=*/false)) {
            Wh_Log(L"Delayed desktop attachment succeeded on attempt %d",
                   attempt + 1);
            return 0;
        }
        if (g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
            return 0;
        }
    }
    Wh_Log(L"Stopped waiting for desktop view (non-desktop explorer instance)");
    return 0;
}

}  // namespace

// ============================================================================
// Windhawk Mod Lifecycle Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"DesktopColumnAnchor (桌面列锚) initializing...");

    g_unloading.store(false);
    g_desktopReady.store(false);
    g_comHooksInstalled.store(false);
    g_baselineLoaded.store(false);
    g_pendingAnchorCheck.store(false);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    LoadSettings();

    // Hook Explorer drag-and-drop initiation APIs early in Wh_ModInit so
    // dragging files from any open folder window onto the desktop is captured
    // reliably even across Explorer UI threads and async drop operations.
    InstallProcessDragDropHooks();

    // If desktop windows are already up in this explorer.exe process, attach
    // synchronously inside Wh_ModInit so Windhawk applies hooks automatically.
    if (TryAttachDesktop(/*calledFromModInit=*/true)) {
        Wh_Log(L"Attached to desktop synchronously in Wh_ModInit");
        return TRUE;
    }

    // Explorer may still be initializing its desktop shell window, or this may
    // be a secondary explorer.exe window process. Wait briefly on a background
    // thread without blocking Explorer startup.
    g_initThread = CreateThread(nullptr, 0, DelayedInitThreadProc, nullptr, 0,
                                nullptr);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    HWND hMsg = g_hMsgWnd.load();
    if (hMsg && IsWindow(hMsg)) {
        SendMessageW(hMsg, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else {
        LoadSettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Wh_ModBeforeUninit");
    g_unloading.store(true);

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_initThread) {
        WaitForSingleObject(g_initThread, 5000);
        CloseHandle(g_initThread);
        g_initThread = nullptr;
    }
    if (g_fsWatchThread) {
        WaitForSingleObject(g_fsWatchThread, 5000);
        CloseHandle(g_fsWatchThread);
        g_fsWatchThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    HWND hMsg = g_hMsgWnd.load();
    if (hMsg && IsWindow(hMsg)) {
        SendMessageW(hMsg, WM_APP_CLEANUP, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"DesktopColumnAnchor (桌面列锚) uninitialized");
    UnregisterClassW(kMsgWindowClassName, GetModuleHandleW(nullptr));
}
