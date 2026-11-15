// ==WindhawkMod==
// @id              desktop-snowfall
// @name            Desktop Snowfall
// @description     Snow falls over your desktop and settles on top of windows and the taskbar
// @version         1.1
// @author          mikybuilds
// @github          https://github.com/mikybuilds
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ld3d11 -ldcomp -ldwmapi -luser32 -lole32 -loleaut32 -lshell32 -lwtsapi32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Snowfall

Snow falls over your desktop. Flakes land on the top edge of your windows and on
the taskbar and slowly build up snow drifts of different heights.

![Desktop Snowfall demo](https://raw.githubusercontent.com/mikybuilds/desktop-snowfall/main/demo.gif)

* The snow follows the window. Toss a window up and the snow flies up in loose
  clumps and lands back with a little bounce. Pull it down or shrink it from
  the top and the snow hangs in the air for a moment before it falls back onto
  the window.
* Make a window narrower and the snow on the cut-off part falls down.
* Shake a window and its snow flies off. Close or minimize it and the snow
  drops in lumps that burst into powder where they land.
* Optionally, the snow slides off windows moved sideways: it stays where it is,
  and the part the window slides out from under falls down.
* Sweep the snow off with the mouse cursor - it falls down to the windows below
  or to the taskbar.
* The wind blows powder off the drifts, and now and then a lump slides off a
  window corner.
* The snow can fall over all windows, or behind them, on the desktop: then the
  falling snow shows only where the desktop is uncovered, and the snow still
  settles on top of the windows and the taskbar.
* Optionally, the lying snow slowly melts, and the snow can be kept out of
  screenshots and screen sharing.
* Works with the default taskbar, an auto-hidden taskbar (the snow rides along
  when it slides in and out) and custom taskbar styles (for example, floating
  "dock" themes of the Windows 11 Taskbar Styler mod): the snow lies on the
  visible taskbar parts.
* Snow piles up along the bottom of the screen where the taskbar doesn't cover
  it (an auto-hidden or see-through taskbar).
* With several monitors, each monitor gets its own snow, and the snow is sized
  for each monitor's display scale.

The snow is drawn on the GPU on a transparent, click-through overlay, so it
never gets in the way of mouse input. Menus, tooltips and the shell's flyouts
stay in front of the snow. The snow is hidden automatically on a monitor where
a fullscreen app (a game or a video) is in the foreground. The mod runs in its
own process, separately from Explorer.

The snow is light on power: calm snowfall is drawn at about 30 frames per
second, and the full frame rate is used only while something moves (a window,
the cursor in the snow, falling lumps). With the snowfall off (intensity 0),
snow that lies still isn't redrawn at all. On battery the frame rate is capped
at 60, and lower in energy saver mode. The snow pauses while the screen is
locked or turned off.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- intensity: 100
  $name: Snowfall intensity (%)
  $description: >-
    How heavily it snows. 100 is a calm snowfall, 300 is a blizzard, 0 stops
    the snowfall (0-600)
- speed: 100
  $name: Fall speed (%)
  $description: 100 is the default speed (10-500)
- wind: 15
  $name: Wind (%)
  $description: Horizontal drift. Negative values blow to the left (-100 to 100)
- size: 100
  $name: Flake size (%)
  $description: 100 is the default size (20-400)
- snowLayer: top
  $name: Where the snow falls
  $description: >-
    "Behind windows" keeps the falling snow on the desktop, behind your
    windows. The snow still settles on top of the windows and the taskbar.
  $options:
  - top: Over all windows
  - behind: Behind windows, on the desktop
- capHeight: 8
  $name: Snow cap height (pixels)
  $description: >-
    Maximum height of the snow that settles on windows and the taskbar, at 100%
    display scale. 0 disables the caps (0-40)
- groundSnow: true
  $name: Snow at the bottom of the screen
  $description: >-
    The snow piles up along the bottom of the screen where nothing covers it:
    under an auto-hidden or see-through taskbar, or on a monitor without a
    taskbar at the bottom
- melt: false
  $name: Snow slowly melts
  $description: >-
    The lying snow slowly melts away, so the drifts come and go, and they
    disappear when the snowfall stops
- sweepWithCursor: true
  $name: Sweep snow with the cursor
  $description: Moving the mouse cursor through a snow cap pushes the snow off
- shakeOff: true
  $name: Shake windows to throw the snow off
  $description: >-
    Quickly moving a window from side to side throws its snow off. Has no
    effect while "Snow slides off moved windows" is on
- slideOff: false
  $name: Snow slides off moved windows
  $description: >-
    When a window is moved sideways, the snow stays where it is and the window
    slides out from under it: the part that ends up past the window edge falls
    down. When off, the snow moves along with the window.
- taskbarSnow: auto
  $name: Snow on the taskbar
  $description: >-
    Where the snow lies on the taskbar. "Automatic" uses the visible taskbar
    parts when a custom style makes the taskbar narrower than the screen.
  $options:
  - auto: Automatic
  - whole: Whole taskbar
  - parts: Visible parts only (buttons and tray)
  - off: No snow on the taskbar
- maxFps: 60
  $name: Frame rate limit
  $description: >-
    The frame rate used while something moves. Calm snowfall is drawn at about
    30 frames per second. Lower values use less GPU and CPU (15-240)
- allMonitors: false
  $name: All monitors
  $description: >-
    Snow on every monitor instead of only the primary one. Each monitor has its
    own snow, and a fullscreen app hides only the snow on its own monitor
- pauseInFullscreen: true
  $name: Hide in fullscreen apps
  $description: Hide the snow while a fullscreen app is in the foreground
- hideFromCapture: false
  $name: Hide from screenshots and screen sharing
  $description: >-
    Screenshots, screen recordings and screen sharing show the desktop without
    the snow. Works with "Over all windows", on Windows 10 version 2004 and
    later (on earlier versions the setting does nothing)
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <shellscalingapi.h>
#include <uiautomation.h>
#include <wtsapi32.h>
#include <wrl/client.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;

// Sizes and speeds are in pixels at 100% display scale, and are scaled with
// the monitor's DPI.
constexpr float kGravity = 2600.0f;      // px/s^2, for snow caps
constexpr float kHangSeconds = 0.12f;    // snow hangs in the air this long
constexpr float kTossFactor = 0.5f;      // share of the window speed snow keeps
constexpr float kMinTossSpeed = 250.0f;  // px/s
constexpr float kMaxTossSpeed = 550.0f;  // px/s
constexpr float kBounceSpeed = 350.0f;   // snow landing faster bounces
constexpr float kChunkMaxSpeed = 700.0f;
constexpr size_t kMaxChunks = 4000;
constexpr size_t kMaxPuffs = 800;
constexpr int kSweepRadius = 7;
constexpr int kChunkColumns = 3;
constexpr float kCapEdgeTaper = 8.0f;
constexpr float kSnowKernel[] = {0.1f, 0.25f, 0.5f, 0.8f, 1.0f,
                                 0.8f, 0.5f,  0.25f, 0.1f};
constexpr float kSnowKernelSum = 4.3f;

enum class TaskbarSnow { Auto, Whole, Parts, Off };

struct {
    std::atomic<int> intensity{100};
    std::atomic<bool> slideOff{false};
    std::atomic<int> speed{100};
    std::atomic<int> wind{15};
    std::atomic<int> size{100};
    std::atomic<int> capHeight{8};
    std::atomic<bool> sweepWithCursor{true};
    std::atomic<TaskbarSnow> taskbarSnow{TaskbarSnow::Auto};
    std::atomic<int> maxFps{60};
    std::atomic<bool> allMonitors{false};
    std::atomic<bool> pauseInFullscreen{true};
    std::atomic<bool> behindWindows{false};
    std::atomic<bool> melt{false};
    std::atomic<bool> shakeOff{true};
    std::atomic<bool> hideFromCapture{false};
    std::atomic<bool> groundSnow{true};
} g_settings;

HANDLE g_stopEvent;
HANDLE g_snowThread;
HANDLE g_taskbarThread;
std::atomic<int> g_visibleOverlays{0};  // the taskbar is looked at only if > 0
std::atomic<bool> g_locked{false};
std::atomic<bool> g_displayOff{false};

// Each monitor's snow runs on a thread of its own. Set there by the window
// event hook, read by the overlay on the same thread.
struct WakeFlags {
    bool windows = false;  // a window moved, appeared or disappeared
    bool cursor = false;   // the mouse cursor moved
    bool restack = false;  // the overlay may need to go back in its place
};
thread_local WakeFlags g_wake;

void LoadSettings() {
    g_settings.intensity = std::clamp(Wh_GetIntSetting(L"intensity"), 0, 600);
    g_settings.slideOff = Wh_GetIntSetting(L"slideOff") != 0;
    g_settings.speed = std::clamp(Wh_GetIntSetting(L"speed"), 10, 500);
    g_settings.wind = std::clamp(Wh_GetIntSetting(L"wind"), -100, 100);
    g_settings.size = std::clamp(Wh_GetIntSetting(L"size"), 20, 400);
    g_settings.capHeight = std::clamp(Wh_GetIntSetting(L"capHeight"), 0, 40);
    g_settings.sweepWithCursor = Wh_GetIntSetting(L"sweepWithCursor") != 0;
    g_settings.maxFps = std::clamp(Wh_GetIntSetting(L"maxFps"), 15, 240);
    g_settings.allMonitors = Wh_GetIntSetting(L"allMonitors") != 0;
    g_settings.pauseInFullscreen = Wh_GetIntSetting(L"pauseInFullscreen") != 0;
    g_settings.melt = Wh_GetIntSetting(L"melt") != 0;
    g_settings.shakeOff = Wh_GetIntSetting(L"shakeOff") != 0;
    g_settings.hideFromCapture = Wh_GetIntSetting(L"hideFromCapture") != 0;
    g_settings.groundSnow = Wh_GetIntSetting(L"groundSnow") != 0;

    WindhawkUtils::StringSetting snowLayer =
        WindhawkUtils::StringSetting::make(L"snowLayer");
    g_settings.behindWindows = !wcscmp(snowLayer, L"behind");

    WindhawkUtils::StringSetting taskbarSnow =
        WindhawkUtils::StringSetting::make(L"taskbarSnow");
    TaskbarSnow mode = TaskbarSnow::Auto;
    if (!wcscmp(taskbarSnow, L"whole")) {
        mode = TaskbarSnow::Whole;
    } else if (!wcscmp(taskbarSnow, L"parts")) {
        mode = TaskbarSnow::Parts;
    } else if (!wcscmp(taskbarSnow, L"off")) {
        mode = TaskbarSnow::Off;
    }
    g_settings.taskbarSnow = mode;
}

static bool IsTaskbarClass(const wchar_t* cls) {
    return !wcscmp(cls, L"Shell_TrayWnd") ||
           !wcscmp(cls, L"Shell_SecondaryTrayWnd");
}

static bool IsShellWindowClass(const wchar_t* cls) {
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
}

static bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked;
}

// Windows that are never something the snow lies on or is hidden by: shadows
// and invisible helper windows.
static bool IsIgnoredClass(const wchar_t* cls) {
    static const wchar_t* const kClasses[] = {
        L"SysShadow",
        L"EdgeUiInputTopWndClass",
        L"EdgeUiInputWndClass",
        L"ApplicationManager_ImmersiveShellWindow",
        L"Internet Explorer_Hidden",
    };
    for (const wchar_t* c : kClasses) {
        if (!wcscmp(cls, c)) {
            return true;
        }
    }
    return false;
}

// Menus, tooltips and the shell's flyouts (Start, the tray overflow, task
// thumbnails and the like).
static bool IsPopupClass(const wchar_t* cls) {
    static const wchar_t* const kClasses[] = {
        L"#32768",
        L"tooltips_class32",
        L"Xaml_WindowedPopupClass",
        L"Windows.UI.Core.CoreWindow",
        L"XamlExplorerHostIslandWindow",
        L"TopLevelWindowForOverflowXamlIsland",
        L"NotifyIconOverflowWindow",
        L"TaskListThumbnailWnd",
        L"MultitaskingViewFrame",
        L"ForegroundStaging",
    };
    for (const wchar_t* c : kClasses) {
        if (!wcscmp(cls, c)) {
            return true;
        }
    }
    return false;
}

