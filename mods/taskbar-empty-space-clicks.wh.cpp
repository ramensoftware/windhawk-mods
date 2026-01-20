// ==WindhawkMod==
// @id              taskbar-empty-space-clicks
// @name            Click on empty taskbar space
// @description     Trigger custom action when empty space on a taskbar is clicked. Various mouse clicks and keyboard modifiers are supported.
// @version         2.5
// @author          m1lhaus
// @github          https://github.com/m1lhaus
// @include         explorer.exe
// @compilerOptions -DWINVER=0x0A00 -lcomctl32 -loleaut32 -lole32 -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/m1lhaus/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m1lhaus/windhawk-mods

// ==WindhawkModReadme==
/*
# Click on empty taskbar space

This mod lets you assign an action to a mouse click on the Windows taskbar. Single, double, and triple clicks are supported - both mouse and touchscreen clicks. You can also assign a keyboard modifier to the action. For example, you can set up a double-click on the taskbar to open Task Manager while holding down the Ctrl key. The mod is designed to be as flexible as possible. You can assign any action to any mouse click or touchscreen tap. You can also assign multiple actions to the same trigger. This mod reacts when empty space on the taskbar is clicked. Buttons, menus, or other taskbar functions are not affected. Click events are normally forwarded to the system, so you can still use the taskbar as usual. Both primary and secondary taskbars are supported.

## Supported actions

1. **Combine Taskbar buttons** - Toggle combining of Taskbar buttons between two states set in the Settings menu (not available on older Windows 11 versions)
2. **Ctrl+Alt+Tab** - Opens Ctrl+Alt+Tab dialog (repeatedly cycles through windows until closed)
3. **Hide desktop icons** - Toggle show/hide of all desktop icons
4. **Media Next Track** - Skip to next track
5. **Media Play/Pause** - Toggle play/pause for media playback
6. **Media Previous Track** - Skip to previous track
7. **Mute system volume** - Toggle mute of system volume (all sound)
8. **Open Start menu** - Sends Win key press to open Start menu
9. **Show desktop** - Toggle show/hide desktop
10. **Switch virtual desktop** - Switch to next or previous virtual desktop
11. **Open application, path or URL** - Starts arbitrary application executable, opens path in Explorer or URL in web browser
12. **Task Manager** - Opens Windows default Task Manager
13. **Taskbar auto-hide** - Toggle Windows taskbar auto-hide feature
14. **Toggle Taskbar alignment** - Toggle taskbar icon alignment between left and center (Windows 11 only)
15. **Virtual key press** - Sends virtual keypress (keyboard shortcut) to the system
16. **Win+Tab** - Opens Win+Tab dialog

### Example

The following animation shows how to set up the **taskbar auto-hide** feature toggle on middle mouse button click and **toggle volume mute** on Ctrl + double-click.

![How to set "Click on empty taskbar space" Windhawk mod](https://i.imgur.com/b6rBLfF.gif)

Once set, a simple middle-click on empty taskbar space will toggle the auto-hide feature:

![Demonstration of Toggle taskbar autohide mod for Windhawk](https://i.imgur.com/BRQrVnX.gif)

## Supported triggers

- **Keyboard** - Optional. Keyboard keypress modifiers. If None is selected or added, the modifier is ignored.
    - **Left Ctrl** - Left Ctrl key
    - **Left Shift** - Left Shift key
    - **Left Alt** - Left Alt key
    - **Win** - Windows key
    - **Right Ctrl** - Right Ctrl key
    - **Right Shift** - Right Shift key
    - **Right Alt** - Right Alt key
- **Mouse** - Required. Mouse click or touchscreen tap trigger. If None is selected, the entire trigger+action is ignored.
    - **Left** - Mouse left button click
    - **Left Double** - Mouse left button double-click
    - **Left Triple** - Mouse left button triple-click
    - **Middle** - Mouse middle button click
    - **Middle Double** - Mouse middle button double-click
    - **Middle Triple** - Mouse middle button triple-click
    - **Right** - Mouse right button click
    - **Right Double** - Mouse right button double-click
    - **Right Triple** - Mouse right button triple-click
    - **Side Button 1** - Mouse side button 1 click (mouse button 4)
    - **Side Button 1 Double** - Mouse side button 1 double-click (mouse button 4)
    - **Side Button 1 Triple** - Mouse side button 1 triple-click (mouse button 4)
    - **Side Button 2** - Mouse side button 2 click (mouse button 5)
    - **Side Button 2 Double** - Mouse side button 2 double-click(mouse button 5)
    - **Side Button 2 Triple** - Mouse side button 2 triple-click (mouse button 5)
    - **Tap** - Touchscreen single tap
    - **Tap Double** - Touchscreen double tap
    - **Tap Triple** - Touchscreen triple tap

## Additional arguments

Some actions support or require additional arguments. You can set them in the Settings menu. Arguments are separated by semicolon. For example: `arg1;arg2`.

1. Combine Taskbar buttons - `priTaskBarBtnState1;priTaskBarBtnState2;secTaskBarBtnState1;secTaskBarBtnState2`
    - priTaskBarBtnState1: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - priTaskBarBtnState2: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - secTaskBarBtnState1: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - secTaskBarBtnState2: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - Example: `COMBINE_ALWAYS;COMBINE_WHEN_FULL;COMBINE_ALWAYS;COMBINE_NEVER`
2. Ctrl+Alt+Tab - argument `reverse` inverts the order of iteration through the windows (Ctrl+Alt+Shift+Tab behavior)
    - You can repeat the action to cycle through the windows like you would with a keyboard. If you click outside the taskbar or your next click gesture is not Ctrl+Alt+Tab trigger, the dialog will close.
3. Hide desktop icons - no additional arguments supported
4. Media Next Track - no additional arguments supported
5. Media Play/Pause - no additional arguments supported
6. Media Previous Track - no additional arguments supported
7. Mute system volume - no additional arguments supported
8. Open Start menu - no additional arguments supported
9. Show desktop - no additional arguments supported
10. Switch virtual desktop - direction is forward, if `reverse` argument is provided, direction is backward
11. Open application, path or URL - `applicationPath arg1 arg2 ... argN`
    - Example: `"c:\Program Files\Notepad++\notepad++.exe" C:\Users\username\Desktop\test.txt` - use quotes around paths with spaces
    - Example: `uac;C:\Windows\System32\notepad.exe C:\Windows\System32\drivers\etc\hosts` - start application with elevated privileges (UAC prompt will appear)
    - Example: `python.exe D:\MyScripts\my_python_script.py arg1 "arg 2 with space" arg3` - user must handle proper quoting of arguments
    - Example: `cmd.exe /c echo Hello & pause` - execute shell commands
    - Example: `https://windhawk.net/mods/` - open URL in default web browser
    - Example: `c:\Users\John Doe\Documents\` - open folder in Explorer
    - Example: `shell:Recent` - open special shell folder in Explorer ([more special shell commands](https://www.winhelponline.com/blog/shell-commands-to-access-the-special-folders/))
    - Uses [ShellExecute](https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea) to open applications, paths, or URLs. If the command starts with the special keyword `uac`, the application will be started with elevated privileges (UAC prompt will appear).
    - The command line parser attempts to intelligently handle spaces in file paths and arguments. However, if you encounter issues, enclose paths containing spaces in double quotes.
    - Error codes and error messages can be found in the mod log if the application fails to start.
12. Task Manager - no additional arguments supported
13. Taskbar auto-hide - no additional arguments supported
14. Toggle Taskbar alignment - no additional arguments supported
15. Virtual key press - `virtualKey1;virtualKey2;...;virtualKeyN`
    - Example: `0x5B;0x45` or `0x7A;focusPreviousWindow`
    - Each text field corresponds to one virtual key press. Fill in hexadecimal key codes of the keys you want to press. Key codes are defined in [Win32 inputdev docs](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes). Use only hexadecimal (0x) or decimal format for a key code! Example: (0x5B and 0x45) corresponds to (Win + E) shortcut that opens an Explorer window. If your key combination has no effect, check the log for more information.
    - There is a special keyword `focusPreviousWindow` that can be used to set focus back to the previously active window. This is useful when you want to send keypresses to the last active window instead of the taskbar. That way you can, for example, turn on fullscreen mode in the web browser by sending the F11 key. You can use this keyword anywhere in the sequence of virtual keys.
    - Please note that some special keyboard shortcuts like Win+L or Ctrl+Alt+Delete cannot be sent via the inputdev interface.
16. Win+Tab - no additional arguments supported

## Example presets

Windows lets you set few handy touchpad gestures for media playback or window control. However, if you are using a mouse, you might miss those gestures.

Following examples serves as an inspiration for setting up your own actions. Of course you are not limited to these examples only. You can set up any action to any mouse click or touchscreen tap. Especially with the virtual key press action, you can setup own macros and shortcuts.

### Media controls

- **Side Button 1 click** - Media Previous Track
- **Middle click** - Media Play/Pause
- **Side Button 2 click** - Media Next Track
- **Mouse scrolling** with [Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control) mod installed - Volume control

### Window management

- **Side Button 1 click** - Ctrl+Alt+Tab
- **Middle click** - Win+Tab
- **Side Button 2 click** - Ctrl+Alt+Shift+Tab (use `reverse` argument)
- **Left double-click** - Switch virtual desktop forward
- **Right double-click** - Switch virtual desktop backward (use `reverse` argument)

Consider installing [Alt+Tab per monitor](https://windhawk.net/mods/alt-tab-per-monitor) mod if you are using multiple monitors. Check out [Virtual Desktop Helper](https://windhawk.net/mods/virtual-desktop-helper) and [Virtual Desktop Preserve Taskbar Order
](https://windhawk.net/mods/virtual-desktop-taskbar-order) mods if you are into virtual desktops.

## Caveats and limitations

### Click/tap gesture evaluation

By default, after every click or tap on the taskbar, the mod waits for the Windows double-click time ([GetDoubleClickTime](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdoubleclicktime), usually 500 ms) before running any action.

This short delay is needed so the mod can correctly decide whether you did a:
- single click/tap
- double click/tap
- triple click/tap

This is what allows you, for example, to double-click the taskbar without triggering the single-click action first.

If you don't like this delay, you can turn on the **Eager trigger evaluation** option in the mod's settings.

With **Eager trigger evaluation** enabled:

The action runs immediately when a matching trigger is detected (no waiting).
However, double or triple clicks/taps can still trigger the single-click/tap action, as long as you haven't configured a separate double or triple click/tap action for that same trigger.
In other words, this option is a trade-off:

- Off – slight delay, but more accurate recognition of single vs. double vs. triple gestures
- On – no delay, but less precise gesture detection

### Right-click behavior
When you configure any right-click trigger (single, double, or triple), the mod needs to temporarily delay the taskbar's context menu to detect your intended action.

#### Here's how it works:

- When you right-click the taskbar, the mod checks if you're using the keyboard modifier associated with your configured trigger
- If the keyboard modifier matches your trigger setup, the context menu is blocked and your custom action runs instead
- If the keyboard modifier doesn't match (or you don't complete the trigger), the context menu appears normally

#### What this means for you:
If you set up a right-click trigger without keyboard modifiers (for example, a right double-click), you'll notice a brief delay before the context menu appears after a single right-click. This happens because the mod waits to see if you're going to complete a double or triple click. The delay is short but noticeable—it's the trade-off for having custom right-click actions.

Tip: To avoid this delay, consider using keyboard modifiers with your right-click triggers (like Ctrl + right double-click). This way, the mod can instantly show the context menu when you right-click without holding the modifier key.

## Supported Windows versions
- Windows 10 22H2 (prior versions are not tested, but should work as well)
- Windows 11 24H2 - latest major release (prior versions are not tested, but should work as well)

I will not be supporting Insider preview or other minor versions of Windows. However, feel free to [report any issues](https://github.com/m1lhaus/windhawk-mods/issues) related to those versions. I'll appreciate the heads-up in advance.

⚠️ **Caution!** Avoid using the option "Get the latest updates as soon as they're available" in Windows Update. Microsoft releases symbols for new Windows versions with a delay. This can render Windhawk mods unusable until the symbols are released (usually a few days).

## Troubleshooting

### I am using Windows 10 taskbar on Windows 11

If you are using the old Windows 10 taskbar on Windows 11 (**ExplorerPatcher** or a similar tool), enable the corresponding option in the Settings menu. ExplorerPatcher should get detected automatically.

### I can't click on empty space when the taskbar gets full

If your taskbar becomes fully occupied by open windows and pinned icons, there is no empty space left to click on. To reserve minimal empty space on the taskbar (the `Reserve empty space` feature from 7+ Taskbar Tweaker), you can use the [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler) mod with [this example configuration](https://github.com/ramensoftware/windhawk-mods/issues/1089#issuecomment-2576243679).

### Task Switching (Ctrl+Alt+Tab) window always reopens on consecutive triggers

If you are using [Vertical Taskbar for Windows 11](https://windhawk.net/mods/taskbar-vertical) or [Taskbar on top for Windows 11](https://windhawk.net/mods/taskbar-on-top) mod, taskbar click detection might not work correctly when the Task Switching window is opened. See [here for more details](https://github.com/ramensoftware/windhawk-mods/issues/3007). As a workaround, you can install [Alt+Tab per monitor](https://windhawk.net/mods/alt-tab-per-monitor) that implicitly fixes the Task Switching window rect size so that the taskbar is not overlaid by it anymore.

### Selected app/window from Task Switching (Ctrl+Alt+Tab) window is not focused

If you've manually selected an app/window from the Task Switching dialog using the mouse, but the window is not focused after closing the dialog, try pressing Win+Tab once. I encountered a weird issue where the underlying mechanism used to identify what you clicked would work only once and then stop working. Opening the Win+Tab window once seems to fix the issue until the next logout/restart.

### I just installed/updated the mod, but no actions are triggered

The mod uses Windhawk's `ArrayOfNestedOptions` widget type that enables you to create multiple trigger+action configurations. However, if you have just installed or updated the mod, there are no configurations set up yet. It might happen that Windhawk spawns a default empty configuration with all keyboard modifiers selected and no mouse trigger or action selected. Users then overlook the modifiers and only set up the mouse trigger and action. The mod will never trigger any action since it is impossible to press all keyboard modifiers at once on most keyboards. If that's the case, open the mod's Settings and either remove the empty configuration or set up your desired trigger+action configuration (including keyboard modifiers). For more information, please see the gif animation on the mod's `Description` tab.

### I have set my trigger correctly, but the action is not executed

Please see the previous section about empty configuration. If that is not the issue, check the mod's log for any error messages or other clues:

1. Disable the mod
2. Go to the `Advanced` tab
3. Under `Debug logging`, select `Mod logs`
4. Click on the `Show log output` button
5. You can clear the console first using the buttons in the upper right corner
6. Re-enable the mod and try to trigger your action again
7. Check the log for any error messages or other clues

Make sure the action trigger parsed from settings corresponds to your expectation. Make sure the mod reports a taskbar version that corresponds to your Windows taskbar. If you are using ExplorerPatcher with the Windows 10 taskbar, check that the mod is using Windows 10 taskbar mode. If you are using the Windows 11 taskbar with ExplorerPatcher, make sure the mod is using Windows 11 taskbar mode.

If you can't find anything useful, try enabling `DEBUG` logging:

1. Disable the mod
2. Fork the mod and click on the `Edit` button
3. Find `// #define ENABLE_LOG_DEBUG` and remove the leading `// ` characters to uncomment the line
4. In the toolbar on the left, enable logging and click on `Compile Mod`
5. Enable your forked mod
6. Now much more information will be logged
7. Try to trigger your action again and check the log for any error messages or other clues

### I have tried everything, but the mod is still not working as expected

Please open an [Issue on the GitHub page](https://github.com/m1lhaus/windhawk-mods/issues) describing your problem. Please always include the following information:

- Your Windows version including the exact build number (e.g., Windows 10 25H2 build 26200.7171) - use the `winver` command to get this information
- Whether you are using the classic taskbar on Windows 11 (ExplorerPatcher or a similar tool)
- Windhawk version
- Mod version
- Mod settings you are using - a screenshot of the mod's Settings tab, or ideally the entire settings JSON record from the `Advanced` tab
- Mod log output with `DEBUG` logging enabled (see the previous section for instructions on how to enable it)

## Hints and tips

### Opening an application on the currently active monitor

By default, Windows opens new application windows on the primary monitor. Even if monitor hint information is provided, many applications ignore it and still open on the primary monitor. If you want to open an application on the currently active monitor more reliably, you can use [Microsoft PowerToys](https://github.com/microsoft/PowerToys?tab=readme-ov-file#-installation) with the [FancyZones](https://learn.microsoft.com/en-us/windows/powertoys/fancyzones) feature enabled. FancyZones will remember the last active monitor and open new windows there. For more information, see GitHub [issue #52](https://github.com/m1lhaus/windhawk-mods/issues/52#issuecomment-3693251071).

### Opening Start menu on currently active monitor

The mod tries to find the Start button on the taskbar you clicked on to open the Start menu. However, if you used e.g., [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler) mod to hide the button, the mod will fall back and send a `Win` key press to open the Start menu. By default, Windows opens the Start menu on the primary monitor. If you want to open the Start menu on the currently active monitor, you can use the [Start menu open location](https://windhawk.net/mods/start-menu-open-location) mod.

### Volume control with mouse wheel over taskbar

If you wish to extend media playback control further, you can use the [Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control) mod. It lets you control the system volume by scrolling the mouse wheel over the taskbar.

### Opening Ctrl+Alt+Tab on the currently active monitor

By default, Windows opens Task Switching dialog on the primary monitor. If you want it to open on the currently active monitor, consider using [Alt+Tab per monitor](https://windhawk.net/mods/alt-tab-per-monitor) mod.

### Fine grade control over virtual desktops

The "Switch virtual desktop" feature is based on [u2x1](https://github.com/u2x1)'s  [Virtual Desktop Helper](https://windhawk.net/mods/virtual-desktop-helper) mod. If you want more fine-grained control over virtual desktops (e.g., switch to a specific desktop number, move the current window to another desktop, etc.), consider installing that mod and sending custom keypresses to it using this mod. Also checkout [Virtual Desktop Preserve Taskbar Order
](https://windhawk.net/mods/virtual-desktop-taskbar-order) mod.

## Suggestions and new features

If you have a request for new functions, suggestions, or you are experiencing some issues, please post an [Issue on the GitHub page](https://github.com/m1lhaus/windhawk-mods/issues). Please be as specific as possible and provide as much information as you can. Please consider using an AI chatbot if you are struggling to put everything together in English.

## Contact

You can contact me via Windhawk's [Discord channel](https://discord.com/servers/windhawk-923944342991818753) (@m1lhaus) or [GitHub page](https://github.com/m1lhaus/windhawk-mods/issues).

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TriggerActionOptions:
  - - KeyboardTriggers: [none, lctrl, lshift, lalt, win, rctrl, rshift, ralt]
      $name: Keyboard
      $description: Select keyboard key press modifiers. If None is selected or added, the modifier is ignored.
      $options:
      - none: None
      - lctrl: Left Ctrl
      - lshift: Left Shift
      - lalt: Left Alt
      - win: Win
      - rctrl: Right Ctrl
      - rshift: Right Shift
      - ralt: Right Alt
    - MouseTrigger: none
      $name: Mouse
      $description: Select the mouse click trigger. If None is selected, this trigger is ignored.
      $options:
      - none: None
      - left: Mouse left button click
      - leftDouble: Mouse left button double-click
      - leftTriple: Mouse left button triple-click
      - middle: Mouse middle button click
      - middleDouble: Mouse middle button double-click
      - middleTriple: Mouse middle button triple-click
      - right: Mouse right button click
      - rightDouble: Mouse right button double-click
      - rightTriple: Mouse right button triple-click
      - mouse4: Mouse side button 1 click
      - mouse4Double: Mouse side button 1 double-click
      - mouse4Triple: Mouse side button 1 triple-click
      - mouse5: Mouse side button 2 click
      - mouse5Double: Mouse side button 2 double-click
      - mouse5Triple: Mouse side button 2 triple-click
      - tapSingle: Touchscreen single tap
      - tapDouble: Touchscreen double tap
      - tapTriple: Touchscreen triple tap
    - TaskbarType: all
      $name: Taskbar
      $description: Select the taskbar for which the mouse click trigger should be active.
      $options:
      - all: All taskbars
      - primary: Primary taskbar only
      - secondary: Secondary taskbars only
    - Action: ACTION_NOTHING
      $name: Action
      $description: Action to invoke on trigger.
      $options:
      - ACTION_NOTHING: Nothing (default)
      - ACTION_COMBINE_TASKBAR_BUTTONS: Combine Taskbar buttons
      - ACTION_ALT_TAB: Ctrl+Alt+Tab
      - ACTION_HIDE_ICONS: Hide desktop icons
      - ACTION_MEDIA_NEXT: Media Next Track
      - ACTION_MEDIA_PLAY_PAUSE: Media Play/Pause
      - ACTION_MEDIA_PREV: Media Previous Track
      - ACTION_MUTE: Mute system volume
      - ACTION_OPEN_START_MENU: Open Start menu
      - ACTION_SHOW_DESKTOP: Show desktop
      - ACTION_SWITCH_VIRTUAL_DESKTOP: Switch virtual desktop
      - ACTION_START_PROCESS: Open application, path or URL
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_TASKBAR_AUTOHIDE: Taskbar auto-hide
      - ACTION_TOGGLE_TASKBAR_ALIGNMENT: Toggle Taskbar alignment
      - ACTION_SEND_KEYPRESS: Virtual key press
      - ACTION_WIN_TAB: Win+Tab
    - AdditionalArgs: arg1;arg2
      $name: Additional Args
      $description: Additional arguments for the selected action, separated by semicolons. See the mod's Details tab for more information about the supported arguments for each action.
  $name: Taskbar empty space actions
  $description: "Using the Keyboard and Mouse combo boxes, select a trigger for a specific action. For example, the combination 'Left Ctrl + Double-click + Task Manager' will open the Windows Task Manager when the user double-clicks empty space on the taskbar while holding the Left Ctrl key. More actions can be set up with the Add new item button."
- oldTaskbarOnWin11: false
  $name: Use the old Windows 10 taskbar on Windows 11
  $description: >-
    Enable this option to if you are using the old Windows 10 taskbar on Windows 11 (ExplorerPatcher or a similar tool).
- eagerTriggerEvaluation: false
  $name: Eager trigger evaluation
  $description: >-
    Run actions immediately when a matching click or tap is detected, instead of
    waiting a short time to see if it becomes a double or triple click/tap.
    This makes the mod feel more responsive, but limits how precisely different
    gestures can be distinguished. See the "Caveats and limitations" section on
    the mod description page for more details.
*/
// ==/WindhawkModSettings==

