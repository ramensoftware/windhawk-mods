// ==WindhawkMod==
// @id              chrome-acrylic-titlebar
// @name            Chrome Acrylic Titlebar
// @description     Show the Windows Acrylic backdrop behind Chrome's tab strip and toolbar
// @version         0.1.0
// @author          kentakang
// @github          https://github.com/kentakang
// @license         MIT
// @architecture    x86-64
// @include         chrome.exe
// @compilerOptions -ldwmapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome Acrylic Titlebar

Makes Google Chrome's tab strip and toolbar transparent, so the Windows 11
Acrylic backdrop shows through. Web page content is not changed.

## How it works

Chrome asks Windows for a Mica backdrop but paints its frame opaque on top of
it. The mod hooks a few functions in `chrome.dll`:

- `BrowserDesktopWindowTreeHostWin::ShouldWindowContentsBeTransparent` returns
  `true`, so Chrome composites its window with a transparent background.
- `BrowserFrameViewWin::OnPaint` skips painting the titlebar.
- `BrowserTabStripController::InitFromModel` marks each tab strip as a glass
  frame with `TabStrip::SetIsGlassFrame(true)`, so inactive tabs don't fill
  their background. On macOS Chrome does this itself; on Windows nothing does.
- `views::View::OnPaintBackground` clears the toolbar and bookmarks bar
  backgrounds (optional).

The backdrop Chrome requests (`DWMSBT_MAINWINDOW`) is replaced with the one
chosen in the settings, Acrylic by default.

## First run and Chrome updates

The mod needs the symbols (PDB) of the installed `chrome.dll`. Windhawk
downloads them from Google's symbol server. For Chrome 158 the download is
about 1.1 GB compressed and about 5.8 GB on disk.

1. Enable the mod and start Chrome. While the symbols download, Chrome starts
   normally without the effect.
2. Wait a few minutes until the download finishes. With logging enabled, the
   mod logs `Symbols are ready`.
3. Fully exit Chrome, including any background instance, and start it again.

Windhawk caches the function addresses, so later starts apply the effect
immediately. A Chrome update replaces `chrome.dll`, so the steps repeat after
each update. Without a cache hit, Chrome waits up to 4 seconds at startup.
Downloaded PDBs stay in `C:\ProgramData\Windhawk\Engine\Symbols`; old versions
can be deleted.

## Notes

- Tested with Chrome 158.0.8127.37 (x64). If a hooked function is missing in
  another version, the mod leaves Chrome unchanged.
- Windows draws the Acrylic backdrop, so its tint follows the system light or
  dark theme, and it turns solid while the window is inactive.
- The address field and the active tab keep their own background colors.
- **Hide duplicate Windows caption buttons** removes `WS_SYSMENU` from browser
  windows. This can change the Alt+Space menu and tools that inspect window
  styles.
- To undo everything, disable the mod and restart Chrome.

