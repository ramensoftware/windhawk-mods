// ==WindhawkMod==
// @id              wobbly-windows
// @name            Wobbly Windows
// @description     The classic Compiz/KDE Plasma style Wobbly Windows effect for Windows 11!
// @version         0.70
// @author          lalimatyus
// @github          https://github.com/lalimatyus
// @include         dwm.exe
// @architecture    x86-64
// @license         GPL-2.0-or-later
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Wobbly Windows

The classic Compiz/KDE Plasma style Wobbly Windows effect for Windows 11!

![Showcase](https://raw.githubusercontent.com/lalimatyus/Wobbly-Windows/refs/heads/main/showcase.gif)

## ⚠️ IMPORTANT ⚠️

Since this mod runs in `dwm.exe`, add `dwm.exe` to Windhawk's
**Settings > Advanced settings > More advanced settings > Process inclusion list**:

![Tutorial](https://raw.githubusercontent.com/lalimatyus/Wobbly-Windows/refs/heads/main/dwm.gif)

This mod is currently in **beta**, so things can occasionally break and it might not work on every Windows 11 build.
It was mostly tested and made on `Windows 11 23H2`, where it works great, and it was also tested on `25H2` and on `Insider Preview 26H2`.

## Features

* Change the wobbliness of the windows from 5 presets
* Enable advanced mode to change each parameter independently, instead of a preset
* Fluid wobble animations for dragging, snapping and even resizing windows
* Uses a 4x4 spring simulation fitted to a smooth whole-window transform

## Known Issues

* ARM64 isn't supported yet; the mod safely refuses to initialize on ARM64 systems.

## Feedback

If you found a reproducible issue or need help, open an issue in the [GitHub repository](https://github.com/lalimatyus/Wobbly-Windows).

## Credits

The physics presets and edge-locking behavior are based on KDE Plasma/KWin's Wobbly Windows effect.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WobblinessPreset: "2"
  $name: Wobbliness
  $description: Select one of the five original KDE Plasma Wobbly Windows presets.
  $options:
  - "0": "Less (Stiffness 15, Drag 80, Move Factor 10)"
  - "1": "Low (Stiffness 10, Drag 85, Move Factor 10)"
  - "2": "Medium (Stiffness 6, Drag 90, Move Factor 10)"
  - "3": "High (Stiffness 3, Drag 92, Move Factor 20)"
  - "4": "More (Stiffness 1, Drag 97, Move Factor 25)"

- EnableResizeWobble: true
  $name: Resize wobble
  $description: Animate windows while resizing them from an edge or corner.

- EnableWindowStateWobble: true
  $name: Snap and maximize wobble
  $description: Animate Snap, maximize and restore transitions.

- AdvancedMode:
  - enable: false
    $name: Advanced Mode
    $description: Use the three physics values instead of the selected preset.

  - Stiffness: 6
    $name: Stiffness
    $description: Custom spring stiffness. 1-100

  - Drag: 90
    $name: Drag
    $description: Custom movement damping. 1-100

  - MoveFactor: 10
    $name: Move Factor
    $description: Custom deformation amount. 1-25

  $name: Custom Physics

*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <algorithm>
#include <atomic>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <regex>
#include <string>
#include <string_view>

#include <windhawk_utils.h>

#if defined(_M_ARM64)

BOOL Wh_ModInit()
{
    Wh_Log(L"Wobbly Windows: ARM64 isn't supported");
    return FALSE;
}

#else

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

struct WobblySettings
{
    int wobbliness;
    bool advancedMode;
    bool resizeWobbleEnabled;
    bool windowStateWobbleEnabled;
    double stiffness;
    double drag;
    double moveFactor;
};

WobblySettings g_settings = {};
SRWLOCK g_settingsLock = SRWLOCK_INIT;

struct Vec2
{
    double x;
    double y;
};

struct WobblePoint
{
    Vec2 basePosition;
    Vec2 position;
    Vec2 velocity;
    Vec2 force;
    bool fixed;
};

static constexpr int GRID_WIDTH = 4;
static constexpr int GRID_HEIGHT = 4;
static constexpr int GRID_POINT_COUNT = GRID_WIDTH * GRID_HEIGHT;

struct WobbleMesh
{
    WobblePoint points[GRID_POINT_COUNT];
    double width;
    double height;
    bool active;
    bool dragging;
    bool resizing;
    bool canWobbleTop;
    bool canWobbleLeft;
    bool canWobbleRight;
    bool canWobbleBottom;
    int dragPointIndex;
    Vec2 dragOffset;
    Vec2 lastDragPosition;
};

struct MeshStepResult
{
    bool wobblying;
};

using IsGhostWindow_t = bool(__cdecl*)(void* pThis, HWND** ghostWindow);
IsGhostWindow_t g_isGhostWindowOriginal = nullptr;
size_t g_windowDataHwndOffset = SIZE_MAX;
size_t g_windowDataTopLevelWindowOffset = SIZE_MAX;
size_t g_windowDataTopLevelWindow3DOffset = SIZE_MAX;
size_t g_topLevelWindow3DWindowDataOffset = SIZE_MAX;
static bool IsReadableMemory(const void* address, size_t size);

static HWND GetHwndFromWindowData(void* windowData)
{
    if (!windowData)
    {
        return nullptr;
    }
    if (g_windowDataHwndOffset == SIZE_MAX)
    {
        return nullptr;
    }
    BYTE* hwndField = static_cast<BYTE*>(windowData) + g_windowDataHwndOffset;
    if (!IsReadableMemory(hwndField, sizeof(HWND)))
    {
        return nullptr;
    }
    HWND hwnd = *reinterpret_cast<HWND*>(hwndField);
    if (!IsWindow(hwnd))
    {
        return nullptr;
    }
    return hwnd;
}

static HWND GetHwndFromTrustedWindowData(void* windowData)
{
    if (!windowData || g_windowDataHwndOffset == SIZE_MAX)
    {
        return nullptr;
    }
    // Hook arguments are live CWindowData objects for the duration of the call.
    return *reinterpret_cast<HWND*>(static_cast<BYTE*>(windowData) +
                                    g_windowDataHwndOffset);
}

HWINEVENTHOOK g_moveSizeHook = nullptr;
HWINEVENTHOOK g_locationHook = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;
HWINEVENTHOOK g_minimizeHook = nullptr;
HWINEVENTHOOK g_destroyHook = nullptr;
HWINEVENTHOOK g_desktopSwitchHook = nullptr;
HWINEVENTHOOK g_cloakHook = nullptr;
static constexpr int MAX_OBSERVED_WINDOWS = 256;

struct ObservedWindowState
{
    HWND hwnd;
    bool zoomed;
    bool iconic;
    bool snapped;
    bool nativeTransitionPending;
    bool expectedZoomed;
    RECT rect;
    ULONGLONG lastSeen;
    ULONGLONG nativeTransitionDeadline;
    ULONGLONG nativeTransitionStartedAt;
    ULONGLONG suppressStateThrobUntil;
    ULONGLONG snapTransitionDeadline;
};

ObservedWindowState g_observedWindows[MAX_OBSERVED_WINDOWS] = {};
HANDLE g_eventThread = nullptr;
DWORD g_eventThreadId = 0;
HANDLE g_eventThreadReady = nullptr;
HANDLE g_eventThreadStop = nullptr;
std::atomic<DWORD> g_eventThreadMessageTarget = 0;
std::atomic<HWND> g_pendingMaximizedStateWindow = nullptr;
std::atomic_bool g_maximizedStateCheckQueued = false;
static constexpr UINT WM_WOBBLY_MAXIMIZED_CHANGE = WM_APP + 0x31A;
static constexpr UINT WM_WOBBLY_NATIVE_WINDOW_TRANSITION = WM_APP + 0x31B;
static constexpr LPARAM NATIVE_TRANSITION_TARGET_IS_WORK_AREA = 0x01;
static constexpr LPARAM NATIVE_TRANSITION_SOURCE_IS_WORK_AREA = 0x02;
static constexpr LPARAM NATIVE_TRANSITION_WINDOW_IS_ZOOMED = 0x04;
static constexpr LPARAM NATIVE_TRANSITION_TARGET_IS_SNAP_LAYOUT = 0x08;
static constexpr LPARAM NATIVE_TRANSITION_DIRECTION_LEFT = 0x10;
static constexpr LPARAM NATIVE_TRANSITION_DIRECTION_RIGHT = 0x20;
static constexpr LPARAM NATIVE_TRANSITION_DIRECTION_UP = 0x40;
static constexpr LPARAM NATIVE_TRANSITION_DIRECTION_DOWN = 0x80;
std::atomic<HWND> g_pendingInteractiveSnapWindow = nullptr;
std::atomic<LPARAM> g_pendingInteractiveSnapFlags = 0;
std::atomic<ULONGLONG> g_windowStateThrobSuppressedUntil = 0;
RECT g_realDraggedWindowRect = {};
RECT g_lastDraggedWindowRect = {};
bool g_lastDraggedWindowZoomed = false;
bool g_finalizingMoveSize = false;
std::atomic_bool g_realDragging = false;
std::atomic<HWND> g_realDraggedWindow = nullptr;
using OnPositionChange_t = void(__cdecl*)(void* pThis, void* pWindowData, bool unknown);
OnPositionChange_t g_onPositionChangeOriginal = nullptr;
using FindWindowDataByHwnd_t = void*(__cdecl*)(void* pThis, HWND hwnd);
FindWindowDataByHwnd_t g_findWindowDataByHwnd = nullptr;
using GetSyncedWindowDataLong_t = long(__cdecl*)(void* pThis, void* dwmWindow, bool synchronize,
                                                 void** windowData);
using GetSyncedWindowDataVoid_t = void(__cdecl*)(void* pThis, void* dwmWindow, bool synchronize,
                                                 void** windowData);
GetSyncedWindowDataLong_t g_getSyncedWindowDataLong = nullptr;
GetSyncedWindowDataVoid_t g_getSyncedWindowDataVoid = nullptr;
using CheckForMaximizedChange_t = void(__cdecl*)(void* pThis, void* pWindowData);
CheckForMaximizedChange_t g_checkForMaximizedChangeOriginal = nullptr;
using StartAnimationForMaximizeSnapTransition_t = long(__cdecl*)(void* pThis, int animationType,
                                                                 const RECT& targetRect);
StartAnimationForMaximizeSnapTransition_t g_startAnimationForMaximizeSnapTransitionOriginal =
    nullptr;
using WindowTransitionChange_t = long(__cdecl*)(void* pThis, void* dwmWindow, int transitionTarget,
                                                const RECT& targetRect, const RECT& rect2,
                                                const RECT& rect3, const RECT& rect4,
                                                const RECT& rect5);
WindowTransitionChange_t g_windowTransitionChangeOriginal = nullptr;
POINT g_lastMousePosition = {};
bool g_hasLastMousePosition = false;
POINT g_dragStartMousePosition = {};
Vec2 g_dragStartLocalMouse = {0.0, 0.0};
HMONITOR g_dragCursorMonitor = nullptr;
ULONGLONG g_monitorTransitionRebaseUntil = 0;
using CTopLevelWindowGetVisualProxy_t = void*(__cdecl*)(void* pThis);
CTopLevelWindowGetVisualProxy_t g_getCanvasRootVisualProxy = nullptr;
using CTopLevelWindowGetWindowData_t = void*(__cdecl*)(void* pThis);
CTopLevelWindowGetWindowData_t g_topLevelWindowGetWindowData = nullptr;
using CTopLevelWindowGetRootVisual_t = void*(__cdecl*)(void* pThis, int rootVisualType);
CTopLevelWindowGetRootVisual_t g_topLevelWindowGetRootVisual = nullptr;
bool g_realResizing = false;
bool g_moveTypeKnown = false;
bool g_dragResizeWobbleEnabled = true;
bool g_dragStartedWindowZoomed = false;
bool g_interactiveWindowStateThrob = false;
bool g_interactiveWindowStateMaximizing = false;
double g_resizeCoordinateScale = 1.0;
using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR monitor, int dpiType, UINT* dpiX, UINT* dpiY);
HMODULE g_shcoreModule = nullptr;
GetDpiForMonitor_t g_getDpiForMonitor = nullptr;
using DwmGetWindowAttribute_t = HRESULT(WINAPI*)(HWND hwnd, DWORD attribute, void* value,
                                                 DWORD valueSize);
HMODULE g_dwmApiModule = nullptr;
DwmGetWindowAttribute_t g_dwmGetWindowAttribute = nullptr;
HWND g_lastForegroundWindow = nullptr;
UINT_PTR g_virtualDesktopRefreshTimer = 0;
ULONGLONG g_virtualDesktopRefreshDeadline = 0;
struct MilMatrix3x2D;
struct D2DMatrix3x2F;
using CMatrixTransformProxyUpdateDouble_t = long(__cdecl*)(void* pThis,
                                                           const MilMatrix3x2D& matrix);
using CMatrixTransformProxyUpdateFloat_t = long(__cdecl*)(void* pThis,
                                                          const D2DMatrix3x2F& matrix);
CMatrixTransformProxyUpdateDouble_t g_cMatrixTransformProxyUpdate = nullptr;
CMatrixTransformProxyUpdateFloat_t g_cMatrixTransformProxyUpdateFloat = nullptr;
using CVisualProxySetTransform_t = long(__cdecl*)(void* pThis, void* transform);
CVisualProxySetTransform_t g_cVisualProxySetTransform = nullptr;
using CCompositorCreateMatrixTransformProxy_t = long(__cdecl*)(void* pThis, void** transformProxy);
CCompositorCreateMatrixTransformProxy_t g_createMatrixTransformProxy = nullptr;
using CBaseObjectRelease_t = unsigned long(__cdecl*)(void* pThis);
CBaseObjectRelease_t g_cBaseObjectRelease = nullptr;
using CTopLevelWindowConstructor_t = void*(__cdecl*)(void* pThis, void* windowData, bool unknown);
CTopLevelWindowConstructor_t g_topLevelWindowConstructorFunction = nullptr;
CTopLevelWindowConstructor_t g_topLevelWindowConstructorOriginal = nullptr;
using CTopLevelWindow3DSetWindowData_t = void(__cdecl*)(void* pThis, void* windowData);
CTopLevelWindow3DSetWindowData_t g_topLevelWindow3DSetWindowDataOriginal = nullptr;
using CWindowDataDestructor_t = void(__cdecl*)(void* pThis);
CWindowDataDestructor_t g_windowDataDestructorOriginal = nullptr;
using CWindowListForceUpdateScene_t = long(__cdecl*)(void* pThis);
CWindowListForceUpdateScene_t g_windowListForceUpdateSceneOriginal = nullptr;
using CWindowListUpdateScene_t = long(__cdecl*)(void* pThis);
CWindowListUpdateScene_t g_windowListUpdateSceneOriginal = nullptr;
using CDesktopManagerAdvanceTimelines_t = void(__cdecl*)(void* pThis, double currentTime);
CDesktopManagerAdvanceTimelines_t g_desktopManagerAdvanceTimelinesOriginal = nullptr;
using CDesktopManagerHandleThreadMessage_t =
    void(__cdecl*)(UINT message, UINT_PTR wParam, INT_PTR lParam);
CDesktopManagerHandleThreadMessage_t g_desktopManagerHandleThreadMessageOriginal = nullptr;
using CDesktopManagerPostStartAnimations_t = long(__cdecl*)(void* pThis);
CDesktopManagerPostStartAnimations_t g_desktopManagerPostStartAnimations = nullptr;
void* g_desktopManagerInstanceAddress = nullptr;
void* g_desktopManagerInitializeFunction = nullptr;
void* g_cCompositorCreateFunction = nullptr;
using CTopLevelWindow3DDestructor_t = void(__cdecl*)(void* pThis);
CTopLevelWindow3DDestructor_t g_topLevelWindow3DDestructorOriginal = nullptr;
using CTopLevelWindowDestructor_t = void(__cdecl*)(void* pThis);
CTopLevelWindowDestructor_t g_topLevelWindowDestructorOriginal = nullptr;
using EnsureTopLevelWindow_t = long(__cdecl*)(void* pThis, void* windowData);
EnsureTopLevelWindow_t g_ensureTopLevelWindowFunction = nullptr;
std::atomic<void*> g_desktopManagerVtable = nullptr;
std::atomic<void*> g_windowListVtable = nullptr;
std::atomic<void*> g_compositorVtable = nullptr;
std::atomic<void*> g_topLevelWindowVtable = nullptr;
std::atomic<void*> g_topLevelWindow3DVtable = nullptr;
std::atomic<void*> g_visualProxyVtable = nullptr;
std::atomic<void*> g_matrixTransformProxyVtable = nullptr;
std::atomic<void*> g_dwmCompositor = nullptr;
std::atomic<void*> g_desktopManager = nullptr;
void* g_desktopManagerVtableSymbol = nullptr;
void* g_windowListVtableSymbol = nullptr;
void* g_topLevelWindowVtableSymbol = nullptr;
void* g_topLevelWindow3DVtableSymbol = nullptr;
void* g_visualProxyVtableSymbol = nullptr;
void* g_matrixTransformProxyVtableSymbol = nullptr;
size_t g_desktopManagerCompositorOffset = SIZE_MAX;
size_t g_desktopManagerThreadIdOffset = SIZE_MAX;
size_t g_visualProxyOffset = SIZE_MAX;
size_t g_topLevelWindowWindowDataOffset = SIZE_MAX;
static bool IsDwmObjectPointerValid(void* object, std::atomic<void*>& expectedVtable);
static bool HasExactDwmVtableTrusted(void* object,
                                    std::atomic<void*>& expectedVtable);
static bool LearnDwmVtableFromTrustedObject(void* object,
                                            std::atomic<void*>& targetVtable);
static void RegisterDwmWindowMapping(void* windowData, void* topLevelWindow,
                                     void* topLevelWindow3D);

struct DwmAddressRange
{
    const BYTE* begin;
    const BYTE* end;
};

struct DwmModuleLayout
{
    HMODULE module;
    const BYTE* imageBegin;
    const BYTE* imageEnd;
    DWORD timeDateStamp;
    DWORD sizeOfImage;
    DwmAddressRange executableRanges[16];
    unsigned int executableRangeCount;
};

DwmModuleLayout g_dwmModuleLayout = {};
static constexpr int MAX_DWM_WINDOW_MAPPINGS = 256;
static constexpr unsigned int MAX_EXISTING_WINDOW_BACKFILL = 256;

struct DwmWindowObjectMapping
{
    void* windowData;
    void* topLevelWindow;
    void* topLevelWindow3D;
    ULONGLONG lastSeen;
};

DwmWindowObjectMapping g_dwmWindowMappings[MAX_DWM_WINDOW_MAPPINGS] = {};
SRWLOCK g_dwmWindowMappingsLock = SRWLOCK_INIT;
HWND g_existingWindowBackfill[MAX_EXISTING_WINDOW_BACKFILL] = {};
SRWLOCK g_existingWindowBackfillLock = SRWLOCK_INIT;
std::atomic<unsigned int> g_existingWindowBackfillCount = 0;
std::atomic<unsigned int> g_existingWindowBackfillIndex = 0;
std::atomic<unsigned int> g_existingWindowBackfillMapped = 0;
std::atomic<DWORD> g_dwmSceneThreadId = 0;
static constexpr UINT WM_DWM_SCENE_WAKE = WM_USER + 1;
static constexpr UINT_PTR DWM_SCENE_WAKE_WPARAM = 0x574F42424C59574BULL;
std::atomic<INT_PTR> g_dwmSceneWakeToken = 0;
std::atomic<unsigned int> g_sceneWakeOutstanding = 0;
std::atomic_bool g_proxyCreationDisabled = false;
std::atomic_bool g_dwmThreadMismatchLogged = false;
std::atomic<unsigned int> g_dwmObjectDiscoveryFailureCount = 0;

struct MilMatrix3x2D
{
    double m11;
    double m12;
    double m21;
    double m22;
    double dx;
    double dy;
};

struct D2DMatrix3x2F
{
    float m11;
    float m12;
    float m21;
    float m22;
    float dx;
    float dy;
};

static void* FindWindowDataByHwnd(void* windowList, HWND hwnd)
{
    return windowList && hwnd && g_findWindowDataByHwnd
               ? g_findWindowDataByHwnd(windowList, hwnd)
               : nullptr;
}

static bool HasSyncedWindowData()
{
    return g_getSyncedWindowDataLong || g_getSyncedWindowDataVoid;
}

static bool GetSyncedWindowDataCompat(void* windowList, void* dwmWindow, bool synchronize,
                                      void** windowData)
{
    *windowData = nullptr;
    if (g_getSyncedWindowDataVoid)
    {
        g_getSyncedWindowDataVoid(windowList, dwmWindow, synchronize, windowData);
        return *windowData != nullptr;
    }
    return g_getSyncedWindowDataLong &&
           g_getSyncedWindowDataLong(windowList, dwmWindow, synchronize, windowData) >= 0 &&
           *windowData;
}

static bool HasMatrixTransformUpdate()
{
    return g_cMatrixTransformProxyUpdate || g_cMatrixTransformProxyUpdateFloat;
}

static long UpdateMatrixTransformProxy(void* proxy, const MilMatrix3x2D& matrix)
{
    if (g_cMatrixTransformProxyUpdate)
    {
        return g_cMatrixTransformProxyUpdate(proxy, matrix);
    }
    if (g_cMatrixTransformProxyUpdateFloat)
    {
        D2DMatrix3x2F floatMatrix = {static_cast<float>(matrix.m11),
                                     static_cast<float>(matrix.m12),
                                     static_cast<float>(matrix.m21),
                                     static_cast<float>(matrix.m22),
                                     static_cast<float>(matrix.dx),
                                     static_cast<float>(matrix.dy)};
        return g_cMatrixTransformProxyUpdateFloat(proxy, floatMatrix);
    }
    return E_NOTIMPL;
}

static void* GetTopLevelVisualProxy(void* topLevelWindow)
{
    if (!IsDwmObjectPointerValid(topLevelWindow, g_topLevelWindowVtable) ||
        !g_topLevelWindowGetRootVisual || g_visualProxyOffset == SIZE_MAX)
    {
        return nullptr;
    }
    constexpr int completeWindowRoot = 0;
    void* rootVisual = g_topLevelWindowGetRootVisual(topLevelWindow, completeWindowRoot);
    if (!rootVisual)
    {
        return nullptr;
    }
    const BYTE* proxyField = static_cast<const BYTE*>(rootVisual) + g_visualProxyOffset;
    if (IsReadableMemory(proxyField, sizeof(void*)))
    {
        void* proxy = *reinterpret_cast<void* const*>(proxyField);
        if (IsDwmObjectPointerValid(proxy, g_visualProxyVtable))
        {
            return proxy;
        }
    }
    return nullptr;
}

static void* GetTransitionVisualProxy(void* topLevelWindow3D)
{
    if (!HasExactDwmVtableTrusted(topLevelWindow3D, g_topLevelWindow3DVtable) ||
        g_visualProxyOffset == SIZE_MAX)
    {
        return nullptr;
    }
    // The exact vftable identifies the primary CRenderDataVisual/CVisual base;
    // reuse the proxy field offset derived from a PDB-resolved CVisual getter.
    const BYTE* proxyField =
        static_cast<const BYTE*>(topLevelWindow3D) + g_visualProxyOffset;
    if (!IsReadableMemory(proxyField, sizeof(void*)))
    {
        return nullptr;
    }
    void* proxy = *reinterpret_cast<void* const*>(proxyField);
    return IsDwmObjectPointerValid(proxy, g_visualProxyVtable) ? proxy : nullptr;
}

static constexpr int MAX_ANIMATION_SLOTS = 6;
static constexpr ULONGLONG DWM_SCENE_STALL_TIMEOUT_MS = 3000;
static constexpr ULONGLONG DWM_UNLOAD_CLEANUP_TIMEOUT_MS = 3000;

struct WindowAnimationSlot
{
    bool active;
    bool retiring;
    bool dragging;
    bool freeStepPending;
    bool windowStateThrob;
    unsigned int hookUsers;
    ULONGLONG generation;
    ULONGLONG order;
    HWND hwnd;
    WobblySettings settings;
    WobbleMesh previousMesh;
    WobbleMesh mesh;
    void* matrixTransformProxy;
    void* boundTopLevelVisualProxy;
    void* boundTransitionVisualProxy;
    bool transformAttached;
    bool transitionTransformAttached;
    ULONGLONG transformRebindRevision;
    ULONGLONG submittedTransformRebindRevision;
    ULONGLONG meshRevision;
    ULONGLONG submittedMeshRevision;
    bool meshIdentityPending;
    bool proxyCreationPending;
    ULONGLONG nextWindowValidation;
    ULONGLONG nextVisualValidation;
    bool identityApplied;
    ULONGLONG lastMatrixErrorLog;
    unsigned int privateCallFailureCount;
};

thread_local bool g_insideWobblyScenePass = false;
WindowAnimationSlot g_animationSlots[MAX_ANIMATION_SLOTS] = {};
SRWLOCK g_animationSlotsLock = SRWLOCK_INIT;
CONDITION_VARIABLE g_animationSlotsCondition = CONDITION_VARIABLE_INIT;
int g_dragAnimationSlot = -1;
ULONGLONG g_animationOrderCounter = 0;
std::atomic_bool g_unloading = false;
std::atomic_bool g_sceneWakeScheduled = false;
std::atomic<ULONGLONG> g_sceneRequestedSerial = 0;
std::atomic<ULONGLONG> g_sceneSubmittedSerial = 0;
std::atomic<ULONGLONG> g_sceneWakePostTimestamp = 0;
std::atomic<ULONGLONG> g_sceneWakeStallStartedAt = 0;
std::atomic<unsigned int> g_sceneWakeRetryCount = 0;
std::atomic_bool g_sceneWakeStalled = false;
std::atomic_bool g_sceneRecoveryCleanupPending = false;
std::atomic<ULONGLONG> g_scenePassCounter = 0;
std::atomic<ULONGLONG> g_lastFallbackInvalidationTimestamp = 0;
std::atomic<unsigned int> g_abandonedProxyCount = 0;
std::atomic<void*> g_windowListForSceneWake = nullptr;
std::atomic_bool g_sceneOwnershipResetPending = false;
ULONGLONG g_lastObservedScenePassCounter = 0;
ULONGLONG g_lastSceneProgressTimestamp = 0;
HANDLE g_animationTimer = nullptr;
LARGE_INTEGER g_animationFrequency = {};
LARGE_INTEGER g_lastAnimationCounter = {};
LARGE_INTEGER g_nextAnimationCounter = {};
double g_animationTargetHz = 0.0;
bool g_animationClockArmed = false;
bool g_animationPriorityElevated = false;
using SetThreadInformation_t = BOOL(WINAPI*)(HANDLE thread,
                                             THREAD_INFORMATION_CLASS informationClass,
                                             void* information, DWORD informationSize);

static bool ResolveDwmWindowObjects(void* windowData, void** topLevelWindow,
                                    void** topLevelWindow3D);
static void* FindWindowDataForTopLevelWindow3D(void* topLevelWindow3D);
static long __cdecl ForceUpdateSceneHook(void* pThis);
static long __cdecl UpdateSceneHook(void* pThis);
static void __cdecl AdvanceTimelinesHook(void* pThis, double currentTime);
static void __cdecl DesktopManagerHandleThreadMessageHook(UINT message,
                                                          UINT_PTR wParam,
                                                          INT_PTR lParam);
static long __cdecl EnsureTopLevelWindowHook(void* pThis, void* windowData);
static void __cdecl TopLevelWindowDestructorHook(void* pThis);
static void __cdecl TopLevelWindow3DDestructorHook(void* pThis);
static void ApplyAnimationSlotTransform(int slotIndex, double interpolationAlpha);
static void FinalizeRetiringSlots();
static void EnsurePendingMatrixTransformProxies();
static bool HasAnyAnimationSlots();
static int GetPointIndex(int x, int y);
static bool ResetMatrixTransformProxy(void* matrixTransformProxy);
static void RequestDwmScenePass(HWND hwnd);
static void MarkAnimationSlotForDwmObjectRefresh(void* windowData);
static void MarkObservedSnapTransition(HWND hwnd, bool transitionStarted);
static void HandleObservedWindowLocationChange(HWND hwnd, LONG idObject, LONG idChild);
static void ResetObservedSnapStateForInteractiveMove(HWND hwnd);
static bool PostPendingDwmSceneWake(HWND hwnd, bool forceRepost);

static void QueueMaximizedStateCheck(HWND hwnd)
{
    if (!hwnd || g_unloading.load(std::memory_order_acquire) ||
        g_realDragging.load(std::memory_order_relaxed))
    {
        return;
    }
    DWORD eventThreadId = g_eventThreadMessageTarget.load(std::memory_order_acquire);
    if (eventThreadId == 0)
    {
        return;
    }
    g_pendingMaximizedStateWindow.store(hwnd, std::memory_order_release);
    if (g_maximizedStateCheckQueued.exchange(true, std::memory_order_acq_rel))
    {
        return;
    }
    if (!PostThreadMessageW(eventThreadId, WM_WOBBLY_MAXIMIZED_CHANGE, 0, 0))
    {
        g_maximizedStateCheckQueued.store(false, std::memory_order_release);
    }
}

static bool IsApproximatelyMonitorWorkArea(const RECT& rect)
{
    if (rect.right <= rect.left || rect.bottom <= rect.top)
    {
        return false;
    }
    HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    if (!monitor)
    {
        return false;
    }
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo))
    {
        return false;
    }
    // Include the invisible resize border around maximized windows.
    constexpr LONGLONG edgeTolerance = 32;
    auto edgeIsClose = [](LONG first, LONG second)
    {
        LONGLONG difference = static_cast<LONGLONG>(first) - static_cast<LONGLONG>(second);
        return difference >= -edgeTolerance && difference <= edgeTolerance;
    };
    return edgeIsClose(rect.left, monitorInfo.rcWork.left) &&
           edgeIsClose(rect.top, monitorInfo.rcWork.top) &&
           edgeIsClose(rect.right, monitorInfo.rcWork.right) &&
           edgeIsClose(rect.bottom, monitorInfo.rcWork.bottom);
}

static bool IsApproximatelySnapLayoutTarget(const RECT& rect)
{
    if (rect.right <= rect.left || rect.bottom <= rect.top)
    {
        return false;
    }
    HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    if (!monitor)
    {
        return false;
    }
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo))
    {
        return false;
    }
    // Snap zones align with at least two work-area edges.
    constexpr LONGLONG edgeTolerance = 32;
    auto edgeIsClose = [](LONG first, LONG second)
    {
        LONGLONG difference = static_cast<LONGLONG>(first) - static_cast<LONGLONG>(second);
        return difference >= -edgeTolerance && difference <= edgeTolerance;
    };
    const RECT& workArea = monitorInfo.rcWork;
    if (rect.left < workArea.left - edgeTolerance || rect.top < workArea.top - edgeTolerance ||
        rect.right > workArea.right + edgeTolerance ||
        rect.bottom > workArea.bottom + edgeTolerance)
    {
        return false;
    }
    int alignedEdges = 0;
    alignedEdges += edgeIsClose(rect.left, workArea.left) ? 1 : 0;
    alignedEdges += edgeIsClose(rect.top, workArea.top) ? 1 : 0;
    alignedEdges += edgeIsClose(rect.right, workArea.right) ? 1 : 0;
    alignedEdges += edgeIsClose(rect.bottom, workArea.bottom) ? 1 : 0;
    // Four aligned edges are the maximized work area, not a Snap Layout zone.
    return alignedEdges >= 2 && alignedEdges < 4;
}

static bool IsEligibleWindowForStateThrob(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd)
    {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR extendedStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    // Exclude shell surfaces, notifications, menus and tool windows.
    if ((style & WS_CHILD) != 0 || (style & WS_THICKFRAME) == 0 ||
        (extendedStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) != 0 ||
        GetWindow(hwnd, GW_OWNER) != nullptr)
    {
        return false;
    }
    return true;
}

static bool IsShellCloakedWindow(HWND hwnd);

struct ExistingWindowCollection
{
    HWND windows[MAX_EXISTING_WINDOW_BACKFILL];
    unsigned int count;
};

static BOOL CALLBACK CollectExistingWindowForBackfill(HWND hwnd, LPARAM parameter)
{
    auto* collection = reinterpret_cast<ExistingWindowCollection*>(parameter);
    if (collection->count >= MAX_EXISTING_WINDOW_BACKFILL)
    {
        return FALSE;
    }
    if (IsWindowVisible(hwnd) && !IsShellCloakedWindow(hwnd) &&
        IsEligibleWindowForStateThrob(hwnd))
    {
        collection->windows[collection->count++] = hwnd;
    }
    return TRUE;
}