// Whether a layered window shows its whole rectangle. Desktop widgets and the
// like are drawn with per-pixel alpha or a color key and are mostly empty.
static bool IsSolidLayered(HWND hwnd) {
    BYTE alpha = 255;
    DWORD flags = 0;
    if (!GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags)) {
        return false;  // per-pixel alpha (UpdateLayeredWindow)
    }
    return !(flags & LWA_COLORKEY) && !((flags & LWA_ALPHA) && alpha < 77);
}

// Where the snow goes in the "behind windows" mode: into Explorer's wallpaper
// layer, just under the desktop icons.
struct DesktopLayer {
    HWND parent;
    HWND icons;  // the icons' window when it shares the parent, or null
};

static bool FindDesktopLayer(DesktopLayer* layer) {
    HWND progman = FindWindow(L"Progman", nullptr);
    if (!progman) {
        return false;
    }

    // Ask Explorer to put the wallpaper into a window of its own behind the
    // icons, as wallpaper apps do. Undocumented, so it's checked below.
    SendMessageTimeout(progman, 0x052C, 0xD, 0x1, SMTO_ABORTIFHUNG, 1000,
                       nullptr);
    SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_ABORTIFHUNG, 1000, nullptr);

    // Windows 11 24H2 and later keep the icons and the wallpaper as children
    // of Progman.
    HWND icons = FindWindowEx(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    if (icons && FindWindowEx(progman, nullptr, L"WorkerW", nullptr)) {
        *layer = {progman, icons};
        return true;
    }

    // Earlier versions: the wallpaper is the top-level WorkerW right behind
    // the one holding the icons.
    HWND wallpaper = nullptr;
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
            if (!FindWindowEx(hwnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
                return TRUE;
            }
            *reinterpret_cast<HWND*>(lParam) =
                FindWindowEx(nullptr, hwnd, L"WorkerW", nullptr);
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&wallpaper));
    if (!wallpaper) {
        return false;
    }

    *layer = {wallpaper, nullptr};
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar parts.
//
// Taskbar styles can make most of the taskbar window transparent, leaving only
// a floating "dock" and the tray visible. The visible parts are found with UI
// Automation on a background thread, and stored relative to the taskbar window
// so that the snow follows the taskbar while it slides in and out (auto-hide).

struct TaskbarParts {
    RECT frame;  // relative to the taskbar window
    RECT tray;
    bool hasFrame;
    bool hasTray;
    int windowWidth;
};

// CLSID_CUIAutomation, defined here to avoid linking with uuid.lib.
constexpr GUID kClsidCUIAutomation = {
    0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};

std::mutex g_taskbarPartsMutex;
std::unordered_map<HWND, TaskbarParts> g_taskbarParts;

static bool IsUsableRect(const RECT& rc) {
    return rc.right > rc.left && rc.bottom > rc.top &&
           rc.right - rc.left < 100000 && rc.bottom - rc.top < 100000;
}

static void UnionInto(RECT& dst, bool& has, const RECT& rc) {
    if (!has) {
        dst = rc;
        has = true;
    } else {
        UnionRect(&dst, &dst, &rc);
    }
}

static bool QueryTaskbarParts(IUIAutomation* uia,
                              IUIAutomationCondition* trueCondition,
                              HWND taskbar,
                              TaskbarParts* parts) {
    RECT windowRect;
    if (!GetWindowRect(taskbar, &windowRect)) {
        return false;
    }

    ComPtr<IUIAutomationElement> root;
    if (FAILED(uia->ElementFromHandle(taskbar, &root)) || !root) {
        return false;
    }

    ComPtr<IUIAutomationElementArray> children;
    if (FAILED(root->FindAll(TreeScope_Children, trueCondition, &children)) ||
        !children) {
        return false;
    }

    RECT frame = {}, tray = {};
    bool hasFrame = false, hasTray = false;

    int childCount = 0;
    children->get_Length(&childCount);
    for (int i = 0; i < childCount; i++) {
        ComPtr<IUIAutomationElement> child;
        if (FAILED(children->GetElement(i, &child)) || !child) {
            continue;
        }

        // The XAML content: the taskbar frame and the tray buttons.
        ComPtr<IUIAutomationElementArray> items;
        if (FAILED(child->FindAll(TreeScope_Children, trueCondition, &items)) ||
            !items) {
            continue;
        }

        int itemCount = 0;
        items->get_Length(&itemCount);
        for (int j = 0; j < itemCount; j++) {
            ComPtr<IUIAutomationElement> item;
            if (FAILED(items->GetElement(j, &item)) || !item) {
                continue;
            }

            BSTR className = nullptr;
            RECT rc;
            if (FAILED(item->get_CurrentClassName(&className)) || !className) {
                continue;
            }
            if (SUCCEEDED(item->get_CurrentBoundingRectangle(&rc)) &&
                IsUsableRect(rc) && rc.bottom - rc.top >= 10) {
                if (!wcscmp(className, L"Taskbar.TaskbarFrameAutomationPeer")) {
                    UnionInto(frame, hasFrame, rc);
                } else if (!wcsncmp(className, L"SystemTray.", 11)) {
                    UnionInto(tray, hasTray, rc);
                }
            }
            SysFreeString(className);
        }
    }

    // Nothing is found while an auto-hidden taskbar is hidden, and the result
    // is useless if the taskbar moved during the query (it was sliding).
    RECT windowRectAfter;
    if (!hasFrame || !GetWindowRect(taskbar, &windowRectAfter) ||
        !EqualRect(&windowRect, &windowRectAfter)) {
        return false;
    }

    OffsetRect(&frame, -windowRect.left, -windowRect.top);
    OffsetRect(&tray, -windowRect.left, -windowRect.top);
    parts->frame = frame;
    parts->tray = tray;
    parts->hasFrame = hasFrame;
    parts->hasTray = hasTray;
    parts->windowWidth = windowRect.right - windowRect.left;
    return true;
}

DWORD WINAPI TaskbarThread(LPVOID) {

    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        return 0;
    }

    {
        ComPtr<IUIAutomation> uia;
        ComPtr<IUIAutomationCondition> trueCondition;
        if (SUCCEEDED(CoCreateInstance(kClsidCUIAutomation, nullptr,
                                       CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(&uia))) &&
            SUCCEEDED(uia->CreateTrueCondition(&trueCondition))) {
            // Don't get stuck for long if the taskbar doesn't respond.
            ComPtr<IUIAutomation2> uia2;
            if (SUCCEEDED(uia.As(&uia2))) {
                uia2->put_ConnectionTimeout(1000);
                uia2->put_TransactionTimeout(1000);
            }

            do {
                TaskbarSnow mode = g_settings.taskbarSnow;
                if ((mode != TaskbarSnow::Auto && mode != TaskbarSnow::Parts) ||
                    g_visibleOverlays == 0) {
                    continue;
                }

                std::vector<HWND> taskbars;
                EnumWindows(
                    [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
                        wchar_t cls[64];
                        if (GetClassName(hwnd, cls, ARRAYSIZE(cls)) &&
                            IsTaskbarClass(cls)) {
                            reinterpret_cast<std::vector<HWND>*>(lParam)
                                ->push_back(hwnd);
                        }
                        return TRUE;
                    },
                    reinterpret_cast<LPARAM>(&taskbars));

                std::unordered_map<HWND, TaskbarParts> previous;
                {
                    std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
                    previous = g_taskbarParts;
                }

                // The parts are stored relative to the taskbar window, so the
                // last known ones stay valid while the taskbar is hidden.
                std::unordered_map<HWND, TaskbarParts> found;
                for (HWND taskbar : taskbars) {
                    TaskbarParts parts;
                    if (QueryTaskbarParts(uia.Get(), trueCondition.Get(),
                                          taskbar, &parts)) {
                        found[taskbar] = parts;
                    } else if (auto it = previous.find(taskbar);
                               it != previous.end()) {
                        found[taskbar] = it->second;
                    }
                }

                std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
                g_taskbarParts = std::move(found);
            } while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT);
        }
    }

    CoUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// The snow.

struct Flake {
    float x, y;  // screen coordinates
    float size;
    float fallSpeed;
    float phase;
    float sway;
    float alpha;
};

// A lump of snow that fell off a cap.
struct Chunk {
    float x, y;
    float vx, vy;
    float size;
    float area;
    uint64_t ignoreKey;  // the cap it came from
    bool burst;          // bursts into powder where it lands
};

// Something that snow can lie on: a window, the taskbar or a part of it. Bodies
// without a snow cap (maximized windows, menus, tool windows) only hide the
// snow behind them.
struct Body {
    uint64_t key;
    HWND hwnd;
    RECT rc;
    bool snowCap;
};

static bool SameBodies(const std::vector<Body>& a, const std::vector<Body>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].key != b[i].key || a[i].snowCap != b[i].snowCap ||
            !EqualRect(&a[i].rc, &b[i].rc)) {
            return false;
        }
    }
    return true;
}

// A puff of powder snow. It drifts with the wind and melts away.
struct Puff {
    float x, y;
    float vx, vy;
    float size;
    float life;
    float maxLife;
};

// One pixel column of a snow cap.
struct Column {
    float h = 0;     // snow height
    float lift = 0;  // how high the snow is above the body's top edge
    float vel = 0;   // vertical speed of the snow, px/s, down is positive
    float hang = 0;  // seconds left to hang in the air
};

struct Cap {
    HWND hwnd = nullptr;
    RECT rc = {};  // the body rect in the previous frame
    std::vector<Column> cols;
    uint32_t seed = 0;
    int offset = 0;  // keeps the drifts in place when the left edge moves
    float vx = 0, vy = 0;  // smoothed speed of the body
    float carry = 0;  // the fastest upward speed the snow was carried at
    float squash = 0, squashVel = 0;
    bool airborne = false;  // some of the snow is in the air
    unsigned seenFrame = 0;
    float shakeDir = 0;   // the direction of the last fast sideways move
    float shakeTime = 0;  // seconds since then
    float shakeSpeed = 0;
    float heightScale = 1;  // the ground holds more snow than windows
};

static uint64_t BodyKey(HWND hwnd, int part) {
    return (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(hwnd)) << 2) |
           part;
}

constexpr uint64_t kGroundKey = 3;  // BodyKey(nullptr, 3)

static uint32_t Hash32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

