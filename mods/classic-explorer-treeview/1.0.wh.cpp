// ==WindhawkMod==
// @id              classic-explorer-treeview
// @name            Classic Explorer Treeview
// @description     Modifies Folder Treeview in file explorer so as to make it look more classic.
// @version         1.0
// @author          Waldemar
// @github          https://github.com/CyprinusCarpio
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -loleaut32 -lole32
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
  $description: Should the mod draw it's own +/- buttons.
- AlternateLineColor: Automatic
  $name: Alternate line color
  $description: Use highlight color instead of shadow color for drawing lines. May produce better results on some dark themes.
  $options:
    - False: Use default color.
    - True: Use alternate color.
    - Automatic: Use alternate color if the default color's contrast is too low.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Classic Explorer Treeview

This mod changes the Explorer Treeview (otherwise known as Folder Pane or
Navigation Pane) to look more classic. It draws it's own Folder Band with a
functional X button. If you are using OpenShell, then you need to set the
correct settings for Classic Explorer in the Navigation Pane tab: Vista style,
Tree item spacing -2, Full size offset for sub-folders. If you are using Windows
11 23H2, then set the tree item spacing to -6, and enable compact spacing in the
Explorer folder settings. For proper function and accurate look, the Organize
bar has to be removed. To accomplish this, use Resource Hacker and modify
shellstyle.dll by adding the the following line:

'<Element padding="rect(0rp,0rp,0rp,-32rp)"/>'

below the line:

'<style resid="FolderBandStyle">'

# Changelog:
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

#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L"__cdecl"
#else
#define CALCON __thiscall
#define SCALCON L"__thiscall"
#endif

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <wingdi.h>
#include <winrt/base.h>

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

enum NSCTExtraFlags
{
    MOUSE_OVER_BUTTON = 1 << 0,
    MOUSE_PRESSED = 1 << 1,
    MOUSE_TRACING = 1 << 2
};

struct NSCTExtra
{
    HWND hWnd;
    int eFlags = 0;

    NSCTExtra(HWND h)
    {
        hWnd = h;
    }
};

// Mod settings
bool g_settingDrawDottedLines = true;
bool g_settingLinesAtRoot = false;
bool g_settingDrawButtons = true;
WindhawkUtils::StringSetting g_settingLineColorOption;
WindhawkUtils::StringSetting g_settingFoldersPaneText;

//Used to keep track of NSTC window handles and some extra data associated with them
std::vector<NSCTExtra> g_subclassedNSTCs;
int g_lineColorOptionInt = 2;
wchar_t g_defaultFoldersPaneText[32];

// Euclidean color distance
double ColorDistance(DWORD color1, DWORD color2)
{
    int b1 = (color1 & 0xFF);
    int g1 = ((color1 >> 8) & 0xFF);
    int r1 = ((color1 >> 16) & 0xFF);

    int b2 = (color2 & 0xFF);
    int g2 = ((color2 >> 8) & 0xFF);
    int r2 = ((color2 >> 16) & 0xFF);

    return std::sqrt(std::pow(r2 - r1, 2) + std::pow(g2 - g1, 2) +
                     std::pow(b2 - b1, 2));
}