static void QueueExistingWindowBackfill()
{
    ExistingWindowCollection collection = {};
    EnumWindows(CollectExistingWindowForBackfill,
                reinterpret_cast<LPARAM>(&collection));
    AcquireSRWLockExclusive(&g_existingWindowBackfillLock);
    g_existingWindowBackfillCount.store(0, std::memory_order_release);
    g_existingWindowBackfillIndex.store(0, std::memory_order_release);
    g_existingWindowBackfillMapped.store(0, std::memory_order_release);
    std::copy_n(collection.windows, collection.count, g_existingWindowBackfill);
    g_existingWindowBackfillCount.store(collection.count, std::memory_order_release);
    HWND firstWindow = collection.count ? collection.windows[0] : nullptr;
    ReleaseSRWLockExclusive(&g_existingWindowBackfillLock);
    if (collection.count != 0)
    {
        Wh_Log(L"DWM existing-window backfill queued: %u windows", collection.count);
        RequestDwmScenePass(firstWindow);
    }
}

static LPARAM EncodeWindowTransitionDirection(const RECT& sourceRect, const RECT& targetRect)
{
    if (sourceRect.right <= sourceRect.left || sourceRect.bottom <= sourceRect.top ||
        targetRect.right <= targetRect.left || targetRect.bottom <= targetRect.top)
    {
        return 0;
    }
    double sourceCentreX =
        (static_cast<double>(sourceRect.left) + static_cast<double>(sourceRect.right)) * 0.5;
    double sourceCentreY =
        (static_cast<double>(sourceRect.top) + static_cast<double>(sourceRect.bottom)) * 0.5;
    double targetCentreX =
        (static_cast<double>(targetRect.left) + static_cast<double>(targetRect.right)) * 0.5;
    double targetCentreY =
        (static_cast<double>(targetRect.top) + static_cast<double>(targetRect.bottom)) * 0.5;
    double deltaX = targetCentreX - sourceCentreX;
    double deltaY = targetCentreY - sourceCentreY;
    // Ignore resize-border and mixed-DPI rounding noise.
    constexpr double directionThreshold = 8.0;
    LPARAM directionFlags = 0;
    if (deltaX < -directionThreshold)
    {
        directionFlags |= NATIVE_TRANSITION_DIRECTION_LEFT;
    }
    else if (deltaX > directionThreshold)
    {
        directionFlags |= NATIVE_TRANSITION_DIRECTION_RIGHT;
    }
    if (deltaY < -directionThreshold)
    {
        directionFlags |= NATIVE_TRANSITION_DIRECTION_UP;
    }
    else if (deltaY > directionThreshold)
    {
        directionFlags |= NATIVE_TRANSITION_DIRECTION_DOWN;
    }
    return directionFlags;
}

static LPARAM EncodeWindowTransitionDirection(const Vec2& direction)
{
    LPARAM flags = direction.x < 0.0   ? NATIVE_TRANSITION_DIRECTION_LEFT
                   : direction.x > 0.0 ? NATIVE_TRANSITION_DIRECTION_RIGHT
                                       : 0;
    flags |= direction.y < 0.0   ? NATIVE_TRANSITION_DIRECTION_UP
             : direction.y > 0.0 ? NATIVE_TRANSITION_DIRECTION_DOWN
                                 : 0;
    return flags;
}

static LPARAM BuildWindowTransitionFlags(HWND hwnd, const RECT& targetRect)
{
    if (!hwnd)
    {
        return 0;
    }
    RECT sourceRect = {};
    bool hasSourceRect = GetWindowRect(hwnd, &sourceRect) != FALSE;
    LPARAM flags = 0;
    if (IsApproximatelyMonitorWorkArea(targetRect))
    {
        flags |= NATIVE_TRANSITION_TARGET_IS_WORK_AREA;
    }
    else if (IsApproximatelySnapLayoutTarget(targetRect))
    {
        flags |= NATIVE_TRANSITION_TARGET_IS_SNAP_LAYOUT;
    }
    if (hasSourceRect)
    {
        flags |= EncodeWindowTransitionDirection(sourceRect, targetRect);
        if (IsApproximatelyMonitorWorkArea(sourceRect))
        {
            flags |= NATIVE_TRANSITION_SOURCE_IS_WORK_AREA;
        }
    }
    if (IsZoomed(hwnd))
    {
        flags |= NATIVE_TRANSITION_WINDOW_IS_ZOOMED;
    }
    return flags;
}

static Vec2 DecodeWindowTransitionDirection(LPARAM transitionFlags)
{
    Vec2 direction = {};
    if ((transitionFlags & NATIVE_TRANSITION_DIRECTION_LEFT) != 0)
    {
        direction.x -= 1.0;
    }
    if ((transitionFlags & NATIVE_TRANSITION_DIRECTION_RIGHT) != 0)
    {
        direction.x += 1.0;
    }
    if ((transitionFlags & NATIVE_TRANSITION_DIRECTION_UP) != 0)
    {
        direction.y -= 1.0;
    }
    if ((transitionFlags & NATIVE_TRANSITION_DIRECTION_DOWN) != 0)
    {
        direction.y += 1.0;
    }
    return direction;
}

static void QueueNativeWindowTransition(HWND hwnd, LPARAM transitionFlags)
{
    if (!hwnd || transitionFlags == 0 || g_unloading.load(std::memory_order_acquire))
    {
        return;
    }
    if (g_realDragging.load(std::memory_order_relaxed))
    {
        // Queue early Snap-in; Snap-out keeps the normal drag wobble.
        if ((transitionFlags & NATIVE_TRANSITION_TARGET_IS_SNAP_LAYOUT) != 0 &&
            g_realDraggedWindow.load(std::memory_order_relaxed) == hwnd)
        {
            // Publish direction before the release-store of its HWND.
            g_pendingInteractiveSnapFlags.store(transitionFlags, std::memory_order_relaxed);
            g_pendingInteractiveSnapWindow.store(hwnd, std::memory_order_release);
        }
        return;
    }
    DWORD eventThreadId = g_eventThreadMessageTarget.load(std::memory_order_acquire);
    if (eventThreadId != 0)
    {
        PostThreadMessageW(eventThreadId, WM_WOBBLY_NATIVE_WINDOW_TRANSITION,
                           reinterpret_cast<WPARAM>(hwnd), transitionFlags);
    }
}

static long __cdecl WindowTransitionChangeHook(void* pThis, void* dwmWindow, int transitionTarget,
                                               const RECT& targetRect, const RECT& rect2,
                                               const RECT& rect3, const RECT& rect4,
                                               const RECT& rect5)
{
    if (HasExactDwmVtableTrusted(pThis, g_windowListVtable))
    {
        g_windowListForSceneWake.store(pThis, std::memory_order_release);
    }
    HWND hwnd = nullptr;
    if (pThis && dwmWindow && HasSyncedWindowData())
    {
        void* windowData = nullptr;
        if (GetSyncedWindowDataCompat(pThis, dwmWindow, true, &windowData) &&
            g_windowDataHwndOffset != SIZE_MAX)
        {
            hwnd = GetHwndFromWindowData(windowData);
        }
    }
    // Queue before the native transition timeline starts.
    QueueNativeWindowTransition(hwnd, BuildWindowTransitionFlags(hwnd, targetRect));
    return g_windowTransitionChangeOriginal(pThis, dwmWindow, transitionTarget, targetRect, rect2,
                                            rect3, rect4, rect5);
}

static long __cdecl StartAnimationForMaximizeSnapTransitionHook(void* pThis, int animationType,
                                                                 const RECT& targetRect)
{
    HWND hwnd = nullptr;
    if (pThis &&
        (IsDwmObjectPointerValid(pThis, g_topLevelWindow3DVtable) ||
         LearnDwmVtableFromTrustedObject(pThis, g_topLevelWindow3DVtable)))
    {
        void* windowData = FindWindowDataForTopLevelWindow3D(pThis);
        if (!windowData && g_topLevelWindow3DWindowDataOffset != SIZE_MAX)
        {
            BYTE* windowDataField =
                static_cast<BYTE*>(pThis) + g_topLevelWindow3DWindowDataOffset;
            if (IsReadableMemory(windowDataField, sizeof(void*)))
            {
                windowData = *reinterpret_cast<void**>(windowDataField);
            }
        }
        if (windowData)
        {
            hwnd = GetHwndFromWindowData(windowData);
            if (hwnd)
            {
                RegisterDwmWindowMapping(windowData, nullptr, pThis);
            }
        }
    }
    // Seed the wobble before uDWM starts its own maximize/restore timeline.
    QueueNativeWindowTransition(hwnd, BuildWindowTransitionFlags(hwnd, targetRect));
    return g_startAnimationForMaximizeSnapTransitionOriginal(pThis, animationType, targetRect);
}

static void __cdecl OnPositionChangeHook(void* pThis, void* pWindowData, bool unknown)
{
    if (HasExactDwmVtableTrusted(pThis, g_windowListVtable))
    {
        g_windowListForSceneWake.store(pThis, std::memory_order_release);
    }
    g_onPositionChangeOriginal(pThis, pWindowData, unknown);
    if (!pWindowData || g_unloading.load(std::memory_order_acquire))
    {
        return;
    }
    HWND hwnd = GetHwndFromTrustedWindowData(pWindowData);
    if (!hwnd)
    {
        return;
    }
    QueueMaximizedStateCheck(hwnd);
}

static void __cdecl CheckForMaximizedChangeHook(void* pThis, void* pWindowData)
{
    if (HasExactDwmVtableTrusted(pThis, g_windowListVtable))
    {
        g_windowListForSceneWake.store(pThis, std::memory_order_release);
    }
    g_checkForMaximizedChangeOriginal(pThis, pWindowData);
    if (!pWindowData)
    {
        return;
    }
    HWND hwnd = GetHwndFromTrustedWindowData(pWindowData);
    QueueMaximizedStateCheck(hwnd);
}

unsigned int g_moveEventCounter = 0;

static bool IsAddressRangeWithin(const void* address, size_t size, const BYTE* begin,
                                 const BYTE* end)
{
    if (!address || size == 0 || !begin || !end || begin >= end)
    {
        return false;
    }
    uintptr_t start = reinterpret_cast<uintptr_t>(address);
    if (start > UINTPTR_MAX - size)
    {
        return false;
    }
    uintptr_t finish = start + size;
    return start >= reinterpret_cast<uintptr_t>(begin) &&
           finish <= reinterpret_cast<uintptr_t>(end);
}

static bool IsReadableMemory(const void* address, size_t size)
{
    if (!address || size == 0)
    {
        return false;
    }
    uintptr_t current = reinterpret_cast<uintptr_t>(address);
    if (current > UINTPTR_MAX - size)
    {
        return false;
    }
    uintptr_t finish = current + size;
    while (current < finish)
    {
        MEMORY_BASIC_INFORMATION mbi = {};
        if (!VirtualQuery(reinterpret_cast<const void*>(current), &mbi, sizeof(mbi)) ||
            mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0)
        {
            return false;
        }
        DWORD readableProtection = mbi.Protect & 0xFF;
        if (readableProtection != PAGE_READONLY && readableProtection != PAGE_READWRITE &&
            readableProtection != PAGE_WRITECOPY && readableProtection != PAGE_EXECUTE_READ &&
            readableProtection != PAGE_EXECUTE_READWRITE &&
            readableProtection != PAGE_EXECUTE_WRITECOPY)
        {
            return false;
        }
        uintptr_t regionEnd = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (regionEnd <= current)
        {
            return false;
        }
        current = std::min(regionEnd, finish);
    }
    return true;
}

static bool InitializeDwmModuleLayout(HMODULE module)
{
    g_dwmModuleLayout = {};
    if (!module || !IsReadableMemory(module, sizeof(IMAGE_DOS_HEADER)))
    {
        return false;
    }
    const BYTE* imageBase = reinterpret_cast<const BYTE*>(module);
    const IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(imageBase);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE || dosHeader->e_lfanew <= 0 ||
        static_cast<size_t>(dosHeader->e_lfanew) > 0x100000)
    {
        return false;
    }
    const IMAGE_NT_HEADERS64* ntHeaders =
        reinterpret_cast<const IMAGE_NT_HEADERS64*>(imageBase + dosHeader->e_lfanew);
    if (!IsReadableMemory(ntHeaders, sizeof(*ntHeaders)) ||
        ntHeaders->Signature != IMAGE_NT_SIGNATURE ||
        ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        ntHeaders->OptionalHeader.SizeOfImage < 0x10000)
    {
        return false;
    }
    const BYTE* imageEnd = imageBase + ntHeaders->OptionalHeader.SizeOfImage;
    g_dwmModuleLayout.module = module;
    g_dwmModuleLayout.imageBegin = imageBase;
    g_dwmModuleLayout.imageEnd = imageEnd;
    g_dwmModuleLayout.timeDateStamp = ntHeaders->FileHeader.TimeDateStamp;
    g_dwmModuleLayout.sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    const IMAGE_SECTION_HEADER* sections =
        IMAGE_FIRST_SECTION(const_cast<IMAGE_NT_HEADERS64*>(ntHeaders));
    if (!IsReadableMemory(sections, static_cast<size_t>(ntHeaders->FileHeader.NumberOfSections) *
                                        sizeof(*sections)))
    {
        g_dwmModuleLayout = {};
        return false;
    }
    for (unsigned int i = 0;
         i < ntHeaders->FileHeader.NumberOfSections &&
         g_dwmModuleLayout.executableRangeCount < ARRAYSIZE(g_dwmModuleLayout.executableRanges);
         i++)
    {
        if ((sections[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) == 0)
        {
            continue;
        }
        size_t sectionSize =
            std::max<size_t>(sections[i].Misc.VirtualSize, sections[i].SizeOfRawData);
        if (sectionSize == 0 || sections[i].VirtualAddress > g_dwmModuleLayout.sizeOfImage ||
            sectionSize > g_dwmModuleLayout.sizeOfImage - sections[i].VirtualAddress)
        {
            g_dwmModuleLayout = {};
            return false;
        }
        DwmAddressRange& range =
            g_dwmModuleLayout.executableRanges[g_dwmModuleLayout.executableRangeCount++];
        range.begin = imageBase + sections[i].VirtualAddress;
        range.end = range.begin + sectionSize;
    }
    return g_dwmModuleLayout.executableRangeCount != 0;
}

static bool IsDwmImageAddress(const void* address, size_t size)
{
    return IsAddressRangeWithin(address, size, g_dwmModuleLayout.imageBegin,
                                g_dwmModuleLayout.imageEnd);
}

static bool IsDwmExecutableAddress(const void* address)
{
    for (unsigned int i = 0; i < g_dwmModuleLayout.executableRangeCount; i++)
    {
        if (IsAddressRangeWithin(address, 1, g_dwmModuleLayout.executableRanges[i].begin,
                                 g_dwmModuleLayout.executableRanges[i].end))
        {
            return true;
        }
    }
    return false;
}

static bool IsDwmFunctionPointerValid(const void* function)
{
    return function && IsDwmExecutableAddress(function);
}

static bool HasExactDwmVtableTrusted(void* object, std::atomic<void*>& expectedVtable)
{
    void* expected = expectedVtable.load(std::memory_order_acquire);
    return object && expected && *reinterpret_cast<void**>(object) == expected;
}

static bool IsDwmObjectPointerValid(void* object, std::atomic<void*>& expectedVtable)
{
    void* knownVtable = expectedVtable.load(std::memory_order_acquire);
    if (!knownVtable || !object || !IsReadableMemory(object, sizeof(void*)))
    {
        return false;
    }
    void* actualVtable = *reinterpret_cast<void**>(object);
    return actualVtable == knownVtable;
}

static bool LearnDwmVtableFromTrustedObject(void* object,
                                            std::atomic<void*>& targetVtable)
{
    if (!object || !IsReadableMemory(object, sizeof(void*)))
    {
        return false;
    }
    void* candidateVtable = *reinterpret_cast<void**>(object);
    if (!IsDwmImageAddress(candidateVtable, sizeof(void*) * 3) ||
        !IsReadableMemory(candidateVtable, sizeof(void*) * 3))
    {
        return false;
    }
    void** functions = static_cast<void**>(candidateVtable);
    for (int i = 0; i < 3; i++)
    {
        if (!IsDwmFunctionPointerValid(functions[i]))
        {
            return false;
        }
    }
    void* expected = nullptr;
    targetVtable.compare_exchange_strong(expected, candidateVtable,
                                         std::memory_order_acq_rel,
                                         std::memory_order_acquire);
    return targetVtable.load(std::memory_order_acquire) == candidateVtable;
}

static size_t FindOffsetFromFunction(void* function, size_t defaultValue)
{
    if (!IsDwmFunctionPointerValid(function))
    {
        return defaultValue;
    }
    BYTE* instruction = static_cast<BYTE*>(function);
    static const std::regex pattern(
        R"(mov \w+, (?:qword ptr )?\[rcx\+0x([0-9a-f]{1,8})\])",
        std::regex_constants::icase);
    size_t bytesRead = 0;
    for (int i = 0; i < 32 && bytesRead < 256; i++)
    {
        WH_DISASM_RESULT result = {};
        if (!IsDwmExecutableAddress(instruction) || !Wh_Disasm(instruction, &result) ||
            result.length == 0)
        {
            break;
        }
        std::string_view text = result.text;
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(text.begin(), text.end(), match, pattern))
        {
            size_t offset = std::stoull(match[1].str(), nullptr, 16);
            if (offset <= 0x1000)
            {
                return offset;
            }
        }
        if (text == "ret")
        {
            break;
        }
        instruction += result.length;
        bytesRead += result.length;
    }
    return defaultValue;
}

static size_t FindDesktopManagerThreadIdOffset(void* function)
{
    if (!IsDwmFunctionPointerValid(function))
    {
        return SIZE_MAX;
    }
    std::string thisAliases[8] = {"rcx"};
    unsigned int aliasCount = 1;
    auto isThisAlias = [&](const std::string& name)
    {
        for (unsigned int i = 0; i < aliasCount; i++)
        {
            if (thisAliases[i] == name)
            {
                return true;
            }
        }
        return false;
    };
    auto addThisAlias = [&](const std::string& name)
    {
        if (!isThisAlias(name) && aliasCount < ARRAYSIZE(thisAliases))
        {
            thisAliases[aliasCount++] = name;
        }
    };
    static const std::regex movePattern(R"(^mov (r[a-z0-9]+), (r[a-z0-9]+)$)",
                                        std::regex_constants::icase);
    static const std::regex loadPattern(
        R"(^mov (?:e[a-z0-9]+|r[0-9]+d), (?:dword ptr )?\[(r[a-z0-9]+)\s*\+\s*0x([0-9a-f]{1,8})\]$)",
        std::regex_constants::icase);
    static const std::regex wakeMessagePattern(R"(^mov edx, 0x0*400$)",
                                               std::regex_constants::icase);
    BYTE* instruction = static_cast<BYTE*>(function);
    size_t candidate = SIZE_MAX;
    size_t bytesRead = 0;
    for (int i = 0; i < 64 && bytesRead < 512; i++)
    {
        WH_DISASM_RESULT result = {};
        if (!IsDwmExecutableAddress(instruction) || !Wh_Disasm(instruction, &result) ||
            result.length == 0)
        {
            break;
        }
        std::string text = result.text;
        std::smatch match;
        if (std::regex_match(text, match, movePattern) && isThisAlias(match[2].str()))
        {
            addThisAlias(match[1].str());
        }
        else if (std::regex_match(text, match, loadPattern) &&
                 isThisAlias(match[1].str()))
        {
            size_t offset = std::stoull(match[2].str(), nullptr, 16);
            if (offset >= sizeof(void*) && offset <= 0x1000 && offset % sizeof(DWORD) == 0)
            {
                candidate = offset;
            }
        }
        else if (std::regex_match(text, wakeMessagePattern))
        {
            return candidate;
        }
        if (text == "ret")
        {
            break;
        }
        instruction += result.length;
        bytesRead += result.length;
    }
    return SIZE_MAX;
}

static size_t FindStoredWindowDataOffset(void* function)
{
    if (!IsDwmFunctionPointerValid(function))
    {
        return SIZE_MAX;
    }
    std::string thisAliases[8] = {"rcx"};
    std::string dataAliases[8] = {"rdx"};
    unsigned int thisAliasCount = 1;
    unsigned int dataAliasCount = 1;
    auto contains = [](const std::string* aliases, unsigned int count,
                       const std::string& name)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            if (aliases[i] == name)
            {
                return true;
            }
        }
        return false;
    };
    auto addAlias = [&](std::string* aliases, unsigned int& count,
                        const std::string& name)
    {
        if (!contains(aliases, count, name) && count < 8)
        {
            aliases[count++] = name;
        }
    };
    static const std::regex movePattern(R"(^mov (r[a-z0-9]+), (r[a-z0-9]+)$)",
                                        std::regex_constants::icase);
    static const std::regex storePattern(
        R"(^mov (?:qword ptr )?\[(r[a-z0-9]+)\s*\+\s*0x([0-9a-f]{1,8})\], (r[a-z0-9]+)$)",
        std::regex_constants::icase);
    BYTE* instruction = static_cast<BYTE*>(function);
    size_t bytesRead = 0;
    for (int i = 0; i < 64 && bytesRead < 512; i++)
    {
        WH_DISASM_RESULT result = {};
        if (!IsDwmExecutableAddress(instruction) || !Wh_Disasm(instruction, &result) ||
            result.length == 0)
        {
            break;
        }
        std::string text = result.text;
        std::smatch match;
        if (std::regex_match(text, match, movePattern))
        {
            const std::string destination = match[1].str();
            const std::string source = match[2].str();
            if (contains(thisAliases, thisAliasCount, source))
            {
                addAlias(thisAliases, thisAliasCount, destination);
            }
            if (contains(dataAliases, dataAliasCount, source))
            {
                addAlias(dataAliases, dataAliasCount, destination);
            }
        }
        else if (std::regex_match(text, match, storePattern) &&
                 contains(thisAliases, thisAliasCount, match[1].str()) &&
                 contains(dataAliases, dataAliasCount, match[3].str()))
        {
            size_t offset = std::stoull(match[2].str(), nullptr, 16);
            if (offset >= sizeof(void*) && offset <= 0x1000 && offset % sizeof(void*) == 0)
            {
                return offset;
            }
        }
        if (text == "ret")
        {
            break;
        }
        instruction += result.length;
        bytesRead += result.length;
    }
    return SIZE_MAX;
}

static size_t FindVisualProxyOffsetFromCanvasGetter(void* function)
{
    if (!IsDwmFunctionPointerValid(function))
    {
        return SIZE_MAX;
    }
    static const std::regex loadPattern(
        R"(^mov (r[a-z0-9]+), (?:qword ptr )?\[(r[a-z0-9]+)\+0x([0-9a-f]{1,8})\]$)",
        std::regex_constants::icase);
    BYTE* instruction = static_cast<BYTE*>(function);
    size_t candidate = SIZE_MAX;
    std::string visualRegister;
    size_t bytesRead = 0;
    for (int i = 0; i < 32 && bytesRead < 256; i++)
    {
        WH_DISASM_RESULT result = {};
        if (!IsDwmExecutableAddress(instruction) || !Wh_Disasm(instruction, &result) ||
            result.length == 0)
        {
            break;
        }
        std::string text = result.text;
        std::smatch match;
        if (std::regex_match(text, match, loadPattern))
        {
            const std::string destination = match[1].str();
            const std::string base = match[2].str();
            size_t offset = std::stoull(match[3].str(), nullptr, 16);
            if (base == "rcx" && offset >= 0x80 && offset <= 0x400 &&
                offset % sizeof(void*) == 0)
            {
                visualRegister = destination;
            }
            else if (!visualRegister.empty() && destination == "rax" &&
                     base == visualRegister && offset <= 0x80 &&
                     offset % sizeof(void*) == 0)
            {
                if (candidate != SIZE_MAX && candidate != offset)
                {
                    return SIZE_MAX;
                }
                candidate = offset;
            }
        }
        if (text == "ret")
        {
            break;
        }
        instruction += result.length;
        bytesRead += result.length;
    }
    return candidate;
}

static size_t FindDesktopManagerCompositorOffset(void* initializeFunction,
                                                 void* compositorCreateFunction)
{
    if (!IsDwmFunctionPointerValid(initializeFunction) ||
        !IsDwmFunctionPointerValid(compositorCreateFunction))
    {
        return SIZE_MAX;
    }
    enum class RegisterKind
    {
        Unknown,
        This,
        MemberAddress
    };
    struct RegisterState
    {
        std::string name;
        RegisterKind kind;
        size_t offset;
    };
    RegisterState registers[16] = {{"rcx", RegisterKind::This, 0}};
    unsigned int registerCount = 1;
    auto findRegister = [&](const std::string& name) -> RegisterState*
    {
        for (unsigned int i = 0; i < registerCount; i++)
        {
            if (registers[i].name == name)
            {
                return &registers[i];
            }
        }
        if (registerCount >= ARRAYSIZE(registers))
        {
            return nullptr;
        }
        registers[registerCount] = {name, RegisterKind::Unknown, 0};
        return &registers[registerCount++];
    };
    auto clearVolatileRegisters = [&]
    {
        for (const char* name : {"rcx", "rdx", "r8", "r9", "r10", "r11"})
        {
            if (RegisterState* state = findRegister(name))
            {
                state->kind = RegisterKind::Unknown;
                state->offset = 0;
            }
        }
    };
    static const std::regex movePattern(R"(^mov (r[a-z0-9]+), (r[a-z0-9]+)$)",
                                        std::regex_constants::icase);
    static const std::regex leaPattern(
        R"(^lea (r[a-z0-9]+), \[(r[a-z0-9]+)\+0x([0-9a-f]{1,8})\]$)",
        std::regex_constants::icase);
    static const std::regex callPattern(R"(^call 0x([0-9a-f]+)$)",
                                        std::regex_constants::icase);
    BYTE* instruction = static_cast<BYTE*>(initializeFunction);
    size_t bytesRead = 0;
    for (int i = 0; i < 1024 && bytesRead < 4096; i++)
    {
        WH_DISASM_RESULT result = {};
        if (!IsDwmExecutableAddress(instruction) || !Wh_Disasm(instruction, &result) ||
            result.length == 0)
        {
            break;
        }
        std::string text = result.text;
        std::smatch match;
        if (std::regex_match(text, match, movePattern))
        {
            RegisterState* destination = findRegister(match[1].str());
            RegisterState* source = findRegister(match[2].str());
            if (destination && source)
            {
                destination->kind = source->kind;
                destination->offset = source->offset;
            }
        }
        else if (std::regex_match(text, match, leaPattern))
        {
            RegisterState* destination = findRegister(match[1].str());
            RegisterState* base = findRegister(match[2].str());
            size_t offset = std::stoull(match[3].str(), nullptr, 16);
            if (destination)
            {
                destination->kind = base && base->kind == RegisterKind::This
                                        ? RegisterKind::MemberAddress
                                        : RegisterKind::Unknown;
                destination->offset = offset;
            }
        }
        else if (std::regex_match(text, match, callPattern))
        {
            uintptr_t target = std::stoull(match[1].str(), nullptr, 16);
            if (target == reinterpret_cast<uintptr_t>(compositorCreateFunction))
            {
                RegisterState* argument = findRegister("rcx");
                if (argument && argument->kind == RegisterKind::MemberAddress &&
                    argument->offset >= sizeof(void*) && argument->offset <= 0x400 &&
                    argument->offset % sizeof(void*) == 0)
                {
                    return argument->offset;
                }
                return SIZE_MAX;
            }
            clearVolatileRegisters();
        }
        if (text == "ret")
        {
            break;
        }
        instruction += result.length;
        bytesRead += result.length;
    }
    return SIZE_MAX;
}

static bool FindConstructorWindowDataOffsets(void* function, size_t* windowDataTopLevelOffset,
                                             size_t* topLevelWindowDataOffset)
{
    *windowDataTopLevelOffset = SIZE_MAX;
    *topLevelWindowDataOffset = SIZE_MAX;
    if (!IsDwmFunctionPointerValid(function))
    {
        return false;
    }
    std::string thisAliases[8] = {"rcx"};
    std::string windowDataAliases[8] = {"rdx"};
    unsigned int thisAliasCount = 1;
    unsigned int windowDataAliasCount = 1;
    auto contains = [](const std::string aliases[], unsigned int count,
                       const std::string& value)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            if (aliases[i] == value)
            {
                return true;
            }
        }
        return false;
    };
    auto remove = [](std::string aliases[], unsigned int& count, const std::string& value)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            if (aliases[i] == value)
            {
                aliases[i] = aliases[--count];
                return;
            }
        }
    };
    auto add = [&](std::string aliases[], unsigned int& count, const std::string& value)
    {
        if (!contains(aliases, count, value) && count < 8)
        {
            aliases[count++] = value;
        }
    };
    auto clearVolatileAliases = [&](std::string aliases[], unsigned int& count)
    {
        static const char* volatileRegisters[] = {"rcx", "rdx", "r8", "r9", "r10", "r11"};
        for (const char* reg : volatileRegisters)
        {
            remove(aliases, count, reg);
        }
    };
    static const std::regex movePattern(R"(^mov (r[a-z0-9]+), (r[a-z0-9]+)$)",
                                        std::regex_constants::icase);
    static const std::regex storePattern(
        R"(^mov (?:qword ptr )?\[(r[a-z0-9]+)\+0x([0-9a-f]{1,8})\], (r[a-z0-9]+)$)",
        std::regex_constants::icase);
    size_t forwardCandidate = SIZE_MAX;
    size_t reverseCandidate = SIZE_MAX;
    bool ambiguous = false;
    BYTE* instruction = static_cast<BYTE*>(function);
    size_t bytesRead = 0;
    for (int i = 0; i < 256 && bytesRead < 1024; i++)
    {
        WH_DISASM_RESULT result = {};
        if (!IsDwmExecutableAddress(instruction) || !Wh_Disasm(instruction, &result) ||
            result.length == 0)
        {
            break;
        }
        std::string text = result.text;
        std::smatch match;
        if (std::regex_match(text, match, movePattern))
        {
            std::string destination = match[1].str();
            std::string source = match[2].str();
            bool sourceIsThis = contains(thisAliases, thisAliasCount, source);
            bool sourceIsWindowData =
                contains(windowDataAliases, windowDataAliasCount, source);
            remove(thisAliases, thisAliasCount, destination);
            remove(windowDataAliases, windowDataAliasCount, destination);
            if (sourceIsThis)
            {
                add(thisAliases, thisAliasCount, destination);
            }
            else if (sourceIsWindowData)
            {
                add(windowDataAliases, windowDataAliasCount, destination);
            }
        }
        else if (std::regex_match(text, match, storePattern))
        {
            std::string base = match[1].str();
            std::string source = match[3].str();
            size_t offset = std::stoull(match[2].str(), nullptr, 16);
            if (offset >= 0x80 && offset <= 0x800 && offset % sizeof(void*) == 0)
            {
                size_t* candidate = nullptr;
                if (contains(windowDataAliases, windowDataAliasCount, base) &&
                    contains(thisAliases, thisAliasCount, source))
                {
                    candidate = &forwardCandidate;
                }
                else if (contains(thisAliases, thisAliasCount, base) &&
                         contains(windowDataAliases, windowDataAliasCount, source))
                {
                    candidate = &reverseCandidate;
                }
                if (candidate)
                {
                    if (*candidate != SIZE_MAX && *candidate != offset)
                    {
                        ambiguous = true;
                    }
                    *candidate = offset;
                }
            }
        }
        if (text.rfind("call ", 0) == 0)
        {
            clearVolatileAliases(thisAliases, thisAliasCount);
            clearVolatileAliases(windowDataAliases, windowDataAliasCount);
        }
        if (text == "ret")
        {
            break;
        }
        instruction += result.length;
        bytesRead += result.length;
    }
    if (ambiguous || forwardCandidate == SIZE_MAX || reverseCandidate == SIZE_MAX)
    {
        Wh_Log(L"DWM constructor offset scan failed: forward=0x%zx reverse=0x%zx ambiguous=%d",
               forwardCandidate, reverseCandidate, ambiguous);
        return false;
    }
    *windowDataTopLevelOffset = forwardCandidate;
    *topLevelWindowDataOffset = reverseCandidate;
    return true;
}

