// ==WindhawkMod==
// @id              taskbar-keyboard-layout-flag
// @name            Taskbar Keyboard Layout Flag
// @description     Replaces the Windows taskbar input language and keyboard layout indicator with a country flag.
// @version         1.0
// @author          akiyorm
// @github          https://github.com/akiyorm
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lgdiplus -luxtheme -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Keyboard Layout Flag

Replaces the Windows taskbar input language and keyboard layout indicator with a crisp, minimalist country flag.

## Overview
| Disabled | Enabled |
| :---: | :---: |
| ![Disabled](https://i.imgur.com/eS8PeBa.png) | ![Enabled](https://i.imgur.com/HYsZyxR.png) |

By default, Windows displays a two-line text abbreviation in the system tray to indicate the active input language and keyboard layout. This mod replaces that text indicator with a high-resolution, properly proportioned country flag while preserving native taskbar transparency, acrylic effects, and hover/click highlights.

## Supported Country Flags

Includes 24 custom-rendered flags in an authentic 3:2 aspect ratio (shown with **Rounded Corners** enabled), with a clean two-letter ISO fallback badge for any other locale:

![Supported Country Flags](https://i.imgur.com/ctzxW7I.png)

`TR`, `US`, `UK`, `CA`, `AU`, `DE`, `CH`, `AT`, `FR`, `BE`, `IT`, `ES`, `PT`, `BR`, `RU`, `UA`, `AZ`, `JP`, `KR`, `CN`, `NL`, `PL`, `SE`, `GR`

## Features

- **4x Supersampled Vector Rendering**: Flags are rendered at 4x resolution and downsampled with high-quality bicubic filtering for sharp edges and fine emblem details on any DPI scale.
- **Native Taskbar Visual Integration**: Composites directly onto the taskbar's native buffered surface so background transparency, blur, and pointer hover/click states remain intact.
- **Accurate Layout & Regional Detection**: Identifies both cross-language keyboard layouts and regional language variants that share a base keyboard layout (such as `English (Australia)` for `AU` and `German (Austria)` for `AT`).
- **UK Flag Option for US Layout**: Optional setting to display the United Kingdom flag whenever the English (US) keyboard layout is active.
- **3:2 Scale Presets**: Five size presets (1 to 5) that scale proportionally while maintaining a strict 3:2 flag ratio.
- **Optional Dark Mode Palette**: Transforms shape-distinctive flags (`TR`, `US`, `UK`, `CA`, `KR`, `GR`) into a sleek deep dark grey and white monochrome palette while keeping color-dependent flags in their standard colors.
- **Dark Mode Sample** (for UK Layout): ![Dark Mode Country Flags](https://i.imgur.com/K8tdMiB.png)

## Settings

- **Flag Scale (1 - 5)**: Controls the size of the taskbar flag (`3` by default).
- **Rounded Corners**: Applies subtle rounded corners and a hairline border to the flag (`true` by default).
- **Dark Mode**: Renders supported monochrome-friendly flags (`TR`, `US`, `UK`, `CA`, `KR`, `GR`) in a deep dark grey and white theme.
- **Use UK Flag for US Layout**: Displays the UK flag instead of the US flag when the English (US) layout is active.

## Compatibility

- Supports **Windows 10** taskbar input indicators (`TrayInputIndicatorWClass`), including multi-monitor secondary taskbars and per-monitor DPI scaling.
- Supports **Windows 11** taskbar XAML input indicators (`SystemTray::IconView`) as well as Windows 10-style taskbar implementations.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FlagScale: 3
  $name: Flag Scale (1 - 5)
  $description: Size of the taskbar flag (default preset 3).
- RoundedCorners: true
  $name: Rounded Corners
  $description: Adds subtle rounded corners to the flag icon.
- DarkMode: false
  $name: Dark Mode
  $description: "Renders supported flags (TR, US, UK, CA, KR, GR) in a deep dark grey and white monochrome style. Other flags remain in standard color."
- UseUkFlagForUsLayout: false
  $name: Use UK Flag for US Layout
  $description: Displays the United Kingdom (UK) flag instead of the US flag when the English (US) keyboard layout is active.
*/
// ==/WindhawkModSettings==

// 1. Standard C++ headers (placed first to prevent Clang module collisions)
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <mutex>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <atomic>
#include <optional>

// 2. Win32, UxTheme, Direct2D, DirectWrite, GDI+ and WinRT base headers
#include <windows.h>
#include <unknwn.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <d2d1.h>
#include <dwrite.h>
#include <gdiplus.h>
#include <robuffer.h>

// 3. C++/WinRT headers
#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

// 4. Windhawk utility library
#include <windhawk_utils.h>

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;

// ============================================================================
// MOD SETTINGS & GLOBAL STATE
// ============================================================================

struct ModSettings {
    int flagScale;              // 1..5 (default 3 -> 24x16, 3:2 aspect ratio)
    int flagWidth;              // Computed: 15 + flagScale * 3 (18, 21, 24, 27, 30)
    int flagHeight;             // Computed: 10 + flagScale * 2 (12, 14, 16, 18, 20)
    bool roundedCorners;
    bool darkMode;              // Default false
    bool useUkFlagForUsLayout;  // Default false
};

struct FlagCacheKey {
    std::wstring iso;
    int width;
    int height;
    bool roundedCorners;
    bool darkMode;

    bool operator==(const FlagCacheKey& other) const {
        return width == other.width &&
               height == other.height &&
               roundedCorners == other.roundedCorners &&
               darkMode == other.darkMode &&
               iso == other.iso;
    }
};

struct FlagCacheKeyHash {
    size_t operator()(const FlagCacheKey& k) const {
        size_t h = std::hash<std::wstring>{}(k.iso);
        h ^= static_cast<size_t>(k.width) * 2654435761u + 0x9e3779b9u + (h << 6) + (h >> 2);
        h ^= static_cast<size_t>(k.height) * 2246822519u + 0x9e3779b9u + (h << 6) + (h >> 2);
        h ^= (k.roundedCorners ? 1u : 0u) | (k.darkMode ? 2u : 0u);
        return h;
    }
};

// Trivially destructible per-thread tracker for active BeginBufferedPaint memory DCs.
// Avoids heap allocations or non-trivial thread_local destructors in explorer.exe.
struct ActivePaintDCSet {
    HDC dcs[8];
    int count;

    void Insert(HDC hdc) {
        if (!hdc || Contains(hdc)) return;
        if (count < 8) {
            dcs[count++] = hdc;
        }
    }

    void Erase(HDC hdc) {
        for (int i = 0; i < count; ++i) {
            if (dcs[i] == hdc) {
                dcs[i] = dcs[--count];
                return;
            }
        }
    }

    bool Contains(HDC hdc) const {
        for (int i = 0; i < count; ++i) {
            if (dcs[i] == hdc) return true;
        }
        return false;
    }

    bool Empty() const {
        return count == 0;
    }
};

struct ReentrancyFlagGuard {
    bool& flag;
    explicit ReentrancyFlagGuard(bool& f) : flag(f) { flag = true; }
    ~ReentrancyFlagGuard() { flag = false; }
    ReentrancyFlagGuard(const ReentrancyFlagGuard&) = delete;
    ReentrancyFlagGuard& operator=(const ReentrancyFlagGuard&) = delete;
};

struct ReentrancyCounterGuard {
    int& counter;
    explicit ReentrancyCounterGuard(int& c) : counter(c) { ++counter; }
    ~ReentrancyCounterGuard() { --counter; }
    ReentrancyCounterGuard(const ReentrancyCounterGuard&) = delete;
    ReentrancyCounterGuard& operator=(const ReentrancyCounterGuard&) = delete;
};

std::shared_mutex g_stateLock;
ModSettings g_settings = { 3, 24, 16, true, false, false };
ULONG_PTR g_gdiplusToken = 0;
std::vector<HWND> g_subclassedIndicators;
std::vector<HMODULE> g_loadedModules;
std::atomic<HWND> g_shellTrayHwnd{ nullptr };
std::atomic<HKL> g_lastKnownHkl{ nullptr };
std::atomic<bool> g_modUnloading{ false };
std::atomic<bool> g_systemTrayHookAttempted{ false };

std::unordered_map<WORD, LANGID> g_deviceLayoutCache;
std::unordered_map<LANGID, std::wstring> g_countryCodeCache;
std::unordered_map<FlagCacheKey, std::vector<BYTE>, FlagCacheKeyHash> g_flagPixelCache;
std::unordered_set<void*> g_hookedD2DVtables;

thread_local bool g_inOurPaint = false;
thread_local int g_inIndicatorNativePaint = 0;
thread_local bool g_bufferedPaintHandled = false;
thread_local ActivePaintDCSet g_activeIndicatorPaintDCs = {};

constexpr UINT_PTR kWatchdogTimerId = 0x464C478;
UINT g_stopWatchdogMsg = 0;
UINT g_runFromWindowThreadMsg = 0;

ModSettings GetSettingsSnapshot() {
    std::shared_lock lock(g_stateLock);
    return g_settings;
}

HMODULE GetOrLoadSystemModule(const wchar_t* dllName) {
    HMODULE mod = GetModuleHandleW(dllName);
    if (!mod) {
        mod = LoadLibraryExW(dllName, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (mod) {
            g_loadedModules.push_back(mod);
        }
    }
    return mod;
}

// ============================================================================
// 0. UI THREAD DISPATCH HELPER
// ============================================================================

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    if (!hWnd || !IsWindow(hWnd) || !g_runFromWindowThreadMsg) {
        return false;
    }

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == g_runFromWindowThreadMsg) {
                    auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr,
        dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{ proc, procParam };
    SendMessageW(hWnd, g_runFromWindowThreadMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

// ============================================================================
// 1. ACTIVE KEYBOARD LAYOUT & COUNTRY RESOLUTION
// ============================================================================

struct KeyboardInfo {
    LANGID inputLangId;     // Top line language ID (e.g., 0x0409, 0x0C09)
    LANGID countryLangId;   // Bottom line layout country ID (e.g., 0x041F, 0x0409)
    WORD rawLayoutWord;     // HIWORD(hkl)
};

LANGID ResolveLayoutCountryLangId(HKL hkl) {
    if (!hkl) return 0x0409;

    ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(hkl);
    LANGID inputLang = LOWORD(raw);
    WORD layoutWord = HIWORD(raw);

    if (layoutWord == 0) {
        return inputLang;
    }

    // Legacy IME HKL (high nibble 0xE, e.g., 0xE00E0804 = Chinese Pinyin, 0xE0010411 = Japanese IME):
    // HIWORD is the IME device identifier, while LOWORD is the actual target LANGID.
    if ((layoutWord & 0xF000) == 0xE000) {
        return inputLang;
    }

    // Standard keyboard layout identifier (e.g., 0x0409 = US, 0x041F = TRQ, 0x0809 = UK)
    if ((layoutWord & 0xF000) != 0xF000) {
        return static_cast<LANGID>(layoutWord);
    }

    // Substitute / device keyboard layout identifier (0xF0xx, e.g., Turkish F = 0001041F)
    WORD deviceId = layoutWord & 0x0FFF;
    {
        std::shared_lock lock(g_stateLock);
        auto it = g_deviceLayoutCache.find(deviceId);
        if (it != g_deviceLayoutCache.end()) {
            return it->second;
        }
    }

    LANGID resolved = inputLang;
    HKEY hKeyLayouts = nullptr;
    if (RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts",
            0,
            KEY_READ,
            &hKeyLayouts) == ERROR_SUCCESS) {
        wchar_t subKeyName[32];
        DWORD index = 0;
        while (true) {
            DWORD nameLen = ARRAYSIZE(subKeyName);
            if (RegEnumKeyExW(hKeyLayouts, index++, subKeyName, &nameLen, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
                break;
            }
            if (nameLen == 8) {
                wchar_t layoutIdStr[16] = {};
                DWORD dataSize = sizeof(layoutIdStr);
                if (RegGetValueW(
                        hKeyLayouts,
                        subKeyName,
                        L"Layout Id",
                        RRF_RT_REG_SZ,
                        nullptr,
                        layoutIdStr,
                        &dataSize) == ERROR_SUCCESS) {
                    WORD regDeviceId = static_cast<WORD>(wcstoul(layoutIdStr, nullptr, 16));
                    if (regDeviceId == deviceId) {
                        LANGID parsed = static_cast<LANGID>(wcstoul(subKeyName + 4, nullptr, 16));
                        if (parsed != 0) {
                            resolved = parsed;
                        }
                        break;
                    }
                }
            }
        }
        RegCloseKey(hKeyLayouts);
    }

    {
        std::unique_lock lock(g_stateLock);
        g_deviceLayoutCache[deviceId] = resolved;
    }
    return resolved;
}

std::wstring ResolveIsoCountryCode(LANGID langId) {
    {
        std::shared_lock lock(g_stateLock);
        auto it = g_countryCodeCache.find(langId);
        if (it != g_countryCodeCache.end()) {
            return it->second;
        }
    }

    std::wstring code;
    wchar_t isoBuf[16] = {};
    LCID lcid = MAKELCID(langId, SORT_DEFAULT);
    if (GetLocaleInfoW(lcid, LOCALE_SISO3166CTRYNAME, isoBuf, ARRAYSIZE(isoBuf)) > 0 && isoBuf[0] != L'\0') {
        code = isoBuf;
    }

    if (code.empty()) {
        switch (PRIMARYLANGID(langId)) {
            case LANG_TURKISH:    code = L"TR"; break;
            case LANG_ENGLISH:    code = L"US"; break;
            case LANG_GERMAN:     code = L"DE"; break;
            case LANG_FRENCH:     code = L"FR"; break;
            case LANG_SPANISH:    code = L"ES"; break;
            case LANG_ITALIAN:    code = L"IT"; break;
            case LANG_RUSSIAN:    code = L"RU"; break;
            case LANG_UKRAINIAN:  code = L"UA"; break;
            case LANG_AZERI:      code = L"AZ"; break;
            case LANG_JAPANESE:   code = L"JP"; break;
            case LANG_KOREAN:     code = L"KR"; break;
            case LANG_CHINESE:    code = L"CN"; break;
            case LANG_PORTUGUESE: code = L"PT"; break;
            case LANG_DUTCH:      code = L"NL"; break;
            case LANG_POLISH:     code = L"PL"; break;
            case LANG_SWEDISH:    code = L"SE"; break;
            case LANG_GREEK:      code = L"GR"; break;
            default:              code = L"??"; break;
        }
    }

    {
        std::unique_lock lock(g_stateLock);
        g_countryCodeCache[langId] = code;
    }
    return code;
}

// Resolves final target country code, handling both cross-language layouts
// and regional language variants that inherit a base keyboard layout.
std::wstring ResolveDisplayCountryCode(const KeyboardInfo& kb) {
    std::wstring layoutIso = ResolveIsoCountryCode(kb.countryLangId);
    std::wstring inputIso  = ResolveIsoCountryCode(kb.inputLangId);

    bool isGenericBaseLayout =
        kb.countryLangId == 0x0409 || // Generic US English layout
        kb.countryLangId == 0x0407 || // Generic German layout
        kb.countryLangId == 0x040C || // Generic French layout
        kb.countryLangId == 0x040A || // Generic Spanish (Traditional) layout
        kb.countryLangId == 0x0C0A || // Generic Spanish (International) layout
        kb.countryLangId == 0x0816 || // Generic Portuguese layout
        kb.countryLangId == 0x0413;   // Generic Dutch layout

    if (isGenericBaseLayout &&
        kb.inputLangId != kb.countryLangId &&
        PRIMARYLANGID(kb.inputLangId) == PRIMARYLANGID(kb.countryLangId) &&
        !inputIso.empty() && inputIso != L"??") {
        return inputIso;
    }

    return layoutIso;
}

bool IsTransientOrTaskbarWindow(HWND hwnd) {
    if (!hwnd) return true;
    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
            return true;
        }
        if (wcscmp(cls, L"#32768") == 0 ||
            wcscmp(cls, L"NotifyIconOverflowWindow") == 0 ||
            wcscmp(cls, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
            wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
            wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid == GetCurrentProcessId()) {
                return true;
            }
        }
    }
    return false;
}

KeyboardInfo GetActiveKeyboardInfo() {
    HWND fg = GetForegroundWindow();
    HKL hkl = nullptr;

    if (fg && !IsTransientOrTaskbarWindow(fg)) {
        wchar_t cls[64] = {};
        if (GetClassNameW(fg, cls, ARRAYSIZE(cls)) > 0 &&
            wcscmp(cls, L"ApplicationFrameWindow") == 0) {
            HWND coreWin = FindWindowExW(fg, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
            if (coreWin) {
                fg = coreWin;
            }
        }

        DWORD threadId = GetWindowThreadProcessId(fg, nullptr);
        if (threadId) {
            hkl = GetKeyboardLayout(threadId);
        }
    }

    if (hkl) {
        g_lastKnownHkl.store(hkl, std::memory_order_relaxed);
    } else {
        hkl = g_lastKnownHkl.load(std::memory_order_relaxed);
    }

    if (!hkl) {
        SystemParametersInfoW(SPI_GETDEFAULTINPUTLANG, 0, &hkl, 0);
    }
    if (!hkl) {
        hkl = GetKeyboardLayout(0);
    }

    ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(hkl);
    LANGID inputLang = LOWORD(raw);
    WORD layoutWord = HIWORD(raw);
    LANGID countryLang = ResolveLayoutCountryLangId(hkl);

    return { inputLang, countryLang, layoutWord };
}

// ============================================================================
// 2. 4X SUPERSAMPLED HIGH-PRECISION FLAG RENDERER (24 COUNTRY FLAGS + DARK MODE)
// ============================================================================

inline Gdiplus::Color C(BYTE r, BYTE g, BYTE b, BYTE a = 255) {
    return Gdiplus::Color(a, r, g, b);
}

// Shared "deep dark grey" (near-black charcoal) used across all Dark Mode supported flags
inline Gdiplus::Color DeepDarkGrey() {
    return C(22, 24, 28);
}

bool IsDarkModeSupportedCountry(std::wstring_view iso) {
    return iso == L"TR" || iso == L"CA" || iso == L"US" ||
           iso == L"GB" || iso == L"KR" || iso == L"GR";
}

void FillSolid(Gdiplus::Graphics& g, float w, float h, Gdiplus::Color col) {
    Gdiplus::SolidBrush b(col);
    g.FillRectangle(&b, -1.0f, -1.0f, w + 2.0f, h + 2.0f);
}

void FillHBands2(Gdiplus::Graphics& g, float w, float h, Gdiplus::Color c1, Gdiplus::Color c2) {
    float mid = h * 0.5f;
    Gdiplus::SolidBrush b1(c1), b2(c2);
    g.FillRectangle(&b1, -1.0f, -1.0f, w + 2.0f, mid + 1.5f);
    g.FillRectangle(&b2, -1.0f, mid, w + 2.0f, h - mid + 1.5f);
}

void FillHBands3(Gdiplus::Graphics& g, float w, float h, Gdiplus::Color c1, Gdiplus::Color c2, Gdiplus::Color c3) {
    float y1 = h / 3.0f;
    float y2 = 2.0f * h / 3.0f;
    Gdiplus::SolidBrush b1(c1), b2(c2), b3(c3);
    g.FillRectangle(&b1, -1.0f, -1.0f, w + 2.0f, y1 + 1.5f);
    g.FillRectangle(&b2, -1.0f, y1 - 0.5f, w + 2.0f, (y2 - y1) + 1.0f);
    g.FillRectangle(&b3, -1.0f, y2 - 0.5f, w + 2.0f, (h - y2) + 1.5f);
}

void FillHBandsSpanish(Gdiplus::Graphics& g, float w, float h, Gdiplus::Color topBot, Gdiplus::Color mid) {
    float y1 = h * 0.25f;
    float y2 = h * 0.75f;
    Gdiplus::SolidBrush b1(topBot), b2(mid);
    g.FillRectangle(&b1, -1.0f, -1.0f, w + 2.0f, y1 + 1.5f);
    g.FillRectangle(&b2, -1.0f, y1 - 0.5f, w + 2.0f, (y2 - y1) + 1.0f);
    g.FillRectangle(&b1, -1.0f, y2 - 0.5f, w + 2.0f, (h - y2) + 1.5f);
}

void FillVBands3(Gdiplus::Graphics& g, float w, float h, Gdiplus::Color c1, Gdiplus::Color c2, Gdiplus::Color c3) {
    float x1 = w / 3.0f;
    float x2 = 2.0f * w / 3.0f;
    Gdiplus::SolidBrush b1(c1), b2(c2), b3(c3);
    g.FillRectangle(&b1, -1.0f, -1.0f, x1 + 1.5f, h + 2.0f);
    g.FillRectangle(&b2, x1 - 0.5f, -1.0f, (x2 - x1) + 1.0f, h + 2.0f);
    g.FillRectangle(&b3, x2 - 0.5f, -1.0f, (w - x2) + 1.5f, h + 2.0f);
}

void DrawStarN(Gdiplus::Graphics& g, Gdiplus::Color col, float cx, float cy,
               int points, float rOuter, float rInner, float angleOffsetDeg = -90.0f) {
    Gdiplus::SolidBrush brush(col);
    std::vector<Gdiplus::PointF> pts(points * 2);
    float step = 180.0f / static_cast<float>(points);
    for (int i = 0; i < points * 2; ++i) {
        float r = (i % 2 == 0) ? rOuter : rInner;
        float rad = (angleOffsetDeg + i * step) * 3.14159265f / 180.0f;
        pts[i] = Gdiplus::PointF(cx + r * std::cos(rad), cy + r * std::sin(rad));
    }
    g.FillPolygon(&brush, pts.data(), static_cast<INT>(pts.size()));
}

void DrawStar(Gdiplus::Graphics& g, Gdiplus::Color col, float cx, float cy,
              float rOuter, float rInner, float angleOffsetDeg = -90.0f) {
    DrawStarN(g, col, cx, cy, 5, rOuter, rInner, angleOffsetDeg);
}

void DrawCrescentAndStar(Gdiplus::Graphics& g, Gdiplus::Color fg, Gdiplus::Color bg,
                         float cx, float cy, float rOuter, float innerOffset, float rInner,
                         float starCx, float starR, int starPoints = 5, float starAngle = 180.0f) {
    Gdiplus::SolidBrush fgB(fg), bgB(bg);
    g.FillEllipse(&fgB, cx - rOuter, cy - rOuter, rOuter * 2.0f, rOuter * 2.0f);
    g.FillEllipse(&bgB, cx + innerOffset - rInner, cy - rInner, rInner * 2.0f, rInner * 2.0f);
    DrawStarN(g, fg, starCx, cy, starPoints, starR, starR * 0.40f, starAngle);
}

// Authentic Union Jack with counterchanged St. Patrick's saltire diagonals
// Uses CombineModeIntersect so outer rounded-corner clipping is preserved.
void DrawUnionJack(Gdiplus::Graphics& g, float x, float y, float w, float h, bool darkMode = false) {
    Gdiplus::GraphicsState state = g.Save();
    g.SetClip(Gdiplus::RectF(x, y, w, h), Gdiplus::CombineModeIntersect);

    const Gdiplus::Color kBlue  = darkMode ? DeepDarkGrey() : C(1, 33, 105);
    const Gdiplus::Color kWhite = C(255, 255, 255);
    const Gdiplus::Color kRed   = darkMode ? DeepDarkGrey() : C(200, 16, 46);

    Gdiplus::SolidBrush blueB(kBlue), whiteB(kWhite), redB(kRed);
    g.FillRectangle(&blueB, x - 1.0f, y - 1.0f, w + 2.0f, h + 2.0f);

    float diagLen = std::hypot(w, h);
    float whiteDiagW = h * 0.20f;
    float redDiagW   = h * 0.067f;

    Gdiplus::Pen whiteDiag(kWhite, whiteDiagW);
    g.DrawLine(&whiteDiag, x, y, x + w, y + h);
    g.DrawLine(&whiteDiag, x + w, y, x, y + h);

    // Counterchanged red St. Patrick's diagonals (offset on each quadrant like official UK flag)
    float ux = w / diagLen;
    float uy = h / diagLen;
    float nx = -uy;
    float ny = ux;
    float shift = redDiagW * 0.52f;
    float cx = x + w * 0.5f;
    float cy = y + h * 0.5f;

    Gdiplus::Pen redDiag(kRed, redDiagW);
    g.DrawLine(&redDiag, x + nx * shift, y + ny * shift, cx + nx * shift, cy + ny * shift);
    g.DrawLine(&redDiag, cx - nx * shift, cy - ny * shift, x + w - nx * shift, y + h - ny * shift);

    float nx2 = uy;
    float ny2 = ux;
    g.DrawLine(&redDiag, x + w - nx2 * shift, y - ny2 * shift, cx - nx2 * shift, cy - ny2 * shift);
    g.DrawLine(&redDiag, cx + nx2 * shift, cy + ny2 * shift, x + nx2 * shift, y + h + ny2 * shift);

    // Central white fimbriation cross & red St. George's cross
    float wVert  = w * 0.222f;
    float wHoriz = h * 0.333f;
    float rVert  = w * 0.133f;
    float rHoriz = h * 0.200f;

    g.FillRectangle(&whiteB, x + (w - wVert) * 0.5f, y - 1.0f, wVert, h + 2.0f);
    g.FillRectangle(&whiteB, x - 1.0f, y + (h - wHoriz) * 0.5f, w + 2.0f, wHoriz);
    g.FillRectangle(&redB,   x + (w - rVert) * 0.5f, y - 1.0f, rVert, h + 2.0f);
    g.FillRectangle(&redB,   x - 1.0f, y + (h - rHoriz) * 0.5f, w + 2.0f, rHoriz);

    g.Restore(state);
}

void DrawFlagByIsoCountry(Gdiplus::Graphics& g, std::wstring_view iso, float w, float h, bool darkMode) {
    const Gdiplus::Color kWhite    = C(255, 255, 255);
    const Gdiplus::Color kBlack    = C(18, 18, 22);
    const Gdiplus::Color kDarkGrey = DeepDarkGrey();
    const Gdiplus::Color kSky      = C(0, 181, 226);
    const Gdiplus::Color kGreen    = C(0, 146, 70);
    const Gdiplus::Color kYellow   = C(254, 204, 2);

    // Supported flags: TR, US, UK (GB), CA, AU, DE, CH, AT, FR, BE, IT, ES, PT, BR, RU, UA, AZ, JP, KR, CN, NL, PL, SE, GR
    // Dark Mode supported flags: TR, US, UK (GB), CA, KR, GR
    if (iso == L"TR") {
        const Gdiplus::Color kTrBg = darkMode ? kDarkGrey : C(227, 10, 23);
        FillSolid(g, w, h, kTrBg);
        DrawCrescentAndStar(
            g, kWhite, kTrBg,
            w * 0.360f, h * 0.50f, h * 0.268f,
            h * 0.067f, h * 0.214f,
            w * 0.595f, h * 0.132f, 5, 180.0f);
    } else if (iso == L"US") {
        // Authentic 13 stripes (7 red/grey, 6 white) + 7-stripe tall canton + staggered white stars
        const Gdiplus::Color kUsStripe = darkMode ? kDarkGrey : C(178, 34, 52);
        const Gdiplus::Color kUsCanton = darkMode ? kDarkGrey : C(60, 59, 110);
        Gdiplus::SolidBrush stripeB(kUsStripe), whiteB(kWhite), cantonB(kUsCanton);

        float stripeH = h / 13.0f;
        for (int i = 0; i < 13; ++i) {
            float y0 = i * stripeH;
            g.FillRectangle((i % 2 == 0) ? &stripeB : &whiteB, -1.0f, y0 - 0.4f, w + 2.0f, stripeH + 0.8f);
        }

        float cw = w * 0.44f;
        float ch = stripeH * 7.0f;
        g.FillRectangle(&cantonB, -1.0f, -1.0f, cw + 1.0f, ch + 1.0f);

        // Staggered 5-row star pattern (5-4-5-4-5 = 23 crisp dots)
        float starR = h * 0.023f;
        for (int row = 0; row < 5; ++row) {
            bool isOddRow = (row % 2 == 1);
            int cols = isOddRow ? 4 : 5;
            float sy = ch * (0.17f + row * 0.165f);
            for (int col = 0; col < cols; ++col) {
                float sx = isOddRow
                    ? cw * (0.21f + col * 0.193f)
                    : cw * (0.115f + col * 0.192f);
                g.FillEllipse(&whiteB, sx - starR, sy - starR, starR * 2.0f, starR * 2.0f);
            }
        }
    } else if (iso == L"GB") {
        DrawUnionJack(g, 0.0f, 0.0f, w, h, darkMode);
    } else if (iso == L"CA") {
        // Authentic 1:2:1 vertical bands + true 11-pointed Canadian Maple Leaf silhouette
        const Gdiplus::Color kCaFg = darkMode ? kDarkGrey : C(216, 6, 33);
        float x1 = w * 0.25f;
        float x2 = w * 0.75f;
        Gdiplus::SolidBrush fgB(kCaFg), whiteB(kWhite);
        g.FillRectangle(&fgB,    -1.0f, -1.0f, x1 + 1.5f, h + 2.0f);
        g.FillRectangle(&whiteB, x1 - 0.5f, -1.0f, (x2 - x1) + 1.0f, h + 2.0f);
        g.FillRectangle(&fgB,    x2 - 0.5f, -1.0f, (w - x2) + 1.5f, h + 2.0f);

        auto P = [&](float nx, float ny) -> Gdiplus::PointF {
            return Gdiplus::PointF(w * 0.50f + nx * h, h * 0.50f + ny * h);
        };
        Gdiplus::PointF leaf[] = {
            P( 0.000f, -0.365f),
            P( 0.055f, -0.245f),
            P( 0.125f, -0.275f),
            P( 0.095f, -0.105f),
            P( 0.205f, -0.195f),
            P( 0.225f, -0.135f),
            P( 0.335f, -0.135f),
            P( 0.285f, -0.025f),
            P( 0.320f,  0.020f),
            P( 0.145f,  0.145f),
            P( 0.165f,  0.200f),
            P( 0.022f,  0.180f),
            P( 0.022f,  0.375f),
            P(-0.022f,  0.375f),
            P(-0.022f,  0.180f),
            P(-0.165f,  0.200f),
            P(-0.145f,  0.145f),
            P(-0.320f,  0.020f),
            P(-0.285f, -0.025f),
            P(-0.335f, -0.135f),
            P(-0.225f, -0.135f),
            P(-0.205f, -0.195f),
            P(-0.095f, -0.105f),
            P(-0.125f, -0.275f),
            P(-0.055f, -0.245f)
        };
        g.FillPolygon(&fgB, leaf, ARRAYSIZE(leaf));
    } else if (iso == L"AU") {
        const Gdiplus::Color kAuBlue = C(1, 33, 105);
        FillSolid(g, w, h, kAuBlue);
        DrawUnionJack(g, 0.0f, 0.0f, w * 0.50f, h * 0.50f, false);
        // Commonwealth 7-pointed star
        DrawStarN(g, kWhite, w * 0.25f, h * 0.75f, 7, h * 0.14f, h * 0.06f);
        // Southern Cross (Crux)
        DrawStarN(g, kWhite, w * 0.75f, h * 0.19f, 7, h * 0.072f, h * 0.032f);
        DrawStarN(g, kWhite, w * 0.63f, h * 0.44f, 7, h * 0.072f, h * 0.032f);
        DrawStarN(g, kWhite, w * 0.87f, h * 0.39f, 7, h * 0.072f, h * 0.032f);
        DrawStarN(g, kWhite, w * 0.75f, h * 0.81f, 7, h * 0.082f, h * 0.036f);
        DrawStarN(g, kWhite, w * 0.80f, h * 0.56f, 5, h * 0.044f, h * 0.019f);
    } else if (iso == L"DE") {
        FillHBands3(g, w, h, kBlack, C(221, 0, 0), C(255, 204, 0));
    } else if (iso == L"CH") {
        FillSolid(g, w, h, C(213, 43, 30));
        Gdiplus::SolidBrush wb(kWhite);
        float arm   = h * 0.62f;
        float thick = h * 0.19f;
        g.FillRectangle(&wb, (w - thick) * 0.5f, (h - arm) * 0.5f, thick, arm);
        g.FillRectangle(&wb, (w - arm) * 0.5f, (h - thick) * 0.5f, arm, thick);
    } else if (iso == L"AT") {
        FillHBands3(g, w, h, C(237, 41, 57), kWhite, C(237, 41, 57));
    } else if (iso == L"FR") {
        FillVBands3(g, w, h, C(0, 38, 84), kWhite, C(237, 41, 57));
    } else if (iso == L"BE") {
        FillVBands3(g, w, h, kBlack, C(253, 218, 36), C(239, 51, 64));
    } else if (iso == L"IT") {
        FillVBands3(g, w, h, kGreen, kWhite, C(206, 43, 55));
    } else if (iso == L"ES") {
        // Authentic 1:2:1 red-gold-red bands + detailed Spanish Coat of Arms
        const Gdiplus::Color kEsRed    = C(170, 21, 27);
        const Gdiplus::Color kEsGold   = C(241, 191, 0);
        const Gdiplus::Color kEsSilver = C(226, 230, 236);
        const Gdiplus::Color kEsBlue   = C(0, 68, 148);
        const Gdiplus::Color kEsLeon   = C(212, 92, 118);

        FillHBandsSpanish(g, w, h, kEsRed, kEsGold);

        float cx = w * 0.33f;
        float cy = h * 0.515f;
        float sw = h * 0.175f; // Shield width
        float sh = h * 0.215f; // Shield height

        Gdiplus::SolidBrush redB(kEsRed), goldB(kEsGold), silverB(kEsSilver), blueB(kEsBlue), leonB(kEsLeon);
        Gdiplus::Pen darkGoldPen(C(156, 112, 12), h * 0.014f);

        // 1. Pillars of Hercules (left & right silver columns with gold bases/capitals & red Plus Ultra banners)
        float pOffset = sw * 0.78f;
        float pW      = sw * 0.21f;
        float pH      = sh * 0.94f;
        float pTop    = cy - sh * 0.44f;

        for (int side : { -1, 1 }) {
            float px = cx + side * pOffset - pW * 0.5f;
            g.FillRectangle(&silverB, px, pTop, pW, pH);
            g.DrawRectangle(&darkGoldPen, px, pTop, pW, pH);
            g.FillRectangle(&goldB, px - pW * 0.22f, pTop - sh * 0.06f, pW * 1.44f, sh * 0.08f);
            g.FillRectangle(&goldB, px - pW * 0.22f, pTop + pH - sh * 0.02f, pW * 1.44f, sh * 0.08f);
            g.FillRectangle(&redB, px - pW * 0.32f, cy - sh * 0.07f, pW * 1.64f, sh * 0.14f);
        }

        // 2. Royal Crown above the shield
        float crW = sw * 0.96f;
        float crH = sh * 0.30f;
        float crY = pTop - crH - sh * 0.04f;
        g.FillPie(&redB, cx - crW * 0.46f, crY, crW * 0.92f, crH * 1.35f, 180.0f, 180.0f);
        g.DrawArc(&darkGoldPen, cx - crW * 0.46f, crY, crW * 0.92f, crH * 1.35f, 180.0f, 180.0f);
        g.FillRectangle(&goldB, cx - crW * 0.48f, crY + crH * 0.58f, crW * 0.96f, crH * 0.34f);
        g.DrawRectangle(&darkGoldPen, cx - crW * 0.48f, crY + crH * 0.58f, crW * 0.96f, crH * 0.34f);
        float finR = h * 0.018f;
        g.FillEllipse(&goldB, cx - finR, crY - finR * 1.6f, finR * 2.0f, finR * 2.0f);

        // 3. Rounded-bottom Escutcheon Shield
        float sx = cx - sw * 0.5f;
        float sy = pTop;
        float archH = sh * 0.46f;
        Gdiplus::GraphicsPath shieldPath;
        shieldPath.AddLine(sx, sy, sx + sw, sy);
        shieldPath.AddLine(sx + sw, sy, sx + sw, sy + sh - archH);
        shieldPath.AddArc(sx, sy + sh - archH * 2.0f, sw, archH * 2.0f, 0.0f, 180.0f);
        shieldPath.CloseFigure();

        Gdiplus::GraphicsState esState = g.Save();
        g.SetClip(&shieldPath, Gdiplus::CombineModeIntersect);

        float midX = cx;
        float midY = sy + sh * 0.48f;
        g.FillRectangle(&redB,   sx - 1.0f, sy - 1.0f, sw * 0.5f + 1.0f, sh * 0.48f + 1.0f);
        g.FillRectangle(&leonB,  midX,      sy - 1.0f, sw * 0.5f + 1.0f, sh * 0.48f + 1.0f);
        g.FillRectangle(&goldB,  sx - 1.0f, midY,      sw * 0.5f + 1.0f, sh * 0.55f);
        g.FillRectangle(&redB,   sx + sw * 0.12f, midY, sw * 0.09f, sh * 0.55f);
        g.FillRectangle(&redB,   sx + sw * 0.30f, midY, sw * 0.09f, sh * 0.55f);
        g.FillRectangle(&redB,   midX,      midY,      sw * 0.5f + 1.0f, sh * 0.55f);

        float bRx = sw * 0.16f;
        float bRy = sh * 0.18f;
        g.FillEllipse(&blueB, midX - bRx, midY - bRy, bRx * 2.0f, bRy * 2.0f);
        g.DrawEllipse(&darkGoldPen, midX - bRx, midY - bRy, bRx * 2.0f, bRy * 2.0f);

        g.Restore(esState);
        g.DrawPath(&darkGoldPen, &shieldPath);
    } else if (iso == L"PT") {
        // Authentic 2:3 vertical green/red split + golden armillary sphere + white/red/blue Portuguese shield
        const Gdiplus::Color kPtGreen = C(0, 102, 0);
        const Gdiplus::Color kPtRed   = C(218, 41, 28);
        const Gdiplus::Color kPtGold  = C(255, 204, 0);
        const Gdiplus::Color kPtBlue  = C(0, 51, 153);

        float split = w * 0.40f;
        Gdiplus::SolidBrush gb(kPtGreen), rb(kPtRed), goldB(kPtGold), whiteB(kWhite), blueB(kPtBlue);
        g.FillRectangle(&gb, -1.0f, -1.0f, split + 1.5f, h + 2.0f);
        g.FillRectangle(&rb, split - 0.5f, -1.0f, (w - split) + 1.5f, h + 2.0f);

        float cx = split;
        float cy = h * 0.50f;
        float rSphere = h * 0.235f;

        g.FillEllipse(&goldB, cx - rSphere, cy - rSphere, rSphere * 2.0f, rSphere * 2.0f);
        Gdiplus::Pen sphereRing(C(196, 138, 0), h * 0.025f);
        g.DrawEllipse(&sphereRing, cx - rSphere * 0.86f, cy - rSphere * 0.86f, rSphere * 1.72f, rSphere * 1.72f);
        g.DrawLine(&sphereRing, cx - rSphere * 0.86f, cy, cx + rSphere * 0.86f, cy);
        g.DrawLine(&sphereRing, cx, cy - rSphere * 0.86f, cx, cy + rSphere * 0.86f);

        float shW = rSphere * 1.15f;
        float shH = rSphere * 1.32f;
        Gdiplus::GraphicsPath shieldPath;
        float sx = cx - shW * 0.5f;
        float sy = cy - shH * 0.5f;
        float archH = shH * 0.42f;
        shieldPath.AddLine(sx, sy, sx + shW, sy);
        shieldPath.AddLine(sx + shW, sy, sx + shW, sy + shH - archH);
        shieldPath.AddArc(sx, sy + shH - archH * 2.0f, shW, archH * 2.0f, 0.0f, 180.0f);
        shieldPath.CloseFigure();

        g.FillPath(&rb, &shieldPath);

        Gdiplus::GraphicsPath innerShield;
        float pad = rSphere * 0.16f;
        float isx = sx + pad;
        float isy = sy + pad;
        float isw = shW - pad * 2.0f;
        float ish = shH - pad * 2.0f;
        float iarch = ish * 0.42f;
        innerShield.AddLine(isx, isy, isx + isw, isy);
        innerShield.AddLine(isx + isw, isy, isx + isw, isy + ish - iarch);
        innerShield.AddArc(isx, isy + ish - iarch * 2.0f, isw, iarch * 2.0f, 0.0f, 180.0f);
        innerShield.CloseFigure();
        g.FillPath(&whiteB, &innerShield);

        float dotR = rSphere * 0.085f;
        float off  = rSphere * 0.21f;
        g.FillEllipse(&blueB, cx - dotR,       cy - dotR,       dotR * 2.0f, dotR * 2.0f);
        g.FillEllipse(&blueB, cx - dotR,       cy - off - dotR, dotR * 2.0f, dotR * 2.0f);
        g.FillEllipse(&blueB, cx - dotR,       cy + off - dotR, dotR * 2.0f, dotR * 2.0f);
        g.FillEllipse(&blueB, cx - off - dotR, cy - dotR,       dotR * 2.0f, dotR * 2.0f);
        g.FillEllipse(&blueB, cx + off - dotR, cy - dotR,       dotR * 2.0f, dotR * 2.0f);
    } else if (iso == L"BR") {
        FillSolid(g, w, h, C(0, 156, 59));
        Gdiplus::SolidBrush yb(C(255, 223, 0)), bb(C(0, 39, 118));
        Gdiplus::PointF diamond[4] = {
            { w * 0.50f, h * 0.11f },
            { w * 0.89f, h * 0.50f },
            { w * 0.50f, h * 0.89f },
            { w * 0.11f, h * 0.50f }
        };
        g.FillPolygon(&yb, diamond, 4);

        float cx = w * 0.50f;
        float cy = h * 0.50f;
        float r = h * 0.23f;
        g.FillEllipse(&bb, cx - r, cy - r, r * 2.0f, r * 2.0f);

        Gdiplus::GraphicsState state = g.Save();
        Gdiplus::GraphicsPath globeClip;
        globeClip.AddEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
        g.SetClip(&globeClip, Gdiplus::CombineModeIntersect);
        Gdiplus::Pen bandPen(kWhite, h * 0.055f);
        g.DrawArc(&bandPen, cx - r * 1.45f, cy - r * 0.35f, r * 2.90f, r * 1.85f, 200.0f, 140.0f);
        g.Restore(state);
    } else if (iso == L"RU") {
        FillHBands3(g, w, h, kWhite, C(0, 57, 166), C(213, 43, 30));
    } else if (iso == L"UA") {
        FillHBands2(g, w, h, C(0, 87, 183), C(255, 215, 0));
    } else if (iso == L"AZ") {
        const Gdiplus::Color kAzRed = C(239, 51, 64);
        FillHBands3(g, w, h, kSky, kAzRed, C(80, 158, 47));
        DrawCrescentAndStar(
            g, kWhite, kAzRed,
            w * 0.465f, h * 0.50f, h * 0.135f,
            h * 0.032f, h * 0.108f,
            w * 0.585f, h * 0.075f, 8, 0.0f);
    } else if (iso == L"JP") {
        FillSolid(g, w, h, kWhite);
        Gdiplus::SolidBrush rb(C(188, 0, 45));
        float r = h * 0.30f;
        g.FillEllipse(&rb, w * 0.50f - r, h * 0.50f - r, r * 2.0f, r * 2.0f);
    } else if (iso == L"KR") {
        const Gdiplus::Color kKrBg     = darkMode ? kDarkGrey       : kWhite;
        const Gdiplus::Color kKrBar    = darkMode ? kWhite          : kBlack;
        const Gdiplus::Color kKrUpper  = darkMode ? C(84, 90, 100)  : C(205, 46, 58);  // Red replacement
        const Gdiplus::Color kKrLower  = darkMode ? C(176, 182, 192): C(0, 71, 160);   // Blue replacement

        FillSolid(g, w, h, kKrBg);
        float cx = w * 0.50f;
        float cy = h * 0.50f;
        float r  = h * 0.262f;

        Gdiplus::GraphicsState state = g.Save();
        g.TranslateTransform(cx, cy);
        g.RotateTransform(33.7f);

        Gdiplus::SolidBrush upperB(kKrUpper), lowerB(kKrLower), barB(kKrBar);
        g.FillPie(&upperB, -r, -r, r * 2.0f, r * 2.0f, 180.0f, 180.0f);
        g.FillPie(&lowerB, -r, -r, r * 2.0f, r * 2.0f,   0.0f, 180.0f);
        g.FillEllipse(&upperB, -r,    -r * 0.5f, r, r);
        g.FillEllipse(&lowerB,  0.0f, -r * 0.5f, r, r);

        float dist  = r * 1.38f;
        float barW  = r * 1.02f;
        float barT  = h * 0.042f;
        float barSp = h * 0.066f;

        auto DrawTrigram = [&](float angleDeg, int brokenMask) {
            Gdiplus::GraphicsState ts = g.Save();
            g.RotateTransform(angleDeg);
            for (int b = 0; b < 3; ++b) {
                float by = -dist - b * barSp - barT;
                if (brokenMask & (1 << b)) {
                    float gapW = barT * 1.05f;
                    float segW = (barW - gapW) * 0.5f;
                    g.FillRectangle(&barB, -barW * 0.5f, by, segW, barT);
                    g.FillRectangle(&barB,  barW * 0.5f - segW, by, segW, barT);
                } else {
                    g.FillRectangle(&barB, -barW * 0.5f, by, barW, barT);
                }
            }
            g.Restore(ts);
        };

        DrawTrigram(-90.0f, 0b000); // Top-Left:     Geon (3 solid)
        DrawTrigram(  0.0f, 0b101); // Top-Right:    Gam  (broken, solid, broken)
        DrawTrigram(180.0f, 0b010); // Bottom-Left:  Ri   (solid, broken, solid)
        DrawTrigram( 90.0f, 0b111); // Bottom-Right: Gon  (3 broken)

        g.Restore(state);
    } else if (iso == L"CN") {
        FillSolid(g, w, h, C(238, 28, 37));
        const Gdiplus::Color kCnYellow = C(255, 255, 0);
        DrawStar(g, kCnYellow, w * 0.22f, h * 0.32f, h * 0.18f, h * 0.072f);
        DrawStar(g, kCnYellow, w * 0.40f, h * 0.16f, h * 0.062f, h * 0.025f, -65.0f);
        DrawStar(g, kCnYellow, w * 0.47f, h * 0.29f, h * 0.062f, h * 0.025f, -40.0f);
        DrawStar(g, kCnYellow, w * 0.47f, h * 0.45f, h * 0.062f, h * 0.025f, -90.0f);
        DrawStar(g, kCnYellow, w * 0.40f, h * 0.58f, h * 0.062f, h * 0.025f, -65.0f);
    } else if (iso == L"NL") {
        FillHBands3(g, w, h, C(174, 28, 40), kWhite, C(33, 70, 139));
    } else if (iso == L"PL") {
        FillHBands2(g, w, h, kWhite, C(220, 20, 60));
    } else if (iso == L"SE") {
        FillSolid(g, w, h, C(0, 106, 167));
        float cx = w * 0.36f;
        float cy = h * 0.50f;
        float iw = h * 0.20f;
        Gdiplus::SolidBrush yb(kYellow);
        g.FillRectangle(&yb, cx - iw * 0.5f, -1.0f, iw, h + 2.0f);
        g.FillRectangle(&yb, -1.0f, cy - iw * 0.5f, w + 2.0f, iw);
    } else if (iso == L"GR") {
        // Authentic 9 horizontal stripes (5 blue/grey, 4 white) + 5-stripe square canton with white cross
        const Gdiplus::Color kGrDark = darkMode ? kDarkGrey : C(13, 94, 175);
        Gdiplus::SolidBrush darkB(kGrDark), whiteB(kWhite);
        float stripeH = h / 9.0f;
        for (int i = 0; i < 9; ++i) {
            float y0 = i * stripeH;
            g.FillRectangle((i % 2 == 0) ? &darkB : &whiteB, -1.0f, y0 - 0.3f, w + 2.0f, stripeH + 0.6f);
        }
        float cSize = stripeH * 5.0f;
        g.FillRectangle(&darkB, -1.0f, -1.0f, cSize + 1.0f, cSize + 1.0f);
        float armW = stripeH;
        g.FillRectangle(&whiteB, (cSize - armW) * 0.5f, -1.0f, armW, cSize + 1.0f);
        g.FillRectangle(&whiteB, -1.0f, (cSize - armW) * 0.5f, cSize + 1.0f, armW);
    } else {
        // Clean ISO 2-letter fallback for any other locale
        FillSolid(g, w, h, C(52, 84, 132));
        std::wstring label(iso.substr(0, 2));
        Gdiplus::FontFamily ff(L"Segoe UI");
        Gdiplus::Font font(&ff, h * 0.58f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush txtB(kWhite);
        Gdiplus::StringFormat fmt;
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::RectF rect(0.0f, 0.0f, w, h);
        g.DrawString(label.c_str(), -1, &font, rect, &fmt, &txtB);
    }
}

std::vector<BYTE> RenderFlagBGRA(KeyboardInfo kb, int width, int height) {
    std::vector<BYTE> buffer(width * height * 4, 0);
    if (width <= 0 || height <= 0) return buffer;

    ModSettings settings = GetSettingsSnapshot();

    std::wstring isoCountry = ResolveDisplayCountryCode(kb);
    if (settings.useUkFlagForUsLayout && isoCountry == L"US") {
        isoCountry = L"GB";
    }
    bool applyDarkMode = settings.darkMode && IsDarkModeSupportedCountry(isoCountry);

    FlagCacheKey cacheKey{ isoCountry, width, height, settings.roundedCorners, applyDarkMode };
    {
        std::shared_lock lock(g_stateLock);
        auto it = g_flagPixelCache.find(cacheKey);
        if (it != g_flagPixelCache.end()) {
            return it->second;
        }
    }

    // 1. Render at 4x supersampled resolution for razor-sharp vector details
    constexpr int kSS = 4;
    const int ssW = width * kSS;
    const int ssH = height * kSS;
    const float w = static_cast<float>(ssW);
    const float h = static_cast<float>(ssH);

    Gdiplus::Bitmap ssBmp(ssW, ssH, PixelFormat32bppPARGB);
    {
        Gdiplus::Graphics g(&ssBmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

        const float cornerR = (std::max)(4.0f, h * 0.095f);

        Gdiplus::GraphicsPath clipPath;
        if (settings.roundedCorners) {
            float d = cornerR * 2.0f;
            clipPath.AddArc(0.0f, 0.0f, d, d, 180.0f, 90.0f);
            clipPath.AddArc(w - d, 0.0f, d, d, 270.0f, 90.0f);
            clipPath.AddArc(w - d, h - d, d, d, 0.0f, 90.0f);
            clipPath.AddArc(0.0f, h - d, d, d, 90.0f, 90.0f);
            clipPath.CloseFigure();
            g.SetClip(&clipPath);
        }

        DrawFlagByIsoCountry(g, isoCountry, w, h, applyDarkMode);

        // Crisp neutral inner hairline border (subtle in Dark Mode so solid flags stay full-sized)
        g.ResetClip();
        float penW = static_cast<float>(kSS);
        float halfPen = penW * 0.5f;
        Gdiplus::Color borderCol = applyDarkMode
            ? Gdiplus::Color(46, 96, 102, 114)
            : Gdiplus::Color(68, 40, 44, 52);
        Gdiplus::Pen borderPen(borderCol, penW);
        if (settings.roundedCorners) {
            Gdiplus::GraphicsPath borderPath;
            float d = cornerR * 2.0f;
            borderPath.AddArc(halfPen, halfPen, d - penW, d - penW, 180.0f, 90.0f);
            borderPath.AddArc(w - d + halfPen, halfPen, d - penW, d - penW, 270.0f, 90.0f);
            borderPath.AddArc(w - d + halfPen, h - d + halfPen, d - penW, d - penW, 0.0f, 90.0f);
            borderPath.AddArc(halfPen, h - d + halfPen, d - penW, d - penW, 90.0f, 90.0f);
            borderPath.CloseFigure();
            g.DrawPath(&borderPen, &borderPath);
        } else {
            g.DrawRectangle(&borderPen, halfPen, halfPen, w - penW, h - penW);
        }
    }

    // 2. High-quality bicubic downsample to target taskbar pixel size
    Gdiplus::Bitmap outBmp(width, height, width * 4, PixelFormat32bppPARGB, buffer.data());
    {
        Gdiplus::Graphics gOut(&outBmp);
        gOut.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        gOut.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        gOut.DrawImage(&ssBmp, Gdiplus::Rect(0, 0, width, height), 0, 0, ssW, ssH, Gdiplus::UnitPixel);
    }

    {
        std::unique_lock lock(g_stateLock);
        if (g_flagPixelCache.size() >= 32) {
            g_flagPixelCache.clear();
        }
        g_flagPixelCache[std::move(cacheKey)] = buffer;
    }

    return buffer;
}

// ============================================================================
// 3. WINDOWS 10 SUPPORT: INDICATOR-SCOPED TEXT SUPPRESSION & ALPHA COMPOSITING
// ============================================================================

bool IsTrayInputIndicatorWindow(HWND hwnd) {
    if (!hwnd) return false;
    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0 &&
        wcscmp(cls, L"TrayInputIndicatorWClass") == 0) {
        return true;
    }
    HWND parent = GetParent(hwnd);
    if (parent && GetClassNameW(parent, cls, ARRAYSIZE(cls)) > 0 &&
        wcscmp(cls, L"TrayInputIndicatorWClass") == 0) {
        return true;
    }
    return false;
}

bool ShouldSuppressTaskbarIndicatorText(HDC hdc) {
    if (!hdc || g_inOurPaint || g_modUnloading.load(std::memory_order_acquire)) {
        return false;
    }

    // 1. Active WM_PAINT / WM_PRINTCLIENT of TrayInputIndicatorWClass on this thread
    if (g_inIndicatorNativePaint > 0) {
        return true;
    }

    // 2. Active BeginBufferedPaint memory DC created for TrayInputIndicatorWClass
    if (!g_activeIndicatorPaintDCs.Empty() && g_activeIndicatorPaintDCs.Contains(hdc)) {
        return true;
    }

    // 3. Direct window DC match for TrayInputIndicatorWClass
    HWND hwnd = WindowFromDC(hdc);
    if (hwnd && IsTrayInputIndicatorWindow(hwnd)) {
        return true;
    }

    return false;
}

int GetWindowEffectiveDpi(HWND hwnd, HDC fallbackDC) {
    if (hwnd) {
        UINT dpi = GetDpiForWindow(hwnd);
        if (dpi > 0) {
            return static_cast<int>(dpi);
        }
    }

    if (fallbackDC) {
        int dpi = GetDeviceCaps(fallbackDC, LOGPIXELSY);
        if (dpi > 0) {
            return dpi;
        }
    }
    return 96;
}

// --- BufferedPaint & GDI Blit Hooks ---

using BeginBufferedPaint_t = HPAINTBUFFER(WINAPI*)(HDC, const RECT*, BP_BUFFERFORMAT, BP_PAINTPARAMS*, HDC*);
BeginBufferedPaint_t BeginBufferedPaint_Orig = nullptr;

using EndBufferedPaint_t = HRESULT(WINAPI*)(HPAINTBUFFER, BOOL);
EndBufferedPaint_t EndBufferedPaint_Orig = nullptr;

using BitBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, DWORD);
BitBlt_t BitBlt_Orig = nullptr;

using StretchBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);
StretchBlt_t StretchBlt_Orig = nullptr;

using GdiAlphaBlend_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
GdiAlphaBlend_t GdiAlphaBlend_Orig = nullptr;

// Alpha-composites the 32-bit PARGB flag directly onto Windows' native buffered DC,
// preserving the taskbar's DWM acrylic/transparency alpha channel as well as native hover/click highlights.
void CompositeFlagOntoDC(HWND hwnd, HDC targetDC) {
    if (!hwnd || !targetDC) return;

    RECT rc;
    if (!GetClientRect(hwnd, &rc)) return;
    int winW = rc.right - rc.left;
    int winH = rc.bottom - rc.top;
    if (winW <= 0 || winH <= 0) return;

    ModSettings settings = GetSettingsSnapshot();
    int dpi = GetWindowEffectiveDpi(hwnd, targetDC);

    int flagW = MulDiv(settings.flagWidth, dpi, 96);
    int flagH = MulDiv(settings.flagHeight, dpi, 96);
    if (flagW > winW - 4) {
        int origW = (std::max)(1, flagW);
        flagW = (std::max)(12, winW - 4);
        flagH = MulDiv(flagH, flagW, origW);
    }

    int posX = (winW - flagW) / 2;
    int posY = (winH - flagH) / 2;

    KeyboardInfo kb = GetActiveKeyboardInfo();
    std::vector<BYTE> pixels = RenderFlagBGRA(kb, flagW, flagH);
    if (pixels.empty()) return;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = flagW;
    bmi.bmiHeader.biHeight = -flagH; // Top-down DIB matching PixelFormat32bppPARGB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* dibBits = nullptr;
    HDC srcDC = CreateCompatibleDC(targetDC);
    HBITMAP srcBmp = CreateDIBSection(targetDC, &bmi, DIB_RGB_COLORS, &dibBits, nullptr, 0);
    if (srcDC && srcBmp && dibBits) {
        std::memcpy(dibBits, pixels.data(), pixels.size());
        HGDIOBJ oldBmp = SelectObject(srcDC, srcBmp);

        BLENDFUNCTION bf = {};
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = AC_SRC_ALPHA;

        if (GdiAlphaBlend_Orig) {
            GdiAlphaBlend_Orig(targetDC, posX, posY, flagW, flagH, srcDC, 0, 0, flagW, flagH, bf);
        } else {
            GdiAlphaBlend(targetDC, posX, posY, flagW, flagH, srcDC, 0, 0, flagW, flagH, bf);
        }

        SelectObject(srcDC, oldBmp);
    }
    if (srcBmp) DeleteObject(srcBmp);
    if (srcDC) DeleteDC(srcDC);
}

HPAINTBUFFER WINAPI BeginBufferedPaint_Hook(
    HDC hdcTarget, const RECT* prcTarget, BP_BUFFERFORMAT dwFormat,
    BP_PAINTPARAMS* pPaintParams, HDC* phdc) {
    HPAINTBUFFER hpb = BeginBufferedPaint_Orig(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc);
    if (hpb && phdc && *phdc && !g_modUnloading.load(std::memory_order_acquire)) {
        bool isIndicator = (g_inIndicatorNativePaint > 0);
        if (!isIndicator && hdcTarget) {
            HWND hwnd = WindowFromDC(hdcTarget);
            isIndicator = (hwnd && IsTrayInputIndicatorWindow(hwnd));
        }
        if (isIndicator) {
            g_activeIndicatorPaintDCs.Insert(*phdc);
        }
    }
    return hpb;
}

HRESULT WINAPI EndBufferedPaint_Hook(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget) {
    if (hBufferedPaint && !g_inOurPaint && !g_modUnloading.load(std::memory_order_acquire)) {
        HDC bufDC = GetBufferedPaintDC(hBufferedPaint);
        if (bufDC) {
            g_activeIndicatorPaintDCs.Erase(bufDC);
        }

        HDC targetDC = GetBufferedPaintTargetDC(hBufferedPaint);
        HWND hwnd = targetDC ? WindowFromDC(targetDC) : nullptr;
        if (hwnd && IsTrayInputIndicatorWindow(hwnd)) {
            g_bufferedPaintHandled = true;
            // Keep g_inOurPaint alive across EndBufferedPaint_Orig so its internal
            // BitBlt/GdiAlphaBlend to targetDC does not composite the flag a second time.
            ReentrancyFlagGuard guard(g_inOurPaint);
            if (fUpdateTarget && bufDC) {
                CompositeFlagOntoDC(hwnd, bufDC);
            }
            return EndBufferedPaint_Orig(hBufferedPaint, fUpdateTarget);
        }
    }
    return EndBufferedPaint_Orig(hBufferedPaint, fUpdateTarget);
}

BOOL WINAPI BitBlt_Hook(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) {
    if (!g_inOurPaint && !g_modUnloading.load(std::memory_order_acquire) && hdc) {
        HWND hwnd = WindowFromDC(hdc);
        if (hwnd && IsTrayInputIndicatorWindow(hwnd)) {
            ReentrancyFlagGuard guard(g_inOurPaint);
            g_bufferedPaintHandled = true;
            BOOL res = BitBlt_Orig(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
            CompositeFlagOntoDC(hwnd, hdc);
            return res;
        }
    }
    return BitBlt_Orig(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
}

BOOL WINAPI StretchBlt_Hook(HDC hdcDest, int xDest, int yDest, int wDest, int hDest,
                            HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop) {
    if (!g_inOurPaint && !g_modUnloading.load(std::memory_order_acquire) && hdcDest) {
        HWND hwnd = WindowFromDC(hdcDest);
        if (hwnd && IsTrayInputIndicatorWindow(hwnd)) {
            ReentrancyFlagGuard guard(g_inOurPaint);
            g_bufferedPaintHandled = true;
            BOOL res = StretchBlt_Orig(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, rop);
            CompositeFlagOntoDC(hwnd, hdcDest);
            return res;
        }
    }
    return StretchBlt_Orig(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, rop);
}

BOOL WINAPI GdiAlphaBlend_Hook(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
                               HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn) {
    if (!g_inOurPaint && !g_modUnloading.load(std::memory_order_acquire) && hdcDest) {
        HWND hwnd = WindowFromDC(hdcDest);
        if (hwnd && IsTrayInputIndicatorWindow(hwnd)) {
            ReentrancyFlagGuard guard(g_inOurPaint);
            g_bufferedPaintHandled = true;
            BOOL res = GdiAlphaBlend_Orig(hdcDest, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
            CompositeFlagOntoDC(hwnd, hdcDest);
            return res;
        }
    }
    return GdiAlphaBlend_Orig(hdcDest, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
}

// --- Direct2D & DirectWrite Text Suppression Hooks (Windows 10 Indicator Paint Only) ---

inline bool IsIndicatorD2DPaintActive() {
    return !g_inOurPaint &&
           !g_modUnloading.load(std::memory_order_acquire) &&
           (g_inIndicatorNativePaint > 0 || !g_activeIndicatorPaintDCs.Empty());
}

using D2D_DrawText_t = void(STDMETHODCALLTYPE*)(
    ID2D1RenderTarget* This,
    const WCHAR* string,
    UINT32 stringLength,
    IDWriteTextFormat* textFormat,
    const D2D1_RECT_F* layoutRect,
    ID2D1Brush* defaultFillBrush,
    D2D1_DRAW_TEXT_OPTIONS options,
    DWRITE_MEASURING_MODE measuringMode);
D2D_DrawText_t D2D_DrawText_Orig = nullptr;

void STDMETHODCALLTYPE D2D_DrawText_Hook(
    ID2D1RenderTarget* This,
    const WCHAR* string,
    UINT32 stringLength,
    IDWriteTextFormat* textFormat,
    const D2D1_RECT_F* layoutRect,
    ID2D1Brush* defaultFillBrush,
    D2D1_DRAW_TEXT_OPTIONS options,
    DWRITE_MEASURING_MODE measuringMode) {
    if (IsIndicatorD2DPaintActive()) {
        return;
    }
    D2D_DrawText_Orig(This, string, stringLength, textFormat, layoutRect, defaultFillBrush, options, measuringMode);
}

using D2D_DrawTextLayout_t = void(STDMETHODCALLTYPE*)(
    ID2D1RenderTarget* This,
    D2D1_POINT_2F origin,
    IDWriteTextLayout* textLayout,
    ID2D1Brush* defaultFillBrush,
    D2D1_DRAW_TEXT_OPTIONS options);
D2D_DrawTextLayout_t D2D_DrawTextLayout_Orig = nullptr;

void STDMETHODCALLTYPE D2D_DrawTextLayout_Hook(
    ID2D1RenderTarget* This,
    D2D1_POINT_2F origin,
    IDWriteTextLayout* textLayout,
    ID2D1Brush* defaultFillBrush,
    D2D1_DRAW_TEXT_OPTIONS options) {
    if (IsIndicatorD2DPaintActive()) {
        return;
    }
    D2D_DrawTextLayout_Orig(This, origin, textLayout, defaultFillBrush, options);
}

using D2D_DrawGlyphRun_t = void(STDMETHODCALLTYPE*)(
    ID2D1RenderTarget* This,
    D2D1_POINT_2F baselineOrigin,
    const DWRITE_GLYPH_RUN* glyphRun,
    ID2D1Brush* foregroundBrush,
    DWRITE_MEASURING_MODE measuringMode);
D2D_DrawGlyphRun_t D2D_DrawGlyphRun_Orig = nullptr;

void STDMETHODCALLTYPE D2D_DrawGlyphRun_Hook(
    ID2D1RenderTarget* This,
    D2D1_POINT_2F baselineOrigin,
    const DWRITE_GLYPH_RUN* glyphRun,
    ID2D1Brush* foregroundBrush,
    DWRITE_MEASURING_MODE measuringMode) {
    if (IsIndicatorD2DPaintActive()) {
        return;
    }
    D2D_DrawGlyphRun_Orig(This, baselineOrigin, glyphRun, foregroundBrush, measuringMode);
}

using DWrite_BitmapDrawGlyphRun_t = HRESULT(STDMETHODCALLTYPE*)(
    IDWriteBitmapRenderTarget* This,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_MEASURING_MODE measuringMode,
    DWRITE_GLYPH_RUN const* glyphRun,
    IDWriteRenderingParams* renderingParams,
    COLORREF textColor,
    RECT* blackBoxRect);
DWrite_BitmapDrawGlyphRun_t DWrite_BitmapDrawGlyphRun_Orig = nullptr;

HRESULT STDMETHODCALLTYPE DWrite_BitmapDrawGlyphRun_Hook(
    IDWriteBitmapRenderTarget* This,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_MEASURING_MODE measuringMode,
    DWRITE_GLYPH_RUN const* glyphRun,
    IDWriteRenderingParams* renderingParams,
    COLORREF textColor,
    RECT* blackBoxRect) {
    if (IsIndicatorD2DPaintActive()) {
        if (blackBoxRect) {
            SetRectEmpty(blackBoxRect);
        }
        return S_OK;
    }
    return DWrite_BitmapDrawGlyphRun_Orig(
        This, baselineOriginX, baselineOriginY, measuringMode, glyphRun, renderingParams, textColor, blackBoxRect);
}

void HookD2DRenderTargetVtable(ID2D1RenderTarget* rt) {
    if (!rt) return;
    void** vtable = *reinterpret_cast<void***>(rt);
    if (!vtable || g_hookedD2DVtables.count(vtable) > 0) {
        return;
    }
    g_hookedD2DVtables.insert(vtable);

    if (!D2D_DrawText_Orig) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<D2D_DrawText_t>(vtable[27]),
            D2D_DrawText_Hook,
            &D2D_DrawText_Orig);
    }
    if (!D2D_DrawTextLayout_Orig) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<D2D_DrawTextLayout_t>(vtable[28]),
            D2D_DrawTextLayout_Hook,
            &D2D_DrawTextLayout_Orig);
    }
    if (!D2D_DrawGlyphRun_Orig) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<D2D_DrawGlyphRun_t>(vtable[29]),
            D2D_DrawGlyphRun_Hook,
            &D2D_DrawGlyphRun_Orig);
    }
}

void InstallDirect2DAndDWriteHooks() {
    HMODULE dwriteMod = GetOrLoadSystemModule(L"dwrite.dll");
    if (dwriteMod) {
        using DWriteCreateFactory_t = HRESULT(WINAPI*)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
        auto pDWriteCreateFactory = reinterpret_cast<DWriteCreateFactory_t>(GetProcAddress(dwriteMod, "DWriteCreateFactory"));
        if (pDWriteCreateFactory) {
            IDWriteFactory* dwFactory = nullptr;
            if (SUCCEEDED(pDWriteCreateFactory(
                    DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwFactory))) && dwFactory) {
                IDWriteGdiInterop* gdiInterop = nullptr;
                if (SUCCEEDED(dwFactory->GetGdiInterop(&gdiInterop)) && gdiInterop) {
                    IDWriteBitmapRenderTarget* bmpTarget = nullptr;
                    if (SUCCEEDED(gdiInterop->CreateBitmapRenderTarget(nullptr, 1, 1, &bmpTarget)) && bmpTarget) {
                        void** vtable = *reinterpret_cast<void***>(bmpTarget);
                        if (vtable && !DWrite_BitmapDrawGlyphRun_Orig) {
                            WindhawkUtils::SetFunctionHook(
                                reinterpret_cast<DWrite_BitmapDrawGlyphRun_t>(vtable[3]),
                                DWrite_BitmapDrawGlyphRun_Hook,
                                &DWrite_BitmapDrawGlyphRun_Orig);
                        }
                        bmpTarget->Release();
                    }
                    gdiInterop->Release();
                }
                dwFactory->Release();
            }
        }
    }

    HMODULE d2dMod = GetOrLoadSystemModule(L"d2d1.dll");
    if (!d2dMod) return;

    using D2D1CreateFactory_t = HRESULT(WINAPI*)(D2D1_FACTORY_TYPE, REFIID, const D2D1_FACTORY_OPTIONS*, void**);
    auto pD2D1CreateFactory = reinterpret_cast<D2D1CreateFactory_t>(GetProcAddress(d2dMod, "D2D1CreateFactory"));
    if (!pD2D1CreateFactory) return;

    ID2D1Factory* factory = nullptr;
    if (SUCCEEDED(pD2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory),
            nullptr,
            reinterpret_cast<void**>(&factory))) && factory) {
        D2D1_RENDER_TARGET_PROPERTIES dcProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        ID2D1DCRenderTarget* dcTarget = nullptr;
        if (SUCCEEDED(factory->CreateDCRenderTarget(&dcProps, &dcTarget)) && dcTarget) {
            HookD2DRenderTargetVtable(dcTarget);
            dcTarget->Release();
        }
        factory->Release();
    }
}

