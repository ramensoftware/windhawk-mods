// ==WindhawkMod==
// @id              explorer-folder-hover-menu
// @name            Folder Hover Menu
// @description     Hover a folder in File Explorer to get an expand button that opens a cascading menu of the folder's contents
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lshlwapi -lshell32 -lgdi32 -lgdiplus -ladvapi32 -luiautomationcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Folder Hover Menu

When you hover the mouse over a folder in the File Explorer file list, a small
expand button appears in the bottom-right corner of that folder. Click it to pop
up a cascading menu of the folder's contents. You can navigate into sub-folders
and launch items straight from the menu, without opening the folder first.

Inspired by [QTTabBar](https://qttabbar.wikidot.com/).

![Screenshot](https://i.imgur.com/ZPceXoZ.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- iconSize: 18
  $name: Button size
  $description: The size of the expand button shown on hover.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <initguid.h>  // Must come first so the shell GUIDs we use get storage.

#include <docobj.h>
#include <exdisp.h>
#include <gdiplus.h>
#include <servprov.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <uiautomation.h>
#include <windows.h>

#include <string>
#include <unordered_map>
#include <vector>

// Undocumented command id in the CLSID_MenuBand command group that dismisses
// the menu band. Used to tear the popup down when the foreground process
// changes.
#define MBAND_CMDID_CLOSE 22

#ifndef SMSET_USEBKICONEXTRACTION
#define SMSET_USEBKICONEXTRACTION 0x00000008
#endif

// {ECD4FC4F-521C-11D0-B792-00A0C90312E1}. Not declared by the public headers,
// so we provide it ourselves (initguid.h above gives it storage in this TU).
DEFINE_GUID(CLSID_MenuDeskBar,
            0xecd4fc4f,
            0x521c,
            0x11d0,
            0xb7,
            0x92,
            0x0,
            0xa0,
            0xc9,
            0x3,
            0x12,
            0xe1);

#define WM_APP_SETTINGS_CHANGED (WM_APP + 1)
// Posted to ask the UI thread to quit. Its handler calls PostQuitMessage;
// posting WM_QUIT directly with PostThreadMessage is discouraged (a modal loop
// can drop it).
#define WM_APP_QUIT (WM_APP + 2)
// Worker -> UI sink window: a fresh snapshot is ready, re-evaluate the hover.
#define WM_APP_REFRESH_DONE (WM_APP + 3)
// UI -> worker thread: build a snapshot for the pending request.
#define WM_APP_DO_REFRESH (WM_APP + 4)

////////////////////////////////////////////////////////////////////////////////
// Settings.

static int g_iconSize = 18;

static void LoadSettings() {
    int iconSize = Wh_GetIntSetting(L"iconSize");
    if (iconSize < 10) {
        iconSize = 10;
    } else if (iconSize > 64) {
        iconSize = 64;
    }
    g_iconSize = iconSize;
}

////////////////////////////////////////////////////////////////////////////////
// Tool process state (everything below runs on the UI thread).

static HINSTANCE g_hInst;
static HANDLE g_uiThread;
static DWORD g_uiThreadId;
static HANDLE g_readyEvent;

static ULONG_PTR g_gdiplusToken;
static HWND g_chevronWnd;  // The visible expand button.
static HWND g_sinkWnd;  // Hidden window: raw input + app messages (UI thread).
static HWINEVENTHOOK g_foregroundHook;

// Background worker (its own STA) that does all UI Automation + shell work, so
// the UI thread only ever hit-tests a cached snapshot and renders the button.
static HANDLE g_workerThread;
static DWORD g_workerThreadId;
static HANDLE g_workerReadyEvent;
static IUIAutomation* g_workerUia;               // Worker thread only.
static IUIAutomationElement* g_workerContainer;  // Worker thread only.
static HWND g_workerContainerRoot;               // Worker thread only.
static PIDLIST_ABSOLUTE g_workerFolderAbs;  // Worker thread only: the folder
static bool g_workerFolderIsDesktop;        // the shared children map holds.
static bool g_workerChildrenValid;

// True while a File Explorer window or the desktop is the foreground window.
// Raw input is ignored otherwise, so the mod does no work for other apps.
static bool g_active;

static bool g_chevronVisible;
static bool g_menuActive;
static RECT g_hoverItemRect;
static RECT g_chevronRect;
static PIDLIST_ABSOLUTE g_targetPidl;

// How long (ms) a snapshot is trusted before a mouse move triggers a background
// rebuild. This is not a timer; it only gates work done on actual input events.
static const ULONGLONG kRefreshTtlMs = 500;

// Raw input is high frequency; coalesce moves to roughly this interval (ms).
static const ULONGLONG kInputCoalesceMs = 10;
static ULONGLONG g_lastInputTick;
static BYTE g_rawInputBuffer[1024];
static bool g_rawInputRegistered;

// One visible item in the active view: its rectangle (screen coords) and name.
struct CachedItem {
    RECT rect;
    std::wstring name;
};

// Snapshot shared between the worker (producer) and the UI thread (consumer),
// guarded by g_snapshotLock. The UI thread hit-tests it locally; the worker
// rebuilds it off-thread on request.
static CRITICAL_SECTION g_snapshotLock;
static bool g_snapValid;
static HWND g_snapRoot;
static bool g_snapIsDesktop;
static std::vector<CachedItem> g_snapItems;
static LONG g_snapNameColumnRight;  // Right edge of the name column, 0 if none.
static ULONGLONG g_snapBuiltTick;
// Child folder display name (lowercased) -> target absolute pidl. Rebuilt only
// when the folder changes; owned here (freed on rebuild and shutdown).
static std::unordered_map<std::wstring, PIDLIST_ABSOLUTE> g_snapChildren;

// Refresh request, UI thread -> worker (guarded by g_snapshotLock).
static bool g_refreshRequested;
static HWND g_reqRoot;
static bool g_reqIsDesktop;
static POINT g_reqPoint;

static IMenuBand* g_pActiveMenuBand;

template <typename T>
static void SafeRelease(T** pp) {
    if (*pp) {
        (*pp)->Release();
        *pp = nullptr;
    }
}

static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    if (!r.empty()) {
        CharLowerBuffW(&r[0], (DWORD)r.size());
    }
    return r;
}

static bool IsLightTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Dark mode for the pop-up menu (undocumented uxtheme exports). The button is
// drawn by us (theme-aware already); the shell menu band needs the process put
// into "allow dark" mode so it follows the system light/dark setting.

enum PreferredAppMode {
    PAM_Default,
    PAM_AllowDark,
    PAM_ForceDark,
    PAM_ForceLight,
    PAM_Max,
};

static PreferredAppMode(WINAPI* g_pSetPreferredAppMode)(PreferredAppMode);
static void(WINAPI* g_pFlushMenuThemes)();
static void(WINAPI* g_pRefreshImmersiveColorPolicyState)();

// True while a menu should render dark (set when a menu is about to be shown);
// gates the GetSysColor overrides below so they only affect our dark menu.
static bool g_menuDark;

// Forces the process's app mode to match the current system theme. The menu
// band is DirectUI (a separate path from the immersive context menus), so it
// only follows when the whole app is forced, not merely "allowed", dark.
static void RefreshDarkMode() {
    bool light = IsLightTheme();
    g_menuDark = !light;
    if (g_pSetPreferredAppMode) {
        g_pSetPreferredAppMode(light ? PAM_ForceLight : PAM_ForceDark);
    }
    if (g_pRefreshImmersiveColorPolicyState) {
        g_pRefreshImmersiveColorPolicyState();
    }
    if (g_pFlushMenuThemes) {
        g_pFlushMenuThemes();
    }
}

static void InitDarkMode() {
    HMODULE uxtheme =
        LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!uxtheme) {
        return;
    }

    // Ordinals: 135 SetPreferredAppMode (AllowDarkModeForApp on 1809), 104
    // RefreshImmersiveColorPolicyState, 136 FlushMenuThemes.
    g_pSetPreferredAppMode =
        (PreferredAppMode(WINAPI*)(PreferredAppMode))GetProcAddress(
            uxtheme, MAKEINTRESOURCEA(135));
    g_pRefreshImmersiveColorPolicyState =
        (void(WINAPI*)())GetProcAddress(uxtheme, MAKEINTRESOURCEA(104));
    g_pFlushMenuThemes =
        (void(WINAPI*)())GetProcAddress(uxtheme, MAKEINTRESOURCEA(136));

    RefreshDarkMode();
}

