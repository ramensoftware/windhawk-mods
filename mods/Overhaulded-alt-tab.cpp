// ==WindhawkMod==
// @id              overhaulded-alt-tab
// @name            OverhauldedWin Alt+Tab
// @description     Replaces the boring Windows Alt+Tab with a modern and elegant window switcher.
// @version         1.0.0
// @author          IMiloDev
// @github          IMiloDev
// @homepage        https://github.com/IMiloDev/OverhauldedWin-Task-Switcher
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
 * # Overhaulded Task Switcher
 *
 * A modern, fluid and highly visual replacement for the Windows Alt+Tab experience 🫩.
 *
 * `Ofc, made as a practice of cpp. Hope you all enjoy this as me developing this.`
 *
 * ## Screenshot
 *
 * # ![OverhauldedWin-Task-Switcher](https://raw.githubusercontent.com/IMiloDev/OverhauldedWin/main/assets/icons/TaskManager.png)
 *
 * ## Features
 *
 * - Modern horizontal task switcher interface
 * - App grouping by application/process
 * - Real DWM window previews
 * - Dynamic Obsidian visual system
 * - CPU-based desktop blur
 * - Fluid Pop opening animation
 * - Smooth horizontal navigation
 * - Hover interactions and window closing
 * - Resolution-aware UI scaling
 * - Configurable animation FPS
 * - AltGr + Tab support
 * - Alt + Tab support
 * - Lightweight native C++ implementation
 *
 * ## Design
 *
 * Overhaulded focuses on a dark, minimal interface inspired by modern desktop UI design while keeping the selector feeling native to Windows.
 *
 * The visual system uses a Black Obsidian surface with subtle content-based illumination, rounded cards, restrained shadows and smooth transitions.
 *
 * ## Requirements
 *
 * - Windows (11 Only)
 *
 * ## License
 *
 * This project is licensed under the **MIT License**.
 *
 * See the [LICENSE](LICENSE) file for the complete license text.
 *
 * `Current ver: 1.0.0`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- AnimationFps: "90"
  $name: Animation FPS
  $description: Maximum animation update frequency. Durations remain time-based.
  $options:
  - "60": 60 FPS
  - "90": 90 FPS
  - "120": 120 FPS
  - "240": 240 FPS
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

// CONFIGURACIÓN

static const wchar_t kWindowClassName[] = L"OverhauldedAltTabSelector";

static const int kSelectorWidth = 1220;
static const int kSelectorHeight = 430;
static const int kCarouselSlots = 5;
static const int kCounterWidth = 140;
static const int kCounterHeight = 28;
static const int kCounterTop = 378;
// Opacidad común de las superficies Black Obsidian; los textos y thumbnails no se alteran.
static const float kCardSurfaceOpacity = 0.75f;
static const UINT_PTR kActivityTimerId = 77;
static const UINT_PTR kAnimTimerId = 88;
static const UINT_PTR kTabRepeatTimerId = 89;
static const UINT_PTR kSelectorMotionTimerId = 90;
static const int kAnimDurationMs = 140;
static const int kDefaultAnimationFps = 90;
static int g_animationFps = kDefaultAnimationFps;

static bool IsSupportedAnimationFps(int value)
{
    return value == 60 || value == 90 || value == 120 || value == 240;
}

