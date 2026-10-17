// ==WindhawkMod==
// @id              explorer-real-folder-paths
// @name            Explorer real folder paths
// @description     Open filesystem-backed Shell shortcuts through their actual paths
// @version         0.1.5
// @author          Nerdworld
// @github          https://github.com/nerdworldDE
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# Explorer real folder paths

When a sidebar shortcut such as Downloads opens a Shell alias, browse to the
same folder through its filesystem path. Explorer can then show the physical
folder hierarchy and an editable address such as `C:\Users\Daniel\Downloads`.
Press **Ctrl+L** or **Alt+D** to edit the address.

![Downloads address bar before and after enabling the mod](https://i.imgur.com/Dc6GdsL.png)

Windows still renders breadcrumbs when the address bar is not being edited.
The mod does not force keyboard focus into the address bar after navigation.
When the sidebar already has the converted folder selected, keep that row
and its scroll position instead of selecting the folder's physical drive.

Paths are resolved through the Windows Shell. Moved folders, OneDrive folder
redirection, localized names, and network paths are not hardcoded.

Dropbox and similar filesystem folders can keep a friendly name even when
opened through their physical path. The mod also supplies the full path to
Explorer's editable address for these folders, keeping their existing
navigation route and sidebar selection.
In the modern Windows 11 address bar, **Copy address as text** also copies
the filesystem path. This applies to any filesystem folder with a friendly
editing name, including folders opened through Home, This PC, or a library.

Virtual locations such as Home, This PC, library roots, searches, and ZIP
views keep their normal behavior. Filesystem folders reached through a
library, OneDrive, or another Shell alias can open through their physical
paths. The merged Shell Desktop is left alone; it is not the same view as
the user's physical Desktop directory.

Only Explorer (`explorer.exe`) windows are affected. Open/Save dialogs hosted
by other applications are unaffected.

## Usage and compatibility

Open a new Explorer window, click Downloads in the sidebar, and press Ctrl+L.
Navigate away and back when testing an already-open window.

Tested on 64-bit Windows 11 23H2, build 22631.6199: version 0.1.5 shows
Dropbox's filesystem path with Ctrl+L and Copy address as text, while
preserving sidebar selection and scrolling. Downloads opened through Home
was also tested with both actions. The original known-folder navigation and
sidebar behavior was tested on this build. Other Windows builds and Windows
on ARM have not been validated.
The address-text fallback applies to the modern Windows 11 address bar.
Classic address bars, including those restored by other mods, are not
covered by this fallback.
The mod needs ExplorerFrame's navigation and sidebar selection symbols. If
Windhawk cannot resolve them, the mod refuses to initialize.
The address-text fallback uses an additional optional Shell symbol. If it is
unavailable, the existing navigation behavior remains active, and logging
reports that the fallback could not be enabled.

Sidebar selection follows Windows' normal behavior when navigating to a
different folder through history or the address bar. The mod preserves a
shortcut that already represents the opened folder.

Enable logging for this mod in Windhawk to see navigation flags, resolution
results, and paths. Disable logging afterwards.

Disabling the mod stops future conversions. Entries already stored in an
Explorer window's navigation history can still point to the physical path.
No folder, pin, or registry setting is changed.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <shlobj.h>
#include <shobjidl.h>

#include <array>
#include <cstring>
#include <memory>
#include <utility>

namespace {

using BrowseObject_t = HRESULT(WINAPI*)(void*, PCUIDLIST_RELATIVE, UINT);
BrowseObject_t g_originalBrowseObject = nullptr;

using SetSelectedItem_t = HRESULT(WINAPI*)(void*, IShellItem*);
SetSelectedItem_t g_originalSetSelectedItem = nullptr;
SetSelectedItem_t g_originalSetSelectedItemNoExpand = nullptr;

using QueryInterface_t = HRESULT(WINAPI*)(void*, REFIID, void**);
QueryInterface_t g_treeQueryInterface = nullptr;

using ShellItemGetDisplayName_t = HRESULT(WINAPI*)(IShellItem*, SIGDN, PWSTR*);
ShellItemGetDisplayName_t g_originalShellItemGetDisplayName = nullptr;
HMODULE g_windowsStorage = nullptr;
thread_local bool g_insideAddressTextResolution = false;

HMODULE g_explorerFrame = nullptr;
// These guards are defensive: Shell calls can cross COM/provider boundaries
// and pump messages. Reentry wasn't observed in the tested Windows build.
thread_local bool g_insideNormalization = false;
thread_local bool g_insideSelectionCheck = false;

struct CoTaskMemDeleter {
    void operator()(void* value) const {
        CoTaskMemFree(value);
    }
};

template <typename T>
using ShellAllocation = std::unique_ptr<T, CoTaskMemDeleter>;

template <typename T>
struct ComDeleter {
    void operator()(T* object) const {
        object->Release();
    }
};

template <typename T>
using ComAllocation = std::unique_ptr<T, ComDeleter<T>>;

// Only folders converted by this mod qualify for selection preservation.
// Eligibility is shared, but the selected item is always read from the exact
// tree being synchronized. No window or tab selection is stored globally.
// Keep Shell-owned strings so remembering a route needs no C++ heap allocation.
SRWLOCK g_convertedPathsLock = SRWLOCK_INIT;
std::array<ShellAllocation<wchar_t>, 32> g_convertedPaths;
size_t g_nextConvertedPath = 0;

bool SamePath(PCWSTR left, PCWSTR right) {
    return left && right &&
           CompareStringOrdinal(left, -1, right, -1, TRUE) == CSTR_EQUAL;
}

void RememberConvertedPath(ShellAllocation<wchar_t> path) {
    AcquireSRWLockExclusive(&g_convertedPathsLock);
    for (const auto& existing : g_convertedPaths) {
        if (SamePath(existing.get(), path.get())) {
            ReleaseSRWLockExclusive(&g_convertedPathsLock);
            return;
        }
    }
    g_convertedPaths[g_nextConvertedPath] = std::move(path);
    g_nextConvertedPath = (g_nextConvertedPath + 1) % g_convertedPaths.size();
    ReleaseSRWLockExclusive(&g_convertedPathsLock);
}

bool IsConvertedPath(PCWSTR path) {
    bool found = false;
    AcquireSRWLockShared(&g_convertedPathsLock);
    for (const auto& existing : g_convertedPaths) {
        if (SamePath(existing.get(), path)) {
            found = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_convertedPathsLock);
    return found;
}

void ClearConvertedPaths() {
    AcquireSRWLockExclusive(&g_convertedPathsLock);
    for (auto& path : g_convertedPaths) {
        path.reset();
    }
    g_nextConvertedPath = 0;
    ReleaseSRWLockExclusive(&g_convertedPathsLock);
}

// BrowseObject's default PIDL mode is absolute (SBSP_ABSOLUTE == 0).
// Relative PIDLs and history commands must never be interpreted as absolute.
bool IsAbsoluteNavigation(PCUIDLIST_RELATIVE pidl, UINT flags) {
    constexpr UINT otherModes = SBSP_RELATIVE | SBSP_PARENT |
                                SBSP_NAVIGATEBACK | SBSP_NAVIGATEFORWARD;
    return !(flags & otherModes) && pidl && pidl->mkid.cb != 0;
}

bool IsAbsoluteFileSystemPath(PCWSTR path) {
    if (!path || !path[0] || !path[1]) {
        return false;
    }

    // Includes UNC paths and extended paths (\\?\C:\... / \\?\UNC\...).
    if (path[0] == L'\\' && path[1] == L'\\') {
        return path[2] != L'\0';
    }

    const bool driveLetter = (path[0] >= L'A' && path[0] <= L'Z') ||
                             (path[0] >= L'a' && path[0] <= L'z');
    return driveLetter && path[1] == L':' && path[2] == L'\\';
}

ShellAllocation<wchar_t> GetName(PCIDLIST_ABSOLUTE pidl, SIGDN kind) {
    PWSTR name = nullptr;
    HRESULT hr = SHGetNameFromIDList(pidl, kind, &name);
    ShellAllocation<wchar_t> result(name);
    if (FAILED(hr)) {
        return {};
    }
    return result;
}

bool IsFileSystemFolder(IShellItem* item) {
    constexpr SFGAOF required = SFGAO_FILESYSTEM | SFGAO_FOLDER;
    SFGAOF attributes = 0;
    if (!item || FAILED(item->GetAttributes(required | SFGAO_STREAM,
                                           &attributes)) ||
        (attributes & required) != required || (attributes & SFGAO_STREAM)) {
        return false;
    }
    return true;
}

ShellAllocation<wchar_t> GetFileSystemPath(IShellItem* item) {
    if (!item) {
        return {};
    }
    PWSTR rawPath = nullptr;
    HRESULT hr = item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath);
    ShellAllocation<wchar_t> path(rawPath);
    if (FAILED(hr) || !IsAbsoluteFileSystemPath(path.get())) {
        return {};
    }
    return path;
}

ShellAllocation<wchar_t> GetAddressFolderPath(PCIDLIST_ABSOLUTE pidl) {
    if (!pidl || pidl->mkid.cb == 0) {
        return {};
    }

    IShellItem* rawItem = nullptr;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&rawItem));
    ComAllocation<IShellItem> item(rawItem);
    if (FAILED(hr) || !item) {
        return {};
    }

    auto path = GetFileSystemPath(item.get());
    if (!path || !IsFileSystemFolder(item.get())) {
        return {};
    }
    return path;
}

