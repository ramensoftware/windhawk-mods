// ==WindhawkMod==
// @id              taskbar-empty-space-clicks
// @name            Click on empty taskbar space
// @description     Trigger custom action when empty space on a taskbar is clicked. Various mouse clicks and keyboard modifiers are supported.
// @version         2.0
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

This mod lets you assign an action to a mouse click on Windows taskbar. Single, double and triple clicks are supported - both mouse and touchscreen clicks. You can also assign a keyboard modifier to the action. For example, you can set up a double click on the taskbar to open Task Manager while holding down the Ctrl key. The mod is designed to be as flexible as possible. You can assign any action to any mouse click or touch screen tap. You can also assign multiple actions to the same trigger. This mod reacts when empty space of the taskbar is clicked. Buttons, menus or other function of the taskbar are not affected. Click events are normally forwarded to the system, so you can still use the taskbar as usual. Both primary and secondary taskbars are supported.

## Supported actions:

1. **Show desktop** - Toggle show/hide desktop
2. **Ctrl+Alt+Tab** - Opens Ctrl+Alt+Tab dialog
3. **Task Manager** - Opens Windows default task manager
4. **Mute system volume** - Toggle mute of system volume (all sound)
5. **Taskbar auto-hide** - Toggle Windows taskbar auto-hide feature
6. **Win+Tab** - Opens Win+Tab dialog
7. **Hide desktop icons** - Toggle show/hide of all desktop icons
7. **Combine Taskbar buttons** - Toggle combining of Taskbar buttons between two states set in the Settings menu (not available on older Windows 11 versions)
7. **Open Start menu** - Sends Win key press to open Start menu
8. **Virtual key press** - Sends virtual keypress (keyboard shortcut) to the system
9. **Start application** - Starts arbitrary application or runs a command

### Example

Following animation shows how to setup **taskbar auto-hide** feature toggle on midle mouse button click and **toggle volume mute** on Ctrl + double-click.

