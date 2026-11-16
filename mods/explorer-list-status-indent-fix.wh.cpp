// ==WindhawkMod==
// @id              explorer-list-status-indent-fix
// @name            File Explorer spacing fixes: List view indent, navigation pane, etc
// @description     Removes the empty space before every item in the List, Small icons and Content views of File Explorer, evens out the gaps around items, icons and names, and tidies up the navigation pane: even padding, thin divider, longer separators, no pin icons
// @version         1.0
// @author          NDruce
// @github          https://github.com/NDruce
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer spacing fixes: List view indent, navigation pane

Starting with Windows 11 version 22H2, File Explorer reserves a 28 pixels wide
area to the left of **every** item in the **List**, **Small icons** and
**Content** views.
The area is meant for sync status icons of cloud folders (OneDrive and other
sync providers), but it's reserved in every folder, including the folders
which have nothing to do with the cloud. The result is an odd indent between
the edge of an item and its icon.

The mod removes that area where there's nothing to show. It also fixes a
number of smaller spacing inconsistencies in the files area and in the
navigation pane. Every adjustment has a setting, and can be changed or turned
off.

## The files area

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

## The navigation pane

* **Thin divider.** The strip between the navigation pane and the files area
  is replaced with a line of 1 pixel. By default, the line has the color of
  the line below the command bar, for the light and for the dark theme. The
  color is configurable. The navigation pane can still be resized by dragging
  the divider.
* **Even padding.** The gap to the left of the items of the navigation pane is
  made the same as the gap to the right of them. The size is configurable. A
  space above and below the contents of the navigation pane can be added too.
* **Pin icons.** The pin icons to the right of the pinned folders can be
  hidden or made less visible.
* **Separators.** The lines which separate the groups of the navigation pane
  can be made longer or shorter. They can go from edge to edge.
* **Expand arrows.** The arrows can be moved vertically.

## Defaults

Most of the adjustments are turned on by default. To get the stock look of a
part back, use these values:

* Even item spacing: off. That also turns off the gaps and the offsets of the
  icons and the names.
* Horizontal and vertical margins of the files area: 0.
* Thin divider: off.
* Padding of the navigation pane: -1.
* Vertical padding of the navigation pane: 0.
* Vertical offset of the expand arrows: 0.
* Hide the pin icons: off.

The adjustment of the separators of the navigation pane is turned off by
default.

## Scaling and themes

All the sizes follow the display scaling. Items which are already displayed
when the display scaling changes keep the old spacing until the folder is
reopened or refreshed. With fractional scaling such as 125%, the gaps can
differ by a pixel because of rounding.

The automatic color of the thin divider is chosen when a folder window is
opened. Windows which are already open when the theme changes keep the old
color until they're reopened.

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
which usually happens when a file dialog is opened for the first time. From
then on, the vertical offset of the expand arrows applies to all tree views of
that program.

## How it works

The indent comes from `UIProperty::HasVisibleContent` in `explorerframe.dll`,
which reports the status icons area as having content in the List, Small icons
and Content views regardless of the actual content. The mod makes it report the
actual state.

Everything else is done by changing properties of DirectUI elements: of the
items, of the items area, and of the frame of a folder window which holds the
navigation pane and its divider. The elements are adjusted when they're
created, and the windows which are already open are adjusted when the mod is
loaded. The functions which are exported by `dui70.dll` are used for that.

No code or resources are patched. The adjustments are only made to elements
which have the expected stock values, so that the mod stays out of the way of
other customizations. When the mod is turned off, the stock values are
restored.

The pin icons of the navigation pane are the state images of its tree. The
mod gives the tree a copy of its state image list with faded images, and
leaves the list of File Explorer itself untouched. The separators of the
navigation pane are adjusted while File Explorer draws them, which relies on
the debug symbols of `explorerframe.dll`. If that part can't be set up on a
Windows build, the rest of the mod still works.

The code of the status area fix was verified to have the expected shape in
Windows 11 builds 22621.317 - 22621.6274 (22H2, 23H2), 26100.1 - 26100.9974
(24H2, 25H2), 28000.1761 - 28000.3086 (26H1) and 26300 (26H2 preview). The
adjustments of the Details view rows and of the navigation pane were tested on
build 26300 at 125% scaling.
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
    Used with the even item spacing. Set to 0 to use the same gap as on the
    left and on the right of an item.
- statusIconOffset: 1
  $name: Vertical offset of the status icons
  $description: >-
    In pixels, in the List and Small icons views, used with the even item
    spacing. A positive value moves the status icons down, a negative value
    moves them up.
- nameOffset: -1
  $name: Vertical offset of the item names
  $description: >-
    In pixels, in the List and Details views, used with the even item spacing.
    A positive value moves the names down, a negative value moves them up. In
    the Details view, a negative value is taken from the height of the rows,
    so that the gaps above and below a name stay equal.
