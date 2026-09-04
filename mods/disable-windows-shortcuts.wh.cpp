// ==WindhawkMod==
// @id              disable-windows-shortcuts
// @name            Disable Windows Shortcuts
// @description     Selectively disable Windows keyboard shortcuts with individual toggles
// @version         1.3.0
// @author          Lone
// @github          https://github.com/Louis047
// @include         explorer.exe
// @include         dwm.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Disable Windows Shortcuts
Selectively disable Windows keyboard shortcuts with individual toggles for each shortcut.

## Features
- Disable any Windows key combination
- Individual toggle for each shortcut
- Uses a lightweight background hook thread ensuring third-party modifiers (like AltSnap, GlazeWM) are completely unaffected.

## How Shortcuts Are Handled
This mod provides three organized categories of shortcuts:
1. **Special Shortcuts (Flyouts):** Windows processes system flyouts (`Win+A`, `Win+N`, `Win+C`, `Win+K`, `Win+P`, `Win+U`, `Win+/`) at a lower compositor level. The mod physically suppresses keypresses via a low-level keyboard hook running in `dwm.exe`. Injected/simulated keystrokes (e.g. custom taskbars like YASB or macros) and native taskbar tray mouse clicks continue to work seamlessly.
2. **Direct Shortcuts (No Restart Required):** Handled immediately via low-level keyboard hook in `dwm.exe`. Disabling shortcuts like `Win` or `Alt+Shift` allows third-party apps (such as Flow Launcher's Windows key launcher or custom `Alt+Shift` keybinds) to be used without Windows opening the Start Menu or switching keyboard layouts. Hardcoded shortcuts (`Ctrl+Esc`, `Win+Tab`, `Win+Arrows`, `Win+Space`, `Win+Ctrl+Shift+Alt`, `Win+Ctrl+Shift+B`) are blocked immediately without requiring an Explorer restart.
3. **Standard Shortcuts (Requires Explorer Restart):** Handled via Explorer's `RegisterHotKey` API. When disabled, Explorer is prevented from claiming the key, freeing it in the OS kernel so other applications (such as PowerToys, GlazeWM, or Flow Launcher) can bind to it. Changes require an Explorer restart to release the key (a restart dialog will prompt you automatically).

## ⚠️ Important `dwm.exe` Installation Step ⚠️
For **Special Shortcuts** and **Direct Shortcuts** to be blocked, you **must** allow Windhawk to inject into the Desktop Window Manager (`dwm.exe`):
1. Open Windhawk and go to **Settings**
2. Click on **Advanced settings** at the bottom
3. Under **Process inclusion list**, ensure `dwm.exe` is added (or `*` is used to include all processes)
4. Click **Save**. Windhawk will automatically restart to apply the new settings.

*Note: Changes to standard shortcuts (like Win+E) require an Explorer restart to completely release the hotkeys for other applications. You will be prompted automatically. If you completely disable or remove this mod from Windhawk, you must restart Explorer to restore those standard shortcuts.*

## Notes
- Win+L (Lock PC) cannot be blocked through standard hooks
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- SpecialShortcuts:
  - DisableWinA: false
    $name: Win+A
    $description: Action Center / Quick Settings
  - DisableWinC: false
    $name: Win+C
    $description: Cortana / Copilot
  - DisableWinK: false
    $name: Win+K
    $description: Connect / Cast
  - DisableWinN: false
    $name: Win+N
    $description: Notification Center
  - DisableWinP: false
    $name: Win+P
    $description: Project / Display mode
  - DisableWinU: false
    $name: Win+U
    $description: Accessibility Settings
  - DisableWinSlash: false
    $name: Win+/
    $description: IME reconversion
  $name: Special Shortcuts
  $description: System flyouts handled via DWM low-level hook. Requires dwm.exe in Process inclusion list.

- DirectShortcuts:
  - DisableWinKey: false
    $name: Win
    $description: Open Start Menu
  - DisableCtrlEsc: false
    $name: Ctrl+Esc
    $description: Open Start Menu
  - DisableAltShift: false
    $name: Alt+Shift
    $description: Switch keyboard layout
  - DisableWinSpace: false
    $name: Win+Space
    $description: Switch keyboard layout
  - DisableOfficeHotkeys: false
    $name: Win+Ctrl+Shift+Alt
    $description: Office Hub / Microsoft 365 app combinations
  - DisableWinTab: false
    $name: Win+Tab
    $description: Task View
  - DisableWinUp: false
    $name: Win+Up
    $description: Maximize window
  - DisableWinDown: false
    $name: Win+Down
    $description: Restore/Minimize window
  - DisableWinLeft: false
    $name: Win+Left
    $description: Snap window left
  - DisableWinRight: false
    $name: Win+Right
    $description: Snap window right
  - DisableWinShiftUp: false
    $name: Win+Shift+Up
    $description: Stretch window vertically
  - DisableWinShiftDown: false
    $name: Win+Shift+Down
    $description: Restore/minimize height
  - DisableWinShiftLeft: false
    $name: Win+Shift+Left
    $description: Move window to left monitor
  - DisableWinShiftRight: false
    $name: Win+Shift+Right
    $description: Move window to right monitor
  - DisableWinCtrlShiftB: false
    $name: Win+Ctrl+Shift+B
    $description: Restart graphics driver
  $name: Direct Shortcuts
  $description: Shortcuts handled immediately via low-level hook without requiring an Explorer restart. Requires dwm.exe in Process inclusion list.

- StandardShortcuts:
  - DisableWinB: false
    $name: Win+B
    $description: Focus system tray
  - DisableWinD: false
    $name: Win+D
    $description: Show/Hide Desktop
  - DisableWinE: false
    $name: Win+E
    $description: File Explorer
  - DisableWinF: false
    $name: Win+F
    $description: Feedback Hub
  - DisableWinF1: false
    $name: Win+F1
    $description: Open Windows Help
  - DisableWinG: false
    $name: Win+G
    $description: Game Bar
  - DisableWinH: false
    $name: Win+H
    $description: Dictation / Voice Typing
  - DisableWinI: false
    $name: Win+I
    $description: Settings
  - DisableWinJ: false
    $name: Win+J
    $description: Focus Windows tips
  - DisableWinM: false
    $name: Win+M
    $description: Minimize all windows
  - DisableWinO: false
    $name: Win+O
    $description: Lock device orientation
  - DisableWinQ: false
    $name: Win+Q
    $description: Search
  - DisableWinR: false
    $name: Win+R
    $description: Run dialog
  - DisableWinS: false
    $name: Win+S
    $description: Search
  - DisableWinT: false
    $name: Win+T
    $description: Cycle taskbar apps
  - DisableWinV: false
    $name: Win+V
    $description: Clipboard History
  - DisableWinW: false
    $name: Win+W
    $description: Widgets
  - DisableWinX: false
    $name: Win+X
    $description: Quick Link menu
  - DisableWinY: false
    $name: Win+Y
    $description: Switch input (Mixed Reality)
  - DisableWinZ: false
    $name: Win+Z
    $description: Snap Layouts
  - DisableWinHome: false
    $name: Win+Home
    $description: Minimize inactive windows
  - DisableWinShiftC: false
    $name: Win+Shift+C
    $description: Charms Menu (Windows 10)
  - DisableWinShiftM: false
    $name: Win+Shift+M
    $description: Restore minimized windows
  - DisableWinComma: false
    $name: Win+Comma
    $description: Peek at desktop
  - DisableWinPause: false
    $name: Win+Pause
    $description: System Properties
  - DisableWinCtrlD: false
    $name: Win+Ctrl+D
    $description: New virtual desktop
  - DisableWinCtrlF: false
    $name: Win+Ctrl+F
    $description: Find Computers
  - DisableWinCtrlF4: false
    $name: Win+Ctrl+F4
    $description: Close virtual desktop
  - DisableWinCtrlLeft: false
    $name: Win+Ctrl+Left
    $description: Previous virtual desktop
  - DisableWinCtrlRight: false
    $name: Win+Ctrl+Right
    $description: Next virtual desktop
  - DisableWinNumbers: false
    $name: Win+Number (0-9)
    $description: Launch/switch taskbar apps
  - DisableWinShiftNumbers: false
    $name: Win+Shift+Number
    $description: Launch new instance
  - DisableWinCtrlNumbers: false
    $name: Win+Ctrl+Number
    $description: Switch to last active window
  - DisableWinAltNumbers: false
    $name: Win+Alt+Number
    $description: Open Jump List
  - DisableWinPlus: false
    $name: Win+Plus
    $description: Magnifier zoom in
  - DisableWinMinus: false
    $name: Win+Minus
    $description: Magnifier zoom out
  - DisableWinEsc: false
    $name: Win+Esc
    $description: Close Magnifier
  - DisableWinCtrlEnter: false
    $name: Win+Ctrl+Enter
    $description: Narrator
  - DisableWinCtrlC: false
    $name: Win+Ctrl+C
    $description: Color filters
  - DisableWinCtrlN: false
    $name: Win+Ctrl+N
    $description: Narrator settings
  - DisableWinCtrlO: false
    $name: Win+Ctrl+O
    $description: On-Screen Keyboard
  - DisableWinCtrlS: false
    $name: Win+Ctrl+S
    $description: Speech Recognition
  - DisableWinShiftR: false
    $name: Win+Shift+R
    $description: Snipping Tool record
  - DisableWinShiftS: false
    $name: Win+Shift+S
    $description: Snipping Tool screenshot
  - DisableWinAltK: false
    $name: Win+Alt+K
    $description: Toggle microphone (calls)
  - DisableWinPeriod: false
    $name: Win+Period
    $description: Emoji picker
  - DisableWinSemicolon: false
    $name: Win+Semicolon
    $description: Emoji picker (alt)
  - DisableWinPrtSc: false
    $name: Win+PrtSc
    $description: Screenshot to file
  - DisableWinAltD: false
    $name: Win+Alt+D
    $description: Show date/time
  - DisableWinAltB: false
    $name: Win+Alt+B
    $description: Toggle HDR
  - DisableWinAltR: false
    $name: Win+Alt+R
    $description: Record (Game Bar)
  - DisableWinAltG: false
    $name: Win+Alt+G
    $description: Record last 30s (Game Bar)
  - DisableWinAltPrtSc: false
    $name: Win+Alt+PrtSc
    $description: Screenshot (Game Bar)
  - DisableWinAltT: false
    $name: Win+Alt+T
    $description: Show/hide recording timer
  - DisableWinAltM: false
    $name: Win+Alt+M
    $description: Toggle microphone (Game Bar)
  - DisableWinCtrlQ: false
    $name: Win+Ctrl+Q
    $description: Quick Assist
  $name: Standard Shortcuts
  $description: Regular shortcuts registered by Explorer. Requires restarting Explorer to apply changes and release hotkeys for third-party apps.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <commctrl.h>

bool g_isExplorer = false;
bool g_isDWM = false;

// Settings structure
struct Settings
{
    bool DisableWinA;
    bool DisableWinB;
    bool DisableWinC;
    bool DisableWinD;
    bool DisableWinE;
    bool DisableWinF;
    bool DisableWinF1;
    bool DisableWinG;
    bool DisableWinH;
    bool DisableWinI;
    bool DisableWinJ;
    bool DisableWinK;
    bool DisableWinM;
    bool DisableWinN;
    bool DisableWinO;
    bool DisableWinP;
    bool DisableWinQ;
    bool DisableWinR;
    bool DisableWinS;
    bool DisableWinT;
    bool DisableWinU;
    bool DisableWinV;
    bool DisableWinW;
    bool DisableWinX;
    bool DisableWinY;
    bool DisableWinZ;
    bool DisableWinSlash;
    bool DisableWinTab;
    bool DisableWinUp;
    bool DisableWinDown;
    bool DisableWinLeft;
    bool DisableWinRight;
    bool DisableWinHome;
    bool DisableWinShiftC;
    bool DisableWinShiftM;
    bool DisableWinComma;
    bool DisableWinPause;
    bool DisableWinCtrlD;
    bool DisableWinCtrlF;
    bool DisableWinCtrlF4;
    bool DisableWinCtrlLeft;
    bool DisableWinCtrlRight;
    bool DisableWinNumbers;
    bool DisableWinShiftNumbers;
    bool DisableWinCtrlNumbers;
    bool DisableWinAltNumbers;
    bool DisableWinPlus;
    bool DisableWinMinus;
    bool DisableWinEsc;
    bool DisableWinCtrlEnter;
    bool DisableWinCtrlC;
    bool DisableWinCtrlN;
    bool DisableWinCtrlO;
    bool DisableWinCtrlS;
    bool DisableWinSpace;
    bool DisableWinShiftR;
    bool DisableWinShiftS;
    bool DisableWinAltK;
    bool DisableWinPeriod;
    bool DisableWinSemicolon;
    bool DisableWinPrtSc;
    bool DisableWinShiftLeft;
    bool DisableWinShiftRight;
    bool DisableWinShiftUp;
    bool DisableWinShiftDown;
    bool DisableOfficeHotkeys;
    bool DisableWinAltD;
    bool DisableWinAltB;
    bool DisableWinAltR;
    bool DisableWinAltG;
    bool DisableWinAltPrtSc;
    bool DisableWinAltT;
    bool DisableWinAltM;
    bool DisableWinCtrlShiftB;
    bool DisableWinCtrlQ;
    bool DisableAltShift;
    bool DisableWinKey;
    bool DisableCtrlEsc;
} g_settings;

bool StandardShortcutsEqual(const Settings& a, const Settings& b)
{
    return a.DisableWinB == b.DisableWinB &&
           a.DisableWinD == b.DisableWinD &&
           a.DisableWinE == b.DisableWinE &&
           a.DisableWinF == b.DisableWinF &&
           a.DisableWinF1 == b.DisableWinF1 &&
           a.DisableWinG == b.DisableWinG &&
           a.DisableWinH == b.DisableWinH &&
           a.DisableWinI == b.DisableWinI &&
           a.DisableWinJ == b.DisableWinJ &&
           a.DisableWinM == b.DisableWinM &&
           a.DisableWinO == b.DisableWinO &&
           a.DisableWinQ == b.DisableWinQ &&
           a.DisableWinR == b.DisableWinR &&
           a.DisableWinS == b.DisableWinS &&
           a.DisableWinT == b.DisableWinT &&
           a.DisableWinV == b.DisableWinV &&
           a.DisableWinW == b.DisableWinW &&
           a.DisableWinX == b.DisableWinX &&
           a.DisableWinY == b.DisableWinY &&
           a.DisableWinZ == b.DisableWinZ &&
           a.DisableWinHome == b.DisableWinHome &&
           a.DisableWinShiftC == b.DisableWinShiftC &&
           a.DisableWinShiftM == b.DisableWinShiftM &&
           a.DisableWinShiftR == b.DisableWinShiftR &&
           a.DisableWinShiftS == b.DisableWinShiftS &&
           a.DisableWinComma == b.DisableWinComma &&
           a.DisableWinPause == b.DisableWinPause &&
           a.DisableWinCtrlD == b.DisableWinCtrlD &&
           a.DisableWinCtrlF == b.DisableWinCtrlF &&
           a.DisableWinCtrlF4 == b.DisableWinCtrlF4 &&
           a.DisableWinCtrlLeft == b.DisableWinCtrlLeft &&
           a.DisableWinCtrlRight == b.DisableWinCtrlRight &&
           a.DisableWinNumbers == b.DisableWinNumbers &&
           a.DisableWinShiftNumbers == b.DisableWinShiftNumbers &&
           a.DisableWinCtrlNumbers == b.DisableWinCtrlNumbers &&
           a.DisableWinAltNumbers == b.DisableWinAltNumbers &&
           a.DisableWinPlus == b.DisableWinPlus &&
           a.DisableWinMinus == b.DisableWinMinus &&
           a.DisableWinEsc == b.DisableWinEsc &&
           a.DisableWinCtrlEnter == b.DisableWinCtrlEnter &&
           a.DisableWinCtrlC == b.DisableWinCtrlC &&
           a.DisableWinCtrlN == b.DisableWinCtrlN &&
           a.DisableWinCtrlO == b.DisableWinCtrlO &&
           a.DisableWinCtrlS == b.DisableWinCtrlS &&
           a.DisableWinAltK == b.DisableWinAltK &&
           a.DisableWinPeriod == b.DisableWinPeriod &&
           a.DisableWinSemicolon == b.DisableWinSemicolon &&
           a.DisableWinPrtSc == b.DisableWinPrtSc &&
           a.DisableWinAltD == b.DisableWinAltD &&
           a.DisableWinAltB == b.DisableWinAltB &&
           a.DisableWinAltR == b.DisableWinAltR &&
           a.DisableWinAltG == b.DisableWinAltG &&
           a.DisableWinAltPrtSc == b.DisableWinAltPrtSc &&
           a.DisableWinAltT == b.DisableWinAltT &&
           a.DisableWinAltM == b.DisableWinAltM &&
           a.DisableWinCtrlQ == b.DisableWinCtrlQ;
}

bool HasAnyStandardShortcutsDisabled()
{
    Settings emptySettings{};
    return !StandardShortcutsEqual(g_settings, emptySettings);
}

void LoadSettings()
{
    // Special Shortcuts (Flyouts handled via DWM)
    g_settings.DisableWinA = Wh_GetIntSetting(L"SpecialShortcuts.DisableWinA");
    g_settings.DisableWinC = Wh_GetIntSetting(L"SpecialShortcuts.DisableWinC");
    g_settings.DisableWinK = Wh_GetIntSetting(L"SpecialShortcuts.DisableWinK");
    g_settings.DisableWinN = Wh_GetIntSetting(L"SpecialShortcuts.DisableWinN");
    g_settings.DisableWinP = Wh_GetIntSetting(L"SpecialShortcuts.DisableWinP");
    g_settings.DisableWinU = Wh_GetIntSetting(L"SpecialShortcuts.DisableWinU");
    g_settings.DisableWinSlash = Wh_GetIntSetting(L"SpecialShortcuts.DisableWinSlash");

    // Direct Shortcuts (No Explorer Restart Required)
    g_settings.DisableWinKey = Wh_GetIntSetting(L"DirectShortcuts.DisableWinKey");
    g_settings.DisableCtrlEsc = Wh_GetIntSetting(L"DirectShortcuts.DisableCtrlEsc");
    g_settings.DisableAltShift = Wh_GetIntSetting(L"DirectShortcuts.DisableAltShift");
    g_settings.DisableWinSpace = Wh_GetIntSetting(L"DirectShortcuts.DisableWinSpace");
    g_settings.DisableOfficeHotkeys = Wh_GetIntSetting(L"DirectShortcuts.DisableOfficeHotkeys");
    g_settings.DisableWinTab = Wh_GetIntSetting(L"DirectShortcuts.DisableWinTab");
    g_settings.DisableWinUp = Wh_GetIntSetting(L"DirectShortcuts.DisableWinUp");
    g_settings.DisableWinDown = Wh_GetIntSetting(L"DirectShortcuts.DisableWinDown");
    g_settings.DisableWinLeft = Wh_GetIntSetting(L"DirectShortcuts.DisableWinLeft");
    g_settings.DisableWinRight = Wh_GetIntSetting(L"DirectShortcuts.DisableWinRight");
    g_settings.DisableWinShiftUp = Wh_GetIntSetting(L"DirectShortcuts.DisableWinShiftUp");
    g_settings.DisableWinShiftDown = Wh_GetIntSetting(L"DirectShortcuts.DisableWinShiftDown");
    g_settings.DisableWinShiftLeft = Wh_GetIntSetting(L"DirectShortcuts.DisableWinShiftLeft");
    g_settings.DisableWinShiftRight = Wh_GetIntSetting(L"DirectShortcuts.DisableWinShiftRight");
    g_settings.DisableWinCtrlShiftB = Wh_GetIntSetting(L"DirectShortcuts.DisableWinCtrlShiftB");

    // Standard Shortcuts (Requires Explorer Restart)
    g_settings.DisableWinB = Wh_GetIntSetting(L"StandardShortcuts.DisableWinB");
    g_settings.DisableWinD = Wh_GetIntSetting(L"StandardShortcuts.DisableWinD");
    g_settings.DisableWinE = Wh_GetIntSetting(L"StandardShortcuts.DisableWinE");
    g_settings.DisableWinF = Wh_GetIntSetting(L"StandardShortcuts.DisableWinF");
    g_settings.DisableWinF1 = Wh_GetIntSetting(L"StandardShortcuts.DisableWinF1");
    g_settings.DisableWinG = Wh_GetIntSetting(L"StandardShortcuts.DisableWinG");
    g_settings.DisableWinH = Wh_GetIntSetting(L"StandardShortcuts.DisableWinH");
    g_settings.DisableWinI = Wh_GetIntSetting(L"StandardShortcuts.DisableWinI");
    g_settings.DisableWinJ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinJ");
    g_settings.DisableWinM = Wh_GetIntSetting(L"StandardShortcuts.DisableWinM");
    g_settings.DisableWinO = Wh_GetIntSetting(L"StandardShortcuts.DisableWinO");
    g_settings.DisableWinQ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinQ");
    g_settings.DisableWinR = Wh_GetIntSetting(L"StandardShortcuts.DisableWinR");
    g_settings.DisableWinS = Wh_GetIntSetting(L"StandardShortcuts.DisableWinS");
    g_settings.DisableWinT = Wh_GetIntSetting(L"StandardShortcuts.DisableWinT");
    g_settings.DisableWinV = Wh_GetIntSetting(L"StandardShortcuts.DisableWinV");
    g_settings.DisableWinW = Wh_GetIntSetting(L"StandardShortcuts.DisableWinW");
    g_settings.DisableWinX = Wh_GetIntSetting(L"StandardShortcuts.DisableWinX");
    g_settings.DisableWinY = Wh_GetIntSetting(L"StandardShortcuts.DisableWinY");
    g_settings.DisableWinZ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinZ");
    g_settings.DisableWinHome = Wh_GetIntSetting(L"StandardShortcuts.DisableWinHome");
    g_settings.DisableWinShiftC = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftC");
    g_settings.DisableWinShiftM = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftM");
    g_settings.DisableWinComma = Wh_GetIntSetting(L"StandardShortcuts.DisableWinComma");
    g_settings.DisableWinPause = Wh_GetIntSetting(L"StandardShortcuts.DisableWinPause");
    g_settings.DisableWinCtrlD = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlD");
    g_settings.DisableWinCtrlF = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlF");
    g_settings.DisableWinCtrlF4 = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlF4");
    g_settings.DisableWinCtrlLeft = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlLeft");
    g_settings.DisableWinCtrlRight = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlRight");
    g_settings.DisableWinNumbers = Wh_GetIntSetting(L"StandardShortcuts.DisableWinNumbers");
    g_settings.DisableWinShiftNumbers = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftNumbers");
    g_settings.DisableWinCtrlNumbers = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlNumbers");
    g_settings.DisableWinAltNumbers = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltNumbers");
    g_settings.DisableWinPlus = Wh_GetIntSetting(L"StandardShortcuts.DisableWinPlus");
    g_settings.DisableWinMinus = Wh_GetIntSetting(L"StandardShortcuts.DisableWinMinus");
    g_settings.DisableWinEsc = Wh_GetIntSetting(L"StandardShortcuts.DisableWinEsc");
    g_settings.DisableWinCtrlEnter = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlEnter");
    g_settings.DisableWinCtrlC = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlC");
    g_settings.DisableWinCtrlN = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlN");
    g_settings.DisableWinCtrlO = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlO");
    g_settings.DisableWinCtrlS = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlS");
    g_settings.DisableWinShiftR = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftR");
    g_settings.DisableWinShiftS = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftS");
    g_settings.DisableWinAltK = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltK");
    g_settings.DisableWinPeriod = Wh_GetIntSetting(L"StandardShortcuts.DisableWinPeriod");
    g_settings.DisableWinSemicolon = Wh_GetIntSetting(L"StandardShortcuts.DisableWinSemicolon");
    g_settings.DisableWinPrtSc = Wh_GetIntSetting(L"StandardShortcuts.DisableWinPrtSc");
    g_settings.DisableWinAltD = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltD");
    g_settings.DisableWinAltB = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltB");
    g_settings.DisableWinAltR = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltR");
    g_settings.DisableWinAltG = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltG");
    g_settings.DisableWinAltPrtSc = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltPrtSc");
    g_settings.DisableWinAltT = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltT");
    g_settings.DisableWinAltM = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltM");
    g_settings.DisableWinCtrlQ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlQ");
}

bool IsNumberKey(DWORD vkCode)
{
    return (vkCode >= '0' && vkCode <= '9');
}

// ============================================================================
// Hotkey Evaluation Logic
// ============================================================================

bool ShouldBlockHotkey(UINT fsModifiers, UINT vk)
{
    // Strip MOD_NOREPEAT for comparison (Windows often registers with this flag)
    UINT baseMods = fsModifiers & ~MOD_NOREPEAT;
    bool hasWin = (baseMods & MOD_WIN) != 0;
    bool hasShift = (baseMods & MOD_SHIFT) != 0;
    bool hasCtrl = (baseMods & MOD_CONTROL) != 0;
    bool hasAlt = (baseMods & MOD_ALT) != 0;
    bool block = false;

    // Office hotkeys - exact match (Ctrl+Shift+Alt+Win)
    if (baseMods == (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN) &&
        g_settings.DisableOfficeHotkeys)
    {
        // Office hotkey VKs: W, T, Y, O, P, D, L, X, N, SPACE, or no VK (0)
        // Some keyboards send the modifiers without a specific VK when just the Office key is pressed.
        if (!vk || vk == 'W' || vk == 'T' || vk == 'Y' || vk == 'O' ||
            vk == 'P' || vk == 'D' || vk == 'L' || vk == 'X' || vk == 'N' ||
            vk == VK_SPACE || (vk >= VK_SHIFT && vk <= VK_MENU) || vk == VK_LWIN || vk == VK_RWIN)
        {
            block = true;
        }
    }
    else if (!hasWin && hasCtrl && !hasShift && !hasAlt && vk == VK_ESCAPE)
    {
        block = g_settings.DisableCtrlEsc;
    }
    else if (hasWin)
    {
        // Win+Ctrl+Shift combinations
        if (hasCtrl && hasShift && !hasAlt)
        {
            if (vk == 'B' && g_settings.DisableWinCtrlShiftB) block = true;
        }
        // Win+Shift combinations
        else if (hasShift && !hasCtrl && !hasAlt)
        {
            switch (vk)
            {
                case 'C': block = g_settings.DisableWinShiftC; break;
                case 'M': block = g_settings.DisableWinShiftM; break;
                case 'R': block = g_settings.DisableWinShiftR; break;
                case 'S': block = g_settings.DisableWinShiftS; break;
                case VK_LEFT: block = g_settings.DisableWinShiftLeft; break;
                case VK_RIGHT: block = g_settings.DisableWinShiftRight; break;
                case VK_UP: block = g_settings.DisableWinShiftUp; break;
                case VK_DOWN: block = g_settings.DisableWinShiftDown; break;
            }
            if (g_settings.DisableWinShiftNumbers && IsNumberKey(vk)) block = true;
        }
        // Win+Ctrl combinations
        else if (hasCtrl && !hasShift && !hasAlt)
        {
            switch (vk)
            {
                case 'C': block = g_settings.DisableWinCtrlC; break;
                case 'D': block = g_settings.DisableWinCtrlD; break;
                case 'F': block = g_settings.DisableWinCtrlF; break;
                case 'N': block = g_settings.DisableWinCtrlN; break;
                case 'O': block = g_settings.DisableWinCtrlO; break;
                case 'Q': block = g_settings.DisableWinCtrlQ; break;
                case 'S': block = g_settings.DisableWinCtrlS; break;
                case VK_F4: block = g_settings.DisableWinCtrlF4; break;
                case VK_LEFT: block = g_settings.DisableWinCtrlLeft; break;
                case VK_RIGHT: block = g_settings.DisableWinCtrlRight; break;
                case VK_RETURN: block = g_settings.DisableWinCtrlEnter; break;
            }
            if (g_settings.DisableWinCtrlNumbers && IsNumberKey(vk)) block = true;
        }
        // Win+Alt combinations
        else if (hasAlt && !hasShift && !hasCtrl)
        {
            switch (vk)
            {
                case 'B': block = g_settings.DisableWinAltB; break;
                case 'D': block = g_settings.DisableWinAltD; break;
                case 'G': block = g_settings.DisableWinAltG; break;
                case 'K': block = g_settings.DisableWinAltK; break;
                case 'M': block = g_settings.DisableWinAltM; break;
                case 'R': block = g_settings.DisableWinAltR; break;
                case 'T': block = g_settings.DisableWinAltT; break;
                case VK_SNAPSHOT: block = g_settings.DisableWinAltPrtSc; break;
            }
            if (g_settings.DisableWinAltNumbers && IsNumberKey(vk)) block = true;
        }
        // Win + key only
        else if (!hasShift && !hasCtrl && !hasAlt)
        {
            switch (vk)
            {
                case 'A': block = g_settings.DisableWinA; break;
                case 'B': block = g_settings.DisableWinB; break;
                case 'C': block = g_settings.DisableWinC; break;
                case 'D': block = g_settings.DisableWinD; break;
                case 'E': block = g_settings.DisableWinE; break;
                case 'F': block = g_settings.DisableWinF; break;
                case VK_F1: block = g_settings.DisableWinF1; break;
                case 'G': block = g_settings.DisableWinG; break;
                case 'H': block = g_settings.DisableWinH; break;
                case 'I': block = g_settings.DisableWinI; break;
                case 'J': block = g_settings.DisableWinJ; break;
                case 'K': block = g_settings.DisableWinK; break;
                case 'M': block = g_settings.DisableWinM; break;
                case 'N': block = g_settings.DisableWinN; break;
                case 'O': block = g_settings.DisableWinO; break;
                case 'P': block = g_settings.DisableWinP; break;
                case 'Q': block = g_settings.DisableWinQ; break;
                case 'R': block = g_settings.DisableWinR; break;
                case 'S': block = g_settings.DisableWinS; break;
                case 'T': block = g_settings.DisableWinT; break;
                case 'U': block = g_settings.DisableWinU; break;
                case 'V': block = g_settings.DisableWinV; break;
                case 'W': block = g_settings.DisableWinW; break;
                case 'X': block = g_settings.DisableWinX; break;
                case 'Y': block = g_settings.DisableWinY; break;
                case 'Z': block = g_settings.DisableWinZ; break;
                case VK_TAB: block = g_settings.DisableWinTab; break;
                case VK_UP: block = g_settings.DisableWinUp; break;
                case VK_DOWN: block = g_settings.DisableWinDown; break;
                case VK_LEFT: block = g_settings.DisableWinLeft; break;
                case VK_RIGHT: block = g_settings.DisableWinRight; break;
                case VK_HOME: block = g_settings.DisableWinHome; break;
                case VK_OEM_COMMA: block = g_settings.DisableWinComma; break;
                case VK_PAUSE: block = g_settings.DisableWinPause; break;
                case VK_OEM_PLUS: block = g_settings.DisableWinPlus; break;
                case VK_OEM_MINUS: block = g_settings.DisableWinMinus; break;
                case VK_ESCAPE: block = g_settings.DisableWinEsc; break;
                case VK_SPACE: block = g_settings.DisableWinSpace; break;
                case VK_OEM_PERIOD: block = g_settings.DisableWinPeriod; break;
                case VK_OEM_2: block = g_settings.DisableWinSlash; break;
                case VK_OEM_1: block = g_settings.DisableWinSemicolon; break;
                case VK_SNAPSHOT: block = g_settings.DisableWinPrtSc; break;
            }
            if (g_settings.DisableWinNumbers && IsNumberKey(vk)) block = true;
        }
    }

    return block;
}

bool IsKnownHardcodedHotkey(UINT fsModifiers, UINT vk)
{
    UINT baseMods = fsModifiers & ~MOD_NOREPEAT;
    bool hasWin = (baseMods & MOD_WIN) != 0;
    bool hasShift = (baseMods & MOD_SHIFT) != 0;
    bool hasCtrl = (baseMods & MOD_CONTROL) != 0;
    bool hasAlt = (baseMods & MOD_ALT) != 0;

    // Office Hotkeys
    if (baseMods == (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN))
        return true;

    if (!hasWin && hasCtrl && !hasShift && !hasAlt && vk == VK_ESCAPE)
        return true;

    if (hasWin && !hasCtrl && !hasAlt)
    {
        if (!hasShift) {
            // Hardcoded keys that bypass RegisterHotKey
            if (vk == VK_TAB || vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT || vk == VK_SPACE)
                return true;
            if (vk == 'A' && g_settings.DisableWinA) return true;
            if (vk == 'C' && g_settings.DisableWinC) return true;
            if (vk == 'K' && g_settings.DisableWinK) return true;
            if (vk == 'N' && g_settings.DisableWinN) return true;
            if (vk == 'P' && g_settings.DisableWinP) return true;
            if (vk == 'U' && g_settings.DisableWinU) return true;
            if (vk == VK_OEM_2 && g_settings.DisableWinSlash) return true;
        } else {
            // Win+Shift+Arrows
            if (vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT)
                return true;
        }
    }
    
    if (hasWin && hasCtrl && hasShift && !hasAlt && vk == 'B')
        return true; // Win+Ctrl+Shift+B
    return false;
}

// ============================================================================
// RegisterHotKey hook
// ============================================================================

typedef BOOL(WINAPI *RegisterHotKey_t)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
RegisterHotKey_t RegisterHotKey_Original;

BOOL WINAPI RegisterHotKey_Hook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (ShouldBlockHotkey(fsModifiers, vk))
    {
        // For Special Shortcuts & hardcoded keys, IsKnownHardcodedHotkey returns true:
        // Explorer registers the hotkey so internal shell flyouts initialize properly 
        // and taskbar tray mouse clicks or simulating apps continue to work, 
        // while the physical keystroke is blocked in DWM.
        // For Standard shortcuts (like Win+E, Win+R, Win+D), IsKnownHardcodedHotkey returns false:
        // Explorer is prevented from claiming the hotkey, freeing it at OS level for other apps.
        if (IsKnownHardcodedHotkey(fsModifiers, vk))
        {
            return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
        }

        SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
        return FALSE;
    }
    return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
}