// Smooth 1D value noise in [0, 1).
static float SmoothNoise(uint32_t seed, float x) {
    float cell = std::floor(x);
    uint32_t i = static_cast<uint32_t>(static_cast<int>(cell));
    float t = x - cell;
    t = t * t * (3.0f - 2.0f * t);
    float a = (Hash32(seed + i * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    float b = (Hash32(seed + (i + 1) * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    return a + (b - a) * t;
}

static float CapNoise(const Cap& cap, int column, uint32_t salt, float scale) {
    return SmoothNoise(cap.seed + salt * 0x51ED27u,
                       static_cast<float>(column + cap.offset) / scale);
}

// The most snow a column can hold. It varies along the cap so that the snow
// settles in drifts of different heights instead of a flat strip.
static float SnowLimit(const Cap& cap, int column, float maxHeight,
                       float scale) {
    float n = 0.65f * CapNoise(cap, column, 1, 70.0f * scale) +
              0.35f * CapNoise(cap, column, 2, 23.0f * scale);
    n = std::clamp((n - 0.5f) * 1.8f + 0.5f, 0.0f, 1.0f);
    return maxHeight * (0.3f + 0.7f * n);
}

// How briskly the snow changes, which sets how often it's drawn.
enum class Pace {
    Still,        // nothing moves: the last frame stays on the screen
    Calm,         // only the snowfall and powder: about 30 fps
    Interactive,  // a window, the cursor or lumps of snow move: full rate
};

enum class RunResult { Stop, Recreate, Retry };

class Overlay {
   public:
    Overlay(HMONITOR monitor, const RECT& area, HANDLE stopEvent)
        : m_monitor(monitor), m_area(area), m_stop(stopEvent) {}
    ~Overlay();

    bool Create();
    RunResult Run();

   private:
    bool CreateDeviceResources();
    void ReleaseDeviceResources();
    bool CreateTargetBitmap();
    bool CreateFlakeSprite();
    void Restack();
    HWND LowestOpenPopup();
    void ApplyCaptureSetting();
    void UpdateRefreshRate();
    UINT SyncIntervalFor(int fps);
    void Step(float dt);

    float Rand();
    void SpawnFlake(Flake& f, bool scatter);
    void SpawnChunk(float x, float y, float area, float vx, float vy,
                    uint64_t ignoreKey, bool burst = false);
    void SpawnPuff(float x, float y, float vx, float vy, float size,
                   float life);
    void DropColumns(Cap& cap, int first, int count, int group = kChunkColumns,
                     bool burst = false);
    void ShakeOff(Cap& cap, const RECT& rc, float vx);
    float CapMax(const Cap& cap) const;
    void Melt(Cap& cap, float dt);

    void CollectBodies();
    void AddTaskbarBodies(HWND hwnd, const RECT& rc);
    bool IsFullscreenAppActive();
    bool IsCoveredAbove(size_t bodyIndex, int x, int y);
    bool IsOnThisMonitor(const RECT& rc) const;
    const Body* FindLanding(float x, float oldY, float newY,
                            uint64_t ignoreKey);
    void AddSnow(uint64_t key, int column, float amount);

    void UpdateCaps(float dt);
    bool InheritSnow(Cap& cap, unsigned lastSeen);
    void ResizeCap(Cap& cap, const RECT& rc);
    void MoveCap(Cap& cap, const RECT& rc, float dt);
    void Ambient(float dt);
    void Sweep(float dt);
    void UpdateFlakes(float dt);
    void UpdateChunks(float dt);
    void UpdatePuffs(float dt);
    bool Render(UINT syncInterval);
    void DrawCap(const Cap& cap, const RECT& rc);

    HWND m_hwnd = nullptr;
    bool m_behind = false;       // in the desktop's wallpaper layer
    bool m_wantBehind = false;   // the setting when the overlay was made
    DesktopLayer m_layer = {};
    bool m_hiddenFromCapture = false;
    HMONITOR m_monitor;
    RECT m_area;  // the monitor
    HANDLE m_stop;
    float m_scale = 1.0f;  // the monitor's display scale
    bool m_counted = false;  // counted in g_visibleOverlays
    int m_width = 0;
    int m_height = 0;
    bool m_visible = true;
    double m_refreshRate = 60.0;

    ComPtr<ID3D11Device> m_d3dDevice;
    ComPtr<ID2D1Factory1> m_d2dFactory;
    ComPtr<ID2D1Device> m_d2dDevice;
    ComPtr<ID2D1DeviceContext> m_ctx;
    ComPtr<IDXGISwapChain1> m_swapChain;
    HANDLE m_frameWait = nullptr;
    ComPtr<IDCompositionDevice> m_dcompDevice;
    ComPtr<IDCompositionTarget> m_dcompTarget;
    ComPtr<IDCompositionVisual> m_dcompVisual;
    ComPtr<ID2D1Bitmap1> m_targetBitmap;
    ComPtr<ID2D1Bitmap1> m_flakeSprite;
    ComPtr<ID2D1LinearGradientBrush> m_capBrush;

    std::vector<Flake> m_flakes;
    std::vector<Chunk> m_chunks;
    std::vector<Puff> m_puffs;
    std::vector<Body> m_bodies;
    std::vector<Body> m_prevBodies;
    std::unordered_map<uint64_t, Cap> m_caps;

    POINT m_lastCursor = {};
    bool m_cursorValid = false;
    uint32_t m_rng = 0x9E3779B9;
    float m_time = 0;
    float m_frameTime = 1.0f / 60;
    unsigned m_frame = 0;

    Pace m_pace = Pace::Calm;
    double m_activeUntil = 0;   // keep the full frame rate until then
    bool m_anyAirborne = false; // some cap's snow is in the air or springs
    bool m_melting = false;     // some snow is melting
};

constexpr wchar_t kOverlayClass[] = L"WindhawkDesktopSnowfallOverlay";

Overlay::~Overlay() {
    if (m_counted && m_visible) {
        g_visibleOverlays--;
    }
    ReleaseDeviceResources();
    if (m_hwnd && IsWindow(m_hwnd)) {
        RemoveProp(m_hwnd, L"NonRudeHWND");
        DestroyWindow(m_hwnd);
    }
}

float Overlay::CapMax(const Cap& cap) const {
    return g_settings.capHeight * m_scale * cap.heightScale;
}

static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                       LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

float Overlay::Rand() {
    m_rng ^= m_rng << 13;
    m_rng ^= m_rng >> 17;
    m_rng ^= m_rng << 5;
    return (m_rng & 0xFFFFFF) / 16777216.0f;
}

void Overlay::UpdateRefreshRate() {
    MONITORINFOEX mi = {};
    mi.cbSize = sizeof(mi);
    DEVMODE dm = {};
    dm.dmSize = sizeof(dm);
    if (GetMonitorInfo(m_monitor, &mi) &&
        EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
        dm.dmDisplayFrequency > 1) {
        m_refreshRate = dm.dmDisplayFrequency;
    }
}

// Frames are shown on whole refreshes of the display, so the rate is the
// display's rate divided by a whole number.
UINT Overlay::SyncIntervalFor(int fps) {
    int interval = static_cast<int>(std::lround(m_refreshRate / fps));
    return static_cast<UINT>(std::clamp(interval, 1, 4));
}

bool Overlay::Create() {
    HINSTANCE instance = GetModuleHandle(nullptr);
    m_width = m_area.right - m_area.left;
    m_height = m_area.bottom - m_area.top;

    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(m_monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                   &dpiY))) {
        m_scale = std::max(dpiX, 96u) / 96.0f;
    }

    m_wantBehind = g_settings.behindWindows;
    if (m_wantBehind) {
        m_behind = FindDesktopLayer(&m_layer);
        if (!m_behind) {
            Wh_Log(L"The desktop's wallpaper layer wasn't found, the snow "
                   L"falls over all windows for now");
        }
    }

    if (m_behind) {
        // A child of the wallpaper layer, under the desktop icons, so it needs
        // neither to be click-through nor to stay above other windows.
        POINT origin = {m_area.left, m_area.top};
        ScreenToClient(m_layer.parent, &origin);
        // Not layered, a child counts as opaque, and the wallpaper under it
        // may stop being drawn. WS_EX_NOPARENTNOTIFY: creating and destroying
        // the window doesn't wait for Explorer, which might be hung.
        for (DWORD layered : {static_cast<DWORD>(WS_EX_LAYERED), 0ul}) {
            m_hwnd = CreateWindowEx(
                WS_EX_NOREDIRECTIONBITMAP | WS_EX_NOACTIVATE |
                    WS_EX_NOPARENTNOTIFY | layered,
                kOverlayClass, L"Desktop Snowfall",
                WS_CHILD | WS_DISABLED | WS_CLIPSIBLINGS, origin.x, origin.y,
                m_width, m_height, m_layer.parent, nullptr, instance, nullptr);
            if (m_hwnd) {
                if (layered) {
                    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
                }
                break;
            }
        }
        if (!m_hwnd) {
            Wh_Log(L"Creating the overlay in the wallpaper layer failed: %u",
                   GetLastError());
            return false;
        }
    } else {
        m_hwnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
            kOverlayClass, L"Desktop Snowfall", WS_POPUP, m_area.left,
            m_area.top, m_width, m_height, nullptr, nullptr, instance, nullptr);
        if (!m_hwnd) {
            return false;
        }

        // Needed for a layered window to be shown; the content itself comes
        // from DirectComposition.
        SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);

        // The overlay covers the whole monitor, so the taskbar would treat it
        // as a fullscreen app and stop staying on top of other windows: an
        // auto-hidden taskbar would then slide out behind a maximized window.
        SetProp(m_hwnd, L"NonRudeHWND", reinterpret_cast<HANDLE>(TRUE));
        ApplyCaptureSetting();
    }

    if (!CreateDeviceResources()) {
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
    Restack();
    UpdateRefreshRate();
    m_visible = true;
    m_counted = true;
    g_visibleOverlays++;
    return true;
}

// The window right above this one, looking past the other monitors' snow.
static HWND WindowAboveOverlay(HWND hwnd) {
    HWND above = GetWindow(hwnd, GW_HWNDPREV);
    wchar_t cls[64];
    while (above && GetClassName(above, cls, ARRAYSIZE(cls)) &&
           !wcscmp(cls, kOverlayClass)) {
        above = GetWindow(above, GW_HWNDPREV);
    }
    return above;
}

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// Before Windows 10 version 2004, WDA_EXCLUDEFROMCAPTURE acts as
// WDA_MONITOR: the overlay would show up as a black screen in captures.
static bool CanExcludeFromCapture() {
    static const bool supported = [] {
        using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
        auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
            GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlGetVersion"));
        OSVERSIONINFOW version = {sizeof(version)};
        return rtlGetVersion && rtlGetVersion(&version) == 0 &&
               version.dwBuildNumber >= 19041;
    }();
    return supported;
}

// Only top-level windows can be kept out of screenshots.
void Overlay::ApplyCaptureSetting() {
    bool hide = g_settings.hideFromCapture && !m_behind &&
                CanExcludeFromCapture();
    if (hide == m_hiddenFromCapture) {
        return;
    }
    if (SetWindowDisplayAffinity(m_hwnd,
                                 hide ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE)) {
        m_hiddenFromCapture = hide;
    } else {
        Wh_Log(L"SetWindowDisplayAffinity failed: %u", GetLastError());
        m_hiddenFromCapture = hide;  // don't retry every second
    }
}

// The lowest of the menus, tooltips and shell flyouts open at the very top,
// which the snow goes just under. Click-through windows are looked past: they
// are overlays like this one.
HWND Overlay::LowestOpenPopup() {
    DWORD shellProcess = 0;
    if (HWND tray = FindWindow(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &shellProcess);
    }

    HWND lowest = nullptr;
    for (HWND hwnd = GetTopWindow(nullptr); hwnd;
         hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        if (!(exStyle & WS_EX_TOPMOST)) {
            break;
        }
        if (hwnd == m_hwnd || (exStyle & WS_EX_TRANSPARENT) ||
            !IsWindowVisible(hwnd) || IsCloaked(hwnd)) {
            continue;
        }

        wchar_t cls[64];
        if (!GetClassName(hwnd, cls, ARRAYSIZE(cls))) {
            continue;
        }
        DWORD process = 0;
        GetWindowThreadProcessId(hwnd, &process);
        bool popup = IsPopupClass(cls) || IsIgnoredClass(cls) ||
                     (process == shellProcess && !IsTaskbarClass(cls));
        if (!popup) {
            break;
        }
        lowest = hwnd;
    }
    return lowest;
}

// Other topmost windows (the taskbar, for one) end up above the overlay when
// they're activated; this puts the snow back on top of them, but still under
// open menus. In the wallpaper layer, the snow is kept just under the icons
// and above live wallpapers, which go into the same layer.
void Overlay::Restack() {
    HWND above = WindowAboveOverlay(m_hwnd);
    HWND after;
    if (m_behind) {
        after = m_layer.icons ? m_layer.icons : HWND_TOP;
        if (above == m_layer.icons) {
            return;
        }
    } else {
        HWND popup = LowestOpenPopup();
        after = popup ? popup : HWND_TOPMOST;
        if (above == popup) {
            return;
        }
    }
    SetWindowPos(m_hwnd, after, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

bool Overlay::CreateDeviceResources() {
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   flags, nullptr, 0, D3D11_SDK_VERSION,
                                   &m_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                               nullptr, 0, D3D11_SDK_VERSION, &m_d3dDevice,
                               nullptr, nullptr);
    }
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: %08X", hr);
        return false;
    }

    ComPtr<IDXGIDevice> dxgiDevice;
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIFactory2> dxgiFactory;
    if (FAILED(m_d3dDevice.As(&dxgiDevice)) ||
        FAILED(dxgiDevice->GetAdapter(&adapter)) ||
        FAILED(adapter->GetParent(IID_PPV_ARGS(&dxgiFactory)))) {
        return false;
    }

    D2D1_FACTORY_OPTIONS options = {};
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory1), &options,
                                 reinterpret_cast<void**>(
                                     m_d2dFactory.GetAddressOf()))) ||
        FAILED(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)) ||
        FAILED(m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_ctx))) {
        Wh_Log(L"Direct2D initialization failed");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    hr = dxgiFactory->CreateSwapChainForComposition(m_d3dDevice.Get(), &desc,
                                                    nullptr, &m_swapChain);
    if (FAILED(hr)) {
        Wh_Log(L"CreateSwapChainForComposition failed: %08X", hr);
        return false;
    }

    // Render one frame per refresh at most, without queuing frames.
    ComPtr<IDXGISwapChain2> swapChain2;
    if (SUCCEEDED(m_swapChain.As(&swapChain2))) {
        swapChain2->SetMaximumFrameLatency(1);
        m_frameWait = swapChain2->GetFrameLatencyWaitableObject();
    }

    if (FAILED(DCompositionCreateDevice(dxgiDevice.Get(),
                                        IID_PPV_ARGS(&m_dcompDevice))) ||
        FAILED(m_dcompDevice->CreateTargetForHwnd(m_hwnd, TRUE,
                                                  &m_dcompTarget)) ||
        FAILED(m_dcompDevice->CreateVisual(&m_dcompVisual)) ||
        FAILED(m_dcompVisual->SetContent(m_swapChain.Get())) ||
        FAILED(m_dcompTarget->SetRoot(m_dcompVisual.Get())) ||
        FAILED(m_dcompDevice->Commit())) {
        Wh_Log(L"DirectComposition initialization failed");
        return false;
    }

    D2D1_GRADIENT_STOP stops[] = {
        {0.0f, D2D1::ColorF(0.99f, 0.995f, 1.0f, 0.97f)},
        {1.0f, D2D1::ColorF(0.78f, 0.85f, 0.94f, 0.97f)},
    };
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    if (FAILED(m_ctx->CreateGradientStopCollection(stops, ARRAYSIZE(stops),
                                                   &stopCollection)) ||
        FAILED(m_ctx->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties({0, 0}, {0, 1}),
            stopCollection.Get(), &m_capBrush))) {
        return false;
    }

    return CreateFlakeSprite() && CreateTargetBitmap();
}