- nameOffsetSmallIcons: 0
  $name: Vertical offset of the item names in the Small icons view
  $description: >-
    In pixels, used with the even item spacing. A positive value moves the
    names down, a negative value moves them up.
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
- navPaneDivider: true
  $name: Thin divider between the navigation pane and the files
  $description: >-
    Replaces the strip between the navigation pane and the files area with a
    line of 1 pixel, of the same color as the line below the command bar. The
    navigation pane can still be resized by dragging the divider.
- navPaneDividerColor: ""
  $name: Color of the thin divider
  $description: >-
    In the #RRGGBB format, for example #3A3A3A. Leave empty to use a color
    which matches the light or the dark theme. Used if the thin divider is
    turned on.
- navPanePadding: 1
  $name: Padding of the navigation pane
  $description: >-
    The gap to the left and to the right of the items of the navigation pane,
    the same on both sides. The stock gaps are 2 on the left and 1 on the
    right. Set to -1 to keep the stock gaps.
- navPanePaddingVertical: 6
  $name: Vertical padding of the navigation pane
  $description: >-
    The space above and below the contents of the navigation pane. Set to 0 to
    keep the stock look.
- navPinIconsHide: true
  $name: Hide the pin icons in the navigation pane
  $description: >-
    Hides the pin icons which are shown to the right of the pinned folders.
- navPinIconOpacity: 0
  $name: Opacity of the pin icons in the navigation pane
  $description: >-
    In percent, from 1 to 99. Used if the pin icons aren't hidden. Set to 0 to
    keep the stock look of the pin icons.
- navSeparatorInset: 0
  $name: Side gaps of the separators in the navigation pane
  $description: >-
    Added to the gaps on the left and on the right of the horizontal lines
    which separate the groups of the navigation pane. The stock gap is 8. A
    negative value makes the lines longer: with -8, they go from edge to edge
    of the navigation pane. A positive value makes them shorter. Set to 0 to
    keep the stock gaps.
- viewMarginSize: 8
  $name: Horizontal margins of the files area
  $description: >-
    The margin on the left and on the right of the files area, in all views.
    The stock margins are 14 on the left and none on the right. Set both the
    horizontal and the vertical margins to 0 to keep the stock margins.
- viewMarginVertical: 9
  $name: Vertical margins of the files area
  $description: >-
    The margin at the top and at the bottom of the files area, in all views.
    The stock margins are 6 at the top and none at the bottom. Set both the
    horizontal and the vertical margins to 0 to keep the stock margins.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <commctrl.h>

#include <atomic>
#include <map>
#include <vector>
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
    bool navPaneDivider;
    // Zero for the color of the theme.
    ULONG navPaneDividerColor;
    int navPanePaddingVertical;
    // Negative to keep the stock padding.
    int navPanePadding;
    bool navPinIconsHide;
    int navPinIconOpacity;
    // Added to the stock side gaps of the separators, zero to keep them.
    int navSeparatorInset;
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
    int navArrowOffset;
} g_settings;

// The element which the top level element is hidden for on the current
// thread, see UIProperty_HasVisibleContent_Hook.
static thread_local void* t_hideTopLevelFor;
static HMODULE g_dui70Module;
static HMODULE g_explorerFrameModule;
static HMODULE g_shell32Module;
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
static HRESULT(WINAPI* Element_SetBorderThickness)(void* pThis, int l, int t, int r, int b);
static HRESULT(WINAPI* Element_SetBorderColor)(void* pThis, ULONG color);
static HRESULT(WINAPI* Element_SetBackgroundColor)(void* pThis, ULONG color);
static PropertyProc_t Element_BorderThicknessProp;
static PropertyProc_t Element_BorderColorProp;
static PropertyProc_t Element_BackgroundProp;
static void*(WINAPI* Value_CreateSize)(int cx, int cy, int dynamicScale);
static const SIZE*(WINAPI* Value_GetSize)(void* pThis);
static void(WINAPI* Value_Release)(void* pThis);
static int(WINAPI* DUIXmlParser_ScaleRelativePixels)(void* pThis, int value);
static HRESULT(WINAPI* DUIXmlParser_CreateElement_Original)(void* pThis, PCWSTR resid, void* substitute, void* parent, DWORD* deferCookie, void** element);

// explorerframe.dll symbols.
static BOOL(WINAPI* UIProperty_HasVisibleContent_Original)(void* pThis);
static PropertyProc_t UIBase_VisibleMinSizeProp;

