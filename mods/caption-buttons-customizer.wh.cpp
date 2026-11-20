// ==WindhawkMod==
// @id              caption-buttons-customizer
// @name            Caption Buttons Customizer
// @description     Move the caption buttons to the left, reposition the window icon and title, and optionally draw macOS-style traffic lights (or your own button images).
// @version         1.1
// @author          grn15RenS
// @github          https://github.com/grn15RenS
// @donateUrl       https://ko-fi.com/grn15rens
// @license         MIT
// @include         dwm.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lole32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Caption Buttons Customizer
![MainScreenshots](https://i.imgur.com/zw9t06n.jpeg)

Moves the caption buttons (minimize, maximize, close) to the left side of the title bar and repositions the window icon and title to match. It can also draw macOS-style traffic lights (red / yellow / green circles) or your own button images. It works by hooking internal functions of the Desktop Window Manager (`dwm.exe` / `uDWM.dll`), so it works on standard Win32 windows. See *Compatibility and known issues* for apps it cannot change.

## ⚙️ Setup

`dwm.exe` is a critical system process, so Windhawk does not inject into it by default. To use this mod, add `dwm.exe` to the process inclusion list: *Windhawk Settings > Advanced settings > More advanced settings > Process inclusion list*. Without it, only the `explorer.exe` part of the mod loads and the caption buttons will not move.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

&nbsp;

# 🔧 Features & Preview 🖼️

## 1. Caption Buttons on the Left

|        |                                                 |
|--------|-------------------------------------------------|
| Before | ![Before](https://i.imgur.com/ScuG5Rl.png)      |
| After  | ![After](https://i.imgur.com/GhBj3BJ.png)       |

- Turn on *Move caption buttons to the left* to move the buttons to the left side of the title bar. When it is off, they stay in their default place on the right.
- If the buttons don't sit where you want them, adjust *Left margin (px)*.
- If the buttons look too far apart, raise *Tighten button spacing (px)*. It works on the left and on the right, and in File Explorer once `CabinetWClass` is removed from *Excluded window classes*. Values above about 20 have no extra effect, so the middle button stays usable. The clickable area of each button gets narrower along with the spacing.
- *Tighten button spacing* is meant for the macOS and Custom images styles. The circles are smaller than the button slot (46 px at 100% scaling), and the effect is capped, so the buttons cannot get quite as close together as on macOS.

## 2. Button order

Pick one of three presets in the *Button order* setting. The order is counted from the left edge of the window.

| Close, minimize, maximize | Close, maximize, minimize | Minimize, maximize, close |
|---------------------------|---------------------------|---------------------------|
| ![1](https://i.imgur.com/LW23zBN.png) | ![2](https://i.imgur.com/SlyZTmQ.png) | ![3](https://i.imgur.com/wfLhnCN.png) |

## 3. Show the close button on dialogs

Windows with only a close button (message boxes, About windows, Properties dialogs) can hide that button with *Show the close button on dialogs*. Alt+F4 and Esc still close the window. Their icon and title have their own alignment, set with *Icon and title alignment in dialogs*. Very narrow windows hide the icon and title when there is no room for them.

| Close Button and Left Alignment | No Close Button and Center Alignment |
|--------|-------|
| ![Dialog On](https://i.imgur.com/mZ8opqQ.png) | ![Dialog Off](https://i.imgur.com/DALB17X.png) |

## 4. Caption Button Style
Three button styles are available in *Caption button style*: the default Windows glyphs (default), macOS circles in colors you choose (a symbol appears on hover: × for close, − for minimize, two triangles for maximize and restore), or your own images (png, jpg or bmp).

| macOS style (circles) | Custom images | Windows default |
|-----------------------|---------------|-----------------|
| ![macOS style (circles)](https://i.imgur.com/SnThhQY.png) | ![Custom images](https://i.imgur.com/KPPIp9R.png) | ![Windows default](https://i.imgur.com/NmmLJ5E.png) |

### Note for custom button images
1. Set *Caption button style* to **Custom images**. (With the macOS style, the image settings are ignored and the circles stay as they are.)
2. Set **Icon folder** (or the path of each button image) to any folder you can read, for example `C:\Users\You\Pictures\ButtonIcons`.
3. Name the files `minimize`, `restore`, `maximize`, `close` (+ `.png`, `.jpg`, `.jpeg` or `.bmp`).
4. Optional state variants: `close_hover` (mouse is over the button), `close_pressed` (button is being clicked) and `close_disabled` (button is greyed out). Same for the other buttons. If a variant is missing, the base image is used (darkened for pressed). The four base images are enough.
5. Images are scaled to fit the button slot. The slot is about 12 px at 100% scaling and grows with scaling (about 15 px at 125%, 18 px at 150%, 21 px at 175%). Images keep their own shape and are not cut into a circle, so square images stay square. Use transparent PNGs with simple designs; a square image of 32-64 px works at every scale.
6. A button without an image keeps its circle.

Want to try it quickly? Download my ready-made set from [here](https://drive.google.com/drive/folders/1FPw3GCr8Xz-18HW6veTPc7TIgY6IFyIO), extract it, and set *Icon folder* to the extracted folder. The images are transparent PNGs and already use the right file names.

`dwm.exe` runs under its own account and usually cannot read your profile folders. To get around that, the part of the mod running inside `explorer.exe` (which can read your folders) decodes your images, shrinks them to at most 64 px, and hands the pixels to `dwm.exe` through the mod's own storage in Windhawk. No image files are copied anywhere, and `dwm.exe` never opens or decodes an image file.

## 5. Icon and title alignment
*Icon and title alignment* decides where the icon and title sit in the title bar. Left puts them right after the buttons, Center puts them in the middle of the window, and Right puts them at the right edge.

| Left | Center | Right |
|------|--------|-------|
| ![Left](https://i.imgur.com/mKIRXkV.png) | ![Center](https://i.imgur.com/w52f183.png) | ![Right](https://i.imgur.com/zDbGFzh.png) |

## 6. Icon Position

*Icon position* decides where the window icon goes. Left or right of the title keeps the icon attached to the title, while far left or far right pins the icon to that edge. To pin the icon to the left edge of the window instead of right after the buttons, turn off *Move caption buttons to the left*. To pin it to the right edge, turn that option on (with it off, a far right icon sits next to the buttons on the right instead). For the best look with an icon pinned to a corner, set *Icon and title alignment* to Center.

| Left of the title | Right of the title |
|-------------------|--------------------|
| ![Left of the title](https://i.imgur.com/w52f183.png) | ![Right of the title](https://i.imgur.com/2WBbHkC.png) |
| **Far left** | **Far right** |
| ![Far left](https://i.imgur.com/pUYiOq3.png) | ![Far right](https://i.imgur.com/ZyPJTJz.png) |

## 7. Hide window icon and title

Use *Hide window icon and title* to hide only the icon, only the title, or both.

| Hide icon only | Hide title only | Hide both |
|----------------|-----------------|-----------|
| ![Hide icon only](https://i.imgur.com/NiBzo8C.png) | ![Hide title only](https://i.imgur.com/PDNFpQb.png) | ![Hide both](https://i.imgur.com/hOEi0Bo.png) |

## 8. Icon responds to click and double-click

By default, Windows treats the window icon as a button: clicking it opens the window menu (Restore, Move, Size, Minimize, Maximize and Close), and double-clicking it closes the window. While this option is on, the menu opens after a short delay (up to 250 ms), so a double-click can be told apart from a single click. This can get in the way when *Icon and title alignment* is set to Center, because the icon then sits in the middle of the title bar, where it's easy to double-click by accident and close the window. Turn off *Icon responds to click and double-click* to prevent that. The icon then behaves like an empty part of the title bar: you can drag the window by it, and double-clicking it maximizes or restores the window.

![Window menu](https://i.imgur.com/dEM6EiY.png)

## 9. File Explorer

By default File Explorer keeps the Windows layout (buttons on the right), because `CabinetWClass` is in *Excluded window classes*.

To put the buttons on the left in File Explorer:

1. Remove `CabinetWClass` from *Excluded window classes*.
2. Keep *Make caption buttons clickable in File Explorer* on (it is on by default). Without it, the tab bar covers the buttons and they cannot be clicked.
3. The tabs still overlap the buttons, so move them with one of these:
   - **Recommended:** the [Windows 11 File Explorer Styler](https://windhawk.net/mods/windows-11-file-explorer-styler) mod by [m417z](https://github.com/m417z), installed separately. It moves only the tabs, so the navigation buttons, address bar and search box stay where they are, right under the caption buttons (target `FileExplorerExtensions.FileExplorerTabControl`, style `Margin=187,0,0,0`; adjust to taste).
   - *Shift tab bar and toolbar (px)* in *File Explorer tabs*. It moves the whole tab bar and toolbar to the right, including the navigation buttons, address bar and search box. About 145 is enough to clear the buttons (less when *Tighten button spacing* is used). A larger value, for example 190, can line the tab bar up with the edge of the navigation pane, so the best value depends on your navigation pane width. The space under the caption buttons, where the navigation buttons used to be, stays empty, which not everyone likes. For that reason the default is 0 (no shift). The value is scaled by display scaling.

| File Explorer caption buttons on the right (default) | File Explorer caption buttons on the left |
|---|---|
| ![File Explorer caption buttons on the right (default)](https://i.imgur.com/rXYTLkE.png) | ![File Explorer caption buttons on the left](https://i.imgur.com/f8lG1yT.png) |

## 10. Excluded apps

Apps that draw their own tabs in the title bar can conflict with buttons on the left. Add their executable names to *Excluded apps* (or their window class names to *Excluded window classes*) and every window of those apps keeps the default Windows layout.

By default *Excluded apps* contains `notepad.exe`, `taskmgr.exe` and `WindowsTerminal.exe`. The icon and title of Task Manager overlap the buttons, and the tabs of Notepad and Windows Terminal overlap the buttons when they are on the left. To see how these apps look with the buttons on the left, remove them from the list. They are not supported yet, so their layout may look broken.

`CabinetWClass` (File Explorer) is in *Excluded window classes* by default, see the File Explorer section above.

## 📁 Files created by the mod

None. Custom images are passed to `dwm.exe` through Windhawk's mod storage, and with *Write debug log* on (off by default) the mod writes detailed messages to the Windhawk log output instead of files. Logs can contain folder paths (including your Windows user name), so check them before sharing.

## ⚠️ Important notes

- When you use the macOS or custom style, drawing the glyphs rewrites a bitmap that DWM shares between all windows. The original glyphs come back **only after dwm.exe restarts**: sign out and in, run `taskkill /f /im dwm.exe` from an administrator prompt, or enable *Restart dwm.exe when the mod is unloaded* (the screen flickers briefly; apps stay open).
- Until dwm.exe restarts, a button can look wrong or be missing after the mod is turned off. For example, the maximize button can go missing if the mod was turned on while a window was maximized and turned off before it was restored.
- dwm.exe is never restarted twice within 30 seconds, and the mod refuses to load if dwm.exe started 4 or more times within a minute.
- After you change display scaling, the screen may flicker briefly while the buttons are redrawn. If the buttons of a window stay blank, click that window once; Windows only creates the active-window button images when a window becomes active.
- The circles or custom images are global. Excluded apps keep them too; only their layout stays at the Windows default.
- When the mod is turned on or off, or when a layout, exclusion, color or style setting changes, DWM is asked to refresh itself so that open windows get the new layout; no window is resized. Changing the debug log does not trigger a refresh.
- If you report a bug, turn on *Write debug log* and attach the Windhawk log output.

## ⚠️ Compatibility and known issues

- **Tested only on Windows 11 25H2 (build 26200.9457), x86-64.** The mod hooks internal, undocumented functions of `uDWM.dll`. On other Windows versions or builds the mod may fail to load or some features may be disabled, and a Windows update can break it until the mod is updated.
- **Display scaling:** tested at 100%, 125%, 150% and 175%. Scaling above 175% and custom scaling values are untested, so the buttons may look wrong there.
- **UWP and other packaged Microsoft apps** (Settings, Calculator, Microsoft Store, Phone Link and some others) are not supported: their caption buttons cannot be moved to the left and stay in their default place on the right. Support is being looked into, but there is no date.
- **Apps that draw their own title bar** (Office apps, Chrome, Edge and others) cannot be changed by this mod. The [Titlebar For Everyone](https://windhawk.net/mods/titlebar-for-everyone) mod by [Ingan121](https://github.com/Ingan121) can make them use the standard title bar.
- **Apps with tabs in the title bar** (Notepad, Windows Terminal, File Explorer) are excluded by default because their tabs overlap buttons on the left. See *Excluded apps* and *File Explorer*.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- buttons:
  - moveButtons: true
    $name: Move caption buttons to the left
    $description: "On = buttons are moved to the left side. Off = buttons stay in their default position on the right."
  - leftMargin: 0
    $name: Left margin (px)
    $description: Gap between the left window edge and the first button
  - buttonTighten: 0
    $name: Tighten button spacing (px)
    $description: "0 = default spacing. Higher = buttons closer together (limited to about 20 px so the middle button stays usable). Works on the left and on the right, and in File Explorer once CabinetWClass is removed from Excluded window classes. Meant for the macOS / Custom image style."
  - order: closeMinMax
    $name: Button order
    $description: Order of the three buttons, from the left edge.
    $options:
    - closeMinMax: Close, minimize, maximize (macOS)
    - closeMaxMin: Close, maximize, minimize
    - minMaxClose: Minimize, maximize, close
  - showDialogClose: true
    $name: Show the close button on dialogs
    $description: "Applies to windows that only have a close button (message boxes, About windows, Properties dialogs). Off = the button is hidden; Alt+F4 and Esc still close the window. Takes effect the next time a window is redrawn."
  $name: Caption buttons
  $description: Position and order of the minimize, maximize and close buttons.
- buttonStyle:
  - style: windows
    $name: Caption button style
    $description: "Windows = the default Windows glyphs (default). macOS = colored circles (the image settings below are ignored). Custom images = your own images; a button without an image keeps its circle. Switching back to Windows only takes effect after dwm.exe restarts (sign out and in)."
    $options:
    - macos: macOS style (circles)
    - custom: Custom images
    - windows: Windows default
  - minimizeColor: "#FEBC2E"
    $name: Minimize color
    $description: "Hex color of the minimize circle, e.g. #FEBC2E. Used by the circles (macOS style, and buttons without an image in the Custom style)."
  - maximizeColor: "#28C840"
    $name: Maximize / restore color
    $description: "Hex color of the maximize/restore circle, e.g. #28C840. Used by the circles (macOS style, and buttons without an image in the Custom style)."
  - closeColor: "#FF5F57"
    $name: Close color
    $description: "Hex color of the close circle, e.g. #FF5F57. Used by the circles (macOS style, and buttons without an image in the Custom style)."
  - iconFolder: ""
    $name: Icon folder
    $description: "Only used with the Custom images style. Folder with minimize, restore, maximize and close images (.png, .jpg, .bmp). Optional state variants: close_hover, close_pressed, close_disabled. Any folder you can read works, e.g. C:\\Users\\You\\Pictures\\ButtonIcons"
  - minimizeIcon: ""
    $name: Minimize image (file)
    $description: Optional. Full path of an image file; overrides the folder for this button.
  - maximizeIcon: ""
    $name: Maximize image (file)
    $description: Optional. Full path of an image file; overrides the folder for this button.
  - restoreIcon: ""
    $name: Restore image (file)
    $description: Optional. Full path of an image file; overrides the folder for this button. Used when the window is maximized.
  - closeIcon: ""
    $name: Close image (file)
    $description: Optional. Full path of an image file; overrides the folder for this button.
  - restartDwmOnUnload: false
    $name: Restart dwm.exe when the mod is unloaded
    $description: "Off (recommended): the painted glyphs stay until dwm.exe restarts, so after the mod is turned off a button can look wrong or be missing. On: dwm.exe is terminated when the mod is unloaded (disable, recompile, Windhawk restart) so the original glyphs come back; the screen flickers briefly and apps stay open. dwm.exe is never restarted twice within 30 seconds."
  $name: Button style
  $description: Choose how the caption buttons look - macOS circles, your own images, or the Windows default.
- titleText:
  - titleAlign: left
    $name: Icon and title alignment
    $description: "Where the icon and title sit in the title bar. Left = right after the buttons."
    $options:
    - left: Left
    - center: Center
    - right: Right
  - dialogAlign: left
    $name: Icon and title alignment in dialogs
    $description: "Same as above, but only for windows that have just a close button (message boxes, About windows, Properties dialogs). This setting always applies to those windows instead of the general alignment."
    $options:
    - left: Left
    - center: Center
    - right: Right
  - centerOnText: true
    $name: Center the title text itself
    $description: "On = the title text is centered on the window (icon sits beside it). Off = the icon + title group is centered."
  - centerNudge: 0
    $name: Center fine-tune (px)
    $description: Positive moves the centered title right, negative moves it left
  - iconPosition: beforeTitle
    $name: Icon position
    $description: "Where the window icon goes. Left/Right of the title keeps the icon attached to the title. Far left / Far right pins the icon to that edge, so the title can be aligned or centered on its own."
    $options:
    - beforeTitle: Left of the title
    - afterTitle: Right of the title
    - farLeft: Far left
    - farRight: Far right
  - titleGap: 8
    $name: Gap after buttons (px)
    $description: Space between the last button and the icon/title group.
  - iconTitleGap: 5
    $name: Gap between icon and title (px)
    $description: Space between the icon and the title text
  - rightEdgeGap: 8
    $name: Right edge gap (px)
    $description: Space between the right edge of the window and the icon/title group.
  - hideMode: off
    $name: Hide window icon and title
    $description: Choose what to hide in the title bar.
    $options:
    - off: Off (show both)
    - icon: Hide icon only
    - title: Hide title only
    - both: Hide both
  - iconClickable: true
    $name: Icon responds to click and double-click
    $description: "On = default Windows behavior: clicking the icon opens the window menu and double-clicking it closes the window. Off = the icon acts like an empty part of the title bar (drag, double-click to maximize or restore)."
  $name: Icon and title text
  $description: Where the window icon and the title text are placed.
- exclusions:
  - excludedApps: "notepad.exe, taskmgr.exe, WindowsTerminal.exe"
    $name: Excluded apps (process names)
    $description: "Comma-separated executable names. Every window of these apps keeps the default Windows layout. Example: notepad.exe, msedge.exe"
  - excludedClasses: "CabinetWClass"
    $name: Excluded window classes
    $description: "Comma-separated window class names, for excluding only specific windows of an app. CabinetWClass (File Explorer) is in this list by default, so File Explorer keeps the default Windows layout with the buttons on the right. Remove it from the list if you want File Explorer to use the left buttons. Other classes: Notepad, CASCADIA_HOSTING_WINDOW_CLASS (Windows Terminal)"
  $name: Excluded apps
  $description: Apps with their own tabs in the title bar conflict with buttons on the left.
- explorerTabs:
  - enabled: true
    $name: Make caption buttons clickable in File Explorer
    $description: "On = the caption buttons can be clicked in File Explorer (the area under them is cut out of the tab bar window). Use the shift setting below to also move the tab bar and toolbar. Only works while Move caption buttons to the left is on and CabinetWClass is not excluded."
  - buttonsExtraMargin: 5
    $name: Extra left margin for File Explorer buttons (px)
    $description: "Moves the caption buttons right, in File Explorer only. About 5 aligns the circles with the nav arrows; about 28 centers them over the four nav buttons."
  - extraPx: 0
    $name: Shift tab bar and toolbar (px)
    $description: "0 = leave the tab bar and toolbar of File Explorer where they are (shift only the tabs with the Windows 11 File Explorer Styler mod, see the readme). A positive value moves the whole tab bar and toolbar to the right, scaled by DPI; about 145 clears the three buttons."
  $name: File Explorer tabs
  $description: Fixes the tab bar of File Explorer blocking the buttons on the left.
- debugLog: false
  $name: Write debug log
  $description: Writes detailed debug messages to the Windhawk log output. Turn on logging for this mod in Windhawk to see them.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <wincodec.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <regex>
#include <string_view>
#include <unordered_map>

// ======================================================================
//  PART 1: Layout (caption buttons on the left, icon and title layout)
// ======================================================================

static struct {
    int leftMargin, order, tighten;
    bool moveButtons, moveTitle;
    int titleAlign, iconPosition, iconSide, titleGap, iconTitleGap, rightEdgeGap;
    int explorerMargin;
    bool centerOnText;
    int centerNudge;
    bool hideIcon, hideTitle, debugLog;
    bool showDialogClose;
    bool iconClickable;
    int dialogAlign;
} g_settings;

// Alignment correction (px) for windows that only have a close button
// (About Windows, message boxes, Run, ...). Not user-configurable.
static const int kCloseOnlyOffset = -1;

// Set while the mod is being unloaded: layouts are no longer changed, windows go back to default
static volatile LONG g_unloading = 0;
static volatile LONG g_unloadRelayouts = 0;

static int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

// Left margin of the caption buttons; File Explorer gets an extra margin.
static int EffLeftMargin(HWND hwnd) {
    int m = g_settings.leftMargin;
    if (hwnd) {
        wchar_t c[64] = L"";
        GetClassNameW(hwnd, c, 64);
        if (wcscmp(c, L"CabinetWClass") == 0) m += g_settings.explorerMargin;
    }
    return m;
}

// Overlap (px) between neighbouring caption buttons for the given DPI.
// midW = width of the middle button; the overlap is capped so it stays usable.
static int TightenPx(int dpi, int midW) {
    if (g_settings.tighten <= 0) return 0;
    int T = MulDiv(g_settings.tighten, dpi, 96);
    int maxT = (midW - 6) / 2;
    if (T > maxT) T = maxT;
    return T < 0 ? 0 : T;
}

// True when the layout depends on the measured width of the title text
// (centered/right title, or the icon placed to the right of the title).
static bool TitleWidthMatters() {
    return g_settings.titleAlign != 0 || g_settings.dialogAlign >= 2 ||
           (g_settings.iconPosition == 0 && g_settings.iconSide == 1);
}

// Keys of the dropdown settings, in the same order as the numbers used in the code
static const wchar_t* const kOrderKeys[]   = { L"closeMinMax", L"closeMaxMin", L"minMaxClose" };
static const wchar_t* const kAlignKeys[]   = { L"left", L"center", L"right" };
static const wchar_t* const kIconPosKeys[] = { L"beforeTitle", L"afterTitle", L"farLeft", L"farRight" };

// Reads a dropdown setting and returns the index of its key in 'keys' (0 if unknown)
static int GetChoice(const wchar_t* setting, const wchar_t* const* keys, int n) {
    PCWSTR v = Wh_GetStringSetting(setting);
    int r = 0;
    if (v) {
        for (int i = 0; i < n; i++) {
            if (wcscmp(v, keys[i]) == 0) { r = i; break; }
        }
    }
    Wh_FreeStringSetting(v);
    return r;
}

static void LoadExclusions();

static void LoadSettings() {
    g_settings.moveButtons = Wh_GetIntSetting(L"buttons.moveButtons") != 0;
    g_settings.leftMargin = Clamp(Wh_GetIntSetting(L"buttons.leftMargin"), 0, 300);
    g_settings.tighten = Clamp(Wh_GetIntSetting(L"buttons.buttonTighten"), 0, 40);
    g_settings.explorerMargin = Clamp(Wh_GetIntSetting(L"explorerTabs.buttonsExtraMargin"), -50, 300);
    g_settings.order = GetChoice(L"buttons.order", kOrderKeys, 3);
    g_settings.showDialogClose = Wh_GetIntSetting(L"buttons.showDialogClose") != 0;

    g_settings.moveTitle = true;  // fixed: icon/title are always repositioned
    g_settings.titleAlign = GetChoice(L"titleText.titleAlign", kAlignKeys, 3);
    // dialogAlign is stored as 1..3 (0 would mean "same as general", no longer offered)
    g_settings.dialogAlign = GetChoice(L"titleText.dialogAlign", kAlignKeys, 3) + 1;
    g_settings.centerOnText = Wh_GetIntSetting(L"titleText.centerOnText") != 0;
    g_settings.centerNudge = Clamp(Wh_GetIntSetting(L"titleText.centerNudge"), -300, 300);

    // One dropdown, two internal values:
    // iconPosition: 0 = next to the title, 1 = far left, 2 = far right
    // iconSide: 0 = icon left of the title, 1 = icon right of the title
    int ip = GetChoice(L"titleText.iconPosition", kIconPosKeys, 4);
    g_settings.iconPosition = (ip <= 1) ? 0 : ip - 1;
    g_settings.iconSide = (ip == 1) ? 1 : 0;

    g_settings.titleGap = Clamp(Wh_GetIntSetting(L"titleText.titleGap"), 0, 300);
    g_settings.iconTitleGap = Clamp(Wh_GetIntSetting(L"titleText.iconTitleGap"), 0, 100);
    g_settings.rightEdgeGap = Clamp(Wh_GetIntSetting(L"titleText.rightEdgeGap"), 0, 100);

    // Hide dropdown: off / icon / title / both
    PCWSTR hm = Wh_GetStringSetting(L"titleText.hideMode");
    g_settings.hideIcon = hm && (wcscmp(hm, L"icon") == 0 || wcscmp(hm, L"both") == 0);
    g_settings.hideTitle = hm && (wcscmp(hm, L"title") == 0 || wcscmp(hm, L"both") == 0);
    Wh_FreeStringSetting(hm);

    g_settings.iconClickable = Wh_GetIntSetting(L"titleText.iconClickable") != 0;
    g_settings.debugLog = Wh_GetIntSetting(L"debugLog") != 0;

    LoadExclusions();
}

// ---------------- logging ----------------
static int g_logCount = 0;
static int g_hitLogCount = 0;
static wchar_t g_lastLog[800] = L"";
static wchar_t g_lastHitLog[300] = L"";

static void WriteLogLine(const wchar_t* line) {
    Wh_Log(L"%s", line);
}

static void DebugLog(const wchar_t* fmt, ...) {
    if (!g_settings.debugLog || g_logCount >= 600) return;
    wchar_t buf[800];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf(buf, 799, fmt, args);
    va_end(args);
    buf[799] = 0;
    if (wcscmp(buf, g_lastLog) == 0) return;
    wcscpy(g_lastLog, buf);
    g_logCount++;
    WriteLogLine(buf);
}

static void HitLog(const wchar_t* fmt, ...) {
    if (!g_settings.debugLog || g_hitLogCount >= 200) return;
    wchar_t buf[300];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf(buf, 299, fmt, args);
    va_end(args);
    buf[299] = 0;
    if (wcscmp(buf, g_lastHitLog) == 0) return;
    wcscpy(g_lastHitLog, buf);
    g_hitLogCount++;
    WriteLogLine(buf);
}

// ---------------- excluded apps ----------------
// Two lists: process names (e.g. notepad.exe) and window class names (e.g. CabinetWClass).
// Classes allow excluding specific windows only: explorer.exe hosts File Explorer
// (CabinetWClass) but also the Run/Properties dialogs (#32770).
static SRWLOCK g_exclLock = SRWLOCK_INIT;
static wchar_t g_exclExe[32][128];
static int g_exclExeN = 0;
static wchar_t g_exclCls[32][128];
static int g_exclClsN = 0;

// Per-HWND result cache (pid is checked too, so a reused HWND is not misjudged)
struct ExclCache { HWND hwnd; DWORD pid; bool excluded; };
static ExclCache g_exclCache[128];
static int g_exclCacheNext = 0;

// Split "a, b; c" into a list. exeNames = true: strip the path and add ".exe" when there is no dot.
static int ParseList(PCWSTR s, wchar_t (*out)[128], int maxN, bool exeNames) {
    int n = 0;
    if (!s) return 0;
    const wchar_t* p = s;
    while (*p && n < maxN) {
        while (*p == L' ' || *p == L',' || *p == L';' || *p == L'\t' || *p == L'\r' || *p == L'\n') p++;
        const wchar_t* st = p;
        while (*p && *p != L',' && *p != L';' && *p != L'\r' && *p != L'\n') p++;
        const wchar_t* en = p;
        while (en > st && (en[-1] == L' ' || en[-1] == L'\t')) en--;
        int len = (int)(en - st);
        if (len <= 0) continue;
        if (len > 120) len = 120;
        memcpy(out[n], st, (size_t)len * sizeof(wchar_t));
        out[n][len] = 0;
        if (exeNames) {
            wchar_t* b = wcsrchr(out[n], L'\\');
            if (b) memmove(out[n], b + 1, (wcslen(b + 1) + 1) * sizeof(wchar_t));
            if (!wcschr(out[n], L'.')) wcsncat(out[n], L".exe", 127 - wcslen(out[n]));
        }
        n++;
    }
    return n;
}

// Exe name from a pid. OpenProcess is often denied for dwm.exe, so fall back to a process snapshot.
static bool GetExeNameForPid(DWORD pid, wchar_t* out, size_t cap) {
    out[0] = 0;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) {
        wchar_t path[MAX_PATH];
        DWORD len = MAX_PATH;
        bool ok = QueryFullProcessImageNameW(h, 0, path, &len) != FALSE;
        CloseHandle(h);
        if (ok) {
            const wchar_t* exe = wcsrchr(path, L'\\');
            exe = exe ? exe + 1 : path;
            wcsncpy(out, exe, cap - 1);
            out[cap - 1] = 0;
            return true;
        }
    }
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32W pe = {};
    pe.dwSize = sizeof(pe);
    bool found = false;
    if (Process32FirstW(snap, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                wcsncpy(out, pe.szExeFile, cap - 1);
                out[cap - 1] = 0;
                found = true;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return found;
}

static void LoadExclusions() {
    PCWSTR a = Wh_GetStringSetting(L"exclusions.excludedApps");
    PCWSTR c = Wh_GetStringSetting(L"exclusions.excludedClasses");
    AcquireSRWLockExclusive(&g_exclLock);
    g_exclExeN = ParseList(a, g_exclExe, 32, true);
    g_exclClsN = ParseList(c, g_exclCls, 32, false);
    memset(g_exclCache, 0, sizeof(g_exclCache));
    g_exclCacheNext = 0;
    int nExe = g_exclExeN, nCls = g_exclClsN;
    ReleaseSRWLockExclusive(&g_exclLock);
    Wh_FreeStringSetting(a);
    Wh_FreeStringSetting(c);
    DebugLog(L"[Exclusions] process list=%d class list=%d", nExe, nCls);
}

static bool IsHwndExcluded(HWND hwnd) {
    if (!hwnd) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return false;

    AcquireSRWLockShared(&g_exclLock);
    int nExe = g_exclExeN, nCls = g_exclClsN;
    for (int i = 0; i < 128; i++) {
        if (g_exclCache[i].hwnd == hwnd && g_exclCache[i].pid == pid) {
            bool r = g_exclCache[i].excluded;
            ReleaseSRWLockShared(&g_exclLock);
            return r;
        }
    }
    ReleaseSRWLockShared(&g_exclLock);
    if (nExe == 0 && nCls == 0) return false;

    wchar_t cls[128] = L"";
    GetClassNameW(hwnd, cls, 128);
    wchar_t exe[128] = L"";
    bool haveExe = (nExe > 0) && GetExeNameForPid(pid, exe, 128);

    bool excluded = false;
    AcquireSRWLockExclusive(&g_exclLock);
    if (haveExe) {
        for (int i = 0; i < g_exclExeN && !excluded; i++)
            if (_wcsicmp(exe, g_exclExe[i]) == 0) excluded = true;
    }
    for (int i = 0; i < g_exclClsN && !excluded; i++)
        if (_wcsicmp(cls, g_exclCls[i]) == 0) excluded = true;
    ExclCache& e = g_exclCache[g_exclCacheNext];
    g_exclCacheNext = (g_exclCacheNext + 1) % 128;
    e.hwnd = hwnd;
    e.pid = pid;
    e.excluded = excluded;
    ReleaseSRWLockExclusive(&g_exclLock);

    DebugLog(L"[ExclCheck] hwnd=%p pid=%lu exe=%s class=%s excluded=%d", hwnd, (unsigned long)pid, exe, cls, (int)excluded);
    return excluded;
}

// ---------------- dynamic APIs ----------------
typedef UINT (WINAPI *GetDpiForWindow_t)(HWND);
typedef UINT (WINAPI *GetDpiForSystem_t)(void);
typedef BOOL (WINAPI *SPIForDpi_t)(UINT, UINT, PVOID, UINT, UINT);
static GetDpiForWindow_t g_pGetDpiForWindow = nullptr;
static GetDpiForSystem_t g_pGetDpiForSystem = nullptr;
static SPIForDpi_t g_pSPIForDpi = nullptr;
typedef int  (WINAPI *InternalGetWindowText_t)(HWND, LPWSTR, int);
typedef BOOL (WINAPI *IsHungAppWindow_t)(HWND);
static InternalGetWindowText_t g_pInternalGetWindowText = nullptr;
static IsHungAppWindow_t g_pIsHungAppWindow = nullptr;

static const unsigned kHashUnsafe = 0xFFFFFFFFu;

// Reads the title WITHOUT sending any message to the window (safe on the DWM thread).
// Returns -1 when the window is hung or the API is unavailable.
static int SafeGetWindowText(HWND hwnd, wchar_t* buf, int cap) {
    if (!hwnd || !g_pInternalGetWindowText) return -1;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    // Ghost windows belong to dwm.exe itself: reading their text never blocks.
    if (pid != GetCurrentProcessId() && g_pIsHungAppWindow && g_pIsHungAppWindow(hwnd)) return -1;
    return g_pInternalGetWindowText(hwnd, buf, cap);
}

static int MeasureTitleWidth(HWND hwnd, int dpi) {
    if (!hwnd || !g_pSPIForDpi) return -1;
    wchar_t text[300] = {};
    int n = SafeGetWindowText(hwnd, text, 300);
    if (n < 0) return -1;   // hung / unavailable: caller falls back to the safe left layout
    if (n == 0) return 0;
    NONCLIENTMETRICSW ncm = {};
    ncm.cbSize = sizeof(ncm);
    if (!g_pSPIForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi)) return -1;
    HFONT font = CreateFontIndirectW(&ncm.lfCaptionFont);
    if (!font) return -1;
    int result = -1;
    HDC dc = CreateCompatibleDC(nullptr);
    if (dc) {
        HGDIOBJ old = SelectObject(dc, font);
        SIZE sz = {};
        if (GetTextExtentPoint32W(dc, text, n, &sz)) result = sz.cx;
        SelectObject(dc, old);
        DeleteDC(dc);
    }
    DeleteObject(font);
    return result;
}

static unsigned HashWindowText(HWND hwnd) {
    if (!hwnd) return 0;
    wchar_t t[300] = {};
    int n = SafeGetWindowText(hwnd, t, 300);
    if (n < 0) return kHashUnsafe;
    unsigned h = 2166136261u;
    for (int i = 0; i < n; i++) {
        h ^= (unsigned)t[i];
        h *= 16777619u;
    }
    return h ^ (unsigned)n;
}

// ---------------- original function pointers ----------------
typedef void (*GetTitleBarInfo_t)(void*, TITLEBARINFOEX*, bool*);
typedef long (*UpdateNCAreaButton_t)(void*, int, int, int, int*);
typedef long (*PosAndSizes_t)(void*);
typedef void (*SetInset_t)(void*, int);
typedef bool (*DoHitTest_t)(void*, const POINT*, void**, unsigned int*);

static GetTitleBarInfo_t GetTitleBarInfo_Orig;
static UpdateNCAreaButton_t UpdateNCAreaButton_Orig;
static PosAndSizes_t PosAndSizes_Orig;
static SetInset_t SetInsetTop_Orig, SetInsetLeft_Orig, SetInsetRight_Orig;
static DoHitTest_t DoHitTest_Orig;
static bool g_hasPosHook = false;

// ---------------- HWND of a CTopLevelWindow, taken from DWM's own data ----------------
// CTopLevelWindow::GetWindowData() returns a CWindowData*, which stores the HWND.
// The offset of that member is derived at init from CWindowData::IsGhostWindow
// (see DeriveHwndOffset); if it can't be derived, no HWND is available.
typedef void* (WINAPI *GetWindowData_t)(void*);
static GetWindowData_t GetWindowData_Orig;
static void* IsGhostWindow_Func;
static size_t g_hwndOffset = SIZE_MAX;

static size_t OffsetFromAssemblyRegex(void* func, size_t defValue, const std::regex& re, int limit) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) break;
        p += result.length;
        std::string_view s = result.text;
        if (s == "ret") break;
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, re)) {
            return std::stoull(match[1], nullptr, 16);
        }
    }
    Wh_Log(L"HWND offset: pattern not found in %p", func);
    return defValue;
}

static void DeriveHwndOffset() {
    if (!IsGhostWindow_Func || !GetWindowData_Orig) {
        Wh_Log(L"HWND lookup unavailable: GetWindowData or IsGhostWindow symbol not found");
        return;
    }
    // IsGhostWindow loads this->hwnd as its first argument to GetPropW, so the first
    // load whose base register is the `this` pointer (rcx on x64, x0 on ARM64) is the HWND member.
    g_hwndOffset = OffsetFromAssemblyRegex(
        IsGhostWindow_Func, SIZE_MAX,
#if defined(_M_X64)
        std::regex(R"(mov \w+, \[rcx\+0x([0-9a-f]+)\])", std::regex_constants::icase),
#elif defined(_M_ARM64)
        std::regex(R"(ldr\s+\w+, \[x0, #0x([0-9a-f]+)\])", std::regex_constants::icase),
#else
#error "Unsupported architecture"
#endif
        10);
    Wh_Log(L"CWindowData HWND offset = 0x%zx", g_hwndOffset);
}

static HWND HwndFromTopLevelWindow(void* pThis) {
    if (g_hwndOffset == SIZE_MAX || !GetWindowData_Orig || !pThis) return nullptr;
    void* data = GetWindowData_Orig(pThis);
    if (!data) return nullptr;
    HWND h = *(HWND*)((BYTE*)data + g_hwndOffset);
    return IsWindow(h) ? h : nullptr;
}

// ---------------- per-window button rectangles (window-local X) ----------------
struct WinLayout { HWND hwnd; int left[4]; int right[4]; int band; bool valid; bool iconSet; int iconL, iconR; };
// One entry per CTopLevelWindow that this mod has laid out. No size limit: an entry is
// only dropped when its window is gone (see StoreWinLayout).
static std::unordered_map<void*, WinLayout> g_winLayouts;
static SRWLOCK g_layoutLock = SRWLOCK_INIT;

static void StoreWinLayout(void* win, const int* l, const int* r, int band, HWND hwnd) {
    AcquireSRWLockExclusive(&g_layoutLock);
    auto it = g_winLayouts.find(win);
    if (it == g_winLayouts.end()) {
        // A new window: forget the entries of windows that are gone
        for (auto p = g_winLayouts.begin(); p != g_winLayouts.end();) {
            if (!p->second.valid || (p->second.hwnd && !IsWindow(p->second.hwnd)))
                p = g_winLayouts.erase(p);
            else
                ++p;
        }
        it = g_winLayouts.emplace(win, WinLayout{}).first;
    }
    WinLayout& e = it->second;
    for (int t = 1; t <= 3; t++) { e.left[t] = l[t]; e.right[t] = r[t]; }
    e.hwnd = hwnd;
    e.band = band;
    e.valid = true;
    e.iconSet = false; e.iconL = 0; e.iconR = 0;
    ReleaseSRWLockExclusive(&g_layoutLock);
}

static void ClearWinLayout(void* win) {
    AcquireSRWLockExclusive(&g_layoutLock);
    auto it = g_winLayouts.find(win);
    if (it != g_winLayouts.end()) it->second.valid = false;
    ReleaseSRWLockExclusive(&g_layoutLock);
}

static bool GetWinLayout(void* win, int* l, int* r, int* band) {
    bool ok = false;
    AcquireSRWLockShared(&g_layoutLock);
    auto it = g_winLayouts.find(win);
    if (it != g_winLayouts.end() && it->second.valid) {
        for (int t = 1; t <= 3; t++) { l[t] = it->second.left[t]; r[t] = it->second.right[t]; }
        *band = it->second.band;
        ok = true;
    }
    ReleaseSRWLockShared(&g_layoutLock);
    return ok;
}

static void SetWinIcon(void* win, bool visible, int l, int r) {
    AcquireSRWLockExclusive(&g_layoutLock);
    auto it = g_winLayouts.find(win);
    if (it != g_winLayouts.end()) {
        it->second.iconSet = true;
        it->second.iconL = visible ? l : 0;
        it->second.iconR = visible ? r : 0;
    }
    ReleaseSRWLockExclusive(&g_layoutLock);
}

static bool GetWinIcon(void* win, int* l, int* r) {
    bool set = false;
    AcquireSRWLockShared(&g_layoutLock);
    auto it = g_winLayouts.find(win);
    if (it != g_winLayouts.end() && it->second.valid && it->second.iconSet) {
        *l = it->second.iconL; *r = it->second.iconR; set = true;
    }
    ReleaseSRWLockShared(&g_layoutLock);
    return set;
}

static HWND GetWinHwnd(void* win) {
    HWND h = nullptr;
    AcquireSRWLockShared(&g_layoutLock);
    auto it = g_winLayouts.find(win);
    if (it != g_winLayouts.end() && it->second.valid) h = it->second.hwnd;
    ReleaseSRWLockShared(&g_layoutLock);
    return h;
}

// ---------------- per-window title-text state ----------------
struct HwndCache { void* win; HWND hashHwnd; unsigned hash; bool hasHash; };
static HwndCache g_hwndCache[64];
static int g_hwndCacheNext = 0;
static SRWLOCK g_hwndLock = SRWLOCK_INIT;

static HwndCache* FindOrCreateCacheLocked(void* win) {
    for (int i = 0; i < 64; i++) {
        if (g_hwndCache[i].win == win) return &g_hwndCache[i];
    }
    HwndCache* e = &g_hwndCache[g_hwndCacheNext];
    g_hwndCacheNext = (g_hwndCacheNext + 1) % 64;
    *e = HwndCache{};
    e->win = win;
    return e;
}

static void SetTitleState(void* win, HWND hashHwnd, unsigned hash) {
    AcquireSRWLockExclusive(&g_hwndLock);
    HwndCache* e = FindOrCreateCacheLocked(win);
    e->hashHwnd = hashHwnd;
    e->hash = hash;
    e->hasHash = true;
    ReleaseSRWLockExclusive(&g_hwndLock);
}

static bool GetTitleState(void* win, HWND* hashHwnd, unsigned* hash) {
    bool ok = false;
    AcquireSRWLockShared(&g_hwndLock);
    for (int i = 0; i < 64; i++) {
        if (g_hwndCache[i].win == win && g_hwndCache[i].hasHash) {
            *hashHwnd = g_hwndCache[i].hashHwnd;
            *hash = g_hwndCache[i].hash;
            ok = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_hwndLock);
    return ok;
}

// ---------------- per-sequence state ----------------
struct VisEntry { void* v; int top; int left; bool hasTop; bool hasLeft; };

struct Seq {
    bool valid;
    bool active;
    bool done;
    bool closeOnly;
    int outBefore[4];
    int w[4];
    int origFinalOut;
    int buttonEnd;
    int rightMargin;
    int winW;
    int dpi;
    HWND hwnd;
    void* win;
    int count;
    VisEntry vis[6];
};

static thread_local Seq t_seq;
static thread_local int t_posDepth;
static thread_local bool t_relayout;
static void* g_lastTitleVis = nullptr;

static VisEntry* FindEntry(void* v) {
    for (int i = 0; i < t_seq.count; i++)
        if (t_seq.vis[i].v == v) return &t_seq.vis[i];
    return nullptr;
}

static VisEntry* GetEntry(void* v) {
    VisEntry* e = FindEntry(v);
    if (e) return e;
    if (t_seq.count >= 6) return nullptr;
    e = &t_seq.vis[t_seq.count++];
    *e = VisEntry{};
    e->v = v;
    return e;
}

// ---------------- title/icon repositioning ----------------
// Alignment for the window being laid out: close-only windows (dialogs) can
// override the general setting. dialogAlign: 0 = general, 1..3 = left/center/right.
static int EffTitleAlign() {
    if (t_seq.closeOnly && g_settings.dialogAlign > 0) return g_settings.dialogAlign - 1;
    return g_settings.titleAlign;
}

static bool RepositionTitle(VisEntry* title) {
    VisEntry* icon = nullptr;
    for (int i = 0; i < t_seq.count; i++) {
        VisEntry* e = &t_seq.vis[i];
        if (e != title && e->hasLeft) { icon = e; break; }
    }
    g_lastTitleVis = title->v;

    int dpi = t_seq.dpi > 0 ? t_seq.dpi : 96;
    int iconW = MulDiv(16, dpi, 96);
    bool origHasIcon = icon && (title->left > icon->left + 4);

    int aL = t_seq.buttonEnd + g_settings.titleGap;
    int aR = t_seq.winW - t_seq.rightMargin - g_settings.rightEdgeGap;
    int gap = g_settings.iconTitleGap;

    // Very narrow window: no room next to the buttons -> hide icon and title
    bool noRoom = (aR - aL) < 24;
    bool showIcon = origHasIcon && !g_settings.hideIcon && !noRoom;
    bool showTitle = !g_settings.hideTitle && !noRoom;

    // Icon position: 0 = next to the title, 1 = far left, 2 = far right.
    // A "far" icon is detached from the title, so the title can be centered alone.
    int iconPos = g_settings.iconPosition;
    bool iconFar = showIcon && iconPos != 0;
    bool iconInGroup = showIcon && !iconFar;

    // Area available for the title (and the icon, when it sits next to it)
    int tL = aL + ((iconFar && iconPos == 1) ? iconW + gap : 0);
    int tR = aR - ((iconFar && iconPos == 2) ? iconW + gap : 0);

    int availText = tR - tL - (iconInGroup ? iconW + gap : 0);
    if (availText < 0) availText = 0;
    if (showTitle && availText < 12) showTitle = false;  // too small to show anything useful

    int align = EffTitleAlign();
    int side = iconInGroup ? g_settings.iconSide : 0;
    int textW = availText;
    bool measured = false;

    if (showTitle && (align != 0 || side == 1)) {
        int m = MeasureTitleWidth(t_seq.hwnd, dpi);
        if (m >= 0) {
            textW = m + 4;
            if (textW > availText) textW = availText;
            measured = true;
        } else {
            // Text width unknown: use the safe layout instead of guessing.
            align = 0;
            side = 0;
        }
    }

    int groupW = (iconInGroup ? iconW : 0)
               + ((iconInGroup && showTitle) ? gap : 0)
               + (showTitle ? textW : 0);

    int cx = t_seq.winW / 2 + g_settings.centerNudge;  // window center
    int gx = tL;
    if (align == 1) {
        if (g_settings.centerOnText && showTitle) {
            // Center the text itself; the icon (if attached) sits beside it
            int textX = cx - textW / 2;
            gx = textX - ((iconInGroup && side == 0) ? iconW + gap : 0);
        } else {
            gx = cx - groupW / 2;
        }
        if (gx + groupW > tR) gx = tR - groupW;
        if (gx < tL) gx = tL;
    } else if (align == 2) {
        gx = tR - groupW;
        if (gx < tL) gx = tL;
    }

    int iconX, titleX;
    if (iconInGroup) {
        if (side == 0) {
            iconX = gx;
            titleX = gx + iconW + gap;
        } else {
            titleX = gx;
            iconX = gx + (showTitle ? textW + gap : 0);
        }
    } else {
        titleX = gx;
        iconX = (iconPos == 2) ? aR - iconW : aL;
    }
    int titleRightEdge = (side == 0) ? tR : titleX + textW;

    if (icon) {
        if (g_settings.hideIcon || noRoom) SetInsetLeft_Orig(icon->v, t_seq.winW + 200);
        else if (showIcon) SetInsetLeft_Orig(icon->v, iconX);
    }

    if (icon) SetWinIcon(t_seq.win, showIcon, iconX, iconX + iconW);
    if (!showTitle) {
        SetInsetLeft_Orig(title->v, t_seq.winW + 100);
        SetInsetRight_Orig(title->v, -200);
    } else {
        SetInsetLeft_Orig(title->v, titleX);
        SetInsetRight_Orig(title->v, t_seq.winW - titleRightEdge);
    }

    DebugLog(L"[Title] vis=%p W=%d buttonEnd=%d aL=%d aR=%d iconW=%d textW=%d measured=%d hwnd=%p align=%d side=%d noRoom=%d cx=%d origIconLeft=%d origTitleLeft=%d -> iconX=%d titleX=%d titleRightEdge=%d showIcon=%d showTitle=%d",
             title->v, t_seq.winW, t_seq.buttonEnd, aL, aR, iconW, textW, (int)measured, t_seq.hwnd, align, side,
             (int)noRoom, cx, icon ? icon->left : -1, title->left, iconX, titleX, titleRightEdge,
             (int)showIcon, (int)showTitle);
    return true;
}

// ---------------- CVisual inset hooks ----------------
static void SetInsetTop_Hook(void* v, int value) {
    if (t_seq.active && !t_seq.done) {
        if (VisEntry* e = GetEntry(v)) { e->top = value; e->hasTop = true; }
    }
    SetInsetTop_Orig(v, value);
}

static void SetInsetLeft_Hook(void* v, int value) {
    // Diagnostic: someone else changing the title's position outside our sequence
    if (g_settings.debugLog && v == g_lastTitleVis && !(t_seq.active && !t_seq.done)) {
        DebugLog(L"[LateInset] vis=%p Left=%d posDepth=%d relayout=%d", v, value, t_posDepth, (int)t_relayout);
    }
    if (t_seq.active && !t_seq.done) {
        if (VisEntry* e = GetEntry(v)) { e->left = value; e->hasLeft = true; }
    }
    SetInsetLeft_Orig(v, value);
}

static void SetInsetRight_Hook(void* v, int value) {
    if (g_settings.debugLog && v == g_lastTitleVis && !(t_seq.active && !t_seq.done)) {
        DebugLog(L"[LateInset] vis=%p Right=%d posDepth=%d relayout=%d", v, value, t_posDepth, (int)t_relayout);
    }
    if (t_seq.active && !t_seq.done) {
        VisEntry* e = FindEntry(v);
        if (e && e->hasLeft && abs(value - t_seq.origFinalOut) <= 3) {
            t_seq.done = true;
            if (RepositionTitle(e)) return;
        }
    }
    SetInsetRight_Orig(v, value);
}

// ---------------- layout ----------------
static bool ComputeLayout(void* pThis, int a, int b, int baseOut) {
    struct { TITLEBARINFOEX t; BYTE pad[256]; } buf = {};
    buf.t.cbSize = sizeof(TITLEBARINFOEX);
    bool flag = false;
    GetTitleBarInfo_Orig(pThis, &buf.t, &flag);

    const RECT& tb = buf.t.rcTitleBar;
    int tbW = tb.right - tb.left;
    if (tbW <= 0 || tbW > 20000) return false;
    if (tb.left < -20000) return false;  // minimized / offscreen representation (thumbnail)

    int expectedInset = (b == 8) ? 8 : baseOut + 1;

    // Window width: estimated from the title bar, then corrected with the real
    // rect of the HWND that DWM itself stores for this window. The tolerance
    // guards against the rect being out of sync with DWM mid-resize.
    int winW = tbW + 2 * expectedInset;
    HWND hwndUsed = HwndFromTopLevelWindow(pThis);
    if (hwndUsed) {
        RECT wr;
        if (GetWindowRect(hwndUsed, &wr) && abs((wr.right - wr.left) - winW) <= 40) {
            winW = wr.right - wr.left;
        }
    }

    // The HWND is only needed to measure the title text.
    bool needHwnd = g_settings.moveTitle && !g_settings.hideTitle &&
                    TitleWidthMatters();

    // Excluded apps keep the default Windows layout (buttons on the right).
    // Returning false here is the same as any other layout failure.
    if (hwndUsed && IsHwndExcluded(hwndUsed)) {
        DebugLog(L"[Excluded] this=%p hwnd=%p -> default layout", pThis, hwndUsed);
        return false;
    }

    // Remember which title text this layout was computed for
    if (needHwnd && hwndUsed) SetTitleState(pThis, hwndUsed, HashWindowText(hwndUsed));

    int w[4] = {};
    for (int t = 1; t <= 3; t++) {
        int tmp = 0;
        UpdateNCAreaButton_Orig(pThis, t, a, b, &tmp);
        w[t] = tmp;
        if (w[t] < 0 || w[t] > 300) {
            DebugLog(L"[LayoutFail] this=%p type=%d w=%d (invalid)", pThis, t, w[t]);
            return false;
        }
    }
    // The close button is required. Minimize/maximize may be 0 (dialogs,
    // message boxes, About Windows: windows that only have a close button).
    if (w[3] <= 0) {
        DebugLog(L"[LayoutFail] this=%p close button width=%d", pThis, w[3]);
        return false;
    }

    static const int kOrders[3][3] = { {3, 1, 2}, {3, 2, 1}, {1, 2, 3} };
    int o = g_settings.order;

    // Windows with only a close button (dialogs, message boxes, About Windows)
    // are positioned slightly differently by DWM, so they get their own offset.
    bool closeOnly = (w[1] <= 0 && w[2] <= 0);

    // Tighten: buttons 2 and 3 overlap the previous one by T px (0 = default).
    // Works on both sides and in File Explorer.
    int T = 0;
    if (g_settings.tighten > 0 && w[1] > 0 && w[2] > 0) {
        int dpiT = 0;
        if (hwndUsed && g_pGetDpiForWindow) dpiT = (int)g_pGetDpiForWindow(hwndUsed);
        if (dpiT <= 0 && g_pGetDpiForSystem) dpiT = (int)g_pGetDpiForSystem();
        if (dpiT <= 0) dpiT = 96;
        // Middle button: on the left it depends on the order, on the right it is maximize
        int midW = g_settings.moveButtons ? w[kOrders[o][1]] : w[2];
        T = TightenPx(dpiT, midW);
    }

    int x = 0;                 // right edge of the button group (from the left)
    int rightMargin = baseOut; // space reserved on the right for the title area
    if (g_settings.moveButtons) {
        x = baseOut + EffLeftMargin(hwndUsed) + (closeOnly ? kCloseOnlyOffset : 0);
        for (int i = 0; i < 3; i++) {
            int t = kOrders[o][i];
            x += w[t] - (i > 0 ? T : 0);
            t_seq.outBefore[t] = winW - x;
        }
    } else {
        // Default Windows placement: close, maximize, minimize from the right.
        static const int kStock[3] = { 3, 2, 1 };
        int out = baseOut;
        for (int i = 0; i < 3; i++) {
            int t = kStock[i];
            t_seq.outBefore[t] = out;
            out += w[t] - ((i < 2) ? T : 0);
        }
        rightMargin = out;  // the buttons occupy the right side
    }

    // Close-only windows: optionally park the close button outside the window.
    // The hit-test fix treats the spot it leaves behind as plain caption.
    if (closeOnly && !g_settings.showDialogClose) {
        t_seq.outBefore[3] = winW + 200;
        if (g_settings.moveButtons) x = baseOut;
        else rightMargin = baseOut;
    }

    // Only skip tiny surfaces. Narrow windows keep the left layout (buttons may
    // extend past the right edge, just like the default layout does).
    if (winW < 100) return false;

    for (int t = 1; t <= 3; t++) t_seq.w[t] = w[t];
    t_seq.origFinalOut = baseOut + w[1] + w[2] + w[3];
    t_seq.buttonEnd = x;
    t_seq.rightMargin = rightMargin;
    t_seq.winW = winW;
    t_seq.hwnd = hwndUsed;
    t_seq.win = pThis;
    t_seq.closeOnly = closeOnly;

    int dpi = 0;
    if (hwndUsed && g_pGetDpiForWindow) dpi = (int)g_pGetDpiForWindow(hwndUsed);
    if (dpi <= 0 && g_pGetDpiForSystem) dpi = (int)g_pGetDpiForSystem();
    if (dpi <= 0) dpi = 96;
    t_seq.dpi = dpi;
    if (!g_settings.moveButtons) {
        // Title area starts where the default icon sits (16px @ 96 dpi);
        // RepositionTitle adds titleGap, so subtract it here.
        x = MulDiv(16, dpi, 96) - g_settings.titleGap;
        t_seq.buttonEnd = x;
    }
    t_seq.valid = true;

    int bl[4] = {}, br[4] = {};
    for (int t = 1; t <= 3; t++) {
        br[t] = winW - t_seq.outBefore[t];
        bl[t] = br[t] - w[t];
        if (T > 0 && w[t] > 0) { bl[t] += T / 2; br[t] -= T - T / 2; }   // hit slot = narrower than the 46px visual
    }
    StoreWinLayout(pThis, bl, br, b + a + 2, hwndUsed);

    if (g_settings.debugLog && hwndUsed) {
        RECT rr = {};
        if (GetWindowRect(hwndUsed, &rr)) {
            DebugLog(L"[Rect] this=%p hwnd=%p rect=(%ld,%ld,%ld,%ld) rectW=%ld tb=(%ld,%ld,%ld,%ld) style=%08lX zoomed=%d winW=%d expectedInset=%d",
                     pThis, hwndUsed, rr.left, rr.top, rr.right, rr.bottom, rr.right - rr.left,
                     tb.left, tb.top, tb.right, tb.bottom,
                     (unsigned long)GetWindowLongW(hwndUsed, GWL_STYLE), (int)IsZoomed(hwndUsed),
                     winW, expectedInset);
        }
    }

    DebugLog(L"[Layout] this=%p closeOnly=%d a=%d b=%d base=%d W=%d match=%d exact=%d hwnd=%p dpi=%d buttonEnd=%d close=[%d,%d] max=[%d,%d] min=[%d,%d] band=%d",
             pThis, (int)closeOnly, a, b, baseOut, winW, (int)(hwndUsed != nullptr), (int)(hwndUsed != nullptr), hwndUsed, dpi,
             x, bl[3], br[3], bl[2], br[2], bl[1], br[1], b + a + 2);
    return true;
}

static long UpdateNCAreaButton_Hook(void* pThis, int type, int a, int b, int* pOut) {
    if (type == 3) {
        t_seq = Seq{};
        ClearWinLayout(pThis);
        if (g_unloading) {
            InterlockedIncrement(&g_unloadRelayouts);
        } else if (a > 0 && pOut && (g_settings.moveButtons || g_settings.moveTitle)) {
            ComputeLayout(pThis, a, b, *pOut);
        }
    }

    if (t_seq.valid && pOut) {
        if (type >= 1 && type <= 3) *pOut = t_seq.outBefore[type];
        else if (type == 0) *pOut = t_seq.origFinalOut;
    }

    long r = UpdateNCAreaButton_Orig(pThis, type, a, b, pOut);

    if (type == 0) {
        if (t_seq.valid && g_settings.moveTitle && g_hasPosHook && t_posDepth > 0) {
            t_seq.active = true;
        }
    }
    return r;
}

static long PosAndSizes_Hook(void* p) {
    Seq saved = t_seq;
    t_seq = Seq{};
    t_posDepth++;
    long r = PosAndSizes_Orig(p);
    t_posDepth--;
    t_seq = saved;
    return r;
}

// ---------------- re-layout when the title text changes ----------------
// A tab/title change only updates the text; the full non-client layout does not
// run, so the title keeps the position computed for the old text. We re-run the
// layout BEFORE the text update (so the text is built for the final box) and
// again AFTER it (in case DWM touched the layout meanwhile).
static bool TitleDependsOnText() {
    // Without a window handle the title width can't be measured, so a text change needs no relayout.
    return g_settings.moveTitle && g_hasPosHook && g_hwndOffset != SIZE_MAX && TitleWidthMatters();
}

static void DoRelayout(void* pThis, const wchar_t* tag) {
    t_relayout = true;
    DebugLog(L"[TextRelayout] %s this=%p", tag, pThis);
    PosAndSizes_Hook(pThis);
    t_relayout = false;
}

static bool RelayoutIfTitleChanged(void* pThis, const wchar_t* tag) {
    if (!TitleDependsOnText() || t_posDepth != 0 || t_relayout) return false;

    int l[4] = {}, rr[4] = {}, band = 0;
    if (!GetWinLayout(pThis, l, rr, &band)) return false;  // never laid out by us

    HWND hw = nullptr;
    unsigned oldHash = 0;
    bool hasState = GetTitleState(pThis, &hw, &oldHash);
    if (hasState && hw && IsWindow(hw)) {
        unsigned nowHash = HashWindowText(hw);
        if (nowHash == kHashUnsafe || nowHash == oldHash) return false;   // hung window: don't relayout
    }

    DoRelayout(pThis, tag);
    return true;
}

typedef long (*UpdateText_t)(void*, void*);
static UpdateText_t UpdateText_Orig;

static long UpdateText_Hook(void* pThis, void* frame) {
    bool pre = RelayoutIfTitleChanged(pThis, L"UpdateText-pre");
    long r = UpdateText_Orig(pThis, frame);
    if (pre && t_posDepth == 0 && !t_relayout) DoRelayout(pThis, L"UpdateText-post");
    return r;
}

typedef void (*OnTitleUpdated_t)(void*);
static OnTitleUpdated_t OnTitleUpdated_Orig;

static void OnTitleUpdated_Hook(void* pThis) {
    bool pre = RelayoutIfTitleChanged(pThis, L"OnTitleUpdated-pre");
    OnTitleUpdated_Orig(pThis);
    if (pre && t_posDepth == 0 && !t_relayout) DoRelayout(pThis, L"OnTitleUpdated-post");
}

// The icon menu is delayed briefly. If a second click (double-click) arrives
// before the timer fires, the menu is cancelled and the window is closed instead.
#ifndef WM_POPUPSYSTEMMENU
#define WM_POPUPSYSTEMMENU 0x0313
#endif

static PTP_TIMER g_menuTimer;
static SRWLOCK g_menuLock = SRWLOCK_INIT;
static HWND g_menuHwnd;
static POINT g_menuPt;

static VOID CALLBACK MenuTimerCallback(PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER) {
    AcquireSRWLockExclusive(&g_menuLock);
    HWND hw = g_menuHwnd;
    POINT pt = g_menuPt;
    g_menuHwnd = nullptr;
    ReleaseSRWLockExclusive(&g_menuLock);
    if (hw && !g_unloading && IsWindow(hw))
        PostMessageW(hw, WM_POPUPSYSTEMMENU, 0, MAKELPARAM(pt.x, pt.y));
}

static void QueueIconMenu(HWND hw, int x, int y) {
    if (!g_menuTimer) return;
    AcquireSRWLockExclusive(&g_menuLock);
    g_menuHwnd = hw;
    g_menuPt = {x, y};
    ReleaseSRWLockExclusive(&g_menuLock);
    UINT dc = GetDoubleClickTime();
    LONGLONG due = -(LONGLONG)(dc > 250 ? 250 : dc) * 10000;   // relative, 100 ns units; change 250 for a faster/slower menu
    FILETIME ft = {(DWORD)due, (DWORD)(due >> 32)};
    SetThreadpoolTimer(g_menuTimer, &ft, 0, 0);
}

// ---------------- hit-test fix ----------------
// DWM stretches the close button's hit area (in maximized windows) over most of
// the title bar, assuming close is the last button. When the reported button
// does not contain the point, replace the result with the correct one:
//  - point inside another button's rectangle -> that button's code
//  - point elsewhere in the title bar band   -> HTCAPTION (drag / double-click)
//  - otherwise                               -> reject
//
// Diagnostic: [HitRaw] logs the raw DWM answer for points near the top-left
// corner, but only when the window or the returned code changes, so the log
// quota is not used up by every mouse move.
static void* g_lastRawWin = nullptr;
static unsigned g_lastRawCode = 0xFFFFFFF0u;

static bool DoHitTest_Hook(void* pThis, const POINT* pt, void** ppVisual, unsigned int* pFlags) {
    bool r = DoHitTest_Orig(pThis, pt, ppVisual, pFlags);

    if (g_settings.debugLog && pt && pFlags && pt->y <= 40) {
        unsigned c = r ? *pFlags : 0xFFFFFFF1u;
        if (pThis != g_lastRawWin || c != g_lastRawCode) {
            g_lastRawWin = pThis;
            g_lastRawCode = c;
            HitLog(L"[HitRaw] win=%p pt=(%ld,%ld) r=%d code=%u", pThis, pt->x, pt->y, (int)r, r ? *pFlags : 0);
        }
    }

    if (!r || !pt || !pFlags) return r;

    unsigned int code = *pFlags;
    if (g_settings.debugLog && pt->y <= 40 && pt->x > 200) {
        int la[4] = {}, ra[4] = {}, ba = 0, ia = 0, ib = 0;
        HitLog(L"[IconChk] pt=(%ld,%ld) code=%u hasLayout=%d hasIcon=%d",
               pt->x, pt->y, code, (int)GetWinLayout(pThis, la, ra, &ba), (int)GetWinIcon(pThis, &ia, &ib));
    }
    if (code == 3 || code == 2) {
        int l0[4] = {}, r0[4] = {}, b0 = 0, il = 0, ir = 0;
        if (GetWinLayout(pThis, l0, r0, &b0) && GetWinIcon(pThis, &il, &ir)) {
            bool onIcon = g_settings.iconClickable && ir > il &&
                          pt->y >= 0 && pt->y <= b0 &&
                          pt->x >= il - 1 && pt->x < ir + 1;
            if (onIcon) {
                for (int t = 1; t <= 3; t++)
                    if (pt->x >= l0[t] && pt->x < r0[t]) { onIcon = false; break; }
            }
            HitLog(L"[IconHit] pt=(%ld,%ld) code=%u icon=[%d,%d] onIcon=%d clickable=%d band=%d",
                   pt->x, pt->y, code, il, ir, (int)onIcon, (int)g_settings.iconClickable, b0);
            static bool s_down = false;
            static ULONGLONG s_lastMenu = 0;
            static ULONGLONG s_lastDown = 0;
            bool down = (GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON) & 0x8000) != 0;
            ULONGLONG nowTick = GetTickCount64();
            bool within = (nowTick - s_lastMenu) <= (ULONGLONG)GetDoubleClickTime();
            bool newPress = down && (!s_down || (nowTick - s_lastDown) > 100);
            bool dblIcon = false;
            if (down) s_lastDown = nowTick;
            if (g_settings.iconClickable && onIcon && newPress) {
                HWND hw = GetWinHwnd(pThis);
                if (within) {
                    // second click on the icon = double-click -> close the window
                    s_lastMenu = 0;
                    AcquireSRWLockExclusive(&g_menuLock); g_menuHwnd = nullptr; ReleaseSRWLockExclusive(&g_menuLock);   // cancel the pending menu
                    dblIcon = true;   // let Windows close it itself (HTSYSMENU)
                } else {
                    s_lastMenu = nowTick;
                    RECT wr;
                    if (hw && GetWindowRect(hw, &wr)) {
                        int sx = wr.left + il;
                        int sy = wr.top + b0 - (IsZoomed(hw) ? 6 : 0);
                        QueueIconMenu(hw, sx, sy);
                    }
                }
            }
            s_down = down;
            if (dblIcon) { *pFlags = 3; code = 3; }   // second click: double-click on the icon = close
            else { *pFlags = 2; code = 2; }           // everything else is caption, including the icon
        }
    }
    int type = (code == 20) ? 3 : (code == 9) ? 2 : (code == 8) ? 1 : 0;
    if (!type) return r;

    int l[4] = {}, rr[4] = {}, band = 0;
    if (!GetWinLayout(pThis, l, rr, &band)) return r;

    if (pt->x >= l[type] - 1 && pt->x <= rr[type] + 1) return r;  // legitimate

    static const unsigned int kCodes[4] = { 0, 8, 9, 20 };
    unsigned int newCode = 0;
    if (pt->y >= 0 && pt->y <= band) {
        for (int t = 1; t <= 3; t++) {
            if (pt->x >= l[t] && pt->x < rr[t]) { newCode = kCodes[t]; break; }
        }
        if (!newCode) newCode = 2;  // HTCAPTION
    }

    HitLog(L"[HitFix] win=%p pt=(%ld,%ld) dwmCode=%u buttonRect=[%d,%d] band=%d -> newCode=%u",
           pThis, pt->x, pt->y, code, l[type], rr[type], band, newCode);

    if (ppVisual) *ppVisual = nullptr;
    if (newCode) {
        *pFlags = newCode;
        return true;
    }
    *pFlags = 0;
    return false;
}

// ======================================================================
//  PART 2: Glyph painter (macOS-style circles / custom button images)
// ======================================================================

static volatile LONG g_tlEnabled = 1;
static volatile LONG g_styleCustom = 0;     // 1 = "Custom images" style
static volatile LONG g_painterBroken = 0;   // 1 = DWM object layout did not match, the painter stays off
static volatile LONG g_restartOnUnload = 0;
static volatile LONG g_debug = 0;
static volatile LONG g_everPainted = 0;
static volatile LONG g_pending = 0;     // 1 = repaint on the next DWM-thread hook call
static volatile LONG g_dirty = 0;
static volatile LONG g_iconVerSeen = 0;     // last icon cache version loaded by dwm.exe
static ULONGLONG g_iconVerCheck = 0;        // tick of the last version check
static int g_base[3][3] = { {254, 188, 46}, {40, 200, 64}, {255, 95, 87} };   // R,G,B: minimize, maximize/restore, close

static int HexVal(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}

// "#RRGGBB", "RRGGBB", "#RGB" or "RGB"
static bool ParseHexColor(PCWSTR s, int* out) {
    if (!s) return false;
    while (*s == L' ') s++;
    if (*s == L'#') s++;
    int d[6];
    int n = 0;
    while (*s && *s != L' ') {
        int v = HexVal(*s);
        if (v < 0 || n >= 6) return false;
        d[n++] = v;
        s++;
    }
    if (n == 6) {
        out[0] = d[0] * 16 + d[1]; out[1] = d[2] * 16 + d[3]; out[2] = d[4] * 16 + d[5];
        return true;
    }
    if (n == 3) {
        out[0] = d[0] * 17; out[1] = d[1] * 17; out[2] = d[2] * 17;
        return true;
    }
    return false;
}

static void LoadIcons();

static void TL_LoadSettings() {
    g_debug = Wh_GetIntSetting(L"debugLog") ? 1 : 0;
    PCWSTR style = Wh_GetStringSetting(L"buttonStyle.style");
    g_tlEnabled = (style && wcscmp(style, L"windows") != 0 && !g_painterBroken) ? 1 : 0;
    g_styleCustom = (style && wcscmp(style, L"custom") == 0) ? 1 : 0;
    Wh_FreeStringSetting(style);
    g_restartOnUnload = Wh_GetIntSetting(L"buttonStyle.restartDwmOnUnload") ? 1 : 0;

    static const int kDef[3][3] = { {254, 188, 46}, {40, 200, 64}, {255, 95, 87} };
    static const wchar_t* const kKeys[3] = {
        L"buttonStyle.minimizeColor", L"buttonStyle.maximizeColor", L"buttonStyle.closeColor"
    };
    for (int r = 0; r < 3; r++) {
        PCWSTR str = Wh_GetStringSetting(kKeys[r]);
        int c[3];
        if (ParseHexColor(str, c)) memcpy(g_base[r], c, sizeof(c));
        else memcpy(g_base[r], kDef[r], sizeof(c));
        Wh_FreeStringSetting(str);
    }
    LoadIcons();   // reads the images from the mod storage (also runs on the DWM thread when explorer.exe syncs new images)
}

// ---------------- painter logging (only when debugLog is on) ----------------
static SRWLOCK g_logLock = SRWLOCK_INIT;
static int g_total = 0;
static int g_catCount[8];
static wchar_t g_catLast[8][400];

static void LogCat(int cat, int cap, const wchar_t* fmt, ...) {
    if (!g_debug) return;
    wchar_t buf[400];
    va_list a;
    va_start(a, fmt);
    _vsnwprintf(buf, 399, fmt, a);
    va_end(a);
    buf[399] = 0;
    AcquireSRWLockExclusive(&g_logLock);
    if (g_catCount[cat] < cap && g_total < 800 && wcscmp(g_catLast[cat], buf) != 0) {
        wcscpy(g_catLast[cat], buf);
        g_catCount[cat]++;
        g_total++;
        Wh_Log(L"%s", buf);
    }
    ReleaseSRWLockExclusive(&g_logLock);
}

// ---------------- memory helpers ----------------
// CBitmapSource layout: +0x00 vtable, +0x08 refcount, +0x10 owner, +0x18 width, +0x1C height,
// +0x50..+0x5C rect in the atlas (left, top, right, bottom).
static bool Readable(const void* p, SIZE_T n) {
    MEMORY_BASIC_INFORMATION mbi;
    if (!p || !VirtualQuery(p, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    return (const char*)p + n <= (const char*)mbi.BaseAddress + mbi.RegionSize;
}

// Layout check before the painter reads or writes a glyph object: the vtable must belong to
// uDWM.dll, the size must be small, and the atlas rect must have the same size as the object.
// (Verified on all 24 objects in the debug log.) If this fails, the painter turns itself off.
static bool AddressInUdwm(const void* p) {
    HMODULE m = nullptr;
    return p && GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   (LPCWSTR)p, &m) && m && m == GetModuleHandleW(L"udwm.dll");
}

static bool LooksLikeBitmapSource(const void* obj) {
    if (!Readable(obj, 0x70)) return false;
    if (!AddressInUdwm(*(void* const*)obj)) return false;
    const unsigned* d = (const unsigned*)obj;
    if (d[6] < 4 || d[6] > 64 || d[7] < 4 || d[7] > 64) return false;
    return d[22] - d[20] == d[6] && d[23] - d[21] == d[7];
}

// ---------------- registry of arrays and glyph objects ----------------
struct ArrInfo { const void* bm[4]; char kind; int set; int role; };
static ArrInfo g_arr[192];
static int g_arrCount = 0;
static int g_curSet = -1, g_curSetCount = 0;
static float g_curSetF = -1.0f;
static unsigned g_curSetH = 0;
static volatile LONG g_dpiChanged = 0;
static ULONGLONG g_lastDpiRefresh = 0;
static void RequestDwmRefresh();   // defined in the lifecycle section
// After a DPI change DWM only rebuilds the inactive glyph set (f=0.4). The active set (f=1.0) is
// created when a window's active state changes, so we flip the foreground window's frame state once.
static HANDLE g_nudgeThread = nullptr;

static DWORD WINAPI ActivateNudgeThread(LPVOID) {
    for (int i = 0; i < 12 && !g_unloading; i++) Sleep(100);   // let DWM finish rebuilding (~1.2 s)
    if (g_unloading) return 0;

    // Pick a visible window that this mod laid out (a normal caption window, not a frame host)
    HWND target = nullptr;
    HWND fg = GetForegroundWindow();
    AcquireSRWLockShared(&g_layoutLock);
    for (auto& p : g_winLayouts) {
        HWND h = p.second.hwnd;
        if (!(p.second.valid && h && IsWindow(h) && IsWindowVisible(h) && !IsIconic(h))) continue;
        if (h == fg) { target = h; break; }   // prefer the foreground window
        if (!target) target = h;              // otherwise the first usable one
    }
    ReleaseSRWLockShared(&g_layoutLock);

    if (target) {
        bool isFg = (GetForegroundWindow() == target);
        DWORD_PTR res = 0;
        // Flip the active state and put it back, so the final state is what it was
        SendMessageTimeoutW(target, WM_NCACTIVATE, isFg ? FALSE : TRUE, 0, SMTO_ABORTIFHUNG, 300, &res);
        SendMessageTimeoutW(target, WM_NCACTIVATE, isFg ? TRUE : FALSE, 0, SMTO_ABORTIFHUNG, 300, &res);
        LogCat(0, 200, L"[DpiRefresh] activation nudge sent to hwnd=%p fg=%d", target, (int)isFg);
    }
    return 0;
}

static SRWLOCK g_arrLock = SRWLOCK_INIT;

struct Glyph { const void* obj; const void* a0; int role, state; unsigned rc[4]; unsigned sz[2]; void* vt; float f; };
static Glyph g_gl[256];
static int g_glN = 0;
static int g_extR = 64, g_extB = 864;
static SRWLOCK g_glLock = SRWLOCK_INIT;

// A stored CBitmapSource pointer can outlive the object (theme/DPI change, device reset).
// Check that the memory still looks like the same kind of live object before touching it.
static bool GlyphStillValid(const Glyph& g) {
    if (!g.obj || !Readable(g.obj, 0x70)) return false;
    if (*(void* const*)g.obj != g.vt) return false;                        // same vtable
    unsigned long long ref = *(const unsigned long long*)((const char*)g.obj + 8);
    if (ref == 0 || ref > 1000000ull) return false;                        // refcount looks sane
    const unsigned* d = (const unsigned*)g.obj;
    return d[6] == g.sz[0] && d[7] == g.sz[1];                             // same size
}

static bool ReadArr(const void* arr, const void** out4) {
    if (!Readable(arr, 24)) return false;
    const unsigned long long* q = (const unsigned long long*)arr;
    const void* const* data = (const void* const*)q[0];
    unsigned cap = (unsigned)(q[2] >> 32);
    if (!data || cap < 4 || cap > 64 || !Readable(data, 32)) return false;
    for (int i = 0; i < 4; i++) out4[i] = data[i];
    return true;
}

// Cells live in the free area of the atlas (y >= 808). Each glyph height class (12, 15, 18, 21, 24 px =
// 100%..200% scaling) has its own 4x4 block of cells: 4 kinds x 4 states. A glyph of one DPI never shares a cell
// with a glyph of another DPI. Kinds: 0 = minimize, 1 = restore, 2 = maximize, 3 = close.
static const int kClsPitch[5] = { 15, 16, 21, 26, 30 };
static const int kClsX[5]     = {  1, 61, 125,  1, 105 };
static const int kClsY[5]     = { 808, 808, 808, 892, 892 };

static int SizeClass(unsigned h) {
    int c = ((int)h - 12) / 3;   // 12..14 -> 0, 15..17 -> 1, 18..20 -> 2, 21..23 -> 3, 24..26 -> 4
    return c < 0 ? 0 : (c > 4 ? 4 : c);
}

// role 0 = minimize, 1 = maximize, 2 = close, 3 = restore
static int CellKind(int role, unsigned w, unsigned h) {
    return role == 2 ? 3 : (role == 3 ? 1 : (role == 1 ? 2 : 0));
}

static void CellRect(unsigned w, unsigned h, int role, int state, unsigned* rc) {
    int cls = SizeClass(h);
    unsigned x = (unsigned)(kClsX[cls] + kClsPitch[cls] * CellKind(role, w, h));
    unsigned y = (unsigned)(kClsY[cls] + kClsPitch[cls] * state);
    rc[0] = x; rc[1] = y; rc[2] = x + w; rc[3] = y + h;
}

// Kind of the cell that contains (x, y), or -1 when the point is not inside one of our blocks
static int KindFromCell(unsigned x, unsigned y) {
    for (int c = 0; c < 5; c++) {
        int p = kClsPitch[c];
        if ((int)x >= kClsX[c] && (int)x < kClsX[c] + 4 * p && (int)y >= kClsY[c] && (int)y < kClsY[c] + 4 * p)
            return ((int)x - kClsX[c]) / p;
    }
    return -1;
}

static void MoveToCell(Glyph& g) {
    if (!GlyphStillValid(g)) return;
    CellRect(g.sz[0], g.sz[1], g.role, g.state, g.rc);
    unsigned* d = (unsigned*)g.obj;
    d[20] = g.rc[0]; d[21] = g.rc[1]; d[22] = g.rc[2]; d[23] = g.rc[3];
}

// First guess: A left margin = 4 -> close; B width = 14 -> maximize; the rest minimize.
// Minimize vs restore is corrected later from the button width (46 = middle, 47 = minimize).
static int DetectRole(const void* a0, const void* b0, int fallback) {
    if (Readable(a0, 0x70) && Readable(b0, 0x70)) {
        const unsigned* da = (const unsigned*)a0;
        const unsigned* db = (const unsigned*)b0;
        if (da[8] < 8) return 2;                  // close: margin A = 4 px at every DPI (12 for min/max)
        if (db[6] > db[7]) return 1;              // maximize: wider than tall (100%, 150%+)
        // 125%: minimize and maximize are both square, so tell them apart by order within the set
        return (fallback == 0 || fallback == 1) ? fallback : 0;
    }
    return fallback;
}

// Middle button: maximize (role 1) or restore (role 3).
// At 100% they differ in size (14x12 vs 12x12). At other DPIs the sizes are equal or
// unreliable, so the second middle set of the same size class and opacity is the other kind.
// Caller must hold g_glLock.
static int MiddleRoleLocked(const void* const* fam, float f, unsigned w, unsigned h) {
    int cls = SizeClass(h);
    if (cls == 0) return w > h ? 1 : 3;
    for (int i = 0; i < g_glN; i++) {
        const Glyph& g = g_gl[i];
        if ((g.role != 1 && g.role != 3) || g.f != f || SizeClass(g.sz[1]) != cls) continue;
        bool self = false;
        for (int s = 0; s < 4; s++) if (fam[s] && fam[s] == g.obj) self = true;
        if (self || !GlyphStillValid(g)) continue;
        return g.role == 1 ? 3 : 1;
    }
    // No sibling yet. The active set (f ~ 1.0) is built for the window that just became
    // active, so the foreground window tells whether this set is restore or maximize.
    if (f > 0.7f) {
        HWND fg = GetForegroundWindow();
        if (fg && IsZoomed(fg)) return 3;
    }
    return 1;
}

static void CorrectRole(const void* bmp, int role) {
    const void* fam[4] = {};
    bool have = false;
    AcquireSRWLockShared(&g_arrLock);
    for (int i = 0; i < g_arrCount && !have; i++) {
        if (g_arr[i].kind != 'B') continue;
        for (int s = 0; s < 4; s++) {
            if (g_arr[i].bm[s] == bmp) { memcpy(fam, g_arr[i].bm, sizeof(fam)); have = true; break; }
        }
    }
    ReleaseSRWLockShared(&g_arrLock);
    if (!have) return;
    bool changed = false;
    AcquireSRWLockExclusive(&g_glLock);
    int midRole = -1;
    for (int i = 0; i < g_glN; i++) {
        for (int s = 0; s < 4; s++) {
            if (!fam[s] || g_gl[i].obj != fam[s]) continue;
            int cur = g_gl[i].role;
            if (cur == 2) continue;
            int want = role;
            if (role == 1) {
                if (cur == 1 || cur == 3) continue;   // already the middle button
                if (midRole < 0) midRole = MiddleRoleLocked(fam, g_gl[i].f, g_gl[i].sz[0], g_gl[i].sz[1]);
                want = midRole;
            } else if (g_gl[i].sz[0] > g_gl[i].sz[1]) {
                continue;   // a wide glyph must not become minimize
            }
            if (cur == want) continue;
            g_gl[i].role = want;
            MoveToCell(g_gl[i]);
            changed = true;
        }
    }
    ReleaseSRWLockExclusive(&g_glLock);
    if (changed) g_dirty = 1;
}

static void AddExtent(const void* obj) {
    if (!Readable(obj, 0x70)) return;
    const unsigned* d = (const unsigned*)obj;
    if (d[22] > 0 && d[22] < 4096 && (int)d[22] > g_extR) g_extR = (int)d[22];
    if (d[23] > 0 && d[23] < 8192 && (int)d[23] > g_extB) g_extB = (int)d[23];
}

// Caller holds g_glLock.
static bool HeightIsLiveLocked(unsigned h) {
    for (int i = 0; i < g_glN; i++)
        if (g_gl[i].sz[1] == h && GlyphStillValid(g_gl[i])) return true;
    return false;
}

static bool RegisterPair(const void* a, const void* b, float f) {
    const void* va[4] = {};
    const void* vb[4] = {};
    if (!ReadArr(a, va) || !ReadArr(b, vb)) return false;
    if (!vb[0] || !Readable(vb[0], 0x70)) return false;   // empty/odd arrays: ignore
    if (g_painterBroken) return false;
    {
        bool known = false;
        AcquireSRWLockShared(&g_arrLock);
        for (int i = 0; i < g_arrCount; i++) {
            if (g_arr[i].kind == 'B' && g_arr[i].bm[0] == vb[0]) { known = true; break; }
        }
        ReleaseSRWLockShared(&g_arrLock);
        if (!known) {
            for (int i = 0; i < 4; i++) {
                if (vb[i] && !LooksLikeBitmapSource(vb[i])) {
                    g_painterBroken = 1;
                    g_tlEnabled = 0;
                    Wh_Log(L"[Painter] unexpected CBitmapSource layout (obj=%p), painter disabled", vb[i]);
                    return false;
                }
            }
        }
    }

    bool isNew = false;
    int set = -1, role = -1;
    AcquireSRWLockExclusive(&g_arrLock);
    bool found = false;
    for (int i = 0; i < g_arrCount; i++) {
        if (g_arr[i].kind == 'B' && g_arr[i].bm[0] == vb[0]) { found = true; break; }
    }
    if (!found && g_arrCount + 2 <= 192) {
        unsigned hNow = ((const unsigned*)vb[0])[7];
        if (g_curSet >= 0 && hNow != g_curSetH) {
            // Only a height with no live registered glyph counts as a DPI change
            AcquireSRWLockShared(&g_glLock);
            if (!HeightIsLiveLocked(hNow)) g_dpiChanged = 1;
            ReleaseSRWLockShared(&g_glLock);
        }
        if (g_curSet < 0 || g_curSetCount >= 3 || f != g_curSetF || hNow != g_curSetH) {
            g_curSet++;
            g_curSetCount = 0;
            g_curSetF = f;
            g_curSetH = hNow;
        }
        set = g_curSet;
        int idx = g_curSetCount++;
        role = DetectRole(va[0], vb[0], idx == 1 ? 1 : 0);
        // A glyph that an earlier session already moved to its cell (y >= 808): only the
        // minimize cell and the wide shape are trusted. Maximize vs restore is decided below.
        if (role != 2) {
            const unsigned* movedRc = (const unsigned*)vb[0];
            int cellKind = KindFromCell(movedRc[20], movedRc[21]);
            bool wide = movedRc[6] > movedRc[7];
            if (cellKind == 0 && !wide) role = 0;
            else if (cellKind == 1 || cellKind == 2) role = 1;   // middle button: maximize vs restore is decided below, not from the old cell
            else if (wide && role == 0) role = 1;
        }
        // The second glyph of a set that is not close is the middle button
        if (idx == 1 && role == 0) role = 1;
        // Middle button: maximize (role 1) or restore (role 3)
        if (role == 1) {
            AcquireSRWLockShared(&g_glLock);
            role = MiddleRoleLocked(vb, f, ((const unsigned*)vb[0])[6], ((const unsigned*)vb[0])[7]);
            ReleaseSRWLockShared(&g_glLock);
        }
        memcpy(g_arr[g_arrCount].bm, vb, sizeof(vb));
        g_arr[g_arrCount].kind = 'B'; g_arr[g_arrCount].set = set; g_arr[g_arrCount].role = role;
        g_arrCount++;
        memcpy(g_arr[g_arrCount].bm, va, sizeof(va));
        g_arr[g_arrCount].kind = 'A'; g_arr[g_arrCount].set = set; g_arr[g_arrCount].role = role;
        g_arrCount++;
        isNew = true;
    }
    ReleaseSRWLockExclusive(&g_arrLock);

    if (isNew) {
        AcquireSRWLockExclusive(&g_glLock);
        // forget glyph entries whose objects were freed or reused
        int keepN = 0;
        for (int k = 0; k < g_glN; k++) {
            if (GlyphStillValid(g_gl[k])) g_gl[keepN++] = g_gl[k];
        }
        g_glN = keepN;
        for (int i = 0; i < 4; i++) {
            AddExtent(va[i]);
            AddExtent(vb[i]);
            if (vb[i] && Readable(vb[i], 0x70) && g_glN < 256) {
                const unsigned* d = (const unsigned*)vb[i];
                Glyph& g = g_gl[g_glN++];
                g.obj = vb[i]; g.role = role; g.state = i;
                for (int k = 0; k < 4; k++) g.rc[k] = d[20 + k];
                g.sz[0] = d[6]; g.sz[1] = d[7];
                g.a0 = va[0];
                g.vt = *(void* const*)vb[i];
                g.f = f;
                MoveToCell(g);
            }
        }
        ReleaseSRWLockExclusive(&g_glLock);

        const unsigned* d0 = (const unsigned*)vb[0];
        unsigned lr[6] = { d0[20], d0[21], d0[22], d0[23], d0[6], d0[7] };
        unsigned aMargin = Readable(va[0], 0x70) ? ((const unsigned*)va[0])[8] : 0;
        unsigned aW = Readable(va[0], 0x70) ? ((const unsigned*)va[0])[6] : 0;
        unsigned aH = Readable(va[0], 0x70) ? ((const unsigned*)va[0])[7] : 0;
        LogCat(0, 200, L"[Pair] set=%d role=%d f=%f A0=%p B0=%p rc=(%u,%u)-(%u,%u) size=%ux%u margin=%u Asize=%ux%u",
               set, role, (double)f, va[0], vb[0], lr[0], lr[1], lr[2], lr[3], lr[4], lr[5], aMargin, aW, aH);
        if (InterlockedExchange(&g_dpiChanged, 0)) {
            ULONGLONG nowTick = GetTickCount64();
            if (nowTick - g_lastDpiRefresh > 3000) {
                g_lastDpiRefresh = nowTick;
                InterlockedExchange(&g_pending, 1);
                RequestDwmRefresh();   // posted message: runs after this hook returns
                LogCat(0, 200, L"[DpiRefresh] glyph size changed, DWM refresh requested");
                if (!g_nudgeThread || WaitForSingleObject(g_nudgeThread, 0) == WAIT_OBJECT_0) {
                    if (g_nudgeThread) CloseHandle(g_nudgeThread);
                    g_nudgeThread = CreateThread(nullptr, 0, ActivateNudgeThread, nullptr, 0, nullptr);
                }
            }
        }
    }
    return isNew;
}

static bool Classify(const void* bmp, char* kind, int* state, const void** st0) {
    bool ok = false;
    AcquireSRWLockShared(&g_arrLock);
    for (int i = 0; i < g_arrCount && !ok; i++) {
        for (int s = 0; s < 4; s++) {
            if (g_arr[i].bm[s] == bmp) {
                *kind = g_arr[i].kind; *state = s; *st0 = g_arr[i].bm[0];
                ok = true;
                break;
            }
        }
    }
    ReleaseSRWLockShared(&g_arrLock);
    return ok;
}

// ---------------- CBitmapSource::Update (address only) ----------------
// CBitmapSource::Update is only called, never hooked. Its address is resolved
// through an address-only entry in the HookSymbols array in Wh_ModInit.
typedef long (*UpdateBmp_t)(void* pThis, IWICBitmap* bmp);
static UpdateBmp_t g_updateBmp = nullptr;

// ---------------- rendering ----------------
static const GUID kPBGRA = {0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10}};
static const GUID kCLSID_WICFactory = {0xcacaf262, 0x9370, 0x4615, {0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a}};
static const GUID kIID_WICFactory = {0xec5ec8a9, 0xc395, 0x4314, {0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70}};

static float DistSeg(float px, float py, float ax, float ay, float bx, float by) {
    float vx = bx - ax, vy = by - ay, wx = px - ax, wy = py - ay;
    float c = vx * vx + vy * vy;
    float t = c > 0.0f ? (wx * vx + wy * vy) / c : 0.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float dx = px - (ax + t * vx), dy = py - (ay + t * vy);
    return sqrtf(dx * dx + dy * dy);
}

// role 0 = minimize, 1 = maximize/restore, 2 = close
static bool InSymbol(int role, float ux, float uy) {
    const float th = 0.6f, L = 2.4f;
    if (role == 2) return DistSeg(ux, uy, -L, -L, L, L) <= th || DistSeg(ux, uy, -L, L, L, -L) <= th;
    if (role == 0) return DistSeg(ux, uy, -L - 0.4f, 0.0f, L + 0.4f, 0.0f) <= th;
    // macOS-style maximize: two right triangles (top-right and bottom-left) with a gap between them
    const float a = 3.0f, d = 4.8f;
    bool tr = ux <= a && uy >= -a && (ux - uy) >= (2.0f * a - d);
    bool bl = ux >= -a && uy <= a && (ux - uy) <= (d - 2.0f * a);
    return tr || bl;
}

static void RenderGlyph(BYTE* px, int w, int h, int role, int state) {
    if (role == 3) role = 1;   // restore uses the maximize colors
    if (role < 0 || role > 2) role = 0;
    int bg[3], sym[3];
    for (int i = 0; i < 3; i++) {
        bg[i] = state == 2 ? g_base[role][i] * 78 / 100 : g_base[role][i];
        sym[i] = g_base[role][i] * g_base[role][i] / 255 * 60 / 100;
    }
    bool drawSym = (state == 1 || state == 2);
    float cx = w / 2.0f, cy = h / 2.0f;
    float rad = (w < h ? w : h) / 2.0f - 0.25f;
    float k = 12.0f / (float)(w < h ? w : h);   // keeps the x / - / + symbols proportional at high DPI
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int nBg = 0, nSym = 0;
            for (int sy = 0; sy < 4; sy++) {
                for (int sx = 0; sx < 4; sx++) {
                    float ux = x + (sx + 0.5f) / 4.0f - cx;
                    float uy = y + (sy + 0.5f) / 4.0f - cy;
                    if (ux * ux + uy * uy <= rad * rad) {
                        if (drawSym && InSymbol(role, ux * k, uy * k)) nSym++;
                        else nBg++;
                    }
                }
            }
            BYTE* p = px + (y * w + x) * 4;
            p[0] = (BYTE)((nBg * bg[2] + nSym * sym[2]) / 16);
            p[1] = (BYTE)((nBg * bg[1] + nSym * sym[1]) / 16);
            p[2] = (BYTE)((nBg * bg[0] + nSym * sym[0]) / 16);
            p[3] = (BYTE)((nBg + nSym) * 255 / 16);
        }
    }
}

static int Pow2AtLeast(int v) {
    int p = 16;
    while (p < v && p < 8192) p <<= 1;
    return p;
}

// ---------------- custom images (png/jpg/bmp), decoded in explorer.exe ----------------
struct IconImg { int w, h; BYTE* px; };
static IconImg g_icons[4][4];   // [kind][state]; kind: 0 minimize, 1 restore, 2 maximize, 3 close
static SRWLOCK g_iconLock = SRWLOCK_INIT;

static const wchar_t* const kIconNames[4] = { L"minimize", L"restore", L"maximize", L"close" };
static const wchar_t* const kIconSuffix[4] = { L"", L"_hover", L"_pressed", L"_disabled" };
static const wchar_t* const kIconExts[4] = { L".png", L".jpg", L".jpeg", L".bmp" };

static void TrimPath(wchar_t* s) {
    size_t n = wcslen(s);
    while (n > 0 && (s[n - 1] == L' ' || s[n - 1] == L'"' || s[n - 1] == L'\t')) s[--n] = 0;
    size_t i = 0;
    while (s[i] == L' ' || s[i] == L'"' || s[i] == L'\t') i++;
    if (i) memmove(s, s + i, (n - i + 1) * sizeof(wchar_t));
}

static bool FileExistsW(const wchar_t* path) {
    DWORD a = GetFileAttributesW(path);
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

static const int kMaxIconPx = 64;   // images are shrunk to this size before they are stored

static bool DecodeImage(IWICImagingFactory* fac, const wchar_t* path, IconImg* out) {
    out->w = 0; out->h = 0; out->px = nullptr;

    // Read the file ourselves so a failure shows the real Win32 error
    HANDLE hf = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        LogCat(3, 40, L"[Icon] CreateFile failed, error=%lu (%s)", (unsigned long)GetLastError(), path);
        return false;
    }
    LARGE_INTEGER fsz = {};
    if (!GetFileSizeEx(hf, &fsz) || fsz.QuadPart <= 0 || fsz.QuadPart > 16 * 1024 * 1024) {
        LogCat(3, 40, L"[Icon] unusable file size %lld (%s)", (long long)fsz.QuadPart, path);
        CloseHandle(hf);
        return false;
    }
    DWORD fileSize = (DWORD)fsz.QuadPart;
    BYTE* data = (BYTE*)malloc(fileSize);
    DWORD got = 0;
    bool readOk = data && ReadFile(hf, data, fileSize, &got, nullptr) && got == fileSize;
    DWORD readErr = GetLastError();
    CloseHandle(hf);
    if (!readOk) {
        LogCat(3, 40, L"[Icon] ReadFile failed, error=%lu (%s)", (unsigned long)readErr, path);
        free(data);
        return false;
    }

    // Only hand png / jpeg / bmp data to the decoder: this keeps the other WIC codecs out of this process
    bool known = (fileSize >= 8 && memcmp(data, "\x89PNG\r\n\x1a\n", 8) == 0) ||
                 (fileSize >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) ||
                 (fileSize >= 2 && data[0] == 'B' && data[1] == 'M');
    if (!known) {
        LogCat(3, 40, L"[Icon] not a png/jpg/bmp file, skipped (%s)", path);
        free(data);
        return false;
    }

    {
        wchar_t hdr[48] = L"";
        for (DWORD i = 0; i < 8 && i < fileSize; i++)
            _snwprintf(hdr + i * 3, 4, L"%02X ", data[i]);
        LogCat(3, 40, L"[Icon] read OK size=%lu header=%s(%s)", (unsigned long)fileSize, hdr, path);
    }

    IWICStream* stream = nullptr;
    IWICBitmapDecoder* dec = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* conv = nullptr;
    IWICBitmapScaler* scl = nullptr;
    const wchar_t* step = L"CreateStream";
    bool ok = false;
    UINT w = 0, h = 0;

    HRESULT hr = fac->CreateStream(&stream);
    if (SUCCEEDED(hr) && stream) {
        step = L"InitializeFromMemory";
        hr = stream->InitializeFromMemory(data, fileSize);
    }
    if (SUCCEEDED(hr)) {
        step = L"CreateDecoderFromStream";
        hr = fac->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnDemand, &dec);
    }
    if (SUCCEEDED(hr) && dec) {
        step = L"GetFrame";
        hr = dec->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr) && frame) {
        step = L"CreateFormatConverter";
        hr = fac->CreateFormatConverter(&conv);
    }
    if (SUCCEEDED(hr) && conv) {
        step = L"ConverterInitialize";
        hr = conv->Initialize(frame, kPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    }
    if (SUCCEEDED(hr) && conv) {
        step = L"GetSize";
        hr = conv->GetSize(&w, &h);
        if (SUCCEEDED(hr) && (w == 0 || h == 0)) hr = E_FAIL;
    }
    if (SUCCEEDED(hr) && conv) {
        IWICBitmapSource* src = conv;
        if (w > kMaxIconPx || h > kMaxIconPx) {
            double k = (double)kMaxIconPx / (w > h ? w : h);
            UINT nw = (UINT)(w * k + 0.5), nh = (UINT)(h * k + 0.5);
            if (nw < 1) nw = 1;
            if (nh < 1) nh = 1;
            if (SUCCEEDED(fac->CreateBitmapScaler(&scl)) && scl &&
                SUCCEEDED(scl->Initialize(conv, nw, nh, WICBitmapInterpolationModeFant))) {
                src = scl; w = nw; h = nh;
            }
        }
        BYTE* buf = (BYTE*)malloc((size_t)w * h * 4);
        step = L"CopyPixels";
        hr = buf ? src->CopyPixels(nullptr, w * 4, w * h * 4, buf) : E_OUTOFMEMORY;
        if (SUCCEEDED(hr)) {
            out->w = (int)w; out->h = (int)h; out->px = buf;
            ok = true;
        } else {
            free(buf);
        }
    }
    if (!ok) {
        LogCat(3, 40, L"[Icon] decode step %s failed, hr=0x%08lX (%s)", step, (unsigned long)hr, path);
    }

    if (scl) scl->Release();
    if (conv) conv->Release();
    if (frame) frame->Release();
    if (dec) dec->Release();
    if (stream) stream->Release();
    free(data);   // safe now: the pixels were copied into our own buffer
    return ok;
}

static void FreeIcons() {
    AcquireSRWLockExclusive(&g_iconLock);
    for (int k = 0; k < 4; k++)
        for (int st = 0; st < 4; st++) {
            free(g_icons[k][st].px);
            g_icons[k][st] = IconImg{};
        }
    ReleaseSRWLockExclusive(&g_iconLock);
}

// ---------------- icon storage (shared between explorer.exe and dwm.exe) ----------------
// explorer.exe runs as the user and can read any folder the user can. It decodes the
// images, shrinks them to at most kMaxIconPx and stores the raw pixels in the mod's own
// storage; dwm.exe only reads those pixels back and never opens or decodes an image file.
// Stored value layout: int width, int height, then width * height * 4 bytes (premultiplied BGRA).
static void IconValueName(int kind, int st, wchar_t* out, size_t cap) {
    _snwprintf(out, cap - 1, L"icon_k%d_s%d", kind, st);
    out[cap - 1] = 0;
}

struct IconSettings {
    wchar_t folder[MAX_PATH];
    wchar_t files[4][MAX_PATH];
    bool any;
};

// Reads the image settings. 'any' is true only in the "Custom images" style:
// with the macOS or Windows style the image settings are ignored completely.
static void ReadIconSettings(IconSettings* s) {
    static const wchar_t* const kKeys[4] = {
        L"buttonStyle.minimizeIcon", L"buttonStyle.restoreIcon",
        L"buttonStyle.maximizeIcon", L"buttonStyle.closeIcon"
    };
    *s = IconSettings{};

    PCWSTR st = Wh_GetStringSetting(L"buttonStyle.style");
    bool custom = st && wcscmp(st, L"custom") == 0;
    Wh_FreeStringSetting(st);
    if (!custom) return;

    PCWSTR v = Wh_GetStringSetting(L"buttonStyle.iconFolder");
    if (v) wcsncpy(s->folder, v, MAX_PATH - 1);
    Wh_FreeStringSetting(v);
    TrimPath(s->folder);
    size_t fl = wcslen(s->folder);
    while (fl > 0 && (s->folder[fl - 1] == L'\\' || s->folder[fl - 1] == L'/')) s->folder[--fl] = 0;

    s->any = s->folder[0] != 0;
    for (int k = 0; k < 4; k++) {
        v = Wh_GetStringSetting(kKeys[k]);
        if (v) wcsncpy(s->files[k], v, MAX_PATH - 1);
        Wh_FreeStringSetting(v);
        TrimPath(s->files[k]);
        if (s->files[k][0]) s->any = true;
    }
}

// Finds the image file for (kind, state). 'cand' must hold MAX_PATH + 32 characters.
static bool FindIconFile(const IconSettings& s, int kind, int st, wchar_t* cand) {
    wchar_t stem[MAX_PATH] = L"";
    wchar_t ownExt[16] = L"";
    if (s.files[kind][0]) {
        wcsncpy(stem, s.files[kind], MAX_PATH - 1);
        wchar_t* dot = wcsrchr(stem, L'.');
        wchar_t* sl = wcsrchr(stem, L'\\');
        if (dot && (!sl || dot > sl)) { wcsncpy(ownExt, dot, 15); *dot = 0; }
    } else if (s.folder[0]) {
        _snwprintf(stem, MAX_PATH - 1, L"%s\\%s", s.folder, kIconNames[kind]);
        stem[MAX_PATH - 1] = 0;
    } else {
        return false;
    }
    const wchar_t* exts[5];
    int ne = 0;
    if (ownExt[0]) exts[ne++] = ownExt;
    for (int e = 0; e < 4; e++) exts[ne++] = kIconExts[e];
    for (int e = 0; e < ne; e++) {
        _snwprintf(cand, MAX_PATH + 31, L"%s%s%s", stem, kIconSuffix[st], exts[e]);
        cand[MAX_PATH + 31] = 0;
        if (FileExistsW(cand)) return true;
    }
    return false;
}

static void LoadIcons() {
    // Read the version BEFORE loading: if explorer bumps it meanwhile, we reload once more.
    g_iconVerSeen = Wh_GetIntValue(L"iconCacheVersion", 0);

    IconImg tmp[4][4] = {};
    const size_t maxBlob = 8 + (size_t)kMaxIconPx * kMaxIconPx * 4;
    BYTE* blob = (BYTE*)malloc(maxBlob);
    int loaded = 0;
    for (int kind = 0; kind < 4 && blob; kind++) {
        for (int st = 0; st < 4; st++) {
            wchar_t name[32];
            IconValueName(kind, st, name, 32);
            size_t size = Wh_GetBinaryValue(name, blob, maxBlob);
            if (size < 8) continue;   // not stored, or unreadable
            int w = 0, h = 0;
            memcpy(&w, blob, 4);
            memcpy(&h, blob + 4, 4);
            if (w < 1 || h < 1 || w > kMaxIconPx || h > kMaxIconPx) continue;
            if (size != 8 + (size_t)w * h * 4) continue;
            BYTE* px = (BYTE*)malloc((size_t)w * h * 4);
            if (!px) continue;
            memcpy(px, blob + 8, (size_t)w * h * 4);
            tmp[kind][st].w = w;
            tmp[kind][st].h = h;
            tmp[kind][st].px = px;
            loaded++;
        }
    }
    free(blob);
    LogCat(3, 40, L"[Icon] loaded %d image(s) from the mod storage", loaded);

    AcquireSRWLockExclusive(&g_iconLock);
    for (int k = 0; k < 4; k++)
        for (int st = 0; st < 4; st++) {
            free(g_icons[k][st].px);
            g_icons[k][st] = tmp[k][st];
        }
    ReleaseSRWLockExclusive(&g_iconLock);
}

// Scale an image into the cell (aspect kept, centered) by box-averaging premultiplied values.
// The image keeps its own shape: nothing is clipped to a circle.
static void BlitIcon(BYTE* cell, int cw, int ch, const IconImg& im) {
    float sc = (float)cw / im.w;
    float sc2 = (float)ch / im.h;
    if (sc2 < sc) sc = sc2;
    int tw = (int)(im.w * sc + 0.5f);
    int th = (int)(im.h * sc + 0.5f);
    if (tw < 1) tw = 1;
    if (th < 1) th = 1;
    if (tw > cw) tw = cw;
    if (th > ch) th = ch;
    int ox = (cw - tw) / 2, oy = (ch - th) / 2;
    for (int y = 0; y < th; y++) {
        int sy0 = y * im.h / th;
        int sy1 = ((y + 1) * im.h + th - 1) / th;
        if (sy1 <= sy0) sy1 = sy0 + 1;
        if (sy1 > im.h) sy1 = im.h;
        for (int x = 0; x < tw; x++) {
            int sx0 = x * im.w / tw;
            int sx1 = ((x + 1) * im.w + tw - 1) / tw;
            if (sx1 <= sx0) sx1 = sx0 + 1;
            if (sx1 > im.w) sx1 = im.w;
            unsigned sum[4] = { 0, 0, 0, 0 };
            int cnt = 0;
            for (int sy = sy0; sy < sy1; sy++) {
                for (int sx = sx0; sx < sx1; sx++) {
                    const BYTE* q = im.px + ((size_t)sy * im.w + sx) * 4;
                    sum[0] += q[0]; sum[1] += q[1]; sum[2] += q[2]; sum[3] += q[3];
                    cnt++;
                }
            }
            if (cnt <= 0) continue;
            BYTE* d = cell + ((size_t)(oy + y) * cw + (ox + x)) * 4;
            for (int c = 0; c < 4; c++) d[c] = (BYTE)(sum[c] / cnt);
        }
    }
}

static bool RenderCustomCell(BYTE* cell, int w, int h, int role, int state, unsigned origW, unsigned origH) {
    int kind = CellKind(role, origW, origH);
    if (kind < 0 || kind > 3 || state < 0 || state > 3) return false;
    AcquireSRWLockShared(&g_iconLock);
    const IconImg* im = nullptr;
    bool dedicated = false;
    if (g_icons[kind][state].px) { im = &g_icons[kind][state]; dedicated = true; }
    else if (g_icons[kind][0].px) im = &g_icons[kind][0];
    if (!im) { ReleaseSRWLockShared(&g_iconLock); return false; }
    memset(cell, 0, (size_t)w * h * 4);
    BlitIcon(cell, w, h, *im);
    ReleaseSRWLockShared(&g_iconLock);
    if (state == 2 && !dedicated) {   // pressed without its own image: darken like the circle
        for (int i = 0; i < w * h; i++) {
            BYTE* q = cell + i * 4;
            q[0] = (BYTE)(q[0] * 78 / 100); q[1] = (BYTE)(q[1] * 78 / 100); q[2] = (BYTE)(q[2] * 78 / 100);
        }
    }
    return true;
}

// ---------------- paint the whole atlas in one Update call ----------------
static SRWLOCK g_paintLock = SRWLOCK_INIT;

static void PaintAtlas() {
    if (!g_updateBmp || !g_tlEnabled) return;
    static Glyph snap[256];
    int n, extR, extB;

    AcquireSRWLockExclusive(&g_paintLock);
    AcquireSRWLockShared(&g_glLock);
    n = g_glN;
    memcpy(snap, g_gl, sizeof(Glyph) * n);
    extR = g_extR; extB = g_extB;
    ReleaseSRWLockShared(&g_glLock);

    int validN = 0;
    for (int i = 0; i < n; i++) {
        if (GlyphStillValid(snap[i])) snap[validN++] = snap[i];
    }
    n = validN;

    if (n == 0 || extR <= 0 || extB <= 0) { ReleaseSRWLockExclusive(&g_paintLock); return; }

    int W = Pow2AtLeast(extR);
    int H = Pow2AtLeast(extB);
    if (W < extR) W = extR;
    if (H < extB) H = extB;
    if (W > 8192 || H > 8192) { ReleaseSRWLockExclusive(&g_paintLock); return; }

    BYTE* px = (BYTE*)calloc((size_t)W * H, 4);
    if (!px) { ReleaseSRWLockExclusive(&g_paintLock); return; }

    bool painted[4][4] = {};   // [cell kind][state]: cells drawn from glyphs registered in this session
    for (int i = 0; i < n; i++) {
        int x0 = (int)snap[i].rc[0], y0 = (int)snap[i].rc[1];
        int w = (int)snap[i].rc[2] - x0, h = (int)snap[i].rc[3] - y0;
        if (w < 4 || h < 4 || w > 64 || h > 64) continue;
        if (x0 < 0 || y0 < 0 || x0 + w > W || y0 + h > H) continue;
        if (y0 >= 808 && snap[i].state >= 0 && snap[i].state < 4 && SizeClass(snap[i].sz[1]) == 0)
            painted[CellKind(snap[i].role, snap[i].sz[0], snap[i].sz[1])][snap[i].state] = true;
        BYTE cell[64 * 64 * 4];
        if (!RenderCustomCell(cell, w, h, snap[i].role, snap[i].state, snap[i].sz[0], snap[i].sz[1]))
            RenderGlyph(cell, w, h, snap[i].role, snap[i].state);
        for (int y = 0; y < h; y++) {
            memcpy(px + ((size_t)(y0 + y) * W + x0) * 4, cell + y * w * 4, (size_t)w * 4);
        }
    }
    // A glyph moved to its cell by an earlier session may not be registered in this one
    // (for example the maximize glyph when the mod was turned on while a window was maximized).
    // The cells are at fixed positions, so draw every cell that this session did not paint.
    for (int kind = 0; kind < 4; kind++) {
        for (int st = 0; st < 4; st++) {
            if (painted[kind][st]) continue;
            int w = (kind == 2) ? 14 : 12, h = 12;
            int x0 = kClsX[0] + kClsPitch[0] * kind, y0 = kClsY[0] + kClsPitch[0] * st;
            if (x0 + w > W || y0 + h > H) continue;
            int role = (kind == 0) ? 0 : (kind == 3 ? 2 : (kind == 1 ? 3 : 1));
            BYTE cell[64 * 64 * 4];
            if (!RenderCustomCell(cell, w, h, role, st, (unsigned)w, (unsigned)h))
                RenderGlyph(cell, w, h, role, st);
            for (int y = 0; y < h; y++) {
                memcpy(px + ((size_t)(y0 + y) * W + x0) * 4, cell + y * w * 4, (size_t)w * 4);
            }
        }
    }

    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IWICImagingFactory* fac = nullptr;
    HRESULT hr = CoCreateInstance(kCLSID_WICFactory, nullptr, CLSCTX_INPROC_SERVER, kIID_WICFactory, (void**)&fac);
    if (SUCCEEDED(hr) && fac) {
        IWICBitmap* bmp = nullptr;
        hr = fac->CreateBitmapFromMemory((UINT)W, (UINT)H, kPBGRA, (UINT)(W * 4), (UINT)((size_t)W * H * 4), px, &bmp);
        if (SUCCEEDED(hr) && bmp) {
            // Update is called on one object only; its size and rect are restored afterwards
            void* target = (void*)snap[0].obj;
            unsigned* d = (unsigned*)target;
            unsigned keep[6] = { d[6], d[7], d[20], d[21], d[22], d[23] };
            long r = g_updateBmp(target, bmp);
            LogCat(2, 40, L"[Atlas] target=%p atlas=%dx%d glyphs=%d ext=%dx%d ret=0x%lx | before size=%ux%u rc=(%u,%u)-(%u,%u) after size=%ux%u rc=(%u,%u)-(%u,%u)",
                   target, W, H, n, extR, extB, (unsigned long)r,
                   keep[0], keep[1], keep[2], keep[3], keep[4], keep[5],
                   d[6], d[7], d[20], d[21], d[22], d[23]);
            d[6] = keep[0]; d[7] = keep[1];
            d[20] = keep[2]; d[21] = keep[3]; d[22] = keep[4]; d[23] = keep[5];
            if (r >= 0) g_everPainted = 1;
            bmp->Release();
        } else {
            LogCat(2, 40, L"[Atlas] CreateBitmapFromMemory failed hr=0x%lx", (unsigned long)hr);
        }
        fac->Release();
    } else {
        LogCat(2, 40, L"[Atlas] CoCreateInstance WIC failed hr=0x%lx", (unsigned long)hr);
    }
    if (SUCCEEDED(hrInit)) CoUninitialize();
    free(px);
    ReleaseSRWLockExclusive(&g_paintLock);
}

// Settings changes arrive on a Windhawk thread, not the DWM thread. Writing to DWM's bitmap
// objects from there races with DWM, so settings only set a flag; the work runs in the hooks below.
static void ProcessPending() {
    // explorer.exe bumps this value after it stores new images in the mod storage
    ULONGLONG now = GetTickCount64();
    if (g_pending || now - g_iconVerCheck >= 500) {
        g_iconVerCheck = now;
        if ((LONG)Wh_GetIntValue(L"iconCacheVersion", 0) != g_iconVerSeen) {
            LoadIcons();       // also updates g_iconVerSeen
            InterlockedExchange(&g_pending, 1);
        }
    }
    if (!InterlockedExchange(&g_pending, 0)) return;
    PaintAtlas();
}

static void RestartDwm() {
    // Never restart dwm.exe twice within 30 seconds (protects against restart loops)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULONGLONG now = ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    ULONGLONG last = 0;
    Wh_GetBinaryValue(L"lastDwmRestart", &last, sizeof(last));
    if (last && now >= last && now - last < 30ULL * 10000000ULL) {
        Wh_Log(L"[Restore] restart skipped: dwm.exe was restarted less than 30 seconds ago");
        return;
    }
    Wh_SetBinaryValue(L"lastDwmRestart", &now, sizeof(now));
    LogCat(2, 40, L"[Restore] restarting dwm.exe");
    TerminateProcess(GetCurrentProcess(), 1);
}

// ---------------- forget freed glyph objects ----------------
// CBitmapSource::~CBitmapSource runs on every free. Drop the object from our
// registries so a later object at the same address is not mistaken for a known one.
static bool ForgetBitmapSource(const void* obj) {
    if (!obj) return false;
    bool removed = false;

    AcquireSRWLockExclusive(&g_glLock);
    for (int i = 0; i < g_glN; i++) {
        if (g_gl[i].obj == obj) {
            g_gl[i] = g_gl[--g_glN];
            i--;
            removed = true;
        }
    }
    ReleaseSRWLockExclusive(&g_glLock);

    AcquireSRWLockExclusive(&g_arrLock);
    for (int i = 0; i < g_arrCount; i++) {
        for (int s = 0; s < 4; s++) {
            if (g_arr[i].bm[s] == obj) {
                g_arr[i].bm[s] = nullptr;
                removed = true;
            }
        }
    }
    ReleaseSRWLockExclusive(&g_arrLock);
    return removed;
}

typedef void (*BmpDtor_t)(void*);
static BmpDtor_t BmpDtor_Orig;
static void BmpDtor_Hook(void* pThis) {
    if (g_tlEnabled && (g_glN > 0 || g_arrCount > 0)) {
        if (ForgetBitmapSource(pThis)) {
            LogCat(4, 40, L"[Free] glyph object %p released, removed from registry", pThis);
        }
    }
    BmpDtor_Orig(pThis);
}

// ---------------- painter hooks ----------------
typedef long (*SetVisualStates_t)(void*, const void*, const void*, float);
static SetVisualStates_t SetVisualStates_Orig;
static long SetVisualStates_Hook(void* pThis, const void* a, const void* b, float f) {
    bool isNew = g_tlEnabled ? RegisterPair(a, b, f) : false;
    long r = SetVisualStates_Orig(pThis, a, b, f);
    if (g_tlEnabled) {
        // Paint after the original function so our pixels are not overwritten by DWM's own upload
        if (isNew) PaintAtlas();
        ProcessPending();
    }
    return r;
}

// Hover/pressed/disabled backgrounds (A, states 1..3) are replaced by A state 0 (transparent).
typedef void (*ImgSetBmp_t)(void*, void*);
static ImgSetBmp_t ImgSetBmp_Orig;
static void ImgSetBmp_Hook(void* img, void* bmp) {
    void* use = bmp;
    char kind = '?'; int state = -1; const void* st0 = nullptr;
    if (g_tlEnabled && bmp && Classify(bmp, &kind, &state, &st0)) {
        if (kind == 'A' && state >= 1 && state <= 3 && st0 &&
            Readable(st0, 0x70) && Readable(bmp, 0x70) &&
            *(void* const*)st0 == *(void* const*)bmp) use = (void*)st0;
    }
    ImgSetBmp_Orig(img, use);
}

// image -> last glyph bitmap B set on it (to read the button role from its width)
static void* g_imgKey[256];
static const void* g_imgBmp[256];
static int g_imgN = 0;
static SRWLOCK g_imgLock = SRWLOCK_INIT;

static void RememberImg(void* img, const void* bmp) {
    AcquireSRWLockExclusive(&g_imgLock);
    int i = 0;
    for (; i < g_imgN; i++) if (g_imgKey[i] == img) break;
    if (i == g_imgN) { if (g_imgN < 256) g_imgN++; else i = 255; }
    g_imgKey[i] = img; g_imgBmp[i] = bmp;
    ReleaseSRWLockExclusive(&g_imgLock);
}

static const void* LookupImg(void* img) {
    const void* r = nullptr;
    AcquireSRWLockShared(&g_imgLock);
    for (int i = 0; i < g_imgN; i++) if (g_imgKey[i] == img) { r = g_imgBmp[i]; break; }
    ReleaseSRWLockShared(&g_imgLock);
    return r;
}

typedef void (*SetGlyph_t)(void*, void*, int, int);
static SetGlyph_t SetGlyph_Orig;
static void SetGlyph_Hook(void* img, void* bmp, int align, int off) {
    if (g_tlEnabled && bmp) RememberImg(img, bmp);
    SetGlyph_Orig(img, bmp, align, off);
}

typedef void (*ImgSetSize_t)(void*, const SIZE*);
static ImgSetSize_t ImgSetSize_Orig;
static unsigned char g_seenW[256];   // button widths seen so far (index = width in px)
static void ImgSetSize_Hook(void* img, const SIZE* sz) {
    ImgSetSize_Orig(img, sz);
    if (g_tlEnabled && sz) {
        const void* bmp = LookupImg(img);
        if (bmp) {
            int cx = (int)sz->cx;
            if (cx == 46) CorrectRole(bmp, 1);           // 100%: middle button is 46 wide
            else if (cx == 47) CorrectRole(bmp, 0);      // minimize and close are 47 (close is left alone)
            else if (cx >= 30 && cx <= 250) {
                g_seenW[cx] = 1;
                bool lo = g_seenW[cx - 1] != 0;
                bool hi = g_seenW[cx + 1] != 0;
                // At other DPIs the middle button is 1 px narrower than minimize and close.
                // Only decide when exactly one neighbour width has been seen.
                if (hi && !lo) CorrectRole(bmp, 1);
                else if (lo && !hi) CorrectRole(bmp, 0);
                LogCat(5, 300, L"[ImgSize] bmp=%p cx=%d lo=%d hi=%d", bmp, cx, (int)lo, (int)hi);
            }
        }
    }
}

typedef void (*UpdatePos_t)(void*);
static UpdatePos_t UpdatePos_Orig;
static void UpdatePos_Hook(void* pThis) {
    UpdatePos_Orig(pThis);
    if (g_tlEnabled) {
        if (g_dirty) { g_dirty = 0; PaintAtlas(); }
        ProcessPending();
    }
}

// ======================================================================
//  PART 3: File Explorer tab bar (runs inside explorer.exe only)
// ======================================================================

static bool g_isExplorer = false;
static volatile LONG g_exEnabled = 1;
static int g_exExtra = 0;

struct TabWin { HWND hwnd; int applied; bool isTab; bool bypass; };
static TabWin g_tabs[32];
static SRWLOCK g_tabLock = SRWLOCK_INIT;

static void ExLog(const wchar_t* fmt, ...) {
    if (!g_settings.debugLog) return;
    static int s_count = 0;
    static wchar_t s_last[500] = L"";
    wchar_t buf[500];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf(buf, 499, fmt, args);
    va_end(args);
    buf[499] = 0;
    if (s_count >= 300 || wcscmp(buf, s_last) == 0) return;
    wcscpy(s_last, buf);
    s_count++;
    Wh_Log(L"%s", buf);
}

static void ExLoadSettings() {
    g_exEnabled = Wh_GetIntSetting(L"explorerTabs.enabled") ? 1 : 0;
    g_exExtra = Clamp(Wh_GetIntSetting(L"explorerTabs.extraPx"), -300, 300);
}

static int TabIndexLocked(HWND h) {
    for (int i = 0; i < 32; i++) if (g_tabs[i].hwnd == h) return i;
    return -1;
}

static bool TabAdd(HWND h) {
    bool added = false;
    AcquireSRWLockExclusive(&g_tabLock);
    if (TabIndexLocked(h) < 0) {
        for (int i = 0; i < 32; i++) {
            if (!g_tabs[i].hwnd) {
                g_tabs[i] = TabWin{};
                g_tabs[i].hwnd = h;
                added = true;
                break;
            }
        }
    }
    ReleaseSRWLockExclusive(&g_tabLock);
    return added;
}

static void TabRemove(HWND h) {
    AcquireSRWLockExclusive(&g_tabLock);
    int i = TabIndexLocked(h);
    if (i >= 0) g_tabs[i] = TabWin{};
    ReleaseSRWLockExclusive(&g_tabLock);
}

static bool TabIsBypass(HWND h) {
    bool b = false;
    AcquireSRWLockShared(&g_tabLock);
    int i = TabIndexLocked(h);
    if (i >= 0) b = g_tabs[i].bypass;
    ReleaseSRWLockShared(&g_tabLock);
    return b;
}

static void TabSetBypass(HWND h, bool v) {
    AcquireSRWLockExclusive(&g_tabLock);
    int i = TabIndexLocked(h);
    if (i >= 0) g_tabs[i].bypass = v;
    ReleaseSRWLockExclusive(&g_tabLock);
}

static void TabSetState(HWND h, int applied, bool isTab) {
    AcquireSRWLockExclusive(&g_tabLock);
    int i = TabIndexLocked(h);
    if (i >= 0) { g_tabs[i].applied = applied; g_tabs[i].isTab = isTab; }
    ReleaseSRWLockExclusive(&g_tabLock);
}

static int TabGetApplied(HWND h) {
    int a = 0;
    AcquireSRWLockShared(&g_tabLock);
    int i = TabIndexLocked(h);
    if (i >= 0) a = g_tabs[i].applied;
    ReleaseSRWLockShared(&g_tabLock);
    return a;
}

static int ExDpiOf(HWND h) {
    int dpi = 0;
    if (h && g_pGetDpiForWindow) dpi = (int)g_pGetDpiForWindow(h);
    if (dpi <= 0 && g_pGetDpiForSystem) dpi = (int)g_pGetDpiForSystem();
    return dpi > 0 ? dpi : 96;
}

// Window rectangle in the coordinates of its parent
static bool GetRectInParent(HWND h, RECT* out) {
    RECT r;
    if (!GetWindowRect(h, &r)) return false;
    HWND p = GetParent(h);
    if (p) MapWindowPoints(nullptr, p, (POINT*)&r, 2);
    *out = r;
    return true;
}

// Shift (px) to apply to the tab bar window of this top-level frame; 0 = leave it alone
static int ComputeTabShift(HWND h) {
    if (!g_exEnabled || !g_settings.moveButtons) return 0;
    HWND top = GetAncestor(h, GA_ROOT);
    if (!top) return 0;
    if (IsHwndExcluded(top)) return 0;
    int s = MulDiv(g_exExtra, ExDpiOf(top), 96);   // 0 = leave the tab bar and toolbar where Explorer puts them
    return s < 0 ? 0 : s;
}

static bool IsTabBridgeClass(const wchar_t* c) {
    return c && (wcsstr(c, L"DesktopChildSiteBridge") ||
                 wcsstr(c, L"DesktopWindowContentBridge") ||
                 wcsstr(c, L"InputNonClientPointerSource") ||
                 wcsstr(c, L"InputSiteWindowClass"));
}

static void AdjustTabPos(HWND hwnd, WINDOWPOS* p) {
    HWND parent = GetParent(hwnd);
    if (!parent) return;
    RECT cur = {};
    if (!GetRectInParent(hwnd, &cur)) return;
    HWND top = GetAncestor(hwnd, GA_ROOT);
    int dpi = ExDpiOf(top ? top : hwnd);

    wchar_t cls[128] = L"";
    GetClassNameW(hwnd, cls, 128);
    bool isInputLayer = wcsstr(cls, L"InputNonClientPointerSource") || wcsstr(cls, L"InputSiteWindowClass");

    bool noSize = (p->flags & SWP_NOSIZE) != 0;
    int cx = noSize ? (int)(cur.right - cur.left) : p->cx;
    int cy = noSize ? (int)(cur.bottom - cur.top) : p->cy;

    bool looksTab = isInputLayer || (p->y <= MulDiv(24, dpi, 96) && cy <= MulDiv(150, dpi, 96));
    int s = looksTab ? ComputeTabShift(hwnd) : 0;
    int prevApplied = TabGetApplied(hwnd);   // shift already applied to the current position
    TabSetState(hwnd, s, looksTab);
    if (s == 0) return;

    // Use the original X when WinUI resized the window without moving it
    int currentX = p->x;
    if (p->flags & SWP_NOMOVE) currentX = cur.left - prevApplied;                       // resize only: strip our own shift
    else if (prevApplied != 0 && p->x == cur.left) currentX = p->x - prevApplied;       // our shifted position echoed back
    int nx = currentX + s;

    RECT pc = {};
    if (GetClientRect(parent, &pc) && pc.right > 0 && !(top && IsIconic(top)) && nx + cx > pc.right) {
        cx = pc.right - nx;
        if (cx < 0) cx = 0;
        p->cx = cx;
        p->cy = cy;
        p->flags &= ~SWP_NOSIZE;
    }

    p->x = nx;
    p->flags &= ~SWP_NOMOVE;  // required: make Windows accept the new X
    ExLog(L"[TabMove] hwnd=%p origX=%d shift=%d -> x=%d y=%d cx=%d cy=%d dpi=%d", hwnd, currentX, s, nx, p->y, cx, cy, dpi);
}

static void ApplyButtonHole(HWND h) {
    static thread_local bool busy = false;
    if (busy) return;

    HWND top = GetAncestor(h, GA_ROOT);
    if (!top || GetParent(h) != top) return;          // only islands directly under the Explorer frame

    wchar_t cls[128] = L"";
    GetClassNameW(h, cls, 128);
    if (!wcsstr(cls, L"DesktopChildSiteBridge")) return;

    // Buttons not on the left, tab bar move off, or app excluded:
    // no hole, and remove any hole that was made earlier
    if (!g_exEnabled || !g_settings.moveButtons || IsHwndExcluded(top)) {
        HRGN cur = CreateRectRgn(0, 0, 0, 0);
        bool hasRgn = GetWindowRgn(h, cur) != ERROR;
        DeleteObject(cur);
        if (hasRgn) {
            busy = true;
            SetWindowRgn(h, nullptr, TRUE);
            busy = false;
        }
        return;
    }

    RECT wr, tr;
    if (!GetWindowRect(h, &wr) || !GetWindowRect(top, &tr)) return;
    int dpi = ExDpiOf(top);
    int w = wr.right - wr.left, hgt = wr.bottom - wr.top;
    if (w <= 0 || hgt <= 0) return;
    if (wr.top - tr.top > MulDiv(40, dpi, 96)) return; // island below the title bar (folder content): skip

    int baseOut = IsZoomed(top) ? 8 : 1;
    int bx1 = tr.left + baseOut + EffLeftMargin(top) - wr.left;
    int bwH = MulDiv(46, dpi, 96);
    int bx2 = bx1 + bwH * 3 - 2 * TightenPx(dpi, bwH);
    int by1 = -10000;
    int by2 = tr.top + MulDiv(35, dpi, 96) + baseOut - wr.top;

    busy = true;
    HRGN full = CreateRectRgn(0, 0, w, hgt);
    HRGN hole = CreateRectRgn(bx1, by1, bx2, by2);
    CombineRgn(full, full, hole, RGN_DIFF);
    DeleteObject(hole);
    SetWindowRgn(h, full, TRUE);   // the system takes ownership of 'full'; do not delete it
    busy = false;
}

typedef HRESULT (WINAPI *DwmDefWindowProc_t)(HWND, UINT, WPARAM, LPARAM, LRESULT*);

// DwmDefWindowProc is what keeps the caption button hover/press look in sync with the pointer.
static bool CallDwmDefWindowProc(HWND h, UINT msg, WPARAM wp, LPARAM lp, LRESULT* res) {
    static DwmDefWindowProc_t pFn = nullptr;
    if (!pFn) {
        HMODULE hDwmApi = GetModuleHandleW(L"dwmapi.dll");
        if (!hDwmApi) hDwmApi = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hDwmApi) pFn = (DwmDefWindowProc_t)GetProcAddress(hDwmApi, "DwmDefWindowProc");
    }
    return pFn && pFn(h, msg, wp, lp, res) != 0;
}

// True only when buttons are on the left AND this window is not excluded.
// Excluded windows keep the default Windows behavior (no hit-test tricks).
static bool LeftActiveFor(HWND anyWnd) {
    if (!g_settings.moveButtons) return false;
    HWND top = GetAncestor(anyWnd, GA_ROOT);
    if (!top) top = anyWnd;
    return !IsHwndExcluded(top);
}

static LRESULT TabSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (msg == WM_NCHITTEST && LeftActiveFor(hwnd)) {
        HWND top = GetAncestor(hwnd, GA_ROOT);
        RECT rw;
        if (top && GetWindowRect(top, &rw)) {
            int x = (int)(short)LOWORD(lParam) - rw.left;
            int y = (int)(short)HIWORD(lParam) - rw.top;
            int dpi = ExDpiOf(top);
            int baseOut = IsZoomed(top) ? 8 : 1;
            int topBand = MulDiv(35, dpi, 96) + baseOut;
            int b1 = baseOut + EffLeftMargin(top);
            int bwT = MulDiv(46, dpi, 96);
            int b4 = b1 + bwT * 3 - 2 * TightenPx(dpi, bwT);
            if (y >= 0 && y <= topBand && x >= b1 && x < b4) return HTTRANSPARENT;
            // Pointer is over the tab bar, not over a button: tell DWM, so a button that was hot loses its hover look
            LRESULT dr = 0;
            CallDwmDefWindowProc(top, WM_NCHITTEST, wParam, lParam, &dr);
        }
    }
    if (msg == WM_WINDOWPOSCHANGING) {
        WINDOWPOS* p = (WINDOWPOS*)lParam;
        if (p && !TabIsBypass(hwnd)) AdjustTabPos(hwnd, p);
    } else if (msg == WM_NCDESTROY) {
        TabRemove(hwnd);
    }
    LRESULT res = DefSubclassProc(hwnd, msg, wParam, lParam);
    if (msg == WM_WINDOWPOSCHANGED || msg == WM_SIZE) ApplyButtonHole(hwnd);
    return res;
}

// Re-send the current position so the subclass applies the shift
static void TabNudge(HWND h) {
    RECT r;
    if (!GetRectInParent(h, &r)) return;
    if (r.right <= r.left || r.bottom <= r.top) return;
    SetWindowPos(h, nullptr, r.left - TabGetApplied(h), r.top, r.right - r.left, r.bottom - r.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    ApplyButtonHole(h);
}

// Move by a delta without letting the subclass add the shift again
static void TabMoveBy(HWND h, int delta) {
    if (!delta) return;
    RECT r;
    if (!GetRectInParent(h, &r)) return;
    TabSetBypass(h, true);
    SetWindowPos(h, nullptr, r.left + delta, r.top, r.right - r.left, r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
    TabSetBypass(h, false);
}

// Subclass the window when it is a XAML island inside a File Explorer frame. True = newly attached.
static bool TryAttach(HWND h) {
    wchar_t cls[128] = L"";
    if (!GetClassNameW(h, cls, 128) || !IsTabBridgeClass(cls)) return false;
    HWND top = GetAncestor(h, GA_ROOT);
    if (!top || top == h) return false;
    wchar_t tc[64] = L"";
    GetClassNameW(top, tc, 64);
    if (wcscmp(tc, L"CabinetWClass") != 0) return false;
    if (!TabAdd(h)) return false;

    // Do not report the window as attached unless Windhawk actually installed
    // the subclass. Otherwise the HWND stays in g_tabs and future scans won't
    // retry, leaving the tab bar over the caption buttons.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(h, TabSubclassProc, 0)) {
        ExLog(L"[AttachFailed] hwnd=%p class=%s top=%p", h, cls, top);
        TabRemove(h);
        return false;
    }

    ExLog(L"[AttachOK] hwnd=%p class=%s top=%p", h, cls, top);
    return true;
}

typedef HWND (WINAPI *CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Orig;

static const UINT_PTR kRescanTimerId = 0x4C46;
static UINT g_msgCleanup = 0;   // registered in ExplorerInit; asks the subclass to remove itself
static BOOL CALLBACK ExChildProc(HWND h, LPARAM);
static int TabSnapshot(HWND* hw, int* applied, bool* isTab);

static void StartRescan(HWND cab) {
    SetPropW(cab, L"CaptionButtonsTicks", (HANDLE)(INT_PTR)0);
    SetTimer(cab, kRescanTimerId, 150, nullptr);
}

static LRESULT CALLBACK CabinetSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (g_msgCleanup && msg == g_msgCleanup) {
        KillTimer(hwnd, kRescanTimerId);
        RemovePropW(hwnd, L"CaptionButtonsTicks");
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, CabinetSubclassProc);
        return 0;
    }
    if (msg == WM_TIMER && wParam == kRescanTimerId) {
        int ticks = (int)(INT_PTR)GetPropW(hwnd, L"CaptionButtonsTicks") + 1;
        SetPropW(hwnd, L"CaptionButtonsTicks", (HANDLE)(INT_PTR)ticks);
        EnumChildWindows(hwnd, ExChildProc, 0);
        if (ticks >= 12) {   // 12 x 150 ms = about 1.8 seconds
            KillTimer(hwnd, kRescanTimerId);
            RemovePropW(hwnd, L"CaptionButtonsTicks");
        }
        return 0;
    }
    if (msg == WM_SHOWWINDOW && wParam) {
        StartRescan(hwnd);
    }

    // Let DWM see the pointer leaving, so the hover look of a caption button is cleared
    if (LeftActiveFor(hwnd) &&
        (msg == WM_NCMOUSELEAVE || msg == WM_MOUSELEAVE || msg == WM_NCMOUSEMOVE)) {
        LRESULT dr = 0;
        CallDwmDefWindowProc(hwnd, msg, wParam, lParam, &dr);
    }

    if (msg == WM_NCHITTEST && LeftActiveFor(hwnd)) {
        POINT pt = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) };
        RECT rw;
        if (GetWindowRect(hwnd, &rw)) {
            int x = pt.x - rw.left;
            int y = pt.y - rw.top;
            int dpi = ExDpiOf(hwnd);
            int baseOut = IsZoomed(hwnd) ? 8 : 1;
            int topBand = MulDiv(35, dpi, 96) + baseOut;

            if (y >= 0 && y <= topBand) {
                int lm = EffLeftMargin(hwnd);
                int bw = MulDiv(46, dpi, 96);
                int Tx = TightenPx(dpi, bw);
                int b1 = baseOut + lm;
                int b4 = b1 + bw * 3 - 2 * Tx;
                if (x >= b1 && x < b4) {

                    // Let DWM answer the hit-test so it keeps drawing the hover animation
                    LRESULT lRes = 0;
                    if (CallDwmDefWindowProc(hwnd, msg, wParam, lParam, &lRes)) {
                        return lRes;
                    }

                    int idx = (x - b1) / (bw - Tx);
                    if (idx > 2) idx = 2;
                    int o = g_settings.order;
                    int type = 0;
                    if (o == 0) type = (idx == 0) ? 3 : (idx == 1 ? 1 : 2);
                    else if (o == 1) type = (idx == 0) ? 3 : (idx == 1 ? 2 : 1);
                    else type = (idx == 0) ? 1 : (idx == 1 ? 2 : 3);

                    if (type == 3) return HTCLOSE;
                    if (type == 2) return HTMAXBUTTON;
                    if (type == 1) return HTMINBUTTON;
                }
            }
        }
    }
    if (msg == WM_NCLBUTTONDOWN && LeftActiveFor(hwnd)) {
        if (wParam == HTCLOSE) { PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, lParam); return 0; }
        if (wParam == HTMINBUTTON) { PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, lParam); return 0; }
        if (wParam == HTMAXBUTTON) { PostMessage(hwnd, WM_SYSCOMMAND, IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE, lParam); return 0; }
    }
    if (msg == WM_NCDESTROY) {
        KillTimer(hwnd, kRescanTimerId);
        RemovePropW(hwnd, L"CaptionButtonsTicks");
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, CabinetSubclassProc);
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static HWND WINAPI CreateWindowExW_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                         HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND r = CreateWindowExW_Orig(ex, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (r && !IS_INTRESOURCE(cls)) {
        if (parent && IsTabBridgeClass(cls)) {
            if (TryAttach(r)) TabNudge(r);
        } else if (!parent && wcscmp(cls, L"CabinetWClass") == 0) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(r, CabinetSubclassProc, 0);
            StartRescan(r);
        }
    }
    return r;
}

// Existing windows: attach and apply. Also lists every child window (debug log) so the
// right class can be identified if the island is not found.
static BOOL CALLBACK ExChildProc(HWND h, LPARAM) {
    if (g_settings.debugLog) {
        wchar_t cls[128] = L"";
        GetClassNameW(h, cls, 128);
        RECT r = {};
        GetRectInParent(h, &r);
        ExLog(L"[Child] hwnd=%p class=%s rect=(%ld,%ld,%ld,%ld)", h, cls, r.left, r.top, r.right, r.bottom);
    }
    if (TryAttach(h)) {
        TabNudge(h);
    } else {
        // Already attached but not shifted yet: force the shift
        HWND hw[32]; int applied[32]; bool isTab[32];
        int n = TabSnapshot(hw, applied, isTab);
        for (int i = 0; i < n; i++) {
            if (hw[i] == h && applied[i] == 0) { TabNudge(h); break; }
        }
    }
    {
        wchar_t c2[128] = L"";
        GetClassNameW(h, c2, 128);
        if (wcsstr(c2, L"DesktopChildSiteBridge")) ApplyButtonHole(h);
    }
    return TRUE;
}

static BOOL CALLBACK ExTopProc(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[64] = L"";
    GetClassNameW(h, cls, 64);
    if (wcscmp(cls, L"CabinetWClass") == 0) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(h, CabinetSubclassProc, 0);
        EnumChildWindows(h, ExChildProc, 0);
    }
    return TRUE;
}

