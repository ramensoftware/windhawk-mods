// ==WindhawkMod==
// @id              desktop-shortcut-cleaner
// @name            Desktop Shortcut Cleaner
// @description     Hide desktop shortcut labels, arrows and admin shields, and replace the hover box with an icon-shaped glow, without changing File Explorer.
// @version         1.2.3
// @author          ELSEGuy
// @github          https://github.com/ELSEGuy
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -lcomctl32 -lshell32 -ladvapi32 -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Shortcut Cleaner

Clean up the Windows 11 desktop without touching File Explorer: hide shortcut
labels, arrows and administrator shields, and replace the white hover box
with a soft glow that follows each icon's shape.

![Hovering and selecting desktop icons shows a soft white glow around each icon's shape instead of the white box](https://raw.githubusercontent.com/ELSEGuy/desktop-shortcut-cleaner-Windhawk/39fa539f259d9b3dff3c2b2d10da7e7cd3cc95be/Screnshot/glow_demo.gif)

## Features

- **Hide shortcut labels only.** Shortcut names are hidden while folders and
  regular files keep theirs. An optional toggle hides every desktop label.
- **Hide shortcut arrows** on the desktop only.
- **Hide administrator shields** on shortcuts whose target uses the
  *Run this program as an administrator* compatibility setting. The raw icon
  is drawn instead, and the shield stays hidden after refreshing the desktop.
  The shortcut still runs as administrator.
- **Icon glow instead of the hover box.** The white box behind hovered,
  selected and drop-target icons is removed and replaced with a soft white
  glow built from the icon's own outline: subtle on hover, brighter when
  selected.
- **File Explorer is unchanged.** Arrows, shields, labels and highlights
  there stay exactly as Windows draws them.

## Known limitations

- Labels are matched by their displayed text, so a non-shortcut item showing
  the same name as a shortcut (for example `Notes.lnk` and `Notes.txt` with
  extensions hidden) loses its label too.
- Shield hiding covers the *Run this program as an administrator*
  compatibility setting only. Shortcuts to programs whose manifest requests
  elevation (many installers) keep their shield, as do shortcuts whose
  target is on a network path.

## Before and after

![Desktop before enabling the mod, with shortcut names and arrows](https://raw.githubusercontent.com/ELSEGuy/desktop-shortcut-cleaner-Windhawk/39fa539f259d9b3dff3c2b2d10da7e7cd3cc95be/Screnshot/before.png)

![Desktop with the mod enabled, shortcut names and arrows hidden](https://raw.githubusercontent.com/ELSEGuy/desktop-shortcut-cleaner-Windhawk/39fa539f259d9b3dff3c2b2d10da7e7cd3cc95be/Screnshot/Shortcut_text_and_arrow.png)

## Credits

Based in part on desktop icon text-hiding code by kivsak. The desktop
detection and icon-label hiding approach were adapted from their script,
which hides the names of all desktop icons. This project extends that
approach with its own shortcut-only label filtering. The desktop-only
shortcut-arrow, administrator-shield and icon-glow handling are implemented
separately by this project.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hide_text: true
  $name: Hide desktop shortcut text
  $description: Hide text only for shortcuts on the desktop. Folder and normal-file names stay visible.

- hide_all_text: false
  $name: Hide all desktop icon text
  $description: Hide the names of every icon on the desktop. This overrides the shortcut-only text option.

- hide_arrows: true
  $name: Hide desktop shortcut arrows
  $description: Hide the standard shortcut arrow only while desktop icons are being painted. File Explorer stays unchanged.

- hide_admin_shields: true
  $name: Hide desktop administrator shields
  $description: On the desktop only, draw the raw icon for shortcuts whose target uses the "Run as administrator" compatibility setting. The shortcut still runs as administrator.

- icon_glow: true
  $name: Replace hover box with icon glow
  $description: On the desktop only, remove the white box shown behind hovered and selected icons and draw a soft white glow around the icon's shape instead. Selected icons glow brighter.

*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <vsstyle.h>

#include <atomic>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// Settings / state
// ---------------------------------------------------------------------------

struct {
    bool hide_text;
    bool hide_all_text;
    bool hide_arrows;
    bool hide_shields;
    bool icon_glow;
} g_settings;

thread_local bool g_isDrawingDesktop = false;

// The desktop ListView this mod subclassed. Kept so the subclass can always be
// removed on unload, even if the window can no longer be found by lookup (for
// example after the desktop view moved under a WorkerW window).
std::atomic<HWND> g_desktopListView{nullptr};

// Sent to the desktop ListView on unload so its timer and caches are cleaned
// up on the desktop thread itself.
UINT g_uninitMessage = 0;

std::unordered_set<std::wstring> g_shortcutNames;
DWORD g_lastShortcutCacheTick = 0;

UINT g_linkOverlayMask = 0;
int g_linkOverlaySlot = 0;

HIMAGELIST g_cleanShieldImageList = nullptr;
std::unordered_map<int, int> g_cleanIconBySystemImage;
DWORD g_lastShieldCacheTick = 0;
HIMAGELIST g_cachedDesktopSystemImageList = nullptr;
int g_cachedDesktopIconWidth = 0;
int g_cachedDesktopIconHeight = 0;
int g_cachedDesktopItemCount = -1;
// System-image index of every desktop item when the map was last built.
// Explorer assigns real icon indexes asynchronously after a refresh, so the
// map must be rebuilt whenever any of these change, not only the item count.
std::vector<int> g_cachedDesktopImageSignature;

// Timer used to retry a shield-map rebuild that was throttled, so the map is
// corrected even if nothing repaints the desktop afterwards.
constexpr UINT_PTR kShieldRefreshTimerId = 0x57484B53;
constexpr UINT kShieldRefreshThrottleMs = 250;
constexpr UINT kShieldRefreshRetryMs = 300;

SRWLOCK g_cleanShieldCacheLock = SRWLOCK_INIT;
volatile LONG g_shieldCacheUpdateInProgress = 0;
volatile LONG g_shieldCacheGeneration = 0;
volatile LONG g_isUnloading = 0;

thread_local bool g_isDrawingCleanShieldIcon = false;

// Desktop ListView currently being painted on this thread (if any).
thread_local HWND g_drawingDesktopHwnd = nullptr;

// Set while rendering an icon into an offscreen bitmap to build its glow,
// so the IImageList::Draw hook passes that draw straight through.
thread_local bool g_isGeneratingGlow = false;

// ---------------------------------------------------------------------------
// Desktop window helpers
// ---------------------------------------------------------------------------

static HWND FindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (!parent) {
        return nullptr;
    }

    return FindWindowExW(parent, nullptr, cls, win);
}

static bool IsDesktopFolderView(HWND hWnd) {
    WCHAR buf[64];

    if (!GetClassNameW(hWnd, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"SysListView32")) {
        return false;
    }

    if (!GetWindowTextW(hWnd, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"FolderView")) {
        return false;
    }

    HWND defView = GetAncestor(hWnd, GA_PARENT);
    if (!defView) {
        return false;
    }

    if (!GetClassNameW(defView, buf, ARRAYSIZE(buf)) ||
        _wcsicmp(buf, L"SHELLDLL_DefView")) {
        return false;
    }

    HWND top = GetAncestor(defView, GA_PARENT);
    if (!top) {
        return false;
    }

    if ((!GetClassNameW(top, buf, ARRAYSIZE(buf)) ||
         _wcsicmp(buf, L"Progman")) &&
        top != GetShellWindow()) {
        return false;
    }

    return true;
}

static HWND FindFolderViewInTopLevel(HWND topLevel) {
    HWND defView = FindChild(topLevel, L"SHELLDLL_DefView", nullptr);
    if (!defView) {
        return nullptr;
    }

    HWND listView = FindChild(defView, L"SysListView32", L"FolderView");
    if (!listView) {
        return nullptr;
    }

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(listView, &pid) ||
        pid != GetCurrentProcessId()) {
        return nullptr;
    }

    return listView;
}

static HWND GetDesktopFolderView() {
    // Usually the desktop view lives under Progman (the shell window).
    if (HWND shell = GetShellWindow()) {
        if (HWND listView = FindFolderViewInTopLevel(shell)) {
            return listView;
        }
    }

    // Wallpaper slideshows and wallpaper tools can move it under a WorkerW
    // window (Windows 10 and Windows 11 before 24H2).
    HWND workerW = nullptr;
    while ((workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr))) {
        if (HWND listView = FindFolderViewInTopLevel(workerW)) {
            return listView;
        }
    }

    return nullptr;
}

