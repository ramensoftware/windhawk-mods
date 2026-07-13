// ==WindhawkMod==
// @id              magic-mouse
// @name            Magic Mouse
// @description     Draw custom mouse gestures to trigger actions like launching apps, toggling desktop icons, fullscreen, and more. Record gestures via an on-screen canvas, then replay them with a configurable modifier key.
// @version         1.0.0
// @author          iMAboud
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -lole32 -loleaut32 -luuid -lcomctl32 -ldwmapi -luxtheme -lgdiplus -lwinhttp
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

| Actions | | |
| :--- | :--- | :--- |
| **Launch App / URL** | **Run Command Line** | **Send Keyboard Shortcut** |
| **Task Manager** | **Windows Settings** | **Admin Terminal Here** |
| **Mute / Unmute Volume** | **Play / Pause Media** | **Next / Prev Track** |
| **Toggle Always-on-Top** | **Create Sticky Note** | **Search Selection** |
| **Trigger Spotlight** | **Toggle Draw Mode** | **Color Picker** |
| **Toggle Desktop Icons** | **Fullscreen** | **Maximize Window** |
| **Minimize Window** | **Close Window** | **Show Desktop** |
| **Lock Computer** | **Sleep Computer** | **Restart / Shutdown** |
| **Explorer Back / Forward** | **Snap Left / Right** | **Snap Top / Bottom** |
| **Snap Corners** | | |

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

## Additional Settings

* **Trail Color (Hex)**: The HEX color of the gesture trail (e.g., `00AAFF` for Cyan, `FF0055` for Pink).
* **Trail Width**: The thickness of the gesture trail in pixels.
* **Enable Particle Effects**: Shows high-quality glowing "echo" particles behind the gesture trail.
* **Show Aura**: Displays a glowing ring around your cursor to indicate when Gesture Mode is fully armed and ready to draw.
* **Armed Timeout (ms)**: How long Gesture Mode stays active after being armed by a Wiggle or Toggle (in milliseconds). Set to `0` to disable the timeout.
* **Wiggle Strength (10-200)**: How far you must physically move the mouse to register a single wiggle stroke. Lower numbers mean you don't have to shake as far.
* **Allow in Fullscreen Apps**: If disabled, the mod will automatically ignore gestures while playing fullscreen games or using fullscreen apps to prevent accidental triggers.

## Example Setups

**Setup 1: Right-Click Wiggle (Highly Recommended)**
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
  $description: "Tip: 'Right Click' + 'Wiggle' is a very popular setup!"
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
- EnableWiggleToActivate: always
  $name: Wiggle to Activate
  $options:
  - never: Disabled
  - always: Always (Wiggle alone activates Gesture Mode)
  - modifier: Only with Modifier (Must have modifier active AND wiggle)
- WiggleStrength: 50
  $name: Wiggle Strength (10-200)
  $description: How far you must move the mouse to register a wiggle stroke.

- TrailColor: 00AAFF
  $name: Trail Color (Hex)
  $description: "6-digit hex code without #. Example: 00AAFF, FF0055"
- TrailWidth: 3
  $name: Trail Width

- ShowAura: true
  $name: Show Aura
  $description: Displays a glowing aura around your cursor when Gesture Mode is armed.

- ArmTimeout: 3000
  $name: Gesture Mode Timeout (ms)
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
      - show_desktop: 👁️‍🗨️ Show Desktop
      - lock: 🔒 Lock PC
      - sleep: 🌙 Sleep PC
      - restart: 🔄 Restart PC
      - shutdown: ⏻ Shut Down PC
      - create_note: 📝 Create Sticky Note
      - search_selection: 🔍 Search Selection
      - spotlight: 🔦 Trigger Spotlight
      - admin_terminal: ⚡ Admin Terminal Here
      - explorer_back: ⬅️ Explorer Back
      - explorer_forward: ➡️ Explorer Forward
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
          - show_desktop: 👁️‍🗨️ Show Desktop
          - lock: 🔒 Lock PC
          - sleep: 🌙 Sleep PC
          - restart: 🔄 Restart PC
          - shutdown: ⏻ Shut Down PC
          - create_note: 📝 Create Sticky Note
          - search_selection: 🔍 Search Selection
          - spotlight: 🔦 Trigger Spotlight
          - admin_terminal: ⚡ Admin Terminal Here
          - explorer_back: ⬅️ Explorer Back
          - explorer_forward: ➡️ Explorer Forward
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
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shobjidl.h>
#include <math.h>
#include <stdio.h>
#include <wchar.h>
#include <string>
#include <gdiplus.h>
#include <tlhelp32.h>
#include <windowsx.h>
#include <uxtheme.h>
#include <winhttp.h>

