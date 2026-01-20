// ==WindhawkMod==
// @id              remove-context-menu-items
// @name            Remove Unwanted Context Menu Items
// @description     Removes unwanted items from file context menus with configurable options
// @version         1.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Unwanted Context Menu Items

This mod removes unwanted items from file context menus with configurable options.

## Configurable Options

You can enable/disable removal of each item in the mod settings:

### Default ON:
- "Move to OneDrive"
- "Ask Copilot"
- "Scan with Microsoft Defender..."
- "Create with Designer"
- "Edit with Clipchamp"

### Default OFF:
- "Print"
- "Add to Favorites"
- "Cast to Device"
- "Give access to"
- "Share"
- "Restore previous versions"
- "Add to VLC's media player's Playlist"
- "Play with VLC media player"

## How it works
The mod hooks into the context menu creation process and removes unwanted menu items
by checking their text labels before they are displayed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- removeOneDrive: true
  $name: Remove "Move to OneDrive"
  $description: Remove OneDrive integration from context menu
- removeCopilot: true
  $name: Remove "Ask Copilot"
  $description: Remove Copilot integration from context menu
- removeDefender: true
  $name: Remove "Scan with Microsoft Defender..."
  $description: Remove Windows Defender scan option
- removeDesigner: true
  $name: Remove "Create with Designer"
  $description: Remove Microsoft Designer integration
- removeClipchamp: true
  $name: Remove "Edit with Clipchamp"
  $description: Remove Clipchamp video editor integration
- removePrint: false
  $name: Remove "Print"
  $description: Remove print option from context menu
- removeFavorites: false
  $name: Remove "Add to Favorites"
  $description: Remove add to favorites option
- removeCast: false
  $name: Remove "Cast to Device"
  $description: Remove cast to device option
- removeGiveAccess: false
  $name: Remove "Give access to"
  $description: Remove share/give access option
- removeShare: false
  $name: Remove "Share"
  $description: Remove share option
- removeRestoreVersions: false
  $name: Remove "Restore previous versions"
  $description: Remove restore previous versions option
- removeVLCPlaylist: false
  $name: Remove "Add to VLC's media player's Playlist"
  $description: Remove VLC playlist option
- removeVLCPlay: false
  $name: Remove "Play with VLC media player"
  $description: Remove VLC play option
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
    bool removePrint;
    bool removeFavorites;
    bool removeCast;
    bool removeGiveAccess;
    bool removeShare;
    bool removeRestoreVersions;
    bool removeVLCPlaylist;
    bool removeVLCPlay;
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
        {L"Ask Copilot", &g_settings.removeCopilot},
        {L"Scan with Microsoft Defender", &g_settings.removeDefender},
        {L"Create with Designer", &g_settings.removeDesigner},
        {L"Edit with Clipchamp", &g_settings.removeClipchamp},
        {L"Print", &g_settings.removePrint},
        {L"Add to Favorites", &g_settings.removeFavorites},
        {L"Add to favorites", &g_settings.removeFavorites},
        {L"Cast to Device", &g_settings.removeCast},
        {L"Give access to", &g_settings.removeGiveAccess},
        {L"Share", &g_settings.removeShare},
        {L"Restore previous versions", &g_settings.removeRestoreVersions},
        {L"Add to VLC", &g_settings.removeVLCPlaylist},
        {L"Play with VLC", &g_settings.removeVLCPlay}
    };
}

// Function to check if a menu item should be removed
bool ShouldRemoveMenuItem(const std::wstring& text) {
    // Check predefined items
    for (const auto& item : g_menuItems) {
        if (*(item.enabled) && text.find(item.text) != std::wstring::npos) {
            return true;
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
        mii.fMask = MIIM_STRING | MIIM_SUBMENU;
        
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
}

// Original function pointer
using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

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

// Load settings
void LoadSettings() {
    g_settings.removeOneDrive = Wh_GetIntSetting(L"removeOneDrive");
    g_settings.removeCopilot = Wh_GetIntSetting(L"removeCopilot");
    g_settings.removeDefender = Wh_GetIntSetting(L"removeDefender");
    g_settings.removeDesigner = Wh_GetIntSetting(L"removeDesigner");
    g_settings.removeClipchamp = Wh_GetIntSetting(L"removeClipchamp");
    g_settings.removePrint = Wh_GetIntSetting(L"removePrint");
    g_settings.removeFavorites = Wh_GetIntSetting(L"removeFavorites");
    g_settings.removeCast = Wh_GetIntSetting(L"removeCast");
    g_settings.removeGiveAccess = Wh_GetIntSetting(L"removeGiveAccess");
    g_settings.removeShare = Wh_GetIntSetting(L"removeShare");
    g_settings.removeRestoreVersions = Wh_GetIntSetting(L"removeRestoreVersions");
    g_settings.removeVLCPlaylist = Wh_GetIntSetting(L"removeVLCPlaylist");
    g_settings.removeVLCPlay = Wh_GetIntSetting(L"removeVLCPlay");
    
    InitializeMenuItems();
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing context menu cleaner mod");
    
    try {
        LoadSettings();
        
        if (!Wh_SetFunctionHook(
            (void*)TrackPopupMenuEx,
            (void*)TrackPopupMenuEx_Hook,
            (void**)&TrackPopupMenuEx_Original
        )) {
            Wh_Log(L"Failed to hook TrackPopupMenuEx");
            return FALSE;
        }
        
        Wh_Log(L"Context menu cleaner mod initialized successfully");
        return TRUE;
    } catch (...) {
        Wh_Log(L"Exception during initialization");
        return FALSE;
    }
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
