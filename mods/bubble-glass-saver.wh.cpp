// ==WindhawkMod==
// @id              bubble-glass-saver
// @name            BubbleGlass Screensaver
// @description     Transparent soap-bubble screensaver after inactivity. GDI+ per-pixel alpha: desktop stays live beneath. Instant dismiss on mouse move or keystroke.
// @version         1.7.6
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -lshell32 -lwtsapi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- idleTimeoutSec: 120
  $name: General - Inactivity timeout (seconds)
  $description: Seconds of no input before BubbleGlass appears.

- bubbleCount: 42
  $name: General - Target bubble count
  $description: Maximum number of bubbles on screen. New bubbles arrive gradually.

- multiMonitor: false
  $name: General - Span all monitors

- dismissDeltaPx: 6
  $name: General - Mouse dismiss threshold (px)
  $description: Mouse travel beyond this distance closes BubbleGlass.

- minBubbleSize: 60
  $name: Bubble Size - Minimum diameter (px)
  $description: Used when Size variation is enabled.

- maxBubbleSize: 620
  $name: Bubble Size - Maximum diameter (px)

- sizeVariation: true
  $name: Bubble Size - Vary bubble sizes
  $description: On = many small/medium and fewer very large bubbles. Off = all bubbles use Max diameter.

- driftSpeedPercent: 100
  $name: Motion - Speed (%)
  $description: 100 = normal speed, 200 = double speed.

- spawnIntervalMs: 650
  $name: Motion - Spawn interval (ms)
  $description: Delay between new bubbles while filling or replacing popped bubbles.

- turbulence: 15
  $name: Motion - Turbulence / air current (0-100)
  $description: 0 = pure billiard motion. Higher values add a gentle changing air current.

- bubbleCollisions: true
  $name: Physics - Bubble-to-bubble collisions

- wallCollisions: true
  $name: Physics - Bounce off screen edges
  $description: Off = bubbles wrap to the opposite edge instead of bouncing.

- minLifetimeSec: 60
  $name: Lifetime - Minimum lifetime (seconds)

- maxLifetimeSec: 240
  $name: Lifetime - Maximum lifetime (seconds)
  $description: Larger bubbles tend to live longer inside this range.

- popEnabled: true
  $name: Pop - Animate bubbles when lifetime ends

- popDurationMs: 220
  $name: Pop - Animation duration (ms)

- popGrowthPercent: 122
  $name: Pop - Maximum size while popping (%)
  $description: 122 = bubble expands to about 122 percent before disappearing.

- bubbleOpacity: 80
  $name: Appearance - Bubble opacity (%)

- iridescence: true
  $name: Appearance - Thin-film iridescence

- colorCycleSpeedPercent: 100
  $name: Appearance - Iridescent color cycle speed (%)
  $description: 0 freezes the interference colors. 100 = normal.

- showShadows: false
  $name: Appearance - Subtle bubble shadows

- dimPercent: 8
  $name: Appearance - Desktop dim (%)
  $description: 0 = no dimming. The live desktop/video remains visible underneath.

- spriteCache: true
  $name: Performance - Cache bubble sprites
  $description: >-
    Renders each bubble into a private sprite only a few times per second and
    blits it every frame. This is the largest speed-up. Turn off to draw every
    bubble live with GDI+ on every frame.

- spriteRefreshMs: 100
  $name: Performance - Sprite refresh interval (ms)
  $description: >-
    How often a cached bubble sprite is re-rendered (iridescent colours,
    wobble). Lower = smoother colour animation but more GDI+ work.
    Refreshing is time-budgeted per frame to limit stutter, but occasional
    long renders can still exceed the budget.

- dirtyRects: true
  $name: Performance - Dirty rectangles
  $description: >-
    Only clear and present the screen areas that changed. Turn off to clear and
    present the whole surface on every frame for comparison.

- showPerformanceStats: false
  $name: Diagnostics - Show internal performance stats
  $description: >-
    Shows BubbleGlass' own measured FPS, frame interval, render/present time,
    jitter and bubble count. Useful for performance diagnostics and frame-time analysis.

- logPerformanceStats: false
  $name: Diagnostics - Log performance stats to Windhawk/DbgView
  $description: Writes one performance line per second while the saver is active.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# BubbleGlass Screensaver

A transparent, animated soap-bubble screensaver for the Windows desktop.

BubbleGlass shows colorful, iridescent soap bubbles directly above the live
Windows desktop after a period of inactivity. The desktop stays visible and
live underneath (per-pixel alpha layered window), and any keyboard or mouse
activity dismisses the saver instantly. Rendering targets smooth 60 FPS even
on large multi-monitor desktops.

## Preview

![BubbleGlass Screensaver preview](https://i.imgur.com/Nh5EyFe.png)

![BubbleGlass Screensaver animated preview](https://i.imgur.com/gCsmdj6.gif)

*Soap bubbles drifting over the live Windows desktop. The optional diagnostics
overlay shows real-time FPS, frame time, jitter and rendering performance.*

## Features

- Transparent overlay above the live desktop - wallpaper, windows and video
  remain visible underneath.
- Smooth 60 FPS animation with QPC-based frame pacing and motion timing.
- Thin-film iridescent bubbles: rainbow interference arcs, membrane rim,
  specular highlight, hotspot and a faint opposite-side reflection.
- Organic wobble, slowly changing interference colors, optional subtle shadow.
- Gradual spawning, size variation, natural drift and optional turbulence.
- Bubble-to-bubble collisions (lightly damped) plus wall bouncing or edge
  wrapping.
- Configurable lifetime with an animated pop effect (expansion, flash,
  expanding ring).
- Primary-monitor or full multi-monitor mode.
- Instant dismissal on keystroke, click, mouse wheel or mouse movement (with a
  small movement threshold). Input is never swallowed.
- Fullscreen video playback (including fullscreen browser video) is intentionally
  supported: bubbles remain above the live, moving video.
- Windows presentation mode is suppressed when reported by Windows. Other
  fullscreen states are not automatically blocked to preserve video playback.
- Hidden automatically while the session is locked, disconnected or the
  display is off.
- Optional on-screen performance diagnostics and Windhawk/DbgView logging.
- No permanent clock or date elements.

## How It Works

BubbleGlass watches keyboard/mouse idle time. After the configured timeout it
creates a click-through, topmost layered window and composites
premultiplied-per-pixel-alpha bubbles over the desktop with
`UpdateLayeredWindowIndirect`. Bubbles are drawn with GDI+ into per-bubble
sprites that are cached and refreshed on a small time budget; only changed
areas are cleared and presented (dirty rectangles). Low-level input hooks run
on a dedicated thread and only while the saver is visible.

## Performance

The renderer targets 60 FPS; actual speed depends on resolution, bubble count
and bubble sizes. The largest tuning levers are bubble count, maximum bubble
size, sprite refresh interval, iridescence and total desktop area.

Enable **Show internal performance stats** for the FPS / frame-time / jitter
overlay, and **Log performance stats to Windhawk/DbgView** for one summary
line per second. Every option (timeout, bubble count and size, motion,
physics, lifetime, pop, appearance, performance, diagnostics) is described in
the settings UI.

## Usage

1. Install and enable the BubbleGlass Screensaver mod.
2. Configure the inactivity timeout and appearance.
3. Leave the computer idle; BubbleGlass appears automatically.
4. Move the mouse or press a key to dismiss it.

No separate `.scr` screensaver file is required.

## Notes

- BubbleGlass intentionally keeps the Windows desktop visible underneath; it
  is not a full-screen scene replacement like classic screensavers.
- The dismissing keystroke or click is also delivered to the focused
  application (input is never swallowed).
- Fullscreen video is intentionally allowed. Automatic suppression of every
  fullscreen app would also block this core feature. Windows presentation mode
  is suppressed when detectable; gamepad-only input may not reset Windows idle
  time, so fullscreen games are not guaranteed to suppress the saver.
- BubbleGlass never appears while the session is locked or display is off.
- Bubble sizes and the diagnostics overlay use raw pixels (no DPI scaling).
- "Span all monitors" uses the virtual-screen bounding rectangle; on
  mixed-size or offset monitor layouts parts of that rectangle may not be on
  any display.

## Compatibility

Designed for Windows 11 and Windhawk. Runs as a dedicated Windhawk tool-mod
process - no `.scr` screensaver file and no winlogon injection. Rendering
uses standard GDI/GDI+ and layered-window APIs (no DirectX or GPU API).

## Credits

Developed and tested by **SilverAmd**, with iterative performance analysis,
profiling and optimization focused on smooth frame pacing, large desktop
resolutions, multi-monitor support and low rendering overhead.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <wtsapi32.h>
#include <cstring>
#include <cstdint>
#include <gdiplus.h>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <cwchar>
#include <algorithm>
#include <memory>

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

using namespace Gdiplus;

constexpr DWORD FADE_IN_MS = 400;
constexpr float COLLISION_RESTITUTION = 0.985f;
constexpr UINT WM_BUBBLE_SETTINGS_CHANGED = WM_APP + 101;
constexpr UINT WM_BUBBLE_DISMISS = WM_APP + 102;
constexpr UINT WM_BUBBLE_INPUT_ARM = WM_APP + 103;
constexpr UINT WM_BUBBLE_INPUT_DISARM = WM_APP + 104;

// Reason codes carried in lParam of WM_BUBBLE_DISMISS.
constexpr LPARAM DISMISS_KEYSTROKE = 1;
constexpr LPARAM DISMISS_CLICK = 2;
constexpr LPARAM DISMISS_MOUSE_MOVE = 3;
constexpr LPARAM DISMISS_DISPLAY_CHANGE = 4;

// Maximum time per frame spent re-rendering stale (non-new) bubble sprites.
constexpr double SPRITE_REFRESH_BUDGET_MS = 3.0;

// Position and size of the cached diagnostics overlay.
constexpr int HUD_X = 28;
constexpr int HUD_Y = 32;
constexpr int HUD_W = 920;
constexpr int HUD_H = 170;

struct Settings {
	// General
	int idleTimeoutSec;
	int bubbleCount;
	BOOL multiMonitor;
	int dismissDeltaPx;

	// Bubble size
	int minBubbleSize;
	int maxBubbleSize;
	BOOL sizeVariation;

	// Motion / physics
	int driftSpeedPercent;
	int spawnIntervalMs;
	int turbulence; // 0..100
	BOOL bubbleCollisions;
	BOOL wallCollisions;

	// Lifetime / pop
	int minLifetimeSec;
	int maxLifetimeSec;
	BOOL popEnabled;
	int popDurationMs;
	int popGrowthPercent;

	// Appearance
	int bubbleOpacity;
	BOOL iridescence;
	int colorCycleSpeedPercent;
	BOOL showShadows;
	int dimPercent;

	// Performance
	BOOL spriteCache;
	int spriteRefreshMs;
	BOOL dirtyRects;

	// Diagnostics
	BOOL showPerformanceStats;
	BOOL logPerformanceStats;

	float Drift() const { return driftSpeedPercent / 100.0f; }

	float ColorCycle() const { return colorCycleSpeedPercent / 100.0f; }

	float PopDurationSec() const { return popDurationMs / 1000.0f; }

	float PopGrowth() const { return (popGrowthPercent - 100) / 100.0f; }
} g_settings;

// A pre-rendered bubble (or the diagnostics overlay): premultiplied ARGB.
struct BubbleSprite {
	int w = 0, h = 0;
	int cx = 0, cy = 0; // bubble centre inside the sprite
	std::vector<DWORD> px;
	double renderSec = -1.0e9; // animation time of the last render
	unsigned generation = 0;   // settings generation it was drawn with
};

struct Bubble {
	float x, y, r;
	float vy, vx;
	float wobAmp, wobFreq, phase, hue, alpha;

	// Long life followed by a short visible pop animation.
	float life;
	bool popping;
	float popProgress;

	// Cached rendering of this bubble (not used while popping).
	std::shared_ptr<BubbleSprite> sprite;
};

// ---- globals ----
// These hook variables and callbacks are used only on the input thread.
static HHOOK g_mouseHook = nullptr;
static HHOOK g_kbdHook = nullptr;
static HANDLE g_inputThread = nullptr;
static DWORD g_inputThreadId = 0;
static HANDLE g_inputReadyEvent = nullptr;
static HWND g_inputWindow = nullptr;
static bool g_hookArmed = false;
static bool g_hookDismissPosted = false;
static POINT g_hookAtShow{};
static int g_hookThreshold = 6;
static UINT_PTR g_hookSerial = 0;
static ULONGLONG g_hookGraceUntil = 0;

// Only the UI thread reads/writes the current display generation.
static UINT_PTR g_showSerial = 0;
static HWND g_saverWnd = nullptr;
// The notification window and its state are owned by BubbleUiThread.
static HWND g_controlWnd = nullptr;
static bool g_controlClassRegistered = false;
static bool g_sessionNotificationsRegistered = false;
static HPOWERNOTIFY g_displayPowerNotification = nullptr;
static bool g_sessionLocked = false;
static bool g_sessionDisconnected = false;
static bool g_displayOff = false;
static ULONGLONG g_resumeGraceUntil = 0;

constexpr GUID kConsoleDisplayState = {
    0x6fe69556,
    0x704a,
    0x47a0,
    {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};

bool IsDesktopUnavailable() {
	return g_sessionLocked || g_sessionDisconnected || g_displayOff;
}
static bool g_saverOn = false;
static bool g_rendering = false;
static bool g_classRegistered = false;
static std::vector<Bubble> g_bubbles;
static std::mt19937 g_rng;
static ULONG_PTR g_gdiplusToken = 0;
static UINT_PTR g_idleTimer = 0;
static int g_vx = 0, g_vy = 0, g_vw = 0, g_vh = 0;
static ULONGLONG g_showTick = 0;
static float g_fade = 0.0f;
static HDC g_memDC = nullptr;
static HBITMAP g_hBmp = nullptr;
static HBITMAP g_oldBmp = nullptr;
static void* g_bitmapBits = nullptr;
static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static HANDLE g_uiReadyEvent = nullptr;
// Published before signaling g_uiReadyEvent; checked by WhTool_ModInit.
static bool g_uiStartupOk = false;
// QPC-based simulation timing, separate from the frame pacer.
static LONGLONG g_lastFrameQpc = 0;
static LONGLONG g_showQpc = 0;
static ULONGLONG g_lastBubbleSpawnTick = 0;

// exact 60 FPS pacing.
static LARGE_INTEGER g_qpcFrequency{};
static LONGLONG g_nextFrameQpc = 0;
static LONGLONG g_frameIntervalQpc = 0;

static HANDLE g_frameTimer = nullptr;

// ---- rendering state (UI thread only) ----
// Persistent GDI+ objects that wrap the back-buffer DIB.
static std::unique_ptr<Bitmap> g_surfaceBmp;
static std::unique_ptr<Graphics> g_surfaceGfx;

// Areas touched in the previous / current frame (for dirty-rect clearing).
static std::vector<RECT> g_prevRects;
static std::vector<RECT> g_curRects;
static bool g_fullClearPending = true;
static int g_lastDimA = -1;

// Bumped whenever settings change so cached sprites get re-rendered.
static unsigned g_spriteGeneration = 1;
static std::vector<size_t> g_refreshQueue;

// Cached diagnostics overlay.
static BubbleSprite g_hudSprite;
static bool g_hudDirty = true;

// ---- lightweight performance diagnostics ----
struct PerfBreakdown {
	double physicsMs = 0.0;
	double spriteMs = 0.0;
	double clearMs = 0.0;
	double bubbleDrawMs = 0.0;
	double hudMs = 0.0;
	double presentMs = 0.0;
	double refreshed = 0.0;
	double dirtyPct = 100.0;
};

struct PerformanceStats {
	LARGE_INTEGER frequency{};
	LARGE_INTEGER renderStart{};
	LARGE_INTEGER lastPresent{};

	double frameMs[120]{};
	double renderMs[120]{};
	PerfBreakdown breakdown[120]{};

	int sampleCount = 0;
	int sampleIndex = 0;

	PerfBreakdown current{};
	PerfBreakdown avg{};

	ULONGLONG lastSummaryTick = 0;
	ULONGLONG lastLogTick = 0;

	double fps = 0.0;
	double avgFrameMs = 0.0;
	double avgRenderMs = 0.0;
	double avgWaitMs = 0.0;

	double jitterMs = 0.0;
	double maxFrameMs = 0.0;
	double maxRenderMs = 0.0;

	std::wstring textLine1;
	std::wstring textLine2;
	std::wstring textLine3;
	std::wstring textLine4;
} g_perf;

struct BubbleGdiResources {
	Pen shadowPen;
	Pen irisPen;
	Pen rimPen;
	Pen highlightPen;
	SolidBrush shineBrush;
	Pen lowerReflectionPen;
	Pen flashPen;

	BubbleGdiResources()
	    : shadowPen(Color(0, 0, 0, 0), 1.0f), irisPen(Color(0, 0, 0, 0), 1.0f),
	      rimPen(Color(0, 0, 0, 0), 1.0f),
	      highlightPen(Color(0, 0, 0, 0), 1.0f), shineBrush(Color(0, 0, 0, 0)),
	      lowerReflectionPen(Color(0, 0, 0, 0), 1.0f),
	      flashPen(Color(0, 0, 0, 0), 1.0f) {

		shadowPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);

		irisPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);

		highlightPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);

		lowerReflectionPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
	}
};

