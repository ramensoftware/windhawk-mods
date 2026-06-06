// ==WindhawkMod==
// @id             fluid-window-engine-pro
// @name           Fluid Window Engine Pro
// @description    Cinema-grade window animations for Windows 10 / 11. 9 physics-based effects, language-independent taskbar targeting, multi-monitor, tunable per-effect — make every minimize, restore, and close beautiful.
// @version        2.0.0
// @author         kivsak (original engine) + Claude (refactor)
// @github         https://github.com/kivsak
// @include        *
// @compilerOptions -ldwmapi -lgdi32 -lmsimg32 -luser32 -loleacc -loleaut32 -lole32 -luuid -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluid Window Engine Pro — v2.0

**Cinema-grade window animations for Windows 10 / 11.**

Transform mundane minimize/restore/close transitions into smooth, physics-based effects inspired by macOS, KDE Plasma, and Material Design — engineered specifically for Windows. Each animation is fully reversible and runs on the monitor where the window lives.

## What you get

Click X, hit Win+Down, or minimize via the title bar — instead of the boring "vanish-to-icon", a tasteful animation plays. A droplet flows into your taskbar icon. A particle vortex spirals into oblivion. Glass shatters and falls. Sand pours through the air. Pick your effect, tune it your way.

## The 9 effects

| Effect | Vibe |
|---|---|
| 🌀 **Genie** | macOS-style stretch into your taskbar icon |
| 💧 **Fluid** | Soft-body droplet — tunable from rubber to oil |
| 📐 **Scale** | Clean shrink toward icon (Windows-default style) |
| 🔍 **Zoom** | Focal-point scale with optional overshoot |
| 💨 **Swipe** | Material 3 directional fly-off, Android-smooth |
| 📺 **CRT** | Retro TV — vertical collapse, horizontal pinch |
| ⚫ **Black Hole** | True particle vortex with motion blur |
| ⏳ **Sand** | Physics grain dissolve — crumble or firework burst |
| 💎 **Glass** | Irregular polygon shatter with rotation |

## What sets it apart

- **Language-independent taskbar targeting** — animations actually fly into YOUR app's icon, not a random spot. Works on any Windows locale (English, Ukrainian, Russian, …). Matches via exe path, version-resource FileDescription, alias map, and fuzzy title scoring — not just the icon's localized label.
- **Tunable physics** per effect. Fluid liquidity from rigid rubber to flowing oil. Sand from straight crumble to fireworks burst. Glass shard count, shape, gravity, rotation.
- **Multi-monitor first-class** — each animation lives on the right monitor, even with secondary taskbars (`Shell_SecondaryTrayWnd`).
- **Per-effect bounding box** rendering — no full-screen blits. Smooth 60 FPS even on 4K.
- **Fully reversible** — minimize and restore use mirror-perfect physics.

## What's new in v2.0

- **Total taskbar targeting rewrite** — multi-language alias map, FileDescription matching, similarity scoring. Picks the BEST candidate, not the first that matches.
- **Fluid seam fix** — smooth body↔tail opacity blend. No more visible joint between droplet body and tail.
- **Black Hole clean separation** — strict base→particles transition, no overlap artifacts on minimize.
- **Glass FPS** — incremental edge functions, ~3× faster inner rasterizer.
- **Sand** — true ballistic structure (restX, restY, vx, vy), upward kick for explosion mode.
- **Adaptive particle resolution** — Black Hole auto-scales `bSize` for 1440p / 4K.

## Tuning tips

- Open Windhawk's settings UI — every parameter has a description.
- `dock_offset_y` is a manual Y-tuner (negative = higher target). Helps if your taskbar height differs from defaults.
- `fluid_liquidity 0 = rubber, 100 = oil` — controls smoothness, corner roundness, tail thickness, wobble strength, morph delay, taper steepness all at once.
- `fluid_edgeInset` skips N pixels around source edges — useful for Windows 11 apps with transparent shadows you don't want dragged.
- `swipe_smoothness 30` ≈ Android Material 3 standard; `60+` for emphasized acceleration.
- `glass_pieces` and `sand_blockSize` directly affect FPS; lower them on slower machines.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

# ═════════════════════════════════════════════════
# 🎬 GLOBAL
# ═════════════════════════════════════════════════

- minMode: fluid
  $name: "🔽 Minimize / Restore effect"
  $description: "Which animation plays when a window is minimized to taskbar or restored back."
  $options:
    - none:      "❌ Disabled"
    - genie:     "🌀 Genie (macOS stretch)"
    - fluid:     "💧 Fluid (droplet physics)"
    - scale:     "📐 Scale (default shrink to icon)"
    - zoom:      "🔍 Zoom (center focus)"
    - swipe:     "💨 Swipe (Material 3 fly)"
    - crt:       "📺 CRT (retro collapse)"
    - blackhole: "⚫ Black Hole (vortex)"
    - sand:      "⏳ Sand (grain dissolve)"
    - glass:     "💎 Glass (shatter)"

- closeMode: glass
  $name: "❌ Close window effect"
  $description: "Animation when user closes a window via X button or Alt+F4."
  $options:
    - none:      "❌ Disabled"
    - zoom:      "🔍 Zoom (center focus)"
    - swipe:     "💨 Swipe (Material 3 fly)"
    - crt:       "📺 CRT (retro collapse)"
    - blackhole: "⚫ Black Hole (vortex)"
    - sand:      "⏳ Sand (grain dissolve)"
    - glass:     "💎 Glass (shatter)"

- dock_offset_y: -15
  $name: "⚙️ Dock Y-offset (px)"
  $description: "Manual vertical adjustment of where animations target the taskbar icon. Negative = higher, positive = lower. Applies to minimize only, not close."

# ═════════════════════════════════════════════════
# 🌀 GENIE
# ═════════════════════════════════════════════════

- genie_minDur: 400
  $name: "🌀 [Genie] Minimize duration (ms)"
  $description: "How long the window stretches into the taskbar."
- genie_resDur: 350
  $name: "🌀 [Genie] Restore duration (ms)"
- genie_bounceOn: true
  $name: "🌀 [Genie] Restore bounce"
  $description: "Adds a spring overshoot when window pops back from taskbar."
- genie_bounceStr: 30
  $name: "🌀 [Genie] Bounce strength (0–100)"
- genie_bounceDur: 300
  $name: "🌀 [Genie] Bounce duration (ms)"
- genie_tailWidth: 12
  $name: "🌀 [Genie] Tail width at dock (px)"
  $description: "How narrow the bottom of the genie tail becomes at the taskbar icon."
- genie_tailOpacity: 5
  $name: "🌀 [Genie] Tail tip opacity (%)"
  $description: "Transparency of the very bottom of the tail. 0 = transparent, 100 = solid."
- genie_quality: 2
  $name: "🌀 [Genie] Render quality (1–3)"
  $description: "Supersampling factor for anti-aliasing. 1 = fast, 3 = max quality (slowest)."

# ═════════════════════════════════════════════════
# 💧 FLUID  (renamed from Jelly)
# ═════════════════════════════════════════════════

- fluid_minDur: 550
  $name: "💧 [Fluid] Minimize duration (ms)"
- fluid_resDur: 550
  $name: "💧 [Fluid] Restore duration (ms)"
- fluid_bounceOn: true
  $name: "💧 [Fluid] Impact wobble"
  $description: "Soft vibration when the droplet lands or pops out."
- fluid_bounceStr: 60
  $name: "💧 [Fluid] Wobble strength (0–100)"
- fluid_bounceDur: 500
  $name: "💧 [Fluid] Wobble duration (ms)"
- fluid_liquidity: 65
  $name: "💧 [Fluid] Liquidity (0–100)"
  $description: "How liquid the droplet feels. 0 = rigid (sharp corners, hard blends), 100 = pure liquid (soft merges between blob, tail and catchup drop)."
- fluid_morphStart: 70
  $name: "💧 [Fluid] Square-morph start (%)"
  $description: "Progress at which the droplet starts morphing back into the original rectangle shape."
- fluid_tailCatchup: 40
  $name: "💧 [Fluid] Tail detach timing (%)"
  $description: "When the tail starts catching up to the head (%)."
- fluid_tailWidth: 4
  $name: "💧 [Fluid] Tail tip width (px)"
- fluid_dropProfile: 22
  $name: "💧 [Fluid] Tail profile curve (5–40)"
  $description: "Shape of the tail taper. Lower = gradual taper, higher = sharper neck."
- fluid_blobRadius: 15
  $name: "💧 [Fluid] Trailing drop radius (px)"
  $description: "Max size of the secondary blob that follows the head."
- fluid_blobTiming: 45
  $name: "💧 [Fluid] Trailing drop start (%)"
- fluid_tailFadeLength: 50
  $name: "💧 [Fluid] Tail fade length (%)"
  $description: "How much of the tail's tip is transparent. 0 = no fade, 100 = full half of tail fades."
- fluid_edgeInset: 5
  $name: "💧 [Fluid] Source edge inset (px)"
  $description: "How many pixels to ignore at the source window's edges when sampling. Prevents grabbing transparent borders or shadows — increase if the droplet appears to drag a transparent frame."

# ═════════════════════════════════════════════════
# 📐 SCALE  (default minimize, renamed from Vanilla)
# ═════════════════════════════════════════════════

- scale_minDur: 350
  $name: "📐 [Scale] Minimize duration (ms)"
- scale_resDur: 300
  $name: "📐 [Scale] Restore duration (ms)"
- scale_bounceOn: true
  $name: "📐 [Scale] Restore bounce"
- scale_bounceStr: 30
  $name: "📐 [Scale] Bounce strength (0–100)"
- scale_bounceDur: 300
  $name: "📐 [Scale] Bounce duration (ms)"
- scale_iconSize: 48
  $name: "📐 [Scale] Target icon size (px)"
  $description: "Final window size when it reaches the taskbar."
- scale_fadeStart: 30
  $name: "📐 [Scale] Fade-out start (%)"
  $description: "Progress at which the window starts fading. 0 = fade from start, 100 = no fade."

# ═════════════════════════════════════════════════
# 🔍 ZOOM
# ═════════════════════════════════════════════════

- zoom_minDur: 300
  $name: "🔍 [Zoom] Minimize duration (ms)"
- zoom_resDur: 300
  $name: "🔍 [Zoom] Restore duration (ms)"
- zoom_closeDur: 300
  $name: "🔍 [Zoom] Close duration (ms)"
- zoom_bounceOn: true
  $name: "🔍 [Zoom] Restore bounce"
- zoom_bounceStr: 30
  $name: "🔍 [Zoom] Bounce strength (0–100)"
- zoom_bounceDur: 300
  $name: "🔍 [Zoom] Bounce duration (ms)"
- zoom_fadeStart: 10
  $name: "🔍 [Zoom] Fade-out start (%)"

# ═════════════════════════════════════════════════
# 💨 SWIPE  (Material 3 easing)
# ═════════════════════════════════════════════════

- swipe_dur: 320
  $name: "💨 [Swipe] Duration (ms)"
- swipe_dir: random
  $name: "💨 [Swipe] Direction"
  $description: "Which way the window flies."
  $options:
    - down:   "⬇ Down"
    - up:     "⬆ Up"
    - left:   "⬅ Left"
    - right:  "➡ Right"
    - random: "🎲 Random per window"
- swipe_distance: 500
  $name: "💨 [Swipe] Fly distance (px)"
- swipe_smoothness: 30
  $name: "💨 [Swipe] Smoothness (0–100)"
  $description: "Easing character. 0 = nearly linear, 30 = Material 3 standard (smooth Android-like), 60 = emphasized accelerate, 100 = snap to oblivion."
- swipe_fadeStrength: 80
  $name: "💨 [Swipe] Fade strength (0–100)"
  $description: "How transparent the window becomes by the time it leaves. 0 = stays fully visible, 100 = full fade to transparent."

# ═════════════════════════════════════════════════
# 📺 CRT
# ═════════════════════════════════════════════════

- crt_dur: 350
  $name: "📺 [CRT] Duration (ms)"
  $description: "Single duration for minimize, restore and close (CRT has only one tempo)."

# ═════════════════════════════════════════════════
# ⚫ BLACK HOLE
# ═════════════════════════════════════════════════

- blackhole_minDur: 450
  $name: "⚫ [Black Hole] Minimize duration (ms)"
- blackhole_resDur: 400
  $name: "⚫ [Black Hole] Restore duration (ms)"
- blackhole_closeDur: 400
  $name: "⚫ [Black Hole] Close duration (ms)"
- blackhole_bounceOn: false
  $name: "⚫ [Black Hole] Restore bounce"
- blackhole_bounceStr: 20
  $name: "⚫ [Black Hole] Bounce strength (0–100)"
- blackhole_bounceDur: 300
  $name: "⚫ [Black Hole] Bounce duration (ms)"
- blackhole_twist: 25
  $name: "⚫ [Black Hole] Spiral twist (0–50)"
  $description: "How sharply particles spiral toward the center."
- blackhole_quality: 2
  $name: "⚫ [Black Hole] Particle size (1=large, 2=med, 3=fine)"

# ═════════════════════════════════════════════════
# ⏳ SAND
# ═════════════════════════════════════════════════

- sand_minDur: 600
  $name: "⏳ [Sand] Minimize duration (ms)"
- sand_resDur: 500
  $name: "⏳ [Sand] Restore duration (ms)"
- sand_closeDur: 600
  $name: "⏳ [Sand] Close duration (ms)"
- sand_bounceOn: false
  $name: "⏳ [Sand] Restore bounce"
- sand_bounceStr: 10
  $name: "⏳ [Sand] Bounce strength (0–100)"
- sand_bounceDur: 300
  $name: "⏳ [Sand] Bounce duration (ms)"
- sand_style: 50
  $name: "⏳ [Sand] Style (0=crumble, 100=explosion)"
  $description: "0 = grains fall straight down. 100 = explosive radial outburst."
- sand_gravity: 1200
  $name: "⏳ [Sand] Fall speed"
- sand_blockSize: 4
  $name: "⏳ [Sand] Grain size (px)"
- sand_fadeStart: 40
  $name: "⏳ [Sand] Fade-out start (%)"
- sand_scatterX: 300
  $name: "⏳ [Sand] Horizontal scatter (px)"
- sand_scatterY: 300
  $name: "⏳ [Sand] Vertical scatter (px)"
- sand_wind: 0
  $name: "⏳ [Sand] Wind (-500…+500)"
  $description: "Constant horizontal push. Negative = left, positive = right."

# ═════════════════════════════════════════════════
# 💎 GLASS
# ═════════════════════════════════════════════════

- glass_minDur: 600
  $name: "💎 [Glass] Minimize duration (ms)"
- glass_resDur: 500
  $name: "💎 [Glass] Restore duration (ms)"
- glass_closeDur: 600
  $name: "💎 [Glass] Close duration (ms)"
- glass_bounceOn: false
  $name: "💎 [Glass] Restore bounce"
- glass_bounceStr: 10
  $name: "💎 [Glass] Bounce strength (0–100)"
- glass_bounceDur: 300
  $name: "💎 [Glass] Bounce duration (ms)"
- glass_style: 80
  $name: "💎 [Glass] Style (0=crumble, 100=explosion)"
- glass_gravity: 1500
  $name: "💎 [Glass] Fall speed"
- glass_fadeStart: 30
  $name: "💎 [Glass] Fade-out start (%)"
- glass_scatterX: 600
  $name: "💎 [Glass] Horizontal scatter (px)"
- glass_scatterY: 600
  $name: "💎 [Glass] Vertical scatter (px)"
- glass_pieces: 100
  $name: "💎 [Glass] Shard count (4–500)"
- glass_shape: rand
  $name: "💎 [Glass] Shard shape"
  $options:
    - tri:   "🔺 Triangles (3D-ish)"
    - rect:  "🟦 Rectangles"
    - penta: "💠 Polygons"
    - rand:  "🎲 Random mix"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <math.h>
#include <stdint.h>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <mutex>
#include <string>
#include <algorithm>
#include <cwctype>
#include <initguid.h>
#include <oleacc.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "oleacc.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib")

#define PI 3.14159265358979323846f
#define MAX_CONCURRENT_ANIMS 4
#define MAX_SNAPSHOT_CACHE 16

// =============================================================================
//   HOOK TYPES
// =============================================================================

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL    (WINAPI *ShowWindow_t)   (HWND, int);
typedef LRESULT (WINAPI *SendMessageW_t) (HWND, UINT, WPARAM, LPARAM);

static DefWindowProcW_t DefWindowProcW_Orig;
static ShowWindow_t     ShowWindow_Orig;
static SendMessageW_t   SendMessageW_Orig;

// =============================================================================
//   ENUMS, STRUCTS, GLOBALS
// =============================================================================

