// ==WindhawkMod==
// @id              classic-explorer-treeview
// @name            Classic Explorer Treeview
// @description     Modifies Folder Treeview in file explorer so as to make it look more classic.
// @version         1.1
// @author          Waldemar
// @github          https://github.com/CyprinusCarpio
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -loleaut32 -lole32 -lshlwapi
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- FoldersText: default
  $name: Folders Pane Text
  $description: If you want a custom text to be displayed on the folders pane, change this text. 'default' will source the text from localized system resources.
- DrawLines: true
  $name: Draw Dotted Lines
  $description: Use TVS_HASLINES style. Disable for a more XP-like look.
- LinesAtRoot: false
  $name: Lines At Root
  $description: Use TVS_LINESATROOT style.
- DrawButtons: true
  $name: Draw +/- Buttons
  $description: Should the mod draw it's own +/- buttons. Disable only if not using Classic theme.
- AlternateLineColor: Automatic
  $name: Alternate line color
  $description: Use highlight color instead of shadow color for drawing lines. May produce better results on some dark themes.
  $options:
    - False: Use default color.
    - True: Use alternate color.
    - Automatic: Use alternate color if the default color's contrast is too low.
- CloseButtonXOffset: 0
  $name: Horizontal close button offset
- CloseButtonYOffset: 0
  $name: Vertical close button offset
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Classic Explorer Treeview

This mod changes the Explorer Treeview (otherwise known as Folder Pane or
Navigation Pane) to look more classic. It draws it's own Folder Band with a
functional X button. If you are using OpenShell, then you need to set the
correct settings for Classic Explorer in the Navigation Pane tab: Vista style
and Tree item spacing 0.

This mod supports the Classic and Basic (non DWM) themes, others may work but
aren't supported.

If the mod is to be disabled, it's recommended not to use file explorer windows that
were opened prior to disabling it, as it may lead to undefined behaviour.

