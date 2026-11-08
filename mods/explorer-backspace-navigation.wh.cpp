// ==WindhawkMod==
// @id              explorer-backspace-navigation
// @name            Explorer Backspace Navigation
// @name:de         Explorer-Ordnernavigation mit Backspace
// @description     Go up with Backspace, or keep history navigation with a parent-folder fallback for missing folders
// @description:de  Mit Backspace zum Elternordner wechseln oder den Verlauf nutzen und bei fehlenden Ordnern zum Elternordner wechseln
// @version         0.1.1
// @author          Nerdworld
// @github          https://github.com/nerdworldDE
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lshlwapi -lole32 -loleaut32 -luuid
// @license         MIT
// ==/WindhawkMod==

// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# Explorer Backspace Navigation

Explorer normally uses Backspace to revisit the previous location. If that
folder has been deleted or moved, it can show a "Location is not available"
error instead of taking you to the parent folder.

![Explorer's "Location is not available" error for a missing folder in navigation history](https://raw.githubusercontent.com/nerdworldDE/windhawk-mods/f212e7dc239e5de9377ef68ffe9b3aab8e7d39fb/explorer-backspace-navigation.png)

The screenshot shows the standard Explorer error this mod's missing-folder
fallback is designed to avoid.

Choose how unmodified **Backspace** navigates in File Explorer:

* **Always go to the parent folder** (enabled by default): Backspace behaves
  like Alt+Up, regardless of navigation history. For example, from
  `C:\test\foo` it opens `C:\test`, even after visiting `C:\test\foo\bar`.
* **Switch disabled**: Keep Explorer's normal history navigation. Before
  opening the previous location, check whether its filesystem folder still
  exists. If it was deleted, renamed, or moved, open the
  parent of the **current** folder instead. Other history entries are not
  searched or removed.

If several ancestors have been deleted, the mod chooses the nearest existing
ancestor. At the Shell Desktop root there is no parent, so Backspace does
nothing in the first mode.

Renaming, typing in the address/search bars, and dialogs keep their normal
Backspace behavior. Shortcuts using Ctrl, Alt, Shift, or the Windows key,
as well as the Back/Forward buttons, are unaffected. Settings apply immediately.

## Scope and compatibility

Intended for 64-bit Windows 10 and Windows 11 Explorer, including tabbed
windows. The browser is obtained through IShellWindows and matched to the
focused tab or folder view. If it cannot be matched, native processing is kept.
The mod does not maintain a separate history.

Initial user testing of version 0.1.1 reported working behavior after enabling
the mod. Broader Windows-build and scenario coverage remains to be validated.
The mod requires ExplorerFrame's
`CShellBrowser::_MayTranslateAccelerator` symbol. If it is unavailable,
initialization fails with a log message. Internal C++ object addresses are
never used as COM interface pointers.

History checks cover absolute filesystem paths and file URLs, including
Unicode and UNC paths. Virtual locations such as Home, This PC, searches,
and archive views keep normal history navigation. If history cannot be read,
the URL is not understood, or the existence check is inconclusive (for example,
access denied), Explorer keeps its normal behavior. Such failures are not
treated as proof that the folder was deleted.

Checks use normal filesystem I/O, so an unresponsive network location can
delay the key press. A folder can also disappear between the check and
Explorer opening it; this mod does not suppress unrelated error dialogs.

Enable this mod's debug logging in Windhawk to diagnose missing symbols,
history-service failures, and navigation results. No files, history entries,
or Explorer registry preferences are changed by the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- alwaysGoUp: true
  $name: Always go to the parent folder
  $name:de: Immer zum übergeordneten Ordner wechseln
  $description: "Enabled: Backspace always goes up. Disabled: keep history navigation, but go up from the current folder if the previous folder no longer exists."
  $description:de: "Aktiv: Backspace geht immer zum Elternordner. Inaktiv: Verlauf nutzen; fehlt das vorherige Ziel, zum Elternordner des aktuellen Ordners wechseln."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shlobj.h>
#include <shlwapi.h>
#include <tlogstg.h>
#include <exdisp.h>

#include <atomic>
#include <cwchar>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Deliberately incomplete: a private C++ object must not implicitly convert
// to an IUnknown/IShellBrowser pointer, regardless of its runtime layout.
struct ShellBrowserImplementation;
using MayTranslateAccelerator_t = HRESULT(WINAPI*)(ShellBrowserImplementation*, MSG*, int);

MayTranslateAccelerator_t g_originalMayTranslateAccelerator = nullptr;
HMODULE g_explorerFrame = nullptr;
std::atomic<bool> g_alwaysGoUp{true};
thread_local bool g_handlingBackspace = false;

struct CoTaskMemDeleter {
    void operator()(void* value) const {
        CoTaskMemFree(value);
    }
};

template <typename T>
using ShellAllocation = std::unique_ptr<T, CoTaskMemDeleter>;

template <typename T>
struct ComDeleter {
    void operator()(T* value) const {
        value->Release();
    }
};

template <typename T>
using ComAllocation = std::unique_ptr<T, ComDeleter<T>>;

struct BackspaceScope {
    BackspaceScope() {
        g_handlingBackspace = true;
    }
    ~BackspaceScope() {
        g_handlingBackspace = false;
    }
};

enum class FolderState { Exists, Missing, Unknown };

FolderState ClassifyAttributes(DWORD attributes, DWORD error) {
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        // An existing file might be a browsable archive or Shell extension.
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) ? FolderState::Exists
                                                       : FolderState::Unknown;
    }
    // Access denied, offline providers, and unavailable network devices do not
    // establish that the folder is missing. Preserve native error handling.
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
        return FolderState::Missing;
    }
    return FolderState::Unknown;
}

