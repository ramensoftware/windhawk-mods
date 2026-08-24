// ==WindhawkMod==
// @id              magic-mouse
// @name            Magic Mouse
// @description     Draw custom mouse gestures to trigger actions like launching apps, toggling desktop icons, fullscreen, and more. Record gestures via an on-screen canvas, then replay them with a configurable modifier key.
// @version         1.0.1
// @author          iMAboud
// @github          https://github.com/iMAboud
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -lole32 -loleaut32 -luuid -lcomctl32 -luxtheme -lgdiplus -ldxva2
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Magic Mouse

![Drawing Gesture](https://i.imgur.com/SdiMibo.gif)

Draw custom shapes with your mouse to trigger powerful actions like launching apps, toggling desktop icons, or controlling media.

## How To Add a Gesture

![Gesture Window](https://i.imgur.com/Wee44Lo.png)

1. **Record the shape**: Press the **Record Canvas Hotkey** (default: `Ctrl+Shift+G`) anywhere on your screen. The drawing canvas will appear.
2. **Copy the code**: Draw your desired shape on the canvas, then click the **Copy to Clipboard** button. This copies the generated gesture code.
3. **Create the item**: Open the Windhawk settings for this mod, scroll down to the **Gestures** list, and click the `+` button to add a new gesture.
4. **Paste the code**: Paste the code you copied into the **Gesture Code (Direction Sequence)** box.
5. **Assign an action**: Choose what happens when you draw this shape using the **Action** dropdown.
6. **Set the Parameter (if needed)**: Some actions require extra information in the **Action Parameter** box:
   - For `Launch Application / URL`, type the path (e.g. `C:\Windows\notepad.exe`) or website (e.g. `https://google.com`).
   - For `Run Command Line`, type the command (e.g. `shutdown /s /t 0`).
   - *Note: Simple actions like `Play / Pause Media` or `Toggle Desktop Icons` don't require any parameter.*

## Activation Modes

Magic Mouse has a robust activation system. You can mix and match modifier keys (or mouse buttons) and physical "mouse wiggles" to arm Gesture Mode.

### 1. Modifier Keys
Set the `Modifier Key / Button` to your preferred trigger (e.g. `Right Click`, `Mouse 4`, `Ctrl+Shift`). 
- **Hold**: You must physically hold the modifier down to arm Gesture Mode.
- **Toggle**: Pressing the modifier acts like a light switch. Press it once to turn Gesture Mode ON (the Aura will appear around your cursor). Draw your gesture, or let it timeout.

### 2. Wiggle to Activate
You can trigger Gesture Mode simply by shaking (wiggling) your mouse rapidly left-and-right!
- **Always**: Wiggling the mouse *always* arms Gesture Mode, completely ignoring any modifier keys.
- **Only with Modifier**: You must be holding the Modifier Key (or have it toggled ON) *while* you wiggle the mouse. 
- **Disabled**: Wiggle is off. Gestures trigger solely based on your Modifier Keys.

> **Note**: When Gesture Mode is armed, a glowing Aura will appear on your cursor. If you don't draw a gesture within the `Armed Timeout` (default: 3 seconds), it will automatically turn off.



## Supported Actions

| Action | Parameter? | Description & Example Usage |
| :--- | :---: | :--- |
| **Launch App / URL** | Yes | Launches a file, program, folder, or website. (e.g. `notepad.exe`, `D:\MyFolder`, or `https://google.com`) |
| **Run Command Line** | Yes | Executes a CMD/PowerShell command silently. (e.g. `shutdown /s /t 0`) |
| **Send Keyboard Shortcut** | Yes | Simulates keystrokes. Use standard keys separated by `+`. (e.g. `ctrl+shift+esc` or `win+d`) |
| **Admin Terminal Here** | Optional | Opens PowerShell as Administrator in the current Explorer folder. Parameter: Optional command to execute. |
| **Take Screenshot** | Optional | Captures the monitor under your cursor. Copies to clipboard and saves to file. Parameter: custom save folder (defaults to `Pictures`). |
| **Window Controls** | No | `Maximize`, `Minimize`, `Close`, `Fullscreen`, `Toggle Always-on-Top` for the active window. |
| **Window Snapping** | No | Snaps the active window `Left`, `Right`, `Top`, `Bottom`, or to any of the 4 corners. |
| **Media Controls** | No | `Play / Pause`, `Next Track`, `Prev Track`, `Mute / Unmute Volume`. |
| **System Controls** | No | `Lock PC`, `Sleep Monitor`, `Sleep PC`, `Restart`, `Shut Down`, `Empty Recycle Bin`, `Show Desktop`, `Task Manager`, `Settings`. |
| **Utilities** | No | `Stopwatch`, `Timer`, `Flash (Flashlight)`, `Next Virtual Desktop`. |
| **Explorer Navigation** | No | `Explorer Back` and `Explorer Forward` to navigate folders. |
| **Special Tools** | No | `Create Sticky Note`, `Search Selection`, `Trigger Spotlight`, `Toggle Draw Mode`, `Color Picker`. |


## Special Features

* **Create Sticky Note**: Trigger the action, and your cursor will become a crosshair. Click and drag to draw a sticky note anywhere on your screen. 

  ![Sticky Note Feature](https://i.imgur.com/dTBI6ws.gif)

  - If you draw the note over an active application window, it will automatically start in "Always on Top" mode. If drawn over the empty desktop, it behaves as a normal window.
  - Right-click the note to change its color, opacity, delete it, or manually toggle "Always on Top".
  - The note will remember your last selected color and opacity for any future notes you create!
* **Search Selection**: Highlight text anywhere and trigger this gesture to instantly search it in your default browser.

  ![Search Selection](https://i.imgur.com/BKSHJyh.gif)

* **Spotlight**: Darkens the rest of the screen and highlights the area around your cursor, perfect for presentations or focusing.

  ![Spotlight Feature](https://i.imgur.com/aFt2jvq.gif)

* **Toggle Draw Mode & Color Picker**: You can assign actions to draw freely on your screen or pick a color. To exit these modes, simply press the `ESC` key. (You can also press `ESC` to close the Gesture Recording Canvas).
* **Take Screenshot**: Takes a full-quality screenshot of the specific monitor your cursor is currently hovering over. The image is instantly copied to your clipboard. It is also saved as a PNG file. By default, it saves to your user `Pictures` folder. You can customize this by entering a specific folder path (e.g., `C:\Screenshots`) into the Action Parameter field!

## Additional Settings

* **Trail Color (Hex)**: The HEX color of the gesture trail (e.g., `00AAFF` for Cyan, `FF0055` for Pink).
* **Trail Width**: The thickness of the gesture trail in pixels.

* **Show Aura**: Displays a glowing ring around your cursor to indicate when Gesture Mode is fully armed and ready to draw.
* **Armed Timeout (ms)**: How long Gesture Mode stays active after being armed by a Wiggle or Toggle (in milliseconds). Set to `0` to disable the timeout.
* **Wiggle Strength (10-200)**: How far you must physically move the mouse to register a single wiggle stroke. Lower numbers mean you don't have to shake as far.
* **Allow in Fullscreen Apps**: If disabled, the mod will automatically ignore gestures while playing fullscreen games or using fullscreen apps to prevent accidental triggers.

## Example Setups

**Setup 1: Right-Click Wiggle**
- `Modifier Key`: Right Click
- `Modifier Behavior`: Hold
- `Draw Button`: Left Click
- `Wiggle to Activate`: Only with Modifier
*How it works*: Hold Right-Click and quickly shake the mouse left-and-right. The aura appears. Now Left-Click and draw your gesture!

**Setup 2: Quick Side-Button Draw**
- `Modifier Key`: Mouse 4
- `Modifier Behavior`: Hold
- `Draw Button`: Left Click
- `Wiggle to Activate`: Disabled
*How it works*: Simply hold Mouse 4 on the side of your mouse, and Left-Click to draw. 

**Setup 3: Pure Wiggle**
- `Modifier Key`: None
- `Wiggle to Activate`: Always
*How it works*: Rapidly shake your mouse. The aura appears. Draw your gesture.

![Wiggle](https://i.imgur.com/PBmCK6v.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ModifierKey: none
  $name: Modifier Key / Button
  $description: "Use gestures when holding or toggling this modifier. If none, gestures will always be enabled."
  $options:
  - none: None (No modifier)
  - ctrl: Ctrl
  - shift: Shift
  - alt: Alt
  - ctrl+shift: Ctrl + Shift
  - ctrl+alt: Ctrl + Alt
  - left: Left Click
  - right: Right Click
  - middle: Middle Click
  - mouse4: Mouse 4
  - mouse5: Mouse 5
  - mouse4+left: Mouse 4 + Left Click
  - mouse5+left: Mouse 5 + Left Click
  - mouse4+right: Mouse 4 + Right Click
  - mouse5+right: Mouse 5 + Right Click
  - ctrl+left: Ctrl + Left Click
  - shift+left: Shift + Left Click
  - alt+left: Alt + Left Click
  - ctrl+right: Ctrl + Right Click
  - shift+right: Shift + Right Click
- ModifierBehavior: hold
  $name: Modifier Behavior
  $options:
  - hold: Hold (You must hold the modifier)
  - toggle: Toggle (Press to turn Gesture Mode On/Off)
- DrawButton: mouse4
  $name: Draw Button
  $description: The mouse button you physically hold to draw.
  $options:
  - right: Right Click
  - middle: Middle Click
  - left: Left Click
  - mouse4: Mouse 4 (Back Button)
  - mouse5: Mouse 5 (Forward Button)
- EnableWiggleToActivate: never
  $name: Wiggle to Activate
  $options:
  - never: Disabled
  - always: Always (Wiggle alone activates Gesture Mode)
  - modifier: Only with Modifier (Must have modifier active AND wiggle)
- WiggleStrength: 10
  $name: Wiggle Strength (10-200)
  $description: How far you must move the mouse to register a wiggle stroke.

- TrailColor: 00AAFF
  $name: Trail Color (Hex)
  $description: "6-digit hex code without #. Example: 00AAFF, FF0055"
- TrailWidth: 15
  $name: Trail Width

- ShowAura: true
  $name: Show Aura
  $description: Displays a glowing aura around your cursor when Gesture Mode is armed.

- ArmTimeout: 3000
  $name: Armed Timeout (ms)
  $description: How long Gesture Mode stays active after being armed by a Wiggle or Toggle. Set to 0 to disable timeout.

- AllowInFullscreen: false
  $name: Allow in Fullscreen
  $description: Master switch to enable gestures in fullscreen apps. If OFF, gestures will not work in fullscreen at all (unless the app is in the Include List).
- FullscreenIncludeList: ""
  $name: Fullscreen Include List
  $description: If "Allow in Fullscreen" is OFF, gestures will still work in these processes (e.g. chrome.exe, discord.exe).
- FullscreenExcludeList: ""
  $name: Fullscreen Exclude List
  $description: If "Allow in Fullscreen" is ON, gestures will NOT work in these processes.


- RecordHotkey: ctrl+shift+g
  $name: Record Canvas Hotkey
  $description: Press this anywhere to open the canvas to draw and record a shape.
- GestureSensitivity: 12
  $name: Gesture Sensitivity (8-20)
  $description: "How strict the shape matching is. Lower = Stricter, Higher = Looser. Default: 12."
- SearchEngineUrl: "https://www.google.com/search?q="
  $name: Search Engine URL
  $description: "URL template for Search Selection. The selected text will be appended. Example: https://duckduckgo.com/?q="


- Gestures:
  - - Name: "New Gesture"
      $name: Name
      $description: A friendly label for this gesture
    - DirectionSequence: ""
      $name: Gesture Code
      $description: Paste the exact code you copied from the drawing canvas here.
    - Action: launch
      $name: Action
      $options:
      - none: 🚫 None
      - launch: 🚀 Launch App/File/Shortcut (Requires Parameter)
      - shell_command: ⌨️ Run Command Line (Requires Parameter)
      - keyboard_shortcut: 🔠 Send Keyboard Shortcut (Requires Parameter)
      - task_manager: 📊 Open Task Manager
      - settings: ⚙️ Open Windows Settings
      - mute_volume: 🔇 Mute / Unmute Volume
      - play_pause: ⏯️ Play / Pause Media
      - next_track: ⏭️ Next Track
      - prev_track: ⏮️ Previous Track
      - draw: ✏️ Toggle Draw Mode
      - color_picker: 🎨 Open Color Picker
      - always_on_top: 📌 Toggle Window Always-on-Top
      - toggle_desktop_icons: 🖥️ Toggle Desktop Icons
      - fullscreen: 🔲 Toggle Fullscreen
      - maximize: 🗖 Maximize Window
      - minimize: 🗕 Minimize Window
      - close: ❌ Close Window
      - show_desktop: 👁️ Show Desktop
      - lock: 🔒 Lock PC
      - sleep_monitor: 🖥️ Sleep Monitor
      - sleep: 🌙 Sleep PC
      - restart: 🔄 Restart PC
      - shutdown: ⏻ Shut Down PC
      - empty_recycle_bin: 🗑️ Empty Recycle Bin
      - create_note: 📝 Create Sticky Note
      - search_selection: 🔍 Search Selection
      - screenshot: 📸 Take Screenshot
      - spotlight: 🔦 Trigger Spotlight
      - admin_terminal: ⚡ Admin Terminal Here
      - explorer_back: ⬅️ Explorer Back
      - explorer_forward: ➡️ Explorer Forward
      - stopwatch: ⏱️ Stopwatch
      - timer: ⏲️ Timer
      - flash: 💡 Flash (Flashlight)
      - next_virtual_desktop: 🖥️ Next Virtual Desktop
      - snap_top_left: ↖️ Snap to Top-Left
      - snap_top_right: ↗️ Snap to Top-Right
      - snap_bottom_left: ↙️ Snap to Bottom-Left
      - snap_bottom_right: ↘️ Snap to Bottom-Right
      - snap_left: ⬅️ Snap Left Half
      - snap_right: ➡️ Snap Right Half
      - snap_top: ⬆️ Snap Top Half
      - snap_bottom: ⬇️ Snap Bottom Half
    - ActionParam: ""
      $name: Action Parameter
      $description: Path, URL, or shortcut string depending on the action selected.
    - ContextRules:
      - - ProcessMatch: ""
          $name: Process Match
          $description: Substring match for foreground process name (e.g. chrome, explorer)
        - OverrideAction: none
          $name: Override Action
          $options:
          - none: 🚫 None
          - launch: 🚀 Launch App/File/Shortcut
          - shell_command: ⌨️ Run Command Line
          - keyboard_shortcut: 🔠 Send Keyboard Shortcut
          - task_manager: 📊 Open Task Manager
          - settings: ⚙️ Open Windows Settings
          - mute_volume: 🔇 Mute / Unmute Volume
          - play_pause: ⏯️ Play / Pause Media
          - next_track: ⏭️ Next Track
          - prev_track: ⏮️ Previous Track
          - draw: ✏️ Toggle Draw Mode
          - color_picker: 🎨 Open Color Picker
          - always_on_top: 📌 Toggle Window Always-on-Top
          - toggle_desktop_icons: 🖥️ Toggle Desktop Icons
          - fullscreen: 🔲 Toggle Fullscreen
          - maximize: 🗖 Maximize Window
          - minimize: 🗕 Minimize Window
          - close: ❌ Close Window
          - show_desktop: 👁️🗨️ Show Desktop
          - lock: 🔒 Lock PC
          - sleep_monitor: 🖥️ Sleep Monitor
          - sleep: 🌙 Sleep PC
          - restart: 🔄 Restart PC
          - shutdown: ⏻ Shut Down PC
          - empty_recycle_bin: 🗑️ Empty Recycle Bin
          - create_note: 📝 Create Sticky Note
          - search_selection: 🔍 Search Selection
          - screenshot: 📸 Take Screenshot
          - spotlight: 🔦 Trigger Spotlight
          - admin_terminal: ⚡ Admin Terminal Here
          - explorer_back: ⬅️ Explorer Back
          - explorer_forward: ➡️ Explorer Forward
          - stopwatch: ⏱️ Stopwatch
          - timer: ⏲️ Timer
          - flash: 💡 Flash (Flashlight)
          - next_virtual_desktop: 🖥️ Next Virtual Desktop
          - snap_top_left: ↖️ Snap to Top-Left
          - snap_top_right: ↗️ Snap to Top-Right
          - snap_bottom_left: ↙️ Snap to Bottom-Left
          - snap_bottom_right: ↘️ Snap to Bottom-Right
          - snap_left: ⬅️ Snap Left Half
          - snap_right: ➡️ Snap Right Half
          - snap_top: ⬆️ Snap Top Half
          - snap_bottom: ⬇️ Snap Bottom Half
        - OverrideParam: ""
          $name: Override Parameter
      $name: Context Rules
      $description: Override actions based on the active window process
  $name: Gestures
  $description: Add your custom gestures here.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <shellapi.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shobjidl.h>
#include <math.h>
#include <stdio.h>
#include <wchar.h>
#include <string>
#include <gdiplus.h>
#include <windowsx.h>
#include <uxtheme.h>
#include <highlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>



#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C
#endif
#ifndef XBUTTON1
#define XBUTTON1 0x0001
#endif
#ifndef GET_XBUTTON_WPARAM
#define GET_XBUTTON_WPARAM(wParam) (HIWORD(wParam))
#endif
#define WM_EXECUTE_ACTION (WM_APP + 101)
#define WM_POST_NOTE_CREATION (WM_APP + 102)
#define WM_HOOK_TOGGLE_DRAW (WM_APP + 103)
#define WM_HOOK_START_PICKER (WM_APP + 104)
#define WM_HOOK_START_SPOTLIGHT (WM_APP + 105)
#define WM_HOOK_TOGGLE_STOPWATCH (WM_APP + 106)
#define WM_HOOK_TOGGLE_TIMER (WM_APP + 107)
#define WM_HOOK_TOGGLE_FLASH (WM_APP + 108)
#define WM_HOOK_START_GESTURE (WM_APP + 109)
#define WM_HOOK_ADD_GESTURE_POINT (WM_APP + 110)
#define WM_HOOK_END_GESTURE (WM_APP + 111)
#define WM_HOOK_SHOW_AURA (WM_APP + 112)
#define WM_HOOK_HIDE_AURA (WM_APP + 113)
#define WM_HOOK_UPDATE_AURA (WM_APP + 114)
#define WM_HOOK_RECORD_CANVAS (WM_APP + 115)
#define WM_HOOK_KEY_TIMER_EDIT (WM_APP + 116)
#define WM_HOOK_MOUSE_NOTE (WM_APP + 117)
#define WM_HOOK_STOP_PICKER (WM_APP + 118)
#define WM_HOOK_MOUSE_PICKER (WM_APP + 119)
#define WM_HOOK_MOUSE_DRAW (WM_APP + 120)
static const int MAX_HEX_CODE = 512;

static const int MAX_GESTURES = 64;
static const int MAX_POINTS = 4096;

enum GestureAction {
    ACTION_NONE = 0,
    ACTION_LAUNCH,
    ACTION_SHELL_COMMAND,
    ACTION_KEYBOARD_SHORTCUT,
    ACTION_TASK_MANAGER,
    ACTION_SETTINGS,
    ACTION_MUTE_VOLUME,
    ACTION_PLAY_PAUSE,
    ACTION_DRAW,
    ACTION_COLOR_PICKER,
    ACTION_TOGGLE_DESKTOP_ICONS,
    ACTION_FULLSCREEN,
    ACTION_MAXIMIZE,
    ACTION_MINIMIZE,
    ACTION_CLOSE,
    ACTION_SHOW_DESKTOP,
    ACTION_LOCK,
    ACTION_SLEEP_MONITOR,
    ACTION_SLEEP,
    ACTION_RESTART,
    ACTION_SHUTDOWN,
    ACTION_EMPTY_RECYCLE_BIN,
    ACTION_SNAP_TOP_LEFT,
    ACTION_SNAP_TOP_RIGHT,
    ACTION_SNAP_BOTTOM_LEFT,
    ACTION_SNAP_BOTTOM_RIGHT,
    ACTION_SNAP_LEFT,
    ACTION_SNAP_RIGHT,
    ACTION_SNAP_TOP,
    ACTION_SNAP_BOTTOM,
    ACTION_NEXT_TRACK,
    ACTION_PREV_TRACK,
    ACTION_ALWAYS_ON_TOP,
    ACTION_CREATE_NOTE,
    ACTION_SEARCH_SELECTION,
    ACTION_SPOTLIGHT,
    ACTION_ADMIN_TERMINAL,
    ACTION_EXPLORER_BACK,
    ACTION_EXPLORER_FORWARD,
    ACTION_SCREENSHOT,
    ACTION_STOPWATCH,
    ACTION_TIMER,
    ACTION_FLASH,
    ACTION_NEXT_VIRTUAL_DESKTOP
};

enum ModifierType {
    MOD_CTRL_KEY = 1,
    MOD_SHIFT_KEY = 2,
    MOD_ALT_KEY = 4,
    MOD_LBUTTON = 8,
    MOD_RBUTTON = 16,
    MOD_MBUTTON = 32,
    MOD_XBUTTON1 = 64,
    MOD_XBUTTON2 = 128
};

enum ModifierBehaviorType {
    MOD_BEHAVIOR_HOLD = 0,
    MOD_BEHAVIOR_TOGGLE = 1
};

enum EnableWiggleType {
    WIGGLE_NEVER = 0,
    WIGGLE_ALWAYS = 1,
    WIGGLE_MODIFIER = 2
};

enum DrawButtonType {
    DRAW_RIGHT = 0,
    DRAW_MIDDLE = 1,
    DRAW_LEFT = 2,
    DRAW_MOUSE4 = 3,
    DRAW_MOUSE5 = 4
};

struct ContextRule {
    wchar_t processMatch[128];
    GestureAction overrideAction;
    wchar_t overrideParam[512];
};

struct GestureConfig {
    wchar_t name[128];
    GestureAction action;
    wchar_t actionParam[512];
    wchar_t directionSequence[MAX_HEX_CODE];
    ContextRule contextRules[8];
    int contextRuleCount;
};

struct Settings {
    int modifierFlags;
    ModifierBehaviorType modifierBehavior;
    DrawButtonType drawButton;
    COLORREF trailColor;
    int trailWidth;
    int armTimeout;
    EnableWiggleType enableWiggle;
    int wiggleStrength;
    UINT recordVk;
    int recordModifiers;
    BOOL allowInFullscreen;
    wchar_t fullscreenIncludeList[1024];
    wchar_t fullscreenExcludeList[1024];
    bool showAura;
    double matchThreshold;
    int minGestureDistance;
    wchar_t searchEngineUrl[512];
    GestureConfig gestures[MAX_GESTURES];
    int gestureCount;
};

static Settings g_settings = {};
static HHOOK g_mouseHook = nullptr;
static HHOOK g_keyboardHook = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;
static HANDLE g_notesThread = nullptr;
static DWORD g_notesThreadId = 0;
static std::atomic<bool> g_running{false};

static std::atomic<BOOL> g_gestureActive{FALSE};
static POINT g_points[MAX_POINTS];
static int g_pointCount = 0;

static HWND g_overlayWnd = nullptr;
static HDC g_memDC = NULL;
static HBITMAP g_memBitmap = NULL;
static HBITMAP g_oldBitmap = NULL;
static void* g_memBits = NULL;
static Gdiplus::Graphics* g_graphics = nullptr;
static HWND g_canvasWnd = nullptr;
static BOOL g_recording = FALSE;
static POINT g_canvasPoints[MAX_POINTS];
static int g_canvasPointCount = 0;
static BOOL g_canvasDrawing = FALSE;
static BOOL g_settingEditText = FALSE;

struct Particle {
    float x, y;
    float dx, dy;
    DWORD spawnTime;
    int lifeMs;
    float size;
    int colorType;
    BOOL active;
};
#define MAX_PARTICLES 100
static Particle g_particles[MAX_PARTICLES];
static int g_particleIndex = 0;

static BOOL g_fadeActive = FALSE;
static DWORD g_fadeStartTick = 0;
static POINT g_fadePoints[MAX_POINTS];
static int g_fadePointCount = 0;
static const int FADE_DURATION_MS = 150;

// Modifier Toggle State
static std::atomic<BOOL> g_modifierToggleArmed{FALSE};
static BOOL g_modifierWasActive = FALSE;
static DWORD g_modifierToggleArmTime = 0;

// Wiggle State
static std::atomic<BOOL> g_wiggleArmed{FALSE};
static DWORD g_wiggleArmTime = 0;
static HWND g_auraWnd = nullptr;
static int g_wiggleCount = 0;
static DWORD g_lastWiggleTime = 0;
static int g_wiggleAccum = 0;
static int g_wiggleSign = 0;
static POINT g_lastWigglePt = {0, 0};




struct DrawPoint {
    float x, y;
};
struct DrawStroke {
    DrawPoint points[1000];
    int count;
    Gdiplus::Color color;
    float width;
};
static DrawStroke g_drawStrokes[200];
static int g_drawStrokeCount = 0;
static DrawStroke g_currentDrawStroke = {};
static BOOL g_drawDrawing = FALSE;

static std::atomic<BOOL> g_drawModeActive{FALSE};
static HWND g_drawModeWnd = nullptr;
static Gdiplus::Color g_drawColor = Gdiplus::Color(255, 0, 170, 255);
static Gdiplus::Color g_drawSavedColor = Gdiplus::Color(0, 0, 0, 0);
static BOOL g_drawEraserMode = FALSE;
static float g_drawWidth = 5.0f;
static BOOL g_paletteVisible = FALSE;
static float g_paletteY = -60.0f;

static std::atomic<BOOL> g_pickerActive{FALSE};
static HWND g_pickerWnd = nullptr;
static POINT g_pickerPos = {};

static const wchar_t OVERLAY_CLASS[] = L"WindhawkGestureOverlay";
static const wchar_t CANVAS_CLASS[] = L"WindhawkGestureCanvas";
static const wchar_t DRAW_MODE_CLASS[] = L"WindhawkGestureDrawMode";
static const wchar_t PICKER_CLASS[] = L"WindhawkGestureColorPicker";
static const wchar_t MSG_WND_CLASS[] = L"WindhawkGestureMsgWnd";
static const wchar_t AURA_CLASS[] = L"WindhawkGestureAura";
static const wchar_t TOAST_CLASS[] = L"WindhawkGestureToast";
static const wchar_t STOPWATCH_CLASS[] = L"MagicMouseStopwatch";
static const wchar_t TIMER_CLASS[] = L"MagicMouseTimer";
static const wchar_t FLASH_CLASS[] = L"MagicMouseFlash";
static HWND g_msgWnd = nullptr;

static HWND g_stopwatchWnd = nullptr;
static BOOL g_stopwatchRunning = FALSE;
static LARGE_INTEGER g_stopwatchStart = {};
static LARGE_INTEGER g_stopwatchAccum = {};
static LARGE_INTEGER g_perfFreq = {};

static HWND g_timerWnd = nullptr;
static BOOL g_timerRunning = FALSE;
static int g_timerRemainingMs = 0;
static DWORD g_timerLastTick = 0;
static int g_timerPresets[3] = { 300, 600, 900 };
static int g_timerSelectedPreset = -1;
static int g_timerShakeState = 0;
static DWORD g_timerShakeStart = 0;
static float g_timerGlowPhase = 0.0f;
static std::atomic<BOOL> g_timerEditMode{FALSE};
static int g_timerEditIndex = -1;
static wchar_t g_timerEditBuf[16] = {};

static HWND g_flashWnd = nullptr;
struct MonitorBrightnessInfo {
    HMONITOR hMon;
    HANDLE hPhysical;
    DWORD originalBrightness;
    BOOL valid;
    BOOL opened;
};
#define MAX_FLASH_MONITORS 16
static MonitorBrightnessInfo g_flashMonitors[MAX_FLASH_MONITORS];
static int g_flashMonitorCount = 0;

static HWND g_toastWnd = nullptr;
static DWORD g_toastStartTick = 0;
static wchar_t g_toastText[128] = {};
static BOOL g_toastIsSuccess = FALSE;
static const int TOAST_DURATION_MS = 1200;

void ToggleDrawMode();
void StartColorPicker();
void StopColorPicker();
void ShowAura(POINT pt);
void HideAura();
void UpdateAura(POINT pt);
void TriggerStandaloneSplash(POINT pt);
void CreateOverlay();
void DestroyOverlay();
void UpdateOverlay();
void UpdateOverlayFade();
void UpdateParticles();
void ShowCanvas();
void HideCanvas();
void ShowToast(const wchar_t* text, BOOL isSuccess, POINT pt);
void ToggleStopwatch();
void ToggleTimer();
void ToggleFlash();

void StartGesture(POINT pt);
void AddGesturePoint(POINT pt);
void EndGesture();

static HFONT g_uiFont = nullptr;

void ParseModifierString(const wchar_t* str, int* outFlags) {
    *outFlags = 0;
    if (!str) return;

    wchar_t buf[128];
    wcsncpy_s(buf, str, _TRUNCATE);
    _wcslwr_s(buf);

    if (wcscmp(buf, L"none") == 0) {
        *outFlags = 0;
        return;
    }
    if (wcsstr(buf, L"ctrl"))  *outFlags |= MOD_CTRL_KEY;
    if (wcsstr(buf, L"shift")) *outFlags |= MOD_SHIFT_KEY;
    if (wcsstr(buf, L"alt"))   *outFlags |= MOD_ALT_KEY;
    if (wcsstr(buf, L"left"))  *outFlags |= MOD_LBUTTON;
    if (wcsstr(buf, L"right")) *outFlags |= MOD_RBUTTON;
    if (wcsstr(buf, L"middle")) *outFlags |= MOD_MBUTTON;
    if (wcsstr(buf, L"mouse4")) *outFlags |= MOD_XBUTTON1;
    if (wcsstr(buf, L"mouse5")) *outFlags |= MOD_XBUTTON2;
}

DrawButtonType ParseDrawButton(const wchar_t* str) {
    if (!str) return DRAW_RIGHT;
    wchar_t buf[32];
    wcsncpy_s(buf, str, _TRUNCATE);
    _wcslwr_s(buf);
    if (wcscmp(buf, L"middle") == 0) return DRAW_MIDDLE;
    if (wcscmp(buf, L"left") == 0) return DRAW_LEFT;
    if (wcscmp(buf, L"mouse4") == 0) return DRAW_MOUSE4;
    if (wcscmp(buf, L"mouse5") == 0) return DRAW_MOUSE5;
    return DRAW_RIGHT;
}

void ParseHotkeyString(const wchar_t* str, UINT* outVk, int* outMod) {
    *outVk = 0;
    *outMod = 0;
    if (!str) {
        *outVk = 'G';
        return;
    }

    wchar_t buf[128];
    wcsncpy_s(buf, str, _TRUNCATE);
    _wcslwr_s(buf);

    if (wcsstr(buf, L"ctrl"))  *outMod |= MOD_CTRL_KEY;
    if (wcsstr(buf, L"shift")) *outMod |= MOD_SHIFT_KEY;
    if (wcsstr(buf, L"alt"))   *outMod |= MOD_ALT_KEY;

    const wchar_t* last = wcsrchr(buf, L'+');
    const wchar_t* keyStr = last ? last + 1 : buf;
    while (*keyStr == L' ') keyStr++;

    if (wcslen(keyStr) == 1) {
        wchar_t key = towupper(keyStr[0]);
        if ((key >= L'A' && key <= L'Z') || (key >= L'0' && key <= L'9')) *outVk = key;
    } else {
        if (wcscmp(keyStr, L"f1") == 0) *outVk = VK_F1;
        else if (wcscmp(keyStr, L"f2") == 0) *outVk = VK_F2;
        else if (wcscmp(keyStr, L"f3") == 0) *outVk = VK_F3;
        else if (wcscmp(keyStr, L"f4") == 0) *outVk = VK_F4;
        else if (wcscmp(keyStr, L"f5") == 0) *outVk = VK_F5;
        else if (wcscmp(keyStr, L"f6") == 0) *outVk = VK_F6;
        else if (wcscmp(keyStr, L"f7") == 0) *outVk = VK_F7;
        else if (wcscmp(keyStr, L"f8") == 0) *outVk = VK_F8;
        else if (wcscmp(keyStr, L"f9") == 0) *outVk = VK_F9;
        else if (wcscmp(keyStr, L"f10") == 0) *outVk = VK_F10;
        else if (wcscmp(keyStr, L"f11") == 0) *outVk = VK_F11;
        else if (wcscmp(keyStr, L"f12") == 0) *outVk = VK_F12;
    }

    if (*outVk == 0) {
        *outVk = 'G'; // Fallback if no valid key was found
    }
}

COLORREF ParseHexColor(const wchar_t* str) {
    if (!str || wcslen(str) < 6) return RGB(0, 170, 255);
    unsigned int r = 0, g = 0, b = 0;
    swscanf_s(str, L"%02x%02x%02x", &r, &g, &b);
    return RGB(r, g, b);
}

GestureAction ParseAction(const wchar_t* str) {
    if (!str) return ACTION_NONE;
    wchar_t buf[64];
    wcsncpy_s(buf, str, _TRUNCATE);
    _wcslwr_s(buf);
    if (wcscmp(buf, L"launch") == 0) return ACTION_LAUNCH;
    if (wcscmp(buf, L"shell_command") == 0) return ACTION_SHELL_COMMAND;
    if (wcscmp(buf, L"keyboard_shortcut") == 0) return ACTION_KEYBOARD_SHORTCUT;
    if (wcscmp(buf, L"task_manager") == 0) return ACTION_TASK_MANAGER;
    if (wcscmp(buf, L"settings") == 0) return ACTION_SETTINGS;
    if (wcscmp(buf, L"mute_volume") == 0) return ACTION_MUTE_VOLUME;
    if (wcscmp(buf, L"play_pause") == 0) return ACTION_PLAY_PAUSE;
    if (wcscmp(buf, L"draw") == 0) return ACTION_DRAW;
    if (wcscmp(buf, L"color_picker") == 0) return ACTION_COLOR_PICKER;
    if (wcscmp(buf, L"toggle_desktop_icons") == 0) return ACTION_TOGGLE_DESKTOP_ICONS;
    if (wcscmp(buf, L"fullscreen") == 0) return ACTION_FULLSCREEN;
    if (wcscmp(buf, L"maximize") == 0) return ACTION_MAXIMIZE;
    if (wcscmp(buf, L"minimize") == 0) return ACTION_MINIMIZE;
    if (wcscmp(buf, L"close") == 0) return ACTION_CLOSE;
    if (wcscmp(buf, L"show_desktop") == 0) return ACTION_SHOW_DESKTOP;
    if (wcscmp(buf, L"lock") == 0) return ACTION_LOCK;
    if (wcscmp(buf, L"sleep_monitor") == 0) return ACTION_SLEEP_MONITOR;
    if (wcscmp(buf, L"sleep") == 0) return ACTION_SLEEP;
    if (wcscmp(buf, L"restart") == 0) return ACTION_RESTART;
    if (wcscmp(buf, L"shutdown") == 0) return ACTION_SHUTDOWN;
    if (wcscmp(buf, L"empty_recycle_bin") == 0) return ACTION_EMPTY_RECYCLE_BIN;
    if (wcscmp(buf, L"snap_top_left") == 0) return ACTION_SNAP_TOP_LEFT;
    if (wcscmp(buf, L"snap_top_right") == 0) return ACTION_SNAP_TOP_RIGHT;
    if (wcscmp(buf, L"snap_bottom_left") == 0) return ACTION_SNAP_BOTTOM_LEFT;
    if (wcscmp(buf, L"snap_bottom_right") == 0) return ACTION_SNAP_BOTTOM_RIGHT;
    if (wcscmp(buf, L"snap_left") == 0) return ACTION_SNAP_LEFT;
    if (wcscmp(buf, L"snap_right") == 0) return ACTION_SNAP_RIGHT;
    if (wcscmp(buf, L"snap_top") == 0) return ACTION_SNAP_TOP;
    if (wcscmp(buf, L"snap_bottom") == 0) return ACTION_SNAP_BOTTOM;
    if (wcscmp(buf, L"next_track") == 0) return ACTION_NEXT_TRACK;
    if (wcscmp(buf, L"prev_track") == 0) return ACTION_PREV_TRACK;
    if (wcscmp(buf, L"always_on_top") == 0) return ACTION_ALWAYS_ON_TOP;
    if (wcscmp(buf, L"create_note") == 0) return ACTION_CREATE_NOTE;
    if (wcscmp(buf, L"search_selection") == 0) return ACTION_SEARCH_SELECTION;
    if (wcscmp(buf, L"spotlight") == 0) return ACTION_SPOTLIGHT;
    if (wcscmp(buf, L"admin_terminal") == 0) return ACTION_ADMIN_TERMINAL;
    if (wcscmp(buf, L"explorer_back") == 0) return ACTION_EXPLORER_BACK;
    if (wcscmp(buf, L"explorer_forward") == 0) return ACTION_EXPLORER_FORWARD;
    if (wcscmp(buf, L"screenshot") == 0) return ACTION_SCREENSHOT;
    if (wcscmp(buf, L"stopwatch") == 0) return ACTION_STOPWATCH;
    if (wcscmp(buf, L"timer") == 0) return ACTION_TIMER;
    if (wcscmp(buf, L"flash") == 0) return ACTION_FLASH;
    if (wcscmp(buf, L"next_virtual_desktop") == 0) return ACTION_NEXT_VIRTUAL_DESKTOP;
    return ACTION_NONE;
}

BOOL IsModifierActive() {
    int flags = g_settings.modifierFlags;
    if (flags == 0) return TRUE;

    BOOL ok = TRUE;
    if (flags & MOD_CTRL_KEY)  ok = ok && (GetAsyncKeyState(VK_CONTROL) & 0x8000);
    if (flags & MOD_SHIFT_KEY) ok = ok && (GetAsyncKeyState(VK_SHIFT) & 0x8000);
    if (flags & MOD_ALT_KEY)   ok = ok && (GetAsyncKeyState(VK_MENU) & 0x8000);
    if (flags & MOD_LBUTTON)   ok = ok && (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
    if (flags & MOD_RBUTTON)   ok = ok && (GetAsyncKeyState(VK_RBUTTON) & 0x8000);
    if (flags & MOD_MBUTTON)   ok = ok && (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
    if (flags & MOD_XBUTTON1)  ok = ok && (GetAsyncKeyState(VK_XBUTTON1) & 0x8000);
    if (flags & MOD_XBUTTON2)  ok = ok && (GetAsyncKeyState(VK_XBUTTON2) & 0x8000);
    return ok;
}

void SendKeyboardShortcut(const wchar_t* str) {
    if (!str || !str[0]) return;

    wchar_t buf[128];
    wcsncpy_s(buf, str, _TRUNCATE);
    _wcslwr_s(buf);

    BOOL hasCtrl = wcsstr(buf, L"ctrl") != nullptr;
    BOOL hasShift = wcsstr(buf, L"shift") != nullptr;
    BOOL hasAlt = wcsstr(buf, L"alt") != nullptr;
    BOOL hasWin = wcsstr(buf, L"win") != nullptr;

    UINT vk = 0;
    const wchar_t* last = wcsrchr(buf, L'+');
    const wchar_t* keyStr = last ? last + 1 : buf;
    while (*keyStr == L' ') keyStr++;

    if (wcslen(keyStr) == 1) {
        wchar_t key = towupper(keyStr[0]);
        if ((key >= L'A' && key <= L'Z') || (key >= L'0' && key <= L'9')) {
            vk = key;
        }
    } else {
        if (wcscmp(keyStr, L"f1") == 0) vk = VK_F1;
        else if (wcscmp(keyStr, L"f2") == 0) vk = VK_F2;
        else if (wcscmp(keyStr, L"f3") == 0) vk = VK_F3;
        else if (wcscmp(keyStr, L"f4") == 0) vk = VK_F4;
        else if (wcscmp(keyStr, L"f5") == 0) vk = VK_F5;
        else if (wcscmp(keyStr, L"f6") == 0) vk = VK_F6;
        else if (wcscmp(keyStr, L"f7") == 0) vk = VK_F7;
        else if (wcscmp(keyStr, L"f8") == 0) vk = VK_F8;
        else if (wcscmp(keyStr, L"f9") == 0) vk = VK_F9;
        else if (wcscmp(keyStr, L"f10") == 0) vk = VK_F10;
        else if (wcscmp(keyStr, L"f11") == 0) vk = VK_F11;
        else if (wcscmp(keyStr, L"f12") == 0) vk = VK_F12;
        else if (wcscmp(keyStr, L"tab") == 0) vk = VK_TAB;
        else if (wcscmp(keyStr, L"esc") == 0) vk = VK_ESCAPE;
        else if (wcscmp(keyStr, L"escape") == 0) vk = VK_ESCAPE;
        else if (wcscmp(keyStr, L"enter") == 0) vk = VK_RETURN;
        else if (wcscmp(keyStr, L"space") == 0) vk = VK_SPACE;
        else if (wcscmp(keyStr, L"backspace") == 0) vk = VK_BACK;
        else if (wcscmp(keyStr, L"delete") == 0) vk = VK_DELETE;
        else if (wcscmp(keyStr, L"del") == 0) vk = VK_DELETE;
        else if (wcscmp(keyStr, L"insert") == 0) vk = VK_INSERT;
        else if (wcscmp(keyStr, L"home") == 0) vk = VK_HOME;
        else if (wcscmp(keyStr, L"end") == 0) vk = VK_END;
        else if (wcscmp(keyStr, L"pageup") == 0) vk = VK_PRIOR;
        else if (wcscmp(keyStr, L"pagedown") == 0) vk = VK_NEXT;
        else if (wcscmp(keyStr, L"up") == 0) vk = VK_UP;
        else if (wcscmp(keyStr, L"down") == 0) vk = VK_DOWN;
        else if (wcscmp(keyStr, L"left") == 0) vk = VK_LEFT;
        else if (wcscmp(keyStr, L"right") == 0) vk = VK_RIGHT;
    }

    if (vk == 0) return;

    INPUT inputs[12] = {};
    memset(inputs, 0, sizeof(inputs));

    int idx = 0;
    if (hasCtrl) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++; }
    if (hasShift) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_SHIFT; idx++; }
    if (hasAlt) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++; }
    if (hasWin) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_LWIN; idx++; }

    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = (WORD)vk; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = (WORD)vk; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;

    if (hasWin) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_LWIN; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++; }
    if (hasAlt) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++; }
    if (hasShift) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_SHIFT; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++; }
    if (hasCtrl) { inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++; }

    SendInput(idx, inputs, sizeof(INPUT));
}

std::wstring UrlEncode(const std::wstring& value) {
    std::wstring escaped;
    escaped.reserve(value.length());
    for (size_t i = 0; i < value.length(); ++i) {
        wchar_t c = value[i];
        if (iswalnum(c) || c == L'-' || c == L'_' || c == L'.' || c == L'~') {
            escaped += c;
        } else if (c == L' ') {
            escaped += L'+';
        } else {
            char utf8[4];
            int len = WideCharToMultiByte(CP_UTF8, 0, &c, 1, utf8, 4, NULL, NULL);
            for (int j = 0; j < len; j++) {
                wchar_t buf[4];
                swprintf_s(buf, L"%%%02X", (unsigned char)utf8[j]);
                escaped += buf;
            }
        }
    }
    return escaped;
}

std::wstring GetSelectedText() {
    std::wstring selectedText = L"";

    IDataObject* pDataObj = nullptr;
    HRESULT hrOle = OleGetClipboard(&pDataObj);

    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_CONTROL;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = 'C';
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = 'C'; inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_CONTROL; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));

    Sleep(100);

    if (OpenClipboard(NULL)) {
        HANDLE hDrop = GetClipboardData(CF_HDROP);
        if (hDrop) {
            HDROP hDropInfo = (HDROP)hDrop;
            UINT fileCount = DragQueryFileW(hDropInfo, 0xFFFFFFFF, NULL, 0);
            if (fileCount > 0) {
                wchar_t filePath[MAX_PATH];
                if (DragQueryFileW(hDropInfo, 0, filePath, MAX_PATH)) {
                    selectedText = filePath;
                }
            }
        }
        
        if (selectedText.empty()) {
            HANDLE hData = GetClipboardData(CF_UNICODETEXT);
            if (hData) {
                wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
                if (pszText) {
                    selectedText = pszText;
                    GlobalUnlock(hData);
                }
            }
        }
        CloseClipboard();
    }

    if (SUCCEEDED(hrOle) && pDataObj) {
        OleSetClipboard(pDataObj);
        OleFlushClipboard();
        pDataObj->Release();
    }

    return selectedText;
}