void Overlay::ReleaseDeviceResources() {
    if (m_ctx) {
        m_ctx->SetTarget(nullptr);
    }
    m_targetBitmap.Reset();
    m_flakeSprite.Reset();
    m_capBrush.Reset();
    m_dcompVisual.Reset();
    m_dcompTarget.Reset();
    m_dcompDevice.Reset();
    if (m_frameWait) {
        CloseHandle(m_frameWait);
        m_frameWait = nullptr;
    }
    m_swapChain.Reset();
    m_ctx.Reset();
    m_d2dDevice.Reset();
    m_d2dFactory.Reset();
    m_d3dDevice.Reset();
}

bool Overlay::CreateTargetBitmap() {
    ComPtr<IDXGISurface> surface;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface)))) {
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(m_ctx->CreateBitmapFromDxgiSurface(surface.Get(), &props,
                                                  &m_targetBitmap))) {
        return false;
    }

    m_ctx->SetTarget(m_targetBitmap.Get());
    return true;
}

// A soft round flake with a faint bluish rim that keeps it visible on light
// windows. It's drawn once and then stamped for every flake.
bool Overlay::CreateFlakeSprite() {
    constexpr UINT32 kSize = 32;
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(m_ctx->CreateBitmap(D2D1::SizeU(kSize, kSize), nullptr, 0,
                                   &props, &m_flakeSprite))) {
        return false;
    }

    D2D1_GRADIENT_STOP stops[] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)},
        {0.42f, D2D1::ColorF(0.98f, 0.99f, 1.0f, 0.95f)},
        {0.6f, D2D1::ColorF(0.75f, 0.82f, 0.95f, 0.35f)},
        {1.0f, D2D1::ColorF(0.6f, 0.7f, 0.85f, 0.0f)},
    };
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    ComPtr<ID2D1RadialGradientBrush> brush;
    if (FAILED(m_ctx->CreateGradientStopCollection(stops, ARRAYSIZE(stops),
                                                   &stopCollection)) ||
        FAILED(m_ctx->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties({kSize / 2.0f, kSize / 2.0f},
                                                {0, 0}, kSize / 2.0f,
                                                kSize / 2.0f),
            stopCollection.Get(), &brush))) {
        return false;
    }

    m_ctx->SetTarget(m_flakeSprite.Get());
    m_ctx->BeginDraw();
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));
    m_ctx->FillRectangle(D2D1::RectF(0, 0, kSize, kSize), brush.Get());
    HRESULT hr = m_ctx->EndDraw();
    m_ctx->SetTarget(nullptr);
    return SUCCEEDED(hr);
}

void Overlay::SpawnFlake(Flake& f, bool scatter) {
    float depth = Rand();
    f.size = (0.8f + depth * depth * 2.7f) * g_settings.size / 100.0f * m_scale;
    f.fallSpeed = 35.0f * m_scale + f.size * 22.0f;
    f.phase = Rand() * 6.2832f;
    f.sway = (6.0f + Rand() * 14.0f) * m_scale;
    f.alpha = 0.45f + 0.5f * Rand();
    f.x = m_area.left + Rand() * m_width;
    f.y = scatter ? m_area.top + Rand() * m_height
                  : m_area.top - f.size * 2 - Rand() * 40.0f * m_scale;
}

void Overlay::SpawnChunk(float x, float y, float area, float vx, float vy,
                         uint64_t ignoreKey, bool burst) {
    if (m_chunks.size() >= kMaxChunks || area < 0.5f) {
        return;
    }

    Chunk c;
    c.x = x;
    c.y = y;
    c.vx = vx;
    c.vy = vy;
    c.area = area;
    c.size = std::clamp(std::sqrt(area) * 0.55f, m_scale,
                        (burst ? 5.0f : 3.5f) * m_scale);
    c.ignoreKey = ignoreKey;
    c.burst = burst;
    m_chunks.push_back(c);
}

void Overlay::SpawnPuff(float x, float y, float vx, float vy, float size,
                        float life) {
    if (m_puffs.size() >= kMaxPuffs) {
        return;
    }
    m_puffs.push_back({x, y, vx, vy, size * m_scale, life, life});
}

// Turns snow columns that lost their support (the cap's position is from the
// previous frame) into falling chunks.
void Overlay::DropColumns(Cap& cap, int first, int count, int group,
                          bool burst) {
    for (int i = 0; i < count; i += group) {
        float area = 0, top = 0, lift = 0;
        for (int k = i; k < std::min(i + group, count); k++) {
            const Column& col = cap.cols[first + k];
            area += col.h;
            top = std::max(top, col.h);
            lift = std::max(lift, col.lift);
        }
        SpawnChunk(cap.rc.left + first + i + group / 2.0f,
                   cap.rc.top - lift - top / 2, area,
                   (Rand() - 0.5f) * (burst ? 50.0f : 30.0f),
                   burst ? -Rand() * 40.0f : 0, 0, burst);
    }
}

void Overlay::AddTaskbarBodies(HWND hwnd, const RECT& rc) {
    TaskbarSnow mode = g_settings.taskbarSnow;
    if (mode == TaskbarSnow::Off) {
        // No snow on it, but it hides the snow on the ground.
        m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, false});
        return;
    }

    if (mode != TaskbarSnow::Whole) {
        TaskbarParts parts;
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
            auto it = g_taskbarParts.find(hwnd);
            if (it != g_taskbarParts.end()) {
                parts = it->second;
                found = true;
            }
        }

        if (found && parts.windowWidth == rc.right - rc.left) {
            bool useParts =
                mode == TaskbarSnow::Parts ||
                parts.frame.right - parts.frame.left < parts.windowWidth * 0.85;
            if (useParts) {
                RECT frame = parts.frame;
                OffsetRect(&frame, rc.left, rc.top);
                m_bodies.push_back({BodyKey(hwnd, 1), hwnd, frame, true});
                if (parts.hasTray) {
                    RECT tray = parts.tray;
                    OffsetRect(&tray, rc.left, rc.top);
                    m_bodies.push_back({BodyKey(hwnd, 2), hwnd, tray, true});
                }
                return;
            }
        }
    }

    m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, true});
}

bool Overlay::IsOnThisMonitor(const RECT& rc) const {
    POINT top = {rc.left + (rc.right - rc.left) / 2, rc.top};
    return MonitorFromPoint(top, MONITOR_DEFAULTTONEAREST) == m_monitor;
}

void Overlay::CollectBodies() {
    m_bodies.clear();
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
            Overlay* self = reinterpret_cast<Overlay*>(lParam);
            if (hwnd == self->m_hwnd || !IsWindowVisible(hwnd) ||
                IsIconic(hwnd)) {
                return TRUE;
            }

            wchar_t cls[64];
            if (!GetClassName(hwnd, cls, ARRAYSIZE(cls)) ||
                IsShellWindowClass(cls) || IsIgnoredClass(cls)) {
                return TRUE;
            }

            bool isTaskbar = IsTaskbarClass(cls);
            LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
            if (!isTaskbar && (exStyle & WS_EX_TRANSPARENT)) {
                return TRUE;  // click-through overlays
            }
            if (IsCloaked(hwnd)) {
                return TRUE;
            }

            RECT rc;
            if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                             &rc, sizeof(rc)))) {
                GetWindowRect(hwnd, &rc);
            }

            RECT visible;
            if (!IntersectRect(&visible, &rc, &self->m_area)) {
                return TRUE;  // on another monitor
            }

            // Menus, tool windows and small windows don't get snow, but they
            // hide the snow behind them.
            const float scale = self->m_scale;
            bool coverOnly = !isTaskbar &&
                             ((exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) ||
                              IsPopupClass(cls) ||
                              rc.right - rc.left < 80 * scale ||
                              rc.bottom - rc.top < 30 * scale);
            if (coverOnly) {
                if (rc.right - rc.left < 2 || rc.bottom - rc.top < 2 ||
                    ((exStyle & WS_EX_LAYERED) && !IsSolidLayered(hwnd))) {
                    return TRUE;
                }
                // An invisible window over the whole screen would hide all the
                // snow.
                MONITORINFO mi = {sizeof(mi)};
                if (GetMonitorInfo(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST),
                                   &mi)) {
                    const RECT& m = mi.rcMonitor;
                    RECT both;
                    IntersectRect(&both, &rc, &m);
                    double covered = static_cast<double>(both.right - both.left) *
                                     (both.bottom - both.top);
                    if (covered >= 0.9 * (m.right - m.left) * (m.bottom - m.top)) {
                        return TRUE;
                    }
                }
                self->m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, false});
                return TRUE;
            }

            // A window's snow lies on the monitor with the middle of its top
            // edge. On the other monitors the window only hides the snow.
            bool here = self->IsOnThisMonitor(rc);
            if (isTaskbar) {
                if (here && rc.right - rc.left >= 80 * scale &&
                    rc.bottom - rc.top >= 30 * scale) {
                    self->AddTaskbarBodies(hwnd, rc);
                } else {
                    self->m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, false});
                }
            } else {
                // Snow on a window that touches the top of the screen (snapped
                // to the top half, say) couldn't be seen.
                bool snowCap =
                    here && !IsZoomed(hwnd) && rc.top > self->m_area.top;
                self->m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, snowCap});
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(this));

    // The ground: the bottom edge of the screen, under everything else. It
    // shows only where nothing covers it, such as under an auto-hidden or
    // see-through taskbar.
    if (g_settings.groundSnow) {
        RECT ground = {m_area.left, m_area.bottom, m_area.right,
                       m_area.bottom + 100};
        m_bodies.push_back({kGroundKey, nullptr, ground, true});
    }
}