// --- UxTheme & GDI Text Suppression Hooks ---

using DrawThemeTextEx_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const DTTOPTS*);
DrawThemeTextEx_t DrawThemeTextEx_Orig = nullptr;

HRESULT WINAPI DrawThemeTextEx_Hook(
    HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
    LPCWSTR pszText, int cchText, DWORD dwTextFlags,
    LPRECT pRect, const DTTOPTS* pOptions) {
    if ((dwTextFlags & DT_CALCRECT) == 0 && ShouldSuppressTaskbarIndicatorText(hdc)) {
        return S_OK;
    }
    return DrawThemeTextEx_Orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions);
}

using DrawThemeText_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, LPCRECT);
DrawThemeText_t DrawThemeText_Orig = nullptr;

HRESULT WINAPI DrawThemeText_Hook(
    HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
    LPCWSTR pszText, int cchText, DWORD dwTextFlags,
    DWORD dwTextFlags2, LPCRECT pRect) {
    if ((dwTextFlags & DT_CALCRECT) == 0 && ShouldSuppressTaskbarIndicatorText(hdc)) {
        return S_OK;
    }
    return DrawThemeText_Orig(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect);
}

using DrawTextW_t = int(WINAPI*)(HDC, LPCWSTR, int, LPRECT, UINT);
DrawTextW_t DrawTextW_Orig = nullptr;

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    if ((format & DT_CALCRECT) == 0 && ShouldSuppressTaskbarIndicatorText(hdc)) {
        return 16;
    }
    return DrawTextW_Orig(hdc, lpchText, cchText, lprc, format);
}