bool IsModernAddressBarCaller(void* address) {
    // On Windows 11 23H2, FileExplorerExtensions requests this name form only
    // for EditModeText and Copy address as text. Look up the loaded module at
    // call time so Explorer can load its modern UI after the mod initializes.
    const auto modernUi = GetModuleHandleW(L"FileExplorerExtensions.dll");
    HMODULE callerModule = nullptr;
    return modernUi &&
           GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              reinterpret_cast<PCWSTR>(address),
                              &callerModule) &&
           callerModule == modernUi;
}

struct AddressTextResolutionScope {
    AddressTextResolutionScope() {
        g_insideAddressTextResolution = true;
    }

    ~AddressTextResolutionScope() {
        g_insideAddressTextResolution = false;
    }
};

HRESULT WINAPI ShellItemGetDisplayName_Hook(IShellItem* item,
                                           SIGDN kind,
                                           PWSTR* name) {
    void* caller = __builtin_return_address(0);
    HRESULT hr = g_originalShellItemGetDisplayName(item, kind, name);
    if (kind != SIGDN_DESKTOPABSOLUTEEDITING || FAILED(hr) || !name || !*name ||
        IsAbsoluteFileSystemPath(*name) || g_insideAddressTextResolution) {
        return hr;
    }
    const bool addressCaller = IsModernAddressBarCaller(caller);
    Wh_Log(L"Editing-name request: %s; modern address caller=%d", *name,
           addressCaller);
    if (!addressCaller) {
        return hr;
    }

    AddressTextResolutionScope scope;
    PIDLIST_ABSOLUTE rawPidl = nullptr;
    HRESULT pidlHr = SHGetIDListFromObject(item, &rawPidl);
    ShellAllocation<ITEMIDLIST_ABSOLUTE> pidl(rawPidl);
    auto path = SUCCEEDED(pidlHr) ? GetAddressFolderPath(pidl.get())
                                 : ShellAllocation<wchar_t>{};
    if (path) {
        Wh_Log(L"Address edit text: %s -> %s", *name, path.get());
        // IShellItem::GetDisplayName returns a CoTaskMem allocation. Transfer
        // the filesystem-path allocation with the same ownership contract.
        CoTaskMemFree(*name);
        *name = path.release();
    } else {
        Wh_Log(L"Address edit text: keeping %s (not a filesystem folder)",
               *name);
    }
    return hr;
}