static bool FindWindowDataTopLevelOffsets(void* function, size_t* topLevelWindowOffset,
                                          size_t* topLevelWindow3DOffset)
{
    *topLevelWindowOffset = SIZE_MAX;
    *topLevelWindow3DOffset = SIZE_MAX;
    if (!IsDwmFunctionPointerValid(function))
    {
        return false;
    }

    struct OffsetCandidate
    {
        size_t offset;
        unsigned int references;
        unsigned int zeroTests;
        unsigned int writes;
    };

    OffsetCandidate candidates[64] = {};
    unsigned int candidateCount = 0;
    std::string aliases[8] = {"rdx"};
    unsigned int aliasCount = 1;
    static const std::regex movePattern(R"(mov (r[a-z0-9]+), (r[a-z0-9]+))",
                                        std::regex_constants::icase);
    // Accept common compiler encodings when deriving these fields.
    static const std::regex memoryPattern(
        R"(\[(r[a-z0-9]+)\+0x([0-9a-f]{1,8})\])", std::regex_constants::icase);
    static const std::regex zeroTestPattern(
        R"(^cmp (?:qword ptr )?\[[^\]]+\], (?:0x)?0$)", std::regex_constants::icase);
    static const std::regex writePattern(
        R"(^mov (?:qword ptr )?\[[^\]]+\], )", std::regex_constants::icase);
    BYTE* instruction = static_cast<BYTE*>(function);
    size_t bytesRead = 0;
    for (int i = 0; i < 192 && bytesRead < 768; i++)
    {
        WH_DISASM_RESULT result = {};
        if (!IsDwmExecutableAddress(instruction) || !Wh_Disasm(instruction, &result) ||
            result.length == 0)
        {
            return false;
        }
        std::string text = result.text;
        std::smatch match;
        if (std::regex_match(text, match, movePattern))
        {
            bool sourceIsAlias = false;
            for (unsigned int aliasIndex = 0; aliasIndex < aliasCount; aliasIndex++)
            {
                if (match[2].str() == aliases[aliasIndex])
                {
                    sourceIsAlias = true;
                    break;
                }
            }
            if (sourceIsAlias && aliasCount < ARRAYSIZE(aliases))
            {
                std::string destination = match[1].str();
                bool alreadyKnown = false;
                for (unsigned int aliasIndex = 0; aliasIndex < aliasCount; aliasIndex++)
                {
                    alreadyKnown = alreadyKnown || aliases[aliasIndex] == destination;
                }
                if (!alreadyKnown)
                {
                    aliases[aliasCount++] = destination;
                }
            }
        }
        if (std::regex_search(text, match, memoryPattern))
        {
            bool baseIsAlias = false;
            for (unsigned int aliasIndex = 0; aliasIndex < aliasCount; aliasIndex++)
            {
                if (match[1].str() == aliases[aliasIndex])
                {
                    baseIsAlias = true;
                    break;
                }
            }
            if (baseIsAlias)
            {
                size_t offset = std::stoull(match[2].str(), nullptr, 16);
                if (offset >= 0x80 && offset <= 0x400 && offset % sizeof(void*) == 0)
                {
                    unsigned int candidateIndex = 0;
                    for (; candidateIndex < candidateCount; candidateIndex++)
                    {
                        if (candidates[candidateIndex].offset == offset)
                        {
                            candidates[candidateIndex].references++;
                            break;
                        }
                    }
                    if (candidateIndex == candidateCount && candidateCount < ARRAYSIZE(candidates))
                    {
                        candidates[candidateCount++] = {offset, 1, 0, 0};
                    }
                    OffsetCandidate& candidate = candidates[candidateIndex];
                    if (std::regex_match(text, zeroTestPattern))
                    {
                        candidate.zeroTests++;
                    }
                    if (std::regex_search(text, writePattern))
                    {
                        candidate.writes++;
                    }
                }
            }
        }
        if (text == "ret")
        {
            break;
        }
        instruction += result.length;
        bytesRead += result.length;
    }
    unsigned int bestScore = 0;
    unsigned int bestPairCount = 0;
    size_t bestLowerOffset = SIZE_MAX;
    size_t bestUpperOffset = SIZE_MAX;
    for (unsigned int lowerIndex = 0; lowerIndex < candidateCount; lowerIndex++)
    {
        for (unsigned int upperIndex = 0; upperIndex < candidateCount; upperIndex++)
        {
            if (candidates[upperIndex].offset == candidates[lowerIndex].offset + sizeof(void*))
            {
                unsigned int score =
                    candidates[lowerIndex].references + candidates[upperIndex].references +
                    6 * (candidates[lowerIndex].zeroTests + candidates[upperIndex].zeroTests) +
                    4 * (candidates[lowerIndex].writes + candidates[upperIndex].writes);
                if (score > bestScore)
                {
                    bestScore = score;
                    bestPairCount = 1;
                    bestLowerOffset = candidates[lowerIndex].offset;
                    bestUpperOffset = candidates[upperIndex].offset;
                }
                else if (score == bestScore)
                {
                    bestPairCount++;
                }
            }
        }
    }
    if (bestPairCount != 1 || bestScore < 2)
    {
        Wh_Log(L"DWM offset scan: candidates=%u bestScore=%u ties=%u", candidateCount, bestScore,
               bestPairCount);
        return false;
    }
    *topLevelWindowOffset = bestLowerOffset;
    *topLevelWindow3DOffset = bestUpperOffset;
    return true;
}

static void ClearDwmWindowMappingByWindowData(void* windowData)
{
    if (!windowData)
    {
        return;
    }
    AcquireSRWLockExclusive(&g_dwmWindowMappingsLock);
    for (int i = 0; i < MAX_DWM_WINDOW_MAPPINGS; i++)
    {
        if (g_dwmWindowMappings[i].windowData == windowData)
        {
            g_dwmWindowMappings[i] = {};
        }
    }
    ReleaseSRWLockExclusive(&g_dwmWindowMappingsLock);
}

static void ClearAllDwmWindowMappings()
{
    AcquireSRWLockExclusive(&g_dwmWindowMappingsLock);
    for (DwmWindowObjectMapping& mapping : g_dwmWindowMappings)
    {
        mapping = {};
    }
    ReleaseSRWLockExclusive(&g_dwmWindowMappingsLock);
}

static void* ClearDwmWindowMappingByTopLevelWindow(void* topLevelWindow)
{
    if (!topLevelWindow)
    {
        return nullptr;
    }
    void* windowData = nullptr;
    AcquireSRWLockExclusive(&g_dwmWindowMappingsLock);
    for (DwmWindowObjectMapping& mapping : g_dwmWindowMappings)
    {
        if (mapping.topLevelWindow == topLevelWindow)
        {
            windowData = mapping.windowData;
            mapping.topLevelWindow = nullptr;
            mapping.lastSeen = GetTickCount64();
        }
    }
    ReleaseSRWLockExclusive(&g_dwmWindowMappingsLock);
    return windowData;
}

static void ClearDwmWindowMappingByTopLevelWindow3D(void* topLevelWindow3D)
{
    if (!topLevelWindow3D)
    {
        return;
    }
    AcquireSRWLockExclusive(&g_dwmWindowMappingsLock);
    for (int i = 0; i < MAX_DWM_WINDOW_MAPPINGS; i++)
    {
        DwmWindowObjectMapping& mapping = g_dwmWindowMappings[i];
        if (mapping.topLevelWindow3D == topLevelWindow3D)
        {
            mapping.topLevelWindow3D = nullptr;
            if (!mapping.topLevelWindow)
            {
                mapping = {};
            }
        }
    }
    ReleaseSRWLockExclusive(&g_dwmWindowMappingsLock);
}

static void RegisterDwmWindowMapping(void* windowData, void* topLevelWindow, void* topLevelWindow3D)
{
    if (!windowData || (!topLevelWindow && !topLevelWindow3D))
    {
        return;
    }
    ULONGLONG now = GetTickCount64();
    AcquireSRWLockExclusive(&g_dwmWindowMappingsLock);
    int targetIndex = -1;
    int oldestIndex = 0;
    ULONGLONG oldestSeen = ULLONG_MAX;
    for (int i = 0; i < MAX_DWM_WINDOW_MAPPINGS; i++)
    {
        DwmWindowObjectMapping& mapping = g_dwmWindowMappings[i];
        if ((topLevelWindow && mapping.windowData != windowData &&
             mapping.topLevelWindow == topLevelWindow) ||
            (topLevelWindow3D && mapping.windowData != windowData &&
             mapping.topLevelWindow3D == topLevelWindow3D))
        {
            mapping = {};
        }
        if (mapping.windowData == windowData)
        {
            targetIndex = i;
        }
        if (!mapping.windowData && targetIndex < 0)
        {
            targetIndex = i;
        }
        if (mapping.lastSeen < oldestSeen)
        {
            oldestSeen = mapping.lastSeen;
            oldestIndex = i;
        }
    }
    if (targetIndex < 0)
    {
        targetIndex = oldestIndex;
        g_dwmWindowMappings[targetIndex] = {};
    }
    for (int i = 0; i < MAX_DWM_WINDOW_MAPPINGS; i++)
    {
        if (i != targetIndex && g_dwmWindowMappings[i].windowData == windowData)
        {
            g_dwmWindowMappings[i] = {};
        }
    }
    DwmWindowObjectMapping& mapping = g_dwmWindowMappings[targetIndex];
    mapping.windowData = windowData;
    if (topLevelWindow)
    {
        mapping.topLevelWindow = topLevelWindow;
    }
    if (topLevelWindow3D)
    {
        mapping.topLevelWindow3D = topLevelWindow3D;
    }
    mapping.lastSeen = now;
    ReleaseSRWLockExclusive(&g_dwmWindowMappingsLock);
}

static void MarkAnimationSlotForDwmObjectRefresh(void* windowData)
{
    HWND hwnd = GetHwndFromWindowData(windowData);
    if (!hwnd)
    {
        return;
    }
    bool refreshNeeded = false;
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        WindowAnimationSlot& slot = g_animationSlots[i];
        if (slot.active && slot.hwnd == hwnd)
        {
            slot.transformRebindRevision++;
            refreshNeeded = true;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (refreshNeeded)
    {
        RequestDwmScenePass(hwnd);
    }
}

static bool IsTopLevelWindowForWindowData(void* candidate, void* windowData)
{
    if (!candidate || !windowData)
    {
        return false;
    }
    if (g_topLevelWindowWindowDataOffset != SIZE_MAX)
    {
        BYTE* backPointer =
            static_cast<BYTE*>(candidate) + g_topLevelWindowWindowDataOffset;
        if (!IsReadableMemory(backPointer, sizeof(void*)) ||
            *reinterpret_cast<void**>(backPointer) != windowData)
        {
            return false;
        }
    }
    return IsDwmObjectPointerValid(candidate, g_topLevelWindowVtable);
}

static bool ResolveDwmWindowObjects(void* windowData, void** topLevelWindow,
                                    void** topLevelWindow3D)
{
    *topLevelWindow = nullptr;
    *topLevelWindow3D = nullptr;
    if (!windowData)
    {
        return false;
    }
    // Constructor/SetWindowData hooks identify the visual that DWM published.
    // Offset reads are only a fallback while that mapping is being rebuilt.
    AcquireSRWLockShared(&g_dwmWindowMappingsLock);
    for (int i = 0; i < MAX_DWM_WINDOW_MAPPINGS; i++)
    {
        const DwmWindowObjectMapping& mapping = g_dwmWindowMappings[i];
        if (mapping.windowData == windowData)
        {
            *topLevelWindow = mapping.topLevelWindow;
            *topLevelWindow3D = mapping.topLevelWindow3D;
            break;
        }
    }
    ReleaseSRWLockShared(&g_dwmWindowMappingsLock);
    if (!IsTopLevelWindowForWindowData(*topLevelWindow, windowData))
    {
        *topLevelWindow = nullptr;
    }
    if (!IsDwmObjectPointerValid(*topLevelWindow3D, g_topLevelWindow3DVtable))
    {
        *topLevelWindow3D = nullptr;
    }
    if (!*topLevelWindow && g_windowDataTopLevelWindowOffset != SIZE_MAX)
    {
        BYTE* fieldAddress = static_cast<BYTE*>(windowData) + g_windowDataTopLevelWindowOffset;
        if (IsReadableMemory(fieldAddress, sizeof(void*)))
        {
            void* candidate = *reinterpret_cast<void**>(fieldAddress);
            if (IsTopLevelWindowForWindowData(candidate, windowData))
            {
                *topLevelWindow = candidate;
            }
        }
    }
    if (!*topLevelWindow3D &&
        g_topLevelWindow3DVtable.load(std::memory_order_acquire) &&
        g_windowDataTopLevelWindow3DOffset != SIZE_MAX)
    {
        BYTE* fieldAddress = static_cast<BYTE*>(windowData) + g_windowDataTopLevelWindow3DOffset;
        if (IsReadableMemory(fieldAddress, sizeof(void*)))
        {
            void* candidate = *reinterpret_cast<void**>(fieldAddress);
            if (IsDwmObjectPointerValid(candidate, g_topLevelWindow3DVtable))
            {
                *topLevelWindow3D = candidate;
            }
        }
    }
    if (*topLevelWindow || *topLevelWindow3D)
    {
        RegisterDwmWindowMapping(windowData, *topLevelWindow, *topLevelWindow3D);
    }
    return *topLevelWindow != nullptr;
}

static void* FindWindowDataForTopLevelWindow3D(void* topLevelWindow3D)
{
    if (!IsDwmObjectPointerValid(topLevelWindow3D, g_topLevelWindow3DVtable))
    {
        return nullptr;
    }
    void* windowData = nullptr;
    AcquireSRWLockShared(&g_dwmWindowMappingsLock);
    for (int i = 0; i < MAX_DWM_WINDOW_MAPPINGS; i++)
    {
        if (g_dwmWindowMappings[i].topLevelWindow3D == topLevelWindow3D)
        {
            windowData = g_dwmWindowMappings[i].windowData;
            break;
        }
    }
    ReleaseSRWLockShared(&g_dwmWindowMappingsLock);
    return windowData;
}

static void* __cdecl TopLevelWindowConstructorHook(void* pThis, void* windowData, bool unknown)
{
    void* result = g_topLevelWindowConstructorOriginal(pThis, windowData, unknown);
    void* constructedObject = result ? result : pThis;
    if (windowData && IsDwmObjectPointerValid(constructedObject, g_topLevelWindowVtable))
    {
        RegisterDwmWindowMapping(windowData, constructedObject, nullptr);
        MarkAnimationSlotForDwmObjectRefresh(windowData);
    }
    return result;
}

static void __cdecl TopLevelWindow3DSetWindowDataHook(void* pThis, void* windowData)
{
    g_topLevelWindow3DSetWindowDataOriginal(pThis, windowData);
    ClearDwmWindowMappingByTopLevelWindow3D(pThis);
    if (windowData &&
        (IsDwmObjectPointerValid(pThis, g_topLevelWindow3DVtable) ||
         LearnDwmVtableFromTrustedObject(pThis, g_topLevelWindow3DVtable)))
    {
        RegisterDwmWindowMapping(windowData, nullptr, pThis);
        MarkAnimationSlotForDwmObjectRefresh(windowData);
    }
}

static void __cdecl WindowDataDestructorHook(void* pThis)
{
    ClearDwmWindowMappingByWindowData(pThis);
    g_windowDataDestructorOriginal(pThis);
}

static long __cdecl EnsureTopLevelWindowHook(void* pThis, void* windowData)
{
    long result = g_ensureTopLevelWindowFunction(pThis, windowData);
    if (result >= 0 && windowData &&
        HasExactDwmVtableTrusted(pThis, g_windowListVtable))
    {
        g_windowListForSceneWake.store(pThis, std::memory_order_release);
        void* topLevelWindow = nullptr;
        void* topLevelWindow3D = nullptr;
        if (ResolveDwmWindowObjects(windowData, &topLevelWindow, &topLevelWindow3D))
        {
            MarkAnimationSlotForDwmObjectRefresh(windowData);
        }
    }
    return result;
}

static void __cdecl TopLevelWindowDestructorHook(void* pThis)
{
    void* windowData = ClearDwmWindowMappingByTopLevelWindow(pThis);
    if (windowData && !g_unloading.load(std::memory_order_acquire))
    {
        MarkAnimationSlotForDwmObjectRefresh(windowData);
    }
    g_topLevelWindowDestructorOriginal(pThis);
}

static void __cdecl TopLevelWindow3DDestructorHook(void* pThis)
{
    ClearDwmWindowMappingByTopLevelWindow3D(pThis);
    g_topLevelWindow3DDestructorOriginal(pThis);
}

static void DropAnimationSlotsForSceneChange()
{
    unsigned int droppedSlots = 0;
    unsigned int retainedProxies = 0;
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    for (WindowAnimationSlot& slot : g_animationSlots)
    {
        if (!slot.active && !slot.retiring)
        {
            continue;
        }
        retainedProxies += slot.matrixTransformProxy != nullptr;
        ULONGLONG nextGeneration = slot.generation + 1;
        slot = {};
        slot.generation = nextGeneration;
        droppedSlots++;
    }
    WakeAllConditionVariable(&g_animationSlotsCondition);
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (droppedSlots)
    {
        g_abandonedProxyCount.fetch_add(retainedProxies, std::memory_order_acq_rel);
        g_sceneRecoveryCleanupPending.store(false, std::memory_order_release);
        g_sceneOwnershipResetPending.store(true, std::memory_order_release);
        Wh_Log(L"DWM scene owner changed: droppedSlots=%u retainedOldProxies=%u",
               droppedSlots, retainedProxies);
    }
}

static bool RegisterDwmSceneThread(bool authoritative)
{
    DWORD currentThreadId = GetCurrentThreadId();
    DWORD ownerThreadId = g_dwmSceneThreadId.load(std::memory_order_acquire);
    if (ownerThreadId == 0 && !authoritative)
    {
        // AdvanceTimelines authoritatively identifies uDWM's scene thread.
        return false;
    }
    if (ownerThreadId == 0 &&
        g_dwmSceneThreadId.compare_exchange_strong(ownerThreadId, currentThreadId,
                                                   std::memory_order_acq_rel,
                                                   std::memory_order_acquire))
    {
        return true;
    }
    if (ownerThreadId == currentThreadId)
    {
        return true;
    }
    if (authoritative)
    {
        bool previousOwnerStopped = false;
        HANDLE previousThread = OpenThread(SYNCHRONIZE, FALSE, ownerThreadId);
        if (previousThread)
        {
            previousOwnerStopped = WaitForSingleObject(previousThread, 0) == WAIT_OBJECT_0;
            CloseHandle(previousThread);
        }
        else
        {
            previousOwnerStopped = GetLastError() == ERROR_INVALID_PARAMETER;
        }
        if (previousOwnerStopped &&
            g_dwmSceneThreadId.compare_exchange_strong(ownerThreadId, currentThreadId,
                                                       std::memory_order_acq_rel,
                                                       std::memory_order_acquire))
        {
            DropAnimationSlotsForSceneChange();
            ClearAllDwmWindowMappings();
            g_dwmThreadMismatchLogged.store(false, std::memory_order_release);
            g_desktopManager.store(nullptr, std::memory_order_release);
            g_dwmCompositor.store(nullptr, std::memory_order_release);
            g_windowListForSceneWake.store(nullptr, std::memory_order_release);
            g_dwmObjectDiscoveryFailureCount.store(0, std::memory_order_release);
            g_sceneSubmittedSerial.store(
                g_sceneRequestedSerial.load(std::memory_order_acquire),
                std::memory_order_release);
            Wh_Log(L"DWM scene thread changed; ownership re-armed");
            return true;
        }
    }
    if (!g_dwmThreadMismatchLogged.exchange(true, std::memory_order_acq_rel))
    {
        Wh_Log(L"DWM safety gate: ignoring a scene callback from a "
               L"non-owner thread");
    }
    return false;
}

static bool IsOnDwmSceneThread()
{
    DWORD sceneThreadId = g_dwmSceneThreadId.load(std::memory_order_acquire);
    return sceneThreadId != 0 && sceneThreadId == GetCurrentThreadId();
}

static void RearmAnimationSlotsForCompositorChange()
{
    unsigned int abandoned = 0;
    bool hasActiveSlots = false;
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    for (WindowAnimationSlot& slot : g_animationSlots)
    {
        if (!slot.active && !slot.retiring)
        {
            continue;
        }
        if (slot.matrixTransformProxy)
        {
            slot.matrixTransformProxy = nullptr;
            abandoned++;
        }
        slot.boundTopLevelVisualProxy = nullptr;
        slot.boundTransitionVisualProxy = nullptr;
        slot.transformAttached = false;
        slot.transitionTransformAttached = false;
        slot.identityApplied = false;
        slot.transformRebindRevision++;
        slot.proxyCreationPending = slot.active;
        hasActiveSlots = hasActiveSlots || slot.active;
    }
    WakeAllConditionVariable(&g_animationSlotsCondition);
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (abandoned)
    {
        g_abandonedProxyCount.fetch_add(abandoned, std::memory_order_acq_rel);
    }
    if (hasActiveSlots)
    {
        g_sceneRequestedSerial.fetch_add(1, std::memory_order_acq_rel);
    }
}

static bool CacheDwmObjectsFromDesktopManager(void* manager)
{
    void* expectedManagerVtable = g_desktopManagerVtable.load(std::memory_order_acquire);
    if (g_desktopManagerCompositorOffset == SIZE_MAX || !manager ||
        !expectedManagerVtable || *reinterpret_cast<void**>(manager) != expectedManagerVtable)
    {
        return false;
    }
    BYTE* compositorField =
        static_cast<BYTE*>(manager) + g_desktopManagerCompositorOffset;
    void* compositor = *reinterpret_cast<void**>(compositorField);
    void* cachedManager = g_desktopManager.load(std::memory_order_acquire);
    void* cachedCompositor = g_dwmCompositor.load(std::memory_order_acquire);
    if (cachedManager == manager && cachedCompositor == compositor && cachedCompositor)
    {
        return true;
    }
    if (!IsDwmObjectPointerValid(compositor, g_compositorVtable) &&
        !LearnDwmVtableFromTrustedObject(compositor, g_compositorVtable))
    {
        if (g_dwmCompositor.exchange(nullptr, std::memory_order_acq_rel))
        {
            RearmAnimationSlotsForCompositorChange();
        }
        g_desktopManager.store(manager, std::memory_order_release);
        return false;
    }
    if (cachedCompositor && cachedCompositor != compositor)
    {
        RearmAnimationSlotsForCompositorChange();
    }
    g_desktopManager.store(manager, std::memory_order_release);
    g_dwmCompositor.store(compositor, std::memory_order_release);
    Wh_Log(L"DWM objects verified: CDesktopManager=%p CCompositor=%p offset=0x%zx",
           manager, compositor, g_desktopManagerCompositorOffset);
    return true;
}

static void* FindDwmCompositor()
{
    g_desktopManager.store(nullptr, std::memory_order_release);
    if (!IsDwmImageAddress(g_desktopManagerInstanceAddress, sizeof(void*)) ||
        !IsReadableMemory(g_desktopManagerInstanceAddress, sizeof(void*)))
    {
        Wh_Log(L"DWM startup discovery: desktop manager singleton unavailable");
        return nullptr;
    }
    void* desktopManager = *static_cast<void**>(g_desktopManagerInstanceAddress);
    if (!IsReadableMemory(desktopManager, sizeof(void*)))
    {
        Wh_Log(L"DWM startup discovery: desktop manager object unavailable");
        return nullptr;
    }
    if (!CacheDwmObjectsFromDesktopManager(desktopManager))
    {
        Wh_Log(L"DWM startup discovery: singleton object validation failed");
        return nullptr;
    }
    return g_dwmCompositor.load(std::memory_order_acquire);
}

static bool InitializeDwmHooks()
{
    HMODULE udwm = GetModuleHandleW(L"udwm.dll");
    if (!udwm)
    {
        Wh_Log(L"DWM hooks: udwm.dll not loaded");
        return false;
    }
    if (!InitializeDwmModuleLayout(udwm))
    {
        Wh_Log(L"DWM safety gate: invalid udwm.dll PE layout");
        return false;
    }
    // Windhawk resolves demangled names; decorated ABI names are exact fallbacks.
    // Every candidate is optional here and the selected core is validated below.
    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {{L"public: bool __cdecl CWindowData::IsGhostWindow(struct HWND__ * *)const ",
          L"?IsGhostWindow@CWindowData@@QEBA_NPEAPEAUHWND__@@@Z"},
         reinterpret_cast<void**>(&g_isGhostWindowOriginal),
         nullptr,
         true},
        {{L"public: void __cdecl CWindowList::OnPositionChange(class CWindowData *,bool)",
          L"?OnPositionChange@CWindowList@@QEAAXPEAVCWindowData@@_N@Z"},
         reinterpret_cast<void**>(&g_onPositionChangeOriginal),
         reinterpret_cast<void*>(OnPositionChangeHook),
         true},
        {{L"public: class CWindowData * __cdecl "
           L"CWindowList::FindWindowDataByHwnd(struct HWND__ *)",
          L"?FindWindowDataByHwnd@CWindowList@@QEAAPEAVCWindowData@@PEAUHWND__@@@Z"},
         reinterpret_cast<void**>(&g_findWindowDataByHwnd),
         nullptr,
         true},
        {{L"public: long __cdecl CWindowList::GetSyncedWindowData("
           L"struct IDwmWindow *,bool,class CWindowData * *)",
          L"?GetSyncedWindowData@CWindowList@@"
           L"QEAAJPEAUIDwmWindow@@_NPEAPEAVCWindowData@@@Z"},
         reinterpret_cast<void**>(&g_getSyncedWindowDataLong),
         nullptr,
         true},
        {{L"public: void __cdecl CWindowList::GetSyncedWindowData("
           L"struct IDwmWindow *,bool,class CWindowData * *)",
          L"?GetSyncedWindowData@CWindowList@@"
           L"QEAAXPEAUIDwmWindow@@_NPEAPEAVCWindowData@@@Z"},
         reinterpret_cast<void**>(&g_getSyncedWindowDataVoid),
         nullptr,
         true},
        {{L"public: virtual long __cdecl CWindowList::WindowTransitionChange("
           L"struct IDwmWindow *,enum DWMTRANSITION_TARGET,struct tagRECT const &,"
           L"struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,"
           L"struct tagRECT const &)",
          L"?WindowTransitionChange@CWindowList@@"
           L"UEAAJPEAUIDwmWindow@@W4DWMTRANSITION_TARGET@@"
           L"AEBUtagRECT@@2222@Z"},
         reinterpret_cast<void**>(&g_windowTransitionChangeOriginal),
         reinterpret_cast<void*>(WindowTransitionChangeHook),
         true},
        {{L"private: void __cdecl CWindowList::CheckForMaximizedChange(class CWindowData *)",
          L"?CheckForMaximizedChange@CWindowList@@AEAAXPEAVCWindowData@@@Z"},
         reinterpret_cast<void**>(&g_checkForMaximizedChangeOriginal),
         reinterpret_cast<void*>(CheckForMaximizedChangeHook),
         true},
        {{L"public: long __cdecl CTopLevelWindow3D::"
           L"StartAnimationForMaximizeSnapTransition("
           L"enum CTopLevelWindow3D::WindowAnimationType,struct tagRECT const &)",
          L"?StartAnimationForMaximizeSnapTransition@"
           L"CTopLevelWindow3D@@QEAAJW4WindowAnimationType@1@"
           L"AEBUtagRECT@@@Z"},
         reinterpret_cast<void**>(&g_startAnimationForMaximizeSnapTransitionOriginal),
         reinterpret_cast<void*>(StartAnimationForMaximizeSnapTransitionHook),
         true},
        {{L"public: class CVisualProxy * __cdecl "
           L"CTopLevelWindow::GetCanvasRootVisualProxy(void)",
          L"?GetCanvasRootVisualProxy@CTopLevelWindow@@"
           L"QEAAPEAVCVisualProxy@@XZ"},
         reinterpret_cast<void**>(&g_getCanvasRootVisualProxy),
         nullptr,
         true},
        {{L"public: class CVisual * __cdecl "
           L"CTopLevelWindow::GetRootVisualNoAddRef(enum TLWRootVisualType)",
          L"?GetRootVisualNoAddRef@CTopLevelWindow@@"
           L"QEAAPEAVCVisual@@W4TLWRootVisualType@@@Z"},
         reinterpret_cast<void**>(&g_topLevelWindowGetRootVisual),
         nullptr,
         true},
        {{L"public: class CWindowData * __cdecl "
           L"CTopLevelWindow::GetWindowData(void)const",
          L"public: class CWindowData * __cdecl "
           L"CTopLevelWindow::GetWindowData(void)const ",
          L"?GetWindowData@CTopLevelWindow@@QEBAPEAVCWindowData@@XZ"},
         reinterpret_cast<void**>(&g_topLevelWindowGetWindowData),
         nullptr,
         true},
        {{L"public: long __cdecl CMatrixTransformProxy::Update("
           L"struct _MilMatrix3x2D const &)",
          L"?Update@CMatrixTransformProxy@@"
           L"QEAAJAEBU_MilMatrix3x2D@@@Z"},
         reinterpret_cast<void**>(&g_cMatrixTransformProxyUpdate),
         nullptr,
         true},
        {{L"public: long __cdecl CMatrixTransformProxy::Update("
           L"struct D2D_MATRIX_3X2_F const &)",
          L"?Update@CMatrixTransformProxy@@"
           L"QEAAJAEBUD2D_MATRIX_3X2_F@@@Z"},
         reinterpret_cast<void**>(&g_cMatrixTransformProxyUpdateFloat),
         nullptr,
         true},
        {{L"public: long __cdecl CVisualProxy::SetTransform("
           L"class CBaseTransformProxy *)",
          L"?SetTransform@CVisualProxy@@QEAAJPEAVCBaseTransformProxy@@@Z"},
         reinterpret_cast<void**>(&g_cVisualProxySetTransform),
         nullptr,
         true},
        {{L"protected: long __cdecl CCompositor::CreateProxy<"
           L"class CMatrixTransformProxy>(class CMatrixTransformProxy * *)",
          L"??$CreateProxy@VCMatrixTransformProxy@@@CCompositor@@"
           L"IEAAJPEAPEAVCMatrixTransformProxy@@@Z"},
         reinterpret_cast<void**>(&g_createMatrixTransformProxy),
         nullptr,
         true},
        {{L"public: unsigned long __cdecl CBaseObject::Release(void)",
          L"?Release@CBaseObject@@QEAAKXZ"},
         reinterpret_cast<void**>(&g_cBaseObjectRelease),
         nullptr,
         true},
        {{L"private: static class CDesktopManager * "
           L"CDesktopManager::s_pDesktopManagerInstance",
          L"?s_pDesktopManagerInstance@CDesktopManager@@0PEAV1@EA"},
         &g_desktopManagerInstanceAddress,
         nullptr,
         true},
        {{L"private: long __cdecl CDesktopManager::Initialize(struct IUnknown *)",
          L"?Initialize@CDesktopManager@@AEAAJPEAUIUnknown@@@Z"},
         &g_desktopManagerInitializeFunction,
         nullptr,
         true},
        {{L"public: static long __cdecl CCompositor::Create(class CCompositor * *)",
          L"?Create@CCompositor@@SAJPEAPEAV1@@Z"},
         &g_cCompositorCreateFunction,
         nullptr,
         true},
        {{L"private: static void __cdecl "
           L"CDesktopManager::HandleThreadMessage(unsigned int,unsigned __int64,__int64)",
          L"?HandleThreadMessage@CDesktopManager@@CAXI_K_J@Z"},
         reinterpret_cast<void**>(&g_desktopManagerHandleThreadMessageOriginal),
         reinterpret_cast<void*>(DesktopManagerHandleThreadMessageHook),
         true},
        {{L"public: long __cdecl CDesktopManager::PostStartAnimations(void)",
           L"?PostStartAnimations@CDesktopManager@@QEAAJXZ"},
         reinterpret_cast<void**>(&g_desktopManagerPostStartAnimations),
         nullptr,
         true},
        {{L"private: __cdecl CTopLevelWindow::CTopLevelWindow(class CWindowData *,bool)",
           L"??0CTopLevelWindow@@AEAA@PEAVCWindowData@@_N@Z"},
         reinterpret_cast<void**>(&g_topLevelWindowConstructorFunction),
         nullptr,
         true},
        {{L"protected: virtual __cdecl CTopLevelWindow::~CTopLevelWindow(void)",
          L"public: virtual __cdecl CTopLevelWindow::~CTopLevelWindow(void)",
          L"private: virtual __cdecl CTopLevelWindow::~CTopLevelWindow(void)",
          L"??1CTopLevelWindow@@EEAA@XZ"},
         reinterpret_cast<void**>(&g_topLevelWindowDestructorOriginal),
         reinterpret_cast<void*>(TopLevelWindowDestructorHook),
         true},
        {{L"public: void __cdecl CTopLevelWindow3D::SetWindowData(class CWindowData *)",
          L"?SetWindowData@CTopLevelWindow3D@@"
           L"QEAAXPEAVCWindowData@@@Z"},
         reinterpret_cast<void**>(&g_topLevelWindow3DSetWindowDataOriginal),
         reinterpret_cast<void*>(TopLevelWindow3DSetWindowDataHook),
         true},
        {{L"public: __cdecl CWindowData::~CWindowData(void)",
          L"??1CWindowData@@QEAA@XZ"},
         reinterpret_cast<void**>(&g_windowDataDestructorOriginal),
         reinterpret_cast<void*>(WindowDataDestructorHook),
         true},
        {{L"private: long __cdecl CWindowList::EnsureTopLevelWindow(class CWindowData *)",
          L"?EnsureTopLevelWindow@CWindowList@@"
           L"AEAAJPEAVCWindowData@@@Z"},
         reinterpret_cast<void**>(&g_ensureTopLevelWindowFunction),
         reinterpret_cast<void*>(EnsureTopLevelWindowHook),
         true},
        {{L"protected: virtual __cdecl CTopLevelWindow3D::~CTopLevelWindow3D(void)",
          L"public: virtual __cdecl CTopLevelWindow3D::~CTopLevelWindow3D(void)",
          L"??1CTopLevelWindow3D@@MEAA@XZ"},
         reinterpret_cast<void**>(&g_topLevelWindow3DDestructorOriginal),
         reinterpret_cast<void*>(TopLevelWindow3DDestructorHook),
         true},
        {{L"public: long __cdecl CWindowList::ForceUpdateScene(void)",
          L"?ForceUpdateScene@CWindowList@@QEAAJXZ"},
         reinterpret_cast<void**>(&g_windowListForceUpdateSceneOriginal),
         reinterpret_cast<void*>(ForceUpdateSceneHook),
         true},
        {{L"public: virtual long __cdecl CWindowList::UpdateScene(void)",
          L"?UpdateScene@CWindowList@@UEAAJXZ"},
         reinterpret_cast<void**>(&g_windowListUpdateSceneOriginal),
         reinterpret_cast<void*>(UpdateSceneHook),
         true},
        {{L"private: void __cdecl CDesktopManager::AdvanceTimelines(double)",
          L"?AdvanceTimelines@CDesktopManager@@AEAAXN@Z"},
         reinterpret_cast<void**>(&g_desktopManagerAdvanceTimelinesOriginal),
         reinterpret_cast<void*>(AdvanceTimelinesHook),
         true},
        {{L"const CTopLevelWindow::`vftable'", L"??_7CTopLevelWindow@@6B@"},
         &g_topLevelWindowVtableSymbol,
         nullptr,
         true},
        {{L"??_7CTopLevelWindow3D@@6BCRenderDataVisual@@@"},
         &g_topLevelWindow3DVtableSymbol,
         nullptr,
         true},
        {{L"const CDesktopManager::`vftable'", L"??_7CDesktopManager@@6B@"},
         &g_desktopManagerVtableSymbol,
         nullptr,
         true},
        {{L"const CWindowList::`vftable'", L"??_7CWindowList@@6B@"},
         &g_windowListVtableSymbol,
         nullptr,
         true},
        {{L"const CVisualProxy::`vftable'", L"??_7CVisualProxy@@6B@"},
         &g_visualProxyVtableSymbol,
         nullptr,
         true},
        {{L"const CMatrixTransformProxy::`vftable'", L"??_7CMatrixTransformProxy@@6B@"},
         &g_matrixTransformProxyVtableSymbol,
         nullptr,
         true}};
    if (!WindhawkUtils::HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks)))
    {
        Wh_Log(L"DWM hooks: symbol resolver failed");
        return false;
    }
    auto keepValid = [](auto& function)
    {
        if (function && !IsDwmFunctionPointerValid(reinterpret_cast<void*>(function)))
        {
            function = nullptr;
        }
    };
    keepValid(g_findWindowDataByHwnd);
    keepValid(g_getSyncedWindowDataLong);
    keepValid(g_getSyncedWindowDataVoid);
    keepValid(g_cMatrixTransformProxyUpdate);
    keepValid(g_cMatrixTransformProxyUpdateFloat);
    keepValid(g_getCanvasRootVisualProxy);
    keepValid(g_topLevelWindowGetRootVisual);
    keepValid(g_topLevelWindowGetWindowData);
    keepValid(g_desktopManagerPostStartAnimations);
    auto cacheVtableSymbol = [](void* symbol, std::atomic<void*>& target)
    {
        if (!IsDwmImageAddress(symbol, sizeof(void*) * 3))
        {
            return false;
        }
        void** functions = static_cast<void**>(symbol);
        for (int i = 0; i < 3; i++)
        {
            if (!IsDwmFunctionPointerValid(functions[i]))
            {
                return false;
            }
        }
        target.store(symbol, std::memory_order_release);
        return true;
    };
    bool hasExactDesktopManagerVtable =
        cacheVtableSymbol(g_desktopManagerVtableSymbol, g_desktopManagerVtable);
    bool hasExactWindowListVtable =
        cacheVtableSymbol(g_windowListVtableSymbol, g_windowListVtable);
    bool hasExactTopLevelWindowVtable =
        cacheVtableSymbol(g_topLevelWindowVtableSymbol, g_topLevelWindowVtable);
    bool hasExactTopLevelWindow3DVtable =
        cacheVtableSymbol(g_topLevelWindow3DVtableSymbol, g_topLevelWindow3DVtable);
    bool hasExactVisualProxyVtable =
        cacheVtableSymbol(g_visualProxyVtableSymbol, g_visualProxyVtable);
    bool hasExactMatrixProxyVtable =
        cacheVtableSymbol(g_matrixTransformProxyVtableSymbol, g_matrixTransformProxyVtable);
    if (g_cMatrixTransformProxyUpdate && g_cMatrixTransformProxyUpdateFloat)
    {
        Wh_Log(L"DWM compatibility: ambiguous ABI variants");
        return false;
    }
    if (g_getSyncedWindowDataLong && g_getSyncedWindowDataVoid)
    {
        Wh_Log(L"DWM compatibility: disabling ambiguous transition lookup ABI");
        g_getSyncedWindowDataLong = nullptr;
        g_getSyncedWindowDataVoid = nullptr;
    }
    Wh_Log(L"Native maximize/restore hooks: request=%s animation=%s",
           g_windowTransitionChangeOriginal && HasSyncedWindowData() ? L"available"
                                                                     : L"unavailable",
           g_startAnimationForMaximizeSnapTransitionOriginal
                ? L"available"
                : L"unavailable; using finalized-state fallback");
    Wh_Log(L"Native transition visual: %s",
           hasExactTopLevelWindow3DVtable ? L"verified" : L"deferred to trusted hook");
    struct RequiredDwmFunction
    {
        const wchar_t* name;
        void* address;
    };
    RequiredDwmFunction requiredFunctions[] = {
        {L"CVisualProxy::SetTransform", reinterpret_cast<void*>(g_cVisualProxySetTransform)},
        {L"CCompositor::CreateProxy<CMatrixTransformProxy>",
         reinterpret_cast<void*>(g_createMatrixTransformProxy)},
        {L"CBaseObject::Release", reinterpret_cast<void*>(g_cBaseObjectRelease)},
        {L"CWindowList::FindWindowDataByHwnd",
         reinterpret_cast<void*>(g_findWindowDataByHwnd)},
        {L"CDesktopManager::Initialize", g_desktopManagerInitializeFunction},
        {L"CCompositor::Create", g_cCompositorCreateFunction},
        {L"CDesktopManager::HandleThreadMessage",
         reinterpret_cast<void*>(g_desktopManagerHandleThreadMessageOriginal)},
        {L"CDesktopManager::PostStartAnimations",
         reinterpret_cast<void*>(g_desktopManagerPostStartAnimations)},
        {L"CDesktopManager::AdvanceTimelines",
         reinterpret_cast<void*>(g_desktopManagerAdvanceTimelinesOriginal)},
        {L"CTopLevelWindow::GetCanvasRootVisualProxy",
         reinterpret_cast<void*>(g_getCanvasRootVisualProxy)},
        {L"CTopLevelWindow::GetRootVisualNoAddRef",
         reinterpret_cast<void*>(g_topLevelWindowGetRootVisual)},
        {L"CTopLevelWindow::CTopLevelWindow",
         reinterpret_cast<void*>(g_topLevelWindowConstructorFunction)},
        {L"CTopLevelWindow::~CTopLevelWindow",
         reinterpret_cast<void*>(g_topLevelWindowDestructorOriginal)},
        {L"CWindowList::EnsureTopLevelWindow",
         reinterpret_cast<void*>(g_ensureTopLevelWindowFunction)}};
    bool missingCoreFunction = false;
    for (const RequiredDwmFunction& function : requiredFunctions)
    {
        if (!IsDwmFunctionPointerValid(function.address))
        {
            Wh_Log(L"DWM compatibility: missing core symbol: %s", function.name);
            missingCoreFunction = true;
        }
    }
    struct RequiredDwmVtable
    {
        const wchar_t* name;
        bool available;
    };
    RequiredDwmVtable requiredVtables[] = {
        {L"CDesktopManager", hasExactDesktopManagerVtable},
        {L"CWindowList", hasExactWindowListVtable},
        {L"CTopLevelWindow", hasExactTopLevelWindowVtable},
        {L"CVisualProxy", hasExactVisualProxyVtable},
        {L"CMatrixTransformProxy", hasExactMatrixProxyVtable}};
    for (const RequiredDwmVtable& vtable : requiredVtables)
    {
        if (!vtable.available)
        {
            Wh_Log(L"DWM compatibility: missing core vftable: %s", vtable.name);
            missingCoreFunction = true;
        }
    }
    if (missingCoreFunction)
    {
        return false;
    }
    Wh_Log(L"DWM compatibility: compositor vftable is verified from its exact member");
    g_visualProxyOffset = FindVisualProxyOffsetFromCanvasGetter(
        reinterpret_cast<void*>(g_getCanvasRootVisualProxy));
    if (g_visualProxyOffset == SIZE_MAX)
    {
        Wh_Log(L"DWM compatibility: full-window root visual path could not be derived");
        return false;
    }
    g_desktopManagerCompositorOffset = FindDesktopManagerCompositorOffset(
        g_desktopManagerInitializeFunction, g_cCompositorCreateFunction);
    if (g_desktopManagerCompositorOffset == SIZE_MAX)
    {
        Wh_Log(L"DWM compatibility: compositor member could not be proven from PDB functions");
        return false;
    }
    g_desktopManagerThreadIdOffset = FindDesktopManagerThreadIdOffset(
        reinterpret_cast<void*>(g_desktopManagerPostStartAnimations));
    if (g_desktopManagerThreadIdOffset == SIZE_MAX)
    {
        Wh_Log(L"DWM compatibility: message-thread member could not be derived");
        return false;
    }
    bool hasMatrixUpdate =
        IsDwmFunctionPointerValid(reinterpret_cast<void*>(g_cMatrixTransformProxyUpdate)) ||
        IsDwmFunctionPointerValid(
            reinterpret_cast<void*>(g_cMatrixTransformProxyUpdateFloat));
    if (!hasMatrixUpdate)
    {
        Wh_Log(L"DWM compatibility: no compatible CMatrixTransformProxy::Update is available");
        return false;
    }
    bool hasForceUpdateScene = IsDwmFunctionPointerValid(
        reinterpret_cast<void*>(g_windowListForceUpdateSceneOriginal));
    bool hasUpdateScene =
        IsDwmFunctionPointerValid(reinterpret_cast<void*>(g_windowListUpdateSceneOriginal));
    if (!hasForceUpdateScene && !hasUpdateScene)
    {
        Wh_Log(L"DWM compatibility: neither CWindowList scene hook is available");
        return false;
    }
    g_windowDataHwndOffset = FindOffsetFromFunction(
        reinterpret_cast<void*>(g_isGhostWindowOriginal), SIZE_MAX);
    size_t accessorWindowDataOffset = FindOffsetFromFunction(
        reinterpret_cast<void*>(g_topLevelWindowGetWindowData), SIZE_MAX);
    if (g_windowDataHwndOffset == SIZE_MAX)
    {
        Wh_Log(L"DWM compatibility: CWindowData HWND offset could not be derived");
        return false;
    }
    if (!FindConstructorWindowDataOffsets(
            reinterpret_cast<void*>(g_topLevelWindowConstructorFunction),
            &g_windowDataTopLevelWindowOffset, &g_topLevelWindowWindowDataOffset))
    {
        Wh_Log(L"DWM compatibility: bidirectional top-level mapping could not be derived");
        return false;
    }
    if (accessorWindowDataOffset != SIZE_MAX &&
        accessorWindowDataOffset != g_topLevelWindowWindowDataOffset)
    {
        Wh_Log(L"DWM compatibility: conflicting CTopLevelWindow mapping offsets");
        return false;
    }
    g_topLevelWindow3DWindowDataOffset = FindStoredWindowDataOffset(
        reinterpret_cast<void*>(g_topLevelWindow3DSetWindowDataOriginal));
    if (g_topLevelWindow3DWindowDataOffset == SIZE_MAX)
    {
        Wh_Log(L"DWM compatibility: early transition reverse mapping unavailable");
    }
    size_t pairedTopLevelWindowOffset = SIZE_MAX;
    size_t pairedTopLevelWindow3DOffset = SIZE_MAX;
    if (FindWindowDataTopLevelOffsets(reinterpret_cast<void*>(g_ensureTopLevelWindowFunction),
                                      &pairedTopLevelWindowOffset,
                                      &pairedTopLevelWindow3DOffset) &&
        pairedTopLevelWindowOffset == g_windowDataTopLevelWindowOffset)
    {
        g_windowDataTopLevelWindow3DOffset = pairedTopLevelWindow3DOffset;
    }
    else
    {
        g_windowDataTopLevelWindow3DOffset = SIZE_MAX;
        Wh_Log(L"DWM compatibility: existing-window transition mapping unavailable");
    }
    Wh_Log(L"DWM compatibility ABI: HWND lookup=pointer transition lookup=%s "
           L"matrix=%s visual=root proxyOffset=0x%zx compositorOffset=0x%zx "
           L"threadIdOffset=0x%zx TLW3DDataOffset=0x%zx",
           g_getSyncedWindowDataVoid
               ? L"void"
               : (g_getSyncedWindowDataLong ? L"HRESULT" : L"unavailable"),
           g_cMatrixTransformProxyUpdate ? L"double" : L"float", g_visualProxyOffset,
           g_desktopManagerCompositorOffset, g_desktopManagerThreadIdOffset,
           g_topLevelWindow3DWindowDataOffset);
    void* compositor = FindDwmCompositor();
    g_dwmCompositor.store(compositor, std::memory_order_release);
    bool canDiscoverCompositorFromTimeline = IsDwmFunctionPointerValid(
        reinterpret_cast<void*>(g_desktopManagerAdvanceTimelinesOriginal));
    if (!compositor && !canDiscoverCompositorFromTimeline)
    {
        Wh_Log(L"DWM compatibility: compositor unavailable and no timeline discovery hook exists");
        return false;
    }
    if (!compositor)
    {
        Wh_Log(L"DWM compatibility: compositor discovery deferred to the first scene timeline");
    }
    Wh_Log(L"DWM startup cache ready: timestamp=0x%08X imageSize=0x%X "
           L"HWND=0x%zx TLW=0x%zx TLW3D=0x%zx TLWData=0x%zx "
           L"TLWVtable=%p sceneHooks=%s%s",
           g_dwmModuleLayout.timeDateStamp, g_dwmModuleLayout.sizeOfImage, g_windowDataHwndOffset,
           g_windowDataTopLevelWindowOffset, g_windowDataTopLevelWindow3DOffset,
           g_topLevelWindowWindowDataOffset, g_topLevelWindowVtableSymbol,
           hasForceUpdateScene ? L"ForceUpdateScene " : L"",
           hasUpdateScene ? L"UpdateScene" : L"");
    if (!WindhawkUtils::SetFunctionHook(g_topLevelWindowConstructorFunction,
                                        TopLevelWindowConstructorHook,
                                        &g_topLevelWindowConstructorOriginal))
    {
        Wh_Log(L"DWM hooks: failed to register CTopLevelWindow constructor hook");
        return false;
    }
    return true;
}