// The menu band paints its background and text from system colors, which
// Windows keeps light even in dark mode. We host the menu in our own process,
// so we hook GetSysColor / GetSysColorBrush to return dark equivalents while
// our dark menu is up. Other indices and every call outside the menu pass
// straight through.
struct DarkColorEntry {
    int index;
    COLORREF color;
};

static const DarkColorEntry kDarkColors[] = {
    {COLOR_MENU, RGB(43, 43, 43)},
    {COLOR_WINDOW, RGB(43, 43, 43)},
    {COLOR_BTNFACE, RGB(43, 43, 43)},
    {COLOR_INFOBK, RGB(43, 43, 43)},
    {COLOR_MENUTEXT, RGB(240, 240, 240)},
    {COLOR_WINDOWTEXT, RGB(240, 240, 240)},
    {COLOR_BTNTEXT, RGB(240, 240, 240)},
    {COLOR_INFOTEXT, RGB(240, 240, 240)},
    {COLOR_HIGHLIGHT, RGB(64, 64, 64)},
    {COLOR_MENUHILIGHT, RGB(64, 64, 64)},
    {COLOR_HIGHLIGHTTEXT, RGB(255, 255, 255)},
    {COLOR_GRAYTEXT, RGB(150, 150, 150)},
    {COLOR_3DSHADOW, RGB(60, 60, 60)},
    {COLOR_3DDKSHADOW, RGB(60, 60, 60)},
    {COLOR_3DHILIGHT, RGB(80, 80, 80)},
    {COLOR_3DLIGHT, RGB(80, 80, 80)},
};
static HBRUSH g_darkBrushes[ARRAYSIZE(kDarkColors)];

static int FindDarkColor(int index) {
    for (int i = 0; i < (int)ARRAYSIZE(kDarkColors); i++) {
        if (kDarkColors[i].index == index) {
            return i;
        }
    }
    return -1;
}

using GetSysColor_t = decltype(&GetSysColor);
GetSysColor_t GetSysColor_Orig;
DWORD WINAPI GetSysColor_Hook(int index) {
    if (g_menuActive && g_menuDark) {
        int i = FindDarkColor(index);
        if (i >= 0) {
            return kDarkColors[i].color;
        }
    }
    return GetSysColor_Orig(index);
}

using GetSysColorBrush_t = decltype(&GetSysColorBrush);
GetSysColorBrush_t GetSysColorBrush_Orig;
HBRUSH WINAPI GetSysColorBrush_Hook(int index) {
    if (g_menuActive && g_menuDark) {
        int i = FindDarkColor(index);
        if (i >= 0 && g_darkBrushes[i]) {
            return g_darkBrushes[i];
        }
    }
    return GetSysColorBrush_Orig(index);
}

static void InitMenuColorHooks() {
    for (int i = 0; i < (int)ARRAYSIZE(kDarkColors); i++) {
        g_darkBrushes[i] = CreateSolidBrush(kDarkColors[i].color);
    }
    WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook,
                                   &GetSysColor_Orig);
    WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook,
                                   &GetSysColorBrush_Orig);
}

////////////////////////////////////////////////////////////////////////////////
// UI Automation (worker thread only): enumerate the visible file-list items.

// Walks up from the element under the point to the file list / grid container
// (List, DataGrid or Table). Returns it (caller releases) or nullptr.
static IUIAutomationElement* FindContainerFromPoint(POINT pt) {
    if (!g_workerUia) {
        return nullptr;
    }

    IUIAutomationElement* element = nullptr;
    if (FAILED(g_workerUia->ElementFromPoint(pt, &element)) || !element) {
        return nullptr;
    }

    IUIAutomationTreeWalker* walker = nullptr;
    g_workerUia->get_ControlViewWalker(&walker);

    IUIAutomationElement* container = nullptr;
    IUIAutomationElement* current = element;
    current->AddRef();
    for (int depth = 0; current && depth < 16; depth++) {
        CONTROLTYPEID controlType = 0;
        current->get_CurrentControlType(&controlType);
        if (controlType == UIA_ListControlTypeId ||
            controlType == UIA_DataGridControlTypeId ||
            controlType == UIA_TableControlTypeId) {
            container = current;
            container->AddRef();
            break;
        }

        IUIAutomationElement* parent = nullptr;
        if (!walker || FAILED(walker->GetParentElement(current, &parent)) ||
            !parent) {
            break;
        }
        current->Release();
        current = parent;
    }

    SafeRelease(&current);
    SafeRelease(&walker);
    SafeRelease(&element);
    return container;
}