bool ShouldKeepSidebarSelection(void* tree, IShellItem* target) {
    // Check eligibility before attributes, which may query a UNC provider.
    // Unrelated sidebar syncs need only the Shell's filesystem name.
    auto targetPath = GetFileSystemPath(target);
    const bool converted = targetPath && IsConvertedPath(targetPath.get());
    Wh_Log(L"Sidebar target: %s; converted=%d",
           targetPath ? targetPath.get() : L"<no filesystem path>", converted);
    if (!converted || !IsFileSystemFolder(target)) {
        return false;
    }

    // The private selection methods receive the CNscTree object, whereas
    // its public interfaces can have adjusted this pointers. Resolve its
    // real QueryInterface method, then use the returned public interface;
    // no class-layout offsets or assumed vtable positions are needed.
    INameSpaceTreeControl* rawControl = nullptr;
    HRESULT hr = g_treeQueryInterface(tree, IID_PPV_ARGS(&rawControl));
    ComAllocation<INameSpaceTreeControl> control(rawControl);
    if (FAILED(hr) || !control) {
        Wh_Log(L"Sidebar QueryInterface failed: 0x%08X", (unsigned)hr);
        return false;
    }

    IShellItemArray* rawSelectedItems = nullptr;
    hr = control->GetSelectedItems(&rawSelectedItems);
    ComAllocation<IShellItemArray> selectedItems(rawSelectedItems);
    DWORD count = 0;
    if (SUCCEEDED(hr) && selectedItems) {
        hr = selectedItems->GetCount(&count);
    }
    if (FAILED(hr) || !selectedItems || count != 1) {
        Wh_Log(L"Sidebar selection unavailable: result=0x%08X; count=%u",
               (unsigned)hr, (unsigned)count);
        return false;
    }

    IShellItem* rawSelected = nullptr;
    hr = selectedItems->GetItemAt(0, &rawSelected);
    ComAllocation<IShellItem> selected(rawSelected);
    if (FAILED(hr) || !selected) {
        Wh_Log(L"Sidebar selected item unavailable: 0x%08X", (unsigned)hr);
        return false;
    }

    auto selectedPath = IsFileSystemFolder(selected.get())
                            ? GetFileSystemPath(selected.get())
                            : ShellAllocation<wchar_t>{};
    Wh_Log(L"Sidebar paths: requested=%s; selected=%s", targetPath.get(),
           selectedPath ? selectedPath.get() : L"<unavailable>");
    if (!SamePath(targetPath.get(), selectedPath.get())) {
        // Includes deliberate drive clicks, navigation to another folder,
        // and a selection in another tab. Let Explorer synchronize normally.
        return false;
    }

    Wh_Log(L"Keeping sidebar selection and scroll position: %s",
           targetPath.get());
    return true;
}

