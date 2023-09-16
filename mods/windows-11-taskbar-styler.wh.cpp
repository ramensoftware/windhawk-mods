// ==WindhawkMod==
// @id              windows-11-taskbar-styler
// @name            Windows 11 Taskbar Styler
// @description     An advanced mod to override style attributes of the taskbar control elements
// @version         1.1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols
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
# Windows 11 Taskbar Styler

An advanced mod to override style attributes of the taskbar control elements.

The settings have two sections: control styles and resource variables. Control
styles allow to override styles, such as size and color, for the target
elements. Resource variables allow to override predefined variables. For a more
detailed explanation and examples, refer to the sections below.

The taskbar's XAML resources can help find out which elements and resource
variables can be customized. To the best of my knowledge, there are no public
tools that are able to decode the resource files of recent Windows versions, but
here are XAML resources which were obtained via other means for your
convenience: [TaskbarResources.xbf and
SystemTrayResources.xbf](https://gist.github.com/m417z/ad0ab39351aca905f1d186b1f1c3d8c7).

The [UWPSpy](https://ramensoftware.com/uwpspy) tool can be used to inspect the
taskbar's control elements in real time, and experiment with various styles.

## Control styles

Each entry has a target control and list of styles.

The target control is written as `Control` or `Control#Name`, i.e. the target
control tag name, such as `taskbar:TaskListButton` or `Rectangle`, optionally
followed by `#` and the target control's `x:Name` attribute. The target control
can also include parent elements, separated by `>`, for example:
`ParentControl#ParentName > Control#Name`.

Each style is written as `Style=Value`, for example: `Height=5`.

A couple of practical examples:

### Task list button corner radius

![Screenshot](https://i.imgur.com/zDATi9K.png)

* Target: `taskbar:TaskListButton`
* Style: `CornerRadius=0`

### Task list button background color

![Screenshot](https://i.imgur.com/eP13uBu.png)

* Target: `taskbar:TaskListButtonPanel > Border#BackgroundElement`
* Style: `Background=gray`

### Running indicator size and color

![Screenshot](https://i.imgur.com/enttdYJ.png)

* Target: `Rectangle#RunningIndicator`
* Styles: `Fill=#FFED7014`, `Height=2`, `MinWidth=12`

**Note:** Currently, it's not possible to set styles only for a specific visual
state. In this example, the color and width apply for all states, including
active and inactive buttons.

### Time font and color

![Screenshot](https://i.imgur.com/ZRM0drT.png)

* Target: `TextBlock#TimeInnerTextBlock`
* Styles: `FontFamily=Comic Sans MS`, `FontSize=14`, `Foreground=Red`

### Hide date

![Screenshot](https://i.imgur.com/9fGyL9W.png)

* Target: `TextBlock#DateInnerTextBlock`
* Styles: `Visibility=Collapsed`

## Resource variables

Some variables, such as size and padding for various controls, are defined as
resource variables. Here are several examples:

* `TaskbarContextMenuMargin`: The margin between the taskbar and the start
  button context menu.

* `ContextMenuMargin`: The margin between the taskbar and the notification area
  context menu.

* `MediumTaskbarButtonExtent`: The width of the taskbar buttons.

## Implementation notes

The VisualTreeWatcher implementation is based on the
[ExplorerTAP](https://github.com/TranslucentTB/TranslucentTB/tree/develop/ExplorerTAP)
code from the **TranslucentTB** project.

Some code is borrowed from MSVC generated headers. To reduce the noise, hide the
relevant `#pragma region` regions in the code editor.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- controlStyles:
  - - target: taskbar:TaskListButton
      $name: Target
    - styles: [CornerRadius=0]
      $name: Styles
  $name: Control styles
- resourceVariables:
  - - variableKey: TaskbarContextMenuMargin
      $name: Variable key
    - value: "0"
      $name: Value
  $name: Resource variables
- promptForExplorerRestart: true
  $name: Prompt for Explorer restart
  $description: >-
    Show a message prompting to restart Explorer on each style change
*/
// ==/WindhawkModSettings==

#include <atomic>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>

std::atomic<bool> g_disabled = false;
std::atomic<DWORD> g_targetThreadId = 0;

void ApplyCustomizations(winrt::Windows::UI::Xaml::FrameworkElement element);

////////////////////////////////////////////////////////////////////////////////
// clang-format off

#pragma region winrt_hpp

#include <guiddef.h>
#include <Unknwn.h>
#include <winrt/base.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Microsoft::UI::Xaml::Controls {}
    namespace TranslucentTB::Xaml::Models::Primitives {}
    namespace Windows {
        namespace Foundation::Collections {}
        namespace UI::Xaml {
            namespace Controls {}
            namespace Hosting {}
        }
    }
}

// alias some long namespaces for convenience
// namespace mux = winrt::Microsoft::UI::Xaml;
// namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
// namespace txmp = winrt::TranslucentTB::Xaml::Models::Primitives;
namespace wf = winrt::Windows::Foundation;
// namespace wfc = wf::Collections;
namespace wux = winrt::Windows::UI::Xaml;
// namespace wuxc = wux::Controls;
namespace wuxh = wux::Hosting;

#pragma endregion  // winrt_hpp

#pragma region xamlOM_h

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include <rpc.h>
#include <rpcndr.h>

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include <windows.h>
#include <ole2.h>
#endif /*COM_NO_WINDOWS_H*/

#ifndef __xamlom_h__
#define __xamlom_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
// #pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVisualTreeServiceCallback_FWD_DEFINED__
#define __IVisualTreeServiceCallback_FWD_DEFINED__
typedef interface IVisualTreeServiceCallback IVisualTreeServiceCallback;

#endif 	/* __IVisualTreeServiceCallback_FWD_DEFINED__ */


#ifndef __IVisualTreeServiceCallback2_FWD_DEFINED__
#define __IVisualTreeServiceCallback2_FWD_DEFINED__
typedef interface IVisualTreeServiceCallback2 IVisualTreeServiceCallback2;

#endif 	/* __IVisualTreeServiceCallback2_FWD_DEFINED__ */


#ifndef __IVisualTreeService_FWD_DEFINED__
#define __IVisualTreeService_FWD_DEFINED__
typedef interface IVisualTreeService IVisualTreeService;

#endif 	/* __IVisualTreeService_FWD_DEFINED__ */


#ifndef __IXamlDiagnostics_FWD_DEFINED__
#define __IXamlDiagnostics_FWD_DEFINED__
typedef interface IXamlDiagnostics IXamlDiagnostics;

#endif 	/* __IXamlDiagnostics_FWD_DEFINED__ */


#ifndef __IBitmapData_FWD_DEFINED__
#define __IBitmapData_FWD_DEFINED__
typedef interface IBitmapData IBitmapData;

#endif 	/* __IBitmapData_FWD_DEFINED__ */


#ifndef __IVisualTreeService2_FWD_DEFINED__
#define __IVisualTreeService2_FWD_DEFINED__
typedef interface IVisualTreeService2 IVisualTreeService2;

#endif 	/* __IVisualTreeService2_FWD_DEFINED__ */


#ifndef __IVisualTreeService3_FWD_DEFINED__
#define __IVisualTreeService3_FWD_DEFINED__
typedef interface IVisualTreeService3 IVisualTreeService3;

#endif 	/* __IVisualTreeService3_FWD_DEFINED__ */

/* header files for imported files */
#include <oaidl.h>
#include <ocidl.h>
#include <inspectable.h>
#include <dxgi1_2.h>

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_xamlom_0000_0000 */
/* [local] */ 

#pragma region Application Family
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#pragma warning(push)
#pragma warning(disable:4668) 
#pragma warning(disable:4001) 
// #pragma once
#pragma warning(pop)
// Win32 API definitions
#define E_NOTFOUND HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
#define E_UNKNOWNTYPE MAKE_HRESULT(SEVERITY_ERROR, FACILITY_XAML, 40L)
_Check_return_ HRESULT InitializeXamlDiagnostic(_In_ LPCWSTR endPointName, _In_ DWORD pid, _In_ LPCWSTR wszDllXamlDiagnostics, _In_ LPCWSTR wszTAPDllName,  _In_ CLSID tapClsid);
_Check_return_ HRESULT InitializeXamlDiagnosticsEx(_In_ LPCWSTR endPointName, _In_ DWORD pid, _In_ LPCWSTR wszDllXamlDiagnostics, _In_ LPCWSTR wszTAPDllName, _In_ CLSID tapClsid, _In_ LPCWSTR wszInitializationData);
typedef MIDL_uhyper InstanceHandle;

typedef 
enum VisualMutationType
    {
        Add	= 0,
        Remove	= ( Add + 1 ) 
    } 	VisualMutationType;

typedef 
enum BaseValueSource
    {
        BaseValueSourceUnknown	= 0,
        BaseValueSourceDefault	= ( BaseValueSourceUnknown + 1 ) ,
        BaseValueSourceBuiltInStyle	= ( BaseValueSourceDefault + 1 ) ,
        BaseValueSourceStyle	= ( BaseValueSourceBuiltInStyle + 1 ) ,
        BaseValueSourceLocal	= ( BaseValueSourceStyle + 1 ) ,
        Inherited	= ( BaseValueSourceLocal + 1 ) ,
        DefaultStyleTrigger	= ( Inherited + 1 ) ,
        TemplateTrigger	= ( DefaultStyleTrigger + 1 ) ,
        StyleTrigger	= ( TemplateTrigger + 1 ) ,
        ImplicitStyleReference	= ( StyleTrigger + 1 ) ,
        ParentTemplate	= ( ImplicitStyleReference + 1 ) ,
        ParentTemplateTrigger	= ( ParentTemplate + 1 ) ,
        Animation	= ( ParentTemplateTrigger + 1 ) ,
        Coercion	= ( Animation + 1 ) ,
        BaseValueSourceVisualState	= ( Coercion + 1 ) 
    } 	BaseValueSource;

typedef struct SourceInfo
    {
    BSTR FileName;
    unsigned int LineNumber;
    unsigned int ColumnNumber;
    unsigned int CharPosition;
    BSTR Hash;
    } 	SourceInfo;

typedef struct ParentChildRelation
    {
    InstanceHandle Parent;
    InstanceHandle Child;
    unsigned int ChildIndex;
    } 	ParentChildRelation;

typedef struct VisualElement
    {
    InstanceHandle Handle;
    SourceInfo SrcInfo;
    BSTR Type;
    BSTR Name;
    unsigned int NumChildren;
    } 	VisualElement;

typedef struct PropertyChainSource
    {
    InstanceHandle Handle;
    BSTR TargetType;
    BSTR Name;
    BaseValueSource Source;
    SourceInfo SrcInfo;
    } 	PropertyChainSource;

typedef 
enum MetadataBit
    {
        None	= 0,
        IsValueHandle	= 0x1,
        IsPropertyReadOnly	= 0x2,
        IsValueCollection	= 0x4,
        IsValueCollectionReadOnly	= 0x8,
        IsValueBindingExpression	= 0x10,
        IsValueNull	= 0x20,
        IsValueHandleAndEvaluatedValue	= 0x40
    } 	MetadataBit;

typedef struct PropertyChainValue
    {
    unsigned int Index;
    BSTR Type;
    BSTR DeclaringType;
    BSTR ValueType;
    BSTR ItemType;
    BSTR Value;
    BOOL Overridden;
    hyper MetadataBits;
    BSTR PropertyName;
    unsigned int PropertyChainIndex;
    } 	PropertyChainValue;

typedef struct EnumType
    {
    BSTR Name;
    SAFEARRAY * ValueInts;
    SAFEARRAY * ValueStrings;
    } 	EnumType;

typedef struct CollectionElementValue
    {
    unsigned int Index;
    BSTR ValueType;
    BSTR Value;
    hyper MetadataBits;
    } 	CollectionElementValue;

typedef 
enum RenderTargetBitmapOptions
    {
        RenderTarget	= 0,
        RenderTargetAndChildren	= ( RenderTarget + 1 ) 
    } 	RenderTargetBitmapOptions;

typedef struct BitmapDescription
    {
    unsigned int Width;
    unsigned int Height;
    DXGI_FORMAT Format;
    DXGI_ALPHA_MODE AlphaMode;
    } 	BitmapDescription;

typedef 
enum ResourceType
    {
        ResourceTypeStatic	= 0,
        ResourceTypeTheme	= ( ResourceTypeStatic + 1 ) 
    } 	ResourceType;

typedef 
enum VisualElementState
    {
        ErrorResolved	= 0,
        ErrorResourceNotFound	= ( ErrorResolved + 1 ) ,
        ErrorInvalidResource	= ( ErrorResourceNotFound + 1 ) 
    } 	VisualElementState;



extern RPC_IF_HANDLE __MIDL_itf_xamlom_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xamlom_0000_0000_v0_0_s_ifspec;

#ifndef __IVisualTreeServiceCallback_INTERFACE_DEFINED__
#define __IVisualTreeServiceCallback_INTERFACE_DEFINED__

/* interface IVisualTreeServiceCallback */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IVisualTreeServiceCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IVisualTreeServiceCallback, 0xAA7A8931, 0x80E4, 0x4FEC, 0x8F, 0x3B, 0x55, 0x3F, 0x87, 0xB4, 0x96, 0x6E);
    #endif

    MIDL_INTERFACE("AA7A8931-80E4-4FEC-8F3B-553F87B4966E")
    IVisualTreeServiceCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnVisualTreeChange( 
            /* [in] */ ParentChildRelation relation,
            /* [in] */ VisualElement element,
            /* [in] */ VisualMutationType mutationType) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IVisualTreeServiceCallback_INTERFACE_DEFINED__ */


#ifndef __IVisualTreeServiceCallback2_INTERFACE_DEFINED__
#define __IVisualTreeServiceCallback2_INTERFACE_DEFINED__

/* interface IVisualTreeServiceCallback2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IVisualTreeServiceCallback2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IVisualTreeServiceCallback2, 0xBAD9EB88, 0xAE77, 0x4397, 0xB9, 0x48, 0x5F, 0xA2, 0xDB, 0x0A, 0x19, 0xEA);
    #endif

    MIDL_INTERFACE("BAD9EB88-AE77-4397-B948-5FA2DB0A19EA")
    IVisualTreeServiceCallback2 : public IVisualTreeServiceCallback
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnElementStateChanged( 
            /* [in] */ InstanceHandle element,
            /* [in] */ VisualElementState elementState,
            /* [in] */ __RPC__in LPCWSTR context) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IVisualTreeServiceCallback2_INTERFACE_DEFINED__ */


#ifndef __IVisualTreeService_INTERFACE_DEFINED__
#define __IVisualTreeService_INTERFACE_DEFINED__

/* interface IVisualTreeService */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IVisualTreeService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IVisualTreeService, 0xA593B11A, 0xD17F, 0x48BB, 0x8F, 0x66, 0x83, 0x91, 0x07, 0x31, 0xC8, 0xA5);
    #endif

    MIDL_INTERFACE("A593B11A-D17F-48BB-8F66-83910731C8A5")
    IVisualTreeService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AdviseVisualTreeChange( 
            /* [in] */ __RPC__in_opt IVisualTreeServiceCallback *pCallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnadviseVisualTreeChange( 
            /* [in] */ __RPC__in_opt IVisualTreeServiceCallback *pCallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEnums( 
            /* [out] */ __RPC__out unsigned int *pCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pCount) EnumType **ppEnums) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ __RPC__in BSTR typeName,
            /* [in] */ __RPC__in BSTR value,
            /* [retval][out] */ __RPC__out InstanceHandle *pInstanceHandle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPropertyValuesChain( 
            /* [in] */ InstanceHandle instanceHandle,
            /* [out] */ __RPC__out unsigned int *pSourceCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pSourceCount) PropertyChainSource **ppPropertySources,
            /* [out] */ __RPC__out unsigned int *pPropertyCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pPropertyCount) PropertyChainValue **ppPropertyValues) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetProperty( 
            /* [in] */ InstanceHandle instanceHandle,
            /* [in] */ InstanceHandle value,
            /* [in] */ unsigned int propertyIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearProperty( 
            /* [in] */ InstanceHandle instanceHandle,
            /* [in] */ unsigned int propertyIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCollectionCount( 
            /* [in] */ InstanceHandle instanceHandle,
            /* [out] */ __RPC__out unsigned int *pCollectionSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCollectionElements( 
            /* [in] */ InstanceHandle instanceHandle,
            /* [in] */ unsigned int startIndex,
            /* [out][in] */ __RPC__inout unsigned int *pElementCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pElementCount) CollectionElementValue **ppElementValues) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddChild( 
            /* [in] */ InstanceHandle parent,
            /* [in] */ InstanceHandle child,
            /* [in] */ unsigned int index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveChild( 
            /* [in] */ InstanceHandle parent,
            /* [in] */ unsigned int index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearChildren( 
            /* [in] */ InstanceHandle parent) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IVisualTreeService_INTERFACE_DEFINED__ */


#ifndef __IXamlDiagnostics_INTERFACE_DEFINED__
#define __IXamlDiagnostics_INTERFACE_DEFINED__

/* interface IXamlDiagnostics */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IXamlDiagnostics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IXamlDiagnostics, 0x18C9E2B6, 0x3F43, 0x4116, 0x9F, 0x2B, 0xFF, 0x93, 0x5D, 0x77, 0x70, 0xD2);
    #endif

    MIDL_INTERFACE("18C9E2B6-3F43-4116-9F2B-FF935D7770D2")
    IXamlDiagnostics : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDispatcher( 
            /* [retval][out] */ __RPC__deref_out_opt IInspectable **ppDispatcher) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUiLayer( 
            /* [retval][out] */ __RPC__deref_out_opt IInspectable **ppLayer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetApplication( 
            /* [retval][out] */ __RPC__deref_out_opt IInspectable **ppApplication) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIInspectableFromHandle( 
            /* [in] */ InstanceHandle instanceHandle,
            /* [retval][out] */ __RPC__deref_out_opt IInspectable **ppInstance) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHandleFromIInspectable( 
            /* [in] */ __RPC__in_opt IInspectable *pInstance,
            /* [retval][out] */ __RPC__out InstanceHandle *pHandle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HitTest( 
            /* [in] */ RECT rect,
            /* [out] */ __RPC__out unsigned int *pCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pCount) InstanceHandle **ppInstanceHandles) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterInstance( 
            /* [in] */ __RPC__in_opt IInspectable *pInstance,
            /* [retval][out] */ __RPC__out InstanceHandle *pInstanceHandle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInitializationData( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *pInitializationData) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IXamlDiagnostics_INTERFACE_DEFINED__ */


#ifndef __IBitmapData_INTERFACE_DEFINED__
#define __IBitmapData_INTERFACE_DEFINED__

/* interface IBitmapData */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBitmapData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IBitmapData, 0xd1a34ef2, 0xcad8, 0x4635, 0xa3, 0xd2, 0xfc, 0xda, 0x8d, 0x3f, 0x3c, 0xaf);
    #endif

    MIDL_INTERFACE("d1a34ef2-cad8-4635-a3d2-fcda8d3f3caf")
    IBitmapData : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CopyBytesTo( 
            /* [in] */ unsigned int sourceOffsetInBytes,
            /* [in] */ unsigned int maxBytesToCopy,
            /* [size_is][out] */ __RPC__out_ecount_full(maxBytesToCopy) byte *pvBytes,
            /* [out] */ __RPC__out unsigned int *numberOfBytesCopied) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStride( 
            /* [out] */ __RPC__out unsigned int *pStride) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBitmapDescription( 
            /* [out] */ __RPC__out BitmapDescription *pBitmapDescription) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceBitmapDescription( 
            /* [out] */ __RPC__out BitmapDescription *pBitmapDescription) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IBitmapData_INTERFACE_DEFINED__ */


#ifndef __IVisualTreeService2_INTERFACE_DEFINED__
#define __IVisualTreeService2_INTERFACE_DEFINED__

/* interface IVisualTreeService2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IVisualTreeService2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IVisualTreeService2, 0x130F5136, 0xEC43, 0x4F61, 0x89, 0xC7, 0x98, 0x01, 0xA3, 0x6D, 0x2E, 0x95);
    #endif

    MIDL_INTERFACE("130F5136-EC43-4F61-89C7-9801A36D2E95")
    IVisualTreeService2 : public IVisualTreeService
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPropertyIndex( 
            /* [in] */ InstanceHandle object,
            /* [in] */ __RPC__in LPCWSTR propertyName,
            /* [out] */ __RPC__out unsigned int *pPropertyIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ InstanceHandle object,
            /* [in] */ unsigned int propertyIndex,
            /* [out] */ __RPC__out InstanceHandle *pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplaceResource( 
            /* [in] */ InstanceHandle resourceDictionary,
            /* [in] */ InstanceHandle key,
            /* [in] */ InstanceHandle newValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderTargetBitmap( 
            /* [in] */ InstanceHandle handle,
            /* [in] */ RenderTargetBitmapOptions options,
            /* [in] */ unsigned int maxPixelWidth,
            /* [in] */ unsigned int maxPixelHeight,
            /* [out] */ __RPC__deref_out_opt IBitmapData **ppBitmapData) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IVisualTreeService2_INTERFACE_DEFINED__ */


#ifndef __IVisualTreeService3_INTERFACE_DEFINED__
#define __IVisualTreeService3_INTERFACE_DEFINED__

/* interface IVisualTreeService3 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IVisualTreeService3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IVisualTreeService3, 0x0E79C6E0, 0x85A0, 0x4BE8, 0xB4, 0x1A, 0x65, 0x5C, 0xF1, 0xFD, 0x19, 0xBD);
    #endif

    MIDL_INTERFACE("0E79C6E0-85A0-4BE8-B41A-655CF1FD19BD")
    IVisualTreeService3 : public IVisualTreeService2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ResolveResource( 
            /* [in] */ InstanceHandle resourceContext,
            /* [in] */ __RPC__in LPCWSTR resourceName,
            /* [in] */ ResourceType resourceType,
            /* [in] */ unsigned int propertyIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDictionaryItem( 
            /* [in] */ InstanceHandle dictionaryHandle,
            /* [in] */ __RPC__in LPCWSTR resourceName,
            /* [in] */ BOOL resourceIsImplicitStyle,
            /* [out] */ __RPC__out InstanceHandle *resourceHandle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddDictionaryItem( 
            /* [in] */ InstanceHandle dictionaryHandle,
            /* [in] */ InstanceHandle resourceKey,
            /* [in] */ InstanceHandle resourceHandle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveDictionaryItem( 
            /* [in] */ InstanceHandle dictionaryHandle,
            /* [in] */ InstanceHandle resourceKey) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IVisualTreeService3_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_xamlom_0000_0007 */
/* [local] */ 

#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */ 
#pragma endregion


extern RPC_IF_HANDLE __MIDL_itf_xamlom_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xamlom_0000_0007_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     __RPC__in unsigned long *, __RPC__in LPSAFEARRAY * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize64(     __RPC__in unsigned long *, unsigned long            , __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal64(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal64(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree64(     __RPC__in unsigned long *, __RPC__in LPSAFEARRAY * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

#pragma endregion  // xamlOM_h

#pragma region windows_ui_xaml_hosting_desktopwindowxamlsource_h

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include <rpc.h>
#include <rpcndr.h>

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include <windows.h>
#include <ole2.h>
#endif /*COM_NO_WINDOWS_H*/

#ifndef __windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_h__
#define __windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
// #pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDesktopWindowXamlSourceNative_FWD_DEFINED__
#define __IDesktopWindowXamlSourceNative_FWD_DEFINED__
typedef interface IDesktopWindowXamlSourceNative IDesktopWindowXamlSourceNative;

#endif 	/* __IDesktopWindowXamlSourceNative_FWD_DEFINED__ */


#ifndef __IDesktopWindowXamlSourceNative2_FWD_DEFINED__
#define __IDesktopWindowXamlSourceNative2_FWD_DEFINED__
typedef interface IDesktopWindowXamlSourceNative2 IDesktopWindowXamlSourceNative2;

#endif 	/* __IDesktopWindowXamlSourceNative2_FWD_DEFINED__ */


/* header files for imported files */
#include <oaidl.h>

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0000 */
/* [local] */ 

#if (NTDDI_VERSION >= NTDDI_WIN10_RS5)


extern RPC_IF_HANDLE __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0000_v0_0_s_ifspec;

#ifndef __IDesktopWindowXamlSourceNative_INTERFACE_DEFINED__
#define __IDesktopWindowXamlSourceNative_INTERFACE_DEFINED__

/* interface IDesktopWindowXamlSourceNative */
/* [unique][local][uuid][object] */ 


EXTERN_C const IID IID_IDesktopWindowXamlSourceNative;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IDesktopWindowXamlSourceNative, 0x3cbcf1bf, 0x2f76, 0x4e9c, 0x96, 0xab, 0xe8, 0x4b, 0x37, 0x97, 0x25, 0x54);
    #endif

    MIDL_INTERFACE("3cbcf1bf-2f76-4e9c-96ab-e84b37972554")
    IDesktopWindowXamlSourceNative : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AttachToWindow( 
            /* [annotation][in] */ 
            _In_  HWND parentWnd) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowHandle( 
            /* [retval][out] */ HWND *hWnd) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IDesktopWindowXamlSourceNative_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0001 */
/* [local] */ 

#endif // NTDDI_VERSION >= NTDDI_WIN10_RS5
#if (NTDDI_VERSION >= NTDDI_WIN10_19H1)


extern RPC_IF_HANDLE __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0001_v0_0_s_ifspec;

#ifndef __IDesktopWindowXamlSourceNative2_INTERFACE_DEFINED__
#define __IDesktopWindowXamlSourceNative2_INTERFACE_DEFINED__

/* interface IDesktopWindowXamlSourceNative2 */
/* [unique][local][uuid][object] */ 


EXTERN_C const IID IID_IDesktopWindowXamlSourceNative2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    #ifdef __CRT_UUID_DECL
    __CRT_UUID_DECL(IDesktopWindowXamlSourceNative2, 0xe3dcd8c7, 0x3057, 0x4692, 0x99, 0xc3, 0x7b, 0x77, 0x20, 0xaf, 0xda, 0x31);
    #endif

    MIDL_INTERFACE("e3dcd8c7-3057-4692-99c3-7b7720afda31")
    IDesktopWindowXamlSourceNative2 : public IDesktopWindowXamlSourceNative
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage( 
            /* [annotation][in] */ 
            _In_  const MSG *message,
            /* [retval][out] */ BOOL *result) = 0;
        
    };
    
    
#else 	/* C style interface */
#error Only C++ style interface is supported
#endif 	/* C style interface */




#endif 	/* __IDesktopWindowXamlSourceNative2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0002 */
/* [local] */ 

#endif // NTDDI_VERSION >= NTDDI_WIN10_19H1


extern RPC_IF_HANDLE __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_windows2Eui2Examl2Ehosting2Edesktopwindowxamlsource_0000_0002_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

#pragma endregion  // windows_ui_xaml_hosting_desktopwindowxamlsource_h

#pragma region visualtreewatcher_hpp

#include <winrt/Windows.UI.Xaml.h>

struct VisualTreeWatcher : winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
{
    VisualTreeWatcher() = default;

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    void SetXamlDiagnostics(winrt::com_ptr<IXamlDiagnostics> diagnostics);

    ~VisualTreeWatcher();

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element, VisualElementState elementState, LPCWSTR context) noexcept override;

    template<typename T>
    T FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));

        return obj.as<T>();
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

#include <winrt/Windows.UI.Xaml.Hosting.h>

void VisualTreeWatcher::SetXamlDiagnostics(winrt::com_ptr<IXamlDiagnostics> diagnostics)
{
    m_XamlDiagnostics = std::move(diagnostics);
}

VisualTreeWatcher::~VisualTreeWatcher() = default;

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try
{
    if (g_disabled || GetCurrentThreadId() != g_targetThreadId)
    {
        return S_OK;
    }

    Wh_Log(L"========================================");

    switch (mutationType)
    {
    case Add:
        Wh_Log(L"Mutation type: Add");
        break;

    case Remove:
        Wh_Log(L"Mutation type: Remove");
        break;

    default:
        Wh_Log(L"Mutation type: %d", static_cast<int>(mutationType));
        break;
    }

    Wh_Log(L"Element type: %s", element.Type);

    const auto inspectable = FromHandle<wf::IInspectable>(element.Handle);

    auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
    if (!frameworkElement)
    {
        const auto desktopXamlSource = FromHandle<wuxh::DesktopWindowXamlSource>(element.Handle);
        frameworkElement = desktopXamlSource.Content().try_as<wux::FrameworkElement>();
    }

    if (frameworkElement)
    {
        Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
    }

    if (frameworkElement && mutationType == Add)
    {
        ApplyCustomizations(frameworkElement);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);

    // Returning an error prevents (some?) further messages, always return
    // success.
    // return hr;
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept
{
    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = { 0xc85d8cc7, 0x5463, 0x40e8, { 0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5 } };

struct WindhawkTAP : winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override;

private:
    template<typename T>
    static winrt::com_ptr<T> FromIUnknown(IUnknown *pSite)
    {
        winrt::com_ptr<IUnknown> site;
        site.copy_from(pSite);

        return site.as<T>();
    }

    winrt::com_ptr<IVisualTreeService3> visualTreeService;
    winrt::com_ptr<VisualTreeWatcher> visualTreeWatcher;
};

#pragma endregion  // tap_hpp

#pragma region tap_cpp

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite) try
{
    if (visualTreeService && visualTreeWatcher)
    {
        winrt::check_hresult(visualTreeService->UnadviseVisualTreeChange(visualTreeWatcher.get()));
        visualTreeWatcher->SetXamlDiagnostics(nullptr);
    }

    visualTreeService = FromIUnknown<IVisualTreeService3>(pUnkSite);

    if (visualTreeService)
    {
        if (!visualTreeWatcher)
        {
            visualTreeWatcher = winrt::make_self<VisualTreeWatcher>();
        }

        visualTreeWatcher->SetXamlDiagnostics(visualTreeService.as<IXamlDiagnostics>());
        winrt::check_hresult(visualTreeService->AdviseVisualTreeChange(visualTreeWatcher.get()));
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept
{
    return visualTreeService.as(riid, ppvSite);
}

#pragma endregion  // tap_cpp

#pragma region simplefactory_hpp

#include <Unknwn.h>

template<class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try
    {
        if (!pUnkOuter)
        {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        else
        {
            return CLASS_E_NOAGGREGATION;
        }
    }
    catch (...)
    {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma endregion  // simplefactory_hpp

#pragma region module_cpp

#include <combaseapi.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try
{
    if (rclsid == CLSID_WindhawkTAP)
    {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}
catch (...)
{
    return winrt::to_hresult();
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow(void)
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

#pragma clang diagnostic pop

#pragma endregion  // module_cpp

#pragma region api_cpp

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HMODULE GetCurrentModuleHandle()
{
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

HRESULT InjectWindhawkTAP() noexcept
{
    HMODULE module = GetCurrentModuleHandle();
    if (!module)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location)))
    {
    case 0:
    case ARRAYSIZE(location):
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
    if (!wux) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HRESULT hr2 = ixde(L"VisualDiagConnection1", GetCurrentProcessId(), nullptr, location, CLSID_WindhawkTAP, nullptr);
    if (FAILED(hr2)) [[unlikely]]
    {
        return hr2;
    }

    return S_OK;
}

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

HANDLE g_restartExplorerPromptThread;
std::atomic<HWND> g_restartExplorerPromptWindow;

using PropertyKeyValue =
    std::pair<DependencyProperty, winrt::Windows::Foundation::IInspectable>;

struct ElementMatcher {
    std::wstring type;
    std::wstring name;
};

struct ElementPropertyOverrides {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    std::vector<PropertyKeyValue> propertyValues;
};

bool g_propertyOverridesLoaded;
std::vector<ElementPropertyOverrides> g_propertyOverrides;
DWORD g_propertyOverridesUpdateCount = 0;

bool TestElementMatcher(FrameworkElement element,
                        const ElementMatcher& matcher) {
    if (!matcher.type.empty() &&
        matcher.type != winrt::get_class_name(element)) {
        return false;
    }

    if (!matcher.name.empty() && matcher.name != element.Name()) {
        return false;
    }

    return true;
}

const std::vector<PropertyKeyValue>* FindElementPropertyOverrides(
    FrameworkElement element) {
    for (const auto& override : g_propertyOverrides) {
        if (!TestElementMatcher(element, override.elementMatcher)) {
            continue;
        }

        auto parentElementIter = element;
        bool parentElementMatchFailed = false;

        for (const auto& matcher : override.parentElementMatchers) {
            // Using parentElementIter.Parent() was sometimes returning null.
            parentElementIter =
                Media::VisualTreeHelper::GetParent(parentElementIter)
                    .try_as<FrameworkElement>();
            if (!parentElementIter) {
                parentElementMatchFailed = true;
                break;
            }

            if (!TestElementMatcher(parentElementIter, matcher)) {
                parentElementMatchFailed = true;
                break;
            }
        }

        if (!parentElementMatchFailed) {
            return &override.propertyValues;
        }
    }

    return nullptr;
}

void ProcessAllStylesFromSettings();
void ProcessResourceVariablesFromSettings();

void ApplyCustomizations(FrameworkElement element) {
    if (!g_propertyOverridesLoaded) {
        ProcessAllStylesFromSettings();
        ProcessResourceVariablesFromSettings();
        g_propertyOverridesLoaded = true;
    }

    auto propertyValues = FindElementPropertyOverrides(element);
    if (!propertyValues) {
        return;
    }

    Wh_Log(L"Applying %zu styles", propertyValues->size());

    auto elementDo = element.as<DependencyObject>();

    for (const auto& [property, value] : *propertyValues) {
        element.SetValue(property, value);
        elementDo.RegisterPropertyChangedCallback(
            property,
            [value = value,
             propertyOverridesUpdateCount = g_propertyOverridesUpdateCount](
                DependencyObject sender, DependencyProperty property) {
                if (g_disabled || propertyOverridesUpdateCount !=
                                      g_propertyOverridesUpdateCount) {
                    return;
                }

                auto element = sender.try_as<FrameworkElement>();
                if (element) {
                    Wh_Log(L"Re-applying style for %s",
                           winrt::get_class_name(element).c_str());
                    element.SetValue(property, value);
                }
            });
    }
}

// https://stackoverflow.com/a/5665377
std::wstring EscapeXmlAttribute(std::wstring_view data) {
    std::wstring buffer;
    buffer.reserve(data.size());
    for (size_t pos = 0; pos != data.size(); ++pos) {
        switch (data[pos]) {
            case '&':
                buffer.append(L"&amp;");
                break;
            case '\"':
                buffer.append(L"&quot;");
                break;
            // case '\'':
            //     buffer.append(L"&apos;");
            //     break;
            case '<':
                buffer.append(L"&lt;");
                break;
            case '>':
                buffer.append(L"&gt;");
                break;
            default:
                buffer.append(&data[pos], 1);
                break;
        }
    }

    return buffer;
}

// https://stackoverflow.com/a/54364173
std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

struct ResolveTypeAndStylesResult {
    std::wstring type;
    std::vector<PropertyKeyValue> propertyValues;
};

ResolveTypeAndStylesResult ResolveTypeAndStyles(
    std::wstring_view targetType,
    std::vector<std::pair<std::wstring, std::wstring>> styles) {
    std::wstring xaml =
        LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls"
    xmlns:taskbar="using:Taskbar"
    xmlns:udk="using:WindowsUdk.UI.Shell"
    xmlns:systemtray="using:SystemTray">
    <Style)";

    xaml += L" TargetType=\"";
    xaml += EscapeXmlAttribute(targetType);
    xaml += L"\">\n";

    for (const auto& [property, value] : styles) {
        xaml += L"        <Setter Property=\"";
        xaml += EscapeXmlAttribute(property);
        xaml += L"\" Value=\"";
        xaml += EscapeXmlAttribute(value);
        xaml += L"\" />\n";
    }

    xaml +=
        LR"(    </Style>
</ResourceDictionary>)";

    Wh_Log(L"======================================== XAML:");
    std::wstringstream ss(xaml);
    std::wstring line;
    while (std::getline(ss, line, L'\n')) {
        Wh_Log(L"%s", line.c_str());
    }
    Wh_Log(L"========================================");

    auto resourceDictionary =
        Markup::XamlReader::Load(xaml).as<ResourceDictionary>();

    auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
    auto style = styleInspectable.as<Style>();

    std::vector<PropertyKeyValue> propertyValues;

    for (const auto& setterBase : style.Setters()) {
        const auto setter = setterBase.as<Setter>();
        propertyValues.push_back({
            setter.Property(),
            setter.Value(),
        });
    }

    Wh_Log(L"%s: %zu styles", style.TargetType().Name.c_str(),
           propertyValues.size());

    return {
        .type = std::wstring(style.TargetType().Name),
        .propertyValues = std::move(propertyValues),
    };
}

void AddStylesToOverrides(
    std::wstring_view target,
    std::vector<std::pair<std::wstring, std::wstring>> styles) {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    std::vector<PropertyKeyValue> propertyValues;

    auto targetParts = SplitStringView(target, L" > ");

    bool first = true;
    for (auto i = targetParts.rbegin(); i != targetParts.rend(); ++i) {
        const auto& targetPart = *i;

        std::wstring_view name;
        std::wstring_view targetType;
        if (auto pos = targetPart.find(L'#'); pos != targetPart.npos) {
            targetType = TrimStringView(targetPart.substr(0, pos));
            name = TrimStringView(targetPart.substr(pos + 1));
        } else {
            targetType = TrimStringView(targetPart);
        }

        if (targetType.empty()) {
            Wh_Log(L"TargetType must not be empty, skipping styles");
            return;
        }

        auto resolved = ResolveTypeAndStyles(targetType, std::move(styles));
        styles.clear();

        if (first) {
            elementMatcher.type = std::move(resolved.type);
            elementMatcher.name = std::wstring(name);
            propertyValues = std::move(resolved.propertyValues);
        } else {
            parentElementMatchers.push_back({
                .type = std::move(resolved.type),
                .name = std::wstring(name),
            });
        }

        first = false;
    }

    g_propertyOverrides.push_back(ElementPropertyOverrides{
        .elementMatcher = std::move(elementMatcher),
        .parentElementMatchers = std::move(parentElementMatchers),
        .propertyValues = std::move(propertyValues),
    });
}

bool ProcessSingleTargetStylesFromSettings(int index) {
    string_setting_unique_ptr targetStringSetting(
        Wh_GetStringSetting(L"controlStyles[%d].target", index));
    if (!*targetStringSetting.get()) {
        return false;
    }

    Wh_Log(L"Processing styles for %s", targetStringSetting.get());

    std::vector<std::pair<std::wstring, std::wstring>> styles;

    for (int styleIndex = 0;; styleIndex++) {
        string_setting_unique_ptr styleSetting(Wh_GetStringSetting(
            L"controlStyles[%d].styles[%d]", index, styleIndex));
        if (!*styleSetting.get()) {
            break;
        }

        std::wstring_view style = styleSetting.get();

        if (auto pos = style.find(L'='); pos != style.npos) {
            styles.push_back({
                std::wstring(TrimStringView(style.substr(0, pos))),
                std::wstring(TrimStringView(style.substr(pos + 1))),
            });
        } else {
            Wh_Log(L"Style has no '=' symbol, skipping: %s",
                   styleSetting.get());
        }
    }

    if (styles.size() > 0) {
        AddStylesToOverrides(targetStringSetting.get(), std::move(styles));
    }

    return true;
}

void ProcessAllStylesFromSettings() {
    for (int i = 0;; i++) {
        try {
            if (!ProcessSingleTargetStylesFromSettings(i)) {
                break;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X", ex.code());
        }
    }
}

bool ProcessSingleResourceVariableFromSettings(int index) {
    string_setting_unique_ptr variableKeyStringSetting(
        Wh_GetStringSetting(L"resourceVariables[%d].variableKey", index));
    if (!*variableKeyStringSetting.get()) {
        return false;
    }

    Wh_Log(L"Processing resource variable %s", variableKeyStringSetting.get());

    std::wstring_view variableKey = variableKeyStringSetting.get();

    auto resources = Application::Current().Resources();

    auto resource = resources.Lookup(winrt::box_value(variableKey));

    // Example: Windows.Foundation.IReference`1<Windows.UI.Xaml.Thickness>
    auto resourceClassName = winrt::get_class_name(resource);
    if (resourceClassName.starts_with(L"Windows.Foundation.IReference`1<") &&
        resourceClassName.ends_with(L'>')) {
        size_t prefixSize = sizeof("Windows.Foundation.IReference`1<") - 1;
        resourceClassName =
            winrt::hstring(resourceClassName.data() + prefixSize,
                           resourceClassName.size() - prefixSize - 1);
    }

    auto resourceTypeName = Interop::TypeName{resourceClassName};

    string_setting_unique_ptr valueStringSetting(
        Wh_GetStringSetting(L"resourceVariables[%d].value", index));

    std::wstring_view value = valueStringSetting.get();

    resources.Insert(winrt::box_value(variableKey),
                     Markup::XamlBindingHelper::ConvertValue(
                         resourceTypeName, winrt::box_value(value)));

    return true;
}

void ProcessResourceVariablesFromSettings() {
    for (int i = 0;; i++) {
        try {
            if (!ProcessSingleResourceVariableFromSettings(i)) {
                break;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X", ex.code());
        }
    }
}

void InitializeSettingsAndTap() {
    g_targetThreadId = GetCurrentThreadId();

    g_propertyOverridesLoaded = false;
    g_propertyOverrides.clear();
    g_propertyOverridesUpdateCount++;

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
    }
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

    WCHAR className[64];
    if (!g_targetThreadId && hWndParent &&
        GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 &&
        GetClassName(hWndParent, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Initializing - Created DesktopWindowContentBridge window");
        InitializeSettingsAndTap();
    }

    return hWnd;
}

void PromptForExplorerRestart() {
    if (!Wh_GetIntSetting(L"promptForExplorerRestart")) {
        return;
    }

    if (g_restartExplorerPromptThread) {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) !=
            WAIT_OBJECT_0) {
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
                .pszWindowTitle = L"Windows 11 Taskbar Styler",
                .pszMainIcon = TD_INFORMATION_ICON,
                .pszContent =
                    L"Explorer might need to be restarted to apply the new "
                    L"styles. Restart now?",
                .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam, LONG_PTR lpRefData)
                                  WINAPI -> HRESULT {
                    switch (msg) {
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
                button == IDYES) {
                WCHAR commandLine[] =
                    L"cmd.exe /c "
                    L"\"taskkill /F /IM explorer.exe & start explorer\"";
                STARTUPINFO si = {
                    .cb = sizeof(si),
                };
                PROCESS_INFORMATION pi{};
                if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE,
                                  0, nullptr, nullptr, &si, &pi)) {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
            }

            return 0;
        },
        nullptr, 0, nullptr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

HWND GetTaskbarUiWnd() {
    DWORD dwProcessId;
    DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd || !GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) ||
        dwProcessId != dwCurrentProcessId) {
        return nullptr;
    }

    return FindWindowEx(hTaskbarWnd, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge",
                        nullptr);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (g_disabled) {
        Wh_Log(L"Already loaded");
        PromptForExplorerRestart();
        g_disabled = false;
        return TRUE;
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Initializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) WINAPI { InitializeSettingsAndTap(); },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    HWND restartExplorerPromptWindow = g_restartExplorerPromptWindow;
    if (restartExplorerPromptWindow) {
        PostMessage(restartExplorerPromptWindow, WM_CLOSE, 0, 0);
    }

    if (g_restartExplorerPromptThread) {
        WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
        CloseHandle(g_restartExplorerPromptThread);
        g_restartExplorerPromptThread = nullptr;
    }

    // TODO: Explore a way to stop the monitoring and unload the dll.
    g_disabled = true;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    PromptForExplorerRestart();

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Initializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) WINAPI { InitializeSettingsAndTap(); },
            nullptr);
    }
}