// ============================================================================
// Explorer restart prompt
// ============================================================================

HANDLE g_restartExplorerPromptThread = NULL;
std::atomic<HWND> g_restartExplorerPromptWindow = NULL;

constexpr WCHAR kRestartExplorerPromptTitle[] = L"Disable Windows Shortcuts - Windhawk";
constexpr WCHAR kRestartExplorerPromptText[] = L"Explorer needs to be restarted to apply changes and release standard shortcuts for other applications. Restart now?";

void PromptForExplorerRestart()
{
    if (g_restartExplorerPromptThread)
    {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) != WAIT_OBJECT_0)
            return;
        CloseHandle(g_restartExplorerPromptThread);
        g_restartExplorerPromptThread = NULL;
    }

    g_restartExplorerPromptThread = CreateThread(nullptr, 0, [](LPVOID) WINAPI -> DWORD {
        TASKDIALOGCONFIG taskDialogConfig{
            .cbSize = sizeof(taskDialogConfig),
            .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
            .dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
            .pszWindowTitle = kRestartExplorerPromptTitle,
            .pszMainIcon = TD_INFORMATION_ICON,
            .pszContent = kRestartExplorerPromptText,
            .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData) WINAPI -> HRESULT {
                switch (msg)
                {
                case TDN_CREATED:
                    g_restartExplorerPromptWindow = hwnd;
                    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    break;
                case TDN_DESTROYED:
                    g_restartExplorerPromptWindow = nullptr;
                    break;
                }
                return S_OK;
            },
        };

        static decltype(&TaskDialogIndirect) pTaskDialogIndirect = []() {
            HMODULE hComctl32 = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (!hComctl32) return (decltype(&TaskDialogIndirect))nullptr;
            return (decltype(&TaskDialogIndirect))GetProcAddress(hComctl32, "TaskDialogIndirect");
        }();

        int button = 0;
        if (pTaskDialogIndirect && SUCCEEDED(pTaskDialogIndirect(&taskDialogConfig, &button, nullptr, nullptr)) && button == IDYES)
        {
            WCHAR commandLine[] = L"cmd.exe /c \"timeout /t 1 /nobreak >nul & taskkill /F /IM explorer.exe & start explorer.exe\"";
            STARTUPINFOW si = { .cb = sizeof(si) };
            PROCESS_INFORMATION pi{};
            if (CreateProcessW(nullptr, commandLine, nullptr, nullptr, FALSE, CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &si, &pi))
            {
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
            }
        }
        return 0;
    }, nullptr, 0, nullptr);
}