#pragma region header_file

#include <initguid.h>
#include <commctrl.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objectarray.h>
#include <windef.h>
#include <windhawk_api.h>
#include <winerror.h>
#include <winuser.h>
#include <windowsx.h>
#include <windhawk_utils.h>
#include <UIAnimation.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>
#include <psapi.h>
#include <shlobj.h>
#include <Knownfolders.h>

#include <string>
#include <unordered_set>
#include <vector>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <tuple>

// get the naming of smart ptrs somehow consistent since winapi naming is wild
using winrt::com_ptr;
using bstr_ptr = _bstr_t;

#pragma region macros

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

// =====================================================================

#define ENABLE_LOG_INFO // info messages will be enabled
// #define ENABLE_LOG_DEBUG // verbose debug messages will be enabled
// #define ENABLE_LOG_TRACE // method enter/leave messages will be enabled
// #define ENABLE_FILE_LOGGER // enable file logger (log file is written to desktop)

// =====================================================================

#ifdef ENABLE_FILE_LOGGER
#include <fstream>
// file logger works as simple Tee to log to both console and file
class FileLogger
{
public:
    FileLogger()
    {
        const std::wstring filename = L"empty_space_clicks_log.txt";

        PWSTR desktopPath = nullptr; // because Microsoft is moving Desktop to Onedrive
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &desktopPath)))
        {
            m_filepath = std::wstring(desktopPath) + L"\\empty_space_clicks_log.txt";
            CoTaskMemFree(desktopPath);
        }
        else
        {
            m_filepath = filename;
        }
        m_file.open(m_filepath, std::ios_base::out | std::ios_base::app);
        if (m_file.is_open())
        {
            std::time_t t = std::time(nullptr);
            std::tm *tm = std::localtime(&t);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            m_file << "===========================" << std::endl;
            m_file << "Log started at " << buffer << std::endl;
        }
    }

    ~FileLogger()
    {
        m_file.close();
    }

    // print status of the logger to actually know if it opened the file correctly
    void printStatus()
    {
        if (m_file.is_open())
        {
            Wh_Log(L"INFO: FileLogger: Log file opened at path: %s", m_filepath.c_str());
        }
        else
        {
            Wh_Log(L"ERROR: FileLogger: Log file is not open! File path: %s", m_filepath.c_str());
        }
    }

    // called by log macros, write formatted string to the log file
    void write(const wchar_t *format, ...)
    {
        if (m_file.is_open())
        {
            const size_t max_size = 255U;
            std::unique_ptr<wchar_t[]> buf(new wchar_t[max_size]);

            va_list args;
            va_start(args, format);
            const size_t size = std::vswprintf(buf.get(), max_size, format, args) + 1; // +1 for '\0'
            va_end(args);

            auto str = std::wstring(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
            m_file << str << '\n';

            m_file.flush();
        }
    }

private:
    std::wofstream m_file;
    std::wstring m_filepath;
} g_fileLogger;

#define LOG(format, ...)                                       \
    do                                                         \
    {                                                          \
        Wh_Log(format __VA_OPT__(, ) __VA_ARGS__);             \
        g_fileLogger.write(format __VA_OPT__(, ) __VA_ARGS__); \
    } while (0)

#else
#define LOG(format, ...) Wh_Log(format __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifdef ENABLE_LOG_TRACE
// to make printf debugging easier, logging tracer records function entry its name and line number
class TraceLogger
{
public:
    TraceLogger(const int line, const std::string function)
    {
        m_function = std::wstring(function.begin(), function.end()); // function is just ascii line number
        m_line = line;
        LOG(L"TRACE: Entering %s at line %d", m_function.c_str(), line);
    }

    ~TraceLogger()
    {
        LOG(L"TRACE: Leaving %s (%d)", m_function.c_str(), m_line);
    }

private:
    std::wstring m_function;
    int m_line;
};
#endif

#define LOG_ERROR(format, ...) LOG(L"ERROR: " format __VA_OPT__(, ) __VA_ARGS__)
#ifdef ENABLE_LOG_INFO
#define LOG_INFO(format, ...) LOG(L"INFO: " format __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif
#ifdef ENABLE_LOG_DEBUG
#define LOG_DEBUG(format, ...) LOG(L"DEBUG: " format __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif
#ifdef ENABLE_LOG_TRACE
#define LOG_TRACE() TraceLogger traceLogger(__LINE__, __FUNCTION__)
#else
#define LOG_TRACE()
#endif

#pragma endregion // macros

enum WindowsVersion
{
    WIN_10 = 0,
    WIN_11,
    UNKNOWN
};
const wchar_t *WindowsVersionNames[] = {L"WIN_10", L"WIN_11", L"UNKNOWN"};

// Enum for key modifiers used to detect specific key states during input events.
enum KeyModifier
{
    KEY_MODIFIER_LCTRL = 0,
    KEY_MODIFIER_RCTRL,
    KEY_MODIFIER_LALT,
    KEY_MODIFIER_RALT,
    KEY_MODIFIER_LSHIFT,
    KEY_MODIFIER_RSHIFT,
    KEY_MODIFIER_LWIN,
    KEY_MODIFIER_INVALID
};

// Enum for taskbar buttons state used to determine how to combine (or not) taskbar buttons.
enum TaskBarButtonsState
{
    COMBINE_ALWAYS = 0,
    COMBINE_WHEN_FULL,
    COMBINE_NEVER,
    COMBINE_INVALID
};

// structure that wraps around what action should be done under which conditions
struct TriggerAction
{
    std::wstring mouseTriggerName;            // Mouse trigger parsed from settings - represents what kind of button click should be detected
    std::wstring taskbarTypeName;             // Taskbar type parsed from settings - represents which taskbar the trigger should be active on
    std::wstring actionName;                  // Name of the action parsed from settings
    uint32_t expectedKeyModifiersState;       // expected state (bitmask) of the key modifiers that should be checked
    std::function<void(HWND)> actionExecutor; // function that executes the action
};

#pragma region globals

static WindowsVersion g_taskbarVersion = UNKNOWN;
static WindowsVersion g_osVersion = UNKNOWN;
static WORD g_twinuiPCShellBuildNumber = 0;

static DWORD g_dwTaskbarThreadId;
static bool g_isWhInitialized = false;

static HWND g_hTaskbarWnd;          // Shell_TrayWnd window handle
static HWND g_hTaskbarInputSiteWnd; // Windows.UI.Input.InputSite.WindowClass window handle (Win11 taskbar)
static std::unordered_set<HWND> g_secondaryTaskbarWindows;
static std::unordered_set<HWND> g_secondaryTaskbarInputSiteWindows;

static std::unordered_set<HWND> g_subclassedTaskbarWindows; // set of subclassed taskbar windows to avoid double subclassing
static std::unordered_set<HWND> g_hookedInputSiteWindows;   // set of hooked InputSite windows to avoid double hooking
static std::unordered_set<void *> g_hookedInputSiteProcs;   // set of hooked InputSite window procs to avoid double hooking

static HWND g_hTaskSwitchingWnd = NULL;        // Alt+Tab window handle
static bool g_keepTaskSwitchingOpened = false; // flag to indicate whether mouse input should be suppressed
static BOOL g_isTaskSwitchingWindowSubclassed = FALSE;

static bool g_isExplorerPatcherDetected = false;

static bool g_isContextMenuSuppressed = false;
static DWORD g_contextMenuSuppressionTimestamp = 0;

static UINT_PTR gMouseClickTimer = (UINT_PTR)NULL;
static const DWORD g_injectedClickID = 0xEADBEAF1u; // magic number to identify synthesized clicks

static const UINT g_explorerPatcherContextMenuMsg = RegisterWindowMessageW(L"Windows11ContextMenu_{D17F1E1A-5919-4427-8F89-A1A8503CA3EB}");
static const UINT g_uninitCOMAPIMsg = RegisterWindowMessageW(L"Windhawk_UnInit_COMAPI_empty-space-clicks");

// Private API for window band (z-order band).
// https://blog.adeltax.com/window-z-order-in-windows-10/
using GetWindowBand_t = BOOL(WINAPI *)(HWND hWnd, PDWORD pdwBand);
GetWindowBand_t pGetWindowBand = nullptr;
constexpr DWORD ZBID_SYSTEM_TOOLS = 16;

#pragma endregion // declarations

struct WindhawkModSettings
{
    bool oldTaskbarOnWin11;
    bool eagerTriggerEvaluation;
    std::vector<TriggerAction> triggerActions;
};
static WindhawkModSettings g_settings;

// Wrapper around COM API initialization and usage to enable lazy init and safe resource management
class COMAPI
{
public:
    COMAPI() : m_isInitialized(false), m_isCOMInitialized(false), m_isUIAInitialized(false), m_isDEInitialized(false),
               m_pUIAutomation(nullptr), m_pDeviceEnumerator(nullptr) {}

    // Initializes COM interfaces for UIAutomation and Volume control
    bool Init()
    {
        if (!m_isCOMInitialized)
        {
            if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) // COM was most likely already initialized in the GUI thread, but just to be sure
            {
                m_isCOMInitialized = true;
                LOG_INFO(L"COM initialized");
            }
            else
            {
                m_isCOMInitialized = false;
                LOG_ERROR(L"COM initialization failed, ModInit failed");
            }
        }
        if (!m_isUIAInitialized && m_isCOMInitialized)
        {
            // init COM interface for UIAutomation
            if (FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                        m_pUIAutomation.put_void())) ||
                !m_pUIAutomation)
            {
                m_isUIAInitialized = false;
                LOG_ERROR(L"Failed to create UIAutomation COM instance, ModInit failed");
            }
            else
            {
                m_isUIAInitialized = true;
                LOG_INFO(L"UIAutomation COM initilized");
            }
        }
        if (!m_isDEInitialized && m_isCOMInitialized)
        {
            // init COM interface for Volume control
            const GUID XIID_IMMDeviceEnumerator = {0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
            const GUID XIID_MMDeviceEnumerator = {0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
            if (FAILED(CoCreateInstance(XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, XIID_IMMDeviceEnumerator,
                                        m_pDeviceEnumerator.put_void())) ||
                !m_pDeviceEnumerator)
            {
                m_isDEInitialized = false;
                LOG_ERROR(L"Failed to create DeviceEnumerator COM instance. Volume mute feature will not be available!");
            }
            else
            {
                m_isDEInitialized = true;
                LOG_INFO(L"DeviceEnumerator COM initilized");
            }
        }
        m_isInitialized = m_isCOMInitialized && m_isUIAInitialized; // m_isDEInitialized is not mandatory
        return m_isInitialized;
    }

    // Releases COM resources and uninitializes interfaces
    void Uninit()
    {
        if (m_isDEInitialized)
        {
            m_pDeviceEnumerator = com_ptr<IMMDeviceEnumerator>(nullptr); // force underlying ptr to get released
            m_isDEInitialized = false;
            LOG(L"DeviceEnumerator COM de-initialized");
        }
        if (m_isUIAInitialized)
        {
            m_pUIAutomation = com_ptr<IUIAutomation>(nullptr); // force underlying ptr to get released
            m_isUIAInitialized = false;
            LOG(L"UIAutomation COM de-initialized");
        }
        if (m_isCOMInitialized)
        {
            CoUninitialize();
            LOG(L"COM de-initialized");
            m_isCOMInitialized = false;
        }
        m_isInitialized = false;
    }

    bool IsInitialized() { return m_isInitialized; }

    const com_ptr<IUIAutomation> GetUIAutomation()
    {
        return m_pUIAutomation;
    }

    const com_ptr<IMMDeviceEnumerator> GetDeviceEnumerator()
    {
        return m_pDeviceEnumerator;
    }

protected:
    bool m_isInitialized;
    bool m_isCOMInitialized;
    bool m_isUIAInitialized;
    bool m_isDEInitialized;

    com_ptr<IUIAutomation> m_pUIAutomation;
    com_ptr<IMMDeviceEnumerator> m_pDeviceEnumerator;
};
static COMAPI g_comAPI;

// Tracks foreground window changes to support focusPreviousWindow feature
class WindowFocusTracker
{
private:
    HWINEVENTHOOK m_hEventHook;
    HANDLE m_hThread;
    DWORD m_dwThreadId;
    volatile bool m_bRunning;

    // Captures foreground window changes and stores non-taskbar windows
    static void CALLBACK WinEventProc(
        HWINEVENTHOOK hWinEventHook,
        DWORD event,
        HWND hwnd,
        LONG idObject,
        LONG idChild,
        DWORD dwEventThread,
        DWORD dwmsEventTime)
    {
        if (event == EVENT_SYSTEM_FOREGROUND && idObject == OBJID_WINDOW)
        {
            if (hwnd && IsWindow(hwnd) && (hwnd != g_hwndLastActive))
            {
                // Check if it's not the taskbar
                WCHAR className[256];
                GetClassName(hwnd, className, 256);
                if ((wcscmp(className, L"Shell_TrayWnd") != 0) &&
                    (wcscmp(className, L"Shell_SecondaryTrayWnd") != 0) &&
                    (wcscmp(className, L"Windows.UI.Input.InputSite.WindowClass") != 0) &&
                    (wcscmp(className, L"Taskbar.TaskbarFrameAutomationPeer") != 0) &&
                    (wcscmp(className, L"Windows.UI.Core.CoreWindow") != 0) &&
                    (wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") != 0))
                {
#ifdef ENABLE_LOG_DEBUG
                    // Retrieve and log the full window title and process id for debugging
                    DWORD dwProcessId = 0;
                    GetWindowThreadProcessId(hwnd, &dwProcessId);

                    // Get window text length (in characters) and read the title
                    int textLen = GetWindowTextLengthW(hwnd);
                    std::wstring windowTitle;
                    if (textLen > 0)
                    {
                        windowTitle.resize(textLen + 1);
                        if (GetWindowTextW(hwnd, &windowTitle[0], textLen + 1) > 0)
                        {
                            windowTitle.resize(std::wcslen(windowTitle.c_str()));
                        }
                        else
                        {
                            windowTitle.clear();
                        }
                    }

                    LOG_DEBUG(L"Foreground window changed: CurrentThread: %u HWND=0x%08X PID=%u ClassName=%s Title=%s",
                              GetCurrentThreadId(), (DWORD)(ULONG_PTR)hwnd, dwProcessId, className, windowTitle.c_str());
#endif
                    g_hwndLastActive = hwnd;
                }
            }
        }
    }

    // Window focus tracker thread procedure that processes window events
    static DWORD WINAPI ThreadProc(LPVOID lpParam)
    {
        WindowFocusTracker *pThis = (WindowFocusTracker *)lpParam;

        pThis->m_hEventHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            NULL, WinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT);

        if (!pThis->m_hEventHook)
        {
            LOG_ERROR(L"Registering of WindowFocusTracker::WinEventProc hook failed");
            return 1;
        }

        LOG_INFO(L"WindowFocusTracker thread started with hook %p", pThis->m_hEventHook);

        MSG msg;
        while (pThis->m_bRunning && GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (pThis->m_hEventHook)
        {
            UnhookWinEvent(pThis->m_hEventHook);
            pThis->m_hEventHook = NULL;
        }

        LOG_INFO(L"WindowFocusTracker thread terminated");
        return 0;
    }

public:
    static HWND g_hwndLastActive;

    WindowFocusTracker() : m_hEventHook(NULL), m_hThread(NULL), m_dwThreadId(0), m_bRunning(false) {}

    // Starts the focus tracking thread
    void Start()
    {
        if (m_hThread)
        {
            LOG_ERROR(L"WindowFocusTracker already started");
            return;
        }

        m_bRunning = true;
        m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &m_dwThreadId);
        if (!m_hThread)
        {
            LOG_ERROR(L"Failed to create WindowFocusTracker thread");
            m_bRunning = false;
        }
        else
        {
            LOG_INFO(L"WindowFocusTracker thread created with ID %u ", m_dwThreadId);
        }
    }

    // Stops the focus tracking thread and cleans up
    void Stop()
    {
        if (m_hThread)
        {
            LOG_INFO(L"Stopping WindowFocusTracker...");
            m_bRunning = false;

            // Post a quit message to the thread's message queue
            PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);

            // Wait for thread to finish
            WaitForSingleObject(m_hThread, 500);
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }
    }

    bool IsRunning()
    {
        return m_hThread && m_bRunning;
    }

    HWND GetLastActiveWindow()
    {
        return g_hwndLastActive;
    }

    ~WindowFocusTracker()
    {
        Stop(); // should be stopped during mod uninit
    }
};
HWND WindowFocusTracker::g_hwndLastActive = NULL;
static WindowFocusTracker g_windowFocusTracker;

void SetBit(uint32_t &value, uint32_t bit);
bool GetBit(const uint32_t &value, uint32_t bit);

// Stores mouse click information including position, button, timestamp and empty space detection
struct MouseClick
{
    enum class Button
    {
        LEFT = 0,
        RIGHT,
        MIDDLE,
        MOUSE4, // side button 1
        MOUSE5, // side button 2
        INVALID
    };

    enum class Type
    {
        MOUSE = 0,
        TOUCH,
        INVALID
    };

    MouseClick() : type(Type::INVALID), button(Button::INVALID), position{0, 0}, timestamp(0), onEmptySpace(false), hWnd(NULL), keyModifiersState(0)
    {
    }

