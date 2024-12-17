// ==WindhawkMod==
// @id              taskbar-button-scroll
// @name            Taskbar minimize/restore on scroll
// @description     Minimize/restore by scrolling the mouse wheel over taskbar buttons and thumbnail previews (Windows 11 only)
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @compilerOptions -DWINVER=0x0602 -lcomctl32 -loleaut32 -lole32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar minimize/restore on scroll

Minimize/restore by scrolling the mouse wheel over taskbar buttons and thumbnail
previews.

Only Windows 11 is currently supported. For older Windows versions check out [7+
Taskbar Tweaker](https://tweaker.ramensoftware.com/).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scrollOverTaskbarButtons: true
  $name: Scroll over taskbar buttons
- scrollOverThumbnailPreviews: true
  $name: Scroll over thumbnail previews
- maximizeAndRestore: false
  $name: Maximize and restore
  $description: >-
    By default, the mod switches between minimize/restore states on scroll. This
    option switches to three states: minimize/restore/maximize.
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
*/
// ==/WindhawkModSettings==

#include <initguid.h>

#include <commctrl.h>
#include <comutil.h>
#include <uiautomation.h>
#include <windowsx.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#ifndef WM_POINTERWHEEL
#define WM_POINTERWHEEL 0x024E
#endif

struct {
    bool scrollOverTaskbarButtons;
    bool scrollOverThumbnailPreviews;
    bool maximizeAndRestore;
    bool reverseScrollingDirection;
} g_settings;

bool g_initialized = false;
bool g_inputSiteProcHooked = false;
WPARAM g_invokingTaskListElementOnPointWParam = 0;
WPARAM g_invokingContextMenuWParam = 0;
PVOID g_lastScrollTarget = nullptr;
DWORD g_lastScrollTime;
short g_lastScrollDeltaRemainder;
std::atomic<DWORD> g_groupMenuCommandThreadId;
ULONGLONG g_noDismissHoverUIUntil = 0;

HWND g_hTaskbarWnd;
DWORD g_dwTaskbarThreadId;
std::unordered_set<HWND> g_thumbnailWindows;

#pragma region uiaclientinterfaces_p

#ifndef __IUIAutomationInvokePattern_INTERFACE_DEFINED__
#define __IUIAutomationInvokePattern_INTERFACE_DEFINED__

// interface IUIAutomationInvokePattern
// [unique][uuid][object]

// {FB377FBE-8EA6-46D5-9C73-6499642D3059}
const IID IID_IUIAutomationInvokePattern = {
    0xFB377FBE,
    0x8EA6,
    0x46D5,
    {0x9C, 0x73, 0x64, 0x99, 0x64, 0x2D, 0x30, 0x59}};

MIDL_INTERFACE("fb377fbe-8ea6-46d5-9c73-6499642d3059")
IUIAutomationInvokePattern : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE Invoke(void) = 0;
};

#endif  // __IUIAutomationInvokePattern_INTERFACE_DEFINED__

enum TreeScope {
    TreeScope_Element = 0x1,
    TreeScope_Children = 0x2,
    TreeScope_Descendants = 0x4,
    TreeScope_Parent = 0x8,
    TreeScope_Ancestors = 0x10,
    TreeScope_Subtree =
        TreeScope_Element | TreeScope_Children | TreeScope_Descendants,
};

enum OrientationType {
    OrientationType_None = 0,
    OrientationType_Horizontal = 1,
    OrientationType_Vertical = 2,
};

enum PropertyConditionFlags {
    PropertyConditionFlags_None = 0x00,
    PropertyConditionFlags_IgnoreCase = 0x01,
};

typedef void* UIA_HWND;

// Source:
// https://github.com/qt/qtbase/blob/544464c3d173246e58d39351599b0ffa87ec43df/src/gui/accessible/windows/apisupport/uiaclientinterfaces_p.h
// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR
// GPL-2.0-only OR GPL-3.0-only

// clang-format off

#ifndef __IUIAutomationElement_INTERFACE_DEFINED__

using IUIAutomationCondition = IUnknown;
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
#endif   /* __IAccessible_FWD_DEFINED__ */