using DrawTextExW_t = int(WINAPI*)(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
DrawTextExW_t DrawTextExW_Orig = nullptr;

int WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp) {
    if ((format & DT_CALCRECT) == 0 && ShouldSuppressTaskbarIndicatorText(hdc)) {
        return 16;
    }
    return DrawTextExW_Orig(hdc, lpchText, cchText, lprc, format, lpdtp);
}

using ExtTextOutW_t = BOOL(WINAPI*)(HDC, int, int, UINT, const RECT*, LPCWSTR, UINT, const INT*);
ExtTextOutW_t ExtTextOutW_Orig = nullptr;

BOOL WINAPI ExtTextOutW_Hook(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx) {
    if (lpString && c > 0 && ShouldSuppressTaskbarIndicatorText(hdc)) {
        return TRUE;
    }
    return ExtTextOutW_Orig(hdc, x, y, options, lprect, lpString, c, lpDx);
}

using TextOutW_t = BOOL(WINAPI*)(HDC, int, int, LPCWSTR, int);
TextOutW_t TextOutW_Orig = nullptr;

BOOL WINAPI TextOutW_Hook(HDC hdc, int x, int y, LPCWSTR lpString, int c) {
    if (lpString && c > 0 && ShouldSuppressTaskbarIndicatorText(hdc)) {
        return TRUE;
    }
    return TextOutW_Orig(hdc, x, y, lpString, c);
}