ShellAllocation<ITEMIDLIST_ABSOLUTE> GetKnownFolderPhysicalRoute(
    PCIDLIST_ABSOLUTE pidl) {
    // Explorer's UI thread already uses COM. A failed COM request simply
    // leaves the generic path-parser fallback available.
    IKnownFolderManager* rawManager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&rawManager));
    ComAllocation<IKnownFolderManager> manager(rawManager);
    if (FAILED(hr) || !manager) {
        return {};
    }

    IKnownFolder* rawFolder = nullptr;
    hr = manager->FindFolderFromIDList(pidl, &rawFolder);
    ComAllocation<IKnownFolder> folder(rawFolder);
    if (FAILED(hr) || !folder) {
        return {};
    }

    PIDLIST_ABSOLUTE rawPhysical = nullptr;
    hr = folder->GetIDList(KF_FLAG_NO_ALIAS | KF_FLAG_DONT_VERIFY,
                          &rawPhysical);
    ShellAllocation<ITEMIDLIST_ABSOLUTE> physical(rawPhysical);
    if (FAILED(hr)) {
        return {};
    }
    return physical;
}

ShellAllocation<ITEMIDLIST_ABSOLUTE> ResolveFileSystemAlias(
    PCIDLIST_ABSOLUTE pidl) {
    auto editingName = GetName(pidl, SIGDN_DESKTOPABSOLUTEEDITING);
    auto parsingName = GetName(pidl, SIGDN_DESKTOPABSOLUTEPARSING);
    Wh_Log(L"Target names: editing=%s; parsing=%s",
           editingName ? editingName.get() : L"<unavailable>",
           parsingName ? parsingName.get() : L"<unavailable>");
    if (!editingName || !parsingName ||
        (IsAbsoluteFileSystemPath(editingName.get()) &&
         IsAbsoluteFileSystemPath(parsingName.get()))) {
        // A physical route has both an absolute editing address and an
        // absolute filesystem parsing name. An alias can expose a GUID route
        // even if its editing name is already a path. Avoid metadata queries
        // and parsing for ordinary filesystem navigation.
        return {};
    }

    // Do not convert virtual folders or file-backed folder providers such as
    // ZIP archives into a plain filesystem navigation.
    IShellItem* rawItem = nullptr;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&rawItem));
    ComAllocation<IShellItem> item(rawItem);
    if (FAILED(hr) || !item) {
        return {};
    }

    constexpr SFGAOF required = SFGAO_FILESYSTEM | SFGAO_FOLDER;
    SFGAOF attributes = 0;
    hr = item->GetAttributes(required | SFGAO_STREAM, &attributes);
    if (FAILED(hr) || (attributes & required) != required ||
        (attributes & SFGAO_STREAM)) {
        Wh_Log(L"Skipping a virtual or stream-backed location");
        return {};
    }

    auto path = GetName(pidl, SIGDN_FILESYSPATH);
    if (!path || !IsAbsoluteFileSystemPath(path.get())) {
        Wh_Log(L"Shell did not supply an absolute filesystem path");
        return {};
    }

    auto canonical = GetKnownFolderPhysicalRoute(pidl);
    if (!canonical) {
        PIDLIST_ABSOLUTE rawCanonical = nullptr;
        SFGAOF canonicalAttributes = required | SFGAO_STREAM;
        hr = SHParseDisplayName(path.get(), nullptr, &rawCanonical,
                               canonicalAttributes, &canonicalAttributes);
        canonical.reset(rawCanonical);
        if (FAILED(hr) || !canonical ||
            (canonicalAttributes & required) != required ||
            (canonicalAttributes & SFGAO_STREAM)) {
            Wh_Log(L"Parsing the physical path failed: 0x%08X", (unsigned)hr);
            return {};
        }
    } else {
        Wh_Log(L"Using the known-folder route with KF_FLAG_NO_ALIAS");
    }

    if (canonical->mkid.cb == 0) {
        Wh_Log(L"Physical route unexpectedly resolved to the Shell Desktop");
        return {};
    }

    // Verify that resolution didn't select a different location or simply
    // recreate the same alias. Compare PIDL bytes: Shell semantic equality
    // can report two different namespace routes as the same folder.
    auto canonicalPath = GetName(canonical.get(), SIGDN_FILESYSPATH);
    auto canonicalName = GetName(canonical.get(), SIGDN_DESKTOPABSOLUTEEDITING);
    auto canonicalParsingName =
        GetName(canonical.get(), SIGDN_DESKTOPABSOLUTEPARSING);
    if (!SamePath(path.get(), canonicalPath.get()) ||
        !canonicalName || !IsAbsoluteFileSystemPath(canonicalName.get()) ||
        !canonicalParsingName ||
        !IsAbsoluteFileSystemPath(canonicalParsingName.get())) {
        Wh_Log(L"Physical path did not produce an absolute editing address: %s",
               path.get());
        return {};
    }

    const UINT originalSize = ILGetSize(pidl);
    if (originalSize == ILGetSize(canonical.get()) &&
        std::memcmp(pidl, canonical.get(), originalSize) == 0) {
        return {};
    }

    Wh_Log(L"Resolved alias: %s (%s) -> %s", editingName.get(),
           parsingName.get(), path.get());

    // Register before calling BrowseObject: sidebar synchronization can run
    // either inside that call or later, after asynchronous navigation finishes.
    RememberConvertedPath(std::move(path));
    return canonical;
}