static void LoadAnimationSettings()
{
    PCWSTR storedValue = Wh_GetStringSetting(L"AnimationFps");
    int value = kDefaultAnimationFps;
    if (storedValue)
    {
        if (wcscmp(storedValue, L"60") == 0) value = 60;
        else if (wcscmp(storedValue, L"90") == 0) value = 90;
        else if (wcscmp(storedValue, L"120") == 0) value = 120;
        else if (wcscmp(storedValue, L"240") == 0) value = 240;
        Wh_FreeStringSetting(storedValue);
    }
    g_animationFps = IsSupportedAnimationFps(value) ? value : kDefaultAnimationFps;
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

static int ScaleLayoutPx(float value)
{
    return static_cast<int>(roundf(value * g_uiScale.value));
}

static void UpdateUIScaleForWorkArea(const RECT& work)
{
    g_uiScale.screenWidth = std::max(1, static_cast<int>(work.right - work.left));
    g_uiScale.screenHeight = std::max(1, static_cast<int>(work.bottom - work.top));
    g_uiScale.dpiScale = 1.0f;
    float scaleX = static_cast<float>(g_uiScale.screenWidth) / kReferenceScreenWidth;
    float scaleY = static_cast<float>(g_uiScale.screenHeight) / kReferenceScreenHeight;
    g_uiScale.value = std::max(kMinimumUiScale, std::min(1.0f, std::min(scaleX, scaleY)));
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

struct AccentPolicyLocal
{
    DWORD accentState;
    DWORD accentFlags;
    DWORD gradientColor;
    DWORD animationId;
};

struct WindowCompositionAttributeDataLocal
{
    DWORD attribute;
    void* data;
    SIZE_T sizeOfData;
};

struct MARGINS
{
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
};

typedef BOOL (WINAPI* SetWindowCompositionAttributeFn)(
    HWND, WindowCompositionAttributeDataLocal*);

static const DWORD kDwmWindowCornerPreference = 33;
static const DWORD kDwmNcRenderingPolicy = 2;
static const DWORD kDwmNcRenderingPolicyDisabled = 1;
static const DWORD kDwmRoundRect = 2;
static const DWORD kDwmWaSystemBackdropType = 38;
static const DWORD kDwmSbtNone = 1;
static const DWORD kDwmSbtAuto = 3;
static const DWORD kDwmSbtTabbed = 4;
static const DWORD kDwmSbtTransient = 5;

static const DWORD kWcaAccentPolicy = 19;
static const DWORD kAccentEnableBlurBehind = 3;

static const DWORD kDwmTnpRectDestination = 0x00000001;
static const DWORD kDwmTnpRectSource = 0x00000002;
static const DWORD kDwmTnpOpacity = 0x00000004;
static const DWORD kDwmTnpVisible = 0x00000008;
static const DWORD kDwmTnpSourceClientAreaOnly = 0x00000010;

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

static HHOOK g_keyboardHook = nullptr;
static HHOOK g_mouseHook = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_hookReadyEvent = nullptr;
static volatile LONG g_shutdownRequested = 0;
static volatile LONG g_hookInstalled = 0;

static HWND g_selector = nullptr;
static bool g_classesRegistered = false;

// Colección visual estable: una ranura = un thumbnail DWM reutilizado.
struct ThumbnailSlot
{
    HTHUMBNAIL thumbnail;
    HWND source;
};

static ThumbnailSlot g_thumbnailSlots[kCarouselSlots] = {};
static HMODULE g_dwmApi = nullptr;
static DwmRegisterThumbnailFn g_dwmRegisterThumbnail = nullptr;
static DwmUnregisterThumbnailFn g_dwmUnregisterThumbnail = nullptr;
static DwmUpdateThumbnailPropertiesFn g_dwmUpdateThumbnailProperties = nullptr;
static DwmSetWindowAttributeFn g_dwmSetWindowAttribute = nullptr;
static SetWindowCompositionAttributeFn g_setWindowCompositionAttribute = nullptr;

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

typedef HFONT (WINAPI* CreateFontIndirectWFn)(const LOGFONTW*);
typedef BOOL (WINAPI* DeleteObjectFn)(HGDIOBJ);
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
typedef BOOL (WINAPI* BitBltFn)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef int (WINAPI* GetDIBitsFn)(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);
typedef BOOL (WINAPI* DeleteDCFn)(HDC);

static HMODULE g_gdiApi = nullptr;
static CreateFontIndirectWFn g_createFontIndirectW = nullptr;
static DeleteObjectFn g_deleteObject = nullptr;
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
static BitBltFn g_bitBlt = nullptr;
static DeleteDCFn g_deleteDC = nullptr;

static HFONT g_nameFont = nullptr;
static HFONT g_selectedNameFont = nullptr;
static HFONT g_secondaryFont = nullptr;

static std::vector<AppGroup> g_groups;
static std::vector<UserWindowInfo> g_userWindows;
static int g_selected = 0;
static SelectorState g_state = SelectorState::Idle;
static ModifierSession g_sessionModifier = ModifierSession::None;

// Variables para el blur del fondo
static HDC g_blurDC = nullptr;
static HBITMAP g_blurBitmap = nullptr;
static HBITMAP g_blurOldBitmap = nullptr;
static unsigned char* g_blurPixels = nullptr;
static int g_blurWidth = 0;
static int g_blurHeight = 0;
static bool g_blurBackgroundCreated = false;
static int g_blurCaptureX = 0;
static int g_blurCaptureY = 0;
static ID2D1Bitmap* g_blurD2DBitmap = nullptr;

static bool LoadGdiFunctions();
static HICON GetApplicationIcon(HWND hwnd, const std::wstring& processPath, bool* outOwnIcon);
static void ReleaseGroupResources();
static void CancelSelection();
static void ConfirmSelection();
static void CleanupKeyboardState();
static void EmergencyCloseSelector();
static void StartSelectorClose(HWND target);
static void UpdateSelectorMotion();
static void UpdateCarouselAnimation(HWND hwnd);
static float GetCardExitProgress();
static float GetCardExitOpacity();
static float GetCardExitOffsetY();
static void DestroySelector();
static void ActivateWindow(HWND target);
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

static bool g_altLeftDown = false;
static bool g_altRightDown = false;
static bool g_ctrlLeftDown = false;
static bool g_ctrlRightDown = false;
static bool g_tabDown = false;
static bool g_rightCtrlSeen = false;
static bool g_rightAltSeen = false;
static bool g_altGrActive = false;
static bool g_tabSuppressed = false;

// UTILIDADES DE ESTADO

static bool IsAltGrPhysicallyDown()
{
    return g_altRightDown && (g_ctrlLeftDown || g_ctrlRightDown);
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

static void ResetKeyboardState()
{
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    g_tabDown = false;
    g_rightCtrlSeen = false;
    g_rightAltSeen = false;
    g_altGrActive = false;
    g_tabSuppressed = false;
    g_sessionModifier = ModifierSession::None;
    g_cleanupInProgress = false;
    g_selectorOpening = false;
    g_lastSelectorActivity = 0;
}

static void ClearSelectorKeyboardSession()
{
    g_tabDown = false;
    g_tabSuppressed = false;
    g_altGrActive = false;
    g_rightCtrlSeen = false;
    g_rightAltSeen = false;
    g_sessionModifier = ModifierSession::None;

    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
}

static void CleanupKeyboardState()
{
    if (g_cleanupInProgress)
        return;

    g_cleanupInProgress = true;
    g_tabDown = false;
    g_tabSuppressed = false;
    g_altGrActive = false;
    g_rightCtrlSeen = false;
    g_rightAltSeen = false;
    g_sessionModifier = ModifierSession::None;

    // Solo se reconcilia el estado interno; nunca se sintetizan KeyUp.
    g_altLeftDown = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    g_altRightDown = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    g_ctrlLeftDown = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    g_ctrlRightDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;

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
        *appName = BaseNameWithoutExtension(fullPath);
    return true;
}

enum class WindowClassification
{
    RealApplication,
    AuxiliaryWindow,
    SystemWindow,
    Unknown
};

static WindowClassification ClassifyApplicationWindow(
    HWND hwnd, DWORD processId, const std::wstring& processPath,
    const std::wstring& appName, const std::wstring& windowClass,
    bool isCodeProcess)
{
    (void)processId;
    (void)processPath;

    std::wstring name = LowerAscii(appName);
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

    if (name == L"textinputhost" ||
        name == L"spotifyxboxgamebarweb" ||
        name == L"searchhost" ||
        name == L"startmenuexperiencehost" ||
        name == L"shellexperiencehost" ||
        name == L"runtimebroker" ||
        name == L"lockapp" ||
        name == L"xboxgamebar" || name == L"gamebar")
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

static bool IsRealUserApplicationWindow(HWND hwnd, DWORD* processId,
                                        std::wstring* processPath,
                                        std::wstring* appName)
{
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd))
        return false;
    if (hwnd == g_selector || IsShellWindow(hwnd))
        return false;
    if (GetWindow(hwnd, GW_OWNER) != nullptr)
        return false;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) != 0)
        return false;
    if ((exStyle & WS_EX_NOACTIVATE) != 0)
        return false;

    wchar_t title[512] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title) - 1);
    if (title[0] == L'\0')
        return false;

    wchar_t windowClass[256] = {};
    GetClassNameW(hwnd, windowClass, ARRAYSIZE(windowClass) - 1);

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || pid == 0)
        return false;

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

    if (processId)
        *processId = pid;
    if (processPath)
        *processPath = path;
    if (appName)
        *appName = name;
    return true;
}