LRESULT CALLBACK Win10IndicatorSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {

    switch (uMsg) {
        case WM_INPUTLANGCHANGE:
        case WM_THEMECHANGED:
        case WM_DPICHANGED: {
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        }
        case WM_PAINT:
        case WM_PRINTCLIENT: {
            g_bufferedPaintHandled = false;
            LRESULT res = 0;
            {
                ReentrancyCounterGuard paintCounter(g_inIndicatorNativePaint);
                res = DefSubclassProc(hwnd, uMsg, wParam, lParam);
            }
            if (uMsg == WM_PAINT && !g_bufferedPaintHandled && !g_inOurPaint) {
                HDC hdc = GetDC(hwnd);
                if (hdc) {
                    ReentrancyFlagGuard guard(g_inOurPaint);
                    CompositeFlagOntoDC(hwnd, hdc);
                    ReleaseDC(hwnd, hdc);
                }
            }
            return res;
        }
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

bool IsAlreadySubclassed(HWND hwnd) {
    std::shared_lock lock(g_stateLock);
    for (HWND h : g_subclassedIndicators) {
        if (h == hwnd) return true;
    }
    return false;
}

void AttachWin10IndicatorWindow(HWND hwnd) {
    if (!hwnd || g_modUnloading.load(std::memory_order_acquire)) return;
    if (!IsTrayInputIndicatorWindow(hwnd) || IsAlreadySubclassed(hwnd)) return;

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, Win10IndicatorSubclassProc, 0)) {
        {
            std::unique_lock lock(g_stateLock);
            g_subclassedIndicators.push_back(hwnd);
        }
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

BOOL CALLBACK EnumAndAttachWin10Proc(HWND hwnd, LPARAM lParam) {
    AttachWin10IndicatorWindow(hwnd);
    return TRUE;
}

void ScanAndAttachWin10Indicators() {
    {
        std::unique_lock lock(g_stateLock);
        for (size_t i = 0; i < g_subclassedIndicators.size();) {
            if (!IsWindow(g_subclassedIndicators[i])) {
                g_subclassedIndicators.erase(g_subclassedIndicators.begin() + i);
            } else {
                ++i;
            }
        }
    }

    HWND primaryTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (primaryTray) {
        EnumChildWindows(primaryTray, EnumAndAttachWin10Proc, 0);
    }

    HWND secondaryTray = nullptr;
    while ((secondaryTray = FindWindowExW(nullptr, secondaryTray, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        EnumChildWindows(secondaryTray, EnumAndAttachWin10Proc, 0);
    }
}

void InvalidateAllWin10Indicators(bool triggerD2DRedraw = false) {
    std::vector<HWND> indicators;
    {
        std::shared_lock lock(g_stateLock);
        indicators = g_subclassedIndicators;
    }

    for (HWND hwnd : indicators) {
        if (IsWindow(hwnd)) {
            if (triggerD2DRedraw) {
                SendNotifyMessageW(hwnd, WM_THEMECHANGED, 0, 0);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
        }
    }
}

// ============================================================================
// 4. WINDOWS 11 SUPPORT: SystemTray::IconView VISUAL TREE & CLEAN LIFECYCLE
// ============================================================================

struct TrackedWin11TextBlock {
    winrt::weak_ref<wuxc::TextBlock> weakTb;
    winrt::Windows::Foundation::IInspectable originalOpacity{ nullptr };
    int64_t textCallbackToken = 0;
};

struct TrackedWin11Indicator {
    HWND trayHwnd = nullptr;
    winrt::weak_ref<wux::FrameworkElement> weakIconView;
    winrt::weak_ref<wuxc::Panel> weakPanel;
    winrt::weak_ref<wuxc::Image> weakFlagImage;
    wux::FrameworkElement::Unloaded_revoker unloadedRevoker;
    std::vector<TrackedWin11TextBlock> textBlocks;
    bool isUpdating = false;
};

struct PendingLoadedEntry {
    HWND trayHwnd = nullptr;
    winrt::weak_ref<wux::FrameworkElement> weakElement;
    wux::FrameworkElement::Loaded_revoker loadedRevoker;
};

// Accessed strictly on taskbar XAML UI thread(s).
// [[clang::no_destroy]] prevents CRT exit-time destruction on an arbitrary thread
// after XAML teardown; CleanupWin11IndicatorsOnUIThread explicitly resets them on the UI thread.
[[clang::no_destroy]] std::optional<std::vector<TrackedWin11Indicator>>
    g_win11Indicators{ std::in_place };
[[clang::no_destroy]] std::optional<std::vector<PendingLoadedEntry>>
    g_pendingLoadedEntries{ std::in_place };
std::unordered_set<void*> g_scannedXamlRoots;

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
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

    return static_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

HMODULE GetSystemTrayModuleHandle() {
    static const PCWSTR moduleNames[] = {
        L"SystemTray.dll",
        L"Taskbar.View.dll",
        L"ExplorerExtensions.dll",
    };

    for (PCWSTR moduleName : moduleNames) {
        HMODULE module = GetModuleHandleW(moduleName);
        if (!module) {
            continue;
        }

        if (_wcsicmp(moduleName, L"Taskbar.View.dll") == 0) {
            VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(module, nullptr);
            if (fixedFileInfo &&
                HIWORD(fixedFileInfo->dwFileVersionMS) >= 2604) {
                continue;
            }
        }

        return module;
    }

    return nullptr;
}

wux::FrameworkElement GetFrameworkElementFromImplThis(void* pThis) {
    if (!pThis) return nullptr;
    IUnknown* unk = reinterpret_cast<IUnknown**>(pThis)[1];
    if (!unk) return nullptr;
    wux::FrameworkElement element{ nullptr };
    if (FAILED(unk->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(element)))) {
        return nullptr;
    }
    return element;
}

HWND FindTaskbarHwndForCurrentThread() {
    DWORD tid = GetCurrentThreadId();
    HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (primary && GetWindowThreadProcessId(primary, nullptr) == tid) {
        return primary;
    }
    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        if (GetWindowThreadProcessId(secondary, nullptr) == tid) {
            return secondary;
        }
    }
    return primary;
}

wux::FrameworkElement FindDescendantByName(const wux::DependencyObject& root, std::wstring_view targetName, int maxDepth = 10) {
    if (!root || maxDepth <= 0) return nullptr;

    struct Node {
        wux::DependencyObject obj;
        int depth;
    };
    std::vector<Node> queue;
    queue.reserve(32);
    queue.push_back({ root, 0 });

    for (size_t idx = 0; idx < queue.size(); ++idx) {
        wux::DependencyObject curr = queue[idx].obj;
        int depth = queue[idx].depth;

        if (auto fe = curr.try_as<wux::FrameworkElement>()) {
            if (std::wstring_view(fe.Name()) == targetName) {
                return fe;
            }
        }

        if (depth < maxDepth) {
            int count = wuxm::VisualTreeHelper::GetChildrenCount(curr);
            for (int i = 0; i < count; ++i) {
                if (auto child = wuxm::VisualTreeHelper::GetChild(curr, i)) {
                    queue.push_back({ std::move(child), depth + 1 });
                }
            }
        }
    }
    return nullptr;
}

void CollectDescendantTextBlocks(const wux::DependencyObject& root, std::vector<wuxc::TextBlock>& out, int maxDepth = 6) {
    if (!root || maxDepth <= 0) return;

    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        wux::DependencyObject child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (!child) continue;
        if (auto tb = child.try_as<wuxc::TextBlock>()) {
            out.push_back(tb);
        } else {
            CollectDescendantTextBlocks(child, out, maxDepth - 1);
        }
    }
}

void UpdateWin11IndicatorFlag(TrackedWin11Indicator& entry) {
    if (g_modUnloading.load(std::memory_order_acquire) || entry.isUpdating) {
        return;
    }

    auto panel = entry.weakPanel.get();
    if (!panel) return;

    ReentrancyFlagGuard updateGuard(entry.isUpdating);

    // Hide native language TextBlocks via Opacity so Windows' own Visibility state is untouched
    for (auto& ttb : entry.textBlocks) {
        if (auto tb = ttb.weakTb.get()) {
            if (tb.Opacity() != 0.0) {
                tb.Opacity(0.0);
            }
        }
    }

    ModSettings settings = GetSettingsSnapshot();

    double rasterScale = 1.0;
    if (auto iconView = entry.weakIconView.get()) {
        if (auto xr = iconView.XamlRoot()) {
            double s = xr.RasterizationScale();
            if (s > 0.5) rasterScale = s;
        }
    } else if (auto xr = panel.XamlRoot()) {
        double s = xr.RasterizationScale();
        if (s > 0.5) rasterScale = s;
    }

    wuxc::Image flagImage = entry.weakFlagImage.get();
    if (!flagImage) {
        for (auto child : panel.Children()) {
            if (auto fe = child.try_as<wux::FrameworkElement>()) {
                if (fe.Name() == L"WindhawkFlagImage") {
                    flagImage = child.try_as<wuxc::Image>();
                    break;
                }
            }
        }
    }

    if (!flagImage) {
        flagImage = wuxc::Image();
        flagImage.Name(L"WindhawkFlagImage");
        flagImage.Stretch(wuxm::Stretch::Fill);
        flagImage.HorizontalAlignment(wux::HorizontalAlignment::Center);
        flagImage.VerticalAlignment(wux::VerticalAlignment::Center);
        panel.Children().Append(flagImage);
    }

    flagImage.Width(static_cast<double>(settings.flagWidth));
    flagImage.Height(static_cast<double>(settings.flagHeight));
    entry.weakFlagImage = winrt::make_weak(flagImage);

    KeyboardInfo kb = GetActiveKeyboardInfo();
    const int physW = (std::max)(1, static_cast<int>(std::lround(settings.flagWidth * rasterScale)));
    const int physH = (std::max)(1, static_cast<int>(std::lround(settings.flagHeight * rasterScale)));
    std::vector<BYTE> pixels = RenderFlagBGRA(kb, physW, physH);
    if (pixels.empty()) return;

    wuxmi::WriteableBitmap wb(physW, physH);
    auto pixelBuf = wb.PixelBuffer();
    winrt::com_ptr<::Windows::Storage::Streams::IBufferByteAccess> byteAccess;
    winrt::check_hresult(
        static_cast<::IUnknown*>(winrt::get_abi(pixelBuf))->QueryInterface(
            __uuidof(::Windows::Storage::Streams::IBufferByteAccess),
            byteAccess.put_void()));

    uint8_t* dest = nullptr;
    winrt::check_hresult(byteAccess->Buffer(&dest));
    if (dest) {
        std::memcpy(dest, pixels.data(), pixels.size());
        wb.Invalidate();
        flagImage.Source(wb);
    }
}

void RestoreAndDetachWin11Indicator(TrackedWin11Indicator& item) {
    item.isUpdating = true;
    item.unloadedRevoker.revoke();

    for (auto& ttb : item.textBlocks) {
        if (auto tb = ttb.weakTb.get()) {
            if (ttb.textCallbackToken != 0) {
                tb.UnregisterPropertyChangedCallback(wuxc::TextBlock::TextProperty(), ttb.textCallbackToken);
            }
            if (ttb.originalOpacity && ttb.originalOpacity != wux::DependencyProperty::UnsetValue()) {
                tb.SetValue(wux::UIElement::OpacityProperty(), ttb.originalOpacity);
            } else {
                tb.ClearValue(wux::UIElement::OpacityProperty());
            }
        }
    }
    item.textBlocks.clear();

    if (auto panel = item.weakPanel.get()) {
        auto children = panel.Children();
        for (uint32_t c = children.Size(); c > 0; --c) {
            if (auto fe = children.GetAt(c - 1).try_as<wux::FrameworkElement>()) {
                if (fe.Name() == L"WindhawkFlagImage") {
                    children.RemoveAt(c - 1);
                }
            }
        }
    }
}

void TrackLanguageIconView(const wux::FrameworkElement& iconView);

void QueuePendingLoadedForElement(const wux::FrameworkElement& iconView) {
    if (!iconView || !g_pendingLoadedEntries || g_modUnloading.load(std::memory_order_acquire)) {
        return;
    }

    // Prune dead or duplicate pending entries first
    for (size_t i = 0; i < g_pendingLoadedEntries->size();) {
        auto existingEl = (*g_pendingLoadedEntries)[i].weakElement.get();
        if (!existingEl || existingEl == iconView) {
            (*g_pendingLoadedEntries)[i].loadedRevoker.revoke();
            g_pendingLoadedEntries->erase(g_pendingLoadedEntries->begin() + i);
        } else {
            ++i;
        }
    }

    PendingLoadedEntry pending{};
    pending.trayHwnd = FindTaskbarHwndForCurrentThread();
    pending.weakElement = winrt::make_weak(iconView);

    winrt::weak_ref<wux::FrameworkElement> weakFe = pending.weakElement;
    pending.loadedRevoker = iconView.Loaded(
        winrt::auto_revoke,
        [weakFe](winrt::Windows::Foundation::IInspectable const& sender, wux::RoutedEventArgs const&) {
            // Erasing this pending entry destroys loadedRevoker, which revokes the Loaded handler.
            if (g_pendingLoadedEntries) {
                for (size_t i = 0; i < g_pendingLoadedEntries->size(); ++i) {
                    auto el = (*g_pendingLoadedEntries)[i].weakElement.get();
                    if (!el || el == weakFe.get()) {
                        g_pendingLoadedEntries->erase(g_pendingLoadedEntries->begin() + i);
                        break;
                    }
                }
            }
            if (g_modUnloading.load(std::memory_order_acquire)) return;
            if (auto loadedFe = sender.try_as<wux::FrameworkElement>()) {
                TrackLanguageIconView(loadedFe);
            }
        });

    g_pendingLoadedEntries->push_back(std::move(pending));
}

void HandleIconViewUnloaded(const wux::FrameworkElement& iconView) {
    if (!g_win11Indicators) return;

    bool found = false;
    for (size_t i = 0; i < g_win11Indicators->size(); ++i) {
        auto existing = (*g_win11Indicators)[i].weakIconView.get();
        if (!existing || existing == iconView) {
            RestoreAndDetachWin11Indicator((*g_win11Indicators)[i]);
            g_win11Indicators->erase(g_win11Indicators->begin() + i);
            if (existing == iconView) {
                found = true;
                break;
            }
            --i;
        }
    }

    // Re-queue Loaded listener in case XAML reloads this same IconView instance later
    if (found && iconView && !g_modUnloading.load(std::memory_order_acquire)) {
        QueuePendingLoadedForElement(iconView);
    }
}

void TrackLanguageIconView(const wux::FrameworkElement& iconView) {
    if (!iconView || !g_win11Indicators || g_modUnloading.load(std::memory_order_acquire)) {
        return;
    }

    // Check if already tracked
    for (auto& existing : *g_win11Indicators) {
        if (auto iv = existing.weakIconView.get()) {
            if (iv == iconView) {
                UpdateWin11IndicatorFlag(existing);
                return;
            }
        }
    }

    // Locate Windows 11 system tray language elements by their exact XAML names
    wux::FrameworkElement contentEl = FindDescendantByName(iconView, L"LanguageTextIconContent", 10);
    wux::FrameworkElement innerTbEl = FindDescendantByName(iconView, L"LanguageInnerTextBlock", 10);
    if (!contentEl && !innerTbEl) {
        return; // Not the language indicator IconView (e.g., clock, Wi-Fi, volume, battery)
    }

    wuxc::Panel targetPanel{ nullptr };
    if (contentEl) {
        targetPanel = contentEl.try_as<wuxc::Panel>();
    }
    if (!targetPanel && innerTbEl) {
        wux::DependencyObject parent = wuxm::VisualTreeHelper::GetParent(innerTbEl);
        while (parent && parent != iconView) {
            targetPanel = parent.try_as<wuxc::Panel>();
            if (targetPanel) break;
            parent = wuxm::VisualTreeHelper::GetParent(parent);
        }
    }
    if (!targetPanel) {
        return;
    }

    std::vector<wuxc::TextBlock> nativeTextBlocks;
    CollectDescendantTextBlocks(targetPanel, nativeTextBlocks, 4);
    if (auto innerTb = innerTbEl.try_as<wuxc::TextBlock>()) {
        bool found = false;
        for (const auto& tb : nativeTextBlocks) {
            if (tb == innerTb) {
                found = true;
                break;
            }
        }
        if (!found) {
            nativeTextBlocks.push_back(innerTb);
        }
    }

    TrackedWin11Indicator entry{};
    entry.trayHwnd = FindTaskbarHwndForCurrentThread();
    entry.weakIconView = winrt::make_weak(iconView);
    entry.weakPanel = winrt::make_weak(targetPanel);

    winrt::weak_ref<wux::FrameworkElement> weakIconViewCopy = entry.weakIconView;
    entry.unloadedRevoker = iconView.Unloaded(
        winrt::auto_revoke,
        [weakIconViewCopy](winrt::Windows::Foundation::IInspectable const&, wux::RoutedEventArgs const&) {
            if (g_modUnloading.load(std::memory_order_acquire)) return;
            if (auto iv = weakIconViewCopy.get()) {
                HandleIconViewUnloaded(iv);
            }
        });

    for (const auto& tb : nativeTextBlocks) {
        TrackedWin11TextBlock ttb{};
        ttb.weakTb = winrt::make_weak(tb);
        ttb.originalOpacity = tb.ReadLocalValue(wux::UIElement::OpacityProperty());

        ttb.textCallbackToken = tb.RegisterPropertyChangedCallback(
            wuxc::TextBlock::TextProperty(),
            [weakIconViewCopy](wux::DependencyObject const&, wux::DependencyProperty const&) {
                if (g_modUnloading.load(std::memory_order_acquire) || !g_win11Indicators) return;
                auto iv = weakIconViewCopy.get();
                if (!iv) return;
                for (auto& item : *g_win11Indicators) {
                    if (auto trackedIv = item.weakIconView.get(); trackedIv && trackedIv == iv) {
                        UpdateWin11IndicatorFlag(item);
                        break;
                    }
                }
            });

        entry.textBlocks.push_back(std::move(ttb));
    }

    g_win11Indicators->push_back(std::move(entry));
    UpdateWin11IndicatorFlag(g_win11Indicators->back());
}

void HandleIconViewInstance(void* pThis) {
    if (!pThis || g_modUnloading.load(std::memory_order_acquire)) return;

    auto iconView = GetFrameworkElementFromImplThis(pThis);
    if (!iconView) return;

    if (iconView.IsLoaded()) {
        TrackLanguageIconView(iconView);
        return;
    }

    QueuePendingLoadedForElement(iconView);
}

void ScanVisualTreeForExistingIndicators(const wux::DependencyObject& root) {
    if (!root) return;

    std::vector<wux::DependencyObject> queue;
    queue.reserve(64);
    queue.push_back(root);

    for (size_t idx = 0; idx < queue.size(); ++idx) {
        wux::DependencyObject curr = queue[idx];

        if (auto fe = curr.try_as<wux::FrameworkElement>()) {
            std::wstring_view nameView(fe.Name());
            if (nameView == L"LanguageTextIconContent" || nameView == L"LanguageInnerTextBlock") {
                // Walk up to the enclosing IconView (UserControl / ContentControl)
                wux::FrameworkElement candidateIconView = fe;
                wux::DependencyObject p = wuxm::VisualTreeHelper::GetParent(fe);
                for (int d = 0; d < 8 && p; ++d) {
                    winrt::hstring cls = winrt::get_class_name(p);
                    if (std::wstring_view(cls).find(L"IconView") != std::wstring_view::npos) {
                        if (auto ivFe = p.try_as<wux::FrameworkElement>()) {
                            candidateIconView = ivFe;
                        }
                        break;
                    }
                    p = wuxm::VisualTreeHelper::GetParent(p);
                }
                TrackLanguageIconView(candidateIconView);
                continue;
            }
        }

        int count = wuxm::VisualTreeHelper::GetChildrenCount(curr);
        for (int i = 0; i < count; ++i) {
            if (auto child = wuxm::VisualTreeHelper::GetChild(curr, i)) {
                queue.push_back(std::move(child));
            }
        }
    }
}

void DiscoverExistingIndicatorsFromIconView(void* pThis) {
    if (!pThis || g_modUnloading.load(std::memory_order_acquire)) return;

    auto fe = GetFrameworkElementFromImplThis(pThis);
    if (!fe) return;

    auto xamlRoot = fe.XamlRoot();
    if (!xamlRoot || !xamlRoot.Content()) return;

    void* xamlRootAbi = winrt::get_abi(xamlRoot);
    if (!xamlRootAbi || g_scannedXamlRoots.count(xamlRootAbi) > 0) {
        return;
    }

    g_scannedXamlRoots.insert(xamlRootAbi);
    ScanVisualTreeForExistingIndicators(xamlRoot.Content());
}

using IconView_Init_t = void*(WINAPI*)(void* pThis);
IconView_Init_t IconView_Init_Orig = nullptr;

void* WINAPI IconView_Init_Hook(void* pThis) {
    void* ret = IconView_Init_Orig(pThis);
    try {
        HandleIconViewInstance(pThis);
    } catch (...) {
    }
    return ret;
}

using IconView_UpdateIcon_t = void(WINAPI*)(void* pThis);
IconView_UpdateIcon_t IconView_UpdateIcon_Orig = nullptr;

void WINAPI IconView_UpdateIcon_Hook(void* pThis) {
    IconView_UpdateIcon_Orig(pThis);
    try {
        DiscoverExistingIndicatorsFromIconView(pThis);
    } catch (...) {
    }
}

using IconView_OnPointerEntered_t = void(WINAPI*)(void* pThis, void* pArgs);
IconView_OnPointerEntered_t IconView_OnPointerEntered_Orig = nullptr;

void WINAPI IconView_OnPointerEntered_Hook(void* pThis, void* pArgs) {
    IconView_OnPointerEntered_Orig(pThis, pArgs);
    try {
        DiscoverExistingIndicatorsFromIconView(pThis);
    } catch (...) {
    }
}

bool HookSystemTraySymbols(HMODULE systemTrayMod) {
    if (!systemTrayMod) return false;

    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {
        {
            { L"public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void)" },
            &IconView_Init_Orig,
            IconView_Init_Hook,
            false
        },
        {
            { L"public: void __cdecl winrt::SystemTray::implementation::IconView::UpdateIcon(void)" },
            &IconView_UpdateIcon_Orig,
            IconView_UpdateIcon_Hook,
            true
        },
        {
            { L"public: void __cdecl winrt::SystemTray::implementation::IconView::OnPointerEntered(struct winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const &)" },
            &IconView_OnPointerEntered_Orig,
            IconView_OnPointerEntered_Hook,
            true
        }
    };

    if (!WindhawkUtils::HookSymbols(systemTrayMod, systemTrayHooks, ARRAYSIZE(systemTrayHooks))) {
        Wh_Log(L"Failed to hook required SystemTray::IconView symbols in system tray module.");
        return false;
    }

    return true;
}

void HandleLoadedModuleIfSystemTray(HMODULE mod, PCWSTR modulePath) {
    (void)modulePath;
    if (!mod || g_modUnloading.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_systemTrayHookAttempted && GetSystemTrayModuleHandle() == mod &&
        !g_systemTrayHookAttempted.exchange(true)) {
        if (HookSystemTraySymbols(mod)) {
            Wh_ApplyHookOperations();
        }
    }
}

void WINAPI RefreshWin11IndicatorsOnUIThread(void*) {
    if (g_modUnloading.load(std::memory_order_acquire) || !g_win11Indicators) return;
    DWORD tid = GetCurrentThreadId();
    for (auto& item : *g_win11Indicators) {
        if (!item.trayHwnd || GetWindowThreadProcessId(item.trayHwnd, nullptr) == tid) {
            UpdateWin11IndicatorFlag(item);
        }
    }
}

void WINAPI CleanupWin11IndicatorsOnUIThread(void*) {
    DWORD tid = GetCurrentThreadId();

    if (g_pendingLoadedEntries) {
        for (size_t i = 0; i < g_pendingLoadedEntries->size();) {
            HWND hwnd = (*g_pendingLoadedEntries)[i].trayHwnd;
            if (!hwnd || !IsWindow(hwnd) || GetWindowThreadProcessId(hwnd, nullptr) == tid) {
                (*g_pendingLoadedEntries)[i].loadedRevoker.revoke();
                g_pendingLoadedEntries->erase(g_pendingLoadedEntries->begin() + i);
            } else {
                ++i;
            }
        }
    }

    if (g_win11Indicators) {
        for (size_t i = 0; i < g_win11Indicators->size();) {
            HWND hwnd = (*g_win11Indicators)[i].trayHwnd;
            if (!hwnd || !IsWindow(hwnd) || GetWindowThreadProcessId(hwnd, nullptr) == tid) {
                RestoreAndDetachWin11Indicator((*g_win11Indicators)[i]);
                g_win11Indicators->erase(g_win11Indicators->begin() + i);
            } else {
                ++i;
            }
        }
    }

    g_scannedXamlRoots.clear();

    if (g_pendingLoadedEntries && g_pendingLoadedEntries->empty()) {
        g_pendingLoadedEntries.reset();
    }
    if (g_win11Indicators && g_win11Indicators->empty()) {
        g_win11Indicators.reset();
    }
}

void DispatchToAllTaskbarUIThreads(RunFromWindowThreadProc_t proc) {
    std::unordered_set<DWORD> dispatchedThreads;

    HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (primary) {
        DWORD tid = GetWindowThreadProcessId(primary, nullptr);
        if (tid && dispatchedThreads.insert(tid).second) {
            RunFromWindowThread(primary, proc, nullptr);
        }
    }

    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        DWORD tid = GetWindowThreadProcessId(secondary, nullptr);
        if (tid && dispatchedThreads.insert(tid).second) {
            RunFromWindowThread(secondary, proc, nullptr);
        }
    }
}

// ============================================================================
// 5. TASKBAR WINDOW HOOKS, WATCHDOG & WINDHAWK LIFECYCLE
// ============================================================================

LRESULT CALLBACK ShellTrayWatchdogSubclassProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {

    if (uMsg == g_stopWatchdogMsg && g_stopWatchdogMsg != 0) {
        KillTimer(hwnd, kWatchdogTimerId);
        RemovePropW(hwnd, L"WhFlagWatchdogActive");
        return 0;
    }

    if (!g_modUnloading.load(std::memory_order_acquire) &&
        !GetPropW(hwnd, L"WhFlagWatchdogActive")) {
        SetTimer(hwnd, kWatchdogTimerId, 200, nullptr);
        SetPropW(hwnd, L"WhFlagWatchdogActive", reinterpret_cast<HANDLE>(1));
    }

    if (uMsg == WM_TIMER && wParam == kWatchdogTimerId) {
        if (g_modUnloading.load(std::memory_order_acquire)) {
            KillTimer(hwnd, kWatchdogTimerId);
            RemovePropW(hwnd, L"WhFlagWatchdogActive");
            return 0;
        }

        // Only poll when Windows 10 TrayInputIndicatorWClass windows are active
        bool hasWin10Indicators = false;
        {
            std::shared_lock lock(g_stateLock);
            hasWin10Indicators = !g_subclassedIndicators.empty();
        }
        if (!hasWin10Indicators) {
            return 0;
        }

        static LANGID s_prevInputLang = 0;
        static LANGID s_prevCountry = 0;
        static WORD s_prevRawLayout = 0;

        KeyboardInfo kb = GetActiveKeyboardInfo();
        if (kb.inputLangId != s_prevInputLang ||
            kb.countryLangId != s_prevCountry ||
            kb.rawLayoutWord != s_prevRawLayout) {
            s_prevInputLang = kb.inputLangId;
            s_prevCountry = kb.countryLangId;
            s_prevRawLayout = kb.rawLayoutWord;
            InvalidateAllWin10Indicators(false);
        }
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        KillTimer(hwnd, kWatchdogTimerId);
        RemovePropW(hwnd, L"WhFlagWatchdogActive");
        HWND expected = hwnd;
        g_shellTrayHwnd.compare_exchange_strong(expected, nullptr);
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void AttachShellTrayWatchdog(HWND shellTray) {
    if (!shellTray || g_modUnloading.load(std::memory_order_acquire)) return;
    HWND expected = nullptr;
    if (!g_shellTrayHwnd.compare_exchange_strong(expected, shellTray)) {
        return;
    }
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(shellTray, ShellTrayWatchdogSubclassProc, 0)) {
        PostMessageW(shellTray, WM_TIMER, kWatchdogTimerId, 0);
    } else {
        g_shellTrayHwnd.store(nullptr, std::memory_order_release);
    }
}

using CreateWindowExW_t = HWND(WINAPI*)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_Orig = nullptr;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && !g_modUnloading.load(std::memory_order_acquire)) {
        wchar_t cls[64] = {};
        if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
            if (wcscmp(cls, L"TrayInputIndicatorWClass") == 0) {
                AttachWin10IndicatorWindow(hwnd);
                HWND primaryTray = FindWindowW(L"Shell_TrayWnd", nullptr);
                if (primaryTray) {
                    AttachShellTrayWatchdog(primaryTray);
                }
            } else if (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
                       wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
                ScanAndAttachWin10Indicators();
            }
        }
    }
    return hwnd;
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExW_t LoadLibraryExW_Orig = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE mod = LoadLibraryExW_Orig(lpLibFileName, hFile, dwFlags);
    if (mod) {
        HandleLoadedModuleIfSystemTray(mod, lpLibFileName);
    }
    return mod;
}