static void BindPendingAnimationSlotTransforms(bool validateCurrentVisuals)
{
    if (!IsOnDwmSceneThread())
    {
        return;
    }
    void* windowList = g_windowListForSceneWake.load(std::memory_order_acquire);
    if (!IsDwmObjectPointerValid(windowList, g_windowListVtable) ||
        !g_findWindowDataByHwnd || !g_cVisualProxySetTransform)
    {
        return;
    }
    ULONGLONG now = GetTickCount64();
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        HWND hwnd = nullptr;
        ULONGLONG generation = 0;
        ULONGLONG rebindRevision = 0;
        void* matrixTransformProxy = nullptr;
        void* previouslyBoundTopLevelVisualProxy = nullptr;
        void* previouslyBoundTransitionVisualProxy = nullptr;
        bool previouslyTopLevelAttached = false;
        bool previouslyTransitionAttached = false;
        bool windowStateThrob = false;
        bool bindingPending = false;
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& slot = g_animationSlots[i];
        bindingPending = !slot.transformAttached ||
                         (slot.windowStateThrob && !slot.transitionTransformAttached) ||
                         slot.transformRebindRevision != slot.submittedTransformRebindRevision;
        bool periodicValidation = validateCurrentVisuals && now >= slot.nextVisualValidation;
        bool forceNativeTransitionRebind = validateCurrentVisuals && slot.windowStateThrob;
        if (slot.active && slot.hwnd && slot.matrixTransformProxy &&
            (bindingPending || periodicValidation || forceNativeTransitionRebind))
        {
            slot.hookUsers++;
            slot.nextVisualValidation = now + 250;
            hwnd = slot.hwnd;
            generation = slot.generation;
            rebindRevision = slot.transformRebindRevision;
            matrixTransformProxy = slot.matrixTransformProxy;
            previouslyBoundTopLevelVisualProxy = slot.boundTopLevelVisualProxy;
            previouslyBoundTransitionVisualProxy = slot.boundTransitionVisualProxy;
            previouslyTopLevelAttached = slot.transformAttached;
            previouslyTransitionAttached = slot.transitionTransformAttached;
            windowStateThrob = slot.windowStateThrob;
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (!matrixTransformProxy)
        {
            continue;
        }
        void* windowData = FindWindowDataByHwnd(windowList, hwnd);
        void* topLevelWindow = nullptr;
        void* topLevelWindow3D = nullptr;
        void* topLevelVisualProxy = nullptr;
        void* transitionVisualProxy = nullptr;
        if (windowData && GetHwndFromWindowData(windowData) == hwnd)
        {
            if (!ResolveDwmWindowObjects(windowData, &topLevelWindow, &topLevelWindow3D) &&
                g_ensureTopLevelWindowFunction(windowList, windowData) >= 0)
            {
                ResolveDwmWindowObjects(windowData, &topLevelWindow, &topLevelWindow3D);
            }
            if (topLevelWindow)
            {
                topLevelVisualProxy = GetTopLevelVisualProxy(topLevelWindow);
            }
            if (windowStateThrob && topLevelWindow3D)
            {
                transitionVisualProxy = GetTransitionVisualProxy(topLevelWindow3D);
            }
        }
        bool topLevelBindingAttempted =
            topLevelVisualProxy &&
            (bindingPending || topLevelVisualProxy != previouslyBoundTopLevelVisualProxy ||
             forceNativeTransitionRebind);
        bool transitionBindingAttempted =
            transitionVisualProxy && transitionVisualProxy != topLevelVisualProxy &&
            (bindingPending ||
             transitionVisualProxy != previouslyBoundTransitionVisualProxy ||
             forceNativeTransitionRebind);
        long topLevelBindResult = E_FAIL;
        long transitionBindResult = E_FAIL;
        if (topLevelBindingAttempted)
        {
            topLevelBindResult =
                g_cVisualProxySetTransform(topLevelVisualProxy, matrixTransformProxy);
        }
        if (transitionBindingAttempted)
        {
            transitionBindResult =
                g_cVisualProxySetTransform(transitionVisualProxy, matrixTransformProxy);
        }
        bool logBinding = false;
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& currentSlot = g_animationSlots[i];
        if (currentSlot.active && currentSlot.generation == generation &&
            currentSlot.matrixTransformProxy == matrixTransformProxy)
        {
            if (!topLevelVisualProxy)
            {
                currentSlot.transformAttached = false;
                currentSlot.boundTopLevelVisualProxy = nullptr;
            }
            else if (topLevelBindingAttempted && topLevelBindResult >= 0)
            {
                currentSlot.transformAttached = true;
                currentSlot.boundTopLevelVisualProxy = topLevelVisualProxy;
                currentSlot.submittedTransformRebindRevision = rebindRevision;
                logBinding = !previouslyTopLevelAttached ||
                             previouslyBoundTopLevelVisualProxy != topLevelVisualProxy;
            }
            else if (topLevelBindingAttempted)
            {
                currentSlot.transformAttached = false;
                currentSlot.boundTopLevelVisualProxy = nullptr;
            }
            if (!windowStateThrob || !transitionVisualProxy ||
                transitionVisualProxy == topLevelVisualProxy)
            {
                currentSlot.transitionTransformAttached =
                    transitionVisualProxy && transitionVisualProxy == topLevelVisualProxy &&
                    currentSlot.transformAttached;
                currentSlot.boundTransitionVisualProxy =
                    currentSlot.transitionTransformAttached ? transitionVisualProxy : nullptr;
            }
            else if (transitionBindingAttempted && transitionBindResult >= 0)
            {
                currentSlot.transitionTransformAttached = true;
                currentSlot.boundTransitionVisualProxy = transitionVisualProxy;
                logBinding = logBinding || !previouslyTransitionAttached ||
                             previouslyBoundTransitionVisualProxy != transitionVisualProxy;
            }
            else if (transitionBindingAttempted)
            {
                currentSlot.transitionTransformAttached = false;
                currentSlot.boundTransitionVisualProxy = nullptr;
            }
        }
        if (currentSlot.hookUsers > 0)
        {
            currentSlot.hookUsers--;
            if (currentSlot.hookUsers == 0)
            {
                WakeAllConditionVariable(&g_animationSlotsCondition);
            }
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (logBinding)
        {
            Wh_Log(L"TRANSFORM BOUND FROM SCENE: Slot=%d HWND=%p "
                   L"CWindowData=%p CTopLevelWindow=%p "
                   L"VisualProxy=%p TransitionProxy=%p MatrixProxy=%p",
                   i, hwnd, windowData, topLevelWindow, topLevelVisualProxy,
                   transitionVisualProxy, matrixTransformProxy);
        }
    }
}

static void BackfillExistingDwmWindowMappings()
{
    if (!IsOnDwmSceneThread())
    {
        return;
    }
    AcquireSRWLockShared(&g_existingWindowBackfillLock);
    void* windowList = g_windowListForSceneWake.load(std::memory_order_acquire);
    unsigned int count = g_existingWindowBackfillCount.load(std::memory_order_acquire);
    unsigned int index = g_existingWindowBackfillIndex.load(std::memory_order_acquire);
    if (!IsDwmObjectPointerValid(windowList, g_windowListVtable) ||
        !g_findWindowDataByHwnd || index >= count)
    {
        ReleaseSRWLockShared(&g_existingWindowBackfillLock);
        return;
    }
    constexpr unsigned int windowsPerPass = 16;
    unsigned int processed = 0;
    while (index < count && processed++ < windowsPerPass)
    {
        HWND hwnd = g_existingWindowBackfill[index++];
        if (!IsWindow(hwnd))
        {
            continue;
        }
        void* windowData = FindWindowDataByHwnd(windowList, hwnd);
        if (!windowData ||
            (g_windowDataHwndOffset != SIZE_MAX && GetHwndFromWindowData(windowData) != hwnd))
        {
            continue;
        }
        void* topLevelWindow = nullptr;
        void* topLevelWindow3D = nullptr;
        bool mapped = ResolveDwmWindowObjects(windowData, &topLevelWindow, &topLevelWindow3D);
        if (!mapped && g_ensureTopLevelWindowFunction(windowList, windowData) >= 0)
        {
            mapped = ResolveDwmWindowObjects(windowData, &topLevelWindow, &topLevelWindow3D);
        }
        if (mapped)
        {
            g_existingWindowBackfillMapped.fetch_add(1, std::memory_order_relaxed);
        }
    }
    g_existingWindowBackfillIndex.store(index, std::memory_order_release);
    HWND nextWindow = index < count ? g_existingWindowBackfill[index] : nullptr;
    unsigned int mapped = g_existingWindowBackfillMapped.load(std::memory_order_relaxed);
    ReleaseSRWLockShared(&g_existingWindowBackfillLock);
    if (index < count)
    {
        RequestDwmScenePass(nextWindow);
    }
    else
    {
        Wh_Log(L"DWM existing-window backfill completed: %u/%u windows mapped", mapped, count);
    }
}

static void SubmitPendingWobblySceneWork()
{
    if (!IsOnDwmSceneThread())
    {
        return;
    }
    // A newer serial published during this pass receives another wake below.
    ULONGLONG submittedThrough = g_sceneRequestedSerial.load(std::memory_order_acquire);
    g_sceneWakeScheduled.store(false, std::memory_order_release);
    g_sceneWakePostTimestamp.store(0, std::memory_order_release);
    g_scenePassCounter.fetch_add(1, std::memory_order_release);
    BackfillExistingDwmWindowMappings();
    // Private compositor resources live only on the verified scene thread.
    EnsurePendingMatrixTransformProxies();
    // Submit immutable physics revisions on the scene thread.
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        ApplyAnimationSlotTransform(i, 1.0);
    }
    FinalizeRetiringSlots();
    g_sceneSubmittedSerial.store(submittedThrough, std::memory_order_release);
    g_sceneWakeRetryCount.store(0, std::memory_order_release);
    ULONGLONG stallStartedAt = g_sceneWakeStallStartedAt.exchange(0, std::memory_order_acq_rel);
    if (g_sceneWakeStalled.exchange(false, std::memory_order_acq_rel))
    {
        Wh_Log(L"DWM scene wake recovered after %llu ms",
               stallStartedAt ? GetTickCount64() - stallStartedAt : 0);
    }
    // Wake again if the worker published a newer matrix.
    if (g_sceneRequestedSerial.load(std::memory_order_acquire) > submittedThrough)
    {
        PostPendingDwmSceneWake(nullptr, false);
    }
}

static bool HasPendingWobblySceneWork()
{
    return g_sceneRequestedSerial.load(std::memory_order_acquire) >
               g_sceneSubmittedSerial.load(std::memory_order_acquire) ||
           g_existingWindowBackfillIndex.load(std::memory_order_acquire) <
               g_existingWindowBackfillCount.load(std::memory_order_acquire) ||
           HasAnyAnimationSlots();
}

static void BindAnimationTransformsAfterNativeScene()
{
    // Check every active slot, but write only when the proxy changed or the
    // binding is pending. Native state transitions are the only forced case.
    BindPendingAnimationSlotTransforms(true);
}

static long __cdecl ForceUpdateSceneHook(void* pThis)
{
    if (HasExactDwmVtableTrusted(pThis, g_windowListVtable))
    {
        g_windowListForSceneWake.store(pThis, std::memory_order_release);
    }
    if (!HasPendingWobblySceneWork())
    {
        return g_windowListForceUpdateSceneOriginal(pThis);
    }
    bool canSubmit = RegisterDwmSceneThread(g_desktopManagerAdvanceTimelinesOriginal == nullptr);
    bool outermostPass = canSubmit && !g_insideWobblyScenePass;
    if (outermostPass)
    {
        g_insideWobblyScenePass = true;
        SubmitPendingWobblySceneWork();
    }
    long result = g_windowListForceUpdateSceneOriginal(pThis);
    if (outermostPass)
    {
        BindAnimationTransformsAfterNativeScene();
        g_insideWobblyScenePass = false;
    }
    return result;
}

static long __cdecl UpdateSceneHook(void* pThis)
{
    if (HasExactDwmVtableTrusted(pThis, g_windowListVtable))
    {
        g_windowListForSceneWake.store(pThis, std::memory_order_release);
    }
    if (!HasPendingWobblySceneWork())
    {
        return g_windowListUpdateSceneOriginal(pThis);
    }
    bool canSubmit = RegisterDwmSceneThread(g_desktopManagerAdvanceTimelinesOriginal == nullptr);
    bool outermostPass = canSubmit && !g_insideWobblyScenePass;
    if (outermostPass)
    {
        g_insideWobblyScenePass = true;
        SubmitPendingWobblySceneWork();
    }
    long result = g_windowListUpdateSceneOriginal(pThis);
    if (outermostPass)
    {
        BindAnimationTransformsAfterNativeScene();
        g_insideWobblyScenePass = false;
    }
    return result;
}

static void __cdecl AdvanceTimelinesHook(void* pThis, double currentTime)
{
    bool canSubmit = RegisterDwmSceneThread(true);
    if (canSubmit)
    {
        if (CacheDwmObjectsFromDesktopManager(pThis))
        {
            g_dwmObjectDiscoveryFailureCount.store(0, std::memory_order_release);
        }
        else
        {
            unsigned int failures =
                g_dwmObjectDiscoveryFailureCount.fetch_add(1, std::memory_order_acq_rel) + 1;
            if (failures == 1 || failures % 600 == 0)
            {
                Wh_Log(L"DWM compositor temporarily unavailable; retrying (%u)", failures);
            }
            canSubmit = false;
        }
    }
    // Serial comparison prevents coalesced scene passes from stranding work.
    if (g_sceneRequestedSerial.load(std::memory_order_acquire) >
            g_sceneSubmittedSerial.load(std::memory_order_acquire) &&
        canSubmit && !g_insideWobblyScenePass)
    {
        g_insideWobblyScenePass = true;
        SubmitPendingWobblySceneWork();
        g_insideWobblyScenePass = false;
    }
    g_desktopManagerAdvanceTimelinesOriginal(pThis, currentTime);
    if (canSubmit && !g_insideWobblyScenePass)
    {
        g_insideWobblyScenePass = true;
        BindAnimationTransformsAfterNativeScene();
        g_insideWobblyScenePass = false;
    }
}

static void __cdecl DesktopManagerHandleThreadMessageHook(UINT message,
                                                          UINT_PTR wParam,
                                                          INT_PTR lParam)
{
    if (message != WM_DWM_SCENE_WAKE || wParam != DWM_SCENE_WAKE_WPARAM)
    {
        g_desktopManagerHandleThreadMessageOriginal(message, wParam, lParam);
        return;
    }
    if (lParam != g_dwmSceneWakeToken.load(std::memory_order_acquire))
    {
        // Swallow a wake left by an older instance of the mod.
        return;
    }
    auto acknowledgeWake = []
    {
        unsigned int outstanding = g_sceneWakeOutstanding.load(std::memory_order_acquire);
        while (outstanding &&
               !g_sceneWakeOutstanding.compare_exchange_weak(
                   outstanding, outstanding - 1, std::memory_order_acq_rel,
                   std::memory_order_acquire))
        {
        }
    };
    if (!RegisterDwmSceneThread(true) || g_insideWobblyScenePass)
    {
        g_sceneWakeScheduled.store(false, std::memory_order_release);
        g_sceneWakePostTimestamp.store(0, std::memory_order_release);
        acknowledgeWake();
        return;
    }

    g_insideWobblyScenePass = true;
    SubmitPendingWobblySceneWork();
    BindPendingAnimationSlotTransforms(false);

    void* windowList = g_windowListForSceneWake.load(std::memory_order_acquire);
    bool nativeSceneUpdated = false;
    if (IsDwmObjectPointerValid(windowList, g_windowListVtable))
    {
        if (g_windowListForceUpdateSceneOriginal)
        {
            g_windowListForceUpdateSceneOriginal(windowList);
            nativeSceneUpdated = true;
        }
        else if (g_windowListUpdateSceneOriginal)
        {
            g_windowListUpdateSceneOriginal(windowList);
            nativeSceneUpdated = true;
        }
    }
    else
    {
        void* manager = g_desktopManager.load(std::memory_order_acquire);
        if (HasExactDwmVtableTrusted(manager, g_desktopManagerVtable) &&
            g_desktopManagerPostStartAnimations)
        {
            g_desktopManagerPostStartAnimations(manager);
        }
    }
    if (nativeSceneUpdated)
    {
        // uDWM can replace the root transform while maximizing or restoring.
        BindPendingAnimationSlotTransforms(true);
    }
    g_insideWobblyScenePass = false;
    acknowledgeWake();
}

static double Lerp(double a, double b, double t)
{
    return a + (b - a) * t;
}

static double SmoothSymmetricCompression(double value, double referenceRange)
{
    if (!std::isfinite(value) || referenceRange <= 0.0)
    {
        return 0.0;
    }
    double magnitude = std::abs(value);
    double knee = referenceRange * 0.75;
    if (magnitude <= knee)
    {
        return value;
    }
    double transitionRange = referenceRange - knee;
    // Monotonic compression avoids sticking at extreme deformation.
    double softenedMagnitude =
        knee + transitionRange * std::asinh((magnitude - knee) / transitionRange);
    return std::copysign(softenedMagnitude, value);
}

static double SmoothCenteredCompression(double value, double center, double radius)
{
    return center + SmoothSymmetricCompression(value - center, radius);
}

static void CalculateBernsteinBasis(double value, double basis[4])
{
    double inverse = 1.0 - value;
    basis[0] = inverse * inverse * inverse;
    basis[1] = 3.0 * inverse * inverse * value;
    basis[2] = 3.0 * inverse * value * value;
    basis[3] = value * value * value;
}

static Vec2 Subtract(const Vec2& a, const Vec2& b)
{
    return {a.x - b.x, a.y - b.y};
}

static double Length(const Vec2& value)
{
    return std::sqrt(value.x * value.x + value.y * value.y);
}

static int GetPointIndex(int x, int y)
{
    return y * GRID_WIDTH + x;
}

