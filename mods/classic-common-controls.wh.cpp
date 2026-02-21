// ==WindhawkMod==
// @id              classic-common-controls
// @name            Classic Common Controls
// @description     Replaces various controls with versions from Windows XP's Common Controls library
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @architecture    amd64
// @architecture    x86
// @compilerOptions -lversion -lole32 -lshlwapi -lpsapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic List/Tree Views
This mod replaces the tree view and tooltip controls with the versions from
Windows XP.

## Compatibility
Due to relying on comctl32.dll from Windows XP, this mod will *not* work on applications
compiled for ARM64.

## Configuration
You will need both and x64 and x86 (WOW64) copy of comctl32.dll, version 6.0 from Windows
Server 2003 x64 Edition or Windows XP Professional x64 Edition. Here is an example of the
paths to these files:

| Machine type | Path |
|-|-|
| x64 | `C:\WINDOWS\WinSxS\amd64_Microsoft.Windows.Common-Controls_6595b64144ccf1df_6.0.3790.3959_x-ww_0A7B2435\comctl32.dll` |
| x86 | `C:\WINDOWS\WinSxS\wow64_Microsoft.Windows.Common-Controls_6595b64144ccf1df_6.0.3790.3959_x-ww_5FA17F4E\comctl32.dll` |

Note that these paths may not be the same on every install of Windows Server 2003/XP x64,
but it should give you a general idea of where to find the necessary files.

Once you have these files, simply copy them somewhere they will be safe, and specify the
paths to them in this mod's options.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- comctl32_64_path: C:\path\to\64\comctl32.dll
  $name: comctl32.dll path (x64)
  $description: Path to comctl32.dll 6.0 from Windows Server 2003/XP x64
- comctl32_32_path: C:\path\to\32\comctl32.dll
  $name: comctl32.dll path (x86)
  $description: Path to WOW64 comctl32.dll 6.0 from Windows Server 2003/XP x64

#- list_views: true
#  $name: Replace list view control
- tree_views: true
  $name: Replace tree view control
- tooltips: true
  $name: Replace tooltip control
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <psapi.h>
#include <windowsx.h>
#include <windhawk_utils.h>

/**
  * Uncomment to enable list views which are extremely broken with shell32
  * CDefView (Explorer browser) as is.
  * 
  * You will also have to uncomment the definition for the `list_views`
  * setting in this mod's setting block.
  */
//#define FEATURE_LIST_VIEWS

HMODULE g_hinstComCtlXP = NULL;

#pragma region "IListView implementation"

#ifdef FEATURE_LIST_VIEWS

enum SELECTION_FLAGS
{
    LVSF_DEFAULT = 0x0,
    LVSF_RESTRICTSELECTTOCONTENT = 0x1,
};

enum TYPEAHEAD_FLAGS
{
    TAHF_DEFAULT = 0x0,
    TAHF_FOCUSONLY = 0x1,
};

MIDL_INTERFACE("96A23E16-A1BC-11D1-B084-00C04FC33AA5")
ILVRange : IUnknown
{
    // *** ISelRange methods ***
    STDMETHOD(IncludeRange)(THIS_ LONG iBegin, LONG iEnd) PURE;
    STDMETHOD(ExcludeRange)(THIS_ LONG iBegin, LONG iEnd) PURE;
    STDMETHOD(InvertRange)(THIS_ LONG iBegin, LONG iEnd) PURE;
    STDMETHOD(InsertItem)(THIS_ LONG iItem) PURE;
    STDMETHOD(RemoveItem)(THIS_ LONG iItem) PURE;

    STDMETHOD(Clear)(THIS) PURE;
    STDMETHOD(IsSelected)(THIS_ LONG iItem) PURE;
    STDMETHOD(IsEmpty)(THIS) PURE;
    STDMETHOD(NextSelected)(THIS_ LONG iItem, LONG *piItem) PURE;
    STDMETHOD(NextUnSelected)(THIS_ LONG iItem, LONG *piItem) PURE;
    STDMETHOD(CountIncluded)(THIS_ LONG *pcIncluded) PURE;
};

typedef DWORD BITBOOL;

typedef struct tagControlInfo 
{
    HWND        hwnd;
    HWND        hwndParent;
    DWORD       style;
    DWORD       dwCustom;
    BITBOOL     bUnicode : 1;
    BITBOOL     bInFakeCustomDraw:1;
    BITBOOL     fDPIAware:1;
    UINT        uiCodePage;
    DWORD       dwExStyle;
    LRESULT     iVersion;
    WORD        wUIState;
} CCONTROLINFO, *LPCCONTROLINFO;

typedef struct ISEARCHINFO 
{
    int iIncrSearchFailed;
    LPTSTR pszCharBuf;                  // isearch string lives here
    int cbCharBuf;                      // allocated size of pszCharBuf
    int ichCharBuf;                     // number of live chars in pszCharBuf
    DWORD timeLast;                     // time of last input event
    BOOL fReplaceCompChar;

} ISEARCHINFO, *PISEARCHINFO;

typedef struct _LV
{
    CCONTROLINFO ci;     // common control header info

    BITBOOL fNoDismissEdit:1;  // don't dismiss in-place edit control
    BITBOOL fButtonDown:1;     // we're tracking the mouse with a button down
    BITBOOL fOneClickOK:1;     // true from creation to double-click-timeout
    BITBOOL fOneClickHappened:1; // true from item-activate to double-click-timeout
    BITBOOL fPlaceTooltip:1;   // should we do the placement of tooltip over the text?
    BITBOOL fImgCtxComplete:1; // TRUE if we have complete bk image
    BITBOOL fNoEmptyText:1;    // we don't have text for an empty view.
    BITBOOL fGroupView:1;
    BITBOOL fIconsPositioned:1;
    BITBOOL fInsertAfter:1;    // insert after (or before) iInsertSlot slot.
    BITBOOL fListviewAlphaSelect:1;
    BITBOOL fListviewShadowText:1;
    BITBOOL fListviewWatermarkBackgroundImages:1;
    BITBOOL fListviewEnableWatermark:1;
    BITBOOL fInFixIScrollPositions:1;

    WORD wView;           // Which view are we in?

    HDPA hdpa;          // item array structure
    DWORD flags;        // LVF_ state bits
    DWORD exStyle;      // the listview LVM_SETEXTENDEDSTYLE
    DWORD dwExStyle;    // the windows ex style
    HFONT hfontLabel;   // font to use for labels
    COLORREF clrBk;     // Background color
    COLORREF clrBkSave; // Background color saved during disable
    COLORREF clrText;   // text color
    COLORREF clrTextBk; // text background color
    COLORREF clrOutline; // focus rect outline color
    HBRUSH hbrBk;
    HANDLE hheap;        // The heap to use to allocate memory from.
    int cyLabelChar;    // height of '0' in hfont
    int cxLabelChar;    // width of '0'
    int cxEllipses;     // width of "..."
    int iDrag;          // index of item being dragged
    int iFocus;         // index of currently-focused item
    int iMark;          // index of "mark" for range selection
    int iItemDrawing;   // item currently being drawn
    int iFirstChangedNoRedraw;  // Index of first item added during no redraw.
    UINT stateCallbackMask; // item state callback mask
    SIZE sizeClient;      // current client rectangle
    int nWorkAreas;                            // Number of workareas
    LPRECT prcWorkAreas;      // The workarea rectangles -- nWorkAreas of them.
    UINT nSelected;
    int iPuntChar;
    HRGN hrgnInval;
    HWND hwndToolTips;      // handle of the tooltip window for this view
    int iTTLastHit;         // last item hit for text
    int iTTLastSubHit;      // last subitem hit for text
    LPTSTR pszTip;          // buffer for tip

#ifdef USE_SORT_FLARE
    int iSortFlare;
#endif

    // Small icon view fields

    HIMAGELIST himlSmall;   // small icons
    int cxSmIcon;          // image list x-icon size
    int cySmIcon;          // image list y-icon size
    int xOrigin;        // Horizontal scroll posiiton
    int cxItem;         // Width of small icon items
    int cyItem;         // item height
    int cItemCol;       // Number of items per column

    int cxIconSpacing;
    int cyIconSpacing;

    // Icon view fields

    HIMAGELIST himl;
    int cxIcon;             // image list x-icon size
    int cyIcon;             // image list y-icon size
    HDPA hdpaZOrder;        // Large icon Z-order array

    // Some definitions, to help make sense of the next two variables:
    //
    // Lets call the pitem->pt coordinate values "listview coordinates".
    //
    // Lets use rcClient as short-hand for the client area of the listview window.
    //
    // (1) ptOrigin is defined as the listview coordinate that falls on rcClient's 0,0 position.
    //
    // i.e., here's how to calculate the x,y location on rcClient for some item:
    //   * pitem->pt.x - ptOrigin.x , pitem->pt.y - ptOrigin.y
    // Let's call that these values "window coordinates".
    //
    // (2) rcView is defined as the bounding rect of: each item's unfolded rcview bounding rect and a bit of buffer
    // note: ListView_ValidatercView() checks this
    //
    // (3) For scrolling listviews (!LVS_NOSCROLL), there are two scrolling cases to consider:
    //   First, where rcClient is smaller than rcView:
    //      * rcView.left <= ptOrigin.x <= ptOrigin.x+RECTWIDTH(rcClient) <= rcView.right
    //   Second, where rcClient is larger than rcView (no scrollbars visible):
    //      * ptOrigin.x <= rcView.left <= rcView.right <= ptOrigin.x+RECTWIDTH(rcClient)
    // note: ListView_ValidateScrollPositions() checks this
    //
    // (4) For non scrolling listviews (LVS_NOSCROLL), we have some legacy behavior to consider:
    //   For clients that persist icon positions but not the ptOrigin value, we must ensure:
    //      * 0 == ptOrigin.x
    // note: ListView_ValidateScrollPositions() checks this
    //
    POINT ptOrigin;         // Scroll position
    RECT rcView;            // Bounds of all icons (ptOrigin relative)
    int iFreeSlot;          // Most-recently found free icon slot since last reposition (-1 if none)
    int cSlots;

    HWND hwndEdit;          // edit field for edit-label-in-place
    int iEdit;              // item being edited
    WNDPROC pfnEditWndProc; // edit field subclass proc

    NMITEMACTIVATE nmOneClickHappened;

#define SMOOTHSCROLLLIMIT 10

    int iScrollCount; // how many times have we gotten scroll messages before an endscroll?

    // Report view fields

    int iLastColSort;
    int cCol;
    HDPA hdpaSubItems;
    HWND hwndHdr;           // Header control
    int yTop;               // First usable pixel (below header)
    int xTotalColumnWidth;  // Total width of all columns
    POINTL ptlRptOrigin;    // Origin of Report.
    int iSelCol;            // to handle column width changing. changing col
    int iSelOldWidth;       // to handle column width changing. changing col width
    int cyItemSave;        // in ownerdrawfixed mode, we put the height into cyItem.  use this to save the old value

    // Tile View fields
    SIZE sizeTile;          // the size of a tile
    int  cSubItems;         // Count of the number of sub items to display in a tile
    DWORD dwTileFlags;      // LVTVIF_FIXEDHEIGHT | LVTVIF_FIXEDWIDTH
    RECT rcTileLabelMargin; // addition space to reserve around label

    // Group View fields
    HDPA hdpaGroups;        // Groups
    RECT rcBorder;          // Border thickness
    COLORREF crHeader;
    COLORREF crFooter;
    COLORREF crTop;
    COLORREF crBottom;
    COLORREF crLeft;
    COLORREF crRight;
    HFONT hfontGroup;
    UINT paddingLeft;
    UINT paddingTop;
    UINT paddingRight;
    UINT paddingBottom;
    TCHAR szItems[50];

    // state image stuff
    HIMAGELIST himlState;
    int cxState;
    int cyState;

    // OWNERDATA stuff
    ILVRange *plvrangeSel;  // selection ranges
    ILVRange *plvrangeCut;  // Cut Range    
    int cTotalItems;        // number of items in the ownerdata lists
    int iDropHilite;        // which item is drop hilited, assume only 1
    int iMSAAMin, iMSAAMax; // keep track of what we told accessibility

    UINT uUnplaced;     // items that have been added but not placed (pt.x == RECOMPUTE)

    int iHot;  // which item is hot
    HFONT hFontHot; // the underlined font .. assume this has the same size metrics as hFont
    int iNoHover; // don't allow hover select on this guy because it's the one we just hover selected (avoids toggling)
    DWORD dwHoverTime;      // Defaults to HOVER_DEFAULT
    HCURSOR hCurHot; // the cursor when we're over a hot item

    // BkImage stuff
    struct IImgCtx *pImgCtx;// Background image interface
    ULONG ulBkImageFlags;   // LVBKIF_*
    HBITMAP hbmBkImage;     // Background bitmap (LVBKIF_SOURCE_HBITMAP)
    LPTSTR pszBkImage;      // Background URL (LVBKIF_SOURCE_URL)
    int xOffsetPercent;     // X offset for LVBKIF_STYLE_NORMAL images
    int yOffsetPercent;     // Y offset for LVBKIF_STYLE_NORMAL images
    HPALETTE hpalHalftone;  // Palette for drawing bk images 

    LPTSTR pszEmptyText;    // buffer for empty view text.

    COLORREF clrHotlight;     // Hot light color set explicitly for this listview.
    POINT ptCapture;

    //incremental search stuff
    ISEARCHINFO is;

    // Themes
    HTHEME hTheme;

    // Insertmark
    int iInsertItem;        // The item to insert next to
    int clrim;              // The color of the insert mark.

    int iTracking;          // Used for tooltips via keyboard (current item in focus for info display, >= 0 is tracking active)
    LPARAM lLastMMove;      // Filter out mouse move messages that didn't result in an actual move (for track tooltip canceling)

    // Frozen Slot
    int iFrozenSlot;        // The slot that should not be used by anyone other than the frozen item
    struct LISTITEM *pFrozenItem;  // Pointer to the frozen item.

    RECT rcViewMargin; // the EnsureVisible margine around an item -- the rcView margin

    RECT rcMarquee;

    // Watermarks
    HBITMAP hbmpWatermark;
    SIZE    szWatermark;

    // Id Tracking
    DWORD   idNext;         // Stores the next ID.
    DWORD   iLastId;         // Stores the index to the previous item for searches
    DWORD   iIncrement;

} LV;