#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
#endif

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
    ACTION_SLEEP,
    ACTION_RESTART,
    ACTION_SHUTDOWN,
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
    ACTION_EXPLORER_FORWARD
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
    BOOL enableParticles;
    int armTimeout;
    EnableWiggleType enableWiggle;
    int wiggleStrength;
    UINT recordVk;
    int recordModifiers;
    BOOL allowInFullscreen;
    wchar_t fullscreenIncludeList[1024];
    wchar_t fullscreenExcludeList[1024];
    bool showAura;
    BOOL blockOtherClicks;
    double matchThreshold;
    int minGestureDistance;
    GestureConfig gestures[MAX_GESTURES];
    int gestureCount;
};

static Settings g_settings = {};
static HHOOK g_mouseHook = nullptr;
static HHOOK g_keyboardHook = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_notesThread = nullptr;
static DWORD g_notesThreadId = 0;
static volatile BOOL g_running = FALSE;

static BOOL g_gestureActive = FALSE;
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
static BOOL g_modifierToggleArmed = FALSE;
static BOOL g_modifierWasActive = FALSE;
static DWORD g_modifierToggleArmTime = 0;

// Wiggle State
static BOOL g_wiggleArmed = FALSE;
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

static BOOL g_drawModeActive = FALSE;
static HWND g_drawModeWnd = nullptr;
static Gdiplus::Color g_drawColor = Gdiplus::Color(255, 0, 170, 255);
static float g_drawWidth = 5.0f;
static BOOL g_paletteVisible = FALSE;
static float g_paletteY = -60.0f;

static BOOL g_pickerActive = FALSE;
static HWND g_pickerWnd = nullptr;
static POINT g_pickerPos = {};

