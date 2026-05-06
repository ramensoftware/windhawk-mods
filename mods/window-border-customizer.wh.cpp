// ==WindhawkMod==
// @id              window-border-customizer
// @name            Window Border Customizer
// @description     Replaces DWM stock borders with custom ARGB translucent borders.
// @version         1.0.0
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         dwm.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwevtapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## Window Border Customizer

A mod that allows you to change the color of the borders for all windows, classic menus and tooltips.
![](https://i.imgur.com/skNvSX3.png)

## ⚠ Important usage note ⚠

This mod needs to hook into `dwm.exe` to work.
Please navigate to Windhawk's
Settings > Advanced settings > More advanced settings > Process inclusion list,
and make sure that `dwm.exe` is in the list.
![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

---

## Contributors
[m417z](https://github.com/m417z/), 
Krisatisa
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- normal_window_border:
    - enable_custom_border: true
      $name: Enable custom border for normal windows
      $description: If disabled, normal non-maximized windows keep the stock border behavior.
    - color: "#80FFFFFF"
      $name: Light Color
      $description: 'ARGB hex format (e.g. #80FFFFFF) or `accent`'
    - dark_color: "#80202020"
      $name: Dark Color
      $description: 'Used when window is in Dark Mode.'
    - enable_unfocused: false
      $name: Enable Unfocused Window Colors
      $description: Apply distinct colors when the window loses focus.
    - unfocused_color: "#80C8C8C8"
      $name: Unfocused Light Color
    - unfocused_dark_color: "#80141414"
      $name: Unfocused Dark Color
  $name: Normal window border

- maximized_window_border:
    - show_custom_border: false
      $name: Show custom border when maximized
    - color: "#80FFFFFF"
      $name: Light Color
    - dark_color: "#80202020"
      $name: Dark Color
    - enable_unfocused: false
      $name: Enable Unfocused Window Colors
    - unfocused_color: "#80C8C8C8"
      $name: Unfocused Light Color
    - unfocused_dark_color: "#80141414"
      $name: Unfocused Dark Color
  $name: Maximized window border

- context_menu_border:
    - enable_custom_border: false
      $name: Enable custom border for context menus
    - color: "#80FFFFFF"
      $name: Light Color
    - dark_color: "#80202020"
      $name: Dark Color
  $name: Context menu border

- tooltip_border:
    - enable_custom_border: false
      $name: Enable custom border for tooltips
    - color: "#80FFFFFF"
      $name: Light Color
    - dark_color: "#80202020"
      $name: Dark Color
  $name: Tooltip border
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <winevt.h>
#include <unordered_map>
#include <atomic>
#include <cmath>

#define SHARED_SECTION __attribute__((section(".shared")))
asm(".section .shared,\"dws\"\n");
volatile LONG g_isDarkTheme SHARED_SECTION = 0;
volatile LONG g_explorerPID SHARED_SECTION = 0;
volatile LONG g_crashStrikes SHARED_SECTION = 0;

HANDLE g_hStopEvent = NULL;
HANDLE g_hExplorerThread = NULL;

bool IsExplorerProcess() {
    static bool isExplorer = []() {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(NULL, path, ARRAYSIZE(path));
        wchar_t* name = wcsrchr(path, L'\\');
        name = name ? name + 1 : path;
        return (_wcsicmp(name, L"explorer.exe") == 0);
    }();
    return isExplorer;
}

// --- EXPLORER THEME PUBLISHER ---
DWORD WINAPI ExplorerThemeWorker(LPVOID lpParam) {
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    using ShouldAppsUseDarkMode_t = bool (WINAPI *)();
    ShouldAppsUseDarkMode_t ShouldAppsUseDarkMode = nullptr;
    if (hUxTheme) {
        ShouldAppsUseDarkMode = (ShouldAppsUseDarkMode_t)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(132));
    }

    while (true) {
        if (ShouldAppsUseDarkMode) {
            LONG darkState = ShouldAppsUseDarkMode() ? 1 : 0;
            InterlockedExchange(&g_isDarkTheme, darkState);
        }

        DWORD waitRes = MsgWaitForMultipleObjects(1, &g_hStopEvent, FALSE, 500, QS_ALLINPUT);
        if (waitRes == WAIT_OBJECT_0) {
            break;
        } else if (waitRes == WAIT_OBJECT_0 + 1) {
            MSG msg;
            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    if (hUxTheme) FreeLibrary(hUxTheme);
    return 0;
}

// --- DWM ENGINE ---
struct DWM_COLOR_VALUE {
    float r, g, b, a;
};
struct BorderStateColors {
    DWM_COLOR_VALUE light;
    DWM_COLOR_VALUE dark;
};

struct BorderCategorySettings {
    bool enabled;
    bool enableUnfocused;
    BorderStateColors focused;
    BorderStateColors unfocused;
};

struct BorderSettings {
    BorderCategorySettings normal;
    BorderCategorySettings maximized;
    BorderCategorySettings contextMenu;
    BorderCategorySettings tooltip;
};

SRWLOCK g_SettingsLock = SRWLOCK_INIT;
BorderSettings g_Settings = {};
enum class WindowKind {
    Unknown,
    Pending,
    Normal,
    ContextMenu,
    Tooltip,
};
struct CacheEntry {
    WindowKind kind;
    HWND hwnd;
    ULONGLONG lastVerifyTick;
};

std::unordered_map<void*, CacheEntry> g_WindowCache;
SRWLOCK g_CacheLock = SRWLOCK_INIT;
SRWLOCK g_EnumLock = SRWLOCK_INIT; 

std::atomic<bool> g_ModUnloading = false;
std::atomic<ULONGLONG> g_LastEnumTick{0};
static DWM_COLOR_VALUE MakeColorFromHEX(PCWSTR hex) {
    DWM_COLOR_VALUE color = {1.0f, 1.0f, 1.0f, 1.0f};
    if (!hex) return color;
    if (_wcsicmp(hex, L"accent") == 0) {
        DWORD colorizationColor = 0;
        BOOL opaqueBlend = FALSE;
        HMODULE hDwmApi = LoadLibraryW(L"dwmapi.dll");
        if (hDwmApi) {
            typedef HRESULT(WINAPI *DwmGetColorizationColor_t)(DWORD*, BOOL*);
            DwmGetColorizationColor_t pDwmGetColorizationColor = 
                (DwmGetColorizationColor_t)GetProcAddress(hDwmApi, "DwmGetColorizationColor");
            if (pDwmGetColorizationColor && SUCCEEDED(pDwmGetColorizationColor(&colorizationColor, &opaqueBlend))) {
                color.a = ((colorizationColor >> 24) & 0xFF) / 255.0f;
                if (color.a <= 0.0f) color.a = 1.0f / 255.0f;
                color.r = ((colorizationColor >> 16) & 0xFF) / 255.0f;
                color.g = ((colorizationColor >> 8) & 0xFF) / 255.0f;
                color.b = (colorizationColor & 0xFF) / 255.0f;
            }
            FreeLibrary(hDwmApi);
        }
        return color;
    }

    if (hex[0] == L'#') hex++;
    unsigned int argb = 0;
    if (swscanf(hex, L"%08X", &argb) == 1) {
        color.a = ((argb >> 24) & 0xFF) / 255.0f;
        if (color.a <= 0.0f) color.a = 1.0f / 255.0f;
        color.r = ((argb >> 16) & 0xFF) / 255.0f;
        color.g = ((argb >> 8) & 0xFF) / 255.0f;
        color.b = (argb & 0xFF) / 255.0f;
    }
    return color;
}

bool HasDwminitWarningInLastMinute() {
    const WCHAR* queryPath = L"Application";
    const WCHAR* query =
        L"*[System[Provider[@Name='Dwminit'] and (Level=3) and "
        L"TimeCreated[timediff(@SystemTime) <= 60000]]]";
    EVT_HANDLE queryHandle = EvtQuery(nullptr, queryPath, query, EvtQueryChannelPath);
    if (!queryHandle) {
        Wh_Log(L"EvtQuery failed with error: %u", GetLastError());
        return false;
    }

    bool found = false;
    EVT_HANDLE eventHandle = nullptr;
    DWORD dwReturned = 0;
    if (EvtNext(queryHandle, 1, &eventHandle, 1000, 0, &dwReturned)) {
        found = true;
        EvtClose(eventHandle);
    }

    EvtClose(queryHandle);
    return found;
}

bool CheckCrashLoopFailsafe() {
    if (!HasDwminitWarningInLastMinute()) {
        InterlockedExchange((volatile LONG*)&g_crashStrikes, 0);
        return true;
    }

    LONG strikes = InterlockedIncrement((volatile LONG*)&g_crashStrikes);

    if (strikes >= 2) {
        Wh_Log(L"CRASH LOOP DETECTED: Dwminit warning present on %ld consecutive loads. Disabling mod.", strikes);
        return false;
    }

    Wh_Log(L"Dwminit warning detected but allowing first attempt (strike %ld/2).", strikes);
    return true;
}

typedef int (*EdgeBorderMustBeOpaque_t)(void* pThis);
EdgeBorderMustBeOpaque_t EdgeBorderMustBeOpaque_Orig;

int EdgeBorderMustBeOpaque_Hook(void* pThis) {
    return 0;
}

using SetBorderParameters_t = long(WINAPI*)(void* pThis,
                                            const RECT& borderRect,
                                            float cornerRadius,
                                            int dpi,
                                            const DWM_COLOR_VALUE& color,
                                            int borderStyle,
                                            int shadowStyle);
SetBorderParameters_t SetBorderParameters_Orig;

struct WindowMatchContext {
    RECT borderRect;
    HWND bestHwnd;
    int bestScore;
};
static int ScoreWindowMatch(HWND hwnd, const RECT& borderRect) {
    if (!IsWindowVisible(hwnd)) return 0;

    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc)) return 0;

    LONG width = rc.right - rc.left;
    LONG height = rc.bottom - rc.top;
    LONG borderWidth = borderRect.right - borderRect.left;
    LONG borderHeight = borderRect.bottom - borderRect.top;
    if (width <= 0 || height <= 0 || borderWidth <= 0 || borderHeight <= 0) return 0;
    LONG dw = std::abs(width - borderWidth);
    LONG dh = std::abs(height - borderHeight);
    if (dw > 200 || dh > 200) return 0;

    int score = 0;
    if (dw <= 4 && dh <= 4) score += 150;
    else if (dw <= 15 && dh <= 15) score += 100;
    else if (dw <= 50 && dh <= 50) score += 50;
    else score += 10;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    bool iconic = IsIconic(hwnd);
    HWND owner = GetWindow(hwnd, GW_OWNER);
    score += 35;
    if (!iconic) score += 10;
    if (owner) score += 12;
    if (exStyle & WS_EX_TOOLWINDOW) score += 12;
    if (!(style & WS_CHILD)) score += 8;

    wchar_t className[256];
    className[0] = L'\0';
    className[255] = L'\0';
    
    wchar_t ownerClassName[256];
    ownerClassName[0] = L'\0';
    ownerClassName[255] = L'\0';
    
    GetClassNameW(hwnd, className, 255);
    if (owner) GetClassNameW(owner, ownerClassName, 255);
    if (className[0] == L't' && wcscmp(className, L"tooltips_class32") == 0) score += 120;
    else if (className[0] == L'#' && wcscmp(className, L"#32768") == 0) score += 120;
    else if (className[0] == L'X' && wcscmp(className, L"Xaml_WindowedPopupClass") == 0) score += 95;
    else if (className[0] == L'W' && wcscmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) score += 40;
    if (ownerClassName[0] == L'S' && wcscmp(ownerClassName, L"Shell_TrayWnd") == 0) score += 25;

    return score;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (g_ModUnloading) return FALSE;

    WindowMatchContext* ctx = reinterpret_cast<WindowMatchContext*>(lParam);
    if (!ctx) return FALSE;

    int score = ScoreWindowMatch(hwnd, ctx->borderRect);
    if (score > ctx->bestScore) {
        ctx->bestScore = score;
        ctx->bestHwnd = hwnd;
        if (score >= 180) {
            return FALSE;
        }
    }
    return TRUE;
}

struct WindowClassification {
    WindowKind kind;
    HWND hwnd;
};

static WindowClassification ClassifyWindow(const RECT& borderRect) {
    WindowMatchContext ctx = {};
    ctx.borderRect = borderRect;
    ctx.bestHwnd = nullptr;
    ctx.bestScore = 0;

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&ctx));

    if (!ctx.bestHwnd) return {WindowKind::Normal, nullptr};

    wchar_t className[256];
    className[0] = L'\0';
    className[255] = L'\0';
    GetClassNameW(ctx.bestHwnd, className, 255);

    if (className[0] == L'#') {
        if (wcscmp(className, L"#32768") == 0) return {WindowKind::ContextMenu, ctx.bestHwnd};
    } else if (className[0] == L't') {
        if (wcscmp(className, L"tooltips_class32") == 0) return {WindowKind::Tooltip, ctx.bestHwnd};
    }

    return {WindowKind::Normal, ctx.bestHwnd};
}

