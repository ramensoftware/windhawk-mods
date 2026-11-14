// ==WindhawkMod==
// @id              explorer-list-status-indent-fix
// @name            File Explorer spacing fixes: List view indent and more
// @description     Fixes item indents and spacing in File Explorer, first of all the empty space before every item in the List view. Also fixes it in Small icons, Content and Tiles, evens out gaps, margins and the position of status icons and names
// @version         1.0
// @author          NDruce
// @github          https://github.com/NDruce
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer spacing fixes: List view indent and more

Starting with Windows 11 version 22H2, File Explorer reserves a 28 pixels wide
area to the left of **every** item in the **List**, **Small icons** and
**Content** views.
The area is meant for sync status icons of cloud folders (OneDrive and other
sync providers), but it's reserved in every folder, including the folders
which have nothing to do with the cloud. The result is an odd indent between
the edge of an item and its icon.

This mod:

* Removes the reserved area where there's nothing to show. Items which have a
  status icon, and all the items inside of a cloud folder, still have it, so
  the status icons keep working and stay aligned.
* Makes the status icon area narrower, with equal gaps on both sides of the
  status icon, also in the Tiles and Content views (optional).
* Makes the gap to the left of an item content the same in the List, Small
  icons, Tiles, Content and Details views, equal to the gap to the right of
  the item text (optional).
* Makes the margins around the files area equal on all sides, in all views
  (optional, the size is configurable).
* Adds a gap between the item icon and the name in the List, Small icons and
  Details views, and can move the names and the status icons vertically
  (optional).
* Can move the expand arrows of the navigation pane vertically (optional).
* Optionally, hides the status icons in the List and Small icons views
  altogether.

All the sizes follow the display scaling. Items which are already displayed
when the display scaling changes keep the old spacing until the folder is
reopened or refreshed. With fractional scaling such as 125%, the gaps can
differ by a pixel because of rounding.

## Other programs

By default, the mod only applies to File Explorer. The same items view is used
by the Open/Save dialogs of other programs. To fix it there too, add the
programs (or `*` for all programs) to the custom process inclusion list in the
Advanced tab of the mod.

## How it works

The indent comes from `UIProperty::HasVisibleContent` in `explorerframe.dll`,
which reports the status icons area as having content in the List, Small icons
and Content views regardless of the actual content. The mod makes it report the
actual state. The spacing is adjusted by changing properties
of the DirectUI elements of an item, using the functions which are exported by
`dui70.dll`. No code or resources are patched, and the adjustments are only
made to elements which have the expected stock values, so that the mod stays
out of the way of other customizations.

The relevant code was verified to have the expected shape in Windows 11 builds
22621.317 - 22621.6274 (22H2, 23H2), 26100.1 - 26100.9974 (24H2, 25H2),
28000.1761 - 28000.3086 (26H1) and 26300 (26H2 preview).
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
    The visible gap between the item border and its content, on both sides.
    Used with the even item spacing. The stock gaps are 3 on the left and 5
    on the right.
- statusIconGap: 0
  $name: Gap between a status icon and the item icon
  $description: >-
    Used with the even item spacing. Set to 0 to use the same gap as on the
    left and on the right of an item.
- statusIconOffset: 0
  $name: Vertical offset of the status icons
  $description: >-
    In pixels, in the List and Small icons views, used with the even item
    spacing. A positive value moves the status icons down, a negative value
    moves them up.
- nameOffset: 0
  $name: Vertical offset of the item names
  $description: >-
    In pixels, in the List, Small icons and Details views, used with the even
    item spacing. A positive value moves the names down, a negative value
    moves them up.
- iconTextGap: 1
  $name: Extra gap between the item icon and the name
  $description: >-
    In the List, Small icons and Details views, used with the even item
    spacing.
- navArrowOffset: -1
  $name: Vertical offset of the expand arrows in the navigation pane
  $description: >-
    In pixels. A negative value moves the arrows up. Applies to all tree views
    of the process, and takes effect when the tree is redrawn.
- contentBottomPadding: 2
  $name: Bottom padding of a row in the Content view
  $description: >-
    The padding below the text of a row. The stock value is 5, the same as
    the padding above the text. Used with the even item spacing.