static void InitializeMesh(WobbleMesh& mesh, double width, double height)
{
    mesh.width = width;
    mesh.height = height;
    mesh.active = false;
    mesh.dragging = false;
    mesh.resizing = false;
    mesh.canWobbleTop = true;
    mesh.canWobbleLeft = true;
    mesh.canWobbleRight = true;
    mesh.canWobbleBottom = true;
    mesh.dragPointIndex = -1;
    mesh.dragOffset = {0.0, 0.0};
    mesh.lastDragPosition = {0.0, 0.0};
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            double normalizedX = static_cast<double>(x) / static_cast<double>(GRID_WIDTH - 1);
            double normalizedY = static_cast<double>(y) / static_cast<double>(GRID_HEIGHT - 1);
            WobblePoint& point = mesh.points[GetPointIndex(x, y)];
            point.basePosition = {normalizedX * width, normalizedY * height};
            point.position = point.basePosition;
            point.velocity = {0.0, 0.0};
            point.force = {0.0, 0.0};
            point.fixed = false;
        }
    }
}

static void UpdateMeshBaseGrid(WobbleMesh& mesh, double width, double height)
{
    mesh.width = width;
    mesh.height = height;
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            double normalizedX = static_cast<double>(x) / static_cast<double>(GRID_WIDTH - 1);
            double normalizedY = static_cast<double>(y) / static_cast<double>(GRID_HEIGHT - 1);
            mesh.points[GetPointIndex(x, y)].basePosition = {normalizedX * width,
                                                             normalizedY * height};
        }
    }
}

static void OffsetMeshPositions(WobbleMesh& mesh, double x, double y)
{
    for (WobblePoint& point : mesh.points)
    {
        point.position.x += x;
        point.position.y += y;
    }
}

static void SetMeshResizeMode(WobbleMesh& mesh, bool resizing)
{
    mesh.resizing = resizing;
    // Unlock only the resize edges that actually moved.
    mesh.canWobbleTop = !resizing;
    mesh.canWobbleLeft = !resizing;
    mesh.canWobbleRight = !resizing;
    mesh.canWobbleBottom = !resizing;
}

static void UpdateResizeEdges(WobbleMesh& mesh, const RECT& originalRect, const RECT& currentRect)
{
    if (!mesh.resizing)
    {
        return;
    }
    if (currentRect.top != originalRect.top)
    {
        mesh.canWobbleTop = true;
    }
    if (currentRect.left != originalRect.left)
    {
        mesh.canWobbleLeft = true;
    }
    if (currentRect.right != originalRect.right)
    {
        mesh.canWobbleRight = true;
    }
    if (currentRect.bottom != originalRect.bottom)
    {
        mesh.canWobbleBottom = true;
    }
}

static void ApplyResizeConstraints(WobbleMesh& mesh)
{
    if (!mesh.resizing)
    {
        return;
    }
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            WobblePoint& point = mesh.points[GetPointIndex(x, y)];
            // Match KWin's edge-locking rules.
            if ((!mesh.canWobbleLeft && x < GRID_WIDTH - 1) || (!mesh.canWobbleRight && x > 0))
            {
                point.position.x = point.basePosition.x;
            }
            if ((!mesh.canWobbleTop && y < GRID_HEIGHT - 1) || (!mesh.canWobbleBottom && y > 0))
            {
                point.position.y = point.basePosition.y;
            }
        }
    }
}

static void ApplyWindowStateThrob(WobbleMesh& mesh, bool maximizing, bool seedInitialDisplacement,
                                  const WobblySettings& settings, Vec2 direction)
{
    double directionLength = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (directionLength < 0.001)
    {
        // Maximize rises towards the monitor edge; restore falls away from it.
        direction = maximizing ? Vec2{0.0, -1.0} : Vec2{0.0, 1.0};
        directionLength = 1.0;
    }
    direction.x /= directionLength;
    direction.y /= directionLength;
    double dragRatio = std::clamp(settings.drag / 100.0, 0.01, 0.99);
    double moveRatio = std::clamp(settings.moveFactor / 100.0, 0.01, 0.25);
    double stiffnessProgress = std::clamp((15.0 - settings.stiffness) / 14.0, 0.0, 1.0);
    double dragProgress = std::clamp((settings.drag - 80.0) / 17.0, 0.0, 1.0);
    double moveProgress = std::clamp((settings.moveFactor - 10.0) / 15.0, 0.0, 1.0);
    double wobbleProgress = 0.30 * stiffnessProgress + 0.45 * dragProgress + 0.25 * moveProgress;
    // Balance impulses across presets without flattening high wobbliness.
    double responseGain = moveRatio / std::max(1.0 - dragRatio, 0.01);
    double compensatedGain = std::clamp(responseGain, 0.65, 8.50);
    double impulseMagnitude = 20.0 / std::sqrt(compensatedGain);
    impulseMagnitude = std::clamp(impulseMagnitude, 6.0, 25.0);
    if (!maximizing)
    {
        impulseMagnitude *= 1.20;
    }
    double initialOffset = seedInitialDisplacement ? 2.5 + 1.5 * wobbleProgress : 0.0;
    if (!maximizing)
    {
        initialOffset *= 1.20;
    }
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            WobblePoint& point = mesh.points[GetPointIndex(x, y)];
            point.velocity = {direction.x * impulseMagnitude, direction.y * impulseMagnitude};
            point.position.x += direction.x * initialOffset;
            point.position.y += direction.y * initialOffset;
            if (x > 0 && x < GRID_WIDTH - 1 && y > 0 && y < GRID_HEIGHT - 1)
            {
                point.fixed = true;
            }
        }
    }
    mesh.active = true;
}

static void ClearWindowStateThrobConstraints(WobbleMesh& mesh)
{
    // Inner anchors are used only for passive state transitions.
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        mesh.points[i].fixed = false;
    }
    mesh.dragPointIndex = -1;
    mesh.dragging = false;
}

static int FindNearestPoint(const WobbleMesh& mesh, const Vec2& position)
{
    int nearestIndex = 0;
    double nearestDistanceSquared = DBL_MAX;
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        const WobblePoint& point = mesh.points[i];
        double dx = point.basePosition.x - position.x;
        double dy = point.basePosition.y - position.y;
        double distanceSquared = dx * dx + dy * dy;
        if (distanceSquared < nearestDistanceSquared)
        {
            nearestDistanceSquared = distanceSquared;
            nearestIndex = i;
        }
    }
    return nearestIndex;
}

static void SmoothMeshField(WobbleMesh& mesh, bool velocityField)
{
    Vec2 smoothed[GRID_POINT_COUNT] = {};
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            int index = GetPointIndex(x, y);
            const Vec2& value =
                velocityField ? mesh.points[index].velocity : mesh.points[index].force;
            Vec2 neighborSum = {};
            int neighborCount = 0;
            for (int offsetY = -1; offsetY <= 1; offsetY++)
            {
                for (int offsetX = -1; offsetX <= 1; offsetX++)
                {
                    if (offsetX == 0 && offsetY == 0)
                    {
                        continue;
                    }
                    int neighborX = x + offsetX;
                    int neighborY = y + offsetY;
                    if (neighborX < 0 || neighborX >= GRID_WIDTH || neighborY < 0 ||
                        neighborY >= GRID_HEIGHT)
                    {
                        continue;
                    }
                    const WobblePoint& neighbor = mesh.points[GetPointIndex(neighborX, neighborY)];
                    const Vec2& neighborValue = velocityField ? neighbor.velocity : neighbor.force;
                    neighborSum.x += neighborValue.x;
                    neighborSum.y += neighborValue.y;
                    neighborCount++;
                }
            }
            if (neighborCount > 0)
            {
                double inverseNeighborCount = 1.0 / static_cast<double>(neighborCount);
                smoothed[index] = {value.x * 0.5 + neighborSum.x * inverseNeighborCount * 0.5,
                                   value.y * 0.5 + neighborSum.y * inverseNeighborCount * 0.5};
            }
            else
            {
                smoothed[index] = value;
            }
        }
    }
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        if (velocityField)
        {
            mesh.points[i].velocity = smoothed[i];
        }
        else
        {
            mesh.points[i].force = smoothed[i];
        }
    }
}

static void CalculateMeshForces(WobbleMesh& mesh, const WobblySettings& settings)
{
    double stiffness = std::clamp(settings.stiffness / 100.0, 0.01, 1.0);
    static constexpr int neighborOffsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            int index = GetPointIndex(x, y);
            WobblePoint& point = mesh.points[index];
            if (point.fixed)
            {
                point.force = {(point.basePosition.x - point.position.x) * stiffness,
                               (point.basePosition.y - point.position.y) * stiffness};
                continue;
            }
            Vec2 springSum = {};
            int neighborCount = 0;
            for (const auto& offset : neighborOffsets)
            {
                int neighborX = x + offset[0];
                int neighborY = y + offset[1];
                if (neighborX < 0 || neighborX >= GRID_WIDTH || neighborY < 0 ||
                    neighborY >= GRID_HEIGHT)
                {
                    continue;
                }
                const WobblePoint& neighbor = mesh.points[GetPointIndex(neighborX, neighborY)];
                springSum.x += (neighbor.position.x - point.position.x) -
                               (neighbor.basePosition.x - point.basePosition.x);
                springSum.y += (neighbor.position.y - point.position.y) -
                               (neighbor.basePosition.y - point.basePosition.y);
                neighborCount++;
            }
            double forceScale =
                neighborCount > 0 ? stiffness / static_cast<double>(neighborCount) : 0.0;
            point.force = {springSum.x * forceScale, springSum.y * forceScale};
        }
    }
    SmoothMeshField(mesh, false);
}

static MeshStepResult SimulateMeshStep(WobbleMesh& mesh, const WobblySettings& settings,
                                       double deltaTime)
{
    CalculateMeshForces(mesh, settings);
    double deltaTimeMilliseconds = std::clamp(deltaTime * 1000.0, 0.0, 10.0);
    double drag = std::clamp(settings.drag / 100.0, 0.01, 1.0);
    double accelerationSum = 0.0;
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        WobblePoint& point = mesh.points[i];
        Vec2 acceleration = {std::clamp(point.force.x, -1000.0, 1000.0),
                             std::clamp(point.force.y, -1000.0, 1000.0)};
        accelerationSum += std::abs(acceleration.x) + std::abs(acceleration.y);
        point.velocity.x = acceleration.x * deltaTimeMilliseconds + point.velocity.x * drag;
        point.velocity.y = acceleration.y * deltaTimeMilliseconds + point.velocity.y * drag;
    }
    SmoothMeshField(mesh, true);
    double moveFactor = std::clamp(settings.moveFactor / 100.0, 0.01, 0.25);
    double velocitySum = 0.0;
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        WobblePoint& point = mesh.points[i];
        point.velocity.x = std::clamp(point.velocity.x, -1000.0, 1000.0);
        point.velocity.y = std::clamp(point.velocity.y, -1000.0, 1000.0);
        velocitySum += std::abs(point.velocity.x) + std::abs(point.velocity.y);
        point.position.x += point.velocity.x * deltaTimeMilliseconds * moveFactor;
        point.position.y += point.velocity.y * deltaTimeMilliseconds * moveFactor;
        if (!std::isfinite(point.position.x) || !std::isfinite(point.position.y) ||
            !std::isfinite(point.velocity.x) || !std::isfinite(point.velocity.y))
        {
            point.position = point.basePosition;
            point.velocity = {};
            point.force = {};
        }
    }
    ApplyResizeConstraints(mesh);
    MeshStepResult result = {!(accelerationSum < 0.5 && velocitySum < 0.5)};
    mesh.active = result.wobblying;
    return result;
}

static void BeginDrag(WobbleMesh& mesh, const Vec2& mousePosition)
{
    mesh.dragPointIndex = FindNearestPoint(mesh, mousePosition);
    WobblePoint& dragPoint = mesh.points[mesh.dragPointIndex];
    // Preserve the exact grab offset for a stable affine pivot.
    mesh.dragOffset = {mousePosition.x - dragPoint.basePosition.x,
                       mousePosition.y - dragPoint.basePosition.y};
    mesh.lastDragPosition = mousePosition;
    // KDE's grabbed point follows a soft spring constraint.
    dragPoint.fixed = true;
    mesh.dragging = true;
    mesh.active = true;
}

static void EndDrag(WobbleMesh& mesh)
{
    if (!mesh.dragging || mesh.dragPointIndex < 0)
    {
        return;
    }
    // Keep the release point anchored to the real window position while the
    // remaining mesh decays. Unpinning it makes the visual coast away from the
    // HWND and snap back when the identity transform is restored.
    mesh.dragging = false;
}

static bool ReplaceAnimationWithStateThrob(WindowAnimationSlot& slot, int width, int height,
                                           bool maximizing, Vec2 direction)
{
    if (width <= 0 || height <= 0)
    {
        return false;
    }
    // Invalidate a scene snapshot of the preceding drag before replacing it.
    slot.generation++;
    InitializeMesh(slot.mesh, static_cast<double>(width), static_cast<double>(height));
    ApplyWindowStateThrob(slot.mesh, maximizing, true, slot.settings, direction);
    slot.previousMesh = slot.mesh;
    slot.dragging = false;
    slot.freeStepPending = true;
    slot.windowStateThrob = true;
    slot.transformRebindRevision++;
    slot.meshRevision++;
    slot.identityApplied = false;
    slot.meshIdentityPending = false;
    slot.order = ++g_animationOrderCounter;
    return true;
}

static void LogMeshSummary(const WobbleMesh& mesh, const wchar_t* prefix)
{
    double maximumDisplacement = 0.0;
    double maximumSpeed = 0.0;
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        const WobblePoint& point = mesh.points[i];
        Vec2 displacement = Subtract(point.position, point.basePosition);
        double displacementLength = Length(displacement);
        double speed = Length(point.velocity);
        maximumDisplacement = std::max(maximumDisplacement, displacementLength);
        maximumSpeed = std::max(maximumSpeed, speed);
    }
    Wh_Log(L"%s MaxDisplacement=%.2f MaxSpeed=%.2f", prefix, maximumDisplacement, maximumSpeed);
}

static WobblySettings GetSettingsSnapshot();
static void UpdateAnimationFrame();
static void RetireAnimationSlot(int slotIndex);
static void StopAllAnimations();
static void MarkObservedWindowTransitionPending(HWND hwnd, bool expectedZoomed);
static void HandleLocationChange(HWND hwnd, LONG idObject, LONG idChild);

static bool HasAnyAnimationSlots()
{
    bool anyAnimationSlots = false;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        if (g_animationSlots[i].active || g_animationSlots[i].retiring)
        {
            anyAnimationSlots = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    return anyAnimationSlots;
}

static double GetPreferredAnimationRate(HWND hwnd)
{
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXW monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    DEVMODEW displayMode = {};
    displayMode.dmSize = sizeof(displayMode);
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo) &&
        EnumDisplaySettingsW(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &displayMode) &&
        displayMode.dmDisplayFrequency >= 30 && displayMode.dmDisplayFrequency <= 500)
    {
        double displayRate =
            std::clamp(static_cast<double>(displayMode.dmDisplayFrequency), 60.0, 240.0);
        if ((monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0)
        {
            // Oversample secondary outputs because DWM exposes no per-output phase.
            return std::min(240.0, displayRate * 2.0);
        }
        return displayRate;
    }
    return 60.0;
}

static bool ArmNextAnimationTick()
{
    if (!g_animationTimer || !g_animationClockArmed || g_animationFrequency.QuadPart <= 0 ||
        g_animationTargetHz <= 0.0)
    {
        return false;
    }
    LARGE_INTEGER now = {};
    if (!QueryPerformanceCounter(&now))
    {
        return false;
    }
    LONGLONG interval = std::max<LONGLONG>(
        1, static_cast<LONGLONG>(std::llround(static_cast<double>(g_animationFrequency.QuadPart) /
                                              g_animationTargetHz)));
    if (g_nextAnimationCounter.QuadPart <= 0)
    {
        g_nextAnimationCounter.QuadPart = now.QuadPart + interval;
    }
    else
    {
        do
        {
            g_nextAnimationCounter.QuadPart += interval;
        } while (g_nextAnimationCounter.QuadPart <= now.QuadPart);
    }
    LONGLONG remainingCounter = g_nextAnimationCounter.QuadPart - now.QuadPart;
    LONGLONG due100Nanoseconds = std::max<LONGLONG>(
        1, static_cast<LONGLONG>(std::ceil(static_cast<double>(remainingCounter) * 10000000.0 /
                                           static_cast<double>(g_animationFrequency.QuadPart))));
    LARGE_INTEGER dueTime = {};
    dueTime.QuadPart = -due100Nanoseconds;
    return SetWaitableTimer(g_animationTimer, &dueTime, 0, nullptr, nullptr, FALSE) != FALSE;
}

static void DisableAnimationThreadPowerThrottling()
{
    // Keep free decay out of EcoQoS throttling.
    THREAD_POWER_THROTTLING_STATE powerThrottling = {};
    powerThrottling.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
    powerThrottling.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
    powerThrottling.StateMask = 0;
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    auto setThreadInformation = kernel32 ? reinterpret_cast<SetThreadInformation_t>(
                                               GetProcAddress(kernel32, "SetThreadInformation"))
                                         : nullptr;
    if (setThreadInformation)
    {
        setThreadInformation(GetCurrentThread(), ThreadPowerThrottling, &powerThrottling,
                             sizeof(powerThrottling));
    }
}

static void SetAnimationThreadActive(bool active)
{
    if (g_animationPriorityElevated == active)
    {
        return;
    }
    int priority = active ? THREAD_PRIORITY_ABOVE_NORMAL : THREAD_PRIORITY_NORMAL;
    if (SetThreadPriority(GetCurrentThread(), priority))
    {
        g_animationPriorityElevated = active;
    }
    else
    {
        Wh_Log(L"Failed to set animation thread priority: %u", GetLastError());
    }
}

static bool EnsureAnimationClockRunning(HWND hwnd)
{
    if (!g_animationTimer)
    {
        return false;
    }
    double preferredRate = GetPreferredAnimationRate(hwnd);
    if (g_animationClockArmed)
    {
        // Follow the newest interaction's monitor rate in both directions.
        if (std::fabs(preferredRate - g_animationTargetHz) > 0.5)
        {
            double previousRate = g_animationTargetHz;
            LARGE_INTEGER previousNextCounter = g_nextAnimationCounter;
            g_animationTargetHz = preferredRate;
            g_nextAnimationCounter = {};
            if (!ArmNextAnimationTick())
            {
                g_animationTargetHz = previousRate;
                g_nextAnimationCounter = previousNextCounter;
            }
        }
        return true;
    }
    SetAnimationThreadActive(true);
    g_animationTargetHz = preferredRate;
    g_nextAnimationCounter = {};
    if (!QueryPerformanceCounter(&g_lastAnimationCounter))
    {
        g_lastAnimationCounter = {};
    }
    g_animationClockArmed = true;
    if (!ArmNextAnimationTick())
    {
        DWORD timerError = GetLastError();
        g_animationClockArmed = false;
        g_animationTargetHz = 0.0;
        g_lastAnimationCounter = {};
        g_nextAnimationCounter = {};
        SetAnimationThreadActive(false);
        Wh_Log(L"Failed to arm animation timer: %u", timerError);
        return false;
    }
    return true;
}

static void ResetDragInputState()
{
    g_realDragging = false;
    g_realDraggedWindow = nullptr;
    g_pendingInteractiveSnapWindow.store(nullptr, std::memory_order_release);
    g_pendingInteractiveSnapFlags.store(0, std::memory_order_relaxed);
    g_realResizing = false;
    g_moveTypeKnown = false;
    g_dragResizeWobbleEnabled = true;
    g_dragStartedWindowZoomed = false;
    g_interactiveWindowStateThrob = false;
    g_interactiveWindowStateMaximizing = false;
    g_resizeCoordinateScale = 1.0;
    g_moveEventCounter = 0;
    g_dragAnimationSlot = -1;
    g_hasLastMousePosition = false;
    g_lastMousePosition = {};
    g_dragStartMousePosition = {};
    g_dragStartLocalMouse = {};
    g_dragCursorMonitor = nullptr;
    g_monitorTransitionRebaseUntil = 0;
    g_realDraggedWindowRect = {};
    g_lastDraggedWindowRect = {};
    g_lastDraggedWindowZoomed = false;
    g_finalizingMoveSize = false;
}

static bool IsInteractiveMoveSizeLoop(HWND hwnd)
{
    DWORD windowThreadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (windowThreadId != 0)
    {
        GUITHREADINFO guiThreadInfo = {};
        guiThreadInfo.cbSize = sizeof(guiThreadInfo);
        if (GetGUIThreadInfo(windowThreadId, &guiThreadInfo))
        {
            return (guiThreadInfo.flags & GUI_INMOVESIZE) != 0;
        }
    }
    // Don't mistake programmatic state changes for interactive moves.
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
}

struct MonitorEdgeState
{
    bool top;
    bool side;
    Vec2 direction;
};

static MonitorEdgeState GetPointMonitorEdgeState(const POINT& point)
{
    MonitorEdgeState result = {};
    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    if (!monitor)
    {
        return result;
    }
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo))
    {
        return result;
    }
    // Aero Snap uses physical monitor edges, with a small DPI tolerance.
    result.top = point.x >= monitorInfo.rcMonitor.left && point.x < monitorInfo.rcMonitor.right &&
                 point.y <= monitorInfo.rcMonitor.top + 3;
    if (result.top)
    {
        result.direction.y = -1.0;
    }
    bool atLeftEdge = point.y >= monitorInfo.rcMonitor.top &&
                      point.y < monitorInfo.rcMonitor.bottom &&
                      point.x <= monitorInfo.rcMonitor.left + 3;
    bool atRightEdge = point.y >= monitorInfo.rcMonitor.top &&
                       point.y < monitorInfo.rcMonitor.bottom &&
                       point.x >= monitorInfo.rcMonitor.right - 4;
    result.side = atLeftEdge || atRightEdge;
    if (atLeftEdge)
    {
        result.direction.x = -1.0;
    }
    else if (atRightEdge)
    {
        result.direction.x = 1.0;
    }
    return result;
}

static bool IsPointAtMonitorTopEdge(const POINT& point)
{
    MonitorEdgeState edgeState = GetPointMonitorEdgeState(point);
    // A corner belongs to a diagonal Snap zone, not to full-screen maximize.
    return edgeState.top && !edgeState.side;
}

static bool ResetMatrixTransformProxy(void* matrixTransformProxy)
{
    if (!IsOnDwmSceneThread() ||
        !IsDwmObjectPointerValid(matrixTransformProxy, g_matrixTransformProxyVtable) ||
        !HasMatrixTransformUpdate())
    {
        return false;
    }
    MilMatrix3x2D identityMatrix = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
    return UpdateMatrixTransformProxy(matrixTransformProxy, identityMatrix) >= 0;
}

static bool RequestFallbackWindowInvalidation(HWND hwnd)
{
    if (!hwnd)
    {
        return false;
    }
    constexpr ULONGLONG minimumIntervalMs = 100;
    ULONGLONG now = GetTickCount64();
    ULONGLONG previous =
        g_lastFallbackInvalidationTimestamp.load(std::memory_order_acquire);
    for (;;)
    {
        if (previous != 0 && now - previous < minimumIntervalMs)
        {
            return true;
        }
        if (g_lastFallbackInvalidationTimestamp.compare_exchange_weak(
                previous, now, std::memory_order_acq_rel,
                std::memory_order_acquire))
        {
            break;
        }
    }
    return RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE) != FALSE;
}

static bool PostPendingDwmSceneWake(HWND hwnd, bool forceRepost)
{
    if (g_unloading.load(std::memory_order_acquire) && !forceRepost)
    {
        return false;
    }
    if (!forceRepost)
    {
        bool expected = false;
        if (!g_sceneWakeScheduled.compare_exchange_strong(expected, true, std::memory_order_acq_rel,
                                                          std::memory_order_acquire))
        {
            return true;
        }
    }
    else
    {
        // Serial numbers make watchdog reposts idempotent.
        g_sceneWakeScheduled.store(true, std::memory_order_release);
    }
    void* manager = g_desktopManager.load(std::memory_order_acquire);
    DWORD sceneThreadId = g_dwmSceneThreadId.load(std::memory_order_acquire);
    if (!sceneThreadId && HasExactDwmVtableTrusted(manager, g_desktopManagerVtable) &&
        g_desktopManagerThreadIdOffset != SIZE_MAX)
    {
        BYTE* threadIdField = static_cast<BYTE*>(manager) + g_desktopManagerThreadIdOffset;
        if (IsReadableMemory(threadIdField, sizeof(DWORD)))
        {
            sceneThreadId = *reinterpret_cast<DWORD*>(threadIdField);
        }
    }
    if (sceneThreadId)
    {
        g_sceneWakeOutstanding.fetch_add(1, std::memory_order_acq_rel);
        if (PostThreadMessageW(sceneThreadId, WM_DWM_SCENE_WAKE,
                               DWM_SCENE_WAKE_WPARAM,
                               g_dwmSceneWakeToken.load(std::memory_order_acquire)))
        {
            g_sceneWakePostTimestamp.store(GetTickCount64(), std::memory_order_release);
            return true;
        }
        g_sceneWakeOutstanding.fetch_sub(1, std::memory_order_acq_rel);
    }
    if (RequestFallbackWindowInvalidation(hwnd))
    {
        g_sceneWakePostTimestamp.store(GetTickCount64(), std::memory_order_release);
        return true;
    }
    g_sceneWakeScheduled.store(false, std::memory_order_release);
    g_sceneWakePostTimestamp.store(0, std::memory_order_release);
    return false;
}

static void RequestDwmScenePass(HWND hwnd)
{
    if (g_unloading.load(std::memory_order_acquire))
    {
        return;
    }
    g_sceneRequestedSerial.fetch_add(1, std::memory_order_acq_rel);
    PostPendingDwmSceneWake(hwnd, false);
}

static void FinalizeRetiringSlots()
{
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        void* matrixTransformProxy = nullptr;
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& slot = g_animationSlots[i];
        if (slot.retiring && slot.hookUsers == 0 && slot.identityApplied &&
            (!slot.matrixTransformProxy || IsOnDwmSceneThread()))
        {
            matrixTransformProxy = slot.matrixTransformProxy;
            ULONGLONG generation = slot.generation;
            slot = {};
            slot.generation = generation;
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (matrixTransformProxy && IsOnDwmSceneThread())
        {
            ResetMatrixTransformProxy(matrixTransformProxy);
            if (g_cBaseObjectRelease)
            {
                // SetTransform(nullptr) isn't a valid detach operation on the
                // supported uDWM builds: it dereferences the transform argument.
                // Leave the retained transform at identity and release our ref.
                g_cBaseObjectRelease(matrixTransformProxy);
            }
        }
    }
}

static void AbandonAnimationSlotsAfterSceneStall(const wchar_t* reason)
{
    unsigned int abandonedSlots = 0;
    unsigned int retainedProxies = 0;
    unsigned int busySlots = 0;
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    for (WindowAnimationSlot& slot : g_animationSlots)
    {
        if (!slot.active && !slot.retiring)
        {
            continue;
        }
        if (slot.hookUsers != 0)
        {
            busySlots++;
            continue;
        }
        retainedProxies += slot.matrixTransformProxy != nullptr;
        ULONGLONG nextGeneration = slot.generation + 1;
        slot = {};
        slot.generation = nextGeneration;
        abandonedSlots++;
    }
    WakeAllConditionVariable(&g_animationSlotsCondition);
    ReleaseSRWLockExclusive(&g_animationSlotsLock);

    if (g_animationTimer)
    {
        CancelWaitableTimer(g_animationTimer);
    }
    g_animationClockArmed = false;
    g_animationTargetHz = 0.0;
    g_lastAnimationCounter = {};
    g_nextAnimationCounter = {};
    SetAnimationThreadActive(false);
    g_sceneWakeScheduled.store(false, std::memory_order_release);
    g_sceneWakePostTimestamp.store(0, std::memory_order_release);
    g_sceneWakeStallStartedAt.store(0, std::memory_order_release);
    g_sceneWakeRetryCount.store(0, std::memory_order_release);
    g_sceneWakeStalled.store(false, std::memory_order_release);
    g_sceneRecoveryCleanupPending.store(false, std::memory_order_release);
    g_sceneSubmittedSerial.store(g_sceneRequestedSerial.load(std::memory_order_acquire),
                                 std::memory_order_release);
    g_lastObservedScenePassCounter = g_scenePassCounter.load(std::memory_order_acquire);
    g_lastSceneProgressTimestamp = 0;
    ResetDragInputState();
    unsigned int totalRetainedProxies =
        g_abandonedProxyCount.fetch_add(retainedProxies, std::memory_order_acq_rel) +
        retainedProxies;
    Wh_Log(L"DWM scene work abandoned (%s): slots=%u retainedProxies=%u "
           L"totalRetainedProxies=%u busySlots=%u",
           reason, abandonedSlots, retainedProxies, totalRetainedProxies, busySlots);
}

static void BeginSceneStallCleanup(const wchar_t* reason)
{
    if (g_sceneRecoveryCleanupPending.exchange(true, std::memory_order_acq_rel))
    {
        return;
    }
    int slotsToRetire[MAX_ANIMATION_SLOTS] = {};
    int retireCount = 0;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        if (g_animationSlots[i].active)
        {
            slotsToRetire[retireCount++] = i;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < retireCount; i++)
    {
        RetireAnimationSlot(slotsToRetire[i]);
    }
    // Keep only a low-rate identity retry alive until a scene pass recovers.
    g_animationTargetHz = 10.0;
    g_nextAnimationCounter = {};
    Wh_Log(L"DWM scene stalled (%s): retiring %d slots for identity cleanup",
           reason, retireCount);
}

static void StopAnimationClockIfIdle()
{
    FinalizeRetiringSlots();
    if (HasAnyAnimationSlots())
    {
        return;
    }
    if (g_animationTimer)
    {
        CancelWaitableTimer(g_animationTimer);
    }
    g_animationClockArmed = false;
    g_animationTargetHz = 0.0;
    g_lastAnimationCounter = {};
    g_nextAnimationCounter = {};
    SetAnimationThreadActive(false);
    g_lastObservedScenePassCounter = g_scenePassCounter.load(std::memory_order_acquire);
    g_lastSceneProgressTimestamp = 0;
    g_sceneRecoveryCleanupPending.store(false, std::memory_order_release);
}

static void RetireAnimationSlot(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= MAX_ANIMATION_SLOTS)
    {
        return;
    }
    bool wasDragSlot = g_dragAnimationSlot == slotIndex;
    HWND sceneWakeWindow = nullptr;
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    WindowAnimationSlot& slot = g_animationSlots[slotIndex];
    if (slot.active)
    {
        slot.active = false;
        slot.retiring = true;
        slot.dragging = false;
        slot.freeStepPending = false;
        slot.transformAttached = false;
        slot.transitionTransformAttached = false;
        slot.boundTopLevelVisualProxy = nullptr;
        slot.boundTransitionVisualProxy = nullptr;
        slot.proxyCreationPending = false;
        if (slot.matrixTransformProxy)
        {
            slot.meshIdentityPending = true;
            slot.identityApplied = false;
            slot.meshRevision++;
            sceneWakeWindow = slot.hwnd;
        }
        else
        {
            slot.meshIdentityPending = false;
            slot.identityApplied = true;
        }
        slot.generation++;
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (sceneWakeWindow)
    {
        RequestDwmScenePass(sceneWakeWindow);
    }
    if (wasDragSlot)
    {
        ResetDragInputState();
    }
    StopAnimationClockIfIdle();
}