// In a columned view (Details or List) the row's children are the cells laid
// out side by side. From the first row's cached cells, work out the right edge
// of the name column so the button can be placed next to the file name. Leaves
// *outNameColumnRight at 0 for non-columned views (icons, tiles, content).
static void ComputeNameColumn(IUIAutomationElement* row,
                              const RECT& rowRect,
                              LONG* outNameColumnRight) {
    IUIAutomationElementArray* cells = nullptr;
    if (FAILED(row->GetCachedChildren(&cells)) || !cells) {
        return;
    }

    int count = 0;
    cells->get_Length(&count);
    if (count >= 2) {
        IUIAutomationElement* c0 = nullptr;
        IUIAutomationElement* c1 = nullptr;
        cells->GetElement(0, &c0);
        cells->GetElement(1, &c1);

        RECT r0;
        RECT r1;
        // The first cell must end before the row's right edge, the next cell
        // must begin at or after it (a horizontal column layout), and the first
        // cell must be wider than the row is tall, so a narrow icon-only first
        // cell is not mistaken for the name column.
        if (c0 && c1 && SUCCEEDED(c0->get_CachedBoundingRectangle(&r0)) &&
            SUCCEEDED(c1->get_CachedBoundingRectangle(&r1)) &&
            r0.right > rowRect.left && r0.right < rowRect.right &&
            (r0.right - r0.left) > (rowRect.bottom - rowRect.top) &&
            r1.left >= r0.right - 8) {
            *outNameColumnRight = r0.right;
        }

        SafeRelease(&c0);
        SafeRelease(&c1);
    }

    cells->Release();
}

// Enumerates the active container's visible items (rect + name) in a single UI
// Automation round trip, into the given output vector. Worker thread only.
static void WorkerBuildItems(std::vector<CachedItem>& items,
                             LONG& nameColumnRight) {
    items.clear();
    nameColumnRight = 0;

    if (!g_workerUia || !g_workerContainer) {
        return;
    }

    IUIAutomationCacheRequest* cache = nullptr;
    if (FAILED(g_workerUia->CreateCacheRequest(&cache)) || !cache) {
        return;
    }
    cache->AddProperty(UIA_BoundingRectanglePropertyId);
    cache->AddProperty(UIA_NamePropertyId);
    cache->put_TreeScope((TreeScope)(TreeScope_Element | TreeScope_Children));

    VARIANT vListItem;
    vListItem.vt = VT_I4;
    vListItem.lVal = UIA_ListItemControlTypeId;
    VARIANT vDataItem;
    vDataItem.vt = VT_I4;
    vDataItem.lVal = UIA_DataItemControlTypeId;

    IUIAutomationCondition* condList = nullptr;
    IUIAutomationCondition* condData = nullptr;
    IUIAutomationCondition* cond = nullptr;
    g_workerUia->CreatePropertyCondition(UIA_ControlTypePropertyId, vListItem,
                                         &condList);
    g_workerUia->CreatePropertyCondition(UIA_ControlTypePropertyId, vDataItem,
                                         &condData);
    if (condList && condData) {
        g_workerUia->CreateOrCondition(condList, condData, &cond);
    }

    if (cond) {
        // Descendants (not just children) so grouped views, where items sit
        // under group headers, are still found.
        IUIAutomationElementArray* rows = nullptr;
        if (SUCCEEDED(g_workerContainer->FindAllBuildCache(
                TreeScope_Descendants, cond, cache, &rows)) &&
            rows) {
            int length = 0;
            rows->get_Length(&length);
            bool firstDone = false;
            for (int i = 0; i < length; i++) {
                IUIAutomationElement* row = nullptr;
                if (FAILED(rows->GetElement(i, &row)) || !row) {
                    continue;
                }

                RECT r;
                BSTR name = nullptr;
                if (SUCCEEDED(row->get_CachedBoundingRectangle(&r)) &&
                    SUCCEEDED(row->get_CachedName(&name)) && name && *name) {
                    items.push_back({r, std::wstring(name)});
                    if (!firstDone) {
                        ComputeNameColumn(row, r, &nameColumnRight);
                        firstDone = true;
                    }
                }
                if (name) {
                    SysFreeString(name);
                }
                row->Release();
            }
            rows->Release();
        }
    }

    SafeRelease(&cond);
    SafeRelease(&condData);
    SafeRelease(&condList);
    cache->Release();
}

////////////////////////////////////////////////////////////////////////////////
// Shell: resolve the current folder of an Explorer window and look up a child.

// Finds the active shell view of the given top-level Explorer window and
// returns its current folder as an IShellFolder plus the folder's absolute pidl
// (the caller owns both).
static bool GetFolderForExplorerWindow(HWND root,
                                       IShellFolder** outFolder,
                                       PIDLIST_ABSOLUTE* outFolderAbs) {
    *outFolder = nullptr;
    *outFolderAbs = nullptr;

    IShellWindows* shellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&shellWindows))) ||
        !shellWindows) {
        return false;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    bool result = false;
    for (long i = 0; i < count && !result; i++) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;

        IDispatch* dispatch = nullptr;
        if (FAILED(shellWindows->Item(v, &dispatch)) || !dispatch) {
            VariantClear(&v);
            continue;
        }
        VariantClear(&v);

        IServiceProvider* serviceProvider = nullptr;
        HRESULT hr = dispatch->QueryInterface(IID_PPV_ARGS(&serviceProvider));
        SafeRelease(&dispatch);
        if (FAILED(hr) || !serviceProvider) {
            continue;
        }

        IShellBrowser* browser = nullptr;
        hr = serviceProvider->QueryService(SID_STopLevelBrowser,
                                           IID_PPV_ARGS(&browser));
        SafeRelease(&serviceProvider);
        if (FAILED(hr) || !browser) {
            continue;
        }

        IShellView* view = nullptr;
        hr = browser->QueryActiveShellView(&view);
        SafeRelease(&browser);
        if (FAILED(hr) || !view) {
            continue;
        }

        HWND viewHwnd = nullptr;
        if (FAILED(view->GetWindow(&viewHwnd)) || !viewHwnd ||
            GetAncestor(viewHwnd, GA_ROOT) != root) {
            SafeRelease(&view);
            continue;
        }

        IFolderView* folderView = nullptr;
        if (SUCCEEDED(view->QueryInterface(IID_PPV_ARGS(&folderView))) &&
            folderView) {
            IPersistFolder2* persistFolder = nullptr;
            if (SUCCEEDED(
                    folderView->GetFolder(IID_PPV_ARGS(&persistFolder))) &&
                persistFolder) {
                PIDLIST_ABSOLUTE folderAbs = nullptr;
                if (SUCCEEDED(persistFolder->GetCurFolder(&folderAbs)) &&
                    folderAbs) {
                    IShellFolder* desktop = nullptr;
                    if (SUCCEEDED(SHGetDesktopFolder(&desktop)) && desktop) {
                        IShellFolder* folder = nullptr;
                        if (SUCCEEDED(desktop->BindToObject(
                                folderAbs, nullptr, IID_PPV_ARGS(&folder))) &&
                            folder) {
                            *outFolder = folder;
                            *outFolderAbs = folderAbs;
                            folderAbs = nullptr;
                            result = true;
                        }
                        desktop->Release();
                    }
                    if (folderAbs) {
                        ILFree(folderAbs);
                    }
                }
                persistFolder->Release();
            }
            folderView->Release();
        }

        SafeRelease(&view);
    }

    shellWindows->Release();
    return result;
}