enum AnimMode {
    MODE_OFF = 0,
    MODE_GENIE,
    MODE_FLUID,
    MODE_SCALE,
    MODE_ZOOM,
    MODE_SWIPE,
    MODE_CRT,
    MODE_BLACKHOLE,
    MODE_SAND,
    MODE_GLASS
};

struct EffectSettings {
    int minDur = 0, resDur = 0, closeDur = 0;
    int bounceStr = 0, bounceDur = 0;
    bool bounceOn = false;
    int ext1 = 0, ext2 = 0, ext3 = 0, ext4 = 0, ext5 = 0, ext6 = 0, ext7 = 0, ext8 = 0;
};

struct GhostAnimData {
    HWND     hRealWnd;
    HBITMAP  hBitmap;
    void*    pBits;
    RECT     targetRect;
    int      width, height;
    int      targetDockX, targetDockY;
    BOOL     isRising, isClosing;
    LONG_PTR originalExStyle;
    int      mode;
    EffectSettings cfg;
    HANDLE   hReadyEvent;
    RECT     monRect;
};

static std::unordered_map<HWND, HBITMAP> g_SnapshotCache;
static std::deque<HWND>                  g_SnapshotOrder;
static std::unordered_map<HWND, int>     g_IconPositionsX;
static std::unordered_map<HWND, int>     g_IconPositionsY;
// Per-exe cache: once user reveals an app's icon via cursor hover, EVERY future
// instance of that exe uses the same cached position (since Win11 groups them).
static std::unordered_map<std::wstring, POINT> g_ExeIconCache;
static std::unordered_set<HWND>          g_animatingWindows;
static std::mutex                        g_CacheMutex;
static std::mutex                        g_SettingsMutex;

static std::atomic<int>  g_minMode{MODE_FLUID}, g_closeMode{MODE_GLASS};
static std::atomic<int>  g_dockOffsetY{-15};
static std::atomic<int>  g_activeAnims{0};
static std::atomic<bool> g_shuttingDown{false};

static EffectSettings s_genie{}, s_fluid{}, s_scale{}, s_zoom{}, s_swipe{};
static EffectSettings s_crt{}, s_blackhole{}, s_sand{}, s_glass{};
static int g_swipeDir = 4; // 4 = random

// =============================================================================
//   PARSE / LOAD SETTINGS
// =============================================================================

static AnimMode ParseMode(PCWSTR str) {
    if (!str || wcscmp(str, L"none") == 0) return MODE_OFF;
    if (wcscmp(str, L"genie")     == 0) return MODE_GENIE;
    if (wcscmp(str, L"fluid")     == 0) return MODE_FLUID;
    if (wcscmp(str, L"scale")     == 0) return MODE_SCALE;
    if (wcscmp(str, L"zoom")      == 0) return MODE_ZOOM;
    if (wcscmp(str, L"swipe")     == 0) return MODE_SWIPE;
    if (wcscmp(str, L"crt")       == 0) return MODE_CRT;
    if (wcscmp(str, L"blackhole") == 0) return MODE_BLACKHOLE;
    if (wcscmp(str, L"sand")      == 0) return MODE_SAND;
    if (wcscmp(str, L"glass")     == 0) return MODE_GLASS;
    return MODE_OFF;
}

static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_SettingsMutex);

    PCWSTR s;
    s = Wh_GetStringSetting(L"minMode");   g_minMode   = ParseMode(s); Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"closeMode"); g_closeMode = ParseMode(s); Wh_FreeStringSetting(s);
    g_dockOffsetY = Wh_GetIntSetting(L"dock_offset_y");

    // Genie
    s_genie.minDur    = Wh_GetIntSetting(L"genie_minDur");
    s_genie.resDur    = Wh_GetIntSetting(L"genie_resDur");
    s_genie.bounceOn  = Wh_GetIntSetting(L"genie_bounceOn") != 0;
    s_genie.bounceStr = Wh_GetIntSetting(L"genie_bounceStr");
    s_genie.bounceDur = Wh_GetIntSetting(L"genie_bounceDur");
    s_genie.ext1      = Wh_GetIntSetting(L"genie_tailWidth");
    s_genie.ext2      = Wh_GetIntSetting(L"genie_tailOpacity");
    s_genie.ext3      = Wh_GetIntSetting(L"genie_quality");

    // Fluid (was Jelly)
    s_fluid.minDur    = Wh_GetIntSetting(L"fluid_minDur");
    s_fluid.resDur    = Wh_GetIntSetting(L"fluid_resDur");
    s_fluid.bounceOn  = Wh_GetIntSetting(L"fluid_bounceOn") != 0;
    s_fluid.bounceStr = Wh_GetIntSetting(L"fluid_bounceStr");
    s_fluid.bounceDur = Wh_GetIntSetting(L"fluid_bounceDur");
    s_fluid.ext1      = Wh_GetIntSetting(L"fluid_blobRadius");
    s_fluid.ext2      = Wh_GetIntSetting(L"fluid_blobTiming");
    s_fluid.ext3      = Wh_GetIntSetting(L"fluid_tailFadeLength");
    s_fluid.ext4      = Wh_GetIntSetting(L"fluid_morphStart");
    s_fluid.ext5      = Wh_GetIntSetting(L"fluid_tailCatchup");
    s_fluid.ext6      = Wh_GetIntSetting(L"fluid_liquidity");
    s_fluid.ext7      = Wh_GetIntSetting(L"fluid_tailWidth");
    s_fluid.ext8      = Wh_GetIntSetting(L"fluid_dropProfile");
    // Edge inset packed into spare slot (reuse minDur slot? no, keep clean):
    // Need one more — pack via crt-style separate global
    // We'll just add a small inline read in RunFluid:
    // (kept clean: read via a helper)

    // Scale (was Vanilla)
    s_scale.minDur    = Wh_GetIntSetting(L"scale_minDur");
    s_scale.resDur    = Wh_GetIntSetting(L"scale_resDur");
    s_scale.bounceOn  = Wh_GetIntSetting(L"scale_bounceOn") != 0;
    s_scale.bounceStr = Wh_GetIntSetting(L"scale_bounceStr");
    s_scale.bounceDur = Wh_GetIntSetting(L"scale_bounceDur");
    s_scale.ext1      = Wh_GetIntSetting(L"scale_iconSize");
    s_scale.ext2      = Wh_GetIntSetting(L"scale_fadeStart");

    // Zoom
    s_zoom.minDur    = Wh_GetIntSetting(L"zoom_minDur");
    s_zoom.resDur    = Wh_GetIntSetting(L"zoom_resDur");
    s_zoom.closeDur  = Wh_GetIntSetting(L"zoom_closeDur");
    s_zoom.bounceOn  = Wh_GetIntSetting(L"zoom_bounceOn") != 0;
    s_zoom.bounceStr = Wh_GetIntSetting(L"zoom_bounceStr");
    s_zoom.bounceDur = Wh_GetIntSetting(L"zoom_bounceDur");
    s_zoom.ext1      = Wh_GetIntSetting(L"zoom_fadeStart");

    // Swipe
    int swDur = Wh_GetIntSetting(L"swipe_dur");
    s_swipe.minDur = s_swipe.resDur = s_swipe.closeDur = swDur;
    s_swipe.bounceOn = false;
    s_swipe.ext1 = Wh_GetIntSetting(L"swipe_fadeStrength");
    s_swipe.ext2 = Wh_GetIntSetting(L"swipe_distance");
    s_swipe.ext3 = Wh_GetIntSetting(L"swipe_smoothness");

    s = Wh_GetStringSetting(L"swipe_dir");
    if (s) {
        if      (wcscmp(s, L"down")  == 0) g_swipeDir = 0;
        else if (wcscmp(s, L"left")  == 0) g_swipeDir = 1;
        else if (wcscmp(s, L"right") == 0) g_swipeDir = 2;
        else if (wcscmp(s, L"up")    == 0) g_swipeDir = 3;
        else                                g_swipeDir = 4;
        Wh_FreeStringSetting(s);
    }

    // CRT
    int crtDur = Wh_GetIntSetting(L"crt_dur");
    s_crt.minDur = s_crt.resDur = s_crt.closeDur = crtDur;
    s_crt.bounceOn  = true;
    s_crt.bounceStr = 25;
    s_crt.bounceDur = 300;

    // Black Hole
    s_blackhole.minDur    = Wh_GetIntSetting(L"blackhole_minDur");
    s_blackhole.resDur    = Wh_GetIntSetting(L"blackhole_resDur");
    s_blackhole.closeDur  = Wh_GetIntSetting(L"blackhole_closeDur");
    s_blackhole.bounceOn  = Wh_GetIntSetting(L"blackhole_bounceOn") != 0;
    s_blackhole.bounceStr = Wh_GetIntSetting(L"blackhole_bounceStr");
    s_blackhole.bounceDur = Wh_GetIntSetting(L"blackhole_bounceDur");
    s_blackhole.ext1      = Wh_GetIntSetting(L"blackhole_twist");
    s_blackhole.ext2      = Wh_GetIntSetting(L"blackhole_quality");

    // Sand
    s_sand.minDur    = Wh_GetIntSetting(L"sand_minDur");
    s_sand.resDur    = Wh_GetIntSetting(L"sand_resDur");
    s_sand.closeDur  = Wh_GetIntSetting(L"sand_closeDur");
    s_sand.bounceOn  = Wh_GetIntSetting(L"sand_bounceOn") != 0;
    s_sand.bounceStr = Wh_GetIntSetting(L"sand_bounceStr");
    s_sand.bounceDur = Wh_GetIntSetting(L"sand_bounceDur");
    s_sand.ext1      = Wh_GetIntSetting(L"sand_blockSize");
    s_sand.ext2      = Wh_GetIntSetting(L"sand_gravity");
    s_sand.ext3      = Wh_GetIntSetting(L"sand_style");
    s_sand.ext4      = Wh_GetIntSetting(L"sand_fadeStart");
    s_sand.ext5      = Wh_GetIntSetting(L"sand_scatterX");
    s_sand.ext6      = Wh_GetIntSetting(L"sand_scatterY");
    s_sand.ext7      = Wh_GetIntSetting(L"sand_wind");

    // Glass
    s_glass.minDur    = Wh_GetIntSetting(L"glass_minDur");
    s_glass.resDur    = Wh_GetIntSetting(L"glass_resDur");
    s_glass.closeDur  = Wh_GetIntSetting(L"glass_closeDur");
    s_glass.bounceOn  = Wh_GetIntSetting(L"glass_bounceOn") != 0;
    s_glass.bounceStr = Wh_GetIntSetting(L"glass_bounceStr");
    s_glass.bounceDur = Wh_GetIntSetting(L"glass_bounceDur");
    s_glass.ext2      = Wh_GetIntSetting(L"glass_gravity");
    s_glass.ext3      = Wh_GetIntSetting(L"glass_style");
    s_glass.ext4      = Wh_GetIntSetting(L"glass_fadeStart");
    s_glass.ext5      = Wh_GetIntSetting(L"glass_scatterX");
    s_glass.ext6      = Wh_GetIntSetting(L"glass_scatterY");
    s_glass.ext7      = Wh_GetIntSetting(L"glass_pieces");

    s = Wh_GetStringSetting(L"glass_shape");
    if (s) {
        if      (wcscmp(s, L"rect")  == 0) s_glass.ext8 = 1;
        else if (wcscmp(s, L"penta") == 0) s_glass.ext8 = 2;
        else if (wcscmp(s, L"rand")  == 0) s_glass.ext8 = 3;
        else                                s_glass.ext8 = 0;
        Wh_FreeStringSetting(s);
    }
}

// Helper to read fluid_edgeInset on demand (kept outside ext-slots for clarity)
static int GetFluidEdgeInset() {
    std::lock_guard<std::mutex> lock(g_SettingsMutex);
    int v = Wh_GetIntSetting(L"fluid_edgeInset");
    return v < 0 ? 0 : (v > 50 ? 50 : v);
}

static void CopyEffectSettings(EffectSettings& dst, const EffectSettings& src) {
    std::lock_guard<std::mutex> lock(g_SettingsMutex);
    dst = src;
}

// =============================================================================
//   MATH & EASING
// =============================================================================

static inline float clampf(float v, float lo, float hi)   { return v < lo ? lo : (v > hi ? hi : v); }
static inline int   clamp_int(int v, int lo, int hi)      { return v < lo ? lo : (v > hi ? hi : v); }
static inline int   max_int(int a, int b)                 { return a > b ? a : b; }
static inline int   min_int(int a, int b)                 { return a < b ? a : b; }

static inline float easeOutQuart(float t)  { float f = t - 1.0f; return 1.0f - (f*f*f*f); }
static inline float easeInQuart(float t)   { return t*t*t*t; }
static inline float easeInOutSine(float t) { return -0.5f * (cosf(PI * t) - 1.0f); }

// Material-3 style accelerate/decelerate via power curve.
// power = 1 → linear; 2 → standard Material; 4.5 → emphasized snap.
static inline float materialEase(float t, bool isRising, float power) {
    // Both directions use the same formula thanks to how t is pre-flipped per direction:
    //   close   → t goes 0→1, easePos goes 0→1 with slow start (accelerate)
    //   restore → t goes 1→0, easePos goes 1→0 with slow end   (decelerate)
    (void)isRising;
    return powf(t, power);
}