bool IsAbsoluteFileSystemPath(std::wstring_view path) {
    if (path.size() >= 3 &&
        ((path[0] >= L'A' && path[0] <= L'Z') ||
         (path[0] >= L'a' && path[0] <= L'z')) &&
        path[1] == L':' && path[2] == L'\\') {
        return true;
    }
    return path.size() > 2 && path[0] == L'\\' && path[1] == L'\\';
}

FolderState CheckFolderPath(PCWSTR path) {
    if (!path || !IsAbsoluteFileSystemPath(path)) {
        return FolderState::Unknown;
    }
    std::wstring probePath(path);
    if (probePath.size() >= MAX_PATH && probePath.compare(0, 4, L"\\\\?\\") != 0 &&
        probePath.compare(0, 4, L"\\\\.\\") != 0) {
        if (probePath.compare(0, 2, L"\\\\") == 0) {
            probePath = L"\\\\?\\UNC\\" + probePath.substr(2);
        } else {
            probePath = L"\\\\?\\" + probePath;
        }
    }
    DWORD attributes = GetFileAttributesW(probePath.c_str());
    DWORD error = attributes == INVALID_FILE_ATTRIBUTES ? GetLastError()
                                                         : ERROR_SUCCESS;
    FolderState state = ClassifyAttributes(attributes, error);
    if (state == FolderState::Missing) {
        // C:\archive.zip\inner is a valid Shell folder, but GetFileAttributes
        // cannot see inside archives. An existing file component makes this
        // an inconclusive check rather than proof of a deleted folder.
        std::wstring ancestor(probePath);
        while (true) {
            size_t previousLength = ancestor.size();
            size_t separator = ancestor.find_last_of(L'\\');
            if (separator == std::wstring::npos || separator < 3) {
                break;
            }
            ancestor.resize(separator);
            if (!ancestor.empty() && ancestor.back() == L':') {
                ancestor += L'\\';
            }
            if (ancestor.size() >= previousLength) {
                break;
            }
            DWORD ancestorAttributes = GetFileAttributesW(ancestor.c_str());
            if (ancestorAttributes != INVALID_FILE_ATTRIBUTES) {
                if (!(ancestorAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    state = FolderState::Unknown;
                }
                break;
            }
            DWORD ancestorError = GetLastError();
            if (ancestorError != ERROR_FILE_NOT_FOUND &&
                ancestorError != ERROR_PATH_NOT_FOUND) {
                state = FolderState::Unknown;
                break;
            }
        }
    }
    Wh_Log(L"Folder check: state=%d, error=%lu", static_cast<int>(state), error);
    return state;
}

FolderState CheckHistoryUrl(PCWSTR url) {
    if (!url || !*url) {
        return FolderState::Unknown;
    }
    // Raw paths must not be URL-decoded: '%' and '#' are legal filename chars.
    if (IsAbsoluteFileSystemPath(url)) {
        return CheckFolderPath(url);
    }
    if (_wcsnicmp(url, L"file:", 5) != 0) {
        return FolderState::Unknown;
    }
    PWSTR rawPath = nullptr;
    HRESULT hr = PathCreateFromUrlAlloc(url, &rawPath, 0);
    ShellAllocation<wchar_t> path(rawPath);
    if (FAILED(hr) || !path) {
        return FolderState::Unknown;
    }
    return CheckFolderPath(path.get());
}

FolderState CheckPreviousFolder(IShellBrowser* browser) {
    IServiceProvider* rawProvider = nullptr;
    HRESULT hr = browser->QueryInterface(IID_PPV_ARGS(&rawProvider));
    ComAllocation<IServiceProvider> provider(rawProvider);
    if (FAILED(hr) || !provider) {
        Wh_Log(L"Browser service provider unavailable: 0x%08lX", hr);
        return FolderState::Unknown;
    }
    ITravelLogStg* rawTravelLog = nullptr;
    // SID_STravelLogCursor is the same GUID as ITravelLogStg. Use the type's
    // UUID so this also links with SDKs whose uuid library omits that IID.
    hr = provider->QueryService(__uuidof(ITravelLogStg),
                                IID_PPV_ARGS(&rawTravelLog));
    ComAllocation<ITravelLogStg> travelLog(rawTravelLog);
    if (FAILED(hr) || !travelLog) {
        Wh_Log(L"Travel log unavailable; keeping native Backspace: 0x%08lX", hr);
        return FolderState::Unknown;
    }
    ITravelLogEntry* rawEntry = nullptr;
    hr = travelLog->GetRelativeEntry(-1, &rawEntry);
    ComAllocation<ITravelLogEntry> entry(rawEntry);
    if (hr != S_OK || !entry) {
        Wh_Log(L"No readable previous history entry: 0x%08lX", hr);
        return FolderState::Unknown;
    }
    PWSTR rawUrl = nullptr;
    hr = entry->GetURL(&rawUrl);
    ShellAllocation<wchar_t> url(rawUrl);
    if (FAILED(hr)) {
        Wh_Log(L"Previous history URL unavailable: 0x%08lX", hr);
        return FolderState::Unknown;
    }
    return CheckHistoryUrl(url.get());
}

ShellAllocation<ITEMIDLIST_ABSOLUTE> GetCurrentFolder(IShellBrowser* browser) {
    IShellView* rawView = nullptr;
    HRESULT hr = browser->QueryActiveShellView(&rawView);
    ComAllocation<IShellView> view(rawView);
    if (FAILED(hr) || !view) {
        return {};
    }
    IFolderView* rawFolderView = nullptr;
    hr = view->QueryInterface(IID_PPV_ARGS(&rawFolderView));
    ComAllocation<IFolderView> folderView(rawFolderView);
    if (FAILED(hr) || !folderView) {
        return {};
    }
    IPersistFolder2* rawFolder = nullptr;
    hr = folderView->GetFolder(IID_PPV_ARGS(&rawFolder));
    ComAllocation<IPersistFolder2> folder(rawFolder);
    if (FAILED(hr) || !folder) {
        return {};
    }
    PIDLIST_ABSOLUTE rawPidl = nullptr;
    hr = folder->GetCurFolder(&rawPidl);
    ShellAllocation<ITEMIDLIST_ABSOLUTE> pidl(rawPidl);
    return SUCCEEDED(hr) ? std::move(pidl)
                         : ShellAllocation<ITEMIDLIST_ABSOLUTE>{};
}

HRESULT NavigateToParent(IShellBrowser* browser) {
    auto parent = GetCurrentFolder(browser);
    if (!parent) {
        // Shell extensions need not expose IFolderView/IPersistFolder2.
        return browser->BrowseObject(nullptr, SBSP_SAMEBROWSER | SBSP_PARENT);
    }
    std::vector<wchar_t> path(32768);
    // GetCurFolder returns an owned copy. Remove components from that copy
    // only; never alter a Shell-owned PIDL or the travel log.
    while (ILRemoveLastID(parent.get())) {
        if (SHGetPathFromIDListEx(parent.get(), path.data(),
                                 static_cast<DWORD>(path.size()),
                                 GPFIDL_DEFAULT) &&
            CheckFolderPath(path.data()) == FolderState::Missing) {
            Wh_Log(L"Parent also missing; checking the next ancestor");
            continue;
        }
        return browser->BrowseObject(parent.get(),
                                     SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
    }
    // The empty PIDL is the Shell Desktop, which has no parent.
    return S_OK;
}

bool HasClass(HWND window, PCWSTR name) {
    wchar_t className[128] = {};
    return GetClassNameW(window, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, name) == 0;
}

HWND FindContainingTab(HWND focus, HWND root) {
    for (HWND window = focus; window && window != root;
         window = GetParent(window)) {
        if (HasClass(window, L"ShellTabWindowClass")) {
            return window;
        }
    }
    return nullptr;
}

bool MatchesFocusedBrowser(IShellBrowser* browser, HWND focus, HWND tab) {
    if (tab) {
        HWND browserWindow = nullptr;
        return SUCCEEDED(browser->GetWindow(&browserWindow)) &&
               browserWindow == tab;
    }
    // Older layouts might not expose a containing tab. Match the actual
    // folder view's window instead of selecting the first browser in a frame.
    IShellView* rawView = nullptr;
    HRESULT hr = browser->QueryActiveShellView(&rawView);
    ComAllocation<IShellView> view(rawView);
    HWND viewWindow = nullptr;
    return SUCCEEDED(hr) && view && SUCCEEDED(view->GetWindow(&viewWindow)) &&
           viewWindow && (focus == viewWindow || IsChild(viewWindow, focus));
}

ComAllocation<IShellBrowser> GetBrowserForMessage(const MSG* message) {
    HWND root = GetAncestor(message->hwnd, GA_ROOT);
    HWND focus = GetFocus();
    if (!root || !focus || GetAncestor(focus, GA_ROOT) != root) {
        return {};
    }
    HWND tab = FindContainingTab(focus, root);
    Wh_Log(L"Resolving browser: frame=%p, focus=%p, tab=%p", root, focus, tab);

    IShellWindows* rawWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr,
                                   CLSCTX_LOCAL_SERVER,
                                   IID_PPV_ARGS(&rawWindows));
    ComAllocation<IShellWindows> windows(rawWindows);
    if (FAILED(hr) || !windows) {
        Wh_Log(L"IShellWindows unavailable: 0x%08lX", hr);
        return {};
    }
    long count = 0;
    hr = windows->get_Count(&count);
    if (FAILED(hr)) {
        Wh_Log(L"Shell window enumeration failed: 0x%08lX", hr);
        return {};
    }
    Wh_Log(L"Enumerating %ld Shell window(s)", count);
    for (long i = 0; i < count; ++i) {
        VARIANT index{};
        index.vt = VT_I4;
        index.lVal = i;
        IDispatch* rawDispatch = nullptr;
        hr = windows->Item(index, &rawDispatch);
        ComAllocation<IDispatch> dispatch(rawDispatch);
        if (FAILED(hr) || !dispatch) {
            continue;
        }
        IWebBrowser2* rawWebBrowser = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&rawWebBrowser));
        ComAllocation<IWebBrowser2> webBrowser(rawWebBrowser);
        SHANDLE_PTR handle = 0;
        if (FAILED(hr) || !webBrowser ||
            FAILED(webBrowser->get_HWND(&handle)) ||
            GetAncestor(reinterpret_cast<HWND>(handle), GA_ROOT) != root) {
            continue;
        }
        IServiceProvider* rawProvider = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&rawProvider));
        ComAllocation<IServiceProvider> provider(rawProvider);
        if (FAILED(hr) || !provider) {
            continue;
        }
        IShellBrowser* rawBrowser = nullptr;
        hr = provider->QueryService(SID_STopLevelBrowser,
                                    IID_PPV_ARGS(&rawBrowser));
        ComAllocation<IShellBrowser> browser(rawBrowser);
        if (SUCCEEDED(hr) && browser &&
            MatchesFocusedBrowser(browser.get(), focus, tab)) {
            // COM may pump messages. Do not navigate a tab whose focus changed
            // while obtaining its interface.
            if (GetForegroundWindow() != root || GetFocus() != focus) {
                Wh_Log(L"Focus changed during browser resolution; keeping native processing");
                return {};
            }
            Wh_Log(L"Matched focused browser through IShellWindows");
            return browser;
        }
    }
    Wh_Log(L"No browser matched the focused tab/view; keeping native processing");
    return {};
}

