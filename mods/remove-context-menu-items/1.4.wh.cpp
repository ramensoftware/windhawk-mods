// ==WindhawkMod==
// @id              remove-context-menu-items
// @name            Remove Unwanted Context Menu Items (Classic Menu Only)
// @description     Removes unwanted items from file context menus with configurable options
// @version         1.4
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Unwanted Context Menu Items (Classic Menu Only)

⚠️ **Windows 11 Users:** This mod currently works only with the classic context menu. Please install the **Classic context menu on Windows 11** mod to use this mod. Support for the modern Windows 11 context menu will be implemented soon.

⚠️ **Language Notice:** The predefined toggle options are currently optimized for English Windows.

- **Other Languages:** If you use Windows in a different language, please use the Custom Items field in the settings to manually enter the names of the items you wish to remove.
- **Spelling Variations:** If your Windows is in English but uses different spellings (for example, "Favourites" instead of "Favorites") and the toggle doesn't work, please let me know on [GitHub](https://github.com/ramensoftware/windhawk-mods/issues) so I can add it to the predefined list!

---

This mod removes unwanted items from file context menus with configurable options.

## Screenshots

**Before and after right-clicking on an empty space:**

![Before and after on empty space](https://i.imgur.com/xEyRTk1.png)

**Before and after right-clicking a video file:**

![Before and after on video file](https://i.imgur.com/CPNiFtQ.png)

## Features

Clean up your Windows context menus by removing bloatware and unwanted items:

### Microsoft Bloatware (Default: ON)
- Move to OneDrive
- Ask Copilot
- Scan with Microsoft Defender
- Create with Designer
- Edit with Clipchamp

### Optional Items (Default: OFF)
- Print, Share, Cast to Device
- Add to Favorites, Pin to Start
- VLC media player options
- Windows Media Player options
- Refresh, Preview, Open, Edit, Play
- Restore previous versions
- Copy as path, Customize folder
- Include in library
- Rotate left/right
- ...and many more!

### Custom Items
You can also add your own custom menu items to remove by entering their text in the settings.

**Basic Usage (Exact Match):**
- "Copy" - Removes only the "Copy" option (exact match)
- "Cut" - Removes only the "Cut" option (exact match)
- "Open" - Removes only "Open" (exact match, won't remove "Open in new tab")

**Wildcard Usage (Prefix Match):**
Add an asterisk (*) at the end to match items that start with the given text:
- "Open*" - Removes "Open", "Open with...", "Open in Terminal", "Open in new tab", etc.
- "Move to OneDrive*" - Removes "Move to OneDrive", "Move to OneDrive - Personal", "Move to OneDrive - Company Name", etc.
- "C*" - Removes all menu items that start with C ("Cut", "Copy", "Create shortcut"). Be careful!

**Tip:** Right-click a file/folder, note the exact text of the menu item you want to remove (ignore keyboard shortcut letters like &), then add it to Custom Items. Use the asterisk (*) only if you want to remove multiple items with the same prefix.

## How it works
The mod hooks into the context menu creation process and removes unwanted menu items by checking their text labels before they are displayed. It also automatically cleans up duplicate separator lines. Custom items use exact matching by default for precision, with optional wildcard support for flexibility.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# NOTE: Predefined options below are in English only. For other languages, use Custom Items at the bottom.

# Items with default ON
- removeOneDrive: true
  $name: Remove "Move to OneDrive"
- removeCopilot: true
  $name: Remove "Ask Copilot"
- removeDefender: true
  $name: Remove "Scan with Microsoft Defender..."
- removeDesigner: true
  $name: Remove "Create with Designer"
- removeClipchamp: true
  $name: Remove "Edit with Clipchamp"

# Basic items (default OFF)
- removeOpen: false
  $name: Remove "Open"
- removeOpenInNewTab: false
  $name: Remove "Open in new tab"
- removeOpenInNewWindow: false
  $name: Remove "Open in new window"
- removeEdit: false
  $name: Remove "Edit"
- removePlay: false
  $name: Remove "Play"
- removePreview: false
  $name: Remove "Preview"
- removePrint: false
  $name: Remove "Print"
- removeShare: false
  $name: Remove "Share"
- removeRefresh: false
  $name: Remove "Refresh"
- removeCopyAsPath: false
  $name: Remove "Copy as path"
- removeCustomizeFolder: false
  $name: Remove "Customize this folder..."
- removeFavorites: false
  $name: Remove "Add to Favorites"
- removePinToStart: false
  $name: Remove "Pin to Start"
- removeCast: false
  $name: Remove "Cast to Device"
- removeGiveAccess: false
  $name: Remove "Give access to"
- removeRestoreVersions: false
  $name: Remove "Restore previous versions"
- removeIncludeInLibrary: false
  $name: Remove "Include in library"
- removeRotate: false
  $name: Remove "Rotate right" and "Rotate left"

# Items with app names mentioned (default OFF)
- removeSendWithQuickShare: false
  $name: Remove "Send with Quick Share"
- removeVLCPlaylist: false
  $name: Remove "Add to VLC media player's Playlist"
- removeVLCPlay: false
  $name: Remove "Play with VLC media player"
- removeAddToMediaPlayerQueue: false
  $name: Remove "Add to Media Player play queue"
- removePlayWithMediaPlayer: false
  $name: Remove "Play with Media Player"
- removeEditInNotepad: false
  $name: Remove "Edit in Notepad"
- removeEditWithPhotos: false
  $name: Remove "Edit with Photos"
- removeEditWithPaint: false
  $name: Remove "Edit with Paint"
- removeNvidiaControlPanel: false
  $name: Remove "NVIDIA Control Panel"

# Custom items
- customItems:
  - ""
  $name: Custom items to remove
  $description: Enter exact menu item names. Add * at the end for prefix matching (e.g., "Open*" removes all items starting with "Open")
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>

// Settings structure
struct {
    bool removeOneDrive;
    bool removeCopilot;
    bool removeDefender;
    bool removeDesigner;
    bool removeClipchamp;
    bool removeOpen;
    bool removeOpenInNewTab;
    bool removeOpenInNewWindow;
    bool removeEdit;
    bool removePlay;
    bool removePreview;
    bool removePrint;
    bool removeShare;
    bool removeSendWithQuickShare;
    bool removeRefresh;
    bool removeCopyAsPath;
    bool removeCustomizeFolder;
    bool removeFavorites;
    bool removePinToStart;
    bool removeCast;
    bool removeGiveAccess;
    bool removeRestoreVersions;
    bool removeIncludeInLibrary;
    bool removeRotate;
    bool removeVLCPlaylist;
    bool removeVLCPlay;
    bool removeAddToMediaPlayerQueue;
    bool removePlayWithMediaPlayer;
    bool removeEditInNotepad;
    bool removeEditWithPhotos;
    bool removeEditWithPaint;
    bool removeNvidiaControlPanel;
    std::vector<std::wstring> customItems;
} g_settings;

// Structure to hold menu item info
struct MenuItem {
    std::wstring text;
    bool* enabled;
};

// List of predefined menu items to check
std::vector<MenuItem> g_menuItems;

// Function to initialize menu items based on settings
void InitializeMenuItems() {
    g_menuItems.clear();
    g_menuItems = {
        {L"Move to OneDrive", &g_settings.removeOneDrive},
        {L"Always keep on this device", &g_settings.removeOneDrive}, // OneDrive submenu item
        {L"Free up space", &g_settings.removeOneDrive}, // OneDrive submenu item
        {L"Ask Copilot", &g_settings.removeCopilot},
        {L"Scan with Microsoft Defender...", &g_settings.removeDefender},
        {L"Create with Designer", &g_settings.removeDesigner},
        {L"Edit with Clipchamp", &g_settings.removeClipchamp},
        {L"Open", &g_settings.removeOpen},
        {L"Open in new tab", &g_settings.removeOpenInNewTab},
        {L"Open in new window", &g_settings.removeOpenInNewWindow},
        {L"Edit", &g_settings.removeEdit},
        {L"Play", &g_settings.removePlay},
        {L"Preview", &g_settings.removePreview},
        {L"Print", &g_settings.removePrint},
        {L"Share", &g_settings.removeShare},
        {L"Send with Quick Share", &g_settings.removeSendWithQuickShare},
        {L"Refresh", &g_settings.removeRefresh},
        {L"Copy as path", &g_settings.removeCopyAsPath},
        {L"Customize this folder...", &g_settings.removeCustomizeFolder},
        {L"Add to Favorites", &g_settings.removeFavorites},
        {L"Add to favorites", &g_settings.removeFavorites},
        {L"Add to Favourites", &g_settings.removeFavorites},
        {L"Add to favourites", &g_settings.removeFavorites},
        {L"Pin to Start", &g_settings.removePinToStart},
        {L"Cast to Device", &g_settings.removeCast},
        {L"Give access to", &g_settings.removeGiveAccess},
        {L"Restore previous versions", &g_settings.removeRestoreVersions},
        {L"Include in library", &g_settings.removeIncludeInLibrary},
        {L"Rotate right", &g_settings.removeRotate},
        {L"Rotate left", &g_settings.removeRotate},
        {L"Add to VLC media player's Playlist", &g_settings.removeVLCPlaylist},
        {L"Play with VLC media player", &g_settings.removeVLCPlay},
        {L"Add to Media Player play queue", &g_settings.removeAddToMediaPlayerQueue},
        {L"Play with Media Player", &g_settings.removePlayWithMediaPlayer},
        {L"Edit in Notepad", &g_settings.removeEditInNotepad},
        {L"Edit with Photos", &g_settings.removeEditWithPhotos},
        {L"Edit with Paint", &g_settings.removeEditWithPaint},
        {L"NVIDIA Control Panel", &g_settings.removeNvidiaControlPanel}
    };
}

// Function to normalize string for comparison (handle different apostrophe types)
std::wstring NormalizeString(const std::wstring& text) {
    std::wstring result = text;
    
    // Replace all types of apostrophes with standard apostrophe
    // ' (U+2019) RIGHT SINGLE QUOTATION MARK
    // ' (U+2018) LEFT SINGLE QUOTATION MARK  
    // ʼ (U+02BC) MODIFIER LETTER APOSTROPHE
    // ´ (U+00B4) ACUTE ACCENT
    // ` (U+0060) GRAVE ACCENT
    
    for (size_t i = 0; i < result.length(); i++) {
        wchar_t c = result[i];
        // Normalize various apostrophe-like characters to standard apostrophe (')
        if (c == L'\u2019' || c == L'\u2018' || c == L'\u02BC' || 
            c == L'\u00B4' || c == L'\u0060') {
            result[i] = L'\'';
        }
    }
    
    return result;
}

// Function to remove ampersands from menu text (keyboard shortcuts)
std::wstring RemoveAmpersands(const std::wstring& text) {
    std::wstring result;
    for (wchar_t c : text) {
        if (c != L'&') {
            result += c;
        }
    }
    return result;
}

// Function to check if a custom item matches a menu text
bool MatchesCustomItem(const std::wstring& menuText, const std::wstring& customItem) {
    if (customItem.empty()) {
        return false;
    }
    
    // Check if the custom item ends with asterisk (wildcard)
    bool useWildcard = (customItem.back() == L'*');
    
    std::wstring searchPattern = customItem;
    if (useWildcard) {
        // Remove the asterisk for comparison
        searchPattern = customItem.substr(0, customItem.length() - 1);
        
        // Empty pattern after removing asterisk means match nothing
        if (searchPattern.empty()) {
            return false;
        }
        
        // Check if menu text starts with the pattern (prefix match)
        if (menuText.length() >= searchPattern.length()) {
            return menuText.substr(0, searchPattern.length()) == searchPattern;
        }
        return false;
    } else {
        // Exact match required
        return menuText == searchPattern;
    }
}

// Function to check if a menu item should be removed
bool ShouldRemoveMenuItem(const std::wstring& text) {
    // Remove ampersands and normalize apostrophes for comparison
    std::wstring cleanText = NormalizeString(RemoveAmpersands(text));
    
    // Check predefined items with exact matching
    for (const auto& item : g_menuItems) {
        if (*(item.enabled)) {
            std::wstring cleanItemText = NormalizeString(RemoveAmpersands(item.text));
            
            // Exact match only
            if (cleanText == cleanItemText) {
                return true;
            }
        }
    }
    
    // Check custom items with exact/wildcard matching
    for (const auto& customItem : g_settings.customItems) {
        if (!customItem.empty()) {
            std::wstring cleanCustomItem = NormalizeString(RemoveAmpersands(customItem));
            
            if (MatchesCustomItem(cleanText, cleanCustomItem)) {
                Wh_Log(L"Removing custom item: %s (matched: %s)", cleanText.c_str(), cleanCustomItem.c_str());
                return true;
            }
        }
    }
    
    return false;
}

// Function to process a menu and remove unwanted items
void ProcessMenu(HMENU hMenu) {
    if (!hMenu) return;
    
    int itemCount = GetMenuItemCount(hMenu);
    
    // Iterate through menu items in reverse to safely remove items
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {0};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE;
        
        // Get the length of the menu item text
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            if (mii.cch > 0) {
                // Allocate buffer and get the actual text
                std::wstring text(mii.cch + 1, L'\0');
                mii.dwTypeData = &text[0];
                mii.cch++;
                
                if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
                    text.resize(wcslen(text.c_str()));
                    
                    // Check if this item should be removed
                    if (ShouldRemoveMenuItem(text)) {
                        DeleteMenu(hMenu, i, MF_BYPOSITION);
                    }
                }
            }
            
            // Recursively process submenus
            if (mii.hSubMenu) {
                ProcessMenu(mii.hSubMenu);
            }
        }
    }
    
    // Remove consecutive separators
    itemCount = GetMenuItemCount(hMenu);
    bool lastWasSeparator = true; // Start as true to remove leading separators
    
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {0};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            bool isSeparator = (mii.fType & MFT_SEPARATOR) != 0;
            
            if (isSeparator && lastWasSeparator) {
                // Remove consecutive separator
                DeleteMenu(hMenu, i, MF_BYPOSITION);
            }
            
            lastWasSeparator = isSeparator;
        }
    }
    
    // Remove trailing separator if exists
    itemCount = GetMenuItemCount(hMenu);
    if (itemCount > 0) {
        MENUITEMINFOW mii = {0};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, itemCount - 1, TRUE, &mii)) {
            if (mii.fType & MFT_SEPARATOR) {
                DeleteMenu(hMenu, itemCount - 1, MF_BYPOSITION);
            }
        }
    }
}