// ---------------------------------------------------------------------------
// Shortcut-name cache for text hiding
// ---------------------------------------------------------------------------

static void UpdateShortcutCache() {
    DWORD now = GetTickCount();

    if (g_lastShortcutCacheTick != 0 &&
        now - g_lastShortcutCacheTick < 500) {
        return;
    }

    g_lastShortcutCacheTick = now;

    std::unordered_set<std::wstring> fresh;

    IShellFolder* desktop = nullptr;
    if (FAILED(SHGetDesktopFolder(&desktop)) || !desktop) {
        return;
    }

    IEnumIDList* enumerator = nullptr;
    if (SUCCEEDED(desktop->EnumObjects(
            nullptr,
            SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,
            &enumerator)) &&
        enumerator) {

        LPITEMIDLIST pidl = nullptr;

        while (enumerator->Next(1, &pidl, nullptr) == S_OK) {
            SFGAOF attributes = SFGAO_LINK;
            LPCITEMIDLIST pidlConst = pidl;

            if (SUCCEEDED(desktop->GetAttributesOf(
                    1,
                    &pidlConst,
                    &attributes)) &&
                (attributes & SFGAO_LINK)) {

                STRRET str = {};
                if (SUCCEEDED(desktop->GetDisplayNameOf(
                        pidl,
                        SHGDN_NORMAL,
                        &str))) {

                    WCHAR name[MAX_PATH];
                    if (SUCCEEDED(StrRetToBufW(
                            &str,
                            pidl,
                            name,
                            ARRAYSIZE(name)))) {
                        fresh.insert(name);
                    }
                }
            }

            CoTaskMemFree(pidl);
            pidl = nullptr;
        }

        enumerator->Release();
    }

    desktop->Release();
    g_shortcutNames = std::move(fresh);
}

static bool ShouldHideLabel(LPCWSTR text, int cch) {
    if (!text) {
        return false;
    }

    if (g_settings.hide_all_text) {
        return true;
    }

    if (!g_settings.hide_text) {
        return false;
    }

    std::wstring value(
        text,
        cch < 0 ? wcslen(text) : static_cast<size_t>(cch));

    return g_shortcutNames.count(value) != 0;
}

// ---------------------------------------------------------------------------
// Standard Windows shortcut-overlay identification
// ---------------------------------------------------------------------------

#ifndef IDO_SHGIOI_LINK
#define IDO_SHGIOI_LINK 0x0FFFFFFE
#endif

static bool ResolveLinkOverlayMask() {
    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) {
        Wh_Log(L"shell32.dll is not loaded");
        return false;
    }

    using SHGetIconOverlayIndexW_t = int(WINAPI*)(LPCWSTR, int);

    auto getOverlayIndex =
        reinterpret_cast<SHGetIconOverlayIndexW_t>(
            GetProcAddress(shell32, "SHGetIconOverlayIndexW"));

    if (!getOverlayIndex) {
        Wh_Log(L"SHGetIconOverlayIndexW wasn't found");
        return false;
    }

    int overlayIndex = getOverlayIndex(nullptr, IDO_SHGIOI_LINK);
    if (overlayIndex <= 0) {
        Wh_Log(L"Couldn't resolve the standard link overlay index: %d",
               overlayIndex);
        return false;
    }

    g_linkOverlayMask = INDEXTOOVERLAYMASK(overlayIndex);
    g_linkOverlaySlot = overlayIndex;
    Wh_Log(L"Standard link overlay index=%d mask=0x%X",
           overlayIndex,
           g_linkOverlayMask);

    return true;
}

// ---------------------------------------------------------------------------
// Raw-icon cache for RUNASADMIN desktop shortcuts
//
// Explorer's UAC shield is already present in the cached system-image-list
// bitmap for this compatibility case. Instead of modifying that shared cache,
// the mod extracts the shortcut's raw icon and substitutes it only while the
// desktop ListView is painting.
// ---------------------------------------------------------------------------