MIDL_INTERFACE("44C09D56-8D3B-419D-A462-7B956B105B47")
IOwnerDataCallback : IUnknown
{
    STDMETHOD(GetItemPosition)(int, POINT *) PURE;
    STDMETHOD(SetItemPosition)(int, POINT) PURE;
    STDMETHOD(GetItemInGroup)(int, int, int *) PURE;
    STDMETHOD(GetItemGroup)(int, int, int *) PURE;
    STDMETHOD(GetItemGroupCount)(int, int *) PURE;
    STDMETHOD(OnCacheHint)(LVITEMINDEX, tagLVITEMINDEX) PURE;
};

enum LV_EDIT_FLAGS
{
    LVEF_DEFAULT = 0x0,
    LVEF_HOVER = 0x1,
};

MIDL_INTERFACE("11A66240-5489-42C2-AEBF-286FC831524C")
ISubItemCallback : IUnknown
{
    STDMETHOD(GetSubItemTitle)(int, LPWSTR, int);
    STDMETHOD(GetSubItemControl)(int, int, LPCGUID, void **);
    STDMETHOD(BeginSubItemEdit)(int, int, LV_EDIT_FLAGS, LPCGUID, void **);
    STDMETHOD(EndSubItemEdit)(int, int, int, struct IPropertyControl *);
    STDMETHOD(BeginGroupEdit)(int, LPCGUID, void **);
    STDMETHOD(EndGroupEdit)(int, int, struct IPropertyControl *);
    STDMETHOD(OnInvokeVerb)(int, LPCWSTR);
};

typedef struct _LVEX : _LV
{
    IOwnerDataCallback *podCallback;
    ISubItemCallback *psiCallback;
} LVEX;

DEFINE_GUID(IID_IListView, 0xE5B16AF2, 0x3990, 0x4681, 0xA6,0x09, 0x1F,0x06,0x0C,0xD1,0x42,0x69);

