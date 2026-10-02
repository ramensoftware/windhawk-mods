// ==WindhawkMod==
// @id              overhaulded-alt-tab
// @name            OverhauldedWin Alt+Tab
// @description     Replaces the boring Windows Alt+Tab with a modern and elegant window switcher.
// @version         1.2.1
// @author          IMiloDev
// @github          https://github.com/IMiloDev
// @homepage        https://github.com/IMiloDev/OverhauldedWin-Task-Switcher
// @include         windhawk.exe
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Overhaulded Task Switcher

A modern, fluid and highly visual replacement for the native Windows Alt+Tab experience.

Built from scratch in native C++ as a project to explore Windows APIs, graphics, animation systems and desktop customization. 

> Ofc, made as a C++ practice project. Hope you enjoy it as much as I enjoy developing it.
 
## Screenshot

![OverhauldedWin Task Switcher](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/TaskManager.jpg)

## Features

- Modern horizontal task switcher interface
- App grouping by application/process
- Real DWM window previews
- Dynamic Obsidian visual system
- CPU-based desktop blur
- Rounded cards with subtle downward shadows
- Floating task switcher surface
- Fluid opening animation
- Smooth horizontal navigation
- Interruptible navigation animations
- Subtle center-card snap animation
- Hover interactions
- Window closing
- Resolution-aware UI scaling
- High-DPI support
- Alt + Tab support
- Alt + Shift + Tab support
- AltGr + Tab support
- Lightweight native C++ implementation
- Background execution for fast activation

### Another Features

- Visual continuity between selections
- Smooth carousel-style navigation 
- Real window previews
- Application-aware grouping
- Subtle depth and lighting
- Fast activation through background execution

Overhaulded also aims to remain visually distinct from other Windows task-switcher projects while exploring its own interaction and visual language.

## Design

Overhaulded focuses on a dark, minimal interface inspired by modern desktop UI design while keeping the selector feeling native to Windows.

The visual system uses a Black Obsidian surface with:

- Rounded cards
- Subtle downward shadows
- Content-based illumination
- Soft depth separation
- Smooth transitions
- A fixed central visualizer

The central visualizer remains fixed while the cards move through the carousel, creating the impression of navigating through a physical stack of windows rather than moving the entire interface.

## Smooth Animations

### Open / Close

![OverhauldedWin Open Animation](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/Open_n'_Close.webp)

### Navigation

![OverhauldedWin](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/Desplacement.webp)

### Window Close

![OverhauldedWin](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/abort.webp)

## Background Execution

Overhaulded remains prepared in the background instead of creating the entire task-switcher interface from scratch every time Alt+Tab is pressed.

This allows the selector to appear immediately while keeping the visual opening animation smooth.

The architecture separates global input handling from the UI/rendering system so that heavy graphics and window-management operations do not run directly inside low-level keyboard hooks.

## Requirements

- Windows 11 only
- Windhawk

## Installation

1. Install Windhawk.
2. Open the Overhaulded Win Task Switcher mod.
3. Install or compile the latest release.
4. Enable the mod.
5. Press `Alt + Tab` to open Overhaulded.

## Controls

| Shortcut                                            | Action                        |
| --------------------------------------------------- | ----------------------------- |
| `Alt + Tab`                                         | Move to the next window       |
| `Alt + Shift + Tab`                                 | Move to the previous window   |
| `Alt + Tab` + release `Alt`                         | Activate the selected window  |
| `Esc`                                               | Cancel the switcher           |
| `Ctrl + Alt + Tab + Arrow` `Alt + Tab Arrow`        | Additional navigation/control |

## Compatibility

Overhaulded is currently designed specifically for Windows 11.

The project is still under active development, so behavior may vary depending on:

- Display scaling
- Multiple-monitor configurations
- Windows configuration
- Application window types
- Other Alt+Tab/task-switcher modifications

Running multiple applications that replace the native Windows task switcher at the same time may cause conflicts.

## Known Limitations

Overhaulded is still a pre-release project.

Some applications may behave differently from standard desktop windows, particularly applications that use unusual window structures, custom rendering or multiple processes.

Additional compatibility improvements are planned as development continues.

## Development

Overhaulded is primarily a C++ project focused on exploring:

- Win32 APIs
- Windows hooks
- DWM
- Direct2D
- DirectWrite
- GDI
- Window management
- High-DPI rendering
- Desktop animation systems
- Native Windows UI

The project is continuously evolving as new ideas and technical improvements are explored.

## License

This project is licensed under the **MIT License**.

See the [LICENSE](https://github.com/IMiloDev/OverhauldedWin/blob/main/LICENSE) file for the complete license text.

---

**Overhaulded Task Switcher**
Native C++ • Windows 11 • Windhawk
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- VisibleCards: "5"
  $name: Visible cards
  $description: Number of cards shown in the carousel.
  $options:
  - "3": 3 cards
  - "5": 5 cards (default)
- AnimationSpeed: "100"
  $name: Animation speed
  $description: Relative speed of existing animations.
  $options:
  - "75": 75%
  - "100": 100% (default)
  - "125": 125%
  - "150": 150%
- PerformanceMode: "balanced"
  $name: Performance mode
  $description: Controls the animation update frequency.
  $options:
  - "balanced": Balanced (default)
  - "smooth": Smooth
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <utility>


#ifndef WH_MOD_ID
#define WH_MOD_ID L"overhaulded-alt-tab"
#endif

typedef DWORD (WINAPI* GetFileVersionInfoSizeWFn)(LPCWSTR, LPDWORD);
typedef BOOL (WINAPI* GetFileVersionInfoWFn)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI* VerQueryValueWFn)(LPCVOID, LPCWSTR, LPVOID*, PUINT);

// CONFIGURACIÓN

static const wchar_t kWindowClassName[] = L"OverhauldedAltTabSelector";

static const int kSelectorWidth = 1220;
static const int kSelectorHeight = 430;
static const int kMaxCarouselSlots = 5;
static int g_visibleCardCount = 5;
static int g_animationSpeedPercent = 100;
static const int kCounterWidth = 140;
static const int kCounterHeight = 28;
static const int kCounterTop = 378;
// Opacidad común de las superficies Black Obsidian; los textos y thumbnails no se alteran.
static const float kCardSurfaceOpacity = 0.75f;
// Timer del failsafe: solo existe mientras el selector está abierto.
static const UINT_PTR kActivityTimerId = 77;
static const UINT_PTR kAnimTimerId = 88;
static const UINT_PTR kTabRepeatTimerId = 89;
static const UINT_PTR kSelectorMotionTimerId = 90;
static const float kNavigationDuration = 150.0f;
static const int kDefaultAnimationFps = 90;
static int g_animationFps = kDefaultAnimationFps;

static int GetCarouselCenterSlot() { return g_visibleCardCount / 2; }
static int GetCarouselSlotCount() { return g_visibleCardCount; }
static float AnimationDuration(float baseMs)
{
    return baseMs * 100.0f / static_cast<float>(g_animationSpeedPercent);
}
static void LoadTaskSwitcherSettings()
{
    auto read = [](PCWSTR key, PCWSTR fallback) {
        PCWSTR value = Wh_GetStringSetting(key);
        if (!value) return std::wstring(fallback);
        std::wstring result(value);
        Wh_FreeStringSetting(value);
        return result;
    };
    std::wstring visible = read(L"VisibleCards", L"5");
    g_visibleCardCount = visible == L"3" ? 3 : 5;
    std::wstring speed = read(L"AnimationSpeed", L"100");
    g_animationSpeedPercent = (speed == L"75" ? 75 : speed == L"125" ? 125 : speed == L"150" ? 150 : 100);
    std::wstring mode = read(L"PerformanceMode", L"balanced");
    g_animationFps = mode == L"smooth" ? 120 : 90;
}




static UINT GetAnimationTimerInterval()
{
    return static_cast<UINT>(std::max(1, 1000 / g_animationFps));
}
static const UINT kTabRepeatInitialDelayMs = 325;
static const UINT kTabRepeatIntervalMs = 125;
static const ULONGLONG kSelectorFailsafeMs = 5000;
static const float kReferenceScreenWidth = 1366.0f;
static const float kReferenceScreenHeight = 768.0f;
static const float kMinimumUiScale = 0.55f;

struct UIScaleState
{
    float value = 1.0f;
    float dpiScale = 1.0f;
    int screenWidth = 1366;
    int screenHeight = 768;
};
static UIScaleState g_uiScale;
static int g_runtimeSelectorWidth = kSelectorWidth;
static int g_runtimeSelectorHeight = kSelectorHeight;
static float g_sceneScaleX = 1.0f;
static float g_sceneScaleY = 1.0f;
static float g_sceneOpacity = 1.0f;
static float g_sceneTiltDegrees = 0.0f;
static float g_selectionTiltDirection = 0.0f;
static float g_cardSnapScale = 1.0f;
static float g_cardSnapStartScale = 1.0f;

static int ScaleLayoutPx(float value)
{
    return static_cast<int>(roundf(value * g_uiScale.value));
}

// Constantes fijas en píxeles (bordes, radios pequeños) que deben crecer con el
// DPI del monitor pero no con el ajuste "fit" por resolución. A 100% DPI es 1:1.
static float DpiPx(float value)
{
    return value * g_uiScale.dpiScale;
}

// El UI thread es Per-Monitor-V2: todo el layout está en píxeles físicos. La
// escala final combina el DPI real del monitor con el ajuste por área de trabajo
// (medida en DIPs), de modo que a 100% el resultado es idéntico al anterior.
static void UpdateUIScaleForWorkArea(const RECT& work, float dpiScale)
{
    dpiScale = std::max(0.5f, std::min(8.0f, dpiScale));
    g_uiScale.screenWidth = std::max(1, static_cast<int>(work.right - work.left));
    g_uiScale.screenHeight = std::max(1, static_cast<int>(work.bottom - work.top));
    g_uiScale.dpiScale = dpiScale;
    float dipWidth = static_cast<float>(g_uiScale.screenWidth) / dpiScale;
    float dipHeight = static_cast<float>(g_uiScale.screenHeight) / dpiScale;
    float scaleX = dipWidth / kReferenceScreenWidth;
    float scaleY = dipHeight / kReferenceScreenHeight;
    float fit = std::max(kMinimumUiScale, std::min(1.0f, std::min(scaleX, scaleY)));
    g_uiScale.value = fit * dpiScale;
    g_runtimeSelectorWidth = ScaleLayoutPx(static_cast<float>(kSelectorWidth));
    g_runtimeSelectorHeight = ScaleLayoutPx(static_cast<float>(kSelectorHeight));
}

static void GetSceneScale(float& scaleX, float& scaleY)
{
    scaleX = g_sceneScaleX;
    scaleY = g_sceneScaleY;
}

static RECT TransformSceneRect(const RECT& source)
{
    float sx = 1.0f, sy = 1.0f;
    GetSceneScale(sx, sy);
    float cx = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
    float cy = static_cast<float>(g_runtimeSelectorHeight) * 0.5f;
    RECT result = {};
    result.left = static_cast<int>(roundf(cx + (source.left - cx) * sx));
    result.right = static_cast<int>(roundf(cx + (source.right - cx) * sx));
    result.top = static_cast<int>(roundf(cy + (source.top - cy) * sy));
    result.bottom = static_cast<int>(roundf(cy + (source.bottom - cy) * sy));
    return result;
}

typedef HANDLE HTHUMBNAIL;

struct DwmThumbnailPropertiesLocal
{
    DWORD dwFlags;
    RECT rcDestination;
    RECT rcSource;
    BYTE opacity;
    BOOL fVisible;
    BOOL fSourceClientAreaOnly;
};

typedef HRESULT (WINAPI* DwmRegisterThumbnailFn)(HWND, HWND, HTHUMBNAIL*);
typedef HRESULT (WINAPI* DwmUnregisterThumbnailFn)(HTHUMBNAIL);
typedef HRESULT (WINAPI* DwmUpdateThumbnailPropertiesFn)(
    HTHUMBNAIL, const DwmThumbnailPropertiesLocal*);
typedef HRESULT (WINAPI* DwmSetWindowAttributeFn)(HWND, DWORD,
                                                   const void*, DWORD);
typedef HRESULT (WINAPI* DwmGetWindowAttributeFn)(HWND, DWORD,
                                                   void*, DWORD);
struct MARGINS
{
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
};

// Atributos y flags DWM usados por el glass y las miniaturas.
static const DWORD kDwmWindowCornerPreference = 33;
static const DWORD kDwmBorderColor = 34;
// COLORREF en formato 0x00BBGGRR: obsidiana acorde al fondo general.
static const DWORD kDwmGlassBorderColor = 0x000D0A08;
static const DWORD kDwmNcRenderingPolicy = 2;
static const DWORD kDwmNcRenderingPolicyDisabled = 1;
static const DWORD kDwmWaSystemBackdropType = 38;
static const DWORD kDwmSbtNone = 1;

static const DWORD kDwmTnpRectDestination = 0x00000001;
static const DWORD kDwmTnpRectSource = 0x00000002;
static const DWORD kDwmTnpOpacity = 0x00000004;
static const DWORD kDwmTnpVisible = 0x00000008;

struct CardStyleConfig
{
    bool glowEnabled = true;
    int glowIntensity = 1;
    COLORREF borderColor = RGB(10, 11, 13);
    COLORREF selectedBorderColor = RGB(20, 21, 23);
    COLORREF cardBackground = RGB(5, 6, 7);
    COLORREF selectedCardBackground = RGB(10, 10, 11);
    int cornerRadius = 12;
    int selectedCornerRadius = 14;
};

static CardStyleConfig g_cardStyle;

// ESTADO

enum class SelectorState
{
    Idle,
    SelectorActive,
    Confirming,
    Canceling
};
enum class SelectorAnimationState
{
    None,
    Opening,
    SelectionChange,
    CardExit,
    Open,
    Closing
};

enum class ModifierSession
{
    None,
    LeftAlt,
    RightAlt,
    AltGr
};

struct MediaAccent
{
    COLORREF color = RGB(0, 0, 0);
    float strength = 0.0f;
    bool valid = false;
};

struct AppGroup
{
    DWORD processId;
    std::wstring appName;
    std::wstring processPath;
    std::vector<HWND> windows;
    HWND representativeWindow;
    HICON icon;
    ID2D1Bitmap* iconBitmap;
    ID2D1LinearGradientBrush* surfaceBrushNormal;
    ID2D1LinearGradientBrush* surfaceBrushSelected;
    bool ownIcon;
    MediaAccent mediaAccent;
};

struct UserWindowInfo
{
    HWND hwnd;
    DWORD processId;
    std::wstring processPath;
    std::wstring appName;
    ULONGLONG lastActivated;
};

// ARQUITECTURA DE HILOS
//
//  Input thread : dueño de WH_KEYBOARD_LL y WH_MOUSE_LL. Los callbacks solo
//                 actualizan estado atómico/local y publican comandos al UI
//                 thread con PostThreadMessage. Nada más.
//  UI thread    : dueño del HWND del selector, D2D/DWrite/GDI/DWM, blur,
//                 animaciones, timers, WinEvent hook, enumeración de ventanas,
//                 iconos, información de procesos y activación de ventanas.
//
// Variables de un solo dueño: las "input" solo las toca el input thread y las
// "ui" solo las toca el UI thread. Lo compartido usa Interlocked*.

static HMODULE g_hModule = nullptr;

// Workers (creados/cerrados por WhTool_ModInit / WhTool_ModUninit).
static HANDLE g_inputThread = nullptr;
static HANDLE g_uiThread = nullptr;
static HANDLE g_inputReadyEvent = nullptr;
static HANDLE g_uiReadyEvent = nullptr;
static DWORD g_inputThreadId = 0;
static volatile DWORD g_uiThreadId = 0;
static volatile LONG g_inputInitOk = 0;
static volatile LONG g_uiInitOk = 0;
static volatile LONG g_shutdownRequested = 0;

// Solo input thread.
static HHOOK g_keyboardHook = nullptr;
static HHOOK g_mouseHook = nullptr;
static HWND volatile g_inputControlWnd = nullptr;   // ventana message-only

// Estado compartido entre hooks y UI.
static volatile LONG g_nextSessionId = 0;
// Id de la sesión de selector actualmente activa (0 = ninguna). Lo publica el
// input thread al abrir; el UI thread solo lo limpia con CAS sobre su propio id.
static volatile LONG g_sessionActive = 0;
// 1 mientras el selector acepta interacción de ratón (el UI thread lo gobierna).
static volatile LONG g_uiSelectorOpen = 0;
static volatile LONG64 g_lastMousePt = 0;
static volatile LONG g_mouseMovePending = 0;

// Comandos input -> UI (PostThreadMessage) y UI -> input (ventana de control).
static const UINT WM_UI_OPEN = WM_APP + 1;         // wParam=sessionId, lParam=shift
static const UINT WM_UI_TAB_DOWN = WM_APP + 2;     // wParam=sessionId, lParam=shift
static const UINT WM_UI_TAB_UP = WM_APP + 3;       // wParam=sessionId
static const UINT WM_UI_NAVIGATE = WM_APP + 4;     // wParam=sessionId, lParam=+1/-1
static const UINT WM_UI_CONFIRM = WM_APP + 5;      // wParam=sessionId
static const UINT WM_UI_CANCEL = WM_APP + 6;       // wParam=sessionId
static const UINT WM_UI_MOUSE_MOVE = WM_APP + 7;   // posición en g_lastMousePt
static const UINT WM_UI_MOUSE_BUTTON = WM_APP + 8; // wParam=mensaje, lParam=pt (16 bit)
static const UINT WM_UI_MOUSE_WHEEL = WM_APP + 9;  // wParam=delta (short)
static const UINT WM_UI_SETTINGS = WM_APP + 10;
static const UINT WM_IN_MOUSEHOOK = WM_APP + 20;   // wParam=1 instalar, 0 quitar
static const UINT WM_IN_ACTIVATE = WM_APP + 21;     // wParam=target HWND

// Solo UI thread.
static HWND g_selector = nullptr;
static bool g_classesRegistered = false;
static LONG g_uiSessionId = 0;
static bool g_uiTabDown = false;
static HWINEVENTHOOK g_winEventHook = nullptr;
static HWND g_foregroundRetryWindow = nullptr;
static UINT_PTR g_foregroundRetryTimer = 0;
static HMODULE g_shcoreApi = nullptr;
typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
static GetDpiForMonitorFn g_getDpiForMonitor = nullptr;

// Colección visual estable: una ranura = un thumbnail DWM reutilizado.
struct ThumbnailSlot
{
    HTHUMBNAIL thumbnail;
    HWND source;
};

static ThumbnailSlot g_thumbnailSlots[kMaxCarouselSlots] = {};
static HMODULE g_dwmApi = nullptr;
static DwmRegisterThumbnailFn g_dwmRegisterThumbnail = nullptr;
static DwmUnregisterThumbnailFn g_dwmUnregisterThumbnail = nullptr;
static DwmUpdateThumbnailPropertiesFn g_dwmUpdateThumbnailProperties = nullptr;
static DwmSetWindowAttributeFn g_dwmSetWindowAttribute = nullptr;
static DwmGetWindowAttributeFn g_dwmGetWindowAttribute = nullptr;
static bool g_dwmGlassEnabled = false;

// Selección lógica vs. posición animada. La colección de AppGroups no se toca.
static float g_animOffset = 0.0f;
static float g_animStartOffset = 0.0f;
static ULONGLONG g_animStartTime = 0;
static bool g_animActive = false;

// Hover es exclusivamente visual y nunca sustituye a g_selected.
static int g_hoveredSlot = -1;
static bool g_hoveredCloseButton = false;
static bool g_tabRepeatStarted = false;
static bool g_cleanupInProgress = false;
static bool g_selectorOpening = false;
static ULONGLONG g_lastSelectorActivity = 0;
static SelectorAnimationState g_selectorAnimation = SelectorAnimationState::None;
static ULONGLONG g_selectorAnimationStart = 0;
static float g_selectionStartScaleX = 1.0f;
static float g_selectionStartScaleY = 1.0f;
static float g_selectionStartOpacity = 1.0f;
static float g_selectionStartTiltDegrees = 0.0f;
static float g_closeStartScaleX = 1.0f;
static float g_closeStartScaleY = 1.0f;
static float g_closeStartOpacity = 1.0f;
static HWND g_pendingActivationTarget = nullptr;
static HWND g_selectorOriginWindow = nullptr;
static bool g_cardExitActive = false;
static int g_cardExitSlot = -1;
static int g_cardExitGroupIndex = -1;
static HWND g_cardExitTarget = nullptr;
static float g_cardExitDirection = 1.0f;
static ULONGLONG g_cardExitStart = 0;

// DIRECT2D Y DIRECTWRITE

typedef HRESULT (WINAPI* D2D1CreateFactoryFn)(
    D2D1_FACTORY_TYPE factoryType,
    REFIID riid,
    const D2D1_FACTORY_OPTIONS* pFactoryOptions,
    void** ppIFactory
);

typedef HRESULT (WINAPI* DWriteCreateFactoryFn)(
    DWRITE_FACTORY_TYPE factoryType,
    REFIID iid,
    IUnknown** factory
);

static const IID kIidD2D1Factory = {
    0x06152247, 0x6f50, 0x465a, { 0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07 }
};

static const IID kIidDWriteFactory = {
    0xb859ee5a, 0xd838, 0x4b5b, { 0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48 }
};

static HMODULE g_d2dApi = nullptr;
static HMODULE g_dwriteApi = nullptr;

static ID2D1Factory* g_d2dFactory = nullptr;
static ID2D1DCRenderTarget* g_d2dDCRenderTarget = nullptr;
static ID2D1SolidColorBrush* g_d2dBrush = nullptr;
static ID2D1RadialGradientBrush* g_closeAccentBrush = nullptr;

static IDWriteFactory* g_dwriteFactory = nullptr;
static IDWriteTextFormat* g_dwriteSelectedFormat = nullptr;
static IDWriteTextFormat* g_dwriteNormalFormat = nullptr;
static IDWriteTextFormat* g_dwriteCounterFormat = nullptr;
static IDWriteTextFormat* g_dwriteCloseFormat = nullptr;
// Escala con la que se construyeron los TextFormat / fuentes GDI actuales.
static float g_dwriteFormatsScale = 0.0f;
static float g_gdiFontsScale = 0.0f;

typedef HFONT (WINAPI* CreateFontIndirectWFn)(const LOGFONTW*);
typedef BOOL (WINAPI* DeleteObjectFn)(HGDIOBJ);
typedef HRGN (WINAPI* CreateRoundRectRgnFn)(int, int, int, int, int, int);
typedef HGDIOBJ (WINAPI* GetStockObjectFn)(int);
typedef int (WINAPI* SetBkModeFn)(HDC, int);
typedef COLORREF (WINAPI* SetTextColorFn)(HDC, COLORREF);
typedef HBRUSH (WINAPI* CreateSolidBrushFn)(COLORREF);
typedef HPEN (WINAPI* CreatePenFn)(int, int, COLORREF);
typedef HGDIOBJ (WINAPI* SelectObjectFn)(HDC, HGDIOBJ);
typedef BOOL (WINAPI* RoundRectFn)(HDC, int, int, int, int, int, int);
typedef int (WINAPI* GetTextFaceWFn)(HDC, int, LPWSTR);
typedef int (WINAPI* GetDIBitsFn)(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);
typedef int (WINAPI* GetObjectWFn)(HGDIOBJ, int, LPVOID);
typedef HDC (WINAPI* CreateCompatibleDCFn)(HDC);
typedef HBITMAP (WINAPI* CreateDIBSectionFn)(HDC, const BITMAPINFO*, UINT, void**, HANDLE*, DWORD);
typedef BOOL (WINAPI* StretchBltFn)(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);
typedef int (WINAPI* SetStretchBltModeFn)(HDC, int);
typedef BOOL (WINAPI* SetBrushOrgExFn)(HDC, int, int, LPPOINT);
typedef BOOL (WINAPI* BitBltFn)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef int (WINAPI* GetDIBitsFn)(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);
typedef BOOL (WINAPI* DeleteDCFn)(HDC);

static HMODULE g_gdiApi = nullptr;
static CreateFontIndirectWFn g_createFontIndirectW = nullptr;
static DeleteObjectFn g_deleteObject = nullptr;
static CreateRoundRectRgnFn g_createRoundRectRgn = nullptr;
static GetStockObjectFn g_getStockObject = nullptr;
static SetBkModeFn g_setBkMode = nullptr;
static SetTextColorFn g_setTextColor = nullptr;
static CreateSolidBrushFn g_createSolidBrush = nullptr;
static CreatePenFn g_createPen = nullptr;
static SelectObjectFn g_selectObject = nullptr;
static RoundRectFn g_roundRect = nullptr;
static GetTextFaceWFn g_getTextFaceW = nullptr;
static GetDIBitsFn g_getDIBits = nullptr;
static GetObjectWFn g_getObjectW = nullptr;
static CreateCompatibleDCFn g_createCompatibleDC = nullptr;
static CreateDIBSectionFn g_createDIBSection = nullptr;
static StretchBltFn g_stretchBlt = nullptr;
static SetStretchBltModeFn g_setStretchBltMode = nullptr;
static SetBrushOrgExFn g_setBrushOrgEx = nullptr;
static BitBltFn g_bitBlt = nullptr;
static DeleteDCFn g_deleteDC = nullptr;

static HFONT g_nameFont = nullptr;
static HFONT g_selectedNameFont = nullptr;
static HFONT g_secondaryFont = nullptr;

static std::vector<AppGroup> g_groups;
static std::vector<UserWindowInfo> g_userWindows;
// UI thread only. A window remains here after WM_CLOSE is posted until it
// actually disappears, so a synchronous refresh cannot re-add its card.
static std::vector<HWND> g_pendingCloseWindows;
static std::vector<std::pair<DWORD, size_t>> g_groupWindowCursors;
static int g_selected = 0;
static SelectorState g_state = SelectorState::Idle;

// Variables para el blur del fondo
static HDC g_blurDC = nullptr;
static HBITMAP g_blurBitmap = nullptr;
static HBITMAP g_blurOldBitmap = nullptr;
static unsigned char* g_blurPixels = nullptr;
static int g_blurWidth = 0;
static int g_blurHeight = 0;
static bool g_blurBackgroundCreated = false;
// El fondo se procesa a media resolución del área realmente capturada.
// Los límites evitan imágenes demasiado pequeñas en resoluciones compactas.
static const int kBlurDownscaleNumerator = 1;
static const int kBlurDownscaleDenominator = 2;
static const int kBlurMinimumWidth = 320;
static const int kBlurMinimumHeight = 180;
static const int kBlurPassCount = 3;
static const int kBlurPassRadius = 2;
// Reduce los halos de color producidos por el antialiasing subpíxel del
// escritorio al hacer downscale/upscale de texto blanco.
// Corrección cromática ligera: conserva los colores del escritorio y solo
// atenúa los bordes RGB más agresivos del antialiasing subpíxel.
static const float kBlurChromaSuppression = 0.18f;
static const float kBlurPreChromaSuppression = 0.16f;
static int g_blurCaptureX = 0;
static int g_blurCaptureY = 0;
static ID2D1Bitmap* g_blurD2DBitmap = nullptr;

static bool LoadGdiFunctions();
static bool LoadDwmFunctions();
static HICON GetApplicationIcon(HWND hwnd, const std::wstring& processPath, bool* outOwnIcon);
static void ReleaseGroupResources();
static void CancelSelection();
static void ConfirmSelection();
static void CleanupKeyboardState();
static void EmergencyCloseSelector();
static void StartSelectorClose(HWND target);
static void UpdateSelectorMotion();
static float EaseOutCubic(float t);
static void UpdateCarouselAnimation(HWND hwnd);
static float GetCardExitProgress();
static float GetCardExitOpacity();
static float GetCardExitOffsetY();
static void DestroySelector();
static bool InitializePersistentSelector();
static void HideSelectorForSession();
static void ActivateWindow(HWND target);
static HWND GetNextGroupWindow(AppGroup& group);
static void StartSlide(int steps);
static void SelectNextSmooth(int steps = 1);
static void SelectPreviousSmooth(int steps = 1);
static void SelectNext();
static void SelectPrevious();
static void StopCarouselAnimation();
static void UpdateSelectorControls();
static void UpdateThumbnailSlots();
static RECT GetSlotCardRect(int slot);
static RECT GetSlotHeaderRect(int slot);
static RECT GetSlotPreviewRect(int slot);
static RECT GetCounterRect();
static RECT GetCloseButtonRect(int slot);
static RECT GetCloseHoverButtonRect(int slot);
static ID2D1PathGeometry* CreateCloseButtonGeometry(const RECT& rect);
static int GetSlotAtPoint(POINT ptClient);
static int ResolveGroupIndex(int slot);
static void UpdateHoveredSlot(POINT ptClient);
static void CloseWindowForSlot(int slot);
static ID2D1RadialGradientBrush* EnsureCloseAccentBrush();
static void PaintSelectorScene(HWND hwnd, HDC hdc);
static void PaintBottomShadowD2D(const RECT& objectRect, float radius,
                                 float strength);
static void CreateBlurBackground(int captureX, int captureY);
static void ReleaseBlurBackground();
static void ApplyBlurToPixels(unsigned char* pixels, int width, int height, int radius);
static MediaAccent ExtractMediaAccentFromImage(HICON icon);
static D2D1_COLOR_F AddMediaAccent(const D2D1_COLOR_F& base,
                                   const MediaAccent& accent, float influence);
static void UiEndSession();
static void LeaveActiveState(SelectorState newState);

// ESTADO DEL TECLADO (solo input thread)

static ModifierSession g_sessionModifier = ModifierSession::None;
static LONG g_inputSessionId = 0;
static bool g_altLeftDown = false;
static bool g_altRightDown = false;
static bool g_ctrlLeftDown = false;
static bool g_ctrlRightDown = false;
static bool g_tabDown = false;
static bool g_altGrActive = false;
static bool g_tabSuppressed = false;
// Se arma únicamente cuando AltGr+Tab pertenece a nuestro selector; permite
// bloquear Ctrl+Alt+Flecha antes de que el selector termine de mostrarse.
static bool g_altGrTaskSwitcherArmed = false;
static bool g_altMenuMaskPending = false;

// UTILIDADES DE ESTADO

// Los hooks nunca llaman a las APIs de ventana desde aquí: solo publican.
static bool PostUiCommand(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return false;
    DWORD threadId = g_uiThreadId;
    if (!threadId)
        return false;
    return PostThreadMessageW(threadId, message, wParam, lParam) != FALSE;
}

static void PostInputControl(UINT message, WPARAM wParam)
{
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;
    HWND control = g_inputControlWnd;
    if (control)
        PostMessageW(control, message, wParam, 0);
}

// El UI thread activa/desactiva la interacción de ratón; el input thread
// instala el WH_MOUSE_LL solo mientras el selector acepta interacción.
static void UiSetInteractive(bool on)
{
    LONG next = on ? 1 : 0;
    LONG previous = InterlockedExchange(&g_uiSelectorOpen, next);
    if (previous != next)
        PostInputControl(WM_IN_MOUSEHOOK, static_cast<WPARAM>(next));
}

// Termina la sesión desde el lado UI. CAS: nunca pisa una sesión más nueva.
static void UiEndSession()
{
    UiSetInteractive(false);
    LONG id = g_uiSessionId;
    if (id != 0)
        InterlockedCompareExchange(&g_sessionActive, 0, id);
}

static void LeaveActiveState(SelectorState newState)
{
    g_state = newState;
    UiEndSession();
}

static bool IsSelectorActuallyActive()
{
    return g_state == SelectorState::SelectorActive &&
           g_selector != nullptr && IsWindow(g_selector) != FALSE;
}

static bool IsSelectorActive()
{
    return IsSelectorActuallyActive();
}

static void MarkSelectorActivity()
{
    g_lastSelectorActivity = GetTickCount64();
}

// Estado del teclado del input thread. Se sincroniza de GetAsyncKeyState solo al
// iniciar; después se mantiene únicamente con los eventos del hook.
static void InputResetKeyboardState()
{
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    g_tabDown = false;
    g_altGrActive = false;
    g_tabSuppressed = false;
    g_altGrTaskSwitcherArmed = false;
    g_altMenuMaskPending = false;
    g_sessionModifier = ModifierSession::None;
    g_inputSessionId = 0;
}

static bool IsAltGrPhysicallyDown()
{
    return g_altRightDown && (g_ctrlLeftDown || g_ctrlRightDown);
}

static void ClearSelectorKeyboardSession()
{
    g_tabDown = false;
    g_tabSuppressed = false;
    g_sessionModifier = ModifierSession::None;
    g_inputSessionId = 0;
    g_altGrActive = IsAltGrPhysicallyDown();
    // Si AltGr sigue físicamente pulsado, Ctrl+Alt+Flecha debe seguir bloqueado
    // hasta soltarlo (p. ej. tras Esc); se desarma en el keyup del modificador.
    g_altGrTaskSwitcherArmed = g_altGrTaskSwitcherArmed && g_altGrActive;
}

// Solo UI thread: reconcilia el estado visual/temporizado al cerrar.
static void CleanupKeyboardState()
{
    if (g_cleanupInProgress)
        return;

    g_cleanupInProgress = true;
    g_uiTabDown = false;
    g_tabRepeatStarted = false;
    g_lastSelectorActivity = 0;
    g_cleanupInProgress = false;
}

// ENUMERACIÓN DE VENTANAS Y FILTROS

static bool IsShellWindow(HWND hwnd)
{
    if (hwnd == GetShellWindow())
        return true;
    if (hwnd == FindWindowW(L"Progman", nullptr))
        return true;
    if (hwnd == FindWindowW(L"WorkerW", nullptr))
        return true;
    if (hwnd == FindWindowW(L"Shell_TrayWnd", nullptr))
        return true;
    if (hwnd == FindWindowW(L"Shell_SecondaryTrayWnd", nullptr))
        return true;
    return false;
}

static std::wstring BaseNameWithoutExtension(const std::wstring& path)
{
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos)
        name.resize(dot);
    return name.empty() ? L"Aplicación" : name;
}