    // Constructs MouseClick from window message parameters and detects if click is on empty taskbar space
    MouseClick(const POINT &ptrPos, Type ptrType, Button btn, HWND hWnd) : type(ptrType), button(btn), position(ptrPos), timestamp(0), onEmptySpace(false), hWnd(hWnd)
    {
        LOG_TRACE();

        timestamp = GetTickCount();
        auto pUIAutomation = g_comAPI.GetUIAutomation();
        if (!pUIAutomation)
        {
            LOG_ERROR(L"UIAutomation COM interface is not initialized, cannot determine if click was on empty space");
            return; // other members are initialized so it's safe to return
        }

        // Note: The reason why UIAutomation interface is used is that it reliably returns a className of the element clicked.
        // If standard Windows API is used, the className returned is always Shell_TrayWnd which is a parrent window wrapping the taskbar.
        // From that we can't really tell reliably whether user clicked on the taskbar empty space or on some UI element on that taskbar, like
        // opened window, icon, start menu, etc.
        com_ptr<IUIAutomationElement> pWindowElement = NULL;
        HRESULT hr = pUIAutomation->ElementFromPoint(position, pWindowElement.put());
        if (FAILED(hr) || !pWindowElement)
        {
            LOG_ERROR(L"Failed to retrieve UI element from mouse click at (%ld, %ld), HRESULT: 0x%08X", position.x, position.y, hr);
            return; // without element info we cannot determine its type, other members are initialized so it's safe to return
        }

        bstr_ptr className;
        if (FAILED(pWindowElement->get_CurrentClassName(className.GetAddress())) || !className)
        {
            LOG_ERROR(L"Failed to retrieve the Name of the UI element clicked.");
            return; // we can't determine the type of the element, other members are initialized so it's safe to return
        }
        onEmptySpace = (wcscmp(className.GetBSTR(), L"Shell_TrayWnd") == 0) ||                        // Windows 10 primary taskbar
                       (wcscmp(className.GetBSTR(), L"Shell_SecondaryTrayWnd") == 0) ||               // Windows 10 secondary taskbar
                       (wcscmp(className.GetBSTR(), L"Taskbar.TaskbarFrameAutomationPeer") == 0) ||   // Windows 11 taskbar
                       (wcscmp(className.GetBSTR(), L"Windows.UI.Input.InputSite.WindowClass") == 0); // Windows 11 21H2 taskbar

        keyModifiersState = GetKeyModifiersState();

#ifdef ENABLE_LOG_DEBUG
        std::wstring keyModifiersStateBinRepr = L"0b";
        for (int i = 7; i >= 0; --i)
        {
            keyModifiersStateBinRepr += GetBit(keyModifiersState, i) ? L'1' : L'0';
        }
        LOG_DEBUG(L"Taskbar clicked clicked at x=%ld, y=%ld, type=%d, btn=%d, element=%s, isEmptySpace=%d, keyModifiersState=%s",
                  position.x, position.y, static_cast<int>(type), static_cast<int>(button), className.GetBSTR(), onEmptySpace, keyModifiersStateBinRepr.c_str());
#endif
    }

    // Extracts mouse position from Win11 pointer message
    static POINT GetMouseClickPositionWin11(LPARAM lParam)
    {
        LOG_TRACE();

        POINT pointerLocation{};
        pointerLocation.x = GET_X_LPARAM(lParam);
        pointerLocation.y = GET_Y_LPARAM(lParam);
        return pointerLocation;
    }

    // Extracts mouse position from message parameters, handling Win10/Win11 differences
    static POINT GetMouseClickPositionWin10()
    {
        LOG_TRACE();

        // message carries mouse position relative to the client window so use GetCursorPos() instead
        POINT pointerLocation{};
        if (!GetCursorPos(&pointerLocation))
        {
            LOG_ERROR(L"Failed to get mouse position");
        }
        return pointerLocation;
    }

    // based on GetMessageExtraInfo() value, determine whether the input came from mouse or touch/pen for legacy mouse messages
    static MouseClick::Type GetPointerTypeWin10(LPARAM lParam)
    {
        LOG_TRACE();

        MouseClick::Type type = MouseClick::Type::INVALID;

        // legacy messages have information about pointer id stored in extra info (lparam)
        // https://learn.microsoft.com/en-us/windows/win32/tablet/system-events-and-mouse-messages?redirectedfrom=MSDN#distinguishing-pen-input-from-mouse-and-touch
        const auto MI_WP_SIGNATURE = 0xFF515700U;
        const auto SIGNATURE_MASK = 0xFFFFFF00U;
        if ((lParam & SIGNATURE_MASK) == MI_WP_SIGNATURE) // IsPenEvent ?
        {
            type = MouseClick::Type::TOUCH;
        }
        else
        {
            type = MouseClick::Type::MOUSE;
        }
        return type;
    }

    // based on POINTER message WPARAM, determine whether the input came from mouse or touch/pen for new pointer messages
    static MouseClick::Type GetPointerTypeWin11(WPARAM wParam)
    {
        LOG_TRACE();

        MouseClick::Type type = MouseClick::Type::INVALID;

        // Retrieve common pointer information to find out source of the click
        POINTER_INFO pointerInfo;
        UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
        if (pointerId == 0) // from synthesized mouse event
        {
            type = MouseClick::Type::MOUSE;
        }
        else if (pointerId == 0xffff) // from synthesized mouse event
        {
            type = MouseClick::Type::TOUCH;
        }
        else if (GetPointerInfo(pointerId, &pointerInfo))
        {
            if ((pointerInfo.pointerType == PT_TOUCH) || (pointerInfo.pointerType == PT_PEN) || (pointerInfo.pointerType == PT_POINTER))
            {
                type = MouseClick::Type::TOUCH;
            }
            else
            {
                type = MouseClick::Type::MOUSE;
            }
        }
        else
        {
            LOG_ERROR(L"Failed to retrieve pointer info for pointer id %d", pointerId);
        }
        return type;
    }

    // based on POINTER message WPARAM, determine which mouse button was clicked for new pointer messages
    static MouseClick::Button GetMouseButtonWin11(WPARAM wParam)
    {
        LOG_TRACE();

        MouseClick::Button button = MouseClick::Button::INVALID;
        if (IS_POINTER_FIRSTBUTTON_WPARAM(wParam))
        {
            button = MouseClick::Button::LEFT;
        }
        else if (IS_POINTER_SECONDBUTTON_WPARAM(wParam))
        {
            button = MouseClick::Button::RIGHT;
        }
        else if (IS_POINTER_THIRDBUTTON_WPARAM(wParam))
        {
            button = MouseClick::Button::MIDDLE;
        }
        else if (IS_POINTER_FOURTHBUTTON_WPARAM(wParam))
        {
            button = MouseClick::Button::MOUSE4;
        }
        else if (IS_POINTER_FIFTHBUTTON_WPARAM(wParam))
        {
            button = MouseClick::Button::MOUSE5;
        }
        return button;
    }

    // based on legacy mouse message and WPARAM, determine which mouse button was clicked for legacy mouse messages
    static MouseClick::Button GetMouseButtonWin10(UINT uMsg, WPARAM wParam)
    {
        LOG_TRACE();

        // button up messages seems really unreliable, so only process down and dblclk messages
        MouseClick::Button button = MouseClick::Button::INVALID;
        if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_NCLBUTTONDOWN) || (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_NCLBUTTONDBLCLK))
        {
            button = MouseClick::Button::LEFT;
        }
        else if ((uMsg == WM_RBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN) || (uMsg == WM_RBUTTONDBLCLK || uMsg == WM_NCRBUTTONDBLCLK))
        {
            button = MouseClick::Button::RIGHT;
        }
        else if ((uMsg == WM_MBUTTONDOWN || uMsg == WM_NCMBUTTONDOWN) || (uMsg == WM_MBUTTONDBLCLK || uMsg == WM_NCMBUTTONDBLCLK))
        {
            button = MouseClick::Button::MIDDLE;
        }
        else if ((uMsg == WM_XBUTTONDOWN || uMsg == WM_NCXBUTTONDOWN) || (uMsg == WM_XBUTTONDBLCLK || uMsg == WM_NCXBUTTONDBLCLK))
        {
            button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseClick::Button::MOUSE4 : MouseClick::Button::MOUSE5;
        }
        return button;
    }

    // Returns bitmask of currently pressed modifier keys (Ctrl, Alt, Shift, Win)
    static uint32_t GetKeyModifiersState()
    {
        LOG_TRACE();

        uint32_t currentKeyModifiersState = 0U;
        const auto LogKeyIfPressed = [&currentKeyModifiersState](const int &vkCode, const KeyModifier &keyModifier)
        {
            // using GetKeyboardState is not reliable for synthesized clicks, so using GetAsyncKeyState instead
            if ((GetAsyncKeyState(vkCode) & 0x8000) != 0)
                SetBit(currentKeyModifiersState, keyModifier);
        };

        LogKeyIfPressed(VK_LCONTROL, KEY_MODIFIER_LCTRL);
        LogKeyIfPressed(VK_LSHIFT, KEY_MODIFIER_LSHIFT);
        LogKeyIfPressed(VK_LMENU, KEY_MODIFIER_LALT);
        LogKeyIfPressed(VK_LWIN, KEY_MODIFIER_LWIN);
        LogKeyIfPressed(VK_RCONTROL, KEY_MODIFIER_RCTRL);
        LogKeyIfPressed(VK_RSHIFT, KEY_MODIFIER_RSHIFT);
        LogKeyIfPressed(VK_RMENU, KEY_MODIFIER_RALT);

        return currentKeyModifiersState;
    }

    Type type;
    Button button;
    POINT position;
    DWORD timestamp;
    bool onEmptySpace;
    HWND hWnd;
    uint32_t keyModifiersState;
};

// Ring buffer storing last 4 clicks with python-style negative indexing (-1 is newest)
class MouseClickQueue
{
public:
    MouseClickQueue() : currentIndex(0) { clear(); }

    void push_back(const MouseClick &click)
    {
        clicks[currentIndex] = click;
        currentIndex = (currentIndex + 1) % size();
    }

    const MouseClick &operator[](int i) const
    {
        int idx = 0;
        if (i > 0)
        {
            if (i < size())
            {
                idx = (currentIndex + i) % size(); // oldest item always first
            }
            else
            {
                LOG_ERROR(L"Index out of bounds");
            }
        }
        else
        {
            if (i >= -size())
            {
                idx = (currentIndex + i + size()) % size(); // -1 is the newest (last) item
            }
            else
            {
                LOG_ERROR(L"Index out of bounds");
            }
        }
        return clicks[idx];
    }

    void clear()
    {
        currentIndex = 0;
        for (auto &click : clicks)
        {
            click = MouseClick();
        }
    }

    int size() const
    {
        return MAX_CLICKS;
    }

private:
    static const int MAX_CLICKS = 4; // 4 to be able to detect continuous clicks beyond triple click

    MouseClick clicks[MAX_CLICKS];
    int currentIndex;
};
static MouseClickQueue g_mouseClickQueue;

// =====================================================================

#pragma region forward_declarations

const wchar_t *GetMessageName(UINT uMsg);
std::wstring GetClassNameString(HWND hWnd);
std::wstring GetWindowTextString(HWND hWnd);
HWND FindTaskSwitchingWindow();
bool IsPrimaryTaskbarWindow(HWND hWnd);
bool IsSecondaryTaskbarWindow(HWND hWnd);
bool IsTaskbarWindow(HWND hWnd);
std::unordered_set<HWND> KeepOnlyValidWindows(const std::unordered_set<HWND> &windows);
bool UserClickedOnTaskViewItemOnTaskSwitchingWindow(const POINT &pt);
bool HandleTaskSwitchingWindowClick(const POINT &pt);

KeyModifier GetKeyModifierFromName(const std::wstring &keyName);
bool IsTaskbarWindow(HWND hWnd);
bool ShallSuppressContextMenu(const MouseClick &lastClick);
bool IsSingleClick(const MouseClick::Button button);
bool IsDoubleClick(const MouseClick::Button button, const MouseClick &previousClick, const MouseClick &currentClick);
bool IsDoubleClick(const MouseClick::Button button);
bool IsTripleClick(const MouseClick::Button button);
bool IsMultiClick(const MouseClick::Button button);
bool IsSingleTap();
bool IsDoubleTap(const MouseClick &previousClick, const MouseClick &currentClick);
bool IsDoubleTap();
bool IsTripleTap();
bool IsMultiTap();
bool ExecuteTaskbarAction(const std::wstring &mouseTriggerName, const uint32_t numClicks);
void SynthesizeTaskbarRightClick(const POINT &ptScreen);
void CALLBACK ProcessDelayedMouseClick(HWND, UINT, UINT_PTR, DWORD);
std::function<bool()> GetTaskbarActionExecutor(const bool checkForHigherOrderClicks);
bool IsTriggerDefined(const std::wstring &mouseTriggerName, const int numClicks);
std::wstring GetActionName(const MouseClick::Type clickType, const uint32_t numClicks, const MouseClick::Button button = MouseClick::Button::INVALID);
bool OnMouseClick(const MouseClick &click);
bool GetTaskbarAutohideState();
void SetTaskbarAutohide(bool enabled);
void ToggleTaskbarAutohide();
void ShowDesktop();
void SwitchVirtualDesktop(bool reverse);
void SendKeypress(const std::vector<int> &keys, const bool focusPreviousWindow = false);
void SendCtrlAltTabKeypress(const bool reverse);
void CloseCtrlAltTabDialog();
void SendWinTabKeypress();
void MediaPlayPause();
void MediaStop();
void MediaNext();
void MediaPrev();
bool ClickStartMenu();
void OpenStartMenu();
void OpenTaskManager(HWND taskbarhWnd);
BOOL IsAudioMuted(com_ptr<IMMDeviceEnumerator> pDeviceEnumerator);
void ToggleVolMuted();
void HideIcons();
void CombineTaskbarButtons(const TaskBarButtonsState primaryTaskBarButtonsState1, const TaskBarButtonsState primaryTaskBarButtonsState2,
                           const TaskBarButtonsState secondaryTaskBarButtonsState1, const TaskBarButtonsState secondaryTaskBarButtonsState2);
DWORD GetCombineTaskbarButtons(const wchar_t *optionName);
bool SetCombineTaskbarButtons(const wchar_t *optionName, unsigned int option);
DWORD GetTaskbarAlignment();
bool SetTaskbarAlignment(DWORD alignment);
void ToggleTaskbarAlignment();
void StartProcess(std::wstring command);
std::tuple<TaskBarButtonsState, TaskBarButtonsState, TaskBarButtonsState, TaskBarButtonsState> ParseTaskBarButtonsState(const std::wstring &args);

#pragma endregion // forward_declarations

// =====================================================================

#pragma endregion // header_file

// =====================================================================

#pragma region hooks_and_win32_methods

// proc handler for older Windows (nonXAML taskbar) versions and ExplorerPatcher
LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData);

// proc handler for Alt+Tab (Task Switching) window to suppress mouse input when needed
LRESULT CALLBACK TaskSwitchingWindowSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData);

// proc handler for newer Windows versions (Windows 11 21H2 and newer) and ExplorerPatcher (Win11 menu)
WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_empty-space-clicks");

BOOL SubclassTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();

    return WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc, NULL);
}

void UnsubclassTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();

    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void SubclassTaskSwitchingWindow()
{
    LOG_TRACE();

    g_hTaskSwitchingWnd = FindTaskSwitchingWindow();
    if (g_hTaskSwitchingWnd)
    {
        LOG_DEBUG(L"Found Task Switching window: 0x%08X", (DWORD)(ULONG_PTR)g_hTaskSwitchingWnd);
        g_isTaskSwitchingWindowSubclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(g_hTaskSwitchingWnd, TaskSwitchingWindowSubclassProc, NULL);
        if (!g_isTaskSwitchingWindowSubclassed)
        {
            LOG_ERROR(L"Failed to subclass Task Switching window 0x%08X", (DWORD)(ULONG_PTR)g_hTaskSwitchingWnd);
        }
        else
        {
            LOG_INFO(L"Subclassed Task Switching window 0x%08X successfully", (DWORD)(ULONG_PTR)g_hTaskSwitchingWnd);
        }
    }
}

void UnsubclassTaskSwitchingWindow()
{
    LOG_TRACE();

    WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hTaskSwitchingWnd, TaskSwitchingWindowSubclassProc);
    g_hTaskSwitchingWnd = NULL;
    g_isTaskSwitchingWindowSubclassed = false;
}

// Hooks InputSite window proc for Win11 taskbar pointer events
bool HandleIdentifiedInputSiteWindow(HWND hWnd)
{
    LOG_TRACE();

    if (!g_dwTaskbarThreadId || GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId)
    {
        return false;
    }

    // store hwnd of InputSite window so we can later send messages to its wndproc
    HWND hParentWnd = GetAncestor(hWnd, GA_ROOT);
    if (IsPrimaryTaskbarWindow(hParentWnd))
    {
        g_hTaskbarInputSiteWnd = hWnd;
    }
    else if (IsSecondaryTaskbarWindow(hParentWnd))
    {
        g_secondaryTaskbarInputSiteWindows = KeepOnlyValidWindows(g_secondaryTaskbarInputSiteWindows);
        g_secondaryTaskbarInputSiteWindows.insert(hWnd);
    }
    else
    {
        LOG_ERROR(L"InputSite window 0x%08X parent is not a known taskbar window", (DWORD)(ULONG_PTR)hWnd);
    }

    // At first, I tried to subclass the window instead of hooking its wndproc,
    // but the inputsite.dll code checks that the value wasn't changed, and
    // crashes otherwise.
    void *wndProc = (void *)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    if (!g_hookedInputSiteProcs.contains(wndProc))
    {
        if (!Wh_SetFunctionHook(wndProc, (void *)InputSiteWindowProc_Hook, (void **)&InputSiteWindowProc_Original))
        {
            LOG_ERROR(L"Failed to hook InputSite wndproc %p", wndProc);
            return false;
        }

        if (g_isWhInitialized)
        {
            Wh_ApplyHookOperations(); // from docs: Can't be called before Wh_ModInit returns or after Wh_ModBeforeUninit returns
        }
        g_hookedInputSiteProcs.insert(wndProc);

        LOG_INFO(L"Hooked InputSite wndproc %p", wndProc);
    }
    else
    {
        LOG_DEBUG(L"InputSite wndproc %p already hooked, skipping", wndProc);
    }
    return true;
}

// Subclasses main taskbar and secondary taskbars, hooks InputSite for Win11
void HandleIdentifiedTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();

    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);

    if (!g_subclassedTaskbarWindows.contains(hWnd))
    {
        if (SubclassTaskbarWindow(hWnd))
        {
            g_subclassedTaskbarWindows.insert(hWnd);
            LOG_INFO(L"Main taskbar window 0x%08X subclassed successfully", (DWORD)(ULONG_PTR)hWnd);
        }
    }

    if ((g_taskbarVersion == WIN_11) && !g_hookedInputSiteWindows.contains(hWnd))
    {
        HWND hXamlIslandWnd = FindWindowEx(hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd)
        {
            HWND hInputSiteWnd = FindWindowEx(hXamlIslandWnd, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd)
            {
                if (HandleIdentifiedInputSiteWindow(hInputSiteWnd))
                {
                    g_hookedInputSiteWindows.insert(hInputSiteWnd);
                }
            }
        }
    }
}

// Subclasses secondary taskbar window and hooks InputSite for Win11
void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();

    if (!g_dwTaskbarThreadId || GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId)
    {
        return;
    }

    g_secondaryTaskbarWindows = KeepOnlyValidWindows(g_secondaryTaskbarWindows);
    g_secondaryTaskbarWindows.insert(hWnd);

    if (!g_subclassedTaskbarWindows.contains(hWnd))
    {
        if (SubclassTaskbarWindow(hWnd))
        {
            LOG_DEBUG(L"Secondary taskbar window 0x%08X subclassed successfully", (DWORD)(ULONG_PTR)hWnd);
        }
    }

    if ((g_taskbarVersion == WIN_11) && !g_hookedInputSiteWindows.contains(hWnd))
    {
        HWND hXamlIslandWnd = FindWindowEx(hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd)
        {
            HWND hInputSiteWnd = FindWindowEx(hXamlIslandWnd, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd)
            {
                if (HandleIdentifiedInputSiteWindow(hInputSiteWnd))
                {
                    g_hookedInputSiteWindows.insert(hInputSiteWnd);
                }
            }
        }
    }
}

// Checks if module is ExplorerPatcher based on filename
bool IsExplorerPatcherModule(HMODULE module)
{
    LOG_TRACE();

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath)))
    {
    case 0:
    case ARRAYSIZE(moduleFilePath):
        return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName)
    {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) == 0)
    {
        return true;
    }

    return false;
}

// Scans loaded modules for ExplorerPatcher and switches to Win10 mode
void HandleLoadedExplorerPatcher()
{
    LOG_TRACE();

    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded))
    {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++)
        {
            if (IsExplorerPatcherModule(hMods[i]))
            {
                g_isExplorerPatcherDetected = true;
                if (g_taskbarVersion != WIN_10)
                {
                    LOG_INFO(L"ExplorerPatcher module detected, switching to WIN_10 taskbar mode");
                }
                g_taskbarVersion = WIN_10;
                break;
            }
        }
    }
}