static std::unique_ptr<BubbleGdiResources> g_bubbleGdi;

// ---- forward declarations ----
void LoadSettings();
bool RegisterSaverClass();
void UnregisterSaverClass();
void CreateBackBuffer(int w, int h);
void DestroyBackBuffer();
void SeedBubbles(int width, int height);
Bubble SpawnBubble(int W, int H);
void StartRenderLoop();
void StopRenderLoop();
void RenderFrame();
void DrawBubbleAt(Graphics& gfx, const Bubble& b, float cx, float cy,
                  float alphaMul, double animationSeconds);
void ResetPerformanceStats();
void BeginPerformanceFrame();
void EndPerformanceFrame();

Color HslToColor(float h, float s, float l, BYTE a);
void ShowSaver();
void HideSaver(const char* reason);
void ScheduleIdleCheck();
bool IsIdle(DWORD* outIdleMs);
LRESULT CALLBACK SaverWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                              LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KbdProc(int nCode, WPARAM wParam, LPARAM lParam);
VOID CALLBACK IdleTimer(HWND, UINT, UINT_PTR, DWORD);
DWORD WINAPI BubbleUiThread(LPVOID);

static inline int MinI(int a, int b) { return a < b ? a : b; }
static inline int MaxI(int a, int b) { return a > b ? a : b; }

// ================= settings =================
int ClampSetting(int value, int minimum, int maximum) {
	if (value < minimum)
		return minimum;
	if (value > maximum)
		return maximum;
	return value;
}

// Reads and clamps every setting. Defaults come from the
// WindhawkModSettings block above.
void LoadSettings() {
	// General
	g_settings.idleTimeoutSec =
	    ClampSetting(Wh_GetIntSetting(L"idleTimeoutSec"), 1, 1800);

	g_settings.bubbleCount =
	    ClampSetting(Wh_GetIntSetting(L"bubbleCount"), 0, 120);

	g_settings.multiMonitor = Wh_GetIntSetting(L"multiMonitor") ? TRUE : FALSE;

	g_settings.dismissDeltaPx =
	    ClampSetting(Wh_GetIntSetting(L"dismissDeltaPx"), 1, 50);

	// Bubble size
	g_settings.minBubbleSize =
	    ClampSetting(Wh_GetIntSetting(L"minBubbleSize"), 20, 1600);

	g_settings.maxBubbleSize =
	    ClampSetting(Wh_GetIntSetting(L"maxBubbleSize"), 20, 1600);

	if (g_settings.minBubbleSize > g_settings.maxBubbleSize) {
		int tmp = g_settings.minBubbleSize;
		g_settings.minBubbleSize = g_settings.maxBubbleSize;
		g_settings.maxBubbleSize = tmp;
	}

	g_settings.sizeVariation =
	    Wh_GetIntSetting(L"sizeVariation") ? TRUE : FALSE;

	// Motion / physics
	g_settings.driftSpeedPercent =
	    ClampSetting(Wh_GetIntSetting(L"driftSpeedPercent"), 10, 500);

	g_settings.spawnIntervalMs =
	    ClampSetting(Wh_GetIntSetting(L"spawnIntervalMs"), 100, 10000);

	g_settings.turbulence =
	    ClampSetting(Wh_GetIntSetting(L"turbulence"), 0, 100);

	g_settings.bubbleCollisions =
	    Wh_GetIntSetting(L"bubbleCollisions") ? TRUE : FALSE;

	g_settings.wallCollisions =
	    Wh_GetIntSetting(L"wallCollisions") ? TRUE : FALSE;

	// Lifetime / pop
	g_settings.minLifetimeSec =
	    ClampSetting(Wh_GetIntSetting(L"minLifetimeSec"), 5, 7200);

	g_settings.maxLifetimeSec =
	    ClampSetting(Wh_GetIntSetting(L"maxLifetimeSec"), 5, 7200);

	if (g_settings.minLifetimeSec > g_settings.maxLifetimeSec) {
		int tmp = g_settings.minLifetimeSec;
		g_settings.minLifetimeSec = g_settings.maxLifetimeSec;
		g_settings.maxLifetimeSec = tmp;
	}

	g_settings.popEnabled = Wh_GetIntSetting(L"popEnabled") ? TRUE : FALSE;

	g_settings.popDurationMs =
	    ClampSetting(Wh_GetIntSetting(L"popDurationMs"), 50, 2000);

	g_settings.popGrowthPercent =
	    ClampSetting(Wh_GetIntSetting(L"popGrowthPercent"), 100, 200);

	// Appearance
	g_settings.bubbleOpacity =
	    ClampSetting(Wh_GetIntSetting(L"bubbleOpacity"), 5, 100);

	g_settings.iridescence = Wh_GetIntSetting(L"iridescence") ? TRUE : FALSE;

	g_settings.colorCycleSpeedPercent =
	    ClampSetting(Wh_GetIntSetting(L"colorCycleSpeedPercent"), 0, 500);

	g_settings.showShadows = Wh_GetIntSetting(L"showShadows") ? TRUE : FALSE;

	g_settings.dimPercent =
	    ClampSetting(Wh_GetIntSetting(L"dimPercent"), 0, 60);

	// Performance
	g_settings.spriteCache = Wh_GetIntSetting(L"spriteCache") ? TRUE : FALSE;

	g_settings.spriteRefreshMs =
	    ClampSetting(Wh_GetIntSetting(L"spriteRefreshMs"), 33, 1000);

	g_settings.dirtyRects = Wh_GetIntSetting(L"dirtyRects") ? TRUE : FALSE;

	// Diagnostics
	g_settings.showPerformanceStats =
	    Wh_GetIntSetting(L"showPerformanceStats") ? TRUE : FALSE;

	g_settings.logPerformanceStats =
	    Wh_GetIntSetting(L"logPerformanceStats") ? TRUE : FALSE;

	// Cached sprites were drawn with the previous appearance settings.
	++g_spriteGeneration;
	g_fullClearPending = true;
	g_hudDirty = true;
}

// The mod is compiled into a DLL loaded inside windhawk.exe. Window classes
// must be registered against that DLL's module handle. Never substitute the
// host executable's handle if resolving the DLL handle fails.
static HINSTANCE GetModInstance() {
	HINSTANCE instance = nullptr;
	if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
	                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
	                        reinterpret_cast<LPCWSTR>(&GetModInstance),
	                        &instance)) {
		Wh_Log(L"GetModuleHandleExW(mod instance) failed (%lu)", GetLastError());
		return nullptr;
	}
	return instance;
}

// ================= input hooks =================
// This dedicated thread pumps low-level keyboard/mouse hooks without ever
// rendering frames. Hooks exist only while the saver is visible. The callbacks
// never block Windows input or call the UI thread synchronously.
void QueueDismiss(int reason) {
	if (!g_hookArmed || g_hookDismissPosted || !g_uiThreadId)
		return;
	if (PostThreadMessageW(g_uiThreadId, WM_BUBBLE_DISMISS,
	                       static_cast<WPARAM>(g_hookSerial),
	                       static_cast<LPARAM>(reason))) {
		g_hookDismissPosted = true;
	}
}

LRESULT CALLBACK KbdProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && g_hookArmed &&
	    (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
		const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
		if (kb && kb->vkCode == VK_SNAPSHOT) {
			// Preserve the ability to take a Snipping Tool screenshot.
			g_hookGraceUntil = GetTickCount64() + 5000;
		} else if (GetTickCount64() >= g_hookGraceUntil) {
			QueueDismiss(DISMISS_KEYSTROKE); // Key press.
		}
	}
	return CallNextHookEx(g_kbdHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && g_hookArmed &&
	    GetTickCount64() >= g_hookGraceUntil) {
		const auto* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
		if (ms) {
			if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
			    wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN ||
			    wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) {
				QueueDismiss(DISMISS_CLICK); // Click or wheel.
			} else if (wParam == WM_MOUSEMOVE) {
				LONGLONG dx = static_cast<LONGLONG>(ms->pt.x) - g_hookAtShow.x;
				LONGLONG dy = static_cast<LONGLONG>(ms->pt.y) - g_hookAtShow.y;
				LONGLONG threshold = g_hookThreshold;
				if (dx * dx + dy * dy >= threshold * threshold)
					QueueDismiss(DISMISS_MOUSE_MOVE); // Mouse movement.
			}
		}
	}
	return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

void DisarmInputHooks() {
	const bool wasArmed = g_hookArmed;
	g_hookArmed = false;
	if (g_mouseHook) {
		UnhookWindowsHookEx(g_mouseHook);
		g_mouseHook = nullptr;
	}
	if (g_kbdHook) {
		UnhookWindowsHookEx(g_kbdHook);
		g_kbdHook = nullptr;
	}
	g_hookDismissPosted = false;
	if (wasArmed)
		Wh_Log(L"Input hooks disarmed");
}