void DrawDottedLinesAndButtons(HWND hTree, HDC hdc)
{
    // Get the handle of the root item in the tree view
    HTREEITEM hItem = TreeView_GetRoot(hTree);

    RECT rect;
    
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

    while (hItem != NULL)
    {
        // Get the bounding rectangle of the specified tree view item
        TreeView_GetItemRect(hTree, hItem, &rect, TRUE); 
        rect.left -= 33; 
        rect.top += 6; 
        
        // Define a TVITEM struct to retrieve the number of children for the specified item
        TVITEM tvi;
        tvi.mask = TVIF_CHILDREN; // Specify that we are interested in the number of children
        tvi.hItem = hItem; // Set the handle of the tree view item
        
        // Check if the item is at the top level by comparing the left position of the rectangle
        // to either 10 or -10 based on the value of g_settingLinesAtRoot
        bool isTopLevel = rect.left <= (g_settingLinesAtRoot ? 10 : -10);

        // Check if we should draw dotted lines
        if (g_settingDrawDottedLines)
        {
            // Create a logical brush
            LOGBRUSH LogBrush;
            LogBrush.lbColor = useHighlight ? highlightColor : shadowColor;
            LogBrush.lbStyle = PS_SOLID;
            // Create a cosmetic pen with alternating dashes and specify the brush
            HPEN hPen = ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, 1, &LogBrush, 0, NULL);
        
            // Select the new pen and save the old pen
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            if (!isTopLevel)
            {
                // Get the next sibling a draw a vertical line to it
                HTREEITEM nextItem = TreeView_GetNextSibling(hTree, hItem);
                MoveToEx(hdc, rect.left + 4, isTopLevel ? rect.top + 4 : rect.top - 4, NULL);
                if (nextItem != NULL)
                {
                    RECT nextItemRect;
                    TreeView_GetItemRect(hTree, nextItem, &nextItemRect, TRUE);
                    LineTo(hdc, rect.left + 4, nextItemRect.top + 5);
                }
                // If there's no next sibling, draw a line to the middle of expando button
                else
                {
                    LineTo(hdc, rect.left + 4, rect.bottom - 7);
                }
            }
        
            // Draw a horizontal line
            MoveToEx(hdc, rect.left + 6, rect.top + 4, NULL);
            LineTo(hdc, rect.left + 14, rect.top + 4);
            // Restore the old pen and delete the new pen
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }

        // Check if the setting to draw buttons is enabled and the node is not a top-level node when lines at root is disabled
        // and the node has one or more children
        if (g_settingDrawButtons && !(isTopLevel && !g_settingLinesAtRoot) && TreeView_GetItem(hTree, &tvi) && tvi.cChildren > 0)
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
        
            // Draw a line in the item's rectangle based on its expanded state
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

        // Traverse the treeview control to find the next visible item
        
        // Get the first child of the current item
        HTREEITEM child = TreeView_GetChild(hTree, hItem);
        if (child != NULL)
        {
            hItem = child; // Set the current item to the child
            continue;
        }
        
        // If no child, get the next sibling of the current item
        HTREEITEM sibling = TreeView_GetNextSibling(hTree, hItem);
        if (sibling != NULL)
        {
            hItem = sibling; // Set the current item to the sibling
            continue;
        }
        
        // If no sibling, get the next visible item
        hItem = TreeView_GetNextVisible(hTree, hItem);
        // If hItem is still NULL, the loop ends.
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

LRESULT CALLBACK NSCSubclassProc(_In_ HWND hWnd,
                                 _In_ UINT uMsg,
                                 _In_ WPARAM wParam,
                                 _In_ LPARAM lParam,
                                 _In_ DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_NOTIFY:
    {
        LPNMHDR lpnmh = (LPNMHDR)lParam;
        switch (lpnmh->code)
        {
        case NM_CUSTOMDRAW: // Handle custom drawing notifications sent by the treeview control
        {
            HWND hTree = FindWindowEx(hWnd, NULL, L"SysTreeView32", NULL); // Find the treeview control
            LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)lParam;
            switch (pCustomDraw->nmcd.dwDrawStage)
            {
            case CDDS_PREPAINT:
                return CDRF_NOTIFYPOSTPAINT; // Notify after the painting cycle
            case CDDS_POSTPAINT:
                DrawDottedLinesAndButtons(hTree, pCustomDraw->nmcd.hdc);
                return S_OK;
                break;
            default:
                break;
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
        size_t s = g_subclassedNSTCs.size();
        for (size_t i = 0; i < s; i++)
        {
            if (hWnd == g_subclassedNSTCs[i].hWnd)
            {
                eFlags = g_subclassedNSTCs[i].eFlags;
                break;
            }
        }

        // Create a brush for the close button
        HBRUSH closeColor = CreateSolidBrush(GetSysColor(COLOR_BTNTEXT));

        // draw the close button
        // ##    ##
        rect.top = origTop + 7;
        rect.left = origRight - 18;
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
        rect.top = origTop + 13;
        rect.left = origRight - 18;
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
            buttonFrame.left = buttonFrame.right - 24;
            buttonFrame.right = buttonFrame.right - 4;
            buttonFrame.bottom = buttonFrame.top + 19;
            buttonFrame.top = buttonFrame.top + 2;
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
        rect.left = rect.right - 24;
        rect.right -= 4;
        rect.bottom = rect.top + 19;
        rect.top += 2;
    
        // Check if the mouse is inside the rectangle
        bool mouseInside = false;
        if (xPos >= rect.left && xPos <= rect.right && yPos >= rect.top &&
            yPos <= rect.bottom)
        {
            mouseInside = true;
        }
    
        // Iterate through the list of subclassed NSTCs
        size_t s = g_subclassedNSTCs.size();
        for (size_t i = 0; i < s; i++)
        {
            // Check if the current window handle matches the one in the list
            if (hWnd == g_subclassedNSTCs[i].hWnd)
            {
                // If mouse is inside, set the flag, else clear it. Redraw the window
                if (mouseInside)
                {
                    InvalidateRect(hWnd, &rect, FALSE);
                    g_subclassedNSTCs[i].eFlags |= MOUSE_OVER_BUTTON;
                }
                else if (g_subclassedNSTCs[i].eFlags & MOUSE_OVER_BUTTON)
                {
                    g_subclassedNSTCs[i].eFlags &= ~MOUSE_OVER_BUTTON;
                    InvalidateRect(hWnd, &rect, FALSE);
                }
                // If the mouse is not being traced, trace it so that we know when it leaves the window
                // so that we can stop drawing the raised button edge
                if (!(g_subclassedNSTCs[i].eFlags & MOUSE_TRACING))
                {
                    g_subclassedNSTCs[i].eFlags |= MOUSE_TRACING;
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
        size_t s = g_subclassedNSTCs.size();
        for (size_t i = 0; i < s; i++)
        {
            if (hWnd == g_subclassedNSTCs[i].hWnd)
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                rect.left = rect.right - 24;
                rect.right -= 4;
                rect.bottom = rect.top + 19;
                rect.top += 2;
                InvalidateRect(hWnd, &rect, FALSE);
                g_subclassedNSTCs[i].eFlags |= MOUSE_PRESSED;
                break;
            }
        }
    }
    break;
    case WM_MOUSELEAVE: //If the mouse leaves the window, clear the flag and redraw so that the raised frame disappears
    {
        size_t s = g_subclassedNSTCs.size();
        for (size_t i = 0; i < s; i++)
        {
            if (hWnd == g_subclassedNSTCs[i].hWnd)
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                rect.left = rect.right - 24;
                rect.right -= 4;
                rect.bottom = rect.top + 19;
                rect.top += 2;
                g_subclassedNSTCs[i].eFlags &= ~MOUSE_PRESSED;
                g_subclassedNSTCs[i].eFlags &= ~MOUSE_TRACING;
                g_subclassedNSTCs[i].eFlags &= ~MOUSE_OVER_BUTTON;
                InvalidateRect(hWnd, &rect, FALSE);
                break;
            }
        }
    }
    break;
    case WM_LBUTTONUP:
    {
        // Iterate through all subclassed NSTC windows
        size_t s = g_subclassedNSTCs.size();
        for (size_t i = 0; i < s; i++)
        {
            // Check if the current window matches and the mouse is pressed
            if (hWnd == g_subclassedNSTCs[i].hWnd && g_subclassedNSTCs[i].eFlags & MOUSE_PRESSED)
            {
                // Reset the mouse pressed flag
                g_subclassedNSTCs[i].eFlags &= ~MOUSE_PRESSED;
                break;
            }
        }

        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        RECT rect;
        GetClientRect(hWnd, &rect);
        rect.left = rect.right - 23;
        rect.right -= 4;
        rect.bottom = rect.top + 20;
        rect.top += 2;

        // Check if the mouse click is within the button rectangle
        if (xPos >= rect.left && xPos <= rect.right && yPos >= rect.top && yPos <= rect.bottom)
        {
            for (size_t i = 0; i < s; i++)
            {
                if (hWnd == g_subclassedNSTCs[i].hWnd)
                {
                    // Remove the flag so that the button edge won't be drawn after the treeview is enabled again
                    g_subclassedNSTCs[i].eFlags &= ~MOUSE_OVER_BUTTON;
                    break;
                }
            }
            // This approach is more or less copied from OpenShell, but I needed to get
            // the browser interface from the ShellTabWindowClass window first.

            //Get a com_ptr to a IOleWindow so that it can be queried.
            //See: https://stackoverflow.com/questions/40878829/is-there-a-way-to-get-ishellbrowser-from-a-dialog
            auto browser = winrt::com_ptr<IOleWindow>
            {
                reinterpret_cast<IOleWindow*>((void*)SendMessage(GetParent(GetParent(GetParent(GetParent(hWnd)))), WM_USER + 7, 0, 0)),
                winrt::take_ownership_from_abi
            };

            // Create smart pointers
            winrt::com_ptr<IPropertyBag> bag;
            winrt::com_ptr<IUnknown> frame;
            
            // Query the browser for the SID_FrameManager service and store it in the frame pointer
            browser.as<IServiceProvider>()->QueryService(SID_FrameManager, IID_PPV_ARGS(&frame));
            // Query the frame for the SID_PerBrowserPropertyBag service and store it in the bag pointer
            frame.as<IServiceProvider>()->QueryService(SID_PerBrowserPropertyBag, IID_PPV_ARGS(&bag));
            
            // Initialize a VARIANT structure to FALSE
            VARIANT val;
            VariantInit(&val);
            val.vt = VT_BOOL;
            val.boolVal = VARIANT_FALSE;
            // Write the VARIANT value to the property bag to hide the treeview.
            bag->Write(L"PageSpaceControlSizer_Visible", &val);
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
        g_subclassedNSTCs.erase(std::remove_if(
                                    g_subclassedNSTCs.begin(), g_subclassedNSTCs.end(),
                                    [hWnd](NSCTExtra ne)
        {
            return ne.hWnd == hWnd;
        }));
        return 0;
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TreeViewSubclassProc(_In_ HWND hWnd,
                                      _In_ UINT uMsg,
                                      _In_ WPARAM wParam,
                                      _In_ LPARAM lParam,
                                      _In_ DWORD_PTR dwRefData)
{
    //Set the cursor to the default arrow, so as to prevent the hand cursor being displayed on item hover
    if (uMsg == WM_SETCURSOR)
    {
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return TRUE;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef HWND (*CALCON NSCCreateTreeview_t)(void*, HWND);
NSCCreateTreeview_t NSCCreateTreeviewOriginal;
HWND CALCON NSCCreateTreeviewHook(void* pThis, HWND hWnd)
{
    // Get the original member function to create the treeview so that it can be modified
    HWND treeview = NSCCreateTreeviewOriginal(pThis, hWnd);

    // Apply the modifications only if the treeview is created in file explorer window proper
    wchar_t root[20];
    GetClassName(GetParent(GetParent(GetParent(GetParent(hWnd)))), root, 20);
    if (lstrcmpW(root, L"ShellTabWindowClass") == 0)
    {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, NSCSubclassProc,
                NULL))
        {
            g_subclassedNSTCs.push_back(NSCTExtra(hWnd));

            // Set the style of the treeview
            DWORD dwStyle = TVS_HASBUTTONS | TVS_EDITLABELS |
                            TVS_SHOWSELALWAYS | WS_TABSTOP | WS_CLIPCHILDREN |
                            WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD |
                            TVS_NONEVENHEIGHT;
            if (g_settingLinesAtRoot)
                dwStyle |= TVS_LINESATROOT;

            SetWindowLongPtrW(treeview, GWL_STYLE, dwStyle);

            TreeView_SetIndent(treeview, 20);
            TreeView_SetItemHeight(treeview, 16);

            // Set the extended style to bring back the classic tooltip
            DWORD exStyle = TreeView_GetExtendedStyle(treeview);
            exStyle &= ~TVS_EX_RICHTOOLTIP;
            TreeView_SetExtendedStyle(treeview, exStyle, TVS_EX_RICHTOOLTIP);

            WindhawkUtils::SetWindowSubclassFromAnyThread(
                treeview, TreeViewSubclassProc, NULL);
        }
    }

    return treeview;
}

// Remove pin icons
typedef HRESULT (*CALCON NSCSetStateImageList_t)(struct _IMAGELIST*);
NSCSetStateImageList_t NSCASetStateImageListOriginal;
HRESULT CALCON NSCSetStateImageListHook(struct _IMAGELIST* himagelist)
{
    return S_OK;
}

// Remove the top margin
typedef HRESULT (*CALCON NSCApplyTopMargin_t)(void);
NSCApplyTopMargin_t NSCApplyTopMarginOriginal;
HRESULT CALCON NSCApplyTopMarginHook()
{
    return S_OK;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Classic Explorer Treeview Init");

    HMODULE hExplorerFrame = GetModuleHandleW(L"explorerframe.dll");

    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    // Define the symbol hooks for required member functions
    WindhawkUtils::SYMBOL_HOOK hooks[] =
    {
        {   {
                L"private: struct HWND__ * " SCALCON " CNscTree::_CreateTreeview(struct HWND__ *)"
            },
            (void**)&NSCCreateTreeviewOriginal,
            (void*)NSCCreateTreeviewHook,
            FALSE
        },
        {   {L"private: long " SCALCON " CNscTree::ApplyTopMargin(void)"},
            (void**)&NSCApplyTopMarginOriginal,
            (void*)NSCApplyTopMarginHook,
            FALSE
        },
        {   {
                L"public: virtual long " SCALCON " CNscTree::SetStateImageList(struct _IMAGELIST *)"
            },
            (void**)&NSCASetStateImageListOriginal,
            (void*)NSCSetStateImageListHook,
            FALSE
        }
    };
    // Hook the symbols in explorerframe.dll, return FALSE if any of the symbols cannot be hooked
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, hooks, 3))
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

    Wh_Log(L"Classic Explorer Treeview init completed successfully.");
    return TRUE;
}


void Wh_ModUninit()
{
    Wh_Log(L"Classic Explorer Treeview uninit");
    size_t s = g_subclassedNSTCs.size();
    Wh_Log(L"Removing subclasses from %u controls.", s);
    for (size_t i = 0; i < s; i++) // Loop through the subclassed controls
    {
        // Remove subclass from the CNSCTree control
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_subclassedNSTCs[i].hWnd, NSCSubclassProc);

        // Find the child treeview and remove the subclass
        HWND child = FindWindowEx(g_subclassedNSTCs[i].hWnd, NULL, L"SysTreeView32", NULL);
        if (child != NULL) // Remove the subclass from the treeview only if it's found
            WindhawkUtils::RemoveWindowSubclassFromAnyThread( child, TreeViewSubclassProc);
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