static const GUID kCLSID_ShellLink = {
    0x00021401,
    0x0000,
    0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

static const GUID kIID_IShellLinkW = {
    0x000214f9,
    0x0000,
    0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

static const GUID kIID_IPersistFile = {
    0x0000010b,
    0x0000,
    0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

static std::wstring ExpandPath(const WCHAR* path) {
    if (!path || !path[0]) {
        return {};
    }

    DWORD needed = ExpandEnvironmentStringsW(path, nullptr, 0);
    if (needed == 0) {
        return path;
    }

    std::wstring expanded(needed, L'\0');
    DWORD length = ExpandEnvironmentStringsW(path, expanded.data(), needed);
    if (length == 0 || length > needed) {
        return path;
    }

    expanded.resize(length - 1);  // Drop the terminating null.
    return expanded;
}

static bool RegistryLayerContainsRunAsAdmin(
    HKEY root,
    REGSAM view,
    const std::wstring& targetPath) {

    static constexpr WCHAR kLayersKey[] =
        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\"
        L"AppCompatFlags\\Layers";

    HKEY key = nullptr;
    if (RegOpenKeyExW(
            root,
            kLayersKey,
            0,
            KEY_QUERY_VALUE | view,
            &key) != ERROR_SUCCESS) {
        return false;
    }

    WCHAR data[2048] = {};
    DWORD type = 0;
    DWORD bytes = sizeof(data);

    LSTATUS status = RegQueryValueExW(
        key,
        targetPath.c_str(),
        nullptr,
        &type,
        reinterpret_cast<BYTE*>(data),
        &bytes);

    RegCloseKey(key);

    if (status != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ)) {
        return false;
    }

    data[ARRAYSIZE(data) - 1] = L'\0';
    return StrStrIW(data, L"RUNASADMIN") != nullptr;
}

static bool TargetUsesRunAsAdminLayer(const std::wstring& targetPath) {
    if (targetPath.empty()) {
        return false;
    }

    if (RegistryLayerContainsRunAsAdmin(
            HKEY_CURRENT_USER,
            0,
            targetPath) ||
        RegistryLayerContainsRunAsAdmin(
            HKEY_LOCAL_MACHINE,
            0,
            targetPath)) {
        return true;
    }

#ifdef _WIN64
    if (RegistryLayerContainsRunAsAdmin(
            HKEY_CURRENT_USER,
            KEY_WOW64_32KEY,
            targetPath) ||
        RegistryLayerContainsRunAsAdmin(
            HKEY_LOCAL_MACHINE,
            KEY_WOW64_32KEY,
            targetPath)) {
        return true;
    }
#endif

    return false;
}

static bool ReadShortcutIconInfo(
    const std::wstring& shortcutPath,
    std::wstring* targetPath,
    std::wstring* iconPath,
    int* iconIndex) {

    IShellLinkW* shellLink = nullptr;
    HRESULT result = CoCreateInstance(
        kCLSID_ShellLink,
        nullptr,
        CLSCTX_INPROC_SERVER,
        kIID_IShellLinkW,
        reinterpret_cast<void**>(&shellLink));

    if (FAILED(result) || !shellLink) {
        return false;
    }

    IPersistFile* persistFile = nullptr;
    result = shellLink->QueryInterface(
        kIID_IPersistFile,
        reinterpret_cast<void**>(&persistFile));

    if (FAILED(result) || !persistFile) {
        shellLink->Release();
        return false;
    }

    result = persistFile->Load(shortcutPath.c_str(), STGM_READ);
    persistFile->Release();

    if (FAILED(result)) {
        shellLink->Release();
        return false;
    }

    WCHAR target[MAX_PATH] = {};
    result = shellLink->GetPath(
        target,
        ARRAYSIZE(target),
        nullptr,
        SLGP_RAWPATH);

    if (FAILED(result) || !target[0]) {
        shellLink->Release();
        return false;
    }

    WCHAR icon[MAX_PATH] = {};
    int index = 0;
    if (FAILED(shellLink->GetIconLocation(
            icon,
            ARRAYSIZE(icon),
            &index))) {
        icon[0] = L'\0';
        index = 0;
    }

    shellLink->Release();

    *targetPath = ExpandPath(target);
    *iconPath = icon[0] ? ExpandPath(icon) : *targetPath;
    *iconIndex = icon[0] ? index : 0;

    if (PathIsRelativeW(iconPath->c_str())) {
        WCHAR shortcutDirectory[MAX_PATH];
        wcsncpy_s(
            shortcutDirectory,
            ARRAYSIZE(shortcutDirectory),
            shortcutPath.c_str(),
            _TRUNCATE);
        PathRemoveFileSpecW(shortcutDirectory);

        WCHAR combined[MAX_PATH];
        if (PathCombineW(
                combined,
                shortcutDirectory,
                iconPath->c_str())) {
            *iconPath = combined;
        }
    }

    return true;
}

static bool ShellDisplayNameToString(
    IShellFolder* folder,
    LPCITEMIDLIST pidl,
    SHGDNF flags,
    std::wstring* value) {

    STRRET str = {};
    if (FAILED(folder->GetDisplayNameOf(pidl, flags, &str))) {
        return false;
    }

    PWSTR buffer = nullptr;
    if (FAILED(StrRetToStrW(&str, pidl, &buffer)) || !buffer) {
        return false;
    }

    *value = buffer;
    CoTaskMemFree(buffer);
    return true;
}

static HICON ExtractIconFromFile(const std::wstring& path,
                                 int index,
                                 int size) {
    // Network paths can block the desktop thread until an SMB timeout if the
    // share is offline, so they're skipped.
    if (path.empty() || PathIsNetworkPathW(path.c_str())) {
        return nullptr;
    }

    HICON icon = nullptr;
    if (SHDefExtractIconW(path.c_str(), index, 0, &icon, nullptr,
                          MAKELONG(size, 0)) == S_OK &&
        icon) {
        return icon;
    }

    return nullptr;
}

static HICON ExtractRawShortcutIcon(
    const std::wstring& iconPath,
    int iconIndex,
    const std::wstring& targetPath,
    int width,
    int height) {

    int size = width > height ? width : height;

    if (HICON icon = ExtractIconFromFile(iconPath, iconIndex, size)) {
        return icon;
    }

    if (_wcsicmp(iconPath.c_str(), targetPath.c_str()) != 0) {
        if (HICON icon = ExtractIconFromFile(targetPath, 0, size)) {
            return icon;
        }
    }

    return nullptr;
}

class ScopedShieldCacheUpdate {
public:
    ScopedShieldCacheUpdate()
        : m_acquired(InterlockedCompareExchange(
              &g_shieldCacheUpdateInProgress,
              1,
              0) == 0) {
    }

    ~ScopedShieldCacheUpdate() {
        if (m_acquired) {
            InterlockedExchange(&g_shieldCacheUpdateInProgress, 0);
        }
    }

    bool IsAcquired() const {
        return m_acquired;
    }

private:
    bool m_acquired;
};

static void ClearGlowCache();

static void ClearShieldedShortcutIconCache() {
    ClearGlowCache();

    InterlockedIncrement(&g_shieldCacheGeneration);

    AcquireSRWLockExclusive(&g_cleanShieldCacheLock);

    HIMAGELIST oldImageList = g_cleanShieldImageList;
    g_cleanShieldImageList = nullptr;
    g_cleanIconBySystemImage.clear();
    g_lastShieldCacheTick = 0;
    g_cachedDesktopSystemImageList = nullptr;
    g_cachedDesktopIconWidth = 0;
    g_cachedDesktopIconHeight = 0;
    g_cachedDesktopItemCount = -1;
    g_cachedDesktopImageSignature.clear();

    ReleaseSRWLockExclusive(&g_cleanShieldCacheLock);

    if (oldImageList) {
        ImageList_Destroy(oldImageList);
    }
}

static std::vector<int> GetDesktopImageSignature(HWND desktopView) {
    int itemCount = ListView_GetItemCount(desktopView);

    std::vector<int> signature;
    signature.reserve(itemCount > 0 ? itemCount : 0);

    for (int itemIndex = 0; itemIndex < itemCount; itemIndex++) {
        LVITEMW item = {};
        item.mask = LVIF_IMAGE;
        item.iItem = itemIndex;

        if (!SendMessageW(desktopView,
                          LVM_GETITEMW,
                          0,
                          reinterpret_cast<LPARAM>(&item))) {
            item.iImage = -1;
        }

        signature.push_back(item.iImage);
    }

    return signature;
}

static bool ShieldIconCacheNeedsRefresh(HWND desktopView) {
    HIMAGELIST currentImageList =
        ListView_GetImageList(desktopView, LVSIL_NORMAL);
    int currentWidth = 0;
    int currentHeight = 0;

    if (!currentImageList ||
        !ImageList_GetIconSize(
            currentImageList,
            &currentWidth,
            &currentHeight)) {
        return true;
    }

    int currentItemCount = ListView_GetItemCount(desktopView);

    AcquireSRWLockShared(&g_cleanShieldCacheLock);
    bool needsRefresh =
        !g_cleanShieldImageList ||
        currentImageList != g_cachedDesktopSystemImageList ||
        currentWidth != g_cachedDesktopIconWidth ||
        currentHeight != g_cachedDesktopIconHeight ||
        currentItemCount != g_cachedDesktopItemCount;
    ReleaseSRWLockShared(&g_cleanShieldCacheLock);

    if (needsRefresh) {
        return true;
    }

    std::vector<int> signature = GetDesktopImageSignature(desktopView);

    AcquireSRWLockShared(&g_cleanShieldCacheLock);
    needsRefresh = signature != g_cachedDesktopImageSignature;
    ReleaseSRWLockShared(&g_cleanShieldCacheLock);

    return needsRefresh;
}

enum class ShieldRebuildResult {
    Rebuilt,  // The map was rebuilt.
    Skipped,  // Throttled or already running; worth retrying shortly.
    Failed,   // Couldn't rebuild; retrying right away won't help.
};

static ShieldRebuildResult UpdateShieldedShortcutIconCache(HWND desktopView,
                                                          bool force = false) {
    if (InterlockedCompareExchange(&g_isUnloading, 0, 0) != 0 ||
        !desktopView) {
        return ShieldRebuildResult::Failed;
    }

    ScopedShieldCacheUpdate updateGuard;
    if (!updateGuard.IsAcquired()) {
        return ShieldRebuildResult::Skipped;
    }

    DWORD now = GetTickCount();

    AcquireSRWLockShared(&g_cleanShieldCacheLock);
    DWORD lastUpdate = g_lastShieldCacheTick;
    ReleaseSRWLockShared(&g_cleanShieldCacheLock);

    if (!force &&
        lastUpdate != 0 &&
        now - lastUpdate < kShieldRefreshThrottleMs) {
        return ShieldRebuildResult::Skipped;
    }

    LONG cacheGeneration =
        InterlockedCompareExchange(&g_shieldCacheGeneration, 0, 0);

    AcquireSRWLockExclusive(&g_cleanShieldCacheLock);
    g_lastShieldCacheTick = now;
    ReleaseSRWLockExclusive(&g_cleanShieldCacheLock);

    HIMAGELIST systemImageList =
        ListView_GetImageList(desktopView, LVSIL_NORMAL);

    int iconWidth = 0;
    int iconHeight = 0;
    if (!systemImageList ||
        !ImageList_GetIconSize(
            systemImageList,
            &iconWidth,
            &iconHeight) ||
        iconWidth <= 0 ||
        iconHeight <= 0) {
        Wh_Log(L"Couldn't determine the desktop icon size");
        return ShieldRebuildResult::Failed;
    }

    HIMAGELIST freshImageList = ImageList_Create(
        iconWidth,
        iconHeight,
        ILC_COLOR32 | ILC_MASK,
        1,
        4);

    if (!freshImageList) {
        Wh_Log(L"Couldn't create the raw desktop icon list");
        return ShieldRebuildResult::Failed;
    }

    // Preserve any normal overlay requested by the desktop draw call. The
    // shortcut arrow is deliberately omitted when that setting is enabled.
    auto systemImageListInterface =
        reinterpret_cast<IImageList*>(systemImageList);

    for (int overlaySlot = 1; overlaySlot <= 15; overlaySlot++) {
        if (g_settings.hide_arrows &&
            overlaySlot == g_linkOverlaySlot) {
            continue;
        }

        int overlayImageIndex = -1;
        if (FAILED(systemImageListInterface->GetOverlayImage(
                overlaySlot,
                &overlayImageIndex)) ||
            overlayImageIndex < 0) {
            continue;
        }

        HICON overlayIcon = ImageList_GetIcon(
            systemImageList,
            overlayImageIndex,
            ILD_TRANSPARENT);

        if (!overlayIcon) {
            continue;
        }

        int privateOverlayIndex = ImageList_AddIcon(
            freshImageList,
            overlayIcon);
        DestroyIcon(overlayIcon);

        if (privateOverlayIndex >= 0) {
            ImageList_SetOverlayImage(
                freshImageList,
                privateOverlayIndex,
                overlaySlot);
        }
    }

    std::unordered_map<std::wstring, int> cleanIndexByName;

    IShellFolder* desktopFolder = nullptr;
    if (SUCCEEDED(SHGetDesktopFolder(&desktopFolder)) &&
        desktopFolder) {

        IEnumIDList* enumerator = nullptr;
        if (SUCCEEDED(desktopFolder->EnumObjects(
                nullptr,
                SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,
                &enumerator)) &&
            enumerator) {

            LPITEMIDLIST pidl = nullptr;
            while (enumerator->Next(1, &pidl, nullptr) == S_OK) {
                SFGAOF attributes = SFGAO_LINK;
                LPCITEMIDLIST pidlConst = pidl;

                bool isLink =
                    SUCCEEDED(desktopFolder->GetAttributesOf(
                        1,
                        &pidlConst,
                        &attributes)) &&
                    (attributes & SFGAO_LINK);

                if (isLink) {
                    std::wstring displayName;
                    std::wstring shortcutPath;

                    if (ShellDisplayNameToString(
                            desktopFolder,
                            pidl,
                            SHGDN_NORMAL,
                            &displayName) &&
                        ShellDisplayNameToString(
                            desktopFolder,
                            pidl,
                            SHGDN_FORPARSING,
                            &shortcutPath)) {

                        std::wstring targetPath;
                        std::wstring iconPath;
                        int iconIndex = 0;

                        if (ReadShortcutIconInfo(
                                shortcutPath,
                                &targetPath,
                                &iconPath,
                                &iconIndex) &&
                            TargetUsesRunAsAdminLayer(targetPath)) {

                            Wh_Log(
                                L"RUNASADMIN desktop shortcut: "
                                L"item=\"%ls\" target=\"%ls\" "
                                L"icon=\"%ls\" index=%d",
                                displayName.c_str(),
                                targetPath.c_str(),
                                iconPath.c_str(),
                                iconIndex);

                            HICON rawIcon = ExtractRawShortcutIcon(
                                iconPath,
                                iconIndex,
                                targetPath,
                                iconWidth,
                                iconHeight);

                            if (rawIcon) {
                                int cleanIndex =
                                    ImageList_AddIcon(
                                        freshImageList,
                                        rawIcon);
                                DestroyIcon(rawIcon);

                                if (cleanIndex >= 0) {
                                    cleanIndexByName[displayName] =
                                        cleanIndex;
                                }
                            } else {
                                Wh_Log(
                                    L"Couldn't extract the raw icon for "
                                    L"item=\"%ls\"",
                                    displayName.c_str());
                            }
                        }
                    }
                }

                CoTaskMemFree(pidl);
                pidl = nullptr;
            }

            enumerator->Release();
        }

        desktopFolder->Release();
    }

    std::unordered_map<int, int> freshBySystemImage;
    std::vector<int> freshSignature;
    int itemCount = ListView_GetItemCount(desktopView);
    freshSignature.reserve(itemCount > 0 ? itemCount : 0);

    for (int itemIndex = 0; itemIndex < itemCount; itemIndex++) {
        WCHAR itemName[MAX_PATH] = {};
        LVITEMW textItem = {};
        textItem.iSubItem = 0;
        textItem.pszText = itemName;
        textItem.cchTextMax = ARRAYSIZE(itemName);

        SendMessageW(
            desktopView,
            LVM_GETITEMTEXTW,
            itemIndex,
            reinterpret_cast<LPARAM>(&textItem));

        LVITEMW item = {};
        item.mask = LVIF_IMAGE;
        item.iItem = itemIndex;
        item.iSubItem = 0;

        if (!SendMessageW(
                desktopView,
                LVM_GETITEMW,
                0,
                reinterpret_cast<LPARAM>(&item))) {
            item.iImage = -1;
        }

        freshSignature.push_back(item.iImage);

        auto cleanIt = cleanIndexByName.find(itemName);
        if (cleanIt == cleanIndexByName.end()) {
            continue;
        }

        if (item.iImage >= 0) {
            freshBySystemImage[item.iImage] = cleanIt->second;

            Wh_Log(
                L"Mapped desktop system image=%d to raw image=%d "
                L"for item=\"%ls\"",
                item.iImage,
                cleanIt->second,
                itemName);
        }
    }

    AcquireSRWLockExclusive(&g_cleanShieldCacheLock);

    if (InterlockedCompareExchange(&g_isUnloading, 0, 0) != 0 ||
        cacheGeneration != InterlockedCompareExchange(
            &g_shieldCacheGeneration,
            0,
            0)) {
        ReleaseSRWLockExclusive(&g_cleanShieldCacheLock);
        ImageList_Destroy(freshImageList);
        return ShieldRebuildResult::Skipped;
    }

    // Glows built from the old private image list must not be reused if a
    // new list happens to get the same handle.
    ClearGlowCache();

    HIMAGELIST oldImageList = g_cleanShieldImageList;
    g_cleanShieldImageList = freshImageList;
    g_cleanIconBySystemImage = std::move(freshBySystemImage);
    g_cachedDesktopSystemImageList = systemImageList;
    g_cachedDesktopIconWidth = iconWidth;
    g_cachedDesktopIconHeight = iconHeight;
    g_cachedDesktopItemCount = itemCount;
    g_cachedDesktopImageSignature = std::move(freshSignature);
    UINT replacementCount =
        static_cast<UINT>(g_cleanIconBySystemImage.size());

    ReleaseSRWLockExclusive(&g_cleanShieldCacheLock);

    if (oldImageList) {
        ImageList_Destroy(oldImageList);
    }

    Wh_Log(
        L"Raw shield-icon cache ready: replacements=%u size=%dx%d",
        replacementCount,
        iconWidth,
        iconHeight);

    return ShieldRebuildResult::Rebuilt;
}

using IImageList_Draw_t =
    HRESULT(STDMETHODCALLTYPE*)(IImageList*, IMAGELISTDRAWPARAMS*);

IImageList_Draw_t IImageList_Draw_Original = nullptr;

// ---------------------------------------------------------------------------
// Icon-shape glow (replaces the hover/selection box)
//
// The glow is built from the icon's own alpha channel: dilated by a pixel,
// blurred, and filled with white. It's drawn underneath the icon, so only the
// part outside the icon's shape shows. Each glow is built once per
// (image list, image, size) and cached.
// ---------------------------------------------------------------------------

// Subtle look. Hovered icons use kGlowHoverAlpha, selected ones the brighter
// kGlowSelectedAlpha.
constexpr BYTE kGlowHoverAlpha = 85;
constexpr BYTE kGlowSelectedAlpha = 225;
constexpr float kGlowGain = 1.6f;
constexpr size_t kGlowCacheMaxEntries = 512;

// Glows are cached by the icon's actual pixels, not by its image-list index:
// Explorer reuses system-image-list slots for different icons (e.g. while
// dragging), so an index-based cache could draw another icon's glow.
struct GlowKey {
    unsigned long long pixelHash;
    int width;
    int height;

    bool operator==(const GlowKey& other) const {
        return pixelHash == other.pixelHash && width == other.width &&
               height == other.height;
    }
};

struct GlowKeyHash {
    size_t operator()(const GlowKey& key) const {
        return static_cast<size_t>(key.pixelHash ^
                                   (static_cast<unsigned long long>(
                                        key.width) << 32) ^
                                   static_cast<unsigned long long>(key.height));
    }
};

struct GlowBitmap {
    HBITMAP bitmap = nullptr;  // 32bpp premultiplied, (w + 2*pad) square-ish
    int width = 0;
    int height = 0;
    int pad = 0;
};

SRWLOCK g_glowCacheLock = SRWLOCK_INIT;
std::unordered_map<GlowKey, GlowBitmap, GlowKeyHash> g_glowCache;

static void ClearGlowCache() {
    AcquireSRWLockExclusive(&g_glowCacheLock);
    for (auto& entry : g_glowCache) {
        if (entry.second.bitmap) {
            DeleteObject(entry.second.bitmap);
        }
    }
    g_glowCache.clear();
    ReleaseSRWLockExclusive(&g_glowCacheLock);
}

static int GlowPadForIcon(int iconSize) {
    int pad = iconSize / 10;
    return pad < 3 ? 3 : (pad > 12 ? 12 : pad);
}

static void BoxBlurPass(std::vector<float>& data,
                        std::vector<float>& scratch,
                        int width,
                        int height,
                        int radius,
                        bool horizontal) {
    int length = horizontal ? width : height;
    int lines = horizontal ? height : width;
    float scale = 1.0f / static_cast<float>(radius * 2 + 1);

    for (int line = 0; line < lines; line++) {
        auto at = [&](int i) -> float& {
            return horizontal ? data[line * width + i]
                              : data[i * width + line];
        };

        float sum = 0.0f;
        for (int i = -radius; i <= radius; i++) {
            if (i >= 0 && i < length) {
                sum += at(i);
            }
        }

        for (int i = 0; i < length; i++) {
            scratch[i] = sum * scale;

            int outIndex = i - radius;
            int inIndex = i + radius + 1;
            if (outIndex >= 0) {
                sum -= at(outIndex);
            }
            if (inIndex < length) {
                sum += at(inIndex);
            }
        }

        for (int i = 0; i < length; i++) {
            at(i) = scratch[i];
        }
    }
}

// Render an image-list icon (with its alpha) into a BGRA pixel buffer.
static bool RenderIconPixels(HIMAGELIST himl,
                             int index,
                             int iconWidth,
                             int iconHeight,
                             std::vector<BYTE>* pixels) {
    if (iconWidth <= 0 || iconHeight <= 0) {
        return false;
    }

    BITMAPINFO iconInfo = {};
    iconInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    iconInfo.bmiHeader.biWidth = iconWidth;
    iconInfo.bmiHeader.biHeight = -iconHeight;
    iconInfo.bmiHeader.biPlanes = 1;
    iconInfo.bmiHeader.biBitCount = 32;
    iconInfo.bmiHeader.biCompression = BI_RGB;

    HDC memDC = CreateCompatibleDC(nullptr);
    if (!memDC) {
        return false;
    }

    void* iconBits = nullptr;
    HBITMAP iconDib = CreateDIBSection(
        memDC, &iconInfo, DIB_RGB_COLORS, &iconBits, nullptr, 0);
    if (!iconDib || !iconBits) {
        if (iconDib) {
            DeleteObject(iconDib);
        }
        DeleteDC(memDC);
        return false;
    }

    size_t byteCount = static_cast<size_t>(iconWidth) * iconHeight * 4;

    HGDIOBJ oldBitmap = SelectObject(memDC, iconDib);
    ZeroMemory(iconBits, byteCount);

    IMAGELISTDRAWPARAMS params = {};
    params.cbSize = sizeof(params);
    params.himl = himl;
    params.i = index;
    params.hdcDst = memDC;
    params.rgbBk = CLR_NONE;
    params.rgbFg = CLR_NONE;
    params.fStyle = ILD_NORMAL | ILD_PRESERVEALPHA;

    g_isGeneratingGlow = true;
    BOOL drawn = ImageList_DrawIndirect(&params);
    g_isGeneratingGlow = false;

    GdiFlush();
    SelectObject(memDC, oldBitmap);

    if (drawn) {
        const BYTE* src = static_cast<const BYTE*>(iconBits);
        pixels->assign(src, src + byteCount);
    }

    DeleteObject(iconDib);
    DeleteDC(memDC);

    return drawn != FALSE;
}

static unsigned long long HashPixels(const std::vector<BYTE>& pixels) {
    // FNV-1a, 64-bit.
    unsigned long long hash = 1469598103934665603ULL;
    for (BYTE b : pixels) {
        hash ^= b;
        hash *= 1099511628211ULL;
    }
    return hash;
}

static GlowBitmap CreateGlowBitmap(const std::vector<BYTE>& iconPixels,
                                   int iconWidth,
                                   int iconHeight) {
    GlowBitmap result;

    if (iconWidth <= 0 || iconHeight <= 0 ||
        iconPixels.size() <
            static_cast<size_t>(iconWidth) * iconHeight * 4) {
        return result;
    }

    const BYTE* src = iconPixels.data();
    // 2. Copy the alpha channel into a padded float buffer.
    int pad = GlowPadForIcon(iconWidth > iconHeight ? iconWidth : iconHeight);
    int width = iconWidth + pad * 2;
    int height = iconHeight + pad * 2;

    std::vector<float> alpha(static_cast<size_t>(width) * height, 0.0f);
    bool anyAlpha = false;

    for (int y = 0; y < iconHeight; y++) {
        for (int x = 0; x < iconWidth; x++) {
            BYTE a = src[(y * iconWidth + x) * 4 + 3];
            if (a) {
                anyAlpha = true;
            }
            alpha[(y + pad) * width + (x + pad)] = a / 255.0f;
        }
    }

    // Icons without an alpha channel: fall back to "any non-black pixel".
    if (!anyAlpha) {
        for (int y = 0; y < iconHeight; y++) {
            for (int x = 0; x < iconWidth; x++) {
                const BYTE* px = &src[(y * iconWidth + x) * 4];
                alpha[(y + pad) * width + (x + pad)] =
                    (px[0] | px[1] | px[2]) ? 1.0f : 0.0f;
            }
        }
    }

    // 3. Dilate by one pixel so the glow hugs the outline, then blur.
    std::vector<float> dilated(alpha.size(), 0.0f);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float m = 0.0f;
            for (int dy = -1; dy <= 1; dy++) {
                int yy = y + dy;
                if (yy < 0 || yy >= height) {
                    continue;
                }
                for (int dx = -1; dx <= 1; dx++) {
                    int xx = x + dx;
                    if (xx < 0 || xx >= width) {
                        continue;
                    }
                    float v = alpha[yy * width + xx];
                    if (v > m) {
                        m = v;
                    }
                }
            }
            dilated[y * width + x] = m;
        }
    }

    std::vector<float> scratch(width > height ? width : height);
    int radius = pad / 2 > 0 ? pad / 2 : 1;
    for (int pass = 0; pass < 3; pass++) {
        BoxBlurPass(dilated, scratch, width, height, radius, true);
        BoxBlurPass(dilated, scratch, width, height, radius, false);
    }

    // 4. Write premultiplied white.
    BITMAPINFO glowInfo = {};
    glowInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    glowInfo.bmiHeader.biWidth = width;
    glowInfo.bmiHeader.biHeight = -height;
    glowInfo.bmiHeader.biPlanes = 1;
    glowInfo.bmiHeader.biBitCount = 32;
    glowInfo.bmiHeader.biCompression = BI_RGB;

    void* glowBits = nullptr;
    HBITMAP glowDib = CreateDIBSection(
        nullptr, &glowInfo, DIB_RGB_COLORS, &glowBits, nullptr, 0);

    if (!glowDib || !glowBits) {
        if (glowDib) {
            DeleteObject(glowDib);
        }
        return result;
    }

    BYTE* dst = static_cast<BYTE*>(glowBits);
    for (size_t i = 0; i < dilated.size(); i++) {
        float v = dilated[i] * kGlowGain;
        if (v > 1.0f) {
            v = 1.0f;
        }
        BYTE a = static_cast<BYTE>(v * 255.0f + 0.5f);
        dst[i * 4 + 0] = a;
        dst[i * 4 + 1] = a;
        dst[i * 4 + 2] = a;
        dst[i * 4 + 3] = a;
    }

    result.bitmap = glowDib;
    result.width = width;
    result.height = height;
    result.pad = pad;
    return result;
}

