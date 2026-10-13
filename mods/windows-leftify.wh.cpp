// ==WindhawkMod==
// @id              windows-leftify
// @name            Windows Leftify
// @description     Move the caption buttons to the left, reposition the window icon and title, and optionally draw macOS-style traffic lights (or your own button images).
// @version         1.0
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
# Windows Leftify

Moves the caption buttons (minimize, maximize, close) to the left side of the title bar and repositions the window icon and title to match. It can also draw macOS-style traffic lights (red / yellow / green circles) or your own button images.

It works by hooking internal functions of the Desktop Window Manager (`dwm.exe` / `uDWM.dll`), so it affects every standard Windows window.

# Features & Preview

## Caption Buttons
**Buttons on the left**, with a left margin and three order presets: close / minimize / maximize (macOS), close / maximize / minimize, minimize / maximize / close.

### Move caption buttons to the left = on
Turn on *Move caption buttons to the left* to move the buttons to the left side of the title bar. When it is off, they stay in their default place on the right.

| Before | After |
|---------|---------|
| ![Before](https://i.imgur.com/TPtC4TD.png) | ![After](https://i.imgur.com/uhoUYhp.png) |

### Button order
Pick one of three presets in the *Button order* setting. The order is counted from the left edge of the window.

| Close, minimize, maximize | Close, maximize, minimize | Minimize, maximize, close |
|---------------------------|---------------------------|---------------------------|
| ![1](https://i.imgur.com/uhoUYhp.png) | ![2](https://i.imgur.com/bdPirba.png) | ![3](https://i.imgur.com/5ZXtvpB.png) |

### Show the close button on dialogs
Windows with only a close button (message boxes, About windows, Properties dialogs) and very narrow windows are handled. The close button of dialogs can be hidden with *Show the close button on dialogs*, and their icon and title can have their own alignment.

| Dialog On | Dialog Off |
|--------|-------|
| ![Dialog On](https://i.imgur.com/IlUb9kF.png) | ![Dialog Off](https://i.imgur.com/Iq9MAWu.png) |

## Icon and title text
**Flexible icon and title layout:** next to the buttons, centered on the window, or right-aligned; icon on either side of the title, far left or far right; hide the icon, the title, or both.

### Icon and title alignment
*Icon and title alignment* decides where the icon and title sit in the title bar. Left puts them right after the buttons, Center puts them in the middle of the window, and Right puts them at the right edge.

| Left | Center | Right |
|------|--------|-------|
| ![Left](https://i.imgur.com/uhoUYhp.png) | ![Center](https://i.imgur.com/DsKWlzm.png) | ![Right](https://i.imgur.com/ja4fJzt.png) |

### Icon Position
*Icon position* decides where the window icon goes. Left or right of the title keeps the icon attached to the title, while far left or far right pins the icon to that edge.

To pin the icon to the left edge of the window instead of right after the buttons, turn off *Move caption buttons to the left*. To pin it to the right edge, turn that option on (with it off, a far right icon sits next to the buttons on the right instead). For the best look with an icon pinned to a corner, set *Icon and title alignment* to Center.

| Left of the title | Right of the title |
|-------------------|--------------------|
| ![Left of the title](https://i.imgur.com/DsKWlzm.png) | ![Right of the title](https://i.imgur.com/77UoaGz.png) |
| **Far left** | **Far right** |
| ![Far left](https://i.imgur.com/8djx87p.png) | ![Far right](https://i.imgur.com/tUx3iGd.png) |

### Hide window icon and title
Use *Hide window icon and title* to hide only the icon, only the title, or both.

| Hide icon only | Hide title only | Hide both |
|----------------|-----------------|-----------|
| ![Hide icon only](https://i.imgur.com/KPXmVYy.png) | ![Hide title only](https://i.imgur.com/e1M8jnX.png) | ![Hide both](https://i.imgur.com/GMB5Wjq.png) |

## Caption Button Style
Three button styles are available in *Caption button style*: the default Windows glyphs (default), macOS circles in colors you choose (with the x, - and + symbols on hover), or your own images (png, jpg or bmp).

The preview images in this README use the macOS style. With the default Windows style, the buttons only move to the left and keep the standard Windows look.

| macOS style (circles) | Custom images | Windows default |
|-----------------------|---------------|-----------------|
| ![macOS style (circles)](https://i.imgur.com/dOdqDHp.png) | ![Custom images](https://i.imgur.com/OmjIMoI.png) | ![Windows default](https://i.imgur.com/KAHAbN0.png) |

### Custom button images
1. Set *Caption button style* to **Custom images**. (With the macOS style, the image settings are ignored and the circles stay as they are.)
2. Set **Icon folder** (or the path of each button image) to any folder you can read, for example `C:\Users\You\Pictures\ButtonIcons`.
3. Name the files `minimize`, `restore`, `maximize`, `close` (+ `.png`, `.jpg`, `.jpeg` or `.bmp`).
4. Optional state variants: `close_hover` (mouse is over the button), `close_pressed` (button is being clicked) and `close_disabled` (button is greyed out). Same for the other buttons. If a variant is missing, the base image is used (darkened for pressed). The four base images are enough.
5. Images are scaled down to fit the button slot (about 12 px at 100% DPI) and keep their own shape. They are not cut into a circle, so square images stay square. Use transparent PNGs and simple designs.
6. A button without an image keeps its circle.

`dwm.exe` runs under its own account and usually cannot read your profile folders. To get around that, the mod copies your images to `C:\Users\Public\Windows Leftify Cache` (done by the part of the mod running inside `explorer.exe`, which can read your folders). The folder is only created when you use custom images, contains nothing but copies of those images, is removed when the custom style is not used or the mod is unloaded, and is safe to delete at any time. Because this folder is shared between all users of the PC, `dwm.exe` only decodes files from it that really are png, jpg or bmp images.

If a changed image doesn't show up right away, move the mouse over a title bar button.

If the buttons stay as circles and the debug log shows `CreateFile failed, error=5` for a cache file, your PC restricts the Public folder. Run this in an administrator Command Prompt:

`icacls "C:\Users\Public\Windows Leftify Cache" /grant *S-1-1-0:(OI)(CI)R /T`

### File Explorer
The tab bar of File Explorer covers the left part of the title bar and blocks the buttons. The mod (running inside `explorer.exe`) makes the buttons clickable again, but the tabs themselves must be shifted with a separate mod.
| File Explorer caption buttons on the right (default) | File Explorer caption buttons on the left |
|------------------------------------------------------|-------------------------------------------|
| ![File Explorer caption buttons on the right (default)](https://i.imgur.com/cZHP6po.png) | ![File Explorer caption buttons on the left](https://i.imgur.com/LarGnlp.png) |

Notes:
- **File Explorer caption buttons on the right (default):** CabinetWClass is in Excluded window classes by default, so File Explorer keeps the default Windows layout. To keep this, leave it in the list. 
- **File Explorer caption buttons on the left:** Remove `CabinetWClass` from *Excluded window classes*, then turn on *Move the File Explorer tab bar* so the buttons can be clicked. The tabs stay where they are and still visually overlap the buttons, so shift them with the [Windows 11 File Explorer Styler](https://windhawk.net/mods/windows-11-file-explorer-styler) mod by [m417z](https://github.com/m417z) (target `FileExplorerExtensions.FileExplorerTabControl`, style `Margin=187,0,0,0`; this value works well for me, so adjust it to taste).

## Excluded apps
Apps that draw their own tabs in the title bar can conflict with buttons on the left. Add their executable names to *Excluded apps* (or their window class names to *Excluded window classes*) and every window of those apps keeps the default Windows layout.

By default *Excluded apps* contains `notepad.exe`, `taskmgr.exe` and `WindowsTerminal.exe`. The icon and title of Task Manager overlap the buttons, and the tabs of Notepad and Windows Terminal overlap the buttons when they are on the left. To see how these apps look with the buttons on the left, remove them from the list. Fixing them properly is planned for later, when time allows.

`CabinetWClass` (File Explorer) is in *Excluded window classes* by default, see the File Explorer section above.

## Files created by the mod

- **Image cache:** `C:\Users\Public\Windows Leftify Cache` (only with custom images, see above).
- **Debug logs:** `C:\Users\Public\Windows Leftify Log` (`layout.txt`, `glyphs.txt`, `explorer-tabs.txt`). Logs are only written while *Write debug log* is on (off by default) and are deleted when you turn it off, so send them before switching it off. Logs can contain folder paths (including your Windows user name), so check them before sharing.

## Important notes

- When you use the macOS or custom style, drawing the glyphs rewrites a bitmap that DWM shares between all windows. The original glyphs come back **only after dwm.exe restarts**: sign out and in, run `taskkill /f /im dwm.exe` from an administrator prompt, or enable *Restart dwm.exe when the mod is unloaded* (the screen flickers briefly; apps stay open).
- The circles or custom images are global. Excluded apps keep them too; only their layout stays at the Windows default.
- The mod hooks internal, undocumented functions of `uDWM.dll`. A Windows update can change them and stop the mod until it is updated.
- If you report a bug, turn on *Write debug log* and attach the log files.

## Compatibility and known issues

- **Apps that draw their own title bar** (Office apps, Chrome, Edge and others) cannot be changed by this mod. The [Titlebar For Everyone](https://windhawk.net/mods/titlebar-for-everyone) mod by [Ingan121](https://github.com/Ingan121) can make them use the standard title bar.
- At 125% and 150% display scaling, the maximize circle can show the wrong color (known issue).
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
    $name: Move the File Explorer tab bar
    $description: "On = the tab bar window of File Explorer is moved to the right of the left-side buttons, so the buttons can be clicked. Only works while Move caption buttons to the left is on and CabinetWClass is not excluded."
  - buttonsExtraMargin: 5
    $name: Extra left margin for File Explorer buttons (px)
    $description: "Moves the caption buttons right, in File Explorer only. About 5 aligns the circles with the nav arrows; about 28 centers them over the four nav buttons."
  - extraPx: 0
    $name: Extra shift (px)
    $description: "Fine-tune. Positive moves the tab bar further right, negative moves it left. The base shift is the width of the three buttons (scaled by DPI) plus the left margin."
  $name: File Explorer tabs
  $description: Fixes the tab bar of File Explorer blocking the buttons on the left.
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
    $description: "Off (recommended): painted glyphs stay until dwm.exe restarts. On: dwm.exe is terminated on every unload (recompile, disable, Windhawk restart); the screen flickers, and repeated restarts in a short time can be dangerous."
  $name: Button style
  $description: Choose how the caption buttons look - macOS circles, your own images, or the Windows default.
- debugLog: false
  $name: Write debug log
  $description: Writes logs to C:/Users/Public/Windows Leftify Log (layout.txt, glyphs.txt, explorer-tabs.txt) and to the Windhawk log output
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

// ======================================================================
//  PART 1: Leftify (caption buttons on the left, icon and title layout)
// ======================================================================

static struct {
    int leftMargin, order;
    bool moveButtons, moveTitle;
    int titleAlign, iconPosition, iconSide, titleGap, iconTitleGap, rightEdgeGap;
    int explorerMargin;
    bool centerOnText;
    int centerNudge;
    bool hideIcon, hideTitle, debugLog;
    bool showDialogClose;
    int dialogAlign;
} g_settings;

// Alignment correction (px) for windows that only have a close button
// (About Windows, message boxes, Run, ...). Not user-configurable.
static const int kCloseOnlyOffset = -1;

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
    g_settings.leftMargin = Wh_GetIntSetting(L"buttons.leftMargin");
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

    g_settings.debugLog = Wh_GetIntSetting(L"debugLog") != 0;

    LoadExclusions();
}

// ---------------- log files ----------------
// All logs live in one folder. dwm.exe cannot write to user profile folders,
// so the folder is created under C:\Users\Public.
static const wchar_t kLogDir[] = L"C:\\Users\\Public\\Windows Leftify Log";

static FILE* OpenLogFile(const wchar_t* name, const wchar_t* mode) {
    CreateDirectoryW(kLogDir, nullptr);
    wchar_t path[MAX_PATH];
    _snwprintf(path, MAX_PATH - 1, L"%s\\%s", kLogDir, name);
    path[MAX_PATH - 1] = 0;
    return _wfopen(path, mode);
}

// Debug logging off: remove the log files and the (empty) folder, so nothing is left behind.
static void RemoveLogFiles() {
    static const wchar_t* const kNames[] = { L"layout.txt", L"glyphs.txt", L"explorer-tabs.txt" };
    for (int i = 0; i < 3; i++) {
        wchar_t path[MAX_PATH];
        _snwprintf(path, MAX_PATH - 1, L"%s\\%s", kLogDir, kNames[i]);
        path[MAX_PATH - 1] = 0;
        DeleteFileW(path);
    }
    RemoveDirectoryW(kLogDir);   // only succeeds when the folder is empty
}

// ---------------- logging ----------------
static int g_logCount = 0;
static int g_hitLogCount = 0;
static wchar_t g_lastLog[800] = L"";
static wchar_t g_lastHitLog[300] = L"";

static void WriteLogLine(const wchar_t* line) {
    Wh_Log(L"%s", line);
    FILE* f = OpenLogFile(L"layout.txt", L"a, ccs=UTF-8");
    if (!f) return;
    fwprintf(f, L"%s\n", line);
    fclose(f);
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

static int MeasureTitleWidth(HWND hwnd, int dpi) {
    if (!hwnd || !g_pSPIForDpi) return -1;
    wchar_t text[300] = {};
    int n = GetWindowTextW(hwnd, text, 300);
    if (n <= 0) return 0;
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
    int n = GetWindowTextW(hwnd, t, 300);
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

// ---------------- per-window button rectangles (window-local X) ----------------
struct WinLayout { void* win; int left[4]; int right[4]; int band; bool valid; };
static WinLayout g_winLayouts[64];
static int g_winLayoutNext = 0;
static SRWLOCK g_layoutLock = SRWLOCK_INIT;

static void StoreWinLayout(void* win, const int* l, const int* r, int band) {
    AcquireSRWLockExclusive(&g_layoutLock);
    WinLayout* e = nullptr;
    for (int i = 0; i < 64; i++) {
        if (g_winLayouts[i].win == win) { e = &g_winLayouts[i]; break; }
    }
    if (!e) {
        e = &g_winLayouts[g_winLayoutNext];
        g_winLayoutNext = (g_winLayoutNext + 1) % 64;
        e->win = win;
    }
    for (int t = 1; t <= 3; t++) { e->left[t] = l[t]; e->right[t] = r[t]; }
    e->band = band;
    e->valid = true;
    ReleaseSRWLockExclusive(&g_layoutLock);
}

static void ClearWinLayout(void* win) {
    AcquireSRWLockExclusive(&g_layoutLock);
    for (int i = 0; i < 64; i++) {
        if (g_winLayouts[i].win == win) { g_winLayouts[i].valid = false; break; }
    }
    ReleaseSRWLockExclusive(&g_layoutLock);
}

static bool GetWinLayout(void* win, int* l, int* r, int* band) {
    bool ok = false;
    AcquireSRWLockShared(&g_layoutLock);
    for (int i = 0; i < 64; i++) {
        if (g_winLayouts[i].win == win && g_winLayouts[i].valid) {
            for (int t = 1; t <= 3; t++) { l[t] = g_winLayouts[i].left[t]; r[t] = g_winLayouts[i].right[t]; }
            *band = g_winLayouts[i].band;
            ok = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_layoutLock);
    return ok;
}

// ---------------- per-window HWND cache + title-text state ----------------
struct HwndCache { void* win; HWND hwnd; HWND hashHwnd; unsigned hash; bool hasHash; };
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

static void StoreHwnd(void* win, HWND hwnd) {
    AcquireSRWLockExclusive(&g_hwndLock);
    FindOrCreateCacheLocked(win)->hwnd = hwnd;
    ReleaseSRWLockExclusive(&g_hwndLock);
}

static HWND LookupHwnd(void* win) {
    HWND h = nullptr;
    AcquireSRWLockShared(&g_hwndLock);
    for (int i = 0; i < 64; i++) {
        if (g_hwndCache[i].win == win) { h = g_hwndCache[i].hwnd; break; }
    }
    ReleaseSRWLockShared(&g_hwndLock);
    return h;
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

// ---------------- window lookup (tolerant) ----------------
struct FindCtx {
    RECT tb;
    int expectedInset;
    HWND best;
    int bestErr;
    bool exact;
    RECT win;
};

static BOOL CALLBACK FindWindowProc(HWND hwnd, LPARAM lp) {
    FindCtx* c = (FindCtx*)lp;
    if (!IsWindowVisible(hwnd)) return TRUE;
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return TRUE;
    int il = c->tb.left - r.left;
    int ir = r.right - c->tb.right;
    int dy = c->tb.top - r.top;
    if (il < 0 || il > 24 || ir < 0 || ir > 24 || dy < 0 || dy > 40) return TRUE;
    int err = abs(il - c->expectedInset) + abs(ir - c->expectedInset);
    if (err > 12) return TRUE;
    if (!c->best || err < c->bestErr) {
        c->best = hwnd;
        c->bestErr = err;
        c->exact = (il == ir);
        c->win = r;
    }
    return TRUE;
}

// Fallback: visible titled window whose WIDTH is closest, ignoring position
struct WidthCtx {
    int winW;
    int x, y;
    HWND fg;
    HWND best;
    long bestCost;
    int tol;        // max allowed width difference
    bool strict;    // also require the window position to match the title bar
    int tbTop;
};

static BOOL CALLBACK FindByWidthProc(HWND hwnd, LPARAM lp) {
    WidthCtx* c = (WidthCtx*)lp;
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return TRUE;
    if (GetWindowTextLengthW(hwnd) <= 0) return TRUE;
    // Only windows that really have a caption (WS_CAPTION) can own this title
    // bar. Without this check, a caption-less window of similar width (e.g. a
    // fullscreen/popup window during a tiling transition) was matched and
    // cached, which gave a wrong width and shifted the buttons.
    if ((GetWindowLongW(hwnd, GWL_STYLE) & WS_CAPTION) != WS_CAPTION) return TRUE;
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return TRUE;
    int dw = abs((r.right - r.left) - c->winW);
    if (dw > c->tol) return TRUE;
    if (c->strict && (abs(r.left - c->x) > 12 || r.top > c->tbTop + 5 || r.top < c->tbTop - 45))
        return TRUE;
    long cost = dw * 4L + abs(r.left - c->x) + abs(r.top - c->y);
    if (hwnd == c->fg) cost -= 40;
    if (!c->best || cost < c->bestCost) {
        c->best = hwnd;
        c->bestCost = cost;
    }
    return TRUE;
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

static void GetTitleBarInfo_Hook(void* pThis, TITLEBARINFOEX* info, bool* flag) {
    GetTitleBarInfo_Orig(pThis, info, flag);
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

    FindCtx ctx = {};
    ctx.tb = tb;
    ctx.expectedInset = (b == 8) ? 8 : baseOut + 1;
    EnumWindows(FindWindowProc, (LPARAM)&ctx);

    int winW = (ctx.best && ctx.exact) ? (ctx.win.right - ctx.win.left)
                                       : (tbW + 2 * ctx.expectedInset);

    // Resolve the HWND (only needed to measure the title text).
    bool needHwnd = g_settings.moveTitle && !g_settings.hideTitle &&
                    TitleWidthMatters();
    HWND hwndUsed = ctx.best;
    const wchar_t* via = L"match";
    if (ctx.best) {
        StoreHwnd(pThis, ctx.best);
    } else {
        // No window matched the title bar insets. The width estimate
        // (tbW + 2*inset) can be off by ~16px for some windows (e.g. About
        // Windows, or windows being tiled/moved), which shifts every button.
        // Try to find the real window and use its true width.
        int estW = winW;
        RECT cr;
        bool resolved = false;

        // 1) The HWND we matched earlier for this very window object. Its real
        //    width is authoritative (also correct after the window was resized).
        HWND cached = LookupHwnd(pThis);
        if (cached && IsWindow(cached) && IsWindowVisible(cached) &&
            GetWindowRect(cached, &cr) && abs((cr.right - cr.left) - winW) <= 40) {
            hwndUsed = cached;
            winW = cr.right - cr.left;
            via = L"cache";
            resolved = true;
        }

        // 2) Search by width AND position.
        if (!resolved) {
            WidthCtx sc = {};
            sc.winW = winW;
            sc.x = tb.left - ctx.expectedInset;
            sc.y = tb.top - 3;
            sc.tbTop = tb.top;
            sc.fg = GetForegroundWindow();
            sc.tol = 40;
            sc.strict = true;
            EnumWindows(FindByWidthProc, (LPARAM)&sc);
            if (sc.best && GetWindowRect(sc.best, &cr)) {
                hwndUsed = sc.best;
                winW = cr.right - cr.left;
                via = L"strict";
                StoreHwnd(pThis, sc.best);
                resolved = true;
            }
        }

        // 3) Search by width only. The real width of the found window is used,
        //    and the HWND is cached for the next layout.
        if (!resolved) {
            WidthCtx wc = {};
            wc.winW = winW;
            wc.x = tb.left - ctx.expectedInset;
            wc.y = tb.top - 3;
            wc.fg = GetForegroundWindow();
            wc.tol = 24;
            EnumWindows(FindByWidthProc, (LPARAM)&wc);
            if (wc.best && GetWindowRect(wc.best, &cr)) {
                hwndUsed = wc.best;
                winW = cr.right - cr.left;
                via = L"width";
                StoreHwnd(pThis, wc.best);
                resolved = true;
            } else {
                via = needHwnd ? L"none" : L"estimate";
            }
        }
        DebugLog(L"[Hwnd] this=%p via=%s hwnd=%p estW=%d winW=%d tb=(%ld,%ld,%ld,%ld)",
                 pThis, via, hwndUsed, estW, winW, tb.left, tb.top, tb.right, tb.bottom);
    }

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

    int x = 0;                 // right edge of the button group (from the left)
    int rightMargin = baseOut; // space reserved on the right for the title area
    if (g_settings.moveButtons) {
        x = baseOut + EffLeftMargin(hwndUsed) + (closeOnly ? kCloseOnlyOffset : 0);
        for (int i = 0; i < 3; i++) {
            int t = kOrders[o][i];
            x += w[t];
            t_seq.outBefore[t] = winW - x;
        }
    } else {
        // Default Windows placement: close, maximize, minimize from the right.
        static const int kStock[3] = { 3, 2, 1 };
        int out = baseOut;
        for (int i = 0; i < 3; i++) {
            int t = kStock[i];
            t_seq.outBefore[t] = out;
            out += w[t];
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
    }
    StoreWinLayout(pThis, bl, br, b + a + 2);

    if (g_settings.debugLog && hwndUsed) {
        RECT rr = {};
        if (GetWindowRect(hwndUsed, &rr)) {
            DebugLog(L"[Rect] this=%p hwnd=%p rect=(%ld,%ld,%ld,%ld) rectW=%ld tb=(%ld,%ld,%ld,%ld) style=%08lX zoomed=%d winW=%d expectedInset=%d",
                     pThis, hwndUsed, rr.left, rr.top, rr.right, rr.bottom, rr.right - rr.left,
                     tb.left, tb.top, tb.right, tb.bottom,
                     (unsigned long)GetWindowLongW(hwndUsed, GWL_STYLE), (int)IsZoomed(hwndUsed),
                     winW, ctx.expectedInset);
        }
    }

    DebugLog(L"[Layout] this=%p closeOnly=%d a=%d b=%d base=%d W=%d match=%d exact=%d hwnd=%p dpi=%d buttonEnd=%d close=[%d,%d] max=[%d,%d] min=[%d,%d] band=%d",
             pThis, (int)closeOnly, a, b, baseOut, winW, (int)(ctx.best != nullptr), (int)(ctx.best && ctx.exact), hwndUsed, dpi,
             x, bl[3], br[3], bl[2], br[2], bl[1], br[1], b + a + 2);
    return true;
}

static long UpdateNCAreaButton_Hook(void* pThis, int type, int a, int b, int* pOut) {
    if (type == 3) {
        t_seq = Seq{};
        ClearWinLayout(pThis);
        if (a > 0 && pOut && (g_settings.moveButtons || g_settings.moveTitle)) {
            ComputeLayout(pThis, a, b, *pOut);
        }
    }

    if (t_seq.valid && pOut) {
        if (type >= 1 && type <= 3) *pOut = t_seq.outBefore[type];
        else if (type == 0) *pOut = t_seq.origFinalOut;
    }

    long r = UpdateNCAreaButton_Orig(pThis, type, a, b, pOut);

    if (type == 0) {
        DebugLog(L"[Seq] this=%p valid=%d moveTitle=%d hasPosHook=%d posDepth=%d",
                 pThis, (int)t_seq.valid, (int)g_settings.moveTitle, (int)g_hasPosHook, t_posDepth);
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
    return g_settings.moveTitle && g_hasPosHook && TitleWidthMatters();
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
    if (hasState && hw && IsWindow(hw) && HashWindowText(hw) == oldHash) return false;

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

    if (g_settings.debugLog && pt && pFlags && pt->y <= 40 && pt->x < 200) {
        unsigned c = r ? *pFlags : 0xFFFFFFF1u;
        if (pThis != g_lastRawWin || c != g_lastRawCode) {
            g_lastRawWin = pThis;
            g_lastRawCode = c;
            HitLog(L"[HitRaw] win=%p pt=(%ld,%ld) r=%d code=%u", pThis, pt->x, pt->y, (int)r, r ? *pFlags : 0);
        }
    }

    if (!r || !pt || !pFlags) return r;

    unsigned int code = *pFlags;
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
    g_tlEnabled = (style && wcscmp(style, L"windows") != 0) ? 1 : 0;
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
    LoadIcons();   // decodes the images to memory (also runs on the DWM thread when explorer.exe syncs new images)
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
        FILE* f = OpenLogFile(L"glyphs.txt", L"a, ccs=UTF-8");
        if (f) { fwprintf(f, L"%s\n", buf); fclose(f); }
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

// ---------------- registry of arrays and glyph objects ----------------
struct ArrInfo { const void* bm[4]; char kind; int set; int role; };
static ArrInfo g_arr[192];
static int g_arrCount = 0;
static int g_curSet = -1, g_curSetCount = 0;
static float g_curSetF = -1.0f;
static SRWLOCK g_arrLock = SRWLOCK_INIT;

struct Glyph { const void* obj; const void* a0; int role, state; unsigned rc[4]; unsigned sz[2]; void* vt; };
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

// Each role has its own cell in the free area of the atlas (y >= 808).
// kind 0 = minimize 12x12, 1 = restore 12x12, 2 = maximize 14x12, 3 = close 12x12
static int CellKind(int role, unsigned w) { return role == 2 ? 3 : (role == 1 ? (w == 14 ? 2 : 1) : 0); }

static void MoveToCell(Glyph& g) {
    if (!GlyphStillValid(g)) return;
    unsigned x = 1 + 16 * (unsigned)CellKind(g.role, g.sz[0]);
    unsigned y = 808 + 14 * (unsigned)g.state;
    g.rc[0] = x; g.rc[1] = y; g.rc[2] = x + g.sz[0]; g.rc[3] = y + g.sz[1];
    unsigned* d = (unsigned*)g.obj;
    d[20] = g.rc[0]; d[21] = g.rc[1]; d[22] = g.rc[2]; d[23] = g.rc[3];
}

// First guess: A left margin = 4 -> close; B width = 14 -> maximize; the rest minimize.
// Minimize vs restore is corrected later from the button width (46 = middle, 47 = minimize).
static int DetectRole(const void* a0, const void* b0, int fallback) {
    if (Readable(a0, 0x70) && Readable(b0, 0x70)) {
        const unsigned* da = (const unsigned*)a0;
        const unsigned* db = (const unsigned*)b0;
        if (da[8] == 4) return 2;
        if (db[6] == 14) return 1;
        return 0;
    }
    return fallback;
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
    for (int i = 0; i < g_glN; i++) {
        for (int s = 0; s < 4; s++) {
            if (g_gl[i].obj == fam[s] && g_gl[i].role != role && g_gl[i].role != 2) {
                g_gl[i].role = role;
                MoveToCell(g_gl[i]);
                changed = true;
            }
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

static bool RegisterPair(const void* a, const void* b, float f) {
    const void* va[4] = {};
    const void* vb[4] = {};
    if (!ReadArr(a, va) || !ReadArr(b, vb)) return false;
    if (!vb[0] || !Readable(vb[0], 0x70)) return false;   // empty/odd arrays: ignore

    bool isNew = false;
    int set = -1, role = -1;
    AcquireSRWLockExclusive(&g_arrLock);
    bool found = false;
    for (int i = 0; i < g_arrCount; i++) {
        if (g_arr[i].kind == 'B' && g_arr[i].bm[0] == vb[0]) { found = true; break; }
    }
    if (!found && g_arrCount + 2 <= 192) {
        if (g_curSet < 0 || g_curSetCount >= 3 || f != g_curSetF) {
            g_curSet++;
            g_curSetCount = 0;
            g_curSetF = f;
        }
        set = g_curSet;
        int idx = g_curSetCount++;
        role = DetectRole(va[0], vb[0], idx == 1 ? 1 : 0);
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
                MoveToCell(g);
            }
        }
        ReleaseSRWLockExclusive(&g_glLock);

        const unsigned* d0 = (const unsigned*)vb[0];
        unsigned lr[6] = { d0[20], d0[21], d0[22], d0[23], d0[6], d0[7] };
        LogCat(0, 60, L"[Pair] set=%d role=%d f=%f A0=%p B0=%p rc=(%u,%u)-(%u,%u) size=%ux%u",
               set, role, (double)f, va[0], vb[0], lr[0], lr[1], lr[2], lr[3], lr[4], lr[5]);
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

// ---------------- CBitmapSource::Update symbol (cached per uDWM.dll version) ----------------
// CBitmapSource::Update is only called (never hooked), so HookSymbols does not
// apply here. The symbol is searched manually, and its RVA is cached per
// uDWM.dll build (Wh_SetIntValue), so the slow symbol enumeration runs only once.
typedef long (*UpdateBmp_t)(void* pThis, IWICBitmap* bmp);
static UpdateBmp_t g_updateBmp = nullptr;

static void FindUpdate(HMODULE udwm) {
    const BYTE* base = (const BYTE*)udwm;
    wchar_t key[64] = L"";
    DWORD imageSize = 0;
    const IMAGE_DOS_HEADER* dos = (const IMAGE_DOS_HEADER*)base;
    if (dos->e_magic == IMAGE_DOS_SIGNATURE) {
        const IMAGE_NT_HEADERS* nt = (const IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
        if (nt->Signature == IMAGE_NT_SIGNATURE) {
            imageSize = nt->OptionalHeader.SizeOfImage;
            _snwprintf(key, 63, L"updRva_%08lX_%08lX",
                       (unsigned long)nt->FileHeader.TimeDateStamp, (unsigned long)imageSize);
            key[63] = 0;
        }
    }
    if (key[0]) {
        int rva = Wh_GetIntValue(key, 0);
        if (rva > 0 && (DWORD)rva < imageSize) {
            g_updateBmp = (UpdateBmp_t)(base + rva);
            LogCat(1, 10, L"[Symbol] Update from cache: %p (rva=0x%x)", (void*)g_updateBmp, rva);
            return;
        }
    }
    WH_FIND_SYMBOL sym;
    HANDLE h = Wh_FindFirstSymbol(udwm, nullptr, &sym);
    if (!h) { LogCat(1, 10, L"[Symbol] Wh_FindFirstSymbol failed"); return; }
    do {
        if (sym.symbol && wcsstr(sym.symbol, L"CBitmapSource::Update(struct IWICBitmap *)")) {
            g_updateBmp = (UpdateBmp_t)sym.address;
            LogCat(1, 10, L"[Symbol] Update found: %p %s", sym.address, sym.symbol);
            break;
        }
    } while (Wh_FindNextSymbol(h, &sym));
    Wh_FindCloseSymbol(h);
    if (!g_updateBmp) LogCat(1, 10, L"[Symbol] Update NOT found");
    else if (key[0]) Wh_SetIntValue(key, (int)((const BYTE*)g_updateBmp - base));
}

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
    return DistSeg(ux, uy, -L, 0.0f, L, 0.0f) <= th || DistSeg(ux, uy, 0.0f, -L, 0.0f, L) <= th;
}

static void RenderGlyph(BYTE* px, int w, int h, int role, int state) {
    if (role < 0 || role > 2) role = 0;
    int bg[3], sym[3];
    for (int i = 0; i < 3; i++) {
        bg[i] = state == 2 ? g_base[role][i] * 78 / 100 : g_base[role][i];
        sym[i] = g_base[role][i] * 28 / 100;
    }
    bool drawSym = (state == 1 || state == 2);
    float cx = w / 2.0f, cy = h / 2.0f;
    float rad = (w < h ? w : h) / 2.0f - 0.25f;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int nBg = 0, nSym = 0;
            for (int sy = 0; sy < 4; sy++) {
                for (int sx = 0; sx < 4; sx++) {
                    float ux = x + (sx + 0.5f) / 4.0f - cx;
                    float uy = y + (sy + 0.5f) / 4.0f - cy;
                    if (ux * ux + uy * uy <= rad * rad) {
                        if (drawSym && InSymbol(role, ux, uy)) nSym++;
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

// ---------------- custom images (png/jpg/bmp), decoded once in LoadIcons ----------------
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

    // Only hand png / jpeg / bmp data to the decoder: this keeps the other WIC codecs out of dwm.exe
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
        if (w > 256 || h > 256) {
            double k = 256.0 / (w > h ? w : h);
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

// ---------------- icon cache (shared between explorer.exe and dwm.exe) ----------------
// explorer.exe runs as the user and can read any folder the user can. It copies the
// images into this folder; dwm.exe reads them from here when it cannot read the original.
static const wchar_t kCacheDir[] = L"C:\\Users\\Public\\Windows Leftify Cache";

static void CachePath(int kind, int st, wchar_t* out, size_t cap) {
    _snwprintf(out, cap - 1, L"%s\\k%d_s%d.img", kCacheDir, kind, st);
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
    IconSettings s;
    ReadIconSettings(&s);
    // Read the version BEFORE loading: if explorer bumps it meanwhile, we reload once more.
    g_iconVerSeen = Wh_GetIntValue(L"iconCacheVersion", 0);

    LogCat(3, 40, L"[Icon] folder='%s' min='%s' restore='%s' max='%s' close='%s' any=%d",
        s.folder, s.files[0], s.files[1], s.files[2], s.files[3], (int)s.any);

    IconImg tmp[4][4] = {};
    if (s.any) {
        HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        IWICImagingFactory* fac = nullptr;
        HRESULT hr = CoCreateInstance(kCLSID_WICFactory, nullptr, CLSCTX_INPROC_SERVER, kIID_WICFactory, (void**)&fac);
        if (SUCCEEDED(hr) && fac) {
            for (int kind = 0; kind < 4; kind++) {
                for (int st = 0; st < 4; st++) {
                    wchar_t cand[MAX_PATH + 32];
                    bool ok = false;
                    // 1) the original path (works when dwm.exe can read it)
                    if (FindIconFile(s, kind, st, cand)) {
                        ok = DecodeImage(fac, cand, &tmp[kind][st]);
                        LogCat(3, 40, L"[Icon] kind=%d state=%d %s -> %s (%dx%d)", kind, st, cand,
                               ok ? L"OK" : L"decode FAILED", tmp[kind][st].w, tmp[kind][st].h);
                    }
                    // 2) the copy made by explorer.exe
                    if (!ok) {
                        wchar_t cp[MAX_PATH];
                        CachePath(kind, st, cp, MAX_PATH);
                        if (FileExistsW(cp)) {
                            ok = DecodeImage(fac, cp, &tmp[kind][st]);
                            LogCat(3, 40, L"[Icon] kind=%d state=%d from cache -> %s (%dx%d)", kind, st,
                                   ok ? L"OK" : L"decode FAILED", tmp[kind][st].w, tmp[kind][st].h);
                        } else if (st == 0) {
                            LogCat(3, 40, L"[Icon] kind=%d: no readable image and no cache copy (yet)", kind);
                        }
                    }
                }
            }
            fac->Release();
        } else {
            LogCat(3, 40, L"[Icon] CoCreateInstance WIC failed hr=0x%lx", (unsigned long)hr);
        }
        if (SUCCEEDED(hrInit)) CoUninitialize();
    }

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

static bool RenderCustomCell(BYTE* cell, int w, int h, int role, int state, unsigned origW) {
    int kind = CellKind(role, origW);
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

    for (int i = 0; i < n; i++) {
        int x0 = (int)snap[i].rc[0], y0 = (int)snap[i].rc[1];
        int w = (int)snap[i].rc[2] - x0, h = (int)snap[i].rc[3] - y0;
        if (w < 4 || h < 4 || w > 64 || h > 64) continue;
        if (x0 < 0 || y0 < 0 || x0 + w > W || y0 + h > H) continue;
        BYTE cell[64 * 64 * 4];
        if (!RenderCustomCell(cell, w, h, snap[i].role, snap[i].state, snap[i].sz[0]))
            RenderGlyph(cell, w, h, snap[i].role, snap[i].state);
        for (int y = 0; y < h; y++) {
            memcpy(px + ((size_t)(y0 + y) * W + x0) * 4, cell + y * w * 4, (size_t)w * 4);
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
            // bmp is intentionally not released: it is unknown whether Update keeps the pointer without AddRef.
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
    // explorer.exe bumps this value after it copies new images into the cache folder
    ULONGLONG now = GetTickCount64();
    if (now - g_iconVerCheck >= 500) {
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
    LogCat(2, 40, L"[Restore] restarting dwm.exe");
    TerminateProcess(GetCurrentProcess(), 1);
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
static void ImgSetSize_Hook(void* img, const SIZE* sz) {
    ImgSetSize_Orig(img, sz);
    if (g_tlEnabled && sz) {
        const void* bmp = LookupImg(img);
        if (bmp) {
            if (sz->cx == 46) CorrectRole(bmp, 1);       // middle button (maximize/restore) is 46 wide
            else if (sz->cx == 47) CorrectRole(bmp, 0);  // minimize and close are 47 (close is left alone)
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

// Width of the three caption buttons at 96 dpi (47 + 46 + 47)
static const int kButtonGroupW96 = 140;

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
    FILE* f = OpenLogFile(L"explorer-tabs.txt", L"a, ccs=UTF-8");
    if (f) { fwprintf(f, L"%s\n", buf); fclose(f); }
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
    int s = EffLeftMargin(top) + MulDiv(kButtonGroupW96, ExDpiOf(top), 96) + g_exExtra;
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
    TabSetState(hwnd, s, looksTab);
    if (s == 0) return;

    // Use the original X when WinUI resized the window without moving it
    int currentX = (p->flags & SWP_NOMOVE) ? cur.left : p->x;
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
    int bx2 = bx1 + MulDiv(46, dpi, 96) * 3;
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
        if (!hDwmApi) hDwmApi = LoadLibraryW(L"dwmapi.dll");
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
            int b4 = b1 + MulDiv(46, dpi, 96) * 3;
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
    SetWindowPos(h, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top,
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
static BOOL CALLBACK ExChildProc(HWND h, LPARAM);
static int TabSnapshot(HWND* hw, int* applied, bool* isTab);

static void StartRescan(HWND cab) {
    SetPropW(cab, L"LeftifyTicks", (HANDLE)(INT_PTR)0);
    SetTimer(cab, kRescanTimerId, 150, nullptr);
}

static LRESULT CALLBACK CabinetSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (msg == WM_TIMER && wParam == kRescanTimerId) {
        int ticks = (int)(INT_PTR)GetPropW(hwnd, L"LeftifyTicks") + 1;
        SetPropW(hwnd, L"LeftifyTicks", (HANDLE)(INT_PTR)ticks);
        EnumChildWindows(hwnd, ExChildProc, 0);
        if (ticks >= 12) {   // 12 x 150 ms = about 1.8 seconds
            KillTimer(hwnd, kRescanTimerId);
            RemovePropW(hwnd, L"LeftifyTicks");
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
                int b1 = baseOut + lm;
                int b4 = b1 + bw * 3;
                if (x >= b1 && x < b4) {

                    // Let DWM answer the hit-test so it keeps drawing the hover animation
                    static DwmDefWindowProc_t pDwmDefWindowProc = nullptr;
                    if (!pDwmDefWindowProc) {
                        HMODULE hDwmApi = GetModuleHandleW(L"dwmapi.dll");
                        if (!hDwmApi) hDwmApi = LoadLibraryW(L"dwmapi.dll");
                        if (hDwmApi) pDwmDefWindowProc = (DwmDefWindowProc_t)GetProcAddress(hDwmApi, "DwmDefWindowProc");
                    }
                    LRESULT lRes = 0;
                    if (pDwmDefWindowProc && pDwmDefWindowProc(hwnd, msg, wParam, lParam, &lRes)) {
                        return lRes;
                    }

                    int idx = (x - b1) / bw;
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
        RemovePropW(hwnd, L"LeftifyTicks");
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

static void ExplorerUninit() {
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

// ---------------- icon cache writer (explorer.exe) ----------------
static bool ReadWholeFile(const wchar_t* path, BYTE** data, DWORD* size) {
    *data = nullptr; *size = 0;
    HANDLE hf = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        ExLog(L"[IconCache] cannot read, error=%lu (%s)", (unsigned long)GetLastError(), path);
        return false;
    }
    LARGE_INTEGER fsz = {};
    if (!GetFileSizeEx(hf, &fsz) || fsz.QuadPart <= 0 || fsz.QuadPart > 16 * 1024 * 1024) {
        CloseHandle(hf);
        return false;
    }
    DWORD n = (DWORD)fsz.QuadPart, got = 0;
    BYTE* buf = (BYTE*)malloc(n);
    bool ok = buf && ReadFile(hf, buf, n, &got, nullptr) && got == n;
    CloseHandle(hf);
    if (!ok) { free(buf); return false; }
    *data = buf; *size = n;
    return true;
}

// Delete first, then create: a NEW file inherits the permissions of the cache folder,
// so dwm.exe can read it (this is what failed with files moved from other folders).
static bool WriteNewFile(const wchar_t* path, const BYTE* data, DWORD size) {
    DeleteFileW(path);
    HANDLE hf = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE,
                            nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        ExLog(L"[IconCache] cannot write, error=%lu (%s)", (unsigned long)GetLastError(), path);
        return false;
    }
    DWORD wr = 0;
    bool ok = WriteFile(hf, data, size, &wr, nullptr) && wr == size;
    CloseHandle(hf);
    return ok;
}

static void SyncIconCache() {
    IconSettings s;
    ReadIconSettings(&s);   // s.any is false unless the Custom images style is selected
    if (s.any) CreateDirectoryW(kCacheDir, nullptr);

    int copied = 0;
    for (int kind = 0; kind < 4; kind++) {
        for (int st = 0; st < 4; st++) {
            wchar_t cp[MAX_PATH];
            CachePath(kind, st, cp, MAX_PATH);
            wchar_t cand[MAX_PATH + 32];
            bool have = false;
            if (s.any && FindIconFile(s, kind, st, cand)) {
                BYTE* data = nullptr;
                DWORD size = 0;
                if (ReadWholeFile(cand, &data, &size)) {
                    have = WriteNewFile(cp, data, size);
                    if (have) copied++;
                    free(data);
                }
            }
            if (!have) DeleteFileW(cp);   // setting removed or file gone: no stale image
        }
    }
    if (copied == 0) RemoveDirectoryW(kCacheDir);   // nothing cached: remove the empty folder
    // Tell dwm.exe to reload
    Wh_SetIntValue(L"iconCacheVersion", Wh_GetIntValue(L"iconCacheVersion", 0) + 1);
    ExLog(L"[IconCache] synced, %d file(s) copied", copied);
}

// Remove the cache copies and the folder (called when the mod is unloaded).
// The cache is rebuilt by SyncIconCache the next time the mod loads.
static void CleanIconCache() {
    for (int kind = 0; kind < 4; kind++) {
        for (int st = 0; st < 4; st++) {
            wchar_t cp[MAX_PATH];
            CachePath(kind, st, cp, MAX_PATH);
            DeleteFileW(cp);
        }
    }
    RemoveDirectoryW(kCacheDir);
}

static void LoadUser32Procs() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        g_pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
        g_pGetDpiForSystem = (GetDpiForSystem_t)GetProcAddress(user32, "GetDpiForSystem");
        g_pSPIForDpi = (SPIForDpi_t)GetProcAddress(user32, "SystemParametersInfoForDpi");
    }
}

static BOOL ExplorerInit() {
    if (Wh_GetIntSetting(L"debugLog")) {
        FILE* f = OpenLogFile(L"explorer-tabs.txt", L"w, ccs=UTF-8");
        if (f) fclose(f);
    } else {
        RemoveLogFiles();
    }
    LoadSettings();     // moveButtons, leftMargin, exclusions
    SyncIconCache();    // copy custom images where dwm.exe can read them
    ExLoadSettings();
    LoadUser32Procs();
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Orig);
    return TRUE;
}

// ======================================================================
//  Lifecycle (all parts)
// ======================================================================
void Wh_ModSettingsChanged() {
    if (g_isExplorer) {
        LoadSettings();
        ExLoadSettings();
        SyncIconCache();
        if (!g_settings.debugLog) RemoveLogFiles();
        ExplorerReapply();
        EnumWindows(ExTopProc, 0);
        return;
    }
    LoadSettings();       // Leftify (including the excluded apps lists)
    TL_LoadSettings();    // painter
    if (!g_settings.debugLog) RemoveLogFiles();
    InterlockedExchange(&g_pending, 1);   // repaint on the next DWM-thread hook call
}

void Wh_ModAfterInit() {
    if (g_isExplorer) EnumWindows(ExTopProc, 0);   // File Explorer windows that already exist
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        ExplorerUninit();
        CleanIconCache();
        return;
    }
    if (g_everPainted && g_restartOnUnload) RestartDwm();
    FreeIcons();
}

BOOL Wh_ModInit() {
    // The mod runs in two processes: dwm.exe (buttons, glyphs) and explorer.exe (tab bar)
    wchar_t exePath[MAX_PATH] = L"";
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;
    g_isExplorer = _wcsicmp(exeName, L"explorer.exe") == 0;
    if (g_isExplorer) return ExplorerInit();

    // Truncate the logs first so lines written while loading settings are not erased.
    // With debug logging off, remove any leftover log files instead.
    if (Wh_GetIntSetting(L"debugLog")) {
        FILE* f = OpenLogFile(L"layout.txt", L"w, ccs=UTF-8");
        if (f) fclose(f);
        f = OpenLogFile(L"glyphs.txt", L"w, ccs=UTF-8");
        if (f) fclose(f);
    } else {
        RemoveLogFiles();
    }

    LoadSettings();
    TL_LoadSettings();

    LoadUser32Procs();

    HMODULE udwm = LoadLibraryW(L"uDWM.dll");
    if (!udwm) return FALSE;

    FindUpdate(udwm);

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        // ---- Leftify ----
        { { LR"(public: void __cdecl CTopLevelWindow::GetTitleBarInfo(struct tagTITLEBARINFOEX *,bool *))" },
          (void**)&GetTitleBarInfo_Orig, (void*)GetTitleBarInfo_Hook, false },
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
    };

    BOOL ok = WindhawkUtils::HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks));
    g_hasPosHook = (PosAndSizes_Orig != nullptr);
    return ok;
}