static std::wstring LowerAscii(std::wstring value)
{
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (value[i] >= L'A' && value[i] <= L'Z')
            value[i] = static_cast<wchar_t>(value[i] + (L'a' - L'A'));
    }
    return value;
}

static bool ContainsInsensitive(const std::wstring& value, const wchar_t* fragment)
{
    return LowerAscii(value).find(LowerAscii(fragment)) != std::wstring::npos;
}

static std::wstring TrimDisplayName(const std::wstring& value)
{
    size_t first = 0;
    while (first < value.size() && (value[first] == L' ' || value[first] == L'\t' ||
                                    value[first] == L'\r' || value[first] == L'\n'))
        ++first;
    size_t last = value.size();
    while (last > first && (value[last - 1] == L' ' || value[last - 1] == L'\t' ||
                            value[last - 1] == L'\r' || value[last - 1] == L'\n'))
        --last;
    return value.substr(first, last - first);
}

static std::wstring GetFileDescription(const std::wstring& path)
{
    if (path.empty())
        return std::wstring();

    HMODULE version = LoadLibraryW(L"version.dll");
    if (!version)
        return std::wstring();
    GetFileVersionInfoSizeWFn getSize = reinterpret_cast<GetFileVersionInfoSizeWFn>(
        GetProcAddress(version, "GetFileVersionInfoSizeW"));
    GetFileVersionInfoWFn getInfo = reinterpret_cast<GetFileVersionInfoWFn>(
        GetProcAddress(version, "GetFileVersionInfoW"));
    VerQueryValueWFn query = reinterpret_cast<VerQueryValueWFn>(
        GetProcAddress(version, "VerQueryValueW"));
    if (!getSize || !getInfo || !query)
    {
        FreeLibrary(version);
        return std::wstring();
    }

    DWORD handle = 0;
    DWORD size = getSize(path.c_str(), &handle);
    if (size == 0 || size > 1024 * 1024)
    {
        FreeLibrary(version);
        return std::wstring();
    }
    std::vector<BYTE> data(size);
    std::wstring description;
    if (getInfo(path.c_str(), 0, size, data.data()))
    {
        struct Translation { WORD language; WORD codePage; };
        Translation* translations = nullptr;
        UINT translationBytes = 0;
        if (query(data.data(), L"\\VarFileInfo\\Translation",
                  reinterpret_cast<LPVOID*>(&translations), &translationBytes) &&
            translationBytes >= sizeof(Translation))
        {
            const UINT count = translationBytes / sizeof(Translation);
            for (UINT i = 0; i < count && description.empty(); ++i)
            {
                wchar_t subBlock[128] = {};
                wsprintfW(subBlock, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                          translations[i].language, translations[i].codePage);
                LPWSTR value = nullptr;
                UINT valueChars = 0;
                if (query(data.data(), subBlock, reinterpret_cast<LPVOID*>(&value), &valueChars) &&
                    value && valueChars > 0)
                    description.assign(value, valueChars > 0 && value[valueChars - 1] == L'\0' ? valueChars - 1 : valueChars);
            }
        }
        if (description.empty())
        {
            LPWSTR value = nullptr;
            UINT valueChars = 0;
            if (query(data.data(), L"\\StringFileInfo\\040904b0\\FileDescription",
                      reinterpret_cast<LPVOID*>(&value), &valueChars) &&
                value && valueChars > 0)
                description.assign(value, valueChars > 0 && value[valueChars - 1] == L'\0' ? valueChars - 1 : valueChars);
        }
    }
    FreeLibrary(version);
    return TrimDisplayName(description);
}

static std::wstring GetHumanApplicationName(const std::wstring& path)
{
    static std::vector<std::pair<std::wstring, std::wstring>> nameCache;
    for (size_t i = 0; i < nameCache.size(); ++i)
    {
        if (nameCache[i].first == path)
            return nameCache[i].second;
    }

    std::wstring description = GetFileDescription(path);
    std::wstring result = description.empty() ? BaseNameWithoutExtension(path) : description;
    nameCache.push_back(std::make_pair(path, result));
    return result;
}

static bool GetProcessDetails(DWORD processId, std::wstring* path, std::wstring* appName)
{
    if (path)
        path->clear();
    if (appName)
        appName->clear();

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process)
        return false;

    wchar_t buffer[1024] = {};
    DWORD length = ARRAYSIZE(buffer);
    BOOL ok = QueryFullProcessImageNameW(process, 0, buffer, &length);
    CloseHandle(process);
    if (!ok || length == 0)
        return false;

    std::wstring fullPath(buffer, length);
    if (path)
        *path = fullPath;
    if (appName)
        *appName = GetHumanApplicationName(fullPath);
    return true;
}

enum class WindowClassification
{
    RealApplication,
    AuxiliaryWindow,
    SystemWindow,
    Unknown
};

static bool IsFullscreenApplicationWindow(HWND hwnd)
{
    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect))
        return false;

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor)
        return false;

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo))
        return false;

    // Borderless fullscreen windows are commonly WS_POPUP windows without
    // WS_EX_APPWINDOW or a normal frame. Accept only a top-level window whose
    // bounds cover an entire monitor (or its work area), not every popup.
    const LONG tolerance = 4;
    const RECT* candidates[] = { &monitorInfo.rcMonitor, &monitorInfo.rcWork };
    for (size_t i = 0; i < ARRAYSIZE(candidates); ++i)
    {
        const RECT& bounds = *candidates[i];
        if (windowRect.left <= bounds.left + tolerance &&
            windowRect.top <= bounds.top + tolerance &&
            windowRect.right >= bounds.right - tolerance &&
            windowRect.bottom >= bounds.bottom - tolerance)
            return true;
    }
    return false;
}

static WindowClassification ClassifyApplicationWindow(
    HWND hwnd, DWORD processId, const std::wstring& processPath,
    const std::wstring& appName, const std::wstring& windowClass,
    bool isCodeProcess)
{
    (void)processId;

    std::wstring name = LowerAscii(appName);
    std::wstring executableName = LowerAscii(BaseNameWithoutExtension(processPath));
    std::wstring cls = LowerAscii(windowClass);

    if (isCodeProcess)
    {
        LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                        WS_SYSMENU)) != 0;
        bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;

        if (!GetWindow(hwnd, GW_OWNER) && (hasNormalFrame || hasApplicationStyle))
            return WindowClassification::RealApplication;
    }

    if (name == L"textinputhost" || executableName == L"textinputhost" ||
        name == L"spotifyxboxgamebarweb" || executableName == L"spotifyxboxgamebarweb" ||
        name == L"searchhost" || executableName == L"searchhost" ||
        name == L"startmenuexperiencehost" || executableName == L"startmenuexperiencehost" ||
        name == L"shellexperiencehost" || executableName == L"shellexperiencehost" ||
        name == L"runtimebroker" || executableName == L"runtimebroker" ||
        name == L"lockapp" || executableName == L"lockapp" ||
        name == L"xboxgamebar" || executableName == L"xboxgamebar" ||
        name == L"gamebar" || executableName == L"gamebar")
        return WindowClassification::SystemWindow;

    if (cls == L"windows.ui.core.corewindow" ||
        cls == L"inputhost" || cls == L"textservicesframework")
        return WindowClassification::AuxiliaryWindow;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_NOACTIVATE) != 0)
        return WindowClassification::AuxiliaryWindow;

    bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;
    bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                    WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                    WS_SYSMENU)) != 0;
    bool isPopup = (style & WS_POPUP) != 0;

    if (name == L"systeminfo" && !hasApplicationStyle && !hasNormalFrame)
        return WindowClassification::AuxiliaryWindow;
    if (!hasApplicationStyle && !hasNormalFrame &&
        IsFullscreenApplicationWindow(hwnd))
        return WindowClassification::RealApplication;
    if (!hasApplicationStyle && !hasNormalFrame && isPopup)
        return WindowClassification::AuxiliaryWindow;

    if (!GetWindow(hwnd, GW_OWNER) && (hasApplicationStyle || hasNormalFrame))
        return WindowClassification::RealApplication;

    return WindowClassification::Unknown;
}

static int FindAppGroup(DWORD processId)
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].processId == processId)
            return static_cast<int>(i);
    }
    return -1;
}

// Ventanas legítimas que Windows/host marcan como owned o ToolWindow y que el
// filtro estricto descartaba (consolas, terminales, visor/editor de Windhawk).
// Solo se consulta para ventanas que ya iban a ser rechazadas por GW_OWNER o
// WS_EX_TOOLWINDOW: no convierte todas las ToolWindow en aplicaciones.
static bool IsConsoleOrWindhawkViewerWindow(HWND hwnd, DWORD processId,
                                            const std::wstring& processPath,
                                            const std::wstring& appName,
                                            const std::wstring& windowClass)
{
    // Nunca ventanas de nuestro propio proceso (selector, ventanas internas).
    if (processId == GetCurrentProcessId())
        return false;

    std::wstring name = LowerAscii(appName);
    std::wstring executableName = LowerAscii(BaseNameWithoutExtension(processPath));
    std::wstring cls = LowerAscii(windowClass);

    // Consolas clásicas (cmd/PowerShell en conhost) y Windows Terminal.
    if (cls == L"consolewindowclass" || cls == L"cascadia_hosting_window_class")
        return true;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    bool hasNormalFrame = (style & (WS_CAPTION | WS_THICKFRAME |
                                    WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                                    WS_SYSMENU)) != 0;
    bool hasApplicationStyle = (exStyle & WS_EX_APPWINDOW) != 0;
    // Exigir marco/AppWindow descarta ventanas auxiliares (IME, tooltips...)
    // que pertenezcan al mismo proceso.
    if (!hasNormalFrame && !hasApplicationStyle)
        return false;

    if (name == L"cmd" || executableName == L"cmd" ||
        name == L"powershell" || executableName == L"powershell" ||
        name == L"pwsh" || executableName == L"pwsh" ||
        name == L"openconsole" || executableName == L"openconsole" ||
        name == L"windowsterminal" || executableName == L"windowsterminal" ||
        name == L"wt" || executableName == L"wt")
        return true;

    // Visor/editor de código de Windhawk (ventana Chromium del proceso UI).
    if ((name == L"windhawk" || executableName == L"windhawk") && cls == L"chrome_widgetwin_1")
        return true;

    return false;
}

static bool IsRealUserApplicationWindow(HWND hwnd, DWORD* processId,
                                        std::wstring* processPath,
                                        std::wstring* appName)
{
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd))
        return false;
    if (LoadDwmFunctions() && g_dwmGetWindowAttribute)
    {
        constexpr DWORD kDwmWaCloaked = 14;
        BOOL cloaked = FALSE;
        if (SUCCEEDED(g_dwmGetWindowAttribute(hwnd, kDwmWaCloaked,
                                               &cloaked, sizeof(cloaked))) &&
            cloaked)
            return false;
    }
    if (hwnd == g_selector || IsShellWindow(hwnd))
        return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_NOACTIVATE) != 0)
        return false;

    // Owned / ToolWindow: rechazo por defecto, salvo la allowlist de abajo.
    const bool ownedOrToolWindow =
        GetWindow(hwnd, GW_OWNER) != nullptr ||
        (exStyle & WS_EX_TOOLWINDOW) != 0;

    wchar_t title[512] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title) - 1);
    if (title[0] == L'\0')
        return false;

    wchar_t windowClass[256] = {};
    GetClassNameW(hwnd, windowClass, ARRAYSIZE(windowClass) - 1);

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || pid == 0)
        return false;

    if (ownedOrToolWindow)
    {
        std::wstring allowPath;
        std::wstring allowName;
        GetProcessDetails(pid, &allowPath, &allowName);
        if (!IsConsoleOrWindhawkViewerWindow(hwnd, pid, allowPath, allowName, windowClass))
            return false;

        // Allowlist: se acepta directamente. El resto de filtros (que exigen
        // ventana sin owner) no aplican a estas ventanas.
        if (allowName.empty())
            allowName = title;
        if (processId)
            *processId = pid;
        if (processPath)
            *processPath = allowPath;
        if (appName)
            *appName = allowName;
        return true;
    }

    std::wstring path;
    std::wstring name;
    GetProcessDetails(pid, &path, &name);

    std::wstring lowerTitle = LowerAscii(title);
    std::wstring lowerClass = LowerAscii(windowClass);

    bool isCode = (LowerAscii(name) == L"code" ||
                   LowerAscii(name) == L"code - insiders" ||
                   LowerAscii(name) == L"vscodium" ||
                   ContainsInsensitive(path, L"code.exe") ||
                   ContainsInsensitive(path, L"vscodium.exe") ||
                   lowerTitle.find(L"visual studio code") != std::wstring::npos ||
                   lowerTitle.find(L"vscodium") != std::wstring::npos ||
                   (lowerClass == L"chrome_widgetwin_1" && lowerTitle.find(L"code") != std::wstring::npos));

    if (name.empty())
    {
        name = isCode ? L"Visual Studio Code" : title;
    }
    else if (isCode && name == L"Code")
    {
        name = L"Visual Studio Code";
    }

    WindowClassification classification = ClassifyApplicationWindow(
        hwnd, pid, path, name, windowClass, isCode);
    if (classification != WindowClassification::RealApplication)
        return false;

    std::wstring displayName = name;
    if (LowerAscii(BaseNameWithoutExtension(path)) == L"applicationframehost")
    {
        std::wstring titleName = TrimDisplayName(title);
        if (!titleName.empty())
            displayName = titleName;
    }

    if (processId)
        *processId = pid;
    if (processPath)
        *processPath = path;
    if (appName)
        *appName = displayName;
    return true;
}

static bool IsWindowPendingClose(HWND hwnd);
static void MarkWindowPendingClose(HWND hwnd);
static void UnmarkWindowPendingClose(HWND hwnd);

// Devuelve true si la ventana quedó (o ya estaba) en el registro MRU.
static bool RegisterUserWindow(HWND hwnd, bool activity)
{
    if (IsWindowPendingClose(hwnd))
        return false;

    DWORD processId = 0;
    std::wstring processPath;
    std::wstring appName;
    if (!IsRealUserApplicationWindow(hwnd, &processId, &processPath, &appName))
        return false;

    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (item.hwnd == hwnd)
        {
            if (activity)
                item.lastActivated = now;
            return true;
        }
    }

    UserWindowInfo item = {};
    item.hwnd = hwnd;
    item.processId = processId;
    item.processPath = processPath;
    item.appName = appName;
    item.lastActivated = activity ? now : 0;
    g_userWindows.push_back(item);
    return true;
}

static bool IsWindowPendingClose(HWND hwnd)
{
    for (size_t i = 0; i < g_pendingCloseWindows.size(); ++i)
    {
        if (g_pendingCloseWindows[i] == hwnd)
            return true;
    }
    return false;
}

static void MarkWindowPendingClose(HWND hwnd)
{
    if (!hwnd || IsWindowPendingClose(hwnd))
        return;
    g_pendingCloseWindows.push_back(hwnd);
}

static void UnmarkWindowPendingClose(HWND hwnd)
{
    std::erase(g_pendingCloseWindows, hwnd);
}