static bool GetGlowBitmap(HIMAGELIST himl,
                          int index,
                          int iconWidth,
                          int iconHeight,
                          GlowBitmap* out) {
    // Rendering a single icon is cheap, and only hovered/selected icons get
    // here, so render it every time and look the glow up by its pixels.
    std::vector<BYTE> pixels;
    if (!RenderIconPixels(himl, index, iconWidth, iconHeight, &pixels)) {
        return false;
    }

    GlowKey key = {HashPixels(pixels), iconWidth, iconHeight};

    AcquireSRWLockShared(&g_glowCacheLock);
    auto it = g_glowCache.find(key);
    if (it != g_glowCache.end()) {
        *out = it->second;
        ReleaseSRWLockShared(&g_glowCacheLock);
        return out->bitmap != nullptr;
    }
    ReleaseSRWLockShared(&g_glowCacheLock);

    GlowBitmap glow = CreateGlowBitmap(pixels, iconWidth, iconHeight);

    AcquireSRWLockExclusive(&g_glowCacheLock);
    if (g_glowCache.size() >= kGlowCacheMaxEntries) {
        for (auto& entry : g_glowCache) {
            if (entry.second.bitmap) {
                DeleteObject(entry.second.bitmap);
            }
        }
        g_glowCache.clear();
    }
    auto inserted = g_glowCache.emplace(key, glow);
    if (!inserted.second && glow.bitmap) {
        DeleteObject(glow.bitmap);
    }
    *out = inserted.first->second;
    ReleaseSRWLockExclusive(&g_glowCacheLock);

    return out->bitmap != nullptr;
}

