// ==WindhawkMod==
// @id              explorer-list-status-indent-fix
// @name            File Explorer List view indent fix and files area spacing
// @description     Removes the empty space before every item in the List, Small icons and Content views of File Explorer, and evens out the gaps around the items, icons and names of the files area
// @version         1.0
// @author          NDruce
// @github          https://github.com/NDruce
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer List view indent fix and files area spacing

Starting with Windows 11 version 22H2, File Explorer reserves a 28 pixels wide
area to the left of **every** item in the **List**, **Small icons** and
**Content** views.
The area is meant for sync status icons of cloud folders (OneDrive and other
sync providers), but it's reserved in every folder, including the folders
which have nothing to do with the cloud. The result is an odd indent between
the edge of an item and its icon.

The mod removes that area where there's nothing to show. It also fixes a
number of smaller spacing inconsistencies in the files area. Every adjustment
has a setting, and can be changed or turned off.

The mod only deals with the files area: the part of a folder window which
shows the files and the folders. It doesn't touch the navigation pane, the
command bar or the address bar.

## Before and after

All the files are selected in the screenshots, to show the gaps around the
items. The top half of each image is without the mod, the bottom half is with
the mod and its default settings.

**List**

![List view, before and after](https://i.imgur.com/wiG8znW.png)

**Small icons**

![Small icons view, before and after](https://i.imgur.com/zTgetJS.png)

**Content**

![Content view, before and after](https://i.imgur.com/hpi1nNR.png)

**Details**

![Details view, before and after](https://i.imgur.com/fYZUSwC.png)

**Tiles**

![Tiles view, before and after](https://i.imgur.com/aKs5Nfm.png)

## What it does

* **No empty status area.** The reserved area is removed where there's nothing
  to show. Items which have a status icon, and all the items inside of a cloud
  folder, still have it, so the status icons keep working and stay aligned.
  Optionally, the status icons can be hidden in the List and Small icons views
  altogether.
* **Even gaps around an item.** The gap between the border of an item and its
  content is the same on the left and on the right, in the List, Small icons,
  Tiles, Content and Details views. The size of the gap is configurable. The
  right gap of the List view has a correction of its own.
* **Narrower status icon area.** The same gap is used on both sides of a
  status icon, also in the Tiles and Content views.
* **Icon and name.** A configurable gap is added between the item icon and the
  name, the same in the List, Small icons and Details views. The names and the
  status icons can be moved vertically. The Small icons view has a separate
  offset for the names.
* **Rows of the Details view.** The gap between the icon and the name is made
  the same as in the List view. A negative offset of the names is taken from
  the height of the rows, so that the names stay centered in the rows.
* **Rows of the Content view.** The padding below the text of a row is
  configurable.
* **Even margins around the files area.** The files area gets margins on all
  four sides, in all views. The horizontal and the vertical margins are
  configurable separately.

## Defaults

Only the removal of the empty status area is a fix in the strict sense. The
other adjustments are a matter of taste, and they're turned on by default too.
To get the stock look of a part back, use these values:

* Even item spacing: off. That also turns off the gaps and the offsets of the
  icons and the names, and the adjustments of the Details and Content rows.
* Horizontal and vertical margins of the files area: 0.

## Scaling

The gaps, the paddings and the margins follow the display scaling. The
vertical offsets and the correction of the right gap are small corrections of
a pixel or two, and are applied in physical pixels regardless of the scaling.

Items which are already displayed when the display scaling changes keep the
old spacing until the folder is reopened or refreshed. With fractional scaling
such as 125%, the gaps can differ by a pixel because of rounding.

The mod works with the compact view of File Explorer turned on or off, and
with the item check boxes.

## Other programs

By default, the mod only applies to File Explorer (`explorer.exe`). That's the
recommended setup, and nothing has to be configured for it.

The same items view is used by the Open/Save dialogs of other programs. To fix
it there too, add the programs to the custom process inclusion list in the
Advanced tab of the mod, by the name of the executable file, for example
`notepad.exe`. `explorer.exe` doesn't have to be added, it's always included.

It's also possible to add `*`, which means all programs. The mod is then
loaded into every process, including the ones which never show a file dialog,
so adding only the programs you need is the lighter option.

Only 64-bit programs are supported. In a program other than File Explorer, the
mod does nothing until the program loads the items view (`explorerframe.dll`),
which usually happens when a file dialog is opened for the first time.

## How it works

The indent comes from `UIProperty::HasVisibleContent` in `explorerframe.dll`,
which reports the status icons area as having content in the List, Small icons
and Content views regardless of the actual content. The mod makes it report the
actual state.

Everything else is done by changing properties of DirectUI elements: of the
items and of the items area. The elements are adjusted when they're created,
and the windows which are already open are adjusted when the mod is loaded.
The functions which are exported by `dui70.dll` are used for that.

No files are modified. The adjustments are only made to elements which have
the expected stock values, so that the mod stays out of the way of other
customizations. When the mod is turned off, the stock values are restored.

The code of the status area fix was verified to have the expected shape in
Windows 11 builds 22621.317 - 22621.6274 (22H2, 23H2), 26100.1 - 26100.9974
(24H2, 25H2), 28000.1761 - 28000.3086 (26H1) and 26300 (26H2 preview). The
adjustments of the Details view rows were tested on build 26300 at 125%
scaling.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- statusIcons: auto
  $name: Sync status icons
  $description: >-
    In the List and Small icons views. The first option shows the status icons
    for the items which have them, and keeps the items of a cloud folder
    aligned.
  $options:
  - auto: Show where they exist
  - hidden: Never show
- equalSpacing: true
  $name: Even item spacing
  $description: >-
    Makes the gap to the left of an item equal to the gap to the right of it,
    and uses the same gap on both sides of a status icon. Applies to the List,
    Small icons, Tiles, Content and Details views.
- itemGap: 4
  $name: Gap on the left and on the right of an item
  $description: >-
    The visible gap between the item border and its content, on both sides,
    from 1 to 32. Used with the even item spacing. The stock gaps are 3 on
    the left and 5 on the right.
- listRightGapOffset: -1
  $name: Correction of the right gap in the List view
  $description: >-
    In pixels, from -4 to 0, used with the even item spacing. Added to the gap
    on the right of an item, in the List view only. With -1, the gap on the
    right is 1 pixel smaller than the gap on the left. Set to 0 to use the
    same gap on both sides.
- statusIconGap: 0
  $name: Gap between a status icon and the item icon
  $description: >-
    From 0 to 32, used with the even item spacing. Set to 0 to use the same
    gap as on the left and on the right of an item.
- statusIconOffset: 1
  $name: Vertical offset of the status icons
  $description: >-
    In pixels, from -4 to 4, in the List and Small icons views, used with the
    even item spacing. A positive value moves the status icons down, a
    negative value moves them up.
- nameOffset: -1
  $name: Vertical offset of the item names
  $description: >-
    In pixels, from -4 to 4, in the List and Details views, used with the even
    item spacing. A positive value moves the names down, a negative value
    moves them up. In the Details view, a negative value is taken from the
    height of the rows, so that the gaps above and below a name stay equal.
- nameOffsetSmallIcons: 0
  $name: Vertical offset of the item names in the Small icons view
  $description: >-
    In pixels, from -4 to 4, used with the even item spacing. A positive value
    moves the names down, a negative value moves them up.
- iconTextGap: 1
  $name: Extra gap between the item icon and the name
  $description: >-
    From 0 to 32, in the List, Small icons and Details views, used with the
    even item spacing.
- contentBottomPadding: 1
  $name: Bottom padding of a row in the Content view
  $description: >-
    The padding below the text of a row, from 0 to 64. The stock value is 5,
    the same as the padding above the text. Used with the even item spacing.
- viewMarginSize: 8
  $name: Horizontal margins of the files area
  $description: >-
    The margin on the left and on the right of the files area, in all views,
    from 0 to 64. The stock margins are 14 on the left and none on the right.
    Set both the horizontal and the vertical margins to 0 to keep the stock
    margins.
- viewMarginVertical: 9
  $name: Vertical margins of the files area
  $description: >-
    The margin at the top and at the bottom of the files area, in all views,
    from 0 to 64. The stock margins are 6 at the top and none at the bottom.
    Set both the horizontal and the vertical margins to 0 to keep the stock
    margins.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <map>
#include <set>
#include <string>
#include <utility>

// Sizes in relative pixels (pixels at 100% scaling).
constexpr int kStatusIconSize = 12;
// The visible gap to the right of the item text is the right padding of the
// item plus about 2 pixels of text overhang, minus the 1 pixel item border.
// Add the same to the left padding to make both gaps equal.
constexpr int kLeftPaddingExtra = 2;
// The item gap which the fixed sizes below are for.
constexpr int kItemGapDefault = 5;
// The height of an item of the List view in the compact view, and how much
// taller it is if the compact view is turned off.
constexpr int kCompactItemHeight = 20;
constexpr int kListItemExtraHeight = 12;
// The stock top and bottom padding of the text block in the Content view.
constexpr int kContentBottomPadding = 5;
// The stock left padding of an item in the Details view.
constexpr int kDetailsItemPadding = 4;
// In the Content view: the left padding of the item icon, and the padding
// which is added before the status icon to end up with the same position as
// in the other views.
constexpr int kContentIconPadding = 4;
constexpr int kContentIconPaddingNew = 3;
constexpr int kContentItemPadding = 3;
constexpr int kContentStatusPadding = 3;
// The large icons of the Tiles and Content views have an empty area of about
// this size on the sides of the image. It's taken into account for the gap
// between the status icon and the item icon.
// In half pixels.
constexpr int kTilesIconInset2 = 5;
constexpr int kContentIconInset2 = 3;
// The stock left and right padding of an item in the Tiles view.
constexpr int kTilesItemPadding = 4;
// The Details view: the stock height of a row, and the stock width and
// padding of the area of the item icon.
constexpr int kDetailsRowHeight = 22;
constexpr int kDetailsIconWidth = 18;
constexpr int kDetailsIconPaddingTop = 1;
constexpr int kDetailsIconPaddingRight = 2;
constexpr int kDetailsIconPaddingBottom = 2;
// The stock right padding of the name cell of a row in the Details view.
constexpr int kDetailsNamePaddingRight = 6;
constexpr int kViewMarginDefault = 9;
constexpr int kViewMarginHorizontalDefault = 8;
// The stock left padding of the items area, and the space above it.
constexpr int kViewLeftMargin = 14;
constexpr int kViewTopMargin = 6;

static struct {
    bool hidden;
    bool equalSpacing;
    // The horizontal margin.
    int viewMarginSize;
    int viewMarginVertical;
    int statusIconGap;
    int itemGap;
    int listRightGapOffset;
    int contentBottomPadding;
    int statusIconOffset;
    int nameOffset;
    int nameOffsetSmallIcons;
    int iconTextGap;
} g_settings;

// The element which the top level element is hidden for on the current
// thread, see UIProperty_HasVisibleContent_Hook.
static thread_local void* t_hideTopLevelFor;
static HMODULE g_dui70Module;
static HMODULE g_explorerFrameModule;
static UINT g_runFromWindowThreadMsg;

// Set once all the functions below are resolved and the hooks are set. Until
// then, the items views must not be touched.
static std::atomic<bool> g_initialized;
// In a program other than File Explorer, the hooks are only set once the
// program loads explorerframe.dll by itself.
static bool g_lateInit;
static std::atomic<bool> g_lateInitStarted;
static std::atomic<int> g_lateInitRefCount;
static std::atomic<bool> g_unloading;

// The original values of the item templates, used for restoring them.
struct TemplateOriginals {
    int stateIconsPaddingLeft;
    int stateIconsPaddingRight;
    int stateIconsPaddingTop;
    int stateIconsPaddingBottom;
    int stateIconsMinWidth;
    int stateIconsMinHeight;
    int visibleMinWidth;
    int visibleMinHeight;
};

static SRWLOCK g_originalsLock = SRWLOCK_INIT;
// The stock padding of an item comes from a style sheet and is only known
// once the item is added to the view. It's remembered here, per template and
// DPI, to be able to adjust new items right when they're created.
using ItemPaddingKey = std::pair<std::wstring, int>;
static std::map<ItemPaddingKey, TemplateOriginals> g_originals;
static std::map<ItemPaddingKey, RECT> g_itemPaddings;
static std::set<ItemPaddingKey> g_itemPaddingsRequested;
static std::map<ItemPaddingKey, int> g_itemPaddingFailures;
static PTP_WORK g_applyWork;

// DUser constants.
constexpr UINT kGG_NEXT = 1;
constexpr UINT kGG_PREV = 2;
constexpr UINT kGG_TOPCHILD = 3;

// DirectUI constants.
constexpr int kPI_Local = 1;
constexpr int kPI_Specified = 2;

// duser.dll exports.
static HANDLE(WINAPI* DUser_GetGadget)(HANDLE gadget, UINT cmd);

// dui70.dll exports.
using PropertyProc_t = void*(WINAPI*)();
static void*(WINAPI* DUI_ElementFromGadget)(HANDLE gadget);
static void*(WINAPI* Element_GetTopLevel_Original)(void* pThis);
static void*(WINAPI* Element_GetParent)(void* pThis);
static ATOM(WINAPI* Element_GetID)(void* pThis);
static PCWSTR(WINAPI* Element_GetClass)(void* pThis, void** ppValue);
static HANDLE(WINAPI* Element_GetDisplayNode)(void* pThis);
static void*(WINAPI* Element_FindDescendent)(void* pThis, ATOM id);
static const RECT*(WINAPI* Element_GetPadding)(void* pThis, void** ppValue);
static const SIZE*(WINAPI* Element_GetMinSize)(void* pThis, void** ppValue);
static HRESULT(WINAPI* Element_SetMinSize)(void* pThis, int cx, int cy);
static int(WINAPI* Element_GetWidth)(void* pThis);
static HRESULT(WINAPI* Element_SetWidth)(void* pThis, int width);
static HRESULT(WINAPI* Element_SetPadding)(void* pThis, int l, int t, int r, int b);
static void*(WINAPI* Element_GetValue)(void* pThis, PropertyProc_t prop, int index, void* updateCache);
static HRESULT(WINAPI* Element_SetValue)(void* pThis, PropertyProc_t prop, int index, void* value);
static HRESULT(WINAPI* Element_RemoveLocalValue)(void* pThis, PropertyProc_t prop);
static PropertyProc_t Element_PaddingProp;
static void*(WINAPI* Value_CreateSize)(int cx, int cy, int dynamicScale);
static const SIZE*(WINAPI* Value_GetSize)(void* pThis);
static void(WINAPI* Value_Release)(void* pThis);
static int(WINAPI* DUIXmlParser_ScaleRelativePixels)(void* pThis, int value);
static HRESULT(WINAPI* DUIXmlParser_CreateElement_Original)(void* pThis, PCWSTR resid, void* substitute, void* parent, DWORD* deferCookie, void** element);

// explorerframe.dll symbols.
static BOOL(WINAPI* UIProperty_HasVisibleContent_Original)(void* pThis);
static PropertyProc_t UIBase_VisibleMinSizeProp;

// Returns true if the compact view of File Explorer is turned on. It's off
// by default. Called from the threads of all the folder windows.
static bool IsCompactView() {
    static std::atomic<ULONGLONG> lastTime;
    static std::atomic<bool> lastResult;
    ULONGLONG time = GetTickCount64();
    ULONGLONG last = lastTime;
    if (!last || time - last > 500) {
        DWORD value = 0;
        DWORD size = sizeof(value);
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"UseCompactMode", RRF_RT_REG_DWORD, nullptr, &value, &size);
        lastResult = value != 0;
        lastTime = time;
    }

    return lastResult;
}

static bool StartsWith(PCWSTR str, PCWSTR prefix) {
    return wcsncmp(str, prefix, wcslen(prefix)) == 0;
}

enum class ItemKind {
    None,
    Compact,  // List, Small icons
    Tiles,
    Content,
    Details,
};

// Identifies an item by the name of its template.
static ItemKind GetItemKind(PCWSTR name) {
    if (!name) {
        return ItemKind::None;
    }

    if (StartsWith(name, L"item.list") || StartsWith(name, L"item.smallicons")) {
        return ItemKind::Compact;
    }

    if (StartsWith(name, L"item.tiles")) {
        return ItemKind::Tiles;
    }

    if (StartsWith(name, L"item.smart")) {
        return ItemKind::Content;
    }

    if (wcscmp(name, L"item.details") == 0) {
        return ItemKind::Details;
    }

    return ItemKind::None;
}

// The items of the Tiles and the Content views don't keep the name of the
// template, and there are several templates for each. Use a common name.
static std::wstring GetItemKindKey(ItemKind kind, PCWSTR name) {
    switch (kind) {
        case ItemKind::Tiles:
            return L"item.tiles";
        case ItemKind::Content:
            return L"item.smart";
        default:
            return name;
    }
}

// The item templates of the List and the Small icons views.
static bool IsTargetItemClass(PCWSTR itemClass) {
    return itemClass && (StartsWith(itemClass, L"item.list") ||
                         StartsWith(itemClass, L"item.smallicons"));
}

// DirectUI element IDs are atoms from the local atom table. An atom doesn't
// exist until the markup which uses it is loaded. It's deleted when the last
// user of it is gone, for example when the last folder window is closed, and
// it can get a different value when it's added again. So the atoms must be
// looked up every time, and must not be remembered.
static ATOM GetStateIconsAtom() {
    return FindAtomW(L"StateIcons");
}

static ATOM GetItemsHostAtom() {
    return FindAtomW(L"UIItemsHost");
}

static ATOM GetFileNameAtom() {
    return FindAtomW(L"FileName");
}

// The templates of the items area of a view.
static bool IsTargetCollectionClass(PCWSTR collectionClass) {
    return collectionClass && StartsWith(collectionClass, L"collection.");
}

static std::wstring GetElementClass(void* element) {
    std::wstring result;
    void* value = nullptr;
    PCWSTR elementClass = Element_GetClass(element, &value);
    if (elementClass) {
        result = elementClass;
    }

    if (value) {
        Value_Release(value);
    }

    return result;
}

// Returns true if the element is the sync status icons area of an item in the
// List or the Small icons view.
static bool IsTargetStateIcons(void* element) {
    ATOM id = Element_GetID(element);
    if (!id || id != GetStateIconsAtom()) {
        return false;
    }

    void* container = Element_GetParent(element);
    void* item = container ? Element_GetParent(container) : nullptr;
    return item && IsTargetItemClass(GetElementClass(item).c_str());
}

static RECT GetElementPadding(void* element) {
    RECT result{};
    void* value = nullptr;
    if (const RECT* rc = Element_GetPadding(element, &value)) {
        result = *rc;
    }

    if (value) {
        Value_Release(value);
    }

    return result;
}

static SIZE GetElementMinSize(void* element) {
    SIZE result{};
    void* value = nullptr;
    if (const SIZE* size = Element_GetMinSize(element, &value)) {
        result = *size;
    }

    if (value) {
        Value_Release(value);
    }

    return result;
}

// The properties of an element which was just created and isn't in a view yet
// hold values for 100% scaling. They're scaled once the element is added to a
// view. The values which are set by this mod are final and aren't scaled, so
// values which are read from a new element have to be scaled first.
static void ScaleIfNew(RECT& rc, int dpi, bool hosted) {
    if (!hosted) {
        rc.left = MulDiv(rc.left, dpi, 96);
        rc.top = MulDiv(rc.top, dpi, 96);
        rc.right = MulDiv(rc.right, dpi, 96);
        rc.bottom = MulDiv(rc.bottom, dpi, 96);
    }
}

static void ScaleIfNew(SIZE& size, int dpi, bool hosted) {
    if (!hosted) {
        size.cx = MulDiv(size.cx, dpi, 96);
        size.cy = MulDiv(size.cy, dpi, 96);
    }
}

static SIZE GetElementVisibleMinSize(void* element) {
    SIZE result{};
    if (!UIBase_VisibleMinSizeProp) {
        return result;
    }

    void* value = Element_GetValue(element, UIBase_VisibleMinSizeProp, kPI_Specified, nullptr);
    if (value) {
        if (const SIZE* size = Value_GetSize(value)) {
            result = *size;
        }

        Value_Release(value);
    }

    return result;
}

static void SetElementVisibleMinSize(void* element, int cx, int cy) {
    void* value = Value_CreateSize(cx, cy, 0);
    if (value) {
        Element_SetValue(element, UIBase_VisibleMinSizeProp, kPI_Local, value);
        Value_Release(value);
    }
}

// Sets the configured margins of the items area, or restores the stock
// padding. The stock padding is 14 pixels on the left and none on the other
// sides, with 6 more pixels above the items area which belong to its parent.
// Only touches the padding if it has the stock values or the adjusted values.
static void ApplyItemsHostPadding(void* host, int dpi, bool apply, bool hosted) {
    if (Element_GetID(host) != GetItemsHostAtom()) {
        return;
    }

    int stockLeft = MulDiv(kViewLeftMargin, dpi, 96);
    int marginX = MulDiv(g_settings.viewMarginSize, dpi, 96);
    int marginY = MulDiv(g_settings.viewMarginVertical, dpi, 96);
    int top = marginY - MulDiv(kViewTopMargin, dpi, 96);
    if (top < 0) {
        top = 0;
    }

    RECT padding = GetElementPadding(host);
    ScaleIfNew(padding, dpi, hosted);
    if (apply) {
        if (padding.left == stockLeft && !padding.top && !padding.right && !padding.bottom) {
            Element_SetPadding(host, marginX, top, marginX, marginY);
        }
    } else if ((marginX > 0 || marginY > 0) && padding.left == marginX && padding.top == top &&
               padding.right == marginX && padding.bottom == marginY) {
        Element_SetPadding(host, stockLeft, 0, 0, 0);
    }
}

// The extra left padding of an item with a small icon (the List, Small icons
// and Details views), in pixels. The gap on the right is larger than the
// padding by the overhang of the text, about 2 pixels at 100% scaling. It
// grows slower than the scaling, so it's rounded down. On the left, the image
// of a small icon gets an empty edge with scaling: none in a 16 pixels icon,
// 1 pixel in the 20 and 24 pixels icons, 2 pixels in a 32 pixels icon.
// Measured at 100% and 125%.
static int GetSmallIconLeftExtra(int dpi) {
    int overhang = kLeftPaddingExtra * dpi / 96;
    int iconInset = (dpi - 96 + 47) / 48;
    return overhang > iconInset ? overhang - iconInset : 0;
}

// Makes the left padding of an item larger so that the visible gaps on both
// sides of the item content are equal, or restores the stock padding.
static void ApplyItemPadding(void* item, ItemKind kind, const std::wstring& key, int dpi, bool apply, const RECT& padding) {
    // The configured gap changes the left and the right padding by the same
    // amount.
    int delta = MulDiv(g_settings.itemGap - kItemGapDefault, dpi, 96);
    int extra = MulDiv(kind == ItemKind::Content ? kContentItemPadding : kLeftPaddingExtra, dpi, 96);
    if (kind == ItemKind::Compact || kind == ItemKind::Details) {
        extra = GetSmallIconLeftExtra(dpi);
    }

    if (kind == ItemKind::Content) {
        extra += delta;
        if (extra < 0) {
            extra = 0;
        }
    }

    if (kind == ItemKind::Content) {
        // No stock padding.
        if (apply && !padding.left) {
            Element_SetPadding(item, extra, padding.top, padding.right, padding.bottom);
        } else if (!apply && padding.left == extra) {
            // The stock padding is an explicit zero, which overrides the
            // padding of the style sheet.
            Element_SetPadding(item, 0, padding.top, padding.right, padding.bottom);
        }

        return;
    }

    ItemPaddingKey paddingKey{key, dpi};

    // The correction of the right padding, for the List view only. It's never
    // positive, so that an adjusted padding can always be told from the
    // symmetric stock padding.
    int rightOffset = StartsWith(key.c_str(), L"item.list") ? g_settings.listRightGapOffset : 0;

    if (padding.left > 0) {
        // The item is in the view and the padding is known. Only touch it if
        // it's the stock padding, or if it was already adjusted. The stock
        // padding is symmetric, except for the Details view, in which there's
        // only a left padding of 4 pixels.
        bool details = kind == ItemKind::Details;
        int rightDelta = details ? 0 : delta;
        int stockLeft;
        int stockRight;
        bool stock;
        bool adjusted;
        if (details) {
            stockLeft = MulDiv(kDetailsItemPadding, dpi, 96);
            stockRight = padding.right;
            stock = padding.left == stockLeft;
            adjusted = !stock && padding.left == stockLeft + extra + delta;
        } else {
            // Adjusted, the left padding is larger than the right padding by
            // the extra padding and the correction of the List view,
            // regardless of the configured gap.
            stock = padding.left == padding.right;
            adjusted = !stock && padding.left == padding.right - rightOffset + extra;
            stockRight = stock ? padding.right : padding.right - rightDelta - rightOffset;
            stockLeft = stockRight;
        }

        if (stockLeft <= 0 || (!stock && !adjusted)) {
            return;
        }

        AcquireSRWLockExclusive(&g_originalsLock);
        g_itemPaddings[paddingKey] = RECT{stockLeft, padding.top, stockRight, padding.bottom};
        ReleaseSRWLockExclusive(&g_originalsLock);

        int newLeft = stockLeft + extra + delta;
        int newRight = stockRight + rightDelta + rightOffset;
        if (apply && !adjusted && newLeft >= 0 && newRight >= 0) {
            Element_SetPadding(item, newLeft, padding.top, newRight, padding.bottom);
        } else if (!apply && adjusted) {
            Element_RemoveLocalValue(item, Element_PaddingProp);
        }

        return;
    }

    if (!apply) {
        return;
    }

    // The item was just created and isn't in the view yet.
    bool known = false;
    bool request = false;
    RECT stockPadding{};
    AcquireSRWLockExclusive(&g_originalsLock);
    if (auto it = g_itemPaddings.find(paddingKey); it != g_itemPaddings.end()) {
        known = true;
        stockPadding = it->second;
    } else {
        request = g_itemPaddingsRequested.insert(paddingKey).second;
    }
    ReleaseSRWLockExclusive(&g_originalsLock);

    if (known) {
        int newLeft = stockPadding.left + extra + delta;
        int newRight = stockPadding.right + (kind == ItemKind::Details ? 0 : delta + rightOffset);
        if (newLeft >= 0 && newRight >= 0) {
            Element_SetPadding(item, newLeft, stockPadding.top, newRight, stockPadding.bottom);
        }
    } else if (request) {
        // Learn the padding and adjust the items once they're in the view.
        // The lock keeps the work object from being closed in the meantime.
        AcquireSRWLockExclusive(&g_originalsLock);
        if (g_applyWork) {
            SubmitThreadpoolWork(g_applyWork);
        }
        ReleaseSRWLockExclusive(&g_originalsLock);
    }
}

// Adjusts the stock padding of the name of a row in the Details view.
static void AdjustDetailsNamePadding(RECT& padding, int dpi) {
    // The area of the item icon loses its right padding, see ApplyDetailsRow.
    // With the same extra gap, the gap between the icon and the name is then
    // the same as in the List view.
    padding.left += MulDiv(g_settings.iconTextGap, dpi, 96);

    if (g_settings.nameOffset < 0) {
        // No top padding to take from. The rows are made lower by the offset,
        // see ApplyDetailsRow. The name is centered with its padding, so with
        // a bottom padding of the same size, the name moves up by the offset
        // and the gaps above and below it stay equal.
        padding.bottom -= g_settings.nameOffset;
    } else {
        // A padding of 2 pixels moves the name by 1 pixel.
        padding.top += g_settings.nameOffset * 2;
    }
}

// The cells of a row in the Details view are created after the row itself.
// Adjusts the name cell right when it's created.
static void ApplyDetailsNameCell(void* cell, int dpi) {
    ATOM id = Element_GetID(cell);
    if (id && id != GetFileNameAtom()) {
        return;
    }

    // A new element holds the values for 100% scaling. Only touch the stock
    // padding.
    RECT padding = GetElementPadding(cell);
    RECT rawStock{0, 0, kDetailsNamePaddingRight, 0};
    RECT stock{0, 0, MulDiv(kDetailsNamePaddingRight, dpi, 96), 0};
    if (!EqualRect(&padding, &stock) && !EqualRect(&padding, &rawStock)) {
        return;
    }

    RECT target = stock;
    AdjustDetailsNamePadding(target, dpi);
    if (!EqualRect(&padding, &target)) {
        Element_SetPadding(cell, target.left, target.top, target.right, target.bottom);
    }
}

// Adjusts the padding of the item name in the row-like views: adds a gap
// between the item icon and the name, and moves the name vertically. Or
// restores the stock padding.
static void ApplyNamePadding(void* item, ItemKind kind, const std::wstring& key, int dpi, bool apply, bool hosted) {
    if (kind != ItemKind::Compact && kind != ItemKind::Details) {
        return;
    }

    ATOM fileNameAtom = GetFileNameAtom();
    void* fileName = fileNameAtom ? Element_FindDescendent(item, fileNameAtom) : nullptr;
    if (!fileName || (kind == ItemKind::Compact && Element_GetParent(fileName) != item)) {
        return;
    }

    RECT padding = GetElementPadding(fileName);
    ScaleIfNew(padding, dpi, hosted);

    // The height of an item of the List view, which is larger if the compact
    // view is turned off. The items of the Small icons view have the same
    // height either way.
    int itemHeight = 0;
    if (StartsWith(key.c_str(), L"item.list")) {
        itemHeight = MulDiv(kCompactItemHeight, dpi, 96);
        if (!IsCompactView()) {
            itemHeight += MulDiv(kListItemExtraHeight, dpi, 96);
        }
    }

    // The stock padding is derived from the current padding, nothing is
    // remembered: a remembered value can be wrong for an item which was
    // created with other options of File Explorer. If the padding doesn't
    // have the expected shape, it isn't touched.
    RECT target;
    if (kind == ItemKind::Details) {
        // The stock padding is on the right only. The right padding is never
        // changed.
        target = RECT{0, 0, MulDiv(kDetailsNamePaddingRight, dpi, 96), 0};
        if (padding.right != target.right) {
            return;
        }
    } else {
        // The stock padding is the same at the top and at the bottom, with
        // none on the sides. The offset of the name only moves padding
        // between the top and the bottom, so the sum stays the same.
        int vertical = padding.top + padding.bottom;
        if (padding.right || (vertical & 1)) {
            return;
        }

        target = RECT{0, vertical / 2, 0, vertical / 2};
    }

    if (apply && kind == ItemKind::Details) {
        AdjustDetailsNamePadding(target, dpi);
    } else if (apply) {
        target.left += MulDiv(g_settings.iconTextGap, dpi, 96);

        // The Small icons view has an offset of its own.
        int nameOffset = StartsWith(key.c_str(), L"item.smallicons") ? g_settings.nameOffsetSmallIcons
                                                                     : g_settings.nameOffset;

        // The name and the icon are centered in the item separately. If the
        // height of the item differs from the compact height by an odd
        // amount, the rounding puts the name a pixel higher relative to the
        // icon than in the compact view. Compensate for that, so that the
        // offset gives the same result with any height.
        int compactHeight = MulDiv(kCompactItemHeight, dpi, 96);
        if (itemHeight > compactHeight && ((itemHeight - compactHeight) & 1)) {
            nameOffset++;
        }

        if (kind == ItemKind::Compact) {
            // Move padding between the top and the bottom.
            int top = target.top + nameOffset;
            if (top < 0) {
                top = 0;
            }

            int bottom = target.bottom + target.top - top;
            if (bottom >= 0) {
                target.top = top;
                target.bottom = bottom;
            }
        }
    }

    if (!EqualRect(&padding, &target)) {
        Element_SetPadding(fileName, target.left, target.top, target.right, target.bottom);
    }
}

// Adjusts a row of the Details view, or restores the stock values:
// * Removes the right padding of the area of the item icon, which makes the
//   gap between the icon and the name the same as in the List view.
// * With a negative offset of the names, makes the row lower by the offset.
//   The item icon keeps its position relative to the name.
// Only touches the values which are the stock values or the adjusted values.
// A new row which isn't in a view yet holds the values for 100% scaling.
// Called when a row is created, and again when its name cell is created, as
// the row isn't complete at first.
static void ApplyDetailsRow(void* item, int dpi, bool apply) {
    int shrink = g_settings.nameOffset < 0 ? -g_settings.nameOffset : 0;

    // The height of the row. The sync status cell of a cloud folder has the
    // same minimum height.
    int stockHeight = MulDiv(kDetailsRowHeight, dpi, 96);
    ATOM syncStatusAtom = FindAtomW(L"SyncStatus");
    void* syncStatus = syncStatusAtom ? Element_FindDescendent(item, syncStatusAtom) : nullptr;
    for (void* element : {item, syncStatus}) {
        if (!element || !shrink || stockHeight - shrink <= 0) {
            continue;
        }

        SIZE minSize = GetElementMinSize(element);
        if (apply) {
            if (minSize.cy == stockHeight) {
                Element_SetMinSize(element, minSize.cx, stockHeight - shrink);
            } else if (minSize.cy == kDetailsRowHeight) {
                Element_SetMinSize(element, MulDiv(minSize.cx, dpi, 96), stockHeight - shrink);
            }
        } else if (minSize.cy == stockHeight - shrink) {
            Element_SetMinSize(element, minSize.cx, stockHeight);
        }
    }

    // The area of the item icon is a child of the row, identified by its
    // stock width and padding.
    int stockWidth = MulDiv(kDetailsIconWidth, dpi, 96);
    RECT rawStock{0, kDetailsIconPaddingTop, kDetailsIconPaddingRight, kDetailsIconPaddingBottom};
    RECT stock{0, MulDiv(kDetailsIconPaddingTop, dpi, 96), MulDiv(kDetailsIconPaddingRight, dpi, 96),
               MulDiv(kDetailsIconPaddingBottom, dpi, 96)};
    RECT adjusted{0, stock.top + shrink, 0, stock.bottom};
    if (stockWidth <= stock.right) {
        return;
    }

    HANDLE itemGadget = Element_GetDisplayNode(item);
    for (HANDLE child = itemGadget ? DUser_GetGadget(itemGadget, kGG_TOPCHILD) : nullptr; child;
         child = DUser_GetGadget(child, kGG_NEXT)) {
        void* icon = DUI_ElementFromGadget(child);
        if (!icon) {
            continue;
        }

        // The width is set after the row is created, so it's not
        // necessarily known yet.
        int width = Element_GetWidth(icon);
        RECT padding = GetElementPadding(icon);
        bool hasStockWidth = width == stockWidth || width == kDetailsIconWidth;
        if (apply) {
            if (EqualRect(&padding, &stock) || EqualRect(&padding, &rawStock)) {
                Element_SetPadding(icon, adjusted.left, adjusted.top, adjusted.right, adjusted.bottom);
                if (hasStockWidth) {
                    Element_SetWidth(icon, stockWidth - stock.right);
                }

                break;
            }

            if (EqualRect(&padding, &adjusted)) {
                if (hasStockWidth) {
                    Element_SetWidth(icon, stockWidth - stock.right);
                }

                break;
            }
        } else if (EqualRect(&padding, &adjusted)) {
            if (width == stockWidth - stock.right) {
                Element_SetWidth(icon, stockWidth);
            }

            Element_SetPadding(icon, stock.left, stock.top, stock.right, stock.bottom);
            break;
        }
    }
}

// Applies or restores the spacing of an item. The stock layout of an item, at
// 100% scaling:
//
//   List:        4 | status area 28 (6 + icon 12 + 10) | icon 16 | text | 4
//   Small icons: 4 | status area 28 (icon 12 + 16)     | icon 16 | text | 4
//   Tiles:       4 | status area 34 (icon 12 + 22)     | icon 48 | ...
//   Content:     0 | status area 30 (icon 12 + 18)     | 4 + icon 32 | ...
//                (adjusted: 3 | 3 + icon 12 + gap | 3 + icon 32)
//   Details:     4 | icon 16 | text...
//
// The adjusted layout has 6 pixels before the first icon, and the configured
// gap between the status icon and the item icon. With the item border of 1
// pixel, the result is a visible gap of 5 pixels on the left. The status area
// takes no space if there's nothing to show.
static void ApplyItemSpacing(void* item, ItemKind kind, const std::wstring& key, int dpi, bool apply, bool hosted) {
    // The status area of the item. There's none in the Details view, in which
    // the status is a column.
    ATOM atom = kind != ItemKind::Details ? GetStateIconsAtom() : 0;
    void* stateIcons = atom ? Element_FindDescendent(item, atom) : nullptr;
    void* container = stateIcons ? Element_GetParent(stateIcons) : nullptr;
    void* containerParent = container ? Element_GetParent(container) : nullptr;
    bool hasStatusArea = containerParent && (containerParent == item || Element_GetParent(containerParent) == item);

    // The items of the Tiles and the Content views are identified by the
    // items area which they're in, and not by a name. Don't touch an element
    // which doesn't look like an item.
    if (!hasStatusArea && (kind == ItemKind::Tiles || kind == ItemKind::Content)) {
        return;
    }

    ApplyItemPadding(item, kind, key, dpi, apply, GetElementPadding(item));
    ApplyNamePadding(item, kind, key, dpi, apply, hosted);

    if (kind == ItemKind::Details) {
        ApplyDetailsRow(item, dpi, apply);
        return;
    }

    if (!hasStatusArea) {
        return;
    }

    RECT stateIconsPadding = GetElementPadding(stateIcons);
    SIZE stateIconsMinSize = GetElementMinSize(stateIcons);
    SIZE visibleMinSize = GetElementVisibleMinSize(container);
    ScaleIfNew(stateIconsPadding, dpi, hosted);
    ScaleIfNew(stateIconsMinSize, dpi, hosted);
    ScaleIfNew(visibleMinSize, dpi, hosted);

    // The values of an item which is seen for the first time are the stock
    // values. If nothing was adjusted yet, there's nothing to restore.
    TemplateOriginals originals{
        .stateIconsPaddingLeft = stateIconsPadding.left,
        .stateIconsPaddingRight = stateIconsPadding.right,
        .stateIconsPaddingTop = stateIconsPadding.top,
        .stateIconsPaddingBottom = stateIconsPadding.bottom,
        .stateIconsMinWidth = stateIconsMinSize.cx,
        .stateIconsMinHeight = stateIconsMinSize.cy,
        .visibleMinWidth = visibleMinSize.cx,
        .visibleMinHeight = visibleMinSize.cy,
    };
    {
        ItemPaddingKey originalsKey{key, dpi};
        AcquireSRWLockExclusive(&g_originalsLock);
        if (auto it = g_originals.find(originalsKey); it != g_originals.end()) {
            originals = it->second;
        } else if (apply) {
            g_originals.emplace(originalsKey, originals);
        }
        ReleaseSRWLockExclusive(&g_originalsLock);
    }

    int stateIconsPaddingLeft = originals.stateIconsPaddingLeft;
    int stateIconsPaddingRight = originals.stateIconsPaddingRight;
    int stateIconsPaddingTop = originals.stateIconsPaddingTop;
    int stateIconsPaddingBottom = originals.stateIconsPaddingBottom;
    if (apply && kind == ItemKind::Compact) {
        // The status icon is centered with its padding, so that a padding of
        // 2 pixels moves it by 1 pixel.
        if (g_settings.statusIconOffset > 0) {
            stateIconsPaddingTop += g_settings.statusIconOffset * 2;
        } else {
            stateIconsPaddingBottom -= g_settings.statusIconOffset * 2;
        }
    }

    int stateIconsMinWidth = originals.stateIconsMinWidth;
    int visibleMinWidth = originals.visibleMinWidth;

    // In the Content view, the height of a row is the height of the text
    // block: 5 + two lines + 5. The status area has the same minimum height.
    // Change both by the same amount for the configured bottom padding.
    int stateIconsMinHeight = originals.stateIconsMinHeight;
    int visibleMinHeight = originals.visibleMinHeight;
    int contentBottomStock = MulDiv(kContentBottomPadding, dpi, 96);
    int contentBottom = contentBottomStock;
    if (apply && kind == ItemKind::Content) {
        contentBottom = MulDiv(g_settings.contentBottomPadding, dpi, 96);
        if (stateIconsMinHeight > 0) {
            stateIconsMinHeight += contentBottom - contentBottomStock;
            if (stateIconsMinHeight < 1) {
                stateIconsMinHeight = 1;
            }
        }

        if (visibleMinHeight > 0) {
            visibleMinHeight += contentBottom - contentBottomStock;
            if (visibleMinHeight < 1) {
                visibleMinHeight = 1;
            }
        }
    }
    if (apply) {
        // In the Content view, the item has no padding of its own and the
        // item icon has a left padding. The status area is sized by its
        // content and padding there.
        int paddingLeft = MulDiv(kind == ItemKind::Content ? kContentStatusPadding : 0, dpi, 96);
        // If not set, the gap of the status icon is the same as the item gap.
        int statusIconGap = g_settings.statusIconGap > 0 ? g_settings.statusIconGap : g_settings.itemGap;
        int delta = MulDiv(g_settings.itemGap - kItemGapDefault, dpi, 96);
        int gap = MulDiv(statusIconGap, dpi, 96);
        if (kind == ItemKind::Tiles || kind == ItemKind::Content) {
            // Make the visible gap to the right of the status icon the same as
            // the visible gap to the left of it, in pixels. The left gap is
            // the offset of the status icon minus the item border. On the
            // right, the item icon has an empty area on its sides, and a
            // padding in the Content view.
            int left = kind == ItemKind::Content
                           ? MulDiv(kContentItemPadding, dpi, 96) + delta + paddingLeft
                           : MulDiv(kTilesItemPadding, dpi, 96) + MulDiv(kLeftPaddingExtra, dpi, 96) + delta;
            gap = left - 1 + MulDiv(statusIconGap - g_settings.itemGap, dpi, 96);
            if (kind == ItemKind::Content) {
                gap -= MulDiv(kContentIconPaddingNew, dpi, 96) + MulDiv(kContentIconInset2, dpi, 192);
            } else {
                gap -= MulDiv(kTilesIconInset2, dpi, 192);
            }
        }

        if (gap < 0) {
            gap = 0;
        }

        int statusAreaWidth = paddingLeft + MulDiv(kStatusIconSize, dpi, 96) + gap;
        stateIconsPaddingLeft = paddingLeft;
        if (kind == ItemKind::Content) {
            stateIconsPaddingRight = gap;
        }

        if (stateIconsMinWidth > 0) {
            stateIconsMinWidth = statusAreaWidth;
        }

        if (visibleMinWidth > 0) {
            visibleMinWidth = statusAreaWidth;
        }
    }

    if (stateIconsPadding.left != stateIconsPaddingLeft || stateIconsPadding.right != stateIconsPaddingRight ||
        stateIconsPadding.top != stateIconsPaddingTop || stateIconsPadding.bottom != stateIconsPaddingBottom) {
        Element_SetPadding(stateIcons, stateIconsPaddingLeft, stateIconsPaddingTop, stateIconsPaddingRight, stateIconsPaddingBottom);
    }

    if (kind == ItemKind::Content) {
        // The text block is the last element of the item. Only touch its
        // bottom padding if the top padding has the stock value, and the
        // bottom padding has the stock value or the adjusted value.
        HANDLE itemGadget = Element_GetDisplayNode(item);
        HANDLE textGadget = itemGadget ? DUser_GetGadget(itemGadget, kGG_TOPCHILD) : nullptr;
        void* textBlock = textGadget ? DUI_ElementFromGadget(textGadget) : nullptr;
        if (textBlock && textBlock != container && Element_GetParent(textBlock) == item) {
            RECT textPadding = GetElementPadding(textBlock);
            ScaleIfNew(textPadding, dpi, hosted);
            int adjustedBottom = MulDiv(g_settings.contentBottomPadding, dpi, 96);
            if (textPadding.top == contentBottomStock &&
                (textPadding.bottom == contentBottomStock || textPadding.bottom == adjustedBottom) &&
                textPadding.bottom != contentBottom) {
                Element_SetPadding(textBlock, textPadding.left, textPadding.top, textPadding.right, contentBottom);
            }
        }
    }

    if (kind == ItemKind::Content) {
        // Make the left padding of the item icon smaller. It's the element
        // which follows the status area.
        HANDLE containerGadget = Element_GetDisplayNode(container);
        HANDLE imageGadget = containerGadget ? DUser_GetGadget(containerGadget, kGG_PREV) : nullptr;
        void* image = imageGadget ? DUI_ElementFromGadget(imageGadget) : nullptr;
        if (image && Element_GetParent(image) == item) {
            int stockLeft = MulDiv(kContentIconPadding, dpi, 96);
            int newLeft = MulDiv(kContentIconPaddingNew, dpi, 96);
            RECT imagePadding = GetElementPadding(image);
            ScaleIfNew(imagePadding, dpi, hosted);
            if (apply && imagePadding.left == stockLeft) {
                Element_SetPadding(image, newLeft, imagePadding.top, imagePadding.right, imagePadding.bottom);
            } else if (!apply && imagePadding.left == newLeft) {
                Element_SetPadding(image, stockLeft, imagePadding.top, imagePadding.right, imagePadding.bottom);
            }
        }
    }

    // Change and restore a property of the status area to make it measured
    // again, as the answer to whether it has something to show might have
    // changed.
    RECT containerPadding = GetElementPadding(container);
    ScaleIfNew(containerPadding, dpi, hosted);
    Element_SetPadding(container, containerPadding.left, containerPadding.top, containerPadding.right + 1, containerPadding.bottom);
    Element_SetPadding(container, containerPadding.left, containerPadding.top, containerPadding.right, containerPadding.bottom);

    if (stateIconsMinSize.cx > 0 && stateIconsMinWidth > 0 &&
        (stateIconsMinSize.cx != stateIconsMinWidth || stateIconsMinSize.cy != stateIconsMinHeight)) {
        Element_SetMinSize(stateIcons, stateIconsMinWidth, stateIconsMinHeight);
    }

    if (visibleMinSize.cx > 0 && visibleMinWidth > 0 &&
        (visibleMinSize.cx != visibleMinWidth || visibleMinSize.cy != visibleMinHeight)) {
        SetElementVisibleMinSize(container, visibleMinWidth, visibleMinHeight);
    }
}

static void* WINAPI Element_GetTopLevel_Hook(void* pThis) {
    if (pThis && t_hideTopLevelFor == pThis) {
        return nullptr;
    }

    return Element_GetTopLevel_Original(pThis);
}

static BOOL WINAPI UIProperty_HasVisibleContent_Hook(void* pThis) {
    BOOL result = UIProperty_HasVisibleContent_Original(pThis);
    if (!result || g_unloading) {
        return result;
    }

    ATOM id = Element_GetID(pThis);
    if (!id || id != GetStateIconsAtom()) {
        return result;
    }

    if (g_settings.hidden && IsTargetStateIcons(pThis)) {
        return FALSE;
    }

    // The original function reports the status icons area as visible in the
    // List, Small icons and Content views regardless of whether there's
    // anything to show. It does that by looking at the view mode of the top
    // level element. Hide the top level element from it to get the actual
    // answer. In other views, the answer stays the same.
    t_hideTopLevelFor = pThis;
    result = UIProperty_HasVisibleContent_Original(pThis);
    t_hideTopLevelFor = nullptr;

    return result;
}

static HRESULT WINAPI DUIXmlParser_CreateElement_Hook(void* pThis, PCWSTR resid, void* substitute, void* parent, DWORD* deferCookie, void** element) {
    HRESULT hr = DUIXmlParser_CreateElement_Original(pThis, resid, substitute, parent, deferCookie, element);
    if (FAILED(hr) || !element || !*element || !resid || g_unloading) {
        return hr;
    }

    if ((g_settings.viewMarginSize > 0 || g_settings.viewMarginVertical > 0) && IsTargetCollectionClass(resid)) {
        ATOM atom = GetItemsHostAtom();
        void* host = atom ? Element_FindDescendent(*element, atom) : nullptr;
        int dpi = DUIXmlParser_ScaleRelativePixels(pThis, 96);
        if (host && dpi > 0) {
            ApplyItemsHostPadding(host, dpi, true, false);
        }
    }

    ItemKind kind = GetItemKind(resid);
    if (g_settings.equalSpacing && kind != ItemKind::None) {
        int dpi = DUIXmlParser_ScaleRelativePixels(pThis, 96);
        if (dpi > 0) {
            ApplyItemSpacing(*element, kind, GetItemKindKey(kind, resid), dpi, true, false);
        }
    }

    if (g_settings.equalSpacing && wcscmp(resid, L"property.details.first") == 0) {
        int dpi = DUIXmlParser_ScaleRelativePixels(pThis, 96);
        if (dpi > 0) {
            ApplyDetailsNameCell(*element, dpi);

            // The row of the cell.
            void* row = parent;
            for (int i = 0; i < 3 && row; i++) {
                if (GetItemKind(GetElementClass(row).c_str()) == ItemKind::Details) {
                    ApplyDetailsRow(row, dpi, true);
                    break;
                }

                row = Element_GetParent(row);
            }
        }
    }

    return hr;
}

// Applies or restores the spacing of the existing items.
struct ApplyParam {
    bool spacing;
    bool viewMargin;
};

static void ApplyRecursive(void* element, int dpi, const ApplyParam& param, int depth = 0) {
    if (depth > 64) {
        return;
    }

    std::wstring elementClass = GetElementClass(element);
    ItemKind kind = GetItemKind(elementClass.c_str());
    if (kind != ItemKind::None) {
        ApplyItemSpacing(element, kind, GetItemKindKey(kind, elementClass.c_str()), dpi, param.spacing, true);
        return;
    }

    // The items of the Tiles and the Content views don't keep the name of the
    // template. Identify them by the items area.
    ItemKind childKind = ItemKind::None;
    if (Element_GetID(element) == GetItemsHostAtom()) {
        ApplyItemsHostPadding(element, dpi, param.viewMargin, true);

        if (elementClass == L"itemgridlayout.tiles") {
            childKind = ItemKind::Tiles;
        } else if (StartsWith(elementClass.c_str(), L"collection.smart")) {
            childKind = ItemKind::Content;
        }
    }
    HANDLE gadget = Element_GetDisplayNode(element);
    if (!gadget) {
        return;
    }

    for (HANDLE child = DUser_GetGadget(gadget, kGG_TOPCHILD); child; child = DUser_GetGadget(child, kGG_NEXT)) {
        void* childElement = DUI_ElementFromGadget(child);
        if (!childElement) {
            continue;
        }

        if (childKind != ItemKind::None) {
            ApplyItemSpacing(childElement, childKind, GetItemKindKey(childKind, L""), dpi, param.spacing, true);
        } else {
            ApplyRecursive(childElement, dpi, param, depth + 1);
        }
    }

    if (childKind == ItemKind::Content) {
        // The rows of the Content view have the same height, which is kept by
        // the items area. Change and restore a property of the items area to
        // make it measure the rows again.
        RECT padding = GetElementPadding(element);
        Element_SetPadding(element, padding.left, padding.top, padding.right, padding.bottom + 1);
        Element_SetPadding(element, padding.left, padding.top, padding.right, padding.bottom);
    }
}

static void* GetItemsViewElement(HWND hWnd) {
    // A DirectUIHWND window keeps a pointer to its root element, or null if
    // there's no element. It's read from the thread of the window.
    void* element = (void*)GetWindowLongPtrW(hWnd, 0);
    if (!element) {
        return nullptr;
    }

    // Make sure that it's indeed an element.
    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            *(PCWSTR*)element, &module) ||
        (module != g_explorerFrameModule && module != g_dui70Module)) {
        return nullptr;
    }

    return element;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(HWND hWnd, void* param);

struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t proc;
    HWND hWnd;
    void* procParam;
};

static LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_runFromWindowThreadMsg) {
            auto* param = (RunFromWindowThreadParam*)cwp->lParam;
            param->proc(param->hWnd, param->procParam);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(hWnd, procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHookProc, nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParam param{proc, hWnd, procParam};
    SendMessageW(hWnd, g_runFromWindowThreadMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

static void WINAPI ApplyToItemsViewWindow(HWND hWnd, void* param) {
    if (void* element = GetItemsViewElement(hWnd)) {
        ApplyRecursive(element, GetDpiForWindow(hWnd), *(const ApplyParam*)param);
    }
}

// Applies the adjustments to the existing items, or restores the stock values
// if not active.
static void ApplyToExistingWindows(bool active) {
    if (!g_initialized) {
        return;
    }

    ApplyParam enumParam{
        .spacing = active && g_settings.equalSpacing,
        .viewMargin = active && (g_settings.viewMarginSize > 0 || g_settings.viewMarginVertical > 0),
    };

    auto enumChildProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        WCHAR className[32];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) && wcscmp(className, L"DirectUIHWND") == 0 &&
            GetClassNameW(GetParent(hWnd), className, ARRAYSIZE(className)) &&
            wcscmp(className, L"SHELLDLL_DefView") == 0) {
            RunFromWindowThread(hWnd, ApplyToItemsViewWindow, (void*)lParam);
        }

        return TRUE;
    };

    auto enumProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD processId = 0;
        if (GetWindowThreadProcessId(hWnd, &processId) && processId == GetCurrentProcessId()) {
            auto enumChildProc = (WNDENUMPROC)((void**)lParam)[0];
            EnumChildWindows(hWnd, enumChildProc, (LPARAM)((void**)lParam)[1]);
        }

        return TRUE;
    };

    void* params[] = {(void*)(WNDENUMPROC)enumChildProc, &enumParam};
    EnumWindows(enumProc, (LPARAM)params);
}

