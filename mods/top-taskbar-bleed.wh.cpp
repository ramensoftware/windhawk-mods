// ==WindhawkMod==
// @id              top-taskbar-bleed
// @name            Top Taskbar Bleed
// @description     Bleeds the color of a maximized window's top edge into a top-docked taskbar behind the task buttons
// @version         1.0.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -ldwmapi -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Top Taskbar Bleed

When a maximized window sits under a top-docked taskbar, the taskbar gets a
full-width flat color panel behind its buttons that matches the window's top
edge, so the window's title bar appears to continue up into the bar.

![Demo](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/refs/heads/main/top-taskbar-bleed/demo.gif)

The mod only activates for a taskbar docked at the top of the screen; on other
dock positions it stays idle.

How it works:

- A tracker thread watches window events and finds, per monitor, the foremost
  maximized window (the first maximized window in z-order on that monitor -
  not necessarily the foreground window, so a small dialog on top of a
  maximized app doesn't cancel the bleed).
- The strip of pixels along the window's top edge is captured from the screen
  and reduced to its dominant color, then smoothed so scrolling and video don't
  make the bar flicker.
- One panel per taskbar is inserted into the taskbar's XAML tree, above the
  taskbar background and below the buttons; color changes apply instantly.

Supports multiple monitors (each taskbar follows its own monitor).

The panel's shape is adjustable: **Margins** takes "left,top,right,bottom"
(positive insets the panel from that taskbar edge, negative extends it) and
**Corner radius** takes "top-left,top-right,bottom-right,bottom-left", both in
screen orientation (the bottom edge of the panel meets the window, so the
rounded corners are normally the top ones).

## Notes

- The taskbar is discovered through `Taskbar.View.dll` hooks (symbols are
  downloaded by Windhawk on first run), not through XAML diagnostics, so the mod
  coexists with Windows 11 Taskbar Styler and other XAML-diagnostics consumers.
- The bleed applies as soon as the mod is enabled. Discovery rides on taskbar
  layout events, so when the mod is enabled mid-session it adds and removes an
  invisible window on each monitor once, which makes every taskbar lay out its
  buttons and announce itself. Each taskbar is matched to its monitor through
  the screen position its XAML automation peers report, so multiple monitors
  are told apart without any interaction.
- With a maximized window present, the mod samples a few thousand pixels every
  few hundred milliseconds. This is cheap, but set **Poll interval** higher if
  you want it even lighter.
- Windows that exclude themselves from capture (protected media, some
  remote-desktop clients) return no usable pixels; the panel then keeps its
  last color.
- The 1px taskbar border line (`BackgroundStroke`) breaks the illusion; it is
  hidden while a panel is showing and restored when the panel hides.
- Readability: a light title bar bled into a dark-mode taskbar leaves white
  labels on a light panel (and vice versa). While a panel is showing, the mod
  switches that monitor's taskbar to the theme that matches the bled color
  (light panel -> light mode, dark panel -> dark mode) and restores the normal
  theme when the panel hides. This is done per taskbar island through XAML's
  `RequestedTheme`, so only the affected monitor changes and the system theme
  itself is untouched.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Panel:
  - Margins: "0,0,0,0"
    $name: Margins
    $description: Four values in pixels as left,top,right,bottom (a single value applies to all four), measured from the taskbar edges. Positive insets that edge of the panel, negative extends it.
  - CornerRadius: "0,0,0,0"
    $name: Corner radius
    $description: Four values in pixels as top-left,top-right,bottom-right,bottom-left (a single value applies to all four), screen orientation. Keep the bottom corners (the ones touching the window) at 0 for a seamless bleed.
  $name: Panel
- Sampling:
  - SampleDepth: 4
    $name: Sample depth (px)
    $description: How many rows of pixels are sampled along the window's top edge.
  - SampleInset: 1
    $name: Sample inset (px)
    $description: Skip this many pixels from the window edge before sampling. 1 skips the 1px window border Windows draws on the edge.
  - PollIntervalMs: 300
    $name: Poll interval (ms)
    $description: How often the edge is re-sampled while a maximized window is present.
  $name: Sampling
- Smoothing:
  - ChangeThreshold: 10
    $name: Change threshold
    $description: Color changes smaller than this (0-255 per channel) are treated as jitter and ignored while they fluctuate; a small change that stays put is still applied after the settle time below, so the panel always converges on the exact edge color.
  - SettleMs: 1000
    $name: Settle time (ms)
    $description: How long a small color change must persist before it is applied.
  - ConfirmSamples: 2
    $name: Confirm samples
    $description: A new color must be seen this many consecutive times before the panel changes.
  - MinCommitIntervalMs: 400
    $name: Minimum time between changes (ms)
  $name: Smoothing
- Behavior:
  - ExcludedProcesses: [""]
    $name: Excluded programs
    $description: Executable names (e.g. game.exe) whose windows never trigger the bleed.
  $name: Behavior
- Debug:
  - LogWindowDecisions: false
    $name: Log window decisions
    $description: Log why the foreground window was or wasn't chosen as the bleed source, and the sampled colors (diagnostics only).
  $name: Debug
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <objbase.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wu = winrt::Windows::UI;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxap = winrt::Windows::UI::Xaml::Automation::Peers;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

// ============================================================================
// Settings
// ============================================================================

struct Rgb {
    int r = 0, g = 0, b = 0;
    bool operator==(const Rgb& o) const { return r == o.r && g == o.g && b == o.b; }
    bool operator!=(const Rgb& o) const { return !(*this == o); }
};

struct Settings {
    int marginLeft = 0, marginTop = 0, marginRight = 0, marginBottom = 0;
    int radiusTopLeft = 0, radiusTopRight = 0, radiusBottomRight = 0, radiusBottomLeft = 0;

    int sampleDepth = 4;
    int sampleInset = 1;
    int pollIntervalMs = 300;

    int changeThreshold = 10;
    int settleMs = 1000;
    int confirmSamples = 2;
    int minCommitIntervalMs = 400;

    std::vector<std::wstring> excludedProcesses;  // lowercase basenames

    bool logDecisions = false;
};

std::mutex g_settingsMutex;
Settings g_settings;

Settings GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

// Parse "a,b,c,d" (or a single "a" applied to all four). Returns false and
// leaves `out` untouched on any other shape.
bool ParseFour(std::wstring_view text, int (&out)[4]) {
    int vals[4];
    int n = 0;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t comma = text.find(L',', pos);
        std::wstring_view tok = text.substr(pos, comma == std::wstring_view::npos ? std::wstring_view::npos : comma - pos);
        while (!tok.empty() && iswspace(tok.front())) tok.remove_prefix(1);
        while (!tok.empty() && iswspace(tok.back())) tok.remove_suffix(1);
        if (tok.empty() || n == 4) return false;
        std::wstring tmp(tok);
        wchar_t* end = nullptr;
        long v = wcstol(tmp.c_str(), &end, 10);
        if (!end || *end != L'\0') return false;
        vals[n++] = (int)v;
        if (comma == std::wstring_view::npos) break;
        pos = comma + 1;
    }
    if (n == 1) {
        for (int i = 0; i < 4; i++) out[i] = vals[0];
        return true;
    }
    if (n == 4) {
        for (int i = 0; i < 4; i++) out[i] = vals[i];
        return true;
    }
    return false;
}

std::wstring GetStringSettingStr(PCWSTR name) {
    PCWSTR s = Wh_GetStringSetting(name);
    std::wstring r = s ? s : L"";
    Wh_FreeStringSetting(s);
    return r;
}

std::wstring GetStringSettingIdx(PCWSTR fmt, int index) {
    PCWSTR s = Wh_GetStringSetting(fmt, index);
    std::wstring r = s ? s : L"";
    Wh_FreeStringSetting(s);
    return r;
}

std::wstring ToLowerBasename(std::wstring s) {
    size_t pos = s.find_last_of(L"\\/");
    if (pos != std::wstring::npos) s = s.substr(pos + 1);
    for (auto& c : s) c = (wchar_t)std::towlower(c);
    return s;
}