// Returns true if the compact view of File Explorer is turned on. It's off
// by default.
static bool IsCompactView() {
    static ULONGLONG lastTime;
    static bool lastResult;
    ULONGLONG time = GetTickCount64();
    if (!lastTime || time - lastTime > 500) {
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
    ApplyItemPadding(item, kind, key, dpi, apply, GetElementPadding(item));
    ApplyNamePadding(item, kind, key, dpi, apply, hosted);

    if (kind == ItemKind::Details) {
        // The status is a column.
        ApplyDetailsRow(item, dpi, apply);
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
// The colors of the line below the command bar, in the DirectUI format.
constexpr ULONG kDividerColorDark = 0xFF3A3A3A;
constexpr ULONG kDividerColorLight = 0xFFE5E5E5;

static bool IsDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

static bool IsHighContrast() {
    HIGHCONTRASTW highContrast{.cbSize = sizeof(highContrast)};
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, 0) &&
           (highContrast.dwFlags & HCF_HIGHCONTRASTON);
}

static ATOM GetNavPaneSizerAtom() {
    return FindAtomW(L"PageSpaceControlSizer");
}

// The strip between the navigation pane and the files area consists of the
// sizer of the navigation pane, which is drawn with a theme part, and of a
// pixel of the background of the navigation pane host next to it. Makes both
// transparent and draws a line of 1 pixel as the left border of the sizer, or
// restores the stock look. The size of the sizer isn't changed, so that it
// stays as easy to drag as before.
static void ApplyNavPaneDivider(void* sizer, bool apply) {
    ATOM hostAtom = FindAtomW(L"ProperTreeHost");
    void* parent = Element_GetParent(sizer);
    void* host = parent && hostAtom ? Element_FindDescendent(parent, hostAtom) : nullptr;
    if (apply) {
        if (IsHighContrast()) {
            return;
        }

        Element_SetBorderThickness(sizer, 1, 0, 0, 0);
        ULONG color = g_settings.navPaneDividerColor;
        if (!color) {
            color = IsDarkMode() ? kDividerColorDark : kDividerColorLight;
        }

        Element_SetBorderColor(sizer, color);
        Element_SetBackgroundColor(sizer, 0);
        if (host) {
            Element_SetBackgroundColor(host, 0);
        }
    } else {
        // The stock values come from the style sheet.
        Element_RemoveLocalValue(sizer, Element_BorderThicknessProp);
        Element_RemoveLocalValue(sizer, Element_BorderColorProp);
        Element_RemoveLocalValue(sizer, Element_BackgroundProp);
        if (host) {
            Element_RemoveLocalValue(host, Element_BackgroundProp);
        }
    }
}

// The stock padding of the navigation pane host and the stock width of its
// sizer, which covers a part of the right padding.
constexpr int kNavPanePaddingLeft = 2;
constexpr int kNavPanePaddingRight = 4;
constexpr int kNavPaneSizerWidth = 3;

// Sets the configured gap on both sides of the items of the navigation pane
// and the configured space above and below its contents, or restores the
// stock padding. On the right, the sizer covers a part of the padding, so the
// padding is larger by the width of the sizer. Only touches the padding if it
// has the stock values or the adjusted values. A new element which isn't in a
// window yet holds the values for 100% scaling.
static void ApplyNavPanePadding(void* sizer, int dpi, bool apply, bool refresh) {
    ATOM hostAtom = FindAtomW(L"ProperTreeHost");
    void* parent = Element_GetParent(sizer);
    void* host = parent && hostAtom ? Element_FindDescendent(parent, hostAtom) : nullptr;
    if (!host) {
        return;
    }

    RECT rawStock{kNavPanePaddingLeft, 0, kNavPanePaddingRight, 0};
    RECT stock{MulDiv(kNavPanePaddingLeft, dpi, 96), 0, MulDiv(kNavPanePaddingRight, dpi, 96), 0};
    RECT adjusted = stock;
    if (g_settings.navPanePadding >= 0) {
        int gap = MulDiv(g_settings.navPanePadding, dpi, 96);
        adjusted.left = gap;
        adjusted.right = gap + MulDiv(kNavPaneSizerWidth, dpi, 96);
    }

    adjusted.top = adjusted.bottom = MulDiv(g_settings.navPanePaddingVertical, dpi, 96);
    if (EqualRect(&adjusted, &stock)) {
        return;
    }

    RECT padding = GetElementPadding(host);
    if (apply && (EqualRect(&padding, &stock) || EqualRect(&padding, &rawStock))) {
        Element_SetPadding(host, adjusted.left, adjusted.top, adjusted.right, adjusted.bottom);
        if (adjusted.top > 0) {
            // The background of the host is darker than the navigation pane.
            Element_SetBackgroundColor(host, 0);
        }
    } else if (!apply && EqualRect(&padding, &adjusted)) {
        Element_SetPadding(host, stock.left, stock.top, stock.right, stock.bottom);
        if (adjusted.top > 0 && !g_settings.navPaneDivider) {
            Element_RemoveLocalValue(host, Element_BackgroundProp);
        }
    } else {
        return;
    }

    // The window of the navigation pane is only moved when the size of the
    // host changes. Change and restore the width of the pane.
    if (refresh) {
        int width = Element_GetWidth(parent);
        if (width > 0) {
            Element_SetWidth(parent, width + 1);
            Element_SetWidth(parent, width);
        }
    }
}

// The separators between the groups of the navigation pane.
//
// A separator isn't an item. The first item of a group is two rows tall, and
// CNscTree::DrawDivider draws a line in its upper row with Polyline. The ends
// of the line are moved on the way, without relying on the layout of
// CNscTree. The height of a separator can't be changed that way: the height
// of a tree item is a whole number of rows.
static void(WINAPI* CNscTree_DrawDivider_Original)(void* pThis, HDC hdc, HTREEITEM item);
static LRESULT(WINAPI* SendMessageW_Original)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL(WINAPI* Polyline_Original)(HDC hdc, const POINT* points, int count);

// Set while a separator is being drawn by the current thread.
static thread_local bool t_drawingNavSeparator;

// The pin icons of the navigation pane.
//
// A pin icon is the state image of a tree item, which the tree draws on the
// right. The state image list of the tree is replaced with a copy in which
// the images are faded or fully transparent. The list of File Explorer itself
// isn't touched: it stays the owner of its list, and gets its own list back
// when it asks for it.
struct NavStateImages {
    HIMAGELIST original;
    HIMAGELIST adjusted;
};

static SRWLOCK g_navStateImagesLock = SRWLOCK_INIT;
static std::map<HWND, NavStateImages> g_navStateImages;

// The opacity of the pin icons in percent, or a negative value to keep them
// as they are.
static int GetNavPinIconOpacity() {
    if (g_settings.navPinIconsHide) {
        return 0;
    }

    return g_settings.navPinIconOpacity > 0 ? g_settings.navPinIconOpacity : -1;
}

static bool IsNavPaneTree(HWND hWnd) {
    WCHAR className[32];
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) && wcscmp(className, L"SysTreeView32") == 0 &&
           GetClassNameW(GetParent(hWnd), className, ARRAYSIZE(className)) &&
           wcscmp(className, L"NamespaceTreeControl") == 0;
}

// Sends a message without the adjustments of the hook below.
static LRESULT SendMessageRaw(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return SendMessageW_Original ? SendMessageW_Original(hWnd, msg, wParam, lParam)
                                 : SendMessageW(hWnd, msg, wParam, lParam);
}

// Creates a copy of an icon with the given opacity.
static HICON CreateFadedIcon(HICON icon, int opacity) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) {
        return nullptr;
    }

    HICON result = nullptr;
    BITMAP bm{};
    if (info.hbmColor && GetObjectW(info.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 && bm.bmHeight > 0 &&
        bm.bmWidth <= 256 && bm.bmHeight <= 256) {
        int width = bm.bmWidth;
        int height = bm.bmHeight;
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        std::vector<DWORD> color(width * height);
        std::vector<DWORD> mask(width * height);
        HDC dc = GetDC(nullptr);
        BITMAPINFO biColor = bi;
        BITMAPINFO biMask = bi;
        bool hasColor = GetDIBits(dc, info.hbmColor, 0, height, color.data(), &biColor, DIB_RGB_COLORS) == height;
        bool hasMask = info.hbmMask &&
                       GetDIBits(dc, info.hbmMask, 0, height, mask.data(), &biMask, DIB_RGB_COLORS) == height;

        void* bits = nullptr;
        HBITMAP newColor = hasColor ? CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
        ReleaseDC(nullptr, dc);

        if (newColor && bits) {
            bool hasAlpha = false;
            for (DWORD pixel : color) {
                if (pixel >> 24) {
                    hasAlpha = true;
                    break;
                }
            }

            // The pixels which end up fully transparent are marked in the
            // mask, as an icon without any alpha is drawn by its mask.
            int maskStride = ((width + 15) / 16) * 2;
            std::vector<BYTE> newMaskBits(maskStride * height, 0);
            DWORD* out = (DWORD*)bits;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    DWORD pixel = color[y * width + x];
                    int alpha = hasAlpha ? (int)(pixel >> 24) : (hasMask && (mask[y * width + x] & 0xFFFFFF) ? 0 : 255);
                    alpha = alpha * opacity / 100;
                    if (alpha <= 0) {
                        out[y * width + x] = 0;
                        newMaskBits[y * maskStride + x / 8] |= (BYTE)(0x80 >> (x % 8));
                    } else {
                        out[y * width + x] = ((DWORD)alpha << 24) | (pixel & 0xFFFFFF);
                    }
                }
            }

            HBITMAP newMask = CreateBitmap(width, height, 1, 1, newMaskBits.data());
            if (newMask) {
                ICONINFO newInfo{};
                newInfo.fIcon = TRUE;
                newInfo.hbmMask = newMask;
                newInfo.hbmColor = newColor;
                result = CreateIconIndirect(&newInfo);
                DeleteObject(newMask);
            }
        }

        if (newColor) {
            DeleteObject(newColor);
        }
    }

    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }

    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }

    return result;
}