static const wchar_t OVERLAY_CLASS[] = L"WindhawkGestureOverlay";
static const wchar_t CANVAS_CLASS[] = L"WindhawkGestureCanvas";
static const wchar_t DRAW_MODE_CLASS[] = L"WindhawkGestureDrawMode";
static const wchar_t PICKER_CLASS[] = L"WindhawkGestureColorPicker";
static const wchar_t MSG_WND_CLASS[] = L"WindhawkGestureMsgWnd";
static const wchar_t AURA_CLASS[] = L"WindhawkGestureAura";
static const wchar_t TOAST_CLASS[] = L"WindhawkGestureToast";
static HWND g_msgWnd = nullptr;

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
    if (wcscmp(buf, L"sleep") == 0) return ACTION_SLEEP;
    if (wcscmp(buf, L"restart") == 0) return ACTION_RESTART;
    if (wcscmp(buf, L"shutdown") == 0) return ACTION_SHUTDOWN;
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

    // 1. Simulate Ctrl+C
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_CONTROL;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = 'C';
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = 'C'; inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_CONTROL; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));

    Sleep(50); // Wait for clipboard to update

    if (OpenClipboard(NULL)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData) {
            wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
            if (pszText) {
                selectedText = pszText;
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }
    return selectedText;
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
            SendMessage(fg, WM_SYSCOMMAND, SC_RESTORE, 0);
        } else {
            SendMessage(fg, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
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

static BOOL g_noteCreationMode = FALSE;
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
    wchar_t path[MAX_PATH];
    ExpandEnvironmentStrings(L"%APPDATA%\\WindhawkMagicNotes.dat", path, MAX_PATH);
    HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        for (int i = 0; i < g_noteCount; i++) {
            if (g_notes[i].hwndEdit) {
                GetWindowText(g_notes[i].hwndEdit, g_notes[i].text, 4096);
                GetWindowRect(g_notes[i].hwnd, &g_notes[i].rect);
            }
        }
        DWORD written;
        WriteFile(hFile, &g_noteCount, sizeof(int), &written, NULL);
        WriteFile(hFile, &g_noteIdCounter, sizeof(int), &written, NULL);
        for (int i = 0; i < g_noteCount; i++) {
            WriteFile(hFile, &g_notes[i], sizeof(NoteData), &written, NULL);
        }
        WriteFile(hFile, &g_lastNoteColor, sizeof(DWORD), &written, NULL);
        WriteFile(hFile, &g_lastNoteOpacity, sizeof(int), &written, NULL);
        CloseHandle(hFile);
    }
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
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            if (note) {
                SetTextColor((HDC)wParam, RGB(255, 255, 255));
                SetBkColor((HDC)wParam, note->color);
                static HBRUSH s_br = NULL;
                if (s_br) DeleteObject(s_br);
                s_br = CreateSolidBrush(note->color);
                return (LRESULT)s_br;
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
    wchar_t path[MAX_PATH];
    ExpandEnvironmentStrings(L"%APPDATA%\\WindhawkMagicNotes.dat", path, MAX_PATH);
    HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD read;
        ReadFile(hFile, &g_noteCount, sizeof(int), &read, NULL);
        ReadFile(hFile, &g_noteIdCounter, sizeof(int), &read, NULL);
        if (g_noteCount > MAX_NOTES) g_noteCount = MAX_NOTES;
        for (int i = 0; i < g_noteCount; i++) {
            ReadFile(hFile, &g_notes[i], sizeof(NoteData), &read, NULL);
            g_notes[i].hwnd = nullptr;
            g_notes[i].hwndEdit = nullptr;
        }
        DWORD bytesRead = 0;
        if (ReadFile(hFile, &g_lastNoteColor, sizeof(DWORD), &bytesRead, NULL) && bytesRead < sizeof(DWORD)) {
            g_lastNoteColor = RGB(43, 43, 43);
        }
        if (ReadFile(hFile, &g_lastNoteOpacity, sizeof(int), &bytesRead, NULL) && bytesRead < sizeof(int)) {
            g_lastNoteOpacity = 255;
        }
        CloseHandle(hFile);
    }
}
void SpawnSavedNotes() {
    LoadLibrary(L"Msftedit.dll");

    WNDCLASS wc = {0};
    wc.lpfnWndProc = NoteWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = NOTE_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    LoadNotes();
    for (int i = 0; i < g_noteCount; i++) {
        CreateNoteWindow(&g_notes[i]);
    }
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

    LoadLibrary(L"Msftedit.dll");

    LoadNotes();
    for (int i = 0; i < g_noteCount; i++) {
        CreateNoteWindow(&g_notes[i]);
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

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
    if (FindWindow(MAIN_MSG_WND_CLASS, NULL)) return;
    g_notesThread = CreateThread(NULL, 0, RunNotesProcess, NULL, 0, &g_notesThreadId);
}

HWND g_gestureTarget = nullptr;
HWND g_spotlightWnd = nullptr;
POINT g_spotlightCurrentPt = {0,0};
float g_spotlightAlpha = 0.0f;
float g_spotlightRadius = 150.0f;
int g_spotlightState = 0; 

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

BOOL HandleNoteCreationMouse(WPARAM wParam, MSLLHOOKSTRUCT* ms) {
    if (wParam == WM_LBUTTONDOWN) {
        g_noteSelStart = ms->pt;
        g_noteSelCurrent = ms->pt;

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
                ms->pt.x, ms->pt.y, 0, 0,
                NULL, NULL, GetModuleHandle(NULL), NULL);
            SetLayeredWindowAttributes(g_noteSelWnd, RGB(0,0,0), 160, LWA_COLORKEY | LWA_ALPHA);
        }
        return TRUE;
    }
    else if (wParam == WM_MOUSEMOVE) {
        if (g_noteSelWnd) {
            g_noteSelCurrent = ms->pt;
            int x = (g_noteSelStart.x < g_noteSelCurrent.x) ? g_noteSelStart.x : g_noteSelCurrent.x;
            int y = (g_noteSelStart.y < g_noteSelCurrent.y) ? g_noteSelStart.y : g_noteSelCurrent.y;
            int w = abs(g_noteSelStart.x - g_noteSelCurrent.x);
            int h = abs(g_noteSelStart.y - g_noteSelCurrent.y);
            MoveWindow(g_noteSelWnd, x, y, w, h, TRUE);
        }
        return FALSE; // IMPORTANT: Do not swallow mouse move, let the cursor move!
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
        g_noteCreationMode = FALSE;

        if (w > 50 && h > 50) {
            HWND hwndNotesMsg = FindWindow(MAIN_MSG_WND_CLASS, NULL);
            if (!hwndNotesMsg) {
                extern void LaunchNotesProcess();
                LaunchNotesProcess();
                for (int i = 0; i < 20; i++) {
                    Sleep(50);
                    hwndNotesMsg = FindWindow(MAIN_MSG_WND_CLASS, NULL);
                    if (hwndNotesMsg) break;
                }
            }
            if (hwndNotesMsg) {
                PostMessage(hwndNotesMsg, WM_USER + 105, MAKEWPARAM(x, y), MAKELPARAM(w, h));
            }
        }
        return TRUE;
    }
    return TRUE; // Swallow other clicks
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
                ShellExecute(NULL, L"open", expanded, NULL, NULL, SW_SHOWNORMAL);
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
                    SendMessage(target, WM_SYSCOMMAND, SC_RESTORE, 0);
                } else {
                    SendMessage(target, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
                }
            }
            break;

        case ACTION_MINIMIZE:
            if (target) SendMessage(target, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            break;

        case ACTION_CLOSE:
            if (target) SendMessage(target, WM_SYSCOMMAND, SC_CLOSE, 0);
            break;

        case ACTION_SHOW_DESKTOP:
            ShowDesktop();
            break;

        case ACTION_LOCK:
            ShellExecute(NULL, L"open", L"rundll32.exe", L"user32.dll,LockWorkStation", NULL, SW_SHOWNORMAL);
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
                SendMessage(target, WM_SYSCOMMAND, SC_RESTORE, 0);
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
                std::wstring url = L"https://www.google.com/search?q=" + encoded;
                ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
            break;
        }

        case ACTION_SPOTLIGHT: {
            extern int g_spotlightState;
            if (g_spotlightState > 0) {
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
            std::wstring args = L"-NoExit -Command \"Set-Location -LiteralPath '" + safePath + L"'\"";
            ShellExecuteW(NULL, L"runas", L"powershell.exe", args.c_str(), NULL, SW_SHOWNORMAL);
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

    if (g_settings.enableParticles) {
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

LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_EXECUTE_ACTION) {
        ExecuteAction(wParam, (HWND)lParam);
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
    if (g_settings.enableParticles) {
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

    if (g_settings.enableParticles) {
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
            addPoint((int)(50 + 50 * cos((i * 10 + 90) * 3.14159 / 180.0)),
                     (int)(50 + 50 * sin((i * 10 + 90) * 3.14159 / 180.0)));
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
    RECT drawArea = { 20, 105, rc.right - 20, rc.bottom - 20 };
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
                SelectObject(memDC, startPen);
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
            static HBRUSH s_br = CreateSolidBrush(RGB(25, 25, 30));
            return (LRESULT)s_br;
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

        case WM_DESTROY:
            return 0;
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
    if (g_gestureActive) return;
    if (IsFullscreenAppActive()) return;

    if (g_fadeActive) {
        g_fadeActive = FALSE;
        KillTimer(g_overlayWnd, 1);
        DestroyOverlay();
    }

    g_gestureTarget = WindowFromPoint(pt);
    g_gestureActive = TRUE;
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
    if (g_pointCount >= MAX_POINTS) return;

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
    if (!g_gestureActive) return;
    g_gestureActive = FALSE;

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
        
        if (g_pointCount > 0 && g_settings.enableParticles) {
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
                    ShowAura(pt);
                }
            } else {
                HideAura();
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
                HideAura();
            }
            if (g_settings.modifierBehavior == MOD_BEHAVIOR_TOGGLE && g_modifierToggleArmed) {
                if (now - g_modifierToggleArmTime > (DWORD)g_settings.armTimeout) {
                    g_modifierToggleArmed = FALSE;
                    HideAura();
                }
            }
        }
        
        MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;

        if (ms->dwExtraInfo == 0x1337) {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }

        
        extern BOOL HandleNoteCreationMouse(WPARAM wParam, MSLLHOOKSTRUCT* ms);
        if (g_noteCreationMode) {
            if (HandleNoteCreationMouse(wParam, ms)) return 1;
        }

        if (wParam == WM_MOUSEMOVE && !g_gestureActive && !g_pickerActive && !g_drawModeActive) {
            if (g_settings.enableWiggle != WIGGLE_NEVER) {
                BOOL modActive = TRUE;
                if (g_settings.enableWiggle == WIGGLE_MODIFIER) {
                    if (g_settings.modifierFlags == 0) {
                        modActive = FALSE;
                    } else {
                        modActive = (g_settings.modifierBehavior == MOD_BEHAVIOR_TOGGLE) ? g_modifierToggleArmed : IsModifierActive();
                    }
                }

                if (modActive) {
                    DWORD now = GetTickCount();
                    if (g_wiggleArmed) {
                        extern void UpdateAura(POINT pt);
                        UpdateAura(ms->pt);
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
                                extern void ShowAura(POINT pt);
                                if (g_settings.showAura) {
                                    ShowAura(ms->pt);
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
                AddGesturePoint(ms->pt);
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            if (IsDrawButtonUp(wParam, ms)) {
                if (g_pointCount > 0 && g_pointCount < MAX_POINTS) {
                    POINT last = g_points[g_pointCount - 1];
                    if (ms->pt.x != last.x || ms->pt.y != last.y) {
                        g_points[g_pointCount++] = ms->pt;
                    }
                }
                EndGesture();
                return 1;
            }
            if (g_settings.blockOtherClicks) {
                if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP ||
                    wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP ||
                    wParam == WM_MBUTTONDOWN || wParam == WM_MBUTTONUP ||
                    wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP) {
                    return 1;
                }
            }
        }

        BOOL modifierValid = FALSE;
        if (g_settings.modifierFlags == 0) {
            modifierValid = TRUE;
        } else {
            modifierValid = (g_settings.modifierBehavior == MOD_BEHAVIOR_TOGGLE) ? g_modifierToggleArmed : IsModifierActive();
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
                extern void HideAura();
                HideAura();
                StartGesture(ms->pt);
                return 1;
            }
        }

        if (g_pickerActive) {
            if (wParam == WM_LBUTTONDOWN) {
                HDC hdcScreen = GetDC(NULL);
                COLORREF color = GetPixel(hdcScreen, ms->pt.x, ms->pt.y);
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

                extern void HideAura();
                HideAura();
                extern void TriggerStandaloneSplash(POINT pt);
                TriggerStandaloneSplash(ms->pt);
                StopColorPicker();
                return 1;
            }
            if (wParam == WM_RBUTTONDOWN) {
                return 1;
            }
            if (wParam == WM_RBUTTONUP) {
                extern void HideAura();
                HideAura();
                extern void TriggerStandaloneSplash(POINT pt);
                TriggerStandaloneSplash(ms->pt);
                StopColorPicker();
                return 1;
            }
            if (wParam == WM_MOUSEMOVE) {
                g_pickerPos = ms->pt;
                if (g_pickerWnd) {
                    SetWindowPos(g_pickerWnd, HWND_TOPMOST, ms->pt.x + 15, ms->pt.y + 15, 160, 160, SWP_NOACTIVATE);
                    InvalidateRect(g_pickerWnd, NULL, FALSE);
                }
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            return 1; // Swallow all other mouse inputs during color pick
        }

        if (g_drawModeActive) {
            POINT pt = ms->pt;
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
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }

            if (!overPalette) {
                if (wParam == WM_RBUTTONDOWN) {
                    return 1;
                }
                if (wParam == WM_RBUTTONUP) {
                    HideAura();
                    TriggerStandaloneSplash(ms->pt);
                    ToggleDrawMode();
                    return 1;
                }
                if (wParam == WM_LBUTTONDOWN) {
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
                    return 1;
                }
                if (wParam == WM_LBUTTONUP && g_drawDrawing) {
                    g_drawDrawing = FALSE;
                    int wndX = GetSystemMetrics(SM_XVIRTUALSCREEN);
                    int wndY = GetSystemMetrics(SM_YVIRTUALSCREEN);
                    if (g_currentDrawStroke.count > 0 && g_currentDrawStroke.count < 1000) {
                        DrawPoint last = g_currentDrawStroke.points[g_currentDrawStroke.count - 1];
                        float localX = (float)(pt.x - wndX);
                        float localY = (float)(pt.y - wndY);
                        if (localX != last.x || localY != last.y) {
                            g_currentDrawStroke.points[g_currentDrawStroke.count++] = { localX, localY };
                        }
                    }
                    if (g_currentDrawStroke.count >= 2) {
                        g_drawStrokes[g_drawStrokeCount++] = g_currentDrawStroke;
                    }
                    InvalidateRect(g_drawModeWnd, NULL, FALSE);
                    return 1;
                }
            }
        }

        if (wParam == WM_XBUTTONDOWN) {
        }
        if (wParam == WM_XBUTTONUP) {
            DWORD btn = HIWORD(ms->mouseData);
            if (btn == XBUTTON1) {
                if (g_gestureActive) { EndGesture(); return 1; }
            }
            if (btn == XBUTTON2) {
                if (g_gestureActive) { EndGesture(); return 1; }
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

void ToggleDrawMode() {
    if (g_drawModeActive) {
        g_drawModeActive = FALSE;
        if (g_drawModeWnd) {
            DestroyWindow(g_drawModeWnd);
            g_drawModeWnd = nullptr;
        }
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

        ShowWindow(g_drawModeWnd, SW_SHOWNOACTIVATE);
    }
}

void StartColorPicker() {
    if (g_pickerActive) return;
    g_pickerActive = TRUE;

    POINT pt;
    GetCursorPos(&pt);
    g_pickerPos = pt;

    g_pickerWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        PICKER_CLASS, L"",
        WS_POPUP,
        pt.x + 15, pt.y + 15, 160, 160,
        NULL, NULL, GetModuleHandle(NULL), NULL);

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
                SetWindowPos(hwnd, HWND_TOPMOST, pt.x + 15, pt.y + 15, 160, 160, SWP_NOACTIVATE);
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
            POINT ptDst = { g_pickerPos.x + 15, g_pickerPos.y + 15 };
            SIZE sizeWnd = { 160, 160 };
            BLENDFUNCTION blend = {};
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;

            UpdateLayeredWindow(hwnd, hdc, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
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
        POINT pt;
        GetCursorPos(&pt);
        // Removed modifier held logic for orb

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;

            if (kb->vkCode == VK_ESCAPE) {
                if (g_pickerActive) {
                    StopColorPicker();
                    return 1;
                }
                if (g_drawModeActive) {
                    ToggleDrawMode();
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
                    PostMessage(NULL, WM_APP + 100, 0, 0);
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

DWORD WINAPI HookThreadProc(LPVOID lpParam) {
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

    WNDCLASS mwc = {};
    mwc.lpfnWndProc = MsgWndProc;
    mwc.hInstance = GetModuleHandle(NULL);
    mwc.lpszClassName = MSG_WND_CLASS;
    RegisterClass(&mwc);

    g_msgWnd = CreateWindowEx(0, MSG_WND_CLASS, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, GetModuleHandle(NULL), NULL);

    g_uiFont = CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    if (!g_mouseHook || !g_keyboardHook) {
        Wh_Log(L"Failed to install hooks (mouse=%p keyboard=%p)", g_mouseHook, g_keyboardHook);
    } else {
        Wh_Log(L"Hooks installed successfully");
    }

    MSG msg;
    while (g_running && GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_APP + 100) {
            ShowCanvas();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_keyboardHook) { UnhookWindowsHookEx(g_keyboardHook); g_keyboardHook = nullptr; }

    if (g_msgWnd) {
        DestroyWindow(g_msgWnd);
        g_msgWnd = nullptr;
    }

    HideCanvas();
    DestroyOverlay();
    if (g_drawModeActive) ToggleDrawMode();
    if (g_pickerActive) StopColorPicker();

    if (g_uiFont) { DeleteObject(g_uiFont); g_uiFont = nullptr; }

    UnregisterClass(OVERLAY_CLASS, GetModuleHandle(NULL));
    UnregisterClass(CANVAS_CLASS, GetModuleHandle(NULL));
    UnregisterClass(DRAW_MODE_CLASS, GetModuleHandle(NULL));
    UnregisterClass(PICKER_CLASS, GetModuleHandle(NULL));
    UnregisterClass(AURA_CLASS, GetModuleHandle(NULL));
    UnregisterClass(TOAST_CLASS, GetModuleHandle(NULL));
    UnregisterClass(MSG_WND_CLASS, GetModuleHandle(NULL));

    Gdiplus::GdiplusShutdown(gdiplusToken);
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

    g_settings.enableParticles = TRUE;

    g_settings.armTimeout = Wh_GetIntSetting(L"ArmTimeout");
    if (g_settings.armTimeout < 0) g_settings.armTimeout = 0;

    g_settings.showAura = Wh_GetIntSetting(L"ShowAura");
    g_settings.blockOtherClicks = TRUE;

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
    if (g_settings.wiggleStrength < 2) g_settings.wiggleStrength = 2;
    if (g_settings.wiggleStrength > 200) g_settings.wiggleStrength = 200;

    g_settings.matchThreshold = (double)Wh_GetIntSetting(L"GestureSensitivity");
    if (g_settings.matchThreshold <= 0.0) g_settings.matchThreshold = 12.5;

    g_settings.minGestureDistance = 60;

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

BOOL Wh_ModInit() {
    Wh_Log(L"Mouse Gestures mod initializing...");

    LoadSettings();

    extern void LaunchNotesProcess();
    LaunchNotesProcess();

    g_running = TRUE;
    g_hookThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &g_hookThreadId);

    if (!g_hookThread) {
        Wh_Log(L"Failed to create hook thread");
        return FALSE;
    }

    Wh_Log(L"Mouse Gestures mod initialized successfully");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Mouse Gestures mod uninitializing...");

    g_running = FALSE;

    HWND hwndNotesMsg = FindWindow(MAIN_MSG_WND_CLASS, NULL);
    if (hwndNotesMsg) {
        PostMessage(hwndNotesMsg, WM_QUIT, 0, 0);
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

    UnregisterClass(NOTE_CLASS, GetModuleHandle(NULL));

    g_hookThreadId = 0;
    Wh_Log(L"Mouse Gestures mod uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed...");
    if (g_gestureActive) {
        Wh_Log(L"Gesture active, skipping settings reload to avoid corruption.");
        return;
    }
    Wh_Log(L"Reloading settings.");
    LoadSettings();
}