// Search Image helpers removed to reduce bugs

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

void TakeScreenshot(const std::wstring& customPath) {
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);
    
    extern HWND g_overlayWnd;
    extern HWND g_auraWnd;
    if (g_overlayWnd) ShowWindow(g_overlayWnd, SW_HIDE);
    if (g_auraWnd) ShowWindow(g_auraWnd, SW_HIDE);
    
    Sleep(150);
    
    int width = mi.rcMonitor.right - mi.rcMonitor.left;
    int height = mi.rcMonitor.bottom - mi.rcMonitor.top;
    
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, mi.rcMonitor.left, mi.rcMonitor.top, SRCCOPY);
    SelectObject(hdcMem, hOld);
    
    HDC hdcCopy = CreateCompatibleDC(hdcScreen);
    HBITMAP hClipBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HGDIOBJ hOldCopy = SelectObject(hdcCopy, hClipBitmap);
    HDC hdcSrc = CreateCompatibleDC(hdcScreen);
    HGDIOBJ hOldSrc = SelectObject(hdcSrc, hBitmap);
    BitBlt(hdcCopy, 0, 0, width, height, hdcSrc, 0, 0, SRCCOPY);
    SelectObject(hdcSrc, hOldSrc);
    SelectObject(hdcCopy, hOldCopy);
    DeleteDC(hdcSrc);
    DeleteDC(hdcCopy);
    
    std::wstring outPath;
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(hBitmap, NULL);
    CLSID pngClsid;
    if (GetEncoderClsid(L"image/png", &pngClsid) != -1) {
        outPath = customPath;
        if (outPath.empty()) {
            PWSTR picturesPath = NULL;
            if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &picturesPath))) {
                outPath = picturesPath;
                CoTaskMemFree(picturesPath);
            } else {
                outPath = L"C:\\"; 
            }
        }
        
        if (!outPath.empty() && outPath.back() != L'\\' && outPath.back() != L'/') outPath += L"\\";
        
        SYSTEMTIME st;
        GetLocalTime(&st);
        wchar_t filename[128];
        swprintf_s(filename, L"Screenshot_%04d%02d%02d_%02d%02d%02d.png", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        outPath += filename;
        
        bmp->Save(outPath.c_str(), &pngClsid, NULL);
    }
    delete bmp;
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    
    HGLOBAL hDrop = NULL;
    if (!outPath.empty()) {
        size_t pathLen = (outPath.length() + 2) * sizeof(wchar_t);
        hDrop = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DROPFILES) + pathLen);
        if (hDrop) {
            DROPFILES* pdf = (DROPFILES*)GlobalLock(hDrop);
            pdf->pFiles = sizeof(DROPFILES);
            pdf->fWide = TRUE;
            wchar_t* pDst = (wchar_t*)((BYTE*)pdf + sizeof(DROPFILES));
            wcscpy_s(pDst, outPath.length() + 1, outPath.c_str());
            GlobalUnlock(hDrop);
        }
    }
    
    for (int retry = 0; retry < 10; ++retry) {
        if (OpenClipboard(GetForegroundWindow())) {
            EmptyClipboard();
            SetClipboardData(CF_BITMAP, hClipBitmap);
            if (hDrop) {
                SetClipboardData(CF_HDROP, hDrop);
            }
            CloseClipboard();
            return;
        }
        Sleep(50);
    }
    DeleteObject(hClipBitmap);
    if (hDrop) GlobalFree(hDrop);
}

void GetForegroundProcessName(HWND hwnd, wchar_t* outName, DWORD maxLen);

BOOL IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg) return FALSE;

    RECT rc;
    GetWindowRect(fg, &rc);

    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(mon, &mi);

    BOOL isFullscreen = (rc.left == mi.rcMonitor.left &&
                         rc.top == mi.rcMonitor.top &&
                         rc.right == mi.rcMonitor.right &&
                         rc.bottom == mi.rcMonitor.bottom);

    if (!isFullscreen) return FALSE;

    // Fast path: If both lists are empty, we don't need to do slow OpenProcess calls
    if (g_settings.fullscreenIncludeList[0] == L'\0' && g_settings.fullscreenExcludeList[0] == L'\0') {
        return !g_settings.allowInFullscreen;
    }

    wchar_t fgName[128] = {0};
    GetForegroundProcessName(fg, fgName, 128);
    _wcslwr_s(fgName);

    auto listContains = [](const wchar_t* listStr, const wchar_t* processName) -> BOOL {
        if (listStr[0] == L'\0') return FALSE;
        wchar_t listCopy[1024];
        wcsncpy_s(listCopy, listStr, _TRUNCATE);
        _wcslwr_s(listCopy);
        
        wchar_t* context = nullptr;
        wchar_t* token = wcstok_s(listCopy, L",; ", &context);
        while (token) {
            if (wcsstr(processName, token) != nullptr) {
                return TRUE;
            }
            token = wcstok_s(nullptr, L",; ", &context);
        }
        return FALSE;
    };

    if (g_settings.allowInFullscreen) {
        if (listContains(g_settings.fullscreenExcludeList, fgName)) {
            return TRUE; // Blocked because it's excluded
        }
        return FALSE; // Allowed
    } else {
        if (listContains(g_settings.fullscreenIncludeList, fgName)) {
            return FALSE; // Allowed because it's included
        }
        return TRUE; // Blocked
    }
}


#define NUM_RESAMPLE_POINTS 64

struct PointD {
    double x, y;
};

void Resample(const POINT* pts, int count, PointD* outPts) {
    double totalLength = 0;
    for (int i = 1; i < count; i++) {
        double dx = pts[i].x - pts[i - 1].x;
        double dy = pts[i].y - pts[i - 1].y;
        totalLength += sqrt(dx * dx + dy * dy);
    }
    double interval = totalLength / (NUM_RESAMPLE_POINTS - 1);
    if (interval < 0.001) interval = 0.001;
    double D = 0;

    outPts[0] = { (double)pts[0].x, (double)pts[0].y };
    int numAdded = 1;

    double curX = pts[0].x, curY = pts[0].y;
    for (int i = 1; i < count; i++) {
        double nextX = pts[i].x, nextY = pts[i].y;
        double dx = nextX - curX;
        double dy = nextY - curY;
        double d = sqrt(dx * dx + dy * dy);

        if (D + d >= interval) {
            double qx = curX + ((interval - D) / d) * dx;
            double qy = curY + ((interval - D) / d) * dy;
            outPts[numAdded++] = { qx, qy };
            curX = qx;
            curY = qy;
            D = 0;
            i--; // keep comparing with next point
        } else {
            D += d;
            curX = nextX;
            curY = nextY;
        }
        if (numAdded == NUM_RESAMPLE_POINTS) break;
    }
    while (numAdded < NUM_RESAMPLE_POINTS) {
        outPts[numAdded++] = { (double)pts[count - 1].x, (double)pts[count - 1].y };
    }
}

void ScaleToBox(PointD* pts) {
    double minX = 999999, maxX = -999999, minY = 999999, maxY = -999999;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        if (pts[i].x < minX) minX = pts[i].x;
        if (pts[i].x > maxX) maxX = pts[i].x;
        if (pts[i].y < minY) minY = pts[i].y;
        if (pts[i].y > maxY) maxY = pts[i].y;
    }
    double w = maxX - minX, h = maxY - minY;
    double scale = (w > h) ? w : h;
    if (scale == 0) scale = 1;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        pts[i].x = (pts[i].x - minX) / scale * 100.0;
        pts[i].y = (pts[i].y - minY) / scale * 100.0;
    }
}

void TranslateToOrigin(PointD* pts) {
    double cx = 0, cy = 0;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) { cx += pts[i].x; cy += pts[i].y; }
    cx /= NUM_RESAMPLE_POINTS; cy /= NUM_RESAMPLE_POINTS;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) { pts[i].x -= cx; pts[i].y -= cy; }
}

void NormalizeGesture(const POINT* pts, int count, PointD* outPts) {
    Resample(pts, count, outPts);
    ScaleToBox(outPts);
    TranslateToOrigin(outPts);
}

void FormatGestureHex(const PointD* pts, wchar_t* outHex) {
    outHex[0] = 0;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        int x = (int)(pts[i].x + 50.0);
        int y = (int)(pts[i].y + 50.0);
        if (x < 0) x = 0; if (x > 255) x = 255;
        if (y < 0) y = 0; if (y > 255) y = 255;
        swprintf_s(outHex + i * 4, 5, L"%02x%02x", x, y);
    }
}

bool ParseGestureHex(const wchar_t* hex, PointD* outPts) {
    if (wcslen(hex) != NUM_RESAMPLE_POINTS * 4) return false;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        unsigned int x, y;
        wchar_t buf[5] = { hex[i * 4], hex[i * 4 + 1], hex[i * 4 + 2], hex[i * 4 + 3], 0 };
        if (swscanf_s(buf, L"%02x%02x", &x, &y) != 2) return false;
        outPts[i].x = (double)x - 50.0;
        outPts[i].y = (double)y - 50.0;
    }
    return true;
}

double AveragePointDistance(const PointD* pts1, const PointD* pts2) {
    double dist = 0;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        double dx = pts1[i].x - pts2[i].x;
        double dy = pts1[i].y - pts2[i].y;
        dist += sqrt(dx * dx + dy * dy);
    }
    return dist / NUM_RESAMPLE_POINTS;
}

int MatchGesture(const PointD* drawnPts) {
    int bestIdx = -1;
    double bestDist = 999999.0;
    double threshold = g_settings.matchThreshold;

    for (int i = 0; i < g_settings.gestureCount; i++) {
        if (g_settings.gestures[i].directionSequence[0] == 0) continue;
        if (g_settings.gestures[i].action == ACTION_NONE) continue;

        PointD templatePts[NUM_RESAMPLE_POINTS];
        if (!ParseGestureHex(g_settings.gestures[i].directionSequence, templatePts)) continue;

        double dist = AveragePointDistance(drawnPts, templatePts);
        if (dist <= threshold && dist < bestDist) {
            bestDist = dist;
            bestIdx = i;
        }
    }
    return bestIdx;
}

void ToggleDesktopIcons() {
    HWND progman = FindWindow(L"Progman", NULL);
    if (!progman) return;

    HWND defView = FindWindowEx(progman, NULL, L"SHELLDLL_DefView", NULL);
    if (!defView) {
        HWND worker = NULL;
        do {
            worker = FindWindowEx(GetDesktopWindow(), worker, L"WorkerW", NULL);
            if (worker) {
                defView = FindWindowEx(worker, NULL, L"SHELLDLL_DefView", NULL);
                if (defView) break;
            }
        } while (worker);
    }

    if (defView) {
        HWND listView = FindWindowEx(defView, NULL, L"SysListView32", NULL);
        if (listView) {
            BOOL visible = IsWindowVisible(listView);
            ShowWindow(listView, visible ? SW_HIDE : SW_SHOW);
        }
    }
}