// Returns true if the stock padding is known for all the item templates for
// which it was requested.
static bool AreRequestedItemPaddingsKnown() {
    bool known = true;
    AcquireSRWLockExclusive(&g_originalsLock);
    for (const auto& key : g_itemPaddingsRequested) {
        if (!g_itemPaddings.contains(key)) {
            known = false;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_originalsLock);
    return known;
}

static void CALLBACK ApplyWorkCallback(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    // The new item which the padding was requested for isn't necessarily in
    // the view yet, and while File Explorer is starting, the view isn't
    // necessarily in its window yet. Try again for a while until the padding
    // is learned. Otherwise, the new items of that template would be left
    // with the stock padding.
    constexpr int kAttempts = 30;
    constexpr int kMaxFailures = 3;
    for (int i = 0; i < kAttempts; i++) {
        if (i > 0) {
            Sleep(100);
        }

        if (g_unloading) {
            return;
        }

        ApplyToExistingWindows(true);
        if (AreRequestedItemPaddingsKnown()) {
            return;
        }
    }

    // Still unknown. Let the next new item request it again, a limited amount
    // of times.
    AcquireSRWLockExclusive(&g_originalsLock);
    for (auto it = g_itemPaddingsRequested.begin(); it != g_itemPaddingsRequested.end();) {
        if (!g_itemPaddings.contains(*it) && ++g_itemPaddingFailures[*it] <= kMaxFailures) {
            it = g_itemPaddingsRequested.erase(it);
        } else {
            ++it;
        }
    }
    ReleaseSRWLockExclusive(&g_originalsLock);
}

static void LoadSettings() {
    g_settings.hidden = wcscmp(WindhawkUtils::StringSetting::make(L"statusIcons"), L"hidden") == 0;

    g_settings.equalSpacing = Wh_GetIntSetting(L"equalSpacing");

    g_settings.viewMarginSize = Wh_GetIntSetting(L"viewMarginSize");
    if (g_settings.viewMarginSize < 0 || g_settings.viewMarginSize > 64) {
        g_settings.viewMarginSize = kViewMarginHorizontalDefault;
    }

    g_settings.viewMarginVertical = Wh_GetIntSetting(L"viewMarginVertical");
    if (g_settings.viewMarginVertical < 0 || g_settings.viewMarginVertical > 64) {
        g_settings.viewMarginVertical = kViewMarginDefault;
    }

    g_settings.statusIconOffset = Wh_GetIntSetting(L"statusIconOffset");
    if (g_settings.statusIconOffset < -4 || g_settings.statusIconOffset > 4) {
        g_settings.statusIconOffset = 1;
    }

    g_settings.nameOffset = Wh_GetIntSetting(L"nameOffset");
    if (g_settings.nameOffset < -4 || g_settings.nameOffset > 4) {
        g_settings.nameOffset = -1;
    }

    g_settings.nameOffsetSmallIcons = Wh_GetIntSetting(L"nameOffsetSmallIcons");
    if (g_settings.nameOffsetSmallIcons < -4 || g_settings.nameOffsetSmallIcons > 4) {
        g_settings.nameOffsetSmallIcons = 0;
    }

    g_settings.iconTextGap = Wh_GetIntSetting(L"iconTextGap");
    if (g_settings.iconTextGap < 0 || g_settings.iconTextGap > 32) {
        g_settings.iconTextGap = 1;
    }

    g_settings.statusIconGap = Wh_GetIntSetting(L"statusIconGap");
    if (g_settings.statusIconGap < 0 || g_settings.statusIconGap > 32) {
        g_settings.statusIconGap = 0;
    }

    g_settings.contentBottomPadding = Wh_GetIntSetting(L"contentBottomPadding");
    if (g_settings.contentBottomPadding < 0 || g_settings.contentBottomPadding > 64) {
        g_settings.contentBottomPadding = 1;
    }

    g_settings.listRightGapOffset = Wh_GetIntSetting(L"listRightGapOffset");
    if (g_settings.listRightGapOffset < -4 || g_settings.listRightGapOffset > 0) {
        g_settings.listRightGapOffset = -1;
    }

    g_settings.itemGap = Wh_GetIntSetting(L"itemGap");
    if (g_settings.itemGap < 1 || g_settings.itemGap > 32) {
        g_settings.itemGap = 4;
    }
}

template <typename T>
static bool GetProc(HMODULE module, PCSTR name, T* proc) {
    *proc = (T)GetProcAddress(module, name);
    if (!*proc) {
        Wh_Log(L"Missing export: %S", name);
        return false;
    }

    return true;
}

// The modules which were loaded by the mod, to release the references when
// the mod is unloaded or when setting it up fails.
static HMODULE g_loadedModules[3];

static HMODULE LoadSystemModule(PCWSTR name) {
    HMODULE module = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module) {
        for (HMODULE& slot : g_loadedModules) {
            if (!slot) {
                slot = module;
                break;
            }
        }
    }

    return module;
}