static int FindAnimationSlotForWindow(HWND hwnd)
{
    int slotIndex = -1;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        if (g_animationSlots[i].active && g_animationSlots[i].hwnd == hwnd)
        {
            slotIndex = i;
            break;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    return slotIndex;
}

static bool WaitForRetiringSlotForWindow(HWND hwnd)
{
    ULONGLONG deadline = GetTickCount64() + 50;
    for (;;)
    {
        FinalizeRetiringSlots();
        bool retiringWindowFound = false;
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
        {
            if (g_animationSlots[i].retiring && g_animationSlots[i].hwnd == hwnd)
            {
                retiringWindowFound = true;
                break;
            }
        }
        if (!retiringWindowFound)
        {
            ReleaseSRWLockExclusive(&g_animationSlotsLock);
            return true;
        }
        ULONGLONG now = GetTickCount64();
        if (now >= deadline)
        {
            ReleaseSRWLockExclusive(&g_animationSlotsLock);
            return false;
        }
        DWORD remaining = static_cast<DWORD>(deadline - now);
        BOOL awakened = SleepConditionVariableSRW(&g_animationSlotsCondition, &g_animationSlotsLock,
                                                  remaining, 0);
        DWORD waitError = awakened ? ERROR_SUCCESS : GetLastError();
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (!awakened && waitError == ERROR_TIMEOUT)
        {
            return false;
        }
    }
}

static int FindFreeAnimationSlot()
{
    FinalizeRetiringSlots();
    int slotIndex = -1;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        if (!g_animationSlots[i].active && !g_animationSlots[i].retiring)
        {
            slotIndex = i;
            break;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    return slotIndex;
}

static int EvictOldestSettlingSlot()
{
    int candidate = -1;
    ULONGLONG oldestOrder = ULLONG_MAX;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        const WindowAnimationSlot& slot = g_animationSlots[i];
        if (slot.active && !slot.dragging && slot.hookUsers == 0 && i != g_dragAnimationSlot &&
            slot.order < oldestOrder)
        {
            candidate = i;
            oldestOrder = slot.order;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    if (candidate >= 0)
    {
        RetireAnimationSlot(candidate);
    }
    return FindFreeAnimationSlot();
}

static bool CreateMatrixTransformProxy(void** matrixTransformProxy)
{
    *matrixTransformProxy = nullptr;
    void* compositor = g_dwmCompositor.load(std::memory_order_acquire);
    if (g_proxyCreationDisabled.load(std::memory_order_acquire) || !IsOnDwmSceneThread() ||
        !HasExactDwmVtableTrusted(compositor, g_compositorVtable) ||
        !g_createMatrixTransformProxy)
    {
        return false;
    }
    long result = g_createMatrixTransformProxy(compositor, matrixTransformProxy);
    bool validProxy = IsDwmObjectPointerValid(*matrixTransformProxy, g_matrixTransformProxyVtable);
    if (result >= 0 && validProxy)
    {
        return true;
    }
    if (*matrixTransformProxy)
    {
        if (validProxy)
        {
            g_cBaseObjectRelease(*matrixTransformProxy);
        }
        else if (result >= 0)
        {
            // A wrong factory type means the private ABI changed; stop using it.
            g_proxyCreationDisabled.store(true, std::memory_order_release);
        }
        *matrixTransformProxy = nullptr;
    }
    Wh_Log(L"Matrix transform creation failed: 0x%08X", static_cast<unsigned int>(result));
    return false;
}

static void EnsurePendingMatrixTransformProxies()
{
    if (!IsOnDwmSceneThread())
    {
        return;
    }
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        ULONGLONG generation = 0;
        HWND hwnd = nullptr;
        bool createProxy = false;
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& slot = g_animationSlots[i];
        if (slot.active && slot.proxyCreationPending && !slot.matrixTransformProxy)
        {
            slot.hookUsers++;
            generation = slot.generation;
            hwnd = slot.hwnd;
            createProxy = true;
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (!createProxy)
        {
            continue;
        }
        void* matrixTransformProxy = nullptr;
        bool created = CreateMatrixTransformProxy(&matrixTransformProxy);
        if (created)
        {
            // Test new proxies with identity before attaching them.
            created = ResetMatrixTransformProxy(matrixTransformProxy);
        }
        bool stored = false;
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& currentSlot = g_animationSlots[i];
        if (currentSlot.active && currentSlot.generation == generation &&
            currentSlot.proxyCreationPending && !currentSlot.matrixTransformProxy)
        {
            currentSlot.proxyCreationPending = false;
            if (created)
            {
                currentSlot.matrixTransformProxy = matrixTransformProxy;
                currentSlot.transformAttached = false;
                currentSlot.transitionTransformAttached = false;
                currentSlot.transformRebindRevision++;
                currentSlot.identityApplied = true;
                currentSlot.privateCallFailureCount = 0;
                stored = true;
            }
            else
            {
                currentSlot.active = false;
                currentSlot.retiring = true;
                currentSlot.dragging = false;
                currentSlot.freeStepPending = false;
                currentSlot.meshIdentityPending = false;
                currentSlot.identityApplied = true;
                currentSlot.generation++;
            }
        }
        if (currentSlot.hookUsers > 0)
        {
            currentSlot.hookUsers--;
            if (currentSlot.hookUsers == 0)
            {
                WakeAllConditionVariable(&g_animationSlotsCondition);
            }
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (matrixTransformProxy && !stored)
        {
            g_cBaseObjectRelease(matrixTransformProxy);
        }
        if (!created)
        {
            Wh_Log(L"DWM safety gate: matrix proxy canary failed for HWND=%p", hwnd);
        }
    }
}

static bool IsResizeHitTestResult(LRESULT hitTestResult)
{
    switch (hitTestResult)
    {
    case HTLEFT:
    case HTRIGHT:
    case HTTOP:
    case HTTOPLEFT:
    case HTTOPRIGHT:
    case HTBOTTOM:
    case HTBOTTOMLEFT:
    case HTBOTTOMRIGHT:
        return true;
    default:
        return false;
    }
}

static bool IsCursorOnResizableFrame(HWND hwnd, const POINT& mousePosition,
                                     const RECT& rect, bool includeTopEdge)
{
    if ((GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_THICKFRAME) == 0 || IsZoomed(hwnd))
    {
        return false;
    }
    UINT dpi = GetDpiForWindow(hwnd);
    if (dpi == 0)
    {
        dpi = USER_DEFAULT_SCREEN_DPI;
    }
    int frameX = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi) +
                 GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
    int frameY = GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) +
                 GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
    frameX = std::clamp(frameX, 4, 32);
    frameY = std::clamp(frameY, 4, 32);
    LONG localX = mousePosition.x - rect.left;
    LONG localY = mousePosition.y - rect.top;
    LONG width = rect.right - rect.left;
    LONG height = rect.bottom - rect.top;
    if (localX < -1 || localY < -1 || localX > width + 1 || localY > height + 1)
    {
        return false;
    }
    bool horizontalEdge = localX <= frameX || localX >= width - frameX;
    bool verticalEdge = localY >= height - frameY || (includeTopEdge && localY <= frameY);
    return horizontalEdge || verticalEdge;
}

static bool ClassifyMoveSizeOperation(HWND hwnd, const POINT& mousePosition, bool& resizing)
{
    // Restoring a maximized window under the cursor is still a move.
    if (IsZoomed(hwnd))
    {
        resizing = false;
        return true;
    }
    RECT rect = {};
    bool hasRect = GetWindowRect(hwnd, &rect) != FALSE;
    DWORD_PTR hitTestResult = HTNOWHERE;
    bool hasHitTest =
        SendMessageTimeoutW(hwnd, WM_NCHITTEST, 0,
                            MAKELPARAM(static_cast<short>(mousePosition.x),
                                       static_cast<short>(mousePosition.y)),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &hitTestResult) != FALSE;
    if (hasHitTest && IsResizeHitTestResult(static_cast<LRESULT>(hitTestResult)))
    {
        resizing = true;
        return true;
    }
    // Some custom frames report HTCAPTION for the invisible side/bottom border.
    // Top-edge caption grabs remain moves unless WM_NCHITTEST identified resize.
    if (hasRect && IsCursorOnResizableFrame(hwnd, mousePosition, rect, false))
    {
        resizing = true;
        return true;
    }
    if (hasHitTest && static_cast<LRESULT>(hitTestResult) == HTCAPTION)
    {
        resizing = false;
        return true;
    }
    // Custom title bars may need geometry-delta classification.
    return false;
}

static void InitializeDpiSupport()
{
    g_shcoreModule = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_shcoreModule)
    {
        g_getDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
            GetProcAddress(g_shcoreModule, "GetDpiForMonitor"));
    }
    g_dwmApiModule = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_dwmApiModule)
    {
        g_dwmGetWindowAttribute = reinterpret_cast<DwmGetWindowAttribute_t>(
            GetProcAddress(g_dwmApiModule, "DwmGetWindowAttribute"));
    }
}

static void UninitializeDpiSupport()
{
    g_getDpiForMonitor = nullptr;
    if (g_shcoreModule)
    {
        FreeLibrary(g_shcoreModule);
        g_shcoreModule = nullptr;
    }
    g_dwmGetWindowAttribute = nullptr;
    if (g_dwmApiModule)
    {
        FreeLibrary(g_dwmApiModule);
        g_dwmApiModule = nullptr;
    }
}

static double GetResizeVisualCoordinateScale(HWND hwnd, const RECT& windowRect)
{
    if (!hwnd || !g_getDpiForMonitor)
    {
        return 1.0;
    }
    HMONITOR monitor = MonitorFromRect(&windowRect, MONITOR_DEFAULTTONEAREST);
    UINT monitorDpiX = 0;
    UINT monitorDpiY = 0;
    if (!monitor ||
        FAILED(g_getDpiForMonitor(monitor,
                                  0, // MDT_EFFECTIVE_DPI
                                  &monitorDpiX, &monitorDpiY)) ||
        monitorDpiX == 0)
    {
        return 1.0;
    }
    UINT windowDpi = GetDpiForWindow(hwnd);
    if (windowDpi == 0)
    {
        return 1.0;
    }
    // Correct system-DPI surfaces transformed by DWM on another monitor.
    return std::clamp(static_cast<double>(windowDpi) / static_cast<double>(monitorDpiX), 0.50,
                      2.00);
}

static int AcquireAnimationSlot(HWND hwnd, const WobblySettings& settings, double width,
                                double height, const Vec2& mousePosition)
{
    if (g_dragAnimationSlot >= 0)
    {
        RetireAnimationSlot(g_dragAnimationSlot);
    }
    int slotIndex = FindAnimationSlotForWindow(hwnd);
    if (slotIndex >= 0)
    {
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& slot = g_animationSlots[slotIndex];
        bool reused = slot.active && slot.hwnd == hwnd;
        if (reused)
        {
            slot.settings = settings;
            bool resetStateThrob = slot.windowStateThrob;
            bool resetResizeDecay = slot.mesh.resizing && !slot.dragging;
            if (resetStateThrob || resetResizeDecay)
            {
                InitializeMesh(slot.mesh, width, height);
                slot.windowStateThrob = false;
                if (resetStateThrob)
                {
                    slot.boundTransitionVisualProxy = nullptr;
                    slot.transitionTransformAttached = false;
                }
            }
            BeginDrag(slot.mesh, mousePosition);
            slot.previousMesh = slot.mesh;
            slot.dragging = true;
            slot.freeStepPending = false;
            slot.nextWindowValidation = GetTickCount64() + 250;
            slot.nextVisualValidation = 0;
            slot.order = ++g_animationOrderCounter;
            slot.identityApplied = false;
            slot.meshIdentityPending = false;
            slot.meshRevision++;
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (reused)
        {
            return slotIndex;
        }
    }
    // Revive an unpinned retiring slot when the same window is re-grabbed.
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        WindowAnimationSlot& slot = g_animationSlots[i];
        if (!slot.retiring || slot.hwnd != hwnd || slot.hookUsers != 0)
        {
            continue;
        }
        slot.active = true;
        slot.retiring = false;
        slot.dragging = true;
        slot.freeStepPending = false;
        slot.generation++;
        slot.settings = settings;
        bool resetStateThrob = slot.windowStateThrob;
        if (resetStateThrob || slot.mesh.resizing)
        {
            InitializeMesh(slot.mesh, width, height);
            slot.windowStateThrob = false;
        }
        BeginDrag(slot.mesh, mousePosition);
        slot.previousMesh = slot.mesh;
        slot.transformAttached = false;
        slot.transitionTransformAttached = false;
        slot.boundTopLevelVisualProxy = nullptr;
        slot.boundTransitionVisualProxy = nullptr;
        slot.transformRebindRevision++;
        slot.submittedTransformRebindRevision = 0;
        slot.meshIdentityPending = false;
        slot.proxyCreationPending = slot.matrixTransformProxy == nullptr;
        slot.identityApplied = false;
        slot.nextWindowValidation = GetTickCount64() + 250;
        slot.nextVisualValidation = 0;
        slot.order = ++g_animationOrderCounter;
        slot.meshRevision++;
        slotIndex = i;
        break;
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (slotIndex >= 0)
    {
        return slotIndex;
    }
    WobbleMesh mesh = {};
    InitializeMesh(mesh, width, height);
    BeginDrag(mesh, mousePosition);
    if (!WaitForRetiringSlotForWindow(hwnd))
    {
        Wh_Log(L"Previous animation hook is still active for HWND=%p", hwnd);
        return -1;
    }
    slotIndex = FindFreeAnimationSlot();
    if (slotIndex < 0)
    {
        slotIndex = EvictOldestSettlingSlot();
    }
    if (slotIndex < 0)
    {
        Wh_Log(L"Animation buffer is full");
        return -1;
    }
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    WindowAnimationSlot& slot = g_animationSlots[slotIndex];
    slot.active = true;
    slot.retiring = false;
    slot.dragging = true;
    slot.freeStepPending = false;
    slot.windowStateThrob = false;
    slot.hookUsers = 0;
    slot.generation++;
    slot.order = ++g_animationOrderCounter;
    slot.hwnd = hwnd;
    slot.settings = settings;
    slot.previousMesh = mesh;
    slot.mesh = mesh;
    slot.matrixTransformProxy = nullptr;
    slot.boundTopLevelVisualProxy = nullptr;
    slot.boundTransitionVisualProxy = nullptr;
    slot.transformAttached = false;
    slot.transitionTransformAttached = false;
    slot.transformRebindRevision = 1;
    slot.submittedTransformRebindRevision = 0;
    slot.meshRevision = 1;
    slot.submittedMeshRevision = 0;
    slot.meshIdentityPending = false;
    slot.proxyCreationPending = true;
    slot.nextWindowValidation = GetTickCount64() + 250;
    slot.nextVisualValidation = 0;
    slot.identityApplied = false;
    slot.lastMatrixErrorLog = 0;
    slot.privateCallFailureCount = 0;
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    return slotIndex;
}

static void HandleMoveSizeStart(HWND hwnd)
{
    if (!hwnd)
    {
        return;
    }
    if (!IsWindow(hwnd))
    {
        return;
    }
    if (!IsWindowVisible(hwnd))
    {
        return;
    }
    if (!IsInteractiveMoveSizeLoop(hwnd))
    {
        return;
    }
    // Reset Snap state so the same side can trigger again.
    ResetObservedSnapStateForInteractiveMove(hwnd);
    g_pendingInteractiveSnapWindow.store(nullptr, std::memory_order_release);
    g_pendingInteractiveSnapFlags.store(0, std::memory_order_relaxed);
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect))
    {
        return;
    }
    POINT mousePosition = {};
    if (!GetCursorPos(&mousePosition))
    {
        return;
    }
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0)
    {
        return;
    }
    WobblySettings activeSettings = GetSettingsSnapshot();
    Vec2 localMousePosition = {static_cast<double>(mousePosition.x - rect.left),
                               static_cast<double>(mousePosition.y - rect.top)};
    bool operationResizing = false;
    bool operationTypeKnown = ClassifyMoveSizeOperation(hwnd, mousePosition, operationResizing);
    if (operationTypeKnown && operationResizing && !activeSettings.resizeWobbleEnabled)
    {
        return;
    }
    g_resizeCoordinateScale =
        operationTypeKnown && operationResizing ? GetResizeVisualCoordinateScale(hwnd, rect) : 1.0;
    if (operationTypeKnown && operationResizing)
    {
        localMousePosition.x *= g_resizeCoordinateScale;
        localMousePosition.y *= g_resizeCoordinateScale;
    }
    double meshWidth = static_cast<double>(width) * g_resizeCoordinateScale;
    double meshHeight = static_cast<double>(height) * g_resizeCoordinateScale;
    int slotIndex =
        AcquireAnimationSlot(hwnd, activeSettings, meshWidth, meshHeight, localMousePosition);
    if (slotIndex < 0)
    {
        return;
    }
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    WindowAnimationSlot& dragSlot = g_animationSlots[slotIndex];
    if (dragSlot.active && dragSlot.hwnd == hwnd)
    {
        SetMeshResizeMode(dragSlot.mesh, operationTypeKnown && operationResizing);
        dragSlot.previousMesh = dragSlot.mesh;
        // A new move loop may replace the visual transform.
        dragSlot.transformRebindRevision++;
        dragSlot.meshRevision++;
        dragSlot.identityApplied = false;
        dragSlot.meshIdentityPending = false;
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    g_dragAnimationSlot = slotIndex;
    g_realDraggedWindow = hwnd;
    g_realDraggedWindowRect = rect;
    g_lastDraggedWindowRect = rect;
    g_lastDraggedWindowZoomed = IsZoomed(hwnd) != FALSE || IsApproximatelyMonitorWorkArea(rect);
    g_dragStartedWindowZoomed = g_lastDraggedWindowZoomed;
    g_interactiveWindowStateThrob = false;
    g_interactiveWindowStateMaximizing = false;
    g_dragStartMousePosition = mousePosition;
    g_lastMousePosition = mousePosition;
    g_dragCursorMonitor = MonitorFromPoint(mousePosition, MONITOR_DEFAULTTONEAREST);
    g_monitorTransitionRebaseUntil = 0;
    g_hasLastMousePosition = true;
    g_dragStartLocalMouse = localMousePosition;
    g_realDragging = true;
    g_realResizing = operationTypeKnown && operationResizing;
    g_moveTypeKnown = operationTypeKnown;
    g_dragResizeWobbleEnabled = activeSettings.resizeWobbleEnabled;
    g_moveEventCounter = 0;
    if (!EnsureAnimationClockRunning(hwnd))
    {
        Wh_Log(L"Failed to start drag animation timer");
        RetireAnimationSlot(slotIndex);
        return;
    }
    RequestDwmScenePass(hwnd);
    int dragPointIndex = -1;
    AcquireSRWLockShared(&g_animationSlotsLock);
    if (g_animationSlots[slotIndex].active)
    {
        dragPointIndex = g_animationSlots[slotIndex].mesh.dragPointIndex;
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    Wh_Log(L"MOVE/SIZE START HWND=%p "
           L"Type=%s "
           L"Size=%dx%d "
           L"ResizeCoordinateScale=%.4f "
           L"MouseLocal=(%.2f,%.2f) "
           L"MeshPoint=%d",
           hwnd, g_moveTypeKnown ? (g_realResizing ? L"RESIZE" : L"MOVE") : L"PENDING", width,
           height, g_resizeCoordinateScale, g_dragStartLocalMouse.x, g_dragStartLocalMouse.y,
           dragPointIndex);
}

static void ApplyAnimationSlotTransform(int slotIndex, double interpolationAlpha)
{
    if (!IsOnDwmSceneThread() || slotIndex < 0 || slotIndex >= MAX_ANIMATION_SLOTS)
    {
        return;
    }
    struct RenderSnapshot
    {
        ULONGLONG generation;
        ULONGLONG meshRevision;
        HWND hwnd;
        void* matrixTransformProxy;
        WobbleMesh mesh;
        Vec2 previousPositions[GRID_POINT_COUNT];
    } snapshot = {};
    bool identityUpdate = false;
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    WindowAnimationSlot& slot = g_animationSlots[slotIndex];
    // The slot owns its verified proxy until scene-thread retirement.
    if ((slot.active || slot.retiring) && slot.matrixTransformProxy &&
        HasMatrixTransformUpdate() &&
        (slot.retiring || slot.meshIdentityPending ||
         slot.meshRevision != slot.submittedMeshRevision))
    {
        snapshot.generation = slot.generation;
        snapshot.meshRevision = slot.meshRevision;
        snapshot.hwnd = slot.hwnd;
        snapshot.matrixTransformProxy = slot.matrixTransformProxy;
        snapshot.mesh = slot.mesh;
        for (int i = 0; i < GRID_POINT_COUNT; i++)
        {
            snapshot.previousPositions[i] = slot.previousMesh.points[i].position;
        }
        identityUpdate = slot.retiring || slot.meshIdentityPending || !slot.mesh.active;
        // Pin the slot while calling its proxy outside the lock.
        slot.hookUsers++;
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (!snapshot.matrixTransformProxy)
    {
        return;
    }
    auto finishUpdate = [&](long updateResult, bool appliedIdentity)
    {
        ULONGLONG now = GetTickCount64();
        bool logFailure = false;
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& currentSlot = g_animationSlots[slotIndex];
        if (currentSlot.generation == snapshot.generation &&
            currentSlot.matrixTransformProxy == snapshot.matrixTransformProxy)
        {
            if (updateResult >= 0)
            {
                currentSlot.privateCallFailureCount = 0;
                currentSlot.submittedMeshRevision = snapshot.meshRevision;
                currentSlot.identityApplied = appliedIdentity;
                if (appliedIdentity && currentSlot.meshRevision == snapshot.meshRevision)
                {
                    currentSlot.meshIdentityPending = false;
                }
            }
            else
            {
                currentSlot.privateCallFailureCount++;
                if (now - currentSlot.lastMatrixErrorLog >= 1000)
                {
                    currentSlot.lastMatrixErrorLog = now;
                    logFailure = true;
                }
            }
        }
        if (currentSlot.hookUsers > 0)
        {
            currentSlot.hookUsers--;
        }
        if (currentSlot.hookUsers == 0)
        {
            WakeAllConditionVariable(&g_animationSlotsCondition);
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        if (logFailure)
        {
            Wh_Log(L"Matrix update failed for HWND=%p: 0x%08X", snapshot.hwnd,
                   static_cast<unsigned int>(updateResult));
        }
    };
    if (identityUpdate)
    {
        MilMatrix3x2D identityMatrix = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
        long updateResult =
            UpdateMatrixTransformProxy(snapshot.matrixTransformProxy, identityMatrix);
        finishUpdate(updateResult, true);
        return;
    }
    if (snapshot.mesh.width <= 0.0 || snapshot.mesh.height <= 0.0)
    {
        finishUpdate(E_INVALIDARG, false);
        return;
    }
    interpolationAlpha = std::clamp(interpolationAlpha, 0.0, 1.0);
    Vec2 renderedPositions[GRID_POINT_COUNT] = {};
    Vec2 renderedCenter = {};
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        const Vec2& previousPosition = snapshot.previousPositions[i];
        const Vec2& currentPosition = snapshot.mesh.points[i].position;
        renderedPositions[i] = {Lerp(previousPosition.x, currentPosition.x, interpolationAlpha),
                                Lerp(previousPosition.y, currentPosition.y, interpolationAlpha)};
        renderedCenter.x += renderedPositions[i].x;
        renderedCenter.y += renderedPositions[i].y;
    }
    renderedCenter.x /= static_cast<double>(GRID_POINT_COUNT);
    renderedCenter.y /= static_cast<double>(GRID_POINT_COUNT);
    Vec2 baseCenter = {snapshot.mesh.width * 0.5, snapshot.mesh.height * 0.5};
    double denominatorX = 0.0;
    double denominatorY = 0.0;
    double numeratorXX = 0.0;
    double numeratorXY = 0.0;
    double numeratorYX = 0.0;
    double numeratorYY = 0.0;
    for (int i = 0; i < GRID_POINT_COUNT; i++)
    {
        const Vec2& basePosition = snapshot.mesh.points[i].basePosition;
        double baseX = basePosition.x - baseCenter.x;
        double baseY = basePosition.y - baseCenter.y;
        double renderedX = renderedPositions[i].x - renderedCenter.x;
        double renderedY = renderedPositions[i].y - renderedCenter.y;
        denominatorX += baseX * baseX;
        denominatorY += baseY * baseY;
        numeratorXX += baseX * renderedX;
        numeratorXY += baseX * renderedY;
        numeratorYX += baseY * renderedX;
        numeratorYY += baseY * renderedY;
    }
    double m11 = denominatorX > 0.0001 ? numeratorXX / denominatorX : 1.0;
    double m12 = denominatorX > 0.0001 ? numeratorXY / denominatorX : 0.0;
    double m21 = denominatorY > 0.0001 ? numeratorYX / denominatorY : 0.0;
    double m22 = denominatorY > 0.0001 ? numeratorYY / denominatorY : 1.0;
    // Compress extreme affine values without flattening their response.
    m11 = SmoothCenteredCompression(m11, 1.0, 0.30);
    m12 = SmoothSymmetricCompression(m12, 0.35);
    m21 = SmoothSymmetricCompression(m21, 0.35);
    m22 = SmoothCenteredCompression(m22, 1.0, 0.30);
    // Remote corruption guards; normal motion stays well inside them.
    m11 = std::clamp(m11, 0.10, 2.00);
    m12 = std::clamp(m12, -1.25, 1.25);
    m21 = std::clamp(m21, -1.25, 1.25);
    m22 = std::clamp(m22, 0.10, 2.00);
    Vec2 translationBase = baseCenter;
    Vec2 translationRendered = renderedCenter;
    bool useSurfacePivot = false;
    if (snapshot.mesh.resizing)
    {
        // Anchor affine resize to the stationary opposite edge.
        if (snapshot.mesh.canWobbleLeft && !snapshot.mesh.canWobbleRight)
        {
            translationBase.x = snapshot.mesh.width;
        }
        else if (snapshot.mesh.canWobbleRight && !snapshot.mesh.canWobbleLeft)
        {
            translationBase.x = 0.0;
        }
        if (snapshot.mesh.canWobbleTop && !snapshot.mesh.canWobbleBottom)
        {
            translationBase.y = snapshot.mesh.height;
        }
        else if (snapshot.mesh.canWobbleBottom && !snapshot.mesh.canWobbleTop)
        {
            translationBase.y = 0.0;
        }
        useSurfacePivot = true;
    }
    else if (snapshot.mesh.dragPointIndex >= 0 && snapshot.mesh.dragPointIndex < GRID_POINT_COUNT)
    {
        const WobblePoint& dragPoint = snapshot.mesh.points[snapshot.mesh.dragPointIndex];
        translationBase = {dragPoint.basePosition.x + snapshot.mesh.dragOffset.x,
                           dragPoint.basePosition.y + snapshot.mesh.dragOffset.y};
        useSurfacePivot = true;
    }
    if (useSurfacePivot)
    {
        double basisX[4] = {};
        double basisY[4] = {};
        CalculateBernsteinBasis(std::clamp(translationBase.x / snapshot.mesh.width, 0.0, 1.0),
                                basisX);
        CalculateBernsteinBasis(std::clamp(translationBase.y / snapshot.mesh.height, 0.0, 1.0),
                                basisY);
        translationRendered = {};
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            for (int x = 0; x < GRID_WIDTH; x++)
            {
                double weight = basisX[x] * basisY[y];
                const Vec2& position = renderedPositions[GetPointIndex(x, y)];
                translationRendered.x += position.x * weight;
                translationRendered.y += position.y * weight;
            }
        }
    }
    double translationX = translationRendered.x - m11 * translationBase.x - m21 * translationBase.y;
    double translationY = translationRendered.y - m12 * translationBase.x - m22 * translationBase.y;
    // Keep the grab/resize pivot attached, with only a corruption guard.
    double emergencyTranslationLimit =
        std::max(4096.0, std::max(snapshot.mesh.width, snapshot.mesh.height) * 4.0);
    if (!std::isfinite(translationX))
    {
        translationX = 0.0;
    }
    if (!std::isfinite(translationY))
    {
        translationY = 0.0;
    }
    translationX = std::clamp(translationX, -emergencyTranslationLimit, emergencyTranslationLimit);
    translationY = std::clamp(translationY, -emergencyTranslationLimit, emergencyTranslationLimit);
    MilMatrix3x2D matrix = {m11, m12, m21, m22, translationX, translationY};
    long updateResult = UpdateMatrixTransformProxy(snapshot.matrixTransformProxy, matrix);
    finishUpdate(updateResult, false);
}

static void StopAllAnimations()
{
    int slotsToRetire[MAX_ANIMATION_SLOTS] = {};
    HWND initialSceneWakeWindow = nullptr;
    int retireCount = 0;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        if (g_animationSlots[i].active)
        {
            slotsToRetire[retireCount] = i;
            if (!initialSceneWakeWindow)
            {
                initialSceneWakeWindow = g_animationSlots[i].hwnd;
            }
            retireCount++;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < retireCount; i++)
    {
        RetireAnimationSlot(slotsToRetire[i]);
    }
    // Repost the existing cleanup serial instead of creating a moving target.
    if (initialSceneWakeWindow)
    {
        PostPendingDwmSceneWake(initialSceneWakeWindow, true);
    }
    ULONGLONG hookWaitDeadline = GetTickCount64() + DWM_UNLOAD_CLEANUP_TIMEOUT_MS;
    bool cleanupTimedOut = false;
    for (;;)
    {
        bool cleanupPending = false;
        HWND sceneWakeWindow = nullptr;
        AcquireSRWLockShared(&g_animationSlotsLock);
        for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
        {
            if (g_animationSlots[i].hookUsers != 0 ||
                (g_animationSlots[i].retiring && g_animationSlots[i].matrixTransformProxy))
            {
                cleanupPending = true;
            }
            if (!sceneWakeWindow && g_animationSlots[i].retiring &&
                g_animationSlots[i].matrixTransformProxy)
            {
                sceneWakeWindow = g_animationSlots[i].hwnd;
            }
        }
        ReleaseSRWLockShared(&g_animationSlotsLock);
        bool wakePending = g_sceneWakeOutstanding.load(std::memory_order_acquire) != 0;
        if (!cleanupPending && !wakePending)
        {
            break;
        }
        ULONGLONG now = GetTickCount64();
        if (now >= hookWaitDeadline)
        {
            Wh_Log(L"Timed out waiting for DWM animation hooks");
            cleanupTimedOut = true;
            break;
        }
        if (sceneWakeWindow && !wakePending)
        {
            PostPendingDwmSceneWake(sceneWakeWindow, true);
        }
        Sleep(16);
    }
    FinalizeRetiringSlots();
    if (cleanupTimedOut)
    {
        // Off-scene private calls are unsafe. Drop bookkeeping and intentionally
        // retain any unresolved DWM proxies instead of blocking unload forever.
        AbandonAnimationSlotsAfterSceneStall(L"cleanup timeout");
    }
    if (g_animationTimer)
    {
        CancelWaitableTimer(g_animationTimer);
    }
    g_animationClockArmed = false;
    g_animationTargetHz = 0.0;
    g_lastAnimationCounter = {};
    g_nextAnimationCounter = {};
    SetAnimationThreadActive(false);
    ResetDragInputState();
}

static void UpdateAnimationFrame()
{
    if (g_sceneOwnershipResetPending.exchange(false, std::memory_order_acq_rel))
    {
        ResetDragInputState();
    }
    FinalizeRetiringSlots();
    if (!HasAnyAnimationSlots())
    {
        StopAnimationClockIfIdle();
        return;
    }
    ULONGLONG now = GetTickCount64();
    LARGE_INTEGER currentCounter = {};
    if (g_animationFrequency.QuadPart <= 0 || !QueryPerformanceCounter(&currentCounter))
    {
        if (!ArmNextAnimationTick())
        {
            StopAllAnimations();
        }
        return;
    }
    double elapsed = 0.0;
    if (g_lastAnimationCounter.QuadPart == 0)
    {
        g_lastAnimationCounter = currentCounter;
    }
    else
    {
        elapsed = static_cast<double>(currentCounter.QuadPart - g_lastAnimationCounter.QuadPart) /
                  static_cast<double>(g_animationFrequency.QuadPart);
        g_lastAnimationCounter = currentCounter;
        if (!std::isfinite(elapsed) || elapsed < 0.0)
        {
            elapsed = 0.0;
        }
        else if (elapsed > 1.0)
        {
            // Start a new clock epoch after suspend/resume.
            elapsed = 0.0;
        }
    }
    double frameMilliseconds = elapsed * 1000.0;
    // Sample dragged geometry at animation cadence, not WinEvent cadence.
    if (g_realDragging.load(std::memory_order_relaxed))
    {
        HWND draggedWindow = g_realDraggedWindow.load(std::memory_order_relaxed);
        if (draggedWindow)
        {
            HandleLocationChange(draggedWindow, OBJID_WINDOW, CHILDID_SELF);
        }
    }
    HWND sceneWakeWindow = nullptr;
    // Publish only fully integrated local mesh snapshots.
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        struct PhysicsSnapshot
        {
            bool active;
            bool dragging;
            bool freeStepPending;
            bool identityApplied;
            ULONGLONG generation;
            HWND hwnd;
            WobblySettings settings;
            WobbleMesh mesh;
        } snapshot = {};
        AcquireSRWLockShared(&g_animationSlotsLock);
        if (g_animationSlots[i].active)
        {
            const WindowAnimationSlot& slot = g_animationSlots[i];
            snapshot.active = true;
            snapshot.dragging = slot.dragging;
            snapshot.freeStepPending = slot.freeStepPending;
            snapshot.identityApplied = slot.identityApplied;
            snapshot.generation = slot.generation;
            snapshot.hwnd = slot.hwnd;
            snapshot.settings = slot.settings;
            snapshot.mesh = slot.mesh;
        }
        ReleaseSRWLockShared(&g_animationSlotsLock);
        if (!snapshot.active ||
            (!snapshot.dragging && !snapshot.mesh.active && !snapshot.freeStepPending) ||
            frameMilliseconds <= 0.0)
        {
            continue;
        }
        WobbleMesh simulatedMesh = snapshot.mesh;
        WobbleMesh previousMesh = snapshot.mesh;
        double remainingMilliseconds = frameMilliseconds;
        bool simulated = false;
        bool freeStepPending = snapshot.freeStepPending;
        while (remainingMilliseconds > 0.0 &&
               (snapshot.dragging || simulatedMesh.active || freeStepPending))
        {
            double stepMilliseconds = std::min(remainingMilliseconds, 10.0);
            previousMesh = simulatedMesh;
            MeshStepResult result =
                SimulateMeshStep(simulatedMesh, snapshot.settings, stepMilliseconds / 1000.0);
            simulated = true;
            freeStepPending = false;
            remainingMilliseconds -= stepMilliseconds;
            // Match KWin's free-effect stop threshold.
            if (!snapshot.dragging && !result.wobblying)
            {
                break;
            }
        }
        if (!simulated)
        {
            continue;
        }
        AcquireSRWLockExclusive(&g_animationSlotsLock);
        WindowAnimationSlot& currentSlot = g_animationSlots[i];
        if (currentSlot.active && currentSlot.generation == snapshot.generation)
        {
            bool renderRevisionNeeded =
                simulatedMesh.active || snapshot.mesh.active != simulatedMesh.active;
            currentSlot.previousMesh = previousMesh;
            currentSlot.mesh = simulatedMesh;
            currentSlot.freeStepPending = false;
            if (renderRevisionNeeded)
            {
                currentSlot.meshRevision++;
                if ((currentSlot.matrixTransformProxy || currentSlot.proxyCreationPending) &&
                    !sceneWakeWindow)
                {
                    sceneWakeWindow = currentSlot.hwnd;
                }
            }
            if (simulatedMesh.active)
            {
                currentSlot.identityApplied = false;
                currentSlot.meshIdentityPending = false;
            }
            else if (currentSlot.dragging && (snapshot.mesh.active || !snapshot.identityApplied))
            {
                // Keep quiet grabbed state but render identity.
                currentSlot.meshIdentityPending = true;
                currentSlot.identityApplied = false;
            }
        }
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
    }
    int slotsToRetire[MAX_ANIMATION_SLOTS] = {};
    int retireCount = 0;
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        struct RetirementSnapshot
        {
            bool active;
            bool dragging;
            bool meshActive;
            bool freeStepPending;
            bool identityApplied;
            bool proxyCreationPending;
            ULONGLONG generation;
            ULONGLONG nextWindowValidation;
            HWND hwnd;
            void* matrixTransformProxy;
        } snapshot = {};
        AcquireSRWLockShared(&g_animationSlotsLock);
        if (g_animationSlots[i].active)
        {
            const WindowAnimationSlot& slot = g_animationSlots[i];
            snapshot.active = true;
            snapshot.dragging = slot.dragging;
            snapshot.meshActive = slot.mesh.active;
            snapshot.freeStepPending = slot.freeStepPending;
            snapshot.identityApplied = slot.identityApplied;
            snapshot.proxyCreationPending = slot.proxyCreationPending;
            snapshot.generation = slot.generation;
            snapshot.nextWindowValidation = slot.nextWindowValidation;
            snapshot.hwnd = slot.hwnd;
            snapshot.matrixTransformProxy = slot.matrixTransformProxy;
        }
        ReleaseSRWLockShared(&g_animationSlotsLock);
        if (!snapshot.active)
        {
            continue;
        }
        bool invalidWindow = !snapshot.hwnd;
        if (!invalidWindow && now >= snapshot.nextWindowValidation)
        {
            invalidWindow = !IsWindow(snapshot.hwnd);
            AcquireSRWLockExclusive(&g_animationSlotsLock);
            WindowAnimationSlot& currentSlot = g_animationSlots[i];
            if (currentSlot.active && currentSlot.generation == snapshot.generation)
            {
                currentSlot.nextWindowValidation = now + 250;
            }
            ReleaseSRWLockExclusive(&g_animationSlotsLock);
        }
        if (invalidWindow ||
            (!snapshot.dragging && !snapshot.meshActive && !snapshot.freeStepPending))
        {
            slotsToRetire[retireCount++] = i;
            continue;
        }
        if (!snapshot.meshActive)
        {
            if (!snapshot.identityApplied)
            {
                AcquireSRWLockExclusive(&g_animationSlotsLock);
                WindowAnimationSlot& currentSlot = g_animationSlots[i];
                if (currentSlot.active && currentSlot.generation == snapshot.generation &&
                    !currentSlot.meshIdentityPending)
                {
                    currentSlot.meshIdentityPending = true;
                    currentSlot.identityApplied = false;
                    currentSlot.meshRevision++;
                    if ((snapshot.matrixTransformProxy || snapshot.proxyCreationPending) &&
                        !sceneWakeWindow)
                    {
                        sceneWakeWindow = currentSlot.hwnd;
                    }
                }
                ReleaseSRWLockExclusive(&g_animationSlotsLock);
            }
            continue;
        }
    }
    if (!sceneWakeWindow)
    {
        AcquireSRWLockShared(&g_animationSlotsLock);
        for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
        {
            const WindowAnimationSlot& slot = g_animationSlots[i];
            if (slot.retiring && slot.matrixTransformProxy && !slot.identityApplied)
            {
                sceneWakeWindow = slot.hwnd;
                break;
            }
        }
        ReleaseSRWLockShared(&g_animationSlotsLock);
    }
    if (sceneWakeWindow)
    {
        ULONGLONG scenePassCounter = g_scenePassCounter.load(std::memory_order_acquire);
        if (g_lastSceneProgressTimestamp == 0 || scenePassCounter != g_lastObservedScenePassCounter)
        {
            g_lastObservedScenePassCounter = scenePassCounter;
            g_lastSceneProgressTimestamp = now;
        }
        if (now - g_lastSceneProgressTimestamp >= DWM_SCENE_STALL_TIMEOUT_MS)
        {
            BeginSceneStallCleanup(L"no scene progress");
        }
        RequestDwmScenePass(sceneWakeWindow);
        ULONGLONG lastWakePost = g_sceneWakePostTimestamp.load(std::memory_order_acquire);
        ULONGLONG lastSceneActivity = std::max(g_lastSceneProgressTimestamp, lastWakePost);
        if (sceneWakeWindow && now - lastSceneActivity >= 50)
        {
            if (!g_sceneWakeStalled.exchange(true, std::memory_order_acq_rel))
            {
                g_sceneWakeStallStartedAt.store(now, std::memory_order_release);
                Wh_Log(L"DWM scene wake stalled: requested=%llu "
                       L"submitted=%llu; reposting",
                       g_sceneRequestedSerial.load(std::memory_order_acquire),
                       g_sceneSubmittedSerial.load(std::memory_order_acquire));
            }
            g_sceneWakeRetryCount.fetch_add(1, std::memory_order_acq_rel);
            PostPendingDwmSceneWake(sceneWakeWindow, true);
        }
    }
    for (int i = 0; i < retireCount; i++)
    {
        RetireAnimationSlot(slotsToRetire[i]);
    }
    FinalizeRetiringSlots();
    StopAnimationClockIfIdle();
    if (g_animationClockArmed && !ArmNextAnimationTick())
    {
        Wh_Log(L"Failed to schedule animation frame: %u", GetLastError());
        StopAllAnimations();
    }
}