static bool IsCacheEntryValid(HWND hwnd, const RECT& borderRect) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc)) return false;
    LONG dw = std::abs((rc.right - rc.left) - (borderRect.right - borderRect.left));
    LONG dh = std::abs((rc.bottom - rc.top) - (borderRect.bottom - borderRect.top));
    return (dw <= 200 && dh <= 200);
}

WindowKind CheckWindowCache(void* pThis, const RECT& rect, HWND* outHwnd) {
    if (g_ModUnloading) return WindowKind::Unknown;

    ULONGLONG currentTick = GetTickCount64();
    ULONGLONG verifyThreshold = 1000 + (reinterpret_cast<uintptr_t>(pThis) % 1000);

    AcquireSRWLockShared(&g_CacheLock);
    auto it = g_WindowCache.find(pThis);
    if (it != g_WindowCache.end()) {
        CacheEntry entry = it->second;
        if (currentTick - entry.lastVerifyTick > verifyThreshold) {
            ReleaseSRWLockShared(&g_CacheLock);

            AcquireSRWLockExclusive(&g_CacheLock);
            auto itEx = g_WindowCache.find(pThis);
            if (itEx != g_WindowCache.end()) {
                if (!IsCacheEntryValid(itEx->second.hwnd, rect)) {
                    g_WindowCache.erase(itEx);
                    ReleaseSRWLockExclusive(&g_CacheLock);
                } else {
                    itEx->second.lastVerifyTick = currentTick;
                    WindowKind cachedKind = itEx->second.kind;
                    HWND cachedHwnd = itEx->second.hwnd;
                    ReleaseSRWLockExclusive(&g_CacheLock);
                    if (outHwnd) *outHwnd = cachedHwnd;
                    return cachedKind;
                }
            } else {
                ReleaseSRWLockExclusive(&g_CacheLock);
            }
        } else {
            ReleaseSRWLockShared(&g_CacheLock);
            if (outHwnd) *outHwnd = entry.hwnd;
            return entry.kind;
        }
    } else {
        ReleaseSRWLockShared(&g_CacheLock);
    }

    ULONGLONG lastEnum = g_LastEnumTick.load(std::memory_order_relaxed);
    if (currentTick - lastEnum < 15) { 
        if (outHwnd) *outHwnd = NULL;
        return WindowKind::Pending; 
    }

    AcquireSRWLockExclusive(&g_EnumLock);
    
    lastEnum = g_LastEnumTick.load(std::memory_order_relaxed);
    if (currentTick - lastEnum < 15) {
        ReleaseSRWLockExclusive(&g_EnumLock);
        if (outHwnd) *outHwnd = NULL;
        return WindowKind::Pending;
    }

    AcquireSRWLockShared(&g_CacheLock);
    auto itDouble = g_WindowCache.find(pThis);
    if (itDouble != g_WindowCache.end()) {
        CacheEntry doubleEntry = itDouble->second;
        ReleaseSRWLockShared(&g_CacheLock);
        ReleaseSRWLockExclusive(&g_EnumLock);
        if (outHwnd) *outHwnd = doubleEntry.hwnd;
        return doubleEntry.kind;
    }
    ReleaseSRWLockShared(&g_CacheLock);

    g_LastEnumTick.store(GetTickCount64(), std::memory_order_relaxed);
    WindowClassification result = ClassifyWindow(rect);
    ReleaseSRWLockExclusive(&g_EnumLock);

    AcquireSRWLockExclusive(&g_CacheLock);
    if (g_WindowCache.size() > 4096) {
        ULONGLONG expireTick = currentTick - 5000;
        int pruned = 0;
        for (auto iter = g_WindowCache.begin(); iter != g_WindowCache.end() && pruned < 128; ) {
            if (iter->second.lastVerifyTick < expireTick) {
                iter = g_WindowCache.erase(iter);
                pruned++;
            } else {
                ++iter;
            }
        }
        if (g_WindowCache.size() > 8192) {
            g_WindowCache.clear();
        }
    }
    g_WindowCache[pThis] = {result.kind, result.hwnd, currentTick}; 
    ReleaseSRWLockExclusive(&g_CacheLock);

    if (outHwnd) *outHwnd = result.hwnd;
    return result.kind;
}