LRESULT CALLBACK InputWndProc(HWND hWnd, UINT message, WPARAM wParam,
                              LPARAM lParam) {
	switch (message) {
	case WM_BUBBLE_INPUT_ARM: {
		DisarmInputHooks();
		g_hookThreshold = static_cast<int>(wParam);
		g_hookSerial = static_cast<UINT_PTR>(lParam);
		g_hookGraceUntil = 0;
		if (!GetCursorPos(&g_hookAtShow)) {
			Wh_Log(L"GetCursorPos(input) failed");
			return FALSE;
		}

		g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, nullptr, 0);
		if (!g_mouseHook) {
			Wh_Log(L"Mouse hook failed (%lu)", GetLastError());
			DisarmInputHooks();
			return FALSE;
		}
		g_kbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, KbdProc, nullptr, 0);
		if (!g_kbdHook) {
			Wh_Log(L"Keyboard hook failed (%lu)", GetLastError());
			DisarmInputHooks();
			return FALSE;
		}
		g_hookArmed = true;
		Wh_Log(L"Input hooks armed");
		return TRUE;
	}
	case WM_BUBBLE_INPUT_DISARM:
		DisarmInputHooks();
		return TRUE;
	case WM_CLOSE:
		DisarmInputHooks();
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

DWORD WINAPI BubbleInputThread(LPVOID) {
	HINSTANCE instance = GetModInstance();
	if (!instance)
		return 1;
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.hInstance = instance;
	wc.lpfnWndProc = InputWndProc;
	wc.lpszClassName = L"BubbleGlassInputControl";
	bool registered = RegisterClassExW(&wc) != 0;
	if (!registered) {
		Wh_Log(L"Input window class registration failed (%lu)", GetLastError());
	}

	HWND wnd = registered ? CreateWindowExW(0, L"BubbleGlassInputControl", L"",
	                                        0, 0, 0, 0, 0, HWND_MESSAGE,
	                                        nullptr, instance, nullptr)
	                      : nullptr;
	if (registered && !wnd) {
		Wh_Log(L"Input window creation failed (%lu)", GetLastError());
	}
	g_inputWindow = wnd;
	if (g_inputReadyEvent)
		SetEvent(g_inputReadyEvent);

	if (wnd) {
		Wh_Log(L"Input thread ready");
		MSG message{};
		while (GetMessageW(&message, nullptr, 0, 0) > 0) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
		DisarmInputHooks();
		if (IsWindow(wnd))
			DestroyWindow(wnd);
	}

	g_inputWindow = nullptr;
	if (registered)
		UnregisterClassW(L"BubbleGlassInputControl", instance);
	return wnd ? 0 : 1;
}

// ================= idle watchdog =================
bool IsIdle(DWORD* outIdleMs) {
	LASTINPUTINFO lii{sizeof(lii)};
	if (!GetLastInputInfo(&lii))
		return false;
	*outIdleMs = GetTickCount() - lii.dwTime;
	return true;
}

// Deliberately do not block QUNS_BUSY or QUNS_RUNNING_D3D_FULL_SCREEN:
// either can also occur during fullscreen video playback, which is a core
// BubbleGlass feature. Suppress only explicit Windows presentation mode.
static bool IsPresentationModeActive() {
	QUERY_USER_NOTIFICATION_STATE state{};
	return SUCCEEDED(SHQueryUserNotificationState(&state)) &&
	       state == QUNS_PRESENTATION_MODE;
}

// Arms the one-shot idle watchdog. The previous timer is always killed
// first, so re-arming can never accumulate timers.
void ArmIdleTimer(DWORD delayMs) {
	if (delayMs < 20)
		delayMs = 20;
	if (g_idleTimer)
		KillTimer(nullptr, g_idleTimer);
	g_idleTimer = SetTimer(nullptr, 0, delayMs, IdleTimer);
	if (!g_idleTimer)
		Wh_Log(L"Idle timer failed (%lu)", GetLastError());
}

// Arms the watchdog for the moment the saver could next appear: the
// remaining idle time until the inactivity timeout expires. While the saver
// is visible, input hooks dismiss it, so no idle polling runs at all.
void ScheduleIdleCheck() {
	if (g_saverOn) {
		if (g_idleTimer) {
			KillTimer(nullptr, g_idleTimer);
			g_idleTimer = 0;
		}
		return;
	}

	DWORD delayMs = 1000;
	const ULONGLONG now = GetTickCount64();
	if (now < g_resumeGraceUntil) {
		// Avoid per-second wakeups during the post-resume/display-change grace.
		const ULONGLONG remaining = g_resumeGraceUntil - now;
		ArmIdleTimer(static_cast<DWORD>(std::min<ULONGLONG>(remaining, 60000)));
		return;
	}
	DWORD idleMs = 0;
	if (!IsDesktopUnavailable() && IsIdle(&idleMs)) {
		const DWORD timeoutMs =
		    static_cast<DWORD>(g_settings.idleTimeoutSec) * 1000u;
		if (idleMs < timeoutMs)
			delayMs = timeoutMs - idleMs;
	}
	ArmIdleTimer(delayMs);
}

// One-shot check: fires when the inactivity timeout should have expired,
// shows the saver once, then re-arms for the next relevant moment.
VOID CALLBACK IdleTimer(HWND, UINT, UINT_PTR, DWORD) {
	if (!g_saverOn && !IsDesktopUnavailable() &&
	    GetTickCount64() >= g_resumeGraceUntil && !IsPresentationModeActive()) {
		DWORD idleMs = 0;
		if (IsIdle(&idleMs) &&
		    idleMs >= static_cast<DWORD>(g_settings.idleTimeoutSec) * 1000u) {
			ShowSaver();
		}
	}

	ScheduleIdleCheck();
}