#define __IUIAutomationElement_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomationElement, 0xd22108aa, 0x8ac5, 0x49a5, 0x83,0x7b, 0x37,0xbb,0xb3,0xd7,0x59,0x1e);
MIDL_INTERFACE("d22108aa-8ac5-49a5-837b-37bbb3d7591e")
IUIAutomationElement : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetFocus() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRuntimeId(__RPC__deref_out_opt SAFEARRAY **runtimeId) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirst(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition *condition, __RPC__deref_out_opt IUIAutomationElement **found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAll(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition *condition, __RPC__deref_out_opt IUIAutomationElementArray **found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindFirstBuildCache(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition *condition, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **found) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindAllBuildCache(enum TreeScope scope, __RPC__in_opt IUIAutomationCondition *condition, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElementArray **found) = 0;
    virtual HRESULT STDMETHODCALLTYPE BuildUpdatedCache(__RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **updatedElement) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValue(PROPERTYID propertyId, __RPC__out VARIANT *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPropertyValueEx(PROPERTYID propertyId, BOOL ignoreDefaultValue, __RPC__out VARIANT *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValue(PROPERTYID propertyId, __RPC__out VARIANT *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPropertyValueEx(PROPERTYID propertyId, BOOL ignoreDefaultValue, __RPC__out VARIANT *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPatternAs(PATTERNID patternId, __RPC__in REFIID riid, __RPC__deref_out_opt void **patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPatternAs(PATTERNID patternId, __RPC__in REFIID riid, __RPC__deref_out_opt void **patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPattern(PATTERNID patternId, __RPC__deref_out_opt IUnknown **patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedPattern(PATTERNID patternId, __RPC__deref_out_opt IUnknown **patternObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedParent(__RPC__deref_out_opt IUIAutomationElement **parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCachedChildren(__RPC__deref_out_opt IUIAutomationElementArray **children) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentProcessId(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentControlType(__RPC__out CONTROLTYPEID *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentLocalizedControlType(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentName(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAcceleratorKey(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAccessKey(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentHasKeyboardFocus(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsKeyboardFocusable(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsEnabled(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAutomationId(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentClassName(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentHelpText(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentCulture(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsControlElement(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsContentElement(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsPassword(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentNativeWindowHandle(__RPC__deref_out_opt UIA_HWND *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentItemType(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsOffscreen(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentOrientation(__RPC__out enum OrientationType *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentFrameworkId(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsRequiredForForm(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentItemStatus(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentBoundingRectangle(__RPC__out RECT *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentLabeledBy(__RPC__deref_out_opt IUIAutomationElement **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAriaRole(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAriaProperties(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentIsDataValidForForm(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentControllerFor(__RPC__deref_out_opt IUIAutomationElementArray **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentDescribedBy(__RPC__deref_out_opt IUIAutomationElementArray **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentFlowsTo(__RPC__deref_out_opt IUIAutomationElementArray **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentProviderDescription(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedProcessId(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedControlType(__RPC__out CONTROLTYPEID *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedLocalizedControlType(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedName(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAcceleratorKey(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAccessKey(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedHasKeyboardFocus(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsKeyboardFocusable(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsEnabled(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAutomationId(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedClassName(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedHelpText(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedCulture(__RPC__out int *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsControlElement(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsContentElement(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsPassword(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedNativeWindowHandle(__RPC__deref_out_opt UIA_HWND *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedItemType(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsOffscreen(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedOrientation(__RPC__out enum OrientationType *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedFrameworkId(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsRequiredForForm(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedItemStatus(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedBoundingRectangle(__RPC__out RECT *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedLabeledBy(__RPC__deref_out_opt IUIAutomationElement **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAriaRole(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedAriaProperties(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedIsDataValidForForm(__RPC__out BOOL *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedControllerFor(__RPC__deref_out_opt IUIAutomationElementArray **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedDescribedBy(__RPC__deref_out_opt IUIAutomationElementArray **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedFlowsTo(__RPC__deref_out_opt IUIAutomationElementArray **retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachedProviderDescription(__RPC__deref_out_opt BSTR *retVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClickablePoint(__RPC__out POINT *clickable, __RPC__out BOOL *gotClickable) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationElement, 0xd22108aa, 0x8ac5, 0x49a5, 0x83,0x7b, 0x37,0xbb,0xb3,0xd7,0x59,0x1e)
#endif
#endif


#ifndef __IUIAutomation_INTERFACE_DEFINED__
#define __IUIAutomation_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomation, 0x30cbe57d, 0xd9d0, 0x452a, 0xab,0x13, 0x7a,0xc5,0xac,0x48,0x25,0xee);
MIDL_INTERFACE("30cbe57d-d9d0-452a-ab13-7ac5ac4825ee")
IUIAutomation : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE CompareElements(__RPC__in_opt IUIAutomationElement *el1, __RPC__in_opt IUIAutomationElement *el2, __RPC__out BOOL *areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE CompareRuntimeIds(__RPC__in SAFEARRAY * runtimeId1, __RPC__in SAFEARRAY * runtimeId2, __RPC__out BOOL *areSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElement(__RPC__deref_out_opt IUIAutomationElement **root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandle(__RPC__in UIA_HWND hwnd, __RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPoint(POINT pt, __RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElement(__RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRootElementBuildCache(__RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **root) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromHandleBuildCache(__RPC__in UIA_HWND hwnd, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromPointBuildCache(POINT pt, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFocusedElementBuildCache(__RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTreeWalker(__RPC__in_opt IUIAutomationCondition *pCondition, __RPC__deref_out_opt IUIAutomationTreeWalker **walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewWalker(__RPC__deref_out_opt IUIAutomationTreeWalker **walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewWalker(__RPC__deref_out_opt IUIAutomationTreeWalker **walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewWalker(__RPC__deref_out_opt IUIAutomationTreeWalker **walker) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RawViewCondition(__RPC__deref_out_opt IUIAutomationCondition **condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlViewCondition(__RPC__deref_out_opt IUIAutomationCondition **condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentViewCondition(__RPC__deref_out_opt IUIAutomationCondition **condition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCacheRequest(__RPC__deref_out_opt IUIAutomationCacheRequest **cacheRequest) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateTrueCondition(__RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFalseCondition(__RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePropertyCondition(PROPERTYID propertyId, VARIANT value, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePropertyConditionEx(PROPERTYID propertyId, VARIANT value, enum PropertyConditionFlags flags, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndCondition(__RPC__in_opt IUIAutomationCondition *condition1, __RPC__in_opt IUIAutomationCondition *condition2, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndConditionFromArray(__RPC__in_opt SAFEARRAY * conditions, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAndConditionFromNativeArray(__RPC__in_ecount_full(conditionCount) IUIAutomationCondition **conditions, int conditionCount, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrCondition(__RPC__in_opt IUIAutomationCondition *condition1, __RPC__in_opt IUIAutomationCondition *condition2, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrConditionFromArray(__RPC__in_opt SAFEARRAY * conditions, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateOrConditionFromNativeArray(__RPC__in_ecount_full(conditionCount) IUIAutomationCondition **conditions, int conditionCount, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateNotCondition(__RPC__in_opt IUIAutomationCondition *condition, __RPC__deref_out_opt IUIAutomationCondition **newCondition) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddAutomationEventHandler(EVENTID eventId, __RPC__in_opt IUIAutomationElement *element, enum TreeScope scope, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__in_opt IUIAutomationEventHandler *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAutomationEventHandler(EVENTID eventId, __RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationEventHandler *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPropertyChangedEventHandlerNativeArray(__RPC__in_opt IUIAutomationElement *element, enum TreeScope scope, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__in_opt IUIAutomationPropertyChangedEventHandler *handler, __RPC__in_ecount_full(propertyCount) PROPERTYID *propertyArray, int propertyCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddPropertyChangedEventHandler(__RPC__in_opt IUIAutomationElement *element, enum TreeScope scope, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__in_opt IUIAutomationPropertyChangedEventHandler *handler, __RPC__in SAFEARRAY * propertyArray) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemovePropertyChangedEventHandler(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationPropertyChangedEventHandler *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddStructureChangedEventHandler(__RPC__in_opt IUIAutomationElement *element, enum TreeScope scope, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__in_opt IUIAutomationStructureChangedEventHandler *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveStructureChangedEventHandler(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationStructureChangedEventHandler *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddFocusChangedEventHandler(__RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__in_opt IUIAutomationFocusChangedEventHandler *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveFocusChangedEventHandler(__RPC__in_opt IUIAutomationFocusChangedEventHandler *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAllEventHandlers() = 0;
    virtual HRESULT STDMETHODCALLTYPE IntNativeArrayToSafeArray(__RPC__in_ecount_full(arrayCount) int *array, int arrayCount, __RPC__deref_out_opt SAFEARRAY **safeArray) = 0;
    virtual HRESULT STDMETHODCALLTYPE IntSafeArrayToNativeArray(__RPC__in SAFEARRAY * intArray, __RPC__deref_out_ecount_full_opt(*arrayCount) int **array, __RPC__out int *arrayCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE RectToVariant(RECT rc, __RPC__out VARIANT *var) = 0;
    virtual HRESULT STDMETHODCALLTYPE VariantToRect(VARIANT var, __RPC__out RECT *rc) = 0;
    virtual HRESULT STDMETHODCALLTYPE SafeArrayToRectNativeArray(__RPC__in SAFEARRAY * rects, __RPC__deref_out_ecount_full_opt(*rectArrayCount) RECT **rectArray, __RPC__out int *rectArrayCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateProxyFactoryEntry(__RPC__in_opt IUIAutomationProxyFactory *factory, __RPC__deref_out_opt IUIAutomationProxyFactoryEntry **factoryEntry) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ProxyFactoryMapping(__RPC__deref_out_opt IUIAutomationProxyFactoryMapping **factoryMapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyProgrammaticName(PROPERTYID property, __RPC__deref_out_opt BSTR *name) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPatternProgrammaticName(PATTERNID pattern, __RPC__deref_out_opt BSTR *name) = 0;
    virtual HRESULT STDMETHODCALLTYPE PollForPotentialSupportedPatterns(__RPC__in_opt IUIAutomationElement *pElement, __RPC__deref_out_opt SAFEARRAY **patternIds, __RPC__deref_out_opt SAFEARRAY **patternNames) = 0;
    virtual HRESULT STDMETHODCALLTYPE PollForPotentialSupportedProperties(__RPC__in_opt IUIAutomationElement *pElement, __RPC__deref_out_opt SAFEARRAY **propertyIds, __RPC__deref_out_opt SAFEARRAY **propertyNames) = 0;
    virtual HRESULT STDMETHODCALLTYPE CheckNotSupported(VARIANT value, __RPC__out BOOL *isNotSupported) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReservedNotSupportedValue(__RPC__deref_out_opt IUnknown **notSupportedValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReservedMixedAttributeValue(__RPC__deref_out_opt IUnknown **mixedAttributeValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromIAccessible(__RPC__in_opt IAccessible *accessible, int childId, __RPC__deref_out_opt IUIAutomationElement **element) = 0;
    virtual HRESULT STDMETHODCALLTYPE ElementFromIAccessibleBuildCache(__RPC__in_opt IAccessible *accessible, int childId, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **element) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomation, 0x30cbe57d, 0xd9d0, 0x452a, 0xab,0x13, 0x7a,0xc5,0xac,0x48,0x25,0xee)
#endif
#endif


#ifndef __IUIAutomationTreeWalker_INTERFACE_DEFINED__
#define __IUIAutomationTreeWalker_INTERFACE_DEFINED__
DEFINE_GUID(IID_IUIAutomationTreeWalker, 0x4042c624, 0x389c, 0x4afc, 0xa6,0x30, 0x9d,0xf8,0x54,0xa5,0x41,0xfc);
MIDL_INTERFACE("4042c624-389c-4afc-a630-9df854a541fc")
IUIAutomationTreeWalker : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetParentElement(__RPC__in_opt IUIAutomationElement *element, __RPC__deref_out_opt IUIAutomationElement **parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstChildElement(__RPC__in_opt IUIAutomationElement *element, __RPC__deref_out_opt IUIAutomationElement **first) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastChildElement(__RPC__in_opt IUIAutomationElement *element, __RPC__deref_out_opt IUIAutomationElement **last) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextSiblingElement(__RPC__in_opt IUIAutomationElement *element, __RPC__deref_out_opt IUIAutomationElement **next) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreviousSiblingElement(__RPC__in_opt IUIAutomationElement *element, __RPC__deref_out_opt IUIAutomationElement **previous) = 0;
    virtual HRESULT STDMETHODCALLTYPE NormalizeElement(__RPC__in_opt IUIAutomationElement *element, __RPC__deref_out_opt IUIAutomationElement **normalized) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParentElementBuildCache(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **parent) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstChildElementBuildCache(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **first) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastChildElementBuildCache(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **last) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextSiblingElementBuildCache(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **next) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreviousSiblingElementBuildCache(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **previous) = 0;
    virtual HRESULT STDMETHODCALLTYPE NormalizeElementBuildCache(__RPC__in_opt IUIAutomationElement *element, __RPC__in_opt IUIAutomationCacheRequest *cacheRequest, __RPC__deref_out_opt IUIAutomationElement **normalized) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Condition(__RPC__deref_out_opt IUIAutomationCondition **condition) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IUIAutomationTreeWalker, 0x4042c624, 0x389c, 0x4afc, 0xa6,0x30, 0x9d,0xf8,0x54,0xa5,0x41,0xfc)
#endif
#endif

DEFINE_GUID(CLSID_CUIAutomation, 0xff48dba4, 0x60ef, 0x4201, 0xaa,0x87, 0x54,0x10,0x3e,0xef,0x59,0x4e);

// clang-format on

#pragma endregion  // uiaclientinterfaces_p

#pragma region offsets

void* CTaskListWnd__TaskCreated;

size_t OffsetFromAssembly(void* func,
                          size_t defValue,
                          std::string opcode = "mov",
                          int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        // Example: mov rax, [rcx+0xE0]
        std::regex regex(
            opcode +
            R"( r(?:[a-z]{2}|\d{1,2}), \[r(?:[a-z]{2}|\d{1,2})\+(0x[0-9A-F]+)\])");
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Failed for %p", func);
    return defValue;
}

PVOID* EV_MM_TASKLIST_TASK_ITEM_FILTER(PVOID lp) {
    static size_t offset =
        OffsetFromAssembly(CTaskListWnd__TaskCreated, 0x268, "mov", 40);

    return (PVOID*)((DWORD_PTR)lp + offset);
}

#pragma endregion  // offsets

using CTaskBtnGroup_GetGroup_t = PVOID(WINAPI*)(PVOID pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup_Original;

using CTaskGroup_GroupMenuCommand_t = HRESULT(WINAPI*)(PVOID pThis,
                                                       PVOID filter,
                                                       int command);
CTaskGroup_GroupMenuCommand_t CTaskGroup_GroupMenuCommand_Original;

using CTaskListWnd__HandleClick_t = void(WINAPI*)(PVOID pThis,
                                                  PVOID taskBtnGroup,
                                                  int taskItemIndex,
                                                  int clickAction,
                                                  int param4,
                                                  int param5);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;
void WINAPI CTaskListWnd__HandleClick_Hook(PVOID pThis,
                                           PVOID taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int param4,
                                           int param5) {
    Wh_Log(L"> %d", clickAction);

    if (!g_invokingTaskListElementOnPointWParam) {
        return CTaskListWnd__HandleClick_Original(
            pThis, taskBtnGroup, taskItemIndex, clickAction, param4, param5);
    }

    short delta =
        GET_WHEEL_DELTA_WPARAM(g_invokingTaskListElementOnPointWParam);

    if (g_lastScrollTarget == taskBtnGroup &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (g_settings.reverseScrollingDirection) {
        clicks = -clicks;
    }

    int command = 0;

    if (clicks > 0) {
        command = SC_RESTORE;
    } else if (clicks < 0) {
        command = SC_MINIMIZE;
    }

    if (command) {
        PVOID taskGroup = CTaskBtnGroup_GetGroup_Original(taskBtnGroup);
        if (taskGroup) {
            // Allows to steal focus.
            INPUT input;
            ZeroMemory(&input, sizeof(INPUT));
            SendInput(1, &input, sizeof(INPUT));

            g_groupMenuCommandThreadId = GetCurrentThreadId();
            CTaskGroup_GroupMenuCommand_Original(
                taskGroup, *EV_MM_TASKLIST_TASK_ITEM_FILTER(pThis), command);
            g_groupMenuCommandThreadId = 0;
        }
    }

    g_lastScrollTarget = taskBtnGroup;
    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

BOOL CanMinimizeWindow(HWND hWnd) {
    if (IsIconic(hWnd) || !IsWindowEnabled(hWnd))
        return FALSE;

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MINIMIZEBOX))
        return FALSE;

    if ((lWndStyle & (WS_CAPTION | WS_SYSMENU)) != (WS_CAPTION | WS_SYSMENU))
        return TRUE;

    HMENU hSystemMenu = GetSystemMenu(hWnd, FALSE);
    if (!hSystemMenu)
        return FALSE;

    UINT uMenuState = GetMenuState(hSystemMenu, SC_MINIMIZE, MF_BYCOMMAND);
    if (uMenuState == (UINT)-1)
        return TRUE;

    return ((uMenuState & MF_DISABLED) == FALSE);
}

BOOL CanMaximizeWindow(HWND hWnd) {
    if (!IsWindowEnabled(hWnd))
        return FALSE;

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MAXIMIZEBOX))
        return FALSE;

    return TRUE;
}

BOOL CanRestoreWindow(HWND hWnd) {
    if (!IsWindowEnabled(hWnd))
        return FALSE;

    long lWndStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (!(lWndStyle & WS_MAXIMIZEBOX))
        return FALSE;

    return TRUE;
}

bool MinimizeWithScroll(HWND hWnd) {
    if (g_settings.maximizeAndRestore && CanRestoreWindow(hWnd) &&
        IsZoomed(hWnd)) {
        SwitchToThisWindow(hWnd, TRUE);
        return PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    }

    if (CanMinimizeWindow(hWnd)) {
        DWORD dwProcessId;
        GetWindowThreadProcessId(hWnd, &dwProcessId);
        AllowSetForegroundWindow(dwProcessId);
        return PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }

    return true;
}

bool RestoreWithScroll(HWND hWnd) {
    if (g_settings.maximizeAndRestore && CanMaximizeWindow(hWnd) &&
        !IsIconic(hWnd) && !IsZoomed(hWnd)) {
        SwitchToThisWindow(hWnd, TRUE);
        return PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }

    SwitchToThisWindow(hWnd, TRUE);
    return true;
}

using CApi_PostMessageW_t = BOOL(
    WINAPI*)(PVOID pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CApi_PostMessageW_t CApi_PostMessageW_Original;
BOOL WINAPI CApi_PostMessageW_Hook(PVOID pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    Wh_Log(L">");

    if (g_groupMenuCommandThreadId == GetCurrentThreadId() &&
        Msg == WM_SYSCOMMAND) {
        switch (wParam) {
            case SC_MINIMIZE:
                return MinimizeWithScroll(hWnd);

            case SC_RESTORE:
                return RestoreWithScroll(hWnd);
        }
    }

    return CApi_PostMessageW_Original(pThis, hWnd, Msg, wParam, lParam);
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

bool InvokeTaskListElementOnPoint(POINT pt) {
    HRESULT hr;

    auto uia = winrt::create_instance<IUIAutomation>(CLSID_CUIAutomation);
    if (!uia) {
        return false;
    }

    winrt::com_ptr<IUIAutomationElement> element;
    hr = uia->ElementFromPoint(pt, element.put());
    if (FAILED(hr) || !element) {
        return false;
    }

    winrt::com_ptr<IUIAutomationCondition> trueCondition;
    hr = uia->CreateTrueCondition(trueCondition.put());
    if (FAILED(hr) || !trueCondition) {
        return false;
    }

    winrt::com_ptr<IUIAutomationTreeWalker> treeWalker;
    hr = uia->CreateTreeWalker(trueCondition.get(), treeWalker.put());
    if (FAILED(hr) || !treeWalker) {
        return false;
    }

    const std::wstring_view taskListButtonClassName =
        L"Taskbar.TaskListButtonAutomationPeer";

    while (true) {
        _bstr_t className;
        hr = element->get_CurrentClassName(className.GetAddress());
        if (SUCCEEDED(hr) && className.GetBSTR() == taskListButtonClassName) {
            break;
        }

        winrt::com_ptr<IUIAutomationElement> parentElement;
        hr = treeWalker->GetParentElement(element.get(), parentElement.put());
        if (FAILED(hr) || !parentElement) {
            return false;
        }

        element = std::move(parentElement);
    }

    winrt::com_ptr<IUIAutomationInvokePattern> patternInvoke;
    hr = element->GetCurrentPatternAs(UIA_InvokePatternId,
                                      IID_IUIAutomationInvokePattern,
                                      patternInvoke.put_void());
    if (FAILED(hr) || !patternInvoke) {
        return false;
    }

    hr = patternInvoke->Invoke();
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (!g_settings.scrollOverTaskbarButtons) {
        return false;
    }

    if (GetCapture()) {
        return false;
    }

    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    g_invokingTaskListElementOnPointWParam = wParam;
    bool result = InvokeTaskListElementOnPoint(pt);
    g_invokingTaskListElementOnPointWParam = 0;

    return result;
}

WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERWHEEL:
            if (HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
                IsTaskbarWindow(hRootWnd) &&
                OnMouseWheel(hRootWnd, wParam, lParam)) {
                return 0;
            }
            break;
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

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

    if (!g_inputSiteProcHooked) {
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

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_" WH_MOD_ID);

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
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
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

using CTaskListWnd_ShowLivePreview_t = HWND(WINAPI*)(PVOID pThis,
                                                     PVOID taskItem,
                                                     DWORD flags);
CTaskListWnd_ShowLivePreview_t CTaskListWnd_ShowLivePreview_Original;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(PVOID pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

void* CImmersiveTaskItem_vftable;

using CTaskListWnd_OnContextMenu_t = void(WINAPI*)(PVOID pThis,
                                                   POINT point,
                                                   HWND hWnd,
                                                   bool dontDismiss,
                                                   PVOID taskGroup,
                                                   PVOID taskItem);
CTaskListWnd_OnContextMenu_t CTaskListWnd_OnContextMenu_Original;
void WINAPI CTaskListWnd_OnContextMenu_Hook(PVOID pThis,
                                            POINT point,
                                            HWND hWnd,
                                            bool dontDismiss,
                                            PVOID taskGroup,
                                            PVOID taskItem) {
    if (!g_invokingContextMenuWParam) {
        return CTaskListWnd_OnContextMenu_Original(
            pThis, point, hWnd, dontDismiss, taskGroup, taskItem);
    }

    short delta = GET_WHEEL_DELTA_WPARAM(g_invokingContextMenuWParam);

    if (g_lastScrollTarget == taskItem &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (g_settings.reverseScrollingDirection) {
        clicks = -clicks;
    }

    if (clicks != 0) {
        HWND hTaskItemWnd;
        if (*(void**)taskItem == CImmersiveTaskItem_vftable) {
            hTaskItemWnd = CImmersiveTaskItem_GetWindow_Original(taskItem);
        } else {
            hTaskItemWnd = CWindowTaskItem_GetWindow_Original(taskItem);
        }

        if (hTaskItemWnd) {
            CTaskListWnd_ShowLivePreview_Original(pThis, nullptr, 0);
            g_noDismissHoverUIUntil = GetTickCount64() + 400;

            KillTimer(hWnd, 2006);

            if (clicks > 0) {
                RestoreWithScroll(hTaskItemWnd);
            } else if (clicks < 0) {
                MinimizeWithScroll(hTaskItemWnd);
            }
        }
    }

    g_lastScrollTarget = taskItem;
    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

using CTaskListWnd_DismissHoverUI_t = HRESULT(WINAPI*)(PVOID pThi);
CTaskListWnd_DismissHoverUI_t CTaskListWnd_DismissHoverUI_Original;
HRESULT WINAPI CTaskListWnd_DismissHoverUI_Hook(PVOID pThis) {
    if (GetTickCount64() < g_noDismissHoverUIUntil) {
        return 0;
    }

    return CTaskListWnd_DismissHoverUI_Original(pThis);
}

using CTaskListThumbnailWnd__HandleContextMenu_t = void(WINAPI*)(PVOID pThis,
                                                                 POINT point,
                                                                 int param2);
CTaskListThumbnailWnd__HandleContextMenu_t
    CTaskListThumbnailWnd__HandleContextMenu_Original;

bool OnThumbnailWheelScroll(HWND hWnd,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam) {
    if (!g_settings.scrollOverThumbnailPreviews) {
        return false;
    }

    PVOID thumbnail = (PVOID)GetWindowLongPtr(hWnd, 0);
    if (!thumbnail) {
        return false;
    }

    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    g_invokingContextMenuWParam = wParam;
    CTaskListThumbnailWnd__HandleContextMenu_Original(thumbnail, pt, 0);
    g_invokingContextMenuWParam = 0;

    return true;
}

LRESULT CALLBACK ThumbnailWindowSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             UINT_PTR uIdSubclass,
                                             DWORD_PTR dwRefData) {
    LRESULT result = 0;

    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, ThumbnailWindowSubclassProc, 0);
    }

    switch (uMsg) {
        case WM_MOUSEWHEEL:
            if (!OnThumbnailWheelScroll(hWnd, uMsg, wParam, lParam)) {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_NCDESTROY:
            g_thumbnailWindows.erase(hWnd);

            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;

        default:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;
    }

    return result;
}

void SubclassThumbnailWindow(HWND hWnd) {
    SetWindowSubclassFromAnyThread(hWnd, ThumbnailWindowSubclassProc, 0, 0);
}

void UnsubclassThumbnailWindow(HWND hWnd) {
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void HandleIdentifiedThumbnailWindow(HWND hWnd) {
    g_thumbnailWindows.insert(hWnd);
    SubclassThumbnailWindow(hWnd);
}

void FindCurrentProcessThumbnailWindows(HWND hTaskbarWnd) {
    DWORD dwProcessId;
    DWORD dwThreadId = GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId);

    EnumThreadWindows(
        dwThreadId,
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"TaskListThumbnailWnd") == 0) {
                g_thumbnailWindows.insert(hWnd);
            }

            return TRUE;
        },
        0);
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
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"TaskListThumbnailWnd") == 0) {
        Wh_Log(L"Thumbnail window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedThumbnailWindow(hWnd);
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
                                           PVOID lpParam,
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
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (!g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"TaskListThumbnailWnd") == 0) {
        Wh_Log(L"Thumbnail window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedThumbnailWindow(hWnd);
    }

    return hWnd;
}

struct SYMBOL_HOOK {
    std::vector<std::wstring_view> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

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
        Wh_Log(L"Cache is too large (%zu)", newSystemCacheStr.length());
    }

    return true;
}

void LoadSettings() {
    g_settings.scrollOverTaskbarButtons =
        Wh_GetIntSetting(L"scrollOverTaskbarButtons");
    g_settings.scrollOverThumbnailPreviews =
        Wh_GetIntSetting(L"scrollOverThumbnailPreviews");
    g_settings.maximizeAndRestore = Wh_GetIntSetting(L"maximizeAndRestore");
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            (void**)&CTaskBtnGroup_GetGroup_Original,
        },
        {
            {LR"(public: virtual long __cdecl CTaskGroup::GroupMenuCommand(struct ITaskItemFilter *,int))"},
            (void**)&CTaskGroup_GroupMenuCommand_Original,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            (void**)&CTaskListWnd__HandleClick_Original,
            (void*)CTaskListWnd__HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CApi::PostMessageW(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            (void**)&CApi_PostMessageW_Original,
            (void*)CApi_PostMessageW_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::ShowLivePreview(struct ITaskItem *,unsigned long))"},
            (void**)&CTaskListWnd_ShowLivePreview_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            (void**)&CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            (void**)&CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            (void**)&CImmersiveTaskItem_vftable,
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::OnContextMenu(struct tagPOINT,struct HWND__ *,bool,struct ITaskGroup *,struct ITaskItem *))"},
            (void**)&CTaskListWnd_OnContextMenu_Original,
            (void*)CTaskListWnd_OnContextMenu_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::DismissHoverUI(int))"},
            (void**)&CTaskListWnd_DismissHoverUI_Original,
            (void*)CTaskListWnd_DismissHoverUI_Hook,
        },
        {
            {LR"(private: void __cdecl CTaskListThumbnailWnd::_HandleContextMenu(struct tagPOINT,int))"},
            (void**)&CTaskListThumbnailWnd__HandleContextMenu_Original,
        },
        // For offsets:
        {
            {LR"(protected: long __cdecl CTaskListWnd::_TaskCreated(struct ITaskGroup *,struct ITaskItem *,int))"},
            (void**)&CTaskListWnd__TaskCreated,
        },
    };

    HMODULE taskbarModule = LoadLibrary(L"taskbar.dll");
    if (!taskbarModule) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return FALSE;
    }

    if (!HookSymbols(taskbarModule, symbolHooks, ARRAYSIZE(symbolHooks))) {
        return FALSE;
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

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    DWORD dwProcessId;
    DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd && GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) &&
        dwProcessId == dwCurrentProcessId) {
        HandleIdentifiedTaskbarWindow(hTaskbarWnd);

        FindCurrentProcessThumbnailWindows(hTaskbarWnd);
        for (HWND hWnd : g_thumbnailWindows) {
            Wh_Log(L"Thumbnail window found: %08X", (DWORD)(ULONG_PTR)hWnd);
            SubclassThumbnailWindow(hWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    for (HWND hWnd : g_thumbnailWindows) {
        UnsubclassThumbnailWindow(hWnd);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
