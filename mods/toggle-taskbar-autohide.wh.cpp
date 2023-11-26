// ==WindhawkMod==
// @id              toggle-taskbar-autohide
// @name            Toggle taskbar autohide
// @description     Toggle taskbar autohide feature with middle mouse click on the taskbar.
// @version         1.0
// @author          m1lhaus
// @github          https://github.com/m1lhaus
// @include         explorer.exe
// @compilerOptions -DWINVER=0x0602 -lcomctl32 -ldwmapi -lole32 -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m1lhaus/windhawk-mods

// ==WindhawkModReadme==
/*
# Toggle taskbar autohide
Toggle taskbar autohide feature with middle mouse click on the taskbar. Whenever you middle click on a free space on your taskbar (outside other UI elemwnts on the taskbar), taskbar's autohide feature gets turned on or off.

![Demonstration](https://i.imgur.com/B6mtUj9.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- volumeIndicator: win11
  $name: Volume control indicator
  $options:
  - win11: Windows 11
  - modern: Windows 10
  - classic: Windows 7
  - none: None
- scrollArea: taskbar
  $name: Scroll area
  $options:
  - taskbar: The taskbar
  - notification_area: The notification area
- middleClickToMute: true
  $name: Middle click to mute
  $description: >-
    With this option enabled, middle clicking the volume tray icon will
    mute/unmute the system volume (Windows 11 version 22H2 or newer).
- noAutomaticMuteToggle: false
  $name: No automatic mute toggle
  $description: >-
    For the Windows 11 indicator, this option causes volume scrolling to be
    disabled when the volume is muted. For the None control indicator: By
    default, the output device is muted once the volume reaches zero, and is
    unmuted on any change to a non-zero volume. Enabling this option turns off
    this functionality, such that the device mute status is not changed.
- volumeChangeStep: 2
  $name: Volume change step
  $description: >-
    Allows to configure the volume change that will occur with each notch of
    mouse wheel movement. This option has effect only for the Windows 11, None
    control indicators. For the Windows 11 indicator, must be a multiple of 2.
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    Explorer Patcher or a similar tool). Note: For Windhawk versions older
    than 1.3, you have to disable and re-enable the mod to apply this option.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <dwmapi.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objbase.h>
#include <oleauto.h>
#include <uiautomation.h>
#include <windhawk_api.h>
#include <windowsx.h>
#include <winerror.h>
#include <winuser.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

#ifndef WM_POINTERDOWN
#define WM_POINTERDOWN 0x0246
#endif

// =====================================================================

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

typedef void* UIA_HWND;

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

enum SupportedTextSelection {
    SupportedTextSelection_None = 0,
    SupportedTextSelection_Single = 1,
    SupportedTextSelection_Multiple = 2
};

enum TextUnit {
    TextUnit_Character = 0,
    TextUnit_Format = 1,
    TextUnit_Word = 2,
    TextUnit_Line = 3,
    TextUnit_Paragraph = 4,
    TextUnit_Page = 5,
    TextUnit_Document = 6
};

enum TextPatternRangeEndpoint {
    TextPatternRangeEndpoint_Start = 0,
    TextPatternRangeEndpoint_End = 1
};

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

enum CaretPosition {
    CaretPosition_Unknown = 0,
    CaretPosition_EndOfLine = 1,
    CaretPosition_BeginningOfLine = 2
};

enum ToggleState {
    ToggleState_Off = 0,
    ToggleState_On = 1,
    ToggleState_Indeterminate = 2
};

enum RowOrColumnMajor {
    RowOrColumnMajor_RowMajor = 0,
    RowOrColumnMajor_ColumnMajor = 1,
    RowOrColumnMajor_Indeterminate = 2
};

enum TreeScope {
    TreeScope_None = 0,
    TreeScope_Element = 0x1,
    TreeScope_Children = 0x2,
    TreeScope_Descendants = 0x4,
    TreeScope_Parent = 0x8,
    TreeScope_Ancestors = 0x10,
    TreeScope_Subtree =
        TreeScope_Element | TreeScope_Children | TreeScope_Descendants
};

enum OrientationType {
    OrientationType_None = 0,
    OrientationType_Horizontal = 1,
    OrientationType_Vertical = 2
};

enum PropertyConditionFlags {
    PropertyConditionFlags_None = 0,
    PropertyConditionFlags_IgnoreCase = 1
};

enum WindowVisualState {
    WindowVisualState_Normal = 0,
    WindowVisualState_Maximized = 1,
    WindowVisualState_Minimized = 2
};

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
DEFINE_GUID(IID_IUIAutomationElement,
            0xd22108aa,
            0x8ac5,
            0x49a5,
            0x83,
            0x7b,
            0x37,
            0xbb,
            0xb3,
            0xd7,
            0x59,
            0x1e);
MIDL_INTERFACE("d22108aa-8ac5-49a5-837b-37bbb3d7591e")
IUIAutomationElement : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE SetFocus() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeId(
        __RPC__deref_out_opt SAFEARRAY * *runtimeId) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirst(
        enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
        __RPC__deref_out_opt IUIAutomationElement * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAll(
        enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
        __RPC__deref_out_opt IUIAutomationElementArray * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirstBuildCache(
        enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAllBuildCache(
        enum TreeScope scope, __RPC__in_opt IUIAutomationCondition * condition,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElementArray * *found) = 0;
    virtual HRESULT STDMETHODCALLTYPE BuildUpdatedCache(
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *updatedElement) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValue(
        PROPERTYID propertyId, __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValueEx(
        PROPERTYID propertyId, BOOL ignoreDefaultValue,
        __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValue(
        PROPERTYID propertyId, __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValueEx(
        PROPERTYID propertyId, BOOL ignoreDefaultValue,
        __RPC__out VARIANT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPatternAs(
        PATTERNID patternId, __RPC__in REFIID riid,
        __RPC__deref_out_opt void** patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPatternAs(
        PATTERNID patternId, __RPC__in REFIID riid,
        __RPC__deref_out_opt void** patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPattern(
        PATTERNID patternId,
        __RPC__deref_out_opt IUnknown * *patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPattern(
        PATTERNID patternId,
        __RPC__deref_out_opt IUnknown * *patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedParent(
        __RPC__deref_out_opt IUIAutomationElement * *parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedChildren(
        __RPC__deref_out_opt IUIAutomationElementArray * *children) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentProcessId(
        __RPC__out int* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentControlType(
        __RPC__out CONTROLTYPEID * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentLocalizedControlType(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentName(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAcceleratorKey(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAccessKey(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentHasKeyboardFocus(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsKeyboardFocusable(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsEnabled(__RPC__out BOOL *
                                                           retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAutomationId(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentClassName(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentHelpText(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentCulture(
        __RPC__out int* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsControlElement(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsContentElement(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsPassword(__RPC__out BOOL *
                                                            retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentNativeWindowHandle(
        __RPC__deref_out_opt UIA_HWND * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentItemType(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsOffscreen(__RPC__out BOOL *
                                                             retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentOrientation(
        __RPC__out enum OrientationType * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentFrameworkId(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsRequiredForForm(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentItemStatus(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentBoundingRectangle(
        __RPC__out RECT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentLabeledBy(
        __RPC__deref_out_opt IUIAutomationElement * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAriaRole(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAriaProperties(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsDataValidForForm(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentControllerFor(
        __RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentDescribedBy(
        __RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentFlowsTo(
        __RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentProviderDescription(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedProcessId(
        __RPC__out int* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedControlType(
        __RPC__out CONTROLTYPEID * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedLocalizedControlType(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedName(__RPC__deref_out_opt BSTR *
                                                     retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAcceleratorKey(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAccessKey(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedHasKeyboardFocus(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsKeyboardFocusable(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsEnabled(__RPC__out BOOL *
                                                          retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAutomationId(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedClassName(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedHelpText(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedCulture(
        __RPC__out int* retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsControlElement(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsContentElement(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsPassword(__RPC__out BOOL *
                                                           retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedNativeWindowHandle(
        __RPC__deref_out_opt UIA_HWND * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedItemType(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsOffscreen(__RPC__out BOOL *
                                                            retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedOrientation(
        __RPC__out enum OrientationType * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedFrameworkId(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsRequiredForForm(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedItemStatus(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedBoundingRectangle(
        __RPC__out RECT * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedLabeledBy(
        __RPC__deref_out_opt IUIAutomationElement * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAriaRole(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAriaProperties(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsDataValidForForm(
        __RPC__out BOOL * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedControllerFor(
        __RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedDescribedBy(
        __RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedFlowsTo(
        __RPC__deref_out_opt IUIAutomationElementArray * *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedProviderDescription(
        __RPC__deref_out_opt BSTR * retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClickablePoint(
        __RPC__out POINT * clickable, __RPC__out BOOL * gotClickable) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationElement,
                0xd22108aa,
                0x8ac5,
                0x49a5,
                0x83,
                0x7b,
                0x37,
                0xbb,
                0xb3,
                0xd7,
                0x59,
                0x1e)
#endif
#endif

#ifndef __IUIAutomation_INTERFACE_DEFINED__
#define __IUIAutomation_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomation,
            0x30cbe57d,
            0xd9d0,
            0x452a,
            0xab,
            0x13,
            0x7a,
            0xc5,
            0xac,
            0x48,
            0x25,
            0xee);
MIDL_INTERFACE("30cbe57d-d9d0-452a-ab13-7ac5ac4825ee")
IUIAutomation : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE CompareElements(
        __RPC__in_opt IUIAutomationElement * el1,
        __RPC__in_opt IUIAutomationElement * el2,
        __RPC__out BOOL * areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE CompareRuntimeIds(
        __RPC__in SAFEARRAY * runtimeId1, __RPC__in SAFEARRAY * runtimeId2,
        __RPC__out BOOL * areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElement(
        __RPC__deref_out_opt IUIAutomationElement * *root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandle(
        __RPC__in UIA_HWND hwnd,
        __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPoint(
        POINT pt, __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElement(
        __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElementBuildCache(
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandleBuildCache(
        __RPC__in UIA_HWND hwnd,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPointBuildCache(
        POINT pt, __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElementBuildCache(
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *element) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTreeWalker(
        __RPC__in_opt IUIAutomationCondition * pCondition,
        __RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewWalker(
        __RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewWalker(
        __RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewWalker(
        __RPC__deref_out_opt IUIAutomationTreeWalker * *walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewCondition(
        __RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewCondition(
        __RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewCondition(
        __RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCacheRequest(
        __RPC__deref_out_opt IUIAutomationCacheRequest * *cacheRequest) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTrueCondition(
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFalseCondition(
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePropertyCondition(
        PROPERTYID propertyId, VARIANT value,
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePropertyConditionEx(
        PROPERTYID propertyId, VARIANT value, enum PropertyConditionFlags flags,
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndCondition(
        __RPC__in_opt IUIAutomationCondition * condition1,
        __RPC__in_opt IUIAutomationCondition * condition2,
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndConditionFromArray(
        __RPC__in_opt SAFEARRAY * conditions,
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndConditionFromNativeArray(
        __RPC__in_ecount_full(conditionCount) IUIAutomationCondition *
            *conditions,
        int conditionCount,
        __RPC__deref_out_opt IUIAutomationCondition** newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrCondition(
        __RPC__in_opt IUIAutomationCondition * condition1,
        __RPC__in_opt IUIAutomationCondition * condition2,
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrConditionFromArray(
        __RPC__in_opt SAFEARRAY * conditions,
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrConditionFromNativeArray(
        __RPC__in_ecount_full(conditionCount) IUIAutomationCondition *
            *conditions,
        int conditionCount,
        __RPC__deref_out_opt IUIAutomationCondition** newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateNotCondition(
        __RPC__in_opt IUIAutomationCondition * condition,
        __RPC__deref_out_opt IUIAutomationCondition * *newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddAutomationEventHandler(
        EVENTID eventId, __RPC__in_opt IUIAutomationElement * element,
        enum TreeScope scope,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__in_opt IUIAutomationEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAutomationEventHandler(
        EVENTID eventId, __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPropertyChangedEventHandlerNativeArray(
        __RPC__in_opt IUIAutomationElement * element, enum TreeScope scope,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__in_opt IUIAutomationPropertyChangedEventHandler * handler,
        __RPC__in_ecount_full(propertyCount) PROPERTYID * propertyArray,
        int propertyCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPropertyChangedEventHandler(
        __RPC__in_opt IUIAutomationElement * element, enum TreeScope scope,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__in_opt IUIAutomationPropertyChangedEventHandler * handler,
        __RPC__in SAFEARRAY * propertyArray) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemovePropertyChangedEventHandler(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationPropertyChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddStructureChangedEventHandler(
        __RPC__in_opt IUIAutomationElement * element, enum TreeScope scope,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__in_opt IUIAutomationStructureChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveStructureChangedEventHandler(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationStructureChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddFocusChangedEventHandler(
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__in_opt IUIAutomationFocusChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveFocusChangedEventHandler(
        __RPC__in_opt IUIAutomationFocusChangedEventHandler * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAllEventHandlers() = 0;
    virtual HRESULT STDMETHODCALLTYPE IntNativeArrayToSafeArray(
        __RPC__in_ecount_full(arrayCount) int* array, int arrayCount,
        __RPC__deref_out_opt SAFEARRAY** safeArray) = 0;
    virtual HRESULT STDMETHODCALLTYPE IntSafeArrayToNativeArray(
        __RPC__in SAFEARRAY * intArray,
        __RPC__deref_out_ecount_full_opt(*arrayCount) int** array,
        __RPC__out int* arrayCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE RectToVariant(
        RECT rc, __RPC__out VARIANT * var) = 0;
    virtual HRESULT STDMETHODCALLTYPE VariantToRect(VARIANT var,
                                                    __RPC__out RECT * rc) = 0;
    virtual HRESULT STDMETHODCALLTYPE SafeArrayToRectNativeArray(
        __RPC__in SAFEARRAY * rects,
        __RPC__deref_out_ecount_full_opt(*rectArrayCount) RECT * *rectArray,
        __RPC__out int* rectArrayCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateProxyFactoryEntry(
        __RPC__in_opt IUIAutomationProxyFactory * factory,
        __RPC__deref_out_opt IUIAutomationProxyFactoryEntry *
            *factoryEntry) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ProxyFactoryMapping(
        __RPC__deref_out_opt IUIAutomationProxyFactoryMapping *
        *factoryMapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyProgrammaticName(
        PROPERTYID property, __RPC__deref_out_opt BSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPatternProgrammaticName(
        PATTERNID pattern, __RPC__deref_out_opt BSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE PollForPotentialSupportedPatterns(
        __RPC__in_opt IUIAutomationElement * pElement,
        __RPC__deref_out_opt SAFEARRAY * *patternIds,
        __RPC__deref_out_opt SAFEARRAY * *patternNames) = 0;
    virtual HRESULT STDMETHODCALLTYPE PollForPotentialSupportedProperties(
        __RPC__in_opt IUIAutomationElement * pElement,
        __RPC__deref_out_opt SAFEARRAY * *propertyIds,
        __RPC__deref_out_opt SAFEARRAY * *propertyNames) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckNotSupported(
        VARIANT value, __RPC__out BOOL * isNotSupported) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReservedNotSupportedValue(
        __RPC__deref_out_opt IUnknown * *notSupportedValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReservedMixedAttributeValue(
        __RPC__deref_out_opt IUnknown * *mixedAttributeValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromIAccessible(
        __RPC__in_opt IAccessible * accessible, int childId,
        __RPC__deref_out_opt IUIAutomationElement** element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromIAccessibleBuildCache(
        __RPC__in_opt IAccessible * accessible, int childId,
        __RPC__in_opt IUIAutomationCacheRequest* cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement** element) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomation,
                0x30cbe57d,
                0xd9d0,
                0x452a,
                0xab,
                0x13,
                0x7a,
                0xc5,
                0xac,
                0x48,
                0x25,
                0xee)
#endif
#endif

#ifndef __IUIAutomationTreeWalker_INTERFACE_DEFINED__
#define __IUIAutomationTreeWalker_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomationTreeWalker,
            0x4042c624,
            0x389c,
            0x4afc,
            0xa6,
            0x30,
            0x9d,
            0xf8,
            0x54,
            0xa5,
            0x41,
            0xfc);
MIDL_INTERFACE("4042c624-389c-4afc-a630-9df854a541fc")
IUIAutomationTreeWalker : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE GetParentElement(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__deref_out_opt IUIAutomationElement * *parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstChildElement(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__deref_out_opt IUIAutomationElement * *first) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastChildElement(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__deref_out_opt IUIAutomationElement * *last) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextSiblingElement(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__deref_out_opt IUIAutomationElement * *next) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreviousSiblingElement(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__deref_out_opt IUIAutomationElement * *previous) = 0;
    virtual HRESULT STDMETHODCALLTYPE NormalizeElement(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__deref_out_opt IUIAutomationElement * *normalized) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParentElementBuildCache(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstChildElementBuildCache(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *first) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastChildElementBuildCache(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *last) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextSiblingElementBuildCache(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *next) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreviousSiblingElementBuildCache(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *previous) = 0;
    virtual HRESULT STDMETHODCALLTYPE NormalizeElementBuildCache(
        __RPC__in_opt IUIAutomationElement * element,
        __RPC__in_opt IUIAutomationCacheRequest * cacheRequest,
        __RPC__deref_out_opt IUIAutomationElement * *normalized) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Condition(
        __RPC__deref_out_opt IUIAutomationCondition * *condition) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationTreeWalker,
                0x4042c624,
                0x389c,
                0x4afc,
                0xa6,
                0x30,
                0x9d,
                0xf8,
                0x54,
                0xa5,
                0x41,
                0xfc)
#endif
#endif

DEFINE_GUID(CLSID_CUIAutomation,
            0xff48dba4,
            0x60ef,
            0x4201,
            0xaa,
            0x87,
            0x54,
            0x10,
            0x3e,
            0xef,
            0x59,
            0x4e);

#endif

typedef class CUIAutomation CUIAutomation;

#pragma endregion

// =====================================================================

struct {
    int volumeIndicator;
    int scrollArea;
    bool middleClickToMute;
    bool noAutomaticMuteToggle;
    int volumeChangeStep;
    bool oldTaskbarOnWin11;
} g_settings;

enum {
    WIN_VERSION_UNSUPPORTED = 0,
    WIN_VERSION_7,
    WIN_VERSION_8,
    WIN_VERSION_81,
    WIN_VERSION_811,
    WIN_VERSION_10_T1,        // 1507
    WIN_VERSION_10_T2,        // 1511
    WIN_VERSION_10_R1,        // 1607
    WIN_VERSION_10_R2,        // 1703
    WIN_VERSION_10_R3,        // 1709
    WIN_VERSION_10_R4,        // 1803
    WIN_VERSION_10_R5,        // 1809
    WIN_VERSION_10_19H1,      // 1903, 1909
    WIN_VERSION_10_20H1,      // 2004, 20H2, 21H1, 21H2
    WIN_VERSION_SERVER_2022,  // Server 2022
    WIN_VERSION_11_21H2,
    WIN_VERSION_11_22H2,
};

struct SYMBOL_HOOK {
    std::vector<std::wstring_view> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

class UIAutomationWrapper {
   public:
    UIAutomationWrapper()
        : isCOMInitialized(false),
          isUIAutomationInitialized(false),
          uiAutomationInstance(NULL) {}

    ~UIAutomationWrapper() { deinit(); }

    bool isInitialized() const {
        return isUIAutomationInitialized && isCOMInitialized;
    }

    IUIAutomation* getInstance() const { return uiAutomationInstance; }

    bool init() {
        HRESULT result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if (SUCCEEDED(result)) {
            isCOMInitialized = true;
        } else {
            Wh_Log(L"CoInitializeEx failed\n");
            return false;
        }

        result = CoCreateInstance(CLSID_CUIAutomation, NULL,
                                  CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                  (void**)&uiAutomationInstance);
        if (SUCCEEDED(result)) {
            isUIAutomationInitialized = true;
        } else {
            Wh_Log(L"CoCreateInstance failed\n");
            return false;
        }
        Wh_Log(L"UIAutomationWrapper initilized\n");
        return true;
    }

    void deinit() {
        if (isUIAutomationInitialized) {
            uiAutomationInstance->Release();
            uiAutomationInstance = NULL;
            isUIAutomationInitialized = false;
        }

        if (isCOMInitialized) {
            CoUninitialize();
            isCOMInitialized = false;
        }
    }

   private:
    bool isCOMInitialized;
    bool isUIAutomationInitialized;

    IUIAutomation* uiAutomationInstance;
};

static int g_nWinVersion;
static int g_nExplorerVersion;
static HWND g_hTaskbarWnd;
static DWORD g_dwTaskbarThreadId;
static bool g_initialized = false;
static bool g_inputSiteProcHooked = false;
static std::unordered_set<HWND> g_secondaryTaskbarWindows;
static UIAutomationWrapper g_UIAutomation;

bool IsTaskbarWindow(HWND hWnd);
VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen);
BOOL WindowsVersionInit();
bool GetTaskbarAutohideState();
void SetTaskbarAutohide(bool enabled);
bool ToggleTaskbarAutohide();
bool OnMouseClick(HWND hWnd, WPARAM wParam, LPARAM lParam);

// =====================================================================

#pragma region hook_magic

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_taskbar-volume-control");

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
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
        [](int nCode, WPARAM wParam,
           LPARAM lParam) WINAPI_LAMBDA_RETURN(LRESULT) {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
                    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                        (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
                    param->result =
                        SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                          param->uIdSubclass, param->dwRefData);
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

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ UINT_PTR uIdSubclass,
                                           _In_ DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
    }

    LRESULT result = 0;
    switch (uMsg) {
        case WM_COPYDATA: {
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            typedef struct _notifyiconidentifier_internal {
                DWORD dwMagic;    // 0x34753423
                DWORD dwRequest;  // 1 for (x,y) | 2 for (w,h)
                DWORD cbSize;     // 0x20
                DWORD hWndHigh;
                DWORD hWndLow;
                UINT uID;
                GUID guidItem;
            } NOTIFYICONIDENTIFIER_INTERNAL;

            COPYDATASTRUCT* p_copydata = (COPYDATASTRUCT*)lParam;

            // Change Shell_NotifyIconGetRect handling result for the volume
            // icon. In case it's not visible, or in Windows 11, it returns 0,
            // which causes sndvol.exe to ignore the command line position.
            if (result == 0 && p_copydata->dwData == 0x03 &&
                p_copydata->cbData == sizeof(NOTIFYICONIDENTIFIER_INTERNAL)) {
                NOTIFYICONIDENTIFIER_INTERNAL* p_icon_ident =
                    (NOTIFYICONIDENTIFIER_INTERNAL*)p_copydata->lpData;
                if (p_icon_ident->dwMagic == 0x34753423 &&
                    (p_icon_ident->dwRequest == 0x01 ||
                     p_icon_ident->dwRequest == 0x02) &&
                    p_icon_ident->cbSize == 0x20 &&
                    memcmp(&p_icon_ident->guidItem,
                           "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4"
                           "\x1C\xB6\x7D\x5B\x9C",
                           sizeof(GUID)) == 0) {
                    RECT rc;
                    GetWindowRect(hWnd, &rc);

                    if (p_icon_ident->dwRequest == 0x01)
                        result = MAKEWORD(rc.left, rc.top);
                    else
                        result =
                            MAKEWORD(rc.right - rc.left, rc.bottom - rc.top);
                }
            }
            break;
        }

        case WM_POINTERDOWN:
            if (g_nExplorerVersion < WIN_VERSION_11_21H2 &&
                OnMouseClick(hWnd, wParam, lParam)) {
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

// taskbar event processing method from our hook that waits for mouse middle
// click
WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERDOWN:
            if (HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
                IsTaskbarWindow(hRootWnd) &&
                OnMouseClick(hRootWnd, wParam, lParam)) {
                return 0;
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

// hook to modern taskbar event processing
void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd ||
        !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }

    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    // At first, I tried to subclass the window instead of hooking its wndproc,
    // but the inputsite.dll code checks that the value wasn't changed, and
    // crashes otherwise.
    void* wndProc = (void*)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    Wh_SetFunctionHook(wndProc, (void*)InputSiteWindowProc_Hook,
                       (void**)&InputSiteWindowProc_Original);

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

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

// finds main task bar and returns its hWnd,
// optinally it finds also secondary taskbars and fills them to the set
// secondaryTaskbarWindows
HWND FindCurrentProcessTaskbarWindows(
    std::unordered_set<HWND>* secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
        std::unordered_set<HWND>* secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
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
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
            !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

bool HookSymbols(HMODULE module,
                 const SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount) {
    const WCHAR cacheVer = L'1';
    const WCHAR cacheSep = L'#';
    constexpr size_t cacheMaxSize = 10240;

    WCHAR moduleFilePath[MAX_PATH];
    if (!GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned an unsupported path");
        return false;
    }

    moduleFileName++;

    WCHAR cacheBuffer[cacheMaxSize + 1];
    std::wstring cacheStrKey = std::wstring(L"symbol-cache-") + moduleFileName;
    Wh_GetStringValue(cacheStrKey.c_str(), cacheBuffer, ARRAYSIZE(cacheBuffer));

    std::wstring_view cacheBufferView(cacheBuffer);

    // https://stackoverflow.com/a/46931770
    auto splitStringView = [](std::wstring_view s, WCHAR delimiter) {
        size_t pos_start = 0, pos_end;
        std::wstring_view token;
        std::vector<std::wstring_view> res;

        while ((pos_end = s.find(delimiter, pos_start)) !=
               std::wstring_view::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + 1;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    };

    auto cacheParts = splitStringView(cacheBufferView, cacheSep);

    std::vector<bool> symbolResolved(symbolHooksCount, false);
    std::wstring newSystemCacheStr;

    auto onSymbolResolved = [symbolHooks, symbolHooksCount, &symbolResolved,
                             &newSystemCacheStr,
                             module](std::wstring_view symbol, void* address) {
        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i]) {
                continue;
            }

            bool match = false;
            for (auto hookSymbol : symbolHooks[i].symbols) {
                if (hookSymbol == symbol) {
                    match = true;
                    break;
                }
            }

            if (!match) {
                continue;
            }

            if (symbolHooks[i].hookFunction) {
                Wh_SetFunctionHook(address, symbolHooks[i].hookFunction,
                                   symbolHooks[i].pOriginalFunction);
                Wh_Log(L"Hooked %p: %.*s", address, symbol.length(),
                       symbol.data());
            } else {
                *symbolHooks[i].pOriginalFunction = address;
                Wh_Log(L"Found %p: %.*s", address, symbol.length(),
                       symbol.data());
            }

            symbolResolved[i] = true;

            newSystemCacheStr += cacheSep;
            newSystemCacheStr += symbol;
            newSystemCacheStr += cacheSep;
            newSystemCacheStr +=
                std::to_wstring((ULONG_PTR)address - (ULONG_PTR)module);

            break;
        }
    };

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    newSystemCacheStr += cacheVer;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += timeStamp;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += imageSize;

    if (cacheParts.size() >= 3 &&
        cacheParts[0] == std::wstring_view(&cacheVer, 1) &&
        cacheParts[1] == timeStamp && cacheParts[2] == imageSize) {
        for (size_t i = 3; i + 1 < cacheParts.size(); i += 2) {
            auto symbol = cacheParts[i];
            auto address = cacheParts[i + 1];
            if (address.length() == 0) {
                continue;
            }

            void* addressPtr =
                (void*)(std::stoull(std::wstring(address), nullptr, 10) +
                        (ULONG_PTR)module);

            onSymbolResolved(symbol, addressPtr);
        }

        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i] || !symbolHooks[i].optional) {
                continue;
            }

            size_t noAddressMatchCount = 0;
            for (size_t j = 3; j + 1 < cacheParts.size(); j += 2) {
                auto symbol = cacheParts[j];
                auto address = cacheParts[j + 1];
                if (address.length() != 0) {
                    continue;
                }

                for (auto hookSymbol : symbolHooks[i].symbols) {
                    if (hookSymbol == symbol) {
                        noAddressMatchCount++;
                        break;
                    }
                }
            }

            if (noAddressMatchCount == symbolHooks[i].symbols.size()) {
                Wh_Log(L"Optional symbol %d doesn't exist (from cache)", i);
                symbolResolved[i] = true;
            }
        }

        if (std::all_of(symbolResolved.begin(), symbolResolved.end(),
                        [](bool b) { return b; })) {
            return true;
        }
    }

    Wh_Log(L"Couldn't resolve all symbols from cache");

    WH_FIND_SYMBOL findSymbol;
    HANDLE findSymbolHandle = Wh_FindFirstSymbol(module, nullptr, &findSymbol);
    if (!findSymbolHandle) {
        Wh_Log(L"Wh_FindFirstSymbol failed");
        return false;
    }

    do {
        onSymbolResolved(findSymbol.symbol, findSymbol.address);
    } while (Wh_FindNextSymbol(findSymbolHandle, &findSymbol));

    Wh_FindCloseSymbol(findSymbolHandle);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (symbolResolved[i]) {
            continue;
        }

        if (!symbolHooks[i].optional) {
            Wh_Log(L"Unresolved symbol: %d", i);
            return false;
        }

        Wh_Log(L"Optional symbol %d doesn't exist", i);

        for (auto hookSymbol : symbolHooks[i].symbols) {
            newSystemCacheStr += cacheSep;
            newSystemCacheStr += hookSymbol;
            newSystemCacheStr += cacheSep;
        }
    }

    if (newSystemCacheStr.length() <= cacheMaxSize) {
        Wh_SetStringValue(cacheStrKey.c_str(), newSystemCacheStr.c_str());
    } else {
        Wh_Log(L"Cache is too large (%Iu)", newSystemCacheStr.length());
    }

    return true;
}

bool GetTaskbarViewDllPath(WCHAR path[MAX_PATH]) {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    // Windows 11 version 22H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    // Windows 11 version 21H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    return false;
}

#pragma endregion  // hook_magic

// =====================================================================

#pragma region functions

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    HRSRC hResource;
    HGLOBAL hGlobal;
    void* pData;
    void* pFixedFileInfo;
    UINT uPtrLen;

    pFixedFileInfo = NULL;
    uPtrLen = 0;

    hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource != NULL) {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal != NULL) {
            pData = LockResource(hGlobal);
            if (pData != NULL) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = NULL;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

BOOL WindowsVersionInit() {
    g_nWinVersion = WIN_VERSION_UNSUPPORTED;

    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo)
        return FALSE;

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    WORD nQFE = LOWORD(pFixedFileInfo->dwFileVersionLS);

    switch (nMajor) {
        case 6:
            switch (nMinor) {
                case 1:
                    g_nWinVersion = WIN_VERSION_7;
                    break;

                case 2:
                    g_nWinVersion = WIN_VERSION_8;
                    break;

                case 3:
                    if (nQFE < 17000)
                        g_nWinVersion = WIN_VERSION_81;
                    else
                        g_nWinVersion = WIN_VERSION_811;
                    break;

                case 4:
                    g_nWinVersion = WIN_VERSION_10_T1;
                    break;
            }
            break;

        case 10:
            if (nBuild <= 10240)
                g_nWinVersion = WIN_VERSION_10_T1;
            else if (nBuild <= 10586)
                g_nWinVersion = WIN_VERSION_10_T2;
            else if (nBuild <= 14393)
                g_nWinVersion = WIN_VERSION_10_R1;
            else if (nBuild <= 15063)
                g_nWinVersion = WIN_VERSION_10_R2;
            else if (nBuild <= 16299)
                g_nWinVersion = WIN_VERSION_10_R3;
            else if (nBuild <= 17134)
                g_nWinVersion = WIN_VERSION_10_R4;
            else if (nBuild <= 17763)
                g_nWinVersion = WIN_VERSION_10_R5;
            else if (nBuild <= 18362)
                g_nWinVersion = WIN_VERSION_10_19H1;
            else if (nBuild <= 19041)
                g_nWinVersion = WIN_VERSION_10_20H1;
            else if (nBuild <= 20348)
                g_nWinVersion = WIN_VERSION_SERVER_2022;
            else if (nBuild <= 22000)
                g_nWinVersion = WIN_VERSION_11_21H2;
            else
                g_nWinVersion = WIN_VERSION_11_22H2;
            break;
    }

    if (g_nWinVersion == WIN_VERSION_UNSUPPORTED)
        return FALSE;

    return TRUE;
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

bool ToggleTaskbarAutohide() {
    const bool isEnabled = GetTaskbarAutohideState();
    SetTaskbarAutohide(!isEnabled);
    return !isEnabled;
}

#pragma endregion  // functions

// =====================================================================

// main body of the mod called every time a taskbar is clicked
bool OnMouseClick(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (GetCapture() != NULL) {
        return false;
    }

    if (IS_POINTER_THIRDBUTTON_WPARAM(wParam) == 0) {
        return false;
    }

    POINTER_INFO pointerInfo{};
    BOOL success = GetPointerInfo(GET_POINTERID_WPARAM(wParam), &pointerInfo);
    if (!success) {
        return false;
    }

    POINT pointerLocation = pointerInfo.ptPixelLocation;
    Wh_Log(L"Middle mouse clicked at x=%ld, y=%ld", pointerLocation.x,
           pointerLocation.y);

    IUIAutomation* pUIAutomation = g_UIAutomation.getInstance();
    if (pUIAutomation == NULL) {
        Wh_Log(L"UIAutomationWrapper failed to get instance\n");
        return false;
    }

    IUIAutomationElement* pWindowElementRaw = NULL;
    HRESULT hr =
        pUIAutomation->ElementFromPoint(pointerLocation, &pWindowElementRaw);
    if (FAILED(hr) || (pWindowElementRaw == NULL)) {
        Wh_Log(L"Failed to retrieve UI element from mouse click\n");
        return false;
    }

    BSTR currentAutomationId;
    hr = pWindowElementRaw->get_CurrentAutomationId(&currentAutomationId);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to retrieve the name of the UI element clicked.");
        pWindowElementRaw->Release();
        return false;
    }

    Wh_Log(L"Clicked element: %s", currentAutomationId);

    if (wcscmp(currentAutomationId, L"TaskbarFrame") == 0) {
        const bool isAutohideEnabled = ToggleTaskbarAutohide();
        Wh_Log(L"Setting taskbar autohide state to %s",
               isAutohideEnabled ? L"enabled" : L"disabled");
    }

    SysFreeString(currentAutomationId);      
    pWindowElementRaw->Release();
    return false;
}

////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (!WindowsVersionInit()) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    g_nExplorerVersion = g_nWinVersion;
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
        g_settings.oldTaskbarOnWin11) {
        g_nExplorerVersion = WIN_VERSION_10_20H1;
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (user32Module) {
        void* pCreateWindowInBand =
            (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
    }

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }

    if (!g_UIAutomation.init()) {
        Wh_Log(L"UIAutomationWrapper failed to initialize\n");
        return FALSE;
    }

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

    g_UIAutomation.deinit();
}