Hiding the duplicate caption buttons and the asynchronous symbol lookup are
adapted from [Brave Acrylic](https://windhawk.net/mods/brave-acrylic) by
bardelyne (MIT).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backdrop: acrylic
  $name: Backdrop
  $description: The Windows backdrop shown behind the tab strip. Applies to new windows.
  $options:
  - acrylic: Acrylic
  - tabbed: Mica Alt
  - mica: Mica
- inactiveTabs: true
  $name: Transparent inactive tabs
  $description: Inactive tabs don't fill their background, so the backdrop shows behind them. Active, selected and hovered tabs are still painted. Applies to new windows.
- toolbar: true
  $name: Transparent toolbar
  $description: Also clear the toolbar and bookmarks bar backgrounds. The address field and the active tab keep their colors. Applies immediately.
- hideNativeCaptionButtons: true
  $name: Hide duplicate Windows caption buttons
  $description: Hide the minimize, maximize and close buttons that Windows draws under the transparent tab strip. This removes WS_SYSMENU from browser windows, which can change the Alt+Space menu.
- downloadSymbols: true
  $name: Download symbols from Google
  $description: When the symbols aren't cached, download the chrome.dll PDB from Google's symbol server (about 1.1 GB compressed, 5.8 GB on disk). When off, only symbols already on disk are used. Restart Chrome to apply.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <atomic>
#include <cstring>
#include <cwchar>
#include <string_view>

#include <windhawk_utils.h>

namespace {

constexpr DWORD kBackdropTypeAttribute = 38;  // DWMWA_SYSTEMBACKDROP_TYPE
constexpr UINT kBackdropMica = 2;             // DWMSBT_MAINWINDOW
constexpr UINT kBackdropAcrylic = 3;          // DWMSBT_TRANSIENTWINDOW
constexpr UINT kBackdropMicaAlt = 4;          // DWMSBT_TABBEDWINDOW

// Chrome continues without the effect if symbols take longer than this.
constexpr DWORD kSymbolWaitMs = 4000;
constexpr wchar_t kChromeSymbolServer[] =
    L"https://chromium-browser-symsrv.commondatastorage.googleapis.com";

// Decorated names, checked against the Chrome 158.0.8127.37 PDB. None of them
// shares its address with another function (no identical-code folding).
enum Slot : unsigned {
    kTransparent,
    kFrameOnPaint,
    kOnPaintBackground,
    kGetClassName,
    kDrawColor,
    kShouldPaintTabBackground,
    kInitFromModel,
    kSetIsGlassFrame,
    kSlotCount,
};
constexpr std::wstring_view kSymbols[kSlotCount] = {
    // private: virtual bool BrowserDesktopWindowTreeHostWin::ShouldWindowContentsBeTransparent() const
    L"?ShouldWindowContentsBeTransparent@BrowserDesktopWindowTreeHostWin@@EEBA_NXZ",
    // protected: virtual void BrowserFrameViewWin::OnPaint(gfx::Canvas*)
    L"?OnPaint@BrowserFrameViewWin@@MEAAXPEAVCanvas@gfx@@@Z",
    // protected: virtual void views::View::OnPaintBackground(gfx::Canvas*)
    L"?OnPaintBackground@View@views@@MEAAXPEAVCanvas@gfx@@@Z",
    // public: std::string_view views::View::GetClassName() const (windows.h renames it)
    L"?GetClassNameW@View@views@@QEBA?AV?$basic_string_view@DU?$char_traits@D@__Cr@std@@@__Cr@std@@XZ",
    // public: void gfx::Canvas::DrawColor(SkColor, SkBlendMode)
    L"?DrawColor@Canvas@gfx@@QEAAXIW4SkBlendMode@@@Z",
    // public: virtual bool TabStyleViewDelegateImpl::ShouldPaintTabBackgroundColor() const
    // (tabs/common/tab_view.cc, the delegate of the TabView that draws tabs).
    // HorizontalTabStyleViews asks it only for inactive, unselected tabs:
    // active and selected tabs are painted before it is asked, and the hover
    // highlight is painted separately.
    L"?ShouldPaintTabBackgroundColor@TabStyleViewDelegateImpl@@UEBA_NXZ",
    // public: void BrowserTabStripController::InitFromModel(TabStrip*)
    // Runs once per browser window for the classic Tab/TabStrip path that
    // Chrome 158 still uses for the horizontal tab strip.
    L"?InitFromModel@BrowserTabStripController@@QEAAXPEAVTabStrip@@@Z",
    // public: void TabStrip::SetIsGlassFrame(bool)
    // Normally driven by GlassFrameService, which Windows builds never create.
    // A glass tab strip leaves inactive tabs unfilled (TabStrip::IsGlassFrame).
    L"?SetIsGlassFrame@TabStrip@@QEAAX_N@Z",
};

// Native x64 ABI shapes. A member function returning std::string_view writes
// it through a hidden pointer passed after `this`.
struct NativeClassName {
    const char* data;
    size_t size;
};
using Transparent_t = bool (*)(const void* host);
using Paint_t = void (*)(void* view, void* canvas);
using GetClassName_t = NativeClassName* (*)(const void* view, NativeClassName* out);
using DrawColor_t = void (*)(void* canvas, unsigned int color, int mode);
using TabFill_t = bool (*)(const void* delegate);
using InitFromModel_t = void (*)(void* controller, void* tabStrip);
using SetIsGlassFrame_t = void (*)(void* tabStrip, bool isGlass);
constexpr int kSkBlendModeClear = 0;

// Settings.
std::atomic<UINT> g_backdrop{kBackdropAcrylic};
std::atomic<bool> g_inactiveTabs{true};
std::atomic<bool> g_toolbar{true};
std::atomic<bool> g_hideCaptions{true};
std::atomic<bool> g_downloadSymbols{true};

// Set once the chrome.dll hooks are applied, before any browser window exists.
std::atomic<bool> g_active{false};
std::atomic<bool> g_toolbarAvailable{false};
std::atomic<bool> g_chromeAttempted{false};
bool g_isBrowserProcess = false;

Transparent_t g_origTransparent = nullptr;
Paint_t g_origFrameOnPaint = nullptr;
Paint_t g_origOnPaintBackground = nullptr;
GetClassName_t g_getClassName = nullptr;
DrawColor_t g_drawColor = nullptr;
TabFill_t g_origShouldPaintTabBackground = nullptr;
InitFromModel_t g_origInitFromModel = nullptr;
SetIsGlassFrame_t g_setIsGlassFrame = nullptr;

decltype(&DwmSetWindowAttribute) g_origDwmSetWindowAttribute = nullptr;
decltype(&LoadLibraryExW) g_origLoadLibraryExW = nullptr;

// ---------------------------------------------------------------------------
// Settings

void LoadSettings() {
    PCWSTR backdrop = Wh_GetStringSetting(L"backdrop");
    UINT type = kBackdropAcrylic;
    if (backdrop && !wcscmp(backdrop, L"mica")) {
        type = kBackdropMica;
    } else if (backdrop && !wcscmp(backdrop, L"tabbed")) {
        type = kBackdropMicaAlt;
    }
    Wh_FreeStringSetting(backdrop);
    g_backdrop.store(type);
    g_inactiveTabs.store(Wh_GetIntSetting(L"inactiveTabs") != 0);
    g_toolbar.store(Wh_GetIntSetting(L"toolbar") != 0);
    g_hideCaptions.store(Wh_GetIntSetting(L"hideNativeCaptionButtons") != 0);
    g_downloadSymbols.store(Wh_GetIntSetting(L"downloadSymbols") != 0);
}

// ---------------------------------------------------------------------------
// Browser window identification
//
// Only browser windows (BrowserDesktopWindowTreeHostWin) ask the hooked
// ShouldWindowContentsBeTransparent while they are created, right after their
// HWND was created on the same thread. That HWND gets a property, and only
// marked windows get the backdrop and caption-button changes. Dialogs, task
// manager and other Chrome windows keep their native frame untouched.

constexpr wchar_t kBrowserFrameProperty[] = L"Windhawk.ChromeAcrylicTitlebar.Frame";
constexpr ULONGLONG kCandidateMaxAgeMs = 2000;
thread_local HWND t_candidate = nullptr;
thread_local ULONGLONG t_candidateTick = 0;

LONG_PTR RealStyle(HWND window);
void ApplyNativeCaption(HWND window);

bool IsBrowserFrameWindow(HWND window) {
    return window && GetPropW(window, kBrowserFrameProperty) != nullptr;
}

bool IsFrameCandidate(HWND window, DWORD style) {
    constexpr DWORD kRequired =
        WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    if ((style & kRequired) != kRequired || (style & WS_CHILD) ||
        GetAncestor(window, GA_ROOT) != window || GetWindow(window, GW_OWNER)) {
        return false;
    }
    wchar_t name[32]{};
    return GetClassNameW(window, name, ARRAYSIZE(name)) &&
           !wcscmp(name, L"Chrome_WidgetWin_1");
}

void MarkBrowserFrame(HWND window) {
    if (!SetPropW(window, kBrowserFrameProperty, reinterpret_cast<HANDLE>(1))) {
        return;
    }
    // Chrome already asked for Mica while creating the window.
    const UINT backdrop = g_backdrop.load();
    g_origDwmSetWindowAttribute(window, kBackdropTypeAttribute, &backdrop,
                                sizeof(backdrop));
    ApplyNativeCaption(window);
}

// ---------------------------------------------------------------------------
// chrome.dll hooks

bool Transparent_Hook(const void* host) {
    if (!g_active.load(std::memory_order_acquire)) {
        return g_origTransparent(host);
    }
    const HWND candidate = t_candidate;
    if (candidate) {
        t_candidate = nullptr;
        if (GetTickCount64() - t_candidateTick < kCandidateMaxAgeMs &&
            IsWindow(candidate)) {
            MarkBrowserFrame(candidate);
        }
    }
    return true;
}

void FrameOnPaint_Hook(void* view, void* canvas) {
    // Leave the titlebar unpainted so the DWM backdrop shows through. Child
    // views (tabs, caption buttons, toolbar) paint afterwards as usual.
    if (g_active.load(std::memory_order_acquire)) {
        return;
    }
    g_origFrameOnPaint(view, canvas);
}

bool IsToolbarRow(void* view) {
    NativeClassName name{};
    g_getClassName(view, &name);
    if (!name.data || name.size == 0 || name.size > 64) {
        return false;
    }
    const std::string_view cls(name.data, name.size);
    return cls == "ToolbarView" || cls == "BookmarkBarView";
}

void OnPaintBackground_Hook(void* view, void* canvas) {
    if (view && canvas && g_toolbar.load(std::memory_order_relaxed) &&
        g_toolbarAvailable.load(std::memory_order_acquire) &&
        g_active.load(std::memory_order_acquire) && IsToolbarRow(view)) {
        // The canvas is clipped to this view; clear it to reveal the backdrop.
        g_drawColor(canvas, 0, kSkBlendModeClear);
        return;
    }
    g_origOnPaintBackground(view, canvas);
}

bool ShouldPaintTabBackground_Hook(const void* delegate) {
    if (g_inactiveTabs.load(std::memory_order_relaxed) &&
        g_active.load(std::memory_order_acquire)) {
        return false;
    }
    return g_origShouldPaintTabBackground(delegate);
}

void InitFromModel_Hook(void* controller, void* tabStrip) {
    g_origInitFromModel(controller, tabStrip);
    if (tabStrip && g_setIsGlassFrame &&
        g_inactiveTabs.load(std::memory_order_relaxed) &&
        g_active.load(std::memory_order_acquire)) {
        g_setIsGlassFrame(tabStrip, true);
    }
}

// ---------------------------------------------------------------------------
// Backdrop type

HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND window,
                                          DWORD attribute,
                                          LPCVOID value,
                                          DWORD size) {
    if (attribute == kBackdropTypeAttribute && value && size == sizeof(UINT) &&
        *static_cast<const UINT*>(value) == kBackdropMica &&
        g_active.load(std::memory_order_acquire) && IsBrowserFrameWindow(window)) {
        const UINT backdrop = g_backdrop.load();
        return g_origDwmSetWindowAttribute(window, attribute, &backdrop,
                                           sizeof(backdrop));
    }
    return g_origDwmSetWindowAttribute(window, attribute, value, size);
}

// ---------------------------------------------------------------------------
// Native caption buttons
//
// Windows still draws its caption buttons underneath Chromium's transparent
// custom frame. Hide only WS_SYSMENU and keep reporting it to Chromium, so its
// own logic (including saving the style for fullscreen) sees the original.
// Adapted from Brave Acrylic (MIT, bardelyne).

constexpr wchar_t kCaptionProperty[] = L"Windhawk.ChromeAcrylicTitlebar.NativeCaption";
std::atomic<bool> g_captionFixActive{false};
std::atomic<bool> g_captionStopping{false};
std::atomic<bool> g_captionPropertyUsed{false};
bool g_captionHooksInstalled = false;
decltype(&CreateWindowExW) g_origCreateWindowExW = nullptr;
decltype(&GetWindowLongPtrW) g_origGetWindowLongPtrW = nullptr;
decltype(&SetWindowLongPtrW) g_origSetWindowLongPtrW = nullptr;
decltype(&GetWindowLongW) g_origGetWindowLongW = nullptr;
decltype(&SetWindowLongW) g_origSetWindowLongW = nullptr;
thread_local HWND t_captionStyleWrite = nullptr;

LONG_PTR RealStyle(HWND window) {
    return g_origGetWindowLongPtrW ? g_origGetWindowLongPtrW(window, GWL_STYLE)
                                   : GetWindowLongPtrW(window, GWL_STYLE);
}

void RefreshFrame(HWND window) {
    SetWindowPos(window, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                     SWP_FRAMECHANGED);
}

void RestoreNativeCaption(HWND window) {
    if (!GetPropW(window, kCaptionProperty)) {
        return;
    }
    RemovePropW(window, kCaptionProperty);
    // The property is gone, so the hooks pass this write through unchanged.
    const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    SetWindowLongPtrW(window, GWL_STYLE, style | WS_SYSMENU);
    RefreshFrame(window);
}

template <typename Value>
Value ReadCaptionStyle(HWND window, int index, Value(WINAPI* original)(HWND, int)) {
    if (index != GWL_STYLE ||
        (!g_captionFixActive.load(std::memory_order_acquire) &&
         !g_captionPropertyUsed.load(std::memory_order_relaxed))) {
        return original(window, index);
    }
    Value value = original(window, index);
    const DWORD error = GetLastError();
    if (GetPropW(window, kCaptionProperty)) {
        value |= WS_SYSMENU;
    }
    SetLastError(error);
    return value;
}

LONG_PTR WINAPI GetWindowLongPtrW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, g_origGetWindowLongPtrW);
}