long WINAPI SetBorderParameters_Hook(void* pThis,
                                     const RECT& borderRect,
                                     float cornerRadius,
                                     int dpi,
                                     const DWM_COLOR_VALUE& color,
                                     int borderStyle,
                                     int shadowStyle) {
    if (g_ModUnloading) return SetBorderParameters_Orig(pThis, borderRect, cornerRadius, dpi, color, borderStyle, shadowStyle);
    HWND hwnd = NULL;
    WindowKind popupKind = CheckWindowCache(pThis, borderRect, &hwnd);
    if (popupKind == WindowKind::Pending) {
        return SetBorderParameters_Orig(pThis, borderRect, cornerRadius, dpi, color, borderStyle, shadowStyle);
    }

    bool isDarkTheme = (g_isDarkTheme == 1);
    bool isFocused = true; 

    if (hwnd && popupKind == WindowKind::Normal) {
        isFocused = (hwnd == GetForegroundWindow());
    }

    DWM_COLOR_VALUE effectiveColor = color;

    AcquireSRWLockShared(&g_SettingsLock);
    
    BorderCategorySettings* cat = nullptr;
    if (popupKind == WindowKind::ContextMenu) cat = &g_Settings.contextMenu;
    else if (popupKind == WindowKind::Tooltip) cat = &g_Settings.tooltip;
    else if (color.a <= 0.001f) cat = &g_Settings.maximized;
    else cat = &g_Settings.normal;
    if (cat->enabled) {
        BorderStateColors* stateColors = (cat->enableUnfocused && !isFocused) ? &cat->unfocused : &cat->focused;
        effectiveColor = isDarkTheme ? stateColors->dark : stateColors->light;
    }

    ReleaseSRWLockShared(&g_SettingsLock);

    return SetBorderParameters_Orig(pThis, borderRect, cornerRadius, dpi, effectiveColor, borderStyle, shadowStyle);
}