static void PrunePendingCloseWindows()
{
    for (size_t i = 0; i < g_pendingCloseWindows.size();)
    {
        if (!g_pendingCloseWindows[i] || !IsWindow(g_pendingCloseWindows[i]))
        {
            g_pendingCloseWindows.erase(g_pendingCloseWindows.begin() + i);
            continue;
        }
        ++i;
    }
}

static void PruneUserWindowRegistry()
{
    PrunePendingCloseWindows();
    for (size_t i = 0; i < g_userWindows.size();)
    {
        HWND hwnd = g_userWindows[i].hwnd;
        if (!hwnd || !IsWindow(hwnd) || hwnd == g_selector ||
            IsWindowPendingClose(hwnd))
        {
            g_userWindows.erase(g_userWindows.begin() + i);
            continue;
        }
        ++i;
    }
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM)
{
    RegisterUserWindow(hwnd, false);
    return TRUE;
}

// MRU por evento: reemplaza el polling de GetForegroundWindow cada 250 ms.
static void CALLBACK ForegroundRetryProc(HWND, UINT, UINT_PTR id, DWORD)
{
    KillTimer(nullptr, id);
    g_foregroundRetryTimer = 0;
    HWND target = g_foregroundRetryWindow;
    g_foregroundRetryWindow = nullptr;
    if (target && IsWindow(target) && GetForegroundWindow() == target)
        RegisterUserWindow(target, true);
}

static void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                            LONG idObject, LONG, DWORD, DWORD)
{
    if (event != EVENT_SYSTEM_FOREGROUND || !hwnd || idObject != OBJID_WINDOW)
        return;
    if (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) != 0)
        return;

    PruneUserWindowRegistry();
    UnmarkWindowPendingClose(hwnd);
    if (RegisterUserWindow(hwnd, true))
        return;

    // Algunas ventanas llegan a primer plano antes de tener título/estilos
    // finales. Un único reintento diferido (no es polling permanente).
    g_foregroundRetryWindow = hwnd;
    if (!g_foregroundRetryTimer)
        g_foregroundRetryTimer = SetTimer(nullptr, 0, 250, ForegroundRetryProc);
}

static void RefreshWindowList()
{
    ReleaseGroupResources();
    PruneUserWindowRegistry();
    RegisterUserWindow(GetForegroundWindow(), true);
    EnumWindows(EnumWindowsProc, 0);

    g_groups.clear();
    HWND foreground = GetForegroundWindow();

    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (!IsRealUserApplicationWindow(item.hwnd, nullptr, nullptr, nullptr))
            continue;

        int groupIndex = FindAppGroup(item.processId);
        if (groupIndex < 0)
        {
            AppGroup group = {};
            group.processId = item.processId;
            group.processPath = item.processPath;
            group.appName = item.appName;
            group.representativeWindow = item.hwnd;
            group.icon = GetApplicationIcon(item.hwnd, item.processPath, &group.ownIcon);
            group.iconBitmap = nullptr;
            group.windows.push_back(item.hwnd);
            g_groups.push_back(group);
        }
        else
        {
            AppGroup& group = g_groups[groupIndex];
            bool duplicate = false;
            for (size_t j = 0; j < group.windows.size(); ++j)
            {
                if (group.windows[j] == item.hwnd)
                {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate)
                group.windows.push_back(item.hwnd);

            if (item.lastActivated > 0)
            {
                UserWindowInfo current = {};
                for (size_t j = 0; j < g_userWindows.size(); ++j)
                {
                    if (g_userWindows[j].hwnd == group.representativeWindow)
                    {
                        current = g_userWindows[j];
                        break;
                    }
                }
                if (item.lastActivated > current.lastActivated)
                {
                    if (group.iconBitmap)
                    {
                        group.iconBitmap->Release();
                        group.iconBitmap = nullptr;
                    }
                    if (group.ownIcon && group.icon)
                    {
                        DestroyIcon(group.icon);
                        group.icon = nullptr;
                    }
                    group.representativeWindow = item.hwnd;
                    group.icon = GetApplicationIcon(item.hwnd, item.processPath, &group.ownIcon);
                }
            }
        }
    }

    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].representativeWindow == foreground)
        {
            if (i != 0)
            {
                AppGroup first = g_groups[0];
                g_groups[0] = g_groups[i];
                g_groups[i] = first;
            }
            break;
        }
    }

    for (size_t i = 1; i < g_groups.size(); ++i)
    {
        size_t newest = i;
        ULONGLONG newestTime = 0;
        for (size_t j = 0; j < g_userWindows.size(); ++j)
        {
            if (g_userWindows[j].hwnd == g_groups[newest].representativeWindow)
            {
                newestTime = g_userWindows[j].lastActivated;
                break;
            }
        }

        for (size_t j = i + 1; j < g_groups.size(); ++j)
        {
            ULONGLONG candidateTime = 0;
            for (size_t k = 0; k < g_userWindows.size(); ++k)
            {
                if (g_userWindows[k].hwnd == g_groups[j].representativeWindow)
                {
                    candidateTime = g_userWindows[k].lastActivated;
                    break;
                }
            }
            if (candidateTime > newestTime)
            {
                newest = j;
                newestTime = candidateTime;
            }
        }

        if (newest != i)
        {
            AppGroup ordered = g_groups[i];
            g_groups[i] = g_groups[newest];
            g_groups[newest] = ordered;
        }
    }

    g_selected = 0;
    for (size_t i = 0; i < g_groups.size(); ++i)
        g_groups[i].mediaAccent = ExtractMediaAccentFromImage(g_groups[i].icon);
}

static std::wstring MakeSlotText(const AppGroup& group, bool selected)
{
    (void)selected;
    return group.appName;
}

static std::wstring MakeCounterText()
{
    if (g_groups.empty())
        return L"0 / 0";

    wchar_t buffer[64] = {};
    wsprintfW(buffer, L"%d / %d", g_selected + 1, static_cast<int>(g_groups.size()));
    return buffer;
}

// DPI POR MONITOR

// DPI efectivo del monitor (shcore.dll, resuelto dinámicamente). 1.0 = 100%.
static float GetMonitorDpiScale(HMONITOR monitor)
{
    if (!g_shcoreApi)
        g_shcoreApi = LoadLibraryW(L"shcore.dll");
    if (g_shcoreApi && !g_getDpiForMonitor)
        g_getDpiForMonitor = reinterpret_cast<GetDpiForMonitorFn>(
            GetProcAddress(g_shcoreApi, "GetDpiForMonitor"));

    UINT dpiX = 96, dpiY = 96;
    const int kMdtEffectiveDpi = 0;
    if (monitor && g_getDpiForMonitor &&
        SUCCEEDED(g_getDpiForMonitor(monitor, kMdtEffectiveDpi, &dpiX, &dpiY)) &&
        dpiX > 0)
        return static_cast<float>(dpiX) / 96.0f;
    return 1.0f;
}

static void UpdateUIScaleForMonitor(HMONITOR monitor)
{
    MONITORINFO info = {};
    info.cbSize = sizeof(info);
    if (monitor && GetMonitorInfoW(monitor, &info))
        UpdateUIScaleForWorkArea(info.rcWork, GetMonitorDpiScale(monitor));
}

// Marca el hilo actual como Per-Monitor-V2 (solo afecta a este hilo).
static void SetThreadPerMonitorAwareV2()
{
    typedef HANDLE (WINAPI* SetThreadDpiAwarenessContextFn)(HANDLE);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32)
        return;
    SetThreadDpiAwarenessContextFn pSetThreadDpiAwarenessContext =
        reinterpret_cast<SetThreadDpiAwarenessContextFn>(
            GetProcAddress(user32, "SetThreadDpiAwarenessContext"));
    if (!pSetThreadDpiAwarenessContext)
        return;
#ifdef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    HANDLE context = reinterpret_cast<HANDLE>(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#else
    HANDLE context = reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4));
#endif
    pSetThreadDpiAwarenessContext(context);
}

// DWM Y EFECTOS DE FONDO

static bool LoadDwmFunctions()
{
    if (g_dwmRegisterThumbnail && g_dwmUnregisterThumbnail &&
        g_dwmUpdateThumbnailProperties && g_dwmGetWindowAttribute)
        return true;

    g_dwmApi = LoadLibraryW(L"dwmapi.dll");
    if (!g_dwmApi)
        return false;

    g_dwmRegisterThumbnail = reinterpret_cast<DwmRegisterThumbnailFn>(
        GetProcAddress(g_dwmApi, "DwmRegisterThumbnail"));
    g_dwmUnregisterThumbnail = reinterpret_cast<DwmUnregisterThumbnailFn>(
        GetProcAddress(g_dwmApi, "DwmUnregisterThumbnail"));
    g_dwmUpdateThumbnailProperties = reinterpret_cast<DwmUpdateThumbnailPropertiesFn>(
        GetProcAddress(g_dwmApi, "DwmUpdateThumbnailProperties"));
    g_dwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttributeFn>(
        GetProcAddress(g_dwmApi, "DwmSetWindowAttribute"));
    g_dwmGetWindowAttribute = reinterpret_cast<DwmGetWindowAttributeFn>(
        GetProcAddress(g_dwmApi, "DwmGetWindowAttribute"));

    if (!g_dwmRegisterThumbnail || !g_dwmUnregisterThumbnail || !g_dwmUpdateThumbnailProperties)
    {
        FreeLibrary(g_dwmApi);
        g_dwmApi = nullptr;
        return false;
    }
    return true;
}

static void UnloadDwmFunctions()
{
    g_dwmRegisterThumbnail = nullptr;
    g_dwmUnregisterThumbnail = nullptr;
    g_dwmUpdateThumbnailProperties = nullptr;
    g_dwmSetWindowAttribute = nullptr;
    g_dwmGetWindowAttribute = nullptr;
    g_dwmGlassEnabled = false;
    if (g_dwmApi)
    {
        FreeLibrary(g_dwmApi);
        g_dwmApi = nullptr;
    }
}

static void RemoveNativeSelectorFrame(HWND hwnd)
{
    if (!hwnd)
        return;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    style &= ~(static_cast<LONG_PTR>(WS_CAPTION | WS_THICKFRAME | WS_BORDER |
                                     WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX |
                                     WS_MAXIMIZEBOX));
    style |= WS_POPUP;
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    exStyle &= ~(static_cast<LONG_PTR>(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME |
                                       WS_EX_CLIENTEDGE | WS_EX_STATICEDGE |
                                       WS_EX_APPWINDOW));
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static void ApplySelectorVisuals(HWND hwnd)
{
    if (!hwnd)
        return;

    g_dwmGlassEnabled = false;
    if (LoadDwmFunctions() && g_dwmSetWindowAttribute)
    {
        // Habilitar glass en toda el área cliente
        MARGINS margins = { -1, -1, -1, -1 };
        typedef HRESULT (WINAPI* DwmExtendFrameIntoClientAreaFn)(HWND, const MARGINS*);
        DwmExtendFrameIntoClientAreaFn pDwmExtendFrameIntoClientArea =
            reinterpret_cast<DwmExtendFrameIntoClientAreaFn>(
                GetProcAddress(g_dwmApi, "DwmExtendFrameIntoClientArea"));
        if (pDwmExtendFrameIntoClientArea)
        {
            HRESULT frameResult = pDwmExtendFrameIntoClientArea(hwnd, &margins);
            // Glass transparente dinámico: el contenido detrás sigue vivo.
            // El backdrop nativo permanece desactivado para conservar la
            // transparencia DWM estable de este selector.
            g_dwmGlassEnabled = SUCCEEDED(frameResult);
        }

        // Mantener el backdrop nativo desactivado: el glass transparente se
        // obtiene mediante el frame extendido y deja vivo el vídeo detrás.
        DWORD backdrop = kDwmSbtNone;
        g_dwmSetWindowAttribute(hwnd, kDwmWaSystemBackdropType,
                                &backdrop, sizeof(backdrop));
        // Mantener el borde nativo, pero igualarlo al tono general obsidiana
        // para que el margen de DWM no aparezca gris ni contrastado.
        DWORD borderColor = kDwmGlassBorderColor;
        g_dwmSetWindowAttribute(hwnd, kDwmBorderColor,
                                &borderColor, sizeof(borderColor));

        DWORD ncPolicy = kDwmNcRenderingPolicyDisabled;
        g_dwmSetWindowAttribute(hwnd, kDwmNcRenderingPolicy,
                                &ncPolicy, sizeof(ncPolicy));
        // El redondeo exterior lo controla el renderer; no pedir esquinas
        // nativas que puedan dejar un marco estático durante las animaciones.
        DWORD preference = 0;
        g_dwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference,
                                &preference, sizeof(preference));
    }
}

static HICON GetApplicationIcon(HWND hwnd, const std::wstring& processPath, bool* outOwnIcon)
{
    if (outOwnIcon)
        *outOwnIcon = false;

    if (!hwnd || !IsWindow(hwnd))
        return LoadIconW(nullptr, MAKEINTRESOURCEW(32512));

    ULONG_PTR result = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_BIG, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    HICON icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON));
    if (icon)
        return icon;

    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL2, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &result) && result)
        return reinterpret_cast<HICON>(result);

    icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM));
    if (icon)
        return icon;

    if (!processPath.empty())
    {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (!shell32)
            shell32 = LoadLibraryW(L"shell32.dll");
        if (shell32)
        {
            typedef UINT (WINAPI* ExtractIconExWFn)(LPCWSTR, int, HICON*, HICON*, UINT);
            ExtractIconExWFn pExtractIconExW = reinterpret_cast<ExtractIconExWFn>(
                GetProcAddress(shell32, "ExtractIconExW"));
            if (pExtractIconExW)
            {
                HICON hLarge = nullptr;
                if (pExtractIconExW(processPath.c_str(), 0, &hLarge, nullptr, 1) > 0 && hLarge)
                {
                    if (outOwnIcon)
                        *outOwnIcon = true;
                    return hLarge;
                }
            }
        }
    }

    return LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
}

// GESTIÓN DE DIRECT2D Y DIRECTWRITE

static void ReleaseDWriteFormats()
{
    if (g_dwriteCounterFormat) { g_dwriteCounterFormat->Release(); g_dwriteCounterFormat = nullptr; }
    if (g_dwriteCloseFormat) { g_dwriteCloseFormat->Release(); g_dwriteCloseFormat = nullptr; }
    if (g_dwriteNormalFormat) { g_dwriteNormalFormat->Release(); g_dwriteNormalFormat = nullptr; }
    if (g_dwriteSelectedFormat) { g_dwriteSelectedFormat->Release(); g_dwriteSelectedFormat = nullptr; }
    g_dwriteFormatsScale = 0.0f;
}

static bool LoadD2DAndDWrite()
{
    // Los TextFormat dependen de g_uiScale: si la escala cambió (otro monitor /
    // otro DPI) se reconstruyen en lugar de reutilizar formatos obsoletos.
    if (g_d2dFactory && g_d2dDCRenderTarget && g_dwriteFactory &&
        g_dwriteFormatsScale == g_uiScale.value)
        return true;

    if (!g_d2dApi)
        g_d2dApi = LoadLibraryW(L"d2d1.dll");
    if (!g_dwriteApi)
        g_dwriteApi = LoadLibraryW(L"dwrite.dll");

    if (g_d2dApi && !g_d2dFactory)
    {
        D2D1CreateFactoryFn pD2D1CreateFactory = reinterpret_cast<D2D1CreateFactoryFn>(
            GetProcAddress(g_d2dApi, "D2D1CreateFactory"));
        if (pD2D1CreateFactory)
        {
            D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_NONE };
            pD2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, kIidD2D1Factory, &options,
                               reinterpret_cast<void**>(&g_d2dFactory));
        }
    }

    if (g_d2dFactory && !g_d2dDCRenderTarget)
    {
        // El layout está expresado en píxeles físicos: el render target debe
        // trabajar a 96 DPI (1 DIP = 1 px) y no heredar el DPI del sistema.
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
        );
        if (SUCCEEDED(g_d2dFactory->CreateDCRenderTarget(&props, &g_d2dDCRenderTarget)))
        {
            g_d2dDCRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White), &g_d2dBrush);
        }
    }

    if (g_dwriteApi && !g_dwriteFactory)
    {
        DWriteCreateFactoryFn pDWriteCreateFactory = reinterpret_cast<DWriteCreateFactoryFn>(
            GetProcAddress(g_dwriteApi, "DWriteCreateFactory"));
        if (pDWriteCreateFactory)
        {
            pDWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, kIidDWriteFactory,
                                 reinterpret_cast<IUnknown**>(&g_dwriteFactory));
        }
    }

    if (g_dwriteFactory && (!g_dwriteSelectedFormat ||
                            g_dwriteFormatsScale != g_uiScale.value))
    {
        ReleaseDWriteFormats();

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(10.0f, 16.0f * g_uiScale.value), L"es-es", &g_dwriteSelectedFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(9.0f, 13.0f * g_uiScale.value), L"es-es", &g_dwriteNormalFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(9.0f, 13.0f * g_uiScale.value), L"es-es", &g_dwriteCounterFormat);

        g_dwriteFactory->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            std::max(11.0f, 18.0f * g_uiScale.value), L"es-es", &g_dwriteCloseFormat);

        if (g_dwriteSelectedFormat)
        {
            g_dwriteSelectedFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteSelectedFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteNormalFormat)
        {
            g_dwriteNormalFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteNormalFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteCounterFormat)
        {
            g_dwriteCounterFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteCounterFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_dwriteCounterFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (g_dwriteCloseFormat)
        {
            g_dwriteCloseFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            g_dwriteCloseFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            g_dwriteCloseFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        g_dwriteFormatsScale = g_uiScale.value;
    }

    return (g_d2dFactory != nullptr && g_d2dDCRenderTarget != nullptr);
}

static ID2D1Bitmap* CreateD2DBitmapFromHIcon(ID2D1RenderTarget* renderTarget, HICON hIcon)
{
    if (!renderTarget || !hIcon)
        return nullptr;

    if (!LoadGdiFunctions())
        return nullptr;

    ICONINFO iconInfo = {};
    if (!GetIconInfo(hIcon, &iconInfo))
        return nullptr;

    HDC screenDC = GetDC(nullptr);
    if (!screenDC)
    {
        if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    BITMAP bm = {};
    g_getObjectW(iconInfo.hbmColor ? iconInfo.hbmColor : iconInfo.hbmMask,
                 sizeof(BITMAP), &bm);

    int width  = bm.bmWidth;
    int height = iconInfo.hbmColor ? bm.bmHeight : (bm.bmHeight / 2);

    if (width <= 0 || height <= 0 || width > 1024 || height > 1024)
    {
        ReleaseDC(nullptr, screenDC);
        if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    std::vector<DWORD> pixels(static_cast<size_t>(width * height), 0);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    bool hasAlpha = false;

    if (iconInfo.hbmColor)
    {
        g_getDIBits(screenDC, iconInfo.hbmColor, 0,
                    static_cast<UINT>(height),
                    pixels.data(), &bmi, DIB_RGB_COLORS);

        for (int i = 0; i < width * height; ++i)
        {
            if ((pixels[i] & 0xFF000000) != 0)
            {
                hasAlpha = true;
                break;
            }
        }
    }

    if (hasAlpha)
    {
        for (int i = 0; i < width * height; ++i)
        {
            DWORD c = pixels[i];
            BYTE  a = static_cast<BYTE>((c >> 24) & 0xFF);
            if (a == 0)
            {
                pixels[i] = 0;
            }
            else
            {
                BYTE r = static_cast<BYTE>((c >> 16) & 0xFF);
                BYTE g = static_cast<BYTE>((c >>  8) & 0xFF);
                BYTE b = static_cast<BYTE>( c        & 0xFF);

                r = static_cast<BYTE>((static_cast<UINT>(r) * a + 127) / 255);
                g = static_cast<BYTE>((static_cast<UINT>(g) * a + 127) / 255);
                b = static_cast<BYTE>((static_cast<UINT>(b) * a + 127) / 255);

                pixels[i] = (static_cast<DWORD>(a) << 24) |
                            (static_cast<DWORD>(r) << 16) |
                            (static_cast<DWORD>(g) <<  8) |
                             static_cast<DWORD>(b);
            }
        }
    }
    else if (iconInfo.hbmMask)
    {
        std::vector<DWORD> maskPixels(static_cast<size_t>(width * height), 0);
        g_getDIBits(screenDC, iconInfo.hbmMask, 0,
                    static_cast<UINT>(height),
                    maskPixels.data(), &bmi, DIB_RGB_COLORS);

        for (int i = 0; i < width * height; ++i)
        {
            bool isTransparent = (maskPixels[i] & 0x00FFFFFF) != 0;
            if (isTransparent)
            {
                pixels[i] = 0;
            }
            else
            {
                DWORD c = pixels[i];
                pixels[i] = 0xFF000000 | (c & 0x00FFFFFF);
            }
        }
    }

    ReleaseDC(nullptr, screenDC);
    if (iconInfo.hbmColor) g_deleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask)  g_deleteObject(iconInfo.hbmMask);

    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f
    );

    ID2D1Bitmap* pBitmap = nullptr;
    HRESULT hr = renderTarget->CreateBitmap(
        D2D1::SizeU(static_cast<UINT32>(width), static_cast<UINT32>(height)),
        pixels.data(),
        static_cast<UINT32>(width * sizeof(DWORD)),
        &props,
        &pBitmap
    );

    return SUCCEEDED(hr) ? pBitmap : nullptr;
}

static void ReleaseGroupResources()
{
    for (size_t i = 0; i < g_groups.size(); ++i)
    {
        if (g_groups[i].iconBitmap)
        {
            g_groups[i].iconBitmap->Release();
            g_groups[i].iconBitmap = nullptr;
        }
        if (g_groups[i].surfaceBrushNormal)
        {
            g_groups[i].surfaceBrushNormal->Release();
            g_groups[i].surfaceBrushNormal = nullptr;
        }
        if (g_groups[i].surfaceBrushSelected)
        {
            g_groups[i].surfaceBrushSelected->Release();
            g_groups[i].surfaceBrushSelected = nullptr;
        }
        if (g_groups[i].ownIcon && g_groups[i].icon)
        {
            DestroyIcon(g_groups[i].icon);
            g_groups[i].icon = nullptr;
        }
    }
}

static void UnloadD2DAndDWrite()
{
    ReleaseGroupResources();

    ReleaseDWriteFormats();
    if (g_dwriteFactory) { g_dwriteFactory->Release(); g_dwriteFactory = nullptr; }

    if (g_closeAccentBrush) { g_closeAccentBrush->Release(); g_closeAccentBrush = nullptr; }
    if (g_d2dBrush) { g_d2dBrush->Release(); g_d2dBrush = nullptr; }
    if (g_d2dDCRenderTarget) { g_d2dDCRenderTarget->Release(); g_d2dDCRenderTarget = nullptr; }
    if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = nullptr; }

    if (g_dwriteApi) { FreeLibrary(g_dwriteApi); g_dwriteApi = nullptr; }
    if (g_d2dApi) { FreeLibrary(g_d2dApi); g_d2dApi = nullptr; }
}