// Checks newly loaded module for ExplorerPatcher and switches to Win10 mode
void HandleLoadedModuleIfExplorerPatcher(HMODULE module)
{
    LOG_TRACE();

    if (module && !((ULONG_PTR)module & 3))
    {
        if (IsExplorerPatcherModule(module))
        {
            g_isExplorerPatcherDetected = true;
            if (g_taskbarVersion != WIN_10)
            {
                LOG_INFO(L"ExplorerPatcher module detected, switching to WIN_10 taskbar mode");
            }
            g_taskbarVersion = WIN_10;
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags)
{
    LOG_TRACE();

    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module)
    {
        HandleLoadedModuleIfExplorerPatcher(module);
    }

    return module;
}

// Finds main and secondary taskbar windows in current process
std::tuple<HWND, std::unordered_set<HWND>> FindTaskbarWindows()
{
    LOG_TRACE();

    struct ENUM_WINDOWS_PARAM
    {
        HWND *hPrimary;
        std::unordered_set<HWND> *secondaries;
    };

    HWND hPrimaryWnd = NULL;
    std::unordered_set<HWND> secondaryTaskbarWindows;

    ENUM_WINDOWS_PARAM param = {&hPrimaryWnd, &secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL)
        {
            ENUM_WINDOWS_PARAM &param = *(ENUM_WINDOWS_PARAM *)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId())
                return TRUE;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0)
            {
                *param.hPrimary = hWnd;
            }
            else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0)
            {
                param.secondaries->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return std::make_tuple(hPrimaryWnd, secondaryTaskbarWindows);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                 int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    LOG_TRACE();

    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent,
                                         hMenu, hInstance, lpParam);

    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0)
    {
        LOG_DEBUG(L"Shell_TrayWnd window created: 0x%08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    }
    else if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0)
    {
        LOG_DEBUG(L"Shell_SecondaryTrayWnd window created: 0x%08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI *)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                            int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                            HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                    int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                    LPVOID lpParam, DWORD dwBand)
{
    LOG_TRACE();

    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent,
                                            hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName && _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) // container for Windows 11 taskbar
    {
        if ((g_taskbarVersion == WIN_11) && !g_hookedInputSiteWindows.contains(hWnd))
        {
            HWND hParentWnd = GetParent(hWnd);
            if (!hParentWnd || GetClassNameString(hParentWnd) != L"Windows.UI.Composition.DesktopWindowContentBridge")
            {
                return hWnd;
            }

            hParentWnd = GetParent(hParentWnd);
            if (!hParentWnd || !IsTaskbarWindow(hParentWnd))
            {
                return hWnd;
            }

            LOG_DEBUG(L"Taskbar InputSite window created: 0x%08X", (DWORD)(ULONG_PTR)hWnd);
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

#pragma endregion // hooks_and_win32_methods

// =====================================================================

#pragma region general_helpers

namespace stringtools
{
    std::wstring ltrim(const std::wstring &s)
    {
        std::wstring result = s;
        if (!result.empty())
        {
            result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](wchar_t ch)
                                                      { return !std::iswspace(ch); }));
        }
        return result;
    }

    std::wstring rtrim(const std::wstring &s)
    {
        std::wstring result = s;
        result.erase(std::find_if(result.rbegin(), result.rend(), [](wchar_t ch)
                                  { return !std::iswspace(ch); })
                         .base(),
                     result.end());
        return result;
    }

    std::wstring trim(const std::wstring &s)
    {
        return rtrim(ltrim(s));
    }

    std::wstring toLower(const std::wstring &s)
    {
        std::wstring result = s;
        std::transform(result.begin(), result.end(), result.begin(), ::towlower);
        return result;
    }

    bool startsWith(const std::wstring &s, const std::wstring &prefix)
    {
        if (s.length() < prefix.length())
        {
            return false;
        }
        return std::equal(prefix.begin(), prefix.end(), s.begin());
    }

    std::wstring join(const std::vector<std::wstring> &elements, const std::wstring &delimiter)
    {
        std::wstring result;
        for (const auto &elem : elements)
        {
            if (!result.empty())
            {
                result += delimiter;
            }
            result += elem;
        }
        return result;
    }
}

// Sets a bit in a bitmask
void SetBit(uint32_t &value, uint32_t bit)
{
    value |= (1U << bit);
}

// Checks if a bit is set in a bitmask
bool GetBit(const uint32_t &value, uint32_t bit)
{
    return (value & (1U << bit)) != 0;
}

#pragma endregion // general_helpers

// =====================================================================

#pragma region win32_helpers

#ifdef ENABLE_LOG_DEBUG
const wchar_t *GetMessageName(UINT uMsg)
{
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        return L"WM_MOUSEMOVE";
    case WM_LBUTTONDOWN:
        return L"WM_LBUTTONDOWN";
    case WM_LBUTTONUP:
        return L"WM_LBUTTONUP";
    case WM_LBUTTONDBLCLK:
        return L"WM_LBUTTONDBLCLK";
    case WM_RBUTTONDOWN:
        return L"WM_RBUTTONDOWN";
    case WM_RBUTTONUP:
        return L"WM_RBUTTONUP";
    case WM_RBUTTONDBLCLK:
        return L"WM_RBUTTONDBLCLK";
    case WM_MBUTTONDOWN:
        return L"WM_MBUTTONDOWN";
    case WM_MBUTTONUP:
        return L"WM_MBUTTONUP";
    case WM_MBUTTONDBLCLK:
        return L"WM_MBUTTONDBLCLK";
    case WM_XBUTTONDOWN:
        return L"WM_XBUTTONDOWN";
    case WM_XBUTTONUP:
        return L"WM_XBUTTONUP";
    case WM_XBUTTONDBLCLK:
        return L"WM_XBUTTONDBLCLK";
    case WM_NCLBUTTONDOWN:
        return L"WM_NCLBUTTONDOWN";
    case WM_NCLBUTTONUP:
        return L"WM_NCLBUTTONUP";
    case WM_NCLBUTTONDBLCLK:
        return L"WM_NCLBUTTONDBLCLK";
    case WM_NCRBUTTONDOWN:
        return L"WM_NCRBUTTONDOWN";
    case WM_NCRBUTTONUP:
        return L"WM_NCRBUTTONUP";
    case WM_NCRBUTTONDBLCLK:
        return L"WM_NCRBUTTONDBLCLK";
    case WM_NCMBUTTONDOWN:
        return L"WM_NCMBUTTONDOWN";
    case WM_NCMBUTTONUP:
        return L"WM_NCMBUTTONUP";
    case WM_NCMBUTTONDBLCLK:
        return L"WM_NCMBUTTONDBLCLK";
    case WM_NCXBUTTONDOWN:
        return L"WM_NCXBUTTONDOWN";
    case WM_NCXBUTTONUP:
        return L"WM_NCXBUTTONUP";
    case WM_NCXBUTTONDBLCLK:
        return L"WM_NCXBUTTONDBLCLK";
    case WM_POINTERDOWN:
        return L"WM_POINTERDOWN";
    case WM_POINTERUP:
        return L"WM_POINTERUP";
    case WM_POINTERUPDATE:
        return L"WM_POINTERUPDATE";
    case WM_CREATE:
        return L"WM_CREATE";
    case WM_DESTROY:
        return L"WM_DESTROY";
    case WM_MOVE:
        return L"WM_MOVE";
    case WM_SIZE:
        return L"WM_SIZE";
    case WM_ACTIVATE:
        return L"WM_ACTIVATE";
    case WM_SETFOCUS:
        return L"WM_SETFOCUS";
    case WM_KILLFOCUS:
        return L"WM_KILLFOCUS";
    case WM_PAINT:
        return L"WM_PAINT";
    case WM_CLOSE:
        return L"WM_CLOSE";
    case WM_CONTEXTMENU:
        return L"WM_CONTEXTMENU";
    case WM_NCDESTROY:
        return L"WM_NCDESTROY";
    case WM_NCPAINT:
        return L"WM_NCPAINT";
    case WM_TIMER:
        return L"WM_TIMER";
    case WM_PARENTNOTIFY:
        return L"WM_PARENTNOTIFY";
    case WM_QUIT:
        return L"WM_QUIT";
    case WM_SHOWWINDOW:
        return L"WM_SHOWWINDOW";
    case WM_WINDOWPOSCHANGING:
        return L"WM_WINDOWPOSCHANGING";
    case WM_WINDOWPOSCHANGED:
        return L"WM_WINDOWPOSCHANGED";
    case WM_NCACTIVATE:
        return L"WM_NCACTIVATE";
    case WM_ACTIVATEAPP:
        return L"WM_ACTIVATEAPP";
    case WM_GETOBJECT:
        return L"WM_GETOBJECT";
    case WM_SETCURSOR:
        return L"WM_SETCURSOR";
    case WM_IME_SETCONTEXT:
        return L"WM_IME_SETCONTEXT";
    case WM_IME_NOTIFY:
        return L"WM_IME_NOTIFY";
    case WM_KEYUP:
        return L"WM_KEYUP";
    case WM_KEYDOWN:
        return L"WM_KEYDOWN";
    case WM_POINTERENTER:
        return L"WM_POINTERENTER";
    case WM_POINTERLEAVE:
        return L"WM_POINTERLEAVE";
    case WM_MOUSEACTIVATE:
        return L"WM_MOUSEACTIVATE";
    case WM_NCHITTEST:
        return L"WM_NCHITTEST";
    case WM_NOTIFY:
        return L"WM_NOTIFY";
    case WM_NCMOUSEMOVE:
        return L"WM_NCMOUSEMOVE";

    default:
    {
        static wchar_t buffer[32];
        swprintf(buffer, ARRAYSIZE(buffer), L"0x%04X", uMsg);
        return buffer;
    }
    }
}

// Converts virtual key code to string representation
std::wstring VKToString(UINT vk)
{
    UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    WCHAR keyName[50];
    LONG lParam = scanCode << 16;

    // Set extended-key flag (bit 24) for extended keys
    switch (vk)
    {
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR: // Page Up
    case VK_NEXT:  // Page Down
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_RCONTROL:
    case VK_RMENU: // Right Alt
    case VK_NUMLOCK:
    case VK_DIVIDE: // Numpad /
        lParam |= (1 << 24);
        break;
    }

    if (GetKeyNameText(lParam, keyName, ARRAYSIZE(keyName)))
        return keyName;
    return L"";
}
#endif

// GetClassName wrapper to avoid dealing C-style strings
std::wstring GetClassNameString(HWND hWnd)
{
    LOG_TRACE();

    if (!hWnd)
    {
        LOG_ERROR(L"GetClassNameString called with NULL hWnd");
        return std::wstring();
    }

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
    {
        LOG_ERROR(L"GetClassName failed for HWND 0x%08X", (DWORD)(ULONG_PTR)hWnd);
        return std::wstring();
    }
    return std::wstring(szClassName);
}

// GetWindowText wrapper to avoid dealing C-style strings
std::wstring GetWindowTextString(HWND hWnd)
{
    LOG_TRACE();

    if (!hWnd)
    {
        LOG_ERROR(L"GetWindowTextString called with NULL hWnd");
        return std::wstring();
    }

    WCHAR szWindowText[256];
    if (GetWindowText(hWnd, szWindowText, ARRAYSIZE(szWindowText)) == 0)
    {
        LOG_ERROR(L"GetWindowText failed for HWND 0x%08X", (DWORD)(ULONG_PTR)hWnd);
        return std::wstring();
    }
    return std::wstring(szWindowText);
}

// Extracts version info from module resources
VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen)
{
    LOG_TRACE();

    HRSRC hResource;
    HGLOBAL hGlobal;
    void *pData;
    void *pFixedFileInfo;
    UINT uPtrLen;

    pFixedFileInfo = NULL;
    uPtrLen = 0;

    hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource != NULL)
    {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal != NULL)
        {
            pData = LockResource(hGlobal);
            if (pData != NULL)
            {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0)
                {
                    pFixedFileInfo = NULL;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO *)pFixedFileInfo;
}

// Detects Windows version and sets taskbar version (Win10/Win11/Unknown)
bool LoadExplorerVersion()
{
    LOG_TRACE();

    VS_FIXEDFILEINFO *pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo)
    {
        LOG_ERROR(L"Failed to get Windows module version info");
        return false;
    }

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    WORD nRevision = LOWORD(pFixedFileInfo->dwFileVersionLS);
    LOG_INFO(L"Windows version: %d.%d.%d.%d", nMajor, nMinor, nBuild, nRevision);

    if (nMajor == 6)
    {
        g_taskbarVersion = WIN_10;
    }
    else if (nMajor == 10)
    {
        if (nBuild < 22000)
        { // 21H2
            g_taskbarVersion = WIN_10;
        }
        else
        {
            g_taskbarVersion = WIN_11;
        }
    }
    else
    {
        g_taskbarVersion = UNKNOWN;
    }
    g_osVersion = g_taskbarVersion; // we don't care about exact OS version, just about pre-XAML and XAML UI
    return true;
}

// Loads twinui.pcshell.dll version to get build number to properly switch virtual desktops
bool LoadTwinuiPCShellVersion()
{
    bool success = false;
    HMODULE hModule = GetModuleHandle(L"twinui.pcshell.dll");
    if (hModule)
    {
        VS_FIXEDFILEINFO *pFixedFileInfo = GetModuleVersionInfo(hModule, NULL);
        if (pFixedFileInfo)
        {
            success = true;
            WORD major = HIWORD(pFixedFileInfo->dwFileVersionMS);
            WORD minor = LOWORD(pFixedFileInfo->dwFileVersionMS);
            WORD build = HIWORD(pFixedFileInfo->dwFileVersionLS);
            WORD revision = LOWORD(pFixedFileInfo->dwFileVersionLS);
            g_twinuiPCShellBuildNumber = build;
            LOG_INFO(L"twinui.pcshell.dll version: %d.%d.%d.%d", major, minor, build, revision);
        }
        else
        {
            LOG_ERROR(L"Failed to get twinui.pcshell.dll version info");
        }
    }
    else
    {
        LOG_ERROR(L"twinui.pcshell.dll not loaded in current process");
    }
    return success;
}

// Finds desktop window for show/hide icons command
HWND FindDesktopWindow()
{
    LOG_TRACE();

    HWND hParentWnd = FindWindow(L"Progman", NULL); // Program Manager window
    if (!hParentWnd)
    {
        LOG_ERROR(L"Failed to find Progman window");
        return NULL;
    }

    HWND hChildWnd = FindWindowEx(hParentWnd, NULL, L"SHELLDLL_DefView", NULL); // parent window of the desktop
    if (!hChildWnd)
    {
        DWORD dwThreadId = GetWindowThreadProcessId(hParentWnd, NULL);
        EnumThreadWindows(
            dwThreadId, [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL)
            {
            WCHAR szClassName[16];
            if (GetClassName(hWnd, szClassName, _countof(szClassName)) == 0)
                return TRUE;

            if (lstrcmp(szClassName, L"WorkerW") != 0)
                return TRUE;

            HWND hChildWnd = FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
            if (!hChildWnd)
                return TRUE;

            *(HWND *)lParam = hChildWnd;
            return FALSE; },
            (LPARAM)&hChildWnd);
    }

    if (!hChildWnd)
    {
        LOG_ERROR(L"Failed to find SHELLDLL_DefView window");
        return NULL;
    }
    return hChildWnd;
}

// Finds Task Switching window (Alt+Tab)
HWND FindTaskSwitchingWindow()
{
    LOG_TRACE();

    struct ENUM_WINDOWS_PARAM
    {
        HWND hWnd;
    };

    ENUM_WINDOWS_PARAM param = {NULL};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL)
        {
            ENUM_WINDOWS_PARAM &param = *(ENUM_WINDOWS_PARAM *)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId())
                return TRUE;

            const auto className = GetClassNameString(hWnd);
            if (className.empty() || ((className != L"XamlExplorerHostIslandWindow") && (className != L"MultitaskingViewFrame")))
                return TRUE;

            if (className == L"XamlExplorerHostIslandWindow") // Windows 11
            {
                // XamlExplorerHostIslandWindow is used as a host not just for the Task Switching window, but also for e.g. that laggy modern Window
                // tiling crap on Win11 - so we need to indentify the window
                DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
                if (!threadId)
                    return TRUE;

                HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
                if (!thread)
                    return TRUE;

                PWSTR threadDescription;
                HRESULT hr = GetThreadDescription(thread, &threadDescription);
                CloseHandle(thread);
                if (FAILED(hr))
                    return TRUE;

                constexpr auto expectedThreadDescription = L"MultitaskingView";
                const bool isMultitaskingView = wcscmp(threadDescription, expectedThreadDescription) == 0;
                LocalFree(threadDescription);
                if (!isMultitaskingView)
                    return TRUE;

                LOG_DEBUG(L"Found Win11 Task Switching window - class %s (thread desc: %s) : 0x%08X", className.c_str(), expectedThreadDescription, (DWORD)(ULONG_PTR)hWnd);
            }
            else
            {
                LOG_DEBUG(L"Found Win10 Task Switching window - class %s : 0x%08X", className.c_str(), (DWORD)(ULONG_PTR)hWnd);
            }

            DWORD band = 0;
            // expecting ZBID_SYSTEM_TOOLS band, see https://blog.adeltax.com/window-z-order-in-windows-10/
            if (!pGetWindowBand || !pGetWindowBand(hWnd, &band) || (band != ZBID_SYSTEM_TOOLS)) 
                return TRUE;

            param.hWnd = hWnd; // return the found window
            return FALSE;
        },
        (LPARAM)&param);

    return param.hWnd;
}

// Checks if window is Shell_TrayWnd class
bool IsPrimaryTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();
    return GetClassNameString(hWnd) == L"Shell_TrayWnd";
}

// Checks if window is Shell_SecondaryTrayWnd class
bool IsSecondaryTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();
    return GetClassNameString(hWnd) == L"Shell_SecondaryTrayWnd";
}

// Checks if window is either Shell_TrayWnd or Shell_SecondaryTrayWnd class
bool IsTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();
    return IsPrimaryTaskbarWindow(hWnd) || IsSecondaryTaskbarWindow(hWnd);
}

// Returns a new set containing only valid window handles from the input set
std::unordered_set<HWND> KeepOnlyValidWindows(const std::unordered_set<HWND> &windows)
{
    std::unordered_set<HWND> validWindows;
    for (HWND hWnd : windows)
    {
        if (IsWindow(hWnd))
        {
            validWindows.insert(hWnd);
        }
        else
        {
            LOG_DEBUG(L"Filtered out invalid window handle: 0x%08X", (DWORD)(ULONG_PTR)hWnd);
        }
    }
    return validWindows;
}

// Checks if user clicked on a Task View item (running app) on the Task Switching window
bool UserClickedOnTaskViewItemOnTaskSwitchingWindow(const POINT &pt)
{
    LOG_TRACE();

    com_ptr<IUIAutomation> pUIAutomation = g_comAPI.GetUIAutomation();
    if (!pUIAutomation)
    {
        return false;
    }

    // UIAutomation::ElementFromPoint can be a bit buggy on Windows 11 on Task Switching window,
    // for whatever reason pressing Win+Tab once fixes that behavior until session logout
    com_ptr<IUIAutomationElement> pWindowElement = NULL;
    if (FAILED(pUIAutomation->ElementFromPoint(pt, pWindowElement.put())) || !pWindowElement)
    {
        return false;
    }

    bstr_ptr uiaClassName_cstr;
    pWindowElement->get_CurrentClassName(uiaClassName_cstr.GetAddress());
    const std::wstring uiaClassName = !uiaClassName_cstr ? L"" : uiaClassName_cstr.GetBSTR();

    bstr_ptr uiaName_cstr;
    pWindowElement->get_CurrentName(uiaName_cstr.GetAddress());
    const std::wstring uiaName = !uiaName_cstr ? L"" : uiaName_cstr.GetBSTR();

    const HWND hRootWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);
    const auto wndClassName = GetClassNameString(hRootWnd);
    const auto wndName = GetWindowTextString(hRootWnd);

    LOG_DEBUG(L"Clicked element: UIA class name: %s", uiaClassName.c_str());
    LOG_DEBUG(L"Clicked element: UIA name : %s", uiaName.c_str());
    LOG_DEBUG(L"Clicked element: root Window class name: %s", wndClassName.c_str());
    LOG_DEBUG(L"Clicked element: root Window name : %s", wndName.c_str());

    if ((uiaClassName == L"ListViewItem") && (wndClassName == L"XamlExplorerHostIslandWindow") && (wndName == L"Task Switching"))
    {
        return true; // windows 11
    }
    else if ((wndClassName == L"MultitaskingViewFrame") && (wndName == L"Task Switching") && (uiaName != L"Dismiss Task Switching Window"))
    {
        return true; // windows 10
    }
    return false;
}