void LoadSettings() {
    Settings s;

    {
        int m[4] = {0, 0, 0, 0};
        std::wstring text = GetStringSettingStr(L"Panel.Margins");
        if (!ParseFour(text, m)) Wh_Log(L"[bleed] Margins '%s' not understood, using defaults", text.c_str());
        s.marginLeft = m[0];
        s.marginTop = m[1];
        s.marginRight = m[2];
        s.marginBottom = m[3];
    }
    {
        int r[4] = {0, 0, 0, 0};
        std::wstring text = GetStringSettingStr(L"Panel.CornerRadius");
        if (!ParseFour(text, r)) Wh_Log(L"[bleed] CornerRadius '%s' not understood, using defaults", text.c_str());
        s.radiusTopLeft = std::max(0, r[0]);
        s.radiusTopRight = std::max(0, r[1]);
        s.radiusBottomRight = std::max(0, r[2]);
        s.radiusBottomLeft = std::max(0, r[3]);
    }

    s.sampleDepth = std::clamp(Wh_GetIntSetting(L"Sampling.SampleDepth"), 1, 64);
    s.sampleInset = std::clamp(Wh_GetIntSetting(L"Sampling.SampleInset"), 0, 256);
    s.pollIntervalMs = std::clamp(Wh_GetIntSetting(L"Sampling.PollIntervalMs"), 50, 60000);

    s.changeThreshold = std::clamp(Wh_GetIntSetting(L"Smoothing.ChangeThreshold"), 0, 255);
    s.settleMs = std::clamp(Wh_GetIntSetting(L"Smoothing.SettleMs"), 0, 60000);
    s.confirmSamples = std::clamp(Wh_GetIntSetting(L"Smoothing.ConfirmSamples"), 1, 100);
    s.minCommitIntervalMs = std::max(0, Wh_GetIntSetting(L"Smoothing.MinCommitIntervalMs"));

    for (int i = 0;; i++) {
        std::wstring p = GetStringSettingIdx(L"Behavior.ExcludedProcesses[%d]", i);
        if (p.empty()) break;
        s.excludedProcesses.push_back(ToLowerBasename(std::move(p)));
    }

    s.logDecisions = Wh_GetIntSetting(L"Debug.LogWindowDecisions") != 0;

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(s);
}

// ============================================================================
// Shared state between the tracker thread and the XAML UI thread
// ============================================================================

std::atomic<bool> g_unloading{false};

// One per taskbar XAML island. Mutated only on the island's UI thread; the
// tracker thread only reads plain fields (hwnd, dispatcher) under the mutex.
struct Island {
    HWND hwnd = nullptr;               // DesktopWindowContentBridge window of this taskbar
    void* gridId = nullptr;            // identity of the hosting Grid
    void* frameId = nullptr;           // identity of the Taskbar.TaskbarFrame element
    ULONGLONG lastMapAttemptTick = 0;  // throttles hwnd re-mapping of an unresolved island
    winrt::weak_ref<wux::FrameworkElement> stroke;
    wuxc::Border panel{nullptr};
    wuxm::SolidColorBrush brush{nullptr};
    wuc::CoreDispatcher dispatcher{nullptr};
    bool shown = false;
    bool strokeHidden = false;
    bool unparentedLogged = false;
    winrt::weak_ref<wux::FrameworkElement> themeRoot;  // island root content; RequestedTheme is set here
    int forcedTheme = 0;                                // 0 none (Default), 1 light, 2 dark
    Rgb color{};
};

std::mutex g_islandsMutex;
std::vector<std::shared_ptr<Island>> g_islands;  // guarded by g_islandsMutex

struct IslandSnapshot {
    HWND hwnd;
    wuc::CoreDispatcher dispatcher;
};

std::vector<IslandSnapshot> SnapshotIslands() {
    std::vector<IslandSnapshot> out;
    std::lock_guard lock(g_islandsMutex);
    for (auto& isl : g_islands) {
        if (isl->hwnd && isl->dispatcher) out.push_back({isl->hwnd, isl->dispatcher});
    }
    return out;
}

void* IdentityOf(wf::IInspectable const& o) {
    if (!o) return nullptr;
    auto unk = o.try_as<wf::IUnknown>();
    return winrt::get_abi(unk);
}

wu::Color ToColor(Rgb c) {
    return wu::Color{255, (uint8_t)std::clamp(c.r, 0, 255), (uint8_t)std::clamp(c.g, 0, 255),
                     (uint8_t)std::clamp(c.b, 0, 255)};
}

void ApplyIslandUpdateOnUiThread(HWND hwnd, bool active, Rgb color);

void PostIslandUpdate(wuc::CoreDispatcher const& dispatcher, HWND hwnd, bool active, Rgb color) {
    if (g_unloading) return;
    try {
        dispatcher.RunAsync(wuc::CoreDispatcherPriority::High,
                            [hwnd, active, color]() { ApplyIslandUpdateOnUiThread(hwnd, active, color); });
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"[bleed] RunAsync failed: %08X", e.code().value);
    } catch (...) {
        Wh_Log(L"[bleed] RunAsync failed");
    }
}

// ============================================================================
// Tracker thread: window events, target resolution, sampling, smoothing
// ============================================================================

constexpr UINT WM_TRACKER_TICK = WM_APP + 1;
constexpr UINT WM_TRACKER_SETTINGS = WM_APP + 2;

HANDLE g_trackerThread = nullptr;
DWORD g_trackerThreadId = 0;
HANDLE g_trackerReady = nullptr;

// Tracker-thread-only state.
UINT_PTR g_debounceTimer = 0;
UINT_PTR g_pollTimer = 0;
int g_pollTimerInterval = 0;
std::vector<HWINEVENTHOOK> g_winEventHooks;

struct TrackState {
    HWND target = nullptr;
    bool active = false;
    bool notTopLogged = false;
    Rgb committed{};
    Rgb candidate{};
    int candidateCount = 0;
    ULONGLONG lastCommitTick = 0;
    // Drift correction: a sub-threshold difference from the committed color,
    // and since when it has been seen continuously (0 = none).
    Rgb drift{};
    ULONGLONG driftSinceTick = 0;
    uint64_t lastHash = 0;
    bool haveLastSample = false;
    Rgb lastSample{};
    bool blockedLogged = false;
    // Diagnostics (Log window decisions).
    RECT lastStrip{};
    Rgb lastRowFirst{}, lastRowLast{};
};
std::unordered_map<HWND, TrackState> g_track;  // keyed by island hwnd

void RequestTick() {
    if (g_trackerThreadId) PostThreadMessageW(g_trackerThreadId, WM_TRACKER_TICK, 0, 0);
}

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) return cloaked != 0;
    return false;
}

bool GetFrameBounds(HWND hwnd, RECT& rc) {
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc)))) return true;
    return GetWindowRect(hwnd, &rc) != FALSE;
}

std::wstring ProcessBasename(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE h = pid ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid) : nullptr;
    if (!h) return L"";
    WCHAR path[MAX_PATH * 2];
    DWORD len = ARRAYSIZE(path);
    std::wstring base;
    if (QueryFullProcessImageNameW(h, 0, path, &len)) base = ToLowerBasename(path);
    CloseHandle(h);
    return base;
}

bool IsProcessExcluded(HWND hwnd, const Settings& s) {
    if (s.excludedProcesses.empty()) return false;
    std::wstring base = ProcessBasename(hwnd);
    return !base.empty() &&
           std::find(s.excludedProcesses.begin(), s.excludedProcesses.end(), base) != s.excludedProcesses.end();
}