enum class DesktopItemDrawState { None, Hot, Selected };

// Work out which desktop item an icon draw at (x, y, cx, cy) belongs to, and
// whether that item is selected or hovered.
static DesktopItemDrawState GetDesktopItemDrawState(HWND listView,
                                                    int x,
                                                    int y,
                                                    int cx,
                                                    int cy) {
    LVHITTESTINFO hit = {};
    hit.pt.x = x + cx / 2;
    hit.pt.y = y + cy / 2;

    int item = static_cast<int>(SendMessageW(
        listView, LVM_HITTEST, 0, reinterpret_cast<LPARAM>(&hit)));
    if (item < 0 || !(hit.flags & LVHT_ONITEM)) {
        return DesktopItemDrawState::None;
    }

    // Drop targets (while dragging icons onto another icon) are shown like
    // selected items, since their box is removed too.
    if (ListView_GetItemState(listView, item,
                              LVIS_SELECTED | LVIS_DROPHILITED) &
        (LVIS_SELECTED | LVIS_DROPHILITED)) {
        return DesktopItemDrawState::Selected;
    }

    if (ListView_GetHotItem(listView) == item) {
        return DesktopItemDrawState::Hot;
    }

    return DesktopItemDrawState::None;
}

static void DrawIconGlow(HDC hdc,
                         HIMAGELIST himl,
                         int index,
                         int x,
                         int y,
                         int drawWidth,
                         int drawHeight,
                         BYTE strength) {
    int iconWidth = 0;
    int iconHeight = 0;
    if (!ImageList_GetIconSize(himl, &iconWidth, &iconHeight) ||
        iconWidth <= 0 || iconHeight <= 0) {
        return;
    }

    GlowBitmap glow;
    if (!GetGlowBitmap(himl, index, iconWidth, iconHeight, &glow)) {
        return;
    }

    // Scale the glow if the icon is being drawn at a different size.
    if (drawWidth <= 0) {
        drawWidth = iconWidth;
    }
    if (drawHeight <= 0) {
        drawHeight = iconHeight;
    }

    int padX = MulDiv(glow.pad, drawWidth, iconWidth);
    int padY = MulDiv(glow.pad, drawHeight, iconHeight);

    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) {
        return;
    }

    HGDIOBJ oldBitmap = SelectObject(memDC, glow.bitmap);

    BLENDFUNCTION blend = {AC_SRC_OVER, 0, strength, AC_SRC_ALPHA};
    GdiAlphaBlend(hdc,
                  x - padX,
                  y - padY,
                  drawWidth + padX * 2,
                  drawHeight + padY * 2,
                  memDC,
                  0,
                  0,
                  glow.width,
                  glow.height,
                  blend);

    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
}

