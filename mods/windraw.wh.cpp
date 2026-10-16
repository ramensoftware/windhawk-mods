// ==WindhawkMod==
// @id              windraw
// @name            WinDraw - Screen Inking & Annotation
// @description     All-in-one hardware-accelerated screen drawing, shapes, radial quick menu, floating toolbar, and screenshot tool for Windows.
// @version         1.0.0
// @author          ZainYousef
// @github          https://github.com/ZainYoussef
// @homepage        https://github.com/ZainYoussef/WinDraw
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lole32 -luser32 -lgdi32 -ldwmapi -lwindowscodecs -lshell32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WinDraw - Screen Inking & Annotation

A complete, zero-bloat, hardware-accelerated screen annotation and drawing suite running as a dedicated, high-performance tool process powered by **Direct2D**, **DirectWrite**, and **Windows Imaging Component (WIC)**.

![WinDraw Inking & Annotation](https://raw.githubusercontent.com/ZainYoussef/WinDraw/main/assets/Screenshot1.png)

![WinDraw Radial Quick Menu & Shapes Flyout](https://raw.githubusercontent.com/ZainYoussef/WinDraw/main/assets/Screenshot2.png)

### Complete Feature Set:

1. **Instant Activation & Dismissal**:
   - Press **`Ctrl + Alt + G`** (customizable) anywhere in Windows to begin annotating immediately.
   - Press **`ESC`** or click the **`✕`** Exit button to dismiss the overlay.
   - Ink strokes survive dismissal so you can re-open WinDraw without losing your work. Press **`C`** (Clear All) whenever you want a completely fresh canvas.

2. **Hardware-Accelerated Inking & Brushes**:
   - 144Hz+ butter-smooth Direct2D drawing with quadratic Bézier curve interpolation.
   - **Pen Mode** (`F` key): Solid color inking with subpixel accuracy.
   - **Highlighter Mode** (`H` key): Translucent alpha-blended highlighting.
   - Mouse wheel or **`[`** / **`]`** keys dynamically resize brush thickness with a real-time 1:1 circular indicator dot showing active color and zoom level.

3. **Shapes & Geometry**:
   - **Freehand** (`F` key)
   - **Straight Line** (`L` key)
   - **Arrow** (`A` key) with automatically oriented sharp arrowheads
   - **Rectangle / Box** (`R` key)
   - **Ellipse / Circle** (`O` key)
   - **Triangle** (`T` key)
   - Hold **`Shift`** while drawing to snap lines to 45° increments or constrain rectangles and ellipses to perfect squares and circles.
   - Floating **Shapes Flyout Modal** for visual selection.

4. **Custom Color & Opacity Studio**:
   - Click the **`+`** slot in the toolbar or press **`5`** to open the full-fledged **Color Studio**.
   - Interactive 2D Saturation-Value picker and continuous 360° Hue spectrum slider.
   - Live **Opacity / Alpha** slider (5% to 100%).
   - One-click **Eyedropper** tool to sample any pixel color directly from your desktop.
   - Hex code display with **Copy to Clipboard** button.
   - Stores and displays your **Last 5 Recent Colors** palette across sessions.

5. **Circular Radial Quick Menu**:
   - **Quick Right-Click Tap**: Opens a sleek circular radial menu centered at your cursor with an orbital color ring, quick tools, and smooth sector hover animations.
   - **Layer 2 Satellite Fan**: Hover over the top Recent Colors hub to smoothly fan out your latest 5 custom colors in an orbital satellite arc.
   - **Hold & Move Right Mouse Button**: Instant stroke-level eraser with circular radius indicator.

6. **Collapsible Floating Toolbar & Status Pill**:
   - Windows 11 Fluent dark acrylic styling with specular highlights and subtle group dividers.
   - Drag the toolbar anywhere on your multi-monitor desktop.
   - **Minimize Button (`B` key)**: Collapses the full bar into an ultra-compact status pill showing active ink color and grip handle.
   - Click the pill in-place to expand it, or drag the pill to park it anywhere on screen.

7. **Vanishing Neon Laser Pointer (`D` key)**:
   - High-visibility neon glowing laser pointer bead with multi-tier glow aura.
   - Temporary fading laser trails that dissolve smoothly after a configurable duration (200ms to 5000ms).
   - Scroll wheel while in Laser mode instantly adjusts trail persistence.

8. **Interactive Grid System (`G` key)**:
   - Dot Grid and Graph Lines Grid overlays.
   - Flyout modal allows switching styles and toggling density between Fine, Medium, and Coarse.
   - Press **`Shift + G`** to cycle grid density (Fine → Medium → Coarse) directly from the keyboard.

9. **Region Snipping & Full Screenshots**:
   - **Region Snip** (`S` key): Click and drag a selection rectangle to crop and copy/save a specific screen region.
   - **Full Snapshot** (`Ctrl + S`): Captures the full annotated screen to the Windows clipboard (`CF_DIBV5` / `CF_DIB` / `CF_BITMAP`) and auto-saves to `%USERPROFILE%\Pictures\WinDraw\`.

10. **Pan & Zoom Canvas Navigation**:
    - **Pan Mode** (`P` key): Click and drag to reposition drawings across large canvases.
    - **Canvas Zoom**: While holding Pan or using the mouse wheel, smoothly zoom in and out (15% to 800%) centered on the cursor.
    - **Reset View**: Press **`0`** (while in Pan mode) or **`Ctrl + 0`** (any mode) to reset zoom to 100% and pan offset to (0, 0).

11. **Pointer / Click-Through Mode (`M` key)**:
    - Allows interacting with underlying Windows applications and games while keeping your drawings overlaid.

12. **Ink History & Visibility**:
    - **Undo** (`Ctrl + Z`) and **Redo** (`Ctrl + Y`).
    - **Clear All** (`C` key) with full undo support.
    - **Hide/Show Ink** (`V` key): Temporarily toggles drawing visibility without clearing strokes.

13. **Customizable Cursor Style & Size**:
    - **Crosshair Style**: Displays a crisp, dual-contrast precision crosshair with configurable arm size (8px to 32px) and an active color/eraser center indicator across Pen, Highlighter, and Eraser.
    - **Live Brush Style**: Displays the exact footprint and diameter of your active brush, highlighter, or eraser with real-time color fill and dual-contrast outline.

14. **Temporary Whiteboard & Blackboard Mode (`K` or `Alt + B`)**:
    - **Multi-Monitor Display Targeting**: Left-click the Whiteboard button on the toolbar to open a Fluent modal flyout where you can choose backdrop styles (`Transparent`, `Whiteboard`, `Blackboard`) and target displays (`Active Screen (Follows Cursor)`, `Primary Screen`, specific monitor `Screen 1`, `Screen 2`, or `All Screens`).
    - **Live Reference Screen**: On multi-monitor setups, isolate your whiteboard/blackboard to a single monitor while keeping your other monitors 100% transparent live desktop for reference documents, IDEs, or communication apps.
    - **Ephemeral Mode**: Automatically resets backdrop on `ESC` while keeping your ink strokes intact.
    - **Full Snapshot & Snip Support**: Captures solid background and all ink strokes when taking snapshots (`Ctrl + S`) or snips (`S` key).
    - **Smart Contextual Grids**: Automatically adapts grid line colors (dark charcoal on white paper, vibrant cyan on dark slate).

---

### Keyboard Shortcuts Reference:

| Shortcut | Action |
|---|---|
| **Ctrl + Alt + G** | Activate / Open WinDraw Overlay (Customizable) |
| **ESC** | Dismiss / Close WinDraw Overlay (Preserves Ink) |
| **F** | Freehand Pen Tool |
| **H** | Highlighter Tool |
| **D** | Vanishing Neon Laser Pointer |
| **E** | Eraser Tool (or hold Right Mouse Button) |
| **L** | Straight Line Shape |
| **A** | Arrow Shape |
| **R** | Rectangle Shape |
| **O** | Ellipse / Circle Shape |
| **T** | Triangle Shape |
| **P** | Pan Canvas Mode |
| **M** | Pointer (Click-Through) Mode |
| **K** / **Alt + B** | Cycle Canvas Backdrop (Transparent → Whiteboard → Blackboard) |
| **G** | Toggle Grid Flyout / Cycle Grid Style |
| **Shift + G** | Cycle Grid Density (Fine → Medium → Coarse) |
| **B** | Collapse / Expand Toolbar Pill |
| **Ctrl + Shift + B** | Reset Toolbar Position to Primary Screen Center |
| **V** | Toggle Ink Visibility (Show/Hide) |
| **C** | Clear All Drawings |
| **Ctrl + Z** | Undo last stroke |
| **Ctrl + Y** | Redo last undone stroke |
| **S** | Region Snipping Tool (Crop & Copy to Clipboard) |
| **Ctrl + S** | Take Full Screen Snapshot & Copy to Clipboard |
| **[** / **]** | Decrease / Increase Brush Size (or Laser Trail) |
| **0** / **Ctrl + 0** | Reset Canvas Zoom & Pan (`0` in Pan mode, `Ctrl + 0` anytime) |
| **1 - 4** | Select Preset Colors (Crimson Red, Tangelo Orange, Amber Gold, Sun Yellow) |
| **5** | Open Custom Color & Opacity Studio |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeyMod: ctrl_alt
  $name: Hotkey Modifier Combo
  $description: Select the combination of modifier keys used to activate WinDraw.
  $options:
    - alt: Alt
    - ctrl: Ctrl
    - ctrl_alt: Ctrl + Alt (Default)
    - shift: Shift
    - shift_alt: Shift + Alt
    - ctrl_shift: Ctrl + Shift
    - ctrl_shift_alt: Ctrl + Shift + Alt
    - win: Win
    - win_alt: Win + Alt
    - win_ctrl: Win + Ctrl
    - win_ctrl_alt: Win + Ctrl + Alt

- hotkeyKey: G
  $name: Activation Key
  $description: Enter the character key to press with your modifier combo (e.g. G, D, P, 1).

- defaultPenWidth: "3.5"
  $name: Default Pen Width (px)
  $description: Initial drawing thickness for the pen. You can resize this with the mouse wheel while drawing.

- defaultHighlighterWidth: "18.0"
  $name: Default Highlighter Width (px)
  $description: Initial drawing thickness for the highlighter tool.

- showBottomToolbar: true
  $name: Show Toolbar
  $description: Display the floating Windows 11 Fluent toolbar on the canvas.

- showTrayIcon: true
  $name: Show System Tray Icon
  $description: Show the WinDraw icon in the Windows notification area for quick access to settings and toggles.

- cornerRadius: fluent
  $name: UI Corner Roundness
  $description: Adjust the corner roundness of the toolbar and floating menus.
  $options:
    - square: Square (0 px)
    - subtle: Subtle (3 px)
    - fluent: Fluent Rounded (5 px - Default)
    - round: Rounded (8 px)
    - pill: Full Pill (12 px)

- autoSaveSnapshot: true
  $name: Auto-Save Snapshots
  $description: Automatically save your snapshots to your Pictures/WinDraw folder when capturing.

- freezeScreen: false
  $name: Freeze Screen on Activation
  $description: If enabled, taking a snapshot of the background freezes all active videos and animations. If disabled, you draw on a transparent live glass layer.

- customSnapshotPath: ""
  $name: Custom Snapshot Folder
  $description: Leave blank to use Default (Pictures/WinDraw). Otherwise, paste a full folder path (e.g., C:\Snips).

- defaultStartupTool: pen
  $name: Default Startup Tool
  $description: Choose which tool is active when you first open the overlay.
  $options:
    - pen: Pen (Default)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - pointer: Pointer (Click-Through)

- showToastNotifications: true
  $name: Show Toast Notifications
  $description: Display brief on-screen popups when taking snapshots or changing modes.

- laserTrailDuration: 800ms
  $name: Laser Trail Duration
  $description: Time before the laser trail disappears.
  $options:
    - 200ms: 200 ms (Fastest - Min)
    - 400ms: 400 ms (Fast)
    - 600ms: 600 ms (Snappy)
    - 800ms: 800 ms (Default)
    - 1200ms: 1.2 seconds (Medium)
    - 2000ms: 2.0 seconds (Long)
    - 3500ms: 3.5 seconds (Very Long)
    - 5000ms: 5.0 seconds (Persistent - Max)

- crossType: cross
  $name: Cursor Style (Crosshair / Brush)
  $description: Choose the inking cursor style. "cross" displays a precision crosshair. "brush" displays the active brush size and color directly under the cursor.
  $options:
    - cross: Crosshair
    - brush: Brush (Show brush size & color)

- crossSize: 16px
  $name: Crosshair Size
  $description: Length of the crosshair cursor arms when Crosshair style is active.
  $options:
    - 8px: 8 px (Small)
    - 12px: 12 px (Compact)
    - 16px: 16 px (Medium - Default)
    - 20px: 20 px (Large)
    - 24px: 24 px (Extra Large)
    - 32px: 32 px (Huge)

- showRadialColorRing: true
  $name: "Radial Menu: Show Color Swatches"
  $description: Display outer 360-degree color ring and recent color expansion hub on the radial quick menu. Disable for a minimalist, focused action dial.

- defaultWhiteboardMonitor: active
  $name: "Whiteboard / Blackboard Default Screen"
  $description: Choose which display is covered by the Whiteboard and Blackboard when activated via the radial menu or shortcut (K).
  $options:
    - active: Active Screen (Follows Cursor)
    - all: All Screens (Span Multi-Monitor Desktop)
    - primary: Primary Monitor Only
    - 1: Screen 1
    - 2: Screen 2
    - 3: Screen 3
    - 4: Screen 4

- radialSlot1: clear
  $name: "Radial Menu: Tool 1"
  $description: Action assigned to radial dial sector 1.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot2: snapshot
  $name: "Radial Menu: Tool 2"
  $description: Action assigned to radial dial sector 2.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot3: eraser
  $name: "Radial Menu: Tool 3"
  $description: Action assigned to radial dial sector 3.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot4: undo
  $name: "Radial Menu: Tool 4"
  $description: Action assigned to radial dial sector 4.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot5: pointer
  $name: "Radial Menu: Tool 5"
  $description: Action assigned to radial dial sector 5.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot6: ink_visible
  $name: "Radial Menu: Tool 6"
  $description: Action assigned to radial dial sector 6.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot7: pan
  $name: "Radial Menu: Tool 7"
  $description: Action assigned to radial dial sector 7.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot8: pen
  $name: "Radial Menu: Tool 8"
  $description: Action assigned to radial dial sector 8.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot9: laser
  $name: "Radial Menu: Tool 9"
  $description: Action assigned to radial dial sector 9.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw

- radialSlot10: shape
  $name: "Radial Menu: Tool 10"
  $description: Action assigned to radial dial sector 10.
  $options:
    - none: None (Disabled / Omit Sector)
    - clear: Clear All Ink
    - snapshot: Snapshot (Click for Snip, Ctrl+Click for Full)
    - snip: Interactive Snip
    - eraser: Eraser
    - undo: Undo
    - redo: Redo
    - pointer: Pointer (Click-Through)
    - ink_visible: Toggle Ink Visibility
    - pan: Pan / Zoom Canvas
    - pen: Pen (Draw Freehand)
    - highlighter: Highlighter
    - laser: Laser Pointer
    - shape: Shape (Scroll wheel to switch)
    - grid: Toggle Grid
    - whiteboard: Toggle Whiteboard / Blackboard
    - exit: Exit WinDraw
*/
// ==/WindhawkModSettings==

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <wincodec.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <shellapi.h>

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif
#include <vector>
#include <deque>
#include <unordered_map>
#include <cmath>
#include <string>
#include <algorithm>
#include <sstream>

// ----------------------------------------------------------------------------
// Configuration & Settings
// ----------------------------------------------------------------------------

enum class CursorType {
    Cross,
    Brush
};

enum class CanvasMonitorScope {
    ActiveCursor = -1, // Follows mouse cursor to whichever display it's currently on
    AllMonitors = 0,   // Spans across all connected screens
    Monitor1 = 1,      // Locked to Screen 1
    Monitor2 = 2,      // Locked to Screen 2
    Monitor3 = 3,      // Locked to Screen 3
    Monitor4 = 4,      // Locked to Screen 4
    Primary = 99       // Locked to Primary Monitor
};

enum class RadialAction {
    None = 0,
    Pen,
    Eraser,
    Highlighter,
    Laser,
    Shape,
    Grid,
    Whiteboard,
    Undo,
    Redo,
    Clear,
    Snapshot,
    Snip,
    Pan,
    Pointer,
    InkVisible,
    Exit
};

struct ModSettings {
    UINT hotkeyMod;
    UINT hotkeyKey;
    float defaultPenWidth;
    float defaultHighlighterWidth;
    bool showBottomToolbar;
    bool showTrayIcon;
    int cornerRadius;
    bool autoSaveSnapshot;
    bool freezeScreen;
    std::wstring customSnapshotPath;
    int defaultStartupTool;
    bool showToastNotifications;
    int laserTrailDuration;
    CursorType crossType;
    int crossSize;
    bool showRadialColorRing;
    CanvasMonitorScope defaultWhiteboardScope;
    RadialAction radialSlots[10];
} g_settings;

static std::vector<RadialAction> g_activeRadialSlots;

float GetFloatSetting(PCWSTR settingName, float defaultVal) {
    PCWSTR str = Wh_GetStringSetting(settingName);
    float val = defaultVal;
    if (str && str[0] != L'\0') {
        try { val = std::stof(str); } catch (...) { val = defaultVal; }
    }
    if (str) {
        Wh_FreeStringSetting(str);
    }
    return val;
}

RadialAction ParseRadialAction(PCWSTR str, RadialAction defaultAction) {
    if (!str || str[0] == L'\0') return defaultAction;
    if (_wcsicmp(str, L"none") == 0) return RadialAction::None;
    if (_wcsicmp(str, L"pen") == 0) return RadialAction::Pen;
    if (_wcsicmp(str, L"eraser") == 0) return RadialAction::Eraser;
    if (_wcsicmp(str, L"highlighter") == 0) return RadialAction::Highlighter;
    if (_wcsicmp(str, L"laser") == 0) return RadialAction::Laser;
    if (_wcsicmp(str, L"shape") == 0) return RadialAction::Shape;
    if (_wcsicmp(str, L"grid") == 0) return RadialAction::Grid;
    if (_wcsicmp(str, L"whiteboard") == 0) return RadialAction::Whiteboard;
    if (_wcsicmp(str, L"undo") == 0) return RadialAction::Undo;
    if (_wcsicmp(str, L"redo") == 0) return RadialAction::Redo;
    if (_wcsicmp(str, L"clear") == 0) return RadialAction::Clear;
    if (_wcsicmp(str, L"snapshot") == 0) return RadialAction::Snapshot;
    if (_wcsicmp(str, L"snip") == 0) return RadialAction::Snip;
    if (_wcsicmp(str, L"pan") == 0) return RadialAction::Pan;
    if (_wcsicmp(str, L"pointer") == 0) return RadialAction::Pointer;
    if (_wcsicmp(str, L"ink_visible") == 0) return RadialAction::InkVisible;
    if (_wcsicmp(str, L"exit") == 0) return RadialAction::Exit;
    return defaultAction;
}

void LoadSettings() {
    PCWSTR modStr = Wh_GetStringSetting(L"hotkeyMod");
    if (modStr) {
        if (_wcsicmp(modStr, L"alt") == 0) g_settings.hotkeyMod = MOD_ALT;
        else if (_wcsicmp(modStr, L"ctrl") == 0) g_settings.hotkeyMod = MOD_CONTROL;
        else if (_wcsicmp(modStr, L"shift") == 0) g_settings.hotkeyMod = MOD_SHIFT;
        else if (_wcsicmp(modStr, L"shift_alt") == 0) g_settings.hotkeyMod = MOD_SHIFT | MOD_ALT;
        else if (_wcsicmp(modStr, L"ctrl_shift") == 0) g_settings.hotkeyMod = MOD_CONTROL | MOD_SHIFT;
        else if (_wcsicmp(modStr, L"ctrl_shift_alt") == 0) g_settings.hotkeyMod = MOD_CONTROL | MOD_SHIFT | MOD_ALT;
        else if (_wcsicmp(modStr, L"win") == 0) g_settings.hotkeyMod = MOD_WIN;
        else if (_wcsicmp(modStr, L"win_alt") == 0) g_settings.hotkeyMod = MOD_WIN | MOD_ALT;
        else if (_wcsicmp(modStr, L"win_ctrl") == 0) g_settings.hotkeyMod = MOD_WIN | MOD_CONTROL;
        else if (_wcsicmp(modStr, L"win_ctrl_alt") == 0) g_settings.hotkeyMod = MOD_WIN | MOD_CONTROL | MOD_ALT;
        else g_settings.hotkeyMod = MOD_CONTROL | MOD_ALT;
        Wh_FreeStringSetting(modStr);
    } else {
        g_settings.hotkeyMod = MOD_CONTROL | MOD_ALT;
    }

    // Direct character input (e.g. "G", "D", "P", "1")
    PCWSTR keyStr = Wh_GetStringSetting(L"hotkeyKey");
    if (keyStr && keyStr[0] != L'\0') {
        SHORT vk = VkKeyScanW(keyStr[0]);
        if (vk != -1) {
            g_settings.hotkeyKey = (UINT)(vk & 0xFF);
        } else {
            g_settings.hotkeyKey = (UINT)towupper(keyStr[0]);
        }
        Wh_FreeStringSetting(keyStr);
    } else {
        g_settings.hotkeyKey = 'G';
        if (keyStr) Wh_FreeStringSetting(keyStr);
    }

    // Safely parse decimals for accurate brush widths
    g_settings.defaultPenWidth = GetFloatSetting(L"defaultPenWidth", 3.5f);
    if (g_settings.defaultPenWidth <= 0.5f) g_settings.defaultPenWidth = 3.5f;

    g_settings.defaultHighlighterWidth = GetFloatSetting(L"defaultHighlighterWidth", 18.0f);
    if (g_settings.defaultHighlighterWidth <= 1.0f) g_settings.defaultHighlighterWidth = 18.0f;

    g_settings.showBottomToolbar = Wh_GetIntSetting(L"showBottomToolbar") != 0;
    g_settings.showTrayIcon = Wh_GetIntSetting(L"showTrayIcon") != 0;

    PCWSTR radiusStr = Wh_GetStringSetting(L"cornerRadius");
    if (radiusStr) {
        if (_wcsicmp(radiusStr, L"square") == 0) g_settings.cornerRadius = 0;
        else if (_wcsicmp(radiusStr, L"subtle") == 0) g_settings.cornerRadius = 3;
        else if (_wcsicmp(radiusStr, L"round") == 0) g_settings.cornerRadius = 8;
        else if (_wcsicmp(radiusStr, L"pill") == 0) g_settings.cornerRadius = 12;
        else g_settings.cornerRadius = 5; // fluent (default)
        Wh_FreeStringSetting(radiusStr);
    } else {
        g_settings.cornerRadius = 5;
    }

    g_settings.autoSaveSnapshot = Wh_GetIntSetting(L"autoSaveSnapshot") != 0;
    g_settings.freezeScreen = Wh_GetIntSetting(L"freezeScreen") != 0;

    PCWSTR pathStr = Wh_GetStringSetting(L"customSnapshotPath");
    g_settings.customSnapshotPath = pathStr ? pathStr : L"";
    if (pathStr) Wh_FreeStringSetting(pathStr);

    PCWSTR startupToolStr = Wh_GetStringSetting(L"defaultStartupTool");
    if (startupToolStr) {
        if (_wcsicmp(startupToolStr, L"highlighter") == 0) g_settings.defaultStartupTool = 2;
        else if (_wcsicmp(startupToolStr, L"laser") == 0) g_settings.defaultStartupTool = 3;
        else if (_wcsicmp(startupToolStr, L"pointer") == 0) g_settings.defaultStartupTool = 4;
        else g_settings.defaultStartupTool = 1; // pen
        Wh_FreeStringSetting(startupToolStr);
    } else {
        g_settings.defaultStartupTool = 1;
    }

    g_settings.showToastNotifications = Wh_GetIntSetting(L"showToastNotifications") != 0;

    PCWSTR laserStr = Wh_GetStringSetting(L"laserTrailDuration");
    if (laserStr) {
        if (_wcsicmp(laserStr, L"200ms") == 0) g_settings.laserTrailDuration = 200;
        else if (_wcsicmp(laserStr, L"400ms") == 0) g_settings.laserTrailDuration = 400;
        else if (_wcsicmp(laserStr, L"600ms") == 0) g_settings.laserTrailDuration = 600;
        else if (_wcsicmp(laserStr, L"1200ms") == 0) g_settings.laserTrailDuration = 1200;
        else if (_wcsicmp(laserStr, L"2000ms") == 0) g_settings.laserTrailDuration = 2000;
        else if (_wcsicmp(laserStr, L"3500ms") == 0) g_settings.laserTrailDuration = 3500;
        else if (_wcsicmp(laserStr, L"5000ms") == 0) g_settings.laserTrailDuration = 5000;
        else g_settings.laserTrailDuration = 800;
        Wh_FreeStringSetting(laserStr);
    } else {
        g_settings.laserTrailDuration = 800;
    }

    PCWSTR cTypeStr = Wh_GetStringSetting(L"crossType");
    if (cTypeStr) {
        if (_wcsicmp(cTypeStr, L"brush") == 0) {
            g_settings.crossType = CursorType::Brush;
        } else {
            g_settings.crossType = CursorType::Cross;
        }
        Wh_FreeStringSetting(cTypeStr);
    } else {
        g_settings.crossType = CursorType::Cross;
    }

    PCWSTR cSizeStr = Wh_GetStringSetting(L"crossSize");
    if (cSizeStr) {
        if (_wcsicmp(cSizeStr, L"8px") == 0) g_settings.crossSize = 8;
        else if (_wcsicmp(cSizeStr, L"12px") == 0) g_settings.crossSize = 12;
        else if (_wcsicmp(cSizeStr, L"20px") == 0) g_settings.crossSize = 20;
        else if (_wcsicmp(cSizeStr, L"24px") == 0) g_settings.crossSize = 24;
        else if (_wcsicmp(cSizeStr, L"32px") == 0) g_settings.crossSize = 32;
        else g_settings.crossSize = 16;
        Wh_FreeStringSetting(cSizeStr);
    } else {
        g_settings.crossSize = 16;
    }

    g_settings.showRadialColorRing = Wh_GetIntSetting(L"showRadialColorRing") != 0;

    PCWSTR wbMonStr = Wh_GetStringSetting(L"defaultWhiteboardMonitor");
    if (wbMonStr) {
        if (_wcsicmp(wbMonStr, L"all") == 0) g_settings.defaultWhiteboardScope = CanvasMonitorScope::AllMonitors;
        else if (_wcsicmp(wbMonStr, L"primary") == 0) g_settings.defaultWhiteboardScope = CanvasMonitorScope::Primary;
        else if (wcscmp(wbMonStr, L"1") == 0) g_settings.defaultWhiteboardScope = CanvasMonitorScope::Monitor1;
        else if (wcscmp(wbMonStr, L"2") == 0) g_settings.defaultWhiteboardScope = CanvasMonitorScope::Monitor2;
        else if (wcscmp(wbMonStr, L"3") == 0) g_settings.defaultWhiteboardScope = CanvasMonitorScope::Monitor3;
        else if (wcscmp(wbMonStr, L"4") == 0) g_settings.defaultWhiteboardScope = CanvasMonitorScope::Monitor4;
        else g_settings.defaultWhiteboardScope = CanvasMonitorScope::ActiveCursor;
        Wh_FreeStringSetting(wbMonStr);
    } else {
        g_settings.defaultWhiteboardScope = CanvasMonitorScope::ActiveCursor;
    }

    RadialAction defaultRadialSlots[10] = {
        RadialAction::Clear,
        RadialAction::Snapshot,
        RadialAction::Eraser,
        RadialAction::Undo,
        RadialAction::Pointer,
        RadialAction::InkVisible,
        RadialAction::Pan,
        RadialAction::Pen,
        RadialAction::Laser,
        RadialAction::Shape
    };

    g_activeRadialSlots.clear();
    for (int i = 0; i < 10; ++i) {
        wchar_t slotKey[32];
        wsprintfW(slotKey, L"radialSlot%d", i + 1);
        PCWSTR valStr = Wh_GetStringSetting(slotKey);
        g_settings.radialSlots[i] = ParseRadialAction(valStr, defaultRadialSlots[i]);
        if (valStr) Wh_FreeStringSetting(valStr);

        if (g_settings.radialSlots[i] != RadialAction::None) {
            g_activeRadialSlots.push_back(g_settings.radialSlots[i]);
        }
    }

    if (g_activeRadialSlots.empty()) {
        for (int i = 0; i < 10; ++i) {
            if (defaultRadialSlots[i] != RadialAction::None) {
                g_activeRadialSlots.push_back(defaultRadialSlots[i]);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Geometry & Stroke Model
// ----------------------------------------------------------------------------

enum class ShapeType {
    Freehand,
    Line,
    Arrow,
    Rectangle,
    Ellipse,
    Triangle
};

enum class ToolMode {
    Pen,
    Highlighter,
    Eraser,
    Pan,
    Pointer,
    Laser
};

struct LaserPoint {
    float x;
    float y;
    ULONGLONG timestamp;
};
struct LaserStroke {
    std::deque<LaserPoint> points;
    ID2D1PathGeometry* pCachedGeometry;

    LaserStroke() : pCachedGeometry(nullptr) {}
    ~LaserStroke() { ReleaseGeometry(); }

    LaserStroke(const LaserStroke& other) = delete;
    LaserStroke& operator=(const LaserStroke& other) = delete;

    LaserStroke(LaserStroke&& other) noexcept
        : points(std::move(other.points)), pCachedGeometry(other.pCachedGeometry) {
        other.pCachedGeometry = nullptr;
    }

    LaserStroke& operator=(LaserStroke&& other) noexcept {
        if (this != &other) {
            ReleaseGeometry();
            points = std::move(other.points);
            pCachedGeometry = other.pCachedGeometry;
            other.pCachedGeometry = nullptr;
        }
        return *this;
    }

    void ReleaseGeometry() {
        if (pCachedGeometry) {
            pCachedGeometry->Release();
            pCachedGeometry = nullptr;
        }
    }

    void InvalidateGeometry() {
        ReleaseGeometry();
    }
};
[[clang::no_destroy]] static std::deque<LaserStroke> g_laserStrokes;
static bool g_isLaserDrawing = false;

enum class GridStyle {
    None = 0,
    DotGrid = 1,
    GraphLines = 2
};

enum class GridDensity {
    Fine = 24,       // 24 px spacing (tight)
    Medium = 48,     // 48 px spacing (standard)
    Coarse = 96      // 96 px spacing (broad)
};

enum class CanvasBg {
    Transparent = 0,
    Whiteboard = 1,
    Blackboard = 2
};

struct StrokePoint {
    float x;
    float y;
};

struct Stroke {
    std::vector<StrokePoint> points;
    D2D1_COLOR_F color;
    float width;
    bool isHighlighter;
    ShapeType shapeType;
    StrokePoint startPt;
    StrokePoint endPt;
    ID2D1PathGeometry* pCachedGeometry;
    D2D1_RECT_F bounds;

    Stroke()
        : color(D2D1::ColorF(0, 0, 0, 1.0f)),
          width(3.5f),
          isHighlighter(false),
          shapeType(ShapeType::Freehand),
          startPt{ 0.0f, 0.0f },
          endPt{ 0.0f, 0.0f },
          pCachedGeometry(nullptr),
          bounds{ 0.0f, 0.0f, 0.0f, 0.0f }
    {}

    Stroke(const Stroke& other) {
        CopyFrom(other);
    }

    Stroke(Stroke&& other) noexcept {
        MoveFrom(std::move(other));
    }

    Stroke& operator=(const Stroke& other) {
        if (this != &other) {
            ReleaseGeometry();
            CopyFrom(other);
        }
        return *this;
    }

    Stroke& operator=(Stroke&& other) noexcept {
        if (this != &other) {
            ReleaseGeometry();
            MoveFrom(std::move(other));
        }
        return *this;
    }

    ~Stroke() {
        ReleaseGeometry();
    }

    void ReleaseGeometry() {
        if (pCachedGeometry) {
            pCachedGeometry->Release();
            pCachedGeometry = nullptr;
        }
    }

    void InvalidateCache() {
        ReleaseGeometry();
        ComputeBounds();
    }

    void ComputeBounds() {
        if (shapeType == ShapeType::Freehand) {
            if (points.empty()) {
                bounds = D2D1::RectF(0, 0, 0, 0);
                return;
            }
            float minX = points[0].x, maxX = points[0].x;
            float minY = points[0].y, maxY = points[0].y;
            for (const auto& pt : points) {
                if (pt.x < minX) minX = pt.x;
                if (pt.x > maxX) maxX = pt.x;
                if (pt.y < minY) minY = pt.y;
                if (pt.y > maxY) maxY = pt.y;
            }
            float pad = width * 0.5f + 4.0f;
            bounds = D2D1::RectF(minX - pad, minY - pad, maxX + pad, maxY + pad);
        }
        else if (shapeType == ShapeType::Line || shapeType == ShapeType::Arrow) {
            float minX = std::min(startPt.x, endPt.x);
            float maxX = std::max(startPt.x, endPt.x);
            float minY = std::min(startPt.y, endPt.y);
            float maxY = std::max(startPt.y, endPt.y);
            float pad = std::max(width * 0.5f, 18.0f) + 4.0f;
            bounds = D2D1::RectF(minX - pad, minY - pad, maxX + pad, maxY + pad);
        }
        else if (shapeType == ShapeType::Rectangle || shapeType == ShapeType::Ellipse || shapeType == ShapeType::Triangle) {
            float minX = std::min(startPt.x, endPt.x);
            float maxX = std::max(startPt.x, endPt.x);
            float minY = std::min(startPt.y, endPt.y);
            float maxY = std::max(startPt.y, endPt.y);
            float pad = width * 0.5f + 4.0f;
            bounds = D2D1::RectF(minX - pad, minY - pad, maxX + pad, maxY + pad);
        }
    }

private:
    void CopyFrom(const Stroke& other) {
        points = other.points;
        color = other.color;
        width = other.width;
        isHighlighter = other.isHighlighter;
        shapeType = other.shapeType;
        startPt = other.startPt;
        endPt = other.endPt;
        bounds = other.bounds;
        pCachedGeometry = other.pCachedGeometry;
        if (pCachedGeometry) {
            pCachedGeometry->AddRef();
        }
    }

    void MoveFrom(Stroke&& other) noexcept {
        points = std::move(other.points);
        color = other.color;
        width = other.width;
        isHighlighter = other.isHighlighter;
        shapeType = other.shapeType;
        startPt = other.startPt;
        endPt = other.endPt;
        bounds = other.bounds;
        pCachedGeometry = other.pCachedGeometry;
        other.pCachedGeometry = nullptr;
    }
};

// ----------------------------------------------------------------------------
// 360-Degree Orbital & Preset Color Palette
// ----------------------------------------------------------------------------

static const D2D1_COLOR_F kPresetColors[] = {
    D2D1::ColorF(0.92f, 0.22f, 0.22f, 1.0f), // 0. Crimson Red
    D2D1::ColorF(0.96f, 0.42f, 0.15f, 1.0f), // 1. Tangelo Orange
    D2D1::ColorF(0.98f, 0.65f, 0.12f, 1.0f), // 2. Amber Gold
    D2D1::ColorF(0.98f, 0.82f, 0.15f, 1.0f), // 3. Sun Yellow
    D2D1::ColorF(0.65f, 0.85f, 0.18f, 1.0f), // 4. Lime
    D2D1::ColorF(0.20f, 0.78f, 0.35f, 1.0f), // 5. Emerald Green
    D2D1::ColorF(0.12f, 0.82f, 0.70f, 1.0f), // 6. Teal Mint
    D2D1::ColorF(0.15f, 0.80f, 0.95f, 1.0f), // 7. Electric Cyan
    D2D1::ColorF(0.18f, 0.52f, 0.95f, 1.0f), // 8. Cobalt Blue
    D2D1::ColorF(0.38f, 0.35f, 0.95f, 1.0f), // 9. Indigo
    D2D1::ColorF(0.65f, 0.28f, 0.92f, 1.0f), // 10. Violet Purple
    D2D1::ColorF(0.92f, 0.25f, 0.75f, 1.0f), // 11. Magenta Pink
    D2D1::ColorF(0.96f, 0.32f, 0.52f, 1.0f), // 12. Rose Coral
    D2D1::ColorF(0.95f, 0.96f, 0.98f, 1.0f), // 13. Titanium White
    D2D1::ColorF(0.55f, 0.58f, 0.64f, 1.0f), // 14. Silver Gray
    D2D1::ColorF(0.12f, 0.14f, 0.18f, 1.0f)  // 15. Charcoal Black
};

static const size_t kPresetColorCount = sizeof(kPresetColors) / sizeof(kPresetColors[0]);

// ----------------------------------------------------------------------------
// Application State
// ----------------------------------------------------------------------------

#define WM_APP_SETTINGS_CHANGED (WM_APP + 1)
#define WM_APP_EXIT             (WM_APP + 2)

static HWND g_hOverlayWnd = NULL;
static HWND g_hHotkeyWnd = NULL;
static HANDLE g_hHotkeyThread = NULL;
static DWORD g_hotkeyThreadId = 0;
static bool g_bIsActive = false;
static float g_toolbarDpiScale = 1.0f;
static float g_radialDpiScale = 1.0f;
static float g_toolbarExpandedWidth = 887.0f;
static float g_toolbarPillWidth = 82.0f;

inline HMODULE GetCurrentModuleHandle() {
    HMODULE hModule = nullptr;
    if (!GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)GetCurrentModuleHandle,
        &hModule)) {
        return nullptr;
    }
    return hModule;
}

template <typename T>
inline void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

inline float GetDpiScaleForMonitor(HMONITOR hMon) {
    if (!hMon) return 1.0f;
    static auto pGetDpiForMonitor = []() -> HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*) {
        HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
        if (!hShcore) hShcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        return hShcore ? (HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*))GetProcAddress(hShcore, "GetDpiForMonitor") : nullptr;
    }();
    if (pGetDpiForMonitor) {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(pGetDpiForMonitor(hMon, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY)) && dpiX > 0) {
            return (float)dpiX / 96.0f;
        }
    }
    return 1.0f;
}

inline float GetDpiScaleAtPoint(float clientX, float clientY) {
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    POINT pt = { (LONG)std::round(clientX + (float)vx), (LONG)std::round(clientY + (float)vy) };
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    return GetDpiScaleForMonitor(hMon);
}

inline float GetFlyoutDpiScale(const D2D1_RECT_F& rect) {
    if (rect.right > rect.left && rect.bottom > rect.top) {
        return GetDpiScaleAtPoint((rect.left + rect.right) * 0.5f, (rect.top + rect.bottom) * 0.5f);
    }
    return g_toolbarDpiScale;
}

static ID2D1Factory* g_pD2DFactory = NULL;
static ID2D1HwndRenderTarget* g_pRenderTarget = NULL;
static ID2D1StrokeStyle* g_pRoundStrokeStyle = NULL;
static ID2D1StrokeStyle* g_pLaserFlatStrokeStyle = NULL;
static ID2D1SolidColorBrush* g_pStrokeBrush = NULL;
static ID2D1SolidColorBrush* g_pWhiteboardBrush = NULL;
static ID2D1SolidColorBrush* g_pBlackboardBrush = NULL;
static ID2D1Bitmap* g_pDesktopBitmap = NULL;
static IDWriteFactory* g_pDWriteFactory = NULL;
static IDWriteTextFormat* g_pTextFormat = NULL;
static IDWriteTextFormat* g_pIconFormat = NULL;
static IDWriteTextFormat* g_pRadialIconFormat = NULL;
static IDWriteTextFormat* g_pCenterBadgeFormat = NULL;
static IDWriteTextFormat* g_pMenuTextFormat = NULL;
static IDWriteTextFormat* g_pMenuKeyFormat = NULL;
static IDWriteTextFormat* g_pToolbarKeyFormat = NULL;
static IDWriteTextFormat* g_pToastTextFormat = NULL;
static IDWriteTextFormat* g_pToastIconFormat = NULL;
static float g_currentToastFontScale = 0.0f;
static IWICImagingFactory* g_pWICFactory = NULL;

// Radial Satellite Arc Geometry Cache
static ID2D1PathGeometry* g_pRadialSatelliteArcGeom = nullptr;
static int g_cachedSatelliteNumOrbs = -1;
static float g_cachedSatelliteScale = -1.0f;

// DirectWrite Text Layout Cache
struct CachedTextLayoutKey {
    std::wstring text;
    IDWriteTextFormat* pFormat;
    int maxW;
    int maxH;

    bool operator==(const CachedTextLayoutKey& o) const {
        return maxW == o.maxW && maxH == o.maxH && pFormat == o.pFormat && text == o.text;
    }
};

struct CachedTextLayoutKeyHash {
    size_t operator()(const CachedTextLayoutKey& k) const {
        size_t h = std::hash<std::wstring>()(k.text);
        h ^= std::hash<void*>()((void*)k.pFormat) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(k.maxW) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(k.maxH) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

static std::unordered_map<CachedTextLayoutKey, IDWriteTextLayout*, CachedTextLayoutKeyHash> g_textLayoutCache;

inline IDWriteTextLayout* GetCachedTextLayout(const std::wstring& text, IDWriteTextFormat* pFormat, float maxW, float maxH) {
    if (!g_pDWriteFactory || !pFormat) return nullptr;
    int iw = (int)std::round(maxW);
    int ih = (int)std::round(maxH);
    CachedTextLayoutKey key{ text, pFormat, iw, ih };
    auto it = g_textLayoutCache.find(key);
    if (it != g_textLayoutCache.end()) {
        return it->second;
    }
    IDWriteTextLayout* pLayout = nullptr;
    HRESULT hr = g_pDWriteFactory->CreateTextLayout(
        text.c_str(), (UINT32)text.length(), pFormat, maxW, maxH, &pLayout
    );
    if (SUCCEEDED(hr) && pLayout) {
        g_textLayoutCache[key] = pLayout;
        return pLayout;
    }
    return nullptr;
}

inline void ClearTextLayoutCache() {
    for (auto& pair : g_textLayoutCache) {
        SafeRelease(pair.second);
    }
    g_textLayoutCache.clear();
}

inline void DrawCachedText(ID2D1HwndRenderTarget* pRT, const std::wstring& text, IDWriteTextFormat* pFormat,
                           const D2D1_RECT_F& rect, ID2D1Brush* pBrush) {
    float w = rect.right - rect.left;
    float h = rect.bottom - rect.top;
    if (w <= 0.0f || h <= 0.0f) return;
    IDWriteTextLayout* pLayout = GetCachedTextLayout(text, pFormat, w, h);
    if (pLayout) {
        pRT->DrawTextLayout(D2D1::Point2F(rect.left, rect.top), pLayout, pBrush);
    } else {
        pRT->DrawText(text.c_str(), (UINT32)text.length(), pFormat, rect, pBrush);
    }
}

[[clang::no_destroy]] static std::vector<Stroke> g_strokes;
[[clang::no_destroy]] static std::vector<std::vector<Stroke>> g_undoStack;
[[clang::no_destroy]] static std::vector<std::vector<Stroke>> g_redoStack;
static const size_t kMaxUndoLevels = 20;

void PushUndoState() {
    g_undoStack.push_back(g_strokes);
    if (g_undoStack.size() > kMaxUndoLevels) {
        g_undoStack.erase(g_undoStack.begin());
    }
    g_redoStack.clear();
}
[[clang::no_destroy]] static Stroke g_currentStroke;
static bool g_isDrawing = false;

static ToolMode g_currentTool = ToolMode::Pen;
static ShapeType g_currentShape = ShapeType::Freehand;
static D2D1_COLOR_F g_activeColor = kPresetColors[0];
static float g_currentPenWidth = 3.5f;
static bool g_inkVisible = true;

// Unified Brush Sizing Constants
constexpr float kMinPenWidth = 1.0f;
constexpr float kMaxPenWidth = 60.0f;
constexpr float kMinHighlighterWidth = 4.0f;
constexpr float kMaxHighlighterWidth = 80.0f;
constexpr int   kMinLaserTrailMs = 200;
constexpr int   kMaxLaserTrailMs = 5000;
constexpr float kMinEraserRadius = 6.0f;
constexpr float kMaxEraserRadius = 150.0f;

// Pan & Zoom state
static float g_panOffsetX = 0.0f;
static float g_panOffsetY = 0.0f;
static float g_zoomScale = 1.0f;
static bool g_isPanning = false;
static POINT g_panStartPos = { 0, 0 };

// Right-click eraser tracking
static bool g_isRightMouseDown = false;
static bool g_isRightClickErasing = false;
static bool g_wheelUsedWhileRightMouseDown = false;
static bool g_isLeftClickErasing = false;
static bool g_isRightClickClearing = false;
static POINT g_rightMouseDownPos = { 0, 0 };
static ULONGLONG g_rightMouseDownTime = 0;
static float g_cursorX = 0;
static float g_cursorY = 0;
static float g_eraserRadius = 24.0f;

// Timer IDs
const UINT_PTR TIMER_ID_UI_ANIMATION  = 1; // 30ms: Toast fade, size preview, zoom preview
const UINT_PTR TIMER_ID_POINTER_WATCH = 2; // 100ms: Pointer (click-through) mode toolbar interaction
const UINT_PTR TIMER_ID_LASER         = 3; // 16ms: High-precision laser trail physics & erosion

// Radial Menu State
enum class RadialTarget {
    None = -1,
    Sector = 0,      // Dynamic tool sector (index in g_radialHoverSector)
    Center = 1,      // Center Hub
    ColorOrb = 2,    // Outer orbital colors
    RecentHub = 3,   // 12 o'clock Recent Colors expansion hub
    RecentOrb = 4    // Layer 2 Recent Color satellite orbs
};
static bool g_radialActive = false;
static float g_radialX = 0;
static float g_radialY = 0;
static RadialTarget g_radialHoverTarget = RadialTarget::None;
static int g_radialHoverSector = -1;
static int g_hoveredOrb = -1;
static bool g_radialRecentFanOpen = false;
static int g_hoveredRecentOrb = -1; // 0..4

// Toolbar State & Dragging
struct ToolbarButton {
    int id;
    D2D1_RECT_F rect;
    std::wstring label;
    bool isPen;
    D2D1_COLOR_F penColor;
    std::wstring shortcut;
    bool isCustomColor = false;
};
static std::vector<ToolbarButton> g_toolbarButtons;
static std::vector<float> g_toolbarDividers;
static D2D1_RECT_F g_toolbarRect = { 0, 0, 0, 0 };
static int g_hoveredToolbarBtn = -1;
static bool g_isDraggingToolbar = false;
static POINT g_toolbarDragStart = { 0, 0 };
static POINT g_toolbarDragStartInit = { 0, 0 };
static float g_toolbarCustomX = -1.0f;
static float g_toolbarCustomY = -1.0f;
static bool g_toolbarCollapsed = false;
static bool g_isPillMouseDown = false;
static bool g_isPillDragging = false;

// Shapes Action Modal / Flyout State
static bool g_shapesFlyoutOpen = false;
static D2D1_RECT_F g_shapesFlyoutRect = { 0, 0, 0, 0 };
static int g_hoveredShapeFlyoutItem = -1;

// Grid Overlay & Action Modal State
static GridStyle g_gridStyle = GridStyle::None;
static GridDensity g_gridDensity = GridDensity::Medium;
static ID2D1BitmapBrush* g_pGridBrush = nullptr;
static bool g_gridFlyoutOpen = false;
static D2D1_RECT_F g_gridFlyoutRect = { 0, 0, 0, 0 };
static int g_hoveredGridFlyoutItem = -1;

// Canvas Backdrop (Temporary Whiteboard / Blackboard) State
static CanvasBg g_canvasBg = CanvasBg::Transparent;
static CanvasMonitorScope g_canvasScope = CanvasMonitorScope::ActiveCursor;
static bool g_backdropFlyoutOpen = false;
static D2D1_RECT_F g_backdropFlyoutRect = { 0, 0, 0, 0 };
static int g_hoveredBackdropFlyoutItem = -1;

// Custom Color & Opacity Studio State
struct CustomColorState {
    float hue = 340.0f;       // 0.0 to 360.0 degrees
    float sat = 0.70f;        // 0.0 to 1.0
    float val = 0.85f;        // 0.0 to 1.0
    float alpha = 1.00f;      // 0.05 to 1.00
    D2D1_COLOR_F activeColor = D2D1::ColorF(0.85f, 0.25f, 0.45f, 1.0f);
};
static CustomColorState g_customColor;
static std::vector<D2D1_COLOR_F> g_recentColors = {
    D2D1::ColorF(0.92f, 0.22f, 0.22f, 1.0f), // Crimson Red
    D2D1::ColorF(0.18f, 0.52f, 0.95f, 1.0f), // Cobalt Blue
    D2D1::ColorF(0.20f, 0.78f, 0.35f, 1.0f), // Emerald Green
    D2D1::ColorF(0.98f, 0.65f, 0.12f, 1.0f), // Amber Gold
    D2D1::ColorF(0.65f, 0.28f, 0.92f, 1.0f)  // Violet Purple
};
static bool g_colorFlyoutOpen = false;
static D2D1_RECT_F g_colorFlyoutRect = { 0, 0, 0, 0 };
enum class ColorPickerDrag { None, SatValCanvas, HueBar, AlphaBar };
static ColorPickerDrag g_pickerDrag = ColorPickerDrag::None;
static bool g_isEyedropperActive = false;
static int g_hoveredRecentSwatch = -1;
static int g_hoveredColorStudioAction = -1; // 1: Eyedropper, 2: Copy Hex

inline D2D1_COLOR_F HSVtoRGB(float h, float s, float v, float a = 1.0f) {
    if (s <= 0.0001f) {
        return D2D1::ColorF(v, v, v, a);
    }
    while (h < 0.0f) h += 360.0f;
    while (h >= 360.0f) h -= 360.0f;
    float hSector = h / 60.0f;
    int i = (int)hSector;
    float f = hSector - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i) {
        case 0: return D2D1::ColorF(v, t, p, a);
        case 1: return D2D1::ColorF(q, v, p, a);
        case 2: return D2D1::ColorF(p, v, t, a);
        case 3: return D2D1::ColorF(p, q, v, a);
        case 4: return D2D1::ColorF(t, p, v, a);
        default: return D2D1::ColorF(v, p, q, a);
    }
}

inline void RGBtoHSV(D2D1_COLOR_F c, float& h, float& s, float& v) {
    float r = std::max(0.0f, std::min(1.0f, c.r));
    float g = std::max(0.0f, std::min(1.0f, c.g));
    float b = std::max(0.0f, std::min(1.0f, c.b));
    float maxV = std::max(r, std::max(g, b));
    float minV = std::min(r, std::min(g, b));
    float delta = maxV - minV;
    v = maxV;
    if (maxV <= 0.0001f) {
        s = 0.0f;
        h = 0.0f;
        return;
    }
    s = delta / maxV;
    if (delta <= 0.0001f) {
        h = 0.0f;
        return;
    }
    if (std::abs(r - maxV) < 0.0001f) {
        h = 60.0f * ((g - b) / delta);
    } else if (std::abs(g - maxV) < 0.0001f) {
        h = 60.0f * (2.0f + (b - r) / delta);
    } else {
        h = 60.0f * (4.0f + (r - g) / delta);
    }
    if (h < 0.0f) h += 360.0f;
}

// Forward declarations for Windhawk local storage persistence
void SavePersistentRecentColors();
void SavePersistentCustomColor();
void SavePersistentToolbarState();
void LoadPersistentState();

inline void PushRecentColor(D2D1_COLOR_F c) {
    for (auto it = g_recentColors.begin(); it != g_recentColors.end(); ++it) {
        if (std::abs(it->r - c.r) < 0.015f &&
            std::abs(it->g - c.g) < 0.015f &&
            std::abs(it->b - c.b) < 0.015f &&
            std::abs(it->a - c.a) < 0.02f) {
            g_recentColors.erase(it);
            break;
        }
    }
    g_recentColors.insert(g_recentColors.begin(), c);
    if (g_recentColors.size() > 5) g_recentColors.resize(5);
    SavePersistentRecentColors();
}

inline std::wstring ColorToHex(D2D1_COLOR_F c, bool includeAlpha = false) {
    int r = (int)std::round(std::max(0.0f, std::min(1.0f, c.r)) * 255.0f);
    int g = (int)std::round(std::max(0.0f, std::min(1.0f, c.g)) * 255.0f);
    int b = (int)std::round(std::max(0.0f, std::min(1.0f, c.b)) * 255.0f);
    int a = (int)std::round(std::max(0.0f, std::min(1.0f, c.a)) * 255.0f);
    WCHAR buf[32];
    if (includeAlpha || a < 255) {
        wsprintfW(buf, L"#%02X%02X%02X%02X", r, g, b, a);
    } else {
        wsprintfW(buf, L"#%02X%02X%02X", r, g, b);
    }
    return std::wstring(buf);
}

// ----------------------------------------------------------------------------
// Windhawk Local Storage Persistence (Wh_Set*Value / Wh_Get*Value)
// ----------------------------------------------------------------------------

inline void SavePersistentRecentColors() {
    std::wstring str;
    for (size_t i = 0; i < g_recentColors.size(); ++i) {
        int r = (int)std::round(std::max(0.0f, std::min(1.0f, g_recentColors[i].r)) * 255.0f);
        int g = (int)std::round(std::max(0.0f, std::min(1.0f, g_recentColors[i].g)) * 255.0f);
        int b = (int)std::round(std::max(0.0f, std::min(1.0f, g_recentColors[i].b)) * 255.0f);
        WCHAR buf[16];
        wsprintfW(buf, L"#%02X%02X%02X", r, g, b);
        if (i > 0) str += L",";
        str += buf;
    }
    Wh_SetStringValue(L"recentColors", str.c_str());
}

inline void SavePersistentCustomColor() {
    WCHAR buf[128];
    int h100 = (int)std::round(g_customColor.hue * 100.0f);
    int s100 = (int)std::round(g_customColor.sat * 100.0f);
    int v100 = (int)std::round(g_customColor.val * 100.0f);
    int a100 = (int)std::round(g_customColor.alpha * 100.0f);
    wsprintfW(buf, L"%d,%d,%d,%d", h100, s100, v100, a100);
    Wh_SetStringValue(L"customColor", buf);
}

inline void SavePersistentToolbarState() {
    Wh_SetIntValue(L"toolbarCollapsed", g_toolbarCollapsed ? 1 : 0);
    if (g_toolbarCustomX >= 0.0f && g_toolbarCustomY >= 0.0f) {
        Wh_SetIntValue(L"toolbarCustomX", (int)std::round(g_toolbarCustomX));
        Wh_SetIntValue(L"toolbarCustomY", (int)std::round(g_toolbarCustomY));
    }
}

inline void LoadPersistentState() {
    // 1. Load Last 5 Recent Colors
    WCHAR recentBuf[256] = { 0 };
    if (Wh_GetStringValue(L"recentColors", recentBuf, 256) > 0) {
        std::vector<D2D1_COLOR_F> loaded;
        std::wstringstream ss(recentBuf);
        std::wstring item;
        while (std::getline(ss, item, L',')) {
            if (item.length() >= 7 && item[0] == L'#') {
                try {
                    unsigned long hex = std::stoul(item.substr(1), nullptr, 16);
                    float r = ((hex >> 16) & 0xFF) / 255.0f;
                    float g = ((hex >> 8) & 0xFF) / 255.0f;
                    float b = (hex & 0xFF) / 255.0f;
                    loaded.push_back(D2D1::ColorF(r, g, b, 1.0f));
                } catch (...) {}
            }
        }
        if (loaded.size() == 5) {
            g_recentColors = loaded;
        }
    }

    // 2. Load Custom Color State
    WCHAR customBuf[128] = { 0 };
    if (Wh_GetStringValue(L"customColor", customBuf, 128) > 0) {
        int h100 = 0, s100 = 0, v100 = 0, a100 = 0;
        if (swscanf_s(customBuf, L"%d,%d,%d,%d", &h100, &s100, &v100, &a100) == 4) {
            g_customColor.hue = std::max(0.0f, std::min(360.0f, (float)h100 / 100.0f));
            g_customColor.sat = std::max(0.0f, std::min(1.0f, (float)s100 / 100.0f));
            g_customColor.val = std::max(0.0f, std::min(1.0f, (float)v100 / 100.0f));
            g_customColor.alpha = std::max(0.05f, std::min(1.0f, (float)a100 / 100.0f));
            g_customColor.activeColor = HSVtoRGB(g_customColor.hue, g_customColor.sat, g_customColor.val, g_customColor.alpha);
        }
    }

    // 3. Load Toolbar Collapsed & Position
    int collapsed = Wh_GetIntValue(L"toolbarCollapsed", -1);
    if (collapsed != -1) {
        g_toolbarCollapsed = (collapsed != 0);
    }
    int customX = Wh_GetIntValue(L"toolbarCustomX", -99999);
    int customY = Wh_GetIntValue(L"toolbarCustomY", -99999);
    if (customX != -99999 && customY != -99999) {
        g_toolbarCustomX = (float)customX;
        g_toolbarCustomY = (float)customY;
    }
}

// Toast feedback
static ULONGLONG g_toastStartTime = 0;
static std::wstring g_toastMessage = L"";

// Eraser Undo State Tracking
static bool g_hasPushedUndoForCurrentErase = false;

// Brush & Zoom size preview timer
static ULONGLONG g_sizePreviewTime = 0;
static ULONGLONG g_zoomPreviewTime = 0;

// Snipping / Region Snapshot State
static bool g_isSnipping = false;
static bool g_isSnippingDrag = false;
static bool g_hideUIForCapture = false;
static POINT g_snipStartPt = { 0, 0 };
static POINT g_snipEndPt = { 0, 0 };
static HBITMAP g_hSnipBackdrop = NULL;
static int g_snipBackdropW = 0;
static int g_snipBackdropH = 0;

#define WM_USER_TOGGLE_POINTER (WM_USER + 101)
#define WM_USER_TRAYICON       (WM_USER + 102)
#define WM_USER_UPDATE_TRAY    (WM_USER + 103)

// ----------------------------------------------------------------------------
// System Tray Notification Icon State & Helpers
// ----------------------------------------------------------------------------

static const UINT kTrayIconId = 1001;
static UINT g_wmTaskbarCreated = 0;
static NOTIFYICONDATAW g_nid = { sizeof(NOTIFYICONDATAW) };
static bool g_bTrayIconVisible = false;
static HICON g_hTrayIcon = NULL;
static ULONGLONG g_lastOverlayOpenTime = 0;

bool IsClickOnTrayIcon() {
    if (!g_settings.showTrayIcon || !g_bTrayIconVisible || !g_hHotkeyWnd) return false;
    if (GetTickCount64() - g_lastOverlayOpenTime < 300) return false;

    NOTIFYICONIDENTIFIER nid = { sizeof(NOTIFYICONIDENTIFIER) };
    nid.hWnd = g_hHotkeyWnd;
    nid.uID = kTrayIconId;
    RECT rc = { 0 };
    if (SUCCEEDED(Shell_NotifyIconGetRect(&nid, &rc))) {
        POINT pt;
        GetCursorPos(&pt);
        return (pt.x >= (rc.left - 6) && pt.x <= (rc.right + 6) &&
                pt.y >= (rc.top - 6) && pt.y <= (rc.bottom + 6));
    }
    return false;
}

// ----------------------------------------------------------------------------
// Forward Declarations
// ----------------------------------------------------------------------------

void ShowOverlay();
void HideOverlay();
void SetToolMode(ToolMode newMode);
void CaptureDesktop();
void ReleaseD2DResources();
void InvalidateOverlay();
void ShowToastNotification(const std::wstring& msg);
void RenderOverlay();
void DrawZoomPreview(ID2D1HwndRenderTarget* pRT);
bool EraseWholeShapeAt(float x, float y, float radius);
void SaveBitmapToPNG(HBITMAP hBitmap, const std::wstring& filePath);
void UpdateTrayIcon(HWND hwnd);
void RemoveTrayIcon();
void StartSnipping();
void CancelSnipping();
bool PreparePristineBackdrop();
void CaptureFullScreenSnapshot();
void SaveCroppedSnapshot(int left, int top, int width, int height);
void DrawSnippingOverlay(ID2D1HwndRenderTarget* pRT);
void DrawShapesFlyout(ID2D1HwndRenderTarget* pRT);
void RebuildGridBrush();
void CycleCanvasBackground();
void DrawGridFlyout(ID2D1HwndRenderTarget* pRT);
void DrawBackdropFlyout(ID2D1HwndRenderTarget* pRT);
void DrawColorFlyout(ID2D1HwndRenderTarget* pRT);
void DrawLaserTrail(ID2D1HwndRenderTarget* pRT);
void DrawLaserCursor(ID2D1HwndRenderTarget* pRT);
void DrawInkingCursor(ID2D1HwndRenderTarget* pRT);

// ----------------------------------------------------------------------------
// Utility Math & Geometry
// ----------------------------------------------------------------------------

static const wchar_t* GetIconFontFamilyName() {
    static const wchar_t* s_fontName = nullptr;
    if (s_fontName) return s_fontName;

    if (g_pDWriteFactory) {
        IDWriteFontCollection* pFontCollection = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->GetSystemFontCollection(&pFontCollection, FALSE)) && pFontCollection) {
            UINT32 index = 0;
            BOOL exists = FALSE;
            if (SUCCEEDED(pFontCollection->FindFamilyName(L"Segoe Fluent Icons", &index, &exists)) && exists) {
                s_fontName = L"Segoe Fluent Icons";
            } else {
                s_fontName = L"Segoe MDL2 Assets";
            }
            pFontCollection->Release();
            return s_fontName;
        }
    }
    s_fontName = L"Segoe Fluent Icons";
    return s_fontName;
}

static float g_currentFontScale = 0.0f;
static float g_currentRadialFontScale = 0.0f;

void ReleaseTextFormats() {
    ClearTextLayoutCache();
    SafeRelease(g_pToolbarKeyFormat);
    SafeRelease(g_pMenuKeyFormat);
    SafeRelease(g_pMenuTextFormat);
    SafeRelease(g_pCenterBadgeFormat);
    SafeRelease(g_pRadialIconFormat);
    SafeRelease(g_pIconFormat);
    SafeRelease(g_pTextFormat);
    SafeRelease(g_pToastTextFormat);
    SafeRelease(g_pToastIconFormat);
    g_currentFontScale = 0.0f;
    g_currentRadialFontScale = 0.0f;
    g_currentToastFontScale = 0.0f;
}

void CreateRadialTextFormats(float scale) {
    if (!g_pDWriteFactory) return;
    if (scale <= 0.1f) scale = 1.0f;
    if (std::abs(scale - g_currentRadialFontScale) < 0.01f && g_pRadialIconFormat && g_pCenterBadgeFormat) return;

    ClearTextLayoutCache();
    if (g_pRadialIconFormat) { g_pRadialIconFormat->Release(); g_pRadialIconFormat = nullptr; }
    if (g_pCenterBadgeFormat) { g_pCenterBadgeFormat->Release(); g_pCenterBadgeFormat = nullptr; }
    g_currentRadialFontScale = scale;
    const wchar_t* iconFont = GetIconFontFamilyName();

    // Radial Menu Icon Format (18.0f Segoe Fluent Icons)
    g_pDWriteFactory->CreateTextFormat(
        iconFont,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        18.0f * scale,
        L"en-us",
        &g_pRadialIconFormat
    );
    if (g_pRadialIconFormat) {
        g_pRadialIconFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        g_pRadialIconFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    // Radial Center Badge Format (14.0f Segoe Fluent Icons)
    g_pDWriteFactory->CreateTextFormat(
        iconFont,
        NULL,
        DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        14.0f * scale,
        L"en-us",
        &g_pCenterBadgeFormat
    );
    if (g_pCenterBadgeFormat) {
        g_pCenterBadgeFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        g_pCenterBadgeFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}

void CreateTextFormats(float scale) {
    if (!g_pDWriteFactory) return;
    if (scale <= 0.1f) scale = 1.0f;
    if (std::abs(scale - g_currentFontScale) < 0.01f && g_pTextFormat) return;

    ClearTextLayoutCache();
    if (g_pToolbarKeyFormat) { g_pToolbarKeyFormat->Release(); g_pToolbarKeyFormat = nullptr; }
    if (g_pMenuKeyFormat) { g_pMenuKeyFormat->Release(); g_pMenuKeyFormat = nullptr; }
    if (g_pMenuTextFormat) { g_pMenuTextFormat->Release(); g_pMenuTextFormat = nullptr; }
    if (g_pIconFormat) { g_pIconFormat->Release(); g_pIconFormat = nullptr; }
    if (g_pTextFormat) { g_pTextFormat->Release(); g_pTextFormat = nullptr; }

    g_currentFontScale = scale;
    const wchar_t* iconFont = GetIconFontFamilyName();

    // General Text Format
    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI Variable Display",
        NULL,
        DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        13.0f * scale,
        L"en-us",
        &g_pTextFormat
    );
    if (g_pTextFormat) {
        g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    // Toolbar Icon Format (16.0f Segoe Fluent Icons)
    g_pDWriteFactory->CreateTextFormat(
        iconFont,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        16.0f * scale,
        L"en-us",
        &g_pIconFormat
    );
    if (g_pIconFormat) {
        g_pIconFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        g_pIconFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    // Shapes Modal Menu Text Format (12.5f Segoe UI Variable Display)
    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI Variable Display",
        NULL,
        DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        12.5f * scale,
        L"en-us",
        &g_pMenuTextFormat
    );
    if (g_pMenuTextFormat) {
        g_pMenuTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        g_pMenuTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    // Shapes Modal Key Hint Format (11.0f Segoe UI Variable Display)
    g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI Variable Display",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        11.0f * scale,
        L"en-us",
        &g_pMenuKeyFormat
    );
    if (g_pMenuKeyFormat) {
        g_pMenuKeyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        g_pMenuKeyFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    // Toolbar Key Badge Format (8.0f Segoe UI Variable Display)
    HRESULT hrKey = g_pDWriteFactory->CreateTextFormat(
        L"Segoe UI Variable Display",
        NULL,
        DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        8.0f * scale,
        L"en-us",
        &g_pToolbarKeyFormat
    );
    if (FAILED(hrKey)) {
        g_pDWriteFactory->CreateTextFormat(
            L"Segoe UI",
            NULL,
            DWRITE_FONT_WEIGHT_SEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            8.0f * scale,
            L"en-us",
            &g_pToolbarKeyFormat
        );
    }
    if (g_pToolbarKeyFormat) {
        g_pToolbarKeyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        g_pToolbarKeyFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    }

    if (!g_pRadialIconFormat || !g_pCenterBadgeFormat) {
        CreateRadialTextFormats(scale);
    }
}

static float DistanceSq(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}

static float DistToSegmentSq(float px, float py, float x1, float y1, float x2, float y2) {
    float l2 = DistanceSq(x1, y1, x2, y2);
    if (l2 == 0.0f) return DistanceSq(px, py, x1, y1);
    float t = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / l2;
    t = std::max(0.0f, std::min(1.0f, t));
    return DistanceSq(px, py, x1 + t * (x2 - x1), y1 + t * (y2 - y1));
}

// ----------------------------------------------------------------------------
// Desktop Screen Capture
// ----------------------------------------------------------------------------

void CaptureDesktop() {
    if (!g_settings.freezeScreen) {
        if (g_pDesktopBitmap) {
            g_pDesktopBitmap->Release();
            g_pDesktopBitmap = nullptr;
        }
        return;
    }
    if (!g_pRenderTarget) return;

    if (g_pDesktopBitmap) {
        g_pDesktopBitmap->Release();
        g_pDesktopBitmap = nullptr;
    }

    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0) return;

    bool wasVisible = (g_hOverlayWnd && IsWindowVisible(g_hOverlayWnd));
    if (wasVisible) {
        ShowWindow(g_hOverlayWnd, SW_HIDE);
    }

    UINT32 maxTexSize = g_pRenderTarget->GetMaximumBitmapSize();
    int capW = vw;
    int capH = vh;
    if (maxTexSize > 0) {
        if (capW > (int)maxTexSize) {
            capH = std::max(1, (int)((float)capH * ((float)maxTexSize / (float)capW)));
            capW = (int)maxTexSize;
        }
        if (capH > (int)maxTexSize) {
            capW = std::max(1, (int)((float)capW * ((float)maxTexSize / (float)capH)));
            capH = (int)maxTexSize;
        }
    }

    HDC hScreenDC = GetDC(NULL);
    if (!hScreenDC) {
        if (wasVisible && g_hOverlayWnd) ShowWindow(g_hOverlayWnd, SW_SHOWNOACTIVATE);
        return;
    }
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    if (!hMemDC) {
        ReleaseDC(NULL, hScreenDC);
        if (wasVisible && g_hOverlayWnd) ShowWindow(g_hOverlayWnd, SW_SHOWNOACTIVATE);
        return;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = capW;
    bmi.bmiHeader.biHeight = -capH; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hBitmap) {
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        if (wasVisible && g_hOverlayWnd) ShowWindow(g_hOverlayWnd, SW_SHOWNOACTIVATE);
        return;
    }

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    if (capW == vw && capH == vh) {
        BitBlt(hMemDC, 0, 0, vw, vh, hScreenDC, vx, vy, SRCCOPY | CAPTUREBLT);
    } else {
        SetStretchBltMode(hMemDC, HALFTONE);
        StretchBlt(hMemDC, 0, 0, capW, capH, hScreenDC, vx, vy, vw, vh, SRCCOPY | CAPTUREBLT);
    }

    if (wasVisible && g_hOverlayWnd) {
        ShowWindow(g_hOverlayWnd, SW_SHOWNOACTIVATE);
    }

    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
    );

    g_pRenderTarget->CreateBitmap(
        D2D1::SizeU(capW, capH),
        pBits,
        capW * 4,
        props,
        &g_pDesktopBitmap
    );

    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
}

// ----------------------------------------------------------------------------
// Direct2D Resource Management
// ----------------------------------------------------------------------------

HRESULT CreateD2DResources(HWND hwnd) {
    if (g_pRenderTarget) return S_OK;

    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    HRESULT hr = g_pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        ),
        D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
        &g_pRenderTarget
    );

    if (SUCCEEDED(hr)) {
        g_pRenderTarget->SetDpi(96.0f, 96.0f);
        g_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        MARGINS margins = { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        if (!g_pRoundStrokeStyle) {
            g_pD2DFactory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(
                    D2D1_CAP_STYLE_ROUND,
                    D2D1_CAP_STYLE_ROUND,
                    D2D1_CAP_STYLE_ROUND,
                    D2D1_LINE_JOIN_ROUND
                ),
                nullptr, 0,
                &g_pRoundStrokeStyle
            );
        }

        if (!g_pLaserFlatStrokeStyle) {
            g_pD2DFactory->CreateStrokeStyle(
                D2D1::StrokeStyleProperties(
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_CAP_STYLE_FLAT,
                    D2D1_LINE_JOIN_ROUND
                ),
                nullptr, 0,
                &g_pLaserFlatStrokeStyle
            );
        }

        if (!g_pStrokeBrush) {
            g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f), &g_pStrokeBrush);
        }

        if (!g_pWhiteboardBrush) {
            g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f), &g_pWhiteboardBrush);
        }
        if (!g_pBlackboardBrush) {
            g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.12f, 0.14f, 0.18f, 1.0f), &g_pBlackboardBrush);
        }

        if (g_gridStyle != GridStyle::None) {
            RebuildGridBrush();
        }

        CaptureDesktop();
    }

    return hr;
}

void ReleaseD2DResources() {
    SafeRelease(g_pRadialSatelliteArcGeom);
    g_cachedSatelliteNumOrbs = -1;
    g_cachedSatelliteScale = -1.0f;
    ClearTextLayoutCache();
    SafeRelease(g_pWhiteboardBrush);
    SafeRelease(g_pBlackboardBrush);
    SafeRelease(g_pGridBrush);
    SafeRelease(g_pStrokeBrush);
    SafeRelease(g_pDesktopBitmap);
    SafeRelease(g_pRoundStrokeStyle);
    SafeRelease(g_pLaserFlatStrokeStyle);
    SafeRelease(g_pRenderTarget);
}

void InvalidateOverlay() {
    if (g_hOverlayWnd) {
        InvalidateRect(g_hOverlayWnd, NULL, FALSE);
    }
}

void ShowToastNotification(const std::wstring& msg) {
    if (g_settings.showToastNotifications) {
        g_toastMessage = msg;
        g_toastStartTime = GetTickCount64();
        if (g_hOverlayWnd) {
            SetTimer(g_hOverlayWnd, TIMER_ID_UI_ANIMATION, 30, NULL);
        }
        InvalidateOverlay();
    }
}

void CycleCanvasBackground() {
    g_backdropFlyoutOpen = false;
    g_hoveredBackdropFlyoutItem = -1;
    if (g_canvasBg == CanvasBg::Transparent) {
        g_canvasBg = CanvasBg::Whiteboard;
        g_canvasScope = g_settings.defaultWhiteboardScope;
        ShowToastNotification(L"Whiteboard Mode (Paper)");
    } else if (g_canvasBg == CanvasBg::Whiteboard) {
        g_canvasBg = CanvasBg::Blackboard;
        ShowToastNotification(L"Blackboard Mode (Dark Slate)");
    } else {
        g_canvasBg = CanvasBg::Transparent;
        ShowToastNotification(L"Screen Mode (Transparent)");
    }
    if (g_gridStyle != GridStyle::None) {
        RebuildGridBrush();
    }
    InvalidateOverlay();
}

void PerformUndo() {
    if (!g_undoStack.empty()) {
        g_redoStack.push_back(std::move(g_strokes));
        if (g_redoStack.size() > kMaxUndoLevels) {
            g_redoStack.erase(g_redoStack.begin());
        }
        g_strokes = std::move(g_undoStack.back());
        g_undoStack.pop_back();
        InvalidateOverlay();
    }
}

void PerformRedo() {
    if (!g_redoStack.empty()) {
        g_undoStack.push_back(std::move(g_strokes));
        if (g_undoStack.size() > kMaxUndoLevels) {
            g_undoStack.erase(g_undoStack.begin());
        }
        g_strokes = std::move(g_redoStack.back());
        g_redoStack.pop_back();
        InvalidateOverlay();
    }
}

// ----------------------------------------------------------------------------
// Layout Setup: Compact Windows 11 Bottom Toolbar & Multi-Monitor Clamping
// ----------------------------------------------------------------------------

struct MonitorBounds {
    float left;
    float top;
    float right;
    float bottom;
};

static float s_cachedMonL = 0.0f, s_cachedMonT = 0.0f, s_cachedMonR = 0.0f, s_cachedMonB = 0.0f;

inline void InvalidateMonitorBoundsCache() {
    s_cachedMonL = s_cachedMonT = s_cachedMonR = s_cachedMonB = 0.0f;
}

inline void GetMonitorBoundsAt(float clientX, float clientY, float& outLeft, float& outTop, float& outRight, float& outBottom) {
    if (s_cachedMonR > s_cachedMonL && clientX >= s_cachedMonL && clientX < s_cachedMonR && clientY >= s_cachedMonT && clientY < s_cachedMonB) {
        outLeft   = s_cachedMonL;
        outTop    = s_cachedMonT;
        outRight  = s_cachedMonR;
        outBottom = s_cachedMonB;
        return;
    }

    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    POINT pt = { (LONG)std::round(clientX + (float)vx), (LONG)std::round(clientY + (float)vy) };
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (hMon && GetMonitorInfo(hMon, &mi)) {
        s_cachedMonL = (float)(mi.rcMonitor.left - vx);
        s_cachedMonT = (float)(mi.rcMonitor.top - vy);
        s_cachedMonR = (float)(mi.rcMonitor.right - vx);
        s_cachedMonB = (float)(mi.rcMonitor.bottom - vy);
    } else {
        s_cachedMonL = 0.0f;
        s_cachedMonT = 0.0f;
        s_cachedMonR = (float)GetSystemMetrics(SM_CXVIRTUALSCREEN);
        s_cachedMonB = (float)GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
    outLeft   = s_cachedMonL;
    outTop    = s_cachedMonT;
    outRight  = s_cachedMonR;
    outBottom = s_cachedMonB;
}

struct MonitorEntry {
    int id;
    std::wstring name;
    D2D1_RECT_F rect;
    bool isPrimary;
};

static std::vector<MonitorEntry> g_cachedMonitors;
inline const std::vector<MonitorEntry>& GetSystemMonitorList(bool forceRefresh = false) {
    if (!g_cachedMonitors.empty() && !forceRefresh) {
        return g_cachedMonitors;
    }
    g_cachedMonitors.clear();
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    struct EnumCtx {
        int vx;
        int vy;
        std::vector<MonitorEntry>* pList;
    } ctx;
    ctx.vx = vx;
    ctx.vy = vy;
    ctx.pList = &g_cachedMonitors;

    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMon, HDC, LPRECT lprc, LPARAM dwData) CALLBACK -> BOOL {
        auto* pCtx = reinterpret_cast<EnumCtx*>(dwData);
        if (lprc && pCtx && pCtx->pList) {
            MONITORINFO mi = { sizeof(MONITORINFO) };
            bool isPrimary = false;
            if (GetMonitorInfo(hMon, &mi)) {
                isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
            }
            int id = (int)pCtx->pList->size() + 1;
            wchar_t buf[64];
            int w = lprc->right - lprc->left;
            int h = lprc->bottom - lprc->top;
            if (isPrimary) {
                wsprintfW(buf, L"Screen %d  (Primary %dx%d)", id, w, h);
            } else {
                wsprintfW(buf, L"Screen %d  (%dx%d)", id, w, h);
            }
            MonitorEntry entry;
            entry.id = id;
            entry.name = buf;
            entry.rect = D2D1::RectF((float)(lprc->left - pCtx->vx), (float)(lprc->top - pCtx->vy),
                                    (float)(lprc->right - pCtx->vx), (float)(lprc->bottom - pCtx->vy));
            entry.isPrimary = isPrimary;
            pCtx->pList->push_back(entry);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));

    return g_cachedMonitors;
}

inline void ClampToolbarToScreen(float& x, float& y, float w, float h, float kPad = 6.0f) {
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);

    struct EnumCtx {
        int vx;
        int vy;
        std::vector<MonitorBounds> mons;
    } ctx;
    ctx.vx = vx;
    ctx.vy = vy;

    EnumDisplayMonitors(NULL, NULL, [](HMONITOR, HDC, LPRECT lprc, LPARAM dwData) CALLBACK -> BOOL {
        EnumCtx* pCtx = reinterpret_cast<EnumCtx*>(dwData);
        if (lprc && pCtx) {
            MonitorBounds mb;
            mb.left = (float)(lprc->left - pCtx->vx);
            mb.top = (float)(lprc->top - pCtx->vy);
            mb.right = (float)(lprc->right - pCtx->vx);
            mb.bottom = (float)(lprc->bottom - pCtx->vy);
            pCtx->mons.push_back(mb);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));

    if (ctx.mons.empty()) {
        int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        float maxX = std::max(kPad, (float)vw - w - kPad);
        float maxY = std::max(kPad, (float)vh - h - kPad);
        x = std::max(kPad, std::min(maxX, x));
        y = std::max(kPad, std::min(maxY, y));
        return;
    }

    // 1. Overall horizontal span across all monitors
    float minAllX = ctx.mons[0].left;
    float maxAllX = ctx.mons[0].right;
    for (const auto& m : ctx.mons) {
        if (m.left < minAllX) minAllX = m.left;
        if (m.right > maxAllX) maxAllX = m.right;
    }
    float boundMinX = minAllX + kPad;
    float boundMaxX = std::max(boundMinX, maxAllX - w - kPad);
    x = std::max(boundMinX, std::min(boundMaxX, x));

    // 2. Find all monitors that intersect the horizontal span [x, x + w]
    std::vector<MonitorBounds> intersecting;
    for (const auto& m : ctx.mons) {
        if (m.left < (x + w) && m.right > x) {
            intersecting.push_back(m);
        }
    }

    if (intersecting.empty()) {
        float centerX = x + w * 0.5f;
        float bestDist = 1e9f;
        size_t bestIdx = 0;
        for (size_t i = 0; i < ctx.mons.size(); ++i) {
            float monCenter = (ctx.mons[i].left + ctx.mons[i].right) * 0.5f;
            float d = std::abs(monCenter - centerX);
            if (d < bestDist) {
                bestDist = d;
                bestIdx = i;
            }
        }
        intersecting.push_back(ctx.mons[bestIdx]);
    }

    // 3. Constrain Y so that every part overlapping an intersecting monitor is strictly on-screen
    float allowableMinY = intersecting[0].top;
    float allowableMaxY = intersecting[0].bottom;
    for (const auto& m : intersecting) {
        if (m.top > allowableMinY) allowableMinY = m.top;
        if (m.bottom < allowableMaxY) allowableMaxY = m.bottom;
    }

    float minY = allowableMinY + kPad;
    float maxY = allowableMaxY - h - kPad;

    if (minY > maxY) {
        // Toolbar is in a transition step between monitors where vertical ranges do not overlap.
        // Snap x to the monitor containing the majority of the toolbar.
        float centerX = x + w * 0.5f;
        float bestDist = 1e9f;
        size_t bestIdx = 0;
        for (size_t i = 0; i < ctx.mons.size(); ++i) {
            float monCenter = (ctx.mons[i].left + ctx.mons[i].right) * 0.5f;
            float d = std::abs(monCenter - centerX);
            if (d < bestDist) {
                bestDist = d;
                bestIdx = i;
            }
        }
        const auto& bestMon = ctx.mons[bestIdx];
        float bMinX = bestMon.left + kPad;
        float bMaxX = std::max(bMinX, bestMon.right - w - kPad);
        x = std::max(bMinX, std::min(bMaxX, x));
        minY = bestMon.top + kPad;
        maxY = std::max(minY, bestMon.bottom - h - kPad);
    }

    y = std::max(minY, std::min(maxY, y));
}

void BuildToolbarLayout(int screenW, int screenH) {
    g_toolbarButtons.clear();
    g_toolbarDividers.clear();

    // Query monitor DPI at toolbar center or fallback
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    float checkX = (g_toolbarCustomX >= 0.0f) ? g_toolbarCustomX : (float)screenW * 0.5f;
    float checkY = (g_toolbarCustomY >= 0.0f) ? g_toolbarCustomY : (float)screenH - 50.0f;
    POINT pt = { (LONG)std::round(checkX + (float)vx), (LONG)std::round(checkY + (float)vy) };
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    float scale = GetDpiScaleForMonitor(hMon);
    if (scale <= 0.1f) scale = 1.0f;
    g_toolbarDpiScale = scale;
    CreateTextFormats(scale);

    const float pillW = 82.0f * scale;
    const float pillH = 30.0f * scale;
    g_toolbarPillWidth = pillW;

    if (g_toolbarCollapsed) {
        float startX = g_toolbarCustomX;
        float startY = g_toolbarCustomY;

        if (startX < 0.0f || startY < 0.0f) {
            HMONITOR hPrimaryMon = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO mi = { sizeof(MONITORINFO) };
            if (hPrimaryMon && GetMonitorInfo(hPrimaryMon, &mi)) {
                float clientLeft = (float)(mi.rcMonitor.left - vx);
                float clientTop = (float)(mi.rcMonitor.top - vy);
                float monW = (float)(mi.rcMonitor.right - mi.rcMonitor.left);
                float monH = (float)(mi.rcMonitor.bottom - mi.rcMonitor.top);

                if (startX < 0.0f) {
                    startX = clientLeft + (monW - pillW) * 0.5f;
                }
                if (startY < 0.0f) {
                    float workBottom = (float)(mi.rcWork.bottom - vy);
                    startY = workBottom - pillH - 16.0f * scale;
                    if (startY + pillH > clientTop + monH - 8.0f * scale) {
                        startY = clientTop + monH - pillH - 8.0f * scale;
                    }
                }
            }
            else {
                if (startX < 0.0f) startX = (screenW - pillW) * 0.5f;
                if (startY < 0.0f) startY = (screenH - pillH - 24.0f * scale);
            }
        }

        ClampToolbarToScreen(startX, startY, pillW, pillH, 6.0f * scale);
        g_toolbarCustomX = startX;
        g_toolbarCustomY = startY;

        g_toolbarRect = D2D1::RectF(startX, startY, startX + pillW, startY + pillH);

        ToolbarButton expandBtn;
        expandBtn.id = 98;
        expandBtn.isPen = false;
        expandBtn.label = L"\uE70E"; // ChevronUp
        expandBtn.shortcut = L"";
        expandBtn.rect = g_toolbarRect;
        g_toolbarButtons.push_back(expandBtn);
        return;
    }

    const float btnH = 34.0f * scale;
    const float btnW = 34.0f * scale;
    const float padY = 6.0f * scale;
    const float barH = btnH + padY * 2.0f;
    const float itemGap = 3.0f * scale;
    const float dividerGap = 10.0f * scale;
    const float gripW = 20.0f * scale;

    float curX = 6.0f * scale;

    // Handle / Dock fold indicator
    ToolbarButton dockBtn;
    dockBtn.id = 0;
    dockBtn.isPen = false;
    dockBtn.label = L"\uE75E"; // GripperTool
    dockBtn.rect = D2D1::RectF(curX, padY, curX + gripW, padY + btnH);
    dockBtn.shortcut = L"";
    g_toolbarButtons.push_back(dockBtn);
    curX += gripW + itemGap;

    // Group 1: Pens (4 Preset Swatches + 1 Custom Color Studio Button)
    for (int i = 0; i < 4; ++i) {
        ToolbarButton btn;
        btn.id = 100 + i;
        btn.isPen = true;
        btn.penColor = kPresetColors[i];
        btn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
        btn.shortcut = std::to_wstring(i + 1);
        btn.isCustomColor = false;
        g_toolbarButtons.push_back(btn);
        curX += btnW + itemGap;
    }

    // Button 5: Custom Color & Opacity Studio Button
    {
        ToolbarButton btn;
        btn.id = 104;
        btn.isPen = true;
        btn.penColor = g_customColor.activeColor;
        btn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
        btn.shortcut = L"5";
        btn.isCustomColor = true;
        g_toolbarButtons.push_back(btn);
        curX += btnW + itemGap;
    }

    // Divider 1
    curX -= itemGap;
    g_toolbarDividers.push_back(curX + dividerGap * 0.5f);
    curX += dividerGap;

    // Group 2: Freehand & Shapes Flyout
    ToolbarButton freehandBtn;
    freehandBtn.id = 20; // Freehand
    freehandBtn.isPen = false;
    freehandBtn.label = L"\uEC87"; // Freehand (Draw)
    freehandBtn.shortcut = L"F";
    freehandBtn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
    g_toolbarButtons.push_back(freehandBtn);
    curX += btnW + itemGap;

    ToolbarButton shapesBtn;
    shapesBtn.id = 25; // Shapes Flyout (Line, Arrow, Rectangle, Ellipse, Triangle)
    shapesBtn.isPen = false;
    switch (g_currentShape) {
        case ShapeType::Line:      shapesBtn.label = L"\uED5E"; shapesBtn.shortcut = L"L"; break;
        case ShapeType::Arrow:     shapesBtn.label = L"\uE72A"; shapesBtn.shortcut = L"A"; break;
        case ShapeType::Rectangle: shapesBtn.label = L"\uE739"; shapesBtn.shortcut = L"R"; break;
        case ShapeType::Ellipse:   shapesBtn.label = L"\uEA3A"; shapesBtn.shortcut = L"O"; break;
        case ShapeType::Triangle:  shapesBtn.label = L"\u25B2"; shapesBtn.shortcut = L"T"; break;
        default:                   shapesBtn.label = L"\uF158"; shapesBtn.shortcut = L"L"; break;
    }
    shapesBtn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
    g_toolbarButtons.push_back(shapesBtn);
    curX += btnW + itemGap;

    // Divider 2
    curX -= itemGap;
    g_toolbarDividers.push_back(curX + dividerGap * 0.5f);
    curX += dividerGap;

    // Group 3: Navigation & Presentation Tools (Highlighter, Laser, Eraser, Pan, Pointer, Eye, Grid, Whiteboard)
    int toolIds[] = { 1, 7, 2, 3, 4, 5, 6, 8 };
    const wchar_t* toolLabels[] = {
        L"\uE7E6", // Highlighter (Highlight)
        L"\uE814", // Laser Pointer
        L"\uE75C", // Eraser (EraseTool)
        L"\uE7C2", // Pan (Move - 4-way arrows)
        L"\uE7C9", // Pointer (TouchPointer)
        L"\uE890", // Eye (View)
        L"#",      // Grid (drawn as vector grid icon)
        L"\uEE56"  // Whiteboard / Blackboard Mode
    };
    const wchar_t* toolShortcuts[] = {
        L"H",
        L"D",
        L"E",
        L"P",
        L"M",
        L"V",
        L"G",
        L"K"
    };
    for (int i = 0; i < 8; ++i) {
        ToolbarButton btn;
        btn.id = toolIds[i];
        btn.isPen = false;
        btn.label = toolLabels[i];
        btn.shortcut = toolShortcuts[i];
        btn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
        g_toolbarButtons.push_back(btn);
        curX += btnW + itemGap;
    }

    // Divider 3
    curX -= itemGap;
    g_toolbarDividers.push_back(curX + dividerGap * 0.5f);
    curX += dividerGap;

    // Group 4: Capture & Edit Actions (Snip, Snapshot, Undo, Redo, Clear)
    int actionIds[] = { 9, 10, 11, 12, 13 };
    const wchar_t* actionLabels[] = {
        L"\uF407", // Snip (SnippingTool)
        L"\uE722", // Snapshot (Camera)
        L"\uE7A7", // Undo
        L"\uE7A6", // Redo
        L"\uE74D"  // Clear (Delete)
    };
    const wchar_t* actionShortcuts[] = {
        L"S",
        L"^S",
        L"Z",
        L"Y",
        L"C"
    };
    for (int i = 0; i < 5; ++i) {
        ToolbarButton btn;
        btn.id = actionIds[i];
        btn.isPen = false;
        btn.label = actionLabels[i];
        btn.shortcut = actionShortcuts[i];
        btn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
        g_toolbarButtons.push_back(btn);
        curX += btnW + itemGap;
    }

    // Divider 4
    curX -= itemGap;
    g_toolbarDividers.push_back(curX + dividerGap * 0.5f);
    curX += dividerGap;

    // Group 5: Minimize / Collapse & Exit
    ToolbarButton minBtn;
    minBtn.id = 98;
    minBtn.isPen = false;
    minBtn.label = L"\uE740"; // ChevronDown
    minBtn.shortcut = L"B";
    minBtn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
    g_toolbarButtons.push_back(minBtn);
    curX += btnW + itemGap;

    ToolbarButton exitBtn;
    exitBtn.id = 99;
    exitBtn.isPen = false;
    exitBtn.label = L"\uE8BB"; // Exit (ChromeClose)
    exitBtn.shortcut = L"Esc";
    exitBtn.rect = D2D1::RectF(curX, padY, curX + btnW, padY + btnH);
    g_toolbarButtons.push_back(exitBtn);
    curX += btnW + 6.0f * scale;

    float barW = curX;
    g_toolbarExpandedWidth = barW;

    // Default placement: centered at the bottom of the Primary / Main Screen
    float startX = g_toolbarCustomX;
    float startY = g_toolbarCustomY;

    if (startX < 0.0f || startY < 0.0f) {
        HMONITOR hPrimaryMon = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = { sizeof(MONITORINFO) };
        if (hPrimaryMon && GetMonitorInfo(hPrimaryMon, &mi)) {
            float clientLeft = (float)(mi.rcMonitor.left - vx);
            float clientTop = (float)(mi.rcMonitor.top - vy);
            float monW = (float)(mi.rcMonitor.right - mi.rcMonitor.left);
            float monH = (float)(mi.rcMonitor.bottom - mi.rcMonitor.top);

            if (startX < 0.0f) {
                startX = clientLeft + (monW - barW) * 0.5f;
            }
            if (startY < 0.0f) {
                float workBottom = (float)(mi.rcWork.bottom - vy);
                startY = workBottom - barH - 16.0f * scale;
                if (startY + barH > clientTop + monH - 8.0f * scale) {
                    startY = clientTop + monH - barH - 8.0f * scale;
                }
            }
        }
        else {
            if (startX < 0.0f) startX = (screenW - barW) * 0.5f;
            if (startY < 0.0f) startY = (screenH - barH - 24.0f * scale);
        }
    }

    ClampToolbarToScreen(startX, startY, barW, barH, 6.0f * scale);
    g_toolbarCustomX = startX;
    g_toolbarCustomY = startY;

    g_toolbarRect = D2D1::RectF(startX, startY, startX + barW, startY + barH);

    // Position buttons to absolute coordinates
    for (auto& btn : g_toolbarButtons) {
        btn.rect.left += startX;
        btn.rect.right += startX;
        btn.rect.top += startY;
        btn.rect.bottom += startY;
    }
    for (auto& divX : g_toolbarDividers) {
        divX += startX;
    }
}

// ----------------------------------------------------------------------------
// Direct2D Stroke & Shape Rendering
// ----------------------------------------------------------------------------

void DrawArrowhead(ID2D1HwndRenderTarget* pRT, ID2D1SolidColorBrush* pBrush, float x1, float y1, float x2, float y2, float strokeW) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 4.0f) return;

    float ux = dx / len;
    float uy = dy / len;
    float arrowLen = std::max(12.0f, strokeW * 3.5f);
    float arrowW = arrowLen * 0.55f;

    if (len < arrowLen * 1.2f) {
        arrowLen = len * 0.6f;
        arrowW = arrowLen * 0.55f;
    }

    float basePx = x2 - ux * arrowLen;
    float basePy = y2 - uy * arrowLen;

    float leftX = basePx - uy * arrowW;
    float leftY = basePy + ux * arrowW;
    float rightX = basePx + uy * arrowW;
    float rightY = basePy - ux * arrowW;

    ID2D1PathGeometry* pArrowGeo = nullptr;
    if (g_pD2DFactory && SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pArrowGeo))) {
        ID2D1GeometrySink* pSink = nullptr;
        if (SUCCEEDED(pArrowGeo->Open(&pSink))) {
            pSink->BeginFigure(D2D1::Point2F(x2, y2), D2D1_FIGURE_BEGIN_FILLED);
            pSink->AddLine(D2D1::Point2F(leftX, leftY));
            pSink->AddLine(D2D1::Point2F(rightX, rightY));
            pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
            pSink->Close();
            SafeRelease(pSink);

            pRT->FillGeometry(pArrowGeo, pBrush);
        }
        SafeRelease(pArrowGeo);
    }
}

void BuildStrokeGeometry(Stroke& stroke) {
    if (stroke.pCachedGeometry) return;
    if (!g_pD2DFactory) return;

    if (stroke.shapeType == ShapeType::Freehand) {
        if (stroke.points.size() <= 1) return;

        ID2D1PathGeometry* pGeometry = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pGeometry))) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pGeometry->Open(&pSink))) {
                pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
                pSink->BeginFigure(
                    D2D1::Point2F(stroke.points[0].x, stroke.points[0].y),
                    D2D1_FIGURE_BEGIN_HOLLOW
                );

                if (stroke.points.size() == 2) {
                    pSink->AddLine(D2D1::Point2F(stroke.points[1].x, stroke.points[1].y));
                }
                else {
                    for (size_t i = 1; i < stroke.points.size() - 1; ++i) {
                        D2D1_POINT_2F midPoint = D2D1::Point2F(
                            (stroke.points[i].x + stroke.points[i + 1].x) * 0.5f,
                            (stroke.points[i].y + stroke.points[i + 1].y) * 0.5f
                        );
                        pSink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
                            D2D1::Point2F(stroke.points[i].x, stroke.points[i].y),
                            midPoint
                        ));
                    }
                    pSink->AddLine(D2D1::Point2F(stroke.points.back().x, stroke.points.back().y));
                }

                pSink->EndFigure(D2D1_FIGURE_END_OPEN);
                pSink->Close();
                SafeRelease(pSink);

                stroke.pCachedGeometry = pGeometry;
            }
            else {
                SafeRelease(pGeometry);
            }
        }
    }
    else if (stroke.shapeType == ShapeType::Arrow) {
        float dx = stroke.endPt.x - stroke.startPt.x;
        float dy = stroke.endPt.y - stroke.startPt.y;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 1.0f) return;

        float ux = dx / len;
        float uy = dy / len;
        float arrowLen = std::max(12.0f, stroke.width * 3.5f);
        float arrowW = arrowLen * 0.55f;

        if (len < arrowLen * 1.2f) {
            arrowLen = len * 0.6f;
            arrowW = arrowLen * 0.55f;
        }

        float basePx = stroke.endPt.x - ux * arrowLen;
        float basePy = stroke.endPt.y - uy * arrowLen;

        float leftX = basePx - uy * arrowW;
        float leftY = basePy + ux * arrowW;
        float rightX = basePx + uy * arrowW;
        float rightY = basePy - ux * arrowW;

        ID2D1PathGeometry* pArrowGeo = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pArrowGeo))) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pArrowGeo->Open(&pSink))) {
                pSink->BeginFigure(D2D1::Point2F(stroke.endPt.x, stroke.endPt.y), D2D1_FIGURE_BEGIN_FILLED);
                pSink->AddLine(D2D1::Point2F(leftX, leftY));
                pSink->AddLine(D2D1::Point2F(rightX, rightY));
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                pSink->Close();
                SafeRelease(pSink);

                stroke.pCachedGeometry = pArrowGeo;
            }
            else {
                SafeRelease(pArrowGeo);
            }
        }
    }
    else if (stroke.shapeType == ShapeType::Triangle) {
        float minX = std::min(stroke.startPt.x, stroke.endPt.x);
        float maxX = std::max(stroke.startPt.x, stroke.endPt.x);
        float minY = std::min(stroke.startPt.y, stroke.endPt.y);
        float maxY = std::max(stroke.startPt.y, stroke.endPt.y);
        float midX = (minX + maxX) * 0.5f;

        ID2D1PathGeometry* pGeo = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pGeo))) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pGeo->Open(&pSink))) {
                pSink->BeginFigure(D2D1::Point2F(midX, minY), D2D1_FIGURE_BEGIN_HOLLOW);
                pSink->AddLine(D2D1::Point2F(minX, maxY));
                pSink->AddLine(D2D1::Point2F(maxX, maxY));
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                pSink->Close();
                SafeRelease(pSink);

                stroke.pCachedGeometry = pGeo;
            }
            else {
                SafeRelease(pGeo);
            }
        }
    }
}

void DrawSmoothStroke(ID2D1HwndRenderTarget* pRT, Stroke& stroke) {
    if (!g_inkVisible || !g_pStrokeBrush) return;

    D2D1_COLOR_F c = stroke.color;
    if (stroke.isHighlighter) {
        c.a = 0.35f;
    }
    g_pStrokeBrush->SetColor(c);

    if (stroke.shapeType == ShapeType::Line) {
        pRT->DrawLine(
            D2D1::Point2F(stroke.startPt.x, stroke.startPt.y),
            D2D1::Point2F(stroke.endPt.x, stroke.endPt.y),
            g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle
        );
    }
    else if (stroke.shapeType == ShapeType::Arrow) {
        float dx = stroke.endPt.x - stroke.startPt.x;
        float dy = stroke.endPt.y - stroke.startPt.y;
        float len = std::sqrt(dx * dx + dy * dy);
        float arrowLen = std::max(12.0f, stroke.width * 3.5f);
        if (len < arrowLen * 1.2f && len > 0.001f) {
            arrowLen = len * 0.6f;
        }
        float ux = (len > 0.001f) ? (dx / len) : 0.0f;
        float uy = (len > 0.001f) ? (dy / len) : 0.0f;

        // Line shaft stops inside the arrowhead base so the round cap never protrudes past the sharp tip!
        float shaftEndX = stroke.endPt.x - ux * (arrowLen * 0.75f);
        float shaftEndY = stroke.endPt.y - uy * (arrowLen * 0.75f);

        pRT->DrawLine(
            D2D1::Point2F(stroke.startPt.x, stroke.startPt.y),
            D2D1::Point2F(shaftEndX, shaftEndY),
            g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle
        );
        if (stroke.pCachedGeometry) {
            pRT->FillGeometry(stroke.pCachedGeometry, g_pStrokeBrush);
        }
        else {
            if (&stroke != &g_currentStroke) {
                BuildStrokeGeometry(stroke);
                if (stroke.pCachedGeometry) {
                    pRT->FillGeometry(stroke.pCachedGeometry, g_pStrokeBrush);
                }
            }
            else {
                DrawArrowhead(pRT, g_pStrokeBrush, stroke.startPt.x, stroke.startPt.y, stroke.endPt.x, stroke.endPt.y, stroke.width);
            }
        }
    }
    else if (stroke.shapeType == ShapeType::Rectangle) {
        float minX = std::min(stroke.startPt.x, stroke.endPt.x);
        float maxX = std::max(stroke.startPt.x, stroke.endPt.x);
        float minY = std::min(stroke.startPt.y, stroke.endPt.y);
        float maxY = std::max(stroke.startPt.y, stroke.endPt.y);
        pRT->DrawRectangle(D2D1::RectF(minX, minY, maxX, maxY), g_pStrokeBrush, stroke.width);
    }
    else if (stroke.shapeType == ShapeType::Ellipse) {
        float cx = (stroke.startPt.x + stroke.endPt.x) * 0.5f;
        float cy = (stroke.startPt.y + stroke.endPt.y) * 0.5f;
        float rx = std::abs(stroke.endPt.x - stroke.startPt.x) * 0.5f;
        float ry = std::abs(stroke.endPt.y - stroke.startPt.y) * 0.5f;
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry), g_pStrokeBrush, stroke.width);
    }
    else if (stroke.shapeType == ShapeType::Triangle) {
        if (stroke.pCachedGeometry) {
            pRT->DrawGeometry(stroke.pCachedGeometry, g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
        }
        else {
            if (&stroke != &g_currentStroke) {
                BuildStrokeGeometry(stroke);
                if (stroke.pCachedGeometry) {
                    pRT->DrawGeometry(stroke.pCachedGeometry, g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
                }
            }
            else {
                float minX = std::min(stroke.startPt.x, stroke.endPt.x);
                float maxX = std::max(stroke.startPt.x, stroke.endPt.x);
                float minY = std::min(stroke.startPt.y, stroke.endPt.y);
                float maxY = std::max(stroke.startPt.y, stroke.endPt.y);
                float midX = (minX + maxX) * 0.5f;

                pRT->DrawLine(D2D1::Point2F(midX, minY), D2D1::Point2F(minX, maxY), g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
                pRT->DrawLine(D2D1::Point2F(minX, maxY), D2D1::Point2F(maxX, maxY), g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
                pRT->DrawLine(D2D1::Point2F(maxX, maxY), D2D1::Point2F(midX, minY), g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
            }
        }
    }
    else {
        // Freehand Inking with Quadratic Bézier smoothing
        if (stroke.points.empty()) {
            return;
        }

        if (stroke.points.size() == 1) {
            float r = stroke.width * 0.5f;
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(stroke.points[0].x, stroke.points[0].y), r, r), g_pStrokeBrush);
        }
        else {
            if (stroke.pCachedGeometry) {
                pRT->DrawGeometry(stroke.pCachedGeometry, g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
            }
            else {
                if (&stroke != &g_currentStroke) {
                    BuildStrokeGeometry(stroke);
                    if (stroke.pCachedGeometry) {
                        pRT->DrawGeometry(stroke.pCachedGeometry, g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
                    }
                }
                else {
                    // Active in-progress stroke being actively drawn:
                    ID2D1PathGeometry* pGeometry = nullptr;
                    if (g_pD2DFactory && SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pGeometry))) {
                        ID2D1GeometrySink* pSink = nullptr;
                        if (SUCCEEDED(pGeometry->Open(&pSink))) {
                            pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
                            pSink->BeginFigure(
                                D2D1::Point2F(stroke.points[0].x, stroke.points[0].y),
                                D2D1_FIGURE_BEGIN_HOLLOW
                            );

                            if (stroke.points.size() == 2) {
                                pSink->AddLine(D2D1::Point2F(stroke.points[1].x, stroke.points[1].y));
                            }
                            else {
                                for (size_t i = 1; i < stroke.points.size() - 1; ++i) {
                                    D2D1_POINT_2F midPoint = D2D1::Point2F(
                                        (stroke.points[i].x + stroke.points[i + 1].x) * 0.5f,
                                        (stroke.points[i].y + stroke.points[i + 1].y) * 0.5f
                                    );
                                    pSink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
                                        D2D1::Point2F(stroke.points[i].x, stroke.points[i].y),
                                        midPoint
                                    ));
                                }
                                pSink->AddLine(D2D1::Point2F(stroke.points.back().x, stroke.points.back().y));
                            }

                            pSink->EndFigure(D2D1_FIGURE_END_OPEN);
                            pSink->Close();
                            SafeRelease(pSink);

                            pRT->DrawGeometry(pGeometry, g_pStrokeBrush, stroke.width, g_pRoundStrokeStyle);
                        }
                        SafeRelease(pGeometry);
                    }
                }
            }
        }
    }
}

void DrawToolbar(ID2D1HwndRenderTarget* pRT) {
    if (!g_settings.showBottomToolbar) return;

    ID2D1SolidColorBrush* pBgBrush = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pRimBrush = nullptr;
    ID2D1SolidColorBrush* pTextBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.08f, 0.10f, 0.14f, 0.94f), &pBgBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.26f, 0.34f, 1.00f), &pBorderBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.48f, 0.60f, 0.50f), &pRimBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.88f, 0.92f, 1.00f), &pTextBrush);

    if (g_toolbarCollapsed) {
        float scale = g_toolbarDpiScale;
        bool isPillHovered = (g_hoveredToolbarBtn == 0 ||
                              (g_cursorX >= g_toolbarRect.left && g_cursorX <= g_toolbarRect.right &&
                               g_cursorY >= g_toolbarRect.top && g_cursorY <= g_toolbarRect.bottom));

        float pillR = (float)g_settings.cornerRadius * scale;
        D2D1_ROUNDED_RECT pillRoundRect = D2D1::RoundedRect(g_toolbarRect, pillR, pillR);

        // Fill frosted dark acrylic
        pRT->FillRoundedRectangle(pillRoundRect, pBgBrush);

        // Border (mint glow if hovered, sleek subtle border otherwise)
        ID2D1SolidColorBrush* pMintGlow = nullptr;
        if (isPillHovered) {
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.32f, 0.85f, 0.69f, 1.0f), &pMintGlow);
            pRT->DrawRoundedRectangle(pillRoundRect, pMintGlow, 1.8f);
        } else {
            pRT->DrawRoundedRectangle(pillRoundRect, pBorderBrush, 1.2f);
        }

        // Specular top rim highlight
        if (pRimBrush) {
            pRT->DrawLine(
                D2D1::Point2F(g_toolbarRect.left + pillR + 2.0f * scale, g_toolbarRect.top + 1.2f),
                D2D1::Point2F(g_toolbarRect.right - pillR - 2.0f * scale, g_toolbarRect.top + 1.2f),
                pRimBrush, 1.0f
            );
        }

        float midY = (g_toolbarRect.top + g_toolbarRect.bottom) * 0.5f;

        // 1. Left: Gripper icon (GripperTool \uE75E)
        if (g_pIconFormat && pTextBrush) {
            D2D1_RECT_F gripR = D2D1::RectF(g_toolbarRect.left + 6.0f * scale, g_toolbarRect.top, g_toolbarRect.left + 26.0f * scale, g_toolbarRect.bottom);
            DrawCachedText(pRT, L"\uE75E", g_pIconFormat, gripR, pTextBrush);
        }

        // 2. Middle: Active Tool / Color Swatch Dot (9px diameter)
        float dotX = (g_toolbarRect.left + g_toolbarRect.right) * 0.5f - 2.0f * scale;
        float dotR = 4.5f * scale;
        if (g_activeColor.a < 0.99f) {
            ID2D1SolidColorBrush* pDotBack = nullptr;
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.18f, 0.22f, 0.28f, 1.0f), &pDotBack);
            if (pDotBack) {
                pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, midY), dotR, dotR), pDotBack);
                SafeRelease(pDotBack);
            }
        }
        ID2D1SolidColorBrush* pDotBrush = nullptr;
        pRT->CreateSolidColorBrush(g_activeColor, &pDotBrush);
        if (pDotBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, midY), dotR, dotR), pDotBrush);
            pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, midY), dotR, dotR), isPillHovered && pMintGlow ? pMintGlow : pBorderBrush, 1.0f);
            SafeRelease(pDotBrush);
        }

        // 3. Right: Expand chevron glyph (\uE70E ChevronUp)
        if (g_pIconFormat && pTextBrush) {
            D2D1_RECT_F chevR = D2D1::RectF(g_toolbarRect.right - 28.0f * scale, g_toolbarRect.top, g_toolbarRect.right - 8.0f * scale, g_toolbarRect.bottom);
            DrawCachedText(pRT, L"\uE70E", g_pIconFormat, chevR, isPillHovered && pMintGlow ? pMintGlow : pTextBrush);
        }

        SafeRelease(pMintGlow);
        SafeRelease(pBgBrush);
        SafeRelease(pBorderBrush);
        SafeRelease(pRimBrush);
        SafeRelease(pTextBrush);
        return;
    }

    float scale = g_toolbarDpiScale;
    float cornerR = (float)g_settings.cornerRadius * scale;
    D2D1_ROUNDED_RECT roundRect = D2D1::RoundedRect(g_toolbarRect, cornerR, cornerR);

    // Chassis background & 1px border
    pRT->FillRoundedRectangle(roundRect, pBgBrush);
    pRT->DrawRoundedRectangle(roundRect, pBorderBrush, 1.2f);

    // Specular top rim highlight
    if (pRimBrush) {
        pRT->DrawLine(
            D2D1::Point2F(g_toolbarRect.left + cornerR + 2.0f * scale, g_toolbarRect.top + 1.2f),
            D2D1::Point2F(g_toolbarRect.right - cornerR - 2.0f * scale, g_toolbarRect.top + 1.2f),
            pRimBrush, 1.0f
        );
    }

    // Subtle vertical dividers
    float divY1 = g_toolbarRect.top + 7.0f * scale;
    float divY2 = g_toolbarRect.bottom - 7.0f * scale;
    for (float divX : g_toolbarDividers) {
        pRT->DrawLine(D2D1::Point2F(divX, divY1), D2D1::Point2F(divX, divY2), pBorderBrush, 1.0f);
    }

    // Draw buttons
    for (size_t i = 0; i < g_toolbarButtons.size(); ++i) {
        const auto& btn = g_toolbarButtons[i];
        bool isHovered = (g_hoveredToolbarBtn == (int)i);

        if (btn.isPen) {
            // Pen swatch
            D2D1_ROUNDED_RECT penR = D2D1::RoundedRect(btn.rect, 4.0f * scale, 4.0f * scale);

            // If custom color button and alpha < 1.0, draw subtle checkerboard under fill
            if (btn.isCustomColor && btn.penColor.a < 0.99f) {
                ID2D1SolidColorBrush* pCheckDark = nullptr;
                pRT->CreateSolidColorBrush(D2D1::ColorF(0.18f, 0.22f, 0.28f, 1.0f), &pCheckDark);
                if (pCheckDark) {
                    pRT->FillRoundedRectangle(penR, pCheckDark);
                    SafeRelease(pCheckDark);
                }
            }

            ID2D1SolidColorBrush* pPenBrush = nullptr;
            pRT->CreateSolidColorBrush(btn.penColor, &pPenBrush);
            if (pPenBrush) {
                pRT->FillRoundedRectangle(penR, pPenBrush);

                bool isActivePen = (g_currentTool == ToolMode::Pen &&
                                    btn.penColor.r == g_activeColor.r &&
                                    btn.penColor.g == g_activeColor.g &&
                                    btn.penColor.b == g_activeColor.b &&
                                    (!btn.isCustomColor || std::abs(btn.penColor.a - g_activeColor.a) < 0.03f));
                if (btn.isCustomColor && g_colorFlyoutOpen) {
                    isActivePen = true;
                }

                ID2D1SolidColorBrush* pPenBorder = nullptr;
                if (isActivePen) {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.32f, 0.85f, 0.69f, 1.0f), &pPenBorder);
                    pRT->DrawRoundedRectangle(penR, pPenBorder, 2.5f);
                }
                else if (isHovered) {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.8f), &pPenBorder);
                    pRT->DrawRoundedRectangle(penR, pPenBorder, 1.5f);
                }
                else {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.35f), &pPenBorder);
                    pRT->DrawRoundedRectangle(penR, pPenBorder, 1.0f);
                }
                SafeRelease(pPenBorder);
                SafeRelease(pPenBrush);
            }

            // 5th button: Custom color button shows a crisp plus icon to indicate it opens custom color picker
            if (btn.isCustomColor) {
                float cx = (btn.rect.left + btn.rect.right) * 0.5f;
                float cy = (btn.rect.top + btn.rect.bottom) * 0.5f;
                const float pLen = 5.0f * scale;

                // Contrast shadow behind plus icon
                ID2D1SolidColorBrush* pPlusShadow = nullptr;
                pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.70f), &pPlusShadow);
                if (pPlusShadow) {
                    pRT->DrawLine(D2D1::Point2F(cx - pLen, cy + 0.8f), D2D1::Point2F(cx + pLen, cy + 0.8f), pPlusShadow, 2.8f, g_pRoundStrokeStyle);
                    pRT->DrawLine(D2D1::Point2F(cx, cy - pLen + 0.8f), D2D1::Point2F(cx, cy + pLen + 0.8f), pPlusShadow, 2.8f, g_pRoundStrokeStyle);
                    SafeRelease(pPlusShadow);
                }

                // Crisp white plus icon
                ID2D1SolidColorBrush* pPlusWhite = nullptr;
                pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &pPlusWhite);
                if (pPlusWhite) {
                    pRT->DrawLine(D2D1::Point2F(cx - pLen, cy), D2D1::Point2F(cx + pLen, cy), pPlusWhite, 2.0f, g_pRoundStrokeStyle);
                    pRT->DrawLine(D2D1::Point2F(cx, cy - pLen), D2D1::Point2F(cx, cy + pLen), pPlusWhite, 2.0f, g_pRoundStrokeStyle);
                    SafeRelease(pPlusWhite);
                }
            }

            // Draw really small shortcut badge on pen swatch (1..5)
            if (g_pToolbarKeyFormat && !btn.shortcut.empty()) {
                float pillW = 9.0f * scale;
                float pillH = 10.0f * scale;
                float pillR = btn.rect.right - 2.5f * scale;
                float pillT = btn.rect.top + 2.0f * scale;
                D2D1_ROUNDED_RECT badgePill = D2D1::RoundedRect(
                    D2D1::RectF(pillR - pillW, pillT, pillR, pillT + pillH),
                    2.0f * scale, 2.0f * scale
                );
                ID2D1SolidColorBrush* pBadgeBacking = nullptr;
                pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f), &pBadgeBacking);
                if (pBadgeBacking) {
                    pRT->FillRoundedRectangle(badgePill, pBadgeBacking);
                    SafeRelease(pBadgeBacking);
                }

                ID2D1SolidColorBrush* pBadgeText = nullptr;
                pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &pBadgeText);
                if (pBadgeText) {
                    D2D1_RECT_F textRect = D2D1::RectF(pillR - pillW, pillT, pillR - 1.5f * scale, pillT + pillH);
                    DrawCachedText(pRT, btn.shortcut, g_pToolbarKeyFormat, textRect, pBadgeText);
                    SafeRelease(pBadgeText);
                }
            }
        }
        else {
            bool isDisabled = (btn.id == 11 && g_undoStack.empty()) ||
                              (btn.id == 12 && g_redoStack.empty()) ||
                              (btn.id == 13 && g_strokes.empty());

            // Determine active highlight
            bool isToolActive = false;
            if (btn.id == 1 && g_currentTool == ToolMode::Highlighter) isToolActive = true;
            if (btn.id == 7 && g_currentTool == ToolMode::Laser) isToolActive = true;
            if (btn.id == 2 && g_currentTool == ToolMode::Eraser) isToolActive = true;
            if (btn.id == 3 && g_currentTool == ToolMode::Pan) isToolActive = true;
            if (btn.id == 4 && g_currentTool == ToolMode::Pointer) isToolActive = true;
            if (btn.id == 5 && !g_inkVisible) isToolActive = true; // Eye closed
            if (btn.id == 20 && g_currentShape == ShapeType::Freehand && g_currentTool == ToolMode::Pen) isToolActive = true;
            if (btn.id == 25 && (g_shapesFlyoutOpen || (g_currentShape != ShapeType::Freehand && g_currentTool == ToolMode::Pen))) isToolActive = true;
            if (btn.id == 6 && (g_gridFlyoutOpen || g_gridStyle != GridStyle::None)) isToolActive = true;
            if (btn.id == 8 && (g_backdropFlyoutOpen || g_canvasBg != CanvasBg::Transparent)) isToolActive = true;

            if (!isDisabled && (isToolActive || isHovered)) {
                ID2D1SolidColorBrush* pHoverBg = nullptr;
                pRT->CreateSolidColorBrush(isToolActive ? D2D1::ColorF(0.20f, 0.32f, 0.44f, 0.95f) : D2D1::ColorF(0.18f, 0.22f, 0.30f, 0.85f), &pHoverBg);
                if (pHoverBg) {
                    pRT->FillRoundedRectangle(D2D1::RoundedRect(btn.rect, 4.0f * scale, 4.0f * scale), pHoverBg);
                    SafeRelease(pHoverBg);
                }
            }

            // Draw label / icon
            if (g_pIconFormat && !btn.label.empty()) {
                ID2D1SolidColorBrush* pLblBrush = nullptr;
                if (isDisabled) {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.44f, 0.52f, 0.38f), &pLblBrush);
                }
                else if (isToolActive) {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.90f, 0.75f, 1.0f), &pLblBrush);
                }
                ID2D1SolidColorBrush* pDrawBrush = pLblBrush ? pLblBrush : pTextBrush;
                if (btn.id == 25 && g_currentShape == ShapeType::Triangle) {
                    float cx = (btn.rect.left + btn.rect.right) * 0.5f;
                    float cy = (btn.rect.top + btn.rect.bottom) * 0.5f - 1.0f;
                    float triH = 12.0f * scale;
                    float triW = 13.0f * scale;
                    D2D1_POINT_2F p1 = D2D1::Point2F(cx, cy - triH * 0.5f);
                    D2D1_POINT_2F p2 = D2D1::Point2F(cx - triW * 0.5f, cy + triH * 0.5f);
                    D2D1_POINT_2F p3 = D2D1::Point2F(cx + triW * 0.5f, cy + triH * 0.5f);
                    if (pDrawBrush) {
                        pRT->DrawLine(p1, p2, pDrawBrush, 1.4f, g_pRoundStrokeStyle);
                        pRT->DrawLine(p2, p3, pDrawBrush, 1.4f, g_pRoundStrokeStyle);
                        pRT->DrawLine(p3, p1, pDrawBrush, 1.4f, g_pRoundStrokeStyle);
                    }
                }
                else if (btn.id == 6) {
                    float cx = (btn.rect.left + btn.rect.right) * 0.5f;
                    float cy = (btn.rect.top + btn.rect.bottom) * 0.5f - 1.0f;
                    float half = 6.0f * scale;
                    float off = 2.4f * scale;
                    if (pDrawBrush) {
                        pRT->DrawLine(D2D1::Point2F(cx - half, cy - off), D2D1::Point2F(cx + half, cy - off), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
                        pRT->DrawLine(D2D1::Point2F(cx - half, cy + off), D2D1::Point2F(cx + half, cy + off), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
                        pRT->DrawLine(D2D1::Point2F(cx - off, cy - half), D2D1::Point2F(cx - off, cy + half), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
                        pRT->DrawLine(D2D1::Point2F(cx + off, cy - half), D2D1::Point2F(cx + off, cy + half), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
                    }
                }
                else if (btn.id == 7) {
                    // Sleek vector laser beacon: central dot + optic ring + 4 cross ticks
                    float cx = (btn.rect.left + btn.rect.right) * 0.5f;
                    float cy = (btn.rect.top + btn.rect.bottom) * 0.5f;
                    if (pDrawBrush) {
                        D2D1_ELLIPSE coreEll = D2D1::Ellipse(D2D1::Point2F(cx, cy), 2.2f * scale, 2.2f * scale);
                        pRT->FillEllipse(coreEll, pDrawBrush);
                        D2D1_ELLIPSE ringEll = D2D1::Ellipse(D2D1::Point2F(cx, cy), 5.5f * scale, 5.5f * scale);
                        pRT->DrawEllipse(ringEll, pDrawBrush, 1.2f);
                        pRT->DrawLine(D2D1::Point2F(cx - 8.0f * scale, cy), D2D1::Point2F(cx - 5.5f * scale, cy), pDrawBrush, 1.2f);
                        pRT->DrawLine(D2D1::Point2F(cx + 5.5f * scale, cy), D2D1::Point2F(cx + 8.0f * scale, cy), pDrawBrush, 1.2f);
                        pRT->DrawLine(D2D1::Point2F(cx, cy - 8.0f * scale), D2D1::Point2F(cx, cy - 5.5f * scale), pDrawBrush, 1.2f);
                        pRT->DrawLine(D2D1::Point2F(cx, cy + 5.5f * scale), D2D1::Point2F(cx, cy + 8.0f * scale), pDrawBrush, 1.2f);
                    }
                }
                else if (btn.id == 8) {
                    // Sleek vector Whiteboard / Blackboard easel icon
                    float cx = (btn.rect.left + btn.rect.right) * 0.5f;
                    float cy = (btn.rect.top + btn.rect.bottom) * 0.5f - 1.0f;
                    D2D1_RECT_F boardR = D2D1::RectF(cx - 7.5f * scale, cy - 6.0f * scale, cx + 7.5f * scale, cy + 3.5f * scale);
                    if (pDrawBrush) {
                        // Board frame
                        pRT->DrawRoundedRectangle(D2D1::RoundedRect(boardR, 1.5f * scale, 1.5f * scale), pDrawBrush, 1.2f);
                        // Bottom tray
                        pRT->DrawLine(D2D1::Point2F(cx - 8.5f * scale, cy + 4.5f * scale), D2D1::Point2F(cx + 8.5f * scale, cy + 4.5f * scale), pDrawBrush, 1.2f);
                        // Easel legs
                        pRT->DrawLine(D2D1::Point2F(cx - 5.0f * scale, cy + 5.0f * scale), D2D1::Point2F(cx - 7.0f * scale, cy + 8.5f * scale), pDrawBrush, 1.1f);
                        pRT->DrawLine(D2D1::Point2F(cx + 5.0f * scale, cy + 5.0f * scale), D2D1::Point2F(cx + 7.0f * scale, cy + 8.5f * scale), pDrawBrush, 1.1f);

                        // If Whiteboard or Blackboard is active, draw a tiny scribble/dot inside
                        if (g_canvasBg == CanvasBg::Whiteboard) {
                            pRT->DrawLine(D2D1::Point2F(cx - 4.0f * scale, cy - 1.0f * scale), D2D1::Point2F(cx + 4.0f * scale, cy - 1.0f * scale), pDrawBrush, 1.0f);
                        } else if (g_canvasBg == CanvasBg::Blackboard) {
                            pRT->DrawLine(D2D1::Point2F(cx - 4.0f * scale, cy - 2.0f * scale), D2D1::Point2F(cx + 2.0f * scale, cy - 2.0f * scale), pDrawBrush, 1.0f);
                            pRT->DrawLine(D2D1::Point2F(cx - 4.0f * scale, cy + 1.0f * scale), D2D1::Point2F(cx + 4.0f * scale, cy + 1.0f * scale), pDrawBrush, 1.0f);
                        }
                    }
                }
                else {
                    const wchar_t* iconText = (btn.id == 5) ? (g_inkVisible ? L"\uE890" : L"\uED1A") : btn.label.c_str();
                    DrawCachedText(pRT, iconText, g_pIconFormat, btn.rect, pDrawBrush);
                }
                SafeRelease(pLblBrush);
            }

            // Draw tiny downward caret for Shapes, Grid, and Whiteboard Flyout buttons
            if (btn.id == 25 || btn.id == 6 || btn.id == 8) {
                float cx = btn.rect.right - 5.0f * scale;
                float cy = btn.rect.bottom - 5.0f * scale;
                ID2D1SolidColorBrush* pCaretBrush = nullptr;
                pRT->CreateSolidColorBrush(isToolActive ? D2D1::ColorF(0.40f, 0.90f, 0.75f, 0.85f) : D2D1::ColorF(0.70f, 0.75f, 0.82f, 0.65f), &pCaretBrush);
                if (pCaretBrush) {
                    pRT->DrawLine(D2D1::Point2F(cx - 2.5f * scale, cy - 1.5f * scale), D2D1::Point2F(cx, cy + 1.5f * scale), pCaretBrush, 1.0f);
                    pRT->DrawLine(D2D1::Point2F(cx, cy + 1.5f * scale), D2D1::Point2F(cx + 2.5f * scale, cy - 1.5f * scale), pCaretBrush, 1.0f);
                    SafeRelease(pCaretBrush);
                }
            }

            // Draw really small shortcut badge on button (e.g. F, L, H, E, P, M, V, G, S, Z, Y, C, Esc)
            std::wstring shortcut = btn.shortcut;
            if (btn.id == 25) {
                switch (g_currentShape) {
                    case ShapeType::Line:      shortcut = L"L"; break;
                    case ShapeType::Arrow:     shortcut = L"A"; break;
                    case ShapeType::Rectangle: shortcut = L"R"; break;
                    case ShapeType::Ellipse:   shortcut = L"O"; break;
                    case ShapeType::Triangle:  shortcut = L"T"; break;
                    default:                   shortcut = L"L"; break;
                }
            }

            if (g_pToolbarKeyFormat && !shortcut.empty()) {
                ID2D1SolidColorBrush* pKeyBadgeBrush = nullptr;
                if (isDisabled) {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.44f, 0.52f, 0.35f), &pKeyBadgeBrush);
                }
                else if (isToolActive) {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.90f, 0.75f, 0.95f), &pKeyBadgeBrush);
                }
                else if (isHovered) {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.98f, 1.00f, 0.95f), &pKeyBadgeBrush);
                }
                else {
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.55f, 0.62f, 0.72f, 0.75f), &pKeyBadgeBrush);
                }

                if (pKeyBadgeBrush) {
                    D2D1_RECT_F keyRect = D2D1::RectF(
                        btn.rect.left + 2.0f * scale,
                        btn.rect.top + 2.0f * scale,
                        btn.rect.right - 3.5f * scale,
                        btn.rect.top + 13.0f * scale
                    );
                    DrawCachedText(pRT, shortcut, g_pToolbarKeyFormat, keyRect, pKeyBadgeBrush);
                    SafeRelease(pKeyBadgeBrush);
                }
            }
        }
    }

    SafeRelease(pTextBrush);
    SafeRelease(pRimBrush);
    SafeRelease(pBorderBrush);
    SafeRelease(pBgBrush);
}

void DrawRadialMenu(ID2D1HwndRenderTarget* pRT) {
    if (!g_radialActive) return;

    ID2D1SolidColorBrush* pBgBrush = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pSpokeBrush = nullptr;
    ID2D1SolidColorBrush* pGlowBrush = nullptr;
    ID2D1SolidColorBrush* pHoverBrush = nullptr;
    ID2D1SolidColorBrush* pTextBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.08f, 0.10f, 0.15f, 0.94f), &pBgBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.24f, 0.28f, 0.38f, 0.90f), &pBorderBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.20f, 0.24f, 0.32f, 0.80f), &pSpokeBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.32f, 0.85f, 0.69f, 0.50f), &pGlowBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.20f, 0.35f, 0.48f, 0.92f), &pHoverBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.92f, 0.95f, 0.98f, 1.00f), &pTextBrush);

    float cx = g_radialX;
    float cy = g_radialY;
    float scale = g_radialDpiScale;
    if (scale <= 0.1f) scale = 1.0f;
    const float kCenterRadius = 36.0f * scale;
    const float kInnerRingR = 44.0f * scale;
    const float kOuterRingR = 96.0f * scale;
    const float kActionIconR = 70.0f * scale;
    const float kOrbitalRadius = 126.0f * scale;
    const int kRadialSectorCount = (int)g_activeRadialSlots.size();
    if (kRadialSectorCount <= 0) return;
    const float kSectorAngle = (float)(2.0 * 3.14159265358979323846 / (double)kRadialSectorCount);

    // 1. Draw Action Ring Annulus (between 44px and 96px)
    float ringMidR = (kInnerRingR + kOuterRingR) * 0.5f;
    float ringThick = (kOuterRingR - kInnerRingR);
    if (pBgBrush) pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), ringMidR, ringMidR), pBgBrush, ringThick);
    if (pBorderBrush) {
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), kInnerRingR, kInnerRingR), pBorderBrush, 1.2f);
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), kOuterRingR, kOuterRingR), pBorderBrush, 1.2f);
    }

    // 2. Draw N Radial Divider Spokes
    if (pSpokeBrush) {
        for (int k = 0; k < kRadialSectorCount; ++k) {
            float spokeAngle = (float)(k * kSectorAngle + kSectorAngle * 0.5f);
            float x1 = cx + std::cos(spokeAngle) * kInnerRingR;
            float y1 = cy + std::sin(spokeAngle) * kInnerRingR;
            float x2 = cx + std::cos(spokeAngle) * kOuterRingR;
            float y2 = cy + std::sin(spokeAngle) * kOuterRingR;
            pRT->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), pSpokeBrush, 1.2f);
        }
    }

    // 3. Draw Hovered Sector Highlight
    if (g_radialHoverSector >= 0 && g_radialHoverSector < kRadialSectorCount) {
        RadialAction hoverAct = g_activeRadialSlots[g_radialHoverSector];
        bool secDisabled = (hoverAct == RadialAction::Clear && g_strokes.empty() && g_laserStrokes.empty()) ||
                           (hoverAct == RadialAction::Undo && g_undoStack.empty()) ||
                           (hoverAct == RadialAction::Redo && g_redoStack.empty());
        if (!secDisabled) {
            float secAngle = (float)(g_radialHoverSector * kSectorAngle);
            float hx = cx + std::cos(secAngle) * kActionIconR;
            float hy = cy + std::sin(secAngle) * kActionIconR;
            if (pHoverBrush) pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(hx, hy), 18.0f * scale, 18.0f * scale), pHoverBrush);
            if (pGlowBrush) pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(hx, hy), 18.0f * scale, 18.0f * scale), pGlowBrush, 1.5f);
        }
    }

    // 4. Draw Sector Action Icons
    for (int k = 0; k < kRadialSectorCount; ++k) {
        RadialAction act = g_activeRadialSlots[k];
        float secAngle = (float)(k * kSectorAngle);
        float ix = cx + std::cos(secAngle) * kActionIconR;
        float iy = cy + std::sin(secAngle) * kActionIconR;

        bool isSecDisabled = (act == RadialAction::Clear && g_strokes.empty() && g_laserStrokes.empty()) ||
                             (act == RadialAction::Undo && g_undoStack.empty()) ||
                             (act == RadialAction::Redo && g_redoStack.empty());

        bool isSecActive = false;
        if (act == RadialAction::Pen && g_currentTool == ToolMode::Pen && g_currentShape == ShapeType::Freehand) isSecActive = true;
        else if (act == RadialAction::Eraser && g_currentTool == ToolMode::Eraser) isSecActive = true;
        else if (act == RadialAction::Highlighter && g_currentTool == ToolMode::Highlighter) isSecActive = true;
        else if (act == RadialAction::Laser && g_currentTool == ToolMode::Laser) isSecActive = true;
        else if (act == RadialAction::Pan && g_currentTool == ToolMode::Pan) isSecActive = true;
        else if (act == RadialAction::Pointer && g_currentTool == ToolMode::Pointer) isSecActive = true;
        else if (act == RadialAction::Shape && (g_currentShape != ShapeType::Freehand && g_currentTool == ToolMode::Pen)) isSecActive = true;
        else if (act == RadialAction::Grid && g_gridStyle != GridStyle::None) isSecActive = true;
        else if (act == RadialAction::Whiteboard && g_canvasBg != CanvasBg::Transparent) isSecActive = true;

        ID2D1SolidColorBrush* pCustomBrush = nullptr;
        if (isSecDisabled) {
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.44f, 0.52f, 0.38f), &pCustomBrush);
        } else if (isSecActive) {
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.90f, 0.75f, 1.0f), &pCustomBrush);
        }
        ID2D1SolidColorBrush* pDrawBrush = pCustomBrush ? pCustomBrush : pTextBrush;

        if (act == RadialAction::Laser) {
            // Draw crisp optic vector laser beacon
            if (pDrawBrush) {
                D2D1_ELLIPSE coreEll = D2D1::Ellipse(D2D1::Point2F(ix, iy), 2.2f * scale, 2.2f * scale);
                pRT->FillEllipse(coreEll, pDrawBrush);
                D2D1_ELLIPSE ringEll = D2D1::Ellipse(D2D1::Point2F(ix, iy), 5.5f * scale, 5.5f * scale);
                pRT->DrawEllipse(ringEll, pDrawBrush, 1.2f);
                pRT->DrawLine(D2D1::Point2F(ix - 8.0f * scale, iy), D2D1::Point2F(ix - 5.5f * scale, iy), pDrawBrush, 1.2f);
                pRT->DrawLine(D2D1::Point2F(ix + 5.5f * scale, iy), D2D1::Point2F(ix + 8.0f * scale, iy), pDrawBrush, 1.2f);
                pRT->DrawLine(D2D1::Point2F(ix, iy - 8.0f * scale), D2D1::Point2F(ix, iy - 5.5f * scale), pDrawBrush, 1.2f);
                pRT->DrawLine(D2D1::Point2F(ix, iy + 5.5f * scale), D2D1::Point2F(ix, iy + 8.0f * scale), pDrawBrush, 1.2f);
            }
        }
        else if (act == RadialAction::Grid) {
            // Draw crisp vector grid icon
            if (pDrawBrush) {
                float half = 6.0f * scale;
                float off = 2.4f * scale;
                pRT->DrawLine(D2D1::Point2F(ix - half, iy - off), D2D1::Point2F(ix + half, iy - off), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
                pRT->DrawLine(D2D1::Point2F(ix - half, iy + off), D2D1::Point2F(ix + half, iy + off), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
                pRT->DrawLine(D2D1::Point2F(ix - off, iy - half), D2D1::Point2F(ix - off, iy + half), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
                pRT->DrawLine(D2D1::Point2F(ix + off, iy - half), D2D1::Point2F(ix + off, iy + half), pDrawBrush, 1.25f, g_pRoundStrokeStyle);
            }
        }
        else if (act == RadialAction::Shape && g_currentShape == ShapeType::Triangle) {
            // Draw crisp vector triangle
            if (pDrawBrush) {
                float triH = 12.0f * scale;
                float triW = 13.0f * scale;
                D2D1_POINT_2F p1 = D2D1::Point2F(ix, iy - triH * 0.5f);
                D2D1_POINT_2F p2 = D2D1::Point2F(ix - triW * 0.5f, iy + triH * 0.5f);
                D2D1_POINT_2F p3 = D2D1::Point2F(ix + triW * 0.5f, iy + triH * 0.5f);
                pRT->DrawLine(p1, p2, pDrawBrush, 1.4f, g_pRoundStrokeStyle);
                pRT->DrawLine(p2, p3, pDrawBrush, 1.4f, g_pRoundStrokeStyle);
                pRT->DrawLine(p3, p1, pDrawBrush, 1.4f, g_pRoundStrokeStyle);
            }
        }
        else if (act == RadialAction::Whiteboard) {
            // Sleek vector Whiteboard / Blackboard easel icon (matching toolbar button 8)
            D2D1_RECT_F boardR = D2D1::RectF(ix - 7.5f * scale, iy - 6.0f * scale, ix + 7.5f * scale, iy + 3.5f * scale);
            if (pDrawBrush) {
                // Board frame
                pRT->DrawRoundedRectangle(D2D1::RoundedRect(boardR, 1.5f * scale, 1.5f * scale), pDrawBrush, 1.2f);
                // Bottom tray
                pRT->DrawLine(D2D1::Point2F(ix - 8.5f * scale, iy + 4.5f * scale), D2D1::Point2F(ix + 8.5f * scale, iy + 4.5f * scale), pDrawBrush, 1.2f);
                // Easel legs
                pRT->DrawLine(D2D1::Point2F(ix - 5.0f * scale, iy + 5.0f * scale), D2D1::Point2F(ix - 7.0f * scale, iy + 8.5f * scale), pDrawBrush, 1.1f);
                pRT->DrawLine(D2D1::Point2F(ix + 5.0f * scale, iy + 5.0f * scale), D2D1::Point2F(ix + 7.0f * scale, iy + 8.5f * scale), pDrawBrush, 1.1f);

                // If Whiteboard or Blackboard is active, draw a tiny scribble/dot inside
                if (g_canvasBg == CanvasBg::Whiteboard) {
                    pRT->DrawLine(D2D1::Point2F(ix - 4.0f * scale, iy - 1.0f * scale), D2D1::Point2F(ix + 4.0f * scale, iy - 1.0f * scale), pDrawBrush, 1.0f);
                } else if (g_canvasBg == CanvasBg::Blackboard) {
                    pRT->DrawLine(D2D1::Point2F(ix - 4.0f * scale, iy - 2.0f * scale), D2D1::Point2F(ix + 2.0f * scale, iy - 2.0f * scale), pDrawBrush, 1.0f);
                    pRT->DrawLine(D2D1::Point2F(ix - 4.0f * scale, iy + 1.0f * scale), D2D1::Point2F(ix + 4.0f * scale, iy + 1.0f * scale), pDrawBrush, 1.0f);
                }
            }
        }
        else {
            const wchar_t* iconText = L"";
            switch (act) {
                case RadialAction::Clear:       iconText = L"\uE74D"; break;
                case RadialAction::Snapshot:    iconText = L"\uE722"; break;
                case RadialAction::Snip:        iconText = L"\uF407"; break;
                case RadialAction::Eraser:      iconText = L"\uE75C"; break;
                case RadialAction::Undo:        iconText = L"\uE7A7"; break;
                case RadialAction::Redo:        iconText = L"\uE7A6"; break;
                case RadialAction::Pointer:     iconText = L"\uE7C9"; break;
                case RadialAction::InkVisible:  iconText = g_inkVisible ? L"\uE890" : L"\uED1A"; break;
                case RadialAction::Pan:         iconText = L"\uE7C2"; break;
                case RadialAction::Pen:         iconText = L"\uEC87"; break;
                case RadialAction::Highlighter: iconText = L"\uE7E6"; break;
                case RadialAction::Exit:        iconText = L"\uE8BB"; break;
                case RadialAction::Shape: {
                    switch (g_currentShape) {
                        case ShapeType::Line:      iconText = L"\uED5E"; break;
                        case ShapeType::Arrow:     iconText = L"\uE72A"; break;
                        case ShapeType::Rectangle: iconText = L"\uE739"; break;
                        case ShapeType::Ellipse:   iconText = L"\uEA3A"; break;
                        default:                   iconText = L"\uED5E"; break;
                    }
                    break;
                }
                default: iconText = L"\uE7C9"; break;
            }

            if (g_pRadialIconFormat && pDrawBrush && iconText[0] != L'\0') {
                D2D1_RECT_F iconRect = D2D1::RectF(ix - 16.0f * scale, iy - 16.0f * scale, ix + 16.0f * scale, iy + 16.0f * scale);
                pRT->DrawText(
                    iconText, (UINT32)wcslen(iconText),
                    g_pRadialIconFormat,
                    iconRect,
                    pDrawBrush,
                    D2D1_DRAW_TEXT_OPTIONS_NONE
                );
            }
        }

        if (pCustomBrush) pCustomBrush->Release();
    }

    // 5. Center Hub: Active Swatch & Pen/Highlighter/Laser Toggle
    bool centerHovered = (g_radialHoverTarget == RadialTarget::Center);
    if (centerHovered && pGlowBrush) {
        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), kCenterRadius + 3.0f * scale, kCenterRadius + 3.0f * scale), pGlowBrush);
    }
    if (pBgBrush) pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), kCenterRadius, kCenterRadius), pBgBrush);
    ID2D1SolidColorBrush* pCenterBorder = centerHovered ? (pGlowBrush ? pGlowBrush : pBorderBrush) : pBorderBrush;
    if (pCenterBorder) pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), kCenterRadius, kCenterRadius), pCenterBorder, centerHovered ? 2.0f : 1.5f);

    ID2D1SolidColorBrush* pActiveBrush = nullptr;
    pRT->CreateSolidColorBrush(g_activeColor, &pActiveBrush);
    if (pActiveBrush) {
        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 16.0f * scale, 16.0f * scale), pActiveBrush);
        if (pBorderBrush) pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 16.0f * scale, 16.0f * scale), pBorderBrush, 1.2f);
        pActiveBrush->Release();
    }

    const wchar_t* centerBadge = (g_currentTool == ToolMode::Highlighter) ? L"\uE7E6" : (g_currentTool == ToolMode::Laser ? L"\uE814" : L"\uEC87");
    if (g_pCenterBadgeFormat) {
        D2D1_RECT_F centerTextRect = D2D1::RectF(cx - 14.0f * scale, cy - 14.0f * scale, cx + 14.0f * scale, cy + 14.0f * scale);
        ID2D1SolidColorBrush* pWhite = nullptr;
        pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &pWhite);
        if (pWhite) {
            pRT->DrawText(centerBadge, (UINT32)wcslen(centerBadge), g_pCenterBadgeFormat, centerTextRect, pWhite);
            pWhite->Release();
        }
    }

    if (g_settings.showRadialColorRing) {
        // 6. Outer Color Wheel Guide track
        if (pBorderBrush) pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), kOrbitalRadius, kOrbitalRadius), pBorderBrush, 0.8f);

        // 7. 360-Degree Orbital Color Orbs (16 colors)
        for (size_t i = 0; i < kPresetColorCount; ++i) {
            float angle = (float)(i * (2.0 * 3.14159265358979323846 / kPresetColorCount) - 3.14159265358979323846 * 0.5);
            float ox = cx + std::cos(angle) * kOrbitalRadius;
            float oy = cy + std::sin(angle) * kOrbitalRadius;

            bool isHovered = (g_hoveredOrb == (int)i);
            float orbR = isHovered ? (15.5f * scale) : (11.5f * scale);

            if (i == 0) {
                // 12 o'clock Recent Colors Expansion Hub
                bool isHubHovered = (g_radialHoverTarget == RadialTarget::RecentHub || isHovered);
                orbR = isHubHovered ? (15.5f * scale) : (12.0f * scale);
                if (isHubHovered && pGlowBrush) {
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), orbR + 4.0f * scale, orbR + 4.0f * scale), pGlowBrush);
                }
                ID2D1SolidColorBrush* pHubFill = nullptr;
                pRT->CreateSolidColorBrush(g_customColor.activeColor, &pHubFill);
                if (pHubFill) {
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), orbR, orbR), pHubFill);
                    pHubFill->Release();
                }
                ID2D1SolidColorBrush* pHubBorder = nullptr;
                pRT->CreateSolidColorBrush(isHubHovered ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f) : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.50f), &pHubBorder);
                if (pHubBorder) {
                    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), orbR, orbR), pHubBorder, isHubHovered ? 2.0f : 1.2f);
                    pHubBorder->Release();
                }
                // Draw crisp plus icon centered in the RecentHub orb
                float plusLen = isHubHovered ? (5.5f * scale) : (4.5f * scale);
                ID2D1SolidColorBrush* pPlusShadow = nullptr;
                pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.65f), &pPlusShadow);
                if (pPlusShadow) {
                    pRT->DrawLine(D2D1::Point2F(ox - plusLen, oy + 0.8f), D2D1::Point2F(ox + plusLen, oy + 0.8f), pPlusShadow, 2.6f, g_pRoundStrokeStyle);
                    pRT->DrawLine(D2D1::Point2F(ox, oy - plusLen + 0.8f), D2D1::Point2F(ox, oy + plusLen + 0.8f), pPlusShadow, 2.6f, g_pRoundStrokeStyle);
                    pPlusShadow->Release();
                }
                ID2D1SolidColorBrush* pPlusWhite = nullptr;
                pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &pPlusWhite);
                if (pPlusWhite) {
                    pRT->DrawLine(D2D1::Point2F(ox - plusLen, oy), D2D1::Point2F(ox + plusLen, oy), pPlusWhite, 2.0f, g_pRoundStrokeStyle);
                    pRT->DrawLine(D2D1::Point2F(ox, oy - plusLen), D2D1::Point2F(ox, oy + plusLen), pPlusWhite, 2.0f, g_pRoundStrokeStyle);
                    pPlusWhite->Release();
                }
                continue;
            }

            ID2D1SolidColorBrush* pOrbBrush = nullptr;
            pRT->CreateSolidColorBrush(kPresetColors[i], &pOrbBrush);
            if (pOrbBrush) {
                if (isHovered) {
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), orbR + 4.0f * scale, orbR + 4.0f * scale), pGlowBrush);
                }
                pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), orbR, orbR), pOrbBrush);

                ID2D1SolidColorBrush* pOrbBorder = nullptr;
                pRT->CreateSolidColorBrush(isHovered ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f) : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.40f), &pOrbBorder);
                if (pOrbBorder) {
                    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(ox, oy), orbR, orbR), pOrbBorder, isHovered ? 2.0f : 1.0f);
                    pOrbBorder->Release();
                }

                pOrbBrush->Release();
            }
        }

        // 8. Layer 2: Radial Menu Satellite Arc (Recent 5 Colors)
        if (g_radialRecentFanOpen) {
            const float kSatelliteRadius = 162.0f * scale;
            const float kRad = 3.14159265358979323846f / 180.0f;
            int numOrbs = std::min(5, (int)g_recentColors.size());

            // Curved guide arc for satellite tier: cached hardware geometry
            if (numOrbs > 1) {
                if (!g_pRadialSatelliteArcGeom || g_cachedSatelliteNumOrbs != numOrbs || g_cachedSatelliteScale != scale) {
                    SafeRelease(g_pRadialSatelliteArcGeom);
                    g_cachedSatelliteNumOrbs = numOrbs;
                    g_cachedSatelliteScale = scale;
                    if (g_pD2DFactory && SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&g_pRadialSatelliteArcGeom))) {
                        ID2D1GeometrySink* pSink = nullptr;
                        if (SUCCEEDED(g_pRadialSatelliteArcGeom->Open(&pSink))) {
                            float aStart = -90.0f * kRad + (float)(0 - (numOrbs - 1) * 0.5f) * (16.0f * kRad);
                            float aEnd = -90.0f * kRad + (float)(numOrbs - 1 - (numOrbs - 1) * 0.5f) * (16.0f * kRad);
                            pSink->BeginFigure(D2D1::Point2F(std::cos(aStart) * kSatelliteRadius, std::sin(aStart) * kSatelliteRadius), D2D1_FIGURE_BEGIN_HOLLOW);
                            pSink->AddArc(D2D1::ArcSegment(
                                D2D1::Point2F(std::cos(aEnd) * kSatelliteRadius, std::sin(aEnd) * kSatelliteRadius),
                                D2D1::SizeF(kSatelliteRadius, kSatelliteRadius),
                                0.0f,
                                D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                D2D1_ARC_SIZE_SMALL
                            ));
                            pSink->EndFigure(D2D1_FIGURE_END_OPEN);
                            pSink->Close();
                            SafeRelease(pSink);
                        } else {
                            SafeRelease(g_pRadialSatelliteArcGeom);
                        }
                    }
                }

                if (g_pRadialSatelliteArcGeom) {
                    ID2D1SolidColorBrush* pArcBrush = nullptr;
                    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.48f, 0.60f, 0.45f), &pArcBrush);
                    if (pArcBrush) {
                        D2D1_MATRIX_3X2_F oldXform;
                        pRT->GetTransform(&oldXform);
                        pRT->SetTransform(D2D1::Matrix3x2F::Translation(cx, cy) * oldXform);
                        pRT->DrawGeometry(g_pRadialSatelliteArcGeom, pArcBrush, 1.2f);
                        pRT->SetTransform(oldXform);
                        SafeRelease(pArcBrush);
                    }
                }
            }

            // Draw the Recent Satellite Orbs
            for (int j = 0; j < (int)g_recentColors.size() && j < 5; ++j) {
                float angle = -90.0f * kRad + (float)(j - (numOrbs - 1) * 0.5f) * (16.0f * kRad);
                float sx = cx + std::cos(angle) * kSatelliteRadius;
                float sy = cy + std::sin(angle) * kSatelliteRadius;

                bool isSatHovered = (g_hoveredRecentOrb == j);
                float satR = isSatHovered ? (15.0f * scale) : (11.5f * scale);

                ID2D1SolidColorBrush* pSatBrush = nullptr;
                pRT->CreateSolidColorBrush(g_recentColors[j], &pSatBrush);
                if (pSatBrush) {
                    if (isSatHovered && pGlowBrush) {
                        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), satR + 4.0f * scale, satR + 4.0f * scale), pGlowBrush);
                    }
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), satR, satR), pSatBrush);

                    ID2D1SolidColorBrush* pSatBorder = nullptr;
                    pRT->CreateSolidColorBrush(isSatHovered ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f) : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.40f), &pSatBorder);
                    if (pSatBorder) {
                        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), satR, satR), pSatBorder, isSatHovered ? 2.0f : 1.0f);
                        pSatBorder->Release();
                    }

                    pSatBrush->Release();
                }
            }
        }
    }

    if (pTextBrush) pTextBrush->Release();
    if (pHoverBrush) pHoverBrush->Release();
    if (pGlowBrush) pGlowBrush->Release();
    if (pSpokeBrush) pSpokeBrush->Release();
    if (pBorderBrush) pBorderBrush->Release();
    if (pBgBrush) pBgBrush->Release();
}

void DrawEraserCursor(ID2D1HwndRenderTarget* pRT) {
    if (!g_bIsActive || g_hideUIForCapture || g_isSnipping || g_isEyedropperActive || g_radialActive) return;
    if (!g_isRightClickErasing && !g_isRightMouseDown && !g_isLeftClickErasing && !g_isRightClickClearing && g_currentTool != ToolMode::Eraser) return;

    // Suppress drawing over toolbar, collapsed pill, and open flyouts
    if (g_shapesFlyoutOpen &&
        g_cursorX >= g_shapesFlyoutRect.left && g_cursorX <= g_shapesFlyoutRect.right &&
        g_cursorY >= g_shapesFlyoutRect.top && g_cursorY <= g_shapesFlyoutRect.bottom) {
        return;
    }
    if (g_gridFlyoutOpen &&
        g_cursorX >= g_gridFlyoutRect.left && g_cursorX <= g_gridFlyoutRect.right &&
        g_cursorY >= g_gridFlyoutRect.top && g_cursorY <= g_gridFlyoutRect.bottom) {
        return;
    }
    if (g_backdropFlyoutOpen &&
        g_cursorX >= g_backdropFlyoutRect.left && g_cursorX <= g_backdropFlyoutRect.right &&
        g_cursorY >= g_backdropFlyoutRect.top && g_cursorY <= g_backdropFlyoutRect.bottom) {
        return;
    }
    if (g_colorFlyoutOpen &&
        g_cursorX >= g_colorFlyoutRect.left && g_cursorX <= g_colorFlyoutRect.right &&
        g_cursorY >= g_colorFlyoutRect.top && g_cursorY <= g_colorFlyoutRect.bottom) {
        return;
    }
    if (g_settings.showBottomToolbar &&
        g_cursorX >= (g_toolbarRect.left - 4.0f) && g_cursorX <= (g_toolbarRect.right + 4.0f) &&
        g_cursorY >= (g_toolbarRect.top - 4.0f) && g_cursorY <= (g_toolbarRect.bottom + 4.0f)) {
        return;
    }

    if (g_settings.crossType == CursorType::Brush) {
        // --- Live Eraser Brush Footprint Cursor ---
        ID2D1SolidColorBrush* pFillBrush = nullptr;
        ID2D1SolidColorBrush* pRingBrush = nullptr;
        ID2D1SolidColorBrush* pShadowBrush = nullptr;
        ID2D1SolidColorBrush* pCenterDotBrush = nullptr;

        pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.35f, 0.40f, 0.22f), &pFillBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.90f), &pRingBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.50f), &pShadowBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.35f, 0.40f, 1.0f), &pCenterDotBrush);

        D2D1_ELLIPSE ell = D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), g_eraserRadius, g_eraserRadius);

        if (pFillBrush) {
            pRT->FillEllipse(ell, pFillBrush);
            pFillBrush->Release();
        }
        // Dual-contrast outline (outer dark shadow ring + inner white ring)
        if (pShadowBrush) {
            pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), g_eraserRadius + 0.6f, g_eraserRadius + 0.6f), pShadowBrush, 1.0f);
        }
        if (pRingBrush) {
            pRT->DrawEllipse(ell, pRingBrush, 1.0f);
            pRingBrush->Release();
        }

        // Precision Center Reticle Dot
        if (pShadowBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 2.0f, 2.0f), pShadowBrush);
            pShadowBrush->Release();
        }
        if (pCenterDotBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 1.2f, 1.2f), pCenterDotBrush);
            pCenterDotBrush->Release();
        }
    }
    else {
        // --- Precision Crosshair Cursor with Customizable crossSize & Eraser Reticle ---
        float arm = (float)std::max(4, g_settings.crossSize);

        ID2D1SolidColorBrush* pShadowBrush = nullptr;
        ID2D1SolidColorBrush* pWhiteBrush = nullptr;
        ID2D1SolidColorBrush* pAccentBrush = nullptr;
        ID2D1SolidColorBrush* pRadiusBrush = nullptr;

        pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.65f), &pShadowBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &pWhiteBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.35f, 0.40f, 1.0f), &pAccentBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.35f, 0.40f, 0.30f), &pRadiusBrush);

        // Faint contextual outer circle indicating active eraser blast radius
        if (pRadiusBrush) {
            pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), g_eraserRadius, g_eraserRadius), pRadiusBrush, 1.0f);
            pRadiusBrush->Release();
        }

        D2D1_POINT_2F pLeft   = D2D1::Point2F(g_cursorX - arm, g_cursorY);
        D2D1_POINT_2F pRight  = D2D1::Point2F(g_cursorX + arm, g_cursorY);
        D2D1_POINT_2F pTop    = D2D1::Point2F(g_cursorX, g_cursorY - arm);
        D2D1_POINT_2F pBottom = D2D1::Point2F(g_cursorX, g_cursorY + arm);

        // 1. Dual-contrast shadow lines (2.5px dark background for universal contrast)
        if (pShadowBrush) {
            pRT->DrawLine(pLeft, pRight, pShadowBrush, 2.5f);
            pRT->DrawLine(pTop, pBottom, pShadowBrush, 2.5f);
        }

        // 2. Crisp 1.2px white crosshair core
        if (pWhiteBrush) {
            pRT->DrawLine(pLeft, pRight, pWhiteBrush, 1.2f);
            pRT->DrawLine(pTop, pBottom, pWhiteBrush, 1.2f);
            pWhiteBrush->Release();
        }

        // 3. Pinpoint center dot in eraser coral/red
        if (pShadowBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 2.2f, 2.2f), pShadowBrush);
            pShadowBrush->Release();
        }
        if (pAccentBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 1.3f, 1.3f), pAccentBrush);
            pAccentBrush->Release();
        }
    }
}

void DrawInkingCursor(ID2D1HwndRenderTarget* pRT) {
    if (!g_bIsActive) return;
    if (g_hideUIForCapture) return;
    if (g_isSnipping || g_isEyedropperActive) return;
    if (g_currentTool != ToolMode::Pen && g_currentTool != ToolMode::Highlighter) return;
    if (g_isRightMouseDown || g_isRightClickErasing || g_isRightClickClearing || g_isLeftClickErasing) return;
    if (g_radialActive) return;

    // Suppress drawing over toolbar, collapsed pill, and open flyouts
    if (g_shapesFlyoutOpen &&
        g_cursorX >= g_shapesFlyoutRect.left && g_cursorX <= g_shapesFlyoutRect.right &&
        g_cursorY >= g_shapesFlyoutRect.top && g_cursorY <= g_shapesFlyoutRect.bottom) {
        return;
    }
    if (g_gridFlyoutOpen &&
        g_cursorX >= g_gridFlyoutRect.left && g_cursorX <= g_gridFlyoutRect.right &&
        g_cursorY >= g_gridFlyoutRect.top && g_cursorY <= g_gridFlyoutRect.bottom) {
        return;
    }
    if (g_backdropFlyoutOpen &&
        g_cursorX >= g_backdropFlyoutRect.left && g_cursorX <= g_backdropFlyoutRect.right &&
        g_cursorY >= g_backdropFlyoutRect.top && g_cursorY <= g_backdropFlyoutRect.bottom) {
        return;
    }
    if (g_colorFlyoutOpen &&
        g_cursorX >= g_colorFlyoutRect.left && g_cursorX <= g_colorFlyoutRect.right &&
        g_cursorY >= g_colorFlyoutRect.top && g_cursorY <= g_colorFlyoutRect.bottom) {
        return;
    }
    if (g_settings.showBottomToolbar &&
        g_cursorX >= (g_toolbarRect.left - 4.0f) && g_cursorX <= (g_toolbarRect.right + 4.0f) &&
        g_cursorY >= (g_toolbarRect.top - 4.0f) && g_cursorY <= (g_toolbarRect.bottom + 4.0f)) {
        return;
    }

    if (g_settings.crossType == CursorType::Brush) {
        // --- Live Brush Size & Color Cursor ---
        float activeWidth = (g_currentTool == ToolMode::Highlighter) 
            ? g_settings.defaultHighlighterWidth 
            : g_currentPenWidth;
        
        float screenRadius = std::max(1.5f, (activeWidth * g_zoomScale) * 0.5f);

        D2D1_COLOR_F inkColor = g_activeColor;
        if (g_currentTool == ToolMode::Highlighter) {
            inkColor.a = 0.35f;
        } else {
            inkColor.a = std::max(0.20f, std::min(0.65f, inkColor.a * 0.60f));
        }

        ID2D1SolidColorBrush* pFillBrush = nullptr;
        ID2D1SolidColorBrush* pRingBrush = nullptr;
        ID2D1SolidColorBrush* pShadowBrush = nullptr;
        ID2D1SolidColorBrush* pCenterDotBrush = nullptr;

        pRT->CreateSolidColorBrush(inkColor, &pFillBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.90f), &pRingBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.50f), &pShadowBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(g_activeColor.r, g_activeColor.g, g_activeColor.b, 1.0f), &pCenterDotBrush);

        D2D1_ELLIPSE ell = D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), screenRadius, screenRadius);

        if (pFillBrush) {
            pRT->FillEllipse(ell, pFillBrush);
            pFillBrush->Release();
        }
        // Dual-contrast outline (outer dark shadow ring + inner white ring) visible on all backgrounds
        if (pShadowBrush) {
            pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), screenRadius + 0.6f, screenRadius + 0.6f), pShadowBrush, 1.0f);
        }
        if (pRingBrush) {
            pRT->DrawEllipse(ell, pRingBrush, 1.0f);
        }

        // Precision Center Reticle Dot
        if (pShadowBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 2.0f, 2.0f), pShadowBrush);
        }
        if (pCenterDotBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 1.2f, 1.2f), pCenterDotBrush);
            pCenterDotBrush->Release();
        }

        if (pShadowBrush) pShadowBrush->Release();
        if (pRingBrush) pRingBrush->Release();
    }
    else {
        // --- Precision Crosshair Cursor with Customizable Size ---
        float scale = GetDpiScaleAtPoint(g_cursorX, g_cursorY);
        float arm = (float)std::max(4, g_settings.crossSize) * scale;

        ID2D1SolidColorBrush* pShadowBrush = nullptr;
        ID2D1SolidColorBrush* pWhiteBrush = nullptr;
        ID2D1SolidColorBrush* pAccentBrush = nullptr;

        pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.65f), &pShadowBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &pWhiteBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(g_activeColor.r, g_activeColor.g, g_activeColor.b, 1.0f), &pAccentBrush);

        D2D1_POINT_2F pLeft   = D2D1::Point2F(g_cursorX - arm, g_cursorY);
        D2D1_POINT_2F pRight  = D2D1::Point2F(g_cursorX + arm, g_cursorY);
        D2D1_POINT_2F pTop    = D2D1::Point2F(g_cursorX, g_cursorY - arm);
        D2D1_POINT_2F pBottom = D2D1::Point2F(g_cursorX, g_cursorY + arm);

        // 1. Dual-contrast shadow lines (2.5px dark background for universal contrast)
        if (pShadowBrush) {
            pRT->DrawLine(pLeft, pRight, pShadowBrush, 2.5f * scale);
            pRT->DrawLine(pTop, pBottom, pShadowBrush, 2.5f * scale);
        }

        // 2. Crisp 1.2px white crosshair core
        if (pWhiteBrush) {
            pRT->DrawLine(pLeft, pRight, pWhiteBrush, 1.2f * scale);
            pRT->DrawLine(pTop, pBottom, pWhiteBrush, 1.2f * scale);
            pWhiteBrush->Release();
        }

        // 3. Pinpoint center dot in active brush color
        if (pShadowBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 2.2f * scale, 2.2f * scale), pShadowBrush);
            pShadowBrush->Release();
        }
        if (pAccentBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), 1.4f * scale, 1.4f * scale), pAccentBrush);
            pAccentBrush->Release();
        }
    }
}

void DrawPenSizePreview(ID2D1HwndRenderTarget* pRT) {
    if (g_sizePreviewTime == 0) return;
    ULONGLONG elapsed = GetTickCount64() - g_sizePreviewTime;
    if (elapsed > 900) {
        g_sizePreviewTime = 0;
        return;
    }

    float alpha = 1.0f;
    if (elapsed > 600) {
        alpha = 1.0f - (float)(elapsed - 600) / 300.0f;
    }

    // Determine current active tool width scaled by canvas zoom
    float activeWidth = (g_currentTool == ToolMode::Highlighter) 
        ? g_settings.defaultHighlighterWidth 
        : g_settings.defaultPenWidth;
    
    // Scale screen radius by zoom so it exactly matches what appears on canvas
    float screenRadius = std::max(1.0f, (activeWidth * g_zoomScale) * 0.5f);

    // Ink fill with active color & tool opacity
    D2D1_COLOR_F inkColor = g_activeColor;
    if (g_currentTool == ToolMode::Highlighter) {
        inkColor.a = 0.35f * alpha;
    } else {
        inkColor.a = std::min(1.0f, inkColor.a) * alpha;
    }

    ID2D1SolidColorBrush* pFillBrush = nullptr;
    pRT->CreateSolidColorBrush(inkColor, &pFillBrush);

    ID2D1SolidColorBrush* pRingBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.90f * alpha), &pRingBrush);

    ID2D1SolidColorBrush* pShadowBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f * alpha), &pShadowBrush);

    D2D1_ELLIPSE ell = D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), screenRadius, screenRadius);

    if (pFillBrush) {
        pRT->FillEllipse(ell, pFillBrush);
        pFillBrush->Release();
    }
    // High-contrast dual-stroke border (subtle outer dark shadow + white ring) so it is visible on any background
    if (pShadowBrush) {
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(g_cursorX, g_cursorY), screenRadius + 0.5f, screenRadius + 0.5f), pShadowBrush, 1.0f);
        pShadowBrush->Release();
    }
    if (pRingBrush) {
        pRT->DrawEllipse(ell, pRingBrush, 1.0f);
        pRingBrush->Release();
    }
}

void DrawZoomPreview(ID2D1HwndRenderTarget* pRT) {
    if (g_zoomPreviewTime == 0) return;
    ULONGLONG elapsed = GetTickCount64() - g_zoomPreviewTime;
    if (elapsed > 1100) {
        g_zoomPreviewTime = 0;
        return;
    }

    float alpha = 1.0f;
    if (elapsed > 800) {
        alpha = 1.0f - (float)(elapsed - 800) / 300.0f;
    }

    int zoomPct = (int)std::round(g_zoomScale * 100.0f);
    std::wstring text = L"Zoom: " + std::to_wstring(zoomPct) + L"%";

    float scale = GetDpiScaleAtPoint((g_toolbarRect.left + g_toolbarRect.right) * 0.5f, g_toolbarRect.top);
    const float badgeW = 110.0f * scale;
    const float badgeH = 26.0f * scale;
    float badgeX = (g_toolbarRect.left + g_toolbarRect.right - badgeW) * 0.5f;
    float badgeY = g_toolbarRect.top - badgeH - 8.0f * scale;
    if (!g_settings.showBottomToolbar || badgeY < 10.0f) {
        D2D1_SIZE_F rtSize = pRT->GetSize();
        badgeX = (rtSize.width - badgeW) * 0.5f;
        badgeY = rtSize.height - 60.0f * scale;
    }

    D2D1_RECT_F rect = D2D1::RectF(badgeX, badgeY, badgeX + badgeW, badgeY + badgeH);

    ID2D1SolidColorBrush* pBgBrush = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pTextBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.10f, 0.12f, 0.16f, 0.88f * alpha), &pBgBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.35f, 0.40f, 0.50f, 0.80f * alpha), &pBorderBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.96f, 0.98f, 0.95f * alpha), &pTextBrush);

    if (pBgBrush && pBorderBrush && pTextBrush && g_pTextFormat) {
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rect, 5.0f * scale, 5.0f * scale), pBgBrush);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rect, 5.0f * scale, 5.0f * scale), pBorderBrush, 1.0f);
        pRT->DrawText(text.c_str(), (UINT32)text.length(), g_pTextFormat, rect, pTextBrush);
    }

    if (pTextBrush) pTextBrush->Release();
    if (pBorderBrush) pBorderBrush->Release();
    if (pBgBrush) pBgBrush->Release();
}

void DrawSnippingOverlay(ID2D1HwndRenderTarget* pRT) {
    if (!g_isSnipping) return;

    D2D1_SIZE_F rtSize = pRT->GetSize();
    float w = rtSize.width;
    float h = rtSize.height;

    ID2D1SolidColorBrush* pDimBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.50f), &pDimBrush);

    if (!g_isSnippingDrag) {
        // Full-screen dim before drag starts
        if (pDimBrush) {
            pRT->FillRectangle(D2D1::RectF(0, 0, w, h), pDimBrush);
        }

        // Instruction badge near top center
        float scale = GetDpiScaleAtPoint(w * 0.5f, 32.0f);
        const float badgeW = 380.0f * scale;
        const float badgeH = 36.0f * scale;
        float badgeX = (w - badgeW) * 0.5f;
        float badgeY = 32.0f * scale;
        D2D1_RECT_F badgeRect = D2D1::RectF(badgeX, badgeY, badgeX + badgeW, badgeY + badgeH);

        ID2D1SolidColorBrush* pBadgeBg = nullptr;
        ID2D1SolidColorBrush* pBadgeBorder = nullptr;
        ID2D1SolidColorBrush* pBadgeText = nullptr;
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.10f, 0.12f, 0.16f, 0.94f), &pBadgeBg);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.32f, 0.85f, 0.69f, 0.90f), &pBadgeBorder);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.98f, 1.00f, 1.00f), &pBadgeText);

        if (pBadgeBg && pBadgeBorder && pBadgeText && g_pTextFormat) {
            pRT->FillRoundedRectangle(D2D1::RoundedRect(badgeRect, 6.0f * scale, 6.0f * scale), pBadgeBg);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(badgeRect, 6.0f * scale, 6.0f * scale), pBadgeBorder, 1.2f * scale);

            std::wstring hint = L"Click and drag to snip a region  \u2022  ESC to cancel";
            pRT->DrawText(hint.c_str(), (UINT32)hint.length(), g_pTextFormat, badgeRect, pBadgeText);
        }

        if (pBadgeText) pBadgeText->Release();
        if (pBadgeBorder) pBadgeBorder->Release();
        if (pBadgeBg) pBadgeBg->Release();
    }
    else {
        // Dragging selection: dim outside, clear inside
        float selLeft = (float)std::min(g_snipStartPt.x, g_snipEndPt.x);
        float selTop = (float)std::min(g_snipStartPt.y, g_snipEndPt.y);
        float selRight = (float)std::max(g_snipStartPt.x, g_snipEndPt.x);
        float selBottom = (float)std::max(g_snipStartPt.y, g_snipEndPt.y);

        if (pDimBrush) {
            // Top rect
            if (selTop > 0) pRT->FillRectangle(D2D1::RectF(0, 0, w, selTop), pDimBrush);
            // Bottom rect
            if (selBottom < h) pRT->FillRectangle(D2D1::RectF(0, selBottom, w, h), pDimBrush);
            // Left rect
            if (selLeft > 0) pRT->FillRectangle(D2D1::RectF(0, selTop, selLeft, selBottom), pDimBrush);
            // Right rect
            if (selRight < w) pRT->FillRectangle(D2D1::RectF(selRight, selTop, w, selBottom), pDimBrush);
        }

        // Selection border (accent cyan/teal)
        ID2D1SolidColorBrush* pSelBorder = nullptr;
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.32f, 0.85f, 0.69f, 1.00f), &pSelBorder);
        if (pSelBorder) {
            pRT->DrawRectangle(D2D1::RectF(selLeft, selTop, selRight, selBottom), pSelBorder, 1.5f);
            pSelBorder->Release();
        }

        // Dimensions Badge (e.g. "800 x 600")
        int cropW = (int)std::round(selRight - selLeft);
        int cropH = (int)std::round(selBottom - selTop);
        if (cropW > 30 && cropH > 20 && g_pTextFormat) {
            float dimScale = GetDpiScaleAtPoint(selRight, selBottom);
            std::wstring dimText = std::to_wstring(cropW) + L" \u00D7 " + std::to_wstring(cropH);
            float dimW = 90.0f * dimScale;
            float dimH = 22.0f * dimScale;
            float dimX = selRight - dimW;
            float dimY = selBottom + 6.0f * dimScale;
            if (dimY + dimH > h - 8.0f) dimY = selTop - dimH - 6.0f * dimScale;
            if (dimX < 8.0f) dimX = selLeft;

            D2D1_RECT_F dimRect = D2D1::RectF(dimX, dimY, dimX + dimW, dimY + dimH);

            ID2D1SolidColorBrush* pDimBg = nullptr;
            ID2D1SolidColorBrush* pDimText = nullptr;
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.10f, 0.12f, 0.16f, 0.90f), &pDimBg);
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.98f, 1.00f, 0.95f), &pDimText);

            if (pDimBg && pDimText) {
                pRT->FillRoundedRectangle(D2D1::RoundedRect(dimRect, 4.0f * dimScale, 4.0f * dimScale), pDimBg);
                pRT->DrawText(dimText.c_str(), (UINT32)dimText.length(), g_pTextFormat, dimRect, pDimText);
            }

            if (pDimText) pDimText->Release();
            if (pDimBg) pDimBg->Release();
        }
    }

    if (pDimBrush) pDimBrush->Release();
}

void DrawToast(ID2D1HwndRenderTarget* pRT, int screenW, int screenH) {
    if (!g_settings.showToastNotifications) return;
    if (g_toastStartTime == 0) return;
    ULONGLONG elapsed = GetTickCount64() - g_toastStartTime;
    if (elapsed > 2000) {
        g_toastStartTime = 0;
        return;
    }

    float alpha = 1.0f;
    if (elapsed > 1600) {
        alpha = 1.0f - (float)(elapsed - 1600) / 400.0f;
    }

    ID2D1SolidColorBrush* pBg = nullptr;
    ID2D1SolidColorBrush* pBorder = nullptr;
    ID2D1SolidColorBrush* pText = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.10f, 0.13f, 0.18f, 0.94f * alpha), &pBg);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.32f, 0.85f, 0.69f, 0.90f * alpha), &pBorder);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.98f, 1.00f, 1.00f * alpha), &pText);

    HMONITOR hPrimaryMon = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    float scale = GetDpiScaleForMonitor(hPrimaryMon);
    if (scale <= 0.1f) scale = 1.0f;

    const float tw = 340.0f * scale;
    const float th = 40.0f * scale;

    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    float toastX = (screenW - tw) * 0.5f;
    float toastY = 40.0f * scale;

    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (hPrimaryMon && GetMonitorInfo(hPrimaryMon, &mi)) {
        float clientLeft = (float)(mi.rcMonitor.left - vx);
        float monW = (float)(mi.rcMonitor.right - mi.rcMonitor.left);
        float workTop = (float)(mi.rcWork.top - vy);
        toastX = clientLeft + (monW - tw) * 0.5f;
        toastY = workTop + 24.0f * scale;
    }

    D2D1_RECT_F toastRect = D2D1::RectF(
        toastX,
        toastY,
        toastX + tw,
        toastY + th
    );

    pRT->FillRoundedRectangle(D2D1::RoundedRect(toastRect, 6.0f * scale, 6.0f * scale), pBg);
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(toastRect, 6.0f * scale, 6.0f * scale), pBorder, 1.2f * scale);

    if (g_pDWriteFactory && (std::abs(scale - g_currentToastFontScale) > 0.01f || !g_pToastTextFormat || !g_pToastIconFormat)) {
        SafeRelease(g_pToastTextFormat);
        SafeRelease(g_pToastIconFormat);
        g_currentToastFontScale = scale;
        const wchar_t* iconFont = GetIconFontFamilyName();

        g_pDWriteFactory->CreateTextFormat(
            L"Segoe UI Variable Display",
            NULL,
            DWRITE_FONT_WEIGHT_SEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            13.0f * scale,
            L"en-us",
            &g_pToastTextFormat
        );
        if (g_pToastTextFormat) {
            g_pToastTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            g_pToastTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }

        g_pDWriteFactory->CreateTextFormat(
            iconFont,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            16.0f * scale,
            L"en-us",
            &g_pToastIconFormat
        );
        if (g_pToastIconFormat) {
            g_pToastIconFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_pToastIconFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    if (g_pToastIconFormat && g_pToastTextFormat) {
        D2D1_RECT_F iconRect = D2D1::RectF(toastRect.left + 12.0f * scale, toastRect.top, toastRect.left + 36.0f * scale, toastRect.bottom);
        D2D1_RECT_F textRect = D2D1::RectF(toastRect.left + 38.0f * scale, toastRect.top, toastRect.right - 12.0f * scale, toastRect.bottom);

        ID2D1SolidColorBrush* pCheckBrush = nullptr;
        pRT->CreateSolidColorBrush(D2D1::ColorF(0.32f, 0.85f, 0.69f, 1.00f * alpha), &pCheckBrush);
        pRT->DrawText(L"\uE73E", 1, g_pToastIconFormat, iconRect, pCheckBrush ? pCheckBrush : pText);
        if (pCheckBrush) pCheckBrush->Release();

        pRT->DrawText(g_toastMessage.c_str(), (UINT32)g_toastMessage.length(), g_pToastTextFormat, textRect, pText);
    }
    else if (g_pToastTextFormat) {
        pRT->DrawText(g_toastMessage.c_str(), (UINT32)g_toastMessage.length(), g_pToastTextFormat, toastRect, pText);
    }

    if (pText) pText->Release();
    if (pBorder) pBorder->Release();
    if (pBg) pBg->Release();
}

void DrawShapesFlyout(ID2D1HwndRenderTarget* pRT) {
    if (!g_shapesFlyoutOpen || !g_settings.showBottomToolbar) return;

    // 1. Locate the Shapes button (id 25) on the toolbar to align directly above it
    float btnAbsLeft = 0, btnAbsRight = 0;
    bool foundBtn = false;
    for (const auto& btn : g_toolbarButtons) {
        if (btn.id == 25) {
            btnAbsLeft = btn.rect.left;
            btnAbsRight = btn.rect.right;
            foundBtn = true;
            break;
        }
    }
    if (!foundBtn) return;

    float scale = GetDpiScaleAtPoint((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top);
    const float flyoutW = 168.0f * scale;
    const float itemH = 32.0f * scale;
    const float padY = 6.0f * scale;
    const int kItemCount = 5;
    const float flyoutH = padY * 2.0f + kItemCount * itemH; // 172.0f * scale

    float monL = 0, monT = 0, monR = 0, monB = 0;
    GetMonitorBoundsAt((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top, monL, monT, monR, monB);

    float flyoutX = (btnAbsLeft + btnAbsRight) * 0.5f - flyoutW * 0.5f;
    if (flyoutX < monL + 8.0f * scale) flyoutX = monL + 8.0f * scale;
    if (flyoutX + flyoutW > monR - 8.0f * scale) {
        flyoutX = monR - flyoutW - 8.0f * scale;
    }

    float flyoutY = g_toolbarRect.top - flyoutH - 8.0f * scale;
    bool showAbove = true;
    if (flyoutY < monT + 8.0f * scale) {
        flyoutY = g_toolbarRect.bottom + 8.0f * scale;
        showAbove = false;
    }
    if (flyoutY + flyoutH > monB - 8.0f * scale) {
        flyoutY = monB - flyoutH - 8.0f * scale;
    }

    g_shapesFlyoutRect = D2D1::RectF(flyoutX, flyoutY, flyoutX + flyoutW, flyoutY + flyoutH);

    // 2. Acrylic Glassmorphism chassis & styling consistent with main toolbar
    ID2D1SolidColorBrush* pBgBrush = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pRimBrush = nullptr;
    ID2D1SolidColorBrush* pShadowBrush = nullptr;
    ID2D1SolidColorBrush* pTextBrush = nullptr;
    ID2D1SolidColorBrush* pKeyBrush = nullptr;
    ID2D1SolidColorBrush* pActiveBrush = nullptr;
    ID2D1SolidColorBrush* pActiveHoverBrush = nullptr;
    ID2D1SolidColorBrush* pHoverBrush = nullptr;
    ID2D1SolidColorBrush* pActiveAccentBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.08f, 0.10f, 0.14f, 0.96f), &pBgBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.26f, 0.34f, 1.00f), &pBorderBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.48f, 0.60f, 0.50f), &pRimBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f), &pShadowBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.88f, 0.92f, 1.00f), &pTextBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.55f, 0.62f, 0.72f, 0.85f), &pKeyBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.20f, 0.32f, 0.44f, 0.95f), &pActiveBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.24f, 0.36f, 0.48f, 0.98f), &pActiveHoverBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.18f, 0.22f, 0.30f, 0.85f), &pHoverBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.90f, 0.75f, 1.00f), &pActiveAccentBrush);

    // Drop shadow
    if (pShadowBrush) {
        D2D1_ROUNDED_RECT shadowR = D2D1::RoundedRect(
            D2D1::RectF(flyoutX + 2.0f * scale, flyoutY + 2.0f * scale, flyoutX + flyoutW + 2.0f * scale, flyoutY + flyoutH + 2.0f * scale),
            8.0f * scale, 8.0f * scale
        );
        pRT->FillRoundedRectangle(shadowR, pShadowBrush);
    }

    // Modal Background
    D2D1_ROUNDED_RECT modalR = D2D1::RoundedRect(g_shapesFlyoutRect, 8.0f * scale, 8.0f * scale);
    if (pBgBrush) pRT->FillRoundedRectangle(modalR, pBgBrush);
    if (pBorderBrush) pRT->DrawRoundedRectangle(modalR, pBorderBrush, 1.2f);

    // Specular top rim highlight (matching main toolbar)
    if (pRimBrush) {
        pRT->DrawLine(
            D2D1::Point2F(flyoutX + 8.0f * scale, flyoutY + 1.5f),
            D2D1::Point2F(flyoutX + flyoutW - 8.0f * scale, flyoutY + 1.5f),
            pRimBrush, 1.0f
        );
    }

    // Downward caret pointing to toolbar button
    if (showAbove && pBgBrush && pBorderBrush) {
        float tipX = (btnAbsLeft + btnAbsRight) * 0.5f;
        float caretY = flyoutY + flyoutH;
        D2D1_POINT_2F p1 = D2D1::Point2F(tipX - 6.0f * scale, caretY - 0.5f);
        D2D1_POINT_2F p2 = D2D1::Point2F(tipX, caretY + 5.5f * scale);
        D2D1_POINT_2F p3 = D2D1::Point2F(tipX + 6.0f * scale, caretY - 0.5f);

        ID2D1PathGeometry* pCaretGeo = nullptr;
        if (g_pD2DFactory && SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pCaretGeo))) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pCaretGeo->Open(&pSink))) {
                pSink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
                pSink->AddLine(p2);
                pSink->AddLine(p3);
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                pSink->Close();
                SafeRelease(pSink);

                pRT->FillGeometry(pCaretGeo, pBgBrush);
                pRT->DrawLine(p1, p2, pBorderBrush, 1.2f);
                pRT->DrawLine(p2, p3, pBorderBrush, 1.2f);
            }
            SafeRelease(pCaretGeo);
        }
    }

    // 3. Shape Items List
    struct ShapeOption {
        ShapeType type;
        const wchar_t* icon;
        const wchar_t* name;
        const wchar_t* key;
    };
    ShapeOption options[kItemCount] = {
        { ShapeType::Line,      L"\uED5E", L"Line",      L"L" },
        { ShapeType::Arrow,     L"\uE72A", L"Arrow",     L"A" },
        { ShapeType::Rectangle, L"\uE739", L"Rectangle", L"R" },
        { ShapeType::Ellipse,   L"\uEA3A", L"Ellipse",   L"O" },
        { ShapeType::Triangle,  L"\u25B2", L"Triangle",  L"T" },
    };

    for (int i = 0; i < kItemCount; ++i) {
        float itemTop = flyoutY + padY + i * itemH;
        D2D1_RECT_F itemR = D2D1::RectF(flyoutX + 5.0f * scale, itemTop, flyoutX + flyoutW - 5.0f * scale, itemTop + itemH);
        D2D1_ROUNDED_RECT roundItem = D2D1::RoundedRect(itemR, 4.0f * scale, 4.0f * scale);

        bool isSelected = (g_currentShape == options[i].type);
        bool isHovered = (g_hoveredShapeFlyoutItem == i);

        // Active button background matching main toolbar active pill (0.20f, 0.32f, 0.44f, 0.95f)
        if (isSelected) {
            ID2D1SolidColorBrush* pPillBg = (isHovered && pActiveHoverBrush) ? pActiveHoverBrush : pActiveBrush;
            if (pPillBg) pRT->FillRoundedRectangle(roundItem, pPillBg);
        }
        else if (isHovered && pHoverBrush) {
            pRT->FillRoundedRectangle(roundItem, pHoverBrush);
        }

        ID2D1SolidColorBrush* pItemIconBrush = isSelected ? pActiveAccentBrush : pTextBrush;
        ID2D1SolidColorBrush* pItemTextBrush = isSelected ? pActiveAccentBrush : pTextBrush;
        ID2D1SolidColorBrush* pItemKeyBrush  = isSelected ? pActiveAccentBrush : pKeyBrush;

        // Active Checkmark (\uE73E CheckMark)
        if (isSelected && g_pIconFormat && pActiveAccentBrush) {
            D2D1_RECT_F checkR = D2D1::RectF(itemR.left + 4.0f * scale, itemTop, itemR.left + 20.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, L"\uE73E", g_pIconFormat, checkR, pActiveAccentBrush);
        }

        // Icon
        if (options[i].type == ShapeType::Triangle) {
            D2D1_RECT_F iconR = D2D1::RectF(itemR.left + 22.0f * scale, itemTop, itemR.left + 42.0f * scale, itemTop + itemH);
            float cx = (iconR.left + iconR.right) * 0.5f;
            float cy = (iconR.top + iconR.bottom) * 0.5f;
            float triH = 13.0f * scale;
            float triW = 14.0f * scale;
            D2D1_POINT_2F p1 = D2D1::Point2F(cx, cy - triH * 0.5f);
            D2D1_POINT_2F p2 = D2D1::Point2F(cx - triW * 0.5f, cy + triH * 0.5f);
            D2D1_POINT_2F p3 = D2D1::Point2F(cx + triW * 0.5f, cy + triH * 0.5f);
            if (pItemIconBrush) {
                pRT->DrawLine(p1, p2, pItemIconBrush, 1.4f, g_pRoundStrokeStyle);
                pRT->DrawLine(p2, p3, pItemIconBrush, 1.4f, g_pRoundStrokeStyle);
                pRT->DrawLine(p3, p1, pItemIconBrush, 1.4f, g_pRoundStrokeStyle);
            }
        }
        else if (g_pIconFormat && pItemIconBrush) {
            D2D1_RECT_F iconR = D2D1::RectF(itemR.left + 22.0f * scale, itemTop, itemR.left + 42.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, options[i].icon, g_pIconFormat, iconR, pItemIconBrush);
        }

        // Name
        if (g_pMenuTextFormat && pItemTextBrush) {
            D2D1_RECT_F textR = D2D1::RectF(itemR.left + 46.0f * scale, itemTop, itemR.right - 28.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, options[i].name, g_pMenuTextFormat, textR, pItemTextBrush);
        }

        // Shortcut Key Badge
        if (g_pMenuKeyFormat && pItemKeyBrush) {
            D2D1_RECT_F keyR = D2D1::RectF(itemR.right - 26.0f * scale, itemTop, itemR.right - 6.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, options[i].key, g_pMenuKeyFormat, keyR, pItemKeyBrush);
        }
    }

    SafeRelease(pActiveAccentBrush);
    SafeRelease(pHoverBrush);
    SafeRelease(pActiveHoverBrush);
    SafeRelease(pActiveBrush);
    SafeRelease(pKeyBrush);
    SafeRelease(pTextBrush);
    SafeRelease(pShadowBrush);
    SafeRelease(pRimBrush);
    SafeRelease(pBorderBrush);
    SafeRelease(pBgBrush);
}

void RebuildGridBrush() {
    if (g_pGridBrush) {
        g_pGridBrush->Release();
        g_pGridBrush = nullptr;
    }
    if (g_gridStyle == GridStyle::None || !g_pRenderTarget) return;

    int S = (int)g_gridDensity;
    if (S < 8) S = 48;

    ID2D1BitmapRenderTarget* pBitmapRT = nullptr;
    HRESULT hr = g_pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF((float)S, (float)S), &pBitmapRT);
    if (SUCCEEDED(hr) && pBitmapRT) {
        pBitmapRT->BeginDraw();
        pBitmapRT->Clear(D2D1::ColorF(0, 0, 0, 0));

        bool isWhiteboard = (g_canvasBg == CanvasBg::Whiteboard);

        if (g_gridStyle == GridStyle::DotGrid) {
            ID2D1SolidColorBrush* pDotBrush = nullptr;
            D2D1_COLOR_F dotColor = isWhiteboard
                ? D2D1::ColorF(0.20f, 0.25f, 0.35f, 0.25f)
                : D2D1::ColorF(0.55f, 0.72f, 0.95f, 0.38f);
            pBitmapRT->CreateSolidColorBrush(dotColor, &pDotBrush);
            if (pDotBrush) {
                float r = (S >= 48) ? 1.25f : 1.0f;
                pBitmapRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F((float)S * 0.5f, (float)S * 0.5f), r, r), pDotBrush);
                pDotBrush->Release();
            }
        }
        else if (g_gridStyle == GridStyle::GraphLines) {
            ID2D1SolidColorBrush* pLineBrush = nullptr;
            D2D1_COLOR_F lineColor = isWhiteboard
                ? D2D1::ColorF(0.20f, 0.25f, 0.35f, 0.15f)
                : D2D1::ColorF(0.50f, 0.68f, 0.90f, 0.22f);
            pBitmapRT->CreateSolidColorBrush(lineColor, &pLineBrush);
            if (pLineBrush) {
                pBitmapRT->DrawLine(D2D1::Point2F(0.0f, 0.5f), D2D1::Point2F((float)S, 0.5f), pLineBrush, 1.0f);
                pBitmapRT->DrawLine(D2D1::Point2F(0.5f, 0.0f), D2D1::Point2F(0.5f, (float)S), pLineBrush, 1.0f);
                pLineBrush->Release();
            }
        }

        pBitmapRT->EndDraw();

        ID2D1Bitmap* pTileBitmap = nullptr;
        if (SUCCEEDED(pBitmapRT->GetBitmap(&pTileBitmap)) && pTileBitmap) {
            D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
                D2D1_EXTEND_MODE_WRAP,
                D2D1_EXTEND_MODE_WRAP,
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
            );
            g_pRenderTarget->CreateBitmapBrush(pTileBitmap, brushProps, &g_pGridBrush);
            pTileBitmap->Release();
        }
        pBitmapRT->Release();
    }
}

void DrawGridFlyout(ID2D1HwndRenderTarget* pRT) {
    if (!g_gridFlyoutOpen || !g_settings.showBottomToolbar) return;

    // 1. Locate the Grid button (id 6) on the toolbar
    float btnAbsLeft = 0, btnAbsRight = 0;
    bool foundBtn = false;
    for (const auto& btn : g_toolbarButtons) {
        if (btn.id == 6) {
            btnAbsLeft = btn.rect.left;
            btnAbsRight = btn.rect.right;
            foundBtn = true;
            break;
        }
    }
    if (!foundBtn) return;

    float scale = GetDpiScaleAtPoint((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top);
    const float flyoutW = 188.0f * scale;
    const float itemH = 30.0f * scale;
    const float padY = 6.0f * scale;
    const float divH = 8.0f * scale;
    const float flyoutH = padY * 2.0f + itemH * 6 + divH;

    float monL = 0, monT = 0, monR = 0, monB = 0;
    GetMonitorBoundsAt((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top, monL, monT, monR, monB);

    float flyoutX = (btnAbsLeft + btnAbsRight) * 0.5f - flyoutW * 0.5f;
    if (flyoutX < monL + 8.0f * scale) flyoutX = monL + 8.0f * scale;
    if (flyoutX + flyoutW > monR - 8.0f * scale) {
        flyoutX = monR - flyoutW - 8.0f * scale;
    }

    float flyoutY = g_toolbarRect.top - flyoutH - 8.0f * scale;
    bool showAbove = true;
    if (flyoutY < monT + 8.0f * scale) {
        flyoutY = g_toolbarRect.bottom + 8.0f * scale;
        showAbove = false;
    }
    if (flyoutY + flyoutH > monB - 8.0f * scale) {
        flyoutY = monB - flyoutH - 8.0f * scale;
    }

    g_gridFlyoutRect = D2D1::RectF(flyoutX, flyoutY, flyoutX + flyoutW, flyoutY + flyoutH);

    // 2. Acrylic Glassmorphism chassis & styling consistent with main toolbar
    ID2D1SolidColorBrush* pBgBrush = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pRimBrush = nullptr;
    ID2D1SolidColorBrush* pShadowBrush = nullptr;
    ID2D1SolidColorBrush* pTextBrush = nullptr;
    ID2D1SolidColorBrush* pKeyBrush = nullptr;
    ID2D1SolidColorBrush* pActiveBrush = nullptr;
    ID2D1SolidColorBrush* pActiveHoverBrush = nullptr;
    ID2D1SolidColorBrush* pHoverBrush = nullptr;
    ID2D1SolidColorBrush* pActiveAccentBrush = nullptr;
    ID2D1SolidColorBrush* pDivBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.08f, 0.10f, 0.14f, 0.96f), &pBgBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.26f, 0.34f, 1.00f), &pBorderBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.48f, 0.60f, 0.50f), &pRimBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f), &pShadowBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.88f, 0.92f, 1.00f), &pTextBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.55f, 0.62f, 0.72f, 0.85f), &pKeyBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.20f, 0.32f, 0.44f, 0.95f), &pActiveBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.24f, 0.36f, 0.48f, 0.98f), &pActiveHoverBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.18f, 0.22f, 0.30f, 0.85f), &pHoverBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.90f, 0.75f, 1.00f), &pActiveAccentBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.26f, 0.34f, 0.80f), &pDivBrush);

    // Drop shadow
    if (pShadowBrush) {
        D2D1_ROUNDED_RECT shadowR = D2D1::RoundedRect(
            D2D1::RectF(flyoutX + 2.0f * scale, flyoutY + 2.0f * scale, flyoutX + flyoutW + 2.0f * scale, flyoutY + flyoutH + 2.0f * scale),
            8.0f * scale, 8.0f * scale
        );
        pRT->FillRoundedRectangle(shadowR, pShadowBrush);
    }

    // Modal Background
    D2D1_ROUNDED_RECT modalR = D2D1::RoundedRect(g_gridFlyoutRect, 8.0f * scale, 8.0f * scale);
    if (pBgBrush) pRT->FillRoundedRectangle(modalR, pBgBrush);
    if (pBorderBrush) pRT->DrawRoundedRectangle(modalR, pBorderBrush, 1.2f);

    // Specular top rim highlight (matching main toolbar)
    if (pRimBrush) {
        pRT->DrawLine(
            D2D1::Point2F(flyoutX + 8.0f * scale, flyoutY + 1.5f),
            D2D1::Point2F(flyoutX + flyoutW - 8.0f * scale, flyoutY + 1.5f),
            pRimBrush, 1.0f
        );
    }

    // Downward caret pointing to toolbar button
    if (showAbove && pBgBrush && pBorderBrush) {
        float tipX = (btnAbsLeft + btnAbsRight) * 0.5f;
        float caretY = flyoutY + flyoutH;
        D2D1_POINT_2F p1 = D2D1::Point2F(tipX - 6.0f * scale, caretY - 0.5f);
        D2D1_POINT_2F p2 = D2D1::Point2F(tipX, caretY + 5.5f * scale);
        D2D1_POINT_2F p3 = D2D1::Point2F(tipX + 6.0f * scale, caretY - 0.5f);

        ID2D1PathGeometry* pCaretGeo = nullptr;
        if (g_pD2DFactory && SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pCaretGeo))) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pCaretGeo->Open(&pSink))) {
                pSink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
                pSink->AddLine(p2);
                pSink->AddLine(p3);
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                pSink->Close();
                SafeRelease(pSink);

                pRT->FillGeometry(pCaretGeo, pBgBrush);
                pRT->DrawLine(p1, p2, pBorderBrush, 1.2f);
                pRT->DrawLine(p2, p3, pBorderBrush, 1.2f);
            }
            SafeRelease(pCaretGeo);
        }
    }

    struct GridOptionItem {
        const wchar_t* name;
        const wchar_t* key;
        bool isSelected;
    };
    GridOptionItem items[6] = {
        { L"Grid Off",      L"",  g_gridStyle == GridStyle::None },
        { L"Dot Grid",      L"",  g_gridStyle == GridStyle::DotGrid },
        { L"Graph Paper",   L"G", g_gridStyle == GridStyle::GraphLines },
        { L"Fine  (24px)",   L"",  g_gridDensity == GridDensity::Fine },
        { L"Medium  (48px)", L"",  g_gridDensity == GridDensity::Medium },
        { L"Coarse  (96px)", L"",  g_gridDensity == GridDensity::Coarse }
    };

    for (int i = 0; i < 6; ++i) {
        float itemTop = flyoutY + padY + i * itemH + (i >= 3 ? divH : 0.0f);
        D2D1_RECT_F itemR = D2D1::RectF(flyoutX + 5.0f * scale, itemTop, flyoutX + flyoutW - 5.0f * scale, itemTop + itemH);
        D2D1_ROUNDED_RECT roundItem = D2D1::RoundedRect(itemR, 4.0f * scale, 4.0f * scale);

        bool isSelected = items[i].isSelected;
        bool isHovered = (g_hoveredGridFlyoutItem == i);

        // Active button background matching main toolbar active pill (0.20f, 0.32f, 0.44f, 0.95f)
        if (isSelected) {
            ID2D1SolidColorBrush* pPillBg = (isHovered && pActiveHoverBrush) ? pActiveHoverBrush : pActiveBrush;
            if (pPillBg) pRT->FillRoundedRectangle(roundItem, pPillBg);
        }
        else if (isHovered && pHoverBrush) {
            pRT->FillRoundedRectangle(roundItem, pHoverBrush);
        }

        ID2D1SolidColorBrush* pItemTextBrush = isSelected ? pActiveAccentBrush : pTextBrush;
        ID2D1SolidColorBrush* pItemKeyBrush  = isSelected ? pActiveAccentBrush : pKeyBrush;

        // Active Checkmark (\uE73E CheckMark)
        if (isSelected && g_pIconFormat && pActiveAccentBrush) {
            D2D1_RECT_F checkR = D2D1::RectF(itemR.left + 4.0f * scale, itemTop, itemR.left + 22.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, L"\uE73E", g_pIconFormat, checkR, pActiveAccentBrush);
        }

        // Name
        if (g_pMenuTextFormat && pItemTextBrush) {
            float textRight = (wcslen(items[i].key) > 0) ? (itemR.right - 28.0f * scale) : (itemR.right - 8.0f * scale);
            D2D1_RECT_F textR = D2D1::RectF(itemR.left + 26.0f * scale, itemTop, textRight, itemTop + itemH);
            DrawCachedText(pRT, items[i].name, g_pMenuTextFormat, textR, pItemTextBrush);
        }

        // Shortcut Key Badge
        if (g_pMenuKeyFormat && pItemKeyBrush && wcslen(items[i].key) > 0) {
            D2D1_RECT_F keyR = D2D1::RectF(itemR.right - 26.0f * scale, itemTop, itemR.right - 6.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, items[i].key, g_pMenuKeyFormat, keyR, pItemKeyBrush);
        }

        // Divider between Style (0..2) and Density (3..5)
        if (i == 2 && pDivBrush) {
            float divY = itemTop + itemH + divH * 0.5f;
            pRT->DrawLine(D2D1::Point2F(flyoutX + 10.0f * scale, divY), D2D1::Point2F(flyoutX + flyoutW - 10.0f * scale, divY), pDivBrush, 1.0f);
        }
    }

    SafeRelease(pDivBrush);
    SafeRelease(pActiveAccentBrush);
    SafeRelease(pHoverBrush);
    SafeRelease(pActiveHoverBrush);
    SafeRelease(pActiveBrush);
    SafeRelease(pKeyBrush);
    SafeRelease(pTextBrush);
    SafeRelease(pShadowBrush);
    SafeRelease(pRimBrush);
    SafeRelease(pBorderBrush);
    SafeRelease(pBgBrush);
}

void DrawBackdropFlyout(ID2D1HwndRenderTarget* pRT) {
    if (!g_backdropFlyoutOpen || !g_settings.showBottomToolbar) return;

    // 1. Locate the Whiteboard button (id 8) on the toolbar
    float btnAbsLeft = 0, btnAbsRight = 0;
    bool foundBtn = false;
    for (const auto& btn : g_toolbarButtons) {
        if (btn.id == 8) {
            btnAbsLeft = btn.rect.left;
            btnAbsRight = btn.rect.right;
            foundBtn = true;
            break;
        }
    }
    if (!foundBtn) return;

    const auto& monitors = GetSystemMonitorList();
    bool hasMultipleMonitors = (monitors.size() > 1);

    struct BackdropOptionItem {
        std::wstring name;
        std::wstring key;
        bool isSelected;
    };

    std::vector<BackdropOptionItem> items;
    // Section 1: Backdrop Style (3 options)
    items.push_back({ L"Transparent (Off)", L"K", g_canvasBg == CanvasBg::Transparent });
    items.push_back({ L"Whiteboard (Paper)", L"", g_canvasBg == CanvasBg::Whiteboard });
    items.push_back({ L"Blackboard (Slate)", L"", g_canvasBg == CanvasBg::Blackboard });

    if (hasMultipleMonitors) {
        // Section 2: Target Display
        items.push_back({ L"Active Screen (Cursor)", L"", g_canvasScope == CanvasMonitorScope::ActiveCursor });
        items.push_back({ L"Primary Screen", L"", g_canvasScope == CanvasMonitorScope::Primary });
        for (size_t i = 0; i < monitors.size(); ++i) {
            CanvasMonitorScope scope = (CanvasMonitorScope)(i + 1);
            items.push_back({ monitors[i].name, L"", g_canvasScope == scope });
        }
        items.push_back({ L"All Screens (Span All)", L"", g_canvasScope == CanvasMonitorScope::AllMonitors });
    }

    float scale = GetDpiScaleAtPoint((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top);
    const float flyoutW = 216.0f * scale;
    const float itemH = 30.0f * scale;
    const float padY = 6.0f * scale;
    const float divH = 8.0f * scale;
    int totalItems = (int)items.size();
    const float flyoutH = padY * 2.0f + itemH * (float)totalItems + (hasMultipleMonitors ? divH : 0.0f);

    float monL = 0, monT = 0, monR = 0, monB = 0;
    GetMonitorBoundsAt((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top, monL, monT, monR, monB);

    float flyoutX = (btnAbsLeft + btnAbsRight) * 0.5f - flyoutW * 0.5f;
    if (flyoutX < monL + 8.0f * scale) flyoutX = monL + 8.0f * scale;
    if (flyoutX + flyoutW > monR - 8.0f * scale) {
        flyoutX = monR - flyoutW - 8.0f * scale;
    }

    float flyoutY = g_toolbarRect.top - flyoutH - 8.0f * scale;
    bool showAbove = true;
    if (flyoutY < monT + 8.0f * scale) {
        flyoutY = g_toolbarRect.bottom + 8.0f * scale;
        showAbove = false;
    }
    if (flyoutY + flyoutH > monB - 8.0f * scale) {
        flyoutY = monB - flyoutH - 8.0f * scale;
    }

    g_backdropFlyoutRect = D2D1::RectF(flyoutX, flyoutY, flyoutX + flyoutW, flyoutY + flyoutH);

    // Brushes
    ID2D1SolidColorBrush* pBgBrush = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pRimBrush = nullptr;
    ID2D1SolidColorBrush* pShadowBrush = nullptr;
    ID2D1SolidColorBrush* pTextBrush = nullptr;
    ID2D1SolidColorBrush* pKeyBrush = nullptr;
    ID2D1SolidColorBrush* pActiveBrush = nullptr;
    ID2D1SolidColorBrush* pActiveHoverBrush = nullptr;
    ID2D1SolidColorBrush* pHoverBrush = nullptr;
    ID2D1SolidColorBrush* pActiveAccentBrush = nullptr;
    ID2D1SolidColorBrush* pDivBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.08f, 0.10f, 0.14f, 0.96f), &pBgBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.26f, 0.34f, 1.00f), &pBorderBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.48f, 0.60f, 0.50f), &pRimBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f), &pShadowBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.88f, 0.92f, 1.00f), &pTextBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.55f, 0.62f, 0.72f, 0.85f), &pKeyBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.20f, 0.32f, 0.44f, 0.95f), &pActiveBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.24f, 0.36f, 0.48f, 0.98f), &pActiveHoverBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.18f, 0.22f, 0.30f, 0.85f), &pHoverBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.90f, 0.75f, 1.00f), &pActiveAccentBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.26f, 0.34f, 0.80f), &pDivBrush);

    // Drop shadow
    if (pShadowBrush) {
        D2D1_ROUNDED_RECT shadowR = D2D1::RoundedRect(
            D2D1::RectF(flyoutX + 2.0f * scale, flyoutY + 2.0f * scale, flyoutX + flyoutW + 2.0f * scale, flyoutY + flyoutH + 2.0f * scale),
            8.0f * scale, 8.0f * scale
        );
        pRT->FillRoundedRectangle(shadowR, pShadowBrush);
    }

    // Modal Background
    D2D1_ROUNDED_RECT modalR = D2D1::RoundedRect(g_backdropFlyoutRect, 8.0f * scale, 8.0f * scale);
    if (pBgBrush) pRT->FillRoundedRectangle(modalR, pBgBrush);
    if (pBorderBrush) pRT->DrawRoundedRectangle(modalR, pBorderBrush, 1.2f);

    // Specular top rim highlight
    if (pRimBrush) {
        pRT->DrawLine(
            D2D1::Point2F(flyoutX + 8.0f * scale, flyoutY + 1.5f),
            D2D1::Point2F(flyoutX + flyoutW - 8.0f * scale, flyoutY + 1.5f),
            pRimBrush, 1.0f
        );
    }

    // Downward caret pointing to button id 8
    if (showAbove && pBgBrush && pBorderBrush) {
        float tipX = (btnAbsLeft + btnAbsRight) * 0.5f;
        float caretY = flyoutY + flyoutH;
        D2D1_POINT_2F p1 = D2D1::Point2F(tipX - 6.0f * scale, caretY - 0.5f);
        D2D1_POINT_2F p2 = D2D1::Point2F(tipX, caretY + 5.5f * scale);
        D2D1_POINT_2F p3 = D2D1::Point2F(tipX + 6.0f * scale, caretY - 0.5f);

        ID2D1PathGeometry* pCaretGeo = nullptr;
        if (g_pD2DFactory && SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pCaretGeo))) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pCaretGeo->Open(&pSink))) {
                pSink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
                pSink->AddLine(p2);
                pSink->AddLine(p3);
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                pSink->Close();
                SafeRelease(pSink);

                pRT->FillGeometry(pCaretGeo, pBgBrush);
                pRT->DrawLine(p1, p2, pBorderBrush, 1.2f);
                pRT->DrawLine(p2, p3, pBorderBrush, 1.2f);
            }
            SafeRelease(pCaretGeo);
        }
    }

    // Draw Items
    for (int i = 0; i < totalItems; ++i) {
        float itemTop = flyoutY + padY + (float)i * itemH + (hasMultipleMonitors && i >= 3 ? divH : 0.0f);
        D2D1_RECT_F itemR = D2D1::RectF(flyoutX + 5.0f * scale, itemTop, flyoutX + flyoutW - 5.0f * scale, itemTop + itemH);
        D2D1_ROUNDED_RECT roundItem = D2D1::RoundedRect(itemR, 4.0f * scale, 4.0f * scale);

        bool isSelected = items[i].isSelected;
        bool isHovered = (g_hoveredBackdropFlyoutItem == i);

        if (isSelected) {
            ID2D1SolidColorBrush* pPillBg = (isHovered && pActiveHoverBrush) ? pActiveHoverBrush : pActiveBrush;
            if (pPillBg) pRT->FillRoundedRectangle(roundItem, pPillBg);
        }
        else if (isHovered && pHoverBrush) {
            pRT->FillRoundedRectangle(roundItem, pHoverBrush);
        }

        ID2D1SolidColorBrush* pItemTextBrush = isSelected ? pActiveAccentBrush : pTextBrush;
        ID2D1SolidColorBrush* pItemKeyBrush  = isSelected ? pActiveAccentBrush : pKeyBrush;

        // Active Checkmark (\uE73E CheckMark)
        if (isSelected && g_pIconFormat && pActiveAccentBrush) {
            D2D1_RECT_F checkR = D2D1::RectF(itemR.left + 4.0f * scale, itemTop, itemR.left + 22.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, L"\uE73E", g_pIconFormat, checkR, pActiveAccentBrush);
        }

        // Name
        if (g_pMenuTextFormat && pItemTextBrush) {
            float textRight = (items[i].key.length() > 0) ? (itemR.right - 28.0f * scale) : (itemR.right - 8.0f * scale);
            D2D1_RECT_F textR = D2D1::RectF(itemR.left + 26.0f * scale, itemTop, textRight, itemTop + itemH);
            DrawCachedText(pRT, items[i].name, g_pMenuTextFormat, textR, pItemTextBrush);
        }

        // Shortcut Key Badge
        if (g_pMenuKeyFormat && pItemKeyBrush && items[i].key.length() > 0) {
            D2D1_RECT_F keyR = D2D1::RectF(itemR.right - 26.0f * scale, itemTop, itemR.right - 6.0f * scale, itemTop + itemH);
            DrawCachedText(pRT, items[i].key, g_pMenuKeyFormat, keyR, pItemKeyBrush);
        }

        // Divider between Backdrop Style (0..2) and Target Displays (3+)
        if (hasMultipleMonitors && i == 2 && pDivBrush) {
            float divY = itemTop + itemH + divH * 0.5f;
            pRT->DrawLine(D2D1::Point2F(flyoutX + 10.0f * scale, divY), D2D1::Point2F(flyoutX + flyoutW - 10.0f * scale, divY), pDivBrush, 1.0f);
        }
    }

    SafeRelease(pDivBrush);
    SafeRelease(pActiveAccentBrush);
    SafeRelease(pHoverBrush);
    SafeRelease(pActiveHoverBrush);
    SafeRelease(pActiveBrush);
    SafeRelease(pKeyBrush);
    SafeRelease(pTextBrush);
    SafeRelease(pShadowBrush);
    SafeRelease(pRimBrush);
    SafeRelease(pBorderBrush);
    SafeRelease(pBgBrush);
}

void DrawColorFlyout(ID2D1HwndRenderTarget* pRT) {
    if (!g_colorFlyoutOpen || !g_settings.showBottomToolbar) return;

    // 1. Locate the Custom Color button (id 104) on the toolbar
    float btnAbsLeft = 0, btnAbsRight = 0;
    bool foundBtn = false;
    for (const auto& btn : g_toolbarButtons) {
        if (btn.id == 104) {
            btnAbsLeft = btn.rect.left;
            btnAbsRight = btn.rect.right;
            foundBtn = true;
            break;
        }
    }
    if (!foundBtn && g_toolbarCollapsed) {
        btnAbsLeft = g_toolbarRect.left;
        btnAbsRight = g_toolbarRect.right;
        foundBtn = true;
    }
    if (!foundBtn) return;

    float scale = GetDpiScaleAtPoint((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top);
    const float flyoutW = 244.0f * scale;
    const float flyoutH = 312.0f * scale;

    float monL = 0, monT = 0, monR = 0, monB = 0;
    GetMonitorBoundsAt((btnAbsLeft + btnAbsRight) * 0.5f, g_toolbarRect.top, monL, monT, monR, monB);

    float flyoutX = (btnAbsLeft + btnAbsRight) * 0.5f - flyoutW * 0.5f;
    if (flyoutX < monL + 8.0f * scale) flyoutX = monL + 8.0f * scale;
    if (flyoutX + flyoutW > monR - 8.0f * scale) {
        flyoutX = monR - flyoutW - 8.0f * scale;
    }

    float flyoutY = g_toolbarRect.top - flyoutH - 8.0f * scale;
    if (flyoutY < monT + 8.0f * scale) {
        flyoutY = g_toolbarRect.bottom + 8.0f * scale;
    }
    if (flyoutY + flyoutH > monB - 8.0f * scale) {
        flyoutY = monB - flyoutH - 8.0f * scale;
    }

    g_colorFlyoutRect = D2D1::RectF(flyoutX, flyoutY, flyoutX + flyoutW, flyoutY + flyoutH);

    // Chassis brushes
    ID2D1SolidColorBrush* pBgBrush = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pRimBrush = nullptr;
    ID2D1SolidColorBrush* pShadowBrush = nullptr;
    ID2D1SolidColorBrush* pTextBrush = nullptr;
    ID2D1SolidColorBrush* pMintBrush = nullptr;
    ID2D1SolidColorBrush* pCardBgBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(0.08f, 0.10f, 0.14f, 0.96f), &pBgBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.26f, 0.34f, 1.00f), &pBorderBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.48f, 0.60f, 0.50f), &pRimBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.45f), &pShadowBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.88f, 0.92f, 0.96f, 1.00f), &pTextBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.40f, 0.90f, 0.75f, 1.00f), &pMintBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.13f, 0.16f, 0.22f, 0.90f), &pCardBgBrush);

    // Drop shadow
    if (pShadowBrush) {
        D2D1_ROUNDED_RECT shadowR = D2D1::RoundedRect(
            D2D1::RectF(flyoutX + 2.0f * scale, flyoutY + 2.0f * scale, flyoutX + flyoutW + 2.0f * scale, flyoutY + flyoutH + 2.0f * scale),
            8.0f * scale, 8.0f * scale
        );
        pRT->FillRoundedRectangle(shadowR, pShadowBrush);
    }

    // Modal background
    D2D1_ROUNDED_RECT modalR = D2D1::RoundedRect(g_colorFlyoutRect, 8.0f * scale, 8.0f * scale);
    if (pBgBrush) pRT->FillRoundedRectangle(modalR, pBgBrush);
    if (pBorderBrush) pRT->DrawRoundedRectangle(modalR, pBorderBrush, 1.2f);
    if (pRimBrush) {
        pRT->DrawLine(
            D2D1::Point2F(flyoutX + 8.0f * scale, flyoutY + 1.5f),
            D2D1::Point2F(flyoutX + flyoutW - 8.0f * scale, flyoutY + 1.5f),
            pRimBrush, 1.0f
        );
    }

    const float padX = 14.0f * scale;
    const float contentW = flyoutW - padX * 2.0f;
    float curY = flyoutY + 12.0f * scale;

    // 2. Recent Colors Bar (Top - 5 Swatches)
    const float swatchDiam = 22.0f * scale;
    const float swatchGap = (contentW - 5.0f * swatchDiam) / 4.0f;
    for (int k = 0; k < 5; ++k) {
        float sx = flyoutX + padX + k * (swatchDiam + swatchGap) + swatchDiam * 0.5f;
        float sy = curY + swatchDiam * 0.5f;
        D2D1_COLOR_F c = (k < (int)g_recentColors.size()) ? g_recentColors[k] : D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f);

        bool isActive = (std::abs(c.r - g_activeColor.r) < 0.02f &&
                         std::abs(c.g - g_activeColor.g) < 0.02f &&
                         std::abs(c.b - g_activeColor.b) < 0.02f &&
                         std::abs(c.a - g_activeColor.a) < 0.03f);
        bool isHov = (g_hoveredRecentSwatch == k);

        ID2D1SolidColorBrush* pSwBrush = nullptr;
        pRT->CreateSolidColorBrush(c, &pSwBrush);
        if (pSwBrush) {
            if (isActive && pMintBrush) {
                pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), swatchDiam * 0.5f + 2.5f * scale, swatchDiam * 0.5f + 2.5f * scale), pMintBrush, 2.0f);
            }
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), swatchDiam * 0.5f, swatchDiam * 0.5f), pSwBrush);
            ID2D1SolidColorBrush* pSwBorder = nullptr;
            pRT->CreateSolidColorBrush(isHov ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.9f) : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.4f), &pSwBorder);
            if (pSwBorder) {
                pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), swatchDiam * 0.5f, swatchDiam * 0.5f), pSwBorder, isHov ? 1.5f : 1.0f);
                SafeRelease(pSwBorder);
            }
            SafeRelease(pSwBrush);
        }
    }
    curY += swatchDiam + 10.0f * scale;

    // 3. 2D Saturation / Value Canvas
    const float canvasH = 118.0f * scale;
    D2D1_RECT_F canvasRect = D2D1::RectF(flyoutX + padX, curY, flyoutX + padX + contentW, curY + canvasH);

    // Horizontal linear gradient: White to pure Hue
    ID2D1LinearGradientBrush* pHorizBrush = nullptr;
    ID2D1GradientStopCollection* pHorizStops = nullptr;
    D2D1_GRADIENT_STOP hStops[2];
    hStops[0].position = 0.0f;
    hStops[0].color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
    hStops[1].position = 1.0f;
    hStops[1].color = HSVtoRGB(g_customColor.hue, 1.0f, 1.0f, 1.0f);
    if (SUCCEEDED(pRT->CreateGradientStopCollection(hStops, 2, &pHorizStops))) {
        pRT->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(canvasRect.left, canvasRect.top), D2D1::Point2F(canvasRect.right, canvasRect.top)),
            pHorizStops,
            &pHorizBrush
        );
        SafeRelease(pHorizStops);
    }
    if (pHorizBrush) {
        D2D1_ROUNDED_RECT cr = D2D1::RoundedRect(canvasRect, 6.0f * scale, 6.0f * scale);
        pRT->FillRoundedRectangle(cr, pHorizBrush);
        SafeRelease(pHorizBrush);
    }

    // Vertical linear gradient: Transparent to Black
    ID2D1LinearGradientBrush* pVertBrush = nullptr;
    ID2D1GradientStopCollection* pVertStops = nullptr;
    D2D1_GRADIENT_STOP vStops[2];
    vStops[0].position = 0.0f;
    vStops[0].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
    vStops[1].position = 1.0f;
    vStops[1].color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
    if (SUCCEEDED(pRT->CreateGradientStopCollection(vStops, 2, &pVertStops))) {
        pRT->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(canvasRect.left, canvasRect.top), D2D1::Point2F(canvasRect.left, canvasRect.bottom)),
            pVertStops,
            &pVertBrush
        );
        SafeRelease(pVertStops);
    }
    if (pVertBrush) {
        D2D1_ROUNDED_RECT cr = D2D1::RoundedRect(canvasRect, 6.0f * scale, 6.0f * scale);
        pRT->FillRoundedRectangle(cr, pVertBrush);
        SafeRelease(pVertBrush);
    }

    // Canvas border
    D2D1_ROUNDED_RECT canvasBorderR = D2D1::RoundedRect(canvasRect, 6.0f * scale, 6.0f * scale);
    pRT->DrawRoundedRectangle(canvasBorderR, pBorderBrush, 1.0f);

    // Crosshair Reticle Ring
    float retX = canvasRect.left + g_customColor.sat * contentW;
    float retY = canvasRect.top + (1.0f - g_customColor.val) * canvasH;
    retX = std::max(canvasRect.left, std::min(canvasRect.right, retX));
    retY = std::max(canvasRect.top, std::min(canvasRect.bottom, retY));

    ID2D1SolidColorBrush* pRetShadow = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.75f), &pRetShadow);
    if (pRetShadow) {
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(retX, retY), 7.0f * scale, 7.0f * scale), pRetShadow, 2.5f * scale);
        SafeRelease(pRetShadow);
    }
    ID2D1SolidColorBrush* pRetWhite = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pRetWhite);
    if (pRetWhite) {
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(retX, retY), 6.0f * scale, 6.0f * scale), pRetWhite, 2.0f * scale);
        SafeRelease(pRetWhite);
    }

    curY += canvasH + 10.0f * scale;

    // 4. Rainbow Hue Slider Track
    const float trackH = 12.0f * scale;
    D2D1_RECT_F hueRect = D2D1::RectF(flyoutX + padX, curY, flyoutX + padX + contentW, curY + trackH);
    D2D1_GRADIENT_STOP hueStops[7] = {
        { 0.000f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f) },
        { 0.166f, D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f) },
        { 0.333f, D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f) },
        { 0.500f, D2D1::ColorF(0.0f, 1.0f, 1.0f, 1.0f) },
        { 0.666f, D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f) },
        { 0.833f, D2D1::ColorF(1.0f, 0.0f, 1.0f, 1.0f) },
        { 1.000f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f) }
    };
    ID2D1GradientStopCollection* pHueColl = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(hueStops, 7, &pHueColl))) {
        ID2D1LinearGradientBrush* pHueBrush = nullptr;
        pRT->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(hueRect.left, hueRect.top), D2D1::Point2F(hueRect.right, hueRect.top)),
            pHueColl, &pHueBrush
        );
        if (pHueBrush) {
            D2D1_ROUNDED_RECT hr = D2D1::RoundedRect(hueRect, 6.0f * scale, 6.0f * scale);
            pRT->FillRoundedRectangle(hr, pHueBrush);
            pRT->DrawRoundedRectangle(hr, pBorderBrush, 1.0f);
            SafeRelease(pHueBrush);
        }
        SafeRelease(pHueColl);
    }

    // Hue Thumb
    float hThumbX = hueRect.left + (g_customColor.hue / 360.0f) * contentW;
    float hThumbY = curY + trackH * 0.5f;
    hThumbX = std::max(hueRect.left, std::min(hueRect.right, hThumbX));

    ID2D1SolidColorBrush* pWhiteBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pWhiteBrush);
    ID2D1SolidColorBrush* pHueColorBrush = nullptr;
    pRT->CreateSolidColorBrush(HSVtoRGB(g_customColor.hue, 1.0f, 1.0f, 1.0f), &pHueColorBrush);
    if (pWhiteBrush) {
        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(hThumbX, hThumbY), 8.0f * scale, 8.0f * scale), pWhiteBrush);
        if (pHueColorBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(hThumbX, hThumbY), 5.5f * scale, 5.5f * scale), pHueColorBrush);
            SafeRelease(pHueColorBrush);
        }
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(hThumbX, hThumbY), 8.0f * scale, 8.0f * scale), pBorderBrush, 1.0f);
        SafeRelease(pWhiteBrush);
    }

    curY += trackH + 10.0f * scale;

    // 5. Opacity / Alpha Slider Track
    D2D1_RECT_F alphaRect = D2D1::RectF(flyoutX + padX, curY, flyoutX + padX + contentW, curY + trackH);
    D2D1_ROUNDED_RECT ar = D2D1::RoundedRect(alphaRect, 6.0f * scale, 6.0f * scale);

    ID2D1SolidColorBrush* pCheckDark = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.18f, 0.22f, 0.28f, 1.0f), &pCheckDark);
    if (pCheckDark) {
        pRT->FillRoundedRectangle(ar, pCheckDark);
        SafeRelease(pCheckDark);
    }

    D2D1_COLOR_F pureRGB = HSVtoRGB(g_customColor.hue, g_customColor.sat, g_customColor.val, 1.0f);
    D2D1_GRADIENT_STOP aStops[2];
    aStops[0].position = 0.0f;
    aStops[0].color = D2D1::ColorF(pureRGB.r, pureRGB.g, pureRGB.b, 0.05f);
    aStops[1].position = 1.0f;
    aStops[1].color = D2D1::ColorF(pureRGB.r, pureRGB.g, pureRGB.b, 1.0f);
    ID2D1GradientStopCollection* pAColl = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(aStops, 2, &pAColl))) {
        ID2D1LinearGradientBrush* pABrush = nullptr;
        pRT->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(alphaRect.left, alphaRect.top), D2D1::Point2F(alphaRect.right, alphaRect.top)),
            pAColl, &pABrush
        );
        if (pABrush) {
            pRT->FillRoundedRectangle(ar, pABrush);
            pRT->DrawRoundedRectangle(ar, pBorderBrush, 1.0f);
            SafeRelease(pABrush);
        }
        SafeRelease(pAColl);
    }

    // Alpha Thumb
    float aThumbX = alphaRect.left + g_customColor.alpha * contentW;
    float aThumbY = curY + trackH * 0.5f;
    aThumbX = std::max(alphaRect.left, std::min(alphaRect.right, aThumbX));

    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pWhiteBrush);
    ID2D1SolidColorBrush* pCurColorBrush = nullptr;
    pRT->CreateSolidColorBrush(g_customColor.activeColor, &pCurColorBrush);
    if (pWhiteBrush) {
        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(aThumbX, aThumbY), 8.0f * scale, 8.0f * scale), pWhiteBrush);
        if (pCurColorBrush) {
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(aThumbX, aThumbY), 5.5f * scale, 5.5f * scale), pCurColorBrush);
        }
        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(aThumbX, aThumbY), 8.0f * scale, 8.0f * scale), pBorderBrush, 1.0f);
        SafeRelease(pWhiteBrush);
    }

    curY += trackH + 12.0f * scale;

    // 6. Bottom Tools Row (Hex readout, Preview Swatch, Eyedropper, Copy)
    const float rowH = 30.0f * scale;
    const float hexW = 92.0f * scale;
    const float swatchBoxW = 32.0f * scale;
    const float btnBoxW = 32.0f * scale;
    const float gap = 8.0f * scale;

    // Hex Box
    D2D1_RECT_F hexRect = D2D1::RectF(flyoutX + padX, curY, flyoutX + padX + hexW, curY + rowH);
    D2D1_ROUNDED_RECT hexR = D2D1::RoundedRect(hexRect, 4.0f * scale, 4.0f * scale);
    if (pCardBgBrush) pRT->FillRoundedRectangle(hexR, pCardBgBrush);
    pRT->DrawRoundedRectangle(hexR, pBorderBrush, 1.0f);

    std::wstring hexStr = ColorToHex(g_customColor.activeColor, false);
    if (g_pMenuKeyFormat && pTextBrush) {
        D2D1_RECT_F hexTextR = D2D1::RectF(hexRect.left + 6.0f * scale, hexRect.top + 6.0f * scale, hexRect.right - 4.0f * scale, hexRect.bottom - 4.0f * scale);
        pRT->DrawText(hexStr.c_str(), (UINT32)hexStr.length(), g_pMenuKeyFormat, hexTextR, pTextBrush);
    }

    // Preview Swatch Box
    float swatchLeft = hexRect.right + gap;
    D2D1_RECT_F prevRect = D2D1::RectF(swatchLeft, curY, swatchLeft + swatchBoxW, curY + rowH);
    D2D1_ROUNDED_RECT prevR = D2D1::RoundedRect(prevRect, 4.0f * scale, 4.0f * scale);
    if (pCurColorBrush) {
        pRT->FillRoundedRectangle(prevR, pCurColorBrush);
        SafeRelease(pCurColorBrush);
    }
    pRT->DrawRoundedRectangle(prevR, pBorderBrush, 1.0f);

    // Eyedropper Button
    float dropLeft = prevRect.right + gap;
    D2D1_RECT_F dropRect = D2D1::RectF(dropLeft, curY, dropLeft + btnBoxW, curY + rowH);
    D2D1_ROUNDED_RECT dropR = D2D1::RoundedRect(dropRect, 4.0f * scale, 4.0f * scale);
    bool dropHov = (g_hoveredColorStudioAction == 1);
    ID2D1SolidColorBrush* pBtnBg = nullptr;
    pRT->CreateSolidColorBrush(dropHov ? D2D1::ColorF(0.22f, 0.28f, 0.38f, 0.90f) : D2D1::ColorF(0.13f, 0.16f, 0.22f, 0.90f), &pBtnBg);
    if (pBtnBg) {
        pRT->FillRoundedRectangle(dropR, pBtnBg);
        SafeRelease(pBtnBg);
    }
    pRT->DrawRoundedRectangle(dropR, (g_isEyedropperActive && pMintBrush) ? pMintBrush : pBorderBrush, g_isEyedropperActive ? 1.8f : 1.0f);
    if (g_pIconFormat && pTextBrush) {
        DrawCachedText(pRT, L"\uEF3C", g_pIconFormat, dropRect,
            (g_isEyedropperActive && pMintBrush) ? pMintBrush : pTextBrush);
    }

    // Copy Hex Button
    float copyLeft = dropRect.right + gap;
    D2D1_RECT_F copyRect = D2D1::RectF(copyLeft, curY, copyLeft + btnBoxW, curY + rowH);
    D2D1_ROUNDED_RECT copyR = D2D1::RoundedRect(copyRect, 4.0f * scale, 4.0f * scale);
    bool copyHov = (g_hoveredColorStudioAction == 2);
    ID2D1SolidColorBrush* pCopyBg = nullptr;
    pRT->CreateSolidColorBrush(copyHov ? D2D1::ColorF(0.22f, 0.28f, 0.38f, 0.90f) : D2D1::ColorF(0.13f, 0.16f, 0.22f, 0.90f), &pCopyBg);
    if (pCopyBg) {
        pRT->FillRoundedRectangle(copyR, pCopyBg);
        SafeRelease(pCopyBg);
    }
    pRT->DrawRoundedRectangle(copyR, pBorderBrush, 1.0f);
    if (g_pIconFormat && pTextBrush) {
        DrawCachedText(pRT, L"\uE8C8", g_pIconFormat, copyRect, pTextBrush);
    }

    // Release chassis brushes
    SafeRelease(pBgBrush);
    SafeRelease(pBorderBrush);
    SafeRelease(pRimBrush);
    SafeRelease(pShadowBrush);
    SafeRelease(pTextBrush);
    SafeRelease(pMintBrush);
    SafeRelease(pCardBgBrush);
}

void DrawLaserTrail(ID2D1HwndRenderTarget* pRT) {
    if (g_laserStrokes.empty()) return;
    ULONGLONG now = GetTickCount64();
    ULONGLONG duration = (ULONGLONG)std::max(200, g_settings.laserTrailDuration);

    ID2D1SolidColorBrush* pGlowBrush = nullptr;
    ID2D1SolidColorBrush* pCoreBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.08f, 0.22f, 1.0f), &pGlowBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.92f, 0.95f, 1.0f), &pCoreBrush);

    if (!pGlowBrush || !pCoreBrush) {
        SafeRelease(pGlowBrush);
        SafeRelease(pCoreBrush);
        return;
    }

    // Process and draw each stroke independently so disconnected strokes NEVER link together
    for (auto& stroke : g_laserStrokes) {
        if (stroke.points.empty()) continue;

        size_t count = stroke.points.size();
        if (count == 1) {
            // Single tap / dot: render as a glowing neon laser bead
            const auto& pt = stroke.points[0];
            ULONGLONG age = (now >= pt.timestamp) ? (now - pt.timestamp) : 0;
            if (age < duration) {
                float life = 1.0f - (float)age / (float)duration;
                if (life > 0.01f) {
                    pGlowBrush->SetOpacity(0.50f * life);
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pt.x, pt.y), 5.0f * life, 5.0f * life), pGlowBrush);
                    pCoreBrush->SetOpacity(0.95f * life);
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pt.x, pt.y), 2.2f * life, 2.2f * life), pCoreBrush);
                }
            }
        }
        else {
            // Continuous stroke: compute overall life from newest point
            ULONGLONG newestAge = (now >= stroke.points.back().timestamp) ? (now - stroke.points.back().timestamp) : 0;
            float strokeLife = 1.0f;
            if (newestAge > 0 && newestAge < duration) {
                strokeLife = 1.0f - (float)newestAge / (float)duration;
            } else if (newestAge >= duration) {
                continue;
            }

            // Build or reuse cached geometry: avoids recreating COM objects from scratch every 16ms
            if (!stroke.pCachedGeometry && g_pD2DFactory) {
                ID2D1PathGeometry* pGeom = nullptr;
                if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pGeom))) {
                    ID2D1GeometrySink* pSink = nullptr;
                    if (SUCCEEDED(pGeom->Open(&pSink))) {
                        pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
                        pSink->BeginFigure(D2D1::Point2F(stroke.points[0].x, stroke.points[0].y), D2D1_FIGURE_BEGIN_HOLLOW);

                        if (count == 2) {
                            pSink->AddLine(D2D1::Point2F(stroke.points[1].x, stroke.points[1].y));
                        }
                        else {
                            // Smooth Quadratic Bézier spline through midpoints (seamless, zero dots)
                            for (size_t k = 0; k + 1 < count; ++k) {
                                D2D1_POINT_2F midPoint = D2D1::Point2F(
                                    (stroke.points[k].x + stroke.points[k + 1].x) * 0.5f,
                                    (stroke.points[k].y + stroke.points[k + 1].y) * 0.5f
                                );
                                pSink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
                                    D2D1::Point2F(stroke.points[k].x, stroke.points[k].y),
                                    midPoint
                                ));
                            }
                            pSink->AddLine(D2D1::Point2F(stroke.points[count - 1].x, stroke.points[count - 1].y));
                        }

                        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
                        pSink->Close();
                        SafeRelease(pSink);

                        stroke.pCachedGeometry = pGeom;
                    } else {
                        SafeRelease(pGeom);
                    }
                }
            }

            if (stroke.pCachedGeometry) {
                // Outer Vibrant Neon Glow
                pGlowBrush->SetOpacity(0.52f * strokeLife);
                pRT->DrawGeometry(stroke.pCachedGeometry, pGlowBrush, 6.5f, g_pRoundStrokeStyle);

                // Hot Inner Laser Beam Core
                pCoreBrush->SetOpacity(0.95f * strokeLife);
                pRT->DrawGeometry(stroke.pCachedGeometry, pCoreBrush, 2.5f, g_pRoundStrokeStyle);
            }
        }
    }

    SafeRelease(pGlowBrush);
    SafeRelease(pCoreBrush);
}

void DrawLaserCursor(ID2D1HwndRenderTarget* pRT) {
    if (g_currentTool != ToolMode::Laser) return;
    if (g_isSnipping) return;
    if (g_isLaserDrawing) {
        // If the user is actively drawing and moving, suppress cursor bead to avoid dot stacking.
        // If the user pauses / stops moving while holding the button, keep bead visible so they have continuous pointing feedback.
        bool hasActiveMovingTip = false;
        if (!g_laserStrokes.empty() && !g_laserStrokes.back().points.empty()) {
            ULONGLONG now = GetTickCount64();
            ULONGLONG age = (now >= g_laserStrokes.back().points.back().timestamp) ?
                            (now - g_laserStrokes.back().points.back().timestamp) : 0;
            if (age < 80) {
                hasActiveMovingTip = true;
            }
        }
        if (hasActiveMovingTip) return;
    }

    // Do not draw laser bead over toolbar or flyouts
    if (g_shapesFlyoutOpen &&
        g_cursorX >= g_shapesFlyoutRect.left && g_cursorX <= g_shapesFlyoutRect.right &&
        g_cursorY >= g_shapesFlyoutRect.top && g_cursorY <= g_shapesFlyoutRect.bottom) {
        return;
    }
    if (g_gridFlyoutOpen &&
        g_cursorX >= g_gridFlyoutRect.left && g_cursorX <= g_gridFlyoutRect.right &&
        g_cursorY >= g_gridFlyoutRect.top && g_cursorY <= g_gridFlyoutRect.bottom) {
        return;
    }
    if (g_backdropFlyoutOpen &&
        g_cursorX >= g_backdropFlyoutRect.left && g_cursorX <= g_backdropFlyoutRect.right &&
        g_cursorY >= g_backdropFlyoutRect.top && g_cursorY <= g_backdropFlyoutRect.bottom) {
        return;
    }
    if (g_colorFlyoutOpen &&
        g_cursorX >= g_colorFlyoutRect.left && g_cursorX <= g_colorFlyoutRect.right &&
        g_cursorY >= g_colorFlyoutRect.top && g_cursorY <= g_colorFlyoutRect.bottom) {
        return;
    }
    if (g_settings.showBottomToolbar &&
        g_cursorX >= (g_toolbarRect.left - 4.0f) && g_cursorX <= (g_toolbarRect.right + 4.0f) &&
        g_cursorY >= (g_toolbarRect.top - 4.0f) && g_cursorY <= (g_toolbarRect.bottom + 4.0f)) {
        return;
    }

    // 3-Tier Neon Glow Laser Pointer Bead
    ID2D1SolidColorBrush* pHaloBrush = nullptr;
    ID2D1SolidColorBrush* pCoreBrush = nullptr;
    ID2D1SolidColorBrush* pSparkBrush = nullptr;

    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.05f, 0.20f, 0.35f), &pHaloBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.15f, 0.25f, 0.90f), &pCoreBrush);
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.95f, 0.98f, 1.00f), &pSparkBrush);

    D2D1_POINT_2F pt = D2D1::Point2F(g_cursorX, g_cursorY);
    float scale = GetDpiScaleAtPoint(g_cursorX, g_cursorY);

    if (pHaloBrush) {
        // Broad outer neon aura
        pRT->FillEllipse(D2D1::Ellipse(pt, 9.0f * scale, 9.0f * scale), pHaloBrush);
        pHaloBrush->Release();
    }
    if (pCoreBrush) {
        // Vivid crimson core
        pRT->FillEllipse(D2D1::Ellipse(pt, 4.5f * scale, 4.5f * scale), pCoreBrush);
        pCoreBrush->Release();
    }
    if (pSparkBrush) {
        // High-intensity white laser center spark
        pRT->FillEllipse(D2D1::Ellipse(pt, 2.0f * scale, 2.0f * scale), pSparkBrush);
        pSparkBrush->Release();
    }
}

void RenderOverlay() {
    if (!g_pRenderTarget) return;

    g_pRenderTarget->BeginDraw();
    g_pRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

    // 1. Draw solid whiteboard/blackboard backdrop or frozen desktop backdrop
    if (g_canvasBg != CanvasBg::Transparent && g_currentTool != ToolMode::Pointer) {
        D2D1_SIZE_F size = g_pRenderTarget->GetSize();
        D2D1_RECT_F targetRect = D2D1::RectF(0, 0, size.width, size.height);
        if (g_canvasScope == CanvasMonitorScope::ActiveCursor) {
            float monL = 0, monT = 0, monR = 0, monB = 0;
            GetMonitorBoundsAt(g_cursorX, g_cursorY, monL, monT, monR, monB);
            targetRect = D2D1::RectF(monL, monT, monR, monB);
        } else if (g_canvasScope == CanvasMonitorScope::Primary) {
            const auto& mons = GetSystemMonitorList();
            for (const auto& m : mons) {
                if (m.isPrimary) {
                    targetRect = m.rect;
                    break;
                }
            }
        } else if (g_canvasScope > CanvasMonitorScope::AllMonitors) {
            const auto& mons = GetSystemMonitorList();
            int idx = (int)g_canvasScope - 1;
            if (idx >= 0 && idx < (int)mons.size()) {
                targetRect = mons[idx].rect;
            }
        }
        ID2D1SolidColorBrush* pBgBrush = (g_canvasBg == CanvasBg::Whiteboard) ? g_pWhiteboardBrush : g_pBlackboardBrush;
        if (pBgBrush) {
            g_pRenderTarget->FillRectangle(targetRect, pBgBrush);
        }
    }
    else if (g_settings.freezeScreen && g_pDesktopBitmap && g_currentTool != ToolMode::Pointer) {
        D2D1_SIZE_F size = g_pRenderTarget->GetSize();
        g_pRenderTarget->DrawBitmap(
            g_pDesktopBitmap,
            D2D1::RectF(0, 0, size.width, size.height)
        );
    }

    // 1b. Low-Resource Hardware-Accelerated Grid Overlay (Single GPU draw call)
    // Visible in normal and click-through pointer modes, but hidden when canvas visibility is toggled off (g_inkVisible == false)
    if (g_gridStyle != GridStyle::None && g_pGridBrush && g_inkVisible) {
        D2D1_MATRIX_3X2_F gridMatrix = D2D1::Matrix3x2F::Scale(g_zoomScale, g_zoomScale) *
                                       D2D1::Matrix3x2F::Translation(g_panOffsetX, g_panOffsetY);
        g_pGridBrush->SetTransform(gridMatrix);
        D2D1_SIZE_F size = g_pRenderTarget->GetSize();
        g_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, size.width, size.height), g_pGridBrush);
    }

    // Apply Pan & Zoom Transform to Strokes
    D2D1_MATRIX_3X2_F canvasMatrix = D2D1::Matrix3x2F::Scale(g_zoomScale, g_zoomScale) *
                                     D2D1::Matrix3x2F::Translation(g_panOffsetX, g_panOffsetY);
    g_pRenderTarget->SetTransform(canvasMatrix);

    // 2. Draw completed strokes
    for (auto& stroke : g_strokes) {
        DrawSmoothStroke(g_pRenderTarget, stroke);
    }

    // 3. Draw active stroke in progress
    if (g_isDrawing) {
        DrawSmoothStroke(g_pRenderTarget, g_currentStroke);
    }

    // 3b. Vanishing Ephemeral Laser Trail
    DrawLaserTrail(g_pRenderTarget);

    // Reset transform for HUD & Toolbar
    g_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

    if (g_hideUIForCapture) {
        // Suppress HUD, toolbar, radial, cursors, and dimming during pristine backdrop snapshot capture
    }
    else if (g_isSnipping) {
        DrawSnippingOverlay(g_pRenderTarget);
    }
    else {
        // 4. Eraser cursor, Laser bead cursor, Inking cursor, Pen size bubble & Zoom badge
        DrawEraserCursor(g_pRenderTarget);
        DrawLaserCursor(g_pRenderTarget);
        DrawInkingCursor(g_pRenderTarget);
        DrawPenSizePreview(g_pRenderTarget);
        DrawZoomPreview(g_pRenderTarget);

        // 5. Compact Bottom Toolbar
        DrawToolbar(g_pRenderTarget);

        // 5b. Shapes Action Modal (drawn on top of toolbar when open)
        if (g_shapesFlyoutOpen) {
            DrawShapesFlyout(g_pRenderTarget);
        }

        // 5c. Grid Settings Action Modal (drawn on top of toolbar when open)
        if (g_gridFlyoutOpen) {
            DrawGridFlyout(g_pRenderTarget);
        }

        // 5d. Custom Color & Opacity Studio Modal (drawn on top of toolbar when open)
        if (g_colorFlyoutOpen) {
            DrawColorFlyout(g_pRenderTarget);
        }

        // 5e. Whiteboard & Backdrop Action Modal (drawn on top of toolbar when open)
        if (g_backdropFlyoutOpen) {
            DrawBackdropFlyout(g_pRenderTarget);
        }

        // 6. Circular Radial Quick Menu
        DrawRadialMenu(g_pRenderTarget);
    }

    // 7. Toast feedback
    if (!g_hideUIForCapture) {
        D2D1_SIZE_F s = g_pRenderTarget->GetSize();
        DrawToast(g_pRenderTarget, (int)s.width, (int)s.height);
    }

    HRESULT hr = g_pRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        ReleaseD2DResources();
        if (g_hOverlayWnd) {
            CreateD2DResources(g_hOverlayWnd);
            InvalidateOverlay();
        }
    }
}

// ----------------------------------------------------------------------------
// Erasing Logic (Whole-Stroke Eraser)
// ----------------------------------------------------------------------------

bool EraseWholeShapeAt(float x, float y, float radius) {
    float adjustedRadius = radius / g_zoomScale;
    bool changed = false;
    float adjustedX = (x - g_panOffsetX) / g_zoomScale;
    float adjustedY = (y - g_panOffsetY) / g_zoomScale;

    float eraserMinX = adjustedX - adjustedRadius;
    float eraserMaxX = adjustedX + adjustedRadius;
    float eraserMinY = adjustedY - adjustedRadius;
    float eraserMaxY = adjustedY + adjustedRadius;

    for (auto it = g_strokes.begin(); it != g_strokes.end();) {
        // Fast-fail AABB check
        if (eraserMaxX < it->bounds.left || eraserMinX > it->bounds.right ||
            eraserMaxY < it->bounds.top  || eraserMinY > it->bounds.bottom) {
            ++it;
            continue;
        }

        bool hit = false;
        float effR = adjustedRadius + it->width * 0.5f;
        float effRSq = effR * effR;

        if (it->shapeType == ShapeType::Freehand) {
            const auto& pts = it->points;
            for (size_t i = 0; i < pts.size(); ++i) {
                if (DistanceSq(adjustedX, adjustedY, pts[i].x, pts[i].y) <= effRSq) {
                    hit = true;
                    break;
                }
                if (i + 1 < pts.size()) {
                    if (DistToSegmentSq(adjustedX, adjustedY, pts[i].x, pts[i].y, pts[i + 1].x, pts[i + 1].y) <= effRSq) {
                        hit = true;
                        break;
                    }
                }
            }
        }
        else if (it->shapeType == ShapeType::Line) {
            if (DistToSegmentSq(adjustedX, adjustedY, it->startPt.x, it->startPt.y, it->endPt.x, it->endPt.y) <= effRSq) {
                hit = true;
            }
        }
        else if (it->shapeType == ShapeType::Arrow) {
            if (DistToSegmentSq(adjustedX, adjustedY, it->startPt.x, it->startPt.y, it->endPt.x, it->endPt.y) <= effRSq) {
                hit = true;
            }
            if (!hit) {
                // Check arrowhead wings/fins
                float dx = it->endPt.x - it->startPt.x;
                float dy = it->endPt.y - it->startPt.y;
                float len = std::sqrt(dx * dx + dy * dy);
                if (len > 0.001f) {
                    float ux = dx / len;
                    float uy = dy / len;
                    float arrowLen = std::min(std::max(it->width * 3.5f, 16.0f), len * 0.45f);
                    float wingW = arrowLen * 0.55f;
                    float bx = it->endPt.x - ux * arrowLen;
                    float by = it->endPt.y - uy * arrowLen;
                    D2D1_POINT_2F pLeft = D2D1::Point2F(bx - uy * wingW, by + ux * wingW);
                    D2D1_POINT_2F pRight = D2D1::Point2F(bx + uy * wingW, by - ux * wingW);
                    if (DistToSegmentSq(adjustedX, adjustedY, it->endPt.x, it->endPt.y, pLeft.x, pLeft.y) <= effRSq ||
                        DistToSegmentSq(adjustedX, adjustedY, it->endPt.x, it->endPt.y, pRight.x, pRight.y) <= effRSq ||
                        DistToSegmentSq(adjustedX, adjustedY, pLeft.x, pLeft.y, pRight.x, pRight.y) <= effRSq) {
                        hit = true;
                    }
                }
            }
        }
        else if (it->shapeType == ShapeType::Rectangle) {
            float minX = std::min(it->startPt.x, it->endPt.x);
            float maxX = std::max(it->startPt.x, it->endPt.x);
            float minY = std::min(it->startPt.y, it->endPt.y);
            float maxY = std::max(it->startPt.y, it->endPt.y);
            if (DistToSegmentSq(adjustedX, adjustedY, minX, minY, maxX, minY) <= effRSq ||
                DistToSegmentSq(adjustedX, adjustedY, maxX, minY, maxX, maxY) <= effRSq ||
                DistToSegmentSq(adjustedX, adjustedY, maxX, maxY, minX, maxY) <= effRSq ||
                DistToSegmentSq(adjustedX, adjustedY, minX, maxY, minX, minY) <= effRSq) {
                hit = true;
            }
        }
        else if (it->shapeType == ShapeType::Ellipse) {
            float cx = (it->startPt.x + it->endPt.x) * 0.5f;
            float cy = (it->startPt.y + it->endPt.y) * 0.5f;
            float rx = std::abs(it->endPt.x - it->startPt.x) * 0.5f;
            float ry = std::abs(it->endPt.y - it->startPt.y) * 0.5f;
            float dx = adjustedX - cx;
            float dy = adjustedY - cy;
            float dist = std::sqrt(dx * dx + dy * dy);
            float avgR = (rx + ry) * 0.5f;
            if (std::abs(dist - avgR) <= effR) {
                hit = true;
            }
        }
        else if (it->shapeType == ShapeType::Triangle) {
            float minX = std::min(it->startPt.x, it->endPt.x);
            float maxX = std::max(it->startPt.x, it->endPt.x);
            float minY = std::min(it->startPt.y, it->endPt.y);
            float maxY = std::max(it->startPt.y, it->endPt.y);
            float midX = (minX + maxX) * 0.5f;
            if (DistToSegmentSq(adjustedX, adjustedY, midX, minY, minX, maxY) <= effRSq ||
                DistToSegmentSq(adjustedX, adjustedY, minX, maxY, maxX, maxY) <= effRSq ||
                DistToSegmentSq(adjustedX, adjustedY, maxX, maxY, midX, minY) <= effRSq) {
                hit = true;
            }
        }

        if (hit) {
            if (!g_hasPushedUndoForCurrentErase) {
                PushUndoState();
                g_hasPushedUndoForCurrentErase = true;
            }
            it = g_strokes.erase(it);
            changed = true;
        }
        else {
            ++it;
        }
    }

    if (changed) {
        InvalidateOverlay();
    }
    return changed;
}

// ----------------------------------------------------------------------------
// Snapshot & PNG Export via WIC
// ----------------------------------------------------------------------------

void SaveBitmapToPNG(HBITMAP hBitmap, const std::wstring& filePath) {
    if (!g_pWICFactory) return;

    IWICBitmap* pWicBitmap = nullptr;
    HRESULT hr = g_pWICFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapIgnoreAlpha, &pWicBitmap);
    if (FAILED(hr)) return;

    IWICStream* pStream = nullptr;
    hr = g_pWICFactory->CreateStream(&pStream);
    if (SUCCEEDED(hr)) {
        hr = pStream->InitializeFromFilename(filePath.c_str(), GENERIC_WRITE);
        if (SUCCEEDED(hr)) {
            IWICBitmapEncoder* pEncoder = nullptr;
            hr = g_pWICFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
            if (SUCCEEDED(hr)) {
                hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
                if (SUCCEEDED(hr)) {
                    IWICBitmapFrameEncode* pFrame = nullptr;
                    hr = pEncoder->CreateNewFrame(&pFrame, NULL);
                    if (SUCCEEDED(hr)) {
                        hr = pFrame->Initialize(NULL);
                        if (SUCCEEDED(hr)) {
                            UINT w = 0, h = 0;
                            pWicBitmap->GetSize(&w, &h);
                            pFrame->SetSize(w, h);

                            WICPixelFormatGUID format = GUID_WICPixelFormat24bppBGR;
                            pFrame->SetPixelFormat(&format);

                            hr = pFrame->WriteSource(pWicBitmap, NULL);
                            if (SUCCEEDED(hr)) {
                                pFrame->Commit();
                                pEncoder->Commit();
                            }
                        }
                        pFrame->Release();
                    }
                }
                pEncoder->Release();
            }
        }
        pStream->Release();
    }
    pWicBitmap->Release();
}

void CancelSnipping() {
    g_hideUIForCapture = false;
    if (g_hSnipBackdrop) {
        DeleteObject(g_hSnipBackdrop);
        g_hSnipBackdrop = NULL;
    }
    if (!g_isSnipping) return;
    g_isSnipping = false;
    g_isSnippingDrag = false;
    InvalidateOverlay();
}

bool PreparePristineBackdrop() {
    if (g_hSnipBackdrop) {
        DeleteObject(g_hSnipBackdrop);
        g_hSnipBackdrop = NULL;
    }

    if (g_currentTool == ToolMode::Pointer) {
        SetToolMode(ToolMode::Pen);
    }

    if (!g_bIsActive) {
        ShowOverlay();
    }

    // Temporarily hide toolbar / HUD / radial / cursor to take a pristine snapshot
    g_hideUIForCapture = true;
    g_radialActive = false;

    if (g_hOverlayWnd) {
        InvalidateRect(g_hOverlayWnd, NULL, FALSE);
        UpdateWindow(g_hOverlayWnd);
        DwmFlush();
    }

    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0) {
        g_hideUIForCapture = false;
        return false;
    }

    g_snipBackdropW = vw;
    g_snipBackdropH = vh;

    HDC hScreenDC = GetDC(NULL);
    bool ok = false;
    if (hScreenDC) {
        HDC hMemDC = CreateCompatibleDC(hScreenDC);
        if (hMemDC) {
            g_hSnipBackdrop = CreateCompatibleBitmap(hScreenDC, vw, vh);
            if (g_hSnipBackdrop) {
                HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, g_hSnipBackdrop);
                BitBlt(hMemDC, 0, 0, vw, vh, hScreenDC, vx, vy, SRCCOPY | CAPTUREBLT);
                SelectObject(hMemDC, hOldBmp);
                ok = true;
            }
            DeleteDC(hMemDC);
        }
        ReleaseDC(NULL, hScreenDC);
    }

    g_hideUIForCapture = false;
    return ok;
}

void StartSnipping() {
    if (g_isSnipping) return;
    if (!PreparePristineBackdrop()) return;

    // Enter active region snipping mode
    g_isSnipping = true;
    g_isSnippingDrag = false;
    InvalidateOverlay();
}

void CaptureFullScreenSnapshot() {
    if (g_isSnipping) {
        CancelSnipping();
    }
    if (!PreparePristineBackdrop()) return;

    SaveCroppedSnapshot(0, 0, g_snipBackdropW, g_snipBackdropH);
}

inline bool EnsureDirectoryExists(const std::wstring& path) {
    if (path.empty()) return false;
    DWORD attribs = GetFileAttributesW(path.c_str());
    if (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    }
    int res = SHCreateDirectoryExW(NULL, path.c_str(), NULL);
    return (res == ERROR_SUCCESS || res == ERROR_ALREADY_EXISTS || res == ERROR_FILE_EXISTS);
}

void SaveCroppedSnapshot(int left, int top, int width, int height) {
    if (!g_hSnipBackdrop || width <= 0 || height <= 0) {
        CancelSnipping();
        return;
    }

    left = std::max(0, std::min(left, g_snipBackdropW - 1));
    top = std::max(0, std::min(top, g_snipBackdropH - 1));
    width = std::min(width, g_snipBackdropW - left);
    height = std::min(height, g_snipBackdropH - top);
    if (width <= 4 || height <= 4) {
        CancelSnipping();
        return;
    }

    HDC hScreenDC = GetDC(NULL);
    if (!hScreenDC) {
        CancelSnipping();
        return;
    }

    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HDC hDstDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hCroppedBmp = CreateCompatibleBitmap(hScreenDC, width, height);

    if (!hSrcDC || !hDstDC || !hCroppedBmp) {
        if (hCroppedBmp) DeleteObject(hCroppedBmp);
        if (hDstDC) DeleteDC(hDstDC);
        if (hSrcDC) DeleteDC(hSrcDC);
        ReleaseDC(NULL, hScreenDC);
        CancelSnipping();
        return;
    }

    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, g_hSnipBackdrop);
    HBITMAP hOldDst = (HBITMAP)SelectObject(hDstDC, hCroppedBmp);

    BitBlt(hDstDC, 0, 0, width, height, hSrcDC, left, top, SRCCOPY);

    SelectObject(hDstDC, hOldDst);
    SelectObject(hSrcDC, hOldSrc);

    // Auto-save PNG if enabled (with recursive directory creation & overwrite prevention)
    bool savedToFile = false;
    if (g_settings.autoSaveSnapshot) {
        std::wstring targetDir = L"";
        if (!g_settings.customSnapshotPath.empty()) {
            if (EnsureDirectoryExists(g_settings.customSnapshotPath)) {
                targetDir = g_settings.customSnapshotPath;
            }
        }
        if (targetDir.empty()) {
            PWSTR pKnownPath = NULL;
            if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &pKnownPath)) && pKnownPath) {
                targetDir = std::wstring(pKnownPath) + L"\\WinDraw";
                CoTaskMemFree(pKnownPath);
                EnsureDirectoryExists(targetDir);
            }
        }

        if (!targetDir.empty()) {
            SYSTEMTIME st;
            GetLocalTime(&st);
            wchar_t filename[128];
            wsprintfW(filename, L"\\WinDraw_%04d-%02d-%02d_%02d%02d%02d_%03d.png",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

            std::wstring fullPath = targetDir + filename;
            int counter = 1;
            while (GetFileAttributesW(fullPath.c_str()) != INVALID_FILE_ATTRIBUTES && counter < 100) {
                wsprintfW(filename, L"\\WinDraw_%04d-%02d-%02d_%02d%02d%02d_%03d_%d.png",
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, counter++);
                fullPath = targetDir + filename;
            }

            SaveBitmapToPNG(hCroppedBmp, fullPath);
            savedToFile = true;
        }
    }

    // Place onto clipboard
    bool clipboardSucceeded = false;
    if (OpenClipboard(g_hOverlayWnd)) {
        if (EmptyClipboard()) {
            // 1. Modern 32-bit DIBV5 with explicit alpha channel bitmasks (for Discord, Word, MS Paint, modern apps)
            BITMAPV5HEADER bi5 = {};
            bi5.bV5Size = sizeof(BITMAPV5HEADER);
            bi5.bV5Width = width;
            bi5.bV5Height = height;
            bi5.bV5Planes = 1;
            bi5.bV5BitCount = 32;
            bi5.bV5Compression = BI_BITFIELDS;
            bi5.bV5RedMask   = 0x00FF0000;
            bi5.bV5GreenMask = 0x0000FF00;
            bi5.bV5BlueMask  = 0x000000FF;
            bi5.bV5AlphaMask = 0xFF000000;
            bi5.bV5CSType    = LCS_sRGB;
            bi5.bV5Intent    = LCS_GM_IMAGES;

            DWORD dib5RowStride = width * 4;
            DWORD dib5ImageSize = dib5RowStride * height;
            DWORD dib5TotalSize = sizeof(BITMAPV5HEADER) + dib5ImageSize;

            HGLOBAL hDIBV5 = GlobalAlloc(GHND, dib5TotalSize);
            if (hDIBV5) {
                BYTE* pDIBV5 = (BYTE*)GlobalLock(hDIBV5);
                if (pDIBV5) {
                    memcpy(pDIBV5, &bi5, sizeof(BITMAPV5HEADER));
                    GetDIBits(hDstDC, hCroppedBmp, 0, height, pDIBV5 + sizeof(BITMAPV5HEADER), (BITMAPINFO*)&bi5, DIB_RGB_COLORS);
                    // Screen captures are fully opaque: force the alpha byte of each pixel to 0xFF
                    // so applications honoring bV5AlphaMask (Paint.NET, GIMP, Office) do not paste a blank/transparent image.
                    BYTE* px = pDIBV5 + sizeof(BITMAPV5HEADER);
                    for (DWORD i = 3; i < dib5ImageSize; i += 4) {
                        px[i] = 0xFF;
                    }
                    GlobalUnlock(hDIBV5);
                    if (!SetClipboardData(CF_DIBV5, hDIBV5)) {
                        GlobalFree(hDIBV5);
                    }
                } else {
                    GlobalFree(hDIBV5);
                }
            }

            // 2. Standard CF_DIB fallback (24-bit RGB ensures legacy apps don't render transparent pixels as solid black boxes)
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = width;
            bmi.bmiHeader.biHeight = height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 24;
            bmi.bmiHeader.biCompression = BI_RGB;
            DWORD rowStride = (width * 3 + 3) & ~3u;
            DWORD dibSize = sizeof(BITMAPINFOHEADER) + rowStride * height;
            HGLOBAL hDIB = GlobalAlloc(GHND, dibSize);
            if (hDIB) {
                BYTE* pDIB = (BYTE*)GlobalLock(hDIB);
                if (pDIB) {
                    memcpy(pDIB, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
                    GetDIBits(hDstDC, hCroppedBmp, 0, height, pDIB + sizeof(BITMAPINFOHEADER), &bmi, DIB_RGB_COLORS);
                    GlobalUnlock(hDIB);
                    if (!SetClipboardData(CF_DIB, hDIB)) {
                        GlobalFree(hDIB);
                    }
                } else {
                    GlobalFree(hDIB);
                }
            }

            // 3. Standard GDI DDB bitmap handle
            if (SetClipboardData(CF_BITMAP, hCroppedBmp)) {
                clipboardSucceeded = true;
            }
        }
        CloseClipboard();
    }

    if (!clipboardSucceeded) {
        DeleteObject(hCroppedBmp);
    }

    if (g_settings.showToastNotifications) {
        if (clipboardSucceeded && savedToFile) {
            g_toastMessage = L"Snapshot saved to Folder & Clipboard";
        } else if (clipboardSucceeded) {
            g_toastMessage = L"Snapshot copied to Clipboard";
        } else if (savedToFile) {
            g_toastMessage = L"Snapshot saved to Folder";
        } else {
            g_toastMessage = L"Snapshot capture failed";
        }
        g_toastStartTime = GetTickCount64();
        if (g_hOverlayWnd) SetTimer(g_hOverlayWnd, TIMER_ID_UI_ANIMATION, 30, NULL);
    }

    DeleteDC(hDstDC);
    DeleteDC(hSrcDC);
    ReleaseDC(NULL, hScreenDC);

    CancelSnipping();
}

void SetToolMode(ToolMode newMode) {
    if (g_currentTool == newMode) return;
    ToolMode oldMode = g_currentTool;
    g_currentTool = newMode;

    if (newMode == ToolMode::Highlighter) {
        g_currentPenWidth = g_settings.defaultHighlighterWidth;
    } else if (newMode == ToolMode::Pen) {
        g_currentPenWidth = g_settings.defaultPenWidth;
    }

    if (g_hOverlayWnd) {
        if (newMode == ToolMode::Pointer) {
            // Dismiss radial menu and modal flyouts before switching to click-through mode
            if (g_radialActive) {
                g_radialActive = false;
                g_radialHoverTarget = RadialTarget::None;
                g_radialHoverSector = -1;
                g_hoveredOrb = -1;
                g_hoveredRecentOrb = -1;
                g_radialRecentFanOpen = false;
            }
            g_shapesFlyoutOpen = false;
            g_gridFlyoutOpen = false;
            g_backdropFlyoutOpen = false;
            g_colorFlyoutOpen = false;

            // Clear any lingering laser trails immediately so they don't orphan on top of desktop apps
            g_laserStrokes.clear();
            g_isLaserDrawing = false;
            KillTimer(g_hOverlayWnd, TIMER_ID_LASER);

            // Enter Pointer (Click-Through) mode:
            // Window is already layered, so only toggle WS_EX_TRANSPARENT to avoid black flashing
            LONG_PTR exStyle = GetWindowLongPtr(g_hOverlayWnd, GWL_EXSTYLE);
            SetWindowLongPtr(g_hOverlayWnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
            SetWindowPos(g_hOverlayWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            SetTimer(g_hOverlayWnd, TIMER_ID_POINTER_WATCH, 100, NULL);

            if (g_settings.showToastNotifications) {
                g_toastMessage = L"Pointer Mode: Click-through active";
                g_toastStartTime = GetTickCount64();
                SetTimer(g_hOverlayWnd, TIMER_ID_UI_ANIMATION, 30, NULL);
            }
        }
        else if (oldMode == ToolMode::Pointer) {
            // Exit Pointer mode:
            KillTimer(g_hOverlayWnd, TIMER_ID_POINTER_WATCH);
            LONG_PTR exStyle = GetWindowLongPtr(g_hOverlayWnd, GWL_EXSTYLE);
            SetWindowLongPtr(g_hOverlayWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
            SetWindowPos(g_hOverlayWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

            if (g_settings.showToastNotifications) {
                g_toastMessage = L"Drawing Mode active";
                g_toastStartTime = GetTickCount64();
                SetTimer(g_hOverlayWnd, TIMER_ID_UI_ANIMATION, 30, NULL);
            }
        }

        if (oldMode == ToolMode::Laser) {
            g_isLaserDrawing = false;
        }

        InvalidateOverlay();
    }
}

// ----------------------------------------------------------------------------
// Overlay Window Procedure
// ----------------------------------------------------------------------------

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_USER_TOGGLE_POINTER: {
        if (g_radialActive) {
            g_radialActive = false;
            g_radialHoverTarget = RadialTarget::None;
            g_radialHoverSector = -1;
            g_hoveredOrb = -1;
            g_hoveredRecentOrb = -1;
            g_radialRecentFanOpen = false;
        }
        g_shapesFlyoutOpen = false;
        g_gridFlyoutOpen = false;
        g_backdropFlyoutOpen = false;
        g_colorFlyoutOpen = false;

        if (g_isSnipping) {
            CancelSnipping();
        }
        if (g_currentTool == ToolMode::Pointer) {
            SetToolMode(ToolMode::Pen);
            SetForegroundWindow(hwnd);
            SetFocus(hwnd);
        }
        else {
            SetToolMode(ToolMode::Pointer);
        }
        return 0;
    }

    case WM_DPICHANGED: {
        int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        BuildToolbarLayout(vw, vh);
        InvalidateOverlay();
        return 0;
    }

    case WM_DISPLAYCHANGE: {
        GetSystemMonitorList(true); // Refresh cached monitor topologies
        InvalidateMonitorBoundsCache();
        int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        SetWindowPos(hwnd, HWND_TOPMOST, vx, vy, vw, vh, SWP_NOACTIVATE);
        if (g_pRenderTarget) {
            g_pRenderTarget->Resize(D2D1::SizeU(vw, vh));
        }
        CaptureDesktop();
        BuildToolbarLayout(vw, vh);
        InvalidateOverlay();
        return 0;
    }

    case WM_SIZE: {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        if (g_pRenderTarget && width > 0 && height > 0) {
            g_pRenderTarget->Resize(D2D1::SizeU(width, height));
            BuildToolbarLayout(width, height);
            InvalidateOverlay();
        }
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        RenderOverlay();
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_MOUSEWHEEL: {
        if (g_isSnipping) return 0;
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        float step = (delta > 0) ? 1.0f : -1.0f;

        POINT wheelPt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hwnd, &wheelPt);
        g_cursorX = (float)wheelPt.x;
        g_cursorY = (float)wheelPt.y;

        // Radial Menu Active: Scroll wheel over Shape sector (or active Shape) cycles shapes!
        if (g_radialActive) {
            bool isHoveringShape = false;
            if (g_radialHoverTarget == RadialTarget::Sector && g_radialHoverSector >= 0 && g_radialHoverSector < (int)g_activeRadialSlots.size()) {
                if (g_activeRadialSlots[g_radialHoverSector] == RadialAction::Shape) {
                    isHoveringShape = true;
                }
            } else if (g_currentShape != ShapeType::Freehand && g_currentTool == ToolMode::Pen) {
                isHoveringShape = true;
            }

            if (isHoveringShape) {
                ShapeType shapeOrder[] = {
                    ShapeType::Line,
                    ShapeType::Arrow,
                    ShapeType::Rectangle,
                    ShapeType::Ellipse,
                    ShapeType::Triangle
                };
                const int kShapeCount = 5;
                int curIdx = 0;
                for (int i = 0; i < kShapeCount; ++i) {
                    if (g_currentShape == shapeOrder[i]) {
                        curIdx = i;
                        break;
                    }
                }
                if (delta > 0) {
                    curIdx = (curIdx + 1) % kShapeCount;
                } else {
                    curIdx = (curIdx - 1 + kShapeCount) % kShapeCount;
                }
                g_currentShape = shapeOrder[curIdx];

                const wchar_t* shapeLabels[] = {
                    L"Shape: Line (Hold Shift for Orthogonal)",
                    L"Shape: Arrow",
                    L"Shape: Rectangle (Hold Shift for Square)",
                    L"Shape: Ellipse (Hold Shift for Circle)",
                    L"Shape: Triangle (Hold Shift for Equilateral)"
                };
                ShowToastNotification(shapeLabels[curIdx]);
                InvalidateOverlay();
            }

            bool isHoveringWhiteboard = false;
            if (g_radialHoverTarget == RadialTarget::Sector && g_radialHoverSector >= 0 && g_radialHoverSector < (int)g_activeRadialSlots.size()) {
                if (g_activeRadialSlots[g_radialHoverSector] == RadialAction::Whiteboard) {
                    isHoveringWhiteboard = true;
                }
            }
            if (isHoveringWhiteboard) {
                const auto& mons = GetSystemMonitorList();
                std::vector<CanvasMonitorScope> scopes = {
                    CanvasMonitorScope::ActiveCursor,
                    CanvasMonitorScope::AllMonitors,
                    CanvasMonitorScope::Primary
                };
                for (size_t i = 0; i < mons.size(); ++i) {
                    scopes.push_back((CanvasMonitorScope)(i + 1));
                }
                int curIdx = 0;
                for (size_t i = 0; i < scopes.size(); ++i) {
                    if (g_canvasScope == scopes[i]) {
                        curIdx = (int)i;
                        break;
                    }
                }
                if (delta > 0) {
                    curIdx = (curIdx + 1) % (int)scopes.size();
                } else {
                    curIdx = (curIdx - 1 + (int)scopes.size()) % (int)scopes.size();
                }
                g_canvasScope = scopes[curIdx];
                if (g_canvasScope == CanvasMonitorScope::ActiveCursor) {
                    ShowToastNotification(L"Whiteboard Display: Active Screen (Follows Cursor)");
                } else if (g_canvasScope == CanvasMonitorScope::AllMonitors) {
                    ShowToastNotification(L"Whiteboard Display: All Screens");
                } else if (g_canvasScope == CanvasMonitorScope::Primary) {
                    ShowToastNotification(L"Whiteboard Display: Primary Monitor");
                } else {
                    wchar_t buf[64];
                    wsprintfW(buf, L"Whiteboard Display: Screen %d", (int)g_canvasScope);
                    ShowToastNotification(buf);
                }
                InvalidateOverlay();
                return 0;
            }
            return 0;
        }

        // Holding Right-click OR in Eraser Mode: Scroll wheel resizes eraser radius!
        if (g_isRightMouseDown || g_isRightClickErasing || g_currentTool == ToolMode::Eraser) {
            g_wheelUsedWhileRightMouseDown = true;
            g_eraserRadius = std::max(kMinEraserRadius, std::min(kMaxEraserRadius, g_eraserRadius + step * 3.0f));
            InvalidateOverlay();
            return 0;
        }

        // Pan Mode / Holding Canvas: Scroll wheel zooms canvas in and out centered on cursor!
        if (g_currentTool == ToolMode::Pan || g_isPanning) {
            float notches = (float)delta / 120.0f;
            float factor = std::pow(1.12f, notches);
            float oldScale = g_zoomScale;
            float newScale = std::max(0.15f, std::min(8.0f, oldScale * factor));

            if (std::abs(newScale - oldScale) > 0.0005f) {
                // Zoom centered on cursor:
                g_panOffsetX = g_cursorX - (g_cursorX - g_panOffsetX) * (newScale / oldScale);
                g_panOffsetY = g_cursorY - (g_cursorY - g_panOffsetY) * (newScale / oldScale);
                g_zoomScale = newScale;

                if (g_isPanning) {
                    g_panStartPos = { (LONG)g_cursorX, (LONG)g_cursorY };
                }

                g_zoomPreviewTime = GetTickCount64();
                SetTimer(hwnd, TIMER_ID_UI_ANIMATION, 30, NULL);
                InvalidateOverlay();
            }
            return 0;
        }

        // Laser Mode: Scroll wheel adjusts laser trail duration between 200ms (min) and 5000ms (max)!
        if (g_currentTool == ToolMode::Laser) {
            int stepMs = (delta > 0) ? 100 : -100;
            g_settings.laserTrailDuration = std::max(kMinLaserTrailMs, std::min(kMaxLaserTrailMs, g_settings.laserTrailDuration + stepMs));
            if (g_settings.showToastNotifications) {
                wchar_t buf[64];
                wsprintfW(buf, L"Laser Trail: %d ms (Min: 200ms, Max: 5000ms)", g_settings.laserTrailDuration);
                g_toastMessage = buf;
                g_toastStartTime = GetTickCount64();
            }
            SetTimer(hwnd, TIMER_ID_UI_ANIMATION, 30, NULL);
            InvalidateOverlay();
            return 0;
        }

        if (g_currentTool == ToolMode::Highlighter) {
            g_settings.defaultHighlighterWidth = std::max(kMinHighlighterWidth, std::min(kMaxHighlighterWidth, g_settings.defaultHighlighterWidth + step * 2.0f));
            g_currentPenWidth = g_settings.defaultHighlighterWidth;
        }
        else {
            g_settings.defaultPenWidth = std::max(kMinPenWidth, std::min(kMaxPenWidth, g_settings.defaultPenWidth + step));
            g_currentPenWidth = g_settings.defaultPenWidth;
        }
        g_sizePreviewTime = GetTickCount64();
        SetTimer(hwnd, TIMER_ID_UI_ANIMATION, 30, NULL);
        InvalidateOverlay();
        return 0;
    }

    case WM_TIMER: {
        if (wParam == TIMER_ID_UI_ANIMATION) {
            bool needTimer = false;
            ULONGLONG now = GetTickCount64();
            if (g_zoomPreviewTime != 0) {
                if (now - g_zoomPreviewTime > 1100) {
                    g_zoomPreviewTime = 0;
                } else {
                    needTimer = true;
                }
            }
            if (g_sizePreviewTime != 0) {
                if (now - g_sizePreviewTime > 900) {
                    g_sizePreviewTime = 0;
                } else {
                    needTimer = true;
                }
            }
            if (g_toastStartTime != 0) {
                if (now - g_toastStartTime > 2000) {
                    g_toastStartTime = 0;
                } else {
                    needTimer = true;
                }
            }
            InvalidateOverlay();
            if (!needTimer) {
                KillTimer(hwnd, TIMER_ID_UI_ANIMATION);
            }
            return 0;
        }
        if (wParam == TIMER_ID_LASER) {
            bool needTimer = false;
            ULONGLONG now = GetTickCount64();
            if (!g_laserStrokes.empty() || g_isLaserDrawing) {
                ULONGLONG duration = (ULONGLONG)std::max(200, g_settings.laserTrailDuration);
                for (auto it = g_laserStrokes.begin(); it != g_laserStrokes.end(); ) {
                    bool popped = false;
                    while (!it->points.empty() && (now - it->points.front().timestamp > duration)) {
                        it->points.pop_front();
                        popped = true;
                    }
                    if (popped) {
                        it->InvalidateGeometry();
                    }
                    if (it->points.empty()) {
                        it->ReleaseGeometry();
                        it = g_laserStrokes.erase(it);
                    } else {
                        ++it;
                    }
                }
                if (!g_laserStrokes.empty() || g_isLaserDrawing) {
                    needTimer = true;
                }
            }
            InvalidateOverlay();
            if (!needTimer) {
                KillTimer(hwnd, TIMER_ID_LASER);
            }
            return 0;
        }
        if (wParam == TIMER_ID_POINTER_WATCH) {
            // Pointer (Click-Through) mode: monitor mouse to allow interacting with the toolbar
            if (g_currentTool == ToolMode::Pointer && g_bIsActive) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);

                bool overInteractive = false;
                if (g_shapesFlyoutOpen &&
                    pt.x >= g_shapesFlyoutRect.left && pt.x <= g_shapesFlyoutRect.right &&
                    pt.y >= g_shapesFlyoutRect.top && pt.y <= g_shapesFlyoutRect.bottom) {
                    overInteractive = true;
                }
                if (g_gridFlyoutOpen &&
                    pt.x >= g_gridFlyoutRect.left && pt.x <= g_gridFlyoutRect.right &&
                    pt.y >= g_gridFlyoutRect.top && pt.y <= g_gridFlyoutRect.bottom) {
                    overInteractive = true;
                }
                if (g_backdropFlyoutOpen &&
                    pt.x >= g_backdropFlyoutRect.left && pt.x <= g_backdropFlyoutRect.right &&
                    pt.y >= g_backdropFlyoutRect.top && pt.y <= g_backdropFlyoutRect.bottom) {
                    overInteractive = true;
                }
                if (g_colorFlyoutOpen &&
                    pt.x >= g_colorFlyoutRect.left && pt.x <= g_colorFlyoutRect.right &&
                    pt.y >= g_colorFlyoutRect.top && pt.y <= g_colorFlyoutRect.bottom) {
                    overInteractive = true;
                }
                if (g_settings.showBottomToolbar) {
                    if (pt.x >= (g_toolbarRect.left - 6.0f) && pt.x <= (g_toolbarRect.right + 6.0f) &&
                        pt.y >= (g_toolbarRect.top - 6.0f) && pt.y <= (g_toolbarRect.bottom + 6.0f)) {
                        overInteractive = true;
                    }
                }
                if (g_isPillMouseDown || g_isPillDragging || g_isDraggingToolbar) {
                    overInteractive = true;
                }

                LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
                if (overInteractive) {
                    // Over the toolbar or open flyout: remove WS_EX_TRANSPARENT so user can hover and click
                    if (exStyle & WS_EX_TRANSPARENT) {
                        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
                        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                        InvalidateOverlay();
                    }
                }
                else {
                    // Outside the toolbar: restore WS_EX_TRANSPARENT so clicks pass to apps underneath
                    if (!(exStyle & WS_EX_TRANSPARENT)) {
                        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
                        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                    }
                }
            }
            else {
                KillTimer(hwnd, TIMER_ID_POINTER_WATCH);
                LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
                if (exStyle & WS_EX_TRANSPARENT) {
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
                    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
            }
            return 0;
        }
        return 0;
    }

    case WM_KEYDOWN: {
        if (wParam == VK_ESCAPE) {
            if (g_radialActive) {
                g_radialActive = false;
                g_radialHoverTarget = RadialTarget::None;
                g_radialHoverSector = -1;
                g_hoveredOrb = -1;
                g_hoveredRecentOrb = -1;
                g_radialRecentFanOpen = false;
                InvalidateOverlay();
                return 0;
            }
            if (g_isEyedropperActive) {
                g_isEyedropperActive = false;
                InvalidateOverlay();
                return 0;
            }
            if (g_shapesFlyoutOpen || g_gridFlyoutOpen || g_backdropFlyoutOpen || g_colorFlyoutOpen) {
                g_shapesFlyoutOpen = false;
                g_gridFlyoutOpen = false;
                g_backdropFlyoutOpen = false;
                g_colorFlyoutOpen = false;
                InvalidateOverlay();
                return 0;
            }
            if (g_isSnipping) {
                CancelSnipping();
                return 0;
            }
            HideOverlay();
            return 0;
        }
        if (g_isSnipping) return 0;
        if (((GetKeyState(VK_CONTROL) & 0x8000) && (wParam == '0' || wParam == VK_NUMPAD0)) ||
            (wParam == '0' && g_currentTool == ToolMode::Pan)) {
            // Reset Pan & Zoom to default (100% scale, 0 offset)
            g_zoomScale = 1.0f;
            g_panOffsetX = 0.0f;
            g_panOffsetY = 0.0f;
            if (g_isPanning) {
                g_panStartPos = { (LONG)g_cursorX, (LONG)g_cursorY };
            }
            g_zoomPreviewTime = GetTickCount64();
            SetTimer(hwnd, TIMER_ID_UI_ANIMATION, 30, NULL);
            InvalidateOverlay();
            return 0;
        }
        if (wParam == 'Z') {
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                if (GetKeyState(VK_SHIFT) & 0x8000) {
                    PerformRedo();
                }
                else {
                    PerformUndo();
                }
            }
            return 0;
        }
        if (wParam == 'Y') {
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                PerformRedo();
            }
            return 0;
        }
        if (wParam == 'S') {
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                CaptureFullScreenSnapshot();
            }
            else {
                StartSnipping();
            }
            return 0;
        }
        if (wParam == 'G') {
            if (GetKeyState(VK_SHIFT) & 0x8000) {
                // Shift+G: Cycle density (Fine -> Medium -> Coarse -> Fine)
                if (g_gridDensity == GridDensity::Fine) {
                    g_gridDensity = GridDensity::Medium;
                }
                else if (g_gridDensity == GridDensity::Medium) {
                    g_gridDensity = GridDensity::Coarse;
                }
                else {
                    g_gridDensity = GridDensity::Fine;
                }
                if (g_gridStyle == GridStyle::None) {
                    g_gridStyle = GridStyle::DotGrid;
                }
                RebuildGridBrush();
                InvalidateOverlay();
                return 0;
            }
            // G: Cycle style (None -> DotGrid -> GraphLines -> None)
            if (g_gridStyle == GridStyle::None) {
                g_gridStyle = GridStyle::DotGrid;
            }
            else if (g_gridStyle == GridStyle::DotGrid) {
                g_gridStyle = GridStyle::GraphLines;
            }
            else {
                g_gridStyle = GridStyle::None;
            }
            RebuildGridBrush();
            InvalidateOverlay();
            return 0;
        }
        if (wParam == 'K') {
            CycleCanvasBackground();
            return 0;
        }
        if (wParam == 'D' || wParam == 'W') { SetToolMode((g_currentTool == ToolMode::Laser) ? ToolMode::Pen : ToolMode::Laser); return 0; }
        if (wParam == 'E') { SetToolMode(ToolMode::Eraser); return 0; }
        if (wParam == 'P') { SetToolMode((g_currentTool == ToolMode::Pan) ? ToolMode::Pen : ToolMode::Pan); return 0; }
        if (wParam == 'M') { SetToolMode((g_currentTool == ToolMode::Pointer) ? ToolMode::Pen : ToolMode::Pointer); return 0; }
        if (wParam == 'H') { SetToolMode(ToolMode::Highlighter); return 0; }
        if (wParam == 'V') {
            if (!(GetKeyState(VK_CONTROL) & 0x8000)) {
                g_inkVisible = !g_inkVisible;
                InvalidateOverlay();
            }
            return 0;
        }
        if (wParam == 'C') {
            if (!(GetKeyState(VK_CONTROL) & 0x8000)) {
                if (!g_strokes.empty() || !g_laserStrokes.empty()) {
                    if (!g_strokes.empty()) PushUndoState();
                    g_strokes.clear();
                    g_laserStrokes.clear();
                    if (!g_isLaserDrawing) KillTimer(hwnd, TIMER_ID_LASER);
                    InvalidateOverlay();
                }
            }
            return 0;
        }
        if (wParam == 'F') { g_currentShape = ShapeType::Freehand; SetToolMode(ToolMode::Pen); g_shapesFlyoutOpen = false; g_gridFlyoutOpen = false; g_backdropFlyoutOpen = false; BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)); InvalidateOverlay(); return 0; }
        if (wParam == 'L') { g_currentShape = ShapeType::Line; SetToolMode(ToolMode::Pen); g_shapesFlyoutOpen = false; g_gridFlyoutOpen = false; g_backdropFlyoutOpen = false; BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)); InvalidateOverlay(); return 0; }
        if (wParam == 'A') { g_currentShape = ShapeType::Arrow; SetToolMode(ToolMode::Pen); g_shapesFlyoutOpen = false; g_gridFlyoutOpen = false; g_backdropFlyoutOpen = false; BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)); InvalidateOverlay(); return 0; }
        if (wParam == 'R') { g_currentShape = ShapeType::Rectangle; SetToolMode(ToolMode::Pen); g_shapesFlyoutOpen = false; g_gridFlyoutOpen = false; g_backdropFlyoutOpen = false; BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)); InvalidateOverlay(); return 0; }
        if (wParam == 'O') { g_currentShape = ShapeType::Ellipse; SetToolMode(ToolMode::Pen); g_shapesFlyoutOpen = false; g_gridFlyoutOpen = false; g_backdropFlyoutOpen = false; BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)); InvalidateOverlay(); return 0; }
        if (wParam == 'T') { g_currentShape = ShapeType::Triangle; SetToolMode(ToolMode::Pen); g_shapesFlyoutOpen = false; g_gridFlyoutOpen = false; g_backdropFlyoutOpen = false; BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)); InvalidateOverlay(); return 0; }
        if (wParam >= '1' && wParam <= '4') {
            int penIdx = (int)(wParam - '1');
            g_activeColor = kPresetColors[penIdx];
            g_colorFlyoutOpen = false;
            SetToolMode(ToolMode::Pen);
            InvalidateOverlay();
            return 0;
        }
        if (wParam == '5') {
            if (g_settings.showBottomToolbar) {
                g_activeColor = g_customColor.activeColor;
                g_colorFlyoutOpen = !g_colorFlyoutOpen;
                g_shapesFlyoutOpen = false;
                g_gridFlyoutOpen = false;
                g_backdropFlyoutOpen = false;
                if (g_colorFlyoutOpen && g_toolbarCollapsed) {
                    g_toolbarCollapsed = false;
                    BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
                    SavePersistentToolbarState();
                }
                SetToolMode(ToolMode::Pen);
                InvalidateOverlay();
            }
            return 0;
        }
        if (wParam == 'B') {
            if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000)) {
                g_toolbarCustomX = -1.0f;
                g_toolbarCustomY = -1.0f;
                int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                BuildToolbarLayout(vw, vh);
                SavePersistentToolbarState();
                ShowToastNotification(L"Toolbar Position Reset to Center");
                InvalidateOverlay();
                return 0;
            }
            g_colorFlyoutOpen = false;
            g_shapesFlyoutOpen = false;
            g_gridFlyoutOpen = false;
            g_backdropFlyoutOpen = false;
            g_toolbarCollapsed = !g_toolbarCollapsed;
            int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
            if (!g_toolbarCollapsed) {
                float pillCenterX = (g_toolbarRect.left + g_toolbarRect.right) * 0.5f;
                g_toolbarCustomX = pillCenterX - g_toolbarExpandedWidth * 0.5f;
                if (g_toolbarCustomX < 10.0f) g_toolbarCustomX = 10.0f;
                if (g_toolbarCustomX + g_toolbarExpandedWidth > (float)vw - 10.0f) g_toolbarCustomX = (float)vw - g_toolbarExpandedWidth - 10.0f;
                ShowToastNotification(L"Toolbar Expanded");
            } else {
                float oldCenterX = (g_toolbarRect.left + g_toolbarRect.right) * 0.5f;
                g_toolbarCustomX = oldCenterX - g_toolbarPillWidth * 0.5f;
                ShowToastNotification(L"Toolbar Collapsed");
            }
            BuildToolbarLayout(vw, vh);
            SavePersistentToolbarState();
            InvalidateOverlay();
            return 0;
        }
        if (wParam == VK_OEM_4) { // '['
            if (g_currentTool == ToolMode::Laser) {
                g_settings.laserTrailDuration = std::max(kMinLaserTrailMs, g_settings.laserTrailDuration - 100);
                if (g_settings.showToastNotifications) {
                    wchar_t buf[64];
                    wsprintfW(buf, L"Laser Trail: %d ms (Min: 200ms, Max: 5000ms)", g_settings.laserTrailDuration);
                    g_toastMessage = buf;
                    g_toastStartTime = GetTickCount64();
                }
                SetTimer(hwnd, TIMER_ID_UI_ANIMATION, 30, NULL);
                InvalidateOverlay();
                return 0;
            }
            if (g_currentTool == ToolMode::Highlighter) {
                g_settings.defaultHighlighterWidth = std::max(kMinHighlighterWidth, g_settings.defaultHighlighterWidth - 2.0f);
                g_currentPenWidth = g_settings.defaultHighlighterWidth;
            } else {
                g_settings.defaultPenWidth = std::max(kMinPenWidth, g_settings.defaultPenWidth - 1.0f);
                g_currentPenWidth = g_settings.defaultPenWidth;
            }
            g_sizePreviewTime = GetTickCount64();
            InvalidateOverlay();
            return 0;
        }
        if (wParam == VK_OEM_6) { // ']'
            if (g_currentTool == ToolMode::Laser) {
                g_settings.laserTrailDuration = std::min(kMaxLaserTrailMs, g_settings.laserTrailDuration + 100);
                if (g_settings.showToastNotifications) {
                    wchar_t buf[64];
                    wsprintfW(buf, L"Laser Trail: %d ms (Min: 200ms, Max: 5000ms)", g_settings.laserTrailDuration);
                    g_toastMessage = buf;
                    g_toastStartTime = GetTickCount64();
                }
                SetTimer(hwnd, TIMER_ID_UI_ANIMATION, 30, NULL);
                InvalidateOverlay();
                return 0;
            }
            if (g_currentTool == ToolMode::Highlighter) {
                g_settings.defaultHighlighterWidth = std::min(kMaxHighlighterWidth, g_settings.defaultHighlighterWidth + 2.0f);
                g_currentPenWidth = g_settings.defaultHighlighterWidth;
            } else {
                g_settings.defaultPenWidth = std::min(kMaxPenWidth, g_settings.defaultPenWidth + 1.0f);
                g_currentPenWidth = g_settings.defaultPenWidth;
            }
            g_sizePreviewTime = GetTickCount64();
            InvalidateOverlay();
            return 0;
        }
        break;
    }

    case WM_SYSKEYDOWN: {
        if (wParam == 'B') {
            CycleCanvasBackground();
            return 0;
        }
        break;
    }

    case WM_MOUSEMOVE: {
        g_cursorX = (float)GET_X_LPARAM(lParam);
        g_cursorY = (float)GET_Y_LPARAM(lParam);

        if (g_isSnipping) {
            if (g_isSnippingDrag) {
                g_snipEndPt = { (LONG)g_cursorX, (LONG)g_cursorY };
                InvalidateOverlay();
            }
            return 0;
        }

        if (g_isEyedropperActive) {
            POINT screenPt;
            GetCursorPos(&screenPt);
            HDC hdc = GetDC(NULL);
            if (hdc) {
                COLORREF cr = GetPixel(hdc, screenPt.x, screenPt.y);
                ReleaseDC(NULL, hdc);
                float r = (float)GetRValue(cr) / 255.0f;
                float g = (float)GetGValue(cr) / 255.0f;
                float b = (float)GetBValue(cr) / 255.0f;
                g_customColor.activeColor = D2D1::ColorF(r, g, b, g_customColor.alpha);
                RGBtoHSV(g_customColor.activeColor, g_customColor.hue, g_customColor.sat, g_customColor.val);
                g_activeColor = g_customColor.activeColor;
            }
            InvalidateOverlay();
            return 0;
        }

        // Color Picker Dragging
        if (g_pickerDrag != ColorPickerDrag::None) {
            float scale = GetFlyoutDpiScale(g_colorFlyoutRect);
            const float padX = 14.0f * scale;
            const float contentW = (g_colorFlyoutRect.right - g_colorFlyoutRect.left) - padX * 2.0f;
            float curY = g_colorFlyoutRect.top + (12.0f + 22.0f + 10.0f) * scale; // Top of canvas
            const float canvasH = 118.0f * scale;

            if (g_pickerDrag == ColorPickerDrag::SatValCanvas) {
                float s = (g_cursorX - (g_colorFlyoutRect.left + padX)) / contentW;
                s = std::max(0.0f, std::min(1.0f, s));
                float v = 1.0f - (g_cursorY - curY) / canvasH;
                v = std::max(0.0f, std::min(1.0f, v));
                g_customColor.sat = s;
                g_customColor.val = v;
            }
            else if (g_pickerDrag == ColorPickerDrag::HueBar) {
                float h = ((g_cursorX - (g_colorFlyoutRect.left + padX)) / contentW) * 360.0f;
                h = std::max(0.0f, std::min(360.0f, h));
                g_customColor.hue = h;
            }
            else if (g_pickerDrag == ColorPickerDrag::AlphaBar) {
                float a = (g_cursorX - (g_colorFlyoutRect.left + padX)) / contentW;
                a = std::max(0.05f, std::min(1.0f, a));
                g_customColor.alpha = a;
            }

            g_customColor.activeColor = HSVtoRGB(g_customColor.hue, g_customColor.sat, g_customColor.val, g_customColor.alpha);
            g_activeColor = g_customColor.activeColor;
            InvalidateOverlay();
            return 0;
        }

        // Collapsed Pill Dragging
        if (g_isPillMouseDown) {
            if (!g_isPillDragging) {
                if (DistanceSq(g_cursorX, g_cursorY, (float)g_toolbarDragStartInit.x, (float)g_toolbarDragStartInit.y) > 16.0f) {
                    g_isPillDragging = true;
                }
            }
            if (g_isPillDragging) {
                float dx = g_cursorX - g_toolbarDragStart.x;
                float dy = g_cursorY - g_toolbarDragStart.y;

                const float pillW = g_toolbarPillWidth;
                const float pillH = 30.0f * g_toolbarDpiScale;

                float nextX = g_toolbarRect.left + dx;
                float nextY = g_toolbarRect.top + dy;

                ClampToolbarToScreen(nextX, nextY, pillW, pillH, 6.0f * g_toolbarDpiScale);

                g_toolbarCustomX = nextX;
                g_toolbarCustomY = nextY;
                g_toolbarDragStart = { (LONG)g_cursorX, (LONG)g_cursorY };

                int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                BuildToolbarLayout(vw, vh);
                InvalidateOverlay();
                return 0;
            }
            return 0;
        }

        // Expanded Toolbar Dragging
        if (g_isDraggingToolbar) {
            float dx = g_cursorX - g_toolbarDragStart.x;
            float dy = g_cursorY - g_toolbarDragStart.y;

            float barW = g_toolbarRect.right - g_toolbarRect.left;
            float barH = g_toolbarRect.bottom - g_toolbarRect.top;
            if (barW <= 0.0f) barW = 800.0f * g_toolbarDpiScale;
            if (barH <= 0.0f) barH = 46.0f * g_toolbarDpiScale;

            float nextX = g_toolbarRect.left + dx;
            float nextY = g_toolbarRect.top + dy;

            ClampToolbarToScreen(nextX, nextY, barW, barH, 6.0f * g_toolbarDpiScale);

            g_toolbarCustomX = nextX;
            g_toolbarCustomY = nextY;
            g_toolbarDragStart = { (LONG)g_cursorX, (LONG)g_cursorY };

            int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
            BuildToolbarLayout(vw, vh);
            InvalidateOverlay();
            return 0;
        }

        // Check Radial Menu hover (Seamless directional selection with zero dead zones)
        if (g_radialActive) {
            float dx = g_cursorX - g_radialX;
            float dy = g_cursorY - g_radialY;
            float dist = std::sqrt(dx * dx + dy * dy);
            float rScale = g_radialDpiScale;
            if (rScale <= 0.1f) rScale = 1.0f;

            RadialTarget oldTarget = g_radialHoverTarget;
            int oldSector = g_radialHoverSector;
            int oldOrb = g_hoveredOrb;
            int oldRecentOrb = g_hoveredRecentOrb;

            g_radialHoverTarget = RadialTarget::None;
            g_radialHoverSector = -1;
            g_hoveredOrb = -1;
            g_hoveredRecentOrb = -1;

            if (dist <= 40.0f * rScale) {
                // 1. Center Hub (Pen / Highlighter / Laser toggle)
                g_radialHoverTarget = RadialTarget::Center;
                g_radialRecentFanOpen = false;
            }
            else if (dist > 40.0f * rScale && dist <= (g_settings.showRadialColorRing ? 106.0f : 96.0f) * rScale) {
                // 2. Action Ring: N Continuous Angular Sectors (Zero Dead Gaps)
                g_radialRecentFanOpen = false;
                int nSectors = (int)g_activeRadialSlots.size();
                if (nSectors > 0) {
                    float angle = std::atan2(dy, dx);
                    if (angle < 0) angle += 2.0f * 3.14159265358979323846f;
                    const float kSecStep = (float)(2.0 * 3.14159265358979323846 / (double)nSectors);
                    int sector = (int)((angle + kSecStep * 0.5f) / kSecStep) % nSectors;
                    RadialAction act = g_activeRadialSlots[sector];
                    bool sectorDisabled = (act == RadialAction::Clear && g_strokes.empty() && g_laserStrokes.empty()) ||
                                          (act == RadialAction::Undo && g_undoStack.empty()) ||
                                          (act == RadialAction::Redo && g_redoStack.empty());
                    if (!sectorDisabled) {
                        g_radialHoverSector = sector;
                        g_radialHoverTarget = RadialTarget::Sector;
                    }
                }
            }
            else if (g_settings.showRadialColorRing) {
                // 3. Outer Ring & Beyond: Color Orbs & Recent Colors Fan
                float angle = std::atan2(dy, dx);
                float degAngle = angle * (180.0f / 3.14159265358979323846f); // -180 to +180

                // Custom color hub center at 12 o'clock
                float hubX = g_radialX;
                float hubY = g_radialY - 126.0f * rScale;
                float hubDist = std::sqrt(DistanceSq(g_cursorX, g_cursorY, hubX, hubY));

                int numRecent = std::min(5, (int)g_recentColors.size());

                // If the recent colors satellite fan is currently open:
                if (g_radialRecentFanOpen && numRecent > 0) {
                    // Check if cursor remains within the upper fan sector (-150 deg to -30 deg, dy < 0)
                    bool inFanZone = (dy < 0.0f && degAngle >= -150.0f && degAngle <= -30.0f);
                    if (inFanZone) {
                        // User is inside the recent colors fan zone!
                        // Distinguish between the Custom Color Hub at 12 o'clock (lower) and Satellite Arc (upper).
                        // Transition threshold: midpoint between hub (126px) and satellite arc (162px) = 144px.
                        if (dist >= 144.0f * rScale) {
                            // In Satellite Arc: smoothly map by continuous angle to the nearest recent color orb
                            int bestRecent = 0;
                            float minAngleDiff = 999.0f;
                            for (int j = 0; j < numRecent; ++j) {
                                float orbAngle = -90.0f + (float)(j - (numRecent - 1) * 0.5f) * 16.0f;
                                float diff = std::abs(degAngle - orbAngle);
                                if (diff < minAngleDiff) {
                                    minAngleDiff = diff;
                                    bestRecent = j;
                                }
                            }
                            g_hoveredRecentOrb = bestRecent;
                            g_radialHoverTarget = RadialTarget::RecentOrb;
                        }
                        else {
                            // In Hub zone (closer to the 12 o'clock custom orb)
                            g_radialHoverTarget = RadialTarget::RecentHub;
                            g_hoveredRecentOrb = -1;
                        }
                        // Fan remains open throughout the entire fan zone
                        g_radialRecentFanOpen = true;

                        if (oldTarget != g_radialHoverTarget || oldSector != g_radialHoverSector || oldOrb != g_hoveredOrb || oldRecentOrb != g_hoveredRecentOrb) {
                            InvalidateOverlay();
                        }
                        return 0;
                    }
                    else {
                        // Cursor moved away from the fan zone; close fan and proceed to standard preset color mapping
                        g_radialRecentFanOpen = false;
                    }
                }

                // Standard continuous angular mapping to the 16 preset color orbs
                float relAngle = angle - (-3.14159265358979323846f * 0.5f);
                while (relAngle < 0.0f) relAngle += 2.0f * 3.14159265358979323846f;
                while (relAngle >= 2.0f * 3.14159265358979323846f) relAngle -= 2.0f * 3.14159265358979323846f;

                const float kOrbStep = (float)(2.0 * 3.14159265358979323846 / (double)kPresetColorCount);
                int orbIdx = (int)((relAngle + kOrbStep * 0.5f) / kOrbStep) % kPresetColorCount;

                if (orbIdx == 0 || hubDist <= 22.0f * rScale) {
                    g_radialHoverTarget = RadialTarget::RecentHub;
                    g_radialRecentFanOpen = (numRecent > 0);
                }
                else {
                    g_hoveredOrb = orbIdx;
                    g_radialHoverTarget = RadialTarget::ColorOrb;
                    g_radialRecentFanOpen = false;
                }
            }

            if (oldTarget != g_radialHoverTarget || oldSector != g_radialHoverSector || oldOrb != g_hoveredOrb || oldRecentOrb != g_hoveredRecentOrb) {
                InvalidateOverlay();
            }
            return 0;
        }

        // Check Shapes Flyout Item Hover
        if (g_shapesFlyoutOpen) {
            float scale = GetFlyoutDpiScale(g_shapesFlyoutRect);
            float padY = 6.0f * scale;
            float itemH = 32.0f * scale;
            int oldFlyoutHover = g_hoveredShapeFlyoutItem;
            g_hoveredShapeFlyoutItem = -1;
            if (g_cursorX >= g_shapesFlyoutRect.left && g_cursorX <= g_shapesFlyoutRect.right &&
                g_cursorY >= (g_shapesFlyoutRect.top + padY) && g_cursorY <= (g_shapesFlyoutRect.bottom - padY)) {
                float relY = g_cursorY - (g_shapesFlyoutRect.top + padY);
                int idx = (int)(relY / itemH);
                if (idx >= 0 && idx < 5) {
                    g_hoveredShapeFlyoutItem = idx;
                }
            }
            if (oldFlyoutHover != g_hoveredShapeFlyoutItem) {
                InvalidateOverlay();
            }
        }

        // Check Grid Flyout Item Hover
        if (g_gridFlyoutOpen) {
            float scale = GetFlyoutDpiScale(g_gridFlyoutRect);
            float padY = 6.0f * scale;
            float itemH = 30.0f * scale;
            float divH = 8.0f * scale;
            int oldGridHover = g_hoveredGridFlyoutItem;
            g_hoveredGridFlyoutItem = -1;
            if (g_cursorX >= g_gridFlyoutRect.left && g_cursorX <= g_gridFlyoutRect.right &&
                g_cursorY >= (g_gridFlyoutRect.top + padY) && g_cursorY <= (g_gridFlyoutRect.bottom - padY)) {
                float relY = g_cursorY - (g_gridFlyoutRect.top + padY);
                if (relY >= 0.0f && relY < 3.0f * itemH) {
                    g_hoveredGridFlyoutItem = (int)(relY / itemH);
                }
                else if (relY >= (3.0f * itemH + divH) && relY < (6.0f * itemH + divH)) {
                    g_hoveredGridFlyoutItem = 3 + (int)((relY - (3.0f * itemH + divH)) / itemH);
                }
            }
            if (oldGridHover != g_hoveredGridFlyoutItem) {
                InvalidateOverlay();
            }
        }

        // Check Backdrop Flyout Item Hover
        if (g_backdropFlyoutOpen) {
            float scale = GetFlyoutDpiScale(g_backdropFlyoutRect);
            float padY = 6.0f * scale;
            float itemH = 30.0f * scale;
            float divH = 8.0f * scale;
            int oldBackdropHover = g_hoveredBackdropFlyoutItem;
            g_hoveredBackdropFlyoutItem = -1;
            if (g_cursorX >= g_backdropFlyoutRect.left && g_cursorX <= g_backdropFlyoutRect.right &&
                g_cursorY >= (g_backdropFlyoutRect.top + padY) && g_cursorY <= (g_backdropFlyoutRect.bottom - padY)) {
                float relY = g_cursorY - (g_backdropFlyoutRect.top + padY);
                const auto& monitors = GetSystemMonitorList();
                bool hasMulti = (monitors.size() > 1);
                if (relY >= 0.0f && relY < 3.0f * itemH) {
                    g_hoveredBackdropFlyoutItem = (int)(relY / itemH);
                }
                else if (hasMulti && relY >= (3.0f * itemH + divH)) {
                    int secIdx = (int)((relY - (3.0f * itemH + divH)) / itemH);
                    int maxSec = 2 + (int)monitors.size() + 1; // ActiveCursor + Primary + N monitors + AllMonitors
                    if (secIdx >= 0 && secIdx < maxSec) {
                        g_hoveredBackdropFlyoutItem = 3 + secIdx;
                    }
                }
            }
            if (oldBackdropHover != g_hoveredBackdropFlyoutItem) {
                InvalidateOverlay();
            }
        }

        // Check Color Flyout Hover
        if (g_colorFlyoutOpen) {
            int oldSw = g_hoveredRecentSwatch;
            int oldAct = g_hoveredColorStudioAction;
            g_hoveredRecentSwatch = -1;
            g_hoveredColorStudioAction = -1;

            if (g_cursorX >= g_colorFlyoutRect.left && g_cursorX <= g_colorFlyoutRect.right &&
                g_cursorY >= g_colorFlyoutRect.top && g_cursorY <= g_colorFlyoutRect.bottom) {

                float scale = GetFlyoutDpiScale(g_colorFlyoutRect);
                const float padX = 14.0f * scale;
                const float contentW = (g_colorFlyoutRect.right - g_colorFlyoutRect.left) - padX * 2.0f;
                float curY = g_colorFlyoutRect.top + 12.0f * scale;

                // Test Recent Swatches
                const float swatchDiam = 22.0f * scale;
                const float swatchGap = (contentW - 5.0f * swatchDiam) / 4.0f;
                for (int k = 0; k < 5; ++k) {
                    float sx = g_colorFlyoutRect.left + padX + k * (swatchDiam + swatchGap) + swatchDiam * 0.5f;
                    float sy = curY + swatchDiam * 0.5f;
                    if (DistanceSq(g_cursorX, g_cursorY, sx, sy) <= (14.0f * scale) * (14.0f * scale)) {
                        g_hoveredRecentSwatch = k;
                        break;
                    }
                }

                // Test Eyedropper and Copy buttons
                float canvasH = 118.0f * scale;
                float trackH = 12.0f * scale;
                float bottomY = curY + swatchDiam + 10.0f * scale + canvasH + 10.0f * scale + trackH + 10.0f * scale + trackH + 12.0f * scale;
                float hexW = 92.0f * scale;
                float swatchBoxW = 32.0f * scale;
                float btnBoxW = 32.0f * scale;
                const float gap = 8.0f * scale;

                float dropLeft = g_colorFlyoutRect.left + padX + hexW + gap + swatchBoxW + gap;
                float copyLeft = dropLeft + btnBoxW + gap;

                if (g_cursorY >= bottomY && g_cursorY <= bottomY + 30.0f * scale) {
                    if (g_cursorX >= dropLeft && g_cursorX <= dropLeft + btnBoxW) {
                        g_hoveredColorStudioAction = 1;
                    } else if (g_cursorX >= copyLeft && g_cursorX <= copyLeft + btnBoxW) {
                        g_hoveredColorStudioAction = 2;
                    }
                }
            }

            if (oldSw != g_hoveredRecentSwatch || oldAct != g_hoveredColorStudioAction) {
                InvalidateOverlay();
            }
        }

        // Check Toolbar Button Hover
        int oldBtn = g_hoveredToolbarBtn;
        g_hoveredToolbarBtn = -1;
        if (g_settings.showBottomToolbar &&
            g_cursorX >= g_toolbarRect.left && g_cursorX <= g_toolbarRect.right &&
            g_cursorY >= g_toolbarRect.top && g_cursorY <= g_toolbarRect.bottom) {
            for (size_t i = 0; i < g_toolbarButtons.size(); ++i) {
                const auto& b = g_toolbarButtons[i].rect;
                if (g_cursorX >= b.left && g_cursorX <= b.right &&
                    g_cursorY >= b.top && g_cursorY <= b.bottom) {
                    g_hoveredToolbarBtn = (int)i;
                    break;
                }
            }
        }
        if (oldBtn != g_hoveredToolbarBtn) {
            InvalidateOverlay();
        }

        // Active Left-Click Brush Erase Drag (when in Eraser mode)
        if (g_isLeftClickErasing) {
            EraseWholeShapeAt(g_cursorX, g_cursorY, g_eraserRadius);
            InvalidateOverlay();
            return 0;
        }

        // Active Right-Click Whole-Shape Clear Drag (when in Eraser mode)
        if (g_isRightClickClearing) {
            EraseWholeShapeAt(g_cursorX, g_cursorY, g_eraserRadius);
            InvalidateOverlay();
            return 0;
        }

        // Right-Click Hold Eraser Brush (in normal drawing modes) or Whole-Shape Clear (in Eraser mode)
        if (g_isRightMouseDown) {
            float distMoved = std::sqrt(DistanceSq(g_cursorX, g_cursorY, (float)g_rightMouseDownPos.x, (float)g_rightMouseDownPos.y));
            if (distMoved > 4.0f || (GetTickCount64() - g_rightMouseDownTime > 120)) {
                if (g_currentTool == ToolMode::Eraser) {
                    if (!g_isRightClickClearing) {
                        g_isRightClickClearing = true;
                    }
                }
                else {
                    if (!g_isRightClickErasing) {
                        g_isRightClickErasing = true;
                    }
                }
            }
            if (g_isRightClickClearing) {
                EraseWholeShapeAt(g_cursorX, g_cursorY, g_eraserRadius);
                InvalidateOverlay();
                return 0;
            }
            if (g_isRightClickErasing) {
                EraseWholeShapeAt(g_cursorX, g_cursorY, g_eraserRadius);
                InvalidateOverlay();
                return 0;
            }
        }

        // Active Panning
        if (g_isPanning) {
            g_panOffsetX += (g_cursorX - g_panStartPos.x);
            g_panOffsetY += (g_cursorY - g_panStartPos.y);
            g_panStartPos = { (LONG)g_cursorX, (LONG)g_cursorY };
            InvalidateOverlay();
            return 0;
        }

        // Active Drawing
        if (g_isDrawing) {
            float adjX = (g_cursorX - g_panOffsetX) / g_zoomScale;
            float adjY = (g_cursorY - g_panOffsetY) / g_zoomScale;

            if (g_currentStroke.shapeType == ShapeType::Freehand) {
                g_currentStroke.points.push_back({ adjX, adjY });
            }
            else {
                if (GetKeyState(VK_SHIFT) & 0x8000) {
                    float dx = adjX - g_currentStroke.startPt.x;
                    float dy = adjY - g_currentStroke.startPt.y;

                    if (g_currentStroke.shapeType == ShapeType::Line || g_currentStroke.shapeType == ShapeType::Arrow) {
                        float len = std::sqrt(dx * dx + dy * dy);
                        if (len > 0.001f) {
                            float angle = std::atan2(dy, dx);
                            float snapAngle = std::round(angle / (3.14159265f / 4.0f)) * (3.14159265f / 4.0f);
                            adjX = g_currentStroke.startPt.x + len * std::cos(snapAngle);
                            adjY = g_currentStroke.startPt.y + len * std::sin(snapAngle);
                        }
                    }
                    else if (g_currentStroke.shapeType == ShapeType::Rectangle || g_currentStroke.shapeType == ShapeType::Ellipse) {
                        float side = std::max(std::abs(dx), std::abs(dy));
                        adjX = g_currentStroke.startPt.x + (dx >= 0 ? side : -side);
                        adjY = g_currentStroke.startPt.y + (dy >= 0 ? side : -side);
                    }
                    else if (g_currentStroke.shapeType == ShapeType::Triangle) {
                        float side = std::max(std::abs(dx), std::abs(dy));
                        adjX = g_currentStroke.startPt.x + (dx >= 0 ? side : -side);
                        adjY = g_currentStroke.startPt.y + (dy >= 0 ? (side * 0.866025f) : (-side * 0.866025f));
                    }
                }
                g_currentStroke.endPt = { adjX, adjY };
            }
            InvalidateOverlay();
            return 0;
        }

        // Active Laser Pointer & Ephemeral Trail
        if (g_currentTool == ToolMode::Laser) {
            if (g_isLaserDrawing) {
                float adjX = (g_cursorX - g_panOffsetX) / g_zoomScale;
                float adjY = (g_cursorY - g_panOffsetY) / g_zoomScale;
                if (g_laserStrokes.empty()) {
                    g_laserStrokes.emplace_back();
                    g_laserStrokes.back().points.push_back({ adjX, adjY, GetTickCount64() });
                } else {
                    auto& pts = g_laserStrokes.back().points;
                    if (!pts.empty()) {
                        float dSq = DistanceSq(adjX, adjY, pts.back().x, pts.back().y);
                        // Filter micro-movements to eliminate point clustering (which causes dotted artifacts)
                        if (dSq >= 9.0f) { // >= 3px
                            // Multi-step interpolation: subdivide large jumps based on distance to guarantee buttery-smooth curves
                            if (dSq > 144.0f) { // > 12px
                                float dist = std::sqrt(dSq);
                                int steps = std::min(16, (int)std::ceil(dist / 10.0f));
                                float startX = pts.back().x;
                                float startY = pts.back().y;
                                ULONGLONG startT = pts.back().timestamp;
                                ULONGLONG nowT = GetTickCount64();
                                for (int s = 1; s < steps; ++s) {
                                    float t = (float)s / (float)steps;
                                    float midX = startX + (adjX - startX) * t;
                                    float midY = startY + (adjY - startY) * t;
                                    ULONGLONG midT = startT + (ULONGLONG)((float)(nowT - startT) * t);
                                    pts.push_back({ midX, midY, midT });
                                }
                            }
                            pts.push_back({ adjX, adjY, GetTickCount64() });
                            g_laserStrokes.back().InvalidateGeometry();
                            SetTimer(hwnd, TIMER_ID_LASER, 16, NULL);
                        }
                    } else {
                        pts.push_back({ adjX, adjY, GetTickCount64() });
                        g_laserStrokes.back().InvalidateGeometry();
                    }
                }
            }
            InvalidateOverlay();
            return 0;
        }

        // When Eraser mode is active or right-click is held down, red circle follows mouse smoothly!
        if (g_currentTool == ToolMode::Eraser || g_isRightMouseDown) {
            InvalidateOverlay();
            return 0;
        }

        // When Pen or Highlighter mode is active, custom cursor follows mouse smoothly!
        if (g_currentTool == ToolMode::Pen || g_currentTool == ToolMode::Highlighter) {
            InvalidateOverlay();
            return 0;
        }

        break;
    }

    case WM_LBUTTONDOWN: {
        float x = (float)GET_X_LPARAM(lParam);
        float y = (float)GET_Y_LPARAM(lParam);
        g_hasPushedUndoForCurrentErase = false;

        // Clicking the tray icon while WinDraw is open closes WinDraw
        if (IsClickOnTrayIcon()) {
            HideOverlay();
            return 0;
        }

        // Eyedropper Desktop Pixel Sampling
        if (g_isEyedropperActive) {
            POINT screenPt;
            GetCursorPos(&screenPt);
            HDC hdc = GetDC(NULL);
            if (hdc) {
                COLORREF cr = GetPixel(hdc, screenPt.x, screenPt.y);
                ReleaseDC(NULL, hdc);
                float r = (float)GetRValue(cr) / 255.0f;
                float g = (float)GetGValue(cr) / 255.0f;
                float b = (float)GetBValue(cr) / 255.0f;
                g_customColor.activeColor = D2D1::ColorF(r, g, b, g_customColor.alpha);
                RGBtoHSV(g_customColor.activeColor, g_customColor.hue, g_customColor.sat, g_customColor.val);
                g_activeColor = g_customColor.activeColor;
                PushRecentColor(g_activeColor);
                SavePersistentCustomColor();
            }
            g_isEyedropperActive = false;
            ShowToastNotification(L"Color Sampled!");
            BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
            InvalidateOverlay();
            return 0;
        }

        // Snipping drag start
        if (g_isSnipping) {
            g_isSnippingDrag = true;
            g_snipStartPt = { (LONG)x, (LONG)y };
            g_snipEndPt = g_snipStartPt;
            SetCapture(hwnd);
            InvalidateOverlay();
            return 0;
        }

        // Radial Menu selection
        if (g_radialActive) {
            if (g_radialHoverTarget == RadialTarget::Center) {
                // Cycle between Pen, Highlighter, and Laser
                if (g_currentTool == ToolMode::Pen) {
                    SetToolMode(ToolMode::Highlighter);
                } else if (g_currentTool == ToolMode::Highlighter) {
                    SetToolMode(ToolMode::Laser);
                } else {
                    SetToolMode(ToolMode::Pen);
                }
            }
            else if (g_radialHoverTarget == RadialTarget::Sector && g_radialHoverSector >= 0 && g_radialHoverSector < (int)g_activeRadialSlots.size()) {
                RadialAction act = g_activeRadialSlots[g_radialHoverSector];
                switch (act) {
                    case RadialAction::Clear: {
                        if (!g_strokes.empty() || !g_laserStrokes.empty()) {
                            if (!g_strokes.empty()) PushUndoState();
                            g_strokes.clear();
                            g_laserStrokes.clear();
                            if (!g_isLaserDrawing) KillTimer(hwnd, TIMER_ID_LASER);
                        }
                        break;
                    }
                    case RadialAction::Snapshot: {
                        if (GetKeyState(VK_CONTROL) & 0x8000) {
                            CaptureFullScreenSnapshot();
                        } else {
                            StartSnipping();
                        }
                        break;
                    }
                    case RadialAction::Snip: {
                        StartSnipping();
                        break;
                    }
                    case RadialAction::Eraser: {
                        SetToolMode(ToolMode::Eraser);
                        break;
                    }
                    case RadialAction::Undo: {
                        PerformUndo();
                        break;
                    }
                    case RadialAction::Redo: {
                        PerformRedo();
                        break;
                    }
                    case RadialAction::Pointer: {
                        SetToolMode(ToolMode::Pointer);
                        break;
                    }
                    case RadialAction::InkVisible: {
                        g_inkVisible = !g_inkVisible;
                        break;
                    }
                    case RadialAction::Pan: {
                        SetToolMode(ToolMode::Pan);
                        break;
                    }
                    case RadialAction::Pen: {
                        g_currentShape = ShapeType::Freehand;
                        SetToolMode(ToolMode::Pen);
                        BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
                        break;
                    }
                    case RadialAction::Highlighter: {
                        SetToolMode(ToolMode::Highlighter);
                        break;
                    }
                    case RadialAction::Laser: {
                        SetToolMode(ToolMode::Laser);
                        break;
                    }
                    case RadialAction::Shape: {
                        if (g_currentShape == ShapeType::Freehand) {
                            g_currentShape = ShapeType::Line;
                        }
                        SetToolMode(ToolMode::Pen);
                        BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
                        break;
                    }
                    case RadialAction::Grid: {
                        if (g_gridStyle == GridStyle::None) {
                            g_gridStyle = GridStyle::DotGrid;
                            ShowToastNotification(L"Grid: Dot Grid");
                        } else if (g_gridStyle == GridStyle::DotGrid) {
                            g_gridStyle = GridStyle::GraphLines;
                            ShowToastNotification(L"Grid: Graph Lines");
                        } else {
                            g_gridStyle = GridStyle::None;
                            ShowToastNotification(L"Grid: Off");
                        }
                        RebuildGridBrush();
                        break;
                    }
                    case RadialAction::Whiteboard: {
                        CycleCanvasBackground();
                        break;
                    }
                    case RadialAction::Exit: {
                        g_radialActive = false;
                        g_radialHoverTarget = RadialTarget::None;
                        g_radialHoverSector = -1;
                        g_hoveredOrb = -1;
                        g_hoveredRecentOrb = -1;
                        g_radialRecentFanOpen = false;
                        HideOverlay();
                        return 0;
                    }
                    default:
                        break;
                }
            }
            else if (g_radialHoverTarget == RadialTarget::ColorOrb && g_hoveredOrb >= 0 && g_hoveredOrb < (int)kPresetColorCount) {
                g_activeColor = kPresetColors[g_hoveredOrb];
                SetToolMode(ToolMode::Pen);
            }
            else if (g_radialHoverTarget == RadialTarget::RecentOrb && g_hoveredRecentOrb >= 0 && g_hoveredRecentOrb < (int)g_recentColors.size()) {
                g_activeColor = g_recentColors[g_hoveredRecentOrb];
                g_customColor.activeColor = g_activeColor;
                RGBtoHSV(g_activeColor, g_customColor.hue, g_customColor.sat, g_customColor.val);
                g_customColor.alpha = g_activeColor.a;
                SetToolMode(ToolMode::Pen);
                BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
            }
            else if (g_radialHoverTarget == RadialTarget::RecentHub) {
                if (g_settings.showBottomToolbar) {
                    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                    if (g_toolbarCollapsed) {
                        g_toolbarCollapsed = false;
                        float pillCenterX = (g_toolbarRect.left + g_toolbarRect.right) * 0.5f;
                        g_toolbarCustomX = pillCenterX - g_toolbarExpandedWidth * 0.5f;
                        if (g_toolbarCustomX < 10.0f) g_toolbarCustomX = 10.0f;
                        if (g_toolbarCustomX + g_toolbarExpandedWidth > (float)vw - 10.0f) g_toolbarCustomX = (float)vw - g_toolbarExpandedWidth - 10.0f;
                        g_colorFlyoutOpen = true;
                    } else {
                        g_colorFlyoutOpen = !g_colorFlyoutOpen;
                    }
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    g_backdropFlyoutOpen = false;
                    BuildToolbarLayout(vw, vh);
                }
                g_activeColor = g_customColor.activeColor;
                SetToolMode(ToolMode::Pen);
            }

            g_radialActive = false;
            g_radialHoverTarget = RadialTarget::None;
            g_radialHoverSector = -1;
            g_hoveredOrb = -1;
            g_hoveredRecentOrb = -1;
            g_radialRecentFanOpen = false;
            InvalidateOverlay();
            return 0;
        }

        // Shapes Flyout Selection
        if (g_shapesFlyoutOpen) {
            if (x >= g_shapesFlyoutRect.left && x <= g_shapesFlyoutRect.right &&
                y >= g_shapesFlyoutRect.top && y <= g_shapesFlyoutRect.bottom) {
                float scale = GetFlyoutDpiScale(g_shapesFlyoutRect);
                float padY = 6.0f * scale;
                float itemH = 32.0f * scale;
                float relY = y - (g_shapesFlyoutRect.top + padY);
                int idx = (int)(relY / itemH);
                ShapeType shapeOptions[] = {
                    ShapeType::Line,
                    ShapeType::Arrow,
                    ShapeType::Rectangle,
                    ShapeType::Ellipse,
                    ShapeType::Triangle
                };
                if (idx >= 0 && idx < 5) {
                    g_currentShape = shapeOptions[idx];
                    SetToolMode(ToolMode::Pen);
                    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                    BuildToolbarLayout(vw, vh);
                }
                g_shapesFlyoutOpen = false;
                InvalidateOverlay();
                return 0;
            } else {
                g_shapesFlyoutOpen = false;
                InvalidateOverlay();

                // If user clicked directly on Shapes button (id 25), dismiss without re-opening
                bool clickedShapesBtn = false;
                if (g_settings.showBottomToolbar &&
                    x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                    y >= g_toolbarRect.top && y <= g_toolbarRect.bottom) {
                    for (const auto& b : g_toolbarButtons) {
                        if (b.id == 25 && x >= b.rect.left && x <= b.rect.right && y >= b.rect.top && y <= b.rect.bottom) {
                            clickedShapesBtn = true;
                            break;
                        }
                    }
                }
                if (clickedShapesBtn) {
                    return 0;
                }

                // If clicked on canvas outside the toolbar, dismiss flyout and do not start inking
                if (!(g_settings.showBottomToolbar &&
                      x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                      y >= g_toolbarRect.top && y <= g_toolbarRect.bottom)) {
                    return 0;
                }
            }
        }

        // Grid Flyout Selection
        if (g_gridFlyoutOpen) {
            if (x >= g_gridFlyoutRect.left && x <= g_gridFlyoutRect.right &&
                y >= g_gridFlyoutRect.top && y <= g_gridFlyoutRect.bottom) {
                float scale = GetFlyoutDpiScale(g_gridFlyoutRect);
                float padY = 6.0f * scale;
                float itemH = 30.0f * scale;
                float divH = 8.0f * scale;
                float relY = y - (g_gridFlyoutRect.top + padY);
                int clickedIdx = -1;
                if (relY >= 0.0f && relY < 3.0f * itemH) {
                    clickedIdx = (int)(relY / itemH);
                }
                else if (relY >= (3.0f * itemH + divH) && relY < (6.0f * itemH + divH)) {
                    clickedIdx = 3 + (int)((relY - (3.0f * itemH + divH)) / itemH);
                }

                if (clickedIdx == 0) {
                    g_gridStyle = GridStyle::None;
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (clickedIdx == 1) {
                    g_gridStyle = GridStyle::DotGrid;
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (clickedIdx == 2) {
                    g_gridStyle = GridStyle::GraphLines;
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (clickedIdx == 3) {
                    g_gridDensity = GridDensity::Fine;
                    if (g_gridStyle == GridStyle::None) g_gridStyle = GridStyle::DotGrid;
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (clickedIdx == 4) {
                    g_gridDensity = GridDensity::Medium;
                    if (g_gridStyle == GridStyle::None) g_gridStyle = GridStyle::DotGrid;
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (clickedIdx == 5) {
                    g_gridDensity = GridDensity::Coarse;
                    if (g_gridStyle == GridStyle::None) g_gridStyle = GridStyle::DotGrid;
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                return 0;
            }
            else {
                g_gridFlyoutOpen = false;
                InvalidateOverlay();

                // If user clicked directly on Grid button (id 6), dismiss without re-opening
                bool clickedGridBtn = false;
                if (g_settings.showBottomToolbar &&
                    x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                    y >= g_toolbarRect.top && y <= g_toolbarRect.bottom) {
                    for (const auto& b : g_toolbarButtons) {
                        if (b.id == 6 && x >= b.rect.left && x <= b.rect.right && y >= b.rect.top && y <= b.rect.bottom) {
                            clickedGridBtn = true;
                            break;
                        }
                    }
                }
                if (clickedGridBtn) {
                    return 0;
                }

                // If clicked on canvas outside toolbar, swallow to prevent accidental inking
                if (!(g_settings.showBottomToolbar &&
                      x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                      y >= g_toolbarRect.top && y <= g_toolbarRect.bottom)) {
                    return 0;
                }
            }
        }

        // Backdrop Flyout Selection
        if (g_backdropFlyoutOpen) {
            if (x >= g_backdropFlyoutRect.left && x <= g_backdropFlyoutRect.right &&
                y >= g_backdropFlyoutRect.top && y <= g_backdropFlyoutRect.bottom) {
                float scale = GetFlyoutDpiScale(g_backdropFlyoutRect);
                float padY = 6.0f * scale;
                float itemH = 30.0f * scale;
                float divH = 8.0f * scale;
                float relY = y - (g_backdropFlyoutRect.top + padY);
                const auto& monitors = GetSystemMonitorList();
                bool hasMulti = (monitors.size() > 1);
                int clickedIdx = -1;
                if (relY >= 0.0f && relY < 3.0f * itemH) {
                    clickedIdx = (int)(relY / itemH);
                }
                else if (hasMulti && relY >= (3.0f * itemH + divH)) {
                    int secIdx = (int)((relY - (3.0f * itemH + divH)) / itemH);
                    int maxSec = 2 + (int)monitors.size() + 1;
                    if (secIdx >= 0 && secIdx < maxSec) {
                        clickedIdx = 3 + secIdx;
                    }
                }

                if (clickedIdx == 0) {
                    g_canvasBg = CanvasBg::Transparent;
                    ShowToastNotification(L"Screen Mode (Transparent)");
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (clickedIdx == 1) {
                    g_canvasBg = CanvasBg::Whiteboard;
                    ShowToastNotification(L"Whiteboard Mode (Paper)");
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (clickedIdx == 2) {
                    g_canvasBg = CanvasBg::Blackboard;
                    ShowToastNotification(L"Blackboard Mode (Dark Slate)");
                    RebuildGridBrush();
                    InvalidateOverlay();
                }
                else if (hasMulti && clickedIdx >= 3) {
                    int monitorChoice = clickedIdx - 3;
                    if (monitorChoice == 0) {
                        g_canvasScope = CanvasMonitorScope::ActiveCursor;
                        ShowToastNotification(L"Target: Active Screen (Follows Cursor)");
                    }
                    else if (monitorChoice == 1) {
                        g_canvasScope = CanvasMonitorScope::Primary;
                        ShowToastNotification(L"Target: Primary Screen");
                    }
                    else if (monitorChoice >= 2 && monitorChoice <= 1 + (int)monitors.size()) {
                        int scrIdx = monitorChoice - 1;
                        g_canvasScope = (CanvasMonitorScope)scrIdx;
                        wchar_t buf[64];
                        wsprintfW(buf, L"Target: Screen %d", scrIdx);
                        ShowToastNotification(buf);
                    }
                    else {
                        g_canvasScope = CanvasMonitorScope::AllMonitors;
                        ShowToastNotification(L"Target: All Screens");
                    }
                    if (g_canvasBg == CanvasBg::Transparent) {
                        g_canvasBg = CanvasBg::Whiteboard;
                        RebuildGridBrush();
                    }
                    InvalidateOverlay();
                }
                return 0;
            }
            else {
                g_backdropFlyoutOpen = false;
                g_hoveredBackdropFlyoutItem = -1;
                InvalidateOverlay();

                // If user clicked directly on Whiteboard button (id 8), dismiss without re-opening
                bool clickedBackdropBtn = false;
                if (g_settings.showBottomToolbar &&
                    x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                    y >= g_toolbarRect.top && y <= g_toolbarRect.bottom) {
                    for (const auto& b : g_toolbarButtons) {
                        if (b.id == 8 && x >= b.rect.left && x <= b.rect.right && y >= b.rect.top && y <= b.rect.bottom) {
                            clickedBackdropBtn = true;
                            break;
                        }
                    }
                }
                if (clickedBackdropBtn) {
                    return 0;
                }

                // If clicked on canvas outside toolbar, swallow to prevent accidental inking
                if (!(g_settings.showBottomToolbar &&
                      x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                      y >= g_toolbarRect.top && y <= g_toolbarRect.bottom)) {
                    return 0;
                }
            }
        }

        // Color Studio Flyout Selection & Interaction
        if (g_colorFlyoutOpen) {
            if (x >= g_colorFlyoutRect.left && x <= g_colorFlyoutRect.right &&
                y >= g_colorFlyoutRect.top && y <= g_colorFlyoutRect.bottom) {

                float scale = GetFlyoutDpiScale(g_colorFlyoutRect);
                const float padX = 14.0f * scale;
                const float contentW = (g_colorFlyoutRect.right - g_colorFlyoutRect.left) - padX * 2.0f;
                float curY = g_colorFlyoutRect.top + 12.0f * scale;

                // 1. Recent Colors Swatches
                const float swatchDiam = 22.0f * scale;
                const float swatchGap = (contentW - 5.0f * swatchDiam) / 4.0f;
                for (int k = 0; k < 5; ++k) {
                    float sx = g_colorFlyoutRect.left + padX + k * (swatchDiam + swatchGap) + swatchDiam * 0.5f;
                    float sy = curY + swatchDiam * 0.5f;
                    if (DistanceSq(x, y, sx, sy) <= (14.0f * scale) * (14.0f * scale)) {
                        if (k < (int)g_recentColors.size()) {
                            g_customColor.activeColor = g_recentColors[k];
                            RGBtoHSV(g_customColor.activeColor, g_customColor.hue, g_customColor.sat, g_customColor.val);
                            g_customColor.alpha = g_customColor.activeColor.a;
                            g_activeColor = g_customColor.activeColor;
                            SetToolMode(ToolMode::Pen);
                            BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
                            InvalidateOverlay();
                        }
                        return 0;
                    }
                }
                curY += swatchDiam + 10.0f * scale;

                // 2. 2D Saturation / Value Canvas
                const float canvasH = 118.0f * scale;
                if (x >= (g_colorFlyoutRect.left + padX) && x <= (g_colorFlyoutRect.right - padX) &&
                    y >= curY && y <= curY + canvasH) {
                    g_pickerDrag = ColorPickerDrag::SatValCanvas;
                    SetCapture(hwnd);
                    float s = (x - (g_colorFlyoutRect.left + padX)) / contentW;
                    float v = 1.0f - (y - curY) / canvasH;
                    g_customColor.sat = std::max(0.0f, std::min(1.0f, s));
                    g_customColor.val = std::max(0.0f, std::min(1.0f, v));
                    g_customColor.activeColor = HSVtoRGB(g_customColor.hue, g_customColor.sat, g_customColor.val, g_customColor.alpha);
                    g_activeColor = g_customColor.activeColor;
                    InvalidateOverlay();
                    return 0;
                }
                curY += canvasH + 10.0f * scale;

                // 3. Rainbow Hue Track
                const float trackH = 12.0f * scale;
                if (x >= (g_colorFlyoutRect.left + padX) && x <= (g_colorFlyoutRect.right - padX) &&
                    y >= (curY - 3.0f * scale) && y <= (curY + trackH + 3.0f * scale)) {
                    g_pickerDrag = ColorPickerDrag::HueBar;
                    SetCapture(hwnd);
                    float h = ((x - (g_colorFlyoutRect.left + padX)) / contentW) * 360.0f;
                    g_customColor.hue = std::max(0.0f, std::min(360.0f, h));
                    g_customColor.activeColor = HSVtoRGB(g_customColor.hue, g_customColor.sat, g_customColor.val, g_customColor.alpha);
                    g_activeColor = g_customColor.activeColor;
                    InvalidateOverlay();
                    return 0;
                }
                curY += trackH + 10.0f * scale;

                // 4. Alpha Track
                if (x >= (g_colorFlyoutRect.left + padX) && x <= (g_colorFlyoutRect.right - padX) &&
                    y >= (curY - 3.0f * scale) && y <= (curY + trackH + 3.0f * scale)) {
                    g_pickerDrag = ColorPickerDrag::AlphaBar;
                    SetCapture(hwnd);
                    float a = (x - (g_colorFlyoutRect.left + padX)) / contentW;
                    g_customColor.alpha = std::max(0.05f, std::min(1.0f, a));
                    g_customColor.activeColor = HSVtoRGB(g_customColor.hue, g_customColor.sat, g_customColor.val, g_customColor.alpha);
                    g_activeColor = g_customColor.activeColor;
                    InvalidateOverlay();
                    return 0;
                }
                curY += trackH + 12.0f * scale;

                // 5. Bottom Row: Hex box, Eyedropper, Copy
                const float rowH = 30.0f * scale;
                const float hexW = 92.0f * scale;
                const float swatchBoxW = 32.0f * scale;
                const float btnBoxW = 32.0f * scale;
                const float gap = 8.0f * scale;

                float dropLeft = g_colorFlyoutRect.left + padX + hexW + gap + swatchBoxW + gap;
                float copyLeft = dropLeft + btnBoxW + gap;

                if (y >= curY && y <= curY + rowH) {
                    if (x >= dropLeft && x <= dropLeft + btnBoxW) {
                        // Eyedropper activate
                        g_isEyedropperActive = true;
                        SetCursor(LoadCursor(NULL, IDC_CROSS));
                        InvalidateOverlay();
                        return 0;
                    }
                    else if ((x >= copyLeft && x <= copyLeft + btnBoxW) ||
                             (x >= (g_colorFlyoutRect.left + padX) && x <= (g_colorFlyoutRect.left + padX + hexW))) {
                        // Copy Hex code to clipboard
                        std::wstring hexStr = ColorToHex(g_customColor.activeColor, false);
                        if (OpenClipboard(hwnd)) {
                            EmptyClipboard();
                            size_t lenBytes = (hexStr.length() + 1) * sizeof(wchar_t);
                            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lenBytes);
                            if (hMem) {
                                void* pLock = GlobalLock(hMem);
                                if (pLock) {
                                    memcpy(pLock, hexStr.c_str(), lenBytes);
                                    GlobalUnlock(hMem);
                                    SetClipboardData(CF_UNICODETEXT, hMem);
                                }
                            }
                            CloseClipboard();
                        }
                        ShowToastNotification(L"Hex Copied!");
                        InvalidateOverlay();
                        return 0;
                    }
                }

                // Any other click inside flyout modal is swallowed
                return 0;
            }
            else {
                // Click was outside Color Flyout
                g_colorFlyoutOpen = false;
                InvalidateOverlay();

                // If user clicked directly on Custom Color button (id 104), dismiss without re-opening
                bool clickedCustomColorBtn = false;
                if (g_settings.showBottomToolbar &&
                    x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                    y >= g_toolbarRect.top && y <= g_toolbarRect.bottom) {
                    for (const auto& b : g_toolbarButtons) {
                        if (b.id == 104 && x >= b.rect.left && x <= b.rect.right && y >= b.rect.top && y <= b.rect.bottom) {
                            clickedCustomColorBtn = true;
                            break;
                        }
                    }
                }
                if (clickedCustomColorBtn) {
                    return 0;
                }

                // If clicked on canvas outside toolbar, swallow to prevent accidental inking
                if (!(g_settings.showBottomToolbar &&
                      x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
                      y >= g_toolbarRect.top && y <= g_toolbarRect.bottom)) {
                    return 0;
                }
            }
        }

        // Collapsed Toolbar interaction: capture for drag or click-expand
        if (g_toolbarCollapsed && g_settings.showBottomToolbar &&
            x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
            y >= g_toolbarRect.top && y <= g_toolbarRect.bottom) {
            g_isPillMouseDown = true;
            g_isPillDragging = false;
            g_toolbarDragStart = { (LONG)x, (LONG)y };
            g_toolbarDragStartInit = { (LONG)x, (LONG)y };
            SetCapture(hwnd);
            return 0;
        }

        // Toolbar Drag Handle Check (Expanded mode only)
        if (!g_toolbarCollapsed && (g_hoveredToolbarBtn == 0 || (x >= g_toolbarRect.left && x <= g_toolbarRect.right && y >= g_toolbarRect.top && y <= g_toolbarRect.bottom && g_hoveredToolbarBtn == -1))) {
            g_isDraggingToolbar = true;
            g_toolbarDragStart = { (LONG)x, (LONG)y };
            SetCapture(hwnd);
            return 0;
        }

        // Toolbar Button Click - verify button under (x, y)
        if (g_settings.showBottomToolbar &&
            x >= g_toolbarRect.left && x <= g_toolbarRect.right &&
            y >= g_toolbarRect.top && y <= g_toolbarRect.bottom) {
            for (size_t i = 0; i < g_toolbarButtons.size(); ++i) {
                const auto& b = g_toolbarButtons[i].rect;
                if (x >= b.left && x <= b.right && y >= b.top && y <= b.bottom) {
                    g_hoveredToolbarBtn = (int)i;
                    break;
                }
            }
        }

        if (g_hoveredToolbarBtn >= 0 && g_hoveredToolbarBtn < (int)g_toolbarButtons.size()) {
            const auto btn = g_toolbarButtons[g_hoveredToolbarBtn];
            if (!btn.isPen) {
                if ((btn.id == 11 && g_undoStack.empty()) ||
                    (btn.id == 12 && g_redoStack.empty()) ||
                    (btn.id == 13 && g_strokes.empty())) {
                    return 0;
                }
            }
            if (btn.isPen) {
                if (btn.isCustomColor) {
                    g_colorFlyoutOpen = !g_colorFlyoutOpen;
                    g_activeColor = g_customColor.activeColor;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    g_backdropFlyoutOpen = false;
                    SetToolMode(ToolMode::Pen);
                    SetForegroundWindow(hwnd);
                }
                else {
                    g_colorFlyoutOpen = false;
                    g_activeColor = btn.penColor;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    g_backdropFlyoutOpen = false;
                    SetToolMode(ToolMode::Pen);
                    SetForegroundWindow(hwnd);
                }
            }
            else {
                if (btn.id != 8) {
                    g_backdropFlyoutOpen = false;
                }
                switch (btn.id) {
                case 1: // Highlighter
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    SetToolMode((g_currentTool == ToolMode::Highlighter) ? ToolMode::Pen : ToolMode::Highlighter);
                    SetForegroundWindow(hwnd);
                    break;
                case 7: // Laser Pointer
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    SetToolMode((g_currentTool == ToolMode::Laser) ? ToolMode::Pen : ToolMode::Laser);
                    SetForegroundWindow(hwnd);
                    break;
                case 2: // Eraser
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    SetToolMode((g_currentTool == ToolMode::Eraser) ? ToolMode::Pen : ToolMode::Eraser);
                    SetForegroundWindow(hwnd);
                    break;
                case 3: // Pan
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    SetToolMode((g_currentTool == ToolMode::Pan) ? ToolMode::Pen : ToolMode::Pan);
                    SetForegroundWindow(hwnd);
                    break;
                case 4: // Pointer (Click-Through)
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    SetToolMode((g_currentTool == ToolMode::Pointer) ? ToolMode::Pen : ToolMode::Pointer);
                    break;
                case 5: // Eye Visibility
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    g_inkVisible = !g_inkVisible;
                    break;
                case 6: // Grid Settings Flyout Modal
                    g_colorFlyoutOpen = false;
                    g_gridFlyoutOpen = !g_gridFlyoutOpen;
                    g_shapesFlyoutOpen = false;
                    SetForegroundWindow(hwnd);
                    break;
                case 8: // Whiteboard / Blackboard Mode Dropdown Flyout
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    g_backdropFlyoutOpen = !g_backdropFlyoutOpen;
                    SetForegroundWindow(hwnd);
                    break;
                case 20: // Freehand
                    g_colorFlyoutOpen = false;
                    g_currentShape = ShapeType::Freehand;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    SetToolMode(ToolMode::Pen);
                    BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
                    SetForegroundWindow(hwnd);
                    break;
                case 25: // Shapes Action Modal Toggle
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = !g_shapesFlyoutOpen;
                    g_gridFlyoutOpen = false;
                    if (g_shapesFlyoutOpen) {
                        SetToolMode(ToolMode::Pen);
                    }
                    SetForegroundWindow(hwnd);
                    break;
                case 10: // Snapshot (Click for Snip, Ctrl+Click for Full Snapshot)
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    g_backdropFlyoutOpen = false;
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        CaptureFullScreenSnapshot();
                    }
                    else {
                        StartSnipping();
                    }
                    break;
                case 11: // Undo
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    PerformUndo();
                    break;
                case 12: // Redo
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    PerformRedo();
                    break;
                case 13: // Clear
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    if (!g_strokes.empty() || !g_laserStrokes.empty()) {
                        if (!g_strokes.empty()) PushUndoState();
                        g_strokes.clear();
                        g_laserStrokes.clear();
                        if (!g_isLaserDrawing) KillTimer(hwnd, TIMER_ID_LASER);
                    }
                    break;
                case 98: // Minimize / Collapse Toolbar into indicator pill
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    g_toolbarCollapsed = true;
                    {
                        float oldCenterX = (g_toolbarRect.left + g_toolbarRect.right) * 0.5f;
                        g_toolbarCustomX = oldCenterX - g_toolbarPillWidth * 0.5f;
                    }
                    BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
                    SavePersistentToolbarState();
                    InvalidateOverlay();
                    return 0;
                case 99: // Exit
                    g_colorFlyoutOpen = false;
                    g_shapesFlyoutOpen = false;
                    g_gridFlyoutOpen = false;
                    HideOverlay();
                    return 0;
                }
            }
            InvalidateOverlay();
            return 0;
        }

        // Pointer mode: clicks pass through to desktop via WM_NCHITTEST
        if (g_currentTool == ToolMode::Pointer) {
            return 0;
        }

        // Pan Drag
        if (g_currentTool == ToolMode::Pan) {
            g_isPanning = true;
            g_panStartPos = { (LONG)x, (LONG)y };
            SetCapture(hwnd);
            return 0;
        }

        // Laser Tool Left-Click: start ephemeral trail drawing
        if (g_currentTool == ToolMode::Laser) {
            SetCapture(hwnd);
            g_isLaserDrawing = true;
            float adjX = (x - g_panOffsetX) / g_zoomScale;
            float adjY = (y - g_panOffsetY) / g_zoomScale;
            g_laserStrokes.emplace_back();
            g_laserStrokes.back().points.push_back({ adjX, adjY, GetTickCount64() });
            SetTimer(hwnd, TIMER_ID_LASER, 16, NULL);
            InvalidateOverlay();
            return 0;
        }

        // Eraser Tool Left-Click: start Brush Erase Drag
        if (g_currentTool == ToolMode::Eraser) {
            g_hasPushedUndoForCurrentErase = false;
            g_isLeftClickErasing = true;
            SetCapture(hwnd);
            EraseWholeShapeAt(x, y, g_eraserRadius);
        }
        else {
            // Start Drawing Stroke or Shape
            SetCapture(hwnd);
            g_isDrawing = true;
            float adjX = (x - g_panOffsetX) / g_zoomScale;
            float adjY = (y - g_panOffsetY) / g_zoomScale;

            g_currentStroke.points.clear();
            g_currentStroke.InvalidateCache();
            g_currentStroke.points.push_back({ adjX, adjY });
            g_currentStroke.startPt = { adjX, adjY };
            g_currentStroke.endPt = { adjX, adjY };
            g_currentStroke.shapeType = g_currentShape;
            g_currentStroke.color = g_activeColor;
            g_currentStroke.isHighlighter = (g_currentTool == ToolMode::Highlighter);
            g_currentStroke.width = g_currentStroke.isHighlighter ? g_settings.defaultHighlighterWidth : g_settings.defaultPenWidth;
        }

        InvalidateOverlay();
        return 0;
    }

    case WM_LBUTTONUP: {
        if (g_pickerDrag != ColorPickerDrag::None) {
            ReleaseCapture();
            g_pickerDrag = ColorPickerDrag::None;
            PushRecentColor(g_customColor.activeColor);
            SavePersistentCustomColor();
            BuildToolbarLayout(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
            InvalidateOverlay();
            return 0;
        }

        if (g_isSnipping) {
            if (g_isSnippingDrag) {
                ReleaseCapture();
                g_isSnippingDrag = false;

                int cropX = (int)std::min(g_snipStartPt.x, g_snipEndPt.x);
                int cropY = (int)std::min(g_snipStartPt.y, g_snipEndPt.y);
                int cropW = (int)std::abs(g_snipEndPt.x - g_snipStartPt.x);
                int cropH = (int)std::abs(g_snipEndPt.y - g_snipStartPt.y);

                if (cropW >= 8 && cropH >= 8) {
                    SaveCroppedSnapshot(cropX, cropY, cropW, cropH);
                } else {
                    InvalidateOverlay();
                }
            }
            return 0;
        }

        if (g_isLaserDrawing) {
            ReleaseCapture();
            g_isLaserDrawing = false;
            float adjX = ((float)GET_X_LPARAM(lParam) - g_panOffsetX) / g_zoomScale;
            float adjY = ((float)GET_Y_LPARAM(lParam) - g_panOffsetY) / g_zoomScale;
            if (!g_laserStrokes.empty() && !g_laserStrokes.back().points.empty()) {
                if (DistanceSq(adjX, adjY, g_laserStrokes.back().points.back().x, g_laserStrokes.back().points.back().y) >= 4.0f) {
                    g_laserStrokes.back().points.push_back({ adjX, adjY, GetTickCount64() });
                    g_laserStrokes.back().InvalidateGeometry();
                }
            }
            InvalidateOverlay();
            return 0;
        }

        if (g_isLeftClickErasing) {
            ReleaseCapture();
            g_isLeftClickErasing = false;
            g_hasPushedUndoForCurrentErase = false;
            InvalidateOverlay();
            return 0;
        }

        if (g_isPillMouseDown) {
            ReleaseCapture();
            float distFromInitSq = DistanceSq((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam), (float)g_toolbarDragStartInit.x, (float)g_toolbarDragStartInit.y);
            bool wasDragging = g_isPillDragging || (distFromInitSq > 16.0f);
            g_isPillMouseDown = false;
            g_isPillDragging = false;

            if (!wasDragging) {
                // Click on collapsed pill without dragging: Expand!
                g_toolbarCollapsed = false;
                float pillCenterX = (g_toolbarRect.left + g_toolbarRect.right) * 0.5f;
                int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                g_toolbarCustomX = pillCenterX - g_toolbarExpandedWidth * 0.5f;
                if (g_toolbarCustomX < 10.0f) g_toolbarCustomX = 10.0f;
                if (g_toolbarCustomX + g_toolbarExpandedWidth > (float)vw - 10.0f) g_toolbarCustomX = (float)vw - g_toolbarExpandedWidth - 10.0f;
                BuildToolbarLayout(vw, vh);
                SavePersistentToolbarState();
                ShowToastNotification(L"Toolbar Expanded");
                InvalidateOverlay();
            } else {
                SavePersistentToolbarState();
            }
            return 0;
        }

        if (g_isDraggingToolbar) {
            ReleaseCapture();
            g_isDraggingToolbar = false;
            SavePersistentToolbarState();
            return 0;
        }

        if (g_isPanning) {
            ReleaseCapture();
            g_isPanning = false;
            return 0;
        }

        if (g_isDrawing) {
            ReleaseCapture();
            g_isDrawing = false;
            if (g_currentStroke.shapeType == ShapeType::Freehand) {
                if (!g_currentStroke.points.empty()) {
                    PushUndoState();
                    g_currentStroke.ComputeBounds();
                    BuildStrokeGeometry(g_currentStroke);
                    g_strokes.push_back(g_currentStroke);
                }
            }
            else {
                if (DistanceSq(g_currentStroke.startPt.x, g_currentStroke.startPt.y, g_currentStroke.endPt.x, g_currentStroke.endPt.y) > 4.0f) {
                    PushUndoState();
                    g_currentStroke.ComputeBounds();
                    BuildStrokeGeometry(g_currentStroke);
                    g_strokes.push_back(g_currentStroke);
                }
            }
            g_currentStroke.points.clear();
            g_currentStroke.InvalidateCache();
            InvalidateOverlay();
        }
        g_hasPushedUndoForCurrentErase = false;
        return 0;
    }

    case WM_RBUTTONDOWN: {
        float x = (float)GET_X_LPARAM(lParam);
        float y = (float)GET_Y_LPARAM(lParam);

        if (g_isSnipping) {
            CancelSnipping();
            return 0;
        }

        if (g_radialActive) {
            g_radialActive = false;
            g_radialHoverTarget = RadialTarget::None;
            g_radialHoverSector = -1;
            g_hoveredOrb = -1;
            g_hoveredRecentOrb = -1;
            g_radialRecentFanOpen = false;
            InvalidateOverlay();
            return 0;
        }

        // Right-Click hold starts erase (brush in normal mode, whole-shape in Eraser mode), quick tap opens radial menu
        g_isRightMouseDown = true;
        g_isRightClickErasing = false;
        g_isRightClickClearing = false;
        g_hasPushedUndoForCurrentErase = false;
        g_wheelUsedWhileRightMouseDown = false;
        g_rightMouseDownPos.x = (LONG)x;
        g_rightMouseDownPos.y = (LONG)y;
        g_rightMouseDownTime = GetTickCount64();
        SetCapture(hwnd);
        InvalidateOverlay();
        return 0;
    }

    case WM_RBUTTONUP: {
        if (g_isRightMouseDown || g_isRightClickClearing) {
            ReleaseCapture();
            float upX = (float)GET_X_LPARAM(lParam);
            float upY = (float)GET_Y_LPARAM(lParam);
            float distMoved = std::sqrt(DistanceSq(upX, upY, (float)g_rightMouseDownPos.x, (float)g_rightMouseDownPos.y));
            ULONGLONG holdDuration = GetTickCount64() - g_rightMouseDownTime;
            bool wasHeldOrErasing = g_isRightClickErasing || g_isRightClickClearing || g_wheelUsedWhileRightMouseDown || (holdDuration > 180) || (distMoved > 4.0f);

            g_isRightMouseDown = false;
            g_isRightClickErasing = false;
            g_isRightClickClearing = false;
            g_hasPushedUndoForCurrentErase = false;
            g_wheelUsedWhileRightMouseDown = false;

            if (!wasHeldOrErasing) {
                // Quick right-click tap: Open Radial Menu!
                g_radialActive = true;
                g_radialX = upX;
                g_radialY = upY;
                g_radialDpiScale = GetDpiScaleAtPoint(g_radialX, g_radialY);
                if (g_radialDpiScale <= 0.1f) g_radialDpiScale = 1.0f;
                CreateRadialTextFormats(g_radialDpiScale);
                g_radialHoverTarget = RadialTarget::None;
                g_radialHoverSector = -1;
                g_hoveredOrb = -1;
                g_hoveredRecentOrb = -1;
                g_radialRecentFanOpen = false;
            }
            InvalidateOverlay();
        }
        return 0;
    }

    case WM_CAPTURECHANGED: {
        g_isPillMouseDown = false;
        g_isPillDragging = false;
        g_isDraggingToolbar = false;
        g_isPanning = false;
        g_isDrawing = false;
        g_isLaserDrawing = false;
        g_isLeftClickErasing = false;
        g_isRightMouseDown = false;
        g_isRightClickErasing = false;
        g_isRightClickClearing = false;
        g_isSnippingDrag = false;
        g_pickerDrag = ColorPickerDrag::None;
        return 0;
    }

    case WM_NCHITTEST: {
        if (g_currentTool == ToolMode::Pointer) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            if (g_isPillMouseDown || g_isPillDragging || g_isDraggingToolbar) {
                return HTCLIENT;
            }
            if (g_shapesFlyoutOpen &&
                pt.x >= g_shapesFlyoutRect.left && pt.x <= g_shapesFlyoutRect.right &&
                pt.y >= g_shapesFlyoutRect.top && pt.y <= g_shapesFlyoutRect.bottom) {
                return HTCLIENT;
            }
            if (g_gridFlyoutOpen &&
                pt.x >= g_gridFlyoutRect.left && pt.x <= g_gridFlyoutRect.right &&
                pt.y >= g_gridFlyoutRect.top && pt.y <= g_gridFlyoutRect.bottom) {
                return HTCLIENT;
            }
            if (g_backdropFlyoutOpen &&
                pt.x >= g_backdropFlyoutRect.left && pt.x <= g_backdropFlyoutRect.right &&
                pt.y >= g_backdropFlyoutRect.top && pt.y <= g_backdropFlyoutRect.bottom) {
                return HTCLIENT;
            }
            if (g_colorFlyoutOpen &&
                pt.x >= g_colorFlyoutRect.left && pt.x <= g_colorFlyoutRect.right &&
                pt.y >= g_colorFlyoutRect.top && pt.y <= g_colorFlyoutRect.bottom) {
                return HTCLIENT;
            }
            if (g_settings.showBottomToolbar &&
                pt.x >= (g_toolbarRect.left - 6.0f) && pt.x <= (g_toolbarRect.right + 6.0f) &&
                pt.y >= (g_toolbarRect.top - 6.0f) && pt.y <= (g_toolbarRect.bottom + 6.0f)) {
                return HTCLIENT;
            }
            return HTTRANSPARENT;
        }
        return HTCLIENT;
    }

    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT) {
            if (g_radialActive) {
                SetCursor(NULL);
                return TRUE;
            }
            if (g_isEyedropperActive) {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                return TRUE;
            }
            if (g_isSnipping) {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                return TRUE;
            }
            if (g_currentTool == ToolMode::Pan) {
                SetCursor(LoadCursor(NULL, IDC_SIZEALL));
                return TRUE;
            }
            if (g_currentTool == ToolMode::Eraser || g_isRightClickErasing || g_isRightMouseDown || g_isLeftClickErasing || g_isRightClickClearing) {
                SetCursor(NULL);
                return TRUE;
            }
            if (g_currentTool == ToolMode::Pointer) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            if (g_shapesFlyoutOpen &&
                pt.x >= g_shapesFlyoutRect.left && pt.x <= g_shapesFlyoutRect.right &&
                pt.y >= g_shapesFlyoutRect.top && pt.y <= g_shapesFlyoutRect.bottom) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            if (g_gridFlyoutOpen &&
                pt.x >= g_gridFlyoutRect.left && pt.x <= g_gridFlyoutRect.right &&
                pt.y >= g_gridFlyoutRect.top && pt.y <= g_gridFlyoutRect.bottom) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            if (g_backdropFlyoutOpen &&
                pt.x >= g_backdropFlyoutRect.left && pt.x <= g_backdropFlyoutRect.right &&
                pt.y >= g_backdropFlyoutRect.top && pt.y <= g_backdropFlyoutRect.bottom) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            if (g_colorFlyoutOpen &&
                pt.x >= g_colorFlyoutRect.left && pt.x <= g_colorFlyoutRect.right &&
                pt.y >= g_colorFlyoutRect.top && pt.y <= g_colorFlyoutRect.bottom) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            if (g_settings.showBottomToolbar &&
                pt.x >= g_toolbarRect.left && pt.x <= g_toolbarRect.right &&
                pt.y >= g_toolbarRect.top && pt.y <= g_toolbarRect.bottom) {
                SetCursor(LoadCursor(NULL, g_toolbarCollapsed ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            if (g_currentTool == ToolMode::Laser) {
                SetCursor(NULL);
                return TRUE;
            }
            if (g_currentTool == ToolMode::Pen || g_currentTool == ToolMode::Highlighter) {
                SetCursor(NULL);
                return TRUE;
            }
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            return TRUE;
        }
        break;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        ReleaseD2DResources();
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ----------------------------------------------------------------------------
// Overlay Show / Hide Management
// ----------------------------------------------------------------------------

void ShowOverlay() {
    if (g_bIsActive) return;

    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (!g_hOverlayWnd) {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
        wc.lpfnWndProc = OverlayWndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = L"WindhawkNativeScreenInkOverlay";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassExW(&wc);

        g_hOverlayWnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
            wc.lpszClassName,
            L"Screen Inking Overlay",
            WS_POPUP,
            vx, vy, vw, vh,
            NULL, NULL, wc.hInstance, NULL
        );
        SetLayeredWindowAttributes(g_hOverlayWnd, 0, 255, LWA_ALPHA);
        CreateD2DResources(g_hOverlayWnd);
    }
    else {
        SetWindowPos(g_hOverlayWnd, HWND_TOPMOST, vx, vy, vw, vh, SWP_NOACTIVATE);
        CreateD2DResources(g_hOverlayWnd);
        CaptureDesktop();
    }

    BuildToolbarLayout(vw, vh);

    g_radialActive = false;
    g_radialHoverTarget = RadialTarget::None;
    g_radialHoverSector = -1;
    g_hoveredOrb = -1;
    g_isPillMouseDown = false;
    g_isPillDragging = false;
    g_isDraggingToolbar = false;
    g_isDrawing = false;
    g_isPanning = false;
    g_isRightMouseDown = false;
    g_isRightClickErasing = false;
    g_isLeftClickErasing = false;
    g_isRightClickClearing = false;
    g_wheelUsedWhileRightMouseDown = false;
    g_hoveredToolbarBtn = -1;
    g_shapesFlyoutOpen = false;
    g_hoveredShapeFlyoutItem = -1;
    g_gridFlyoutOpen = false;
    g_hoveredGridFlyoutItem = -1;
    g_backdropFlyoutOpen = false;
    g_hoveredBackdropFlyoutItem = -1;
    g_colorFlyoutOpen = false;
    g_isEyedropperActive = false;
    g_pickerDrag = ColorPickerDrag::None;
    g_radialRecentFanOpen = false;
    g_hoveredRecentOrb = -1;
    g_hoveredRecentSwatch = -1;
    g_hoveredColorStudioAction = -1;
    g_lastOverlayOpenTime = GetTickCount64();

    switch (g_settings.defaultStartupTool) {
        case 2: SetToolMode(ToolMode::Highlighter); break;
        case 3: SetToolMode(ToolMode::Laser); break;
        case 4: SetToolMode(ToolMode::Pointer); break;
        case 1:
        default: SetToolMode(ToolMode::Pen); break;
    }

    // Pre-render the fresh frame while hidden so DWM never composites a stale backbuffer
    RenderOverlay();

    ShowWindow(g_hOverlayWnd, SW_SHOW);
    SetForegroundWindow(g_hOverlayWnd);
    SetFocus(g_hOverlayWnd);
    g_bIsActive = true;

    InvalidateOverlay();
}

void HideOverlay() {
    if (!g_bIsActive) return;
    CancelSnipping();
    g_bIsActive = false;
    g_shapesFlyoutOpen = false;
    g_hoveredShapeFlyoutItem = -1;
    g_gridFlyoutOpen = false;
    g_hoveredGridFlyoutItem = -1;
    g_backdropFlyoutOpen = false;
    g_hoveredBackdropFlyoutItem = -1;
    g_colorFlyoutOpen = false;
    g_isEyedropperActive = false;
    g_pickerDrag = ColorPickerDrag::None;
    g_radialRecentFanOpen = false;
    g_hoveredRecentOrb = -1;
    g_hoveredRecentSwatch = -1;
    g_hoveredColorStudioAction = -1;
    g_radialActive = false;
    g_isPillMouseDown = false;
    g_isPillDragging = false;
    g_isDraggingToolbar = false;
    g_isDrawing = false;
    g_isPanning = false;
    g_isRightMouseDown = false;
    g_isRightClickErasing = false;
    g_isLeftClickErasing = false;
    g_isRightClickClearing = false;
    g_wheelUsedWhileRightMouseDown = false;

    if (g_hOverlayWnd) {
        if (GetCapture() == g_hOverlayWnd) {
            ReleaseCapture();
        }
        KillTimer(g_hOverlayWnd, TIMER_ID_UI_ANIMATION);
        KillTimer(g_hOverlayWnd, TIMER_ID_POINTER_WATCH);
        KillTimer(g_hOverlayWnd, TIMER_ID_LASER);
        LONG_PTR exStyle = GetWindowLongPtr(g_hOverlayWnd, GWL_EXSTYLE);
        if (exStyle & WS_EX_TRANSPARENT) {
            SetWindowLongPtr(g_hOverlayWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
            SetWindowPos(g_hOverlayWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        // Wipe D2D surface clean on hide to guarantee no ghosting/flashing on next activation
        if (g_pRenderTarget) {
            g_pRenderTarget->BeginDraw();
            g_pRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
            g_pRenderTarget->EndDraw();
        }
        ShowWindow(g_hOverlayWnd, SW_HIDE);
    }
    g_zoomPreviewTime = 0;
    g_sizePreviewTime = 0;
    g_laserStrokes.clear();
    g_isLaserDrawing = false;
    g_currentTool = ToolMode::Pen;
    g_canvasBg = CanvasBg::Transparent;
}

// ----------------------------------------------------------------------------
// System Tray Notification Icon (PenWorkspace \uEDC6)
// ----------------------------------------------------------------------------

HICON CreateGlyphIcon(WCHAR glyph, int size) {
    if (!g_pWICFactory || !g_pD2DFactory || !g_pDWriteFactory || size <= 0) return NULL;

    IWICBitmap* pWicBitmap = nullptr;
    HRESULT hr = g_pWICFactory->CreateBitmap(size, size, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &pWicBitmap);
    if (FAILED(hr) || !pWicBitmap) return NULL;

    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f
    );

    ID2D1RenderTarget* pRT = nullptr;
    hr = g_pD2DFactory->CreateWicBitmapRenderTarget(pWicBitmap, rtProps, &pRT);
    if (FAILED(hr) || !pRT) {
        pWicBitmap->Release();
        return NULL;
    }

    pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    const wchar_t* fontName = GetIconFontFamilyName();
    float fontSize = (float)size * 0.90f;
    IDWriteTextFormat* pFormat = nullptr;
    g_pDWriteFactory->CreateTextFormat(
        fontName,
        NULL,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-us",
        &pFormat
    );

    if (pFormat) {
        pFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        pFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    ID2D1SolidColorBrush* pBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);

    pRT->BeginDraw();
    pRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

    if (pFormat && pBrush) {
        WCHAR str[2] = { glyph, 0 };
        pRT->DrawTextW(str, 1, pFormat, D2D1::RectF(0, 0, (float)size, (float)size), pBrush);
    }

    hr = pRT->EndDraw();

    if (pBrush) pBrush->Release();
    if (pFormat) pFormat->Release();
    pRT->Release();

    HICON hIcon = NULL;
    IWICBitmapLock* pLock = nullptr;
    WICRect rc = { 0, 0, size, size };
    if (SUCCEEDED(pWicBitmap->Lock(&rc, WICBitmapLockRead, &pLock))) {
        UINT bufferSize = 0;
        BYTE* pBytes = nullptr;
        if (SUCCEEDED(pLock->GetDataPointer(&bufferSize, &pBytes)) && pBytes) {
            BITMAPV5HEADER bi = { sizeof(BITMAPV5HEADER) };
            bi.bV5Width = size;
            bi.bV5Height = -size; // top-down
            bi.bV5Planes = 1;
            bi.bV5BitCount = 32;
            bi.bV5Compression = BI_BITFIELDS;
            bi.bV5RedMask   = 0x00FF0000;
            bi.bV5GreenMask = 0x0000FF00;
            bi.bV5BlueMask  = 0x000000FF;
            bi.bV5AlphaMask = 0xFF000000;

            HDC hdcScreen = GetDC(NULL);
            void* pDIBBits = nullptr;
            HBITMAP hColorBitmap = CreateDIBSection(hdcScreen, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pDIBBits, NULL, 0);
            ReleaseDC(NULL, hdcScreen);

            UINT stride = 0;
            pLock->GetStride(&stride);

            if (hColorBitmap && pDIBBits) {
                BYTE* pDst = (BYTE*)pDIBBits;
                BYTE* pSrc = pBytes;
                UINT rowBytes = (UINT)size * 4;
                for (int row = 0; row < size; ++row) {
                    memcpy(pDst, pSrc, rowBytes);
                    pDst += rowBytes;
                    pSrc += stride;
                }

                HBITMAP hMonoMask = CreateBitmap(size, size, 1, 1, NULL);
                if (hMonoMask) {
                    ICONINFO ii = { 0 };
                    ii.fIcon = TRUE;
                    ii.xHotspot = 0;
                    ii.yHotspot = 0;
                    ii.hbmMask = hMonoMask;
                    ii.hbmColor = hColorBitmap;

                    hIcon = CreateIconIndirect(&ii);
                    DeleteObject(hMonoMask);
                }
                DeleteObject(hColorBitmap);
            }
        }
        pLock->Release();
    }

    pWicBitmap->Release();
    return hIcon;
}

static bool g_hotkeyRegistered = false;

std::wstring GetConfiguredHotkeyString() {
    std::wstring s;
    if (g_settings.hotkeyMod & MOD_CONTROL) {
        if (!s.empty()) s += L"+";
        s += L"Ctrl";
    }
    if (g_settings.hotkeyMod & MOD_ALT) {
        if (!s.empty()) s += L"+";
        s += L"Alt";
    }
    if (g_settings.hotkeyMod & MOD_SHIFT) {
        if (!s.empty()) s += L"+";
        s += L"Shift";
    }
    if (g_settings.hotkeyMod & MOD_WIN) {
        if (!s.empty()) s += L"+";
        s += L"Win";
    }

    std::wstring keyPart;
    if (g_settings.hotkeyKey >= 'A' && g_settings.hotkeyKey <= 'Z') {
        keyPart += (wchar_t)g_settings.hotkeyKey;
    } else if (g_settings.hotkeyKey >= '0' && g_settings.hotkeyKey <= '9') {
        keyPart += (wchar_t)g_settings.hotkeyKey;
    } else if (g_settings.hotkeyKey >= VK_F1 && g_settings.hotkeyKey <= VK_F24) {
        keyPart = L"F" + std::to_wstring(g_settings.hotkeyKey - VK_F1 + 1);
    } else {
        UINT scanCode = MapVirtualKeyW(g_settings.hotkeyKey, MAPVK_VK_TO_VSC);
        if (scanCode != 0) {
            wchar_t keyName[32] = { 0 };
            LONG lP = (scanCode << 16);
            if (GetKeyNameTextW(lP, keyName, ARRAYSIZE(keyName)) > 0) {
                keyPart = keyName;
            }
        }
        if (keyPart.empty()) {
            keyPart = L"Key";
        }
    }

    if (!s.empty()) {
        s += L"+" + keyPart;
    } else {
        s = keyPart;
    }
    return s;
}

void RemoveTrayIcon() {
    if (g_bTrayIconVisible) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        g_bTrayIconVisible = false;
    }
    if (g_hTrayIcon) {
        DestroyIcon(g_hTrayIcon);
        g_hTrayIcon = NULL;
    }
}

void UpdateTrayIcon(HWND hwnd) {
    if (!g_settings.showTrayIcon) {
        RemoveTrayIcon();
        return;
    }

    if (!hwnd) return;

    if (!g_hTrayIcon) {
        int iconSize = GetSystemMetrics(SM_CXSMICON);
        if (iconSize <= 0) iconSize = 16;
        g_hTrayIcon = CreateGlyphIcon(0xEDC6, iconSize);
    }

    ZeroMemory(&g_nid, sizeof(g_nid));
    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = hwnd;
    g_nid.uID = kTrayIconId;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_nid.uCallbackMessage = WM_USER_TRAYICON;
    g_nid.hIcon = g_hTrayIcon;

    std::wstring hotkeyStr = GetConfiguredHotkeyString();
    if (!g_hotkeyRegistered) {
        swprintf_s(g_nid.szTip, ARRAYSIZE(g_nid.szTip), L"WinDraw - Screen Inking & Annotation (Conflict: %s)", hotkeyStr.c_str());
    } else {
        swprintf_s(g_nid.szTip, ARRAYSIZE(g_nid.szTip), L"WinDraw - Screen Inking & Annotation (%s)", hotkeyStr.c_str());
    }

    if (!g_bTrayIconVisible) {
        if (Shell_NotifyIconW(NIM_ADD, &g_nid)) {
            g_bTrayIconVisible = true;
        }
    }
    else {
        Shell_NotifyIconW(NIM_MODIFY, &g_nid);
    }
}

// ----------------------------------------------------------------------------
// Hotkey & Tray Background Message Loop
// ----------------------------------------------------------------------------

static const int kHotkeyId = 1042;

LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_wmTaskbarCreated && msg == g_wmTaskbarCreated) {
        g_bTrayIconVisible = false;
        UpdateTrayIcon(hwnd);
        return 0;
    }

    if (msg == WM_USER_UPDATE_TRAY) {
        UpdateTrayIcon(hwnd);
        return 0;
    }

    if (msg == WM_USER_TRAYICON) {
        UINT uMsg = LOWORD(lParam);
        static ULONGLONG s_lastTrayClickTime = 0;

        if (uMsg == WM_LBUTTONUP || uMsg == WM_LBUTTONDBLCLK) {
            ULONGLONG now = GetTickCount64();
            if (now - s_lastTrayClickTime < 250) return 0; // Debounce duplicate events
            s_lastTrayClickTime = now;

            if (g_bIsActive) {
                HideOverlay();
            }
            else {
                ShowOverlay();
            }
            return 0;
        }
        else if (uMsg == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            if (hMenu) {
                std::wstring hotkeyStr = GetConfiguredHotkeyString();
                std::wstring openLabel;
                if (g_bIsActive) {
                    openLabel = L"Hide WinDraw\tEsc";
                } else if (!g_hotkeyRegistered) {
                    openLabel = L"Open WinDraw (Conflict)\t" + hotkeyStr;
                } else {
                    openLabel = L"Open WinDraw\t" + hotkeyStr;
                }

                // Header & Primary Toggle
                AppendMenuW(hMenu, MF_STRING, 1, openLabel.c_str());
                SetMenuDefaultItem(hMenu, 1, FALSE);
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

                // Quick Tools
                AppendMenuW(hMenu, MF_STRING, 5, L"Region Snip\tS");
                AppendMenuW(hMenu, MF_STRING, 2, L"Full Snapshot\tCtrl+S");
                AppendMenuW(hMenu, MF_STRING, 6, L"Whiteboard Mode\tK");

                UINT clearFlags = (g_bIsActive && (!g_strokes.empty() || !g_laserStrokes.empty())) ? MF_STRING : (MF_STRING | MF_GRAYED | MF_DISABLED);
                AppendMenuW(hMenu, clearFlags, 3, L"Clear Canvas\tC");

                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING, 7, L"Reset Toolbar Position\tCtrl+Shift+B");

                if (g_bIsActive) {
                    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                    AppendMenuW(hMenu, MF_STRING, 4, L"Hide Overlay");
                }

                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                PostMessageW(hwnd, WM_NULL, 0, 0);
                DestroyMenu(hMenu);

                if (cmd == 1 || cmd == 4) {
                    if (g_bIsActive) HideOverlay();
                    else ShowOverlay();
                }
                else if (cmd == 2) {
                    CaptureFullScreenSnapshot();
                }
                else if (cmd == 5) {
                    StartSnipping();
                }
                else if (cmd == 6) {
                    if (!g_bIsActive) ShowOverlay();
                    CycleCanvasBackground();
                }
                else if (cmd == 3) {
                    if (g_bIsActive && (!g_strokes.empty() || !g_laserStrokes.empty())) {
                        if (!g_strokes.empty()) PushUndoState();
                        g_strokes.clear();
                        g_laserStrokes.clear();
                        if (!g_isLaserDrawing) KillTimer(hwnd, TIMER_ID_LASER);
                        InvalidateOverlay();
                    }
                }
                else if (cmd == 7) {
                    g_toolbarCustomX = -1.0f;
                    g_toolbarCustomY = -1.0f;
                    SavePersistentToolbarState();
                    if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
                        int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                        int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                        BuildToolbarLayout(vw, vh);
                        InvalidateOverlay();
                    }
                    ShowToastNotification(L"Toolbar Position Reset to Center");
                }
            }
            return 0;
        }
        return 0;
    }

    if (msg == WM_HOTKEY && wParam == kHotkeyId) {
        if (g_bIsActive) {
            // Toggling hotkey while active switches between click-through Mouse Pointer and Inking
            if (g_hOverlayWnd) {
                PostMessageW(g_hOverlayWnd, WM_USER_TOGGLE_POINTER, 0, 0);
            }
        }
        else {
            ShowOverlay();
        }
        return 0;
    }

    if (msg == WM_APP_SETTINGS_CHANGED) {
        LoadSettings();
        g_currentPenWidth = (g_currentTool == ToolMode::Highlighter) ? g_settings.defaultHighlighterWidth : g_settings.defaultPenWidth;
        UnregisterHotKey(hwnd, kHotkeyId);
        g_hotkeyRegistered = (RegisterHotKey(hwnd, kHotkeyId, g_settings.hotkeyMod | MOD_NOREPEAT, g_settings.hotkeyKey) != FALSE);
        UpdateTrayIcon(hwnd);
        if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
            int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
            BuildToolbarLayout(vw, vh);
            RebuildGridBrush();
            InvalidateOverlay();
        }
        return 0;
    }

    if (msg == WM_APP_EXIT) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI HotkeyThread(LPVOID) {
    // Force message queue creation immediately so PostThreadMessage won't fail with ERROR_INVALID_THREAD_ID
    MSG initMsg;
    PeekMessageW(&initMsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    // Declare Per-Monitor DPI awareness V2
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        using SetThreadDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
        auto pSetThreadDpiAwarenessContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
        if (pSetThreadDpiAwarenessContext) {
            pSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }

    CoInitialize(NULL);

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to create Direct2D Factory (0x%08X)", hr);
        CoUninitialize();
        return 1;
    }

    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&g_pDWriteFactory)
    );

    CreateTextFormats(1.0f);

    CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&g_pWICFactory)
    );

    g_wmTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = HotkeyWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = L"WindhawkNativeInkHotkeyReceiver";
    RegisterClassExW(&wc);

    g_hHotkeyWnd = CreateWindowExW(
        0, wc.lpszClassName, L"WinDrawTrayReceiver",
        WS_POPUP, 0, 0, 0, 0,
        NULL, NULL, wc.hInstance, NULL
    );

    g_hotkeyRegistered = (RegisterHotKey(g_hHotkeyWnd, kHotkeyId, g_settings.hotkeyMod | MOD_NOREPEAT, g_settings.hotkeyKey) != FALSE);
    if (!g_hotkeyRegistered) {
        Wh_Log(L"Warning - RegisterHotKey failed (error %lu)", GetLastError());
    } else {
        Wh_Log(L"Hotkey registered successfully");
    }

    UpdateTrayIcon(g_hHotkeyWnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == WM_APP_SETTINGS_CHANGED) {
            if (g_hHotkeyWnd) {
                SendMessageW(g_hHotkeyWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    HideOverlay();
    RemoveTrayIcon();

    if (g_hOverlayWnd) {
        DestroyWindow(g_hOverlayWnd);
        g_hOverlayWnd = NULL;
    }
    if (g_hHotkeyWnd) {
        UnregisterHotKey(g_hHotkeyWnd, kHotkeyId);
        DestroyWindow(g_hHotkeyWnd);
        g_hHotkeyWnd = NULL;
    }

    UnregisterClassW(L"WindhawkNativeScreenInkOverlay", GetCurrentModuleHandle());
    UnregisterClassW(L"WindhawkNativeInkHotkeyReceiver", GetCurrentModuleHandle());

    // Clean up all strokes and cached geometries before releasing D2D factory
    g_strokes.clear();
    g_laserStrokes.clear();
    g_undoStack.clear();
    g_redoStack.clear();
    g_currentStroke.InvalidateCache();

    ReleaseD2DResources();

    if (g_pWICFactory) { g_pWICFactory->Release(); g_pWICFactory = nullptr; }
    ReleaseTextFormats();
    if (g_pDWriteFactory) { g_pDWriteFactory->Release(); g_pDWriteFactory = nullptr; }
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }

    CoUninitialize();
    return 0;
}

// ----------------------------------------------------------------------------
// Windhawk Tool Mod Lifecycle
// ----------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing");

    LoadSettings();
    Wh_Log(L"Settings loaded");
    LoadPersistentState();
    Wh_Log(L"Persistent state loaded");
    g_currentPenWidth = (g_currentTool == ToolMode::Highlighter) ? g_settings.defaultHighlighterWidth : g_settings.defaultPenWidth;

    g_hHotkeyThread = CreateThread(NULL, 0, HotkeyThread, NULL, 0, &g_hotkeyThreadId);
    if (!g_hHotkeyThread) {
        Wh_Log(L"Failed to create HotkeyThread: %lu", GetLastError());
        return FALSE;
    }

    Wh_Log(L"Ready (Dedicated tool process active)");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings Changed");
    if (g_hHotkeyWnd) {
        PostMessageW(g_hHotkeyWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else if (g_hotkeyThreadId != 0) {
        PostThreadMessageW(g_hotkeyThreadId, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading");

    if (g_hHotkeyWnd) {
        PostMessageW(g_hHotkeyWnd, WM_CANCELMODE, 0, 0);
        PostMessageW(g_hHotkeyWnd, WM_APP_EXIT, 0, 0);
    } else if (g_hotkeyThreadId != 0) {
        PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0);
    }

    if (g_hHotkeyThread) {
        WaitForSingleObject(g_hHotkeyThread, 3000);
        CloseHandle(g_hHotkeyThread);
        g_hHotkeyThread = NULL;
        g_hotkeyThreadId = 0;
    }
}

// clang-format off
// ============================================================================
// Tool Mod Boilerplate
// ============================================================================
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
// clang-format on