![BeforeAndAfter](https://i.imgur.com/ZcfJz1T.png)

For issue reports, contact waldemar3194 on Discord, or file a report at my [github repository](https://github.com/CyprinusCarpio/windhawk-mods).


# Changelog:
## 1.1
- Enabling the folders pane will now hide real InfoBands and vice versa
- Fixed a hidden bug where clicking the X button many times would hang Explorer
- Added a working entry in the Explorer Bars submenu
- Removed 32bit "support"

## 1.0.3
- Optimized expando button drawing
- Mod will now explicitly load the required dll module

## 1.0.2
- Accurate dotted lines
- Fixed a bug with quick access item being too big

## 1.0.1
- More accurate treeview item spacing
- Mod no longer requires different settings on Win10 and Win11
- Added a preview image to the description
- Added settings to set the X, Y offsets of the close button

## 1.0
- Fixed explorer.exe instability
- Removed redundant linker arguments
- Removed redundant includes
- Added a option to use a default, system localized text for the folder band
- Moving the treeview on the Y axis will no longer be attempted
- Fixed the tooltips displayed when hovering over a item that doesn't fit in the treeview window
- Added many comments to the code

## 0.5.1
- Hotfix: fixed a graphical bug that led to spurious expando buttons being drawn

## 0.5
- Changed the Folders band text font to be the same as in a real InfoBand
- Folders band is now scaled semi-accurately when a non-standard sized theme is
applied
- Fixed the gaps to the sides of the folder band
- Fixed some memory leaks
- Fixed scrollbar buttons not appearing pressed in

## 0.4
- Changed raw COM pointers to winrt::com_ptr
- Changed the way Folders band text is stored in the mod
- Added a option to automatically set the line color based on the currently used
theme
- Changed the mod to only subclass CNscTree controls that are part of file
explorer windows, disregarding others that may be created in the explorer.exe
process
- Accurate mouse pointer will be displayed when hovering over tree view items

## 0.3
- More accurate positioning and sizing of the folder band (Cynosphere)
- Proper close button "glyph" (Cynosphere)
- Close button glyph will now appear pressed in like a proper button label
- Fixed a bug with close button appearing pressed in on mouse hover after it was
pressed, but the mouse pointer left the window area before the mouse button was
released
- Fixed a bug with incorrect cursor being sometimes displayed when hovering over
the folder band
- Draw bottom rebar line on the folder band (Cynosphere)
- Add option to use highlight color for drawing lines for better visibility on
dark themes (Cynosphere)

## 0.2
- Fixed a bug with drawing lines when LinesAtRoot is enabled.
- Added a 3D border to the X button when mouse is hovering over it.
- Added a option not to draw +/- buttons so that the mod can be used when themes
are enabled.
- Fixed the treeview being drawn over in certain circumstances.

## 0.1
- Initial release.
*/
// ==/WindhawkModReadme==

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#define CSB_PTHIS_OFFSET 16
#define TV_MENUITEM_ID 2121420

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <wingdi.h>
#include <winrt/base.h>
#include <shlwapi.h>
#include <shdeprecated.h>

// Mod settings
bool g_settingDrawDottedLines = true;
bool g_settingLinesAtRoot = false;
bool g_settingDrawButtons = true;
WindhawkUtils::StringSetting g_settingLineColorOption;
WindhawkUtils::StringSetting g_settingFoldersPaneText;
int g_settingCloseButtonXOffset = 0;
int g_settingCloseButtonYOffset = 0;

int g_lineColorOptionInt = 2;
wchar_t g_defaultFoldersPaneText[32];
wchar_t g_menuTreeviewText[32];

std::vector<unsigned int> g_knownBrowserBandIds;

enum FEWExtraFlags
{
    MOUSE_OVER_BUTTON = 1 << 0,
    MOUSE_PRESSED = 1 << 1,
    MOUSE_TRACING = 1 << 2,
    COM_POINTERS = 1 << 3,
    TREEVIEW_VIS = 1 << 4
};

class FEWExtra //File Explorer Window Extra things
{
    winrt::com_ptr<IShellBrowser> pShellBrowser;
    winrt::com_ptr<IPropertyBag> pPropertyBag;
    //winrt::com_ptr<IWebBrowser2> pWebBrowser;
public:
    HWND hSTWC = nullptr; //ShellTabWindowClass
    HWND hCNS = nullptr; //CtrlNotifySink
    HWND hNSTC = nullptr; //NamespaceTreeControl
    HWND hTV = nullptr; //Treeview
    int eFlags = 0;
    void* csb = nullptr; //Ptr to 'this' of the CShellBrowser

    FEWExtra(HWND hS, IShellBrowser* psb)
    {
        hSTWC = hS;
        pShellBrowser.copy_from(psb);
        csb = pShellBrowser.get();
        csb = static_cast<char *>(csb) - CSB_PTHIS_OFFSET;
    }

    void populateCOMPointers()
    {
        static GUID SID_FrameManager =
        {
            0x31e4fa78,
            0x02b4,
            0x419f,
            {0x94, 0x30, 0x7b, 0x75, 0x85, 0x23, 0x7c, 0x77}
        };
        static GUID SID_PerBrowserPropertyBag =
        {
            0xa3b24a0a,
            0x7b68,
            0x448d,
            {0x99, 0x79, 0xc7, 0x00, 0x05, 0x9c, 0x3a, 0xd1}
        };
        /*static GUID SID_IWebBrowserApp =
        {
            0x0002df05,
            0x0000,
            0x0000, 
            {0xc0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46}
        };*/

        winrt::com_ptr<IUnknown> frame;
        pShellBrowser.as<IServiceProvider>()->QueryService(SID_FrameManager, IID_PPV_ARGS(&frame));
        frame.as<IServiceProvider>()->QueryService(SID_PerBrowserPropertyBag, IID_PPV_ARGS(&pPropertyBag));
        //pShellBrowser.as<IServiceProvider>()->QueryService(SID_IWebBrowserApp, IID_PPV_ARGS(&pWebBrowser));

        eFlags |= COM_POINTERS;
    }

    void SetTreeviewVisible(bool visible)
    {
        if((eFlags & COM_POINTERS) == 0)
        {
            populateCOMPointers();
        }
        VARIANT val;
        VariantInit(&val);
        val.vt = VT_BOOL;
        val.boolVal = visible ? VARIANT_TRUE : VARIANT_FALSE;
        pPropertyBag->Write(L"PageSpaceControlSizer_Visible", &val);
    }

    void ModifyToolbarMenu(HMENU hMenu)
    {
        // The menu entry disappears on navigation to a new folder, so we need to check
        // if it still exists.
        if(!hMenu) return;
        size_t itemIndex = GetMenuItemCount(hMenu);
        for (size_t i = 0; i < itemIndex; i++)
        {
            UINT itemID = GetMenuItemID(hMenu, i);
            if (itemID == TV_MENUITEM_ID)
            {
                CheckMenuItem(hMenu, TV_MENUITEM_ID, (eFlags & TREEVIEW_VIS) ? MF_CHECKED : MF_UNCHECKED);
                return;
            }
        }
        AppendMenu(hMenu, MF_STRING | (eFlags & TREEVIEW_VIS) ? MF_CHECKED : MF_UNCHECKED, TV_MENUITEM_ID, g_menuTreeviewText);
    }
};

//Used to keep track of the extra data assigned to each file explorer window
std::vector<FEWExtra> g_FEWExtras;

// Euclidean color distance
double ColorDistance(DWORD color1, DWORD color2)
{
    int b1 = (color1 & 0xFF);
    int g1 = ((color1 >> 8) & 0xFF);
    int r1 = ((color1 >> 16) & 0xFF);

    int b2 = (color2 & 0xFF);
    int g2 = ((color2 >> 8) & 0xFF);
    int r2 = ((color2 >> 16) & 0xFF);

    return std::sqrt(std::pow(r2 - r1, 2) + std::pow(g2 - g1, 2) + std::pow(b2 - b1, 2));
}

void DrawExpandoButton(HWND hTree, HDC hdc, HTREEITEM hItem)
{
    RECT rect;
    TreeView_GetItemRect(hTree, hItem, &rect, TRUE);

    // Get system colors
    bool useHighlight = false;
    DWORD windowColor = GetSysColor(COLOR_WINDOW);
    DWORD shadowColor = GetSysColor(COLOR_3DSHADOW);
    DWORD highlightColor = GetSysColor(COLOR_3DHIGHLIGHT);

    // Check the line color option and set the useHighlight flag accordingly
    if (g_lineColorOptionInt == 1)
    {
        useHighlight = true;
    }
    else if (g_lineColorOptionInt == 2)
    {
        // Calculate the color distances
        double distanceShadow = ColorDistance(windowColor, shadowColor);
        double distanceHighlight = ColorDistance(windowColor, highlightColor);

        // Determine whether to use highlight based on color distances
        if (distanceHighlight > distanceShadow)
        {
            useHighlight = true;
        }
    }

    // Even though the item height should be 16, I'm accomodating some more heights
    UINT itemHeight = TreeView_GetItemHeight(hTree);
    switch(itemHeight)
    {
    default:
    case 16:
        rect.left -= 34;
        rect.top += 4;
        break;
    case 18:
        rect.left -= 33;
        rect.top += 6;
        break;
    }

    // Define a TVITEM struct to retrieve the number of children for the specified item
    TVITEM tvi;
    tvi.mask = TVIF_CHILDREN; // Specify that we are interested in the number of children
    tvi.hItem = hItem; // Set the handle of the tree view item

    // Check if the item is at the top level by comparing the left position of the rectangle
    // to either 10 or -10 based on the value of g_settingLinesAtRoot
    bool isTopLevel = rect.left <= (g_settingLinesAtRoot ? 10 : -10);

    // Check if the setting to draw buttons is enabled and the node is not a top-level node when lines at root is disabled
    // and the node has one or more children
    if (!(isTopLevel && !g_settingLinesAtRoot) && TreeView_GetItem(hTree, &tvi) && tvi.cChildren > 0)
    {
        // Get the state of the item
        UINT state = TreeView_GetItemState(hTree, hItem, TVIS_EXPANDED);

        // Create a solid brush with the window color and select it into the device context
        HBRUSH brush = CreateSolidBrush(windowColor);
        HBRUSH original = (HBRUSH)SelectObject(hdc, brush);

        // Define the rectangle for the button
        RECT buttonRect;
        buttonRect.left = rect.left;
        buttonRect.top = rect.top;
        buttonRect.right = rect.left + 9;
        buttonRect.bottom = rect.top + 9;

        // Fill the rectangle with the brush color and delete the brush
        FillRect(hdc, &buttonRect, brush);
        DeleteObject(brush);

        // Create a new brush with the highlight color if useHighlight is true, otherwise shadowColor
        brush = CreateSolidBrush(useHighlight ? highlightColor : shadowColor);

        // Draw a frame around the button rectangle with the new brush and delete the brush
        FrameRect(hdc, &buttonRect, brush);
        DeleteObject(brush);

        // Restore the original brush to the device context
        SelectObject(hdc, original);

        // Create a pen with a solid style and the button text color, and select it into the device context
        HPEN hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        // Draw a +/- symbol in the item's button based on its expanded state
        if (state & TVIS_EXPANDED)
        {
            MoveToEx(hdc, rect.left + 2, rect.top + 4, NULL);
            LineTo(hdc, rect.left + 7, rect.top + 4);
        }
        else
        {
            MoveToEx(hdc, rect.left + 2, rect.top + 4, NULL);
            LineTo(hdc, rect.left + 7, rect.top + 4);
            MoveToEx(hdc, rect.left + 4, rect.top + 2, NULL);
            LineTo(hdc, rect.left + 4, rect.top + 7);
        }

        // Restore the original pen to the device context and delete the pen
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
}

int GetThemedBandOffset()  // extremely retarded and only semi-accurate
{
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),
                         &ncm, 0);
    int size = std::round(abs(ncm.lfMenuFont.lfHeight) / 1.21f - 1.21f);
    if (size < 7)
        size++;
    if (size > 16)
        size--;
    if (size > 11)
        size += (size - 11);
    size -= 8;
    return size;
}

void SetCloseButtonRect(RECT &rect)
{
    rect.left = rect.right - 24 + g_settingCloseButtonXOffset;
    rect.right -= 4 - g_settingCloseButtonXOffset;
    rect.bottom = rect.top + 19 + g_settingCloseButtonYOffset;
    rect.top += 2 + g_settingCloseButtonYOffset;
}

typedef void (*__cdecl CSBSetBrowserBarState_t)(void*, unsigned int, void*, int, void*);
CSBSetBrowserBarState_t CSBSetBrowserBarStateOriginal;
void __cdecl CSBSetBrowserBarStateHook(void* pThis, unsigned int var1, void* var2, int var3, void* var4)
{
    // This function gets called when a InfoBand is about to be shown or hidden.
    // var 3: -1 = toggle, 0 = off, 1 = on
    // We need to keep track of the band ids, so that they can be hidden when the mod treeview is shown.

    if(std::find(g_knownBrowserBandIds.begin(), g_knownBrowserBandIds.end(), var1) == g_knownBrowserBandIds.end())
    {
        g_knownBrowserBandIds.push_back(var1);
    }
    if(var3 == -1 || var3 == 1)
    {
        for(FEWExtra& f : g_FEWExtras)
        {
            if(f.csb == pThis)
            {
                f.SetTreeviewVisible(false);
                break;
            }
        }
    }

    CSBSetBrowserBarStateOriginal(pThis, var1, var2, var3, var4);
}

LRESULT CALLBACK CtrlNotSinkSubclassProc(_In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _In_ DWORD_PTR dwRefData)
{
    if(uMsg == WM_SHOWWINDOW) //Mod treeview is hidden/shown
    {
        HWND stwc = GetParent(GetParent(GetParent(hWnd)));
        for(FEWExtra& f : g_FEWExtras)
        {
            if(f.hSTWC == stwc)
            {
                if(wParam) //wParam - bool for hide/show
                {
                    f.eFlags |= TREEVIEW_VIS;
                    // Hide every known browser band
                    for(unsigned int id : g_knownBrowserBandIds)
                        CSBSetBrowserBarStateHook(f.csb, id, NULL, 0, NULL);
                }
                else
                    f.eFlags &= ~TREEVIEW_VIS;
                break;
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NTCSubclassProc(_In_ HWND hWnd,
                                 _In_ UINT uMsg,
                                 _In_ WPARAM wParam,
                                 _In_ LPARAM lParam,
                                 _In_ DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_NOTIFY:
    {
        if(!g_settingDrawButtons)
            break;

        LPNMHDR lpnmh = (LPNMHDR)lParam;
        switch (lpnmh->code)
        {
        case NM_CUSTOMDRAW: // Handle custom drawing notifications sent by the treeview control
        {
            HWND hTree = FindWindowEx(hWnd, NULL, L"SysTreeView32", NULL); // Find the treeview control
            LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)lParam;
            if(pCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
            {
                return CDRF_NOTIFYITEMDRAW;
            }
            if(pCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
            {
                return CDRF_NOTIFYPOSTPAINT;
            }
            if(pCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
            {
                HTREEITEM hItem = reinterpret_cast<HTREEITEM>(pCustomDraw->nmcd.dwItemSpec);
                DrawExpandoButton(hTree, pCustomDraw->nmcd.hdc, hItem);
                return S_OK;
            }
        }
        break;
        case TVN_ITEMEXPANDED: //If a item is expanded, redraw the treeview to fix graphical issues
        {
            HWND hwndTreeView = lpnmh->hwndFrom;
            InvalidateRect(hwndTreeView, NULL, TRUE);
            return S_OK;
        }
        break;
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        HBRUSH original = (HBRUSH)SelectObject(hdc, brush);

        // Get the themed band offset and calculate the rectangle for the background, and draw it
        int themeBandOffset = GetThemedBandOffset();
        RECT rect;
        GetClientRect(hWnd, &rect);
        long origBottom = rect.bottom;
        rect.bottom = rect.top + 22 + themeBandOffset;
        FillRect(hdc, &rect, brush);
        SelectObject(hdc, original);
        DeleteObject(brush);
        long origTop = rect.top;

        // Create and fill the top line of the rebar
        HBRUSH rebarLineTop = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
        rect.top = origTop + 22 + themeBandOffset;
        rect.bottom = rect.top + 1;
        FillRect(hdc, &rect, rebarLineTop);
        DeleteObject(rebarLineTop);

        // Create and fill the bottom line of the rebar
        HBRUSH rebarLineBottom = CreateSolidBrush(GetSysColor(COLOR_3DHIGHLIGHT));
        rect.top += 1;
        rect.bottom += 1;
        FillRect(hdc, &rect, rebarLineBottom);
        DeleteObject(rebarLineBottom);

        long origLeft = rect.left;
        long origRight = rect.right;

        //Get flags for this NSTC
        int eFlags = 0;
        size_t s = g_FEWExtras.size();
        for (size_t i = 0; i < s; i++)
        {
            if (hWnd == g_FEWExtras[i].hNSTC)
            {
                eFlags = g_FEWExtras[i].eFlags;
                break;
            }
        }

        // Create a brush for the close button
        HBRUSH closeColor = CreateSolidBrush(GetSysColor(COLOR_BTNTEXT));

        // draw the close button
        // ##    ##
        rect.top = origTop + 7 + g_settingCloseButtonYOffset;
        rect.left = origRight - 18 + g_settingCloseButtonXOffset;
        rect.bottom = rect.top + 1;
        rect.right = rect.left + 2;
        if (eFlags & MOUSE_OVER_BUTTON && eFlags & MOUSE_PRESSED)
        {
            rect.left += 1;
            rect.right += 1;
            rect.top += 1;
            rect.bottom += 1;
        }
        FillRect(hdc, &rect, closeColor);

        rect.left += 6;
        rect.right += 6;
        FillRect(hdc, &rect, closeColor);

        //  ##  ##
        rect.top += 1;
        rect.bottom += 1;
        rect.left -= 1;
        rect.right -= 1;
        FillRect(hdc, &rect, closeColor);

        rect.left -= 4;
        rect.right -= 4;
        FillRect(hdc, &rect, closeColor);

        //   ####
        rect.top += 1;
        rect.bottom += 1;
        rect.left += 1;
        rect.right += 3;
        FillRect(hdc, &rect, closeColor);

        //    ##
        rect.top += 1;
        rect.bottom += 1;
        rect.left += 1;
        rect.right -= 1;
        FillRect(hdc, &rect, closeColor);

        // reset and draw last three lines in reverse cause easier math
        // ##    ##
        rect.top = origTop + 13 + g_settingCloseButtonYOffset;
        rect.left = origRight - 18 + g_settingCloseButtonXOffset;
        rect.bottom = rect.top + 1;
        rect.right = rect.left + 2;
        if (eFlags & MOUSE_OVER_BUTTON && eFlags & MOUSE_PRESSED)
        {
            rect.left += 1;
            rect.right += 1;
            rect.top += 1;
            rect.bottom += 1;
        }
        FillRect(hdc, &rect, closeColor);

        rect.left += 6;
        rect.right += 6;
        FillRect(hdc, &rect, closeColor);

        //  ##  ##
        rect.top -= 1;
        rect.bottom -= 1;
        rect.left -= 1;
        rect.right -= 1;
        FillRect(hdc, &rect, closeColor);

        rect.left -= 4;
        rect.right -= 4;
        FillRect(hdc, &rect, closeColor);

        //   ####
        rect.top -= 1;
        rect.bottom -= 1;
        rect.left += 1;
        rect.right += 3;
        FillRect(hdc, &rect, closeColor);

        DeleteObject(closeColor);

        rect.top = origTop;
        rect.bottom = origBottom;
        rect.left = origLeft;
        rect.right = origRight;

        SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

        //Draw the etched edge around the NSTC
        DrawEdge(hdc, &rect, EDGE_ETCHED, BF_RECT);

        // If the mouse is over the button, draw a raised inner border over the close button
        if (eFlags & MOUSE_OVER_BUTTON)
        {
            RECT buttonFrame;
            GetClientRect(hWnd, &buttonFrame);
            SetCloseButtonRect(buttonFrame);
            DrawEdge(
                hdc, &buttonFrame,
                eFlags & MOUSE_PRESSED ? BDR_SUNKENOUTER : BDR_RAISEDINNER,
                BF_RECT);
        }
        SelectObject(hdc, original);

        // Get the font and draw the folder pane text
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(NONCLIENTMETRICS);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

        HFONT hFont = CreateFontIndirect(&ncm.lfMenuFont);
        HFONT hFontOrig = (HFONT)SelectObject(hdc, hFont);
        SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

        PCWSTR foldersPaneText = g_settingFoldersPaneText.get();
        unsigned int foldersPaneTextLength;
        if (lstrcmpW(foldersPaneText, L"default") == 0)
        {
            foldersPaneTextLength = wcslen(g_defaultFoldersPaneText);
            TextOut(hdc, 8, 5, g_defaultFoldersPaneText,
                    foldersPaneTextLength);
        }
        else
        {
            foldersPaneTextLength = wcslen(foldersPaneText);
            TextOut(hdc, 8, 5, foldersPaneText, foldersPaneTextLength);
        }

        SelectObject(hdc, hFontOrig);
        DeleteObject(hFont);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SETCURSOR:
    {
        //Set the cursor to the default cursor, because this window does not handle this message by default
        //so sometimes the wrong cursor would be displayed
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return TRUE;
    }
    break;
    case WM_MOUSEMOVE:
    {
        // Get the x and y coordinates of the mouse
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        RECT rect;
        GetClientRect(hWnd, &rect);

        // Set the rect to the X button bounds
        SetCloseButtonRect(rect);

        // Check if the mouse is inside the rectangle
        bool mouseInside = false;
        if (xPos >= rect.left && xPos <= rect.right && yPos >= rect.top &&
                yPos <= rect.bottom)
        {
            mouseInside = true;
        }

        // Iterate through the list of subclassed NSTCs
        size_t s = g_FEWExtras.size();
        for (size_t i = 0; i < s; i++)
        {
            // Check if the current window handle matches the one in the list
            if (hWnd == g_FEWExtras[i].hNSTC)
            {
                // If mouse is inside, set the flag, else clear it. Redraw the window
                if (mouseInside)
                {
                    InvalidateRect(hWnd, &rect, FALSE);
                    g_FEWExtras[i].eFlags |= MOUSE_OVER_BUTTON;
                }
                else if (g_FEWExtras[i].eFlags & MOUSE_OVER_BUTTON)
                {
                    g_FEWExtras[i].eFlags &= ~MOUSE_OVER_BUTTON;
                    InvalidateRect(hWnd, &rect, FALSE);
                }
                // If the mouse is not being traced, trace it so that we know when it leaves the window
                // so that we can stop drawing the raised button edge
                if (!(g_FEWExtras[i].eFlags & MOUSE_TRACING))
                {
                    g_FEWExtras[i].eFlags |= MOUSE_TRACING;
                    TRACKMOUSEEVENT tme{};
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hWnd;
                    TrackMouseEvent(&tme);
                }
                break;
            }
        }
    }
    break;
    case WM_LBUTTONDOWN: // If the left mouse button is pressed, set the flag and redraw
    {
        size_t s = g_FEWExtras.size();
        for (size_t i = 0; i < s; i++)
        {
            if (hWnd == g_FEWExtras[i].hNSTC)
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                SetCloseButtonRect(rect);
                InvalidateRect(hWnd, &rect, FALSE);
                g_FEWExtras[i].eFlags |= MOUSE_PRESSED;
                break;
            }
        }
    }
    break;
    case WM_MOUSELEAVE: //If the mouse leaves the window, clear the flag and redraw so that the raised frame disappears
    {
        size_t s = g_FEWExtras.size();
        for (size_t i = 0; i < s; i++)
        {
            if (hWnd == g_FEWExtras[i].hNSTC)
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                SetCloseButtonRect(rect);
                g_FEWExtras[i].eFlags &= ~MOUSE_PRESSED;
                g_FEWExtras[i].eFlags &= ~MOUSE_TRACING;
                g_FEWExtras[i].eFlags &= ~MOUSE_OVER_BUTTON;
                InvalidateRect(hWnd, &rect, FALSE);
                break;
            }
        }
    }
    break;
    case WM_LBUTTONUP:
    {
        // Iterate through all subclassed NSTC windows
        size_t s = g_FEWExtras.size();
        //size_t i = 0;
        for (size_t i = 0; i < s; i++)
        {
            // Check if the current window matches and the mouse is pressed
            if (hWnd == g_FEWExtras[i].hNSTC && g_FEWExtras[i].eFlags & MOUSE_PRESSED)
            {
                // Reset the mouse pressed flag
                g_FEWExtras[i].eFlags &= ~MOUSE_PRESSED;
                break;
            }
        }

        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        RECT rect;
        GetClientRect(hWnd, &rect);
        SetCloseButtonRect(rect);

        // Check if the mouse click is within the button rectangle
        if (xPos >= rect.left && xPos <= rect.right && yPos >= rect.top && yPos <= rect.bottom)
        {
            size_t i = 0;
            for (; i < s; i++)
            {
                if (hWnd == g_FEWExtras[i].hNSTC)
                {
                    // Remove the flag so that the button edge won't be drawn after the treeview is enabled again
                    g_FEWExtras[i].eFlags &= ~MOUSE_OVER_BUTTON;
                    break;
                }
            }
            //SetFoldersPaneVisible(GetParent(GetParent(GetParent(GetParent(hWnd)))), false);
            g_FEWExtras[i].SetTreeviewVisible(false);
        }
    }
    break;
    case WM_SIZE:
    {
        // Static variable to track if resizing is in progress
        static bool resizing = false;
        if (!resizing) // If not currently resizing
        {
            resizing = true; // Set resizing flag to true
            RECT rect; // Create a RECT struct to hold window dimensions

            // Get the parent window of the current window
            HWND ctrlNotSink = GetParent(hWnd);
            GetWindowRect(ctrlNotSink, &rect); // Get the screen coordinates of the parent window
            MapWindowPoints(NULL, GetParent(ctrlNotSink), (POINT*)&rect, 2); // Map the screen coordinates to client coordinates
            int ctrlNotSinkXOffset = rect.left; // Get the x offset of the parent window
            int ctrlNotSinkYOffset = rect.top; // Get the y offset of the parent window
            GetClientRect(ctrlNotSink, &rect); // Get the client area dimensions of the parent window

            // Resize the parent window to fit the client area
            SetWindowPos(
                ctrlNotSink, 0, 0, ctrlNotSinkYOffset, rect.right + ctrlNotSinkXOffset + 1,
                rect.bottom,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

            // Resize the current window to fit the client area of the parent window
            GetClientRect(ctrlNotSink, &rect); // Get the client area dimensions of the parent window
            SetWindowPos(hWnd, 0, 0, 0, rect.right, rect.bottom,
                         SWP_NOZORDER | SWP_NOACTIVATE);

            // Find the treeview window within the current window
            HWND treeview = FindWindowEx(hWnd, NULL, L"SysTreeView32", NULL);
            GetClientRect(hWnd, &rect); // Get the client area dimensions of the current window

            // Modify the style of the treeview to remove the client edge
            LONG lStyle = GetWindowLong(treeview, GWL_EXSTYLE);
            lStyle &= ~WS_EX_CLIENTEDGE;
            SetWindowLong(treeview, GWL_EXSTYLE, lStyle);

            // Calculate and set the position and size of the treeview window
            int themedBandOffset = GetThemedBandOffset(); // Get themed band offset
            SetWindowPos(treeview, 0, 2, 24 + themedBandOffset,
                         rect.right - 4, rect.bottom - 26 - themedBandOffset,
                         SWP_NOZORDER | SWP_NOSENDCHANGING);

            // Redraw the parent window, current window, and treeview window
            InvalidateRect(ctrlNotSink, NULL, true);
            InvalidateRect(hWnd, NULL, true);
            InvalidateRect(treeview, NULL, true);

            resizing = false; // Set resizing flag back to false
        }
        return S_OK;
    }
    break;

    case WM_DESTROY: // Remove the entry in the global vector
        g_FEWExtras.erase(std::remove_if(
                              g_FEWExtras.begin(), g_FEWExtras.end(),
                              [hWnd](FEWExtra ne)
        {
            return ne.hNSTC == hWnd;
        }));
        return 0;
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TVSubclassProc(_In_ HWND hWnd,
                                _In_ UINT uMsg,
                                _In_ WPARAM wParam,
                                _In_ LPARAM lParam,
                                _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_SETCURSOR)
    {
        SetCursor(LoadCursor(NULL, IDC_ARROW)); //Remove hand cursor
        return S_OK;
    }
    else if(uMsg == TV_FIRST+74) //Remove top margin
    {
        wParam = 0;
    }
    else if(uMsg == TVM_SETINDENT) //Set full indent
    {
        wParam = 19;
    }
    else if(uMsg == TVM_SETITEMHEIGHT) //Set item height to the accurate value
    {
        wParam = 16;
    }
    else if (uMsg == TVM_INSERTITEM)
    {
        //Ensure that iIntegral is set to 1 for every item to achieve accurate item height
        LPTVINSERTSTRUCT lpis = (LPTVINSERTSTRUCT)lParam;
        lpis->itemex.iIntegral = 1;
    }
    else if (uMsg == TVM_SETITEM) // Quick access item workaround
    {
        LPTVITEMEXW lptvi = (LPTVITEMEXW)lParam;
        if (lptvi->mask & TVIF_INTEGRAL)
        {
            lptvi->iIntegral = 1;
        }
    }
    else if(uMsg == WM_SYSCOLORCHANGE && g_lineColorOptionInt == 2)
    {
        //If the automatic line color setting is enabled, new best color needs to be set
        DWORD windowColor = GetSysColor(COLOR_WINDOW);
        DWORD shadowColor = GetSysColor(COLOR_3DSHADOW);
        DWORD highlightColor = GetSysColor(COLOR_3DHIGHLIGHT);

        // Calculate the color distances
        double distanceShadow = ColorDistance(windowColor, shadowColor);
        double distanceHighlight = ColorDistance(windowColor, highlightColor);

        TreeView_SetLineColor(hWnd, distanceHighlight > distanceShadow ? highlightColor : shadowColor);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef HWND (*__cdecl NSCCreateTreeview_t)(void*, HWND);
NSCCreateTreeview_t NSCCreateTreeviewOriginal;
HWND __cdecl NSCCreateTreeviewHook(void* pThis, HWND hWnd)
{
    // Get the original member function to create the treeview so that it can be modified
    HWND treeview = NSCCreateTreeviewOriginal(pThis, hWnd);

    // Apply the modifications only if the treeview is created in file explorer window proper
    wchar_t root[20];
    HWND stwc = GetParent(GetParent(GetParent(GetParent(hWnd))));
    GetClassName(stwc, root, 20);
    if (lstrcmpW(root, L"ShellTabWindowClass") == 0)
    {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, NTCSubclassProc,
                NULL))
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(GetParent(hWnd), CtrlNotSinkSubclassProc, NULL);
            WindhawkUtils::SetWindowSubclassFromAnyThread(treeview, TVSubclassProc, NULL);
            for(FEWExtra& f : g_FEWExtras)
            {
                if(f.hSTWC == stwc)
                {
                    f.hCNS = GetParent(hWnd);
                    f.hNSTC = hWnd;
                    f.hTV = treeview;
                    break;
                }
            }

            // Set the style of the treeview
            DWORD dwStyle = TVS_HASBUTTONS | TVS_EDITLABELS |
                            TVS_SHOWSELALWAYS | WS_TABSTOP | WS_CLIPCHILDREN |
                            WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD;
            if (g_settingLinesAtRoot)
                dwStyle |= TVS_LINESATROOT;
            if(g_settingDrawDottedLines)
                dwStyle |= TVS_HASLINES;

            SetWindowLongPtrW(treeview, GWL_STYLE, dwStyle);

            TreeView_SetIndent(treeview, 20);
            TreeView_SetItemHeight(treeview, 16);

            // Set dotted line color. Shadow color is default
            DWORD highlightColor = GetSysColor(COLOR_3DHIGHLIGHT);
            if(g_lineColorOptionInt == 1) //highlight color
            {
                TreeView_SetLineColor(treeview, highlightColor);
            }
            else if(g_lineColorOptionInt == 2) //automatic color
            {
                DWORD windowColor = GetSysColor(COLOR_WINDOW);
                DWORD shadowColor = GetSysColor(COLOR_3DSHADOW);

                // Calculate the color distances
                double distanceShadow = ColorDistance(windowColor, shadowColor);
                double distanceHighlight = ColorDistance(windowColor, highlightColor);

                TreeView_SetLineColor(treeview, distanceHighlight > distanceShadow ? highlightColor : shadowColor);
            }
            // Set the extended style to bring back the classic tooltip
            DWORD exStyle = TreeView_GetExtendedStyle(treeview);
            exStyle &= ~TVS_EX_RICHTOOLTIP;
            TreeView_SetExtendedStyle(treeview, exStyle, TVS_EX_RICHTOOLTIP);
        }
    }

    return treeview;
}

// Remove pin icons
typedef HRESULT (*__cdecl NSCSetStateImageList_t)(struct _IMAGELIST*);
NSCSetStateImageList_t NSCASetStateImageListOriginal;
HRESULT __cdecl NSCSetStateImageListHook(struct _IMAGELIST* himagelist)
{
    return S_OK;
}

typedef void (*__cdecl CSBAddBrowserBarMenuItems_t)(void*, HMENU);
CSBAddBrowserBarMenuItems_t CSBAddBrowserBarMenuItemsOriginal;
void __cdecl CSBAddBrowserBarMenuItemsHook(void* pThis, HMENU hMenu)
{
    // This gets called when CShellBrowser populates the View menu.
    // If no menu items are added by this function, hook or original,
    // the submenu disappears.
    // There is a small bug that happens when the original function adds
    // exactly one item: the submenu label is singular, even though there
    // are two entries - one added by the original function, and one by
    // this hook.
    CSBAddBrowserBarMenuItemsOriginal(pThis, hMenu);
    for(FEWExtra &f : g_FEWExtras)
    {
        if(f.csb == pThis)
        {
            f.ModifyToolbarMenu(hMenu);
            break;
        }
    }
}

typedef HRESULT (*__cdecl CSBOnViewCommand_t)(void*, unsigned long, long);
CSBOnViewCommand_t CSBOnViewCommandOriginal;
HRESULT __cdecl CSBOnViewCommandHook(void* pThis, unsigned long a, long b)
{
    // This processes commands from the View menu, including the command of
    // the added menu item.

    if(a == TV_MENUITEM_ID)
    {
        for(FEWExtra& f : g_FEWExtras)
        {
            if(f.csb == pThis)
            {
                // Switch mod treeview visibility
                f.SetTreeviewVisible((f.eFlags & TREEVIEW_VIS) == 0);
                break;
            }
        }
        return S_OK;
    }

    return CSBOnViewCommandOriginal(pThis, a, b);
}

typedef HRESULT (*__cdecl FileCabinet_CreateViewWindow2_t)(IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;
HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(IShellBrowser* psb, void* var1, IShellView* psv1, IShellView* psv2, void* var2, HWND* hWnd)
{
    // This gets called on the creation of a file explorer window.
    // We get the COM interfaces from here, it's far more reliable than
    // the undocumented message used earlier.

    HRESULT hRes = FileCabinet_CreateViewWindow2Original(psb, var1, psv1, psv2, var2, hWnd);
    HWND stwc = GetParent(*hWnd); //?????
    g_FEWExtras.push_back(FEWExtra(stwc, psb));
    return hRes;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Classic Explorer Treeview Init");

    HMODULE hExplorerFrame = LoadLibraryW(L"explorerframe.dll");

    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    // Define the symbol hooks for required member functions
    WindhawkUtils::SYMBOL_HOOK hooks[] =
    {
        {   {
                L"private: struct HWND__ * __cdecl CNscTree::_CreateTreeview(struct HWND__ *)"
            },
            (void**)&NSCCreateTreeviewOriginal,
            (void*)NSCCreateTreeviewHook,
            FALSE
        },
        {   {
                L"public: virtual long __cdecl CNscTree::SetStateImageList(struct _IMAGELIST *)"
            },
            (void**)&NSCASetStateImageListOriginal,
            (void*)NSCSetStateImageListHook,
            FALSE
        },
        {   {
                L"long __cdecl FileCabinet_CreateViewWindow2(struct IShellBrowser *,struct tagFolderSetDataBase *,struct IShellView *,struct IShellView *,struct tagRECT *,struct HWND__ * *)"
            },
            (void**)&FileCabinet_CreateViewWindow2Original,
            (void*)FileCabinet_CreateViewWindow2Hook,
            FALSE
        },
        {   {
                L"private: void __cdecl CShellBrowser::_SetBrowserBarState(unsigned int,struct _GUID const *,int,struct _ITEMIDLIST_ABSOLUTE const *)"
            },
            (void**)&CSBSetBrowserBarStateOriginal,
            (void*)CSBSetBrowserBarStateHook,
            FALSE
        },
        {   {
                L"private: void __cdecl CShellBrowser::_AddBrowserBarMenuItems(struct HMENU__ *)"
            },
            (void**)&CSBAddBrowserBarMenuItemsOriginal,
            (void*)CSBAddBrowserBarMenuItemsHook,
            FALSE
        },
        {   {
                L"private: __int64 __cdecl CShellBrowser::_OnViewCommand(unsigned __int64,__int64)"
            },
            (void**)&CSBOnViewCommandOriginal,
            (void*)CSBOnViewCommandHook,
            FALSE
        }
    };
    // Hook the symbols in explorerframe.dll, return FALSE if any of the symbols cannot be hooked
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, hooks, 6))
    {
        Wh_Log(L"Failed to hook one or more member functions in ExplorerFrame.dll");
        return FALSE;
    }

    // Load mod settings
    g_settingDrawDottedLines = Wh_GetIntSetting(L"DrawLines");
    g_settingLinesAtRoot = Wh_GetIntSetting(L"LinesAtRoot");
    g_settingDrawButtons = Wh_GetIntSetting(L"DrawButtons");
    g_settingLineColorOption =
        WindhawkUtils::StringSetting::make(L"AlternateLineColor");
    g_settingFoldersPaneText =
        WindhawkUtils::StringSetting::make(L"FoldersText");
    g_settingCloseButtonXOffset = Wh_GetIntSetting(L"CloseButtonXOffset");
    g_settingCloseButtonYOffset = Wh_GetIntSetting(L"CloseButtonYOffset");

    // Set the line color option based on the string setting
    g_lineColorOptionInt = 0;
    if (wcscmp(g_settingLineColorOption.get(), L"true") == 0)
    {
        g_lineColorOptionInt = 1;
    }
    else if (wcscmp(g_settingLineColorOption.get(), L"Automatic") == 0)
    {
        g_lineColorOptionInt = 2;
    }

    // Set the folders pane text based on the string setting
    if (wcscmp(g_settingFoldersPaneText.get(), L"default") == 0)
    {
        Wh_Log(L"Loading default localized folders pane text.");
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (!shell32)
        {
            // Load the default, non localized text
            Wh_Log(L"Unable to load localized text resource: shell32 9045");
            std::wstring def = L"Folders";
            def.copy(g_defaultFoldersPaneText, 32);
        }
        else
        {
            // Load the default localized folders pane text from shell32 resources
            // On failure, load the default, non localized text
            if (LoadStringW(shell32, 9045, g_defaultFoldersPaneText, 32) == 0)
            {
                Wh_Log(
                    L"Unable to load localized text resource: shell32 "
                    L"9045");
                std::wstring def = L"Folders";
                def.copy(g_defaultFoldersPaneText, 32);
            }
        }
    }

    //Load the localized Treeview text
    HMODULE hIeFrame = LoadLibraryW(L"ieframe.dll");
    if(hIeFrame)
    {
        if (LoadStringW(hIeFrame, 3010, g_menuTreeviewText, 32) == 0)
        {
            Wh_Log(L"Unable to load localized text resource: ieframe 3010");
            std::wstring def = L"Treeview";
            def.copy(g_menuTreeviewText, 32);
        }
        FreeLibrary(hIeFrame);
    }

    Wh_Log(L"Classic Explorer Treeview init completed successfully.");
    return TRUE;
}


void Wh_ModUninit()
{
    Wh_Log(L"Classic Explorer Treeview uninit");
    size_t s = g_FEWExtras.size();
    Wh_Log(L"Removing subclasses from %u controls.", s);
    for (size_t i = 0; i < s; i++) // Loop through the subclassed controls
    {
        if(g_FEWExtras[i].hCNS == nullptr) continue; //if this is null then all are.
        // Remove subclasses
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_FEWExtras[i].hCNS, CtrlNotSinkSubclassProc);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_FEWExtras[i].hNSTC, NTCSubclassProc);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_FEWExtras[i].hTV, TVSubclassProc);
    }
    Wh_Log(L"Classic Explorer Treeview uninit completed successfully.");
}

void Wh_ModSettingsChanged()
{
    g_settingDrawDottedLines = Wh_GetIntSetting(L"DrawLines");
    g_settingLinesAtRoot = Wh_GetIntSetting(L"LinesAtRoot");
    g_settingDrawButtons = Wh_GetIntSetting(L"DrawButtons");
    g_settingLineColorOption =
        WindhawkUtils::StringSetting::make(L"AlternateLineColor");
    g_settingFoldersPaneText =
        WindhawkUtils::StringSetting::make(L"FoldersText");
    g_settingCloseButtonXOffset = Wh_GetIntSetting(L"CloseButtonXOffset");
    g_settingCloseButtonYOffset = Wh_GetIntSetting(L"CloseButtonYOffset");

    // Set the line color option based on the string setting
    g_lineColorOptionInt = 0;
    if (wcscmp(g_settingLineColorOption.get(), L"true") == 0)
    {
        g_lineColorOptionInt = 1;
    }
    else if (wcscmp(g_settingLineColorOption.get(), L"Automatic") == 0)
    {
        g_lineColorOptionInt = 2;
    }
}