static inline float smin(float a, float b, float k) {
    float h = clampf(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return a * h + b * (1.0f - h) - k * h * (1.0f - h);
}

static inline uint32_t FastRand(uint32_t& s) {
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return s;
}
static inline float FastRandFloat(uint32_t& s) {
    return (FastRand(s) & 0xFFFFFF) / (float)0x1000000;
}

// Premultiply RGBA by float alpha; promotes alpha=0 pixels with color (PrintWindow quirk).
static inline uint32_t PremultiplyBlendFast(uint32_t px, float a) {
    if (a >= 0.997f) return px;
    if (a <= 0.003f) return 0;
    uint32_t a256 = (uint32_t)(a * 256.0f);
    uint32_t origA = (px >> 24) & 0xFF;
    if (origA == 0 && (px & 0xFFFFFF) != 0) origA = 255;
    uint32_t newA = (origA * a256) >> 8;
    uint32_t r = (((px >> 16) & 0xFF) * a256) >> 8;
    uint32_t g = (((px >>  8) & 0xFF) * a256) >> 8;
    uint32_t b = ( (px        & 0xFF) * a256) >> 8;
    return (newA << 24) | (r << 16) | (g << 8) | b;
}

// Source-over compositing of two premultiplied pixels (additive overdraw for particles).
static inline uint32_t AlphaBlendPixels(uint32_t dst, uint32_t src_premult) {
    if (dst == 0) return src_premult;
    uint32_t srcA = src_premult >> 24;
    if (srcA == 255) return src_premult;
    if (srcA == 0)   return dst;
    uint32_t invA = 255 - srcA;
    uint32_t outA = srcA + ((((dst >> 24)       ) * invA) >> 8);
    uint32_t outR = ((src_premult >> 16) & 0xFF) + ((((dst >> 16) & 0xFF) * invA) >> 8);
    uint32_t outG = ((src_premult >>  8) & 0xFF) + ((((dst >>  8) & 0xFF) * invA) >> 8);
    uint32_t outB = ( src_premult        & 0xFF) + ((( dst        & 0xFF) * invA) >> 8);
    return (outA << 24) | (outR << 16) | (outG << 8) | outB;
}

static inline float SpringBounce(float tB, float strength) {
    float impact = (strength / 100.0f) * 0.18f;
    float decay  = expf(-tB * 4.5f);
    float osc    = sinf(tB * PI * 2.2f);
    return 1.0f + impact * osc * decay;
}

// =============================================================================
//   DIB / GHOST WINDOW HELPERS
// =============================================================================

static HBITMAP CreateArgbDib(HDC hRefDC, int w, int h, void** outBits) {
    if (w <= 0 || h <= 0) return NULL;
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth  = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(hRefDC, &bmi, DIB_RGB_COLORS, outBits, NULL, 0);
}

// Allow the animation bbox to extend N pixels beyond monitor edges.
// Ghost windows can be positioned off-screen; the off-screen pixels are simply
// not visible. This eliminates the "edge clipping" artifact when minimizing
// or restoring a window near the screen edge.
#define BBOX_OVERFLOW_PX 250

static RECT ClipToMonitor(RECT r, const RECT& mon) {
    LONG L = mon.left   - BBOX_OVERFLOW_PX;
    LONG T = mon.top    - BBOX_OVERFLOW_PX;
    LONG R = mon.right  + BBOX_OVERFLOW_PX;
    LONG B = mon.bottom + BBOX_OVERFLOW_PX;
    if (r.left   < L) r.left   = L;
    if (r.top    < T) r.top    = T;
    if (r.right  > R) r.right  = R;
    if (r.bottom > B) r.bottom = B;
    if (r.right  < r.left) r.right  = r.left;
    if (r.bottom < r.top)  r.bottom = r.top;
    return r;
}

static RECT ComputeAnimBBox(GhostAnimData* data) {
    const RECT& mon = data->monRect;
    RECT r = data->targetRect;
    RECT b = r;

    switch (data->mode) {
        case MODE_SCALE: {
            int half = max_int(8, data->cfg.ext1 / 2) + 8;
            b.left   = min_int(b.left,   data->targetDockX - half);
            b.top    = min_int(b.top,    data->targetDockY - half);
            b.right  = max_int(b.right,  data->targetDockX + half);
            b.bottom = max_int(b.bottom, data->targetDockY + half);
            break;
        }
        case MODE_FLUID: {
            int margin = 100;
            b.left   = min_int(b.left,   data->targetDockX - margin);
            b.top    = min_int(b.top,    data->targetDockY - margin);
            b.right  = max_int(b.right,  data->targetDockX + margin);
            b.bottom = max_int(b.bottom, data->targetDockY + margin);
            b.left -= 40; b.top -= 40; b.right += 40; b.bottom += 40;
            break;
        }
        case MODE_GENIE: {
            int m = max_int(20, data->height / 4);
            b.left = mon.left; b.right = mon.right; b.bottom = mon.bottom;
            b.top  = max_int(mon.top, r.top - m);
            break;
        }
        case MODE_CRT:
        case MODE_ZOOM: {
            int m = max_int(20, max_int(data->width, data->height) / 6);
            b.left -= m; b.top -= m; b.right += m; b.bottom += m;
            break;
        }
        case MODE_BLACKHOLE: {
            // Particles only travel toward window center → small margin sufficient
            int m = max_int(20, max_int(data->width, data->height) / 10);
            b.left -= m; b.top -= m; b.right += m; b.bottom += m;
            break;
        }
        case MODE_SAND: {
            int sX = data->cfg.ext5 + 80;
            int sY = data->cfg.ext6 + 80;
            int grav = data->cfg.ext2;
            int wind = abs(data->cfg.ext7);
            b.left   -= sX + wind / 2;
            b.right  += sX + wind / 2;
            b.top    -= sY / 2;
            b.bottom += sY + grav;
            break;
        }
        case MODE_GLASS: {
            // Tighter bbox: actual ballistic reach ≈ 0.7 × scatter over animation
            int sX = (data->cfg.ext5 * 7) / 10 + 60;
            int sY = (data->cfg.ext6 * 7) / 10 + 60;
            int grav = data->cfg.ext2;
            b.left -= sX; b.right += sX;
            b.top  -= sY / 2;
            b.bottom += sY + grav / 4 + 100;
            break;
        }
        case MODE_SWIPE: {
            int d = max_int(50, data->cfg.ext2);
            b.left -= d; b.right += d; b.top -= d; b.bottom += d;
            break;
        }
        default: {
            int m = 50;
            b.left -= m; b.top -= m; b.right += m; b.bottom += m;
        }
    }
    return ClipToMonitor(b, mon);
}

struct GhostBuf {
    HWND    hGhost   = NULL;
    HBITMAP hWork    = NULL;
    HBITMAP hOldWork = NULL;
    HDC     hWorkDC  = NULL;
    void*   workBits = NULL;
    int     x = 0, y = 0, w = 0, h = 0;

    bool Init(const RECT& bb, HDC hRefDC) {
        x = bb.left; y = bb.top;
        w = bb.right - bb.left; h = bb.bottom - bb.top;
        if (w <= 0 || h <= 0) return false;
        hWork = CreateArgbDib(hRefDC, w, h, &workBits);
        if (!hWork) return false;
        hWorkDC = CreateCompatibleDC(hRefDC);
        if (!hWorkDC) { DeleteObject(hWork); hWork = NULL; return false; }
        hOldWork = (HBITMAP)SelectObject(hWorkDC, hWork);
        hGhost = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
            L"STATIC", L"FluidEngineGhost", WS_POPUP,
            x, y, w, h, NULL, NULL, NULL, NULL);
        if (!hGhost) {
            SelectObject(hWorkDC, hOldWork); DeleteDC(hWorkDC);
            DeleteObject(hWork); hWork = NULL; hWorkDC = NULL;
            return false;
        }
        SetStretchBltMode(hWorkDC, COLORONCOLOR);
        return true;
    }
    void Clear() { if (workBits) memset(workBits, 0, (size_t)w * h * 4); }
    void Show()  { if (hGhost) ShowWindow(hGhost, SW_SHOWNOACTIVATE); }
    void Present(BYTE alpha = 255) {
        if (!hGhost) return;
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
        POINT ptSrc = {0, 0};
        SIZE  sz    = { w, h };
        UpdateLayeredWindow(hGhost, NULL, NULL, &sz, hWorkDC, &ptSrc, 0, &bf, ULW_ALPHA);
    }
    ~GhostBuf() {
        if (hWorkDC && hOldWork) SelectObject(hWorkDC, hOldWork);
        if (hWorkDC) DeleteDC(hWorkDC);
        if (hWork)   DeleteObject(hWork);
        if (hGhost)  DestroyWindow(hGhost);
    }
};

// =============================================================================
//   DWM / WINDOW HELPERS
// =============================================================================

static inline void SetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