LONG WINAPI GetWindowLongW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, g_origGetWindowLongW);
}

template <typename Value>
Value WriteCaptionStyle(HWND window,
                        int index,
                        Value value,
                        Value(WINAPI* original)(HWND, int, Value)) {
    // Windows may forward one API to the other; translate the style only once.
    if (index != GWL_STYLE || t_captionStyleWrite == window ||
        (!g_captionFixActive.load(std::memory_order_acquire) &&
         !g_captionPropertyUsed.load(std::memory_order_relaxed))) {
        return original(window, index, value);
    }
    const bool owned = GetPropW(window, kCaptionProperty) != nullptr;
    const bool hide = g_captionFixActive.load(std::memory_order_acquire) &&
                      (value & WS_SYSMENU) && (owned || IsBrowserFrameWindow(window));
    bool marked = owned;
    if (hide && !owned) {
        g_captionPropertyUsed.store(true, std::memory_order_relaxed);
        marked = SetPropW(window, kCaptionProperty, reinterpret_cast<HANDLE>(1)) != FALSE;
    } else if (!hide && owned) {
        RemovePropW(window, kCaptionProperty);
        marked = false;
    }
    const HWND previousWrite = t_captionStyleWrite;
    t_captionStyleWrite = window;
    // Zero is a valid previous style; clear stale errors first.
    SetLastError(ERROR_SUCCESS);
    const Value previous = original(
        window, index, hide && marked ? value & ~static_cast<Value>(WS_SYSMENU) : value);
    const DWORD error = GetLastError();
    t_captionStyleWrite = previousWrite;
    if (!previous && error) {
        if (owned) {
            SetPropW(window, kCaptionProperty, reinterpret_cast<HANDLE>(1));
        } else {
            RemovePropW(window, kCaptionProperty);
        }
    } else if (hide && marked && !g_captionFixActive.load(std::memory_order_acquire)) {
        // The setting changed while SetWindowLong dispatched style messages.
        RestoreNativeCaption(window);
    }
    SetLastError(error);
    return previous && owned ? previous | WS_SYSMENU : previous;
}

LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND window, int index, LONG_PTR value) {
    return WriteCaptionStyle(window, index, value, g_origSetWindowLongPtrW);
}

LONG WINAPI SetWindowLongW_Hook(HWND window, int index, LONG value) {
    return WriteCaptionStyle(window, index, value, g_origSetWindowLongW);
}

void ApplyNativeCaption(HWND window) {
    if (!g_captionFixActive.load(std::memory_order_acquire) ||
        GetPropW(window, kCaptionProperty)) {
        return;
    }
    const LONG_PTR style = RealStyle(window);
    if (!(style & WS_SYSMENU) || !IsBrowserFrameWindow(window)) {
        return;
    }
    SetWindowLongPtrW_Hook(window, GWL_STYLE, style);
    if (GetPropW(window, kCaptionProperty)) {
        RefreshFrame(window);
    }
}

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle,
                                 LPCWSTR className,
                                 LPCWSTR windowName,
                                 DWORD style,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 HWND parent,
                                 HMENU menu,
                                 HINSTANCE instance,
                                 LPVOID param) {
    const HWND window = g_origCreateWindowExW(exStyle, className, windowName, style, x,
                                              y, width, height, parent, menu, instance,
                                              param);
    const DWORD error = GetLastError();
    if (window && g_active.load(std::memory_order_acquire) &&
        IsFrameCandidate(window, style)) {
        t_candidate = window;
        t_candidateTick = GetTickCount64();
    }
    SetLastError(error);
    return window;
}