// Creates a copy of an image list in which all the images have the given
// opacity.
static HIMAGELIST CreateFadedImageList(HIMAGELIST source, int opacity) {
    HIMAGELIST copy = ImageList_Duplicate(source);
    if (!copy) {
        return nullptr;
    }

    int count = ImageList_GetImageCount(copy);
    for (int i = 0; i < count; i++) {
        HICON icon = ImageList_GetIcon(source, i, ILD_TRANSPARENT);
        if (!icon) {
            continue;
        }

        if (HICON faded = CreateFadedIcon(icon, opacity)) {
            ImageList_ReplaceIcon(copy, i, faded);
            DestroyIcon(faded);
        }

        DestroyIcon(icon);
    }

    return copy;
}

// Replaces the state image list of a navigation pane tree with an adjusted
// copy, or restores the list of File Explorer. Called from the thread of the
// tree.
static void ApplyNavPinIcons(HWND tree, bool apply) {
    NavStateImages previous{};
    AcquireSRWLockExclusive(&g_navStateImagesLock);
    if (auto it = g_navStateImages.find(tree); it != g_navStateImages.end()) {
        previous = it->second;
        g_navStateImages.erase(it);
    }
    ReleaseSRWLockExclusive(&g_navStateImagesLock);

    HIMAGELIST current = (HIMAGELIST)SendMessageRaw(tree, TVM_GETIMAGELIST, TVSIL_STATE, 0);
    bool changed = false;
    if (previous.adjusted) {
        if (current == previous.adjusted) {
            SendMessageRaw(tree, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)previous.original);
            current = previous.original;
            changed = true;
        }

        ImageList_Destroy(previous.adjusted);
    }

    int opacity = apply ? GetNavPinIconOpacity() : -1;
    if (opacity >= 0 && current) {
        if (HIMAGELIST adjusted = CreateFadedImageList(current, opacity)) {
            SendMessageRaw(tree, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)adjusted);
            AcquireSRWLockExclusive(&g_navStateImagesLock);
            g_navStateImages[tree] = NavStateImages{current, adjusted};
            ReleaseSRWLockExclusive(&g_navStateImagesLock);
            changed = true;
        }
    }

    if (changed) {
        InvalidateRect(tree, nullptr, TRUE);
    }
}