![How to set "Click on empty taskbar space" Windhawk mod](https://i.imgur.com/b6rBLfF.gif)

Once set, simple middle-click on empty taskbar space will toggle auto-hide feature:

![Demonstration of Toggle taskbar autohide mod for Windhawk](https://i.imgur.com/BRQrVnX.gif)

## Supported triggers:

- **Keyboard** - Optional. Keyboard keypress modifiers. If None is selected or added, the modifier gets ignored.
    - **Left Ctrl** - Left Ctrl key
    - **Left Shift** - Left Shift key
    - **Left Alt** - Left Alt key
    - **Win** - Windows key
    - **Right Ctrl** - Right Ctrl key
    - **Right Shift** - Right Shift key
    - **Right Alt** - Right Alt key
- **Mouse** - Required. Mouse click or touchscreen tap trigger. If None is selected, the whole trigger+action gets ignored.
    - **Left** - Mouse left button click
    - **Left Double** - Mouse left button double click
    - **Left Triple** - Mouse left button triple click
    - **Middle** - Mouse middle button click
    - **Middle Double** - Mouse middle button double click
    - **Middle Triple** - Mouse middle button triple click
    - **Right** - Mouse right button click
    - **Right Double** - Mouse right button double click
    - **Right Triple** - Mouse right button triple click
    - **Tap** - Touchscreen single tap
    - **Tap Double** - Touchscreen double tap
    - **Tap Triple** - Touchscreen triple tap

## Additional arguments:

Some actions support or require additional arguments. You can set them in the Settings menu. Arguments are separated by semicolon. For example: `arg1;arg2`.

1. Show desktop - no additional arguments supported
2. Ctrl+Alt+Tab - no additional arguments supported
3. Task Manager - no additional arguments supported
4. Mute system volume - no additional arguments supported
5. Taskbar auto-hide - no additional arguments supported
6. Win+Tab - no additional arguments supported
7. Hide desktop icons - no additional arguments supported
7. Combine Taskbar buttons - `priTaskBarBtnState1;priTaskBarBtnState2;secTaskBarBtnState1;secTaskBarBtnState2`
    - priTaskBarBtnState1: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - priTaskBarBtnState2: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - secTaskBarBtnState1: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - secTaskBarBtnState2: `COMBINE_ALWAYS`, `COMBINE_WHEN_FULL`, `COMBINE_NEVER`
    - Example: `COMBINE_ALWAYS;COMBINE_WHEN_FULL;COMBINE_ALWAYS;COMBINE_NEVER`
7. Open Start menu - no additional arguments supported
8. Virtual key press - `virtualKey1;virtualKey2;...;virtualKeyN`
    - Example: `0x5B;0x45`
    - Each following text field correspond to one virtual key press. Fill hexa-decimal key codes of keys you want to press. Key codes are defined in [win32 inputdev docs](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes). Use only hexa-decimal (0x) or decimal format of a key code! Example: (0x5B and 0x45) corresponds to  (Win + E) shortcut that opens Explorer window. If your key combination has no effect, check out log for more information. Please note, that some special keyboard shortcuts like Win+L or Ctrl+Alt+Delete cannot be sent via inputdev interface.
9. Start application - `applicationPath arg1 arg2 ... argN`
    - Example: `C:\Windows\System32\notepad.exe C:\Users\username\Desktop\test.txt`
    - Example: `python.exe D:\MyScripts\my_python_script.py arg1 "arg 2 with space" arg3`
    - Example: `cmd.exe /c echo Hello & pause`
    - Takes and executes the whole `applicationPath` string as a new process. No semicolons are parsed! Only leading and trailing white characters are removed. You can use full path to the application or just the executable name if it is in PATH. In case you want to execute shell command, use cmd.exe with corresponding flag.

## Caveats and limitations:

### Click/tap gesture evaluation

By default, after every click or tap on the taskbar, the mod waits for the Windows double-click time ([GetDoubleClickTime](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdoubleclicktime), usually 500 ms) before running any action.

This short delay is needed so the mod can correctly decide whether you did a:
- single click/tap
- double click/tap
- triple click/tap

This is what allows you, for example, to double-click the taskbar without triggering the single-click action first.

If you don’t like this delay, you can turn on the **Eager trigger evaluation** option in the mod’s settings.

With **Eager trigger evaluation** enabled:

The action runs immediately when a matching trigger is detected (no waiting).
However, double or triple clicks/taps can still trigger the single-click/tap action, as long as you haven’t configured a separate double or triple click/tap action for that same trigger.
In other words, this option is a trade-off:

- Off – slight delay, but more accurate recognition of single vs. double vs. triple gestures
- On – no delay, but less precise gesture detection

### Right-click behavior:
When you configure any right-click trigger (single, double, or triple), the mod needs to temporarily delay the taskbar's context menu to detect your intended action.

#### Here's how it works:

- When you right-click the taskbar, the mod checks if you're using the keyboard modifier associated with your configured trigger
- If the keyboard modifier matches your trigger setup, the context menu is blocked and your custom action runs instead
- If the keyboard modifier doesn't match (or you don't complete the trigger), the context menu appears normally

#### What this means for you:
If you set up a right-click trigger without keyboard modifiers (for example, a right double-click), you'll notice a brief delay before the context menu appears after a single right-click. This happens because the mod waits to see if you're going to complete a double or triple click. The delay is short but noticeable—it's the trade-off for having custom right-click actions.

Tip: To avoid this delay, consider using keyboard modifiers with your right-click triggers (like Ctrl + right double-click). This way, the mod can instantly show the context menu when you right-click without holding the modifier key.

## Supported Windows versions are:
- Windows 10 22H2 (prior versions are not tested, but should work as well)
- Windows 11 24H2 - latest major (prior versions are not tested, but should work as well)

I will not supporting Insider preview or other minor versions of Windows. However, feel free to [report any issues](https://github.com/m1lhaus/windhawk-mods/issues) related to those versions. I'll appreciate the heads-up in advance.

⚠️ **Caution!** Avoid using option "Get the latest updates as soon as they're available" in Windows Update. Microsoft releases symbols for new Windows versions with a delay. This can render Windhawk mods unusable until the symbols are released (usually few days).

## Classic taskbar on Windows 11

In case you are using old Windows 10 taskbar on Windows 11 (**ExplorerPatcher** or a similar tool), enable corresponding option on Settings menu. This options will be tested only with the latest major version of Windows 11 (e.g. 24H2).

## Suggestions and new features

If you have request for new functions, suggestions or you are experiencing some issues, please post an [Issue on Github page](https://github.com/m1lhaus/windhawk-mods/issues). Please, be as specific as possible and provide as much information as you can. Please consider using AI chatbot if you are struggling with putting all together in English.

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
      - tapSingle: Touchscreen single tap
      - tapDouble: Touchscreen double tap
      - tapTriple: Touchscreen triple tap
    - Action: ACTION_NOTHING
      $name: Action
      $description: Action to invoke on trigger.
      $options:
      - ACTION_NOTHING: Nothing (default)
      - ACTION_SHOW_DESKTOP: Show desktop
      - ACTION_ALT_TAB: Ctrl+Alt+Tab
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_MUTE: Mute system volume
      - ACTION_TASKBAR_AUTOHIDE: Taskbar auto-hide
      - ACTION_WIN_TAB: Win+Tab
      - ACTION_HIDE_ICONS: Hide desktop icons
      - ACTION_COMBINE_TASKBAR_BUTTONS: Combine Taskbar buttons
      - ACTION_OPEN_START_MENU: Open Start menu
      - ACTION_SEND_KEYPRESS: Virtual key press
      - ACTION_START_PROCESS: Start application
    - AdditionalArgs: arg1;arg2
      $name: Additional Args
      $description: Additional arguments for the selected action, separated by semicolons. See the mod's Details tab for more information about the supported arguments for each action.
  $name: Taskbar empty space actions
  $description: "Using the Keyboard and Mouse combo boxes, select a trigger for a specific action. For example, the combination 'Left Ctrl + Double-click + Task Manager' will open the Windows Task Manager when the user double-clicks empty space on the taskbar while holding the Left Ctrl key. More actions can be set up with the Add new item button."
- oldTaskbarOnWin11: false
  $name: Use the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool). Note: For Windhawk versions earlier
    than 1.3, you must disable and re-enable the mod to apply this option.
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

#include <initguid.h>
#include <commctrl.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
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

#include <string>
#include <unordered_set>
#include <vector>
#include <functional>
#include <algorithm>

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

// get the naming of smart ptrs somehow consistent since winapi naming is wild
using winrt::com_ptr;
using bstr_ptr = _bstr_t;

// =====================================================================

#define ENABLE_LOG_INFO // info messages will be enabled
#define ENABLE_LOG_DEBUG // verbose debug messages will be enabled
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
        const char *filename = "empty_space_clicks_log.txt";
        std::string filepath;

        // get the path to the desktop
        const char *homeDir = std::getenv("USERPROFILE");
        if (homeDir)
        {
            std::string path(homeDir);
            path += "\\Desktop\\";
            path += filename;
            filepath = path;
        }
        else
        {
            filepath = filename;
        }
        m_file.open(filepath, std::ios_base::out | std::ios_base::app);
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

enum TaskBarVersion
{
    WIN_10_TASKBAR = 0,
    WIN_11_TASKBAR,
    UNKNOWN_TASKBAR
};
const wchar_t *TaskBarVersionNames[] = {L"WIN_10_TASKBAR", L"WIN_11_TASKBAR", L"UNKNOWN_TASKBAR"};

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
    std::wstring actionName;                  // Name of the action parsed from settings
    uint32_t expectedKeyModifiersState;       // expected state (bitmask) of the key modifiers that should be checked
    std::function<void(HWND)> actionExecutor; // function that executes the action
};

static struct
{
    bool oldTaskbarOnWin11;
    bool eagerTriggerEvaluation;
    std::vector<TriggerAction> triggerActions;
} g_settings;

// wrapper around COM API initialization and usage to enable lazy init and safe resource management
class COMAPI
{
public:
    COMAPI() : m_isInitialized(false), m_isCOMInitialized(false), m_isUIAInitialized(false), m_isDEInitialized(false),
               m_pUIAutomation(nullptr), m_pDeviceEnumerator(nullptr) {}

    // init COM for UIAutomation and Volume control
    bool Init()
    {
        if (!m_isCOMInitialized)
        {
            if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) // COM was most likely already initialized in the GUI thread, but just to be sure
            {
                m_isCOMInitialized = true;
                LOG_INFO(L"COM initilized");
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

// few helpers to ease up working with strings
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
}

void SetBit(uint32_t &value, uint32_t bit);
bool GetBit(const uint32_t &value, uint32_t bit);

static TaskBarVersion g_taskbarVersion = UNKNOWN_TASKBAR;

static DWORD g_dwTaskbarThreadId;
static bool g_isWhInitialized = false;
static bool g_inputSiteProcHooked = false;

static HWND g_hTaskbarWnd;
static std::unordered_set<HWND> g_secondaryTaskbarWindows;
static bool g_isContextMenuSuppressed = false;
static DWORD g_lastActionExecutionTimestamp = 0;

static const int g_mouseClickTimeoutMs = 200; // time to wait since the last time an action was executed
static const DWORD g_injectedClickID = 0xEADBEAF1u; // magic number to identify synthesized clicks
static const UINT g_explorerPatcherContextMenuMsg = RegisterWindowMessageW(L"Windows11ContextMenu_{D17F1E1A-5919-4427-8F89-A1A8503CA3EB}");
static const UINT g_uninitCOMAPIMsg = RegisterWindowMessageW(L"Windhawk_UnInit_COMAPI_empty-space-clicks");

// object to store information about the mouse click, its position, button, timestamp and whether it was on empty space
struct MouseClick
{
    enum class Button
    {
        LEFT = 0,
        RIGHT,
        MIDDLE,
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

    MouseClick(WPARAM wParam, LPARAM lParam, Type ptrType, Button btn, HWND hWnd) : type(ptrType), button(btn), position{0, 0}, timestamp(0), onEmptySpace(false), hWnd(hWnd)
    {
        timestamp = GetTickCount();
        if (!GetMouseClickPosition(lParam, position))
        {
            return; // without position there is no point to going further, other members are initialized so it's safe to return
        }

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
        if (FAILED(pUIAutomation->ElementFromPoint(position, pWindowElement.put())) || !pWindowElement)
        {
            LOG_ERROR(L"Failed to retrieve UI element from mouse click");
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

    static bool GetMouseClickPosition(LPARAM lParam, POINT &pointerLocation)
    {
        LOG_TRACE();

        // old Windows mouse handling of WM_MBUTTONDOWN message
        if (g_taskbarVersion == WIN_10_TASKBAR)
        {
            // message carries mouse position relative to the client window so use GetCursorPos() instead
            if (!GetCursorPos(&pointerLocation))
            {
                LOG_ERROR(L"Failed to get mouse position");
                return false;
            }
        }
        else
        {
            pointerLocation.x = GET_X_LPARAM(lParam);
            pointerLocation.y = GET_Y_LPARAM(lParam);
        }
        return true;
    }

    static MouseClick::Type GetPointerType(WPARAM wParam, LPARAM lParam)
    {
        MouseClick::Type type = MouseClick::Type::INVALID;

        if (g_taskbarVersion == WIN_10_TASKBAR)
        {
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
        }
        else
        {
            // Retrieve common pointer information to find out source of the click
            POINTER_INFO pointerInfo;
            UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
            if (GetPointerInfo(pointerId, &pointerInfo))
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
        }
        return type;
    }

    static uint32_t GetKeyModifiersState()
    {
        // Get all key states at once
        BYTE keyState[256] = {0};
        if (!GetKeyboardState(keyState))
        {
            LOG_ERROR(L"Failed to retrieve keyboard state");
            return 0U;
        }

        uint32_t currentKeyModifiersState = 0U;
        // Check for each modifier key if it is pressed
        if (keyState[VK_LCONTROL] & 0x80)
        {
            SetBit(currentKeyModifiersState, KEY_MODIFIER_LCTRL);
        }
        if (keyState[VK_LSHIFT] & 0x80)
        {
            SetBit(currentKeyModifiersState, KEY_MODIFIER_LSHIFT);
        }
        if (keyState[VK_LMENU] & 0x80)
        {
            SetBit(currentKeyModifiersState, KEY_MODIFIER_LALT);
        }
        if (keyState[VK_LWIN] & 0x80)
        {
            SetBit(currentKeyModifiersState, KEY_MODIFIER_LWIN);
        }
        if (keyState[VK_RCONTROL] & 0x80)
        {
            SetBit(currentKeyModifiersState, KEY_MODIFIER_RCTRL);
        }
        if (keyState[VK_RSHIFT] & 0x80)
        {
            SetBit(currentKeyModifiersState, KEY_MODIFIER_RSHIFT);
        }
        if (keyState[VK_RMENU] & 0x80)
        {
            SetBit(currentKeyModifiersState, KEY_MODIFIER_RALT);
        }
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

// simple ring buffer to store last 3 mouse clicks with python-like index access
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

static UINT_PTR gMouseClickTimer = (UINT_PTR)NULL;

// =====================================================================
// Forward declarations

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
bool IsKeyPressed(int vkCode);
bool ExecuteTaskbarAction(const std::wstring &mouseTriggerName, const uint32_t numClicks);
void SynthesizeTaskbarRightClick(POINT ptScreen);
void CALLBACK ProcessDelayedMouseClick(HWND, UINT, UINT_PTR, DWORD);
std::function<bool()> GetTaskbarActionExecutor(const bool checkForHigherOrderClicks);
bool isTriggerDefined(const std::wstring &mouseTriggerName, const int numClicks);
std::wstring GetActionName(const MouseClick::Type clickType, const uint32_t numClicks, const MouseClick::Button button = MouseClick::Button::INVALID);
bool OnMouseClick(MouseClick click);
bool GetTaskbarAutohideState();
void SetTaskbarAutohide(bool enabled);
void ToggleTaskbarAutohide();
void ShowDesktop();
void SendKeypress(std::vector<int> keys);
void SendCtrlAltTabKeypress();
void SendWinTabKeypress();
bool ClickStartMenu();
void OpenStartMenu();
void OpenTaskManager(HWND taskbarhWnd);
void ToggleVolMuted();
void HideIcons();
void CombineTaskbarButtons(const TaskBarButtonsState primaryTaskBarButtonsState1, const TaskBarButtonsState primaryTaskBarButtonsState2,
                           const TaskBarButtonsState secondaryTaskBarButtonsState1, const TaskBarButtonsState secondaryTaskBarButtonsState2);
DWORD GetCombineTaskbarButtons(const wchar_t *optionName);
bool SetCombineTaskbarButtons(const wchar_t *optionName, unsigned int option);
void StartProcess(const std::wstring &command);
std::tuple<TaskBarButtonsState, TaskBarButtonsState, TaskBarButtonsState, TaskBarButtonsState> ParseTaskBarButtonsState(const std::wstring &args);

// =====================================================================

#pragma region hook_magic

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_empty-space-clicks");

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    LOG_TRACE();

    struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM
    {
        SUBCLASSPROC pfnSubclass;
        UINT_PTR uIdSubclass;
        DWORD_PTR dwRefData;
        BOOL result;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0)
    {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId())
    {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI_LAMBDA_RETURN(LRESULT)
        {
            if (nCode == HC_ACTION)
            {
                const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
                if (cwp->message == g_subclassRegisteredMsg && cwp->wParam)
                {
                    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM *param = (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM *)cwp->lParam;
                    param->result = SetWindowSubclass(cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook)
    {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

#ifdef ENABLE_LOG_DEBUG
void printMessage(UINT uMsg)
{
    if ((uMsg != WM_NCMOUSEMOVE) &&
        (uMsg != WM_NOTIFY) &&
        (uMsg != 0x84) && // ? SPI_GETMOUSEDRAGOUTTHRESHOLD
        (uMsg != 0x20) &&
        (uMsg != WM_WINDOWPOSCHANGING) &&
        (uMsg != WM_WINDOWPOSCHANGED) &&
        (uMsg != WM_ACTIVATEAPP) &&
        (uMsg != WM_NCACTIVATE) &&
        (uMsg != WM_ACTIVATE) &&
        (uMsg != 0x281) &&
        (uMsg != 0x282) &&
        (uMsg != 0x003D) &&
        (uMsg != WM_SETFOCUS) &&
        (uMsg != WM_KILLFOCUS) &&
        (uMsg != WM_SYSCOMMAND) &&
        (uMsg != WM_CAPTURECHANGED) &&
        (uMsg != 0x0014) &&
        (uMsg != WM_PRINTCLIENT) &&
        (uMsg != WM_CHANGEUISTATE) &&
        (uMsg != CB_GETCOMBOBOXINFO) &&
        (uMsg != WM_ENTERIDLE) &&
        (uMsg != 0x2b) &&
        (uMsg != 0x2c) &&
        (uMsg != 0x113) &&
        (uMsg != 0x200) &&
        (uMsg != 0x24a) &&
        (uMsg != 0x245))
    {
        switch (uMsg)
        {
        case WM_CONTEXTMENU:
            LOG_DEBUG(L"Message: WM_CONTEXTMENU");
            break;
        case WM_NCLBUTTONDOWN:
            LOG_DEBUG(L"Message: WM_NCLBUTTONDOWN");
            break;
        case WM_NCLBUTTONUP:
            LOG_DEBUG(L"Message: WM_NCLBUTTONUP");
            break;
        case WM_NCLBUTTONDBLCLK:
            LOG_DEBUG(L"Message: WM_NCLBUTTONDBLCLK");
            break;
        case WM_NCRBUTTONDOWN:
            LOG_DEBUG(L"Message: WM_NCRBUTTONDOWN");
            break;
        case WM_NCRBUTTONUP:
            LOG_DEBUG(L"Message: WM_NCRBUTTONUP");
            break;
        case WM_NCRBUTTONDBLCLK:
            LOG_DEBUG(L"Message: WM_NCRBUTTONDBLCLK");
            break;
        case WM_NCMBUTTONDOWN:
            LOG_DEBUG(L"Message: WM_NCMBUTTONDOWN");
            break;
        case WM_NCMBUTTONUP:
            LOG_DEBUG(L"Message: WM_NCMBUTTONUP");
            break;
        case WM_NCMBUTTONDBLCLK:
            LOG_DEBUG(L"Message: WM_NCMBUTTONDBLCLK");
            break;
        case WM_POINTERDOWN:
            LOG_DEBUG(L"Message: WM_POINTERDOWN");
            break;
        case WM_POINTERUP:
            LOG_DEBUG(L"Message: WM_POINTERUP");
            break;
        default:
            LOG_DEBUG(L"Message: 0x%x", uMsg);
            break;
        }
    }
}
#endif

// proc handler for older Windows (nonXAML taskbar) versions and ExplorerPatcher
LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam,
                                           _In_ UINT_PTR uIdSubclass, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam))
    {
        RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
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

    // printMessage(uMsg);

    bool suppress = false;
    // button up messages seems really unreliable, so only process down and dblclk messages
    const bool isLeftButton = (uMsg == WM_LBUTTONDOWN || uMsg == WM_NCLBUTTONDOWN) || (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_NCLBUTTONDBLCLK);
    const bool isRightButton = (uMsg == WM_RBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN) || (uMsg == WM_RBUTTONDBLCLK || uMsg == WM_NCRBUTTONDBLCLK);
    const bool isMiddleButton = (uMsg == WM_MBUTTONDOWN || uMsg == WM_NCMBUTTONDOWN) || (uMsg == WM_MBUTTONDBLCLK || uMsg == WM_NCMBUTTONDBLCLK);
    if ((g_taskbarVersion == WIN_10_TASKBAR) &&
        (isLeftButton || isRightButton || isMiddleButton))
    {
        const LPARAM extraInfo = GetMessageExtraInfo() & 0xFFFFFFFFu;
        if (extraInfo != g_injectedClickID)
        {
            // do lazy init, since doing Init during Wh_ModInit breaks Spotify's global (media) shortcuts
            if (!g_comAPI.IsInitialized())
            {
                g_comAPI.Init(); // make sure it gets initialied from GUI thread
            }

            MouseClick::Button button = isLeftButton ? MouseClick::Button::LEFT : (isRightButton ? MouseClick::Button::RIGHT : MouseClick::Button::MIDDLE);
            OnMouseClick(MouseClick(wParam, lParam, MouseClick::GetPointerType(wParam, lParam), button, hWnd));
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
        const auto &lastClick = g_mouseClickQueue[-1];
        const LPARAM extraInfo = GetMessageExtraInfo() & 0xFFFFFFFFu;
        if ((extraInfo != g_injectedClickID) && lastClick.onEmptySpace && (lastClick.button == MouseClick::Button::RIGHT))
        {
            if (ShallSuppressContextMenu(lastClick)) // avoid opening right click menu when performing a right click action
            {
                g_isContextMenuSuppressed = true;
                suppress = true; // suppress the right click menu (otherwise a double click would be impossible)
            }
        }
    }

    LRESULT result = 0;
    if (!suppress)
    {
        result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    return result;
}

// proc handler for newer Windows versions (Windows 11 21H2 and newer) and ExplorerPatcher (Win11 menu)
WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == g_uninitCOMAPIMsg)
    {
        LOG_INFO("Received uninit COM API message, uninitializing COM API");
        g_comAPI.Uninit();
        return 0;
    }

    // printMessage(uMsg);

    bool suppressMsg = false;
    switch (uMsg)
    {
    case WM_POINTERDOWN:
        HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
        if (IsTaskbarWindow(hRootWnd))
        {
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
            else
            {
                break;
            }

            // do lazy init, since doing Init during Wh_ModInit breaks Spotify's global (media) shortcuts
            if (!g_comAPI.IsInitialized())
            {
                g_comAPI.Init(); // make sure it gets initialied from GUI thread
            }

            // check whether we need to suppress the context menu for right clicks
            const auto lastClick = MouseClick(wParam, lParam, MouseClick::GetPointerType(wParam, 0), button, hRootWnd);
            if (lastClick.onEmptySpace && (lastClick.button == MouseClick::Button::RIGHT))
            {
                LPARAM extraInfo = GetMessageExtraInfo() & 0xFFFFFFFFu;
                if (extraInfo == g_injectedClickID)
                {
                    LOG_DEBUG("Recognized synthesized right click via extra info tag, skipping");
                    break;
                }
                if (ShallSuppressContextMenu(lastClick))
                {
                    g_isContextMenuSuppressed = true;
                    suppressMsg = true; // suppress the right click menu (otherwise a double click would be impossible)
                }
            }
            OnMouseClick(lastClick);
        }
        break;
    }

    if (suppressMsg)
    {
        return 0; // suppress the message
    }
    else
    {
        return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam); // pass the message to the original wndproc (make e.g. right click work)
    }
}

BOOL SubclassTaskbarWindow(HWND hWnd)
{
    return SetWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc, 0, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd)
{
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void HandleIdentifiedInputSiteWindow(HWND hWnd)
{
    if (!g_dwTaskbarThreadId || GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId)
    {
        return;
    }

    // just double check that we are trying to hook the right window
    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd || !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName, L"Windows.UI.Composition.DesktopWindowContentBridge") != 0)
    {
        LOG_DEBUG("Parent window is not Windows.UI.Composition.DesktopWindowContentBridge, but %s", szClassName);
        return;
    }
    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd))
    {
        LOG_DEBUG("Parent window of Windows.UI.Composition.DesktopWindowContentBridge is not taskbar window");
        return;
    }

    // At first, I tried to subclass the window instead of hooking its wndproc,
    // but the inputsite.dll code checks that the value wasn't changed, and
    // crashes otherwise.
    void *wndProc = (void *)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    Wh_SetFunctionHook(wndProc, (void *)InputSiteWindowProc_Hook, (void **)&InputSiteWindowProc_Original);

    if (g_isWhInitialized)
    {
        LOG_DEBUG("Calling Wh_ApplyHookOperations");
        Wh_ApplyHookOperations(); // from docs: Can't be called before Wh_ModInit returns or after Wh_ModBeforeUninit returns
    }

    LOG_DEBUG(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HandleIdentifiedTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();

    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (SubclassTaskbarWindow(hWnd))
    {
        LOG_DEBUG(L"Main taskbar window %d subclassed successfully", (DWORD)(ULONG_PTR)hWnd);
    }
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows)
    {
        if (SubclassTaskbarWindow(hSecondaryWnd))
        {
            LOG_DEBUG(L"Secondary taskbar window %d subclassed successfully", (DWORD)(ULONG_PTR)hSecondaryWnd);
        }
    }

    if ((g_taskbarVersion == WIN_11_TASKBAR) && !g_inputSiteProcHooked)
    {
        HWND hXamlIslandWnd = FindWindowEx(hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd)
        {
            HWND hInputSiteWnd = FindWindowEx(hXamlIslandWnd, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd)
            {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();

    if (!g_dwTaskbarThreadId || GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId)
    {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    if (SubclassTaskbarWindow(hWnd))
    {
        LOG_DEBUG(L"Secondary taskbar window %d subclassed successfully", (DWORD)(ULONG_PTR)hWnd);
    }

    if ((g_taskbarVersion == WIN_11_TASKBAR) && !g_inputSiteProcHooked)
    {
        HWND hXamlIslandWnd = FindWindowEx(hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd)
        {
            HWND hInputSiteWnd = FindWindowEx(hXamlIslandWnd, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd)
            {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

// finds main task bar and returns its hWnd,
// optinally it finds also secondary taskbars and fills them to the set
// secondaryTaskbarWindows
HWND FindCurrentProcessTaskbarWindows(std::unordered_set<HWND> *secondaryTaskbarWindows)
{
    struct ENUM_WINDOWS_PARAM
    {
        HWND *hWnd;
        std::unordered_set<HWND> *secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
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
                *param.hWnd = hWnd;
            }
            else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0)
            {
                param.secondaryTaskbarWindows->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
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
        LOG_DEBUG(L"Shell_TrayWnd window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    }
    else if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0)
    {
        LOG_DEBUG(L"Shell_SecondaryTrayWnd window created: %08X", (DWORD)(ULONG_PTR)hWnd);
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
    if (bTextualClassName && _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) // Windows 11 taskbar
    {
        LOG_DEBUG(L"Windows.UI.Input.InputSite.WindowClass window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if ((g_taskbarVersion == WIN_11_TASKBAR) && !g_inputSiteProcHooked)
        {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

#pragma endregion // hook_magic

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

BOOL WindowsVersionInit()
{
    LOG_TRACE();

    VS_FIXEDFILEINFO *pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo)
    {
        LOG_ERROR(L"Failed to get Windows module version info");
        return FALSE;
    }

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    LOG_INFO(L"Windows version (major.minor.build): %d.%d.%d", nMajor, nMinor, nBuild);

    if (nMajor == 6)
    {
        g_taskbarVersion = WIN_10_TASKBAR;
    }
    else if (nMajor == 10)
    {
        if (nBuild < 22000)
        { // 21H2
            g_taskbarVersion = WIN_10_TASKBAR;
        }
        else
        {
            g_taskbarVersion = WIN_11_TASKBAR;
        }
    }
    else
    {
        g_taskbarVersion = UNKNOWN_TASKBAR;
    }

    return TRUE;
}

KeyModifier GetKeyModifierFromName(const std::wstring &keyName)
{
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

std::vector<std::wstring> SplitArgs(const std::wstring &args)
{
    std::vector<std::wstring> result;

    std::wstring args_ = stringtools::trim(args);
    if (args_.empty())
    {
        return result;
    }

    size_t start = 0;
    size_t end = args_.find(L';');
    while (end != std::wstring::npos)
    {
        auto substring = stringtools::trim(args_.substr(start, end - start));
        if (!substring.empty())
        {
            result.push_back(substring);
        }
        start = end + 1;
        end = args_.find(L';', start);
    }
    auto substring = stringtools::trim(args_.substr(start));
    if (!substring.empty())
    {
        result.push_back(substring);
    }
    return result;
}

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

std::vector<int> ParseVirtualKeypressSetting(const std::wstring &args)
{
    LOG_TRACE();

    std::vector<int> keys;

    const auto argsSplit = SplitArgs(args);
    for (const auto &arg : argsSplit)
    {
        const auto keyCode = ParseVirtualKey(arg.c_str());
        if (keyCode)
        {
            keys.push_back(keyCode);
        }
    }

    return keys;
}

std::function<void(HWND)> ParseMouseActionSetting(const std::wstring &actionName, const std::wstring &args)
{
    LOG_TRACE();

    auto doNothing = [](HWND) { /* Do nothing */ };

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
        return [](HWND)
        { SendCtrlAltTabKeypress(); };
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
        TaskBarButtonsState primaryState1; // use tie for Windhawk 1.4.1 compiler compatibility
        TaskBarButtonsState primaryState2;
        TaskBarButtonsState secondaryState1;
        TaskBarButtonsState secondaryState2;
        std::tie(primaryState1, primaryState2, secondaryState1, secondaryState2) = ParseTaskBarButtonsState(args);
        return [primaryState1, primaryState2, secondaryState1, secondaryState2](HWND)
        { CombineTaskbarButtons(primaryState1, primaryState2, secondaryState1, secondaryState2); };
    }
    else if (actionName == L"ACTION_OPEN_START_MENU")
    {
        return [](HWND)
        { OpenStartMenu(); };
    }
    else if (actionName == L"ACTION_SEND_KEYPRESS")
    {
        std::vector<int> keyCodes = ParseVirtualKeypressSetting(args);
        return [keyCodes](HWND)
        {
            LOG_INFO(L"Sending arbitrary keypress");
            SendKeypress(keyCodes);
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

    LOG_ERROR(L"Unknown action '%s'", actionName.c_str());
    return doNothing;
}

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
        auto actionStr = std::wstring(StringSetting(Wh_GetStringSetting(L"TriggerActionOptions[%d].Action", i)));
        auto additionalArgsStr = std::wstring(StringSetting(Wh_GetStringSetting(L"TriggerActionOptions[%d].AdditionalArgs", i)));

        // no other actions were added by user, end parsing
        if (keyboardTriggers.empty() && mouseTriggerStr.empty() && actionStr.empty() && additionalArgsStr.empty())
            break;

        // if mouse trigger or action is missing, skip since the rest is irrelevant
        if (mouseTriggerStr.empty() || actionStr.empty())
            continue;

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
        triggerAction.mouseTriggerName = mouseTriggerStr;
        triggerAction.actionName = actionStr;
        triggerAction.actionExecutor = ParseMouseActionSetting(actionStr, additionalArgsStr);
        g_settings.triggerActions.push_back(triggerAction);
    }

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
    g_settings.eagerTriggerEvaluation = Wh_GetIntSetting(L"eagerTriggerEvaluation");
    LOG_INFO(L"Settings loaded: oldTaskbarOnWin11=%d, eagerTriggerEvaluation=%d, number of trigger actions=%d",
             g_settings.oldTaskbarOnWin11, g_settings.eagerTriggerEvaluation, (int)g_settings.triggerActions.size());
}

/**
 * @brief Finds the desktop window. Desktop window handle is used to send messages to the desktop (show/hide icons).
 *
 * @return HWND Desktop window handle
 */
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

void SendKeypress(std::vector<int> keys)
{
    LOG_TRACE();
    if (keys.empty())
    {
        LOG_DEBUG(L"No virtual key codes to send");
        return;
    }

    const int NUM_KEYS = keys.size();
    LOG_DEBUG(L"Sending %d keypresses", NUM_KEYS);
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

void SendCtrlAltTabKeypress()
{
    LOG_TRACE();

    LOG_INFO(L"Sending Ctrl+Alt+Tab keypress");
    SendKeypress({VK_LCONTROL, VK_LMENU, VK_TAB});
}

void SendWinTabKeypress()
{
    LOG_TRACE();

    LOG_INFO(L"Sending Win+Tab keypress");
    SendKeypress({VK_LWIN, VK_TAB});
}

bool ClickStartMenu()
{
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
    if (g_taskbarVersion == WIN_10_TASKBAR)
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
    if (g_taskbarVersion == WIN_10_TASKBAR)
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

void OpenStartMenu()
{
    LOG_TRACE();
    if (!ClickStartMenu()) // if user hide the start menu via other Windhawk mod, we can't click it
    {
        LOG_INFO(L"Sending Win keypress");
        SendKeypress({VK_LWIN});
    }
}

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

    com_ptr<IMMDevice> defaultAudioDevice;
    if (SUCCEEDED(pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, defaultAudioDevice.put())))
    {
        // GUID of audio enpoint defined in Windows SDK (see Endpointvolume.h) - defined manually to avoid linking the whole lib
        const GUID XIID_IAudioEndpointVolume = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

        // get handle to the audio endpoint volume control
        com_ptr<IAudioEndpointVolume> endpointVolume;
        if (SUCCEEDED(defaultAudioDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, endpointVolume.put_void())))
        {
            BOOL isMuted = FALSE;
            if (FAILED(endpointVolume->GetMute(&isMuted)) || FAILED(endpointVolume->SetMute(!isMuted, NULL)))
            {
                LOG_ERROR(L"Failed to toggle volume mute - failed to get and set mute state!");
            }
        }
        else
        {
            LOG_ERROR(L"Failed to toggle volume mute - failed to get audio endpoint volume handle!");
        }
    }
    else
    {
        LOG_ERROR(L"Failed to toggle volume mute - failed to get default audio endpoint!");
    }
}

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

void CombineTaskbarButtons(const TaskBarButtonsState primaryTaskBarButtonsState1, const TaskBarButtonsState primaryTaskBarButtonsState2,
                           const TaskBarButtonsState secondaryTaskBarButtonsState1, const TaskBarButtonsState secondaryTaskBarButtonsState2)
{
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
        SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)TEXT("TraySettings"));
    }
}

/**
 * Retrieves the current setting for combining taskbar buttons. This setting is used as initial value for the toggle
 * so that the toggle actually does the toggling for the forst time it is activated.
 *
 * @return The current value of the taskbar button combining setting. (0 = Always, 1 = When taskbar is full, 2 = Never)
 */
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

/**
 * @brief Sets the option for combining taskbar buttons.
 *
 * This function allows you to set the option for combining taskbar buttons.
 * The option parameter specifies the desired behavior for combining taskbar buttons.
 *
 * @param optionName The name of the registry key to set.
 * @param option The option for combining taskbar buttons.
 *               Possible values:
 *               - 0: Do not combine taskbar buttons.
 *               - 1: Combine taskbar buttons when the taskbar is full.
 *               - 2: Always combine taskbar buttons.
 */
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

void StartProcess(const std::wstring &command)
{
    LOG_TRACE();
    if (command.empty())
    {
        LOG_DEBUG(L"Command is empty, nothing to start");
        return;
    }

    std::vector<std::wstring> args = SplitArgs(command);
    if (args.empty())
    {
        LOG_DEBUG(L"Command parsing resulted in empty arguments, nothing to start");
        return;
    }

    // First argument is the executable path/name
    std::wstring executable = args[0];

    // Build command line with remaining arguments
    std::wstring commandLine = executable;
    for (size_t i = 1; i < args.size(); i++)
    {
        // Add quotes around arguments that contain spaces
        if (args[i].find(L' ') != std::wstring::npos &&
            (args[i].front() != L'"' || args[i].back() != L'"'))
        {
            commandLine += L" \"" + args[i] + L"\"";
        }
        else
        {
            commandLine += L" " + args[i];
        }
    }

    LOG_INFO(L"Starting process: %s", commandLine.c_str());

    STARTUPINFO si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    if (!CreateProcess(NULL, (LPWSTR)commandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        DWORD error = GetLastError();
        LOG_ERROR(L"Failed to start process - CreateProcess failed with error code: %d", error);
    }
    else
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

// =====================================================================

bool IsTaskbarWindow(HWND hWnd)
{
    LOG_TRACE();

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)))
    {
        return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 || _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
    }
    else
    {
        LOG_ERROR(L"Failed to get window class name");
        return false;
    }
}

void SetBit(uint32_t &value, uint32_t bit)
{
    value |= (1U << bit);
}

bool GetBit(const uint32_t &value, uint32_t bit)
{
    return (value & (1U << bit)) != 0;
}

bool ShallSuppressContextMenu(const MouseClick &lastClick)
{
    for (const auto &triggerAction : g_settings.triggerActions)
    {
        if (triggerAction.mouseTriggerName.find(L"right", 0) == 0)
        {
            // we want to suppress only if user "is going for the trigger"
            if (lastClick.keyModifiersState == triggerAction.expectedKeyModifiersState)
            {
                LOG_DEBUG("Suppressing right click to suppress context menu");
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

bool IsTripleClick(const MouseClick::Button button)
{
    LOG_TRACE();
    return IsDoubleClick(button, g_mouseClickQueue[-2], g_mouseClickQueue[-1]) &&
           IsDoubleClick(button, g_mouseClickQueue[-3], g_mouseClickQueue[-2]);
}

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

bool IsTripleTap()
{
    LOG_TRACE();
    return IsDoubleTap(g_mouseClickQueue[-2], g_mouseClickQueue[-1]) && IsDoubleTap(g_mouseClickQueue[-3], g_mouseClickQueue[-2]);
}

bool IsMultiTap()
{
    LOG_TRACE();
    return IsDoubleTap(g_mouseClickQueue[-2], g_mouseClickQueue[-1]) &&
           IsDoubleTap(g_mouseClickQueue[-3], g_mouseClickQueue[-2]) &&
           IsDoubleTap(g_mouseClickQueue[-4], g_mouseClickQueue[-3]);
}

bool IsKeyPressed(int vkCode)
{
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
}

void SynthesizeTaskbarRightClick(POINT ptScreen)
{
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

void CALLBACK ProcessDelayedMouseClick(HWND, UINT, UINT_PTR, DWORD)
{
    if (!KillTimer(NULL, gMouseClickTimer))
    {
        LOG_ERROR(L"Failed to kill triple click timer");
    }
    gMouseClickTimer = 0;

    const POINT lastMousePos = g_mouseClickQueue[-1].position; // store since the queue will be cleared

    const auto actionExecutor = GetTaskbarActionExecutor(false /* do not check for existing higher-click-count triggers */);
    if (actionExecutor)
    {
        actionExecutor(); // execute taskbar action
    }

    if (g_isContextMenuSuppressed)  // action not executed, so we need to synthesize right-click to show context menu
    {
        SynthesizeTaskbarRightClick(lastMousePos);
        g_isContextMenuSuppressed = false;
    }
}

std::wstring GetActionName(const MouseClick::Type clickType, const uint32_t numClicks, const MouseClick::Button button)
{
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

bool isTriggerDefined(const std::wstring &mouseTriggerName, const int numClicks)
{
    for (const auto &triggerAction : g_settings.triggerActions)
    {
        if ((triggerAction.mouseTriggerName == mouseTriggerName) && triggerAction.actionExecutor)
        {
            bool allModifiersPressed = true;
            for (int i = 1; i <= numClicks; i++)
            {
                allModifiersPressed &= (g_mouseClickQueue[-i].keyModifiersState == triggerAction.expectedKeyModifiersState);
            }
            return allModifiersPressed;
        }
    }
    return false;
}

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
            for (int i = 1; i <= numClicks; i++)
            {
                allModifiersPressed &= (g_mouseClickQueue[-i].keyModifiersState == triggerAction.expectedKeyModifiersState);
                LOG_DEBUG(L"Click %d key modifiers state: %u, expected: %u",
                          i, g_mouseClickQueue[-i].keyModifiersState, triggerAction.expectedKeyModifiersState);
            }
            if (allModifiersPressed)
            {
                if (triggerAction.actionExecutor)
                {
                    LOG_INFO(L"Executing action: %s", triggerAction.actionName.c_str());
                    triggerAction.actionExecutor(hWnd);
                    g_lastActionExecutionTimestamp = GetTickCount();
                    g_mouseClickQueue.clear();
                    g_isContextMenuSuppressed = false;
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

// Returns a lambda function that checks if the given trigger matches and returns true if executed
std::function<bool()> GetTaskbarActionExecutor(const bool checkForHigherOrderClicks)
{
    const auto isHigherOrderClickDefined = [&](const MouseClick::Type clickType, const int currentCount, const MouseClick::Button button)
    {
        for (int higherCount = currentCount + 1; higherCount <= 3; higherCount++)
        {
            if (isTriggerDefined(GetActionName(clickType, higherCount, button), higherCount))
            {
                LOG_DEBUG(L"Higher order click action defined for %s, skipping lower order click",
                          GetActionName(MouseClick::Type::MOUSE, higherCount, button).c_str());
                return true; // higher order click action defined, skip this one
            }
        }
        return false;
    };

    // mouse clicks
    const MouseClick::Button mouseButtons[] = {MouseClick::Button::LEFT, MouseClick::Button::RIGHT, MouseClick::Button::MIDDLE};
    const std::function<bool(MouseClick::Button)> mouseChecks[] = {IsTripleClick, static_cast<bool(*)(MouseClick::Button)>(IsDoubleClick), IsSingleClick};
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
    const std::function<bool()> touchChecks[] = {IsTripleTap, static_cast<bool(*)()>(IsDoubleTap), IsSingleTap};
    for (const auto &checkFunc : touchChecks)
    {
        clickCount--;
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
    }
    return nullptr;
}

// main body of the mod called every time a taskbar is clicked
bool OnMouseClick(MouseClick click)
{
    LOG_TRACE();

    if ((GetCapture() != NULL) || !click.onEmptySpace)
    {
        return false;
    }

    if ((GetTickCount() - g_lastActionExecutionTimestamp) < g_mouseClickTimeoutMs)
    {
        LOG_DEBUG(L"In cooldown period, ignoring click");
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
            actionExecutor(); // execute taskbar action
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

////////////////////////////////////////////////////////////

BOOL Wh_ModInit()
{
    LOG_TRACE();

    LoadSettings();

    if (!WindowsVersionInit() || (g_taskbarVersion == UNKNOWN_TASKBAR))
    {
        LOG_ERROR(L"Unsupported Windows version, ModInit failed");
        return FALSE;
    }
    // treat Windows 11 taskbar as on older windows
    if ((g_taskbarVersion == WIN_11_TASKBAR) && g_settings.oldTaskbarOnWin11)
    {
        g_taskbarVersion = WIN_10_TASKBAR;
    }
    LOG_INFO(L"Using taskbar version: %s", TaskBarVersionNames[g_taskbarVersion]);

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
    // indentify taskbar windows so that message processing can be hooked
    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) // if Shell_TrayWnd class is defined
    {
        HWND hWnd = FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd)
        {
            HandleIdentifiedTaskbarWindow(hWnd); // hook
        }
    }
    else
    {
        LOG_ERROR(L"Failed to find Shell_TrayWnd class. Something changed under the hood! Taskbar might not get hooked properly!");
    }

    g_isWhInitialized = true; // if not set the hook operations will not be applied after Windows startup

    return TRUE;
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
    if (g_hTaskbarWnd)
    {
        SendMessage(g_hTaskbarWnd, g_uninitCOMAPIMsg, FALSE, 0); // uninitialize COM API from gui thread

        UnsubclassTaskbarWindow(g_hTaskbarWnd);
        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows)
        {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }
}