bool Overlay::IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == m_hwnd) {
        return false;
    }

    wchar_t cls[64];
    if (!GetClassName(fg, cls, ARRAYSIZE(cls)) || IsShellWindowClass(cls) ||
        IsTaskbarClass(cls)) {
        return false;
    }

    RECT rc;
    MONITORINFO mi = {sizeof(mi)};
    HMONITOR monitor = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    if (monitor != m_monitor || !GetWindowRect(fg, &rc) ||
        !GetMonitorInfo(monitor, &mi)) {
        return false;
    }

    if (rc.left > mi.rcMonitor.left || rc.top > mi.rcMonitor.top ||
        rc.right < mi.rcMonitor.right || rc.bottom < mi.rcMonitor.bottom) {
        return false;
    }

    // With an auto-hidden taskbar, a maximized window covers the whole
    // monitor too, but it isn't a fullscreen app.
    LONG style = GetWindowLong(fg, GWL_STYLE);
    return !(IsZoomed(fg) && (style & WS_CAPTION) == WS_CAPTION);
}

bool Overlay::IsCoveredAbove(size_t bodyIndex, int x, int y) {
    for (size_t j = 0; j < bodyIndex; j++) {
        const RECT& o = m_bodies[j].rc;
        if (x >= o.left && x < o.right && y >= o.top && y < o.bottom) {
            return true;
        }
    }
    return false;
}

// The topmost body under a falling particle decides what happens to it: the
// particle lands if it just crossed the body's top edge.
const Body* Overlay::FindLanding(float x, float oldY, float newY,
                                 uint64_t ignoreKey) {
    int sx = static_cast<int>(std::floor(x));
    for (const Body& b : m_bodies) {
        if (sx < b.rc.left || sx >= b.rc.right || newY < b.rc.top ||
            newY >= b.rc.bottom) {
            continue;
        }
        if (b.key != ignoreKey && b.snowCap && oldY < b.rc.top) {
            return &b;
        }
        return nullptr;
    }
    return nullptr;
}

void Overlay::AddSnow(uint64_t key, int column, float amount) {
    auto it = m_caps.find(key);
    if (it == m_caps.end()) {
        return;
    }

    Cap& cap = it->second;
    std::vector<Column>& cols = cap.cols;
    const int width = static_cast<int>(cols.size());
    const float maxHeight = CapMax(cap);

    // Let the pile relax so it doesn't form needles, and keep it under the
    // drift profile.
    constexpr int kReach = 14;
    int from = std::max(column - kReach, 1);
    int to = std::min(column + kReach, width - 2);
    if (from > to) {
        return;
    }

    float limit[2 * kReach + 1];
    for (int c = from; c <= to; c++) {
        limit[c - from] = SnowLimit(cap, c, maxHeight, m_scale);
    }

    for (int i = 0; i < 9; i++) {
        int c = column + i - 4;
        if (c >= from && c <= to) {
            cols[c].h = std::min(limit[c - from],
                                 cols[c].h + amount * kSnowKernel[i]);
        }
    }

    for (int pass = 0; pass < 2; pass++) {
        for (int c = from; c <= to; c++) {
            float h = cols[c].h * 0.5f + (cols[c - 1].h + cols[c + 1].h) * 0.25f;
            cols[c].h = std::min(h, limit[c - from]);
        }
    }
}

// Keeps the columns in place when only one side of the body moved, and drops
// the snow from the part that's gone.
void Overlay::ResizeCap(Cap& cap, const RECT& rc) {
    int oldWidth = cap.rc.right - cap.rc.left;
    int newWidth = rc.right - rc.left;
    std::vector<Column>& cols = cap.cols;

    if (oldWidth == newWidth) {
        // Moved sideways. Optionally, the snow stays where it is and the
        // window slides out from under it.
        int dx = rc.left - cap.rc.left;
        int width = static_cast<int>(cols.size());
        if (dx == 0 || !g_settings.slideOff || width == 0) {
            return;
        }

        int n = std::min(std::abs(dx), width);
        if (dx > 0) {
            DropColumns(cap, 0, n);
            cols.erase(cols.begin(), cols.begin() + n);
            cols.resize(width, Column{});
        } else {
            DropColumns(cap, width - n, n);
            cols.resize(width - n);
            cols.insert(cols.begin(), n, Column{});
        }
        cap.offset += dx;
        return;
    }
    int dl = rc.left - cap.rc.left;
    int dr = rc.right - cap.rc.right;
    if (dl != 0 && dr != 0) {
        // Moved and resized at once (snapped, restored): stretch the snow.
        std::vector<Column> old = std::move(cols);
        cols.assign(newWidth, Column{});
        if (!old.empty()) {
            for (int c = 0; c < newWidth; c++) {
                size_t src = static_cast<size_t>(c) * old.size() / newWidth;
                cols[c].h = old[src].h;
            }
        }
        return;
    }

    if (dl > 0) {
        int n = std::min(dl, static_cast<int>(cols.size()));
        DropColumns(cap, 0, n);
        cols.erase(cols.begin(), cols.begin() + n);
    } else if (dl < 0) {
        cols.insert(cols.begin(), -dl, Column{});
    }
    cap.offset += dl;

    if (dr < 0) {  // the left edge didn't move
        int n = std::min(-dr, static_cast<int>(cols.size()));
        int first = static_cast<int>(cols.size()) - n;
        DropColumns(cap, first, n);
        cols.resize(first);
    }

    cols.resize(newWidth, Column{});
}

void Overlay::UpdateCaps(float dt) {
    m_anyAirborne = false;
    if (g_settings.capHeight <= 0) {
        m_caps.clear();
        return;
    }

    std::vector<HWND> inheritedFrom;
    for (const Body& b : m_bodies) {
        if (!b.snowCap) {
            continue;
        }

        auto [it, inserted] = m_caps.try_emplace(b.key);
        Cap& cap = it->second;
        const int width = b.rc.right - b.rc.left;

        if (inserted || cap.seenFrame != m_frame - 1) {
            // New, or back after being minimized or hidden: just put the snow
            // on top.
            unsigned lastSeen = inserted ? 0 : cap.seenFrame;
            if (inserted) {
                cap.hwnd = b.hwnd;
                cap.heightScale = b.key == kGroundKey ? 1.5f : 1.0f;
                cap.seed = Hash32(static_cast<uint32_t>(b.key) ^
                                  static_cast<uint32_t>(b.key >> 32));
            }
            cap.rc = b.rc;
            cap.cols.resize(width);
            for (Column& col : cap.cols) {
                col.lift = col.vel = col.hang = 0;
            }
            cap.vx = cap.vy = cap.carry = 0;
            cap.squash = cap.squashVel = 0;
            cap.airborne = false;
            if (InheritSnow(cap, lastSeen)) {
                inheritedFrom.push_back(cap.hwnd);
            }
        } else {
            ResizeCap(cap, b.rc);
            MoveCap(cap, b.rc, dt);
        }

        cap.rc = b.rc;
        cap.seenFrame = m_frame;
    }

    // A taskbar switched between being a whole and being split into its
    // visible parts. Whatever snow didn't move over to the new caps falls.
    for (HWND hwnd : inheritedFrom) {
        for (auto it = m_caps.begin(); it != m_caps.end();) {
            Cap& cap = it->second;
            if (cap.hwnd == hwnd && cap.seenFrame != m_frame) {
                if (cap.seenFrame == m_frame - 1) {
                    DropColumns(cap, 0, static_cast<int>(cap.cols.size()));
                }
                it = m_caps.erase(it);
            } else {
                ++it;
            }
        }
    }

    // A window that was closed, minimized, hidden or moved to another monitor
    // drops its snow in lumps. A maximized window keeps its snow for later, and
    // so does a window on another virtual desktop (it's cloaked).
    for (auto it = m_caps.begin(); it != m_caps.end();) {
        Cap& cap = it->second;
        if (cap.seenFrame == m_frame) {
            m_anyAirborne = m_anyAirborne || cap.airborne ||
                            cap.squash != 0 || cap.squashVel != 0;
            ++it;
            continue;
        }

        bool alive = IsWindow(cap.hwnd);
        if (cap.seenFrame == m_frame - 1 && alive &&
            (IsZoomed(cap.hwnd) || IsCloaked(cap.hwnd))) {
            ++it;
            continue;
        }
        if (cap.seenFrame == m_frame - 1) {
            DropColumns(cap, 0, static_cast<int>(cap.cols.size()), 6, true);
            it = m_caps.erase(it);
            continue;
        }
        it = alive ? std::next(it) : m_caps.erase(it);
    }
}

// Moves the snow from the other caps of the same window that were seen more
// recently than this one.
bool Overlay::InheritSnow(Cap& cap, unsigned lastSeen) {
    bool inherited = false;
    for (auto& [key, other] : m_caps) {
        if (&other == &cap || other.hwnd != cap.hwnd ||
            other.seenFrame != m_frame - 1 || other.seenFrame <= lastSeen) {
            continue;
        }

        for (size_t c = 0; c < other.cols.size(); c++) {
            int dst = other.rc.left + static_cast<int>(c) - cap.rc.left;
            if (dst >= 0 && dst < static_cast<int>(cap.cols.size())) {
                cap.cols[dst].h = std::max(cap.cols[dst].h, other.cols[c].h);
                other.cols[c].h = 0;
            }
        }
        inherited = true;
    }
    return inherited;
}

void Overlay::MoveCap(Cap& cap, const RECT& rc, float dt) {
    const float dTop = static_cast<float>(rc.top - cap.rc.top);
    const float prevVy = cap.vy;
    float vxNow = 0;
    if (rc.right - rc.left == cap.rc.right - cap.rc.left) {
        vxNow = (rc.left - cap.rc.left) / dt;
    }
    cap.vy = cap.vy * 0.5f + dTop / dt * 0.5f;
    cap.vx = cap.vx * 0.5f + vxNow * 0.5f;

    // Shaken from side to side: each quick turn throws some snow off, which
    // flies on the way the window was going.
    if (g_settings.shakeOff && !g_settings.slideOff) {
        const float shakeSpeed = 450.0f * m_scale;  // px/s
        cap.shakeTime += dt;
        if (std::fabs(cap.vx) > shakeSpeed) {
            float dir = cap.vx > 0 ? 1.0f : -1.0f;
            if (cap.shakeDir != 0 && dir != cap.shakeDir &&
                cap.shakeTime < 0.3f) {
                ShakeOff(cap, rc, cap.shakeDir * cap.shakeSpeed);
            }
            if (dir != cap.shakeDir) {
                cap.shakeSpeed = 0;
            }
            cap.shakeDir = dir;
            cap.shakeTime = 0;
            cap.shakeSpeed = std::max(cap.shakeSpeed, std::fabs(cap.vx));
        } else if (cap.shakeTime > 0.3f) {
            cap.shakeDir = 0;
        }
    }

    // Thrown up: the snow rides up with the body, and flies on when the body
    // slows down.
    const float s = m_scale;
    const float minToss = kMinTossSpeed * s, maxToss = kMaxTossSpeed * s;
    bool launch = false;
    float tossSpeed = 0;
    if (cap.vy < cap.carry) {
        cap.carry = cap.vy;
    } else if (cap.carry < -minToss && cap.vy > cap.carry * 0.35f) {
        launch = true;
        tossSpeed = std::max(cap.carry * kTossFactor, -maxToss);
        cap.carry = 0;
    } else if (cap.vy > -minToss * 0.5f) {
        cap.carry = 0;
    }

    // Pulled down faster than before (shrunk from the top, moved down): the
    // snow keeps its speed and falls after the body, and after a sudden jump
    // it hangs in the air for a moment.
    const float excess = dTop - std::max(prevVy, 0.0f) * dt;
    const bool drop = excess > 3.0f * s;
    const float hang =
        excess > 8.0f * s
            ? kHangSeconds *
                  std::clamp((excess - 8.0f * s) / (20.0f * s), 0.3f, 1.0f)
            : 0.0f;

    if (!cap.airborne && !launch && !drop) {
        return;
    }

    std::vector<Column>& cols = cap.cols;
    const int width = static_cast<int>(cols.size());
    float impact = 0;
    bool anyInAir = false;

    for (int c = 0; c < width; c++) {
        Column& col = cols[c];
        bool inAir = col.lift > 0 || col.vel < 0 || col.hang > 0;
        // All columns, even the empty ones, move together, so that the snow
        // in the air stays one connected shape.
        if (inAir) {
            // The snow in the air stays where it is while the body moves.
            col.lift += dTop;
        } else if (drop) {
            col.lift = dTop;
            col.vel = std::max(prevVy, 0.0f);
            col.hang = hang * (0.8f + 0.4f * CapNoise(cap, c, 3, 60.0f * s));
        } else if (launch) {
            // The cap flies up as one piece; loose bits break off below.
            col.vel = tossSpeed;
            col.lift = 0;
        } else {
            continue;
        }

        if (col.lift < 0) {
            // The body caught up with the snow from below and pushes it.
            col.lift = 0;
            col.hang = 0;
            col.vel = std::max(std::min(col.vel, cap.vy), -maxToss);
        }

        if (col.hang > 0) {
            col.hang -= dt;
            anyInAir = true;
            continue;
        }

        col.vel += kGravity * s * dt;
        col.lift -= col.vel * dt;
        if (col.lift <= 0) {
            col.lift = 0;
            if (col.vel > kBounceSpeed * s) {
                impact = std::max(impact, col.vel / s);
                if (col.vel > 550.0f * s && Rand() < 0.06f) {
                    SpawnPuff(rc.left + c + 0.5f, rc.top - col.h,
                              (Rand() - 0.5f) * 90.0f,
                              -40.0f - Rand() * 90.0f, 0.7f + Rand() * 0.8f,
                              0.5f + Rand() * 0.4f);
                }
                col.vel *= -0.2f;
            } else {
                col.vel = 0;
            }
        }
        if (col.lift > 0 || col.vel < 0) {
            anyInAir = true;
        }
    }
    cap.airborne = anyInAir;

    if (launch) {
        cap.squashVel += 1.5f;  // stretched by the throw

        // A few lumps and some powder break off and fly a bit higher than
        // the cap; a hard throw breaks off more.
        float breakShare =
            0.06f + std::clamp((-tossSpeed / s / kTossFactor - 900.0f) / 2500.0f,
                               0.0f, 0.25f);
        for (int c = 0; c + kChunkColumns <= width; c += kChunkColumns) {
            float top = 0;
            for (int k = c; k < c + kChunkColumns; k++) {
                top = std::max(top, cols[k].h);
            }
            if (top < 1.0f) {
                continue;
            }
            if (Rand() < breakShare) {
                float area = 0;
                for (int k = c; k < c + kChunkColumns; k++) {
                    area += cols[k].h * 0.35f;
                    cols[k].h *= 0.65f;
                }
                SpawnChunk(rc.left + c + 1.0f, rc.top - top, area,
                           cap.vx * 0.3f + (Rand() - 0.5f) * 100.0f,
                           tossSpeed * (1.1f + 0.4f * Rand()), 0);
            }
            if (Rand() < 0.04f) {
                SpawnPuff(rc.left + c + 1.0f, rc.top - top,
                          (Rand() - 0.5f) * 60.0f,
                          tossSpeed * (0.4f + 0.5f * Rand()),
                          0.6f + Rand() * 0.8f, 0.6f + Rand() * 0.5f);
            }
        }
    }

    if (impact > 0) {
        cap.squashVel -= std::min(impact * 0.003f, 4.0f);  // squashed
    }
}