// ============================================================================
// Low Level Keyboard Hook Implementation (AHK Method)
// ============================================================================

HHOOK g_hHook = NULL;
HWINEVENTHOOK g_desktopSwitchHook = NULL;
HANDLE g_hookThread = NULL;
std::atomic<bool> g_hookThreadRunning{false};
std::atomic<DWORD> g_hookThreadId{0};
bool g_suppressedKeys[256] = {false};
bool g_keyState[256] = {false}; // Track our own key states reliably

bool g_winKeyUsed = false;

void RefreshKeyboardState()
{
    for (int vkCode = 0; vkCode < 256; vkCode++)
    {
        g_keyState[vkCode] = !!(GetAsyncKeyState(vkCode) & 0x8000);
        g_suppressedKeys[vkCode] = false;
    }

    g_winKeyUsed = false;
}

void CALLBACK DesktopSwitchProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime)
{
    RefreshKeyboardState();
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // If nCode is less than zero, the hook procedure must pass the message
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyBoard->vkCode;
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
        bool isInjected = (pKeyBoard->flags & LLKHF_INJECTED) != 0;

        // --- STEP 1: Unconditionally update software key state FIRST ---
        bool isInitialDown = false;
        if (vkCode < 256)
        {
            if (isDown)
            {
                if (!g_keyState[vkCode])
                {
                    isInitialDown = true;
                    if (vkCode == VK_LWIN || vkCode == VK_RWIN)
                    {
                        g_winKeyUsed = false;
                    }
                    else
                    {
                        g_winKeyUsed = true;
                    }
                }
                g_keyState[vkCode] = true;
            }
            else if (isUp)
            {
                g_keyState[vkCode] = false;
            }
        }

        // --- STEP 2: Evaluate 100% accurate modifier states ---
        bool hasWin = g_keyState[VK_LWIN] || g_keyState[VK_RWIN] || (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
        bool hasCtrl = g_keyState[VK_CONTROL] || g_keyState[VK_LCONTROL] || g_keyState[VK_RCONTROL] || (GetAsyncKeyState(VK_CONTROL) & 0x8000) || (GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
        bool hasShift = g_keyState[VK_SHIFT] || g_keyState[VK_LSHIFT] || g_keyState[VK_RSHIFT] || (GetAsyncKeyState(VK_SHIFT) & 0x8000) || (GetAsyncKeyState(VK_LSHIFT) & 0x8000) || (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
        bool hasAlt = g_keyState[VK_MENU] || g_keyState[VK_LMENU] || g_keyState[VK_RMENU] || (GetAsyncKeyState(VK_MENU) & 0x8000) || (GetAsyncKeyState(VK_LMENU) & 0x8000) || (GetAsyncKeyState(VK_RMENU) & 0x8000);

        UINT fsModifiers = 0;
        if (hasWin) fsModifiers |= MOD_WIN;
        if (hasCtrl) fsModifiers |= MOD_CONTROL;
        if (hasShift) fsModifiers |= MOD_SHIFT;
        if (hasAlt) fsModifiers |= MOD_ALT;

        // --- STEP 3: Handle Injected Keystrokes ---
        // Allow injected/simulated keystrokes (e.g. from third-party tools, tray clicks, or macros)
        if (isInjected)
        {
            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        // --- STEP 4: Pass Modifiers Through (Physical) ---
        // Never block modifiers themselves to preserve third-party app compatibility
        if ((vkCode >= VK_SHIFT && vkCode <= VK_MENU) ||
            vkCode == VK_LWIN || vkCode == VK_RWIN ||
            vkCode == VK_LSHIFT || vkCode == VK_RSHIFT ||
            vkCode == VK_LCONTROL || vkCode == VK_RCONTROL ||
            vkCode == VK_LMENU || vkCode == VK_RMENU)
        {
            if (g_settings.DisableAltShift)
            {
                bool isShift = (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT);
                bool isAlt = (vkCode == VK_LMENU || vkCode == VK_RMENU || vkCode == VK_MENU);

                // Inject dummy key (vkE8) exactly on KEYUP of the first modifier released 
                // to disrupt the layout switcher's sequence detection.
                if (isUp && (isShift || isAlt))
                {
                    bool hasWinState = g_keyState[VK_LWIN] || g_keyState[VK_RWIN];
                    bool hasCtrlState = g_keyState[VK_CONTROL] || g_keyState[VK_LCONTROL] || g_keyState[VK_RCONTROL];
                    bool hasShiftState = g_keyState[VK_SHIFT] || g_keyState[VK_LSHIFT] || g_keyState[VK_RSHIFT] || isShift;
                    bool hasAltState = g_keyState[VK_MENU] || g_keyState[VK_LMENU] || g_keyState[VK_RMENU] || isAlt;

                    if (hasAltState && hasShiftState && !hasWinState && !hasCtrlState)
                    {
                        INPUT inputs[2] = {};
                        inputs[0].type = INPUT_KEYBOARD;
                        inputs[0].ki.wVk = 0xE8;
                        inputs[1].type = INPUT_KEYBOARD;
                        inputs[1].ki.wVk = 0xE8;
                        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(2, inputs, sizeof(INPUT));
                    }
                }
            }

            // If DisableOfficeHotkeys is enabled, we MUST prevent the OS from ever seeing 
            // Ctrl+Shift+Alt+Win pressed at the same time, because Windows registers this 
            // combination globally (even without an extra key) to launch the Office Hub.
            // Sending a dummy key isn't enough to block a registered modifier-only hotkey.
            if (g_settings.DisableOfficeHotkeys)
            {
                bool isWin = (vkCode == VK_LWIN || vkCode == VK_RWIN);
                bool isCtrl = (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL);
                bool isShift = (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT);
                bool isAlt = (vkCode == VK_LMENU || vkCode == VK_RMENU || vkCode == VK_MENU);

                bool hasWinState = hasWin || isWin;
                bool hasCtrlState = hasCtrl || isCtrl;
                bool hasShiftState = hasShift || isShift;
                bool hasAltState = hasAlt || isAlt;
                
                if (hasWinState && hasCtrlState && hasShiftState && hasAltState)
                {
                    if (isDown)
                    {
                        if (vkCode < 256) g_suppressedKeys[vkCode] = true;
                        return 1; // Suppress the 4th modifier so the combination is never completed
                    }
                }
            }

            if (isUp && (vkCode == VK_LWIN || vkCode == VK_RWIN))
            {
                if (g_settings.DisableWinKey && !g_winKeyUsed)
                {
                    INPUT inputs[2] = {};
                    inputs[0].type = INPUT_KEYBOARD;
                    inputs[0].ki.wVk = 0xFF;
                    inputs[1].type = INPUT_KEYBOARD;
                    inputs[1].ki.wVk = 0xFF;
                    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(2, inputs, sizeof(INPUT));
                }
            }

            if (isUp && vkCode < 256 && g_suppressedKeys[vkCode])
            {
                g_suppressedKeys[vkCode] = false;
                return 1; // Suppress the matching modifier UP event
            }

            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        // --- STEP 5: Handle UP Events (Physical) ---
        if (isUp)
        {
            if (vkCode < 256 && g_suppressedKeys[vkCode])
            {
                g_suppressedKeys[vkCode] = false;
                return 1; // Suppress the UP event
            }
            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        // --- STEP 6: Handle DOWN Events (Physical) ---
        if (isDown)
        {
            // Check if this hotkey is disabled in settings AND is hardcoded
            if (ShouldBlockHotkey(fsModifiers, vkCode) && IsKnownHardcodedHotkey(fsModifiers, vkCode))
            {
                if (vkCode < 256)
                    g_suppressedKeys[vkCode] = true;

                if (hasWin && isInitialDown) {
                    // AHK Start Menu Masking Trick (sent once per modifier-key sequence)
                    // Forces the OS to see an unassigned keystroke, cancelling the Start Menu pop-up
                    INPUT inputs[2] = {};
                    inputs[0].type = INPUT_KEYBOARD;
                    inputs[0].ki.wVk = 0xFF;
                    inputs[1].type = INPUT_KEYBOARD;
                    inputs[1].ki.wVk = 0xFF;
                    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(2, inputs, sizeof(INPUT));
                }

                return 1; // Suppress physical key
            }
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID lpParam)
{
    // Force OS to create a message queue for this thread
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    HMODULE hMod = NULL;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&LowLevelKeyboardProc,
        &hMod
    );

    g_hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
    if (!g_hHook)
    {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        g_hookThreadRunning = false;
        return 1;
    }

    g_desktopSwitchHook = SetWinEventHook(
        EVENT_SYSTEM_DESKTOPSWITCH,
        EVENT_SYSTEM_DESKTOPSWITCH,
        NULL,
        DesktopSwitchProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT
    );
    if (!g_desktopSwitchHook)
    {
        Wh_Log(L"SetWinEventHook failed: %u (non-fatal, continuing keyboard hook)", GetLastError());
    }

    // Dedicated message pump to keep the hook alive and responsive
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_desktopSwitchHook)
    {
        UnhookWinEvent(g_desktopSwitchHook);
        g_desktopSwitchHook = NULL;
    }
    UnhookWindowsHookEx(g_hHook);
    g_hHook = NULL;
    return 0;
}

void StartHookThread()
{
    if (g_hookThreadRunning) return;
    
    if (g_hookThread)
    {
        CloseHandle(g_hookThread);
        g_hookThread = NULL;
    }

    g_hookThreadRunning = true;
    DWORD threadId = 0;
    g_hookThread = CreateThread(NULL, 0, HookThread, NULL, 0, &threadId);
    if (g_hookThread)
    {
        g_hookThreadId = threadId;
    }
    else
    {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        g_hookThreadRunning = false;
    }
}

void StopHookThread()
{
    if (g_hookThreadRunning)
    {
        while (!PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0))
        {
            if (WaitForSingleObject(g_hookThread, 10) == WAIT_OBJECT_0)
                break; // thread already exited
        }
        g_hookThreadRunning = false;
    }

    if (g_hookThread)
    {
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = NULL;
    }
}

bool NeedsDwmHook()
{
    return g_settings.DisableWinA || g_settings.DisableWinC || 
           g_settings.DisableWinK || g_settings.DisableWinN || 
           g_settings.DisableWinP || g_settings.DisableWinU || 
           g_settings.DisableWinSlash || 
           g_settings.DisableWinTab ||
           g_settings.DisableWinUp || g_settings.DisableWinDown || 
           g_settings.DisableWinLeft || g_settings.DisableWinRight ||
           g_settings.DisableWinShiftUp || g_settings.DisableWinShiftDown || 
           g_settings.DisableWinShiftLeft || g_settings.DisableWinShiftRight ||
           g_settings.DisableWinCtrlShiftB || g_settings.DisableOfficeHotkeys ||
           g_settings.DisableWinSpace || g_settings.DisableAltShift || 
           g_settings.DisableWinKey || g_settings.DisableCtrlEsc;
}

bool IsMainExplorer()
{
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTaskbar)
    {
        DWORD trayPid = 0;
        GetWindowThreadProcessId(hTaskbar, &trayPid);
        if (trayPid != GetCurrentProcessId())
        {
            return false;
        }
    }
    return true;
}

bool IsExplorerMidSession()
{
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTaskbar)
    {
        DWORD trayPid = 0;
        GetWindowThreadProcessId(hTaskbar, &trayPid);
        return (trayPid == GetCurrentProcessId());
    }
    return false;
}

// ----------------------------------------------------------------------------
// Windhawk Mod Entry Points
// ----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, ARRAYSIZE(exePath));
    PCWSTR exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;

    g_isExplorer = (_wcsicmp(exeName, L"explorer.exe") == 0);
    g_isDWM = (_wcsicmp(exeName, L"dwm.exe") == 0);

    LoadSettings();

    if (g_isDWM && !NeedsDwmHook())
    {
        return FALSE; // Unload from DWM if no hardcoded/special keys are disabled
    }

    // Hook RegisterHotKey in explorer (we don't need it in DWM)
    if (g_isExplorer)
    {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32)
        {
            void* pRegisterHotKey = (void*)GetProcAddress(hUser32, "RegisterHotKey");
            if (pRegisterHotKey)
                Wh_SetFunctionHook(pRegisterHotKey, (void*)RegisterHotKey_Hook, (void**)&RegisterHotKey_Original);
        }

        // Prompt if Explorer is running mid-session and standard shortcuts are disabled
        if (IsExplorerMidSession() && !GetSystemMetrics(SM_SHUTTINGDOWN) && HasAnyStandardShortcutsDisabled())
        {
            PromptForExplorerRestart();
        }
    }

    if (g_isDWM)
        StartHookThread();

    return TRUE;
}