// ================= window class =================
LRESULT CALLBACK SaverWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                              LPARAM lParam) {
	switch (uMsg) {
	case WM_NCHITTEST:
		return HTTRANSPARENT; // extra safety: never take clicks
	case WM_DISPLAYCHANGE:
		// Resolution or monitor layout changed while active: hide now and
		// let the saver reappear after the next idle period with the new
		// desktop geometry. Posted so the window is not destroyed while
		// its own window procedure is running.
		if (g_saverOn) {
			PostThreadMessageW(g_uiThreadId, WM_BUBBLE_DISMISS,
			                   static_cast<WPARAM>(g_showSerial),
			                   DISMISS_DISPLAY_CHANGE);
		}
		break;
	}
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool RegisterSaverClass() {
	if (g_classRegistered)
		return true;

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = SaverWndProc;
	wc.hInstance = GetModInstance();
	if (!wc.hInstance)
		return false;
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.lpszClassName = L"BubbleGlassSaver";

	if (!RegisterClassExW(&wc)) {
		// A stale registration from a previous instance would make
		// CreateWindowExW use that instance's dangling window procedure,
		// so a failed registration must always block window creation.
		Wh_Log(L"RegisterClassExW(saver) failed (%lu)", GetLastError());
		return false;
	}

	g_classRegistered = true;
	return true;
}

void UnregisterSaverClass() {
	if (g_classRegistered) {
		UnregisterClassW(L"BubbleGlassSaver", GetModInstance());
		g_classRegistered = false;
	}
}

// Session and power notifications arrive on the UI thread that owns the
// saver window, so state changes do not need cross-thread synchronization.
LRESULT CALLBACK ControlWndProc(HWND hWnd, UINT message, WPARAM wParam,
                                LPARAM lParam) {
	bool handled = false;
	bool previouslyUnavailable = IsDesktopUnavailable();

	if (message == WM_WTSSESSION_CHANGE) {
		switch (wParam) {
		case WTS_SESSION_LOCK:
			g_sessionLocked = true;
			handled = true;
			break;
		case WTS_SESSION_UNLOCK:
			g_sessionLocked = false;
			handled = true;
			break;
		case WTS_CONSOLE_DISCONNECT:
		case WTS_REMOTE_DISCONNECT:
			g_sessionDisconnected = true;
			handled = true;
			break;
		case WTS_CONSOLE_CONNECT:
		case WTS_REMOTE_CONNECT:
			g_sessionDisconnected = false;
			handled = true;
			break;
		}
	} else if (message == WM_POWERBROADCAST &&
	           wParam == PBT_POWERSETTINGCHANGE) {
		const auto* power =
		    reinterpret_cast<const POWERBROADCAST_SETTING*>(lParam);
		if (power && power->DataLength >= sizeof(DWORD) &&
		    IsEqualGUID(power->PowerSetting, kConsoleDisplayState)) {
			DWORD state = 1;
			memcpy(&state, power->Data, sizeof(state));
			g_displayOff = state == 0; // 0 = off, 1 = on, 2 = dimmed
			handled = true;
		}
	}

	if (handled) {
		if (IsDesktopUnavailable()) {
			HideSaver("session-locked/disconnected-or-display-off");
		} else if (previouslyUnavailable) {
			// Avoid reopening immediately after the user unlocks or wakes
			// the screen even when the last-input timestamp is still old.
			g_resumeGraceUntil = GetTickCount64() + 3000;
		}
		if (previouslyUnavailable != IsDesktopUnavailable() ||
		    message == WM_WTSSESSION_CHANGE) {
			Wh_Log(L"Visibility: locked=%d disconnected=%d displayOff=%d",
			       g_sessionLocked, g_sessionDisconnected, g_displayOff);
		}
		return message == WM_POWERBROADCAST ? TRUE : 0;
	}

	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void RegisterControlWindow() {
	HINSTANCE instance = GetModInstance();
	if (!instance)
		return;
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = ControlWndProc;
	wc.hInstance = instance;
	wc.lpszClassName = L"BubbleGlassControl";
	if (!RegisterClassExW(&wc)) {
		Wh_Log(L"RegisterClassExW(control) failed (%lu)", GetLastError());
		return;
	}
	g_controlClassRegistered = true;

	// Hidden zero-sized top-level window, following Windhawk fluid-wallpaper.
	// Unlike the saver window, it exists for the entire tool-mod lifetime.
	g_controlWnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
	                               L"BubbleGlassControl", L"", WS_POPUP, 0, 0,
	                               0, 0, nullptr, nullptr, instance, nullptr);
	if (!g_controlWnd) {
		Wh_Log(L"CreateWindowExW(control) failed (%lu)", GetLastError());
		return;
	}

	g_sessionNotificationsRegistered =
	    WTSRegisterSessionNotification(g_controlWnd, NOTIFY_FOR_THIS_SESSION) !=
	    FALSE;
	if (!g_sessionNotificationsRegistered) {
		Wh_Log(L"WTSRegisterSessionNotification failed (%lu)", GetLastError());
	}

	g_displayPowerNotification = RegisterPowerSettingNotification(
	    g_controlWnd, &kConsoleDisplayState, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (!g_displayPowerNotification) {
		Wh_Log(L"RegisterPowerSettingNotification failed (%lu)", GetLastError());
	}
}

void UnregisterControlWindow() {
	if (g_sessionNotificationsRegistered) {
		WTSUnRegisterSessionNotification(g_controlWnd);
		g_sessionNotificationsRegistered = false;
	}
	if (g_displayPowerNotification) {
		UnregisterPowerSettingNotification(g_displayPowerNotification);
		g_displayPowerNotification = nullptr;
	}
	if (g_controlWnd) {
		DestroyWindow(g_controlWnd);
		g_controlWnd = nullptr;
	}
	if (g_controlClassRegistered) {
		UnregisterClassW(L"BubbleGlassControl", GetModInstance());
		g_controlClassRegistered = false;
	}
}

// ================= back buffer =================
// 32-bit top-down DIB selected into a memory DC, presented with
// UpdateLayeredWindowIndirect(ULW_ALPHA) for true per-pixel transparency.
// Persistent GDI+ Bitmap/Graphics objects wrap the DIB memory (used for the
// few bubbles that are drawn live, i.e. popping ones or when the sprite cache
// is switched off).
void CreateBackBuffer(int w, int h) {
	DestroyBackBuffer();

	HDC hdcScreen = GetDC(nullptr);
	if (!hdcScreen)
		return;

	g_memDC = CreateCompatibleDC(hdcScreen);
	if (!g_memDC) {
		ReleaseDC(nullptr, hdcScreen);
		return;
	}

	BITMAPINFO bmi{};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = -h;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	g_bitmapBits = nullptr;

	g_hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &g_bitmapBits,
	                          nullptr, 0);

	if (g_hBmp && g_bitmapBits) {
		g_oldBmp = static_cast<HBITMAP>(SelectObject(g_memDC, g_hBmp));

		g_surfaceBmp.reset(new Bitmap(w, h, w * 4, PixelFormat32bppPARGB,
		                              static_cast<BYTE*>(g_bitmapBits)));
		if (g_surfaceBmp->GetLastStatus() == Ok) {
			g_surfaceGfx.reset(new Graphics(g_surfaceBmp.get()));
			g_surfaceGfx->SetSmoothingMode(SmoothingModeAntiAlias);
			g_surfaceGfx->SetCompositingQuality(CompositingQualityHighSpeed);
			g_surfaceGfx->SetPixelOffsetMode(PixelOffsetModeHighSpeed);
		} else {
			g_surfaceBmp.reset();
		}
	} else {
		if (g_hBmp) {
			DeleteObject(g_hBmp);
			g_hBmp = nullptr;
		}

		DeleteDC(g_memDC);
		g_memDC = nullptr;
		g_bitmapBits = nullptr;
	}

	ReleaseDC(nullptr, hdcScreen);

	// A fresh DIB: nothing is known about its contents.
	g_prevRects.clear();
	g_curRects.clear();
	g_fullClearPending = true;
	g_lastDimA = -1;
}

void DestroyBackBuffer() {
	// GDI+ objects must go before the memory they wrap.
	g_surfaceGfx.reset();
	g_surfaceBmp.reset();

	if (g_memDC) {
		if (g_oldBmp) {
			SelectObject(g_memDC, g_oldBmp);
			g_oldBmp = nullptr;
		}

		if (g_hBmp) {
			DeleteObject(g_hBmp);
			g_hBmp = nullptr;
		}

		DeleteDC(g_memDC);
		g_memDC = nullptr;
	}

	g_bitmapBits = nullptr;
}

// The pixel the surface is cleared to (desktop dim), premultiplied.
static DWORD MakeDimPixel(BYTE a) {
	auto Premul = [a](BYTE c) -> BYTE {
		return static_cast<BYTE>(
		    (static_cast<unsigned>(c) * static_cast<unsigned>(a) + 127u) /
		    255u);
	};

	const BYTE pr = Premul(4);
	const BYTE pg = Premul(6);
	const BYTE pb = Premul(15);

	return (static_cast<DWORD>(a) << 24) | (static_cast<DWORD>(pr) << 16) |
	       (static_cast<DWORD>(pg) << 8) | static_cast<DWORD>(pb);
}

// Fast direct clear of the complete 32-bit PARGB DIB.
static void ClearDibAll(DWORD pixel) {
	if (!g_bitmapBits || g_vw <= 0 || g_vh <= 0)
		return;

	DWORD* dst = static_cast<DWORD*>(g_bitmapBits);

	const size_t pixelCount =
	    static_cast<size_t>(g_vw) * static_cast<size_t>(g_vh);

	std::fill_n(dst, pixelCount, pixel);
}

// Clears only one rectangle (already clipped to the surface by the caller).
static void ClearDibRect(const RECT& rc, DWORD pixel) {
	if (!g_bitmapBits)
		return;

	const int x0 = MaxI(static_cast<int>(rc.left), 0);
	const int y0 = MaxI(static_cast<int>(rc.top), 0);
	const int x1 = MinI(static_cast<int>(rc.right), g_vw);
	const int y1 = MinI(static_cast<int>(rc.bottom), g_vh);
	if (x0 >= x1 || y0 >= y1)
		return;

	DWORD* base = static_cast<DWORD*>(g_bitmapBits);
	for (int y = y0; y < y1; ++y) {
		DWORD* row = base + static_cast<size_t>(y) * g_vw;
		std::fill(row + x0, row + x1, pixel);
	}
}

// ---- premultiplied source-over blitter ----
// Scales all four premultiplied channels by ca/256 (ca in 0..256).
static inline DWORD ScalePremul(DWORD s, uint32_t ca) {
	const uint32_t rb = (((s & 0x00FF00FFu) * ca) >> 8) & 0x00FF00FFu;
	const uint32_t ag = (((s >> 8) & 0x00FF00FFu) * ca) & 0xFF00FF00u;
	return rb | ag;
}

// dst = src + dst * (1 - src.alpha); both premultiplied.
static inline DWORD BlendPremul(DWORD s, DWORD d) {
	const uint32_t inv = 256u - (s >> 24);
	const uint32_t rb = (((d & 0x00FF00FFu) * inv) >> 8) & 0x00FF00FFu;
	const uint32_t ag = (((d >> 8) & 0x00FF00FFu) * inv) & 0xFF00FF00u;
	return s + (rb | ag);
}

// Blits a sprite with constant opacity ca (0..256) at (dx, dy).
// Fully transparent pixels are skipped four at a time.
// Returns the clipped destination rectangle that may have been modified.
static bool BlitSprite(const BubbleSprite& sp, int dx, int dy, uint32_t ca,
                       RECT* out) {
	if (ca == 0 || sp.w <= 0 || sp.h <= 0 || !g_bitmapBits)
		return false;
	if (ca > 256u)
		ca = 256u;

	const int x0 = MaxI(dx, 0);
	const int y0 = MaxI(dy, 0);
	const int x1 = MinI(dx + sp.w, g_vw);
	const int y1 = MinI(dy + sp.h, g_vh);
	if (x0 >= x1 || y0 >= y1)
		return false;

	DWORD* dstBase = static_cast<DWORD*>(g_bitmapBits);
	const int n = x1 - x0;

	for (int y = y0; y < y1; ++y) {
		const DWORD* src =
		    sp.px.data() + static_cast<size_t>(y - dy) * sp.w + (x0 - dx);
		DWORD* dst = dstBase + static_cast<size_t>(y) * g_vw + x0;

		int i = 0;
		for (; i + 4 <= n; i += 4) {
			uint64_t a, b;
			memcpy(&a, src + i, 8);
			memcpy(&b, src + i + 2, 8);
			if ((a | b) == 0)
				continue;

			for (int k = 0; k < 4; ++k) {
				DWORD s = src[i + k];
				if (!s)
					continue;
				if (ca != 256u)
					s = ScalePremul(s, ca);
				dst[i + k] = BlendPremul(s, dst[i + k]);
			}
		}
		for (; i < n; ++i) {
			DWORD s = src[i];
			if (!s)
				continue;
			if (ca != 256u)
				s = ScalePremul(s, ca);
			dst[i] = BlendPremul(s, dst[i]);
		}
	}

	out->left = x0;
	out->top = y0;
	out->right = x1;
	out->bottom = y1;
	return true;
}

// ================= bubbles =================
Bubble SpawnBubble(int W, int H) {
	std::uniform_real_distribution<float> u01(0.0f, 1.0f);
	std::uniform_real_distribution<float> uHue(0.0f, 360.0f);
	std::uniform_real_distribution<float> uSpeed(0.75f, 1.55f);
	std::uniform_real_distribution<float> uWob(0.4f, 1.8f);
	std::uniform_real_distribution<float> uFrq(0.5f, 2.1f);
	std::uniform_real_distribution<float> uPh(0.0f, 6.2832f);
	std::uniform_real_distribution<float> uAl(0.65f, 1.0f);

	const float safeW = W > 1 ? static_cast<float>(W) : 1.0f;
	const float safeH = H > 1 ? static_cast<float>(H) : 1.0f;

	float minR = g_settings.minBubbleSize / 2.0f;
	float maxR = g_settings.maxBubbleSize / 2.0f;

	if (minR < 10.0f)
		minR = 10.0f;
	if (maxR < minR)
		maxR = minR;

	// Never create a circle so large that its centre cannot fit on screen.
	float screenMaxR = (safeW < safeH ? safeW : safeH) * 0.48f;
	if (screenMaxR < 10.0f)
		screenMaxR = 10.0f;
	if (maxR > screenMaxR)
		maxR = screenMaxR;
	if (minR > maxR)
		minR = maxR;

	float r = maxR;

	if (g_settings.sizeVariation && maxR > minR) {
		// t^2 deliberately creates many small/medium bubbles and fewer huge ones.
		float t = u01(g_rng);
		r = minR + t * t * (maxR - minR);
	}

	Bubble b{};
	b.r = r;

	// Classic Windows-Bubbles feel: new bubbles arrive from lower-left.
	float spawnXJitter = safeW * 0.025f;
	if (spawnXJitter > 110.0f)
		spawnXJitter = 110.0f;

	float spawnYJitter = safeH * 0.12f;
	if (spawnYJitter > 160.0f)
		spawnYJitter = 160.0f;

	b.x = r + 4.0f + u01(g_rng) * spawnXJitter;
	b.y = safeH - r - 4.0f - u01(g_rng) * spawnYJitter;

	// With "span all monitors" the virtual bounding rectangle can
	// include areas that are not on any display. Keep the spawn point
	// on a real monitor so a bubble is never born off-screen.
	POINT spawnScreen{g_vx + static_cast<LONG>(b.x + 0.5f),
	                  g_vy + static_cast<LONG>(b.y + 0.5f)};
	if (!MonitorFromPoint(spawnScreen, MONITOR_DEFAULTTONULL)) {
		MONITORINFO mi{sizeof(mi)};
		HMONITOR mon = MonitorFromPoint(spawnScreen, MONITOR_DEFAULTTONEAREST);
		if (mon && GetMonitorInfoW(mon, &mi)) {
			b.x = static_cast<float>(mi.rcMonitor.left - g_vx) + r + 4.0f;
			b.y = static_cast<float>(mi.rcMonitor.bottom - g_vy) - r - 4.0f;
			if (b.x < r + 4.0f)
				b.x = r + 4.0f;
			if (b.x > safeW - r - 4.0f)
				b.x = safeW - r - 4.0f;
			if (b.y < r + 4.0f)
				b.y = r + 4.0f;
			if (b.y > safeH - r - 4.0f)
				b.y = safeH - r - 4.0f;
		}
	}

	// Mostly up/right. Screen Y grows downward, hence negative vy.
	float angleDeg = 28.0f + u01(g_rng) * 42.0f;
	float angle = angleDeg * 3.14159265f / 180.0f;
	float speed = uSpeed(g_rng);

	b.vx = cosf(angle) * speed;
	b.vy = -sinf(angle) * speed;

	b.wobAmp = uWob(g_rng);
	b.wobFreq = uFrq(g_rng);
	b.phase = uPh(g_rng);
	b.hue = uHue(g_rng);
	b.alpha = uAl(g_rng);

	// Size-biased lifetime inside the user-configured range.
	// With 60..240 seconds this yields roughly:
	// small ≈ 60..120, medium ≈ 90..180, large ≈ 180..240.
	float sizeRatio = 1.0f;
	if (maxR > minR)
		sizeRatio = (r - minR) / (maxR - minR);

	if (sizeRatio < 0.0f)
		sizeRatio = 0.0f;
	if (sizeRatio > 1.0f)
		sizeRatio = 1.0f;

	float minLife = static_cast<float>(g_settings.minLifetimeSec);
	float maxLife = static_cast<float>(g_settings.maxLifetimeSec);
	float lifeSpan = maxLife - minLife;

	b.life =
	    minLife + sizeRatio * lifeSpan * 0.67f + u01(g_rng) * lifeSpan * 0.33f;

	if (b.life > maxLife)
		b.life = maxLife;

	b.popping = false;
	b.popProgress = 0.0f;

	return b;
}

void SeedBubbles(int width, int height) {
	g_bubbles.clear();
	g_bubbles.reserve(static_cast<size_t>(
	    g_settings.bubbleCount > 0 ? g_settings.bubbleCount : 0));

	// Start with one bubble. The desktop fills progressively from lower-left.
	if (g_settings.bubbleCount > 0)
		g_bubbles.push_back(SpawnBubble(width, height));

	g_lastBubbleSpawnTick = GetTickCount64();
}

// ================= render loop =================
void StartRenderLoop() {
	if (!g_saverWnd || g_rendering)
		return;

	QueryPerformanceFrequency(&g_qpcFrequency);

	LARGE_INTEGER nowQpc{};
	QueryPerformanceCounter(&nowQpc);

	// Exact 60 FPS target: about 16.666667 ms total per frame.
	g_frameIntervalQpc = (g_qpcFrequency.QuadPart + 30) / 60;

	g_lastFrameQpc = 0; // First frame uses nominal 1/60 second.
	g_showQpc = nowQpc.QuadPart;

	// ShowSaver() renders the first frame immediately.
	g_nextFrameQpc = nowQpc.QuadPart + g_frameIntervalQpc;

	g_rendering = true;
}

void StopRenderLoop() {
	g_rendering = false;
	g_lastFrameQpc = 0;
	g_showQpc = 0;
	g_nextFrameQpc = 0;

	if (g_frameTimer)
		CancelWaitableTimer(g_frameTimer);
}

Color HslToColor(float h, float s, float l, BYTE a) {
	h = fmodf(h, 360.0f);
	if (h < 0.0f)
		h += 360.0f;
	h /= 360.0f;
	float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
	float p = 2.0f * l - q;
	auto hue2rgb = [&](float t) -> float {
		if (t < 0.0f)
			t += 1.0f;
		if (t > 1.0f)
			t -= 1.0f;
		if (t < 1.0f / 6.0f)
			return p + (q - p) * 6.0f * t;
		if (t < 0.5f)
			return q;
		if (t < 2.0f / 3.0f)
			return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
		return p;
	};
	BYTE r = (BYTE)(hue2rgb(h + 1.0f / 3.0f) * 255.0f);
	BYTE g = (BYTE)(hue2rgb(h) * 255.0f);
	BYTE b = (BYTE)(hue2rgb(h - 1.0f / 3.0f) * 255.0f);
	return Color(a, r, g, b);
}

double PerfDeltaMs(const LARGE_INTEGER& begin, const LARGE_INTEGER& end) {
	if (g_perf.frequency.QuadPart <= 0)
		return 0.0;

	return static_cast<double>(end.QuadPart - begin.QuadPart) * 1000.0 /
	       static_cast<double>(g_perf.frequency.QuadPart);
}

void ResetPerformanceStats() {
	g_perf = PerformanceStats{};

	QueryPerformanceFrequency(&g_perf.frequency);

	g_perf.textLine1 = L"FPS: collecting...";
	g_perf.textLine2 = L"Work / Wait: collecting...";
	g_perf.textLine3 = L"Physics / Blit / Present: collecting...";
	g_perf.textLine4 = L"Clear / Sprites / HUD: collecting...";
	g_hudDirty = true;
}

void BeginPerformanceFrame() {
	if (!g_settings.showPerformanceStats && !g_settings.logPerformanceStats) {
		return;
	}

	if (g_perf.frequency.QuadPart <= 0)
		QueryPerformanceFrequency(&g_perf.frequency);

	g_perf.current = PerfBreakdown{};

	QueryPerformanceCounter(&g_perf.renderStart);
}

void EndPerformanceFrame() {
	if (!g_settings.showPerformanceStats && !g_settings.logPerformanceStats) {
		return;
	}

	LARGE_INTEGER now{};
	QueryPerformanceCounter(&now);

	if (g_perf.frequency.QuadPart <= 0)
		return;

	const double renderMs = PerfDeltaMs(g_perf.renderStart, now);

	double frameMs = 0.0;

	if (g_perf.lastPresent.QuadPart != 0) {
		frameMs = PerfDeltaMs(g_perf.lastPresent, now);
	}

	g_perf.lastPresent = now;

	if (frameMs > 0.1 && frameMs < 2000.0) {
		const int index = g_perf.sampleIndex;

		g_perf.frameMs[index] = frameMs;
		g_perf.renderMs[index] = renderMs;
		g_perf.breakdown[index] = g_perf.current;

		g_perf.sampleIndex = (g_perf.sampleIndex + 1) % 120;

		if (g_perf.sampleCount < 120)
			++g_perf.sampleCount;
	}

	const ULONGLONG tickNow = GetTickCount64();

	if (g_perf.sampleCount <= 0 || (g_perf.lastSummaryTick != 0 &&
	                                tickNow - g_perf.lastSummaryTick < 500)) {
		return;
	}

	g_perf.lastSummaryTick = tickNow;

	double sumFrame = 0.0;
	double sumRender = 0.0;
	double maxFrame = 0.0;
	double maxRender = 0.0;

	PerfBreakdown sum{};
	sum.dirtyPct = 0.0;

	for (int i = 0; i < g_perf.sampleCount; ++i) {
		const double f = g_perf.frameMs[i];
		const double r = g_perf.renderMs[i];

		sumFrame += f;
		sumRender += r;

		if (f > maxFrame)
			maxFrame = f;

		if (r > maxRender)
			maxRender = r;

		const PerfBreakdown& p = g_perf.breakdown[i];

		sum.physicsMs += p.physicsMs;
		sum.spriteMs += p.spriteMs;
		sum.clearMs += p.clearMs;
		sum.bubbleDrawMs += p.bubbleDrawMs;
		sum.hudMs += p.hudMs;
		sum.presentMs += p.presentMs;
		sum.refreshed += p.refreshed;
		sum.dirtyPct += p.dirtyPct;
	}

	const double n = static_cast<double>(g_perf.sampleCount);

	g_perf.avgFrameMs = sumFrame / n;
	g_perf.avgRenderMs = sumRender / n;

	g_perf.avg.physicsMs = sum.physicsMs / n;
	g_perf.avg.spriteMs = sum.spriteMs / n;
	g_perf.avg.clearMs = sum.clearMs / n;
	g_perf.avg.bubbleDrawMs = sum.bubbleDrawMs / n;
	g_perf.avg.hudMs = sum.hudMs / n;
	g_perf.avg.presentMs = sum.presentMs / n;
	g_perf.avg.refreshed = sum.refreshed / n;
	g_perf.avg.dirtyPct = sum.dirtyPct / n;

	g_perf.avgWaitMs = g_perf.avgFrameMs - g_perf.avgRenderMs;

	if (g_perf.avgWaitMs < 0.0)
		g_perf.avgWaitMs = 0.0;

	double variance = 0.0;

	for (int i = 0; i < g_perf.sampleCount; ++i) {
		const double d = g_perf.frameMs[i] - g_perf.avgFrameMs;

		variance += d * d;
	}

	variance /= n;

	g_perf.jitterMs = sqrt(variance);
	g_perf.maxFrameMs = maxFrame;
	g_perf.maxRenderMs = maxRender;

	g_perf.fps = g_perf.avgFrameMs > 0.0 ? 1000.0 / g_perf.avgFrameMs : 0.0;

	wchar_t line1[256]{};
	wchar_t line2[256]{};
	wchar_t line3[256]{};
	wchar_t line4[256]{};

	std::swprintf(line1, sizeof(line1) / sizeof(line1[0]),
	              L"FPS %.1f   Frame %.2f ms   Jitter %.2f ms", g_perf.fps,
	              g_perf.avgFrameMs, g_perf.jitterMs);

	std::swprintf(line2, sizeof(line2) / sizeof(line2[0]),
	              L"Work %.2f ms   Wait %.2f ms   Bubbles %zu",
	              g_perf.avgRenderMs, g_perf.avgWaitMs, g_bubbles.size());

	std::swprintf(line3, sizeof(line3) / sizeof(line3[0]),
	              L"Physics %.2f  Blit %.2f  Present %.2f ms (%.0f%%)",
	              g_perf.avg.physicsMs, g_perf.avg.bubbleDrawMs,
	              g_perf.avg.presentMs, g_perf.avg.dirtyPct);

	std::swprintf(line4, sizeof(line4) / sizeof(line4[0]),
	              L"Clear %.2f  Sprites %.2f (%.1f/f)  HUD %.2f ms",
	              g_perf.avg.clearMs, g_perf.avg.spriteMs, g_perf.avg.refreshed,
	              g_perf.avg.hudMs);

	g_perf.textLine1 = line1;
	g_perf.textLine2 = line2;
	g_perf.textLine3 = line3;
	g_perf.textLine4 = line4;
	g_hudDirty = true;

	if (g_settings.logPerformanceStats &&
	    (g_perf.lastLogTick == 0 || tickNow - g_perf.lastLogTick >= 1000)) {

		g_perf.lastLogTick = tickNow;

		Wh_Log(L"PERF: "
		       L"FPS=%.1f frame=%.2fms work=%.2fms wait=%.2fms "
		       L"physics=%.2fms sprite=%.2fms(n=%.1f) clear=%.2fms "
		       L"draw=%.2fms hud=%.2fms present=%.2fms dirty=%.0f%% "
		       L"jitter=%.2fms maxFrame=%.2fms maxWork=%.2fms "
		       L"bubbles=%zu",

		       g_perf.fps, g_perf.avgFrameMs, g_perf.avgRenderMs,
		       g_perf.avgWaitMs, g_perf.avg.physicsMs, g_perf.avg.spriteMs,
		       g_perf.avg.refreshed, g_perf.avg.clearMs,
		       g_perf.avg.bubbleDrawMs, g_perf.avg.hudMs, g_perf.avg.presentMs,
		       g_perf.avg.dirtyPct, g_perf.jitterMs, g_perf.maxFrameMs,
		       g_perf.maxRenderMs, g_bubbles.size());
	}
}

// Renders the diagnostics overlay text into its sprite. Called only when the
// text changed (twice per second), not every frame.
static void RenderHudSprite() {
	BubbleSprite& sp = g_hudSprite;

	if (sp.w != HUD_W || sp.h != HUD_H) {
		sp.px.assign(static_cast<size_t>(HUD_W) * HUD_H, 0);
		sp.w = HUD_W;
		sp.h = HUD_H;
	} else {
		std::fill(sp.px.begin(), sp.px.end(), 0u);
	}

	Bitmap bmp(HUD_W, HUD_H, HUD_W * 4, PixelFormat32bppPARGB,
	           reinterpret_cast<BYTE*>(sp.px.data()));
	Graphics gfx(&bmp);
	gfx.SetTextRenderingHint(TextRenderingHintAntiAlias);

	FontFamily family(L"Consolas");
	Font font(&family, 30.0f, FontStyleRegular, UnitPixel);
	StringFormat format;
	format.SetAlignment(StringAlignmentNear);
	format.SetLineAlignment(StringAlignmentNear);

	SolidBrush bg(Color(145, 0, 0, 0));
	gfx.FillRectangle(&bg, RectF(0.0f, 0.0f, static_cast<REAL>(HUD_W),
	                             static_cast<REAL>(HUD_H)));

	SolidBrush textBrush(Color(235, 255, 255, 255));

	const std::wstring* lines[4] = {&g_perf.textLine1, &g_perf.textLine2,
	                                &g_perf.textLine3, &g_perf.textLine4};

	for (int i = 0; i < 4; ++i) {
		gfx.DrawString(lines[i]->c_str(), -1, &font,
		               RectF(14.0f, 10.0f + 38.0f * i,
		                     static_cast<REAL>(HUD_W - 28), 38.0f),
		               &format, &textBrush);
	}

	g_hudDirty = false;
}

// Draws one bubble centred at (cx, cy) into gfx.
// alphaMul is the per-bubble alpha (b.alpha * fade for live drawing,
// 1.0 when rendering a cached sprite; the sprite is then blitted with the
// matching constant alpha).
void DrawBubbleAt(Graphics& gfx, const Bubble& b, float cx, float cy,
                  float alphaMul, double animationSeconds) {

	if (alphaMul <= 0.01f)
		return;

	float popP = b.popping ? b.popProgress : 0.0f;
	if (popP < 0.0f)
		popP = 0.0f;
	if (popP > 1.0f)
		popP = 1.0f;

	// Pop animation: the membrane stretches outward, flashes, then vanishes.
	float popEase = 1.0f - (1.0f - popP) * (1.0f - popP);
	float popScale = 1.0f + g_settings.PopGrowth() * popEase;
	float popFade = b.popping ? (1.0f - popP) : 1.0f;
	float popFlash = b.popping ? sinf(popP * 3.14159265f) : 0.0f;

	float globalA = alphaMul * (g_settings.bubbleOpacity / 100.0f) * popFade;

	if (globalA <= 0.005f)
		return;

	auto Alpha = [globalA](float value) -> BYTE {
		float a = value * globalA;
		if (a < 0.0f)
			a = 0.0f;
		if (a > 255.0f)
			a = 255.0f;
		return static_cast<BYTE>(a + 0.5f);
	};

	const REAL r = b.r * popScale;
	const REAL x = cx;
	const REAL y = cy;

	// Tiny organic deformation. Large bubbles stay calmer than small ones.
	float sizeCalm = 1.0f / (1.0f + r * 0.0022f);
	float wobble = static_cast<float>(std::sin(
	                   animationSeconds * 1.25 * b.wobFreq + b.phase)) *
	               0.026f * sizeCalm;

	wobble += popFlash * 0.018f;

	REAL rx = r * (1.0f + wobble);
	REAL ry = r * (1.0f - wobble);

	RectF rect(x - rx, y - ry, rx * 2.0f, ry * 2.0f);

	REAL rootR = sqrtf(r > 1.0f ? r : 1.0f);

	// Optional subtle lower-right shadow/reflection. It is an arc, not a filled
	// disc, so the live desktop/video remains visible through the membrane.
	if (g_settings.showShadows) {
		REAL shadowOffset = 1.5f + rootR * 0.28f;
		if (shadowOffset > 14.0f)
			shadowOffset = 14.0f;

		REAL shadowWidth = 1.0f + rootR * 0.16f;
		if (shadowWidth > 5.0f)
			shadowWidth = 5.0f;

		g_bubbleGdi->shadowPen.SetColor(Color(Alpha(30.0f), 0, 0, 0));
		g_bubbleGdi->shadowPen.SetWidth(shadowWidth);

		gfx.DrawArc(&g_bubbleGdi->shadowPen, rect.X + shadowOffset,
		            rect.Y + shadowOffset, rect.Width, rect.Height, 18.0f,
		            145.0f);
	}

	// Slowly travelling thin-film interference colours. The arcs are deliberately
	// asymmetric so the bubble doesn't look like a neon ring.
	double colourTime = animationSeconds * 1000.0 * g_settings.ColorCycle();
	double colourTravel = colourTime * 0.0055;
	float hueBreath =
	    static_cast<float>(std::sin(colourTime * 0.00022 + b.phase) * 24.0);
	float baseHue = fmodf(b.hue + hueBreath + 360.0f, 360.0f);

	REAL filmWidth = 1.15f + rootR * 0.19f;
	if (filmWidth > 5.2f)
		filmWidth = 5.2f;
	filmWidth *= 1.0f + popFlash * 0.55f;

	if (g_settings.iridescence) {
		struct FilmArc {
			float hueOffset;
			float angleOffset;
			float sweep;
			float alpha;
		};

		const FilmArc arcs[] = {
		    {0.0f, 8.0f, 82.0f, 142.0f},
		    {112.0f, 126.0f, 70.0f, 118.0f},
		    {226.0f, 232.0f, 88.0f, 132.0f},
		    {308.0f, 326.0f, 48.0f, 92.0f},
		};

		// GDI+ receives a bounded float angle even after long runs.
		float rotation = static_cast<float>(
		    std::fmod(b.phase * 57.2958 + colourTravel, 360.0));

		for (const auto& arc : arcs) {
			float hue = fmodf(baseHue + arc.hueOffset, 360.0f);
			Color c = HslToColor(hue, 0.92f, 0.68f,
			                     Alpha(arc.alpha + popFlash * 72.0f));

			g_bubbleGdi->irisPen.SetColor(c);
			g_bubbleGdi->irisPen.SetWidth(filmWidth);

			gfx.DrawArc(&g_bubbleGdi->irisPen, rect.X, rect.Y, rect.Width,
			            rect.Height, rotation + arc.angleOffset, arc.sweep);
		}
	}

	// Neutral membrane rim. Kept subtle so the colour arcs remain dominant.
	REAL rimWidth = 0.85f + rootR * 0.105f;
	if (rimWidth > 3.1f)
		rimWidth = 3.1f;
	rimWidth *= 1.0f + popFlash * 1.0f;

	g_bubbleGdi->rimPen.SetColor(
	    Color(Alpha(78.0f + popFlash * 125.0f), 238, 248, 255));

	g_bubbleGdi->rimPen.SetWidth(rimWidth);

	gfx.DrawEllipse(&g_bubbleGdi->rimPen, rect);

	// Curved specular highlight in the upper-left quadrant. This reads more like
	// reflected light on a spherical membrane than a large opaque white blob.
	REAL highlightWidth = 1.25f + rootR * 0.13f;
	if (highlightWidth > 3.8f)
		highlightWidth = 3.8f;

	g_bubbleGdi->highlightPen.SetColor(
	    Color(Alpha(188.0f + popFlash * 50.0f), 255, 255, 255));

	g_bubbleGdi->highlightPen.SetWidth(highlightWidth);

	float highlightStart = 205.0f + sinf(b.phase) * 12.0f;
	gfx.DrawArc(&g_bubbleGdi->highlightPen, rect.X + rx * 0.035f,
	            rect.Y + ry * 0.035f, rect.Width - rx * 0.070f,
	            rect.Height - ry * 0.070f, highlightStart, 54.0f);

	// Tiny hot spot near the highlight. Scale it gently instead of turning it
	// into a huge solid patch on very large bubbles.
	REAL dotR = 1.6f + rootR * 0.17f;
	if (dotR > r * 0.065f)
		dotR = r * 0.065f;
	if (dotR < 1.1f)
		dotR = 1.1f;

	g_bubbleGdi->shineBrush.SetColor(Color(Alpha(222.0f), 255, 255, 255));

	gfx.FillEllipse(&g_bubbleGdi->shineBrush, x - rx * 0.31f - dotR,
	                y - ry * 0.34f - dotR, dotR * 2.0f, dotR * 2.0f);

	// Very faint opposite-side reflection gives the sphere a little depth.
	// Level of detail: on very small bubbles it is practically invisible
	// (about 1 px wide at alpha 44), so it is skipped there.
	if (r >= 36.0f) {
		g_bubbleGdi->lowerReflectionPen.SetColor(
		    Color(Alpha(44.0f), 210, 235, 255));

		g_bubbleGdi->lowerReflectionPen.SetWidth(rimWidth * 0.82f);

		gfx.DrawArc(&g_bubbleGdi->lowerReflectionPen, rect.X + rx * 0.055f,
		            rect.Y + ry * 0.055f, rect.Width - rx * 0.110f,
		            rect.Height - ry * 0.110f, 38.0f, 44.0f);
	}

	// Pop: a fast expanding white ring, using the same live bubble geometry.
	if (b.popping && popFlash > 0.02f) {
		REAL extra = r * (0.012f + popFlash * 0.040f);
		RectF popRect(rect.X - extra, rect.Y - extra, rect.Width + extra * 2.0f,
		              rect.Height + extra * 2.0f);

		g_bubbleGdi->flashPen.SetColor(
		    Color(Alpha(220.0f * popFlash), 255, 255, 255));

		g_bubbleGdi->flashPen.SetWidth(rimWidth * (1.0f + popFlash * 1.35f));

		gfx.DrawEllipse(&g_bubbleGdi->flashPen, popRect);
	}
}

// ---- sprite cache ----
// Sprite half-size for a bubble: covers wobble (+2.6 %), pen widths and, if
// enabled, the offset shadow arc.
static int SpriteMarginFor(const Bubble& b) {
	const float extra = g_settings.showShadows ? 24.0f : 10.0f;
	return static_cast<int>(std::ceil(b.r * 1.035f + extra));
}

// Renders (or re-renders) the cached sprite of a non-popping bubble.
static void RenderBubbleSprite(Bubble& b, double animationSeconds) {
	if (!b.sprite)
		b.sprite = std::make_shared<BubbleSprite>();

	BubbleSprite& sp = *b.sprite;

	const int margin = SpriteMarginFor(b);
	const int size = margin * 2;

	if (sp.w != size || sp.h != size) {
		sp.px.assign(static_cast<size_t>(size) * size, 0);
		sp.w = size;
		sp.h = size;
		sp.cx = margin;
		sp.cy = margin;
	} else {
		std::fill(sp.px.begin(), sp.px.end(), 0u);
	}

	{
		Bitmap bmp(size, size, size * 4, PixelFormat32bppPARGB,
		           reinterpret_cast<BYTE*>(sp.px.data()));
		Graphics gfx(&bmp);

		gfx.SetSmoothingMode(SmoothingModeAntiAlias);
		gfx.SetCompositingQuality(CompositingQualityHighSpeed);
		gfx.SetPixelOffsetMode(PixelOffsetModeHighSpeed);

		DrawBubbleAt(gfx, b, static_cast<float>(margin),
		             static_cast<float>(margin), 1.0f, animationSeconds);
	}

	sp.renderSec = animationSeconds;
	sp.generation = g_spriteGeneration;
}

// Re-renders sprites that are missing or older than the refresh interval.
// Bubbles without any sprite are always rendered immediately (otherwise they
// would be invisible); all other refreshes share a small per-frame time budget,
// oldest first, so they can never stall a frame. Returns the number rendered.
static int RefreshSprites(double animationSeconds) {
	const double interval = g_settings.spriteRefreshMs / 1000.0;
	int rendered = 0;

	g_refreshQueue.clear();

	for (size_t i = 0; i < g_bubbles.size(); ++i) {
		Bubble& b = g_bubbles[i];
		if (b.popping)
			continue;

		if (!b.sprite || b.sprite->w <= 0) {
			RenderBubbleSprite(b, animationSeconds);
			++rendered;
		} else if (b.sprite->generation != g_spriteGeneration ||
		           animationSeconds - b.sprite->renderSec >= interval) {
			g_refreshQueue.push_back(i);
		}
	}

	if (g_refreshQueue.empty())
		return rendered;

	auto Key = [](size_t index) -> double {
		const BubbleSprite& sp = *g_bubbles[index].sprite;
		// Sprites drawn with outdated settings go first.
		return sp.generation != g_spriteGeneration ? -1.0e18 : sp.renderSec;
	};

	std::sort(g_refreshQueue.begin(), g_refreshQueue.end(),
	          [&](size_t a, size_t b) { return Key(a) < Key(b); });

	LARGE_INTEGER startQpc{};
	QueryPerformanceCounter(&startQpc);

	for (size_t index : g_refreshQueue) {
		RenderBubbleSprite(g_bubbles[index], animationSeconds);
		++rendered;

		LARGE_INTEGER nowQpc{};
		QueryPerformanceCounter(&nowQpc);
		const double spentMs =
		    static_cast<double>(nowQpc.QuadPart - startQpc.QuadPart) * 1000.0 /
		    static_cast<double>(g_qpcFrequency.QuadPart);
		if (spentMs >= SPRITE_REFRESH_BUDGET_MS)
			break;
	}

	return rendered;
}

// Conservative screen rectangle covered by a live-drawn bubble.
static RECT LiveBubbleBounds(const Bubble& b) {
	float popP = b.popping ? b.popProgress : 0.0f;
	if (popP < 0.0f)
		popP = 0.0f;
	if (popP > 1.0f)
		popP = 1.0f;

	const float popEase = 1.0f - (1.0f - popP) * (1.0f - popP);
	const float popScale = 1.0f + g_settings.PopGrowth() * popEase;

	// wobble (2.6 % + 1.8 % flash) + pop ring (up to ~5.2 %) + pens + shadow.
	const float extent =
	    b.r * popScale * 1.10f + (g_settings.showShadows ? 26.0f : 12.0f);

	RECT rc;
	rc.left = static_cast<LONG>(std::floor(b.x - extent));
	rc.top = static_cast<LONG>(std::floor(b.y - extent));
	rc.right = static_cast<LONG>(std::ceil(b.x + extent));
	rc.bottom = static_cast<LONG>(std::ceil(b.y + extent));

	rc.left = MaxI(static_cast<int>(rc.left), 0);
	rc.top = MaxI(static_cast<int>(rc.top), 0);
	rc.right = MinI(static_cast<int>(rc.right), g_vw);
	rc.bottom = MinI(static_cast<int>(rc.bottom), g_vh);
	return rc;
}

void RenderFrame() {
	if (!g_saverOn || !g_saverWnd)
		return;

	BeginPerformanceFrame();

	const bool perfEnabled =
	    g_settings.showPerformanceStats || g_settings.logPerformanceStats;

	LARGE_INTEGER perfMark = g_perf.renderStart;

	auto PerfMark = [&](double& destination) {
		if (!perfEnabled)
			return;

		LARGE_INTEGER q{};
		QueryPerformanceCounter(&q);

		destination = PerfDeltaMs(perfMark, q);

		perfMark = q;
	};

	ULONGLONG now64 = GetTickCount64();

	// Use QPC for physics and animation. GetTickCount64 can advance in
	// coarse ~15.6 ms steps, even with an accurate QPC frame pacer.
	LARGE_INTEGER nowQpc{};
	QueryPerformanceCounter(&nowQpc);
	float dt = 1.0f / 60.0f;
	double animationSeconds = 0.0;
	if (g_qpcFrequency.QuadPart > 0) {
		if (g_lastFrameQpc != 0) {
			dt = static_cast<float>(
			    static_cast<double>(nowQpc.QuadPart - g_lastFrameQpc) /
			    static_cast<double>(g_qpcFrequency.QuadPart));
			// Avoid jumps after breakpoints or a stalled renderer.
			if (dt < 0.001f)
				dt = 0.001f;
			if (dt > 0.500f)
				dt = 0.500f;
		}
		if (g_showQpc != 0) {
			animationSeconds =
			    static_cast<double>(nowQpc.QuadPart - g_showQpc) /
			    static_cast<double>(g_qpcFrequency.QuadPart);
		}
	}
	g_lastFrameQpc = nowQpc.QuadPart;

	float t =
	    static_cast<float>(now64 - g_showTick) / static_cast<float>(FADE_IN_MS);

	if (t > 1.0f)
		t = 1.0f;
	if (t < 0.0f)
		t = 0.0f;

	g_fade = 1.0f - (1.0f - t) * (1.0f - t);

	float drift = g_settings.Drift();

	// Velocities are tuned for 60 updates per second.
	float frameScale = dt * 60.0f;

	// Add one bubble at a time until the configured target is reached.
	// When an old bubble expires, the same mechanism replenishes it later.
	if (static_cast<int>(g_bubbles.size()) < g_settings.bubbleCount &&
	    now64 - g_lastBubbleSpawnTick >=
	        static_cast<ULONGLONG>(g_settings.spawnIntervalMs)) {
		g_bubbles.push_back(SpawnBubble(g_vw, g_vh));
		g_lastBubbleSpawnTick = now64;
	}

	// Integrate positions, age bubbles, and bounce at all four walls.
	// When life reaches zero, the bubble switches into a short visible pop state.
	for (auto& b : g_bubbles) {
		if (!b.popping) {
			// Optional smooth air current. It gently bends trajectories without
			// replacing the existing billiard-style velocity/collision system.
			if (g_settings.turbulence > 0) {
				float turbulenceAmount = g_settings.turbulence / 100.0f;

				float windX =
				    static_cast<float>(
				        std::sin(animationSeconds * 0.31 + b.phase)) +
				    static_cast<float>(std::sin(
				        animationSeconds * 0.17 * b.wobFreq + b.phase * 1.73)) *
				        0.55f;

				float windY =
				    static_cast<float>(
				        std::cos(animationSeconds * 0.27 + b.phase * 0.83)) +
				    static_cast<float>(std::sin(
				        animationSeconds * 0.13 * b.wobFreq + b.phase * 2.11)) *
				        0.45f;

				float windAcceleration =
				    turbulenceAmount * 0.0065f * frameScale;

				b.vx += windX * windAcceleration;
				b.vy += windY * windAcceleration;

				// Keep turbulence from accelerating a bubble forever.
				float speedSq = b.vx * b.vx + b.vy * b.vy;
				float maxSpeed = 2.3f + turbulenceAmount * 1.7f;

				if (speedSq > maxSpeed * maxSpeed) {
					float speed = sqrtf(speedSq);
					if (speed > 0.0001f) {
						float scale = maxSpeed / speed;
						b.vx *= scale;
						b.vy *= scale;
					}
				}
			}

			b.x += b.vx * drift * frameScale;
			b.y += b.vy * drift * frameScale;

			// Tiny organic side drift. This is not a second velocity integration.
			b.x += static_cast<float>(
			           std::sin(animationSeconds * b.wobFreq + b.phase)) *
			       b.wobAmp * 0.08f * frameScale;

			b.life -= dt;

			if (b.life <= 0.0f) {
				b.life = 0.0f;
				b.popping = true;
				b.popProgress = g_settings.popEnabled ? 0.0f : 1.0f;
				// The cached sprite is no longer used: popping bubbles are
				// drawn live (they change every frame).
				b.sprite.reset();
			}
		} else {
			// Let a popping bubble keep a little of its momentum instead of
			// freezing in place during the membrane burst.
			b.x += b.vx * drift * frameScale * 0.30f;
			b.y += b.vy * drift * frameScale * 0.30f;

			if (g_settings.popEnabled) {
				float popDt = dt;
				if (popDt > 0.050f)
					popDt = 0.050f;

				float popDuration = g_settings.PopDurationSec();

				if (popDuration < 0.050f)
					popDuration = 0.050f;

				b.popProgress += popDt / popDuration;
			} else {
				b.popProgress = 1.0f;
			}
		}

		// Edge behaviour is configurable: classic bounce, or seamless wrap.
		if (g_settings.wallCollisions) {
			if (b.x - b.r < 0.0f) {
				b.x = b.r;
				b.vx = fabsf(b.vx);
			} else if (b.x + b.r > static_cast<float>(g_vw)) {
				b.x = static_cast<float>(g_vw) - b.r;
				b.vx = -fabsf(b.vx);
			}

			if (b.y - b.r < 0.0f) {
				b.y = b.r;
				b.vy = fabsf(b.vy);
			} else if (b.y + b.r > static_cast<float>(g_vh)) {
				b.y = static_cast<float>(g_vh) - b.r;
				b.vy = -fabsf(b.vy);
			}
		} else {
			if (b.x < -b.r)
				b.x = static_cast<float>(g_vw) + b.r;
			else if (b.x > static_cast<float>(g_vw) + b.r)
				b.x = -b.r;

			if (b.y < -b.r)
				b.y = static_cast<float>(g_vh) + b.r;
			else if (b.y > static_cast<float>(g_vh) + b.r)
				b.y = -b.r;
		}
	}

	// Remove bubbles only after their visible pop animation has completed.
	// Reset the spawn clock so a replacement arrives after a short pause.
	bool removedPoppedBubble = false;
	for (size_t i = 0; i < g_bubbles.size();) {
		if (g_bubbles[i].popping && g_bubbles[i].popProgress >= 1.0f) {
			g_bubbles.erase(g_bubbles.begin() + i);
			removedPoppedBubble = true;
		} else {
			++i;
		}
	}

	if (removedPoppedBubble)
		g_lastBubbleSpawnTick = now64;

	// Elastic bubble-to-bubble collisions.
	// Equal collision mass gives the lively "billiard-ball" reaction used by
	// classic Bubbles-style implementations, while radii still differ visually.
	if (g_settings.bubbleCollisions) {
		const size_t bubbleCount = g_bubbles.size();
		for (size_t i = 0; i < bubbleCount; ++i) {
			for (size_t j = i + 1; j < bubbleCount; ++j) {
				Bubble& a = g_bubbles[i];
				Bubble& b = g_bubbles[j];

				// A burst membrane no longer participates in billiard physics.
				if (a.popping || b.popping)
					continue;

				float dx = b.x - a.x;
				float dy = b.y - a.y;
				float minDist = a.r + b.r;
				float distSq = dx * dx + dy * dy;

				if (distSq >= minDist * minDist)
					continue;

				float dist;
				float nx;
				float ny;

				if (distSq > 0.0001f) {
					dist = sqrtf(distSq);
					nx = dx / dist;
					ny = dy / dist;
				} else {
					dist = 0.0f;
					nx = 1.0f;
					ny = 0.0f;
				}

				// Separate overlapping circles equally so they don't stick together.
				float overlap = minDist - dist;
				float correction = overlap * 0.5f + 0.01f;

				a.x -= nx * correction;
				a.y -= ny * correction;
				b.x += nx * correction;
				b.y += ny * correction;

				float rvx = b.vx - a.vx;
				float rvy = b.vy - a.vy;
				float velocityAlongNormal = rvx * nx + rvy * ny;

				// Already moving apart: positional correction is enough.
				if (velocityAlongNormal >= 0.0f)
					continue;

				// Equal-mass elastic impulse, very slightly damped.
				float impulse = -(1.0f + COLLISION_RESTITUTION) *
				                velocityAlongNormal * 0.5f;

				float impulseX = impulse * nx;
				float impulseY = impulse * ny;

				a.vx -= impulseX;
				a.vy -= impulseY;
				b.vx += impulseX;
				b.vy += impulseY;
			}
		}
	}

	// Collision separation can push circles a few pixels beyond an edge.
	if (g_settings.wallCollisions) {
		for (auto& b : g_bubbles) {
			if (b.popping)
				continue;

			if (b.x - b.r < 0.0f) {
				b.x = b.r;
				b.vx = fabsf(b.vx);
			} else if (b.x + b.r > static_cast<float>(g_vw)) {
				b.x = static_cast<float>(g_vw) - b.r;
				b.vx = -fabsf(b.vx);
			}

			if (b.y - b.r < 0.0f) {
				b.y = b.r;
				b.vy = fabsf(b.vy);
			} else if (b.y + b.r > static_cast<float>(g_vh)) {
				b.y = static_cast<float>(g_vh) - b.r;
				b.vy = -fabsf(b.vy);
			}
		}
	}

	PerfMark(g_perf.current.physicsMs);

	if (!g_memDC)
		CreateBackBuffer(g_vw, g_vh);
	if (!g_memDC)
		return;

	const bool useSprites = g_settings.spriteCache != FALSE;

	// ---- 1. Re-render stale / new sprites (time-budgeted) ----
	if (useSprites) {
		const int refreshed = RefreshSprites(animationSeconds);
		if (perfEnabled)
			g_perf.current.refreshed = refreshed;
	}
	PerfMark(g_perf.current.spriteMs);

	// ---- 2. Clear: complete surface, or only last frame's areas ----
	const BYTE dimA = static_cast<BYTE>(std::lround(
	    static_cast<float>(g_settings.dimPercent) * 255.0f / 100.0f * g_fade));
	const DWORD dimPixel = MakeDimPixel(dimA);

	const bool fullClear = g_fullClearPending ||
	                       g_lastDimA != static_cast<int>(dimA) ||
	                       !g_settings.dirtyRects;

	if (fullClear) {
		ClearDibAll(dimPixel);
		g_fullClearPending = false;
		g_lastDimA = static_cast<int>(dimA);
	} else {
		for (const RECT& rc : g_prevRects)
			ClearDibRect(rc, dimPixel);
	}
	PerfMark(g_perf.current.clearMs);

	// ---- 3. Draw bubbles ----
	g_curRects.clear();
	bool drewLive = false;

	const uint32_t fadeQ256 = static_cast<uint32_t>(g_fade * 256.0f + 0.5f);

	for (const auto& b : g_bubbles) {
		if (!b.popping && useSprites) {
			if (!b.sprite || b.sprite->w <= 0)
				continue;

			float a = b.alpha * g_fade;
			if (a < 0.0f)
				a = 0.0f;
			if (a > 1.0f)
				a = 1.0f;
			const uint32_t ca = static_cast<uint32_t>(a * 256.0f + 0.5f);

			RECT rc;
			if (BlitSprite(*b.sprite,
			               static_cast<int>(std::lround(b.x)) - b.sprite->cx,
			               static_cast<int>(std::lround(b.y)) - b.sprite->cy,
			               ca, &rc)) {
				g_curRects.push_back(rc);
			}
		} else if (g_surfaceGfx) {
			// Popping bubbles (and everything when the sprite cache is off)
			// are drawn live with GDI+.
			DrawBubbleAt(*g_surfaceGfx, b, b.x, b.y, b.alpha * g_fade,
			             animationSeconds);
			g_curRects.push_back(LiveBubbleBounds(b));
			drewLive = true;
		}
	}

	if (drewLive && g_surfaceGfx)
		g_surfaceGfx->Flush(FlushIntentionSync);

	PerfMark(g_perf.current.bubbleDrawMs);

	// ---- 4. Cached diagnostics overlay ----
	if (g_settings.showPerformanceStats && g_fade > 0.01f) {
		if (g_hudDirty || g_hudSprite.px.empty())
			RenderHudSprite();

		RECT rc;
		if (BlitSprite(g_hudSprite, HUD_X, HUD_Y, fadeQ256, &rc))
			g_curRects.push_back(rc);
	}
	PerfMark(g_perf.current.hudMs);

	// ---- 5. Present (only the changed area when possible) ----
	RECT dirty{LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN};
	bool haveDirty = false;
	auto AddDirty = [&](const RECT& rc) {
		if (rc.left >= rc.right || rc.top >= rc.bottom)
			return;
		if (rc.left < dirty.left)
			dirty.left = rc.left;
		if (rc.top < dirty.top)
			dirty.top = rc.top;
		if (rc.right > dirty.right)
			dirty.right = rc.right;
		if (rc.bottom > dirty.bottom)
			dirty.bottom = rc.bottom;
		haveDirty = true;
	};
	for (const RECT& rc : g_prevRects)
		AddDirty(rc);
	for (const RECT& rc : g_curRects)
		AddDirty(rc);

	const double totalPixels =
	    static_cast<double>(g_vw) * static_cast<double>(g_vh);

	bool presentWhole = fullClear;
	bool skipPresent = false;

	if (!presentWhole) {
		if (!haveDirty) {
			skipPresent = true; // nothing changed since last frame
		} else {
			const double area = static_cast<double>(dirty.right - dirty.left) *
			                    static_cast<double>(dirty.bottom - dirty.top);
			// A nearly full-screen dirty box is not worth the bookkeeping.
			if (area >= totalPixels * 0.90)
				presentWhole = true;
			if (perfEnabled && totalPixels > 0.0)
				g_perf.current.dirtyPct = area * 100.0 / totalPixels;
		}
	}
	if (presentWhole)
		g_perf.current.dirtyPct = 100.0;
	if (skipPresent)
		g_perf.current.dirtyPct = 0.0;

	if (!skipPresent) {
		POINT ptSrc{0, 0};
		SIZE sz{g_vw, g_vh};
		BLENDFUNCTION bf{};
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255; // per-pixel alpha in DIB
		bf.AlphaFormat = AC_SRC_ALPHA;

		UPDATELAYEREDWINDOWINFO info{};
		info.cbSize = sizeof(info);
		info.hdcDst = nullptr;
		info.pptDst = nullptr;
		info.psize = &sz;
		info.hdcSrc = g_memDC;
		info.pptSrc = &ptSrc;
		info.crKey = 0;
		info.pblend = &bf;
		info.dwFlags = ULW_ALPHA;
		info.prcDirty = presentWhole ? nullptr : &dirty;

		BOOL presented = UpdateLayeredWindowIndirect(g_saverWnd, &info);
		if (!presented && !presentWhole) {
			// A full-surface update may work even if the dirty-rect path fails.
			info.prcDirty = nullptr;
			presented = UpdateLayeredWindowIndirect(g_saverWnd, &info);
		}
		if (!presented) {
			static ULONGLONG lastPresentErrorLog = 0;
			const DWORD error = GetLastError();
			const ULONGLONG now = GetTickCount64();
			if (lastPresentErrorLog == 0 || now - lastPresentErrorLog >= 5000) {
				Wh_Log(L"UpdateLayeredWindowIndirect failed (%lu)", error);
				lastPresentErrorLog = now;
			}
			g_fullClearPending = true; // Retry with a clean full frame next time.
		}
	}

	PerfMark(g_perf.current.presentMs);

	// What this frame touched must be cleared next frame.
	g_prevRects.swap(g_curRects);

	EndPerformanceFrame();
}

// ================= show / hide =================
// Transparent topmost layered window, click-through by design.
// DWM keeps compositing the live desktop underneath.
void ShowSaver() {
	if (g_saverOn || IsDesktopUnavailable() ||
	    GetTickCount64() < g_resumeGraceUntil || !g_inputWindow) {
		return;
	}

	if (g_settings.multiMonitor) {
		g_vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
		g_vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
		g_vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		g_vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	} else {
		g_vx = 0;
		g_vy = 0;
		g_vw = GetSystemMetrics(SM_CXSCREEN);
		g_vh = GetSystemMetrics(SM_CYSCREEN);
	}
	if (g_vw < 64)
		g_vw = 64;
	if (g_vh < 64)
		g_vh = 64;

	if (!g_classRegistered) {
		Wh_Log(L"Saver window class is not registered; skipping saver");
		g_resumeGraceUntil = GetTickCount64() + 5000;
		return;
	}

	// Arm hooks on the input thread before revealing the overlay.
	// Never show an overlay that cannot be dismissed by user input.
	DWORD_PTR armResult = 0;
	UINT_PTR serial = ++g_showSerial;
	if (!SendMessageTimeoutW(g_inputWindow, WM_BUBBLE_INPUT_ARM,
	                         g_settings.dismissDeltaPx,
	                         static_cast<LPARAM>(serial),
	                         SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &armResult) ||
	    armResult != TRUE) {
		Wh_Log(L"Input hooks unavailable; skipping saver");
		g_resumeGraceUntil = GetTickCount64() + 5000;
		return;
	}

	g_saverWnd =
	    CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
		                    WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
		                L"BubbleGlassSaver", L"", WS_POPUP, g_vx, g_vy, g_vw,
		                g_vh, nullptr, nullptr, GetModInstance(), nullptr);
	if (!g_saverWnd) {
		SendMessageTimeoutW(g_inputWindow, WM_BUBBLE_INPUT_DISARM, 0, 0,
		                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, nullptr);
		Wh_Log(L"CreateWindowExW(saver) failed (%lu)", GetLastError());
		return;
	}

	CreateBackBuffer(g_vw, g_vh);
	SeedBubbles(g_vw, g_vh);
	g_showTick = GetTickCount64();
	g_fade = 0.0f;
	ResetPerformanceStats();
	g_prevRects.clear();
	g_curRects.clear();
	g_fullClearPending = true;
	g_lastDimA = -1;
	g_saverOn = true;

	ShowWindow(g_saverWnd, SW_SHOWNOACTIVATE);
	SetWindowPos(g_saverWnd, HWND_TOPMOST, 0, 0, 0, 0,
	             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	StartRenderLoop();
	RenderFrame(); // first frame immediately (no black flash)

	Wh_Log(L"Saver shown (%dx%d @ %d,%d, %d bubbles)", g_vw, g_vh, g_vx, g_vy,
	       g_settings.bubbleCount);
}

void HideSaver(const char* reason) {
	if (!g_saverOn)
		return;
	g_saverOn = false;
	if (g_inputWindow) {
		SendMessageTimeoutW(g_inputWindow, WM_BUBBLE_INPUT_DISARM, 0, 0,
		                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, nullptr);
	}
	StopRenderLoop();
	if (g_saverWnd) {
		DestroyWindow(g_saverWnd);
		g_saverWnd = nullptr;
	}
	DestroyBackBuffer();
	g_bubbles.clear();
	g_lastBubbleSpawnTick = 0;
	g_prevRects.clear();
	g_curRects.clear();
	g_refreshQueue.clear();
	// Release the large sprite buffers while the saver is not visible.
	std::vector<DWORD>().swap(g_hudSprite.px);
	g_hudSprite.w = g_hudSprite.h = 0;
	ResetPerformanceStats();
	ScheduleIdleCheck();
	Wh_Log(L"Saver hidden (%S)", reason ? reason : "input");
}

DWORD WINAPI BubbleUiThread(LPVOID) {
	g_uiThreadId = GetCurrentThreadId();

	// Important: create the message queue for this thread.
	MSG msg{};
	PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

	RegisterSaverClass();
	RegisterControlWindow();

	// High-resolution frame timer (Windows 10 1803+; the mod targets
	// Windows 11). The message loop falls back to a plain timeout when
	// the timer cannot be created, so no timer-resolution hack is needed.
	g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr,
	                                      CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
	                                      TIMER_MODIFY_STATE | SYNCHRONIZE);
	if (!g_frameTimer) {
		Wh_Log(L"High-resolution frame timer failed (%lu)", GetLastError());
	}

	// Independent, message-only hook thread: never waits on GDI+ rendering.
	g_inputReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
	if (g_inputReadyEvent) {
		g_inputThread = CreateThread(nullptr, 0, BubbleInputThread, nullptr, 0,
		                             &g_inputThreadId);
		if (g_inputThread) {
			if (WaitForSingleObject(g_inputReadyEvent, 5000) != WAIT_OBJECT_0 ||
			    !g_inputWindow) {
				Wh_Log(L"Input thread did not start correctly");
			}
		} else {
			Wh_Log(L"Cannot create input thread (%lu)", GetLastError());
		}
	} else {
		Wh_Log(L"Cannot create input ready event (%lu)", GetLastError());
	}

	// Start the idle watchdog on this thread as well.
	ScheduleIdleCheck();

	g_uiStartupOk = g_classRegistered && g_controlWnd &&
	                g_sessionNotificationsRegistered &&
	                g_displayPowerNotification && g_inputWindow && g_idleTimer;
	if (!g_uiStartupOk)
		Wh_Log(L"UI startup incomplete; refusing to initialize");
	if (g_uiReadyEvent)
		SetEvent(g_uiReadyEvent);

	Wh_Log(L"UI thread running, thread=%lu", g_uiThreadId);

	bool running = true;

	while (running) {
		DWORD waitMs = INFINITE;
		HANDLE waits[1]{};
		DWORD waitCount = 0;

		if (g_saverOn && g_rendering && g_qpcFrequency.QuadPart > 0) {

			LARGE_INTEGER nowQpc{};
			QueryPerformanceCounter(&nowQpc);

			LONGLONG remaining = g_nextFrameQpc - nowQpc.QuadPart;

			if (remaining <= 0) {
				// Deadline already reached.
				waitMs = 0;
			} else if (g_frameTimer) {
				// Convert QPC ticks to relative 100-ns units.
				LONGLONG due100ns =
				    (remaining * 10000000LL) / g_qpcFrequency.QuadPart;

				if (due100ns < 1)
					due100ns = 1;

				LARGE_INTEGER due{};
				due.QuadPart = -due100ns;

				SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr,
				                 FALSE);

				waits[0] = g_frameTimer;
				waitCount = 1;
			} else {
				// Very unlikely final fallback.
				LONGLONG remainingMs =
				    (remaining * 1000LL + g_qpcFrequency.QuadPart - 1) /
				    g_qpcFrequency.QuadPart;

				if (remainingMs < 1)
					remainingMs = 1;

				if (remainingMs > 1000)
					remainingMs = 1000;

				waitMs = static_cast<DWORD>(remainingMs);
			}
		}

		MsgWaitForMultipleObjectsEx(waitCount, waitCount ? waits : nullptr,
		                            waitMs, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}

			if (msg.message == WM_BUBBLE_DISMISS) {
				// Ignore late events from a previous saver activation.
				if (g_saverOn &&
				    static_cast<UINT_PTR>(msg.wParam) == g_showSerial) {
					const char* reason =
					    msg.lParam == DISMISS_KEYSTROKE ? "keystroke"
						: msg.lParam == DISMISS_CLICK   ? "click/wheel"
						: msg.lParam == DISMISS_DISPLAY_CHANGE
					        ? "display-change"
					        : "mouse-move";
					if (msg.lParam == DISMISS_DISPLAY_CHANGE) {
						// An old Windows last-input timestamp must not cause an
						// immediate redisplay after a resolution/layout change.
						g_resumeGraceUntil =
						    GetTickCount64() +
						    static_cast<ULONGLONG>(g_settings.idleTimeoutSec) * 1000u;
					}
					HideSaver(reason);
				}
				continue;
			}

			if (msg.message == WM_BUBBLE_SETTINGS_CHANGED) {
				LoadSettings();
				ScheduleIdleCheck();

				if (g_saverOn) {
					if (static_cast<int>(g_bubbles.size()) >
					    g_settings.bubbleCount) {

						g_bubbles.resize(static_cast<size_t>(
						    g_settings.bubbleCount > 0 ? g_settings.bubbleCount
							                           : 0));
					}

					if (static_cast<int>(g_bubbles.size()) <
					    g_settings.bubbleCount) {
						g_lastBubbleSpawnTick = 0;
					}
				}

				continue;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		if (!running)
			break;

		if (g_saverOn && g_rendering && g_qpcFrequency.QuadPart > 0) {

			LARGE_INTEGER nowQpc{};
			QueryPerformanceCounter(&nowQpc);

			if (nowQpc.QuadPart >= g_nextFrameQpc) {
				RenderFrame();

				// Advance the absolute timeline instead of
				// waiting another interval after rendering.
				g_nextFrameQpc += g_frameIntervalQpc;

				LARGE_INTEGER afterQpc{};
				QueryPerformanceCounter(&afterQpc);

				// If something stalls us for several complete frames,
				// re-anchor once instead of entering a catch-up loop.
				if (afterQpc.QuadPart >
				    g_nextFrameQpc + g_frameIntervalQpc * 3) {

					g_nextFrameQpc = afterQpc.QuadPart + g_frameIntervalQpc;
				}
			}
		}
	}

	// Tear down everything owned by this thread before returning.
	HideSaver("thread-exit");

	if (g_idleTimer) {
		KillTimer(nullptr, g_idleTimer);
		g_idleTimer = 0;
	}

	// Hooks and their control window belong to the input thread.
	if (g_inputWindow)
		PostMessageW(g_inputWindow, WM_CLOSE, 0, 0);
	else if (g_inputThreadId)
		PostThreadMessageW(g_inputThreadId, WM_QUIT, 0, 0);
	if (g_inputThread) {
		// On timeout terminate the dedicated tool process, never leave
		// executing hook callbacks in a module about to be unloaded.
		if (WaitForSingleObject(g_inputThread, 5000) != WAIT_OBJECT_0) {
			Wh_Log(L"Input thread did not exit in time");
			ExitProcess(1);
		}
		CloseHandle(g_inputThread);
		g_inputThread = nullptr;
		g_inputThreadId = 0;
		g_inputWindow = nullptr;
	}
	if (g_inputReadyEvent) {
		CloseHandle(g_inputReadyEvent);
		g_inputReadyEvent = nullptr;
	}

	if (g_frameTimer) {
		CancelWaitableTimer(g_frameTimer);
		CloseHandle(g_frameTimer);
		g_frameTimer = nullptr;
	}

	UnregisterControlWindow();
	UnregisterSaverClass();

	g_uiThreadId = 0;

	return 0;
}

