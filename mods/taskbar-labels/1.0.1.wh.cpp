// ==WindhawkMod==
// @id              taskbar-labels
// @name            Taskbar Labels for Windows 11
// @description     Show text labels for running programs on the taskbar (Windows 11 only)
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Taskbar Labels for Windows 11
Show text labels for running programs on the taskbar (Windows 11 only).

By default, the Windows 11 taskbar only shows icons for taskbar items, without
any text labels. This mod adds text labels, similarly to the way it was possible
to configure in older Windows versions.

Before:

![Before screenshot](https://i.imgur.com/SjHSF7g.png)

After:

![After screenshot](https://i.imgur.com/qpc4iFh.png)

## Known limitations

* When there are too many items, the rightmost items become unreachable.
* Switching between virtual desktops might result in missing labels.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- taskbarItemWidth: 160
  $name: Taskbar item width
  $description: >-
    The width of taskbar items for which text labels are added
*/
// ==/WindhawkModSettings==

#include <initguid.h>  // must come before knownfolders.h

#include <hstring.h>
#include <inspectable.h>
#include <knownfolders.h>
#include <roapi.h>
#include <shlobj.h>
#include <winstring.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#include <memory>
#include <regex>
#include <type_traits>

// #define EXTRA_DBG_LOG

struct {
    int taskbarItemWidth;
} g_settings;

bool g_applyingSettings = false;
bool g_unloading = false;

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

////////////////////////////////////////////////////////////////////////////////

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::Wrappers::HStringReference;

using HString = std::unique_ptr<std::remove_pointer_t<HSTRING>,
                                decltype(&WindowsDeleteString)>;

interface IVector : public IInspectable {
    // read methods
    virtual HRESULT STDMETHODCALLTYPE GetAt(_In_opt_ unsigned index,
                                            _Out_ void** item) = 0;
    virtual /* propget */ HRESULT STDMETHODCALLTYPE
    get_Size(_Out_ unsigned* size) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetView(_Outptr_result_maybenull_ void* view) = 0;
    virtual HRESULT STDMETHODCALLTYPE IndexOf(_In_opt_ void* value,
                                              _Out_ unsigned* index,
                                              _Out_ boolean* found) = 0;

    // write methods
    virtual HRESULT STDMETHODCALLTYPE SetAt(_In_ unsigned index,
                                            _In_opt_ void* item) = 0;
    virtual HRESULT STDMETHODCALLTYPE InsertAt(_In_ unsigned index,
                                               _In_opt_ void* item) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAt(_In_ unsigned index) = 0;
    virtual HRESULT STDMETHODCALLTYPE Append(_In_opt_ void* item) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAtEnd() = 0;
    virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

    // bulk transfer methods
    virtual HRESULT STDMETHODCALLTYPE
    GetMany(_In_ unsigned startIndex,
            _In_ unsigned capacity,
            _Out_writes_to_(capacity, *actual) void** value,
            _Out_ unsigned* actual) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    ReplaceAll(_In_ unsigned count, _In_reads_(count) void** value) = 0;
};

// Some of the definitions below are based on the source code of Explorer
// Patcher. Reference:
// https://github.com/valinet/ExplorerPatcher/blob/db576425272023b1ead811e7627823cb8b5517f2/ExplorerPatcher/lvt.h

typedef struct _Windows_UI_Xaml_Thickness {
    double Left;
    double Top;
    double Right;
    double Bottom;
} Windows_UI_Xaml_Thickness;

typedef enum _Windows_UI_Xaml_Visibility {
    Windows_UI_Xaml_Visibility_Visible = 0,
    Windows_UI_Xaml_Visibility_Collapsed = 1
} Windows_UI_Xaml_Visibility;

typedef enum _Windows_UI_Xaml_HorizontalAlignment {
    Windows_UI_Xaml_HorizontalAlignment_Left = 0,
    Windows_UI_Xaml_HorizontalAlignment_Center = 1,
    Windows_UI_Xaml_HorizontalAlignment_Right = 2,
    Windows_UI_Xaml_HorizontalAlignment_Stretch = 3
} Windows_UI_Xaml_HorizontalAlignment;

#pragma region "Windows.UI.Xaml.IDependencyObject"

DEFINE_GUID(IID_Windows_UI_Xaml_IDependencyObject,
            0x5c526665,
            0xf60e,
            0x4912,
            0xaf,
            0x59,
            0x5f,
            0xe0,
            0x68,
            0x0f,
            0x08,
            0x9d);

interface Windows_UI_Xaml_IDependencyObject : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE GetValue(
        /* [in] */ __RPC__in IInspectable* dp,
        /* [out] */ __RPC__out IInspectable** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetValue(
        /* [in] */ __RPC__in IInspectable* dp,
        /* [in] */ __RPC__in IInspectable* value) = 0;
};

#pragma endregion

#pragma region "Windows.UI.Xaml.Markup.IXamlReaderStatics"

DEFINE_GUID(IID_Windows_UI_Xaml_Markup_IXamlReaderStatics,
            0x9891c6bd,
            0x534f,
            0x4955,
            0xb8,
            0x5a,
            0x8a,
            0x8d,
            0xc0,
            0xdc,
            0xa6,
            0x02);

interface Windows_UI_Xaml_Markup_IXamlReaderStatics : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Load(
        /* [in] */ __RPC__in HSTRING xaml,
        /* [out][retval] */ __RPC__deref_out_opt IInspectable**
            returnValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE LoadWithInitialTemplateValidation(
        /* [in] */ __RPC__in HSTRING xaml,
        /* [out][retval] */ __RPC__deref_out_opt IInspectable**
            returnValue) = 0;
};

#pragma endregion

#pragma region "Windows.UI.Xaml.IVisualTreeHelperStatics"

DEFINE_GUID(IID_Windows_UI_Xaml_IVisualTreeHelperStatics,
            0xe75758c4,
            0xd25d,
            0x4b1d,
            0x97,
            0x1f,
            0x59,
            0x6f,
            0x17,
            0xf1,
            0x2b,
            0xaa);

interface Windows_UI_Xaml_IVisualTreeHelperStatics : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE FindElementsInHostCoordinatesPoint() = 0;

    virtual HRESULT STDMETHODCALLTYPE FindElementsInHostCoordinatesRect() = 0;

    virtual HRESULT STDMETHODCALLTYPE
    FindAllElementsInHostCoordinatesPoint() = 0;

    virtual HRESULT STDMETHODCALLTYPE
    FindAllElementsInHostCoordinatesRect() = 0;

    virtual HRESULT STDMETHODCALLTYPE GetChild(
        /* [in] */ __RPC__in Windows_UI_Xaml_IDependencyObject* reference,
        /* [in] */ __RPC__in INT32 childIndex,
        /* [out] */ __RPC__out Windows_UI_Xaml_IDependencyObject** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetChildrenCount(
        /* [in] */ __RPC__in Windows_UI_Xaml_IDependencyObject* reference,
        /* [out] */ __RPC__out INT32* result) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetParent(
        /* [in] */ __RPC__in Windows_UI_Xaml_IDependencyObject* reference,
        /* [out] */ __RPC__out Windows_UI_Xaml_IDependencyObject** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE DisconnectChildrenRecursive() = 0;
};

#pragma endregion

#pragma region "Windows.UI.Xaml.IFrameworkElement"

DEFINE_GUID(IID_Windows_UI_Xaml_IFrameworkElement,
            0xa391d09b,
            0x4a99,
            0x4b7c,
            0x9d,
            0x8d,
            0x6f,
            0xa5,
            0xd0,
            0x1f,
            0x6f,
            0xbf);

interface Windows_UI_Xaml_IFrameworkElement : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Triggers() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Resources() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Resources() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Tag() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Tag() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Language() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Language() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_ActualWidth(
        /* [out] */ __RPC__out DOUBLE* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_ActualHeight(
        /* [out] */ __RPC__out DOUBLE* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Width(
        /* [out] */ __RPC__out DOUBLE* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Width(
        /* [in] */ __RPC__in DOUBLE value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Height(
        /* [out] */ __RPC__out DOUBLE* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Height(
        /* [in] */ __RPC__in DOUBLE value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_MinWidth() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_MinWidth() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_MaxWidth() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_MaxWidth() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_MinHeight() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_MinHeight() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_MaxHeight() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_MaxHeight() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_HorizontalAlignment(
        /* [out] */ __RPC__out int32_t* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_HorizontalAlignment(
        /* [in] */ __RPC__in int32_t value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_VerticalAlignment(
        /* [out] */ __RPC__out int32_t* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_VerticalAlignment(
        /* [in] */ __RPC__in int32_t value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Margin(
        /* [out] */ __RPC__out Windows_UI_Xaml_Thickness* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Margin(
        /* [in] */ __RPC__in Windows_UI_Xaml_Thickness value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Name(
        /* [out] */ __RPC__deref_out_opt HSTRING* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Name() = 0;

    // ...
};

#pragma endregion

#pragma region "Windows.UI.Xaml.Controls.IPanel"

DEFINE_GUID(IID_Windows_UI_Xaml_Controls_IPanel,
            0xa50a4bbd,
            0x8361,
            0x469c,
            0x90,
            0xda,
            0xe9,
            0xa4,
            0x0c,
            0x74,
            0x74,
            0xdf);

interface Windows_UI_Xaml_Controls_IPanel : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Children(
        /* [out] */ __RPC__out IVector** value) = 0;

    // ...
};

#pragma endregion

#pragma region "Windows.UI.Xaml.Controls.ITextBlock"

DEFINE_GUID(IID_Windows_UI_Xaml_Controls_ITextBlock,
            0xae2d9271,
            0x3b4a,
            0x45fc,
            0x84,
            0x68,
            0xf7,
            0x94,
            0x95,
            0x48,
            0xf4,
            0xd5);

interface Windows_UI_Xaml_Controls_ITextBlock : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_FontSize(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FontSize(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FontFamily(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FontFamily(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FontWeight(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FontWeight(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FontStyle(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FontStyle(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FontStretch(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FontStretch(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        get_CharacterSpacing(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        put_CharacterSpacing(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Foreground(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Foreground(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TextWrapping(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TextWrapping(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TextTrimming(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TextTrimming(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TextAlignment(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TextAlignment(/*to-be-filled*/) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Text(
        /* [out] */ __RPC__deref_out_opt HSTRING* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Text(
        /* [in] */ __RPC__in HSTRING value) = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Inlines(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Padding(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Padding(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LineHeight(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LineHeight(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        get_LineStackingStrategy(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        put_LineStackingStrategy(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        get_IsTextSelectionEnabled(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        put_IsTextSelectionEnabled(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SelectedText(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentStart(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContentEnd(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SelectionStart(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SelectionEnd(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BaselineOffset(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        add_SelectionChanged(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        remove_SelectionChanged(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        add_ContextMenuOpening(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE
        remove_ContextMenuOpening(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE SelectAll(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE Select(/*to-be-filled*/) = 0;
    virtual HRESULT STDMETHODCALLTYPE Focus(/*to-be-filled*/) = 0;
};

#pragma endregion

#pragma region "Windows.UI.Xaml.IUIElement"

DEFINE_GUID(IID_Windows_UI_Xaml_IUIElement,
            0x676d0be9,
            0xb65c,
            0x41c6,
            0xba,
            0x40,
            0x58,
            0xcf,
            0x87,
            0xf2,
            0x01,
            0xc1);

interface Windows_UI_Xaml_IUIElement : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_DesiredSize() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_AllowDrop() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_AllowDrop() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Opacity() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Opacity() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Clip() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Clip() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_RenderTransform() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_RenderTransform() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Projection() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Projection() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_RenderTransformOrigin() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_RenderTransformOrigin() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_IsHitTestVisible() = 0;

    virtual HRESULT STDMETHODCALLTYPE put_IsHitTestVisible() = 0;

    virtual HRESULT STDMETHODCALLTYPE get_Visibility(
        /* [out] */ __RPC__out Windows_UI_Xaml_Visibility* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE put_Visibility(
        /* [in] */ __RPC__in Windows_UI_Xaml_Visibility value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RenderSize(
        /* [out][retval] */ __RPC__out /*ABI::Windows::Foundation::Size*/ void*
            value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UseLayoutRounding(
        /* [out][retval] */ __RPC__out boolean* value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_UseLayoutRounding(
        /* [in] */ boolean value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Transitions(
        /* [out][retval] */
        __RPC__deref_out_opt /*__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition*/
        void** value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Transitions(
        /* [in] */
        __RPC__in_opt /*__FIVector_1_Windows__CUI__CXaml__CMedia__CAnimation__CTransition*/
        void* value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CacheMode(
        /* [out][retval] */
        __RPC__deref_out_opt /*ABI::Windows::UI::Xaml::Media::ICacheMode*/
        void** value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CacheMode(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::Media::ICacheMode*/
        void* value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsTapEnabled(
        /* [out][retval] */ __RPC__out boolean* value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IsTapEnabled(
        /* [in] */ boolean value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsDoubleTapEnabled(
        /* [out][retval] */ __RPC__out boolean* value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IsDoubleTapEnabled(
        /* [in] */ boolean value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsRightTapEnabled(
        /* [out][retval] */ __RPC__out boolean* value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IsRightTapEnabled(
        /* [in] */ boolean value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsHoldingEnabled(
        /* [out][retval] */ __RPC__out boolean* value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IsHoldingEnabled(
        /* [in] */ boolean value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ManipulationMode(
        /* [out][retval] */
        __RPC__out /*ABI::Windows::UI::Xaml::Input::ManipulationModes*/ void*
            value) = 0;

    virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ManipulationMode(
        /* [in] */ /*ABI::Windows::UI::Xaml::Input::ManipulationModes*/ int
            value) = 0;

    virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PointerCaptures(
        /* [out][retval] */
        __RPC__deref_out_opt /*__FIVectorView_1_Windows__CUI__CXaml__CInput__CPointer*/
        void** value) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_KeyUp(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IKeyEventHandler*/ void*
            value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_KeyUp(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_KeyDown(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IKeyEventHandler*/ void*
            value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_KeyDown(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_GotFocus(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IRoutedEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_GotFocus(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_LostFocus(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IRoutedEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_LostFocus(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_DragEnter(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IDragEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_DragEnter(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_DragLeave(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IDragEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_DragLeave(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_DragOver(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IDragEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_DragOver(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_Drop(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IDragEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_Drop(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerPressed(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerPressed(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerMoved(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerMoved(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerReleased(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerReleased(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerEntered(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerEntered(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerExited(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerExited(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerCaptureLost(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerCaptureLost(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerCanceled(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerCanceled(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_PointerWheelChanged(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointerEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_PointerWheelChanged(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_Tapped(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::ITappedEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_Tapped(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_DoubleTapped(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IDoubleTappedEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_DoubleTapped(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_Holding(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IHoldingEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_Holding(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_RightTapped(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IRightTappedEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_RightTapped(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_ManipulationStarting(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IManipulationStartingEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_ManipulationStarting(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_ManipulationInertiaStarting(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IManipulationInertiaStartingEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_ManipulationInertiaStarting(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_ManipulationStarted(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IManipulationStartedEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_ManipulationStarted(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_ManipulationDelta(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IManipulationDeltaEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_ManipulationDelta(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE add_ManipulationCompleted(
        /* [in] */
        __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IManipulationCompletedEventHandler*/
        void* value,
        /* [out][retval] */ __RPC__out /*EventRegistrationToken*/ int64_t*
            token) = 0;

    virtual HRESULT STDMETHODCALLTYPE remove_ManipulationCompleted(
        /* [in] */ /*EventRegistrationToken*/ int64_t token) = 0;

    virtual HRESULT STDMETHODCALLTYPE Measure(
        /* [in] */ /*ABI::Windows::Foundation::Size*/ int availableSize) = 0;

    virtual HRESULT STDMETHODCALLTYPE Arrange(
        /* [in] */ /*ABI::Windows::Foundation::Rect*/ void* finalRect) = 0;

    virtual HRESULT STDMETHODCALLTYPE CapturePointer(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointer*/
        void* value,
        /* [out][retval] */ __RPC__out boolean* returnValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE ReleasePointerCapture(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::Input::IPointer*/
        void* value) = 0;

    virtual HRESULT STDMETHODCALLTYPE ReleasePointerCaptures(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddHandler(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IRoutedEvent*/ void*
            routedEvent,
        /* [in] */ __RPC__in_opt IInspectable* handler,
        /* [in] */ boolean handledEventsToo) = 0;

    virtual HRESULT STDMETHODCALLTYPE RemoveHandler(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IRoutedEvent*/ void*
            routedEvent,
        /* [in] */ __RPC__in_opt IInspectable* handler) = 0;

    virtual HRESULT STDMETHODCALLTYPE TransformToVisual(
        /* [in] */ __RPC__in_opt /*ABI::Windows::UI::Xaml::IUIElement*/ void*
            visual,
        /* [out][retval] */
        __RPC__deref_out_opt /*ABI::Windows::UI::Xaml::Media::IGeneralTransform*/
        void** returnValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE InvalidateMeasure(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE InvalidateArrange(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE UpdateLayout(void) = 0;
};

#pragma endregion

////////////////////////////////////////////////////////////////////////////////

ComPtr<Windows_UI_Xaml_IUIElement> LoadXamlControl(PCWSTR xaml) {
    HRESULT hr;

    HStringReference hsMarkupXamlReaderStatics(
        L"Windows.UI.Xaml.Markup.XamlReader");
    ComPtr<Windows_UI_Xaml_Markup_IXamlReaderStatics> pMarkupXamlReaderStatics;
    hr = RoGetActivationFactory(hsMarkupXamlReaderStatics.Get(),
                                IID_Windows_UI_Xaml_Markup_IXamlReaderStatics,
                                (void**)&pMarkupXamlReaderStatics);
    if (SUCCEEDED(hr)) {
        HStringReference xamlRef(xaml);
        ComPtr<IInspectable> pResult;
        hr = pMarkupXamlReaderStatics->Load(xamlRef.Get(), &pResult);
        if (SUCCEEDED(hr)) {
            ComPtr<Windows_UI_Xaml_IUIElement> pUIElement;
            hr = pResult->QueryInterface(IID_Windows_UI_Xaml_IUIElement,
                                         (void**)&pUIElement);
            if (SUCCEEDED(hr)) {
                return pUIElement;
            }
        }
    }

    return nullptr;
}

// Based on code from Explorer Patcher.
ComPtr<Windows_UI_Xaml_IDependencyObject> FindChildByClassName(
    Windows_UI_Xaml_IDependencyObject* pRootDependencyObject,
    Windows_UI_Xaml_IVisualTreeHelperStatics* pVisualTreeHelperStatics,
    LPCWSTR pwszRefName,
    INT* prevIndex) {
    // WCHAR wszDebug[MAX_PATH];
    HRESULT hr = S_OK;
    INT32 Count = -1;
    hr = pVisualTreeHelperStatics->GetChildrenCount(pRootDependencyObject,
                                                    &Count);
    if (SUCCEEDED(hr)) {
        for (INT32 Index = (prevIndex ? *prevIndex : 0); Index < Count;
             ++Index) {
            ComPtr<Windows_UI_Xaml_IDependencyObject> pChild;
            hr = pVisualTreeHelperStatics->GetChild(pRootDependencyObject,
                                                    Index, &pChild);
            if (SUCCEEDED(hr)) {
                HString hsChild(nullptr, &WindowsDeleteString);
                {
                    HSTRING hs = nullptr;
                    hr = pChild->GetRuntimeClassName(&hs);
                    if (SUCCEEDED(hr))
                        hsChild.reset(hs);
                }

                if (hsChild) {
                    PCWSTR pwszName =
                        WindowsGetStringRawBuffer(hsChild.get(), 0);
                    // swprintf_s(wszDebug, MAX_PATH, L"Name: %s\n", pwszName);
                    // OutputDebugStringW(wszDebug);
                    if (!_wcsicmp(pwszName, pwszRefName)) {
                        if (prevIndex)
                            *prevIndex = Index + 1;
                        return pChild;
                    }
                }
            }
        }
    }

    if (prevIndex)
        *prevIndex = Count;
    return nullptr;
}

// Based on code from Explorer Patcher.
ComPtr<Windows_UI_Xaml_IDependencyObject> FindChildByName(
    Windows_UI_Xaml_IDependencyObject* pRootDependencyObject,
    Windows_UI_Xaml_IVisualTreeHelperStatics* pVisualTreeHelperStatics,
    LPCWSTR pwszRefName) {
    // WCHAR wszDebug[MAX_PATH];
    HRESULT hr = S_OK;
    INT32 Count = -1;
    hr = pVisualTreeHelperStatics->GetChildrenCount(pRootDependencyObject,
                                                    &Count);
    if (SUCCEEDED(hr)) {
        for (INT32 Index = 0; Index < Count; ++Index) {
            ComPtr<Windows_UI_Xaml_IDependencyObject> pChild;
            hr = pVisualTreeHelperStatics->GetChild(pRootDependencyObject,
                                                    Index, &pChild);
            if (SUCCEEDED(hr)) {
                ComPtr<Windows_UI_Xaml_IFrameworkElement> pFrameworkElement;
                hr = pChild->QueryInterface(
                    IID_Windows_UI_Xaml_IFrameworkElement,
                    (void**)&pFrameworkElement);
                if (SUCCEEDED(hr)) {
                    HString hsChild(nullptr, &WindowsDeleteString);
                    {
                        HSTRING hs = nullptr;
                        hr = pFrameworkElement->get_Name(&hs);
                        if (SUCCEEDED(hr))
                            hsChild.reset(hs);
                    }

                    if (hsChild) {
                        PCWSTR pwszName =
                            WindowsGetStringRawBuffer(hsChild.get(), 0);
                        // swprintf_s(wszDebug, MAX_PATH, L"Name: %s\n",
                        // pwszName); OutputDebugStringW(wszDebug);
                        if (!_wcsicmp(pwszName, pwszRefName)) {
                            return pChild;
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

#if defined(EXTRA_DBG_LOG)

void LogElement(Windows_UI_Xaml_IDependencyObject* pChild, int depth) {
    HRESULT hr;

    {
        HString hsChild(nullptr, &WindowsDeleteString);
        {
            HSTRING hs = nullptr;
            hr = pChild->GetRuntimeClassName(&hs);
            if (SUCCEEDED(hr))
                hsChild.reset(hs);
        }

        PCWSTR pwszName =
            hsChild ? WindowsGetStringRawBuffer(hsChild.get(), 0) : L"-";

        WCHAR padding[MAX_PATH]{};
        for (int i = 0; i < depth * 4; i++) {
            padding[i] = L' ';
        }
        Wh_Log(L"%sClass: %s", padding, pwszName);
    }

    ComPtr<Windows_UI_Xaml_IFrameworkElement> pFrameworkElement;
    hr = pChild->QueryInterface(IID_Windows_UI_Xaml_IFrameworkElement,
                                (void**)&pFrameworkElement);
    if (SUCCEEDED(hr)) {
        HString hsChild(nullptr, &WindowsDeleteString);
        {
            HSTRING hs = nullptr;
            hr = pFrameworkElement->get_Name(&hs);
            if (SUCCEEDED(hr))
                hsChild.reset(hs);
        }

        PCWSTR pwszName =
            hsChild ? WindowsGetStringRawBuffer(hsChild.get(), 0) : L"-";

        WCHAR padding[MAX_PATH]{};
        for (int i = 0; i < depth * 4; i++) {
            padding[i] = L' ';
        }
        Wh_Log(L"%sName: %s", padding, pwszName);
    }
}

void LogElementTreeAux(
    Windows_UI_Xaml_IDependencyObject* pRootDependencyObject,
    Windows_UI_Xaml_IVisualTreeHelperStatics* pVisualTreeHelperStatics,
    int depth) {
    if (!pRootDependencyObject) {
        return;
    }

    LogElement(pRootDependencyObject, depth);

    HRESULT hr = S_OK;
    INT32 Count = -1;
    hr = pVisualTreeHelperStatics->GetChildrenCount(pRootDependencyObject,
                                                    &Count);
    if (SUCCEEDED(hr)) {
        for (INT32 Index = 0; Index < Count; ++Index) {
            ComPtr<Windows_UI_Xaml_IDependencyObject> pChild;
            hr = pVisualTreeHelperStatics->GetChild(pRootDependencyObject,
                                                    Index, &pChild);
            if (SUCCEEDED(hr)) {
                LogElementTreeAux(pChild.Get(), pVisualTreeHelperStatics,
                                  depth + 1);
            }
        }
    }
}

void LogElementTree(IInspectable* pElement) {
    HRESULT hr;

    HStringReference hsVisualTreeHelperStatics(
        L"Windows.UI.Xaml.Media.VisualTreeHelper");
    ComPtr<Windows_UI_Xaml_IVisualTreeHelperStatics> pVisualTreeHelperStatics;
    hr = RoGetActivationFactory(hsVisualTreeHelperStatics.Get(),
                                IID_Windows_UI_Xaml_IVisualTreeHelperStatics,
                                (void**)&pVisualTreeHelperStatics);
    if (SUCCEEDED(hr)) {
        ComPtr<Windows_UI_Xaml_IDependencyObject> pRootDependencyObject;
        hr = pElement->QueryInterface(IID_Windows_UI_Xaml_IDependencyObject,
                                      (void**)&pRootDependencyObject);
        if (SUCCEEDED(hr)) {
            LogElementTreeAux(pRootDependencyObject.Get(),
                              pVisualTreeHelperStatics.Get(), 0);
        }
    }
}

#endif  // defined(EXTRA_DBG_LOG)

////////////////////////////////////////////////////////////////////////////////

using CTaskListWnd__GetTBGroupFromGroup_t = void*(WINAPI*)(void* pThis,
                                                           void* taskGroup,
                                                           int* index);
CTaskListWnd__GetTBGroupFromGroup_t CTaskListWnd__GetTBGroupFromGroup_Original;

using CTaskBtnGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetNumItems_t CTaskBtnGroup_GetNumItems_Original;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(void* pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem_Original;

using CTaskGroup_GetTitleText_t = LONG_PTR(WINAPI*)(void* pThis,
                                                    void* taskItem,
                                                    WCHAR* text,
                                                    int bufferSize);
CTaskGroup_GetTitleText_t CTaskGroup_GetTitleText_Original;

using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    if (g_applyingSettings) {
        return true;
    }

    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}

bool g_inGroupChanged;
WCHAR g_taskBtnGroupTitleInGroupChanged[256];

using CTaskListWnd_GroupChanged_t = LONG_PTR(WINAPI*)(void* pThis,
                                                      void* taskGroup,
                                                      int taskGroupProperty);
CTaskListWnd_GroupChanged_t CTaskListWnd_GroupChanged_Original;
LONG_PTR WINAPI CTaskListWnd_GroupChanged_Hook(void* pThis,
                                               void* taskGroup,
                                               int taskGroupProperty) {
    Wh_Log(L">");

    int numItems = 0;
    void* taskItem = nullptr;

    void* taskBtnGroup = CTaskListWnd__GetTBGroupFromGroup_Original(
        (BYTE*)pThis - 0x28, taskGroup, nullptr);
    if (taskBtnGroup) {
        numItems = CTaskBtnGroup_GetNumItems_Original(taskBtnGroup);
        if (numItems == 1) {
            taskItem = CTaskBtnGroup_GetTaskItem_Original(taskBtnGroup, 0);
        }
    }

    WCHAR* textBuffer = g_taskBtnGroupTitleInGroupChanged;
    int textBufferSize = ARRAYSIZE(g_taskBtnGroupTitleInGroupChanged);

    if (numItems > 1) {
        int written = wsprintf(textBuffer, L"[%d] ", numItems);
        textBuffer += written;
        textBufferSize -= written;
    }

    CTaskGroup_GetTitleText_Original(taskGroup, taskItem, textBuffer,
                                     textBufferSize);

    g_inGroupChanged = true;
    LONG_PTR ret =
        CTaskListWnd_GroupChanged_Original(pThis, taskGroup, taskGroupProperty);
    g_inGroupChanged = false;

    *g_taskBtnGroupTitleInGroupChanged = L'\0';

    return ret;
}

using CTaskListWnd_TaskDestroyed_t = LONG_PTR(WINAPI*)(void* pThis,
                                                       void* taskGroup,
                                                       void* taskItem,
                                                       int taskDestroyedFlags);
CTaskListWnd_TaskDestroyed_t CTaskListWnd_TaskDestroyed_Original;
LONG_PTR WINAPI CTaskListWnd_TaskDestroyed_Hook(void* pThis,
                                                void* taskGroup,
                                                void* taskItem,
                                                int taskDestroyedFlags) {
    Wh_Log(L">");

    LONG_PTR ret = CTaskListWnd_TaskDestroyed_Original(
        pThis, taskGroup, taskItem, taskDestroyedFlags);

    // Trigger CTaskListWnd::GroupChanged to trigger the title change.
    int taskGroupProperty = 4;  // saw this in the debugger
    CTaskListWnd_GroupChanged_Hook(pThis, taskGroup, taskGroupProperty);

    return ret;
}

////////////////////////////////////////////////////////////////////////////////

double* double_48_value_Original;

using ITaskListButton_get_IsRunning_t = HRESULT(WINAPI*)(void* pThis,
                                                         bool* running);
ITaskListButton_get_IsRunning_t ITaskListButton_get_IsRunning_Original;

void UpdateTaskListButtonCustomizations(void* pTaskListButtonImpl) {
    HRESULT hr;

    IInspectable* pTaskListButton = *(IInspectable**)pTaskListButtonImpl;

    ComPtr<Windows_UI_Xaml_IFrameworkElement> pTaskListButtonElement;
    ComPtr<Windows_UI_Xaml_IFrameworkElement> pIconPanelElement;
    ComPtr<Windows_UI_Xaml_IFrameworkElement> pIconElement;
    ComPtr<Windows_UI_Xaml_IFrameworkElement> pWindhawkTextElement;

    hr = pTaskListButton->QueryInterface(IID_Windows_UI_Xaml_IFrameworkElement,
                                         (void**)&pTaskListButtonElement);

    HStringReference hsVisualTreeHelperStatics(
        L"Windows.UI.Xaml.Media.VisualTreeHelper");
    ComPtr<Windows_UI_Xaml_IVisualTreeHelperStatics> pVisualTreeHelperStatics;
    hr = RoGetActivationFactory(hsVisualTreeHelperStatics.Get(),
                                IID_Windows_UI_Xaml_IVisualTreeHelperStatics,
                                (void**)&pVisualTreeHelperStatics);
    if (SUCCEEDED(hr)) {
        ComPtr<Windows_UI_Xaml_IDependencyObject> pRootDependencyObject;
        hr = pTaskListButton->QueryInterface(
            IID_Windows_UI_Xaml_IDependencyObject,
            (void**)&pRootDependencyObject);
        if (SUCCEEDED(hr)) {
            ComPtr<Windows_UI_Xaml_IDependencyObject> pIconPanel =
                FindChildByName(pRootDependencyObject.Get(),
                                pVisualTreeHelperStatics.Get(), L"IconPanel");
            if (pIconPanel) {
                hr = pIconPanel->QueryInterface(
                    IID_Windows_UI_Xaml_IFrameworkElement,
                    (void**)&pIconPanelElement);

                ComPtr<Windows_UI_Xaml_IDependencyObject> pIcon =
                    FindChildByName(pIconPanel.Get(),
                                    pVisualTreeHelperStatics.Get(), L"Icon");
                if (pIcon) {
                    hr = pIcon->QueryInterface(
                        IID_Windows_UI_Xaml_IFrameworkElement,
                        (void**)&pIconElement);
                }

                ComPtr<Windows_UI_Xaml_IDependencyObject> pWindhawkText =
                    FindChildByName(pIconPanel.Get(),
                                    pVisualTreeHelperStatics.Get(),
                                    L"WindhawkText");
                if (pWindhawkText) {
                    hr = pWindhawkText->QueryInterface(
                        IID_Windows_UI_Xaml_IFrameworkElement,
                        (void**)&pWindhawkTextElement);
                }
            }
        }
    }

    if (!pTaskListButtonElement || !pIconPanelElement || !pIconElement) {
        return;
    }

    double taskListButtonWidth = 0;
    pTaskListButtonElement->get_ActualWidth(&taskListButtonWidth);

    double iconPanelWidth = 0;
    pIconPanelElement->get_ActualWidth(&iconPanelWidth);

    // Check if non-positive or NaN.
    if (!(taskListButtonWidth > 0) || !(iconPanelWidth > 0)) {
        return;
    }

    static double initialWidth = iconPanelWidth;

    void* taskListButtonProducer = (BYTE*)pTaskListButtonImpl + 0x10;

    bool isRunning = false;
    hr = ITaskListButton_get_IsRunning_Original(taskListButtonProducer,
                                                &isRunning);

    bool showLabels = isRunning && !g_unloading;

    double widthToSet = showLabels ? g_settings.taskbarItemWidth : initialWidth;

    if (widthToSet != taskListButtonWidth || widthToSet != iconPanelWidth) {
        pTaskListButtonElement->put_Width(widthToSet);
        pIconPanelElement->put_Width(widthToSet);

        int32_t horizontalAlignment =
            (int32_t)(showLabels ? Windows_UI_Xaml_HorizontalAlignment_Left
                                 : Windows_UI_Xaml_HorizontalAlignment_Center);
        pIconElement->put_HorizontalAlignment(horizontalAlignment);

        Windows_UI_Xaml_Thickness margin{};
        if (showLabels) {
            margin.Left = 10;
        }
        pIconElement->put_Margin(margin);
    }

    if (showLabels && !pWindhawkTextElement) {
        ComPtr<Windows_UI_Xaml_Controls_IPanel> pIconPanel;
        hr = pIconPanelElement->QueryInterface(
            IID_Windows_UI_Xaml_Controls_IPanel, (void**)&pIconPanel);
        if (SUCCEEDED(hr)) {
            ComPtr<IVector> children;
            hr = pIconPanel->get_Children(&children);
            if (SUCCEEDED(hr)) {
                PCWSTR xaml =
                    LR"(
                        <TextBlock
                            xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                            xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
                            xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
                            xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
                            mc:Ignorable="d"
                            Name="WindhawkText"
                            VerticalAlignment="Center"
                            FontSize="12"
                            TextTrimming="CharacterEllipsis"
                        />
                    )";

                Microsoft::WRL::ComPtr<Windows_UI_Xaml_IUIElement> pUIElement =
                    LoadXamlControl(xaml);
                if (pUIElement) {
                    children->Append(pUIElement.Get());

                    ComPtr<Windows_UI_Xaml_IFrameworkElement> pAddedElement;
                    hr = pUIElement->QueryInterface(
                        IID_Windows_UI_Xaml_IFrameworkElement,
                        (void**)&pAddedElement);
                    if (SUCCEEDED(hr)) {
                        Windows_UI_Xaml_Thickness margin{};
                        margin.Left = 10 + 24 + 8;
                        margin.Right = 10;
                        margin.Bottom = 2;
                        pAddedElement->put_Margin(margin);
                    }
                }
            }
        }
    } else if (!showLabels && pWindhawkTextElement) {
        // Don't remove, for some reason it causes a bug - the running indicator
        // ends up being behind the semi-transparent rectangle of the active
        // button.
        /*
        ComPtr<Windows_UI_Xaml_Controls_IPanel> pIconPanel;
        hr = pIconPanelElement->QueryInterface(
            IID_Windows_UI_Xaml_Controls_IPanel, (void**)&pIconPanel);
        if (SUCCEEDED(hr)) {
            ComPtr<IVector> children;
            hr = pIconPanel->get_Children(&children);
            if (SUCCEEDED(hr)) {
                unsigned int index = -1;
                boolean found = false;
                hr = children->IndexOf(pWindhawkTextElement.Get(), &index,
                                       &found);
                if (SUCCEEDED(hr) && found) {
                    hr = children->RemoveAt(index);
                }
            }
        }
        */

        // Set empty text instead.
        ComPtr<Windows_UI_Xaml_Controls_ITextBlock> pWindhawkTextControl;
        hr = pWindhawkTextElement->QueryInterface(
            IID_Windows_UI_Xaml_Controls_ITextBlock,
            (void**)&pWindhawkTextControl);
        if (pWindhawkTextControl) {
            HStringReference hsTitle(L"");
            pWindhawkTextControl->put_Text(hsTitle.Get());
        }
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateVisualStates_Original(pThis);

    void* pTaskListButtonImpl = (BYTE*)pThis + 0x08;

#if defined(EXTRA_DBG_LOG)
    Wh_Log(L"=====");
    IInspectable* pTaskListButton = *(IInspectable**)pTaskListButtonImpl;
    LogElementTree(pTaskListButton);
    Wh_Log(L"=====");
#endif  // defined(EXTRA_DBG_LOG)

    UpdateTaskListButtonCustomizations(pTaskListButtonImpl);
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateButtonPadding_Original(pThis);

    void* pTaskListButtonImpl = (BYTE*)pThis + 0x08;

    UpdateTaskListButtonCustomizations(pTaskListButtonImpl);
}

using TaskListButton_Icon_t = void(WINAPI*)(void* pThis,
                                            LONG_PTR randomAccessStream);
TaskListButton_Icon_t TaskListButton_Icon_Original;
void WINAPI TaskListButton_Icon_Hook(void* pThis, LONG_PTR randomAccessStream) {
    Wh_Log(L">");

    TaskListButton_Icon_Original(pThis, randomAccessStream);

    if (!g_inGroupChanged) {
        return;
    }

    void* pTaskListButtonImpl = (BYTE*)pThis + 0x08;

    HRESULT hr;

    IInspectable* pTaskListButton = *(IInspectable**)pTaskListButtonImpl;

    ComPtr<Windows_UI_Xaml_Controls_ITextBlock> pWindhawkTextControl;

    HStringReference hsVisualTreeHelperStatics(
        L"Windows.UI.Xaml.Media.VisualTreeHelper");
    ComPtr<Windows_UI_Xaml_IVisualTreeHelperStatics> pVisualTreeHelperStatics;
    hr = RoGetActivationFactory(hsVisualTreeHelperStatics.Get(),
                                IID_Windows_UI_Xaml_IVisualTreeHelperStatics,
                                (void**)&pVisualTreeHelperStatics);
    if (SUCCEEDED(hr)) {
        ComPtr<Windows_UI_Xaml_IDependencyObject> pRootDependencyObject;
        hr = pTaskListButton->QueryInterface(
            IID_Windows_UI_Xaml_IDependencyObject,
            (void**)&pRootDependencyObject);
        if (SUCCEEDED(hr)) {
            ComPtr<Windows_UI_Xaml_IDependencyObject> pIconPanel =
                FindChildByName(pRootDependencyObject.Get(),
                                pVisualTreeHelperStatics.Get(), L"IconPanel");
            if (pIconPanel) {
                ComPtr<Windows_UI_Xaml_IDependencyObject> pWindhawkText =
                    FindChildByName(pIconPanel.Get(),
                                    pVisualTreeHelperStatics.Get(),
                                    L"WindhawkText");
                if (pWindhawkText) {
                    hr = pWindhawkText->QueryInterface(
                        IID_Windows_UI_Xaml_Controls_ITextBlock,
                        (void**)&pWindhawkTextControl);
                }
            }
        }
    }

    if (pWindhawkTextControl) {
        HStringReference hsTitle(g_taskBtnGroupTitleInGroupChanged);
        pWindhawkTextControl->put_Text(hsTitle.Get());
    }
}

void LoadSettings() {
    g_settings.taskbarItemWidth = Wh_GetIntSetting(L"taskbarItemWidth");
}

void FreeSettings() {
    // Nothing for now.
}

bool ProtectAndMemspy(DWORD protect, void* dst, const void* src, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtect(dst, size, protect, &oldProtect)) {
        return false;
    }

    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
    return true;
}

void ApplySettings() {
    g_applyingSettings = true;

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);

    double prevTaskbarHeight = *double_48_value_Original;

    // Temporarily change the height to zero to force a UI refresh.
    double tempTaskbarHeight = 0;
    ProtectAndMemspy(PAGE_READWRITE, double_48_value_Original,
                     &tempTaskbarHeight, sizeof(double));

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    // Wait for the change to apply.
    int counter = 0;
    RECT rc;
    while (GetWindowRect(hTaskbarWnd, &rc) && rc.bottom > rc.top) {
        if (++counter >= 100) {
            break;
        }
        Sleep(100);
    }

    ProtectAndMemspy(PAGE_READWRITE, double_48_value_Original,
                     &prevTaskbarHeight, sizeof(double));

    if (hTaskbarWnd) {
        // Trigger TrayUI::_HandleSettingChange.
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);

        HWND hReBarWindow32 =
            FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
        if (hReBarWindow32) {
            HWND hMSTaskSwWClass = FindWindowEx(hReBarWindow32, nullptr,
                                                L"MSTaskSwWClass", nullptr);
            if (hMSTaskSwWClass) {
                // Trigger CTaskBand::_HandleSyncDisplayChange.
                SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
            }
        }
    }

    g_applyingSettings = false;
}

struct SYMBOL_HOOK {
    std::wregex symbolRegex;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount) {
    WH_FIND_SYMBOL symbol;
    HANDLE findSymbol = Wh_FindFirstSymbol(module, nullptr, &symbol);
    if (!findSymbol) {
        return false;
    }

    do {
        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (!*symbolHooks[i].pOriginalFunction &&
                std::regex_match(symbol.symbol, symbolHooks[i].symbolRegex)) {
                if (symbolHooks[i].hookFunction) {
                    Wh_SetFunctionHook(symbol.address,
                                       symbolHooks[i].hookFunction,
                                       symbolHooks[i].pOriginalFunction);
                    Wh_Log(L"Hooked %p (%s)", symbol.address, symbol.symbol);
                } else {
                    *symbolHooks[i].pOriginalFunction = symbol.address;
                    Wh_Log(L"Found %p (%s)", symbol.address, symbol.symbol);
                }
                break;
            }
        }
    } while (Wh_FindNextSymbol(findSymbol, &symbol));

    Wh_FindCloseSymbol(findSymbol);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (!symbolHooks[i].optional && !*symbolHooks[i].pOriginalFunction) {
            Wh_Log(L"Missing symbol: %d", i);
            return false;
        }
    }

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    SYMBOL_HOOK symbolHooks[] = {
        {std::wregex(
             LR"(protected: struct ITaskBtnGroup \* __ptr64 __cdecl CTaskListWnd::_GetTBGroupFromGroup\(struct ITaskGroup \* __ptr64,int \* __ptr64\) __ptr64)"),
         (void**)&CTaskListWnd__GetTBGroupFromGroup_Original, nullptr},

        {std::wregex(
             LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems\(void\) __ptr64)"),
         (void**)&CTaskBtnGroup_GetNumItems_Original, nullptr},

        {std::wregex(
             LR"(public: virtual struct ITaskItem \* __ptr64 __cdecl CTaskBtnGroup::GetTaskItem\(int\) __ptr64)"),
         (void**)&CTaskBtnGroup_GetTaskItem_Original, nullptr},

        {std::wregex(
             LR"(public: virtual long __cdecl CTaskGroup::GetTitleText\(struct ITaskItem \* __ptr64,unsigned short \* __ptr64,int\) __ptr64)"),
         (void**)&CTaskGroup_GetTitleText_Original, nullptr},

        {std::wregex(
             LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired\(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const & __ptr64,enum IconContainerFlags\) __ptr64)"),
         (void**)&IconContainer_IsStorageRecreationRequired_Original,
         (void*)IconContainer_IsStorageRecreationRequired_Hook},

        {std::wregex(
             LR"(public: virtual void __cdecl CTaskListWnd::GroupChanged\(struct ITaskGroup \* __ptr64,enum winrt::WindowsUdk::UI::Shell::TaskGroupProperty\) __ptr64)"),
         (void**)&CTaskListWnd_GroupChanged_Original,
         (void*)CTaskListWnd_GroupChanged_Hook},

        {std::wregex(
             LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed\(struct ITaskGroup \* __ptr64,struct ITaskItem \* __ptr64,enum TaskDestroyedFlags\) __ptr64)"),
         (void**)&CTaskListWnd_TaskDestroyed_Original,
         (void*)CTaskListWnd_TaskDestroyed_Hook},
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarViewDllSymbols() {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    bool windowsVersionIdentified = false;
    HMODULE module;

    WCHAR szTargetDllPath[MAX_PATH];
    wcscpy_s(szTargetDllPath, szWindowsDirectory);
    wcscat_s(
        szTargetDllPath,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(szTargetDllPath) != INVALID_FILE_ATTRIBUTES) {
        // Windows 11 version 22H2.
        windowsVersionIdentified = true;

        module = GetModuleHandle(szTargetDllPath);
        if (!module) {
            // Try to load dependency DLLs. At process start, if they're not
            // loaded, loading the taskbar view DLL fails.
            WCHAR szRuntimeDllPath[MAX_PATH];

            wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
            wcscat_s(
                szRuntimeDllPath,
                LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\vcruntime140_app.dll)");
            LoadLibrary(szRuntimeDllPath);

            wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
            wcscat_s(
                szRuntimeDllPath,
                LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\vcruntime140_1_app.dll)");
            LoadLibrary(szRuntimeDllPath);

            wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
            wcscat_s(
                szRuntimeDllPath,
                LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\msvcp140_app.dll)");
            LoadLibrary(szRuntimeDllPath);

            module = LoadLibrary(szTargetDllPath);
        }
    }

    if (!windowsVersionIdentified) {
        wcscpy_s(szTargetDllPath, szWindowsDirectory);
        wcscat_s(
            szTargetDllPath,
            LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
        if (GetFileAttributes(szTargetDllPath) != INVALID_FILE_ATTRIBUTES) {
            // Windows 11 version 21H2.
            windowsVersionIdentified = true;

            module = GetModuleHandle(szTargetDllPath);
            if (!module) {
                // Try to load dependency DLLs. At process start, if they're not
                // loaded, loading the ExplorerExtensions DLL fails.
                WCHAR szRuntimeDllPath[MAX_PATH];

                PWSTR pProgramFilesDirectory;
                if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0,
                                                   nullptr,
                                                   &pProgramFilesDirectory))) {
                    wcscpy_s(szRuntimeDllPath, pProgramFilesDirectory);
                    wcscat_s(
                        szRuntimeDllPath,
                        LR"(\WindowsApps\Microsoft.VCLibs.140.00_14.0.29231.0_x64__8wekyb3d8bbwe\vcruntime140_app.dll)");
                    LoadLibrary(szRuntimeDllPath);

                    wcscpy_s(szRuntimeDllPath, pProgramFilesDirectory);
                    wcscat_s(
                        szRuntimeDllPath,
                        LR"(\WindowsApps\Microsoft.VCLibs.140.00_14.0.29231.0_x64__8wekyb3d8bbwe\vcruntime140_1_app.dll)");
                    LoadLibrary(szRuntimeDllPath);

                    wcscpy_s(szRuntimeDllPath, pProgramFilesDirectory);
                    wcscat_s(
                        szRuntimeDllPath,
                        LR"(\WindowsApps\Microsoft.VCLibs.140.00_14.0.29231.0_x64__8wekyb3d8bbwe\msvcp140_app.dll)");
                    LoadLibrary(szRuntimeDllPath);

                    CoTaskMemFree(pProgramFilesDirectory);

                    module = LoadLibrary(szTargetDllPath);
                }
            }
        }
    }

    if (!module) {
        Wh_Log(L"Failed to load module");
        return FALSE;
    }

    SYMBOL_HOOK symbolHooks[] = {
        {std::wregex(LR"(__real@4048000000000000)"),
         (void**)&double_48_value_Original, nullptr},

        {std::wregex(
             LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning\(bool \* __ptr64\) __ptr64)"),
         (void**)&ITaskListButton_get_IsRunning_Original, nullptr},

        {std::wregex(
             LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates\(void\) __ptr64)"),
         (void**)&TaskListButton_UpdateVisualStates_Original,
         (void*)TaskListButton_UpdateVisualStates_Hook},

        {std::wregex(
             LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding\(void\) __ptr64)"),
         (void**)&TaskListButton_UpdateButtonPadding_Original,
         (void*)TaskListButton_UpdateButtonPadding_Hook},

        {std::wregex(
             LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Icon\(struct winrt::Windows::Storage::Streams::IRandomAccessStream\) __ptr64)"),
         (void**)&TaskListButton_Icon_Original,
         (void*)TaskListButton_Icon_Hook},
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (!HookTaskbarViewDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit(void) {
    Wh_Log(L">");

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    FreeSettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    FreeSettings();
    LoadSettings();

    ApplySettings();
}