static int TabSnapshot(HWND* hw, int* applied, bool* isTab) {
    int n = 0;
    AcquireSRWLockShared(&g_tabLock);
    for (int i = 0; i < 32; i++) {
        if (g_tabs[i].hwnd) {
            hw[n] = g_tabs[i].hwnd;
            applied[n] = g_tabs[i].applied;
            isTab[n] = g_tabs[i].isTab;
            n++;
        }
    }
    ReleaseSRWLockShared(&g_tabLock);
    return n;
}

// After a settings change: move every known tab bar to the shift of the new settings
static void ExplorerReapply() {
    HWND hw[32];
    int applied[32];
    bool isTab[32];
    int n = TabSnapshot(hw, applied, isTab);
    for (int i = 0; i < n; i++) {
        if (!IsWindow(hw[i])) continue;
        if (!isTab[i]) { TabNudge(hw[i]); continue; }
        int desired = ComputeTabShift(hw[i]);
        int delta = desired - applied[i];
        if (delta) {
            TabMoveBy(hw[i], delta);
            TabSetState(hw[i], desired, true);
        }
    }
}

// Removes CabinetSubclassProc from every File Explorer frame of this process.
// The rescan timer and the window property belong to the window's thread, so
// the subclass cleans them up itself when it receives g_msgCleanup.
static BOOL CALLBACK ExTopUninitProc(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[64] = L"";
    GetClassNameW(h, cls, 64);
    if (wcscmp(cls, L"CabinetWClass") != 0) return TRUE;

    DWORD_PTR res = 0;
    if (!g_msgCleanup || !SendMessageTimeoutW(h, g_msgCleanup, 0, 0, SMTO_ABORTIFHUNG, 1000, &res)) {
        // The window did not answer: remove the subclass directly
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, CabinetSubclassProc);
    }
    return TRUE;
}