// File Explorer sets or retrieves the state image list of a navigation pane
// tree.
static LRESULT NavStateImageListMessage(HWND hWnd, UINT msg, LPARAM lParam) {
    if (msg == TVM_GETIMAGELIST) {
        LRESULT result = SendMessageW_Original(hWnd, msg, TVSIL_STATE, lParam);
        AcquireSRWLockExclusive(&g_navStateImagesLock);
        if (auto it = g_navStateImages.find(hWnd); it != g_navStateImages.end() && (HIMAGELIST)result == it->second.adjusted) {
            result = (LRESULT)it->second.original;
        }
        ReleaseSRWLockExclusive(&g_navStateImagesLock);
        return result;
    }

    HIMAGELIST newList = (HIMAGELIST)lParam;
    int opacity = g_unloading ? -1 : GetNavPinIconOpacity();
    HIMAGELIST adjusted = newList && opacity >= 0 ? CreateFadedImageList(newList, opacity) : nullptr;
    LRESULT result = SendMessageW_Original(hWnd, msg, TVSIL_STATE, (LPARAM)(adjusted ? adjusted : newList));

    NavStateImages previous{};
    AcquireSRWLockExclusive(&g_navStateImagesLock);
    if (auto it = g_navStateImages.find(hWnd); it != g_navStateImages.end()) {
        previous = it->second;
        g_navStateImages.erase(it);
    }

    if (adjusted) {
        g_navStateImages[hWnd] = NavStateImages{newList, adjusted};
    }
    ReleaseSRWLockExclusive(&g_navStateImagesLock);

    if (previous.adjusted) {
        // The previous list is returned to the caller, which might destroy it.
        if ((HIMAGELIST)result == previous.adjusted) {
            result = (LRESULT)previous.original;
        }

        ImageList_Destroy(previous.adjusted);
    }

    return result;
}

