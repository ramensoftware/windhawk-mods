// ==WindhawkMod==
// @id              finder-explorer
// @name            Finder-style Explorer (column view)
// @description     macOS Finder-like column view in the List and Details modes of File Explorer, with rounded selection, file preview and a cleaner sidebar
// @version         1.0.0
// @author          Rafael Mendes
// @github          https://github.com/RafaelMendes1407
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -lshell32 -lshlwapi -lcomctl32 -lgdi32 -luxtheme -lmsimg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Finder-style Explorer (column view)

Brings the macOS Finder column view to the Windows 11 File Explorer, with a
look inspired by macOS.

## Column view
Choose **View > List** or **View > Details** in Explorer. The folder shows up
normally, taking the whole area; when you select a folder inside it, the
view switches to columns:
- Clicking a folder opens its contents in a new column to the right.
- **Esc** closes the columns and the folder takes the whole area again.
- Each level takes a column, from left to right. Clicking an empty space of
  an earlier column doesn't close anything; only selecting another item in it
  replaces the columns that came after.
- Each column scrolls independently; drag the divider between two columns to
  change the width.
- Selecting a file shows its preview (thumbnail, type, size and dates) in the
  last column.
- In Details mode, each row also shows the modification date and the size of
  the files.
- The address bar stays on the folder where the columns started. For
  Explorer's commands (New, Paste...) to apply to the last column, turn on
  "Address bar follows the last column" in the settings, or use the context
  menu and Ctrl+V inside the column.

To leave the columns, choose another view mode (Details, Icons...).

### Keyboard and mouse
- **Up and down arrows** select; **right arrow** enters the folder; **left
  arrow** or **Backspace** go back one column.
- **Enter** or double click open the file.
- **Ctrl+Shift+N** creates a folder in the column, ready for renaming.
- **F2** renames, **Delete** sends to the Recycle Bin (**Shift+Delete**
  deletes), **Ctrl+C/X/V** copy, cut and paste, **Ctrl+A** selects all,
  **F5** refreshes.
- Right click opens the context menu of the item or of the folder.
- Files can be dragged between columns, to folders and to other apps.

## macOS look
- Rounded selection in macOS blue (or in the Windows accent color), gray in
  unfocused columns, like in Finder.
- Taller rows, arrows marking folders and the Segoe UI Variable font.
- Sidebar with a soft gray background and adjustable spacing.

Explorer windows that are already open start using the mod within a few
seconds; there's no need to restart Explorer.

## Limitations
- The Explorer header (tabs, address bar and command bar) is XAML and isn't
  changed by this mod.
- The columns' context menu is the classic menu (the same as "Show more
  options"); the new Windows 11 menu only exists inside the native list.
- Folders opened through the columns are saved in List mode, because that's
  the mode that turns the columns on next time.
- Very large or network folders take a while to open a column, since the
  contents are read at once.
- With "Address bar follows the last column" on, each opened folder really
  navigates Explorer, and mods that animate folder changes (like "File
  Explorer WinUI-Like Content Animation") show the native list over the
  columns during the animation.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- columnView: true
  $name: Column view
  $description: In Explorer's "List" and "Details" view modes, selecting a folder shows the folders in columns, like in Finder
- macAppearance: true
  $name: macOS look
  $description: Rounded selection, taller rows, arrows on folders and a cleaner sidebar
- selectionColor: macos
  $name: Selection color
  $options:
  - macos: macOS blue
  - accent: Windows accent color
- columnWidth: 240
  $name: Column width (pixels at 100%)
- showPreview: true
  $name: Show the preview of the selected file
- foldersFirst: true
  $name: Folders before files
- syncAddressBar: false
  $name: Address bar follows the last column
  $description: Navigates Explorer to the folder of the last column, so its commands apply to it. Conflicts with mods that animate folder changes
- sidebarRowHeight: 28
  $name: Sidebar row height (pixels at 100%)
  $description: Only with the macOS look. 0 keeps the Windows one (32)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <propsys.h>
#include <uxtheme.h>
#include <wrl/client.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

constexpr WCHAR kOverlayClass[] = L"WindhawkFinderColumns";

// Timer on the tab window, which belongs to Explorer: an id it doesn't use.
constexpr UINT_PTR kTimerPoll = 0x57484645;
constexpr UINT kPollInterval = 120;

// On the columns window, which belongs to the mod.
constexpr UINT WM_APP_SELCHANGE = WM_APP + 1;
constexpr UINT WM_APP_FOCUSLIST = WM_APP + 2;
constexpr UINT WM_APP_NOTIFY = WM_APP + 100;  // + column index
constexpr UINT_PTR kTimerSync = 1;
constexpr UINT_PTR kTimerFocus = 2;
constexpr UINT_PTR kTimerScroll = 3;
constexpr UINT_PTR kTimerRefresh = 100;  // + column index
constexpr int kMaxColumns = 200;
constexpr int kListIdBase = 1000;

constexpr UINT_PTR kSubclassId = 1;

// Properties used in the preview (propkey.h), defined here so we don't
// depend on the library that exports them.
constexpr GUID kStorageGuid = {
    0xB725F130,
    0x47EF,
    0x101A,
    {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}};
constexpr PROPERTYKEY kPkeyItemTypeText = {kStorageGuid, 4};
constexpr PROPERTYKEY kPkeySize = {kStorageGuid, 12};
constexpr PROPERTYKEY kPkeyDateModified = {kStorageGuid, 14};
constexpr PROPERTYKEY kPkeyDateCreated = {kStorageGuid, 15};

HINSTANCE Instance() {
    return (HINSTANCE)&__ImageBase;
}

double Now() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

////////////////////////////////////////////////////////////////////////////////
// Settings
//
// Read by the threads of all Explorer windows and replaced by Windhawk's;
// each use takes a copy.

struct Settings {
    bool columnView = true;
    bool macAppearance = true;
    bool accentSelection = false;
    int columnWidth = 240;
    bool showPreview = true;
    bool foldersFirst = true;
    bool syncAddressBar = false;
    int sidebarRowHeight = 28;
};

Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;

Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

void LoadSettings() {
    Settings s;
    s.columnView = Wh_GetIntSetting(L"columnView") != 0;
    s.macAppearance = Wh_GetIntSetting(L"macAppearance") != 0;
    PCWSTR color = Wh_GetStringSetting(L"selectionColor");
    s.accentSelection = color && !wcscmp(color, L"accent");
    Wh_FreeStringSetting(color);
    s.columnWidth = std::clamp(Wh_GetIntSetting(L"columnWidth"), 120, 800);
    s.showPreview = Wh_GetIntSetting(L"showPreview") != 0;
    s.foldersFirst = Wh_GetIntSetting(L"foldersFirst") != 0;
    s.syncAddressBar = Wh_GetIntSetting(L"syncAddressBar") != 0;
    s.sidebarRowHeight = std::clamp(Wh_GetIntSetting(L"sidebarRowHeight"), 0, 64);
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = s;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

////////////////////////////////////////////////////////////////////////////////
// State of each tab
//
// Each Explorer tab has its own ShellTabWindowClass window, running on the
// Explorer window's thread. Everything about a tab is handled on that thread.

struct Item {
    PITEMID_CHILD child = nullptr;
    std::wstring name;
    bool folder = false;
    bool hidden = false;
    int icon = -1;  // In the system image list; read when drawing.
    std::wstring date, size;  // Only in Details mode.
};

struct Column {
    PIDLIST_ABSOLUTE pidl = nullptr;
    ComPtr<IShellFolder> folder;
    std::vector<Item> items;
    HWND list = nullptr;
    float width = 240;  // In pixels at 100%.
    ULONG notify = 0;

    ~Column() {
        for (auto& item : items) {
            CoTaskMemFree(item.child);
        }
        CoTaskMemFree(pidl);
    }
};

struct Preview {
    PIDLIST_ABSOLUTE pidl = nullptr;
    HBITMAP image = nullptr;  // 32 bits, premultiplied alpha.
    int imageWidth = 0, imageHeight = 0;
    std::wstring name, kind, size, modified, created;
};

struct Tab {
    HWND tab = nullptr;
    HWND defView = nullptr;       // Current SHELLDLL_DefView (recreated for each folder).
    HWND defViewChild = nullptr;  // The native list inside it.
    HWND overlay = nullptr;       // The columns, over the native list.
    RECT overlayRect{};

    bool active = false;
    PIDLIST_ABSOLUTE lastPidl = nullptr;
    UINT lastMode = 0;
    double lastSync = 0;
    double resetTime = 0;
    double shownFromNative = 0;
    double nativeInput = 0;   // Last click or key in the native list.
    double ignoredTime = 0;
    bool details = false;     // Columns opened from Details mode.
    HWND hiddenNative = nullptr;  // Native list hidden behind the columns.

    // New item expected in a column (New > Folder), for renaming.
    int renameColumn = -1;
    double renameUntil = 0;
    std::set<std::wstring> renameBefore;
    // Native list selection that must not open the columns (the one
    // Explorer makes by itself when going back to the parent folder, for example).
    PITEMID_CHILD ignoredSelection = nullptr;

    std::vector<std::unique_ptr<Column>> columns;
    int focusColumn = 0;
    Preview preview;
    RECT previewRect{};
    int scrollX = 0;
    int scrollTarget = 0;         // Where the smooth scroll is heading.
    double lastVerticalWheel = 0;
    int pendingSelection = -1;
    bool suppressSelection = false;
    std::wstring selectAfterRefresh;

    int resizing = -1;
    int resizeStartX = 0;
    float resizeStartWidth = 0;

    // Sidebar.
    HWND tree = nullptr;
    int treeOriginalHeight = -1;
    COLORREF treeOriginalBk = CLR_NONE;
    bool treeChanged = false;

    // Look, computed when creating the columns.
    Settings settings;
    float scale = 1;
    bool dark = false;
    HFONT font = nullptr, boldFont = nullptr, smallFont = nullptr;
    COLORREF bg = 0, text = 0, secondary = 0, divider = 0;
    COLORREF selection = 0, selectionInactive = 0, selectionText = 0;
    int rowHeight = 24;
};

std::mutex g_tabsMutex;
std::set<HWND> g_tabs;
UINT g_attachMsg, g_detachMsg, g_settingsMsg;

int Px(const Tab& t, float dip) {
    return (int)std::lround(dip * t.scale);
}

bool IsDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

COLORREF AccentColor(COLORREF fallback) {
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return fallback;
    }
    // ABGR, the same layout as a COLORREF.
    return value & 0xFFFFFF;
}

bool FontExists(PCWSTR face) {
    LOGFONTW lf{};
    lf.lfCharSet = DEFAULT_CHARSET;
    wcsncpy_s(lf.lfFaceName, face, _TRUNCATE);
    bool found = false;
    HDC dc = GetDC(nullptr);
    EnumFontFamiliesExW(
        dc, &lf,
        [](const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM param) -> int {
            *(bool*)param = true;
            return 0;
        },
        (LPARAM)&found, 0);
    ReleaseDC(nullptr, dc);
    return found;
}

HFONT MakeFont(float size, int weight) {
    static const PCWSTR face = FontExists(L"Segoe UI Variable Text")
                                   ? L"Segoe UI Variable Text"
                                   : L"Segoe UI";
    return CreateFontW(-(int)std::lround(size), 0, 0, 0, weight, FALSE, FALSE,
                       FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                       face);
}

void FreeLook(Tab& t) {
    for (HFONT* f : {&t.font, &t.boldFont, &t.smallFont}) {
        if (*f) {
            DeleteObject(*f);
            *f = nullptr;
        }
    }
}

void UpdateLook(Tab& t) {
    FreeLook(t);
    t.settings = GetSettings();
    t.scale = GetDpiForWindow(t.tab) / 96.0f;
    t.dark = IsDarkMode();
    bool mac = t.settings.macAppearance;

    t.font = MakeFont(13 * t.scale, FW_NORMAL);
    t.boldFont = MakeFont(14 * t.scale, FW_SEMIBOLD);
    t.smallFont = MakeFont(12 * t.scale, FW_NORMAL);
    t.rowHeight = Px(t, mac ? 24 : 22);

    if (t.dark) {
        t.bg = RGB(0x1C, 0x1C, 0x1C);
        t.text = RGB(0xEB, 0xEB, 0xEB);
        t.secondary = RGB(0x9A, 0x9A, 0x9A);
        t.divider = RGB(0x38, 0x38, 0x38);
        t.selectionInactive = RGB(0x46, 0x46, 0x46);
    } else {
        t.bg = RGB(0xFF, 0xFF, 0xFF);
        t.text = RGB(0x1F, 0x1F, 0x1F);
        t.secondary = RGB(0x80, 0x80, 0x80);
        t.divider = RGB(0xE3, 0xE3, 0xE3);
        t.selectionInactive = RGB(0xDC, 0xDC, 0xDC);
    }
    COLORREF macBlue = t.dark ? RGB(0x0A, 0x5F, 0xD8) : RGB(0x00, 0x63, 0xE1);
    if (!mac) {
        t.selection = GetSysColor(COLOR_HIGHLIGHT);
        t.selectionText = GetSysColor(COLOR_HIGHLIGHTTEXT);
    } else {
        t.selection = t.settings.accentSelection ? AccentColor(macBlue) : macBlue;
        t.selectionText = RGB(0xFF, 0xFF, 0xFF);
    }
}

HIMAGELIST SystemSmallIcons() {
    SHFILEINFOW sfi{};
    return (HIMAGELIST)SHGetFileInfoW(
        L"C:\\", FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi),
        SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
}

////////////////////////////////////////////////////////////////////////////////
// Column contents

std::wstring ShortDate(const FILETIME& utc) {
    FILETIME local;
    SYSTEMTIME st;
    WCHAR text[64] = L"";
    if (FileTimeToLocalFileTime(&utc, &local) &&
        FileTimeToSystemTime(&local, &st)) {
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr,
                        text, ARRAYSIZE(text), nullptr);
    }
    return text;
}