static bool IsFolderPidl(PCIDLIST_ABSOLUTE pidl) {
    IShellItem* item = nullptr;
    bool isFolder = false;
    if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) && item) {
        SFGAOF attrs = 0;
        if (SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, &attrs)) &&
            (attrs & SFGAO_FOLDER)) {
            isFolder = true;
        }
        item->Release();
    }
    return isFolder;
}

// If `child` (a shortcut item in `folder`) points to a folder, returns the
// target's absolute pidl (caller frees); otherwise nullptr. Only stored target
// data is read (no link resolution / network access), so it cannot stall.
static PIDLIST_ABSOLUTE ResolveFolderShortcut(IShellFolder* folder,
                                              LPCITEMIDLIST child) {
    IShellLinkW* link = nullptr;
    if (FAILED(folder->GetUIObjectOf(nullptr, 1, &child, IID_IShellLinkW,
                                     nullptr, (void**)&link)) ||
        !link) {
        return nullptr;
    }

    PIDLIST_ABSOLUTE result = nullptr;
    PIDLIST_ABSOLUTE target = nullptr;
    if (SUCCEEDED(link->GetIDList(&target)) && target) {
        if (IsFolderPidl(target)) {
            result = target;
            target = nullptr;
        }
    }
    if (!result) {
        // Fall back to the link's raw stored path.
        WCHAR path[MAX_PATH];
        if (SUCCEEDED(
                link->GetPath(path, ARRAYSIZE(path), nullptr, SLGP_RAWPATH)) &&
            *path) {
            PIDLIST_ABSOLUTE pathPidl = nullptr;
            if (SUCCEEDED(
                    SHParseDisplayName(path, nullptr, &pathPidl, 0, nullptr)) &&
                pathPidl) {
                if (IsFolderPidl(pathPidl)) {
                    result = pathPidl;
                } else {
                    ILFree(pathPidl);
                }
            }
        }
    }
    if (target) {
        ILFree(target);
    }
    link->Release();
    return result;
}

// Enumerates the folder's children into a name -> target-pidl map. Real
// sub-folders map to their own pidl; shortcuts to a folder map to the target's.
// The map owns its pidls. Worker thread only.
static void WorkerBuildChildren(
    IShellFolder* folder,
    PCIDLIST_ABSOLUTE folderAbs,
    bool isDesktop,
    std::unordered_map<std::wstring, PIDLIST_ABSOLUTE>& out) {
    // Non-folders are enumerated too, so shortcuts (.lnk files) that point to a
    // folder can be picked up.
    IEnumIDList* enumerator = nullptr;
    if (FAILED(folder->EnumObjects(
            nullptr,
            SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN,
            &enumerator)) ||
        !enumerator) {
        return;
    }

    LPITEMIDLIST child = nullptr;
    ULONG fetched = 0;
    while (enumerator->Next(1, &child, &fetched) == S_OK && fetched == 1) {
        LPCITEMIDLIST childConst = child;
        SFGAOF attrs = SFGAO_FOLDER | SFGAO_LINK;
        if (FAILED(folder->GetAttributesOf(1, &childConst, &attrs))) {
            attrs = 0;
        }

        PIDLIST_ABSOLUTE childAbs = nullptr;
        if (attrs & SFGAO_FOLDER) {
            // For the desktop, folderAbs is the root, so the child relative
            // pidl already is the absolute pidl. ILCombine(nullptr, child)
            // clones it.
            childAbs = ILCombine(isDesktop ? nullptr : folderAbs, child);
        } else if (attrs & SFGAO_LINK) {
            childAbs = ResolveFolderShortcut(folder, child);
        }

        if (childAbs) {
            std::wstring keys[2];
            int keyCount = 0;

            STRRET strret;
            WCHAR name[MAX_PATH];
            if (SUCCEEDED(
                    folder->GetDisplayNameOf(child, SHGDN_NORMAL, &strret)) &&
                SUCCEEDED(
                    StrRetToBufW(&strret, child, name, ARRAYSIZE(name))) &&
                *name) {
                keys[keyCount++] = ToLower(name);
            }
            // Shortcuts can be shown with their .lnk extension, so also key by
            // the in-folder parsing name to match the UIA item name in that
            // case.
            if ((attrs & SFGAO_LINK) &&
                SUCCEEDED(folder->GetDisplayNameOf(
                    child, SHGDN_INFOLDER | SHGDN_FORPARSING, &strret)) &&
                SUCCEEDED(
                    StrRetToBufW(&strret, child, name, ARRAYSIZE(name))) &&
                *name) {
                std::wstring key = ToLower(name);
                if (keyCount == 0 || key != keys[0]) {
                    keys[keyCount++] = key;
                }
            }

            bool stored = false;
            for (int k = 0; k < keyCount; k++) {
                if (out.find(keys[k]) != out.end()) {
                    continue;
                }
                PIDLIST_ABSOLUTE p = stored ? ILClone(childAbs) : childAbs;
                if (!p) {
                    continue;
                }
                out[keys[k]] = p;
                stored = true;
            }
            if (!stored) {
                ILFree(childAbs);
            }
        }

        CoTaskMemFree(child);
        child = nullptr;
    }

    enumerator->Release();
}

////////////////////////////////////////////////////////////////////////////////
// Worker thread: produces snapshots off the UI thread.

static bool EnsureWorkerUia() {
    if (g_workerUia) {
        return true;
    }
    HRESULT hr =
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&g_workerUia));
    if (FAILED(hr) || !g_workerUia) {
        g_workerUia = nullptr;
        Wh_Log(L"worker CoCreateInstance(CUIAutomation) failed, hr=0x%08X", hr);
        return false;
    }
    return true;
}