static void HandleLocationChange(HWND hwnd, LONG idObject, LONG idChild)
{
    HWND draggedWindow = g_realDraggedWindow.load(std::memory_order_relaxed);
    if (!g_realDragging.load(std::memory_order_relaxed) || !draggedWindow ||
        hwnd != draggedWindow || g_dragAnimationSlot < 0 ||
        g_dragAnimationSlot >= MAX_ANIMATION_SLOTS || idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF)
    {
        return;
    }
    int slotIndex = g_dragAnimationSlot;
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect))
    {
        return;
    }
    int originalWidth = g_realDraggedWindowRect.right - g_realDraggedWindowRect.left;
    int originalHeight = g_realDraggedWindowRect.bottom - g_realDraggedWindowRect.top;
    int currentWidth = rect.right - rect.left;
    int currentHeight = rect.bottom - rect.top;
    if (currentWidth <= 0 || currentHeight <= 0)
    {
        return;
    }
    bool rectChanged = !EqualRect(&rect, &g_lastDraggedWindowRect);
    bool resizeDetectedThisEvent = false;
    bool operationDetectedThisEvent = false;
    if (!g_moveTypeKnown && rectChanged)
    {
        g_realResizing = currentWidth != originalWidth || currentHeight != originalHeight;
        if (g_realResizing)
        {
            g_resizeCoordinateScale = GetResizeVisualCoordinateScale(hwnd, rect);
            resizeDetectedThisEvent = true;
        }
        g_moveTypeKnown = true;
        operationDetectedThisEvent = true;
    }
    POINT mousePosition = {};
    if (!GetCursorPos(&mousePosition))
    {
        return;
    }
    bool mouseMoved =
        mousePosition.x != g_lastMousePosition.x || mousePosition.y != g_lastMousePosition.y;
    bool sizeChangedFromStart = currentWidth != originalWidth || currentHeight != originalHeight;
    if (g_moveTypeKnown && !g_realResizing && sizeChangedFromStart &&
        IsCursorOnResizableFrame(hwnd, mousePosition, rect, true) &&
        MonitorFromPoint(mousePosition, MONITOR_DEFAULTTONEAREST) == g_dragCursorMonitor)
    {
        g_realResizing = true;
        g_resizeCoordinateScale = GetResizeVisualCoordinateScale(hwnd, rect);
        resizeDetectedThisEvent = true;
        operationDetectedThisEvent = true;
    }
    if (g_realResizing && !g_dragResizeWobbleEnabled)
    {
        RetireAnimationSlot(slotIndex);
        return;
    }
    if (!rectChanged && !mouseMoved)
    {
        return;
    }
    bool mouseAtMonitorTopEdge = IsPointAtMonitorTopEdge(mousePosition);
    double movementX = static_cast<double>(mousePosition.x - g_dragStartMousePosition.x);
    double movementY = static_cast<double>(mousePosition.y - g_dragStartMousePosition.y);
    double windowDeltaX = static_cast<double>(rect.left - g_lastDraggedWindowRect.left) *
                          (g_realResizing ? g_resizeCoordinateScale : 1.0);
    double windowDeltaY = static_cast<double>(rect.top - g_lastDraggedWindowRect.top) *
                          (g_realResizing ? g_resizeCoordinateScale : 1.0);
    int previousWidth = g_lastDraggedWindowRect.right - g_lastDraggedWindowRect.left;
    int previousHeight = g_lastDraggedWindowRect.bottom - g_lastDraggedWindowRect.top;
    bool sizeChanged = currentWidth != previousWidth || currentHeight != previousHeight;
    bool windowZoomed = IsZoomed(hwnd) != FALSE;
    bool zoomStateChanged = windowZoomed != g_lastDraggedWindowZoomed;
    ULONGLONG now = GetTickCount64();
    HMONITOR cursorMonitor = MonitorFromPoint(mousePosition, MONITOR_DEFAULTTONEAREST);
    bool monitorChanged = cursorMonitor && cursorMonitor != g_dragCursorMonitor;
    if (monitorChanged)
    {
        g_dragCursorMonitor = cursorMonitor;
        g_monitorTransitionRebaseUntil = now + 750;
    }
    bool dpiReflow = !g_realResizing && sizeChanged && !zoomStateChanged;
    if (dpiReflow)
    {
        g_monitorTransitionRebaseUntil = now + 750;
    }
    bool largeNativeCatchUp = std::abs(windowDeltaX) > 64.0 || std::abs(windowDeltaY) > 64.0;
    bool rebaseMonitorTransition =
        !g_realResizing && now <= g_monitorTransitionRebaseUntil &&
        (monitorChanged || dpiReflow || largeNativeCatchUp);
    Vec2 localMousePosition = {static_cast<double>(mousePosition.x - rect.left) *
                                   (g_realResizing ? g_resizeCoordinateScale : 1.0),
                               static_cast<double>(mousePosition.y - rect.top) *
                                   (g_realResizing ? g_resizeCoordinateScale : 1.0)};
    double currentMeshWidth =
        static_cast<double>(currentWidth) * (g_realResizing ? g_resizeCoordinateScale : 1.0);
    double currentMeshHeight =
        static_cast<double>(currentHeight) * (g_realResizing ? g_resizeCoordinateScale : 1.0);
    static ULONGLONG lastDebugLogTimestamp = 0;
    ULONGLONG debugLogTimestamp = GetTickCount64();
    bool logMesh = debugLogTimestamp - lastDebugLogTimestamp >= 250;
    WobbleMesh debugMesh = {};
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    WindowAnimationSlot& slot = g_animationSlots[slotIndex];
    if (!slot.active || slot.hwnd != hwnd)
    {
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        return;
    }
    if (!slot.transformAttached)
    {
        // Don't accumulate invisible deformation while an existing window's
        // DWM visual is being discovered and bound for the first time.
        InitializeMesh(slot.mesh, currentMeshWidth, currentMeshHeight);
        BeginDrag(slot.mesh, localMousePosition);
        SetMeshResizeMode(slot.mesh, g_realResizing);
        if (g_realResizing)
        {
            UpdateResizeEdges(slot.mesh, g_realDraggedWindowRect, rect);
            ApplyResizeConstraints(slot.mesh);
        }
        slot.previousMesh = slot.mesh;
        slot.identityApplied = false;
        slot.meshIdentityPending = false;
        slot.meshRevision++;
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        g_dragStartMousePosition = mousePosition;
        g_dragStartLocalMouse = localMousePosition;
        g_lastDraggedWindowRect = rect;
        g_lastDraggedWindowZoomed = IsZoomed(hwnd) != FALSE;
        g_lastMousePosition = mousePosition;
        RequestDwmScenePass(hwnd);
        return;
    }
    bool startedInteractiveStateThrob = false;
    if (slot.settings.windowStateWobbleEnabled && g_moveTypeKnown && !g_realResizing &&
        !g_dragStartedWindowZoomed && mouseAtMonitorTopEdge &&
        (!g_interactiveWindowStateThrob || !g_interactiveWindowStateMaximizing))
    {
        // Start maximize wobble at the Aero Snap trigger.
        InitializeMesh(slot.mesh, static_cast<double>(currentWidth),
                       static_cast<double>(currentHeight));
        ApplyWindowStateThrob(slot.mesh, true, true, slot.settings, {0.0, -1.0});
        ClearWindowStateThrobConstraints(slot.mesh);
        BeginDrag(slot.mesh, localMousePosition);
        slot.previousMesh = slot.mesh;
        slot.windowStateThrob = true;
        g_interactiveWindowStateThrob = true;
        g_interactiveWindowStateMaximizing = true;
        startedInteractiveStateThrob = true;
    }
    else if (g_realResizing)
    {
        if (resizeDetectedThisEvent)
        {
            // Rebase custom-title-bar resize state into local DPI coordinates.
            InitializeMesh(slot.mesh, currentMeshWidth, currentMeshHeight);
            BeginDrag(slot.mesh, localMousePosition);
            slot.previousMesh = slot.mesh;
            windowDeltaX = 0.0;
            windowDeltaY = 0.0;
        }
        if (!slot.mesh.resizing)
        {
            SetMeshResizeMode(slot.mesh, true);
            SetMeshResizeMode(slot.previousMesh, true);
        }
        // Preserve screen-space points during top/left resize.
        OffsetMeshPositions(slot.mesh, -windowDeltaX, -windowDeltaY);
        OffsetMeshPositions(slot.previousMesh, -windowDeltaX, -windowDeltaY);
        UpdateMeshBaseGrid(slot.mesh, currentMeshWidth, currentMeshHeight);
        UpdateMeshBaseGrid(slot.previousMesh, currentMeshWidth, currentMeshHeight);
        UpdateResizeEdges(slot.mesh, g_realDraggedWindowRect, rect);
        slot.previousMesh.resizing = slot.mesh.resizing;
        slot.previousMesh.canWobbleTop = slot.mesh.canWobbleTop;
        slot.previousMesh.canWobbleLeft = slot.mesh.canWobbleLeft;
        slot.previousMesh.canWobbleRight = slot.mesh.canWobbleRight;
        slot.previousMesh.canWobbleBottom = slot.mesh.canWobbleBottom;
        // Preserve the original grab offset across mixed-DPI resize.
        slot.mesh.lastDragPosition = localMousePosition;
        slot.previousMesh.lastDragPosition = localMousePosition;
        ApplyResizeConstraints(slot.mesh);
        ApplyResizeConstraints(slot.previousMesh);
    }
    else if (rebaseMonitorTransition)
    {
        // Per-monitor DPI changes can make the native window catch up in one
        // large step. Treat that step as new geometry, not as wobble velocity.
        InitializeMesh(slot.mesh, static_cast<double>(currentWidth),
                       static_cast<double>(currentHeight));
        BeginDrag(slot.mesh, localMousePosition);
        slot.previousMesh = slot.mesh;
        slot.windowStateThrob = false;
        g_dragStartMousePosition = mousePosition;
        g_dragStartLocalMouse = localMousePosition;
    }
    else if (sizeChanged)
    {
        // Only IsZoomed transitions trigger state wobble during a move.
        InitializeMesh(slot.mesh, static_cast<double>(currentWidth),
                       static_cast<double>(currentHeight));
        if (zoomStateChanged && slot.settings.windowStateWobbleEnabled)
        {
            ApplyWindowStateThrob(slot.mesh, windowZoomed, false, slot.settings,
                                  windowZoomed ? Vec2{0.0, -1.0} : Vec2{0.0, 1.0});
            ClearWindowStateThrobConstraints(slot.mesh);
            slot.windowStateThrob = true;
            g_interactiveWindowStateThrob = true;
            g_interactiveWindowStateMaximizing = windowZoomed;
            startedInteractiveStateThrob = true;
        }
        BeginDrag(slot.mesh, localMousePosition);
        slot.previousMesh = slot.mesh;
        g_dragStartMousePosition = mousePosition;
        g_dragStartLocalMouse = localMousePosition;
    }
    else if (!startedInteractiveStateThrob)
    {
        OffsetMeshPositions(slot.mesh, -windowDeltaX, -windowDeltaY);
        OffsetMeshPositions(slot.previousMesh, -windowDeltaX, -windowDeltaY);
        slot.mesh.lastDragPosition = localMousePosition;
        slot.previousMesh.lastDragPosition = localMousePosition;
    }
    slot.mesh.active = true;
    slot.identityApplied = false;
    slot.meshIdentityPending = false;
    slot.meshRevision++;
    if (logMesh)
    {
        debugMesh = slot.mesh;
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    g_lastDraggedWindowRect = rect;
    g_lastDraggedWindowZoomed = windowZoomed;
    g_lastMousePosition = mousePosition;
    g_moveEventCounter++;
    if (operationDetectedThisEvent)
    {
        Wh_Log(L"Operation detected: %s", g_realResizing ? L"RESIZE" : L"MOVE");
    }
    if (logMesh)
    {
        lastDebugLogTimestamp = debugLogTimestamp;
        Wh_Log(L"%s #%u Delta=(%.2f,%.2f)", g_realResizing ? L"RESIZE" : L"MOVE",
               g_moveEventCounter, movementX, movementY);
        LogMeshSummary(debugMesh, L"PHYSICS");
    }
}

static void HandleMoveSizeEnd(HWND hwnd)
{
    HWND draggedWindow = g_realDraggedWindow.load(std::memory_order_relaxed);
    if (!g_realDragging.load(std::memory_order_relaxed) || hwnd != draggedWindow ||
        g_dragAnimationSlot < 0 || g_dragAnimationSlot >= MAX_ANIMATION_SLOTS)
    {
        return;
    }
    int slotIndex = g_dragAnimationSlot;
    POINT releaseMousePosition = {};
    bool hasReleaseMousePosition = !g_realResizing && GetCursorPos(&releaseMousePosition);
    MonitorEdgeState releaseEdgeState = hasReleaseMousePosition
                                            ? GetPointMonitorEdgeState(releaseMousePosition)
                                            : MonitorEdgeState{};
    if (g_hasLastMousePosition && (!releaseEdgeState.top || !releaseEdgeState.side))
    {
        // MOVESIZEEND may arrive after the cursor leaves the edge.
        MonitorEdgeState lastDragEdgeState = GetPointMonitorEdgeState(g_lastMousePosition);
        releaseEdgeState.top = releaseEdgeState.top || lastDragEdgeState.top;
        releaseEdgeState.side = releaseEdgeState.side || lastDragEdgeState.side;
        if (releaseEdgeState.direction.x == 0.0)
        {
            releaseEdgeState.direction.x = lastDragEdgeState.direction.x;
        }
        if (releaseEdgeState.direction.y == 0.0)
        {
            releaseEdgeState.direction.y = lastDragEdgeState.direction.y;
        }
    }
    bool releasedAtMonitorTopEdge =
        !g_realResizing && releaseEdgeState.top && !releaseEdgeState.side;
    bool releasedAtMonitorSideEdge = !g_realResizing && releaseEdgeState.side;
    // Publish final geometry before releasing the grabbed point.
    g_finalizingMoveSize = true;
    HandleObservedWindowLocationChange(hwnd, OBJID_WINDOW, CHILDID_SELF);
    g_finalizingMoveSize = false;
    HandleLocationChange(hwnd, OBJID_WINDOW, CHILDID_SELF);
    // Late Snap notifications can now use the normal queue.
    g_realDragging.store(false, std::memory_order_release);
    HWND pendingInteractiveSnapWindow =
        g_pendingInteractiveSnapWindow.exchange(nullptr, std::memory_order_acq_rel);
    LPARAM pendingInteractiveSnapFlags =
        g_pendingInteractiveSnapFlags.exchange(0, std::memory_order_relaxed);
    bool animate = false;
    bool stateTransitionWobble = false;
    bool snapTransitionWobble = false;
    bool transitionExpectedZoomed = false;
    LPARAM transitionReplayFlags = 0;
    RECT releaseRect = {};
    bool hasReleaseRect = GetWindowRect(hwnd, &releaseRect) != FALSE;
    // Final geometry detects Snap even when edge notifications coalesce.
    bool releasedAtSnapGeometry = !g_realResizing && hasReleaseRect && !IsZoomed(hwnd) &&
                                  IsEligibleWindowForStateThrob(hwnd) &&
                                  IsApproximatelySnapLayoutTarget(releaseRect);
    bool releasedIntoSnapTarget =
        !g_realResizing && (releasedAtMonitorSideEdge || pendingInteractiveSnapWindow == hwnd ||
                            releasedAtSnapGeometry);
    Vec2 snapDirection = pendingInteractiveSnapWindow == hwnd
                             ? DecodeWindowTransitionDirection(pendingInteractiveSnapFlags)
                             : Vec2{};
    if (snapDirection.x == 0.0 && snapDirection.y == 0.0)
    {
        snapDirection = releaseEdgeState.direction;
    }
    if (snapDirection.x == 0.0 && snapDirection.y == 0.0)
    {
        snapDirection = DecodeWindowTransitionDirection(
            EncodeWindowTransitionDirection(g_realDraggedWindowRect, releaseRect));
    }
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    WindowAnimationSlot& slot = g_animationSlots[slotIndex];
    if (!slot.active || slot.hwnd != hwnd)
    {
        ReleaseSRWLockExclusive(&g_animationSlotsLock);
        ResetDragInputState();
        return;
    }
    bool wasDragging = slot.mesh.dragging;
    if (slot.settings.windowStateWobbleEnabled && releasedAtMonitorTopEdge)
    {
        int releaseWidth = releaseRect.right - releaseRect.left;
        int releaseHeight = releaseRect.bottom - releaseRect.top;
        if (ReplaceAnimationWithStateThrob(slot, releaseWidth, releaseHeight, true,
                                           {0.0, -1.0}))
        {
            animate = slot.matrixTransformProxy != nullptr;
            stateTransitionWobble = true;
            transitionExpectedZoomed = true;
            transitionReplayFlags = NATIVE_TRANSITION_TARGET_IS_WORK_AREA |
                                    NATIVE_TRANSITION_DIRECTION_UP;
        }
        else if (wasDragging)
        {
            EndDrag(slot.mesh);
            animate = slot.matrixTransformProxy != nullptr;
        }
    }
    else if (slot.settings.windowStateWobbleEnabled && releasedIntoSnapTarget)
    {
        int releaseWidth = releaseRect.right - releaseRect.left;
        int releaseHeight = releaseRect.bottom - releaseRect.top;
        if (ReplaceAnimationWithStateThrob(slot, releaseWidth, releaseHeight, true,
                                           snapDirection))
        {
            animate = slot.matrixTransformProxy != nullptr;
            snapTransitionWobble = true;
            transitionReplayFlags = NATIVE_TRANSITION_TARGET_IS_SNAP_LAYOUT |
                                    EncodeWindowTransitionDirection(snapDirection);
        }
    }
    else
    {
        // Preserve restore pulses after release.
        stateTransitionWobble = slot.windowStateThrob;
        transitionExpectedZoomed = false;
        if (wasDragging)
        {
            EndDrag(slot.mesh);
        }
        animate = wasDragging && slot.matrixTransformProxy;
    }
    if (animate)
    {
        slot.dragging = false;
        slot.freeStepPending = true;
        slot.order = ++g_animationOrderCounter;
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (stateTransitionWobble && animate)
    {
        MarkObservedWindowTransitionPending(hwnd, transitionExpectedZoomed);
        RequestDwmScenePass(hwnd);
    }
    if (snapTransitionWobble && animate)
    {
        MarkObservedSnapTransition(hwnd, false);
        RequestDwmScenePass(hwnd);
    }
    Wh_Log(L"%s END HWND=%p Events=%u", g_realResizing ? L"RESIZE" : L"MOVE", hwnd,
           g_moveEventCounter);
    if (stateTransitionWobble && transitionExpectedZoomed)
    {
        Wh_Log(L"WINDOW STATE THROB HWND=%p State=MAXIMIZED "
               L"Source=INTERACTIVE_TOP_EDGE",
               hwnd);
    }
    else if (snapTransitionWobble)
    {
        Wh_Log(L"WINDOW STATE THROB HWND=%p State=SNAPPED "
               L"Source=INTERACTIVE_EDGE_OR_FINAL_GEOMETRY",
               hwnd);
    }
    if (!animate)
    {
        ResetDragInputState();
        RetireAnimationSlot(slotIndex);
        return;
    }
    ResetDragInputState();
    // Re-sample the destination monitor's animation rate at release.
    if (!EnsureAnimationClockRunning(hwnd))
    {
        Wh_Log(L"Failed to continue free wobble animation");
        RetireAnimationSlot(slotIndex);
        return;
    }
    if (transitionReplayFlags != 0)
    {
        // The native transition hook often fires before MOVESIZEEND. Replay it
        // once after release so the clean state pulse binds to the new visual.
        QueueNativeWindowTransition(hwnd, transitionReplayFlags);
    }
}

static int FindObservedWindowState(HWND hwnd)
{
    for (int i = 0; i < MAX_OBSERVED_WINDOWS; i++)
    {
        if (g_observedWindows[i].hwnd == hwnd)
        {
            return i;
        }
    }
    return -1;
}

static void ForgetObservedWindowState(HWND hwnd)
{
    int index = FindObservedWindowState(hwnd);
    if (index >= 0)
    {
        g_observedWindows[index] = {};
    }
}

static void RememberObservedWindowState(HWND hwnd)
{
    if (!IsEligibleWindowForStateThrob(hwnd))
    {
        return;
    }
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect))
    {
        return;
    }
    int index = FindObservedWindowState(hwnd);
    if (index < 0)
    {
        ULONGLONG oldestTimestamp = ULLONG_MAX;
        for (int i = 0; i < MAX_OBSERVED_WINDOWS; i++)
        {
            if (!g_observedWindows[i].hwnd)
            {
                index = i;
                break;
            }
            if (g_observedWindows[i].lastSeen < oldestTimestamp)
            {
                oldestTimestamp = g_observedWindows[i].lastSeen;
                index = i;
            }
        }
    }
    if (index < 0)
    {
        return;
    }
    g_observedWindows[index] = {hwnd,
                                IsZoomed(hwnd) != FALSE,
                                IsIconic(hwnd) != FALSE,
                                IsApproximatelySnapLayoutTarget(rect),
                                false,
                                false,
                                rect,
                                 GetTickCount64(),
                                 0,
                                 0,
                                 0,
                                  0};
}

static int EnsureObservedWindowState(HWND hwnd)
{
    int index = FindObservedWindowState(hwnd);
    if (index < 0)
    {
        RememberObservedWindowState(hwnd);
        index = FindObservedWindowState(hwnd);
    }
    return index;
}

static void CancelWindowStateThrobForWindow(HWND hwnd)
{
    int slotIndex = -1;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        const WindowAnimationSlot& slot = g_animationSlots[i];
        if (slot.active && slot.hwnd == hwnd && slot.windowStateThrob && !slot.dragging)
        {
            slotIndex = i;
            break;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    if (slotIndex >= 0)
    {
        RetireAnimationSlot(slotIndex);
    }
}

static void ResetObservedSnapStateForInteractiveMove(HWND hwnd)
{
    int index = EnsureObservedWindowState(hwnd);
    if (index < 0)
    {
        return;
    }
    ObservedWindowState& observed = g_observedWindows[index];
    observed.snapped = false;
    observed.snapTransitionDeadline = 0;
    observed.nativeTransitionPending = false;
    observed.nativeTransitionDeadline = 0;
    observed.nativeTransitionStartedAt = 0;
    observed.lastSeen = GetTickCount64();
}

static bool ShouldSuppressWindowStateThrob(HWND hwnd)
{
    int index = FindObservedWindowState(hwnd);
    if (index < 0)
    {
        return false;
    }
    const ObservedWindowState& observed = g_observedWindows[index];
    return observed.iconic || GetTickCount64() <= observed.suppressStateThrobUntil;
}

static void HandleMinimizeLifecycle(HWND hwnd, bool minimizing)
{
    if (!hwnd || !IsWindow(hwnd))
    {
        return;
    }
    int index = FindObservedWindowState(hwnd);
    if (index < 0)
    {
        RememberObservedWindowState(hwnd);
        index = FindObservedWindowState(hwnd);
    }
    if (index < 0)
    {
        return;
    }
    ULONGLONG now = GetTickCount64();
    ObservedWindowState& observed = g_observedWindows[index];
    observed.iconic = minimizing;
    observed.nativeTransitionPending = false;
    observed.nativeTransitionDeadline = 0;
    observed.snapTransitionDeadline = 0;
    observed.suppressStateThrobUntil = now + 500;
    observed.lastSeen = now;
    if (!minimizing)
    {
        // Sync taskbar restore without triggering maximize wobble.
        observed.zoomed = IsZoomed(hwnd) != FALSE;
        RECT rect = {};
        if (GetWindowRect(hwnd, &rect))
        {
            observed.rect = rect;
            observed.snapped = !observed.zoomed && IsApproximatelySnapLayoutTarget(rect);
        }
    }
    // Cancel only a transition pulse racing taskbar restore.
    CancelWindowStateThrobForWindow(hwnd);
    if (!minimizing)
    {
        Wh_Log(L"TASKBAR RESTORE: window-state wobble suppressed "
               L"for HWND=%p",
               hwnd);
    }
}

static void MarkObservedWindowTransitionPending(HWND hwnd, bool expectedZoomed)
{
    int index = EnsureObservedWindowState(hwnd);
    if (index < 0)
    {
        return;
    }
    ULONGLONG now = GetTickCount64();
    g_observedWindows[index].nativeTransitionPending = true;
    g_observedWindows[index].expectedZoomed = expectedZoomed;
    g_observedWindows[index].nativeTransitionDeadline = now + 750;
    g_observedWindows[index].nativeTransitionStartedAt = 0;
    g_observedWindows[index].lastSeen = now;
}

static void MarkObservedSnapTransition(HWND hwnd, bool transitionStarted)
{
    int index = EnsureObservedWindowState(hwnd);
    if (index < 0)
    {
        return;
    }
    ULONGLONG now = GetTickCount64();
    ObservedWindowState& observed = g_observedWindows[index];
    // Suppress duplicate native Snap notifications briefly.
    observed.snapTransitionDeadline = now + 300;
    // Consume the IsZoomed edge as part of this Snap pulse.
    observed.nativeTransitionPending = true;
    observed.expectedZoomed = false;
    observed.nativeTransitionDeadline = now + 750;
    observed.nativeTransitionStartedAt = transitionStarted ? now : 0;
    observed.lastSeen = now;
}

static void StartWindowStateThrob(HWND hwnd, bool maximizing, bool snapTransition = false,
                                  Vec2 direction = {})
{
    if (!hwnd || GetTickCount64() <
                     g_windowStateThrobSuppressedUntil.load(std::memory_order_acquire) ||
        g_realDragging.load(std::memory_order_relaxed) ||
        !IsEligibleWindowForStateThrob(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd) ||
        ShouldSuppressWindowStateThrob(hwnd))
    {
        return;
    }
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect))
    {
        return;
    }
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0)
    {
        return;
    }
    WobblySettings settings = GetSettingsSnapshot();
    if (!settings.windowStateWobbleEnabled)
    {
        return;
    }
    Vec2 centre = {static_cast<double>(width) * 0.5, static_cast<double>(height) * 0.5};
    int slotIndex = AcquireAnimationSlot(hwnd, settings, static_cast<double>(width),
                                         static_cast<double>(height), centre);
    if (slotIndex < 0)
    {
        return;
    }
    bool started = false;
    AcquireSRWLockExclusive(&g_animationSlotsLock);
    WindowAnimationSlot& slot = g_animationSlots[slotIndex];
    if (slot.active && slot.hwnd == hwnd)
    {
        started = ReplaceAnimationWithStateThrob(slot, width, height, maximizing, direction);
    }
    ReleaseSRWLockExclusive(&g_animationSlotsLock);
    if (!started)
    {
        RetireAnimationSlot(slotIndex);
        return;
    }
    if (!EnsureAnimationClockRunning(hwnd))
    {
        RetireAnimationSlot(slotIndex);
        return;
    }
    RequestDwmScenePass(hwnd);
    Wh_Log(L"WINDOW STATE THROB HWND=%p State=%s Size=%dx%d", hwnd,
           snapTransition ? L"SNAPPED" : (maximizing ? L"MAXIMIZED" : L"RESTORED"), width,
           height);
}

