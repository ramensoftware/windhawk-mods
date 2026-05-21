// ==WindhawkMod==
// @id              disable-windows-shortcuts
// @name            Disable Windows Shortcuts
// @description     Selectively disable Windows keyboard shortcuts with individual toggles
// @version         1.1.1
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

## Special Shortcuts
A small number of system shortcuts (Win+A, Win+C, Win+K, Win+N, Win+P, Win+U) and hardcoded keys (Win+Tab, Win+Arrows) operate at a lower OS level.
To handle these properly, this mod provides **three options** for them in a dedicated section at the top of the settings:
- **0 - Off:** The shortcut is completely unaffected.
- **1 - Disable hotkey:** Disables the shortcut natively. Lightweight, but third-party apps that simulate these keys (like custom taskbars) will also be blocked.
- **2 - Block hotkey:** Blocks the physical keystroke but tricks Windows into thinking it was registered. This allows simulating apps to work while physically blocking the key, but **requires injecting into `dwm.exe`**.

## ⚠️ Important `dwm.exe` Installation Step ⚠️
If you use the **"Block hotkey"** option, or if you disable window snapping (Win+Arrows), Task View (Win+Tab), Switch keyboard layout (Win+Space, Alt+Shift), or Start Menu (Win, Ctrl+Esc), you **must** allow Windhawk to inject into the Desktop Window Manager (`dwm.exe`):
1. Open Windhawk and go to **Settings**
2. Click on **Advanced settings** at the bottom
3. Under **Process inclusion list**, ensure `dwm.exe` is added (or `*` is used to include all processes)
4. Click **Save**. Windhawk will automatically restart to apply the new settings.

*Note: Changes to standard shortcuts (like Win+E) require an Explorer restart to take effect. You will be prompted automatically. If you completely disable or remove this mod from Windhawk, you must manually restart Explorer to restore those standard shortcuts.*

## Notes
- Win key (Start Menu) is handled by the "Block Start Menu and Hosts" mod
- Win+L (Lock PC) cannot be blocked through standard hooks
- Win+Q is redundant with Win+S (both open Search)
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- SpecialShortcuts:
  - DisableWinA: "off"
    $name: Win+A
    $description: Action Center / Quick Settings
    $options:
    - "off": Off
    - disable: Disable hotkey (Simulating apps affected)
    - block: Block hotkey (Requires dwm.exe, simulating apps work)
  - DisableWinC: "off"
    $name: Win+C
    $description: Cortana / Copilot (May require 'Block hotkey' on Win 11)
    $options:
    - "off": Off
    - disable: Disable hotkey (Simulating apps affected)
    - block: Block hotkey (Requires dwm.exe, simulating apps work)
  - DisableWinK: "off"
    $name: Win+K
    $description: Connect (Cast)
    $options:
    - "off": Off
    - disable: Disable hotkey (Simulating apps affected)
    - block: Block hotkey (Requires dwm.exe, simulating apps work)
  - DisableWinN: "off"
    $name: Win+N
    $description: Notification Center
    $options:
    - "off": Off
    - disable: Disable hotkey (Simulating apps affected)
    - block: Block hotkey (Requires dwm.exe, simulating apps work)
  - DisableWinP: "off"
    $name: Win+P
    $description: Project / Display mode
    $options:
    - "off": Off
    - disable: Disable hotkey (Simulating apps affected)
    - block: Block hotkey (Requires dwm.exe, simulating apps work)
  - DisableWinU: "off"
    $name: Win+U
    $description: Accessibility Settings
    $options:
    - "off": Off
    - disable: Disable hotkey (Simulating apps affected)
    - block: Block hotkey (Requires dwm.exe, simulating apps work)
  - DisableWinSlash: "off"
    $name: Win+/
    $description: IME reconversion
    $options:
    - "off": Off
    - disable: Disable hotkey (Simulating apps affected)
    - block: Block hotkey (Requires dwm.exe, simulating apps work)
  $name: Special Shortcuts (3-Tier Options)
  $description: See 'Special Shortcuts' in Details

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
  - DisableWinHome: false
    $name: Win+Home
    $description: Minimize inactive windows
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
  - DisableWinSpace: false
    $name: Win+Space
    $description: Switch keyboard layout
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
  - DisableWinShiftLeft: false
    $name: Win+Shift+Left
    $description: Move window to left monitor
  - DisableWinShiftRight: false
    $name: Win+Shift+Right
    $description: Move window to right monitor
  - DisableWinShiftUp: false
    $name: Win+Shift+Up
    $description: Stretch window vertically
  - DisableWinShiftDown: false
    $name: Win+Shift+Down
    $description: Restore/minimize height
  - DisableOfficeHotkeys: false
    $name: Office Hotkeys
    $description: Ctrl+Shift+Alt+Win combinations
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
  - DisableWinCtrlShiftB: false
    $name: Win+Ctrl+Shift+B
    $description: Restart graphics driver
  - DisableWinCtrlQ: false
    $name: Win+Ctrl+Q
    $description: Quick Assist
  - DisableAltShift: false
    $name: Alt+Shift
    $description: Switch keyboard layout
  - DisableWinKey: false
    $name: Win
    $description: Open Start Menu
  - DisableCtrlEsc: false
    $name: Ctrl+Esc
    $description: Open Start Menu
  $name: Standard Shortcuts (On/Off)
  $description: Regular shortcuts that only require Explorer
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <atomic>
#include <commctrl.h>
#include <algorithm>
#include <string>