bool IsNavigationBackspace(const MSG* message) {
    if (!message || message->message != WM_KEYDOWN ||
        message->wParam != VK_BACK || !message->hwnd) {
        return false;
    }
    for (int key : {VK_CONTROL, VK_MENU, VK_SHIFT, VK_LWIN, VK_RWIN}) {
        if (GetKeyState(key) & 0x8000) {
            return false;
        }
    }
    HWND root = GetAncestor(message->hwnd, GA_ROOT);
    if (!root || root != GetForegroundWindow() ||
        (!HasClass(root, L"CabinetWClass") && !HasClass(root, L"ExploreWClass"))) {
        return false;
    }
    GUITHREADINFO info{};
    info.cbSize = sizeof(info);
    if (!GetGUIThreadInfo(GetCurrentThreadId(), &info) || !info.hwndFocus ||
        GetAncestor(info.hwndFocus, GA_ROOT) != root || info.hwndCaret ||
        (info.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE | GUI_SYSTEMMENUMODE))) {
        return false;
    }
    // Restrict interception to folder-view or navigation-tree focus. Modern
    // XAML address/search controls can have no Win32 caret; class checks alone
    // are insufficient to identify them as text inputs.
    bool navigationControl = false;
    for (HWND window = info.hwndFocus; window && window != root;
         window = GetParent(window)) {
        wchar_t name[128] = {};
        if (!GetClassNameW(window, name, ARRAYSIZE(name))) {
            return false;
        }
        if (_wcsicmp(name, L"Edit") == 0 ||
            _wcsnicmp(name, L"RichEdit", 8) == 0 ||
            wcsstr(name, L"TextBox") || wcsstr(name, L"Address") ||
            wcsstr(name, L"Search") || _wcsicmp(name, L"ComboBoxEx32") == 0 ||
            wcsstr(name, L"DesktopChildSiteBridge") ||
            wcsstr(name, L"DesktopWindowContentBridge")) {
            return false;
        }
        if (_wcsicmp(name, L"SHELLDLL_DefView") == 0 ||
            _wcsicmp(name, L"SysTreeView32") == 0 ||
            _wcsicmp(name, L"NamespaceTreeControl") == 0) {
            navigationControl = true;
        }
    }
    return navigationControl;
}