// Destroys the copies which are left, for the trees which are gone.
static void FreeNavStateImages() {
    AcquireSRWLockExclusive(&g_navStateImagesLock);
    for (const auto& [tree, images] : g_navStateImages) {
        if (!IsWindow(tree) ||
            (HIMAGELIST)SendMessageRaw(tree, TVM_GETIMAGELIST, TVSIL_STATE, 0) != images.adjusted) {
            ImageList_Destroy(images.adjusted);
        }
    }

    g_navStateImages.clear();
    ReleaseSRWLockExclusive(&g_navStateImagesLock);
}

static void WINAPI CNscTree_DrawDivider_Hook(void* pThis, HDC hdc, HTREEITEM item) {
    t_drawingNavSeparator = true;
    CNscTree_DrawDivider_Original(pThis, hdc, item);
    t_drawingNavSeparator = false;
}

static LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if ((msg == TVM_SETIMAGELIST || msg == TVM_GETIMAGELIST) && wParam == TVSIL_STATE && IsNavPaneTree(hWnd)) {
        return NavStateImageListMessage(hWnd, msg, lParam);
    }

    return SendMessageW_Original(hWnd, msg, wParam, lParam);
}

static BOOL WINAPI Polyline_Hook(HDC hdc, const POINT* points, int count) {
    if (!t_drawingNavSeparator || !g_settings.navSeparatorInset || count != 2 || !points ||
        points[0].y != points[1].y) {
        return Polyline_Original(hdc, points, count);
    }

    // The line is drawn in the pixels of the device.
    int inset = MulDiv(g_settings.navSeparatorInset, GetDeviceCaps(hdc, LOGPIXELSX), 96);
    POINT adjusted[2] = {points[0], points[1]};
    adjusted[0].x += inset;
    adjusted[1].x -= inset;
    if (adjusted[0].x >= adjusted[1].x) {
        return TRUE;
    }

    return Polyline_Original(hdc, adjusted, 2);
}

// uxtheme.dll
constexpr int kTVP_TREEITEM = 1;
constexpr int kTVP_GLYPH = 2;
constexpr int kTVP_HOTGLYPH = 4;
static BOOL(WINAPI* Uxtheme_IsThemePartDefined)(HANDLE hTheme, int partId, int stateId);
// Exported by ordinal only.
using GetThemeClass_t = HRESULT(WINAPI*)(HANDLE hTheme, LPWSTR buffer, int size);
static GetThemeClass_t Uxtheme_GetThemeClass;
static HRESULT(WINAPI* DrawThemeBackground_Original)(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const RECT* clipRect);
static HRESULT(WINAPI* DrawThemeBackgroundEx_Original)(HANDLE hTheme, HDC hdc, int partId, int stateId, const RECT* rect, const void* options);

// Returns true for the expand arrow of a tree view, such as the navigation
// pane.
static bool IsTreeViewGlyph(HANDLE hTheme, int partId, const RECT* rect) {
    if (!g_settings.navArrowOffset || (partId != kTVP_GLYPH && partId != kTVP_HOTGLYPH) || !rect) {
        return false;
    }

    // The theme class is identified by its name, such as "Explorer::TreeView"
    // or "DarkMode_Explorer::TreeView". Other classes use the same part
    // numbers for other things, for example for the buttons of up-down and
    // pager controls.
    if (Uxtheme_GetThemeClass) {
        WCHAR themeClass[64] = {};
        if (SUCCEEDED(Uxtheme_GetThemeClass(hTheme, themeClass, ARRAYSIZE(themeClass)))) {
            themeClass[ARRAYSIZE(themeClass) - 1] = L'\0';
            constexpr size_t kSuffixLength = 8;
            size_t length = wcslen(themeClass);
            if (length > 0 &&
                (length < kSuffixLength || _wcsicmp(themeClass + length - kSuffixLength, L"TreeView") != 0)) {
                return false;
            }
        }
    }

    // Also check the parts: a tree view theme has parts up to 4. If the name
    // of the class can't be retrieved, that's the only check.
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

    // The frame of a folder window.
    if (g_settings.navPaneDivider && kind == ItemKind::None && !IsTargetCollectionClass(resid) &&
        !StartsWith(resid, L"property.")) {
        if (ATOM atom = GetNavPaneSizerAtom()) {
            void* sizer = Element_GetID(*element) == atom ? *element : Element_FindDescendent(*element, atom);
            if (sizer) {
                ApplyNavPaneDivider(sizer, true);
            }
        }
    }

    if ((g_settings.navPanePadding >= 0 || g_settings.navPanePaddingVertical > 0) && kind == ItemKind::None && !IsTargetCollectionClass(resid) &&
        !StartsWith(resid, L"property.")) {
        if (ATOM atom = GetNavPaneSizerAtom()) {
            void* sizer = Element_GetID(*element) == atom ? *element : Element_FindDescendent(*element, atom);
            int dpi = sizer ? DUIXmlParser_ScaleRelativePixels(pThis, 96) : 0;
            if (dpi > 0) {
                ApplyNavPanePadding(sizer, dpi, true, false);
            }
        }
    }
    return hr;
}