static bool LoadGdiFunctions()
{
    if (g_createFontIndirectW && g_deleteObject && g_createRoundRectRgn && g_getStockObject &&
        g_setBkMode && g_setTextColor && g_createSolidBrush &&
        g_createPen && g_selectObject && g_roundRect && g_getTextFaceW &&
        g_getDIBits && g_getObjectW && g_createCompatibleDC &&
        g_createDIBSection && g_stretchBlt && g_bitBlt && g_deleteDC)
        return true;

    if (!g_gdiApi)
        g_gdiApi = LoadLibraryW(L"gdi32.dll");
    if (!g_gdiApi)
        return false;

    g_createFontIndirectW = reinterpret_cast<CreateFontIndirectWFn>(
        GetProcAddress(g_gdiApi, "CreateFontIndirectW"));
    g_deleteObject = reinterpret_cast<DeleteObjectFn>(
        GetProcAddress(g_gdiApi, "DeleteObject"));
    g_createRoundRectRgn = reinterpret_cast<CreateRoundRectRgnFn>(
        GetProcAddress(g_gdiApi, "CreateRoundRectRgn"));
    g_getStockObject = reinterpret_cast<GetStockObjectFn>(
        GetProcAddress(g_gdiApi, "GetStockObject"));
    g_setBkMode = reinterpret_cast<SetBkModeFn>(
        GetProcAddress(g_gdiApi, "SetBkMode"));
    g_setTextColor = reinterpret_cast<SetTextColorFn>(
        GetProcAddress(g_gdiApi, "SetTextColor"));
    g_createSolidBrush = reinterpret_cast<CreateSolidBrushFn>(
        GetProcAddress(g_gdiApi, "CreateSolidBrush"));
    g_createPen = reinterpret_cast<CreatePenFn>(
        GetProcAddress(g_gdiApi, "CreatePen"));
    g_selectObject = reinterpret_cast<SelectObjectFn>(
        GetProcAddress(g_gdiApi, "SelectObject"));
    g_roundRect = reinterpret_cast<RoundRectFn>(
        GetProcAddress(g_gdiApi, "RoundRect"));
    g_getTextFaceW = reinterpret_cast<GetTextFaceWFn>(
        GetProcAddress(g_gdiApi, "GetTextFaceW"));
    g_getDIBits = reinterpret_cast<GetDIBitsFn>(
        GetProcAddress(g_gdiApi, "GetDIBits"));
    g_getObjectW = reinterpret_cast<GetObjectWFn>(
        GetProcAddress(g_gdiApi, "GetObjectW"));
    g_createCompatibleDC = reinterpret_cast<CreateCompatibleDCFn>(
        GetProcAddress(g_gdiApi, "CreateCompatibleDC"));
    g_createDIBSection = reinterpret_cast<CreateDIBSectionFn>(
        GetProcAddress(g_gdiApi, "CreateDIBSection"));
    g_stretchBlt = reinterpret_cast<StretchBltFn>(
        GetProcAddress(g_gdiApi, "StretchBlt"));
    g_setStretchBltMode = reinterpret_cast<SetStretchBltModeFn>(
        GetProcAddress(g_gdiApi, "SetStretchBltMode"));
    g_setBrushOrgEx = reinterpret_cast<SetBrushOrgExFn>(
        GetProcAddress(g_gdiApi, "SetBrushOrgEx"));
    g_bitBlt = reinterpret_cast<BitBltFn>(
        GetProcAddress(g_gdiApi, "BitBlt"));
    g_deleteDC = reinterpret_cast<DeleteDCFn>(
        GetProcAddress(g_gdiApi, "DeleteDC"));

    if (!g_createFontIndirectW || !g_deleteObject || !g_createRoundRectRgn || !g_getStockObject ||
        !g_setBkMode || !g_setTextColor || !g_createSolidBrush ||
        !g_createPen || !g_selectObject || !g_roundRect ||
        !g_getDIBits || !g_getObjectW || !g_createCompatibleDC ||
        !g_createDIBSection || !g_stretchBlt || !g_bitBlt || !g_deleteDC)
    {
        FreeLibrary(g_gdiApi);
        g_gdiApi = nullptr;
        return false;
    }
    return true;
}

static MediaAccent ExtractMediaAccentFromImage(HICON icon)
{
    MediaAccent result;
    if (!icon || !LoadGdiFunctions())
        return result;

    ICONINFO iconInfo = {};
    if (!GetIconInfo(icon, &iconInfo) || !iconInfo.hbmColor)
    {
        if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
        return result;
    }

    BITMAP bitmap = {};
    if (!g_getObjectW(iconInfo.hbmColor, sizeof(bitmap), &bitmap) ||
        bitmap.bmWidth <= 0 || bitmap.bmHeight <= 0)
    {
        g_deleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
        return result;
    }

    const int width = std::min(static_cast<int>(bitmap.bmWidth), 128);
    const int height = std::min(static_cast<int>(bitmap.bmHeight), 128);
    std::vector<DWORD> pixels(static_cast<size_t>(width) * height, 0);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screenDC = GetDC(nullptr);
    if (screenDC && g_getDIBits(screenDC, iconInfo.hbmColor, 0,
                                static_cast<UINT>(height), pixels.data(),
                                &bmi, DIB_RGB_COLORS) != 0)
    {
        double weightedR = 0.0, weightedG = 0.0, weightedB = 0.0;
        double totalWeight = 0.0;
        for (DWORD pixel : pixels)
        {
            int r = static_cast<int>((pixel >> 16) & 0xFF);
            int g = static_cast<int>((pixel >> 8) & 0xFF);
            int b = static_cast<int>(pixel & 0xFF);
            int maximum = std::max(r, std::max(g, b));
            int minimum = std::min(r, std::min(g, b));
            if (maximum < 28)
                continue;

            // Favorecer tonos con información cromática sin dejar que dominen.
            double saturation = static_cast<double>(maximum - minimum) / 255.0;
            double brightness = static_cast<double>(maximum) / 255.0;
            double weight = 0.25 + saturation * 0.75;
            weight *= 0.35 + brightness * 0.65;
            weightedR += r * weight;
            weightedG += g * weight;
            weightedB += b * weight;
            totalWeight += weight;
        }

        if (totalWeight > 0.0)
        {
            int r = static_cast<int>(weightedR / totalWeight);
            int g = static_cast<int>(weightedG / totalWeight);
            int b = static_cast<int>(weightedB / totalWeight);
            int maximum = std::max(r, std::max(g, b));
            int minimum = std::min(r, std::min(g, b));
            if (maximum > 0 && maximum - minimum >= 8)
            {
                // La card sigue siendo Obsidian: el color solo ilumina su borde.
                const float scale = 0.78f;
                result.color = RGB(static_cast<BYTE>(r * scale),
                                   static_cast<BYTE>(g * scale),
                                   static_cast<BYTE>(b * scale));
                result.strength = 1.0f;
                result.valid = true;
            }
            else
            {
                // Imagen casi monocroma/negra: producir un tono frío mínimo,
                // determinista y oscuro para que el contorno no desaparezca.
                BYTE level = static_cast<BYTE>(std::max(12, std::min(24, maximum)));
                result.color = RGB(level, static_cast<BYTE>(level + 2),
                                   static_cast<BYTE>(level + 5));
                result.strength = 0.75f;
                result.valid = true;
            }
        }
        else
        {
            // No se encontraron píxeles suficientemente luminosos: fallback
            // cromático fijo, sobrio y no aleatorio para artwork casi negro.
            result.color = RGB(12, 14, 18);
            result.strength = 0.65f;
            result.valid = true;
        }
    }

    if (screenDC)
        ReleaseDC(nullptr, screenDC);
    g_deleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask) g_deleteObject(iconInfo.hbmMask);
    return result;
}

static D2D1_COLOR_F AddMediaAccent(const D2D1_COLOR_F& base,
                                   const MediaAccent& accent, float influence)
{
    if (!accent.valid || influence <= 0.0f)
        return base;

    const float ar = static_cast<float>(GetRValue(accent.color)) / 255.0f;
    const float ag = static_cast<float>(GetGValue(accent.color)) / 255.0f;
    const float ab = static_cast<float>(GetBValue(accent.color)) / 255.0f;
    return D2D1::ColorF(
        std::min(1.0f, base.r + ar * influence),
        std::min(1.0f, base.g + ag * influence),
        std::min(1.0f, base.b + ab * influence), base.a);
}

static HFONT CreateModernFont(const wchar_t* preferredFace, const wchar_t* fallbackFace, LONG height, LONG weight)
{
    if (!LoadGdiFunctions())
        return nullptr;

    LOGFONTW font = {};
    font.lfHeight = height;
    font.lfWeight = weight;
    font.lfCharSet = DEFAULT_CHARSET;
    font.lfOutPrecision = OUT_TT_PRECIS;
    font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    font.lfQuality = 6;
    font.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

    if (preferredFace && preferredFace[0] != L'\0')
    {
        lstrcpynW(font.lfFaceName, preferredFace, LF_FACESIZE);
        HFONT hFont = g_createFontIndirectW(&font);
        if (hFont)
        {
            if (g_getTextFaceW)
            {
                HDC screenDC = GetDC(nullptr);
                if (screenDC)
                {
                    HGDIOBJ old = g_selectObject(screenDC, hFont);
                    wchar_t actualFace[LF_FACESIZE] = {};
                    g_getTextFaceW(screenDC, LF_FACESIZE, actualFace);
                    g_selectObject(screenDC, old);
                    ReleaseDC(nullptr, screenDC);

                    if (_wcsicmp(actualFace, preferredFace) == 0)
                        return hFont;
                }
            }
            else
            {
                return hFont;
            }
            g_deleteObject(hFont);
        }
    }

    lstrcpynW(font.lfFaceName, (fallbackFace && fallbackFace[0] != L'\0') ? fallbackFace : L"Segoe UI", LF_FACESIZE);
    return g_createFontIndirectW(&font);
}

static void ReleaseUiFontObjects()
{
    if (g_deleteObject)
    {
        if (g_nameFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_nameFont));
        if (g_selectedNameFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_selectedNameFont));
        if (g_secondaryFont)
            g_deleteObject(reinterpret_cast<HGDIOBJ>(g_secondaryFont));
    }
    g_nameFont = nullptr;
    g_selectedNameFont = nullptr;
    g_secondaryFont = nullptr;
    g_gdiFontsScale = 0.0f;
}

static void CreateUiFonts()
{
    if (g_nameFont || g_selectedNameFont || g_secondaryFont)
    {
        // Las fuentes GDI del fallback también dependen de la escala.
        if (g_gdiFontsScale == g_uiScale.value)
            return;
        ReleaseUiFontObjects();
    }

    g_selectedNameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                          -std::max(10, ScaleLayoutPx(16.0f)), FW_SEMIBOLD);
    g_nameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                  -std::max(9, ScaleLayoutPx(13.0f)), 500);
    g_secondaryFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                       -std::max(9, ScaleLayoutPx(13.0f)), 500);
    g_gdiFontsScale = g_uiScale.value;
}

static void DestroyUiFonts()
{
    ReleaseUiFontObjects();
    g_getTextFaceW = nullptr;
    g_getDIBits = nullptr;
    g_getObjectW = nullptr;
    if (g_gdiApi)
    {
        FreeLibrary(g_gdiApi);
        g_gdiApi = nullptr;
    }
}

static RECT ContainedRect(const RECT& area, int sourceWidth, int sourceHeight)
{
    RECT result = area;
    if (sourceWidth <= 0 || sourceHeight <= 0)
        return result;

    int areaWidth = area.right - area.left;
    int areaHeight = area.bottom - area.top;
    if (areaWidth <= 0 || areaHeight <= 0)
        return result;

    long long widthByHeight = static_cast<long long>(areaHeight) * sourceWidth / sourceHeight;
    long long heightByWidth = static_cast<long long>(areaWidth) * sourceHeight / sourceWidth;
    int width = widthByHeight <= areaWidth ? static_cast<int>(widthByHeight) : areaWidth;
    int height = widthByHeight <= areaWidth ? areaHeight : static_cast<int>(heightByWidth);
    result.left = area.left + (areaWidth - width) / 2;
    result.top = area.top + (areaHeight - height) / 2;
    result.right = result.left + width;
    result.bottom = result.top + height;
    return result;
}

// SISTEMA DE BLUR PROPIO

static void ApplyBlurToPixels(unsigned char* pixels, int width, int height, int radius)
{
    if (!pixels || width <= 0 || height <= 0 || radius <= 0)
        return;

    // Kernel Gaussian binomial de cinco taps: [1, 4, 6, 4, 1] / 16.
    // Es separable y se repite para obtener una distribución más suave sin
    // reservar un kernel grande ni introducir una ruta D3D adicional.
    const int weights[5] = { 1, 4, 6, 4, 1 };
    const int kernelRadius = std::min(radius, 2);
    const int kernelWidth = kernelRadius * 2 + 1;
    const int weightOffset = 2 - kernelRadius;
    const int weightDivisor = kernelRadius == 2 ? 16 : 6;
    const size_t pixelBytes = static_cast<size_t>(width) * height * 4;
    std::vector<unsigned char> source(pixelBytes);
    std::vector<unsigned char> horizontal(pixelBytes);
    std::vector<unsigned char> destination(pixelBytes);
    std::copy(pixels, pixels + pixelBytes, source.begin());

    for (int pass = 0; pass < kBlurPassCount; ++pass)
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int sumB = 0, sumG = 0, sumR = 0, sumA = 0;
                for (int k = 0; k < kernelWidth; ++k)
                {
                    int sx = std::max(0, std::min(width - 1,
                        x + k - kernelRadius));
                    const unsigned char* p = source.data() +
                        (static_cast<size_t>(y) * width + sx) * 4;
                    int weight = weights[k + weightOffset];
                    sumB += p[0] * weight;
                    sumG += p[1] * weight;
                    sumR += p[2] * weight;
                    sumA += p[3] * weight;
                }

                unsigned char* d = horizontal.data() +
                    (static_cast<size_t>(y) * width + x) * 4;
                d[0] = static_cast<unsigned char>((sumB + weightDivisor / 2) /
                                                   weightDivisor);
                d[1] = static_cast<unsigned char>((sumG + weightDivisor / 2) /
                                                   weightDivisor);
                d[2] = static_cast<unsigned char>((sumR + weightDivisor / 2) /
                                                   weightDivisor);
                d[3] = static_cast<unsigned char>((sumA + weightDivisor / 2) /
                                                   weightDivisor);
            }
        }

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int sumB = 0, sumG = 0, sumR = 0, sumA = 0;
                for (int k = 0; k < kernelWidth; ++k)
                {
                    int sy = std::max(0, std::min(height - 1,
                        y + k - kernelRadius));
                    const unsigned char* p = horizontal.data() +
                        (static_cast<size_t>(sy) * width + x) * 4;
                    int weight = weights[k + weightOffset];
                    sumB += p[0] * weight;
                    sumG += p[1] * weight;
                    sumR += p[2] * weight;
                    sumA += p[3] * weight;
                }

                unsigned char* d = destination.data() +
                    (static_cast<size_t>(y) * width + x) * 4;
                d[0] = static_cast<unsigned char>((sumB + weightDivisor / 2) /
                                                   weightDivisor);
                d[1] = static_cast<unsigned char>((sumG + weightDivisor / 2) /
                                                   weightDivisor);
                d[2] = static_cast<unsigned char>((sumR + weightDivisor / 2) /
                                                   weightDivisor);
                d[3] = static_cast<unsigned char>((sumA + weightDivisor / 2) /
                                                   weightDivisor);
            }
        }

        source.swap(destination);
    }

    std::copy(source.begin(), source.end(), pixels);
}

static void CreateBlurBackground(int captureX, int captureY)
{
    if (g_blurBackgroundCreated)
        return;

    if (!LoadGdiFunctions())
        return;

    const int captureWidth = g_runtimeSelectorWidth;
    const int captureHeight = g_runtimeSelectorHeight;
    g_blurCaptureX = captureX;
    g_blurCaptureY = captureY;

    // Procesar a ~50% reduce el área de trabajo a una cuarta parte. El tamaño
    // se deriva del área/escala del monitor ya calculada por el selector; no
    // se usan dimensiones fijas dependientes de una resolución concreta.
    g_blurWidth = std::max(kBlurMinimumWidth,
        (captureWidth * kBlurDownscaleNumerator) / kBlurDownscaleDenominator);
    g_blurHeight = std::max(kBlurMinimumHeight,
        (captureHeight * kBlurDownscaleNumerator) / kBlurDownscaleDenominator);

    HDC screenDC = GetDC(nullptr);
    if (!screenDC)
        return;

    g_blurDC = g_createCompatibleDC(screenDC);
    if (!g_blurDC)
    {
        ReleaseDC(nullptr, screenDC);
        return;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = g_blurWidth;
    bmi.bmiHeader.biHeight = -g_blurHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    g_blurBitmap = g_createDIBSection(screenDC, &bmi, DIB_RGB_COLORS,
                                      reinterpret_cast<void**>(&g_blurPixels), nullptr, 0);
    if (!g_blurBitmap || !g_blurPixels)
    {
        if (g_blurBitmap) g_deleteObject(g_blurBitmap);
        g_blurBitmap = nullptr;
        g_deleteDC(g_blurDC);
        g_blurDC = nullptr;
        ReleaseDC(nullptr, screenDC);
        return;
    }

    g_blurOldBitmap = reinterpret_cast<HBITMAP>(g_selectObject(g_blurDC, g_blurBitmap));

    // HALFTONE evita el patrón de interpolación por defecto de StretchBlt,
    // que puede generar líneas de color al reducir texto y vídeo. Las dos
    // funciones son opcionales para conservar compatibilidad con GDI.
    if (g_setStretchBltMode)
        g_setStretchBltMode(g_blurDC, HALFTONE);
    if (g_setBrushOrgEx)
        g_setBrushOrgEx(g_blurDC, 0, 0, nullptr);

    // Captura y downscale en una sola operación, antes de hacer visible la ventana.
    BOOL copied = g_stretchBlt(g_blurDC, 0, 0, g_blurWidth, g_blurHeight,
                               screenDC, captureX, captureY,
                               captureWidth, captureHeight,
                               SRCCOPY | CAPTUREBLT);
    if (!copied)
    {
        ReleaseBlurBackground();
        ReleaseDC(nullptr, screenDC);
        return;
    }

    // El DIB creado directamente expone ya los píxeles BGRA contiguos.
    // Se atenúa la crominancia subpíxel antes del blur: se conservan los
    // colores reales, pero los bordes RGB extremos no se propagan.
    for (int i = 0; i < g_blurWidth * g_blurHeight * 4; i += 4)
    {
        float blue = static_cast<float>(g_blurPixels[i + 0]);
        float green = static_cast<float>(g_blurPixels[i + 1]);
        float red = static_cast<float>(g_blurPixels[i + 2]);
        float luminance = red * 0.2126f + green * 0.7152f + blue * 0.0722f;
        blue += (luminance - blue) * kBlurPreChromaSuppression;
        green += (luminance - green) * kBlurPreChromaSuppression;
        red += (luminance - red) * kBlurPreChromaSuppression;
        g_blurPixels[i + 0] = static_cast<unsigned char>(blue);
        g_blurPixels[i + 1] = static_cast<unsigned char>(green);
        g_blurPixels[i + 2] = static_cast<unsigned char>(red);
        g_blurPixels[i + 3] = 255;
    }

    ApplyBlurToPixels(g_blurPixels, g_blurWidth, g_blurHeight,
                      kBlurPassRadius);

    // Tinte obsidiana moderado; el alpha permanece opaco porque la intensidad
    // final de la capa se controla de forma independiente al dibujarla.
    for (int i = 0; i < g_blurWidth * g_blurHeight * 4; i += 4)
    {
        // Atenuar ligeramente la crominancia evita halos RGB sin eliminar
        // el color real del escritorio o del vídeo.
        float blue = static_cast<float>(g_blurPixels[i + 0]);
        float green = static_cast<float>(g_blurPixels[i + 1]);
        float red = static_cast<float>(g_blurPixels[i + 2]);
        float luminance = red * 0.2126f + green * 0.7152f + blue * 0.0722f;
        blue += (luminance - blue) * kBlurChromaSuppression;
        green += (luminance - green) * kBlurChromaSuppression;
        red += (luminance - red) * kBlurChromaSuppression;
        g_blurPixels[i + 0] = static_cast<unsigned char>(blue * 0.90f);
        g_blurPixels[i + 1] = static_cast<unsigned char>(green * 0.92f);
        g_blurPixels[i + 2] = static_cast<unsigned char>(red * 0.94f);
        g_blurPixels[i + 3] = 255;
    }

    ReleaseDC(nullptr, screenDC);
    g_blurBackgroundCreated = true;
}

static void ReleaseBlurBackground()
{
    if (g_blurD2DBitmap)
    {
        g_blurD2DBitmap->Release();
        g_blurD2DBitmap = nullptr;
    }

    if (g_blurDC)
    {
        if (g_blurOldBitmap)
            g_selectObject(g_blurDC, g_blurOldBitmap);
        if (g_blurBitmap)
            g_deleteObject(g_blurBitmap);
        g_deleteDC(g_blurDC);
    }

    g_blurDC = nullptr;
    g_blurBitmap = nullptr;
    g_blurOldBitmap = nullptr;
    g_blurPixels = nullptr;
    g_blurWidth = 0;
    g_blurHeight = 0;
    g_blurCaptureX = 0;
    g_blurCaptureY = 0;
    g_blurBackgroundCreated = false;
}

// GEOMETRÍA Y TRAYECTORIA DEL CARRUSEL

struct CardSlotGeometry
{
    float left;
    float top;
    float right;
    float bottom;
    float cornerRadius;
    float headerHeight;
    float padding;
};

static CardSlotGeometry GetCanonicalSlotKeyframe(int slot)
{
    CardSlotGeometry g = {};
    if (slot <= -1)
    {
        g.left = -115.0f;
        g.top = 160.0f;
        g.right = -5.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 10.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else if (slot == 0)
    {
        g.left = 15.0f;
        g.top = 160.0f;
        g.right = 125.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else if (slot == 1)
    {
        g.left = 135.0f;
        g.top = 125.0f;
        g.right = 375.0f;
        g.bottom = 315.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 28.0f;
        g.padding = 8.0f;
    }
    else if (slot == 2)
    {
        g.left = 390.0f;
        g.top = 55.0f;
        g.right = 830.0f;
        g.bottom = 355.0f;
        g.cornerRadius = 14.0f;
        g.headerHeight = 34.0f;
        g.padding = 10.0f;
    }
    else if (slot == 3)
    {
        g.left = 845.0f;
        g.top = 125.0f;
        g.right = 1085.0f;
        g.bottom = 315.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 28.0f;
        g.padding = 8.0f;
    }
    else if (slot == 4)
    {
        g.left = 1095.0f;
        g.top = 160.0f;
        g.right = 1205.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 12.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    else
    {
        g.left = 1225.0f;
        g.top = 160.0f;
        g.right = 1335.0f;
        g.bottom = 280.0f;
        g.cornerRadius = 10.0f;
        g.headerHeight = 24.0f;
        g.padding = 6.0f;
    }
    return g;
}

static CardSlotGeometry GetSlotKeyframe(int slot)
{
    CardSlotGeometry g = {};
    if (g_visibleCardCount == 5)
    {
        // Keep the original five-card trajectory, including its off-screen
        // keyframes used while a navigation animation is in progress.
        g = GetCanonicalSlotKeyframe(slot);
    }
    else
    {
        // The three-card layout reuses the center and adjacent keyframes.
        const int relative = slot - GetCarouselCenterSlot();
        const int canonical = std::max(-1, std::min(1, relative)) + 2;
        g = GetCanonicalSlotKeyframe(canonical);
    }
    return g;
}

static inline float LerpFloat(float a, float b, float t)
{
    return a + (b - a) * t;
}

static float EaseOutCubic(float t)
{
    t = std::max(0.0f, std::min(1.0f, t));
    float inverse = 1.0f - t;
    return 1.0f - inverse * inverse * inverse;
}

static RECT ScaleRectAroundCenter(const RECT& rect, float scale)
{
    if (fabsf(scale - 1.0f) < 0.0001f)
        return rect;
    float cx = (static_cast<float>(rect.left) + static_cast<float>(rect.right)) * 0.5f;
    float cy = (static_cast<float>(rect.top) + static_cast<float>(rect.bottom)) * 0.5f;
    RECT result = {};
    result.left = static_cast<LONG>(roundf(cx + (rect.left - cx) * scale));
    result.right = static_cast<LONG>(roundf(cx + (rect.right - cx) * scale));
    result.top = static_cast<LONG>(roundf(cy + (rect.top - cy) * scale));
    result.bottom = static_cast<LONG>(roundf(cy + (rect.bottom - cy) * scale));
    return result;
}

static RECT ScaleRectAroundPoint(const RECT& rect, float scale,
                                 float pivotX, float pivotY)
{
    if (fabsf(scale - 1.0f) < 0.0001f)
        return rect;
    RECT result = {};
    result.left = static_cast<LONG>(roundf(pivotX + (rect.left - pivotX) * scale));
    result.right = static_cast<LONG>(roundf(pivotX + (rect.right - pivotX) * scale));
    result.top = static_cast<LONG>(roundf(pivotY + (rect.top - pivotY) * scale));
    result.bottom = static_cast<LONG>(roundf(pivotY + (rect.bottom - pivotY) * scale));
    return result;
}

static CardSlotGeometry GetInterpolatedSlotGeometry(float virtualSlot)
{
    int k0 = static_cast<int>(floorf(virtualSlot));
    int k1 = k0 + 1;
    float f = virtualSlot - static_cast<float>(k0);

    CardSlotGeometry g0 = GetSlotKeyframe(k0);
    CardSlotGeometry g1 = GetSlotKeyframe(k1);

    CardSlotGeometry res = {};
    res.left = LerpFloat(g0.left, g1.left, f);
    res.top = LerpFloat(g0.top, g1.top, f);
    res.right = LerpFloat(g0.right, g1.right, f);
    res.bottom = LerpFloat(g0.bottom, g1.bottom, f);
    res.cornerRadius = LerpFloat(g0.cornerRadius, g1.cornerRadius, f);
    res.headerHeight = LerpFloat(g0.headerHeight, g1.headerHeight, f);
    res.padding = LerpFloat(g0.padding, g1.padding, f);
    res.left *= g_uiScale.value;
    res.top *= g_uiScale.value;
    res.right *= g_uiScale.value;
    res.bottom *= g_uiScale.value;
    res.cornerRadius *= g_uiScale.value;
    res.headerHeight *= g_uiScale.value;
    res.padding *= g_uiScale.value;

    // Inclinación sutil del carrusel durante la navegación. Se aplica como
    // una deformación geométrica vertical alrededor del eje central, no como
    // un bitmap rotado; así los DWM thumbnails reciben exactamente la misma
    // trayectoria y no se desacoplan visualmente de las cards.
    if (fabsf(g_sceneTiltDegrees) > 0.001f)
    {
        const float selectorCenterX = static_cast<float>(g_runtimeSelectorWidth) * 0.5f;
        const float slotCenterX = (res.left + res.right) * 0.5f;
        const float radians = g_sceneTiltDegrees * 3.14159265358979323846f / 180.0f;
        const float verticalOffset = tanf(radians) * (slotCenterX - selectorCenterX);
        res.top += verticalOffset;
        res.bottom += verticalOffset;
    }
    return res;
}

static RECT GetSlotCardRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    RECT rc = {};
    rc.left = static_cast<int>(roundf(g.left));
    rc.top = static_cast<int>(roundf(g.top));
    rc.right = static_cast<int>(roundf(g.right));
    rc.bottom = static_cast<int>(roundf(g.bottom));
    if (slot == GetCarouselCenterSlot())
        rc = ScaleRectAroundCenter(rc, g_cardSnapScale);
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        rc.top += offset;
        rc.bottom += offset;
    }
    return rc;
}

static RECT GetSlotHeaderRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    int pad = static_cast<int>(roundf(g.padding));
    RECT header = {};
    header.left = static_cast<int>(roundf(g.left)) + pad;
    header.right = static_cast<int>(roundf(g.right)) - pad;
    header.top = static_cast<int>(roundf(g.top)) + pad;
    header.bottom = header.top + static_cast<int>(roundf(g.headerHeight));
    // El título se mantiene en su geometría normal. El bounce se separa
    // lógicamente y se aplica únicamente al icono en PaintSlotHeader*.
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        header.top += offset;
        header.bottom += offset;
    }
    return header;
}

