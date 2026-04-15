// ==WindhawkMod==
// @id              disable-windows-shortcuts
// @name            Disable Windows Shortcuts
// @description     Selectively disable Windows keyboard shortcuts with individual toggles
// @version         1.0.0
// @author          Lone
// @github          https://github.com/Louis047
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Disable Windows Shortcuts
Selectively disable Windows keyboard shortcuts with individual toggles for each shortcut.
## Features
- Disable any Windows key combination
- Individual toggle for each shortcut
- Hooks RegisterHotKey to block hotkey registration
## Supported Shortcuts
### General
- Win+B through Win+Z (excluding Win, Win+L, Win+Q)
- Win+Tab, Win+Arrow Keys, Win+Home
### With Modifiers
- Win+Shift combinations
- Win+Ctrl combinations
- Win+Alt combinations
- Win+Ctrl+Shift combinations
### Special
- Win+Number (0-9) for taskbar apps
- Win+Plus/Minus for Magnifier
- Win+Period/Semicolon for Emoji picker
- Office hotkeys (Ctrl+Shift+Alt+Win)
## Notes
- **Explorer restart is required** to apply setting changes or after
  uninstalling the mod. You will be prompted to restart Explorer.
- Win key (Start Menu) is handled by the "Block Start Menu and Hosts" mod
- Win+L (Lock PC) cannot be blocked through standard hooks
- Win+Q is redundant with Win+S (both open Search)
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- DisableWinB: false
  $name: Win+B
  $description: Focus system tray
- DisableWinC: false
  $name: Win+C
  $description: Cortana / Copilot
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
- DisableWinK: false
  $name: Win+K
  $description: Connect (Cast)
- DisableWinM: false
  $name: Win+M
  $description: Minimize all windows
- DisableWinN: false
  $name: Win+N
  $description: Notification Center
- DisableWinO: false
  $name: Win+O
  $description: Lock device orientation
- DisableWinP: false
  $name: Win+P
  $description: Project / Display mode
- DisableWinR: false
  $name: Win+R
  $description: Run dialog
- DisableWinS: false
  $name: Win+S
  $description: Search
- DisableWinT: false
  $name: Win+T
  $description: Cycle taskbar apps