MIDL_INTERFACE("E5B16AF2-3990-4681-A609-1F060CD14269")
IListView : IOleWindow
{
    STDMETHOD(GetImageList)(int iImageList, HIMAGELIST *phiml) PURE;
    STDMETHOD(SetImageList)(int iImageList, HIMAGELIST himl, HIMAGELIST *phimlOld) PURE;
    STDMETHOD(GetBackgroundColor)(COLORREF *pclr) PURE;
    STDMETHOD(SetBackgroundColor)(COLORREF clr) PURE;
    STDMETHOD(GetTextColor)(COLORREF *pclr) PURE;
    STDMETHOD(SetTextColor)(COLORREF clr) PURE;
    STDMETHOD(GetTextBackgroundColor)(COLORREF *pclr) PURE;
    STDMETHOD(SetTextBackgroundColor)(COLORREF clr) PURE;
    STDMETHOD(GetHotLightColor)(COLORREF *pclr) PURE;
    STDMETHOD(SetHotLightColor)(COLORREF clr) PURE;
    STDMETHOD(GetItemCount)(int *pcItems) PURE;
    STDMETHOD(SetItemCount)(int cItems, DWORD dwFlags) PURE;
    STDMETHOD(GetItem)(LPLVITEMW pitem) PURE;
    STDMETHOD(SetItem)(const LPLVITEMW pitem) PURE;
    STDMETHOD(GetItemState)(int iItem, int iGroup, DWORD dwMask, LPDWORD pdwState) PURE;
    STDMETHOD(SetItemState)(int iItem, int iGroup, DWORD dwMask, DWORD dwState) PURE;
    STDMETHOD(GetItemText)(int iItem, int iSubItem, LPWSTR pszText, int cchText) PURE;
    STDMETHOD(SetItemText)(int iItem, int iSubItem, LPWSTR pszText) PURE;
    STDMETHOD(GetBackgroundImage)(LPLVBKIMAGEW plvbki) PURE;
    STDMETHOD(SetBackgroundImage)(const LPLVBKIMAGEW plvbki) PURE;
    STDMETHOD(GetFocusedColumn)(int *piColumn) PURE;
    STDMETHOD(SetSelectionFlags)(SELECTION_FLAGS dwMask, SELECTION_FLAGS dwFlags) PURE;
    STDMETHOD(GetSelectedColumn)(int *piColumn) PURE;
    STDMETHOD(SetSelectedColumn)(int iColumn) PURE;
    STDMETHOD(GetView)(LPDWORD pdwView) PURE;
    STDMETHOD(SetView)(DWORD wView) PURE;
    STDMETHOD(InsertItem)(const LPLVITEMW pitem, int *piIndex) PURE;
    STDMETHOD(DeleteItem)(int iItem) PURE;
    STDMETHOD(DeleteAllItems)() PURE;
    STDMETHOD(UpdateItem)(int iItem) PURE;
    STDMETHOD(GetItemRect)(LVITEMINDEX iiItem, UINT iCode, LPRECT prc) PURE;
    STDMETHOD(GetSubItemRect)(LVITEMINDEX iiItem, int iSubItem, UINT iCode, LPRECT prc) PURE;
    STDMETHOD(HitTestSubItem)(LPLVHITTESTINFO phti) PURE;
    STDMETHOD(GetIncrSearchString)(LPWSTR pszText, DWORD cch, int *pcchLen) PURE;
    STDMETHOD(GetItemSpacing)(BOOL fSmall, int *pcx, int *pcy) PURE;
    STDMETHOD(SetIconSpacing)(WORD cx, WORD cy, int *pcxPrev, int *pcyPrev) PURE;
    STDMETHOD(GetNextItem)(LVITEMINDEX iiStart, DWORD dwFlags, LVITEMINDEX *piiNext) PURE;
    STDMETHOD(FindItem)(LVITEMINDEX iiStart, const LVFINDINFOW *plvfi, LVITEMINDEX *piiItem) PURE;
    STDMETHOD(GetSelectionMark)(LVITEMINDEX *piiItem) PURE;
    STDMETHOD(SetSelectionMark)(LVITEMINDEX iiStart, LVITEMINDEX *piiItemPrev) PURE;
    STDMETHOD(GetItemPosition)(LVITEMINDEX iiItem, LPPOINT ppt) PURE;
    STDMETHOD(SetItemPosition)(int iItem, LPPOINT ppt) PURE;
    STDMETHOD(ScrollView)(int dx, int dy) PURE;
    STDMETHOD(EnsureItemVisible)(LVITEMINDEX iiItem, BOOL fPartialOK) PURE;
    STDMETHOD(EnsureSubItemVisible)(LVITEMINDEX iiItem, int iSubItem) PURE;
    STDMETHOD(EditSubItem)(LVITEMINDEX iiItem, int iSubItem) PURE;
    STDMETHOD(RedrawItems)(int iFirst, int iLast) PURE;
    STDMETHOD(ArrangeItems)(UINT iCode) PURE;
    STDMETHOD(RecomputeItems)(BOOL fForce) PURE;
    STDMETHOD(GetEditControl)(HWND *phwnd) PURE;
    STDMETHOD(EditLabel)(LVITEMINDEX iiItem, LPWSTR pszText, HWND *phwnd) PURE;
    STDMETHOD(EditGroupLabel)(int iGroup) PURE;
    STDMETHOD(CancelEditLabel)() PURE;
    STDMETHOD(GetEditItem)(LVITEMINDEX *piiItem, int *piSubItem) PURE;
    STDMETHOD(HitTest)(LPLVHITTESTINFO *pinfo) PURE;
    STDMETHOD(GetStringWidth)(LPWSTR psz, int *piWidth) PURE;
    STDMETHOD(GetColumn)(int iColumn, LPLVCOLUMNW pcolumn) PURE;
    STDMETHOD(SetColumn)(int iColumn, const LPLVCOLUMNW pcolumn) PURE;
    STDMETHOD(GetColumnOrderArray)(int cColumns, int *rgiColumns) PURE;
    STDMETHOD(SetColumnOrderArray)(int cColumns, const int *rgiColumns) PURE;
    STDMETHOD(GetHeaderControl)(HWND *phwnd) PURE;
    STDMETHOD(InsertColumn)(int iColumn, const LPLVCOLUMNW pcolumn, int *piColumn) PURE;
    STDMETHOD(DeleteColumn)(int iColumn) PURE;
    STDMETHOD(CreateDragImage)(int iItem, LPPOINT ppt, HIMAGELIST *phiml) PURE;
    STDMETHOD(GetViewRect)(LPRECT prc) PURE;
    STDMETHOD(GetClientRect)(BOOL fScrollbars, LPRECT prc) PURE;
    STDMETHOD(GetColumnWidth)(int iColumn, int *piWidth) PURE;
    STDMETHOD(SetColumnWidth)(int iColumn, int iWidth) PURE;
    STDMETHOD(GetCallbackMask)(LPDWORD pdwMask) PURE;
    STDMETHOD(SetCallbackMask)(DWORD dwMask) PURE;
    STDMETHOD(GetTopIndex)(int *piItem) PURE;
    STDMETHOD(GetCountPerPage)(int *pcItems) PURE;
    STDMETHOD(GetOrigin)(LPPOINT pptOrigin) PURE;
    STDMETHOD(GetSelectedCount)(int *pcItems) PURE;
    STDMETHOD(SortItems)(BOOL fByIndex, LPARAM lParamSort, PFNLVCOMPARE pfnCompare) PURE;
    STDMETHOD(GetExtendedStyle)(DWORD *pdwExStyle) PURE;
    STDMETHOD(SetExtendedStyle)(DWORD dwExMask, DWORD dwExStyle, DWORD *pdwExStylePrev) PURE;
    STDMETHOD(GetHoverTime)(UINT *puMilliseconds) PURE;
    STDMETHOD(SetHoverTime)(UINT uMilliseconds, UINT *puMillisecondsPrev) PURE;
    STDMETHOD(GetTooltip)(HWND *phwnd) PURE;
    STDMETHOD(SetToolTip)(HWND hwnd, HWND *phwndPrev) PURE;
    STDMETHOD(GetHotItem)(LVITEMINDEX *piiItem) PURE;
    STDMETHOD(SetHotItem)(LVITEMINDEX iiItem, LVITEMINDEX *piiItemPrev) PURE;
    STDMETHOD(GetHotCursor)(HICON *phCursor) PURE;
    STDMETHOD(SetHotCursor)(HICON hCursor, HICON *phCursorPrev) PURE;
    STDMETHOD(ApproximateViewRect)(int cItems, int *pcx, int *pcy) PURE;
    STDMETHOD(SetRangeObject)(int iWhich, ILVRange *prange) PURE;
    STDMETHOD(GetWorkAreas)(int cAreas, LPRECT rgrc) PURE;
    STDMETHOD(SetWorkAreas)(int cAreas, LPCRECT rgrc) PURE;
    STDMETHOD(GetWorkAreaCount)(int *pcAreas) PURE;
    STDMETHOD(ResetEmptyText)() PURE;
    STDMETHOD(EnableGroupView)(BOOL fEnable) PURE;
    STDMETHOD(IsGroupViewEnabled)(BOOL *pfEnabled) PURE;
    STDMETHOD(SortGroups)(PFNLVGROUPCOMPARE pfnCompare, void *pvoid) PURE;
    STDMETHOD(GetGroupInfo)(BOOL fByIndex, int iGroupId, PLVGROUP pgrp) PURE;
    STDMETHOD(SetGroupInfo)(BOOL fByIndex, int iGroupId, const PLVGROUP pgrp) PURE;
    STDMETHOD(GetGroupRect)(BOOL fByIndex, int iGroupId, int iCode, LPRECT prc) PURE;
    STDMETHOD(GetGroupState)(int iGroupId, DWORD dwMask, DWORD *pdwState) PURE;
    STDMETHOD(HasGroup)(int iGroupId, BOOL *pfHasGroup) PURE;
    STDMETHOD(InsertGroup)(int iGroup, const PLVGROUP pgrp, int *piGroup) PURE;
    STDMETHOD(RemoveGroup)(int iGroupId) PURE;
    STDMETHOD(InsertGroupSorted)(const LVINSERTGROUPSORTED *pigs, int *piGroup) PURE;
    STDMETHOD(GetGroupMetrics)(PLVGROUPMETRICS pGroupMetrics) PURE;
    STDMETHOD(SetGroupMetrics)(const PLVGROUPMETRICS pGroupMetrics) PURE;
    STDMETHOD(RemoveAllGroups)() PURE;
    STDMETHOD(GetFocusedGroup)(int *piGroup) PURE;
    STDMETHOD(GetGroupCount)(int *pcGroups) PURE;
    STDMETHOD(SetOwnerDataCallback)(IOwnerDataCallback *pcallback) PURE;
    STDMETHOD(GetTileViewInfo)(PLVTILEVIEWINFO plvtvinfo) PURE;
    STDMETHOD(SetTileViewInfo)(const PLVTILEVIEWINFO plvtvinfo) PURE;
    STDMETHOD(GetTileInfo)(PLVTILEINFO plvtinfo) PURE;
    STDMETHOD(SetTileInfo)(const PLVTILEINFO plvtinfo) PURE;
    STDMETHOD(GetInsertMark)(LVINSERTMARK *plvim) PURE;
    STDMETHOD(SetInsertMark)(const LVINSERTMARK *plvim) PURE;
    STDMETHOD(GetInsertMarkRect)(LPRECT prc) PURE;
    STDMETHOD(GetInsertMarkColor)(COLORREF *pclr) PURE;
    STDMETHOD(SetInsertMarkColor)(COLORREF clr, COLORREF *pclrPrev) PURE;
    STDMETHOD(HitTestInsertMark)(LPPOINT ppt, LVINSERTMARK *plvim) PURE;
    STDMETHOD(SetInfoTip)(const PLVSETINFOTIP plvsit) PURE;
    STDMETHOD(GetOutlineColor)(COLORREF *pclr) PURE;
    STDMETHOD(SetOutlineColor)(COLORREF clr, COLORREF *pclrPrev) PURE;
    STDMETHOD(GetFrozenItem)(int *piItem) PURE;
    STDMETHOD(SetFrozenItem)(BOOL fFreeze, int iItem) PURE;
    STDMETHOD(GetFrozenSlot)(LPRECT prc) PURE;
    STDMETHOD(SetFrozenSlot)(BOOL fFreeze, const POINT *ppt) PURE;
    STDMETHOD(GetViewMargin)(LPRECT prc) PURE;
    STDMETHOD(SetViewMargin)(LPRECT prc) PURE;
    STDMETHOD(SetKeyboardSelected)(LVITEMINDEX iiItem) PURE;
    STDMETHOD(MapIndexToId)(UINT iItem, int *piId) PURE;
    STDMETHOD(MapIdToIndex)(UINT iId, int *piItem) PURE;
    STDMETHOD(IsItemVisible)(LVITEMINDEX iiItem, BOOL *pfVisible) PURE;
    STDMETHOD(EnableAlphaShadow)(bool fEnable) PURE;
    STDMETHOD(GetGroupSubsetCount)(int *pcRows) PURE;
    STDMETHOD(SetGroupSubsetCount)(UINT cRows) PURE;
    STDMETHOD(GetVisibleSlotCount)(int *pcSlots) PURE;
    STDMETHOD(GetColumnMargin)(LPRECT prcMargin) PURE;
    STDMETHOD(SetSubItemCallback)(ISubItemCallback *pcallback) PURE;
    STDMETHOD(GetVisibleItemRange)(PLVITEMINDEX piiStart, PLVITEMINDEX piiEnd) PURE;
    STDMETHOD(SetTypeAheadFlags)(TYPEAHEAD_FLAGS dwMask, TYPEAHEAD_FLAGS dwFlags) PURE;
};

/**
  * Vista and above's CListView implement IListView, which is used by
  * shell32!CListViewHost, which is then used by shell32!CDefView, for the
  * Explorer list view. I don't see much of a reason for this interface to exist,
  * given most of it is done through window messages anyway, but regardless, we
  * must implement it.
  *
  * CListView also implements more interfaces, but none of them seem to be used by
  * any shell components, so I'll just leave them be.
  */
class CListViewXP : public IListView
{
private:
    HWND _hwnd;
    LVEX *_plv;
    ULONG _cRef;

public:
    CListViewXP(HWND hwnd)
        : _hwnd(hwnd)
        , _cRef(1)
    {
        _plv = (LVEX *)GetWindowLongPtrW(_hwnd, 0);
    }