// Handles mouse click on Task Switching window, returns true to suppress the click
bool HandleTaskSwitchingWindowClick(const POINT &position)
{
    LOG_TRACE();

    bool supressClick = false;
    if (!UserClickedOnTaskViewItemOnTaskSwitchingWindow(position))
    {
        LOG_DEBUG("User clicked outside of Task Switching dialog, closing it and stopping mouse input suppression");
        CloseCtrlAltTabDialog();
        supressClick = true; // suppress the message
    }
    else
    {
        LOG_DEBUG("User clicked on item from Task Switching dialog, stopping mouse input suppression");
        g_keepTaskSwitchingOpened = false;
    }
    return supressClick;
}

#pragma endregion // hooks_and_win32_methods

// =====================================================================

#pragma region actions

// Converts key name string to KeyModifier enum
KeyModifier GetKeyModifierFromName(const std::wstring &keyName)
{
    LOG_TRACE();

    if (keyName == L"lctrl")
        return KEY_MODIFIER_LCTRL;
    if (keyName == L"rctrl")
        return KEY_MODIFIER_RCTRL;
    if (keyName == L"lshift")
        return KEY_MODIFIER_LSHIFT;
    if (keyName == L"rshift")
        return KEY_MODIFIER_RSHIFT;
    if (keyName == L"lalt")
        return KEY_MODIFIER_LALT;
    if (keyName == L"ralt")
        return KEY_MODIFIER_RALT;
    if (keyName == L"win")
        return KEY_MODIFIER_LWIN;
    LOG_ERROR(L"Unknown key name '%s'", keyName.c_str());
    return KEY_MODIFIER_INVALID; // Return 0 for unrecognized key names
}

// Splits semicolon-separated argument string into vector
std::vector<std::wstring> SplitArgs(const std::wstring &args, const wchar_t delimiter = L';')
{
    LOG_TRACE();

    std::vector<std::wstring> result;
    std::wstring args_ = stringtools::trim(args);
    if (args_.empty())
    {
        return result;
    }

    size_t start = 0;
    size_t end = args_.find(delimiter);
    while (end != std::wstring::npos)
    {
        auto substring = stringtools::trim(args_.substr(start, end - start));
        if (!substring.empty())
        {
            result.push_back(substring);
        }
        start = end + 1;
        end = args_.find(delimiter, start);
    }
    auto substring = stringtools::trim(args_.substr(start));
    if (!substring.empty())
    {
        result.push_back(substring);
    }
    return result;
}

// Parses taskbar button combine states from arg string (4 states: primary1, primary2, secondary1, secondary2)
std::tuple<TaskBarButtonsState, TaskBarButtonsState, TaskBarButtonsState, TaskBarButtonsState> ParseTaskBarButtonsState(const std::wstring &args)
{
    LOG_TRACE();

    // defaults in case parsing fails
    TaskBarButtonsState primaryTaskBarButtonsState1 = COMBINE_INVALID;
    TaskBarButtonsState primaryTaskBarButtonsState2 = COMBINE_INVALID;
    TaskBarButtonsState secondaryTaskBarButtonsState1 = COMBINE_INVALID;
    TaskBarButtonsState secondaryTaskBarButtonsState2 = COMBINE_INVALID;

    const auto argsSplit = SplitArgs(args);
    if (argsSplit.size() != 4)
    {
        LOG_ERROR(L"Invalid number of arguments for taskbar buttons state setting. "
                  "Expected format is: PRIMARY_STATE1;PRIMARY_STATE2;SECONDARY_STATE1;SECONDARY_STATE2");
    }

    auto parseTaskBarButtonState = [](const std::wstring &arg) -> TaskBarButtonsState
    {
        if (arg == L"COMBINE_ALWAYS")
        {
            return COMBINE_ALWAYS;
        }
        else if (arg == L"COMBINE_WHEN_FULL")
        {
            return COMBINE_WHEN_FULL;
        }
        else if (arg == L"COMBINE_NEVER")
        {
            return COMBINE_NEVER;
        }
        else
        {
            LOG_ERROR(L"Unknown state '%s' for taskbar buttons state setting", arg.c_str());
            return COMBINE_INVALID; // Default value in case of error
        }
    };

    // even if the parsing fails, we parse as much as we can (e.g. user is not interested in secondary taskbar buttons state)
    if (argsSplit.size() >= 1)
        primaryTaskBarButtonsState1 = parseTaskBarButtonState(argsSplit[0]);
    if (argsSplit.size() >= 2)
        primaryTaskBarButtonsState2 = parseTaskBarButtonState(argsSplit[1]);
    if (argsSplit.size() >= 3)
        secondaryTaskBarButtonsState1 = parseTaskBarButtonState(argsSplit[2]);
    if (argsSplit.size() >= 4)
        secondaryTaskBarButtonsState2 = parseTaskBarButtonState(argsSplit[3]);

    return std::make_tuple(primaryTaskBarButtonsState1, primaryTaskBarButtonsState2, secondaryTaskBarButtonsState1, secondaryTaskBarButtonsState2);
}

// Parses virtual key code from hex (0x prefix) or decimal string
unsigned int ParseVirtualKey(const wchar_t *value)
{
    LOG_TRACE();

    const auto base = (wcsncmp(value, L"0x", 2) == 0) ? 16 : 10; // 0x prefix means hex
    const auto number = std::wcstol(value, nullptr, base);

    unsigned int keyCode = 0;
    if ((number > 0) && (number < 0xFF)) // expected valid key code range
    {
        keyCode = static_cast<unsigned int>(number);
    }
    else
    {
        LOG_ERROR(L"Failed to parse virtual key code from string '%s'", value);
    }
    return keyCode;
}

// Returns trimmed command string for process execution
std::wstring ParseProcessArg(const std::wstring &args)
{
    LOG_TRACE();

    auto cmd = stringtools::trim(args); // take the whole string as command
    if (cmd.empty())
    {
        LOG_ERROR(L"Empty process name / command");
    }
    return cmd;
}

// Parses virtual key codes and focusPreviousWindow flag from settings string
std::tuple<std::vector<int>, bool> ParseVirtualKeypressSetting(const std::wstring &args)
{
    LOG_TRACE();

    std::vector<int> keys;

    bool focusPreviousWindow = false;
    const auto argsSplit = SplitArgs(args);
    for (const auto &arg : argsSplit)
    {
        if (stringtools::toLower(arg) == L"focuspreviouswindow")
        {
            focusPreviousWindow = true;
        }
        else
        {
            const auto keyCode = ParseVirtualKey(arg.c_str());
            if (keyCode)
            {
                keys.push_back(keyCode);
            }
        }
    }

    return std::make_tuple(keys, focusPreviousWindow);
}

// Parses Ctrl+Alt+Tab, SwitchVirtualDesktop action arguments
bool ParseReverseArg(const std::wstring &args)
{
    LOG_TRACE();

    bool reverse = false;
    const auto argsSplit = SplitArgs(args);
    for (const auto &arg : argsSplit)
    {
        if (stringtools::toLower(arg) == L"reverse")
        {
            reverse = true;
        }
    }
    return reverse;
}

// Creates action executor lambda from action name and arguments
std::function<void(HWND)> ParseMouseActionSetting(const std::wstring &actionName, const std::wstring &args)
{
    LOG_TRACE();

    auto doNothing = [](HWND)
    { LOG_INFO(L"Doing empty action"); };

    if (actionName == L"ACTION_NOTHING")
    {
        return doNothing;
    }
    else if (actionName == L"ACTION_SHOW_DESKTOP")
    {
        return [](HWND)
        { ShowDesktop(); };
    }
    else if (actionName == L"ACTION_ALT_TAB")
    {
        bool reverse = ParseReverseArg(args);
        return [reverse](HWND)
        { SendCtrlAltTabKeypress(reverse); };
    }
    else if (actionName == L"ACTION_SWITCH_VIRTUAL_DESKTOP")
    {
        bool reverse = ParseReverseArg(args);
        return [reverse](HWND)
        { SwitchVirtualDesktop(reverse); };
    }
    else if (actionName == L"ACTION_TASK_MANAGER")
    {
        return [](HWND hWnd)
        { OpenTaskManager(hWnd); };
    }
    else if (actionName == L"ACTION_MUTE")
    {
        return [](HWND)
        { ToggleVolMuted(); };
    }
    else if (actionName == L"ACTION_TASKBAR_AUTOHIDE")
    {
        return [](HWND)
        { ToggleTaskbarAutohide(); };
    }
    else if (actionName == L"ACTION_WIN_TAB")
    {
        return [](HWND)
        { SendWinTabKeypress(); };
    }
    else if (actionName == L"ACTION_HIDE_ICONS")
    {
        return [](HWND)
        { HideIcons(); };
    }
    else if (actionName == L"ACTION_COMBINE_TASKBAR_BUTTONS")
    {
        const auto [primaryState1, primaryState2, secondaryState1, secondaryState2] = ParseTaskBarButtonsState(args);
        return [primaryState1, primaryState2, secondaryState1, secondaryState2](HWND)
        { CombineTaskbarButtons(primaryState1, primaryState2, secondaryState1, secondaryState2); };
    }
    else if (actionName == L"ACTION_TOGGLE_TASKBAR_ALIGNMENT")
    {
        return [](HWND)
        { ToggleTaskbarAlignment(); };
    }
    else if (actionName == L"ACTION_OPEN_START_MENU")
    {
        return [](HWND)
        { OpenStartMenu(); };
    }
    else if (actionName == L"ACTION_SEND_KEYPRESS")
    {
        const auto [keyCodes, focusPreviousWindow] = ParseVirtualKeypressSetting(args);

        // start focus tracker only if needed
        if (focusPreviousWindow && !g_windowFocusTracker.IsRunning())
        {
            g_windowFocusTracker.Start();
        }

        return [keyCodes, focusPreviousWindow](HWND)
        {
            LOG_INFO(L"Sending arbitrary keypress");
            SendKeypress(keyCodes, focusPreviousWindow);
        };
    }
    else if (actionName == L"ACTION_START_PROCESS")
    {
        const auto cmd = ParseProcessArg(args);
        return [cmd](HWND)
        {
            StartProcess(cmd);
        };
    }
    else if (actionName == L"ACTION_MEDIA_PLAY_PAUSE")
    {
        return [](HWND)
        { MediaPlayPause(); };
    }
    else if (actionName == L"ACTION_MEDIA_NEXT")
    {
        return [](HWND)
        { MediaNext(); };
    }
    else if (actionName == L"ACTION_MEDIA_PREV")
    {
        return [](HWND)
        { MediaPrev(); };
    }

    LOG_ERROR(L"Unknown action '%s'", actionName.c_str());
    return doNothing;
}

// Loads trigger-action settings from Windhawk configuration
void LoadSettings()
{
    LOG_TRACE();

    using WindhawkUtils::StringSetting;

    g_settings.triggerActions.clear();
    for (int i = 0; i < 50; i++) // the loop will get interrupted by empty item
    {
        std::vector<std::wstring> keyboardTriggers;
        for (int j = 0; j < 50; j++) // the loop will get interrupted by empty item
        {
            auto keyboardTriggerStr = std::wstring(StringSetting(Wh_GetStringSetting(L"TriggerActionOptions[%d].KeyboardTriggers[%d]", i, j)).get());
            if (keyboardTriggerStr.empty())
                break;
            keyboardTriggers.push_back(keyboardTriggerStr);
        }
        auto mouseTriggerStr = std::wstring(StringSetting(Wh_GetStringSetting(L"TriggerActionOptions[%d].MouseTrigger", i)));
        auto taskbarTypeStr = std::wstring(StringSetting(Wh_GetStringSetting(L"TriggerActionOptions[%d].TaskbarType", i)));
        auto actionStr = std::wstring(StringSetting(Wh_GetStringSetting(L"TriggerActionOptions[%d].Action", i)));
        auto additionalArgsStr = std::wstring(StringSetting(Wh_GetStringSetting(L"TriggerActionOptions[%d].AdditionalArgs", i)));

        // no other actions were added by user, end parsing
        if (keyboardTriggers.empty() && mouseTriggerStr.empty() && taskbarTypeStr.empty() && actionStr.empty() && additionalArgsStr.empty())
            break;

        // if mouse trigger or action is missing, skip since the rest is irrelevant
        if (mouseTriggerStr.empty() || actionStr.empty())
            continue;

        // handle default value
        if (taskbarTypeStr.empty())
        {
            taskbarTypeStr = L"all";
        }

#ifdef ENABLE_LOG_INFO
        std::wstring logMessage = std::wstring(L"Settings: TriggerActionOptions[") + std::to_wstring(i) + std::wstring(L"] = ");
        for (const auto &keyboardTrigger : keyboardTriggers)
        {
            logMessage += L"Key(";
            logMessage += keyboardTrigger.c_str();
            logMessage += L") + ";
        }
        logMessage += L"Mouse(";
        logMessage += mouseTriggerStr.c_str();
        logMessage += L") + ";
        logMessage += L"TaskbarType(";
        logMessage += taskbarTypeStr.c_str();
        logMessage += L") -> ";
        logMessage += actionStr.c_str();
        if (!additionalArgsStr.empty())
        {
            logMessage += L" (";
            logMessage += additionalArgsStr.c_str();
            logMessage += L")";
        }
        LOG_INFO(L"%s", logMessage.c_str());
#endif

        // parse trigger->action settings
        TriggerAction triggerAction{};
        triggerAction.expectedKeyModifiersState = 0U;
        for (const auto &keyboardTrigger : keyboardTriggers)
        {
            if (keyboardTrigger == L"none")
                continue;

            KeyModifier keyModifier = GetKeyModifierFromName(keyboardTrigger);
            if (keyModifier != KEY_MODIFIER_INVALID)
            {
                SetBit(triggerAction.expectedKeyModifiersState, keyModifier);
            }
        }
        // when settings storage is empty, it can return all key modifiers set and users may not be aware of it -> assume 'none' in that case
        uint32_t defaultKeyModifierValue = 0;
        for (int i = 0; i < KeyModifier::KEY_MODIFIER_INVALID; i++)
        {
            SetBit(defaultKeyModifierValue, i);
        }
        if (triggerAction.expectedKeyModifiersState == defaultKeyModifierValue)
        {
            LOG_INFO(L"Default (invalid) keyboard modifiers detected for TriggerActionOptions[%d], ignoring", i);
            triggerAction.expectedKeyModifiersState = 0U; // no modifiers
        }

        triggerAction.mouseTriggerName = mouseTriggerStr;
        triggerAction.taskbarTypeName = taskbarTypeStr;
        triggerAction.actionName = actionStr;
        triggerAction.actionExecutor = ParseMouseActionSetting(actionStr, additionalArgsStr);
        g_settings.triggerActions.push_back(triggerAction);
    }

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
    g_settings.eagerTriggerEvaluation = Wh_GetIntSetting(L"eagerTriggerEvaluation");
}

// Returns current taskbar autohide state
bool GetTaskbarAutohideState()
{
    LOG_TRACE();

    if (g_hTaskbarWnd != NULL)
    {
        APPBARDATA msgData{};
        msgData.cbSize = sizeof(msgData);
        msgData.hWnd = g_hTaskbarWnd;
        LPARAM state = SHAppBarMessage(ABM_GETSTATE, &msgData);
        return state & ABS_AUTOHIDE;
    }
    else
    {
        return false;
    }
}

// Sets taskbar autohide state
void SetTaskbarAutohide(bool enabled)
{
    LOG_TRACE();

    if (g_hTaskbarWnd != NULL)
    {
        APPBARDATA msgData{};
        msgData.cbSize = sizeof(msgData);
        msgData.hWnd = g_hTaskbarWnd;
        msgData.lParam = enabled ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
        SHAppBarMessage(ABM_SETSTATE, &msgData); // always returns TRUE
    }
}

// Toggles taskbar autohide state
void ToggleTaskbarAutohide()
{
    LOG_TRACE();

    if (g_hTaskbarWnd != NULL)
    {
        const bool isEnabled = GetTaskbarAutohideState();
        LOG_INFO(L"Setting taskbar autohide state to %s", !isEnabled ? L"enabled" : L"disabled");
        SetTaskbarAutohide(!isEnabled);
    }
    else
    {
        LOG_ERROR(L"Failed to toggle taskbar autohide - taskbar window not found");
    }
}

// Sends show desktop command to taskbar
void ShowDesktop()
{
    LOG_TRACE();

    if (g_hTaskbarWnd)
    {
        LOG_INFO(L"Sending ShowDesktop message");
        // https://www.codeproject.com/Articles/14380/Manipulating-The-Windows-Taskbar
        if (SendMessage(g_hTaskbarWnd, WM_COMMAND, MAKELONG(407, 0), 0) != 0)
        {
            LOG_ERROR(L"Failed to send ShowDesktop message");
        }
    }
    else
    {
        LOG_ERROR(L"Failed to show desktop - taskbar window not found");
    }
}