std::wstring DescribeWindow(HWND hwnd) {
    WCHAR cls[64] = L"?";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    std::wstring exe = ProcessBasename(hwnd);
    RECT wr{};
    GetFrameBounds(hwnd, wr);
    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    WCHAR buf[512];
    swprintf_s(buf, L"%p %s [%s] vis=%d iconic=%d zoomed=%d cloaked=%u ex=%08X frame=(%d,%d,%d,%d)", hwnd, cls,
               exe.empty() ? L"?" : exe.c_str(), IsWindowVisible(hwnd) ? 1 : 0, IsIconic(hwnd) ? 1 : 0,
               IsZoomed(hwnd) ? 1 : 0, cloaked, (unsigned)GetWindowLongPtrW(hwnd, GWL_EXSTYLE), wr.left, wr.top,
               wr.right, wr.bottom);
    return buf;
}

struct EnumCtx {
    const Settings* settings;
    std::unordered_map<HMONITOR, HWND> foremost;  // first maximized window in z-order, per monitor
};

BOOL CALLBACK EnumWindowsForemostProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumCtx*>(lParam);

    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || !IsZoomed(hwnd)) return TRUE;
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    if (IsWindowCloaked(hwnd)) return TRUE;

    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!hmon || ctx->foremost.count(hmon)) return TRUE;
    if (IsProcessExcluded(hwnd, *ctx->settings)) return TRUE;

    ctx->foremost[hmon] = hwnd;
    return TRUE;
}

bool IsTaskbarTopDocked(HWND taskbar, const RECT& mon) {
    RECT tb;
    if (!taskbar || !GetWindowRect(taskbar, &tb)) return false;
    bool horizontal = (tb.right - tb.left) >= (tb.bottom - tb.top);
    return horizontal && std::abs(tb.top - mon.top) < std::abs(tb.bottom - mon.bottom);
}

// Rows to sample along the window's top edge. The frame is clipped to the work
// area first (some hosts report a maximized frame that extends under the
// taskbar by the invisible border width), and then kept off the taskbar
// window itself, so taskbar pixels - including the panel's own color - can
// never feed back into the sampler.
RECT ComputeStrip(const RECT& frame, const RECT& work, HWND taskbar, const Settings& s) {
    RECT wr;
    if (!IntersectRect(&wr, &frame, &work)) return RECT{0, 0, 0, 0};
    LONG top = wr.top + s.sampleInset;
    RECT tb;
    if (taskbar && GetWindowRect(taskbar, &tb) && top < tb.bottom && wr.left < tb.right && wr.right > tb.left) {
        top = std::max(top, tb.bottom + s.sampleInset);
    }
    RECT r{wr.left, top, wr.right, top + s.sampleDepth};
    RECT clipped;
    if (!IntersectRect(&clipped, &r, &work)) return RECT{0, 0, 0, 0};
    return clipped;
}

struct StripBitmap {
    std::vector<uint32_t> px;  // BGRX, top-down
    int w = 0, h = 0;
};

bool CaptureStrip(const RECT& strip, StripBitmap& out) {
    int w = strip.right - strip.left, h = strip.bottom - strip.top;
    if (w <= 0 || h <= 0 || (long long)w * h > 8192LL * 64) return false;

    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    HDC mem = CreateCompatibleDC(screen);
    if (!mem) {
        ReleaseDC(nullptr, screen);
        return false;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    bool ok = false;
    if (bmp && bits) {
        HGDIOBJ old = SelectObject(mem, bmp);
        ok = BitBlt(mem, 0, 0, w, h, screen, strip.left, strip.top, SRCCOPY) != FALSE;
        GdiFlush();
        if (ok) {
            out.w = w;
            out.h = h;
            out.px.assign(static_cast<uint32_t*>(bits), static_cast<uint32_t*>(bits) + (size_t)w * h);
        }
        SelectObject(mem, old);
    }
    if (bmp) DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return ok;
}

uint64_t HashPixels(const std::vector<uint32_t>& px) {
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t v : px) {
        h ^= v & 0x00FFFFFFu;
        h *= 1099511628211ULL;
    }
    return h;
}

// Dominant color: 5-bit-per-channel histogram mode, refined by two mean-shift
// passes so a tight cluster straddling a bin edge (Mica, acrylic, dithered flat
// colors) still yields its true center.
Rgb DominantColor(const std::vector<uint32_t>& px) {
    struct Bin {
        uint32_t count = 0;
        uint64_t r = 0, g = 0, b = 0;
    };
    static std::vector<Bin> bins(32768);
    static std::vector<uint32_t> touched;
    touched.clear();

    for (uint32_t v : px) {
        uint32_t b = v & 0xFF, g = (v >> 8) & 0xFF, r = (v >> 16) & 0xFF;
        uint32_t key = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
        Bin& bin = bins[key];
        if (bin.count == 0) touched.push_back(key);
        bin.count++;
        bin.r += r;
        bin.g += g;
        bin.b += b;
    }
    if (touched.empty()) return Rgb{};

    std::sort(touched.begin(), touched.end(),
              [](uint32_t a, uint32_t b) { return bins[a].count > bins[b].count; });

    Rgb result;
    const Bin& top = bins[touched[0]];
    if ((double)top.count / (double)px.size() >= 0.15 || touched.size() == 1) {
        result = Rgb{(int)(top.r / top.count), (int)(top.g / top.count), (int)(top.b / top.count)};
    } else {
        // No clear winner (gradient, photo): population-weighted mean of the top bins.
        uint64_t cnt = 0, r = 0, g = 0, b = 0;
        size_t k = std::min<size_t>(3, touched.size());
        for (size_t i = 0; i < k; i++) {
            const Bin& bin = bins[touched[i]];
            cnt += bin.count;
            r += bin.r;
            g += bin.g;
            b += bin.b;
        }
        result = Rgb{(int)(r / cnt), (int)(g / cnt), (int)(b / cnt)};
    }
    for (uint32_t key : touched) bins[key] = Bin{};

    for (int pass = 0; pass < 2; pass++) {
        uint64_t cnt = 0, r = 0, g = 0, b = 0;
        for (uint32_t v : px) {
            int pb = v & 0xFF, pg = (v >> 8) & 0xFF, pr = (v >> 16) & 0xFF;
            if (std::abs(pr - result.r) <= 12 && std::abs(pg - result.g) <= 12 && std::abs(pb - result.b) <= 12) {
                cnt++;
                r += pr;
                g += pg;
                b += pb;
            }
        }
        if (cnt == 0) break;
        Rgb refined{(int)std::lround((double)r / cnt), (int)std::lround((double)g / cnt),
                    (int)std::lround((double)b / cnt)};
        if (refined == result) break;
        result = refined;
    }
    return result;
}

int ColorDistance(Rgb a, Rgb b) {
    return std::max({std::abs(a.r - b.r), std::abs(a.g - b.g), std::abs(a.b - b.b)});
}

void Commit(TrackState& st, const IslandSnapshot& isl, Rgb c, ULONGLONG now, const Settings& s) {
    if (s.logDecisions) {
        Wh_Log(L"[bleed] island %p: commit #%02X%02X%02X (target %p, strip=(%d,%d,%d,%d), first row #%02X%02X%02X, "
               L"last row #%02X%02X%02X, fg=%p)",
               isl.hwnd, c.r, c.g, c.b, st.target, st.lastStrip.left, st.lastStrip.top, st.lastStrip.right,
               st.lastStrip.bottom, st.lastRowFirst.r, st.lastRowFirst.g, st.lastRowFirst.b, st.lastRowLast.r,
               st.lastRowLast.g, st.lastRowLast.b, GetForegroundWindow());
    }
    st.active = true;
    st.committed = c;
    st.candidateCount = 0;
    st.driftSinceTick = 0;
    st.lastCommitTick = now;
    PostIslandUpdate(isl.dispatcher, isl.hwnd, true, c);
}

void Deactivate(TrackState& st, const IslandSnapshot& isl) {
    if (st.active) PostIslandUpdate(isl.dispatcher, isl.hwnd, false, st.committed);
    bool notTopLogged = st.notTopLogged;
    st = TrackState{};
    st.notTopLogged = notTopLogged;
}