// Throws part of the snow off a shaken window: the harder the shake, the more.
void Overlay::ShakeOff(Cap& cap, const RECT& rc, float vx) {
    std::vector<Column>& cols = cap.cols;
    const int width = static_cast<int>(cols.size());
    const float share =
        std::clamp(std::fabs(vx) / m_scale / 2500.0f, 0.25f, 0.7f);
    for (int c = 0; c + kChunkColumns <= width; c += kChunkColumns) {
        float area = 0, top = 0;
        for (int k = c; k < c + kChunkColumns; k++) {
            top = std::max(top, cols[k].h);
        }
        if (top < 0.5f || Rand() > share) {
            continue;
        }
        for (int k = c; k < c + kChunkColumns; k++) {
            area += cols[k].h;
            cols[k].h = 0;
        }
        SpawnChunk(rc.left + c + 1.0f, rc.top - top, area,
                   vx * (0.25f + 0.25f * Rand()), -60.0f - Rand() * 180.0f,
                   BodyKey(cap.hwnd, 0));
        if (Rand() < 0.15f) {
            SpawnPuff(rc.left + c + 1.0f, rc.top - top,
                      vx * 0.15f + (Rand() - 0.5f) * 60.0f,
                      -30.0f - Rand() * 60.0f, 0.6f + Rand() * 0.7f,
                      0.5f + Rand() * 0.5f);
        }
    }
    m_activeUntil = std::max(m_activeUntil, static_cast<double>(m_time) + 0.5);
}

// The snow melts slowly and unevenly: a full drift is gone in about two
// minutes once the snowfall stops.
void Overlay::Melt(Cap& cap, float dt) {
    const float rate = CapMax(cap) / 120.0f;
    for (size_t c = 0; c < cap.cols.size(); c++) {
        Column& col = cap.cols[c];
        if (col.h <= 0) {
            continue;
        }
        float unevenness =
            0.5f + CapNoise(cap, static_cast<int>(c), 4, 40.0f * m_scale);
        col.h = std::max(0.0f, col.h - rate * unevenness * dt);
        m_melting = true;
    }
}

// Small things that happen to the snow at rest: the wind blows powder off the
// drifts, now and then a lump slides off a window corner, and the snow melts
// if it's set to. The wind and the lumps come only while it snows, so that the
// snow lying still isn't redrawn.
void Overlay::Ambient(float dt) {
    const float wind = g_settings.wind * 0.6f * m_scale;
    const float powderRate = 0.4f + std::fabs(wind) / 30.0f;  // per 1000 px
    const bool snowing = g_settings.intensity > 0;
    const bool melt = g_settings.melt;
    m_melting = false;

    for (size_t i = 0; i < m_bodies.size(); i++) {
        const Body& b = m_bodies[i];
        // A maximized window keeps its cap for later, but the cap isn't drawn.
        if (!b.snowCap) {
            continue;
        }
        auto it = m_caps.find(b.key);
        if (it == m_caps.end()) {
            continue;
        }

        Cap& cap = it->second;
        const float maxHeight = CapMax(cap);

        // The squash and stretch of the whole cap settles like a spring.
        if (cap.squash != 0 || cap.squashVel != 0) {
            cap.squashVel += (-220.0f * cap.squash - 16.0f * cap.squashVel) * dt;
            cap.squash = std::clamp(cap.squash + cap.squashVel * dt, -0.4f,
                                    0.35f);
            if (std::fabs(cap.squash) < 0.002f &&
                std::fabs(cap.squashVel) < 0.02f) {
                cap.squash = cap.squashVel = 0;
            }
        }

        std::vector<Column>& cols = cap.cols;
        const int width = static_cast<int>(cols.size());
        if (cap.airborne) {
            continue;
        }
        if (melt) {
            Melt(cap, dt);
        }
        if (!snowing || width < 40) {
            continue;
        }

        if (Rand() < powderRate * width / 1000.0f * dt) {
            int c = static_cast<int>(Rand() * width);
            float x = static_cast<float>(b.rc.left + c);
            float y = b.rc.top - cols[c].h;
            if (cols[c].h > 0.6f * SnowLimit(cap, c, maxHeight, m_scale) &&
                !IsCoveredAbove(i, static_cast<int>(x),
                                static_cast<int>(y) - 1)) {
                SpawnPuff(x, y, wind * (0.5f + Rand()) + (Rand() - 0.5f) * 20.0f,
                          -15.0f - Rand() * 30.0f, 0.5f + Rand() * 0.7f,
                          1.0f + Rand() * 1.2f);
            }
        }

        // A lump slides off one of the corners.
        constexpr int kLump = 6;
        if (Rand() < 0.05f * dt) {
            bool right = Rand() < 0.5f;
            int first = right ? width - kLump - 2 : 2;
            float area = 0;
            for (int c = first; c < first + kLump; c++) {
                area += cols[c].h * 0.5f;
            }
            float x = static_cast<float>(right ? b.rc.right - 2 : b.rc.left + 2);
            if (area > kLump * 0.3f * maxHeight &&
                !IsCoveredAbove(i, static_cast<int>(x), b.rc.top - 2)) {
                for (int c = first; c < first + kLump; c++) {
                    cols[c].h *= 0.5f;
                }
                SpawnChunk(x, b.rc.top - 2.0f, area,
                           (right ? 1.0f : -1.0f) * (25.0f + Rand() * 30.0f),
                           -20.0f, b.key);
            }
        }
    }
}

void Overlay::Sweep(float dt) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    POINT last = m_lastCursor;
    m_lastCursor = pt;
    if (!m_cursorValid) {
        m_cursorValid = true;
        return;
    }
    if (!g_settings.sweepWithCursor || (pt.x == last.x && pt.y == last.y)) {
        return;
    }

    const float s = m_scale;
    const float cursorVx =
        std::clamp((pt.x - last.x) / dt, -1500.0f * s, 1500.0f * s);
    const int radius = static_cast<int>(std::lround(kSweepRadius * s));

    int x0 = std::min(last.x, pt.x) - radius;
    int x1 = std::max(last.x, pt.x) + radius;
    if (x1 - x0 > 300 * s) {
        // The cursor jumped; only sweep where it is now.
        x0 = pt.x - radius;
        x1 = pt.x + radius;
    }

    for (size_t i = 0; i < m_bodies.size(); i++) {
        const Body& b = m_bodies[i];
        // A maximized window keeps its cap for later, but the cap isn't drawn.
        if (!b.snowCap) {
            continue;
        }
        auto it = m_caps.find(b.key);
        if (it == m_caps.end()) {
            continue;
        }

        Cap& cap = it->second;
        const float maxHeight = CapMax(cap);
        const float top = static_cast<float>(b.rc.top);
        if (pt.y > top + 4 * s || pt.y < top - 400 * s || x1 < b.rc.left ||
            x0 >= b.rc.right || IsCoveredAbove(i, pt.x, b.rc.top - 2)) {
            continue;
        }

        // The cursor is near the snow: draw at the full frame rate so that
        // the sweep looks smooth.
        if (pt.y > top - maxHeight - 60 * s) {
            m_activeUntil =
                std::max(m_activeUntil, static_cast<double>(m_time) + 0.3);
        }

        std::vector<Column>& cols = cap.cols;
        int from = std::max(x0 - static_cast<int>(b.rc.left), 0);
        int to = std::min(x1 - static_cast<int>(b.rc.left),
                          static_cast<int>(cols.size()));
        for (int c = from; c < to; c += kChunkColumns) {
            float area = 0, height = 0;
            const float base = top - cols[c].lift;
            if (pt.y < base - maxHeight - 6 * s || pt.y > base + 4 * s) {
                continue;
            }
            for (int k = c; k < std::min(c + kChunkColumns, to); k++) {
                area += cols[k].h;
                height = std::max(height, cols[k].h);
                cols[k].h = 0;
            }
            if (area < 0.5f) {
                continue;
            }
            SpawnChunk(b.rc.left + c + 1.0f, base - height / 2, area,
                       cursorVx * 0.35f + (Rand() - 0.5f) * 80.0f,
                       -Rand() * 120.0f, b.key);
            if (Rand() < 0.35f) {
                SpawnPuff(b.rc.left + c + 1.0f, base - height,
                          cursorVx * 0.2f + (Rand() - 0.5f) * 60.0f,
                          -30.0f - Rand() * 60.0f, 0.6f + Rand() * 0.7f,
                          0.5f + Rand() * 0.5f);
            }
        }
    }
}