// Original function pointers
using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

// Hook function for TrackPopupMenuEx
BOOL WINAPI TrackPopupMenuEx_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    HWND hWnd,
    LPTPMPARAMS lptpm
) {
    ProcessMenu(hMenu);
    return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
}

// Hook function for TrackPopupMenu
BOOL WINAPI TrackPopupMenu_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    int nReserved,
    HWND hWnd,
    const RECT* prcRect
) {
    ProcessMenu(hMenu);
    return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

// Load settings
void LoadSettings() {
    g_settings.removeOneDrive = Wh_GetIntSetting(L"removeOneDrive");
    g_settings.removeCopilot = Wh_GetIntSetting(L"removeCopilot");
    g_settings.removeDefender = Wh_GetIntSetting(L"removeDefender");
    g_settings.removeDesigner = Wh_GetIntSetting(L"removeDesigner");
    g_settings.removeClipchamp = Wh_GetIntSetting(L"removeClipchamp");
    g_settings.removeOpen = Wh_GetIntSetting(L"removeOpen");
    g_settings.removeOpenInNewTab = Wh_GetIntSetting(L"removeOpenInNewTab");
    g_settings.removeOpenInNewWindow = Wh_GetIntSetting(L"removeOpenInNewWindow");
    g_settings.removeEdit = Wh_GetIntSetting(L"removeEdit");
    g_settings.removePlay = Wh_GetIntSetting(L"removePlay");
    g_settings.removePreview = Wh_GetIntSetting(L"removePreview");
    g_settings.removePrint = Wh_GetIntSetting(L"removePrint");
    g_settings.removeShare = Wh_GetIntSetting(L"removeShare");
    g_settings.removeSendWithQuickShare = Wh_GetIntSetting(L"removeSendWithQuickShare");
    g_settings.removeRefresh = Wh_GetIntSetting(L"removeRefresh");
    g_settings.removeCopyAsPath = Wh_GetIntSetting(L"removeCopyAsPath");
    g_settings.removeCustomizeFolder = Wh_GetIntSetting(L"removeCustomizeFolder");
    g_settings.removeFavorites = Wh_GetIntSetting(L"removeFavorites");
    g_settings.removePinToStart = Wh_GetIntSetting(L"removePinToStart");
    g_settings.removeCast = Wh_GetIntSetting(L"removeCast");
    g_settings.removeGiveAccess = Wh_GetIntSetting(L"removeGiveAccess");
    g_settings.removeRestoreVersions = Wh_GetIntSetting(L"removeRestoreVersions");
    g_settings.removeIncludeInLibrary = Wh_GetIntSetting(L"removeIncludeInLibrary");
    g_settings.removeRotate = Wh_GetIntSetting(L"removeRotate");
    g_settings.removeVLCPlaylist = Wh_GetIntSetting(L"removeVLCPlaylist");
    g_settings.removeVLCPlay = Wh_GetIntSetting(L"removeVLCPlay");
    g_settings.removeAddToMediaPlayerQueue = Wh_GetIntSetting(L"removeAddToMediaPlayerQueue");
    g_settings.removePlayWithMediaPlayer = Wh_GetIntSetting(L"removePlayWithMediaPlayer");
    g_settings.removeEditInNotepad = Wh_GetIntSetting(L"removeEditInNotepad");
    g_settings.removeEditWithPhotos = Wh_GetIntSetting(L"removeEditWithPhotos");
    g_settings.removeEditWithPaint = Wh_GetIntSetting(L"removeEditWithPaint");
    g_settings.removeNvidiaControlPanel = Wh_GetIntSetting(L"removeNvidiaControlPanel");
    
    // Load custom items - with proper bounds checking
    g_settings.customItems.clear();
    int maxItems = 100; // Safety limit to prevent infinite loop
    for (int i = 0; i < maxItems; i++) {
        PCWSTR customItem = Wh_GetStringSetting(L"customItems[%d]", i);
        if (!customItem) break; // No more items
        
        std::wstring item(customItem);
        Wh_FreeStringSetting(customItem);
        
        if (!item.empty()) {
            g_settings.customItems.push_back(item);
            Wh_Log(L"Loaded custom item %d: %s", i, item.c_str());
        }
    }
    
    Wh_Log(L"Total custom items loaded: %d", (int)g_settings.customItems.size());
    
    InitializeMenuItems();
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing context menu cleaner mod");
    Wh_Log(L"NOTE: Predefined options are in English. For other languages, use Custom Items in settings.");
    
    LoadSettings();
    
    if (!Wh_SetFunctionHook(
        (void*)TrackPopupMenuEx,
        (void*)TrackPopupMenuEx_Hook,
        (void**)&TrackPopupMenuEx_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook(
        (void*)TrackPopupMenu,
        (void*)TrackPopupMenu_Hook,
        (void**)&TrackPopupMenu_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenu");
        return FALSE;
    }
    
    Wh_Log(L"Context menu cleaner mod initialized successfully");
    return TRUE;
}

// Settings changed callback
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"Uninitializing context menu cleaner mod");
}