BOOL CALLBACK SyncCaptionWindow(HWND window, LPARAM) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId()) {
        if (g_captionFixActive.load(std::memory_order_acquire)) {
            ApplyNativeCaption(window);
        } else {
            RestoreNativeCaption(window);
        }
    }
    return TRUE;
}

BOOL CALLBACK ForgetBrowserFrame(HWND window, LPARAM) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId()) {
        RemovePropW(window, kBrowserFrameProperty);
    }
    return TRUE;
}

void SyncCaptions() {
    const bool active = g_captionHooksInstalled &&
                        !g_captionStopping.load(std::memory_order_acquire) &&
                        g_active.load(std::memory_order_acquire) &&
                        g_hideCaptions.load(std::memory_order_relaxed);
    g_captionFixActive.store(active, std::memory_order_release);
    if (g_captionHooksInstalled) {
        EnumWindows(SyncCaptionWindow, 0);
    }
}

BOOL CALLBACK UpdateBackdropWindow(HWND window, LPARAM) {
    if (IsBrowserFrameWindow(window)) {
        const UINT backdrop = g_backdrop.load();
        g_origDwmSetWindowAttribute(window, kBackdropTypeAttribute, &backdrop,
                                    sizeof(backdrop));
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// Symbol lookup

SRWLOCK g_lock = SRWLOCK_INIT;
CONDITION_VARIABLE g_lookupChanged = CONDITION_VARIABLE_INIT;
HANDLE g_worker = nullptr;
bool g_stopping = false;
struct {
    HMODULE module;
    ULONGLONG deadline;
    bool finished;
    bool succeeded;
    void* address[kSlotCount];
} g_lookup{};

bool ResolveAddresses(HMODULE module, void** result) {
    void* address[kSlotCount]{};
    WindhawkUtils::SYMBOL_HOOK chromeDllHooks[kSlotCount];
    for (unsigned i = 0; i < kSlotCount; ++i) {
        chromeDllHooks[i] = {{kSymbols[i]}, &address[i], nullptr, true};
    }
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.symbolServer = g_downloadSymbols.load() ? kChromeSymbolServer : L"";
    // The Chrome PDB is about 5.8 GB with millions of symbols. Undecorating all
    // of them is very slow, so match the decorated names only.
    options.noUndecoratedSymbols = TRUE;
    if (!WindhawkUtils::HookSymbols(module, chromeDllHooks, kSlotCount, &options)) {
        return false;
    }
    std::memcpy(result, address, sizeof(address));
    return address[kTransparent] && address[kFrameOnPaint];
}

DWORD WINAPI LookupWorker(void*) {
    void* address[kSlotCount]{};
    HMODULE pinned = nullptr;
    const HMODULE module = g_lookup.module;
    const bool succeeded =
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                           reinterpret_cast<LPCWSTR>(module), &pinned) &&
        ResolveAddresses(module, address);
    if (pinned) {
        FreeLibrary(pinned);
    }

    AcquireSRWLockExclusive(&g_lock);
    std::memcpy(g_lookup.address, address, sizeof(address));
    g_lookup.succeeded = succeeded;
    g_lookup.finished = true;
    const bool late = GetTickCount64() >= g_lookup.deadline;
    WakeAllConditionVariable(&g_lookupChanged);
    ReleaseSRWLockExclusive(&g_lock);

    if (!succeeded) {
        Wh_Log(L"Symbol lookup failed; Chrome stays unmodified");
    } else if (late) {
        Wh_Log(L"Symbols are ready; fully exit and restart Chrome to apply");
    }
    return 0;
}

bool InstallChromeHooks() {
    void* const* address = g_lookup.address;
    if (!WindhawkUtils::SetFunctionHook(
            reinterpret_cast<Transparent_t>(address[kTransparent]), Transparent_Hook,
            &g_origTransparent) ||
        !WindhawkUtils::SetFunctionHook(
            reinterpret_cast<Paint_t>(address[kFrameOnPaint]), FrameOnPaint_Hook,
            &g_origFrameOnPaint)) {
        return false;
    }
    if (address[kOnPaintBackground] && address[kGetClassName] && address[kDrawColor]) {
        g_getClassName = reinterpret_cast<GetClassName_t>(address[kGetClassName]);
        g_drawColor = reinterpret_cast<DrawColor_t>(address[kDrawColor]);
        g_toolbarAvailable.store(WindhawkUtils::SetFunctionHook(
            reinterpret_cast<Paint_t>(address[kOnPaintBackground]),
            OnPaintBackground_Hook, &g_origOnPaintBackground));
    }
    if (address[kShouldPaintTabBackground] &&
        !WindhawkUtils::SetFunctionHook(
            reinterpret_cast<TabFill_t>(address[kShouldPaintTabBackground]),
            ShouldPaintTabBackground_Hook, &g_origShouldPaintTabBackground)) {
        Wh_Log(L"Inactive tab background hook unavailable");
    }
    if (address[kInitFromModel] && address[kSetIsGlassFrame]) {
        g_setIsGlassFrame = reinterpret_cast<SetIsGlassFrame_t>(address[kSetIsGlassFrame]);
        if (!WindhawkUtils::SetFunctionHook(
                reinterpret_cast<InitFromModel_t>(address[kInitFromModel]),
                InitFromModel_Hook, &g_origInitFromModel)) {
            Wh_Log(L"Glass tab strip hook unavailable");
        }
    }
    return true;
}

// Resolve symbols and, if they arrive within the budget, hook chrome.dll.
// `install` is false when Chrome already has windows: those decided to be
// opaque at creation, so the lookup only fills Windhawk's cache.
void AttemptChrome(HMODULE module, bool install) {
    if (!module || g_chromeAttempted.exchange(true)) {
        return;
    }
    AcquireSRWLockExclusive(&g_lock);
    if (g_stopping) {
        ReleaseSRWLockExclusive(&g_lock);
        return;
    }
    g_lookup.module = module;
    g_lookup.deadline = GetTickCount64() + (install ? kSymbolWaitMs : 0);
    g_worker = CreateThread(nullptr, 0, LookupWorker, nullptr, 0, nullptr);
    if (!g_worker) {
        ReleaseSRWLockExclusive(&g_lock);
        Wh_Log(L"Could not start the symbol lookup");
        return;
    }
    if (!install) {
        ReleaseSRWLockExclusive(&g_lock);
        Wh_Log(L"Chrome was already running; restart Chrome to apply");
        return;
    }
    while (!g_lookup.finished && !g_stopping) {
        const ULONGLONG now = GetTickCount64();
        if (now >= g_lookup.deadline ||
            !SleepConditionVariableSRW(&g_lookupChanged, &g_lock,
                                       static_cast<DWORD>(g_lookup.deadline - now), 0)) {
            break;
        }
    }
    bool active = false;
    if (!g_stopping && g_lookup.finished && g_lookup.succeeded) {
        active = InstallChromeHooks() && Wh_ApplyHookOperations();
        if (!active) {
            Wh_Log(L"Could not hook chrome.dll; Chrome stays unmodified");
        }
    } else if (!g_lookup.finished) {
        Wh_Log(L"Symbols are still loading; Chrome starts without the effect");
    }
    g_active.store(active, std::memory_order_release);
    ReleaseSRWLockExclusive(&g_lock);
    if (active) {
        Wh_Log(L"Transparent titlebar is active");
        SyncCaptions();
    }
}

bool IsExecutableLoad(DWORD flags) {
    return !(flags & (DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE |
                      LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                      LOAD_LIBRARY_AS_IMAGE_RESOURCE));
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    const HMODULE module = g_origLoadLibraryExW(name, file, flags);
    const DWORD error = GetLastError();
    if (module && !g_chromeAttempted.load(std::memory_order_acquire) &&
        IsExecutableLoad(flags) && module == GetModuleHandleW(L"chrome.dll")) {
        // Chrome cannot use chrome.dll until this returns, so no window exists yet.
        AttemptChrome(module, true);
    }
    SetLastError(error);
    return module;
}

BOOL CALLBACK FindChromeWindow(HWND window, LPARAM found) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    wchar_t name[32]{};
    if (process == GetCurrentProcessId() &&
        GetClassNameW(window, name, ARRAYSIZE(name)) &&
        !wcscmp(name, L"Chrome_WidgetWin_1")) {
        *reinterpret_cast<bool*>(found) = true;
        return FALSE;
    }
    return TRUE;
}