static void ExplorerUninit() {
    EnumWindows(ExTopUninitProc, 0);
    HWND hw[32];
    int applied[32];
    bool isTab[32];
    int n = TabSnapshot(hw, applied, isTab);
    for (int i = 0; i < n; i++) {
        if (!IsWindow(hw[i])) continue;
        if (applied[i]) TabMoveBy(hw[i], -applied[i]);   // put the tab bar back (Explorer re-lays it out on the next resize)
        SetWindowRgn(hw[i], nullptr, TRUE);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hw[i], TabSubclassProc);
        TabRemove(hw[i]);
    }
}

// ---------------- icon storage writer (explorer.exe) ----------------
// explorer.exe can read the user's folders, dwm.exe usually cannot. So the images are
// decoded and shrunk here, and the raw pixels are handed to dwm.exe through the mod's
// storage (see IconValueName). dwm.exe never opens or decodes an image file.
static void SyncIconCache() {
    g_debug = g_settings.debugLog ? 1 : 0;   // DecodeImage reports through LogCat

    IconSettings s;
    ReadIconSettings(&s);   // s.any is false unless the Custom images style is selected

    IconImg img[4][4] = {};
    if (s.any) {
        HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        IWICImagingFactory* fac = nullptr;
        HRESULT hr = CoCreateInstance(kCLSID_WICFactory, nullptr, CLSCTX_INPROC_SERVER, kIID_WICFactory, (void**)&fac);
        if (SUCCEEDED(hr) && fac) {
            for (int kind = 0; kind < 4; kind++) {
                for (int st = 0; st < 4; st++) {
                    wchar_t cand[MAX_PATH + 32];
                    if (FindIconFile(s, kind, st, cand)) DecodeImage(fac, cand, &img[kind][st]);
                }
            }
            fac->Release();
        } else {
            ExLog(L"[IconCache] CoCreateInstance WIC failed hr=0x%lx", (unsigned long)hr);
        }
        if (SUCCEEDED(hrInit)) CoUninitialize();
    }

    int stored = 0;
    for (int kind = 0; kind < 4; kind++) {
        for (int st = 0; st < 4; st++) {
            wchar_t name[32];
            IconValueName(kind, st, name, 32);
            IconImg& im = img[kind][st];
            bool ok = false;
            if (im.px) {
                size_t bytes = (size_t)im.w * im.h * 4;
                BYTE* blob = (BYTE*)malloc(8 + bytes);
                if (blob) {
                    memcpy(blob, &im.w, 4);
                    memcpy(blob + 4, &im.h, 4);
                    memcpy(blob + 8, im.px, bytes);
                    ok = Wh_SetBinaryValue(name, blob, 8 + bytes) != FALSE;
                    free(blob);
                }
                free(im.px);
            }
            if (ok) stored++;
            else Wh_DeleteValue(name);   // setting removed, file gone or store failed: no stale image
        }
    }
    // Tell dwm.exe to reload
    Wh_SetIntValue(L"iconCacheVersion", Wh_GetIntValue(L"iconCacheVersion", 0) + 1);
    ExLog(L"[IconCache] synced, %d image(s) stored", stored);
}