HRESULT WINAPI MayTranslateAccelerator_Hook(ShellBrowserImplementation* opaqueBrowserObject,
                                           MSG* message,
                                           int flags) {
    DWORD savedError = GetLastError();
    if (!g_handlingBackspace && IsNavigationBackspace(message)) {
        BackspaceScope scope;
        try {
            Wh_Log(L"Backspace accepted (v0.1.1); obtaining the focused browser");
            // The hook receives a private C++ object, not a COM interface.
            // Preserve it only for forwarding the original method. In 0.1.0 a
            // direct QueryInterface implementation call here used the wrong
            // object-layout assumptions and could crash before the first log.
            auto browser = GetBrowserForMessage(message);
            if (browser) {
                bool alwaysGoUp = g_alwaysGoUp.load(std::memory_order_relaxed);
                if (alwaysGoUp ||
                    CheckPreviousFolder(browser.get()) == FolderState::Missing) {
                    Wh_Log(L"Backspace: %s", alwaysGoUp ? L"always go up"
                                                       : L"missing history target");
                    HRESULT hr = NavigateToParent(browser.get());
                    Wh_Log(L"Parent navigation returned 0x%08lX", hr);
                    // S_OK marks the key as consumed, even when navigation
                    // failed. Replaying it could open the missing history URL.
                    return S_OK;
                }
            }
        } catch (...) {
            // Never let allocation exceptions escape into Explorer.
            Wh_Log(L"Backspace handler failed; keeping native processing");
        }
    }
    SetLastError(savedError);
    return g_originalMayTranslateAccelerator(opaqueBrowserObject, message, flags);
}