// Sends virtual key sequence, optionally focusing previous window first
void SendKeypress(const std::vector<int> &keys, const bool focusPreviousWindow)
{
    LOG_TRACE();

    if (keys.empty())
    {
        LOG_DEBUG(L"No virtual key codes to send");
        return;
    }

    if (focusPreviousWindow && WindowFocusTracker::g_hwndLastActive && IsWindow(WindowFocusTracker::g_hwndLastActive))
    {
        // bring the target window to foreground to ensure it receives the keypresses
        LOG_DEBUG(L"Focusing the previously active window (HWND=0x%08X) before sending keypresses", (DWORD)(ULONG_PTR)WindowFocusTracker::g_hwndLastActive);
        SetForegroundWindow(WindowFocusTracker::g_hwndLastActive);

        // Wait until the window is actually in the foreground
        for (int i = 0; i < 50; i++) // Max 500ms
        {
            if (GetForegroundWindow() == WindowFocusTracker::g_hwndLastActive)
            {
                LOG_DEBUG(L"Focused the previously active window successfully");
                break;
            }
            Sleep(10);
        }
    }

#ifdef ENABLE_LOG_DEBUG
    std::vector<std::wstring> keyNamesStrs;
    for (const auto &key : keys)
    {
        keyNamesStrs.emplace_back(VKToString(key));
    }
    const std::wstring keyNamesStr = stringtools::join(keyNamesStrs, L" + ");
    LOG_DEBUG(L"Sending keypresses: %s", keyNamesStr.c_str());
#endif

    const int NUM_KEYS = keys.size();
    std::unique_ptr<INPUT[]> input(new INPUT[NUM_KEYS * 2]);
    for (int i = 0; i < NUM_KEYS; i++)
    {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.wScan = 0;
        input[i].ki.time = 0;
        input[i].ki.dwExtraInfo = 0;
        input[i].ki.wVk = keys[i];
        input[i].ki.dwFlags = 0; // KEYDOWN
    }

    for (int i = 0; i < NUM_KEYS; i++)
    {
        input[NUM_KEYS + i].type = INPUT_KEYBOARD;
        input[NUM_KEYS + i].ki.wScan = 0;
        input[NUM_KEYS + i].ki.time = 0;
        input[NUM_KEYS + i].ki.dwExtraInfo = 0;
        input[NUM_KEYS + i].ki.wVk = keys[i];
        input[NUM_KEYS + i].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    if (SendInput(NUM_KEYS * 2, input.get(), sizeof(input[0])) != (UINT)(NUM_KEYS * 2))
    {
        LOG_ERROR(L"Failed to send all key inputs");
    }
}

// Sends Ctrl+Alt+Tab keypress
void SendCtrlAltTabKeypress(const bool reverse)
{
    LOG_TRACE();

    LOG_INFO(L"Sending Ctrl+Alt+Tab keypress to open Alt+Tab dialog");

    const std::vector<int> keyCombo = reverse ? std::vector<int>{VK_LCONTROL, VK_LMENU, VK_SHIFT, VK_TAB} : std::vector<int>{VK_LCONTROL, VK_LMENU, VK_TAB};
    SendKeypress(keyCombo);
    if (!g_keepTaskSwitchingOpened) // when opening, call twice, otherwise current window is selected
    {
        SendKeypress(keyCombo);
    }
    g_keepTaskSwitchingOpened = true;
}

// Sends Enter keypress to close Ctrl+Alt+Tab dialog
void CloseCtrlAltTabDialog()
{
    LOG_TRACE();

    LOG_DEBUG(L"Closing Alt+Tab dialog by sending Enter keypress");

    if (g_hTaskSwitchingWnd) // clicking on the Taskbar might have changed the foreground window and the Enter keypress would go to the wrong window
        SetForegroundWindow(g_hTaskSwitchingWnd);

    g_keepTaskSwitchingOpened = false;
    SendKeypress({VK_RETURN});
}

// Switches to the next or previous virtual desktop (with wraparound)
// main credits goes to: u2x1 (virtual-desktop-helper.wh.cpp)
// Note: Since I don't have a header file, I am enclosing all this COM crap in this method to avoid cluttering the global namespace.
void SwitchVirtualDesktop(bool reverse)
{
    LOG_TRACE();

    if ((g_twinuiPCShellBuildNumber == 0) && !LoadTwinuiPCShellVersion()) // load lazily
    {
        LOG_ERROR(L"Cannot switch virtual desktops - failed to determine twinui.pcshell.dll version");
        return;
    }

    // Minimal interface definitions for vtable-based access
    struct IVirtualDesktop : public IUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown *, BOOL *) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetId(GUID *) = 0;
    };

    struct IVirtualDesktopManagerInternal : public IUnknown
    {
    };

    // Virtual Desktop COM interfaces - vary by Windows version
    // These are undocumented interfaces for virtual desktop management

    // CLSID_ImmersiveShell: {C2F03A33-21F5-47FA-B4BB-156362A2F239}
    const CLSID CLSID_ImmersiveShell = {0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

    // CLSID_VirtualDesktopManagerInternal: {C5E0CDCA-7B6E-41B2-9FC4-D93975CC467B}
    const CLSID CLSID_VirtualDesktopManagerInternal = {0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

    IID IID_IVirtualDesktopManagerInternal;
    IID IID_IVirtualDesktop;
    bool usesHMonitor;
    if (g_twinuiPCShellBuildNumber >= 26100) // Windows 11 (Build 26100+ / 24H2)
    {
        // IVirtualDesktopManagerInternal: {53F5CA0B-158F-4124-900C-057158060B27}
        // IVirtualDesktop: {3F07F4BE-B107-441A-AF0F-39D82529072C} (same as 22621)
        IID_IVirtualDesktopManagerInternal = {0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}};
        IID_IVirtualDesktop = {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};
        usesHMonitor = false;
    }
    else if (g_twinuiPCShellBuildNumber >= 22621) // Windows 11 (Build 22621/22631/23H2)
    {
        // IVirtualDesktopManagerInternal: {A3175F2D-239C-4BD2-8AA0-EEBA8B0B138E}
        // IVirtualDesktop: {3F07F4BE-B107-441A-AF0F-39D82529072C}
        IID_IVirtualDesktopManagerInternal = {0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}};
        IID_IVirtualDesktop = {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};
        usesHMonitor = false;
    }
    else if (g_twinuiPCShellBuildNumber >= 22000) // Windows 11 (Build 22000 - 22482)
    {
        // IVirtualDesktopManagerInternal: {B2F925B9-5A0F-4D2E-9F4D-2B1507593C10}
        // IVirtualDesktop: {536D3495-B208-4CC9-AE26-DE8111275BF8}
        IID_IVirtualDesktopManagerInternal = {0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}};
        IID_IVirtualDesktop = {0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}};
        usesHMonitor = true;
    }
    else if (g_twinuiPCShellBuildNumber >= 20348) // Windows Server 2022 (Build 20348 - 21999)
    {
        // IVirtualDesktopManagerInternal: {094AFE11-44F2-4BA0-976F-29A97E263EE0}
        // IVirtualDesktop: {62FDF88B-11CA-4AFB-8BD8-2296DFAE49E2}
        IID_IVirtualDesktopManagerInternal = {0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}};
        IID_IVirtualDesktop = {0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}};
        usesHMonitor = true;
    }
    else // [0] Windows 10 (Build < 20348)
    {
        // IVirtualDesktopManagerInternal: {F31574D6-B682-4CDC-BD56-1827860ABEC6}
        // IVirtualDesktop: {FF72FFDD-BE7E-43FC-9C03-AD81681E88E4}
        IID_IVirtualDesktopManagerInternal = {0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}};
        IID_IVirtualDesktop = {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}};
        usesHMonitor = false;
    }

    LOG_INFO(L"Attempting to switch to %s virtual desktop", reverse ? L"previous" : L"next");

    // Get IServiceProvider from ImmersiveShell
    com_ptr<IServiceProvider> pServiceProvider;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(pServiceProvider.put()));
    if (FAILED(hr))
    {
        LOG_ERROR(L"Failed to get IServiceProvider: 0x%08X", hr);
        return;
    }

    // Get IVirtualDesktopManagerInternal
    com_ptr<IVirtualDesktopManagerInternal> pDesktopManagerInternal;
    hr = pServiceProvider->QueryService(CLSID_VirtualDesktopManagerInternal, IID_IVirtualDesktopManagerInternal, pDesktopManagerInternal.put_void());
    if (FAILED(hr))
    {
        LOG_ERROR(L"Failed to get IVirtualDesktopManagerInternal: 0x%08X", hr);
        return;
    }

    // VTable method indices: GetCurrentDesktop=6, GetDesktops=7, SwitchDesktop=9
    // Some Windows versions require HMONITOR parameter, others don't

    // Get all desktops array (vtable index 7)
    com_ptr<IObjectArray> pDesktopArray;
    if (usesHMonitor)
    {
        typedef HRESULT(STDMETHODCALLTYPE * GetDesktopsProcWithMonitor)(IVirtualDesktopManagerInternal *, HMONITOR, IObjectArray **);
        GetDesktopsProcWithMonitor GetDesktops = (GetDesktopsProcWithMonitor)(*(void ***)pDesktopManagerInternal.get())[7];
        hr = GetDesktops(pDesktopManagerInternal.get(), nullptr, pDesktopArray.put());
    }
    else
    {
        typedef HRESULT(STDMETHODCALLTYPE * GetDesktopsProc)(IVirtualDesktopManagerInternal *, IObjectArray **);
        GetDesktopsProc GetDesktops = (GetDesktopsProc)(*(void ***)pDesktopManagerInternal.get())[7];
        hr = GetDesktops(pDesktopManagerInternal.get(), pDesktopArray.put());
    }
    if (FAILED(hr))
    {
        LOG_ERROR(L"Failed to get desktops array: 0x%08X", hr);
        return;
    }

    // Get current desktop (vtable index 6)
    com_ptr<IVirtualDesktop> pCurrentDesktop;
    if (usesHMonitor)
    {
        typedef HRESULT(STDMETHODCALLTYPE * GetCurrentDesktopProcWithMonitor)(IVirtualDesktopManagerInternal *, HMONITOR, IVirtualDesktop **);
        GetCurrentDesktopProcWithMonitor GetCurrentDesktop = (GetCurrentDesktopProcWithMonitor)(*(void ***)pDesktopManagerInternal.get())[6];
        hr = GetCurrentDesktop(pDesktopManagerInternal.get(), nullptr, pCurrentDesktop.put());
    }
    else
    {
        typedef HRESULT(STDMETHODCALLTYPE * GetCurrentDesktopProc)(IVirtualDesktopManagerInternal *, IVirtualDesktop **);
        GetCurrentDesktopProc GetCurrentDesktop = (GetCurrentDesktopProc)(*(void ***)pDesktopManagerInternal.get())[6];
        hr = GetCurrentDesktop(pDesktopManagerInternal.get(), pCurrentDesktop.put());
    }
    if (FAILED(hr))
    {
        LOG_ERROR(L"Failed to get current desktop: 0x%08X", hr);
        return;
    }

    // Get desktop count
    UINT desktopCount = 0;
    hr = pDesktopArray->GetCount(&desktopCount);
    if (FAILED(hr) || desktopCount == 0)
    {
        LOG_ERROR(L"Failed to get desktop count or no desktops available: 0x%08X", hr);
        return;
    }

    LOG_DEBUG(L"Found %u virtual desktops", desktopCount);

    // Find current desktop index by comparing pointers
    int currentIndex = -1;
    for (UINT i = 0; i < desktopCount; i++)
    {
        com_ptr<IVirtualDesktop> pDesktop;
        hr = pDesktopArray->GetAt(i, IID_IVirtualDesktop, pDesktop.put_void());
        if (SUCCEEDED(hr) && pDesktop.get() == pCurrentDesktop.get())
        {
            currentIndex = i;
            break;
        }
    }
    if (currentIndex == -1)
    {
        LOG_ERROR(L"Failed to find current desktop index");
        return;
    }
    LOG_DEBUG(L"Current desktop index: %d", currentIndex);

    // Calculate target index with wraparound
    int targetIndex = reverse ? ((currentIndex - 1 + desktopCount) % desktopCount) : ((currentIndex + 1) % desktopCount);
    LOG_DEBUG(L"Target desktop index: %d", targetIndex);

    // Get target desktop
    com_ptr<IVirtualDesktop> pTargetDesktop;
    hr = pDesktopArray->GetAt(targetIndex, IID_IVirtualDesktop, pTargetDesktop.put_void());
    if (FAILED(hr))
    {
        LOG_ERROR(L"Failed to get target desktop: 0x%08X", hr);
        return;
    }

    // Switch to target desktop (vtable index 9)
    if (usesHMonitor)
    {
        typedef HRESULT(STDMETHODCALLTYPE * SwitchDesktopProcWithMonitor)(IVirtualDesktopManagerInternal *, HMONITOR, IVirtualDesktop *);
        SwitchDesktopProcWithMonitor SwitchDesktop = (SwitchDesktopProcWithMonitor)(*(void ***)pDesktopManagerInternal.get())[9];
        hr = SwitchDesktop(pDesktopManagerInternal.get(), nullptr, pTargetDesktop.get());
    }
    else
    {
        typedef HRESULT(STDMETHODCALLTYPE * SwitchDesktopProc)(IVirtualDesktopManagerInternal *, IVirtualDesktop *);
        SwitchDesktopProc SwitchDesktop = (SwitchDesktopProc)(*(void ***)pDesktopManagerInternal.get())[9];
        hr = SwitchDesktop(pDesktopManagerInternal.get(), pTargetDesktop.get());
    }
    if (FAILED(hr))
    {
        LOG_ERROR(L"Failed to switch desktop: 0x%08X", hr);
        return;
    }

    LOG_INFO(L"Successfully switched to %s virtual desktop (index %d)", reverse ? L"previous" : L"next", targetIndex);
}

// Sends Win+Tab keypress
void SendWinTabKeypress()
{
    LOG_TRACE();

    LOG_INFO(L"Sending Win+Tab keypress");
    SendKeypress({VK_LWIN, VK_TAB});
}

// Sends Media Play/Pause keypress
void MediaPlayPause()
{
    LOG_TRACE();

    LOG_INFO(L"Sending Media Play/Pause keypress");
    SendKeypress({VK_MEDIA_PLAY_PAUSE});
}

// Sends Media Next Track keypress
void MediaNext()
{
    LOG_TRACE();

    LOG_INFO(L"Sending Media Next Track keypress");
    SendKeypress({VK_MEDIA_NEXT_TRACK});
}

// Sends Media Previous Track keypress
void MediaPrev()
{
    LOG_TRACE();

    LOG_INFO(L"Sending Media Previous Track keypress");
    SendKeypress({VK_MEDIA_PREV_TRACK});
}

// Clicks Start button using UIAutomation, handling Win10/Win11 differences
bool ClickStartMenu()
{
    LOG_TRACE();

    const MouseClick &lastClick = g_mouseClickQueue[-1];
    if (!lastClick.onEmptySpace)
    {
        LOG_ERROR(L"Failed to send Win keypress - last click was not on empty space");
        return false;
    }

    auto pUIAutomation = g_comAPI.GetUIAutomation();
    if (!pUIAutomation)
    {
        LOG_ERROR(L"Failed to get UIAutomation instance");
        return false;
    }

    // Get the taskbar element from the last mouse click position
    com_ptr<IUIAutomationElement> pWindowElement = NULL;
    if (FAILED(pUIAutomation->ElementFromPoint(lastClick.position, pWindowElement.put())) || !pWindowElement)
    {
        LOG_ERROR(L"Failed to taskbar UI element from mouse click");
        return false;
    }

    com_ptr<IUIAutomationElement> pStartButton = NULL;

    // find the Start button element on the Taskbar so it can be clicked
    if (g_taskbarVersion == WIN_10)
    {
        // *************** Handling Windows 10 Start Button ***************

        // Create Condition 1: ControlType == Button
        com_ptr<IUIAutomationCondition> pControlTypeCondition = NULL;
        if (FAILED(pUIAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId,
                                                          _variant_t(static_cast<int>(UIA_ButtonControlTypeId)),
                                                          pControlTypeCondition.put())) ||
            !pControlTypeCondition)
        {
            LOG_ERROR(L"Failed to create ControlType condition for Start button search.");
            return false;
        }

        // Create Condition 2: ClassName == "Start"
        com_ptr<IUIAutomationCondition> pClassNameCondition = NULL;
        if (FAILED(pUIAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId,
                                                          _variant_t(L"Start"),
                                                          pClassNameCondition.put())) ||
            !pClassNameCondition)
        {
            LOG_ERROR(L"Failed to create ClassName condition for Start button search.");
            return false;
        }

        // Combine both conditions using AndCondition
        com_ptr<IUIAutomationCondition> pAndCondition = NULL;
        if (FAILED(pUIAutomation->CreateAndCondition(pControlTypeCondition.get(),
                                                     pClassNameCondition.get(),
                                                     pAndCondition.put())) ||
            !pAndCondition)
        {
            LOG_ERROR(L"Failed to create ControlType&&ClassName condition for Start button search.");
            return false;
        }

        // Use the combined condition to find the Start button within the Taskbar
        if (FAILED(pWindowElement->FindFirst(TreeScope_Children, pAndCondition.get(), pStartButton.put())) || !pStartButton)
        {
            LOG_ERROR(L"Failed to locate the Start button element for Windows 10 taskbar.");
            return false;
        }
    }
    else
    {
        // *************** Handling Windows 11 Start Button ***************

        // Create a condition to find the Start button by AutomationId
        com_ptr<IUIAutomationCondition> pCondition = NULL;
        if (FAILED(pUIAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId,
                                                          _variant_t(L"StartButton"),
                                                          pCondition.put())) ||
            !pCondition)
        {
            LOG_ERROR(L"Failed to create property condition for locating the Start button.");
            return false;
        }

        // Use the AutomationId condition to find the Start button within the Taskbar
        if (FAILED(pWindowElement->FindFirst(TreeScope_Children, pCondition.get(), pStartButton.put())) || !pStartButton)
        {
            LOG_ERROR(L"Failed to locate the Start button element for Windows 11 taskbar.");
            return false;
        }
    }

    // Perform the appropriate action based on the Taskbar version
    if (g_taskbarVersion == WIN_10)
    {
        // Use Invoke pattern to click the Start button on Windows 10
        com_ptr<IUIAutomationInvokePattern> pInvoke = NULL;
        if (FAILED(pStartButton->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(pInvoke.put()))) || !pInvoke)
        {
            LOG_ERROR(L"Invoke pattern not supported by the Start button.");
            return false;
        }

        if (FAILED(pInvoke->Invoke()))
        {
            LOG_ERROR(L"Failed to invoke the Start button.");
            return false;
        }
        else
        {
            LOG_INFO(L"Start button clicked successfully on Windows 10 taskbar.");
        }
    }
    else
    {
        // Use Toggle pattern to toggle the Start button on Windows 11
        com_ptr<IUIAutomationTogglePattern> pToggle = NULL;
        if (FAILED(pStartButton->GetCurrentPatternAs(UIA_TogglePatternId, IID_PPV_ARGS(pToggle.put()))) || !pToggle)
        {
            LOG_ERROR(L"Toggle pattern not supported by the Start button.");
            return false;
        }

        if (FAILED(pToggle->Toggle()))
        {
            LOG_ERROR(L"Failed to toggle the Start button.");
            return false;
        }
        else
        {
            LOG_INFO(L"Start button toggled successfully on Windows 11 taskbar.");
        }
    }
    return true;
}

// Opens Start menu, falls back to Win key if Start button is hidden
void OpenStartMenu()
{
    LOG_TRACE();

    if (!ClickStartMenu()) // if user hide the start menu via other Windhawk mod, we can't click it
    {
        LOG_INFO(L"Sending Win keypress");
        SendKeypress({VK_LWIN});
    }
}

// Opens Task Manager using ShellExecuteEx
void OpenTaskManager(HWND taskbarhWnd)
{
    LOG_TRACE();

    LOG_INFO(L"Opening Taskmgr.exe using ShellExecuteEx");

    WCHAR szWindowsDirectory[MAX_PATH];
    GetWindowsDirectory(szWindowsDirectory, ARRAYSIZE(szWindowsDirectory));
    std::wstring taskmgrPath = szWindowsDirectory;
    taskmgrPath += L"\\System32\\Taskmgr.exe";

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.lpVerb = L"open"; // Use "runas" to explicitly request elevation
    sei.lpFile = taskmgrPath.c_str();
    sei.nShow = SW_SHOW;

    if (!ShellExecuteEx(&sei))
    {
        DWORD error = GetLastError();
        if (error != ERROR_CANCELLED) // User declined the elevation.
        {
            LOG_ERROR(L"Failed to start process taskmgr.exe with error code: %d", error);
        }
    }
}

// Returns mute state of default audio device
BOOL IsAudioMuted(com_ptr<IMMDeviceEnumerator> pDeviceEnumerator)
{
    LOG_TRACE();

    // GUID of audio enpoint defined in Windows SDK (see Endpointvolume.h) - defined manually to avoid linking the whole lib
    const GUID XIID_IAudioEndpointVolume = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

    BOOL isMuted = FALSE;
    com_ptr<IMMDevice> defaultAudioDevice;
    if (SUCCEEDED(pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, defaultAudioDevice.put())))
    {
        // get handle to the default audio endpoint volume control
        com_ptr<IAudioEndpointVolume> endpointVolume;
        if (SUCCEEDED(defaultAudioDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, endpointVolume.put_void())))
        {
            if (FAILED(endpointVolume->GetMute(&isMuted)))
            {
                LOG_ERROR(L"Failed to volume mute status!");
            }
        }
        else
        {
            LOG_ERROR(L"Failed to get default audio endpoint volume handle!");
        }
    }
    else
    {
        LOG_ERROR(L"Failed to get default audio endpoint!");
    }

    return isMuted;
}

// Toggles mute state for all active audio devices
void ToggleVolMuted()
{
    LOG_TRACE();

    auto pDeviceEnumerator = g_comAPI.GetDeviceEnumerator();
    if (!pDeviceEnumerator)
    {
        LOG_ERROR(L"Failed to toggle volume mute - device enumerator not initialized!");
        return;
    }
    LOG_INFO(L"Toggling volume mute");

    // GUID of audio enpoint defined in Windows SDK (see Endpointvolume.h) - defined manually to avoid linking the whole lib
    const GUID XIID_IAudioEndpointVolume = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

    const BOOL isMuted = IsAudioMuted(pDeviceEnumerator);

    // Get all audio render (playback) devices
    com_ptr<IMMDeviceCollection> pDeviceCollection;
    if (FAILED(pDeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, pDeviceCollection.put())))
    {
        LOG_ERROR(L"Failed to enumerate audio endpoints!");
        return;
    }

    UINT deviceCount = 0;
    if (FAILED(pDeviceCollection->GetCount(&deviceCount)))
    {
        LOG_ERROR(L"Failed to get device count!");
        return;
    }
    LOG_DEBUG(L"Found %u active audio device(s)", deviceCount);

    // Apply the target mute state to all devices
    for (UINT i = 0; i < deviceCount; i++)
    {
        com_ptr<IMMDevice> pDevice;
        if (SUCCEEDED(pDeviceCollection->Item(i, pDevice.put())))
        {
            com_ptr<IAudioEndpointVolume> endpointVolume;
            if (SUCCEEDED(pDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, endpointVolume.put_void())))
            {
                if (FAILED(endpointVolume->SetMute(!isMuted, NULL)))
                {
                    LOG_ERROR(L"Failed to set mute state for device %u!", i);
                }
            }
            else
            {
                LOG_ERROR(L"Failed to get audio endpoint volume handle for device %u!", i);
            }
        }
        else
        {
            LOG_ERROR(L"Failed to get device %u from collection!", i);
        }
    }
}