// Builds a fresh snapshot for the given window/point and installs it for the UI
// thread to hit-test. The expensive UIA + shell work happens here, off the UI
// thread; only the brief install is done under the lock.
static void WorkerBuildSnapshot(HWND root, bool isDesktop, POINT pt) {
    ULONGLONG tick = GetTickCount64();

    IShellFolder* folder = nullptr;
    PIDLIST_ABSOLUTE folderAbs = nullptr;
    if (isDesktop) {
        if (FAILED(SHGetDesktopFolder(&folder))) {
            folder = nullptr;
        }
    } else if (!GetFolderForExplorerWindow(root, &folder, &folderAbs)) {
        folder = nullptr;
    }

    if (!folder) {
        // Install an empty snapshot so the UI throttles instead of
        // re-requesting on every move.
        EnterCriticalSection(&g_snapshotLock);
        g_snapItems.clear();
        g_snapNameColumnRight = 0;
        g_snapRoot = root;
        g_snapIsDesktop = isDesktop;
        g_snapBuiltTick = tick;
        g_snapValid = true;
        LeaveCriticalSection(&g_snapshotLock);
        return;
    }

    bool folderChanged =
        isDesktop ? (!g_workerChildrenValid || !g_workerFolderIsDesktop)
                  : (!g_workerChildrenValid || g_workerFolderIsDesktop ||
                     !g_workerFolderAbs || !folderAbs ||
                     !ILIsEqual(g_workerFolderAbs, folderAbs));

    // (Re)acquire the list element when the window changed.
    if (root != g_workerContainerRoot || !g_workerContainer) {
        IUIAutomationElement* container = FindContainerFromPoint(pt);
        SafeRelease(&g_workerContainer);
        g_workerContainer = container;
        g_workerContainerRoot = container ? root : nullptr;
    }

    std::vector<CachedItem> items;
    LONG nameColumnRight = 0;
    WorkerBuildItems(items, nameColumnRight);

    // An empty rebuild over a populated view means the element went stale
    // (navigation, re-sort, view-mode change): re-acquire it and try once more.
    if (items.empty()) {
        IUIAutomationElement* container = FindContainerFromPoint(pt);
        if (container) {
            SafeRelease(&g_workerContainer);
            g_workerContainer = container;
            g_workerContainerRoot = root;
            WorkerBuildItems(items, nameColumnRight);
        }
    }

    std::unordered_map<std::wstring, PIDLIST_ABSOLUTE> children;
    bool rebuiltChildren = false;
    if (folderChanged) {
        WorkerBuildChildren(folder, folderAbs, isDesktop, children);
        rebuiltChildren = true;
    }

    folder->Release();

    // Swap the new data in under the lock (O(1)); the old data ends up in the
    // local temporaries and is destroyed/freed below, outside the lock.
    EnterCriticalSection(&g_snapshotLock);
    g_snapItems.swap(items);
    g_snapNameColumnRight = nameColumnRight;
    g_snapRoot = root;
    g_snapIsDesktop = isDesktop;
    g_snapBuiltTick = tick;
    g_snapValid = true;
    if (rebuiltChildren) {
        g_snapChildren.swap(children);
    }
    LeaveCriticalSection(&g_snapshotLock);

    if (rebuiltChildren) {
        for (auto& entry : children) {  // The previous folder's child pidls.
            ILFree(entry.second);
        }
        if (g_workerFolderAbs) {
            ILFree(g_workerFolderAbs);
            g_workerFolderAbs = nullptr;
        }
        g_workerFolderAbs =
            (folderAbs && !isDesktop) ? ILClone(folderAbs) : nullptr;
        g_workerFolderIsDesktop = isDesktop;
        g_workerChildrenValid = true;
    }

    if (folderAbs) {
        ILFree(folderAbs);
    }
}

static DWORD WINAPI WorkerThreadProc(LPVOID param) {
    // Match the UI thread's physical-pixel coordinate space.
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pSetThreadDpiAwarenessContext =
            (void*(WINAPI*)(void*))GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (pSetThreadDpiAwarenessContext) {
            pSetThreadDpiAwarenessContext((void*)(LONG_PTR)-4);
        }
    }

    OleInitialize(nullptr);
    EnsureWorkerUia();

    // Force the thread message queue to exist before signaling ready, so the UI
    // thread's PostThreadMessage requests are never lost.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    SetEvent(g_workerReadyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_QUIT) {
            PostQuitMessage(0);
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_DO_REFRESH) {
            HWND root = nullptr;
            bool isDesktop = false;
            POINT pt = {};
            bool have = false;
            EnterCriticalSection(&g_snapshotLock);
            if (g_refreshRequested) {
                root = g_reqRoot;
                isDesktop = g_reqIsDesktop;
                pt = g_reqPoint;
                g_refreshRequested = false;
                have = true;
            }
            LeaveCriticalSection(&g_snapshotLock);
            if (have) {
                WorkerBuildSnapshot(root, isDesktop, pt);
                if (g_sinkWnd) {
                    PostMessageW(g_sinkWnd, WM_APP_REFRESH_DONE, 0, 0);
                }
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SafeRelease(&g_workerContainer);
    SafeRelease(&g_workerUia);
    if (g_workerFolderAbs) {
        ILFree(g_workerFolderAbs);
        g_workerFolderAbs = nullptr;
    }
    OleUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// The cascading folder menu (adapted from folder_menu.c / "Quick Folder Menu").

// Tears the menu band down. Used both when another window takes the foreground
// and when we cannot bring the popup to the foreground ourselves.
static void CloseMenuBand(IMenuBand* band) {
    IOleCommandTarget* commandTarget = nullptr;
    if (SUCCEEDED(band->QueryInterface(IID_PPV_ARGS(&commandTarget)))) {
        commandTarget->Exec(&CLSID_MenuBand, MBAND_CMDID_CLOSE, 0, nullptr,
                            nullptr);
        commandTarget->Release();
    }
}

// The top-level window hosting the menu band, or nullptr.
static HWND GetMenuBandWindow(IMenuBand* band) {
    IOleWindow* oleWindow = nullptr;
    HWND hwnd = nullptr;
    if (SUCCEEDED(band->QueryInterface(IID_PPV_ARGS(&oleWindow))) &&
        oleWindow) {
        oleWindow->GetWindow(&hwnd);
        oleWindow->Release();
    }
    return hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
}

static VOID CALLBACK ForegroundChangedProc(HWINEVENTHOOK hook,
                                           DWORD event,
                                           HWND hwnd,
                                           LONG idObject,
                                           LONG idChild,
                                           DWORD idEventThread,
                                           DWORD dwmsEventTime) {
    IMenuBand* band = g_pActiveMenuBand;
    if (band) {
        CloseMenuBand(band);
    }
}

// Hosts the folder's contents in a menu band inside a menu desk bar and pops it
// up next to the anchor rect (the expand button). Returns the live IMenuBand
// (caller releases) or nullptr on failure.
static IMenuBand* PopupFolderMenu(PCIDLIST_ABSOLUTE pidlAbs, RECT anchorRect) {
    IShellMenu* shellMenu = nullptr;
    IDeskBand* deskBand = nullptr;
    IMenuBand* menuBand = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_MenuBand, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&shellMenu));
    if (SUCCEEDED(hr)) {
        hr = shellMenu->Initialize(nullptr, -1, ANCESTORDEFAULT,
                                   SMINIT_TOPLEVEL | SMINIT_VERTICAL);

        IShellFolder* desktop = nullptr;
        if (SUCCEEDED(hr)) {
            hr = SHGetDesktopFolder(&desktop);
        }

        if (SUCCEEDED(hr)) {
            IShellFolder* folder = nullptr;
            hr = desktop->BindToObject(pidlAbs, nullptr, IID_PPV_ARGS(&folder));
            if (SUCCEEDED(hr)) {
                hr = shellMenu->SetShellFolder(
                    folder, pidlAbs, nullptr,
                    SMSET_BOTTOM | SMSET_USEBKICONEXTRACTION);
                if (SUCCEEDED(hr)) {
                    hr = shellMenu->QueryInterface(IID_PPV_ARGS(&deskBand));
                }
                folder->Release();
            }
            desktop->Release();
        }

        shellMenu->Release();
    }

    if (deskBand) {
        IUnknown* deskBarUnk = nullptr;
        hr = CoCreateInstance(CLSID_MenuDeskBar, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&deskBarUnk));
        if (SUCCEEDED(hr)) {
            IMenuPopup* menuPopup = nullptr;
            hr = deskBarUnk->QueryInterface(IID_PPV_ARGS(&menuPopup));
            if (SUCCEEDED(hr)) {
                IBandSite* bandSite = nullptr;
                hr = CoCreateInstance(CLSID_MenuBandSite, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(&bandSite));
                if (SUCCEEDED(hr)) {
                    hr = menuPopup->SetClient(bandSite);
                    if (SUCCEEDED(hr)) {
                        hr = bandSite->AddBand(deskBand);
                    }

                    if (SUCCEEDED(hr)) {
                        // Anchor the menu to the button's right edge and pass
                        // its rect as the exclude region so the menu opens
                        // alongside the button without covering it.
                        POINTL pt = {anchorRect.right, anchorRect.top};
                        RECTL exclude = {anchorRect.left, anchorRect.top,
                                         anchorRect.right, anchorRect.bottom};

                        hr = menuPopup->Popup(&pt, &exclude, MPPF_SETFOCUS);
                        if (SUCCEEDED(hr)) {
                            hr = deskBand->QueryInterface(
                                IID_PPV_ARGS(&menuBand));
                        }
                    }

                    bandSite->Release();
                }
                menuPopup->Release();
            }
            deskBarUnk->Release();
        }

        // The desk bar's window holds an internal self-reference while the
        // popup is shown, so the menu stays alive after we drop this reference.
        deskBand->Release();
    }

    if (SUCCEEDED(hr) && menuBand) {
        return menuBand;
    }

    SafeRelease(&menuBand);
    return nullptr;
}

