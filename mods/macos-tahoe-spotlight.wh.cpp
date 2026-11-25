// ==WindhawkMod==
// @id              macos-tahoe-spotlight
// @name            macOS 26 Tahoe Spotlight
// @description     macOS 26 Tahoe Liquid Glass Spotlight Search (Alt+Space) with floating mode orbs, Everything disk search, Quick Look previews, inline calculator, clipboard history, process killer, windows switcher, bookmarks & Dock integration
// @version         1.0.0
// @author          Badalemo
// @github          https://github.com/Badalemo
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lshlwapi -lpropsys -ldwmapi -ld2d1 -ld3d11 -ldxgi -ldwrite -lwindowscodecs -lgdi32 -luser32 -lversion -lshcore -ladvapi32 -lcrypt32 -limm32 -lpsapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS 26 Tahoe Spotlight

A faithful **macOS 26 Tahoe Spotlight Search** launcher built with real-time
**DXGI Optical Liquid Glass** refraction (`Clear` & `Tinted` modes), **zero
idle overhead** (all DXGI capture and GPU work stops completely the instant
Spotlight hides), and deep Windows integration.

![macOS 26 Tahoe Spotlight collapsed pill and mode orbs](https://raw.githubusercontent.com/Badalemo/windhawk-mods-media/main/spotlight-pill.png)

![macOS 26 Tahoe Spotlight expanded search view](https://raw.githubusercontent.com/Badalemo/windhawk-mods-media/main/spotlight.png)

## Highlights
- **1:1 macOS 26 Tahoe Appearance**:
  - **Collapsed State**: Floating frosted Liquid Glass `Spotlight Search` pill
    paired with 4 circular glass mode orbs (**Apps** `Ctrl+1`, **Files**
    `Ctrl+2`, **Actions** `Ctrl+3`, **Clipboard** `Ctrl+4`).
  - **Expanded Card**: Seamless morph into a unified squircle glass sheet with
    inline ghost autocomplete pill (`Tab`), contextual category pills (`All`,
    `Apps`, `Files`, `Actions`, `Clipboard`, `Windows`, `Settings`), and quick
    key badges.
  - **Right-Hand Quick Look Split Pane** (`Alt+P` or `Ctrl+I`): Live image
    previews, monospace file viewer (`JetBrains Mono`), app details & **Pin to
    Dock** integration, window/process controls, and clipboard previews.
- **Instant Multi-Source Search**:
  - **Apps**: Start Menu shortcuts, Desktop links, and Microsoft Store / UWP
    apps (`shell:AppsFolder`), with PATH tool matching, live filesystem watcher,
    per-query learning, and pinning (`Ctrl+P`).
  - **Files (Everything IPC + Native Fallback)**: Instant full-disk search
    when **Everything** is running (with category filter tabs plus `dir:`,
    `file:`, `type:`, and Everything query syntax), and Windows **Recent files**.
  - **Calculator & Unit / Base Converter**: Type math (`24 * (12 + 8%)`,
    `sqrt(144)`, `20% of 350`), conversions (`10 km to miles`, `72 f to c`,
    `255 to hex`), or reuse `ans`.
  - **Quick Keys & Process Killer**: `kl <name>` / `kill <name>` terminates
    matching running processes cleanly; `win:` switches/manages open windows;
    `tm`, `wt`, `ps`, `cmd`, `lock`, `sleep`, `bm:` (Bookmarks), `cfg:`
    (Settings), `>` (Actions).
  - **Encrypted Clipboard History**: Captures text and screenshots (encrypted
    per user with DPAPI) and pastes straight into the previously focused window.

> **Tip**: By default `Alt+Space` automatically overrides busy global
> hotkeys (even if another launcher holds `Alt+Space`) without closing it, or
> you can switch shortcuts cleanly in Settings.

## Credits & Differentiation
Inline calculator, Everything IPC query integration, and DPAPI clipboard history
patterns were inspired by David Ravelo's MIT-licensed
[`spotlight-launcher`](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/spotlight-launcher.wh.cpp)
(<https://github.com/DavidRaveloU>, Copyright (c) 2026-present David Ravelo).
**macOS 26 Tahoe Spotlight** is a distinct macOS 26 Tahoe redesign featuring a
collapsed frosted glass pill + 4 mode orbs, DXGI optical Liquid Glass
refraction, a right-hand Quick Look split pane, `kl`/`kill` process termination,
a `win:` window switcher, browser bookmarks, and direct **macOS 26 Tahoe Dock**
(`macos-tahoe-dock`) IPC Pin-to-Dock integration.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearance: auto
  $name: Appearance theme
  $description: Follow Windows Light/Dark mode or force Dark or Light Liquid Glass.
  $options:
  - auto: Auto (Follow Windows)
  - dark: Dark Mode
  - light: Light Mode
- glassStyle: clear
  $name: Liquid Glass style
  $description: Clear renders crystal translucent Tahoe glass; Tinted adds Windows accent color warmth.
  $options:
  - clear: Clear (Crystal Glass)
  - tinted: Tinted (Accent)
- glassRefraction: lens
  $name: Optical refraction mode
  $description: Real-time DXGI edge refraction around the pill, orbs, and expanded card.
  $options:
  - lens: Lens Refraction (macOS 26 Tahoe)
  - reflection: Mirror Reflection
  - stretch: Convex Stretch
  - off: Frosted Backdrop Only (No DXGI Capture)
- glassEdgeDepth: 45
  $name: Edge refraction depth (0-100)
  $description: Width of the optical glass bezel refraction.
- glassEdgeStrength: 68
  $name: Specular rim strength (0-100)
  $description: Crispness of the curved Fresnel highlight rim.
- glassTint: 24
  $name: Glass tint opacity (0-100)
  $description: Background tint balance behind Spotlight text.
- glassBlur: 52
  $name: Backdrop blur (0-100)
  $description: Optical frost strength behind the glass surface.
- glassBrightnessLimit: 78
  $name: Dark-mode highlight limiter (0-100)
  $description: Caps bright wallpaper highlights behind dark Liquid Glass so text stays legible.
- hotkeyCombo: altSpace
  $name: Global shortcut
  $description: Keyboard shortcut to open or close Spotlight from anywhere.
  $options:
  - altSpace: Alt + Space (macOS Default)
  - ctrlSpace: Ctrl + Space
  - ctrlAltSpace: Ctrl + Alt + Space
  - shiftAltSpace: Shift + Alt + Space
  - winShiftS: Win + Shift + S (Alternate)
- overrideBusyHotkey: true
  $name: Override busy Alt+Space automatically
  $description: Use a lightweight keyboard hook when another app already claims Alt+Space (prevents Windows system menu popup too).
- openMonitor: cursor
  $name: Open on monitor
  $description: Which display Spotlight appears on when summoned.
  $options:
  - cursor: Monitor Under Mouse Cursor
  - foreground: Monitor of Focused Window
  - primary: Primary Monitor
- verticalPosition: 24
  $name: Vertical offset (% from top)
  $description: Top position percentage inside the active monitor work area (macOS default ~24%).
- disableInFullscreen: true
  $name: Disable in fullscreen apps
  $description: Ignore the shortcut while a fullscreen game or video is focused.
- enableEverything: true
  $name: Use Everything for instant file search
  $description: Queries voidtools Everything over IPC when running; falls back to Windows folders automatically.
- enableClipboardHistory: true
  $name: Clipboard history (Ctrl+4 / clip:)
  $description: Remember copied text and screenshots encrypted for your Windows account.
- clipRetentionDays: 7
  $name: Clipboard retention (days)
  $description: Unpinned clipboard items expire after this many days (0 = keep up to max items).
- enableBookmarks: true
  $name: Browser bookmarks (bm:)
  $description: Index Chrome, Edge, Brave, Vivaldi, and Opera bookmarks asynchronously.
- enableCalculator: true
  $name: Inline calculator & unit conversions
  $description: Evaluate math expressions, units, and hex/bin/oct/dec conversions inline.
- enableWebSearch: true
  $name: Web search & URL launch
  $description: Offer direct URL launch and web search fallback when no local item wins.
- webEngine: google
  $name: Default web search engine
  $options:
  - google: Google
  - duckduckgo: DuckDuckGo
  - bing: Bing
  - youtube: YouTube
  - wikipedia: Wikipedia
- showQuickLookByDefault: false
  $name: Open Quick Look panel automatically
  $description: Always show the right-hand Quick Look & Actions split pane when expanded.
*/
// ==/WindhawkModSettings==

// MIT Attribution & License Notice:
// Inline calculator, Everything IPC query, and DPAPI clipboard history
// patterns were inspired by David Ravelo's spotlight-launcher (MIT License,
// Copyright (c) 2026-present David Ravelo, https://github.com/DavidRaveloU).
// Redesigned independently for macOS 26 Tahoe with optical Liquid Glass,
// mode orbs, Quick Look split pane, kl/kill process killer, and Dock IPC.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <appmodel.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <imm.h>
#include <ole2.h>
#include <propkey.h>
#include <propsys.h>
#include <psapi.h>
#include <sddl.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>
#include <wincrypt.h>
#include <wincodec.h>
#include <winver.h>

// windows.h defines DrawText as DrawTextW; remove before including d2d1.h.
#ifdef DrawText
#undef DrawText
#endif

#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <d2d1effects.h>
#include <d3d10.h>
#include <d3d11.h>
#include <dispatcherqueue.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi.h>
#include <dxgi1_2.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.h>

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

#ifndef WM_CLIPBOARDUPDATE
#define WM_CLIPBOARDUPDATE 0x031D
#endif

#ifndef FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000
#endif

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

namespace wuc = winrt::Windows::UI::Composition;
namespace wf = winrt::Windows::Foundation;
namespace wge = winrt::Windows::Graphics::Effects;

// ============================================================================
// 1. Composition Interop, Constants & State Types
// ============================================================================

constexpr DWORD kWdaExcludeFromCapture = 0x00000011;

constexpr UINT kWmAppSettings = WM_APP + 101;
constexpr UINT kWmAppQuit = WM_APP + 102;
constexpr UINT kWmAppGlassFrame = WM_APP + 106;
constexpr UINT kWmAppDeviceLost = WM_APP + 107;
constexpr UINT kWmAppIndexDone = WM_APP + 110;
constexpr UINT kWmAppMenuChanged = WM_APP + 111;
constexpr UINT kWmAppBookmarksDone = WM_APP + 112;
constexpr UINT kWmAppRecentDone = WM_APP + 113;
constexpr UINT kWmAppLocalFilesDone = WM_APP + 115;
constexpr UINT kWmAppThumbDone = WM_APP + 116;
constexpr UINT kWmAppIconDone = WM_APP + 117;

constexpr UINT_PTR kTimerFrame = 1;
constexpr UINT_PTR kTimerBlink = 2;
constexpr UINT_PTR kTimerGlassMaintenance = 3;
constexpr UINT_PTR kTimerSearchDebounce = 4;
constexpr UINT_PTR kTimerEvFallback = 5;
constexpr UINT_PTR kTimerConfirmReset = 6;
constexpr UINT_PTR kTimerToastHide = 7;
constexpr UINT_PTR kTimerClipSave = 8;

constexpr int kHotkeySpotlight = 3001;
constexpr WCHAR kSpotlightHostClass[] = L"WindhawkMacOSTahoeSpotlightHostWnd";
constexpr WCHAR kSpotlightUiClass[] = L"WindhawkMacOSTahoeSpotlightWnd";

// WinRT Composition interop interfaces (self-contained for MinGW)
struct ICompositorDesktopInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall CreateDesktopWindowTarget(HWND window,
                                                        BOOL isTopmost,
                                                        void** target) = 0;
    virtual HRESULT __stdcall EnsureOnThread(DWORD threadId) = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<ICompositorDesktopInteropLocal>{
        0x29E691FA,
        0x4567,
        0x4DCA,
        {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};

struct ICompositorInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall CreateCompositionSurfaceForHandle(
        HANDLE swapChain,
        void** result) = 0;
    virtual HRESULT __stdcall CreateCompositionSurfaceForSwapChain(
        ::IUnknown* swapChain,
        void** result) = 0;
    virtual HRESULT __stdcall CreateGraphicsDevice(::IUnknown* renderingDevice,
                                                   void** result) = 0;
};
template <>
inline constexpr winrt::guid winrt::impl::guid_v<ICompositorInteropLocal>{
    0x25297D5C,
    0x3AD4,
    0x4C9C,
    {0xB5, 0xCF, 0xE3, 0x6A, 0x38, 0x51, 0x23, 0x30}};

struct ICompositionGraphicsDeviceInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall GetRenderingDevice(::IUnknown** value) = 0;
    virtual HRESULT __stdcall SetRenderingDevice(::IUnknown* value) = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<ICompositionGraphicsDeviceInteropLocal>{
        0xA116FF71,
        0xF8BF,
        0x4C8A,
        {0x9C, 0x98, 0x70, 0x77, 0x9A, 0x32, 0xA9, 0xC8}};

struct ICompositionDrawingSurfaceInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall BeginDraw(const RECT* updateRect,
                                        REFIID iid,
                                        void** updateObject,
                                        POINT* updateOffset) = 0;
    virtual HRESULT __stdcall EndDraw() = 0;
    virtual HRESULT __stdcall Resize(SIZE sizePixels) = 0;
    virtual HRESULT __stdcall Scroll(const RECT* scrollRect,
                                     const RECT* clipRect,
                                     int offsetX,
                                     int offsetY) = 0;
    virtual HRESULT __stdcall ResumeDraw() = 0;
    virtual HRESULT __stdcall SuspendDraw() = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<ICompositionDrawingSurfaceInteropLocal>{
        0xFD04E6E3,
        0xFE0C,
        0x4C3C,
        {0xAB, 0x19, 0xA0, 0x76, 0x01, 0xA5, 0x76, 0xEE}};

enum SPOTLIGHT_EFFECT_PROP_MAP : int {
    SPOTLIGHT_EFFECT_PROP_DIRECT = 1,
};

MIDL_INTERFACE("2FC57384-A068-44D7-A331-30982FCF7177")
ISpotlightGraphicsEffectD2D1Interop : public ::IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        SPOTLIGHT_EFFECT_PROP_MAP* mapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) = 0;
};
__CRT_UUID_DECL(ISpotlightGraphicsEffectD2D1Interop,
                0x2fc57384,
                0xa068,
                0x44d7,
                0xa3,
                0x31,
                0x30,
                0x98,
                0x2f,
                0xcf,
                0x71,
                0x77)

struct IGeometrySource2DInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall GetGeometry(ID2D1Geometry** value) = 0;
    virtual HRESULT __stdcall TryGetGeometryUsingFactory(
        ID2D1Factory* factory,
        ID2D1Geometry** value) = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<IGeometrySource2DInteropLocal>{
        0x0657AF73,
        0x53FD,
        0x47CF,
        {0x84, 0xFF, 0xC8, 0x49, 0x2D, 0x2A, 0x80, 0xA3}};

struct GlassPath
    : winrt::implements<GlassPath,
                        winrt::Windows::Graphics::IGeometrySource2D,
                        IGeometrySource2DInteropLocal> {
    explicit GlassPath(ID2D1Geometry* geometry) {
        m_geometry.copy_from(geometry);
    }
    HRESULT __stdcall GetGeometry(ID2D1Geometry** value) noexcept override {
        if (!value) {
            return E_POINTER;
        }
        m_geometry.copy_to(value);
        return S_OK;
    }
    HRESULT __stdcall TryGetGeometryUsingFactory(
        ID2D1Factory*,
        ID2D1Geometry** value) noexcept override {
        if (!value) {
            return E_POINTER;
        }
        *value = nullptr;
        return E_NOTIMPL;
    }

private:
    winrt::com_ptr<ID2D1Geometry> m_geometry;
};

// Settings enums
enum class AppearanceMode { Auto = 0, Dark = 1, Light = 2 };
enum class GlassStyle { Clear = 0, Tinted = 1 };
enum class GlassRefraction { Lens = 0, Reflection = 1, Stretch = 2, Off = 3 };
enum class HotkeyCombo {
    AltSpace = 0,
    CtrlSpace = 1,
    CtrlAltSpace = 2,
    ShiftAltSpace = 3,
    WinShiftS = 4
};
enum class OpenMonitor { Cursor = 0, Foreground = 1, Primary = 2 };
enum class WebEngine {
    Google = 0,
    DuckDuckGo = 1,
    Bing = 2,
    YouTube = 3,
    Wikipedia = 4
};

struct SpotlightSettings {
    AppearanceMode appearance = AppearanceMode::Auto;
    GlassStyle glassStyle = GlassStyle::Clear;
    GlassRefraction glassRefraction = GlassRefraction::Lens;
    int glassEdgeDepth = 45;
    int glassEdgeStrength = 68;
    int glassTint = 24;
    int glassBlur = 52;
    int glassBrightnessLimit = 78;
    HotkeyCombo hotkeyCombo = HotkeyCombo::AltSpace;
    bool overrideBusyHotkey = true;
    OpenMonitor openMonitor = OpenMonitor::Cursor;
    int verticalPosition = 24;
    bool disableInFullscreen = true;
    bool enableEverything = true;
    bool enableClipboardHistory = true;
    int clipRetentionDays = 7;
    bool enableBookmarks = true;
    bool enableCalculator = true;
    bool enableWebSearch = true;
    WebEngine webEngine = WebEngine::Google;
    bool showQuickLookByDefault = false;
};

// Spotlight logical modes (orbs + filter tabs + prefix modes)
enum class SpotlightMode {
    All = 0,
    Apps = 1,
    Files = 2,
    Actions = 3,
    Clipboard = 4,
    Windows = 5,
    KillProcess = 6,
    Bookmarks = 7,
    Settings = 8
};

// Contextual filter sub-tabs shown inside Files mode
enum class FileSubTab {
    All = 0,
    Folders = 1,
    Images = 2,
    Documents = 3,
    Code = 4,
    Archives = 5
};

enum class ResultKind {
    App,
    File,
    Action,
    Clipboard,
    Window,
    Process,
    Setting,
    Bookmark,
    Web,
    Url,
    RecentFile
};

enum class ActionGlyph {
    Lock,
    Moon,
    Hibernate,
    Power,
    Restart,
    SignOut,
    TaskMgr,
    Sliders,
    Chip,
    List,
    Key,
    Globe,
    Gear,
    Update,
    Grid,
    Monitor,
    Speaker,
    Mute,
    Bluetooth,
    Printer,
    Terminal,
    Folder,
    Recycle,
    Wifi,
    Clock,
    Bell,
    Camera,
    Mic,
    Doc,
    Star,
    Clipboard,
    Skull,
    Play,
    Next,
    Prev,
    Search
};

// Layout constants (DIP, scaled by monitor DPI)
constexpr float kShadowMarginDip = 36.0f;
constexpr float kCollapsedPillW = 468.0f;
constexpr float kCollapsedPillH = 56.0f;
constexpr float kOrbSizeDip = 56.0f;
constexpr float kOrbGapDip = 10.0f;
constexpr float kSheetCompactW = 696.0f;
constexpr float kSheetPreviewW = 996.0f;
constexpr float kQuickLookW = 300.0f;
constexpr float kHeaderH = 56.0f;
constexpr float kTabsBandH = 38.0f;
constexpr float kCalcHeroH = 74.0f;
constexpr float kRowH = 50.0f;
constexpr float kFooterH = 30.0f;
constexpr float kMaxListH = 370.0f;
constexpr float kSheetCornerDip = 24.0f;

SpotlightSettings g_settings;
bool g_quitting = false;
UINT g_msgToggleSpotlight = 0;
HWND g_hostHwnd = nullptr;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;

struct CachedBitmapSlot {
    winrt::com_ptr<IWICBitmapSource> wic;
    uint64_t boundDeviceGen = 0;
    winrt::com_ptr<ID2D1Bitmap> d2d;
};

struct AppEntry {
    std::wstring name;
    std::wstring folded;
    std::wstring id;       // AUMID or .lnk path
    std::wstring target;   // resolved executable
    std::wstring args;
    std::wstring launch;   // direct file launch path if non-AppsFolder
    std::wstring usageKey;
    std::wstring subtitle;
    int uses = 0;
    int lastUsed = 0;
    bool pinned = false;
    bool exactOnly = false;
    std::wstring foldedFile;
    bool iconTried = false;
    CachedBitmapSlot icon;
};

struct FileEntry {
    std::wstring name;
    std::wstring path;
    std::wstring dir;
    std::wstring ext;
    std::wstring folded;
    uint64_t size = 0;
    FILETIME writeTime{};
    DWORD attr = 0;
    bool isDir = false;
    bool noInfo = false;
};

struct BookmarkEntry {
    std::wstring name;
    std::wstring folded;
    std::wstring url;
    std::wstring folder;
    std::wstring browser;
    std::wstring profile;
    std::wstring tail;
};

struct ClipEntry {
    std::wstring text;
    std::wstring protectedText;
    bool pinned = false;
    bool image = false;
    int stamp = 0;
    int w = 0;
    int h = 0;
};

struct WindowEntry {
    HWND hwnd = nullptr;
    DWORD pid = 0;
    std::wstring title;
    std::wstring process;
    std::wstring folded;
    std::wstring exePath;
    int appIdx = -1;
    bool iconic = false;
    bool zoomed = false;
    bool topmost = false;
};

struct ProcessGroup {
    std::wstring exeName;
    std::wstring folded;
    std::wstring exePath;
    std::vector<DWORD> pids;
    SIZE_T totalBytes = 0;
    int appIdx = -1;
};

struct BuiltinAction {
    const wchar_t* id;
    const wchar_t* title;
    const wchar_t* subtitle;
    const wchar_t* keywords;
    const wchar_t* quickKey;  // shown in pill on right, and triggers instant prefix
    ActionGlyph glyph;
    D2D1_COLOR_F badgeColor;
    bool destructive = false;
    SpotlightMode switchMode = SpotlightMode::All;  // non-All jumps into that mode
};

struct BuiltinSetting {
    const wchar_t* title;
    const wchar_t* keywords;
    const wchar_t* uri;
    ActionGlyph glyph;
    D2D1_COLOR_F badgeColor;
};

struct SearchResult {
    ResultKind kind = ResultKind::App;
    int appIdx = -1;
    int actionIdx = -1;
    int settingIdx = -1;
    int clipIdx = -1;
    int windowIdx = -1;
    int procIdx = -1;
    int bookmarkIdx = -1;
    int fileIdx = -1;
    int score = 0;
    std::wstring title;
    std::wstring subtitle;
    std::wstring payload;
    std::wstring rightBadge;
    std::wstring autoFill;  // inline ghost completion text (e.g. "Message — Run")
    std::wstring foldedTitle;
    std::vector<uint8_t> matchMask;
    bool isDir = false;
    bool pinned = false;
    bool isImage = false;
    uint64_t size = 0;
    FILETIME writeTime{};
    std::wstring ext;
    D2D1_COLOR_F badgeColor{0.35f, 0.55f, 0.95f, 1.0f};
    ActionGlyph glyph = ActionGlyph::Search;
    D2D1_RECT_F hitRect{};
};

std::wstring ReadWhStringSetting(PCWSTR name) {
    PCWSTR raw = Wh_GetStringSetting(name);
    std::wstring out = raw ? raw : L"";
    if (raw) {
        Wh_FreeStringSetting(raw);
    }
    return out;
}

inline bool PointInRectF(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x <= r.right && y >= r.top && y <= r.bottom;
}

// ============================================================================
// 2. Utilities, Settings, DPAPI Protection & Inline Calculator
// ============================================================================

double NowSeconds() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f{};
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart) / static_cast<double>(freq.QuadPart);
}

constexpr ULONGLONG kEpoch2020 = 132223104000000000ULL;

int NowStamp() {
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER u{};
    u.LowPart = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    if (u.QuadPart <= kEpoch2020) return 1;
    return static_cast<int>((u.QuadPart - kEpoch2020) / 10000000ULL);
}

inline float Clamp01(float x) {
    return std::clamp(x, 0.0f, 1.0f);
}

inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float EaseOutCubic(float t) {
    t = Clamp01(t);
    float u = 1.0f - t;
    return 1.0f - u * u * u;
}

inline float EaseInOutCubic(float t) {
    t = Clamp01(t);
    return t < 0.5f ? 4.0f * t * t * t
                    : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

bool StepCriticallyDampedSpring(float& value,
                                float& velocity,
                                float target,
                                float omega,
                                float dt) {
    if (dt <= 0.0f) {
        return std::fabs(value - target) > 0.0005f ||
               std::fabs(velocity) > 0.0005f;
    }
    dt = std::min(dt, 0.05f);
    float x = value - target;
    float e = std::exp(-omega * dt);
    float temp = (velocity + omega * x) * dt;
    velocity = (velocity - omega * temp) * e;
    value = target + (x + temp) * e;
    if (std::fabs(value - target) < 0.0005f &&
        std::fabs(velocity) < 0.0005f) {
        value = target;
        velocity = 0.0f;
        return false;
    }
    return true;
}

bool SmoothExp(float& cur, float target, float rate, float dt, float eps) {
    if (cur == target) return false;
    dt = std::clamp(dt, 0.0f, 0.05f);
    cur += (target - cur) * (1.0f - std::exp(-rate * dt));
    if (std::fabs(target - cur) < eps) {
        cur = target;
        return false;
    }
    return true;
}

std::wstring TrimBlanks(std::wstring_view s) {
    size_t a = 0;
    size_t b = s.size();
    while (a < b && iswspace(s[a])) ++a;
    while (b > a && iswspace(s[b - 1])) --b;
    return std::wstring(s.substr(a, b - a));
}

wchar_t FoldChar(wchar_t c) {
    if (c < 0x80) {
        return (c >= L'A' && c <= L'Z') ? static_cast<wchar_t>(c + 32) : c;
    }
    wchar_t out[4]{};
    int n = FoldStringW(MAP_COMPOSITE, &c, 1, out, 4);
    wchar_t base = (n > 0 && out[0]) ? out[0] : c;
    return static_cast<wchar_t>(
        reinterpret_cast<uintptr_t>(CharLowerW(reinterpret_cast<LPWSTR>(
            static_cast<uintptr_t>(base)))));
}

std::wstring FoldString(std::wstring_view sv) {
    std::wstring r(sv.size(), L' ');
    for (size_t i = 0; i < sv.size(); ++i) {
        r[i] = FoldChar(sv[i]);
    }
    return r;
}

std::wstring ToLower(std::wstring_view sv) {
    std::wstring out(sv);
    for (wchar_t& ch : out) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return out;
}

bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (towlower(a[i]) != towlower(b[i])) return false;
    }
    return true;
}

bool StartsWithNoCase(std::wstring_view s, std::wstring_view prefix) {
    if (s.size() < prefix.size()) return false;
    return EqualsIgnoreCase(s.substr(0, prefix.size()), prefix);
}

bool EndsWithNoCase(std::wstring_view s, std::wstring_view suffix) {
    if (s.size() < suffix.size()) return false;
    return EqualsIgnoreCase(s.substr(s.size() - suffix.size()), suffix);
}

std::wstring NormalizePath(std::wstring_view p) {
    std::wstring s(p);
    while (!s.empty() && (s.back() == L'\\' || s.back() == L'/')) {
        if (s.size() <= 3 && s.size() >= 2 && s[1] == L':') break;
        s.pop_back();
    }
    for (wchar_t& ch : s) {
        ch = (ch == L'/') ? L'\\' : static_cast<wchar_t>(towlower(ch));
    }
    return s;
}

bool SamePath(const std::wstring& a, const std::wstring& b) {
    if (a.empty() || b.empty()) return false;
    return NormalizePath(a) == NormalizePath(b);
}

std::wstring FileNameOf(const std::wstring& p) {
    size_t i = p.find_last_of(L"\\/");
    return i == std::wstring::npos ? p : p.substr(i + 1);
}

std::wstring ParentDir(const std::wstring& p) {
    std::wstring t = p;
    while (t.size() > 3 && (t.back() == L'\\' || t.back() == L'/')) t.pop_back();
    if (t.size() <= 3) return {};
    size_t i = t.find_last_of(L"\\/");
    if (i == std::wstring::npos) return {};
    std::wstring r = t.substr(0, i);
    if (r.size() == 2 && r[1] == L':') r += L'\\';
    return r;
}

std::wstring JoinPath(const std::wstring& dir, const std::wstring& name) {
    if (dir.empty()) return name;
    return (dir.back() == L'\\' || dir.back() == L'/') ? (dir + name)
                                                       : (dir + L'\\' + name);
}

std::wstring FileExtOf(const std::wstring& name) {
    size_t d = name.find_last_of(L'.');
    if (d == std::wstring::npos || d == 0 || d + 1 >= name.size()) return {};
    std::wstring e = name.substr(d + 1);
    if (e.size() > 6) return {};
    for (auto& c : e) c = static_cast<wchar_t>(towupper(c));
    return e;
}

std::wstring ExpandedPath(const std::wstring& p) {
    if (p.empty()) return p;
    wchar_t buf[MAX_PATH * 4]{};
    DWORD n = ExpandEnvironmentStringsW(p.c_str(), buf, ARRAYSIZE(buf));
    if (n > 0 && n <= ARRAYSIZE(buf)) return std::wstring(buf, n - 1);
    return p;
}

std::wstring NativeSystemPath(const std::wstring& path) {
    static const bool wow64 = [] {
        BOOL w = FALSE;
        return IsWow64Process(GetCurrentProcess(), &w) && w;
    }();
    if (!wow64 || path.empty()) return path;
    wchar_t win[MAX_PATH]{};
    UINT n = GetWindowsDirectoryW(win, ARRAYSIZE(win));
    if (n == 0 || n >= ARRAYSIZE(win)) return path;
    std::wstring prefix = std::wstring(win) + L"\\System32\\";
    if (StartsWithNoCase(path, prefix)) {
        return std::wstring(win) + L"\\Sysnative\\" + path.substr(prefix.size());
    }
    return path;
}

std::wstring KnownFolderPath(REFKNOWNFOLDERID id) {
    PWSTR p = nullptr;
    std::wstring out;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &p)) && p) {
        out = p;
        CoTaskMemFree(p);
    }
    return out;
}

std::wstring FormatFileSize(uint64_t bytes) {
    if (bytes < 1024) {
        wchar_t buf[32]{};
        swprintf_s(buf, L"%llu B", static_cast<unsigned long long>(bytes));
        return buf;
    }
    const wchar_t* units[] = {L"KB", L"MB", L"GB", L"TB"};
    double v = static_cast<double>(bytes) / 1024.0;
    int u = 0;
    while (v >= 1024.0 && u < 3) {
        v /= 1024.0;
        ++u;
    }
    wchar_t buf[32]{};
    if (v < 10.0) {
        swprintf_s(buf, L"%.1f %s", v, units[u]);
    } else {
        swprintf_s(buf, L"%.0f %s", v, units[u]);
    }
    return buf;
}

std::wstring FormatRelativeTime(const FILETIME& utc) {
    if (utc.dwLowDateTime == 0 && utc.dwHighDateTime == 0) return {};
    FILETIME nowFt{};
    GetSystemTimeAsFileTime(&nowFt);
    ULARGE_INTEGER uNow{}, uFt{};
    uNow.LowPart = nowFt.dwLowDateTime;
    uNow.HighPart = nowFt.dwHighDateTime;
    uFt.LowPart = utc.dwLowDateTime;
    uFt.HighPart = utc.dwHighDateTime;
    if (uNow.QuadPart >= uFt.QuadPart) {
        uint64_t sec = (uNow.QuadPart - uFt.QuadPart) / 10000000ULL;
        if (sec < 90) return L"Just now";
        if (sec < 3600) return std::to_wstring(sec / 60) + L"m ago";
        if (sec < 86400) return L"Today";
        if (sec < 172800) return L"Yesterday";
        uint64_t days = sec / 86400;
        if (days < 14) return std::to_wstring(days) + L"d ago";
    }
    FILETIME local{};
    SYSTEMTIME st{};
    if (!FileTimeToLocalFileTime(&utc, &local) ||
        !FileTimeToSystemTime(&local, &st)) {
        return {};
    }
    wchar_t d[48]{};
    GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, d,
                   ARRAYSIZE(d));
    return d;
}

std::wstring FormatStamp(int stamp) {
    if (stamp <= 0) return {};
    ULARGE_INTEGER u{};
    u.QuadPart = kEpoch2020 + static_cast<ULONGLONG>(stamp) * 10000000ULL;
    FILETIME ft{u.LowPart, u.HighPart};
    return FormatRelativeTime(ft);
}

std::wstring UrlEncode(const std::wstring& input) {
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, input.c_str(),
                                      static_cast<int>(input.size()), nullptr,
                                      0, nullptr, nullptr);
    if (utf8Len <= 0) return {};
    std::string utf8(static_cast<size_t>(utf8Len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, input.c_str(),
                        static_cast<int>(input.size()), utf8.data(), utf8Len,
                        nullptr, nullptr);
    static constexpr wchar_t kHex[] = L"0123456789ABCDEF";
    std::wstring out;
    out.reserve(utf8.size() * 2);
    for (unsigned char b : utf8) {
        if ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') ||
            (b >= '0' && b <= '9') || b == '-' || b == '_' || b == '.' ||
            b == '~') {
            out.push_back(static_cast<wchar_t>(b));
        } else if (b == ' ') {
            out.push_back(L'+');
        } else {
            out.push_back(L'%');
            out.push_back(kHex[(b >> 4) & 0xF]);
            out.push_back(kHex[b & 0xF]);
        }
    }
    return out;
}

bool IsSystemDarkMode() {
    if (g_settings.appearance == AppearanceMode::Light) return false;
    if (g_settings.appearance == AppearanceMode::Dark) return true;
    DWORD val = 1;
    DWORD sz = sizeof(val);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &val,
            &sz) == ERROR_SUCCESS) {
        return val == 0;
    }
    return false;
}

winrt::Windows::UI::Color GetAccentColor() {
    DWORD argb = 0xFF0078D4;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque))) {
        return winrt::Windows::UI::Color{
            255, static_cast<uint8_t>((argb >> 16) & 0xFF),
            static_cast<uint8_t>((argb >> 8) & 0xFF),
            static_cast<uint8_t>(argb & 0xFF)};
    }
    return winrt::Windows::UI::Color{255, 0, 120, 212};
}

HMONITOR GetPrimaryMonitor() {
    POINT zero{0, 0};
    return MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
}

HMONITOR ResolveTargetMonitor() {
    if (g_settings.openMonitor == OpenMonitor::Primary) {
        return GetPrimaryMonitor();
    }
    if (g_settings.openMonitor == OpenMonitor::Foreground) {
        HWND fg = GetForegroundWindow();
        if (fg && IsWindow(fg)) {
            if (HMONITOR m = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST)) {
                return m;
            }
        }
    }
    POINT pt{};
    if (GetCursorPos(&pt)) {
        if (HMONITOR m = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST)) {
            return m;
        }
    }
    return GetPrimaryMonitor();
}

float GetMonitorDpiScale(HMONITOR mon) {
    if (!mon) mon = GetPrimaryMonitor();
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
        dpiX > 0) {
        return static_cast<float>(dpiX) / 96.0f;
    }
    return 1.0f;
}

bool IsForegroundFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || !IsWindow(fg)) return false;
    WCHAR cls[64]{};
    if (GetClassNameW(fg, cls, ARRAYSIZE(cls))) {
        if (_wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
            _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(cls, kSpotlightUiClass) == 0) {
            return false;
        }
    }
    RECT rc{};
    if (!GetWindowRect(fg, &rc)) return false;
    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) return false;
    return rc.left <= mi.rcMonitor.left && rc.top <= mi.rcMonitor.top &&
           rc.right >= mi.rcMonitor.right && rc.bottom >= mi.rcMonitor.bottom;
}

// Per-user SID & DPAPI helpers (ensures encrypted per-account clipboard history)
std::wstring CurrentUserSid() {
    static std::wstring sid;
    if (!sid.empty()) return sid;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) && token) {
        DWORD size = 0;
        GetTokenInformation(token, TokenUser, nullptr, 0, &size);
        if (size > 0) {
            std::vector<BYTE> buf(size);
            auto* user = reinterpret_cast<TOKEN_USER*>(buf.data());
            if (GetTokenInformation(token, TokenUser, user, size, &size) &&
                user->User.Sid) {
                LPWSTR str = nullptr;
                if (ConvertSidToStringSidW(user->User.Sid, &str) && str) {
                    sid = str;
                    LocalFree(str);
                }
            }
        }
        CloseHandle(token);
    }
    if (sid.empty()) sid = L"shared";
    return sid;
}

bool ProtectBytes(const std::vector<BYTE>& in, std::vector<BYTE>& out) {
    DATA_BLOB src{static_cast<DWORD>(in.size()), const_cast<BYTE*>(in.data())};
    DATA_BLOB dst{};
    if (!CryptProtectData(&src, L"Tahoe Spotlight", nullptr, nullptr, nullptr,
                          CRYPTPROTECT_UI_FORBIDDEN, &dst)) {
        return false;
    }
    out.assign(dst.pbData, dst.pbData + dst.cbData);
    LocalFree(dst.pbData);
    return true;
}

bool UnprotectBytes(const std::vector<BYTE>& in, std::vector<BYTE>& out) {
    DATA_BLOB src{static_cast<DWORD>(in.size()), const_cast<BYTE*>(in.data())};
    DATA_BLOB dst{};
    if (!CryptUnprotectData(&src, nullptr, nullptr, nullptr, nullptr,
                            CRYPTPROTECT_UI_FORBIDDEN, &dst)) {
        return false;
    }
    out.assign(dst.pbData, dst.pbData + dst.cbData);
    LocalFree(dst.pbData);
    return true;
}

std::string Base64Encode(const std::vector<BYTE>& in) {
    static constexpr char kAlphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((in.size() + 2) / 3 * 4);
    for (size_t i = 0; i < in.size(); i += 3) {
        uint32_t b0 = in[i];
        uint32_t b1 = i + 1 < in.size() ? in[i + 1] : 0;
        uint32_t b2 = i + 2 < in.size() ? in[i + 2] : 0;
        uint32_t v = (b0 << 16) | (b1 << 8) | b2;
        out.push_back(kAlphabet[(v >> 18) & 63]);
        out.push_back(kAlphabet[(v >> 12) & 63]);
        out.push_back(i + 1 < in.size() ? kAlphabet[(v >> 6) & 63] : '=');
        out.push_back(i + 2 < in.size() ? kAlphabet[v & 63] : '=');
    }
    return out;
}

bool Base64Decode(const std::string& in, std::vector<BYTE>& out) {
    auto val = [](char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    };
    out.clear();
    out.reserve(in.size() / 4 * 3);
    uint32_t acc = 0;
    int bits = 0;
    for (char c : in) {
        if (c == '=' || c == '\0') break;
        int v = val(c);
        if (v < 0) return false;
        acc = (acc << 6) | static_cast<uint32_t>(v);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<BYTE>((acc >> bits) & 0xFF));
        }
    }
    return true;
}

std::wstring ProtectText(const std::wstring& plain) {
    const BYTE* p = reinterpret_cast<const BYTE*>(plain.data());
    std::vector<BYTE> enc;
    if (!ProtectBytes(
            std::vector<BYTE>(p, p + plain.size() * sizeof(wchar_t)), enc)) {
        return {};
    }
    std::string b64 = Base64Encode(enc);
    return L"e" + std::wstring(b64.begin(), b64.end());
}

bool UnprotectText(const std::wstring& stored, std::wstring& plain) {
    plain.clear();
    if (stored.empty()) return false;
    if (stored[0] != L'e') {
        plain = stored;
        return true;
    }
    std::vector<BYTE> enc, bytes;
    std::string b64(stored.begin() + 1, stored.end());
    if (!Base64Decode(b64, enc) || !UnprotectBytes(enc, bytes)) return false;
    if (bytes.size() % sizeof(wchar_t) != 0) return false;
    plain.assign(reinterpret_cast<const wchar_t*>(bytes.data()),
                 bytes.size() / sizeof(wchar_t));
    return true;
}

std::wstring g_clipDir;
BOOL CALLBACK ClipDirInitProc(PINIT_ONCE, PVOID, PVOID*) {
    wchar_t buf[MAX_PATH]{};
    if (Wh_GetModStoragePath(buf, ARRAYSIZE(buf)) > 0 && buf[0]) {
        g_clipDir = std::wstring(buf) + L"\\clips\\" + CurrentUserSid();
    }
    return TRUE;
}
std::wstring ClipImageDir() {
    static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
    InitOnceExecuteOnce(&once, ClipDirInitProc, nullptr, nullptr);
    return g_clipDir;
}
std::wstring ClipImagePath(const std::wstring& stored) {
    size_t bar = stored.find(L'|');
    if (bar == std::wstring::npos || bar == 0) return {};
    std::wstring dir = ClipImageDir();
    return dir.empty() ? std::wstring() : dir + L"\\" + stored.substr(0, bar);
}

bool ReadFileBytesRaw(const std::wstring& path,
                      std::vector<BYTE>& out,
                      size_t maxBytes) {
    HANDLE h = CreateFileW(
        path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER sz{};
    bool ok = GetFileSizeEx(h, &sz) && sz.QuadPart > 0 &&
              static_cast<uint64_t>(sz.QuadPart) <= maxBytes;
    if (ok) {
        out.resize(static_cast<size_t>(sz.QuadPart));
        DWORD got = 0;
        ok = ReadFile(h, out.data(), static_cast<DWORD>(out.size()), &got,
                      nullptr) &&
             got > 0;
        if (ok) out.resize(got);
    }
    CloseHandle(h);
    return ok;
}

bool WriteFileBytesRaw(const std::wstring& path, const std::vector<BYTE>& data) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    BOOL ok = WriteFile(h, data.data(), static_cast<DWORD>(data.size()),
                        &written, nullptr);
    CloseHandle(h);
    return ok && written == data.size();
}

void LoadSettings() {
    SpotlightSettings s;
    std::wstring app = ReadWhStringSetting(L"appearance");
    if (app == L"dark") {
        s.appearance = AppearanceMode::Dark;
    } else if (app == L"light") {
        s.appearance = AppearanceMode::Light;
    } else {
        s.appearance = AppearanceMode::Auto;
    }

    s.glassStyle = (ReadWhStringSetting(L"glassStyle") == L"tinted")
                       ? GlassStyle::Tinted
                       : GlassStyle::Clear;

    std::wstring ref = ReadWhStringSetting(L"glassRefraction");
    if (ref == L"reflection") {
        s.glassRefraction = GlassRefraction::Reflection;
    } else if (ref == L"stretch") {
        s.glassRefraction = GlassRefraction::Stretch;
    } else if (ref == L"off") {
        s.glassRefraction = GlassRefraction::Off;
    } else {
        s.glassRefraction = GlassRefraction::Lens;
    }

    s.glassEdgeDepth = std::clamp(Wh_GetIntSetting(L"glassEdgeDepth"), 0, 100);
    s.glassEdgeStrength =
        std::clamp(Wh_GetIntSetting(L"glassEdgeStrength"), 0, 100);
    s.glassTint = std::clamp(Wh_GetIntSetting(L"glassTint"), 0, 100);
    s.glassBlur = std::clamp(Wh_GetIntSetting(L"glassBlur"), 0, 100);
    s.glassBrightnessLimit =
        std::clamp(Wh_GetIntSetting(L"glassBrightnessLimit"), 0, 100);

    std::wstring hk = ReadWhStringSetting(L"hotkeyCombo");
    if (hk == L"ctrlSpace") {
        s.hotkeyCombo = HotkeyCombo::CtrlSpace;
    } else if (hk == L"ctrlAltSpace") {
        s.hotkeyCombo = HotkeyCombo::CtrlAltSpace;
    } else if (hk == L"shiftAltSpace") {
        s.hotkeyCombo = HotkeyCombo::ShiftAltSpace;
    } else if (hk == L"winShiftS") {
        s.hotkeyCombo = HotkeyCombo::WinShiftS;
    } else {
        s.hotkeyCombo = HotkeyCombo::AltSpace;
    }
    s.overrideBusyHotkey = Wh_GetIntSetting(L"overrideBusyHotkey") != 0;

    std::wstring mon = ReadWhStringSetting(L"openMonitor");
    if (mon == L"foreground") {
        s.openMonitor = OpenMonitor::Foreground;
    } else if (mon == L"primary") {
        s.openMonitor = OpenMonitor::Primary;
    } else {
        s.openMonitor = OpenMonitor::Cursor;
    }

    s.verticalPosition =
        std::clamp(Wh_GetIntSetting(L"verticalPosition"), 0, 60);
    s.disableInFullscreen = Wh_GetIntSetting(L"disableInFullscreen") != 0;
    s.enableEverything = Wh_GetIntSetting(L"enableEverything") != 0;
    s.enableClipboardHistory = Wh_GetIntSetting(L"enableClipboardHistory") != 0;
    s.clipRetentionDays =
        std::clamp(Wh_GetIntSetting(L"clipRetentionDays"), 0, 365);
    s.enableBookmarks = Wh_GetIntSetting(L"enableBookmarks") != 0;
    s.enableCalculator = Wh_GetIntSetting(L"enableCalculator") != 0;
    s.enableWebSearch = Wh_GetIntSetting(L"enableWebSearch") != 0;

    std::wstring we = ReadWhStringSetting(L"webEngine");
    if (we == L"duckduckgo") {
        s.webEngine = WebEngine::DuckDuckGo;
    } else if (we == L"bing") {
        s.webEngine = WebEngine::Bing;
    } else if (we == L"youtube") {
        s.webEngine = WebEngine::YouTube;
    } else if (we == L"wikipedia") {
        s.webEngine = WebEngine::Wikipedia;
    } else {
        s.webEngine = WebEngine::Google;
    }

    s.showQuickLookByDefault =
        Wh_GetIntSetting(L"showQuickLookByDefault") != 0;
    g_settings = std::move(s);
}

// ===== Inline Calculator & Unit/Base Converter =====
namespace Calc {

struct Locale {
    wchar_t decimal = L'.';
    wchar_t group = L',';
    bool grouping = true;
};

inline Locale SystemLocale() {
    Locale l;
    wchar_t buf[8]{};
    if (GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, buf,
                       ARRAYSIZE(buf)) > 1) {
        l.decimal = buf[0];
    }
    int n = GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, buf,
                           ARRAYSIZE(buf));
    if (n > 1) {
        l.group = buf[0];
    } else {
        l.grouping = false;
    }
    if (l.group == l.decimal) l.grouping = false;
    return l;
}

enum class Status { NotMath, Ok, Error };

struct Result {
    Status status = Status::NotMath;
    double value = 0.0;
    std::wstring error;
    std::wstring display;
    std::wstring insertable;
    bool loneConstant = false;
};

struct Context {
    bool hasAns = false;
    double ans = 0.0;
};

struct UnitDef {
    const wchar_t* name;
    const wchar_t* sym;
    int dim;
    double factor;
    double offset;
};

enum {
    DIM_LENGTH = 0,
    DIM_MASS,
    DIM_TIME,
    DIM_DATA,
    DIM_VOLUME,
    DIM_SPEED,
    DIM_AREA,
    DIM_ANGLE,
    DIM_PRESSURE,
    DIM_ENERGY,
    DIM_POWER,
    DIM_TEMP,
};

inline const std::vector<UnitDef>& UnitTable() {
    static const std::vector<UnitDef> t = {
        {L"nm", L"nm", DIM_LENGTH, 1e-9, 0},
        {L"um", L"\u00B5m", DIM_LENGTH, 1e-6, 0},
        {L"mm", L"mm", DIM_LENGTH, 1e-3, 0},
        {L"cm", L"cm", DIM_LENGTH, 1e-2, 0},
        {L"dm", L"dm", DIM_LENGTH, 0.1, 0},
        {L"m", L"m", DIM_LENGTH, 1, 0},
        {L"meter", L"m", DIM_LENGTH, 1, 0},
        {L"metre", L"m", DIM_LENGTH, 1, 0},
        {L"km", L"km", DIM_LENGTH, 1000, 0},
        {L"in", L"in", DIM_LENGTH, 0.0254, 0},
        {L"inch", L"in", DIM_LENGTH, 0.0254, 0},
        {L"inches", L"in", DIM_LENGTH, 0.0254, 0},
        {L"ft", L"ft", DIM_LENGTH, 0.3048, 0},
        {L"foot", L"ft", DIM_LENGTH, 0.3048, 0},
        {L"feet", L"ft", DIM_LENGTH, 0.3048, 0},
        {L"yd", L"yd", DIM_LENGTH, 0.9144, 0},
        {L"yard", L"yd", DIM_LENGTH, 0.9144, 0},
        {L"mi", L"mi", DIM_LENGTH, 1609.344, 0},
        {L"mile", L"mi", DIM_LENGTH, 1609.344, 0},
        {L"nmi", L"nmi", DIM_LENGTH, 1852, 0},
        {L"mg", L"mg", DIM_MASS, 1e-6, 0},
        {L"g", L"g", DIM_MASS, 1e-3, 0},
        {L"kg", L"kg", DIM_MASS, 1, 0},
        {L"t", L"t", DIM_MASS, 1000, 0},
        {L"tonne", L"t", DIM_MASS, 1000, 0},
        {L"lb", L"lb", DIM_MASS, 0.45359237, 0},
        {L"lbs", L"lb", DIM_MASS, 0.45359237, 0},
        {L"pound", L"lb", DIM_MASS, 0.45359237, 0},
        {L"oz", L"oz", DIM_MASS, 0.028349523125, 0},
        {L"ounce", L"oz", DIM_MASS, 0.028349523125, 0},
        {L"st", L"st", DIM_MASS, 6.35029318, 0},
        {L"stone", L"st", DIM_MASS, 6.35029318, 0},
        {L"ns", L"ns", DIM_TIME, 1e-9, 0},
        {L"us", L"\u00B5s", DIM_TIME, 1e-6, 0},
        {L"ms", L"ms", DIM_TIME, 1e-3, 0},
        {L"s", L"s", DIM_TIME, 1, 0},
        {L"sec", L"s", DIM_TIME, 1, 0},
        {L"second", L"s", DIM_TIME, 1, 0},
        {L"min", L"min", DIM_TIME, 60, 0},
        {L"minute", L"min", DIM_TIME, 60, 0},
        {L"h", L"h", DIM_TIME, 3600, 0},
        {L"hr", L"h", DIM_TIME, 3600, 0},
        {L"hour", L"h", DIM_TIME, 3600, 0},
        {L"day", L"d", DIM_TIME, 86400, 0},
        {L"week", L"wk", DIM_TIME, 604800, 0},
        {L"year", L"yr", DIM_TIME, 31557600, 0},
        {L"bit", L"bit", DIM_DATA, 0.125, 0},
        {L"byte", L"B", DIM_DATA, 1, 0},
        {L"b", L"B", DIM_DATA, 1, 0},
        {L"kb", L"KB", DIM_DATA, 1000, 0},
        {L"mb", L"MB", DIM_DATA, 1e6, 0},
        {L"gb", L"GB", DIM_DATA, 1e9, 0},
        {L"tb", L"TB", DIM_DATA, 1e12, 0},
        {L"pb", L"PB", DIM_DATA, 1e15, 0},
        {L"kib", L"KiB", DIM_DATA, 1024, 0},
        {L"mib", L"MiB", DIM_DATA, 1048576, 0},
        {L"gib", L"GiB", DIM_DATA, 1073741824.0, 0},
        {L"tib", L"TiB", DIM_DATA, 1099511627776.0, 0},
        {L"ml", L"mL", DIM_VOLUME, 1e-3, 0},
        {L"l", L"L", DIM_VOLUME, 1, 0},
        {L"liter", L"L", DIM_VOLUME, 1, 0},
        {L"litre", L"L", DIM_VOLUME, 1, 0},
        {L"gal", L"gal", DIM_VOLUME, 3.785411784, 0},
        {L"gallon", L"gal", DIM_VOLUME, 3.785411784, 0},
        {L"qt", L"qt", DIM_VOLUME, 0.946352946, 0},
        {L"pt", L"pt", DIM_VOLUME, 0.473176473, 0},
        {L"cup", L"cup", DIM_VOLUME, 0.2365882365, 0},
        {L"floz", L"fl oz", DIM_VOLUME, 0.0295735295625, 0},
        {L"mps", L"m/s", DIM_SPEED, 1, 0},
        {L"kph", L"km/h", DIM_SPEED, 1000.0 / 3600, 0},
        {L"kmh", L"km/h", DIM_SPEED, 1000.0 / 3600, 0},
        {L"mph", L"mph", DIM_SPEED, 0.44704, 0},
        {L"knot", L"kn", DIM_SPEED, 1000.0 / 3600 * 1.852, 0},
        {L"fps", L"ft/s", DIM_SPEED, 0.3048, 0},
        {L"m2", L"m\u00B2", DIM_AREA, 1, 0},
        {L"sqm", L"m\u00B2", DIM_AREA, 1, 0},
        {L"sqft", L"ft\u00B2", DIM_AREA, 0.09290304, 0},
        {L"km2", L"km\u00B2", DIM_AREA, 1e6, 0},
        {L"ha", L"ha", DIM_AREA, 10000, 0},
        {L"acre", L"ac", DIM_AREA, 4046.8564224, 0},
        {L"rad", L"rad", DIM_ANGLE, 1, 0},
        {L"deg", L"\u00B0", DIM_ANGLE, 3.14159265358979323846 / 180, 0},
        {L"degree", L"\u00B0", DIM_ANGLE, 3.14159265358979323846 / 180, 0},
        {L"pa", L"Pa", DIM_PRESSURE, 1, 0},
        {L"kpa", L"kPa", DIM_PRESSURE, 1000, 0},
        {L"bar", L"bar", DIM_PRESSURE, 1e5, 0},
        {L"atm", L"atm", DIM_PRESSURE, 101325, 0},
        {L"psi", L"psi", DIM_PRESSURE, 6894.757293168, 0},
        {L"j", L"J", DIM_ENERGY, 1, 0},
        {L"kj", L"kJ", DIM_ENERGY, 1000, 0},
        {L"cal", L"cal", DIM_ENERGY, 4.184, 0},
        {L"kcal", L"kcal", DIM_ENERGY, 4184, 0},
        {L"wh", L"Wh", DIM_ENERGY, 3600, 0},
        {L"kwh", L"kWh", DIM_ENERGY, 3.6e6, 0},
        {L"btu", L"BTU", DIM_ENERGY, 1055.05585262, 0},
        {L"w", L"W", DIM_POWER, 1, 0},
        {L"kw", L"kW", DIM_POWER, 1000, 0},
        {L"hp", L"hp", DIM_POWER, 745.6998715823, 0},
        {L"c", L"\u00B0C", DIM_TEMP, 1, 273.15},
        {L"celsius", L"\u00B0C", DIM_TEMP, 1, 273.15},
        {L"f", L"\u00B0F", DIM_TEMP, 5.0 / 9, 459.67 * 5.0 / 9},
        {L"fahrenheit", L"\u00B0F", DIM_TEMP, 5.0 / 9, 459.67 * 5.0 / 9},
        {L"k", L"K", DIM_TEMP, 1, 0},
        {L"kelvin", L"K", DIM_TEMP, 1, 0},
    };
    return t;
}

namespace detail {

enum class Tk { Num, Ident, Op, LP, RP, End };
struct Token {
    Tk kind = Tk::End;
    double num = 0.0;
    std::wstring id;
    wchar_t op = 0;
};
struct Value {
    double v = 0.0;
    bool percent = false;
};

inline bool IsDigit(wchar_t c) { return c >= L'0' && c <= L'9'; }
inline int DigitVal(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}
inline bool IsAlpha(wchar_t c) {
    return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
}
inline bool IsAlnum(wchar_t c) { return IsAlpha(c) || IsDigit(c); }

inline bool IsConstant(const std::wstring& id, double& out) {
    if (id == L"pi" || id == L"\u03c0") {
        out = 3.14159265358979323846;
        return true;
    }
    if (id == L"e") {
        out = 2.71828182845904523536;
        return true;
    }
    if (id == L"tau") {
        out = 6.28318530717958647692;
        return true;
    }
    if (id == L"phi") {
        out = 1.61803398874989484820;
        return true;
    }
    return false;
}

inline bool IsKeyword(const std::wstring& id) {
    return id == L"mod" || id == L"of";
}

inline bool IsFunction(const std::wstring& id) {
    static constexpr const wchar_t* kFuncs[] = {
        L"sqrt", L"cbrt", L"abs", L"round", L"floor", L"ceil",
        L"trunc", L"sign", L"sin", L"cos", L"tan", L"asin",
        L"acos", L"atan", L"sinh", L"cosh", L"tanh", L"log",
        L"ln", L"log2", L"exp", L"fact",
    };
    for (const wchar_t* f : kFuncs) {
        if (id == f) return true;
    }
    return false;
}

inline bool ParseCleanDecimal(const std::wstring& s, double& out) {
    size_t i = 0;
    bool neg = false;
    if (i < s.size() && (s[i] == L'-' || s[i] == L'+')) {
        neg = (s[i] == L'-');
        ++i;
    }
    double v = 0.0;
    bool any = false;
    while (i < s.size() && IsDigit(s[i])) {
        v = v * 10.0 + (s[i] - L'0');
        any = true;
        ++i;
    }
    if (i < s.size() && s[i] == L'.') {
        ++i;
        double scale = 0.1;
        while (i < s.size() && IsDigit(s[i])) {
            v += (s[i] - L'0') * scale;
            scale *= 0.1;
            any = true;
            ++i;
        }
    }
    if (!any || i != s.size()) return false;
    out = neg ? -v : v;
    return true;
}

inline int CountChar(const std::wstring& s, wchar_t c) {
    int n = 0;
    for (wchar_t x : s) {
        if (x == c) ++n;
    }
    return n;
}

inline bool ParseNumberLiteral(const std::wstring& run,
                               const Locale& loc,
                               double& out) {
    wchar_t dec = loc.decimal;
    wchar_t alt = (dec == L'.') ? L',' : L'.';
    int decCount = CountChar(run, dec);
    int altCount = CountChar(run, alt);
    bool altIsDecimal = false;
    if (decCount == 0 && altCount == 1) {
        size_t p = run.find(alt);
        size_t before = p;
        size_t after = run.size() - p - 1;
        altIsDecimal = !(after == 3 && before >= 1 && before <= 3);
    }
    std::wstring cleaned;
    bool hasDecimal = false;
    for (wchar_t c : run) {
        if (IsDigit(c)) {
            cleaned += c;
        } else if (c == dec) {
            if (hasDecimal) return false;
            cleaned += L'.';
            hasDecimal = true;
        } else if (c == alt) {
            if (altIsDecimal) {
                if (hasDecimal) return false;
                cleaned += L'.';
                hasDecimal = true;
            }
        } else {
            return false;
        }
    }
    if (cleaned.empty() || cleaned == L".") return false;
    return ParseCleanDecimal(cleaned, out);
}

inline wchar_t NormalizeMathChar(wchar_t c) {
    switch (c) {
        case 0x00A0:
        case 0x2007:
        case 0x2008:
        case 0x2009:
        case 0x202F:
        case 0x205F:
        case 0x3000:
            return L' ';
        case 0x2212:
        case 0x2013:
        case 0x2014:
        case 0x2011:
        case 0xFE63:
        case 0xFF0D:
            return L'-';
        case 0x00D7:
        case 0x2217:
        case 0x22C5:
        case 0x00B7:
        case 0xFF0A:
            return L'*';
        case 0x00F7:
        case 0x2215:
        case 0x2044:
        case 0xFF0F:
            return L'/';
        case 0xFF0B:
            return L'+';
        case 0xFF05:
            return L'%';
        case 0xFF01:
            return L'!';
        case 0xFF08:
            return L'(';
        case 0xFF09:
            return L')';
        case 0xFF3E:
            return L'^';
        default:
            return c;
    }
}

struct Lexed {
    std::vector<Token> toks;
    bool ok = true;
    bool mathy = false;
    bool knownIdents = true;
};

inline Lexed Lex(const std::wstring& in, const Locale& loc, bool hasAns) {
    Lexed out;
    bool sawKeyword = false;
    size_t i = 0;
    const size_t n = in.size();
    while (i < n) {
        wchar_t c = NormalizeMathChar(in[i]);
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            ++i;
            continue;
        }
        if (c == L'0' && i + 1 < n) {
            wchar_t p = in[i + 1];
            int base = (p == L'x' || p == L'X')   ? 16
                       : (p == L'b' || p == L'B') ? 2
                       : (p == L'o' || p == L'O') ? 8
                                                  : 0;
            if (base) {
                size_t j = i + 2;
                double val = 0.0;
                bool any = false;
                for (; j < n; ++j) {
                    int d = DigitVal(in[j]);
                    if (d < 0 || d >= base) break;
                    val = val * base + d;
                    any = true;
                }
                if (any) {
                    Token t;
                    t.kind = Tk::Num;
                    t.num = val;
                    out.toks.push_back(t);
                    out.mathy = true;
                    i = j;
                    continue;
                }
            }
        }
        if (IsDigit(c) ||
            ((c == loc.decimal || c == (loc.decimal == L'.' ? L',' : L'.')) &&
             i + 1 < n && IsDigit(in[i + 1]))) {
            size_t start = i;
            while (i < n && (IsDigit(in[i]) || in[i] == L'.' || in[i] == L',')) {
                ++i;
            }
            std::wstring run = in.substr(start, i - start);
            double val = 0.0;
            if (!ParseNumberLiteral(run, loc, val)) {
                out.ok = false;
                return out;
            }
            if (i < n && (in[i] == L'e' || in[i] == L'E')) {
                size_t save = i;
                size_t j = i + 1;
                if (j < n && (in[j] == L'+' || in[j] == L'-')) ++j;
                if (j < n && IsDigit(in[j])) {
                    while (j < n && IsDigit(in[j])) ++j;
                    double ev = 0.0;
                    if (ParseCleanDecimal(in.substr(save + 1, j - save - 1),
                                          ev)) {
                        val *= std::pow(10.0, ev);
                        out.mathy = true;
                        i = j;
                    }
                }
            }
            Token t;
            t.kind = Tk::Num;
            t.num = val;
            out.toks.push_back(t);
            continue;
        }
        if (IsAlpha(c)) {
            size_t start = i;
            while (i < n && IsAlnum(in[i])) ++i;
            std::wstring id = in.substr(start, i - start);
            for (wchar_t& ch : id) {
                if (ch >= L'A' && ch <= L'Z') {
                    ch = static_cast<wchar_t>(ch - L'A' + L'a');
                }
            }
            double cval = 0.0;
            bool isKw = IsKeyword(id);
            if (!IsFunction(id) && !isKw && !IsConstant(id, cval) &&
                !(hasAns && id == L"ans")) {
                out.knownIdents = false;
            }
            if (isKw) sawKeyword = true;
            Token t;
            t.kind = Tk::Ident;
            t.id = std::move(id);
            out.toks.push_back(std::move(t));
            continue;
        }
        if (c == L'+' || c == L'-' || c == L'*' || c == L'/' || c == L'^' ||
            c == L'%' || c == L'!') {
            Token t;
            t.kind = Tk::Op;
            t.op = c;
            out.toks.push_back(t);
            out.mathy = true;
            ++i;
            continue;
        }
        if (c == L'(' || c == L')') {
            Token t;
            t.kind = (c == L'(') ? Tk::LP : Tk::RP;
            out.toks.push_back(t);
            out.mathy = true;
            ++i;
            continue;
        }
        out.ok = false;
        return out;
    }
    if (sawKeyword) {
        for (const Token& tk : out.toks) {
            if (tk.kind == Tk::Num) {
                out.mathy = true;
                break;
            }
        }
    }
    out.toks.push_back(Token{Tk::End, 0.0, {}, 0});
    return out;
}

struct Parser {
    const std::vector<Token>& t;
    size_t i = 0;
    bool ok = true;
    std::wstring err;
    Context ctx;

    Parser(const std::vector<Token>& toks, const Context& c) : t(toks), ctx(c) {}
    const Token& Cur() const { return t[i]; }
    void Adv() {
        if (i + 1 < t.size()) ++i;
    }
    void Fail(const std::wstring& m) {
        if (ok) {
            ok = false;
            err = m;
        }
    }
    double Factorial(double x) {
        if (x < 0.0 || x != std::floor(x) || x > 170.0) {
            Fail(L"factorial requires integer 0..170");
            return 0.0;
        }
        double r = 1.0;
        for (int k = 2; k <= static_cast<int>(x); ++k) r *= k;
        return r;
    }
    double CallFunc(const std::wstring& id, double x) {
        if (id == L"sqrt") {
            if (x < 0) {
                Fail(L"square root of negative number");
                return 0.0;
            }
            return std::sqrt(x);
        }
        if (id == L"cbrt") return std::cbrt(x);
        if (id == L"abs") return std::fabs(x);
        if (id == L"round") return std::round(x);
        if (id == L"floor") return std::floor(x);
        if (id == L"ceil") return std::ceil(x);
        if (id == L"trunc") return std::trunc(x);
        if (id == L"sign") return static_cast<double>((x > 0) - (x < 0));
        if (id == L"sin") return std::sin(x);
        if (id == L"cos") return std::cos(x);
        if (id == L"tan") return std::tan(x);
        if (id == L"asin") {
            if (x < -1 || x > 1) {
                Fail(L"asin requires [-1, 1]");
                return 0.0;
            }
            return std::asin(x);
        }
        if (id == L"acos") {
            if (x < -1 || x > 1) {
                Fail(L"acos requires [-1, 1]");
                return 0.0;
            }
            return std::acos(x);
        }
        if (id == L"atan") return std::atan(x);
        if (id == L"sinh") return std::sinh(x);
        if (id == L"cosh") return std::cosh(x);
        if (id == L"tanh") return std::tanh(x);
        if (id == L"log") {
            if (x <= 0) {
                Fail(L"log requires positive number");
                return 0.0;
            }
            return std::log10(x);
        }
        if (id == L"ln") {
            if (x <= 0) {
                Fail(L"ln requires positive number");
                return 0.0;
            }
            return std::log(x);
        }
        if (id == L"log2") {
            if (x <= 0) {
                Fail(L"log2 requires positive number");
                return 0.0;
            }
            return std::log2(x);
        }
        if (id == L"exp") return std::exp(x);
        if (id == L"fact") return Factorial(x);
        Fail(L"unknown function");
        return 0.0;
    }
    Value ParsePrimary() {
        Value r;
        const Token& c = Cur();
        if (c.kind == Tk::Num) {
            r.v = c.num;
            Adv();
            return r;
        }
        if (c.kind == Tk::LP) {
            Adv();
            r = ParseExpr();
            if (Cur().kind != Tk::RP) {
                Fail(L"missing ')'");
                return r;
            }
            Adv();
            return r;
        }
        if (c.kind == Tk::Ident) {
            std::wstring id = c.id;
            Adv();
            if (Cur().kind == Tk::LP) {
                Adv();
                Value a = ParseExpr();
                if (Cur().kind != Tk::RP) {
                    Fail(L"missing ')'");
                    return r;
                }
                Adv();
                r.v = CallFunc(id, a.v);
                return r;
            }
            double cv = 0.0;
            if (IsConstant(id, cv)) {
                r.v = cv;
                return r;
            }
            if (id == L"ans" && ctx.hasAns) {
                r.v = ctx.ans;
                return r;
            }
            Fail(L"unexpected identifier");
            return r;
        }
        Fail(L"unexpected token");
        return r;
    }
    Value ParsePostfix() {
        Value r = ParsePrimary();
        while (Cur().kind == Tk::Op && (Cur().op == L'%' || Cur().op == L'!')) {
            wchar_t op = Cur().op;
            Adv();
            if (op == L'%') {
                r.v /= 100.0;
                r.percent = true;
            } else {
                r.v = Factorial(r.v);
                r.percent = false;
            }
        }
        return r;
    }
    Value ParsePower() {
        Value base = ParsePostfix();
        if (Cur().kind == Tk::Op && Cur().op == L'^') {
            Adv();
            Value ex = ParseUnary();
            return Value{std::pow(base.v, ex.v), false};
        }
        return base;
    }
    Value ParseUnary() {
        if (Cur().kind == Tk::Op && (Cur().op == L'+' || Cur().op == L'-')) {
            wchar_t op = Cur().op;
            Adv();
            Value u = ParseUnary();
            u.v = (op == L'-') ? -u.v : u.v;
            u.percent = false;
            return u;
        }
        return ParsePower();
    }
    Value ParseTerm() {
        Value left = ParseUnary();
        while (true) {
            const Token& c = Cur();
            bool isMul = (c.kind == Tk::Op && (c.op == L'*' || c.op == L'/'));
            bool isWord = (c.kind == Tk::Ident && IsKeyword(c.id));
            if (!isMul && !isWord) break;
            wchar_t op = isMul ? c.op : 0;
            std::wstring word = isMul ? std::wstring() : c.id;
            Adv();
            Value right = ParseUnary();
            if (isMul) {
                if (op == L'*') {
                    left.v *= right.v;
                } else {
                    if (right.v == 0.0) Fail(L"division by zero");
                    else left.v /= right.v;
                }
            } else if (word == L"mod") {
                if (right.v == 0.0) Fail(L"division by zero");
                else left.v = std::fmod(left.v, right.v);
            } else {
                left.v *= right.v;
            }
            left.percent = false;
        }
        return left;
    }
    Value ParseExpr() {
        Value left = ParseTerm();
        while (Cur().kind == Tk::Op && (Cur().op == L'+' || Cur().op == L'-')) {
            wchar_t op = Cur().op;
            Adv();
            Value right = ParseTerm();
            double rhs = right.percent ? (left.v * right.v) : right.v;
            left.v = (op == L'+') ? (left.v + rhs) : (left.v - rhs);
            left.percent = false;
        }
        return left;
    }
};

inline std::wstring GroupInt(const std::wstring& digits, const Locale& loc) {
    bool neg = !digits.empty() && digits[0] == L'-';
    std::wstring d = neg ? digits.substr(1) : digits;
    if (!loc.grouping || d.size() <= 3) return digits;
    std::wstring out;
    int cnt = 0;
    for (int i = static_cast<int>(d.size()) - 1; i >= 0; --i) {
        out.push_back(d[static_cast<size_t>(i)]);
        if (++cnt % 3 == 0 && i > 0) out.push_back(loc.group);
    }
    std::reverse(out.begin(), out.end());
    return neg ? (L"-" + out) : out;
}

}  // namespace detail

inline std::wstring Format(double v, const Locale& loc) {
    if (!std::isfinite(v)) return L"-";
    bool integral = std::fabs(v) < 1e15 &&
                    std::fabs(v - std::round(v)) <
                        1e-9 * std::max(1.0, std::fabs(v));
    if (integral) {
        long long iv = std::llround(v);
        wchar_t buf[32]{};
        swprintf_s(buf, L"%lld", iv);
        return detail::GroupInt(buf, loc);
    }
    wchar_t buf[64]{};
    swprintf_s(buf, L"%.12g", v);
    std::wstring s = buf;
    std::wstring mant = s, exp;
    if (size_t epos = s.find_first_of(L"eE"); epos != std::wstring::npos) {
        mant = s.substr(0, epos);
        exp = s.substr(epos);
    }
    std::wstring ip = mant, fp;
    if (size_t dot = mant.find(L'.'); dot != std::wstring::npos) {
        ip = mant.substr(0, dot);
        fp = mant.substr(dot + 1);
    }
    std::wstring out = detail::GroupInt(ip, loc);
    if (!fp.empty()) {
        out += loc.decimal;
        out += fp;
    }
    out += exp;
    return out;
}

inline Result EvalCore(const std::wstring& input,
                       const Locale& loc,
                       const Context& ctx,
                       bool requireMathy) {
    Result res;
    auto lx = detail::Lex(input, loc, ctx.hasAns);
    if (!lx.ok || !lx.knownIdents) return res;
    const detail::Token* only = nullptr;
    size_t count = 0;
    for (const auto& t : lx.toks) {
        if (t.kind == detail::Tk::End) continue;
        only = &t;
        ++count;
    }
    if (count == 0) return res;
    bool loneConst = false;
    if (count == 1 && only->kind == detail::Tk::Ident) {
        double cv = 0.0;
        loneConst =
            only->id.size() >= 2 &&
            (detail::IsConstant(only->id, cv) || (ctx.hasAns && only->id == L"ans"));
    }
    if (requireMathy && !lx.mathy && !loneConst) return res;
    detail::Parser p(lx.toks, ctx);
    detail::Value v = p.ParseExpr();
    if (!p.ok) {
        res.status = Status::Error;
        res.error = p.err;
        return res;
    }
    if (p.Cur().kind != detail::Tk::End) {
        res.status = Status::Error;
        res.error = L"incomplete expression";
        return res;
    }
    if (!std::isfinite(v.v)) {
        res.status = Status::Error;
        res.error = L"result out of range";
        return res;
    }
    res.status = Status::Ok;
    res.value = v.v;
    res.loneConstant = loneConst;
    return res;
}

inline const UnitDef* FindUnit(const std::wstring& word) {
    const auto& t = UnitTable();
    for (const auto& u : t) {
        if (word == u.name) return &u;
    }
    if (word.size() > 1 && word.back() == L's') {
        std::wstring sing = word.substr(0, word.size() - 1);
        for (const auto& u : t) {
            if (sing == u.name) return &u;
        }
    }
    return nullptr;
}

inline int BaseTargetOf(const std::wstring& w) {
    if (w == L"hex" || w == L"hexadecimal") return 16;
    if (w == L"bin" || w == L"binary") return 2;
    if (w == L"oct" || w == L"octal") return 8;
    if (w == L"dec" || w == L"decimal") return 10;
    return 0;
}

inline std::wstring FormatInBase(double v, int base) {
    if (!std::isfinite(v) || std::fabs(v) >= 18446744073709551616.0) return {};
    bool neg = v < 0;
    unsigned long long u = static_cast<unsigned long long>(neg ? -v : v);
    static constexpr wchar_t kDigits[] = L"0123456789ABCDEF";
    std::wstring s;
    do {
        s.push_back(kDigits[u % static_cast<unsigned long long>(base)]);
        u /= static_cast<unsigned long long>(base);
    } while (u);
    std::reverse(s.begin(), s.end());
    PCWSTR pre = (base == 16) ? L"0x"
               : (base == 2)  ? L"0b"
               : (base == 8)  ? L"0o"
                              : L"";
    return std::wstring(neg ? L"-" : L"") + pre + s;
}

inline Result Evaluate(const std::wstring& input,
                       const Locale& loc = SystemLocale(),
                       const Context& ctx = Context{}) {
    std::vector<std::wstring> w;
    std::wstring cur;
    for (wchar_t ch : input) {
        wchar_t c = detail::NormalizeMathChar(ch);
        if (c == L' ' || c == L'\t') {
            if (!cur.empty()) {
                w.push_back(cur);
                cur.clear();
            }
        } else {
            if (c >= L'A' && c <= L'Z') {
                c = static_cast<wchar_t>(c - L'A' + L'a');
            }
            cur.push_back(c);
        }
    }
    if (!cur.empty()) w.push_back(cur);

    auto joinWords = [&](size_t a, size_t b) {
        std::wstring s;
        for (size_t idx = a; idx < b; ++idx) {
            if (!s.empty()) s += L' ';
            s += w[idx];
        }
        return s;
    };

    if (w.size() >= 3) {
        size_t kwIdx = std::wstring::npos;
        for (size_t idx = 1; idx + 1 < w.size(); ++idx) {
            if (w[idx] == L"to" || w[idx] == L"as" || w[idx] == L"in") {
                kwIdx = idx;
                break;
            }
        }
        if (kwIdx != std::wstring::npos && kwIdx + 2 == w.size()) {
            const std::wstring& dstWord = w[kwIdx + 1];
            if (int base = BaseTargetOf(dstWord)) {
                Result l = EvalCore(joinWords(0, kwIdx), loc, ctx, false);
                if (l.status == Status::Ok) {
                    Result out;
                    if (l.value != std::floor(l.value)) {
                        out.status = Status::Error;
                        out.error = L"base conversion needs whole integer";
                        return out;
                    }
                    std::wstring fmt = FormatInBase(l.value, base);
                    if (fmt.empty()) {
                        out.status = Status::Error;
                        out.error = L"number too large";
                        return out;
                    }
                    out.status = Status::Ok;
                    out.value = l.value;
                    out.display = fmt;
                    out.insertable = fmt;
                    return out;
                }
            } else if (const UnitDef* dst = FindUnit(dstWord)) {
                size_t last = kwIdx - 1;
                const UnitDef* src = FindUnit(w[last]);
                std::wstring glueNum;
                if (!src) {
                    size_t cut = std::wstring::npos;
                    for (size_t p = 0; p < w[last].size(); ++p) {
                        if (w[last][p] >= L'a' && w[last][p] <= L'z') {
                            cut = p;
                            break;
                        }
                    }
                    if (cut != std::wstring::npos && cut > 0) {
                        if (const UnitDef* s2 = FindUnit(w[last].substr(cut))) {
                            src = s2;
                            glueNum = w[last].substr(0, cut);
                        }
                    }
                }
                if (src && src->dim == dst->dim) {
                    std::wstring left = joinWords(0, last);
                    if (!glueNum.empty()) {
                        if (!left.empty()) left += L' ';
                        left += glueNum;
                    }
                    Result l = EvalCore(left, loc, ctx, false);
                    if (l.status == Status::Ok) {
                        double baseVal = l.value * src->factor + src->offset;
                        double conv = (baseVal - dst->offset) / dst->factor;
                        Result out;
                        out.status = Status::Ok;
                        out.value = conv;
                        out.insertable = Format(conv, loc);
                        out.display = out.insertable + L" " + dst->sym;
                        return out;
                    }
                }
            }
        }
    }
    return EvalCore(input, loc, ctx, true);
}

}  // namespace Calc

// ============================================================================
// 3. Graphics Device, Tahoe Optical Liquid Glass & Typography
// ============================================================================

constexpr GUID kGaussianBlurEffectId = {
    0x1feb6d69,
    0x2fe6,
    0x4ac9,
    {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};
constexpr GUID kSaturationEffectId = {
    0x5cb2d9cf,
    0x327d,
    0x459f,
    {0xa0, 0xce, 0x40, 0xc0, 0xb2, 0x08, 0x6b, 0xf7}};
constexpr GUID kClsidD2D1DisplacementMap = {
    0xedc48364,
    0x0417,
    0x4111,
    {0x94, 0x50, 0x43, 0x84, 0x5f, 0xa9, 0xf8, 0x90}};
constexpr GUID kClsidD2D1Crop = {
    0xe23f7110,
    0x0e9a,
    0x4324,
    {0xaf, 0x47, 0x6a, 0x2c, 0x0c, 0x46, 0xf3, 0x5b}};
constexpr GUID kClsidD2D1Border = {
    0x2a2d49c0,
    0x4acf,
    0x43c7,
    {0x8c, 0x6a, 0x7c, 0x4a, 0x27, 0x87, 0x4d, 0x27}};
constexpr GUID kClsidD2D1ColorMatrix = {
    0x921f03d6,
    0x641c,
    0x47df,
    {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};
constexpr GUID kClsidD2D12DAffineTransform = {
    0x6aa97485,
    0x6354,
    0x4cfc,
    {0x90, 0x8c, 0xe4, 0xa7, 0x4f, 0x62, 0xc9, 0x6c}};

class GaussianBlurEffect
    : public winrt::implements<GaussianBlurEffect,
                               wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource,
                               ISpotlightGraphicsEffectD2D1Interop> {
public:
    GaussianBlurEffect(float blurAmount,
                       wge::IGraphicsEffectSource const& source)
        : m_blurAmount(blurAmount), m_source(source) {}
    winrt::hstring Name() const { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_POINTER;
        *id = kGaussianBlurEffectId;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        SPOTLIGHT_EFFECT_PROP_MAP* mapping) noexcept override {
        if (!name || !index || !mapping) return E_POINTER;
        if (_wcsicmp(name, L"BlurAmount") == 0) {
            *index = 0;
            *mapping = SPOTLIGHT_EFFECT_PROP_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_POINTER;
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index,
                                          void** value) noexcept override {
        if (!value) return E_POINTER;
        try {
            wf::IInspectable v;
            switch (index) {
                case 0:
                    v = wf::PropertyValue::CreateSingle(m_blurAmount);
                    break;
                case 1:
                    v = wf::PropertyValue::CreateUInt32(1);
                    break;
                case 2:
                    v = wf::PropertyValue::CreateUInt32(1);
                    break;
                default:
                    return E_INVALIDARG;
            }
            *value = winrt::detach_abi(v.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        void** source) noexcept override {
        if (!source) return E_POINTER;
        if (index != 0) return E_INVALIDARG;
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_POINTER;
        *count = 1;
        return S_OK;
    }

private:
    float m_blurAmount = 18.0f;
    wge::IGraphicsEffectSource m_source{nullptr};
    winrt::hstring m_name;
};

class SaturationEffect
    : public winrt::implements<SaturationEffect,
                               wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource,
                               ISpotlightGraphicsEffectD2D1Interop> {
public:
    SaturationEffect(float saturation, wge::IGraphicsEffectSource const& source)
        : m_saturation(saturation), m_source(source) {}
    winrt::hstring Name() const { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_POINTER;
        *id = kSaturationEffectId;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        SPOTLIGHT_EFFECT_PROP_MAP* mapping) noexcept override {
        if (!name || !index || !mapping) return E_POINTER;
        if (_wcsicmp(name, L"Saturation") == 0) {
            *index = 0;
            *mapping = SPOTLIGHT_EFFECT_PROP_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_POINTER;
        *count = 1;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index,
                                          void** value) noexcept override {
        if (!value) return E_POINTER;
        try {
            if (index != 0) return E_INVALIDARG;
            auto pv = wf::PropertyValue::CreateSingle(m_saturation);
            *value = winrt::detach_abi(pv.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        void** source) noexcept override {
        if (!source) return E_POINTER;
        if (index != 0) return E_INVALIDARG;
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_POINTER;
        *count = 1;
        return S_OK;
    }

private:
    float m_saturation = 1.0f;
    wge::IGraphicsEffectSource m_source{nullptr};
    winrt::hstring m_name;
};

wuc::CompositionBrush CreateLiquidGlassBackdropBrush(
    wuc::Compositor const& c,
    wuc::CompositionBackdropBrush const& hostBackdrop,
    float blurStdDev,
    float saturation) {
    try {
        wge::IGraphicsEffectSource src =
            wuc::CompositionEffectSourceParameter(L"backdrop");
        if (blurStdDev > 0.25f) {
            auto blur = winrt::make<GaussianBlurEffect>(blurStdDev, src);
            blur.Name(L"Blur");
            src = blur.as<wge::IGraphicsEffectSource>();
        }
        auto sat = winrt::make<SaturationEffect>(saturation, src);
        sat.Name(L"Sat");
        auto factory = c.CreateEffectFactory(sat);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"backdrop", hostBackdrop);
        return brush;
    } catch (...) {
    }
    return hostBackdrop;
}

void ClearSpotlightFonts();

struct D2DMultithreadLock {
    ID2D1Multithread* lock = nullptr;
    explicit D2DMultithreadLock(ID2D1Multithread* l) : lock(l) {
        if (lock) lock->Enter();
    }
    ~D2DMultithreadLock() {
        if (lock) lock->Leave();
    }
};

struct GraphicsContext {
    winrt::com_ptr<ID3D11Device> d3dDevice;
    winrt::com_ptr<ID3D11DeviceContext> d3dContext;
    winrt::com_ptr<ID2D1Factory1> d2dFactory;
    winrt::com_ptr<ID2D1Multithread> d2dMultithread;
    winrt::com_ptr<ID2D1Device> d2dDevice;
    winrt::com_ptr<IDWriteFactory> dwriteFactory;
    winrt::com_ptr<IWICImagingFactory> wicFactory;
    wuc::Compositor compositor{nullptr};
    wuc::CompositionGraphicsDevice compGraphics{nullptr};
    winrt::event_token deviceReplacedToken{};
    uint64_t deviceGen = 1;

    void Shutdown() {
        if (compGraphics && deviceReplacedToken.value != 0) {
            try {
                compGraphics.RenderingDeviceReplaced(deviceReplacedToken);
            } catch (...) {
            }
            deviceReplacedToken = {};
        }
        if (d3dContext) {
            d3dContext->ClearState();
            d3dContext->Flush();
        }
        compGraphics = nullptr;
        compositor = nullptr;
        wicFactory = nullptr;
        dwriteFactory = nullptr;
        d2dDevice = nullptr;
        d2dMultithread = nullptr;
        d2dFactory = nullptr;
        d3dContext = nullptr;
        d3dDevice = nullptr;
        ClearSpotlightFonts();
    }
};

GraphicsContext g_gfx;

void OnGraphicsDeviceLost();

winrt::com_ptr<IDXGIAdapter1> FindAdapterForMonitor(HMONITOR monitor) {
    winrt::com_ptr<IDXGIFactory1> factory;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1),
                                  factory.put_void())) ||
        !factory) {
        return nullptr;
    }
    winrt::com_ptr<IDXGIAdapter1> adapter;
    for (UINT i = 0; SUCCEEDED(factory->EnumAdapters1(i, adapter.put()));
         ++i, adapter = nullptr) {
        winrt::com_ptr<IDXGIOutput> output;
        for (UINT j = 0; SUCCEEDED(adapter->EnumOutputs(j, output.put()));
             ++j, output = nullptr) {
            DXGI_OUTPUT_DESC desc{};
            if (SUCCEEDED(output->GetDesc(&desc)) && desc.Monitor == monitor) {
                return adapter;
            }
        }
    }
    return nullptr;
}

bool InitD3DAndD2D() {
    g_gfx.d2dDevice = nullptr;
    if (g_gfx.d3dContext) {
        g_gfx.d3dContext->ClearState();
        g_gfx.d3dContext->Flush();
    }
    g_gfx.d3dContext = nullptr;
    g_gfx.d3dDevice = nullptr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL obtained{};
    auto adapter = FindAdapterForMonitor(GetPrimaryMonitor());
    HRESULT hr = D3D11CreateDevice(
        adapter.get(),
        adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE, nullptr,
        flags, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
        g_gfx.d3dDevice.put(), &obtained, g_gfx.d3dContext.put());
    if (FAILED(hr) && adapter) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                               flags, levels, ARRAYSIZE(levels),
                               D3D11_SDK_VERSION, g_gfx.d3dDevice.put(),
                               &obtained, g_gfx.d3dContext.put());
    }
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                               levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
                               g_gfx.d3dDevice.put(), &obtained,
                               g_gfx.d3dContext.put());
    }
    if (FAILED(hr)) {
        Wh_Log(L"Spotlight D3D11CreateDevice failed: 0x%08X",
               static_cast<unsigned>(hr));
        return false;
    }

    static constexpr GUID kIidD3D10Multithread = {
        0x9B7E4E00, 0x342C, 0x4106,
        {0xA1, 0x9F, 0x4F, 0x27, 0x04, 0xF6, 0x89, 0xF0}};
    winrt::com_ptr<ID3D10Multithread> d3dMt;
    if (SUCCEEDED(g_gfx.d3dDevice->QueryInterface(
            kIidD3D10Multithread, d3dMt.put_void())) && d3dMt) {
        d3dMt->SetMultithreadProtected(TRUE);
    }

    winrt::com_ptr<IDXGIDevice> dxgiDevice;
    if (FAILED(g_gfx.d3dDevice->QueryInterface(IID_PPV_ARGS(dxgiDevice.put())))) {
        return false;
    }
    if (!g_gfx.d2dFactory) {
        D2D1_FACTORY_OPTIONS opts{};
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                                     __uuidof(ID2D1Factory1), &opts,
                                     g_gfx.d2dFactory.put_void()))) {
            return false;
        }
        g_gfx.d2dMultithread = g_gfx.d2dFactory.try_as<ID2D1Multithread>();
    }
    if (FAILED(g_gfx.d2dFactory->CreateDevice(dxgiDevice.get(),
                                              g_gfx.d2dDevice.put()))) {
        return false;
    }
    ++g_gfx.deviceGen;
    return true;
}

bool InitGraphics(wuc::Compositor const& compositor) {
    g_gfx.compositor = compositor;
    if (!g_gfx.dwriteFactory &&
        FAILED(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(g_gfx.dwriteFactory.put())))) {
        return false;
    }
    if (!g_gfx.wicFactory &&
        FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(g_gfx.wicFactory.put())))) {
        return false;
    }
    if (!InitD3DAndD2D()) return false;
    auto interop = compositor.as<ICompositorInteropLocal>();
    if (FAILED(interop->CreateGraphicsDevice(
            g_gfx.d2dDevice.get(), winrt::put_abi(g_gfx.compGraphics))) ||
        !g_gfx.compGraphics) {
        return false;
    }
    g_gfx.deviceReplacedToken = g_gfx.compGraphics.RenderingDeviceReplaced(
        [](wuc::CompositionGraphicsDevice const&,
           wuc::RenderingDeviceReplacedEventArgs const&) {
            if (g_hostHwnd && !g_quitting) {
                PostMessageW(g_hostHwnd, kWmAppDeviceLost, 0, 0);
            }
        });
    return true;
}

template <typename Fn>
bool DrawCompositionSurface(wuc::CompositionDrawingSurface const& surface,
                            Fn&& drawFn) {
    if (!surface) return false;
    D2DMultithreadLock mtLock(g_gfx.d2dMultithread.get());
    auto interop = surface.as<ICompositionDrawingSurfaceInteropLocal>();
    winrt::com_ptr<ID2D1DeviceContext> dc;
    POINT offset{};
    if (FAILED(interop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext),
                                  dc.put_void(), &offset)) ||
        !dc) {
        return false;
    }
    bool ok = false;
    try {
        dc->SetDpi(96.0f, 96.0f);
        dc->SetTransform(D2D1::Matrix3x2F::Translation(
            static_cast<float>(offset.x), static_cast<float>(offset.y)));
        dc->Clear(D2D1::ColorF(0, 0, 0, 0));
        drawFn(dc.get());
        ok = true;
    } catch (...) {
        ok = false;
    }
    HRESULT endHr = interop->EndDraw();
    return ok && SUCCEEDED(endHr);
}

wuc::CompositionDrawingSurface CreateDrawingSurface(float widthPx,
                                                    float heightPx) {
    if (!g_gfx.compGraphics) return nullptr;
    float w = std::max(1.0f, std::ceil(widthPx));
    float h = std::max(1.0f, std::ceil(heightPx));
    return g_gfx.compGraphics.CreateDrawingSurface(
        wf::Size{w, h},
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::
            B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
}

// Apple continuous-curvature squircle path builder
constexpr float kAppleCornerReach = 1.52866f;
constexpr float kAppleCorner[16][2] = {
    {kAppleCornerReach, 0.0f},
    {1.08849f, 0.0f},  {0.86841f, 0.0f},  {0.66993f, 0.06550f},
    {0.66993f, 0.06550f}, {0.63149f, 0.07491f}, {0.63149f, 0.07491f},
    {0.37282f, 0.16906f}, {0.16906f, 0.37282f}, {0.07491f, 0.63149f},
    {0.07491f, 0.63149f}, {0.06550f, 0.66993f}, {0.06550f, 0.66993f},
    {0.0f, 0.86841f},  {0.0f, 1.08849f},  {0.0f, kAppleCornerReach},
};

winrt::com_ptr<ID2D1PathGeometry> BuildContinuousSquirclePath(float x,
                                                              float y,
                                                              float w,
                                                              float h,
                                                              float radius) {
    winrt::com_ptr<ID2D1PathGeometry> geom;
    if (!g_gfx.d2dFactory || w <= 0.0f || h <= 0.0f ||
        FAILED(g_gfx.d2dFactory->CreatePathGeometry(geom.put()))) {
        return nullptr;
    }
    winrt::com_ptr<ID2D1GeometrySink> sink;
    if (FAILED(geom->Open(sink.put()))) return nullptr;
    float r =
        std::clamp(radius, 0.0f, std::min(w, h) * 0.5f / kAppleCornerReach);
    struct CornerSpec {
        float cx, cy, inX, inY, outX, outY;
    };
    const CornerSpec corners[4] = {
        {x + w, y, -1, 0, 0, 1},
        {x + w, y + h, 0, -1, -1, 0},
        {x, y + h, 1, 0, 0, -1},
        {x, y, 0, 1, 1, 0},
    };
    std::vector<D2D1_POINT_2F> pts;
    pts.reserve(76);
    for (const auto& c : corners) {
        for (int i = 0; i < 16; ++i) {
            float in = kAppleCorner[i][0];
            float out = kAppleCorner[i][1];
            D2D1_POINT_2F pt =
                D2D1::Point2F(c.cx + (in * c.inX + out * c.outX) * r,
                              c.cy + (in * c.inY + out * c.outY) * r);
            if (i == 0 && !pts.empty()) {
                pts.push_back(pts.back());
                pts.push_back(pt);
            }
            pts.push_back(pt);
        }
    }
    sink->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_FILLED);
    for (size_t i = 1; i + 2 < pts.size(); i += 3) {
        sink->AddBezier(D2D1::BezierSegment(pts[i], pts[i + 1], pts[i + 2]));
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    return geom;
}

wuc::CompositionPath MakeCompositionSquirclePath(float x,
                                                 float y,
                                                 float w,
                                                 float h,
                                                 float radius) {
    auto d2dPath = BuildContinuousSquirclePath(x, y, w, h, radius);
    if (!d2dPath) return nullptr;
    return wuc::CompositionPath(winrt::make<GlassPath>(d2dPath.get()));
}

winrt::com_ptr<ID2D1PathGeometry> BuildSquircle(D2D1_RECT_F rect) {
    float w = rect.right - rect.left;
    float h = rect.bottom - rect.top;
    return BuildContinuousSquirclePath(rect.left, rect.top, w, h,
                                       std::min(w, h) * 0.225f);
}

// ============================================================================
// 3b. Optical Liquid Glass Capture & Refraction Plates
// ============================================================================

struct LiquidCaptureWorker {
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    winrt::com_ptr<IDXGIOutputDuplication> duplication;
    winrt::com_ptr<ID3D11Texture2D> sharedTex;
    winrt::com_ptr<IDXGIKeyedMutex> sharedLock;
    RECT screenRect{};
    std::atomic<bool> stop{false};
    std::atomic<uint64_t> copied{0};
    enum State : int { Running = 0, Lost = 1, Failed = 2 };
    std::atomic<int> state{Running};
    std::mutex copyRectMutex;
    RECT pendingCopyRect{};
    bool pendingCopyFull = true;
    bool pendingCopyValid = false;
};

struct LiquidCaptureState {
    HMONITOR monitor = nullptr;
    RECT screenRect{};
    std::shared_ptr<LiquidCaptureWorker> worker;
    HANDLE workerThread = nullptr;
    HANDLE watchEvent = nullptr;
    HANDLE stopEvent = nullptr;
    winrt::com_ptr<ID3D11Texture2D> openedSharedTex;
    winrt::com_ptr<IDXGIKeyedMutex> openedSharedLock;
    uint64_t takenCopies = 0;

    winrt::com_ptr<ID3D11Texture2D> desktopTex;
    uint64_t frameId = 0;

    bool excludedApplied = false;
    ULONGLONG excludedAtMs = 0;
    ULONGLONG retryAtMs = 0;
    bool unsupported = false;
    bool loggedFail = false;
    int consecutiveFailures = 0;

    std::atomic<bool> framePosted{false};
    std::atomic<bool> watching{false};
    std::atomic<bool> highRate{true};
    std::atomic<bool> forceFullCopy{true};

    std::mutex regionsMutex;
    std::vector<RECT> watchedRegions;
    std::vector<RECT> lastUiRegions;
    RECT lastCoveredRect{};
};

LiquidCaptureState g_liquidCapture;

void EnsureLiquidCaptureEvents() {
    if (!g_liquidCapture.watchEvent) {
        g_liquidCapture.watchEvent =
            CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }
    if (!g_liquidCapture.stopEvent) {
        g_liquidCapture.stopEvent =
            CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }
}

HWND GetSpotlightNotifyHwnd();
void ApplyCaptureExclusion(bool excluded);

void UpdateLiquidCaptureWatchedRegions(const std::vector<RECT>& regions) {
    auto& prev = g_liquidCapture.lastUiRegions;
    if (prev.size() == regions.size()) {
        bool same = true;
        for (size_t i = 0; i < regions.size(); ++i) {
            if (!EqualRect(&prev[i], &regions[i])) {
                same = false;
                break;
            }
        }
        if (same) return;
    }
    prev = regions;
    RECT bounds{};
    for (const auto& r : regions) {
        if (r.right > r.left && r.bottom > r.top) {
            if (IsRectEmpty(&bounds)) bounds = r;
            else UnionRect(&bounds, &bounds, &r);
        }
    }
    if (!IsRectEmpty(&bounds)) {
        RECT inter{};
        if (IsRectEmpty(&g_liquidCapture.lastCoveredRect) ||
            !IntersectRect(&inter, &bounds, &g_liquidCapture.lastCoveredRect) ||
            !EqualRect(&inter, &bounds)) {
            g_liquidCapture.forceFullCopy.store(true, std::memory_order_release);
        }
        g_liquidCapture.lastCoveredRect = bounds;
    }
    std::lock_guard<std::mutex> lock(g_liquidCapture.regionsMutex);
    g_liquidCapture.watchedRegions = regions;
}

bool DuplicationChangedNearWatched(IDXGIOutputDuplication* dup,
                                   const RECT& screenRect,
                                   const DXGI_OUTDUPL_FRAME_INFO& info,
                                   std::vector<BYTE>& metaBuf,
                                   std::vector<RECT>& regions) {
    if (!info.TotalMetadataBufferSize) return true;
    {
        std::lock_guard<std::mutex> lock(g_liquidCapture.regionsMutex);
        regions = g_liquidCapture.watchedRegions;
    }
    if (regions.empty()) return true;
    for (auto& r : regions) OffsetRect(&r, -screenRect.left, -screenRect.top);
    metaBuf.resize(info.TotalMetadataBufferSize);
    UINT sz = 0;
    RECT hit{};
    auto* moves = reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(metaBuf.data());
    if (FAILED(dup->GetFrameMoveRects(static_cast<UINT>(metaBuf.size()), moves,
                                      &sz))) {
        return true;
    }
    UINT moveCount = sz / sizeof(DXGI_OUTDUPL_MOVE_RECT);
    for (UINT i = 0; i < moveCount; ++i) {
        for (const auto& r : regions) {
            if (IntersectRect(&hit, &moves[i].DestinationRect, &r)) return true;
        }
    }
    auto* dirty = reinterpret_cast<RECT*>(metaBuf.data());
    if (FAILED(dup->GetFrameDirtyRects(static_cast<UINT>(metaBuf.size()), dirty,
                                       &sz))) {
        return true;
    }
    UINT dirtyCount = sz / sizeof(RECT);
    for (UINT i = 0; i < dirtyCount; ++i) {
        for (const auto& r : regions) {
            if (IntersectRect(&hit, &dirty[i], &r)) return true;
        }
    }
    return false;
}

DWORD WINAPI LiquidCaptureThreadProc(LPVOID param) {
    auto* holder = static_cast<std::shared_ptr<LiquidCaptureWorker>*>(param);
    auto w = *holder;
    delete holder;
    bool first = true;
    bool missed = false;
    std::vector<BYTE> metaBuf;
    std::vector<RECT> regionsBuf;
    try {
        while (!w->stop.load(std::memory_order_relaxed)) {
            if (!first && !g_liquidCapture.watching.load(std::memory_order_acquire)) {
                HANDLE waits[2] = {g_liquidCapture.stopEvent,
                                   g_liquidCapture.watchEvent};
                if (waits[0] && waits[1]) {
                    DWORD wr =
                        WaitForMultipleObjects(2, waits, FALSE, INFINITE);
                    if (wr == WAIT_OBJECT_0 || wr == WAIT_FAILED ||
                        w->stop.load(std::memory_order_acquire)) {
                        break;
                    }
                } else {
                    Sleep(60);
                }
                continue;
            }
            DXGI_OUTDUPL_FRAME_INFO info{};
            winrt::com_ptr<IDXGIResource> res;
            HRESULT hr = w->duplication->AcquireNextFrame(100, &info, res.put());
            if (hr == DXGI_ERROR_WAIT_TIMEOUT) continue;
            if (FAILED(hr)) {
                w->state = LiquidCaptureWorker::Lost;
                break;
            }
            bool doFull =
                std::exchange(first, false) ||
                g_liquidCapture.forceFullCopy.exchange(false,
                                                       std::memory_order_acq_rel);
            bool copiedFrame = false;
            bool lost = false;
            {
                struct FrameGuard {
                    IDXGIOutputDuplication* dup;
                    ~FrameGuard() { dup->ReleaseFrame(); }
                } guard{w->duplication.get()};

                do {
                    bool keep =
                        doFull || std::exchange(missed, false) ||
                        (info.LastPresentTime.QuadPart != 0 &&
                         DuplicationChangedNearWatched(w->duplication.get(),
                                                       w->screenRect, info, metaBuf,
                                                       regionsBuf));
                    if (!keep || w->stop.load(std::memory_order_relaxed)) break;
                    auto tex = res.try_as<ID3D11Texture2D>();
                    if (!tex) break;

                    D3D11_TEXTURE2D_DESC srcDesc{}, dstDesc{};
                    tex->GetDesc(&srcDesc);
                    w->sharedTex->GetDesc(&dstDesc);
                    if (srcDesc.Format != dstDesc.Format ||
                        srcDesc.Width != dstDesc.Width ||
                        srcDesc.Height != dstDesc.Height) {
                        w->state = LiquidCaptureWorker::Lost;
                        lost = true;
                        break;
                    }

                    RECT copyRc = w->screenRect;
                    bool copyFull = doFull;
                    if (!copyFull) {
                        {
                            std::lock_guard<std::mutex> lock(
                                g_liquidCapture.regionsMutex);
                            regionsBuf = g_liquidCapture.watchedRegions;
                        }
                        RECT bounds{};
                        for (const auto& r : regionsBuf) {
                            if (r.right > r.left && r.bottom > r.top) {
                                if (IsRectEmpty(&bounds)) bounds = r;
                                else UnionRect(&bounds, &bounds, &r);
                            }
                        }
                        if (!IsRectEmpty(&bounds)) {
                            InflateRect(&bounds, 64, 64);
                            RECT clamped{};
                            if (IntersectRect(&clamped, &bounds, &w->screenRect) &&
                                clamped.right > clamped.left &&
                                clamped.bottom > clamped.top) {
                                copyRc = clamped;
                            } else {
                                break;
                            }
                        } else {
                            copyFull = true;
                        }
                    }

                    HRESULT syncHr = w->sharedLock->AcquireSync(0, 50);
                    if (syncHr == static_cast<HRESULT>(WAIT_TIMEOUT)) {
                        missed = true;
                        if (doFull) {
                            g_liquidCapture.forceFullCopy.store(
                                true, std::memory_order_release);
                        }
                        break;
                    }
                    if (syncHr != S_OK) {
                        w->state = LiquidCaptureWorker::Lost;
                        lost = true;
                        break;
                    }
                    if (copyFull) {
                        w->context->CopyResource(w->sharedTex.get(), tex.get());
                    } else {
                        D3D11_BOX box{
                            static_cast<UINT>(
                                std::max<LONG>(0, copyRc.left - w->screenRect.left)),
                            static_cast<UINT>(
                                std::max<LONG>(0, copyRc.top - w->screenRect.top)),
                            0,
                            static_cast<UINT>(std::min<LONG>(
                                dstDesc.Width, copyRc.right - w->screenRect.left)),
                            static_cast<UINT>(std::min<LONG>(
                                dstDesc.Height, copyRc.bottom - w->screenRect.top)),
                            1};
                        if (box.right > box.left && box.bottom > box.top) {
                            w->context->CopySubresourceRegion(
                                w->sharedTex.get(), 0, box.left, box.top, 0,
                                tex.get(), 0, &box);
                        }
                    }
                    w->sharedLock->ReleaseSync(0);

                    {
                        std::lock_guard<std::mutex> lock(w->copyRectMutex);
                        if (copyFull) {
                            w->pendingCopyFull = true;
                            w->pendingCopyRect = w->screenRect;
                            w->pendingCopyValid = true;
                        } else if (!w->pendingCopyFull) {
                            if (!w->pendingCopyValid) {
                                w->pendingCopyRect = copyRc;
                                w->pendingCopyValid = true;
                            } else {
                                UnionRect(&w->pendingCopyRect, &w->pendingCopyRect,
                                          &copyRc);
                            }
                        }
                    }

                    w->copied.fetch_add(1, std::memory_order_release);
                    copiedFrame = true;
                    if (g_liquidCapture.watching.load(std::memory_order_relaxed)) {
                        HWND notify = GetSpotlightNotifyHwnd();
                        if (notify && !g_liquidCapture.framePosted.exchange(true)) {
                            if (!PostMessageW(notify, kWmAppGlassFrame, 0, 0)) {
                                g_liquidCapture.framePosted = false;
                            }
                        }
                    }
                } while (false);
            }
            if (lost) break;
            if (!w->stop.load(std::memory_order_relaxed)) {
                DWORD waitMs = copiedFrame ? 48u : 60u;
                if (g_liquidCapture.stopEvent) {
                    if (WaitForSingleObject(g_liquidCapture.stopEvent,
                                            waitMs) == WAIT_OBJECT_0) {
                        break;
                    }
                } else {
                    Sleep(waitMs);
                }
            }
        }
    } catch (...) {
        w->state = LiquidCaptureWorker::Failed;
    }
    w->sharedLock = nullptr;
    w->sharedTex = nullptr;
    w->duplication = nullptr;
    w->context = nullptr;
    w->device = nullptr;
    return 0;
}

void StopLiquidCaptureWorker() {
    auto& c = g_liquidCapture;
    if (c.worker) c.worker->stop.store(true, std::memory_order_release);
    if (c.stopEvent) SetEvent(c.stopEvent);
    if (c.watchEvent) SetEvent(c.watchEvent);
    if (c.workerThread) {
        WaitForSingleObject(c.workerThread, 1500);
        CloseHandle(c.workerThread);
        c.workerThread = nullptr;
    }
    if (c.stopEvent) ResetEvent(c.stopEvent);
    if (c.watchEvent && !c.watching.load(std::memory_order_relaxed)) {
        ResetEvent(c.watchEvent);
    }
    c.worker.reset();
    c.openedSharedLock = nullptr;
    c.openedSharedTex = nullptr;
    c.takenCopies = 0;
}

void ShutdownLiquidCapture() {
    g_liquidCapture.watching = false;
    StopLiquidCaptureWorker();
    if (g_liquidCapture.watchEvent) {
        CloseHandle(g_liquidCapture.watchEvent);
        g_liquidCapture.watchEvent = nullptr;
    }
    if (g_liquidCapture.stopEvent) {
        CloseHandle(g_liquidCapture.stopEvent);
        g_liquidCapture.stopEvent = nullptr;
    }
    g_liquidCapture.desktopTex = nullptr;
    g_liquidCapture.frameId = 0;
    g_liquidCapture.excludedApplied = false;
    g_liquidCapture.unsupported = false;
    g_liquidCapture.loggedFail = false;
    g_liquidCapture.consecutiveFailures = 0;
    g_liquidCapture.lastCoveredRect = {};
    g_liquidCapture.lastUiRegions.clear();
}

void SetLiquidCaptureWatching(bool watching, bool highRate) {
    EnsureLiquidCaptureEvents();
    bool prev =
        g_liquidCapture.watching.exchange(watching, std::memory_order_acq_rel);
    g_liquidCapture.highRate.store(highRate, std::memory_order_relaxed);
    if (watching) {
        if (!prev) {
            g_liquidCapture.forceFullCopy.store(true,
                                                std::memory_order_release);
        }
        if (g_liquidCapture.watchEvent) SetEvent(g_liquidCapture.watchEvent);
    } else {
        if (g_liquidCapture.watchEvent) ResetEvent(g_liquidCapture.watchEvent);
    }
}

bool WantOpticalLiquidGlass() {
    return g_settings.glassRefraction != GlassRefraction::Off &&
           !g_liquidCapture.unsupported &&
           g_liquidCapture.consecutiveFailures < 5 && !g_quitting;
}

void ApplyWindowCaptureAffinity(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    DWORD want = WantOpticalLiquidGlass() ? kWdaExcludeFromCapture : WDA_NONE;
    DWORD cur = WDA_NONE;
    if (!GetWindowDisplayAffinity(hwnd, &cur) || cur != want) {
        SetWindowDisplayAffinity(hwnd, want);
    }
}

bool StartLiquidCaptureForMonitor(HMONITOR targetMon) {
    auto& c = g_liquidCapture;
    if (!g_gfx.d3dDevice || !g_gfx.d3dContext) return false;
    HMONITOR mon = targetMon ? targetMon : GetPrimaryMonitor();
    if (!mon) return false;
    c.monitor = mon;

    winrt::com_ptr<IDXGIFactory1> factory;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1),
                                  factory.put_void())) ||
        !factory) {
        return false;
    }
    winrt::com_ptr<IDXGIAdapter1> adapter;
    for (UINT ai = 0; SUCCEEDED(factory->EnumAdapters1(ai, adapter.put()));
         ++ai, adapter = nullptr) {
        winrt::com_ptr<IDXGIOutput> output;
        for (UINT oi = 0; SUCCEEDED(adapter->EnumOutputs(oi, output.put()));
             ++oi, output = nullptr) {
            DXGI_OUTPUT_DESC desc{};
            if (FAILED(output->GetDesc(&desc)) || desc.Monitor != mon) continue;
            auto output1 = output.try_as<IDXGIOutput1>();
            if (!output1) {
                c.unsupported = true;
                return false;
            }
            auto w = std::make_shared<LiquidCaptureWorker>();
            D3D_FEATURE_LEVEL levels[] = {
                D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
            D3D_FEATURE_LEVEL obtained{};
            if (FAILED(D3D11CreateDevice(
                    adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, levels,
                    ARRAYSIZE(levels), D3D11_SDK_VERSION, w->device.put(),
                    &obtained, w->context.put()))) {
                return false;
            }
            HRESULT hr =
                output1->DuplicateOutput(w->device.get(), w->duplication.put());
            if (FAILED(hr)) {
                if (!std::exchange(c.loggedFail, true)) {
                    Wh_Log(L"Spotlight DXGI DuplicateOutput failed (0x%08X)",
                           static_cast<unsigned>(hr));
                }
                return false;
            }
            DXGI_OUTDUPL_DESC dupDesc{};
            w->duplication->GetDesc(&dupDesc);
            if (dupDesc.Rotation != DXGI_MODE_ROTATION_IDENTITY &&
                dupDesc.Rotation != DXGI_MODE_ROTATION_UNSPECIFIED) {
                c.unsupported = true;
                return false;
            }
            c.screenRect = w->screenRect = desc.DesktopCoordinates;
            LONG monW = c.screenRect.right - c.screenRect.left;
            LONG monH = c.screenRect.bottom - c.screenRect.top;
            if (monW <= 0 || monH <= 0) return false;

            D3D11_TEXTURE2D_DESC td{};
            td.Width = static_cast<UINT>(monW);
            td.Height = static_cast<UINT>(monH);
            td.MipLevels = 1;
            td.ArraySize = 1;
            td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            td.SampleDesc = {1, 0};
            td.Usage = D3D11_USAGE_DEFAULT;
            td.BindFlags =
                D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            td.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
            if (FAILED(w->device->CreateTexture2D(&td, nullptr,
                                                  w->sharedTex.put()))) {
                return false;
            }
            w->sharedLock = w->sharedTex.try_as<IDXGIKeyedMutex>();
            auto sharedRes = w->sharedTex.try_as<IDXGIResource>();
            HANDLE sharedHandle = nullptr;
            if (!w->sharedLock || !sharedRes ||
                FAILED(sharedRes->GetSharedHandle(&sharedHandle)) ||
                !sharedHandle) {
                return false;
            }
            winrt::com_ptr<ID3D11Texture2D> opened;
            if (FAILED(g_gfx.d3dDevice->OpenSharedResource(
                    sharedHandle, __uuidof(ID3D11Texture2D),
                    opened.put_void())) ||
                !opened) {
                return false;
            }
            auto openedLock = opened.try_as<IDXGIKeyedMutex>();
            if (!openedLock) return false;

            StopLiquidCaptureWorker();
            EnsureLiquidCaptureEvents();
            if (c.stopEvent) ResetEvent(c.stopEvent);
            auto* arg = new std::shared_ptr<LiquidCaptureWorker>(w);
            HANDLE th = CreateThread(nullptr, 0, LiquidCaptureThreadProc, arg,
                                     0, nullptr);
            if (!th) {
                delete arg;
                return false;
            }
            c.worker = w;
            c.workerThread = th;
            c.openedSharedTex = opened;
            c.openedSharedLock = openedLock;
            c.takenCopies = 0;
            c.loggedFail = false;
            return true;
        }
    }
    return false;
}

bool EnsureLiquidCaptureReady(HMONITOR targetMon) {
    auto& c = g_liquidCapture;
    if (!WantOpticalLiquidGlass()) {
        if (c.worker) StopLiquidCaptureWorker();
        if (c.excludedApplied) {
            ApplyCaptureExclusion(false);
            c.excludedApplied = false;
        }
        return false;
    }
    if (c.worker && targetMon && c.monitor != targetMon) {
        StopLiquidCaptureWorker();
        c.desktopTex = nullptr;
        c.retryAtMs = 0;
    }
    ULONGLONG now = GetTickCount64();
    if (!c.excludedApplied) {
        ApplyCaptureExclusion(true);
        c.excludedApplied = true;
        c.excludedAtMs = now;
        c.retryAtMs = now + 45;
        return c.desktopTex != nullptr;
    }
    if (c.worker) {
        int st = c.worker->state.load(std::memory_order_acquire);
        if (st == LiquidCaptureWorker::Lost) {
            StopLiquidCaptureWorker();
            c.retryAtMs = now + 400;
            return c.desktopTex != nullptr;
        }
        if (st == LiquidCaptureWorker::Failed) {
            StopLiquidCaptureWorker();
            c.consecutiveFailures++;
            c.retryAtMs = now + 1500;
            return false;
        }
        uint64_t copied = c.worker->copied.load(std::memory_order_acquire);
        if (copied != c.takenCopies && c.openedSharedTex &&
            c.openedSharedLock && g_gfx.d3dDevice && g_gfx.d3dContext) {
            D3D11_TEXTURE2D_DESC desc{}, have{};
            c.openedSharedTex->GetDesc(&desc);
            if (c.desktopTex) c.desktopTex->GetDesc(&have);
            winrt::com_ptr<ID3D11Texture2D> target = c.desktopTex;
            bool newTarget =
                (!target || have.Width != desc.Width ||
                 have.Height != desc.Height || have.Format != desc.Format);
            if (newTarget) {
                desc.BindFlags = 0;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;
                target = nullptr;
                if (FAILED(g_gfx.d3dDevice->CreateTexture2D(&desc, nullptr,
                                                            target.put()))) {
                    return false;
                }
                g_liquidCapture.forceFullCopy.store(true,
                                                    std::memory_order_release);
            }
            {
                D2DMultithreadLock lock(g_gfx.d2dMultithread.get());
                if (c.openedSharedLock->AcquireSync(0, 0) == S_OK) {
                    RECT copyRc{};
                    bool copyFull = true;
                    bool copyValid = false;
                    {
                        std::lock_guard<std::mutex> rlock(
                            c.worker->copyRectMutex);
                        copyFull = c.worker->pendingCopyFull || newTarget;
                        copyRc = c.worker->pendingCopyRect;
                        copyValid = c.worker->pendingCopyValid;
                        c.worker->pendingCopyFull = false;
                        c.worker->pendingCopyValid = false;
                    }
                    if (copyFull || !copyValid || newTarget) {
                        g_gfx.d3dContext->CopyResource(
                            target.get(), c.openedSharedTex.get());
                    } else {
                        D3D11_BOX box{
                            static_cast<UINT>(
                                std::max<LONG>(0, copyRc.left - c.screenRect.left)),
                            static_cast<UINT>(
                                std::max<LONG>(0, copyRc.top - c.screenRect.top)),
                            0,
                            static_cast<UINT>(std::min<LONG>(
                                desc.Width, copyRc.right - c.screenRect.left)),
                            static_cast<UINT>(std::min<LONG>(
                                desc.Height, copyRc.bottom - c.screenRect.top)),
                            1};
                        if (box.right > box.left && box.bottom > box.top) {
                            g_gfx.d3dContext->CopySubresourceRegion(
                                target.get(), 0, box.left, box.top, 0,
                                c.openedSharedTex.get(), 0, &box);
                        }
                    }
                    c.openedSharedLock->ReleaseSync(0);
                    c.desktopTex = target;
                    c.frameId++;
                    c.takenCopies = copied;
                }
            }
        }
        return c.desktopTex != nullptr;
    }

    if (now >= c.retryAtMs && now >= c.excludedAtMs + 40) {
        if (!StartLiquidCaptureForMonitor(targetMon)) {
            c.retryAtMs = now + 1500;
        }
    }
    return c.desktopTex != nullptr;
}

template <typename T>
void SetD2DEffectVal(ID2D1Effect* fx, UINT32 prop, const T& val) {
    winrt::check_hresult(fx->SetValue(
        prop, D2D1_PROPERTY_TYPE_UNKNOWN,
        reinterpret_cast<const BYTE*>(&val), sizeof(val)));
}

winrt::com_ptr<ID2D1Image> GetD2DEffectOut(ID2D1Effect* fx) {
    winrt::com_ptr<ID2D1Image> img;
    fx->GetOutput(img.put());
    return img;
}

struct LiquidGlassStyleParams {
    float screenX = 0.0f;
    float screenY = 0.0f;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
    float cornerRadiusPx = 0.0f;
    float scale = 1.0f;
    bool darkMode = true;
    bool useSquircle = true;
    float tintBoost = 0.0f;
    float blurMul = 1.0f;
    float rimDepthMul = 1.0f;
    float strengthMul = 1.0f;
};

struct LiquidGlassPlate {
    wuc::CompositionDrawingSurface surface{nullptr};
    wuc::CompositionSurfaceBrush brush{nullptr};
    wuc::SpriteVisual visual{nullptr};
    SIZE surfaceSize{};

    winrt::com_ptr<ID3D11Texture2D> behindTex;
    winrt::com_ptr<ID2D1Bitmap1> behindBmp;
    winrt::com_ptr<ID2D1Bitmap1> mapBmp;
    SIZE mapCap{};
    std::vector<float> mapPixels;
    float mapScale = 1.0f;
    std::array<float, 11> mapKey{};

    ID2D1DeviceContext* fxDc = nullptr;
    winrt::com_ptr<ID2D1Effect> fxCrop;
    winrt::com_ptr<ID2D1Effect> fxBorder;
    winrt::com_ptr<ID2D1Effect> fxMapScale;
    winrt::com_ptr<ID2D1Effect> fxDisp;
    winrt::com_ptr<ID2D1Effect> fxBlur;
    winrt::com_ptr<ID2D1Effect> fxSat;
    winrt::com_ptr<ID2D1Effect> fxCeil1;
    winrt::com_ptr<ID2D1Effect> fxCeil2;

    std::array<float, 5> lastRect{};
    uint64_t lastFrameId = 0;
    uint32_t lastStyleSig = 0;
    bool dirty = true;
    bool active = false;

    winrt::com_ptr<ID2D1PathGeometry> cachedClipGeom;
    winrt::com_ptr<ID2D1PathGeometry> cachedOuterGeom;
    winrt::com_ptr<ID2D1PathGeometry> cachedRimGeom;
    std::array<float, 5> geomKey{};

    void ClearEffects() {
        fxDc = nullptr;
        fxCrop = nullptr;
        fxBorder = nullptr;
        fxMapScale = nullptr;
        fxDisp = nullptr;
        fxBlur = nullptr;
        fxSat = nullptr;
        fxCeil1 = nullptr;
        fxCeil2 = nullptr;
    }
    void ResetGpuResources() {
        ClearEffects();
        cachedClipGeom = nullptr;
        cachedOuterGeom = nullptr;
        cachedRimGeom = nullptr;
        geomKey = {};
        behindBmp = nullptr;
        behindTex = nullptr;
        mapBmp = nullptr;
        mapCap = {};
        mapKey = {};
        surface = nullptr;
        surfaceSize = {};
        if (brush) brush.Surface(nullptr);
        dirty = true;
        active = false;
    }
    void Shutdown() {
        ResetGpuResources();
        visual = nullptr;
        brush = nullptr;
    }
};

inline LONG RoundUpAlign(LONG val, LONG step) {
    return ((val + step - 1) / step) * step;
}

float ComputeLiquidBlurSigmaPx(float scale, float blurMul = 1.0f) {
    float v = static_cast<float>(std::clamp(g_settings.glassBlur, 0, 100)) /
              100.0f;
    if (v <= 0.001f) return 0.0f;
    return std::pow(v, 1.20f) * 15.0f * scale * blurMul;
}

float ComputeRimShiftCurve(float u) {
    switch (g_settings.glassRefraction) {
        case GlassRefraction::Reflection: {
            constexpr float kTurn = 0.25f;
            return u < kTurn ? (u * u / kTurn) : (2.0f * u - kTurn);
        }
        case GlassRefraction::Stretch:
            return 0.5f * u * u;
        case GlassRefraction::Lens:
        default:
            return 2.0f * u * u;
    }
}

float BuildLiquidGlassMap(std::vector<float>& pixels,
                          int w,
                          int h,
                          float sx,
                          float sy,
                          float sw,
                          float sh,
                          float radius,
                          float scale,
                          float rimDepthMul,
                          float strengthMul) {
    float depthPx =
        static_cast<float>(g_settings.glassEdgeDepth) * scale * rimDepthMul;
    float strength =
        (static_cast<float>(g_settings.glassEdgeStrength) / 100.0f) *
        strengthMul;
    float dispScale = std::max(4.0f * depthPx * std::max(strength, 0.2f), 1.0f);
    pixels.assign(static_cast<size_t>(w) * static_cast<size_t>(h) * 4, 0.5f);
    for (size_t i = 3; i < pixels.size(); i += 4) pixels[i] = 1.0f;

    float halfW = sw * 0.5f;
    float halfH = sh * 0.5f;
    float cx = sx + halfW;
    float cy = sy + halfH;
    radius = std::clamp(radius, 0.0f, std::min(halfW, halfH));
    float rim = std::min(depthPx, std::min(halfW, halfH) * 0.48f);
    if (rim <= 0.5f || strength <= 0.001f) return dispScale;

    float roundCorner = std::max(radius, rim);
    auto bendPixel = [&](int x, int y) {
        float px = (static_cast<float>(x) + 0.5f) - cx;
        float py = (static_cast<float>(y) + 0.5f) - cy;
        float ax = std::fabs(px);
        float ay = std::fabs(py);
        float qx = ax - (halfW - radius);
        float qy = ay - (halfH - radius);
        float ox = std::max(qx, 0.0f);
        float oy = std::max(qy, 0.0f);
        float depth = radius - std::min(std::max(qx, qy), 0.0f) -
                      std::sqrt(ox * ox + oy * oy);
        if (depth < 0.0f || depth >= rim) return;
        float nx = std::max(ax - (halfW - roundCorner), 0.0f);
        float ny = std::max(ay - (halfH - roundCorner), 0.0f);
        float len = std::sqrt(nx * nx + ny * ny);
        if (len <= 1e-5f) return;
        float u = 1.0f - depth / rim;
        float off = rim * ComputeRimShiftCurve(u) * strength / dispScale / len;
        float* p =
            &pixels[(static_cast<size_t>(y) * static_cast<size_t>(w) +
                     static_cast<size_t>(x)) *
                    4];
        p[0] = std::clamp(0.5f - std::copysign(nx, px) * off, 0.0f, 1.0f);
        p[1] = std::clamp(0.5f - std::copysign(ny, py) * off, 0.0f, 1.0f);
    };

    int x0 = std::max(0, static_cast<int>(std::floor(sx)));
    int x1 = std::min(w, static_cast<int>(std::ceil(sx + sw)));
    int y0 = std::max(0, static_cast<int>(std::floor(sy)));
    int y1 = std::min(h, static_cast<int>(std::ceil(sy + sh)));
    float ends = halfW - radius - rim;
    int innerLeft =
        std::clamp(static_cast<int>(std::floor(cx - ends - 0.5f)) + 1, x0, x1);
    int innerRight =
        std::clamp(static_cast<int>(std::ceil(cx + ends - 0.5f)), innerLeft, x1);
    for (int y = y0; y < y1; ++y) {
        bool inBandY =
            std::fabs((static_cast<float>(y) + 0.5f) - cy) >= (halfH - rim);
        for (int x = x0; x < x1; ++x) {
            if (!inBandY && x == innerLeft) x = innerRight;
            if (x >= x1) break;
            bendPixel(x, y);
        }
    }
    return dispScale;
}

winrt::com_ptr<ID2D1PathGeometry> BuildPlateShapeGeometry(float x,
                                                            float y,
                                                            float w,
                                                            float h,
                                                            float radius,
                                                            bool useSquircle) {
    w = std::max(1.0f, w);
    h = std::max(1.0f, h);
    radius = std::max(0.5f, radius);
    if (!useSquircle && g_gfx.d2dFactory) {
        winrt::com_ptr<ID2D1RoundedRectangleGeometry> rr;
        if (SUCCEEDED(g_gfx.d2dFactory->CreateRoundedRectangleGeometry(
                D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), radius,
                                  radius),
                rr.put()))) {
            winrt::com_ptr<ID2D1PathGeometry> pg;
            if (SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(pg.put()))) {
                winrt::com_ptr<ID2D1GeometrySink> sk;
                if (SUCCEEDED(pg->Open(sk.put()))) {
                    rr->Outline(nullptr, sk.get());
                    sk->Close();
                    return pg;
                }
            }
        }
    }
    return BuildContinuousSquirclePath(x, y, w, h, radius);
}

void EnsurePlateGeometries(LiquidGlassPlate& plate,
                           const LiquidGlassStyleParams& p) {
    std::array<float, 5> gKey{p.widthPx, p.heightPx, p.cornerRadiusPx,
                              p.useSquircle ? 1.0f : 0.0f, p.scale};
    if (plate.geomKey == gKey && plate.cachedOuterGeom && plate.cachedRimGeom &&
        (p.cornerRadiusPx <= 0.5f || plate.cachedClipGeom)) {
        return;
    }
    plate.cachedClipGeom =
        (p.cornerRadiusPx > 0.5f)
            ? BuildPlateShapeGeometry(0.0f, 0.0f, p.widthPx, p.heightPx,
                                      p.cornerRadiusPx, p.useSquircle)
            : nullptr;
    plate.cachedOuterGeom = BuildPlateShapeGeometry(
        0.5f, 0.5f, std::max(1.0f, p.widthPx - 1.0f),
        std::max(1.0f, p.heightPx - 1.0f),
        std::max(0.5f, p.cornerRadiusPx - 0.5f * 0.65f), p.useSquircle);
    float rimW = std::max(1.0f, 1.15f * p.scale);
    float rimInset = 0.9f + rimW * 0.5f;
    plate.cachedRimGeom = BuildPlateShapeGeometry(
        rimInset, rimInset, std::max(1.0f, p.widthPx - 2.0f * rimInset),
        std::max(1.0f, p.heightPx - 2.0f * rimInset),
        std::max(0.5f, p.cornerRadiusPx - rimInset * 0.65f), p.useSquircle);
    plate.geomKey = gKey;
}

void DrawMacOS27GlassLighting(ID2D1DeviceContext* dc,
                              const LiquidGlassStyleParams& p,
                              ID2D1Geometry* clipGeom,
                              ID2D1Geometry* outerGeom = nullptr,
                              ID2D1Geometry* rimGeom = nullptr) {
    const float w = p.widthPx;
    const float h = p.heightPx;
    const float s = p.scale;
    const bool dark = p.darkMode;
    const float tintFrac =
        static_cast<float>(std::clamp(g_settings.glassTint, 0, 100)) / 100.0f;

    auto makeGrad = [&](D2D1_POINT_2F a, D2D1_POINT_2F b,
                        const D2D1_GRADIENT_STOP* stops, UINT32 count) {
        winrt::com_ptr<ID2D1GradientStopCollection> coll;
        winrt::com_ptr<ID2D1LinearGradientBrush> br;
        if (SUCCEEDED(
                dc->CreateGradientStopCollection(stops, count, coll.put()))) {
            dc->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(a, b), coll.get(),
                br.put());
        }
        return br;
    };
    auto fillShape = [&](ID2D1Brush* br) {
        if (!br) return;
        if (clipGeom) dc->FillGeometry(clipGeom, br);
        else dc->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), br);
    };

    float baseR = dark ? 0.045f : 0.985f;
    float baseG = dark ? 0.058f : 0.988f;
    float baseB = dark ? 0.088f : 1.000f;
    const bool tintedWash = (g_settings.glassStyle == GlassStyle::Tinted);
    if (tintedWash) {
        auto acc = GetAccentColor();
        baseR = Lerp(baseR, acc.R / 255.0f, dark ? 0.28f : 0.18f);
        baseG = Lerp(baseG, acc.G / 255.0f, dark ? 0.28f : 0.18f);
        baseB = Lerp(baseB, acc.B / 255.0f, dark ? 0.28f : 0.18f);
    }

    float baseAlpha = dark ? (0.14f + 0.44f * tintFrac + p.tintBoost)
                           : (0.24f + 0.44f * tintFrac + p.tintBoost);
    baseAlpha = std::clamp(baseAlpha, 0.08f, 0.88f);

    if (dark) {
        D2D1_GRADIENT_STOP stops[3] = {
            {0.0f, D2D1::ColorF(baseR * 1.15f, baseG * 1.20f, baseB * 1.30f,
                                baseAlpha * 0.84f)},
            {0.55f, D2D1::ColorF(baseR * 0.78f, baseG * 0.84f, baseB * 0.96f,
                                 baseAlpha)},
            {1.0f, D2D1::ColorF(baseR * 0.95f, baseG * 1.05f, baseB * 1.18f,
                                baseAlpha * 0.95f)},
        };
        if (auto br = makeGrad({0, 0}, {0, h}, stops, 3)) fillShape(br.get());
    } else {
        D2D1_GRADIENT_STOP stops[3] = {
            {0.0f, D2D1::ColorF(baseR, baseG, baseB,
                                std::min(0.92f, baseAlpha * 1.14f))},
            {0.55f, D2D1::ColorF(baseR * 0.98f, baseG * 0.98f, baseB,
                                 baseAlpha * 0.92f)},
            {1.0f, D2D1::ColorF(baseR * 0.95f, baseG * 0.97f, baseB,
                                baseAlpha * 0.86f)},
        };
        if (auto br = makeGrad({0, 0}, {0, h}, stops, 3)) fillShape(br.get());
    }

    {
        float sheenH = std::max(2.0f, std::min(h * 0.22f, 16.0f * s));
        D2D1_GRADIENT_STOP sheen[2] = {
            {0.0f, D2D1::ColorF(0.94f, 0.97f, 1.0f, dark ? 0.07f : 0.28f)},
            {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
        };
        if (auto br = makeGrad({0, 0}, {0, sheenH}, sheen, 2)) {
            fillShape(br.get());
        }
    }
    {
        float bounceH = std::max(2.0f, std::min(h * 0.16f, 12.0f * s));
        D2D1_GRADIENT_STOP bounce[2] = {
            {0.0f, D2D1::ColorF(0.76f, 0.88f, 1.0f, 0.0f)},
            {1.0f, D2D1::ColorF(0.76f, 0.88f, 1.0f, dark ? 0.06f : 0.14f)},
        };
        if (auto br = makeGrad({0, h - bounceH}, {0, h}, bounce, 2)) {
            fillShape(br.get());
        }
    }

    winrt::com_ptr<ID2D1PathGeometry> localOuter;
    ID2D1Geometry* useOuter = outerGeom;
    if (!useOuter) {
        localOuter = BuildPlateShapeGeometry(
            0.5f, 0.5f, w - 1.0f, h - 1.0f,
            std::max(0.5f, p.cornerRadiusPx - 0.5f * 0.65f), p.useSquircle);
        useOuter = localOuter.get();
    }
    if (useOuter) {
        winrt::com_ptr<ID2D1SolidColorBrush> darkEdge;
        if (SUCCEEDED(dc->CreateSolidColorBrush(
                D2D1::ColorF(0.0f, 0.0f, 0.0f, dark ? 0.42f : 0.14f),
                darkEdge.put()))) {
            dc->DrawGeometry(useOuter, darkEdge.get(), 1.0f);
        }
    }
    float rimW = std::max(1.0f, 1.15f * s);
    winrt::com_ptr<ID2D1PathGeometry> localRim;
    ID2D1Geometry* useRim = rimGeom;
    if (!useRim) {
        float inset = 0.9f + rimW * 0.5f;
        localRim = BuildPlateShapeGeometry(
            inset, inset, w - 2.0f * inset, h - 2.0f * inset,
            std::max(0.5f, p.cornerRadiusPx - inset * 0.65f), p.useSquircle);
        useRim = localRim.get();
    }
    if (useRim) {
        float rimR = 0.88f, rimG = 0.94f, rimB = 1.0f;
        if (tintedWash) {
            auto acc = GetAccentColor();
            rimR = Lerp(rimR, acc.R / 255.0f, 0.40f);
            rimG = Lerp(rimG, acc.G / 255.0f, 0.40f);
            rimB = Lerp(rimB, acc.B / 255.0f, 0.40f);
        }
        winrt::com_ptr<ID2D1SolidColorBrush> baseRim;
        if (SUCCEEDED(dc->CreateSolidColorBrush(
                D2D1::ColorF(rimR, rimG, rimB, dark ? 0.30f : 0.56f),
                baseRim.put()))) {
            dc->DrawGeometry(useRim, baseRim.get(), rimW);
        }
        D2D1_GRADIENT_STOP rStops[5] = {
            {0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, dark ? 0.58f : 0.86f)},
            {0.18f, D2D1::ColorF(0.92f, 0.96f, 1.0f, dark ? 0.18f : 0.30f)},
            {0.50f, D2D1::ColorF(0.86f, 0.92f, 1.0f, 0.02f)},
            {0.82f, D2D1::ColorF(0.86f, 0.94f, 1.0f, dark ? 0.18f : 0.28f)},
            {1.00f, D2D1::ColorF(0.92f, 0.96f, 1.0f, dark ? 0.46f : 0.72f)},
        };
        if (auto rbr = makeGrad({0.0f, 0.0f}, {w, h}, rStops, 5)) {
            dc->DrawGeometry(useRim, rbr.get(), rimW);
        }
    }
}

uint32_t ComputeGlassStyleSignature(const LiquidGlassStyleParams& p) {
    uint32_t h = 2166136261u;
    auto mix = [&](uint32_t v) {
        h ^= v;
        h *= 16777619u;
    };
    mix(static_cast<uint32_t>(g_settings.glassStyle));
    mix(static_cast<uint32_t>(g_settings.glassRefraction));
    mix(static_cast<uint32_t>(g_settings.glassEdgeDepth));
    mix(static_cast<uint32_t>(g_settings.glassEdgeStrength));
    mix(static_cast<uint32_t>(g_settings.glassTint));
    mix(static_cast<uint32_t>(g_settings.glassBlur));
    mix(static_cast<uint32_t>(g_settings.glassBrightnessLimit));
    mix(p.darkMode ? 1u : 0u);
    mix(p.useSquircle ? 1u : 0u);
    mix(static_cast<uint32_t>(std::lround(p.tintBoost * 100.0f)));
    return h;
}

bool RenderLiquidGlassPlate(LiquidGlassPlate& plate,
                            const LiquidGlassStyleParams& p) {
    if (!g_gfx.compositor || !g_gfx.compGraphics || !g_gfx.d3dDevice ||
        !g_gfx.d3dContext || !g_liquidCapture.desktopTex ||
        p.widthPx < 2.0f || p.heightPx < 2.0f) {
        return false;
    }
    uint32_t styleSig = ComputeGlassStyleSignature(p);
    std::array<float, 5> curRect{p.screenX, p.screenY, p.widthPx, p.heightPx,
                                 p.cornerRadiusPx};
    if (!plate.dirty && plate.active && curRect == plate.lastRect &&
        plate.lastFrameId == g_liquidCapture.frameId &&
        plate.lastStyleSig == styleSig) {
        return true;
    }

    float blurSigma = ComputeLiquidBlurSigmaPx(p.scale, p.blurMul);
    int pad = static_cast<int>(std::ceil(3.0f * blurSigma)) + 8;
    LONG left = static_cast<LONG>(std::floor(p.screenX));
    LONG top = static_cast<LONG>(std::floor(p.screenY));
    float fx = p.screenX - static_cast<float>(left);
    float fy = p.screenY - static_cast<float>(top);
    SIZE surfSz{std::max(1L, static_cast<LONG>(std::ceil(p.widthPx))),
                std::max(1L, static_cast<LONG>(std::ceil(p.heightPx)))};
    SIZE areaSz{surfSz.cx + 1 + 2 * pad, surfSz.cy + 1 + 2 * pad};
    RECT around{left - pad, top - pad, left - pad + areaSz.cx,
                top - pad + areaSz.cy};
    RECT valid{};
    if (!IntersectRect(&valid, &around, &g_liquidCapture.screenRect)) {
        return false;
    }

    D3D11_TEXTURE2D_DESC deskDesc{}, haveDesc{};
    g_liquidCapture.desktopTex->GetDesc(&deskDesc);
    if (plate.behindTex) plate.behindTex->GetDesc(&haveDesc);
    if (!plate.behindTex || haveDesc.Format != deskDesc.Format ||
        static_cast<LONG>(haveDesc.Width) < areaSz.cx ||
        static_cast<LONG>(haveDesc.Height) < areaSz.cy) {
        D3D11_TEXTURE2D_DESC td{};
        td.Width = static_cast<UINT>(RoundUpAlign(areaSz.cx, 256));
        td.Height = static_cast<UINT>(RoundUpAlign(areaSz.cy, 64));
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = deskDesc.Format;
        td.SampleDesc.Count = 1;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        plate.behindTex = nullptr;
        plate.behindBmp = nullptr;
        if (FAILED(g_gfx.d3dDevice->CreateTexture2D(&td, nullptr,
                                                    plate.behindTex.put()))) {
            return false;
        }
    }

    {
        D2DMultithreadLock lock(g_gfx.d2dMultithread.get());
        D3D11_BOX box{
            static_cast<UINT>(valid.left - g_liquidCapture.screenRect.left),
            static_cast<UINT>(valid.top - g_liquidCapture.screenRect.top),
            0,
            static_cast<UINT>(valid.right - g_liquidCapture.screenRect.left),
            static_cast<UINT>(valid.bottom - g_liquidCapture.screenRect.top),
            1};
        g_gfx.d3dContext->CopySubresourceRegion(
            plate.behindTex.get(), 0,
            static_cast<UINT>(valid.left - around.left),
            static_cast<UINT>(valid.top - around.top), 0,
            g_liquidCapture.desktopTex.get(), 0, &box);
    }
    OffsetRect(&valid, -around.left, -around.top);

    if (!plate.surface) {
        plate.ClearEffects();
        plate.surface = CreateDrawingSurface(static_cast<float>(surfSz.cx),
                                             static_cast<float>(surfSz.cy));
        if (!plate.surface) return false;
        plate.surfaceSize = surfSz;
    } else if (plate.surfaceSize.cx != surfSz.cx ||
               plate.surfaceSize.cy != surfSz.cy) {
        plate.ClearEffects();
        auto interop = plate.surface.as<ICompositionDrawingSurfaceInteropLocal>();
        if (FAILED(interop->Resize(surfSz))) {
            plate.surface =
                CreateDrawingSurface(static_cast<float>(surfSz.cx),
                                     static_cast<float>(surfSz.cy));
            if (!plate.surface) return false;
        }
        plate.surfaceSize = surfSz;
    }
    if (!plate.brush) {
        plate.brush = g_gfx.compositor.CreateSurfaceBrush(plate.surface);
        plate.brush.Stretch(wuc::CompositionStretch::Fill);
    } else {
        plate.brush.Surface(plate.surface);
    }
    if (!plate.visual) {
        plate.visual = g_gfx.compositor.CreateSpriteVisual();
        plate.visual.Brush(plate.brush);
    }
    plate.visual.Size({p.widthPx, p.heightPx});

    D2DMultithreadLock mtLock(g_gfx.d2dMultithread.get());
    auto interop = plate.surface.as<ICompositionDrawingSurfaceInteropLocal>();
    winrt::com_ptr<ID2D1DeviceContext> dc;
    POINT offset{};
    if (FAILED(interop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext),
                                  dc.put_void(), &offset)) ||
        !dc) {
        return false;
    }

    bool ok = false;
    try {
        dc->SetDpi(96.0f, 96.0f);
        if (!plate.behindBmp) {
            D2D1_BITMAP_PROPERTIES1 bp{
                {deskDesc.Format, D2D1_ALPHA_MODE_IGNORE},
                96.0f,
                96.0f,
                D2D1_BITMAP_OPTIONS_NONE,
                nullptr};
            winrt::check_hresult(dc->CreateBitmapFromDxgiSurface(
                plate.behindTex.as<IDXGISurface>().get(), &bp,
                plate.behindBmp.put()));
        }

        int mapW = std::max(2, static_cast<int>((areaSz.cx + 1) / 2));
        int mapH = std::max(2, static_cast<int>((areaSz.cy + 1) / 2));
        float invScaleX =
            static_cast<float>(mapW) / static_cast<float>(areaSz.cx);
        float invScaleY =
            static_cast<float>(mapH) / static_cast<float>(areaSz.cy);

        std::array<float, 11> mKey{
            static_cast<float>(mapW),
            static_cast<float>(mapH),
            (static_cast<float>(pad) + fx) * invScaleX,
            (static_cast<float>(pad) + fy) * invScaleY,
            std::round(p.widthPx * invScaleX),
            std::round(p.heightPx * invScaleY),
            std::round(p.cornerRadiusPx * invScaleX),
            p.scale * invScaleX,
            static_cast<float>(g_settings.glassRefraction),
            static_cast<float>(g_settings.glassEdgeDepth) * p.rimDepthMul,
            static_cast<float>(g_settings.glassEdgeStrength) * p.strengthMul};

        if (!plate.mapBmp || mKey != plate.mapKey) {
            plate.mapScale = BuildLiquidGlassMap(
                                 plate.mapPixels, mapW, mapH, mKey[2], mKey[3],
                                 p.widthPx * invScaleX, p.heightPx * invScaleY,
                                 p.cornerRadiusPx * invScaleX, mKey[7],
                                 p.rimDepthMul, p.strengthMul) /
                             std::max(invScaleX, 0.25f);
            if (!plate.mapBmp || plate.mapCap.cx < mapW ||
                plate.mapCap.cy < mapH) {
                plate.mapBmp = nullptr;
                plate.mapCap = {RoundUpAlign(mapW, 128),
                                RoundUpAlign(mapH, 64)};
                D2D1_BITMAP_PROPERTIES1 mp{
                    {DXGI_FORMAT_R32G32B32A32_FLOAT,
                     D2D1_ALPHA_MODE_PREMULTIPLIED},
                    96.0f,
                    96.0f,
                    D2D1_BITMAP_OPTIONS_NONE,
                    nullptr};
                winrt::check_hresult(dc->CreateBitmap(
                    D2D1::SizeU(static_cast<UINT32>(plate.mapCap.cx),
                                static_cast<UINT32>(plate.mapCap.cy)),
                    nullptr, 0, &mp, plate.mapBmp.put()));
            }
            D2D1_RECT_U dstU{0, 0, static_cast<UINT32>(mapW),
                             static_cast<UINT32>(mapH)};
            winrt::check_hresult(plate.mapBmp->CopyFromMemory(
                &dstU, plate.mapPixels.data(),
                static_cast<UINT32>(mapW) * 16u));
            plate.mapKey = mKey;
        }

        if (plate.fxDc != dc.get() || !plate.fxCrop) {
            plate.ClearEffects();
            plate.fxDc = dc.get();
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1Crop, plate.fxCrop.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1Border, plate.fxBorder.put()));
            winrt::check_hresult(dc->CreateEffect(kClsidD2D12DAffineTransform,
                                                  plate.fxMapScale.put()));
            winrt::check_hresult(dc->CreateEffect(kClsidD2D1DisplacementMap,
                                                  plate.fxDisp.put()));
            winrt::check_hresult(
                dc->CreateEffect(kGaussianBlurEffectId, plate.fxBlur.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1ColorMatrix, plate.fxSat.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1ColorMatrix, plate.fxCeil1.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1ColorMatrix, plate.fxCeil2.put()));

            SetD2DEffectVal(plate.fxCrop.get(), 1, static_cast<UINT32>(1));
            SetD2DEffectVal(plate.fxMapScale.get(),
                            D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE,
                            D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR);
            constexpr UINT32 kPropPrecision = 0x80000007u;
            constexpr UINT32 kPrecision32Float = 5u;
            SetD2DEffectVal(plate.fxDisp.get(), kPropPrecision,
                            kPrecision32Float);
            SetD2DEffectVal(plate.fxDisp.get(), 1, static_cast<UINT32>(0));
            SetD2DEffectVal(plate.fxDisp.get(), 2, static_cast<UINT32>(1));
            SetD2DEffectVal(plate.fxBlur.get(), 1, static_cast<UINT32>(1));
            SetD2DEffectVal(plate.fxBlur.get(), 2, static_cast<UINT32>(1));
        }

        plate.fxCrop->SetInput(0, plate.behindBmp.get(), TRUE);
        D2D1_VECTOR_4F cropRect{static_cast<float>(valid.left),
                                static_cast<float>(valid.top),
                                static_cast<float>(valid.right),
                                static_cast<float>(valid.bottom)};
        SetD2DEffectVal(plate.fxCrop.get(), 0, cropRect);
        plate.fxBorder->SetInputEffect(0, plate.fxCrop.get(), TRUE);
        auto glassImg = GetD2DEffectOut(plate.fxBorder.get());

        if (g_settings.glassEdgeStrength > 0) {
            plate.fxMapScale->SetInput(0, plate.mapBmp.get(), TRUE);
            D2D1::Matrix3x2F upMat = D2D1::Matrix3x2F::Scale(
                static_cast<float>(areaSz.cx) / static_cast<float>(mapW),
                static_cast<float>(areaSz.cy) / static_cast<float>(mapH));
            SetD2DEffectVal(plate.fxMapScale.get(),
                            D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, upMat);
            plate.fxDisp->SetInput(0, glassImg.get(), TRUE);
            plate.fxDisp->SetInputEffect(1, plate.fxMapScale.get(), TRUE);
            SetD2DEffectVal(plate.fxDisp.get(), 0, plate.mapScale);
            glassImg = GetD2DEffectOut(plate.fxDisp.get());
        }

        if (blurSigma > 0.15f) {
            plate.fxBlur->SetInput(0, glassImg.get(), TRUE);
            SetD2DEffectVal(plate.fxBlur.get(), 0, blurSigma);
            glassImg = GetD2DEffectOut(plate.fxBlur.get());
        }

        {
            plate.fxSat->SetInput(0, glassImg.get(), TRUE);
            float sat = p.darkMode ? 1.22f : 1.15f;
            float gain = p.darkMode ? 1.02f : 1.04f;
            const float lw[3] = {0.2126f, 0.7152f, 0.0722f};
            D2D1_MATRIX_5X4_F cm{};
            for (int r = 0; r < 3; ++r) {
                for (int cIdx = 0; cIdx < 3; ++cIdx) {
                    cm.m[r][cIdx] =
                        ((1.0f - sat) * lw[r] + (r == cIdx ? sat : 0.0f)) *
                        gain;
                }
            }
            cm.m[3][3] = 1.0f;
            cm.m[4][0] = cm.m[4][1] = cm.m[4][2] = p.darkMode ? 0.004f : 0.014f;
            SetD2DEffectVal(plate.fxSat.get(), 0, cm);
            SetD2DEffectVal(plate.fxSat.get(), 2, static_cast<BOOL>(TRUE));
            glassImg = GetD2DEffectOut(plate.fxSat.get());
        }

        if (p.darkMode && g_settings.glassBrightnessLimit < 100) {
            float ceiling = std::clamp(
                static_cast<float>(g_settings.glassBrightnessLimit) / 100.0f,
                0.30f, 0.99f);
            D2D1_MATRIX_5X4_F m1{}, m2{};
            m1.m[0][0] = m1.m[1][1] = m1.m[2][2] = 1.0f / ceiling;
            m1.m[3][3] = 1.0f;
            m2.m[0][0] = m2.m[1][1] = m2.m[2][2] = ceiling;
            m2.m[3][3] = 1.0f;
            plate.fxCeil1->SetInput(0, glassImg.get(), TRUE);
            SetD2DEffectVal(plate.fxCeil1.get(), 0, m1);
            SetD2DEffectVal(plate.fxCeil1.get(), 2, static_cast<BOOL>(TRUE));
            plate.fxCeil2->SetInputEffect(0, plate.fxCeil1.get(), TRUE);
            SetD2DEffectVal(plate.fxCeil2.get(), 0, m2);
            SetD2DEffectVal(plate.fxCeil2.get(), 2, static_cast<BOOL>(FALSE));
            glassImg = GetD2DEffectOut(plate.fxCeil2.get());
        }

        dc->SetTransform(D2D1::Matrix3x2F::Translation(
            static_cast<float>(offset.x), static_cast<float>(offset.y)));
        D2D1_RECT_F bounds =
            D2D1::RectF(0.0f, 0.0f, static_cast<float>(surfSz.cx),
                        static_cast<float>(surfSz.cy));
        dc->PushAxisAlignedClip(&bounds, D2D1_ANTIALIAS_MODE_ALIASED);
        dc->Clear(D2D1::ColorF(0, 0, 0, 0));

        EnsurePlateGeometries(plate, p);
        ID2D1PathGeometry* clipShape = plate.cachedClipGeom.get();
        bool pushedLayer = false;
        if (p.cornerRadiusPx > 0.5f && clipShape) {
            dc->PushLayer(D2D1::LayerParameters(
                              bounds, clipShape,
                              D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                          nullptr);
            pushedLayer = true;
        }

        D2D1_RECT_F srcRect{static_cast<float>(pad) + fx,
                            static_cast<float>(pad) + fy,
                            static_cast<float>(pad) + fx + p.widthPx,
                            static_cast<float>(pad) + fy + p.heightPx};
        dc->DrawImage(glassImg.get(), nullptr, &srcRect,
                      D2D1_INTERPOLATION_MODE_LINEAR,
                      D2D1_COMPOSITE_MODE_SOURCE_COPY);

        DrawMacOS27GlassLighting(dc.get(), p, clipShape,
                                 plate.cachedOuterGeom.get(),
                                 plate.cachedRimGeom.get());

        if (pushedLayer) dc->PopLayer();
        dc->PopAxisAlignedClip();
        ok = true;
    } catch (...) {
        ok = false;
    }

    HRESULT endHr = interop->EndDraw();
    if (!ok || FAILED(endHr)) return false;
    plate.lastRect = curRect;
    plate.lastFrameId = g_liquidCapture.frameId;
    plate.lastStyleSig = styleSig;
    plate.dirty = false;
    plate.active = true;
    return true;
}

std::wstring g_fontFamilyUi;
std::wstring g_fontFamilyMono;

const std::wstring& ResolveFontFamily() {
    if (!g_fontFamilyUi.empty()) return g_fontFamilyUi;
    g_fontFamilyUi = L"Segoe UI Variable Display";
    if (!g_gfx.dwriteFactory) return g_fontFamilyUi;
    winrt::com_ptr<IDWriteFontCollection> coll;
    if (SUCCEEDED(g_gfx.dwriteFactory->GetSystemFontCollection(coll.put()))) {
        UINT32 idx = 0;
        BOOL exists = FALSE;
        for (PCWSTR cand : {L"SF Pro Rounded", L"SF Pro Display", L"SF Pro Text",
                            L"Segoe UI Variable Display", L"Segoe UI"}) {
            if (SUCCEEDED(coll->FindFamilyName(cand, &idx, &exists)) &&
                exists) {
                g_fontFamilyUi = cand;
                break;
            }
        }
    }
    return g_fontFamilyUi;
}

const std::wstring& ResolveMonoFontFamily() {
    if (!g_fontFamilyMono.empty()) return g_fontFamilyMono;
    g_fontFamilyMono = L"Consolas";
    if (!g_gfx.dwriteFactory) return g_fontFamilyMono;
    winrt::com_ptr<IDWriteFontCollection> coll;
    if (SUCCEEDED(g_gfx.dwriteFactory->GetSystemFontCollection(coll.put()))) {
        UINT32 idx = 0;
        BOOL exists = FALSE;
        for (PCWSTR cand :
             {L"JetBrains Mono", L"SF Mono", L"Cascadia Mono", L"Consolas"}) {
            if (SUCCEEDED(coll->FindFamilyName(cand, &idx, &exists)) &&
                exists) {
                g_fontFamilyMono = cand;
                break;
            }
        }
    }
    return g_fontFamilyMono;
}

// Soft ambient macOS-style drop shadow drawn into a reusable surface for collapsed or expanded shapes
void UpdateShadowSurfaceForRects(
    wuc::CompositionDrawingSurface& surf,
    SIZE& surfSz,
    float surfW,
    float surfH,
    const std::vector<D2D1_ROUNDED_RECT>& shapes,
    float scale,
    bool darkMode) {
    SIZE wantSz{std::max(1L, static_cast<LONG>(std::ceil(surfW))),
                std::max(1L, static_cast<LONG>(std::ceil(surfH)))};
    if (!surf) {
        surf = CreateDrawingSurface(static_cast<float>(wantSz.cx),
                                    static_cast<float>(wantSz.cy));
        surfSz = wantSz;
    } else if (surfSz.cx != wantSz.cx || surfSz.cy != wantSz.cy) {
        auto interop = surf.as<ICompositionDrawingSurfaceInteropLocal>();
        if (FAILED(interop->Resize(wantSz))) {
            surf = CreateDrawingSurface(static_cast<float>(wantSz.cx),
                                        static_cast<float>(wantSz.cy));
        }
        surfSz = wantSz;
    }
    if (!surf) return;
    DrawCompositionSurface(surf, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1BitmapRenderTarget> rt;
        if (FAILED(dc->CreateCompatibleRenderTarget(
                D2D1::SizeF(static_cast<float>(wantSz.cx),
                            static_cast<float>(wantSz.cy)),
                rt.put()))) {
            return;
        }
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0, 0, 0));
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f), br.put());
        if (br) {
            for (const auto& s : shapes) {
                rt->FillRoundedRectangle(s, br.get());
            }
        }
        rt->EndDraw();

        winrt::com_ptr<ID2D1Bitmap> bmp;
        if (FAILED(rt->GetBitmap(bmp.put()))) return;
        winrt::com_ptr<ID2D1Effect> sh;
        if (FAILED(dc->CreateEffect(CLSID_D2D1Shadow, sh.put()))) return;
        sh->SetInput(0, bmp.get());
        sh->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 16.0f * scale);
        sh->SetValue(D2D1_SHADOW_PROP_COLOR,
                     D2D1::Vector4F(0.0f, 0.0f, 0.0f,
                                    darkMode ? 0.56f : 0.30f));
        sh->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, static_cast<UINT32>(1));
        dc->DrawImage(sh.get(), D2D1::Point2F(0.0f, 8.0f * scale));
    });
}

// ============================================================================
// 4. Backends: Actions, Settings, Apps, Everything IPC, Clipboard & Media Worker
// ============================================================================

constexpr GUID kFolderIdAppsFolder = {
    0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};

// Built-in Actions & Quick Keys (inspired by Spotlight & Fluent Search)
const BuiltinAction kBuiltinActions[] = {
    {L"mode_apps", L"Search Applications", L"Filter to installed apps & Store packages",
     L"apps programs launcher mode", L"Ctrl+1", ActionGlyph::Grid,
     D2D1::ColorF(0.20f, 0.56f, 0.96f, 1.0f), false, SpotlightMode::Apps},
    {L"mode_files", L"Search Files & Folders", L"Instant disk search & folder browser",
     L"files folders search find mode", L"Ctrl+2", ActionGlyph::Folder,
     D2D1::ColorF(0.90f, 0.64f, 0.22f, 1.0f), false, SpotlightMode::Files},
    {L"mode_clip", L"Clipboard History", L"View copied text and encrypted screenshots",
     L"clipboard history paste copies mode", L"Ctrl+4", ActionGlyph::Clipboard,
     D2D1::ColorF(0.24f, 0.70f, 0.58f, 1.0f), false, SpotlightMode::Clipboard},
    {L"mode_win", L"Switch Open Windows", L"Focus, minimize, maximize, or close windows",
     L"windows switcher focus open mode", L"win:", ActionGlyph::Monitor,
     D2D1::ColorF(0.28f, 0.54f, 0.80f, 1.0f), false, SpotlightMode::Windows},
    {L"mode_kill", L"Kill Running Process", L"Force terminate processes matching name",
     L"kill process terminate force close end task kl", L"kl", ActionGlyph::Skull,
     D2D1::ColorF(0.88f, 0.28f, 0.30f, 1.0f), false, SpotlightMode::KillProcess},
    {L"mode_bm", L"Browser Bookmarks", L"Search Chrome, Edge, Brave & Vivaldi bookmarks",
     L"bookmarks favorites links browser mode", L"bm:", ActionGlyph::Star,
     D2D1::ColorF(0.88f, 0.68f, 0.22f, 1.0f), false, SpotlightMode::Bookmarks},
    {L"mode_cfg", L"Windows Settings Pages", L"Jump directly to any Windows Settings panel",
     L"settings preferences system mode", L"cfg:", ActionGlyph::Gear,
     D2D1::ColorF(0.46f, 0.52f, 0.64f, 1.0f), false, SpotlightMode::Settings},
    {L"taskmgr", L"Task Manager", L"System resource & process monitor",
     L"task manager performance processes tm", L"tm", ActionGlyph::TaskMgr,
     D2D1::ColorF(0.20f, 0.62f, 0.58f, 1.0f), false, SpotlightMode::All},
    {L"terminal", L"Windows Terminal", L"Open Windows Terminal",
     L"terminal wt shell console", L"wt", ActionGlyph::Terminal,
     D2D1::ColorF(0.18f, 0.20f, 0.25f, 1.0f), false, SpotlightMode::All},
    {L"powershell", L"PowerShell", L"Open Windows PowerShell interactive session",
     L"powershell ps script shell", L"ps", ActionGlyph::Terminal,
     D2D1::ColorF(0.14f, 0.38f, 0.78f, 1.0f), false, SpotlightMode::All},
    {L"cmd", L"Command Prompt", L"Open classic cmd.exe shell",
     L"cmd command prompt dos", L"cmd", ActionGlyph::Terminal,
     D2D1::ColorF(0.26f, 0.28f, 0.33f, 1.0f), false, SpotlightMode::All},
    {L"explorer", L"File Explorer", L"Open a new File Explorer window",
     L"explorer files home", L"fe", ActionGlyph::Folder,
     D2D1::ColorF(0.86f, 0.64f, 0.24f, 1.0f), false, SpotlightMode::All},
    {L"control", L"Control Panel", L"Classic Windows Control Panel",
     L"control panel system", L"cp", ActionGlyph::Sliders,
     D2D1::ColorF(0.40f, 0.50f, 0.64f, 1.0f), false, SpotlightMode::All},
    {L"devmgmt", L"Device Manager", L"Hardware & driver manager",
     L"device manager hardware drivers", L"dm", ActionGlyph::Chip,
     D2D1::ColorF(0.30f, 0.60f, 0.42f, 1.0f), false, SpotlightMode::All},
    {L"services", L"Services", L"Windows background services console",
     L"services background daemon", L"svc", ActionGlyph::List,
     D2D1::ColorF(0.72f, 0.56f, 0.24f, 1.0f), false, SpotlightMode::All},
    {L"regedit", L"Registry Editor", L"Edit Windows Registry",
     L"registry editor regedit", L"reg", ActionGlyph::Key,
     D2D1::ColorF(0.62f, 0.44f, 0.30f, 1.0f), false, SpotlightMode::All},
    {L"lock", L"Lock Screen", L"Lock Windows workstation immediately",
     L"lock workstation screen sleep lk", L"lk", ActionGlyph::Lock,
     D2D1::ColorF(0.36f, 0.48f, 0.72f, 1.0f), false, SpotlightMode::All},
    {L"sleep", L"Sleep", L"Suspend PC to low-power sleep",
     L"sleep suspend standby sl", L"sl", ActionGlyph::Moon,
     D2D1::ColorF(0.44f, 0.42f, 0.74f, 1.0f), false, SpotlightMode::All},
    {L"hibernate", L"Hibernate", L"Save state to disk and power off",
     L"hibernate suspend", L"↵", ActionGlyph::Hibernate,
     D2D1::ColorF(0.42f, 0.50f, 0.64f, 1.0f), true, SpotlightMode::All},
    {L"restart", L"Restart Computer", L"Reboot Windows",
     L"restart reboot pc rb", L"↵", ActionGlyph::Restart,
     D2D1::ColorF(0.86f, 0.46f, 0.28f, 1.0f), true, SpotlightMode::All},
    {L"shutdown", L"Shut Down Computer", L"Power off computer safely",
     L"shutdown power off turn off", L"↵", ActionGlyph::Power,
     D2D1::ColorF(0.86f, 0.28f, 0.28f, 1.0f), true, SpotlightMode::All},
    {L"signout", L"Sign Out", L"Log off current Windows account",
     L"sign out logoff log out", L"↵", ActionGlyph::SignOut,
     D2D1::ColorF(0.80f, 0.42f, 0.54f, 1.0f), true, SpotlightMode::All},
    {L"emptybin", L"Empty Recycle Bin", L"Permanently clear deleted files",
     L"empty recycle bin trash cleanup", L"↵", ActionGlyph::Recycle,
     D2D1::ColorF(0.70f, 0.38f, 0.38f, 1.0f), true, SpotlightMode::All},
    {L"media_play", L"Play / Pause Media", L"Toggle active media playback",
     L"media play pause music spotify", L"⏯", ActionGlyph::Play,
     D2D1::ColorF(0.86f, 0.36f, 0.56f, 1.0f), false, SpotlightMode::All},
    {L"media_next", L"Next Track", L"Skip to next media track",
     L"next track skip song", L"⏭", ActionGlyph::Next,
     D2D1::ColorF(0.86f, 0.36f, 0.56f, 1.0f), false, SpotlightMode::All},
    {L"media_prev", L"Previous Track", L"Return to previous media track",
     L"previous track back song", L"⏮", ActionGlyph::Prev,
     D2D1::ColorF(0.86f, 0.36f, 0.56f, 1.0f), false, SpotlightMode::All},
    {L"vol_mute", L"Mute / Unmute Audio", L"Toggle master system volume mute",
     L"mute sound audio volume", L"🔇", ActionGlyph::Mute,
     D2D1::ColorF(0.50f, 0.56f, 0.72f, 1.0f), false, SpotlightMode::All},
    {L"clearclip", L"Clear Unpinned Clipboard History",
     L"Erase saved unpinned clipboard entries",
     L"clear clipboard history erase", L"↵", ActionGlyph::Clipboard,
     D2D1::ColorF(0.30f, 0.62f, 0.55f, 1.0f), true, SpotlightMode::All},
    {L"reindex", L"Reindex Applications", L"Rescan Start Menu & Store apps",
     L"reindex refresh rescan apps", L"Ctrl+R", ActionGlyph::Update,
     D2D1::ColorF(0.24f, 0.55f, 0.88f, 1.0f), false, SpotlightMode::All},
};
constexpr int kBuiltinActionCount =
    static_cast<int>(sizeof(kBuiltinActions) / sizeof(kBuiltinActions[0]));

const BuiltinSetting kBuiltinSettings[] = {
    {L"Wi-Fi", L"wifi wireless network internet wlan",
     L"ms-settings:network-wifi", ActionGlyph::Wifi,
     D2D1::ColorF(0.24f, 0.56f, 0.85f, 1.0f)},
    {L"Bluetooth & devices", L"bluetooth devices pair audio mouse keyboard",
     L"ms-settings:bluetooth", ActionGlyph::Bluetooth,
     D2D1::ColorF(0.22f, 0.50f, 0.85f, 1.0f)},
    {L"Display", L"display monitor resolution scaling hdr brightness",
     L"ms-settings:display", ActionGlyph::Monitor,
     D2D1::ColorF(0.24f, 0.52f, 0.72f, 1.0f)},
    {L"Sound", L"sound audio volume output input microphone speaker",
     L"ms-settings:sound", ActionGlyph::Speaker,
     D2D1::ColorF(0.76f, 0.38f, 0.48f, 1.0f)},
    {L"Notifications", L"notifications banners focus alerts",
     L"ms-settings:notifications", ActionGlyph::Bell,
     D2D1::ColorF(0.86f, 0.54f, 0.25f, 1.0f)},
    {L"Power & battery", L"power sleep battery saver screen timeout",
     L"ms-settings:powersleep", ActionGlyph::Power,
     D2D1::ColorF(0.32f, 0.64f, 0.42f, 1.0f)},
    {L"Storage", L"storage disk space cleanup sense", L"ms-settings:storagesense",
     ActionGlyph::Folder, D2D1::ColorF(0.76f, 0.56f, 0.24f, 1.0f)},
    {L"Installed apps", L"apps features uninstall programs",
     L"ms-settings:appsfeatures", ActionGlyph::Grid,
     D2D1::ColorF(0.50f, 0.42f, 0.72f, 1.0f)},
    {L"Default apps", L"default apps browser file associations",
     L"ms-settings:defaultapps", ActionGlyph::Grid,
     D2D1::ColorF(0.50f, 0.42f, 0.72f, 1.0f)},
    {L"Startup apps", L"startup boot login programs",
     L"ms-settings:startupapps", ActionGlyph::List,
     D2D1::ColorF(0.46f, 0.50f, 0.60f, 1.0f)},
    {L"Windows Update", L"windows update patches check upgrade",
     L"ms-settings:windowsupdate", ActionGlyph::Update,
     D2D1::ColorF(0.24f, 0.54f, 0.85f, 1.0f)},
    {L"Personalization", L"personalization wallpaper theme taskbar start",
     L"ms-settings:personalization", ActionGlyph::Gear,
     D2D1::ColorF(0.58f, 0.42f, 0.74f, 1.0f)},
    {L"Colors & Theme", L"colors dark mode light accent transparency",
     L"ms-settings:colors", ActionGlyph::Gear,
     D2D1::ColorF(0.58f, 0.42f, 0.74f, 1.0f)},
    {L"Network & Ethernet", L"ethernet wired lan vpn proxy",
     L"ms-settings:network-ethernet", ActionGlyph::Globe,
     D2D1::ColorF(0.20f, 0.48f, 0.78f, 1.0f)},
    {L"VPN", L"vpn virtual private network tunnel", L"ms-settings:network-vpn",
     ActionGlyph::Key, D2D1::ColorF(0.30f, 0.44f, 0.70f, 1.0f)},
    {L"Night light", L"night light warm blue filter",
     L"ms-settings:nightlight", ActionGlyph::Moon,
     D2D1::ColorF(0.42f, 0.40f, 0.70f, 1.0f)},
    {L"Date & time", L"date time clock timezone ntp",
     L"ms-settings:dateandtime", ActionGlyph::Clock,
     D2D1::ColorF(0.36f, 0.52f, 0.70f, 1.0f)},
    {L"Language & region", L"language keyboard input region locale",
     L"ms-settings:regionlanguage", ActionGlyph::Globe,
     D2D1::ColorF(0.30f, 0.54f, 0.72f, 1.0f)},
    {L"Privacy & Camera", L"camera webcam permissions", L"ms-settings:camera",
     ActionGlyph::Camera, D2D1::ColorF(0.30f, 0.52f, 0.68f, 1.0f)},
    {L"Microphone Privacy", L"microphone mic permissions",
     L"ms-settings:privacy-microphone", ActionGlyph::Mic,
     D2D1::ColorF(0.64f, 0.40f, 0.58f, 1.0f)},
    {L"Printers & scanners", L"printers scanners devices",
     L"ms-settings:printers", ActionGlyph::Printer,
     D2D1::ColorF(0.46f, 0.50f, 0.58f, 1.0f)},
    {L"Accounts & Sign-in", L"accounts password pin windows hello",
     L"ms-settings:signinoptions", ActionGlyph::SignOut,
     D2D1::ColorF(0.40f, 0.50f, 0.72f, 1.0f)},
    {L"About PC", L"about specs cpu ram windows version",
     L"ms-settings:about", ActionGlyph::List,
     D2D1::ColorF(0.46f, 0.50f, 0.60f, 1.0f)},
};
constexpr int kBuiltinSettingCount =
    static_cast<int>(sizeof(kBuiltinSettings) / sizeof(kBuiltinSettings[0]));

// Shared catalog & async worker data
std::vector<AppEntry> g_apps;
UINT g_indexGen = 0;
bool g_indexing = false;
bool g_indexedOnce = false;
bool g_menuDirty = true;

std::vector<BookmarkEntry> g_bookmarks;
UINT g_bookmarkGen = 0;
std::atomic<ULONGLONG> g_bookmarkStamp{0};
double g_bookmarkCheckedAt = -1e9;

std::vector<FileEntry> g_recentFiles;
UINT g_recentGen = 0;
ULONGLONG g_recentStamp = 0;
double g_recentCheckedAt = -1e9;

std::vector<ClipEntry> g_clips;
std::vector<WindowEntry> g_windows;
std::vector<ProcessGroup> g_processes;

// Frecency & pinned storage (per user SID)
struct LearnEntry {
    std::wstring query;
    std::wstring appId;
    int stamp = 0;
};
std::vector<LearnEntry> g_learned;
std::unordered_map<std::wstring, std::pair<int, int>> g_usageStats;  // key -> (uses, lastUsed)
std::unordered_set<std::wstring> g_pinnedAppIds;

std::wstring UsageKeyStr() { return L"spot.usage_" + CurrentUserSid(); }
std::wstring PinnedKeyStr() { return L"spot.pins_" + CurrentUserSid(); }
std::wstring LearnKeyStr() { return L"spot.learn_" + CurrentUserSid(); }

void LoadUserStats() {
    g_usageStats.clear();
    std::wstring raw(32768, L'\0');
    size_t len = Wh_GetStringValue(UsageKeyStr().c_str(), raw.data(), raw.size());
    raw.resize(len);
    size_t pos = 0;
    while (pos < raw.size()) {
        size_t nl = raw.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = raw.size();
        std::wstring_view line(raw.data() + pos, nl - pos);
        pos = nl + 1;
        size_t t1 = line.find(L'\t');
        size_t t2 = (t1 == std::wstring_view::npos) ? t1 : line.find(L'\t', t1 + 1);
        if (t1 != std::wstring_view::npos && t2 != std::wstring_view::npos) {
            std::wstring key(line.substr(0, t1));
            int uses = _wtoi(std::wstring(line.substr(t1 + 1, t2 - t1 - 1)).c_str());
            int last = _wtoi(std::wstring(line.substr(t2 + 1)).c_str());
            if (!key.empty() && uses > 0) g_usageStats[key] = {uses, last};
        }
    }

    g_pinnedAppIds.clear();
    std::wstring pins(16384, L'\0');
    size_t plen = Wh_GetStringValue(PinnedKeyStr().c_str(), pins.data(), pins.size());
    pins.resize(plen);
    pos = 0;
    while (pos < pins.size()) {
        size_t nl = pins.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = pins.size();
        std::wstring k = TrimBlanks(std::wstring_view(pins.data() + pos, nl - pos));
        pos = nl + 1;
        if (!k.empty()) g_pinnedAppIds.insert(FoldString(k));
    }

    g_learned.clear();
    std::wstring lraw(16384, L'\0');
    size_t llen = Wh_GetStringValue(LearnKeyStr().c_str(), lraw.data(), lraw.size());
    lraw.resize(llen);
    pos = 0;
    while (pos < lraw.size()) {
        size_t nl = lraw.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = lraw.size();
        std::wstring_view line(lraw.data() + pos, nl - pos);
        pos = nl + 1;
        size_t t1 = line.find(L'\t');
        size_t t2 = (t1 == std::wstring_view::npos) ? t1 : line.find(L'\t', t1 + 1);
        if (t1 != std::wstring_view::npos && t2 != std::wstring_view::npos) {
            LearnEntry le;
            le.query = std::wstring(line.substr(0, t1));
            le.appId = std::wstring(line.substr(t1 + 1, t2 - t1 - 1));
            le.stamp = _wtoi(std::wstring(line.substr(t2 + 1)).c_str());
            if (!le.query.empty() && !le.appId.empty()) g_learned.push_back(le);
        }
    }
}

void SaveUsageStats() {
    std::wstring out;
    int saved = 0;
    for (const auto& kv : g_usageStats) {
        if (kv.second.first <= 0) continue;
        out += kv.first + L"\t" + std::to_wstring(kv.second.first) + L"\t" +
               std::to_wstring(kv.second.second) + L"\n";
        if (++saved >= 400) break;
    }
    Wh_SetStringValue(UsageKeyStr().c_str(), out.c_str());
}

void SavePinnedApps() {
    std::wstring out;
    for (const auto& k : g_pinnedAppIds) {
        out += k + L"\n";
    }
    Wh_SetStringValue(PinnedKeyStr().c_str(), out.c_str());
}

void SaveLearned() {
    std::wstring out;
    for (size_t i = 0; i < g_learned.size() && i < 200; ++i) {
        out += g_learned[i].query + L"\t" + g_learned[i].appId + L"\t" +
               std::to_wstring(g_learned[i].stamp) + L"\n";
    }
    Wh_SetStringValue(LearnKeyStr().c_str(), out.c_str());
}

void RecordAppPick(AppEntry& app, const std::wstring& queryFold) {
    app.uses++;
    app.lastUsed = NowStamp();
    if (!app.usageKey.empty()) {
        g_usageStats[app.usageKey] = {app.uses, app.lastUsed};
        SaveUsageStats();
    }
    if (!queryFold.empty() && !app.id.empty()) {
        for (size_t i = 0; i < g_learned.size(); ++i) {
            if (g_learned[i].query == queryFold && g_learned[i].appId == app.id) {
                g_learned.erase(g_learned.begin() + static_cast<std::ptrdiff_t>(i));
                break;
            }
        }
        g_learned.insert(g_learned.begin(),
                         LearnEntry{queryFold, app.id, NowStamp()});
        if (g_learned.size() > 200) g_learned.resize(200);
        SaveLearned();
    }
}

int FrecencyScore(const AppEntry& a) {
    if (a.uses <= 0) return 0;
    int last = a.lastUsed > 0 ? a.lastUsed : NowStamp() - 60 * 86400;
    double days = std::max(0, NowStamp() - last) / 86400.0;
    double decayed = static_cast<double>(a.uses) * std::pow(0.5, days / 30.0);
    return std::min(65, static_cast<int>(std::lround(decayed * 4.0)));
}

// ===== Background App Indexing & Folder Watcher =====
bool IsPackagedId(const std::wstring& id) {
    return id.find(L'!') != std::wstring::npos &&
           id.find(L'\\') == std::wstring::npos &&
           id.find(L'/') == std::wstring::npos &&
           id.find(L':') == std::wstring::npos;
}

std::wstring ShellItemStr(IShellItem2* item, PCWSTR propName) {
    PROPERTYKEY key{};
    if (FAILED(PSGetPropertyKeyFromName(propName, &key))) return {};
    PWSTR s = nullptr;
    std::wstring out;
    if (SUCCEEDED(item->GetString(key, &s)) && s) {
        out = s;
        CoTaskMemFree(s);
    }
    return out;
}

struct LnkMeta {
    std::wstring target;
    std::wstring args;
    std::wstring desc;
    bool ok = false;
};

LnkMeta ReadLnkMeta(const std::wstring& lnkPath) {
    LnkMeta info;
    winrt::com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(link.put()))) ||
        !link) {
        return info;
    }
    auto pf = link.try_as<IPersistFile>();
    if (!pf || FAILED(pf->Load(lnkPath.c_str(), STGM_READ))) return info;
    wchar_t buf[MAX_PATH * 4]{};
    WIN32_FIND_DATAW fd{};
    if (SUCCEEDED(link->GetPath(buf, ARRAYSIZE(buf), &fd, SLGP_RAWPATH)) &&
        buf[0]) {
        std::wstring t = TrimBlanks(ExpandedPath(buf));
        while (t.size() >= 2 && t.front() == L'"' && t.back() == L'"') {
            t = TrimBlanks(t.substr(1, t.size() - 2));
        }
        wchar_t longBuf[MAX_PATH * 4]{};
        DWORD ln = GetLongPathNameW(t.c_str(), longBuf, ARRAYSIZE(longBuf));
        if (ln > 0 && ln < ARRAYSIZE(longBuf)) t.assign(longBuf, ln);
        info.target = std::move(t);
        info.ok = !info.target.empty();
    }
    if (SUCCEEDED(link->GetArguments(buf, ARRAYSIZE(buf))) && buf[0]) {
        info.args = buf;
    }
    if (SUCCEEDED(link->GetDescription(buf, ARRAYSIZE(buf))) && buf[0]) {
        info.desc = buf;
    }
    return info;
}

bool IsUtilityLnkOrExe(const std::wstring& name,
                       const std::wstring& target,
                       const std::wstring& args) {
    std::wstring fn = FoldString(name);
    std::wstring ft = FoldString(target);
    std::wstring fa = FoldString(args);
    std::wstring base = FileNameOf(ft);
    if (base == L"msiexec.exe") return true;
    if (base.find(L"unins") == 0 || base == L"uninstall.exe" ||
        base == L"unwise.exe") {
        return true;
    }
    if (EndsWithNoCase(ft, L".chm") || EndsWithNoCase(ft, L".html") ||
        EndsWithNoCase(ft, L".htm") || EndsWithNoCase(ft, L".txt") ||
        EndsWithNoCase(ft, L".pdf") || EndsWithNoCase(ft, L".url")) {
        return true;
    }
    if (fn.find(L"uninstall") == 0 || EndsWithNoCase(fn, L" uninstall") ||
        fn.find(L"readme") == 0 || fn.find(L"read me") == 0) {
        return true;
    }
    if (fa.find(L"uninstall") != std::wstring::npos ||
        fa.find(L"removeonly") != std::wstring::npos) {
        return true;
    }
    return false;
}

struct IndexedAppsPayload {
    UINT gen = 0;
    std::vector<AppEntry> apps;
};

void ScanShortcutDirForApps(const std::wstring& dir,
                            int depth,
                            std::vector<AppEntry>& out,
                            std::unordered_set<std::wstring>& seenKeys,
                            std::unordered_set<std::wstring>& seenNames) {
    if (dir.empty() || depth > 4) return;
    WIN32_FIND_DATAW fd{};
    HANDLE hf = FindFirstFileW(JoinPath(dir, L"*").c_str(), &fd);
    if (hf == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.cFileName[0] == L'.' &&
            (fd.cFileName[1] == 0 ||
             (fd.cFileName[1] == L'.' && fd.cFileName[2] == 0))) {
            continue;
        }
        std::wstring full = JoinPath(dir, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanShortcutDirForApps(full, depth + 1, out, seenKeys, seenNames);
            continue;
        }
        if (!EndsWithNoCase(fd.cFileName, L".lnk")) continue;
        std::wstring stem(fd.cFileName);
        if (stem.size() > 4) stem.resize(stem.size() - 4);
        LnkMeta lm = ReadLnkMeta(full);
        if (IsUtilityLnkOrExe(stem, lm.target, lm.args)) continue;
        std::wstring k =
            !lm.target.empty()
                ? (FoldString(lm.target) + L"|" + FoldString(TrimBlanks(lm.args)))
                : (L"name:" + FoldString(stem));
        if (!seenKeys.insert(k).second) continue;
        if (!seenNames.insert(FoldString(stem)).second && lm.args.empty()) {
            continue;
        }
        AppEntry a;
        a.name = stem;
        a.folded = FoldString(stem);
        a.id = full;
        a.target = lm.target;
        a.args = lm.args;
        a.launch = full;
        a.usageKey = FoldString(full);
        a.subtitle = lm.desc.empty() ? L"Application" : lm.desc;
        out.push_back(std::move(a));
    } while (FindNextFileW(hf, &fd));
    FindClose(hf);
}

DWORD WINAPI IndexWorkerProc(LPVOID param) {
    UINT gen = static_cast<UINT>(reinterpret_cast<uintptr_t>(param));
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    std::vector<AppEntry> out;
    std::unordered_set<std::wstring> seenKeys;
    std::unordered_set<std::wstring> seenNames;

    // 1. Enumerate Common + User Start Menu & Desktop shortcuts (.lnk)
    const KNOWNFOLDERID shortcutDirs[] = {
        FOLDERID_CommonPrograms, FOLDERID_Programs, FOLDERID_Desktop,
        FOLDERID_PublicDesktop};
    for (const auto& fid : shortcutDirs) {
        ScanShortcutDirForApps(KnownFolderPath(fid), 0, out, seenKeys,
                               seenNames);
    }

    // 2. Enumerate UWP / MSIX & registered apps in shell:AppsFolder
    winrt::com_ptr<IShellItem> root;
    if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0, nullptr,
                                            IID_PPV_ARGS(root.put()))) &&
        root) {
        winrt::com_ptr<IEnumShellItems> en;
        if (SUCCEEDED(root->BindToHandler(nullptr, BHID_EnumItems,
                                          IID_PPV_ARGS(en.put()))) &&
            en) {
            winrt::com_ptr<IShellItem> child;
            ULONG got = 0;
            while (en->Next(1, child.put(), &got) == S_OK && got == 1) {
                PWSTR disp = nullptr;
                PWSTR parse = nullptr;
                if (SUCCEEDED(child->GetDisplayName(SIGDN_NORMALDISPLAY, &disp)) &&
                    disp && *disp &&
                    SUCCEEDED(child->GetDisplayName(
                        SIGDN_PARENTRELATIVEPARSING, &parse)) &&
                    parse && *parse) {
                    std::wstring name = disp;
                    std::wstring id = parse;
                    std::wstring target;
                    std::wstring args;
                    if (auto i2 = child.try_as<IShellItem2>()) {
                        target = ShellItemStr(i2.get(),
                                              L"System.Link.TargetParsingPath");
                        args = ShellItemStr(i2.get(), L"System.Link.Arguments");
                    }
                    bool packaged = IsPackagedId(id);
                    if (!IsUtilityLnkOrExe(name, target, args)) {
                        std::wstring dedupKey =
                            packaged
                                ? (L"aumid:" + FoldString(id))
                                : (!target.empty()
                                       ? (FoldString(target) + L"|" +
                                          FoldString(TrimBlanks(args)))
                                       : (L"name:" + FoldString(name)));
                        if (seenKeys.insert(dedupKey).second &&
                            (packaged ||
                             seenNames.insert(FoldString(name)).second)) {
                            AppEntry a;
                            a.name = name;
                            a.folded = FoldString(name);
                            a.id = id;
                            a.target = target;
                            a.args = args;
                            a.usageKey = FoldString(id);
                            a.subtitle = packaged ? L"Store App" : L"Application";
                            out.push_back(std::move(a));
                        }
                    }
                }
                if (disp) CoTaskMemFree(disp);
                if (parse) CoTaskMemFree(parse);
                child = nullptr;
            }
        }
    }

    // 3. Exact-match command tools on %PATH% (Windows built-in utilities & CLI tools)
    wchar_t pathEnv[32768]{};
    DWORD pLen = GetEnvironmentVariableW(L"PATH", pathEnv, ARRAYSIZE(pathEnv));
    if (pLen > 0 && pLen < ARRAYSIZE(pathEnv)) {
        std::unordered_set<std::wstring> seenCmds;
        std::wstring rawPath(pathEnv, pLen);
        size_t pos = 0;
        while (pos < rawPath.size()) {
            size_t semi = rawPath.find(L';', pos);
            if (semi == std::wstring::npos) semi = rawPath.size();
            std::wstring dir = TrimBlanks(
                std::wstring_view(rawPath.data() + pos, semi - pos));
            pos = semi + 1;
            if (dir.empty()) continue;
            WIN32_FIND_DATAW fd{};
            HANDLE hf = FindFirstFileW(JoinPath(dir, L"*.exe").c_str(), &fd);
            if (hf == INVALID_HANDLE_VALUE) continue;
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                std::wstring fname = fd.cFileName;
                std::wstring lowFile = FoldString(fname);
                if (seenNames.count(lowFile.substr(0, lowFile.size() - 4)) ||
                    !seenCmds.insert(lowFile).second) {
                    continue;
                }
                std::wstring stem = fname.substr(0, fname.size() - 4);
                AppEntry a;
                a.name = stem;
                a.folded = FoldString(stem);
                a.id = JoinPath(dir, fname);
                a.target = a.id;
                a.launch = a.id;
                a.usageKey = FoldString(a.id);
                a.exactOnly = true;
                a.foldedFile = lowFile;
                a.subtitle = L"Run command";
                out.push_back(std::move(a));
            } while (FindNextFileW(hf, &fd));
            FindClose(hf);
        }
    }

    std::sort(out.begin(), out.end(), [](const AppEntry& a, const AppEntry& b) {
        if (a.exactOnly != b.exactOnly) return !a.exactOnly;
        return CompareStringOrdinal(a.folded.c_str(), -1, b.folded.c_str(), -1,
                                    TRUE) == CSTR_LESS_THAN;
    });

    CoUninitialize();
    auto* payload = new IndexedAppsPayload{gen, std::move(out)};
    if (!g_hostHwnd ||
        !PostMessageW(g_hostHwnd, kWmAppIndexDone, 0,
                      reinterpret_cast<LPARAM>(payload))) {
        delete payload;
    }
    return 0;
}

void StartIndex() {
    g_menuDirty = false;
    g_indexing = true;
    const UINT gen = ++g_indexGen;
    HANDLE th = CreateThread(
        nullptr, 0, IndexWorkerProc,
        reinterpret_cast<LPVOID>(static_cast<uintptr_t>(gen)), 0, nullptr);
    if (th) CloseHandle(th);
}

// Start Menu change watcher (event-driven ReadDirectoryChangesW, zero polling)
HANDLE g_watchThread = nullptr;
HANDLE g_watchStop = nullptr;

DWORD WINAPI StartMenuWatchProc(LPVOID) {
    std::vector<std::wstring> dirs = {
        KnownFolderPath(FOLDERID_Programs),
        KnownFolderPath(FOLDERID_CommonPrograms),
        KnownFolderPath(FOLDERID_Desktop)};
    struct WatchDir {
        HANDLE h = INVALID_HANDLE_VALUE;
        OVERLAPPED ov{};
        BYTE buf[4096]{};
    };
    std::vector<WatchDir*> list;
    std::vector<HANDLE> waits = {g_watchStop};
    for (const auto& d : dirs) {
        if (d.empty()) continue;
        HANDLE h = CreateFileW(
            d.c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr);
        if (h == INVALID_HANDLE_VALUE) continue;
        auto* wd = new WatchDir();
        wd->h = h;
        wd->ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        ReadDirectoryChangesW(
            wd->h, wd->buf, sizeof(wd->buf), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
            nullptr, &wd->ov, nullptr);
        list.push_back(wd);
        waits.push_back(wd->ov.hEvent);
    }
    ULONGLONG lastPost = 0;
    while (!list.empty()) {
        DWORD res = WaitForMultipleObjects(static_cast<DWORD>(waits.size()),
                                           waits.data(), FALSE, INFINITE);
        if (res < WAIT_OBJECT_0 + 1 ||
            res >= WAIT_OBJECT_0 + static_cast<DWORD>(waits.size())) {
            break;
        }
        size_t idx = static_cast<size_t>(res - WAIT_OBJECT_0 - 1);
        if (idx < list.size()) {
            DWORD xfer = 0;
            BOOL okRes =
                GetOverlappedResult(list[idx]->h, &list[idx]->ov, &xfer, FALSE);
            ResetEvent(list[idx]->ov.hEvent);
            BOOL rearmed = okRes && ReadDirectoryChangesW(
                list[idx]->h, list[idx]->buf, sizeof(list[idx]->buf), TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_LAST_WRITE,
                nullptr, &list[idx]->ov, nullptr);
            if (!rearmed) {
                CancelIo(list[idx]->h);
                CloseHandle(list[idx]->ov.hEvent);
                CloseHandle(list[idx]->h);
                delete list[idx];
                list.erase(list.begin() + static_cast<ptrdiff_t>(idx));
                waits.erase(waits.begin() + static_cast<ptrdiff_t>(idx + 1));
                Sleep(50);
                continue;
            }
            ULONGLONG now = GetTickCount64();
            if (now - lastPost >= 1500 && g_hostHwnd) {
                lastPost = now;
                PostMessageW(g_hostHwnd, kWmAppMenuChanged, 0, 0);
            }
        }
    }
    for (auto* wd : list) {
        CancelIo(wd->h);
        CloseHandle(wd->ov.hEvent);
        CloseHandle(wd->h);
        delete wd;
    }
    return 0;
}

void StartWatcher() {
    if (g_watchThread) return;
    g_watchStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_watchThread =
        CreateThread(nullptr, 0, StartMenuWatchProc, nullptr, 0, nullptr);
}

void StopWatcher() {
    if (g_watchStop) SetEvent(g_watchStop);
    if (g_watchThread) {
        WaitForSingleObject(g_watchThread, 1500);
        CloseHandle(g_watchThread);
        g_watchThread = nullptr;
    }
    if (g_watchStop) {
        CloseHandle(g_watchStop);
        g_watchStop = nullptr;
    }
}

// ===== Background Media Worker (App Icons + File Thumbs + Quick Look Images) =====
enum class MediaJobKind { AppIcon, FileThumb, QuickLookImage };

struct MediaJob {
    MediaJobKind kind = MediaJobKind::FileThumb;
    int appIdx = -1;
    UINT appGen = 0;
    std::wstring path;
    std::wstring aumid;
    UINT previewGen = 0;
};

struct ThumbCacheEntry {
    winrt::com_ptr<IWICBitmapSource> wic;
    uint64_t boundDeviceGen = 0;
    winrt::com_ptr<ID2D1Bitmap> d2d;
    bool failed = false;
};

std::unordered_map<std::wstring, ThumbCacheEntry> g_thumbCache;
std::unordered_set<std::wstring> g_thumbPending;
CRITICAL_SECTION g_mediaLock;
std::deque<MediaJob> g_mediaQueue;
HANDLE g_mediaEvent = nullptr;
HANDLE g_mediaStop = nullptr;
HANDLE g_mediaThread = nullptr;

struct QuickLookState {
    bool open = false;
    UINT gen = 0;
    std::wstring path;
    bool isImage = false;
    bool isText = false;
    bool loaded = false;
    bool failed = false;
    std::vector<std::wstring> lines;
    int scroll = 0;
    int hoveredAction = -1;
    CachedBitmapSlot previewBmp;
    D2D1_RECT_F actionRects[8]{};
    int actionCount = 0;
};
QuickLookState g_ql;

struct AsyncBitmapDone {
    MediaJobKind kind = MediaJobKind::FileThumb;
    int appIdx = -1;
    UINT appGen = 0;
    UINT previewGen = 0;
    std::wstring key;
    winrt::com_ptr<IWICBitmapSource> wic;
};

winrt::com_ptr<IWICBitmapSource> ConvertWicToPbgra(IWICImagingFactory* wic,
                                                   IWICBitmapSource* src,
                                                   int maxSide) {
    if (!wic || !src) return nullptr;
    UINT w = 0, h = 0;
    if (FAILED(src->GetSize(&w, &h)) || w == 0 || h == 0) return nullptr;
    winrt::com_ptr<IWICBitmapSource> cur;
    cur.copy_from(src);
    if (maxSide > 0 && (static_cast<int>(w) > maxSide ||
                        static_cast<int>(h) > maxSide)) {
        UINT nw = w >= h ? static_cast<UINT>(maxSide)
                         : std::max(1u, w * static_cast<UINT>(maxSide) / h);
        UINT nh = h > w ? static_cast<UINT>(maxSide)
                        : std::max(1u, h * static_cast<UINT>(maxSide) / w);
        winrt::com_ptr<IWICBitmapScaler> scaler;
        if (SUCCEEDED(wic->CreateBitmapScaler(scaler.put())) &&
            SUCCEEDED(scaler->Initialize(cur.get(), nw, nh,
                                         WICBitmapInterpolationModeFant))) {
            cur = scaler;
        }
    }
    winrt::com_ptr<IWICFormatConverter> conv;
    if (FAILED(wic->CreateFormatConverter(conv.put())) ||
        FAILED(conv->Initialize(cur.get(), GUID_WICPixelFormat32bppPBGRA,
                                WICBitmapDitherTypeNone, nullptr, 0.0,
                                WICBitmapPaletteTypeCustom))) {
        return nullptr;
    }
    return conv;
}

winrt::com_ptr<IWICBitmapSource> DecodeHBitmapToWic(IWICImagingFactory* wic,
                                                    HBITMAP hbmp,
                                                    int maxSide) {
    if (!wic || !hbmp) return nullptr;
    winrt::com_ptr<IWICBitmap> wb;
    if (FAILED(wic->CreateBitmapFromHBITMAP(hbmp, nullptr, WICBitmapUseAlpha,
                                            wb.put()))) {
        return nullptr;
    }
    UINT w = 0, h = 0;
    if (SUCCEEDED(wb->GetSize(&w, &h)) && w > 0 && h > 0) {
        WICRect rc{0, 0, static_cast<INT>(w), static_cast<INT>(h)};
        winrt::com_ptr<IWICBitmapLock> lk;
        if (SUCCEEDED(wb->Lock(&rc, WICBitmapLockRead, lk.put()))) {
            UINT sz = 0;
            BYTE* ptr = nullptr;
            if (SUCCEEDED(lk->GetDataPointer(&sz, &ptr)) && ptr) {
                bool anyAlpha = false;
                for (UINT i = 3; i < sz; i += 4) {
                    if (ptr[i] != 0) {
                        anyAlpha = true;
                        break;
                    }
                }
                lk = nullptr;
                if (!anyAlpha) {
                    wb = nullptr;
                    if (FAILED(wic->CreateBitmapFromHBITMAP(
                            hbmp, nullptr, WICBitmapIgnoreAlpha, wb.put()))) {
                        return nullptr;
                    }
                }
            }
        }
    }
    return ConvertWicToPbgra(wic, wb.get(), maxSide);
}

winrt::com_ptr<IWICBitmapSource> DecodeAppIconWic(IWICImagingFactory* wic,
                                                  const std::wstring& path,
                                                  const std::wstring& aumid) {
    if (!wic) return nullptr;
    auto tryShellItem = [&](IShellItem* item) -> winrt::com_ptr<IWICBitmapSource> {
        if (!item) return nullptr;
        winrt::com_ptr<IShellItemImageFactory> f;
        if (FAILED(item->QueryInterface(IID_PPV_ARGS(f.put())))) return nullptr;
        HBITMAP hbm = nullptr;
        if (SUCCEEDED(f->GetImage({96, 96},
                                  SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                                  &hbm)) &&
            hbm) {
            auto r = DecodeHBitmapToWic(wic, hbm, 96);
            DeleteObject(hbm);
            return r;
        }
        return nullptr;
    };
    if (!path.empty()) {
        winrt::com_ptr<IShellItem> si;
        std::wstring real = NativeSystemPath(path);
        if (SUCCEEDED(SHCreateItemFromParsingName(real.c_str(), nullptr,
                                                  IID_PPV_ARGS(si.put())))) {
            if (auto r = tryShellItem(si.get())) return r;
        }
    }
    if (!aumid.empty()) {
        winrt::com_ptr<IShellItem> si;
        if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0,
                                                aumid.c_str(),
                                                IID_PPV_ARGS(si.put())))) {
            if (auto r = tryShellItem(si.get())) return r;
        }
    }
    if (!path.empty()) {
        SHFILEINFOW sfi{};
        std::wstring real = NativeSystemPath(path);
        if (SHGetFileInfoW(real.c_str(), 0, &sfi, sizeof(sfi),
                           SHGFI_ICON | SHGFI_LARGEICON) &&
            sfi.hIcon) {
            winrt::com_ptr<IWICBitmap> wb;
            winrt::com_ptr<IWICBitmapSource> r;
            if (SUCCEEDED(wic->CreateBitmapFromHICON(sfi.hIcon, wb.put()))) {
                r = ConvertWicToPbgra(wic, wb.get(), 96);
            }
            DestroyIcon(sfi.hIcon);
            if (r) return r;
        }
    }
    return nullptr;
}

winrt::com_ptr<IWICBitmapSource> DecodeFileImageWic(IWICImagingFactory* wic,
                                                    const std::wstring& path,
                                                    int maxSide) {
    if (!wic || path.empty()) return nullptr;
    winrt::com_ptr<IWICBitmapDecoder> dec;
    std::wstring dir = ClipImageDir();
    bool isClip = !dir.empty() && StartsWithNoCase(path, dir);
    if (isClip) {
        std::vector<BYTE> enc, png;
        if (!ReadFileBytesRaw(path, enc, 32 << 20) || !UnprotectBytes(enc, png)) {
            return nullptr;
        }
        winrt::com_ptr<IWICStream> stream;
        if (FAILED(wic->CreateStream(stream.put())) ||
            FAILED(stream->InitializeFromMemory(
                png.data(), static_cast<DWORD>(png.size()))) ||
            FAILED(wic->CreateDecoderFromStream(stream.get(), nullptr,
                                                WICDecodeMetadataCacheOnLoad,
                                                dec.put()))) {
            return nullptr;
        }
    } else {
        if (FAILED(wic->CreateDecoderFromFilename(
                path.c_str(), nullptr, GENERIC_READ,
                WICDecodeMetadataCacheOnLoad, dec.put()))) {
            return nullptr;
        }
    }
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    if (FAILED(dec->GetFrame(0, frame.put()))) return nullptr;
    return ConvertWicToPbgra(wic, frame.get(), maxSide);
}

DWORD WINAPI MediaWorkerProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    winrt::com_ptr<IWICImagingFactory> wic;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(wic.put()));
    HANDLE waits[2] = {g_mediaStop, g_mediaEvent};
    for (;;) {
        DWORD wr = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (wr == WAIT_OBJECT_0 || wr == WAIT_FAILED) break;
        for (;;) {
            MediaJob job;
            bool hasJob = false;
            EnterCriticalSection(&g_mediaLock);
            if (!g_mediaQueue.empty()) {
                job = std::move(g_mediaQueue.front());
                g_mediaQueue.pop_front();
                hasJob = true;
            }
            LeaveCriticalSection(&g_mediaLock);
            if (!hasJob || WaitForSingleObject(g_mediaStop, 0) == WAIT_OBJECT_0) {
                break;
            }
            auto* res = new AsyncBitmapDone();
            res->kind = job.kind;
            res->appIdx = job.appIdx;
            res->appGen = job.appGen;
            res->previewGen = job.previewGen;
            res->key = job.path;
            if (job.kind == MediaJobKind::AppIcon) {
                res->wic = DecodeAppIconWic(wic.get(), job.path, job.aumid);
            } else if (job.kind == MediaJobKind::FileThumb) {
                res->wic = DecodeFileImageWic(wic.get(), job.path, 96);
            } else {
                res->wic = DecodeFileImageWic(wic.get(), job.path, 520);
            }
            if (!g_hostHwnd ||
                !PostMessageW(
                    g_hostHwnd,
                    job.kind == MediaJobKind::AppIcon ? kWmAppIconDone
                                                      : kWmAppThumbDone,
                    0, reinterpret_cast<LPARAM>(res))) {
                delete res;
            }
        }
    }
    wic = nullptr;
    CoUninitialize();
    return 0;
}

void StartMediaWorker() {
    if (g_mediaThread) return;
    InitializeCriticalSection(&g_mediaLock);
    g_mediaEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_mediaStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_mediaThread =
        CreateThread(nullptr, 0, MediaWorkerProc, nullptr, 0, nullptr);
}

void StopMediaWorker() {
    if (g_mediaStop) SetEvent(g_mediaStop);
    if (g_mediaEvent) SetEvent(g_mediaEvent);
    if (g_mediaThread) {
        WaitForSingleObject(g_mediaThread, 1500);
        CloseHandle(g_mediaThread);
        g_mediaThread = nullptr;
    }
    if (g_mediaEvent) {
        CloseHandle(g_mediaEvent);
        g_mediaEvent = nullptr;
    }
    if (g_mediaStop) {
        CloseHandle(g_mediaStop);
        g_mediaStop = nullptr;
    }
    DeleteCriticalSection(&g_mediaLock);
}

void EnqueueMediaJob(MediaJob job) {
    if (!g_mediaThread) return;
    EnterCriticalSection(&g_mediaLock);
    if (g_mediaQueue.size() < 64) {
        if (job.kind == MediaJobKind::QuickLookImage) {
            g_mediaQueue.push_front(std::move(job));
        } else {
            g_mediaQueue.push_back(std::move(job));
        }
    }
    LeaveCriticalSection(&g_mediaLock);
    SetEvent(g_mediaEvent);
}

void ClearMediaQueue() {
    if (!g_mediaThread) return;
    EnterCriticalSection(&g_mediaLock);
    g_mediaQueue.clear();
    LeaveCriticalSection(&g_mediaLock);
    g_thumbPending.clear();
}

ID2D1Bitmap* GetCachedD2DBitmap(CachedBitmapSlot& slot,
                                ID2D1DeviceContext* dc) {
    if (!slot.wic || !dc) return nullptr;
    if (slot.d2d && slot.boundDeviceGen == g_gfx.deviceGen) {
        return slot.d2d.get();
    }
    slot.d2d = nullptr;
    slot.boundDeviceGen = 0;
    if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(slot.wic.get(), nullptr,
                                                slot.d2d.put())) &&
        slot.d2d) {
        slot.boundDeviceGen = g_gfx.deviceGen;
    }
    return slot.d2d.get();
}

// ===== Lightweight JSON + Chromium / Edge / Brave / Opera Bookmarks =====
namespace JsonMini {

struct Value {
    enum class Type { Null, Bool, Num, Str, Arr, Obj } type = Type::Null;
    std::wstring str;
    std::vector<Value> items;
    std::vector<std::pair<std::wstring, Value>> members;
    const Value* Get(const wchar_t* k) const {
        if (type != Type::Obj) return nullptr;
        for (const auto& m : members) {
            if (m.first == k) return &m.second;
        }
        return nullptr;
    }
    std::wstring Str(const wchar_t* k) const {
        const Value* v = Get(k);
        return (v && v->type == Type::Str) ? v->str : std::wstring();
    }
};

class Parser {
public:
    explicit Parser(const std::string& text)
        : p_(text.c_str()), n_(text.size()) {}
    bool Parse(Value& out) {
        SkipWs();
        return Val(out, 0);
    }

private:
    void SkipWs() {
        while (i_ < n_ && (p_[i_] == ' ' || p_[i_] == '\t' ||
                           p_[i_] == '\r' || p_[i_] == '\n')) {
            ++i_;
        }
    }
    static void AppendCp(std::wstring& out, unsigned cp) {
        if (cp <= 0xFFFF) {
            out += static_cast<wchar_t>(cp);
        } else if (cp <= 0x10FFFF) {
            cp -= 0x10000;
            out += static_cast<wchar_t>(0xD800 + (cp >> 10));
            out += static_cast<wchar_t>(0xDC00 + (cp & 0x3FF));
        }
    }
    bool Hex4(unsigned& out) {
        if (i_ + 4 > n_) return false;
        out = 0;
        for (int k = 0; k < 4; ++k) {
            char c = p_[i_++];
            out <<= 4;
            if (c >= '0' && c <= '9') out |= static_cast<unsigned>(c - '0');
            else if (c >= 'a' && c <= 'f') out |= static_cast<unsigned>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') out |= static_cast<unsigned>(c - 'A' + 10);
            else return false;
        }
        return true;
    }
    bool Str(std::wstring& out) {
        if (i_ >= n_ || p_[i_] != '"') return false;
        ++i_;
        while (i_ < n_) {
            unsigned char c = static_cast<unsigned char>(p_[i_++]);
            if (c == '"') return true;
            if (c == '\\') {
                if (i_ >= n_) return false;
                char e = p_[i_++];
                switch (e) {
                    case '"': out += L'"'; break;
                    case '\\': out += L'\\'; break;
                    case '/': out += L'/'; break;
                    case 'b': out += L'\b'; break;
                    case 'f': out += L'\f'; break;
                    case 'n': out += L'\n'; break;
                    case 'r': out += L'\r'; break;
                    case 't': out += L'\t'; break;
                    case 'u': {
                        unsigned cp = 0;
                        if (!Hex4(cp)) return false;
                        if (cp >= 0xD800 && cp <= 0xDBFF && i_ + 1 < n_ &&
                            p_[i_] == '\\' && p_[i_ + 1] == 'u') {
                            size_t save = i_;
                            i_ += 2;
                            unsigned lo = 0;
                            if (Hex4(lo) && lo >= 0xDC00 && lo <= 0xDFFF) {
                                cp = 0x10000 + ((cp - 0xD800) << 10) +
                                     (lo - 0xDC00);
                            } else {
                                i_ = save;
                            }
                        }
                        AppendCp(out, cp);
                        break;
                    }
                    default: return false;
                }
            } else if (c < 0x80) {
                out += static_cast<wchar_t>(c);
            } else {
                int extra = (c >= 0xF0) ? 3 : (c >= 0xE0) ? 2 : (c >= 0xC0) ? 1 : -1;
                if (extra < 0 || i_ + static_cast<size_t>(extra) > n_) return false;
                unsigned cp = c & (0x3Fu >> extra);
                for (int k = 0; k < extra; ++k) {
                    unsigned char cc = static_cast<unsigned char>(p_[i_++]);
                    if ((cc & 0xC0) != 0x80) return false;
                    cp = (cp << 6) | (cc & 0x3F);
                }
                AppendCp(out, cp);
            }
            if (out.size() > 65536) return false;
        }
        return false;
    }
    bool Arr(Value& v, int depth) {
        ++i_;
        v.type = Value::Type::Arr;
        SkipWs();
        if (i_ < n_ && p_[i_] == ']') {
            ++i_;
            return true;
        }
        for (;;) {
            SkipWs();
            v.items.emplace_back();
            if (!Val(v.items.back(), depth + 1)) return false;
            SkipWs();
            if (i_ < n_ && p_[i_] == ',') {
                ++i_;
                continue;
            }
            if (i_ < n_ && p_[i_] == ']') {
                ++i_;
                return true;
            }
            return false;
        }
    }
    bool Obj(Value& v, int depth) {
        ++i_;
        v.type = Value::Type::Obj;
        SkipWs();
        if (i_ < n_ && p_[i_] == '}') {
            ++i_;
            return true;
        }
        for (;;) {
            SkipWs();
            std::wstring key;
            if (!Str(key)) return false;
            SkipWs();
            if (i_ >= n_ || p_[i_] != ':') return false;
            ++i_;
            SkipWs();
            v.members.emplace_back(std::move(key), Value());
            if (!Val(v.members.back().second, depth + 1)) return false;
            SkipWs();
            if (i_ < n_ && p_[i_] == ',') {
                ++i_;
                continue;
            }
            if (i_ < n_ && p_[i_] == '}') {
                ++i_;
                return true;
            }
            return false;
        }
    }
    bool Val(Value& v, int depth) {
        if (depth > 64 || i_ >= n_) return false;
        switch (p_[i_]) {
            case '{': return Obj(v, depth);
            case '[': return Arr(v, depth);
            case '"': v.type = Value::Type::Str; return Str(v.str);
            case 't':
                if (i_ + 4 <= n_ && memcmp(p_ + i_, "true", 4) == 0) {
                    i_ += 4;
                    v.type = Value::Type::Bool;
                    return true;
                }
                return false;
            case 'f':
                if (i_ + 5 <= n_ && memcmp(p_ + i_, "false", 5) == 0) {
                    i_ += 5;
                    v.type = Value::Type::Bool;
                    return true;
                }
                return false;
            case 'n':
                if (i_ + 4 <= n_ && memcmp(p_ + i_, "null", 4) == 0) {
                    i_ += 4;
                    v.type = Value::Type::Null;
                    return true;
                }
                return false;
            default: {
                size_t start = i_;
                while (i_ < n_ &&
                       ((p_[i_] >= '0' && p_[i_] <= '9') || p_[i_] == '-' ||
                        p_[i_] == '+' || p_[i_] == '.' || p_[i_] == 'e' ||
                        p_[i_] == 'E')) {
                    ++i_;
                }
                v.type = Value::Type::Num;
                return i_ > start;
            }
        }
    }
    const char* p_;
    size_t n_;
    size_t i_ = 0;
};

}  // namespace JsonMini

struct BookmarkResultPayload {
    UINT gen = 0;
    std::vector<BookmarkEntry> entries;
};

void CollectChromiumBookmarks(const JsonMini::Value& node,
                              const std::wstring& folder,
                              const std::wstring& browser,
                              std::vector<BookmarkEntry>& out,
                              std::unordered_set<std::wstring>& seen,
                              int depth) {
    if (depth > 48 || out.size() >= 15000) return;
    std::wstring type = node.Str(L"type");
    if (type == L"url") {
        std::wstring url = node.Str(L"url");
        if (!StartsWithNoCase(url, L"http://") &&
            !StartsWithNoCase(url, L"https://")) {
            return;
        }
        if (!seen.insert(url).second) return;
        std::wstring name = node.Str(L"name");
        BookmarkEntry b;
        b.name = name.empty() ? url : name;
        b.folded = FoldString(b.name);
        b.url = url;
        b.folder = folder;
        b.browser = browser;
        std::wstring host = url;
        if (size_t s = host.find(L"://"); s != std::wstring::npos) {
            host = host.substr(s + 3);
        }
        if (size_t sl = host.find(L'/'); sl != std::wstring::npos) {
            host.resize(sl);
        }
        b.tail = FoldString(folder + L" " + host);
        out.push_back(std::move(b));
        return;
    }
    if (type == L"folder") {
        const auto* ch = node.Get(L"children");
        if (!ch || ch->type != JsonMini::Value::Type::Arr) return;
        std::wstring title = node.Str(L"name");
        std::wstring sub =
            title.empty() ? folder
                          : (folder.empty() ? title : folder + L" / " + title);
        for (const auto& item : ch->items) {
            CollectChromiumBookmarks(item, sub, browser, out, seen, depth + 1);
        }
    }
}

std::vector<std::pair<std::wstring, std::wstring>> FindBookmarkJsonFiles() {
    std::vector<std::pair<std::wstring, std::wstring>> out;
    std::wstring local = KnownFolderPath(FOLDERID_LocalAppData);
    std::wstring roam = KnownFolderPath(FOLDERID_RoamingAppData);
    struct Spec {
        PCWSTR name;
        PCWSTR rel;
        bool roaming;
    };
    const Spec specs[] = {
        {L"Chrome", L"Google\\Chrome\\User Data", false},
        {L"Chrome Beta", L"Google\\Chrome Beta\\User Data", false},
        {L"Edge", L"Microsoft\\Edge\\User Data", false},
        {L"Brave", L"BraveSoftware\\Brave-Browser\\User Data", false},
        {L"Vivaldi", L"Vivaldi\\User Data", false},
        {L"Chromium", L"Chromium\\User Data", false},
        {L"Opera", L"Opera Software\\Opera Stable", true},
        {L"Opera GX", L"Opera Software\\Opera GX Stable", true},
    };
    for (const auto& sp : specs) {
        const std::wstring& base = sp.roaming ? roam : local;
        if (base.empty()) continue;
        std::wstring root = JoinPath(base, sp.rel);
        if (GetFileAttributesW(root.c_str()) == INVALID_FILE_ATTRIBUTES) continue;
        std::wstring direct = JoinPath(root, L"Bookmarks");
        if (GetFileAttributesW(direct.c_str()) != INVALID_FILE_ATTRIBUTES) {
            out.push_back({direct, sp.name});
        }
        WIN32_FIND_DATAW fd{};
        HANDLE hf = FindFirstFileW(JoinPath(root, L"*").c_str(), &fd);
        if (hf != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                    fd.cFileName[0] == L'.') {
                    continue;
                }
                std::wstring bm =
                    JoinPath(JoinPath(root, fd.cFileName), L"Bookmarks");
                if (GetFileAttributesW(bm.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    out.push_back({bm, sp.name});
                }
            } while (FindNextFileW(hf, &fd));
            FindClose(hf);
        }
    }
    return out;
}

DWORD WINAPI BookmarkThreadProc(LPVOID param) {
    UINT gen = static_cast<UINT>(reinterpret_cast<uintptr_t>(param));
    auto files = FindBookmarkJsonFiles();
    ULONGLONG best = 0;
    for (const auto& [path, browser] : files) {
        (void)browser;
        WIN32_FILE_ATTRIBUTE_DATA d{};
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &d)) {
            ULONGLONG t =
                (static_cast<ULONGLONG>(d.ftLastWriteTime.dwHighDateTime)
                 << 32) |
                d.ftLastWriteTime.dwLowDateTime;
            if (t > best) best = t;
        }
    }
    if (best > 0 && best == g_bookmarkStamp.load(std::memory_order_acquire)) {
        return 0;
    }
    if (best > 0) {
        g_bookmarkStamp.store(best, std::memory_order_release);
    }

    std::vector<BookmarkEntry> all;
    std::unordered_set<std::wstring> seen;
    for (const auto& [path, browser] : files) {
        std::vector<BYTE> raw;
        if (!ReadFileBytesRaw(path, raw, 24 << 20)) continue;
        std::string txt(reinterpret_cast<const char*>(raw.data()), raw.size());
        JsonMini::Value doc;
        JsonMini::Parser parser(txt);
        if (!parser.Parse(doc)) continue;
        const auto* roots = doc.Get(L"roots");
        if (!roots) continue;
        for (const auto& root : roots->members) {
            const auto* ch = root.second.Get(L"children");
            if (!ch || ch->type != JsonMini::Value::Type::Arr) continue;
            for (const auto& child : ch->items) {
                CollectChromiumBookmarks(child, L"", browser, all, seen, 0);
            }
        }
    }
    auto* payload = new BookmarkResultPayload{gen, std::move(all)};
    if (!g_hostHwnd ||
        !PostMessageW(g_hostHwnd, kWmAppBookmarksDone, 0,
                      reinterpret_cast<LPARAM>(payload))) {
        delete payload;
    }
    return 0;
}

void RefreshBookmarksIfStale() {
    if (!g_settings.enableBookmarks || !g_hostHwnd) return;
    double now = NowSeconds();
    if (now - g_bookmarkCheckedAt < 30.0) return;
    g_bookmarkCheckedAt = now;
    UINT gen = ++g_bookmarkGen;
    HANDLE th = CreateThread(
        nullptr, 0, BookmarkThreadProc,
        reinterpret_cast<LPVOID>(static_cast<uintptr_t>(gen)), 0, nullptr);
    if (th) CloseHandle(th);
}

// ===== Recent Files (Windows FOLDERID_Recent) =====
struct RecentPayload {
    UINT gen = 0;
    ULONGLONG stamp = 0;
    std::vector<FileEntry> entries;
};

DWORD WINAPI RecentFilesThreadProc(LPVOID param) {
    UINT gen = static_cast<UINT>(reinterpret_cast<uintptr_t>(param));
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    std::wstring recDir = KnownFolderPath(FOLDERID_Recent);
    std::vector<FileEntry> out;
    ULONGLONG stamp = 0;
    if (!recDir.empty()) {
        struct Cand {
            std::wstring path;
            FILETIME ft{};
        };
        std::vector<Cand> cands;
        WIN32_FIND_DATAW fd{};
        HANDLE hf = FindFirstFileW(JoinPath(recDir, L"*.lnk").c_str(), &fd);
        if (hf != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                ULONGLONG t =
                    (static_cast<ULONGLONG>(fd.ftLastWriteTime.dwHighDateTime)
                     << 32) |
                    fd.ftLastWriteTime.dwLowDateTime;
                if (t > stamp) stamp = t;
                cands.push_back({JoinPath(recDir, fd.cFileName), fd.ftLastWriteTime});
            } while (FindNextFileW(hf, &fd));
            FindClose(hf);
        }
        std::sort(cands.begin(), cands.end(), [](const Cand& a, const Cand& b) {
            return CompareFileTime(&a.ft, &b.ft) > 0;
        });
        if (cands.size() > 50) cands.resize(50);
        std::unordered_set<std::wstring> seen;
        for (const auto& c : cands) {
            LnkMeta lm = ReadLnkMeta(c.path);
            if (!lm.ok || !lm.args.empty()) continue;
            WIN32_FILE_ATTRIBUTE_DATA fa{};
            if (!GetFileAttributesExW(lm.target.c_str(), GetFileExInfoStandard,
                                      &fa)) {
                continue;
            }
            if (!seen.insert(NormalizePath(lm.target)).second) continue;
            FileEntry fe;
            fe.name = FileNameOf(lm.target);
            fe.path = lm.target;
            fe.dir = ParentDir(lm.target);
            fe.folded = FoldString(fe.name + L" " + fe.dir);
            fe.isDir = (fa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            fe.attr = fa.dwFileAttributes;
            fe.ext = fe.isDir ? std::wstring() : FileExtOf(fe.name);
            fe.size = (static_cast<uint64_t>(fa.nFileSizeHigh) << 32) |
                      fa.nFileSizeLow;
            fe.writeTime = fa.ftLastWriteTime;
            out.push_back(std::move(fe));
            if (out.size() >= 36) break;
        }
    }
    CoUninitialize();
    auto* payload = new RecentPayload{gen, stamp, std::move(out)};
    if (!g_hostHwnd ||
        !PostMessageW(g_hostHwnd, kWmAppRecentDone, 0,
                      reinterpret_cast<LPARAM>(payload))) {
        delete payload;
    }
    return 0;
}

void RefreshRecentIfStale() {
    if (!g_hostHwnd) return;
    double now = NowSeconds();
    if (now - g_recentCheckedAt < 15.0) return;
    g_recentCheckedAt = now;
    UINT gen = ++g_recentGen;
    HANDLE th = CreateThread(
        nullptr, 0, RecentFilesThreadProc,
        reinterpret_cast<LPVOID>(static_cast<uintptr_t>(gen)), 0, nullptr);
    if (th) CloseHandle(th);
}

// ===== Everything IPC + Native Filesystem Fallback Search =====
constexpr WCHAR kEverythingWndClass[] = L"EVERYTHING_TASKBAR_NOTIFICATION";
constexpr DWORD kEverythingCopyDataQuery2W = 18;
constexpr DWORD kEvReqName = 0x00000001;
constexpr DWORD kEvReqPath = 0x00000002;
constexpr DWORD kEvReqSize = 0x00000010;
constexpr DWORD kEvReqDateMod = 0x00000040;
constexpr DWORD kEvItemFolder = 0x00000001;
constexpr DWORD kEvItemDrive = 0x00000002;
constexpr DWORD kEvMaxResults = 250;

#pragma pack(push, 1)
struct EvQuery2W {
    DWORD replyHwnd;
    DWORD replyMessage;
    DWORD searchFlags;
    DWORD offset;
    DWORD maxResults;
    DWORD requestFlags;
    DWORD sortType;
    wchar_t search[1];
};
struct EvItem2W {
    DWORD flags;
    DWORD dataOffset;
};
struct EvList2W {
    DWORD totItems, numItems, offset, requestFlags, sortType;
};
#pragma pack(pop)

HWND g_evHwnd = nullptr;
DWORD g_evSeq = 0;
DWORD g_evPendingSeq = 0;
bool g_evPending = false;
std::wstring g_evPendingKey;
std::vector<FileEntry> g_searchedFiles;
std::wstring g_searchedFilesKey;
bool g_fileSearchActive = false;
std::atomic<LONG> g_localSearchGen{0};

bool IsSystemNoisePath(const std::wstring& path) {
    std::wstring low = ToLower(path);
    return low.find(L":\\windows\\") != std::wstring::npos ||
           low.find(L"\\$recycle.bin") != std::wstring::npos ||
           low.find(L"\\system volume information") != std::wstring::npos ||
           low.find(L"\\appdata\\local\\temp\\") != std::wstring::npos ||
           low.find(L"\\winsxs\\") != std::wstring::npos;
}

int UserPathPriority(const std::wstring& path) {
    std::wstring low = ToLower(path);
    if (low.find(L"\\desktop\\") != std::wstring::npos ||
        low.find(L"\\documents\\") != std::wstring::npos ||
        low.find(L"\\downloads\\") != std::wstring::npos ||
        low.find(L"\\my drive\\") != std::wstring::npos) {
        return 120;
    }
    if (low.find(L"\\users\\") != std::wstring::npos) return 80;
    if (IsSystemNoisePath(path)) return -140;
    return 0;
}

std::wstring BuildEverythingSyntax(const std::wstring& raw,
                                   FileSubTab subTab) {
    std::wstring s = TrimBlanks(raw);
    std::wstring out;
    if (subTab == FileSubTab::Folders) out += L"folder: ";
    else if (subTab == FileSubTab::Images) {
        out += L"ext:png;jpg;jpeg;webp;gif;bmp;ico;heic;svg ";
    } else if (subTab == FileSubTab::Documents) {
        out += L"ext:pdf;doc;docx;txt;md;xls;xlsx;csv;ppt;pptx ";
    } else if (subTab == FileSubTab::Code) {
        out += L"ext:c;cpp;h;hpp;cs;py;js;ts;json;html;css;rs;go;ps1 ";
    } else if (subTab == FileSubTab::Archives) {
        out += L"ext:zip;rar;7z;tar;gz ";
    }

    size_t i = 0;
    while (i < s.size()) {
        size_t a = s.find_first_not_of(L" \t", i);
        if (a == std::wstring::npos) break;
        size_t b = s.find_first_of(L" \t", a);
        if (b == std::wstring::npos) b = s.size();
        std::wstring tok = s.substr(a, b - a);
        std::wstring low = ToLower(tok);
        if (low == L"dir:" || low == L"folder:") {
            out += L"folder: ";
        } else if (low == L"file:" || low == L"files:") {
            out += L"file: ";
        } else if (low.compare(0, 5, L"type:") == 0) {
            std::wstring v = low.substr(5);
            if (v == L"image" || v == L"images" || v == L"img") {
                out += L"ext:png;jpg;jpeg;webp;gif;bmp;ico;heic;svg ";
            } else if (v == L"doc" || v == L"docs" || v == L"documents") {
                out += L"ext:pdf;doc;docx;txt;md;xls;xlsx;ppt;pptx ";
            } else if (v == L"code") {
                out += L"ext:c;cpp;h;py;js;ts;json;html;css;rs;go;ps1 ";
            } else if (v == L"video") {
                out += L"ext:mp4;mkv;mov;avi;webm ";
            } else if (v == L"audio") {
                out += L"ext:mp3;wav;flac;m4a;ogg ";
            } else {
                out += tok + L" ";
            }
        } else {
            out += tok + L" ";
        }
        i = b + 1;
    }
    return TrimBlanks(out);
}

bool TrySendEverythingQuery(const std::wstring& query, DWORD seq) {
    if (!g_settings.enableEverything || !g_hostHwnd || query.empty()) {
        return false;
    }
    if (!g_evHwnd || !IsWindow(g_evHwnd)) {
        g_evHwnd = FindWindowW(kEverythingWndClass, nullptr);
    }
    if (!g_evHwnd) return false;

    size_t chars = query.size() + 1;
    std::vector<BYTE> packet(
        sizeof(EvQuery2W) - sizeof(wchar_t) + chars * sizeof(wchar_t), 0);
    auto* q = reinterpret_cast<EvQuery2W*>(packet.data());
    q->replyHwnd = static_cast<DWORD>(reinterpret_cast<uintptr_t>(g_hostHwnd));
    q->replyMessage = seq;
    q->searchFlags = 0;
    q->offset = 0;
    q->maxResults = kEvMaxResults;
    q->requestFlags = kEvReqName | kEvReqPath | kEvReqSize | kEvReqDateMod;
    q->sortType = 14;  // EVERYTHING_SORT_DATE_MODIFIED_DESCENDING
    memcpy(q->search, query.c_str(), chars * sizeof(wchar_t));

    COPYDATASTRUCT cds{};
    cds.dwData = kEverythingCopyDataQuery2W;
    cds.cbData = static_cast<DWORD>(packet.size());
    cds.lpData = packet.data();
    DWORD_PTR ignored = 0;
    LRESULT ok = SendMessageTimeoutW(g_evHwnd, WM_COPYDATA,
                                     reinterpret_cast<WPARAM>(g_hostHwnd),
                                     reinterpret_cast<LPARAM>(&cds),
                                     SMTO_ABORTIFHUNG, 60, &ignored);
    if (!ok) {
        g_evHwnd = nullptr;
        return false;
    }
    return true;
}

bool EvReadItemFields(const std::vector<BYTE>& buf,
                      size_t pos,
                      DWORD flags,
                      std::wstring& name,
                      std::wstring& path,
                      uint64_t& size,
                      FILETIME& modified,
                      bool& noInfo) {
    auto readStr = [&](std::wstring* dst) -> bool {
        if (pos > buf.size() || buf.size() - pos < 4) return false;
        DWORD len = 0;
        memcpy(&len, buf.data() + pos, 4);
        pos += 4;
        if (len > 4096 || pos > buf.size() ||
            buf.size() - pos < (static_cast<size_t>(len) + 1) * sizeof(wchar_t)) {
            return false;
        }
        if (dst) {
            dst->resize(len);
            if (len) {
                memcpy(dst->data(), buf.data() + pos,
                       static_cast<size_t>(len) * sizeof(wchar_t));
            }
        }
        pos += (static_cast<size_t>(len) + 1) * sizeof(wchar_t);
        return true;
    };
    auto take = [&](size_t n, void* dst) -> bool {
        if (pos > buf.size() || buf.size() - pos < n) return false;
        if (dst) memcpy(dst, buf.data() + pos, n);
        pos += n;
        return true;
    };
    bool hasSz = false, hasMod = false;
    if ((flags & 0x00000001u) && !readStr(&name)) return false;
    if ((flags & 0x00000002u) && !readStr(&path)) return false;
    if ((flags & 0x00000004u) && !readStr(nullptr)) return false;
    if ((flags & 0x00000008u) && !readStr(nullptr)) return false;
    if (flags & 0x00000010u) {
        long long s = 0;
        if (!take(8, &s)) return false;
        size = s > 0 ? static_cast<uint64_t>(s) : 0;
        hasSz = true;
    }
    if ((flags & 0x00000020u) && !take(8, nullptr)) return false;
    if (flags & 0x00000040u) {
        if (!take(8, &modified)) return false;
        hasMod = true;
    }
    noInfo = !(hasSz && hasMod);
    return true;
}

std::vector<FileEntry> ParseEverythingCopyData(const COPYDATASTRUCT* cds) {
    std::vector<FileEntry> out;
    if (!cds || !cds->lpData || cds->cbData < sizeof(EvList2W)) return out;
    std::vector<BYTE> buf(
        static_cast<const BYTE*>(cds->lpData),
        static_cast<const BYTE*>(cds->lpData) + cds->cbData);
    EvList2W hdr{};
    memcpy(&hdr, buf.data(), sizeof(hdr));
    size_t fitting = (buf.size() - sizeof(EvList2W)) / sizeof(EvItem2W);
    DWORD n = static_cast<DWORD>(
        std::min<size_t>(std::min<DWORD>(hdr.numItems, kEvMaxResults), fitting));
    out.reserve(n);
    for (DWORD i = 0; i < n; ++i) {
        EvItem2W it{};
        memcpy(&it, buf.data() + sizeof(EvList2W) + i * sizeof(EvItem2W),
               sizeof(it));
        if (it.flags & kEvItemDrive) continue;
        FileEntry e;
        if (!EvReadItemFields(buf, it.dataOffset, hdr.requestFlags, e.name, e.dir,
                              e.size, e.writeTime, e.noInfo)) {
            continue;
        }
        if (e.name.empty()) continue;
        e.isDir = (it.flags & kEvItemFolder) != 0;
        e.path = e.dir.empty() ? e.name : JoinPath(e.dir, e.name);
        if (IsSystemNoisePath(e.path)) continue;
        e.folded = FoldString(e.name);
        if (!e.isDir) e.ext = FileExtOf(e.name);
        out.push_back(std::move(e));
    }
    return out;
}

struct LocalFileJob {
    LONG gen = 0;
    std::wstring key;
    std::wstring rawQuery;
    FileSubTab subTab = FileSubTab::All;
};

struct LocalFilePayload {
    LONG gen = 0;
    std::wstring key;
    std::vector<FileEntry> entries;
};

bool SubTabMatchesFile(FileSubTab tab, bool isDir, const std::wstring& ext) {
    switch (tab) {
        case FileSubTab::All:
            return true;
        case FileSubTab::Folders:
            return isDir;
        case FileSubTab::Images:
            return !isDir &&
                   (ext == L"PNG" || ext == L"JPG" || ext == L"JPEG" ||
                    ext == L"WEBP" || ext == L"GIF" || ext == L"BMP" ||
                    ext == L"ICO" || ext == L"HEIC" || ext == L"SVG");
        case FileSubTab::Documents:
            return !isDir &&
                   (ext == L"PDF" || ext == L"DOC" || ext == L"DOCX" ||
                    ext == L"TXT" || ext == L"MD" || ext == L"XLS" ||
                    ext == L"XLSX" || ext == L"CSV" || ext == L"PPT" ||
                    ext == L"PPTX");
        case FileSubTab::Code:
            return !isDir &&
                   (ext == L"C" || ext == L"CPP" || ext == L"H" ||
                    ext == L"HPP" || ext == L"CS" || ext == L"PY" ||
                    ext == L"JS" || ext == L"TS" || ext == L"JSON" ||
                    ext == L"HTML" || ext == L"CSS" || ext == L"RS" ||
                    ext == L"GO" || ext == L"PS1");
        case FileSubTab::Archives:
            return !isDir &&
                   (ext == L"ZIP" || ext == L"RAR" || ext == L"7Z" ||
                    ext == L"TAR" || ext == L"GZ");
    }
    return true;
}

DWORD WINAPI LocalFileSearchProc(LPVOID param) {
    std::unique_ptr<LocalFileJob> job(static_cast<LocalFileJob*>(param));
    std::vector<std::wstring> tokens;
    {
        std::wstring low = FoldString(job->rawQuery);
        size_t i = 0;
        while (i < low.size()) {
            size_t a = low.find_first_not_of(L" \t", i);
            if (a == std::wstring::npos) break;
            size_t b = low.find_first_of(L" \t", a);
            if (b == std::wstring::npos) b = low.size();
            tokens.push_back(low.substr(a, b - a));
            i = b + 1;
        }
    }
    std::vector<std::wstring> roots;
    if (auto p = KnownFolderPath(FOLDERID_Desktop); !p.empty()) roots.push_back(p);
    if (auto p = KnownFolderPath(FOLDERID_Downloads); !p.empty()) roots.push_back(p);
    if (auto p = KnownFolderPath(FOLDERID_Documents); !p.empty()) roots.push_back(p);
    if (auto p = KnownFolderPath(FOLDERID_Profile); !p.empty()) roots.push_back(p);

    struct Node {
        std::wstring dir;
        int depth = 0;
    };
    std::deque<Node> q;
    for (const auto& r : roots) q.push_back({r, 0});

    std::vector<FileEntry> out;
    std::unordered_set<std::wstring> seen;
    DWORD t0 = GetTickCount();
    size_t scanned = 0;
    while (!q.empty() && out.size() < 80 && scanned < 15000 &&
           GetTickCount() - t0 < 600) {
        if (job->gen != g_localSearchGen.load(std::memory_order_acquire)) {
            return 0;
        }
        Node cur = std::move(q.front());
        q.pop_front();
        WIN32_FIND_DATAW fd{};
        HANDLE hf = FindFirstFileExW(JoinPath(cur.dir, L"*").c_str(),
                                     FindExInfoBasic, &fd,
                                     FindExSearchNameMatch, nullptr,
                                     FIND_FIRST_EX_LARGE_FETCH);
        if (hf == INVALID_HANDLE_VALUE) continue;
        do {
            if (fd.cFileName[0] == L'.') continue;
            if (fd.dwFileAttributes & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) {
                continue;
            }
            ++scanned;
            bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            std::wstring full = JoinPath(cur.dir, fd.cFileName);
            std::wstring norm = NormalizePath(full);
            if (!seen.insert(norm).second) continue;

            std::wstring folded = FoldString(fd.cFileName);
            std::wstring ext = isDir ? std::wstring() : FileExtOf(fd.cFileName);
            if (SubTabMatchesFile(job->subTab, isDir, ext)) {
                bool match = true;
                for (const auto& tk : tokens) {
                    if (folded.find(tk) == std::wstring::npos) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    FileEntry fe;
                    fe.name = fd.cFileName;
                    fe.path = full;
                    fe.dir = cur.dir;
                    fe.folded = folded;
                    fe.isDir = isDir;
                    fe.attr = fd.dwFileAttributes;
                    fe.ext = ext;
                    fe.size = (static_cast<uint64_t>(fd.nFileSizeHigh) << 32) |
                              fd.nFileSizeLow;
                    fe.writeTime = fd.ftLastWriteTime;
                    out.push_back(std::move(fe));
                }
            }
            if (isDir && cur.depth < 4 &&
                !(fd.dwFileAttributes &
                  (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_OFFLINE |
                   FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS))) {
                if (folded != L"appdata" && folded != L"node_modules" &&
                    folded != L".git") {
                    q.push_back({full, cur.depth + 1});
                }
            }
        } while (FindNextFileW(hf, &fd) && out.size() < 120);
        FindClose(hf);
    }
    auto* payload =
        new LocalFilePayload{job->gen, job->key, std::move(out)};
    if (!g_hostHwnd ||
        !PostMessageW(g_hostHwnd, kWmAppLocalFilesDone, 0,
                      reinterpret_cast<LPARAM>(payload))) {
        delete payload;
    }
    return 0;
}

HWND GetSpotlightUiHwnd();

void CancelPendingFileSearch() {
    g_evPending = false;
    g_fileSearchActive = false;
    g_localSearchGen.fetch_add(1, std::memory_order_acq_rel);
    if (g_hostHwnd && IsWindow(g_hostHwnd)) {
        KillTimer(g_hostHwnd, kTimerEvFallback);
    }
    if (HWND ui = GetSpotlightUiHwnd(); ui && IsWindow(ui)) {
        KillTimer(ui, kTimerSearchDebounce);
    }
}

void StartLocalFileSearchFallback(const std::wstring& query,
                                  const std::wstring& key,
                                  FileSubTab subTab) {
    LONG gen = g_localSearchGen.fetch_add(1, std::memory_order_acq_rel) + 1;
    g_fileSearchActive = true;
    auto* job = new LocalFileJob{gen, key, query, subTab};
    HANDLE th =
        CreateThread(nullptr, 0, LocalFileSearchProc, job, 0, nullptr);
    if (th) CloseHandle(th);
    else {
        g_fileSearchActive = false;
        delete job;
    }
}

void StartAsyncFileSearch(const std::wstring& query,
                          SpotlightMode mode,
                          FileSubTab subTab) {
    if ((query.empty() && mode != SpotlightMode::Files) ||
        (mode != SpotlightMode::Files && query.size() < 2)) {
        CancelPendingFileSearch();
        g_searchedFiles.clear();
        g_searchedFilesKey.clear();
        return;
    }
    std::wstring key =
        std::to_wstring(static_cast<int>(mode)) + L":" +
        std::to_wstring(static_cast<int>(subTab)) + L":" + query;
    if (key == g_searchedFilesKey && !g_fileSearchActive) return;

    std::wstring evQuery = BuildEverythingSyntax(
        query.empty() ? L"*" : query,
        mode == SpotlightMode::Files ? subTab : FileSubTab::All);

    DWORD seq = ++g_evSeq;
    if (!evQuery.empty() && TrySendEverythingQuery(evQuery, seq)) {
        g_evPending = true;
        g_evPendingSeq = seq;
        g_evPendingKey = key;
        g_fileSearchActive = true;
        if (g_hostHwnd) SetTimer(g_hostHwnd, kTimerEvFallback, 500, nullptr);
        return;
    }
    // Fallback to bounded native BFS
    StartLocalFileSearchFallback(query, key, subTab);
}

// ===== Clipboard History (Event-Driven + DPAPI Encrypted) =====
constexpr size_t kMaxClipItems = 40;
constexpr size_t kMaxClipChars = 8192;
constexpr int kMaxClipImageSide = 2560;

std::wstring ClipKey(const std::wstring& suffix) {
    return L"spotclip." + CurrentUserSid() + L"." + suffix;
}

void DeleteClipImageFile(const std::wstring& stored) {
    std::wstring p = ClipImagePath(stored);
    if (!p.empty()) DeleteFileW(p.c_str());
}

bool PurgeExpiredClips() {
    if (g_settings.clipRetentionDays <= 0 || g_clips.empty()) return false;
    int cutoff = NowStamp() - g_settings.clipRetentionDays * 86400;
    bool removed = false;
    for (size_t i = g_clips.size(); i-- > 0;) {
        if (g_clips[i].pinned || g_clips[i].stamp <= 0 ||
            g_clips[i].stamp > cutoff) {
            continue;
        }
        if (g_clips[i].image) DeleteClipImageFile(g_clips[i].text);
        g_clips.erase(g_clips.begin() + static_cast<std::ptrdiff_t>(i));
        removed = true;
    }
    return removed;
}

int g_savedClipCount = 0;
bool g_clipDirty = false;

void SaveClipHistory() {
    int limit = std::clamp(
        std::max(static_cast<int>(g_clips.size()), g_savedClipCount), 0,
        static_cast<int>(kMaxClipItems));
    for (int i = 0; i < limit; ++i) {
        bool has = i < static_cast<int>(g_clips.size());
        const ClipEntry* c = has ? &g_clips[static_cast<size_t>(i)] : nullptr;
        std::wstring idx = std::to_wstring(i);
        std::wstring slot = c ? (c->image ? c->text : c->protectedText) : L"";
        Wh_SetStringValue(ClipKey(L"i" + idx).c_str(), slot.c_str());
        Wh_SetIntValue(ClipKey(L"p" + idx).c_str(), (c && c->pinned) ? 1 : 0);
        Wh_SetIntValue(ClipKey(L"k" + idx).c_str(), (c && c->image) ? 1 : 0);
        Wh_SetIntValue(ClipKey(L"t" + idx).c_str(),
                       (c && !slot.empty()) ? c->stamp : 0);
    }
    g_savedClipCount = static_cast<int>(g_clips.size());
    Wh_SetIntValue(ClipKey(L"count").c_str(), g_savedClipCount);
    g_clipDirty = false;
    if (g_hostHwnd && IsWindow(g_hostHwnd)) {
        KillTimer(g_hostHwnd, kTimerClipSave);
    }
}

void ScheduleClipHistorySave() {
    g_clipDirty = true;
    if (g_hostHwnd && IsWindow(g_hostHwnd)) {
        SetTimer(g_hostHwnd, kTimerClipSave, 1500, nullptr);
    } else {
        SaveClipHistory();
    }
}

void LoadClipHistory() {
    g_clips.clear();
    int n = std::clamp(Wh_GetIntValue(ClipKey(L"count").c_str(), 0), 0,
                       static_cast<int>(kMaxClipItems));
    std::vector<wchar_t> buf(kMaxClipChars * 3 + 256, L'\0');
    for (int i = 0; i < n; ++i) {
        std::wstring idx = std::to_wstring(i);
        size_t len = Wh_GetStringValue(ClipKey(L"i" + idx).c_str(), buf.data(),
                                       buf.size());
        if (len == 0) continue;
        std::wstring slot(buf.data(), len);
        ClipEntry e;
        e.image = Wh_GetIntValue(ClipKey(L"k" + idx).c_str(), 0) != 0;
        e.pinned = Wh_GetIntValue(ClipKey(L"p" + idx).c_str(), 0) != 0;
        e.stamp = Wh_GetIntValue(ClipKey(L"t" + idx).c_str(), 0);
        if (e.image) {
            e.text = slot;
            size_t bar = e.text.find(L'|');
            if (bar != std::wstring::npos) {
                std::wstring dims = e.text.substr(bar + 1);
                if (size_t x = dims.find(L'x'); x != std::wstring::npos) {
                    e.w = _wtoi(dims.c_str());
                    e.h = _wtoi(dims.c_str() + x + 1);
                }
            }
        } else if (UnprotectText(slot, e.text)) {
            e.protectedText = slot;
        } else {
            continue;
        }
        g_clips.push_back(std::move(e));
    }
    g_savedClipCount = std::max(n, static_cast<int>(g_clips.size()));
    if (PurgeExpiredClips()) SaveClipHistory();
    g_clipDirty = false;
}

void ClearUnpinnedClipHistory() {
    for (size_t i = g_clips.size(); i-- > 0;) {
        if (!g_clips[i].pinned) {
            if (g_clips[i].image) DeleteClipImageFile(g_clips[i].text);
            g_clips.erase(g_clips.begin() + static_cast<std::ptrdiff_t>(i));
        }
    }
    SaveClipHistory();
}

bool ClipboardOptedOut() {
    static const UINT kExcl = RegisterClipboardFormatW(
        L"ExcludeClipboardContentFromMonitorProcessing");
    static const UINT kIgn =
        RegisterClipboardFormatW(L"Clipboard Viewer Ignore");
    static const UINT kHist =
        RegisterClipboardFormatW(L"CanIncludeInClipboardHistory");
    if ((kExcl && IsClipboardFormatAvailable(kExcl)) ||
        (kIgn && IsClipboardFormatAvailable(kIgn))) {
        return true;
    }
    if (!kHist || !IsClipboardFormatAvailable(kHist)) return false;
    if (!OpenClipboard(g_hostHwnd)) return true;
    bool out = true;
    if (HANDLE h = GetClipboardData(kHist)) {
        if (const DWORD* v = static_cast<const DWORD*>(GlobalLock(h))) {
            out = (*v == 0);
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return out;
}

void AddClipEntry(std::wstring payload, bool image, int w, int h) {
    bool wasPinned = false;
    for (size_t i = 0; i < g_clips.size(); ++i) {
        if (g_clips[i].text == payload && g_clips[i].image == image) {
            wasPinned = g_clips[i].pinned;
            g_clips.erase(g_clips.begin() + static_cast<std::ptrdiff_t>(i));
            break;
        }
    }
    ClipEntry e;
    e.text = std::move(payload);
    e.image = image;
    e.w = w;
    e.h = h;
    e.pinned = wasPinned;
    e.stamp = NowStamp();
    if (!image) {
        e.protectedText = ProtectText(e.text);
        if (e.protectedText.empty()) return;
    }
    g_clips.insert(g_clips.begin(), std::move(e));
    while (g_clips.size() > kMaxClipItems) {
        size_t victim = g_clips.size() - 1;
        for (size_t i = g_clips.size(); i-- > 1;) {
            if (!g_clips[i].pinned) {
                victim = i;
                break;
            }
        }
        if (g_clips[victim].image) DeleteClipImageFile(g_clips[victim].text);
        g_clips.erase(g_clips.begin() + static_cast<std::ptrdiff_t>(victim));
    }
    PurgeExpiredClips();
    ScheduleClipHistorySave();
}

void CaptureClipboardEvent() {
    if (!g_settings.enableClipboardHistory || !g_hostHwnd) return;
    if (ClipboardOptedOut()) return;

    if (!OpenClipboard(g_hostHwnd)) return;
    std::wstring textCap;
    if (HDROP drop = static_cast<HDROP>(GetClipboardData(CF_HDROP))) {
        UINT len = DragQueryFileW(drop, 0, nullptr, 0);
        if (len > 0 && len < kMaxClipChars) {
            textCap.resize(len + 1);
            if (DragQueryFileW(drop, 0, textCap.data(), len + 1) == len) {
                textCap.resize(len);
            } else {
                textCap.clear();
            }
        }
    }
    if (textCap.empty()) {
        if (HANDLE h = GetClipboardData(CF_UNICODETEXT)) {
            if (const wchar_t* s = static_cast<const wchar_t*>(GlobalLock(h))) {
                textCap = s;
                GlobalUnlock(h);
            }
        }
    }
    std::vector<uint32_t> imgPx;
    int imgW = 0, imgH = 0;
    if (textCap.empty()) {
        HANDLE dib = GetClipboardData(CF_DIBV5);
        if (!dib) dib = GetClipboardData(CF_DIB);
        if (dib) {
            const BYTE* base = static_cast<const BYTE*>(GlobalLock(dib));
            size_t avail = GlobalSize(dib);
            if (base && avail >= sizeof(BITMAPINFOHEADER)) {
                const auto* bih = reinterpret_cast<const BITMAPINFOHEADER*>(base);
                int sw = std::abs(bih->biWidth);
                int sh = std::abs(bih->biHeight);
                if (bih->biSize >= sizeof(BITMAPINFOHEADER) && sw > 0 && sh > 0 &&
                    static_cast<uint64_t>(sw) * sh <= 48ULL * 1000 * 1000) {
                    size_t off = bih->biSize;
                    if (bih->biBitCount <= 8) {
                        off += (static_cast<size_t>(1) << bih->biBitCount) *
                               sizeof(RGBQUAD);
                    } else if (bih->biSize == sizeof(BITMAPINFOHEADER) &&
                               (bih->biCompression == 3 ||
                                bih->biCompression == 6)) {
                        off += 3 * sizeof(DWORD);
                    }
                    int tw = sw, th = sh;
                    if (sw > kMaxClipImageSide || sh > kMaxClipImageSide) {
                        if (sw >= sh) {
                            tw = kMaxClipImageSide;
                            th = std::max(1, sh * kMaxClipImageSide / sw);
                        } else {
                            th = kMaxClipImageSide;
                            tw = std::max(1, sw * kMaxClipImageSide / sh);
                        }
                    }
                    if (off < avail) {
                        BITMAPINFO dst{};
                        dst.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        dst.bmiHeader.biWidth = tw;
                        dst.bmiHeader.biHeight = -th;
                        dst.bmiHeader.biPlanes = 1;
                        dst.bmiHeader.biBitCount = 32;
                        dst.bmiHeader.biCompression = BI_RGB;
                        HDC scr = GetDC(nullptr);
                        void* bits = nullptr;
                        HBITMAP hbmp =
                            CreateDIBSection(scr, &dst, DIB_RGB_COLORS, &bits,
                                             nullptr, 0);
                        HDC mem = hbmp ? CreateCompatibleDC(scr) : nullptr;
                        if (mem && bits) {
                            HGDIOBJ old = SelectObject(mem, hbmp);
                            SetStretchBltMode(mem, HALFTONE);
                            if (StretchDIBits(mem, 0, 0, tw, th, 0, 0, sw, sh,
                                              base + off,
                                              reinterpret_cast<const BITMAPINFO*>(
                                                  bih),
                                              DIB_RGB_COLORS, SRCCOPY) > 0) {
                                imgPx.resize(static_cast<size_t>(tw) * th);
                                memcpy(imgPx.data(), bits, imgPx.size() * 4);
                                for (uint32_t& px : imgPx) px |= 0xFF000000u;
                                imgW = tw;
                                imgH = th;
                            }
                            SelectObject(mem, old);
                        }
                        if (mem) DeleteDC(mem);
                        if (hbmp) DeleteObject(hbmp);
                        if (scr) ReleaseDC(nullptr, scr);
                    }
                }
            }
            if (base) GlobalUnlock(dib);
        }
    }
    CloseClipboard();

    if (!textCap.empty() && textCap.size() <= kMaxClipChars &&
        !TrimBlanks(textCap).empty()) {
        AddClipEntry(std::move(textCap), false, 0, 0);
        return;
    }
    if (!imgPx.empty() && imgW > 0 && imgH > 0 && g_gfx.wicFactory) {
        std::wstring dir = ClipImageDir();
        if (dir.empty()) return;
        SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
        uint64_t hash = 14695981039346656037ULL;
        for (uint32_t px : imgPx) hash = (hash ^ px) * 1099511628211ULL;
        wchar_t fname[64]{};
        swprintf_s(fname, L"%016llx.dat",
                   static_cast<unsigned long long>(hash));
        std::wstring fpath = JoinPath(dir, fname);
        if (GetFileAttributesW(fpath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            winrt::com_ptr<IWICBitmap> wb;
            winrt::com_ptr<IStream> strm;
            winrt::com_ptr<IWICBitmapEncoder> enc;
            winrt::com_ptr<IWICBitmapFrameEncode> frm;
            winrt::com_ptr<IPropertyBag2> props;
            WICPixelFormatGUID pfmt = GUID_WICPixelFormat32bppBGRA;
            UINT stride = static_cast<UINT>(imgW) * 4u;
            UINT bytes = stride * static_cast<UINT>(imgH);
            bool encOk = false;
            if (SUCCEEDED(CreateStreamOnHGlobal(nullptr, TRUE, strm.put())) &&
                SUCCEEDED(g_gfx.wicFactory->CreateBitmapFromMemory(
                    static_cast<UINT>(imgW), static_cast<UINT>(imgH), pfmt,
                    stride, bytes, reinterpret_cast<BYTE*>(imgPx.data()),
                    wb.put())) &&
                SUCCEEDED(g_gfx.wicFactory->CreateEncoder(
                    GUID_ContainerFormatPng, nullptr, enc.put())) &&
                SUCCEEDED(enc->Initialize(strm.get(), WICBitmapEncoderNoCache)) &&
                SUCCEEDED(enc->CreateNewFrame(frm.put(), props.put())) &&
                SUCCEEDED(frm->Initialize(props.get())) &&
                SUCCEEDED(frm->SetSize(static_cast<UINT>(imgW),
                                       static_cast<UINT>(imgH))) &&
                SUCCEEDED(frm->SetPixelFormat(&pfmt)) &&
                SUCCEEDED(frm->WritePixels(
                    static_cast<UINT>(imgH), stride, bytes,
                    reinterpret_cast<BYTE*>(imgPx.data()))) &&
                SUCCEEDED(frm->Commit()) && SUCCEEDED(enc->Commit())) {
                STATSTG stat{};
                HGLOBAL hg = nullptr;
                if (SUCCEEDED(strm->Stat(&stat, STATFLAG_NONAME)) &&
                    stat.cbSize.QuadPart > 0 &&
                    SUCCEEDED(GetHGlobalFromStream(strm.get(), &hg))) {
                    if (const void* p = GlobalLock(hg)) {
                        std::vector<BYTE> png(
                            static_cast<const BYTE*>(p),
                            static_cast<const BYTE*>(p) +
                                static_cast<size_t>(stat.cbSize.QuadPart));
                        GlobalUnlock(hg);
                        std::vector<BYTE> cipher;
                        encOk = ProtectBytes(png, cipher) &&
                                WriteFileBytesRaw(fpath, cipher);
                    }
                }
            }
            if (!encOk) {
                DeleteFileW(fpath.c_str());
                return;
            }
        }
        AddClipEntry(std::wstring(fname) + L"|" + std::to_wstring(imgW) +
                         L"x" + std::to_wstring(imgH),
                     true, imgW, imgH);
    }
}

void CopyTextToClipboard(const std::wstring& text) {
    if (text.empty() || !g_hostHwnd) return;
    if (!OpenClipboard(g_hostHwnd)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (mem) {
        if (void* dst = GlobalLock(mem)) {
            memcpy(dst, text.c_str(), bytes);
            GlobalUnlock(mem);
        }
        if (!SetClipboardData(CF_UNICODETEXT, mem)) GlobalFree(mem);
    }
    CloseClipboard();
}

bool PutClipImageOnClipboard(const std::wstring& stored) {
    std::wstring path = ClipImagePath(stored);
    if (path.empty() || !g_gfx.wicFactory || !g_hostHwnd) return false;
    auto src = DecodeFileImageWic(g_gfx.wicFactory.get(), path, 0);
    if (!src) return false;
    UINT w = 0, h = 0;
    if (FAILED(src->GetSize(&w, &h)) || w == 0 || h == 0) return false;
    winrt::com_ptr<IWICFormatConverter> conv;
    if (FAILED(g_gfx.wicFactory->CreateFormatConverter(conv.put())) ||
        FAILED(conv->Initialize(src.get(), GUID_WICPixelFormat32bppBGRA,
                                WICBitmapDitherTypeNone, nullptr, 0.0,
                                WICBitmapPaletteTypeCustom))) {
        return false;
    }
    size_t pixBytes = static_cast<size_t>(w) * h * 4;
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + pixBytes);
    if (!mem) return false;
    auto* bih = static_cast<BITMAPINFOHEADER*>(GlobalLock(mem));
    if (!bih) {
        GlobalFree(mem);
        return false;
    }
    ZeroMemory(bih, sizeof(BITMAPINFOHEADER));
    bih->biSize = sizeof(BITMAPINFOHEADER);
    bih->biWidth = static_cast<LONG>(w);
    bih->biHeight = -static_cast<LONG>(h);
    bih->biPlanes = 1;
    bih->biBitCount = 32;
    bih->biCompression = BI_RGB;
    bih->biSizeImage = static_cast<DWORD>(pixBytes);
    HRESULT hr = conv->CopyPixels(nullptr, w * 4, static_cast<UINT>(pixBytes),
                                  reinterpret_cast<BYTE*>(bih + 1));
    GlobalUnlock(mem);
    if (FAILED(hr) || !OpenClipboard(g_hostHwnd)) {
        GlobalFree(mem);
        return false;
    }
    EmptyClipboard();
    if (!SetClipboardData(CF_DIB, mem)) GlobalFree(mem);
    CloseClipboard();
    return true;
}

// ===== Live Windows & Process Groups (Window Switcher + Process Killer) =====
std::wstring QueryProcPath(DWORD pid) {
    if (!pid) return {};
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return {};
    wchar_t buf[MAX_PATH * 2]{};
    DWORD n = ARRAYSIZE(buf);
    std::wstring res;
    if (QueryFullProcessImageNameW(p, 0, buf, &n)) res.assign(buf, n);
    CloseHandle(p);
    return res;
}

int FindAppIndexForExe(const std::wstring& path) {
    if (path.empty()) return -1;
    std::wstring np = NormalizePath(path);
    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (!g_apps[i].target.empty() && NormalizePath(g_apps[i].target) == np) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

struct EnumWinCtx {
    HWND skipWnd = nullptr;
    std::vector<WindowEntry>* out = nullptr;
};

BOOL CALLBACK EnumSpotlightWindowsProc(HWND hwnd, LPARAM lp) {
    auto* c = reinterpret_cast<EnumWinCtx*>(lp);
    if (hwnd == c->skipWnd || hwnd == g_hostHwnd || !IsWindowVisible(hwnd)) {
        return TRUE;
    }
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return TRUE;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked != 0) {
        return TRUE;
    }
    WCHAR cls[64]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        if (_wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
            _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
            _wcsicmp(cls, kSpotlightUiClass) == 0) {
            return TRUE;
        }
    }
    wchar_t title[256]{};
    DWORD_PTR res = 0;
    if (!SendMessageTimeoutW(hwnd, WM_GETTEXT, ARRAYSIZE(title),
                             reinterpret_cast<LPARAM>(title),
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 30, &res)) {
        return TRUE;
    }
    std::wstring t = TrimBlanks(title);
    if (t.empty()) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    WindowEntry w;
    w.hwnd = hwnd;
    w.pid = pid;
    w.title = std::move(t);
    w.exePath = QueryProcPath(pid);
    w.process = w.exePath.empty() ? std::wstring() : FileNameOf(w.exePath);
    w.folded = FoldString(w.title + L" " + w.process);
    w.appIdx = FindAppIndexForExe(w.exePath);
    w.iconic = IsIconic(hwnd) != FALSE;
    w.zoomed = IsZoomed(hwnd) != FALSE;
    w.topmost = (ex & WS_EX_TOPMOST) != 0;
    c->out->push_back(std::move(w));
    return c->out->size() < 150 ? TRUE : FALSE;
}

void RefreshWindowsSnapshot(HWND uiHwnd) {
    g_windows.clear();
    EnumWinCtx ctx{uiHwnd, &g_windows};
    EnumWindows(EnumSpotlightWindowsProc, reinterpret_cast<LPARAM>(&ctx));
}

void RefreshProcessesSnapshot() {
    g_processes.clear();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    DWORD selfPid = GetCurrentProcessId();
    std::unordered_map<std::wstring, size_t> byKey;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            if (pe.th32ProcessID == 0 || pe.th32ProcessID == 4 ||
                pe.th32ProcessID == selfPid) {
                continue;
            }
            std::wstring exe = pe.szExeFile;
            std::wstring key = FoldString(exe);
            if (key == L"svchost.exe" || key == L"csrss.exe" ||
                key == L"smss.exe" || key == L"wininit.exe" ||
                key == L"lsass.exe" || key == L"services.exe" ||
                key == L"dwm.exe" || key == L"system") {
                continue;
            }
            SIZE_T ws = 0;
            std::wstring fullPath;
            if (HANDLE hp = OpenProcess(
                    PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE,
                    pe.th32ProcessID)) {
                PROCESS_MEMORY_COUNTERS pmc{};
                if (GetProcessMemoryInfo(hp, &pmc, sizeof(pmc))) {
                    ws = pmc.WorkingSetSize;
                }
                wchar_t buf[MAX_PATH * 2]{};
                DWORD len = ARRAYSIZE(buf);
                if (QueryFullProcessImageNameW(hp, 0, buf, &len)) {
                    fullPath.assign(buf, len);
                }
                CloseHandle(hp);
            }
            auto it = byKey.find(key);
            if (it == byKey.end()) {
                byKey[key] = g_processes.size();
                ProcessGroup pg;
                pg.exeName = exe;
                pg.folded = key;
                pg.exePath = fullPath;
                pg.pids.push_back(pe.th32ProcessID);
                pg.totalBytes = ws;
                pg.appIdx = FindAppIndexForExe(fullPath);
                g_processes.push_back(std::move(pg));
            } else {
                g_processes[it->second].pids.push_back(pe.th32ProcessID);
                g_processes[it->second].totalBytes += ws;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    std::sort(g_processes.begin(), g_processes.end(),
              [](const ProcessGroup& a, const ProcessGroup& b) {
                  return a.totalBytes > b.totalBytes;
              });
}


// ============================================================================
// 5. Search Ranking, Mode Chips, Autocomplete, Quick Look Loader & Actions
// ============================================================================

struct SpotlightUiState;
HWND GetSpotlightUiHwnd();
void HideSpotlight(bool restoreFocus);
void ShowToast(const std::wstring& msg);
void TriggerSpotlightRebuild(bool triggerFileSearch);
void SyncQuickLookToSelection();

// Fuzzy + Prefix + Word-Start Scorer with Match Highlight Indices
int ScoreFuzzyText(const std::wstring& foldedTarget,
                   const std::wstring& foldedQuery,
                   std::vector<uint8_t>* maskOut) {
    if (foldedQuery.empty()) return 10;
    if (foldedTarget.empty()) return -1;

    if (foldedTarget == foldedQuery) {
        if (maskOut) {
            maskOut->assign(foldedTarget.size(), 1);
        }
        return 1000;
    }

    // Direct prefix match
    if (foldedTarget.size() >= foldedQuery.size() &&
        foldedTarget.compare(0, foldedQuery.size(), foldedQuery) == 0) {
        if (maskOut) {
            maskOut->assign(foldedTarget.size(), 0);
            for (size_t i = 0; i < foldedQuery.size(); ++i) (*maskOut)[i] = 1;
        }
        return 860 - static_cast<int>(
                         std::min<size_t>(60, foldedTarget.size() -
                                                  foldedQuery.size()));
    }

    // Substring match (higher score at word boundary)
    if (size_t sub = foldedTarget.find(foldedQuery); sub != std::wstring::npos) {
        bool wordStart = (sub == 0 || !iswalnum(foldedTarget[sub - 1]));
        if (maskOut) {
            maskOut->assign(foldedTarget.size(), 0);
            for (size_t i = 0; i < foldedQuery.size(); ++i) {
                (*maskOut)[sub + i] = 1;
            }
        }
        return (wordStart ? 760 : 620) -
               static_cast<int>(std::min<size_t>(80, sub * 3));
    }

    // Tokenized multi-word query: every space-separated token must match
    if (foldedQuery.find(L' ') != std::wstring::npos) {
        std::vector<std::wstring> tokens;
        size_t i = 0;
        while (i < foldedQuery.size()) {
            size_t a = foldedQuery.find_first_not_of(L" \t", i);
            if (a == std::wstring::npos) break;
            size_t b = foldedQuery.find_first_of(L" \t", a);
            if (b == std::wstring::npos) b = foldedQuery.size();
            tokens.push_back(foldedQuery.substr(a, b - a));
            i = b + 1;
        }
        if (tokens.size() >= 2) {
            std::vector<uint8_t> m(foldedTarget.size(), 0);
            int total = 540;
            bool allOk = true;
            for (const auto& tok : tokens) {
                size_t p = foldedTarget.find(tok);
                if (p == std::wstring::npos) {
                    allOk = false;
                    break;
                }
                bool wb = (p == 0 || !iswalnum(foldedTarget[p - 1]));
                total += wb ? 45 : 18;
                for (size_t k = 0; k < tok.size(); ++k) m[p + k] = 1;
            }
            if (allOk) {
                if (maskOut) *maskOut = std::move(m);
                return total;
            }
        }
    }

    // Subsequence / Initialism fuzzy match
    std::vector<uint8_t> m(foldedTarget.size(), 0);
    size_t qi = 0;
    size_t prevMatch = std::wstring::npos;
    int score = 380;
    for (size_t ti = 0; ti < foldedTarget.size() && qi < foldedQuery.size();
         ++ti) {
        if (foldedTarget[ti] == foldedQuery[qi]) {
            m[ti] = 1;
            bool wb = (ti == 0 || !iswalnum(foldedTarget[ti - 1]));
            if (wb) score += 28;
            if (prevMatch != std::wstring::npos && ti == prevMatch + 1) {
                score += 18;
            } else if (prevMatch != std::wstring::npos) {
                score -= static_cast<int>(std::min<size_t>(18, ti - prevMatch));
            }
            prevMatch = ti;
            ++qi;
        }
    }
    if (qi < foldedQuery.size()) return -1;
    if (maskOut) *maskOut = std::move(m);
    return std::max(120, score - static_cast<int>(foldedTarget.size() / 2));
}

bool LooksLikeDirectUrl(const std::wstring& raw) {
    std::wstring s = TrimBlanks(raw);
    if (s.size() < 4 || s.find(L' ') != std::wstring::npos) return false;
    if (StartsWithNoCase(s, L"http://") || StartsWithNoCase(s, L"https://") ||
        StartsWithNoCase(s, L"ms-settings:") || StartsWithNoCase(s, L"file:///")) {
        return true;
    }
    if (StartsWithNoCase(s, L"localhost:") || s == L"localhost") return true;
    size_t dot = s.rfind(L'.');
    if (dot != std::wstring::npos && dot > 0 && dot + 2 < s.size()) {
        std::wstring tld = ToLower(s.substr(dot + 1));
        size_t slash = tld.find_first_of(L"/?:");
        if (slash != std::wstring::npos) tld.resize(slash);
        if (tld == L"com" || tld == L"org" || tld == L"net" ||
            tld == L"io" || tld == L"dev" || tld == L"app" ||
            tld == L"ai" || tld == L"edu" || tld == L"gov" ||
            tld == L"co" || tld == L"me" || tld == L"gg") {
            return true;
        }
    }
    return false;
}

bool LooksLikeFilesystemPath(const std::wstring& raw) {
    std::wstring s = TrimBlanks(raw);
    if (s.size() >= 3 && iswalpha(s[0]) && s[1] == L':' &&
        (s[2] == L'\\' || s[2] == L'/')) {
        return true;
    }
    if (s.size() >= 3 && s[0] == L'\\' && s[1] == L'\\') return true;
    return false;
}

struct QueryBuildOutput {
    std::vector<SearchResult> results;
    Calc::Result calc;
    bool hasCalc = false;
    std::wstring autoFillGhost;
    std::wstring autoFillFull;
    std::wstring autoFillBadge;
};

int LearnedBonusForApp(const AppEntry& app, const std::wstring& foldedQuery) {
    if (foldedQuery.empty()) return 0;
    for (size_t i = 0; i < g_learned.size(); ++i) {
        if (g_learned[i].appId == app.id && g_learned[i].query == foldedQuery) {
            return static_cast<int>(260 - std::min<size_t>(120, i * 18));
        }
    }
    return 0;
}

bool FileMatchesQueryTokens(const FileEntry& f,
                            const std::vector<std::wstring>& tokens) {
    if (tokens.empty()) return true;
    std::wstring pathFold = FoldString(f.dir);
    for (const auto& t : tokens) {
        if (f.folded.find(t) == std::wstring::npos &&
            pathFold.find(t) == std::wstring::npos) {
            return false;
        }
    }
    return true;
}

QueryBuildOutput BuildSpotlightResults(SpotlightMode mode,
                                       FileSubTab fileTab,
                                       const std::wstring& rawQuery,
                                       std::wstring& autoPrefixModeLabel,
                                       SpotlightMode* outPrefixMode,
                                       std::wstring* outEffectiveQuery) {
    QueryBuildOutput out;
    std::wstring trimmed = TrimBlanks(rawQuery);
    SpotlightMode effectiveMode = mode;
    std::wstring query = trimmed;
    if (outPrefixMode) *outPrefixMode = SpotlightMode::All;
    autoPrefixModeLabel.clear();

    // Detect Fluent Search / Spotlight inline mode prefixes when in All mode
    if (mode == SpotlightMode::All && !query.empty()) {
        struct PrefixRule {
            const wchar_t* prefix;
            size_t len;
            SpotlightMode target;
            const wchar_t* chipLabel;
            bool requireTrailingSpaceOrExact;
        };
        static const PrefixRule kRules[] = {
            {L"kl ", 3, SpotlightMode::KillProcess, L"Kill", false},
            {L"kill ", 5, SpotlightMode::KillProcess, L"Kill", false},
            {L"win:", 4, SpotlightMode::Windows, L"Windows", false},
            {L"w:", 2, SpotlightMode::Windows, L"Windows", false},
            {L"bm:", 3, SpotlightMode::Bookmarks, L"Bookmarks", false},
            {L"cfg:", 4, SpotlightMode::Settings, L"Settings", false},
            {L"set:", 4, SpotlightMode::Settings, L"Settings", false},
            {L"cb:", 3, SpotlightMode::Clipboard, L"Clipboard", false},
            {L"clip:", 5, SpotlightMode::Clipboard, L"Clipboard", false},
            {L"app:", 4, SpotlightMode::Apps, L"Apps", false},
            {L"apps:", 5, SpotlightMode::Apps, L"Apps", false},
            {L"file:", 5, SpotlightMode::Files, L"Files", false},
            {L"f:", 2, SpotlightMode::Files, L"Files", false},
            {L">", 1, SpotlightMode::Actions, L"Actions", false},
            {L"act:", 4, SpotlightMode::Actions, L"Actions", false},
        };
        for (const auto& r : kRules) {
            if (StartsWithNoCase(query, std::wstring_view(r.prefix, r.len))) {
                effectiveMode = r.target;
                query = TrimBlanks(query.substr(r.len));
                autoPrefixModeLabel = r.chipLabel;
                if (outPrefixMode) *outPrefixMode = r.target;
                break;
            }
        }
        // Single exact "kl" without space also highlights Kill Process nicely
    }

    if (outEffectiveQuery) *outEffectiveQuery = query;
    std::wstring foldedQ = FoldString(query);

    // Prepare tokens for multi-term file matching
    std::vector<std::wstring> queryTokens;
    {
        size_t i = 0;
        while (i < foldedQ.size()) {
            size_t a = foldedQ.find_first_not_of(L" \t", i);
            if (a == std::wstring::npos) break;
            size_t b = foldedQ.find_first_of(L" \t", a);
            if (b == std::wstring::npos) b = foldedQ.size();
            queryTokens.push_back(foldedQ.substr(a, b - a));
            i = b + 1;
        }
    }

    // 1. Live Calculator / Unit & Base Converter
    if (g_settings.enableCalculator &&
        (effectiveMode == SpotlightMode::All ||
         effectiveMode == SpotlightMode::Actions) &&
        !trimmed.empty()) {
        std::wstring mathSrc = trimmed;
        if (!mathSrc.empty() && mathSrc[0] == L'=') {
            mathSrc = TrimBlanks(mathSrc.substr(1));
        }
        if (!mathSrc.empty()) {
            Calc::Result cr = Calc::Evaluate(mathSrc);
            if (cr.status == Calc::Status::Ok) {
                out.calc = std::move(cr);
                out.hasCalc = true;
            }
        }
    }

    auto pushUniqueApp = [&](int idx, int baseScore,
                             const std::vector<uint8_t>& mask) {
        const auto& a = g_apps[static_cast<size_t>(idx)];
        SearchResult r;
        r.kind = ResultKind::App;
        r.appIdx = idx;
        r.score = baseScore + FrecencyScore(a) +
                  LearnedBonusForApp(a, foldedQ) + (a.pinned ? 140 : 0);
        r.title = a.name;
        r.foldedTitle = a.folded;
        r.subtitle = a.subtitle.empty() ? ParentDir(a.target) : a.subtitle;
        r.payload = !a.launch.empty() ? a.launch
                                      : (!a.target.empty() ? a.target : a.id);
        r.pinned = a.pinned;
        r.rightBadge = a.pinned ? L"Pinned" : L"App";
        r.matchMask = mask;
        r.badgeColor = D2D1::ColorF(0.22f, 0.56f, 0.96f, 1.0f);
        r.glyph = ActionGlyph::Grid;
        out.results.push_back(std::move(r));
    };

    // 2. Mode-specific or All-mode result collection
    if (effectiveMode == SpotlightMode::All ||
        effectiveMode == SpotlightMode::Apps) {
        for (int i = 0; i < static_cast<int>(g_apps.size()); ++i) {
            const auto& a = g_apps[static_cast<size_t>(i)];
            if (foldedQ.empty()) {
                if (effectiveMode == SpotlightMode::Apps) {
                    if (!a.exactOnly) pushUniqueApp(i, a.pinned ? 500 : 120, {});
                } else if (a.pinned || a.uses > 0) {
                    pushUniqueApp(i, a.pinned ? 420 : 160, {});
                }
                continue;
            }
            if (a.exactOnly) {
                if (a.folded == foldedQ || a.foldedFile == foldedQ) {
                    std::vector<uint8_t> m(a.folded.size(), 1);
                    pushUniqueApp(i, 780, m);
                }
                continue;
            }
            std::vector<uint8_t> mask;
            int sc = ScoreFuzzyText(a.folded, foldedQ, &mask);
            if (sc >= 0) pushUniqueApp(i, sc + 40, mask);
        }
    }

    // Actions
    if (effectiveMode == SpotlightMode::All ||
        effectiveMode == SpotlightMode::Actions) {
        for (int i = 0; i < kBuiltinActionCount; ++i) {
            const auto& act = kBuiltinActions[i];
            if (foldedQ.empty()) {
                if (effectiveMode == SpotlightMode::Actions) {
                    SearchResult r;
                    r.kind = ResultKind::Action;
                    r.actionIdx = i;
                    r.score = 600 - i;
                    r.title = act.title;
                    r.foldedTitle = FoldString(act.title);
                    r.subtitle = act.subtitle;
                    r.rightBadge = act.quickKey ? act.quickKey : L"Run";
                    r.badgeColor = act.badgeColor;
                    r.glyph = act.glyph;
                    out.results.push_back(std::move(r));
                }
                continue;
            }
            std::wstring qk = act.quickKey ? FoldString(act.quickKey) : L"";
            if (!qk.empty() && qk == foldedQ && qk != L"↵") {
                SearchResult r;
                r.kind = ResultKind::Action;
                r.actionIdx = i;
                r.score = 1120;
                r.title = act.title;
                r.foldedTitle = FoldString(act.title);
                r.subtitle = act.subtitle;
                r.rightBadge = act.quickKey;
                r.badgeColor = act.badgeColor;
                r.glyph = act.glyph;
                out.results.push_back(std::move(r));
                continue;
            }
            std::vector<uint8_t> mask;
            std::wstring foldedTitle = FoldString(act.title);
            int sc = ScoreFuzzyText(foldedTitle, foldedQ, &mask);
            if (sc < 0 && act.keywords) {
                int ksc = ScoreFuzzyText(FoldString(act.keywords), foldedQ,
                                         nullptr);
                if (ksc >= 0) sc = ksc - 110;
            }
            if (sc >= 0) {
                SearchResult r;
                r.kind = ResultKind::Action;
                r.actionIdx = i;
                r.score = sc + (effectiveMode == SpotlightMode::Actions ? 120 : 15);
                r.title = act.title;
                r.foldedTitle = std::move(foldedTitle);
                r.subtitle = act.subtitle;
                r.rightBadge = act.quickKey ? act.quickKey : L"Run";
                r.matchMask = std::move(mask);
                r.badgeColor = act.badgeColor;
                r.glyph = act.glyph;
                out.results.push_back(std::move(r));
            }
        }
    }

    // Settings
    if (effectiveMode == SpotlightMode::All ||
        effectiveMode == SpotlightMode::Settings) {
        for (int i = 0; i < kBuiltinSettingCount; ++i) {
            const auto& st = kBuiltinSettings[i];
            std::wstring ft = FoldString(st.title);
            if (foldedQ.empty()) {
                if (effectiveMode == SpotlightMode::Settings) {
                    SearchResult r;
                    r.kind = ResultKind::Setting;
                    r.settingIdx = i;
                    r.score = 540 - i;
                    r.title = st.title;
                    r.foldedTitle = std::move(ft);
                    r.subtitle = L"Windows Settings";
                    r.payload = st.uri;
                    r.rightBadge = L"Open";
                    r.badgeColor = st.badgeColor;
                    r.glyph = st.glyph;
                    out.results.push_back(std::move(r));
                }
                continue;
            }
            std::vector<uint8_t> mask;
            int sc = ScoreFuzzyText(ft, foldedQ, &mask);
            if (sc < 0 && st.keywords) {
                int ksc = ScoreFuzzyText(FoldString(st.keywords), foldedQ, nullptr);
                if (ksc >= 0) sc = ksc - 120;
            }
            if (sc >= 0) {
                SearchResult r;
                r.kind = ResultKind::Setting;
                r.settingIdx = i;
                r.score = sc + (effectiveMode == SpotlightMode::Settings ? 100 : -25);
                r.title = st.title;
                r.foldedTitle = std::move(ft);
                r.subtitle = L"Windows Settings";
                r.payload = st.uri;
                r.rightBadge = L"Open";
                r.matchMask = std::move(mask);
                r.badgeColor = st.badgeColor;
                r.glyph = st.glyph;
                out.results.push_back(std::move(r));
            }
        }
    }

    // Files (Everything + Local BFS + Recent Files)
    if (effectiveMode == SpotlightMode::Files ||
        (effectiveMode == SpotlightMode::All && !foldedQ.empty())) {
        // Explicit filesystem path check
        if (LooksLikeFilesystemPath(query)) {
            WIN32_FILE_ATTRIBUTE_DATA fa{};
            if (GetFileAttributesExW(query.c_str(), GetFileExInfoStandard, &fa)) {
                SearchResult r;
                r.kind = ResultKind::File;
                r.score = 1220;
                r.isDir = (fa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                r.title = FileNameOf(query);
                if (r.title.empty()) r.title = query;
                r.foldedTitle = FoldString(r.title);
                r.subtitle = ParentDir(query);
                r.payload = query;
                r.size = (static_cast<uint64_t>(fa.nFileSizeHigh) << 32) |
                         fa.nFileSizeLow;
                r.writeTime = fa.ftLastWriteTime;
                r.ext = r.isDir ? std::wstring() : FileExtOf(r.title);
                r.rightBadge = r.isDir ? L"Folder" : FormatFileSize(r.size);
                r.badgeColor = r.isDir
                                   ? D2D1::ColorF(0.24f, 0.62f, 0.96f, 1.0f)
                                   : D2D1::ColorF(0.46f, 0.56f, 0.72f, 1.0f);
                r.glyph = r.isDir ? ActionGlyph::Folder : ActionGlyph::Doc;
                out.results.push_back(std::move(r));
            }
        }

        // Everything / Async file search results
        const auto& pool = (effectiveMode == SpotlightMode::Files &&
                            foldedQ.empty() && g_searchedFiles.empty())
                               ? g_recentFiles
                               : g_searchedFiles;
        std::unordered_set<std::wstring> seenFiles;
        int fileCount = 0;
        for (int i = 0; i < static_cast<int>(pool.size()); ++i) {
            const auto& f = pool[static_cast<size_t>(i)];
            if (effectiveMode == SpotlightMode::Files &&
                !SubTabMatchesFile(fileTab, f.isDir, f.ext)) {
                continue;
            }
            if (!FileMatchesQueryTokens(f, queryTokens)) continue;
            std::wstring np = NormalizePath(f.path);
            if (!seenFiles.insert(np).second) continue;

            std::vector<uint8_t> mask;
            int sc = foldedQ.empty() ? (540 - i)
                                     : ScoreFuzzyText(f.folded, foldedQ, &mask);
            if (sc < 0) sc = 220;
            sc += UserPathPriority(f.path);
            if (f.isDir) sc += 24;

            SearchResult r;
            r.kind = ResultKind::File;
            r.fileIdx = i;
            r.score =
                sc + (effectiveMode == SpotlightMode::Files ? 160 : -55);
            r.title = f.name;
            r.foldedTitle = FoldString(f.name);
            r.subtitle = f.dir.empty() ? f.path : f.dir;
            r.payload = f.path;
            r.isDir = f.isDir;
            r.size = f.size;
            r.writeTime = f.writeTime;
            r.ext = f.ext;
            std::wstring age = FormatRelativeTime(f.writeTime);
            r.rightBadge = !age.empty()
                               ? age
                               : (f.isDir ? L"Folder" : FormatFileSize(f.size));
            r.matchMask = std::move(mask);
            r.badgeColor = f.isDir
                               ? D2D1::ColorF(0.24f, 0.62f, 0.96f, 1.0f)
                               : D2D1::ColorF(0.46f, 0.56f, 0.72f, 1.0f);
            r.glyph = f.isDir ? ActionGlyph::Folder : ActionGlyph::Doc;
            out.results.push_back(std::move(r));
            if (++fileCount >= (effectiveMode == SpotlightMode::Files ? 60 : 12)) {
                break;
            }
        }

        // Also match recent files quickly in All mode even before Everything responds
        if (effectiveMode == SpotlightMode::All) {
            for (int i = 0; i < static_cast<int>(g_recentFiles.size()); ++i) {
                const auto& f = g_recentFiles[static_cast<size_t>(i)];
                if (!seenFiles.insert(NormalizePath(f.path)).second) continue;
                std::vector<uint8_t> mask;
                int sc = ScoreFuzzyText(FoldString(f.name), foldedQ, &mask);
                if (sc < 0) continue;
                SearchResult r;
                r.kind = ResultKind::RecentFile;
                r.fileIdx = i;
                r.score = sc - 15 + UserPathPriority(f.path);
                r.title = f.name;
                r.foldedTitle = FoldString(f.name);
                r.subtitle = f.dir;
                r.payload = f.path;
                r.isDir = f.isDir;
                r.size = f.size;
                r.writeTime = f.writeTime;
                r.ext = f.ext;
                std::wstring age = FormatRelativeTime(f.writeTime);
                r.rightBadge = !age.empty() ? age : L"Recent";
                r.matchMask = std::move(mask);
                r.badgeColor = f.isDir
                                   ? D2D1::ColorF(0.24f, 0.62f, 0.96f, 1.0f)
                                   : D2D1::ColorF(0.46f, 0.56f, 0.72f, 1.0f);
                r.glyph = f.isDir ? ActionGlyph::Folder : ActionGlyph::Doc;
                out.results.push_back(std::move(r));
            }
        }
    }

    // Clipboard History
    if (g_settings.enableClipboardHistory &&
        effectiveMode == SpotlightMode::Clipboard) {
        for (int i = 0; i < static_cast<int>(g_clips.size()); ++i) {
            const auto& c = g_clips[static_cast<size_t>(i)];
            std::wstring label = c.image
                                     ? (L"Screenshot (" + std::to_wstring(c.w) +
                                        L" x " + std::to_wstring(c.h) + L")")
                                     : c.text;
            std::wstring preview = label;
            for (wchar_t& ch : preview) {
                if (ch == L'\r' || ch == L'\n' || ch == L'\t') ch = L' ';
            }
            if (preview.size() > 96) preview = preview.substr(0, 95) + L"\u2026";
            std::vector<uint8_t> mask;
            std::wstring ft = FoldString(preview);
            int sc = foldedQ.empty() ? ((c.pinned ? 820 : 520) - i)
                                     : ScoreFuzzyText(FoldString(label), foldedQ,
                                                      &mask);
            if (sc < 0) continue;

            SearchResult r;
            r.kind = ResultKind::Clipboard;
            r.clipIdx = i;
            r.score = sc + (c.pinned ? 90 : 0);
            r.title = preview;
            r.foldedTitle = std::move(ft);
            r.isImage = c.image;
            r.pinned = c.pinned;
            std::wstring age = FormatStamp(c.stamp);
            r.subtitle = c.image
                             ? L"Encrypted image capture"
                             : (std::to_wstring(c.text.size()) + L" chars");
            r.payload = c.image ? ClipImagePath(c.text) : c.text;
            r.rightBadge = c.pinned ? L"Pinned"
                                    : (!age.empty() ? age : L"Paste");
            r.matchMask = std::move(mask);
            r.badgeColor = D2D1::ColorF(0.24f, 0.70f, 0.58f, 1.0f);
            r.glyph = c.image ? ActionGlyph::Camera : ActionGlyph::Clipboard;
            out.results.push_back(std::move(r));
        }
    } else if (g_settings.enableClipboardHistory &&
               effectiveMode == SpotlightMode::All && !foldedQ.empty()) {
        int added = 0;
        for (int i = 0; i < static_cast<int>(g_clips.size()) && added < 3; ++i) {
            const auto& c = g_clips[static_cast<size_t>(i)];
            if (c.image) continue;
            std::vector<uint8_t> mask;
            std::wstring flat = c.text;
            for (wchar_t& ch : flat) {
                if (ch == L'\r' || ch == L'\n' || ch == L'\t') ch = L' ';
            }
            if (flat.size() > 80) flat = flat.substr(0, 79) + L"\u2026";
            int sc = ScoreFuzzyText(FoldString(flat), foldedQ, &mask);
            if (sc < 260) continue;
            SearchResult r;
            r.kind = ResultKind::Clipboard;
            r.clipIdx = i;
            r.score = sc - 135;
            r.title = flat;
            r.foldedTitle = FoldString(flat);
            r.subtitle = L"Clipboard item";
            r.payload = c.text;
            r.pinned = c.pinned;
            r.rightBadge = L"Paste";
            r.matchMask = std::move(mask);
            r.badgeColor = D2D1::ColorF(0.24f, 0.70f, 0.58f, 1.0f);
            r.glyph = ActionGlyph::Clipboard;
            out.results.push_back(std::move(r));
            ++added;
        }
    }

    // Open Windows Switcher
    if (effectiveMode == SpotlightMode::Windows ||
        (effectiveMode == SpotlightMode::All && !foldedQ.empty())) {
        for (int i = 0; i < static_cast<int>(g_windows.size()); ++i) {
            const auto& w = g_windows[static_cast<size_t>(i)];
            std::vector<uint8_t> mask;
            int sc = foldedQ.empty() ? (560 - i)
                                     : ScoreFuzzyText(w.folded, foldedQ, &mask);
            if (sc < 0) continue;
            SearchResult r;
            r.kind = ResultKind::Window;
            r.windowIdx = i;
            r.appIdx = w.appIdx;
            r.score =
                sc + (effectiveMode == SpotlightMode::Windows ? 120 : -85);
            r.title = w.title;
            r.foldedTitle = FoldString(w.title);
            r.subtitle =
                w.process + (w.iconic ? L" · Minimized"
                                      : (w.zoomed ? L" · Maximized" : L""));
            r.payload = w.exePath;
            r.rightBadge = L"Focus";
            r.matchMask = std::move(mask);
            r.badgeColor = D2D1::ColorF(0.28f, 0.54f, 0.80f, 1.0f);
            r.glyph = ActionGlyph::Monitor;
            out.results.push_back(std::move(r));
        }
    }

    // Running Process Killer (Fluent Search `kl` parity)
    if (effectiveMode == SpotlightMode::KillProcess) {
        for (int i = 0; i < static_cast<int>(g_processes.size()); ++i) {
            const auto& p = g_processes[static_cast<size_t>(i)];
            std::vector<uint8_t> mask;
            int sc = foldedQ.empty() ? (560 - i)
                                     : ScoreFuzzyText(p.folded, foldedQ, &mask);
            if (sc < 0) continue;
            SearchResult r;
            r.kind = ResultKind::Process;
            r.procIdx = i;
            r.appIdx = p.appIdx;
            r.score = sc + 160;
            r.title = p.exeName;
            r.foldedTitle = p.folded;
            r.subtitle =
                std::to_wstring(p.pids.size()) +
                (p.pids.size() == 1 ? L" process · " : L" processes · ") +
                FormatFileSize(p.totalBytes);
            r.payload = p.exePath;
            r.rightBadge = L"Kill";
            r.matchMask = std::move(mask);
            r.badgeColor = D2D1::ColorF(0.88f, 0.28f, 0.30f, 1.0f);
            r.glyph = ActionGlyph::Skull;
            out.results.push_back(std::move(r));
        }
    }

    // Browser Bookmarks
    if (g_settings.enableBookmarks &&
        (effectiveMode == SpotlightMode::Bookmarks ||
         (effectiveMode == SpotlightMode::All && !foldedQ.empty()))) {
        int bCount = 0;
        for (int i = 0; i < static_cast<int>(g_bookmarks.size()); ++i) {
            const auto& b = g_bookmarks[static_cast<size_t>(i)];
            std::vector<uint8_t> mask;
            int sc = foldedQ.empty() ? (500 - i)
                                     : ScoreFuzzyText(b.folded, foldedQ, &mask);
            if (sc < 0) continue;
            SearchResult r;
            r.kind = ResultKind::Bookmark;
            r.bookmarkIdx = i;
            r.score = sc + (effectiveMode == SpotlightMode::Bookmarks ? 140 : -75);
            r.title = b.name;
            r.foldedTitle = FoldString(b.name);
            r.subtitle = b.browser + L" · " + b.url;
            r.payload = b.url;
            r.rightBadge = L"Open";
            r.matchMask = std::move(mask);
            r.badgeColor = D2D1::ColorF(0.88f, 0.68f, 0.22f, 1.0f);
            r.glyph = ActionGlyph::Star;
            out.results.push_back(std::move(r));
            if (effectiveMode == SpotlightMode::Bookmarks && ++bCount >= 40) break;
        }
    }

    // Direct URL or Web Search fallback
    if (effectiveMode == SpotlightMode::All && !trimmed.empty()) {
        if (LooksLikeDirectUrl(trimmed)) {
            SearchResult r;
            r.kind = ResultKind::Url;
            r.score = 1080;
            r.title = L"Open " + trimmed;
            r.foldedTitle = FoldString(r.title);
            r.subtitle = L"Open website in default browser";
            r.payload = (StartsWithNoCase(trimmed, L"http://") ||
                         StartsWithNoCase(trimmed, L"https://") ||
                         StartsWithNoCase(trimmed, L"ms-settings:") ||
                         StartsWithNoCase(trimmed, L"file:///"))
                            ? trimmed
                            : (L"https://" + trimmed);
            r.rightBadge = L"Open";
            r.badgeColor = D2D1::ColorF(0.24f, 0.56f, 0.94f, 1.0f);
            r.glyph = ActionGlyph::Globe;
            out.results.push_back(std::move(r));
        }
        if (g_settings.enableWebSearch && !out.hasCalc) {
            const wchar_t* engName = L"Google";
            switch (g_settings.webEngine) {
                case WebEngine::DuckDuckGo: engName = L"DuckDuckGo"; break;
                case WebEngine::Bing: engName = L"Bing"; break;
                case WebEngine::YouTube: engName = L"YouTube"; break;
                case WebEngine::Wikipedia: engName = L"Wikipedia"; break;
                case WebEngine::Google:
                default: engName = L"Google"; break;
            }
            SearchResult r;
            r.kind = ResultKind::Web;
            r.score = 65;  // Appears neatly at bottom as Safari/Web search fallback
            r.title = std::wstring(L"Search ") + engName + L" for \u201C" +
                      trimmed + L"\u201D";
            r.foldedTitle = FoldString(r.title);
            r.subtitle = std::wstring(engName) + L" Web Search";
            r.payload = trimmed;
            r.rightBadge = L"Web";
            r.badgeColor = D2D1::ColorF(0.24f, 0.56f, 0.94f, 1.0f);
            r.glyph = ActionGlyph::Search;
            out.results.push_back(std::move(r));
        }
    }

    // Default suggestions when expanded in All mode with empty query
    if (effectiveMode == SpotlightMode::All && foldedQ.empty() &&
        out.results.size() < 8) {
        for (int i = 0;
             i < static_cast<int>(g_recentFiles.size()) && i < 4; ++i) {
            const auto& f = g_recentFiles[static_cast<size_t>(i)];
            SearchResult r;
            r.kind = ResultKind::RecentFile;
            r.fileIdx = i;
            r.score = 190 - i;
            r.title = f.name;
            r.foldedTitle = FoldString(f.name);
            r.subtitle = f.dir;
            r.payload = f.path;
            r.isDir = f.isDir;
            r.size = f.size;
            r.writeTime = f.writeTime;
            r.ext = f.ext;
            std::wstring age = FormatRelativeTime(f.writeTime);
            r.rightBadge = !age.empty() ? age : L"Recent";
            r.badgeColor = f.isDir
                               ? D2D1::ColorF(0.24f, 0.62f, 0.96f, 1.0f)
                               : D2D1::ColorF(0.46f, 0.56f, 0.72f, 1.0f);
            r.glyph = f.isDir ? ActionGlyph::Folder : ActionGlyph::Doc;
            out.results.push_back(std::move(r));
        }
        static constexpr int kDefaultActs[] = {0, 1, 2, 7};
        for (int idx : kDefaultActs) {
            if (idx < kBuiltinActionCount && out.results.size() < 10) {
                const auto& act = kBuiltinActions[idx];
                SearchResult r;
                r.kind = ResultKind::Action;
                r.actionIdx = idx;
                r.score = 150 - idx;
                r.title = act.title;
                r.foldedTitle = FoldString(act.title);
                r.subtitle = act.subtitle;
                r.rightBadge = act.quickKey ? act.quickKey : L"Run";
                r.badgeColor = act.badgeColor;
                r.glyph = act.glyph;
                out.results.push_back(std::move(r));
            }
        }
    }

    // Sort highest score first
    std::stable_sort(
        out.results.begin(), out.results.end(),
        [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return CompareStringOrdinal(a.title.c_str(), -1, b.title.c_str(),
                                        -1, TRUE) == CSTR_LESS_THAN;
        });

    if (out.results.size() > 64) out.results.resize(64);

    // Inline Frosted Autocomplete Ghost Pill (Image 2: typed "Send" -> "Message — Run")
    if (!query.empty() && !out.results.empty() && !out.hasCalc) {
        const auto& top = out.results[0];
        if (top.title.size() > query.size() &&
            StartsWithNoCase(top.title, query)) {
            out.autoFillGhost = top.title.substr(query.size());
            out.autoFillFull = top.title;
            switch (top.kind) {
                case ResultKind::Action: out.autoFillBadge = L"Run"; break;
                case ResultKind::App: out.autoFillBadge = L"Open"; break;
                case ResultKind::Setting: out.autoFillBadge = L"Settings"; break;
                case ResultKind::File:
                case ResultKind::RecentFile:
                    out.autoFillBadge = top.isDir ? L"Folder" : L"Open";
                    break;
                case ResultKind::Window: out.autoFillBadge = L"Focus"; break;
                case ResultKind::Process: out.autoFillBadge = L"Kill"; break;
                case ResultKind::Bookmark: out.autoFillBadge = L"Bookmark"; break;
                default: break;
            }
        }
    }
    return out;
}

// IPC Pin-to-Dock integration with macos-tahoe-dock
constexpr ULONG_PTR kTahoePinDockMagic = 0x54414831u;

bool PinEntryToMacDock(const std::wstring& path,
                       const std::wstring& aumid,
                       const std::wstring& name) {
    HWND dockWnd = FindWindowW(L"WindhawkMacOS27DockWnd", nullptr);
    if (!dockWnd) return false;
    std::wstring payload = path + L"\x1F" + aumid + L"\x1F" + name;
    COPYDATASTRUCT cds{};
    cds.dwData = kTahoePinDockMagic;
    cds.cbData = static_cast<DWORD>((payload.size() + 1) * sizeof(wchar_t));
    cds.lpData = const_cast<wchar_t*>(payload.c_str());
    DWORD_PTR res = 0;
    return SendMessageTimeoutW(dockWnd, WM_COPYDATA,
                               reinterpret_cast<WPARAM>(g_hostHwnd),
                               reinterpret_cast<LPARAM>(&cds),
                               SMTO_ABORTIFHUNG | SMTO_BLOCK, 300, &res) != 0;
}

void SendGlobalPasteShortcut() {
    INPUT in[4]{};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = VK_CONTROL;
    in[1].type = INPUT_KEYBOARD;
    in[1].ki.wVk = 'V';
    in[2].type = INPUT_KEYBOARD;
    in[2].ki.wVk = 'V';
    in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3].type = INPUT_KEYBOARD;
    in[3].ki.wVk = VK_CONTROL;
    in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, in, sizeof(INPUT));
}

void SendMediaVirtualKey(WORD vk) {
    INPUT in[2]{};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = vk;
    in[1].type = INPUT_KEYBOARD;
    in[1].ki.wVk = vk;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

void ShellLaunchTarget(const std::wstring& target,
                       const std::wstring& args,
                       bool elevated) {
    if (target.empty()) return;
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = elevated ? L"runas" : L"open";
    sei.lpFile = target.c_str();
    sei.lpParameters = args.empty() ? nullptr : args.c_str();
    std::wstring dir = ParentDir(target);
    sei.lpDirectory = dir.empty() ? nullptr : dir.c_str();
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}

void ActivatePackagedAumid(const std::wstring& aumid) {
    if (aumid.empty()) return;
    winrt::com_ptr<IApplicationActivationManager> aam;
    if (SUCCEEDED(CoCreateInstance(CLSID_ApplicationActivationManager, nullptr,
                                   CLSCTX_LOCAL_SERVER,
                                   IID_PPV_ARGS(aam.put())))) {
        DWORD pid = 0;
        aam->ActivateApplication(aumid.c_str(), nullptr, AO_NONE, &pid);
    }
}

void RevealInFileExplorer(const std::wstring& path) {
    if (path.empty()) return;
    std::wstring params = L"/select,\"" + path + L"\"";
    ShellLaunchTarget(L"explorer.exe", params, false);
}

bool FocusOrSwitchWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    if (IsIconic(hwnd)) ShowWindowAsync(hwnd, SW_RESTORE);
    HWND fg = GetForegroundWindow();
    DWORD fgTid = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    DWORD curTid = GetCurrentThreadId();
    if (fgTid && fgTid != curTid) AttachThreadInput(curTid, fgTid, TRUE);
    BringWindowToTop(hwnd);
    BOOL ok = SetForegroundWindow(hwnd);
    if (fgTid && fgTid != curTid) AttachThreadInput(curTid, fgTid, FALSE);
    return ok != FALSE;
}

bool AdjustWindowsPrivilege(PCWSTR name) {
    HANDLE tok = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tok)) {
        return false;
    }
    LUID luid{};
    bool ok = false;
    if (LookupPrivilegeValueW(nullptr, name, &luid)) {
        TOKEN_PRIVILEGES tp{};
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        ok = AdjustTokenPrivileges(tok, FALSE, &tp, sizeof(tp), nullptr,
                                   nullptr) &&
             GetLastError() == ERROR_SUCCESS;
    }
    CloseHandle(tok);
    return ok;
}

// Execute a single built-in action; returns true if Spotlight should stay open
bool ExecuteBuiltinAction(const BuiltinAction& act) {
    std::wstring id = act.id;
    if (id == L"taskmgr") {
        ShellLaunchTarget(L"taskmgr.exe", L"", false);
    } else if (id == L"terminal") {
        ShellLaunchTarget(L"wt.exe", L"", false);
    } else if (id == L"powershell") {
        ShellLaunchTarget(L"powershell.exe", L"", false);
    } else if (id == L"cmd") {
        ShellLaunchTarget(L"cmd.exe", L"", false);
    } else if (id == L"explorer") {
        ShellLaunchTarget(L"explorer.exe", L"", false);
    } else if (id == L"control") {
        ShellLaunchTarget(L"control.exe", L"", false);
    } else if (id == L"devmgmt") {
        ShellLaunchTarget(L"mmc.exe", L"devmgmt.msc", false);
    } else if (id == L"services") {
        ShellLaunchTarget(L"mmc.exe", L"services.msc", false);
    } else if (id == L"regedit") {
        ShellLaunchTarget(L"regedit.exe", L"", true);
    } else if (id == L"lock") {
        LockWorkStation();
    } else if (id == L"sleep") {
        AdjustWindowsPrivilege(SE_SHUTDOWN_NAME);
        HMODULE powr = LoadLibraryExW(L"powrprof.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (powr) {
            using SetSuspendState_t = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
            if (auto fn = reinterpret_cast<SetSuspendState_t>(
                    GetProcAddress(powr, "SetSuspendState"))) {
                fn(FALSE, FALSE, FALSE);
            }
            FreeLibrary(powr);
        }
    } else if (id == L"hibernate") {
        AdjustWindowsPrivilege(SE_SHUTDOWN_NAME);
        HMODULE powr = LoadLibraryExW(L"powrprof.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (powr) {
            using SetSuspendState_t = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
            if (auto fn = reinterpret_cast<SetSuspendState_t>(
                    GetProcAddress(powr, "SetSuspendState"))) {
                fn(TRUE, FALSE, FALSE);
            }
            FreeLibrary(powr);
        }
    } else if (id == L"restart") {
        AdjustWindowsPrivilege(SE_SHUTDOWN_NAME);
        ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OTHER);
    } else if (id == L"shutdown") {
        AdjustWindowsPrivilege(SE_SHUTDOWN_NAME);
        ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF | EWX_FORCEIFHUNG,
                      SHTDN_REASON_MAJOR_OTHER);
    } else if (id == L"signout") {
        ExitWindowsEx(EWX_LOGOFF, 0);
    } else if (id == L"emptybin") {
        SHEmptyRecycleBinW(nullptr, nullptr,
                           SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI |
                               SHERB_NOSOUND);
        ShowToast(L"Recycle Bin emptied");
    } else if (id == L"media_play") {
        SendMediaVirtualKey(VK_MEDIA_PLAY_PAUSE);
        return true;
    } else if (id == L"media_next") {
        SendMediaVirtualKey(VK_MEDIA_NEXT_TRACK);
        return true;
    } else if (id == L"media_prev") {
        SendMediaVirtualKey(VK_MEDIA_PREV_TRACK);
        return true;
    } else if (id == L"vol_mute") {
        SendMediaVirtualKey(VK_VOLUME_MUTE);
        return true;
    } else if (id == L"clearclip") {
        ClearUnpinnedClipHistory();
        ShowToast(L"Cleared unpinned clipboard items");
        TriggerSpotlightRebuild(false);
        return true;
    } else if (id == L"reindex") {
        StartIndex();
        ShowToast(L"Reindexing applications\u2026");
        return true;
    }
    return false;
}

std::wstring BuildWebSearchUrl(const std::wstring& q) {
    std::wstring enc = UrlEncode(q);
    switch (g_settings.webEngine) {
        case WebEngine::DuckDuckGo:
            return L"https://duckduckgo.com/?q=" + enc;
        case WebEngine::Bing:
            return L"https://www.bing.com/search?q=" + enc;
        case WebEngine::YouTube:
            return L"https://www.youtube.com/results?search_query=" + enc;
        case WebEngine::Wikipedia:
            return L"https://en.wikipedia.org/wiki/Special:Search?search=" + enc;
        case WebEngine::Google:
        default:
            return L"https://www.google.com/search?q=" + enc;
    }
}

void OpenUrlInDefaultBrowser(const std::wstring& url) {
    if (url.empty()) return;
    ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

bool IsTextPreviewExt(const std::wstring& ext) {
    return ext == L"TXT" || ext == L"MD" || ext == L"LOG" ||
           ext == L"JSON" || ext == L"XML" || ext == L"YAML" ||
           ext == L"YML" || ext == L"INI" || ext == L"CONF" ||
           ext == L"CSV" || ext == L"C" || ext == L"CPP" ||
           ext == L"H" || ext == L"HPP" || ext == L"PY" ||
           ext == L"JS" || ext == L"TS" || ext == L"HTML" ||
           ext == L"CSS" || ext == L"RS" || ext == L"GO" ||
           ext == L"PS1" || ext == L"SH" || ext == L"BAT";
}

bool IsImagePreviewExt(const std::wstring& ext) {
    return ext == L"PNG" || ext == L"JPG" || ext == L"JPEG" ||
           ext == L"WEBP" || ext == L"GIF" || ext == L"BMP" ||
           ext == L"ICO";
}

void LoadQuickLookForResult(const SearchResult* r) {
    g_ql.gen++;
    g_ql.path.clear();
    g_ql.isImage = false;
    g_ql.isText = false;
    g_ql.loaded = false;
    g_ql.failed = false;
    g_ql.lines.clear();
    g_ql.scroll = 0;
    g_ql.hoveredAction = -1;
    g_ql.previewBmp.wic = nullptr;
    g_ql.previewBmp.d2d = nullptr;
    g_ql.previewBmp.boundDeviceGen = 0;
    g_ql.actionCount = 0;

    if (!r) return;
    if (r->kind == ResultKind::Clipboard) {
        if (r->isImage) {
            g_ql.isImage = true;
            g_ql.path = r->payload;
            EnqueueMediaJob({MediaJobKind::QuickLookImage, -1, 0, r->payload, L"",
                             g_ql.gen});
        } else if (r->clipIdx >= 0 &&
                   r->clipIdx < static_cast<int>(g_clips.size())) {
            g_ql.isText = true;
            g_ql.loaded = true;
            const auto& full = g_clips[static_cast<size_t>(r->clipIdx)].text;
            size_t pos = 0;
            while (pos < full.size() && g_ql.lines.size() < 48) {
                size_t nl = full.find(L'\n', pos);
                if (nl == std::wstring::npos) nl = full.size();
                std::wstring line = full.substr(pos, nl - pos);
                if (!line.empty() && line.back() == L'\r') line.pop_back();
                g_ql.lines.push_back(std::move(line));
                pos = nl + 1;
            }
        }
        return;
    }

    if (r->kind == ResultKind::File || r->kind == ResultKind::RecentFile) {
        g_ql.path = r->payload;
        if (!r->isDir && IsImagePreviewExt(r->ext)) {
            g_ql.isImage = true;
            EnqueueMediaJob({MediaJobKind::QuickLookImage, -1, 0, r->payload, L"",
                             g_ql.gen});
        } else if (!r->isDir && IsTextPreviewExt(r->ext) && r->size <= 512000) {
            g_ql.isText = true;
            std::vector<BYTE> bytes;
            if (ReadFileBytesRaw(r->payload, bytes, 64 * 1024)) {
                int wlen = MultiByteToWideChar(
                    CP_UTF8, 0, reinterpret_cast<const char*>(bytes.data()),
                    static_cast<int>(bytes.size()), nullptr, 0);
                std::wstring text;
                if (wlen > 0) {
                    text.resize(static_cast<size_t>(wlen));
                    MultiByteToWideChar(
                        CP_UTF8, 0, reinterpret_cast<const char*>(bytes.data()),
                        static_cast<int>(bytes.size()), text.data(), wlen);
                }
                size_t pos = 0;
                while (pos < text.size() && g_ql.lines.size() < 60) {
                    size_t nl = text.find(L'\n', pos);
                    if (nl == std::wstring::npos) nl = text.size();
                    std::wstring ln = text.substr(pos, nl - pos);
                    if (!ln.empty() && ln.back() == L'\r') ln.pop_back();
                    g_ql.lines.push_back(std::move(ln));
                    pos = nl + 1;
                }
                g_ql.loaded = true;
            } else {
                g_ql.failed = true;
            }
        }
    }
}


// ============================================================================
// 6. Spotlight UI State, Vector Glyphs, Optical Glass Tree & D2D Renderer
// ============================================================================

struct QuickActionItem {
    std::wstring label;
    std::wstring shortcut;
    bool destructive = false;
    int actionCode = 0;  // interpreted by ExecuteQuickAction
};

enum QuickActionCode : int {
    QA_Open = 1,
    QA_RunAdmin = 2,
    QA_PinToDock = 3,
    QA_TogglePinSpot = 4,
    QA_RevealExplorer = 5,
    QA_CopyPath = 6,
    QA_PasteNow = 7,
    QA_CopyClip = 8,
    QA_PinClip = 9,
    QA_DeleteClip = 10,
    QA_SwitchWindow = 11,
    QA_MinRestoreWindow = 12,
    QA_MaximizeWindow = 13,
    QA_CloseWindow = 14,
    QA_KillProcess = 15,
    QA_OpenUrl = 16,
    QA_CopyUrl = 17,
};

struct SpotlightUiState {
    HWND hwnd = nullptr;
    HWND prevForeground = nullptr;
    HMONITOR monitor = nullptr;
    RECT workAreaPx{};
    RECT windowPx{};
    float scale = 1.0f;

    bool visible = false;
    bool expanded = false;
    bool helpOpen = false;
    bool darkMode = true;

    SpotlightMode mode = SpotlightMode::All;
    SpotlightMode detectedPrefixMode = SpotlightMode::All;
    FileSubTab fileTab = FileSubTab::All;

    std::wstring query;
    std::wstring effectiveQuery;
    std::wstring prefixChipLabel;
    int caret = 0;
    int selAnchor = 0;
    bool caretOn = true;

    bool imeComposing = false;
    std::wstring imeComp;
    int imeCompCursor = 0;

    std::wstring autoFillGhost;
    std::wstring autoFillFull;
    std::wstring autoFillBadge;

    std::vector<SearchResult> results;
    std::vector<QuickActionItem> qlActions;
    Calc::Result calc;
    bool hasCalc = false;
    bool calcSelected = false;

    int selected = 0;
    int hoveredRow = -1;
    int hoveredOrb = -1;
    int hoveredTab = -1;
    bool hoverChipClose = false;
    bool hoverQuickLookBtn = false;

    float scrollY = 0.0f;
    float targetScrollY = 0.0f;
    float expandProg = 0.0f;
    float expandVel = 0.0f;
    float openAlpha = 0.0f;
    float openAlphaTarget = 0.0f;
    float openScale = 0.94f;
    float selYCur = 0.0f;
    float selYVel = 0.0f;
    bool selYInit = false;
    float orbHoverT[4]{};

    std::wstring toastMsg;
    ULONGLONG toastUntilMs = 0;
    std::wstring armedActionKey;
    ULONGLONG armedUntilMs = 0;

    // Logical DIP rects relative to g_spot.hwnd client area
    float padDip = kShadowMarginDip;
    D2D1_RECT_F pillRect{};
    D2D1_RECT_F orbRects[4]{};
    D2D1_RECT_F sheetRect{};
    D2D1_RECT_F targetSheetRect{};
    D2D1_RECT_F queryTextRect{};
    D2D1_RECT_F chipCloseRect{};
    D2D1_RECT_F quickLookToggleRect{};
    D2D1_RECT_F calcCardRect{};
    D2D1_RECT_F listViewportRect{};
    D2D1_RECT_F qlPaneRect{};

    struct FilterTabHit {
        D2D1_RECT_F rect{};
        std::wstring label;
        SpotlightMode mode = SpotlightMode::All;
        FileSubTab fileSub = FileSubTab::All;
        bool active = false;
    };
    std::vector<FilterTabHit> filterTabs;

    // WinRT Composition visual tree
    wuc::Desktop::DesktopWindowTarget compTarget{nullptr};
    wuc::ContainerVisual rootVisual{nullptr};
    wuc::SpriteVisual shadowVisual{nullptr};
    wuc::CompositionDrawingSurface shadowSurface{nullptr};
    wuc::CompositionSurfaceBrush shadowBrush{nullptr};
    SIZE shadowSurfSize{};
    std::array<float, 6> lastShadowKey{};

    LiquidGlassPlate pillPlate;
    LiquidGlassPlate orbPlates[4];
    LiquidGlassPlate sheetPlate;

    wuc::SpriteVisual uiVisual{nullptr};
    wuc::CompositionDrawingSurface uiSurface{nullptr};
    wuc::CompositionSurfaceBrush uiBrush{nullptr};
    SIZE uiSurfSize{};
    bool needsRedraw = true;
    bool animTimerActive = false;
    bool isOpening = false;
    bool evAliveCached = false;
    ULONGLONG evAliveCheckedMs = 0;
};

SpotlightUiState g_spot;

HWND GetSpotlightNotifyHwnd() { return g_hostHwnd; }
HWND GetSpotlightUiHwnd() { return g_spot.hwnd; }

void EnsureAnimTimerRunning() {
    if (g_spot.hwnd && g_spot.visible && !g_spot.animTimerActive) {
        SetTimer(g_spot.hwnd, kTimerFrame, 16, nullptr);
        g_spot.animTimerActive = true;
    }
}

void ApplyCaptureExclusion(bool excluded) {
    if (!g_spot.hwnd || !IsWindow(g_spot.hwnd)) return;
    DWORD want = excluded ? kWdaExcludeFromCapture : WDA_NONE;
    DWORD cur = WDA_NONE;
    if (!GetWindowDisplayAffinity(g_spot.hwnd, &cur) || cur != want) {
        SetWindowDisplayAffinity(g_spot.hwnd, want);
    }
}

void OnGraphicsDeviceLost() {
    ShutdownLiquidCapture();
    ClearSpotlightFonts();
    g_spot.pillPlate.ResetGpuResources();
    for (auto& op : g_spot.orbPlates) op.ResetGpuResources();
    g_spot.sheetPlate.ResetGpuResources();
    g_spot.shadowSurface = nullptr;
    g_spot.shadowSurfSize = {};
    g_spot.lastShadowKey = {};
    if (g_spot.shadowBrush) g_spot.shadowBrush.Surface(nullptr);
    g_spot.uiSurface = nullptr;
    g_spot.uiSurfSize = {};
    if (g_spot.uiBrush) g_spot.uiBrush.Surface(nullptr);
    for (auto& a : g_apps) {
        a.icon.d2d = nullptr;
        a.icon.boundDeviceGen = 0;
    }
    for (auto& kv : g_thumbCache) {
        kv.second.d2d = nullptr;
        kv.second.boundDeviceGen = 0;
    }
    g_ql.previewBmp.d2d = nullptr;
    g_ql.previewBmp.boundDeviceGen = 0;
    g_spot.needsRedraw = true;
}

void ShowToast(const std::wstring& msg) {
    g_spot.toastMsg = msg;
    g_spot.toastUntilMs = GetTickCount64() + 2100;
    if (g_spot.hwnd && g_spot.visible) {
        SetTimer(g_spot.hwnd, kTimerToastHide, 2200, nullptr);
        g_spot.needsRedraw = true;
    }
}

SpotlightMode EffectiveMode() {
    if (g_spot.mode == SpotlightMode::All &&
        g_spot.detectedPrefixMode != SpotlightMode::All) {
        return g_spot.detectedPrefixMode;
    }
    return g_spot.mode;
}

std::wstring ModeDisplayName(SpotlightMode m) {
    switch (m) {
        case SpotlightMode::All: return L"All";
        case SpotlightMode::Apps: return L"Apps";
        case SpotlightMode::Files: return L"Files";
        case SpotlightMode::Actions: return L"Actions";
        case SpotlightMode::Clipboard: return L"Clipboard";
        case SpotlightMode::Windows: return L"Windows";
        case SpotlightMode::KillProcess: return L"Kill Process";
        case SpotlightMode::Bookmarks: return L"Bookmarks";
        case SpotlightMode::Settings: return L"Settings";
    }
    return L"All";
}

std::vector<QuickActionItem> BuildQuickActionsForResult(const SearchResult* r) {
    std::vector<QuickActionItem> list;
    if (!r) return list;
    switch (r->kind) {
        case ResultKind::App: {
            bool isPinned =
                (r->appIdx >= 0 && r->appIdx < static_cast<int>(g_apps.size()) &&
                 g_apps[static_cast<size_t>(r->appIdx)].pinned);
            list.push_back({L"Open App", L"\u21B5", false, QA_Open});
            list.push_back(
                {L"Run as Administrator", L"Ctrl+\u21B5", false, QA_RunAdmin});
            list.push_back({L"Pin to Mac Dock", L"", false, QA_PinToDock});
            list.push_back({isPinned ? L"Unpin from Spotlight" : L"Pin in Spotlight",
                            L"Ctrl+P", false, QA_TogglePinSpot});
            if (!r->payload.empty() && r->payload.find(L'!') == std::wstring::npos) {
                list.push_back(
                    {L"Open File Location", L"Alt+\u21B5", false, QA_RevealExplorer});
                list.push_back({L"Copy Path", L"Ctrl+Shift+C", false, QA_CopyPath});
            }
            break;
        }
        case ResultKind::File:
        case ResultKind::RecentFile:
            list.push_back({r->isDir ? L"Open Folder" : L"Open File", L"\u21B5",
                            false, QA_Open});
            if (!r->isDir) {
                list.push_back({L"Open File Location", L"Alt+\u21B5", false,
                                QA_RevealExplorer});
            }
            list.push_back({L"Copy Full Path", L"Ctrl+Shift+C", false, QA_CopyPath});
            if (r->isDir || EndsWithNoCase(r->payload, L".exe") ||
                EndsWithNoCase(r->payload, L".lnk")) {
                list.push_back({L"Pin to Mac Dock", L"", false, QA_PinToDock});
            }
            break;
        case ResultKind::Clipboard:
            list.push_back({L"Paste Now", L"\u21B5", false, QA_PasteNow});
            list.push_back({L"Copy to Clipboard", L"Ctrl+C", false, QA_CopyClip});
            list.push_back({r->pinned ? L"Unpin Clipboard Item" : L"Pin Clipboard Item",
                            L"Ctrl+P", false, QA_PinClip});
            list.push_back({L"Delete Entry", L"Del", true, QA_DeleteClip});
            break;
        case ResultKind::Window:
            list.push_back({L"Switch to Window", L"\u21B5", false, QA_SwitchWindow});
            list.push_back(
                {L"Minimize / Restore", L"", false, QA_MinRestoreWindow});
            list.push_back({L"Maximize Window", L"", false, QA_MaximizeWindow});
            list.push_back({L"Close Window", L"Del", true, QA_CloseWindow});
            break;
        case ResultKind::Process:
            list.push_back(
                {L"Terminate Process Group", L"\u21B5", true, QA_KillProcess});
            if (!r->payload.empty()) {
                list.push_back({L"Open Executable Folder", L"Alt+\u21B5", false,
                                QA_RevealExplorer});
                list.push_back(
                    {L"Copy Executable Path", L"Ctrl+C", false, QA_CopyPath});
            }
            break;
        case ResultKind::Bookmark:
        case ResultKind::Url:
            list.push_back({L"Open in Browser", L"\u21B5", false, QA_OpenUrl});
            list.push_back({L"Copy URL", L"Ctrl+C", false, QA_CopyUrl});
            break;
        case ResultKind::Action:
        case ResultKind::Setting:
        case ResultKind::Web:
            list.push_back({L"Run Command", L"\u21B5", false, QA_Open});
            break;
    }
    return list;
}

void SyncQuickLookToSelection() {
    if (!g_ql.open) return;
    if (g_spot.hasCalc && g_spot.calcSelected) {
        LoadQuickLookForResult(nullptr);
        g_spot.qlActions.clear();
        return;
    }
    const SearchResult* sel =
        (g_spot.selected >= 0 &&
         g_spot.selected < static_cast<int>(g_spot.results.size()))
            ? &g_spot.results[static_cast<size_t>(g_spot.selected)]
            : nullptr;
    LoadQuickLookForResult(sel);
    g_spot.qlActions = BuildQuickActionsForResult(sel);
}

bool ConfirmDestructiveActionIfNeeded(const std::wstring& key,
                                      const std::wstring& prompt) {
    ULONGLONG now = GetTickCount64();
    if (g_spot.armedActionKey == key && now < g_spot.armedUntilMs) {
        g_spot.armedActionKey.clear();
        g_spot.armedUntilMs = 0;
        return true;
    }
    g_spot.armedActionKey = key;
    g_spot.armedUntilMs = now + 3000;
    if (g_spot.hwnd) SetTimer(g_spot.hwnd, kTimerConfirmReset, 3100, nullptr);
    ShowToast(prompt + L" (press \u21B5 again to confirm)");
    return false;
}

void ExecuteSearchResultItem(const SearchResult& r,
                             bool elevated,
                             bool revealOnly,
                             bool copyOnly);

void ExecuteQuickAction(const SearchResult& r, int actionCode) {
    switch (actionCode) {
        case QA_Open:
            ExecuteSearchResultItem(r, false, false, false);
            break;
        case QA_RunAdmin:
            ExecuteSearchResultItem(r, true, false, false);
            break;
        case QA_RevealExplorer:
            ExecuteSearchResultItem(r, false, true, false);
            break;
        case QA_CopyPath:
        case QA_CopyUrl:
            if (!r.payload.empty()) {
                CopyTextToClipboard(r.payload);
                ShowToast(L"Copied to clipboard");
            }
            break;
        case QA_PinToDock: {
            std::wstring path = r.payload;
            std::wstring aumid;
            if (r.appIdx >= 0 && r.appIdx < static_cast<int>(g_apps.size())) {
                const auto& app = g_apps[static_cast<size_t>(r.appIdx)];
                path = !app.target.empty() ? app.target : app.launch;
                if (IsPackagedId(app.id)) aumid = app.id;
            }
            if (PinEntryToMacDock(path, aumid, r.title)) {
                ShowToast(L"Pinned \u201C" + r.title + L"\u201D to Mac Dock");
            } else {
                ShowToast(L"Mac Dock not active");
            }
            break;
        }
        case QA_TogglePinSpot:
            if (r.appIdx >= 0 && r.appIdx < static_cast<int>(g_apps.size())) {
                auto& app = g_apps[static_cast<size_t>(r.appIdx)];
                app.pinned = !app.pinned;
                std::wstring key = FoldString(app.usageKey.empty() ? app.id
                                                                   : app.usageKey);
                if (app.pinned) g_pinnedAppIds.insert(key);
                else g_pinnedAppIds.erase(key);
                SavePinnedApps();
                ShowToast(app.pinned ? L"Pinned in Spotlight"
                                     : L"Unpinned from Spotlight");
                TriggerSpotlightRebuild(false);
            }
            break;
        case QA_PasteNow:
            ExecuteSearchResultItem(r, false, false, false);
            break;
        case QA_CopyClip:
            if (r.clipIdx >= 0 && r.clipIdx < static_cast<int>(g_clips.size())) {
                const auto& c = g_clips[static_cast<size_t>(r.clipIdx)];
                if (c.image) PutClipImageOnClipboard(c.text);
                else CopyTextToClipboard(c.text);
                ShowToast(L"Copied back to clipboard");
            }
            break;
        case QA_PinClip:
            if (r.clipIdx >= 0 && r.clipIdx < static_cast<int>(g_clips.size())) {
                g_clips[static_cast<size_t>(r.clipIdx)].pinned =
                    !g_clips[static_cast<size_t>(r.clipIdx)].pinned;
                SaveClipHistory();
                TriggerSpotlightRebuild(false);
            }
            break;
        case QA_DeleteClip:
            if (r.clipIdx >= 0 && r.clipIdx < static_cast<int>(g_clips.size())) {
                if (g_clips[static_cast<size_t>(r.clipIdx)].image) {
                    DeleteClipImageFile(
                        g_clips[static_cast<size_t>(r.clipIdx)].text);
                }
                g_clips.erase(g_clips.begin() +
                              static_cast<std::ptrdiff_t>(r.clipIdx));
                SaveClipHistory();
                TriggerSpotlightRebuild(false);
            }
            break;
        case QA_SwitchWindow:
            if (r.windowIdx >= 0 &&
                r.windowIdx < static_cast<int>(g_windows.size())) {
                HWND target = g_windows[static_cast<size_t>(r.windowIdx)].hwnd;
                HideSpotlight(false);
                FocusOrSwitchWindow(target);
            }
            break;
        case QA_MinRestoreWindow:
            if (r.windowIdx >= 0 &&
                r.windowIdx < static_cast<int>(g_windows.size())) {
                HWND target = g_windows[static_cast<size_t>(r.windowIdx)].hwnd;
                if (IsWindow(target)) {
                    ShowWindowAsync(target,
                                    IsIconic(target) ? SW_RESTORE : SW_MINIMIZE);
                    RefreshWindowsSnapshot(g_spot.hwnd);
                    TriggerSpotlightRebuild(false);
                }
            }
            break;
        case QA_MaximizeWindow:
            if (r.windowIdx >= 0 &&
                r.windowIdx < static_cast<int>(g_windows.size())) {
                HWND target = g_windows[static_cast<size_t>(r.windowIdx)].hwnd;
                if (IsWindow(target)) {
                    ShowWindowAsync(target,
                                    IsZoomed(target) ? SW_RESTORE : SW_MAXIMIZE);
                    RefreshWindowsSnapshot(g_spot.hwnd);
                    TriggerSpotlightRebuild(false);
                }
            }
            break;
        case QA_CloseWindow:
            if (r.windowIdx >= 0 &&
                r.windowIdx < static_cast<int>(g_windows.size())) {
                HWND target = g_windows[static_cast<size_t>(r.windowIdx)].hwnd;
                if (IsWindow(target)) {
                    PostMessageW(target, WM_CLOSE, 0, 0);
                    ShowToast(L"Sent close to window");
                    RefreshWindowsSnapshot(g_spot.hwnd);
                    TriggerSpotlightRebuild(false);
                }
            }
            break;
        case QA_KillProcess:
            ExecuteSearchResultItem(r, false, false, false);
            break;
    }
}

void ExecuteSearchResultItem(const SearchResult& r,
                             bool elevated,
                             bool revealOnly,
                             bool copyOnly) {
    if (copyOnly) {
        if (!r.payload.empty()) {
            CopyTextToClipboard(r.payload);
            ShowToast(L"Copied to clipboard");
        }
        return;
    }
    if (revealOnly && !r.payload.empty()) {
        std::wstring path = r.payload;
        if (r.kind == ResultKind::App && r.appIdx >= 0 &&
            r.appIdx < static_cast<int>(g_apps.size()) &&
            !g_apps[static_cast<size_t>(r.appIdx)].target.empty()) {
            path = g_apps[static_cast<size_t>(r.appIdx)].target;
        }
        HideSpotlight(false);
        RevealInFileExplorer(path);
        return;
    }

    switch (r.kind) {
        case ResultKind::App: {
            if (r.appIdx < 0 || r.appIdx >= static_cast<int>(g_apps.size())) {
                break;
            }
            auto& app = g_apps[static_cast<size_t>(r.appIdx)];
            std::wstring qf = FoldString(g_spot.effectiveQuery);
            std::wstring id = app.id;
            std::wstring launch = app.launch;
            std::wstring target = app.target;
            std::wstring args = app.args;
            RecordAppPick(app, qf);
            HideSpotlight(false);
            if (!elevated && IsPackagedId(id)) {
                ActivatePackagedAumid(id);
            } else {
                ShellLaunchTarget(!launch.empty() ? launch : target, args,
                                  elevated);
            }
            break;
        }
        case ResultKind::Action: {
            if (r.actionIdx < 0 || r.actionIdx >= kBuiltinActionCount) break;
            const auto& act = kBuiltinActions[r.actionIdx];
            if (act.switchMode != SpotlightMode::All) {
                g_spot.mode = act.switchMode;
                g_spot.expanded = true;
                g_spot.query.clear();
                g_spot.caret = 0;
                g_spot.selAnchor = 0;
                g_spot.selected = 0;
                TriggerSpotlightRebuild(act.switchMode == SpotlightMode::Files);
                return;
            }
            if (act.destructive &&
                !ConfirmDestructiveActionIfNeeded(
                    std::wstring(L"act:") + act.id, act.title)) {
                return;
            }
            bool keepOpen = ExecuteBuiltinAction(act);
            if (!keepOpen) HideSpotlight(false);
            break;
        }
        case ResultKind::Setting: {
            HideSpotlight(false);
            ShellLaunchTarget(r.payload, L"", false);
            break;
        }
        case ResultKind::File:
        case ResultKind::RecentFile: {
            std::wstring path = r.payload;
            HideSpotlight(false);
            ShellLaunchTarget(path, L"", elevated);
            break;
        }
        case ResultKind::Clipboard: {
            if (r.clipIdx < 0 || r.clipIdx >= static_cast<int>(g_clips.size())) {
                break;
            }
            const auto& c = g_clips[static_cast<size_t>(r.clipIdx)];
            if (c.image) PutClipImageOnClipboard(c.text);
            else CopyTextToClipboard(c.text);
            HideSpotlight(true);
            SendGlobalPasteShortcut();
            break;
        }
        case ResultKind::Window: {
            if (r.windowIdx < 0 ||
                r.windowIdx >= static_cast<int>(g_windows.size())) {
                break;
            }
            HWND target = g_windows[static_cast<size_t>(r.windowIdx)].hwnd;
            HideSpotlight(false);
            FocusOrSwitchWindow(target);
            break;
        }
        case ResultKind::Process: {
            if (r.procIdx < 0 ||
                r.procIdx >= static_cast<int>(g_processes.size())) {
                break;
            }
            const auto& pg = g_processes[static_cast<size_t>(r.procIdx)];
            if (!ConfirmDestructiveActionIfNeeded(
                    L"kill:" + pg.folded, L"Terminate " + pg.exeName)) {
                return;
            }
            int killed = 0;
            for (DWORD pid : pg.pids) {
                if (HANDLE hp = OpenProcess(PROCESS_TERMINATE, FALSE, pid)) {
                    if (TerminateProcess(hp, 1)) ++killed;
                    CloseHandle(hp);
                }
            }
            ShowToast(L"Terminated " + std::to_wstring(killed) + L" \u00D7 " +
                      pg.exeName);
            RefreshProcessesSnapshot();
            TriggerSpotlightRebuild(false);
            break;
        }
        case ResultKind::Bookmark:
        case ResultKind::Url:
            HideSpotlight(false);
            OpenUrlInDefaultBrowser(r.payload);
            break;
        case ResultKind::Web:
            HideSpotlight(false);
            OpenUrlInDefaultBrowser(BuildWebSearchUrl(r.payload));
            break;
    }
}

void ExecutePrimaryAction(bool elevated, bool revealOnly, bool copyOnly) {
    if (g_spot.hasCalc && g_spot.calcSelected && !revealOnly) {
        std::wstring copyVal = g_spot.calc.insertable.empty()
                                   ? g_spot.calc.display
                                   : g_spot.calc.insertable;
        CopyTextToClipboard(copyVal);
        ShowToast(L"Copied \u201C" + copyVal + L"\u201D");
        if (!copyOnly) HideSpotlight(false);
        return;
    }
    if (g_spot.selected >= 0 &&
        g_spot.selected < static_cast<int>(g_spot.results.size())) {
        SearchResult copy = g_spot.results[static_cast<size_t>(g_spot.selected)];
        ExecuteSearchResultItem(copy, elevated, revealOnly, copyOnly);
    }
}

// Vector D2D Glyphs (Mac-style icons + Screenshot 1 Orbs)
void DrawOrbGlyph(ID2D1DeviceContext* dc,
                  int orbIdx,
                  D2D1_RECT_F rc,
                  D2D1_COLOR_F color) {
    winrt::com_ptr<ID2D1SolidColorBrush> br;
    if (FAILED(dc->CreateSolidColorBrush(color, br.put())) || !br) return;

    float w = rc.right - rc.left;
    float h = rc.bottom - rc.top;
    float cx = (rc.left + rc.right) * 0.5f;
    float cy = (rc.top + rc.bottom) * 0.5f;
    float u = std::min(w, h) * 0.42f;
    float sw = std::max(1.5f, u * 0.145f);

    winrt::com_ptr<ID2D1StrokeStyle> roundCap;
    if (g_gfx.d2dFactory) {
        D2D1_STROKE_STYLE_PROPERTIES sp{};
        sp.startCap = D2D1_CAP_STYLE_ROUND;
        sp.endCap = D2D1_CAP_STYLE_ROUND;
        sp.lineJoin = D2D1_LINE_JOIN_ROUND;
        g_gfx.d2dFactory->CreateStrokeStyle(&sp, nullptr, 0, roundCap.put());
    }

    switch (orbIdx) {
        case 0: {
            // App Store "A" (three crossing strokes like Screenshot 1)
            dc->DrawLine({cx, cy - u * 0.84f}, {cx - u * 0.68f, cy + u * 0.78f},
                         br.get(), sw, roundCap.get());
            dc->DrawLine({cx - u * 0.08f, cy - u * 0.68f},
                         {cx + u * 0.70f, cy + u * 0.78f}, br.get(), sw,
                         roundCap.get());
            dc->DrawLine({cx - u * 0.84f, cy + u * 0.34f},
                         {cx + u * 0.84f, cy + u * 0.34f}, br.get(), sw,
                         roundCap.get());
            break;
        }
        case 1: {
            // Folder outline (Screenshot 1 Orb 2)
            winrt::com_ptr<ID2D1PathGeometry> g;
            winrt::com_ptr<ID2D1GeometrySink> sk;
            if (g_gfx.d2dFactory &&
                SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(g.put())) &&
                SUCCEEDED(g->Open(sk.put()))) {
                float x0 = cx - u * 0.90f, x1 = cx + u * 0.90f;
                float y0 = cy - u * 0.64f, y1 = cy + u * 0.68f;
                float r = u * 0.22f;
                sk->BeginFigure({x0 + r, y0}, D2D1_FIGURE_BEGIN_HOLLOW);
                sk->AddLine({cx - u * 0.20f, y0});
                sk->AddLine({cx, y0 + u * 0.22f});
                sk->AddLine({x1 - r, y0 + u * 0.22f});
                sk->AddArc(D2D1::ArcSegment(
                    {x1, y0 + u * 0.22f + r}, {r, r}, 0,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                sk->AddLine({x1, y1 - r});
                sk->AddArc(D2D1::ArcSegment(
                    {x1 - r, y1}, {r, r}, 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL));
                sk->AddLine({x0 + r, y1});
                sk->AddArc(D2D1::ArcSegment(
                    {x0, y1 - r}, {r, r}, 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL));
                sk->AddLine({x0, y0 + r});
                sk->AddArc(D2D1::ArcSegment(
                    {x0 + r, y0}, {r, r}, 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL));
                sk->EndFigure(D2D1_FIGURE_END_CLOSED);
                sk->Close();
                dc->DrawGeometry(g.get(), br.get(), sw, roundCap.get());
            }
            break;
        }
        case 2: {
            // Actions / Shortcuts stacked diamonds (Screenshot 1 Orb 3)
            auto drawDiamond = [&](float dy) {
                winrt::com_ptr<ID2D1PathGeometry> g;
                winrt::com_ptr<ID2D1GeometrySink> sk;
                if (!g_gfx.d2dFactory ||
                    FAILED(g_gfx.d2dFactory->CreatePathGeometry(g.put())) ||
                    FAILED(g->Open(sk.put()))) {
                    return;
                }
                float dx = u * 0.86f;
                float dv = u * 0.48f;
                float cy2 = cy + dy;
                sk->BeginFigure({cx, cy2 - dv}, D2D1_FIGURE_BEGIN_HOLLOW);
                sk->AddLine({cx + dx, cy2});
                sk->AddLine({cx, cy2 + dv});
                sk->AddLine({cx - dx, cy2});
                sk->EndFigure(D2D1_FIGURE_END_CLOSED);
                sk->Close();
                dc->DrawGeometry(g.get(), br.get(), sw, roundCap.get());
            };
            drawDiamond(-u * 0.34f);
            drawDiamond(+u * 0.34f);
            break;
        }
        case 3:
        default: {
            // Clipboard overlapping document pages (Screenshot 1 Orb 4)
            D2D1_ROUNDED_RECT back{
                D2D1::RectF(cx - u * 0.66f, cy - u * 0.78f, cx + u * 0.20f,
                            cy + u * 0.42f),
                u * 0.16f, u * 0.16f};
            D2D1_ROUNDED_RECT front{
                D2D1::RectF(cx - u * 0.16f, cy - u * 0.38f, cx + u * 0.70f,
                            cy + u * 0.82f),
                u * 0.16f, u * 0.16f};
            dc->DrawRoundedRectangle(back, br.get(), sw * 0.92f);
            winrt::com_ptr<ID2D1SolidColorBrush> clearBr;
            D2D1_COLOR_F cutTint{color.r, color.g, color.b, 0.14f};
            if (SUCCEEDED(dc->CreateSolidColorBrush(cutTint, clearBr.put()))) {
                dc->FillRoundedRectangle(front, clearBr.get());
            }
            dc->DrawRoundedRectangle(front, br.get(), sw * 0.95f);
            break;
        }
    }
}

void DrawMagnifierGlyph(ID2D1DeviceContext* dc,
                        float cx,
                        float cy,
                        float r,
                        D2D1_COLOR_F color,
                        float strokeW) {
    winrt::com_ptr<ID2D1SolidColorBrush> br;
    if (FAILED(dc->CreateSolidColorBrush(color, br.put())) || !br) return;
    winrt::com_ptr<ID2D1StrokeStyle> cap;
    if (g_gfx.d2dFactory) {
        D2D1_STROKE_STYLE_PROPERTIES sp{};
        sp.startCap = D2D1_CAP_STYLE_ROUND;
        sp.endCap = D2D1_CAP_STYLE_ROUND;
        g_gfx.d2dFactory->CreateStrokeStyle(&sp, nullptr, 0, cap.put());
    }
    float cr = r * 0.62f;
    float ccx = cx - r * 0.18f;
    float ccy = cy - r * 0.18f;
    dc->DrawEllipse(D2D1::Ellipse({ccx, ccy}, cr, cr), br.get(), strokeW);
    dc->DrawLine({ccx + cr * 0.72f, ccy + cr * 0.72f},
                 {cx + r * 0.92f, cy + r * 0.92f}, br.get(), strokeW,
                 cap.get());
}

void DrawActionBadgeAndGlyph(ID2D1DeviceContext* dc,
                             D2D1_RECT_F rc,
                             ActionGlyph glyph,
                             D2D1_COLOR_F bgColor) {
    float w = rc.right - rc.left;
    float h = rc.bottom - rc.top;
    float rad = std::min(w, h) * 0.26f;
    D2D1_ROUNDED_RECT rr{rc, rad, rad};
    winrt::com_ptr<ID2D1SolidColorBrush> bg, fg;
    if (SUCCEEDED(dc->CreateSolidColorBrush(bgColor, bg.put()))) {
        dc->FillRoundedRectangle(rr, bg.get());
    }
    // Subtle top glossy highlight inside colored icon badge
    winrt::com_ptr<ID2D1SolidColorBrush> rim;
    if (SUCCEEDED(dc->CreateSolidColorBrush(
            D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f), rim.put()))) {
        dc->DrawRoundedRectangle(rr, rim.get(), 1.0f);
    }
    if (FAILED(dc->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f),
                                         fg.put()))) {
        return;
    }

    float cx = (rc.left + rc.right) * 0.5f;
    float cy = (rc.top + rc.bottom) * 0.5f;
    float u = std::min(w, h) * 0.27f;
    float sw = std::max(1.5f, u * 0.22f);

    winrt::com_ptr<ID2D1StrokeStyle> cap;
    if (g_gfx.d2dFactory) {
        D2D1_STROKE_STYLE_PROPERTIES sp{};
        sp.startCap = D2D1_CAP_STYLE_ROUND;
        sp.endCap = D2D1_CAP_STYLE_ROUND;
        sp.lineJoin = D2D1_LINE_JOIN_ROUND;
        g_gfx.d2dFactory->CreateStrokeStyle(&sp, nullptr, 0, cap.put());
    }

    switch (glyph) {
        case ActionGlyph::Search:
            DrawMagnifierGlyph(dc, cx, cy, u * 0.95f,
                               D2D1::ColorF(1, 1, 1, 0.96f), sw);
            break;
        case ActionGlyph::Folder: {
            D2D1_ROUNDED_RECT f{
                D2D1::RectF(cx - u, cy - u * 0.65f, cx + u, cy + u * 0.72f),
                u * 0.22f, u * 0.22f};
            dc->DrawRoundedRectangle(f, fg.get(), sw);
            dc->DrawLine({cx - u * 0.95f, cy - u * 0.30f},
                         {cx + u * 0.10f, cy - u * 0.30f}, fg.get(), sw,
                         cap.get());
            break;
        }
        case ActionGlyph::Doc: {
            D2D1_ROUNDED_RECT d{
                D2D1::RectF(cx - u * 0.68f, cy - u * 0.86f, cx + u * 0.68f,
                            cy + u * 0.86f),
                u * 0.18f, u * 0.18f};
            dc->DrawRoundedRectangle(d, fg.get(), sw);
            dc->DrawLine({cx - u * 0.35f, cy - u * 0.25f},
                         {cx + u * 0.35f, cy - u * 0.25f}, fg.get(), sw * 0.8f,
                         cap.get());
            dc->DrawLine({cx - u * 0.35f, cy + u * 0.20f},
                         {cx + u * 0.28f, cy + u * 0.20f}, fg.get(), sw * 0.8f,
                         cap.get());
            break;
        }
        case ActionGlyph::Terminal:
            dc->DrawLine({cx - u * 0.65f, cy - u * 0.45f},
                         {cx - u * 0.10f, cy}, fg.get(), sw, cap.get());
            dc->DrawLine({cx - u * 0.10f, cy},
                         {cx - u * 0.65f, cy + u * 0.45f}, fg.get(), sw,
                         cap.get());
            dc->DrawLine({cx + u * 0.08f, cy + u * 0.45f},
                         {cx + u * 0.72f, cy + u * 0.45f}, fg.get(), sw,
                         cap.get());
            break;
        case ActionGlyph::Clipboard:
            DrawOrbGlyph(dc, 3,
                         D2D1::RectF(cx - u * 1.15f, cy - u * 1.15f,
                                     cx + u * 1.15f, cy + u * 1.15f),
                         D2D1::ColorF(1, 1, 1, 0.96f));
            break;
        case ActionGlyph::Monitor:
            dc->DrawRoundedRectangle(
                D2D1::RoundedRect(
                    D2D1::RectF(cx - u, cy - u * 0.70f, cx + u, cy + u * 0.45f),
                    u * 0.16f, u * 0.16f),
                fg.get(), sw);
            dc->DrawLine({cx - u * 0.45f, cy + u * 0.78f},
                         {cx + u * 0.45f, cy + u * 0.78f}, fg.get(), sw,
                         cap.get());
            break;
        case ActionGlyph::Skull:
            dc->DrawEllipse(D2D1::Ellipse({cx, cy - u * 0.12f}, u * 0.74f,
                                          u * 0.68f),
                            fg.get(), sw);
            dc->FillEllipse(D2D1::Ellipse({cx - u * 0.26f, cy - u * 0.12f},
                                          u * 0.16f, u * 0.16f),
                            fg.get());
            dc->FillEllipse(D2D1::Ellipse({cx + u * 0.26f, cy - u * 0.12f},
                                          u * 0.16f, u * 0.16f),
                            fg.get());
            dc->DrawLine({cx - u * 0.25f, cy + u * 0.75f},
                         {cx + u * 0.25f, cy + u * 0.75f}, fg.get(), sw,
                         cap.get());
            break;
        case ActionGlyph::Star: {
            winrt::com_ptr<ID2D1PathGeometry> g;
            winrt::com_ptr<ID2D1GeometrySink> sk;
            if (g_gfx.d2dFactory &&
                SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(g.put())) &&
                SUCCEEDED(g->Open(sk.put()))) {
                for (int i = 0; i < 10; ++i) {
                    float a = -1.5707963f + i * 0.62831853f;
                    float starRad = (i % 2 == 0) ? u * 0.95f : u * 0.42f;
                    D2D1_POINT_2F pt{cx + std::cos(a) * starRad,
                                     cy + std::sin(a) * starRad};
                    if (i == 0) sk->BeginFigure(pt, D2D1_FIGURE_BEGIN_FILLED);
                    else sk->AddLine(pt);
                }
                sk->EndFigure(D2D1_FIGURE_END_CLOSED);
                sk->Close();
                dc->FillGeometry(g.get(), fg.get());
            }
            break;
        }
        case ActionGlyph::Play: {
            winrt::com_ptr<ID2D1PathGeometry> g;
            winrt::com_ptr<ID2D1GeometrySink> sk;
            if (g_gfx.d2dFactory &&
                SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(g.put())) &&
                SUCCEEDED(g->Open(sk.put()))) {
                sk->BeginFigure({cx - u * 0.48f, cy - u * 0.72f},
                                D2D1_FIGURE_BEGIN_FILLED);
                sk->AddLine({cx + u * 0.72f, cy});
                sk->AddLine({cx - u * 0.48f, cy + u * 0.72f});
                sk->EndFigure(D2D1_FIGURE_END_CLOSED);
                sk->Close();
                dc->FillGeometry(g.get(), fg.get());
            }
            break;
        }
        case ActionGlyph::Power:
        case ActionGlyph::Restart:
        case ActionGlyph::Lock:
        case ActionGlyph::Gear:
        default:
            dc->DrawEllipse(D2D1::Ellipse({cx, cy}, u * 0.70f, u * 0.70f),
                            fg.get(), sw);
            dc->DrawLine({cx, cy - u * 0.92f}, {cx, cy}, fg.get(), sw,
                         cap.get());
            break;
    }
}

void DrawChipBadge(ID2D1DeviceContext* dc,
                   IDWriteTextFormat* fmt,
                   D2D1_RECT_F rc,
                   const std::wstring& text,
                   D2D1_COLOR_F fillCol,
                   D2D1_COLOR_F strokeCol,
                   D2D1_COLOR_F textCol,
                   float radius) {
    winrt::com_ptr<ID2D1SolidColorBrush> fillBr, strokeBr, txtBr;
    D2D1_ROUNDED_RECT rr{rc, radius, radius};
    if (SUCCEEDED(dc->CreateSolidColorBrush(fillCol, fillBr.put()))) {
        dc->FillRoundedRectangle(rr, fillBr.get());
    }
    if (strokeCol.a > 0.005f &&
        SUCCEEDED(dc->CreateSolidColorBrush(strokeCol, strokeBr.put()))) {
        dc->DrawRoundedRectangle(rr, strokeBr.get(), 1.0f);
    }
    if (fmt && !text.empty() &&
        SUCCEEDED(dc->CreateSolidColorBrush(textCol, txtBr.put()))) {
        dc->DrawText(text.c_str(), static_cast<UINT32>(text.size()), fmt, rc,
                     txtBr.get());
    }
}

winrt::com_ptr<IDWriteTextFormat> MakeTextFormat(float sizeDip,
                                                 DWRITE_FONT_WEIGHT weight,
                                                 DWRITE_TEXT_ALIGNMENT align,
                                                 bool mono = false) {
    if (!g_gfx.dwriteFactory) return nullptr;
    const std::wstring& fam =
        mono ? ResolveMonoFontFamily() : ResolveFontFamily();
    winrt::com_ptr<IDWriteTextFormat> fmt;
    if (FAILED(g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, sizeDip, L"en-us", fmt.put()))) {
        return nullptr;
    }
    fmt->SetTextAlignment(align);
    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    winrt::com_ptr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(g_gfx.dwriteFactory->CreateEllipsisTrimmingSign(
            fmt.get(), ellipsis.put()))) {
        fmt->SetTrimming(&trim, ellipsis.get());
    }
    return fmt;
}

struct SpotlightFonts {
    winrt::com_ptr<IDWriteTextFormat> fQuery;
    winrt::com_ptr<IDWriteTextFormat> fGhost;
    winrt::com_ptr<IDWriteTextFormat> fChip;
    winrt::com_ptr<IDWriteTextFormat> fTab;
    winrt::com_ptr<IDWriteTextFormat> fTitle;
    winrt::com_ptr<IDWriteTextFormat> fSub;
    winrt::com_ptr<IDWriteTextFormat> fBadge;
    winrt::com_ptr<IDWriteTextFormat> fCalcBig;
    winrt::com_ptr<IDWriteTextFormat> fMono;
    winrt::com_ptr<IDWriteTextFormat> fFooter;
    winrt::com_ptr<IDWriteTextFormat> fFooterR;
    winrt::com_ptr<IDWriteTextFormat> fEmpty;
    winrt::com_ptr<IDWriteTextFormat> fCtr;
    winrt::com_ptr<IDWriteTextFormat> fSubC;
    winrt::com_ptr<IDWriteTextFormat> fHdrChip;
};

SpotlightFonts g_fonts;

void ClearSpotlightFonts() { g_fonts = {}; }

void EnsureSpotlightFonts() {
    if (!g_gfx.dwriteFactory) return;
    if (!g_fonts.fQuery) {
        g_fonts.fQuery = MakeTextFormat(22.0f, DWRITE_FONT_WEIGHT_REGULAR,
                                        DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    if (!g_fonts.fGhost) {
        g_fonts.fGhost = MakeTextFormat(21.0f, DWRITE_FONT_WEIGHT_MEDIUM,
                                        DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    if (!g_fonts.fChip) {
        g_fonts.fChip = MakeTextFormat(12.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                       DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (!g_fonts.fTab) {
        g_fonts.fTab = MakeTextFormat(12.5f, DWRITE_FONT_WEIGHT_MEDIUM,
                                      DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (!g_fonts.fTitle) {
        g_fonts.fTitle = MakeTextFormat(14.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                        DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    if (!g_fonts.fSub) {
        g_fonts.fSub = MakeTextFormat(12.0f, DWRITE_FONT_WEIGHT_REGULAR,
                                      DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    if (!g_fonts.fBadge) {
        g_fonts.fBadge = MakeTextFormat(11.5f, DWRITE_FONT_WEIGHT_MEDIUM,
                                        DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (!g_fonts.fCalcBig) {
        g_fonts.fCalcBig = MakeTextFormat(30.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                          DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    if (!g_fonts.fMono) {
        g_fonts.fMono = MakeTextFormat(12.0f, DWRITE_FONT_WEIGHT_REGULAR,
                                       DWRITE_TEXT_ALIGNMENT_LEADING, true);
    }
    if (!g_fonts.fFooter) {
        g_fonts.fFooter = MakeTextFormat(11.0f, DWRITE_FONT_WEIGHT_MEDIUM,
                                         DWRITE_TEXT_ALIGNMENT_LEADING);
    }
    if (!g_fonts.fFooterR) {
        g_fonts.fFooterR = MakeTextFormat(11.0f, DWRITE_FONT_WEIGHT_MEDIUM,
                                          DWRITE_TEXT_ALIGNMENT_TRAILING);
    }
    if (!g_fonts.fEmpty) {
        g_fonts.fEmpty = MakeTextFormat(13.5f, DWRITE_FONT_WEIGHT_MEDIUM,
                                        DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (!g_fonts.fCtr) {
        g_fonts.fCtr = MakeTextFormat(13.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                      DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (!g_fonts.fSubC) {
        g_fonts.fSubC = MakeTextFormat(11.5f, DWRITE_FONT_WEIGHT_REGULAR,
                                       DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    if (!g_fonts.fHdrChip) {
        g_fonts.fHdrChip = MakeTextFormat(13.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                          DWRITE_TEXT_ALIGNMENT_CENTER);
    }
}

float MeasureTextWidthDip(const std::wstring& text,
                          IDWriteTextFormat* fmt,
                          float maxW = 1200.0f,
                          float h = 40.0f) {
    if (!g_gfx.dwriteFactory || !fmt || text.empty()) return 0.0f;
    winrt::com_ptr<IDWriteTextLayout> layout;
    if (FAILED(g_gfx.dwriteFactory->CreateTextLayout(
            text.c_str(), static_cast<UINT32>(text.size()), fmt, maxW, h,
            layout.put()))) {
        return 0.0f;
    }
    DWRITE_TEXT_METRICS m{};
    if (FAILED(layout->GetMetrics(&m))) return 0.0f;
    return m.widthIncludingTrailingWhitespace;
}

int VisibleResultListRows() {
    return std::min(7, static_cast<int>(g_spot.results.size()));
}

void UpdateRowHitRectsOnly() {
    float bodyY = g_spot.listViewportRect.top;
    float mainRight = g_spot.listViewportRect.right;
    for (int i = 0; i < static_cast<int>(g_spot.results.size()); ++i) {
        float ry = bodyY + static_cast<float>(i) * kRowH - g_spot.scrollY;
        g_spot.results[static_cast<size_t>(i)].hitRect = {
            g_spot.sheetRect.left + 10.0f, ry + 2.0f, mainRight - 10.0f,
            ry + kRowH - 2.0f};
    }
}

void ComputeSpotlightLayout() {
    EnsureSpotlightFonts();
    HMONITOR mon = g_spot.monitor ? g_spot.monitor : ResolveTargetMonitor();
    g_spot.monitor = mon;
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) {
        mi.rcWork = {0, 0, GetSystemMetrics(SM_CXSCREEN),
                     GetSystemMetrics(SM_CYSCREEN)};
    }
    g_spot.workAreaPx = mi.rcWork;
    g_spot.scale = GetMonitorDpiScale(mon);
    g_spot.darkMode = IsSystemDarkMode();

    float s = g_spot.scale;
    float workW = static_cast<float>(mi.rcWork.right - mi.rcWork.left) / s;
    float workH = static_cast<float>(mi.rcWork.bottom - mi.rcWork.top) / s;

    // Expanded height depends on tabs + optional calc hero + visible result rows
    SpotlightMode effMode = EffectiveMode();
    bool showTabs = g_spot.expanded;
    float tabsH = showTabs ? kTabsBandH : 0.0f;
    float calcH = (g_spot.expanded && g_spot.hasCalc) ? kCalcHeroH : 0.0f;
    float listH = 0.0f;
    if (g_spot.expanded) {
        if (g_spot.results.empty()) {
            listH = g_spot.hasCalc ? 0.0f : 62.0f;
        } else {
            listH = std::min(kMaxListH,
                             static_cast<float>(g_spot.results.size()) * kRowH);
        }
        if (g_ql.open) listH = std::max(listH, 260.0f);
    }
    float expandedSheetW = g_ql.open ? kSheetPreviewW : kSheetCompactW;
    float expandedSheetH =
        kHeaderH + tabsH + calcH + listH + (g_spot.expanded ? kFooterH : 0.0f);

    float collapsedGroupW =
        kCollapsedPillW + 4.0f * (kOrbGapDip + kOrbSizeDip);

    float t = Clamp01(g_spot.expandProg);
    float curW = Lerp(collapsedGroupW, expandedSheetW, t);
    float curH = Lerp(kCollapsedPillH, expandedSheetH, t);

    float pad = kShadowMarginDip;
    float maxSheetH = std::min(
        workH - 36.0f,
        kHeaderH + kTabsBandH + kCalcHeroH + kMaxListH + kFooterH);
    float hostW = std::max(collapsedGroupW, expandedSheetW) + 2.0f * pad;
    float hostH = std::max(kCollapsedPillH, maxSheetH) + 2.0f * pad;

    float cx = static_cast<float>(mi.rcWork.left) / s + workW * 0.5f;
    float topFrac =
        static_cast<float>(std::clamp(g_settings.verticalPosition, 8, 55)) /
        100.0f;
    float topDip =
        static_cast<float>(mi.rcWork.top) / s + workH * topFrac;
    if (topDip + maxSheetH >
        static_cast<float>(mi.rcWork.top) / s + workH - 20.0f) {
        topDip = std::max(static_cast<float>(mi.rcWork.top) / s + 16.0f,
                          static_cast<float>(mi.rcWork.top) / s + workH -
                              maxSheetH - 20.0f);
    }
    float hostLeftDip = cx - hostW * 0.5f;
    float hostTopDip = topDip - pad;

    g_spot.padDip = pad;
    g_spot.windowPx = {
        static_cast<LONG>(std::lround(hostLeftDip * s)),
        static_cast<LONG>(std::lround(hostTopDip * s)),
        static_cast<LONG>(std::lround((hostLeftDip + hostW) * s)),
        static_cast<LONG>(std::lround((hostTopDip + hostH) * s))};

    float groupLeftInClient = (hostW - curW) * 0.5f;
    g_spot.pillRect = {groupLeftInClient, pad, groupLeftInClient + kCollapsedPillW,
                       pad + kCollapsedPillH};
    float ox = g_spot.pillRect.right + kOrbGapDip;
    for (int i = 0; i < 4; ++i) {
        g_spot.orbRects[i] = {ox, pad, ox + kOrbSizeDip, pad + kOrbSizeDip};
        ox += kOrbSizeDip + kOrbGapDip;
    }

    float sheetWCur = Lerp(kCollapsedPillW, expandedSheetW, t);
    float sheetLeft = (hostW - sheetWCur) * 0.5f;
    g_spot.sheetRect = {sheetLeft, pad, sheetLeft + sheetWCur, pad + curH};
    float targetLeft = (hostW - expandedSheetW) * 0.5f;
    g_spot.targetSheetRect = {targetLeft, pad, targetLeft + expandedSheetW,
                              pad + expandedSheetH};

    // Quick Look split pane rect (on right side of sheet)
    if (g_spot.expanded && g_ql.open) {
        g_spot.qlPaneRect = {g_spot.sheetRect.right - kQuickLookW,
                             g_spot.sheetRect.top + kHeaderH,
                             g_spot.sheetRect.right,
                             g_spot.sheetRect.bottom - kFooterH};
    } else {
        g_spot.qlPaneRect = {};
    }

    float mainRight =
        (g_spot.expanded && g_ql.open) ? g_spot.qlPaneRect.left : g_spot.sheetRect.right;

    // Header sub-rects
    float chipOffset = 0.0f;
    IDWriteTextFormat* hdrFont = g_fonts.fHdrChip.get();
    std::wstring chipLbl =
        !g_spot.prefixChipLabel.empty()
            ? g_spot.prefixChipLabel
            : (g_spot.mode != SpotlightMode::All ? ModeDisplayName(g_spot.mode)
                                                 : L"");
    if (g_spot.expanded && !chipLbl.empty()) {
        float cw = MeasureTextWidthDip(chipLbl, hdrFont, 180.0f, 26.0f) +
                   32.0f;
        g_spot.chipCloseRect = {g_spot.sheetRect.left + 48.0f + cw - 20.0f,
                                pad + 15.0f,
                                g_spot.sheetRect.left + 48.0f + cw,
                                pad + 41.0f};
        chipOffset = cw + 8.0f;
    } else {
        g_spot.chipCloseRect = {};
    }

    g_spot.quickLookToggleRect =
        g_spot.expanded
            ? D2D1::RectF(mainRight - 36.0f, pad + 14.0f, mainRight - 10.0f,
                          pad + 42.0f)
            : D2D1::RectF();

    float inputStartX =
        (g_spot.expanded ? g_spot.sheetRect.left : g_spot.pillRect.left) +
        48.0f + chipOffset;
    float inputEndX =
        g_spot.expanded ? (mainRight - 44.0f) : (g_spot.pillRect.right - 16.0f);
    g_spot.queryTextRect = {inputStartX, pad + 6.0f, inputEndX, pad + kHeaderH - 6.0f};

    // Build filter tabs
    g_spot.filterTabs.clear();
    if (g_spot.expanded) {
        IDWriteTextFormat* tabFont = g_fonts.fTab.get();
        float tx = g_spot.sheetRect.left + 16.0f;
        float ty = pad + kHeaderH + 4.0f;
        if (effMode == SpotlightMode::Files) {
            struct FT {
                const wchar_t* lbl;
                FileSubTab st;
            };
            static const FT kFileTabs[] = {
                {L"All Files", FileSubTab::All},
                {L"Folders", FileSubTab::Folders},
                {L"Images", FileSubTab::Images},
                {L"Documents", FileSubTab::Documents},
                {L"Code", FileSubTab::Code},
                {L"Archives", FileSubTab::Archives},
            };
            for (const auto& ft : kFileTabs) {
                float w =
                    MeasureTextWidthDip(ft.lbl, tabFont, 140.0f, 26.0f) +
                    24.0f;
                SpotlightUiState::FilterTabHit hit;
                hit.rect = {tx, ty, tx + w, ty + 26.0f};
                hit.label = ft.lbl;
                hit.mode = SpotlightMode::Files;
                hit.fileSub = ft.st;
                hit.active = (g_spot.fileTab == ft.st);
                g_spot.filterTabs.push_back(std::move(hit));
                tx += w + 8.0f;
            }
        } else {
            struct MT {
                const wchar_t* lbl;
                SpotlightMode m;
            };
            static const MT kModeTabs[] = {
                {L"All", SpotlightMode::All},
                {L"Apps", SpotlightMode::Apps},
                {L"Files", SpotlightMode::Files},
                {L"Actions", SpotlightMode::Actions},
                {L"Clipboard", SpotlightMode::Clipboard},
                {L"Windows", SpotlightMode::Windows},
                {L"Settings", SpotlightMode::Settings},
            };
            for (const auto& mt : kModeTabs) {
                float w =
                    MeasureTextWidthDip(mt.lbl, tabFont, 140.0f, 26.0f) +
                    24.0f;
                if (tx + w > mainRight - 14.0f) break;
                SpotlightUiState::FilterTabHit hit;
                hit.rect = {tx, ty, tx + w, ty + 26.0f};
                hit.label = mt.lbl;
                hit.mode = mt.m;
                hit.active = (effMode == mt.m);
                g_spot.filterTabs.push_back(std::move(hit));
                tx += w + 8.0f;
            }
        }
    }

    float bodyY = pad + kHeaderH + tabsH;
    if (g_spot.expanded && g_spot.hasCalc) {
        g_spot.calcCardRect = {g_spot.sheetRect.left + 12.0f, bodyY + 4.0f,
                               mainRight - 12.0f, bodyY + kCalcHeroH - 4.0f};
        bodyY += kCalcHeroH;
    } else {
        g_spot.calcCardRect = {};
    }

    g_spot.listViewportRect = {g_spot.sheetRect.left, bodyY, mainRight,
                               g_spot.sheetRect.bottom - kFooterH};

    UpdateRowHitRectsOnly();
}

void TriggerSpotlightRebuild(bool triggerFileSearch) {
    SpotlightMode prevMode = EffectiveMode();
    QueryBuildOutput built = BuildSpotlightResults(
        g_spot.mode, g_spot.fileTab, g_spot.query, g_spot.prefixChipLabel,
        &g_spot.detectedPrefixMode, &g_spot.effectiveQuery);
    g_spot.results = std::move(built.results);
    g_spot.calc = std::move(built.calc);
    g_spot.hasCalc = built.hasCalc;
    g_spot.autoFillGhost = std::move(built.autoFillGhost);
    g_spot.autoFillFull = std::move(built.autoFillFull);
    g_spot.autoFillBadge = std::move(built.autoFillBadge);

    if (EffectiveMode() != prevMode) {
        if (EffectiveMode() == SpotlightMode::Windows) {
            RefreshWindowsSnapshot(g_spot.hwnd);
        } else if (EffectiveMode() == SpotlightMode::KillProcess) {
            RefreshProcessesSnapshot();
        } else if (EffectiveMode() == SpotlightMode::Bookmarks) {
            RefreshBookmarksIfStale();
        }
    }

    if (g_spot.hasCalc && g_spot.selected == 0 && g_spot.results.empty()) {
        g_spot.calcSelected = true;
    } else if (!g_spot.hasCalc) {
        g_spot.calcSelected = false;
    }
    if (g_spot.results.empty()) {
        g_spot.selected = 0;
    } else {
        g_spot.selected =
            std::clamp(g_spot.selected, 0,
                       static_cast<int>(g_spot.results.size()) - 1);
    }
    g_spot.scrollY = 0.0f;
    g_spot.targetScrollY = 0.0f;
    g_spot.selYInit = false;

    if (triggerFileSearch &&
        (EffectiveMode() == SpotlightMode::Files ||
         (EffectiveMode() == SpotlightMode::All &&
          g_spot.effectiveQuery.size() >= 2))) {
        if (g_spot.hwnd) {
            SetTimer(
                g_spot.hwnd, kTimerSearchDebounce,
                EffectiveMode() == SpotlightMode::Files ? 90 : 120, nullptr);
        }
    } else if (triggerFileSearch) {
        CancelPendingFileSearch();
    }
    SyncQuickLookToSelection();
    ComputeSpotlightLayout();
    g_spot.needsRedraw = true;
}

void SyncGlassVisualTree() {
    if (!g_spot.hwnd || !g_spot.rootVisual || !g_gfx.compositor) return;
    ComputeSpotlightLayout();

    // Move/resize host HWND cleanly
    RECT cur{};
    GetWindowRect(g_spot.hwnd, &cur);
    if (!EqualRect(&cur, &g_spot.windowPx)) {
        SetWindowPos(
            g_spot.hwnd, HWND_TOPMOST, g_spot.windowPx.left,
            g_spot.windowPx.top,
            g_spot.windowPx.right - g_spot.windowPx.left,
            g_spot.windowPx.bottom - g_spot.windowPx.top,
            SWP_NOACTIVATE | SWP_NOZORDER);
    }

    float s = g_spot.scale;
    float hostLeftPx = static_cast<float>(g_spot.windowPx.left);
    float hostTopPx = static_cast<float>(g_spot.windowPx.top);
    float winWPx =
        static_cast<float>(g_spot.windowPx.right - g_spot.windowPx.left);
    float winHPx =
        static_cast<float>(g_spot.windowPx.bottom - g_spot.windowPx.top);

    if (g_spot.visible &&
        !g_liquidCapture.watching.load(std::memory_order_relaxed)) {
        SetLiquidCaptureWatching(
            true, g_liquidCapture.highRate.load(std::memory_order_relaxed));
    }
    bool useOptGlass = EnsureLiquidCaptureReady(g_spot.monitor);
    float t = Clamp01(g_spot.expandProg);
    bool showCollapsed = (t < 0.08f && !g_spot.expanded);
    bool morphing = (!showCollapsed && t < 0.995f);

    float targetSW = std::max(
        1.0f, (g_spot.targetSheetRect.right - g_spot.targetSheetRect.left) * s);
    float targetSH = std::max(
        1.0f, (g_spot.targetSheetRect.bottom - g_spot.targetSheetRect.top) * s);
    float curSW = std::max(
        1.0f, (g_spot.sheetRect.right - g_spot.sheetRect.left) * s);
    float curSH = std::max(
        1.0f, (g_spot.sheetRect.bottom - g_spot.sheetRect.top) * s);

    // Update watched DXGI screen regions so background motion updates glass live
    std::vector<RECT> watchRegions;
    if (g_spot.visible) {
        if (showCollapsed) {
            watchRegions.push_back({
                static_cast<LONG>(std::floor(hostLeftPx + g_spot.pillRect.left * s)),
                static_cast<LONG>(std::floor(hostTopPx + g_spot.pillRect.top * s)),
                static_cast<LONG>(std::ceil(hostLeftPx + g_spot.pillRect.right * s)),
                static_cast<LONG>(std::ceil(hostTopPx + g_spot.pillRect.bottom * s))});
            for (int i = 0; i < 4; ++i) {
                watchRegions.push_back({
                    static_cast<LONG>(std::floor(hostLeftPx + g_spot.orbRects[i].left * s)),
                    static_cast<LONG>(std::floor(hostTopPx + g_spot.orbRects[i].top * s)),
                    static_cast<LONG>(std::ceil(hostLeftPx + g_spot.orbRects[i].right * s)),
                    static_cast<LONG>(std::ceil(hostTopPx + g_spot.orbRects[i].bottom * s))});
            }
        } else {
            watchRegions.push_back({
                static_cast<LONG>(std::floor(hostLeftPx + g_spot.targetSheetRect.left * s)),
                static_cast<LONG>(std::floor(hostTopPx + g_spot.targetSheetRect.top * s)),
                static_cast<LONG>(std::ceil(hostLeftPx + g_spot.targetSheetRect.right * s)),
                static_cast<LONG>(std::ceil(hostTopPx + g_spot.targetSheetRect.bottom * s))});
        }
    }
    UpdateLiquidCaptureWatchedRegions(watchRegions);
    SetLiquidCaptureWatching(
        g_spot.visible, g_liquidCapture.highRate.load(std::memory_order_relaxed));

    // Build / cache macOS soft ambient shadow surface
    std::vector<D2D1_ROUNDED_RECT> shRects;
    if (showCollapsed) {
        shRects.push_back(D2D1::RoundedRect(
            D2D1::RectF(g_spot.pillRect.left * s, g_spot.pillRect.top * s,
                        g_spot.pillRect.right * s, g_spot.pillRect.bottom * s),
            (kCollapsedPillH * 0.5f) * s, (kCollapsedPillH * 0.5f) * s));
        for (int i = 0; i < 4; ++i) {
            shRects.push_back(D2D1::RoundedRect(
                D2D1::RectF(g_spot.orbRects[i].left * s,
                            g_spot.orbRects[i].top * s,
                            g_spot.orbRects[i].right * s,
                            g_spot.orbRects[i].bottom * s),
                (kOrbSizeDip * 0.5f) * s, (kOrbSizeDip * 0.5f) * s));
        }
    } else {
        float cr = kSheetCornerDip * s;
        shRects.push_back(D2D1::RoundedRect(
            D2D1::RectF(g_spot.targetSheetRect.left * s,
                        g_spot.targetSheetRect.top * s,
                        g_spot.targetSheetRect.right * s,
                        g_spot.targetSheetRect.bottom * s),
            cr, cr));
    }
    std::array<float, 6> shKey{
        showCollapsed ? 0.0f : targetSW, showCollapsed ? 0.0f : targetSH,
        showCollapsed ? 0.0f : 1.0f, g_ql.open ? 1.0f : 0.0f,
        g_spot.darkMode ? 1.0f : 0.0f, s};
    if (!g_spot.shadowSurface || shKey != g_spot.lastShadowKey) {
        UpdateShadowSurfaceForRects(g_spot.shadowSurface, g_spot.shadowSurfSize,
                                    winWPx, winHPx, shRects, s, g_spot.darkMode);
        if (g_spot.shadowSurface) {
            if (!g_spot.shadowBrush) {
                g_spot.shadowBrush =
                    g_gfx.compositor.CreateSurfaceBrush(g_spot.shadowSurface);
                g_spot.shadowBrush.Stretch(wuc::CompositionStretch::Fill);
            } else {
                g_spot.shadowBrush.Surface(g_spot.shadowSurface);
            }
            if (!g_spot.shadowVisual) {
                g_spot.shadowVisual = g_gfx.compositor.CreateSpriteVisual();
                g_spot.shadowVisual.Brush(g_spot.shadowBrush);
                g_spot.rootVisual.Children().InsertAtBottom(
                    g_spot.shadowVisual);
            }
        }
        g_spot.lastShadowKey = shKey;
    }
    if (g_spot.shadowVisual) {
        g_spot.shadowVisual.Size({winWPx, winHPx});
        if (morphing) {
            g_spot.shadowVisual.CenterPoint(
                {winWPx * 0.5f, (g_spot.padDip + kHeaderH * 0.5f) * s, 0.0f});
            g_spot.shadowVisual.Scale(
                {std::clamp(curSW / targetSW, 0.1f, 1.0f),
                 std::clamp(curSH / targetSH, 0.1f, 1.0f), 1.0f});
        } else {
            g_spot.shadowVisual.Scale({1.0f, 1.0f, 1.0f});
        }
    }

    auto hidePlate = [](LiquidGlassPlate& pl) {
        if (pl.visual) pl.visual.IsVisible(false);
    };
    auto placePlate = [&](LiquidGlassPlate& pl, D2D1_RECT_F dipRc, float radDip,
                          bool squircle, float orbHover) {
        float x = dipRc.left * s;
        float y = dipRc.top * s;
        float w = (dipRc.right - dipRc.left) * s;
        float h = (dipRc.bottom - dipRc.top) * s;
        if (useOptGlass) {
            LiquidGlassStyleParams lp{};
            lp.screenX = hostLeftPx + x;
            lp.screenY = hostTopPx + y;
            lp.widthPx = w;
            lp.heightPx = h;
            lp.cornerRadiusPx = radDip * s;
            lp.scale = s;
            lp.darkMode = g_spot.darkMode;
            lp.useSquircle = squircle;
            lp.tintBoost = orbHover * 0.08f;
            if (RenderLiquidGlassPlate(pl, lp) && pl.visual) {
                if (!pl.visual.Parent()) {
                    g_spot.rootVisual.Children().InsertAbove(
                        pl.visual, g_spot.shadowVisual);
                }
                pl.visual.Scale({1.0f, 1.0f, 1.0f});
                pl.visual.Offset({x, y, 0.0f});
                pl.visual.Size({w, h});
                pl.visual.IsVisible(true);
                return;
            }
        }
        hidePlate(pl);
    };

    if (showCollapsed) {
        hidePlate(g_spot.sheetPlate);
        placePlate(g_spot.pillPlate, g_spot.pillRect, kCollapsedPillH * 0.5f,
                   false, 0.0f);
        for (int i = 0; i < 4; ++i) {
            placePlate(g_spot.orbPlates[i], g_spot.orbRects[i],
                       kOrbSizeDip * 0.5f, false, g_spot.orbHoverT[i]);
        }
    } else {
        hidePlate(g_spot.pillPlate);
        for (int i = 0; i < 4; ++i) hidePlate(g_spot.orbPlates[i]);
        placePlate(g_spot.sheetPlate, g_spot.targetSheetRect, kSheetCornerDip,
                   true, 0.0f);
        if (g_spot.sheetPlate.visual) {
            if (morphing) {
                g_spot.sheetPlate.visual.CenterPoint({0.0f, 0.0f, 0.0f});
                g_spot.sheetPlate.visual.Offset(
                    {g_spot.sheetRect.left * s, g_spot.sheetRect.top * s, 0.0f});
                g_spot.sheetPlate.visual.Scale(
                    {std::clamp(curSW / targetSW, 0.1f, 1.0f),
                     std::clamp(curSH / targetSH, 0.1f, 1.0f), 1.0f});
            } else {
                g_spot.sheetPlate.visual.Scale({1.0f, 1.0f, 1.0f});
                g_spot.sheetPlate.visual.Offset(
                    {g_spot.sheetRect.left * s, g_spot.sheetRect.top * s, 0.0f});
            }
        }
    }

    // Ensure foreground UI sprite visual sits on top of all glass plates
    SIZE wantSz{std::max(1L, static_cast<LONG>(std::ceil(winWPx))),
                std::max(1L, static_cast<LONG>(std::ceil(winHPx)))};
    if (!g_spot.uiSurface) {
        g_spot.uiSurface =
            CreateDrawingSurface(static_cast<float>(wantSz.cx),
                                 static_cast<float>(wantSz.cy));
        g_spot.uiSurfSize = wantSz;
    } else if (g_spot.uiSurfSize.cx != wantSz.cx ||
               g_spot.uiSurfSize.cy != wantSz.cy) {
        auto interop =
            g_spot.uiSurface.as<ICompositionDrawingSurfaceInteropLocal>();
        if (FAILED(interop->Resize(wantSz))) {
            g_spot.uiSurface =
                CreateDrawingSurface(static_cast<float>(wantSz.cx),
                                     static_cast<float>(wantSz.cy));
        }
        g_spot.uiSurfSize = wantSz;
    }
    if (g_spot.uiSurface) {
        if (!g_spot.uiBrush) {
            g_spot.uiBrush =
                g_gfx.compositor.CreateSurfaceBrush(g_spot.uiSurface);
            g_spot.uiBrush.Stretch(wuc::CompositionStretch::Fill);
        } else {
            g_spot.uiBrush.Surface(g_spot.uiSurface);
        }
        if (!g_spot.uiVisual) {
            g_spot.uiVisual = g_gfx.compositor.CreateSpriteVisual();
            g_spot.uiVisual.Brush(g_spot.uiBrush);
            g_spot.rootVisual.Children().InsertAtTop(g_spot.uiVisual);
        }
        g_spot.uiVisual.Size({winWPx, winHPx});
    }

    // Smooth open fade & subtle spring pop on rootVisual
    g_spot.rootVisual.Opacity(Clamp01(g_spot.openAlpha));
    g_spot.rootVisual.CenterPoint({winWPx * 0.5f, winHPx * 0.30f, 0.0f});
    float sc = Lerp(0.95f, 1.0f, Clamp01(g_spot.openScale));
    g_spot.rootVisual.Scale({sc, sc, 1.0f});
}

void RenderSpotlightSurface();

void RefreshGlassBackdropOnly() {
    if (!g_spot.hwnd || !g_spot.rootVisual || !g_gfx.compositor ||
        !g_spot.visible) {
        return;
    }
    bool hadGlass = g_spot.sheetPlate.active || g_spot.pillPlate.active;
    if (!EnsureLiquidCaptureReady(g_spot.monitor) ||
        !g_liquidCapture.desktopTex) {
        return;
    }

    float s = g_spot.scale;
    float hostLeftPx = static_cast<float>(g_spot.windowPx.left);
    float hostTopPx = static_cast<float>(g_spot.windowPx.top);
    float t = Clamp01(g_spot.expandProg);
    bool showCollapsed = (t < 0.08f && !g_spot.expanded);

    auto refreshPlate = [&](LiquidGlassPlate& pl, D2D1_RECT_F dipRc,
                            float radDip, bool squircle, float orbHover) {
        float x = dipRc.left * s;
        float y = dipRc.top * s;
        float w = (dipRc.right - dipRc.left) * s;
        float h = (dipRc.bottom - dipRc.top) * s;
        LiquidGlassStyleParams lp{};
        lp.screenX = hostLeftPx + x;
        lp.screenY = hostTopPx + y;
        lp.widthPx = w;
        lp.heightPx = h;
        lp.cornerRadiusPx = radDip * s;
        lp.scale = s;
        lp.darkMode = g_spot.darkMode;
        lp.useSquircle = squircle;
        lp.tintBoost = orbHover * 0.08f;
        if (RenderLiquidGlassPlate(pl, lp) && pl.visual) {
            if (!pl.visual.Parent()) {
                g_spot.rootVisual.Children().InsertAbove(pl.visual,
                                                         g_spot.shadowVisual);
            }
            pl.visual.Size({w, h});
            pl.visual.IsVisible(true);
        }
    };

    if (showCollapsed) {
        refreshPlate(g_spot.pillPlate, g_spot.pillRect, kCollapsedPillH * 0.5f,
                     false, 0.0f);
        for (int i = 0; i < 4; ++i) {
            refreshPlate(g_spot.orbPlates[i], g_spot.orbRects[i],
                         kOrbSizeDip * 0.5f, false, g_spot.orbHoverT[i]);
        }
    } else {
        refreshPlate(g_spot.sheetPlate, g_spot.targetSheetRect, kSheetCornerDip,
                     true, 0.0f);
        if (g_spot.sheetPlate.visual) {
            bool morphing = (t < 0.995f);
            if (morphing) {
                float targetSW = std::max(
                    1.0f,
                    (g_spot.targetSheetRect.right - g_spot.targetSheetRect.left) * s);
                float targetSH = std::max(
                    1.0f,
                    (g_spot.targetSheetRect.bottom - g_spot.targetSheetRect.top) * s);
                float curSW = std::max(
                    1.0f, (g_spot.sheetRect.right - g_spot.sheetRect.left) * s);
                float curSH = std::max(
                    1.0f, (g_spot.sheetRect.bottom - g_spot.sheetRect.top) * s);
                g_spot.sheetPlate.visual.CenterPoint({0.0f, 0.0f, 0.0f});
                g_spot.sheetPlate.visual.Offset(
                    {g_spot.sheetRect.left * s, g_spot.sheetRect.top * s, 0.0f});
                g_spot.sheetPlate.visual.Scale(
                    {std::clamp(curSW / targetSW, 0.1f, 1.0f),
                     std::clamp(curSH / targetSH, 0.1f, 1.0f), 1.0f});
            } else {
                g_spot.sheetPlate.visual.Scale({1.0f, 1.0f, 1.0f});
                g_spot.sheetPlate.visual.Offset(
                    {g_spot.sheetRect.left * s, g_spot.sheetRect.top * s, 0.0f});
            }
        }
    }

    bool nowGlass = g_spot.sheetPlate.active || g_spot.pillPlate.active;
    if (nowGlass != hadGlass) RenderSpotlightSurface();
}

void DrawFallbackGlassSurface(ID2D1DeviceContext* dc,
                              D2D1_RECT_F rc,
                              float radius,
                              bool squircle,
                              bool dark) {
    winrt::com_ptr<ID2D1SolidColorBrush> fill, rim;
    D2D1_COLOR_F fc = dark ? D2D1::ColorF(0.11f, 0.12f, 0.16f, 0.90f)
                           : D2D1::ColorF(0.96f, 0.97f, 0.99f, 0.88f);
    D2D1_COLOR_F sc = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f)
                           : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.78f);
    dc->CreateSolidColorBrush(fc, fill.put());
    dc->CreateSolidColorBrush(sc, rim.put());
    if (squircle) {
        if (auto g = BuildContinuousSquirclePath(rc.left, rc.top,
                                                 rc.right - rc.left,
                                                 rc.bottom - rc.top, radius)) {
            if (fill) dc->FillGeometry(g.get(), fill.get());
            if (rim) dc->DrawGeometry(g.get(), rim.get(), 1.15f);
            return;
        }
    }
    D2D1_ROUNDED_RECT rr{rc, radius, radius};
    if (fill) dc->FillRoundedRectangle(rr, fill.get());
    if (rim) dc->DrawRoundedRectangle(rr, rim.get(), 1.15f);
}

void DrawTitleWithHighlights(ID2D1DeviceContext* dc,
                             IDWriteTextFormat* fmt,
                             const std::wstring& text,
                             const std::vector<uint8_t>& mask,
                             D2D1_RECT_F rc,
                             D2D1_COLOR_F normalCol,
                             D2D1_COLOR_F hiCol) {
    if (!g_gfx.dwriteFactory || !fmt || text.empty()) return;
    winrt::com_ptr<IDWriteTextLayout> layout;
    if (FAILED(g_gfx.dwriteFactory->CreateTextLayout(
            text.c_str(), static_cast<UINT32>(text.size()), fmt,
            std::max(1.0f, rc.right - rc.left),
            std::max(1.0f, rc.bottom - rc.top), layout.put()))) {
        return;
    }
    winrt::com_ptr<ID2D1SolidColorBrush> baseBr, hiBr;
    dc->CreateSolidColorBrush(normalCol, baseBr.put());
    dc->CreateSolidColorBrush(hiCol, hiBr.put());
    if (hiBr && !mask.empty()) {
        size_t lim = std::min(mask.size(), text.size());
        size_t i = 0;
        while (i < lim) {
            if (!mask[i]) {
                ++i;
                continue;
            }
            size_t j = i;
            while (j < lim && mask[j]) ++j;
            DWRITE_TEXT_RANGE rng{static_cast<UINT32>(i),
                                  static_cast<UINT32>(j - i)};
            layout->SetDrawingEffect(hiBr.get(), rng);
            layout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, rng);
            i = j;
        }
    }
    if (baseBr) dc->DrawTextLayout({rc.left, rc.top}, layout.get(), baseBr.get());
}

void RenderSpotlightSurface() {
    if (!g_spot.uiSurface) return;
    g_spot.needsRedraw = false;
    EnsureSpotlightFonts();

    DrawCompositionSurface(g_spot.uiSurface, [&](ID2D1DeviceContext* dc) {
        float s = g_spot.scale;
        dc->SetTransform(D2D1::Matrix3x2F::Scale(s, s));

        const bool dark = g_spot.darkMode;
        const bool optGlass = g_spot.sheetPlate.active || g_spot.pillPlate.active;
        const auto acc = GetAccentColor();
        const D2D1_COLOR_F accentCol = D2D1::ColorF(
            acc.R / 255.0f, acc.G / 255.0f, acc.B / 255.0f, 1.0f);
        const D2D1_COLOR_F caretCol =
            D2D1::ColorF(0.14f, 0.50f, 0.96f, 1.0f);  // Apple Spotlight blue

        const D2D1_COLOR_F textPrimary =
            dark ? D2D1::ColorF(0.95f, 0.97f, 1.0f, 0.96f)
                 : D2D1::ColorF(0.08f, 0.10f, 0.13f, 0.92f);
        const D2D1_COLOR_F textSecondary =
            dark ? D2D1::ColorF(0.76f, 0.81f, 0.88f, 0.70f)
                 : D2D1::ColorF(0.24f, 0.28f, 0.35f, 0.68f);
        const D2D1_COLOR_F textHint =
            dark ? D2D1::ColorF(0.72f, 0.77f, 0.84f, 0.52f)
                 : D2D1::ColorF(0.20f, 0.24f, 0.30f, 0.48f);

        float t = Clamp01(g_spot.expandProg);
        bool collapsedView = (t < 0.08f && !g_spot.expanded);

        // Fallback glass backing if DXGI Desktop Duplication unavailable
        if (!optGlass) {
            if (collapsedView) {
                DrawFallbackGlassSurface(dc, g_spot.pillRect,
                                         kCollapsedPillH * 0.5f, false, dark);
                for (int i = 0; i < 4; ++i) {
                    DrawFallbackGlassSurface(dc, g_spot.orbRects[i],
                                             kOrbSizeDip * 0.5f, false, dark);
                }
            } else {
                DrawFallbackGlassSurface(
                    dc, g_spot.sheetRect,
                    Lerp(kCollapsedPillH * 0.5f, kSheetCornerDip, t),
                    t >= 0.35f, dark);
            }
        }

        // Fonts (cached in g_fonts)
        auto& fQuery = g_fonts.fQuery;
        auto& fGhost = g_fonts.fGhost;
        auto& fChip = g_fonts.fChip;
        auto& fTab = g_fonts.fTab;
        auto& fTitle = g_fonts.fTitle;
        auto& fSub = g_fonts.fSub;
        auto& fBadge = g_fonts.fBadge;
        auto& fCalcBig = g_fonts.fCalcBig;
        auto& fMono = g_fonts.fMono;
        auto& fFooter = g_fonts.fFooter;
        auto& fFooterR = g_fonts.fFooterR;

        // 1. Render Collapsed Floating Orbs (Image 1)
        if (collapsedView) {
            for (int i = 0; i < 4; ++i) {
                float hv = g_spot.orbHoverT[i];
                if (hv > 0.01f) {
                    winrt::com_ptr<ID2D1SolidColorBrush> hbr;
                    D2D1_COLOR_F hc =
                        dark ? D2D1::ColorF(1, 1, 1, 0.14f * hv)
                             : D2D1::ColorF(1, 1, 1, 0.42f * hv);
                    if (SUCCEEDED(dc->CreateSolidColorBrush(hc, hbr.put()))) {
                        dc->FillRoundedRectangle(
                            D2D1::RoundedRect(g_spot.orbRects[i],
                                              kOrbSizeDip * 0.5f,
                                              kOrbSizeDip * 0.5f),
                            hbr.get());
                    }
                }
                D2D1_COLOR_F gCol =
                    hv > 0.25f ? accentCol
                               : (dark ? D2D1::ColorF(0.92f, 0.95f, 1.0f, 0.90f)
                                       : D2D1::ColorF(0.14f, 0.16f, 0.20f, 0.84f));
                DrawOrbGlyph(dc, i, g_spot.orbRects[i], gCol);
            }
        }

        // 2. Search Bar Header (shared in Collapsed Pill & Expanded Card)
        D2D1_RECT_F hdr = collapsedView ? g_spot.pillRect : g_spot.sheetRect;
        float magCx = hdr.left + 28.0f;
        float magCy = hdr.top + kHeaderH * 0.5f;
        DrawMagnifierGlyph(dc, magCx, magCy, 9.2f, textSecondary, 1.85f);

        // Active mode chip pill (when in Apps/Files/Actions/Clipboard/etc.)
        std::wstring chipLbl =
            !g_spot.prefixChipLabel.empty()
                ? g_spot.prefixChipLabel
                : (g_spot.mode != SpotlightMode::All
                       ? ModeDisplayName(g_spot.mode)
                       : L"");
        if (g_spot.expanded && !chipLbl.empty()) {
            float chipW = MeasureTextWidthDip(chipLbl, fChip.get(), 180.0f, 26.0f) +
                          32.0f;
            D2D1_RECT_F chipRc{hdr.left + 48.0f, hdr.top + 15.0f,
                               hdr.left + 48.0f + chipW, hdr.top + 41.0f};
            D2D1_COLOR_F cFill =
                dark ? D2D1::ColorF(accentCol.r, accentCol.g, accentCol.b, 0.26f)
                     : D2D1::ColorF(accentCol.r, accentCol.g, accentCol.b, 0.16f);
            D2D1_COLOR_F cStroke =
                dark ? D2D1::ColorF(accentCol.r, accentCol.g, accentCol.b, 0.55f)
                     : D2D1::ColorF(accentCol.r, accentCol.g, accentCol.b, 0.36f);
            D2D1_RECT_F lblRc{chipRc.left + 8.0f, chipRc.top,
                              g_spot.chipCloseRect.left + 2.0f, chipRc.bottom};
            DrawChipBadge(dc, fChip.get(), chipRc, L"", cFill, cStroke,
                          textPrimary, 13.0f);
            winrt::com_ptr<ID2D1SolidColorBrush> cTxt;
            dc->CreateSolidColorBrush(textPrimary, cTxt.put());
            if (fChip && cTxt) {
                dc->DrawText(chipLbl.c_str(), static_cast<UINT32>(chipLbl.size()),
                             fChip.get(), lblRc, cTxt.get());
                std::wstring xTxt = L"\u00D7";
                dc->DrawText(xTxt.c_str(), 1, fChip.get(), g_spot.chipCloseRect,
                             cTxt.get());
            }
        }

        // Quick Look header toggle icon (Alt+P)
        if (g_spot.expanded && !collapsedView) {
            D2D1_COLOR_F qBg =
                g_ql.open
                    ? D2D1::ColorF(accentCol.r, accentCol.g, accentCol.b,
                                   dark ? 0.32f : 0.18f)
                    : (g_spot.hoverQuickLookBtn
                           ? D2D1::ColorF(1, 1, 1, dark ? 0.14f : 0.36f)
                           : D2D1::ColorF(0, 0, 0, 0));
            D2D1_ROUNDED_RECT qrr{g_spot.quickLookToggleRect, 8.0f, 8.0f};
            winrt::com_ptr<ID2D1SolidColorBrush> qFill, qStroke;
            if (qBg.a > 0.01f &&
                SUCCEEDED(dc->CreateSolidColorBrush(qBg, qFill.put()))) {
                dc->FillRoundedRectangle(qrr, qFill.get());
            }
            if (SUCCEEDED(dc->CreateSolidColorBrush(
                    g_ql.open ? accentCol : textSecondary, qStroke.put()))) {
                D2D1_RECT_F box{g_spot.quickLookToggleRect.left + 5.0f,
                                g_spot.quickLookToggleRect.top + 7.0f,
                                g_spot.quickLookToggleRect.right - 5.0f,
                                g_spot.quickLookToggleRect.bottom - 7.0f};
                dc->DrawRoundedRectangle(D2D1::RoundedRect(box, 2.5f, 2.5f),
                                         qStroke.get(), 1.35f);
                float splitX = box.left + (box.right - box.left) * 0.56f;
                dc->DrawLine({splitX, box.top}, {splitX, box.bottom},
                             qStroke.get(), 1.25f);
            }
        }

        // Query Text / Watermark / Selection / Caret / Inline Autocomplete Ghost Pill
        std::wstring displayText = g_spot.query;
        if (g_spot.imeComposing && !g_spot.imeComp.empty()) {
            int cp = std::clamp(g_spot.caret, 0,
                                static_cast<int>(displayText.size()));
            displayText.insert(static_cast<size_t>(cp), g_spot.imeComp);
        }

        float textClipW =
            std::max(10.0f, g_spot.queryTextRect.right - g_spot.queryTextRect.left);
        float textH = g_spot.queryTextRect.bottom - g_spot.queryTextRect.top;

        if (displayText.empty()) {
            // Draw Apple-blue blinking caret on left of "Spotlight Search" (Image 1!)
            if (g_spot.caretOn) {
                winrt::com_ptr<ID2D1SolidColorBrush> cb;
                dc->CreateSolidColorBrush(caretCol, cb.put());
                if (cb) {
                    dc->FillRectangle(
                        D2D1::RectF(g_spot.queryTextRect.left,
                                    hdr.top + 16.0f,
                                    g_spot.queryTextRect.left + 2.0f,
                                    hdr.top + kHeaderH - 16.0f),
                        cb.get());
                }
            }
            std::wstring ph =
                EffectiveMode() == SpotlightMode::All
                    ? L"Spotlight Search"
                    : (L"Search " + ModeDisplayName(EffectiveMode()) +
                       L"\u2026");
            winrt::com_ptr<ID2D1SolidColorBrush> phBr;
            dc->CreateSolidColorBrush(textHint, phBr.put());
            if (fQuery && phBr) {
                D2D1_RECT_F phRc{g_spot.queryTextRect.left + 4.0f,
                                 g_spot.queryTextRect.top,
                                 g_spot.queryTextRect.right,
                                 g_spot.queryTextRect.bottom};
                dc->DrawText(ph.c_str(), static_cast<UINT32>(ph.size()),
                             fQuery.get(), phRc, phBr.get());
            }
        } else {
            winrt::com_ptr<IDWriteTextLayout> qLayout;
            if (g_gfx.dwriteFactory && fQuery &&
                SUCCEEDED(g_gfx.dwriteFactory->CreateTextLayout(
                    displayText.c_str(), static_cast<UINT32>(displayText.size()),
                    fQuery.get(), 4000.0f, textH, qLayout.put()))) {
                DWRITE_TEXT_METRICS qm{};
                qLayout->GetMetrics(&qm);
                float caretPos = static_cast<float>(std::clamp(
                    g_spot.caret +
                        (g_spot.imeComposing ? g_spot.imeCompCursor : 0),
                    0, static_cast<int>(displayText.size())));
                float cxPx = 0.0f, cyPx = 0.0f;
                DWRITE_HIT_TEST_METRICS htm{};
                qLayout->HitTestTextPosition(
                    static_cast<UINT32>(caretPos), FALSE, &cxPx, &cyPx, &htm);
                float scrollX = std::max(0.0f, cxPx - textClipW * 0.85f);

                dc->PushAxisAlignedClip(g_spot.queryTextRect,
                                        D2D1_ANTIALIAS_MODE_ALIASED);

                // Selection highlight
                int s0 = std::min(g_spot.caret, g_spot.selAnchor);
                int s1 = std::max(g_spot.caret, g_spot.selAnchor);
                if (!g_spot.imeComposing && s1 > s0) {
                    float sx0 = 0, sx1 = 0, sy = 0;
                    DWRITE_HIT_TEST_METRICS m0{}, m1{};
                    qLayout->HitTestTextPosition(static_cast<UINT32>(s0), FALSE,
                                                 &sx0, &sy, &m0);
                    qLayout->HitTestTextPosition(static_cast<UINT32>(s1), FALSE,
                                                 &sx1, &sy, &m1);
                    winrt::com_ptr<ID2D1SolidColorBrush> selBr;
                    dc->CreateSolidColorBrush(
                        D2D1::ColorF(caretCol.r, caretCol.g, caretCol.b, 0.28f),
                        selBr.put());
                    if (selBr) {
                        dc->FillRoundedRectangle(
                            D2D1::RoundedRect(
                                D2D1::RectF(
                                    g_spot.queryTextRect.left + sx0 - scrollX,
                                    hdr.top + 14.0f,
                                    g_spot.queryTextRect.left + sx1 - scrollX,
                                    hdr.top + kHeaderH - 14.0f),
                                4.0f, 4.0f),
                            selBr.get());
                    }
                }

                // Draw query text
                winrt::com_ptr<ID2D1SolidColorBrush> qBr;
                dc->CreateSolidColorBrush(textPrimary, qBr.put());
                if (qBr) {
                    dc->DrawTextLayout(
                        {g_spot.queryTextRect.left - scrollX,
                         g_spot.queryTextRect.top},
                        qLayout.get(), qBr.get());
                }

                // Blinking caret
                if (g_spot.caretOn) {
                    winrt::com_ptr<ID2D1SolidColorBrush> cb;
                    dc->CreateSolidColorBrush(caretCol, cb.put());
                    if (cb) {
                        float rx = g_spot.queryTextRect.left + cxPx - scrollX;
                        dc->FillRectangle(
                            D2D1::RectF(rx, hdr.top + 15.0f, rx + 2.0f,
                                        hdr.top + kHeaderH - 15.0f),
                            cb.get());
                    }
                }

                // Inline Frosted Autocomplete Ghost Pill (Image 2: [Message — Run])
                if (g_spot.expanded && !g_spot.imeComposing &&
                    !g_spot.autoFillGhost.empty() &&
                    g_spot.caret == static_cast<int>(g_spot.query.size()) &&
                    g_spot.selAnchor == g_spot.caret) {
                    float pillStartX = g_spot.queryTextRect.left +
                                       qm.widthIncludingTrailingWhitespace -
                                       scrollX + 6.0f;
                    float gw = MeasureTextWidthDip(g_spot.autoFillGhost,
                                                   fGhost.get(), 320.0f, 30.0f);
                    float bw =
                        g_spot.autoFillBadge.empty()
                            ? 0.0f
                            : MeasureTextWidthDip(
                                  L"  \u2014  " + g_spot.autoFillBadge,
                                  fTab.get(), 140.0f, 30.0f);
                    float pillW = gw + bw + 20.0f;
                    if (pillStartX + 36.0f < g_spot.queryTextRect.right) {
                        D2D1_RECT_F gRc{
                            pillStartX, hdr.top + 11.0f,
                            std::min(g_spot.queryTextRect.right,
                                     pillStartX + pillW),
                            hdr.top + kHeaderH - 11.0f};
                        D2D1_COLOR_F gf =
                            dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                 : D2D1::ColorF(1, 1, 1, 0.56f);
                        D2D1_COLOR_F gs =
                            dark ? D2D1::ColorF(1, 1, 1, 0.22f)
                                 : D2D1::ColorF(1, 1, 1, 0.85f);
                        DrawChipBadge(dc, nullptr, gRc, L"", gf, gs,
                                      textPrimary, 10.0f);
                        D2D1_RECT_F gTxt{gRc.left + 9.0f, gRc.top,
                                         gRc.right - 8.0f, gRc.bottom};
                        winrt::com_ptr<ID2D1SolidColorBrush> gb, bb;
                        dc->CreateSolidColorBrush(textPrimary, gb.put());
                        dc->CreateSolidColorBrush(textSecondary, bb.put());
                        if (fGhost && gb) {
                            dc->DrawText(
                                g_spot.autoFillGhost.c_str(),
                                static_cast<UINT32>(g_spot.autoFillGhost.size()),
                                fGhost.get(), gTxt, gb.get());
                        }
                        if (!g_spot.autoFillBadge.empty() && fTab && bb &&
                            gTxt.left + gw + 10.0f < gTxt.right) {
                            std::wstring tail =
                                L"\u2014 " + g_spot.autoFillBadge;
                            D2D1_RECT_F bTxt{gTxt.left + gw + 6.0f, gRc.top,
                                             gTxt.right, gRc.bottom};
                            dc->DrawText(tail.c_str(),
                                         static_cast<UINT32>(tail.size()),
                                         fTab.get(), bTxt, bb.get());
                        }
                    }
                }

                dc->PopAxisAlignedClip();
            }
        }

        // 3. Expanded Content (Filter Pills + Calc Hero + Results + Quick Look + Footer)
        if (g_spot.expanded && t > 0.25f) {
            dc->PushAxisAlignedClip(g_spot.sheetRect,
                                    D2D1_ANTIALIAS_MODE_ALIASED);
            // Soft hairline divider below header
            winrt::com_ptr<ID2D1SolidColorBrush> divBr;
            dc->CreateSolidColorBrush(
                dark ? D2D1::ColorF(1, 1, 1, 0.10f)
                     : D2D1::ColorF(0, 0, 0, 0.08f),
                divBr.put());
            if (divBr) {
                dc->DrawLine({g_spot.sheetRect.left + 12.0f, hdr.top + kHeaderH},
                             {g_spot.sheetRect.right - 12.0f, hdr.top + kHeaderH},
                             divBr.get(), 1.0f);
            }

            // Category Filter Pills (Image 2: Photos | Actions | Help | Mail bar)
            for (int ti = 0; ti < static_cast<int>(g_spot.filterTabs.size());
                 ++ti) {
                const auto& tab = g_spot.filterTabs[static_cast<size_t>(ti)];
                bool hov = (ti == g_spot.hoveredTab);
                D2D1_COLOR_F fFill =
                    tab.active
                        ? (dark ? D2D1::ColorF(1, 1, 1, 0.22f)
                                : D2D1::ColorF(1, 1, 1, 0.72f))
                        : (hov ? (dark ? D2D1::ColorF(1, 1, 1, 0.12f)
                                       : D2D1::ColorF(1, 1, 1, 0.42f))
                               : (dark ? D2D1::ColorF(1, 1, 1, 0.06f)
                                       : D2D1::ColorF(1, 1, 1, 0.24f)));
                D2D1_COLOR_F fStroke =
                    tab.active
                        ? (dark ? D2D1::ColorF(1, 1, 1, 0.34f)
                                : D2D1::ColorF(1, 1, 1, 0.90f))
                        : (dark ? D2D1::ColorF(1, 1, 1, 0.09f)
                                : D2D1::ColorF(0, 0, 0, 0.06f));
                DrawChipBadge(dc, fTab.get(), tab.rect, tab.label, fFill,
                              fStroke, tab.active ? textPrimary : textSecondary,
                              13.0f);
            }

            // Calculator / Unit Conversion Hero Card
            if (g_spot.hasCalc && g_spot.calcCardRect.bottom > g_spot.calcCardRect.top) {
                D2D1_COLOR_F cFill =
                    g_spot.calcSelected
                        ? (dark ? D2D1::ColorF(1, 1, 1, 0.18f)
                                : D2D1::ColorF(1, 1, 1, 0.62f))
                        : (dark ? D2D1::ColorF(1, 1, 1, 0.09f)
                                : D2D1::ColorF(1, 1, 1, 0.36f));
                D2D1_COLOR_F cRim =
                    g_spot.calcSelected
                        ? (dark ? D2D1::ColorF(1, 1, 1, 0.36f)
                                : D2D1::ColorF(1, 1, 1, 0.92f))
                        : (dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                : D2D1::ColorF(0, 0, 0, 0.08f));
                DrawChipBadge(dc, nullptr, g_spot.calcCardRect, L"", cFill, cRim,
                              textPrimary, 14.0f);

                winrt::com_ptr<ID2D1SolidColorBrush> pBr, sBr;
                dc->CreateSolidColorBrush(textPrimary, pBr.put());
                dc->CreateSolidColorBrush(textSecondary, sBr.put());
                D2D1_RECT_F valRc{g_spot.calcCardRect.left + 16.0f,
                                  g_spot.calcCardRect.top + 4.0f,
                                  g_spot.calcCardRect.right - 130.0f,
                                  g_spot.calcCardRect.bottom - 20.0f};
                if (fCalcBig && pBr) {
                    dc->DrawText(g_spot.calc.display.c_str(),
                                 static_cast<UINT32>(g_spot.calc.display.size()),
                                 fCalcBig.get(), valRc, pBr.get());
                }
                D2D1_RECT_F capRc{g_spot.calcCardRect.left + 16.0f,
                                  g_spot.calcCardRect.bottom - 24.0f,
                                  g_spot.calcCardRect.right - 130.0f,
                                  g_spot.calcCardRect.bottom - 4.0f};
                std::wstring capTxt = L"Calculator \u00B7 Press \u21B5 to copy";
                if (fSub && sBr) {
                    dc->DrawText(capTxt.c_str(),
                                 static_cast<UINT32>(capTxt.size()), fSub.get(),
                                 capRc, sBr.get());
                }
                D2D1_RECT_F badgeRc{g_spot.calcCardRect.right - 108.0f,
                                    g_spot.calcCardRect.top + 19.0f,
                                    g_spot.calcCardRect.right - 14.0f,
                                    g_spot.calcCardRect.bottom - 19.0f};
                DrawChipBadge(dc, fBadge.get(), badgeRc, L"Copy Result",
                              dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                   : D2D1::ColorF(1, 1, 1, 0.65f),
                              dark ? D2D1::ColorF(1, 1, 1, 0.22f)
                                   : D2D1::ColorF(0, 0, 0, 0.08f),
                              textPrimary, 9.0f);
            }

            // Results List Viewport
            if (g_spot.listViewportRect.bottom > g_spot.listViewportRect.top) {
                dc->PushAxisAlignedClip(g_spot.listViewportRect,
                                        D2D1_ANTIALIAS_MODE_ALIASED);

                if (g_spot.results.empty() && !g_spot.hasCalc) {
                    winrt::com_ptr<ID2D1SolidColorBrush> eBr;
                    dc->CreateSolidColorBrush(textSecondary, eBr.put());
                    std::wstring emptyMsg =
                        g_fileSearchActive
                            ? L"Searching files\u2026"
                            : (g_spot.query.empty()
                                   ? L"Start typing to search apps, files, and commands"
                                   : L"No matching items found");
                    auto& fEmpty = g_fonts.fEmpty;
                    if (fEmpty && eBr) {
                        dc->DrawText(emptyMsg.c_str(),
                                     static_cast<UINT32>(emptyMsg.size()),
                                     fEmpty.get(), g_spot.listViewportRect,
                                     eBr.get());
                    }
                } else {
                    // Gliding Frosted Selection Pill (Image 2!)
                    if (!g_spot.calcSelected && !g_spot.results.empty()) {
                        D2D1_RECT_F sRc{
                            g_spot.sheetRect.left + 10.0f,
                            g_spot.listViewportRect.top + g_spot.selYCur + 2.0f,
                            g_spot.listViewportRect.right - 10.0f,
                            g_spot.listViewportRect.top + g_spot.selYCur +
                                kRowH - 2.0f};
                        D2D1_COLOR_F sFill =
                            dark ? D2D1::ColorF(1, 1, 1, 0.16f)
                                 : D2D1::ColorF(1, 1, 1, 0.56f);
                        D2D1_COLOR_F sRim =
                            dark ? D2D1::ColorF(1, 1, 1, 0.28f)
                                 : D2D1::ColorF(1, 1, 1, 0.88f);
                        DrawChipBadge(dc, nullptr, sRc, L"", sFill, sRim,
                                      textPrimary, 14.0f);
                    }

                    for (int i = 0; i < static_cast<int>(g_spot.results.size());
                         ++i) {
                        const auto& r = g_spot.results[static_cast<size_t>(i)];
                        if (r.hitRect.bottom < g_spot.listViewportRect.top ||
                            r.hitRect.top > g_spot.listViewportRect.bottom) {
                            continue;
                        }

                        bool isSel = (!g_spot.calcSelected && i == g_spot.selected);
                        bool isHov = (i == g_spot.hoveredRow && !isSel);
                        if (isHov) {
                            DrawChipBadge(dc, nullptr, r.hitRect, L"",
                                          dark ? D2D1::ColorF(1, 1, 1, 0.08f)
                                               : D2D1::ColorF(1, 1, 1, 0.30f),
                                          D2D1::ColorF(0, 0, 0, 0), textPrimary,
                                          14.0f);
                        }

                        // Left 32x32 Icon / Thumbnail / Action Glyph
                        D2D1_RECT_F iconRc{r.hitRect.left + 12.0f,
                                           r.hitRect.top + 7.0f,
                                           r.hitRect.left + 44.0f,
                                           r.hitRect.top + 39.0f};
                        ID2D1Bitmap* bmp = nullptr;
                        if (r.appIdx >= 0 &&
                            r.appIdx < static_cast<int>(g_apps.size())) {
                            auto& app = g_apps[static_cast<size_t>(r.appIdx)];
                            bmp = GetCachedD2DBitmap(app.icon, dc);
                            if (!bmp && !app.iconTried) {
                                app.iconTried = true;
                                EnqueueMediaJob({MediaJobKind::AppIcon, r.appIdx,
                                                 g_indexGen,
                                                 !app.target.empty() ? app.target
                                                                     : app.launch,
                                                 app.id, 0});
                            }
                        } else if ((r.kind == ResultKind::Clipboard &&
                                    r.isImage) ||
                                   ((r.kind == ResultKind::File ||
                                     r.kind == ResultKind::RecentFile) &&
                                    !r.isDir && IsImagePreviewExt(r.ext))) {
                            auto it = g_thumbCache.find(r.payload);
                            if (it != g_thumbCache.end()) {
                                CachedBitmapSlot tmp{it->second.wic,
                                                     it->second.boundDeviceGen,
                                                     it->second.d2d};
                                bmp = GetCachedD2DBitmap(tmp, dc);
                                it->second.d2d = tmp.d2d;
                                it->second.boundDeviceGen = tmp.boundDeviceGen;
                            } else if (g_thumbPending.insert(r.payload).second) {
                                EnqueueMediaJob({MediaJobKind::FileThumb, -1, 0,
                                                 r.payload, L"", 0});
                            }
                        }

                        if (bmp) {
                            winrt::com_ptr<ID2D1RoundedRectangleGeometry> rg;
                            if (g_gfx.d2dFactory &&
                                SUCCEEDED(
                                    g_gfx.d2dFactory->CreateRoundedRectangleGeometry(
                                        D2D1::RoundedRect(iconRc, 7.0f, 7.0f),
                                        rg.put()))) {
                                dc->PushLayer(D2D1::LayerParameters(iconRc,
                                                                    rg.get()),
                                              nullptr);
                                dc->DrawBitmap(bmp, iconRc);
                                dc->PopLayer();
                            } else {
                                dc->DrawBitmap(bmp, iconRc);
                            }
                        } else {
                            DrawActionBadgeAndGlyph(dc, iconRc, r.glyph,
                                                    r.badgeColor);
                        }

                        // Right Badge Pill or Date/Size text
                        float rightEdge = r.hitRect.right - 14.0f;
                        if (!r.rightBadge.empty()) {
                            bool drawAsPill =
                                (isSel || r.kind == ResultKind::Action ||
                                 r.kind == ResultKind::Clipboard ||
                                 r.kind == ResultKind::Process ||
                                 r.pinned);
                            float bw = MeasureTextWidthDip(r.rightBadge,
                                                           fBadge.get(), 110.0f,
                                                           22.0f) +
                                       (drawAsPill ? 16.0f : 4.0f);
                            D2D1_RECT_F bRc{rightEdge - bw,
                                            r.hitRect.top + 12.0f, rightEdge,
                                            r.hitRect.bottom - 12.0f};
                            if (drawAsPill) {
                                DrawChipBadge(
                                    dc, fBadge.get(), bRc, r.rightBadge,
                                    dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                         : D2D1::ColorF(1, 1, 1, 0.78f),
                                    dark ? D2D1::ColorF(1, 1, 1, 0.18f)
                                         : D2D1::ColorF(0, 0, 0, 0.08f),
                                    textPrimary, 6.0f);
                            } else {
                                winrt::com_ptr<ID2D1SolidColorBrush> rb;
                                dc->CreateSolidColorBrush(textSecondary,
                                                          rb.put());
                                if (fSub && rb) {
                                    dc->DrawText(
                                        r.rightBadge.c_str(),
                                        static_cast<UINT32>(r.rightBadge.size()),
                                        fSub.get(), bRc, rb.get());
                                }
                            }
                            rightEdge -= (bw + 10.0f);
                        }

                        // Title + Subtitle
                        D2D1_RECT_F titleRc{iconRc.right + 12.0f,
                                            r.hitRect.top + 5.0f, rightEdge,
                                            r.hitRect.top + 25.0f};
                        DrawTitleWithHighlights(dc, fTitle.get(), r.title,
                                                r.matchMask, titleRc,
                                                textPrimary, accentCol);
                        if (!r.subtitle.empty()) {
                            winrt::com_ptr<ID2D1SolidColorBrush> subBr;
                            dc->CreateSolidColorBrush(textSecondary, subBr.put());
                            D2D1_RECT_F subRc{iconRc.right + 12.0f,
                                              r.hitRect.top + 24.0f, rightEdge,
                                              r.hitRect.bottom - 4.0f};
                            if (fSub && subBr) {
                                dc->DrawText(
                                    r.subtitle.c_str(),
                                    static_cast<UINT32>(r.subtitle.size()),
                                    fSub.get(), subRc, subBr.get());
                            }
                        }
                    }
                }
                dc->PopAxisAlignedClip();
            }

            // Right-Hand Quick Look & Actions Split Pane (when Alt+P open)
            if (g_ql.open && g_spot.qlPaneRect.right > g_spot.qlPaneRect.left) {
                if (divBr) {
                    dc->DrawLine({g_spot.qlPaneRect.left, hdr.top + 10.0f},
                                 {g_spot.qlPaneRect.left,
                                  g_spot.sheetRect.bottom - 10.0f},
                                 divBr.get(), 1.0f);
                }
                dc->PushAxisAlignedClip(g_spot.qlPaneRect,
                                        D2D1_ANTIALIAS_MODE_ALIASED);

                D2D1_RECT_F previewArea{g_spot.qlPaneRect.left + 14.0f,
                                        g_spot.qlPaneRect.top + 10.0f,
                                        g_spot.qlPaneRect.right - 14.0f,
                                        g_spot.qlPaneRect.top + 164.0f};
                DrawChipBadge(dc, nullptr, previewArea, L"",
                              dark ? D2D1::ColorF(1, 1, 1, 0.06f)
                                   : D2D1::ColorF(1, 1, 1, 0.36f),
                              dark ? D2D1::ColorF(1, 1, 1, 0.10f)
                                   : D2D1::ColorF(0, 0, 0, 0.06f),
                              textPrimary, 12.0f);

                const SearchResult* sel =
                    (g_spot.selected >= 0 &&
                     g_spot.selected < static_cast<int>(g_spot.results.size()))
                        ? &g_spot.results[static_cast<size_t>(g_spot.selected)]
                        : nullptr;

                ID2D1Bitmap* qlBmp = GetCachedD2DBitmap(g_ql.previewBmp, dc);
                if (g_ql.isImage && qlBmp) {
                    D2D1_SIZE_F bsz = qlBmp->GetSize();
                    float aw = previewArea.right - previewArea.left - 12.0f;
                    float ah = previewArea.bottom - previewArea.top - 12.0f;
                    float ratio = std::min(aw / std::max(1.0f, bsz.width),
                                           ah / std::max(1.0f, bsz.height));
                    float dw = bsz.width * ratio;
                    float dh = bsz.height * ratio;
                    float cxp = (previewArea.left + previewArea.right) * 0.5f;
                    float cyp = (previewArea.top + previewArea.bottom) * 0.5f;
                    dc->DrawBitmap(
                        qlBmp,
                        D2D1::RectF(cxp - dw * 0.5f, cyp - dh * 0.5f,
                                    cxp + dw * 0.5f, cyp + dh * 0.5f));
                } else if (g_ql.isText && !g_ql.lines.empty()) {
                    winrt::com_ptr<ID2D1SolidColorBrush> mBr;
                    dc->CreateSolidColorBrush(textPrimary, mBr.put());
                    float ly = previewArea.top + 8.0f;
                    int start = std::clamp(
                        g_ql.scroll, 0,
                        std::max(0, static_cast<int>(g_ql.lines.size()) - 1));
                    for (int li = start;
                         li < static_cast<int>(g_ql.lines.size()) &&
                         ly + 16.0f <= previewArea.bottom - 6.0f;
                         ++li) {
                        D2D1_RECT_F lr{previewArea.left + 10.0f, ly,
                                       previewArea.right - 10.0f, ly + 16.0f};
                        if (fMono && mBr) {
                            dc->DrawText(
                                g_ql.lines[static_cast<size_t>(li)].c_str(),
                                static_cast<UINT32>(
                                    g_ql.lines[static_cast<size_t>(li)].size()),
                                fMono.get(), lr, mBr.get());
                        }
                        ly += 16.0f;
                    }
                } else if (g_spot.hasCalc && g_spot.calcSelected) {
                    winrt::com_ptr<ID2D1SolidColorBrush> mBr;
                    dc->CreateSolidColorBrush(textPrimary, mBr.put());
                    D2D1_RECT_F tr{previewArea.left + 12.0f,
                                   previewArea.top + 12.0f,
                                   previewArea.right - 12.0f,
                                   previewArea.bottom - 12.0f};
                    if (fCalcBig && mBr) {
                        dc->DrawText(
                            g_spot.calc.display.c_str(),
                            static_cast<UINT32>(g_spot.calc.display.size()),
                            fCalcBig.get(), tr, mBr.get());
                    }
                } else if (sel) {
                    // Metadata + Large App Icon / Action Badge
                    float cxp = (previewArea.left + previewArea.right) * 0.5f;
                    D2D1_RECT_F bigIcon{cxp - 28.0f, previewArea.top + 16.0f,
                                        cxp + 28.0f, previewArea.top + 72.0f};
                    ID2D1Bitmap* ab = nullptr;
                    if (sel->appIdx >= 0 &&
                        sel->appIdx < static_cast<int>(g_apps.size())) {
                        ab = GetCachedD2DBitmap(
                            g_apps[static_cast<size_t>(sel->appIdx)].icon, dc);
                    }
                    if (ab) dc->DrawBitmap(ab, bigIcon);
                    else DrawActionBadgeAndGlyph(dc, bigIcon, sel->glyph,
                                                 sel->badgeColor);

                    winrt::com_ptr<ID2D1SolidColorBrush> pBr, sBr;
                    dc->CreateSolidColorBrush(textPrimary, pBr.put());
                    dc->CreateSolidColorBrush(textSecondary, sBr.put());
                    auto& fCtr = g_fonts.fCtr;
                    auto& fSubC = g_fonts.fSubC;
                    if (fCtr && pBr) {
                        dc->DrawText(
                            sel->title.c_str(),
                            static_cast<UINT32>(sel->title.size()), fCtr.get(),
                            D2D1::RectF(previewArea.left + 10.0f,
                                        bigIcon.bottom + 8.0f,
                                        previewArea.right - 10.0f,
                                        bigIcon.bottom + 28.0f),
                            pBr.get());
                    }
                    std::wstring meta =
                        !sel->payload.empty() ? sel->payload : sel->subtitle;
                    if (fSubC && sBr && !meta.empty()) {
                        dc->DrawText(
                            meta.c_str(), static_cast<UINT32>(meta.size()),
                            fSubC.get(),
                            D2D1::RectF(previewArea.left + 10.0f,
                                        bigIcon.bottom + 30.0f,
                                        previewArea.right - 10.0f,
                                        previewArea.bottom - 8.0f),
                            sBr.get());
                    }
                }

                // Quick Look Interactive Actions List
                g_ql.actionCount =
                    std::min(8, static_cast<int>(g_spot.qlActions.size()));
                float ay = previewArea.bottom + 12.0f;
                for (int ai = 0; ai < g_ql.actionCount; ++ai) {
                    const auto& qa = g_spot.qlActions[static_cast<size_t>(ai)];
                    D2D1_RECT_F aRc{g_spot.qlPaneRect.left + 14.0f, ay,
                                    g_spot.qlPaneRect.right - 14.0f, ay + 28.0f};
                    g_ql.actionRects[ai] = aRc;
                    bool hov = (ai == g_ql.hoveredAction);
                    D2D1_COLOR_F aBg =
                        hov ? (dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                    : D2D1::ColorF(1, 1, 1, 0.62f))
                            : (dark ? D2D1::ColorF(1, 1, 1, 0.05f)
                                    : D2D1::ColorF(1, 1, 1, 0.25f));
                    DrawChipBadge(dc, nullptr, aRc, L"", aBg,
                                  D2D1::ColorF(0, 0, 0, 0), textPrimary, 8.0f);
                    winrt::com_ptr<ID2D1SolidColorBrush> lb, kb;
                    dc->CreateSolidColorBrush(
                        qa.destructive ? D2D1::ColorF(0.95f, 0.36f, 0.36f, 1.0f)
                                       : textPrimary,
                        lb.put());
                    dc->CreateSolidColorBrush(textSecondary, kb.put());
                    if (fTitle && lb) {
                        dc->DrawText(
                            qa.label.c_str(),
                            static_cast<UINT32>(qa.label.size()), fSub.get(),
                            D2D1::RectF(aRc.left + 10.0f, aRc.top,
                                        aRc.right - 60.0f, aRc.bottom),
                            lb.get());
                    }
                    if (!qa.shortcut.empty() && fBadge && kb) {
                        dc->DrawText(
                            qa.shortcut.c_str(),
                            static_cast<UINT32>(qa.shortcut.size()),
                            fFooterR.get(),
                            D2D1::RectF(aRc.right - 66.0f, aRc.top,
                                        aRc.right - 10.0f, aRc.bottom),
                            kb.get());
                    }
                    ay += 32.0f;
                    if (ay + 28.0f > g_spot.qlPaneRect.bottom) {
                        g_ql.actionCount = ai + 1;
                        break;
                    }
                }
                dc->PopAxisAlignedClip();
            }

            // Footer Bar
            D2D1_RECT_F footRc{g_spot.sheetRect.left + 16.0f,
                               g_spot.sheetRect.bottom - kFooterH,
                               g_spot.sheetRect.right - 16.0f,
                               g_spot.sheetRect.bottom};
            if (divBr) {
                dc->DrawLine({g_spot.sheetRect.left + 12.0f, footRc.top},
                             {g_spot.sheetRect.right - 12.0f, footRc.top},
                             divBr.get(), 1.0f);
            }
            winrt::com_ptr<ID2D1SolidColorBrush> footBr, toastBr;
            dc->CreateSolidColorBrush(textSecondary, footBr.put());
            dc->CreateSolidColorBrush(accentCol, toastBr.put());

            bool showToast = !g_spot.toastMsg.empty() &&
                             GetTickCount64() < g_spot.toastUntilMs;
            std::wstring leftInfo;
            if (showToast) {
                leftInfo = g_spot.toastMsg;
            } else {
                leftInfo =
                    std::to_wstring(g_spot.results.size()) + L" results";
                if (g_settings.enableEverything) {
                    ULONGLONG nowMs = GetTickCount64();
                    if (nowMs - g_spot.evAliveCheckedMs >= 2000) {
                        g_spot.evAliveCheckedMs = nowMs;
                        g_spot.evAliveCached =
                            FindWindowW(kEverythingWndClass, nullptr) != nullptr;
                    }
                    if (g_spot.evAliveCached) {
                        leftInfo += L" \u00B7 Everything";
                    }
                }
            }
            if (fFooter) {
                dc->DrawText(
                    leftInfo.c_str(), static_cast<UINT32>(leftInfo.size()),
                    fFooter.get(), footRc,
                    showToast ? toastBr.get() : footBr.get());
            }
            std::wstring rightHints =
                L"\u21B5 Open   \u00B7   Alt+P Preview   \u00B7   Tab Mode   \u00B7   F1 Shortcuts";
            if (fFooterR && footBr) {
                dc->DrawText(rightHints.c_str(),
                             static_cast<UINT32>(rightHints.size()),
                             fFooterR.get(), footRc, footBr.get());
            }
            dc->PopAxisAlignedClip();
        }

        // 4. Optional F1 Shortcuts Cheat-Sheet Modal
        if (g_spot.helpOpen && g_spot.expanded) {
            D2D1_RECT_F modal{g_spot.sheetRect.left + 32.0f,
                              g_spot.sheetRect.top + 32.0f,
                              g_spot.sheetRect.right - 32.0f,
                              g_spot.sheetRect.bottom - 32.0f};
            DrawChipBadge(
                dc, nullptr, modal, L"",
                dark ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 0.94f)
                     : D2D1::ColorF(0.98f, 0.98f, 1.0f, 0.96f),
                dark ? D2D1::ColorF(1, 1, 1, 0.24f)
                     : D2D1::ColorF(0, 0, 0, 0.14f),
                textPrimary, 16.0f);
            static const std::pair<const wchar_t*, const wchar_t*> kHelp[] = {
                {L"Alt+Space", L"Open or close Spotlight"},
                {L"Tab / Ctrl+1..7", L"Accept autocomplete or switch mode"},
                {L"Alt+P / Ctrl+I", L"Toggle Quick Look & Actions split pane"},
                {L"Ctrl+\u21B5 / Alt+\u21B5", L"Run elevated / Open in Explorer"},
                {L"Ctrl+Shift+C", L"Copy path or result to clipboard"},
                {L"kl / win: / bm: / cfg:", L"Kill process, Windows, Bookmarks, Settings"},
                {L"=", L"Force Calculator & Unit Converter"},
                {L"Esc / F1", L"Close cheat-sheet or dismiss Spotlight"},
            };
            float hy = modal.top + 18.0f;
            winrt::com_ptr<ID2D1SolidColorBrush> kBr, dBr;
            dc->CreateSolidColorBrush(accentCol, kBr.put());
            dc->CreateSolidColorBrush(textPrimary, dBr.put());
            for (const auto& [hk, desc] : kHelp) {
                if (hy + 26.0f > modal.bottom) break;
                if (fTitle && kBr) {
                    dc->DrawText(hk, static_cast<UINT32>(wcslen(hk)),
                                 fTitle.get(),
                                 D2D1::RectF(modal.left + 20.0f, hy,
                                             modal.left + 220.0f, hy + 24.0f),
                                 kBr.get());
                }
                if (fSub && dBr) {
                    dc->DrawText(desc, static_cast<UINT32>(wcslen(desc)),
                                 fSub.get(),
                                 D2D1::RectF(modal.left + 230.0f, hy,
                                             modal.right - 20.0f, hy + 24.0f),
                                 dBr.get());
                }
                hy += 28.0f;
            }
        }
    });
}


// ============================================================================
// 7. Window Input, IME, Hotkey / LL Hook, Host Dispatch & Windhawk Tool Entry
// ============================================================================

inline bool PtInRectDip(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

std::wstring CurrentClipboardText() {
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT) || !OpenClipboard(g_spot.hwnd)) {
        return {};
    }
    std::wstring res;
    if (HANDLE h = GetClipboardData(CF_UNICODETEXT)) {
        if (const wchar_t* p = static_cast<const wchar_t*>(GlobalLock(h))) {
            res = p;
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return res;
}

void DeleteQuerySelectionOrChar(bool forward, bool wholeWord) {
    int a = std::min(g_spot.caret, g_spot.selAnchor);
    int b = std::max(g_spot.caret, g_spot.selAnchor);
    if (b > a) {
        g_spot.query.erase(static_cast<size_t>(a), static_cast<size_t>(b - a));
        g_spot.caret = a;
        g_spot.selAnchor = a;
        return;
    }
    if (!forward) {
        if (g_spot.caret <= 0) {
            if (g_spot.mode != SpotlightMode::All) {
                g_spot.mode = SpotlightMode::All;
                g_spot.fileTab = FileSubTab::All;
                TriggerSpotlightRebuild(false);
            }
            return;
        }
        int start = g_spot.caret - 1;
        if (wholeWord) {
            while (start > 0 && iswspace(g_spot.query[static_cast<size_t>(start)])) {
                --start;
            }
            while (start > 0 &&
                   !iswspace(g_spot.query[static_cast<size_t>(start - 1)])) {
                --start;
            }
        }
        g_spot.query.erase(static_cast<size_t>(start),
                           static_cast<size_t>(g_spot.caret - start));
        g_spot.caret = start;
        g_spot.selAnchor = start;
    } else if (g_spot.caret < static_cast<int>(g_spot.query.size())) {
        int end = g_spot.caret + 1;
        if (wholeWord) {
            int n = static_cast<int>(g_spot.query.size());
            while (end < n && iswspace(g_spot.query[static_cast<size_t>(end)])) {
                ++end;
            }
            while (end < n && !iswspace(g_spot.query[static_cast<size_t>(end)])) {
                ++end;
            }
        }
        g_spot.query.erase(static_cast<size_t>(g_spot.caret),
                           static_cast<size_t>(end - g_spot.caret));
        g_spot.selAnchor = g_spot.caret;
    }
}

void InsertTextAtCaret(const std::wstring& txt) {
    if (txt.empty()) return;
    int a = std::min(g_spot.caret, g_spot.selAnchor);
    int b = std::max(g_spot.caret, g_spot.selAnchor);
    g_spot.query.replace(static_cast<size_t>(a), static_cast<size_t>(b - a),
                         txt);
    g_spot.caret = a + static_cast<int>(txt.size());
    g_spot.selAnchor = g_spot.caret;
    g_spot.expanded = true;
    g_spot.caretOn = true;
    g_spot.selected = 0;
    g_spot.calcSelected = false;
    TriggerSpotlightRebuild(true);
    EnsureAnimTimerRunning();
}

void EnsureSelectionVisible() {
    if (g_spot.results.empty()) return;
    float vh = g_spot.listViewportRect.bottom - g_spot.listViewportRect.top;
    if (vh <= 10.0f) vh = kMaxListH;
    float rowTop = static_cast<float>(g_spot.selected) * kRowH;
    float rowBot = rowTop + kRowH;
    if (rowTop < g_spot.targetScrollY) g_spot.targetScrollY = rowTop;
    else if (rowBot > g_spot.targetScrollY + vh) {
        g_spot.targetScrollY = rowBot - vh;
    }
    float maxScroll = std::max(
        0.0f, static_cast<float>(g_spot.results.size()) * kRowH - vh);
    g_spot.targetScrollY = std::clamp(g_spot.targetScrollY, 0.0f, maxScroll);
}

void HideSpotlight(bool restoreFocus) {
    g_spot.visible = false;
    g_spot.isOpening = false;
    g_spot.openAlphaTarget = 0.0f;
    g_spot.helpOpen = false;
    g_spot.imeComposing = false;
    g_spot.imeComp.clear();
    SetLiquidCaptureWatching(false, false);
    CancelPendingFileSearch();
    ClearMediaQueue();
    if (g_spot.hwnd && IsWindow(g_spot.hwnd)) {
        KillTimer(g_spot.hwnd, kTimerFrame);
        g_spot.animTimerActive = false;
        KillTimer(g_spot.hwnd, kTimerBlink);
        KillTimer(g_spot.hwnd, kTimerSearchDebounce);
        KillTimer(g_spot.hwnd, kTimerGlassMaintenance);
        ShowWindow(g_spot.hwnd, SW_HIDE);
    }
    if (restoreFocus && g_spot.prevForeground && IsWindow(g_spot.prevForeground)) {
        FocusOrSwitchWindow(g_spot.prevForeground);
    }
}

void OpenSpotlight(SpotlightMode initialMode) {
    if (!g_spot.hwnd || !IsWindow(g_spot.hwnd)) return;
    if (g_settings.disableInFullscreen && IsForegroundFullscreen()) return;

    g_spot.isOpening = true;
    g_spot.prevForeground = GetForegroundWindow();
    g_spot.monitor = ResolveTargetMonitor();
    g_spot.scale = GetMonitorDpiScale(g_spot.monitor);
    g_spot.darkMode = IsSystemDarkMode();

    g_spot.visible = true;
    g_spot.mode = initialMode;
    g_spot.fileTab = FileSubTab::All;
    g_spot.expanded = (initialMode != SpotlightMode::All) ||
                      g_settings.showQuickLookByDefault;
    g_spot.expandProg = g_spot.expanded ? 1.0f : 0.0f;
    g_spot.expandVel = 0.0f;
    g_spot.openAlpha = 0.0f;
    g_spot.openAlphaTarget = 1.0f;
    g_spot.openScale = 0.0f;
    g_spot.helpOpen = false;

    g_spot.query.clear();
    g_spot.effectiveQuery.clear();
    g_spot.prefixChipLabel.clear();
    g_spot.detectedPrefixMode = SpotlightMode::All;
    g_spot.caret = 0;
    g_spot.selAnchor = 0;
    g_spot.caretOn = true;
    g_spot.imeComposing = false;
    g_spot.imeComp.clear();

    g_spot.selected = 0;
    g_spot.calcSelected = false;
    g_spot.hoveredRow = -1;
    g_spot.hoveredOrb = -1;
    g_spot.hoveredTab = -1;
    g_spot.scrollY = 0.0f;
    g_spot.targetScrollY = 0.0f;
    g_spot.selYInit = false;
    g_spot.armedActionKey.clear();
    g_spot.armedUntilMs = 0;
    for (float& hv : g_spot.orbHoverT) hv = 0.0f;

    g_ql.open = g_settings.showQuickLookByDefault;

    if ((g_menuDirty || !g_indexedOnce) && !g_indexing) StartIndex();
    RefreshRecentIfStale();
    RefreshBookmarksIfStale();
    if (initialMode == SpotlightMode::Windows) {
        RefreshWindowsSnapshot(g_spot.hwnd);
    }

    TriggerSpotlightRebuild(initialMode == SpotlightMode::Files);
    SyncGlassVisualTree();
    RenderSpotlightSurface();

    SetWindowPos(
        g_spot.hwnd, HWND_TOPMOST, g_spot.windowPx.left, g_spot.windowPx.top,
        g_spot.windowPx.right - g_spot.windowPx.left,
        g_spot.windowPx.bottom - g_spot.windowPx.top,
        SWP_SHOWWINDOW | SWP_NOACTIVATE);

    if ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) {
        INPUT mask[2]{};
        mask[0].type = INPUT_KEYBOARD;
        mask[0].ki.wVk = 0xFC;  // VK_NONAME cancels Alt menu activation cleanly
        mask[1].type = INPUT_KEYBOARD;
        mask[1].ki.wVk = 0xFC;
        mask[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, mask, sizeof(INPUT));
    }

    // Ensure clean keyboard focus even when invoked across threads
    HWND fg = GetForegroundWindow();
    DWORD fgTid = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    DWORD curTid = GetCurrentThreadId();
    if (fgTid && fgTid != curTid) AttachThreadInput(curTid, fgTid, TRUE);
    BringWindowToTop(g_spot.hwnd);
    SetForegroundWindow(g_spot.hwnd);
    SetFocus(g_spot.hwnd);
    if (fgTid && fgTid != curTid) AttachThreadInput(curTid, fgTid, FALSE);

    g_liquidCapture.highRate.store(true, std::memory_order_relaxed);
    EnsureAnimTimerRunning();
    SetTimer(g_spot.hwnd, kTimerBlink, 520, nullptr);
    SetTimer(g_spot.hwnd, kTimerGlassMaintenance, 350, nullptr);
    g_spot.isOpening = false;
}

void ToggleSpotlight(SpotlightMode mode) {
    if (g_spot.visible) {
        if (mode != SpotlightMode::All && mode != EffectiveMode()) {
            g_spot.mode = mode;
            g_spot.expanded = true;
            g_spot.selected = 0;
            TriggerSpotlightRebuild(mode == SpotlightMode::Files);
            EnsureAnimTimerRunning();
            SyncGlassVisualTree();
            RenderSpotlightSurface();
            return;
        }
        HideSpotlight(true);
    } else {
        OpenSpotlight(mode);
    }
}

void ShowResultContextMenu(const SearchResult& r, POINT ptScreen) {
    auto items = BuildQuickActionsForResult(&r);
    if (items.empty()) return;
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        AppendMenuW(menu, MF_STRING, static_cast<UINT_PTR>(100 + i),
                    items[static_cast<size_t>(i)].label.c_str());
    }
    int cmd = TrackPopupMenuEx(
        menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, ptScreen.x,
        ptScreen.y, g_spot.hwnd, nullptr);
    DestroyMenu(menu);
    if (cmd >= 100 && cmd < 100 + static_cast<int>(items.size())) {
        ExecuteQuickAction(
            r, items[static_cast<size_t>(cmd - 100)].actionCode);
    }
}

void StepAnimations() {
    constexpr float dt = 0.016f;
    bool rootAnim = false;
    bool glassAnim = false;
    bool uiAnim = false;

    rootAnim |= SmoothExp(g_spot.openAlpha, g_spot.openAlphaTarget, 22.0f, dt,
                          0.004f);
    rootAnim |= SmoothExp(g_spot.openScale, 1.0f, 20.0f, dt, 0.004f);

    float targetExp = g_spot.expanded ? 1.0f : 0.0f;
    if (StepCriticallyDampedSpring(g_spot.expandProg, g_spot.expandVel, targetExp,
                                   26.0f, dt)) {
        glassAnim = true;
    }

    for (int i = 0; i < 4; ++i) {
        float target = (g_spot.hoveredOrb == i) ? 1.0f : 0.0f;
        glassAnim |= SmoothExp(g_spot.orbHoverT[i], target, 18.0f, dt, 0.004f);
    }

    uiAnim |= SmoothExp(g_spot.scrollY, g_spot.targetScrollY, 20.0f, dt, 0.2f);

    if (!g_spot.results.empty() && !g_spot.calcSelected) {
        float selTarget = static_cast<float>(g_spot.selected) * kRowH - g_spot.scrollY;
        if (!g_spot.selYInit) {
            g_spot.selYCur = selTarget;
            g_spot.selYVel = 0.0f;
            g_spot.selYInit = true;
        } else if (StepCriticallyDampedSpring(g_spot.selYCur, g_spot.selYVel,
                                              selTarget, 30.0f, dt)) {
            uiAnim = true;
        }
    }

    if (g_spot.visible) {
        if (glassAnim) {
            SyncGlassVisualTree();
            RenderSpotlightSurface();
        } else {
            if (rootAnim && g_spot.rootVisual) {
                g_spot.rootVisual.Opacity(Clamp01(g_spot.openAlpha));
                float sc = Lerp(0.95f, 1.0f, Clamp01(g_spot.openScale));
                g_spot.rootVisual.Scale({sc, sc, 1.0f});
            }
            if (uiAnim || g_spot.needsRedraw) {
                UpdateRowHitRectsOnly();
                RenderSpotlightSurface();
            }
        }
    }

    bool anyAnim = rootAnim || glassAnim || uiAnim;
    g_liquidCapture.highRate.store(anyAnim, std::memory_order_relaxed);
    if (!anyAnim && !g_spot.needsRedraw && g_spot.hwnd) {
        KillTimer(g_spot.hwnd, kTimerFrame);
        g_spot.animTimerActive = false;
    }
}

LRESULT CALLBACK SpotlightUiWndProc(HWND hwnd,
                                    UINT msg,
                                    WPARAM wp,
                                    LPARAM lp) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE && g_spot.visible &&
                !g_spot.isOpening) {
                // Keep alive while context popup menu is owned by us
                DWORD fgPid = 0;
                if (HWND fg = reinterpret_cast<HWND>(lp)) {
                    GetWindowThreadProcessId(fg, &fgPid);
                }
                if (fgPid != GetCurrentProcessId()) HideSpotlight(false);
            }
            return 0;

        case WM_DISPLAYCHANGE:
            StopLiquidCaptureWorker();
            g_liquidCapture.desktopTex = nullptr;
            g_liquidCapture.retryAtMs = 0;
            if (g_spot.visible) {
                SyncGlassVisualTree();
                RenderSpotlightSurface();
            }
            return 0;

        case WM_NCHITTEST: {
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(hwnd, &pt);
            float mx = static_cast<float>(pt.x) / g_spot.scale;
            float my = static_cast<float>(pt.y) / g_spot.scale;
            if (!g_spot.expanded && g_spot.expandProg < 0.08f) {
                if (PtInRectDip(g_spot.pillRect, mx, my)) return HTCLIENT;
                for (int i = 0; i < 4; ++i) {
                    if (PtInRectDip(g_spot.orbRects[i], mx, my)) return HTCLIENT;
                }
                return HTTRANSPARENT;
            }
            return PtInRectDip(g_spot.sheetRect, mx, my) ? HTCLIENT
                                                         : HTTRANSPARENT;
        }

        case WM_SYSCOMMAND:
            if ((wp & 0xFFF0) == SC_KEYMENU) return 0;
            break;

        case WM_TIMER:
            if (wp == kTimerFrame) {
                if (g_spot.visible) StepAnimations();
                return 0;
            }
            if (wp == kTimerBlink) {
                g_spot.caretOn = !g_spot.caretOn;
                g_spot.needsRedraw = true;
                if (g_spot.visible) RenderSpotlightSurface();
                return 0;
            }
            if (wp == kTimerGlassMaintenance) {
                if (g_spot.visible) {
                    if (g_liquidCapture.desktopTex) {
                        if (g_liquidCapture.framePosted.exchange(false)) {
                            RefreshGlassBackdropOnly();
                        }
                        KillTimer(hwnd, kTimerGlassMaintenance);
                    } else {
                        g_liquidCapture.framePosted.store(false);
                        RefreshGlassBackdropOnly();
                    }
                }
                return 0;
            }
            if (wp == kTimerSearchDebounce) {
                KillTimer(hwnd, kTimerSearchDebounce);
                StartAsyncFileSearch(g_spot.effectiveQuery, EffectiveMode(),
                                     g_spot.fileTab);
                return 0;
            }
            if (wp == kTimerConfirmReset) {
                KillTimer(hwnd, kTimerConfirmReset);
                g_spot.armedActionKey.clear();
                g_spot.armedUntilMs = 0;
                return 0;
            }
            if (wp == kTimerToastHide) {
                KillTimer(hwnd, kTimerToastHide);
                g_spot.toastMsg.clear();
                if (g_spot.visible) RenderSpotlightSurface();
                return 0;
            }
            break;

        case WM_IME_STARTCOMPOSITION:
            g_spot.imeComposing = true;
            g_spot.imeComp.clear();
            g_spot.imeCompCursor = 0;
            g_spot.expanded = true;
            return 0;

        case WM_IME_COMPOSITION: {
            HIMC imm = ImmGetContext(hwnd);
            if (imm) {
                if (lp & GCS_COMPSTR) {
                    LONG bytes = ImmGetCompositionStringW(imm, GCS_COMPSTR,
                                                          nullptr, 0);
                    if (bytes >= 0) {
                        std::wstring s(static_cast<size_t>(bytes) / 2, L'\0');
                        if (bytes > 0) {
                            ImmGetCompositionStringW(imm, GCS_COMPSTR, s.data(),
                                                     static_cast<DWORD>(bytes));
                        }
                        g_spot.imeComp = std::move(s);
                        g_spot.imeCompCursor = static_cast<int>(
                            ImmGetCompositionStringW(imm, GCS_CURSORPOS, nullptr,
                                                     0));
                    }
                    RenderSpotlightSurface();
                }
                if (lp & GCS_RESULTSTR) {
                    LONG bytes = ImmGetCompositionStringW(imm, GCS_RESULTSTR,
                                                          nullptr, 0);
                    if (bytes > 0) {
                        std::wstring s(static_cast<size_t>(bytes) / 2, L'\0');
                        ImmGetCompositionStringW(imm, GCS_RESULTSTR, s.data(),
                                                 static_cast<DWORD>(bytes));
                        g_spot.imeComposing = false;
                        g_spot.imeComp.clear();
                        InsertTextAtCaret(s);
                    }
                }
                ImmReleaseContext(hwnd, imm);
                return 0;
            }
            break;
        }

        case WM_IME_ENDCOMPOSITION:
            g_spot.imeComposing = false;
            g_spot.imeComp.clear();
            RenderSpotlightSurface();
            return 0;

        case WM_CHAR: {
            wchar_t ch = static_cast<wchar_t>(wp);
            if (ch == 127 || (GetKeyState(VK_CONTROL) & 0x8000)) return 0;
            if (ch >= 0x20) {
                std::wstring s(1, ch);
                InsertTextAtCaret(s);
                SyncGlassVisualTree();
                RenderSpotlightSurface();
            }
            return 0;
        }

        case WM_SYSCHAR:
        case WM_SYSKEYUP:
            return 0;

        case WM_SYSKEYDOWN:
            if (wp == VK_F4) {
                return DefWindowProcW(hwnd, msg, wp, lp);
            }
            [[fallthrough]];

        case WM_KEYDOWN: {
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
            g_spot.caretOn = true;

            if (wp == VK_F1) {
                g_spot.helpOpen = !g_spot.helpOpen;
                g_spot.expanded = true;
                EnsureAnimTimerRunning();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }
            if (wp == VK_ESCAPE) {
                if (g_spot.helpOpen) {
                    g_spot.helpOpen = false;
                } else if (!g_spot.query.empty()) {
                    g_spot.query.clear();
                    g_spot.caret = 0;
                    g_spot.selAnchor = 0;
                    TriggerSpotlightRebuild(false);
                } else if (g_spot.mode != SpotlightMode::All) {
                    g_spot.mode = SpotlightMode::All;
                    g_spot.fileTab = FileSubTab::All;
                    TriggerSpotlightRebuild(false);
                } else {
                    HideSpotlight(true);
                    return 0;
                }
                EnsureAnimTimerRunning();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }
            if (g_spot.helpOpen) {
                g_spot.helpOpen = false;
                RenderSpotlightSurface();
                return 0;
            }

            if (wp == VK_RETURN) {
                if (!g_spot.expanded) {
                    g_spot.expanded = true;
                    TriggerSpotlightRebuild(false);
                    EnsureAnimTimerRunning();
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                    return 0;
                }
                ExecutePrimaryAction(ctrl, alt, false);
                return 0;
            }

            if (wp == VK_TAB) {
                if (!g_spot.expanded) {
                    g_spot.expanded = true;
                    TriggerSpotlightRebuild(false);
                } else if (!g_spot.autoFillGhost.empty() &&
                           g_spot.caret == static_cast<int>(g_spot.query.size()) &&
                           g_spot.selAnchor == g_spot.caret) {
                    g_spot.query = g_spot.autoFillFull;
                    g_spot.caret = static_cast<int>(g_spot.query.size());
                    g_spot.selAnchor = g_spot.caret;
                    TriggerSpotlightRebuild(true);
                } else {
                    static constexpr SpotlightMode kCycle[] = {
                        SpotlightMode::All, SpotlightMode::Apps,
                        SpotlightMode::Files, SpotlightMode::Actions,
                        SpotlightMode::Clipboard, SpotlightMode::Windows,
                        SpotlightMode::Settings};
                    int idx = 0;
                    for (int i = 0; i < 7; ++i) {
                        if (kCycle[i] == g_spot.mode) {
                            idx = i;
                            break;
                        }
                    }
                    int nxt = (idx + (shift ? 6 : 1)) % 7;
                    g_spot.mode = kCycle[nxt];
                    g_spot.fileTab = FileSubTab::All;
                    g_spot.selected = 0;
                    if (g_spot.mode == SpotlightMode::Windows) {
                        RefreshWindowsSnapshot(hwnd);
                    }
                    TriggerSpotlightRebuild(g_spot.mode == SpotlightMode::Files);
                }
                EnsureAnimTimerRunning();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }

            if (ctrl && wp >= '1' && wp <= '7') {
                static constexpr SpotlightMode kMap[] = {
                    SpotlightMode::All, SpotlightMode::Apps,
                    SpotlightMode::Files, SpotlightMode::Actions,
                    SpotlightMode::Clipboard, SpotlightMode::Windows,
                    SpotlightMode::Settings};
                g_spot.expanded = true;
                g_spot.mode = kMap[wp - '1'];
                g_spot.fileTab = FileSubTab::All;
                g_spot.selected = 0;
                if (g_spot.mode == SpotlightMode::Windows) {
                    RefreshWindowsSnapshot(hwnd);
                }
                TriggerSpotlightRebuild(g_spot.mode == SpotlightMode::Files);
                EnsureAnimTimerRunning();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }

            if ((alt && wp == 'P') || (ctrl && wp == 'I')) {
                g_spot.expanded = true;
                g_ql.open = !g_ql.open;
                SyncQuickLookToSelection();
                EnsureAnimTimerRunning();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }

            if (ctrl && wp == 'P') {
                if (g_spot.selected >= 0 &&
                    g_spot.selected < static_cast<int>(g_spot.results.size())) {
                    const auto& r =
                        g_spot.results[static_cast<size_t>(g_spot.selected)];
                    if (r.kind == ResultKind::App) {
                        ExecuteQuickAction(r, QA_TogglePinSpot);
                    } else if (r.kind == ResultKind::Clipboard) {
                        ExecuteQuickAction(r, QA_PinClip);
                    }
                }
                return 0;
            }

            if (ctrl && wp == 'R') {
                StartIndex();
                ShowToast(L"Reindexing applications\u2026");
                RenderSpotlightSurface();
                return 0;
            }

            if (ctrl && wp == 'A') {
                g_spot.selAnchor = 0;
                g_spot.caret = static_cast<int>(g_spot.query.size());
                RenderSpotlightSurface();
                return 0;
            }

            if (ctrl && wp == 'C') {
                int a = std::min(g_spot.caret, g_spot.selAnchor);
                int b = std::max(g_spot.caret, g_spot.selAnchor);
                if (!shift && b > a) {
                    CopyTextToClipboard(
                        g_spot.query.substr(static_cast<size_t>(a),
                                            static_cast<size_t>(b - a)));
                    ShowToast(L"Copied text");
                    RenderSpotlightSurface();
                } else {
                    ExecutePrimaryAction(false, false, true);
                    RenderSpotlightSurface();
                }
                return 0;
            }

            if (ctrl && wp == 'V') {
                std::wstring paste = CurrentClipboardText();
                for (wchar_t& ch : paste) {
                    if (ch == L'\r' || ch == L'\n') ch = L' ';
                }
                InsertTextAtCaret(paste);
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }

            if (wp == VK_BACK) {
                DeleteQuerySelectionOrChar(false, ctrl);
                TriggerSpotlightRebuild(true);
                EnsureAnimTimerRunning();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }
            if (wp == VK_DELETE) {
                if (ctrl && g_spot.selected >= 0 &&
                    g_spot.selected < static_cast<int>(g_spot.results.size())) {
                    const auto& r =
                        g_spot.results[static_cast<size_t>(g_spot.selected)];
                    if (r.kind == ResultKind::Clipboard) {
                        ExecuteQuickAction(r, QA_DeleteClip);
                    } else if (r.kind == ResultKind::Window) {
                        ExecuteQuickAction(r, QA_CloseWindow);
                    }
                    return 0;
                }
                DeleteQuerySelectionOrChar(true, ctrl);
                TriggerSpotlightRebuild(true);
                EnsureAnimTimerRunning();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }

            if (wp == VK_LEFT || wp == VK_RIGHT || wp == VK_HOME || wp == VK_END) {
                if (wp == VK_RIGHT && !shift && !ctrl &&
                    !g_spot.autoFillGhost.empty() &&
                    g_spot.caret == static_cast<int>(g_spot.query.size()) &&
                    g_spot.selAnchor == g_spot.caret) {
                    g_spot.query = g_spot.autoFillFull;
                    g_spot.caret = static_cast<int>(g_spot.query.size());
                    g_spot.selAnchor = g_spot.caret;
                    TriggerSpotlightRebuild(true);
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                    return 0;
                }
                int len = static_cast<int>(g_spot.query.size());
                if (!shift && g_spot.caret != g_spot.selAnchor) {
                    int pos = (wp == VK_LEFT || wp == VK_HOME)
                                  ? std::min(g_spot.caret, g_spot.selAnchor)
                                  : std::max(g_spot.caret, g_spot.selAnchor);
                    g_spot.caret = pos;
                    g_spot.selAnchor = pos;
                } else {
                    if (wp == VK_HOME) g_spot.caret = 0;
                    else if (wp == VK_END) g_spot.caret = len;
                    else if (wp == VK_LEFT && g_spot.caret > 0) {
                        --g_spot.caret;
                        if (ctrl) {
                            while (g_spot.caret > 0 &&
                                   iswspace(g_spot.query[static_cast<size_t>(
                                       g_spot.caret)])) {
                                --g_spot.caret;
                            }
                            while (g_spot.caret > 0 &&
                                   !iswspace(g_spot.query[static_cast<size_t>(
                                       g_spot.caret - 1)])) {
                                --g_spot.caret;
                            }
                        }
                    } else if (wp == VK_RIGHT && g_spot.caret < len) {
                        ++g_spot.caret;
                        if (ctrl) {
                            while (g_spot.caret < len &&
                                   !iswspace(g_spot.query[static_cast<size_t>(
                                       g_spot.caret)])) {
                                ++g_spot.caret;
                            }
                            while (g_spot.caret < len &&
                                   iswspace(g_spot.query[static_cast<size_t>(
                                       g_spot.caret)])) {
                                ++g_spot.caret;
                            }
                        }
                    }
                    if (!shift) g_spot.selAnchor = g_spot.caret;
                }
                RenderSpotlightSurface();
                return 0;
            }

            if (wp == VK_DOWN || wp == VK_UP || wp == VK_NEXT || wp == VK_PRIOR) {
                if (!g_spot.expanded) {
                    g_spot.expanded = true;
                    TriggerSpotlightRebuild(false);
                    EnsureAnimTimerRunning();
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                    return 0;
                }
                int n = static_cast<int>(g_spot.results.size());
                int step = (wp == VK_NEXT || wp == VK_PRIOR) ? 5 : 1;
                if (wp == VK_DOWN || wp == VK_NEXT) {
                    if (g_spot.calcSelected) {
                        if (n > 0) {
                            g_spot.calcSelected = false;
                            g_spot.selected = 0;
                        }
                    } else if (n > 0) {
                        g_spot.selected = std::min(n - 1, g_spot.selected + step);
                    }
                } else {
                    if (!g_spot.calcSelected && g_spot.selected > 0) {
                        g_spot.selected = std::max(0, g_spot.selected - step);
                    } else if (g_spot.hasCalc) {
                        g_spot.calcSelected = true;
                    }
                }
                EnsureSelectionVisible();
                SyncQuickLookToSelection();
                EnsureAnimTimerRunning();
                RenderSpotlightSurface();
                return 0;
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            float mx = static_cast<float>(GET_X_LPARAM(lp)) / g_spot.scale;
            float my = static_cast<float>(GET_Y_LPARAM(lp)) / g_spot.scale;

            int newOrb = -1;
            int newRow = -1;
            int newTab = -1;
            int newQlAct = -1;
            bool newChipClose = false;
            bool newQlBtn = false;

            if (!g_spot.expanded && g_spot.expandProg < 0.08f) {
                for (int i = 0; i < 4; ++i) {
                    if (PtInRectDip(g_spot.orbRects[i], mx, my)) {
                        newOrb = i;
                        break;
                    }
                }
            } else {
                newChipClose = PtInRectDip(g_spot.chipCloseRect, mx, my);
                newQlBtn = PtInRectDip(g_spot.quickLookToggleRect, mx, my);
                for (int i = 0; i < static_cast<int>(g_spot.filterTabs.size());
                     ++i) {
                    if (PtInRectDip(g_spot.filterTabs[static_cast<size_t>(i)]
                                        .rect,
                                    mx, my)) {
                        newTab = i;
                        break;
                    }
                }
                if (PtInRectDip(g_spot.listViewportRect, mx, my)) {
                    for (int i = 0;
                         i < static_cast<int>(g_spot.results.size()); ++i) {
                        if (PtInRectDip(
                                g_spot.results[static_cast<size_t>(i)].hitRect,
                                mx, my)) {
                            newRow = i;
                            break;
                        }
                    }
                }
                if (g_ql.open && PtInRectDip(g_spot.qlPaneRect, mx, my)) {
                    for (int i = 0; i < g_ql.actionCount; ++i) {
                        if (PtInRectDip(g_ql.actionRects[i], mx, my)) {
                            newQlAct = i;
                            break;
                        }
                    }
                }
            }

            if (newOrb != g_spot.hoveredOrb || newRow != g_spot.hoveredRow ||
                newTab != g_spot.hoveredTab ||
                newQlAct != g_ql.hoveredAction ||
                newChipClose != g_spot.hoverChipClose ||
                newQlBtn != g_spot.hoverQuickLookBtn) {
                bool orbChanged = (newOrb != g_spot.hoveredOrb);
                g_spot.hoveredOrb = newOrb;
                g_spot.hoveredRow = newRow;
                g_spot.hoveredTab = newTab;
                g_ql.hoveredAction = newQlAct;
                g_spot.hoverChipClose = newChipClose;
                g_spot.hoverQuickLookBtn = newQlBtn;
                if (orbChanged) EnsureAnimTimerRunning();
                RenderSpotlightSurface();
            }
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            float mx = static_cast<float>(GET_X_LPARAM(lp)) / g_spot.scale;
            float my = static_cast<float>(GET_Y_LPARAM(lp)) / g_spot.scale;
            if (g_spot.expanded && PtInRectDip(g_spot.listViewportRect, mx, my)) {
                for (int i = 0; i < static_cast<int>(g_spot.results.size());
                     ++i) {
                    if (PtInRectDip(
                            g_spot.results[static_cast<size_t>(i)].hitRect, mx,
                            my)) {
                        g_spot.selected = i;
                        g_spot.calcSelected = false;
                        ExecutePrimaryAction(
                            (GetKeyState(VK_CONTROL) & 0x8000) != 0,
                            (GetKeyState(VK_MENU) & 0x8000) != 0, false);
                        return 0;
                    }
                }
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            float mx = static_cast<float>(GET_X_LPARAM(lp)) / g_spot.scale;
            float my = static_cast<float>(GET_Y_LPARAM(lp)) / g_spot.scale;

            if (!g_spot.expanded && g_spot.expandProg < 0.08f) {
                for (int i = 0; i < 4; ++i) {
                    if (PtInRectDip(g_spot.orbRects[i], mx, my)) {
                        static constexpr SpotlightMode kOrbModes[] = {
                            SpotlightMode::Apps, SpotlightMode::Files,
                            SpotlightMode::Actions, SpotlightMode::Clipboard};
                        g_spot.mode = kOrbModes[i];
                        g_spot.fileTab = FileSubTab::All;
                        g_spot.expanded = true;
                        g_spot.selected = 0;
                        TriggerSpotlightRebuild(
                            g_spot.mode == SpotlightMode::Files);
                        EnsureAnimTimerRunning();
                        SyncGlassVisualTree();
                        RenderSpotlightSurface();
                        return 0;
                    }
                }
                if (PtInRectDip(g_spot.pillRect, mx, my)) {
                    g_spot.expanded = true;
                    TriggerSpotlightRebuild(false);
                    EnsureAnimTimerRunning();
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                    return 0;
                }
                HideSpotlight(false);
                return 0;
            }

            if (!PtInRectDip(g_spot.sheetRect, mx, my)) {
                HideSpotlight(false);
                return 0;
            }

            if (PtInRectDip(g_spot.chipCloseRect, mx, my)) {
                g_spot.mode = SpotlightMode::All;
                g_spot.fileTab = FileSubTab::All;
                TriggerSpotlightRebuild(false);
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }

            if (PtInRectDip(g_spot.quickLookToggleRect, mx, my)) {
                g_ql.open = !g_ql.open;
                SyncQuickLookToSelection();
                SyncGlassVisualTree();
                RenderSpotlightSurface();
                return 0;
            }

            for (const auto& tab : g_spot.filterTabs) {
                if (PtInRectDip(tab.rect, mx, my)) {
                    if (tab.mode == SpotlightMode::Files &&
                        EffectiveMode() == SpotlightMode::Files) {
                        g_spot.fileTab = tab.fileSub;
                    } else {
                        g_spot.mode = tab.mode;
                        g_spot.fileTab = FileSubTab::All;
                    }
                    g_spot.selected = 0;
                    if (g_spot.mode == SpotlightMode::Windows) {
                        RefreshWindowsSnapshot(hwnd);
                    }
                    TriggerSpotlightRebuild(g_spot.mode == SpotlightMode::Files);
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                    return 0;
                }
            }

            if (g_spot.hasCalc && PtInRectDip(g_spot.calcCardRect, mx, my)) {
                g_spot.calcSelected = true;
                ExecutePrimaryAction(false, false, false);
                return 0;
            }

            if (PtInRectDip(g_spot.listViewportRect, mx, my)) {
                for (int i = 0; i < static_cast<int>(g_spot.results.size());
                     ++i) {
                    if (PtInRectDip(
                            g_spot.results[static_cast<size_t>(i)].hitRect, mx,
                            my)) {
                        if (g_spot.selected == i && !g_spot.calcSelected) {
                            ExecutePrimaryAction(
                                (GetKeyState(VK_CONTROL) & 0x8000) != 0,
                                (GetKeyState(VK_MENU) & 0x8000) != 0, false);
                        } else {
                            g_spot.selected = i;
                            g_spot.calcSelected = false;
                            SyncQuickLookToSelection();
                            EnsureAnimTimerRunning();
                            RenderSpotlightSurface();
                        }
                        return 0;
                    }
                }
            }

            if (g_ql.open && PtInRectDip(g_spot.qlPaneRect, mx, my)) {
                for (int i = 0; i < g_ql.actionCount; ++i) {
                    if (PtInRectDip(g_ql.actionRects[i], mx, my) &&
                        i < static_cast<int>(g_spot.qlActions.size()) &&
                        g_spot.selected >= 0 &&
                        g_spot.selected < static_cast<int>(g_spot.results.size())) {
                        SearchResult copy =
                            g_spot.results[static_cast<size_t>(g_spot.selected)];
                        ExecuteQuickAction(
                            copy,
                            g_spot.qlActions[static_cast<size_t>(i)].actionCode);
                        return 0;
                    }
                }
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            float mx = static_cast<float>(GET_X_LPARAM(lp)) / g_spot.scale;
            float my = static_cast<float>(GET_Y_LPARAM(lp)) / g_spot.scale;
            if (g_spot.expanded && PtInRectDip(g_spot.listViewportRect, mx, my)) {
                for (int i = 0; i < static_cast<int>(g_spot.results.size());
                     ++i) {
                    if (PtInRectDip(
                            g_spot.results[static_cast<size_t>(i)].hitRect, mx,
                            my)) {
                        g_spot.selected = i;
                        g_spot.calcSelected = false;
                        POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                        ClientToScreen(hwnd, &pt);
                        SearchResult copy =
                            g_spot.results[static_cast<size_t>(i)];
                        ShowResultContextMenu(copy, pt);
                        return 0;
                    }
                }
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            if (!g_spot.expanded) return 0;
            POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(hwnd, &pt);
            float mx = static_cast<float>(pt.x) / g_spot.scale;
            float my = static_cast<float>(pt.y) / g_spot.scale;
            int delta = GET_WHEEL_DELTA_WPARAM(wp);
            if (g_ql.open && PtInRectDip(g_spot.qlPaneRect, mx, my) &&
                g_ql.isText) {
                g_ql.scroll = std::clamp(
                    g_ql.scroll - (delta / WHEEL_DELTA) * 3, 0,
                    std::max(0, static_cast<int>(g_ql.lines.size()) - 4));
                RenderSpotlightSurface();
                return 0;
            }
            float vh = g_spot.listViewportRect.bottom - g_spot.listViewportRect.top;
            float maxScroll = std::max(
                0.0f, static_cast<float>(g_spot.results.size()) * kRowH - vh);
            g_spot.targetScrollY = std::clamp(
                g_spot.targetScrollY -
                    static_cast<float>(delta) / WHEEL_DELTA * kRowH * 2.0f,
                0.0f, maxScroll);
            EnsureAnimTimerRunning();
            RenderSpotlightSurface();
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Dedicated low-level keyboard hook thread for overriding busy hotkeys (e.g. Alt+Space claimed by Fluent Search)
HANDLE g_llHookThread = nullptr;
DWORD g_llHookThreadId = 0;
std::atomic<int> g_llHookCombo{0};
std::atomic<bool> g_llHookOverride{true};
std::atomic<bool> g_llHookActive{false};
std::atomic<HWND> g_hookTargetHwnd{nullptr};

bool HotkeyModifiersMatchNow(HotkeyCombo combo) {
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool win = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) ||
               ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);
    switch (combo) {
        case HotkeyCombo::AltSpace:
            return alt && !ctrl && !shift && !win;
        case HotkeyCombo::CtrlSpace:
            return ctrl && !alt && !shift && !win;
        case HotkeyCombo::CtrlAltSpace:
            return ctrl && alt && !shift && !win;
        case HotkeyCombo::ShiftAltSpace:
            return shift && alt && !ctrl && !win;
        case HotkeyCombo::WinShiftS:
            return win && shift && !ctrl && !alt;
    }
    return false;
}

LRESULT CALLBACK SpotlightLowLevelKeyHook(int code, WPARAM wp, LPARAM lp) {
    static bool s_swallowed = false;
    if (code == HC_ACTION && g_llHookActive.load(std::memory_order_relaxed) &&
        g_llHookOverride.load(std::memory_order_relaxed) && !g_quitting) {
        const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lp);
        if (kb && !(kb->flags & LLKHF_INJECTED)) {
            auto combo = static_cast<HotkeyCombo>(
                g_llHookCombo.load(std::memory_order_relaxed));
            DWORD wantVk =
                (combo == HotkeyCombo::WinShiftS) ? 'S' : VK_SPACE;
            if (kb->vkCode == wantVk) {
                if (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN) {
                    if (HotkeyModifiersMatchNow(combo)) {
                        if (!s_swallowed) {
                            s_swallowed = true;
                            if (HWND target = g_hookTargetHwnd.load(
                                    std::memory_order_relaxed)) {
                                PostMessageW(target, g_msgToggleSpotlight, 0,
                                             0);
                            }
                        }
                        return 1;
                    }
                } else if (wp == WM_KEYUP || wp == WM_SYSKEYUP) {
                    if (s_swallowed || HotkeyModifiersMatchNow(combo)) {
                        s_swallowed = false;
                        return 1;
                    }
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

DWORD WINAPI HotkeyHookThreadProc(LPVOID param) {
    HANDLE readyEv = static_cast<HANDLE>(param);
    MSG initMsg{};
    PeekMessageW(&initMsg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_llHookThreadId = GetCurrentThreadId();
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, SpotlightLowLevelKeyHook,
                                   GetModuleHandleW(nullptr), 0);
    if (readyEv) SetEvent(readyEv);
    if (!hook) {
        g_llHookThreadId = 0;
        return 1;
    }
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(hook);
    g_llHookThreadId = 0;
    return 0;
}

void StartHotkeyHookThread() {
    if (g_llHookThread) return;
    HANDLE readyEv = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_llHookThread =
        CreateThread(nullptr, 0, HotkeyHookThreadProc, readyEv, 0, nullptr);
    if (g_llHookThread && readyEv) {
        WaitForSingleObject(readyEv, 500);
    }
    if (readyEv) CloseHandle(readyEv);
}

void StopHotkeyHookThread() {
    g_llHookActive.store(false, std::memory_order_release);
    if (g_llHookThreadId) {
        PostThreadMessageW(g_llHookThreadId, WM_QUIT, 0, 0);
    }
    if (g_llHookThread) {
        WaitForSingleObject(g_llHookThread, 1500);
        CloseHandle(g_llHookThread);
        g_llHookThread = nullptr;
    }
    g_llHookThreadId = 0;
}

void UpdateGlobalSpotlightHotkeys(HWND host) {
    g_hookTargetHwnd.store(host, std::memory_order_release);
    UnregisterHotKey(host, kHotkeySpotlight);
    g_llHookCombo.store(static_cast<int>(g_settings.hotkeyCombo),
                        std::memory_order_relaxed);
    g_llHookOverride.store(g_settings.overrideBusyHotkey,
                           std::memory_order_relaxed);

    UINT mods = MOD_ALT | MOD_NOREPEAT;
    UINT vk = VK_SPACE;
    switch (g_settings.hotkeyCombo) {
        case HotkeyCombo::CtrlSpace: mods = MOD_CONTROL | MOD_NOREPEAT; break;
        case HotkeyCombo::CtrlAltSpace:
            mods = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
            break;
        case HotkeyCombo::ShiftAltSpace:
            mods = MOD_SHIFT | MOD_ALT | MOD_NOREPEAT;
            break;
        case HotkeyCombo::WinShiftS:
            mods = MOD_WIN | MOD_SHIFT | MOD_NOREPEAT;
            vk = 'S';
            break;
        case HotkeyCombo::AltSpace:
        default: break;
    }
    if (RegisterHotKey(host, kHotkeySpotlight, mods, vk)) {
        g_llHookActive.store(false, std::memory_order_release);
        StopHotkeyHookThread();
    } else if (g_settings.overrideBusyHotkey) {
        g_llHookActive.store(true, std::memory_order_release);
        StartHotkeyHookThread();
    } else {
        g_llHookActive.store(false, std::memory_order_release);
        StopHotkeyHookThread();
    }
}

LRESULT CALLBACK SpotlightHostWndProc(HWND hwnd,
                                      UINT msg,
                                      WPARAM wp,
                                      LPARAM lp) {
    if (g_msgToggleSpotlight != 0 && msg == g_msgToggleSpotlight) {
        SpotlightMode req = SpotlightMode::All;
        if (wp >= 0 && wp <= static_cast<WPARAM>(SpotlightMode::Settings)) {
            req = static_cast<SpotlightMode>(wp);
        }
        ToggleSpotlight(req);
        return 0;
    }
    switch (msg) {
        case WM_CREATE:
            AddClipboardFormatListener(hwnd);
            UpdateGlobalSpotlightHotkeys(hwnd);
            return 0;

        case WM_HOTKEY:
            if (wp == kHotkeySpotlight) {
                ToggleSpotlight(SpotlightMode::All);
                return 0;
            }
            break;

        case WM_CLIPBOARDUPDATE:
            CaptureClipboardEvent();
            if (g_spot.visible && EffectiveMode() == SpotlightMode::Clipboard) {
                TriggerSpotlightRebuild(false);
                SyncGlassVisualTree();
                RenderSpotlightSurface();
            }
            return 0;

        case WM_COPYDATA: {
            const auto* cds = reinterpret_cast<const COPYDATASTRUCT*>(lp);
            if (cds && cds->dwData == g_evPendingSeq && g_evPending) {
                g_evPending = false;
                KillTimer(hwnd, kTimerEvFallback);
                g_searchedFiles = ParseEverythingCopyData(cds);
                g_searchedFilesKey = g_evPendingKey;
                g_fileSearchActive = false;
                if (g_spot.visible) {
                    TriggerSpotlightRebuild(false);
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                }
                return TRUE;
            }
            break;
        }

        case WM_TIMER:
            if (wp == kTimerEvFallback && g_evPending) {
                KillTimer(hwnd, kTimerEvFallback);
                g_evPending = false;
                if (g_spot.visible && g_spot.results.empty() &&
                    !g_spot.effectiveQuery.empty()) {
                    StartLocalFileSearchFallback(g_spot.effectiveQuery,
                                                 g_evPendingKey, g_spot.fileTab);
                } else {
                    g_fileSearchActive = false;
                    if (g_spot.visible) RenderSpotlightSurface();
                }
                return 0;
            }
            if (wp == kTimerClipSave) {
                KillTimer(hwnd, kTimerClipSave);
                if (g_clipDirty) SaveClipHistory();
                return 0;
            }
            break;

        case kWmAppSettings:
            LoadSettings();
            UpdateGlobalSpotlightHotkeys(hwnd);
            if (g_spot.visible) {
                g_spot.darkMode = IsSystemDarkMode();
                g_spot.sheetPlate.dirty = true;
                g_spot.pillPlate.dirty = true;
                for (auto& op : g_spot.orbPlates) op.dirty = true;
                TriggerSpotlightRebuild(false);
                SyncGlassVisualTree();
                RenderSpotlightSurface();
            }
            return 0;

        case kWmAppGlassFrame:
            if (g_spot.visible) {
                g_liquidCapture.framePosted.store(false);
                RefreshGlassBackdropOnly();
            }
            return 0;

        case kWmAppDeviceLost:
            if (!g_quitting && g_gfx.compGraphics) {
                if (InitD3DAndD2D()) {
                    try {
                        auto gfxInterop =
                            g_gfx.compGraphics
                                .as<ICompositionGraphicsDeviceInteropLocal>();
                        gfxInterop->SetRenderingDevice(g_gfx.d2dDevice.get());
                    } catch (...) {
                    }
                }
                OnGraphicsDeviceLost();
                if (g_spot.visible) {
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                }
            }
            return 0;

        case kWmAppIndexDone: {
            std::unique_ptr<IndexedAppsPayload> p(
                reinterpret_cast<IndexedAppsPayload*>(lp));
            g_indexing = false;
            g_indexedOnce = true;
            if (p && p->gen == g_indexGen) {
                g_apps = std::move(p->apps);
                if (g_spot.visible) {
                    TriggerSpotlightRebuild(false);
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                }
            }
            return 0;
        }

        case kWmAppMenuChanged:
            g_menuDirty = true;
            if (!g_indexing) StartIndex();
            return 0;

        case kWmAppBookmarksDone: {
            std::unique_ptr<BookmarkResultPayload> p(
                reinterpret_cast<BookmarkResultPayload*>(lp));
            if (p && p->gen == g_bookmarkGen) {
                g_bookmarks = std::move(p->entries);
                if (g_spot.visible &&
                    EffectiveMode() == SpotlightMode::Bookmarks) {
                    TriggerSpotlightRebuild(false);
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                }
            }
            return 0;
        }

        case kWmAppRecentDone: {
            std::unique_ptr<RecentPayload> p(reinterpret_cast<RecentPayload*>(lp));
            if (p && p->gen == g_recentGen) {
                g_recentStamp = p->stamp;
                g_recentFiles = std::move(p->entries);
                if (g_spot.visible) {
                    TriggerSpotlightRebuild(false);
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                }
            }
            return 0;
        }

        case kWmAppLocalFilesDone: {
            std::unique_ptr<LocalFilePayload> p(
                reinterpret_cast<LocalFilePayload*>(lp));
            if (p && p->gen == g_localSearchGen.load(std::memory_order_acquire)) {
                g_searchedFiles = std::move(p->entries);
                g_searchedFilesKey = p->key;
                g_fileSearchActive = false;
                if (g_spot.visible) {
                    TriggerSpotlightRebuild(false);
                    SyncGlassVisualTree();
                    RenderSpotlightSurface();
                }
            }
            return 0;
        }

        case kWmAppIconDone: {
            std::unique_ptr<AsyncBitmapDone> p(
                reinterpret_cast<AsyncBitmapDone*>(lp));
            if (p && p->appIdx >= 0 &&
                p->appIdx < static_cast<int>(g_apps.size()) &&
                p->appGen == g_indexGen) {
                g_apps[static_cast<size_t>(p->appIdx)].icon.wic = p->wic;
                g_apps[static_cast<size_t>(p->appIdx)].icon.d2d = nullptr;
                g_apps[static_cast<size_t>(p->appIdx)].icon.boundDeviceGen = 0;
                if (g_spot.visible) RenderSpotlightSurface();
            }
            return 0;
        }

        case kWmAppThumbDone: {
            std::unique_ptr<AsyncBitmapDone> p(
                reinterpret_cast<AsyncBitmapDone*>(lp));
            if (p) {
                if (p->kind == MediaJobKind::QuickLookImage) {
                    if (p->previewGen == g_ql.gen) {
                        g_ql.previewBmp.wic = p->wic;
                        g_ql.previewBmp.d2d = nullptr;
                        g_ql.previewBmp.boundDeviceGen = 0;
                        g_ql.loaded = (p->wic != nullptr);
                        g_ql.failed = !g_ql.loaded;
                        if (g_spot.visible && g_ql.open) RenderSpotlightSurface();
                    }
                } else {
                    g_thumbPending.erase(p->key);
                    ThumbCacheEntry entry;
                    entry.wic = p->wic;
                    entry.failed = (p->wic == nullptr);
                    g_thumbCache[p->key] = std::move(entry);
                    if (g_thumbCache.size() > 120) g_thumbCache.clear();
                    if (g_spot.visible) RenderSpotlightSurface();
                }
            }
            return 0;
        }

        case kWmAppQuit:
            PostQuitMessage(0);
            return 0;

        case WM_DESTROY:
            UnregisterHotKey(hwnd, kHotkeySpotlight);
            StopHotkeyHookThread();
            RemoveClipboardFormatListener(hwnd);
            KillTimer(hwnd, kTimerClipSave);
            if (g_clipDirty) SaveClipHistory();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI SpotlightUiThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    HMODULE hCoreMsg = LoadLibraryExW(L"CoreMessaging.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    using CreateDispatcherQueueControllerFn =
        HRESULT(WINAPI*)(DispatcherQueueOptions, PDISPATCHERQUEUECONTROLLER*);
    auto createDqc =
        hCoreMsg ? reinterpret_cast<CreateDispatcherQueueControllerFn>(
                       GetProcAddress(hCoreMsg,
                                      "CreateDispatcherQueueController"))
                 : nullptr;

    DispatcherQueueOptions dqOpts{sizeof(dqOpts), DQTYPE_THREAD_CURRENT,
                                  DQTAT_COM_NONE};
    winrt::Windows::System::DispatcherQueueController dqCtrl{nullptr};
    if (!createDqc ||
        FAILED(createDqc(dqOpts, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(
                                     winrt::put_abi(dqCtrl))))) {
        Wh_Log(L"Spotlight failed to create DispatcherQueueController");
        if (hCoreMsg) FreeLibrary(hCoreMsg);
        winrt::uninit_apartment();
        CoUninitialize();
        return 1;
    }

    wuc::Compositor compositor;
    if (!InitGraphics(compositor)) {
        Wh_Log(L"Spotlight failed to initialize Direct3D / Composition");
        dqCtrl = nullptr;
        if (hCoreMsg) FreeLibrary(hCoreMsg);
        winrt::uninit_apartment();
        CoUninitialize();
        return 1;
    }

    WNDCLASSEXW hostWc{sizeof(hostWc)};
    hostWc.lpfnWndProc = SpotlightHostWndProc;
    hostWc.hInstance = GetModuleHandleW(nullptr);
    hostWc.lpszClassName = kSpotlightHostClass;
    RegisterClassExW(&hostWc);

    WNDCLASSEXW uiWc{sizeof(uiWc)};
    uiWc.style = CS_DBLCLKS;
    uiWc.lpfnWndProc = SpotlightUiWndProc;
    uiWc.hInstance = GetModuleHandleW(nullptr);
    uiWc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    uiWc.lpszClassName = kSpotlightUiClass;
    RegisterClassExW(&uiWc);

    g_hostHwnd = CreateWindowExW(WS_EX_TOOLWINDOW, kSpotlightHostClass, L"",
                                 WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                                 GetModuleHandleW(nullptr), nullptr);
    g_spot.hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP,
        kSpotlightUiClass, L"Spotlight Search", WS_POPUP, 0, 0, 1, 1,
        g_hostHwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (g_spot.hwnd && WantOpticalLiquidGlass()) {
        ApplyCaptureExclusion(true);
        g_liquidCapture.excludedApplied = true;
        g_liquidCapture.excludedAtMs = GetTickCount64();
    }

    if (g_spot.hwnd && g_gfx.compositor) {
        auto desktopInterop = g_gfx.compositor.as<ICompositorDesktopInteropLocal>();
        if (SUCCEEDED(desktopInterop->CreateDesktopWindowTarget(
                g_spot.hwnd, TRUE, winrt::put_abi(g_spot.compTarget))) &&
            g_spot.compTarget) {
            g_spot.rootVisual = g_gfx.compositor.CreateContainerVisual();
            g_spot.compTarget.Root(g_spot.rootVisual);
        }
    }

    LoadUserStats();
    LoadClipHistory();
    StartMediaWorker();
    StartWatcher();
    StartIndex();

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    HideSpotlight(false);
    if (g_clipDirty) SaveClipHistory();
    StopHotkeyHookThread();
    StopWatcher();
    StopMediaWorker();
    ShutdownLiquidCapture();

    g_spot.pillPlate.Shutdown();
    for (auto& op : g_spot.orbPlates) op.Shutdown();
    g_spot.sheetPlate.Shutdown();
    g_spot.shadowVisual = nullptr;
    g_spot.shadowSurface = nullptr;
    g_spot.shadowBrush = nullptr;
    g_spot.uiVisual = nullptr;
    g_spot.uiSurface = nullptr;
    g_spot.uiBrush = nullptr;
    g_spot.rootVisual = nullptr;
    g_spot.compTarget = nullptr;

    if (g_spot.hwnd) {
        DestroyWindow(g_spot.hwnd);
        g_spot.hwnd = nullptr;
    }
    if (g_hostHwnd) {
        DestroyWindow(g_hostHwnd);
        g_hostHwnd = nullptr;
    }
    UnregisterClassW(kSpotlightUiClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kSpotlightHostClass, GetModuleHandleW(nullptr));

    g_gfx.Shutdown();
    if (dqCtrl) {
        try {
            auto shutdown = dqCtrl.ShutdownQueueAsync();
            for (int i = 0;
                 i < 100 && shutdown.Status() == wf::AsyncStatus::Started; ++i) {
                MsgWaitForMultipleObjects(0, nullptr, FALSE, 10, QS_ALLINPUT);
                MSG qm{};
                while (PeekMessageW(&qm, nullptr, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&qm);
                    DispatchMessageW(&qm);
                }
            }
        } catch (...) {
        }
        dqCtrl = nullptr;
    }
    winrt::uninit_apartment();
    if (hCoreMsg) FreeLibrary(hCoreMsg);
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_quitting = false;
    g_msgToggleSpotlight =
        RegisterWindowMessageW(L"WindhawkMacOSTahoe_ToggleSpotlight");
    LoadSettings();
    g_uiThread =
        CreateThread(nullptr, 0, SpotlightUiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_hostHwnd) PostMessageW(g_hostHwnd, kWmAppSettings, 0, 0);
}

void WhTool_ModUninit() {
    g_quitting = true;
    if (g_hostHwnd) PostMessageW(g_hostHwnd, kWmAppQuit, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 4000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

// Windhawk tool-mod boilerplate for windhawk.exe host
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;
HANDLE g_toolModClosingEvent = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
            ULONGLONG start = GetTickCount64();
            for (;;) {
                Sleep(100);
                HANDLE m = CreateMutexW(nullptr, TRUE,
                                        L"windhawk-tool-mod_" WH_MOD_ID);
                if (m && GetLastError() != ERROR_ALREADY_EXISTS) {
                    g_toolModProcessMutex = m;
                    break;
                }
                if (m) {
                    CloseHandle(m);
                }
                bool closing = false;
                if (HANDLE ev = OpenEventW(
                        SYNCHRONIZE, FALSE,
                        L"windhawk-tool-mod-closing_" WH_MOD_ID)) {
                    closing = WaitForSingleObject(ev, 0) == WAIT_OBJECT_0;
                    CloseHandle(ev);
                }
                if (GetTickCount64() - start > (closing ? 12000u : 3000u)) {
                    break;
                }
            }
            if (!g_toolModProcessMutex) {
                Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
                ExitProcess(1);
            }
        }

        g_toolModClosingEvent = CreateEventW(
            nullptr, TRUE, FALSE, L"windhawk-tool-mod-closing_" WH_MOD_ID);

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
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

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
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

    if (g_toolModClosingEvent) {
        SetEvent(g_toolModClosingEvent);
    }
    WhTool_ModUninit();
    if (g_toolModClosingEvent) {
        CloseHandle(g_toolModClosingEvent);
        g_toolModClosingEvent = nullptr;
    }
    ExitProcess(0);
}