// ---------------------------------------------------------------------------
// IImageList::Draw hook (arrows, admin shields, icon glow)
// ---------------------------------------------------------------------------

HRESULT STDMETHODCALLTYPE IImageList_Draw_Hook(
    IImageList* self,
    IMAGELISTDRAWPARAMS* drawParams) {

    if (!g_isDrawingDesktop || !drawParams || g_isGeneratingGlow ||
        g_isDrawingCleanShieldIcon) {
        return IImageList_Draw_Original(self, drawParams);
    }

    // Work on a local copy so the caller's parameters are never modified.
    // Callers may pass an older, smaller IMAGELISTDRAWPARAMS, so copy only
    // what they provided.
    if (drawParams->cbSize < IMAGELISTDRAWPARAMS_V3_SIZE) {
        return IImageList_Draw_Original(self, drawParams);
    }

    IMAGELISTDRAWPARAMS local = {};
    memcpy(&local,
           drawParams,
           drawParams->cbSize < sizeof(local) ? drawParams->cbSize
                                              : sizeof(local));

    HIMAGELIST drawImageList =
        local.himl ? local.himl : reinterpret_cast<HIMAGELIST>(self);

    HIMAGELIST desktopImageList =
        g_drawingDesktopHwnd
            ? ListView_GetImageList(g_drawingDesktopHwnd, LVSIL_NORMAL)
            : nullptr;

    bool isDesktopIcon =
        desktopImageList && drawImageList == desktopImageList;

    if (g_settings.icon_glow && isDesktopIcon) {
        // Selected icons can be drawn with a blue blend tint; that's part of
        // the selection box look, so drop it.
        local.fStyle &= ~(ILD_BLEND25 | ILD_BLEND50);
    }

    // Admin-shield replacement: the image list/index actually drawn.
    HIMAGELIST glowImageList = drawImageList;
    int glowIndex = local.i;
    bool useCleanIcon = false;

    if (g_settings.hide_shields) {
        AcquireSRWLockShared(&g_cleanShieldCacheLock);

        auto cleanIt = g_cleanIconBySystemImage.find(local.i);

        bool isDesktopSystemImageList =
            self == reinterpret_cast<IImageList*>(
                        g_cachedDesktopSystemImageList) ||
            local.himl == g_cachedDesktopSystemImageList;

        if (g_cleanShieldImageList &&
            isDesktopSystemImageList &&
            cleanIt != g_cleanIconBySystemImage.end()) {
            useCleanIcon = true;
            glowImageList = g_cleanShieldImageList;
            glowIndex = cleanIt->second;
        }

        // Keep the lock until the clean icon has been drawn below.
        if (!useCleanIcon) {
            ReleaseSRWLockShared(&g_cleanShieldCacheLock);
        }
    }

    if (g_settings.icon_glow && isDesktopIcon && g_drawingDesktopHwnd) {
        int iconWidth = 0;
        int iconHeight = 0;
        ImageList_GetIconSize(drawImageList, &iconWidth, &iconHeight);

        int cx = local.cx > 0 ? local.cx : iconWidth;
        int cy = local.cy > 0 ? local.cy : iconHeight;

        DesktopItemDrawState state = GetDesktopItemDrawState(
            g_drawingDesktopHwnd, local.x, local.y, cx, cy);

        if (state != DesktopItemDrawState::None) {
            DrawIconGlow(local.hdcDst,
                         glowImageList,
                         glowIndex,
                         local.x,
                         local.y,
                         local.cx,
                         local.cy,
                         state == DesktopItemDrawState::Selected
                             ? kGlowSelectedAlpha
                             : kGlowHoverAlpha);
        }
    }

    if (g_settings.hide_arrows &&
        g_linkOverlayMask != 0 &&
        (local.fStyle & ILD_OVERLAYMASK) == g_linkOverlayMask) {
        local.fStyle &= ~ILD_OVERLAYMASK;
    }

    if (useCleanIcon) {
        IMAGELISTDRAWPARAMS copy = local;
        copy.himl = g_cleanShieldImageList;
        copy.i = glowIndex;

        // ImageList_DrawIndirect dispatches to the implementation that owns
        // the private image list. Calling the system-image-list trampoline
        // with this private object would be invalid.
        g_isDrawingCleanShieldIcon = true;
        BOOL drawn = ImageList_DrawIndirect(&copy);
        g_isDrawingCleanShieldIcon = false;

        ReleaseSRWLockShared(&g_cleanShieldCacheLock);
        return drawn ? S_OK : E_FAIL;
    }

    return IImageList_Draw_Original(self, &local);
}