void ToggleFullscreen(HWND fg) {
    if (!fg) return;

    wchar_t className[256] = {};
    GetClassName(fg, className, 256);

    BOOL useF11 = FALSE;
    if (wcscmp(className, L"Chrome_WidgetWin_1") == 0 ||
        wcscmp(className, L"MozillaWindowClass") == 0 ||
        wcscmp(className, L"CabinetWClass") == 0 ||
        wcscmp(className, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0 ||
        wcsstr(className, L"IEFrame") != NULL) {
        useF11 = TRUE;
    }

    if (useF11) {
        INPUT inputs[2] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_F11;

        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK_F11;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(2, inputs, sizeof(INPUT));
    } else {
        WINDOWPLACEMENT wp = { sizeof(wp) };
        GetWindowPlacement(fg, &wp);
        if (wp.showCmd == SW_SHOWMAXIMIZED) {
            DWORD_PTR dwResult;
            SendMessageTimeout(fg, WM_SYSCOMMAND, SC_RESTORE, 0, SMTO_ABORTIFHUNG, 500, &dwResult);
        } else {
            DWORD_PTR dwResult;
            SendMessageTimeout(fg, WM_SYSCOMMAND, SC_MAXIMIZE, 0, SMTO_ABORTIFHUNG, 500, &dwResult);
        }
    }
}

void ShowDesktop() {
    INPUT inputs[4] = {};
    
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'D';
    
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'D';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_LWIN;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    
    SendInput(4, inputs, sizeof(INPUT));
}

BOOL GetExplorerPath(HWND hwnd, wchar_t* outPath, DWORD maxLen) {
    outPath[0] = L'\0';
    if (!hwnd) return FALSE;

    wchar_t className[256] = {0};
    HWND hwndExplorer = hwnd;
    while (hwndExplorer) {
        if (GetClassName(hwndExplorer, className, 256)) {
            if (wcscmp(className, L"CabinetWClass") == 0 ||
                wcscmp(className, L"Progman") == 0 ||
                wcscmp(className, L"WorkerW") == 0) {
                break;
            }
        }
        hwndExplorer = GetParent(hwndExplorer);
    }
    if (!hwndExplorer) return FALSE;

    if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0) {
        SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, outPath);
        return TRUE;
    }

    IShellWindows* psw = NULL;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, IID_IShellWindows, (void**)&psw);
    if (SUCCEEDED(hr)) {
        long count = 0;
        psw->get_Count(&count);
        wchar_t fallbackPath[MAX_PATH] = {0};
        for (long i = 0; i < count; i++) {
            VARIANT v;
            VariantInit(&v);
            v.vt = VT_I4;
            v.lVal = i;
            IDispatch* pdisp = NULL;
            if (SUCCEEDED(psw->Item(v, &pdisp)) && pdisp) {
                IWebBrowser2* pwb = NULL;
                if (SUCCEEDED(pdisp->QueryInterface(IID_IWebBrowser2, (void**)&pwb)) && pwb) {
                    SHANDLE_PTR hwndB = 0;
                    if (SUCCEEDED(pwb->get_HWND(&hwndB)) && (HWND)hwndB == hwndExplorer) {
                        IServiceProvider* psp = NULL;
                        if (SUCCEEDED(pwb->QueryInterface(IID_IServiceProvider, (void**)&psp)) && psp) {
                            IShellBrowser* psb = NULL;
                            if (SUCCEEDED(psp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&psb)) && psb) {
                                HWND thisTab = NULL;
                                if (SUCCEEDED(psb->GetWindow(&thisTab)) && thisTab) {
                                    BOOL isTargetChild = (thisTab == hwnd || IsChild(thisTab, hwnd));
                                    BOOL isVisible = IsWindowVisible(thisTab);
                                    if (isTargetChild || isVisible) {
                                        IShellView* psv = NULL;
                                        if (SUCCEEDED(psb->QueryActiveShellView(&psv)) && psv) {
                                            IFolderView* pfv = NULL;
                                            if (SUCCEEDED(psv->QueryInterface(IID_IFolderView, (void**)&pfv)) && pfv) {
                                                IPersistFolder2* ppf2 = NULL;
                                                if (SUCCEEDED(pfv->GetFolder(IID_IPersistFolder2, (void**)&ppf2)) && ppf2) {
                                                    PIDLIST_ABSOLUTE pidl = NULL;
                                                    if (SUCCEEDED(ppf2->GetCurFolder(&pidl)) && pidl) {
                                                        wchar_t currentPath[MAX_PATH] = {0};
                                                        SHGetPathFromIDList(pidl, currentPath);
                                                        CoTaskMemFree(pidl);
                                                        if (currentPath[0] != L'\0') {
                                                            if (isTargetChild) {
                                                                wcsncpy_s(outPath, maxLen, currentPath, _TRUNCATE);
                                                            } else if (fallbackPath[0] == L'\0') {
                                                                wcsncpy_s(fallbackPath, MAX_PATH, currentPath, _TRUNCATE);
                                                            }
                                                        }
                                                    }
                                                    ppf2->Release();
                                                }
                                                pfv->Release();
                                            }
                                            psv->Release();
                                        }
                                    }
                                }
                                psb->Release();
                            }
                            psp->Release();
                        }
                    }
                    pwb->Release();
                }
                pdisp->Release();
            }
            if (outPath[0] != L'\0') {
                break;
            }
        }
        psw->Release();
        if (outPath[0] == L'\0' && fallbackPath[0] != L'\0') {
            wcsncpy_s(outPath, maxLen, fallbackPath, _TRUNCATE);
        }
    }
    return outPath[0] != L'\0';
}

void GetForegroundProcessName(HWND hwnd, wchar_t* outName, DWORD maxLen) {
    outName[0] = L'\0';
    if (!hwnd) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProc) {
        wchar_t path[MAX_PATH];
        DWORD size = MAX_PATH;
        if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
            wchar_t* name = wcsrchr(path, L'\\');
            if (name) {
                wcsncpy_s(outName, maxLen, name + 1, _TRUNCATE);
            } else {
                wcsncpy_s(outName, maxLen, path, _TRUNCATE);
            }
            _wcslwr_s(outName, maxLen);
        }
        CloseHandle(hProc);
    }
}


// ============================================================================
// MAGIC STICKY NOTES SYSTEM
// ============================================================================
#include <richedit.h>

struct NoteData {
    int id;
    RECT rect;
    DWORD color;
    int opacity;
    BOOL pinned;
    wchar_t text[4096];
    HWND hwnd;
    HWND hwndEdit;
};

#define MAX_NOTES 50
static NoteData g_notes[MAX_NOTES];
static int g_noteCount = 0;
static int g_noteIdCounter = 1;

static std::atomic<BOOL> g_noteCreationMode{FALSE};
static POINT g_noteSelStart = {0,0};
static POINT g_noteSelCurrent = {0,0};
static DWORD g_lastNoteColor = RGB(43, 43, 43);
static int g_lastNoteOpacity = 255;
static HWND g_noteSelWnd = nullptr;
static HWND g_mainMsgWnd = nullptr;
static const wchar_t MAIN_MSG_WND_CLASS[] = L"WindhawkMagicNotesMainMsg";

static const wchar_t NOTE_CLASS[] = L"WindhawkStickyNote";
static const wchar_t NOTE_SEL_CLASS[] = L"WindhawkStickyNoteSel";

void SaveNotes() {
    for (int i = 0; i < g_noteCount; i++) {
        if (g_notes[i].hwndEdit) {
            GetWindowText(g_notes[i].hwndEdit, g_notes[i].text, 4096);
            GetWindowRect(g_notes[i].hwnd, &g_notes[i].rect);
        }
    }
    size_t totalSize = sizeof(int) * 2 + sizeof(NoteData) * g_noteCount + sizeof(DWORD) + sizeof(int);
    BYTE* buffer = (BYTE*)malloc(totalSize);
    if (!buffer) return;
    BYTE* p = buffer;
    memcpy(p, &g_noteCount, sizeof(int)); p += sizeof(int);
    memcpy(p, &g_noteIdCounter, sizeof(int)); p += sizeof(int);
    for (int i = 0; i < g_noteCount; i++) {
        memcpy(p, &g_notes[i], sizeof(NoteData)); p += sizeof(NoteData);
    }
    memcpy(p, &g_lastNoteColor, sizeof(DWORD)); p += sizeof(DWORD);
    memcpy(p, &g_lastNoteOpacity, sizeof(int)); p += sizeof(int);
    Wh_SetBinaryValue(L"NotesData", buffer, totalSize);
    free(buffer);
}

void DeleteNote(int id) {
    for (int i = 0; i < g_noteCount; i++) {
        if (g_notes[i].id == id) {
            if (g_notes[i].hwnd) DestroyWindow(g_notes[i].hwnd);
            for (int j = i; j < g_noteCount - 1; j++) g_notes[j] = g_notes[j+1];
            g_noteCount--;
            SaveNotes();
            break;
        }
    }
}

NoteData* GetNoteByHwnd(HWND hwnd) {
    for (int i = 0; i < g_noteCount; i++) {
        if (g_notes[i].hwnd == hwnd || g_notes[i].hwndEdit == hwnd) return &g_notes[i];
    }
    return nullptr;
}

LRESULT CALLBACK NoteEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_RBUTTONUP) {
        SendMessage(GetParent(hWnd), WM_RBUTTONUP, wParam, lParam);
        return 0;
    }
    
    if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE) {
        SetFocus(NULL);
        return 0;
    }
    
    if (uMsg == WM_TIMER && wParam == 1001) {
        KillTimer(hWnd, 1001);
        SaveNotes();
        return 0;
    }

    LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_KEYUP || uMsg == WM_CHAR) {
        SetTimer(hWnd, 1001, 1000, NULL);
    } else if (uMsg == WM_KILLFOCUS) {
        SaveNotes();
    }

    return res;
}

LRESULT CALLBACK NoteWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    NoteData* note = GetNoteByHwnd(hwnd);
    
    switch (msg) {
        case WM_NCHITTEST: {
            RECT rc; GetClientRect(hwnd, &rc);
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            ScreenToClient(hwnd, &pt);
            int border = 10;
            if (pt.x > rc.right - border && pt.y > rc.bottom - border) return HTBOTTOMRIGHT;
            if (pt.x < border && pt.y < border) return HTTOPLEFT;
            if (pt.x > rc.right - border && pt.y < border) return HTTOPRIGHT;
            if (pt.x < border && pt.y > rc.bottom - border) return HTBOTTOMLEFT;
            if (pt.y > rc.bottom - border) return HTBOTTOM;
            if (pt.y < border) return HTTOP;
            if (pt.x > rc.right - border) return HTRIGHT;
            if (pt.x < border) return HTLEFT;
            return HTCAPTION; // Allow drag anywhere else
        }
        case WM_SIZE:
        case WM_MOVE: {
            if (note) {
                GetWindowRect(hwnd, &note->rect);
                SetWindowPos(note->hwndEdit, NULL, 15, 15, note->rect.right - note->rect.left - 30, note->rect.bottom - note->rect.top - 30, SWP_NOZORDER);
                
                HRGN hrgn = CreateRoundRectRgn(0, 0, note->rect.right - note->rect.left, note->rect.bottom - note->rect.top, 24, 24);
                SetWindowRgn(hwnd, hrgn, TRUE);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }
        case WM_EXITSIZEMOVE: {
            SaveNotes();
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            if (note) {
                HBRUSH br = CreateSolidBrush(note->color);
                FillRect(hdc, &rc, br);
                DeleteObject(br);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: {
            HBRUSH hbr = (HBRUSH)GetProp(hwnd, L"NoteBgBrush");
            if (hbr) {
                DeleteObject(hbr);
                RemoveProp(hwnd, L"NoteBgBrush");
            }
            RemoveProp(hwnd, L"NoteBgBrushColor");
            break;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            if (note) {
                SetTextColor((HDC)wParam, RGB(255, 255, 255));
                SetBkColor((HDC)wParam, note->color);
                HBRUSH hbr = (HBRUSH)GetProp(hwnd, L"NoteBgBrush");
                COLORREF lastColor = (COLORREF)(UINT_PTR)GetProp(hwnd, L"NoteBgBrushColor");
                if (!hbr || lastColor != note->color) {
                    if (hbr) {
                        DeleteObject(hbr);
                    }
                    hbr = CreateSolidBrush(note->color);
                    SetProp(hwnd, L"NoteBgBrush", hbr);
                    SetProp(hwnd, L"NoteBgBrushColor", (HANDLE)(UINT_PTR)note->color);
                }
                return (LRESULT)hbr;
            }
            break;
        }
        case WM_RBUTTONUP: {
            if (!note) break;
            
            SetWindowTheme(hwnd, L"DarkMode_Explorer", NULL);
            
            HMENU hMenu = CreatePopupMenu();
            
            AppendMenu(hMenu, MF_STRING, 41, L"✂️ Cut");
            AppendMenu(hMenu, MF_STRING, 42, L"📋 Copy");
            AppendMenu(hMenu, MF_STRING, 43, L"📋 Paste");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, 44, L"🔠 Select All");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            
            AppendMenu(hMenu, MF_STRING | (note->pinned ? MF_CHECKED : 0), 1, L"📌 Pin Always on Top");
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            HMENU hColorMenu = CreatePopupMenu();
            AppendMenu(hColorMenu, MF_STRING, 11, L"Purple");
            AppendMenu(hColorMenu, MF_STRING, 12, L"Blue");
            AppendMenu(hColorMenu, MF_STRING, 13, L"Green");
            AppendMenu(hColorMenu, MF_STRING, 14, L"Red");
            AppendMenu(hColorMenu, MF_STRING, 15, L"Orange");
            AppendMenu(hColorMenu, MF_STRING, 16, L"Amber");
            AppendMenu(hColorMenu, MF_STRING, 17, L"Teal");
            AppendMenu(hColorMenu, MF_STRING, 18, L"Pink");
            AppendMenu(hColorMenu, MF_STRING, 19, L"Dark");
            AppendMenu(hColorMenu, MF_STRING, 20, L"Gray");
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hColorMenu, L"🎨 Color");
            
            HMENU hOpacityMenu = CreatePopupMenu();
            AppendMenu(hOpacityMenu, MF_STRING, 21, L"100%");
            AppendMenu(hOpacityMenu, MF_STRING, 22, L"90%");
            AppendMenu(hOpacityMenu, MF_STRING, 23, L"80%");
            AppendMenu(hOpacityMenu, MF_STRING, 24, L"70%");
            AppendMenu(hOpacityMenu, MF_STRING, 25, L"60%");
            AppendMenu(hOpacityMenu, MF_STRING, 26, L"50%");
            AppendMenu(hOpacityMenu, MF_STRING, 27, L"40%");
            AppendMenu(hOpacityMenu, MF_STRING, 28, L"30%");
            AppendMenu(hOpacityMenu, MF_STRING, 29, L"20%");
            AppendMenu(hOpacityMenu, MF_STRING, 30, L"10%");
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hOpacityMenu, L"👁️ Opacity");
            
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_STRING, 2, L"❌ Delete Note");

            POINT pt; GetCursorPos(&pt);
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);

            if (cmd == 1) {
                note->pinned = !note->pinned;
                SetWindowPos(hwnd, note->pinned ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            } else if (cmd >= 11 && cmd <= 20) {
                if (cmd == 11) note->color = RGB(138, 43, 226);
                else if (cmd == 12) note->color = RGB(0, 120, 215);
                else if (cmd == 13) note->color = RGB(16, 124, 65);
                else if (cmd == 14) note->color = RGB(192, 57, 43);
                else if (cmd == 15) note->color = RGB(211, 84, 0);
                else if (cmd == 16) note->color = RGB(220, 165, 0);
                else if (cmd == 17) note->color = RGB(22, 160, 133);
                else if (cmd == 18) note->color = RGB(199, 21, 133);
                else if (cmd == 19) note->color = RGB(43, 43, 43);
                else if (cmd == 20) note->color = RGB(100, 110, 120);
                g_lastNoteColor = note->color;
                InvalidateRect(hwnd, NULL, TRUE);
                if (note->hwndEdit) SendMessage(note->hwndEdit, EM_SETBKGNDCOLOR, 0, note->color);
            } else if (cmd >= 21 && cmd <= 30) {
                int opVal = 255;
                if (cmd == 21) opVal = 255;
                else if (cmd == 22) opVal = 230;
                else if (cmd == 23) opVal = 204;
                else if (cmd == 24) opVal = 178;
                else if (cmd == 25) opVal = 153;
                else if (cmd == 26) opVal = 127;
                else if (cmd == 27) opVal = 102;
                else if (cmd == 28) opVal = 76;
                else if (cmd == 29) opVal = 51;
                else if (cmd == 30) opVal = 25;
                note->opacity = opVal;
                g_lastNoteOpacity = note->opacity;
                SetLayeredWindowAttributes(hwnd, 0, note->opacity, LWA_ALPHA);
            } else if (cmd == 41) {
                SendMessage(note->hwndEdit, WM_CUT, 0, 0);
            } else if (cmd == 42) {
                SendMessage(note->hwndEdit, WM_COPY, 0, 0);
            } else if (cmd == 43) {
                SendMessage(note->hwndEdit, WM_PASTE, 0, 0);
            } else if (cmd == 44) {
                SendMessage(note->hwndEdit, EM_SETSEL, 0, -1);
            } else if (cmd == 2) {
                DeleteNote(note->id);
            }
            SaveNotes();
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void SanityCheckNoteRect(RECT* rc) {
    int w = rc->right - rc->left;
    int h = rc->bottom - rc->top;
    if (w < 100) w = 300;
    if (h < 100) h = 300;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    if (rc->left < -10000 || rc->top < -10000 || rc->left > 10000 || rc->top > 10000 ||
        rc->right < rc->left || rc->bottom < rc->top) {
        rc->left = (screenW - w) / 2;
        rc->top = (screenH - h) / 2;
        rc->right = rc->left + w;
        rc->bottom = rc->top + h;
    }
}

void CreateNoteWindow(NoteData* n) {
    if (n->hwnd) return;
    
    SanityCheckNoteRect(&n->rect);
    
    n->hwnd = CreateWindowEx(
        WS_EX_LAYERED | (n->pinned ? WS_EX_TOPMOST : 0) | WS_EX_TOOLWINDOW,
        NOTE_CLASS, L"Sticky Note",
        WS_POPUP | WS_VISIBLE,
        n->rect.left, n->rect.top, n->rect.right - n->rect.left, n->rect.bottom - n->rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    SetLayeredWindowAttributes(n->hwnd, 0, n->opacity, LWA_ALPHA);
    HRGN hrgn = CreateRoundRectRgn(0, 0, n->rect.right - n->rect.left, n->rect.bottom - n->rect.top, 24, 24);
    SetWindowRgn(n->hwnd, hrgn, TRUE);

    n->hwndEdit = CreateWindowEx(
        0, L"RICHEDIT50W", L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN,
        15, 15, n->rect.right - n->rect.left - 30, n->rect.bottom - n->rect.top - 30,
        n->hwnd, NULL, GetModuleHandle(NULL), NULL);

    SendMessage(n->hwndEdit, EM_SETBKGNDCOLOR, 0, n->color);
    SetWindowSubclass(n->hwndEdit, NoteEditSubclassProc, 1, 0);

    // Font
    HFONT hFont = CreateFont(22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
                             DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(n->hwndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Set Text
    if (n->text[0] != L'\0') {
        SendMessage(n->hwndEdit, WM_SETTEXT, 0, (LPARAM)n->text);
    }
}

void LoadNotes() {
    size_t maxSize = sizeof(int) * 2 + sizeof(NoteData) * MAX_NOTES + sizeof(DWORD) + sizeof(int);
    BYTE* buffer = (BYTE*)malloc(maxSize);
    if (!buffer) return;
    size_t bytesRead = Wh_GetBinaryValue(L"NotesData", buffer, maxSize);
    if (bytesRead >= sizeof(int) * 2) {
        BYTE* p = buffer;
        memcpy(&g_noteCount, p, sizeof(int)); p += sizeof(int);
        memcpy(&g_noteIdCounter, p, sizeof(int)); p += sizeof(int);
        if (g_noteCount > MAX_NOTES) g_noteCount = MAX_NOTES;
        size_t notesSize = sizeof(NoteData) * g_noteCount;
        if (bytesRead >= sizeof(int) * 2 + notesSize) {
            for (int i = 0; i < g_noteCount; i++) {
                memcpy(&g_notes[i], p, sizeof(NoteData)); p += sizeof(NoteData);
                g_notes[i].hwnd = nullptr;
                g_notes[i].hwndEdit = nullptr;
            }
            if (bytesRead >= sizeof(int) * 2 + notesSize + sizeof(DWORD)) {
                memcpy(&g_lastNoteColor, p, sizeof(DWORD)); p += sizeof(DWORD);
            } else {
                g_lastNoteColor = RGB(43, 43, 43);
            }
            if (bytesRead >= sizeof(int) * 2 + notesSize + sizeof(DWORD) + sizeof(int)) {
                memcpy(&g_lastNoteOpacity, p, sizeof(int)); p += sizeof(int);
            } else {
                g_lastNoteOpacity = 255;
            }
        } else {
            g_noteCount = 0;
        }
    }
    free(buffer);
}


LRESULT CALLBACK MainMsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_USER + 105) {
        int x = (short)LOWORD(wParam);
        int y = (short)HIWORD(wParam);
        int w = (short)LOWORD(lParam);
        int h = (short)HIWORD(lParam);

        if (g_noteCount < MAX_NOTES) {
            NoteData* n = &g_notes[g_noteCount++];
            n->id = ++g_noteIdCounter;
            n->rect = { x, y, x + w, y + h };
            n->color = g_lastNoteColor;
            n->opacity = g_lastNoteOpacity;
            n->pinned = FALSE;
            n->text[0] = L'\0';
            n->hwnd = nullptr;
            n->hwndEdit = nullptr;
            SaveNotes();
            CreateNoteWindow(n);
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI RunNotesProcess(LPVOID lpParam) {
    OleInitialize(NULL);

    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = NoteWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = NOTE_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    WNDCLASS mwc = {0};
    mwc.lpfnWndProc = MainMsgWndProc;
    mwc.hInstance = GetModuleHandle(NULL);
    mwc.lpszClassName = MAIN_MSG_WND_CLASS;
    RegisterClass(&mwc);

    g_mainMsgWnd = CreateWindowEx(0, MAIN_MSG_WND_CLASS, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, GetModuleHandle(NULL), NULL);

    LoadLibraryExW(L"Msftedit.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    LoadNotes();
    for (int i = 0; i < g_noteCount; i++) {
        CreateNoteWindow(&g_notes[i]);
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SaveNotes();

    for (int i = 0; i < g_noteCount; i++) {
        if (g_notes[i].hwnd) {
            DestroyWindow(g_notes[i].hwnd);
        }
    }

    if (g_mainMsgWnd) {
        DestroyWindow(g_mainMsgWnd);
    }

    UnregisterClass(NOTE_CLASS, GetModuleHandle(NULL));
    UnregisterClass(MAIN_MSG_WND_CLASS, GetModuleHandle(NULL));

    Gdiplus::GdiplusShutdown(gdiplusToken);
    OleUninitialize();
    return 0;
}

void LaunchNotesProcess() {
    if (g_notesThread) return;
    g_notesThread = CreateThread(NULL, 0, RunNotesProcess, NULL, 0, &g_notesThreadId);
}

HWND g_gestureTarget = nullptr;
HWND g_spotlightWnd = nullptr;
POINT g_spotlightCurrentPt = {0,0};
float g_spotlightAlpha = 0.0f;
float g_spotlightRadius = 150.0f;
std::atomic<int> g_spotlightState{0}; 

HDC g_spotlightHdcMem = NULL;
HBITMAP g_spotlightHBitmap = NULL;
HBITMAP g_spotlightOldBitmap = NULL;
DWORD* g_spotlightBits = nullptr;

void CleanupSpotlightGraphics() {
    if (g_spotlightHdcMem) {
        SelectObject(g_spotlightHdcMem, g_spotlightOldBitmap);
        DeleteObject(g_spotlightHBitmap);
        DeleteDC(g_spotlightHdcMem);
        g_spotlightHdcMem = NULL;
        g_spotlightBits = nullptr;
    }
}

void InitSpotlightGraphics(HWND hwnd) {
    if (g_spotlightHdcMem) return;
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hdcScreen = GetDC(NULL);
    g_spotlightHdcMem = CreateCompatibleDC(hdcScreen);
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = vw;
    bmi.bmiHeader.biHeight = -vh;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    g_spotlightHBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, (void**)&g_spotlightBits, NULL, 0);
    g_spotlightOldBitmap = (HBITMAP)SelectObject(g_spotlightHdcMem, g_spotlightHBitmap);
    
    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptSrc = {0, 0};
    SIZE sizeWnd = {vw, vh};
    POINT ptDst = { GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN) };
    
    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sizeWnd, g_spotlightHdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
}

LRESULT CALLBACK SpotlightProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER) {
        if (wParam == 1) { 
            POINT pt;
            GetCursorPos(&pt);
            
            g_spotlightCurrentPt.x += (pt.x - g_spotlightCurrentPt.x) * 0.5f;
            g_spotlightCurrentPt.y += (pt.y - g_spotlightCurrentPt.y) * 0.5f;
            
            if (g_spotlightState == 1) { 
                g_spotlightAlpha += 0.1f;
                if (g_spotlightAlpha >= 1.0f) {
                    g_spotlightAlpha = 1.0f;
                    g_spotlightState = 2; 
                }
            } else if (g_spotlightState == 3) { 
                g_spotlightAlpha -= 0.1f;
                if (g_spotlightAlpha <= 0.0f) {
                    g_spotlightAlpha = 0.0f;
                    g_spotlightState = 0;
                    KillTimer(hwnd, 1);
                    ShowWindow(hwnd, SW_HIDE);
                    CleanupSpotlightGraphics();
                    return 0;
                }
            }

            if (g_spotlightBits) {
                int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                int total = vw * vh;
                DWORD bgAlpha = (DWORD)(160.0f * g_spotlightAlpha);
                DWORD bgPixel = (bgAlpha << 24);
                
                // Fast background fill
                for (int i = 0; i < total; i++) g_spotlightBits[i] = bgPixel;

                // Punch hole
                int cx = (int)g_spotlightCurrentPt.x - GetSystemMetrics(SM_XVIRTUALSCREEN);
                int cy = (int)g_spotlightCurrentPt.y - GetSystemMetrics(SM_YVIRTUALSCREEN);
                int r = g_spotlightRadius;
                int r2 = r * r;
                
                for (int y = cy - r; y <= cy + r; y++) {
                    if (y < 0 || y >= vh) continue;
                    for (int x = cx - r; x <= cx + r; x++) {
                        if (x < 0 || x >= vw) continue;
                        int dx = x - cx;
                        int dy = y - cy;
                        int dist2 = dx*dx + dy*dy;
                        if (dist2 <= r2) {
                            float dist = sqrtf((float)dist2);
                            float ratio = dist / (float)r;
                            DWORD a = 0;
                            if (ratio > 0.8f) {
                                a = (DWORD)(bgAlpha * ((ratio - 0.8f) / 0.2f));
                            }
                            g_spotlightBits[y * vw + x] = (a << 24);
                        }
                    }
                }

                HDC hdcScreen = GetDC(NULL);
                POINT ptDst = { GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN) };
                SIZE sizeWnd = { vw, vh };
                POINT ptSrc = { 0, 0 };
                BLENDFUNCTION blend = {0};
                blend.BlendOp = AC_SRC_OVER;
                blend.SourceConstantAlpha = 255;
                blend.AlphaFormat = AC_SRC_ALPHA;
                
                UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sizeWnd, g_spotlightHdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
                ReleaseDC(NULL, hdcScreen);
            }
        }
    } else if (msg == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void StartSpotlight() {
    if (!g_spotlightWnd) {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = SpotlightProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"MagicMouseSpotlight";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClass(&wc);

        g_spotlightWnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
            L"MagicMouseSpotlight", L"",
            WS_POPUP,
            0, 0, 0, 0,
            NULL, NULL, GetModuleHandle(NULL), NULL
        );
    }
    
    GetCursorPos(&g_spotlightCurrentPt);
    g_spotlightAlpha = 0.0f;
    g_spotlightState = 1;
    
    InitSpotlightGraphics(g_spotlightWnd);

    ShowWindow(g_spotlightWnd, SW_SHOWNOACTIVATE);
    SetTimer(g_spotlightWnd, 1, 16, NULL); 
}

static BOOL g_startNoteOnTop = FALSE;

void StartNoteCreation(BOOL onTop) {
    if (g_noteCreationMode || g_noteCount >= MAX_NOTES) return;
    g_noteCreationMode = TRUE;
    g_startNoteOnTop = onTop;
}

LRESULT CALLBACK NoteSelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) {
        return 1;
    }
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        
        // Fill black (colorkey for transparency)
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rc, blackBrush);
        DeleteObject(blackBrush);

        using namespace Gdiplus;
        Graphics graphics(hdc);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);

        int w = rc.right;
        int h = rc.bottom;
        if (w > 0 && h > 0) {
            SolidBrush fillBrush(Color(255, 43, 43, 43)); // Dark Slate, opacity handled by LWA_ALPHA
            Pen borderPen(Color(255, 200, 100, 255), 3.0f);
            
            GraphicsPath path;
            int r = (w > 24 && h > 24) ? 12 : ((w > h ? h : w) / 2);
            if (r > 0) {
                path.AddArc(0, 0, r*2, r*2, 180, 90);
                path.AddArc(w - r*2, 0, r*2, r*2, 270, 90);
                path.AddArc(w - r*2, h - r*2, r*2, r*2, 0, 90);
                path.AddArc(0, h - r*2, r*2, r*2, 90, 90);
                path.CloseFigure();

                graphics.FillPath(&fillBrush, &path);
                graphics.DrawPath(&borderPen, &path);
            }
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


DWORD WINAPI EmptyRecycleBinThreadProc(LPVOID lpParam) {
    SHEmptyRecycleBinW(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
    return 0;
}

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
};
struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};
enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
};
struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