// Toggles desktop icon visibility
void HideIcons()
{
    LOG_TRACE();

    HWND hDesktopWnd = FindDesktopWindow();
    if (hDesktopWnd != NULL)
    {
        LOG_INFO(L"Sending show/hide icons message");
        if (!PostMessage(hDesktopWnd, WM_COMMAND, 0x7402, 0))
        {
            LOG_ERROR(L"Failed to send show/hide icons message");
        }
    }
    else
    {
        LOG_ERROR(L"Failed to send show/hide icons message - desktop window not found");
    }
}

// Toggles taskbar button combining between two states for primary and secondary taskbars (Win11 only)
void CombineTaskbarButtons(const TaskBarButtonsState primaryTaskBarButtonsState1, const TaskBarButtonsState primaryTaskBarButtonsState2,
                           const TaskBarButtonsState secondaryTaskBarButtonsState1, const TaskBarButtonsState secondaryTaskBarButtonsState2)
{
    LOG_TRACE();

    if (g_taskbarVersion != WIN_11)
    {
        LOG_INFO(L"Taskbar button combining is only supported on Windows 11 taskbar");
        return;
    }

    bool shallNotify = false;
    if ((primaryTaskBarButtonsState1 != COMBINE_INVALID) && (primaryTaskBarButtonsState2 != COMBINE_INVALID))
    {
        // get the initial state so that first click actually toggles to the other state (avoid switching to a state that is already set)
        static bool zigzagPrimary = (GetCombineTaskbarButtons(L"TaskbarGlomLevel") == primaryTaskBarButtonsState1);
        zigzagPrimary = !zigzagPrimary;
        shallNotify |= SetCombineTaskbarButtons(L"TaskbarGlomLevel",
                                                zigzagPrimary ? primaryTaskBarButtonsState1 : primaryTaskBarButtonsState2);
    }
    if ((secondaryTaskBarButtonsState1 != COMBINE_INVALID) && (secondaryTaskBarButtonsState2 != COMBINE_INVALID))
    {
        // get the initial state so that first click actually toggles to the other state (avoid switching to a state that is already set)
        static bool zigzagSecondary = (GetCombineTaskbarButtons(L"MMTaskbarGlomLevel") == secondaryTaskBarButtonsState1);
        zigzagSecondary = !zigzagSecondary;
        shallNotify |= SetCombineTaskbarButtons(L"MMTaskbarGlomLevel",
                                                zigzagSecondary ? secondaryTaskBarButtonsState1 : secondaryTaskBarButtonsState2);
    }
    if (shallNotify)
    {
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)TEXT("TraySettings"), SMTO_ABORTIFHUNG, 100, NULL);
    }
}

// Returns current taskbar button combine state from registry (0=Always, 1=WhenFull, 2=Never)
DWORD GetCombineTaskbarButtons(const wchar_t *optionName)
{
    LOG_TRACE();

    HKEY hKey = NULL;
    DWORD dwValue = 0;
    DWORD dwBufferSize = sizeof(DWORD);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
                     0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hKey, optionName, NULL, NULL, (LPBYTE)&dwValue, &dwBufferSize) != ERROR_SUCCESS)
        {
            LOG_ERROR(L"Failed to read registry key %s!", optionName);
        }
        RegCloseKey(hKey);
    }
    else
    {
        LOG_ERROR(L"Failed to open registry path Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced!");
    }
    return dwValue;
}

// Sets taskbar button combine state in registry (0=Never, 1=WhenFull, 2=Always)
bool SetCombineTaskbarButtons(const wchar_t *optionName, unsigned int option)
{
    LOG_TRACE();

    bool shallNotify = false;
    if (option <= 2)
    {
        LOG_INFO(L"Setting taskbar button combining to %d", option);
        HKEY hKey = NULL;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
                         0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
        {
            DWORD dwValue = option;
            if (RegSetValueEx(hKey, optionName, 0, REG_DWORD, (BYTE *)&dwValue, sizeof(dwValue)) == ERROR_SUCCESS)
            {
                // Notify all applications of the change
                shallNotify = true;
            }
            else
            {
                LOG_ERROR(L"Failed to set registry key %s!", optionName);
            }
            RegCloseKey(hKey);
        }
        else
        {
            LOG_ERROR(L"Failed to open registry path Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced!");
        }
    }
    else
    {
        LOG_ERROR(L"Invalid option for combining taskbar buttons!");
    }
    return shallNotify;
}

// Returns taskbar alignment from registry (0=Left, 1=Center), defaults to 1
DWORD GetTaskbarAlignment()
{
    LOG_TRACE();

    HKEY hKey = NULL;
    DWORD dwValue = 1; // Default to center alignment if key doesn't exist
    DWORD dwBufferSize = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
                     0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hKey, TEXT("TaskbarAl"), NULL, NULL, (LPBYTE)&dwValue, &dwBufferSize) != ERROR_SUCCESS)
        {
            LOG_INFO(L"TaskbarAl registry key not found, using default value (1 = Center)");
            dwValue = 1; // Default to center
        }
        RegCloseKey(hKey);
    }
    else
    {
        LOG_ERROR(L"Failed to open registry path Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced!");
    }

    return dwValue;
}

// Sets taskbar alignment in registry (0=Left, 1=Center)
bool SetTaskbarAlignment(DWORD alignment)
{
    LOG_TRACE();

    HKEY hKey = NULL;
    bool success = false;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
                     0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        DWORD dwValue = alignment;
        if (RegSetValueEx(hKey, TEXT("TaskbarAl"), 0, REG_DWORD, (BYTE *)&dwValue, sizeof(dwValue)) == ERROR_SUCCESS)
        {
            LOG_INFO(L"Set taskbar alignment to %d", alignment);
            success = true;
        }
        else
        {
            LOG_ERROR(L"Failed to set registry key TaskbarAl!");
        }
        RegCloseKey(hKey);
    }
    else
    {
        LOG_ERROR(L"Failed to open registry path Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced!");
    }

    return success;
}

// Toggles taskbar alignment between left and center (Win11 only)
void ToggleTaskbarAlignment()
{
    LOG_TRACE();

    if (g_taskbarVersion != WIN_11)
    {
        LOG_INFO(L"Taskbar alignment toggle is only supported on Windows 11 taskbar");
        return;
    }

    DWORD currentAlignment = GetTaskbarAlignment();
    DWORD newAlignment = 0;
    if (currentAlignment == 0)
    {
        newAlignment = 1;
    }
    else if (currentAlignment == 1)
    {
        newAlignment = 0;
    }
    else
    {
        LOG_ERROR(L"Invalid current alignment value %d (must be 0 or 1)", currentAlignment); // report in case API changes in future
        return;
    }
    const wchar_t *alignmentStrs[] = {L"Left", L"Center"};
    LOG_INFO(L"Toggling taskbar alignment from '%s' to '%s'", alignmentStrs[currentAlignment], alignmentStrs[newAlignment]);

    if (SetTaskbarAlignment(newAlignment))
    {
        // Notify all applications of the change
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)TEXT("TraySettings"), SMTO_ABORTIFHUNG, 100, NULL);
    }
}

// Parses command line into executable path and parameters
std::tuple<std::wstring, std::wstring> ParseExecutableAndParameters(const std::wstring &command)
{
    LOG_TRACE();

    std::wstring executable = command;
    std::wstring parameters;

    // in case user provided quoted executable path
    if (stringtools::startsWith(command, L"\"") || stringtools::startsWith(command, L"'"))
    {
        // Find the closing quote
        size_t closingQuotePos = command.find(command[0], 1);
        if (closingQuotePos != std::wstring::npos)
        {
            executable = command.substr(1, closingQuotePos - 1);
            if (command.length() > closingQuotePos + 1)
            {
                parameters = command.substr(closingQuotePos + 1);
            }
        }
        else
        {
            LOG_ERROR(L"Failed to parse executable and parameters - missing closing quote in command");
        }
    }
    else
    {
        // split by space and try to put together executable path that may contain spaces
        std::vector<std::wstring> args = SplitArgs(command, L' ');
        if (args.size() > 1)
        {
            executable = L"";
            for (const auto &arg : args)
            {
                executable += arg;
                if (std::filesystem::path(executable).extension().wstring().length() > 1) // is ext more than just dot ?
                {
                    break;
                }
                else
                {
                    executable += L" ";
                }
            }
            if (command.length() > executable.length())
            {
                parameters = command.substr(executable.length());
            }
        }
    }
    return std::make_tuple(stringtools::trim(executable), stringtools::trim(parameters));
}

// Starts a process with command line arguments
void StartProcess(std::wstring command)
{
    LOG_TRACE();

    if (command.empty())
    {
        LOG_DEBUG(L"Command is empty, nothing to start");
        return;
    }

    // check for UAC prefix
    std::vector<std::wstring> uac_args = SplitArgs(command, L';');
    std::wstring shellExVerb = L"open";
    if (stringtools::toLower(uac_args[0]) == L"uac")
    {
        shellExVerb = L"runas";                                                 // request elevation
        command = stringtools::ltrim(command.substr(uac_args[0].length() + 1)); // remove the "uac;" prefix
    }

    const auto [executable, parameters] = ParseExecutableAndParameters(command);
    LOG_INFO(L"Starting process with %s: '%s' '%s'", shellExVerb.c_str(), executable.c_str(), parameters.c_str());

    // Get current cursor position and monitor handle
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    LOG_DEBUG(L"Launching process on monitor handle: 0x%08X", (DWORD)(ULONG_PTR)hMonitor);

    // Use ShellExecuteEx with hMonitor to launch on the correct monitor
    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_HMONITOR | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = shellExVerb.c_str();
    sei.lpFile = executable.c_str();
    sei.lpParameters = parameters.empty() ? NULL : parameters.c_str();
    sei.nShow = SW_SHOWNORMAL;
    sei.hMonitor = hMonitor; // Specify the target monitor, but most apps ignore this anyway

    if (!ShellExecuteEx(&sei))
    {
        DWORD error = GetLastError();
        if (error != ERROR_CANCELLED) // User cancelled UAC or similar
        {
            LPWSTR errorMsg = nullptr;
            FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPWSTR)&errorMsg, 0, NULL);
            if (errorMsg)
            {
                LOG_ERROR(L"Failed to start process - ShellExecuteEx failed with error code %d: %s", error, errorMsg);
                LocalFree(errorMsg);
            }
            else
            {
                LOG_ERROR(L"Failed to start process - ShellExecuteEx failed with error code: %d", error);
            }
        }
        else
        {
            LOG_DEBUG(L"Process launch cancelled by user");
        }
    }
    else
    {
        LOG_DEBUG(L"Process launched successfully on target monitor");
    }
}

#pragma endregion // actions

// =====================================================================

#pragma region trigger_handling

// Checks if window matches desired taskbar type (primary/secondary/all)
bool IsCorrectTaskbarType(const std::wstring &taskbarTypeName, HWND hWnd)
{
    LOG_TRACE();

    bool isCorrectTaskbar = true;
    if (taskbarTypeName == L"primary")
    {
        isCorrectTaskbar = (hWnd == g_hTaskbarWnd);
    }
    else if (taskbarTypeName == L"secondary")
    {
        isCorrectTaskbar = g_secondaryTaskbarWindows.contains(hWnd);
    }
    return isCorrectTaskbar;
}

// Determines if context menu should be suppressed for right-click triggers
bool ShallSuppressContextMenu(const MouseClick &lastClick)
{
    LOG_TRACE();

    for (const auto &triggerAction : g_settings.triggerActions)
    {
        if (triggerAction.mouseTriggerName.find(L"right", 0) == 0)
        {
            // we want to suppress only if user "is going for the trigger"
            if (lastClick.keyModifiersState == triggerAction.expectedKeyModifiersState)
            {
                LOG_DEBUG("Suppressing right click to suppress context menu");
                g_contextMenuSuppressionTimestamp = GetTickCount();
                return true;
            }
        }
    }
    return false;
}

bool IsSingleClick(const MouseClick::Button button)
{
    LOG_TRACE();

    const MouseClick &currentClick = g_mouseClickQueue[-1];
    return (currentClick.type == MouseClick::Type::MOUSE) && (currentClick.button == button);
}

// Checks if two clicks form a double-click based on timing and distance
bool IsDoubleClick(const MouseClick::Button button, const MouseClick &previousClick, const MouseClick &currentClick)
{
    LOG_TRACE();

    if (previousClick.type != MouseClick::Type::MOUSE ||
        currentClick.type != MouseClick::Type::MOUSE ||
        previousClick.button != button ||
        currentClick.button != button)
    {
        return false;
    }

    if (previousClick.hWnd != currentClick.hWnd)
    {
        return false;
    }

    UINT dpi = GetDpiForWindow(currentClick.hWnd);

    // Check if the current event is within the double-click time and distance
    bool result = abs(previousClick.position.x - currentClick.position.x) <= MulDiv(GetSystemMetrics(SM_CXDOUBLECLK), dpi, 96) &&
                  abs(previousClick.position.y - currentClick.position.y) <= MulDiv(GetSystemMetrics(SM_CYDOUBLECLK), dpi, 96) &&
                  ((currentClick.timestamp - previousClick.timestamp) <= GetDoubleClickTime());
    return result;
}

bool IsDoubleClick(const MouseClick::Button button)
{
    LOG_TRACE();

    return IsDoubleClick(button, g_mouseClickQueue[-2], g_mouseClickQueue[-1]);
}

// Checks if last 3 clicks form a triple-click
bool IsTripleClick(const MouseClick::Button button)
{
    LOG_TRACE();

    return IsDoubleClick(button, g_mouseClickQueue[-2], g_mouseClickQueue[-1]) &&
           IsDoubleClick(button, g_mouseClickQueue[-3], g_mouseClickQueue[-2]);
}

// Checks if last 4 clicks form a multi-click (to ignore quad-clicks)
bool IsMultiClick(const MouseClick::Button button)
{
    LOG_TRACE();

    return IsDoubleClick(button, g_mouseClickQueue[-2], g_mouseClickQueue[-1]) &&
           IsDoubleClick(button, g_mouseClickQueue[-3], g_mouseClickQueue[-2]) &&
           IsDoubleClick(button, g_mouseClickQueue[-4], g_mouseClickQueue[-3]);
}

bool IsSingleTap()
{
    LOG_TRACE();

    const MouseClick &currentClick = g_mouseClickQueue[-1];
    return (currentClick.type == MouseClick::Type::TOUCH) && (currentClick.button == MouseClick::Button::LEFT);
}

// Checks if two touch taps form a double-tap based on timing and distance
bool IsDoubleTap(const MouseClick &previousClick, const MouseClick &currentClick)
{
    LOG_TRACE();

    if (previousClick.type != MouseClick::Type::TOUCH || currentClick.type != MouseClick::Type::TOUCH)
    {
        return false;
    }

    if (previousClick.hWnd != currentClick.hWnd)
    {
        return false;
    }

    HWND hWnd = currentClick.hWnd;
    UINT dpi = GetDpiForWindow(hWnd);

    // GetSystemMetrics(SM_CXDOUBLECLK) is suitable just for mouse, not really for touch
    const int MAX_POS_OFFSET_PX = 15;
    // if user has hi-res screen, every slight movement result in big pixel offset
    const int MAX_POS_OFFSET_PX_SCALED = MulDiv(MAX_POS_OFFSET_PX, dpi, 96);

    // Check if the current event is within the double-click time and distance
    bool result = abs(previousClick.position.x - currentClick.position.x) <= MAX_POS_OFFSET_PX_SCALED &&
                  abs(previousClick.position.y - currentClick.position.y) <= MAX_POS_OFFSET_PX_SCALED &&
                  ((currentClick.timestamp - previousClick.timestamp) <= (GetDoubleClickTime()));
    return result;
}

bool IsDoubleTap()
{
    LOG_TRACE();

    const MouseClick &previousClick = g_mouseClickQueue[-2];
    const MouseClick &currentClick = g_mouseClickQueue[-1];
    return IsDoubleTap(previousClick, currentClick);
}

// Checks if last 3 taps form a triple-tap
bool IsTripleTap()
{
    LOG_TRACE();

    return IsDoubleTap(g_mouseClickQueue[-2], g_mouseClickQueue[-1]) && IsDoubleTap(g_mouseClickQueue[-3], g_mouseClickQueue[-2]);
}

// Checks if last 4 taps form a multi-tap (to ignore quad-taps)
bool IsMultiTap()
{
    LOG_TRACE();

    return IsDoubleTap(g_mouseClickQueue[-2], g_mouseClickQueue[-1]) &&
           IsDoubleTap(g_mouseClickQueue[-3], g_mouseClickQueue[-2]) &&
           IsDoubleTap(g_mouseClickQueue[-4], g_mouseClickQueue[-3]);
}

// Injects a synthetic right-click at specified screen position
void SynthesizeTaskbarRightClick(const POINT &ptScreen)
{
    LOG_TRACE();

    LOG_DEBUG(L"Synthesizing right-click at %ld,%ld", ptScreen.x, ptScreen.y);

    SetCursorPos(ptScreen.x, ptScreen.y); // likely not necessary, but just to be sure

    INPUT input[2] = {};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    input[0].mi.dwExtraInfo = g_injectedClickID; // to identify our synthesized click

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    input[1].mi.dwExtraInfo = g_injectedClickID;

    if (SendInput(2, input, sizeof(INPUT)) != 2)
    {
        LOG_ERROR(L"SendInput failed when synthesizing right click");
    }
}

// Timer callback that executes action after double-click timeout expires
void CALLBACK ProcessDelayedMouseClick(HWND, UINT, UINT_PTR, DWORD)
{
    LOG_TRACE();

    if (!KillTimer(NULL, gMouseClickTimer))
    {
        LOG_ERROR(L"Failed to kill triple click timer");
    }
    gMouseClickTimer = 0;

    const POINT lastMousePos = g_mouseClickQueue[-1].position; // store since the queue will be cleared

    bool wasActionExecuted = false;
    const auto actionExecutor = GetTaskbarActionExecutor(false /* do not check for existing higher-click-count triggers */);
    if (actionExecutor)
    {
        wasActionExecuted = actionExecutor(); // execute taskbar action
    }

    if (g_keepTaskSwitchingOpened && !wasActionExecuted) // close Ctrl+Alt+Tab dialog
    {
        CloseCtrlAltTabDialog();
    }

    if (g_isContextMenuSuppressed && !wasActionExecuted) // action not executed, so we need to synthesize right-click to show context menu
    {
        SynthesizeTaskbarRightClick(lastMousePos);
    }
    g_isContextMenuSuppressed = false;
}

// Constructs trigger name string from click type, count and button
std::wstring GetActionName(const MouseClick::Type clickType, const uint32_t numClicks, const MouseClick::Button button)
{
    LOG_TRACE();

    std::wstring mouseTriggerName;
    if (clickType == MouseClick::Type::MOUSE)
    {
        if (button == MouseClick::Button::LEFT)
        {
            mouseTriggerName = L"left";
        }
        else if (button == MouseClick::Button::RIGHT)
        {
            mouseTriggerName = L"right";
        }
        else if (button == MouseClick::Button::MIDDLE)
        {
            mouseTriggerName = L"middle";
        }
        else if (button == MouseClick::Button::MOUSE4)
        {
            mouseTriggerName = L"mouse4";
        }
        else if (button == MouseClick::Button::MOUSE5)
        {
            mouseTriggerName = L"mouse5";
        }
        if (numClicks == 3)
        {
            mouseTriggerName += L"Triple";
        }
        else if (numClicks == 2)
        {
            mouseTriggerName += L"Double";
        }
    }
    else if (clickType == MouseClick::Type::TOUCH)
    {
        mouseTriggerName = L"tap";
        if (numClicks == 3)
        {
            mouseTriggerName += L"Triple";
        }
        else if (numClicks == 2)
        {
            mouseTriggerName += L"Double";
        }
        else if (numClicks == 1)
        {
            mouseTriggerName += L"Single";
        }
    }
    return mouseTriggerName;
}