// ================= Windhawk entry points =================
void WhTool_ModUninit();

BOOL WhTool_ModInit() {
	GdiplusStartupInput gdiInput;

	if (GdiplusStartup(&g_gdiplusToken, &gdiInput, nullptr) != Ok) {

		Wh_Log(L"GdiplusStartup failed");
		return FALSE;
	}
	g_bubbleGdi = std::make_unique<BubbleGdiResources>();

	std::random_device rd;

	g_rng.seed(rd() ^ GetTickCount() ^ GetCurrentProcessId());

	LoadSettings();

	g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

	if (!g_uiReadyEvent) {
		Wh_Log(L"CreateEvent failed (%lu)", GetLastError());

		g_bubbleGdi.reset();

		GdiplusShutdown(g_gdiplusToken);
		g_gdiplusToken = 0;
		return FALSE;
	}

	g_uiThread = CreateThread(nullptr, 0, BubbleUiThread, nullptr, 0, nullptr);

	if (!g_uiThread) {
		Wh_Log(L"CreateThread failed (%lu)", GetLastError());

		CloseHandle(g_uiReadyEvent);
		g_uiReadyEvent = nullptr;

		g_bubbleGdi.reset();

		GdiplusShutdown(g_gdiplusToken);
		g_gdiplusToken = 0;

		return FALSE;
	}

	// Never report successful initialization for an incomplete or stalled UI.
	DWORD ready = WaitForSingleObject(g_uiReadyEvent, 5000);
	if (ready != WAIT_OBJECT_0 || !g_uiStartupOk) {
		Wh_Log(L"UI initialization failed (wait=%lu, ready=%d)", ready,
		       static_cast<int>(g_uiStartupOk));
		WhTool_ModUninit(); // Joins the UI thread or exits the tool process on timeout.
		return FALSE;
	}

	Wh_Log(L"Initialized v1.7.6: timeout=%ds, bubbles=%d, "
	       L"spriteCache=%d, dirtyRects=%d",
	       g_settings.idleTimeoutSec, g_settings.bubbleCount,
	       static_cast<int>(g_settings.spriteCache),
	       static_cast<int>(g_settings.dirtyRects));

	return TRUE;
}

void WhTool_ModSettingsChanged() {
	if (g_uiThreadId) {
		PostThreadMessageW(g_uiThreadId, WM_BUBBLE_SETTINGS_CHANGED, 0, 0);
	}
}

void WhTool_ModUninit() {
	if (g_uiThreadId) {
		PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
	}

	if (g_uiThread) {
		// Terminate only the dedicated tool process if shutdown stalls.
		// Never unload the mod while its UI thread might still run.
		if (WaitForSingleObject(g_uiThread, 5000) != WAIT_OBJECT_0) {
			Wh_Log(L"UI thread did not exit in time");
			ExitProcess(1);
		}

		CloseHandle(g_uiThread);
		g_uiThread = nullptr;
		g_uiThreadId = 0;
	}

	if (g_uiReadyEvent) {
		CloseHandle(g_uiReadyEvent);
		g_uiReadyEvent = nullptr;
	}

	if (g_gdiplusToken) {

		g_bubbleGdi.reset();

		GdiplusShutdown(g_gdiplusToken);
		g_gdiplusToken = 0;
	}
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