void LoadSettings() {
    int scale = Wh_GetIntSetting(L"FlagScale");
    if (scale < 1) scale = 1;
    if (scale > 5) scale = 5;

    ModSettings next{};
    // 3:2 aspect ratio presets:
    // 1 -> 18x12, 2 -> 21x14, 3 -> 24x16 (default), 4 -> 27x18, 5 -> 30x20
    next.flagScale = scale;
    next.flagWidth = 15 + scale * 3;
    next.flagHeight = 10 + scale * 2;
    next.roundedCorners = Wh_GetIntSetting(L"RoundedCorners") != 0;
    next.darkMode = Wh_GetIntSetting(L"DarkMode") != 0;
    next.useUkFlagForUsLayout = Wh_GetIntSetting(L"UseUkFlagForUsLayout") != 0;

    std::unique_lock lock(g_stateLock);
    g_settings = next;
    g_flagPixelCache.clear();
}

BOOL Wh_ModInit() {
    g_modUnloading.store(false, std::memory_order_release);
    g_stopWatchdogMsg = RegisterWindowMessageW(L"Windhawk_StopWatchdog_" WH_MOD_ID);
    g_runFromWindowThreadMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    LoadSettings();

    Gdiplus::GdiplusStartupInput gdipInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdipInput, nullptr);

    // 1. Hook Windows 11 SystemTray::IconView in SystemTray.dll / Taskbar.View.dll / ExplorerExtensions.dll
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto pLoadLibraryExW = kernelBase
        ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"))
        : LoadLibraryExW;
    if (pLoadLibraryExW) {
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Orig);
    }

    HMODULE systemTrayMod = GetSystemTrayModuleHandle();
    if (systemTrayMod && !g_systemTrayHookAttempted.exchange(true)) {
        HookSystemTraySymbols(systemTrayMod);
    }

    // 2. Install Windows 10 indicator text suppression & buffered-paint hooks
    //    (strictly gated to TrayInputIndicatorWClass paint via thread-local checks)
    InstallDirect2DAndDWriteHooks();

    WindhawkUtils::SetFunctionHook(BeginBufferedPaint, BeginBufferedPaint_Hook, &BeginBufferedPaint_Orig);
    WindhawkUtils::SetFunctionHook(EndBufferedPaint, EndBufferedPaint_Hook, &EndBufferedPaint_Orig);
    WindhawkUtils::SetFunctionHook(GdiAlphaBlend, GdiAlphaBlend_Hook, &GdiAlphaBlend_Orig);
    WindhawkUtils::SetFunctionHook(BitBlt, BitBlt_Hook, &BitBlt_Orig);
    WindhawkUtils::SetFunctionHook(StretchBlt, StretchBlt_Hook, &StretchBlt_Orig);
    WindhawkUtils::SetFunctionHook(DrawThemeTextEx, DrawThemeTextEx_Hook, &DrawThemeTextEx_Orig);
    WindhawkUtils::SetFunctionHook(DrawThemeText, DrawThemeText_Hook, &DrawThemeText_Orig);
    WindhawkUtils::SetFunctionHook(DrawTextW, DrawTextW_Hook, &DrawTextW_Orig);
    WindhawkUtils::SetFunctionHook(DrawTextExW, DrawTextExW_Hook, &DrawTextExW_Orig);
    WindhawkUtils::SetFunctionHook(ExtTextOutW, ExtTextOutW_Hook, &ExtTextOutW_Orig);
    WindhawkUtils::SetFunctionHook(TextOutW, TextOutW_Hook, &TextOutW_Orig);
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Orig);

    // 3. Attach to already-existing Windows 10 indicators (when enabled mid-session)
    ScanAndAttachWin10Indicators();
    InvalidateAllWin10Indicators(true);

    bool hasWin10Indicators = false;
    {
        std::shared_lock lock(g_stateLock);
        hasWin10Indicators = !g_subclassedIndicators.empty();
    }
    if (hasWin10Indicators) {
        HWND shellTray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (shellTray) {
            AttachShellTrayWatchdog(shellTray);
        }
    }

    // 4. If enabled mid-session on Windows 11, prompt taskbars to refresh tray icons so
    //    IconView::UpdateIcon discovers existing language indicators via XamlRoot immediately.
    if (systemTrayMod) {
        HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (primary) {
            SendNotifyMessageW(primary, WM_SETTINGCHANGE, 0, 0);
        }
        HWND secondary = nullptr;
        while ((secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
            SendNotifyMessageW(secondary, WM_SETTINGCHANGE, 0, 0);
        }
    }

    ModSettings settings = GetSettingsSnapshot();
    Wh_Log(L"Taskbar Keyboard Layout Flag v1.0 initialized (Scale=%d, %dx%d, DarkMode=%d).",
           settings.flagScale, settings.flagWidth, settings.flagHeight, settings.darkMode ? 1 : 0);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_modUnloading.load(std::memory_order_acquire) ||
        g_systemTrayHookAttempted.load(std::memory_order_acquire)) {
        return;
    }

    if (HMODULE systemTrayMod = GetSystemTrayModuleHandle()) {
        if (!g_systemTrayHookAttempted.exchange(true)) {
            if (HookSystemTraySymbols(systemTrayMod)) {
                Wh_ApplyHookOperations();
            }
        }
    }
}