// Checks if a trigger with matching name and modifiers is defined in settings
bool IsTriggerDefined(const std::wstring &mouseTriggerName, const int numClicks)
{
    LOG_TRACE();

    for (const auto &triggerAction : g_settings.triggerActions)
    {
        if ((triggerAction.mouseTriggerName == mouseTriggerName) && triggerAction.actionExecutor)
        {
            bool allModifiersPressed = true;
            bool isCorrectTaskbar = true;
            for (int i = 1; i <= numClicks; i++)
            {
                allModifiersPressed &= (g_mouseClickQueue[-i].keyModifiersState == triggerAction.expectedKeyModifiersState);
                isCorrectTaskbar &= IsCorrectTaskbarType(triggerAction.taskbarTypeName, g_mouseClickQueue[-i].hWnd);
            }
            return allModifiersPressed && isCorrectTaskbar;
        }
    }
    return false;
}

// Finds and executes action matching trigger name and current modifier state
bool ExecuteTaskbarAction(const std::wstring &mouseTriggerName, const uint32_t numClicks)
{
    LOG_TRACE();

    bool wasActionExecuted = false;
    HWND hWnd = g_mouseClickQueue[-1].hWnd;

    LOG_DEBUG(L"Searching for action for trigger: %s", mouseTriggerName.c_str());
    for (const auto &triggerAction : g_settings.triggerActions)
    {
        if (triggerAction.mouseTriggerName == mouseTriggerName)
        {
            LOG_DEBUG(L"Found action: %s", triggerAction.actionName.c_str());
            bool allModifiersPressed = true;
            bool isCorrectTaskbar = true;
            for (int i = 1; i <= numClicks; i++)
            {
                allModifiersPressed &= (g_mouseClickQueue[-i].keyModifiersState == triggerAction.expectedKeyModifiersState);
                isCorrectTaskbar &= IsCorrectTaskbarType(triggerAction.taskbarTypeName, g_mouseClickQueue[-i].hWnd);
                LOG_DEBUG(L"Click #%d - key modifiers state: %u, expected: %u, taskbar type: %s",
                          i, g_mouseClickQueue[-i].keyModifiersState, triggerAction.expectedKeyModifiersState, triggerAction.taskbarTypeName.c_str());
            }
            if (allModifiersPressed && isCorrectTaskbar)
            {
                if (triggerAction.actionExecutor)
                {
                    LOG_INFO(L"Executing action: %s", triggerAction.actionName.c_str());

                    if (g_keepTaskSwitchingOpened && (triggerAction.actionName != L"ACTION_ALT_TAB"))
                    {
                        CloseCtrlAltTabDialog();
                    }

                    triggerAction.actionExecutor(hWnd);
                    g_mouseClickQueue.clear();
                    wasActionExecuted = true;
                }
                else
                {
                    LOG_ERROR(L"Action executor is not set for action: %s", triggerAction.actionName.c_str());
                }
            }
            else
            {
                LOG_DEBUG(L"Not all modifiers are pressed for action: %s", triggerAction.actionName.c_str());
            }
        }
    }
    return wasActionExecuted;
}

// Returns action executor for detected click pattern, checking for higher-order triggers if requested
std::function<bool()> GetTaskbarActionExecutor(const bool checkForHigherOrderClicks)
{
    LOG_TRACE();

    const auto isHigherOrderClickDefined = [&](const MouseClick::Type clickType, const int currentCount, const MouseClick::Button button)
    {
        for (int higherCount = currentCount + 1; higherCount <= 3; higherCount++)
        {
            if (IsTriggerDefined(GetActionName(clickType, higherCount, button), higherCount))
            {
                LOG_DEBUG(L"Higher order click action defined for %s, skipping lower order click",
                          GetActionName(MouseClick::Type::MOUSE, higherCount, button).c_str());
                return true; // higher order click action defined, skip this one
            }
        }
        return false;
    };

    // mouse clicks
    const MouseClick::Button mouseButtons[] = {MouseClick::Button::LEFT, MouseClick::Button::RIGHT, MouseClick::Button::MIDDLE,
                                               MouseClick::Button::MOUSE4, MouseClick::Button::MOUSE5};
    const std::function<bool(MouseClick::Button)> mouseChecks[] = {IsTripleClick, static_cast<bool (*)(MouseClick::Button)>(IsDoubleClick), IsSingleClick};
    for (const auto &button : mouseButtons)
    {
        if (IsMultiClick(button))
        {
            return nullptr; // ignore quadruple and more clicks
        }

        int clickCount = 3;
        for (const auto &checkFunc : mouseChecks)
        {
            if (checkFunc(button)) // is there a trigger matching the click pattern ?
            {
                if (checkForHigherOrderClicks && isHigherOrderClickDefined(MouseClick::Type::MOUSE, clickCount, button))
                {
                    return nullptr; // current click count match a trigger, but trigger with the higher click count is defined so skip this one
                }
                return [button, clickCount]()
                {
                    return ExecuteTaskbarAction(GetActionName(MouseClick::Type::MOUSE, clickCount, button), clickCount);
                };
            }
            clickCount--;
        }
    }

    // touch taps
    if (IsMultiTap())
    {
        return nullptr; // ignore quadruple and more taps
    }
    int clickCount = 3;
    const std::function<bool()> touchChecks[] = {IsTripleTap, static_cast<bool (*)()>(IsDoubleTap), IsSingleTap};
    for (const auto &checkFunc : touchChecks)
    {
        if (checkFunc())
        {
            if (checkForHigherOrderClicks && isHigherOrderClickDefined(MouseClick::Type::TOUCH, clickCount, MouseClick::Button::INVALID))
            {
                return nullptr; // current click count match a trigger, but trigger with the higher click count is defined so skip this one
            }
            return [clickCount]()
            {
                return ExecuteTaskbarAction(GetActionName(MouseClick::Type::TOUCH, clickCount), clickCount);
            };
        }
        clickCount--;
    }
    return nullptr;
}

#pragma endregion // trigger_handling

// =====================================================================

#pragma region proc_handlers

// Proc handler for older Windows (nonXAML taskbar) versions and ExplorerPatcher
LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam))
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc);
    }

    if (WM_NCDESTROY == uMsg)
    {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        if (hWnd != g_hTaskbarWnd)
        {
            g_secondaryTaskbarWindows.erase(hWnd);
        }
        return result;
    }

    if (uMsg == g_uninitCOMAPIMsg)
    {
        LOG_INFO("Received uninit COM API message, uninitializing COM API");
        g_comAPI.Uninit();
        return 0;
    }

    // on Windows 10 Task Switching window is opened only when invoked, so we need to subclass it lazily here
    if (g_keepTaskSwitchingOpened && (!g_hTaskSwitchingWnd || !IsWindow(g_hTaskSwitchingWnd) || !g_isTaskSwitchingWindowSubclassed))
    {
        LOG_DEBUG(L"Trying to find and subclass Task Switching window");
        SubclassTaskSwitchingWindow();
    }

    const auto button = MouseClick::GetMouseButtonWin10(uMsg, wParam);
    if ((g_taskbarVersion == WIN_10) && (button != MouseClick::Button::INVALID))
    {
        const LPARAM messageExtraInfo = GetMessageExtraInfo();
        const LPARAM dwExtraInfo = messageExtraInfo & 0xFFFFFFFFu;
        if (dwExtraInfo != g_injectedClickID)
        {
            // do lazy init, since doing Init during Wh_ModInit breaks Spotify's global (media) shortcuts
            if (!g_comAPI.IsInitialized())
            {
                g_comAPI.Init(); // make sure it gets initialied from GUI thread
            }

            const auto lastClick = MouseClick(MouseClick::GetMouseClickPositionWin10(), MouseClick::GetPointerTypeWin10(messageExtraInfo), button, hWnd);
            if (lastClick.onEmptySpace && (lastClick.button == MouseClick::Button::RIGHT) &&
                ShallSuppressContextMenu(lastClick)) // avoid opening right click menu when performing a right click action
            {
                g_isContextMenuSuppressed = true;
            }

            OnMouseClick(lastClick);
        }
        else
        {
            LOG_DEBUG("Recognized synthesized right click via extra info tag, skipping, 0x%x", uMsg);
        }
    }

    else if ((WM_NCRBUTTONUP == uMsg) ||                // WM_NCRBUTTONUP for Win10
             (WM_CONTEXTMENU == uMsg) ||                // WM_CONTEXTMENU for ExplorerPatcher Win10 menu
             (uMsg == g_explorerPatcherContextMenuMsg)) // g_explorerPatcherContextMenuMsg for ExplorerPatcher Win11 menu
    {
        const auto lastClick = MouseClick(MouseClick::GetMouseClickPositionWin10(), MouseClick::Type::MOUSE, MouseClick::Button::RIGHT, hWnd);
        const LPARAM extraInfo = GetMessageExtraInfo() & 0xFFFFFFFFu;
        const bool isSuppressionStillValid = (GetTickCount() - g_contextMenuSuppressionTimestamp) <= 1000; // reset context menu suppression after 1 second
        if (isSuppressionStillValid && g_isContextMenuSuppressed && (extraInfo != g_injectedClickID) && lastClick.onEmptySpace &&
            (lastClick.button == MouseClick::Button::RIGHT) && ShallSuppressContextMenu(lastClick))
        {
            return 0; // suppress the right click menu (otherwise a double click would be impossible)
        }
        else
        {
            g_isContextMenuSuppressed = false;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Proc handler for newer Windows versions (Windows 11 21H2 and newer) and ExplorerPatcher (Win11 menu)
// Proc handler is shared between Taskbar, TaskSwitching and possibly other windows
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == g_uninitCOMAPIMsg)
    {
        LOG_INFO("Received uninit COM API message, uninitializing COM API");
        g_comAPI.Uninit();
        return 0;
    }

    // Task switching window is created on demand and the only way to really identify it is by checking not just class name (generic), but
    // windows text as well - however the text is not present during window creation (window create hooks above), so we need to find it lazily here
    // even though the window stays open, when user logs in, the window doesn't exist yet
    if (g_keepTaskSwitchingOpened &&
        (!g_hTaskSwitchingWnd || !IsWindow(g_hTaskSwitchingWnd) || !g_isTaskSwitchingWindowSubclassed) &&
        (g_dwTaskbarThreadId == GetWindowThreadProcessId(hWnd, NULL)))
    {
        LOG_DEBUG(L"Trying to find and subclass Task Switching window");
        SubclassTaskSwitchingWindow();
    }

    // if user clicked on Task Switching window
    const HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (g_keepTaskSwitchingOpened && g_hTaskSwitchingWnd && (uMsg == WM_POINTERDOWN) && (hRootWnd == g_hTaskSwitchingWnd) &&
        IsWindow(g_hTaskSwitchingWnd) && IsWindowVisible(g_hTaskSwitchingWnd))
    {
        if (HandleTaskSwitchingWindowClick(MouseClick::GetMouseClickPositionWin11(lParam)))
        {
            return 0; // suppress the message
        }
    }

    // if user clicked on taskbar window
    if ((uMsg == WM_POINTERDOWN) && IsTaskbarWindow(hRootWnd))
    {
        // do lazy init, since doing Init during Wh_ModInit breaks Spotify's global (media) shortcuts
        if (!g_comAPI.IsInitialized())
        {
            g_comAPI.Init(); // make sure it gets initialied from GUI thread
        }

        // check whether we need to suppress the context menu for right clicks
        const auto lastClick = MouseClick(MouseClick::GetMouseClickPositionWin11(lParam), MouseClick::GetPointerTypeWin11(wParam), MouseClick::GetMouseButtonWin11(wParam), hRootWnd);
        if (lastClick.onEmptySpace && (lastClick.button == MouseClick::Button::RIGHT))
        {
            LPARAM dwExtraInfo = GetMessageExtraInfo() & 0xFFFFFFFFu;
            if (dwExtraInfo == g_injectedClickID)
            {
                LOG_DEBUG("Recognized synthesized right click via extra info tag, skipping");
                return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
            }
            if (ShallSuppressContextMenu(lastClick))
            {
                g_isContextMenuSuppressed = true;
                OnMouseClick(lastClick);
                return 0; // suppress the message // suppress the right click menu (otherwise a double click would be impossible)
            }
        }
        OnMouseClick(lastClick);
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TaskSwitchingWindowSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData)
{
    if ((uMsg == WM_NCDESTROY) && (hWnd == g_hTaskSwitchingWnd))
    {
        LOG_DEBUG("WM_NCDESTROY received, removing subclass");
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskSwitchingWindowSubclassProc);
        g_hTaskSwitchingWnd = NULL;
        g_isTaskSwitchingWindowSubclassed = false;
        g_keepTaskSwitchingOpened = false;
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Task Switching gets closed via these messages, so suppress them to keep it open
    if ((uMsg == WM_ACTIVATEAPP || uMsg == WM_ACTIVATE || uMsg == WM_SHOWWINDOW || uMsg == WM_NCACTIVATE) &&
        g_keepTaskSwitchingOpened && IsWindowVisible(hWnd))
    {
        LOG_DEBUG("Suppressing WM_NCACTIVATE, WM_ACTIVATEAPP, WM_ACTIVATE and WM_SHOWWINDOW to keep Task Switching window open");
        return 0;
    }

    const bool isMouseButtonDown = (uMsg == WM_LBUTTONDOWN || uMsg == WM_NCLBUTTONDOWN ||
                                    uMsg == WM_RBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN ||
                                    uMsg == WM_MBUTTONDOWN || uMsg == WM_NCMBUTTONDOWN ||
                                    uMsg == WM_XBUTTONDOWN || uMsg == WM_NCXBUTTONDOWN);

    // handle the click if user clicked on opened Task Switching window
    if (g_hTaskSwitchingWnd && isMouseButtonDown)
    {
        if (HandleTaskSwitchingWindowClick(MouseClick::GetMouseClickPositionWin10()))
        {
            return 0; // suppress the click
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Processes mouse clicks, starts timer for multi-click detection, called from taskbar window proc handlers
bool OnMouseClick(const MouseClick &click)
{
    LOG_TRACE();

    if ((GetCapture() != NULL) || !click.onEmptySpace)
    {
        return false;
    }

    // if there is already a timer running, kill it (one click becomes double click, etc.)
    if (gMouseClickTimer != NULL)
    {
        if (!KillTimer(NULL, gMouseClickTimer))
        {
            LOG_ERROR(L"Failed to kill triple click timer");
        }
        gMouseClickTimer = NULL;
    }

    g_mouseClickQueue.push_back(click);
    if (g_settings.eagerTriggerEvaluation)
    {
        const auto actionExecutor = GetTaskbarActionExecutor(true /* check for existing higher-click-count triggers */);
        if (actionExecutor)
        {
            LOG_DEBUG(L"Eagerly executing taskbar action for current click sequence");
            bool wasActionExecuted = actionExecutor(); // execute taskbar action

            if (g_keepTaskSwitchingOpened && !wasActionExecuted) // close Ctrl+Alt+Tab dialog
            {
                CloseCtrlAltTabDialog();
            }
        }
        else
        {
            // start timer to wait for possible next click
            gMouseClickTimer = SetTimer(NULL, 0, GetDoubleClickTime(), ProcessDelayedMouseClick);
        }
    }
    else
    {
        // start timer to wait for possible next click
        gMouseClickTimer = SetTimer(NULL, 0, GetDoubleClickTime(), ProcessDelayedMouseClick);
    }
    return true;
}

#pragma endregion // proc_handlers

// =====================================================================

#pragma region windhawk_mod_functions

BOOL Wh_ModInit()
{
    LOG_TRACE();

#ifdef ENABLE_FILE_LOGGER
    g_fileLogger.printStatus();
#endif

    LoadSettings();

    if (!LoadExplorerVersion() || (g_taskbarVersion == UNKNOWN))
    {
        LOG_ERROR(L"Unsupported Windows version, ModInit failed");
        return FALSE;
    }
    // treat Windows 11 taskbar as on older windows
    if ((g_taskbarVersion == WIN_11) && g_settings.oldTaskbarOnWin11)
    {
        g_taskbarVersion = WIN_10;
    }
    LOG_INFO(L"Using taskbar version: %s", WindowsVersionNames[g_taskbarVersion]);

    // hook CreateWindowExW to be able to identify taskbar windows on re-creation
    if (!Wh_SetFunctionHook((void *)CreateWindowExW, (void *)CreateWindowExW_Hook, (void **)&CreateWindowExW_Original))
    {
        LOG_ERROR(L"Failed to hook CreateWindowExW, ModInit failed");
        return FALSE;
    }
    // hook CreateWindowInBand to be able to identify taskbar windows on re-creation
    HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (!user32Module)
    {
        LOG_ERROR(L"Failed to load user32.dll, ModInit failed");
        return FALSE;
    }
    void *pCreateWindowInBand = (void *)GetProcAddress(user32Module, "CreateWindowInBand");
    if (!pCreateWindowInBand)
    {
        LOG_ERROR(L"Failed to get CreateWindowInBand address, ModInit failed");
        return FALSE;
    }
    if (!Wh_SetFunctionHook(pCreateWindowInBand, (void *)CreateWindowInBand_Hook, (void **)&CreateWindowInBand_Original))
    {
        LOG_ERROR(L"Failed to hook CreateWindowInBand, ModInit failed");
        return FALSE;
    }
    pGetWindowBand = (GetWindowBand_t)GetProcAddress(user32Module, "GetWindowBand");
    if (!pGetWindowBand)
    {
        LOG_ERROR(L"Failed to get GetWindowBand address");
    }

    // autodetect Explorer Patcher on Windows 11 taskbar
    if (g_taskbarVersion == WIN_11)
    {
        // hook LoadLibraryExW to detect Explorer Patcher loading after us
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (void *)GetProcAddress(kernelBaseModule, "LoadLibraryExW");
        if (!Wh_SetFunctionHook(pKernelBaseLoadLibraryExW, (void *)LoadLibraryExW_Hook, (void **)&LoadLibraryExW_Original))
        {
            LOG_ERROR(L"Failed to hook LoadLibraryExW, automatic Explorer Patcher detection might not work properly!");
        }

        HandleLoadedExplorerPatcher();
    }

    g_isWhInitialized = true; // if not set the hook operations will not be applied after Windows startup

    return TRUE;
}

void Wh_ModAfterInit()
{
    LOG_TRACE();

    // autodetect Explorer Patcher on Windows 11 taskbar
    if (g_taskbarVersion == WIN_11)
    {
        // Try again in case there's a race between the previous attempt and the LoadLibraryExW hook.
        HandleLoadedExplorerPatcher();
    }

    // indentify taskbar windows so that message processing can be hooked
    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) // if Shell_TrayWnd class is defined
    {
        const auto [hPrimaryTaskbarWnd, secondaryTaskbarWindows] = FindTaskbarWindows();
        if (hPrimaryTaskbarWnd)
        {
            HandleIdentifiedTaskbarWindow(hPrimaryTaskbarWnd);
        }
        for (HWND hSecondaryWnd : secondaryTaskbarWindows)
        {
            HandleIdentifiedSecondaryTaskbarWindow(hSecondaryWnd);
        }
    }
    else
    {
        LOG_INFO(L"Failed to find Shell_TrayWnd class. Taskbar might not get hooked properly.");
    }
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    LOG_TRACE();

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    LoadSettings();
    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;
    return TRUE;
}

void Wh_ModUninit()
{
    LOG_TRACE();

    g_windowFocusTracker.Stop();

    if (IsWindow(g_hTaskbarWnd))
    {
        SendMessage(g_hTaskbarWnd, g_uninitCOMAPIMsg, FALSE, 0); // uninitialize COM API from gui thread

        UnsubclassTaskbarWindow(g_hTaskbarWnd);
        g_hTaskbarWnd = NULL;
        g_hTaskbarInputSiteWnd = NULL;
    }

    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows)
    {
        if (IsWindow(hSecondaryWnd))
        {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }
    g_secondaryTaskbarWindows.clear();
    g_secondaryTaskbarInputSiteWindows.clear();

    g_hookedInputSiteProcs.clear();

    if (g_hTaskSwitchingWnd && g_isTaskSwitchingWindowSubclassed && IsWindow(g_hTaskSwitchingWnd))
    {
        UnsubclassTaskSwitchingWindow();
    }
}

#pragma endregion // windhawk_mod_functions