// Removes the stored images (called when the mod is unloaded).
static void CleanIconCache() {
    for (int kind = 0; kind < 4; kind++) {
        for (int st = 0; st < 4; st++) {
            wchar_t name[32];
            IconValueName(kind, st, name, 32);
            Wh_DeleteValue(name);
        }
    }
}

static void LoadUser32Procs() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        g_pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
        g_pGetDpiForSystem = (GetDpiForSystem_t)GetProcAddress(user32, "GetDpiForSystem");
        g_pSPIForDpi = (SPIForDpi_t)GetProcAddress(user32, "SystemParametersInfoForDpi");
        g_pInternalGetWindowText = (InternalGetWindowText_t)GetProcAddress(user32, "InternalGetWindowText");
        g_pIsHungAppWindow = (IsHungAppWindow_t)GetProcAddress(user32, "IsHungAppWindow");
    }
}

static BOOL ExplorerInit() {
    LoadSettings();     // moveButtons, leftMargin, exclusions
    SyncIconCache();    // copy custom images where dwm.exe can read them
    ExLoadSettings();
    LoadUser32Procs();
    g_msgCleanup = RegisterWindowMessageW(L"CaptionButtonsCustomizer_RemoveSubclass");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);
    return TRUE;
}

// ======================================================================
//  Lifecycle (all parts)
// ======================================================================
// Asks DWM to refresh itself (as if the system colors changed). DWM then lays out every
// window again, so no window has to be resized.
static void RequestDwmRefresh() {
    HWND hDwm = FindWindowW(L"Dwm", nullptr);
    if (!hDwm) {
        Wh_Log(L"DWM notification window wasn't found");
        return;
    }
    PostMessageW(hDwm, WM_SYSCOLORCHANGE, 0, 0);
}
static unsigned long long Fnv(unsigned long long h, unsigned long long v) {
    h ^= v;
    h *= 1099511628211ull;
    return h;
}