static IImageList_Draw_t GetDesktopImageListDrawTarget() {
    if (HWND desktop = GetDesktopFolderView()) {
        HIMAGELIST imageListHandle =
            ListView_GetImageList(desktop, LVSIL_NORMAL);

        if (imageListHandle) {
            auto imageList =
                reinterpret_cast<IImageList*>(imageListHandle);

            void** vtable =
                *reinterpret_cast<void***>(imageList);

            if (vtable && vtable[8]) {
                Wh_Log(
                    L"Using the desktop ListView's IImageList::Draw target");
                return reinterpret_cast<IImageList_Draw_t>(vtable[8]);
            }
        }
    }

    HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
    if (!shell32) {
        return nullptr;
    }

    using SHGetImageList_t =
        HRESULT(WINAPI*)(int, REFIID, void**);

    auto getImageList =
        reinterpret_cast<SHGetImageList_t>(
            GetProcAddress(shell32, "SHGetImageList"));

    if (!getImageList) {
        return nullptr;
    }

    static const GUID iidImageList = {
        0x46eb5926,
        0x582e,
        0x4017,
        {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}
    };

    IImageList* imageList = nullptr;

    if (FAILED(getImageList(
            0,
            iidImageList,
            reinterpret_cast<void**>(&imageList))) ||
        !imageList) {
        return nullptr;
    }

    void** vtable = *reinterpret_cast<void***>(imageList);
    IImageList_Draw_t target = nullptr;

    if (vtable && vtable[8]) {
        target = reinterpret_cast<IImageList_Draw_t>(vtable[8]);
    }

    imageList->Release();

    if (target) {
        Wh_Log(L"Using SHGetImageList IImageList::Draw fallback target");
    }

    return target;
}

// ---------------------------------------------------------------------------
// Desktop subclass
// ---------------------------------------------------------------------------

