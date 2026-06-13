// ==WindhawkMod==
// @id              explorer-frame-classic
// @name            Classic Explorer navigation bar
// @description     Restores the classic Explorer navigation bar to the version before the Windows 11 "Moments 4" update
// @version         1.0.8
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lversion
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
# Classic Explorer navigation bar

Restores the classic Explorer navigation bar to the version before the Windows
11 "Moments 4" update.

**Note**: You may need to restart Explorer to apply the changes.

**The new Windows "Moments 4" update version**:

![Moments 4 screenshot](https://i.imgur.com/fOmI2Rs.png)

**After installing this mod, with the "Classic navigation bar" configuration**:

![Classic navigation bar screenshot](https://i.imgur.com/Hnvjquv.png)

**After installing this mod, with the "Classic ribbon UI (no tabs)"
configuration**:

![Classic ribbon UI (no tabs) screenshot](https://i.imgur.com/SCBbirW.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- explorerStyle: classicNavigationBar
  $name: Explorer style
  $options:
  - classicNavigationBar: Classic navigation bar
  - classicRibbonUI: Classic ribbon UI (no tabs)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <inspectable.h>

#include <winrt/base.h>
#include <winrt/windows.foundation.collections.h>

enum class ExplorerStyle {
    classicNavigationBar,
    classicRibbonUI,
};

struct {
    ExplorerStyle explorerStyle;
} g_settings;

std::atomic<bool> g_fileExplorerExtensionsLoaded;

#pragma region WASDK
namespace WASDK {

// C3C01020-320C-5CF6-9D24-D396BBFA4D8B
constexpr winrt::guid IID_IUIElement{
    0xC3C01020,
    0x320C,
    0x5CF6,
    {0x9D, 0x24, 0xD3, 0x96, 0xBB, 0xFA, 0x4D, 0x8B}};

// FE08F13D-DC6A-5495-AD44-C2D8D21863B0
constexpr winrt::guid IID_IFrameworkElement{
    0xFE08F13D,
    0xDC6A,
    0x5495,
    {0xAD, 0x44, 0xC2, 0xD8, 0xD2, 0x18, 0x63, 0xB0}};

// C4496219-9014-58A1-B4AD-C5044913A5BB
constexpr winrt::guid IID_Controls_IGrid{
    0xC4496219,
    0x9014,
    0x58A1,
    {0xB4, 0xAD, 0xC5, 0x04, 0x49, 0x13, 0xA5, 0xBB}};

// clang-format off
struct IUIElement : ::IInspectable {
    virtual int32_t __stdcall get_DesiredSize(winrt::Windows::Foundation::Size*) noexcept = 0;
    virtual int32_t __stdcall get_AllowDrop(bool*) noexcept = 0;
    virtual int32_t __stdcall put_AllowDrop(bool) noexcept = 0;
    virtual int32_t __stdcall get_Opacity(double*) noexcept = 0;
    virtual int32_t __stdcall put_Opacity(double) noexcept = 0;
    virtual int32_t __stdcall get_Clip(void**) noexcept = 0;
    virtual int32_t __stdcall put_Clip(void*) noexcept = 0;
    virtual int32_t __stdcall get_RenderTransform(void**) noexcept = 0;
    virtual int32_t __stdcall put_RenderTransform(void*) noexcept = 0;
    virtual int32_t __stdcall get_Projection(void**) noexcept = 0;
    virtual int32_t __stdcall put_Projection(void*) noexcept = 0;
    virtual int32_t __stdcall get_Transform3D(void**) noexcept = 0;
    virtual int32_t __stdcall put_Transform3D(void*) noexcept = 0;
    virtual int32_t __stdcall get_RenderTransformOrigin(winrt::Windows::Foundation::Point*) noexcept = 0;
    virtual int32_t __stdcall put_RenderTransformOrigin(winrt::Windows::Foundation::Point) noexcept = 0;
    virtual int32_t __stdcall get_IsHitTestVisible(bool*) noexcept = 0;
    virtual int32_t __stdcall put_IsHitTestVisible(bool) noexcept = 0;
    virtual int32_t __stdcall get_Visibility(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_Visibility(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_RenderSize(winrt::Windows::Foundation::Size*) noexcept = 0;
    virtual int32_t __stdcall get_UseLayoutRounding(bool*) noexcept = 0;
    virtual int32_t __stdcall put_UseLayoutRounding(bool) noexcept = 0;
    virtual int32_t __stdcall get_Transitions(void**) noexcept = 0;
    virtual int32_t __stdcall put_Transitions(void*) noexcept = 0;
    virtual int32_t __stdcall get_CacheMode(void**) noexcept = 0;
    virtual int32_t __stdcall put_CacheMode(void*) noexcept = 0;
    virtual int32_t __stdcall get_IsTapEnabled(bool*) noexcept = 0;
    virtual int32_t __stdcall put_IsTapEnabled(bool) noexcept = 0;
    virtual int32_t __stdcall get_IsDoubleTapEnabled(bool*) noexcept = 0;
    virtual int32_t __stdcall put_IsDoubleTapEnabled(bool) noexcept = 0;
    virtual int32_t __stdcall get_CanDrag(bool*) noexcept = 0;
    virtual int32_t __stdcall put_CanDrag(bool) noexcept = 0;
    virtual int32_t __stdcall get_IsRightTapEnabled(bool*) noexcept = 0;
    virtual int32_t __stdcall put_IsRightTapEnabled(bool) noexcept = 0;
    virtual int32_t __stdcall get_IsHoldingEnabled(bool*) noexcept = 0;
    virtual int32_t __stdcall put_IsHoldingEnabled(bool) noexcept = 0;
    virtual int32_t __stdcall get_ManipulationMode(uint32_t*) noexcept = 0;
    virtual int32_t __stdcall put_ManipulationMode(uint32_t) noexcept = 0;
    virtual int32_t __stdcall get_PointerCaptures(void**) noexcept = 0;
    virtual int32_t __stdcall get_ContextFlyout(void**) noexcept = 0;
    virtual int32_t __stdcall put_ContextFlyout(void*) noexcept = 0;
    virtual int32_t __stdcall get_CompositeMode(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_CompositeMode(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_Lights(void**) noexcept = 0;
    virtual int32_t __stdcall get_CanBeScrollAnchor(bool*) noexcept = 0;
    virtual int32_t __stdcall put_CanBeScrollAnchor(bool) noexcept = 0;
    virtual int32_t __stdcall get_ExitDisplayModeOnAccessKeyInvoked(bool*) noexcept = 0;
    virtual int32_t __stdcall put_ExitDisplayModeOnAccessKeyInvoked(bool) noexcept = 0;
    virtual int32_t __stdcall get_IsAccessKeyScope(bool*) noexcept = 0;
    virtual int32_t __stdcall put_IsAccessKeyScope(bool) noexcept = 0;
    virtual int32_t __stdcall get_AccessKeyScopeOwner(void**) noexcept = 0;
    virtual int32_t __stdcall put_AccessKeyScopeOwner(void*) noexcept = 0;
    virtual int32_t __stdcall get_AccessKey(void**) noexcept = 0;
    virtual int32_t __stdcall put_AccessKey(void*) noexcept = 0;
    virtual int32_t __stdcall get_KeyTipPlacementMode(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_KeyTipPlacementMode(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_KeyTipHorizontalOffset(double*) noexcept = 0;
    virtual int32_t __stdcall put_KeyTipHorizontalOffset(double) noexcept = 0;
    virtual int32_t __stdcall get_KeyTipVerticalOffset(double*) noexcept = 0;
    virtual int32_t __stdcall put_KeyTipVerticalOffset(double) noexcept = 0;
    virtual int32_t __stdcall get_KeyTipTarget(void**) noexcept = 0;
    virtual int32_t __stdcall put_KeyTipTarget(void*) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusKeyboardNavigation(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusKeyboardNavigation(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusUpNavigationStrategy(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusUpNavigationStrategy(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusDownNavigationStrategy(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusDownNavigationStrategy(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusLeftNavigationStrategy(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusLeftNavigationStrategy(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusRightNavigationStrategy(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusRightNavigationStrategy(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_KeyboardAccelerators(void**) noexcept = 0;
    virtual int32_t __stdcall get_KeyboardAcceleratorPlacementTarget(void**) noexcept = 0;
    virtual int32_t __stdcall put_KeyboardAcceleratorPlacementTarget(void*) noexcept = 0;
    virtual int32_t __stdcall get_KeyboardAcceleratorPlacementMode(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_KeyboardAcceleratorPlacementMode(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_HighContrastAdjustment(uint32_t*) noexcept = 0;
    virtual int32_t __stdcall put_HighContrastAdjustment(uint32_t) noexcept = 0;
    virtual int32_t __stdcall get_TabFocusNavigation(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_TabFocusNavigation(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_OpacityTransition(void**) noexcept = 0;
    virtual int32_t __stdcall put_OpacityTransition(void*) noexcept = 0;
    virtual int32_t __stdcall get_Translation(winrt::Windows::Foundation::Numerics::float3*) noexcept = 0;
    virtual int32_t __stdcall put_Translation(winrt::Windows::Foundation::Numerics::float3) noexcept = 0;
    virtual int32_t __stdcall get_TranslationTransition(void**) noexcept = 0;
    virtual int32_t __stdcall put_TranslationTransition(void*) noexcept = 0;
    virtual int32_t __stdcall get_Rotation(float*) noexcept = 0;
    virtual int32_t __stdcall put_Rotation(float) noexcept = 0;
    virtual int32_t __stdcall get_RotationTransition(void**) noexcept = 0;
    virtual int32_t __stdcall put_RotationTransition(void*) noexcept = 0;
    virtual int32_t __stdcall get_Scale(winrt::Windows::Foundation::Numerics::float3*) noexcept = 0;
    virtual int32_t __stdcall put_Scale(winrt::Windows::Foundation::Numerics::float3) noexcept = 0;
    virtual int32_t __stdcall get_ScaleTransition(void**) noexcept = 0;
    virtual int32_t __stdcall put_ScaleTransition(void*) noexcept = 0;
    virtual int32_t __stdcall get_TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4*) noexcept = 0;
    virtual int32_t __stdcall put_TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4) noexcept = 0;
    virtual int32_t __stdcall get_CenterPoint(winrt::Windows::Foundation::Numerics::float3*) noexcept = 0;
    virtual int32_t __stdcall put_CenterPoint(winrt::Windows::Foundation::Numerics::float3) noexcept = 0;
    virtual int32_t __stdcall get_RotationAxis(winrt::Windows::Foundation::Numerics::float3*) noexcept = 0;
    virtual int32_t __stdcall put_RotationAxis(winrt::Windows::Foundation::Numerics::float3) noexcept = 0;
    virtual int32_t __stdcall get_ActualOffset(winrt::Windows::Foundation::Numerics::float3*) noexcept = 0;
    virtual int32_t __stdcall get_ActualSize(winrt::Windows::Foundation::Numerics::float2*) noexcept = 0;
    virtual int32_t __stdcall get_XamlRoot(void**) noexcept = 0;
    virtual int32_t __stdcall put_XamlRoot(void*) noexcept = 0;
    virtual int32_t __stdcall get_Shadow(void**) noexcept = 0;
    virtual int32_t __stdcall put_Shadow(void*) noexcept = 0;
    virtual int32_t __stdcall get_RasterizationScale(double*) noexcept = 0;
    virtual int32_t __stdcall put_RasterizationScale(double) noexcept = 0;
    virtual int32_t __stdcall get_FocusState(int32_t*) noexcept = 0;
    virtual int32_t __stdcall get_UseSystemFocusVisuals(bool*) noexcept = 0;
    virtual int32_t __stdcall put_UseSystemFocusVisuals(bool) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusLeft(void**) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusLeft(void*) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusRight(void**) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusRight(void*) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusUp(void**) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusUp(void*) noexcept = 0;
    virtual int32_t __stdcall get_XYFocusDown(void**) noexcept = 0;
    virtual int32_t __stdcall put_XYFocusDown(void*) noexcept = 0;
    virtual int32_t __stdcall get_IsTabStop(bool*) noexcept = 0;
    virtual int32_t __stdcall put_IsTabStop(bool) noexcept = 0;
    virtual int32_t __stdcall get_TabIndex(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_TabIndex(int32_t) noexcept = 0;
    virtual int32_t __stdcall add_KeyUp(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_KeyUp(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_KeyDown(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_KeyDown(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_GotFocus(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_GotFocus(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_LostFocus(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_LostFocus(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_DragStarting(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_DragStarting(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_DropCompleted(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_DropCompleted(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_CharacterReceived(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_CharacterReceived(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_DragEnter(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_DragEnter(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_DragLeave(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_DragLeave(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_DragOver(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_DragOver(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_Drop(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_Drop(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerPressed(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerPressed(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerMoved(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerMoved(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerReleased(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerReleased(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerEntered(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerEntered(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerExited(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerExited(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerCaptureLost(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerCaptureLost(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerCanceled(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerCanceled(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PointerWheelChanged(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PointerWheelChanged(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_Tapped(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_Tapped(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_DoubleTapped(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_DoubleTapped(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_Holding(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_Holding(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ContextRequested(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ContextRequested(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ContextCanceled(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ContextCanceled(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_RightTapped(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_RightTapped(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ManipulationStarting(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ManipulationStarting(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ManipulationInertiaStarting(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ManipulationInertiaStarting(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ManipulationStarted(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ManipulationStarted(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ManipulationDelta(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ManipulationDelta(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ManipulationCompleted(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ManipulationCompleted(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_AccessKeyDisplayRequested(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_AccessKeyDisplayRequested(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_AccessKeyDisplayDismissed(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_AccessKeyDisplayDismissed(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_AccessKeyInvoked(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_AccessKeyInvoked(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ProcessKeyboardAccelerators(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ProcessKeyboardAccelerators(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_GettingFocus(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_GettingFocus(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_LosingFocus(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_LosingFocus(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_NoFocusCandidateFound(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_NoFocusCandidateFound(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PreviewKeyDown(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PreviewKeyDown(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_PreviewKeyUp(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_PreviewKeyUp(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_BringIntoViewRequested(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_BringIntoViewRequested(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall Measure(winrt::Windows::Foundation::Size) noexcept = 0;
    virtual int32_t __stdcall Arrange(winrt::Windows::Foundation::Rect) noexcept = 0;
    virtual int32_t __stdcall CapturePointer(void*, bool*) noexcept = 0;
    virtual int32_t __stdcall ReleasePointerCapture(void*) noexcept = 0;
    virtual int32_t __stdcall ReleasePointerCaptures() noexcept = 0;
    virtual int32_t __stdcall AddHandler(void*, void*, bool) noexcept = 0;
    virtual int32_t __stdcall RemoveHandler(void*, void*) noexcept = 0;
    virtual int32_t __stdcall TransformToVisual(void*, void**) noexcept = 0;
    virtual int32_t __stdcall InvalidateMeasure() noexcept = 0;
    virtual int32_t __stdcall InvalidateArrange() noexcept = 0;
    virtual int32_t __stdcall UpdateLayout() noexcept = 0;
    virtual int32_t __stdcall CancelDirectManipulations(bool*) noexcept = 0;
    virtual int32_t __stdcall StartDragAsync(void*, void**) noexcept = 0;
    virtual int32_t __stdcall StartBringIntoView() noexcept = 0;
    virtual int32_t __stdcall StartBringIntoViewWithOptions(void*) noexcept = 0;
    virtual int32_t __stdcall TryInvokeKeyboardAccelerator(void*) noexcept = 0;
    virtual int32_t __stdcall Focus(int32_t, bool*) noexcept = 0;
    virtual int32_t __stdcall StartAnimation(void*) noexcept = 0;
    virtual int32_t __stdcall StopAnimation(void*) noexcept = 0;
};

struct IFrameworkElement : ::IInspectable {
    virtual int32_t __stdcall get_Triggers(void**) noexcept = 0;
    virtual int32_t __stdcall get_Resources(void**) noexcept = 0;
    virtual int32_t __stdcall put_Resources(void*) noexcept = 0;
    virtual int32_t __stdcall get_Tag(void**) noexcept = 0;
    virtual int32_t __stdcall put_Tag(void*) noexcept = 0;
    virtual int32_t __stdcall get_Language(void**) noexcept = 0;
    virtual int32_t __stdcall put_Language(void*) noexcept = 0;
    virtual int32_t __stdcall get_ActualWidth(double*) noexcept = 0;
    virtual int32_t __stdcall get_ActualHeight(double*) noexcept = 0;
    virtual int32_t __stdcall get_Width(double*) noexcept = 0;
    virtual int32_t __stdcall put_Width(double) noexcept = 0;
    virtual int32_t __stdcall get_Height(double*) noexcept = 0;
    virtual int32_t __stdcall put_Height(double) noexcept = 0;
    virtual int32_t __stdcall get_MinWidth(double*) noexcept = 0;
    virtual int32_t __stdcall put_MinWidth(double) noexcept = 0;
    virtual int32_t __stdcall get_MaxWidth(double*) noexcept = 0;
    virtual int32_t __stdcall put_MaxWidth(double) noexcept = 0;
    virtual int32_t __stdcall get_MinHeight(double*) noexcept = 0;
    virtual int32_t __stdcall put_MinHeight(double) noexcept = 0;
    virtual int32_t __stdcall get_MaxHeight(double*) noexcept = 0;
    virtual int32_t __stdcall put_MaxHeight(double) noexcept = 0;
    virtual int32_t __stdcall get_HorizontalAlignment(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_HorizontalAlignment(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_VerticalAlignment(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_VerticalAlignment(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_Margin(struct struct_Microsoft_UI_Xaml_Thickness*) noexcept = 0;
    virtual int32_t __stdcall put_Margin(struct struct_Microsoft_UI_Xaml_Thickness) noexcept = 0;
    virtual int32_t __stdcall get_Name(void**) noexcept = 0;
    virtual int32_t __stdcall put_Name(void*) noexcept = 0;
    virtual int32_t __stdcall get_BaseUri(void**) noexcept = 0;
    virtual int32_t __stdcall get_DataContext(void**) noexcept = 0;
    virtual int32_t __stdcall put_DataContext(void*) noexcept = 0;
    virtual int32_t __stdcall get_AllowFocusOnInteraction(bool*) noexcept = 0;
    virtual int32_t __stdcall put_AllowFocusOnInteraction(bool) noexcept = 0;
    virtual int32_t __stdcall get_FocusVisualMargin(struct struct_Microsoft_UI_Xaml_Thickness*) noexcept = 0;
    virtual int32_t __stdcall put_FocusVisualMargin(struct struct_Microsoft_UI_Xaml_Thickness) noexcept = 0;
    virtual int32_t __stdcall get_FocusVisualSecondaryThickness(struct struct_Microsoft_UI_Xaml_Thickness*) noexcept = 0;
    virtual int32_t __stdcall put_FocusVisualSecondaryThickness(struct struct_Microsoft_UI_Xaml_Thickness) noexcept = 0;
    virtual int32_t __stdcall get_FocusVisualPrimaryThickness(struct struct_Microsoft_UI_Xaml_Thickness*) noexcept = 0;
    virtual int32_t __stdcall put_FocusVisualPrimaryThickness(struct struct_Microsoft_UI_Xaml_Thickness) noexcept = 0;
    virtual int32_t __stdcall get_FocusVisualSecondaryBrush(void**) noexcept = 0;
    virtual int32_t __stdcall put_FocusVisualSecondaryBrush(void*) noexcept = 0;
    virtual int32_t __stdcall get_FocusVisualPrimaryBrush(void**) noexcept = 0;
    virtual int32_t __stdcall put_FocusVisualPrimaryBrush(void*) noexcept = 0;
    virtual int32_t __stdcall get_AllowFocusWhenDisabled(bool*) noexcept = 0;
    virtual int32_t __stdcall put_AllowFocusWhenDisabled(bool) noexcept = 0;
    virtual int32_t __stdcall get_Style(void**) noexcept = 0;
    virtual int32_t __stdcall put_Style(void*) noexcept = 0;
    virtual int32_t __stdcall get_Parent(void**) noexcept = 0;
    virtual int32_t __stdcall get_FlowDirection(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_FlowDirection(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_RequestedTheme(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_RequestedTheme(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_IsLoaded(bool*) noexcept = 0;
    virtual int32_t __stdcall get_ActualTheme(int32_t*) noexcept = 0;
    virtual int32_t __stdcall add_Loaded(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_Loaded(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_Unloaded(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_Unloaded(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_DataContextChanged(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_DataContextChanged(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_SizeChanged(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_SizeChanged(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_LayoutUpdated(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_LayoutUpdated(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_Loading(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_Loading(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_ActualThemeChanged(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_ActualThemeChanged(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_EffectiveViewportChanged(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_EffectiveViewportChanged(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall FindName(void*, void**) noexcept = 0;
    virtual int32_t __stdcall SetBinding(void*, void*) noexcept = 0;
    virtual int32_t __stdcall GetBindingExpression(void*, void**) noexcept = 0;
};

struct Controls_IGrid : ::IInspectable {
    virtual int32_t __stdcall get_RowDefinitions(void**) noexcept = 0;
    virtual int32_t __stdcall get_ColumnDefinitions(void**) noexcept = 0;
    virtual int32_t __stdcall get_BackgroundSizing(int32_t*) noexcept = 0;
    virtual int32_t __stdcall put_BackgroundSizing(int32_t) noexcept = 0;
    virtual int32_t __stdcall get_BorderBrush(void**) noexcept = 0;
    virtual int32_t __stdcall put_BorderBrush(void*) noexcept = 0;
    virtual int32_t __stdcall get_BorderThickness(struct struct_Microsoft_UI_Xaml_Thickness*) noexcept = 0;
    virtual int32_t __stdcall put_BorderThickness(struct struct_Microsoft_UI_Xaml_Thickness) noexcept = 0;
    virtual int32_t __stdcall get_CornerRadius(struct struct_Microsoft_UI_Xaml_CornerRadius*) noexcept = 0;
    virtual int32_t __stdcall put_CornerRadius(struct struct_Microsoft_UI_Xaml_CornerRadius) noexcept = 0;
    virtual int32_t __stdcall get_Padding(struct struct_Microsoft_UI_Xaml_Thickness*) noexcept = 0;
    virtual int32_t __stdcall put_Padding(struct struct_Microsoft_UI_Xaml_Thickness) noexcept = 0;
    virtual int32_t __stdcall get_RowSpacing(double*) noexcept = 0;
    virtual int32_t __stdcall put_RowSpacing(double) noexcept = 0;
    virtual int32_t __stdcall get_ColumnSpacing(double*) noexcept = 0;
    virtual int32_t __stdcall put_ColumnSpacing(double) noexcept = 0;
};

struct IRowDefinition : ::IInspectable {
    virtual int32_t __stdcall get_Height(struct struct_Microsoft_UI_Xaml_GridLength*) noexcept = 0;
    virtual int32_t __stdcall put_Height(struct struct_Microsoft_UI_Xaml_GridLength) noexcept = 0;
    virtual int32_t __stdcall get_MaxHeight(double*) noexcept = 0;
    virtual int32_t __stdcall put_MaxHeight(double) noexcept = 0;
    virtual int32_t __stdcall get_MinHeight(double*) noexcept = 0;
    virtual int32_t __stdcall put_MinHeight(double) noexcept = 0;
    virtual int32_t __stdcall get_ActualHeight(double*) noexcept = 0;
};
// clang-format on

enum Visibility : int32_t {
    Visible = 0,
    Collapsed = 1,
};

}  // namespace WASDK
#pragma endregion  // WASDK

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

bool IsVersionAtLeast(HMODULE hModule,
                      WORD major,
                      WORD minor,
                      WORD build,
                      WORD qfe) {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(hModule, nullptr);
    if (!fixedFileInfo) {
        return false;
    }

    WORD moduleMajor = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleMinor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleBuild = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD moduleQfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    if (moduleMajor != major) {
        return moduleMajor > major;
    }

    if (moduleMinor != minor) {
        return moduleMinor > minor;
    }

    if (moduleBuild != build) {
        return moduleBuild > build;
    }

    return moduleQfe >= qfe;
}

bool IsExplorerVersionAtLeast(WORD major, WORD minor, WORD build, WORD qfe) {
    return IsVersionAtLeast(nullptr, major, minor, build, qfe);
}

bool IsFileExplorerExtensionsVersionAtLeast(WORD major,
                                            WORD minor,
                                            WORD build,
                                            WORD qfe) {
    HMODULE hModule = GetModuleHandle(L"FileExplorerExtensions.dll");
    if (!hModule) {
        return false;
    }

    return IsVersionAtLeast(hModule, major, minor, build, qfe);
}

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };

#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)

    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
        HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();

    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);

        switch (feature.enabledState) {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }

    return std::nullopt;
}

bool HandleNavigationBarControl(IUnknown* navigationBarControl) {
    winrt::com_ptr<WASDK::IUIElement> uiElement;
    (*(IUnknown**)((BYTE*)navigationBarControl + 0x08))
        ->QueryInterface(WASDK::IID_IUIElement, uiElement.put_void());
    if (!uiElement) {
        return false;
    }

    uiElement->put_Visibility(WASDK::Visibility::Collapsed);

    winrt::com_ptr<WASDK::IFrameworkElement> frameworkElement;
    uiElement->QueryInterface(WASDK::IID_IFrameworkElement,
                              frameworkElement.put_void());
    if (!frameworkElement) {
        return false;
    }

    winrt::com_ptr<::IInspectable> parent;
    frameworkElement->get_Parent(parent.put_void());
    if (!parent) {
        return false;
    }

    winrt::com_ptr<WASDK::Controls_IGrid> grid;
    parent->QueryInterface(WASDK::IID_Controls_IGrid, grid.put_void());
    if (!grid) {
        return false;
    }

    winrt::com_ptr<winrt::impl::abi<winrt::Windows::Foundation::Collections::
                                        IVector<WASDK::IRowDefinition*>>::type>
        rowDefinitions;
    grid->get_RowDefinitions(rowDefinitions.put_void());
    if (!rowDefinitions) {
        return false;
    }

    winrt::com_ptr<WASDK::IRowDefinition> rowDefinition;
    rowDefinitions->GetAt(1, rowDefinition.put());
    if (!rowDefinition) {
        return false;
    }

    rowDefinition->put_MaxHeight(0);
    return true;
}

using XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_t =
    HRESULT(WINAPI*)(PVOID pThis, SIZE* size);
XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_t
    XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original;
HRESULT WINAPI
XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Hook(PVOID pThis,
                                                           SIZE* size) {
    Wh_Log(L">");

    HRESULT ret =
        XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original(pThis,
                                                                       size);

    if (SUCCEEDED(ret) &&
        g_settings.explorerStyle == ExplorerStyle::classicNavigationBar) {
        int originalCy = size->cy;
        size->cy = MulDiv(size->cy, 90, 136);
        Wh_Log(L"%d -> %d", originalCy, size->cy);
    }

    return ret;
}

using Feature_NavigationBarControl_OnApplyTemplate_t =
    HRESULT(WINAPI*)(PVOID pThis);
Feature_NavigationBarControl_OnApplyTemplate_t
    Feature_NavigationBarControl_OnApplyTemplate_Original;
HRESULT WINAPI Feature_NavigationBarControl_OnApplyTemplate_Hook(PVOID pThis) {
    Wh_Log(L">");

    if (g_settings.explorerStyle == ExplorerStyle::classicNavigationBar) {
        static size_t offset =
            IsFileExplorerExtensionsVersionAtLeast(2125, 4200, 50, 0) ? 0x40
                                                                      : 0x08;

        HandleNavigationBarControl(*(IUnknown**)((BYTE*)pThis + offset));
    }

    return Feature_NavigationBarControl_OnApplyTemplate_Original(pThis);
}

using CommandBarExtension_GetHeight_t =
    int(WINAPI*)(PVOID pThis,
                 const winrt::Windows::Foundation::Size* sizeIn,
                 winrt::Windows::Foundation::Size* sizeOut);
CommandBarExtension_GetHeight_t CommandBarExtension_GetHeight_Original;
int WINAPI CommandBarExtension_GetHeight_Hook(
    PVOID pThis,
    const winrt::Windows::Foundation::Size* sizeIn,
    winrt::Windows::Foundation::Size* sizeOut) {
    Wh_Log(L">");

    int ret = CommandBarExtension_GetHeight_Original(pThis, sizeIn, sizeOut);

    if (g_settings.explorerStyle == ExplorerStyle::classicNavigationBar) {
        static const float newHeight =
            IsExplorerVersionAtLeast(10, 0, 22621, 3958) ? 0 : 6;
        sizeOut->Height = newHeight;
    }

    return ret;
}

using CanShowModernNavBar_t = bool(WINAPI*)(PVOID pThis);
CanShowModernNavBar_t CanShowModernNavBar_Original;
bool WINAPI CanShowModernNavBar_Hook(PVOID pThis) {
    Wh_Log(L">");

    if (g_settings.explorerStyle == ExplorerStyle::classicNavigationBar) {
        return false;
    }

    return CanShowModernNavBar_Original(pThis);
}

using CachedExplorerExtensionState_IsModernNavBarAvailable_t =
    bool(WINAPI*)(PVOID pThis);
CachedExplorerExtensionState_IsModernNavBarAvailable_t
    CachedExplorerExtensionState_IsModernNavBarAvailable_Original;
bool WINAPI
CachedExplorerExtensionState_IsModernNavBarAvailable_Hook(PVOID pThis) {
    Wh_Log(L">");

    if (g_settings.explorerStyle == ExplorerStyle::classicNavigationBar) {
        return false;
    }

    return CachedExplorerExtensionState_IsModernNavBarAvailable_Original(pThis);
}

using CoCreateInstance_t = decltype(&CoCreateInstance);
CoCreateInstance_t CoCreateInstance_Original;
HRESULT WINAPI CoCreateInstance_Hook(REFCLSID rclsid,
                                     LPUNKNOWN pUnkOuter,
                                     DWORD dwClsContext,
                                     REFIID riid,
                                     LPVOID* ppv) {
    constexpr winrt::guid CLSID_XamlIslandViewAdapter{
        0x6480100B,
        0x5A83,
        0x4D1E,
        {0x9F, 0x69, 0x8A, 0xE5, 0xA8, 0x8E, 0x9A, 0x33}};

    if (IsEqualCLSID(rclsid, CLSID_XamlIslandViewAdapter) &&
        g_settings.explorerStyle == ExplorerStyle::classicRibbonUI) {
        Wh_Log(L">");

        return REGDB_E_CLASSNOTREG;
    }

    return CoCreateInstance_Original(rclsid, pUnkOuter, dwClsContext, riid,
                                     ppv);
}

bool HookExplorerFrameSymbols() {
    HMODULE module = LoadLibraryEx(L"explorerframe.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load explorerframe.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
        {
            {LR"(bool __cdecl CanShowModernNavBar(void))"},
            &CanShowModernNavBar_Original,
            CanShowModernNavBar_Hook,
            true,  // Before Win11 24H2.
        },
        {
            {LR"(public: static bool __cdecl CachedExplorerExtensionState::IsModernNavBarAvailable(void))"},
            &CachedExplorerExtensionState_IsModernNavBarAvailable_Original,
            CachedExplorerExtensionState_IsModernNavBarAvailable_Hook,
            true,  // Since Win11 24H2.
        },
    };

    if (!HookSymbols(module, explorerFrameDllHooks,
                     ARRAYSIZE(explorerFrameDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookWindowsUIFileExplorerSymbols() {
    if (IsExplorerVersionAtLeast(10, 0, 26100, 0)) {
        if (!IsExplorerVersionAtLeast(10, 0, 26100, 1591)) {
            return true;
        }
    } else if (IsExplorerVersionAtLeast(10, 0, 22621, 0)) {
        if (!IsExplorerVersionAtLeast(10, 0, 22621, 4111)) {
            return true;
        }
    } else {
        return true;
    }

    constexpr UINT kFileExplorerTabLineFix = 51960011;
    if (!IsOsFeatureEnabled(kFileExplorerTabLineFix).value_or(true)) {
        return true;
    }

    HMODULE module = LoadLibraryEx(L"Windows.UI.FileExplorer.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load Windows.UI.FileExplorer.dll");
        return false;
    }

    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlIslandViewAdapter::get_DesiredSizeInPhysicalPixels(struct tagSIZE *))"},
            &XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original,
            XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Hook,
            true,
        },
    };

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookFileExplorerExtensionsSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::NavigationBarControl::OnApplyTemplate(void))"},
            &Feature_NavigationBarControl_OnApplyTemplate_Original,
            Feature_NavigationBarControl_OnApplyTemplate_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::FileExplorerExtensions::factory_implementation::CommandBarExtension,struct winrt::WindowsUdk::UI::Shell::IFileExplorerCommandBarExtensionStatics>::GetHeight(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &CommandBarExtension_GetHeight_Original,
            CommandBarExtension_GetHeight_Hook,
        },
    };

    if (!HookSymbols(module, fileExplorerExtensionsDllHooks,
                     ARRAYSIZE(fileExplorerExtensionsDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetFileExplorerExtensionsModuleHandle() {
    return GetModuleHandle(L"FileExplorerExtensions.dll");
}

void HandleLoadedModuleIfFileExplorerExtensions(HMODULE module,
                                                LPCWSTR lpLibFileName) {
    if (!g_fileExplorerExtensionsLoaded &&
        GetFileExplorerExtensionsModuleHandle() == module &&
        !g_fileExplorerExtensionsLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookFileExplorerExtensionsSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfFileExplorerExtensions(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    PCWSTR explorerStyle = Wh_GetStringSetting(L"explorerStyle");
    g_settings.explorerStyle = ExplorerStyle::classicNavigationBar;
    if (wcscmp(explorerStyle, L"classicRibbonUI") == 0) {
        g_settings.explorerStyle = ExplorerStyle::classicRibbonUI;
    }
    Wh_FreeStringSetting(explorerStyle);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookExplorerFrameSymbols()) {
        return FALSE;
    }

    if (!HookWindowsUIFileExplorerSymbols()) {
        return FALSE;
    }

    if (HMODULE fileExplorerExtensionsModule =
            GetFileExplorerExtensionsModuleHandle()) {
        g_fileExplorerExtensionsLoaded = true;
        if (!HookFileExplorerExtensionsSymbols(fileExplorerExtensionsModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"File explorer extensions module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    WindhawkUtils::Wh_SetFunctionHookT(CoCreateInstance, CoCreateInstance_Hook,
                                       &CoCreateInstance_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_fileExplorerExtensionsLoaded) {
        if (HMODULE fileExplorerExtensionsModule =
                GetFileExplorerExtensionsModuleHandle()) {
            if (!g_fileExplorerExtensionsLoaded.exchange(true)) {
                Wh_Log(L"Got FileExplorerExtensions.dll");

                if (HookFileExplorerExtensionsSymbols(
                        fileExplorerExtensionsModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