// Shows the folder menu and pumps a nested message loop until it is dismissed.
static void ShowFolderMenuModal(PCIDLIST_ABSOLUTE pidlAbs, RECT anchorRect) {
    g_menuActive = true;

    // Pick up the current system theme in case it changed since startup.
    RefreshDarkMode();

    IMenuBand* band = PopupFolderMenu(pidlAbs, anchorRect);
    if (band) {
        // The popup belongs to our background tool process, so the menu band's
        // own focus attempt can lose to the foreground lock and leave the menu
        // on screen with no way to dismiss it. Claim the foreground explicitly;
        // if that fails, tear the popup down instead of letting it linger.
        HWND hwndMenu = GetMenuBandWindow(band);
        if (!hwndMenu || !SetForegroundWindow(hwndMenu)) {
            CloseMenuBand(band);
            band->Release();
            g_menuActive = false;
            return;
        }

        g_pActiveMenuBand = band;

        HWINEVENTHOOK hook =
            SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                            nullptr, ForegroundChangedProc, 0, 0,
                            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

        bool done = false;
        while (!done) {
            MSG msg;
            BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);
            if (bRet == 0) {
                // WM_QUIT. Re-post it so the outer loop also exits.
                PostQuitMessage((int)msg.wParam);
                break;
            }
            if (bRet == -1) {
                break;
            }
            if (msg.hwnd == nullptr && msg.message == WM_APP_QUIT) {
                // Uninit while the menu is open: turn it into a real quit and
                // leave the menu loop so the outer loop can exit too.
                PostQuitMessage(0);
                break;
            }
            if (msg.hwnd == nullptr && msg.message == WM_APP_SETTINGS_CHANGED) {
                // The outer loop won't see this null-hwnd thread message once
                // it is consumed here, so apply it now.
                LoadSettings();
                continue;
            }

            LRESULT lr;
            switch (band->IsMenuMessage(&msg)) {
                case S_OK:
                    band->TranslateMenuMessage(&msg, &lr);
                    break;
                case E_FAIL:
                    done = true;
                    break;
                default:
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    break;
            }
        }

        if (hook) {
            UnhookWinEvent(hook);
        }

        g_pActiveMenuBand = nullptr;
        band->Release();
    }

    // Deferred work posted by the menu band (e.g. launching an item) runs once
    // we return to the outer message loop, which keeps pumping.
    g_menuActive = false;
}

////////////////////////////////////////////////////////////////////////////////
// The expand-button overlay window.

// Effective DPI of the monitor that contains the given rectangle.
static UINT GetDpiForRect(const RECT& rc) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitor_t pGetDpiForMonitor = []() -> GetDpiForMonitor_t {
        HMODULE shcore = LoadLibraryExW(L"shcore.dll", nullptr,
                                        LOAD_LIBRARY_SEARCH_SYSTEM32);
        return shcore ? (GetDpiForMonitor_t)GetProcAddress(shcore,
                                                           "GetDpiForMonitor")
                      : nullptr;
    }();

    POINT center = {(rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2};
    HMONITOR monitor = MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (pGetDpiForMonitor &&
        SUCCEEDED(pGetDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX,
                                    &dpiY))) {
        return dpiX;
    }
    return 96;
}

static void AddRoundedRectPath(Gdiplus::GraphicsPath& path,
                               const Gdiplus::RectF& rc,
                               float radius) {
    float d = radius * 2.0f;
    path.AddArc(rc.X, rc.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rc.GetRight() - d, rc.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rc.GetRight() - d, rc.GetBottom() - d, d, d, 0.0f, 90.0f);
    path.AddArc(rc.X, rc.GetBottom() - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

// Renders the WinUI3-style button into a per-pixel-alpha layered window: an
// antialiased rounded surface with a subtle border and a smooth chevron glyph.
static void RenderChevron(HWND hwnd, int x, int y, int size) {
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;  // Top-down.
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib =
        CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, dib);

    bool light = IsLightTheme();
    Gdiplus::Color fill = light ? Gdiplus::Color(250, 252, 252, 252)
                                : Gdiplus::Color(245, 50, 50, 50);
    Gdiplus::Color border = light ? Gdiplus::Color(150, 0, 0, 0)
                                  : Gdiplus::Color(120, 255, 255, 255);
    Gdiplus::Color glyph = light ? Gdiplus::Color(235, 20, 20, 20)
                                 : Gdiplus::Color(245, 240, 240, 240);

    {
        // Wrap the DIB's bits in a premultiplied GDI+ bitmap so antialiased
        // edges blend correctly through UpdateLayeredWindow.
        Gdiplus::Bitmap bitmap(size, size, size * 4, PixelFormat32bppPARGB,
                               (BYTE*)bits);
        Gdiplus::Graphics g(&bitmap);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        float inset = 0.75f;
        Gdiplus::RectF surface(inset, inset, size - inset * 2.0f,
                               size - inset * 2.0f);
        Gdiplus::GraphicsPath path;
        AddRoundedRectPath(path, surface, size * 0.25f);

        Gdiplus::SolidBrush fillBrush(fill);
        g.FillPath(&fillBrush, &path);
        Gdiplus::Pen borderPen(border, 1.0f);
        g.DrawPath(&borderPen, &path);

        float cx = size / 2.0f;
        float cy = size / 2.0f;
        float ext = size * 0.17f;
        float thickness = size / 9.0f;
        if (thickness < 1.4f) {
            thickness = 1.4f;
        }
        Gdiplus::Pen glyphPen(glyph, thickness);
        glyphPen.SetStartCap(Gdiplus::LineCapRound);
        glyphPen.SetEndCap(Gdiplus::LineCapRound);
        glyphPen.SetLineJoin(Gdiplus::LineJoinRound);
        Gdiplus::PointF pts[3] = {{cx - ext * 0.55f, cy - ext},
                                  {cx + ext * 0.55f, cy},
                                  {cx - ext * 0.55f, cy + ext}};
        g.DrawLines(&glyphPen, pts, 3);
    }

    POINT dst = {x, y};
    SIZE sz = {size, size};
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, screenDC, &dst, &sz, memDC, &src, 0, &blend,
                        ULW_ALPHA);

    SelectObject(memDC, oldBmp);
    DeleteObject(dib);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}

