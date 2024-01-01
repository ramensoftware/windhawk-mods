// ==WindhawkMod==
// @id              taskbar-empty-space-clicks
// @name            Click on empty taskbar space
// @description     Trigger custom action when empty space on a taskbar is double/middle clicked
// @version         1.0
// @author          m1lhaus
// @github          https://github.com/m1lhaus
// @include         explorer.exe
// @compilerOptions -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -lcomctl32 -loleaut32 -lole32 -lversion
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

This mod lets you assign an action to a mouse click on Windows taskbar. Double-click and middle-click actions are supported. This mod only modifies behaviour when empty space of the taskbar is clicked. Buttons, menus or other function of the taskbar are not affected. Both primary and secondary taskbars are supported.

## Supported actions:

1. **Show desktop** - Toggle show/hide desktop
2. **Ctrl+Alt+Tab** - Opens Ctrl+Alt+Tab dialog
3. **Task Manager** - Opens Windows default task manager
4. **Mute system volume** - Toggle mute of system volume (all sound)
5. **Taskbar auto-hide** - Toggle Windows taskbar auto-hide feature 
6. **Win+Tab** - Opens Win+Tab dialog
7. **Hide desktop icons** - Toggle show/hide of all desktop icons

## Example

Following animation shows **Taskbar auto-hide** feature. Feature gets toggled whenever user double-clicks the empty space on a taskbar.