#define LOAD_COLOR_STATE(prefix, state, out) \
    { \
        PCWSTR hexLight = Wh_GetStringSetting(prefix L"." state L"color"); \
        out.light = MakeColorFromHEX(hexLight); \
        Wh_FreeStringSetting(hexLight); \
        PCWSTR hexDark = Wh_GetStringSetting(prefix L"." state L"dark_color"); \
        out.dark = MakeColorFromHEX(hexDark); \
        Wh_FreeStringSetting(hexDark); \
    }

#define LOAD_CATEGORY_WITH_FOCUS(prefix, field, enable_key) \
    newSettings.field.enabled = Wh_GetIntSetting(prefix L"." enable_key) != 0; \
    newSettings.field.enableUnfocused = Wh_GetIntSetting(prefix L".enable_unfocused") != 0; \
    LOAD_COLOR_STATE(prefix, L"", newSettings.field.focused); \
    LOAD_COLOR_STATE(prefix, L"unfocused_", newSettings.field.unfocused)

#define LOAD_CATEGORY_SIMPLE(prefix, field, enable_key) \
    newSettings.field.enabled = Wh_GetIntSetting(prefix L"." enable_key) != 0; \
    newSettings.field.enableUnfocused = false; \
    LOAD_COLOR_STATE(prefix, L"", newSettings.field.focused)