    virtual ~CListViewXP() {}

    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv) noexcept override
    {
        static const QITAB qit[] = {
            QITABENT(CListViewXP, IUnknown),
            // whatever man
            { &IID_IListView, OFFSETOFCLASS(CListViewXP, IListView) },
            { 0 }
        };
        return QISearch(this, qit, riid, ppv);
    }

    STDMETHODIMP_(ULONG) AddRef() noexcept override
    {
        return InterlockedIncrement(&_cRef);
    }

    STDMETHODIMP_(ULONG) Release() noexcept override
    {
        ULONG cRef = InterlockedDecrement(&_cRef);
        if (cRef == 0)
            delete this;
        return cRef;
    }

    // IOleWindow interface
    STDMETHODIMP GetWindow(HWND *phwnd) noexcept override
    {
        *phwnd = _hwnd;
        return _hwnd ? S_OK : E_FAIL;
    }

    STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode) noexcept override
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP GetImageList(int iImageList, HIMAGELIST *phiml) noexcept override
    {
        Wh_Log(L"GetImageList");
        *phiml = ListView_GetImageList(_hwnd, iImageList);
        return *phiml ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetImageList(int iImageList, HIMAGELIST himl, HIMAGELIST *phimlOld) noexcept override
    {
        Wh_Log(L"SetImageList, iImageList: %d", iImageList);
        HIMAGELIST himlOld;

// New value introduced sometime after XP that sets both normal and small
// imagelists
#define LVSIL_NORMAL_SMALL 5

        if (iImageList == LVSIL_NORMAL_SMALL)
        {
            ListView_SetImageList(_hwnd, himl, LVSIL_NORMAL);
            himlOld = ListView_SetImageList(_hwnd, himl, LVSIL_SMALL);
        }
        else
        {
            himlOld = ListView_SetImageList(_hwnd, himl, iImageList);
        }
        if (phimlOld)
            *phimlOld = himlOld;
        Wh_Log(L"himlOld: %X", himlOld);
        return S_OK;
    }

    STDMETHODIMP GetBackgroundColor(COLORREF *pclr) noexcept override
    {
        Wh_Log(L"GetBackgroundColor");
        *pclr = ListView_GetBkColor(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetBackgroundColor(COLORREF clr) noexcept override
    {
        Wh_Log(L"SetBackgroundColor");
        return ListView_SetBkColor(_hwnd, clr) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetTextColor(COLORREF *pclr) noexcept override
    {
        Wh_Log(L"GetTextColor");
        *pclr = ListView_GetTextColor(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetTextColor(COLORREF clr) noexcept override
    {
        Wh_Log(L"SetTextColor");
        return ListView_SetTextColor(_hwnd, clr) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetTextBackgroundColor(COLORREF *pclr) noexcept override
    {
        Wh_Log(L"GetTextBackgroundColor");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetTextBackgroundColor(COLORREF clr) noexcept override
    {
        Wh_Log(L"SetTextBackgroundColor");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetHotLightColor(COLORREF *pclr) noexcept override
    {
        Wh_Log(L"GetHotLightColor");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetHotLightColor(COLORREF clr) noexcept override
    {
        Wh_Log(L"SetHotLightColor");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetItemCount(int *pcItems) noexcept override
    {
        Wh_Log(L"GetItemCount");
        *pcItems = ListView_GetItemCount(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetItemCount(int cItems, DWORD dwFlags) noexcept override
    {
        Wh_Log(L"SetItemCount");
        ListView_SetItemCountEx(_hwnd, cItems, dwFlags);
        return E_NOTIMPL;
    }

    STDMETHODIMP GetItem(LPLVITEMW pitem) noexcept override
    {
        Wh_Log(L"GetItem");
        return ListView_GetItem(_hwnd, pitem) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetItem(const LPLVITEMW pitem) noexcept override
    {
        Wh_Log(L"SetItem");
        return ListView_SetItem(_hwnd, pitem) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetItemState(int iItem, int iGroup, DWORD dwMask, LPDWORD pdwState) noexcept override
    {
        Wh_Log(L"GetItemState");
        *pdwState = ListView_GetItemState(_hwnd, iItem, dwMask);
        return S_OK;
    }

    STDMETHODIMP SetItemState(int iItem, int iGroup, DWORD dwMask, DWORD dwState) noexcept override
    {
        Wh_Log(L"SetItemState");
        ListView_SetItemState(_hwnd, iItem, dwState, dwMask);
        return S_OK;
    }

    STDMETHODIMP GetItemText(int iItem, int iSubItem, LPWSTR pszText, int cchText) noexcept override
    {
        Wh_Log(L"GetItemText");
        ListView_GetItemText(_hwnd, iItem, iSubItem, pszText, cchText);
        return S_OK;
    }

    STDMETHODIMP SetItemText(int iItem, int iSubItem, LPWSTR pszText) noexcept override
    {
        Wh_Log(L"SetItemText");
        ListView_SetItemText(_hwnd, iItem, iSubItem, pszText);
        return S_OK;
    }

    STDMETHODIMP GetBackgroundImage(LPLVBKIMAGEW plvbki) noexcept override
    {
        Wh_Log(L"GetBackgroundImage");
        return ListView_GetBkImage(_hwnd, plvbki) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetBackgroundImage(const LPLVBKIMAGEW plvbki) noexcept override
    {
        Wh_Log(L"SetBackgroundImage");
        return ListView_SetBkImage(_hwnd, plvbki) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetFocusedColumn(int *piColumn) noexcept override
    {    
        Wh_Log(L"GetFocusedColumn");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetSelectionFlags(SELECTION_FLAGS dwMask, SELECTION_FLAGS dwFlags) noexcept override
    {
        Wh_Log(L"SetSelectionFlags");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetSelectedColumn(int *piColumn) noexcept override
    {
        Wh_Log(L"GetSelectedColumn");
        *piColumn = ListView_GetSelectedColumn(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetSelectedColumn(int iColumn) noexcept override
    {
        Wh_Log(L"SetSelectedColumn");
        return ListView_SetSelectedColumn(_hwnd, iColumn) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetView(LPDWORD pdwView) noexcept override
    {
        Wh_Log(L"GetView");
        *pdwView = ListView_GetView(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetView(DWORD wView) noexcept override
    {
        Wh_Log(L"SetView");
        return (ListView_SetView(_hwnd, wView) == (DWORD)-1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP InsertItem(const LPLVITEMW pitem, int *piIndex) noexcept override
    {
        Wh_Log(L"InsertItem");
        *piIndex = ListView_InsertItem(_hwnd, pitem);
        return (*piIndex == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP DeleteItem(int iItem) noexcept override
    {
        Wh_Log(L"DeleteItem");
        return ListView_DeleteItem(_hwnd, iItem) ? S_OK : E_FAIL;
    }

    STDMETHODIMP DeleteAllItems() noexcept override
    {
        Wh_Log(L"DeleteAllItems");
        return ListView_DeleteAllItems(_hwnd) ? S_OK : E_FAIL;
    }

    STDMETHODIMP UpdateItem(int iItem) noexcept override
    {
        Wh_Log(L"UpdateItem");
        return ListView_Update(_hwnd, iItem) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetItemRect(LVITEMINDEX iiItem, UINT iCode, LPRECT prc) noexcept override
    {
        Wh_Log(L"GetItemRect");
        return ListView_GetItemRect(_hwnd, iiItem.iItem, prc, iCode) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetSubItemRect(LVITEMINDEX iiItem, int iSubItem, UINT iCode, LPRECT prc) noexcept override
    {
        Wh_Log(L"GetSubItemRect");
        return ListView_GetSubItemRect(_hwnd, iiItem.iItem, iSubItem, iCode, prc) ? S_OK : E_FAIL;
    }

    STDMETHODIMP HitTestSubItem(LPLVHITTESTINFO phti) noexcept override
    {
        Wh_Log(L"HitTestSubItem");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetIncrSearchString(LPWSTR pszText, DWORD cch, int *pcchLen) noexcept override
    {
        Wh_Log(L"GetIncrSearchString");
        return ListView_GetISearchString(_hwnd, pszText) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetItemSpacing(BOOL fSmall, int *pcx, int *pcy) noexcept override
    {
        Wh_Log(L"GetItemSpacing");
        DWORD dwSpacing = ListView_GetItemSpacing(_hwnd, fSmall);
        *pcx = LOWORD(dwSpacing);
        *pcy = HIWORD(dwSpacing);
        return S_OK;
    }

    STDMETHODIMP SetIconSpacing(WORD cx, WORD cy, int *pcxPrev, int *pcyPrev) noexcept override
    {
        Wh_Log(L"SetIconSpacing");
        ListView_SetIconSpacing(_hwnd, cx, cy);
        return S_OK;
    }

    STDMETHODIMP GetNextItem(LVITEMINDEX iiStart, DWORD dwFlags, LVITEMINDEX *piiNext) noexcept override
    {
        Wh_Log(L"GetNextItem");
        int iItem = ListView_GetNextItem(_hwnd, iiStart.iItem, dwFlags);
        piiNext->iItem = iItem;
        piiNext->iGroup = 0;
        return (iItem == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP FindItem(LVITEMINDEX iiStart, const LVFINDINFOW *plvfi, LVITEMINDEX *piiItem) noexcept override
    {
        Wh_Log(L"FindItem");
        int iItem = ListView_FindItem(_hwnd, iiStart.iItem, plvfi);
        piiItem->iItem = iItem;
        piiItem->iGroup = 0;
        return (iItem == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP GetSelectionMark(LVITEMINDEX *piiItem) noexcept override
    {
        Wh_Log(L"GetSelectionMark");
        piiItem->iItem = ListView_GetSelectionMark(_hwnd);
        piiItem->iGroup = 0;
        return (piiItem->iItem == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP SetSelectionMark(LVITEMINDEX iiStart, LVITEMINDEX *piiItemPrev) noexcept override
    {
        Wh_Log(L"SetSelectionMark");
        piiItemPrev->iItem = ListView_SetSelectionMark(_hwnd, iiStart.iItem);
        piiItemPrev->iGroup = 0;
        return S_OK;
    }

    STDMETHODIMP GetItemPosition(LVITEMINDEX iiItem, LPPOINT ppt) noexcept override
    {
        Wh_Log(L"GetItemPosition");
        return ListView_GetItemPosition(_hwnd, iiItem.iItem, ppt) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetItemPosition(int iItem, LPPOINT ppt) noexcept override
    {
        Wh_Log(L"SetItemPosition");
        ListView_SetItemPosition32(_hwnd, iItem, ppt->x, ppt->y);
        return E_NOTIMPL;
    }

    STDMETHODIMP ScrollView(int dx, int dy) noexcept override
    {
        Wh_Log(L"ScrollView");
        return ListView_Scroll(_hwnd, dx, dy) ? S_OK : E_FAIL;
    }

    STDMETHODIMP EnsureItemVisible(LVITEMINDEX iiItem, BOOL fPartialOK) noexcept override
    {
        Wh_Log(L"EnsureItemVisible");
        return ListView_EnsureVisible(_hwnd, iiItem.iItem, fPartialOK) ? S_OK : E_FAIL;
    }

    STDMETHODIMP EnsureSubItemVisible(LVITEMINDEX iiItem, int iSubItem) noexcept override
    {
        Wh_Log(L"EnsureSubItemVisible");
        return E_NOTIMPL;
    }

    STDMETHODIMP EditSubItem(LVITEMINDEX iiItem, int iSubItem) noexcept override
    {
        Wh_Log(L"EditSubItem");
        return E_NOTIMPL;
    }

    STDMETHODIMP RedrawItems(int iFirst, int iLast) noexcept override
    {
        Wh_Log(L"RedrawItems");
        return ListView_RedrawItems(_hwnd, iFirst, iLast) ? S_OK : E_FAIL;
    }

    STDMETHODIMP ArrangeItems(UINT iCode) noexcept override
    {
        Wh_Log(L"ArrangeItems");
        return ListView_Arrange(_hwnd, iCode) ? S_OK : E_FAIL;
    }

    STDMETHODIMP RecomputeItems(BOOL fForce) noexcept override
    {
        Wh_Log(L"RecomputeItems");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetEditControl(HWND *phwnd) noexcept override
    {
        Wh_Log(L"GetEditControl");
        *phwnd = ListView_GetEditControl(_hwnd);
        return *phwnd ? S_OK : E_FAIL;
    }

    STDMETHODIMP EditLabel(LVITEMINDEX iiItem, LPWSTR pszText, HWND *phwnd) noexcept override
    {
        Wh_Log(L"EditLabel");
        HWND hwnd = ListView_EditLabel(_hwnd, iiItem.iItem);
        if (phwnd)
            *phwnd = hwnd;
        return S_OK;
    }

    STDMETHODIMP EditGroupLabel(int iGroup) noexcept override
    {
        Wh_Log(L"EditGroupLabel");
        return E_NOTIMPL;
    }

    STDMETHODIMP CancelEditLabel() noexcept override
    {
        Wh_Log(L"CancelEditLabel");
        ListView_CancelEditLabel(_hwnd);
        return S_OK;
    }

    STDMETHODIMP GetEditItem(LVITEMINDEX *piiItem, int *piSubItem) noexcept override
    {
        Wh_Log(L"GetEditItem");
        return E_NOTIMPL;
    }

    STDMETHODIMP HitTest(LPLVHITTESTINFO *pinfo) noexcept override
    {
        Wh_Log(L"HitTest");

        return (ListView_HitTest(_hwnd, pinfo) == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP GetStringWidth(LPWSTR psz, int *piWidth) noexcept override
    {
        Wh_Log(L"GetStringWidth");
        *piWidth = ListView_GetStringWidth(_hwnd, psz);
        return *piWidth ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetColumn(int iColumn, LPLVCOLUMNW pcolumn) noexcept override
    {
        Wh_Log(L"GetColumn");
        return ListView_GetColumn(_hwnd, iColumn, pcolumn) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetColumn(int iColumn, const LPLVCOLUMNW pcolumn) noexcept override
    {
        Wh_Log(L"SetColumn");
        return ListView_SetColumn(_hwnd, iColumn, pcolumn) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetColumnOrderArray(int cColumns, int *rgiColumns) noexcept override
    {
        Wh_Log(L"GetColumnOrderArray");
        return ListView_GetColumnOrderArray(_hwnd, cColumns, rgiColumns) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetColumnOrderArray(int cColumns, const int *rgiColumns) noexcept override
    {
        Wh_Log(L"SetColumnOrderArray");
        return ListView_SetColumnOrderArray(_hwnd, cColumns, rgiColumns) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetHeaderControl(HWND *phwnd) noexcept override
    {
        Wh_Log(L"GetHeaderControl");
        *phwnd = ListView_GetHeader(_hwnd);
        return S_OK;
    }

    STDMETHODIMP InsertColumn(int iColumn, const LPLVCOLUMNW pcolumn, int *piColumn) noexcept override
    {
        Wh_Log(L"InsertColumn");
        *piColumn = ListView_InsertColumn(_hwnd, iColumn, pcolumn);
        return (*piColumn == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP DeleteColumn(int iColumn) noexcept override
    {
        Wh_Log(L"DeleteColumn");
        return ListView_DeleteColumn(_hwnd, iColumn) ? S_OK : E_FAIL;
    }

    STDMETHODIMP CreateDragImage(int iItem, LPPOINT ppt, HIMAGELIST *phiml) noexcept override
    {
        Wh_Log(L"CreateDragImage");
        *phiml = ListView_CreateDragImage(_hwnd, iItem, ppt);
        return *phiml ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetViewRect(LPRECT prc) noexcept override
    {
        Wh_Log(L"GetViewRect");
        return ListView_GetViewRect(_hwnd, prc) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetClientRect(BOOL fScrollbars, LPRECT prc) noexcept override
    {
        Wh_Log(L"GetClientRect");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetColumnWidth(int iColumn, int *piWidth) noexcept override
    {
        Wh_Log(L"GetColumnWidth");
        *piWidth = ListView_GetColumnWidth(_hwnd, iColumn);
        return *piWidth ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetColumnWidth(int iColumn, int iWidth) noexcept override
    {
        Wh_Log(L"SetColumnWidth");
        return ListView_SetColumnWidth(_hwnd, iColumn, iWidth) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetCallbackMask(LPDWORD pdwMask) noexcept override
    {
        Wh_Log(L"GetCallbackMask");
        *pdwMask = ListView_GetCallbackMask(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetCallbackMask(DWORD dwMask) noexcept override
    {
        Wh_Log(L"SetCallbackMask");
        return ListView_SetCallbackMask(_hwnd, dwMask) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetTopIndex(int *piItem) noexcept override
    {
        Wh_Log(L"GetTopIndex");
        *piItem = ListView_GetTopIndex(_hwnd);
        return S_OK;
    }

    STDMETHODIMP GetCountPerPage(int *pcItems) noexcept override
    {
        Wh_Log(L"GetCountPerPage");
        *pcItems = ListView_GetCountPerPage(_hwnd);
        return S_OK;
    }

    STDMETHODIMP GetOrigin(LPPOINT pptOrigin) noexcept override
    {
        Wh_Log(L"GetOrigin");
        return ListView_GetOrigin(_hwnd, pptOrigin) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetSelectedCount(int *pcItems) noexcept override
    {
        Wh_Log(L"GetSelectedCount");
        *pcItems = ListView_GetSelectedCount(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SortItems(BOOL fByIndex, LPARAM lParamSort, PFNLVCOMPARE pfnCompare) noexcept override
    {
        Wh_Log(L"SortItems");
        return ListView_SortItems(_hwnd, pfnCompare, lParamSort) ? S_OK : E_FAIL;
        return E_NOTIMPL;
    }

    STDMETHODIMP GetExtendedStyle(DWORD *pdwExStyle) noexcept override
    {
        Wh_Log(L"GetExtendedStyle");
        *pdwExStyle = ListView_GetExtendedListViewStyle(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetExtendedStyle(DWORD dwExMask, DWORD dwExStyle, DWORD *pdwExStylePrev) noexcept override
    {
        Wh_Log(L"SetExtendedStyle");
        DWORD dwExStylePrev = ListView_SetExtendedListViewStyleEx(_hwnd, dwExMask, dwExStyle);
        if (pdwExStylePrev)
            *pdwExStylePrev = dwExStylePrev;
        return S_OK;
    }

    STDMETHODIMP GetHoverTime(UINT *puMilliseconds) noexcept override
    {
        Wh_Log(L"GetHoverTime");
        *puMilliseconds = ListView_GetHoverTime(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetHoverTime(UINT uMilliseconds, UINT *puMillisecondsPrev) noexcept override
    {
        Wh_Log(L"SetHoverTime");
        *puMillisecondsPrev = ListView_SetHoverTime(_hwnd, uMilliseconds);
        return S_OK;
    }

    STDMETHODIMP GetTooltip(HWND *phwnd) noexcept override
    {
        Wh_Log(L"GetTooltip");
        *phwnd = ListView_GetToolTips(_hwnd);
        return *phwnd ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetToolTip(HWND hwnd, HWND *phwndPrev) noexcept override
    {
        Wh_Log(L"SetToolTip");
        *phwndPrev = ListView_SetToolTips(_hwnd, hwnd);
        return S_OK;
    }

    STDMETHODIMP GetHotItem(LVITEMINDEX *piiItem) noexcept override
    {
        Wh_Log(L"GetHotItem");
        piiItem->iItem = ListView_GetHotItem(_hwnd);
        piiItem->iGroup = 0;
        return S_OK;
    }

    STDMETHODIMP SetHotItem(LVITEMINDEX iiItem, LVITEMINDEX *piiItemPrev) noexcept override
    {
        Wh_Log(L"SetHotItem");
        piiItemPrev->iItem = ListView_SetHotItem(_hwnd, iiItem.iItem);
        piiItemPrev->iGroup = 0;
        return S_OK;
    }

    STDMETHODIMP GetHotCursor(HICON *phCursor) noexcept override
    {
        Wh_Log(L"GetHotCursor");
        *phCursor = ListView_GetHotCursor(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetHotCursor(HICON hCursor, HICON *phCursorPrev) noexcept override
    {
        Wh_Log(L"SetHotCursor");
        HCURSOR hCursorPrev = ListView_SetHotCursor(_hwnd, hCursor);
        if (phCursorPrev)
            *phCursorPrev = hCursorPrev;
        return S_OK;
    }

    STDMETHODIMP ApproximateViewRect(int cItems, int *pcx, int *pcy) noexcept override
    {
        Wh_Log(L"ApproximateViewRect");
        DWORD dwSize = ListView_ApproximateViewRect(
            _hwnd,
            pcx ? *pcx : -1,
            pcy ? *pcy : -1,
            cItems
        );
        *pcx = LOWORD(dwSize);
        *pcy = HIWORD(dwSize);
        return S_OK;
    }

    STDMETHODIMP SetRangeObject(int iWhich, ILVRange *prange) noexcept override
    {
        Wh_Log(L"SetRangeObject");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetWorkAreas(int cAreas, LPRECT rgrc) noexcept override
    {
        Wh_Log(L"GetWorkAreas");
        ListView_GetWorkAreas(_hwnd, cAreas, rgrc);
        return S_OK;
    }

    STDMETHODIMP SetWorkAreas(int cAreas, LPCRECT rgrc) noexcept override
    {
        Wh_Log(L"SetWorkAreas");
        ListView_SetWorkAreas(_hwnd, cAreas, rgrc);
        return S_OK;
    }

    STDMETHODIMP GetWorkAreaCount(int *pcAreas) noexcept override
    {
        Wh_Log(L"GetWorkAreaCount");
        return ListView_GetNumberOfWorkAreas(_hwnd, pcAreas) ? S_OK : E_FAIL;
    }

    STDMETHODIMP ResetEmptyText() noexcept override
    {
        Wh_Log(L"ResetEmptyText");
        return E_NOTIMPL;
    }

    STDMETHODIMP EnableGroupView(BOOL fEnable) noexcept override
    {
        Wh_Log(L"EnableGroupView");
        return (ListView_EnableGroupView(_hwnd, fEnable) == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP IsGroupViewEnabled(BOOL *pfEnabled) noexcept override
    {
        Wh_Log(L"IsGroupViewEnabled");
        *pfEnabled = ListView_IsGroupViewEnabled(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SortGroups(PFNLVGROUPCOMPARE pfnCompare, void *pvoid) noexcept override
    {
        Wh_Log(L"SortGroups");
        return ListView_SortGroups(_hwnd, pfnCompare, pvoid) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetGroupInfo(BOOL fByIndex, int iGroupId, PLVGROUP pgrp) noexcept override
    {
        Wh_Log(L"GetGroupInfo");
        return (ListView_GetGroupInfo(_hwnd, iGroupId, pgrp) == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP SetGroupInfo(BOOL fByIndex, int iGroupId, const PLVGROUP pgrp) noexcept override
    {
        Wh_Log(L"SetGroupInfo");
        return (ListView_SetGroupInfo(_hwnd, iGroupId, pgrp) == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP GetGroupRect(BOOL fByIndex, int iGroupId, int iCode, LPRECT prc) noexcept override
    {
        Wh_Log(L"GetGroupRect");
        return ListView_GetGroupRect(_hwnd, iGroupId, iCode, prc) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetGroupState(int iGroupId, DWORD dwMask, DWORD *pdwState) noexcept override
    {
        Wh_Log(L"GetGroupState");
        *pdwState = ListView_GetGroupState(_hwnd, iGroupId, dwMask);
        return S_OK;
    }

    STDMETHODIMP HasGroup(int iGroupId, BOOL *pfHasGroup) noexcept override
    {
        Wh_Log(L"HasGroup");
        *pfHasGroup = ListView_HasGroup(_hwnd, iGroupId);
        return S_OK;
    }

    STDMETHODIMP InsertGroup(int iGroup, const PLVGROUP pgrp, int *piGroup) noexcept override
    {
        Wh_Log(L"InsertGroup");
        int iGroupInserted = ListView_InsertGroup(_hwnd, iGroup, pgrp);
        if (piGroup && iGroupInserted != -1)
            *piGroup = iGroupInserted;
        return (iGroupInserted == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP RemoveGroup(int iGroupId) noexcept override
    {
        Wh_Log(L"RemoveGroup");
        return (ListView_RemoveGroup(_hwnd, iGroupId) == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP InsertGroupSorted(const LVINSERTGROUPSORTED *pigs, int *piGroup) noexcept override
    {
        Wh_Log(L"InsertGroupSorted");
        int iGroup = ListView_InsertGroupSorted(_hwnd, pigs);
        if (piGroup)
            *piGroup = iGroup;
        return (iGroup == -1) ? E_FAIL : S_OK;
    }

    STDMETHODIMP GetGroupMetrics(PLVGROUPMETRICS pGroupMetrics) noexcept override
    {
        Wh_Log(L"GetGroupMetrics");
        ListView_GetGroupMetrics(_hwnd, pGroupMetrics);
        return S_OK;
    }

    STDMETHODIMP SetGroupMetrics(const PLVGROUPMETRICS pGroupMetrics) noexcept override
    {
        Wh_Log(L"SetGroupMetrics");
        ListView_SetGroupMetrics(_hwnd, pGroupMetrics);
        return S_OK;
    }

    STDMETHODIMP RemoveAllGroups() noexcept override
    {
        Wh_Log(L"RemoveAllGroups");
        ListView_RemoveAllGroups(_hwnd);
        return S_OK;
    }

    STDMETHODIMP GetFocusedGroup(int *piGroup) noexcept override
    {
        Wh_Log(L"GetFocusedGroup");
        *piGroup = ListView_GetFocusedGroup(_hwnd);
        return S_OK;
    }

    STDMETHODIMP GetGroupCount(int *pcGroups) noexcept override
    {
        Wh_Log(L"GetGroupCount");
        *pcGroups = ListView_GetGroupCount(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetOwnerDataCallback(IOwnerDataCallback *pcallback) noexcept override
    {
        Wh_Log(L"SetOwnerDataCallback");
        if (!pcallback)
            return E_INVALIDARG;
        if (!_plv)
            return E_FAIL;

        _plv->podCallback = pcallback;
        pcallback->AddRef();
        return S_OK;
    }

    STDMETHODIMP GetTileViewInfo(PLVTILEVIEWINFO plvtvinfo) noexcept override
    {
        Wh_Log(L"GetTileViewInfo");
        return ListView_GetTileViewInfo(_hwnd, plvtvinfo) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetTileViewInfo(const PLVTILEVIEWINFO plvtvinfo) noexcept override
    {
        Wh_Log(L"SetTileViewInfo");
        return ListView_SetTileViewInfo(_hwnd, plvtvinfo) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetTileInfo(PLVTILEINFO plvtinfo) noexcept override
    {
        Wh_Log(L"GetTileInfo");
        return ListView_GetTileInfo(_hwnd, plvtinfo) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetTileInfo(const PLVTILEINFO plvtinfo) noexcept override
    {
        Wh_Log(L"SetTileInfo");
        return ListView_SetTileInfo(_hwnd, plvtinfo) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetInsertMark(LVINSERTMARK *plvim) noexcept override
    {
        Wh_Log(L"GetInsertMark");
        return ListView_GetInsertMark(_hwnd, plvim) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetInsertMark(const LVINSERTMARK *plvim) noexcept override
    {
        Wh_Log(L"SetInsertMark");
        return ListView_SetInsertMark(_hwnd, plvim) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetInsertMarkRect(LPRECT prc) noexcept override
    {
        Wh_Log(L"GetInsertMarkRect");
        ListView_GetInsertMarkRect(_hwnd, prc);
        return S_OK;
    }

    STDMETHODIMP GetInsertMarkColor(COLORREF *pclr) noexcept override
    {
        Wh_Log(L"GetInsertMarkColor");
        *pclr = ListView_GetInsertMarkColor(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetInsertMarkColor(COLORREF clr, COLORREF *pclrPrev) noexcept override
    {
        Wh_Log(L"SetInsertMarkColor");
        COLORREF clrPrev = ListView_SetInsertMarkColor(_hwnd, clr);
        if (pclrPrev)
            *pclrPrev = clrPrev;
        return S_OK;
    }

    STDMETHODIMP HitTestInsertMark(LPPOINT ppt, LVINSERTMARK *plvim) noexcept override
    {
        Wh_Log(L"HitTestInsertMark");
        return ListView_InsertMarkHitTest(_hwnd, ppt, plvim) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetInfoTip(const PLVSETINFOTIP plvsit) noexcept override
    {
        Wh_Log(L"SetInfoTip");
        return ListView_SetInsertMark(_hwnd, plvsit) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetOutlineColor(COLORREF *pclr) noexcept override
    {
        Wh_Log(L"GetOutlineColor");
        *pclr = ListView_GetOutlineColor(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetOutlineColor(COLORREF clr, COLORREF *pclrPrev) noexcept override
    {
        Wh_Log(L"SetOutlineColor");
        COLORREF clrPrev = ListView_SetOutlineColor(_hwnd, clr);
        if (pclrPrev)
            *pclrPrev = clrPrev;
        return E_NOTIMPL;
    }

    STDMETHODIMP GetFrozenItem(int *piItem) noexcept override
    {

#define LVM_GETFROZENITEM       (LVM_FIRST + 86)
#define ListView_GetFrozenItem(hwndLV)\
        (int)SNDMSG((hwndLV), LVM_GETFROZENITEM, 0, 0)

        Wh_Log(L"GetFrozenItem");
        *piItem = ListView_GetFrozenItem(_hwnd);
        return S_OK;
    }

    STDMETHODIMP SetFrozenItem(BOOL fFreeze, int iItem) noexcept override
    {

#define LVM_SETFROZENITEM       (LVM_FIRST + 85)
#define ListView_SetFrozenItem(hwndLV, fFreezeOrUnfreeze, iIndex)\
        (BOOL)SNDMSG((hwndLV), LVM_SETFROZENITEM, (WPARAM)(fFreezeOrUnfreeze), (LPARAM)(iIndex))

        Wh_Log(L"SetFrozenItem");
        return ListView_SetFrozenItem(_hwnd, fFreeze, iItem) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetFrozenSlot(LPRECT prc) noexcept override
    {
        Wh_Log(L"GetFrozenSlot");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetFrozenSlot(BOOL fFreeze, const POINT *ppt) noexcept override
    {

#define LVM_SETFROZENSLOT       (LVM_FIRST + 88)
#define ListView_SetFrozenSlot(hwndLV, fFreezeOrUnfreeze, lpPt)\
        (BOOL)SNDMSG((hwndLV), LVM_SETFROZENSLOT, (WPARAM)(fFreezeOrUnfreeze), (LPARAM)(lpPt))

        Wh_Log(L"SetFrozenSlot");
        return ListView_SetFrozenSlot(_hwnd, fFreeze, ppt) ? S_OK : E_FAIL;
    }

    STDMETHODIMP GetViewMargin(LPRECT prc) noexcept override
    {

#define LVM_GETVIEWMARGINS (LVM_FIRST + 91)
#define ListView_GetViewMargins(hwndLV, lpRect)\
        (BOOL)SNDMSG((hwndLV), LVM_GETVIEWMARGINS, (WPARAM)(0), (LPARAM)(lpRect))

        Wh_Log(L"GetViewMargin");
        return ListView_GetViewMargins(_hwnd, prc) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetViewMargin(LPRECT prc) noexcept override
    {

#define LVM_SETVIEWMARGINS (LVM_FIRST + 90)
#define ListView_SetViewMargins(hwndLV, lpRect)\
        (BOOL)SNDMSG((hwndLV), LVM_SETVIEWMARGINS, (WPARAM)(0), (LPARAM)(lpRect))
    
        Wh_Log(L"SetViewMargin");
        return ListView_SetViewMargins(_hwnd, prc) ? S_OK : E_FAIL;
    }

    STDMETHODIMP SetKeyboardSelected(LVITEMINDEX iiItem) noexcept override
    {

#define LVM_KEYBOARDSELECTED    (LVM_FIRST + 178)
#define ListView_KeyboardSelected(hwnd, i) \
    (BOOL)SNDMSG((hwnd), LVM_KEYBOARDSELECTED, (WPARAM)(i), 0)

        Wh_Log(L"SetKeyboardSelected");
        return ListView_KeyboardSelected(_hwnd, iiItem.iItem) ? S_OK : E_FAIL;
    }

    STDMETHODIMP MapIndexToId(UINT iItem, int *piId) noexcept override
    {
        Wh_Log(L"MapIndexToId");
        return E_NOTIMPL;
    }

    STDMETHODIMP MapIdToIndex(UINT iId, int *piItem) noexcept override
    {
        Wh_Log(L"MapIdToIndex");
        return E_NOTIMPL;
    }

    STDMETHODIMP IsItemVisible(LVITEMINDEX iiItem, BOOL *pfVisible) noexcept override
    {
        Wh_Log(L"IsItemVisible");
        *pfVisible = ListView_IsItemVisible(_hwnd, iiItem.iItem);
        return S_OK;
    }

    STDMETHODIMP EnableAlphaShadow(bool fEnable) noexcept override
    {
        Wh_Log(L"EnableAlphaShadow");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetGroupSubsetCount(int *pcRows) noexcept override
    {
        Wh_Log(L"GetGroupSubsetCount");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetGroupSubsetCount(UINT cRows) noexcept override
    {
        Wh_Log(L"SetGroupSubsetCount");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetVisibleSlotCount(int *pcSlots) noexcept override
    {
        Wh_Log(L"GetVisibleSlotCount");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetColumnMargin(LPRECT prcMargin) noexcept override
    {
        Wh_Log(L"GetColumnMargin");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetSubItemCallback(ISubItemCallback *pcallback) noexcept override
    {
        Wh_Log(L"SetSubItemCallback");
        if (!pcallback)
            return E_INVALIDARG;
        if (!_plv)
            return E_FAIL;

        _plv->psiCallback = pcallback;
        pcallback->AddRef();
        return S_OK;
    }

    STDMETHODIMP GetVisibleItemRange(PLVITEMINDEX piiStart, PLVITEMINDEX piiEnd) noexcept override
    {
        Wh_Log(L"GetVisibleItemRange");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetTypeAheadFlags(TYPEAHEAD_FLAGS dwMask, TYPEAHEAD_FLAGS dwFlags) noexcept override
    {
        Wh_Log(L"SetTypeAheadFlags");
        return E_NOTIMPL;
    }
};

#endif // FEATURE_LIST_VIEWS

#pragma endregion // "IListView implementation"

#define DECLARE_HOOK_FUNCTION(RETURN_TYPE, ATTRIBUTES, NAME, ...) \
    RETURN_TYPE (ATTRIBUTES *NAME ## _orig)(__VA_ARGS__);         \
    RETURN_TYPE ATTRIBUTES NAME ## _hook(__VA_ARGS__)

// All of these initialization functions expect a valid WNDCLASS structure
// to be written to `pwc` on success, unlike XP's functions. If not, then 
// InitCommonControlsEx will fail. Because of that, we need to write a dummy
// class to be successfully registered.
#ifdef FEATURE_LIST_VIEWS


// Hijack header class
BOOL (__fastcall *Header_Init)(HINSTANCE hinst);

DECLARE_HOOK_FUNCTION(BOOL, __fastcall, InitHeaderClass,
    HINSTANCE   hinst,
    LPWNDCLASSW pwc)
{
    if (!Wh_GetIntSetting(L"list_views"))
        return InitHeaderClass_orig(hinst, pwc);

    Wh_Log(L"Registering header class");
    BOOL fRet = Header_Init(hinst);
    if (fRet)
    {
        ZeroMemory(pwc, sizeof(*pwc));
        pwc->lpfnWndProc = DefWindowProcW;
        pwc->hInstance = hinst;
        pwc->lpszClassName = L"WH_ClassicListViews_DummyHeaderClass";
    }
    return fRet;
}



// Hijack list view class
BOOL (__fastcall *ListView_Init)(HINSTANCE hinst);

DECLARE_HOOK_FUNCTION(BOOL, WINAPI, InitListViewClass,
    HINSTANCE   hinst,
    LPWNDCLASSW pwc)
{
    if (!Wh_GetIntSetting(L"list_views"))
        return InitListViewClass_orig(hinst, pwc);

    Wh_Log(L"Registering list view class");
    BOOL fRet = ListView_Init(hinst);
    if (fRet)
    {
        ZeroMemory(pwc, sizeof(*pwc));
        pwc->lpfnWndProc = DefWindowProcW;
        pwc->hInstance = hinst;
        pwc->lpszClassName = L"WH_ClassicListViews_DummyListViewClass";
    }
    return fRet;
}

#define LVM_QUERYINTERFACE    (LVM_FIRST+189)

#define LVGS_MASK           0x00000003
#define LVGA_ALIGN_MASK     0x0000002F

DECLARE_HOOK_FUNCTION(LRESULT, CALLBACK, ListView_WndProc,
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
        // Need this to support Explorer:
        case LVM_QUERYINTERFACE:
        {
            CListViewXP *plv = new CListViewXP(hwnd);
            if (!plv)
                return FALSE;

            if (SUCCEEDED(plv->QueryInterface(*(const IID *)wParam, (LPVOID *)lParam)))
            {
                return TRUE;
            }
            
            delete plv;
            return FALSE;
        }
        // Groups in Vista+ have certain flags that don't exist in XP, these
        // are immediately rejected by XP comctl32. Fix this by removing invalid
        // flags.
        case LVM_INSERTGROUP:
        {
            PLVGROUP plvgrp = (PLVGROUP)lParam;
            if (plvgrp)
            {
                Wh_Log(L"%X", plvgrp->state);
                if ((plvgrp->mask & LVGF_STATE)
                && (plvgrp->state & LVGS_MASK) != plvgrp->state)
                {
                    Wh_Log(L"found invalid state");

                    plvgrp->state &= LVGS_MASK;
                    Wh_Log(L"%X", plvgrp->state);
                }
            }
            LRESULT lRes = ListView_WndProc_orig(hwnd, uMsg, wParam, lParam);
            Wh_Log(L"%X", lRes);
            return lRes;
        }
        case LVM_INSERTITEMW:
        {
            Wh_Log(L"hi insertitem");
            break;
        }
    }

    return ListView_WndProc_orig(hwnd, uMsg, wParam, lParam);
}

ULONGLONG g_uComCtlXPBase = 0;
ULONGLONG g_uComCtlXPSize = 0;

// Allocate more bytes for extended listview struct
DECLARE_HOOK_FUNCTION(HLOCAL, WINAPI, LocalAlloc,
    UINT   uFlags,
    SIZE_T uBytes)
{
    ULONGLONG ulRetAddr = (ULONGLONG)__builtin_return_address(0);
    if (uBytes == sizeof(LV)
    && (ulRetAddr >= g_uComCtlXPBase) && (ulRetAddr < (g_uComCtlXPBase + g_uComCtlXPSize)))
    {
        uBytes = sizeof(LVEX);
    }
    return LocalAlloc_orig(uFlags, uBytes);
}

DECLARE_HOOK_FUNCTION(LRESULT, WINAPI, CCSendNotify,
    CCONTROLINFO *pci,
    int           code,
    LPNMHDR       pnmhdr)
{
    LRESULT lRes = CCSendNotify_orig(pci, code, pnmhdr);
    switch (code)
    {
        case (int)LVN_ODCACHEHINT:
        {
            LVEX *plv = (LVEX *)pci;
            if (plv->podCallback)
            {
                Wh_Log(L"Sending cache hint");
                NMLVCACHEHINT *pnmlv = (NMLVCACHEHINT *)pnmhdr;
                plv->podCallback->OnCacheHint(
                    { pnmlv->iFrom, 0 },
                    { pnmlv->iTo, 0 }
                );
            }
            break;
        }
        case (int)LVN_GETDISPINFOA:
        case (int)LVN_GETDISPINFOW:
        {
            Wh_Log(L"GETDISPINFO");
            LVEX *plv = (LVEX *)pci;
            if (plv->podCallback)
            {
                NMLVDISPINFOW *pnmlv = (NMLVDISPINFOW *)pnmhdr;
                Wh_Log(L"has callback, %X", pnmlv->item.mask & LVIF_GROUPID);
                if ((pnmlv->item.mask & LVIF_GROUPID)
                && pnmlv->item.iGroupId == I_GROUPIDCALLBACK)
                {
                    Wh_Log(L"Getting group");
                    plv->podCallback->GetItemGroup(
                        pnmlv->item.iItem,
                        0,
                        &pnmlv->item.iGroup
                    );
                }
            }
        }
    }
    return lRes;
}

#endif // FEATURE_LIST_VIEWS

// Hijack tree view class
BOOL (__fastcall *TV_Init)(HINSTANCE hinst);

DECLARE_HOOK_FUNCTION(BOOL, WINAPI, InitTreeViewClass,
    HINSTANCE   hinst,
    LPWNDCLASSW pwc)
{
    if (!Wh_GetIntSetting(L"tree_views"))
        return InitTreeViewClass_orig(hinst, pwc);

    Wh_Log(L"Registering tree view class");
    BOOL fRet = TV_Init(hinst);
    if (fRet)
    {
        ZeroMemory(pwc, sizeof(*pwc));
        pwc->lpfnWndProc = DefWindowProcW;
        pwc->hInstance = hinst;
        pwc->lpszClassName = L"WH_ClassicListViews_DummyTreeViewClass";
    }
    return fRet;
}

// Hijack tooltip class
BOOL (WINAPI *InitToolTipsClass_XP)(HINSTANCE hinst);

DECLARE_HOOK_FUNCTION(BOOL, WINAPI, InitToolTipsClass,
    HINSTANCE   hInstance,
    LPWNDCLASSW pwc)
{
    if (!Wh_GetIntSetting(L"tooltips"))
        return InitToolTipsClass_orig(hInstance, pwc);

    Wh_Log(L"Registering tooltip class");
    BOOL fRet = InitToolTipsClass_XP(hInstance);
    if (fRet)
    {
        ZeroMemory(pwc, sizeof(*pwc));
        pwc->lpfnWndProc = DefWindowProcW;
        pwc->hInstance = hInstance;
        pwc->lpszClassName = L"WH_ClassicListViews_DummyToolTipClass";
    }
    return fRet;
}

#pragma region "Tooltip slide animation fix"

typedef DWORD   BITBOOL;

typedef struct tagControlInfo 
{
    HWND        hwnd;
    HWND        hwndParent;
    DWORD       style;
    DWORD       dwCustom;
    BITBOOL     bUnicode : 1;
    BITBOOL     bInFakeCustomDraw:1;
    BITBOOL     fDPIAware:1;
    UINT        uiCodePage;
    DWORD       dwExStyle;
    LRESULT     iVersion;
    WORD        wUIState;
} CCONTROLINFO, *LPCCONTROLINFO;

class CToolTipsMgr
{
public:
    void *__vftable;
    CCONTROLINFO _ci;
    LONG _cRef;
    int iNumTools;
    int iDelayTime;
    int iReshowTime;
    int iAutoPopTime;
    PTOOLINFO tools;
    TOOLINFO *pCurTool;
    BOOL fMyFont;
    HFONT hFont;
    HFONT hFontUnderline;
    DWORD dwFlags;

    // Timer info;
    UINT_PTR idTimer;
    POINT pt;

    UINT_PTR idtAutoPop;

    // Tip title buffer
    LPTSTR lpTipTitle;
    UINT   cchTipTitle; 
    UINT   uTitleBitmap;
    int    iTitleHeight;
    HIMAGELIST himlTitleBitmaps;

    POINT ptTrack; // the saved track point from TTM_TRACKPOSITION

    BOOL fBkColorSet :1;
    BOOL fTextColorSet :1;
    BOOL fUnderStem : 1;        // true if stem is under the balloon
    BOOL fInWindowFromPoint:1;  // handling a TTM_WINDOWFROMPOINT message
    BOOL fEverShown:1;          // Have we ever been shown before?
    COLORREF clrTipBk;          // This is joeb's idea...he wants it
    COLORREF clrTipText;        // to be able to _blend_ more, so...
    
    int  iMaxTipWidth;          // the maximum tip width
    RECT rcMargin;              // margin offset b/t border and text
    int  iStemHeight;           // balloon mode stem/wedge height
    DWORD dwLastDisplayTime;    // The tick count taken at the last display. Used for animate puroposes.
};

#define TTT_SLIDE           0x7569736B
#define CMS_TOOLTIP         135
#define TIMEBETWEENANIMATE  2000

thread_local bool g_fTooltipWasShown  = false;
thread_local bool g_fAnimatingTooltip = false;

WNDPROC ToolTipsWndProc_orig;
LRESULT CALLBACK ToolTipsWndProc_hook(
    HWND          hwnd, 
    UINT          uMsg,
    WPARAM        wParam,
    LPARAM        lParam
)
{
    CToolTipsMgr *pThis = (CToolTipsMgr *)GetWindowLongPtrW(hwnd, 0);
    if (!pThis)
        return ToolTipsWndProc_orig(hwnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
        case WM_WINDOWPOSCHANGING:
        {
            LPWINDOWPOS pwp = (LPWINDOWPOS)lParam;
            BOOL fAnimate = FALSE, fFade = FALSE;
            SystemParametersInfoW(SPI_GETTOOLTIPANIMATION, 0, &fAnimate, 0);
            SystemParametersInfoW(SPI_GETTOOLTIPFADE, 0, &fFade, 0);
            if (fAnimate && !fFade
            && !g_fAnimatingTooltip && (pwp->flags & SWP_SHOWWINDOW))
            {
                g_fTooltipWasShown = true;
                pwp->flags &= ~SWP_SHOWWINDOW;
            }
            goto DoDefault;
        }
        default:
        DoDefault:
            return ToolTipsWndProc_orig(hwnd, uMsg, wParam, lParam);
    }
}

void (__stdcall *DoShowBubble_orig)(CToolTipsMgr *);
void __stdcall DoShowBubble_hook(
    CToolTipsMgr *pTtm
)
{
    DWORD dwLastDisplayTime = pTtm->dwLastDisplayTime;
    DoShowBubble_orig(pTtm);
    BOOL fAnimate = FALSE, fFade = FALSE;
    SystemParametersInfoW(SPI_GETTOOLTIPANIMATION, 0, &fAnimate, 0);
    SystemParametersInfoW(SPI_GETTOOLTIPFADE, 0, &fFade, 0);
    // If the tooltip isn't left visible from the original function,
    // bail out.
    if (g_fTooltipWasShown && fAnimate && !fFade)
    {
        g_fTooltipWasShown = false;
        
        HWND hwnd = pTtm->_ci.hwnd;
        
        DWORD dwCurrentTime = (dwLastDisplayTime == 0)
            ? TIMEBETWEENANIMATE
            : GetTickCount();
        DWORD dwDelta = dwCurrentTime - dwLastDisplayTime;
        if ((pTtm->_ci.style & (TTS_BALLOON | TTS_NOANIMATE)) || dwDelta < TIMEBETWEENANIMATE)
        {
            g_fAnimatingTooltip = true;
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            g_fAnimatingTooltip = false;
            return;
        }

        ShowWindow(hwnd, SW_HIDE);
        
        RECT rc;
        GetWindowRect(hwnd, &rc);

        DWORD dwPos = GetMessagePos();
        DWORD dwFlags;
        if (GET_Y_LPARAM(dwPos) > rc.top + (rc.bottom - rc.top) / 2)
        {
            dwFlags = AW_VER_NEGATIVE;
        }
        else
        {
            dwFlags = AW_VER_POSITIVE;
        }

        g_fAnimatingTooltip = true;
        AnimateWindow(hwnd, CMS_TOOLTIP, dwFlags | AW_SLIDE);
        g_fAnimatingTooltip = false;
    }
}

#pragma endregion // "Tooltip slide animation fix"

VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
{ 
    void *pFixedFileInfo = nullptr; 
    UINT uPtrLen = 0; 

    HRSRC hResource = 
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION); 
    if (hResource)
    { 
        HGLOBAL hGlobal = LoadResource(hModule, hResource); 
        if (hGlobal)
        { 
            void *pData = LockResource(hGlobal); 
            if (pData)
            { 
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                { 
                    pFixedFileInfo = nullptr; 
                    uPtrLen = 0; 
                } 
            } 
        } 
    } 

    if (puPtrLen)
    { 
        *puPtrLen = uPtrLen; 
    } 
  
     return (VS_FIXEDFILEINFO *)pFixedFileInfo; 
 } 

/**
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  */
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

#ifdef _WIN64
#   define STDCALL_STR L"__cdecl"
#else
#   define STDCALL_STR L"__stdcall"
#endif

// comctl32.dll, Windows XP edition
const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks_XP[] = {
#ifdef FEATURE_LIST_VIEWS
    {
        {
#ifdef _WIN64
            L"Header_Init"
#else
            L"_Header_Init@4"
#endif
        },
        &Header_Init,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"ListView_Init"
#else
            L"_ListView_Init@4"
#endif
        },
        &ListView_Init,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"ListView_WndProc"
#else
            L"_ListView_WndProc@16"
#endif
        },
        &ListView_WndProc_orig,
        ListView_WndProc_hook,
        false
    },
#endif // FEATURE_LIST_VIEWS
    {
        {
#ifdef _WIN64
            L"TV_Init"
#else
            L"_TV_Init@4"
#endif
        },
        &TV_Init,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"InitToolTipsClass"
#else
            L"_InitToolTipsClass@4"
#endif
        },
        &InitToolTipsClass_XP,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"__int64 __cdecl ToolTipsWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
#else
            L"long __stdcall ToolTipsWndProc(struct HWND__ *,unsigned int,unsigned int,long)"
#endif
        },
        &ToolTipsWndProc_orig,
        ToolTipsWndProc_hook,
        false
    },
    {
        {
            L"void " STDCALL_STR L" DoShowBubble(class CToolTipsMgr *)"
        },
        &DoShowBubble_orig,
        DoShowBubble_hook,
        false
    }
#ifdef FEATURE_LIST_VIEWS
    {
        {
#ifdef _WIN64
            L"CCSendNotify"
#else
            L"_CCSendNotify@12"
#endif
        },
        &CCSendNotify_orig,
        CCSendNotify_hook,
        false
    },
#endif // FEATURE_LIST_VIEWS
};

const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
#ifdef FEATURE_LIST_VIEWS
    {
        {
#ifdef _WIN64
            L"InitHeaderClass"
#else
            L"_InitHeaderClass@8"
#endif
        },
        &InitHeaderClass_orig,
        InitHeaderClass_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"InitListViewClass"
#else
            L"_InitListViewClass@8"
#endif
        },
        &InitListViewClass_orig,
        InitListViewClass_hook,
        false
    },
#endif // FEATURE_LIST_VIEWS
    {
        {
#ifdef _WIN64
            L"InitTreeViewClass"
#else
            L"_InitTreeViewClass@8"
#endif
        },
        &InitTreeViewClass_orig,
        InitTreeViewClass_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"InitToolTipsClass"
#else
            L"_InitToolTipsClass@8"
#endif
        },
        &InitToolTipsClass_orig,
        InitToolTipsClass_hook,
        false
    },
};

BOOL Wh_ModInit(void)
{
#ifdef _WIN64 
    LPCWSTR c_szPathSetting = L"comctl32_64_path";
#else
    LPCWSTR c_szPathSetting = L"comctl32_32_path";
#endif

    HMODULE hComCtl;
    LPCWSTR pszComCtlPath;
    WCHAR szExpandedPath[MAX_PATH];

    // XP comctl32 has shunimpl'd functions that don't get used by us.
    // Make this atom to just allow the stubs to load.
    ATOM atomShunimpl = AddAtomW(L"FailObsoleteShellAPIs");
    if (!atomShunimpl)
    {
        Wh_Log(L"Failed to create SHUNIMPL atom");
        goto Fail;
    }

    pszComCtlPath = Wh_GetStringSetting(c_szPathSetting);
    ExpandEnvironmentStringsW(pszComCtlPath, szExpandedPath, ARRAYSIZE(szExpandedPath));
    Wh_FreeStringSetting(pszComCtlPath);

    g_hinstComCtlXP = LoadLibraryW(szExpandedPath);
    if (!g_hinstComCtlXP)
    {
        Wh_Log(L"Failed to load the XP comctl32.dll");
        goto Fail;
    }

#ifdef FEATURE_LIST_VIEWS
    MODULEINFO mi;
    GetModuleInformation(
        GetCurrentProcess(),
        g_hinstComCtlXP,
        &mi,
        sizeof(mi));
    g_uComCtlXPBase = (ULONGLONG)mi.lpBaseOfDll;
    g_uComCtlXPSize = (ULONGLONG)mi.SizeOfImage;
#endif // FEATURE_LIST_VIEWS

    DeleteAtom(atomShunimpl);

    if (!WindhawkUtils::HookSymbols(
        g_hinstComCtlXP,
        comctl32DllHooks_XP,
        ARRAYSIZE(comctl32DllHooks_XP)
    ))
    {
        Wh_Log(L"Failed to find one or more symbol functions in the XP comctl32.dll");
        goto Fail;
    }

    hComCtl = LoadComCtlModule();
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load system comctl32.dll");
        goto Fail;
    }

    if (!WindhawkUtils::HookSymbols(
        hComCtl,
        comctl32DllHooks,
        ARRAYSIZE(comctl32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in system comctl32.dll");
        return FALSE;
    }

#ifdef FEATURE_LIST_VIEWS
    if (!Wh_SetFunctionHook(
        (void *)LocalAlloc,
        (void *)LocalAlloc_hook,
        (void **)&LocalAlloc_orig
    ))
    {
        Wh_Log(L"Failed to hook LocalAlloc");
        return FALSE;
    }
#endif // FEATURE_LIST_VIEWS

    return TRUE;

Fail:
    if (g_hinstComCtlXP)
        FreeLibrary(g_hinstComCtlXP);
    
    return FALSE;
}