static void HandleNativeWindowTransitionStart(HWND hwnd, LPARAM transitionFlags)
{
    if (!hwnd || !IsWindow(hwnd) || g_realDragging.load(std::memory_order_relaxed))
    {
        return;
    }
    int index = EnsureObservedWindowState(hwnd);
    ULONGLONG now = GetTickCount64();
    if (index >= 0 && (g_observedWindows[index].iconic ||
                       now <= g_observedWindows[index].suppressStateThrobUntil))
    {
        ObservedWindowState& observed = g_observedWindows[index];
        if (!IsIconic(hwnd))
        {
            observed.iconic = false;
            observed.zoomed = IsZoomed(hwnd) != FALSE;
        }
        observed.nativeTransitionPending = false;
        observed.nativeTransitionDeadline = 0;
        observed.lastSeen = now;
        CancelWindowStateThrobForWindow(hwnd);
        return;
    }
    bool targetIsWorkArea = (transitionFlags & NATIVE_TRANSITION_TARGET_IS_WORK_AREA) != 0;
    bool sourceIsWorkArea = (transitionFlags & NATIVE_TRANSITION_SOURCE_IS_WORK_AREA) != 0;
    bool windowIsZoomed = (transitionFlags & NATIVE_TRANSITION_WINDOW_IS_ZOOMED) != 0;
    bool targetIsSnapLayout = (transitionFlags & NATIVE_TRANSITION_TARGET_IS_SNAP_LAYOUT) != 0;
    if (targetIsSnapLayout)
    {
        if (index >= 0 && now < g_observedWindows[index].snapTransitionDeadline &&
            g_observedWindows[index].nativeTransitionStartedAt != 0)
        {
            return;
        }
        // Re-seed an early interactive pulse on the native transition visual.
        MarkObservedSnapTransition(hwnd, true);
        StartWindowStateThrob(hwnd, true, true, DecodeWindowTransitionDirection(transitionFlags));
        return;
    }
    bool previouslyZoomed = index >= 0 && g_observedWindows[index].zoomed;
    bool maximizing = false;
    bool recognizedStateTransition = false;
    if (index >= 0 && windowIsZoomed != previouslyZoomed)
    {
        // Prefer uDWM's published logical state over transitional geometry.
        maximizing = windowIsZoomed;
        recognizedStateTransition = true;
    }
    else if (targetIsWorkArea)
    {
        maximizing = true;
        recognizedStateTransition = true;
    }
    else if (sourceIsWorkArea || previouslyZoomed)
    {
        maximizing = false;
        recognizedStateTransition = true;
    }
    else if (windowIsZoomed)
    {
        // Accept early IsZoomed changes before transitional geometry settles.
        maximizing = true;
        recognizedStateTransition = true;
    }
    if (!recognizedStateTransition)
    {
        // Leave Snap-out and cross-monitor placement as ordinary motion.
        return;
    }
    if (index >= 0)
    {
        ObservedWindowState& observed = g_observedWindows[index];
        // Multiple uDWM hooks can describe two phases of the same transition
        // with opposite flags. Keep the first pulse instead of replacing it.
        if (observed.nativeTransitionStartedAt != 0 &&
            now - observed.nativeTransitionStartedAt < 120)
        {
            return;
        }
        if (observed.nativeTransitionPending && observed.expectedZoomed == maximizing &&
            now < observed.nativeTransitionDeadline && observed.nativeTransitionStartedAt != 0)
        {
            return;
        }
        observed.nativeTransitionPending = true;
        observed.expectedZoomed = maximizing;
        observed.nativeTransitionDeadline = now + 750;
        observed.nativeTransitionStartedAt = now;
        observed.lastSeen = now;
    }
    StartWindowStateThrob(hwnd, maximizing);
}

static void HandleObservedWindowLocationChange(HWND hwnd, LONG idObject, LONG idChild)
{
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF ||
        !IsEligibleWindowForStateThrob(hwnd))
    {
        return;
    }
    int index = FindObservedWindowState(hwnd);
    bool zoomed = IsZoomed(hwnd) != FALSE;
    bool iconic = IsIconic(hwnd) != FALSE;
    RECT rect = {};
    bool hasRect = GetWindowRect(hwnd, &rect) != FALSE;
    ULONGLONG now = GetTickCount64();
    if (index < 0)
    {
        RememberObservedWindowState(hwnd);
        return;
    }
    ObservedWindowState& observed = g_observedWindows[index];
    RECT previousObservedRect = observed.rect;
    bool wasIconic = observed.iconic;
    if (iconic)
    {
        // Preserve zoom state while a window is minimized.
        observed.iconic = true;
        observed.nativeTransitionPending = false;
        observed.nativeTransitionDeadline = 0;
        observed.lastSeen = now;
        return;
    }
    if (wasIconic || now <= observed.suppressStateThrobUntil)
    {
        observed.iconic = false;
        observed.zoomed = zoomed;
        observed.nativeTransitionPending = false;
        observed.nativeTransitionDeadline = 0;
        observed.lastSeen = now;
        if (wasIconic)
        {
            observed.suppressStateThrobUntil = now + 500;
        }
        if (hasRect)
        {
            observed.rect = rect;
            observed.snapped = !zoomed && IsApproximatelySnapLayoutTarget(rect);
        }
        CancelWindowStateThrobForWindow(hwnd);
        return;
    }
    observed.iconic = false;
    bool zoomStateChanged = observed.zoomed != zoomed;
    bool realDragging = g_realDragging.load(std::memory_order_relaxed);
    bool rectChanged = hasRect && !EqualRect(&rect, &observed.rect);
    // Skip fallback work when neither geometry nor state changed.
    if (!g_finalizingMoveSize && !rectChanged && !zoomStateChanged &&
        !observed.nativeTransitionPending)
    {
        observed.lastSeen = now;
        return;
    }
    if (realDragging && !g_finalizingMoveSize)
    {
        // Defer Snap classification until the final drag geometry.
        if (hasRect)
        {
            observed.rect = rect;
        }
        observed.zoomed = zoomed;
        observed.nativeTransitionPending = false;
        observed.nativeTransitionDeadline = 0;
        observed.lastSeen = now;
        return;
    }
    bool snapTargetChanged = false;
    if (hasRect)
    {
        bool snapped = !zoomed && IsApproximatelySnapLayoutTarget(rect);
        snapTargetChanged = snapped && (!observed.snapped || rectChanged);
        observed.snapped = snapped;
        observed.rect = rect;
    }
    if (realDragging)
    {
        // Use final geometry when private Snap hooks are absent.
        bool finalGeometryChanged = hasRect && !EqualRect(&rect, &g_realDraggedWindowRect);
        if (observed.snapped && finalGeometryChanged && !g_realResizing &&
            g_realDraggedWindow.load(std::memory_order_relaxed) == hwnd)
        {
            g_pendingInteractiveSnapFlags.store(
                EncodeWindowTransitionDirection(g_realDraggedWindowRect, rect),
                std::memory_order_relaxed);
            g_pendingInteractiveSnapWindow.store(hwnd, std::memory_order_release);
        }
        observed.zoomed = zoomed;
        observed.nativeTransitionPending = false;
        observed.nativeTransitionDeadline = 0;
        observed.lastSeen = now;
        return;
    }
    if (observed.nativeTransitionPending)
    {
        if (zoomed == observed.expectedZoomed)
        {
            // Consume the state finalized after an early native pulse.
            observed.zoomed = zoomed;
            observed.nativeTransitionPending = false;
            observed.lastSeen = now;
            return;
        }
        if (now < observed.nativeTransitionDeadline)
        {
            observed.lastSeen = now;
            return;
        }
        observed.nativeTransitionPending = false;
    }
    observed.zoomed = zoomed;
    observed.lastSeen = now;
    if (snapTargetChanged)
    {
        if (now >= observed.snapTransitionDeadline)
        {
            MarkObservedSnapTransition(hwnd, true);
            StartWindowStateThrob(hwnd, true, true,
                                  DecodeWindowTransitionDirection(
                                      EncodeWindowTransitionDirection(previousObservedRect, rect)));
        }
        // Snap-in supersedes a simultaneous restore edge.
        return;
    }
    if (zoomStateChanged)
    {
        StartWindowStateThrob(hwnd, zoomed);
    }
}

static bool IsShellCloakedWindow(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd) || !g_dwmGetWindowAttribute)
    {
        return false;
    }
    constexpr DWORD dwmwaCloaked = 14;
    constexpr DWORD dwmCloakedByShell = 0x00000002;
    DWORD cloaked = 0;
    return SUCCEEDED(g_dwmGetWindowAttribute(hwnd, dwmwaCloaked, &cloaked, sizeof(cloaked))) &&
           (cloaked & dwmCloakedByShell) != 0;
}

static void HandleVirtualDesktopSwitch()
{
    ULONGLONG now = GetTickCount64();
    int slotsToRetire[MAX_ANIMATION_SLOTS] = {};
    int retireCount = 0;
    AcquireSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < MAX_ANIMATION_SLOTS; i++)
    {
        if (g_animationSlots[i].active)
        {
            slotsToRetire[retireCount++] = i;
        }
    }
    ReleaseSRWLockShared(&g_animationSlotsLock);
    for (int i = 0; i < retireCount; i++)
    {
        RetireAnimationSlot(slotsToRetire[i]);
    }
    ResetDragInputState();
    // Old desktop objects can remain valid while pointing at a hidden visual.
    // Force the delayed backfill to read the active CWindowData object graph.
    ClearAllDwmWindowMappings();
    for (ObservedWindowState& observed : g_observedWindows)
    {
        observed = {};
    }
    g_pendingMaximizedStateWindow.store(nullptr, std::memory_order_release);
    g_maximizedStateCheckQueued.store(false, std::memory_order_release);
    g_windowStateThrobSuppressedUntil.store(now + 750,
                                            std::memory_order_release);
    RememberObservedWindowState(GetForegroundWindow());
    QueueExistingWindowBackfill();
    Wh_Log(L"Virtual desktop changed: retired=%d, active windows refreshed", retireCount);
}

static void ScheduleVirtualDesktopRefresh()
{
    if (g_unloading.load(std::memory_order_acquire))
    {
        return;
    }
    g_virtualDesktopRefreshDeadline = GetTickCount64() + 300;
    if (g_virtualDesktopRefreshTimer)
    {
        KillTimer(nullptr, g_virtualDesktopRefreshTimer);
    }
    g_virtualDesktopRefreshTimer = SetTimer(nullptr, 0, 300, nullptr);
    if (!g_virtualDesktopRefreshTimer)
    {
        g_virtualDesktopRefreshDeadline = 0;
        HandleVirtualDesktopSwitch();
    }
}

static void CALLBACK WinEventCallback(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject,
                                      LONG idChild, DWORD eventThread, DWORD eventTime)
{
    UNREFERENCED_PARAMETER(hook);
    UNREFERENCED_PARAMETER(eventThread);
    UNREFERENCED_PARAMETER(eventTime);
    if (g_unloading.load(std::memory_order_acquire))
    {
        return;
    }
    if (event == EVENT_OBJECT_LOCATIONCHANGE &&
        (idObject != OBJID_WINDOW || idChild != CHILDID_SELF))
    {
        return;
    }
    switch (event)
    {
    case EVENT_SYSTEM_MOVESIZESTART:
    {
        HandleMoveSizeStart(hwnd);
        break;
    }
    case EVENT_OBJECT_LOCATIONCHANGE:
    {
        HandleObservedWindowLocationChange(hwnd, idObject, idChild);
        HandleLocationChange(hwnd, idObject, idChild);
        break;
    }
    case EVENT_SYSTEM_MOVESIZEEND:
    {
        HandleMoveSizeEnd(hwnd);
        break;
    }
    case EVENT_SYSTEM_FOREGROUND:
    {
        HWND previousForeground = g_lastForegroundWindow;
        g_lastForegroundWindow = hwnd;
        if (previousForeground && previousForeground != hwnd &&
            !IsIconic(previousForeground) && IsShellCloakedWindow(previousForeground))
        {
            ScheduleVirtualDesktopRefresh();
        }
        HandleObservedWindowLocationChange(hwnd, OBJID_WINDOW, CHILDID_SELF);
        break;
    }
    case EVENT_SYSTEM_MINIMIZESTART:
    {
        HandleMinimizeLifecycle(hwnd, true);
        break;
    }
    case EVENT_SYSTEM_MINIMIZEEND:
    {
        HandleMinimizeLifecycle(hwnd, false);
        break;
    }
    case EVENT_OBJECT_DESTROY:
    {
        if (idObject == OBJID_WINDOW)
        {
            ForgetObservedWindowState(hwnd);
        }
        break;
    }
    case EVENT_OBJECT_CLOAKED:
    {
        if (idObject == OBJID_WINDOW && idChild == CHILDID_SELF && !IsIconic(hwnd) &&
            IsShellCloakedWindow(hwnd))
        {
            ScheduleVirtualDesktopRefresh();
        }
        break;
    }
    case EVENT_SYSTEM_DESKTOPSWITCH:
    {
        g_lastForegroundWindow = hwnd ? hwnd : GetForegroundWindow();
        ScheduleVirtualDesktopRefresh();
        break;
    }
    }
}

static bool InitializeWindowEventHooks()
{
    g_moveSizeHook =
        SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, nullptr,
                        WinEventCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_moveSizeHook)
    {
        Wh_Log(L"Failed to create move/size WinEvent hook");
        return false;
    }
    g_locationHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                                     nullptr, WinEventCallback, 0, 0,
                                     WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_locationHook)
    {
        Wh_Log(L"Failed to create location WinEvent hook");
        UnhookWinEvent(g_moveSizeHook);
        g_moveSizeHook = nullptr;
        return false;
    }
    g_foregroundHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventCallback,
                        0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_foregroundHook)
    {
        Wh_Log(L"Failed to create foreground WinEvent hook");
        UnhookWinEvent(g_locationHook);
        UnhookWinEvent(g_moveSizeHook);
        g_locationHook = nullptr;
        g_moveSizeHook = nullptr;
        return false;
    }
    g_minimizeHook =
        SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr,
                        WinEventCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_minimizeHook)
    {
        Wh_Log(L"Failed to create minimize WinEvent hook");
        UnhookWinEvent(g_foregroundHook);
        UnhookWinEvent(g_locationHook);
        UnhookWinEvent(g_moveSizeHook);
        g_foregroundHook = nullptr;
        g_locationHook = nullptr;
        g_moveSizeHook = nullptr;
        return false;
    }
    g_destroyHook =
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr, WinEventCallback, 0, 0,
                        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_destroyHook)
    {
        Wh_Log(L"Failed to create destroy WinEvent hook");
        UnhookWinEvent(g_minimizeHook);
        UnhookWinEvent(g_foregroundHook);
        UnhookWinEvent(g_locationHook);
        UnhookWinEvent(g_moveSizeHook);
        g_minimizeHook = nullptr;
        g_foregroundHook = nullptr;
        g_locationHook = nullptr;
        g_moveSizeHook = nullptr;
        return false;
    }
    g_desktopSwitchHook =
        SetWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH, nullptr,
                        WinEventCallback, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_desktopSwitchHook)
    {
        Wh_Log(L"Failed to create virtual desktop WinEvent hook");
        UnhookWinEvent(g_destroyHook);
        UnhookWinEvent(g_minimizeHook);
        UnhookWinEvent(g_foregroundHook);
        UnhookWinEvent(g_locationHook);
        UnhookWinEvent(g_moveSizeHook);
        g_destroyHook = nullptr;
        g_minimizeHook = nullptr;
        g_foregroundHook = nullptr;
        g_locationHook = nullptr;
        g_moveSizeHook = nullptr;
        return false;
    }
    g_cloakHook = SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                                  WinEventCallback, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_cloakHook)
    {
        Wh_Log(L"Failed to create window cloak WinEvent hook");
        UnhookWinEvent(g_desktopSwitchHook);
        UnhookWinEvent(g_destroyHook);
        UnhookWinEvent(g_minimizeHook);
        UnhookWinEvent(g_foregroundHook);
        UnhookWinEvent(g_locationHook);
        UnhookWinEvent(g_moveSizeHook);
        g_desktopSwitchHook = nullptr;
        g_destroyHook = nullptr;
        g_minimizeHook = nullptr;
        g_foregroundHook = nullptr;
        g_locationHook = nullptr;
        g_moveSizeHook = nullptr;
        return false;
    }
    g_lastForegroundWindow = GetForegroundWindow();
    RememberObservedWindowState(g_lastForegroundWindow);
    Wh_Log(L"Window event hooks initialized");
    return true;
}

static void UninitializeWindowEventHooks()
{
    if (g_virtualDesktopRefreshTimer)
    {
        KillTimer(nullptr, g_virtualDesktopRefreshTimer);
        g_virtualDesktopRefreshTimer = 0;
    }
    g_virtualDesktopRefreshDeadline = 0;
    g_existingWindowBackfillCount.store(0, std::memory_order_release);
    g_existingWindowBackfillIndex.store(0, std::memory_order_release);
    g_existingWindowBackfillMapped.store(0, std::memory_order_release);
    StopAllAnimations();
    if (g_cloakHook)
    {
        UnhookWinEvent(g_cloakHook);
        g_cloakHook = nullptr;
    }
    if (g_desktopSwitchHook)
    {
        UnhookWinEvent(g_desktopSwitchHook);
        g_desktopSwitchHook = nullptr;
    }
    if (g_destroyHook)
    {
        UnhookWinEvent(g_destroyHook);
        g_destroyHook = nullptr;
    }
    if (g_foregroundHook)
    {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
    if (g_minimizeHook)
    {
        UnhookWinEvent(g_minimizeHook);
        g_minimizeHook = nullptr;
    }
    if (g_locationHook)
    {
        UnhookWinEvent(g_locationHook);
        g_locationHook = nullptr;
    }
    if (g_moveSizeHook)
    {
        UnhookWinEvent(g_moveSizeHook);
        g_moveSizeHook = nullptr;
    }
    for (int i = 0; i < MAX_OBSERVED_WINDOWS; i++)
    {
        g_observedWindows[i] = {};
    }
    g_lastForegroundWindow = nullptr;
    Wh_Log(L"Window event hooks removed");
}

static DWORD WINAPI WindowEventThreadProc(LPVOID parameter)
{
    UNREFERENCED_PARAMETER(parameter);
    Wh_Log(L"Window event thread starting");
    // Force creation of this thread's message queue.
    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_eventThreadMessageTarget.store(GetCurrentThreadId(), std::memory_order_release);
    g_animationTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!g_animationTimer)
    {
        g_animationTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    }
    if (!g_animationTimer || !QueryPerformanceFrequency(&g_animationFrequency))
    {
        Wh_Log(L"Window event thread: animation clock initialization failed");
        if (g_animationTimer)
        {
            CloseHandle(g_animationTimer);
            g_animationTimer = nullptr;
        }
        if (g_eventThreadReady)
        {
            SetEvent(g_eventThreadReady);
        }
        g_eventThreadMessageTarget.store(0, std::memory_order_release);
        return 1;
    }
    DisableAnimationThreadPowerThrottling();
    Wh_Log(L"Animation clock: refresh-synchronized high-resolution timer, EcoQoS disabled");
    if (!InitializeWindowEventHooks())
    {
        Wh_Log(L"Window event thread: hook initialization failed");
        if (g_eventThreadReady)
        {
            SetEvent(g_eventThreadReady);
        }
        CloseHandle(g_animationTimer);
        g_animationTimer = nullptr;
        g_eventThreadMessageTarget.store(0, std::memory_order_release);
        return 1;
    }
    if (g_eventThreadReady)
    {
        SetEvent(g_eventThreadReady);
    }
    Wh_Log(L"Window event thread ready");
    bool running = true;
    HANDLE waitHandles[] = {g_animationTimer, g_eventThreadStop};
    while (running)
    {
        bool frameDue = false;
        DWORD waitResult = MsgWaitForMultipleObjectsEx(
            ARRAYSIZE(waitHandles), waitHandles, INFINITE, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE);
        if (waitResult == WAIT_OBJECT_0)
        {
            frameDue = true;
        }
        else if (waitResult == WAIT_OBJECT_0 + 1)
        {
            running = false;
            continue;
        }
        else if (waitResult != WAIT_OBJECT_0 + ARRAYSIZE(waitHandles))
        {
            Wh_Log(L"Window event thread wait failed: %u", GetLastError());
            break;
        }
        // Bound message work so location floods cannot starve animation.
        int processedMessages = 0;
        LARGE_INTEGER messageBatchStart = {};
        QueryPerformanceCounter(&messageBatchStart);
        while (processedMessages < 16 && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            processedMessages++;
            if (message.message == WM_QUIT)
            {
                running = false;
                break;
            }
            if (message.message == WM_TIMER &&
                message.wParam == g_virtualDesktopRefreshTimer)
            {
                KillTimer(nullptr, g_virtualDesktopRefreshTimer);
                g_virtualDesktopRefreshTimer = 0;
                ULONGLONG now = GetTickCount64();
                if (now < g_virtualDesktopRefreshDeadline)
                {
                    UINT remaining = static_cast<UINT>(
                        std::max<ULONGLONG>(1, g_virtualDesktopRefreshDeadline - now));
                    g_virtualDesktopRefreshTimer = SetTimer(nullptr, 0, remaining, nullptr);
                    if (g_virtualDesktopRefreshTimer)
                    {
                        continue;
                    }
                }
                g_virtualDesktopRefreshDeadline = 0;
                HandleVirtualDesktopSwitch();
                continue;
            }
            if (message.message == WM_WOBBLY_MAXIMIZED_CHANGE)
            {
                // Clear the gate before consuming the latest coalesced HWND.
                g_maximizedStateCheckQueued.store(false, std::memory_order_release);
                HWND maximizedStateWindow =
                    g_pendingMaximizedStateWindow.exchange(nullptr, std::memory_order_acq_rel);
                HandleObservedWindowLocationChange(maximizedStateWindow, OBJID_WINDOW,
                                                   CHILDID_SELF);
                continue;
            }
            if (message.message == WM_WOBBLY_NATIVE_WINDOW_TRANSITION)
            {
                HandleNativeWindowTransitionStart(reinterpret_cast<HWND>(message.wParam),
                                                  message.lParam);
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
            if (g_animationFrequency.QuadPart > 0 && messageBatchStart.QuadPart > 0)
            {
                LARGE_INTEGER messageBatchNow = {};
                if (QueryPerformanceCounter(&messageBatchNow) &&
                    messageBatchNow.QuadPart - messageBatchStart.QuadPart >=
                        g_animationFrequency.QuadPart / 1000)
                {
                    break;
                }
            }
        }
        if (running && frameDue && g_animationClockArmed)
        {
            UpdateAnimationFrame();
        }
    }
    Wh_Log(L"Window event thread stopping");
    g_eventThreadMessageTarget.store(0, std::memory_order_release);
    g_pendingMaximizedStateWindow.store(nullptr, std::memory_order_release);
    g_maximizedStateCheckQueued.store(false, std::memory_order_release);
    UninitializeWindowEventHooks();
    CloseHandle(g_animationTimer);
    g_animationTimer = nullptr;
    g_animationFrequency = {};
    return 0;
}

static void StopWindowEventThread();

static bool StartWindowEventThread()
{
    g_eventThreadStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_eventThreadStop)
    {
        Wh_Log(L"Failed to create event thread stop event");
        return false;
    }
    g_eventThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_eventThreadReady)
    {
        Wh_Log(L"Failed to create event thread ready event");
        CloseHandle(g_eventThreadStop);
        g_eventThreadStop = nullptr;
        return false;
    }
    g_eventThread = CreateThread(nullptr, 0, WindowEventThreadProc, nullptr, 0, &g_eventThreadId);
    if (!g_eventThread)
    {
        Wh_Log(L"Failed to create window event thread");
        CloseHandle(g_eventThreadReady);
        g_eventThreadReady = nullptr;
        CloseHandle(g_eventThreadStop);
        g_eventThreadStop = nullptr;
        return false;
    }
    DWORD waitResult = WaitForSingleObject(g_eventThreadReady, 3000);
    if (waitResult != WAIT_OBJECT_0)
    {
        Wh_Log(L"Window event thread didn't become ready");
        StopWindowEventThread();
        return false;
    }
    return g_moveSizeHook != nullptr && g_locationHook != nullptr;
}

static void StopWindowEventThread()
{
    g_existingWindowBackfillCount.store(0, std::memory_order_release);
    g_existingWindowBackfillIndex.store(0, std::memory_order_release);
    g_existingWindowBackfillMapped.store(0, std::memory_order_release);
    g_eventThreadMessageTarget.store(0, std::memory_order_release);
    if (g_eventThreadStop)
    {
        SetEvent(g_eventThreadStop);
    }
    if (g_eventThread)
    {
        DWORD waitResult = WaitForSingleObject(g_eventThread, INFINITE);
        if (waitResult != WAIT_OBJECT_0)
        {
            Wh_Log(L"Failed waiting for window event thread");
        }
        CloseHandle(g_eventThread);
        g_eventThread = nullptr;
    }
    if (g_eventThreadReady)
    {
        CloseHandle(g_eventThreadReady);
        g_eventThreadReady = nullptr;
    }
    if (g_eventThreadStop)
    {
        CloseHandle(g_eventThreadStop);
        g_eventThreadStop = nullptr;
    }
    g_eventThreadId = 0;
}

static void CalculateAutomaticParameters(int wobbliness, double& stiffness, double& drag,
                                         double& moveFactor)
{
    struct Preset
    {
        double stiffness;
        double drag;
        double moveFactor;
    };

    static constexpr Preset presets[] = {{15.0, 80.0, 10.0},
                                         {10.0, 85.0, 10.0},
                                         {6.0, 90.0, 10.0},
                                         {3.0, 92.0, 20.0},
                                         {1.0, 97.0, 25.0}};
    int presetIndex = std::clamp(wobbliness, 0, static_cast<int>(ARRAYSIZE(presets)) - 1);
    stiffness = presets[presetIndex].stiffness;
    drag = presets[presetIndex].drag;
    moveFactor = presets[presetIndex].moveFactor;
}

static WobblySettings GetSettingsSnapshot()
{
    AcquireSRWLockShared(&g_settingsLock);
    WobblySettings settings = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return settings;
}

static void LoadSettings()
{
    WobblySettings settings = {};
    auto wobblinessPresetSetting = WindhawkUtils::StringSetting::make(L"WobblinessPreset");
    PCWSTR wobblinessPreset = wobblinessPresetSetting.get();
    settings.wobbliness = 2;
    if (wobblinessPreset[0] >= L'0' && wobblinessPreset[0] <= L'4' &&
        wobblinessPreset[1] == L'\0')
    {
        settings.wobbliness = wobblinessPreset[0] - L'0';
    }
    settings.advancedMode = Wh_GetIntSetting(L"AdvancedMode") != 0;
    settings.resizeWobbleEnabled = Wh_GetIntSetting(L"EnableResizeWobble") != 0;
    settings.windowStateWobbleEnabled =
        Wh_GetIntSetting(L"EnableWindowStateWobble") != 0;
    if (settings.advancedMode)
    {
        settings.stiffness = static_cast<double>(Wh_GetIntSetting(L"Stiffness"));
        settings.drag = static_cast<double>(Wh_GetIntSetting(L"Drag"));
        settings.moveFactor = static_cast<double>(Wh_GetIntSetting(L"MoveFactor"));
    }
    else
    {
        CalculateAutomaticParameters(settings.wobbliness, settings.stiffness, settings.drag,
                                     settings.moveFactor);
    }
    // Clamp imported and current settings at the boundary.
    settings.stiffness = std::clamp(settings.stiffness, 1.0, 100.0);
    settings.drag = std::clamp(settings.drag, 1.0, 100.0);
    settings.moveFactor = std::clamp(settings.moveFactor, 1.0, 25.0);
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = settings;
    ReleaseSRWLockExclusive(&g_settingsLock);
    Wh_Log(L"Wobbly Windows settings: "
           L"WobblinessPreset=%d, "
           L"Advanced=%d, "
           L"ResizeWobble=%d, "
           L"WindowStateWobble=%d, "
           L"Stiffness=%.2f, "
           L"Drag=%.2f, "
           L"MoveFactor=%.2f",
           settings.wobbliness, settings.advancedMode, settings.resizeWobbleEnabled,
           settings.windowStateWobbleEnabled, settings.stiffness, settings.drag,
           settings.moveFactor);
}

BOOL Wh_ModInit()
{
    g_unloading.store(false, std::memory_order_release);
    g_desktopManagerThreadIdOffset = SIZE_MAX;
    g_topLevelWindow3DWindowDataOffset = SIZE_MAX;
    g_dwmSceneThreadId.store(0, std::memory_order_release);
    g_dwmCompositor.store(nullptr, std::memory_order_release);
    g_desktopManager.store(nullptr, std::memory_order_release);
    g_proxyCreationDisabled.store(false, std::memory_order_release);
    g_dwmThreadMismatchLogged.store(false, std::memory_order_release);
    g_dwmObjectDiscoveryFailureCount.store(0, std::memory_order_release);
    g_sceneWakeScheduled.store(false, std::memory_order_release);
    g_sceneWakeOutstanding.store(0, std::memory_order_release);
    LARGE_INTEGER wakeTokenCounter = {};
    QueryPerformanceCounter(&wakeTokenCounter);
    INT_PTR wakeToken = static_cast<INT_PTR>(wakeTokenCounter.QuadPart ^ GetTickCount64() ^
                                             reinterpret_cast<UINT_PTR>(&g_dwmSceneWakeToken));
    g_dwmSceneWakeToken.store(wakeToken ? wakeToken : 1, std::memory_order_release);
    g_sceneRequestedSerial.store(0, std::memory_order_release);
    g_sceneSubmittedSerial.store(0, std::memory_order_release);
    g_sceneWakePostTimestamp.store(0, std::memory_order_release);
    g_sceneWakeStallStartedAt.store(0, std::memory_order_release);
    g_sceneWakeRetryCount.store(0, std::memory_order_release);
    g_sceneWakeStalled.store(false, std::memory_order_release);
    g_sceneRecoveryCleanupPending.store(false, std::memory_order_release);
    g_scenePassCounter.store(0, std::memory_order_release);
    g_lastFallbackInvalidationTimestamp.store(0, std::memory_order_release);
    g_abandonedProxyCount.store(0, std::memory_order_release);
    g_sceneOwnershipResetPending.store(false, std::memory_order_release);
    g_windowStateThrobSuppressedUntil.store(0, std::memory_order_release);
    g_existingWindowBackfillCount.store(0, std::memory_order_release);
    g_existingWindowBackfillIndex.store(0, std::memory_order_release);
    g_existingWindowBackfillMapped.store(0, std::memory_order_release);
    g_lastObservedScenePassCounter = 0;
    g_lastSceneProgressTimestamp = 0;
    Wh_Log(L"Wobbly Windows 0.70: initializing");
    InitializeDpiSupport();
    LoadSettings();
    if (!InitializeDwmHooks())
    {
        Wh_Log(L"Wobbly Windows: failed to initialize DWM hooks");
        UninitializeDpiSupport();
        return FALSE;
    }
    if (!StartWindowEventThread())
    {
        Wh_Log(L"Wobbly Windows: failed to initialize event thread");
        StopWindowEventThread();
        UninitializeDpiSupport();
        return FALSE;
    }
    Wh_Log(L"Wobbly Windows: initialized successfully");
    return TRUE;
}

void Wh_ModAfterInit()
{
    // Windhawk activates detours after Wh_ModInit returns. Queue the first
    // scene pass only now so already-open windows cannot miss the wake.
    QueueExistingWindowBackfill();
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"Wobbly Windows: settings changed");
    LoadSettings();
}

void Wh_ModBeforeUninit()
{
    g_unloading.store(true, std::memory_order_release);
    Wh_Log(L"Wobbly Windows: preparing to unload");
    // Restore scene resources before Windhawk removes the hooks.
    StopWindowEventThread();
    UninitializeDpiSupport();
}

void Wh_ModUninit()
{
    Wh_Log(L"Wobbly Windows: unloaded");
}

#endif