void ReadDetails(IShellFolder2* folder, Item& item) {
    VARIANT value;
    VariantInit(&value);
    if (SUCCEEDED(folder->GetDetailsEx(item.child, &kPkeyDateModified, &value))) {
        SYSTEMTIME st;
        FILETIME utc;
        if (value.vt == VT_DATE && VariantTimeToSystemTime(value.date, &st) &&
            SystemTimeToFileTime(&st, &utc)) {
            item.date = ShortDate(utc);
        }
        VariantClear(&value);
    }
    if (!item.folder &&
        SUCCEEDED(folder->GetDetailsEx(item.child, &kPkeySize, &value))) {
        VARIANT size;
        VariantInit(&size);
        WCHAR text[64];
        if (SUCCEEDED(VariantChangeType(&size, &value, 0, VT_UI8)) &&
            SUCCEEDED(StrFormatByteSizeEx(
                size.ullVal, SFBS_FLAGS_TRUNCATE_UNDISPLAYED_DECIMAL_DIGITS, text,
                ARRAYSIZE(text)))) {
            item.size = text;
        }
        VariantClear(&size);
        VariantClear(&value);
    }
}

void Enumerate(Tab& t, Column& c, bool allowUi = true) {
    for (auto& item : c.items) {
        CoTaskMemFree(item.child);
    }
    c.items.clear();

    // Respects "Show hidden items" and "Hide protected operating system files".
    SHELLSTATE state{};
    SHGetSetSettings(&state, SSF_SHOWALLOBJECTS | SSF_SHOWSUPERHIDDEN, FALSE);
    SHCONTF flags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS;
    if (state.fShowAllObjects) {
        flags |= SHCONTF_INCLUDEHIDDEN;
    }
    if (state.fShowSuperHidden) {
        flags |= SHCONTF_INCLUDESUPERHIDDEN;
    }

    ComPtr<IEnumIDList> items;
    if (c.folder->EnumObjects(allowUi ? t.overlay : nullptr, flags, &items) != S_OK ||
        !items) {
        return;
    }
    ComPtr<IShellFolder2> folder2;
    if (t.details) {
        c.folder.As(&folder2);
    }
    PITEMID_CHILD child;
    while (items->Next(1, &child, nullptr) == S_OK) {
        Item item;
        item.child = child;
        SFGAOF attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_HIDDEN;
        PCUITEMID_CHILD children[] = {child};
        c.folder->GetAttributesOf(1, children, &attributes);
        // A .zip is both a folder and a file; like in Finder, it's a file.
        item.folder = (attributes & SFGAO_FOLDER) && !(attributes & SFGAO_STREAM);
        item.hidden = attributes & SFGAO_HIDDEN;
        STRRET name;
        PWSTR text = nullptr;
        if (SUCCEEDED(c.folder->GetDisplayNameOf(child, SHGDN_INFOLDER, &name)) &&
            SUCCEEDED(StrRetToStrW(&name, child, &text))) {
            item.name = text;
            CoTaskMemFree(text);
        }
        if (folder2) {
            ReadDetails(folder2.Get(), item);
        }
        c.items.push_back(std::move(item));
    }

    bool foldersFirst = t.settings.foldersFirst;
    std::stable_sort(c.items.begin(), c.items.end(),
                     [foldersFirst](const Item& a, const Item& b) {
                         if (foldersFirst && a.folder != b.folder) {
                             return a.folder;
                         }
                         return StrCmpLogicalW(a.name.c_str(), b.name.c_str()) < 0;
                     });
}

std::vector<int> Selected(HWND list) {
    std::vector<int> result;
    int i = -1;
    while ((i = ListView_GetNextItem(list, i, LVNI_SELECTED)) >= 0) {
        result.push_back(i);
    }
    return result;
}

int ColumnOfList(Tab& t, HWND list) {
    for (int k = 0; k < (int)t.columns.size(); k++) {
        if (t.columns[k]->list == list) {
            return k;
        }
    }
    return -1;
}