static void HideChevron() {
    if (g_chevronVisible) {
        ShowWindow(g_chevronWnd, SW_HIDE);
        g_chevronVisible = false;
    }
    SetRectEmpty(&g_hoverItemRect);
    SetRectEmpty(&g_chevronRect);
}

// Takes ownership of childAbs.
static void ShowChevronForItem(PIDLIST_ABSOLUTE childAbs, RECT itemRect) {
    if (g_targetPidl) {
        ILFree(g_targetPidl);
    }
    g_targetPidl = childAbs;
    g_hoverItemRect = itemRect;

    UINT dpi = GetDpiForRect(itemRect);
    int size = MulDiv(g_iconSize, dpi, 96);
    int margin = MulDiv(2, dpi, 96);
    int x = itemRect.right - size - margin;
    int y = itemRect.bottom - size - margin;
    if (x < itemRect.left) {
        x = itemRect.left;
    }
    if (y < itemRect.top) {
        y = itemRect.top;
    }
    SetRect(&g_chevronRect, x, y, x + size, y + size);

    // UpdateLayeredWindow (inside RenderChevron) sets the position, size, and
    // per-pixel-alpha content; SetWindowPos only asserts top-most and shows it.
    RenderChevron(g_chevronWnd, x, y, size);
    SetWindowPos(g_chevronWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    g_chevronVisible = true;
}

static LRESULT CALLBACK ChevronWndProc(HWND hwnd,
                                       UINT msg,
                                       WPARAM wParam,
                                       LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            if (g_targetPidl && !g_menuActive) {
                // Leave the button on screen while the menu is open; the hover
                // engine is frozen (g_menuActive) so it stays put until
                // dismissed. Anchor the menu to the button itself, not the
                // folder row.
                RECT anchorRect = g_chevronRect;
                PIDLIST_ABSOLUTE pidl = ILClone(g_targetPidl);
                if (pidl) {
                    ShowFolderMenuModal(pidl, anchorRect);
                    ILFree(pidl);
                }
            }
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Hover engine: raw input driven, hit-testing against the cached item rects.

static bool IsExplorerOrDesktopRoot(HWND root, bool* outIsDesktop) {
    WCHAR className[64];
    if (!GetClassNameW(root, className, ARRAYSIZE(className))) {
        return false;
    }

    if (wcscmp(className, L"CabinetWClass") == 0 ||
        wcscmp(className, L"ExploreWClass") == 0) {
        *outIsDesktop = false;
        return true;
    }

    if (wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0) {
        *outIsDesktop = true;
        return true;
    }

    return false;
}

// True if hwnd or one of its ancestors (up to maxDepth) has the given class.
static bool IsWithinClass(HWND hwnd, PCWSTR className, int maxDepth) {
    for (int i = 0; hwnd && i < maxDepth; i++) {
        WCHAR c[64];
        if (GetClassNameW(hwnd, c, ARRAYSIZE(c)) && wcscmp(c, className) == 0) {
            return true;
        }
        hwnd = GetParent(hwnd);
    }
    return false;
}

// The heart of the mod: hit-test the cursor against the current snapshot and
// show or hide the button. All UIA/shell work is delegated to the worker
// thread; this only ever does a local scan + render, so it never blocks. Called
// from raw input (mouse move / wheel), foreground changes, and worker
// refresh-done messages.
static void Evaluate(bool wheel) {
    if (g_menuActive || !g_active) {
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    // Keep the button while the cursor rests on it or on the hovered item. A
    // wheel scroll bypasses this so the (now moved) item is re-evaluated.
    if (!wheel && g_chevronVisible &&
        (PtInRect(&g_chevronRect, pt) || PtInRect(&g_hoverItemRect, pt))) {
        return;
    }

    // Cheap window-level gate: only consider the file list area.
    HWND under = WindowFromPoint(pt);
    HWND root = under ? GetAncestor(under, GA_ROOT) : nullptr;
    bool isDesktop = false;
    if (!under || !root || !IsExplorerOrDesktopRoot(root, &isDesktop) ||
        !IsWithinClass(under, L"SHELLDLL_DefView", 8)) {
        HideChevron();
        return;
    }

    PIDLIST_ABSOLUTE childAbs = nullptr;
    RECT hitRect = {};
    bool requestRefresh = false;

    EnterCriticalSection(&g_snapshotLock);
    bool snapMatches =
        g_snapValid && g_snapRoot == root && g_snapIsDesktop == isDesktop;
    if (snapMatches) {
        for (const CachedItem& item : g_snapItems) {
            RECT r = item.rect;
            if (g_snapNameColumnRight && g_snapNameColumnRight > r.left &&
                g_snapNameColumnRight < r.right) {
                r.right = g_snapNameColumnRight;
            }
            if (PtInRect(&r, pt)) {
                auto it = g_snapChildren.find(ToLower(item.name));
                if (it != g_snapChildren.end()) {
                    childAbs = ILClone(it->second);
                    hitRect = r;
                }
                break;
            }
        }
        if (wheel || (GetTickCount64() - g_snapBuiltTick) > kRefreshTtlMs) {
            requestRefresh = true;
        }
    } else {
        requestRefresh = true;
    }

    if (requestRefresh) {
        g_reqRoot = root;
        g_reqIsDesktop = isDesktop;
        g_reqPoint = pt;
        g_refreshRequested = true;
    }
    LeaveCriticalSection(&g_snapshotLock);

    if (requestRefresh && g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP_DO_REFRESH, 0, 0);
    }

    if (childAbs) {
        ShowChevronForItem(childAbs, hitRect);
    } else {
        HideChevron();
    }
}

// Subscribes to or unsubscribes from raw mouse input. While Explorer is not in
// the foreground we unregister entirely, so no WM_INPUT is delivered and the
// mod does no input handling at all.
static void SetRawInputActive(bool enable) {
    if (enable == g_rawInputRegistered || !g_sinkWnd) {
        return;
    }
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01;  // Generic desktop controls.
    rid.usUsage = 0x02;      // Mouse.
    rid.dwFlags = enable ? RIDEV_INPUTSINK : RIDEV_REMOVE;
    rid.hwndTarget = enable ? g_sinkWnd : nullptr;
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        g_rawInputRegistered = enable;
    }
}

// Tracks whether Explorer / the desktop is the foreground window, so raw input
// is only acted on then. Also re-checks the hover when Explorer is activated.
static VOID CALLBACK ActivationChangedProc(HWINEVENTHOOK hook,
                                           DWORD event,
                                           HWND hwnd,
                                           LONG idObject,
                                           LONG idChild,
                                           DWORD idEventThread,
                                           DWORD dwmsEventTime) {
    if (g_menuActive) {
        return;
    }

    // Ignore our own windows (the button or the popup) taking the foreground;
    // they are part of the mod's UI, so a failed/closing popup must not look
    // like a switch away from Explorer and deactivate the mod.
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == GetCurrentProcessId()) {
        return;
    }

    bool isDesktop = false;
    g_active = hwnd && IsExplorerOrDesktopRoot(hwnd, &isDesktop);
    SetRawInputActive(g_active);
    if (!g_active) {
        HideChevron();
        // Invalidate the snapshot so re-activation rebuilds from scratch.
        EnterCriticalSection(&g_snapshotLock);
        g_snapValid = false;
        LeaveCriticalSection(&g_snapshotLock);
    } else {
        Evaluate(false);
    }
}