void EnableAcrylicBlur(HWND hwnd) {
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
        pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) {
            ACCENT_POLICY policy = {};
            policy.AccentState = ACCENT_ENABLE_BLURBEHIND; // Respects per-pixel alpha on UpdateLayeredWindow

            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
    }
}

// ============================================================================
// STOPWATCH
// ============================================================================

static const int SW_PILL_W = 180;
static const int SW_PILL_H = 46;
static const int SW_BTN_RADIUS = 14;

void PaintStopwatchPill(HWND hwnd) {
    int w = SW_PILL_W;
    int h = SW_PILL_H;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    DWORD* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
    HBITMAP oldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

    Gdiplus::Graphics gfx(hdcMem);
    gfx.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    gfx.Clear(Gdiplus::Color(0, 0, 0, 0));

    Gdiplus::GraphicsPath pillPath;
    int radius = h / 2;
    pillPath.AddArc(0, 0, h, h, 90, 180);
    pillPath.AddArc(w - h, 0, h, h, 270, 180);
    pillPath.CloseFigure();

    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(210, 20, 20, 30));
    gfx.FillPath(&bgBrush, &pillPath);



    double elapsedSec = 0.0;
    if (g_stopwatchRunning) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        elapsedSec = (double)(g_stopwatchAccum.QuadPart + (now.QuadPart - g_stopwatchStart.QuadPart)) / (double)g_perfFreq.QuadPart;
    } else {
        elapsedSec = (double)g_stopwatchAccum.QuadPart / (double)g_perfFreq.QuadPart;
    }
    if (elapsedSec < 0) elapsedSec = 0;

    int mins = (int)(elapsedSec / 60.0);
    int secs = (int)fmod(elapsedSec, 60.0);
    int tenths = (int)(fmod(elapsedSec * 10.0, 10.0));

    wchar_t timeStr[32];
    swprintf_s(timeStr, L"%02d:%02d.%d", mins, secs, tenths);

    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 240, 240, 245));

    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::RectF textRect(0, 0, (float)(w - 46), (float)h);
    gfx.DrawString(timeStr, -1, &font, textRect, &fmt, &textBrush);

    int btnCx = w - radius - 2;
    int btnCy = h / 2;
    int btnR = SW_BTN_RADIUS;

    if (g_stopwatchRunning) {
        Gdiplus::SolidBrush btnBrush(Gdiplus::Color(220, 220, 60, 60));
        gfx.FillEllipse(&btnBrush, btnCx - btnR, btnCy - btnR, btnR * 2, btnR * 2);
        Gdiplus::Pen btnBorder(Gdiplus::Color(120, 255, 255, 255), 1.5f);
        gfx.DrawEllipse(&btnBorder, btnCx - btnR, btnCy - btnR, btnR * 2, btnR * 2);

        Gdiplus::SolidBrush iconBrush(Gdiplus::Color(255, 255, 255, 255));
        int pauseW = 5, pauseH = 16, pauseGap = 6;
        gfx.FillRectangle(&iconBrush, btnCx - pauseGap/2 - pauseW, btnCy - pauseH/2, pauseW, pauseH);
        gfx.FillRectangle(&iconBrush, btnCx + pauseGap/2, btnCy - pauseH/2, pauseW, pauseH);
    } else {
        Gdiplus::SolidBrush btnBrush(Gdiplus::Color(220, 40, 180, 80));
        gfx.FillEllipse(&btnBrush, btnCx - btnR, btnCy - btnR, btnR * 2, btnR * 2);
        Gdiplus::Pen btnBorder(Gdiplus::Color(120, 255, 255, 255), 1.5f);
        gfx.DrawEllipse(&btnBorder, btnCx - btnR, btnCy - btnR, btnR * 2, btnR * 2);

        Gdiplus::Point tri[3];
        tri[0] = Gdiplus::Point(btnCx - 6, btnCy - 10);
        tri[1] = Gdiplus::Point(btnCx - 6, btnCy + 10);
        tri[2] = Gdiplus::Point(btnCx + 10, btnCy);
        Gdiplus::SolidBrush iconBrush(Gdiplus::Color(255, 255, 255, 255));
        gfx.FillPolygon(&iconBrush, tri, 3);
    }

    gfx.Flush();

    POINT ptDst;
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);
    ptDst.x = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
    ptDst.y = mi.rcWork.top + 30;

    SIZE sizeWnd = { w, h };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

LRESULT CALLBACK StopwatchProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == 1) {
        PaintStopwatchPill(hwnd);
        return 0;
    }
    if (msg == WM_LBUTTONDOWN) {
        int xClick = GET_X_LPARAM(lParam);
        int btnCx = SW_PILL_W - SW_PILL_H / 2 - 2;
        int btnCy = SW_PILL_H / 2;
        int dx = xClick - btnCx;
        int dy = GET_Y_LPARAM(lParam) - btnCy;
        if (dx * dx + dy * dy <= SW_BTN_RADIUS * SW_BTN_RADIUS) {
            if (g_stopwatchRunning) {
                LARGE_INTEGER now;
                QueryPerformanceCounter(&now);
                g_stopwatchAccum.QuadPart += (now.QuadPart - g_stopwatchStart.QuadPart);
                g_stopwatchRunning = FALSE;
            } else {
                QueryPerformanceCounter(&g_stopwatchStart);
                g_stopwatchRunning = TRUE;
            }
            PaintStopwatchPill(hwnd);
        }
        return 0;
    }
    if (msg == WM_RBUTTONDOWN) {
        ToggleStopwatch();
        return 0;
    }
    if (msg == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ToggleStopwatch() {
    if (g_stopwatchWnd && IsWindowVisible(g_stopwatchWnd)) {
        KillTimer(g_stopwatchWnd, 1);
        ShowWindow(g_stopwatchWnd, SW_HIDE);
        g_stopwatchRunning = FALSE;
        g_stopwatchAccum.QuadPart = 0;
        return;
    }

    QueryPerformanceFrequency(&g_perfFreq);
    g_stopwatchAccum.QuadPart = 0;
    g_stopwatchRunning = FALSE;

    if (!g_stopwatchWnd) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = StopwatchProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = STOPWATCH_CLASS;
        wc.hCursor = LoadCursor(NULL, IDC_HAND);
        RegisterClass(&wc);

        g_stopwatchWnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            STOPWATCH_CLASS, L"",
            WS_POPUP,
            0, 0, SW_PILL_W, SW_PILL_H,
            NULL, NULL, GetModuleHandle(NULL), NULL);

    }

    PaintStopwatchPill(g_stopwatchWnd);
    ShowWindow(g_stopwatchWnd, SW_SHOWNOACTIVATE);
    SetTimer(g_stopwatchWnd, 1, 30, NULL);
}

// ============================================================================
// TIMER
// ============================================================================

static const int TM_PILL_W = 190;
static const int TM_PILL_H = 52;
static const int TM_PRESET_W = 44;
static const int TM_PRESET_H = 26;
static const int TM_PRESET_GAP = 8;

void FormatTimerPreset(int seconds, wchar_t* buf, int bufLen) {
    int m = seconds / 60;
    int s = seconds % 60;
    if (s > 0) swprintf_s(buf, bufLen, L"%dm%ds", m, s);
    else swprintf_s(buf, bufLen, L"%dm", m);
}

void PaintTimerPill(HWND hwnd) {
    int w = TM_PILL_W;
    int h = TM_PILL_H;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    DWORD* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
    HBITMAP oldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

    Gdiplus::Graphics gfx(hdcMem);
    gfx.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    gfx.Clear(Gdiplus::Color(0, 0, 0, 0));

    Gdiplus::GraphicsPath pillPath;
    pillPath.AddArc(0, 0, h, h, 90, 180);
    pillPath.AddArc(w - h, 0, h, h, 270, 180);
    pillPath.CloseFigure();

    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(210, 20, 20, 30));
    gfx.FillPath(&bgBrush, &pillPath);

    if (g_timerShakeState > 0) {
        float glowAlpha = (sinf(g_timerGlowPhase) * 0.5f + 0.5f) * 180.0f;
        Gdiplus::Pen glowPen(Gdiplus::Color((BYTE)glowAlpha, 255, 80, 40), 4.0f);
        gfx.DrawPath(&glowPen, &pillPath);
        Gdiplus::Pen glowPen2(Gdiplus::Color((BYTE)(glowAlpha * 0.4f), 255, 120, 60), 8.0f);
        gfx.DrawPath(&glowPen2, &pillPath);
    }



    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    if (g_timerSelectedPreset < 0 && !g_timerRunning && g_timerShakeState == 0) {
        int totalPresetsW = 3 * TM_PRESET_W + 2 * TM_PRESET_GAP;
        int startX = (w - totalPresetsW) / 2;
        int presetY = (h - TM_PRESET_H) / 2;

        for (int i = 0; i < 3; i++) {
            int px = startX + i * (TM_PRESET_W + TM_PRESET_GAP);

            Gdiplus::GraphicsPath presetPath;
            presetPath.AddArc(px, presetY, TM_PRESET_H, TM_PRESET_H, 90, 180);
            presetPath.AddArc(px + TM_PRESET_W - TM_PRESET_H, presetY, TM_PRESET_H, TM_PRESET_H, 270, 180);
            presetPath.CloseFigure();

            Gdiplus::SolidBrush presetBg(Gdiplus::Color(160, 50, 50, 65));
            gfx.FillPath(&presetBg, &presetPath);
            Gdiplus::Pen presetBorder(Gdiplus::Color(60, 200, 200, 220), 1.0f);
            gfx.DrawPath(&presetBorder, &presetPath);

            wchar_t label[16];
            if (g_timerEditMode && g_timerEditIndex == i) {
                wcsncpy_s(label, g_timerEditBuf, _TRUNCATE);
                wcscat_s(label, L"|");
            } else {
                FormatTimerPreset(g_timerPresets[i], label, 16);
            }

            Gdiplus::Font presetFont(&fontFamily, 14, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            Gdiplus::SolidBrush presetText(Gdiplus::Color(255, 220, 220, 230));
            Gdiplus::RectF presetRect((float)px, (float)presetY, (float)TM_PRESET_W, (float)TM_PRESET_H);
            gfx.DrawString(label, -1, &presetFont, presetRect, &fmt, &presetText);
        }
    } else {
        int totalSec = g_timerRemainingMs / 1000;
        if (totalSec < 0) totalSec = 0;
        int mins = totalSec / 60;
        int secs = totalSec % 60;

        wchar_t timeStr[32];
        swprintf_s(timeStr, L"%02d:%02d", mins, secs);

        float fontSize = 32.0f;
        if (g_timerShakeState > 0) {
            float pulse = sinf(g_timerGlowPhase * 2.0f) * 0.15f + 1.0f;
            fontSize *= pulse;
        }

        Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

        Gdiplus::Color textColor(255, 240, 240, 245);
        if (g_timerShakeState > 0) {
            float pulse = sinf(g_timerGlowPhase * 3.0f) * 0.5f + 0.5f;
            textColor = Gdiplus::Color(255, 255, (BYTE)(100 + 155 * pulse), (BYTE)(60 + 60 * pulse));
        }
        Gdiplus::SolidBrush textBrush(textColor);
        Gdiplus::RectF textRect(0, 0, (float)w, (float)h);
        gfx.DrawString(timeStr, -1, &font, textRect, &fmt, &textBrush);
    }

    gfx.Flush();

    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);
    POINT ptDst;
    ptDst.x = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
    ptDst.y = mi.rcWork.top + 30;

    if (g_timerShakeState > 0) {
        DWORD elapsed = GetTickCount() - g_timerShakeStart;
        if (elapsed < 3000) {
            ptDst.x += (rand() % 11) - 5;
            ptDst.y += (rand() % 7) - 3;
        } else {
            g_timerShakeState = 0;
        }
    }

    SIZE sizeWnd = { w, h };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);


    SelectObject(hdcMem, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

LRESULT CALLBACK TimerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == 1) {
        if (g_timerRunning) {
            DWORD now = GetTickCount();
            int delta = (int)(now - g_timerLastTick);
            g_timerLastTick = now;
            g_timerRemainingMs -= delta;
            if (g_timerRemainingMs <= 0) {
                g_timerRemainingMs = 0;
                g_timerRunning = FALSE;
                g_timerShakeState = 1;
                g_timerShakeStart = GetTickCount();
                g_timerGlowPhase = 0.0f;
            }
        }
        if (g_timerShakeState > 0) {
            g_timerGlowPhase += 0.25f;
        }
        PaintTimerPill(hwnd);
        return 0;
    }
    if (msg == WM_LBUTTONDOWN) {
        if (g_timerEditMode) {
            g_timerEditMode = FALSE;
            int val = _wtoi(g_timerEditBuf);
            if (val > 0 && val <= 999 && g_timerEditIndex >= 0 && g_timerEditIndex < 3) {
                g_timerPresets[g_timerEditIndex] = val * 60;
            }
            g_timerEditIndex = -1;
            PaintTimerPill(hwnd);
            return 0;
        }

        if (g_timerShakeState > 0) {
            g_timerShakeState = 0;
            g_timerSelectedPreset = -1;
            PaintTimerPill(hwnd);
            return 0;
        }

        if (g_timerSelectedPreset < 0 && !g_timerRunning) {
            int xClick = GET_X_LPARAM(lParam);
            int yClick = GET_Y_LPARAM(lParam);
            int totalPresetsW = 3 * TM_PRESET_W + 2 * TM_PRESET_GAP;
            int startX = (TM_PILL_W - totalPresetsW) / 2;
            int presetY = (TM_PILL_H - TM_PRESET_H) / 2;

            for (int i = 0; i < 3; i++) {
                int px = startX + i * (TM_PRESET_W + TM_PRESET_GAP);
                if (xClick >= px && xClick <= px + TM_PRESET_W &&
                    yClick >= presetY && yClick <= presetY + TM_PRESET_H) {
                    g_timerSelectedPreset = i;
                    g_timerRemainingMs = g_timerPresets[i] * 1000;
                    g_timerLastTick = GetTickCount();
                    g_timerRunning = TRUE;
                    PaintTimerPill(hwnd);
                    return 0;
                }
            }
        }
        return 0;
    }
    if (msg == WM_RBUTTONDOWN) {
        if (g_timerSelectedPreset < 0 && !g_timerRunning && g_timerShakeState == 0) {
            int xClick = GET_X_LPARAM(lParam);
            int yClick = GET_Y_LPARAM(lParam);
            int totalPresetsW = 3 * TM_PRESET_W + 2 * TM_PRESET_GAP;
            int startX = (TM_PILL_W - totalPresetsW) / 2;
            int presetY = (TM_PILL_H - TM_PRESET_H) / 2;

            for (int i = 0; i < 3; i++) {
                int px = startX + i * (TM_PRESET_W + TM_PRESET_GAP);
                if (xClick >= px && xClick <= px + TM_PRESET_W &&
                    yClick >= presetY && yClick <= presetY + TM_PRESET_H) {
                    g_timerEditMode = TRUE;
                    g_timerEditIndex = i;
                    swprintf_s(g_timerEditBuf, L"%d", g_timerPresets[i] / 60);
                    PaintTimerPill(hwnd);
                    return 0;
                }
            }
        }
        ToggleTimer();
        return 0;
    }

    if (msg == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ToggleTimer() {
    if (g_timerWnd && IsWindowVisible(g_timerWnd)) {
        KillTimer(g_timerWnd, 1);
        ShowWindow(g_timerWnd, SW_HIDE);
        g_timerRunning = FALSE;
        g_timerSelectedPreset = -1;
        g_timerShakeState = 0;
        g_timerEditMode = FALSE;
        return;
    }

    g_timerRunning = FALSE;
    g_timerSelectedPreset = -1;
    g_timerShakeState = 0;
    g_timerRemainingMs = 0;
    g_timerEditMode = FALSE;

    if (!g_timerWnd) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = TimerProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = TIMER_CLASS;
        wc.hCursor = LoadCursor(NULL, IDC_HAND);
        RegisterClass(&wc);

        g_timerWnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            TIMER_CLASS, L"",
            WS_POPUP,
            0, 0, TM_PILL_W, TM_PILL_H,
            NULL, NULL, GetModuleHandle(NULL), NULL);

    }

    PaintTimerPill(g_timerWnd);
    ShowWindow(g_timerWnd, SW_SHOWNOACTIVATE);
    SetTimer(g_timerWnd, 1, 30, NULL);
}

// ============================================================================
// FLASH (Flashlight)
// ============================================================================

struct EnumData {
    MonitorBrightnessInfo* infos;
    int* count;
};

static BOOL CALLBACK SaveMonitorBrightnessCallback(HMONITOR hMon, HDC, LPRECT, LPARAM lp) {
    EnumData* d = (EnumData*)lp;
    if (*d->count >= MAX_FLASH_MONITORS) return FALSE;

    MonitorBrightnessInfo& info = d->infos[*d->count];
    info.hMon = hMon;
    info.valid = FALSE;
    info.opened = FALSE;

    DWORD numPhysical = 0;
    PHYSICAL_MONITOR physMon[1] = {};
    if (GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &numPhysical) && numPhysical > 0) {
        if (GetPhysicalMonitorsFromHMONITOR(hMon, 1, physMon)) {
            info.hPhysical = physMon[0].hPhysicalMonitor;
            info.opened = TRUE;
            DWORD minB = 0, curB = 0, maxB = 0;
            if (GetMonitorBrightness(info.hPhysical, &minB, &curB, &maxB)) {
                info.originalBrightness = curB;
                info.valid = TRUE;
                SetMonitorBrightness(info.hPhysical, maxB);
            }
        }
    }
    (*d->count)++;
    return TRUE;
}

void SaveMonitorBrightness() {
    g_flashMonitorCount = 0;
    EnumData data = { g_flashMonitors, &g_flashMonitorCount };
    EnumDisplayMonitors(NULL, NULL, SaveMonitorBrightnessCallback, (LPARAM)&data);
}

void RestoreMonitorBrightness() {
    for (int i = 0; i < g_flashMonitorCount; i++) {
        if (g_flashMonitors[i].valid) {
            SetMonitorBrightness(g_flashMonitors[i].hPhysical, g_flashMonitors[i].originalBrightness);
        }
        if (g_flashMonitors[i].opened) {
            DestroyPhysicalMonitor(g_flashMonitors[i].hPhysical);
        }
    }
    g_flashMonitorCount = 0;
}

LRESULT CALLBACK FlashProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);

        HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &rc, whiteBrush);
        DeleteObject(whiteBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(180, 180, 180));
        HFONT font = CreateFont(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        HFONT oldFont = (HFONT)SelectObject(hdc, font);

        const wchar_t* label = L"Click any key to turn FLASH off";
        RECT textRect = rc;
        textRect.top = rc.bottom - 50;
        DrawText(hdc, label, -1, &textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

        SelectObject(hdc, oldFont);
        DeleteObject(font);

        EndPaint(hwnd, &ps);
        return 0;
    }
    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN ||
        msg == WM_XBUTTONDOWN || msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
        ToggleFlash();
        return 0;
    }
    if (msg == WM_ERASEBKGND) {
        return 1;
    }
    if (msg == WM_SETCURSOR) {
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return TRUE;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ToggleFlash() {
    if (g_flashWnd && IsWindowVisible(g_flashWnd)) {
        ShowWindow(g_flashWnd, SW_HIDE);
        RestoreMonitorBrightness();
        return;
    }

    SaveMonitorBrightness();

    if (!g_flashWnd) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = FlashProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = FLASH_CLASS;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        RegisterClass(&wc);

        int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        g_flashWnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            FLASH_CLASS, L"",
            WS_POPUP,
            vx, vy, vw, vh,
            NULL, NULL, GetModuleHandle(NULL), NULL);
    } else {
        int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        SetWindowPos(g_flashWnd, HWND_TOPMOST, vx, vy, vw, vh, SWP_NOACTIVATE);
    }

    ShowWindow(g_flashWnd, SW_SHOW);
    SetForegroundWindow(g_flashWnd);
    SetFocus(g_flashWnd);
}

void ExecuteAction(int gestureIdx, HWND target) {
    if (gestureIdx < 0 || gestureIdx >= g_settings.gestureCount) return;

    GestureConfig* gc = &g_settings.gestures[gestureIdx];

    GestureAction finalAction = gc->action;
    const wchar_t* finalParam = gc->actionParam;

    if (gc->contextRuleCount > 0) {
        wchar_t procName[128] = {0};
        GetForegroundProcessName(target, procName, 128);
        if (procName[0] != L'\0') {
            for (int i = 0; i < gc->contextRuleCount; i++) {
                if (wcsstr(procName, gc->contextRules[i].processMatch) != nullptr) {
                    finalAction = gc->contextRules[i].overrideAction;
                    finalParam = gc->contextRules[i].overrideParam;
                    Wh_Log(L"Context rule matched '%s', overriding action", gc->contextRules[i].processMatch);
                    break;
                }
            }
        }
    }

    Wh_Log(L"Executing gesture: %s (action=%d)", gc->name, finalAction);

    // Force release modifier keys that are currently pressed
    INPUT modUp[6] = {};
    int releasedKeys[6] = {};
    int kCount = 0;
    auto addKeyUp = [&](int vk) {
        if (GetAsyncKeyState(vk) & 0x8000) {
            modUp[kCount].type = INPUT_KEYBOARD;
            modUp[kCount].ki.wVk = vk;
            modUp[kCount].ki.dwFlags = KEYEVENTF_KEYUP;
            releasedKeys[kCount] = vk;
            kCount++;
        }
    };
    addKeyUp(VK_LCONTROL); addKeyUp(VK_RCONTROL);
    addKeyUp(VK_LSHIFT);   addKeyUp(VK_RSHIFT);
    addKeyUp(VK_LMENU);    addKeyUp(VK_RMENU);
    if (kCount > 0) SendInput(kCount, modUp, sizeof(INPUT));

    switch (finalAction) {
        case ACTION_LAUNCH:
            if (finalParam[0]) {
                wchar_t expanded[MAX_PATH];
                ExpandEnvironmentStrings(finalParam, expanded, MAX_PATH);
                
                std::wstring pathStr = expanded;
                if (pathStr.size() >= 2 && pathStr.front() == L'"' && pathStr.back() == L'"') {
                    pathStr = pathStr.substr(1, pathStr.size() - 2);
                }
                
                ShellExecute(NULL, L"open", pathStr.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
            break;

        case ACTION_KEYBOARD_SHORTCUT:
            if (finalParam[0]) {
                SendKeyboardShortcut(finalParam);
            }
            break;
            
        case ACTION_SHELL_COMMAND:
            if (finalParam[0]) {
                wchar_t expanded[MAX_PATH];
                ExpandEnvironmentStrings(finalParam, expanded, MAX_PATH);
                
                wchar_t comspec[MAX_PATH];
                if (!GetEnvironmentVariable(L"COMSPEC", comspec, MAX_PATH)) {
                    wcscpy_s(comspec, MAX_PATH, L"cmd.exe");
                }
                
                wchar_t cmdLine[MAX_PATH * 2 + 50];
                swprintf_s(cmdLine, MAX_PATH * 2 + 50, L"\"%s\" /k \"%s\"", comspec, expanded);
                
                wchar_t explorerPath[MAX_PATH] = {0};
                BOOL hasExplorerPath = GetExplorerPath(g_gestureTarget, explorerPath, MAX_PATH);
                if (!hasExplorerPath) {
                    hasExplorerPath = GetExplorerPath(GetForegroundWindow(), explorerPath, MAX_PATH);
                }
                
                STARTUPINFO si = { sizeof(si) };
                PROCESS_INFORMATION pi = {};
                if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, hasExplorerPath ? explorerPath : NULL, &si, &pi)) {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }
            }
            break;

        case ACTION_TASK_MANAGER:
            ShellExecute(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_SHOWNORMAL);
            break;

        case ACTION_SETTINGS:
            ShellExecute(NULL, L"open", L"ms-settings:", NULL, NULL, SW_SHOWNORMAL);
            break;

        case ACTION_MUTE_VOLUME:
        {
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_VOLUME_MUTE;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_VOLUME_MUTE;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, inputs, sizeof(INPUT));
            break;
        }

        case ACTION_PLAY_PAUSE:
        {
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_MEDIA_PLAY_PAUSE;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_MEDIA_PLAY_PAUSE;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, inputs, sizeof(INPUT));
            break;
        }

        case ACTION_DRAW:
            ToggleDrawMode();
            break;

        case ACTION_COLOR_PICKER:
            StartColorPicker();
            break;

        case ACTION_TOGGLE_DESKTOP_ICONS:
            ToggleDesktopIcons();
            break;

        case ACTION_FULLSCREEN:
            ToggleFullscreen(target);
            break;

        case ACTION_MAXIMIZE:
            if (target) {
                WINDOWPLACEMENT wp = { sizeof(wp) };
                if (GetWindowPlacement(target, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
                    DWORD_PTR dwResult;
                    SendMessageTimeout(target, WM_SYSCOMMAND, SC_RESTORE, 0, SMTO_ABORTIFHUNG, 500, &dwResult);
                } else {
                    DWORD_PTR dwResult;
                    SendMessageTimeout(target, WM_SYSCOMMAND, SC_MAXIMIZE, 0, SMTO_ABORTIFHUNG, 500, &dwResult);
                }
            }
            break;

        case ACTION_MINIMIZE:
            if (target) {
                DWORD_PTR dwResult;
                SendMessageTimeout(target, WM_SYSCOMMAND, SC_MINIMIZE, 0, SMTO_ABORTIFHUNG, 500, &dwResult);
            }
            break;

        case ACTION_CLOSE:
            if (target) {
                DWORD_PTR dwResult;
                SendMessageTimeout(target, WM_SYSCOMMAND, SC_CLOSE, 0, SMTO_ABORTIFHUNG, 500, &dwResult);
            }
            break;

        case ACTION_SHOW_DESKTOP:
            ShowDesktop();
            break;

        case ACTION_LOCK:
            ShellExecute(NULL, L"open", L"rundll32.exe", L"user32.dll,LockWorkStation", NULL, SW_SHOWNORMAL);
            break;

        case ACTION_SLEEP_MONITOR:
            Sleep(500);
            SendNotifyMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
            break;

        case ACTION_SLEEP:
            ShellExecute(NULL, L"open", L"rundll32.exe", L"powrprof.dll,SetSuspendState 0,1,0", NULL, SW_SHOWNORMAL);
            break;

        case ACTION_RESTART:
            ShellExecute(NULL, L"open", L"shutdown.exe", L"/r /t 0", NULL, SW_SHOWNORMAL);
            break;

        case ACTION_SHUTDOWN:
            ShellExecute(NULL, L"open", L"shutdown.exe", L"/s /t 0", NULL, SW_SHOWNORMAL);
            break;

        case ACTION_EMPTY_RECYCLE_BIN:
            if (HANDLE hThread = CreateThread(NULL, 0, EmptyRecycleBinThreadProc, NULL, 0, NULL)) {
                CloseHandle(hThread);
            }
            break;

        case ACTION_SNAP_LEFT:
        case ACTION_SNAP_RIGHT:
        case ACTION_SNAP_TOP:
        case ACTION_SNAP_BOTTOM:
        case ACTION_SNAP_TOP_LEFT:
        case ACTION_SNAP_TOP_RIGHT:
        case ACTION_SNAP_BOTTOM_LEFT:
        case ACTION_SNAP_BOTTOM_RIGHT:
        {
            if (!target) break;
            HMONITOR mon = MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            GetMonitorInfo(mon, &mi);

            WINDOWPLACEMENT wp = { sizeof(wp) };
            if (GetWindowPlacement(target, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
                DWORD_PTR dwResult;
                SendMessageTimeout(target, WM_SYSCOMMAND, SC_RESTORE, 0, SMTO_ABORTIFHUNG, 500, &dwResult);
            }

            RECT wA = mi.rcWork;
            int w = wA.right - wA.left;
            int h = wA.bottom - wA.top;
            RECT nr = wA;
            if (finalAction == ACTION_SNAP_LEFT) { nr.right = wA.left + w / 2; }
            if (finalAction == ACTION_SNAP_RIGHT) { nr.left = wA.left + w / 2; }
            if (finalAction == ACTION_SNAP_TOP) { nr.bottom = wA.top + h / 2; }
            if (finalAction == ACTION_SNAP_BOTTOM) { nr.top = wA.top + h / 2; }
            if (finalAction == ACTION_SNAP_TOP_LEFT) { nr.right = wA.left + w / 2; nr.bottom = wA.top + h / 2; }
            if (finalAction == ACTION_SNAP_TOP_RIGHT) { nr.left = wA.left + w / 2; nr.bottom = wA.top + h / 2; }
            if (finalAction == ACTION_SNAP_BOTTOM_LEFT) { nr.right = wA.left + w / 2; nr.top = wA.top + h / 2; }
            if (finalAction == ACTION_SNAP_BOTTOM_RIGHT) { nr.left = wA.left + w / 2; nr.top = wA.top + h / 2; }
            SetWindowPos(target, NULL, nr.left, nr.top, nr.right - nr.left, nr.bottom - nr.top, SWP_NOZORDER | SWP_NOACTIVATE);
            break;
        }


        case ACTION_NEXT_TRACK:
        {
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_MEDIA_NEXT_TRACK;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_MEDIA_NEXT_TRACK;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, inputs, sizeof(INPUT));
            break;
        }

        case ACTION_PREV_TRACK:
        {
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_MEDIA_PREV_TRACK;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_MEDIA_PREV_TRACK;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, inputs, sizeof(INPUT));
            break;
        }

        case ACTION_ALWAYS_ON_TOP:
        {
            if (target) {
                LONG exStyle = GetWindowLong(target, GWL_EXSTYLE);
                BOOL isTopMost = (exStyle & WS_EX_TOPMOST) != 0;
                SetWindowPos(target, isTopMost ? HWND_NOTOPMOST : HWND_TOPMOST,
                    0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            break;
        }

        case ACTION_CREATE_NOTE: {
            extern void StartNoteCreation(BOOL onTop);
            BOOL onTop = FALSE;
            if (target) {
                wchar_t className[256] = {0};
                GetClassNameW(target, className, 256);
                if (wcscmp(className, L"Progman") != 0 && wcscmp(className, L"WorkerW") != 0) {
                    onTop = TRUE;
                }
            }
            StartNoteCreation(onTop);
            break;
        }

        case ACTION_SEARCH_SELECTION: {
            std::wstring selected = GetSelectedText();
            if (!selected.empty()) {
                std::wstring encoded = UrlEncode(selected);
                std::wstring url = g_settings.searchEngineUrl + encoded;
                ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
            break;
        }

        case ACTION_SCREENSHOT: {
            TakeScreenshot(gc->actionParam);
            break;
        }

        case ACTION_SPOTLIGHT: {
            extern std::atomic<int> g_spotlightState;
            if (g_spotlightState.load() > 0) {
                g_spotlightState = 3;
            } else {
                StartSpotlight();
            }
            break;
        }

        case ACTION_EXPLORER_BACK: {
            HWND fg = GetForegroundWindow();
            if (fg) {
                INPUT inputs[4] = {};
                inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_MENU;
                inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_LEFT;
                inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = VK_LEFT; inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
                inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_MENU; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(4, inputs, sizeof(INPUT));
            }
            break;
        }

        case ACTION_EXPLORER_FORWARD: {
            HWND fg = GetForegroundWindow();
            if (fg) {
                INPUT inputs[4] = {};
                inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_MENU;
                inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_RIGHT;
                inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = VK_RIGHT; inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
                inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_MENU; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(4, inputs, sizeof(INPUT));
            }
            break;
        }

        case ACTION_ADMIN_TERMINAL: {
            wchar_t explorerPath[MAX_PATH] = {0};
            BOOL hasPath = GetExplorerPath(g_gestureTarget, explorerPath, MAX_PATH);
            if (!hasPath) {
                hasPath = GetExplorerPath(GetForegroundWindow(), explorerPath, MAX_PATH);
            }
            
            std::wstring path;
            if (hasPath && explorerPath[0] != L'\0') {
                path = explorerPath;
            } else {
                wchar_t userProfile[MAX_PATH] = {0};
                GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH);
                path = userProfile;
            }

            std::wstring safePath;
            for (wchar_t c : path) {
                if (c == L'\'') {
                    safePath += L"''";
                } else {
                    safePath += c;
                }
            }
            std::wstring args = L"-NoExit -Command \"Set-Location -LiteralPath '" + safePath + L"'";
            if (gc->actionParam[0] != L'\0') {
                args += L"; " + std::wstring(gc->actionParam);
            }
            args += L"\"";
            ShellExecuteW(NULL, L"runas", L"powershell.exe", args.c_str(), NULL, SW_SHOWNORMAL);
            break;
        }

        case ACTION_STOPWATCH:
            ToggleStopwatch();
            break;

        case ACTION_TIMER:
            ToggleTimer();
            break;

        case ACTION_FLASH:
            ToggleFlash();
            break;

        case ACTION_NEXT_VIRTUAL_DESKTOP: {
            INPUT inputs[6] = {};
            inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_LWIN;
            inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_LCONTROL;
            inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = VK_RIGHT;
            inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_RIGHT;    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
            inputs[4].type = INPUT_KEYBOARD; inputs[4].ki.wVk = VK_LCONTROL; inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;
            inputs[5].type = INPUT_KEYBOARD; inputs[5].ki.wVk = VK_LWIN;     inputs[5].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(6, inputs, sizeof(INPUT));
            break;
        }

        default:
            break;
    }

    // Restore modifier keys back to their pressed state
    if (kCount > 0) {
        INPUT modDown[6] = {};
        for (int i = 0; i < kCount; i++) {
            modDown[i].type = INPUT_KEYBOARD;
            modDown[i].ki.wVk = releasedKeys[i];
            modDown[i].ki.dwFlags = 0;
        }
        SendInput(kCount, modDown, sizeof(INPUT));
    }
}