void Smooth(TrackState& st, const IslandSnapshot& isl, Rgb c, bool targetChanged, const Settings& s) {
    ULONGLONG now = GetTickCount64();
    if (!st.active || targetChanged) {
        Commit(st, isl, c, now, s);
        return;
    }
    if (ColorDistance(c, st.committed) <= s.changeThreshold) {
        st.candidateCount = 0;
        // Sub-threshold difference: not a change worth animating on its own,
        // but if it persists it is the real edge color (the first commit may
        // have landed mid-transition), so converge on it after the settle time.
        if (c == st.committed) {
            st.driftSinceTick = 0;
        } else if (st.driftSinceTick == 0 || ColorDistance(c, st.drift) > 2) {
            st.drift = c;
            st.driftSinceTick = now;
        } else if (now - st.driftSinceTick >= (ULONGLONG)s.settleMs) {
            Commit(st, isl, c, now, s);
        }
        return;
    }
    st.driftSinceTick = 0;
    if (st.candidateCount > 0 && ColorDistance(c, st.candidate) <= s.changeThreshold) {
        st.candidateCount++;
    } else {
        st.candidate = c;
        st.candidateCount = 1;
    }
    if (st.candidateCount >= s.confirmSamples && now - st.lastCommitTick >= (ULONGLONG)s.minCommitIntervalMs) {
        Commit(st, isl, st.candidate, now, s);
    }
}

// Capture failed (protected content, capture-excluded window, transient GDI
// failure): log once per target and keep whatever the panel shows.
void HandleBlocked(TrackState& st, const IslandSnapshot& isl, HWND target) {
    if (!st.blockedLogged) {
        Wh_Log(L"[bleed] island %p: capture blocked for target %p, keeping last color", isl.hwnd, target);
        st.blockedLogged = true;
    }
}

void ProcessIsland(const IslandSnapshot& isl, const Settings& s, const std::unordered_map<HMONITOR, HWND>& foremost,
                   bool& anyTarget) {
    TrackState& st = g_track[isl.hwnd];

    HMONITOR hmon = MonitorFromWindow(isl.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!hmon || !GetMonitorInfoW(hmon, &mi)) {
        Deactivate(st, isl);
        return;
    }

    HWND taskbar = GetAncestor(isl.hwnd, GA_ROOT);
    if (!IsTaskbarTopDocked(taskbar, mi.rcMonitor)) {
        if (!st.notTopLogged) Wh_Log(L"[bleed] island %p: taskbar not docked at top, idle", isl.hwnd);
        Deactivate(st, isl);
        st.notTopLogged = true;
        return;
    }
    st.notTopLogged = false;

    auto it = foremost.find(hmon);
    HWND target = it == foremost.end() ? nullptr : it->second;
    if (!target) {
        if (st.target) Wh_Log(L"[bleed] island %p: no maximized window", isl.hwnd);
        Deactivate(st, isl);
        return;
    }

    anyTarget = true;
    bool targetChanged = target != st.target;
    if (targetChanged) {
        if (s.logDecisions) Wh_Log(L"[bleed] island %p: target %s", isl.hwnd, DescribeWindow(target).c_str());
        else Wh_Log(L"[bleed] island %p: target %p", isl.hwnd, target);
        st.target = target;
        st.haveLastSample = false;
        st.lastHash = 0;
        st.blockedLogged = false;
    }

    RECT wr;
    if (!GetFrameBounds(target, wr)) {
        HandleBlocked(st, isl, target);
        return;
    }
    RECT strip = ComputeStrip(wr, mi.rcWork, taskbar, s);
    st.lastStrip = strip;
    if (targetChanged && s.logDecisions) {
        Wh_Log(L"[bleed] island %p: strip=(%d,%d,%d,%d)", isl.hwnd, strip.left, strip.top, strip.right, strip.bottom);
    }

    StripBitmap bmp;
    if (!CaptureStrip(strip, bmp) || bmp.px.empty()) {
        HandleBlocked(st, isl, target);
        return;
    }

    uint64_t hash = HashPixels(bmp.px);
    Rgb sampled;
    if (st.haveLastSample && hash == st.lastHash) {
        sampled = st.lastSample;
    } else {
        sampled = DominantColor(bmp.px);
        st.lastHash = hash;
        st.lastSample = sampled;
        st.haveLastSample = true;
        if (s.logDecisions) {
            std::vector<uint32_t> row(bmp.px.begin(), bmp.px.begin() + bmp.w);
            st.lastRowFirst = DominantColor(row);
            row.assign(bmp.px.end() - bmp.w, bmp.px.end());
            st.lastRowLast = DominantColor(row);
        }
    }

    Smooth(st, isl, sampled, targetChanged, s);
}

void SetPollTimer(bool enable, int intervalMs) {
    if (enable) {
        if (g_pollTimer && g_pollTimerInterval == intervalMs) return;
        if (g_pollTimer) KillTimer(nullptr, g_pollTimer);
        g_pollTimer = SetTimer(nullptr, 0, (UINT)intervalMs, nullptr);
        g_pollTimerInterval = intervalMs;
    } else if (g_pollTimer) {
        KillTimer(nullptr, g_pollTimer);
        g_pollTimer = 0;
        g_pollTimerInterval = 0;
    }
}

void Tick() {
    if (g_unloading) return;
    Settings s = GetSettings();
    auto islands = SnapshotIslands();

    {
        static bool emptyLogged = false;
        if (islands.empty() && !emptyLogged) {
            emptyLogged = true;
            size_t total;
            {
                std::lock_guard lock(g_islandsMutex);
                total = g_islands.size();
            }
            Wh_Log(L"[bleed] tick: %zu islands known, none mapped to a taskbar window yet", total);
        } else if (!islands.empty()) {
            emptyLogged = false;
        }
    }

    EnumCtx ctx{&s, {}};
    EnumWindows(EnumWindowsForemostProc, reinterpret_cast<LPARAM>(&ctx));

    if (s.logDecisions) {
        static HWND lastFg = nullptr;
        static HWND lastChosen = nullptr;
        HWND fg = GetForegroundWindow();
        HMONITOR fgMon = fg ? MonitorFromWindow(fg, MONITOR_DEFAULTTONULL) : nullptr;
        auto it = fgMon ? ctx.foremost.find(fgMon) : ctx.foremost.end();
        HWND chosen = it == ctx.foremost.end() ? nullptr : it->second;
        if (fg && (fg != lastFg || chosen != lastChosen)) {
            lastFg = fg;
            lastChosen = chosen;
            MONITORINFO mi{sizeof(mi)};
            if (fgMon) GetMonitorInfoW(fgMon, &mi);
            Wh_Log(L"[bleed] foreground %s | work=(%d,%d,%d,%d) | chosen=%s", DescribeWindow(fg).c_str(), mi.rcWork.left,
                   mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom,
                   chosen ? (chosen == fg ? L"foreground" : DescribeWindow(chosen).c_str()) : L"none");
        }
    }

    bool anyTarget = false;
    for (auto& isl : islands) ProcessIsland(isl, s, ctx.foremost, anyTarget);

    // Drop state for islands that disappeared.
    for (auto it = g_track.begin(); it != g_track.end();) {
        bool alive = std::any_of(islands.begin(), islands.end(), [&](auto& i) { return i.hwnd == it->first; });
        it = alive ? std::next(it) : g_track.erase(it);
    }

    SetPollTimer(anyTarget, s.pollIntervalMs);
}

void ScheduleDebouncedTick() {
    if (g_debounceTimer) KillTimer(nullptr, g_debounceTimer);
    g_debounceTimer = SetTimer(nullptr, 0, 60, nullptr);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event >= EVENT_OBJECT_CREATE) {  // object events: top-level windows only
        if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
        if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return;
    }
    ScheduleDebouncedTick();
}