// Hash of every setting that changes where DWM puts the buttons, icon and title,
// including the exclusion lists. Colors, images, the restart option and the
// debug log are not part of it, so changing them does not trigger a relayout.
static unsigned long long LayoutSignature() {
    unsigned long long h = 1469598103934665603ull;
    h = Fnv(h, (unsigned long long)(long long)g_settings.moveButtons);
    h = Fnv(h, (unsigned long long)(long long)g_settings.leftMargin);
    h = Fnv(h, (unsigned long long)(long long)g_settings.tighten);
    h = Fnv(h, (unsigned long long)(long long)g_settings.explorerMargin);
    h = Fnv(h, (unsigned long long)(long long)g_settings.order);
    h = Fnv(h, (unsigned long long)(long long)g_settings.showDialogClose);
    h = Fnv(h, (unsigned long long)(long long)g_settings.titleAlign);
    h = Fnv(h, (unsigned long long)(long long)g_settings.dialogAlign);
    h = Fnv(h, (unsigned long long)(long long)g_settings.centerOnText);
    h = Fnv(h, (unsigned long long)(long long)g_settings.centerNudge);
    h = Fnv(h, (unsigned long long)(long long)g_settings.iconPosition);
    h = Fnv(h, (unsigned long long)(long long)g_settings.iconSide);
    h = Fnv(h, (unsigned long long)(long long)g_settings.titleGap);
    h = Fnv(h, (unsigned long long)(long long)g_settings.iconTitleGap);
    h = Fnv(h, (unsigned long long)(long long)g_settings.rightEdgeGap);
    h = Fnv(h, (unsigned long long)(long long)g_settings.hideIcon);
    h = Fnv(h, (unsigned long long)(long long)g_settings.hideTitle);

    AcquireSRWLockShared(&g_exclLock);
    h = Fnv(h, (unsigned long long)g_exclExeN);
    for (int i = 0; i < g_exclExeN; i++) {
        for (const wchar_t* p = g_exclExe[i]; *p; p++) h = Fnv(h, (unsigned long long)*p);
        h = Fnv(h, 0);
    }
    h = Fnv(h, (unsigned long long)g_exclClsN);
    for (int i = 0; i < g_exclClsN; i++) {
        for (const wchar_t* p = g_exclCls[i]; *p; p++) h = Fnv(h, (unsigned long long)*p);
        h = Fnv(h, 0);
    }
    ReleaseSRWLockShared(&g_exclLock);
    return h;
}