void Overlay::UpdateFlakes(float dt) {
    m_time += dt;

    // 100% is 150 flakes per 1920x1080 of screen area at 100% scale.
    size_t target = static_cast<size_t>(
        g_settings.intensity * 1.5 *
        (static_cast<double>(m_width) * m_height) /
        (1920.0 * 1080.0 * m_scale * m_scale));
    if (m_flakes.size() > target) {
        m_flakes.resize(target);
    }
    while (m_flakes.size() < target) {
        Flake f;
        SpawnFlake(f, true);
        m_flakes.push_back(f);
    }

    const float speedScale = g_settings.speed / 100.0f;
    const float wind = g_settings.wind * 0.6f * m_scale;
    const bool caps = g_settings.capHeight > 0;
    const float left = static_cast<float>(m_area.left);
    const float right = static_cast<float>(m_area.right);

    for (Flake& f : m_flakes) {
        float oldY = f.y;
        f.y += f.fallSpeed * speedScale * dt;
        f.x += (wind * (0.6f + 0.4f * f.size / (3.5f * m_scale)) +
                std::sin(m_time * 1.3f + f.phase) * f.sway) *
               dt;

        if (f.x > right + 10) {
            f.x -= m_width + 20;
        } else if (f.x < left - 10) {
            f.x += m_width + 20;
        }

        if (caps) {
            if (const Body* b = FindLanding(f.x, oldY, f.y, 0)) {
                // Each flake lands on ~9 columns, so this is tuned for a full
                // cap to build up in about half a minute.
                AddSnow(b->key, static_cast<int>(f.x) - b->rc.left,
                        2.0f * m_scale + 1.5f * f.size);
                SpawnFlake(f, false);
                continue;
            }
        }

        if (f.y > m_area.bottom + 5) {
            SpawnFlake(f, false);
        }
    }
}

void Overlay::UpdateChunks(float dt) {
    const float drag = std::exp(-2.0f * dt);
    size_t kept = 0;
    for (size_t i = 0; i < m_chunks.size(); i++) {
        Chunk c = m_chunks[i];
        float oldY = c.y;
        c.vy = std::min(c.vy + kGravity * 0.5f * m_scale * dt,
                        kChunkMaxSpeed * m_scale);
        c.vx *= drag;
        c.x += c.vx * dt;
        c.y += c.vy * dt;

        if (const Body* b = FindLanding(c.x, oldY, c.y, c.ignoreKey)) {
            if (c.burst) {
                // A lump bursts into powder; some of it stays.
                AddSnow(b->key, static_cast<int>(c.x) - b->rc.left,
                        c.area * 0.4f / kSnowKernelSum);
                int puffs = 2 + static_cast<int>(Rand() * 3);
                for (int k = 0; k < puffs; k++) {
                    SpawnPuff(c.x + (Rand() - 0.5f) * c.size * 3,
                              static_cast<float>(b->rc.top) - 2,
                              c.vx * 0.3f + (Rand() - 0.5f) * 120.0f,
                              -40.0f - Rand() * 80.0f,
                              0.6f + c.size * 0.25f + Rand() * 0.5f,
                              0.5f + Rand() * 0.5f);
                }
                continue;
            }
            AddSnow(b->key, static_cast<int>(c.x) - b->rc.left,
                    c.area / kSnowKernelSum);
            if (c.vy > 450.0f && Rand() < 0.5f) {
                SpawnPuff(c.x, static_cast<float>(b->rc.top) - 2,
                          c.vx * 0.3f + (Rand() - 0.5f) * 70.0f,
                          -30.0f - Rand() * 50.0f, 0.5f + c.size * 0.3f,
                          0.4f + Rand() * 0.3f);
            }
            continue;
        }
        if (c.y > m_area.bottom + 10 || c.x < m_area.left - 50 ||
            c.x > m_area.right + 50) {
            continue;
        }
        m_chunks[kept++] = c;
    }
    m_chunks.resize(kept);
}

void Overlay::UpdatePuffs(float dt) {
    const float drag = std::exp(-3.0f * dt);
    const float wind = g_settings.wind * 0.6f * m_scale;
    size_t kept = 0;
    for (size_t i = 0; i < m_puffs.size(); i++) {
        Puff p = m_puffs[i];
        p.life -= dt;
        if (p.life <= 0) {
            continue;
        }
        p.vx = p.vx * drag + wind * (1.0f - drag);
        p.vy = p.vy * drag + 40.0f * m_scale * dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        m_puffs[kept++] = p;
    }
    m_puffs.resize(kept);
}

void Overlay::DrawCap(const Cap& cap, const RECT& rc) {
    const std::vector<Column>& cols = cap.cols;
    int width = static_cast<int>(cols.size());
    if (width < 2) {
        return;
    }

    float highest = 0, liftSum = 0;
    int snowy = 0;
    for (const Column& col : cols) {
        highest = std::max(highest, col.h);
        if (col.h > 0.2f) {
            liftSum += col.lift;
            snowy++;
        }
    }
    if (highest < 0.2f) {
        return;
    }

    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(m_d2dFactory->CreatePathGeometry(&path)) ||
        FAILED(path->Open(&sink))) {
        return;
    }

    // The screen shows windows a frame later than they're reported, so the
    // snow is drawn a frame ahead while a window moves. Otherwise it would
    // trail behind, and sink into the window when it moves up.
    // Snow that slides off moved windows stays still on the screen.
    const float dx = g_settings.slideOff
                         ? 0.0f
                         : std::clamp(cap.vx * m_frameTime, -40.0f, 40.0f);
    const float dy = -std::min(std::fabs(cap.vy) * m_frameTime, 40.0f);
    const float left = rc.left + dx;
    const float top = rc.top + dy;
    const float scale = 1.0f + cap.squash;

    auto height = [&](int c) {
        // Round off the cap at the window corners.
        float edge = static_cast<float>(std::min(c, width - 1 - c));
        return cols[c].h * scale *
               std::clamp(edge / (kCapEdgeTaper * m_scale), 0.0f, 1.0f);
    };

    // One figure per run of snow, so that bare stretches aren't drawn as thin
    // lines while the snow is in the air.
    constexpr float kMinHeight = 0.3f;
    for (int c = 0; c < width;) {
        if (height(c) < kMinHeight) {
            c++;
            continue;
        }
        int first = c;
        while (c < width && height(c) >= kMinHeight) {
            c++;
        }
        int last = c - 1;

        float x0 = left + first;
        float x1 = left + last + 1.0f;
        sink->BeginFigure({x0, top - cols[first].lift},
                          D2D1_FIGURE_BEGIN_FILLED);
        for (int k = first; k <= last; k += 2) {
            sink->AddLine({left + k + 0.5f, top - cols[k].lift - height(k)});
        }
        sink->AddLine({x1 - 0.5f, top - cols[last].lift - height(last)});
        sink->AddLine({x1, top - cols[last].lift});
        if (cap.airborne) {
            for (int k = last; k >= first; k -= 2) {
                sink->AddLine({left + k + 0.5f, top - cols[k].lift});
            }
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    }
    if (FAILED(sink->Close())) {
        return;
    }

    const float base = top - (snowy ? liftSum / snowy : 0.0f);
    m_capBrush->SetStartPoint({0, base - highest * scale});
    m_capBrush->SetEndPoint({0, base});
    m_ctx->FillGeometry(path.Get(), m_capBrush.Get());
}

bool Overlay::Render(UINT syncInterval) {
    m_ctx->BeginDraw();
    m_ctx->SetTransform(D2D1::Matrix3x2F::Translation(
        static_cast<float>(-m_area.left), static_cast<float>(-m_area.top)));
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    // Draw from the bottom window to the top one. Each window first erases the
    // snow of the windows below it that it covers.
    bool anyDrawn = false;
    for (size_t i = m_bodies.size(); i-- > 0;) {
        const Body& b = m_bodies[i];
        if (anyDrawn) {
            m_ctx->PushAxisAlignedClip(
                D2D1::RectF(static_cast<float>(b.rc.left),
                            static_cast<float>(b.rc.top),
                            static_cast<float>(b.rc.right),
                            static_cast<float>(b.rc.bottom)),
                D2D1_ANTIALIAS_MODE_ALIASED);
            m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));
            m_ctx->PopAxisAlignedClip();
        }

        if (!b.snowCap) {
            continue;
        }
        auto it = m_caps.find(b.key);
        if (it != m_caps.end()) {
            DrawCap(it->second, b.rc);
            anyDrawn = true;
        }
    }

    for (const Chunk& c : m_chunks) {
        float r = c.size * 2.0f;
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(c.x - r, c.y - r, c.x + r, c.y + r), 0.95f,
                          D2D1_INTERPOLATION_MODE_LINEAR);
    }

    for (const Puff& p : m_puffs) {
        float r = p.size * 2.0f;
        float alpha = 0.7f * std::min(1.0f, 2.5f * p.life / p.maxLife);
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(p.x - r, p.y - r, p.x + r, p.y + r),
                          alpha, D2D1_INTERPOLATION_MODE_LINEAR);
    }

    for (const Flake& f : m_flakes) {
        float r = f.size * 2.0f;
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(f.x - r, f.y - r, f.x + r, f.y + r),
                          f.alpha, D2D1_INTERPOLATION_MODE_LINEAR);
    }

    if (FAILED(m_ctx->EndDraw())) {
        return false;
    }

    HRESULT hr = m_swapChain->Present(syncInterval, 0);
    return SUCCEEDED(hr) || hr == DXGI_STATUS_OCCLUDED;
}

// One step of the snow.
void Overlay::Step(float dt) {
    m_frameTime = m_frameTime * 0.9f + dt * 0.1f;
    m_frame++;

    UpdateCaps(dt);
    Ambient(dt);
    Sweep(dt);
    UpdateFlakes(dt);
    UpdateChunks(dt);
    UpdatePuffs(dt);
}