static RECT GetSlotPreviewRect(int slot)
{
    CardSlotGeometry g = GetInterpolatedSlotGeometry(static_cast<float>(slot) + g_animOffset);
    int pad = static_cast<int>(roundf(g.padding));
    int hHeight = static_cast<int>(roundf(g.headerHeight));
    float dist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
    int gap = ScaleLayoutPx(static_cast<float>((dist < 0.5f) ? 8 : (dist < 1.5f ? 6 : 4)));

    RECT preview = {};
    preview.left = static_cast<int>(roundf(g.left)) + pad;
    preview.right = static_cast<int>(roundf(g.right)) - pad;
    preview.top = static_cast<int>(roundf(g.top)) + pad + hHeight + gap;
    preview.bottom = static_cast<int>(roundf(g.bottom)) - pad;
    if (slot == GetCarouselCenterSlot())
        preview = ScaleRectAroundCenter(preview, g_cardSnapScale);
    if (g_cardExitActive && slot == g_cardExitSlot)
    {
        int offset = static_cast<int>(roundf(GetCardExitOffsetY()));
        preview.top += offset;
        preview.bottom += offset;
    }
    return preview;
}

static RECT GetCounterRect()
{
    RECT rc = {};
    int width = ScaleLayoutPx(static_cast<float>(kCounterWidth));
    int height = ScaleLayoutPx(static_cast<float>(kCounterHeight));
    rc.left = (g_runtimeSelectorWidth - width) / 2;
    rc.top = ScaleLayoutPx(static_cast<float>(kCounterTop));
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;
    return rc;
}

static RECT GetCloseButtonRect(int slot)
{
    RECT card = GetSlotCardRect(slot);
    const int hitSize = ScaleLayoutPx(28.0f);
    RECT rc = {};
    rc.right = card.right - static_cast<int>(roundf(DpiPx(5.0f)));
    rc.left = rc.right - hitSize;
    rc.top = card.top + ScaleLayoutPx(3.0f);
    rc.bottom = rc.top + hitSize;
    return rc;
}

static RECT GetCloseHoverButtonRect(int slot)
{
    RECT rc = GetCloseButtonRect(slot);
    int buttonOffsetY = ScaleLayoutPx(3.0f);
    rc.top += buttonOffsetY;
    rc.bottom += buttonOffsetY;
    return rc;
}

static ID2D1PathGeometry* CreateCloseButtonGeometry(const RECT& rect)
{
    if (!g_d2dFactory)
        return nullptr;

    ID2D1PathGeometry* geometry = nullptr;
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geometry)) || !geometry ||
        FAILED(geometry->Open(&sink)) || !sink)
    {
        if (geometry) geometry->Release();
        return nullptr;
    }

    float left = static_cast<float>(rect.left);
    float top = static_cast<float>(rect.top);
    float right = static_cast<float>(rect.right);
    float bottom = static_cast<float>(rect.bottom);
    float r = static_cast<float>(ScaleLayoutPx(6.0f));
    float topRightX = static_cast<float>(ScaleLayoutPx(12.0f));
    float topRightY = static_cast<float>(ScaleLayoutPx(6.0f));

    sink->BeginFigure(D2D1::Point2F(left + r, top), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1::Point2F(right - topRightX, top));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(right, top + topRightY),
        D2D1::SizeF(topRightX, topRightY), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(right, bottom - r));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(right - r, bottom), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(left + r, bottom));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(left, bottom - r), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->AddLine(D2D1::Point2F(left, top + r));
    sink->AddArc(D2D1::ArcSegment(
        D2D1::Point2F(left + r, top), D2D1::SizeF(r, r), 0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geometry;
}

static int ResolveGroupIndex(int slot)
{
    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
        return -1;

    int index = g_selected + (slot - GetCarouselCenterSlot());
    while (index < 0)
        index += count;
    while (index >= count)
        index -= count;
    return index;
}

static void UpdateHoveredSlot(POINT ptClient)
{
    int next = GetSlotAtPoint(ptClient);
    bool nextClose = false;
    if (next >= 0)
    {
        RECT closeRect = GetCloseHoverButtonRect(next);
        nextClose = PtInRect(&closeRect, ptClient) != FALSE;
    }
    if (next == g_hoveredSlot && nextClose == g_hoveredCloseButton)
        return;
    g_hoveredSlot = next;
    g_hoveredCloseButton = nextClose;
    if (g_selector && IsWindow(g_selector))
        InvalidateRect(g_selector, nullptr, FALSE);
}

static int GetSlotAtPoint(POINT ptClient)
{
    RECT rcCenter = GetSlotCardRect(GetCarouselCenterSlot());
    if (PtInRect(&rcCenter, ptClient))
        return GetCarouselCenterSlot();

    for (int i = 0; i < GetCarouselSlotCount(); ++i)
    {
        int slot = i;
        RECT rc = GetSlotCardRect(slot);
        if (PtInRect(&rc, ptClient))
            return slot;
    }
    return -1;
}

static void CloseWindowForSlot(int slot)
{
    if (!IsSelectorActive())
        return;

    int groupIndex = ResolveGroupIndex(slot);
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()))
        return;

    AppGroup& group = g_groups[groupIndex];
    HWND target = group.representativeWindow;
    if (!target || !IsWindow(target))
        return;

    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_cardExitActive = true;
    g_cardExitSlot = slot;
    g_cardExitGroupIndex = groupIndex;
    g_cardExitTarget = target;
    g_cardExitDirection = (GetTickCount64() & 1ULL) ? 1.0f : -1.0f;
    g_cardExitStart = GetTickCount64();
    g_selectorAnimation = SelectorAnimationState::CardExit;
    g_selectorAnimationStart = g_cardExitStart;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    SetTimer(g_selector, kSelectorMotionTimerId, GetAnimationTimerInterval(), nullptr);
    UpdateSelectorControls();
}

static void DrawCardGlowAndBorderGDI(HDC hdc, const RECT& rect, int radius, bool selected)
{
    COLORREF bgCol = selected ? g_cardStyle.selectedCardBackground : g_cardStyle.cardBackground;
    COLORREF borderCol = selected ? g_cardStyle.selectedBorderColor : g_cardStyle.borderColor;

    HBRUSH bgBrush = g_createSolidBrush ? g_createSolidBrush(bgCol) : nullptr;
    HPEN borderPen = g_createPen ? g_createPen(PS_SOLID, selected ? 2 : 1, borderCol) : nullptr;

    if (bgBrush && borderPen && g_roundRect && g_deleteObject)
    {
        HGDIOBJ oldBrush = g_selectObject(hdc, bgBrush);
        HGDIOBJ oldPen = g_selectObject(hdc, borderPen);
        g_roundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
        g_selectObject(hdc, oldBrush);
        g_selectObject(hdc, oldPen);
        g_deleteObject(bgBrush);
        g_deleteObject(borderPen);
    }
}

static void PaintBottomShadowD2D(const RECT& objectRect, float radius,
                                 float strength)
{
    if (!g_d2dDCRenderTarget || !g_d2dBrush)
        return;
    const int layers = 3;
    for (int i = 0; i < layers; ++i)
    {
        int spread = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int offset = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int depth = ScaleLayoutPx(static_cast<float>(4 + i * 3));
        RECT shadow = {
            objectRect.left - spread,
            objectRect.bottom - ScaleLayoutPx(1.0f) + offset,
            objectRect.right + spread,
            objectRect.bottom + offset + depth
        };
        float alpha = strength * (i == 0 ? 0.42f : (i == 1 ? 0.22f : 0.09f));
        g_d2dBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, alpha));
        D2D1_ROUNDED_RECT shadowRect = D2D1::RoundedRect(
            D2D1::RectF(static_cast<float>(shadow.left),
                        static_cast<float>(shadow.top),
                        static_cast<float>(shadow.right),
                        static_cast<float>(shadow.bottom)),
            radius + static_cast<float>(spread), radius + static_cast<float>(spread));
        g_d2dDCRenderTarget->FillRoundedRectangle(&shadowRect, g_d2dBrush);
    }
}

static void PaintBottomShadowGDI(HDC hdc, const RECT& objectRect, int radius)
{
    if (!hdc || !g_createSolidBrush || !g_selectObject || !g_getStockObject ||
        !g_roundRect || !g_deleteObject)
        return;
    const COLORREF colors[] = { RGB(10, 10, 13), RGB(16, 16, 20), RGB(23, 23, 28) };
    for (int i = 0; i < 3; ++i)
    {
        int spread = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int offset = ScaleLayoutPx(static_cast<float>(1 + i * 2));
        int depth = ScaleLayoutPx(static_cast<float>(3 + i * 2));
        RECT shadow = { objectRect.left - spread,
                        objectRect.bottom - 1 + offset,
                        objectRect.right + spread,
                        objectRect.bottom + offset + depth };
        HBRUSH brush = g_createSolidBrush(colors[i]);
        HGDIOBJ oldBrush = g_selectObject(hdc, brush);
        HGDIOBJ oldPen = g_selectObject(hdc, g_getStockObject(NULL_PEN));
        g_roundRect(hdc, shadow.left, shadow.top, shadow.right, shadow.bottom,
                    radius + spread, radius + spread);
        g_selectObject(hdc, oldPen);
        g_selectObject(hdc, oldBrush);
        g_deleteObject(brush);
    }
}

static void UnregisterThumbnailSlot(int slot)
{
    if (slot < 0 || slot >= kMaxCarouselSlots)
        return;
    if (g_thumbnailSlots[slot].thumbnail && g_dwmUnregisterThumbnail)
        g_dwmUnregisterThumbnail(g_thumbnailSlots[slot].thumbnail);
    g_thumbnailSlots[slot].thumbnail = nullptr;
    g_thumbnailSlots[slot].source = nullptr;
}

static void UnregisterAllThumbnails()
{
    for (int i = 0; i < kMaxCarouselSlots; ++i)
        UnregisterThumbnailSlot(i);
}

static void UpdateThumbnailProperties(int slot, const RECT& area, HWND source)
{
    if (slot < 0 || slot >= kMaxCarouselSlots ||
        !g_thumbnailSlots[slot].thumbnail || !source ||
        !g_dwmUpdateThumbnailProperties)
        return;

    int sourceWidth = 0;
    int sourceHeight = 0;

    if (IsIconic(source))
    {
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(WINDOWPLACEMENT);
        if (GetWindowPlacement(source, &wp))
        {
            sourceWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
            sourceHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
        }
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        RECT sourceRect = {};
        GetClientRect(source, &sourceRect);
        sourceWidth = sourceRect.right - sourceRect.left;
        sourceHeight = sourceRect.bottom - sourceRect.top;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        RECT windowRect = {};
        GetWindowRect(source, &windowRect);
        sourceWidth = windowRect.right - windowRect.left;
        sourceHeight = windowRect.bottom - windowRect.top;
    }

    if (sourceWidth <= 0 || sourceHeight <= 0)
    {
        sourceWidth = 1600;
        sourceHeight = 1000;
    }

    DwmThumbnailPropertiesLocal properties = {};
    properties.dwFlags = kDwmTnpRectDestination | kDwmTnpOpacity |
                         kDwmTnpVisible;
    RECT destination = ContainedRect(area, sourceWidth, sourceHeight);
    RECT viewport = { 0, 0, g_runtimeSelectorWidth, g_runtimeSelectorHeight };
    RECT clipped = {
        std::max(destination.left, viewport.left),
        std::max(destination.top, viewport.top),
        std::min(destination.right, viewport.right),
        std::min(destination.bottom, viewport.bottom)
    };
    bool visible = clipped.left < clipped.right && clipped.top < clipped.bottom;
    properties.rcDestination = clipped;
    if (!visible)
        properties.rcDestination = RECT{ 0, 0, 0, 0 };
    else
    {
        // Crop the matching source pixels when a card crosses the selector
        // edge. Without rcSource, DWM scales the complete source into the
        // remaining destination rectangle and deforms the preview.
        int destinationWidth = destination.right - destination.left;
        int destinationHeight = destination.bottom - destination.top;
        RECT sourceClip = {
            static_cast<LONG>((static_cast<long long>(clipped.left - destination.left) * sourceWidth) /
                              destinationWidth),
            static_cast<LONG>((static_cast<long long>(clipped.top - destination.top) * sourceHeight) /
                              destinationHeight),
            static_cast<LONG>((static_cast<long long>(clipped.right - destination.left) * sourceWidth) /
                              destinationWidth),
            static_cast<LONG>((static_cast<long long>(clipped.bottom - destination.top) * sourceHeight) /
                              destinationHeight)
        };
        sourceClip.left = std::max<LONG>(0, std::min<LONG>(sourceWidth, sourceClip.left));
        sourceClip.top = std::max<LONG>(0, std::min<LONG>(sourceHeight, sourceClip.top));
        sourceClip.right = std::max<LONG>(sourceClip.left + 1,
                                          std::min<LONG>(sourceWidth, sourceClip.right));
        sourceClip.bottom = std::max<LONG>(sourceClip.top + 1,
                                           std::min<LONG>(sourceHeight, sourceClip.bottom));
        properties.dwFlags |= kDwmTnpRectSource;
        properties.rcSource = sourceClip;
    }
    properties.fVisible = visible ? TRUE : FALSE;
    // Previews DWM siempre nítidas; la translucidez solo pertenece a la card.
    float thumbnailOpacity = g_sceneOpacity;
    if (g_cardExitActive && slot == g_cardExitSlot)
        thumbnailOpacity *= GetCardExitOpacity();
    properties.opacity = static_cast<BYTE>(std::max(0, std::min(255,
        static_cast<int>(roundf(thumbnailOpacity * 255.0f)))));
    g_dwmUpdateThumbnailProperties(g_thumbnailSlots[slot].thumbnail, &properties);
}

static void UpdateThumbnailSlots()
{
    if (!g_selector || !LoadDwmFunctions())
        return;

    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
    {
        UnregisterAllThumbnails();
        return;
    }

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
    {
        int index = ResolveGroupIndex(slot);
        HWND source = (index >= 0) ? g_groups[index].representativeWindow : nullptr;
        if (!source || !IsWindow(source))
        {
            UnregisterThumbnailSlot(slot);
            continue;
        }

        if (g_thumbnailSlots[slot].source != source)
        {
            UnregisterThumbnailSlot(slot);
            HTHUMBNAIL thumbnail = nullptr;
            if (SUCCEEDED(g_dwmRegisterThumbnail(g_selector, source, &thumbnail)))
            {
                g_thumbnailSlots[slot].thumbnail = thumbnail;
                g_thumbnailSlots[slot].source = source;
            }
        }

        RECT area = TransformSceneRect(GetSlotPreviewRect(slot));
        UpdateThumbnailProperties(slot, area, source);
    }
}

static void StopCarouselAnimation()
{
    g_animActive = false;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_animStartTime = 0;
    if (g_selector && IsWindow(g_selector))
    {
        KillTimer(g_selector, kAnimTimerId);
        KillTimer(g_selector, kTabRepeatTimerId);
        KillTimer(g_selector, kSelectorMotionTimerId);
    }
    g_tabRepeatStarted = false;
    g_selectorAnimation = SelectorAnimationState::None;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    g_sceneOpacity = 1.0f;
    g_sceneTiltDegrees = 0.0f;
    g_selectionTiltDirection = 0.0f;
    g_cardSnapScale = 1.0f;
    g_cardSnapStartScale = 1.0f;
    g_selectionStartScaleX = 1.0f;
    g_selectionStartScaleY = 1.0f;
    g_selectionStartTiltDegrees = 0.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitGroupIndex = -1;
    g_cardExitTarget = nullptr;
}

static void StartSelectorClose(HWND target)
{
    if (!g_selector || !IsWindow(g_selector) ||
        g_selectorAnimation == SelectorAnimationState::Closing)
        return;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitGroupIndex = -1;
    g_cardExitTarget = nullptr;
    g_pendingActivationTarget = target;
    g_closeStartScaleX = g_sceneScaleX;
    g_closeStartScaleY = g_sceneScaleY;
    g_closeStartOpacity = g_sceneOpacity;
    if (g_selectorAnimation != SelectorAnimationState::Opening &&
        g_selectorAnimation != SelectorAnimationState::SelectionChange)
    {
        g_sceneScaleX = 1.0f;
        g_sceneScaleY = 1.0f;
        g_sceneTiltDegrees = 0.0f;
    }
    g_selectorAnimation = SelectorAnimationState::Closing;
    g_selectorAnimationStart = GetTickCount64();
    SetTimer(g_selector, kSelectorMotionTimerId, GetAnimationTimerInterval(), nullptr);
    InvalidateRect(g_selector, nullptr, FALSE);
}

static float GetCardExitProgress()
{
    if (!g_cardExitActive)
        return 0.0f;
    return std::min(1.0f,
        static_cast<float>(GetTickCount64() - g_cardExitStart) / AnimationDuration(360.0f));
}

static float GetCardExitOpacity()
{
    float t = GetCardExitProgress();
    float fadeT = std::min(1.0f, t / 0.72f);
    float eased = fadeT * fadeT * (3.0f - 2.0f * fadeT);
    return 1.0f - eased;
}

static float GetCardExitOffsetY()
{
    if (!g_cardExitActive)
        return 0.0f;
    float t = GetCardExitProgress();
    return g_cardExitDirection * ScaleLayoutPx(250.0f) * t * t * t;
}

static void UpdateSelectorMotion()
{
    if (!g_selector || !IsWindow(g_selector))
        return;
    if (g_selectorAnimation == SelectorAnimationState::Open && !g_animActive)
    {
        KillTimer(g_selector, kSelectorMotionTimerId);
        return;
    }
    ULONGLONG elapsed = GetTickCount64() - g_selectorAnimationStart;
    if (g_selectorAnimation == SelectorAnimationState::CardExit)
    {
        if (GetCardExitProgress() >= 1.0f)
        {
            HWND target = g_cardExitTarget;
            g_cardExitActive = false;
            g_cardExitSlot = -1;
            g_cardExitGroupIndex = -1;
            g_cardExitTarget = nullptr;
            g_selectorAnimation = SelectorAnimationState::Open;
            if (target && IsWindow(target) && PostMessageW(target, WM_CLOSE, 0, 0))
                MarkWindowPendingClose(target);
            for (size_t i = 0; i < g_userWindows.size(); ++i)
            {
                if (g_userWindows[i].hwnd == target)
                {
                    g_userWindows.erase(g_userWindows.begin() + i);
                    break;
                }
            }
            RefreshWindowList();
            if (g_groups.empty())
            {
                LeaveActiveState(SelectorState::Canceling);
                StartSelectorClose(nullptr);
                return;
            }
            if (g_selected >= static_cast<int>(g_groups.size()))
                g_selected = static_cast<int>(g_groups.size()) - 1;
            g_hoveredSlot = -1;
            g_hoveredCloseButton = false;
            KillTimer(g_selector, kSelectorMotionTimerId);
            UpdateSelectorControls();
            return;
        }
        UpdateSelectorControls();
        return;
    }
    if (g_selectorAnimation == SelectorAnimationState::Opening)
    {
        const float duration = AnimationDuration(240.0f);
        float t = std::min(1.0f, static_cast<float>(elapsed) / duration);
        // Fluid Pop: una expansión rápida, un único overshoot sutil y
        // un asentamiento corto; no es un rebote elástico.
        static const float phaseTimes[] =
            { 0.0f, 0.58f, 1.0f };
        static const float phaseScales[] =
            { 0.94f, 1.045f, 1.0f };
        int phase = t <= phaseTimes[1] ? 0 :
                    1;
        float phaseSpan = phaseTimes[phase + 1] - phaseTimes[phase];
        float localT = phaseSpan > 0.0f
            ? (t - phaseTimes[phase]) / phaseSpan : 1.0f;
        localT = std::max(0.0f, std::min(1.0f, localT));
        float smoothT = localT * localT * (3.0f - 2.0f * localT);
        g_sceneScaleX = phaseScales[phase] +
                        (phaseScales[phase + 1] - phaseScales[phase]) * smoothT;
        g_sceneScaleY = g_sceneScaleX;
        float opacityT = std::min(1.0f, t / 0.48f);
        g_sceneOpacity = opacityT * opacityT * (3.0f - 2.0f * opacityT);
        if (t >= 1.0f)
        {
            g_sceneScaleX = 1.0f;
            g_sceneScaleY = 1.0f;
            g_sceneOpacity = 1.0f;
            g_selectorAnimation = SelectorAnimationState::Open;
            KillTimer(g_selector, kSelectorMotionTimerId);
        }
    }
    else if (g_selectorAnimation == SelectorAnimationState::SelectionChange)
    {
        const float duration = AnimationDuration(kNavigationDuration);
        float t = std::min(1.0f, static_cast<float>(elapsed) / duration);
        // La navegación se anima exclusivamente mediante g_animOffset. No
        // escalamos ni inclinamos la escena completa: el visualizer y el
        // centro de referencia permanecen perfectamente fijos.
        g_sceneScaleX = 1.0f;
        g_sceneScaleY = 1.0f;
        g_sceneOpacity = 1.0f;
        g_sceneTiltDegrees = 0.0f;
        if (t >= 1.0f)
        {
            g_sceneScaleX = 1.0f;
            g_sceneScaleY = 1.0f;
            g_sceneTiltDegrees = 0.0f;
            g_selectorAnimation = SelectorAnimationState::Open;
            if (!g_animActive)
                KillTimer(g_selector, kSelectorMotionTimerId);
        }
    }
    else if (g_selectorAnimation == SelectorAnimationState::Closing)
    {
        const float duration = AnimationDuration(420.0f);
        float t = std::min(1.0f, static_cast<float>(elapsed) / duration);
        float verticalPhase = std::min(1.0f, t / 0.72f);
        float verticalEase = verticalPhase * verticalPhase * verticalPhase;
        g_sceneScaleY = g_closeStartScaleY * (1.0f - 0.97f * verticalEase);
        float horizontalPhase = std::max(0.0f, (t - 0.72f) / 0.28f);
        g_sceneScaleX = g_closeStartScaleX *
                        (1.0f - 0.92f * horizontalPhase * horizontalPhase);
        g_sceneOpacity = g_closeStartOpacity * (1.0f - t);
        if (t >= 1.0f)
        {
            HWND target = g_pendingActivationTarget;
            g_pendingActivationTarget = nullptr;
            g_sceneScaleX = 1.0f;
            g_sceneScaleY = 1.0f;
            g_sceneTiltDegrees = 0.0f;
            g_selectorAnimation = SelectorAnimationState::None;
            KillTimer(g_selector, kSelectorMotionTimerId);
            g_cleanupInProgress = true;
            DestroySelector();
            g_state = SelectorState::Idle;
            g_cleanupInProgress = false;
            CleanupKeyboardState();
            ActivateWindow(target);
            return;
        }
    }
    UpdateSelectorControls();
}