static void RevealRealWindow(GhostAnimData* data) {
    if (!IsWindow(data->hRealWnd)) return;

    if (data->isClosing) {
        // Close: if the window is STILL alive at the end of the animation, the close
        // was cancelled (e.g., "Save changes?" dialog) — restore its visibility.
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
        if (!(data->originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            SetWindowPos(data->hRealWnd, NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
        }
        SetDwmTransitions(data->hRealWnd, TRUE);
        return;
    }

    if (data->isRising) {
        SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
        RedrawWindow(data->hRealWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
        if (!(data->originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            SetWindowPos(data->hRealWnd, NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
        }
        DwmFlush(); Sleep(15);
    } else {
        SetLayeredWindowAttributes(data->hRealWnd, 0, 0, LWA_ALPHA);
    }
    SetDwmTransitions(data->hRealWnd, TRUE);
}

static void SignalReady(GhostAnimData* data, BOOL& firstFrame, GhostBuf& gb) {
    if (firstFrame) {
        gb.Show();
        if (data->hReadyEvent) SetEvent(data->hReadyEvent);
        firstFrame = FALSE;
    }
}

static double GetAnimDuration(GhostAnimData* d) {
    return d->isClosing ? d->cfg.closeDur : (d->isRising ? d->cfg.resDur : d->cfg.minDur);
}
static double GetBounceDuration(GhostAnimData* d) {
    return (d->isRising && d->cfg.bounceOn && !d->isClosing) ? d->cfg.bounceDur : 0;
}

// =============================================================================
//   EFFECT: 📐 SCALE  (default minimize, was Vanilla)
// =============================================================================

static void RunScale(GhostAnimData* data) {
    RECT r = data->targetRect; int w = data->width, h = data->height;
    float dockX = (float)data->targetDockX, dockY = (float)data->targetDockY;

    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = GetAnimDuration(data), bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;

    float iconTarget = clampf((float)data->cfg.ext1, 8.0f, 4096.0f);
    float targetX = dockX - iconTarget / 2.0f;
    float targetY = dockY - iconTarget / 2.0f;

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear(); BYTE alpha = 255;

        if (elapsed <= animDur || !data->isRising) {
            float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
            float t = data->isRising ? (1.0f - p) : p;
            float u = easeOutQuart(t);

            int curX = (int)(r.left + (targetX - r.left) * u) - X;
            int curY = (int)(r.top  + (targetY - r.top)  * u) - Y;
            int curW = max_int(2, (int)(w + (iconTarget - w) * u));
            int curH = max_int(2, (int)(h + (iconTarget - h) * u));

            StretchBlt(gb.hWorkDC, curX, curY, curW, curH, hSrcDC, 0, 0, w, h, SRCCOPY);

            float fadeStart = clampf(data->cfg.ext2 / 100.0f, 0.0f, 0.95f);
            float fT = clampf((u - fadeStart) / (1.0f - fadeStart), 0.0f, 1.0f);
            alpha = (BYTE)clampf(255.0f * (1.0f - fT), 0.0f, 255.0f);
        } else {
            float tB = clampf((float)((elapsed - animDur) / bounceDur), 0.0f, 1.0f);
            float scale = SpringBounce(tB, (float)data->cfg.bounceStr);
            int curW = max_int(2, (int)(w * scale));
            int curH = max_int(2, (int)(h * scale));
            int curX = (int)(r.left + w / 2.0f - curW / 2.0f) - X;
            int curY = (int)(r.top  + h / 2.0f - curH / 2.0f) - Y;
            StretchBlt(gb.hWorkDC, curX, curY, curW, curH, hSrcDC, 0, 0, w, h, SRCCOPY);
        }

        gb.Present(alpha);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: 🔍 ZOOM
// =============================================================================

static void RunZoom(GhostAnimData* data) {
    RECT r = data->targetRect; int w = data->width, h = data->height;
    float cx = r.left + w / 2.0f, cy = r.top + h / 2.0f;

    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = GetAnimDuration(data), bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear(); BYTE alpha = 255;

        if (elapsed <= animDur || !data->isRising) {
            float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
            float t = data->isRising ? (1.0f - p) : p;
            float u = easeOutQuart(t);

            int curW = max_int(2, (int)(w * (1.0f - u)));
            int curH = max_int(2, (int)(h * (1.0f - u)));
            int curX = (int)(cx - curW / 2.0f) - X;
            int curY = (int)(cy - curH / 2.0f) - Y;

            StretchBlt(gb.hWorkDC, curX, curY, curW, curH, hSrcDC, 0, 0, w, h, SRCCOPY);

            float fadeStart = clampf(data->cfg.ext1 / 100.0f, 0.0f, 0.95f);
            float fT = clampf((u - fadeStart) / (1.0f - fadeStart), 0.0f, 1.0f);
            alpha = (BYTE)clampf(255.0f * (1.0f - fT), 0.0f, 255.0f);
        } else {
            float tB = clampf((float)((elapsed - animDur) / bounceDur), 0.0f, 1.0f);
            float scale = SpringBounce(tB, (float)data->cfg.bounceStr);
            int curW = max_int(2, (int)(w * scale));
            int curH = max_int(2, (int)(h * scale));
            int curX = (int)(cx - curW / 2.0f) - X;
            int curY = (int)(cy - curH / 2.0f) - Y;
            StretchBlt(gb.hWorkDC, curX, curY, curW, curH, hSrcDC, 0, 0, w, h, SRCCOPY);
        }

        gb.Present(alpha);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: 💨 SWIPE  (Material 3 easing)
// =============================================================================

static void RunSwipe(GhostAnimData* data) {
    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = GetAnimDuration(data);

    float fadeStr = clampf(data->cfg.ext1 / 100.0f, 0.0f, 1.0f);
    int   dist    = data->cfg.ext2 > 0 ? data->cfg.ext2 : 500;

    // smoothness 0..100 → power 1..4.5 (Material curves)
    float smoothness = clampf(data->cfg.ext3 / 100.0f, 0.0f, 1.0f);
    float power      = 1.0f + smoothness * 3.5f;
    float alphaPower = power * 1.4f;   // alpha fades slightly later for cleaner exit

    uint32_t rng = (uint32_t)GetTickCount64() ^ (uint32_t)(uintptr_t)data->hRealWnd;
    int dir = g_swipeDir;
    if (dir == 4) dir = FastRand(rng) % 4;

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear();

        float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
        float t = data->isRising ? (1.0f - p) : p;

        // close (t: 0→1) → easePos: 0→1, slow start → accelerate
        // rising (t: 1→0) → easePos: 1→0, derivative steepest at start of restore → decelerate
        float easePos   = materialEase(t, data->isRising != FALSE, power);
        float easeAlpha = materialEase(t, data->isRising != FALSE, alphaPower);

        int dx = 0, dy = 0;
        if      (dir == 0) dy =  (int)(easePos * dist);
        else if (dir == 1) dx = -(int)(easePos * dist);
        else if (dir == 2) dx =  (int)(easePos * dist);
        else if (dir == 3) dy = -(int)(easePos * dist);

        int curX = data->targetRect.left + dx - X;
        int curY = data->targetRect.top  + dy - Y;
        StretchBlt(gb.hWorkDC, curX, curY, data->width, data->height,
                   hSrcDC, 0, 0, data->width, data->height, SRCCOPY);

        BYTE alpha = (BYTE)clampf(255.0f * (1.0f - easeAlpha * fadeStr), 0.0f, 255.0f);
        gb.Present(alpha);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= animDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: 📺 CRT
// =============================================================================

static void RunCRT(GhostAnimData* data) {
    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = GetAnimDuration(data), bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;
    int cx = data->targetRect.left + data->width  / 2;
    int cy = data->targetRect.top  + data->height / 2;

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear(); BYTE alpha = 255;

        if (elapsed <= animDur || !data->isRising) {
            float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
            float t = data->isRising ? (1.0f - p) : p;
            int curW = data->width, curH = data->height;
            int curX = data->targetRect.left, curY = data->targetRect.top;

            if (t < 0.4f) {
                float eY = easeOutQuart(t / 0.4f);
                curH = data->height - (int)((data->height - 2) * eY);
                curY = cy - curH / 2;
            } else {
                float eX = easeOutQuart((t - 0.4f) / 0.6f);
                curH = 2; curY = cy - 1;
                curW = data->width - (int)(data->width * eX);
                curX = cx - curW / 2;
                alpha = (BYTE)(255 * (1.0f - eX));
            }
            if (curW < 2) curW = 2;
            if (curH < 2) curH = 2;

            StretchBlt(gb.hWorkDC, curX - X, curY - Y, curW, curH, hSrcDC, 0, 0, data->width, data->height, SRCCOPY);
        } else {
            float tB = clampf((float)((elapsed - animDur) / bounceDur), 0.0f, 1.0f);
            float scale = SpringBounce(tB, (float)data->cfg.bounceStr);
            int newW = max_int(2, (int)(data->width  * scale));
            int newH = max_int(2, (int)(data->height * scale));
            StretchBlt(gb.hWorkDC, cx - newW / 2 - X, cy - newH / 2 - Y, newW, newH,
                       hSrcDC, 0, 0, data->width, data->height, SRCCOPY);
        }

        gb.Present(alpha);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: 🌀 GENIE
// =============================================================================

static void RunGenie(GhostAnimData* data) {
    RECT r = data->targetRect; int w = data->width, h = data->height;
    float top = (float)r.top, dockY = (float)data->targetDockY, dockX = (float)data->targetDockX;
    float startCenterX = (float)(r.left + w / 2);

    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int bboxLeft = gb.x, bboxTop = gb.y, bboxW = gb.w, bboxH = gb.h;

    int SS = (int)clampf((float)data->cfg.ext3, 1.0f, 3.0f);
    int rW = bboxW * SS, rH = bboxH * SS;

    float tailW    = clampf((float)data->cfg.ext1, 2.0f, (float)w);
    float tailFlat = clampf(tailW * 1.5f, 8.0f, 80.0f);
    float neckLen  = clampf((float)(w - tailW) * 1.2f, (float)h * 0.2f, (float)bboxH * 0.85f);

    void*   renderBits  = gb.workBits;
    HBITMAP hRender     = NULL;
    HDC     hRenderDC   = gb.hWorkDC;
    HBITMAP hOldRender  = NULL;
    if (SS > 1) {
        hRender = CreateArgbDib(hScreenDC, rW, rH, &renderBits);
        if (hRender) {
            hRenderDC = CreateCompatibleDC(hScreenDC);
            hOldRender = (HBITMAP)SelectObject(hRenderDC, hRender);
            SetStretchBltMode(hRenderDC, COLORONCOLOR);
        } else {
            SS = 1; hRenderDC = gb.hWorkDC; renderBits = gb.workBits;
        }
    }

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = data->isRising ? data->cfg.resDur : data->cfg.minDur;
    double bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;
    int N = (int)clampf((float)h / 2.0f, 80.0f, 450.0f);

    float minTailOp = clampf(data->cfg.ext2 / 100.0f, 0.0f, 1.0f);
    float tailFrac  = 0.4f;

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        float flightP = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
        float tFade = data->isRising ? (1.0f - flightP) : flightP;
        BYTE ca = (tFade > 0.90f) ? (BYTE)(255 * (1.0f - (tFade - 0.90f) / 0.10f)) : 255;

        float geEase = data->isRising ? (1.0f - easeOutQuart(flightP)) : easeOutQuart(flightP);
        float logTop    = top + (dockY - top) * geEase;
        float logBottom = (top + h) + (dockY - (top + h)) * geEase;
        float logH      = (float)max_int(1, (int)(logBottom - logTop));
        float centerY   = logTop + logH / 2.0f;

        float bounceScale = 1.0f;
        if (data->isRising && data->cfg.bounceOn && elapsed > animDur * 0.65f) {
            float tB = clampf((float)((elapsed - animDur * 0.65f) / (totalDur - animDur * 0.65f)), 0.0f, 1.0f);
            bounceScale = SpringBounce(tB, (float)data->cfg.bounceStr);
        }

        float curH_f    = logH * bounceScale;
        float curTop    = centerY - curH_f / 2.0f;
        float curBottom = centerY + curH_f / 2.0f;
        float curW      = w * bounceScale;
        int   curH      = max_int(1, (int)(curH_f + 0.5f));
        float morphU    = powf(1.0f - geEase, 6.0f);
        bool  isFullRect = (geEase == 0.0f);

        int bandTop = max_int(0, (int)curTop    - bboxTop - 2);
        int bandBot = min_int(bboxH, (int)curBottom - bboxTop + 2);
        int marginX = w / 8 + 2;
        int spanL = max_int(0, min_int(r.left, (int)dockX) - bboxLeft - marginX);
        int spanR = min_int(bboxW, max_int(r.right, (int)dockX) - bboxLeft + marginX);
        if (spanR < spanL) spanR = spanL;

        memset(gb.workBits, 0, (size_t)bboxW * bboxH * 4);
        if (SS > 1 && bandBot > bandTop) {
            for (int ry = bandTop * SS; ry < bandBot * SS; ry++)
                memset((BYTE*)renderBits + ((size_t)ry * rW + spanL * SS) * 4, 0, (size_t)(spanR - spanL) * SS * 4);
        }

        if (isFullRect) {
            StretchBlt(hRenderDC,
                (int)(((startCenterX - curW / 2.0f) - bboxLeft) * SS + 0.5f),
                (int)((curTop - bboxTop) * SS + 0.5f),
                (int)(curW * SS + 0.5f), (int)(curH * SS + 0.5f),
                hSrcDC, 0, 0, w, h, SRCCOPY);
        } else {
            for (int i = 0; i < N; i++) {
                float v0 = (float)i / N, v1 = (float)(i + 1) / N;
                float y0 = curTop + v0 * curH_f, y1 = curTop + v1 * curH_f;
                float dist = dockY - y0;
                float c0 = (dist <= tailFlat) ? 0.0f
                         : (0.5f - 0.5f * cosf(PI * clampf((dist - tailFlat) / neckLen, 0.0f, 1.0f)));
                float cEff = c0 + (1.0f - c0) * morphU;
                float rw0 = tailW + (curW - tailW) * cEff;
                float cx0 = dockX + (startCenterX - dockX) * cEff;

                int dx_  = (int)(((cx0 - rw0 / 2.0f) - bboxLeft) * SS + 0.5f);
                int dy_  = (int)((y0 - bboxTop) * SS + 0.5f);
                int dh_  = max_int(1, (int)((y1 - bboxTop) * SS + 0.5f) - dy_ + 1);
                int dw_  = max_int(1, (int)(rw0 * SS + 0.5f));
                if (dy_ + dh_ <= 0 || dy_ >= rH) continue;
                StretchBlt(hRenderDC, dx_, dy_, dw_, dh_, hSrcDC, 0,
                    (int)(v0 * h + 0.5f), w,
                    max_int(1, (int)(v1 * h + 0.5f) - (int)(v0 * h + 0.5f)), SRCCOPY);
            }
        }
        GdiFlush();

        if (SS == 2) {
            for (int y = bandTop; y < bandBot; y++) {
                uint32_t* s0 = (uint32_t*)((BYTE*)renderBits + (size_t)(y * 2)     * rW * 4);
                uint32_t* s1 = (uint32_t*)((BYTE*)renderBits + (size_t)(y * 2 + 1) * rW * 4);
                uint32_t* d  = (uint32_t*)((BYTE*)gb.workBits + (size_t)y * bboxW * 4);
                for (int x = spanL; x < spanR; x++) {
                    int xx = x * 2;
                    uint32_t r0 = ((s0[xx] & 0xFEFEFEFEu) >> 1) + ((s0[xx + 1] & 0xFEFEFEFEu) >> 1);
                    uint32_t r1 = ((s1[xx] & 0xFEFEFEFEu) >> 1) + ((s1[xx + 1] & 0xFEFEFEFEu) >> 1);
                    d[x] = ((r0 & 0xFEFEFEFEu) >> 1) + ((r1 & 0xFEFEFEFEu) >> 1);
                }
            }
        } else if (SS > 1) {
            int n = SS * SS;
            for (int y = bandTop; y < bandBot; y++) {
                BYTE* drow = (BYTE*)gb.workBits + (size_t)y * bboxW * 4;
                for (int x = spanL; x < spanR; x++) {
                    int sB = 0, sG = 0, sR = 0, sA = 0;
                    for (int sy = 0; sy < SS; sy++) {
                        BYTE* sp = (BYTE*)renderBits + ((size_t)(y * SS + sy) * rW + (size_t)(x * SS)) * 4;
                        for (int sx = 0; sx < SS; sx++) { sB += sp[0]; sG += sp[1]; sR += sp[2]; sA += sp[3]; sp += 4; }
                    }
                    BYTE* dp = drow + (size_t)x * 4;
                    dp[0] = (BYTE)(sB / n); dp[1] = (BYTE)(sG / n);
                    dp[2] = (BYTE)(sR / n); dp[3] = (BYTE)(sA / n);
                }
            }
        }

        if (!isFullRect && minTailOp < 0.999f) {
            float bodyH = curBottom - curTop + 0.1f;
            int yStart = max_int(bandTop, max_int(0, (int)curTop    - bboxTop));
            int yEnd   = min_int(bandBot, min_int(bboxH, (int)curBottom - bboxTop + 1));
            BYTE* base = (BYTE*)gb.workBits;
            for (int row = yStart; row < yEnd; row++) {
                float op = minTailOp + (1.0f - minTailOp)
                         * clampf((1.0f - clampf((row + bboxTop - curTop) / bodyH, 0.0f, 1.0f)) / tailFrac, 0.0f, 1.0f);
                op = op + (1.0f - op) * morphU;
                if (op >= 0.999f) continue;
                BYTE* px = base + (size_t)row * bboxW * 4 + (size_t)spanL * 4;
                for (int x = spanL; x < spanR; x++) {
                    if (px[3]) {
                        px[0] = (BYTE)(px[0] * op); px[1] = (BYTE)(px[1] * op);
                        px[2] = (BYTE)(px[2] * op); px[3] = (BYTE)(px[3] * op);
                    }
                    px += 4;
                }
            }
        }

        gb.Present(ca);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    if (hRender) { SelectObject(hRenderDC, hOldRender); DeleteObject(hRender); DeleteDC(hRenderDC); }
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: 💧 FLUID  (renamed from Jelly, with fullscreen fix + liquidity)
// =============================================================================

static void RunFluid(GhostAnimData* data) {
    RECT r = data->targetRect; int w = data->width, h = data->height;
    float startCX = r.left + w / 2.0f, startCY = r.top + h / 2.0f;
    float dockX = (float)data->targetDockX, dockY = (float)data->targetDockY;

    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y, bW = gb.w, bH = gb.h;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = data->isRising ? data->cfg.resDur : data->cfg.minDur;
    double bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;

    float maxBlobR          = clampf((float)data->cfg.ext1, 0.0f, 200.0f);
    float blobTimingT       = clampf(data->cfg.ext2 / 100.0f, 0.0f, 0.99f);
    float tailFadeLengthPct = clampf(data->cfg.ext3 / 100.0f, 0.0f, 1.0f);
    float morphStartT       = clampf(data->cfg.ext4 / 100.0f, 0.0f, 0.99f);
    float tailStartT        = clampf(data->cfg.ext5 / 100.0f, 0.0f, 0.99f);
    float liquidity         = clampf(data->cfg.ext6 / 100.0f, 0.0f, 1.0f);
    float tailW             = clampf((float)data->cfg.ext7, 0.0f, 500.0f);
    float profileExp        = clampf(data->cfg.ext8 / 10.0f, 0.1f, 5.0f);
    int   edgeInset         = clamp_int(GetFluidEdgeInset(), 0, 50);

    // Liquidity-driven character modifiers (rubber → oil):
    //   0   = stiff rubber: tight blends, sharp corners, narrow tail, quick morph back to square
    //   100 = liquid oil:   soft blends, round corners, fat smooth tail, stays liquid longer, more wobble
    float sminKMult     = 0.4f  + liquidity * 1.0f;   // smin smoothness:  0.4 → 1.4
    float cornerMult    = 0.7f  + liquidity * 0.6f;   // corner radius:    0.7 → 1.3
    float tailWMult     = 0.7f  + liquidity * 0.8f;   // tail thickness:   0.7 → 1.5
    float profileMult   = 1.4f  - liquidity * 0.6f;   // taper steepness:  1.4 → 0.8 (smoother for oil)
    float wobbleMult    = 0.6f  + liquidity * 0.9f;   // wobble strength:  0.6 → 1.5
    float morphDelay    = liquidity * 0.15f;          // morph delay:      0   → +0.15 (oil stays liquid)

    float tailW_eff           = tailW * tailWMult;
    float profileExp_eff      = profileExp * profileMult;
    float morphStartT_eff     = clampf(morphStartT + morphDelay, 0.4f, 0.95f);

    float tailRadiusLUT[1024];

    // Pre-compute path distance for fullscreen blob cap
    float pathDistY = fabsf(startCY - dockY);
    float pathDistX = fabsf(startCX - dockX);
    float pathDist  = sqrtf(pathDistX * pathDistX + pathDistY * pathDistY);

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear();

        float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
        float t = data->isRising ? p : (1.0f - p);

        float headEase = easeOutQuart(t);
        float Cx_abs = dockX + (startCX - dockX) * headEase;
        float Cy_abs = dockY + (startCY - dockY) * headEase;
        float Cx = Cx_abs - X;
        float Cy = Cy_abs - Y;

        float morphF = 0.0f;
        if (t > morphStartT_eff) morphF = easeInOutSine(clampf((t - morphStartT_eff) / (1.0f - morphStartT_eff), 0.0f, 1.0f));

        float tailT  = (t > tailStartT) ? clampf((t - tailStartT) / (1.0f - tailStartT), 0.0f, 1.0f) : 0.0f;
        float tailE  = easeInQuart(tailT);
        float Tx_abs = dockX + (Cx_abs - dockX) * tailE;
        float Ty_abs = dockY + (Cy_abs - dockY) * tailE;
        float Tx = Tx_abs - X;
        float Ty = Ty_abs - Y;

        float blobT  = (t > blobTimingT) ? clampf((t - blobTimingT) / (1.0f - blobTimingT), 0.0f, 1.0f) : 0.0f;
        float blobE  = easeInQuart(blobT);
        float Bx = (dockX + (Cx_abs - dockX) * blobE) - X;
        float By = (dockY + (Cy_abs - dockY) * blobE) - Y;

        float bScaleW = 1.0f, bScaleH = 1.0f;
        if (elapsed > animDur && data->isRising && data->cfg.bounceOn) {
            float b = clampf((float)((elapsed - animDur) / bounceDur), 0.0f, 1.0f);
            float impact = (data->cfg.bounceStr / 100.0f) * 0.6f * wobbleMult;
            float spring = sinf(b * PI * 3.0f) * expf(-b * 5.0f);
            bScaleH = 1.0f + impact * spring;
            bScaleW = 1.0f - impact * spring * 0.5f;
        }

        float S = 0.05f + 0.95f * headEase;
        float targetW = w * S * bScaleW;
        float targetH = h * S * bScaleH;

        float circleRadius = sqrtf(targetW * targetH) / 2.0f;

        // FULLSCREEN FIX: dynamic cap that tightens near dock
        //   headEase=1 (at window) → large blob allowed (window-shape morphing)
        //   headEase=0 (at dock)   → tiny blob (≈30 px) so it fits in taskbar
        float maxR_static    = fminf((float)bW, (float)bH) * 0.18f;
        float maxR_proximity = 30.0f + headEase * fminf(pathDist * 0.20f, fminf((float)bW, (float)bH) * 0.16f);
        float maxR = fminf(maxR_static, maxR_proximity);
        circleRadius = fminf(circleRadius, maxR);

        float curW = (circleRadius * 2.0f) * (1.0f - morphF) + targetW * morphF;
        float curH = (circleRadius * 2.0f) * (1.0f - morphF) + targetH * morphF;

        float minRadius = fminf(curW, curH) / 2.0f;
        float targetCorner = fmaxf(4.0f * S, minRadius * 0.05f);
        float cornerRadius = (minRadius * (1.0f - morphF) + targetCorner * morphF) * cornerMult;
        cornerRadius = fminf(cornerRadius, minRadius);

        float innerW = fmaxf(0.0f, curW / 2.0f - cornerRadius);
        float innerH = fmaxf(0.0f, curH / 2.0f - cornerRadius);

        float dxT = Cx - Tx, dyT = Cy - Ty;
        float L = sqrtf(dxT * dxT + dyT * dyT);
        float nx = 0.0f, ny = 0.0f;
        if (L > 0.001f) { nx = dxT / L; ny = dyT / L; }

        // Tail BASE (at head connection) auto-syncs to the blob's perpendicular
        // extent, slightly smaller (×0.9) so it visually fits INSIDE the blob
        // edge instead of overlapping it — exactly as requested:
        //   "основаніє хвоста шоб автоматично сінкувалось і було завжди трошки меньше"
        // No separate setting needed for base width; only the TIP width
        // (fluid_tailWidth) remains user-configurable.
        //
        //   vertical tail   (ny≈±1) → base diameter = curW × 0.9 (slightly < window width)
        //   horizontal tail (nx≈±1) → base diameter = curH × 0.9 (slightly < window height)
        //   diagonal               → smooth perpendicular projection
        float headR;
        if (L > 0.001f) {
            float perpExtent = curW * fabsf(ny) + curH * fabsf(nx);
            // Cap so we never exceed the larger dimension (prevents diagonal blow-up)
            float maxDim = fmaxf(curW, curH);
            if (perpExtent > maxDim) perpExtent = maxDim;
            // base diameter = perpExtent × 0.9 − 20 (extra 20 px so the tail
            // visually fits INSIDE the blob edge with a small margin)
            headR = perpExtent * 0.5f * 0.9f - 10.0f;
            if (headR < 4.0f) headR = 4.0f;
        } else {
            headR = minRadius;
        }

        float catchupBlobR = tailW_eff;
        if (blobT > 0.0f) catchupBlobR = tailW_eff + maxBlobR * clampf(blobT * 8.0f, 0.0f, 1.0f);

        float smin_k = fminf(headR * 0.6f * sminKMult, 80.0f + 40.0f * liquidity);

        float bbExt = fmaxf(curW, curH) / 2.0f + smin_k + 16.0f;
        int bbL = max_int(0, (int)(fminf(Cx - bbExt, fminf(Tx, Bx) - catchupBlobR - smin_k - 16.0f)));
        int bbR = min_int(bW, (int)(fmaxf(Cx + bbExt, fmaxf(Tx, Bx) + catchupBlobR + smin_k + 16.0f) + 1.0f));
        int bbT = max_int(0, (int)(fminf(Cy - bbExt, fminf(Ty, By) - catchupBlobR - smin_k - 16.0f)));
        int bbB = min_int(bH, (int)(fmaxf(Cy + bbExt, fmaxf(Ty, By) + catchupBlobR + smin_k + 16.0f) + 1.0f));

        for (int i = 0; i < 1024; i++) {
            float tp = i / 1023.0f;
            tailRadiusLUT[i] = tailW_eff + (headR - tailW_eff) * powf(tp, profileExp_eff);
        }

        // Smoother tail emergence: tail starts becoming visible at half of tailStartT,
        // ramps to ~40% solid by tailStartT, then full solid via tailT.
        // Fixes "tail doesn't always appear on first opening" issue.
        float earlyEmergence = 0.0f;
        if (t > tailStartT * 0.5f) {
            earlyEmergence = clampf((t - tailStartT * 0.5f) / (tailStartT * 0.5f + 0.001f), 0.0f, 1.0f) * 0.45f;
        }
        float detachSolidify = fmaxf(earlyEmergence, clampf(tailT * 10.0f, 0.0f, 1.0f));
        float invCurH = 1.0f / curH;
        float invCurW = 1.0f / curW;
        int   insetX  = min_int(edgeInset, w / 4);
        int   insetY  = min_int(edgeInset, h / 4);

        for (int y = bbT; y < bbB; y++) {
            uint32_t* dstRow = (uint32_t*)gb.workBits + y * bW;
            float py = (float)y - Cy;
            float py_tail = (float)y - Ty;

            float dy = fabsf(py) - innerH;
            float v_tex = py * invCurH + 0.5f;
            int sY = clamp_int((int)(v_tex * h), insetY, h - 1 - insetY);
            uint32_t* srcRow = (uint32_t*)data->pBits + sY * w;

            for (int x = bbL; x < bbR; x++) {
                float px = (float)x - Cx;
                float dx = fabsf(px) - innerW;

                float distBody = (dx > 0.0f && dy > 0.0f)
                    ? sqrtf(dx * dx + dy * dy) - cornerRadius
                    : fmaxf(dx, dy) - cornerRadius;

                float distTail = 9999.0f;
                float opacityFactor = 1.0f;

                if (L > 2.0f) {
                    float px_tail = (float)x - Tx;
                    float proj = px_tail * nx + py_tail * ny;
                    if (proj <= L + headR) {
                        float perp = px_tail * (-ny) + py_tail * nx;
                        float absPerp = fabsf(perp);
                        if (proj >= 0.0f && proj <= L) {
                            float projN = clampf(proj / L, 0.0f, 1.0f);
                            float rFunnel = tailRadiusLUT[(int)(projN * 1023.0f)];
                            distTail = absPerp - rFunnel;
                            if (tailFadeLengthPct > 0.001f) {
                                float fadeLen = L * tailFadeLengthPct;
                                if (proj < fadeLen) {
                                    float fadeOp = clampf(proj / fadeLen, 0.0f, 1.0f);
                                    opacityFactor = fadeOp + (1.0f - fadeOp) * detachSolidify;
                                }
                            }
                        } else if (proj < 0.0f) {
                            distTail = sqrtf(proj * proj + perp * perp) - tailW_eff;
                            if (tailFadeLengthPct > 0.001f) opacityFactor = detachSolidify;
                        }
                        if (blobT > 0.0f) {
                            float dxB = (float)x - Bx;
                            float dyB = (float)y - By;
                            float distBlob = sqrtf(dxB * dxB + dyB * dyB) - catchupBlobR;
                            distTail = smin(distTail, distBlob, headR * 0.2f);
                        }
                    }
                }

                // h_blend = 1 means body fully dominates the shape, 0 means tail dominates.
                // The smin already blends the SHAPE smoothly; we now also blend OPACITY
                // along the same axis — otherwise the boundary shows a hard step (the "seam").
                float h_blend = 1.0f;
                float finalDist = distBody;
                if (distTail < 999.0f) {
                    h_blend = clampf(0.5f + 0.5f * (distTail - distBody) / smin_k, 0.0f, 1.0f);
                    finalDist = distBody * h_blend + distTail * (1.0f - h_blend) - smin_k * h_blend * (1.0f - h_blend);
                }

                if (finalDist <= 1.0f) {
                    // Smooth body↔tail opacity blend. Body side = full opacity (1.0),
                    // tail side = the computed tail-fade. h_blend transitions between them.
                    float blendedOpacity = h_blend * 1.0f + (1.0f - h_blend) * opacityFactor;

                    float u = px * invCurW + 0.5f;
                    int sX = clamp_int((int)(u * w), insetX, w - 1 - insetX);
                    uint32_t pxColor = srcRow[sX];
                    if ((pxColor & 0xFF000000) != 0 || (pxColor & 0xFFFFFF) != 0) {
                        float pixelAlpha = (finalDist > 0.0f) ? (1.0f - finalDist) : 1.0f;
                        dstRow[x] = PremultiplyBlendFast(pxColor, pixelAlpha * blendedOpacity);
                    }
                }
            }
        }

        gb.Present(255);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: ⚫ BLACK HOLE
// =============================================================================

struct BHBlock { float r0; float a0; int x, y; };

static void RunBlackHole(GhostAnimData* data) {
    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y, bW = gb.w, bH = gb.h;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = GetAnimDuration(data), bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;

    float cx = (data->targetRect.left + data->width  / 2.0f) - X;
    float cy = (data->targetRect.top  + data->height / 2.0f) - Y;
    float twistForce = data->cfg.ext1 / 2.0f;
    uint32_t* srcPx = (uint32_t*)data->pBits;
    uint32_t* dstPx = (uint32_t*)gb.workBits;
    float maxR = sqrtf((data->width / 2.0f) * (data->width / 2.0f) + (data->height / 2.0f) * (data->height / 2.0f));
    if (maxR < 1.0f) maxR = 1.0f;
    float invMaxR = 1.0f / maxR;

    // Adaptive block size: keep ~30K particles regardless of window size to maintain FPS
    int bSize = data->cfg.ext2 == 1 ? 12 : (data->cfg.ext2 == 3 ? 3 : 6);
    int pixels = data->width * data->height;
    if      (pixels > 6000000) bSize = max_int(bSize, bSize * 2);     // 4K+
    else if (pixels > 2500000) bSize = max_int(bSize, (bSize * 3) / 2); // 1440p+
    else if (pixels > 1500000) bSize = max_int(bSize, bSize + 2);     // 1080p+

    // Pre-compute (r0, a0) for each block — halves per-frame trig work.
    std::vector<BHBlock> blocks;
    blocks.reserve((size_t)((data->width / bSize + 1) * (data->height / bSize + 1)));
    float wcx = data->width / 2.0f, wcy = data->height / 2.0f;
    for (int y = 0; y < data->height; y += bSize) {
        float dy = (y + bSize / 2.0f) - wcy;
        for (int x = 0; x < data->width; x += bSize) {
            float dx = (x + bSize / 2.0f) - wcx;
            BHBlock b;
            b.r0 = sqrtf(dx * dx + dy * dy);
            b.a0 = atan2f(dy, dx);
            b.x = x; b.y = y;
            blocks.push_back(b);
        }
    }

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear();

        if (elapsed <= animDur || !data->isRising) {
            float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
            float t = data->isRising ? (1.0f - p) : p;
            float pull = powf(t, 2.5f);
            float blockAlpha = 1.0f;
            if (t > 0.7f) blockAlpha = 1.0f - (t - 0.7f) / 0.3f;

            // PHASE SEPARATION — clean visual distinction prevents the "double-window" overlap:
            //   t < 0.30           → PURE base (full window, no particles)
            //   t in 0.30..0.55    → cross-fade (base out, particles in)
            //   t > 0.55           → PURE particles (no base)
            // By the time particles fade in, pull has displaced them enough (~0.10 r0) to
            // look like genuine particle motion, not a duplicate of the base layer.
            float baseAlpha = 1.0f - clampf((t - 0.30f) / 0.25f, 0.0f, 1.0f);
            baseAlpha = baseAlpha * baseAlpha;  // ease-out fade

            if (baseAlpha > 0.02f) {
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)(baseAlpha * 255.0f), AC_SRC_ALPHA };
                AlphaBlend(gb.hWorkDC,
                    (int)(cx - wcx), (int)(cy - wcy),
                    data->width, data->height,
                    hSrcDC, 0, 0, data->width, data->height, bf);
            }

            // Particle visibility ramps in 0.30..0.55, then full until fade-out at 0.70+.
            float particleVisibility = clampf((t - 0.30f) / 0.25f, 0.0f, 1.0f);
            float particleAlpha = blockAlpha * particleVisibility;

            if (pull > 0.05f && particleAlpha > 0.02f) {
                BYTE pa255 = (BYTE)(particleAlpha * 255.0f);
                bool fastPath = (pa255 == 255);   // skip per-pixel alpha math when fully opaque
                for (auto& blk : blocks) {
                    float r_new = blk.r0 * (1.0f - pull);
                    float a_new = blk.a0 + twistForce * pull * (1.5f - blk.r0 * invMaxR);
                    int destX = (int)(cx + cosf(a_new) * r_new) - bSize / 2;
                    int destY = (int)(cy + sinf(a_new) * r_new) - bSize / 2;
                    // Early reject blocks completely off-buffer
                    if (destX + bSize <= 0 || destX >= bW ||
                        destY + bSize <= 0 || destY >= bH) continue;
                    for (int by = 0; by < bSize && blk.y + by < data->height; by++) {
                        int fY = destY + by;
                        if (fY < 0 || fY >= bH) continue;
                        uint32_t* dstRow = dstPx + fY * bW;
                        uint32_t* srcRow = srcPx + (blk.y + by) * data->width + blk.x;
                        for (int bx = 0; bx < bSize && blk.x + bx < data->width; bx++) {
                            int fX = destX + bx;
                            if (fX < 0 || fX >= bW) continue;
                            uint32_t px = srcRow[bx];
                            if (!px) continue;
                            if (fastPath) {
                                dstRow[fX] = AlphaBlendPixels(dstRow[fX], px);
                            } else {
                                uint32_t pm = PremultiplyBlendFast(px, particleAlpha);
                                if (pm) dstRow[fX] = AlphaBlendPixels(dstRow[fX], pm);
                            }
                        }
                    }
                }
            }
        } else {
            float tB = clampf((float)((elapsed - animDur) / bounceDur), 0.0f, 1.0f);
            float scale = SpringBounce(tB, (float)data->cfg.bounceStr);
            int curW = max_int(2, (int)(data->width  * scale));
            int curH = max_int(2, (int)(data->height * scale));
            StretchBlt(gb.hWorkDC, (int)(cx - curW / 2.0f), (int)(cy - curH / 2.0f), curW, curH,
                       hSrcDC, 0, 0, data->width, data->height, SRCCOPY);
        }

        gb.Present(255);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: ⏳ SAND
// =============================================================================

// Sand grain: stores rest position + initial velocity directly (analytic ballistic).
// Cleaner & more predictable than driftX/driftY accumulators.
struct SandGrain {
    int   srcX, srcY;
    float restX, restY;   // buffer-local rest position
    float vx, vy;          // initial velocity (px per unit of fallT)
    float speedMult;       // per-grain gravity scaling for variety
    float delay;
};

static void RunSand(GhostAnimData* data) {
    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y, bW = gb.w, bH = gb.h;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = GetAnimDuration(data), bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;

    int   blockSize    = max_int(1, data->cfg.ext1);
    float gravity      = (float)data->cfg.ext2;
    float styleBlend   = clampf(data->cfg.ext3 / 100.0f, 0.0f, 1.0f);
    float fadeStartPct = clampf(data->cfg.ext4 / 100.0f, 0.0f, 0.99f);
    float scatterX     = (float)data->cfg.ext5;
    float scatterY     = (float)data->cfg.ext6;
    float windForce    = (float)data->cfg.ext7;

    uint32_t* srcPx = (uint32_t*)data->pBits;
    uint32_t* dstPx = (uint32_t*)gb.workBits;
    uint32_t rng = (uint32_t)GetTickCount64() ^ (uint32_t)(uintptr_t)data->hRealWnd;

    std::vector<SandGrain> grains;
    grains.reserve((size_t)(data->width / blockSize + 1) * (data->height / blockSize + 1));

    float invHalfW = 1.0f / (data->width  / 2.0f);
    float invHalfH = 1.0f / (data->height / 2.0f);
    int   winLeft  = data->targetRect.left - X;
    int   winTop   = data->targetRect.top  - Y;

    for (int y = 0; y < data->height; y += blockSize) {
        float vy_norm = ((float)y - data->height / 2.0f) * invHalfH;
        float crumbleDelay = (1.0f - ((float)y / data->height)) * 0.8f;
        for (int x = 0; x < data->width; x += blockSize) {
            float r1 = FastRandFloat(rng), r2 = FastRandFloat(rng);
            float vx_norm = ((float)x - data->width / 2.0f) * invHalfW;
            float explodeR = sqrtf(vx_norm * vx_norm + vy_norm * vy_norm);
            float explodeDelay = explodeR * 0.5f + r1 * 0.2f;
            float delay = crumbleDelay * (1.0f - styleBlend) + explodeDelay * styleBlend + (r2 * 0.10f);
            if (delay > 0.9f) delay = 0.9f;

            SandGrain g;
            g.srcX = x; g.srcY = y;
            g.restX = (float)(winLeft + x);
            g.restY = (float)(winTop  + y);
            g.delay = delay;
            g.speedMult = 0.6f + r2 * 0.9f;
            // Outward radial burst (×2.5) + per-grain jitter for organic variety
            g.vx = vx_norm * scatterX * styleBlend * 2.5f
                 + (r1 - 0.5f) * scatterX * 0.4f * styleBlend;
            // Vertical: outward + jitter + upward kick → firework parabolas in explosion mode
            g.vy = vy_norm * scatterY * styleBlend * 1.8f
                 + (r2 - 0.5f) * scatterY * 0.2f * styleBlend
                 - 90.0f * styleBlend;
            grains.push_back(g);
        }
    }

    float invFadeLen = 1.0f / (1.0f - fadeStartPct);
    // Less gravity dampening in explosion so grains DO fall back down (parabolic arcs)
    float gravDampen = 1.0f - styleBlend * 0.55f;

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear();

        if (elapsed <= animDur || !data->isRising) {
            float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
            float t = data->isRising ? (1.0f - p) : p;

            for (auto& g : grains) {
                if (t < g.delay) {
                    // Resting: draw grain at its original position
                    int sx0 = (int)g.restX;
                    int sy0 = (int)g.restY;
                    for (int by = 0; by < blockSize && g.srcY + by < data->height; by++) {
                        int sy = sy0 + by;
                        if (sy < 0 || sy >= bH) continue;
                        uint32_t* dstRow = dstPx + sy * bW;
                        uint32_t* srcRow = srcPx + (g.srcY + by) * data->width + g.srcX;
                        for (int bx = 0; bx < blockSize && g.srcX + bx < data->width; bx++) {
                            int sx = sx0 + bx;
                            if (sx < 0 || sx >= bW) continue;
                            dstRow[sx] = srcRow[bx];
                        }
                    }
                } else {
                    float fallT = clampf((t - g.delay) / (1.0f - g.delay), 0.0f, 1.0f);
                    float blockAlpha = 1.0f;
                    if (fallT > fadeStartPct) blockAlpha = 1.0f - (fallT - fadeStartPct) * invFadeLen;
                    if (blockAlpha <= 0) continue;

                    // Analytic ballistic: pos = rest + (v + wind)*t + 0.5*g*t² (gravity only on Y)
                    int dispX = (int)((g.vx + windForce) * fallT);
                    int dispY = (int)(g.vy * fallT
                              + gravity * fallT * fallT * g.speedMult * gravDampen);
                    int sx0 = (int)g.restX + dispX;
                    int sy0 = (int)g.restY + dispY;

                    for (int by = 0; by < blockSize && g.srcY + by < data->height; by++) {
                        int sy = sy0 + by;
                        if (sy < 0 || sy >= bH) continue;
                        uint32_t* dstRow = dstPx + sy * bW;
                        uint32_t* srcRow = srcPx + (g.srcY + by) * data->width + g.srcX;
                        for (int bx = 0; bx < blockSize && g.srcX + bx < data->width; bx++) {
                            int sx = sx0 + bx;
                            if (sx < 0 || sx >= bW) continue;
                            uint32_t px = srcRow[bx];
                            if (!px) continue;
                            uint32_t pm = PremultiplyBlendFast(px, blockAlpha);
                            dstRow[sx] = AlphaBlendPixels(dstRow[sx], pm);
                        }
                    }
                }
            }
        } else {
            float tB = clampf((float)((elapsed - animDur) / bounceDur), 0.0f, 1.0f);
            float scale = SpringBounce(tB, (float)data->cfg.bounceStr);
            int curW = max_int(2, (int)(data->width  * scale));
            int curH = max_int(2, (int)(data->height * scale));
            StretchBlt(gb.hWorkDC,
                winLeft + data->width / 2 - curW / 2,
                winTop  + data->height / 2 - curH / 2,
                curW, curH, hSrcDC, 0, 0, data->width, data->height, SRCCOPY);
        }

        gb.Present(255);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   EFFECT: 💎 GLASS
// =============================================================================

struct GlassShard {
    float lx[6], ly[6];
    float edx[6], edy[6];
    int   count;
    float cx, cy;
    float delay, speed, dirX, dirY, rotVel;
};

static void RunGlass(GhostAnimData* data) {
    HDC hScreenDC = GetDC(NULL);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    GhostBuf gb;
    if (!gb.Init(ComputeAnimBBox(data), hScreenDC)) {
        SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
        RevealRealWindow(data); return;
    }
    int X = gb.x, Y = gb.y, bW = gb.w, bH = gb.h;

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    BOOL firstFrame = TRUE;
    double animDur = GetAnimDuration(data), bounceDur = GetBounceDuration(data);
    double totalDur = animDur + bounceDur;

    float gravity      = (float)data->cfg.ext2;
    float styleBlend   = clampf(data->cfg.ext3 / 100.0f, 0.0f, 1.0f);
    float fadeStartPct = clampf(data->cfg.ext4 / 100.0f, 0.0f, 0.99f);
    float scatterX     = (float)data->cfg.ext5;
    float scatterY     = (float)data->cfg.ext6;
    int   shapeMode    = data->cfg.ext8;

    std::vector<GlassShard> shards;
    uint32_t rng = (uint32_t)GetTickCount64() ^ (uint32_t)(uintptr_t)data->hRealWnd;

    int targetRects = max_int(2, data->cfg.ext7 / 2);
    if      (shapeMode == 2) targetRects = min_int(targetRects, 15);
    else if (shapeMode == 3) targetRects = min_int(targetRects, 20);
    else if (shapeMode == 1) targetRects = min_int(targetRects, 30);

    std::vector<RECT> rects;
    rects.push_back({0, 0, data->width, data->height});

    while ((int)rects.size() < targetRects) {
        int idx = 0; int maxArea = 0;
        for (size_t i = 0; i < rects.size(); i++) {
            int a = (rects[i].right - rects[i].left) * (rects[i].bottom - rects[i].top);
            if (a > maxArea) { maxArea = a; idx = (int)i; }
        }
        if (maxArea < 100) break;
        RECT r = rects[idx];
        rects.erase(rects.begin() + idx);

        bool vert = (r.right - r.left) > (r.bottom - r.top);
        if ((FastRand(rng) % 100) < 20) vert = !vert;

        if (vert && (r.right - r.left) > 10) {
            int minW = (r.right - r.left) / 3;
            int maxW = (r.right - r.left) - minW;
            if (maxW <= minW) { rects.push_back(r); continue; }
            int sx = r.left + minW + (FastRand(rng) % (maxW - minW + 1));
            rects.push_back({r.left, r.top, sx, r.bottom});
            rects.push_back({sx,     r.top, r.right, r.bottom});
        } else if (!vert && (r.bottom - r.top) > 10) {
            int minH = (r.bottom - r.top) / 3;
            int maxH = (r.bottom - r.top) - minH;
            if (maxH <= minH) { rects.push_back(r); continue; }
            int sy = r.top + minH + (FastRand(rng) % (maxH - minH + 1));
            rects.push_back({r.left, r.top, r.right, sy});
            rects.push_back({r.left, sy,    r.right, r.bottom});
        } else { rects.push_back(r); break; }
    }

    int winLeft = data->targetRect.left - X;
    int winTop  = data->targetRect.top  - Y;

    for (auto& rc : rects) {
        int st = shapeMode;
        if (st == 3) st = FastRand(rng) % 3;

        auto addP = [&](std::vector<POINT> pts, float mulSp = 1.0f) {
            if (pts.size() < 3 || pts.size() > 6) return;
            GlassShard s;
            s.count = (int)pts.size();
            float sx = 0, sy = 0;
            for (auto& p : pts) { sx += p.x; sy += p.y; }
            float lcx = sx / s.count, lcy = sy / s.count;
            std::sort(pts.begin(), pts.end(), [&](const POINT& a, const POINT& b) {
                return atan2f(a.y - lcy, a.x - lcx) < atan2f(b.y - lcy, b.x - lcx);
            });
            s.cx = winLeft + lcx; s.cy = winTop + lcy;
            for (int i = 0; i < s.count; i++) { s.lx[i] = pts[i].x - lcx; s.ly[i] = pts[i].y - lcy; }
            for (int i = 0; i < s.count; i++) {
                int j = (i + 1) % s.count;
                s.edx[i] = s.lx[j] - s.lx[i];
                s.edy[i] = s.ly[j] - s.ly[i];
            }
            float vX = (lcx - data->width  / 2.0f) / (data->width  / 2.0f);
            float vY = (lcy - data->height / 2.0f) / (data->height / 2.0f);
            float eR = sqrtf(vX * vX + vY * vY);
            float rr = FastRandFloat(rng);
            float crumble = (1.0f - (lcy / data->height)) * 0.8f;
            s.delay = crumble * (1.0f - styleBlend) + (eR * 0.5f + rr * 0.2f) * styleBlend + FastRandFloat(rng) * 0.1f;
            if (s.delay > 0.9f) s.delay = 0.9f;
            s.speed = (0.6f + rr * 0.8f) * mulSp;
            s.dirX = (vX * scatterX * styleBlend * 2.0f + (rr - 0.5f) * scatterX * 0.5f) * mulSp;
            s.dirY = (vY * scatterY * styleBlend * 2.0f + (FastRandFloat(rng) - 0.5f) * scatterY * 0.5f) * mulSp;
            s.rotVel = (rr - 0.5f) * 15.0f * mulSp;
            shards.push_back(s);
        };

        POINT pTL = {rc.left, rc.top}, pTR = {rc.right, rc.top};
        POINT pBL = {rc.left, rc.bottom}, pBR = {rc.right, rc.bottom};

        if (st == 0) {
            if (FastRand(rng) % 2 == 0) { addP({pTL, pTR, pBR}); addP({pTL, pBR, pBL}); }
            else                         { addP({pTL, pTR, pBL}); addP({pTR, pBR, pBL}); }
        } else if (st == 1 || (rc.right - rc.left < 15 || rc.bottom - rc.top < 15)) {
            addP({pTL, pTR, pBR, pBL});
        } else if (st == 2) {
            int rw = rc.right - rc.left, rh = rc.bottom - rc.top;
            POINT p1 = {rc.left + rw / 2 + (int)((FastRand(rng) % (rw / 4 + 1)) - rw / 8), rc.top};
            POINT p2 = {rc.right, rc.top + rh / 3 + (int)((FastRand(rng) % (rh / 4 + 1)) - rh / 8)};
            POINT p3 = {rc.right - rw / 4 + (int)((FastRand(rng) % (rw / 8 + 1)) - rw / 16), rc.bottom};
            POINT p4 = {rc.left + rw / 4 + (int)((FastRand(rng) % (rw / 8 + 1)) - rw / 16), rc.bottom};
            POINT p5 = {rc.left, rc.bottom - rh / 2 + (int)((FastRand(rng) % (rh / 4 + 1)) - rh / 8)};
            addP({p1, p2, p3, p4, p5});
            addP({pTL, p1, p5}, 1.5f);
            addP({p1, pTR, p2}, 1.5f);
            addP({p2, pBR, p3}, 1.5f);
            addP({p4, pBL, p5}, 1.5f);
        }
    }

    std::sort(shards.begin(), shards.end(), [](const GlassShard& a, const GlassShard& b) { return a.delay < b.delay; });

    uint32_t* srcPx = (uint32_t*)data->pBits;
    uint32_t* dstPx = (uint32_t*)gb.workBits;
    float invFadeLen = 1.0f / (1.0f - fadeStartPct);

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        gb.Clear();

        if (elapsed <= animDur || !data->isRising) {
            float p = clampf((float)(elapsed / animDur), 0.0f, 1.0f);
            float t = data->isRising ? (1.0f - p) : p;

            for (auto& s : shards) {
                float angle = 0, curCx = s.cx, curCy = s.cy;
                float blockAlpha = 1.0f, blockScale = 1.0f;
                if (t > s.delay) {
                    float fT = clampf((t - s.delay) / (1.0f - s.delay), 0.0f, 1.0f);
                    curCx += s.dirX * fT;
                    curCy += s.dirY * fT + gravity * fT * fT * s.speed * (1.0f - styleBlend * 0.8f);
                    angle = s.rotVel * fT;
                    if (fT > fadeStartPct) blockAlpha = 1.0f - (fT - fadeStartPct) * invFadeLen;
                    blockScale = 1.0f - (fT * 0.5f * styleBlend);
                }
                if (blockAlpha <= 0) continue;

                float cosA = cosf(angle), sinA = sinf(angle);
                float invScale = 1.0f / blockScale;

                // Compute rotated vertices & bbox
                float rx[6], ry[6];
                float mnX = 99999, mxX = -99999, mnY = 99999, mxY = -99999;
                for (int i = 0; i < s.count; i++) {
                    float sx = s.lx[i] * blockScale, sy = s.ly[i] * blockScale;
                    rx[i] = curCx + sx * cosA - sy * sinA;
                    ry[i] = curCy + sx * sinA + sy * cosA;
                    if (rx[i] < mnX) mnX = rx[i];
                    if (rx[i] > mxX) mxX = rx[i];
                    if (ry[i] < mnY) mnY = ry[i];
                    if (ry[i] > mxY) mxY = ry[i];
                }
                int iMnX = max_int(0, (int)mnX), iMxX = min_int(bW - 1, (int)(mxX + 1));
                int iMnY = max_int(0, (int)mnY), iMxY = min_int(bH - 1, (int)(mxY + 1));

                // Incremental edge functions:
                //   cross[i](unX, unY) = A[i]*unX + B[i]*unY + C[i]
                //   A[i]=-edy[i], B[i]=edx[i], C[i]=edy[i]*lx[i] - edx[i]*ly[i]
                // Per x++: unX += cosA*invScale, unY += -sinA*invScale → cross[i] += dC[i]
                float A[6], B[6], C[6], dC[6];
                float dux = cosA * invScale, duy = -sinA * invScale;
                for (int i = 0; i < s.count; i++) {
                    A[i] = -s.edy[i];
                    B[i] =  s.edx[i];
                    C[i] =  s.edy[i] * s.lx[i] - s.edx[i] * s.ly[i];
                    dC[i] = A[i] * dux + B[i] * duy;
                }

                float srcOriginX = s.cx - winLeft;
                float srcOriginY = s.cy - winTop;

                for (int y = iMnY; y <= iMxY; y++) {
                    uint32_t* dstRow = dstPx + y * bW;
                    float dxS = (float)iMnX - curCx;
                    float dyP = (float)y    - curCy;
                    float localX = dxS * cosA + dyP * sinA;
                    float localY = -dxS * sinA + dyP * cosA;
                    float unX = localX * invScale;
                    float unY = localY * invScale;

                    // Initialize cross[] at first pixel of scanline
                    float cross[6];
                    for (int i = 0; i < s.count; i++) cross[i] = A[i] * unX + B[i] * unY + C[i];

                    for (int x = iMnX; x <= iMxX; x++) {
                        bool inside = true;
                        for (int i = 0; i < s.count; i++) {
                            if (cross[i] < -0.5f) { inside = false; break; }
                        }
                        if (inside) {
                            int srcX = (int)(srcOriginX + unX);
                            int srcY = (int)(srcOriginY + unY);
                            if ((unsigned)srcX < (unsigned)data->width && (unsigned)srcY < (unsigned)data->height) {
                                uint32_t px = srcPx[srcY * data->width + srcX];
                                if ((px & 0xFF000000) != 0)
                                    dstRow[x] = PremultiplyBlendFast(px, blockAlpha);
                            }
                        }
                        // Advance to next pixel: just adds, no multiplies
                        for (int i = 0; i < s.count; i++) cross[i] += dC[i];
                        unX += dux;
                        unY += duy;
                    }
                }
            }
        } else {
            float tB = clampf((float)((elapsed - animDur) / bounceDur), 0.0f, 1.0f);
            float scale = SpringBounce(tB, (float)data->cfg.bounceStr);
            int curW = max_int(2, (int)(data->width  * scale));
            int curH = max_int(2, (int)(data->height * scale));
            StretchBlt(gb.hWorkDC,
                winLeft + data->width  / 2 - curW / 2,
                winTop  + data->height / 2 - curH / 2,
                curW, curH, hSrcDC, 0, 0, data->width, data->height, SRCCOPY);
        }

        gb.Present(255);
        SignalReady(data, firstFrame, gb);
        if (elapsed >= totalDur) break;
        DwmFlush();
    }
    RevealRealWindow(data);
    SelectObject(hSrcDC, hOldSrc); DeleteDC(hSrcDC); ReleaseDC(NULL, hScreenDC);
}

// =============================================================================
//   ANIMATION WORKER THREAD
// =============================================================================

static DWORD WINAPI GhostAnimationThread(LPVOID lpParam) {
    GhostAnimData* data = (GhostAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    switch (data->mode) {
        case MODE_SCALE:     RunScale(data); break;
        case MODE_ZOOM:      RunZoom(data); break;
        case MODE_CRT:       RunCRT(data); break;
        case MODE_FLUID:     RunFluid(data); break;
        case MODE_GENIE:     RunGenie(data); break;
        case MODE_BLACKHOLE: RunBlackHole(data); break;
        case MODE_SAND:      RunSand(data); break;
        case MODE_GLASS:     RunGlass(data); break;
        case MODE_SWIPE:     RunSwipe(data); break;
    }

    if (data->hReadyEvent) { CloseHandle(data->hReadyEvent); data->hReadyEvent = NULL; }
    DeleteObject(data->hBitmap);
    HWND realWnd = data->hRealWnd;
    delete data;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_animatingWindows.erase(realWnd);
    }
    g_activeAnims.fetch_sub(1, std::memory_order_relaxed);
    return 0;
}

static void PruneSnapshotCache_NoLock() {
    while (g_SnapshotOrder.size() > MAX_SNAPSHOT_CACHE) {
        HWND oldest = g_SnapshotOrder.front();
        g_SnapshotOrder.pop_front();
        auto it = g_SnapshotCache.find(oldest);
        if (it != g_SnapshotCache.end()) { DeleteObject(it->second); g_SnapshotCache.erase(it); }
    }
}

// =============================================================================
//   SMART TASKBAR ICON TARGETING (multi-monitor, normalized matching)
// =============================================================================

static std::wstring NormalizeForMatch(std::wstring s) {
    for (auto& c : s) {
        if (c == L'_' || c == L'-') c = L' ';
        else c = towlower(c);
    }
    while (!s.empty() && iswspace(s.back()))  s.pop_back();
    while (!s.empty() && iswspace(s.front())) s.erase(0, 1);
    auto strip = [&](const std::wstring& pre) {
        if (s.length() > pre.length() && s.compare(0, pre.length(), pre) == 0)
            s = s.substr(pre.length());
    };
    strip(L"windows ");
    strip(L"microsoft ");
    return s;
}

// Multi-language ignore list — system taskbar items that aren't user apps.
// Localized for English / Ukrainian / Russian (the three most common cases I
// can verify). Extend as needed for other locales.
static bool IsIgnoredTaskbarName(const std::wstring& n) {
    if (n.length() <= 2) return true;
    static const wchar_t* const kIgnore[] = {
        L"start", L"пуск",
        L"search", L"пошук", L"поиск",
        L"task view", L"перегляд завдань", L"представление задач",
        L"widgets", L"віджети", L"виджеты",
        L"chat", L"чат",
        L"copilot",
        L"notification chevron", L"chevron",
        L"show hidden icons", L"показати приховані піктограми", L"показать скрытые значки",
        L"meet now",
        L"network", L"мережа", L"сеть",
        L"volume", L"гучність", L"громкость",
        L"power", L"живлення", L"питание",
        L"battery", L"батарея",
        L"action center", L"центр сповіщень", L"центр уведомлений",
        L"clock", L"годинник", L"часы",
        L"language", L"мова", L"язык",
        L"input indicator",
        L"more options", L"меню",
        L"recycle bin", L"кошик", L"корзина",
        NULL
    };
    for (int i = 0; kIgnore[i]; i++) {
        if (n == kIgnore[i]) return true;
    }
    return false;
}

// Multi-language exe → known taskbar name fragments. Each row maps an exe
// (without .exe, lowercased) to a list of name fragments any of which should
// match the taskbar item. Localized variants for common system apps.
static bool MatchByAlias(const std::wstring& exe, const std::wstring& candidate) {
    if (exe.empty()) return false;
    // Direct substring match (cheap path)
    if (candidate.find(exe) != std::wstring::npos) return true;

    struct Alias { const wchar_t* exe; const wchar_t* names[8]; };
    static const Alias kAliases[] = {
        // Console hosts
        { L"powershell",      { L"powershell", NULL } },
        { L"pwsh",            { L"powershell", L"pwsh", NULL } },
        { L"cmd",             { L"command prompt", L"командний рядок", L"командна оболонка",
                                L"командная строка", NULL } },
        { L"conhost",         { L"console host", L"console", L"консоль", NULL } },
        { L"windowsterminal", { L"terminal", L"windows terminal", L"термінал", L"терминал", NULL } },
        { L"wt",              { L"terminal", NULL } },
        // Browsers
        { L"chrome",          { L"chrome", L"google chrome", NULL } },
        { L"firefox",         { L"firefox", L"mozilla firefox", NULL } },
        { L"msedge",          { L"edge", L"microsoft edge", NULL } },
        { L"opera",           { L"opera", NULL } },
        { L"brave",           { L"brave", NULL } },
        // Dev
        { L"devenv",          { L"visual studio", NULL } },
        { L"code",            { L"visual studio code", L"vscode", L"code", NULL } },
        { L"rider64",         { L"rider", NULL } },
        { L"webstorm64",      { L"webstorm", NULL } },
        { L"pycharm64",       { L"pycharm", NULL } },
        { L"idea64",          { L"intellij", L"idea", NULL } },
        // System
        { L"explorer",        { L"file explorer", L"провідник файлів", L"проводник",
                                L"провідник", L"проводник файлов", L"explorer", NULL } },
        { L"notepad",         { L"notepad", L"блокнот", NULL } },
        { L"calc",            { L"calculator", L"калькулятор", NULL } },
        { L"taskmgr",         { L"task manager", L"диспетчер завдань", L"диспетчер задач", NULL } },
        { L"control",         { L"control panel", L"панель керування", L"панель управления", NULL } },
        { L"mspaint",         { L"paint", NULL } },
        { L"wordpad",         { L"wordpad", NULL } },
        { L"snippingtool",    { L"snipping tool", L"ножиці", L"ножницы", NULL } },
        { L"screensketch",    { L"snip", L"набросок", NULL } },
        // Office
        { L"winword",         { L"word", L"microsoft word", NULL } },
        { L"excel",           { L"excel", L"microsoft excel", NULL } },
        { L"powerpnt",        { L"powerpoint", L"microsoft powerpoint", NULL } },
        { L"outlook",         { L"outlook", L"microsoft outlook", NULL } },
        // Chat
        { L"discord",         { L"discord", NULL } },
        { L"telegram",        { L"telegram", NULL } },
        { L"whatsapp",        { L"whatsapp", NULL } },
        { L"slack",           { L"slack", NULL } },
    };
    for (const auto& a : kAliases) {
        if (exe == a.exe) {
            for (int i = 0; i < 8 && a.names[i]; i++) {
                if (candidate.find(a.names[i]) != std::wstring::npos) return true;
            }
            return false;
        }
    }
    return false;
}

// Read the FileDescription string from the exe's version resource.
// This is typically the developer-set app name in English — perfect for
// matching even on localized Windows where the title is translated.
static std::wstring GetExeFileDescription(const wchar_t* exeKey) {
    if (!exeKey || !*exeKey) return std::wstring();
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeW(exeKey, &handle);
    if (size == 0 || size > 65536) return std::wstring();
    std::vector<BYTE> buf(size);
    if (!GetFileVersionInfoW(exeKey, 0, size, buf.data())) return std::wstring();

    struct LangCp { WORD wLang; WORD wCp; };
    LangCp* pTr = NULL;
    UINT tsize = 0;
    if (!VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&pTr, &tsize) || !pTr || tsize < sizeof(LangCp))
        return std::wstring();

    wchar_t key[128];
    wsprintfW(key, L"\\StringFileInfo\\%04x%04x\\FileDescription", pTr[0].wLang, pTr[0].wCp);
    wchar_t* desc = NULL;
    UINT dsize = 0;
    if (!VerQueryValueW(buf.data(), key, (LPVOID*)&desc, &dsize) || !desc)
        return std::wstring();
    return std::wstring(desc);
}

// Score a candidate taskbar button name against a target window.
// Higher = better match. 0 = no match at all.
static int ScoreTaskbarMatch(const std::wstring& target, const std::wstring& exe,
                              const std::wstring& desc,   const std::wstring& candidate) {
    if (candidate.empty()) return 0;

    // 1. Alias hit — highest confidence (exe + known localized name)
    if (!exe.empty() && MatchByAlias(exe, candidate)) return 100;

    // 2. Direct title match (substring either way, full title)
    if (!target.empty()) {
        if (candidate.find(target) != std::wstring::npos) return 92;
        if (target.find(candidate) != std::wstring::npos && candidate.length() > 3) return 88;
    }

    // 3. FileDescription substring (usually English, language-agnostic)
    if (!desc.empty() && desc.length() > 2) {
        if (candidate.find(desc) != std::wstring::npos) return 82;
        if (desc.find(candidate) != std::wstring::npos && candidate.length() > 3) return 78;
    }

    // 4. Exe substring (often matches for 3rd-party apps)
    if (!exe.empty() && exe.length() > 3) {
        if (candidate.find(exe) != std::wstring::npos) return 65;
    }

    // 5. Token-split: "Document - Notepad" → try "Document", "Notepad" separately
    if (!target.empty()) {
        size_t pos = 0;
        while (pos < target.length()) {
            size_t sep = target.find(L" - ", pos);
            size_t end = (sep == std::wstring::npos) ? target.length() : sep;
            std::wstring tok = target.substr(pos, end - pos);
            while (!tok.empty() && iswspace(tok.back()))  tok.pop_back();
            while (!tok.empty() && iswspace(tok.front())) tok.erase(0, 1);
            if (tok.length() > 3 && candidate.find(tok) != std::wstring::npos) return 55;
            if (sep == std::wstring::npos) break;
            pos = sep + 3;
        }
    }

    return 0;
}

static HWND GetTaskbarForMonitor(HMONITOR hMon) {
    HWND hPrimary = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hPrimary && MonitorFromWindow(hPrimary, MONITOR_DEFAULTTONEAREST) == hMon)
        return hPrimary;
    HWND hSec = FindWindowExW(NULL, NULL, L"Shell_SecondaryTrayWnd", NULL);
    while (hSec) {
        if (MonitorFromWindow(hSec, MONITOR_DEFAULTTONEAREST) == hMon) return hSec;
        hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL);
    }
    return hPrimary;
}

// Drill down to the actual icon container (MSTaskListWClass) inside a taskbar.
// Skipping Start/Search/Widgets/SystemTray nodes makes MSAA matching way more
// reliable for apps like PowerShell, Terminal, etc.
static HWND FindTaskListInTaskbar(HWND hTb) {
    if (!hTb) return NULL;
    // Win10 path: Shell_TrayWnd > ReBarWindow32 > MSTaskSwWClass > MSTaskListWClass
    HWND hReBar  = FindWindowExW(hTb, NULL, L"ReBarWindow32", NULL);
    HWND hTaskSw = NULL;
    if (hReBar)  hTaskSw = FindWindowExW(hReBar, NULL, L"MSTaskSwWClass", NULL);
    if (!hTaskSw) hTaskSw = FindWindowExW(hTb, NULL, L"MSTaskSwWClass", NULL);
    HWND hList = NULL;
    if (hTaskSw) hList = FindWindowExW(hTaskSw, NULL, L"MSTaskListWClass", NULL);
    if (!hList)  hList = FindWindowExW(hTb, NULL, L"MSTaskListWClass", NULL);
    return hList;
}

// Collect identifiers used to score taskbar matches against a target window.
static void CollectTargetIdentifiers(HWND hWnd,
                                      std::wstring& title,
                                      std::wstring& exe,
                                      std::wstring& desc) {
    wchar_t titleBuf[512] = {0};
    GetWindowTextW(hWnd, titleBuf, 512);
    title = NormalizeForMatch(titleBuf);

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess) {
        wchar_t exeKey[MAX_PATH] = {0};
        DWORD sz = MAX_PATH;
        if (QueryFullProcessImageNameW(hProcess, 0, exeKey, &sz)) {
            std::wstring e = exeKey;
            size_t slash = e.find_last_of(L"\\/");
            if (slash != std::wstring::npos) e = e.substr(slash + 1);
            size_t dot = e.find_last_of(L".");
            if (dot != std::wstring::npos) e = e.substr(0, dot);
            exe  = NormalizeForMatch(e);
            desc = NormalizeForMatch(GetExeFileDescription(exeKey));
        }
        CloseHandle(hProcess);
    }
}

// Spatial probe — walks the taskbar geometrically with AccessibleObjectFromPoint
// at regular intervals. Works on Win11 XAML taskbars where the MSAA tree from
// Shell_TrayWnd often returns no usable buttons.
// COM must be initialized by the caller.
static void ProbeTaskbarSpatial(HWND hTb,
                                 const std::wstring& title,
                                 const std::wstring& exe,
                                 const std::wstring& desc,
                                 int& bestScore, int& bestX, int& bestY) {
    RECT tb;
    if (!GetWindowRect(hTb, &tb)) return;
    bool horizontal = (tb.right - tb.left) > (tb.bottom - tb.top);
    int span = horizontal ? (tb.right - tb.left) : (tb.bottom - tb.top);
    int step = span / 60; if (step < 24) step = 24;     // ~60 probes max
    long lastL = -99999, lastT = -99999;
    int virtW = GetSystemMetrics(SM_CXVIRTUALSCREEN);

    auto probe = [&](POINT pt) -> bool {
        IAccessible* pAcc = NULL;
        VARIANT v; VariantInit(&v);
        bool perfect = false;
        if (SUCCEEDED(AccessibleObjectFromPoint(pt, &pAcc, &v))) {
            long l = 0, t = 0, w = 0, h = 0;
            // Filter: valid rect, not whole-taskbar width, not the same button as last probe
            if (SUCCEEDED(pAcc->accLocation(&l, &t, &w, &h, v))
                && w > 0 && h > 0 && w < virtW
                && !(l == lastL && t == lastT)) {
                lastL = l; lastT = t;
                BSTR bstrName = NULL;
                if (SUCCEEDED(pAcc->get_accName(v, &bstrName)) && bstrName) {
                    std::wstring nm = NormalizeForMatch(bstrName);
                    SysFreeString(bstrName);
                    if (!IsIgnoredTaskbarName(nm)) {
                        int s = ScoreTaskbarMatch(title, exe, desc, nm);
                        if (s > bestScore) {
                            bestScore = s;
                            bestX = l + w / 2;
                            bestY = t + h / 2;
                            if (s >= 100) perfect = true;
                        }
                    }
                }
            }
            pAcc->Release();
        }
        VariantClear(&v);
        return perfect;
    };

    if (horizontal) {
        int midY = (tb.top + tb.bottom) / 2;
        for (int x = tb.left + 16; x < tb.right - 16; x += step) {
            if (probe({x, midY})) break;
        }
    } else {
        int midX = (tb.left + tb.right) / 2;
        for (int y = tb.top + 16; y < tb.bottom - 16; y += step) {
            if (probe({midX, y})) break;
        }
    }
}

// v2.0 — Language-independent taskbar targeting.
//
// Two-stage strategy for maximum compatibility across Win10/11 + any locale:
//   Stage A. MSAA tree walk from MSTaskListWClass (or Shell_TrayWnd fallback)
//     - Works great on Win10 classic taskbar
//     - Often returns NO usable buttons on Win11 XAML taskbar
//   Stage B. Spatial probe — sweeps the taskbar with AccessibleObjectFromPoint
//     - Works on BOTH Win10 (redundant if A worked) AND Win11 XAML
//     - Slightly slower (~150ms worst case) but result is cached per-window
//
// Buttons are scored, not just matched. Highest-scoring candidate wins.
// Matching uses: alias map (multi-locale) → window title → exe FileDescription
// → exe name → title token-split. FileDescription is read from the exe's
// version resource and is almost always English regardless of OS locale.
static BOOL GetTaskbarIconCenterFast(HWND hWndTarget, int& outX, int& outY) {
    HMONITOR hMon = MonitorFromWindow(hWndTarget, MONITOR_DEFAULTTONEAREST);
    HWND hTaskbar = GetTaskbarForMonitor(hMon);
    if (!hTaskbar) return FALSE;

    HWND hSearchRoot = FindTaskListInTaskbar(hTaskbar);
    bool narrowSearch = (hSearchRoot != NULL);
    if (!hSearchRoot) hSearchRoot = hTaskbar;

    std::wstring targetStr, exeStr, descStr;
    CollectTargetIdentifiers(hWndTarget, targetStr, exeStr, descStr);

    HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // --- Stage A: MSAA tree walk ---------------------------------------------
    int bestScore = 0;
    int bestX = 0, bestY = 0;
    IAccessible* pAcc = NULL;

    if (SUCCEEDED(AccessibleObjectFromWindow(hSearchRoot, OBJID_CLIENT, IID_IAccessible, (void**)&pAcc))) {
        std::deque<IAccessible*> queue;
        queue.push_back(pAcc);
        int maxDepth = narrowSearch ? 80 : 300;

        while (!queue.empty() && maxDepth-- > 0) {
            IAccessible* current = queue.front();
            queue.pop_front();

            long childCount = 0;
            if (SUCCEEDED(current->get_accChildCount(&childCount)) && childCount > 0 && childCount < 100) {
                VARIANT* pVars = new VARIANT[childCount];
                for (long i = 0; i < childCount; i++) VariantInit(&pVars[i]);
                long obtained = 0;

                if (SUCCEEDED(AccessibleChildren(current, 0, childCount, pVars, &obtained))) {
                    for (long i = 0; i < obtained; i++) {
                        IAccessible* pChild = NULL;
                        VARIANT varChild; VariantInit(&varChild);

                        if (pVars[i].vt == VT_DISPATCH && pVars[i].pdispVal) {
                            if (SUCCEEDED(pVars[i].pdispVal->QueryInterface(IID_IAccessible, (void**)&pChild))) {
                                varChild.vt = VT_I4; varChild.lVal = CHILDID_SELF;
                            }
                        } else if (pVars[i].vt == VT_I4) {
                            pChild = current;
                            pChild->AddRef();
                            varChild = pVars[i];
                        }

                        if (pChild) {
                            BSTR bstrName = NULL;
                            if (SUCCEEDED(pChild->get_accName(varChild, &bstrName)) && bstrName) {
                                std::wstring nameStr = NormalizeForMatch(bstrName);
                                SysFreeString(bstrName);

                                // Score this candidate (skip known shell items)
                                if (!IsIgnoredTaskbarName(nameStr)) {
                                    int score = ScoreTaskbarMatch(targetStr, exeStr, descStr, nameStr);
                                    if (score > bestScore) {
                                        long l = 0, t = 0, w = 0, hh = 0;
                                        if (SUCCEEDED(pChild->accLocation(&l, &t, &w, &hh, varChild))
                                            && w > 0 && hh > 0
                                            && w < GetSystemMetrics(SM_CXVIRTUALSCREEN)) {
                                            bestScore = score;
                                            bestX = l + w / 2;
                                            bestY = t + hh / 2;
                                        }
                                    }
                                }
                            }

                            // Queue VT_DISPATCH children for deeper traversal — but only if
                            // we haven't found a "perfect" alias hit yet (score 100).
                            if (bestScore < 100 && pVars[i].vt == VT_DISPATCH) queue.push_back(pChild);
                            else pChild->Release();
                        }

                        VariantClear(&pVars[i]);
                    }
                }
                delete[] pVars;
            }
            current->Release();
        }
        for (auto p : queue) p->Release();
    }

    // --- Stage B: Spatial probe ----------------------------------------------
    // Runs when MSAA tree gave us nothing solid. Critical for Win11 XAML
    // taskbars where the tree often exposes no usable buttons. Skipped only
    // when we already have a perfect (alias-hit) match from Stage A.
    if (bestScore < 100) {
        ProbeTaskbarSpatial(hTaskbar, targetStr, exeStr, descStr,
                            bestScore, bestX, bestY);
    }

    if (SUCCEEDED(hrInit)) CoUninitialize();

    if (bestScore > 0) {
        outX = bestX;
        outY = bestY;
        return TRUE;
    }
    return FALSE;
}

// =============================================================================
//   START ANIMATION
// =============================================================================

static void StartAnimation(HWND hWnd, BOOL isRising, BOOL isClosing) {
    if (g_shuttingDown.load()) return;

    if (g_activeAnims.load(std::memory_order_relaxed) >= MAX_CONCURRENT_ANIMS) {
        if (!isClosing && isRising) {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
            SetDwmTransitions(hWnd, TRUE);
        }
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_animatingWindows.count(hWnd)) return;
        g_animatingWindows.insert(hWnd);
    }

    RECT logRect, visRect; GetWindowRect(hWnd, &logRect);
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &visRect, sizeof(visRect))))
        visRect = logRect;

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi)) {
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0;
        mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        mi.rcWork = mi.rcMonitor;
    }

    // For maximized windows: if DWM gave us bounds clearly outside the monitor,
    // fall back to work area. Otherwise trust DWM (it knows the true visual rect).
    if (IsZoomed(hWnd)) {
        bool dwmOutOfBounds = (visRect.left < mi.rcMonitor.left - 4) ||
                              (visRect.top  < mi.rcMonitor.top  - 4) ||
                              (visRect.right > mi.rcMonitor.right + 4) ||
                              (visRect.bottom > mi.rcMonitor.bottom + 4);
        if (dwmOutOfBounds) {
            bool isFullscreenLike = (logRect.left <= mi.rcMonitor.left && logRect.top <= mi.rcMonitor.top &&
                                     logRect.right >= mi.rcMonitor.right && logRect.bottom >= mi.rcMonitor.bottom);
            visRect = isFullscreenLike ? mi.rcMonitor : mi.rcWork;
        }
    }

    int w = visRect.right - visRect.left, h = visRect.bottom - visRect.top;
    if (w <= 0 || h <= 0) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_animatingWindows.erase(hWnd);
        return;
    }

    int learnedX = visRect.left + w / 2;
    int learnedY = visRect.bottom;

    if (!isClosing) {
        // --- Resolve the exe FILE NAME for the per-exe cache ---------------
        // Key is filename only (lowercase, no path/extension): "powershell",
        // "notepad", "chrome". This unifies multiple installations of the
        // same app — e.g. PowerShell launched from a custom shortcut vs.
        // System32 path. One taskbar interaction = remembered everywhere.
        std::wstring exeKey;
        DWORD pidT = 0;
        GetWindowThreadProcessId(hWnd, &pidT);
        if (HANDLE hPr = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pidT)) {
            wchar_t buf[MAX_PATH] = {0};
            DWORD sz = MAX_PATH;
            if (QueryFullProcessImageNameW(hPr, 0, buf, &sz)) {
                std::wstring p = buf;
                size_t slash = p.find_last_of(L"\\/");
                if (slash != std::wstring::npos) p = p.substr(slash + 1);
                size_t dot = p.find_last_of(L".");
                if (dot != std::wstring::npos) p = p.substr(0, dot);
                for (auto& c : p) c = towlower(c);
                exeKey = p;
            }
            CloseHandle(hPr);
        }

        // === A. CURSOR-HOVER CAPTURE =======================================
        // If the cursor is on the taskbar at the moment of minimize/restore,
        // remember WHERE on the taskbar — that's the icon for this app.
        // This is the most reliable signal because the user just clicked or
        // hovered the icon. We cache by BOTH hwnd AND exe path so future
        // instances of the same app fly to the same icon too.
        POINT pt; GetCursorPos(&pt);
        HWND hHover = WindowFromPoint(pt);
        HWND hRoot  = GetAncestor(hHover, GA_ROOT);
        wchar_t cls[256] = {0};
        if (hRoot) GetClassNameW(hRoot, cls, 256);
        bool onTaskbar = (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
                          wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0);

        if (onTaskbar) {
            // First try the accessible object under cursor — gives us the icon's
            // actual rect (precise center). If that returns suspicious bounds
            // (whole-taskbar), fall back to the raw cursor X (etalon-style).
            int hitX = pt.x;
            int hitY = pt.y;
            bool accGood = false;

            IAccessible* pAcc = NULL;
            VARIANT varChild; VariantInit(&varChild);
            if (SUCCEEDED(AccessibleObjectFromPoint(pt, &pAcc, &varChild))) {
                long l = 0, t = 0, ww = 0, hh = 0;
                if (SUCCEEDED(pAcc->accLocation(&l, &t, &ww, &hh, varChild))
                    && ww > 8 && hh > 8 && ww < 200 && hh < 200) {
                    // Looks like an icon-sized rect → trust it
                    hitX = l + ww / 2;
                    hitY = t + hh / 2;
                    accGood = true;
                }
                pAcc->Release();
            }
            VariantClear(&varChild);

            if (!accGood) {
                // Use cursor X + taskbar middle Y (etalon fallback)
                RECT tbR;
                if (hRoot && GetWindowRect(hRoot, &tbR)) {
                    hitY = (tbR.top + tbR.bottom) / 2;
                }
            }

            {
                std::lock_guard<std::mutex> lock(g_CacheMutex);
                g_IconPositionsX[hWnd] = hitX;
                g_IconPositionsY[hWnd] = hitY;
                if (!exeKey.empty()) {
                    g_ExeIconCache[exeKey] = { hitX, hitY };
                }
            }
        }

        // === B. CACHE LOOKUP: HWND, then EXE ===============================
        // HWND cache catches the same window across minimize/restore cycles.
        // Exe cache catches FRESH instances of an app the user has used before
        // — critical because Win11 groups instances under one icon.
        bool cacheHit = false;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            auto itHw = g_IconPositionsX.find(hWnd);
            if (itHw != g_IconPositionsX.end()) {
                learnedX = itHw->second;
                learnedY = g_IconPositionsY[hWnd];
                cacheHit = true;
            } else if (!exeKey.empty()) {
                auto itEx = g_ExeIconCache.find(exeKey);
                if (itEx != g_ExeIconCache.end()) {
                    learnedX = itEx->second.x;
                    learnedY = itEx->second.y;
                    cacheHit = true;
                    // Promote into hwnd cache too so subsequent lookups are O(1)
                    g_IconPositionsX[hWnd] = learnedX;
                    g_IconPositionsY[hWnd] = learnedY;
                }
            }
        }

        // === C. MSAA + spatial probe =======================================
        if (!cacheHit) {
            int accX, accY;
            if (GetTaskbarIconCenterFast(hWnd, accX, accY)) {
                learnedX = accX; learnedY = accY;
                std::lock_guard<std::mutex> lock(g_CacheMutex);
                g_IconPositionsX[hWnd] = accX;
                g_IconPositionsY[hWnd] = accY;
                if (!exeKey.empty()) {
                    g_ExeIconCache[exeKey] = { accX, accY };
                }
            } else {
                // === D. Fallback ===========================================
                // Aim for the middle of the actual taskbar strip — Y in the
                // center of the bar, X in the center of the monitor. On Win11
                // centered taskbar this hits the icon row; on Win10 left-
                // aligned this is the icon column too (since they fill from
                // the left of the strip).
                HWND hTbFb = GetTaskbarForMonitor(hMon);
                RECT tbR;
                if (hTbFb && GetWindowRect(hTbFb, &tbR)) {
                    learnedY = (tbR.top + tbR.bottom) / 2;
                } else {
                    learnedY = mi.rcWork.bottom;
                }
                learnedX = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left) / 2;
            }
        }

        // === E. User Y-offset (minimize only) ==============================
        learnedY += g_dockOffsetY.load();
    } else {
        learnedX = visRect.left + w / 2;
        learnedY = visRect.top  + h / 2;
        // dock_offset_y intentionally NOT applied for close effects
    }

    int mode = isClosing ? g_closeMode.load() : g_minMode.load();
    if (mode == MODE_OFF) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_animatingWindows.erase(hWnd);
        return;
    }

    HANDLE hEventMain   = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE hEventThread = NULL;
    if (hEventMain) {
        DuplicateHandle(GetCurrentProcess(), hEventMain, GetCurrentProcess(),
                        &hEventThread, 0, FALSE, DUPLICATE_SAME_ACCESS);
    }

    GhostAnimData* data = new GhostAnimData();
    data->hRealWnd = hWnd; data->targetRect = visRect; data->width = w; data->height = h;
    data->isRising = isRising; data->isClosing = isClosing;
    data->targetDockX = learnedX; data->targetDockY = learnedY;
    data->originalExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    data->mode = mode;
    data->hReadyEvent = hEventThread;
    data->monRect = mi.rcMonitor;

    switch (mode) {
        case MODE_GENIE:     CopyEffectSettings(data->cfg, s_genie);     break;
        case MODE_FLUID:     CopyEffectSettings(data->cfg, s_fluid);     break;
        case MODE_SCALE:     CopyEffectSettings(data->cfg, s_scale);     break;
        case MODE_ZOOM:      CopyEffectSettings(data->cfg, s_zoom);      break;
        case MODE_SWIPE:     CopyEffectSettings(data->cfg, s_swipe);     break;
        case MODE_CRT:       CopyEffectSettings(data->cfg, s_crt);       break;
        case MODE_BLACKHOLE: CopyEffectSettings(data->cfg, s_blackhole); break;
        case MODE_SAND:      CopyEffectSettings(data->cfg, s_sand);      break;
        case MODE_GLASS:     CopyEffectSettings(data->cfg, s_glass);     break;
    }

    HDC hScreenDC = GetDC(NULL); HDC hMemDC = CreateCompatibleDC(hScreenDC);
    data->hBitmap = CreateArgbDib(hScreenDC, w, h, &data->pBits);
    if (!data->hBitmap) {
        DeleteDC(hMemDC); ReleaseDC(NULL, hScreenDC);
        if (hEventMain)   CloseHandle(hEventMain);
        if (hEventThread) CloseHandle(hEventThread);
        delete data;
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_animatingWindows.erase(hWnd);
        return;
    }
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, data->hBitmap);

    if (isRising && !isClosing) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            auto it = g_SnapshotCache.find(hWnd);
            if (it != g_SnapshotCache.end()) {
                HDC hCacheDC = CreateCompatibleDC(hScreenDC);
                HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, it->second);
                BitBlt(hMemDC, 0, 0, w, h, hCacheDC, 0, 0, SRCCOPY);
                SelectObject(hCacheDC, hOldCacheBmp); DeleteDC(hCacheDC);
                DeleteObject(it->second); g_SnapshotCache.erase(it);
                auto ord = std::find(g_SnapshotOrder.begin(), g_SnapshotOrder.end(), hWnd);
                if (ord != g_SnapshotOrder.end()) g_SnapshotOrder.erase(ord);
                fromCache = TRUE;
            }
        }
        if (!fromCache) {
            RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
            SetWindowOrgEx(hMemDC, visRect.left - logRect.left, visRect.top - logRect.top, NULL);
            PrintWindow(hWnd, hMemDC, PW_CLIENTONLY | 0x00000002);
            SetWindowOrgEx(hMemDC, 0, 0, NULL);
        }
    } else {
        // Detect whether the window extends BEYOND the monitor edges. If it
        // does, BitBlt from screen would capture BLACK for the off-screen
        // portion (the screen only has visible pixels). Use PrintWindow
        // instead — it renders the full window content regardless of which
        // pixels are currently visible.
        bool offscreen = (visRect.left   < mi.rcMonitor.left)
                      || (visRect.top    < mi.rcMonitor.top)
                      || (visRect.right  > mi.rcMonitor.right)
                      || (visRect.bottom > mi.rcMonitor.bottom);
        if (offscreen) {
            RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
            SetWindowOrgEx(hMemDC, visRect.left - logRect.left, visRect.top - logRect.top, NULL);
            PrintWindow(hWnd, hMemDC, PW_CLIENTONLY | 0x00000002);
            SetWindowOrgEx(hMemDC, 0, 0, NULL);
        } else {
            BitBlt(hMemDC, 0, 0, w, h, hScreenDC, visRect.left, visRect.top, SRCCOPY);
        }
        if (!isClosing) {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            auto it = g_SnapshotCache.find(hWnd);
            if (it != g_SnapshotCache.end()) {
                DeleteObject(it->second); g_SnapshotCache.erase(it);
                auto ord = std::find(g_SnapshotOrder.begin(), g_SnapshotOrder.end(), hWnd);
                if (ord != g_SnapshotOrder.end()) g_SnapshotOrder.erase(ord);
            }
            HBITMAP hCache = CreateCompatibleBitmap(hScreenDC, w, h);
            if (hCache) {
                HDC hCacheDC = CreateCompatibleDC(hScreenDC);
                HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, hCache);
                BitBlt(hCacheDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
                SelectObject(hCacheDC, hOldCacheBmp); DeleteDC(hCacheDC);
                g_SnapshotCache[hWnd] = hCache;
                g_SnapshotOrder.push_back(hWnd);
                PruneSnapshotCache_NoLock();
            }
        }
    }

    GdiFlush();
    SelectObject(hMemDC, hOldBmp); DeleteDC(hMemDC); ReleaseDC(NULL, hScreenDC);

    g_activeAnims.fetch_add(1, std::memory_order_relaxed);
    HANDLE hThread = CreateThread(NULL, 0, GhostAnimationThread, data, 0, NULL);
    if (hThread) {
        if (!isRising || isClosing) {
            if (hEventMain) WaitForSingleObject(hEventMain, 150);
            // Hide the real window for BOTH minimize and close to prevent the
            // ghost+real "double window" overlap during the animation.
            // RevealRealWindow() will restore it later if the close was cancelled.
            LONG_PTR ex = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
        }
        CloseHandle(hThread);
        if (hEventMain) CloseHandle(hEventMain);
    } else {
        DeleteObject(data->hBitmap);
        if (hEventMain)   CloseHandle(hEventMain);
        if (hEventThread) CloseHandle(hEventThread);
        delete data;
        g_activeAnims.fetch_sub(1, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_animatingWindows.erase(hWnd);
    }
}