static void FreeLoadedModules() {
    for (HMODULE& slot : g_loadedModules) {
        if (slot) {
            FreeLibrary(slot);
            slot = nullptr;
        }
    }
}

// Resolves the functions and sets the hooks. The modules are loaded if they
// aren't loaded yet.
static BOOL SetupHooksImpl() {
    HMODULE duser = LoadSystemModule(L"duser.dll");
    g_dui70Module = LoadSystemModule(L"dui70.dll");
    g_explorerFrameModule = LoadSystemModule(L"explorerframe.dll");
    if (!duser || !g_dui70Module || !g_explorerFrameModule) {
        return FALSE;
    }

    HMODULE dui70 = g_dui70Module;
    void*(WINAPI* getTopLevel)(void*);
    HRESULT(WINAPI* createElement)(void*, PCWSTR, void*, void*, DWORD*, void**);
    if (!GetProc(duser, "GetGadget", &DUser_GetGadget) ||
        !GetProc(dui70, "ElementFromGadget", &DUI_ElementFromGadget) ||
        !GetProc(dui70, "?GetTopLevel@Element@DirectUI@@QEAAPEAV12@XZ", &getTopLevel) ||
        !GetProc(dui70, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ", &Element_GetParent) ||
        !GetProc(dui70, "?GetID@Element@DirectUI@@QEAAGXZ", &Element_GetID) ||
        !GetProc(dui70, "?GetClass@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z", &Element_GetClass) ||
        !GetProc(dui70, "?GetDisplayNode@Element@DirectUI@@QEAAPEAUHGADGET__@@XZ", &Element_GetDisplayNode) ||
        !GetProc(dui70, "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z", &Element_FindDescendent) ||
        !GetProc(dui70, "?GetPadding@Element@DirectUI@@QEAAPEBUtagRECT@@PEAPEAVValue@2@@Z", &Element_GetPadding) ||
        !GetProc(dui70, "?GetMinSize@Element@DirectUI@@QEAAPEBUtagSIZE@@PEAPEAVValue@2@@Z", &Element_GetMinSize) ||
        !GetProc(dui70, "?SetMinSize@Element@DirectUI@@QEAAJHH@Z", &Element_SetMinSize) ||
        !GetProc(dui70, "?GetWidth@Element@DirectUI@@QEAAHXZ", &Element_GetWidth) ||
        !GetProc(dui70, "?SetWidth@Element@DirectUI@@QEAAJH@Z", &Element_SetWidth) ||
        !GetProc(dui70, "?SetPadding@Element@DirectUI@@QEAAJHHHH@Z", &Element_SetPadding) ||
        !GetProc(dui70, "?GetValue@Element@DirectUI@@QEAAPEAVValue@2@P6APEBUPropertyInfo@2@XZHPEAUUpdateCache@2@@Z", &Element_GetValue) ||
        !GetProc(dui70, "?SetValue@Element@DirectUI@@QEAAJP6APEBUPropertyInfo@2@XZHPEAVValue@2@@Z", &Element_SetValue) ||
        !GetProc(dui70, "?RemoveLocalValue@Element@DirectUI@@QEAAJP6APEBUPropertyInfo@2@XZ@Z", &Element_RemoveLocalValue) ||
        !GetProc(dui70, "?PaddingProp@Element@DirectUI@@SAPEBUPropertyInfo@2@XZ", &Element_PaddingProp) ||
        !GetProc(dui70, "?CreateSize@Value@DirectUI@@SAPEAV12@HHW4DynamicScaleValue@@@Z", &Value_CreateSize) ||
        !GetProc(dui70, "?GetSize@Value@DirectUI@@QEAAPEBUtagSIZE@@XZ", &Value_GetSize) ||
        !GetProc(dui70, "?Release@Value@DirectUI@@QEAAXXZ", &Value_Release) ||
        !GetProc(dui70, "?_ScaleRelativePixels@DUIXmlParser@DirectUI@@AEBAHH@Z", &DUIXmlParser_ScaleRelativePixels) ||
        !GetProc(dui70, "?CreateElement@DUIXmlParser@DirectUI@@QEAAJPEBGPEAVElement@2@1PEAKPEAPEAV32@@Z", &createElement)) {
        return FALSE;
    }

    // explorerframe.dll
    WindhawkUtils::SYMBOL_HOOK explorerFrameHooks[] = {
        {
            {LR"(public: virtual int __cdecl UIProperty::HasVisibleContent(void))"},
            &UIProperty_HasVisibleContent_Original,
            UIProperty_HasVisibleContent_Hook,
        },
        {
            {LR"(public: static struct DirectUI::PropertyInfo const * __cdecl UIBase::VisibleMinSizeProp(void))"},
            &UIBase_VisibleMinSizeProp,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(g_explorerFrameModule, explorerFrameHooks, ARRAYSIZE(explorerFrameHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    // Nothing fails from here on.
    PTP_WORK work = CreateThreadpoolWork(ApplyWorkCallback, nullptr, nullptr);
    AcquireSRWLockExclusive(&g_originalsLock);
    g_applyWork = work;
    ReleaseSRWLockExclusive(&g_originalsLock);

    WindhawkUtils::SetFunctionHook(getTopLevel, Element_GetTopLevel_Hook, &Element_GetTopLevel_Original);
    WindhawkUtils::SetFunctionHook(createElement, DUIXmlParser_CreateElement_Hook, &DUIXmlParser_CreateElement_Original);

    return TRUE;
}

static BOOL SetupHooks() {
    if (SetupHooksImpl()) {
        return TRUE;
    }

    FreeLoadedModules();
    return FALSE;
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) {
        return false;
    }

    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

// Sets the hooks in a program which didn't have explorerframe.dll loaded when
// the mod was loaded. Only done once.
static void LateInit() {
    g_lateInitRefCount++;

    if (!g_unloading && !g_lateInitStarted.exchange(true)) {
        Wh_Log(L"explorerframe.dll is loaded, setting the hooks");

        if (!SetupHooks()) {
            Wh_Log(L"Setting the hooks failed");
        } else if (!g_unloading) {
            g_initialized = true;
            Wh_ApplyHookOperations();
        }
    }

    g_lateInitRefCount--;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module && !g_lateInitStarted) {
        DWORD lastError = GetLastError();
        if (module == GetModuleHandleW(L"explorerframe.dll")) {
            LateInit();
        }

        SetLastError(lastError);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_runFromWindowThreadMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    // File Explorer uses the items view itself, so that explorerframe.dll and
    // the modules it depends on are loaded there anyway. The same is true for
    // any program which has it loaded already.
    if (IsExplorerProcess() || GetModuleHandleW(L"explorerframe.dll")) {
        if (!SetupHooks()) {
            return FALSE;
        }

        g_initialized = true;
        return TRUE;
    }

    // Another program, which might never show a file dialog. Don't load
    // anything into it, wait for the program to load explorerframe.dll.
    // duser.dll and dui70.dll are loaded along with it.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase ? (LoadLibraryExW_t)GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
    if (!loadLibraryExW ||
        !WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
        Wh_Log(L"Failed to hook LoadLibraryExW");
        return FALSE;
    }

    g_lateInit = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // In case explorerframe.dll was loaded before the LoadLibraryExW hook was
    // applied.
    if (g_lateInit && GetModuleHandleW(L"explorerframe.dll")) {
        LateInit();
    }

    // Apply to the items which already exist. That also causes the items to be
    // measured again.
    ApplyToExistingWindows(true);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    AcquireSRWLockExclusive(&g_originalsLock);
    PTP_WORK work = std::exchange(g_applyWork, nullptr);
    ReleaseSRWLockExclusive(&g_originalsLock);
    if (work) {
        WaitForThreadpoolWorkCallbacks(work, TRUE);
        CloseThreadpoolWork(work);
    }

    ApplyToExistingWindows(false);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Don't unload in the middle of setting the hooks. Wait for a limited
    // time, the symbols might be loading.
    for (int i = 0; i < 500 && g_lateInitRefCount > 0; i++) {
        Sleep(10);
    }

    FreeLoadedModules();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Restore with the old settings, then apply the new settings.
    ApplyToExistingWindows(false);
    LoadSettings();
    ApplyToExistingWindows(true);
}