- DisableWinU: false
  $name: Win+U
  $description: Accessibility Settings
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
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <commctrl.h>
#include <atomic>
// Settings structure
struct
{
  bool DisableWinB;
  bool DisableWinC;
  bool DisableWinD;
  bool DisableWinE;
  bool DisableWinF;
  bool DisableWinG;
  bool DisableWinH;
  bool DisableWinI;
  bool DisableWinJ;
  bool DisableWinK;
  bool DisableWinM;
  bool DisableWinN;
  bool DisableWinO;
  bool DisableWinP;
  bool DisableWinR;
  bool DisableWinS;
  bool DisableWinT;
  bool DisableWinU;
  bool DisableWinV;
  bool DisableWinW;
  bool DisableWinX;
  bool DisableWinY;
  bool DisableWinZ;
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
} g_settings;
// ============================================================================
// Helper functions
// ============================================================================
void LoadSettings()
{
  g_settings.DisableWinB = Wh_GetIntSetting(L"DisableWinB");
  g_settings.DisableWinC = Wh_GetIntSetting(L"DisableWinC");
  g_settings.DisableWinD = Wh_GetIntSetting(L"DisableWinD");
  g_settings.DisableWinE = Wh_GetIntSetting(L"DisableWinE");
  g_settings.DisableWinF = Wh_GetIntSetting(L"DisableWinF");
  g_settings.DisableWinG = Wh_GetIntSetting(L"DisableWinG");
  g_settings.DisableWinH = Wh_GetIntSetting(L"DisableWinH");
  g_settings.DisableWinI = Wh_GetIntSetting(L"DisableWinI");
  g_settings.DisableWinJ = Wh_GetIntSetting(L"DisableWinJ");
  g_settings.DisableWinK = Wh_GetIntSetting(L"DisableWinK");
  g_settings.DisableWinM = Wh_GetIntSetting(L"DisableWinM");
  g_settings.DisableWinN = Wh_GetIntSetting(L"DisableWinN");
  g_settings.DisableWinO = Wh_GetIntSetting(L"DisableWinO");
  g_settings.DisableWinP = Wh_GetIntSetting(L"DisableWinP");
  g_settings.DisableWinR = Wh_GetIntSetting(L"DisableWinR");
  g_settings.DisableWinS = Wh_GetIntSetting(L"DisableWinS");
  g_settings.DisableWinT = Wh_GetIntSetting(L"DisableWinT");
  g_settings.DisableWinU = Wh_GetIntSetting(L"DisableWinU");
  g_settings.DisableWinV = Wh_GetIntSetting(L"DisableWinV");
  g_settings.DisableWinW = Wh_GetIntSetting(L"DisableWinW");
  g_settings.DisableWinX = Wh_GetIntSetting(L"DisableWinX");
  g_settings.DisableWinY = Wh_GetIntSetting(L"DisableWinY");
  g_settings.DisableWinZ = Wh_GetIntSetting(L"DisableWinZ");
  g_settings.DisableWinTab = Wh_GetIntSetting(L"DisableWinTab");
  g_settings.DisableWinUp = Wh_GetIntSetting(L"DisableWinUp");
  g_settings.DisableWinDown = Wh_GetIntSetting(L"DisableWinDown");
  g_settings.DisableWinLeft = Wh_GetIntSetting(L"DisableWinLeft");
  g_settings.DisableWinRight = Wh_GetIntSetting(L"DisableWinRight");
  g_settings.DisableWinHome = Wh_GetIntSetting(L"DisableWinHome");
  g_settings.DisableWinShiftM = Wh_GetIntSetting(L"DisableWinShiftM");
  g_settings.DisableWinComma = Wh_GetIntSetting(L"DisableWinComma");
  g_settings.DisableWinPause = Wh_GetIntSetting(L"DisableWinPause");
  g_settings.DisableWinCtrlD = Wh_GetIntSetting(L"DisableWinCtrlD");
  g_settings.DisableWinCtrlF4 = Wh_GetIntSetting(L"DisableWinCtrlF4");
  g_settings.DisableWinCtrlLeft = Wh_GetIntSetting(L"DisableWinCtrlLeft");
  g_settings.DisableWinCtrlRight = Wh_GetIntSetting(L"DisableWinCtrlRight");
  g_settings.DisableWinNumbers = Wh_GetIntSetting(L"DisableWinNumbers");
  g_settings.DisableWinShiftNumbers = Wh_GetIntSetting(L"DisableWinShiftNumbers");
  g_settings.DisableWinCtrlNumbers = Wh_GetIntSetting(L"DisableWinCtrlNumbers");
  g_settings.DisableWinAltNumbers = Wh_GetIntSetting(L"DisableWinAltNumbers");
  g_settings.DisableWinPlus = Wh_GetIntSetting(L"DisableWinPlus");
  g_settings.DisableWinMinus = Wh_GetIntSetting(L"DisableWinMinus");
  g_settings.DisableWinEsc = Wh_GetIntSetting(L"DisableWinEsc");
  g_settings.DisableWinCtrlEnter = Wh_GetIntSetting(L"DisableWinCtrlEnter");
  g_settings.DisableWinCtrlC = Wh_GetIntSetting(L"DisableWinCtrlC");
  g_settings.DisableWinCtrlN = Wh_GetIntSetting(L"DisableWinCtrlN");
  g_settings.DisableWinCtrlO = Wh_GetIntSetting(L"DisableWinCtrlO");
  g_settings.DisableWinCtrlS = Wh_GetIntSetting(L"DisableWinCtrlS");
  g_settings.DisableWinSpace = Wh_GetIntSetting(L"DisableWinSpace");
  g_settings.DisableWinShiftS = Wh_GetIntSetting(L"DisableWinShiftS");
  g_settings.DisableWinAltK = Wh_GetIntSetting(L"DisableWinAltK");
  g_settings.DisableWinPeriod = Wh_GetIntSetting(L"DisableWinPeriod");
  g_settings.DisableWinSemicolon = Wh_GetIntSetting(L"DisableWinSemicolon");
  g_settings.DisableWinPrtSc = Wh_GetIntSetting(L"DisableWinPrtSc");
  g_settings.DisableWinShiftLeft = Wh_GetIntSetting(L"DisableWinShiftLeft");
  g_settings.DisableWinShiftRight = Wh_GetIntSetting(L"DisableWinShiftRight");
  g_settings.DisableWinShiftUp = Wh_GetIntSetting(L"DisableWinShiftUp");
  g_settings.DisableWinShiftDown = Wh_GetIntSetting(L"DisableWinShiftDown");
  g_settings.DisableOfficeHotkeys = Wh_GetIntSetting(L"DisableOfficeHotkeys");
  g_settings.DisableWinAltD = Wh_GetIntSetting(L"DisableWinAltD");
  g_settings.DisableWinAltB = Wh_GetIntSetting(L"DisableWinAltB");
  g_settings.DisableWinAltR = Wh_GetIntSetting(L"DisableWinAltR");
  g_settings.DisableWinAltG = Wh_GetIntSetting(L"DisableWinAltG");
  g_settings.DisableWinAltPrtSc = Wh_GetIntSetting(L"DisableWinAltPrtSc");
  g_settings.DisableWinAltT = Wh_GetIntSetting(L"DisableWinAltT");
  g_settings.DisableWinAltM = Wh_GetIntSetting(L"DisableWinAltM");
  g_settings.DisableWinCtrlShiftB = Wh_GetIntSetting(L"DisableWinCtrlShiftB");
  g_settings.DisableWinCtrlQ = Wh_GetIntSetting(L"DisableWinCtrlQ");
}
// Check if vk is a number key (0-9)
bool IsNumberKey(UINT vk)
{
  return (vk >= '0' && vk <= '9');
}
// ============================================================================
// RegisterHotKey hook
// ============================================================================
typedef BOOL(WINAPI *RegisterHotKey_t)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
RegisterHotKey_t RegisterHotKey_Original;
BOOL WINAPI RegisterHotKey_Hook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
  // Strip MOD_NOREPEAT for comparison (Windows often registers with this flag)
  UINT baseMods = fsModifiers & ~MOD_NOREPEAT;
  bool hasWin = (baseMods & MOD_WIN) != 0;
  bool hasShift = (baseMods & MOD_SHIFT) != 0;
  bool hasCtrl = (baseMods & MOD_CONTROL) != 0;
  bool hasAlt = (baseMods & MOD_ALT) != 0;
  bool block = false;

  // Office hotkeys - exact match including MOD_NOREPEAT (Ctrl+Shift+Alt+Win)
  if (fsModifiers == (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN | MOD_NOREPEAT) &&
      g_settings.DisableOfficeHotkeys)
  {
    // Office hotkey VKs: W, T, Y, O, P, D, L, X, N, SPACE, or no VK
    if (!vk || vk == 'W' || vk == 'T' || vk == 'Y' || vk == 'O' ||
        vk == 'P' || vk == 'D' || vk == 'L' || vk == 'X' || vk == 'N' ||
        vk == VK_SPACE)
    {
      block = true;
    }
  }
  else if (hasWin)
  {
    // Win+Ctrl+Shift combinations
    if (hasCtrl && hasShift && !hasAlt)
    {
      if (vk == 'B' && g_settings.DisableWinCtrlShiftB)
        block = true;
    }
    // Win+Shift combinations
    else if (hasShift && !hasCtrl && !hasAlt)
    {
      switch (vk)
      {
      case 'M':
        if (g_settings.DisableWinShiftM)
          block = true;
        break;
      case 'S':
        if (g_settings.DisableWinShiftS)
          block = true;
        break;
      case VK_LEFT:
        if (g_settings.DisableWinShiftLeft)
          block = true;
        break;
      case VK_RIGHT:
        if (g_settings.DisableWinShiftRight)
          block = true;
        break;
      case VK_UP:
        if (g_settings.DisableWinShiftUp)
          block = true;
        break;
      case VK_DOWN:
        if (g_settings.DisableWinShiftDown)
          block = true;
        break;
      }
      if (g_settings.DisableWinShiftNumbers && IsNumberKey(vk))
        block = true;
    }
    // Win+Ctrl combinations
    else if (hasCtrl && !hasShift && !hasAlt)
    {
      switch (vk)
      {
      case 'C':
        if (g_settings.DisableWinCtrlC)
          block = true;
        break;
      case 'D':
        if (g_settings.DisableWinCtrlD)
          block = true;
        break;
      case 'N':
        if (g_settings.DisableWinCtrlN)
          block = true;
        break;
      case 'O':
        if (g_settings.DisableWinCtrlO)
          block = true;
        break;
      case 'Q':
        if (g_settings.DisableWinCtrlQ)
          block = true;
        break;
      case 'S':
        if (g_settings.DisableWinCtrlS)
          block = true;
        break;
      case VK_F4:
        if (g_settings.DisableWinCtrlF4)
          block = true;
        break;
      case VK_LEFT:
        if (g_settings.DisableWinCtrlLeft)
          block = true;
        break;
      case VK_RIGHT:
        if (g_settings.DisableWinCtrlRight)
          block = true;
        break;
      case VK_RETURN:
        if (g_settings.DisableWinCtrlEnter)
          block = true;
        break;
      }
      if (g_settings.DisableWinCtrlNumbers && IsNumberKey(vk))
        block = true;
    }
    // Win+Alt combinations
    else if (hasAlt && !hasShift && !hasCtrl)
    {
      switch (vk)
      {
      case 'B':
        if (g_settings.DisableWinAltB)
          block = true;
        break;
      case 'D':
        if (g_settings.DisableWinAltD)
          block = true;
        break;
      case 'G':
        if (g_settings.DisableWinAltG)
          block = true;
        break;
      case 'K':
        if (g_settings.DisableWinAltK)
          block = true;
        break;
      case 'M':
        if (g_settings.DisableWinAltM)
          block = true;
        break;
      case 'R':
        if (g_settings.DisableWinAltR)
          block = true;
        break;
      case 'T':
        if (g_settings.DisableWinAltT)
          block = true;
        break;
      case VK_SNAPSHOT:
        if (g_settings.DisableWinAltPrtSc)
          block = true;
        break;
      }
      if (g_settings.DisableWinAltNumbers && IsNumberKey(vk))
        block = true;
    }
    // Win + key only
    else if (!hasShift && !hasCtrl && !hasAlt)
    {
      switch (vk)
      {
      case 'B':
        if (g_settings.DisableWinB)
          block = true;
        break;
      case 'C':
        if (g_settings.DisableWinC)
          block = true;
        break;
      case 'D':
        if (g_settings.DisableWinD)
          block = true;
        break;
      case 'E':
        if (g_settings.DisableWinE)
          block = true;
        break;
      case 'F':
        if (g_settings.DisableWinF)
          block = true;
        break;
      case 'G':
        if (g_settings.DisableWinG)
          block = true;
        break;
      case 'H':
        if (g_settings.DisableWinH)
          block = true;
        break;
      case 'I':
        if (g_settings.DisableWinI)
          block = true;
        break;
      case 'J':
        if (g_settings.DisableWinJ)
          block = true;
        break;
      case 'K':
        if (g_settings.DisableWinK)
          block = true;
        break;
      case 'M':
        if (g_settings.DisableWinM)
          block = true;
        break;
      case 'N':
        if (g_settings.DisableWinN)
          block = true;
        break;
      case 'O':
        if (g_settings.DisableWinO)
          block = true;
        break;
      case 'P':
        if (g_settings.DisableWinP)
          block = true;
        break;
      case 'R':
        if (g_settings.DisableWinR)
          block = true;
        break;
      case 'S':
        if (g_settings.DisableWinS)
          block = true;
        break;
      case 'T':
        if (g_settings.DisableWinT)
          block = true;
        break;
      case 'U':
        if (g_settings.DisableWinU)
          block = true;
        break;
      case 'V':
        if (g_settings.DisableWinV)
          block = true;
        break;
      case 'W':
        if (g_settings.DisableWinW)
          block = true;
        break;
      case 'X':
        if (g_settings.DisableWinX)
          block = true;
        break;
      case 'Y':
        if (g_settings.DisableWinY)
          block = true;
        break;
      case 'Z':
        if (g_settings.DisableWinZ)
          block = true;
        break;
      case VK_TAB:
        if (g_settings.DisableWinTab)
          block = true;
        break;
      case VK_UP:
        if (g_settings.DisableWinUp)
          block = true;
        break;
      case VK_DOWN:
        if (g_settings.DisableWinDown)
          block = true;
        break;
      case VK_LEFT:
        if (g_settings.DisableWinLeft)
          block = true;
        break;
      case VK_RIGHT:
        if (g_settings.DisableWinRight)
          block = true;
        break;
      case VK_HOME:
        if (g_settings.DisableWinHome)
          block = true;
        break;
      case VK_OEM_COMMA:
        if (g_settings.DisableWinComma)
          block = true;
        break;
      case VK_PAUSE:
        if (g_settings.DisableWinPause)
          block = true;
        break;
      case VK_OEM_PLUS:
        if (g_settings.DisableWinPlus)
          block = true;
        break;
      case VK_OEM_MINUS:
        if (g_settings.DisableWinMinus)
          block = true;
        break;
      case VK_ESCAPE:
        if (g_settings.DisableWinEsc)
          block = true;
        break;
      case VK_SPACE:
        if (g_settings.DisableWinSpace)
          block = true;
        break;
      case VK_OEM_PERIOD:
        if (g_settings.DisableWinPeriod)
          block = true;
        break;
      case VK_OEM_1:
        if (g_settings.DisableWinSemicolon)
          block = true;
        break;
      case VK_SNAPSHOT:
        if (g_settings.DisableWinPrtSc)
          block = true;
        break;
      }
      if (g_settings.DisableWinNumbers && IsNumberKey(vk))
        block = true;
    }
  }
  if (block)
  {
    Wh_Log(L"[RegisterHotKey] Blocked: mods=0x%X, vk=0x%X", fsModifiers, vk);
    SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
    return FALSE;
  }
  return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
}
// ============================================================================
// Explorer restart prompt
// Taken from Taskbar height and icon size mod by Ramen Software (m417z)
// https://github.com/ramensoftware/windhawk-mods/blob/7e0bd27ae1d12ae639497fbc9b48bb791f98b078/mods/taskbar-icon-size.wh.cpp#L723
// ============================================================================
HANDLE g_restartExplorerPromptThread;
std::atomic<HWND> g_restartExplorerPromptWindow;