// =============================================================================
//   WINDOW FILTER & HOOKS
// =============================================================================

static BOOL IsValidAnimWindow(HWND hWnd) {
    if (g_closeMode.load() == MODE_OFF) return FALSE;
    HWND hParent = GetParent(hWnd);
    if (hParent != NULL && hParent != GetDesktopWindow()) return FALSE;
    // UWP apps (Settings, Store, modern Calculator, ...) live inside
    // ApplicationFrameWindow which technically HAS an owner — but we still
    // want close animations for them. Allow these specific frame classes.
    if (GetWindow(hWnd, GW_OWNER) != NULL) {
        wchar_t ownedCls[256] = {0};
        if (!GetClassNameW(hWnd, ownedCls, 256) ||
            (wcscmp(ownedCls, L"ApplicationFrameWindow") != 0 &&
             wcscmp(ownedCls, L"Windows.UI.Core.CoreWindow") != 0)) {
            return FALSE;
        }
    }

    LONG style   = GetWindowLongW(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);

    if (!(style & WS_VISIBLE)) return FALSE;
    if (style & WS_CHILD) return FALSE;
    if (exStyle & WS_EX_TOOLWINDOW) return FALSE;
    if (IsIconic(hWnd)) return FALSE;
    if (IsHungAppWindow(hWnd)) return FALSE;

    wchar_t cls[256];
    if (GetClassNameW(hWnd, cls, 256)) {
        if (wcscmp(cls, L"FluidEngineGhost") == 0) return FALSE;
        if ((style & WS_POPUP) && !(style & WS_CAPTION)) {
            if (wcscmp(cls, L"ApplicationFrameWindow")    == 0 ||
                wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
                wcscmp(cls, L"Chrome_WidgetWin_1")        == 0 ||
                wcscmp(cls, L"MozillaWindowClass")        == 0 ||
                wcsstr(cls, L"Cef")                       != NULL) {
                return TRUE;
            }
            return FALSE;
        }
    }
    return TRUE;
}