struct NormalizationScope {
    NormalizationScope() {
        g_insideNormalization = true;
    }

    ~NormalizationScope() {
        g_insideNormalization = false;
    }
};

struct SelectionCheckScope {
    SelectionCheckScope() {
        g_insideSelectionCheck = true;
    }

    ~SelectionCheckScope() {
        g_insideSelectionCheck = false;
    }
};

HRESULT SetSelectedItem(void* tree,
                       IShellItem* item,
                       SetSelectedItem_t original,
                       PCWSTR method) {
    if (!g_insideSelectionCheck) {
        SelectionCheckScope scope;
        Wh_Log(L"Sidebar selection: %s", method);
        if (ShouldKeepSidebarSelection(tree, item)) {
            // Both native methods select a tree row and call _EnsureVisible.
            // The correct folder is already selected, so neither operation
            // is needed. Keep the clicked row and viewport without a timer,
            // a second navigation, or a later scroll-position restoration.
            return S_OK;
        }
    }
    return original(tree, item);
}

HRESULT WINAPI SetSelectedItem_Hook(void* tree, IShellItem* item) {
    return SetSelectedItem(tree, item, g_originalSetSelectedItem,
                           L"_SetSelectedItem");
}

HRESULT WINAPI SetSelectedItemNoExpand_Hook(void* tree, IShellItem* item) {
    return SetSelectedItem(tree, item, g_originalSetSelectedItemNoExpand,
                           L"_SetSelectedItemNoExpand");
}