![Demonstration of Toggle taskbar autohide mod for Windhawk](https://i.imgur.com/BRQrVnX.gif)

## Supported Windows versions are:
- Windows 10 22H2 (prior versions are not tested, but should work as well)
- Windows 11 21H2 - latest major

I will not supporting Insider preview or other minor versions of Windows. However, feel free to [report any issues](https://github.com/m1lhaus/windhawk-mods/issues) related to those versions. I'll appreciate the heads-up in advance.  

In case you are using old Windows taskbar on Windows 11 (**ExplorerPatcher** or a similar tool), enable corresponding option on Settings menu. This options will be tested only with the latest major version of Windows 11 (e.g. 23H2). 

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- doubleClickAction: ACTION_NOTHING
  $name: Double click on empty space
  $options:
  - ACTION_NOTHING: Nothing (default)
  - ACTION_SHOW_DESKTOP: Show desktop
  - ACTION_ALT_TAB: Ctrl+Alt+Tab
  - ACTION_TASK_MANAGER: Task Manager
  - ACTION_MUTE: Mute system volume
  - ACTION_TASKBAR_AUTOHIDE: Taskbar auto-hide
  - ACTION_WIN_TAB: Win+Tab
  - ACTION_HIDE_ICONS: Hide desktop icons 
- middleClickAction: ACTION_NOTHING
  $name: Middle click on empty space
  $options:
  - ACTION_NOTHING: Nothing (default)
  - ACTION_SHOW_DESKTOP: Show desktop
  - ACTION_ALT_TAB: Ctrl+Alt+Tab
  - ACTION_TASK_MANAGER: Task Manager
  - ACTION_MUTE: Mute system volume
  - ACTION_TASKBAR_AUTOHIDE: Taskbar auto-hide
  - ACTION_WIN_TAB: Win+Tab
  - ACTION_HIDE_ICONS: Hide desktop icons 
- oldTaskbarOnWin11: false
  $name: Use the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool). Note: For Windhawk versions older
    than 1.3, you have to disable and re-enable the mod to apply this option.
*/
// ==/WindhawkModSettings==

// Note: If intellisense is giving you a trouble, add -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 flags to compile_flags.txt (Ctrl+E).

#include <commctrl.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <windef.h>
#include <windhawk_api.h>
#include <winerror.h>
#include <winuser.h>
#include <windowsx.h>

#include <UIAnimation.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <string>
#include <unordered_set>
#include <vector>

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

// following include are taken from Qt project since builtin compiler is missing those definitions
#pragma region uiautomation_includes

#include <initguid.h>

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

enum SupportedTextSelection { SupportedTextSelection_None = 0, SupportedTextSelection_Single = 1, SupportedTextSelection_Multiple = 2 };

enum TextUnit {
    TextUnit_Character = 0,
    TextUnit_Format = 1,
    TextUnit_Word = 2,
    TextUnit_Line = 3,
    TextUnit_Paragraph = 4,
    TextUnit_Page = 5,
    TextUnit_Document = 6
};

enum TextPatternRangeEndpoint { TextPatternRangeEndpoint_Start = 0, TextPatternRangeEndpoint_End = 1 };

enum TextDecorationLineStyle {
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

enum CaretPosition { CaretPosition_Unknown = 0, CaretPosition_EndOfLine = 1, CaretPosition_BeginningOfLine = 2 };

enum ToggleState { ToggleState_Off = 0, ToggleState_On = 1, ToggleState_Indeterminate = 2 };

enum RowOrColumnMajor { RowOrColumnMajor_RowMajor = 0, RowOrColumnMajor_ColumnMajor = 1, RowOrColumnMajor_Indeterminate = 2 };

enum TreeScope {
    TreeScope_None = 0,
    TreeScope_Element = 0x1,
    TreeScope_Children = 0x2,
    TreeScope_Descendants = 0x4,
    TreeScope_Parent = 0x8,
    TreeScope_Ancestors = 0x10,
    TreeScope_Subtree = TreeScope_Element | TreeScope_Children | TreeScope_Descendants
};

enum OrientationType { OrientationType_None = 0, OrientationType_Horizontal = 1, OrientationType_Vertical = 2 };

enum PropertyConditionFlags { PropertyConditionFlags_None = 0, PropertyConditionFlags_IgnoreCase = 1 };

enum WindowVisualState { WindowVisualState_Normal = 0, WindowVisualState_Maximized = 1, WindowVisualState_Minimized = 2 };

enum WindowInteractionState {
    WindowInteractionState_Running = 0,
    WindowInteractionState_Closing = 1,
    WindowInteractionState_ReadyForUserInteraction = 2,
    WindowInteractionState_BlockedByModalWindow = 3,
    WindowInteractionState_NotResponding = 4
};

enum ExpandCollapseState {
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

struct UiaPoint {
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
IUIAutomationElement : public IUnknown {
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
IUIAutomation : public IUnknown {
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
IUIAutomationTreeWalker : public IUnknown {
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

#pragma endregion

// =====================================================================

enum TaskBarVersion { 
    WIN_10_TASKBAR = 0, 
    WIN_11_TASKBAR, 
    UNKNOWN_TASKBAR 
};

enum TaskBarAction {
    ACTION_NOTHING = 0,
    ACTION_SHOW_DESKTOP,
    ACTION_ALT_TAB,
    ACTION_TASK_MANAGER,
    ACTION_MUTE,
    ACTION_TASKBAR_AUTOHIDE,
    ACTION_WIN_TAB,
    ACTION_HIDE_ICONS
};

static struct {
    bool oldTaskbarOnWin11;
    TaskBarAction doubleClickTaskbarAction;
    TaskBarAction middleClickTaskbarAction;
} g_settings;

// wrapper to always call COM de-initialization
class COMInitializer {
  public:
    COMInitializer() : initialized(false) {}

    bool Init() {
        if (!initialized) {
            initialized = SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED));
        }
        return initialized;
    }

    void Uninit() {
        if (initialized) {
            CoUninitialize();
            initialized = false;
            Wh_Log(L"COM de-initialized");
        }
    }

    ~COMInitializer() {
        Uninit();
    }

    bool IsInitialized() { return initialized; }

protected:
    bool initialized;
} g_comInitializer;

static int nWinVersion;
static TaskBarVersion g_taskbarVersion = UNKNOWN_TASKBAR;

static DWORD g_dwTaskbarThreadId;
static bool g_initialized = false;
static bool g_inputSiteProcHooked = false;

static HWND g_hTaskbarWnd;
static std::unordered_set<HWND> g_secondaryTaskbarWindows;
static HWND g_hDesktopWnd;

static com_ptr<IUIAutomation> g_pUIAutomation;
static com_ptr<IMMDeviceEnumerator> g_pDeviceEnumerator;


// since the mod can't be split to multiple files, the definition order becomes somehow complicated
bool IsTaskbarWindow(HWND hWnd);
bool isMouseDoubleClick(LPARAM lParam);
bool OnMouseClick(HWND hWnd, WPARAM wParam, LPARAM lParam, TaskBarAction taskbarAction);

// =====================================================================

#pragma region hook_magic

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_toggle-taskbar-autohide");

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
        SUBCLASSPROC pfnSubclass;
        UINT_PTR uIdSubclass;
        DWORD_PTR dwRefData;
        BOOL result;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI_LAMBDA_RETURN(LRESULT) {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
                if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
                    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM *param = (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM *)cwp->lParam;
                    param->result = SetWindowSubclass(cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
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
                                           _In_ UINT_PTR uIdSubclass, _In_ DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
    }

    // Wh_Log(L"Message: 0x%x", uMsg);

    LRESULT result = 0;
    switch (uMsg) {
    // catch middle mouse button on both main and secondary taskbars
    case WM_NCMBUTTONDOWN:
    case WM_MBUTTONDOWN:
        if ((g_taskbarVersion == WIN_10_TASKBAR) && OnMouseClick(hWnd, wParam, lParam, g_settings.middleClickTaskbarAction)) {
            result = 0;
        } else {
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
        break;

    case WM_LBUTTONDBLCLK:
    case WM_NCLBUTTONDBLCLK:
        if ((g_taskbarVersion == WIN_10_TASKBAR) && OnMouseClick(hWnd, wParam, lParam, g_settings.doubleClickTaskbarAction)) {
            result = 0;
        } else {
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
        break;

    case WM_NCDESTROY:
        result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

        if (hWnd != g_hTaskbarWnd) {
            g_secondaryTaskbarWindows.erase(hWnd);
        }
        break;

    default:
        result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        break;
    }

    return result;
}

// proc handler for newer Windows versions (Windows 11 21H2 and newer)
WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Wh_Log(L"Message: 0x%x", uMsg);

    switch (uMsg) {
    case WM_POINTERDOWN:
        HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
        if (IsTaskbarWindow(hRootWnd)) {
            TaskBarAction action;
            if (IS_POINTER_THIRDBUTTON_WPARAM(wParam)) {
                action = g_settings.middleClickTaskbarAction;
            } else if (IS_POINTER_FIRSTBUTTON_WPARAM(wParam) && isMouseDoubleClick(lParam)) {
                action = g_settings.doubleClickTaskbarAction;
            } else {
                action = ACTION_NOTHING;
            }
            if(OnMouseClick(hRootWnd, wParam, lParam, action)) {
                return 0;
            }
        }
        break;
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

void SubclassTaskbarWindow(HWND hWnd) { 
    SetWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc, 0, 0); 
}

void UnsubclassTaskbarWindow(HWND hWnd) { 
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0); 
}

void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId || GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd || !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName, L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }

    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    // At first, I tried to subclass the window instead of hooking its wndproc,
    // but the inputsite.dll code checks that the value wasn't changed, and
    // crashes otherwise.
    void *wndProc = (void *)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    Wh_SetFunctionHook(wndProc, (void *)InputSiteWindowProc_Hook, (void **)&InputSiteWindowProc_Original);

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    SubclassTaskbarWindow(hWnd);
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }

    if ((g_taskbarVersion == WIN_11_TASKBAR) && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(hXamlIslandWnd, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId || GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);

    if ((g_taskbarVersion == WIN_11_TASKBAR) && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(hXamlIslandWnd, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

// finds main task bar and returns its hWnd,
// optinally it finds also secondary taskbars and fills them to the set
// secondaryTaskbarWindows
HWND FindCurrentProcessTaskbarWindows(std::unordered_set<HWND> *secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND *hWnd;
        std::unordered_set<HWND> *secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            ENUM_WINDOWS_PARAM &param = *(ENUM_WINDOWS_PARAM *)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId())
                return TRUE;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *param.hWnd = hWnd;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                param.secondaryTaskbarWindows->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI *)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth,
                                            int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance,
                                            lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if ((g_taskbarVersion == WIN_11_TASKBAR) && !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

#pragma endregion // hook_magic

#pragma region helper_defines
// taken from https://github.com/m417z/7-Taskbar-Tweaker/blob/6d529922a8609b68d6d4492d91dcc704c0dfbe4d/dll/functions.h

#define WIN_VERSION_UNSUPPORTED (-1)
#define WIN_VERSION_7 0
#define WIN_VERSION_8 1
#define WIN_VERSION_81 2
#define WIN_VERSION_811 3
#define WIN_VERSION_10_T1 4        // 1507
#define WIN_VERSION_10_T2 5        // 1511
#define WIN_VERSION_10_R1 6        // 1607
#define WIN_VERSION_10_R2 7        // 1703
#define WIN_VERSION_10_R3 8        // 1709
#define WIN_VERSION_10_R4 9        // 1803
#define WIN_VERSION_10_R5 10       // 1809
#define WIN_VERSION_10_19H1 11     // 1903, 1909
#define WIN_VERSION_10_20H1 12     // 2004, 20H2, 21H1, 21H2, 22H2
#define WIN_VERSION_SERVER_2022 13 // Server 2022
#define WIN_VERSION_11_21H2 14
#define WIN_VERSION_11_22H2 15

// helper macros
#define FIRST_NONEMPTY_ARG_2(a, b) \
                                   ( (sizeof(#a) > sizeof("")) ? (a+0) : (b) )
#define FIRST_NONEMPTY_ARG_3(a, b, c) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_2(b, c))
#define FIRST_NONEMPTY_ARG_4(a, b, c, d) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_3(b, c, d))
#define FIRST_NONEMPTY_ARG_5(a, b, c, d, e) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_4(b, c, d, e))
#define FIRST_NONEMPTY_ARG_6(a, b, c, d, e, f) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_5(b, c, d, e, f))
#define FIRST_NONEMPTY_ARG_7(a, b, c, d, e, f, g) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_6(b, c, d, e, f, g))
#define FIRST_NONEMPTY_ARG_8(a, b, c, d, e, f, g, h) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_7(b, c, d, e, f, g, h))
#define FIRST_NONEMPTY_ARG_9(a, b, c, d, e, f, g, h, i) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_8(b, c, d, e, f, g, h, i))
#define FIRST_NONEMPTY_ARG_10(a, b, c, d, e, f, g, h, i, j) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_9(b, c, d, e, f, g, h, i, j))
#define FIRST_NONEMPTY_ARG_11(a, b, c, d, e, f, g, h, i, j, k) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_10(b, c, d, e, f, g,  h, i, j, k))
#define FIRST_NONEMPTY_ARG_12(a, b, c, d, e, f, g, h, i, j, k, l) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_11(b, c, d, e, f, g, h, i, j, k, l))
#define FIRST_NONEMPTY_ARG_13(a, b, c, d, e, f, g, h, i, j, k, l, m) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_12(b, c, d, e, f, g, h, i, j, k, l, m))
#define FIRST_NONEMPTY_ARG_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_13(b, c, d, e, f, g, h, i, j, k, l, m, n))
#define FIRST_NONEMPTY_ARG_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_14(b, c, d, e, f, g, h, i, j, k, l, m, n, o))
#define FIRST_NONEMPTY_ARG_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_15(b, c, d, e, f, g, h, i, j, k, l, m, n, o, p))
#define FIRST_NONEMPTY_ARG_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) \
                                   FIRST_NONEMPTY_ARG_2(a, FIRST_NONEMPTY_ARG_16(b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q))

#define DO2(d7, dx)                ( (nWinVersion > WIN_VERSION_7) ? FIRST_NONEMPTY_ARG_2(dx, d7) : (d7) )
#define DO3(d7, d8, dx)            ( (nWinVersion > WIN_VERSION_8) ? FIRST_NONEMPTY_ARG_3(dx, d8, d7) : DO2(d7, d8) )
#define DO4(d7, d8, d81, dx)       ( (nWinVersion > WIN_VERSION_81) ? FIRST_NONEMPTY_ARG_4(dx, d81, d8, d7) : DO3(d7, d8, d81) )
#define DO5(d7, d8, d81, d811, dx) ( (nWinVersion > WIN_VERSION_811) ? FIRST_NONEMPTY_ARG_5(dx, d811, d81, d8, d7) : DO4(d7, d8, d81, d811) )
#define DO6(d7, d8, d81, d811, d10_t1, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_T1) ? \
                                     FIRST_NONEMPTY_ARG_6(dx, d10_t1, d811, d81, d8, d7) : \
                                     DO5(d7, d8, d81, d811, d10_t1) )
#define DO7(d7, d8, d81, d811, d10_t1, d10_t2, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_T2) ? \
                                     FIRST_NONEMPTY_ARG_7(dx, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO6(d7, d8, d81, d811, d10_t1, d10_t2) )
#define DO8(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_R1) ? \
                                     FIRST_NONEMPTY_ARG_8(dx, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO7(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1) )
#define DO9(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_R2) ? \
                                     FIRST_NONEMPTY_ARG_9(dx, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO8(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2) )
#define DO10(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_R3) ? \
                                     FIRST_NONEMPTY_ARG_10(dx, d10_r3, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO9(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3) )
#define DO11(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_R4) ? \
                                     FIRST_NONEMPTY_ARG_11(dx, d10_r4, d10_r3, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO10(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4) )
#define DO12(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_R5) ? \
                                     FIRST_NONEMPTY_ARG_12(dx, d10_r5, d10_r4, d10_r3, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO11(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5) )
#define DO13(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_19H1) ? \
                                     FIRST_NONEMPTY_ARG_13(dx, d10_19h1, d10_r5, d10_r4, d10_r3, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO12(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1) )
#define DO14(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1, dx) \
                                   ( (nWinVersion > WIN_VERSION_10_20H1) ? \
                                     FIRST_NONEMPTY_ARG_14(dx, d10_20h1, d10_19h1, d10_r5, d10_r4, d10_r3, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO13(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1) )
#define DO15(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1, ds_2022, dx) \
                                   ( (nWinVersion > WIN_VERSION_SERVER_2022) ? \
                                     FIRST_NONEMPTY_ARG_15(dx, ds_2022, d10_20h1, d10_19h1, d10_r5, d10_r4, d10_r3, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO14(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1, ds_2022) )
#define DO16(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1, ds_2022, d11_21h2, dx) \
                                   ( (nWinVersion > WIN_VERSION_11_21H2) ? \
                                     FIRST_NONEMPTY_ARG_16(dx, d11_21h2, ds_2022, d10_20h1, d10_19h1, d10_r5, d10_r4, d10_r3, d10_r2, d10_r1, d10_t2, d10_t1, d811, d81, d8, d7) : \
                                     DO15(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1, ds_2022, d11_21h2) )
#define DO17(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1, ds_2022, d11_21h2, d11_22h2, dx) \
                                   DO16(d7, d8, d81, d811, d10_t1, d10_t2, d10_r1, d10_r2, d10_r3, d10_r4, d10_r5, d10_19h1, d10_20h1, ds_2022, d11_21h2, d11_22h2)

#ifdef _WIN64
#define DEF3264(d32, d64)          (d64)
#else
#define DEF3264(d32, d64)          (d32)
#endif

#define DO2_3264(d7_32, d7_64, dx_32, dx_64) \
                                   DEF3264(DO2(d7_32, dx_32), \
                                           DO2(d7_64, dx_64))

#define DO3_3264(d7_32, d7_64, d8_32, d8_64, dx_32, dx_64) \
                                   DEF3264(DO3(d7_32, d8_32, dx_32), \
                                           DO3(d7_64, d8_64, dx_64))

#define DO4_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, dx_32, dx_64) \
                                   DEF3264(DO4(d7_32, d8_32, d81_32, dx_32), \
                                           DO4(d7_64, d8_64, d81_64, dx_64))

#define DO5_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, dx_32, dx_64) \
                                   DEF3264(DO5(d7_32, d8_32, d81_32, d811_32, dx_32), \
                                           DO5(d7_64, d8_64, d81_64, d811_64, dx_64))

#define DO6_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, dx_32, dx_64) \
                                   DEF3264(DO6(d7_32, d8_32, d81_32, d811_32, d10_t1_32, dx_32), \
                                           DO6(d7_64, d8_64, d81_64, d811_64, d10_t1_64, dx_64))

#define DO7_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, dx_32, dx_64) \
                                   DEF3264(DO7(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, dx_32), \
                                           DO7(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, dx_64))

#define DO8_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, dx_32, dx_64) \
                                   DEF3264(DO8(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, dx_32), \
                                           DO8(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, dx_64))

#define DO9_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, dx_32, dx_64) \
                                   DEF3264(DO9(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, dx_32), \
                                           DO9(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, dx_64))

#define DO10_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, dx_32, dx_64) \
                                   DEF3264(DO10(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, dx_32), \
                                           DO10(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, dx_64))

#define DO11_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, d10_r4_32, d10_r4_64, dx_32, dx_64) \
                                   DEF3264(DO11(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, d10_r4_32, dx_32), \
                                           DO11(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, d10_r4_64, dx_64))

#define DO12_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, d10_r4_32, d10_r4_64, d10_r5_32, d10_r5_64, dx_32, dx_64) \
                                   DEF3264(DO12(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, d10_r4_32, d10_r5_32, dx_32), \
                                           DO12(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, d10_r4_64, d10_r5_64, dx_64))

#define DO13_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, d10_r4_32, d10_r4_64, d10_r5_32, d10_r5_64, d10_19h1_32, d10_19h1_64, dx_32, dx_64) \
                                   DEF3264(DO13(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, d10_r4_32, d10_r5_32, d10_19h1_32, dx_32), \
                                           DO13(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, d10_r4_64, d10_r5_64, d10_19h1_64, dx_64))

#define DO14_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, d10_r4_32, d10_r4_64, d10_r5_32, d10_r5_64, d10_19h1_32, d10_19h1_64, d10_20h1_32, d10_20h1_64, dx_64) \
                                   DEF3264(DO13(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, d10_r4_32, d10_r5_32, d10_19h1_32, d10_20h1_32), \
                                           DO14(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, d10_r4_64, d10_r5_64, d10_19h1_64, d10_20h1_64, dx_64))

#define DO15_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, d10_r4_32, d10_r4_64, d10_r5_32, d10_r5_64, d10_19h1_32, d10_19h1_64, d10_20h1_32, d10_20h1_64, ds_2022_64, dx_64) \
                                   DEF3264(DO13(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, d10_r4_32, d10_r5_32, d10_19h1_32, d10_20h1_32), \
                                           DO15(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, d10_r4_64, d10_r5_64, d10_19h1_64, d10_20h1_64, ds_2022_64, dx_64))

#define DO16_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, d10_r4_32, d10_r4_64, d10_r5_32, d10_r5_64, d10_19h1_32, d10_19h1_64, d10_20h1_32, d10_20h1_64, ds_2022_64, d11_21h2_64, dx_64) \
                                   DEF3264(DO13(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, d10_r4_32, d10_r5_32, d10_19h1_32, d10_20h1_32), \
                                           DO16(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, d10_r4_64, d10_r5_64, d10_19h1_64, d10_20h1_64, ds_2022_64, d11_21h2_64, dx_64))

#define DO17_3264(d7_32, d7_64, d8_32, d8_64, d81_32, d81_64, d811_32, d811_64, d10_t1_32, d10_t1_64, d10_t2_32, d10_t2_64, d10_r1_32, d10_r1_64, d10_r2_32, d10_r2_64, d10_r3_32, d10_r3_64, d10_r4_32, d10_r4_64, d10_r5_32, d10_r5_64, d10_19h1_32, d10_19h1_64, d10_20h1_32, d10_20h1_64, ds_2022_64, d11_21h2_64, d11_22h2_64, dx_64) \
                                   DEF3264(DO13(d7_32, d8_32, d81_32, d811_32, d10_t1_32, d10_t2_32, d10_r1_32, d10_r2_32, d10_r3_32, d10_r4_32, d10_r5_32, d10_19h1_32, d10_20h1_32), \
                                           DO17(d7_64, d8_64, d81_64, d811_64, d10_t1_64, d10_t2_64, d10_r1_64, d10_r2_64, d10_r3_64, d10_r4_64, d10_r5_64, d10_19h1_64, d10_20h1_64, ds_2022_64, d11_21h2_64, d11_22h2_64, dx_64))

#pragma endregion

#pragma region functions

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 || _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

bool isMouseDoubleClick(LPARAM lParam) {
    static DWORD lastPointerDownTime = 0;
    static POINT lastPointerDownLocation = {0, 0};

    DWORD currentTime = GetTickCount();
    POINT currentLocation;
    currentLocation.x = GET_X_LPARAM(lParam);
    currentLocation.y = GET_Y_LPARAM(lParam);

    // Check if the current event is within the double-click time and distance
    bool result = false;
    if (abs(currentLocation.x - lastPointerDownLocation.x) <= GetSystemMetrics(SM_CXDOUBLECLK) &&
        abs(currentLocation.y - lastPointerDownLocation.y) <= GetSystemMetrics(SM_CYDOUBLECLK) &&
        ((currentTime - lastPointerDownTime) <= GetDoubleClickTime()))
    {
        result = true;
    }

    // Update the time and location of the last WM_POINTERDOWN event
    lastPointerDownTime = currentTime;
    lastPointerDownLocation = currentLocation;

    return result;
}

VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) {
    HRSRC hResource;
    HGLOBAL hGlobal;
    void *pData;
    void *pFixedFileInfo;
    UINT uPtrLen;

    pFixedFileInfo = NULL;
    uPtrLen = 0;

    hResource = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource != NULL) {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal != NULL) {
            pData = LockResource(hGlobal);
            if (pData != NULL) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0) {
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

BOOL WindowsVersionInit() {
    nWinVersion = WIN_VERSION_UNSUPPORTED;

    VS_FIXEDFILEINFO *pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo)
        return FALSE;

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    WORD nQFE = LOWORD(pFixedFileInfo->dwFileVersionLS);
    Wh_Log(L"Windows version (major.minor.build): %d.%d.%d", nMajor, nMinor, nBuild);

    // windows version needed for macros and functions ported from 7+Taskbar to work 
    switch (nMajor) {
    case 6:
        switch (nMinor) {
        case 1:
            nWinVersion = WIN_VERSION_7;
            break;

        case 2:
            nWinVersion = WIN_VERSION_8;
            break;

        case 3:
            if (nQFE < 17000)
                nWinVersion = WIN_VERSION_81;
            else
                nWinVersion = WIN_VERSION_811;
            break;

        case 4:
            nWinVersion = WIN_VERSION_10_T1;
            break;
        }
        break;

    case 10:
        if (nBuild <= 10240)
            nWinVersion = WIN_VERSION_10_T1;
        else if (nBuild <= 10586)
            nWinVersion = WIN_VERSION_10_T2;
        else if (nBuild <= 14393)
            nWinVersion = WIN_VERSION_10_R1;
        else if (nBuild <= 15063)
            nWinVersion = WIN_VERSION_10_R2;
        else if (nBuild <= 16299)
            nWinVersion = WIN_VERSION_10_R3;
        else if (nBuild <= 17134)
            nWinVersion = WIN_VERSION_10_R4;
        else if (nBuild <= 17763)
            nWinVersion = WIN_VERSION_10_R5;
        else if (nBuild <= 18362)
            nWinVersion = WIN_VERSION_10_19H1;
        else if (nBuild <= 19041)
            nWinVersion = WIN_VERSION_10_20H1;
        else if (nBuild <= 20348)
            nWinVersion = WIN_VERSION_SERVER_2022;
        else if (nBuild <= 22000)
            nWinVersion = WIN_VERSION_11_21H2;
        else {
            nWinVersion = WIN_VERSION_11_22H2;
        }
        break;
    }

    if (nMajor == 6) {
        g_taskbarVersion = WIN_10_TASKBAR;
    } else if (nMajor == 10) {
        if (nBuild < 22000) { // 21H2
            g_taskbarVersion = WIN_10_TASKBAR;
        } else {
            g_taskbarVersion = WIN_11_TASKBAR;
        }
    } else {
        g_taskbarVersion = UNKNOWN_TASKBAR;
    }

    return TRUE;
}

HWND GetWindows10ImmersiveWorkerWindow(void) {
    HWND hApplicationManagerDesktopShellWindow = FindWindow(L"ApplicationManager_DesktopShellWindow", NULL);
    if (!hApplicationManagerDesktopShellWindow)
        return NULL;

    LONG_PTR lpApplicationManagerDesktopShellWindow = GetWindowLongPtr(hApplicationManagerDesktopShellWindow, 0);
    if (!lpApplicationManagerDesktopShellWindow)
        return NULL;

    LONG_PTR lpImmersiveShellHookService =
        *(LONG_PTR *)(lpApplicationManagerDesktopShellWindow + DO9_3264(0, 0, , , , , , , 0x10, 0x20, , , , , , , 0x14, 0x28));
    if (!lpImmersiveShellHookService)
        return NULL;

    LONG_PTR lpImmersiveWindowMessageService =
        *(LONG_PTR *)(lpImmersiveShellHookService + DO10_3264(0, 0, , , , , , , 0x88, 0xE0, , , , , , , 0xA0, 0x108, 0x98, 0x100));
    if (!lpImmersiveWindowMessageService)
        return NULL;

    return *(HWND *)(lpImmersiveWindowMessageService + DO10_3264(0, 0, , , , , , , 0x4C, 0x80, , , 0x50, 0x88, , , 0x58, 0x98, 0x54, 0x90));
}

TaskBarAction ParseMouseActionSetting(const wchar_t *option) {
    const auto value = Wh_GetStringSetting(option);
    const auto equals = [](const wchar_t *str1, const wchar_t *str2) { return wcscmp(str1, str2) == 0; };

    TaskBarAction action = ACTION_NOTHING;
    if (equals(value, L"ACTION_NOTHING")) {
        action = ACTION_NOTHING;
    } else if (equals(value, L"ACTION_SHOW_DESKTOP")) {
        action = ACTION_SHOW_DESKTOP;
    } else if (equals(value, L"ACTION_ALT_TAB")) {
        action = ACTION_ALT_TAB;
    } else if (equals(value, L"ACTION_TASK_MANAGER")) {
        action = ACTION_TASK_MANAGER;
    } else if (equals(value, L"ACTION_MUTE")) {
        action = ACTION_MUTE;
    } else if (equals(value, L"ACTION_TASKBAR_AUTOHIDE")) {
        action = ACTION_TASKBAR_AUTOHIDE;
    } else if (equals(value, L"ACTION_WIN_TAB")) {
        action = ACTION_WIN_TAB;
    } else if (equals(value, L"ACTION_HIDE_ICONS")) {
        action = ACTION_HIDE_ICONS;
    } else {
        Wh_Log(L"Error: unknown action '%s' for option '%s'!", value, option);
        action = ACTION_NOTHING;
    }
    Wh_FreeStringSetting(value);
    Wh_Log(L"Selected '%s' option %d", option, action);

    return action;
}

void LoadSettings() {
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
    g_settings.doubleClickTaskbarAction = ParseMouseActionSetting(L"doubleClickAction");
    g_settings.middleClickTaskbarAction = ParseMouseActionSetting(L"middleClickAction");
}

bool GetTaskbarAutohideState() {
    if (g_hTaskbarWnd == NULL) {
        return false;
    }
    APPBARDATA msgData{};
    msgData.cbSize = sizeof(msgData);
    msgData.hWnd = g_hTaskbarWnd;
    LPARAM state = SHAppBarMessage(ABM_GETSTATE, &msgData);
    return state & ABS_AUTOHIDE;
}

void SetTaskbarAutohide(bool enabled) {
    if (g_hTaskbarWnd == NULL) {
        return;
    }

    APPBARDATA msgData{};
    msgData.cbSize = sizeof(msgData);
    msgData.hWnd = g_hTaskbarWnd;
    msgData.lParam = enabled ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &msgData);
}

bool FindDesktopWindow() {
    HWND hParentWnd = FindWindow(L"Progman", NULL);     // Program Manager window
    if (!hParentWnd) {
        Wh_Log(L"Failed to find Progman window");
        return false;
    }

    HWND hChildWnd = FindWindowEx(hParentWnd, NULL, L"SHELLDLL_DefView", NULL);     // parent window of the desktop
    if (!hChildWnd)
    {
        DWORD dwThreadId = GetWindowThreadProcessId(hParentWnd, NULL);
        EnumThreadWindows(dwThreadId, [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            WCHAR szClassName[16];
            if (GetClassName(hWnd, szClassName, _countof(szClassName)) == 0)
                return TRUE;

            if (lstrcmp(szClassName, L"WorkerW") != 0)
                return TRUE;

            HWND hChildWnd = FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
            if (!hChildWnd)
                return TRUE;

            *(HWND *)lParam = hChildWnd;
            return FALSE;
        }, (LPARAM)&hChildWnd);
    }

    if (!hChildWnd) {
        Wh_Log(L"Failed to find SHELLDLL_DefView window");
        return false;
    }
    g_hDesktopWnd = hChildWnd;
    return true;
}

#pragma endregion

#pragma region features

bool ToggleTaskbarAutohide() {
    const bool isEnabled = GetTaskbarAutohideState();
    Wh_Log(L"Setting taskbar autohide state to %s", !isEnabled ? L"enabled" : L"disabled");
    SetTaskbarAutohide(!isEnabled);
    return !isEnabled;
}

void ShowDesktop(HWND taskbarhWnd) {
    Wh_Log(L"Sending ShowDesktop message");
    SendMessage(taskbarhWnd, WM_COMMAND, MAKELONG(407, 0), 0);
}

void SendAltTab() {
    HWND hImmersiveWorkerWnd = GetWindows10ImmersiveWorkerWindow();
    if (hImmersiveWorkerWnd) {
        WPARAM wHotkeyIdentifier = DO9(0, , , , 47, , , , 45);
        Wh_Log(L"Sending AltTab message");
        PostMessage(hImmersiveWorkerWnd, WM_HOTKEY, wHotkeyIdentifier, MAKELPARAM(MOD_ALT | MOD_CONTROL, VK_TAB));
    }
}

void SendWinTab() {
    HWND hImmersiveWorkerWnd = GetWindows10ImmersiveWorkerWindow();
    if (hImmersiveWorkerWnd) {
        Wh_Log(L"Sending WinTab message");
        PostMessage(hImmersiveWorkerWnd, WM_HOTKEY, 11, MAKELPARAM(MOD_WIN, VK_TAB));
    }
}

void OpenTaskManager(HWND taskbarhWnd) {
    Wh_Log(L"Sending OpenTaskManager message");
    SendMessage(taskbarhWnd, WM_COMMAND, MAKELONG(420, 0), 0);
}

BOOL ToggleVolMuted() {
    IMMDevice *defaultDevice = NULL;
    IAudioEndpointVolume *endpointVolume = NULL;
    HRESULT hr;
    BOOL bMuted = FALSE;
    BOOL bSuccess = FALSE;

    const GUID XIID_IAudioEndpointVolume = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

    hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    if (SUCCEEDED(hr)) {
        hr = defaultDevice->Activate(XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
        if (SUCCEEDED(hr)) {
            if (SUCCEEDED(endpointVolume->GetMute(&bMuted))) {
                Wh_Log(L"Toggling volume mute");
                if (SUCCEEDED(endpointVolume->SetMute(!bMuted, NULL))) {
                    bSuccess = TRUE;
                }
            }
            endpointVolume->Release();
        }
        defaultDevice->Release();
    }

    return bSuccess;
}

void HideIcons() {
    if (g_hDesktopWnd != NULL) {
        Wh_Log(L"Sending hide icons message");
        PostMessage(g_hDesktopWnd, WM_COMMAND, 0x7402, 0);
    } 
}

#pragma endregion // functions

// =====================================================================

// main body of the mod called every time a taskbar is clicked
bool OnMouseClick(HWND hWnd, WPARAM wParam, LPARAM lParam, TaskBarAction taskbarAction) {
    if ((GetCapture() != NULL) || (taskbarAction == ACTION_NOTHING)) {
        return false;
    }

    // old Windows mouse handling of WM_MBUTTONDOWN message
    POINT pointerLocation{};
    if (g_taskbarVersion == WIN_10_TASKBAR) {
        // message carries mouse position relative to the client window so use GetCursorPos() instead
        if (!GetCursorPos(&pointerLocation)) {
            Wh_Log(L"Failed to get mouse position");
            return false;
        }

        // new Windows sends different message - WM_POINTERDOWN
    } else {
        pointerLocation.x = GET_X_LPARAM(lParam);
        pointerLocation.y = GET_Y_LPARAM(lParam);
    }
    Wh_Log(L"Mouse clicked at x=%ld, y=%ld", pointerLocation.x, pointerLocation.y);

    // Note: The reason why UIAutomation interface is used is that it reliably returns a className of the element clicked.
    // If standard Windows API is used, the className returned is always Shell_TrayWnd which is a parrent window wrapping the taskbar.
    // From that we can't really tell reliably whether user clicked on the taskbar empty space or on some UI element on that taskbar, like
    // opened window, icon, start menu, etc.

    com_ptr<IUIAutomationElement> pWindowElement = NULL;
    HRESULT hr = g_pUIAutomation->ElementFromPoint(pointerLocation, pWindowElement.put());
    if (FAILED(hr) || !pWindowElement) {
        Wh_Log(L"Failed to retrieve UI element from mouse click");
        return false;
    }

    bstr_ptr className;
    hr = pWindowElement->get_CurrentClassName(className.GetAddress());
    if (FAILED(hr) || !className) {
        Wh_Log(L"Failed to retrieve the Name of the UI element clicked.");
        return false;
    }
    Wh_Log(L"Clicked UI element ClassName: %s", className.GetBSTR());
    const bool taskbarClicked = (wcscmp(className.GetBSTR(), L"Shell_TrayWnd") == 0) ||                        // Windows 10 primary taskbar
                                (wcscmp(className.GetBSTR(), L"Shell_SecondaryTrayWnd") == 0) ||               // Windows 10 secondary taskbar
                                (wcscmp(className.GetBSTR(), L"Taskbar.TaskbarFrameAutomationPeer") == 0) ||   // Windows 11 taskbar
                                (wcscmp(className.GetBSTR(), L"Windows.UI.Input.InputSite.WindowClass") == 0); // Windows 11 21H2 taskbar
    if (!taskbarClicked) {
        return false;
    }

    if (taskbarAction == ACTION_SHOW_DESKTOP) {
        ShowDesktop(hWnd);
    } else if (taskbarAction == ACTION_ALT_TAB) {
        SendAltTab();
    } else if (taskbarAction == ACTION_TASK_MANAGER) {
        OpenTaskManager(hWnd);
    } else if (taskbarAction == ACTION_MUTE) {
        ToggleVolMuted();
    } else if (taskbarAction == ACTION_TASKBAR_AUTOHIDE) {
        (void)ToggleTaskbarAutohide();
    } else if (taskbarAction == ACTION_WIN_TAB) {
        SendWinTab();
    } else if (taskbarAction == ACTION_HIDE_ICONS) {
        HideIcons();
    } else {
        Wh_Log(L"Error: Unknown taskbar action '%d'", taskbarAction);
    }

    return false;
}

////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!WindowsVersionInit() || (g_taskbarVersion == UNKNOWN_TASKBAR)) {
        Wh_Log(L"Unsupported Windows version, ModInit failed");
        return FALSE;
    }
    // treat Windows 11 taskbar as on older windows
    if ((g_taskbarVersion == WIN_11_TASKBAR) && g_settings.oldTaskbarOnWin11) {
        g_taskbarVersion = WIN_10_TASKBAR;
    }
    Wh_Log(L"Using taskbar version: %d");

    // init COM for UIAutomation and Volume control
    if (!g_comInitializer.Init()) {
        Wh_Log(L"COM initialization failed, ModInit failed");
        return FALSE;
    } else {
        Wh_Log(L"COM initilized");
    }

    // init COM interface for UIAutomation
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), g_pUIAutomation.put_void())) || !g_pUIAutomation) {
        Wh_Log(L"Failed to create UIAutomation COM instance, ModInit failed");
        return FALSE;
    } else {
        Wh_Log(L"UIAutomation COM initilized");
    }

    // init COM interface for Volume control
    const GUID XIID_IMMDeviceEnumerator = {0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
    const GUID XIID_MMDeviceEnumerator = {0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
    if (FAILED(CoCreateInstance(XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, XIID_IMMDeviceEnumerator, g_pDeviceEnumerator.put_void())) || !g_pDeviceEnumerator) {
        Wh_Log(L"Failed to create DeviceEnumerator COM instance, ModInit failed");
        return FALSE;
    } else {
        Wh_Log(L"DeviceEnumerator COM initilized");
    }

    // optional feature, not critical for the mod
    if (!FindDesktopWindow()) {
        Wh_Log(L"Failed to find Desktop window. Hide icons feature will not be available!");
    }

    // hook magic
    Wh_SetFunctionHook((void *)CreateWindowExW, (void *)CreateWindowExW_Hook, (void **)&CreateWindowExW_Original);
    HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (user32Module) {
        void *pCreateWindowInBand = (void *)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand, (void *)CreateWindowInBand_Hook, (void **)&CreateWindowInBand_Original);
        }
    }
    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd = FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }

    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL *bReload) {
    Wh_Log(L">");
    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    LoadSettings();
    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);

        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }
    g_comInitializer.Uninit();
}
