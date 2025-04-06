// ==WindhawkMod==
// @id              taskbar-empty-space-clicks
// @name            Click on empty taskbar space
// @description     Trigger custom action when empty space on a taskbar is double/middle clicked
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

This mod lets you assign an action to a mouse click on Windows taskbar. Double-click and middle-click actions are supported. Additionally double tap and triple tap on touch screens are supported. This mod only modifies behaviour when empty space of the taskbar is clicked. Buttons, menus or other function of the taskbar are not affected. Both primary and secondary taskbars are supported.

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

## TODO: write more info about optional args

- CombineTaskbarButtons:
  - State1: COMBINE_ALWAYS
    $options:
    - COMBINE_ALWAYS: Always combine
    - COMBINE_WHEN_FULL: Combine when taskbar is full
    - COMBINE_NEVER: Never combine
    $name: Main taskbar state 1
  - State2: COMBINE_NEVER
    $options:
    - COMBINE_ALWAYS: Always combine
    - COMBINE_WHEN_FULL: Combine when taskbar is full
    - COMBINE_NEVER: Never combine
    $name: Main taskbar state 2
  - StateSecondary1: COMBINE_ALWAYS
    $options:
    - COMBINE_ALWAYS: Always combine
    - COMBINE_WHEN_FULL: Combine when taskbar is full
    - COMBINE_NEVER: Never combine
    $name: Secondary taskbar state 1
  - StateSecondary2: COMBINE_NEVER
    $options:
    - COMBINE_ALWAYS: Always combine
    - COMBINE_WHEN_FULL: Combine when taskbar is full
    - COMBINE_NEVER: Never combine
    $name: Secondary taskbar state 2
  $name: Combine Taskbar Buttons toggle
  $description: When toggle activated, switch between following states
- VirtualKeyPress: ["0x5B", "0x45"]
  $name: Virtual key press
  $description: >-
    Send custom virtual key press to the system. Each following text field correspond to one virtual key press. Fill hexa-decimal key codes of keys you want to press. Key codes are defined in win32 inputdev docs (https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes). Use only hexa-decimal (0x) or decimal format of a key code! Example: (0x5B and 0x45) corresponds to  (Win + E) shortcut that opens Explorer window. If your key combination has no effect, check out log for more information. Please note, that some special keyboard shortcuts like Win+L or Ctrl+Alt+Delete cannot be sent via inputdev interface.
- StartProcess: "C:\\Windows\\System32\\notepad.exe"
  $name: Start an application
  $description: >-
    Start arbitrary application or run a command. Use the executable name if it is in PATH. Otherwise use the full path to the application. Example: "C:\Windows\System32\notepad.exe". In case you want to execute shell command, use corresponding flag. Example: "cmd.exe /c echo Hello & pause".


## Example

Following animation shows **Taskbar auto-hide** feature. Feature gets toggled whenever user double-clicks the empty space on a taskbar.