void AddWinEventHook(DWORD minEvent, DWORD maxEvent) {
    HWINEVENTHOOK h = SetWinEventHook(minEvent, maxEvent, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (h) g_winEventHooks.push_back(h);
    else Wh_Log(L"[bleed] SetWinEventHook(%04X-%04X) failed", minEvent, maxEvent);
}

DWORD WINAPI TrackerThreadProc(LPVOID) {
    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        if (auto fn = (SetThreadDpiAwarenessContext_t)GetProcAddress(user32, "SetThreadDpiAwarenessContext")) {
            fn((HANDLE)-4);  // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2: physical pixels everywhere
        }
    }
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the queue
    SetEvent(g_trackerReady);

    AddWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND);
    AddWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND);
    AddWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND);
    AddWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH);
    AddWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE);  // DESTROY, SHOW, HIDE
    AddWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE);
    AddWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED);

    ScheduleDebouncedTick();

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER && !msg.hwnd) {
            if (msg.wParam == g_debounceTimer) {
                KillTimer(nullptr, g_debounceTimer);
                g_debounceTimer = 0;
                Tick();
            } else if (msg.wParam == g_pollTimer) {
                Tick();
            }
        } else if (msg.message == WM_TRACKER_TICK) {
            ScheduleDebouncedTick();
        } else if (msg.message == WM_TRACKER_SETTINGS) {
            SetPollTimer(false, 0);
            for (auto& kv : g_track) kv.second.haveLastSample = false;  // re-run the color pipeline
            Tick();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    for (HWINEVENTHOOK h : g_winEventHooks) UnhookWinEvent(h);
    g_winEventHooks.clear();
    if (g_debounceTimer) KillTimer(nullptr, g_debounceTimer);
    if (g_pollTimer) KillTimer(nullptr, g_pollTimer);
    g_debounceTimer = g_pollTimer = 0;
    g_track.clear();

    if (SUCCEEDED(coHr)) CoUninitialize();
    return 0;
}

bool StartTracker() {
    g_trackerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_trackerReady) return false;
    g_trackerThread = CreateThread(nullptr, 0, TrackerThreadProc, nullptr, 0, &g_trackerThreadId);
    if (!g_trackerThread) {
        CloseHandle(g_trackerReady);
        g_trackerReady = nullptr;
        return false;
    }
    WaitForSingleObject(g_trackerReady, 5000);
    return true;
}

void StopTracker() {
    if (g_trackerThread) {
        PostThreadMessageW(g_trackerThreadId, WM_QUIT, 0, 0);
        if (WaitForSingleObject(g_trackerThread, 5000) == WAIT_TIMEOUT) {
            Wh_Log(L"[bleed] tracker thread did not exit in time");
        }
        CloseHandle(g_trackerThread);
        g_trackerThread = nullptr;
        g_trackerThreadId = 0;
    }
    if (g_trackerReady) {
        CloseHandle(g_trackerReady);
        g_trackerReady = nullptr;
    }
}

// ============================================================================
// Painter: XAML panel per taskbar island (UI thread only)
// ============================================================================

wux::FrameworkElement FindDescendantByName(wux::DependencyObject const& root, std::wstring_view name, int depth) {
    if (!root || depth < 0) return nullptr;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>(); fe && fe.Name() == name) return fe;
        if (auto found = FindDescendantByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}

std::shared_ptr<Island> FindIslandByHwnd(HWND hwnd) {
    std::lock_guard lock(g_islandsMutex);
    for (auto& isl : g_islands)
        if (isl->hwnd == hwnd) return isl;
    return nullptr;
}

std::shared_ptr<Island> FindIslandByGridId(void* gridId) {
    std::lock_guard lock(g_islandsMutex);
    for (auto& isl : g_islands)
        if (isl->gridId == gridId) return isl;
    return nullptr;
}

std::shared_ptr<Island> FindIslandByFrameId(void* frameId) {
    std::lock_guard lock(g_islandsMutex);
    for (auto& isl : g_islands)
        if (isl->frameId == frameId) return isl;
    return nullptr;
}

// The panel fills the taskbar; margins are in screen orientation, measured from
// the taskbar edges (positive insets, negative extends).
void UpdateExtent(Island& isl, const Settings& s) {
    if (!isl.panel) return;
    auto& p = isl.panel;
    p.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    p.VerticalAlignment(wux::VerticalAlignment::Stretch);
    p.Margin(wux::Thickness{(double)s.marginLeft, (double)s.marginTop, (double)s.marginRight, (double)s.marginBottom});
    // CornerRadius{TopLeft, TopRight, BottomRight, BottomLeft}, screen orientation.
    p.CornerRadius(wux::CornerRadius{(double)s.radiusTopLeft, (double)s.radiusTopRight, (double)s.radiusBottomRight,
                                     (double)s.radiusBottomLeft});
}

// No animations: the panel's brush and opacity are plain local values. (A
// storyboard's held value would override local sets, so none are ever used.)
void SetPanelColor(Island& isl, wu::Color c) {
    if (isl.brush) isl.brush.Color(c);
}

void SetPanelVisible(Island& isl, bool visible) {
    if (isl.panel) isl.panel.Opacity(visible ? 1.0 : 0.0);
}

// ---- Adaptive theme ---------------------------------------------------------
// RequestedTheme on the island's root content re-resolves every theme-resource
// brush under it (labels, monochrome icons, hover highlights, the tray) for
// this island only, i.e. for this monitor's taskbar. Reset to Default when the
// panel hides.

constexpr int ThemeNone = 0, ThemeLight = 1, ThemeDark = 2;