void Wh_ModUninit()
{
    if (g_isDWM)
    {
        StopHookThread();
    }

    if (g_isExplorer)
    {
        // 1. Signal any pending prompt dialog from a prior settings change to close.
        // Thread join must not depend on IsMainExplorer() — handle existence is the
        // correct gate. Loop is bounded to 5 s to prevent an infinite hang if the
        // dialog never opens (e.g. TaskDialogIndirect fails).
        if (g_restartExplorerPromptThread)
        {
            for (int i = 0; i < 50 && WaitForSingleObject(g_restartExplorerPromptThread, 100) == WAIT_TIMEOUT; i++)
            {
                if (HWND promptWindow = g_restartExplorerPromptWindow.load())
                {
                    PostMessage(promptWindow, WM_CLOSE, 0, 0);
                }
            }
            CloseHandle(g_restartExplorerPromptThread);
            g_restartExplorerPromptThread = nullptr;
        }

        if (IsMainExplorer())
        {
            // 2. If standard shortcuts were disabled, prompt user on unload to restore them
            if (!GetSystemMetrics(SM_SHUTTINGDOWN) && HasAnyStandardShortcutsDisabled())
            {
                PromptForExplorerRestart();
            }

            // 3. Safe bounded wait (30s) matching official simple-window-switcher pattern
            if (g_restartExplorerPromptThread)
            {
                if (WaitForSingleObject(g_restartExplorerPromptThread, 30000) == WAIT_TIMEOUT)
                {
                    if (HWND promptWindow = g_restartExplorerPromptWindow.load())
                    {
                        PostMessage(promptWindow, WM_CLOSE, 0, 0);
                    }
                    WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
                }
                CloseHandle(g_restartExplorerPromptThread);
                g_restartExplorerPromptThread = nullptr;
            }
        }
    }
}

void Wh_ModSettingsChanged()
{
    Settings oldSettings = g_settings;
    LoadSettings();
    
    if (g_isDWM)
    {
        if (NeedsDwmHook())
            StartHookThread();
        else
            StopHookThread();
    }
    
    if (g_isExplorer && IsMainExplorer() && !GetSystemMetrics(SM_SHUTTINGDOWN) && !StandardShortcutsEqual(oldSettings, g_settings))
    {
        PromptForExplorerRestart();
    }
}