RunResult Overlay::Run() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    auto clock = [&freq]() {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return static_cast<double>(t.QuadPart) / freq.QuadPart;
    };

    // A high resolution timer exists since Windows 10 version 1803.
    struct TimerHandle {
        HANDLE handle;
        ~TimerHandle() {
            if (handle) {
                CloseHandle(handle);
            }
        }
        HANDLE get() const { return handle; }
    } timer{CreateWaitableTimerEx(nullptr, nullptr,
                                  CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                  TIMER_ALL_ACCESS)};
    if (!timer.handle) {
        timer.handle = CreateWaitableTimer(nullptr, FALSE, nullptr);
    }

    double lastStep = clock();
    double nextStep = lastStep;
    double nextCheck = 0;    // the checks done once a second
    double nextCollect = 0;  // windows are listed again at least this often
    double nextRestack = 0;
    double nextLayerSearch = lastStep + 5.0;
    bool waitingForFrame = false;
    int fastFps = g_settings.maxFps;
    int calmFps = std::min(fastFps, 30);

    for (;;) {
        double now = clock();
        if (!waitingForFrame && (g_wake.windows || g_wake.cursor)) {
            nextStep = std::min(nextStep, lastStep + 1.0 / fastFps);
        }

        if (waitingForFrame || now < nextStep) {
            HANDLE handles[2] = {m_stop, nullptr};
            DWORD count = 1;
            DWORD timeout;
            if (waitingForFrame) {
                handles[count++] = m_frameWait;
                timeout = 200;
            } else {
                timeout =
                    static_cast<DWORD>(std::ceil((nextStep - now) * 1000.0));
                // A timeout is only as precise as the system timer (often
                // 15.6 ms), too coarse for frame pacing.
                LARGE_INTEGER due;
                due.QuadPart = -static_cast<LONGLONG>((nextStep - now) * 1e7);
                if (timer.get() &&
                    SetWaitableTimer(timer.get(), &due, 0, nullptr, nullptr,
                                     FALSE)) {
                    handles[count++] = timer.get();
                    timeout += 50;
                }
            }
            DWORD result = MsgWaitForMultipleObjects(count, handles, FALSE,
                                                     timeout, QS_ALLINPUT);
            if (result == WAIT_OBJECT_0) {
                return RunResult::Stop;
            }
            if (waitingForFrame &&
                (result == WAIT_OBJECT_0 + 1 || result == WAIT_TIMEOUT)) {
                waitingForFrame = false;
            }

            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            continue;
        }

        if (now >= nextCheck) {
            nextCheck = now + 1.0;

            if (!IsWindow(m_hwnd) ||
                (m_behind && !IsWindow(m_layer.parent))) {
                // Explorer restarted and took the wallpaper layer with it.
                Wh_Log(L"The overlay window is gone");
                return RunResult::Retry;
            }
            if (g_settings.behindWindows != m_wantBehind) {
                return RunResult::Recreate;
            }
            if (m_wantBehind && !m_behind && now >= nextLayerSearch) {
                nextLayerSearch = now + 5.0;
                DesktopLayer layer;
                if (FindDesktopLayer(&layer)) {
                    return RunResult::Recreate;
                }
            }

            UpdateRefreshRate();
            ApplyCaptureSetting();
            Restack();

            // On battery, faster frame rates are hard to tell apart in snow
            // anyway; in energy saver mode the snow is drawn less often.
            fastFps = g_settings.maxFps;
            calmFps = 30;
            SYSTEM_POWER_STATUS power;
            if (GetSystemPowerStatus(&power)) {
                if (power.ACLineStatus == 0) {
                    fastFps = std::min(fastFps, 60);
                }
                if (power.SystemStatusFlag & 1) {
                    fastFps = std::min(fastFps, 30);
                    calmFps = 20;
                }
            }
            calmFps = std::min(calmFps, fastFps);
        }

        if (g_wake.restack && now >= nextRestack) {
            g_wake.restack = false;
            nextRestack = now + 0.1;
            Restack();
        }

        // Nobody can see the snow while the screen is locked or off.
        bool paused = g_locked || g_displayOff;
        bool fullscreen =
            !paused && g_settings.pauseInFullscreen && IsFullscreenAppActive();
        bool visible = !paused && !fullscreen;
        if (visible != m_visible) {
            m_visible = visible;
            g_visibleOverlays += m_visible ? 1 : -1;
            ShowWindow(m_hwnd, m_visible ? SW_SHOWNOACTIVATE : SW_HIDE);
            m_cursorValid = false;
            m_pace = Pace::Calm;  // draw at least one frame when it's back
            if (m_visible) {
                Restack();
            }
        }
        if (!m_visible) {
            g_wake.windows = g_wake.cursor = false;
            lastStep = now;
            nextStep = now + (paused ? 0.25 : 0.1);
            continue;
        }

        float dt =
            std::clamp(static_cast<float>(now - lastStep), 0.001f, 0.05f);
        lastStep = now;

        // The windows are listed again when they change, and twice a second
        // in case a change went unreported (or the taskbar's parts changed).
        bool windowsChanged = g_wake.windows;
        g_wake.windows = g_wake.cursor = false;
        if (windowsChanged || now >= nextCollect) {
            nextCollect = now + 0.5;
            CollectBodies();
        }
        bool bodiesChanged = !SameBodies(m_bodies, m_prevBodies);
        if (bodiesChanged) {
            m_prevBodies = m_bodies;
        }

        Step(dt);

        if (bodiesChanged) {
            m_activeUntil =
                std::max(m_activeUntil, static_cast<double>(m_time) + 0.3);
        }

        Pace pace;
        if (!m_chunks.empty() || m_anyAirborne ||
            static_cast<double>(m_time) < m_activeUntil) {
            pace = Pace::Interactive;
        } else if (!m_flakes.empty() || !m_puffs.empty() || m_melting) {
            pace = Pace::Calm;
        } else {
            pace = Pace::Still;
        }

        // Snow that lies still isn't drawn again: the last frame stays on
        // the screen. One more frame is drawn after things stop moving.
        bool draw = pace != Pace::Still || m_pace != Pace::Still;
        m_pace = pace;
        if (!draw) {
            nextStep = now + 0.1;
            continue;
        }

        const int fps = pace == Pace::Calm ? calmFps : fastFps;
        UINT sync = SyncIntervalFor(fps);
        if (!Render(sync)) {
            // The GPU device was lost (driver update, adapter change).
            Wh_Log(L"Recreating device resources");
            ReleaseDeviceResources();
            if (!CreateDeviceResources()) {
                Wh_Log(L"Failed to recreate device resources");
                return RunResult::Retry;
            }
            waitingForFrame = false;
            nextStep = now;
            continue;
        }

        // The next frame is made once this one is shown, after `sync`
        // refreshes of the display, and not sooner than the frame rate allows:
        // on a fast display, even the longest sync interval may be too short.
        // The small margin keeps a frame that's due at the same refresh from
        // waiting for the next one.
        const double gap = 1.0 / fps - 0.002;
        waitingForFrame = m_frameWait != nullptr;
        nextStep = now + (waitingForFrame ? gap
                                          : std::max(gap, sync / m_refreshRate));
    }
}

////////////////////////////////////////////////////////////////////////////////
// What the snow listens to: window events (so that it reacts at once to
// windows moving, and doesn't have to look at all windows on every frame), and
// the screen being locked or turned off.

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                           LONG idChild, DWORD, DWORD) {
    if (idObject == OBJID_CURSOR) {
        g_wake.cursor = true;
        return;
    }
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) {
        return;
    }

    // Location changes come for every control and caret too; only top-level
    // windows matter. A window that's already gone may have had snow on it.
    if (IsWindow(hwnd)) {
        if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
            return;
        }
    } else if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        return;
    }

    g_wake.windows = true;
    if (event == EVENT_SYSTEM_FOREGROUND || event == EVENT_OBJECT_SHOW) {
        g_wake.restack = true;
    }
}

constexpr wchar_t kListenerClass[] = L"WindhawkDesktopSnowfallListener";

// GUID_CONSOLE_DISPLAY_STATE, defined here to avoid linking with uuid.lib.
constexpr GUID kGuidConsoleDisplayState = {
    0x6fe69556, 0x704a, 0x47a0, {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};

LRESULT CALLBACK ListenerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam) {
    switch (msg) {
        case WM_WTSSESSION_CHANGE:
            if (wParam == WTS_SESSION_LOCK) {
                g_locked = true;
            } else if (wParam == WTS_SESSION_UNLOCK) {
                g_locked = false;
            }
            return 0;

        case WM_POWERBROADCAST:
            if (wParam == PBT_POWERSETTINGCHANGE) {
                auto* setting =
                    reinterpret_cast<const POWERBROADCAST_SETTING*>(lParam);
                if (IsEqualGUID(setting->PowerSetting,
                                kGuidConsoleDisplayState) &&
                    setting->DataLength >= sizeof(DWORD)) {
                    // 0 is off, 1 is on, 2 is dimmed.
                    g_displayOff =
                        *reinterpret_cast<const DWORD*>(setting->Data) == 0;
                }
            }
            return TRUE;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Waits while handling messages. Returns true if `stop` was set.
static bool WaitForStop(HANDLE stop, DWORD ms) {
    DWORD start = GetTickCount();
    for (;;) {
        DWORD elapsed = GetTickCount() - start;
        if (elapsed >= ms) {
            return WaitForSingleObject(stop, 0) == WAIT_OBJECT_0;
        }
        DWORD result =
            MsgWaitForMultipleObjects(1, &stop, FALSE, ms - elapsed, QS_ALLINPUT);
        if (result == WAIT_OBJECT_0) {
            return true;
        }
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

struct MonitorSnow {
    HMONITOR monitor;
    RECT area;
    UINT dpi;
    HANDLE stop;  // set when this monitor's snow should end
};

// The monitors that get snow: all of them, or only the main one.
static std::vector<MonitorSnow> ListMonitors(bool all) {
    std::vector<MonitorSnow> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM lParam) WINAPI -> BOOL {
            auto* list = reinterpret_cast<std::vector<MonitorSnow>*>(lParam);
            MONITORINFO mi = {sizeof(mi)};
            if (!GetMonitorInfo(monitor, &mi)) {
                return TRUE;
            }
            UINT dpiX = 96, dpiY = 96;
            GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
            MonitorSnow m = {monitor, mi.rcMonitor, dpiX, nullptr};
            if (mi.dwFlags & MONITORINFOF_PRIMARY) {
                list->insert(list->begin(), m);
            } else {
                list->push_back(m);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));
    if (!all && monitors.size() > 1) {
        monitors.resize(1);
    }
    return monitors;
}

static bool SameMonitors(const std::vector<MonitorSnow>& a,
                         const std::vector<MonitorSnow>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].monitor != b[i].monitor || !EqualRect(&a[i].area, &b[i].area) ||
            a[i].dpi != b[i].dpi) {
            return false;
        }
    }
    return true;
}

// The snow of one monitor. Each monitor has its own thread, so that a monitor
// with a higher refresh rate doesn't hold back the others.
DWORD WINAPI OverlayThread(LPVOID param) {
    const MonitorSnow monitor = *static_cast<MonitorSnow*>(param);

    // The events come to the thread that asked for them.
    static const DWORD kEventRanges[][2] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    std::vector<HWINEVENTHOOK> hooks;
    for (const auto& range : kEventRanges) {
        if (HWINEVENTHOOK hook = SetWinEventHook(
                range[0], range[1], nullptr, WinEventProc, 0, 0,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS)) {
            hooks.push_back(hook);
        }
    }

    for (;;) {
        RunResult result = RunResult::Retry;
        {
            Overlay overlay(monitor.monitor, monitor.area, monitor.stop);
            if (overlay.Create()) {
                result = overlay.Run();
            } else {
                Wh_Log(L"Failed to create the overlay");
            }
        }
        if (result == RunResult::Stop) {
            break;
        }
        // Explorer may be restarting: try again in a moment.
        if (WaitForStop(monitor.stop, result == RunResult::Retry ? 2000 : 0)) {
            break;
        }
    }

    for (HWINEVENTHOOK hook : hooks) {
        UnhookWinEvent(hook);
    }
    return 0;
}

// Starts the snow on each monitor, and starts it anew when the monitors change
// (added, removed, moved, resized or rescaled).
DWORD WINAPI SnowThread(LPVOID) {

    g_taskbarThread = CreateThread(nullptr, 0, TaskbarThread, nullptr, 0,
                                   nullptr);

    HINSTANCE instance = GetModuleHandle(nullptr);
    WNDCLASSEX wc = {sizeof(wc)};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = kOverlayClass;
    RegisterClassEx(&wc);

    wc.lpfnWndProc = ListenerWndProc;
    wc.lpszClassName = kListenerClass;
    RegisterClassEx(&wc);
    HWND listener =
        CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kListenerClass,
                       L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance,
                       nullptr);
    HPOWERNOTIFY powerNotify = nullptr;
    if (listener) {
        WTSRegisterSessionNotification(listener, NOTIFY_FOR_THIS_SESSION);
        powerNotify = RegisterPowerSettingNotification(
            listener, &kGuidConsoleDisplayState, DEVICE_NOTIFY_WINDOW_HANDLE);
    }

    bool stopping = false;
    while (!stopping) {
        bool all = g_settings.allMonitors;
        std::vector<MonitorSnow> monitors = ListMonitors(all);
        HANDLE stop = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        std::vector<HANDLE> threads;
        if (stop) {
            for (MonitorSnow& m : monitors) {
                m.stop = stop;
                if (HANDLE thread = CreateThread(nullptr, 0, OverlayThread, &m,
                                                 0, nullptr)) {
                    threads.push_back(thread);
                }
            }
        }
        Wh_Log(L"Snow on %d monitor(s)", static_cast<int>(threads.size()));

        for (;;) {
            if (WaitForStop(g_stopEvent, 1000)) {
                stopping = true;
                break;
            }
            if (g_settings.allMonitors != all ||
                !SameMonitors(ListMonitors(all), monitors)) {
                Wh_Log(L"The monitors changed");
                break;
            }
        }

        if (stop) {
            SetEvent(stop);
        }
        for (HANDLE thread : threads) {
            // Keep handling messages: a thread that's ending may be waiting
            // for a message sent to a window of this thread.
            while (MsgWaitForMultipleObjects(1, &thread, FALSE, INFINITE,
                                             QS_ALLINPUT) != WAIT_OBJECT_0) {
                MSG msg;
                while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            CloseHandle(thread);
        }
        if (stop) {
            CloseHandle(stop);
        }
    }

    if (listener) {
        if (powerNotify) {
            UnregisterPowerSettingNotification(powerNotify);
        }
        WTSUnRegisterSessionNotification(listener);
        DestroyWindow(listener);
    }
    UnregisterClass(kListenerClass, instance);
    UnregisterClass(kOverlayClass, instance);

    if (g_taskbarThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_taskbarThread, INFINITE);
        CloseHandle(g_taskbarThread);
        g_taskbarThread = nullptr;
    }
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_snowThread = CreateThread(nullptr, 0, SnowThread, nullptr, 0, nullptr);
    if (!g_snowThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_snowThread, INFINITE);
    CloseHandle(g_snowThread);
    CloseHandle(g_stopEvent);
    g_snowThread = nullptr;
    g_stopEvent = nullptr;
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