// The native list is renaming an item (Explorer's "New > Folder", for
// example): its text box is visible inside SHELLDLL_DefView.
bool NativeRenaming(Tab& t) {
    if (!t.defView || !IsWindow(t.defView)) {
        return false;
    }
    bool found = false;
    EnumChildWindows(
        t.defView,
        [](HWND hwnd, LPARAM param) -> BOOL {
            WCHAR className[16];
            if (IsWindowVisible(hwnd) &&
                GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
                !_wcsicmp(className, L"Edit")) {
                *(bool*)param = true;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&found);
    return found;
}

int ColumnOfPidl(Tab& t, PCIDLIST_ABSOLUTE pidl) {
    for (int k = 0; k < (int)t.columns.size(); k++) {
        if (ILIsEqual(t.columns[k]->pidl, pidl)) {
            return k;
        }
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Drag and drop in the columns
//
// Forwards to Windows' own drop target: the folder under the mouse, or the
// column's folder when the mouse is outside the items.

class DropTarget final : public IDropTarget {
   public:
    DropTarget(Tab* tab, HWND list) : m_tab(tab), m_list(list) {}

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    IFACEMETHODIMP_(ULONG) AddRef() override { return ++m_refs; }
    IFACEMETHODIMP_(ULONG) Release() override {
        ULONG refs = --m_refs;
        if (!refs) {
            delete this;
        }
        return refs;
    }

    IFACEMETHODIMP DragEnter(IDataObject* data,
                             DWORD keys,
                             POINTL pt,
                             DWORD* effect) override {
        m_data = data;
        m_item = -2;
        return DragOver(keys, pt, effect);
    }

    IFACEMETHODIMP DragOver(DWORD keys, POINTL pt, DWORD* effect) override {
        int k = ColumnOfList(*m_tab, m_list);
        if (k < 0) {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        Column& c = *m_tab->columns[k];
        LVHITTESTINFO hit{};
        hit.pt = {pt.x, pt.y};
        ScreenToClient(m_list, &hit.pt);
        int item = ListView_HitTest(m_list, &hit);
        if (item >= 0 && (item >= (int)c.items.size() || !c.items[item].folder)) {
            item = -1;
        }
        DWORD wanted = *effect;
        if (item != m_item) {
            Leave();
            m_item = item;
            if (item >= 0) {
                PCUITEMID_CHILD children[] = {c.items[item].child};
                c.folder->GetUIObjectOf(m_tab->overlay, 1, children,
                                        IID_IDropTarget, nullptr,
                                        (void**)&m_target);
                ListView_SetItemState(m_list, item, LVIS_DROPHILITED,
                                      LVIS_DROPHILITED);
            } else {
                c.folder->CreateViewObject(m_tab->overlay,
                                           IID_PPV_ARGS(&m_target));
            }
            if (m_target &&
                FAILED(m_target->DragEnter(m_data.Get(), keys, pt, effect))) {
                m_target.Reset();
            }
            if (!m_target) {
                *effect = DROPEFFECT_NONE;
            }
            return S_OK;
        }
        if (m_target) {
            *effect = wanted;
            m_target->DragOver(keys, pt, effect);
        } else {
            *effect = DROPEFFECT_NONE;
        }
        return S_OK;
    }

    IFACEMETHODIMP DragLeave() override {
        Leave();
        m_data.Reset();
        return S_OK;
    }

    IFACEMETHODIMP Drop(IDataObject* data,
                        DWORD keys,
                        POINTL pt,
                        DWORD* effect) override {
        HRESULT hr = S_OK;
        if (m_target) {
            hr = m_target->Drop(data, keys, pt, effect);
            m_target.Reset();
        } else {
            *effect = DROPEFFECT_NONE;
        }
        ClearHighlight();
        m_data.Reset();
        return hr;
    }

   private:
    void ClearHighlight() {
        if (m_item >= 0) {
            ListView_SetItemState(m_list, m_item, 0, LVIS_DROPHILITED);
        }
        m_item = -2;
    }

    void Leave() {
        if (m_target) {
            m_target->DragLeave();
            m_target.Reset();
        }
        ClearHighlight();
    }

    std::atomic<ULONG> m_refs{1};
    Tab* m_tab;
    HWND m_list;
    ComPtr<IDataObject> m_data;
    ComPtr<IDropTarget> m_target;
    int m_item = -2;  // -1: column background.
};

////////////////////////////////////////////////////////////////////////////////
// Layout

int ColumnPx(const Tab& t, const Column& c) {
    return Px(t, c.width);
}

int GapPx(const Tab& t) {
    return std::max(1, Px(t, 5));
}

int PreviewPx(const Tab& t) {
    return Px(t, 280);
}

bool HasPreview(const Tab& t) {
    return t.preview.pidl != nullptr;
}

int ContentWidth(const Tab& t) {
    int width = 0;
    for (const auto& c : t.columns) {
        width += ColumnPx(t, *c) + GapPx(t);
    }
    if (HasPreview(t)) {
        width += PreviewPx(t);
    }
    return width;
}

void LayoutOverlay(Tab& t) {
    if (!t.overlay) {
        return;
    }
    RECT client;
    GetClientRect(t.overlay, &client);
    int content = ContentWidth(t);
    t.scrollX = std::clamp(t.scrollX, 0, std::max(0, content - (int)client.right));
    SCROLLINFO si{sizeof(si)};
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = std::max(0, content - 1);
    si.nPage = client.right;
    si.nPos = t.scrollX;
    SetScrollInfo(t.overlay, SB_HORZ, &si, TRUE);
    // The scroll bar may have appeared or disappeared.
    GetClientRect(t.overlay, &client);

    int x = -t.scrollX;
    HDWP defer = BeginDeferWindowPos((int)t.columns.size());
    for (auto& c : t.columns) {
        int width = ColumnPx(t, *c);
        defer = DeferWindowPos(defer, c->list, nullptr, x, 0, width,
                               client.bottom,
                               SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        x += width + GapPx(t);
    }
    EndDeferWindowPos(defer);
    for (auto& c : t.columns) {
        RECT rc;
        GetClientRect(c->list, &rc);
        ListView_SetColumnWidth(c->list, 0, rc.right);
    }
    t.previewRect = {x, 0, x + PreviewPx(t), client.bottom};
    InvalidateRect(t.overlay, nullptr, TRUE);
}

void ScrollToEnd(Tab& t, bool animate = true) {
    RECT client;
    GetClientRect(t.overlay, &client);
    int target = std::max(0, ContentWidth(t) - (int)client.right);
    if (!animate || !IsWindowVisible(t.overlay) || target == t.scrollX) {
        KillTimer(t.overlay, kTimerScroll);
        t.scrollX = target;
        LayoutOverlay(t);
        return;
    }
    // Puts the new column in place and slides to it, like in Finder.
    t.scrollTarget = target;
    LayoutOverlay(t);
    SetTimer(t.overlay, kTimerScroll, 15, nullptr);
}

// The divider to the right of column k takes the space between columns.
int DividerAt(Tab& t, int x) {
    int left = -t.scrollX;
    for (int k = 0; k < (int)t.columns.size(); k++) {
        left += ColumnPx(t, *t.columns[k]);
        if (x >= left - Px(t, 2) && x < left + GapPx(t) + Px(t, 2)) {
            return k;
        }
        left += GapPx(t);
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Preview

void ClearPreview(Tab& t) {
    Preview& p = t.preview;
    if (p.image) {
        DeleteObject(p.image);
    }
    CoTaskMemFree(p.pidl);
    p = {};
}

// Windows doesn't always deliver the thumbnail with premultiplied alpha,
// which is what AlphaBlend expects.
HBITMAP PremultipliedCopy(HBITMAP source, int& width, int& height) {
    BITMAP bm{};
    if (!GetObjectW(source, sizeof(bm), &bm) || bm.bmWidth <= 0) {
        return nullptr;
    }
    width = bm.bmWidth;
    height = std::abs(bm.bmHeight);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP copy = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!copy) {
        return nullptr;
    }
    HDC dc = GetDC(nullptr);
    GetDIBits(dc, source, 0, height, bits, &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);

    auto* pixels = (UINT32*)bits;
    size_t count = (size_t)width * height;
    bool anyAlpha = false, premultiplied = true;
    for (size_t i = 0; i < count; i++) {
        UINT32 p = pixels[i];
        UINT a = p >> 24;
        anyAlpha |= a != 0;
        if (((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a || (p & 0xFF) > a) {
            premultiplied = false;
        }
    }
    for (size_t i = 0; i < count; i++) {
        UINT32 p = pixels[i];
        if (!anyAlpha) {
            pixels[i] = p | 0xFF000000;  // Photo without alpha: opaque.
        } else if (!premultiplied) {
            UINT a = p >> 24;
            pixels[i] = (a << 24) | ((((p >> 16) & 0xFF) * a / 255) << 16) |
                        ((((p >> 8) & 0xFF) * a / 255) << 8) | ((p & 0xFF) * a / 255);
        }
    }
    return copy;
}

std::wstring FormatFileTime(const FILETIME& time) {
    FILETIME local;
    SYSTEMTIME st;
    if (!FileTimeToLocalFileTime(&time, &local) ||
        !FileTimeToSystemTime(&local, &st)) {
        return L"";
    }
    WCHAR date[128] = L"", clock[64] = L"";
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, date,
                    ARRAYSIZE(date), nullptr);
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr,
                    clock, ARRAYSIZE(clock));
    return std::wstring(date) + L", " + clock;
}

void SetPreview(Tab& t, PIDLIST_ABSOLUTE pidl) {
    ClearPreview(t);
    Preview& p = t.preview;
    p.pidl = pidl;
    ComPtr<IShellItem2> item;
    if (FAILED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item)))) {
        return;
    }
    PWSTR text = nullptr;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &text))) {
        p.name = text;
        CoTaskMemFree(text);
    }
    if (SUCCEEDED(item->GetString(kPkeyItemTypeText, &text))) {
        p.kind = text;
        CoTaskMemFree(text);
    }
    ULONGLONG size = 0;
    if (SUCCEEDED(item->GetUInt64(kPkeySize, &size))) {
        WCHAR buffer[64];
        if (SUCCEEDED(StrFormatByteSizeEx(
                size, SFBS_FLAGS_TRUNCATE_UNDISPLAYED_DECIMAL_DIGITS, buffer,
                ARRAYSIZE(buffer)))) {
            p.size = buffer;
        }
    }
    FILETIME time;
    if (SUCCEEDED(item->GetFileTime(kPkeyDateModified, &time))) {
        p.modified = FormatFileTime(time);
    }
    if (SUCCEEDED(item->GetFileTime(kPkeyDateCreated, &time))) {
        p.created = FormatFileTime(time);
    }

    ComPtr<IShellItemImageFactory> factory;
    HBITMAP bitmap = nullptr;
    int side = Px(t, 200);
    if (SUCCEEDED(item.As(&factory)) &&
        SUCCEEDED(factory->GetImage(SIZE{side, side}, SIIGBF_BIGGERSIZEOK,
                                    &bitmap))) {
        p.image = PremultipliedCopy(bitmap, p.imageWidth, p.imageHeight);
        DeleteObject(bitmap);
    }
}

void DrawPreview(Tab& t, HDC dc) {
    const Preview& p = t.preview;
    RECT r = t.previewRect;
    int pad = Px(t, 20);
    int y = r.top + Px(t, 24);
    int width = r.right - r.left - 2 * pad;
    if (width <= 0) {
        return;
    }

    if (p.image && p.imageWidth > 0 && p.imageHeight > 0) {
        int box = std::min(width, Px(t, 200));
        float fit = std::min((float)box / p.imageWidth, (float)box / p.imageHeight);
        fit = std::min(fit, 1.0f * t.scale * 2);
        int w = std::max(1, (int)(p.imageWidth * fit));
        int h = std::max(1, (int)(p.imageHeight * fit));
        int x = r.left + (r.right - r.left - w) / 2;
        HDC memory = CreateCompatibleDC(dc);
        HGDIOBJ old = SelectObject(memory, p.image);
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, x, y + (box - h) / 2, w, h, memory, 0, 0, p.imageWidth,
                   p.imageHeight, blend);
        SelectObject(memory, old);
        DeleteDC(memory);
        y += box + Px(t, 16);
    }

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, t.text);
    SelectObject(dc, t.boldFont);
    RECT name{r.left + pad, y, r.right - pad, y + Px(t, 60)};
    DrawTextW(dc, p.name.c_str(), (int)p.name.size(), &name,
              DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_END_ELLIPSIS |
                  DT_EDITCONTROL);
    y = name.bottom + Px(t, 2);
    RECT measured = name;
    DrawTextW(dc, p.name.c_str(), (int)p.name.size(), &measured,
              DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT | DT_EDITCONTROL);
    y = std::min(name.bottom, measured.bottom) + Px(t, 16);

    // Labels right-aligned to the middle, values on the left, like in "Get Info".
    SelectObject(dc, t.smallFont);
    int middle = r.left + pad + width * 2 / 5;
    int line = Px(t, 18);
    auto row = [&](PCWSTR label, const std::wstring& value) {
        if (value.empty()) {
            return;
        }
        RECT left{r.left + pad, y, middle - Px(t, 4), y + line * 2};
        SetTextColor(dc, t.secondary);
        DrawTextW(dc, label, -1, &left, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX);
        RECT right{middle + Px(t, 4), y, r.right - pad, y + line * 2};
        SetTextColor(dc, t.text);
        RECT calc = right;
        DrawTextW(dc, value.c_str(), (int)value.size(), &calc,
                  DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
        DrawTextW(dc, value.c_str(), (int)value.size(), &right,
                  DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_END_ELLIPSIS);
        y += std::min((int)(calc.bottom - calc.top), line * 2) + Px(t, 6);
    };
    row(L"Kind", p.kind);
    row(L"Size", p.size);
    row(L"Created", p.created);
    row(L"Modified", p.modified);
}

////////////////////////////////////////////////////////////////////////////////
// Creating and removing columns

LRESULT CALLBACK ListSubclassProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

void RegisterNotify(Tab& t, Column& c, int k) {
    SHChangeNotifyEntry entry{c.pidl, FALSE};
    c.notify = SHChangeNotifyRegister(
        t.overlay,
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_CREATE | SHCNE_DELETE | SHCNE_MKDIR | SHCNE_RMDIR |
            SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEDIR |
            SHCNE_ATTRIBUTES,
        WM_APP_NOTIFY + k, 1, &entry);
}

void DestroyColumn(Tab& t, Column& c, int k) {
    if (c.notify) {
        SHChangeNotifyDeregister(c.notify);
        c.notify = 0;
    }
    KillTimer(t.overlay, kTimerRefresh + k);
    if (c.list) {
        RevokeDragDrop(c.list);
        RemoveWindowSubclass(c.list, ListSubclassProc, kSubclassId);
        DestroyWindow(c.list);
        c.list = nullptr;
    }
}

void RemoveColumnsAfter(Tab& t, int k) {
    while ((int)t.columns.size() > k + 1) {
        int last = (int)t.columns.size() - 1;
        DestroyColumn(t, *t.columns[last], last);
        t.columns.pop_back();
    }
    t.focusColumn = std::min(t.focusColumn, std::max(0, (int)t.columns.size() - 1));
}