void CreateOverlay() {
    if (g_overlayWnd) return;

    int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    g_overlayWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        OVERLAY_CLASS, L"",
        WS_POPUP,
        x, y, cx, cy,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);

    // Initialize with a fully transparent screen
    HDC screenDC = GetDC(NULL);
    g_memDC = CreateCompatibleDC(screenDC);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = -cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    g_memBitmap = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &g_memBits, NULL, 0);
    g_oldBitmap = (HBITMAP)SelectObject(g_memDC, g_memBitmap);
    g_graphics = new Gdiplus::Graphics(g_memDC);
    g_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    ReleaseDC(NULL, screenDC);
}

void DestroyOverlay() {
    if (g_graphics) {
        delete g_graphics;
        g_graphics = nullptr;
    }
    if (g_memDC) {
        SelectObject(g_memDC, g_oldBitmap);
        DeleteObject(g_memBitmap);
        DeleteDC(g_memDC);
        g_memDC = NULL;
        g_memBitmap = NULL;
        g_memBits = NULL;
    }
    if (g_overlayWnd) {
        DestroyWindow(g_overlayWnd);
        g_overlayWnd = nullptr;
    }
}

void SpawnParticles(POINT pt, double velX, double velY) {
    int count = rand() % 2; // 0 or 1 particle per point
    for (int i = 0; i < count; i++) {
        Particle& p = g_particles[g_particleIndex % MAX_PARTICLES];
        p.x = (float)pt.x + (rand() % 8 - 4);
        p.y = (float)pt.y + (rand() % 8 - 4);
        p.dx = (float)(-velX * 0.05 + (rand() % 40 - 20) * 0.02);
        p.dy = (float)(-velY * 0.05 + (rand() % 40 - 20) * 0.02);
        p.spawnTime = GetTickCount();
        p.lifeMs = 150 + (rand() % 150);
        p.size = (float)(1 + (rand() % 3)); // Smaller sizes
        p.colorType = rand() % 3; // 0=base, 1=light, 2=white
        p.active = TRUE;
        g_particleIndex++;
    }
}

void SpawnSplash(POINT pt) {
    for (int i = 0; i < 40; i++) {
        Particle& p = g_particles[g_particleIndex % MAX_PARTICLES];
        p.x = (float)pt.x;
        p.y = (float)pt.y;
        float angle = (rand() % 360) * 3.14159f / 180.0f;
        float speed = (rand() % 50 + 20) * 0.1f;
        p.dx = cos(angle) * speed;
        p.dy = sin(angle) * speed;
        p.spawnTime = GetTickCount();
        p.lifeMs = 400 + (rand() % 400);
        p.size = (float)(2 + (rand() % 4)); 
        p.colorType = rand() % 3;
        p.active = TRUE;
        g_particleIndex++;
    }
}

void TriggerStandaloneSplash(POINT pt) {
    if (!g_overlayWnd) {
        extern void CreateOverlay();
        CreateOverlay();
    }
    SpawnSplash(pt);
    g_fadeActive = TRUE;
    g_fadeStartTick = GetTickCount();
    g_fadePointCount = 0; 
    SetTimer(g_overlayWnd, 1, 16, NULL);
}


void UpdateParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = g_particles[i];
        if (!p.active) continue;
        p.x += p.dx;
        p.y += p.dy;
        p.dx *= 0.95f;
        p.dy *= 0.95f;
        p.dy -= 0.08f;
    }
}

void DrawParticles(Gdiplus::Graphics& graphics, int offsetX, int offsetY, DWORD currentTick, int globalAlpha) {
    using namespace Gdiplus;
    Color baseColor(
        GetRValue(g_settings.trailColor),
        GetGValue(g_settings.trailColor),
        GetBValue(g_settings.trailColor)
    );

    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle& p = g_particles[i];
        if (!p.active) continue;

        int elapsed = currentTick - p.spawnTime;
        if (elapsed >= p.lifeMs) {
            p.active = FALSE;
            continue;
        }

        float progress = (float)elapsed / p.lifeMs;
        int alpha = (int)(255 * (1.0f - progress));
        alpha = (alpha * globalAlpha) / 255;
        if (alpha < 0) alpha = 0;
        if (alpha > 255) alpha = 255;

        Color pColor;
        if (p.colorType == 0) {
            pColor = Color(alpha, baseColor.GetR(), baseColor.GetG(), baseColor.GetB());
        } else if (p.colorType == 1) {
            pColor = Color(alpha, (baseColor.GetR() + 255) / 2, (baseColor.GetG() + 255) / 2, (baseColor.GetB() + 255) / 2);
        } else {
            pColor = Color(alpha, 255, 255, 255);
        }

        int glowAlpha = (alpha * 60) / 255;
        SolidBrush glowBrush(Color(glowAlpha, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()));
        graphics.FillEllipse(&glowBrush, p.x - p.size * 2.5f - offsetX, p.y - p.size * 2.5f - offsetY, p.size * 5, p.size * 5);

        SolidBrush coreBrush(pColor);
        graphics.FillEllipse(&coreBrush, p.x - p.size / 2 - offsetX, p.y - p.size / 2 - offsetY, p.size, p.size);
    }
}

void UpdateOverlay() {
    using namespace Gdiplus;
    if (!g_overlayWnd || g_pointCount < 2) return;
    
    int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    g_graphics->Clear(Color(0, 0, 0, 0));

    Color baseColor(
        GetRValue(g_settings.trailColor),
        GetGValue(g_settings.trailColor),
        GetBValue(g_settings.trailColor)
    );

    PointF* points = new PointF[g_pointCount];
    for (int i = 0; i < g_pointCount; i++) {
        points[i].X = (REAL)(g_points[i].x - x);
        points[i].Y = (REAL)(g_points[i].y - y);
    }

    // Fast 2-pass rendering to drastically improve performance
    REAL width = (REAL)g_settings.trailWidth;

    // 1. Soft glow
    Pen glow(Color(80, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()), width * 2.5f);
    glow.SetStartCap(LineCapRound); glow.SetEndCap(LineCapRound); glow.SetLineJoin(LineJoinRound);
    g_graphics->DrawLines(&glow, points, g_pointCount);

    // 2. Solid colored core
    Pen core(Color(220, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()), width * 0.8f);
    core.SetStartCap(LineCapRound); core.SetEndCap(LineCapRound); core.SetLineJoin(LineJoinRound);
    g_graphics->DrawLines(&core, points, g_pointCount);

    delete[] points;

    if (TRUE) {
        DrawParticles(*g_graphics, x, y, GetTickCount(), 255);
    }

    HDC screenDC = GetDC(NULL);
    POINT ptSrc = { 0, 0 };
    POINT ptDst = { x, y };
    SIZE sizeWnd = { cx, cy };
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(g_overlayWnd, screenDC, &ptDst, &sizeWnd, g_memDC, &ptSrc, 0, &blend, ULW_ALPHA);

    ReleaseDC(NULL, screenDC);
}

void HandleTimerEditKey(WPARAM vkCode) {
    extern void PaintTimerPill(HWND hwnd);
    if (vkCode == VK_ESCAPE) {
        g_timerEditMode = FALSE;
        g_timerEditIndex = -1;
        PaintTimerPill(g_timerWnd);
    } else if (vkCode == VK_RETURN) {
        int val = _wtoi(g_timerEditBuf);
        if (val > 0 && val <= 999 && g_timerEditIndex >= 0 && g_timerEditIndex < 3) {
            g_timerPresets[g_timerEditIndex] = val * 60;
        }
        g_timerEditMode = FALSE;
        g_timerEditIndex = -1;
        PaintTimerPill(g_timerWnd);
    } else if (vkCode == VK_BACK) {
        int len = (int)wcslen(g_timerEditBuf);
        if (len > 0) g_timerEditBuf[len - 1] = 0;
        PaintTimerPill(g_timerWnd);
    } else if ((vkCode >= '0' && vkCode <= '9') || (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)) {
        int len = (int)wcslen(g_timerEditBuf);
        if (len < 3) {
            wchar_t c = (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9) ? (wchar_t)(vkCode - VK_NUMPAD0 + L'0') : (wchar_t)vkCode;
            g_timerEditBuf[len] = c;
            g_timerEditBuf[len + 1] = 0;
        }
        PaintTimerPill(g_timerWnd);
    }
}

void HandleNoteCreationMouseWorker(WPARAM wParam, POINT pt) {
    extern LRESULT CALLBACK NoteSelWndProc(HWND, UINT, WPARAM, LPARAM);
    if (wParam == WM_LBUTTONDOWN) {
        g_noteSelStart = pt;
        g_noteSelCurrent = pt;

        if (!g_noteSelWnd) {
            WNDCLASS wc = {0};
            wc.lpfnWndProc = NoteSelWndProc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = NOTE_SEL_CLASS;
            RegisterClass(&wc);

            g_noteSelWnd = CreateWindowEx(
                WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
                NOTE_SEL_CLASS, L"",
                WS_POPUP | WS_VISIBLE,
                pt.x, pt.y, 0, 0,
                NULL, NULL, GetModuleHandle(NULL), NULL);
            SetLayeredWindowAttributes(g_noteSelWnd, RGB(0,0,0), 160, LWA_COLORKEY | LWA_ALPHA);
        }
    }
    else if (wParam == WM_MOUSEMOVE) {
        if (g_noteSelWnd) {
            g_noteSelCurrent = pt;
            int x = (g_noteSelStart.x < g_noteSelCurrent.x) ? g_noteSelStart.x : g_noteSelCurrent.x;
            int y = (g_noteSelStart.y < g_noteSelCurrent.y) ? g_noteSelStart.y : g_noteSelCurrent.y;
            int w = abs(g_noteSelStart.x - g_noteSelCurrent.x);
            int h = abs(g_noteSelStart.y - g_noteSelCurrent.y);
            MoveWindow(g_noteSelWnd, x, y, w, h, TRUE);
        }
    }
    else if (wParam == WM_LBUTTONUP) {
        int x = (g_noteSelStart.x < g_noteSelCurrent.x) ? g_noteSelStart.x : g_noteSelCurrent.x;
        int y = (g_noteSelStart.y < g_noteSelCurrent.y) ? g_noteSelStart.y : g_noteSelCurrent.y;
        int w = abs(g_noteSelStart.x - g_noteSelCurrent.x);
        int h = abs(g_noteSelStart.y - g_noteSelCurrent.y);

        if (g_noteSelWnd) {
            DestroyWindow(g_noteSelWnd);
            g_noteSelWnd = nullptr;
        }

        if (w > 50 && h > 50) {
            if (g_msgWnd) {
                PostMessage(g_msgWnd, WM_POST_NOTE_CREATION, MAKEWPARAM(x, y), MAKELPARAM(w, h));
            }
        }
    }
}

void HandlePickerMouseWorker(WPARAM wParam, POINT pt) {
    if (wParam == WM_LBUTTONDOWN) {
        HDC hdcScreen = GetDC(NULL);
        COLORREF color = GetPixel(hdcScreen, pt.x, pt.y);
        ReleaseDC(NULL, hdcScreen);

        wchar_t hexSeq[16];
        swprintf_s(hexSeq, L"#%02X%02X%02X", GetRValue(color), GetGValue(color), GetBValue(color));

        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            size_t len = (wcslen(hexSeq) + 1) * sizeof(wchar_t);
            HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, len);
            if (hg) {
                memcpy(GlobalLock(hg), hexSeq, len);
                GlobalUnlock(hg);
                SetClipboardData(CF_UNICODETEXT, hg);
            }
            CloseClipboard();
        }

        HideAura();
        TriggerStandaloneSplash(pt);
        StopColorPicker();
    }
    else if (wParam == WM_RBUTTONUP) {
        HideAura();
        TriggerStandaloneSplash(pt);
        StopColorPicker();
    }
    else if (wParam == WM_MOUSEMOVE) {
        g_pickerPos = pt;
        if (g_pickerWnd) {
            HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            GetMonitorInfo(hMonitor, &mi);

            int wx = pt.x + 15;
            int wy = pt.y + 15;
            if (wx + 160 > mi.rcMonitor.right) wx = pt.x - 160 - 15;
            if (wy + 160 > mi.rcMonitor.bottom) wy = pt.y - 160 - 15;
            if (wx < mi.rcMonitor.left) wx = mi.rcMonitor.left;
            if (wy < mi.rcMonitor.top) wy = mi.rcMonitor.top;

            SetWindowPos(g_pickerWnd, HWND_TOPMOST, wx, wy, 160, 160, SWP_NOACTIVATE);
            InvalidateRect(g_pickerWnd, NULL, FALSE);
        }
    }
}

void HandleDrawMouseWorker(WPARAM wParam, POINT pt) {
    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMonitor, &mi);
    
    int monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    int pW = 500;
    int pH = 50;
    int px = mi.rcMonitor.left + (monitorWidth - pW) / 2;
    int py = mi.rcMonitor.top + (int)g_paletteY;
    RECT paletteRect = { px, py, px + pW, py + pH };
    
    BOOL overPalette = (g_paletteY > -60.0f && PtInRect(&paletteRect, pt));
    
    if (wParam == WM_MOUSEMOVE) {
        if (g_drawDrawing && !overPalette) {
            int wndX = GetSystemMetrics(SM_XVIRTUALSCREEN);
            int wndY = GetSystemMetrics(SM_YVIRTUALSCREEN);
            double localX = pt.x - wndX;
            double localY = pt.y - wndY;
            
            if (g_currentDrawStroke.count > 0 && g_currentDrawStroke.count < 1000) {
                DrawPoint last = g_currentDrawStroke.points[g_currentDrawStroke.count - 1];
                float dx = (float)localX - last.x;
                float dy = (float)localY - last.y;
                if (dx*dx + dy*dy >= 4.0f) {
                    g_currentDrawStroke.points[g_currentDrawStroke.count++] = { (float)localX, (float)localY };
                }
            }
        }
    }
    else if (!overPalette) {
        if (wParam == WM_RBUTTONUP) {
            HideAura();
            TriggerStandaloneSplash(pt);
            ToggleDrawMode();
        }
        else if (wParam == WM_LBUTTONDOWN || wParam == WM_MBUTTONDOWN) {
            if (wParam == WM_MBUTTONDOWN && !g_drawEraserMode) {
                g_drawEraserMode = TRUE;
                g_drawSavedColor = g_drawColor;
                g_drawColor = Gdiplus::Color(0, 0, 0, 0);
            }
            if (g_drawStrokeCount < 200) {
                HideAura();
                g_drawDrawing = TRUE;
                g_currentDrawStroke.count = 0;
                g_currentDrawStroke.color = g_drawColor;
                g_currentDrawStroke.width = (g_drawColor.GetAlpha() == 0) ? g_drawWidth * 4.0f : g_drawWidth;
                
                int wndX = GetSystemMetrics(SM_XVIRTUALSCREEN);
                int wndY = GetSystemMetrics(SM_YVIRTUALSCREEN);
                g_currentDrawStroke.points[g_currentDrawStroke.count++] = { (float)(pt.x - wndX), (float)(pt.y - wndY) };
            }
        }
        else if ((wParam == WM_LBUTTONUP || wParam == WM_MBUTTONUP) && g_drawDrawing) {
            g_drawDrawing = FALSE;
            int wndX = GetSystemMetrics(SM_XVIRTUALSCREEN);
            int wndY = GetSystemMetrics(SM_YVIRTUALSCREEN);
            if (g_currentDrawStroke.count > 0 && g_currentDrawStroke.count < 1000) {
                DrawPoint last = g_currentDrawStroke.points[g_currentDrawStroke.count - 1];
                float localX = (float)(pt.x - wndX);
                float localY = (float)(pt.y - wndY);
                if (localX != last.x || localY != last.y) {
                    g_currentDrawStroke.points[g_currentDrawStroke.count++] = { localX, localY };
                } else {
                    g_currentDrawStroke.points[g_currentDrawStroke.count++] = { localX + 0.01f, localY };
                }
            }
            if (g_currentDrawStroke.count >= 2) {
                g_drawStrokes[g_drawStrokeCount++] = g_currentDrawStroke;
            }
            if (wParam == WM_MBUTTONUP && g_drawEraserMode) {
                g_drawEraserMode = FALSE;
                g_drawColor = g_drawSavedColor;
            }
            InvalidateRect(g_drawModeWnd, NULL, FALSE);
        }
    }
}

LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_EXECUTE_ACTION) {
        ExecuteAction(wParam, (HWND)lParam);
        return 0;
    }
    if (msg == WM_POST_NOTE_CREATION) {
        int x = (short)LOWORD(wParam);
        int y = (short)HIWORD(wParam);
        int w = (short)LOWORD(lParam);
        int h = (short)HIWORD(lParam);

        HWND hwndNotesMsg = g_mainMsgWnd;
        if (!hwndNotesMsg) {
            LaunchNotesProcess();
            for (int i = 0; i < 20; i++) {
                Sleep(50);
                hwndNotesMsg = g_mainMsgWnd;
                if (hwndNotesMsg) break;
            }
        }
        if (hwndNotesMsg) {
            PostMessage(hwndNotesMsg, WM_USER + 105, MAKEWPARAM(x, y), MAKELPARAM(w, h));
        }
        return 0;
    }
    if (msg == WM_HOOK_START_GESTURE) {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        StartGesture(pt);
        return 0;
    }
    if (msg == WM_HOOK_ADD_GESTURE_POINT) {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        AddGesturePoint(pt);
        return 0;
    }
    if (msg == WM_HOOK_END_GESTURE) {
        EndGesture();
        return 0;
    }
    if (msg == WM_HOOK_SHOW_AURA) {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        if (g_settings.showAura) {
            ShowAura(pt);
        }
        return 0;
    }
    if (msg == WM_HOOK_HIDE_AURA) {
        HideAura();
        return 0;
    }
    if (msg == WM_HOOK_UPDATE_AURA) {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        UpdateAura(pt);
        return 0;
    }
    if (msg == WM_HOOK_RECORD_CANVAS) {
        ShowCanvas();
        return 0;
    }
    if (msg == WM_HOOK_KEY_TIMER_EDIT) {
        HandleTimerEditKey(wParam);
        return 0;
    }
    if (msg == WM_HOOK_MOUSE_NOTE) {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        HandleNoteCreationMouseWorker(wParam, pt);
        return 0;
    }
    if (msg == WM_HOOK_MOUSE_PICKER) {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        HandlePickerMouseWorker(wParam, pt);
        return 0;
    }
    if (msg == WM_HOOK_MOUSE_DRAW) {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        HandleDrawMouseWorker(wParam, pt);
        return 0;
    }
    if (msg == WM_HOOK_TOGGLE_DRAW) {
        ToggleDrawMode();
        return 0;
    }
    if (msg == WM_HOOK_START_PICKER) {
        StartColorPicker();
        return 0;
    }
    if (msg == WM_HOOK_START_SPOTLIGHT) {
        StartSpotlight();
        return 0;
    }
    if (msg == WM_HOOK_TOGGLE_STOPWATCH) {
        ToggleStopwatch();
        return 0;
    }
    if (msg == WM_HOOK_TOGGLE_TIMER) {
        ToggleTimer();
        return 0;
    }
    if (msg == WM_HOOK_TOGGLE_FLASH) {
        ToggleFlash();
        return 0;
    }
    if (msg == WM_HOOK_STOP_PICKER) {
        StopColorPicker();
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}



LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) {
        return 1;
    }
    if (msg == WM_TIMER) {
        if (wParam == 1) {
            if (g_fadeActive) {
                UpdateParticles();
                UpdateOverlayFade();
            } else {
                KillTimer(hwnd, 1);
            }
        } else if (wParam == 2) {
            if (g_gestureActive) {
                UpdateParticles();
                UpdateOverlay();
            } else {
                KillTimer(hwnd, 2);
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void UpdateOverlayFade() {
    using namespace Gdiplus;
    if (!g_overlayWnd) return;
    
    DWORD elapsed = GetTickCount() - g_fadeStartTick;
    
    BOOL particlesAlive = FALSE;
    if (TRUE) {
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (g_particles[i].active) { particlesAlive = TRUE; break; }
        }
    }

    if (elapsed >= FADE_DURATION_MS && !particlesAlive) {
        DestroyOverlay();
        g_fadeActive = FALSE;
        return;
    }

    double progress = (double)elapsed / FADE_DURATION_MS;
    if (progress > 1.0) progress = 1.0;
    int alpha = (int)(255 * (1.0 - progress));
    if (alpha < 0) alpha = 0;

    int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    g_graphics->Clear(Color(0, 0, 0, 0));

    if (g_fadePointCount >= 2) {
        Color baseColor(
            GetRValue(g_settings.trailColor),
            GetGValue(g_settings.trailColor),
            GetBValue(g_settings.trailColor)
        );

        PointF* points = new PointF[g_fadePointCount];
        for (int i = 0; i < g_fadePointCount; i++) {
            points[i].X = (REAL)(g_fadePoints[i].x - x);
            points[i].Y = (REAL)(g_fadePoints[i].y - y);
        }

        // Fast 2-pass fade rendering
        REAL width = (REAL)g_settings.trailWidth;

        Pen glow(Color((80 * alpha) / 255, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()), width * 2.5f);
        glow.SetStartCap(LineCapRound); glow.SetEndCap(LineCapRound); glow.SetLineJoin(LineJoinRound);
        g_graphics->DrawLines(&glow, points, g_fadePointCount);

        Pen core(Color((220 * alpha) / 255, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()), width * 0.8f);
        core.SetStartCap(LineCapRound); core.SetEndCap(LineCapRound); core.SetLineJoin(LineJoinRound);
        g_graphics->DrawLines(&core, points, g_fadePointCount);

        delete[] points;
    }

    if (TRUE) {
        DrawParticles(*g_graphics, x, y, GetTickCount(), 255);
    }

    HDC screenDC = GetDC(NULL);
    POINT ptSrc = { 0, 0 };
    POINT ptDst = { x, y };
    SIZE sizeWnd = { cx, cy };
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(g_overlayWnd, screenDC, &ptDst, &sizeWnd, g_memDC, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(NULL, screenDC);
}



void GenerateShapePoints(const wchar_t* shapeName) {
    g_canvasPointCount = 0;
    auto addPoint = [&](int x, int y) { g_canvasPoints[g_canvasPointCount++] = {x, y}; };

    if (wcscmp(shapeName, L"Up") == 0) { addPoint(0, 100); addPoint(0, 0); }
    else if (wcscmp(shapeName, L"Down") == 0) { addPoint(0, 0); addPoint(0, 100); }
    else if (wcscmp(shapeName, L"Left") == 0) { addPoint(100, 0); addPoint(0, 0); }
    else if (wcscmp(shapeName, L"Right") == 0) { addPoint(0, 0); addPoint(100, 0); }
    else if (wcscmp(shapeName, L"L-Shape") == 0) { addPoint(0, 0); addPoint(0, 100); addPoint(100, 100); }
    else if (wcscmp(shapeName, L"V-Shape") == 0) { addPoint(0, 0); addPoint(50, 100); addPoint(100, 0); }
    else if (wcscmp(shapeName, L"Square") == 0) { addPoint(0, 0); addPoint(100, 0); addPoint(100, 100); addPoint(0, 100); addPoint(0, 0); }
    else if (wcscmp(shapeName, L"Triangle") == 0) { addPoint(50, 0); addPoint(100, 100); addPoint(0, 100); addPoint(50, 0); }
    else if (wcscmp(shapeName, L"Circle") == 0) {
        for (int i = 0; i <= 36; i++) {
            addPoint((int)(50 + 50 * cos((i * 10 - 90) * 3.14159 / 180.0)),
                     (int)(50 + 50 * sin((i * 10 - 90) * 3.14159 / 180.0)));
        }
    }
    else if (wcscmp(shapeName, L"Letter C") == 0) {
        for (int i = 0; i <= 18; i++) {
            addPoint((int)(50 + 50 * cos((270 - i * 10) * 3.14159 / 180.0)),
                     (int)(50 + 50 * sin((270 - i * 10) * 3.14159 / 180.0)));
        }
    }
    else if (wcscmp(shapeName, L"Letter M") == 0) { addPoint(0, 100); addPoint(0, 0); addPoint(50, 50); addPoint(100, 0); addPoint(100, 100); }
    else if (wcscmp(shapeName, L"Letter N") == 0) { addPoint(0, 100); addPoint(0, 0); addPoint(100, 100); addPoint(100, 0); }
    else if (wcscmp(shapeName, L"Letter S") == 0) { addPoint(100, 0); addPoint(0, 0); addPoint(0, 50); addPoint(100, 50); addPoint(100, 100); addPoint(0, 100); }
    else if (wcscmp(shapeName, L"Letter V") == 0) { addPoint(0, 0); addPoint(50, 100); addPoint(100, 0); }
    else if (wcscmp(shapeName, L"Letter W") == 0) { addPoint(0, 0); addPoint(25, 100); addPoint(50, 50); addPoint(75, 100); addPoint(100, 0); }
    else if (wcscmp(shapeName, L"Letter Z") == 0) { addPoint(0, 0); addPoint(100, 0); addPoint(0, 100); addPoint(100, 100); }
    else if (wcscmp(shapeName, L"Up-Right") == 0) { addPoint(0, 100); addPoint(100, 0); }
    else if (wcscmp(shapeName, L"Down-Right") == 0) { addPoint(0, 0); addPoint(100, 100); }
    else if (wcscmp(shapeName, L"Down-Left") == 0) { addPoint(100, 0); addPoint(0, 100); }
    else if (wcscmp(shapeName, L"Up-Left") == 0) { addPoint(100, 100); addPoint(0, 0); }
    else { g_canvasPointCount = 0; return; }

    PointD normalizedPts[NUM_RESAMPLE_POINTS];
    Resample(g_canvasPoints, g_canvasPointCount, normalizedPts);
    
    int minX = 9999, maxX = -9999, minY = 9999, maxY = -9999;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        if (normalizedPts[i].x < minX) minX = (int)normalizedPts[i].x;
        if (normalizedPts[i].x > maxX) maxX = (int)normalizedPts[i].x;
        if (normalizedPts[i].y < minY) minY = (int)normalizedPts[i].y;
        if (normalizedPts[i].y > maxY) maxY = (int)normalizedPts[i].y;
    }
    int w = maxX - minX, h = maxY - minY;
    double scale = (w > h ? w : h);
    if (scale == 0) scale = 1;

    int targetSize = 350;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        normalizedPts[i].x = ((normalizedPts[i].x - minX) / scale) * targetSize;
        normalizedPts[i].y = ((normalizedPts[i].y - minY) / scale) * targetSize;
    }
    
    minX = 9999; maxX = -9999; minY = 9999; maxY = -9999;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        if (normalizedPts[i].x < minX) minX = (int)normalizedPts[i].x;
        if (normalizedPts[i].x > maxX) maxX = (int)normalizedPts[i].x;
        if (normalizedPts[i].y < minY) minY = (int)normalizedPts[i].y;
        if (normalizedPts[i].y > maxY) maxY = (int)normalizedPts[i].y;
    }
    int shapeW = maxX - minX;
    int shapeH = maxY - minY;
    
    RECT rc;
    GetClientRect(g_canvasWnd, &rc);
    RECT drawArea = { 20, 105, rc.right - 20, rc.bottom - 60 };
    int canvasW = drawArea.right - drawArea.left;
    int canvasH = drawArea.bottom - drawArea.top;

    int offsetX = drawArea.left + (canvasW - shapeW) / 2;
    int offsetY = drawArea.top + (canvasH - shapeH) / 2;
    
    g_canvasPointCount = NUM_RESAMPLE_POINTS;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        g_canvasPoints[i].x = (LONG)(normalizedPts[i].x + offsetX);
        g_canvasPoints[i].y = (LONG)(normalizedPts[i].y + offsetY);
    }
}

static int g_hoverBtn = 0;
static wchar_t g_canvasStatusText[256] = L"Draw gesture, or pick a shape below.";

void SetCanvasStatus(HWND hwnd, const wchar_t* text) {
    wcsncpy_s(g_canvasStatusText, text, _TRUNCATE);
    RECT statusArea = { 20, 535, 780, 570 };
    InvalidateRect(hwnd, &statusArea, FALSE);
}

void LoadPreviewFromNormalized(const PointD* normalizedPts, HWND hwndCanvas) {
    PointD ptsCopy[NUM_RESAMPLE_POINTS];
    memcpy(ptsCopy, normalizedPts, sizeof(ptsCopy));

    double minX = 9999, maxX = -9999, minY = 9999, maxY = -9999;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        if (ptsCopy[i].x < minX) minX = ptsCopy[i].x;
        if (ptsCopy[i].x > maxX) maxX = ptsCopy[i].x;
        if (ptsCopy[i].y < minY) minY = ptsCopy[i].y;
        if (ptsCopy[i].y > maxY) maxY = ptsCopy[i].y;
    }
    double w = maxX - minX, h = maxY - minY;
    double scale = (w > h ? w : h);
    if (scale == 0) scale = 1;

    int targetSize = 350;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        ptsCopy[i].x = ((ptsCopy[i].x - minX) / scale) * targetSize;
        ptsCopy[i].y = ((ptsCopy[i].y - minY) / scale) * targetSize;
    }

    minX = 9999; maxX = -9999; minY = 9999; maxY = -9999;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        if (ptsCopy[i].x < minX) minX = ptsCopy[i].x;
        if (ptsCopy[i].x > maxX) maxX = ptsCopy[i].x;
        if (ptsCopy[i].y < minY) minY = ptsCopy[i].y;
        if (ptsCopy[i].y > maxY) maxY = ptsCopy[i].y;
    }
    double shapeW = maxX - minX;
    double shapeH = maxY - minY;

    RECT rc;
    GetClientRect(hwndCanvas, &rc);
    RECT drawArea = { 20, 105, rc.right - 20, rc.bottom - 60 };
    int canvasW = drawArea.right - drawArea.left;
    int canvasH = drawArea.bottom - drawArea.top;

    int offsetX = drawArea.left + (canvasW - (int)shapeW) / 2;
    int offsetY = drawArea.top + (canvasH - (int)shapeH) / 2;

    g_canvasPointCount = NUM_RESAMPLE_POINTS;
    for (int i = 0; i < NUM_RESAMPLE_POINTS; i++) {
        g_canvasPoints[i].x = (LONG)(ptsCopy[i].x + offsetX);
        g_canvasPoints[i].y = (LONG)(ptsCopy[i].y + offsetY);
    }
}

LRESULT CALLBACK CanvasProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            HWND hCmb = CreateWindow(L"COMBOBOX", L"",
                CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
                80, 15, 180, 300, hwnd, (HMENU)3001, GetModuleHandle(NULL), NULL);
                
            const wchar_t* shapes[] = {
                L"--- Directions ---", L"Up", L"Down", L"Left", L"Right",
                L"--- Diagonals ---", L"Up-Right", L"Down-Right", L"Down-Left", L"Up-Left",
                L"--- Letters ---", L"Letter C", L"Letter M", L"Letter N", L"Letter S", L"Letter V", L"Letter W", L"Letter Z"
            };
            for (const wchar_t* s : shapes) {
                SendMessage(hCmb, CB_ADDSTRING, 0, (LPARAM)s);
            }

            HWND hEdit = CreateWindowEx(0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                122, 62, 636, 22, hwnd, (HMENU)3002, GetModuleHandle(NULL), NULL);

            if (g_uiFont) {
                SendMessage(hCmb, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
                SendMessage(hEdit, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBM = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

            using namespace Gdiplus;
            Graphics graphics(memDC);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);

            // Dark Modern Glassy Background
            SolidBrush bgBrush(Color(255, 16, 16, 22));
            graphics.FillRectangle(&bgBrush, 0, 0, rc.right, rc.bottom);

            auto AddRoundedRect = [](GraphicsPath& path, int x, int y, int w, int h, int r) {
                path.AddArc(x, y, r*2, r*2, 180, 90);
                path.AddArc(x + w - r*2, y, r*2, r*2, 270, 90);
                path.AddArc(x + w - r*2, y + h - r*2, r*2, r*2, 0, 90);
                path.AddArc(x, y + h - r*2, r*2, r*2, 90, 90);
                path.CloseFigure();
            };

            // Drawing Area (Rounded Glass Pane)
            RECT drawArea = { 20, 105, rc.right - 20, rc.bottom - 60 };
            GraphicsPath drawBgPath;
            AddRoundedRect(drawBgPath, drawArea.left, drawArea.top, drawArea.right - drawArea.left, drawArea.bottom - drawArea.top, 16);
            SolidBrush drawBgBrush(Color(255, 12, 12, 16));
            graphics.FillPath(&drawBgBrush, &drawBgPath);
            
            Pen drawBorderPen(Color(100, 0, 170, 255), 1.5f);
            graphics.DrawPath(&drawBorderPen, &drawBgPath);

            // Draw Buttons
            auto DrawButton = [&](int id, const wchar_t* text, int x, int y, int w, int h, bool isPrimary) {
                GraphicsPath btnPath;
                AddRoundedRect(btnPath, x, y, w, h, 16); // Modern slightly rounded corners

                bool isHover = (g_hoverBtn == id);
                Color btnBg = isPrimary ? (isHover ? Color(255, 0, 160, 255) : Color(255, 0, 120, 210)) 
                                         : (isHover ? Color(255, 60, 60, 75)  : Color(255, 40, 40, 50));
                
                SolidBrush bBrush(btnBg);
                graphics.FillPath(&bBrush, &btnPath);
                
                if (!isPrimary) {
                    Pen bPen(Color(40, 255, 255, 255), 1.0f);
                    graphics.DrawPath(&bPen, &btnPath);
                }

                FontFamily fontFamily(L"Segoe UI");
                Font font(&fontFamily, 10, FontStyleRegular, UnitPoint);
                SolidBrush textBrush(Color(255, 255, 255, 255));
                StringFormat sf;
                sf.SetAlignment(StringAlignmentCenter);
                sf.SetLineAlignment(StringAlignmentCenter);
                RectF textRect((REAL)x, (REAL)y, (REAL)w, (REAL)h);
                graphics.DrawString(text, -1, &font, textRect, &sf, &textBrush);
            };

            DrawButton(1, L"\u2398 Copy Code", 280, 15, 140, 32, true);
            DrawButton(2, L"\u21BA Clear", 430, 15, 100, 32, false);
            DrawButton(3, L"Cancel", 540, 15, 80, 32, false);

            FontFamily statusFamily(L"Segoe UI");
            Font statusFont(&statusFamily, 10, FontStyleRegular, UnitPoint);
            SolidBrush statusBrush(Color(255, 180, 180, 190));
            StringFormat sfLeft;
            sfLeft.SetAlignment(StringAlignmentNear);
            sfLeft.SetLineAlignment(StringAlignmentCenter);
            
            RectF statusRect(20.0f, 540.0f, 760.0f, 30.0f);
            graphics.DrawString(g_canvasStatusText, -1, &statusFont, statusRect, &sfLeft, &statusBrush);

            FontFamily labelFamily(L"Segoe UI");
            Font labelFont(&labelFamily, 10, FontStyleRegular, UnitPoint);
            SolidBrush labelBrush(Color(255, 200, 200, 200));

            RectF presetRect(20.0f, 15.0f, 80.0f, 32.0f);
            graphics.DrawString(L"Preset:", -1, &labelFont, presetRect, &sfLeft, &labelBrush);

            RectF codeRect(20.0f, 60.0f, 100.0f, 26.0f);
            graphics.DrawString(L"Gesture Code:", -1, &labelFont, codeRect, &sfLeft, &labelBrush);

            GraphicsPath editPath;
            AddRoundedRect(editPath, 120, 60, 640, 26, 4);
            Pen editBorderPen(Color(80, 255, 255, 255), 1.0f);
            graphics.DrawPath(&editBorderPen, &editPath);

            if (g_canvasPointCount >= 2) {
                using namespace Gdiplus;
                Graphics graphics(memDC);
                graphics.SetSmoothingMode(SmoothingModeAntiAlias);

                PointF* points = new PointF[g_canvasPointCount];
                for (int i = 0; i < g_canvasPointCount; i++) {
                    points[i].X = (REAL)g_canvasPoints[i].x;
                    points[i].Y = (REAL)g_canvasPoints[i].y;
                }

                Color baseColor(0, 255, 160);

                // Pass 1: Soft outer glow
                Pen glowPen(Color(35, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()), 10.5f);
                glowPen.SetStartCap(LineCapRound);
                glowPen.SetEndCap(LineCapRound);
                glowPen.SetLineJoin(LineJoinRound);
                graphics.DrawLines(&glowPen, points, g_canvasPointCount);

                // Pass 2: Bright medium glow
                Pen midPen(Color(100, baseColor.GetR(), baseColor.GetG(), baseColor.GetB()), 6.0f);
                midPen.SetStartCap(LineCapRound);
                midPen.SetEndCap(LineCapRound);
                midPen.SetLineJoin(LineJoinRound);
                graphics.DrawLines(&midPen, points, g_canvasPointCount);

                // Pass 3: White high-quality core
                Pen corePen(Color(255, 255, 255, 255), 2.7f);
                corePen.SetStartCap(LineCapRound);
                corePen.SetEndCap(LineCapRound);
                corePen.SetLineJoin(LineJoinRound);
                graphics.DrawLines(&corePen, points, g_canvasPointCount);

                delete[] points;

                // Draw start circle (Green)
                HBRUSH startBrush = CreateSolidBrush(RGB(0, 255, 100));
                HBRUSH oldB = (HBRUSH)SelectObject(memDC, startBrush);
                HPEN startPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                HPEN oldPen = (HPEN)SelectObject(memDC, startPen);
                int sx = g_canvasPoints[0].x, sy = g_canvasPoints[0].y;
                Ellipse(memDC, sx - 8, sy - 8, sx + 8, sy + 8);
                
                // Draw end arrow/circle (Red)
                HBRUSH endBrush = CreateSolidBrush(RGB(255, 80, 80));
                SelectObject(memDC, endBrush);
                int ex = g_canvasPoints[g_canvasPointCount - 1].x, ey = g_canvasPoints[g_canvasPointCount - 1].y;
                Ellipse(memDC, ex - 8, ey - 8, ex + 8, ey + 8);

                // Draw arrow at end pointing in direction of last segment
                int prevIdx = g_canvasPointCount > 5 ? g_canvasPointCount - 5 : 0;
                int lx = g_canvasPoints[prevIdx].x;
                int ly = g_canvasPoints[prevIdx].y;
                double dx = ex - lx, dy = ey - ly;
                double len = sqrt(dx*dx + dy*dy);
                if (len > 0) {
                    dx /= len; dy /= len;
                    POINT arrow[3];
                    arrow[0] = { ex + (int)(dx * 16), ey + (int)(dy * 16) };
                    arrow[1] = { ex - (int)(dx * 12) - (int)(dy * 12), ey - (int)(dy * 12) + (int)(dx * 12) };
                    arrow[2] = { ex - (int)(dx * 12) + (int)(dy * 12), ey - (int)(dy * 12) - (int)(dx * 12) };
                    Polygon(memDC, arrow, 3);
                }

                SelectObject(memDC, oldB);
                SelectObject(memDC, oldPen);
                DeleteObject(startBrush);
                DeleteObject(endBrush);
                DeleteObject(startPen);
            }

            if (g_canvasPointCount == 0) {
                SetBkMode(memDC, TRANSPARENT);
                SetTextColor(memDC, RGB(80, 80, 100));
                RECT textRc = drawArea;
                DrawText(memDC, L"Hold left mouse button and draw your gesture shape here",
                    -1, &textRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBM);
            DeleteObject(memBM);
            DeleteDC(memDC);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            
            if (pt.y >= 15 && pt.y <= 47) {
                if (pt.x >= 280 && pt.x <= 420) { SendMessage(hwnd, WM_COMMAND, 1001, 0); return 0; }
                if (pt.x >= 430 && pt.x <= 530) { SendMessage(hwnd, WM_COMMAND, 1002, 0); return 0; }
                if (pt.x >= 540 && pt.x <= 620) { SendMessage(hwnd, WM_COMMAND, 1003, 0); return 0; }
            }
            
            RECT rc;
            GetClientRect(hwnd, &rc);
            RECT drawArea = { 20, 105, rc.right - 20, rc.bottom - 60 };

            if (PtInRect(&drawArea, pt)) {
                g_canvasDrawing = TRUE;
                g_canvasPointCount = 0;
                g_canvasPoints[g_canvasPointCount++] = pt;
                SetCapture(hwnd);
                InvalidateRect(hwnd, &drawArea, FALSE);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            
            int newHover = 0;
            if (pt.y >= 15 && pt.y <= 47) {
                if (pt.x >= 280 && pt.x <= 420) newHover = 1;
                else if (pt.x >= 430 && pt.x <= 530) newHover = 2;
                else if (pt.x >= 540 && pt.x <= 620) newHover = 3;
            }
            
            if (newHover != g_hoverBtn) {
                g_hoverBtn = newHover;
                RECT btnArea = { 280, 15, 630, 47 };
                InvalidateRect(hwnd, &btnArea, FALSE);
            }
            
            if (g_canvasDrawing && g_canvasPointCount < MAX_POINTS) {
                POINT smoothedPt = { pt.x, pt.y };

                POINT last = g_canvasPoints[g_canvasPointCount - 1];
                double dx = (double)(smoothedPt.x - last.x);
                double dy = (double)(smoothedPt.y - last.y);
                if (dx * dx + dy * dy >= 4.0) {
                    g_canvasPoints[g_canvasPointCount++] = smoothedPt;
                    RECT drawArea = { 20, 105, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                    InvalidateRect(hwnd, &drawArea, FALSE);
                }
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_LBUTTONUP: {
            if (g_canvasDrawing) {
                g_canvasDrawing = FALSE;
                ReleaseCapture();

                POINT pt = { LOWORD(lParam), HIWORD(lParam) };
                if (g_canvasPointCount > 0 && g_canvasPointCount < MAX_POINTS) {
                    POINT last = g_canvasPoints[g_canvasPointCount - 1];
                    if (pt.x != last.x || pt.y != last.y) {
                        g_canvasPoints[g_canvasPointCount++] = pt;
                    }
                }

                wchar_t status[256];
                if (g_canvasPointCount >= 2) {
                    swprintf_s(status, L"\u2714 Gesture recorded (%d points). Click \"Copy Gesture Code\".", g_canvasPointCount);
                    
                    PointD normalizedPts[NUM_RESAMPLE_POINTS];
                    NormalizeGesture(g_canvasPoints, g_canvasPointCount, normalizedPts);
                    wchar_t hexSeq[NUM_RESAMPLE_POINTS * 4 + 1];
                    FormatGestureHex(normalizedPts, hexSeq);
                    
                    g_settingEditText = TRUE;
                    SetWindowText(GetDlgItem(hwnd, 3002), hexSeq);
                    g_settingEditText = FALSE;
                } else {
                    swprintf_s(status, L"Gesture too short or unclear. Try drawing a larger shape.");
                }
                SetCanvasStatus(hwnd, status);
            }
            return 0;
        }

        case WM_CTLCOLOREDIT: {
            SetTextColor((HDC)wParam, RGB(255, 255, 255));
            SetBkColor((HDC)wParam, RGB(25, 25, 30));
            HBRUSH hbr = (HBRUSH)GetProp(hwnd, L"EditBgBrush");
            if (!hbr) {
                hbr = CreateSolidBrush(RGB(25, 25, 30));
                SetProp(hwnd, L"EditBgBrush", hbr);
            }
            return (LRESULT)hbr;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            
            if (id == 3002 && HIWORD(wParam) == EN_CHANGE) {
                if (!g_settingEditText) {
                    HWND hEdit = (HWND)lParam;
                    int len = GetWindowTextLength(hEdit);
                    if (len == NUM_RESAMPLE_POINTS * 4) {
                        wchar_t* text = new wchar_t[len + 1];
                        GetWindowText(hEdit, text, len + 1);
                        PointD previewPts[NUM_RESAMPLE_POINTS];
                        if (ParseGestureHex(text, previewPts)) {
                            LoadPreviewFromNormalized(previewPts, hwnd);
                            SetCanvasStatus(hwnd, L"\u2714 Custom gesture preview loaded.");
                            RECT drawArea = { 20, 105, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                            InvalidateRect(hwnd, &drawArea, FALSE);
                        } else {
                            SetCanvasStatus(hwnd, L"Invalid gesture code format.");
                        }
                        delete[] text;
                    }
                }
                return 0;
            }

            if (id == 3001 && HIWORD(wParam) == CBN_SELCHANGE) {
                HWND hCmb = (HWND)lParam;
                int sel = SendMessage(hCmb, CB_GETCURSEL, 0, 0);
                if (sel >= 0) {
                    wchar_t text[128];
                    SendMessage(hCmb, CB_GETLBTEXT, sel, (LPARAM)text);
                    if (wcsstr(text, L"---")) return 0; // Ignore category headers
                    GenerateShapePoints(text);
                    
                    if (g_canvasPointCount >= 2) {
                        PointD normalizedPts[NUM_RESAMPLE_POINTS];
                        NormalizeGesture(g_canvasPoints, g_canvasPointCount, normalizedPts);
                        wchar_t hexSeq[NUM_RESAMPLE_POINTS * 4 + 1];
                        FormatGestureHex(normalizedPts, hexSeq);
                        
                        g_settingEditText = TRUE;
                        SetWindowText(GetDlgItem(hwnd, 3002), hexSeq);
                        g_settingEditText = FALSE;
                    }
                    
                    RECT drawArea = { 20, 105, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                    InvalidateRect(hwnd, &drawArea, FALSE); // Prevent background flicker
                    SetCanvasStatus(hwnd, L"\u2714 Preset loaded. Click \"Copy Gesture Code\".");
                }
                return 0;
            }

            if (id == 1001) {
                if (g_canvasPointCount < 2) {
                    SetCanvasStatus(hwnd, L"Cannot copy: gesture too short. Draw a bigger shape.");
                    return 0;
                }

                PointD normalizedPts[NUM_RESAMPLE_POINTS];
                NormalizeGesture(g_canvasPoints, g_canvasPointCount, normalizedPts);
                wchar_t hexSeq[NUM_RESAMPLE_POINTS * 4 + 1];
                FormatGestureHex(normalizedPts, hexSeq);

                if (OpenClipboard(hwnd)) {
                    EmptyClipboard();
                    size_t len = (wcslen(hexSeq) + 1) * sizeof(wchar_t);
                    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, len);
                    if (hg) {
                        memcpy(GlobalLock(hg), hexSeq, len);
                        GlobalUnlock(hg);
                        SetClipboardData(CF_UNICODETEXT, hg);
                    }
                    CloseClipboard();
                    SetCanvasStatus(hwnd, L"\u2714 Copied! Go to Windhawk Settings \u2192 Gestures \u2192 Paste into \"Gesture Code\" field.");
                }

                g_canvasPointCount = 0;
                
                g_settingEditText = TRUE;
                SetWindowText(GetDlgItem(hwnd, 3002), L"");
                g_settingEditText = FALSE;

                RECT drawArea = { 20, 105, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                InvalidateRect(hwnd, &drawArea, FALSE);
            }
            else if (id == 1002) {
                g_canvasPointCount = 0;
                g_canvasDrawing = FALSE;
                SetCanvasStatus(hwnd, L"Canvas cleared. Draw a new gesture.");
                
                g_settingEditText = TRUE;
                SetWindowText(GetDlgItem(hwnd, 3002), L"");
                g_settingEditText = FALSE;

                RECT drawArea = { 20, 105, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
                InvalidateRect(hwnd, &drawArea, FALSE);
            }
            else if (id == 1003) {
                HideCanvas();
            }
            return 0;
        }

        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                HideCanvas();
            }
            return 0;
        }

        case WM_CLOSE: {
            HideCanvas();
            return 0;
        }

        case WM_DESTROY: {
            HBRUSH hbr = (HBRUSH)GetProp(hwnd, L"EditBgBrush");
            if (hbr) {
                DeleteObject(hbr);
                RemoveProp(hwnd, L"EditBgBrush");
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowCanvas() {
    if (g_canvasWnd && IsWindowVisible(g_canvasWnd)) {
        SetForegroundWindow(g_canvasWnd);
        return;
    }

    g_canvasPointCount = 0;
    g_canvasDrawing = FALSE;
    g_recording = TRUE;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int w = 800;
    int h = 680;
    if (w > screenW - 40) w = screenW - 40;
    if (h > screenH - 40) h = screenH - 40;
    int x = (screenW - w) / 2;
    int y = (screenH - h) / 2;

    g_canvasWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_APPWINDOW,
        CANVAS_CLASS, L"Mouse Gestures \u2014 Record New Gesture",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
        x, y, w, h,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(g_canvasWnd, SW_SHOW);
    UpdateWindow(g_canvasWnd);
    SetForegroundWindow(g_canvasWnd);
}

void HideCanvas() {
    g_recording = FALSE;
    g_canvasDrawing = FALSE;
    if (g_canvasWnd) {
        DestroyWindow(g_canvasWnd);
        g_canvasWnd = nullptr;
    }
}

void StartGesture(POINT pt) {
    if (IsFullscreenAppActive()) {
        g_gestureActive = FALSE;
        return;
    }

    if (g_fadeActive) {
        g_fadeActive = FALSE;
        KillTimer(g_overlayWnd, 1);
        DestroyOverlay();
    }

    g_gestureTarget = WindowFromPoint(pt);
    g_pointCount = 0;
    g_points[g_pointCount++] = pt;
    g_particleIndex = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        g_particles[i].active = FALSE;
    }

    CreateOverlay();
    if (g_overlayWnd) {
        SetTimer(g_overlayWnd, 2, 16, NULL);
    }
}

void AddGesturePoint(POINT pt) {
    if (!g_gestureActive) return;
    if (g_pointCount == 0 || g_pointCount >= MAX_POINTS) return;

    POINT smoothedPt = { pt.x, pt.y };

    POINT last = g_points[g_pointCount - 1];
    double dx = (double)(smoothedPt.x - last.x);
    double dy = (double)(smoothedPt.y - last.y);
    if (dx * dx + dy * dy < 4.0) return;

    g_points[g_pointCount++] = smoothedPt;
    SpawnParticles(smoothedPt, dx, dy);
}

void SynthesizeClick() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwExtraInfo = 0x1337;

    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwExtraInfo = 0x1337;

    switch (g_settings.drawButton) {
        case DRAW_RIGHT:
            inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case DRAW_MIDDLE:
            inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        case DRAW_LEFT:
            inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case DRAW_MOUSE4:
            inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
            inputs[0].mi.mouseData = XBUTTON1;
            inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
            inputs[1].mi.mouseData = XBUTTON1;
            break;
        case DRAW_MOUSE5:
            inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
            inputs[0].mi.mouseData = XBUTTON2;
            inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;
            inputs[1].mi.mouseData = XBUTTON2;
            break;
    }

    SendInput(2, inputs, sizeof(INPUT));
}

void EndGesture() {
    if (g_pointCount == 0) return;

    if (g_pointCount > 1 && g_overlayWnd) {
        g_fadeActive = TRUE;
        g_fadeStartTick = GetTickCount();
        g_fadePointCount = g_pointCount;
        memcpy(g_fadePoints, g_points, sizeof(POINT) * g_pointCount);
        KillTimer(g_overlayWnd, 2);
        SetTimer(g_overlayWnd, 1, 16, NULL);
    } else {
        if (g_overlayWnd) KillTimer(g_overlayWnd, 2);
        DestroyOverlay();
    }

    double totalDist = 0;
    for (int i = 1; i < g_pointCount; i++) {
        double dx = (double)(g_points[i].x - g_points[i - 1].x);
        double dy = (double)(g_points[i].y - g_points[i - 1].y);
        totalDist += sqrt(dx * dx + dy * dy);
    }

    if (totalDist < g_settings.minGestureDistance || g_pointCount < 2) {
        Wh_Log(L"Gesture too short (%.0f px < %d px min)", totalDist, g_settings.minGestureDistance);
        if (g_pointCount > 0) {
            HWND target = WindowFromPoint(g_points[0]);
            target = GetAncestor(target, GA_ROOT);
            if (target && target != GetDesktopWindow() && target != GetShellWindow()) {
                SetForegroundWindow(target);
            }
        }
        SynthesizeClick();
        return;
    }

    PointD normalizedPts[NUM_RESAMPLE_POINTS];
    NormalizeGesture(g_points, g_pointCount, normalizedPts);

    int match = MatchGesture(normalizedPts);
    if (match >= 0) {
        Wh_Log(L"Matched gesture: %s", g_settings.gestures[match].name);
        HWND target = GetAncestor(g_gestureTarget, GA_ROOT);
        if (!target) target = g_gestureTarget;
        PostMessage(g_msgWnd, WM_EXECUTE_ACTION, match, (LPARAM)target);
        
        if (g_pointCount > 0) {
            SpawnSplash(g_points[g_pointCount - 1]);
        }

        POINT toastPt = g_points[g_pointCount - 1];
        ShowToast(g_settings.gestures[match].name, TRUE, toastPt);

        g_wiggleArmed = FALSE;
        g_modifierToggleArmed = FALSE;
        HideAura();
    } else {
        Wh_Log(L"No matching gesture found.");
        POINT toastPt = g_points[g_pointCount > 0 ? g_pointCount - 1 : 0];
        ShowToast(L"No match", FALSE, toastPt);
    }
}

BOOL IsDrawButtonDown(WPARAM wParam, MSLLHOOKSTRUCT* ms) {
    switch (g_settings.drawButton) {
        case DRAW_RIGHT:  return wParam == WM_RBUTTONDOWN;
        case DRAW_MIDDLE: return wParam == WM_MBUTTONDOWN;
        case DRAW_LEFT:   return wParam == WM_LBUTTONDOWN;
        case DRAW_MOUSE4: return wParam == WM_XBUTTONDOWN && HIWORD(ms->mouseData) == XBUTTON1;
        case DRAW_MOUSE5: return wParam == WM_XBUTTONDOWN && HIWORD(ms->mouseData) == XBUTTON2;
    }
    return FALSE;
}

BOOL IsDrawButtonUp(WPARAM wParam, MSLLHOOKSTRUCT* ms) {
    switch (g_settings.drawButton) {
        case DRAW_RIGHT:  return wParam == WM_RBUTTONUP;
        case DRAW_MIDDLE: return wParam == WM_MBUTTONUP;
        case DRAW_LEFT:   return wParam == WM_LBUTTONUP;
        case DRAW_MOUSE4: return wParam == WM_XBUTTONUP && HIWORD(ms->mouseData) == XBUTTON1;
        case DRAW_MOUSE5: return wParam == WM_XBUTTONUP && HIWORD(ms->mouseData) == XBUTTON2;
    }
    return FALSE;
}

void UpdateModifierToggle() {
    if (g_settings.modifierBehavior != MOD_BEHAVIOR_TOGGLE) return;
    if (g_settings.modifierFlags == 0) return;

    static DWORD lastToggleTime = 0;
    DWORD now = GetTickCount();

    BOOL active = IsModifierActive();
    if (active && !g_modifierWasActive && (now - lastToggleTime > 500)) {
        lastToggleTime = now;
        g_modifierToggleArmed = !g_modifierToggleArmed;
        if (g_modifierToggleArmed) {
            g_modifierToggleArmTime = now;
        }
        if (g_settings.enableWiggle == WIGGLE_NEVER) {
            if (g_modifierToggleArmed) {
                POINT pt; GetCursorPos(&pt);
                if (g_settings.showAura) {
                    if (g_msgWnd) {
                        PostMessage(g_msgWnd, WM_HOOK_SHOW_AURA, 0, MAKELPARAM(pt.x, pt.y));
                    }
                }
            } else {
                if (g_msgWnd) {
                    PostMessage(g_msgWnd, WM_HOOK_HIDE_AURA, 0, 0);
                }
            }
        }
    }
    g_modifierWasActive = active;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        UpdateModifierToggle();
        
        if (g_settings.armTimeout > 0) {
            DWORD now = GetTickCount();
            if (g_wiggleArmed && (now - g_wiggleArmTime > (DWORD)g_settings.armTimeout)) {
                g_wiggleArmed = FALSE;
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_HIDE_AURA, 0, 0);
            }
            if (g_settings.modifierBehavior == MOD_BEHAVIOR_TOGGLE && g_modifierToggleArmed) {
                if (now - g_modifierToggleArmTime > (DWORD)g_settings.armTimeout) {
                    g_modifierToggleArmed = FALSE;
                    if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_HIDE_AURA, 0, 0);
                }
            }
        }
        
        MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;

        if (ms->dwExtraInfo == 0x1337) {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }

        if (g_noteCreationMode) {
            POINT pt = ms->pt;
            if (g_msgWnd) {
                PostMessage(g_msgWnd, WM_HOOK_MOUSE_NOTE, wParam, MAKELPARAM(pt.x, pt.y));
            }
            if (wParam == WM_LBUTTONUP) {
                g_noteCreationMode = FALSE;
            }
            if (wParam == WM_MOUSEMOVE) {
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            return 1;
        }

        if (wParam == WM_MOUSEMOVE && !g_gestureActive && !g_pickerActive && !g_drawModeActive) {
            if (g_settings.enableWiggle != WIGGLE_NEVER) {
                BOOL modActive = TRUE;
                if (g_settings.enableWiggle == WIGGLE_MODIFIER) {
                    if (g_settings.modifierFlags == 0) {
                        modActive = FALSE;
                    } else {
                        modActive = (g_settings.modifierBehavior == MOD_BEHAVIOR_TOGGLE) ? (BOOL)g_modifierToggleArmed : IsModifierActive();
                    }
                }

                if (modActive) {
                    DWORD now = GetTickCount();
                    if (g_wiggleArmed) {
                        if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_UPDATE_AURA, 0, MAKELPARAM(ms->pt.x, ms->pt.y));
                    } else {
                        if (now - g_lastWiggleTime > 250) {
                            g_wiggleCount = 0;
                            g_wiggleSign = 0; // 0 = uninitialized
                        }
                        
                        int currentX = ms->pt.x;
                        int requiredDist = g_settings.wiggleStrength * 3;
                        
                        if (g_wiggleSign == 0) {
                            if (currentX != g_lastWigglePt.x) {
                                g_wiggleAccum = currentX; // Use accum as extremeX
                                g_wiggleSign = (currentX > g_lastWigglePt.x) ? 1 : -1;
                                g_lastWiggleTime = now;
                            }
                        } else if (g_wiggleSign == 1) { // Moving Right
                            if (currentX > g_wiggleAccum) {
                                g_wiggleAccum = currentX;
                            } else if (currentX < g_wiggleAccum - requiredDist) {
                                g_wiggleCount++;
                                g_wiggleSign = -1;
                                g_wiggleAccum = currentX;
                                g_lastWiggleTime = now;
                            }
                        } else if (g_wiggleSign == -1) { // Moving Left
                            if (currentX < g_wiggleAccum) {
                                g_wiggleAccum = currentX;
                            } else if (currentX > g_wiggleAccum + requiredDist) {
                                g_wiggleCount++;
                                g_wiggleSign = 1;
                                g_wiggleAccum = currentX;
                                g_lastWiggleTime = now;
                            }
                        }

                        if (g_wiggleCount >= 6) {
                            if (IsFullscreenAppActive()) {
                                g_wiggleCount = 0;
                                g_wiggleSign = 0;
                            } else {
                                g_wiggleArmed = TRUE;
                                g_wiggleArmTime = now;
                                g_wiggleCount = 0;
                                g_wiggleSign = 0;
                                if (g_msgWnd) {
                                    PostMessage(g_msgWnd, WM_HOOK_SHOW_AURA, 0, MAKELPARAM(ms->pt.x, ms->pt.y));
                                }
                            }
                        }
                        
                        g_lastWigglePt = ms->pt;
                    }
                } else {
                    g_wiggleCount = 0;
                    g_wiggleSign = 0;
                    g_lastWigglePt = ms->pt;
                }
            }
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }

        if (g_gestureActive) {
            if (wParam == WM_MOUSEMOVE) {
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_ADD_GESTURE_POINT, 0, MAKELPARAM(ms->pt.x, ms->pt.y));
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            if (IsDrawButtonUp(wParam, ms)) {
                g_gestureActive = FALSE;
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_END_GESTURE, 0, MAKELPARAM(ms->pt.x, ms->pt.y));
                return 1;
            }
            if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP ||
                wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP ||
                wParam == WM_MBUTTONDOWN || wParam == WM_MBUTTONUP ||
                wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP) {
                return 1;
            }
        }

        BOOL modifierValid = FALSE;
        if (g_settings.modifierFlags == 0) {
            modifierValid = TRUE;
        } else {
            modifierValid = (g_settings.modifierBehavior == MOD_BEHAVIOR_TOGGLE) ? (BOOL)g_modifierToggleArmed : IsModifierActive();
        }

        BOOL isReady = FALSE;
        if (g_settings.enableWiggle == WIGGLE_ALWAYS || g_settings.enableWiggle == WIGGLE_MODIFIER) {
            isReady = g_wiggleArmed;
        } else {
            isReady = modifierValid;
        }

        if (!g_recording && IsDrawButtonDown(wParam, ms) && isReady) {
            BOOL conflict = FALSE;
            if (g_pickerActive) {
                BOOL gestureUsesLeftOrRight = (g_settings.drawButton == DRAW_LEFT || g_settings.drawButton == DRAW_RIGHT);
                if (gestureUsesLeftOrRight && (g_settings.modifierFlags == 0)) {
                    conflict = TRUE;
                }
            }
            if (g_drawModeActive) {
                BOOL gestureUsesLeft = (g_settings.drawButton == DRAW_LEFT);
                if (gestureUsesLeft && (g_settings.modifierFlags == 0)) {
                    conflict = TRUE;
                }
            }

            if (!conflict) {
                g_wiggleArmed = FALSE;
                g_modifierToggleArmed = FALSE;
                g_gestureActive = TRUE;
                if (g_msgWnd) {
                    PostMessage(g_msgWnd, WM_HOOK_HIDE_AURA, 0, 0);
                    PostMessage(g_msgWnd, WM_HOOK_START_GESTURE, 0, MAKELPARAM(ms->pt.x, ms->pt.y));
                }
                return 1;
            }
        }

        if (g_pickerActive) {
            POINT pt = ms->pt;
            if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONUP) {
                g_pickerActive = FALSE;
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_MOUSE_PICKER, wParam, MAKELPARAM(pt.x, pt.y));
                return 1;
            }
            if (wParam == WM_RBUTTONDOWN) {
                return 1;
            }
            if (wParam == WM_MOUSEMOVE) {
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_MOUSE_PICKER, wParam, MAKELPARAM(pt.x, pt.y));
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            return 1; // Swallow all other mouse inputs during color pick
        }

        if (g_drawModeActive) {
            POINT pt = ms->pt;
            BOOL overStopwatch = FALSE;
            if (!g_drawDrawing && g_stopwatchWnd && IsWindowVisible(g_stopwatchWnd)) {
                RECT rcStopwatch;
                if (GetWindowRect(g_stopwatchWnd, &rcStopwatch)) {
                    if (PtInRect(&rcStopwatch, pt)) {
                        overStopwatch = TRUE;
                    }
                }
            }
            BOOL overTimer = FALSE;
            if (!g_drawDrawing && g_timerWnd && IsWindowVisible(g_timerWnd)) {
                RECT rcTimer;
                if (GetWindowRect(g_timerWnd, &rcTimer)) {
                    if (PtInRect(&rcTimer, pt)) {
                        overTimer = TRUE;
                    }
                }
            }
            BOOL overPalette = FALSE;
            if (!g_drawDrawing && g_paletteY > -60.0f) {
                HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                GetMonitorInfo(hMonitor, &mi);
                int monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
                int pW = 500;
                int pH = 50;
                int px = mi.rcMonitor.left + (monitorWidth - pW) / 2;
                int py = mi.rcMonitor.top + (int)g_paletteY;
                RECT paletteRect = { px, py, px + pW, py + pH };
                if (PtInRect(&paletteRect, pt)) {
                    overPalette = TRUE;
                }
            }
            if (overStopwatch || overTimer || overPalette) {
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            if (wParam == WM_MOUSEMOVE) {
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_MOUSE_DRAW, wParam, MAKELPARAM(pt.x, pt.y));
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            if (wParam == WM_RBUTTONUP) {
                g_drawModeActive = FALSE;
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_MOUSE_DRAW, wParam, MAKELPARAM(pt.x, pt.y));
                return 1;
            }
            if (wParam == WM_LBUTTONDOWN || wParam == WM_MBUTTONDOWN || 
                wParam == WM_LBUTTONUP || wParam == WM_MBUTTONUP ||
                wParam == WM_RBUTTONDOWN) {
                if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_MOUSE_DRAW, wParam, MAKELPARAM(pt.x, pt.y));
                return 1;
            }
            return 1;
        }

        if (wParam == WM_XBUTTONUP) {
            DWORD btn = HIWORD(ms->mouseData);
            if (btn == XBUTTON1 || btn == XBUTTON2) {
                if (g_gestureActive) {
                    g_gestureActive = FALSE;
                    if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_END_GESTURE, 0, MAKELPARAM(ms->pt.x, ms->pt.y));
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

void ToggleDrawMode() {
    if (g_drawModeWnd) {
        g_drawModeActive = FALSE;
        DestroyWindow(g_drawModeWnd);
        g_drawModeWnd = nullptr;
    } else {
        g_drawModeActive = TRUE;
        g_drawStrokeCount = 0;
        g_drawDrawing = FALSE;

        int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int y = GetSystemMetrics(SM_YVIRTUALSCREEN);

        g_drawModeWnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            DRAW_MODE_CLASS, L"",
            WS_POPUP,
            x, y, cx, cy,
            NULL, NULL, GetModuleHandle(NULL), NULL);

        if (!g_drawModeWnd) {
            g_drawModeActive = FALSE;
            return;
        }

        ShowWindow(g_drawModeWnd, SW_SHOWNOACTIVATE);
    }
}

void StartColorPicker() {
    if (g_pickerActive) return;
    g_pickerActive = TRUE;

    POINT pt;
    GetCursorPos(&pt);
    g_pickerPos = pt;

    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMonitor, &mi);

    int wx = pt.x + 15;
    int wy = pt.y + 15;
    if (wx + 160 > mi.rcMonitor.right) wx = pt.x - 160 - 15;
    if (wy + 160 > mi.rcMonitor.bottom) wy = pt.y - 160 - 15;
    if (wx < mi.rcMonitor.left) wx = mi.rcMonitor.left;
    if (wy < mi.rcMonitor.top) wy = mi.rcMonitor.top;

    g_pickerWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        PICKER_CLASS, L"",
        WS_POPUP,
        wx, wy, 160, 160,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!g_pickerWnd) {
        g_pickerActive = FALSE;
        return;
    }

    ShowWindow(g_pickerWnd, SW_SHOWNOACTIVATE);
}