int PerceivedBrightness(Rgb c) {
    return (int)std::lround(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
}

wux::FrameworkElement ThemeRootOf(Island& isl) {
    if (auto r = isl.themeRoot.get()) return r;
    try {
        if (isl.panel) {
            if (auto xr = isl.panel.XamlRoot()) {
                if (auto root = xr.Content().try_as<wux::FrameworkElement>()) {
                    isl.themeRoot = root;
                    return root;
                }
            }
        }
    } catch (...) {
    }
    return nullptr;
}

// Perceived brightness above which a bled color counts as light (black text
// starts out-contrasting white around here), with an 8-level hysteresis band so
// a color sitting on the threshold can't flip the theme back and forth.
constexpr int kLightThreshold = 120;
constexpr int kThemeHysteresis = 8;

void ApplyTheme(Island& isl, bool active, Rgb color, const Settings& s) {
    int want = ThemeNone;
    int lum = 0;
    if (active) {
        lum = PerceivedBrightness(color);
        bool light = isl.forcedTheme == ThemeLight  ? lum >= kLightThreshold - kThemeHysteresis
                     : isl.forcedTheme == ThemeDark ? lum > kLightThreshold + kThemeHysteresis
                                                    : lum > kLightThreshold;
        want = light ? ThemeLight : ThemeDark;
    }
    if (want == isl.forcedTheme) return;

    auto root = ThemeRootOf(isl);
    if (!root) return;
    // Forced unconditionally while the panel shows (identical to Default when it
    // happens to match the system theme, and immune to the system theme changing
    // underneath); Default again when the panel hides.
    root.RequestedTheme(want == ThemeLight  ? wux::ElementTheme::Light
                        : want == ThemeDark ? wux::ElementTheme::Dark
                                            : wux::ElementTheme::Default);
    if (s.logDecisions) {
        Wh_Log(L"[bleed] island %p: theme %s (brightness %d)", isl.hwnd,
               want == ThemeLight ? L"light" : want == ThemeDark ? L"dark" : L"restored", lum);
    }
    isl.forcedTheme = want;
}

void SetStrokeHidden(Island& isl, bool hide) {
    if (isl.strokeHidden == hide) return;
    auto stroke = isl.stroke.get();
    if (!stroke) {
        isl.strokeHidden = false;
        return;
    }
    stroke.Visibility(hide ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    isl.strokeHidden = hide;
}

void ApplyIslandUpdateOnUiThread(HWND hwnd, bool active, Rgb color) try {
    if (g_unloading) return;
    auto isl = FindIslandByHwnd(hwnd);
    if (!isl || !isl->panel) return;

    if (!isl->panel.Parent()) {
        if (!isl->unparentedLogged) {
            Wh_Log(L"[bleed] island %p: panel lost its parent, waiting for rediscovery", hwnd);
            isl->unparentedLogged = true;
        }
        return;
    }
    isl->unparentedLogged = false;

    Settings s = GetSettings();
    if (!isl->shown) UpdateExtent(*isl, s);

    if (active) {
        if (!isl->shown || isl->color != color) SetPanelColor(*isl, ToColor(color));
        if (!isl->shown) SetPanelVisible(*isl, true);
        isl->color = color;
        SetStrokeHidden(*isl, true);
        ApplyTheme(*isl, true, color, s);
    } else if (isl->shown) {
        SetPanelVisible(*isl, false);
        SetStrokeHidden(*isl, false);
        ApplyTheme(*isl, false, color, s);
    }
    isl->shown = active;
} catch (winrt::hresult_error const& e) {
    Wh_Log(L"[bleed] ApplyIslandUpdate failed: %08X", e.code().value);
} catch (...) {
    Wh_Log(L"[bleed] ApplyIslandUpdate failed");
}

void TeardownIslandOnUiThread(Island& isl) try {
    if (isl.forcedTheme != ThemeNone) {
        if (auto root = ThemeRootOf(isl)) root.RequestedTheme(wux::ElementTheme::Default);
        isl.forcedTheme = ThemeNone;
    }
    SetStrokeHidden(isl, false);
    if (isl.panel) {
        if (auto parent = isl.panel.Parent().try_as<wuxc::Panel>()) {
            uint32_t idx = 0;
            if (parent.Children().IndexOf(isl.panel, idx)) parent.Children().RemoveAt(idx);
        }
    }
    isl.panel = nullptr;
    isl.brush = nullptr;
    isl.shown = false;
} catch (winrt::hresult_error const& e) {
    Wh_Log(L"[bleed] teardown failed: %08X", e.code().value);
} catch (...) {
    Wh_Log(L"[bleed] teardown failed");
}

// ----------------------------------------------------------------------------
// Island -> taskbar window mapping
// ----------------------------------------------------------------------------

HWND BridgeHwndForTray(HWND tray) {
    if (HWND direct = FindWindowExW(tray, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr)) {
        return direct;
    }
    // Not a direct child on this build: search all descendants.
    HWND found = nullptr;
    EnumChildWindows(
        tray,
        [](HWND h, LPARAM lp) -> BOOL {
            WCHAR cls[128];
            if (GetClassNameW(h, cls, ARRAYSIZE(cls)) &&
                wcscmp(cls, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) {
                *reinterpret_cast<HWND*>(lp) = h;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&found));
    return found;
}

std::vector<HWND> AllTaskbarBridges() {
    std::vector<HWND> out;
    if (HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        if (HWND b = BridgeHwndForTray(primary)) out.push_back(b);
    }
    for (HWND sec = FindWindowExW(nullptr, nullptr, L"Shell_SecondaryTrayWnd", nullptr); sec;
         sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd", nullptr)) {
        if (HWND b = BridgeHwndForTray(sec)) out.push_back(b);
    }
    return out;
}

// Bridge windows already owned by other islands.
std::vector<HWND> ClaimedBridges() {
    std::vector<HWND> out;
    std::lock_guard lock(g_islandsMutex);
    for (auto& isl : g_islands)
        if (isl->hwnd) out.push_back(isl->hwnd);
    return out;
}

// Screen rectangle of an element, via its automation peer (peers report bounds
// in screen coordinates - that is how UI Automation locates taskbar buttons).
// Not every element has a peer, so the first descendant that does is used.
bool ScreenRectOf(wux::DependencyObject const& start, RECT& out) {
    try {
        std::vector<wux::DependencyObject> queue{start};
        for (size_t i = 0; i < queue.size() && i < 64; i++) {
            auto node = queue[i];
            if (auto fe = node.try_as<wux::UIElement>()) {
                if (auto peer = wuxap::FrameworkElementAutomationPeer::CreatePeerForElement(fe)) {
                    auto r = peer.GetBoundingRectangle();
                    if (r.Width > 0 && r.Height > 0) {
                        out = RECT{(LONG)std::lround(r.X), (LONG)std::lround(r.Y), (LONG)std::lround(r.X + r.Width),
                                   (LONG)std::lround(r.Y + r.Height)};
                        return true;
                    }
                }
            }
            int n = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int c = 0; c < n && queue.size() < 64; c++) queue.push_back(wuxm::VisualTreeHelper::GetChild(node, c));
        }
    } catch (...) {
    }
    return false;
}

// Map the island hosting `fe` to its DesktopWindowContentBridge window.
//   screen      - a peer's screen rect lies inside exactly one bridge window (normal case)
//   elimination - every other bridge is already claimed by another island
//   thread      - the only unclaimed bridge owned by the current UI thread
//   size        - the only unclaimed bridge whose client size matches the island
// nullptr = still ambiguous; retried on a later layout event.
HWND ResolveIslandHwnd(wux::FrameworkElement const& fe, const wchar_t** how) {
    *how = L"UNRESOLVED";
    std::vector<HWND> all = AllTaskbarBridges();
    if (all.empty()) {
        Wh_Log(L"[bleed] no DesktopWindowContentBridge window found under any taskbar window");
        return nullptr;
    }
    std::vector<HWND> claimed = ClaimedBridges();
    auto isClaimed = [&](HWND h) { return std::find(claimed.begin(), claimed.end(), h) != claimed.end(); };

    RECT elemRect;
    if (ScreenRectOf(fe, elemRect)) {
        POINT center{(elemRect.left + elemRect.right) / 2, (elemRect.top + elemRect.bottom) / 2};
        std::vector<HWND> hits;
        for (HWND b : all) {
            RECT rc;
            if (GetWindowRect(b, &rc) && PtInRect(&rc, center)) hits.push_back(b);
        }
        if (hits.size() == 1 && !isClaimed(hits[0])) {
            *how = L"screen";
            return hits[0];
        }
    }

    std::vector<HWND> unclaimed;
    for (HWND b : all)
        if (!isClaimed(b)) unclaimed.push_back(b);
    if (unclaimed.size() == 1) {
        *how = claimed.empty() ? L"single" : L"elimination";
        return unclaimed[0];
    }

    DWORD tid = GetCurrentThreadId();
    std::vector<HWND> byThread;
    for (HWND b : unclaimed)
        if (GetWindowThreadProcessId(b, nullptr) == tid) byThread.push_back(b);
    if (byThread.size() == 1) {
        *how = L"thread";
        return byThread[0];
    }
    std::vector<HWND>& pool = byThread.empty() ? unclaimed : byThread;

    auto xr = fe.XamlRoot();
    if (!xr) return nullptr;
    auto size = xr.Size();
    double scale = xr.RasterizationScale();
    int pw = (int)std::lround(size.Width * scale), ph = (int)std::lround(size.Height * scale);
    std::vector<HWND> bySize;
    for (HWND b : pool) {
        RECT rc;
        if (!GetClientRect(b, &rc)) continue;
        if (std::abs((rc.right - rc.left) - pw) <= 1 && std::abs((rc.bottom - rc.top) - ph) <= 1) bySize.push_back(b);
    }
    if (bySize.size() == 1) {
        *how = L"size";
        return bySize[0];
    }
    static ULONGLONG lastAmbiguousLog = 0;
    ULONGLONG now = GetTickCount64();
    if (now - lastAmbiguousLog > 5000) {
        lastAmbiguousLog = now;
        Wh_Log(L"[bleed] island mapping ambiguous: %zu unclaimed bridges, %zu on thread %u, %zu match %dx%d; will retry",
               unclaimed.size(), byThread.size(), tid, bySize.size(), pw, ph);
    }
    return nullptr;
}

// ----------------------------------------------------------------------------
// Island creation
// ----------------------------------------------------------------------------

void OnRepeaterDiscovered(wux::FrameworkElement const& rep) {
    Settings s = GetSettings();

    // Nearest Grid ancestor hosts the panel; `anchor` is that grid's direct child on the path.
    wuxc::Grid grid{nullptr};
    wux::UIElement anchor{nullptr};
    wux::DependencyObject frame{nullptr};
    wux::DependencyObject prev = rep;
    wux::DependencyObject cur = wuxm::VisualTreeHelper::GetParent(rep);
    for (int hop = 0; cur && hop < 8; hop++) {
        if (!grid) {
            if (auto g = cur.try_as<wuxc::Grid>()) {
                grid = g;
                anchor = prev.try_as<wux::UIElement>();
            }
        }
        if (winrt::get_class_name(cur) == L"Taskbar.TaskbarFrame") {
            frame = cur;
            break;
        }
        prev = cur;
        cur = wuxm::VisualTreeHelper::GetParent(cur);
    }
    if (!frame) {
        Wh_Log(L"[bleed] TaskbarFrameRepeater without a Taskbar.TaskbarFrame ancestor, ignored");
        return;
    }
    if (!grid || !anchor) {
        Wh_Log(L"[bleed] no Grid ancestor for TaskbarFrameRepeater");
        return;
    }

    void* gridId = IdentityOf(grid);
    void* frameId = IdentityOf(frame);

    std::shared_ptr<Island> isl = FindIslandByGridId(gridId);
    if (isl && isl->panel && isl->panel.Parent()) {
        // Already set up; only a late hwnd mapping may be outstanding.
        if (!isl->hwnd) {
            isl->lastMapAttemptTick = GetTickCount64();
            const wchar_t* how = nullptr;
            if (HWND hwnd = ResolveIslandHwnd(rep, &how)) {
                std::lock_guard lock(g_islandsMutex);
                isl->hwnd = hwnd;
                Wh_Log(L"[bleed] island grid=%p mapped late to hwnd=%p (%s)", gridId, hwnd, how);
                RequestTick();
            }
        }
        return;
    }
    if (isl) {
        // Stale island (panel lost its parent, e.g. the frame was re-templated).
        TeardownIslandOnUiThread(*isl);
        {
            std::lock_guard lock(g_islandsMutex);
            g_islands.erase(std::remove(g_islands.begin(), g_islands.end(), isl), g_islands.end());
        }
        isl = nullptr;
    }

    const wchar_t* how = nullptr;
    HWND hwnd = ResolveIslandHwnd(rep, &how);
    Wh_Log(L"[bleed] host grid=%p (%s) frame=%p hwnd=%p (%s)", gridId, grid.Name().c_str(), frameId, hwnd, how);

    isl = std::make_shared<Island>();
    isl->hwnd = hwnd;
    isl->lastMapAttemptTick = GetTickCount64();
    isl->gridId = gridId;
    isl->frameId = frameId;
    isl->dispatcher = rep.Dispatcher();
    isl->brush = wuxm::SolidColorBrush(wu::Color{255, 0, 0, 0});

    wuxc::Border panel;
    panel.Name(L"TaskbarBleedPanel");
    panel.IsHitTestVisible(false);
    panel.Background(isl->brush);
    panel.Opacity(0.0);
    isl->panel = panel;

    auto children = grid.Children();
    uint32_t idx = 0;
    if (children.IndexOf(anchor, idx)) {
        children.InsertAt(idx, panel);
    } else {
        bool inserted = false;
        for (uint32_t i = 0; i < children.Size(); i++) {
            if (winrt::get_class_name(children.GetAt(i)) == L"Taskbar.TaskbarBackground") {
                children.InsertAt(i + 1, panel);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            Wh_Log(L"[bleed] could not find an insertion point in the taskbar grid");
            return;
        }
    }

    isl->stroke = FindDescendantByName(grid, L"BackgroundStroke", 6);
    UpdateExtent(*isl, s);

    {
        std::lock_guard lock(g_islandsMutex);
        g_islands.push_back(isl);
    }

    HMONITOR hmon = hwnd ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) : nullptr;
    Wh_Log(L"[bleed] island created: grid=%p hwnd=%p monitor=%p stroke=%d mapping=%s", gridId, hwnd, hmon,
           isl->stroke.get() ? 1 : 0, how);
    RequestTick();
}

void RefreshIslandsOnThisThread() {
    std::vector<std::shared_ptr<Island>> mine;
    {
        std::lock_guard lock(g_islandsMutex);
        for (auto& isl : g_islands)
            if (isl->dispatcher && isl->dispatcher.HasThreadAccess()) mine.push_back(isl);
    }
    Settings s = GetSettings();
    for (auto& isl : mine) {
        try {
            UpdateExtent(*isl, s);
            ApplyTheme(*isl, isl->shown, isl->color, s);
        } catch (...) {
        }
    }
}

void CleanupIslandsOnThisThread() {
    std::vector<std::shared_ptr<Island>> mine;
    {
        std::lock_guard lock(g_islandsMutex);
        for (auto it = g_islands.begin(); it != g_islands.end();) {
            if ((*it)->dispatcher && (*it)->dispatcher.HasThreadAccess()) {
                mine.push_back(*it);
                it = g_islands.erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto& isl : mine) TeardownIslandOnUiThread(*isl);
    mine.clear();  // destroy XAML objects here, on their thread
}

// ============================================================================
// Taskbar.View.dll hooks (island discovery without XAML diagnostics)
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded{false};

// winrt::implements objects used by the taskbar: the IUnknown of the projected
// element sits three pointers past the implementation `this` (same resolver as
// the catalog taskbar mods).
wux::FrameworkElement ElementFromImplThis(void* pThis) {
    try {
        void* unkPtr = (void**)pThis + 3;
        wf::IUnknown unk;
        winrt::copy_from_abi(unk, unkPtr);
        return unk.try_as<wux::FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

wux::DependencyObject FindTaskbarFrameAncestor(wux::DependencyObject const& start) {
    wux::DependencyObject cur = start;
    for (int hop = 0; cur && hop < 16; hop++) {
        if (winrt::get_class_name(cur) == L"Taskbar.TaskbarFrame") return cur;
        cur = wuxm::VisualTreeHelper::GetParent(cur);
    }
    return nullptr;
}

std::mutex g_pendingMutex;
std::vector<void*> g_pendingFrames;  // frames with a discovery post in flight

// Called on the island's UI thread from inside a hook. Does a cheap dedupe and
// defers the actual tree work to the dispatcher queue, so nothing is inserted
// while the taskbar is in the middle of a layout pass.
void DiscoverFromFrame(wux::DependencyObject const& frame) {
    if (g_unloading || !frame) return;
    void* frameId = IdentityOf(frame);
    if (auto isl = FindIslandByFrameId(frameId)) {
        bool parented = isl->panel && isl->panel.Parent();
        if (parented && isl->hwnd) return;  // fully set up
        if (parented && GetTickCount64() - isl->lastMapAttemptTick < 2000) return;  // mapping retry throttled
    }
    auto fe = frame.try_as<wux::FrameworkElement>();
    if (!fe) return;
    {
        std::lock_guard lock(g_pendingMutex);
        if (std::find(g_pendingFrames.begin(), g_pendingFrames.end(), frameId) != g_pendingFrames.end()) return;
        g_pendingFrames.push_back(frameId);
    }
    auto clearPending = [frameId]() {
        std::lock_guard lock(g_pendingMutex);
        g_pendingFrames.erase(std::remove(g_pendingFrames.begin(), g_pendingFrames.end(), frameId), g_pendingFrames.end());
    };
    try {
        fe.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal, [fe, clearPending]() {
            clearPending();
            if (g_unloading) return;
            try {
                auto rep = FindDescendantByName(fe, L"TaskbarFrameRepeater", 10);
                if (!rep) {
                    static bool logged = false;
                    if (!logged) {
                        logged = true;
                        Wh_Log(L"[bleed] TaskbarFrameRepeater not found under Taskbar.TaskbarFrame");
                    }
                    return;
                }
                OnRepeaterDiscovered(rep);
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"[bleed] discovery failed: %08X %s", e.code().value, e.message().c_str());
            } catch (...) {
                Wh_Log(L"[bleed] discovery failed");
            }
        });
    } catch (...) {
        clearPending();
    }
}

using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t = void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;
void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);
    if (g_unloading) return;
    static std::atomic<int> logged{0};
    auto frame = ElementFromImplThis(pThis);
    if (logged < 3) {
        logged++;
        Wh_Log(L"[bleed] frame hook fired (thread %u): element=%s", GetCurrentThreadId(),
               frame ? winrt::get_class_name(frame).c_str() : L"<unresolved>");
    }
    if (frame) DiscoverFromFrame(frame);
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_unloading) return;
    static std::atomic<int> logged{0};
    auto btn = ElementFromImplThis(pThis);
    auto frame = btn ? FindTaskbarFrameAncestor(btn) : nullptr;
    if (logged < 3) {
        logged++;
        Wh_Log(L"[bleed] button hook fired (thread %u): element=%s frame=%d", GetCurrentThreadId(),
               btn ? winrt::get_class_name(btn).c_str() : L"<unresolved>", frame ? 1 : 0);
    }
    if (frame) DiscoverFromFrame(frame);
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))"},
            &TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
            TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"[bleed] HookSymbols failed");
        return false;
    }
    if (!TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original && !TaskListButton_UpdateVisualStates_Original) {
        Wh_Log(L"[bleed] no discovery symbol resolved");
        return false;
    }
    Wh_Log(L"[bleed] Taskbar.View.dll hooks: frame=%d button=%d",
           TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original ? 1 : 0,
           TaskListButton_UpdateVisualStates_Original ? 1 : 0);
    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandleW(L"ExplorerExtensions.dll");
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR name) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"[bleed] loaded %s", name ? name : L"?");
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    return module;
}