static void UpdateSelectorControls()
{
    if (!g_selector || !IsWindow(g_selector))
        return;

    CreateUiFonts();
    UpdateThumbnailSlots();
    InvalidateRect(g_selector, nullptr, FALSE);
}

static void EnsureIconBitmap(int groupIndex)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()))
        return;
    if (!g_d2dDCRenderTarget)
        return;
    if (g_groups[groupIndex].icon && !g_groups[groupIndex].iconBitmap)
        g_groups[groupIndex].iconBitmap = CreateD2DBitmapFromHIcon(
            g_d2dDCRenderTarget, g_groups[groupIndex].icon);
}

static ID2D1LinearGradientBrush* EnsureSurfaceGradientBrush(int groupIndex,
                                                            bool selected)
{
    if (groupIndex < 0 || groupIndex >= static_cast<int>(g_groups.size()) ||
        !g_d2dDCRenderTarget)
        return nullptr;

    AppGroup& group = g_groups[groupIndex];
    ID2D1LinearGradientBrush*& brush = selected
        ? group.surfaceBrushSelected : group.surfaceBrushNormal;
    if (brush)
        return brush;

    D2D1_COLOR_F base = selected
        ? D2D1::ColorF(0.035f, 0.040f, 0.045f, kCardSurfaceOpacity)
        : D2D1::ColorF(0.018f, 0.020f, 0.024f, kCardSurfaceOpacity);
    // Columna ambiental horizontalmente modulada: el centro recibe una
    // influencia moderada y los extremos recuperan Black Obsidian.
    D2D1_COLOR_F edge = base;
    D2D1_COLOR_F soft = AddMediaAccent(base, group.mediaAccent,
                                         selected ? 0.145f : 0.115f);
    D2D1_COLOR_F center = AddMediaAccent(base, group.mediaAccent,
                                           selected ? 0.300f : 0.235f);

    D2D1_GRADIENT_STOP stops[5] = {};
    stops[0].position = 0.0f;
    stops[0].color = edge;
    stops[1].position = 0.22f;
    stops[1].color = soft;
    stops[2].position = 0.50f;
    stops[2].color = center;
    stops[3].position = 0.78f;
    stops[3].color = soft;
    stops[4].position = 1.0f;
    stops[4].color = edge;

    ID2D1GradientStopCollection* stopCollection = nullptr;
    HRESULT hr = g_d2dDCRenderTarget->CreateGradientStopCollection(
        stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.startPoint = D2D1::Point2F(0.0f, 0.5f);
    properties.endPoint = D2D1::Point2F(1.0f, 0.5f);
    hr = g_d2dDCRenderTarget->CreateLinearGradientBrush(
        properties, stopCollection, &brush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? brush : nullptr;
}

static ID2D1RadialGradientBrush* EnsureCloseAccentBrush()
{
    if (g_closeAccentBrush || !g_d2dDCRenderTarget)
        return g_closeAccentBrush;

    D2D1_GRADIENT_STOP stops[3] = {};
    stops[0].position = 0.0f;
    stops[0].color = D2D1::ColorF(0.48f, 0.055f, 0.065f, 0.42f);
    stops[1].position = 0.38f;
    stops[1].color = D2D1::ColorF(0.30f, 0.030f, 0.040f, 0.20f);
    stops[2].position = 1.0f;
    stops[2].color = D2D1::ColorF(0.18f, 0.015f, 0.020f, 0.0f);

    ID2D1GradientStopCollection* stopCollection = nullptr;
    HRESULT hr = g_d2dDCRenderTarget->CreateGradientStopCollection(
        stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection);
    if (FAILED(hr) || !stopCollection)
        return nullptr;

    D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES properties = {};
    properties.center = D2D1::Point2F(0.5f, 0.5f);
    properties.gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
    properties.radiusX = 1.0f;
    properties.radiusY = 1.0f;
    hr = g_d2dDCRenderTarget->CreateRadialGradientBrush(
        properties, stopCollection, &g_closeAccentBrush);
    stopCollection->Release();
    return SUCCEEDED(hr) ? g_closeAccentBrush : nullptr;
}

static void PaintSlotHeaderD2D(int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    std::wstring text = MakeSlotText(group, selected);

    int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 20 : (distFromCenter < 1.5f ? 18 : 16)));
    int paddingLeft = ScaleLayoutPx(static_cast<float>(selected ? 10 : (distFromCenter < 1.5f ? 8 : 6)));
    int iconX = rc.left + paddingLeft;
    int iconY = rc.top + (rc.bottom - rc.top - iconSize) / 2;
    int textX = iconX + iconSize + ScaleLayoutPx(8.0f);
    RECT iconRect = { iconX, iconY, iconX + iconSize, iconY + iconSize };
    if (slot == GetCarouselCenterSlot())
    {
        RECT cardRect = GetSlotCardRect(slot);
        float pivotX = (static_cast<float>(cardRect.left) + cardRect.right) * 0.5f;
        float pivotY = (static_cast<float>(cardRect.top) + cardRect.bottom) * 0.5f;
        iconRect = ScaleRectAroundPoint(iconRect, g_cardSnapScale, pivotX, pivotY);
    }

    // El encabezado es deliberadamente transparente: no crea una superficie
    // oscura independiente sobre el gradiente Dynamic Obsidian de la card.
    // El icono y el texto se dibujan directamente sobre la superficie iluminada.

    EnsureIconBitmap(groupIndex);
    if (group.iconBitmap)
    {
        D2D1_RECT_F iconDest = D2D1::RectF(
            static_cast<float>(iconRect.left),
            static_cast<float>(iconRect.top),
            static_cast<float>(iconRect.right),
            static_cast<float>(iconRect.bottom));
        g_d2dDCRenderTarget->DrawBitmap(
            group.iconBitmap, &iconDest, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    IDWriteTextFormat* format = selected ? g_dwriteSelectedFormat : g_dwriteNormalFormat;
    if (format)
    {
        D2D1_RECT_F textRectD2D = D2D1::RectF(
            static_cast<float>(textX), static_cast<float>(rc.top),
            static_cast<float>(rc.right - ScaleLayoutPx(8.0f)), static_cast<float>(rc.bottom));
        D2D1_COLOR_F textColor = selected
            ? D2D1::ColorF(0.965f, 0.968f, 0.98f, 1.0f)
            : D2D1::ColorF(0.815f, 0.823f, 0.855f, 1.0f);
        g_d2dBrush->SetColor(textColor);
        g_d2dDCRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
                                      format, &textRectD2D, g_d2dBrush);
    }
}

static void PaintCounterD2D()
{
    RECT rc = GetCounterRect();
    std::wstring text = MakeCounterText();

    // El contador no tiene superficie propia: el texto flota directamente
    // sobre la misma card translúcida, sin cápsula, rectángulo ni borde.

    if (g_dwriteCounterFormat)
    {
        D2D1_RECT_F textRectD2D = D2D1::RectF(
            static_cast<float>(rc.left), static_cast<float>(rc.top),
            static_cast<float>(rc.right), static_cast<float>(rc.bottom));
        g_d2dBrush->SetColor(D2D1::ColorF(0.73f, 0.74f, 0.77f, 1.0f));
        g_d2dDCRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()),
                                      g_dwriteCounterFormat, &textRectD2D, g_d2dBrush);
    }
}

static void PaintSlotHeaderGDI(HDC hdc, int slot, int groupIndex, bool selected, float distFromCenter)
{
    RECT rc = GetSlotHeaderRect(slot);
    AppGroup& group = g_groups[groupIndex];
    std::wstring text = MakeSlotText(group, selected);

    int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 20 : (distFromCenter < 1.5f ? 18 : 16)));
    int paddingLeft = ScaleLayoutPx(static_cast<float>(selected ? 10 : (distFromCenter < 1.5f ? 8 : 6)));
    int iconX = rc.left + paddingLeft;
    int iconY = rc.top + (rc.bottom - rc.top - iconSize) / 2;
    int textX = iconX + iconSize + ScaleLayoutPx(8.0f);
    RECT iconRect = { iconX, iconY, iconX + iconSize, iconY + iconSize };
    if (slot == GetCarouselCenterSlot())
    {
        RECT cardRect = GetSlotCardRect(slot);
        float pivotX = (static_cast<float>(cardRect.left) + cardRect.right) * 0.5f;
        float pivotY = (static_cast<float>(cardRect.top) + cardRect.bottom) * 0.5f;
        iconRect = ScaleRectAroundPoint(iconRect, g_cardSnapScale, pivotX, pivotY);
    }

    // Sin panel, relleno ni borde propio: el encabezado deja ver la card.

    RECT textRect = rc;
    textRect.left = textX;
    textRect.right = rc.right - ScaleLayoutPx(8.0f);
    HFONT font = selected ? g_selectedNameFont : g_nameFont;
    COLORREF textColor = selected ? RGB(246, 247, 250) : RGB(208, 210, 218);
    if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
    if (g_setTextColor) g_setTextColor(hdc, textColor);
    HGDIOBJ oldFont = font && g_selectObject ? g_selectObject(hdc, font) : nullptr;
    DrawTextW(hdc, text.c_str(), -1, &textRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);

    if (group.icon)
    {
        DrawIconEx(hdc, iconRect.left, iconRect.top, group.icon,
                   iconRect.right - iconRect.left, iconRect.bottom - iconRect.top,
                   0, nullptr, DI_NORMAL);
    }
}

static void PaintCounterGDI(HDC hdc)
{
    RECT rc = GetCounterRect();
    std::wstring text = MakeCounterText();

    // GDI no ofrece alpha blending con el pincel sólido usado por este
    // fallback; no dibujamos una superficie opaca independiente detrás del texto.
    if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
    if (g_setTextColor) g_setTextColor(hdc, RGB(186, 188, 196));
    HGDIOBJ oldFont = g_secondaryFont && g_selectObject ?
        g_selectObject(hdc, g_secondaryFont) : nullptr;
    DrawTextW(hdc, text.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);
}

static void PaintCenterAccentMarginD2D(const RECT& cardRect, int groupIndex, float radius)
{
    if (!g_d2dDCRenderTarget || !g_d2dBrush || groupIndex < 0 ||
        groupIndex >= static_cast<int>(g_groups.size()))
        return;

    COLORREF accent = g_groups[groupIndex].mediaAccent.valid
        ? g_groups[groupIndex].mediaAccent.color
        : RGB(18, 22, 28);
    float r = GetRValue(accent) / 255.0f;
    float g = GetGValue(accent) / 255.0f;
    float b = GetBValue(accent) / 255.0f;

    const float margin = static_cast<float>(ScaleLayoutPx(5.0f));
    D2D1_ROUNDED_RECT outer = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(cardRect.left) - margin,
                    static_cast<float>(cardRect.top) - margin,
                    static_cast<float>(cardRect.right) + margin,
                    static_cast<float>(cardRect.bottom) + margin),
        radius + margin, radius + margin);

    // Marco vectorial: no es un bitmap ni una sombra rasterizada. Sigue
    // exactamente el contorno redondeado de la card central y toma el color
    // dominante extraído de su contenido.
    g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, 0.34f));
    g_d2dDCRenderTarget->DrawRoundedRectangle(&outer, g_d2dBrush,
                                               static_cast<float>(ScaleLayoutPx(2.0f)));

    D2D1_ROUNDED_RECT inner = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(cardRect.left) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.top) - ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.right) + ScaleLayoutPx(2.0f),
                    static_cast<float>(cardRect.bottom) + ScaleLayoutPx(2.0f)),
        radius + ScaleLayoutPx(2.0f), radius + ScaleLayoutPx(2.0f));
    g_d2dBrush->SetColor(D2D1::ColorF(r, g, b, 0.18f));
    g_d2dDCRenderTarget->DrawRoundedRectangle(&inner, g_d2dBrush,
                                               static_cast<float>(ScaleLayoutPx(1.0f)));
}

static void PaintSelectorScene(HWND hwnd, HDC hdc)
{
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int count = static_cast<int>(g_groups.size());
    if (count <= 0)
        return;

    bool d2dActive = LoadD2DAndDWrite() && g_d2dDCRenderTarget && g_d2dBrush &&
                     SUCCEEDED(g_d2dDCRenderTarget->BindDC(hdc, &clientRect));

    if (d2dActive)
    {
        g_d2dDCRenderTarget->BeginDraw();
        g_d2dDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        // Clear completamente para dejar que DWM glass sea visible
        g_d2dDCRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        g_d2dDCRenderTarget->PushAxisAlignedClip(
            D2D1::RectF(0.0f, 0.0f,
                        static_cast<float>(g_runtimeSelectorWidth),
                        static_cast<float>(g_runtimeSelectorHeight)),
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        float sceneScaleX = 1.0f;
        float sceneScaleY = 1.0f;
        GetSceneScale(sceneScaleX, sceneScaleY);
        if (sceneScaleX != 1.0f || sceneScaleY != 1.0f)
        {
            g_d2dDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(
                D2D1::SizeF(sceneScaleX, sceneScaleY),
                D2D1::Point2F(static_cast<float>(g_runtimeSelectorWidth) * 0.5f,
                              static_cast<float>(g_runtimeSelectorHeight) * 0.5f)));
        }
        ID2D1Layer* sceneOpacityLayer = nullptr;
        if (g_sceneOpacity < 0.999f &&
            SUCCEEDED(g_d2dDCRenderTarget->CreateLayer(nullptr, &sceneOpacityLayer)) &&
            sceneOpacityLayer)
        {
            D2D1_LAYER_PARAMETERS opacityParameters = D2D1::LayerParameters(
                D2D1::InfiniteRect(), nullptr,
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                D2D1::Matrix3x2F::Identity(), g_sceneOpacity, nullptr,
                D2D1_LAYER_OPTIONS_NONE);
            g_d2dDCRenderTarget->PushLayer(&opacityParameters, sceneOpacityLayer);
        }

        // Glass DWM transparente: no se dibuja bitmap y el vídeo permanece
        // vivo detrás; solo se añade un tinte oscuro muy ligero.
        if (g_dwmGlassEnabled)
        {
            D2D1_RECT_F bgRect = D2D1::RectF(
                static_cast<float>(clientRect.left),
                static_cast<float>(clientRect.top),
                static_cast<float>(clientRect.right),
                static_cast<float>(clientRect.bottom));
            // Mantener el color original de la superficie; el ajuste de borde
            // se realiza únicamente mediante DWMWA_BORDER_COLOR.
            g_d2dBrush->SetColor(D2D1::ColorF(0.035f, 0.050f, 0.075f, 0.20f));
            g_d2dDCRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }
        else if (g_blurBackgroundCreated && g_blurPixels && g_blurDC)
        {
            D2D1_RECT_F bgRect = D2D1::RectF(
                static_cast<float>(clientRect.left),
                static_cast<float>(clientRect.top),
                static_cast<float>(clientRect.right),
                static_cast<float>(clientRect.bottom));

            if (!g_blurD2DBitmap)
            {
                D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                    96.0f, 96.0f);
                g_d2dDCRenderTarget->CreateBitmap(
                    D2D1::SizeU(static_cast<UINT32>(g_blurWidth), static_cast<UINT32>(g_blurHeight)),
                    g_blurPixels, static_cast<UINT32>(g_blurWidth * 4), &props,
                    &g_blurD2DBitmap);
            }

            if (g_blurD2DBitmap)
                // Opacidad completa: evita que el vídeo vivo se mezcle con la
                // captura y deje el primer frame superpuesto.
                g_d2dDCRenderTarget->DrawBitmap(g_blurD2DBitmap, &bgRect, 1.0f,
                                                 D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);

            // Capa obsidiana separada: evita oscurecer en exceso los píxeles
            // capturados y mantiene control independiente sobre la composición.
            g_d2dBrush->SetColor(D2D1::ColorF(0.035f, 0.050f, 0.075f, 0.15f));
            g_d2dDCRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }
        else
        {
            // Fallback si no hay blur
            D2D1_RECT_F bgRect = D2D1::RectF(
                static_cast<float>(clientRect.left),
                static_cast<float>(clientRect.top),
                static_cast<float>(clientRect.right),
                static_cast<float>(clientRect.bottom));
            g_d2dBrush->SetColor(D2D1::ColorF(0.70f, 0.74f, 0.82f, 0.18f));
            g_d2dDCRenderTarget->FillRectangle(&bgRect, g_d2dBrush);
        }

        // Borde vectorial sutil del contenedor. La región real de la ventana
        // ya recorta las esquinas, por lo que no queda un rectángulo cuadrado.
        const float containerRadius = static_cast<float>(ScaleLayoutPx(27.0f));
        const float containerInset = DpiPx(0.75f);
        D2D1_ROUNDED_RECT containerRect = D2D1::RoundedRect(
            D2D1::RectF(containerInset, containerInset,
                        static_cast<float>(clientRect.right) - containerInset,
                        static_cast<float>(clientRect.bottom) - containerInset),
            containerRadius, containerRadius);
        g_d2dBrush->SetColor(D2D1::ColorF(0.32f, 0.36f, 0.43f, 0.30f));
        g_d2dDCRenderTarget->DrawRoundedRectangle(&containerRect, g_d2dBrush, DpiPx(1.0f));

        RECT containerShadow = {
            ScaleLayoutPx(12.0f),
            clientRect.bottom - ScaleLayoutPx(8.0f),
            clientRect.right - ScaleLayoutPx(12.0f),
            clientRect.bottom
        };
        PaintBottomShadowD2D(containerShadow, ScaleLayoutPx(12.0f), 0.34f);

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
        {
            int index = ResolveGroupIndex(slot);
            if (index < 0)
                continue;

            ID2D1Layer* cardExitLayer = nullptr;
            if (g_cardExitActive && slot == g_cardExitSlot && g_d2dDCRenderTarget)
            {
                float opacity = GetCardExitOpacity();
                if (SUCCEEDED(g_d2dDCRenderTarget->CreateLayer(nullptr, &cardExitLayer)) &&
                    cardExitLayer)
                {
                    D2D1_LAYER_PARAMETERS layerParameters = D2D1::LayerParameters(
                        D2D1::InfiniteRect(), nullptr,
                        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                        D2D1::Matrix3x2F::Identity(), opacity, nullptr,
                        D2D1_LAYER_OPTIONS_NONE);
                    g_d2dDCRenderTarget->PushLayer(&layerParameters, cardExitLayer);
                }
            }

            float distFromCenter = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
            bool selected = (distFromCenter < 0.5f);
            RECT cardRect = GetSlotCardRect(slot);
            float radius = selected ? static_cast<float>(g_cardStyle.selectedCornerRadius) * g_uiScale.value
                                    : static_cast<float>(g_cardStyle.cornerRadius) * g_uiScale.value;
            PaintBottomShadowD2D(cardRect, radius, selected ? 0.28f : 0.20f);
            if (selected && slot == GetCarouselCenterSlot())
                PaintCenterAccentMarginD2D(cardRect, index, radius);

            D2D1_COLOR_F bgCol = selected
                ? D2D1::ColorF(0.035f, 0.040f, 0.045f, kCardSurfaceOpacity)
                : D2D1::ColorF(0.018f, 0.020f, 0.024f, kCardSurfaceOpacity);
            D2D1_ROUNDED_RECT cr = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(cardRect.left) + 0.5f, static_cast<float>(cardRect.top) + 0.5f,
                            static_cast<float>(cardRect.right) - 0.5f, static_cast<float>(cardRect.bottom) - 0.5f),
                radius, radius);
            ID2D1LinearGradientBrush* surfaceBrush =
                EnsureSurfaceGradientBrush(index, selected);
            if (surfaceBrush)
            {
                float cardHeight = static_cast<float>(cardRect.bottom - cardRect.top);
                surfaceBrush->SetStartPoint(D2D1::Point2F(
                    static_cast<float>(cardRect.left),
                    static_cast<float>(cardRect.top) + cardHeight * 0.50f));
                surfaceBrush->SetEndPoint(D2D1::Point2F(
                    static_cast<float>(cardRect.right),
                    static_cast<float>(cardRect.top) + cardHeight * 0.50f));
                g_d2dDCRenderTarget->FillRoundedRectangle(&cr, surfaceBrush);
            }
            else
            {
                g_d2dBrush->SetColor(bgCol);
                g_d2dDCRenderTarget->FillRoundedRectangle(&cr, g_d2dBrush);
            }

            D2D1_COLOR_F borderCol = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.92f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.82f);
            g_d2dBrush->SetColor(borderCol);
            g_d2dDCRenderTarget->DrawRoundedRectangle(&cr, g_d2dBrush,
                                                      DpiPx(selected ? 1.5f : 1.0f));

            RECT prevRect = GetSlotPreviewRect(slot);
            D2D1_ROUNDED_RECT pr = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(prevRect.left) + 0.5f, static_cast<float>(prevRect.top) + 0.5f,
                            static_cast<float>(prevRect.right) - 0.5f, static_cast<float>(prevRect.bottom) - 0.5f),
                DpiPx(6.0f), DpiPx(6.0f));
            // No se pinta un panel opaco detrás del thumbnail. El thumbnail
            // DWM permanece nítido y con opacity=255; los huecos dejan ver la
            // superficie translúcida de la card.
            D2D1_COLOR_F prevBorder = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.60f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.50f);
            g_d2dBrush->SetColor(prevBorder);
            g_d2dDCRenderTarget->DrawRoundedRectangle(&pr, g_d2dBrush, DpiPx(1.0f));

            if (g_groups[index].representativeWindow &&
                IsIconic(g_groups[index].representativeWindow) &&
                g_groups[index].icon)
            {
                EnsureIconBitmap(index);
                int relative = slot - GetCarouselCenterSlot();
                int iconSize = ScaleLayoutPx(static_cast<float>(selected ? 56 : (abs(relative) == 1 ? 40 : 28)));
                int cx = prevRect.left + (prevRect.right - prevRect.left - iconSize) / 2;
                int cy = prevRect.top + (prevRect.bottom - prevRect.top - iconSize) / 2;
                if (g_groups[index].iconBitmap)
                {
                    D2D1_RECT_F iconDest = D2D1::RectF(
                        static_cast<float>(cx),
                        static_cast<float>(cy),
                        static_cast<float>(cx + iconSize),
                        static_cast<float>(cy + iconSize));
                    g_d2dDCRenderTarget->DrawBitmap(
                        g_groups[index].iconBitmap, &iconDest, 1.0f,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            }

            PaintSlotHeaderD2D(slot, index, selected, distFromCenter);

            if (g_hoveredSlot == slot)
            {
                const float hoverOffset = DpiPx(3.0f);
                D2D1_ROUNDED_RECT hoverRect = D2D1::RoundedRect(
                    D2D1::RectF(static_cast<float>(cardRect.left) - hoverOffset,
                                static_cast<float>(cardRect.top) - hoverOffset,
                                static_cast<float>(cardRect.right) + hoverOffset,
                                static_cast<float>(cardRect.bottom) + hoverOffset),
                    radius + DpiPx(2.0f), radius + DpiPx(2.0f));
                COLORREF accent = g_groups[index].mediaAccent.valid
                    ? g_groups[index].mediaAccent.color : RGB(110, 116, 130);
                g_d2dBrush->SetColor(D2D1::ColorF(
                    GetRValue(accent) / 255.0f, GetGValue(accent) / 255.0f,
                    GetBValue(accent) / 255.0f, 0.42f));
                g_d2dDCRenderTarget->DrawRoundedRectangle(&hoverRect, g_d2dBrush, DpiPx(1.25f));

                if (g_hoveredCloseButton)
                {
                    RECT closeRect = GetCloseHoverButtonRect(slot);
                    ID2D1RadialGradientBrush* closeBrush = EnsureCloseAccentBrush();
                    ID2D1RoundedRectangleGeometry* cardMask = nullptr;
                    ID2D1Layer* closeLayer = nullptr;
                    if (g_d2dFactory &&
                        SUCCEEDED(g_d2dFactory->CreateRoundedRectangleGeometry(&cr, &cardMask)) &&
                        cardMask && g_d2dDCRenderTarget &&
                        SUCCEEDED(g_d2dDCRenderTarget->CreateLayer(nullptr, &closeLayer)) &&
                        closeLayer)
                    {
                        g_d2dDCRenderTarget->PushLayer(
                            D2D1::LayerParameters(D2D1::InfiniteRect(), cardMask), closeLayer);
                    }
                    if (closeBrush)
                    {
                        closeBrush->SetCenter(D2D1::Point2F(
                            static_cast<float>(closeRect.left + closeRect.right) * 0.5f,
                            static_cast<float>(closeRect.top + closeRect.bottom) * 0.5f));
                        float glowRadiusX = static_cast<float>(ScaleLayoutPx(58.0f));
                        float glowRadiusY = static_cast<float>(ScaleLayoutPx(52.0f));
                        closeBrush->SetRadiusX(glowRadiusX);
                        closeBrush->SetRadiusY(glowRadiusY);
                        D2D1_ELLIPSE glowEllipse = D2D1::Ellipse(
                            D2D1::Point2F(
                                static_cast<float>(closeRect.left + closeRect.right) * 0.5f,
                                static_cast<float>(closeRect.top + closeRect.bottom) * 0.5f),
                            glowRadiusX, glowRadiusY);
                        g_d2dDCRenderTarget->FillEllipse(&glowEllipse, closeBrush);
                    }
                    if (closeLayer)
                    {
                        g_d2dDCRenderTarget->PopLayer();
                        closeLayer->Release();
                    }
                    if (cardMask)
                        cardMask->Release();

                    int buttonPad = ScaleLayoutPx(2.0f);
                    RECT buttonRect = {
                        closeRect.left - buttonPad, closeRect.top - buttonPad,
                        closeRect.right + buttonPad, closeRect.bottom + buttonPad
                    };
                    ID2D1PathGeometry* closeGeometry =
                        CreateCloseButtonGeometry(buttonRect);
                    if (closeGeometry)
                    {
                        g_d2dBrush->SetColor(D2D1::ColorF(0.035f, 0.018f, 0.022f, 0.75f));
                        g_d2dDCRenderTarget->FillGeometry(closeGeometry, g_d2dBrush);
                        g_d2dBrush->SetColor(D2D1::ColorF(0.45f, 0.10f, 0.12f, 0.62f));
                        g_d2dDCRenderTarget->DrawGeometry(closeGeometry, g_d2dBrush, DpiPx(1.0f));
                        closeGeometry->Release();
                    }
                }

                if (g_dwriteCloseFormat)
                {
                    RECT closeRect = GetCloseHoverButtonRect(slot);
                    D2D1_RECT_F closeTextRect = D2D1::RectF(
                        static_cast<float>(closeRect.left), static_cast<float>(closeRect.top),
                        static_cast<float>(closeRect.right), static_cast<float>(closeRect.bottom));
                    g_d2dBrush->SetColor(g_hoveredCloseButton
                        ? D2D1::ColorF(0.86f, 0.38f, 0.40f, 1.0f)
                        : D2D1::ColorF(0.62f, 0.24f, 0.25f, 0.92f));
                    const wchar_t closeGlyph[] = L"\x00D7";
                    g_d2dDCRenderTarget->DrawText(closeGlyph, 1, g_dwriteCloseFormat,
                                                  &closeTextRect, g_d2dBrush);
                }
            }
            if (cardExitLayer)
            {
                g_d2dDCRenderTarget->PopLayer();
                cardExitLayer->Release();
            }
        }

        PaintCounterD2D();
        if (sceneOpacityLayer)
        {
            g_d2dDCRenderTarget->PopLayer();
            sceneOpacityLayer->Release();
        }
        g_d2dDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        g_d2dDCRenderTarget->PopAxisAlignedClip();
        g_d2dDCRenderTarget->EndDraw();
        return;
    }

    RECT containerShadow = {
        ScaleLayoutPx(12.0f),
        clientRect.bottom - ScaleLayoutPx(8.0f),
        clientRect.right - ScaleLayoutPx(12.0f),
        clientRect.bottom
    };
    PaintBottomShadowGDI(hdc, containerShadow, ScaleLayoutPx(12.0f));

    for (int slot = 0; slot < GetCarouselSlotCount(); ++slot)
    {
        int index = ResolveGroupIndex(slot);
        if (index < 0)
            continue;
        float dist = fabsf(static_cast<float>(slot - GetCarouselCenterSlot()) + g_animOffset);
        bool selected = (dist < 0.5f);
        RECT cardRect = GetSlotCardRect(slot);
        int radius = selected ? ScaleLayoutPx(static_cast<float>(g_cardStyle.selectedCornerRadius))
                              : ScaleLayoutPx(static_cast<float>(g_cardStyle.cornerRadius));
        PaintBottomShadowGDI(hdc, cardRect, radius);
        DrawCardGlowAndBorderGDI(hdc, cardRect, radius, selected);
        PaintSlotHeaderGDI(hdc, slot, index, selected, dist);
        if (g_hoveredSlot == slot)
        {
            HPEN hoverPen = g_createPen ? g_createPen(PS_SOLID, 1, RGB(110, 116, 130)) : nullptr;
            if (hoverPen && g_selectObject && g_getStockObject && g_roundRect && g_deleteObject)
            {
                RECT hoverRect = cardRect;
                hoverRect.left -= 3; hoverRect.top -= 3;
                hoverRect.right += 3; hoverRect.bottom += 3;
                HGDIOBJ oldPen = g_selectObject(hdc, hoverPen);
                HGDIOBJ oldBrush = g_selectObject(hdc, g_getStockObject(NULL_BRUSH));
                g_roundRect(hdc, hoverRect.left, hoverRect.top, hoverRect.right, hoverRect.bottom, radius + 2, radius + 2);
                g_selectObject(hdc, oldBrush);
                g_selectObject(hdc, oldPen);
                g_deleteObject(hoverPen);
            }
            RECT closeRect = GetCloseHoverButtonRect(slot);
            if (g_hoveredCloseButton && g_createSolidBrush && g_createPen &&
                g_selectObject && g_roundRect && g_deleteObject)
            {
                RECT buttonRect = closeRect;
                int buttonPad = ScaleLayoutPx(2.0f);
                buttonRect.left -= buttonPad; buttonRect.top -= buttonPad;
                buttonRect.right += buttonPad; buttonRect.bottom += buttonPad;
                HBRUSH buttonBrush = g_createSolidBrush(RGB(24, 8, 10));
                HPEN buttonPen = g_createPen(PS_SOLID, 1, RGB(122, 36, 42));
                HGDIOBJ oldBrush = g_selectObject(hdc, buttonBrush);
                HGDIOBJ oldPen = g_selectObject(hdc, buttonPen);
                g_roundRect(hdc, buttonRect.left, buttonRect.top,
                            buttonRect.right, buttonRect.bottom,
                            ScaleLayoutPx(12.0f), ScaleLayoutPx(6.0f));
                g_selectObject(hdc, oldBrush);
                g_selectObject(hdc, oldPen);
                g_deleteObject(buttonBrush);
                g_deleteObject(buttonPen);
            }
            if (g_setBkMode) g_setBkMode(hdc, TRANSPARENT);
            if (g_setTextColor)
                g_setTextColor(hdc, g_hoveredCloseButton ? RGB(220, 100, 104) : RGB(158, 61, 64));
            HGDIOBJ oldFont = g_selectedNameFont && g_selectObject ?
                g_selectObject(hdc, g_selectedNameFont) : nullptr;
            DrawTextW(hdc, L"\x00D7", -1, &closeRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if (oldFont && g_selectObject) g_selectObject(hdc, oldFont);
        }
    }
    PaintCounterGDI(hdc);
}