HRESULT WINAPI BrowseObject_Hook(void* browser,
                                PCUIDLIST_RELATIVE pidl,
                                UINT flags) {
    if (g_insideNormalization) {
        return g_originalBrowseObject(browser, pidl, flags);
    }

    Wh_Log(L"BrowseObject: flags=0x%08X", flags);

    if (!IsAbsoluteNavigation(pidl, flags)) {
        return g_originalBrowseObject(browser, pidl, flags);
    }

    ShellAllocation<ITEMIDLIST_ABSOLUTE> canonical;
    {
        NormalizationScope scope;
        canonical = ResolveFileSystemAlias(
            reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl));
    }

    if (canonical) {
        Wh_Log(L"Converted navigation: flags=0x%08X; sidebar preservation eligible",
               flags);
    }

    // Keep the allocated PIDL alive through the original call. No second
    // navigation, window messages, or focus changes are necessary.
    return g_originalBrowseObject(
        browser,
        canonical ? reinterpret_cast<PCUIDLIST_RELATIVE>(canonical.get()) : pidl,
        flags);
}

void ReleaseExplorerFrame() {
    if (g_explorerFrame) {
        FreeLibrary(g_explorerFrame);
        g_explorerFrame = nullptr;
    }
}

}  // namespace

BOOL Wh_ModInit() {
    g_explorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr,
                                     LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_explorerFrame) {
        Wh_Log(L"Could not load ExplorerFrame.dll: %u", GetLastError());
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {
                L"public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const __unaligned *,unsigned int)",
                L"public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const *,unsigned int)",
                L"public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST const *,unsigned int)",
            },
            &g_originalBrowseObject,
            BrowseObject_Hook,
        },
        {
            {
                L"private: long __cdecl CNscTree::_SetSelectedItem(struct IShellItem *)",
            },
            &g_originalSetSelectedItem,
            SetSelectedItem_Hook,
        },
        {
            {
                L"private: long __cdecl CNscTree::_SetSelectedItemNoExpand(struct IShellItem *)",
            },
            &g_originalSetSelectedItemNoExpand,
            SetSelectedItemNoExpand_Hook,
        },
        {
            {
                L"public: virtual long __cdecl CNscTree::QueryInterface(struct _GUID const &,void * *)",
                L"public: virtual long __cdecl CNscTree::QueryInterface(struct _GUID const &,void **)",
            },
            &g_treeQueryInterface,
        },
    };

    if (!WindhawkUtils::HookSymbols(g_explorerFrame, explorerFrameDllHooks,
                                   ARRAYSIZE(explorerFrameDllHooks)) ||
        !g_originalBrowseObject || !g_originalSetSelectedItem ||
        !g_originalSetSelectedItemNoExpand || !g_treeQueryInterface) {
        Wh_Log(L"Required navigation/sidebar symbols unavailable; this Windows build is unsupported");
        ReleaseExplorerFrame();
        return FALSE;
    }

    // The modern address bar calls IShellItem::GetDisplayName directly;
    // CAddressList's legacy ComboBoxEx update isn't used by this Windows UI.
    // This optional hook changes only full editing-name requests from the
    // modern UI. Existing navigation/sidebar behavior survives missing symbols.
    g_windowsStorage = LoadLibraryExW(L"Windows.Storage.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_windowsStorage) {
        // Windows.Storage.dll
        WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
            {
                {
                    L"public: virtual long __cdecl CShellItem::GetDisplayName(enum _SIGDN,unsigned short * *)",
                    L"public: virtual long __cdecl CShellItem::GetDisplayName(enum _SIGDN,unsigned short **)",
                    L"public: virtual long __cdecl CShellItem::GetDisplayName(enum _SIGDN,wchar_t * *)",
                    L"public: virtual long __cdecl CShellItem::GetDisplayName(enum _SIGDN,wchar_t **)",
                },
                &g_originalShellItemGetDisplayName,
                ShellItemGetDisplayName_Hook,
                true,
            },
        };
        if (WindhawkUtils::HookSymbols(g_windowsStorage, windowsStorageHooks,
                                      ARRAYSIZE(windowsStorageHooks)) &&
            g_originalShellItemGetDisplayName) {
            Wh_Log(L"Modern filesystem address-text fallback enabled (v0.1.5)");
        } else {
            Wh_Log(L"Shell editing-name symbol unavailable; using navigation conversion only");
        }
    } else {
        Wh_Log(L"Could not load Windows.Storage.dll; using navigation conversion only");
    }

    Wh_Log(L"Explorer real folder paths initialized");
    return TRUE;
}

void Wh_ModUninit() {
    // Windhawk has removed our hooks and waited for active hook calls.
    ClearConvertedPaths();
    if (g_windowsStorage) {
        FreeLibrary(g_windowsStorage);
        g_windowsStorage = nullptr;
    }
    ReleaseExplorerFrame();
}