void LoadSettings() {
    g_alwaysGoUp.store(Wh_GetIntSetting(L"alwaysGoUp") != 0,
                       std::memory_order_relaxed);
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_explorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr,
                                     LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_explorerFrame) {
        Wh_Log(L"Could not load ExplorerFrame.dll: %lu", GetLastError());
        return FALSE;
    }

    // ExplorerFrame.dll
    WindhawkUtils::SYMBOL_HOOK explorerFrameHooks[] = {
        {
            {L"private: long __cdecl CShellBrowser::_MayTranslateAccelerator(struct tagMSG *,int)"},
            &g_originalMayTranslateAccelerator,
            MayTranslateAccelerator_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(g_explorerFrame, explorerFrameHooks,
                                   ARRAYSIZE(explorerFrameHooks)) ||
        !g_originalMayTranslateAccelerator) {
        Wh_Log(L"Required Explorer keyboard symbols unavailable; unsupported Windows build");
        FreeLibrary(g_explorerFrame);
        g_explorerFrame = nullptr;
        return FALSE;
    }
    Wh_Log(L"Explorer Backspace Navigation v0.1.1 initialized (IShellWindows browser resolution)");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Windhawk has removed the hooks and waited for active hook calls.
    if (g_explorerFrame) {
        FreeLibrary(g_explorerFrame);
        g_explorerFrame = nullptr;
    }
}