// Hash of the settings that change how the buttons are painted (not where they are).
static unsigned long long PaintSignature() {
    unsigned long long h = 1469598103934665603ull;
    h = Fnv(h, (unsigned long long)(long long)g_tlEnabled);
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            h = Fnv(h, (unsigned long long)(long long)g_base[r][c]);
    h = Fnv(h, (unsigned long long)g_styleCustom);
    return h;
}

void Wh_ModSettingsChanged() {
    if (g_isExplorer) {
        LoadSettings();
        ExLoadSettings();
        SyncIconCache();
        ExplorerReapply();
        EnumWindows(ExTopProc, 0);
        return;
    }
    LONG iconVerBefore = (LONG)Wh_GetIntValue(L"iconCacheVersion", 0);
    bool wasCustom = g_styleCustom != 0;
    unsigned long long sigBefore = LayoutSignature();
    unsigned long long paintBefore = PaintSignature();
    LoadSettings();       // layout (including the excluded apps lists)
    TL_LoadSettings();    // painter
    InterlockedExchange(&g_pending, 1);   // repaint on the next DWM-thread hook call
    // Open windows need a new layout when a layout setting (or the exclusion lists) or the
    // painting changed. DWM refreshes itself and lays out every window again; no window is resized.
    bool refresh = (LayoutSignature() != sigBefore) || (PaintSignature() != paintBefore);

    // Custom images: explorer.exe decodes the images and bumps iconCacheVersion.
    // Wait for it (max ~2 s), so the DWM thread loads and paints them on the refresh.
    if (g_tlEnabled && (wasCustom || g_styleCustom)) {
        for (int i = 0; i < 40 && (LONG)Wh_GetIntValue(L"iconCacheVersion", 0) == iconVerBefore; i++) {
            Sleep(50);
        }
        InterlockedExchange(&g_pending, 1);
        refresh = true;
    }

    if (refresh) RequestDwmRefresh();
}