bool g_isExplorer = false;
bool g_isDWM = false;

// Settings structure
struct
{
    int DisableWinA;
    bool DisableWinB;
    int DisableWinC;
    bool DisableWinD;
    bool DisableWinE;
    bool DisableWinF;
    bool DisableWinG;
    bool DisableWinH;
    bool DisableWinI;
    bool DisableWinJ;
    int DisableWinK;
    bool DisableWinM;
    int DisableWinN;
    bool DisableWinO;
    int DisableWinP;
    bool DisableWinQ;
    bool DisableWinR;
    bool DisableWinS;
    bool DisableWinT;
    int DisableWinU;
    bool DisableWinV;
    bool DisableWinW;
    bool DisableWinX;
    bool DisableWinY;
    bool DisableWinZ;
    int DisableWinSlash;
    bool DisableWinTab;
    bool DisableWinUp;
    bool DisableWinDown;
    bool DisableWinLeft;
    bool DisableWinRight;
    bool DisableWinHome;
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


int GetSettingIntSafe(PCWSTR settingName) {
    PCWSTR val = Wh_GetStringSetting(settingName);
    if (!val) return 0;
    int res = 0;
    if (wcscmp(val, L"true") == 0 || wcscmp(val, L"disable") == 0) res = 1;
    else if (wcscmp(val, L"false") == 0 || wcscmp(val, L"off") == 0) res = 0;
    else if (wcscmp(val, L"block") == 0) res = 2;
    else res = _wtoi(val); // fallback for legacy numbers
    Wh_FreeStringSetting(val);
    return res;
}

void LoadSettings()
{
    g_settings.DisableWinA = GetSettingIntSafe(L"SpecialShortcuts.DisableWinA");
    g_settings.DisableWinB = Wh_GetIntSetting(L"StandardShortcuts.DisableWinB");
    g_settings.DisableWinC = GetSettingIntSafe(L"SpecialShortcuts.DisableWinC");
    g_settings.DisableWinD = Wh_GetIntSetting(L"StandardShortcuts.DisableWinD");
    g_settings.DisableWinE = Wh_GetIntSetting(L"StandardShortcuts.DisableWinE");
    g_settings.DisableWinF = Wh_GetIntSetting(L"StandardShortcuts.DisableWinF");
    g_settings.DisableWinG = Wh_GetIntSetting(L"StandardShortcuts.DisableWinG");
    g_settings.DisableWinH = Wh_GetIntSetting(L"StandardShortcuts.DisableWinH");
    g_settings.DisableWinI = Wh_GetIntSetting(L"StandardShortcuts.DisableWinI");
    g_settings.DisableWinJ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinJ");
    g_settings.DisableWinK = GetSettingIntSafe(L"SpecialShortcuts.DisableWinK");
    g_settings.DisableWinM = Wh_GetIntSetting(L"StandardShortcuts.DisableWinM");
    g_settings.DisableWinN = GetSettingIntSafe(L"SpecialShortcuts.DisableWinN");
    g_settings.DisableWinO = Wh_GetIntSetting(L"StandardShortcuts.DisableWinO");
    g_settings.DisableWinP = GetSettingIntSafe(L"SpecialShortcuts.DisableWinP");
    g_settings.DisableWinQ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinQ");
    g_settings.DisableWinR = Wh_GetIntSetting(L"StandardShortcuts.DisableWinR");
    g_settings.DisableWinS = Wh_GetIntSetting(L"StandardShortcuts.DisableWinS");
    g_settings.DisableWinT = Wh_GetIntSetting(L"StandardShortcuts.DisableWinT");
    g_settings.DisableWinU = GetSettingIntSafe(L"SpecialShortcuts.DisableWinU");
    g_settings.DisableWinV = Wh_GetIntSetting(L"StandardShortcuts.DisableWinV");
    g_settings.DisableWinW = Wh_GetIntSetting(L"StandardShortcuts.DisableWinW");
    g_settings.DisableWinX = Wh_GetIntSetting(L"StandardShortcuts.DisableWinX");
    g_settings.DisableWinY = Wh_GetIntSetting(L"StandardShortcuts.DisableWinY");
    g_settings.DisableWinZ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinZ");
    g_settings.DisableWinSlash = GetSettingIntSafe(L"SpecialShortcuts.DisableWinSlash");
    g_settings.DisableWinTab = Wh_GetIntSetting(L"StandardShortcuts.DisableWinTab");
    g_settings.DisableWinUp = Wh_GetIntSetting(L"StandardShortcuts.DisableWinUp");
    g_settings.DisableWinDown = Wh_GetIntSetting(L"StandardShortcuts.DisableWinDown");
    g_settings.DisableWinLeft = Wh_GetIntSetting(L"StandardShortcuts.DisableWinLeft");
    g_settings.DisableWinRight = Wh_GetIntSetting(L"StandardShortcuts.DisableWinRight");
    g_settings.DisableWinHome = Wh_GetIntSetting(L"StandardShortcuts.DisableWinHome");
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
    g_settings.DisableWinSpace = Wh_GetIntSetting(L"StandardShortcuts.DisableWinSpace");
    g_settings.DisableWinShiftS = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftS");
    g_settings.DisableWinAltK = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltK");
    g_settings.DisableWinPeriod = Wh_GetIntSetting(L"StandardShortcuts.DisableWinPeriod");
    g_settings.DisableWinSemicolon = Wh_GetIntSetting(L"StandardShortcuts.DisableWinSemicolon");
    g_settings.DisableWinPrtSc = Wh_GetIntSetting(L"StandardShortcuts.DisableWinPrtSc");
    g_settings.DisableWinShiftLeft = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftLeft");
    g_settings.DisableWinShiftRight = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftRight");
    g_settings.DisableWinShiftUp = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftUp");
    g_settings.DisableWinShiftDown = Wh_GetIntSetting(L"StandardShortcuts.DisableWinShiftDown");
    g_settings.DisableOfficeHotkeys = Wh_GetIntSetting(L"StandardShortcuts.DisableOfficeHotkeys");
    g_settings.DisableWinAltD = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltD");
    g_settings.DisableWinAltB = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltB");
    g_settings.DisableWinAltR = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltR");
    g_settings.DisableWinAltG = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltG");
    g_settings.DisableWinAltPrtSc = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltPrtSc");
    g_settings.DisableWinAltT = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltT");
    g_settings.DisableWinAltM = Wh_GetIntSetting(L"StandardShortcuts.DisableWinAltM");
    g_settings.DisableWinCtrlShiftB = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlShiftB");
    g_settings.DisableWinCtrlQ = Wh_GetIntSetting(L"StandardShortcuts.DisableWinCtrlQ");
    g_settings.DisableAltShift = Wh_GetIntSetting(L"StandardShortcuts.DisableAltShift");
    g_settings.DisableWinKey = Wh_GetIntSetting(L"StandardShortcuts.DisableWinKey");
    g_settings.DisableCtrlEsc = Wh_GetIntSetting(L"StandardShortcuts.DisableCtrlEsc");
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
                case 'M': block = g_settings.DisableWinShiftM; break;
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
                case 'A': block = (g_settings.DisableWinA > 0); break;
                case 'B': block = g_settings.DisableWinB; break;
                case 'C': block = (g_settings.DisableWinC > 0); break;
                case 'D': block = g_settings.DisableWinD; break;
                case 'E': block = g_settings.DisableWinE; break;
                case 'F': block = g_settings.DisableWinF; break;
                case 'G': block = g_settings.DisableWinG; break;
                case 'H': block = g_settings.DisableWinH; break;
                case 'I': block = g_settings.DisableWinI; break;
                case 'J': block = g_settings.DisableWinJ; break;
                case 'K': block = (g_settings.DisableWinK > 0); break;
                case 'M': block = g_settings.DisableWinM; break;
                case 'N': block = (g_settings.DisableWinN > 0); break;
                case 'O': block = g_settings.DisableWinO; break;
                case 'P': block = (g_settings.DisableWinP > 0); break;
                case 'Q': block = g_settings.DisableWinQ; break;
                case 'R': block = g_settings.DisableWinR; break;
                case 'S': block = g_settings.DisableWinS; break;
                case 'T': block = g_settings.DisableWinT; break;
                case 'U': block = (g_settings.DisableWinU > 0); break;
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
                case VK_OEM_2: block = (g_settings.DisableWinSlash > 0); break;
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
            if (vk == 'A' && g_settings.DisableWinA == 2) return true;
            if (vk == 'C' && g_settings.DisableWinC == 2) return true;
            if (vk == 'K' && g_settings.DisableWinK == 2) return true;
            if (vk == 'N' && g_settings.DisableWinN == 2) return true;
            if (vk == 'P' && g_settings.DisableWinP == 2) return true;
            if (vk == 'U' && g_settings.DisableWinU == 2) return true;
            if (vk == VK_OEM_2 && g_settings.DisableWinSlash == 2) return true;
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
        // If the hotkey is a known hardcoded shell key (like Win+A for Action Center),
        // we MUST let Explorer successfully register it. If we fake a failure here, 
        // Explorer components fail to initialize and the user can't even open them 
        // with a manual mouse click on the taskbar tray icons!
        // The physical keyboard shortcut will still be blocked safely by our DWM LL hook.
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

bool IsFirstTimeInit()
{
    int initialized = Wh_GetIntValue(L"Initialized", 0);
    if (initialized == 0)
    {
        Wh_SetIntValue(L"Initialized", 1);
        return true; // First time
    }
    return false;
}

HANDLE g_restartExplorerPromptThread = NULL;
std::atomic<HWND> g_restartExplorerPromptWindow = NULL;

constexpr WCHAR kRestartExplorerPromptTitle[] = L"Disable Windows Shortcuts - Windhawk";
constexpr WCHAR kRestartExplorerPromptText[] = L"Explorer needs to be restarted to apply the changes to standard shortcuts. Restart now?";

void PromptForExplorerRestart()
{
    if (g_restartExplorerPromptThread)
    {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) != WAIT_OBJECT_0)
            return;
        CloseHandle(g_restartExplorerPromptThread);
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

        int button;
        if (SUCCEEDED(TaskDialogIndirect(&taskDialogConfig, &button, nullptr, nullptr)) && button == IDYES)
        {
            // By adding a slight timeout, we give Wh_ModUninit the exact chance to return gracefully to Windhawk
            // *before* explorer.exe violently closes. This prevents Windhawk from registering an uninit crash,
            // which avoids the backoff/slow re-init protection delays. We also use 'start' to detach the shell gracefully.
            WCHAR commandLine[] = L"cmd.exe /c \"timeout /t 1 /nobreak >nul & taskkill /F /IM explorer.exe & start explorer.exe\"";
            STARTUPINFO si = { .cb = sizeof(si) };
            PROCESS_INFORMATION pi{};
            if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
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
HANDLE g_hookThread = NULL;
std::atomic<bool> g_hookThreadRunning{false};
std::atomic<DWORD> g_hookThreadId{0};
bool g_suppressedKeys[256] = {false};
bool g_keyState[256] = {false}; // Track our own key states reliably

bool g_winKeyUsed = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // If nCode is less than zero, the hook procedure must pass the message
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyBoard->vkCode;
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        // Ignore programmatically injected keystrokes. 
        // This is extremely important because when a user manually clicks on a taskbar
        // tray icon (like Network/Volume to open Quick Settings), Explorer actually 
        // synthesizes a fake Win+A keystroke to trigger the flyout. If we block 
        // injected keys, clicking the tray icon with the mouse will fail!
        if (pKeyBoard->flags & LLKHF_INJECTED)
        {
            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        // Fast path for dummy key (used to mask Start Menu)
        if (vkCode == 0xFF)
            return CallNextHookEx(g_hHook, nCode, wParam, lParam);

        if (vkCode < 256)
        {
            if (isDown)
            {
                if (!g_keyState[vkCode])
                {
                    if (vkCode == VK_LWIN || vkCode == VK_RWIN)
                    {
                        g_winKeyUsed = false;
                    }
                    else if (vkCode != 0xFF)
                    {
                        g_winKeyUsed = true;
                    }
                }
                g_keyState[vkCode] = true;
            }
            if (isUp) g_keyState[vkCode] = false;
        }

        // --- 1. Pass Modifiers Through ---
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
                    bool hasWinState = g_keyState[VK_LWIN] || g_keyState[VK_RWIN] || (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
                    bool hasCtrlState = g_keyState[VK_CONTROL] || g_keyState[VK_LCONTROL] || g_keyState[VK_RCONTROL] || (GetAsyncKeyState(VK_CONTROL) & 0x8000);
                    
                    bool hasShiftState = g_keyState[VK_SHIFT] || g_keyState[VK_LSHIFT] || g_keyState[VK_RSHIFT] || (GetAsyncKeyState(VK_SHIFT) & 0x8000) || isShift;
                    bool hasAltState = g_keyState[VK_MENU] || g_keyState[VK_LMENU] || g_keyState[VK_RMENU] || (GetAsyncKeyState(VK_MENU) & 0x8000) || isAlt;

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

                bool hasWin = g_keyState[VK_LWIN] || g_keyState[VK_RWIN] || (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000) || isWin;
                bool hasCtrl = g_keyState[VK_CONTROL] || g_keyState[VK_LCONTROL] || g_keyState[VK_RCONTROL] || (GetAsyncKeyState(VK_CONTROL) & 0x8000) || (GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_RCONTROL) & 0x8000) || isCtrl;
                bool hasShift = g_keyState[VK_SHIFT] || g_keyState[VK_LSHIFT] || g_keyState[VK_RSHIFT] || (GetAsyncKeyState(VK_SHIFT) & 0x8000) || (GetAsyncKeyState(VK_LSHIFT) & 0x8000) || (GetAsyncKeyState(VK_RSHIFT) & 0x8000) || isShift;
                bool hasAlt = g_keyState[VK_MENU] || g_keyState[VK_LMENU] || g_keyState[VK_RMENU] || (GetAsyncKeyState(VK_MENU) & 0x8000) || (GetAsyncKeyState(VK_LMENU) & 0x8000) || (GetAsyncKeyState(VK_RMENU) & 0x8000) || isAlt;
                
                if (hasWin && hasCtrl && hasShift && hasAlt)
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

            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        // --- 3. Handle UP Events ---
        if (isUp)
        {
            if (vkCode < 256 && g_suppressedKeys[vkCode])
            {
                g_suppressedKeys[vkCode] = false;
                return 1; // Suppress the UP event
            }
            return CallNextHookEx(g_hHook, nCode, wParam, lParam);
        }

        // --- 4. Handle DOWN Events ---
        if (isDown)
        {
            // Use our own state tracking to ensure we don't miss modifiers 
            // Fallback to GetAsyncKeyState in case hook missed the down event (e.g. started while key held)
            bool hasWin = g_keyState[VK_LWIN] || g_keyState[VK_RWIN] || (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
            bool hasCtrl = g_keyState[VK_CONTROL] || g_keyState[VK_LCONTROL] || g_keyState[VK_RCONTROL] || (GetAsyncKeyState(VK_CONTROL) & 0x8000) || (GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
            bool hasShift = g_keyState[VK_SHIFT] || g_keyState[VK_LSHIFT] || g_keyState[VK_RSHIFT] || (GetAsyncKeyState(VK_SHIFT) & 0x8000) || (GetAsyncKeyState(VK_LSHIFT) & 0x8000) || (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
            bool hasAlt = g_keyState[VK_MENU] || g_keyState[VK_LMENU] || g_keyState[VK_RMENU] || (GetAsyncKeyState(VK_MENU) & 0x8000) || (GetAsyncKeyState(VK_LMENU) & 0x8000) || (GetAsyncKeyState(VK_RMENU) & 0x8000);

            // Convert to MOD_* flags for evaluating
            UINT fsModifiers = 0;
            if (hasWin) fsModifiers |= MOD_WIN;
            if (hasCtrl) fsModifiers |= MOD_CONTROL;
            if (hasShift) fsModifiers |= MOD_SHIFT;
            if (hasAlt) fsModifiers |= MOD_ALT;

            // Check if this hotkey is disabled in settings AND is hardcoded
            if (ShouldBlockHotkey(fsModifiers, vkCode) && IsKnownHardcodedHotkey(fsModifiers, vkCode))
            {
                if (vkCode < 256)
                    g_suppressedKeys[vkCode] = true;

                if (hasWin) {
                    // AHK Start Menu Masking Trick
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
        return 1;

    // Dedicated message pump to keep the hook alive and responsive
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_hHook);
    return 0;
}

void StartHookThread()
{
    if (g_hookThreadRunning) return;
    
    g_hookThreadRunning = true;
    g_hookThread = CreateThread(NULL, 0, HookThread, NULL, 0, (LPDWORD)&g_hookThreadId);
}

void StopHookThread()
{
    if (g_hookThreadRunning)
    {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
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
    return (g_settings.DisableWinA == 2) || (g_settings.DisableWinC == 2) || 
           (g_settings.DisableWinK == 2) || (g_settings.DisableWinN == 2) || 
           (g_settings.DisableWinP == 2) || (g_settings.DisableWinU == 2) || 
           (g_settings.DisableWinSlash == 2) || 
           g_settings.DisableWinTab ||
           g_settings.DisableWinUp || g_settings.DisableWinDown || 
           g_settings.DisableWinLeft || g_settings.DisableWinRight ||
           g_settings.DisableWinShiftUp || g_settings.DisableWinShiftDown || 
           g_settings.DisableWinShiftLeft || g_settings.DisableWinShiftRight ||
           g_settings.DisableWinCtrlShiftB || g_settings.DisableOfficeHotkeys ||
           g_settings.DisableWinSpace || g_settings.DisableAltShift || 
           g_settings.DisableWinKey || g_settings.DisableCtrlEsc;
}

// ----------------------------------------------------------------------------
// Windhawk Mod Entry Points
// ----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    WCHAR exeName[MAX_PATH];
    GetModuleFileNameW(NULL, exeName, MAX_PATH);
    std::wstring exeStr(exeName);
    std::transform(exeStr.begin(), exeStr.end(), exeStr.begin(), ::towlower);

    g_isExplorer = (exeStr.find(L"explorer.exe") != std::wstring::npos);
    g_isDWM = (exeStr.find(L"dwm.exe") != std::wstring::npos);

    LoadSettings();

    if (g_isDWM && !NeedsDwmHook())
    {
        return FALSE; // Unload from DWM if no hardcoded keys are disabled
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

        // Check if Explorer has already registered standard hotkeys.
        // We use Win+R as a probe. If it fails, Explorer is mid-session and already owns it.
        if (!IsFirstTimeInit())
        {
            if (!RegisterHotKey(NULL, 0x1337, MOD_WIN | MOD_NOREPEAT, 'R'))
            {
                PromptForExplorerRestart();
            }
            else
            {
                UnregisterHotKey(NULL, 0x1337);
            }
        }
    }

    if (g_isDWM)
        StartHookThread();

    return TRUE;
}

void Wh_ModUninit()
{
    if (g_isDWM)
        StopHookThread();

    if (g_isExplorer)
    {
        // Use the native prompt instead of PowerShell. 
        // We call the existing prompt function and wait for it to complete.
        PromptForExplorerRestart();
    }

    if (g_restartExplorerPromptThread)
    {
        WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
        CloseHandle(g_restartExplorerPromptThread);
        g_restartExplorerPromptThread = nullptr;
    }
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
    
    if (g_isDWM)
    {
        if (NeedsDwmHook())
            StartHookThread();
        else
            StopHookThread();
    }
    
    if (g_isExplorer)
        PromptForExplorerRestart();
}