constexpr WCHAR kRestartExplorerPromptTitle[] =
    L"Disable Windows Shortcuts - Windhawk";
constexpr WCHAR kRestartExplorerPromptText[] =
    L"Explorer needs to be restarted to apply the new changes. "
    L"Restart now?";

void PromptForExplorerRestart()
{
  if (g_restartExplorerPromptThread)
  {
    if (WaitForSingleObject(g_restartExplorerPromptThread, 0) !=
        WAIT_OBJECT_0)
    {
      return;
    }

    CloseHandle(g_restartExplorerPromptThread);
  }

  g_restartExplorerPromptThread = CreateThread(
      nullptr, 0,
      [](LPVOID lpParameter) WINAPI -> DWORD {
        TASKDIALOGCONFIG taskDialogConfig{
            .cbSize = sizeof(taskDialogConfig),
            .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
            .dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
            .pszWindowTitle = kRestartExplorerPromptTitle,
            .pszMainIcon = TD_INFORMATION_ICON,
            .pszContent = kRestartExplorerPromptText,
            .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam,
                             LPARAM lParam, LONG_PTR lpRefData)
                              WINAPI -> HRESULT {
              switch (msg)
              {
              case TDN_CREATED:
                g_restartExplorerPromptWindow = hwnd;
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE);
                break;

              case TDN_DESTROYED:
                g_restartExplorerPromptWindow = nullptr;
                break;
              }

              return S_OK;
            },
        };

        int button;
        if (SUCCEEDED(TaskDialogIndirect(&taskDialogConfig, &button,
                                         nullptr, nullptr)) &&
            button == IDYES)
        {
          WCHAR commandLine[] =
              L"cmd.exe /c "
              L"\"taskkill /F /IM explorer.exe & start explorer\"";
          STARTUPINFO si = {
              .cb = sizeof(si),
          };
          PROCESS_INFORMATION pi{};
          if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE,
                            0, nullptr, nullptr, &si, &pi))
          {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
          }
        }

        return 0;
      },
      nullptr, 0, nullptr);
}

// ============================================================================
// Windhawk mod entry points
// ============================================================================
BOOL Wh_ModInit()
{
  Wh_Log(L"Initializing Disable Windows Shortcuts mod v1.0");
  LoadSettings();

  // Hook RegisterHotKey to block hotkey registration
  HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
  if (hUser32)
  {
    void *pRegisterHotKey = (void *)GetProcAddress(hUser32, "RegisterHotKey");
    if (pRegisterHotKey)
    {
      Wh_SetFunctionHook(pRegisterHotKey, (void *)RegisterHotKey_Hook, (void **)&RegisterHotKey_Original);
      Wh_Log(L"Hooked RegisterHotKey");
    }
  }
  Wh_Log(L"Mod initialization complete");
  return TRUE;
}
void Wh_ModUninit()
{
  Wh_Log(L"Uninitializing Disable Windows Shortcuts mod");

  HWND restartExplorerPromptWindow = g_restartExplorerPromptWindow;
  if (restartExplorerPromptWindow)
  {
    PostMessage(restartExplorerPromptWindow, WM_CLOSE, 0, 0);
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
  Wh_Log(L"Settings changed, prompting for explorer restart...");
  LoadSettings();
  PromptForExplorerRestart();
}