- viewMarginSize: 9
  $name: Margins around the files area
  $description: >-
    The same margin is used on all four sides, in all views. The stock margins
    are 14 on the left, 6 at the top and none on the right and at the bottom.
    Set to 0 to keep the stock margins.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

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
// The gap between the status icon area and the item icon. The status icons
// are 10 to 12 pixels wide in an area of 12 pixels, so that a gap of 5 results
// in a visible gap of 5 to 7 pixels.
// The item gap which the fixed sizes below are for.
constexpr int kItemGapDefault = 5;
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
constexpr int kViewMarginDefault = 9;
// The stock left padding of the items area, and the space above it.
constexpr int kViewLeftMargin = 14;
constexpr int kViewTopMargin = 6;

static struct {
    bool hidden;
    bool equalSpacing;
    int viewMarginSize;
    int statusIconGap;
    int itemGap;
    int contentBottomPadding;
    int statusIconOffset;
    int nameOffset;
    int iconTextGap;
    int navArrowOffset;
} g_settings;

static DWORD g_tlsIndex = TLS_OUT_OF_INDEXES;
static ATOM g_stateIconsAtom;
static ATOM g_itemsHostAtom;
static ATOM g_fileNameAtom;
static HMODULE g_dui70Module;
static HMODULE g_explorerFrameModule;
static UINT g_runFromWindowThreadMsg;

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
static std::map<ItemPaddingKey, RECT> g_nameOriginals;
static std::set<ItemPaddingKey> g_itemPaddingsRequested;
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

static ATOM GetStateIconsAtom() {
    // DirectUI element IDs are atoms from the local atom table. The atom
    // doesn't exist until the items view markup is loaded.
    if (!g_stateIconsAtom) {
        g_stateIconsAtom = FindAtomW(L"StateIcons");
    }

    return g_stateIconsAtom;
}

static ATOM GetItemsHostAtom() {
    if (!g_itemsHostAtom) {
        g_itemsHostAtom = FindAtomW(L"UIItemsHost");
    }

    return g_itemsHostAtom;
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

// Makes the padding of the items area equal on all sides, or restores the
// stock padding. The stock padding is 14 pixels on the left and none on the
// other sides, with 6 more pixels above the items area which belong to its
// parent. Only touches the padding if it has the stock values or the adjusted
// values.
static void ApplyItemsHostPadding(void* host, int dpi, bool apply, bool hosted) {
    if (Element_GetID(host) != GetItemsHostAtom()) {
        return;
    }

    int stockLeft = MulDiv(kViewLeftMargin, dpi, 96);
    int margin = MulDiv(g_settings.viewMarginSize, dpi, 96);
    int top = margin - MulDiv(kViewTopMargin, dpi, 96);
    if (top < 0) {
        top = 0;
    }

    RECT padding = GetElementPadding(host);
    ScaleIfNew(padding, dpi, hosted);
    if (apply) {
        if (padding.left == stockLeft && !padding.top && !padding.right && !padding.bottom) {
            Element_SetPadding(host, margin, top, margin, margin);
        }
    } else if (margin > 0 && padding.left == margin && padding.top == top && padding.right == margin && padding.bottom == margin) {
        Element_SetPadding(host, stockLeft, 0, 0, 0);
    }
}

// Makes the left padding of an item larger so that the visible gaps on both
// sides of the item content are equal, or restores the stock padding.
static void ApplyItemPadding(void* item, ItemKind kind, const std::wstring& key, int dpi, bool apply, const RECT& padding) {
    // The configured gap changes the left and the right padding by the same
    // amount.
    int delta = MulDiv(g_settings.itemGap - kItemGapDefault, dpi, 96);
    int extra = MulDiv(kind == ItemKind::Content ? kContentItemPadding : kLeftPaddingExtra, dpi, 96);
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
            // the extra padding, regardless of the configured gap.
            stock = padding.left == padding.right;
            adjusted = !stock && padding.left == padding.right + extra;
            stockRight = stock ? padding.right : padding.right - rightDelta;
            stockLeft = stockRight;
        }

        if (stockLeft <= 0 || (!stock && !adjusted)) {
            return;
        }

        AcquireSRWLockExclusive(&g_originalsLock);
        g_itemPaddings[paddingKey] = RECT{stockLeft, padding.top, stockRight, padding.bottom};
        ReleaseSRWLockExclusive(&g_originalsLock);

        int newLeft = stockLeft + extra + delta;
        int newRight = stockRight + rightDelta;
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
        int newRight = stockPadding.right + (kind == ItemKind::Details ? 0 : delta);
        if (newLeft >= 0 && newRight >= 0) {
            Element_SetPadding(item, newLeft, stockPadding.top, newRight, stockPadding.bottom);
        }
    } else if (request && g_applyWork) {
        // Learn the padding and adjust the items once they're in the view.
        SubmitThreadpoolWork(g_applyWork);
    }
}