static void TryCloseAnimation(HWND hWnd) {
    if (IsValidAnimWindow(hWnd) && IsWindowVisible(hWnd))
        StartAnimation(hWnd, FALSE, TRUE);
}

static LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if ((Msg == WM_CLOSE || (Msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE))
        && !g_shuttingDown.load()) {
        TryCloseAnimation(hWnd);
    }
    return SendMessageW_Orig(hWnd, Msg, wParam, lParam);
}

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_minMode.load() != MODE_OFF && !g_shuttingDown.load()) {
        if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
            SetDwmTransitions(hWnd, FALSE);
            StartAnimation(hWnd, FALSE, FALSE);
            return ShowWindow_Orig(hWnd, nCmdShow);
        } else if ((nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) && IsIconic(hWnd)) {
            SetDwmTransitions(hWnd, FALSE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            BOOL r = ShowWindow_Orig(hWnd, nCmdShow);
            StartAnimation(hWnd, TRUE, FALSE);
            return r;
        }
    }
    return ShowWindow_Orig(hWnd, nCmdShow);
}

static LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_SnapshotCache.find(hWnd);
        if (it != g_SnapshotCache.end()) {
            DeleteObject(it->second); g_SnapshotCache.erase(it);
            auto ord = std::find(g_SnapshotOrder.begin(), g_SnapshotOrder.end(), hWnd);
            if (ord != g_SnapshotOrder.end()) g_SnapshotOrder.erase(ord);
        }
        g_IconPositionsX.erase(hWnd);
        g_IconPositionsY.erase(hWnd);
    }
    if (g_shuttingDown.load()) return DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);

    if (Msg == WM_CLOSE && g_closeMode.load() != MODE_OFF) {
        TryCloseAnimation(hWnd);
        return DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);
    }
    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE && g_minMode.load() != MODE_OFF) {
            SetDwmTransitions(hWnd, FALSE);
            StartAnimation(hWnd, FALSE, FALSE);
            return DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);
        } else if (cmd == SC_RESTORE && IsIconic(hWnd) && g_minMode.load() != MODE_OFF) {
            SetDwmTransitions(hWnd, FALSE);
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            LRESULT r = DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);
            StartAnimation(hWnd, TRUE, FALSE);
            return r;
        } else if (cmd == SC_CLOSE && g_closeMode.load() != MODE_OFF) {
            TryCloseAnimation(hWnd);
            return DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);
        }
    }
    return DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);
}

// =============================================================================
//   WINDHAWK ENTRY POINTS
// =============================================================================

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Orig);
    Wh_SetFunctionHook((void*)SendMessageW,   (void*)SendMessageW_Hook,   (void**)&SendMessageW_Orig);
    Wh_SetFunctionHook((void*)ShowWindow,     (void*)ShowWindow_Hook,     (void**)&ShowWindow_Orig);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    g_shuttingDown.store(true);
    int spins = 500;
    while (g_activeAnims.load(std::memory_order_relaxed) > 0 && spins-- > 0) Sleep(10);

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) DeleteObject(pair.second);
    g_SnapshotCache.clear();
    g_SnapshotOrder.clear();
    g_IconPositionsX.clear();
    g_IconPositionsY.clear();
    g_ExeIconCache.clear();
    g_animatingWindows.clear();
}