static LRESULT CALLBACK SelectorWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        UpdateSelectorControls();
        return 0;

    case WM_TIMER:
    {
        if (wParam == kTabRepeatTimerId)
        {
            if (!IsSelectorActive() || !g_uiTabDown)
            {
                KillTimer(hwnd, kTabRepeatTimerId);
                g_tabRepeatStarted = false;
                return 0;
            }
            bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            if (!g_tabRepeatStarted)
                g_tabRepeatStarted = true;
            if (shiftDown) SelectPrevious();
            else SelectNext();
            SetTimer(hwnd, kTabRepeatTimerId, kTabRepeatIntervalMs, nullptr);
            return 0;
        }

        if (wParam == kActivityTimerId)
        {
            // Failsafe: existe únicamente mientras el selector está abierto.
            if (g_state == SelectorState::SelectorActive)
            {
                ULONGLONG now = GetTickCount64();
                bool invalidWindow = !g_selector || !IsWindow(g_selector);
                bool modifiersDown =
                    (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
                bool staleWithoutPhysicalSession =
                    g_lastSelectorActivity != 0 &&
                    now - g_lastSelectorActivity > kSelectorFailsafeMs &&
                    !g_uiTabDown && !modifiersDown;
                if (invalidWindow || staleWithoutPhysicalSession)
                    EmergencyCloseSelector();
            }
            return 0;
        }

        if (wParam == kSelectorMotionTimerId)
        {
            UpdateSelectorMotion();
            UpdateCarouselAnimation(hwnd);
            return 0;
        }

        if (wParam == kAnimTimerId)
        {
            if (!g_animActive)
            {
                KillTimer(hwnd, kAnimTimerId);
                return 0;
            }

            ULONGLONG now = GetTickCount64();
            ULONGLONG elapsed = now - g_animStartTime;
            if (elapsed >= static_cast<ULONGLONG>(AnimationDuration(kNavigationDuration)))
            {
                g_animActive = false;
                g_animOffset = 0.0f;
                g_animStartOffset = 0.0f;
                KillTimer(hwnd, kAnimTimerId);
            }
            else
            {
                float t = static_cast<float>(elapsed) / kNavigationDuration;
                float ease = EaseOutCubic(t);
                g_animOffset = g_animStartOffset * (1.0f - ease);
            }
            UpdateSelectorControls();
            return 0;
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        PaintSelectorScene(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_COMMAND:
        return 0;

    case WM_DPICHANGED:
        // El tamaño y la posición los fija CreateSelector en píxeles físicos
        // para el monitor de destino; no se acepta el rectángulo sugerido.
        return 0;

    case WM_SIZE:
        UpdateThumbnailSlots();
        return 0;

    case WM_CLOSE:
        return 0;

    case WM_DESTROY:
        StopCarouselAnimation();
        KillTimer(hwnd, kActivityTimerId);
        UnregisterAllThumbnails();
        if (g_selector == hwnd)
            g_selector = nullptr;
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static bool RegisterSelectorClasses()
{
    if (g_classesRegistered)
        return true;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = SelectorWndProc;
    wc.hInstance = g_hModule;
    wc.lpszClassName = kWindowClassName;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.hbrBackground = nullptr;
    if (!RegisterClassW(&wc))
    {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        return false;
    }

    g_classesRegistered = true;
    return true;
}

static void UnregisterSelectorClasses()
{
    if (!g_classesRegistered)
        return;
    UnregisterClassW(kWindowClassName, g_hModule);
    g_classesRegistered = false;
}

static void ApplySelectorRoundedRegion(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd) || !LoadGdiFunctions() || !g_createRoundRectRgn)
        return;

    RECT rc = {};
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0)
        return;

    int radius = std::max(ScaleLayoutPx(27.0f), 16);
    radius = std::min(radius, std::min(width, height) / 2);
    HRGN region = g_createRoundRectRgn(0, 0, width + 1, height + 1, radius, radius);
    if (region)
        SetWindowRgn(hwnd, region, TRUE);
}

static bool InitializePersistentSelector()
{
    if (g_selector && IsWindow(g_selector))
        return true;

    if (g_selectorAnimation == SelectorAnimationState::Closing)
        g_selectorAnimation = SelectorAnimationState::None;

    if (!RegisterSelectorClasses())
        return false;

    // Creada por el UI thread (Per-Monitor-V2) con el HMODULE del mod.
    HWND selector = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWindowClassName, L"",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hModule, nullptr);

    if (!selector)
    {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        return false;
    }

    g_selector = selector;
    RemoveNativeSelectorFrame(g_selector);
    ApplySelectorVisuals(g_selector);

    // Precalentar el renderer mientras el selector está oculto. Esto elimina
    // la creación de D2D/DWrite/fonts del camino crítico de Alt+Tab.
    LoadD2DAndDWrite();
    CreateUiFonts();
    UpdateSelectorControls();
    ShowWindow(g_selector, SW_HIDE);
    return true;
}

// Se ejecuta siempre en el UI thread, nunca en un hook.
static bool CreateSelector(bool shiftHeld)
{
    if (g_state != SelectorState::SelectorActive || g_cleanupInProgress)
        return false;

    // El selector se crea y precalienta únicamente durante la inicialización
    // del UI thread. Nunca se crea ni se carga en el camino de Alt+Tab.
    if (!g_selector || !IsWindow(g_selector))
        return false;

    g_selectorOpening = true;
    g_selectorOriginWindow = GetForegroundWindow();
    MarkSelectorActivity();

    g_pendingCloseWindows.clear();
    RefreshWindowList();
    if (g_groups.empty())
    {
        g_selectorOpening = false;
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(g_selectorOriginWindow,
                                         MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo))
    {
        g_selectorOpening = false;
        return false;
    }

    const RECT& work = monitorInfo.rcWork;
    UpdateUIScaleForWorkArea(work, GetMonitorDpiScale(monitor));
    // Si el monitor tiene otra escala, reconstruir TextFormat y fuentes GDI
    // antes de mostrar (no reutilizar formatos de otra escala).
    LoadD2DAndDWrite();
    CreateUiFonts();
    int x = work.left + ((work.right - work.left) - g_runtimeSelectorWidth) / 2;
    int y = work.top + ((work.bottom - work.top) - g_runtimeSelectorHeight) / 2;

    StopCarouselAnimation();
    ReleaseBlurBackground();

    // Con DWM glass el fondo es transparente y vivo; no se captura ningún
    // frame estático. Si DWM no está disponible, se usa el blur propio.
    if (!g_dwmGlassEnabled)
        CreateBlurBackground(x, y);

    SetWindowPos(g_selector, HWND_TOPMOST, x, y,
                 g_runtimeSelectorWidth, g_runtimeSelectorHeight,
                 SWP_NOACTIVATE | SWP_HIDEWINDOW);
    ApplySelectorRoundedRegion(g_selector);

    // RefreshWindowList coloca la ventana que estaba en foreground en índice 0.
    // La primera pulsación respeta Shift: Alt+Tab avanza y Alt+Shift+Tab
    // retrocede desde la ventana actualmente activa.
    if (g_groups.size() > 1)
        g_selected = shiftHeld ? static_cast<int>(g_groups.size()) - 1 : 1;
    else
        g_selected = 0;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_animActive = false;
    g_sceneScaleX = 0.94f;
    g_sceneScaleY = 0.94f;
    g_sceneOpacity = 0.05f;
    g_sceneTiltDegrees = 0.0f;
    g_selectorAnimation = SelectorAnimationState::Opening;
    g_selectorAnimationStart = GetTickCount64();

    UpdateSelectorControls();

    // Allows the Alt+Tab window to take foreground when it is shown from
    // the WH_KEYBOARD_LL hook context.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    ShowWindow(g_selector, SW_SHOWNOACTIVATE);
    UpdateWindow(g_selector);
    SetWindowPos(g_selector, HWND_TOPMOST, x, y,
                 g_runtimeSelectorWidth, g_runtimeSelectorHeight,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);

    g_selectorOpening = false;
    // El bitmap permanece cacheado durante la sesión. No se recaptura después
    // de mostrar el selector: ocultar/mostrar la ventana produciría parpadeo.
    // La captura inicial se realizó mientras el selector aún estaba oculto.
    SetTimer(g_selector, kSelectorMotionTimerId,
             GetAnimationTimerInterval(), nullptr);
    // Failsafe solo mientras el selector está abierto (sin polling permanente).
    SetTimer(g_selector, kActivityTimerId, 250, nullptr);
    return true;
}

static void HideSelectorForSession()
{
    if (!g_selector || !IsWindow(g_selector))
        return;

    KillTimer(g_selector, kAnimTimerId);
    KillTimer(g_selector, kTabRepeatTimerId);
    KillTimer(g_selector, kSelectorMotionTimerId);
    KillTimer(g_selector, kActivityTimerId);
    g_tabRepeatStarted = false;
    g_uiTabDown = false;

    // Los thumbnails permanecen registrados mientras el HWND persistente está
    // oculto. En la siguiente sesión UpdateThumbnailSlots reutiliza los que aún
    // apuntan a la misma ventana y reemplaza únicamente los que hayan cambiado.
    ShowWindow(g_selector, SW_HIDE);
    UiSetInteractive(false);
    g_selectorAnimation = SelectorAnimationState::None;
    g_sceneScaleX = 1.0f;
    g_sceneScaleY = 1.0f;
    g_sceneOpacity = 1.0f;
    g_sceneTiltDegrees = 0.0f;
    g_selectionTiltDirection = 0.0f;
    g_cardSnapScale = 1.0f;
    g_cardSnapStartScale = 1.0f;
    g_animActive = false;
    g_animOffset = 0.0f;
    g_animStartOffset = 0.0f;
    g_cardExitActive = false;
    g_cardExitSlot = -1;
    g_cardExitGroupIndex = -1;
    g_cardExitTarget = nullptr;
    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_selectorOpening = false;
    g_lastSelectorActivity = 0;
    g_pendingActivationTarget = nullptr;
    g_selectorOriginWindow = nullptr;

    // El fondo depende del escritorio actual; se libera al terminar la sesión
    // para que la siguiente apertura capture una imagen fresca.
    ReleaseBlurBackground();
}

static void DestroySelector()
{
    // Esta función mantiene el HWND y el renderer vivos entre sesiones. El
    // desmontaje real ocurre únicamente al terminar el UI thread. No se toca
    // ClipCursor ni ningún estado global que Overhaulded no haya creado.
    ReleaseCapture();
    HideSelectorForSession();
}

// Solo UI thread, al terminar el hilo.
static void ShutdownPersistentSelector()
{
    if (g_selector && IsWindow(g_selector))
    {
        KillTimer(g_selector, kAnimTimerId);
        KillTimer(g_selector, kTabRepeatTimerId);
        KillTimer(g_selector, kSelectorMotionTimerId);
        KillTimer(g_selector, kActivityTimerId);
        UnregisterAllThumbnails();
        HWND selector = g_selector;
        g_selector = nullptr;
        DestroyWindow(selector);
    }

    ReleaseBlurBackground();
    ReleaseGroupResources();
    UnloadD2DAndDWrite();
    UnloadDwmFunctions();
    DestroyUiFonts();
    g_groups.clear();
    g_selected = 0;
    g_hoveredSlot = -1;
    g_hoveredCloseButton = false;
    g_tabRepeatStarted = false;
    g_uiTabDown = false;
    g_selectorOpening = false;
    g_lastSelectorActivity = 0;
    g_pendingActivationTarget = nullptr;
    g_selectorOriginWindow = nullptr;
}

static void EmergencyCloseSelector()
{
    if (g_cleanupInProgress)
        return;

    g_cleanupInProgress = true;
    LeaveActiveState(SelectorState::Canceling);
    if (g_selector && IsWindow(g_selector))
        KillTimer(g_selector, kTabRepeatTimerId);
    DestroySelector();
    g_state = SelectorState::Idle;
    g_cleanupInProgress = false;
    CleanupKeyboardState();
}

static void UpdateCarouselAnimation(HWND hwnd)
{
    if (!g_animActive)
    {
        KillTimer(hwnd, kAnimTimerId);
        return;
    }
    ULONGLONG now = GetTickCount64();
    ULONGLONG elapsed = now - g_animStartTime;
    float progress = std::min(1.0f,
        static_cast<float>(elapsed) / AnimationDuration(kNavigationDuration));
    float eased = EaseOutCubic(progress);
    g_animOffset = g_animStartOffset * (1.0f - eased);

    // Snap sutil de la card que llega al centro; no se aplica al contenedor,
    // al fondo ni al visualizer, que permanecen inmóviles.
    float snapT = progress <= 0.64f ? 0.0f :
        std::min(1.0f, (progress - 0.64f) / 0.36f);
    float targetSnapScale = 1.0f + 0.045f * sinf(
        snapT * 3.14159265358979323846f);
    g_cardSnapScale = g_cardSnapStartScale +
        (targetSnapScale - g_cardSnapStartScale) * eased;

    if (progress >= 1.0f)
    {
        g_animActive = false;
        g_animOffset = 0.0f;
        g_animStartOffset = 0.0f;
        g_cardSnapScale = 1.0f;
        g_cardSnapStartScale = 1.0f;
        KillTimer(hwnd, kAnimTimerId);
    }
    UpdateSelectorControls();
}

static void StartSlide(int steps)
{
    if (steps == 0 || g_groups.empty())
        return;
    if (g_selectorAnimation != SelectorAnimationState::Closing &&
        g_selectorAnimation != SelectorAnimationState::CardExit)
    {
        g_selectionStartScaleX = 1.0f;
        g_selectionStartScaleY = 1.0f;
        g_selectionStartOpacity = 1.0f;
        g_selectionStartTiltDegrees = 0.0f;
        g_sceneScaleX = 1.0f;
        g_sceneScaleY = 1.0f;
        g_sceneOpacity = 1.0f;
        g_sceneTiltDegrees = 0.0f;
        g_selectorAnimation = SelectorAnimationState::SelectionChange;
        g_selectorAnimationStart = GetTickCount64();
        if (g_selector && IsWindow(g_selector))
            SetTimer(g_selector, kSelectorMotionTimerId, GetAnimationTimerInterval(), nullptr);
    }
    g_selectionTiltDirection = steps > 0 ? -1.0f : 1.0f;
    g_cardSnapStartScale = g_cardSnapScale;

    int count = static_cast<int>(g_groups.size());
    g_selected = (g_selected + steps) % count;
    while (g_selected < 0)
        g_selected += count;

    g_animStartOffset = g_animOffset + static_cast<float>(steps);
    if (g_animStartOffset > 2.0f) g_animStartOffset = 2.0f;
    if (g_animStartOffset < -2.0f) g_animStartOffset = -2.0f;

    g_animOffset = g_animStartOffset;
    g_animStartTime = GetTickCount64();
    g_animActive = true;

    if (g_selector && IsWindow(g_selector))
        SetTimer(g_selector, kSelectorMotionTimerId, GetAnimationTimerInterval(), nullptr);

    UpdateSelectorControls();
}

static void SelectNextSmooth(int steps)
{
    StartSlide(steps);
}

static void SelectPreviousSmooth(int steps)
{
    StartSlide(-steps);
}

static void SelectNext()
{
    SelectNextSmooth(1);
}

static void SelectPrevious()
{
    SelectPreviousSmooth(1);
}

static HWND GetNextGroupWindow(AppGroup& group)
{
    if (group.windows.empty())
        return nullptr;

    size_t cursor = group.windows.size();
    for (size_t i = 0; i < g_groupWindowCursors.size(); ++i)
    {
        if (g_groupWindowCursors[i].first == group.processId)
        {
            cursor = g_groupWindowCursors[i].second % group.windows.size();
            break;
        }
    }

    if (cursor == group.windows.size())
    {
        for (size_t i = 0; i < group.windows.size(); ++i)
        {
            if (group.windows[i] == group.representativeWindow)
            {
                cursor = i;
                break;
            }
        }
        if (cursor == group.windows.size())
            cursor = 0;
    }

    HWND target = nullptr;
    for (size_t attempt = 0; attempt < group.windows.size(); ++attempt)
    {
        size_t index = (cursor + attempt) % group.windows.size();
        if (group.windows[index] && IsWindow(group.windows[index]))
        {
            target = group.windows[index];
            cursor = (index + 1) % group.windows.size();
            break;
        }
    }

    bool stored = false;
    for (size_t i = 0; i < g_groupWindowCursors.size(); ++i)
    {
        if (g_groupWindowCursors[i].first == group.processId)
        {
            g_groupWindowCursors[i].second = cursor;
            stored = true;
            break;
        }
    }
    if (!stored)
        g_groupWindowCursors.push_back({ group.processId, cursor });
    return target;
}

// Solicita la activación al input thread sin bloquear el UI thread.
static void ActivateWindow(HWND target)
{
    if (!target || !IsWindow(target))
        return;

    DWORD inputThreadId = g_inputThreadId;
    if (inputThreadId)
        PostThreadMessageW(inputThreadId, WM_IN_ACTIVATE,
                           reinterpret_cast<WPARAM>(target), 0);
}

// Esta función solo se ejecuta desde InputThreadProc.
static void ActivateWindowOnInputThread(HWND target)
{
    if (!target || !IsWindow(target))
        return;

    if (IsIconic(target))
        ShowWindowAsync(target, SW_RESTORE);

    SetForegroundWindow(target);
}