void Wh_ModUninit() {
    g_modUnloading.store(true, std::memory_order_release);

    // 1. Restore Windows 11 XAML indicator TextBlocks and remove injected Image on the UI thread
    DispatchToAllTaskbarUIThreads(CleanupWin11IndicatorsOnUIThread);

    // 2. Stop watchdog timer on Shell_TrayWnd's thread before removing the subclass
    HWND shellTray = g_shellTrayHwnd.exchange(nullptr, std::memory_order_acq_rel);
    if (shellTray && IsWindow(shellTray)) {
        if (g_stopWatchdogMsg != 0) {
            SendMessageW(shellTray, g_stopWatchdogMsg, 0, 0);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(shellTray, ShellTrayWatchdogSubclassProc);
    }

    // 3. Remove Windows 10 indicator subclasses and trigger native repaint
    std::vector<HWND> indicators;
    {
        std::unique_lock lock(g_stateLock);
        indicators.swap(g_subclassedIndicators);
        g_flagPixelCache.clear();
    }

    for (HWND hwnd : indicators) {
        if (IsWindow(hwnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, Win10IndicatorSubclassProc);
            SendNotifyMessageW(hwnd, WM_THEMECHANGED, 0, 0);
            InvalidateRect(hwnd, nullptr, TRUE);
        }
    }

    for (HMODULE mod : g_loadedModules) {
        if (mod) {
            FreeLibrary(mod);
        }
    }
    g_loadedModules.clear();

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    InvalidateAllWin10Indicators(true);
    DispatchToAllTaskbarUIThreads(RefreshWin11IndicatorsOnUIThread);
}