// Takes ownership of the pidl.
bool AddColumn(Tab& t, PIDLIST_ABSOLUTE pidl) {
    if ((int)t.columns.size() >= kMaxColumns) {
        CoTaskMemFree(pidl);
        return false;
    }
    auto c = std::make_unique<Column>();
    c->pidl = pidl;
    c->width = (float)t.settings.columnWidth + (t.details ? 130 : 0);
    HRESULT hr = ILIsEmpty(pidl) ? SHGetDesktopFolder(&c->folder)
                                 : SHBindToObject(nullptr, pidl, nullptr,
                                                  IID_PPV_ARGS(&c->folder));
    if (FAILED(hr) || !c->folder) {
        return false;
    }
    Enumerate(t, *c);

    int k = (int)t.columns.size();
    Column& column = *c;
    t.columns.push_back(std::move(c));
    column.list = CreateWindowExW(
        0, WC_LISTVIEWW, L"",
        WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | LVS_REPORT |
            LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS | LVS_OWNERDATA |
            LVS_OWNERDRAWFIXED | LVS_EDITLABELS,
        0, 0, ColumnPx(t, column), 100, t.overlay,
        (HMENU)(INT_PTR)(kListIdBase + k), Instance(), nullptr);
    if (!column.list) {
        t.columns.pop_back();
        return false;
    }
    ListView_SetExtendedListViewStyle(column.list,
                                      LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    LVCOLUMNW lvc{};
    lvc.mask = LVCF_WIDTH;
    lvc.cx = ColumnPx(t, column);
    ListView_InsertColumn(column.list, 0, &lvc);
    SetWindowTheme(column.list, t.dark ? L"DarkMode_Explorer" : L"Explorer",
                   nullptr);
    ListView_SetBkColor(column.list, t.bg);
    SetWindowFont(column.list, t.font, FALSE);
    ListView_SetItemCountEx(column.list, (int)column.items.size(),
                            LVSICF_NOINVALIDATEALL);
    SetWindowSubclass(column.list, ListSubclassProc, kSubclassId,
                      (DWORD_PTR)&t);
    auto* target = new DropTarget(&t, column.list);
    RegisterDragDrop(column.list, target);
    target->Release();
    RegisterNotify(t, column, k);
    return true;
}

void DestroyColumns(Tab& t) {
    RemoveColumnsAfter(t, -1);
    ClearPreview(t);
    t.scrollX = 0;
    t.focusColumn = 0;
}

void SyncNativeRegion(Tab& t);

void ResetColumns(Tab& t, PCIDLIST_ABSOLUTE pidl) {
    DestroyColumns(t);
    t.resetTime = Now();
    if (t.overlay) {
        ShowWindow(t.overlay, SW_HIDE);
        t.overlayRect = {};
    }
    SyncNativeRegion(t);
    AddColumn(t, ILClone(pidl));
    LayoutOverlay(t);
}

////////////////////////////////////////////////////////////////////////////////
// Selection and navigation

void UpdateOverlayVisibility(Tab& t);

void ScheduleSync(Tab& t) {
    // The address bar follows the last column, without changing folders on
    // every arrow press.
    SetTimer(t.overlay, kTimerSync, 350, nullptr);
}

IShellBrowser* GetBrowser(HWND tab) {
    // CWM_GETISHELLBROWSER: pointer without an extra reference, valid on the thread.
    auto* browser = (IShellBrowser*)SendMessageW(tab, WM_USER + 7, 0, 0);
    if (!browser) {
        browser = (IShellBrowser*)SendMessageW(GetAncestor(tab, GA_ROOT),
                                               WM_USER + 7, 0, 0);
    }
    return browser;
}

void SyncLocation(Tab& t) {
    if (t.columns.empty()) {
        return;
    }
    PCIDLIST_ABSOLUTE target = t.columns.back()->pidl;
    if (t.lastPidl && ILIsEqual(target, t.lastPidl)) {
        return;
    }
    if (IShellBrowser* browser = GetBrowser(t.tab)) {
        t.lastSync = Now();
        browser->BrowseObject(target, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
    }
}

void OnSelectionChanged(Tab& t, int k) {
    if (k < 0 || k >= (int)t.columns.size()) {
        return;
    }
    Column& c = *t.columns[k];
    std::vector<int> selected = Selected(c.list);

    // The next column stays if it's still the one of the selected folder (a
    // list refresh, for example).
    bool keepNext = false;
    PIDLIST_ABSOLUTE chosen = nullptr;
    if (selected.size() == 1 && selected[0] < (int)c.items.size()) {
        chosen = ILCombine(c.pidl, c.items[selected[0]].child);
        if (c.items[selected[0]].folder) {
            keepNext = k + 1 < (int)t.columns.size() &&
                       ILIsEqual(chosen, t.columns[k + 1]->pidl);
        } else {
            keepNext = k + 1 == (int)t.columns.size() && t.preview.pidl &&
                       ILIsEqual(chosen, t.preview.pidl);
        }
    }
    if (keepNext) {
        CoTaskMemFree(chosen);
        return;
    }

    RemoveColumnsAfter(t, k);
    ClearPreview(t);
    if (chosen) {
        if (c.items[selected[0]].folder) {
            AddColumn(t, chosen);
        } else if (t.settings.showPreview) {
            SetPreview(t, chosen);
        } else {
            CoTaskMemFree(chosen);
        }
    }
    ScrollToEnd(t);
    UpdateOverlayVisibility(t);
    if (t.settings.syncAddressBar) {
        ScheduleSync(t);
    }
}

void SelectOnly(HWND list, int index) {
    ListView_SetItemState(list, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    if (index >= 0) {
        ListView_SetItemState(list, index, LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(list, index, FALSE);
    }
}

void FocusColumn(Tab& t, int k) {
    if (k >= 0 && k < (int)t.columns.size()) {
        SetFocus(t.columns[k]->list);
    }
}

// Right arrow: enters the selected folder.
void MoveRight(Tab& t, int k) {
    if (k + 1 >= (int)t.columns.size()) {
        return;
    }
    Column& next = *t.columns[k + 1];
    FocusColumn(t, k + 1);
    if (ListView_GetSelectedCount(next.list) == 0 && !next.items.empty()) {
        SelectOnly(next.list, 0);
    }
}

// Left arrow: goes back one column; the current one stays, without selection.
void MoveLeft(Tab& t, int k) {
    if (k <= 0) {
        return;
    }
    SelectOnly(t.columns[k]->list, -1);
    FocusColumn(t, k - 1);
}

ComPtr<IContextMenu> ItemsMenu(Tab& t, Column& c, const std::vector<int>& selected) {
    std::vector<PCUITEMID_CHILD> children;
    for (int i : selected) {
        if (i < (int)c.items.size()) {
            children.push_back(c.items[i].child);
        }
    }
    ComPtr<IContextMenu> menu;
    if (!children.empty()) {
        c.folder->GetUIObjectOf(t.overlay, (UINT)children.size(), children.data(),
                                IID_IContextMenu, nullptr, (void**)&menu);
    }
    return menu;
}

ComPtr<IContextMenu> BackgroundMenu(Tab& t, Column& c) {
    ComPtr<IContextMenu> menu;
    c.folder->CreateViewObject(t.overlay, IID_PPV_ARGS(&menu));
    return menu;
}

void Invoke(Tab& t, IContextMenu* menu, LPCSTR verb, PCWSTR verbW, POINT* pt) {
    CMINVOKECOMMANDINFOEX info{sizeof(info)};
    info.fMask = CMIC_MASK_UNICODE | CMIC_MASK_ASYNCOK;
    if (GetKeyState(VK_SHIFT) < 0) {
        info.fMask |= CMIC_MASK_SHIFT_DOWN;
    }
    if (GetKeyState(VK_CONTROL) < 0) {
        info.fMask |= CMIC_MASK_CONTROL_DOWN;
    }
    if (pt) {
        info.fMask |= CMIC_MASK_PTINVOKE;
        info.ptInvoke = *pt;
    }
    info.hwnd = t.overlay;
    info.lpVerb = verb;
    info.lpVerbW = verbW;
    info.nShow = SW_SHOWNORMAL;
    menu->InvokeCommand((CMINVOKECOMMANDINFO*)&info);
}

// Verbs by name (copy, delete...) only work after the menu has been
// built.
void InvokeVerb(Tab& t, IContextMenu* menu, const char* verb) {
    if (!menu) {
        return;
    }
    HMENU popup = CreatePopupMenu();
    if (SUCCEEDED(menu->QueryContextMenu(popup, 0, 1, 0x7FFF, CMF_NORMAL))) {
        std::wstring verbW(verb, verb + strlen(verb));
        Invoke(t, menu, verb, verbW.c_str(), nullptr);
    }
    DestroyMenu(popup);
}

void OpenSelection(Tab& t, int k) {
    Column& c = *t.columns[k];
    std::vector<int> selected = Selected(c.list);
    if (selected.empty()) {
        return;
    }
    if (selected.size() == 1 && c.items[selected[0]].folder) {
        MoveRight(t, k);
        return;
    }
    ComPtr<IContextMenu> menu = ItemsMenu(t, c, selected);
    if (!menu) {
        return;
    }
    HMENU popup = CreatePopupMenu();
    if (SUCCEEDED(menu->QueryContextMenu(popup, 0, 1, 0x7FFF, CMF_DEFAULTONLY))) {
        UINT id = GetMenuDefaultItem(popup, FALSE, 0);
        if (id != (UINT)-1) {
            Invoke(t, menu.Get(), MAKEINTRESOURCEA(id - 1),
                   MAKEINTRESOURCEW(id - 1), nullptr);
        }
    }
    DestroyMenu(popup);
}

void StartRename(Tab& t, int k) {
    Column& c = *t.columns[k];
    std::vector<int> selected = Selected(c.list);
    if (selected.size() == 1) {
        SetFocus(c.list);
        ListView_EditLabel(c.list, selected[0]);
    }
}

void RefreshColumn(Tab& t, int k);

void RefreshAll(Tab& t) {
    for (int k = 0; k < (int)t.columns.size(); k++) {
        RefreshColumn(t, k);
    }
}

// After "New", the item that shows up in the column gets its name in editing.
void ExpectNewItem(Tab& t, int k) {
    t.renameColumn = k;
    t.renameUntil = Now() + 5;
    t.renameBefore.clear();
    for (const auto& item : t.columns[k]->items) {
        t.renameBefore.insert(item.name);
    }
}

bool MenuHasVerb(IContextMenu* menu, HMENU popup, const char* wanted) {
    for (int i = 0, n = GetMenuItemCount(popup); i < n; i++) {
        UINT id = GetMenuItemID(popup, i);
        if (id == (UINT)-1 || id == 0) {
            continue;
        }
        char verb[64] = "";
        if (SUCCEEDED(menu->GetCommandString(id - 1, GCS_VERBA, nullptr, verb,
                                             sizeof(verb))) &&
            !_stricmp(verb, wanted)) {
            return true;
        }
    }
    return false;
}

// The chosen command is in the "New" submenu (the one with "NewFolder").
bool IsNewCommand(IContextMenu* menu, HMENU popup, UINT id) {
    for (int i = 0, n = GetMenuItemCount(popup); i < n; i++) {
        HMENU sub = GetSubMenu(popup, i);
        if (!sub) {
            continue;
        }
        bool contains = false;
        for (int j = 0, m = GetMenuItemCount(sub); j < m; j++) {
            if (GetMenuItemID(sub, j) == id) {
                contains = true;
                break;
            }
        }
        if (contains && MenuHasVerb(menu, sub, "NewFolder")) {
            return true;
        }
    }
    return false;
}

// Context menu. Items drawn by the menu itself (Send to, Open with) need
// the messages forwarded while it's open.
thread_local IContextMenu2* t_menu2;
thread_local IContextMenu3* t_menu3;

void ShowContextMenu(Tab& t, int k, POINT screen, bool onItems) {
    Column& c = *t.columns[k];
    std::vector<int> selected = Selected(c.list);
    ComPtr<IContextMenu> menu =
        onItems && !selected.empty() ? ItemsMenu(t, c, selected) : BackgroundMenu(t, c);
    if (!menu) {
        return;
    }
    HMENU popup = CreatePopupMenu();
    UINT flags = CMF_NORMAL | CMF_CANRENAME;
    if (GetKeyState(VK_SHIFT) < 0) {
        flags |= CMF_EXTENDEDVERBS;
    }
    if (FAILED(menu->QueryContextMenu(popup, 0, 1, 0x7FFF, flags))) {
        DestroyMenu(popup);
        return;
    }
    ComPtr<IContextMenu2> menu2;
    ComPtr<IContextMenu3> menu3;
    menu.As(&menu2);
    menu.As(&menu3);
    t_menu2 = menu2.Get();
    t_menu3 = menu3.Get();
    UINT id = TrackPopupMenuEx(popup, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                               screen.x, screen.y, t.overlay, nullptr);
    t_menu2 = nullptr;
    t_menu3 = nullptr;

    if (id) {
        char verb[64] = "";
        menu->GetCommandString(id - 1, GCS_VERBA, nullptr, verb, sizeof(verb));
        bool singleFolder = onItems && selected.size() == 1 &&
                            c.items[selected[0]].folder;
        if (!onItems &&
            (!_stricmp(verb, "NewFolder") || IsNewCommand(menu.Get(), popup, id))) {
            ExpectNewItem(t, k);
        }
        if (!_stricmp(verb, "rename")) {
            // Without a native list, the column does the renaming.
            StartRename(t, k);
        } else if (singleFolder && !_stricmp(verb, "open")) {
            MoveRight(t, k);
        } else {
            Invoke(t, menu.Get(), MAKEINTRESOURCEA(id - 1),
                   MAKEINTRESOURCEW(id - 1), &screen);
        }
    }
    DestroyMenu(popup);
}

////////////////////////////////////////////////////////////////////////////////
// Refresh when the contents of a folder change

bool FolderExists(PCIDLIST_ABSOLUTE pidl) {
    ComPtr<IShellItem> item;
    SFGAOF attributes = 0;
    return SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) &&
           SUCCEEDED(item->GetAttributes(SFGAO_VALIDATE | SFGAO_FOLDER, &attributes));
}

void RefreshColumn(Tab& t, int k) {
    if (k < 0 || k >= (int)t.columns.size()) {
        return;
    }
    // The column's folder was deleted or moved: closes the column and refreshes
    // the previous one, without trying to read a folder that no longer exists.
    if (k > 0 && !FolderExists(t.columns[k]->pidl)) {
        RemoveColumnsAfter(t, k - 1);
        ClearPreview(t);
        LayoutOverlay(t);
        RefreshColumn(t, k - 1);
        return;
    }
    Column& c = *t.columns[k];
    std::set<std::wstring> selectedNames;
    for (int i : Selected(c.list)) {
        if (i < (int)c.items.size()) {
            selectedNames.insert(c.items[i].name);
        }
    }
    if (!t.selectAfterRefresh.empty()) {
        selectedNames = {t.selectAfterRefresh};
        t.selectAfterRefresh.clear();
    }
    int top = ListView_GetTopIndex(c.list);

    Enumerate(t, c, false);
    t.suppressSelection = true;
    ListView_SetItemState(c.list, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    ListView_SetItemCountEx(c.list, (int)c.items.size(),
                            LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
    int first = -1;
    for (int i = 0; i < (int)c.items.size(); i++) {
        if (selectedNames.count(c.items[i].name)) {
            ListView_SetItemState(c.list, i, LVIS_SELECTED, LVIS_SELECTED);
            if (first < 0) {
                first = i;
            }
        }
    }
    if (first >= 0) {
        ListView_SetItemState(c.list, first, LVIS_FOCUSED, LVIS_FOCUSED);
    }
    t.suppressSelection = false;
    if (top > 0 && top < (int)c.items.size()) {
        ListView_EnsureVisible(c.list, (int)c.items.size() - 1, FALSE);
        ListView_EnsureVisible(c.list, top, FALSE);
    }
    InvalidateRect(c.list, nullptr, TRUE);
    OnSelectionChanged(t, k);

    if (k == t.renameColumn && Now() < t.renameUntil) {
        for (int i = 0; i < (int)c.items.size(); i++) {
            if (t.renameBefore.count(c.items[i].name)) {
                continue;
            }
            t.renameColumn = -1;
            t.suppressSelection = true;
            SelectOnly(c.list, i);
            t.suppressSelection = false;
            RemoveColumnsAfter(t, k);
            ClearPreview(t);
            LayoutOverlay(t);
            SetFocus(c.list);
            ListView_EditLabel(c.list, i);
            break;
        }
    } else if (Now() >= t.renameUntil) {
        t.renameColumn = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

void DrawChevron(HDC dc, int cx, int cy, int size, COLORREF color, int width) {
    HPEN pen = CreatePen(PS_SOLID, width, color);
    HGDIOBJ old = SelectObject(dc, pen);
    POINT points[] = {{cx - size / 2, cy - size}, {cx + size / 2, cy},
                      {cx - size / 2, cy + size}};
    Polyline(dc, points, 3);
    SelectObject(dc, old);
    DeleteObject(pen);
}

void DrawRow(Tab& t, Column& c, const DRAWITEMSTRUCT* d) {
    int index = (int)d->itemID;
    if (index < 0 || index >= (int)c.items.size()) {
        return;
    }
    Item& item = c.items[index];
    HDC dc = d->hDC;
    RECT rc = d->rcItem;
    bool mac = t.settings.macAppearance;
    UINT state = ListView_GetItemState(c.list, index, LVIS_SELECTED | LVIS_DROPHILITED);
    bool selected = state & LVIS_SELECTED;
    bool drop = state & LVIS_DROPHILITED;
    bool focused = GetFocus() == c.list;
    bool strong = drop || (selected && focused);

    HBRUSH background = CreateSolidBrush(t.bg);
    FillRect(dc, &rc, background);
    DeleteObject(background);

    if (selected || drop) {
        COLORREF color = strong ? t.selection : t.selectionInactive;
        HBRUSH brush = CreateSolidBrush(color);
        if (mac) {
            RECT pill = rc;
            InflateRect(&pill, -Px(t, 6), -Px(t, 1));
            HGDIOBJ oldBrush = SelectObject(dc, brush);
            HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
            int radius = Px(t, 10);
            RoundRect(dc, pill.left, pill.top, pill.right + 1, pill.bottom + 1,
                      radius, radius);
            SelectObject(dc, oldPen);
            SelectObject(dc, oldBrush);
        } else {
            FillRect(dc, &rc, brush);
        }
        DeleteObject(brush);
    }

    COLORREF textColor = strong ? t.selectionText
                         : item.hidden ? t.secondary
                                       : t.text;
    HIMAGELIST icons = SystemSmallIcons();
    int iconW = 16, iconH = 16;
    ImageList_GetIconSize(icons, &iconW, &iconH);
    if (item.icon < 0) {
        item.icon = SHMapPIDLToSystemImageListIndex(c.folder.Get(), item.child,
                                                    nullptr);
        item.icon = std::max(item.icon, 0);
    }
    int x = rc.left + Px(t, mac ? 14 : 6);
    int cy = (rc.top + rc.bottom) / 2;
    ImageList_Draw(icons, item.icon, dc, x, cy - iconH / 2,
                   ILD_TRANSPARENT | (item.hidden ? ILD_BLEND25 : 0));

    int right = rc.right - Px(t, item.folder ? 26 : 10);
    if (t.details) {
        // Size and date in gray on the right, aligned across rows.
        HGDIOBJ old = SelectObject(dc, t.smallFont);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, strong ? t.selectionText : t.secondary);
        UINT format = DT_RIGHT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
        int sizeRight = rc.right - Px(t, 26);
        int sizeLeft = sizeRight - Px(t, 58);
        if (!item.folder) {
            RECT r{sizeLeft, rc.top, sizeRight, rc.bottom};
            DrawTextW(dc, item.size.c_str(), (int)item.size.size(), &r, format);
        }
        RECT r{sizeLeft - Px(t, 80), rc.top, sizeLeft - Px(t, 8), rc.bottom};
        DrawTextW(dc, item.date.c_str(), (int)item.date.size(), &r, format);
        SelectObject(dc, old);
        right = r.left - Px(t, 8);
    }
    RECT textRect{x + iconW + Px(t, 6), rc.top, right, rc.bottom};
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, textColor);
    HGDIOBJ oldFont = SelectObject(dc, t.font);
    DrawTextW(dc, item.name.c_str(), (int)item.name.size(), &textRect,
              DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    SelectObject(dc, oldFont);

    if (item.folder) {
        DrawChevron(dc, rc.right - Px(t, 16), cy, Px(t, 4),
                    strong ? t.selectionText : t.secondary,
                    std::max(1, Px(t, 1.5f)));
    }
}

void PaintOverlay(Tab& t, HDC dc, const RECT& dirty) {
    HBRUSH background = CreateSolidBrush(t.bg);
    FillRect(dc, &dirty, background);
    DeleteObject(background);

    RECT client;
    GetClientRect(t.overlay, &client);
    HBRUSH line = CreateSolidBrush(t.divider);
    int x = -t.scrollX;
    for (auto& c : t.columns) {
        x += ColumnPx(t, *c);
        int middle = x + GapPx(t) / 2;
        RECT divider{middle, 0, middle + 1, client.bottom};
        FillRect(dc, &divider, line);
        x += GapPx(t);
    }
    DeleteObject(line);
    if (HasPreview(t)) {
        DrawPreview(t, dc);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windows

LRESULT CALLBACK ListSubclassProc(HWND hwnd,
                                  UINT msg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  UINT_PTR,
                                  DWORD_PTR refData) {
    Tab& t = *(Tab*)refData;
    int k = ColumnOfList(t, hwnd);
    switch (msg) {
        case WM_MOUSEWHEEL:
            t.lastVerticalWheel = Now();
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK: {
            if ((Now() - t.shownFromNative) * 1000 < GetDoubleClickTime()) {
                return 0;
            }
            // Clicking the empty area of a column only moves the focus to it:
            // the selection and the columns opened after it stay.
            LVHITTESTINFO hit{};
            hit.pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (k >= 0 && !(wParam & (MK_CONTROL | MK_SHIFT)) &&
                ListView_HitTest(hwnd, &hit) < 0) {
                SetFocus(hwnd);
                return 0;
            }
            break;
        }

        case WM_RBUTTONDOWN: {
            // The same with the right button: opens the menu of the column's
            // folder without clearing the selection.
            LVHITTESTINFO hit{};
            hit.pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (k >= 0 && ListView_HitTest(hwnd, &hit) < 0) {
                SetFocus(hwnd);
                POINT screen = hit.pt;
                ClientToScreen(hwnd, &screen);
                ShowContextMenu(t, k, screen, false);
                return 0;
            }
            break;
        }

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            if (msg == WM_SETFOCUS && k >= 0) {
                t.focusColumn = k;
            }
            // The selection changes from blue to gray.
            InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case WM_KEYDOWN: {
            if (k < 0) {
                break;
            }
            bool ctrl = GetKeyState(VK_CONTROL) < 0;
            Column& c = *t.columns[k];
            switch (wParam) {
                case VK_RIGHT:
                    MoveRight(t, k);
                    return 0;
                case VK_LEFT:
                case VK_BACK:
                    MoveLeft(t, k);
                    return 0;
                case VK_RETURN:
                    OpenSelection(t, k);
                    return 0;
                case VK_ESCAPE:
                    // Closes the columns; the native list takes everything again.
                    SelectOnly(t.columns[0]->list, -1);
                    return 0;
                case VK_DELETE:
                    InvokeVerb(t, ItemsMenu(t, c, Selected(hwnd)).Get(), "delete");
                    return 0;
                case VK_F2:
                    StartRename(t, k);
                    return 0;
                case VK_F5:
                    RefreshAll(t);
                    return 0;
                case VK_APPS:
                case VK_F10: {
                    if (wParam == VK_F10 && GetKeyState(VK_SHIFT) >= 0) {
                        break;
                    }
                    int focusedItem = ListView_GetNextItem(hwnd, -1, LVNI_FOCUSED);
                    RECT rc{};
                    if (focusedItem >= 0) {
                        ListView_GetItemRect(hwnd, focusedItem, &rc, LVIR_BOUNDS);
                    }
                    POINT pt{rc.left + Px(t, 24), rc.bottom};
                    ClientToScreen(hwnd, &pt);
                    ShowContextMenu(t, k, pt, ListView_GetSelectedCount(hwnd) > 0);
                    return 0;
                }
            }
            if (ctrl) {
                switch (wParam) {
                    case 'C':
                        InvokeVerb(t, ItemsMenu(t, c, Selected(hwnd)).Get(), "copy");
                        return 0;
                    case 'X':
                        InvokeVerb(t, ItemsMenu(t, c, Selected(hwnd)).Get(), "cut");
                        return 0;
                    case 'V':
                        InvokeVerb(t, BackgroundMenu(t, c).Get(), "paste");
                        return 0;
                    case 'N':
                        if (GetKeyState(VK_SHIFT) < 0) {
                            ExpectNewItem(t, k);
                            InvokeVerb(t, BackgroundMenu(t, c).Get(), "NewFolder");
                            return 0;
                        }
                        break;
                    case 'A':
                        ListView_SetItemState(hwnd, -1, LVIS_SELECTED, LVIS_SELECTED);
                        return 0;
                }
            }
            break;
        }

        case WM_CHAR:
            // Ctrl+letter becomes a control character; the list would beep.
            if (wParam < 32 && wParam != VK_BACK) {
                return 0;
            }
            break;

        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, ListSubclassProc, kSubclassId);
            break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT OnListNotify(Tab& t, NMHDR* hdr) {
    int k = ColumnOfList(t, hdr->hwndFrom);
    if (k < 0) {
        return 0;
    }
    Column& c = *t.columns[k];
    switch (hdr->code) {
        case LVN_GETDISPINFOW: {
            auto* info = (NMLVDISPINFOW*)hdr;
            int i = info->item.iItem;
            if ((info->item.mask & LVIF_TEXT) && i >= 0 && i < (int)c.items.size() &&
                info->item.pszText && info->item.cchTextMax > 0) {
                wcsncpy_s(info->item.pszText, info->item.cchTextMax,
                          c.items[i].name.c_str(), _TRUNCATE);
            }
            return 0;
        }

        case LVN_ODFINDITEMW: {
            // Typing the start of a name jumps to it.
            auto* find = (NMLVFINDITEMW*)hdr;
            if (!(find->lvfi.flags & (LVFI_STRING | LVFI_PARTIAL)) ||
                !find->lvfi.psz || c.items.empty()) {
                return -1;
            }
            int count = (int)c.items.size();
            size_t length = wcslen(find->lvfi.psz);
            for (int n = 0; n < count; n++) {
                int i = (find->iStart + n) % count;
                if (!StrCmpNIW(c.items[i].name.c_str(), find->lvfi.psz,
                               (int)length)) {
                    return i;
                }
            }
            return -1;
        }

        case LVN_ITEMCHANGED: {
            auto* change = (NMLISTVIEW*)hdr;
            if (!(change->uChanged & LVIF_STATE) ||
                !((change->uNewState ^ change->uOldState) & LVIS_SELECTED)) {
                return 0;
            }
            [[fallthrough]];
        }
        case LVN_ODSTATECHANGED:
            if (!t.suppressSelection) {
                bool posted = t.pendingSelection >= 0;
                t.pendingSelection =
                    posted ? std::min(t.pendingSelection, k) : k;
                if (!posted) {
                    PostMessageW(t.overlay, WM_APP_SELCHANGE, 0, 0);
                }
            }
            return 0;

        case NM_DBLCLK:
            OpenSelection(t, k);
            return 0;

        case LVN_BEGINDRAG:
        case LVN_BEGINRDRAG: {
            ComPtr<IDataObject> data;
            std::vector<int> selected = Selected(c.list);
            std::vector<PCUITEMID_CHILD> children;
            for (int i : selected) {
                children.push_back(c.items[i].child);
            }
            if (!children.empty() &&
                SUCCEEDED(c.folder->GetUIObjectOf(
                    t.overlay, (UINT)children.size(), children.data(),
                    IID_IDataObject, nullptr, (void**)&data))) {
                DWORD effect = 0;
                SHDoDragDrop(t.overlay, data.Get(), nullptr,
                             DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK,
                             &effect);
            }
            return 0;
        }

        case LVN_BEGINLABELEDITW: {
            auto* info = (NMLVDISPINFOW*)hdr;
            int i = info->item.iItem;
            if (i < 0 || i >= (int)c.items.size()) {
                return TRUE;
            }
            PCUITEMID_CHILD children[] = {c.items[i].child};
            SFGAOF attributes = SFGAO_CANRENAME;
            c.folder->GetAttributesOf(1, children, &attributes);
            if (!(attributes & SFGAO_CANRENAME)) {
                return TRUE;
            }
            // The name for editing may have the extension the list hides.
            STRRET name;
            PWSTR text = nullptr;
            if (SUCCEEDED(c.folder->GetDisplayNameOf(
                    c.items[i].child, SHGDN_INFOLDER | SHGDN_FOREDITING, &name)) &&
                SUCCEEDED(StrRetToStrW(&name, c.items[i].child, &text))) {
                if (HWND edit = ListView_GetEditControl(c.list)) {
                    SetWindowTextW(edit, text);
                    SendMessageW(edit, EM_SETSEL, 0, -1);
                }
                CoTaskMemFree(text);
            }
            return FALSE;
        }

        case LVN_ENDLABELEDITW: {
            auto* info = (NMLVDISPINFOW*)hdr;
            int i = info->item.iItem;
            if (!info->item.pszText || i < 0 || i >= (int)c.items.size()) {
                return FALSE;
            }
            PITEMID_CHILD renamed = nullptr;
            if (SUCCEEDED(c.folder->SetNameOf(t.overlay, c.items[i].child,
                                              info->item.pszText,
                                              SHGDN_INFOLDER | SHGDN_FOREDITING,
                                              &renamed)) &&
                renamed) {
                STRRET name;
                PWSTR text = nullptr;
                if (SUCCEEDED(c.folder->GetDisplayNameOf(renamed, SHGDN_INFOLDER,
                                                         &name)) &&
                    SUCCEEDED(StrRetToStrW(&name, renamed, &text))) {
                    t.selectAfterRefresh = text;
                    CoTaskMemFree(text);
                }
                CoTaskMemFree(renamed);
                RefreshColumn(t, k);
            }
            return FALSE;
        }
    }
    return 0;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* cs = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
    }
    auto* tp = (Tab*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!tp) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    Tab& t = *tp;

    // Messages for the items drawn by the context menu itself.
    if (t_menu3 || t_menu2) {
        switch (msg) {
            case WM_INITMENUPOPUP:
            case WM_DRAWITEM:
            case WM_MEASUREITEM:
            case WM_MENUCHAR: {
                bool forMenu = msg == WM_INITMENUPOPUP || msg == WM_MENUCHAR ||
                               (msg == WM_DRAWITEM &&
                                ((DRAWITEMSTRUCT*)lParam)->CtlType == ODT_MENU) ||
                               (msg == WM_MEASUREITEM &&
                                ((MEASUREITEMSTRUCT*)lParam)->CtlType == ODT_MENU);
                if (forMenu) {
                    LRESULT result = 0;
                    if (t_menu3 &&
                        SUCCEEDED(t_menu3->HandleMenuMsg2(msg, wParam, lParam,
                                                          &result))) {
                        return result;
                    }
                    if (t_menu2 &&
                        SUCCEEDED(t_menu2->HandleMenuMsg(msg, wParam, lParam))) {
                        return msg == WM_INITMENUPOPUP ? 0 : TRUE;
                    }
                }
                break;
            }
        }
    }

    switch (msg) {
        case WM_MEASUREITEM: {
            auto* measure = (MEASUREITEMSTRUCT*)lParam;
            if (measure->CtlType == ODT_LISTVIEW) {
                measure->itemHeight = t.rowHeight;
                return TRUE;
            }
            break;
        }

        case WM_DRAWITEM: {
            auto* draw = (DRAWITEMSTRUCT*)lParam;
            if (draw->CtlType == ODT_LISTVIEW) {
                int k = ColumnOfList(t, draw->hwndItem);
                if (k >= 0) {
                    DrawRow(t, *t.columns[k], draw);
                }
                return TRUE;
            }
            break;
        }

        case WM_NOTIFY:
            return OnListNotify(t, (NMHDR*)lParam);

        case WM_APP_SELCHANGE: {
            int k = t.pendingSelection;
            t.pendingSelection = -1;
            OnSelectionChanged(t, k);
            return 0;
        }

        case WM_APP_FOCUSLIST:
            if (IsWindowVisible(hwnd) && !t.columns.empty() && !NativeRenaming(t)) {
                FocusColumn(t, std::min(t.focusColumn, (int)t.columns.size() - 1));
            }
            return 0;

        case WM_CONTEXTMENU: {
            int k = ColumnOfList(t, (HWND)wParam);
            if (k < 0) {
                return 0;
            }
            HWND list = t.columns[k]->list;
            POINT screen{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            bool onItems = true;
            if (screen.x == -1 && screen.y == -1) {
                GetCursorPos(&screen);
            } else {
                LVHITTESTINFO hit{};
                hit.pt = screen;
                ScreenToClient(list, &hit.pt);
                int item = ListView_HitTest(list, &hit);
                if (item < 0) {
                    onItems = false;  // Background: the menu of the column's folder.
                } else if (!(ListView_GetItemState(list, item, LVIS_SELECTED) &
                             LVIS_SELECTED)) {
                    SelectOnly(list, item);
                }
            }
            ShowContextMenu(t, k, screen, onItems);
            return 0;
        }

        case WM_TIMER:
            if (wParam == kTimerSync) {
                KillTimer(hwnd, kTimerSync);
                SyncLocation(t);
            } else if (wParam == kTimerScroll) {
                int delta = t.scrollTarget - t.scrollX;
                if (std::abs(delta) <= 2) {
                    t.scrollX = t.scrollTarget;
                    KillTimer(hwnd, kTimerScroll);
                } else {
                    t.scrollX += delta * 30 / 100 + (delta > 0 ? 1 : -1);
                }
                LayoutOverlay(t);
                UpdateWindow(hwnd);
            } else if (wParam == kTimerFocus) {
                KillTimer(hwnd, kTimerFocus);
                PostMessageW(hwnd, WM_APP_FOCUSLIST, 0, 0);
            } else if (wParam >= kTimerRefresh &&
                       wParam < kTimerRefresh + kMaxColumns) {
                KillTimer(hwnd, wParam);
                RefreshColumn(t, (int)(wParam - kTimerRefresh));
            }
            return 0;

        case WM_HSCROLL: {
            SCROLLINFO si{sizeof(si)};
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_HORZ, &si);
            int pos = t.scrollX;
            switch (LOWORD(wParam)) {
                case SB_LINELEFT:
                    pos -= Px(t, 40);
                    break;
                case SB_LINERIGHT:
                    pos += Px(t, 40);
                    break;
                case SB_PAGELEFT:
                    pos -= (int)si.nPage;
                    break;
                case SB_PAGERIGHT:
                    pos += (int)si.nPage;
                    break;
                case SB_THUMBTRACK:
                case SB_THUMBPOSITION:
                    pos = si.nTrackPos;
                    break;
            }
            if (pos != t.scrollX) {
                KillTimer(hwnd, kTimerScroll);
                t.scrollX = pos;
                LayoutOverlay(t);
            }
            return 0;
        }

        case WM_MOUSEHWHEEL:
            // Scrolling a column on a touchpad also sends a bit of horizontal
            // scrolling; without this the columns went back and forth by themselves.
            if (Now() - t.lastVerticalWheel < 0.4) {
                return 0;
            }
            KillTimer(hwnd, kTimerScroll);
            t.scrollX += GET_WHEEL_DELTA_WPARAM(wParam) * Px(t, 40) / WHEEL_DELTA;
            LayoutOverlay(t);
            return 0;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                if (t.resizing >= 0 || DividerAt(t, pt.x) >= 0) {
                    SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
                    return TRUE;
                }
            }
            break;

        case WM_LBUTTONDOWN: {
            int k = DividerAt(t, GET_X_LPARAM(lParam));
            if (k >= 0) {
                t.resizing = k;
                t.resizeStartX = GET_X_LPARAM(lParam);
                t.resizeStartWidth = t.columns[k]->width;
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_MOUSEMOVE:
            if (t.resizing >= 0 && t.resizing < (int)t.columns.size()) {
                float delta = (GET_X_LPARAM(lParam) - t.resizeStartX) / t.scale;
                t.columns[t.resizing]->width =
                    std::max(120.0f, t.resizeStartWidth + delta);
                LayoutOverlay(t);
                UpdateWindow(hwnd);
            }
            return 0;

        case WM_LBUTTONUP:
        case WM_CAPTURECHANGED:
            if (t.resizing >= 0) {
                t.resizing = -1;
                if (GetCapture() == hwnd) {
                    ReleaseCapture();
                }
            }
            return 0;

        case WM_SIZE:
            LayoutOverlay(t);
            return 0;

        case WM_DESTROY:
            // When the tab closes the columns are destroyed along with it;
            // undoes the drag and drop registration while they still exist.
            DestroyColumns(t);
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            // No flicker and opaque over Mica (see the columns' WM_PAINT).
            HDC buffer = nullptr;
            HPAINTBUFFER paint = BeginBufferedPaint(dc, &ps.rcPaint, BPBF_TOPDOWNDIB,
                                                    nullptr, &buffer);
            if (paint) {
                PaintOverlay(t, buffer, ps.rcPaint);
                BufferedPaintSetAlpha(paint, nullptr, 255);
                EndBufferedPaint(paint, TRUE);
            } else {
                PaintOverlay(t, dc, ps.rcPaint);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    if (msg >= WM_APP_NOTIFY && msg < WM_APP_NOTIFY + kMaxColumns) {
        // A folder changed; waits for the sequence of changes to finish.
        PIDLIST_ABSOLUTE* pidls = nullptr;
        LONG event = 0;
        HANDLE lock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam,
                                                &pidls, &event);
        if (lock) {
            SHChangeNotification_Unlock(lock);
        }
        SetTimer(hwnd, kTimerRefresh + (msg - WM_APP_NOTIFY), 150, nullptr);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Link with Explorer's native list

LRESULT CALLBACK DefViewSubclassProc(HWND hwnd,
                                     UINT msg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     UINT_PTR,
                                     DWORD_PTR refData);
LRESULT CALLBACK DefViewChildSubclassProc(HWND hwnd,
                                          UINT msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          UINT_PTR,
                                          DWORD_PTR refData);

// The columns live on the tab window, which lasts as long as the tab, over
// the native list. The native list is recreated on each folder change; the
// columns aren't, and that's why they don't flicker.
void PositionOverlay(Tab& t) {
    if (!t.overlay || !t.defView || !IsWindow(t.defView)) {
        return;
    }
    RECT r;
    GetWindowRect(t.defView, &r);
    MapWindowPoints(nullptr, t.tab, (POINT*)&r, 2);
    bool onTop = GetWindow(t.tab, GW_CHILD) == t.overlay;
    if (EqualRect(&r, &t.overlayRect) && onTop && IsWindowVisible(t.overlay)) {
        return;
    }
    t.overlayRect = r;
    SetWindowPos(t.overlay, HWND_TOP, r.left, r.top, r.right - r.left,
                 r.bottom - r.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

// The native list draws part of the animations (icons, new item) through
// composition, over any child window. While the columns are shown, an empty
// region hides it completely.
void SyncNativeRegion(Tab& t) {
    HWND wanted = t.overlay && IsWindowVisible(t.overlay) && t.defViewChild &&
                          IsWindow(t.defViewChild)
                      ? t.defViewChild
                      : nullptr;
    if (wanted == t.hiddenNative) {
        return;
    }
    if (t.hiddenNative && IsWindow(t.hiddenNative)) {
        SetWindowRgn(t.hiddenNative, nullptr, TRUE);
    }
    t.hiddenNative = wanted;
    if (wanted) {
        SetWindowRgn(wanted, CreateRectRgn(0, 0, 0, 0), TRUE);
    }
}

bool IsColumnMode(UINT mode) {
    return mode == FVM_LIST || mode == FVM_DETAILS;
}

// With only the current folder open and nothing selected in it, Explorer's
// native list takes the whole area; the columns show up when something is selected.
bool ShouldShowOverlay(Tab& t) {
    return t.active && t.overlay && !t.columns.empty() &&
           (t.columns.size() > 1 || HasPreview(t) ||
            ListView_GetSelectedCount(t.columns[0]->list) > 0);
}

void UpdateOverlayVisibility(Tab& t) {
    if (!t.overlay) {
        return;
    }
    if (ShouldShowOverlay(t)) {
        bool wasVisible = IsWindowVisible(t.overlay);
        PositionOverlay(t);
        if (!wasVisible) {
            ScrollToEnd(t, false);
        }
        SyncNativeRegion(t);
        return;
    }
    if (!IsWindowVisible(t.overlay)) {
        return;
    }
    bool hadFocus = IsChild(t.overlay, GetFocus());
    ShowWindow(t.overlay, SW_HIDE);
    t.overlayRect = {};
    SyncNativeRegion(t);
    // The native selection that opened the columns can't open them again.
    if (IShellBrowser* browser = GetBrowser(t.tab)) {
        ComPtr<IShellView> view;
        if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
            view->SelectItem(nullptr, SVSI_DESELECTOTHERS);
        }
    }
    if (hadFocus && t.defViewChild) {
        SetFocus(t.defViewChild);
    }
}

void UnhookDefView(Tab& t) {
    if (t.hiddenNative && IsWindow(t.hiddenNative)) {
        SetWindowRgn(t.hiddenNative, nullptr, TRUE);
    }
    t.hiddenNative = nullptr;
    if (t.defView && IsWindow(t.defView)) {
        RemoveWindowSubclass(t.defView, DefViewSubclassProc, kSubclassId);
    }
    if (t.defViewChild && IsWindow(t.defViewChild)) {
        RemoveWindowSubclass(t.defViewChild, DefViewChildSubclassProc, kSubclassId);
    }
    t.defView = nullptr;
    t.defViewChild = nullptr;
}

void HookDefView(Tab& t, HWND defView) {
    if (defView != t.defView) {
        UnhookDefView(t);
        if (defView && SetWindowSubclass(defView, DefViewSubclassProc,
                                         kSubclassId, (DWORD_PTR)&t)) {
            t.defView = defView;
        }
    }
    // The native list inside it may show up later.
    if (t.defView && !t.defViewChild) {
        HWND child = FindWindowExW(t.defView, nullptr, L"DirectUIHWND", nullptr);
        if (child && SetWindowSubclass(child, DefViewChildSubclassProc,
                                       kSubclassId, (DWORD_PTR)&t)) {
            t.defViewChild = child;
        }
    }
}

LRESULT CALLBACK DefViewSubclassProc(HWND hwnd,
                                     UINT msg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     UINT_PTR,
                                     DWORD_PTR refData) {
    Tab& t = *(Tab*)refData;
    LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
    switch (msg) {
        case WM_WINDOWPOSCHANGED:
            if (t.active) {
                UpdateOverlayVisibility(t);
            }
            break;
        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, DefViewSubclassProc, kSubclassId);
            if (t.defView == hwnd) {
                t.defView = nullptr;
                t.defViewChild = nullptr;
            }
            break;
    }
    return result;
}

LRESULT CALLBACK DefViewChildSubclassProc(HWND hwnd,
                                          UINT msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          UINT_PTR,
                                          DWORD_PTR refData) {
    Tab& t = *(Tab*)refData;
    switch (msg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_POINTERDOWN:
        case WM_POINTERUP:
        case WM_KEYDOWN:
            // Only a selection made by the user opens the columns.
            t.nativeInput = Now();
            break;

        case WM_SETFOCUS:
            // When changing folders Explorer puts the focus on the native list,
            // which is hidden behind the columns. Waits a bit: if it's for
            // renaming, the text box shows up right after.
            if (t.active && t.overlay && IsWindowVisible(t.overlay)) {
                SetTimer(t.overlay, kTimerFocus, 80, nullptr);
            }
            break;
        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, DefViewChildSubclassProc, kSubclassId);
            if (t.defViewChild == hwnd) {
                t.defViewChild = nullptr;
            }
            break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

bool CreateOverlay(Tab& t) {
    UpdateLook(t);
    t.overlay = CreateWindowExW(WS_EX_LAYERED, kOverlayClass, L"",
                                WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL,
                                0, 0, 0, 0, t.tab, nullptr, Instance(), &t);
    if (!t.overlay) {
        return false;
    }
    // Own opaque layer: the Mica background of the Explorer window reads the
    // alpha of each pixel, and GDI drawing leaves the alpha at zero (black bands).
    SetLayeredWindowAttributes(t.overlay, 0, 255, LWA_ALPHA);
    SetWindowTheme(t.overlay, t.dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
    t.overlayRect = {};
    return true;
}

void DestroyOverlay(Tab& t) {
    if (!t.overlay) {
        return;
    }
    // The focus was on the columns: gives it back to the native list.
    bool hadFocus = IsChild(t.overlay, GetFocus());
    DestroyColumns(t);
    DestroyWindow(t.overlay);
    t.overlay = nullptr;
    SyncNativeRegion(t);
    FreeLook(t);
    if (hadFocus && t.defViewChild) {
        SetFocus(t.defViewChild);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Sidebar

HWND FindSidebarTree(HWND tab) {
    HWND found = nullptr;
    EnumChildWindows(
        tab,
        [](HWND hwnd, LPARAM param) -> BOOL {
            WCHAR className[32], parentClass[32];
            if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
                !wcscmp(className, L"SysTreeView32") &&
                GetClassNameW(GetParent(hwnd), parentClass, ARRAYSIZE(parentClass)) &&
                !wcscmp(parentClass, L"NamespaceTreeControl")) {
                *(HWND*)param = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&found);
    return found;
}

void RestoreSidebar(Tab& t) {
    if (t.treeChanged && t.tree && IsWindow(t.tree)) {
        TreeView_SetItemHeight(t.tree, t.treeOriginalHeight);
        TreeView_SetBkColor(t.tree, t.treeOriginalBk);
        InvalidateRect(t.tree, nullptr, TRUE);
    }
    t.treeChanged = false;
}

void ApplySidebar(Tab& t) {
    Settings s = GetSettings();
    if (!t.tree || !IsWindow(t.tree)) {
        t.tree = FindSidebarTree(t.tab);
        t.treeChanged = false;
        if (!t.tree) {
            return;
        }
        t.treeOriginalHeight = TreeView_GetItemHeight(t.tree);
        t.treeOriginalBk = TreeView_GetBkColor(t.tree);
    }
    if (!s.macAppearance) {
        RestoreSidebar(t);
        return;
    }
    // Explorer may redo the height (scaling or theme change); always
    // checks it.
    float scale = GetDpiForWindow(t.tree) / 96.0f;
    if (s.sidebarRowHeight > 0) {
        int height = (int)std::lround(s.sidebarRowHeight * scale);
        if (TreeView_GetItemHeight(t.tree) != height) {
            TreeView_SetItemHeight(t.tree, height);
            t.treeChanged = true;
        }
    }
    // Soft gray background, like the Finder sidebar.
    COLORREF bk = IsDarkMode() ? RGB(0x24, 0x24, 0x24) : RGB(0xF3, 0xF3, 0xF3);
    if (TreeView_GetBkColor(t.tree) != bk) {
        TreeView_SetBkColor(t.tree, bk);
        InvalidateRect(t.tree, nullptr, TRUE);
        t.treeChanged = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Following the tab
//
// Every 120 ms: which folder is open, in which view mode and which is the
// native list. List and Details modes turn the columns on; the others turn
// them off. With the columns hidden, selecting a folder in the native list
// shows the columns.

void Deactivate(Tab& t) {
    t.active = false;
    DestroyOverlay(t);
}

void ShowNativeView(Tab& t) {
    t.suppressSelection = true;
    SelectOnly(t.columns[0]->list, -1);
    t.suppressSelection = false;
    RemoveColumnsAfter(t, 0);
    ClearPreview(t);
    LayoutOverlay(t);
    ShowWindow(t.overlay, SW_HIDE);
    t.overlayRect = {};
    SyncNativeRegion(t);
}

void SetIgnoredSelection(Tab& t, PITEMID_CHILD child) {
    CoTaskMemFree(t.ignoredSelection);
    t.ignoredSelection = child;
}

// A folder selected in the native list opens the columns from it.
void CheckNativeSelection(Tab& t, IFolderView* folderView) {
    if (t.columns.empty()) {
        return;
    }
    Column& root = *t.columns[0];
    ComPtr<IFolderView2> folderView2;
    int count = 0, index = -1;
    PITEMID_CHILD child = nullptr;
    if (FAILED(folderView->ItemCount(SVGIO_SELECTION, &count)) || count != 1 ||
        FAILED(folderView->QueryInterface(IID_PPV_ARGS(&folderView2))) ||
        FAILED(folderView2->GetSelectedItem(-1, &index)) || index < 0 ||
        FAILED(folderView->Item(index, &child)) || !child) {
        SetIgnoredSelection(t, nullptr);
        return;
    }
    auto same = [&](PCUITEMID_CHILD a, PCUITEMID_CHILD b) {
        return HRESULT_CODE(root.folder->CompareIDs(0, a, b)) == 0;
    };
    // Only a new selection made with the mouse or keyboard in the list
    // counts; Explorer's own ones (new folder to rename, the folder it went
    // back from) stay in the native list.
    double now = Now();
    bool sameAsBefore = t.ignoredSelection && same(t.ignoredSelection, child);
    bool byUser = now - t.nativeInput < 0.6 && t.nativeInput > t.resetTime &&
                  (!sameAsBefore || t.nativeInput > t.ignoredTime);
    SetIgnoredSelection(t, child);
    t.ignoredTime = now;
    if (!byUser || NativeRenaming(t)) {
        return;
    }
    // Files stay in the native list; only folders open the columns.
    int found = -1;
    for (int i = 0; i < (int)root.items.size(); i++) {
        if (same(root.items[i].child, child)) {
            found = i;
            break;
        }
    }
    if (found < 0 || !root.items[found].folder) {
        return;
    }
    HWND focus = GetFocus();
    bool nativeFocus = focus && t.defView &&
                       (focus == t.defView || IsChild(t.defView, focus));
    t.suppressSelection = true;
    SelectOnly(root.list, found);
    t.suppressSelection = false;
    OnSelectionChanged(t, 0);
    t.shownFromNative = Now();
    if (nativeFocus) {
        FocusColumn(t, 0);
    }
}

void Poll(Tab& t) {
    ApplySidebar(t);
    if (!GetSettings().columnView) {
        if (t.active) {
            Deactivate(t);
        }
        return;
    }

    IShellBrowser* browser = GetBrowser(t.tab);
    ComPtr<IShellView> view;
    ComPtr<IFolderView> folderView;
    ComPtr<IPersistFolder2> persist;
    PIDLIST_ABSOLUTE pidl = nullptr;
    UINT mode = 0;
    HWND defView = nullptr;
    if (!browser || FAILED(browser->QueryActiveShellView(&view)) || !view ||
        FAILED(view.As(&folderView)) ||
        FAILED(folderView->GetCurrentViewMode(&mode)) ||
        FAILED(folderView->GetFolder(IID_PPV_ARGS(&persist))) ||
        FAILED(persist->GetCurFolder(&pidl)) || !pidl) {
        return;
    }
    view->GetWindow(&defView);

    bool changed = !t.lastPidl || !ILIsEqual(pidl, t.lastPidl);
    if (changed) {
        int k = t.active ? ColumnOfPidl(t, pidl) : -1;
        if (k >= 0) {
            // A folder of the columns: opened by them, or Explorer's Back.
            if (Now() - t.lastSync > 2.0 && k + 1 < (int)t.columns.size()) {
                SelectOnly(t.columns[k]->list, -1);
                RemoveColumnsAfter(t, k);
                ClearPreview(t);
                LayoutOverlay(t);
            }
            // To switch modes keep turning the columns off.
            if (!IsColumnMode(mode)) {
                UINT keep = IsColumnMode(t.lastMode) ? t.lastMode : FVM_LIST;
                folderView->SetCurrentViewMode(keep);
                mode = keep;
            }
        } else {
            bool wasActive = t.active;
            t.active = IsColumnMode(mode);
            if (t.active) {
                if (!t.overlay && !CreateOverlay(t)) {
                    t.active = false;
                } else {
                    t.details = mode == FVM_DETAILS;
                    ResetColumns(t, pidl);
                }
            } else if (wasActive) {
                Deactivate(t);
            }
        }
    } else if (t.active && t.overlay && IsColumnMode(mode) &&
               (mode == FVM_DETAILS) != t.details) {
        t.details = mode == FVM_DETAILS;
        ResetColumns(t, pidl);
    } else if (IsColumnMode(mode) != IsColumnMode(t.lastMode)) {
        t.active = IsColumnMode(mode);
        if (t.active) {
            if (!t.overlay && CreateOverlay(t)) {
                t.details = mode == FVM_DETAILS;
                ResetColumns(t, pidl);
            }
            t.active = t.overlay != nullptr;
        } else {
            Deactivate(t);
        }
    }
    CoTaskMemFree(t.lastPidl);
    t.lastPidl = pidl;
    t.lastMode = mode;

    if (t.active && t.overlay) {
        bool newView = defView != t.defView;
        HookDefView(t, defView);
        if (!IsWindowVisible(t.overlay)) {
            CheckNativeSelection(t, folderView.Get());
        } else if (NativeRenaming(t) && !t.columns.empty() &&
                   t.renameColumn < 0) {
            ShowNativeView(t);
        }
        // The native list may have been recreated.
        SyncNativeRegion(t);
        UpdateOverlayVisibility(t);
        // The focus stayed on the new native list (or is still on it when turning on).
        HWND focus = GetFocus();
        if (newView && focus && t.defView && IsWindowVisible(t.overlay) &&
            (focus == t.defView || IsChild(t.defView, focus))) {
            PostMessageW(t.overlay, WM_APP_FOCUSLIST, 0, 0);
        }
    } else {
        UnhookDefView(t);
    }
}

void CleanupTab(Tab& t) {
    KillTimer(t.tab, kTimerPoll);
    Deactivate(t);
    UnhookDefView(t);
    RestoreSidebar(t);
    CoTaskMemFree(t.lastPidl);
    t.lastPidl = nullptr;
    SetIgnoredSelection(t, nullptr);
}

LRESULT CALLBACK TabSubclassProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam,
                                 DWORD_PTR refData) {
    Tab* t = (Tab*)refData;
    if (msg == WM_TIMER && wParam == kTimerPoll) {
        Poll(*t);
        return 0;
    }
    if (msg == g_attachMsg && g_attachMsg) {
        SetTimer(hwnd, kTimerPoll, kPollInterval, nullptr);
        Poll(*t);
        return 0;
    }
    if (msg == g_settingsMsg && g_settingsMsg) {
        // Starts over with the new look.
        Deactivate(*t);
        RestoreSidebar(*t);
        CoTaskMemFree(t->lastPidl);
        t->lastPidl = nullptr;
        Poll(*t);
        return 0;
    }
    if (msg == g_detachMsg && g_detachMsg) {
        CleanupTab(*t);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TabSubclassProc);
        {
            std::lock_guard<std::mutex> lock(g_tabsMutex);
            g_tabs.erase(hwnd);
        }
        delete t;
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        // The subclass was already removed by Windhawk.
        CleanupTab(*t);
        {
            std::lock_guard<std::mutex> lock(g_tabsMutex);
            g_tabs.erase(hwnd);
        }
        delete t;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Finding the tabs
//
// A mod thread follows window creation in the process itself and hooks up
// each new tab; the ones that already existed are hooked up at startup.

HANDLE g_watcher;
DWORD g_watcherThreadId;

bool IsTabWindow(HWND hwnd) {
    WCHAR className[32];
    return GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
           !wcscmp(className, L"ShellTabWindowClass");
}

void AttachTab(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lock(g_tabsMutex);
        if (!g_tabs.insert(hwnd).second) {
            return;
        }
    }
    auto* t = new Tab();
    t->tab = hwnd;
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TabSubclassProc,
                                                       (DWORD_PTR)t)) {
        std::lock_guard<std::mutex> lock(g_tabsMutex);
        g_tabs.erase(hwnd);
        delete t;
        return;
    }
    PostMessageW(hwnd, g_attachMsg, 0, 0);
}

void CALLBACK OnWindowEvent(HWINEVENTHOOK,
                            DWORD,
                            HWND hwnd,
                            LONG idObject,
                            LONG idChild,
                            DWORD,
                            DWORD) {
    if (hwnd && idObject == OBJID_WINDOW && idChild == CHILDID_SELF &&
        IsTabWindow(hwnd)) {
        AttachTab(hwnd);
    }
}

void AttachExistingTabs() {
    EnumWindows(
        [](HWND hwnd, LPARAM) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            WCHAR className[32];
            if (pid == GetCurrentProcessId() &&
                GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
                !wcscmp(className, L"CabinetWClass")) {
                EnumChildWindows(
                    hwnd,
                    [](HWND child, LPARAM) -> BOOL {
                        if (IsTabWindow(child)) {
                            AttachTab(child);
                        }
                        return TRUE;
                    },
                    0);
            }
            return TRUE;
        },
        0);
}

DWORD WINAPI WatcherThread(void*) {
    HWINEVENTHOOK hook = SetWinEventHook(
        EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, nullptr, OnWindowEvent,
        GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    AttachExistingTabs();
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        DispatchMessageW(&msg);
    }
    if (hook) {
        UnhookWinEvent(hook);
    }
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    g_attachMsg = RegisterWindowMessageW(L"WindhawkFinderExplorer.Attach");
    g_detachMsg = RegisterWindowMessageW(L"WindhawkFinderExplorer.Detach");
    g_settingsMsg = RegisterWindowMessageW(L"WindhawkFinderExplorer.Settings");

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = Instance();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kOverlayClass;
    if (!RegisterClassExW(&wc)) {
        return FALSE;
    }

    g_watcher = CreateThread(nullptr, 0, WatcherThread, nullptr, 0,
                             &g_watcherThreadId);
    if (!g_watcher) {
        UnregisterClassW(kOverlayClass, Instance());
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_watcher) {
        PostThreadMessageW(g_watcherThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_watcher, INFINITE);
        CloseHandle(g_watcher);
        g_watcher = nullptr;
    }

    // Each tab turns the columns off on its own thread.
    std::vector<HWND> tabs;
    {
        std::lock_guard<std::mutex> lock(g_tabsMutex);
        tabs.assign(g_tabs.begin(), g_tabs.end());
    }
    for (HWND tab : tabs) {
        if (IsWindow(tab)) {
            SendMessageW(tab, g_detachMsg, 0, 0);
        }
    }
    UnregisterClassW(kOverlayClass, Instance());
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();
    std::lock_guard<std::mutex> lock(g_tabsMutex);
    for (HWND tab : g_tabs) {
        PostMessageW(tab, g_settingsMsg, 0, 0);
    }
    return TRUE;
}