static void RegisterUserWindow(HWND hwnd, bool activity)
{
    DWORD processId = 0;
    std::wstring processPath;
    std::wstring appName;
    if (!IsRealUserApplicationWindow(hwnd, &processId, &processPath, &appName))
        return;

    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_userWindows.size(); ++i)
    {
        UserWindowInfo& item = g_userWindows[i];
        if (item.hwnd == hwnd)
        {
            if (activity)
                item.lastActivated = now;
            return;
        }
    }

    UserWindowInfo item = {};
    item.hwnd = hwnd;
    item.processId = processId;
    item.processPath = processPath;
    item.appName = appName;
    item.lastActivated = activity ? now : 0;
    g_userWindows.push_back(item);
}

static void PruneUserWindowRegistry()
{
    for (size_t i = 0; i < g_userWindows.size();)
    {
        HWND hwnd = g_userWindows[i].hwnd;
        if (!hwnd || !IsWindow(hwnd) || hwnd == g_selector)
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

// DWM Y EFECTOS DE FONDO

static bool LoadDwmFunctions()
{
    if (g_dwmRegisterThumbnail && g_dwmUnregisterThumbnail && g_dwmUpdateThumbnailProperties)
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
            pDwmExtendFrameIntoClientArea(hwnd, &margins);
        }

        // Desactivar backdrop nativo (usaremos blur propio)
        DWORD backdrop = kDwmSbtNone;
        g_dwmSetWindowAttribute(hwnd, kDwmWaSystemBackdropType,
                                &backdrop, sizeof(backdrop));

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

static bool LoadD2DAndDWrite()
{
    if (g_d2dFactory && g_d2dDCRenderTarget && g_dwriteFactory)
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
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f, 0.0f, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
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

    if (g_dwriteFactory && !g_dwriteSelectedFormat)
    {
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

    if (g_dwriteCounterFormat) { g_dwriteCounterFormat->Release(); g_dwriteCounterFormat = nullptr; }
    if (g_dwriteCloseFormat) { g_dwriteCloseFormat->Release(); g_dwriteCloseFormat = nullptr; }
    if (g_dwriteNormalFormat) { g_dwriteNormalFormat->Release(); g_dwriteNormalFormat = nullptr; }
    if (g_dwriteSelectedFormat) { g_dwriteSelectedFormat->Release(); g_dwriteSelectedFormat = nullptr; }
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
    if (g_createFontIndirectW && g_deleteObject && g_getStockObject &&
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
    g_bitBlt = reinterpret_cast<BitBltFn>(
        GetProcAddress(g_gdiApi, "BitBlt"));
    g_deleteDC = reinterpret_cast<DeleteDCFn>(
        GetProcAddress(g_gdiApi, "DeleteDC"));

    if (!g_createFontIndirectW || !g_deleteObject || !g_getStockObject ||
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
                const float scale = 0.55f;
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

static void CreateUiFonts()
{
    if (g_nameFont || g_selectedNameFont || g_secondaryFont)
        return;

    g_selectedNameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                          -std::max(10, ScaleLayoutPx(16.0f)), FW_SEMIBOLD);
    g_nameFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                  -std::max(9, ScaleLayoutPx(13.0f)), 500);
    g_secondaryFont = CreateModernFont(L"Segoe UI Variable Text", L"Segoe UI",
                                       -std::max(9, ScaleLayoutPx(13.0f)), 500);
}

static void DestroyUiFonts()
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

    // Box blur separable con ventana deslizante. El buffer es BGRA contiguo.
    const int diameter = radius * 2 + 1;
    std::vector<unsigned char> horizontal(static_cast<size_t>(width) * height * 4);

    for (int y = 0; y < height; ++y)
    {
        int sumB = 0, sumG = 0, sumR = 0, sumA = 0;
        for (int k = -radius; k <= radius; ++k)
        {
            int sx = std::max(0, std::min(width - 1, k));
            const unsigned char* p = pixels + (static_cast<size_t>(y) * width + sx) * 4;
            sumB += p[0]; sumG += p[1]; sumR += p[2]; sumA += p[3];
        }

        for (int x = 0; x < width; ++x)
        {
            unsigned char* d = horizontal.data() + (static_cast<size_t>(y) * width + x) * 4;
            d[0] = static_cast<unsigned char>(sumB / diameter);
            d[1] = static_cast<unsigned char>(sumG / diameter);
            d[2] = static_cast<unsigned char>(sumR / diameter);
            d[3] = static_cast<unsigned char>(sumA / diameter);

            int removeX = std::max(0, x - radius);
            int addX = std::min(width - 1, x + radius + 1);
            const unsigned char* removeP = pixels + (static_cast<size_t>(y) * width + removeX) * 4;
            const unsigned char* addP = pixels + (static_cast<size_t>(y) * width + addX) * 4;
            sumB += addP[0] - removeP[0];
            sumG += addP[1] - removeP[1];
            sumR += addP[2] - removeP[2];
            sumA += addP[3] - removeP[3];
        }
    }

    for (int x = 0; x < width; ++x)
    {
        int sumB = 0, sumG = 0, sumR = 0, sumA = 0;
        for (int k = -radius; k <= radius; ++k)
        {
            int sy = std::max(0, std::min(height - 1, k));
            const unsigned char* p = horizontal.data() + (static_cast<size_t>(sy) * width + x) * 4;
            sumB += p[0]; sumG += p[1]; sumR += p[2]; sumA += p[3];
        }

        for (int y = 0; y < height; ++y)
        {
            unsigned char* d = pixels + (static_cast<size_t>(y) * width + x) * 4;
            d[0] = static_cast<unsigned char>(sumB / diameter);
            d[1] = static_cast<unsigned char>(sumG / diameter);
            d[2] = static_cast<unsigned char>(sumR / diameter);
            d[3] = static_cast<unsigned char>(sumA / diameter);

            int removeY = std::max(0, y - radius);
            int addY = std::min(height - 1, y + radius + 1);
            const unsigned char* removeP = horizontal.data() + (static_cast<size_t>(removeY) * width + x) * 4;
            const unsigned char* addP = horizontal.data() + (static_cast<size_t>(addY) * width + x) * 4;
            sumB += addP[0] - removeP[0];
            sumG += addP[1] - removeP[1];
            sumR += addP[2] - removeP[2];
            sumA += addP[3] - removeP[3];
        }
    }
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

    // El coste del blur se reduce a aproximadamente una novena parte.
    g_blurWidth = std::max(160, captureWidth / 3);
    g_blurHeight = std::max(90, captureHeight / 3);

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
    // Se conserva alpha opaco y la transparencia visual se aplica al dibujar la capa.
    for (int i = 0; i < g_blurWidth * g_blurHeight; ++i)
        g_blurPixels[i * 4 + 3] = 255;

    ApplyBlurToPixels(g_blurPixels, g_blurWidth, g_blurHeight, 3);

    // Tinte gris azulado oscuro sutil; no convierte el fondo en negro opaco.
    for (int i = 0; i < g_blurWidth * g_blurHeight * 4; i += 4)
    {
        g_blurPixels[i + 0] = static_cast<unsigned char>(g_blurPixels[i + 0] * 0.86f);
        g_blurPixels[i + 1] = static_cast<unsigned char>(g_blurPixels[i + 1] * 0.88f);
        g_blurPixels[i + 2] = static_cast<unsigned char>(g_blurPixels[i + 2] * 0.90f);
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

static CardSlotGeometry GetSlotKeyframe(int slot)
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

static inline float LerpFloat(float a, float b, float t)
{
    return a + (b - a) * t;
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
    float dist = fabsf(static_cast<float>(slot - 2) + g_animOffset);
    int gap = ScaleLayoutPx(static_cast<float>((dist < 0.5f) ? 8 : (dist < 1.5f ? 6 : 4)));

    RECT preview = {};
    preview.left = static_cast<int>(roundf(g.left)) + pad;
    preview.right = static_cast<int>(roundf(g.right)) - pad;
    preview.top = static_cast<int>(roundf(g.top)) + pad + hHeight + gap;
    preview.bottom = static_cast<int>(roundf(g.bottom)) - pad;
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
    rc.right = card.right - 5;
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

    int index = g_selected + (slot - 2);
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
    RECT rcCenter = GetSlotCardRect(2);
    if (PtInRect(&rcCenter, ptClient))
        return 2;

    int order[] = { 1, 3, 0, 4 };
    for (int i = 0; i < 4; ++i)
    {
        int slot = order[i];
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
    if (slot < 0 || slot >= kCarouselSlots)
        return;
    if (g_thumbnailSlots[slot].thumbnail && g_dwmUnregisterThumbnail)
        g_dwmUnregisterThumbnail(g_thumbnailSlots[slot].thumbnail);
    g_thumbnailSlots[slot].thumbnail = nullptr;
    g_thumbnailSlots[slot].source = nullptr;
}

static void UnregisterAllThumbnails()
{
    for (int i = 0; i < kCarouselSlots; ++i)
        UnregisterThumbnailSlot(i);
}

static void UpdateThumbnailProperties(int slot, const RECT& area, HWND source)
{
    if (slot < 0 || slot >= kCarouselSlots ||
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

    for (int slot = 0; slot < kCarouselSlots; ++slot)
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
    g_selectionStartScaleX = 1.0f;
    g_selectionStartScaleY = 1.0f;
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
        static_cast<float>(GetTickCount64() - g_cardExitStart) / 360.0f);
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
            if (target && IsWindow(target))
                PostMessageW(target, WM_CLOSE, 0, 0);
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
                g_state = SelectorState::Canceling;
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
        const float duration = 240.0f;
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
        const float duration = 110.0f;
        float t = std::min(1.0f, static_cast<float>(elapsed) / duration);
        float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
        g_sceneScaleX = g_selectionStartScaleX +
                        (1.0f - g_selectionStartScaleX) * ease;
        g_sceneScaleY = g_selectionStartScaleY +
                        (1.0f - g_selectionStartScaleY) * ease;
        g_sceneOpacity = g_selectionStartOpacity +
                         (1.0f - g_selectionStartOpacity) * ease;
        if (t >= 1.0f)
        {
            g_sceneScaleX = 1.0f;
            g_sceneScaleY = 1.0f;
            g_selectorAnimation = SelectorAnimationState::Open;
            KillTimer(g_selector, kSelectorMotionTimerId);
        }
    }
    else if (g_selectorAnimation == SelectorAnimationState::Closing)
    {
        const float duration = 420.0f;
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

static void ClearClientToGlassKey(HDC hdc, const RECT& clientRect)
{
    // Eliminado el relleno negro para permitir transparencia DWM
    // DWM composition ahora maneja el fondo con blur ligero
    (void)hdc;
    (void)clientRect;
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
                                         selected ? 0.080f : 0.065f);
    D2D1_COLOR_F center = AddMediaAccent(base, group.mediaAccent,
                                           selected ? 0.160f : 0.130f);

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

    // El encabezado es deliberadamente transparente: no crea una superficie
    // oscura independiente sobre el gradiente Dynamic Obsidian de la card.
    // El icono y el texto se dibujan directamente sobre la superficie iluminada.

    EnsureIconBitmap(groupIndex);
    if (group.iconBitmap)
    {
        D2D1_RECT_F iconDest = D2D1::RectF(
            static_cast<float>(iconX),
            static_cast<float>(iconY),
            static_cast<float>(iconX + iconSize),
            static_cast<float>(iconY + iconSize));
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

    D2D1_ROUNDED_RECT cr = D2D1::RoundedRect(
        D2D1::RectF(static_cast<float>(rc.left) + 0.5f, static_cast<float>(rc.top) + 0.5f,
                    static_cast<float>(rc.right) - 0.5f, static_cast<float>(rc.bottom) - 0.5f),
        12.0f, 12.0f);
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
        DrawIconEx(hdc, iconX, iconY, group.icon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
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

static void PaintSelectorScene(HWND hwnd, HDC hdc)
{
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    // Ya no rellenamos con negro - DWM composition maneja el fondo
    ClearClientToGlassKey(hdc, clientRect);

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

        // El fondo es una única bitmap cacheada durante toda la sesión.
        if (g_blurBackgroundCreated && g_blurPixels && g_blurDC)
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
                g_d2dDCRenderTarget->DrawBitmap(g_blurD2DBitmap, &bgRect, 0.82f,
                                                 D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);

            // Tinte adicional muy ligero, manteniendo visible la composición translúcida.
            g_d2dBrush->SetColor(D2D1::ColorF(0.08f, 0.11f, 0.16f, 0.10f));
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

        RECT containerShadow = {
            ScaleLayoutPx(12.0f),
            clientRect.bottom - ScaleLayoutPx(8.0f),
            clientRect.right - ScaleLayoutPx(12.0f),
            clientRect.bottom
        };
        PaintBottomShadowD2D(containerShadow, ScaleLayoutPx(12.0f), 0.34f);

    for (int slot = 0; slot < kCarouselSlots; ++slot)
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

            float distFromCenter = fabsf(static_cast<float>(slot - 2) + g_animOffset);
            bool selected = (distFromCenter < 0.5f);
            RECT cardRect = GetSlotCardRect(slot);
            float radius = selected ? static_cast<float>(g_cardStyle.selectedCornerRadius) * g_uiScale.value
                                    : static_cast<float>(g_cardStyle.cornerRadius) * g_uiScale.value;
            PaintBottomShadowD2D(cardRect, radius, selected ? 0.28f : 0.20f);

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
            g_d2dDCRenderTarget->DrawRoundedRectangle(&cr, g_d2dBrush, selected ? 1.5f : 1.0f);

            RECT prevRect = GetSlotPreviewRect(slot);
            D2D1_ROUNDED_RECT pr = D2D1::RoundedRect(
                D2D1::RectF(static_cast<float>(prevRect.left) + 0.5f, static_cast<float>(prevRect.top) + 0.5f,
                            static_cast<float>(prevRect.right) - 0.5f, static_cast<float>(prevRect.bottom) - 0.5f),
                6.0f, 6.0f);
            // No se pinta un panel opaco detrás del thumbnail. El thumbnail
            // DWM permanece nítido y con opacity=255; los huecos dejan ver la
            // superficie translúcida de la card.
            D2D1_COLOR_F prevBorder = selected
                ? D2D1::ColorF(0.080f, 0.085f, 0.095f, 0.60f)
                : D2D1::ColorF(0.035f, 0.040f, 0.050f, 0.50f);
            g_d2dBrush->SetColor(prevBorder);
            g_d2dDCRenderTarget->DrawRoundedRectangle(&pr, g_d2dBrush, 1.0f);

            if (g_groups[index].representativeWindow &&
                IsIconic(g_groups[index].representativeWindow) &&
                g_groups[index].icon)
            {
                EnsureIconBitmap(index);
                int relative = slot - 2;
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
                D2D1_ROUNDED_RECT hoverRect = D2D1::RoundedRect(
                    D2D1::RectF(static_cast<float>(cardRect.left) - 3.0f,
                                static_cast<float>(cardRect.top) - 3.0f,
                                static_cast<float>(cardRect.right) + 3.0f,
                                static_cast<float>(cardRect.bottom) + 3.0f),
                    radius + 2.0f, radius + 2.0f);
                COLORREF accent = g_groups[index].mediaAccent.valid
                    ? g_groups[index].mediaAccent.color : RGB(110, 116, 130);
                g_d2dBrush->SetColor(D2D1::ColorF(
                    GetRValue(accent) / 255.0f, GetGValue(accent) / 255.0f,
                    GetBValue(accent) / 255.0f, 0.42f));
                g_d2dDCRenderTarget->DrawRoundedRectangle(&hoverRect, g_d2dBrush, 1.25f);

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
                        g_d2dDCRenderTarget->DrawGeometry(closeGeometry, g_d2dBrush, 1.0f);
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

    for (int slot = 0; slot < kCarouselSlots; ++slot)
    {
        int index = ResolveGroupIndex(slot);
        if (index < 0)
            continue;
        float dist = fabsf(static_cast<float>(slot - 2) + g_animOffset);
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
            if (!IsSelectorActive() || !g_tabDown)
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
            if (elapsed >= static_cast<ULONGLONG>(kAnimDurationMs))
            {
                g_animActive = false;
                g_animOffset = 0.0f;
                g_animStartOffset = 0.0f;
                KillTimer(hwnd, kAnimTimerId);
            }
            else
            {
                float t = static_cast<float>(elapsed) / static_cast<float>(kAnimDurationMs);
                float ease = 1.0f - (1.0f - t) * (1.0f - t);
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

    case WM_SIZE:
        UpdateThumbnailSlots();
        return 0;

    case WM_CLOSE:
        return 0;

    case WM_DESTROY:
        StopCarouselAnimation();
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

    HINSTANCE hInst = GetModuleHandleW(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = SelectorWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kWindowClassName;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    wc.hbrBackground = nullptr;
    RegisterClassW(&wc);

    g_classesRegistered = true;
    return true;
}

static void UnregisterSelectorClasses()
{
    if (!g_classesRegistered)
        return;
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    UnregisterClassW(kWindowClassName, hInst);
    g_classesRegistered = false;
}

static bool CreateSelector()
{
    if (g_selectorAnimation == SelectorAnimationState::Closing)
        EmergencyCloseSelector();
    if (g_selector || g_selectorOpening || g_state != SelectorState::SelectorActive ||
        !RegisterSelectorClasses())
        return false;

    g_selectorOpening = true;
    g_selectorOriginWindow = GetForegroundWindow();
    MarkSelectorActivity();
    RefreshWindowList();
    if (g_groups.empty())
    {
        g_selectorOpening = false;
        return false;
    }

    // Usar el monitor de la ventana activa; no asumir 1920x1080 ni el monitor primario.
    HMONITOR monitor = MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo))
    {
        g_selectorOpening = false;
        return false;
    }

    const RECT& work = monitorInfo.rcWork;
    UpdateUIScaleForWorkArea(work);
    int x = work.left + ((work.right - work.left) - g_runtimeSelectorWidth) / 2;
    int y = work.top + ((work.bottom - work.top) - g_runtimeSelectorHeight) / 2;

    StopCarouselAnimation();

    // Capturar/procesar antes de crear y mostrar la ventana, evitando capturarla a sí misma.
    CreateBlurBackground(x, y);

    g_selector = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWindowClassName, L"",
        WS_POPUP,
        x, y, g_runtimeSelectorWidth, g_runtimeSelectorHeight,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!g_selector)
    {
        g_selectorOpening = false;
        return false;
    }

    g_selectorOpening = false;
    RemoveNativeSelectorFrame(g_selector);
    ApplySelectorVisuals(g_selector);
    UpdateSelectorControls();
    ShowWindow(g_selector, SW_SHOWNOACTIVATE);
    UpdateWindow(g_selector);
    SetWindowPos(g_selector, HWND_TOPMOST, x, y,
                 g_runtimeSelectorWidth, g_runtimeSelectorHeight,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    g_selectorAnimation = SelectorAnimationState::Opening;
    g_selectorAnimationStart = GetTickCount64();
    g_sceneScaleX = 0.94f;
    g_sceneScaleY = 0.94f;
    g_sceneOpacity = 0.05f;
    SetTimer(g_selector, kSelectorMotionTimerId, GetAnimationTimerInterval(), nullptr);
    return true;
}

static void DestroySelector()
{
    // El selector no utiliza captura ni clipping, pero estas llamadas hacen
    // la salida defensiva e idempotente ante una interrupción externa.
    ReleaseCapture();
    ClipCursor(nullptr);
    StopCarouselAnimation();
    if (g_selector && IsWindow(g_selector))
        KillTimer(g_selector, kTabRepeatTimerId);
    g_tabRepeatStarted = false;
    UnregisterAllThumbnails();
    if (g_selector)
    {
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
    g_state = SelectorState::Canceling;
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
    if (elapsed >= static_cast<ULONGLONG>(kAnimDurationMs))
    {
        g_animActive = false;
        g_animOffset = 0.0f;
        g_animStartOffset = 0.0f;
    }
    else
    {
        float t = static_cast<float>(elapsed) / static_cast<float>(kAnimDurationMs);
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        g_animOffset = g_animStartOffset * (1.0f - ease);
    }
    UpdateSelectorControls();
}

static void StartSlide(int steps)
{
    if (steps == 0 || g_groups.empty())
        return;
    if (g_selectorAnimation == SelectorAnimationState::Opening ||
        g_selectorAnimation == SelectorAnimationState::SelectionChange)
    {
        g_selectionStartScaleX = g_sceneScaleX;
        g_selectionStartScaleY = g_sceneScaleY;
        g_selectionStartOpacity = g_sceneOpacity;
        g_selectorAnimation = SelectorAnimationState::SelectionChange;
        g_selectorAnimationStart = GetTickCount64();
        if (g_selector && IsWindow(g_selector))
            SetTimer(g_selector, kSelectorMotionTimerId, GetAnimationTimerInterval(), nullptr);
    }
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

static void ActivateWindow(HWND target)
{
    if (!target || !IsWindow(target))
        return;

    HWND foreground = GetForegroundWindow();
    DWORD currentThread = GetCurrentThreadId();
    DWORD foregroundThread = foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    DWORD targetThread = GetWindowThreadProcessId(target, nullptr);

    if (IsIconic(target))
        ShowWindow(target, SW_RESTORE);

    bool attachedForeground = false;
    bool attachedTarget = false;

    if (foregroundThread && foregroundThread != currentThread)
        attachedForeground = AttachThreadInput(currentThread, foregroundThread, TRUE) != FALSE;
    if (targetThread && targetThread != currentThread && targetThread != foregroundThread)
        attachedTarget = AttachThreadInput(currentThread, targetThread, TRUE) != FALSE;

    AllowSetForegroundWindow(ASFW_ANY);
    BringWindowToTop(target);
    SetForegroundWindow(target);
    SetActiveWindow(target);

    if (attachedTarget)
        AttachThreadInput(currentThread, targetThread, FALSE);
    if (attachedForeground)
        AttachThreadInput(currentThread, foregroundThread, FALSE);
}

static void ConfirmSelection()
{
    if (!IsSelectorActuallyActive() || g_cleanupInProgress)
        return;

    g_state = SelectorState::Confirming;
    HWND target = nullptr;
    if (g_selected >= 0 && g_selected < static_cast<int>(g_groups.size()))
    {
        AppGroup& group = g_groups[g_selected];
        if (group.representativeWindow && IsWindow(group.representativeWindow))
            target = group.representativeWindow;
        else
        {
            for (size_t i = 0; i < group.windows.size(); ++i)
            {
                if (group.windows[i] && IsWindow(group.windows[i]))
                {
                    target = group.windows[i];
                    break;
                }
            }
        }
    }

    if (target && target == g_selectorOriginWindow)
    {
        g_state = SelectorState::Canceling;
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

    g_state = SelectorState::Canceling;
    StartSelectorClose(nullptr);
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

    if (g_sessionModifier == ModifierSession::AltGr)
        return !g_altLeftDown && !g_altRightDown &&
               !g_ctrlLeftDown && !g_ctrlRightDown;

    return false;
}

static LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode != HC_ACTION || !lParam)
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if ((key->flags & LLKHF_INJECTED) != 0)
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);

    bool up = IsKeyUpMessage(wParam) || ((key->flags & LLKHF_UP) != 0);
    bool down = !up && IsKeyDownMessage(wParam);
    DWORD vk = key->vkCode;

    if (IsSelectorActuallyActive())
        MarkSelectorActivity();

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
            if (IsSelectorActive() && g_sessionModifier == ModifierSession::LeftAlt)
            {
                ConfirmSelection();
            }
        }
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isRightAlt)
    {
        if (down)
        {
            g_altRightDown = true;
            g_rightAltSeen = true;
            g_altGrActive = IsAltGrPhysicallyDown() || g_rightCtrlSeen;
        }
        if (up)
        {
            g_altRightDown = false;
            if (IsSelectorActive() && SessionModifierReleased())
            {
                ConfirmSelection();
            }
            g_rightAltSeen = false;
        }
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isRightCtrl)
    {
        if (down)
        {
            g_ctrlRightDown = true;
            g_rightCtrlSeen = true;
            g_altGrActive = IsAltGrPhysicallyDown() || g_rightAltSeen;
        }
        if (up)
        {
            g_ctrlRightDown = false;
            if (IsSelectorActive() && SessionModifierReleased())
            {
                ConfirmSelection();
            }
            g_rightCtrlSeen = false;
        }
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (isLeftCtrl)
    {
        if (down)
        {
            g_ctrlLeftDown = true;
            g_altGrActive = IsAltGrPhysicallyDown();
        }
        if (up)
        {
            g_ctrlLeftDown = false;
            if (IsSelectorActive() && SessionModifierReleased())
            {
                ConfirmSelection();
            }
        }
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (vk == VK_ESCAPE && down && IsSelectorActive())
    {
        CancelSelection();
        return 1;
    }

    if (IsSelectorActive() && down && vk == VK_LEFT)
    {
        SelectPrevious();
        return 1;
    }

    if (IsSelectorActive() && down && vk == VK_RIGHT)
    {
        SelectNext();
        return 1;
    }

    if (vk == VK_RETURN && IsSelectorActive())
    {
        if (down)
        {
            ConfirmSelection();
        }
        return 1;
    }

    if (vk == VK_TAB)
    {
        bool altHeld = g_altLeftDown || g_altRightDown ||
                       ((key->flags & LLKHF_ALTDOWN) != 0) ||
                       ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
        bool altGrHeld = IsAltGrPhysicallyDown() || g_altGrActive;

        if (down)
        {
            if (!IsSelectorActive())
            {
                if (altHeld || altGrHeld)
                {
                    g_sessionModifier = altGrHeld ? ModifierSession::AltGr : ModifierSession::LeftAlt;
                    g_state = SelectorState::SelectorActive;
                    g_tabDown = true;
                    g_tabSuppressed = true;
                    g_altGrActive = altGrHeld;

                    if (CreateSelector())
                    {
                        SelectNext();
                        g_tabRepeatStarted = false;
                        SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
                    }
                    else
                    {
                        g_state = SelectorState::Idle;
                        g_sessionModifier = ModifierSession::None;
                    }
                    return 1;
                }
            }
            else
            {
                if (!g_tabDown)
                {
                    bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    if (shiftDown)
                        SelectPrevious();
                    else
                        SelectNext();
                    g_tabDown = true;
                    g_tabRepeatStarted = false;
                    SetTimer(g_selector, kTabRepeatTimerId, kTabRepeatInitialDelayMs, nullptr);
                }
                return 1;
            }
        }
        else if (up)
        {
            g_tabDown = false;
            g_tabRepeatStarted = false;
            if (g_selector && IsWindow(g_selector))
                KillTimer(g_selector, kTabRepeatTimerId);
            if (IsSelectorActive() || g_tabSuppressed)
            {
                g_tabSuppressed = false;
                return 1;
            }
        }
    }

    if (IsSelectorActive())
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
    if (nCode != HC_ACTION || !lParam)
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);

    MSLLHOOKSTRUCT* mouse = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

    if (IsSelectorActive() && g_selector && IsWindow(g_selector))
    {
        if (wParam == WM_MOUSEMOVE)
        {
            POINT hoverPoint = mouse->pt;
            ScreenToClient(g_selector, &hoverPoint);
            UpdateHoveredSlot(hoverPoint);
            MarkSelectorActivity();
            // El hook observa el movimiento, pero no lo suprime: devolver 1
            // aquí congelaba el cursor y evitaba que Windows entregara
            // WM_MOUSEMOVE normalmente.
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }

        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
            wParam == WM_MBUTTONDOWN || wParam == WM_NCLBUTTONDOWN ||
            wParam == WM_NCRBUTTONDOWN || wParam == WM_NCMBUTTONDOWN)
        {
            RECT selRect = {};
            GetWindowRect(g_selector, &selRect);

            if (!PtInRect(&selRect, mouse->pt))
            {
                g_hoveredSlot = -1;
                g_hoveredCloseButton = false;
                CancelSelection();
                return 1;
            }
            else if (wParam == WM_LBUTTONDOWN)
            {
                POINT clientPt = mouse->pt;
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
                    else if (clickedSlot == 2)
                    {
                        ConfirmSelection();
                    }
                    else
                    {
                        int steps = clickedSlot - 2;
                        StartSlide(steps);
                    }
                }
                return 1;
            }
            else
            {
                return 1;
            }
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
            if (delta > 0)
                SelectNextSmooth(1);
            else if (delta < 0)
                SelectPreviousSmooth(1);
            return 1;
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

static DWORD WINAPI HookThreadProc(LPVOID)
{
    MSG initialMessage = {};
    PeekMessageW(&initialMessage, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hookThreadId = GetCurrentThreadId();
    g_keyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL, KeyboardHook, GetModuleHandleW(nullptr), 0);
    g_mouseHook = SetWindowsHookExW(
        WH_MOUSE_LL, MouseHook, GetModuleHandleW(nullptr), 0);

    if (g_keyboardHook)
        InterlockedExchange(&g_hookInstalled, 1);

    if (g_hookReadyEvent)
        SetEvent(g_hookReadyEvent);

    SetTimer(nullptr, kActivityTimerId, 250, nullptr);

    MSG message;
    while (InterlockedCompareExchange(&g_shutdownRequested, 0, 0) == 0 &&
           GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        if (message.message == WM_TIMER && message.wParam == kActivityTimerId)
        {
            RegisterUserWindow(GetForegroundWindow(), true);
            PruneUserWindowRegistry();

            if (g_state == SelectorState::SelectorActive)
            {
                ULONGLONG now = GetTickCount64();
                bool invalidWindow = !g_selector || !IsWindow(g_selector);
                bool staleWithoutPhysicalSession =
                    g_lastSelectorActivity != 0 &&
                    now - g_lastSelectorActivity > kSelectorFailsafeMs &&
                    !g_tabDown && !g_altLeftDown && !g_altRightDown &&
                    !g_ctrlLeftDown && !g_ctrlRightDown;
                if (invalidWindow || staleWithoutPhysicalSession)
                    EmergencyCloseSelector();
            }
        }
        else
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    KillTimer(nullptr, kActivityTimerId);

    InterlockedExchange(&g_shutdownRequested, 1);
    if (g_selector || g_state != SelectorState::Idle)
        EmergencyCloseSelector();
    else
        CleanupKeyboardState();

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
    InterlockedExchange(&g_hookInstalled, 0);

    UnregisterSelectorClasses();
    g_hookThreadId = 0;
    return 0;
}

BOOL Wh_ModInit()
{
    LoadAnimationSettings();
    ResetKeyboardState();
    g_state = SelectorState::Idle;
    InterlockedExchange(&g_shutdownRequested, 0);
    InterlockedExchange(&g_hookInstalled, 0);

    g_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookReadyEvent)
        return FALSE;

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr);
    if (!g_hookThread)
    {
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_hookReadyEvent, 3000);
    CloseHandle(g_hookReadyEvent);
    g_hookReadyEvent = nullptr;

    if (InterlockedCompareExchange(&g_hookInstalled, 0, 0) == 0)
    {
        InterlockedExchange(&g_shutdownRequested, 1);
        if (g_hookThreadId)
            PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 3000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadAnimationSettings();
    if (g_selector && IsWindow(g_selector))
    {
        if (g_selectorAnimation != SelectorAnimationState::None &&
            g_selectorAnimation != SelectorAnimationState::Open)
            SetTimer(g_selector, kSelectorMotionTimerId,
                     GetAnimationTimerInterval(), nullptr);
        if (g_animActive)
            SetTimer(g_selector, kSelectorMotionTimerId,
                     GetAnimationTimerInterval(), nullptr);
    }
}

void Wh_ModUninit()
{
    InterlockedExchange(&g_shutdownRequested, 1);

    if (g_hookThreadId)
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);

    if (g_hookThread)
    {
        WaitForSingleObject(g_hookThread, 5000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    g_keyboardHook = nullptr;
    g_mouseHook = nullptr;
    g_selector = nullptr;

    ReleaseBlurBackground();
    ReleaseGroupResources();
    g_groups.clear();
    g_userWindows.clear();
    g_selected = 0;
    g_state = SelectorState::Idle;
    CleanupKeyboardState();
    ResetKeyboardState();
}