// ============================================================================
// Mod lifecycle
// ============================================================================

// One RunAsync per distinct island dispatcher.
std::vector<wuc::CoreDispatcher> IslandDispatchers() {
    std::vector<wuc::CoreDispatcher> out;
    std::lock_guard lock(g_islandsMutex);
    for (auto& isl : g_islands) {
        if (!isl->dispatcher) continue;
        bool dup = std::any_of(out.begin(), out.end(),
                               [&](auto& d) { return winrt::get_abi(d) == winrt::get_abi(isl->dispatcher); });
        if (!dup) out.push_back(isl->dispatcher);
    }
    return out;
}

BOOL Wh_ModInit() {
    Wh_Log(L"[bleed] init");
    LoadSettings();

    if (HMODULE taskbarView = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarView)) return FALSE;
    } else {
        Wh_Log(L"[bleed] Taskbar.View.dll not loaded yet, hooking LoadLibraryExW");
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW = kernelBase ? (decltype(&LoadLibraryExW))GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
        if (!pLoadLibraryExW) {
            Wh_Log(L"[bleed] kernelbase!LoadLibraryExW not found");
            return FALSE;
        }
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }

    if (!StartTracker()) {
        Wh_Log(L"[bleed] failed to start tracker thread");
        return FALSE;
    }
    Wh_Log(L"[bleed] tracker running (thread %u)", g_trackerThreadId);
    return TRUE;
}