// Applies or restores the spacing of the existing items.
struct ApplyParam {
    bool active;
    bool spacing;
    bool viewMargin;
    bool divider;
    bool navPadding;
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
        (module != g_explorerFrameModule && module != g_dui70Module && module != g_shell32Module)) {
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

static void WINAPI ApplyToNavTreeWindow(HWND hWnd, void* param) {
    ApplyNavPinIcons(hWnd, ((const ApplyParam*)param)->active);
}

// The frame of a folder window: the navigation pane, the files area and the
// divider between them.
static void WINAPI ApplyToFrameWindow(HWND hWnd, void* param) {
    const ApplyParam& applyParam = *(const ApplyParam*)param;
    ATOM atom = GetNavPaneSizerAtom();
    void* element = (applyParam.divider || applyParam.navPadding) && atom ? GetItemsViewElement(hWnd) : nullptr;
    void* sizer = element ? Element_FindDescendent(element, atom) : nullptr;
    if (!sizer) {
        return;
    }

    if (applyParam.divider) {
        ApplyNavPaneDivider(sizer, applyParam.active);
    }

    if (applyParam.navPadding) {
        ApplyNavPanePadding(sizer, GetDpiForWindow(hWnd), applyParam.active, true);
    }
}

// Applies the adjustments to the existing items, or restores the stock values
// if not active.
static void ApplyToExistingWindows(bool active) {
    if (!g_initialized) {
        return;
    }

    ApplyParam enumParam{
        .active = active,
        .spacing = active && g_settings.equalSpacing,
        .viewMargin = active && (g_settings.viewMarginSize > 0 || g_settings.viewMarginVertical > 0),
        .divider = g_settings.navPaneDivider,
        .navPadding = g_settings.navPanePadding >= 0 || g_settings.navPanePaddingVertical > 0,
    };

    auto enumChildProc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        if (IsNavPaneTree(hWnd)) {
            RunFromWindowThread(hWnd, ApplyToNavTreeWindow, (void*)lParam);
            return TRUE;
        }

        WCHAR className[32];
        if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) && wcscmp(className, L"DirectUIHWND") == 0 &&
            GetClassNameW(GetParent(hWnd), className, ARRAYSIZE(className))) {
            if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
                RunFromWindowThread(hWnd, ApplyToItemsViewWindow, (void*)lParam);
            } else if (wcscmp(className, L"DUIViewWndClassName") == 0) {
                RunFromWindowThread(hWnd, ApplyToFrameWindow, (void*)lParam);
            }
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
    g_settings.navPaneDivider = Wh_GetIntSetting(L"navPaneDivider");
    g_settings.navPanePaddingVertical = Wh_GetIntSetting(L"navPanePaddingVertical");
    if (g_settings.navPanePaddingVertical < 0 || g_settings.navPanePaddingVertical > 64) {
        g_settings.navPanePaddingVertical = 6;
    }

    g_settings.navPanePadding = Wh_GetIntSetting(L"navPanePadding");
    if (g_settings.navPanePadding < -1 || g_settings.navPanePadding > 32) {
        g_settings.navPanePadding = 1;
    }

    g_settings.navPinIconsHide = Wh_GetIntSetting(L"navPinIconsHide");
    g_settings.navPinIconOpacity = Wh_GetIntSetting(L"navPinIconOpacity");
    if (g_settings.navPinIconOpacity < 0 || g_settings.navPinIconOpacity > 99) {
        g_settings.navPinIconOpacity = 0;
    }

    g_settings.navSeparatorInset = Wh_GetIntSetting(L"navSeparatorInset");
    if (g_settings.navSeparatorInset < -64 || g_settings.navSeparatorInset > 64) {
        g_settings.navSeparatorInset = 0;
    }

    // #RRGGBB, converted to the DirectUI format: opaque, red in the low byte.
    g_settings.navPaneDividerColor = 0;
    auto dividerColor = WindhawkUtils::StringSetting::make(L"navPaneDividerColor");
    PCWSTR hex = dividerColor;
    while (*hex == L' ' || *hex == L'#') {
        hex++;
    }

    PWSTR hexEnd = nullptr;
    ULONG rgb = wcstoul(hex, &hexEnd, 16);
    while (*hexEnd == L' ') {
        hexEnd++;
    }

    if (hexEnd - hex >= 6 && wcsspn(hex, L"0123456789abcdefABCDEF") == 6 && !*hexEnd) {
        g_settings.navPaneDividerColor = 0xFF000000 | ((rgb & 0xFF) << 16) | (rgb & 0xFF00) | ((rgb >> 16) & 0xFF);
    }
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

    g_settings.navArrowOffset = Wh_GetIntSetting(L"navArrowOffset");
    if (g_settings.navArrowOffset < -8 || g_settings.navArrowOffset > 8) {
        g_settings.navArrowOffset = -1;
    }

    g_settings.statusIconGap = Wh_GetIntSetting(L"statusIconGap");
    if (g_settings.statusIconGap < 0 || g_settings.statusIconGap > 32) {
        g_settings.statusIconGap = 0;
    }

    g_settings.contentBottomPadding = Wh_GetIntSetting(L"contentBottomPadding");
    if (g_settings.contentBottomPadding < 0 || g_settings.contentBottomPadding > 64) {
        g_settings.contentBottomPadding = 2;
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
static HMODULE g_loadedModules[4];

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

    // The frame of a folder window belongs to shell32.dll.
    g_shell32Module = GetModuleHandleW(L"shell32.dll");

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
        !GetProc(dui70, "?SetBorderThickness@Element@DirectUI@@QEAAJHHHH@Z", &Element_SetBorderThickness) ||
        !GetProc(dui70, "?SetBorderColor@Element@DirectUI@@QEAAJK@Z", &Element_SetBorderColor) ||
        !GetProc(dui70, "?SetBackgroundColor@Element@DirectUI@@QEAAJK@Z", &Element_SetBackgroundColor) ||
        !GetProc(dui70, "?BorderThicknessProp@Element@DirectUI@@SAPEBUPropertyInfo@2@XZ", &Element_BorderThicknessProp) ||
        !GetProc(dui70, "?BorderColorProp@Element@DirectUI@@SAPEBUPropertyInfo@2@XZ", &Element_BorderColorProp) ||
        !GetProc(dui70, "?BackgroundProp@Element@DirectUI@@SAPEBUPropertyInfo@2@XZ", &Element_BackgroundProp) ||
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
        // The separators of the navigation pane, optional.
        {
            {LR"(private: void __cdecl CNscTree::DrawDivider(struct HDC__ *,struct _TREEITEM *))"},
            &CNscTree_DrawDivider_Original,
            CNscTree_DrawDivider_Hook,
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

    // The expand arrows of tree views.
    if (HMODULE uxtheme = LoadSystemModule(L"uxtheme.dll")) {
        Uxtheme_IsThemePartDefined = (decltype(Uxtheme_IsThemePartDefined))GetProcAddress(uxtheme, "IsThemePartDefined");
        // Optional, GetThemeClass is exported by ordinal 74.
        Uxtheme_GetThemeClass = (GetThemeClass_t)GetProcAddress(uxtheme, MAKEINTRESOURCEA(74));
        auto drawThemeBackground = (decltype(DrawThemeBackground_Original))GetProcAddress(uxtheme, "DrawThemeBackground");
        auto drawThemeBackgroundEx = (decltype(DrawThemeBackgroundEx_Original))GetProcAddress(uxtheme, "DrawThemeBackgroundEx");
        if (Uxtheme_IsThemePartDefined && drawThemeBackground && drawThemeBackgroundEx) {
            WindhawkUtils::SetFunctionHook(drawThemeBackground, DrawThemeBackground_Hook, &DrawThemeBackground_Original);
            WindhawkUtils::SetFunctionHook(drawThemeBackgroundEx, DrawThemeBackgroundEx_Hook, &DrawThemeBackgroundEx_Original);
        }
    }

    WindhawkUtils::SetFunctionHook(getTopLevel, Element_GetTopLevel_Hook, &Element_GetTopLevel_Original);
    WindhawkUtils::SetFunctionHook(createElement, DUIXmlParser_CreateElement_Hook, &DUIXmlParser_CreateElement_Original);

    // The separators and the pin icons of the navigation pane.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    auto sendMessageW = user32 ? (decltype(SendMessageW_Original))GetProcAddress(user32, "SendMessageW") : nullptr;
    auto polyline = gdi32 ? (decltype(Polyline_Original))GetProcAddress(gdi32, "Polyline") : nullptr;
    if (sendMessageW && polyline) {
        WindhawkUtils::SetFunctionHook(sendMessageW, SendMessageW_Hook, &SendMessageW_Original);
        WindhawkUtils::SetFunctionHook(polyline, Polyline_Hook, &Polyline_Original);
    }

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
    FreeNavStateImages();
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