LRESULT CALLBACK DesktopListSubclass(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR) {

    if ((uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT) &&
        (g_settings.hide_text ||
         g_settings.hide_all_text ||
         g_settings.hide_arrows ||
         g_settings.hide_shields ||
         g_settings.icon_glow)) {

        if (g_settings.hide_text && !g_settings.hide_all_text) {
            UpdateShortcutCache();
        }

        if (g_settings.hide_shields &&
            ShieldIconCacheNeedsRefresh(hWnd)) {
            // Check again shortly if the rebuild ran or was throttled:
            // Explorer may still be swapping placeholder icons for real ones,
            // and without the check a stale map would only be fixed by the
            // next repaint (e.g. hovering an icon). After a failure, retrying
            // right away won't help, so wait for the next paint instead.
            if (UpdateShieldedShortcutIconCache(hWnd) !=
                ShieldRebuildResult::Failed) {
                SetTimer(hWnd, kShieldRefreshTimerId, kShieldRefreshRetryMs,
                         nullptr);
            }
        }

        bool previous = g_isDrawingDesktop;
        HWND previousHwnd = g_drawingDesktopHwnd;
        g_isDrawingDesktop = true;
        g_drawingDesktopHwnd = hWnd;

        LRESULT result =
            DefSubclassProc(hWnd, uMsg, wParam, lParam);

        g_isDrawingDesktop = previous;
        g_drawingDesktopHwnd = previousHwnd;
        return result;
    }

    if (uMsg == WM_TIMER && wParam == kShieldRefreshTimerId) {
        KillTimer(hWnd, kShieldRefreshTimerId);

        if (g_settings.hide_shields &&
            InterlockedCompareExchange(&g_isUnloading, 0, 0) == 0 &&
            ShieldIconCacheNeedsRefresh(hWnd)) {
            switch (UpdateShieldedShortcutIconCache(hWnd)) {
                case ShieldRebuildResult::Rebuilt:
                    InvalidateRect(hWnd, nullptr, FALSE);
                    break;
                case ShieldRebuildResult::Skipped:
                    SetTimer(hWnd, kShieldRefreshTimerId,
                             kShieldRefreshRetryMs, nullptr);
                    break;
                case ShieldRebuildResult::Failed:
                    break;
            }
        }

        return 0;
    }

    if (g_uninitMessage && uMsg == g_uninitMessage) {
        // Runs on the desktop thread, between paints, so nothing is using the
        // caches while they're freed.
        KillTimer(hWnd, kShieldRefreshTimerId);
        ClearShieldedShortcutIconCache();
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        KillTimer(hWnd, kShieldRefreshTimerId);

        HWND expected = hWnd;
        g_desktopListView.compare_exchange_strong(expected, nullptr);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Text drawing hooks
// ---------------------------------------------------------------------------

using DrawTextW_t = decltype(&DrawTextW);
DrawTextW_t DrawTextW_Original = nullptr;

int WINAPI DrawTextW_Hook(
    HDC hdc,
    LPCWSTR text,
    int cchText,
    LPRECT rect,
    UINT format) {

    if (g_isDrawingDesktop &&
        text &&
        ShouldHideLabel(text, cchText)) {
        return DrawTextW_Original(hdc, L"", 0, rect, format);
    }

    return DrawTextW_Original(hdc, text, cchText, rect, format);
}

using DrawThemeTextEx_t = HRESULT(WINAPI*)(
    HTHEME,
    HDC,
    int,
    int,
    LPCWSTR,
    int,
    DWORD,
    LPRECT,
    const void*);

DrawThemeTextEx_t DrawThemeTextEx_Original = nullptr;

HRESULT WINAPI DrawThemeTextEx_Hook(
    HTHEME theme,
    HDC hdc,
    int partId,
    int stateId,
    LPCWSTR text,
    int cchText,
    DWORD flags,
    LPRECT rect,
    const void* options) {

    if (g_isDrawingDesktop &&
        text &&
        ShouldHideLabel(text, cchText)) {
        return DrawThemeTextEx_Original(
            theme,
            hdc,
            partId,
            stateId,
            L"",
            0,
            flags,
            rect,
            options);
    }

    return DrawThemeTextEx_Original(
        theme,
        hdc,
        partId,
        stateId,
        text,
        cchText,
        flags,
        rect,
        options);
}

// ---------------------------------------------------------------------------
// Hover/selection box removal
//
// The white box behind hovered/selected desktop icons is the ListView theme's
// LVP_LISTITEM part. Skip it while the desktop is painting; File Explorer
// windows are never affected.
// ---------------------------------------------------------------------------

static bool ShouldSkipThemeBackground(int partId, int stateId) {
    if (!g_isDrawingDesktop || !g_settings.icon_glow ||
        partId != LVP_LISTITEM) {
        return false;
    }

    return stateId == LISS_HOT || stateId == LISS_SELECTED ||
           stateId == LISS_SELECTEDNOTFOCUS || stateId == LISS_HOTSELECTED;
}

using DrawThemeBackground_t =
    HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCRECT, LPCRECT);
DrawThemeBackground_t DrawThemeBackground_Original = nullptr;

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME theme,
                                        HDC hdc,
                                        int partId,
                                        int stateId,
                                        LPCRECT rect,
                                        LPCRECT clipRect) {
    if (ShouldSkipThemeBackground(partId, stateId)) {
        return S_OK;
    }

    return DrawThemeBackground_Original(
        theme, hdc, partId, stateId, rect, clipRect);
}

using DrawThemeBackgroundEx_t =
    HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCRECT, const void*);
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original = nullptr;

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME theme,
                                          HDC hdc,
                                          int partId,
                                          int stateId,
                                          LPCRECT rect,
                                          const void* options) {
    if (ShouldSkipThemeBackground(partId, stateId)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Original(
        theme, hdc, partId, stateId, rect, options);
}

// ---------------------------------------------------------------------------
// Catch desktop ListView recreation
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    PVOID param) {

    HWND hWnd = CreateWindowExW_Original(
        exStyle,
        className,
        windowName,
        style,
        x,
        y,
        width,
        height,
        parent,
        menu,
        instance,
        param);

    if (hWnd && IsDesktopFolderView(hWnd)) {
        ClearShieldedShortcutIconCache();

        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWnd,
                DesktopListSubclass,
                0)) {
            g_desktopListView = hWnd;
        }
    }

    return hWnd;
}

// ---------------------------------------------------------------------------
// Settings / lifecycle
// ---------------------------------------------------------------------------

static void LoadSettings() {
    g_settings.hide_text =
        Wh_GetIntSetting(L"hide_text") != 0;

    g_settings.hide_all_text =
        Wh_GetIntSetting(L"hide_all_text") != 0;

    g_settings.hide_arrows =
        Wh_GetIntSetting(L"hide_arrows") != 0;

    g_settings.hide_shields =
        Wh_GetIntSetting(L"hide_admin_shields") != 0;

    g_settings.icon_glow =
        Wh_GetIntSetting(L"icon_glow") != 0;
}

BOOL Wh_ModInit() {
    InterlockedExchange(&g_isUnloading, 0);
    LoadSettings();

    g_uninitMessage =
        RegisterWindowMessageW(L"DesktopShortcutCleaner_Uninit");

    WindhawkUtils::SetFunctionHook(
        CreateWindowExW,
        CreateWindowExW_Hook,
        &CreateWindowExW_Original);

    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        if (auto proc = reinterpret_cast<DrawTextW_t>(
                GetProcAddress(user32, "DrawTextW"))) {
            WindhawkUtils::SetFunctionHook(
                proc,
                DrawTextW_Hook,
                &DrawTextW_Original);
        }
    }

    if (HMODULE uxTheme = GetModuleHandleW(L"uxtheme.dll")) {
        if (auto proc = reinterpret_cast<DrawThemeTextEx_t>(
                GetProcAddress(uxTheme, "DrawThemeTextEx"))) {
            WindhawkUtils::SetFunctionHook(
                proc,
                DrawThemeTextEx_Hook,
                &DrawThemeTextEx_Original);
        }
    }

    if (g_settings.icon_glow) {
        if (HMODULE uxTheme = GetModuleHandleW(L"uxtheme.dll")) {
            if (auto proc = reinterpret_cast<DrawThemeBackground_t>(
                    GetProcAddress(uxTheme, "DrawThemeBackground"))) {
                WindhawkUtils::SetFunctionHook(
                    proc,
                    DrawThemeBackground_Hook,
                    &DrawThemeBackground_Original);
            }

            if (auto proc = reinterpret_cast<DrawThemeBackgroundEx_t>(
                    GetProcAddress(uxTheme, "DrawThemeBackgroundEx"))) {
                WindhawkUtils::SetFunctionHook(
                    proc,
                    DrawThemeBackgroundEx_Hook,
                    &DrawThemeBackgroundEx_Original);
            }
        }
    }

    if (g_settings.hide_arrows ||
        g_settings.hide_shields) {
        ResolveLinkOverlayMask();
    }

    if (g_settings.hide_arrows ||
        g_settings.hide_shields ||
        g_settings.icon_glow) {
        if (IImageList_Draw_t drawTarget =
                GetDesktopImageListDrawTarget()) {
            WindhawkUtils::SetFunctionHook(
                drawTarget,
                IImageList_Draw_Hook,
                &IImageList_Draw_Original);
        } else {
            Wh_Log(L"Couldn't resolve IImageList::Draw target");
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    HWND desktop = GetDesktopFolderView();
    if (!desktop) {
        Wh_Log(L"Desktop ListView not found yet");
        return;
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            desktop,
            DesktopListSubclass,
            0)) {
        g_desktopListView = desktop;
    }

    // The repaint builds the shield map on the desktop thread.
    InvalidateRect(desktop, nullptr, TRUE);
    UpdateWindow(desktop);
}

void Wh_ModUninit() {
    InterlockedExchange(&g_isUnloading, 1);

    HWND desktop = g_desktopListView.exchange(nullptr);
    if (!desktop) {
        return;
    }

    // Kill the retry timer and free the caches on the desktop thread, then
    // remove the subclass from the exact window that was subclassed.
    DWORD_PTR result = 0;
    SendMessageTimeoutW(desktop, g_uninitMessage, 0, 0,
                        SMTO_NORMAL | SMTO_ABORTIFHUNG, 2000, &result);

    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        desktop,
        DesktopListSubclass);

    InvalidateRect(desktop, nullptr, TRUE);
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