// Adjusts the padding of the item name in the row-like views: adds a gap
// between the item icon and the name, and moves the name vertically. Or
// restores the stock padding.
static void ApplyNamePadding(void* item, ItemKind kind, const std::wstring& key, int dpi, bool apply, bool hosted) {
    if (kind != ItemKind::Compact && kind != ItemKind::Details) {
        return;
    }

    if (!g_fileNameAtom) {
        g_fileNameAtom = FindAtomW(L"FileName");
    }

    void* fileName = g_fileNameAtom ? Element_FindDescendent(item, g_fileNameAtom) : nullptr;
    if (!fileName || (kind == ItemKind::Compact && Element_GetParent(fileName) != item)) {
        return;
    }

    RECT padding = GetElementPadding(fileName);
    ScaleIfNew(padding, dpi, hosted);

    RECT target = padding;
    {
        ItemPaddingKey originalsKey{key, dpi};
        AcquireSRWLockExclusive(&g_originalsLock);
        if (auto it = g_nameOriginals.find(originalsKey); it != g_nameOriginals.end()) {
            target = it->second;
        } else if (apply) {
            g_nameOriginals.emplace(originalsKey, padding);
        }
        ReleaseSRWLockExclusive(&g_originalsLock);
    }

    if (apply) {
        target.left += MulDiv(g_settings.iconTextGap, dpi, 96);

        if (kind == ItemKind::Compact) {
            // Move padding between the top and the bottom.
            int top = target.top + g_settings.nameOffset;
            if (top < 0) {
                top = 0;
            }

            int bottom = target.bottom + target.top - top;
            if (bottom >= 0) {
                target.top = top;
                target.bottom = bottom;
            }
        } else if (g_settings.nameOffset < 0) {
            // No top padding to take from. The name is centered with its
            // padding, so that a padding of 2 pixels moves it by 1 pixel.
            target.bottom -= g_settings.nameOffset * 2;
        } else {
            target.top += g_settings.nameOffset * 2;
        }
    }

    if (!EqualRect(&padding, &target)) {
        Element_SetPadding(fileName, target.left, target.top, target.right, target.bottom);
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
    ApplyItemPadding(item, kind, key, dpi, apply, GetElementPadding(item));
    ApplyNamePadding(item, kind, key, dpi, apply, hosted);

    if (kind == ItemKind::Details) {
        // The status is a column.
        return;
    }

    ATOM atom = GetStateIconsAtom();
    void* stateIcons = atom ? Element_FindDescendent(item, atom) : nullptr;
    void* container = stateIcons ? Element_GetParent(stateIcons) : nullptr;
    void* containerParent = container ? Element_GetParent(container) : nullptr;
    if (!containerParent || (containerParent != item && Element_GetParent(containerParent) != item)) {
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

    // Change and restore a property of the status area to make it measured
    // again, as the answer to whether it has something to show might have
    // changed.
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
// uxtheme.dll
constexpr int kTVP_TREEITEM = 1;
constexpr int kTVP_GLYPH = 2;
constexpr int kTVP_HOTGLYPH = 4;
static BOOL(WINAPI* Uxtheme_IsThemePartDefined)(HANDLE hTheme, int partId, int stateId);
static HRESULT(WINAPI* DrawThemeBackground_Original)(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const RECT* clipRect);
static HRESULT(WINAPI* DrawThemeBackgroundEx_Original)(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const void* options);

// Returns true for the expand arrow of a tree view, such as the navigation
// pane. The theme class is identified by its parts: a tree view theme has
// parts up to 4.
static bool IsTreeViewGlyph(HANDLE hTheme, int partId, const RECT* rect) {
    if (!g_settings.navArrowOffset || (partId != kTVP_GLYPH && partId != kTVP_HOTGLYPH) || !rect) {
        return false;
    }

    return Uxtheme_IsThemePartDefined(hTheme, kTVP_TREEITEM, 0) &&
           Uxtheme_IsThemePartDefined(hTheme, kTVP_HOTGLYPH, 0) &&
           !Uxtheme_IsThemePartDefined(hTheme, kTVP_HOTGLYPH + 1, 0);
}

static HRESULT WINAPI DrawThemeBackground_Hook(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const RECT* clipRect) {
    if (IsTreeViewGlyph(hTheme, partId, rect)) {
        RECT rc = *rect;
        OffsetRect(&rc, 0, g_settings.navArrowOffset);
        return DrawThemeBackground_Original(hTheme, hdc, partId, stateId, &rc, clipRect);
    }

    return DrawThemeBackground_Original(hTheme, hdc, partId, stateId, rect, clipRect);
}

static HRESULT WINAPI DrawThemeBackgroundEx_Hook(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const void* options) {
    if (IsTreeViewGlyph(hTheme, partId, rect)) {
        RECT rc = *rect;
        OffsetRect(&rc, 0, g_settings.navArrowOffset);
        return DrawThemeBackgroundEx_Original(hTheme, hdc, partId, stateId, &rc, options);
    }

    return DrawThemeBackgroundEx_Original(hTheme, hdc, partId, stateId, rect, options);
}

static void* WINAPI Element_GetTopLevel_Hook(void* pThis) {
    if (g_tlsIndex != TLS_OUT_OF_INDEXES && pThis &&
        TlsGetValue(g_tlsIndex) == pThis) {
        return nullptr;
    }

    return Element_GetTopLevel_Original(pThis);
}

static BOOL WINAPI UIProperty_HasVisibleContent_Hook(void* pThis) {
    BOOL result = UIProperty_HasVisibleContent_Original(pThis);
    if (!result) {
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
    TlsSetValue(g_tlsIndex, pThis);
    result = UIProperty_HasVisibleContent_Original(pThis);
    TlsSetValue(g_tlsIndex, nullptr);

    return result;
}

static HRESULT WINAPI DUIXmlParser_CreateElement_Hook(void* pThis, PCWSTR resid, void* substitute, void* parent, DWORD* deferCookie, void** element) {
    HRESULT hr = DUIXmlParser_CreateElement_Original(pThis, resid, substitute, parent, deferCookie, element);
    if (FAILED(hr) || !element || !*element || !resid) {
        return hr;
    }

    if (g_settings.viewMarginSize > 0 && IsTargetCollectionClass(resid)) {
        ATOM atom = GetItemsHostAtom();
        void* host = atom ? Element_FindDescendent(*element, atom) : nullptr;
        int dpi = DUIXmlParser_ScaleRelativePixels(pThis, 96);
        if (host && dpi > 0) {
            ApplyItemsHostPadding(host, dpi, true, false);
        }
    }

    if (ItemKind kind = GetItemKind(resid); g_settings.equalSpacing && kind != ItemKind::None) {
        int dpi = DUIXmlParser_ScaleRelativePixels(pThis, 96);
        if (dpi > 0) {
            ApplyItemSpacing(*element, kind, GetItemKindKey(kind, resid), dpi, true, false);
        }
    }
    return hr;
}

// Applies or restores the spacing of the existing items.
struct ApplyParam {
    bool active;
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
    // A DirectUIHWND window keeps a pointer to its root element.
    void* element = (void*)GetWindowLongPtrW(hWnd, 0);
    if (!element || IsBadReadPtr(element, sizeof(void*))) {
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
    ApplyParam enumParam{
        .active = active,
        .spacing = active && g_settings.equalSpacing,
        .viewMargin = active && g_settings.viewMarginSize > 0,
    };

    auto enumChildProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        WCHAR className[32];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) && wcscmp(className, L"DirectUIHWND") == 0 &&
            GetClassNameW(GetParent(hWnd), className, ARRAYSIZE(className)) && wcscmp(className, L"SHELLDLL_DefView") == 0) {
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

static void CALLBACK ApplyWorkCallback(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    ApplyToExistingWindows(true);
}

static void LoadSettings() {
    PCWSTR statusIcons = Wh_GetStringSetting(L"statusIcons");
    g_settings.hidden = wcscmp(statusIcons, L"hidden") == 0;
    Wh_FreeStringSetting(statusIcons);

    g_settings.equalSpacing = Wh_GetIntSetting(L"equalSpacing");
    g_settings.viewMarginSize = Wh_GetIntSetting(L"viewMarginSize");
    if (g_settings.viewMarginSize < 0 || g_settings.viewMarginSize > 64) {
        g_settings.viewMarginSize = kViewMarginDefault;
    }

    g_settings.statusIconOffset = Wh_GetIntSetting(L"statusIconOffset");
    if (g_settings.statusIconOffset < -4 || g_settings.statusIconOffset > 4) {
        g_settings.statusIconOffset = 0;
    }

    g_settings.nameOffset = Wh_GetIntSetting(L"nameOffset");
    if (g_settings.nameOffset < -4 || g_settings.nameOffset > 4) {
        g_settings.nameOffset = 0;
    }

    g_settings.iconTextGap = Wh_GetIntSetting(L"iconTextGap");
    if (g_settings.iconTextGap < 0 || g_settings.iconTextGap > 32) {
        g_settings.iconTextGap = 0;
    }

    g_settings.navArrowOffset = Wh_GetIntSetting(L"navArrowOffset");
    if (g_settings.navArrowOffset < -8 || g_settings.navArrowOffset > 8) {
        g_settings.navArrowOffset = 0;
    }

    g_settings.statusIconGap = Wh_GetIntSetting(L"statusIconGap");
    if (g_settings.statusIconGap < 0 || g_settings.statusIconGap > 32) {
        g_settings.statusIconGap = 0;
    }

    g_settings.contentBottomPadding = Wh_GetIntSetting(L"contentBottomPadding");
    if (g_settings.contentBottomPadding < 0 || g_settings.contentBottomPadding > 64) {
        g_settings.contentBottomPadding = 2;
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

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_runFromWindowThreadMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    HMODULE duser = LoadLibraryExW(L"duser.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    g_dui70Module = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    g_explorerFrameModule = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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

    g_tlsIndex = TlsAlloc();
    if (g_tlsIndex == TLS_OUT_OF_INDEXES) {
        return FALSE;
    }

    g_applyWork = CreateThreadpoolWork(ApplyWorkCallback, nullptr, nullptr);

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

    // The expand arrows of tree views.
    if (HMODULE uxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        Uxtheme_IsThemePartDefined = (decltype(Uxtheme_IsThemePartDefined))GetProcAddress(uxtheme, "IsThemePartDefined");
        auto drawThemeBackground = (decltype(DrawThemeBackground_Original))GetProcAddress(uxtheme, "DrawThemeBackground");
        auto drawThemeBackgroundEx = (decltype(DrawThemeBackgroundEx_Original))GetProcAddress(uxtheme, "DrawThemeBackgroundEx");
        if (Uxtheme_IsThemePartDefined && drawThemeBackground && drawThemeBackgroundEx) {
            WindhawkUtils::SetFunctionHook(drawThemeBackground, DrawThemeBackground_Hook, &DrawThemeBackground_Original);
            WindhawkUtils::SetFunctionHook(drawThemeBackgroundEx, DrawThemeBackgroundEx_Hook, &DrawThemeBackgroundEx_Original);
        }
    }

    WindhawkUtils::SetFunctionHook(getTopLevel, Element_GetTopLevel_Hook, &Element_GetTopLevel_Original);
    WindhawkUtils::SetFunctionHook(createElement, DUIXmlParser_CreateElement_Hook, &DUIXmlParser_CreateElement_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Apply to the items which already exist. That also causes the items to be
    // measured again.
    ApplyToExistingWindows(true);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (g_applyWork) {
        PTP_WORK work = g_applyWork;
        g_applyWork = nullptr;
        WaitForThreadpoolWorkCallbacks(work, TRUE);
        CloseThreadpoolWork(work);
    }

    ApplyToExistingWindows(false);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_tlsIndex != TLS_OUT_OF_INDEXES) {
        TlsFree(g_tlsIndex);
        g_tlsIndex = TLS_OUT_OF_INDEXES;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Restore with the old settings, then apply the new settings.
    ApplyToExistingWindows(false);
    LoadSettings();
    ApplyToExistingWindows(true);
}