// Called by Windhawk just before the mod is unloaded, while the hooks still work
void Wh_ModBeforeUninit() {
    if (g_isExplorer) return;
    InterlockedExchange(&g_unloading, 1);
    // DWM lays every window out again; with g_unloading set the hooks restore the default layout
    RequestDwmRefresh();
    for (int i = 0; i < 25 && g_unloadRelayouts == 0; i++) Sleep(20);   // wait up to ~0.5 second
    if (g_unloadRelayouts > 0) Sleep(50);                               // let the layout pass finish
    Wh_Log(L"[Unload] DWM refresh requested, %ld layout(s) handled", (long)g_unloadRelayouts);
}

// dwm.exe starts before explorer.exe, so the images are not in the mod storage yet when
// dwm.exe loads the mod. This thread waits until explorer.exe has stored them (it bumps
// iconCacheVersion), then asks DWM to refresh so that the images are loaded and painted.
static HANDLE g_iconWaitThread = nullptr;

static DWORD WINAPI IconWaitThread(LPVOID) {
    for (int i = 0; i < 100 && !g_unloading; i++) {   // up to ~10 seconds
        Sleep(100);
        if ((LONG)Wh_GetIntValue(L"iconCacheVersion", 0) != g_iconVerSeen) {
            InterlockedExchange(&g_pending, 1);
            RequestDwmRefresh();
            break;
        }
    }
    return 0;
}