void StopColorPicker() {
    g_pickerActive = FALSE;
    if (g_pickerWnd) {
        DestroyWindow(g_pickerWnd);
        g_pickerWnd = nullptr;
    }
}

LRESULT CALLBACK DrawModeProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            SetTimer(hwnd, 1, 16, NULL);
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, 1);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == 1) {
                POINT pt;
                GetCursorPos(&pt);
                HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                GetMonitorInfo(hMonitor, &mi);

                int monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
                int paletteW = 500;
                int paletteX1 = mi.rcMonitor.left + (monitorWidth - paletteW) / 2;
                int paletteX2 = paletteX1 + paletteW;

                BOOL hover = (pt.x >= paletteX1 && pt.x <= paletteX2 && pt.y >= mi.rcMonitor.top && pt.y <= mi.rcMonitor.top + 70);
                if (g_drawDrawing) hover = FALSE;
                
                if (hover) {
                    g_paletteVisible = TRUE;
                    if (g_paletteY < 15.0f) {
                        g_paletteY += 4.0f;
                        if (g_paletteY > 15.0f) g_paletteY = 15.0f;
                    }
                } else {
                    g_paletteVisible = FALSE;
                    if (g_paletteY > -70.0f) {
                        g_paletteY -= 4.0f;
                        if (g_paletteY < -70.0f) g_paletteY = -70.0f;
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONDOWN: {
            POINT ptCursor;
            GetCursorPos(&ptCursor);
            HMONITOR hMonitor = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            GetMonitorInfo(hMonitor, &mi);
            
            POINT wndOrigin = {0, 0};
            ClientToScreen(hwnd, &wndOrigin);
            
            int monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
            int pW = 500;
            int pH = 50;
            int px = mi.rcMonitor.left + (monitorWidth - pW) / 2 - wndOrigin.x;
            int py = mi.rcMonitor.top + (int)g_paletteY - wndOrigin.y;
            
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            RECT paletteRect = { px, py, px + pW, py + pH };
            if (g_paletteY > -60.0f && PtInRect(&paletteRect, pt)) {
                int cx = pt.x - px;
                int cy = pt.y - py;

                int colorsXStart = 20;
                int colorsSpacing = 32;
                Gdiplus::Color presetColors[] = {
                    Gdiplus::Color(255, 255, 50, 50),
                    Gdiplus::Color(255, 255, 150, 0),
                    Gdiplus::Color(255, 255, 220, 0),
                    Gdiplus::Color(255, 50, 220, 50),
                    Gdiplus::Color(255, 0, 170, 255),
                    Gdiplus::Color(255, 50, 80, 255),
                    Gdiplus::Color(255, 255, 80, 180),
                    Gdiplus::Color(0, 0, 0, 0)
                };

                for (int i = 0; i < 8; i++) {
                    int bx = colorsXStart + i * colorsSpacing;
                    int by = 25;
                    int dx = cx - bx;
                    int dy = cy - by;
                    if (dx*dx + dy*dy <= 100) {
                        g_drawColor = presetColors[i];
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                }

                int sizesXStart = 290;
                int sizesSpacing = 30;
                float presetWidths[] = { 2.0f, 5.0f, 10.0f };
                for (int i = 0; i < 3; i++) {
                    int bx = sizesXStart + i * sizesSpacing;
                    int by = 25;
                    int dx = cx - bx;
                    int dy = cy - by;
                    if (dx*dx + dy*dy <= 100) {
                        g_drawWidth = presetWidths[i];
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                }

                int clearBx = 410;
                int clearBy = 25;
                if ((cx - clearBx)*(cx - clearBx) + (cy - clearBy)*(cy - clearBy) <= 225) {
                    g_drawStrokeCount = 0;
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }

                int closeBx = 460;
                int closeBy = 25;
                if ((cx - closeBx)*(cx - closeBx) + (cy - closeBy)*(cy - closeBy) <= 225) {
                    g_drawModeActive = FALSE;
                    DestroyWindow(hwnd);
                    g_drawModeWnd = nullptr;
                    return 0;
                }
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HDC memDC = CreateCompatibleDC(hdc);
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = rc.right;
            bmi.bmiHeader.biHeight = -rc.bottom;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            void* bits = nullptr;
            HBITMAP memBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
            if (memBitmap && bits) {
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
                memset(bits, 0, rc.right * rc.bottom * 4);

            using namespace Gdiplus;
            Graphics graphics(memDC);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);

            for (int s = 0; s < g_drawStrokeCount; s++) {
                DrawStroke* stroke = &g_drawStrokes[s];
                if (stroke->count < 2) continue;

                PointF* pts = new PointF[stroke->count];
                for (int i = 0; i < stroke->count; i++) {
                    pts[i].X = stroke->points[i].x;
                    pts[i].Y = stroke->points[i].y;
                }

                Pen pen(stroke->color, stroke->width);
                pen.SetStartCap(LineCapRound);
                pen.SetEndCap(LineCapRound);
                pen.SetLineJoin(LineJoinRound);

                if (stroke->color.GetAlpha() == 0) {
                    graphics.SetCompositingMode(CompositingModeSourceCopy);
                } else {
                    graphics.SetCompositingMode(CompositingModeSourceOver);
                }

                graphics.DrawLines(&pen, pts, stroke->count);
                delete[] pts;
            }

            if (g_drawDrawing && g_currentDrawStroke.count >= 2) {
                PointF* pts = new PointF[g_currentDrawStroke.count];
                for (int i = 0; i < g_currentDrawStroke.count; i++) {
                    pts[i].X = g_currentDrawStroke.points[i].x;
                    pts[i].Y = g_currentDrawStroke.points[i].y;
                }

                Pen pen(g_currentDrawStroke.color, g_currentDrawStroke.width);
                pen.SetStartCap(LineCapRound);
                pen.SetEndCap(LineCapRound);
                pen.SetLineJoin(LineJoinRound);

                if (g_currentDrawStroke.color.GetAlpha() == 0) {
                    graphics.SetCompositingMode(CompositingModeSourceCopy);
                } else {
                    graphics.SetCompositingMode(CompositingModeSourceOver);
                }

                graphics.DrawLines(&pen, pts, g_currentDrawStroke.count);
                delete[] pts;
            }

            if (g_paletteY > -60.0f) {
                graphics.SetCompositingMode(CompositingModeSourceOver);
                int pW = 500;
                int pH = 50;
                
                POINT ptCursor;
                GetCursorPos(&ptCursor);
                HMONITOR hMonitor = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                GetMonitorInfo(hMonitor, &mi);
                
                POINT wndOrigin = {0, 0};
                ClientToScreen(hwnd, &wndOrigin);
                
                int monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
                int px = mi.rcMonitor.left + (monitorWidth - pW) / 2 - wndOrigin.x;
                int py = mi.rcMonitor.top + (int)g_paletteY - wndOrigin.y;

                SolidBrush bgBrush(Color(200, 20, 20, 30));
                Pen borderPen(Color(100, 255, 255, 255), 1.5f);
                GraphicsPath path;
                int r = 25;
                path.AddArc(px, py, r*2, r*2, 180, 90);
                path.AddArc(px + pW - r*2, py, r*2, r*2, 270, 90);
                path.AddArc(px + pW - r*2, py + pH - r*2, r*2, r*2, 0, 90);
                path.AddArc(px, py + pH - r*2, r*2, r*2, 90, 90);
                path.CloseFigure();
                graphics.FillPath(&bgBrush, &path);
                graphics.DrawPath(&borderPen, &path);

                Color presetColors[] = {
                    Color(255, 255, 50, 50),
                    Color(255, 255, 150, 0),
                    Color(255, 255, 220, 0),
                    Color(255, 50, 220, 50),
                    Color(255, 0, 170, 255),
                    Color(255, 50, 80, 255),
                    Color(255, 255, 80, 180),
                    Color(255, 255, 255, 255)
                };

                int colorsXStart = 20;
                int colorsSpacing = 32;
                for (int i = 0; i < 8; i++) {
                    int bx = px + colorsXStart + i * colorsSpacing;
                    int by = py + 25;

                    if (i == 7) {
                        GraphicsState state = graphics.Save();
                        graphics.TranslateTransform((REAL)bx, (REAL)by);
                        graphics.RotateTransform(-30.0f);
                        SolidBrush eraserPink(Color(255, 255, 150, 150));
                        SolidBrush eraserWhite(Color(255, 240, 240, 240));
                        Pen eraserBorder(Color(255, 100, 100, 100), 1.0f);
                        graphics.FillRectangle(&eraserPink, -9.0f, -6.0f, 9.0f, 12.0f);
                        graphics.FillRectangle(&eraserWhite, 0.0f, -6.0f, 9.0f, 12.0f);
                        graphics.DrawRectangle(&eraserBorder, -9.0f, -6.0f, 18.0f, 12.0f);
                        graphics.Restore(state);
                    } else {
                        SolidBrush colorBrush(presetColors[i]);
                        graphics.FillEllipse(&colorBrush, bx - 10, by - 10, 20, 20);
                    }

                    BOOL isSelected = FALSE;
                    if (i == 7) {
                        isSelected = (g_drawColor.GetAlpha() == 0);
                    } else {
                        isSelected = (g_drawColor.GetValue() == presetColors[i].GetValue() && g_drawColor.GetAlpha() != 0);
                    }

                    if (isSelected) {
                        Pen selPen(Color(255, 255, 255, 255), 2.0f);
                        graphics.DrawEllipse(&selPen, bx - 12, by - 12, 24, 24);
                    }
                }

                int sizesXStart = 290;
                int sizesSpacing = 30;
                float presetWidths[] = { 2.0f, 5.0f, 10.0f };
                for (int i = 0; i < 3; i++) {
                    int bx = px + sizesXStart + i * sizesSpacing;
                    int by = py + 25;

                    SolidBrush brushBrush(Color(200, 200, 200, 200));
                    int sizeRadius = 3 + i * 2;
                    graphics.FillEllipse(&brushBrush, bx - sizeRadius, by - sizeRadius, sizeRadius * 2, sizeRadius * 2);

                    if (g_drawWidth == presetWidths[i]) {
                        Pen selPen(Color(255, 255, 255, 255), 2.0f);
                        graphics.DrawEllipse(&selPen, bx - 12, by - 12, 24, 24);
                    }
                }

                int clearBx = px + 410;
                int clearBy = py + 25;
                SolidBrush btnBg(Color(200, 40, 40, 50));
                graphics.FillEllipse(&btnBg, clearBx - 14, clearBy - 14, 28, 28);
                Pen trashPen(Color(255, 220, 220, 220), 1.5f);
                graphics.DrawRectangle(&trashPen, (REAL)(clearBx - 5), (REAL)(clearBy - 2), 10.0f, 10.0f);
                graphics.DrawLine(&trashPen, (REAL)(clearBx - 7), (REAL)(clearBy - 2), (REAL)(clearBx + 7), (REAL)(clearBy - 2));
                graphics.DrawRectangle(&trashPen, (REAL)(clearBx - 2), (REAL)(clearBy - 4), 4.0f, 2.0f);
                graphics.DrawLine(&trashPen, (REAL)(clearBx - 2), (REAL)(clearBy + 1), (REAL)(clearBx - 2), (REAL)(clearBy + 5));
                graphics.DrawLine(&trashPen, (REAL)(clearBx + 2), (REAL)(clearBy + 1), (REAL)(clearBx + 2), (REAL)(clearBy + 5));

                int closeBx = px + 460;
                int closeBy = py + 25;
                graphics.FillEllipse(&btnBg, closeBx - 14, closeBy - 14, 28, 28);
                Pen closePen(Color(255, 255, 80, 80), 2.0f);
                graphics.DrawLine(&closePen, (REAL)(closeBx - 5), (REAL)(closeBy - 5), (REAL)(closeBx + 5), (REAL)(closeBy + 5));
                graphics.DrawLine(&closePen, (REAL)(closeBx - 5), (REAL)(closeBy + 5), (REAL)(closeBx + 5), (REAL)(closeBy - 5));
            }

            POINT ptCursor;
            GetCursorPos(&ptCursor);
            POINT wndOrigin = {0, 0};
            ClientToScreen(hwnd, &wndOrigin);
            float curX = (float)(ptCursor.x - wndOrigin.x);
            float curY = (float)(ptCursor.y - wndOrigin.y);
            float curW = (g_drawColor.GetAlpha() == 0) ? g_drawWidth * 4.0f : g_drawWidth;
            if (curW < 2.0f) curW = 2.0f;

            Pen cursorPen(Color(150, 255, 255, 255), 1.5f);
            if (g_drawColor.GetAlpha() == 0) {
                 cursorPen.SetColor(Color(255, 255, 50, 50));
            }
            graphics.DrawEllipse(&cursorPen, curX - curW/2.0f, curY - curW/2.0f, curW, curW);

            POINT ptSrc = { 0, 0 };
            POINT ptDst = { rc.left, rc.top };
            SIZE sizeWnd = { rc.right, rc.bottom };
            BLENDFUNCTION blend = {};
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;

                UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

                SelectObject(memDC, oldBitmap);
                DeleteObject(memBitmap);
            }
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK ColorPickerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            SetTimer(hwnd, 1, 16, NULL);
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, 1);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == 1) {
                POINT pt;
                GetCursorPos(&pt);
                g_pickerPos = pt;
                
                HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(mi) };
                GetMonitorInfo(hMonitor, &mi);

                int wx = pt.x + 15;
                int wy = pt.y + 15;
                if (wx + 160 > mi.rcMonitor.right) wx = pt.x - 160 - 15;
                if (wy + 160 > mi.rcMonitor.bottom) wy = pt.y - 160 - 15;
                if (wx < mi.rcMonitor.left) wx = mi.rcMonitor.left;
                if (wy < mi.rcMonitor.top) wy = mi.rcMonitor.top;

                SetWindowPos(hwnd, HWND_TOPMOST, wx, wy, 160, 160, SWP_NOACTIVATE);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HDC memDC = CreateCompatibleDC(hdc);
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = 160;
            bmi.bmiHeader.biHeight = -160;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            void* bits = nullptr;
            HBITMAP memBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
            if (memBitmap && bits) {
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
                memset(bits, 0, 160 * 160 * 4);

            HDC hdcScreen = GetDC(NULL);
            HDC capDC = CreateCompatibleDC(hdcScreen);
            HBITMAP capBitmap = CreateCompatibleBitmap(hdcScreen, 16, 16);
            HBITMAP oldCap = (HBITMAP)SelectObject(capDC, capBitmap);

            BitBlt(capDC, 0, 0, 16, 16, hdcScreen, g_pickerPos.x - 8, g_pickerPos.y - 8, SRCCOPY);
            COLORREF pickedColor = GetPixel(hdcScreen, g_pickerPos.x, g_pickerPos.y);

            using namespace Gdiplus;
            Graphics graphics(memDC);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);

            GraphicsPath path;
            path.AddEllipse(5, 5, 150, 150);
            graphics.SetClip(&path);

            Bitmap capBmp(capBitmap, NULL);
            graphics.DrawImage(&capBmp, 5, 5, 150, 150);

            graphics.ResetClip();

            Pen gridPen(Color(128, 255, 255, 255), 1.0f);
            graphics.DrawRectangle(&gridPen, 80.0f - 4.6f, 80.0f - 4.6f, 9.3f, 9.3f);

            Pen crossPen(Color(180, 255, 255, 255), 1.0f);
            graphics.DrawLine(&crossPen, 80.0f, 5.0f, 80.0f, 70.0f);
            graphics.DrawLine(&crossPen, 80.0f, 90.0f, 80.0f, 155.0f);
            graphics.DrawLine(&crossPen, 5.0f, 80.0f, 70.0f, 80.0f);
            graphics.DrawLine(&crossPen, 90.0f, 80.0f, 155.0f, 80.0f);

            Color indicatorColor(255, GetRValue(pickedColor), GetGValue(pickedColor), GetBValue(pickedColor));
            Pen borderPen(indicatorColor, 6.0f);
            graphics.DrawEllipse(&borderPen, 8, 8, 144, 144);

            Pen contrastPen(Color(255, 255, 255, 255), 2.0f);
            graphics.DrawEllipse(&contrastPen, 5, 5, 150, 150);

            wchar_t hexStr[32];
            swprintf_s(hexStr, L"#%02X%02X%02X", GetRValue(pickedColor), GetGValue(pickedColor), GetBValue(pickedColor));

            SolidBrush badgeBrush(Color(200, 20, 20, 20));
            graphics.FillRectangle(&badgeBrush, 40.0f, 115.0f, 80.0f, 22.0f);
            Pen badgeBorder(Color(150, 255, 255, 255), 1.0f);
            graphics.DrawRectangle(&badgeBorder, 40.0f, 115.0f, 80.0f, 22.0f);

            FontFamily fontFamily(L"Segoe UI");
            Font font(&fontFamily, 9.0f, FontStyleBold, UnitPoint);
            SolidBrush textBrush(Color(255, 255, 255, 255));
            StringFormat sf;
            sf.SetAlignment(StringAlignmentCenter);
            sf.SetLineAlignment(StringAlignmentCenter);
            RectF layoutRect(40.0f, 115.0f, 80.0f, 22.0f);
            graphics.DrawString(hexStr, -1, &font, layoutRect, &sf, &textBrush);

            SelectObject(capDC, oldCap);
            DeleteObject(capBitmap);
            DeleteDC(capDC);
            ReleaseDC(NULL, hdcScreen);

            POINT ptSrc = { 0, 0 };
            RECT rcWnd;
            GetWindowRect(hwnd, &rcWnd);
            POINT ptDst = { rcWnd.left, rcWnd.top };
            SIZE sizeWnd = { 160, 160 };
            BLENDFUNCTION blend = {};
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;

                UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

                SelectObject(memDC, oldBitmap);
                DeleteObject(memBitmap);
            }
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_KEYUP || wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP)) {
        UpdateModifierToggle();

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
            if (kb->vkCode == VK_ESCAPE) {
                if (g_spotlightState == 2 || g_spotlightState == 1) g_spotlightState = 3;
            }
        }

        if (g_timerEditMode) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
                if (g_msgWnd) {
                    PostMessage(g_msgWnd, WM_HOOK_KEY_TIMER_EDIT, kb->vkCode, 0);
                }
            }
            return 1;
        }

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;

            if (g_flashWnd && IsWindowVisible(g_flashWnd)) {
                if (g_msgWnd) {
                    PostMessage(g_msgWnd, WM_HOOK_TOGGLE_FLASH, 0, 0);
                }
                return 1;
            }

            if (kb->vkCode == VK_ESCAPE) {
                if (g_pickerActive) {
                    g_pickerActive = FALSE;
                    if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_STOP_PICKER, 0, 0);
                    return 1;
                }
                if (g_drawModeActive) {
                    g_drawModeActive = FALSE;
                    if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_TOGGLE_DRAW, 0, 0);
                    return 1;
                }
            }

            if (kb->vkCode == g_settings.recordVk) {
                BOOL modOk = TRUE;
                if (g_settings.recordModifiers & MOD_CTRL_KEY)
                    modOk = modOk && (GetAsyncKeyState(VK_CONTROL) & 0x8000);
                if (g_settings.recordModifiers & MOD_SHIFT_KEY)
                    modOk = modOk && (GetAsyncKeyState(VK_SHIFT) & 0x8000);
                if (g_settings.recordModifiers & MOD_ALT_KEY)
                    modOk = modOk && (GetAsyncKeyState(VK_MENU) & 0x8000);

                if (modOk) {
                    if (g_msgWnd) PostMessage(g_msgWnd, WM_HOOK_RECORD_CANVAS, 0, 0);
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK ToastProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == 1) {
        DWORD elapsed = GetTickCount() - g_toastStartTick;
        if (elapsed > TOAST_DURATION_MS) {
            KillTimer(hwnd, 1);
            DestroyWindow(hwnd);
            g_toastWnd = nullptr;
            return 0;
        }

        // Constantly enforce topmost to bypass newly elevated windows
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        int alpha = 230; // Max opacity slightly translucent
        if (elapsed > TOAST_DURATION_MS - 200) {
            alpha = (int)(230.0f * (1.0f - ((float)(elapsed - (TOAST_DURATION_MS - 200)) / 200.0f)));
        }

        HDC screenDC = GetDC(NULL);
        HDC memDC = CreateCompatibleDC(screenDC);
        
        using namespace Gdiplus;
        FontFamily fontFamily(L"Segoe UI");
        Font font(&fontFamily, 14, FontStyleBold, UnitPoint);
        
        int w = 200;
        int h = 40;
        {
            Graphics measureGraphics(screenDC);
            RectF textRect;
            measureGraphics.MeasureString(g_toastText, -1, &font, PointF(0.0f, 0.0f), &textRect);
            w = (int)textRect.Width + 40;
        }

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HBITMAP memBitmap = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        Graphics g(memDC);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.Clear(Color(0, 0, 0, 0));

        float fw = (float)w;
        float fh = (float)h;
        SolidBrush bgBrush(Color(255, 25, 25, 25));
        
        // Draw a pill shape (two circles and a connecting rectangle)
        float radius = fh / 2.0f;
        g.FillEllipse(&bgBrush, 0.0f, 0.0f, fh, fh);
        g.FillEllipse(&bgBrush, fw - fh, 0.0f, fh, fh);
        g.FillRectangle(&bgBrush, radius, 0.0f, fw - fh, fh);

        Color textColor = g_toastIsSuccess ? Color(255, 100, 255, 100) : Color(255, 255, 100, 100);
        SolidBrush textBrush(textColor);
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(g_toastText, -1, &font, RectF(0.0f, 0.0f, fw, fh), &format, &textBrush);

        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = alpha;
        blend.AlphaFormat = AC_SRC_ALPHA;

        POINT ptSrc = { 0, 0 };
        SIZE sizeWnd = { w, h };
        
        RECT rc;
        GetWindowRect(hwnd, &rc);
        POINT ptDst = { rc.left, rc.top };

        UpdateLayeredWindow(hwnd, screenDC, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);
        ReleaseDC(NULL, screenDC);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowToast(const wchar_t* text, BOOL isSuccess, POINT pt) {
    if (g_toastWnd) {
        DestroyWindow(g_toastWnd);
        g_toastWnd = nullptr;
    }

    wcsncpy_s(g_toastText, text, _TRUNCATE);
    g_toastIsSuccess = isSuccess;
    g_toastStartTick = GetTickCount();

    // Approximate width for initial placement
    int approxWidth = (wcslen(text) * 10) + 40;
    int x = pt.x + 20;
    int y = pt.y - 20;

    g_toastWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        TOAST_CLASS, L"",
        WS_POPUP,
        x, y, approxWidth, 40,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (g_toastWnd) {
        SetTimer(g_toastWnd, 1, 16, NULL);
        ShowWindow(g_toastWnd, SW_SHOWNA);
    }
}

LRESULT CALLBACK AuraProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == 1) {
        DWORD auraTimeout = (g_settings.armTimeout > 0) ? (DWORD)g_settings.armTimeout : 3000;
        if (g_wiggleArmed && (GetTickCount() - g_wiggleArmTime > auraTimeout)) {
            g_wiggleArmed = FALSE;
            KillTimer(hwnd, 1);
            DestroyWindow(hwnd);
            g_auraWnd = nullptr;
            return 0;
        }

        POINT pt;
        GetCursorPos(&pt);
        
        int x = pt.x - 30;
        int y = pt.y - 30;

        HDC screenDC = GetDC(NULL);
        HDC memDC = CreateCompatibleDC(screenDC);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = 60;
        bmi.bmiHeader.biHeight = -60;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HBITMAP memBitmap = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        using namespace Gdiplus;
        Graphics graphics(memDC);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.Clear(Color(0, 0, 0, 0));

        double pulse = (sin(GetTickCount() * 0.01) + 1.0) / 2.0;
        
        int r = 138 + (int)((220 - 138) * pulse);
        int g = 43 + (int)((20 - 43) * pulse);
        int b = 226 + (int)((60 - 226) * pulse);
        
        Color c(255, r, g, b);
        Pen pen(c, 3.0f + 2.0f * (float)pulse);
        graphics.DrawEllipse(&pen, 4, 4, 52, 52);

        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;

        POINT ptSrc = { 0, 0 };
        SIZE sizeWnd = { 60, 60 };
        POINT ptDst = { x, y };

        UpdateLayeredWindow(hwnd, screenDC, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);
        ReleaseDC(NULL, screenDC);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowAura(POINT pt) {
    if (g_auraWnd) return;
    g_auraWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        AURA_CLASS, L"",
        WS_POPUP,
        pt.x - 30, pt.y - 30, 60, 60,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(g_auraWnd, SW_SHOWNOACTIVATE);
    SetTimer(g_auraWnd, 1, 16, NULL);
}

void UpdateAura(POINT pt) {
    // Window position is handled by the timer for smoothness
}

void HideAura() {
    if (g_auraWnd) {
        KillTimer(g_auraWnd, 1);
        DestroyWindow(g_auraWnd);
        g_auraWnd = nullptr;
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    OleInitialize(NULL);

    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = OVERLAY_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    RegisterClass(&wc);

    WNDCLASS cwc = {};
    cwc.lpfnWndProc = CanvasProc;
    cwc.hInstance = GetModuleHandle(NULL);
    cwc.lpszClassName = CANVAS_CLASS;
    cwc.hCursor = LoadCursor(NULL, IDC_CROSS);
    cwc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&cwc);

    WNDCLASS dwc = {};
    dwc.lpfnWndProc = DrawModeProc;
    dwc.hInstance = GetModuleHandle(NULL);
    dwc.lpszClassName = DRAW_MODE_CLASS;
    dwc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&dwc);

    WNDCLASS pwc = {};
    pwc.lpfnWndProc = ColorPickerProc;
    pwc.hInstance = GetModuleHandle(NULL);
    pwc.lpszClassName = PICKER_CLASS;
    pwc.hCursor = LoadCursor(NULL, IDC_CROSS);
    RegisterClass(&pwc);

    WNDCLASS awc = {};
    awc.lpfnWndProc = AuraProc;
    awc.hInstance = GetModuleHandle(NULL);
    awc.lpszClassName = AURA_CLASS;
    awc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&awc);

    WNDCLASS twc = {};
    twc.lpfnWndProc = ToastProc;
    twc.hInstance = GetModuleHandle(NULL);
    twc.lpszClassName = TOAST_CLASS;
    RegisterClass(&twc);

    g_uiFont = CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

    WNDCLASS mwc = {};
    mwc.lpfnWndProc = MsgWndProc;
    mwc.hInstance = GetModuleHandle(NULL);
    mwc.lpszClassName = MSG_WND_CLASS;
    RegisterClass(&mwc);

    g_msgWnd = CreateWindowEx(0, MSG_WND_CLASS, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, GetModuleHandle(NULL), NULL);

    MSG msg;
    while (g_running.load() && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_msgWnd) {
        DestroyWindow(g_msgWnd);
        g_msgWnd = nullptr;
    }

    HideCanvas();
    DestroyOverlay();
    if (g_drawModeActive) ToggleDrawMode();
    if (g_pickerActive) StopColorPicker();

    if (g_flashWnd && IsWindowVisible(g_flashWnd)) { RestoreMonitorBrightness(); }
    if (g_flashWnd) { DestroyWindow(g_flashWnd); g_flashWnd = nullptr; }
    if (g_stopwatchWnd) { KillTimer(g_stopwatchWnd, 1); DestroyWindow(g_stopwatchWnd); g_stopwatchWnd = nullptr; }
    if (g_timerWnd) { KillTimer(g_timerWnd, 1); DestroyWindow(g_timerWnd); g_timerWnd = nullptr; }

    if (g_uiFont) { DeleteObject(g_uiFont); g_uiFont = nullptr; }

    UnregisterClass(MSG_WND_CLASS, GetModuleHandle(NULL));
    UnregisterClass(OVERLAY_CLASS, GetModuleHandle(NULL));
    UnregisterClass(CANVAS_CLASS, GetModuleHandle(NULL));
    UnregisterClass(DRAW_MODE_CLASS, GetModuleHandle(NULL));
    UnregisterClass(PICKER_CLASS, GetModuleHandle(NULL));
    UnregisterClass(AURA_CLASS, GetModuleHandle(NULL));
    UnregisterClass(TOAST_CLASS, GetModuleHandle(NULL));
    UnregisterClass(L"MagicMouseSpotlight", GetModuleHandle(NULL));
    UnregisterClass(NOTE_SEL_CLASS, GetModuleHandle(NULL));

    UnregisterClass(STOPWATCH_CLASS, GetModuleHandle(NULL));
    UnregisterClass(TIMER_CLASS, GetModuleHandle(NULL));
    UnregisterClass(FLASH_CLASS, GetModuleHandle(NULL));

    Gdiplus::GdiplusShutdown(gdiplusToken);
    OleUninitialize();
    return 0;
}