void LoadSettings() {
    BorderSettings newSettings;
    LOAD_CATEGORY_WITH_FOCUS(L"normal_window_border", normal, L"enable_custom_border");
    LOAD_CATEGORY_WITH_FOCUS(L"maximized_window_border", maximized, L"show_custom_border");
    
    LOAD_CATEGORY_SIMPLE(L"context_menu_border", contextMenu, L"enable_custom_border");
    LOAD_CATEGORY_SIMPLE(L"tooltip_border", tooltip, L"enable_custom_border");

    AcquireSRWLockExclusive(&g_SettingsLock);
    g_Settings = newSettings;
    ReleaseSRWLockExclusive(&g_SettingsLock);
}

BOOL Wh_ModInit() {
    g_ModUnloading = false;
    
    if (!IsExplorerProcess() && !CheckCrashLoopFailsafe()) return FALSE;

    if (IsExplorerProcess()) {
        LONG pid = (LONG)GetCurrentProcessId();
        LONG oldPid = InterlockedCompareExchange((volatile LONG*)&g_explorerPID, pid, 0);
        if (oldPid != 0 && oldPid != pid) {
            HANDLE hProc = OpenProcess(SYNCHRONIZE, FALSE, (DWORD)oldPid);
            if (hProc) {
                bool alive = (WaitForSingleObject(hProc, 0) == WAIT_TIMEOUT);
                CloseHandle(hProc);
                if (alive) {
                    Wh_Log(L"[Explorer Hook] Secondary explorer alive. Skip.");
                    return FALSE; 
                }
            }
            if (InterlockedCompareExchange((volatile LONG*)&g_explorerPID, pid, oldPid) != oldPid) {
                Wh_Log(L"[Explorer Hook] Race lost. Skip.");
                return FALSE;
            }
        }

        g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
        Wh_Log(L"[Explorer Hook] Initializing theme publisher thread...");
        g_hExplorerThread = CreateThread(NULL, 0, ExplorerThemeWorker, NULL, 0, NULL);
        return TRUE;
    }

    Wh_Log(L"[DWM Hook] Initializing Custom Translucent Borders...");
    LoadSettings();

    HMODULE udwm = LoadLibraryW(L"uDWM.dll");
    if (!udwm) return FALSE;
    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {
                LR"(private: int __cdecl CTopLevelWindow::EdgeBorderMustBeOpaque(void))",
                LR"(private: int __cdecl CTopLevelWindow::EdgeBorderMustBeOpaque(void)const)"
            },
            (void**)&EdgeBorderMustBeOpaque_Orig,
            (void*)EdgeBorderMustBeOpaque_Hook,
            false
        },
        {
            {
                LR"(public: long __cdecl CWindowBorder::SetBorderParameters(struct tagRECT const &,float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle))"
            },
            (void**)&SetBorderParameters_Orig,
            (void*)SetBorderParameters_Hook,
            false
        }
    };
    if (!WindhawkUtils::HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"Failed to hook Windows 11 border functions.");
        return FALSE;
    }

    Wh_Log(L"Custom Translucent Borders injected successfully.");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (!IsExplorerProcess()) {
        LoadSettings();
        PostMessageW(HWND_BROADCAST, WM_DWMCOLORIZATIONCOLORCHANGED, 0, 0);
    }
}

void Wh_ModUninit() {
    g_ModUnloading = true;

    if (IsExplorerProcess()) {
        if (g_hStopEvent) {
            SetEvent(g_hStopEvent);
        }
        if (g_hExplorerThread) {
            WaitForSingleObject(g_hExplorerThread, 1000);
            CloseHandle(g_hExplorerThread);
            g_hExplorerThread = NULL;
        }
        InterlockedCompareExchange((volatile LONG*)&g_explorerPID, 0, (LONG)GetCurrentProcessId());
        if (g_hStopEvent) {
            CloseHandle(g_hStopEvent);
            g_hStopEvent = NULL;
        }
    } else {
        AcquireSRWLockExclusive(&g_CacheLock);
        g_WindowCache.clear();
        ReleaseSRWLockExclusive(&g_CacheLock);
    }
}