void Wh_ModAfterInit() {
    if (g_isExplorer) {
        EnumWindows(ExTopProc, 0);   // File Explorer windows that already exist
    } else {
        RequestDwmRefresh();         // windows that are already open get the new layout right away
        if (g_tlEnabled && g_styleCustom) {
            g_iconWaitThread = CreateThread(nullptr, 0, IconWaitThread, nullptr, 0, nullptr);
        }
    }
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        ExplorerUninit();
        CleanIconCache();
        return;
    }
    if (g_iconWaitThread) {
        InterlockedExchange(&g_unloading, 1);   // make sure the thread stops even if Wh_ModBeforeUninit was skipped
        WaitForSingleObject(g_iconWaitThread, INFINITE);   // the thread checks g_unloading every 100 ms
        CloseHandle(g_iconWaitThread);
        g_iconWaitThread = nullptr;
    }
    if (g_menuTimer) {
        SetThreadpoolTimer(g_menuTimer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(g_menuTimer, TRUE);
        CloseThreadpoolTimer(g_menuTimer);
        g_menuTimer = nullptr;
    }
    if (g_nudgeThread) {
        InterlockedExchange(&g_unloading, 1);
        WaitForSingleObject(g_nudgeThread, INFINITE);   // the thread checks g_unloading every 100 ms and sends at most two 300 ms messages
        CloseHandle(g_nudgeThread);
        g_nudgeThread = nullptr;
    }
    if (g_everPainted && g_restartOnUnload) RestartDwm();
    FreeIcons();
}

// Crash-loop guard: refuse to load when dwm.exe started 4 or more times within 60 seconds.
// Recompiling the mod inside the same dwm.exe does not count (same process start time).
static bool IsDwmRestartLoop() {
    FILETIME ct, et, kt, ut, nowFt;
    if (!GetProcessTimes(GetCurrentProcess(), &ct, &et, &kt, &ut)) return false;
    GetSystemTimeAsFileTime(&nowFt);
    ULONGLONG start = ((ULONGLONG)ct.dwHighDateTime << 32) | ct.dwLowDateTime;
    ULONGLONG now = ((ULONGLONG)nowFt.dwHighDateTime << 32) | nowFt.dwLowDateTime;
    const ULONGLONG kWindow = 60ULL * 10000000ULL;

    ULONGLONG stored[8] = {};
    size_t got = Wh_GetBinaryValue(L"dwmStartTimes", stored, sizeof(stored));
    int storedN = (int)(got / sizeof(ULONGLONG));
    if (storedN > 8) storedN = 8;

    ULONGLONG keep[8] = {};
    int n = 0;
    for (int i = 0; i < storedN; i++) {
        if (stored[i] != start && now >= stored[i] && now - stored[i] <= kWindow && n < 7)
            keep[n++] = stored[i];
    }
    keep[n++] = start;
    Wh_SetBinaryValue(L"dwmStartTimes", keep, n * sizeof(ULONGLONG));
    return n >= 4;
}

BOOL Wh_ModInit() {
    // The mod runs in two processes: dwm.exe (buttons, glyphs) and explorer.exe (tab bar)
    wchar_t exePath[MAX_PATH] = L"";
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;
    g_isExplorer = _wcsicmp(exeName, L"explorer.exe") == 0;
    if (g_isExplorer) return ExplorerInit();
    if (IsDwmRestartLoop()) {
        Wh_Log(L"dwm.exe started 4 or more times within a minute, the mod stays unloaded for safety");
        return FALSE;
    }

    LoadSettings();
    TL_LoadSettings();

    LoadUser32Procs();

    HMODULE udwm = GetModuleHandleW(L"udwm.dll");
    if (!udwm) return FALSE;

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        // ---- Layout ----
        { { LR"(public: void __cdecl CTopLevelWindow::GetTitleBarInfo(struct tagTITLEBARINFOEX *,bool *))" },
          (void**)&GetTitleBarInfo_Orig, nullptr, false },
        { { LR"(private: long __cdecl CTopLevelWindow::UpdateNCAreaButton(enum CTopLevelWindow::ButtonType,int,int,int *))" },
          (void**)&UpdateNCAreaButton_Orig, (void*)UpdateNCAreaButton_Hook, false },
        { { LR"(private: long __cdecl CTopLevelWindow::UpdateNCAreaPositionsAndSizes(void))" },
          (void**)&PosAndSizes_Orig, (void*)PosAndSizes_Hook, true },
        { { LR"(public: void __cdecl CVisual::SetInsetFromParentTop(int))" },
          (void**)&SetInsetTop_Orig, (void*)SetInsetTop_Hook, false },
        { { LR"(public: void __cdecl CVisual::SetInsetFromParentLeft(int))" },
          (void**)&SetInsetLeft_Orig, (void*)SetInsetLeft_Hook, false },
        { { LR"(public: void __cdecl CVisual::SetInsetFromParentRight(int))" },
          (void**)&SetInsetRight_Orig, (void*)SetInsetRight_Hook, false },
        { { LR"(public: virtual bool __cdecl CTopLevelWindow::DoHitTest(struct tagPOINT const &,class CVisual * *,unsigned int *))" },
          (void**)&DoHitTest_Orig, (void*)DoHitTest_Hook, true },
        { { LR"(private: long __cdecl CTopLevelWindow::UpdateText(struct CTopLevelWindow::WindowFrame *))" },
          (void**)&UpdateText_Orig, (void*)UpdateText_Hook, true },
        { { LR"(public: void __cdecl CTopLevelWindow::OnTitleUpdated(void))" },
          (void**)&OnTitleUpdated_Orig, (void*)OnTitleUpdated_Hook, true },

        // ---- HWND lookup (address only, no hook) ----
        { { LR"(public: class CWindowData * __cdecl CTopLevelWindow::GetWindowData(void)const )" },
          (void**)&GetWindowData_Orig, nullptr, true },
        { { LR"(public: bool __cdecl CWindowData::IsGhostWindow(struct HWND__ * *)const )" },
          (void**)&IsGhostWindow_Func, nullptr, true },
        // ---- Glyph painter ----
        { { LR"(public: long __cdecl CButton::SetVisualStates(class CBitmapSourceArray const &,class CBitmapSourceArray const &,float))" },
          (void**)&SetVisualStates_Orig, (void*)SetVisualStates_Hook, true },
        { { LR"(public: void __cdecl CAtlasedImage::SetBitmapSource(class CBitmapSource *))" },
          (void**)&ImgSetBmp_Orig, (void*)ImgSetBmp_Hook, true },
        { { LR"(public: void __cdecl CAtlasButton::SetGlyphImage(class CBitmapSource *,enum CAtlasButton::VerticalAlignment,int))" },
          (void**)&SetGlyph_Orig, (void*)SetGlyph_Hook, true },
        { { LR"(public: void __cdecl CAtlasedImage::SetSize(struct tagSIZE const &))" },
          (void**)&ImgSetSize_Orig, (void*)ImgSetSize_Hook, true },
        { { LR"(private: void __cdecl CButton::UpdateAtlasPositionAndSize(void))" },
          (void**)&UpdatePos_Orig, (void*)UpdatePos_Hook, true },

        // Address only (no hook): called by PaintAtlas
        { { LR"(private: long __cdecl CBitmapSource::Update(struct IWICBitmap *))" },
          (void**)&g_updateBmp, nullptr, true },
        // Frees of glyph objects: drop them from our registries
        { { LR"(protected: virtual __cdecl CBitmapSource::~CBitmapSource(void))" },
          (void**)&BmpDtor_Orig, (void*)BmpDtor_Hook, true },
    };

    BOOL ok = WindhawkUtils::HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks));
    if (ok) g_menuTimer = CreateThreadpoolTimer(MenuTimerCallback, nullptr, nullptr);
    g_hasPosHook = (PosAndSizes_Orig != nullptr);
    DeriveHwndOffset();

    if (!g_updateBmp) Wh_Log(L"CBitmapSource::Update not found, custom button styles will not be painted");
    if (!BmpDtor_Orig) Wh_Log(L"CBitmapSource destructor not found, freed glyph objects are not tracked");
    return ok;
}