static LRESULT CALLBACK SinkWndProc(HWND hwnd,
                                    UINT msg,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    if (msg == WM_APP_REFRESH_DONE) {
        // The worker installed a fresh snapshot; re-evaluate at the cursor.
        Evaluate(false);
        return 0;
    }

    if (msg == WM_INPUT) {
        UINT size = 0;
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size,
                            sizeof(RAWINPUTHEADER)) == 0 &&
            size > 0 && size <= sizeof(g_rawInputBuffer) &&
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, g_rawInputBuffer,
                            &size, sizeof(RAWINPUTHEADER)) == size) {
            RAWINPUT* raw = (RAWINPUT*)g_rawInputBuffer;
            if (raw->header.dwType == RIM_TYPEMOUSE) {
                bool wheel = (raw->data.mouse.usButtonFlags &
                              (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL)) != 0;
                ULONGLONG now = GetTickCount64();
                if (wheel || now - g_lastInputTick >= kInputCoalesceMs) {
                    g_lastInputTick = now;
                    Evaluate(wheel);
                }
            }
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// UI thread and tool-mod lifecycle.

static DWORD WINAPI UiThreadProc(LPVOID param) {
    // Match Explorer's physical-pixel coordinate space so UI Automation
    // rectangles, the cursor, and our window all line up under mixed DPI.
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pSetThreadDpiAwarenessContext =
            (void*(WINAPI*)(void*))GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (pSetThreadDpiAwarenessContext) {
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2.
            pSetThreadDpiAwarenessContext((void*)(LONG_PTR)-4);
        }
    }

    OleInitialize(nullptr);
    InitDarkMode();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    WNDCLASSEXW wcChevron = {sizeof(wcChevron)};
    wcChevron.lpfnWndProc = ChevronWndProc;
    wcChevron.hInstance = g_hInst;
    wcChevron.hCursor = LoadCursorW(nullptr, IDC_HAND);
    wcChevron.lpszClassName = L"WH_FolderHoverChevron";
    RegisterClassExW(&wcChevron);

    WNDCLASSEXW wcSink = {sizeof(wcSink)};
    wcSink.lpfnWndProc = SinkWndProc;
    wcSink.hInstance = g_hInst;
    wcSink.lpszClassName = L"WH_FolderHoverSink";
    RegisterClassExW(&wcSink);

    // Per-pixel alpha (WS_EX_LAYERED + UpdateLayeredWindow) for an antialiased,
    // rounded button; content is supplied by RenderChevron.
    g_chevronWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wcChevron.lpszClassName, L"", WS_POPUP, 0, 0, g_iconSize, g_iconSize,
        nullptr, nullptr, g_hInst, nullptr);

    // Hidden window that receives raw mouse input (movement + wheel) even when
    // in the background, replacing any polling. Raw input is only registered
    // while Explorer/the desktop is the foreground window (see
    // SetRawInputActive).
    g_sinkWnd =
        CreateWindowExW(WS_EX_TOOLWINDOW, wcSink.lpszClassName, L"", WS_POPUP,
                        0, 0, 0, 0, nullptr, nullptr, g_hInst, nullptr);

    // Start the worker thread that does all UIA + shell work off this thread.
    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_workerThreadId);
    if (g_workerThread && g_workerReadyEvent) {
        WaitForSingleObject(g_workerReadyEvent, 5000);
    }
    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }

    bool isDesktop = false;
    g_active = IsExplorerOrDesktopRoot(GetForegroundWindow(), &isDesktop);
    SetRawInputActive(g_active);
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ActivationChangedProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    SetEvent(g_readyEvent);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_QUIT) {
            PostQuitMessage(0);
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_SETTINGS_CHANGED) {
            LoadSettings();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
    SetRawInputActive(false);

    // Stop the worker before tearing down windows it posts to.
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP_QUIT, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }

    // The worker is gone; free the snapshot's child pidls it owned.
    for (auto& entry : g_snapChildren) {
        ILFree(entry.second);
    }
    g_snapChildren.clear();

    if (g_sinkWnd) {
        DestroyWindow(g_sinkWnd);
        g_sinkWnd = nullptr;
    }
    if (g_chevronWnd) {
        DestroyWindow(g_chevronWnd);
        g_chevronWnd = nullptr;
    }
    if (g_targetPidl) {
        ILFree(g_targetPidl);
        g_targetPidl = nullptr;
    }
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    OleUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_hInst = GetModuleHandleW(nullptr);

    LoadSettings();

    InitMenuColorHooks();

    InitializeCriticalSection(&g_snapshotLock);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_uiThread =
        CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    if (!g_uiThread) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        DeleteCriticalSection(&g_snapshotLock);
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, 5000);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_APP_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        g_uiThreadId = 0;
    }

    DeleteCriticalSection(&g_snapshotLock);
}
////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