bool IsBrowserProcess() {
    int count = 0;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!args) {
        return false;
    }
    bool browser = true;
    for (int i = 1; i < count; ++i) {
        if (!wcscmp(args[i], L"--") || !wcscmp(args[i], L"--single-argument")) {
            break;
        }
        if (!wcsncmp(args[i], L"--type=", 7)) {
            browser = false;
            break;
        }
    }
    LocalFree(args);
    return browser;
}

}  // namespace

BOOL Wh_ModInit() {
    if (!IsBrowserProcess()) {
        return FALSE;
    }
    g_isBrowserProcess = true;
    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute,
                                        DwmSetWindowAttribute_Hook,
                                        &g_origDwmSetWindowAttribute)) {
        Wh_Log(L"Could not hook DwmSetWindowAttribute");
        return FALSE;
    }
    g_captionHooksInstalled =
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                       &g_origCreateWindowExW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongPtrW, GetWindowLongPtrW_Hook,
                                       &g_origGetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongPtrW, SetWindowLongPtrW_Hook,
                                       &g_origSetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongW, GetWindowLongW_Hook,
                                       &g_origGetWindowLongW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongW, SetWindowLongW_Hook,
                                       &g_origSetWindowLongW);
    if (!g_captionHooksInstalled) {
        Wh_Log(L"Caption button fix unavailable");
    }

    if (GetModuleHandleW(L"chrome.dll")) {
        // Handled in Wh_ModAfterInit, once the hooks above are active.
        return TRUE;
    }
    const HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    const auto loader =
        kernelBase ? reinterpret_cast<decltype(&LoadLibraryExW)>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;
    if (!loader ||
        !WindhawkUtils::SetFunctionHook(loader, LoadLibraryExW_Hook,
                                        &g_origLoadLibraryExW)) {
        Wh_Log(L"Could not hook LoadLibraryExW");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isBrowserProcess) {
        return;
    }
    // chrome.dll was loaded before the loader hook became active.
    if (const HMODULE chrome = GetModuleHandleW(L"chrome.dll")) {
        bool hasWindows = false;
        EnumWindows(FindChromeWindow, reinterpret_cast<LPARAM>(&hasWindows));
        AttemptChrome(chrome, !hasWindows);
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isBrowserProcess) {
        return;
    }
    LoadSettings();
    SyncCaptions();
    if (g_active.load(std::memory_order_acquire)) {
        EnumWindows(UpdateBackdropWindow, 0);
    }
}

void Wh_ModBeforeUninit() {
    g_captionStopping.store(true, std::memory_order_release);
    g_captionFixActive.store(false, std::memory_order_release);
    AcquireSRWLockExclusive(&g_lock);
    g_stopping = true;
    g_active.store(false, std::memory_order_release);
    const HANDLE worker = g_worker;
    g_worker = nullptr;
    WakeAllConditionVariable(&g_lookupChanged);
    ReleaseSRWLockExclusive(&g_lock);
    // Give WS_SYSMENU back while the style hooks are still installed.
    if (g_isBrowserProcess && g_captionHooksInstalled) {
        EnumWindows(SyncCaptionWindow, 0);
    }
    if (g_isBrowserProcess) {
        EnumWindows(ForgetBrowserFrame, 0);
    }
    // Windhawk's symbol lookup cannot be cancelled; it must finish before the
    // mod DLL unloads.
    if (worker) {
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
    }
}