DWORD WINAPI HookThreadProc(LPVOID lpParam) {
    OleInitialize(NULL);

    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    if (!g_mouseHook || !g_keyboardHook) {
        Wh_Log(L"Failed to install hooks (mouse=%p keyboard=%p)", g_mouseHook, g_keyboardHook);
    } else {
        Wh_Log(L"Hooks installed successfully");
    }

    MSG msg;
    while (g_running.load() && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_keyboardHook) { UnhookWindowsHookEx(g_keyboardHook); g_keyboardHook = nullptr; }

    OleUninitialize();
    return 0;
}

void LoadSettings() {
    PCWSTR modKey = Wh_GetStringSetting(L"ModifierKey");
    ParseModifierString(modKey, &g_settings.modifierFlags);
    Wh_FreeStringSetting(modKey);

    PCWSTR drawBtn = Wh_GetStringSetting(L"DrawButton");
    g_settings.drawButton = ParseDrawButton(drawBtn);
    Wh_FreeStringSetting(drawBtn);

    PCWSTR hotkey = Wh_GetStringSetting(L"RecordHotkey");
    ParseHotkeyString(hotkey, &g_settings.recordVk, &g_settings.recordModifiers);
    Wh_FreeStringSetting(hotkey);

    PCWSTR colorStr = Wh_GetStringSetting(L"TrailColor");
    if (colorStr) {
        g_settings.trailColor = ParseHexColor(colorStr);
        Wh_FreeStringSetting(colorStr);
    }

    g_settings.allowInFullscreen = Wh_GetIntSetting(L"AllowInFullscreen");

    PCWSTR fsList = Wh_GetStringSetting(L"FullscreenIncludeList");
    if (fsList) {
        wcsncpy_s(g_settings.fullscreenIncludeList, fsList, _TRUNCATE);
        Wh_FreeStringSetting(fsList);
    } else {
        g_settings.fullscreenIncludeList[0] = L'\0';
    }

    PCWSTR exList = Wh_GetStringSetting(L"FullscreenExcludeList");
    if (exList) {
        wcsncpy_s(g_settings.fullscreenExcludeList, exList, _TRUNCATE);
        Wh_FreeStringSetting(exList);
    } else {
        g_settings.fullscreenExcludeList[0] = L'\0';
    }

    g_settings.trailWidth = Wh_GetIntSetting(L"TrailWidth");
    if (g_settings.trailWidth < 1) g_settings.trailWidth = 1;
    if (g_settings.trailWidth > 20) g_settings.trailWidth = 20;

    g_settings.armTimeout = Wh_GetIntSetting(L"ArmTimeout");
    if (g_settings.armTimeout < 0) g_settings.armTimeout = 0;

    g_settings.showAura = Wh_GetIntSetting(L"ShowAura");

    PCWSTR modBehaviorStr = Wh_GetStringSetting(L"ModifierBehavior");
    if (modBehaviorStr && wcscmp(modBehaviorStr, L"toggle") == 0) {
        g_settings.modifierBehavior = MOD_BEHAVIOR_TOGGLE;
    } else {
        g_settings.modifierBehavior = MOD_BEHAVIOR_HOLD;
    }
    if (modBehaviorStr) Wh_FreeStringSetting(modBehaviorStr);

    PCWSTR wiggleStr = Wh_GetStringSetting(L"EnableWiggleToActivate");
    if (wiggleStr) {
        if (wcscmp(wiggleStr, L"always") == 0) g_settings.enableWiggle = WIGGLE_ALWAYS;
        else if (wcscmp(wiggleStr, L"modifier") == 0) g_settings.enableWiggle = WIGGLE_MODIFIER;
        else g_settings.enableWiggle = WIGGLE_NEVER;
        Wh_FreeStringSetting(wiggleStr);
    } else {
        g_settings.enableWiggle = WIGGLE_NEVER;
    }
    g_settings.wiggleStrength = Wh_GetIntSetting(L"WiggleStrength");
    if (g_settings.wiggleStrength < 10) g_settings.wiggleStrength = 10;
    if (g_settings.wiggleStrength > 200) g_settings.wiggleStrength = 200;

    int sens = Wh_GetIntSetting(L"GestureSensitivity");
    if (sens < 8) sens = 8;
    if (sens > 20) sens = 20;
    g_settings.matchThreshold = (double)sens;

    g_settings.minGestureDistance = 60;

    PCWSTR searchUrl = Wh_GetStringSetting(L"SearchEngineUrl");
    if (searchUrl && searchUrl[0]) {
        wcsncpy_s(g_settings.searchEngineUrl, searchUrl, _TRUNCATE);
    } else {
        wcscpy_s(g_settings.searchEngineUrl, L"https://www.google.com/search?q=");
    }
    if (searchUrl) Wh_FreeStringSetting(searchUrl);

    g_settings.gestureCount = 0;
    for (int i = 0; i < MAX_GESTURES; i++) {
        wchar_t key[128];

        swprintf_s(key, L"Gestures[%d].Name", i);
        PCWSTR name = Wh_GetStringSetting(key);
        if (!name || *name == L'\0') {
            Wh_FreeStringSetting(name);
            break;
        }
        wcsncpy_s(g_settings.gestures[i].name, name, _TRUNCATE);
        Wh_FreeStringSetting(name);

        swprintf_s(key, L"Gestures[%d].Action", i);
        PCWSTR action = Wh_GetStringSetting(key);
        g_settings.gestures[i].action = ParseAction(action);
        Wh_FreeStringSetting(action);

        swprintf_s(key, L"Gestures[%d].ActionParam", i);
        PCWSTR param = Wh_GetStringSetting(key);
        if (param) wcsncpy_s(g_settings.gestures[i].actionParam, param, _TRUNCATE);
        else g_settings.gestures[i].actionParam[0] = 0;
        Wh_FreeStringSetting(param);

        swprintf_s(key, L"Gestures[%d].DirectionSequence", i);
        PCWSTR seq = Wh_GetStringSetting(key);
        if (seq) wcsncpy_s(g_settings.gestures[i].directionSequence, seq, _TRUNCATE);
        else g_settings.gestures[i].directionSequence[0] = 0;
        Wh_FreeStringSetting(seq);


        g_settings.gestures[i].contextRuleCount = 0;
        for (int j = 0; j < 8; j++) {
            wchar_t subKey[256];
            swprintf_s(subKey, L"Gestures[%d].ContextRules[%d].ProcessMatch", i, j);
            PCWSTR match = Wh_GetStringSetting(subKey);
            if (!match || *match == L'\0') {
                Wh_FreeStringSetting(match);
                break;
            }
            wcsncpy_s(g_settings.gestures[i].contextRules[j].processMatch, match, _TRUNCATE);
            _wcslwr_s(g_settings.gestures[i].contextRules[j].processMatch);
            Wh_FreeStringSetting(match);

            swprintf_s(subKey, L"Gestures[%d].ContextRules[%d].OverrideAction", i, j);
            PCWSTR oAction = Wh_GetStringSetting(subKey);
            g_settings.gestures[i].contextRules[j].overrideAction = ParseAction(oAction);
            Wh_FreeStringSetting(oAction);

            swprintf_s(subKey, L"Gestures[%d].ContextRules[%d].OverrideParam", i, j);
            PCWSTR oParam = Wh_GetStringSetting(subKey);
            if (oParam) wcsncpy_s(g_settings.gestures[i].contextRules[j].overrideParam, oParam, _TRUNCATE);
            else g_settings.gestures[i].contextRules[j].overrideParam[0] = 0;
            Wh_FreeStringSetting(oParam);

            g_settings.gestures[i].contextRuleCount++;
        }

        g_settings.gestureCount++;
    }

    Wh_Log(L"Settings loaded: %d gestures, modifier=0x%x",
        g_settings.gestureCount, g_settings.modifierFlags);
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Mouse Gestures mod initializing...");

    LoadSettings();

    extern void LaunchNotesProcess();
    LaunchNotesProcess();

    g_running.store(true);

    g_workerThread = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread");
        g_running.store(false);
        return FALSE;
    }

    // Wait up to 1000ms for g_msgWnd to be initialized by the worker thread
    for (int i = 0; i < 100; i++) {
        if (g_msgWnd) break;
        Sleep(10);
    }

    g_hookThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &g_hookThreadId);

    if (!g_hookThread) {
        Wh_Log(L"Failed to create hook thread");
        g_running.store(false);
        if (g_workerThreadId) {
            PostThreadMessage(g_workerThreadId, WM_QUIT, 0, 0);
        }
        if (g_workerThread) {
            WaitForSingleObject(g_workerThread, 3000);
            CloseHandle(g_workerThread);
            g_workerThread = nullptr;
        }
        g_workerThreadId = 0;
        return FALSE;
    }

    Wh_Log(L"Mouse Gestures mod initialized successfully");
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Mouse Gestures mod uninitializing...");

    g_running.store(false);

    if (g_notesThreadId) {
        PostThreadMessage(g_notesThreadId, WM_QUIT, 0, 0);
    }

    if (g_notesThread) {
        WaitForSingleObject(g_notesThread, 3000);
        CloseHandle(g_notesThread);
        g_notesThread = nullptr;
    }

    if (g_hookThreadId) {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
    }

    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 3000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    if (g_workerThreadId) {
        PostThreadMessage(g_workerThreadId, WM_QUIT, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 3000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    g_hookThreadId = 0;
    g_workerThreadId = 0;
    g_notesThreadId = 0;
    Wh_Log(L"Mouse Gestures mod uninitialized");
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed...");
    if (g_gestureActive) {
        Wh_Log(L"Gesture active, skipping settings reload to avoid corruption.");
        return;
    }
    Wh_Log(L"Reloading settings.");
    LoadSettings();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

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

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") /
                       sizeof(WCHAR)) -
                      1];
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
        DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
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
                                 nullptr, nullptr, FALSE,
                                 NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si,
                                 &pi, nullptr)) {
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