![Demonstration of Toggle taskbar autohide mod for Windhawk](https://i.imgur.com/BRQrVnX.gif)

## Supported Windows versions are:
- Windows 10 22H2 (prior versions are not tested, but should work as well)
- Windows 11 23H2 - latest major

I will not supporting Insider preview or other minor versions of Windows. However, feel free to [report any issues](https://github.com/m1lhaus/windhawk-mods/issues) related to those versions. I'll appreciate the heads-up in advance.

⚠️ **Caution!** Avoid using option "Get the latest updates as soon as they're available" in Windows Update. Microsoft releases symbols for new Windows versions with a delay. This can render Windhawk mods unusable until the symbols are released (usually few days).

## Classic taskbar on Windows 11

In case you are using old Windows taskbar on Windows 11 (**ExplorerPatcher** or a similar tool), enable corresponding option on Settings menu. This options will be tested only with the latest major version of Windows 11 (e.g. 24H2).

## Suggestions and new features

If you have request for new functions, suggestions or you are experiencing some issues, please post an [Issue on Github page](https://github.com/m1lhaus/windhawk-mods/issues). Please, be as specific as possible and provide as much information as you can.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TriggerActionOptions:
  - - KeyboardTriggers: [none, lctrl, lshift, lalt, win, rctrl, rshift, ralt ]
      $name: Keyboard
      $description: Select keyboard keypress modifiers. If None is selected or added, this trigger gets ignored.
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
      $description: Select mouse click trigger. If None is selected, this trigger gets ignored.
      $options:
      - none: None
      - left: Mouse left button click
      - leftDouble: Mouse left button double click
      - leftTriple: Mouse left button triple click
      - middle: Mouse middle button click
      - middleDouble: Mouse middle button double click
      - middleTriple: Mouse middle button triple click
      - right: Mouse right button click
      - rightDouble: Mouse right button double click
      - rightTriple: Mouse right button triple click
      - tap: Touchscreen single tap
      - tapDouble: Touchscreen double tap
      - tapTriple: Touchscreen triple tap
    - Action: ACTION_NOTHING
      $name: Action
      $description: Action invoked on trigger
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
      $description: Additional arguments for selected action separated by semicolon. See mod's Details tab for more info about supported arguments for each action.
  $name: Taskbar empty space actions
  $description: "Using Keyboard and Mouse combo boxes select a trigger for certain Action. For example combination 'Left Ctrl + Double click + Task Manager' will open Windows Task Manager when user double clicks empty space on Taskbar while holding left Ctrl key pressed down. More actions can be setup with Add new item button."
- oldTaskbarOnWin11: false
  $name: Use the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool). Note: For Windhawk versions older
    than 1.3, you have to disable and re-enable the mod to apply this option.
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

// following block is to keep compatibility with pre Windhawk 1.5 versions
#ifndef __IUIAutomationElement_INTERFACE_DEFINED__

// following include are taken from Qt project since builtin compiler is missing those definitions
#pragma region uiautomation_includes

// Pasted below and commented duplicate definitions:
// https://github.com/qt/qtbase/blob/dev/src/gui/accessible/windows/apisupport/uiatypes_p.h
// https://github.com/qt/qtbase/blob/dev/src/gui/accessible/windows/apisupport/uiaclientinterfaces_p.h

// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR
// GPL-2.0-only OR GPL-3.0-only

#ifndef UIATYPES_H
#define UIATYPES_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

typedef int PROPERTYID;
typedef int PATTERNID;
typedef int EVENTID;
typedef int TEXTATTRIBUTEID;
typedef int CONTROLTYPEID;
typedef int LANDMARKTYPEID;
typedef int METADATAID;

typedef void *UIA_HWND;

// enum NavigateDirection {
//     NavigateDirection_Parent           = 0,
//     NavigateDirection_NextSibling      = 1,
//     NavigateDirection_PreviousSibling  = 2,
//     NavigateDirection_FirstChild       = 3,
//     NavigateDirection_LastChild        = 4
// };

// enum ProviderOptions {
//     ProviderOptions_ClientSideProvider      = 0x1,
//     ProviderOptions_ServerSideProvider      = 0x2,
//     ProviderOptions_NonClientAreaProvider   = 0x4,
//     ProviderOptions_OverrideProvider        = 0x8,
//     ProviderOptions_ProviderOwnsSetFocus    = 0x10,
//     ProviderOptions_UseComThreading         = 0x20,
//     ProviderOptions_RefuseNonClientSupport  = 0x40,
//     ProviderOptions_HasNativeIAccessible    = 0x80,
//     ProviderOptions_UseClientCoordinates    = 0x100
// };

enum SupportedTextSelection
{
    SupportedTextSelection_None = 0,
    SupportedTextSelection_Single = 1,
    SupportedTextSelection_Multiple = 2
};

enum TextUnit
{
    TextUnit_Character = 0,
    TextUnit_Format = 1,
    TextUnit_Word = 2,
    TextUnit_Line = 3,
    TextUnit_Paragraph = 4,
    TextUnit_Page = 5,
    TextUnit_Document = 6
};

enum TextPatternRangeEndpoint
{
    TextPatternRangeEndpoint_Start = 0,
    TextPatternRangeEndpoint_End = 1
};

enum TextDecorationLineStyle
{
    TextDecorationLineStyle_None = 0,
    TextDecorationLineStyle_Single = 1,
    TextDecorationLineStyle_WordsOnly = 2,
    TextDecorationLineStyle_Double = 3,
    TextDecorationLineStyle_Dot = 4,
    TextDecorationLineStyle_Dash = 5,
    TextDecorationLineStyle_DashDot = 6,
    TextDecorationLineStyle_DashDotDot = 7,
    TextDecorationLineStyle_Wavy = 8,
    TextDecorationLineStyle_ThickSingle = 9,
    TextDecorationLineStyle_DoubleWavy = 11,
    TextDecorationLineStyle_ThickWavy = 12,
    TextDecorationLineStyle_LongDash = 13,
    TextDecorationLineStyle_ThickDash = 14,
    TextDecorationLineStyle_ThickDashDot = 15,
    TextDecorationLineStyle_ThickDashDotDot = 16,
    TextDecorationLineStyle_ThickDot = 17,
    TextDecorationLineStyle_ThickLongDash = 18,
    TextDecorationLineStyle_Other = -1
};

enum CaretPosition
{
    CaretPosition_Unknown = 0,
    CaretPosition_EndOfLine = 1,
    CaretPosition_BeginningOfLine = 2
};

enum ToggleState
{
    ToggleState_Off = 0,
    ToggleState_On = 1,
    ToggleState_Indeterminate = 2
};

enum RowOrColumnMajor
{
    RowOrColumnMajor_RowMajor = 0,
    RowOrColumnMajor_ColumnMajor = 1,
    RowOrColumnMajor_Indeterminate = 2
};

enum TreeScope
{
    TreeScope_None = 0,
    TreeScope_Element = 0x1,
    TreeScope_Children = 0x2,
    TreeScope_Descendants = 0x4,
    TreeScope_Parent = 0x8,
    TreeScope_Ancestors = 0x10,
    TreeScope_Subtree = TreeScope_Element | TreeScope_Children | TreeScope_Descendants
};

enum OrientationType
{
    OrientationType_None = 0,
    OrientationType_Horizontal = 1,
    OrientationType_Vertical = 2
};

enum PropertyConditionFlags
{
    PropertyConditionFlags_None = 0,
    PropertyConditionFlags_IgnoreCase = 1
};

enum WindowVisualState
{
    WindowVisualState_Normal = 0,
    WindowVisualState_Maximized = 1,
    WindowVisualState_Minimized = 2
};

enum WindowInteractionState
{
    WindowInteractionState_Running = 0,
    WindowInteractionState_Closing = 1,
    WindowInteractionState_ReadyForUserInteraction = 2,
    WindowInteractionState_BlockedByModalWindow = 3,
    WindowInteractionState_NotResponding = 4
};

enum ExpandCollapseState
{
    ExpandCollapseState_Collapsed = 0,
    ExpandCollapseState_Expanded = 1,
    ExpandCollapseState_PartiallyExpanded = 2,
    ExpandCollapseState_LeafNode = 3
};

// struct UiaRect {
//     double left;
//     double top;
//     double width;
//     double height;
// };

struct UiaPoint
{
    double x;
    double y;
};

#endif

// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR
// GPL-2.0-only OR GPL-3.0-only

#ifndef UIACLIENTINTERFACES_H
#define UIACLIENTINTERFACES_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <unknwn.h>

#ifndef __IUIAutomationElement_INTERFACE_DEFINED__

struct IUIAutomationCondition;
struct IUIAutomationCacheRequest;
struct IUIAutomationElementArray;
struct IUIAutomationTreeWalker;
struct IUIAutomationEventHandler;
struct IUIAutomationPropertyChangedEventHandler;
struct IUIAutomationStructureChangedEventHandler;
struct IUIAutomationFocusChangedEventHandler;
struct IUIAutomationProxyFactory;
struct IUIAutomationProxyFactoryEntry;
struct IUIAutomationProxyFactoryMapping;
#ifndef __IAccessible_FWD_DEFINED__
#define __IAccessible_FWD_DEFINED__
struct IAccessible;
#endif /* __IAccessible_FWD_DEFINED__ */

#define __IUIAutomationElement_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomationElement, 0xd22108aa, 0x8ac5, 0x49a5, 0x83, 0x7b, 0x37, 0xbb, 0xb3, 0xd7, 0x59, 0x1e);
MIDL_INTERFACE("d22108aa-8ac5-49a5-837b-37bbb3d7591e")
IUIAutomationElement : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetFocus() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeId(__RPC__deref_out_opt SAFEARRAY * *runtimeId) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirst(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
                                                __RPC__deref_out_opt IUIAutomationElement * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAll(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
                                              __RPC__deref_out_opt IUIAutomationElementArray * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirstBuildCache(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
                                                          __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                          __RPC__deref_out_opt IUIAutomationElement * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAllBuildCache(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
                                                        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                        __RPC__deref_out_opt IUIAutomationElementArray * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE BuildUpdatedCache(__RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                        __RPC__deref_out_opt IUIAutomationElement * *updatedElement) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValue(PROPERTYID propertyId, __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValueEx(PROPERTYID propertyId, BOOL ignoreDefaultValue, __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValue(PROPERTYID propertyId, __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValueEx(PROPERTYID propertyId, BOOL ignoreDefaultValue, __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPatternAs(PATTERNID patternId, __RPC__in REFIID riid, __RPC__deref_out_opt void **patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPatternAs(PATTERNID patternId, __RPC__in REFIID riid, __RPC__deref_out_opt void **patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPattern(PATTERNID patternId, __RPC__deref_out_opt IUnknown * *patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPattern(PATTERNID patternId, __RPC__deref_out_opt IUnknown * *patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedParent(__RPC__deref_out_opt IUIAutomationElement * *parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedChildren(__RPC__deref_out_opt IUIAutomationElementArray * *children) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentProcessId(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentControlType(__RPC__out CONTROLTYPEID * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentLocalizedControlType(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentName(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAcceleratorKey(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAccessKey(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentHasKeyboardFocus(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsKeyboardFocusable(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsEnabled(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAutomationId(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentClassName(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentHelpText(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentCulture(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsControlElement(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsContentElement(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsPassword(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentNativeWindowHandle(__RPC__deref_out_opt UIA_HWND * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentItemType(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsOffscreen(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentOrientation(__RPC__out enum OrientationType * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentFrameworkId(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsRequiredForForm(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentItemStatus(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentBoundingRectangle(__RPC__out RECT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentLabeledBy(__RPC__deref_out_opt IUIAutomationElement * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAriaRole(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAriaProperties(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsDataValidForForm(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentControllerFor(__RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentDescribedBy(__RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentFlowsTo(__RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentProviderDescription(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedProcessId(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedControlType(__RPC__out CONTROLTYPEID * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedLocalizedControlType(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedName(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAcceleratorKey(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAccessKey(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedHasKeyboardFocus(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsKeyboardFocusable(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsEnabled(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAutomationId(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedClassName(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedHelpText(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedCulture(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsControlElement(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsContentElement(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsPassword(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedNativeWindowHandle(__RPC__deref_out_opt UIA_HWND * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedItemType(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsOffscreen(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedOrientation(__RPC__out enum OrientationType * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedFrameworkId(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsRequiredForForm(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedItemStatus(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedBoundingRectangle(__RPC__out RECT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedLabeledBy(__RPC__deref_out_opt IUIAutomationElement * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAriaRole(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAriaProperties(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsDataValidForForm(__RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedControllerFor(__RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedDescribedBy(__RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedFlowsTo(__RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedProviderDescription(__RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClickablePoint(__RPC__out POINT * clickable, __RPC__out BOOL * gotClickable) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationElement, 0xd22108aa, 0x8ac5, 0x49a5, 0x83, 0x7b, 0x37, 0xbb, 0xb3, 0xd7, 0x59, 0x1e)
#endif
#endif

#ifndef __IUIAutomation_INTERFACE_DEFINED__
#define __IUIAutomation_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomation, 0x30cbe57d, 0xd9d0, 0x452a, 0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee);
MIDL_INTERFACE("30cbe57d-d9d0-452a-ab13-7ac5ac4825ee")
IUIAutomation : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE CompareElements(__RPC__in_opt IUIAutomationElement * el1, __RPC__in_opt IUIAutomationElement * el2,
                                                      __RPC__out BOOL * areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE CompareRuntimeIds(__RPC__in SAFEARRAY * runtimeId1, __RPC__in SAFEARRAY * runtimeId2,
                                                        __RPC__out BOOL * areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElement(__RPC__deref_out_opt IUIAutomationElement * *root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandle(__RPC__in UIA_HWND hwnd, __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPoint(POINT pt, __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElement(__RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElementBuildCache(__RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                               __RPC__deref_out_opt IUIAutomationElement * *root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandleBuildCache(__RPC__in UIA_HWND hwnd, __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                  __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPointBuildCache(POINT pt, __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                 __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElementBuildCache(__RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                  __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTreeWalker(__RPC__in_opt IUIAutomationCondition * pCondition,
                                                       __RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewWalker(__RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewWalker(__RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewWalker(__RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewCondition(__RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewCondition(__RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewCondition(__RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCacheRequest(__RPC__deref_out_opt IUIAutomationCacheRequest * *cacheRequest) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTrueCondition(__RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFalseCondition(__RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePropertyCondition(PROPERTYID propertyId, VARIANT value,
                                                              __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePropertyConditionEx(PROPERTYID propertyId, VARIANT value, enum PropertyConditionFlags flags,
                                                                __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndCondition(__RPC__in_opt IUIAutomationCondition * condition1,
                                                         __RPC__in_opt IUIAutomationCondition * condition2,
                                                         __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndConditionFromArray(__RPC__in_opt SAFEARRAY * conditions,
                                                                  __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndConditionFromNativeArray(__RPC__in_ecount_full(conditionCount) IUIAutomationCondition * *conditions,
                                                                        int conditionCount,
                                                                        __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrCondition(__RPC__in_opt IUIAutomationCondition * condition1,
                                                        __RPC__in_opt IUIAutomationCondition * condition2,
                                                        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrConditionFromArray(__RPC__in_opt SAFEARRAY * conditions,
                                                                 __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrConditionFromNativeArray(__RPC__in_ecount_full(conditionCount) IUIAutomationCondition * *conditions,
                                                                       int conditionCount,
                                                                       __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateNotCondition(__RPC__in_opt IUIAutomationCondition * condition,
                                                         __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddAutomationEventHandler(EVENTID eventId, __RPC__in_opt IUIAutomationElement * element, enum TreeScope scope,
                                                                __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                __RPC__in_opt IUIAutomationEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAutomationEventHandler(EVENTID eventId, __RPC__in_opt IUIAutomationElement * element,
                                                                   __RPC__in_opt IUIAutomationEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPropertyChangedEventHandlerNativeArray(
        __RPC__in_opt IUIAutomationElement * element, enum TreeScope scope, __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__in_opt IUIAutomationPropertyChangedEventHandler * handler, __RPC__in_ecount_full(propertyCount) PROPERTYID * propertyArray,
        int propertyCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPropertyChangedEventHandler(
        __RPC__in_opt IUIAutomationElement * element, enum TreeScope scope, __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__in_opt IUIAutomationPropertyChangedEventHandler * handler, __RPC__in SAFEARRAY * propertyArray) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemovePropertyChangedEventHandler(__RPC__in_opt IUIAutomationElement * element,
                                                                        __RPC__in_opt IUIAutomationPropertyChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddStructureChangedEventHandler(__RPC__in_opt IUIAutomationElement * element, enum TreeScope scope,
                                                                      __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                      __RPC__in_opt IUIAutomationStructureChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveStructureChangedEventHandler(__RPC__in_opt IUIAutomationElement * element,
                                                                         __RPC__in_opt IUIAutomationStructureChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddFocusChangedEventHandler(__RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                  __RPC__in_opt IUIAutomationFocusChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveFocusChangedEventHandler(__RPC__in_opt IUIAutomationFocusChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAllEventHandlers() = 0;
    virtual HRESULT STDMETHODCALLTYPE IntNativeArrayToSafeArray(__RPC__in_ecount_full(arrayCount) int *array, int arrayCount,
                                                                __RPC__deref_out_opt SAFEARRAY **safeArray) = 0;
    virtual HRESULT STDMETHODCALLTYPE IntSafeArrayToNativeArray(
        __RPC__in SAFEARRAY * intArray, __RPC__deref_out_ecount_full_opt(*arrayCount) int **array, __RPC__out int *arrayCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE RectToVariant(RECT rc, __RPC__out VARIANT * var) = 0;
    virtual HRESULT STDMETHODCALLTYPE VariantToRect(VARIANT var, __RPC__out RECT * rc) = 0;
    virtual HRESULT STDMETHODCALLTYPE SafeArrayToRectNativeArray(
        __RPC__in SAFEARRAY * rects, __RPC__deref_out_ecount_full_opt(*rectArrayCount) RECT * *rectArray, __RPC__out int *rectArrayCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateProxyFactoryEntry(__RPC__in_opt IUIAutomationProxyFactory * factory,
                                                              __RPC__deref_out_opt IUIAutomationProxyFactoryEntry * *factoryEntry) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ProxyFactoryMapping(__RPC__deref_out_opt IUIAutomationProxyFactoryMapping * *factoryMapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyProgrammaticName(PROPERTYID property, __RPC__deref_out_opt BSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPatternProgrammaticName(PATTERNID pattern, __RPC__deref_out_opt BSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE PollForPotentialSupportedPatterns(__RPC__in_opt IUIAutomationElement * pElement,
                                                                        __RPC__deref_out_opt SAFEARRAY * *patternIds,
                                                                        __RPC__deref_out_opt SAFEARRAY * *patternNames) = 0;
    virtual HRESULT STDMETHODCALLTYPE PollForPotentialSupportedProperties(__RPC__in_opt IUIAutomationElement * pElement,
                                                                          __RPC__deref_out_opt SAFEARRAY * *propertyIds,
                                                                          __RPC__deref_out_opt SAFEARRAY * *propertyNames) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckNotSupported(VARIANT value, __RPC__out BOOL * isNotSupported) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReservedNotSupportedValue(__RPC__deref_out_opt IUnknown * *notSupportedValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReservedMixedAttributeValue(__RPC__deref_out_opt IUnknown * *mixedAttributeValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromIAccessible(__RPC__in_opt IAccessible * accessible, int childId,
                                                             __RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromIAccessibleBuildCache(__RPC__in_opt IAccessible * accessible, int childId,
                                                                       __RPC__in_opt IUIAutomationCacheRequest *cacheRequest,
                                                                       __RPC__deref_out_opt IUIAutomationElement **element) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomation, 0x30cbe57d, 0xd9d0, 0x452a, 0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee)
#endif
#endif

#ifndef __IUIAutomationTreeWalker_INTERFACE_DEFINED__
#define __IUIAutomationTreeWalker_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomationTreeWalker, 0x4042c624, 0x389c, 0x4afc, 0xa6, 0x30, 0x9d, 0xf8, 0x54, 0xa5, 0x41, 0xfc);
MIDL_INTERFACE("4042c624-389c-4afc-a630-9df854a541fc")
IUIAutomationTreeWalker : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetParentElement(__RPC__in_opt IUIAutomationElement * element,
                                                       __RPC__deref_out_opt IUIAutomationElement * *parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstChildElement(__RPC__in_opt IUIAutomationElement * element,
                                                           __RPC__deref_out_opt IUIAutomationElement * *first) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastChildElement(__RPC__in_opt IUIAutomationElement * element,
                                                          __RPC__deref_out_opt IUIAutomationElement * *last) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextSiblingElement(__RPC__in_opt IUIAutomationElement * element,
                                                            __RPC__deref_out_opt IUIAutomationElement * *next) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreviousSiblingElement(__RPC__in_opt IUIAutomationElement * element,
                                                                __RPC__deref_out_opt IUIAutomationElement * *previous) = 0;
    virtual HRESULT STDMETHODCALLTYPE NormalizeElement(__RPC__in_opt IUIAutomationElement * element,
                                                       __RPC__deref_out_opt IUIAutomationElement * *normalized) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParentElementBuildCache(__RPC__in_opt IUIAutomationElement * element,
                                                                 __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                 __RPC__deref_out_opt IUIAutomationElement * *parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstChildElementBuildCache(__RPC__in_opt IUIAutomationElement * element,
                                                                     __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                     __RPC__deref_out_opt IUIAutomationElement * *first) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastChildElementBuildCache(__RPC__in_opt IUIAutomationElement * element,
                                                                    __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                    __RPC__deref_out_opt IUIAutomationElement * *last) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextSiblingElementBuildCache(__RPC__in_opt IUIAutomationElement * element,
                                                                      __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                      __RPC__deref_out_opt IUIAutomationElement * *next) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreviousSiblingElementBuildCache(__RPC__in_opt IUIAutomationElement * element,
                                                                          __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                          __RPC__deref_out_opt IUIAutomationElement * *previous) = 0;
    virtual HRESULT STDMETHODCALLTYPE NormalizeElementBuildCache(__RPC__in_opt IUIAutomationElement * element,
                                                                 __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
                                                                 __RPC__deref_out_opt IUIAutomationElement * *normalized) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Condition(__RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationTreeWalker, 0x4042c624, 0x389c, 0x4afc, 0xa6, 0x30, 0x9d, 0xf8, 0x54, 0xa5, 0x41, 0xfc)
#endif
#endif

DEFINE_GUID(CLSID_CUIAutomation, 0xff48dba4, 0x60ef, 0x4201, 0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e);

#endif

typedef class CUIAutomation CUIAutomation;

#ifndef __IUIAutomationInvokePattern_FWD_DEFINED__
#define __IUIAutomationInvokePattern_FWD_DEFINED__
typedef interface IUIAutomationInvokePattern IUIAutomationInvokePattern;
#ifdef __cplusplus
interface IUIAutomationInvokePattern;
#endif /* __cplusplus */
#endif

#ifndef __IUIAutomationTogglePattern_FWD_DEFINED__
#define __IUIAutomationTogglePattern_FWD_DEFINED__
typedef interface IUIAutomationTogglePattern IUIAutomationTogglePattern;
#ifdef __cplusplus
interface IUIAutomationTogglePattern;
#endif /* __cplusplus */
#endif

#ifndef __IUIAutomationCondition_FWD_DEFINED__
#define __IUIAutomationCondition_FWD_DEFINED__
typedef interface IUIAutomationCondition IUIAutomationCondition;
#ifdef __cplusplus
interface IUIAutomationCondition;
#endif /* __cplusplus */
#endif

/*****************************************************************************
 * IUIAutomationInvokePattern interface
 */
#ifndef __IUIAutomationInvokePattern_INTERFACE_DEFINED__
#define __IUIAutomationInvokePattern_INTERFACE_DEFINED__

DEFINE_GUID(IID_IUIAutomationInvokePattern, 0xfb377fbe, 0x8ea6, 0x46d5, 0x9c, 0x73, 0x64, 0x99, 0x64, 0x2d, 0x30, 0x59);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fb377fbe-8ea6-46d5-9c73-6499642d3059")
IUIAutomationInvokePattern : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Invoke() = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationInvokePattern, 0xfb377fbe, 0x8ea6, 0x46d5, 0x9c, 0x73, 0x64, 0x99, 0x64, 0x2d, 0x30, 0x59)
#endif
#else
typedef struct IUIAutomationInvokePatternVtbl
{
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT(STDMETHODCALLTYPE *QueryInterface)
    (
        IUIAutomationInvokePattern *This,
        REFIID riid,
        void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)
    (
        IUIAutomationInvokePattern *This);

    ULONG(STDMETHODCALLTYPE *Release)
    (
        IUIAutomationInvokePattern *This);

    /*** IUIAutomationInvokePattern methods ***/
    HRESULT(STDMETHODCALLTYPE *Invoke)
    (
        IUIAutomationInvokePattern *This);

    END_INTERFACE
} IUIAutomationInvokePatternVtbl;

interface IUIAutomationInvokePattern
{
    CONST_VTBL IUIAutomationInvokePatternVtbl *lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IUIAutomationInvokePattern_QueryInterface(This, riid, ppvObject) (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IUIAutomationInvokePattern_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IUIAutomationInvokePattern_Release(This) (This)->lpVtbl->Release(This)
/*** IUIAutomationInvokePattern methods ***/
#define IUIAutomationInvokePattern_Invoke(This) (This)->lpVtbl->Invoke(This)
#else
/*** IUnknown methods ***/
static __WIDL_INLINE HRESULT IUIAutomationInvokePattern_QueryInterface(IUIAutomationInvokePattern *This, REFIID riid, void **ppvObject)
{
    return This->lpVtbl->QueryInterface(This, riid, ppvObject);
}
static __WIDL_INLINE ULONG IUIAutomationInvokePattern_AddRef(IUIAutomationInvokePattern *This)
{
    return This->lpVtbl->AddRef(This);
}
static __WIDL_INLINE ULONG IUIAutomationInvokePattern_Release(IUIAutomationInvokePattern *This)
{
    return This->lpVtbl->Release(This);
}
/*** IUIAutomationInvokePattern methods ***/
static __WIDL_INLINE HRESULT IUIAutomationInvokePattern_Invoke(IUIAutomationInvokePattern *This)
{
    return This->lpVtbl->Invoke(This);
}
#endif
#endif

#endif

#endif /* __IUIAutomationInvokePattern_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IUIAutomationTogglePattern interface
 */
#ifndef __IUIAutomationTogglePattern_INTERFACE_DEFINED__
#define __IUIAutomationTogglePattern_INTERFACE_DEFINED__

DEFINE_GUID(IID_IUIAutomationTogglePattern, 0x94cf8058, 0x9b8d, 0x4ab9, 0x8b, 0xfd, 0x4c, 0xd0, 0xa3, 0x3c, 0x8c, 0x70);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("94cf8058-9b8d-4ab9-8bfd-4cd0a33c8c70")
IUIAutomationTogglePattern : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Toggle() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_CurrentToggleState(
        enum ToggleState * retVal) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_CachedToggleState(
        enum ToggleState * retVal) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationTogglePattern, 0x94cf8058, 0x9b8d, 0x4ab9, 0x8b, 0xfd, 0x4c, 0xd0, 0xa3, 0x3c, 0x8c, 0x70)
#endif
#else
typedef struct IUIAutomationTogglePatternVtbl
{
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT(STDMETHODCALLTYPE *QueryInterface)
    (
        IUIAutomationTogglePattern *This,
        REFIID riid,
        void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)
    (
        IUIAutomationTogglePattern *This);

    ULONG(STDMETHODCALLTYPE *Release)
    (
        IUIAutomationTogglePattern *This);

    /*** IUIAutomationTogglePattern methods ***/
    HRESULT(STDMETHODCALLTYPE *Toggle)
    (
        IUIAutomationTogglePattern *This);

    HRESULT(STDMETHODCALLTYPE *get_CurrentToggleState)
    (
        IUIAutomationTogglePattern *This,
        enum ToggleState *retVal);

    HRESULT(STDMETHODCALLTYPE *get_CachedToggleState)
    (
        IUIAutomationTogglePattern *This,
        enum ToggleState *retVal);

    END_INTERFACE
} IUIAutomationTogglePatternVtbl;

interface IUIAutomationTogglePattern
{
    CONST_VTBL IUIAutomationTogglePatternVtbl *lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IUIAutomationTogglePattern_QueryInterface(This, riid, ppvObject) (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IUIAutomationTogglePattern_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IUIAutomationTogglePattern_Release(This) (This)->lpVtbl->Release(This)
/*** IUIAutomationTogglePattern methods ***/
#define IUIAutomationTogglePattern_Toggle(This) (This)->lpVtbl->Toggle(This)
#define IUIAutomationTogglePattern_get_CurrentToggleState(This, retVal) (This)->lpVtbl->get_CurrentToggleState(This, retVal)
#define IUIAutomationTogglePattern_get_CachedToggleState(This, retVal) (This)->lpVtbl->get_CachedToggleState(This, retVal)
#else
/*** IUnknown methods ***/
static __WIDL_INLINE HRESULT IUIAutomationTogglePattern_QueryInterface(IUIAutomationTogglePattern *This, REFIID riid, void **ppvObject)
{
    return This->lpVtbl->QueryInterface(This, riid, ppvObject);
}
static __WIDL_INLINE ULONG IUIAutomationTogglePattern_AddRef(IUIAutomationTogglePattern *This)
{
    return This->lpVtbl->AddRef(This);
}
static __WIDL_INLINE ULONG IUIAutomationTogglePattern_Release(IUIAutomationTogglePattern *This)
{
    return This->lpVtbl->Release(This);
}
/*** IUIAutomationTogglePattern methods ***/
static __WIDL_INLINE HRESULT IUIAutomationTogglePattern_Toggle(IUIAutomationTogglePattern *This)
{
    return This->lpVtbl->Toggle(This);
}
static __WIDL_INLINE HRESULT IUIAutomationTogglePattern_get_CurrentToggleState(IUIAutomationTogglePattern *This, enum ToggleState *retVal)
{
    return This->lpVtbl->get_CurrentToggleState(This, retVal);
}
static __WIDL_INLINE HRESULT IUIAutomationTogglePattern_get_CachedToggleState(IUIAutomationTogglePattern *This, enum ToggleState *retVal)
{
    return This->lpVtbl->get_CachedToggleState(This, retVal);
}
#endif
#endif

#endif

#endif /* __IUIAutomationTogglePattern_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IUIAutomationCondition interface
 */
#ifndef __IUIAutomationCondition_INTERFACE_DEFINED__
#define __IUIAutomationCondition_INTERFACE_DEFINED__

DEFINE_GUID(IID_IUIAutomationCondition, 0x352ffba8, 0x0973, 0x437c, 0xa6, 0x1f, 0xf6, 0x4c, 0xaf, 0xd8, 0x1d, 0xf9);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("352ffba8-0973-437c-a61f-f64cafd81df9")
IUIAutomationCondition : public IUnknown{};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationCondition, 0x352ffba8, 0x0973, 0x437c, 0xa6, 0x1f, 0xf6, 0x4c, 0xaf, 0xd8, 0x1d, 0xf9)
#endif
#else
typedef struct IUIAutomationConditionVtbl
{
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT(STDMETHODCALLTYPE *QueryInterface)
    (
        IUIAutomationCondition *This,
        REFIID riid,
        void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)
    (
        IUIAutomationCondition *This);

    ULONG(STDMETHODCALLTYPE *Release)
    (
        IUIAutomationCondition *This);

    END_INTERFACE
} IUIAutomationConditionVtbl;

interface IUIAutomationCondition
{
    CONST_VTBL IUIAutomationConditionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IUIAutomationCondition_QueryInterface(This, riid, ppvObject) (This)->lpVtbl->QueryInterface(This, riid, ppvObject)
#define IUIAutomationCondition_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IUIAutomationCondition_Release(This) (This)->lpVtbl->Release(This)
#else
/*** IUnknown methods ***/
static __WIDL_INLINE HRESULT IUIAutomationCondition_QueryInterface(IUIAutomationCondition *This, REFIID riid, void **ppvObject)
{
    return This->lpVtbl->QueryInterface(This, riid, ppvObject);
}
static __WIDL_INLINE ULONG IUIAutomationCondition_AddRef(IUIAutomationCondition *This)
{
    return This->lpVtbl->AddRef(This);
}
static __WIDL_INLINE ULONG IUIAutomationCondition_Release(IUIAutomationCondition *This)
{
    return This->lpVtbl->Release(This);
}
#endif
#endif

#endif

#endif /* __IUIAutomationCondition_INTERFACE_DEFINED__ */

#pragma endregion

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
            va_list args;
            va_start(args, format);
            size_t size = std::vswprintf(nullptr, 0, format, args) + 1; // +1 for '\0'
            va_end(args);

            std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);

            va_start(args, format);
            std::vswprintf(buf.get(), size, format, args);
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

#define LOG_ERROR(format, ...) LOG(L"ERROR: " format, __VA_ARGS__)
#ifdef ENABLE_LOG_INFO
#define LOG_INFO(format, ...) LOG(L"INFO: " format, __VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif
#ifdef ENABLE_LOG_DEBUG
#define LOG_DEBUG(format, ...) LOG(L"DEBUG: " format, __VA_ARGS__)
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
};

// structure that wraps around what action should be done under which conditions
struct TriggerAction
{
    std::wstring mouseTriggerName;            // Mouse trigger parsed from settings - represents what kind of button click should be detected
    std::wstring actionName;                  // Name of the action parsed from settings
    uint32_t extepctedKeyModifiersState;      // expected state (bitmask) of the key modifiers that should be checked
    std::function<void(HWND)> actionExecutor; // function that executes the action
};

static struct
{
    bool oldTaskbarOnWin11;
    std::vector<TriggerAction> triggerActions;
} g_settings;

// wrapper to always call COM de-initialization
class COMInitializer
{
public:
    COMInitializer() : initialized(false) {}

    bool Init()
    {
        if (!initialized)
        {
            initialized = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
        }
        return initialized;
    }

    void Uninit()
    {
        if (initialized)
        {
            CoUninitialize();
            initialized = false;
            LOG(L"COM de-initialized");
        }
    }

    ~COMInitializer()
    {
        Uninit();
    }

    bool IsInitialized() { return initialized; }

protected:
    bool initialized;
} g_comInitializer;

// few helpers to ease up working with strings
namespace stringtools
{
    std::wstring ltrim(const std::wstring &s)
    {
        std::wstring result = s;
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](wchar_t ch)
                                                  { return !std::iswspace(ch); }));
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
static bool g_initialized = false;
static bool g_inputSiteProcHooked = false;

static HWND g_hTaskbarWnd;
static std::unordered_set<HWND> g_secondaryTaskbarWindows;

static com_ptr<IUIAutomation> g_pUIAutomation;
static com_ptr<IMMDeviceEnumerator> g_pDeviceEnumerator;

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

        // Note: The reason why UIAutomation interface is used is that it reliably returns a className of the element clicked.
        // If standard Windows API is used, the className returned is always Shell_TrayWnd which is a parrent window wrapping the taskbar.
        // From that we can't really tell reliably whether user clicked on the taskbar empty space or on some UI element on that taskbar, like
        // opened window, icon, start menu, etc.
        com_ptr<IUIAutomationElement> pWindowElement = NULL;
        if (FAILED(g_pUIAutomation->ElementFromPoint(position, pWindowElement.put())) || !pWindowElement)
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
    void push_back(const MouseClick &click)
    {
        clicks[currentIndex] = click;
        currentIndex = (currentIndex + 1) % 3;
    }

    const MouseClick &operator[](int i) const
    {
        int idx = 0;
        if (i > 0)
        {
            if (idx < size())
            {
                idx = (currentIndex + i) % 3; // oldest item always first
            }
            else
            {
                LOG_ERROR(L"Index out of bounds");
            }
        }
        else
        {
            if (idx >= -size())
            {
                idx = (currentIndex + i + 3) % 3; // -1 is the newest (last) item
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
        clicks[0] = MouseClick();
        clicks[1] = MouseClick();
        clicks[2] = MouseClick();
    }

    int size() const
    {
        return MAX_CLICKS;
    }

private:
    static const int MAX_CLICKS = 3;
    MouseClick clicks[MAX_CLICKS];
    int currentIndex;

} g_mouseClickQueue;

UINT_PTR gMouseClickTimer = (UINT_PTR)0;

// =====================================================================
// Forward declarations

KeyModifier GetKeyModifierFromName(const std::wstring &keyName);
bool IsTaskbarWindow(HWND hWnd);
bool IsSingleClick(const MouseClick::Button button);
bool IsDoubleClick(const MouseClick::Button button, const MouseClick &previousClick, const MouseClick &currentClick);
bool IsDoubleClick(const MouseClick::Button button);
bool IsTripleClick(const MouseClick::Button button);
bool IsSingleTap();
bool IsDoubleTap(const MouseClick &previousClick, const MouseClick &currentClick);
bool IsDoubleTap();
bool IsTripleTap();
bool IsKeyPressed(int vkCode);
void ExecuteTaskbarAction(const std::wstring &mouseTriggerName, const uint32_t numClicks);
void CALLBACK ProcessDelayedMouseClick(HWND, UINT, UINT_PTR, DWORD);
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

// proc handler for older Windows (nonXAML taskbar) versions
LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam,
                                           _In_ UINT_PTR uIdSubclass, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam))
    {
        RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
    }

    // LOG_DEBUG(L"Message: 0x%x", uMsg);
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam); // pass the message to the original wndproc (make e.g. right click work)

    const bool isLeftButton = (uMsg == WM_LBUTTONDOWN || uMsg == WM_NCLBUTTONDOWN);
    const bool isRightButton = (uMsg == WM_RBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN);
    const bool isMiddleButton = (uMsg == WM_MBUTTONDOWN || uMsg == WM_NCMBUTTONDOWN);
    const bool isLeftDoubleClick = (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_NCLBUTTONDBLCLK);
    const bool isRightDoubleClick = (uMsg == WM_RBUTTONDBLCLK || uMsg == WM_NCRBUTTONDBLCLK);
    const bool isMiddleDoubleClick = (uMsg == WM_MBUTTONDBLCLK || uMsg == WM_NCMBUTTONDBLCLK);
    if ((g_taskbarVersion == WIN_10_TASKBAR) &&
        (isLeftButton || isRightButton || isMiddleButton || isLeftDoubleClick || isRightDoubleClick || isMiddleDoubleClick))
    {
        MouseClick::Button button;
        if (isLeftButton || isLeftDoubleClick)
        {
            button = MouseClick::Button::LEFT;
        }
        else if (isRightButton || isRightDoubleClick)
        {
            button = MouseClick::Button::RIGHT;
        }
        else
        {
            button = MouseClick::Button::MIDDLE;
        }

        MouseClick::Type type = MouseClick::GetPointerType(wParam, lParam);
        OnMouseClick(MouseClick(wParam, lParam, type, button, hWnd));
        if (isLeftDoubleClick || isRightDoubleClick || isMiddleDoubleClick)
        {
            OnMouseClick(MouseClick(wParam, lParam, type, button, hWnd));
        }
    }
    else if (WM_NCDESTROY == uMsg)
    {
        if (hWnd != g_hTaskbarWnd)
        {
            g_secondaryTaskbarWindows.erase(hWnd);
        }
    }

    return result;
}

// proc handler for newer Windows versions (Windows 11 21H2 and newer)
WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // LOG_DEBUG(L"Message: 0x%x", uMsg);

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

            OnMouseClick(MouseClick(wParam, lParam, MouseClick::GetPointerType(wParam, 0), button, hRootWnd));
        }
        break;
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam); // pass the message to the original wndproc (make e.g. right click work)
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

    if (g_initialized)
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
    TaskBarButtonsState primaryTaskBarButtonsState1 = COMBINE_ALWAYS;
    TaskBarButtonsState primaryTaskBarButtonsState2 = COMBINE_NEVER;
    TaskBarButtonsState secondaryTaskBarButtonsState1 = COMBINE_ALWAYS;
    TaskBarButtonsState secondaryTaskBarButtonsState2 = COMBINE_NEVER;

    const auto argsSplit = SplitArgs(args);
    if (argsSplit.size() != 4)
    {
        LOG_ERROR(L"Invalid number of arguments for taskbar buttons state setting. "
                  "Expected format is: PRIMARY_STATE1;PRIMARY_STATE2;SECONDARY_STATE1;SECONDARY_STATE2");
    }

    for (const auto &arg : argsSplit)
    {
        if (arg == L"PRIMARY_COMBINE_ALWAYS")
        {
            primaryTaskBarButtonsState1 = COMBINE_ALWAYS;
        }
        else if (arg == L"PRIMARY_COMBINE_WHEN_FULL")
        {
            primaryTaskBarButtonsState1 = COMBINE_WHEN_FULL;
        }
        else if (arg == L"PRIMARY_COMBINE_NEVER")
        {
            primaryTaskBarButtonsState1 = COMBINE_NEVER;
        }
        else if (arg == L"SECONDARY_COMBINE_ALWAYS")
        {
            secondaryTaskBarButtonsState1 = COMBINE_ALWAYS;
        }
        else if (arg == L"SECONDARY_COMBINE_WHEN_FULL")
        {
            secondaryTaskBarButtonsState1 = COMBINE_WHEN_FULL;
        }
        else if (arg == L"SECONDARY_COMBINE_NEVER")
        {
            secondaryTaskBarButtonsState1 = COMBINE_NEVER;
        }
        else
        {
            LOG_ERROR(L"Unknown state '%s' for taskbar buttons state setting", arg.c_str());
        }
    }

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

    const auto argsSplit = SplitArgs(args);
    if (argsSplit.size() != 1)
    {
        LOG_ERROR(L"Invalid number of arguments for process setting. Expected format is: PROCESS_NAME");
        return L"";
    }

    return argsSplit[0];
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
    else if (actionName == L"ACTION_CTRL_ALT_TAB")
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
        auto [primaryState1, primaryState2, secondaryState1, secondaryState2] = ParseTaskBarButtonsState(args);
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
        triggerAction.extepctedKeyModifiersState = 0U;
        for (const auto &keyboardTrigger : keyboardTriggers)
        {
            if (keyboardTrigger == L"none")
                continue;

            KeyModifier keyModifier = GetKeyModifierFromName(keyboardTrigger);
            if (keyModifier != KEY_MODIFIER_INVALID)
            {
                SetBit(triggerAction.extepctedKeyModifiersState, keyModifier);
            }
        }
        triggerAction.mouseTriggerName = mouseTriggerStr;
        triggerAction.actionName = actionStr;
        triggerAction.actionExecutor = ParseMouseActionSetting(actionStr, additionalArgsStr);
        g_settings.triggerActions.push_back(triggerAction);
    }

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
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

    // Get the taskbar element from the last mouse click position
    com_ptr<IUIAutomationElement> pWindowElement = NULL;
    if (FAILED(g_pUIAutomation->ElementFromPoint(lastClick.position, pWindowElement.put())) || !pWindowElement)
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
        if (FAILED(g_pUIAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId,
                                                            _variant_t(static_cast<int>(UIA_ButtonControlTypeId)),
                                                            pControlTypeCondition.put())) ||
            !pControlTypeCondition)
        {
            LOG_ERROR(L"Failed to create ControlType condition for Start button search.");
            return false;
        }

        // Create Condition 2: ClassName == "Start"
        com_ptr<IUIAutomationCondition> pClassNameCondition = NULL;
        if (FAILED(g_pUIAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId,
                                                            _variant_t(L"Start"),
                                                            pClassNameCondition.put())) ||
            !pClassNameCondition)
        {
            LOG_ERROR(L"Failed to create ClassName condition for Start button search.");
            return false;
        }

        // Combine both conditions using AndCondition
        com_ptr<IUIAutomationCondition> pAndCondition = NULL;
        if (FAILED(g_pUIAutomation->CreateAndCondition(pControlTypeCondition.get(),
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
        if (FAILED(g_pUIAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId,
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

    if (!g_pDeviceEnumerator)
    {
        LOG_ERROR(L"Failed to toggle volume mute - device enumerator not initialized!");
        return;
    }
    LOG_INFO(L"Toggling volume mute");

    com_ptr<IMMDevice> defaultAudioDevice;
    if (SUCCEEDED(g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, defaultAudioDevice.put())))
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
    // get the initial state so that first click actually toggles to the other state (avoid switching to a state that is already set)
    static bool zigzagPrimary = (GetCombineTaskbarButtons(L"TaskbarGlomLevel") == primaryTaskBarButtonsState1);
    zigzagPrimary = !zigzagPrimary;
    shallNotify |= SetCombineTaskbarButtons(L"TaskbarGlomLevel",
                                            zigzagPrimary ? primaryTaskBarButtonsState1 : primaryTaskBarButtonsState2);
    static bool zigzagSecondary = (GetCombineTaskbarButtons(L"MMTaskbarGlomLevel") == secondaryTaskBarButtonsState1);
    zigzagSecondary = !zigzagSecondary;
    shallNotify |= SetCombineTaskbarButtons(L"MMTaskbarGlomLevel",
                                            zigzagSecondary ? secondaryTaskBarButtonsState1 : secondaryTaskBarButtonsState2);
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

    LOG_INFO(L"Starting process: %s", command.c_str());

    STARTUPINFO si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    if (!CreateProcess(NULL, (LPWSTR)command.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
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
    const MouseClick &previousClick = g_mouseClickQueue[-2];
    const MouseClick &currentClick = g_mouseClickQueue[-1];
    return IsDoubleClick(button, previousClick, currentClick);
}

bool IsTripleClick(const MouseClick::Button button)
{
    LOG_TRACE();
    return IsDoubleClick(button, g_mouseClickQueue[-2], g_mouseClickQueue[-1]) && IsDoubleClick(button, g_mouseClickQueue[-3], g_mouseClickQueue[-2]);
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

bool IsKeyPressed(int vkCode)
{
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
}

void CALLBACK ProcessDelayedMouseClick(HWND, UINT, UINT_PTR, DWORD)
{
    if (!KillTimer(NULL, gMouseClickTimer))
    {
        LOG_ERROR(L"Failed to kill triple click timer");
    }
    gMouseClickTimer = 0;

    // mouse left button clicks
    if (IsTripleClick(MouseClick::Button::LEFT)) // should never happen
    {
        ExecuteTaskbarAction(L"leftTriple", 3);
    }
    else if (IsDoubleClick(MouseClick::Button::LEFT))
    {
        ExecuteTaskbarAction(L"leftDouble", 2);
    }
    else if (IsSingleClick(MouseClick::Button::LEFT))
    {
        ExecuteTaskbarAction(L"left", 1);
    }

    // mouse right button clicks
    else if (IsTripleClick(MouseClick::Button::RIGHT)) // should never happen
    {
        ExecuteTaskbarAction(L"rightTriple", 3);
    }
    else if (IsDoubleClick(MouseClick::Button::RIGHT))
    {
        ExecuteTaskbarAction(L"rightDouble", 2);
    }
    else if (IsSingleClick(MouseClick::Button::RIGHT))
    {
        ExecuteTaskbarAction(L"right", 1);
    }

    // mouse middle button clicks
    else if (IsTripleClick(MouseClick::Button::MIDDLE)) // should never happen
    {
        ExecuteTaskbarAction(L"middleTriple", 3);
    }
    else if (IsDoubleClick(MouseClick::Button::MIDDLE))
    {
        ExecuteTaskbarAction(L"middleDouble", 2);
    }
    else if (IsSingleClick(MouseClick::Button::MIDDLE))
    {
        ExecuteTaskbarAction(L"middle", 1);
    }

    // touchscreen tap
    else if (IsTripleTap()) // should never happen
    {
        ExecuteTaskbarAction(L"tapTriple", 3);
    }
    else if (IsDoubleTap())
    {
        ExecuteTaskbarAction(L"tapDouble", 2);
    }
    else if (IsSingleTap())
    {
        ExecuteTaskbarAction(L"tapSingle", 1);
    }
}

void ExecuteTaskbarAction(const std::wstring &mouseTriggerName, const uint32_t numClicks)
{
    LOG_TRACE();

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
                allModifiersPressed &= (g_mouseClickQueue[-i].keyModifiersState == triggerAction.extepctedKeyModifiersState);
                LOG_DEBUG(L"Click %d key modifiers state: %u, expected: %u",
                          i, g_mouseClickQueue[-i].keyModifiersState, triggerAction.extepctedKeyModifiersState);
            }
            if (allModifiersPressed)
            {
                if (triggerAction.actionExecutor)
                {
                    LOG_INFO(L"Executing action: %s", triggerAction.actionName.c_str());
                    triggerAction.actionExecutor(hWnd);
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
    g_mouseClickQueue.clear();
}

// main body of the mod called every time a taskbar is clicked
bool OnMouseClick(MouseClick click)
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
    if (IsTripleClick(MouseClick::Button::LEFT))
    {
        ExecuteTaskbarAction(L"leftTriple", 3);
    }
    else if (IsTripleClick(MouseClick::Button::RIGHT))
    {
        ExecuteTaskbarAction(L"rightTriple", 3);
    }
    else if (IsTripleClick(MouseClick::Button::MIDDLE))
    {
        ExecuteTaskbarAction(L"middleTriple", 3);
    }
    else if (IsTripleTap())
    {
        ExecuteTaskbarAction(L"tapTriple", 3);
    }
    else
    {
        // single or double click -> chance to become double/triple click
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

    // init COM for UIAutomation and Volume control
    if (!g_comInitializer.Init())
    {
        LOG_ERROR(L"COM initialization failed, ModInit failed");
        return FALSE;
    }
    else
    {
        LOG_INFO(L"COM initilized");
    }

    // init COM interface for UIAutomation
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                g_pUIAutomation.put_void())) ||
        !g_pUIAutomation)
    {
        LOG_ERROR(L"Failed to create UIAutomation COM instance, ModInit failed");
        return FALSE; // UIAutomation is mandatory to find where the mouse clicked
    }
    else
    {
        LOG_INFO(L"UIAutomation COM initilized");
    }

    // init COM interface for Volume control
    const GUID XIID_IMMDeviceEnumerator = {0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
    const GUID XIID_MMDeviceEnumerator = {0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
    if (FAILED(CoCreateInstance(XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, XIID_IMMDeviceEnumerator,
                                g_pDeviceEnumerator.put_void())) ||
        !g_pDeviceEnumerator)
    {
        // this is not mandatory, if failed the volume mute feature will not be available
        LOG_ERROR(L"Failed to create DeviceEnumerator COM instance. Volume mute feature will not be available!");
    }
    else
    {
        LOG_INFO(L"DeviceEnumerator COM initilized");
    }

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

    g_initialized = true; // if not set the hook operations will not be applied after Windows startup

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
        UnsubclassTaskbarWindow(g_hTaskbarWnd);

        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows)
        {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }
    g_comInitializer.Uninit();
}