static void ConfirmSelection()
{
    if (!IsSelectorActuallyActive() || g_cleanupInProgress)
        return;

    LeaveActiveState(SelectorState::Confirming);
    HWND target = nullptr;
    if (g_selected >= 0 && g_selected < static_cast<int>(g_groups.size()))
    {
        AppGroup& group = g_groups[g_selected];
        target = GetNextGroupWindow(group);
    }

    if (target && target == g_selectorOriginWindow)
    {
        LeaveActiveState(SelectorState::Canceling);
        StartSelectorClose(nullptr);
        return;
    }

    // Confirmar una ventana no es una cancelación: no ejecutar el CRT.
    // El selector se limpia inmediatamente y la ventana destino se activa.
    g_cleanupInProgress = true;
    DestroySelector();
    g_state = SelectorState::Idle;
    g_cleanupInProgress = false;
    CleanupKeyboardState();
    ActivateWindow(target);
}

static void CancelSelection()
{
    if ((!IsSelectorActuallyActive() && !g_selector) || g_cleanupInProgress)
        return;

    LeaveActiveState(SelectorState::Canceling);
    StartSelectorClose(nullptr);
}

// COMANDOS EN EL UI THREAD

static bool UiCommandMatchesSession(WPARAM sessionId)
{
    return static_cast<LONG>(sessionId) == g_uiSessionId && IsSelectorActive();
}

static void UiOpen(LONG sessionId, bool shiftHeld)
{
    g_uiSessionId = sessionId;
    g_uiTabDown = false;
    g_state = SelectorState::SelectorActive;

    if (!CreateSelector(shiftHeld))
    {
        // Nada que mostrar (o precarga no disponible): terminar la sesión.
        g_state = SelectorState::Idle;
        UiEndSession();
        return;
    }

    UiSetInteractive(true);
    g_uiTabDown = true;
    g_tabRepeatStarted = false;
    SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
}

static void UiHandleMouseMove()
{
    InterlockedExchange(&g_mouseMovePending, 0);
    LONG64 packed = InterlockedCompareExchange64(&g_lastMousePt, 0, 0);
    POINT pt = {};
    pt.x = static_cast<LONG>(static_cast<ULONG>(static_cast<ULONG64>(packed) & 0xFFFFFFFFULL));
    pt.y = static_cast<LONG>(static_cast<ULONG>((static_cast<ULONG64>(packed) >> 32) & 0xFFFFFFFFULL));

    if (!IsSelectorActive() || !g_selector || !IsWindow(g_selector))
        return;
    ScreenToClient(g_selector, &pt);
    UpdateHoveredSlot(pt);
    MarkSelectorActivity();
}

static void UiHandleMouseButton(UINT message, POINT screenPt)
{
    if (!IsSelectorActive() || !g_selector || !IsWindow(g_selector))
        return;
    MarkSelectorActivity();

    RECT selRect = {};
    GetWindowRect(g_selector, &selRect);

    if (!PtInRect(&selRect, screenPt))
    {
        g_hoveredSlot = -1;
        g_hoveredCloseButton = false;
        CancelSelection();
        return;
    }

    if (message == WM_LBUTTONDOWN)
    {
        POINT clientPt = screenPt;
        ScreenToClient(g_selector, &clientPt);
        int clickedSlot = GetSlotAtPoint(clientPt);
        if (clickedSlot >= 0)
        {
            RECT closeRect = GetCloseHoverButtonRect(clickedSlot);
            if (g_hoveredSlot == clickedSlot && g_hoveredCloseButton &&
                PtInRect(&closeRect, clientPt))
            {
                CloseWindowForSlot(clickedSlot);
            }
            else if (clickedSlot == GetCarouselCenterSlot())
            {
                ConfirmSelection();
            }
            else
            {
                StartSlide(clickedSlot - GetCarouselCenterSlot());
            }
        }
    }
}

static void UiHandleSettingsChanged()
{
    if (g_selector && IsWindow(g_selector))
    {
        if (g_selectorAnimation != SelectorAnimationState::None &&
            g_selectorAnimation != SelectorAnimationState::Open)
            SetTimer(g_selector, kSelectorMotionTimerId,
                     GetAnimationTimerInterval(), nullptr);
        if (g_animActive)
            SetTimer(g_selector, kSelectorMotionTimerId,
                     GetAnimationTimerInterval(), nullptr);
        // A visible-card count change can invalidate existing DWM slot mapping.
        UnregisterAllThumbnails();
        UpdateSelectorControls();
    }
}

static void HandleUiCommand(const MSG& msg)
{
    switch (msg.message)
    {
    case WM_UI_OPEN:
        UiOpen(static_cast<LONG>(msg.wParam), msg.lParam != 0);
        break;

    case WM_UI_TAB_DOWN:
        if (!UiCommandMatchesSession(msg.wParam))
            break;
        MarkSelectorActivity();
        if (msg.lParam != 0) SelectPrevious();
        else SelectNext();
        g_uiTabDown = true;
        g_tabRepeatStarted = false;
        SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
        break;

    case WM_UI_TAB_UP:
        if (static_cast<LONG>(msg.wParam) != g_uiSessionId)
            break;
        g_uiTabDown = false;
        g_tabRepeatStarted = false;
        if (g_selector && IsWindow(g_selector))
            KillTimer(g_selector, kTabRepeatTimerId);
        break;

    case WM_UI_NAVIGATE:
        if (!UiCommandMatchesSession(msg.wParam))
            break;
        MarkSelectorActivity();
        if (msg.lParam < 0) SelectPrevious();
        else SelectNext();
        break;

    case WM_UI_CONFIRM:
        if (static_cast<LONG>(msg.wParam) == g_uiSessionId)
            ConfirmSelection();
        break;

    case WM_UI_CANCEL:
        if (static_cast<LONG>(msg.wParam) == g_uiSessionId)
            CancelSelection();
        break;

    case WM_UI_MOUSE_MOVE:
        UiHandleMouseMove();
        break;

    case WM_UI_MOUSE_BUTTON:
    {
        POINT pt = {};
        pt.x = static_cast<short>(LOWORD(msg.lParam));
        pt.y = static_cast<short>(HIWORD(msg.lParam));
        UiHandleMouseButton(static_cast<UINT>(msg.wParam), pt);
        break;
    }

    case WM_UI_MOUSE_WHEEL:
    {
        if (!IsSelectorActive())
            break;
        MarkSelectorActivity();
        short delta = static_cast<short>(LOWORD(msg.wParam));
        if (delta > 0)
            SelectNextSmooth(1);
        else if (delta < 0)
            SelectPreviousSmooth(1);
        break;
    }

    case WM_UI_SETTINGS:
        UiHandleSettingsChanged();
        break;
    }
}

// HOOKS (input thread). Callbacks mínimos: estado local/atómico + PostThreadMessage.

static bool InputSessionActive()
{
    return g_inputSessionId != 0 &&
           InterlockedCompareExchange(&g_sessionActive, 0, 0) == g_inputSessionId;
}

// Si el UI cerró la sesión (clic, timeout, ventana inválida), reconciliar.
static void SyncInputSession()
{
    if (g_inputSessionId != 0 && !InputSessionActive())
        ClearSelectorKeyboardSession();
}

static void InputEndSession(UINT command)
{
    LONG id = g_inputSessionId;
    if (id == 0)
        return;
    InterlockedCompareExchange(&g_sessionActive, 0, id);
    ClearSelectorKeyboardSession();
    PostUiCommand(command, static_cast<WPARAM>(id), 0);
}

static bool IsKeyDownMessage(WPARAM message)
{
    return message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
}

static bool IsKeyUpMessage(WPARAM message)
{
    return message == WM_KEYUP || message == WM_SYSKEYUP;
}

static bool SessionModifierReleased()
{
    if (g_sessionModifier == ModifierSession::LeftAlt)
        return !g_altLeftDown;

    if (g_sessionModifier == ModifierSession::RightAlt)
        return !g_altRightDown;

    if (g_sessionModifier == ModifierSession::AltGr)
        return !g_altLeftDown && !g_altRightDown &&
               !g_ctrlLeftDown && !g_ctrlRightDown;

    return false;
}

static void SendAltMenuMaskIfNeeded()
{
    if (!g_altMenuMaskPending)
        return;

    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = 0xE8;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0xE8;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
    g_altMenuMaskPending = false;
}

// Al soltar modificadores: confirmar si termina la sesión y desarmar el bloqueo
// de Ctrl+Alt+Flecha cuando AltGr ya no está pulsado.
static void OnModifierReleased()
{
    if (InputSessionActive() && SessionModifierReleased())
        InputEndSession(WM_UI_CONFIRM);
    g_altGrActive = IsAltGrPhysicallyDown();
    if (!InputSessionActive() && !g_altGrActive)
        g_altGrTaskSwitcherArmed = false;
}

static LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode != HC_ACTION || !lParam)
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    // En layouts con AltGr el sistema genera un Ctrl izquierdo "falso" (scancode
    // 0x21D). Se procesa aunque venga marcado como inyectado para no perder el
    // estado de AltGr; el resto de teclas inyectadas se ignoran.
    const bool fakeAltGrCtrl = key->vkCode == VK_LCONTROL && key->scanCode == 0x21D;
    if ((key->flags & LLKHF_INJECTED) != 0 && !fakeAltGrCtrl)
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);

    SyncInputSession();

    bool up = IsKeyUpMessage(wParam) || ((key->flags & LLKHF_UP) != 0);
    bool down = !up && IsKeyDownMessage(wParam);
    DWORD vk = key->vkCode;

    bool isLeftAlt = (vk == VK_LMENU) || (vk == VK_MENU && (key->flags & LLKHF_EXTENDED) == 0);
    bool isRightAlt = (vk == VK_RMENU) || (vk == VK_MENU && (key->flags & LLKHF_EXTENDED) != 0);
    bool isLeftCtrl = (vk == VK_LCONTROL) || (vk == VK_CONTROL && (key->flags & LLKHF_EXTENDED) == 0);
    bool isRightCtrl = (vk == VK_RCONTROL) || (vk == VK_CONTROL && (key->flags & LLKHF_EXTENDED) != 0);

    if (isLeftAlt)
    {
        if (down)
            g_altLeftDown = true;
        if (up)
        {
            g_altLeftDown = false;
            SendAltMenuMaskIfNeeded();
            OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isRightAlt)
    {
        if (down)
            g_altRightDown = true;
        if (up)
        {
            g_altRightDown = false;
            SendAltMenuMaskIfNeeded();
            OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isRightCtrl)
    {
        if (down)
            g_ctrlRightDown = true;
        if (up)
        {
            g_ctrlRightDown = false;
            OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isLeftCtrl)
    {
        if (down)
            g_ctrlLeftDown = true;
        if (up)
        {
            g_ctrlLeftDown = false;
            OnModifierReleased();
        }
        else
            g_altGrActive = IsAltGrPhysicallyDown();
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    const bool sessionActive = InputSessionActive();

    if (vk == VK_ESCAPE && down && sessionActive)
    {
        InputEndSession(WM_UI_CANCEL);
        return 1;
    }

    // AltGr se representa como Ctrl + Alt derecho. Cuando AltGr+Tab ya armó
    // nuestro selector, nunca dejamos que Ctrl+Alt+Flecha alcance al sistema
    // y active, por ejemplo, la rotación de pantalla. El bloqueo está limitado
    // a esta sesión; Ctrl+Alt+Flecha normal fuera del selector sigue intacto.
    if (g_altGrTaskSwitcherArmed &&
        (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN))
    {
        if (sessionActive && down)
        {
            if (vk == VK_LEFT)
                PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                              static_cast<LPARAM>(-1));
            else if (vk == VK_RIGHT)
                PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                              static_cast<LPARAM>(1));
        }
        // Up/Down and any key-up are simply consumed so Ctrl+Alt+Arrow
        // never reaches the Windows display-rotation shortcut.
        return 1;
    }

    if (sessionActive && down && vk == VK_LEFT)
    {
        PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                      static_cast<LPARAM>(-1));
        return 1;
    }

    if (sessionActive && down && vk == VK_RIGHT)
    {
        PostUiCommand(WM_UI_NAVIGATE, static_cast<WPARAM>(g_inputSessionId),
                      static_cast<LPARAM>(1));
        return 1;
    }

    if (vk == VK_RETURN && sessionActive)
    {
        if (down)
            InputEndSession(WM_UI_CONFIRM);
        return 1;
    }

    if (vk == VK_TAB)
    {
        if (down)
        {
            if (!sessionActive)
            {
                // Alt izquierdo, Alt derecho y AltGr se distinguen a partir del
                // estado rastreado por eventos (más GetAsyncKeyState como
                // respaldo para modificadores pulsados antes del hook). Ya no
                // se usa un flag AltGr "pegajoso": AltGr+otras teclas no abre
                // el selector ni deja estado residual.
                bool leftAlt = g_altLeftDown ||
                               ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0);
                bool rightAlt = g_altRightDown ||
                                ((GetAsyncKeyState(VK_RMENU) & 0x8000) != 0);
                bool ctrlHeld = g_ctrlLeftDown || g_ctrlRightDown ||
                                ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0);
                bool altGrHeld = rightAlt && ctrlHeld;

                if (leftAlt || rightAlt)
                {
                    LONG id = InterlockedIncrement(&g_nextSessionId);
                    if (id == 0)
                        id = InterlockedIncrement(&g_nextSessionId);
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

                    g_inputSessionId = id;
                    InterlockedExchange(&g_sessionActive, id);
                    g_sessionModifier = altGrHeld
                        ? ModifierSession::AltGr
                        : (leftAlt ? ModifierSession::LeftAlt
                                   : ModifierSession::RightAlt);
                    g_tabDown = true;
                    g_tabSuppressed = true;
                    g_altGrActive = altGrHeld;
                    g_altGrTaskSwitcherArmed = altGrHeld;

                    // El UI thread abre el selector ya precargado. El hook no
                    // enumera ventanas ni toca D2D/DWM: solo publica el comando.
                    if (PostUiCommand(WM_UI_OPEN, static_cast<WPARAM>(id),
                                      shift ? 1 : 0))
                    {
                        g_altMenuMaskPending = !altGrHeld;
                        return 1;
                    }

                    // Sin UI disponible no se bloquea la pulsación: Windows
                    // conserva su comportamiento normal.
                    InterlockedCompareExchange(&g_sessionActive, 0, id);
                    ClearSelectorKeyboardSession();
                    g_altGrTaskSwitcherArmed = false;
                    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
                }
            }
            else
            {
                if (!g_tabDown)
                {
                    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    g_tabDown = true;
                    PostUiCommand(WM_UI_TAB_DOWN, static_cast<WPARAM>(g_inputSessionId),
                                  shift ? 1 : 0);
                }
                return 1;
            }
        }
        else if (up)
        {
            g_tabDown = false;
            if (sessionActive)
                PostUiCommand(WM_UI_TAB_UP, static_cast<WPARAM>(g_inputSessionId), 0);
            if (sessionActive || g_tabSuppressed)
            {
                g_tabSuppressed = false;
                return 1;
            }
        }
    }

    if (sessionActive)
    {
        if (vk == VK_LWIN || vk == VK_RWIN || vk == VK_APPS)
            return 1;

        if (vk != VK_SHIFT && vk != VK_LSHIFT && vk != VK_RSHIFT &&
            !isLeftAlt && !isRightAlt && !isLeftCtrl && !isRightCtrl)
            return 1;
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Solo interviene mientras el UI thread declara el selector interactivo.
    if (nCode != HC_ACTION || !lParam ||
        InterlockedCompareExchange(&g_uiSelectorOpen, 0, 0) == 0)
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);

    const MSLLHOOKSTRUCT* mouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

    if (wParam == WM_MOUSEMOVE)
    {
        LONG64 packed = static_cast<LONG64>(
            (static_cast<ULONG64>(static_cast<ULONG>(mouse->pt.y)) << 32) |
            static_cast<ULONG64>(static_cast<ULONG>(mouse->pt.x)));
        InterlockedExchange64(&g_lastMousePt, packed);
        // Coalescencia: como máximo un WM_UI_MOUSE_MOVE pendiente. El hook
        // observa el movimiento pero no lo suprime.
        if (InterlockedExchange(&g_mouseMovePending, 1) == 0)
        {
            if (!PostUiCommand(WM_UI_MOUSE_MOVE, 0, 0))
                InterlockedExchange(&g_mouseMovePending, 0);
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
        wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN ||
        wParam == WM_NCRBUTTONDOWN || wParam == WM_NCMBUTTONDOWN)
    {
        LPARAM packedPt = MAKELPARAM(static_cast<WORD>(static_cast<short>(mouse->pt.x)),
                                     static_cast<WORD>(static_cast<short>(mouse->pt.y)));
        if (!PostUiCommand(WM_UI_MOUSE_BUTTON, wParam, packedPt))
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        return 1;
    }

    if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP ||
        wParam == WM_MBUTTONUP || wParam == WM_NCLBUTTONUP ||
        wParam == WM_NCRBUTTONUP || wParam == WM_NCMBUTTONUP)
    {
        return 1;
    }

    if (wParam == WM_MOUSEWHEEL)
    {
        short delta = static_cast<short>(HIWORD(mouse->mouseData));
        if (!PostUiCommand(WM_UI_MOUSE_WHEEL,
                           static_cast<WPARAM>(static_cast<USHORT>(delta)), 0))
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        return 1;
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// HILOS

static DWORD WINAPI InputThreadProc(LPVOID)
{
    SetThreadPerMonitorAwareV2();

    // Crear la cola de mensajes antes de publicar la disponibilidad.
    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    InputResetKeyboardState();

    bool ok = false;
    HWND control = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0,
                                   HWND_MESSAGE, nullptr, g_hModule, nullptr);
    if (control)
    {
        g_inputControlWnd = control;
        g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook, g_hModule, 0);
        ok = g_keyboardHook != nullptr;
    }
    if (!ok)
        Wh_Log(L"Input thread initialization failed: %lu", GetLastError());

    InterlockedExchange(&g_inputInitOk, ok ? 1 : 0);
    SetEvent(g_inputReadyEvent);

    if (ok)
    {
        while (GetMessageW(&message, nullptr, 0, 0) > 0)
        {
            if (message.message == WM_IN_ACTIVATE)
            {
                ActivateWindowOnInputThread(
                    reinterpret_cast<HWND>(message.wParam));
                continue;
            }
            if (message.message == WM_IN_MOUSEHOOK)
            {
                if (message.wParam != 0)
                {
                    if (!g_mouseHook)
                        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook,
                                                        g_hModule, 0);
                }
                else if (g_mouseHook)
                {
                    UnhookWindowsHookEx(g_mouseHook);
                    g_mouseHook = nullptr;
                }
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    // El propio hilo retira sus hooks y destruye su ventana.
    if (g_keyboardHook)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    if (g_mouseHook)
    {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    g_inputControlWnd = nullptr;
    if (control)
        DestroyWindow(control);
    return 0;
}

static bool InitializeUiThreadState()
{
    if (!RegisterSelectorClasses())
        return false;

    // Escala del monitor principal para que el precalentado (D2D/DWrite/fuentes)
    // ya use la escala correcta en el caso habitual.
    POINT origin = { 0, 0 };
    UpdateUIScaleForMonitor(MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY));

    if (!InitializePersistentSelector())
        return false;

    RegisterUserWindow(GetForegroundWindow(), true);

    // MRU por evento, sin polling. OUTOFCONTEXT: se entrega en este hilo.
    g_winEventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                     nullptr, ForegroundWinEventProc, 0, 0,
                                     WINEVENT_OUTOFCONTEXT);
    if (!g_winEventHook)
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
    return true;
}

static void ShutdownUiThreadState()
{
    if (g_winEventHook)
    {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    if (g_foregroundRetryTimer)
    {
        KillTimer(nullptr, g_foregroundRetryTimer);
        g_foregroundRetryTimer = 0;
    }
    g_foregroundRetryWindow = nullptr;

    if (g_selector || g_state != SelectorState::Idle)
        EmergencyCloseSelector();
    else
        CleanupKeyboardState();

    // El hilo propietario destruye la ventana y libera D2D/DWrite/GDI/DWM.
    ShutdownPersistentSelector();
    UnregisterSelectorClasses();

    g_userWindows.clear();
    g_groups.clear();
    g_selected = 0;
    g_state = SelectorState::Idle;

    g_getDpiForMonitor = nullptr;
    if (g_shcoreApi)
    {
        FreeLibrary(g_shcoreApi);
        g_shcoreApi = nullptr;
    }
}

static DWORD WINAPI UiThreadProc(LPVOID)
{
    // Debe hacerse antes de crear cualquier ventana de este hilo.
    SetThreadPerMonitorAwareV2();

    MSG message = {};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    bool ok = InitializeUiThreadState();
    InterlockedExchange(&g_uiInitOk, ok ? 1 : 0);
    SetEvent(g_uiReadyEvent);

    if (ok)
    {
        while (GetMessageW(&message, nullptr, 0, 0) > 0)
        {
            if (message.hwnd == nullptr &&
                message.message >= WM_UI_OPEN && message.message <= WM_UI_SETTINGS)
            {
                HandleUiCommand(message);
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    ShutdownUiThreadState();
    return 0;
}

// Arranca un hilo y espera sin timeout a que esté listo o haya terminado.
// El evento no se cierra mientras el hilo pueda seguir usándolo.
static bool StartWorkerThread(LPTHREAD_START_ROUTINE proc, HANDLE* outThread,
                              DWORD* outThreadId, HANDLE* outEvent,
                              volatile LONG* okFlag)
{
    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent)
        return false;
    *outEvent = readyEvent;

    DWORD threadId = 0;
    HANDLE thread = CreateThread(nullptr, 0, proc, nullptr, 0, &threadId);
    if (!thread)
    {
        CloseHandle(readyEvent);
        *outEvent = nullptr;
        return false;
    }

    HANDLE waits[2] = { readyEvent, thread };
    DWORD result = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    bool ok = (result == WAIT_OBJECT_0) &&
              InterlockedCompareExchange(okFlag, 0, 0) == 1;
    if (!ok)
    {
        // El hilo ya terminó o está terminando por sí mismo: esperar su fin
        // antes de cerrar handles.
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
        CloseHandle(readyEvent);
        *outEvent = nullptr;
        return false;
    }

    *outThread = thread;
    *outThreadId = threadId;
    return true;
}

// TOOL MOD

BOOL WhTool_ModInit()
{
    LoadTaskSwitcherSettings();

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&SelectorWndProc), &g_hModule);
    if (!g_hModule)
        return FALSE;

    InterlockedExchange(&g_shutdownRequested, 0);
    InterlockedExchange(&g_sessionActive, 0);
    InterlockedExchange(&g_uiSelectorOpen, 0);
    InterlockedExchange(&g_mouseMovePending, 0);
    g_state = SelectorState::Idle;

    // 1) UI thread: crea el selector y precalienta el renderer.
    DWORD uiThreadId = 0;
    if (!StartWorkerThread(UiThreadProc, &g_uiThread, &uiThreadId,
                           &g_uiReadyEvent, &g_uiInitOk))
        return FALSE;
    g_uiThreadId = uiThreadId;

    // 2) Input thread: solo hooks. Se publica cuando el selector ya existe.
    if (!StartWorkerThread(InputThreadProc, &g_inputThread, &g_inputThreadId,
                           &g_inputReadyEvent, &g_inputInitOk))
    {
        InterlockedExchange(&g_shutdownRequested, 1);
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
        g_uiThreadId = 0;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    LoadTaskSwitcherSettings();
    // Los timers pertenecen al UI thread; se le pide que los reprograme.
    PostUiCommand(WM_UI_SETTINGS, 0, 0);
}

void WhTool_ModUninit()
{
    InterlockedExchange(&g_shutdownRequested, 1);

    // 1) Input thread: retira sus hooks, destruye su ventana y termina.
    if (g_inputThread)
    {
        PostThreadMessageW(g_inputThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_inputThread, INFINITE);
        CloseHandle(g_inputThread);
        g_inputThread = nullptr;
    }

    // 2) UI thread: retira el WinEvent hook, destruye la ventana y libera
    //    D2D/DWrite/GDI/DWM antes de terminar.
    if (g_uiThread)
    {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    // Ningún hilo puede usar ya los eventos: cerrarlos ahora es seguro.
    if (g_inputReadyEvent)
    {
        CloseHandle(g_inputReadyEvent);
        g_inputReadyEvent = nullptr;
    }
    if (g_uiReadyEvent)
    {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
    }
    g_inputThreadId = 0;
    g_uiThreadId = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