// Discovery rides on taskbar layout events, which a mid-session enable doesn't
// get by itself. Adding a taskbar button and removing it again makes every
// taskbar lay out its buttons once (TaskbarFrame::OnTaskbarLayoutChildBoundsChanged).
// The button belongs to a 1x1, fully transparent, non-activating window placed
// on each monitor, so it is never seen; it lives for a few hundred milliseconds.
HANDLE g_nudgeThread = nullptr;

DWORD WINAPI NudgeThreadProc(LPVOID) {
    struct Ctx {
        std::vector<HWND> windows;
    } ctx;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hmon, HDC, LPRECT, LPARAM lp) -> BOOL {
            MONITORINFO mi{sizeof(mi)};
            if (!GetMonitorInfoW(hmon, &mi)) return TRUE;
            HWND w = CreateWindowExW(WS_EX_APPWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT, L"Static", L"",
                                     WS_POPUP, mi.rcWork.left + 8, mi.rcWork.top + 8, 1, 1, nullptr, nullptr,
                                     GetModuleHandleW(nullptr), nullptr);
            if (!w) return TRUE;
            SetLayeredWindowAttributes(w, 0, 0, LWA_ALPHA);
            ShowWindow(w, SW_SHOWNOACTIVATE);
            reinterpret_cast<Ctx*>(lp)->windows.push_back(w);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
    Wh_Log(L"[bleed] discovery nudge: %zu window(s)", ctx.windows.size());

    // Pump messages so the shell's queries to these windows don't time out.
    ULONGLONG until = GetTickCount64() + 400;
    MSG msg;
    while (!g_unloading && GetTickCount64() < until) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        MsgWaitForMultipleObjects(0, nullptr, FALSE, 50, QS_ALLINPUT);
    }
    for (HWND w : ctx.windows) DestroyWindow(w);
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

void StartDiscoveryNudge() {
    if (g_nudgeThread) return;
    g_nudgeThread = CreateThread(nullptr, 0, NudgeThreadProc, nullptr, 0, nullptr);
    if (!g_nudgeThread) Wh_Log(L"[bleed] could not start the discovery nudge");
}

void Wh_ModAfterInit() {
    bool taskbarAlreadyUp = g_taskbarViewDllLoaded.load();

    // Taskbar.View.dll may have loaded between Wh_ModInit and the hook taking effect.
    if (!taskbarAlreadyUp) {
        if (HMODULE taskbarView = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"[bleed] got Taskbar.View.dll after init");
                if (HookTaskbarViewDllSymbols(taskbarView)) Wh_ApplyHookOperations();
                taskbarAlreadyUp = true;
            }
        }
    }

    // Mid-session enable: the taskbar is already laid out, so provoke one layout pass.
    if (taskbarAlreadyUp) StartDiscoveryNudge();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[bleed] before uninit");
    g_unloading = true;

    if (g_nudgeThread) {
        WaitForSingleObject(g_nudgeThread, 2000);
        CloseHandle(g_nudgeThread);
        g_nudgeThread = nullptr;
    }

    // 1. No more tracker posts after this returns.
    StopTracker();
    {
        std::lock_guard lock(g_pendingMutex);
        g_pendingFrames.clear();
    }

    // 2. Remove panels on their UI threads and wait. Hooks still fire until
    //    Windhawk removes them, but bail on g_unloading before touching anything.
    auto dispatchers = IslandDispatchers();
    if (!dispatchers.empty()) {
        HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        auto pending = std::make_shared<std::atomic<int>>((int)dispatchers.size());
        for (auto& d : dispatchers) {
            bool posted = false;
            try {
                d.RunAsync(wuc::CoreDispatcherPriority::High, [pending, done]() {
                    CleanupIslandsOnThisThread();
                    if (--(*pending) == 0 && done) SetEvent(done);
                });
                posted = true;
            } catch (...) {
            }
            if (!posted && --(*pending) == 0 && done) SetEvent(done);
        }
        if (done) {
            if (WaitForSingleObject(done, 3000) == WAIT_TIMEOUT) Wh_Log(L"[bleed] cleanup wait timed out");
            CloseHandle(done);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"[bleed] uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[bleed] settings changed");
    LoadSettings();

    if (g_trackerThreadId) PostThreadMessageW(g_trackerThreadId, WM_TRACKER_SETTINGS, 0, 0);

    for (auto& d : IslandDispatchers()) {
        try {
            d.RunAsync(wuc::CoreDispatcherPriority::High, []() {
                if (!g_unloading) RefreshIslandsOnThisThread();
            });
        } catch (...) {
        }
    }
}
