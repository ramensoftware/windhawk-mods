// ==WindhawkMod==
// @id              touchpad-relaxed-hand-tracking
// @name            Touchpad Relaxed Hand Tracking
// @description     Enables 5-2 fingers mouse pointer tracking for improved hand ergonomics (admin windows need Windhawk 2)
// @version         1.1.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -lhid -lwtsapi32 -lgdi32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Touchpad Relaxed Hand Tracking

Move the pointer with more fingers resting on the precision touchpad, as if gliding a weightless mouse. Your hand can relax with more natural finger movements, instead of holding a single digit position over the pad.

## How it works

- **Trigger:** with 5 (or as few as configured) fingers touching the pad.
- **Disarm:** after all fingers leave the touchpad. Lifting some keeps the glide going.
- **Movement:** each finger weighs as much as it moves, so resting fingers drop out and don't fight the others. The thumb is completely ignored (unless it is the only one touching, when Windows takes control). With one or two fingers moving, the lower speed multiplier applies; with more moving together, the higher one.
- **Click:** excludes the thumb. You don't need to lift all fingers. With more fingers resting, same as native - single finger (double) tap to (double) click, tap and hold to drag, tap-tap and hold to drag-select text. A hold lasts until another tap or the whole hand leaves the pad. 
- **Scrolling:** Windows 2-finger scroll is ignored while more-fingers-gliding. Instead, 2-finger tap-slide to scroll, while "inside" the more-fingers action (or with 2 fingers detection enabled). **Both will get activated if you start with 2 and rest more on the touchpad.**

## The thumb

It only helps start the glide and counts as a finger on the pad for the speed multiplier: **it never moves the pointer or clicks with more fingers touching the pad.** Alone, Windows takes control. Touchpads read a resting thumb poorly (it slides at its own angle and is often split in two touches), so leaving it out keeps the movement steady. It's recognized by where it lands: the touch farthest inside the hand (set by the **Hand** option) on the lower third of the pad or, with five touches down, the one farthest inside wherever it landed.

## 4 and 3 fingers detection

The options to start with four or three fingers conflict with the respective Windows swipes. **Disable them in Settings → Bluetooth & devices → Touchpad → Four-finger gestures (or Three) → Swipes: Nothing**. Otherwise Windows simultaneously triggers the swipe. Four and three-finger taps can keep their actions. If the Three Finger Drag mod is installed and set to the same finger count, both mods respond to the same swipe, so give them different counts.

## 2 fingers detection

When active, two fingers do everything: glide, right click (tap) and scroll (tap-drag). Windows' own two-finger gestures have to be disabled in **Settings → Bluetooth & devices → Touchpad**: uncheck **Scroll & zoom → Drag two fingers to scroll** and **Taps → Tap with two fingers to right-click**. Otherwise both commands respond. On Windhawk 1.x, with two-finger detection active, programs running as administrator get no two-finger scroll at all, since Windows' will be disabled and the mod can't reach them.

## Speed

The mod learns the touchpad's native speed and acceleration by watching ordinary one-finger use: for each range of finger speed, it compares how far the finger moved with how far Windows moved the pointer. The glide reproduces that curve, and the multipliers apply on top of it: **1.0 is the same as native**, 1.5 is 50% faster. When the touchpad speed is changed in Windows settings, the curve adjusts with use.

Right after installing, before anything is learned, the glide uses a default speed. Using the touchpad normally for a few minutes, with slow and fast movements, is enough for it to learn.

## Diagnostics overlay

Off by default. Set **Diagnostics overlay hotkey** (for example Ctrl+Alt+Shift+F) and press it to show how the mod reads each finger: green steers the pointer (bigger, more weight), gray rests, dark gray is the thumb, yellow may still tap, blue scrolls, orange holds, and a red ring is a palm. The dotted rectangle is the pad's edge band and the line across is the top of the thumb zone. Drag it anywhere; the X closes it.

## Limitations

- Requires a precision touchpad.
- Windows gestures can't be blocked by the mod. Windows picks the gesture by the fingers that came down last: starting with one finger and then resting four more, or lifting and resting fingers again, triggers the gesture for that count. **Disabling Windows three and four fingers swipes removes the conflict.**
- **Programs running as administrator need Windhawk 2.** On Windhawk 1.x, while one is in the foreground, Windows doesn't communicate with the mod, and only the native gestures work.
- Movements against the edge of the screen don't teach the curve, since the pointer stops there while the finger goes on.

## Credits

Reading the touchpad follows the approach of the [Three Finger Drag](https://windhawk.net/mods/three-finger-drag) mod by RodrigoThomazi, under the GPL v3.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hand: right
  $name: Hand
  $description: The hand on the touchpad. To help filter thumb contrary movements.
  $options:
  - right: Right
  - left: Left
- tapToClick: high
  $name: Tap sensitivity
  $description: Tapping with one or two fingers while others rest (three and four-finger taps belong to Windows settings). Low ignores more accidental touches, but needs firmer taps; High is the opposite.
  $options:
  - "off": "Off"
  - low: Low
  - medium: Medium
  - high: High
- multiplierMore: "1.5"
  $name: 5-3 fingers speed multiplier
  $description: When more fingers are gliding and you'd need room to travel. 1.0 is the system's touchpad speed and acceleration, 1.5 is 50% faster.
- multiplierLess: "1.0"
  $name: 2-1 fingers speed multiplier
  $description: When fewer fingers move or are left gliding. 1.0 is native.
- armFive: true
  $name: 5 fingers detection
  $description: The glide starts with five contacts, the thumb included. Off, it only starts with the counts enabled below.
- armFour: false
  $name: 4 fingers detection
  $description: You have to disable the four-finger swipes in Windows touchpad settings, otherwise it also responds.
- armThree: false
  $name: 3 fingers detection
  $description: You have to disable the three-finger swipes in Windows touchpad settings, otherwise it also responds.
- armTwo: false
  $name: 2 fingers detection
  $description: You have to disable "Drag two fingers to scroll" and "Tap with two fingers to right-click" in Windows touchpad settings. TAP to RIGHT CLICK, TAP-SLIDE to SCROLL. You can stay in the scroll operation by putting a finger (thumb) down while other 2 scroll.
- scrollReverse: false
  $name: Reverse scroll direction
  $description: For the two-finger scroll. Off, page follows the fingers, like Windows default.
- scrollOutput: wheel
  $name: Scroll output
  $description: Mouse wheel has a tighter response and more compatibility, but some programs (Excel) scroll badly. Touch scrolls smoothly in programs that support it, with their own inertia, but rolling a background window brings it to the front. Stay inside the scroll action with your thumb for better control.
  $options:
  - wheel: Mouse wheel
  - touch: Touch
- diagHotkey: ""
  $name: Diagnostics overlay hotkey
  $description: Shows how the mod reads each finger, for troubleshooting. Ctrl, Alt, Shift or Win joined with + to a letter, like Win+Shift+F. Empty for none.
*/
// ==/WindhawkModSettings==

// The touchpad is read as raw HID input (Raw Input with RIDEV_INPUTSINK), in a dedicated Windhawk process
// (tool mod block at the end). The pointer is placed with SetCursorPos by the learned native curve, so that
// it doesn't go through the mouse acceleration curve.

#include <hidusage.h>
#include <windowsx.h>
#include <hidpi.h>
#include <shellapi.h>
#include <wtsapi32.h>

// Windhawk 2 ships the tool mod launcher, which can run the mod with UIAccess; Windhawk 1.x needs the one at the
// end of this file.
#if __has_include(<windhawk_tool_mod.h>)
#include <windhawk_tool_mod.h>
#endif
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cwctype>
#include <cstdlib>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

constexpr USAGE kPageGeneric = 0x01;
constexpr USAGE kUsageX = 0x30;
constexpr USAGE kUsageY = 0x31;
constexpr USAGE kPageDigitizer = 0x0D;
constexpr USAGE kUsageTouchPad = 0x05;
constexpr USAGE kUsageTipSwitch = 0x42;
constexpr USAGE kUsageConfidence = 0x47;
constexpr USAGE kUsageContactId = 0x51;
constexpr USAGE kUsageContactCount = 0x54;

constexpr int kFingers = 5;
// How many fingers move, weighing each by how much it moved (1 when one moves alone, 5 when five move as one):
// over the first the higher speed multiplier applies, under the second the lower one, and in between the last
// one stays, so that the multiplier doesn't flap.
constexpr double kHandMovers = 2.7;
constexpr double kFewMovers = 2.3;
// A scroll's movement is judged over kSpeedWindowSec, like the glide's: under this, it's the resting fingers'
// jitter, which goes back and forth and cancels out. Judged frame by frame, slow scrolling, which comes as one
// unit of the pad every few frames, was all dropped as jitter: nothing moved, then it jumped.
constexpr double kScrollStillMm = 0.2;
// Under the first speed the pointer stops, and it only starts again over the second: resting fingers keep
// creeping for a moment after a stop. Both are the hand's speed as StepSpeed measures it, not a frame's.
constexpr double kStopSpeed = 2.5;
constexpr double kStartSpeed = 3.0;
// The touchpad reports whole units (about 0.08 mm) a few dozen times a second, fewer with more fingers down, so
// slow movement comes as one unit every few frames. Read frame by frame, its speed jumped between zero and
// several mm/s, which stopped and started the pointer all the time and lost slow movement. A frame under
// kFastFrameUnits is measured together with the frames like it before it, over up to kSpeedWindowSec and at
// least kSpeedMinSpanSec. A frame over it is its own speed, so that a sudden stop is seen at once.
constexpr double kSpeedWindowSec = 0.08;
constexpr double kSpeedMinSpanSec = 0.04;
constexpr double kFastFrameUnits = 3;
// Each finger weighs as much as it went over this long, squared: resting ones drop out, one moving alone leads
// with its full reach, and a frame's jitter doesn't shift the weights. kWeightFloorUnits is added to each, so
// that a hand at rest weighs its fingers alike.
constexpr double kWeightWindowSec = 0.1;
constexpr double kWeightFloorUnits = 0.5;
// The speed multiplier is only chosen again when some finger went this far over kWeightWindowSec.
constexpr double kMoversMinMm = 0.5;
// The hand's creep after a stop (2–4 mm/s) isn't hand travel: under this, taps still see the hand as still.
constexpr double kTravelSpeed = 4.0;
// More than this in one frame isn't a finger moving: the touchpad gave a contact's id to another contact,
// typically when the thumb lands late, and following it would throw the pointer to the thumb.
constexpr double kJumpMm = 10.0;
// When the contact count changes, the touchpad shifts contacts for a few frames: a finger landing or lifting
// grows or shrinks its contact, and neighbors which it touched are reported apart. Within this long of a count
// change the median is followed, which a single shifted contact doesn't move.
constexpr DWORD kSettleMs = 80;
// The hand comes down over a moment: fingers landing this soon after the glide started still move the pointer,
// otherwise starting with two fingers enabled would leave the rest of a landing hand out.
constexpr DWORD kRefJoinMs = 300;
// The thumb moves at its own angle even with the hand coordinated, and pulled the glide aside, and the touchpad
// reads it poorly (split in two, ids handed around): it's dead. It only counts toward starting the glide and as a
// finger on the pad for the speed multiplier. It never moves the pointer, even alone, nor taps or holds.
// It's chosen as contacts land, and kept until it lifts or a landing shows another one. In order ("farther in":
// farther inside the hand, to the side the hand setting tells, by more than kInwardMm):
// 1. A thumb has no finger farther in, but for the index crossed over it, with at least 3 contacts down. More
//    than that, it's another finger: the pinky, landed first in the zone. With 2 contacts down, the one finger
//    farther in is never taken for a crossed index, which would leave the pinky the thumb until a third landed.
// 2. Of the contacts which landed in this bottom share of the pad, the one farthest in is the thumb.
// 3. Without any, with 5 contacts down, the one farthest in is the thumb, wherever it landed.
// With 2, 3 or 4 fingers detection, a thumb is only looked for with one contact more than the smallest count on:
// without the thumb down, rule 2 took a finger in the zone for it, and the glide or the scroll lost that finger.
// Thumbs learned from where they landed went wrong with four fingers, where the pinky taught the other hand.
constexpr double kThumbZone = 1.0 / 3;
constexpr double kInwardMm = 5.0;
// A finger slipping off the pad (at its edge) comes back as a new contact, which was judged again and could be
// taken for the thumb: a contact landing within kReturnMs and kReturnMm of where one left is that finger back,
// and keeps what it was.
constexpr DWORD kReturnMs = 500;
constexpr double kReturnMm = 15.0;
// A thumb lying flat can be reported as two contacts: one landing this close to the thumb, and closer to it than
// to any finger, is part of it, above the zone too: its tip half lies there, and at half this distance it became
// a finger which steered at the glide's start. That part comes and goes as the thumb rolls: it doesn't tap, and
// isn't a contact for the 5-contact rule.
constexpr double kThumbSplitMm = 18.0;
// A later finger moving this much more than the hand glided since it landed is moving on its own, for precision:
// it joins them.
constexpr double kSoloJoinMm = 2.0;

struct TapProfile {
    bool on;
    DWORD maxMs;      // longer contacts are a finger resting, not a tap
    double maxMm;     // contacts which moved more are a finger shifting, not a tap
    DWORD minMs;      // shorter contacts are a finger flickering, not a tap
    DWORD settleMs;   // fingers landing this soon after the start are the hand settling
    DWORD flickerMs;  // a contact appearing this soon after another vanished is the same finger
};

// Two fingers landing this close together are a two-finger tap or scroll; three or more, the hand coming down.
constexpr DWORD kTogetherMs = 100;
// A tap only counts with the hand still: the glide traveling more than this from kTapStillMs before the finger
// landed until it lifts is the hand moving with a finger bouncing, which clicked while gliding.
constexpr DWORD kTapStillMs = 150;
constexpr double kTapHandMm = 1.5;
// A click goes where the pointer was before the tap began: a finger lifting off drags its contact for a few
// frames, and the tap shakes the hand, which moved the pointer off the target. The tap begins when its finger
// lands, or when a resting finger lifted up to kAimLookbackMs before that, minus kAimLiftMs of drag. Not when
// the hand glided more than kAimMaxMm since, or the pointer is farther than kAimMaxPx: that's a real move.
constexpr DWORD kAimLookbackMs = 800;
constexpr DWORD kAimLiftMs = 50;
constexpr double kAimMaxMm = 3.0;
constexpr int kAimMaxPx = 30;
constexpr ULONG kNoId = (ULONG)-1;
// Tap and hold: a finger landing within the double-click time and this close to where the last tap was holds
// the primary button once it can't be a tap anymore, until the whole hand leaves or another tap.
constexpr double kHoldNearMm = 12.0;
// A finger at the edge of the pad slips off it and back, and the touchpad reports a new contact each time, which
// tapped: contacts landing this close to the edge never tap, pair or start a hold.
constexpr double kEdgeMm = 5.0;
// The first tap of a hold is judged looser than a click: a finger which came down after the start and lifted
// within these, even if it didn't click, starts a hold when it comes back. A click needs the stricter profile, a
// tap-and-hold only needs its return.
constexpr DWORD kHoldTapMaxMs = 400;
// Right after a click, the hold presses just outside the double-click rectangle around it: pressed at the
// click's spot within the double-click time, Windows took it for the second click of a double click, and a file
// opened instead of moving. Waiting out the double-click time instead held the drag back noticeably.

// From the second tap on, each click is owed up to this long, waiting for the finger to come back. If it does,
// the owed press goes down as it lands: lifting completes the click (a triple click on the third tap), staying
// holds it (tap-tap-hold drags word by word, like Windows). If not, the click goes out when the wait ends.
constexpr DWORD kDoubleWaitMs = 200;
constexpr double kHoldTapMaxMm = 4.0;
// A finger back after a click which hasn't moved by then is the hand resting again: it won't start a hold.
constexpr DWORD kHoldStartMs = 600;

// Two-finger tap: both fingers down together, up within kTwoTapMaxMs, moving at most kTwoTapMaxMm. A right click
// follows unless the fingers come back within kTwoTapWaitMs, which starts a scroll instead.
constexpr DWORD kTwoTapMaxMs = 250;
constexpr double kTwoTapMaxMm = 3.0;
constexpr DWORD kTwoTapWaitMs = 300;
// Wheel units per finger mm while scrolling: one notch (120) every 6 mm.
constexpr double kScrollUnitsPerMm = 20.0;
// Programs scroll much farther per horizontal wheel unit than per vertical one.
constexpr double kScrollHorizontalShare = 0.05;
// Many programs round each wheel message to whole lines and dropped the small ones sent every frame while the
// fingers were down: the wheel only goes out in steps of at least this many units (120 = one notch).
constexpr double kWheelStepUnits = 40;
// Marks the mod's own input: the wheel block lets its wheel through, and other mods which skip injected input can
// tell it apart.
constexpr ULONG_PTR kInputMark = 0x52484754;
// After the fingers lift from a scroll moving faster than kInertiaMinSpeed (wheel units/s), it goes on and slows
// down with time constant kInertiaTau (s), like the Windows touchpad. Any touch stops it.
constexpr double kInertiaMinSpeed = 600.0;
constexpr double kInertiaStopSpeed = 120.0;
constexpr double kInertiaTau = 1.0;
constexpr UINT kInertiaStepMs = 16;
// Touch output of the scroll: contact px per finger mm, px between the two contacts, and under how many px of
// movement lifting them is canceled instead, since it would be a touch tap, a click.
constexpr double kTouchPxPerMm = 15.0;
constexpr int kTouchSpreadPx = 20;
constexpr double kTouchTapPx = 10.0;
// Touch hides the mouse pointer until the mouse moves, and the touch comes through after the mod's own input: the
// move which shows the pointer again waits this long, otherwise the touch hid it again right after.
constexpr UINT kTouchShowMs = 50;
// Fingers lifting drag their contacts a few mm, fast, which the program took for a swipe: after the fingers held
// still for kTouchStillMs (and at the start), movement only passes once it adds up to kTouchStartMm.
constexpr double kTouchStartMm = 2.0;
constexpr DWORD kTouchStillMs = 100;
// The two-finger scroll follows the pair of fingers closest together, at most this far apart: a resting thumb
// is alone, and stays out.
constexpr double kScrollPairMm = 40.0;

constexpr TapProfile kTapProfiles[] = {
    {false, 0, 0, 0, 0, 0},          // off
    {true, 180, 1.5, 80, 400, 100},  // low
    {true, 250, 2.0, 50, 300, 75},   // medium
    {true, 350, 3.0, 30, 200, 50},   // high
};

constexpr WCHAR kWindowClassName[] = L"Touchpad_Relaxed_Hand";

// Native curve: pointer px per finger mm, by finger speed range (mm/s, range centers).
constexpr double kSpeedBins[] = {4, 8, 16, 32, 64, 128, 256, 512};
constexpr int kBinCount = sizeof(kSpeedBins) / sizeof(kSpeedBins[0]);
constexpr int kBinTrusted = 3;            // samples before a range is used
constexpr double kDefaultPxPerMm = 10.0;  // before any range is learned
constexpr double kChunkSec = 0.1;         // window of each sample
constexpr double kChunkMinMm = 0.5;
constexpr double kSaveIntervalSec = 30;

// No frame for this long with an armed glide: the lift of the fingers was missed (an elevated window or the
// secure desktop took the touchpad), and a timer ends the glide, so that the wheel hook comes off.
constexpr DWORD kStaleFrameMs = 500;
// The wheel is only blocked while the touchpad reports: a wheel event without a recent report comes from a
// mouse, or from a glide whose end was missed.
constexpr DWORD kWheelReportMs = 250;
// Two fingers alone on the pad moving apart or together are a pinch: the pointer holds still and Windows' zoom,
// which reaches programs as Ctrl+wheel, passes the wheel block, until this long after the last pinching frame.
constexpr DWORD kPinchHoldMs = 200;
// Per frame, each pinching finger: under it, opposite moves are the fingers' jitter.
constexpr double kPinchMm = 0.2;
// Frames processed back to back would give a near-zero interval and spike the measured speed.
constexpr double kMinFrameSec = 0.005;

std::atomic<double> g_multMore{1.5};
std::atomic<DWORD> g_lastReportTick;
std::atomic<DWORD> g_pinchTick;
std::atomic<double> g_multLess{1.0};
std::atomic<bool> g_armTwo;
std::atomic<UINT> g_vizHotkeyMods;  // MOD_* flags; with g_vizHotkeyKey 0, no hotkey
std::atomic<UINT> g_vizHotkeyKey;   // virtual key
double g_padX0, g_padX1, g_padY0, g_padY1;  // the pad's bounds in mm, touchpad thread only
double g_unitMm = 1.0 / 12;                 // the pad's resolution: mm per reported unit
std::atomic<int> g_hand{1};  // 1 right hand (thumb left of the fingers), -1 left hand
std::atomic<bool> g_scrollReverse;
std::atomic<bool> g_touchScroll;
std::atomic<bool> g_armThree;
std::atomic<bool> g_armFour;
std::atomic<bool> g_armFive{true};
std::atomic<int> g_tapProfile{2};

struct Bin {
    double pxPerMm = 0;
    int samples = 0;
    int rejected = 0;  // in a row, not saved
};

// Loaded before the touchpad thread starts, saved by it every kSaveIntervalSec and by WhTool_ModUninit
// after it ends.
Bin g_curve[kBinCount];
bool g_curveDirty;
double g_lastSave;

HANDLE g_stopEvent;
HANDLE g_thread;
std::atomic<HWND> g_hWnd;  // read by WhTool_ModSettingsChanged on the engine thread
HANDLE g_hookThread;
DWORD g_hookThreadId;
constexpr UINT kWheelBlockMessage = WM_APP + 1;
constexpr UINT kHotkeySettingMessage = WM_APP + 2;  // posted to g_hWnd: the hotkey setting changed
constexpr int kVizHotkeyId = 1;
constexpr UINT_PTR kStaleTimerId = 1;
constexpr UINT_PTR kTwoTapTimerId = 2;
constexpr UINT_PTR kInertiaTimerId = 3;
constexpr UINT_PTR kDoubleTimerId = 4;
constexpr UINT_PTR kShowPointerTimerId = 5;

void SetWheelBlock(bool on);

// Touchpad and glide state, up to g_state: only used by the touchpad thread.

struct Device {
    bool valid = false;
    std::vector<BYTE> preparsed;
    std::vector<USHORT> slots;  // link collection of each finger
    bool hasContactCount = false;
    USHORT contactCountCollection = 0;
    double mmX = 0;  // mm per logical unit
    double mmY = 0;
};

struct Contact {
    ULONG id;
    double x, y;  // mm
    bool tip, conf;
};

struct Track {
    DWORD born;
    double x0, y0;
    double maxDist = 0;
    double travelLand = 0;    // g_state.travel when it landed
    double travelBefore = 0;  // hand travel in the kTapStillMs before it landed
    bool noTap = false;
    bool pairable = false;
    bool holdCandidate = false;
    bool afterClick = false;  // came back after a click, not a loose tap
    bool pressed = false;     // landed during an owed click, whose press went down with it
    ULONG partner = kNoId;    // landed together with this one, for a two-finger tap
    DWORD partnerUp = 0;      // when the partner lifted as a tap, 0 if it didn't
    bool thumb = false;       // see g_state.thumbs
};

struct TrailPoint {
    double t, x, y;  // s, mm
};

struct Step {
    double dt, dx, dy;  // s, mm: the glide's (or the scroll's) movement in one frame
    bool held;          // not sent to the pointer (or the scroll) yet
    bool fast;          // see kFastFrameUnits
};

std::deque<Step> g_scrollSteps;  // the scroll's last frames, newest last

struct GoneContact {
    char thumb;  // as in g_state.thumbs
    double x, y;  // where it left, mm
    DWORD tick;
};

std::deque<std::pair<DWORD, double>> g_travelLog;  // tick, g_state.travel then
std::deque<std::pair<DWORD, POINT>> g_pointerLog;  // tick, pointer then
std::unordered_map<HANDLE, Device> g_devices;
std::vector<Contact> g_frame;
ULONG g_frameExpected;
bool g_seenConfidence;

struct {
    bool armed;
    DWORD armTime;
    bool anchorValid;
    std::unordered_map<ULONG, std::pair<double, double>> last;  // id -> previous position, mm
    bool fewMoving;  // the lower speed multiplier applies
    double accX, accY;
    std::unordered_map<ULONG, Track> tracks;
    bool recentClick;
    DWORD lastLandTime;
    double lastTapX, lastTapY;  // mm, where the last tap's finger was
    DWORD looseTapTime;         // when a finger last lifted within the hold's looser tap, 0 if none
    double looseTapX, looseTapY;
    bool holding;
    ULONG holdId;
    std::vector<ULONG> confDropped;  // lost their Confidence with the tip still down
    std::unordered_map<ULONG, char> thumbs;  // every contact down, by id: 't' the thumb (or part of it), 'n' not
    ULONG thumbMain = kNoId;                 // the thumb's own contact, when it's split in two
    std::unordered_map<ULONG, bool> landedLow;  // every contact down, by id: whether it landed in the thumb zone
    std::unordered_map<ULONG, std::pair<double, double>> thumbsLast;  // their positions last frame, mm
    std::vector<GoneContact> gone;  // contacts which left in the last kReturnMs
    std::unordered_map<ULONG, DWORD> backAfter;  // contacts back this frame where one left: ms since it left
    bool pendingDouble;  // a click is owed, waiting for a possible hold
    DWORD restLiftTime;  // when a resting finger last lifted with the hand still on the pad
    int lastHand;  // contacts down which aren't the thumb
    DWORD countChangeTime;
    std::vector<ULONG> refIds;  // the fingers which move the pointer
    std::unordered_map<ULONG, std::deque<TrailPoint>> trails;  // every finger's last kWeightWindowSec
    std::deque<Step> steps;                                    // the glide's last frames, newest last
    double travel;       // mm the glide moved, cumulative; creeping under kTravelSpeed doesn't count
    bool pairScroll;     // two fingers came back after a two-finger tap and scroll while they stay
    bool scrollSession;  // a scroll happened in this touch: until the pad empties, no taps, two fingers scroll
    ULONG scrollA, scrollB;
    std::unordered_map<ULONG, std::pair<double, double>> scrollLast;
    double scrollTime;
    ULONG lastLandId;
    DWORD lastClickTime;
    POINT lastClickPos;
    double lastFrameTime;  // s
    double speed;          // mm/s, see StepSpeed
    bool stopped;
    DWORD lastFrameTick;
} g_state;

// A touch from the first contact down to the last one up, for the two-finger gestures. The pending tap outlives
// the touch, until a right click or a scroll settles it.
struct {
    bool inTouch;
    DWORD start;
    int maxFingers;
    int maxContacts;
    DWORD twoAt;  // when two fingers were first down, 0 if never
    double maxMove;
    std::unordered_map<ULONG, std::pair<double, double>> origin;
    bool scrolling;
    bool deciding;  // started within the wait after a two-finger tap
    bool tapPending;
    DWORD tapTime;
    double scrollAccX, scrollAccY;
    double velX, velY;  // wheel units/s, smoothed
    bool inertia;
    bool lifted;  // the scrolling fingers are up, another finger holds the scroll
    double inertiaTime;
} g_two;

// The two touch contacts of the scroll's touch output.
struct {
    bool tried;  // InitializeTouchInjection was called
    bool ready;  // and succeeded
    bool down;
    POINT origin;  // the pointer where the scroll began
    double x, y;   // contacts' offset from origin, px
    double moved;  // px
    bool passing;           // the fingers went past kTouchStartMm since they last held still
    double gateX, gateY;    // mm moved meanwhile
    DWORD lastMoveTick;
} g_touch;

// Sample under way of native one-finger movement.
struct {
    bool active;
    ULONG id;
    double t0;
    double fx0, fy0;
    POINT c0;
} g_learn;

double Now() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

double ReadMultiplier(PCWSTR name, double fallback) {
    std::wstring value = WindhawkUtils::StringSetting::make(name).get();
    std::replace(value.begin(), value.end(), L',', L'.');
    double d = wcstod(value.c_str(), nullptr);
    return d > 0 ? std::clamp(d, 0.05, 10.0) : fallback;
}

// "Win+Alt+F": modifiers and one key, joined with +, in any case. Empty or unknown parts: false.
bool ParseHotkey(PCWSTR text, UINT& mods, UINT& key) {
    mods = 0;
    key = 0;
    std::wstring all = text;
    size_t start = 0;
    while (start <= all.size()) {
        size_t end = all.find(L'+', start);
        std::wstring part = all.substr(start, end == std::wstring::npos ? std::wstring::npos : end - start);
        part.erase(0, part.find_first_not_of(L" \t"));
        part.erase(part.find_last_not_of(L" \t") + 1);
        for (auto& ch : part) {
            ch = towupper(ch);
        }
        if (part == L"CTRL" || part == L"CONTROL") {
            mods |= MOD_CONTROL;
        } else if (part == L"ALT") {
            mods |= MOD_ALT;
        } else if (part == L"SHIFT") {
            mods |= MOD_SHIFT;
        } else if (part == L"WIN" || part == L"WINDOWS") {
            mods |= MOD_WIN;
        } else if (part.size() == 1 && ((part[0] >= L'A' && part[0] <= L'Z') || (part[0] >= L'0' && part[0] <= L'9'))) {
            key = part[0];
        } else if (part.size() >= 2 && part.size() <= 3 && part[0] == L'F' && iswdigit(part[1]) &&
                   (part.size() == 2 || iswdigit(part[2])) && _wtoi(part.c_str() + 1) >= 1 &&
                   _wtoi(part.c_str() + 1) <= 24) {
            key = VK_F1 + _wtoi(part.c_str() + 1) - 1;
        } else {
            return false;
        }
        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }
    // A key alone would be taken from every program while the mod runs.
    return key && mods;
}

void LoadSettings() {
    g_multMore = ReadMultiplier(L"multiplierMore", 1.5);
    g_multLess = ReadMultiplier(L"multiplierLess", 1.0);
    g_armTwo = Wh_GetIntSetting(L"armTwo");
    UINT mods = 0;
    UINT key = 0;
    if (!ParseHotkey(WindhawkUtils::StringSetting::make(L"diagHotkey"), mods, key)) {
        mods = 0;
        key = 0;
    }
    g_vizHotkeyMods = mods;
    g_vizHotkeyKey = key;
    g_hand = wcscmp(WindhawkUtils::StringSetting::make(L"hand"), L"left") == 0 ? -1 : 1;
    g_scrollReverse = Wh_GetIntSetting(L"scrollReverse");
    g_touchScroll = wcscmp(WindhawkUtils::StringSetting::make(L"scrollOutput"), L"touch") == 0;
    g_armThree = Wh_GetIntSetting(L"armThree");
    g_armFour = Wh_GetIntSetting(L"armFour");
    g_armFive = Wh_GetIntSetting(L"armFive");

    auto tap = WindhawkUtils::StringSetting::make(L"tapToClick");
    int profile = 2;
    if (wcscmp(tap, L"off") == 0) {
        profile = 0;
    } else if (wcscmp(tap, L"low") == 0) {
        profile = 1;
    } else if (wcscmp(tap, L"high") == 0) {
        profile = 3;
    }
    g_tapProfile = profile;
}

// The learned curve.

void LogCurve() {
    WCHAR text[512];
    int length = 0;
    for (int i = 0; i < kBinCount && length >= 0; i++) {
        length += swprintf_s(text + length, ARRAYSIZE(text) - length, L" %.0f:%.2f(%d)", kSpeedBins[i],
                             g_curve[i].pxPerMm, g_curve[i].samples);
    }
    Wh_Log(L"Curve (mm/s:px/mm(samples)):%s", text);
}

void LoadCurve() {
    for (int i = 0; i < kBinCount; i++) {
        WCHAR name[16];
        swprintf_s(name, L"curve%d", i);
        g_curve[i].pxPerMm = Wh_GetIntValue(name, 0) / 1000.0;
        swprintf_s(name, L"curve%dn", i);
        g_curve[i].samples = Wh_GetIntValue(name, 0);
    }
    LogCurve();
}

void SaveCurve() {
    if (!g_curveDirty) {
        return;
    }
    for (int i = 0; i < kBinCount; i++) {
        WCHAR name[16];
        swprintf_s(name, L"curve%d", i);
        Wh_SetIntValue(name, (int)std::lround(g_curve[i].pxPerMm * 1000));
        swprintf_s(name, L"curve%dn", i);
        Wh_SetIntValue(name, g_curve[i].samples);
    }
    g_curveDirty = false;
    g_lastSave = Now();
    LogCurve();
}

int NearestBin(double speed) {
    double ls = std::log(std::max(speed, 1e-3));
    int best = 0;
    for (int i = 1; i < kBinCount; i++) {
        if (std::fabs(ls - std::log(kSpeedBins[i])) < std::fabs(ls - std::log(kSpeedBins[best]))) {
            best = i;
        }
    }
    return best;
}

void AddSample(double speed, double pxPerMm) {
    Bin& bin = g_curve[NearestBin(speed)];
    // Far above a trusted range: something else moved the pointer meanwhile (a mouse, a program). Several in a
    // row mean the range itself is off (the touchpad speed was raised, or it sank to 0): it's learned again.
    if (bin.samples >= kBinTrusted && pxPerMm > bin.pxPerMm * 3) {
        if (++bin.rejected < 5) {
            return;
        }
        bin.samples = 0;
    }
    bin.rejected = 0;
    // A plain average for the first samples, then a moving one, to follow setting changes.
    double alpha = std::max(0.05, 1.0 / (bin.samples + 1));
    bin.pxPerMm = bin.samples ? bin.pxPerMm + (pxPerMm - bin.pxPerMm) * alpha : pxPerMm;
    bin.samples = std::min(bin.samples + 1, 1000);
    g_curveDirty = true;
}

// Interpolates in log speed between the neighboring trusted ranges; outside them, the nearest one. A range is
// never above a faster one: the slowest, fed by few samples, came out above the next and raised the gain of a
// stopping hand.
double TrustedBin(int i) {
    double v = g_curve[i].pxPerMm;
    for (int j = i + 1; j < kBinCount; j++) {
        if (g_curve[j].samples >= kBinTrusted) {
            v = std::min(v, g_curve[j].pxPerMm);
        }
    }
    return v;
}

double CurveAt(double speed) {
    double ls = std::log(std::max(speed, 1e-3));
    int lo = -1;
    int hi = -1;
    for (int i = 0; i < kBinCount; i++) {
        if (g_curve[i].samples < kBinTrusted) {
            continue;
        }
        if (std::log(kSpeedBins[i]) <= ls) {
            lo = i;
        } else if (hi < 0) {
            hi = i;
        }
    }
    if (lo < 0 && hi < 0) {
        return kDefaultPxPerMm;
    }
    if (lo < 0) {
        return TrustedBin(hi);
    }
    if (hi < 0) {
        return TrustedBin(lo);
    }
    double a = std::log(kSpeedBins[lo]);
    double b = std::log(kSpeedBins[hi]);
    double t = (ls - a) / (b - a);
    return TrustedBin(lo) + (TrustedBin(hi) - TrustedBin(lo)) * t;
}

// The pointer stops at the edge while the finger goes on: samples there would underestimate the curve.
bool NearMonitorEdge(POINT p) {
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST), &mi)) {
        return true;
    }
    const RECT& r = mi.rcMonitor;
    return p.x - r.left < 3 || r.right - 1 - p.x < 3 || p.y - r.top < 3 || r.bottom - 1 - p.y < 3;
}

// A hidden or clipped pointer (games, 3D viewports, slider drags) is moved back or held, and doesn't show
// Windows' movement.
bool PointerCaptured() {
    CURSORINFO ci{.cbSize = sizeof(ci)};
    if (GetCursorInfo(&ci) && !(ci.flags & CURSOR_SHOWING)) {
        return true;
    }
    RECT clip;
    if (!GetClipCursor(&clip)) {
        return false;
    }
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    return clip.left > x || clip.top > y || clip.right < x + GetSystemMetrics(SM_CXVIRTUALSCREEN) ||
           clip.bottom < y + GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

// Outside a glide, with one finger, Windows moves the pointer: compares the finger's displacement with the
// pointer's over kChunkSec windows.
void Learn(const std::vector<const Contact*>& fingers, int touching, double t) {
    if (g_state.armed || touching != 1 || fingers.size() != 1) {
        g_learn.active = false;
        return;
    }

    const Contact* c = fingers[0];
    POINT cursor;
    if (!GetCursorPos(&cursor) || PointerCaptured()) {
        g_learn.active = false;
        return;
    }

    if (g_learn.active && g_learn.id == c->id) {
        double dt = t - g_learn.t0;
        if (dt < kChunkSec) {
            return;
        }
        double mm = std::hypot(c->x - g_learn.fx0, c->y - g_learn.fy0);
        double px = std::hypot((double)(cursor.x - g_learn.c0.x), (double)(cursor.y - g_learn.c0.y));
        // Samples where Windows didn't move the pointer (px = 0) are learned too: skipping them pushed the slow
        // end of the curve up, and the glide slid on after the hand stopped.
        if (mm >= kChunkMinMm && !NearMonitorEdge(g_learn.c0) && !NearMonitorEdge(cursor)) {
            AddSample(mm / dt, px / mm);
        }
    }

    g_learn.active = true;
    g_learn.id = c->id;
    g_learn.t0 = t;
    g_learn.fx0 = c->x;
    g_learn.fy0 = c->y;
    g_learn.c0 = cursor;
}

void SendPrimary(bool down);

void FirePendingDouble(DWORD now);

void TouchEnd();

void EndGlide() {
    FirePendingDouble(GetTickCount());
    if (g_state.holding) {
        g_state.holding = false;
        SendPrimary(false);
    }
    g_state.pairScroll = false;
    g_state.scrollSession = false;
    TouchEnd();
    g_travelLog.clear();
    g_pointerLog.clear();
    g_state.restLiftTime = 0;
    g_state.refIds.clear();
    g_state.trails.clear();
    g_state.steps.clear();
    g_state.confDropped.clear();
    if (g_state.armed) {
        Wh_Log(L"Glide stopped");
        SetWheelBlock(false);
        KillTimer(g_hWnd, kStaleTimerId);
    }
    g_state.armed = false;
    g_state.anchorValid = false;
    g_state.speed = 0;
    g_state.stopped = true;
    g_state.tracks.clear();
    g_learn.active = false;
    if (g_curveDirty && Now() - g_lastSave >= kSaveIntervalSec) {
        SaveCurve();
    }
}

void ResetSession() {
    EndGlide();
    g_frame.clear();
    g_frameExpected = 0;
}

// Output.

// SendInput's button flags name the physical buttons: with the buttons swapped, the primary one is the right.
void SendPrimaryClick() {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    INPUT input[2]{};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
    input[0].mi.dwExtraInfo = kInputMark;
    input[1].mi.dwExtraInfo = kInputMark;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
}

void SendPrimary(bool down) {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = down ? (swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN)
                            : (swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP);
    input.mi.dwExtraInfo = kInputMark;
    SendInput(1, &input, sizeof(input));
}

// An absolute move as input: unlike SetCursorPos, programs reading mouse input see it, and a drag with the
// button held follows it. Absolute, it skips the mouse acceleration like SetCursorPos.
void SendCursorPos(int x, int y) {
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0) {
        SetCursorPos(x, y);
        return;
    }
    // Windows maps an absolute coordinate n to the pixel n * width / 65536, rounded down: rounding up here lands
    // on the exact pixel, so one-pixel steps aren't lost.
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dx = (LONG)(((LONGLONG)(x - vx) * 65536 + vw - 1) / vw);
    input.mi.dy = (LONG)(((LONGLONG)(y - vy) * 65536 + vh - 1) / vh);
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    input.mi.dwExtraInfo = kInputMark;
    SendInput(1, &input, sizeof(input));
}

void SendSecondaryClick() {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    INPUT input[2]{};
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = swapped ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = swapped ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
    input[0].mi.dwExtraInfo = kInputMark;
    input[1].mi.dwExtraInfo = kInputMark;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
    Wh_Log(L"Right click");
}

void SendWheel(DWORD flag, LONG amount) {
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = flag;
    input.mi.mouseData = (DWORD)amount;
    input.mi.dwExtraInfo = kInputMark;
    SendInput(1, &input, sizeof(input));
}

// Sends what's accumulated, in small deltas: a smooth scroll that follows the fingers frame by frame.
void FlushScroll() {
    LONG wy = std::fabs(g_two.scrollAccY) >= kWheelStepUnits ? (LONG)g_two.scrollAccY : 0;
    // Horizontal units go farther each, by kScrollHorizontalShare: the step shrinks with them, so that it comes
    // every bit of finger travel as the vertical one does, not every 40 mm.
    LONG wx = std::fabs(g_two.scrollAccX) >= kWheelStepUnits * kScrollHorizontalShare ? (LONG)g_two.scrollAccX : 0;
    g_two.scrollAccY -= wy;
    g_two.scrollAccX -= wx;
    if (wy) {
        SendWheel(MOUSEEVENTF_WHEEL, wy);
    }
    if (wx) {
        SendWheel(MOUSEEVENTF_HWHEEL, wx);
    }
}

// Touch output: the scroll as two touch contacts dragged together, which programs with touch support scroll
// smoothly and with their own inertia, like the native touchpad scroll.

bool InjectPan(POINTER_FLAGS flags) {
    POINTER_TOUCH_INFO contacts[2]{};
    for (int i = 0; i < 2; i++) {
        POINTER_INFO& p = contacts[i].pointerInfo;
        p.pointerType = PT_TOUCH;
        p.pointerId = i;
        p.pointerFlags = flags;
        p.ptPixelLocation.x =
            g_touch.origin.x + (LONG)std::lround(g_touch.x) + (i ? kTouchSpreadPx / 2 : -kTouchSpreadPx / 2);
        p.ptPixelLocation.y = g_touch.origin.y + (LONG)std::lround(g_touch.y);
    }
    return InjectTouchInput(ARRAYSIZE(contacts), contacts);
}

// Both contacts stay on the monitor where the scroll began: off the screen the injection fails.
bool TouchFits(double x, double y) {
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromPoint(g_touch.origin, MONITOR_DEFAULTTONEAREST), &mi)) {
        return false;
    }
    const RECT& r = mi.rcMonitor;
    LONG px = g_touch.origin.x + (LONG)std::lround(x);
    LONG py = g_touch.origin.y + (LONG)std::lround(y);
    return px - kTouchSpreadPx / 2 - r.left > 2 && r.right - 1 - (px + kTouchSpreadPx / 2) > 2 && py - r.top > 2 &&
           r.bottom - 1 - py > 2;
}

// at: where the contacts come down, nullptr for the pointer.
bool TouchBegin(const POINT* at) {
    if (!g_touch.tried) {
        g_touch.tried = true;
        g_touch.ready = InitializeTouchInjection(2, TOUCH_FEEDBACK_NONE);
        if (!g_touch.ready) {
            Wh_Log(L"InitializeTouchInjection error: %u", GetLastError());
        }
    }
    if (!g_touch.ready) {
        return false;
    }
    if (at) {
        g_touch.origin = *at;
    } else if (!GetCursorPos(&g_touch.origin)) {
        return false;
    }
    g_touch.x = 0;
    g_touch.y = 0;
    g_touch.moved = 0;
    if (!TouchFits(0, 0) || !InjectPan(POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT)) {
        Wh_Log(L"Touch scroll can't start: %u", GetLastError());
        return false;
    }
    g_touch.down = true;
    return true;
}

// Touch takes the mouse pointer to the contacts: it goes back where the scroll began, shown again.
void ShowPointer() {
    KillTimer(g_hWnd, kShowPointerTimerId);
    if (!g_touch.down) {
        SendCursorPos(g_touch.origin.x + 1, g_touch.origin.y);
        SendCursorPos(g_touch.origin.x, g_touch.origin.y);
    }
}

void TouchEnd() {
    if (!g_touch.down) {
        return;
    }
    g_touch.down = false;
    g_touch.passing = false;
    g_touch.gateX = 0;
    g_touch.gateY = 0;
    InjectPan(g_touch.moved >= kTouchTapPx ? POINTER_FLAG_UP : POINTER_FLAG_UP | POINTER_FLAG_CANCELED);
    SetTimer(g_hWnd, kShowPointerTimerId, kTouchShowMs, nullptr);
}

void TouchPan(double dxMm, double dyMm) {
    double sign = g_scrollReverse ? -1 : 1;
    double ddx = dxMm * kTouchPxPerMm * sign;
    double ddy = dyMm * kTouchPxPerMm * sign;
    // At the edge of the screen the contacts lift and come down again where the scroll began.
    if (!TouchFits(g_touch.x + ddx, g_touch.y + ddy)) {
        POINT origin = g_touch.origin;
        TouchEnd();
        if (!TouchBegin(&origin)) {
            return;
        }
        g_touch.passing = true;
    }
    g_touch.x += ddx;
    g_touch.y += ddy;
    g_touch.moved += std::hypot(ddx, ddy);
    // Sent even without movement: contacts left without input are canceled by the system.
    if (!InjectPan(POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT)) {
        Wh_Log(L"InjectTouchInput error: %u", GetLastError());
        g_touch.down = false;
    }
}

// Wheel units follow the fingers, without acceleration. Off, the page follows the fingers: down scrolls up.
void Scroll(double dxMm, double dyMm, double dt) {
    g_scrollSteps.push_back({std::max(dt, kMinFrameSec), dxMm, dyMm, true, false});
    if (g_scrollSteps.size() > 32) {
        g_scrollSteps.pop_front();
    }
    double sx = 0;
    double sy = 0;
    double span = 0;
    for (auto it = g_scrollSteps.rbegin(); it != g_scrollSteps.rend() && span < kSpeedWindowSec; ++it) {
        sx += it->dx;
        sy += it->dy;
        span += it->dt;
    }
    span = std::max(span, kSpeedMinSpanSec);
    // See kScrollStillMm. Moving, the frames held back go out together, so that a slow scroll loses nothing.
    bool still = std::hypot(sx, sy) < kScrollStillMm;
    dxMm = 0;
    dyMm = 0;
    if (!still) {
        double held = 0;
        for (auto it = g_scrollSteps.rbegin(); it != g_scrollSteps.rend() && held < kSpeedWindowSec; ++it) {
            held += it->dt;
            if (it->held) {
                it->held = false;
                dxMm += it->dx;
                dyMm += it->dy;
            }
        }
    }
    // Mostly along one axis: the other is the fingers' drift, not a scroll.
    if (std::fabs(sy) > 2 * std::fabs(sx)) {
        dxMm = 0;
        sx = 0;
    } else if (std::fabs(sx) > 2 * std::fabs(sy)) {
        dyMm = 0;
        sy = 0;
    }
    if (g_touchScroll) {
        DWORD now = GetTickCount();
        if (dxMm || dyMm) {
            g_touch.lastMoveTick = now;
        } else if (now - g_touch.lastMoveTick >= kTouchStillMs) {
            g_touch.passing = false;
            g_touch.gateX = 0;
            g_touch.gateY = 0;
        }
        if (!g_touch.passing) {
            g_touch.gateX += dxMm;
            g_touch.gateY += dyMm;
            if (std::hypot(g_touch.gateX, g_touch.gateY) < kTouchStartMm) {
                if (g_touch.down) {
                    TouchPan(0, 0);
                }
                return;
            }
            dxMm = g_touch.gateX;
            dyMm = g_touch.gateY;
            g_touch.passing = true;
        }
        if (g_touch.down || TouchBegin(nullptr)) {
            TouchPan(dxMm, dyMm);
            return;
        }
    }
    double sign = g_scrollReverse ? -1 : 1;
    double unitsX = -kScrollUnitsPerMm * kScrollHorizontalShare * sign;
    double unitsY = kScrollUnitsPerMm * sign;
    double ux = dxMm * unitsX;
    double uy = dyMm * unitsY;
    // The velocity, for the inertia, is the window's, and it's followed even while the fingers hold still, so that
    // a stop before the lift leaves no inertia.
    double k = 0.5;
    g_two.velX += ((still ? 0 : sx / span * unitsX) - g_two.velX) * k;
    g_two.velY += ((still ? 0 : sy / span * unitsY) - g_two.velY) * k;
    if (!ux && !uy) {
        return;
    }
    g_two.scrollAccX += ux;
    g_two.scrollAccY += uy;
    FlushScroll();
}

void StopInertia() {
    if (g_two.inertia) {
        g_two.inertia = false;
        KillTimer(g_hWnd, kInertiaTimerId);
    }
}

void InertiaStep() {
    double t = Now();
    double dt = t - g_two.inertiaTime;
    g_two.inertiaTime = t;
    double decay = std::exp(-dt / kInertiaTau);
    g_two.velX *= decay;
    g_two.velY *= decay;
    if (std::hypot(g_two.velX, g_two.velY) < kInertiaStopSpeed) {
        StopInertia();
        return;
    }
    g_two.scrollAccX += g_two.velX * dt;
    g_two.scrollAccY += g_two.velY * dt;
    FlushScroll();
}

// With touch output, lifting the contacts leaves the inertia to the program.
void StartInertia() {
    if (g_touch.down) {
        TouchEnd();
        return;
    }
    if (!g_two.inertia && std::hypot(g_two.velX, g_two.velY) >= kInertiaMinSpeed) {
        g_two.inertia = true;
        g_two.inertiaTime = Now();
        SetTimer(g_hWnd, kInertiaTimerId, kInertiaStepMs, nullptr);
        return;
    }
    // Without inertia, what's below a step goes out now: zeroed at the next touch, a slow scroll fell short.
    LONG wy = std::lround(g_two.scrollAccY);
    LONG wx = std::lround(g_two.scrollAccX);
    g_two.scrollAccY = 0;
    g_two.scrollAccX = 0;
    if (wy) {
        SendWheel(MOUSEEVENTF_WHEEL, wy);
    }
    if (wx) {
        SendWheel(MOUSEEVENTF_HWHEEL, wx);
    }
}

void SettleTwoTap() {
    if (g_two.tapPending) {
        g_two.tapPending = false;
        KillTimer(g_hWnd, kTwoTapTimerId);
        SendSecondaryClick();
    }
}

// Two-finger gestures, tracked over a whole touch: a tap ends as the last finger lifts, before the glide does.
void TrackTwoFingers(const std::vector<const Contact*>& fingers, int touching, DWORD now) {
    if (!g_armTwo) {
        g_two.inTouch = false;
        g_two.scrolling = false;
        return;
    }

    if (touching == 0) {
        if (g_two.inTouch && g_two.scrolling) {
            StartInertia();
        }
        if (g_two.inTouch && !g_two.scrolling && !g_state.scrollSession && g_two.maxFingers == 2 && g_two.maxContacts == 2 && g_two.twoAt &&
            g_two.twoAt - g_two.start <= kTogetherMs && now - g_two.start <= kTwoTapMaxMs &&
            g_two.maxMove <= kTwoTapMaxMm) {
            g_two.tapPending = true;
            g_two.tapTime = now;
            SetTimer(g_hWnd, kTwoTapTimerId, kTwoTapWaitMs, nullptr);
        }
        g_two.inTouch = false;
        g_two.scrolling = false;
        return;
    }

    if (!g_two.inTouch) {
        g_two.inTouch = true;
        g_two.start = now;
        g_two.maxFingers = 0;
        g_two.maxContacts = 0;
        g_two.twoAt = 0;
        g_two.maxMove = 0;
        g_two.origin.clear();
        StopInertia();
        g_two.scrollAccX = 0;
        g_two.scrollAccY = 0;
        g_two.velX = 0;
        g_two.velY = 0;
        g_scrollSteps.clear();
        g_two.lifted = false;
        g_two.deciding = g_two.tapPending && now - g_two.tapTime <= kTwoTapWaitMs;
    }

    g_two.maxFingers = std::max(g_two.maxFingers, (int)fingers.size());
    g_two.maxContacts = std::max(g_two.maxContacts, touching);
    if (fingers.size() >= 2 && !g_two.twoAt) {
        g_two.twoAt = now;
    }
    for (const Contact* c : fingers) {
        auto it = g_two.origin.find(c->id);
        if (it == g_two.origin.end()) {
            g_two.origin[c->id] = {c->x, c->y};
        } else {
            g_two.maxMove = std::max(g_two.maxMove, std::hypot(c->x - it->second.first, c->y - it->second.second));
        }
    }

    // Back within the wait after a tap: two fingers make it a scroll, anything else lets the right click go.
    if (g_two.deciding) {
        if (fingers.size() >= 2 && now - g_two.start <= kTogetherMs) {
            g_two.deciding = false;
            g_two.tapPending = false;
            KillTimer(g_hWnd, kTwoTapTimerId);
            g_two.scrolling = true;
            g_state.scrollSession = true;
            Wh_Log(L"Scroll started");
        } else if (now - g_two.start > kTogetherMs) {
            g_two.deciding = false;
            SettleTwoTap();
        }
    }

    // A finger left down (the thumb) holds the scroll: the scrolling fingers lifting coast like a lift of the
    // whole hand, and coming back down scroll again without a new tap.
    if (g_two.scrolling) {
        if (fingers.size() <= 1 && !g_two.lifted) {
            g_two.lifted = true;
            StartInertia();
        } else if (fingers.size() >= 2 && g_two.lifted) {
            g_two.lifted = false;
            StopInertia();
            g_two.velX = 0;
            g_two.velY = 0;
            g_scrollSteps.clear();
            g_two.scrollAccX = 0;
            g_two.scrollAccY = 0;
        }
    }
}

// The hand's speed, for the stop, the curve and the taps: see kSpeedWindowSec.
double StepSpeed(double dx, double dy, double dt) {
    dt = std::max(dt, kMinFrameSec);
    double d = std::hypot(dx, dy);
    bool fast = d >= kFastFrameUnits * g_unitMm;
    g_state.steps.push_back({dt, dx, dy, true, fast});
    if (g_state.steps.size() > 32) {
        g_state.steps.pop_front();
    }
    if (fast) {
        return d / dt;
    }
    double sx = 0;
    double sy = 0;
    double span = 0;
    for (auto it = g_state.steps.rbegin(); it != g_state.steps.rend() && !it->fast && span < kSpeedWindowSec; ++it) {
        sx += it->dx;
        sy += it->dy;
        span += it->dt;
    }
    return std::hypot(sx, sy) / std::max(span, kSpeedMinSpanSec);
}

void Move(double multiplier) {
    if (g_state.stopped ? g_state.speed < kStartSpeed : g_state.speed < kStopSpeed) {
        g_state.stopped = true;
        return;
    }
    g_state.stopped = false;

    // The frames held back while stopped go out with the start, so that a slow start loses nothing.
    double dxMm = 0;
    double dyMm = 0;
    double span = 0;
    for (auto it = g_state.steps.rbegin(); it != g_state.steps.rend() && span < kSpeedWindowSec; ++it) {
        span += it->dt;
        if (it->held) {
            it->held = false;
            dxMm += it->dx;
            dyMm += it->dy;
        }
    }

    double pxPerMm = CurveAt(g_state.speed) * multiplier;
    g_state.accX += dxMm * pxPerMm;
    g_state.accY += dyMm * pxPerMm;
    LONG mx = (LONG)g_state.accX;
    LONG my = (LONG)g_state.accY;
    g_state.accX -= mx;
    g_state.accY -= my;
    if (!mx && !my) {
        return;
    }

    POINT pos;
    if (GetCursorPos(&pos)) {
        SendCursorPos(pos.x + mx, pos.y + my);
    }
}

bool InDoubleClickTime(DWORD now) {
    return g_state.recentClick && now - g_state.lastClickTime <= GetDoubleClickTime();
}

// The tap shakes the resting hand and the glide moves the pointer past the Windows double click
// tolerance; the second tap goes back to the first one's spot when it's near it.
void SnapToLastClick() {
    POINT pos;
    GetCursorPos(&pos);
    int tolX = GetSystemMetrics(SM_CXDOUBLECLK) * 4;
    int tolY = GetSystemMetrics(SM_CYDOUBLECLK) * 4;
    if (std::abs(pos.x - g_state.lastClickPos.x) <= tolX && std::abs(pos.y - g_state.lastClickPos.y) <= tolY) {
        SetCursorPos(g_state.lastClickPos.x, g_state.lastClickPos.y);
    }
}

// Clicks within the double-click time chain at the first one's spot, so that the program counts them: two
// select a word, three a paragraph.
void Click(DWORD now) {
    bool chained = InDoubleClickTime(now);
    if (chained) {
        SnapToLastClick();
    } else {
        GetCursorPos(&g_state.lastClickPos);
        g_state.recentClick = true;
    }
    g_state.lastClickTime = now;
    SendPrimaryClick();
    Wh_Log(L"%s", chained ? L"Repeated click" : L"Click");
}

// The glide.

// Hand travel since tick.
double TravelSince(DWORD tick) {
    double at = g_travelLog.empty() ? g_state.travel : g_travelLog.front().second;
    for (const auto& e : g_travelLog) {
        if ((LONG)(e.first - tick) > 0) {
            break;
        }
        at = e.second;
    }
    return g_state.travel - at;
}

void AimTap(const Track& t) {
    DWORD start = t.born;
    if (g_state.restLiftTime && (LONG)(t.born - g_state.restLiftTime) >= 0 &&
        t.born - g_state.restLiftTime <= kAimLookbackMs) {
        start = g_state.restLiftTime;
    }
    start -= kAimLiftMs;
    if (TravelSince(start) > kAimMaxMm) {
        return;
    }
    const POINT* at = nullptr;
    for (const auto& e : g_pointerLog) {
        if ((LONG)(e.first - start) > 0) {
            break;
        }
        at = &e.second;
    }
    POINT pos;
    if (!at || !GetCursorPos(&pos) || (pos.x == at->x && pos.y == at->y)) {
        return;
    }
    if (std::abs(pos.x - at->x) <= kAimMaxPx && std::abs(pos.y - at->y) <= kAimMaxPx) {
        SendCursorPos(at->x, at->y);
    }
}

bool IsThumb(ULONG id);
bool IsThumbPart(ULONG id);

bool NearPadEdge(double x, double y) {
    return g_padX1 > g_padX0 && (x - g_padX0 < kEdgeMm || g_padX1 - x < kEdgeMm || y - g_padY0 < kEdgeMm ||
                                 g_padY1 - y < kEdgeMm);
}

bool HandStill(const Track& t) {
    return t.travelBefore + (g_state.travel - t.travelLand) <= kTapHandMm;
}

bool IsTapCandidate(const Track& t, DWORD now) {
    const TapProfile& tap = kTapProfiles[g_tapProfile];
    return tap.on && g_state.armed && (LONG)(t.born - g_state.armTime) > 0 && now - t.born <= tap.maxMs;
}

// touching > 0: the tap ended with the hand still resting, it's not the whole hand leaving.
bool IsTap(const Track& t, DWORD now, int touching) {
    const TapProfile& tap = kTapProfiles[g_tapProfile];
    return touching > 0 && !t.noTap && !t.thumb && IsTapCandidate(t, now) && now - t.born >= tap.minMs &&
           t.maxDist <= tap.maxMm && HandStill(t);
}

void FirePendingDouble(DWORD now) {
    if (g_state.pendingDouble) {
        g_state.pendingDouble = false;
        KillTimer(g_hWnd, kDoubleTimerId);
        Click(now);
    }
}

// Owes this tap's click while the finger may come back, if the double-click time leaves room for the wait.
bool OweClick(DWORD now) {
    if (!InDoubleClickTime(now)) {
        return false;
    }
    DWORD left = GetDoubleClickTime() - (now - g_state.lastClickTime);
    if (left <= 60) {
        return false;
    }
    g_state.pendingDouble = true;
    SetTimer(g_hWnd, kDoubleTimerId, std::min(kDoubleWaitMs, left - 30), nullptr);
    return true;
}

// A tap during a hold is its button's release, as on the touchpad alone.
void ReleaseHold() {
    g_state.holding = false;
    g_state.holdId = kNoId;
    g_state.recentClick = false;
    SendPrimary(false);
}

void StartPairScroll(ULONG a, ULONG b) {
    g_state.pairScroll = true;
    g_state.scrollSession = true;
    g_state.scrollA = a;
    g_state.scrollB = b;
    g_state.scrollLast.clear();
    g_state.scrollTime = Now();
    StopInertia();
    g_two.velX = 0;
    g_two.velY = 0;
    g_scrollSteps.clear();
    g_two.scrollAccX = 0;
    g_two.scrollAccY = 0;
    Wh_Log(L"Scroll started");
}

void TwoFingerTap(DWORD now) {
    FirePendingDouble(now);
    if (g_state.holding) {
        ReleaseHold();
        return;
    }
    g_two.tapPending = true;
    g_two.tapTime = now;
    g_two.deciding = false;
    SetTimer(g_hWnd, kTwoTapTimerId, kTwoTapWaitMs, nullptr);
}

// Windows doesn't recognize taps with other fingers resting; here, while gliding, they work as on the touchpad
// alone: one finger taps (any number of times) and taps and holds, two fingers right click and tap-hold to
// scroll. The fingers which started the glide never tap, and nothing taps while the hand moves.
void UpdateTracks(const std::vector<const Contact*>& fingers, const std::vector<const Contact*>& tips, DWORD now,
                  int touching) {
    const TapProfile& tap = kTapProfiles[g_tapProfile];

    g_state.confDropped.erase(
        std::remove_if(g_state.confDropped.begin(), g_state.confDropped.end(),
                       [&](ULONG id) {
                           return std::none_of(tips.begin(), tips.end(),
                                               [id](const Contact* c) { return c->id == id; });
                       }),
        g_state.confDropped.end());

    // Losses first, so that a contact which vanishes and comes back under a new id in the same frame is
    // seen as a flicker.
    for (auto it = g_state.tracks.begin(); it != g_state.tracks.end();) {
        ULONG id = it->first;
        if (std::any_of(fingers.begin(), fingers.end(), [id](const Contact* c) { return c->id == id; })) {
            ++it;
            continue;
        }
        Track t = it->second;
        it = g_state.tracks.erase(it);
        // The thumb is dead to taps: it lifting sends no owed click early and isn't a resting finger rising.
        if (t.thumb) {
            continue;
        }
        if (g_state.pairScroll && (id == g_state.scrollA || id == g_state.scrollB)) {
            g_state.pairScroll = false;
            StartInertia();
            continue;
        }
        // The button stays down until the whole hand leaves: the finger which started the hold can lift while
        // others keep dragging.
        if (g_state.holding && id == g_state.holdId) {
            g_state.holdId = kNoId;
            continue;
        }
        // The touchpad dropped its Confidence (a resting thumb taken for a palm) with the tip still down: not a lift,
        // and its return isn't a landing.
        if (std::any_of(tips.begin(), tips.end(), [id](const Contact* c) { return c->id == id; })) {
            g_state.confDropped.push_back(id);
            Wh_Log(L"Confidence drop %u", id);
            continue;
        }
        bool isTap = IsTap(t, now, touching);
        if (t.partner != kNoId) {
            auto p = g_state.tracks.find(t.partner);
            if (p != g_state.tracks.end()) {
                p->second.partnerUp = isTap ? now : 0;
            } else if (isTap && t.partnerUp && now - t.partnerUp <= kTogetherMs) {
                AimTap(t);
                TwoFingerTap(now);
            } else {
                FirePendingDouble(now);
            }
            continue;
        }
        bool released = false;
        if (t.pressed) {
            SendPrimary(false);
            if (isTap && !OweClick(now)) {
                Click(now);
            }
        } else if (isTap && g_state.holding) {
            ReleaseHold();
            released = true;
        } else if (isTap) {
            FirePendingDouble(now);
            if (!(t.afterClick && OweClick(now))) {
                g_state.lastTapX = t.x0;
                g_state.lastTapY = t.y0;
                AimTap(t);
                Click(now);
            }
        } else {
            FirePendingDouble(now);
            // The thumb leaving isn't the tapping finger rising: the click's aim doesn't go back to it.
            if (touching > 0 && !t.thumb) {
                g_state.restLiftTime = now;
            }
        }
        if (!released && !t.pressed && !t.noTap && touching > 0 && tap.on && g_state.armed &&
            (LONG)(t.born - g_state.armTime) >= (LONG)tap.settleMs && now - t.born >= tap.minMs &&
            now - t.born <= kHoldTapMaxMs && t.maxDist <= kHoldTapMaxMm && HandStill(t)) {
            g_state.looseTapTime = now;
            g_state.looseTapX = t.x0;
            g_state.looseTapY = t.y0;
        }
    }

    for (const Contact* c : fingers) {
        auto it = g_state.tracks.find(c->id);
        if (it == g_state.tracks.end()) {
            auto dropped = std::find(g_state.confDropped.begin(), g_state.confDropped.end(), c->id);
            if (dropped != g_state.confDropped.end()) {
                g_state.confDropped.erase(dropped);
                Track t{now, c->x, c->y};
                t.travelLand = g_state.travel;
                t.noTap = true;
                g_state.tracks[c->id] = t;
                continue;
            }
            // The thumb is dead to taps: it landing neither taps, holds, pairs nor stops a scroll's inertia.
            if (IsThumb(c->id)) {
                Track t{now, c->x, c->y};
                t.travelLand = g_state.travel;
                t.noTap = true;
                t.thumb = true;
                g_state.tracks[c->id] = t;
                continue;
            }
            StopInertia();
            Track t{now, c->x, c->y};
            t.travelLand = g_state.travel;
            t.travelBefore = TravelSince(now - kTapStillMs);
            bool still = t.travelBefore <= kTapHandMm;
            bool settled = now - g_state.armTime >= tap.settleMs;
            bool edge = NearPadEdge(c->x, c->y);
            // A flicker is a contact back where one just left. Any contact leaving used to count, anywhere on
            // the pad: a thumb slipping off the edge or losing its Confidence made the fingers' taps fail.
            auto back = g_state.backAfter.find(c->id);
            bool flicker = back != g_state.backAfter.end() && back->second <= tap.flickerMs;
            t.thumb = IsThumb(c->id);
            // The thumb, a flicker and a contact at the edge land on their own: they neither pair with a finger
            // landing at the same time nor cancel its tap, as the hand coming down does.
            bool apart = t.thumb || flicker || edge;
            t.noTap = !tap.on || !still || !settled || flicker || g_state.scrollSession || edge || IsThumbPart(c->id);
            t.pairable = tap.on && g_state.armed && still && settled && !g_state.pairScroll && !apart;

            bool paired = false;
            auto other = g_state.tracks.find(g_state.lastLandId);
            if (!apart && other != g_state.tracks.end() && now - g_state.lastLandTime <= kTogetherMs) {
                Track& o = other->second;
                if (t.pairable && o.pairable && o.partner == kNoId && !o.pressed) {
                    o.holdCandidate = false;
                    if (g_state.scrollSession || (g_two.tapPending && now - g_two.tapTime <= kTwoTapWaitMs)) {
                        // Two fingers back right after a two-finger tap, or anytime within a scroll: a scroll,
                        // while both stay.
                        g_two.tapPending = false;
                        KillTimer(g_hWnd, kTwoTapTimerId);
                        if (!g_two.scrolling) {
                            StartPairScroll(other->first, c->id);
                        }
                        t.noTap = true;
                        o.noTap = true;
                    } else {
                        t.partner = other->first;
                        o.partner = c->id;
                        t.noTap = false;
                        o.noTap = false;
                    }
                    paired = true;
                } else {
                    t.noTap = true;
                    t.pairable = false;
                    o.noTap = true;
                    o.pairable = false;
                    if (!o.pressed) {
                        o.holdCandidate = false;
                    }
                    auto op = g_state.tracks.find(o.partner);
                    if (op != g_state.tracks.end()) {
                        op->second.noTap = true;
                    }
                }
            }

            // The tap finger coming back is no flicker nor hand settling: it's another tap, or a hold.
            if (!paired && !edge && !g_state.holding && !g_state.scrollSession && tap.on && g_state.armed && still &&
                !flicker) {
                double fromTap = std::hypot(c->x - g_state.lastTapX, c->y - g_state.lastTapY);
                if (g_state.pendingDouble && fromTap <= kHoldNearMm) {
                    // Back during an owed click: its press goes down now. Lifting completes it, staying holds it.
                    g_state.pendingDouble = false;
                    KillTimer(g_hWnd, kDoubleTimerId);
                    SnapToLastClick();
                    SendPrimary(true);
                    g_state.lastClickTime = now;
                    g_state.recentClick = true;
                    g_state.looseTapTime = 0;
                    t.pressed = true;
                    t.holdCandidate = true;
                    t.afterClick = true;
                    t.noTap = false;
                } else if (InDoubleClickTime(now) && fromTap <= kHoldNearMm) {
                    t.holdCandidate = true;
                    t.afterClick = true;
                    t.noTap = false;
                } else if (g_state.looseTapTime && now - g_state.looseTapTime <= GetDoubleClickTime() &&
                           std::hypot(c->x - g_state.looseTapX, c->y - g_state.looseTapY) <= kHoldNearMm) {
                    t.holdCandidate = true;
                }
            }
            if (!apart) {
                g_state.lastLandTime = now;
                g_state.lastLandId = c->id;
            }
            g_state.tracks[c->id] = t;
        } else {
            Track& t = it->second;
            t.thumb = IsThumb(c->id);
            t.maxDist = std::max(t.maxDist, std::hypot(c->x - t.x0, c->y - t.y0));
            // No longer a tap (it moved, or the hand glided; a pressed one also by staying): the button goes down,
            // or stays down if it already went, until another tap or the whole hand leaves.
            bool moved = t.maxDist > tap.maxMm || g_state.travel - t.travelLand > kTapHandMm;
            if (t.holdCandidate && !t.pressed && !moved && now - t.born > kHoldStartMs) {
                t.holdCandidate = false;
            }
            if (t.holdCandidate && !g_state.holding && (moved || (t.pressed && now - t.born > tap.maxMs))) {
                t.holdCandidate = false;
                Wh_Log(L"Hold: back %u ms after the click, moving after %u ms", t.born - g_state.lastClickTime,
                       now - t.born);
                g_state.holding = true;
                g_state.holdId = c->id;
                g_state.looseTapTime = 0;
                if (t.pressed) {
                    // Already down since it landed.
                } else if (t.afterClick && now - g_state.lastClickTime <= GetDoubleClickTime()) {
                    // Off the click's spot by just past half the double-click rectangle, toward where the pointer
                    // went since (to the right if it didn't move), then back where the pointer is.
                    POINT pos;
                    GetCursorPos(&pos);
                    POINT click = g_state.lastClickPos;
                    int offX = GetSystemMetrics(SM_CXDOUBLECLK) / 2 + 1;
                    int offY = GetSystemMetrics(SM_CYDOUBLECLK) / 2 + 1;
                    int dx = pos.x - click.x;
                    int dy = pos.y - click.y;
                    POINT press = click;
                    if (std::abs(dy) > std::abs(dx)) {
                        press.y += dy < 0 ? -offY : offY;
                    } else {
                        press.x += dx < 0 ? -offX : offX;
                    }
                    if (std::abs(dx) <= offX && std::abs(dy) <= offY) {
                        pos = press;
                    }
                    SendCursorPos(press.x, press.y);
                    SendPrimary(true);
                    SendCursorPos(pos.x, pos.y);
                } else {
                    SendPrimary(true);
                }
                g_state.recentClick = false;
            }
        }
    }
}

// Diagnostics overlay: draws each contact as the mod reads it. Sizes are at 96 DPI, scaled to the monitor's.

constexpr WCHAR kVizClassName[] = L"Touchpad_Relaxed_Hand_Overlay";
constexpr int kVizWidth = 500;
constexpr int kVizHeader = 80;
constexpr int kVizLegend = 44;
constexpr int kVizMargin = 12;
constexpr DWORD kVizTrailMs = 600;
constexpr double kVizArrowGain = 12.0;  // arrow mm drawn per mm moved in a frame
constexpr int kVizCloseSize = 22;       // the close button, in the top right corner
// Per-frame weights and deltas jump with the pad's jitter: they're smoothed over about kVizSmoothFrames frames,
// and held while the hand is stopped, so the overlay shows who pulled the pointer, not the noise.
constexpr double kVizSmoothFrames = 12;
constexpr double kVizMinArrowMm = 0.15;  // smoothed per-frame movement under this draws no arrow
constexpr DWORD kVizShareMs = 500;       // window of the estimator shares in the header

enum VizRole { kVizPalm, kVizThumb, kVizTap, kVizRest, kVizSteer, kVizScroll, kVizHold };

struct VizDot {
    ULONG id;
    double x, y;
    VizRole role;
    double dx, dy;  // mm this frame, 0 if it didn't count
    double weight;  // share of the direction this frame
    bool holdCandidate;
};

struct {
    HWND hWnd;
    double padW = 100, padH = 65;  // mm
    // Filled while a frame is processed:
    std::vector<ULONG> usedAll;    // past the tap filter
    std::vector<ULONG> usedFinal;  // steering (or every finger, during a hold or a scroll)
    std::vector<ULONG> pair;       // the armTwo scroll pair
    std::unordered_map<ULONG, std::pair<double, double>> delta;
    double dx, dy;
    int estimator;  // this frame: 0 none, 1 held by the stop, 2 median while contacts settle, 3 weighted
    std::unordered_map<ULONG, double> share;  // each steering finger's share of the direction this frame
    std::deque<std::pair<DWORD, int>> estimators;  // the last kVizShareMs of them
    double movers;
    // Published for painting:
    std::vector<VizDot> dots;
    std::unordered_map<ULONG, std::deque<std::pair<DWORD, std::pair<double, double>>>> trails;
    double outDx, outDy;
    std::unordered_map<ULONG, double> weight;                       // smoothed
    std::unordered_map<ULONG, std::pair<double, double>> motion;    // smoothed delta, mm per frame
    std::wstring header, detail, shares;
    double scale = 1;  // the monitor's DPI over 96
    HFONT big, small;
} g_viz;

int VizPx(double v) {
    return (int)std::lround(v * g_viz.scale);
}

void VizSetDpi(UINT dpi) {
    g_viz.scale = (dpi ? dpi : 96) / 96.0;
    for (HFONT font : {g_viz.big, g_viz.small}) {
        if (font) {
            DeleteObject(font);
        }
    }
    g_viz.big = CreateFontW(-VizPx(16), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                            L"Segoe UI");
    g_viz.small = CreateFontW(-VizPx(12), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                              L"Segoe UI");
}

// Off, nothing is recorded: the frame only pays for this check.
bool g_vizOn;
bool g_vizShown;  // toggled by the hotkey, cleared by the close button

void VizBeginFrame() {
    g_vizOn = g_vizShown && g_viz.hWnd;
    if (!g_vizOn) {
        return;
    }
    g_viz.usedAll.clear();
    g_viz.usedFinal.clear();
    g_viz.pair.clear();
    g_viz.delta.clear();
    g_viz.share.clear();
    g_viz.dx = 0;
    g_viz.dy = 0;
    g_viz.estimator = 0;
}

bool VizHas(const std::vector<ULONG>& v, ULONG id) {
    return std::find(v.begin(), v.end(), id) != v.end();
}

void VizPublish(const std::vector<const Contact*>& tips, const std::vector<const Contact*>& fingers, DWORD now) {
    if (!g_viz.hWnd) {
        return;
    }
    if (!g_vizOn) {
        if (IsWindowVisible(g_viz.hWnd)) {
            ShowWindow(g_viz.hWnd, SW_HIDE);
            g_viz.trails.clear();
        }
        return;
    }
    if (!IsWindowVisible(g_viz.hWnd)) {
        ShowWindow(g_viz.hWnd, SW_SHOWNOACTIVATE);
    }


    g_viz.dots.clear();
    for (const Contact* c : tips) {
        // A contact is a maybe tap only while it can still tap: before the glide starts, the mod takes no taps.
        auto track = g_state.tracks.find(c->id);
        bool tapping = track != g_state.tracks.end() && !track->second.noTap && IsTapCandidate(track->second, now);
        VizDot dot{c->id, c->x, c->y, tapping ? kVizTap : kVizRest, 0, 0, 0, false};
        bool finger = std::find(fingers.begin(), fingers.end(), c) != fingers.end();
        bool scrolling = (g_state.pairScroll && (c->id == g_state.scrollA || c->id == g_state.scrollB)) ||
                         VizHas(g_viz.pair, c->id);
        if (!finger) {
            dot.role = kVizPalm;
        } else if (IsThumb(c->id)) {
            dot.role = kVizThumb;
        } else if (scrolling) {
            dot.role = kVizScroll;
        } else if (g_state.holding && c->id == g_state.holdId) {
            dot.role = kVizHold;
        } else if (VizHas(g_viz.usedFinal, c->id)) {
            dot.role = kVizSteer;
        } else if (VizHas(g_viz.usedAll, c->id)) {
            dot.role = kVizRest;
        }
        // Moving: weights and arrows follow, smoothed. Stopped: they hold, and the arrows fade out.
        constexpr double k = 1.0 / kVizSmoothFrames;
        auto d = g_viz.delta.find(c->id);
        double fx = d != g_viz.delta.end() ? d->second.first : 0;
        double fy = d != g_viz.delta.end() ? d->second.second : 0;
        if (g_state.stopped) {
            fx = 0;
            fy = 0;
        }
        auto& m = g_viz.motion[c->id];
        m.first += (fx - m.first) * k;
        m.second += (fy - m.second) * k;
        if (std::hypot(m.first, m.second) >= kVizMinArrowMm) {
            dot.dx = m.first;
            dot.dy = m.second;
        }
        double& w = g_viz.weight[c->id];
        if (dot.role != kVizSteer) {
            w = 0;
        } else if (auto share = g_viz.share.find(c->id); !g_state.stopped && share != g_viz.share.end()) {
            w += (share->second - w) * k;
        }
        dot.weight = w;
        auto t = g_state.tracks.find(c->id);
        dot.holdCandidate = t != g_state.tracks.end() && t->second.holdCandidate;
        g_viz.dots.push_back(dot);
        g_viz.padW = std::max(g_viz.padW, c->x);
        g_viz.padH = std::max(g_viz.padH, c->y);

        auto& trail = g_viz.trails[c->id];
        trail.push_back({now, {c->x, c->y}});
    }
    for (auto it = g_viz.trails.begin(); it != g_viz.trails.end();) {
        auto& trail = it->second;
        while (!trail.empty() && now - trail.front().first > kVizTrailMs) {
            trail.pop_front();
        }
        it = trail.empty() ? g_viz.trails.erase(it) : std::next(it);
    }

    for (auto it = g_viz.motion.begin(); it != g_viz.motion.end();) {
        ULONG id = it->first;
        bool down = std::any_of(tips.begin(), tips.end(), [id](const Contact* c) { return c->id == id; });
        if (!down) {
            g_viz.weight.erase(id);
        }
        it = down ? std::next(it) : g_viz.motion.erase(it);
    }

    constexpr double k = 1.0 / kVizSmoothFrames;
    double ox = g_state.stopped ? 0 : g_viz.dx;
    double oy = g_state.stopped ? 0 : g_viz.dy;
    g_viz.outDx += (ox - g_viz.outDx) * k;
    g_viz.outDy += (oy - g_viz.outDy) * k;

    WCHAR text[256];
    if (!g_state.armed) {
        swprintf_s(text, L"idle  ·  %u contacts", (UINT)tips.size());
        g_viz.shares.clear();
    } else {
        std::wstring mode = L"GLIDE";
        if (g_state.holding) {
            mode += L" · HOLD";
        }
        if (g_state.pairScroll || g_two.scrolling) {
            mode += L" · SCROLL";
        }
        if (now - g_pinchTick < kPinchHoldMs) {
            mode += L" · PINCH";
        }
        if (g_state.stopped) {
            mode += L" · stopped";
        }
        // Estimators switch frame by frame: their share over the last moment, largest first.
        if (g_viz.estimator) {
            g_viz.estimators.push_back({now, g_viz.estimator});
        }
        while (!g_viz.estimators.empty() && now - g_viz.estimators.front().first > kVizShareMs) {
            g_viz.estimators.pop_front();
        }
        int count[4] = {};
        for (const auto& e : g_viz.estimators) {
            count[e.second]++;
        }
        const wchar_t* names[4] = {L"", L"stopped", L"settling", L"weighted"};
        int order[3] = {3, 1, 2};
        std::sort(std::begin(order), std::end(order), [&](int a, int b) { return count[a] > count[b]; });
        std::wstring shares;
        for (int i : order) {
            if (count[i]) {
                WCHAR part[32];
                swprintf_s(part, L"%s%s %d%%", shares.empty() ? L"" : L" · ", names[i],
                           (int)std::lround(100.0 * count[i] / g_viz.estimators.size()));
                shares += part;
            }
        }
        g_viz.shares = L"estimator, last 0.5 s:  " + shares;
        swprintf_s(text, L"%s", mode.c_str());
    }
    g_viz.header = text;
    if (g_state.armed) {
        swprintf_s(text, L"%s multiplier  ·  movers %.1f  ·  %.0f mm/s  ·  refs %u  ·  %s hand",
                   g_state.fewMoving ? L"2-1" : L"5-3", g_viz.movers, g_state.speed, (UINT)g_state.refIds.size(),
                   g_hand > 0 ? L"right" : L"left");
    } else {
        text[0] = 0;
    }
    g_viz.detail = text;

    InvalidateRect(g_viz.hWnd, nullptr, FALSE);
}

// Publishes the frame on every way out of ProcessFrame.
struct VizFrame {
    const std::vector<const Contact*>& tips;
    const std::vector<const Contact*>& fingers;
    DWORD now;
    ~VizFrame() { VizPublish(tips, fingers, now); }
};

COLORREF VizColor(VizRole role) {
    switch (role) {
        case kVizPalm:
            return RGB(224, 80, 80);
        case kVizThumb:
            return RGB(80, 80, 80);
        case kVizTap:
            return RGB(224, 176, 0);
        case kVizRest:
            return RGB(130, 130, 130);
        case kVizSteer:
            return RGB(76, 175, 80);
        case kVizScroll:
            return RGB(74, 144, 226);
        case kVizHold:
            return RGB(255, 152, 0);
    }
    return RGB(255, 255, 255);
}

void VizArrow(HDC dc, int x0, int y0, int x1, int y1, COLORREF color, int width) {
    HPEN pen = CreatePen(PS_SOLID, width, color);
    HGDIOBJ old = SelectObject(dc, pen);
    MoveToEx(dc, x0, y0, nullptr);
    LineTo(dc, x1, y1);
    double len = std::hypot(x1 - x0, y1 - y0);
    if (len > VizPx(6)) {
        double ux = (x1 - x0) / len;
        double uy = (y1 - y0) / len;
        double head = 8 * g_viz.scale;
        double side = 5 * g_viz.scale;
        for (int s : {-1, 1}) {
            MoveToEx(dc, x1, y1, nullptr);
            LineTo(dc, x1 - (int)(head * ux - s * side * uy), y1 - (int)(head * uy + s * side * ux));
        }
    }
    SelectObject(dc, old);
    DeleteObject(pen);
}

void VizPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC wdc = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    HDC dc = CreateCompatibleDC(wdc);
    HBITMAP bmp = CreateCompatibleBitmap(wdc, rc.right, rc.bottom);
    HGDIOBJ oldBmp = SelectObject(dc, bmp);

    HBRUSH bg = CreateSolidBrush(RGB(24, 24, 24));
    FillRect(dc, &rc, bg);
    DeleteObject(bg);
    SetBkMode(dc, TRANSPARENT);

    int margin = VizPx(kVizMargin);
    HGDIOBJ oldFont = SelectObject(dc, g_viz.big);
    SetTextColor(dc, RGB(240, 240, 240));
    TextOutW(dc, margin, VizPx(8), g_viz.header.c_str(), (int)g_viz.header.size());
    {
        int size = VizPx(kVizCloseSize);
        int inset = VizPx(6);
        RECT close{rc.right - size - inset, inset, rc.right - inset, inset + size};
        HPEN pen = CreatePen(PS_SOLID, VizPx(2), RGB(170, 170, 170));
        HGDIOBJ old = SelectObject(dc, pen);
        int pad = VizPx(6);
        MoveToEx(dc, close.left + pad, close.top + pad, nullptr);
        LineTo(dc, close.right - pad, close.bottom - pad);
        MoveToEx(dc, close.right - pad, close.top + pad, nullptr);
        LineTo(dc, close.left + pad, close.bottom - pad);
        SelectObject(dc, old);
        DeleteObject(pen);
    }
    SelectObject(dc, g_viz.small);
    SetTextColor(dc, RGB(170, 170, 170));
    TextOutW(dc, margin, VizPx(32), g_viz.detail.c_str(), (int)g_viz.detail.size());
    TextOutW(dc, margin, VizPx(50), g_viz.shares.c_str(), (int)g_viz.shares.size());

    // The pad.
    int padLeft = margin;
    int padTop = VizPx(kVizHeader);
    int padRight = rc.right - margin;
    int padBottom = rc.bottom - VizPx(kVizLegend);
    double scale = std::min((padRight - padLeft) / g_viz.padW, (padBottom - padTop) / g_viz.padH);
    padRight = padLeft + (int)(g_viz.padW * scale);
    padBottom = padTop + (int)(g_viz.padH * scale);
    HBRUSH padBrush = CreateSolidBrush(RGB(40, 40, 40));
    HPEN padPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
    HGDIOBJ oldBrush = SelectObject(dc, padBrush);
    HGDIOBJ oldPen = SelectObject(dc, padPen);
    RoundRect(dc, padLeft, padTop, padRight, padBottom, VizPx(12), VizPx(12));
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(padPen);
    DeleteObject(padBrush);
    auto px = [&](double mm) { return padLeft + (int)(mm * scale); };
    auto py = [&](double mm) { return padTop + (int)(mm * scale); };

    // The edge band, where landing contacts don't tap.
    if (g_padX1 > g_padX0) {
        HPEN edgePen = CreatePen(PS_DOT, 1, RGB(110, 110, 110));
        SelectObject(dc, edgePen);
        SelectObject(dc, GetStockObject(NULL_BRUSH));
        Rectangle(dc, px(g_padX0 + kEdgeMm), py(g_padY0 + kEdgeMm), px(g_padX1 - kEdgeMm), py(g_padY1 - kEdgeMm));
        // The thumb zone's top.
        int zoneY = py(g_padY1 - (g_padY1 - g_padY0) * kThumbZone);
        MoveToEx(dc, px(g_padX0), zoneY, nullptr);
        LineTo(dc, px(g_padX1), zoneY);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(edgePen);
    }

    // Trails, fading with age.
    DWORD now = GetTickCount();
    for (const auto& [id, trail] : g_viz.trails) {
        for (const auto& [tick, p] : trail) {
            int shade = 40 + (int)(110 * (1.0 - std::min<double>(now - tick, kVizTrailMs) / kVizTrailMs));
            SetPixel(dc, px(p.first), py(p.second), RGB(shade, shade, shade));
            SetPixel(dc, px(p.first) + 1, py(p.second), RGB(shade, shade, shade));
        }
    }

    // Contacts: the fill is the role; a steering finger grows with its share of the direction.
    for (const VizDot& dot : g_viz.dots) {
        int cx = px(dot.x);
        int cy = py(dot.y);
        int r = VizPx(dot.role == kVizSteer ? 9 + 16 * dot.weight : 10);
        COLORREF color = VizColor(dot.role);
        HBRUSH brush = dot.role == kVizPalm ? (HBRUSH)GetStockObject(NULL_BRUSH) : CreateSolidBrush(color);
        HPEN pen = CreatePen(PS_SOLID, VizPx(dot.role == kVizPalm ? 3 : 1), dot.holdCandidate ? RGB(255, 152, 0) : color);
        SelectObject(dc, brush);
        SelectObject(dc, pen);
        Ellipse(dc, cx - r, cy - r, cx + r, cy + r);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(pen);
        if (dot.role != kVizPalm) {
            DeleteObject(brush);
        }
        if (dot.dx || dot.dy) {
            VizArrow(dc, cx, cy, cx + (int)(dot.dx * scale * kVizArrowGain), cy + (int)(dot.dy * scale * kVizArrowGain),
                     RGB(220, 220, 220), VizPx(1));
        }
        WCHAR label[32];
        if (dot.role == kVizSteer) {
            swprintf_s(label, L"#%u %.0f%%", dot.id, std::round(dot.weight * 20) * 5);
        } else {
            swprintf_s(label, L"#%u", dot.id);
        }
        SetTextColor(dc, RGB(220, 220, 220));
        TextOutW(dc, cx + r + VizPx(3), cy - VizPx(7), label, (int)wcslen(label));
    }

    // The direction the pointer took this frame, from the pad's center.
    if (std::hypot(g_viz.outDx, g_viz.outDy) >= kVizMinArrowMm) {
        int cx = (padLeft + padRight) / 2;
        int cy = (padTop + padBottom) / 2;
        VizArrow(dc, cx, cy, cx + (int)(g_viz.outDx * scale * kVizArrowGain),
                 cy + (int)(g_viz.outDy * scale * kVizArrowGain), RGB(255, 255, 255), VizPx(3));
    }

    // Legend.
    struct {
        VizRole role;
        const wchar_t* name;
    } legend[] = {{kVizSteer, L"steers"}, {kVizRest, L"resting"}, {kVizThumb, L"thumb"}, {kVizTap, L"maybe tap"},
                  {kVizScroll, L"scroll"}, {kVizHold, L"hold"},   {kVizPalm, L"palm"}};
    int lx = margin;
    int ly = rc.bottom - VizPx(kVizLegend) + VizPx(14);
    int gap = VizPx(14);
    for (const auto& item : legend) {
        HBRUSH brush = CreateSolidBrush(VizColor(item.role));
        RECT sw{lx, ly + VizPx(2), lx + VizPx(10), ly + VizPx(12)};
        FillRect(dc, &sw, brush);
        DeleteObject(brush);
        SetTextColor(dc, RGB(190, 190, 190));
        TextOutW(dc, lx + gap, ly, item.name, (int)wcslen(item.name));
        SIZE size;
        GetTextExtentPoint32W(dc, item.name, (int)wcslen(item.name), &size);
        lx += gap + size.cx + gap;
    }

    SelectObject(dc, oldFont);
    BitBlt(wdc, 0, 0, rc.right, rc.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBmp);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(hWnd, &ps);
}

void VizShow(bool show) {
    g_vizShown = show;
    if (!g_viz.hWnd) {
        return;
    }
    if (show) {
        ShowWindow(g_viz.hWnd, SW_SHOWNOACTIVATE);
        InvalidateRect(g_viz.hWnd, nullptr, FALSE);
    } else {
        ShowWindow(g_viz.hWnd, SW_HIDE);
        g_viz.trails.clear();
    }
}

bool VizOnClose(HWND hWnd, POINT screen) {
    RECT rc;
    GetWindowRect(hWnd, &rc);
    int size = VizPx(kVizCloseSize);
    int inset = VizPx(6);
    return screen.x >= rc.right - size - inset && screen.x < rc.right - inset && screen.y >= rc.top + inset &&
           screen.y < rc.top + inset + size;
}

// Dragged anywhere but the close button, which hides it. The spot it's dragged to is kept.
LRESULT CALLBACK VizWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT:
            VizPaint(hWnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST:
            return VizOnClose(hWnd, {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}) ? HTCLIENT : HTCAPTION;
        case WM_LBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ClientToScreen(hWnd, &pt);
            if (VizOnClose(hWnd, pt)) {
                VizShow(false);
            }
            return 0;
        }
        // On the caption, a double click would maximize the window and a right click open its system menu.
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONUP:
            return 0;
        // Moved to a monitor with another scale: sized again, to the rect Windows suggests.
        case WM_DPICHANGED: {
            VizSetDpi(HIWORD(wParam));
            const RECT* r = (const RECT*)lParam;
            SetWindowPos(hWnd, nullptr, r->left, r->top, r->right - r->left, r->bottom - r->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_EXITSIZEMOVE: {
            RECT rc;
            GetWindowRect(hWnd, &rc);
            Wh_SetIntValue(L"overlayX", rc.left);
            Wh_SetIntValue(L"overlayY", rc.top);
            return 0;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Bottom right of the primary work area, or where it was last dragged to; never activated.
void VizCreate(HINSTANCE hInstance) {
    WNDCLASS wc{
        .lpfnWndProc = VizWndProc,
        .hInstance = hInstance,
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .lpszClassName = kVizClassName,
    };
    RegisterClass(&wc);
    RECT work;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &work, 0);
    // Where it was last dragged to, if that's still on a monitor.
    int savedX = Wh_GetIntValue(L"overlayX", INT_MIN);
    int savedY = Wh_GetIntValue(L"overlayY", INT_MIN);
    bool saved =
        savedX != INT_MIN && savedY != INT_MIN && MonitorFromPoint({savedX + 40, savedY + 20}, MONITOR_DEFAULTTONULL);
    int x = saved ? savedX : work.right - kVizWidth;
    int y = saved ? savedY : work.bottom - kVizWidth;
    // Created at its spot first, so that its monitor's DPI is known, then sized for it.
    g_viz.hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kVizClassName,
                                nullptr, WS_POPUP, x, y, kVizWidth, kVizWidth, nullptr, nullptr, hInstance, nullptr);
    if (!g_viz.hWnd) {
        Wh_Log(L"Overlay window error: %u", GetLastError());
        return;
    }
    VizSetDpi(GetDpiForWindow(g_viz.hWnd));
    int width = VizPx(kVizWidth);
    int height = VizPx(kVizHeader) + VizPx(kVizLegend) +
                 (int)((width - 2 * VizPx(kVizMargin)) * g_viz.padH / g_viz.padW) + VizPx(4);
    if (!saved) {
        x = work.right - width - VizPx(16);
        y = work.bottom - height - VizPx(16);
    }
    SetWindowPos(g_viz.hWnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
    SetLayeredWindowAttributes(g_viz.hWnd, 0, 225, LWA_ALPHA);
}

// The hotkey is only taken while its setting is on: it's system-wide, and other programs may want it.
void VizUpdateHotkey() {
    UnregisterHotKey(g_hWnd, kVizHotkeyId);
    UINT key = g_vizHotkeyKey;
    if (!key) {
        Wh_Log(L"Overlay hotkey: none");
        VizShow(false);
        return;
    }
    if (RegisterHotKey(g_hWnd, kVizHotkeyId, g_vizHotkeyMods | MOD_NOREPEAT, key)) {
        Wh_Log(L"Overlay hotkey registered");
    } else {
        Wh_Log(L"Overlay hotkey taken by another program, or refused: %u", GetLastError());
    }
}

void VizDestroy(HINSTANCE hInstance) {
    if (g_viz.hWnd) {
        DestroyWindow(g_viz.hWnd);
        g_viz.hWnd = nullptr;
    }
    for (HFONT* font : {&g_viz.big, &g_viz.small}) {
        if (*font) {
            DeleteObject(*font);
            *font = nullptr;
        }
    }
    UnregisterClass(kVizClassName, hInstance);
}

bool IsThumb(ULONG id) {
    auto it = g_state.thumbs.find(id);
    return it != g_state.thumbs.end() && it->second == 't';
}

// The thumb's other half, when it's split in two.
bool IsThumbPart(ULONG id) {
    return IsThumb(id) && id != g_state.thumbMain;
}

void FindThumbs(const std::vector<const Contact*>& tips, DWORD now) {
    g_state.backAfter.clear();
    std::erase_if(g_state.gone, [now](const auto& g) { return now - g.tick > kReturnMs; });
    for (auto it = g_state.thumbs.begin(); it != g_state.thumbs.end();) {
        ULONG id = it->first;
        bool down = std::any_of(tips.begin(), tips.end(), [id](const Contact* c) { return c->id == id; });
        if (!down) {
            g_state.landedLow.erase(id);
            auto last = g_state.thumbsLast.find(id);
            if (last != g_state.thumbsLast.end()) {
                g_state.gone.push_back({it->second, last->second.first, last->second.second, now});
                g_state.thumbsLast.erase(last);
            }
        }
        it = down ? std::next(it) : g_state.thumbs.erase(it);
    }
    double zoneTop = g_padY1 - (g_padY1 - g_padY0) * kThumbZone;
    auto partOfThumb = [&](const Contact* c) {
        // Measured from the thumb's own contact, not from its parts: from them, fingers landing in the zone one next
        // to the other were taken for the thumb one after the other. That contact itself, when its id jumps between
        // the halves, is measured from the other half: measured from nothing, it became a finger.
        bool mainDown = c->id != g_state.thumbMain &&
                        std::any_of(tips.begin(), tips.end(), [](const Contact* o) { return o->id == g_state.thumbMain; });
        double toThumb = 1e9;
        double toFinger = 1e9;
        for (const Contact* o : tips) {
            auto state = g_state.thumbs.find(o->id);
            if (o == c || state == g_state.thumbs.end()) {
                continue;
            }
            double d = std::hypot(c->x - o->x, c->y - o->y);
            if (state->second == 't' && (!mainDown || o->id == g_state.thumbMain)) {
                toThumb = std::min(toThumb, d);
            } else if (state->second == 'n') {
                toFinger = std::min(toFinger, d);
            }
        }
        return toThumb < toFinger && toThumb <= kThumbSplitMm;
    };
    // The touchpad can hand an id to another contact (the thumb's to the middle finger, in one frame): a contact
    // jumping farther than a finger moves in a frame is a new one, and is judged again where it is now.
    std::vector<const Contact*> landed;
    // A fast swipe moves every contact past kJumpMm a frame: only a jump beyond what the hand moved (the contacts'
    // lower median) is an id handed to another contact. Read alone, the thumb lost its status in fast swipes.
    std::vector<double> moved;
    for (const Contact* c : tips) {
        auto last = g_state.thumbsLast.find(c->id);
        if (last != g_state.thumbsLast.end()) {
            moved.push_back(std::hypot(c->x - last->second.first, c->y - last->second.second));
        }
    }
    std::sort(moved.begin(), moved.end());
    double handMoved = moved.empty() ? 0 : moved[(moved.size() - 1) / 2];
    for (const Contact* c : tips) {
        auto last = g_state.thumbsLast.find(c->id);
        bool jumped = last != g_state.thumbsLast.end() &&
                      std::hypot(c->x - last->second.first, c->y - last->second.second) > kJumpMm + handMoved;
        GoneContact left{};
        if (jumped) {
            left = {g_state.thumbs[c->id], last->second.first, last->second.second, now};
        }
        g_state.thumbsLast[c->id] = {c->x, c->y};
        if (!g_state.thumbs.count(c->id) || jumped) {
            g_state.landedLow[c->id] = g_padY1 > g_padY0 && c->y >= zoneTop;
            auto back = std::min_element(g_state.gone.begin(), g_state.gone.end(), [c](const auto& a, const auto& b) {
                return std::hypot(c->x - a.x, c->y - a.y) < std::hypot(c->x - b.x, c->y - b.y);
            });
            if (back != g_state.gone.end() && std::hypot(c->x - back->x, c->y - back->y) <= kReturnMm) {
                g_state.thumbs[c->id] = back->thumb;
                g_state.backAfter[c->id] = now - back->tick;
                g_state.gone.erase(back);
            } else if (partOfThumb(c)) {
                g_state.thumbs[c->id] = 't';
            } else {
                g_state.thumbs[c->id] = 'n';
                landed.push_back(c);
            }
        }
        // The contact which had this id left: it may come back under another.
        if (jumped) {
            g_state.gone.push_back(left);
        }
    }

    // The thumb's own contact follows the thumb when ids are handed around: left on a finger, contacts landing
    // next to that finger were taken for parts of the thumb.
    if (!IsThumb(g_state.thumbMain)) {
        auto main = std::find_if(tips.begin(), tips.end(), [](const Contact* c) { return IsThumb(c->id); });
        g_state.thumbMain = main != tips.end() ? (*main)->id : kNoId;
    }

    // See kThumbZone. Between landings, the thumb stays what it is.
    if (landed.empty()) {
        return;
    }
    int contacts = (int)std::count_if(tips.begin(), tips.end(), [](const Contact* c) { return !IsThumbPart(c->id); });
    int needed = g_armTwo ? 3 : g_armThree ? 4 : g_armFour ? 5 : 1;
    auto inside = [](const Contact* c) { return c->x * g_hand; };  // smaller: farther inside the hand
    auto makeThumb = [&](const Contact* c) {
        // The thumb's other half stays part of it: made a finger, it moved the pointer.
        for (auto& [id, state] : g_state.thumbs) {
            auto at = g_state.thumbsLast.find(id);
            if (state != 't' || at == g_state.thumbsLast.end() ||
                std::hypot(at->second.first - c->x, at->second.second - c->y) > kThumbSplitMm) {
                state = 'n';
            }
        }
        g_state.thumbs[c->id] = 't';
        g_state.thumbMain = c->id;
        // Its other half may have landed before it was found: taken then for a finger, it would steer.
        for (const Contact* o : tips) {
            if (o != c && g_state.thumbs[o->id] == 'n' && partOfThumb(o)) {
                g_state.thumbs[o->id] = 't';
            }
        }
    };
    // Fingers only: the thumb's other half, when it's split in two, isn't a finger crossed over it.
    auto fartherIn = [&](const Contact* c) {
        return std::count_if(tips.begin(), tips.end(),
                             [&](const Contact* o) { return !IsThumb(o->id) && inside(c) - inside(o) > kInwardMm; });
    };
    long crossed = tips.size() >= 3 ? 1 : 0;
    for (const Contact* t : tips) {
        if (IsThumb(t->id) && fartherIn(t) > crossed) {
            g_state.thumbs[t->id] = 'n';
        }
    }
    if (contacts < needed) {
        return;
    }
    const Contact* low = nullptr;
    for (const Contact* c : tips) {
        if (g_state.landedLow[c->id] && fartherIn(c) <= crossed && (!low || inside(c) < inside(low))) {
            low = c;
        }
    }
    if (low) {
        if (!IsThumb(low->id)) {
            makeThumb(low);
        }
        return;
    }
    if (contacts >= 5) {
        const Contact* in = *std::min_element(tips.begin(), tips.end(),
                                              [&](const Contact* a, const Contact* b) { return inside(a) < inside(b); });
        if (!IsThumb(in->id)) {
            makeThumb(in);
        }
    }
}

double Median(std::vector<double>& v) {
    size_t mid = v.size() / 2;
    std::nth_element(v.begin(), v.begin() + mid, v.end());
    double m = v[mid];
    if (v.size() % 2 == 0) {
        m = (m + *std::max_element(v.begin(), v.begin() + mid)) / 2;
    }
    return m;
}

void ProcessFrame() {
    DWORD now = GetTickCount();
    double t = Now();
    if (g_state.armed && now - g_state.lastFrameTick > kStaleFrameMs) {
        EndGlide();
    }
    g_state.lastFrameTick = now;
    for (const Contact& c : g_frame) {
        if (c.conf) {
            g_seenConfidence = true;
        }
    }

    int touching = 0;
    std::vector<const Contact*> fingers;
    std::vector<const Contact*> tips;
    for (const Contact& c : g_frame) {
        if (!c.tip) {
            continue;
        }
        touching++;
        tips.push_back(&c);
        // Confidence = 0 marks a resting palm or thumb: it counts to start, but stays out of the center and
        // of taps. Before Confidence is seen once, it's unknown whether the touchpad reports it.
        if (c.conf || !g_seenConfidence) {
            fingers.push_back(&c);
        }
    }
    VizBeginFrame();
    VizFrame vizFrame{tips, fingers, now};

    FindThumbs(tips, now);
    // The thumb landing, lifting or splitting in two doesn't shift the fingers' contacts, and its count changing
    // kept the glide on the settling median.
    int hand = (int)std::count_if(tips.begin(), tips.end(),
                                  [](const Contact* c) { return g_state.thumbs[c->id] == 'n'; });
    if (hand != g_state.lastHand) {
        g_state.lastHand = hand;
        g_state.countChangeTime = now;
    }
    UpdateTracks(fingers, tips, now, touching);
    Learn(fingers, touching, t);
    TrackTwoFingers(fingers, touching, now);

    if (touching == 0) {
        ResetSession();
        return;
    }

    // A palm or thumb (Confidence = 0) only counts toward the whole hand's five: with three on, a two-finger
    // scroll with the thumb resting would start a glide. A count which is off doesn't start: its swipe is Windows'.
    int count = (int)fingers.size();
    bool canArm = (g_armFive && (touching >= kFingers || count >= kFingers)) || (count == 2 && g_armTwo) ||
                  (count == 3 && g_armThree) || (count == 4 && g_armFour && (g_armFive || touching == 4));
    if (canArm && !g_state.armed) {
        g_state.armed = true;
        g_state.armTime = now;
        g_state.fewMoving = count <= 2;
        SetWheelBlock(true);
        SetTimer(g_hWnd, kStaleTimerId, kStaleFrameMs, nullptr);
        Wh_Log(L"Glide started");
    }

    if (!g_state.armed) {
        g_state.anchorValid = false;
        return;
    }

    // A finger which just came down stays out until it can't be a tap anymore, otherwise the tap itself would
    // drag the pointer and the click would miss its target.
    if (g_state.pairScroll) {
        double sx = 0;
        double sy = 0;
        int n = 0;
        for (const Contact* c : fingers) {
            if (c->id != g_state.scrollA && c->id != g_state.scrollB) {
                continue;
            }
            auto found = g_state.scrollLast.find(c->id);
            if (found != g_state.scrollLast.end()) {
                double fx = c->x - found->second.first;
                double fy = c->y - found->second.second;
                if (std::hypot(fx, fy) <= kJumpMm) {
                    sx += fx;
                    sy += fy;
                    n++;
                }
            }
            g_state.scrollLast[c->id] = {c->x, c->y};
        }
        double st = t - g_state.scrollTime;
        g_state.scrollTime = t;
        if (n) {
            Scroll(sx / n, sy / n, st);
        }
    }

    const std::vector<const Contact*>& movers = fingers.empty() ? tips : fingers;
    std::vector<const Contact*> used;
    for (const Contact* c : movers) {
        if (g_state.pairScroll && (c->id == g_state.scrollA || c->id == g_state.scrollB)) {
            continue;
        }
        auto it = g_state.tracks.find(c->id);
        // A finger which already moved past a tap's reach can't be one: it joins at once.
        if (it != g_state.tracks.end() && !it->second.noTap && IsTapCandidate(it->second, now) &&
            it->second.maxDist <= kTapProfiles[g_tapProfile].maxMm) {
            continue;
        }
        used.push_back(c);
        if (g_vizOn) {
            g_viz.usedAll.push_back(c->id);
        }
    }

    // How far each finger went over kWeightWindowSec, for the multiplier and for its weight.
    for (auto it = g_state.trails.begin(); it != g_state.trails.end();) {
        ULONG id = it->first;
        bool present = std::any_of(used.begin(), used.end(), [id](const Contact* c) { return c->id == id; });
        it = present ? std::next(it) : g_state.trails.erase(it);
    }
    std::unordered_map<ULONG, double> reach;
    for (const Contact* c : used) {
        auto& trail = g_state.trails[c->id];
        if (!trail.empty() && std::hypot(c->x - trail.back().x, c->y - trail.back().y) > kJumpMm) {
            trail.clear();
        }
        trail.push_back({t, c->x, c->y});
        while (trail.size() > 1 && t - trail.front().t > kWeightWindowSec) {
            trail.pop_front();
        }
        reach[c->id] = std::hypot(c->x - trail.front().x, c->y - trail.front().y);
    }

    // The speed multiplier counts every finger, the ones which joined later too: only the pointer's direction
    // comes from the fingers which started the glide. The thumb counts as one finger moving with the hand, whatever
    // it does itself: counted by its own movement, its sticking and slipping flipped the multiplier.
    double movedSum = 0;
    double movedSq = 0;
    double movedMost = 0;
    int moving = 0;
    for (const Contact* c : used) {
        if (!IsThumb(c->id)) {
            double d = reach[c->id];
            movedSum += d;
            movedSq += d * d;
            movedMost = std::max(movedMost, d);
            moving++;
        }
    }
    if (moving && std::any_of(tips.begin(), tips.end(), [](const Contact* c) { return IsThumb(c->id); })) {
        double d = movedSum / moving;
        movedSum += d;
        movedSq += d * d;
    }
    if (movedMost >= kMoversMinMm) {
        double movers = movedSum * movedSum / movedSq;
        if (g_vizOn) {
            g_viz.movers = movers;
        }
        if (movers >= kHandMovers) {
            g_state.fewMoving = false;
        } else if (movers <= kFewMovers) {
            g_state.fewMoving = true;
        }
    }

    // Only the fingers which started the glide move the pointer, until they have all lifted: then those left
    // take over. Fingers joining later rest, tap and count for nothing else, so the pointer doesn't take up
    // their shifting as they land and lift.
    auto isRef = [](const Contact* c) {
        return std::find(g_state.refIds.begin(), g_state.refIds.end(), c->id) != g_state.refIds.end();
    };
    // A thumb alone doesn't hold the group: with the other fingers which started the glide up, the rest take over.
    if (!std::any_of(used.begin(), used.end(), [&](const Contact* c) { return isRef(c) && !IsThumb(c->id); })) {
        g_state.refIds.clear();
        for (const Contact* c : used) {
            g_state.refIds.push_back(c->id);
        }
    }
    for (const Contact* c : used) {
        auto it = g_state.tracks.find(c->id);
        if (!isRef(c) && it != g_state.tracks.end() &&
            ((LONG)(it->second.born - g_state.armTime) <= (LONG)kRefJoinMs ||
             it->second.maxDist > kSoloJoinMm + (g_state.travel - it->second.travelLand))) {
            g_state.refIds.push_back(c->id);
        }
    }
    // A hold or a scroll is the fingers working, not resting: every finger moves the pointer, or the wheel, so
    // that the finger which holds, or the fingers back down on a scroll held by the thumb, are followed.
    if (!g_state.holding && !g_two.scrolling) {
        used.erase(std::remove_if(used.begin(), used.end(), [&](const Contact* c) { return !isRef(c); }),
                   used.end());
    }
    // The thumb only rests and starts the glide: alone, it doesn't move the pointer either.
    used.erase(std::remove_if(used.begin(), used.end(), [](const Contact* c) { return IsThumb(c->id); }), used.end());
    // The pad reads a finger at its edge from part of the tip, and its position wanders: it rests while a finger
    // inside steers. All at the edge, they steer, so that the glide doesn't stop there.
    if (std::any_of(used.begin(), used.end(), [](const Contact* c) { return !NearPadEdge(c->x, c->y); })) {
        used.erase(std::remove_if(used.begin(), used.end(), [](const Contact* c) { return NearPadEdge(c->x, c->y); }),
                   used.end());
    }
    if (g_vizOn) {
        for (const Contact* c : used) {
            g_viz.usedFinal.push_back(c->id);
        }
    }
    if (used.empty()) {
        g_state.anchorValid = false;
        return;
    }

    if (!g_state.anchorValid) {
        g_state.last.clear();
        for (const Contact* c : used) {
            g_state.last[c->id] = {c->x, c->y};
        }
        g_state.accX = 0;
        g_state.accY = 0;
        g_state.steps.clear();
        g_state.lastFrameTime = t;
        g_state.anchorValid = true;
        return;
    }

    // Fingers joining or leaving don't stop the others: a joining one only takes its position this frame. Taking
    // every position again on each change lost a frame of movement whenever a finger landed, lifted or became
    // part of the movement, which made five fingers with tap to click on less responsive.
    for (auto it = g_state.last.begin(); it != g_state.last.end();) {
        ULONG id = it->first;
        bool present = std::any_of(used.begin(), used.end(), [id](const Contact* c) { return c->id == id; });
        it = present ? std::next(it) : g_state.last.erase(it);
    }

    // See kWeightWindowSec. While the contacts settle after a count change, the median is followed, which a single
    // shifted contact doesn't move.
    double dx = 0;
    double dy = 0;
    std::vector<double> allX;
    std::vector<double> allY;
    std::vector<const Contact*> allC;
    for (const Contact* c : used) {
        auto found = g_state.last.find(c->id);
        if (found == g_state.last.end()) {
            g_state.last[c->id] = {c->x, c->y};
            continue;
        }
        auto& prev = found->second;
        double fx = c->x - prev.first;
        double fy = c->y - prev.second;
        prev = {c->x, c->y};
        double d = std::hypot(fx, fy);
        if (d > kJumpMm) {
            continue;
        }
        allX.push_back(fx);
        allY.push_back(fy);
        allC.push_back(c);
        if (g_vizOn) {
            g_viz.delta[c->id] = {fx, fy};
        }
    }

    if (allX.empty()) {
        g_state.lastFrameTime = t;
        return;
    }

    if (now - g_state.countChangeTime < kSettleMs) {
        if (g_vizOn) {
            g_viz.estimator = 2;
            for (const Contact* c : allC) {
                g_viz.share[c->id] = 1.0 / allC.size();
            }
        }
        dx = Median(allX);
        dy = Median(allY);
    } else {
        std::vector<double> weights;
        double sum = 0;
        for (size_t i = 0; i < allX.size(); i++) {
            double w = reach[allC[i]->id] + kWeightFloorUnits * g_unitMm;
            w *= w;
            weights.push_back(w);
            dx += allX[i] * w;
            dy += allY[i] * w;
            sum += w;
        }
        dx /= sum;
        dy /= sum;
        if (g_vizOn) {
            g_viz.estimator = 3;
            for (size_t i = 0; i < allC.size(); i++) {
                g_viz.share[allC[i]->id] = weights[i] / sum;
            }
        }
    }

    if (g_vizOn) {
        g_viz.dx = dx;
        g_viz.dy = dy;
    }
    double dt = t - g_state.lastFrameTime;
    g_state.lastFrameTime = t;
    g_state.speed = StepSpeed(dx, dy, dt);
    if (g_state.speed >= kTravelSpeed) {
        g_state.travel += std::hypot(dx, dy);
    }
    g_travelLog.push_back({now, g_state.travel});
    while (g_travelLog.size() > 1 && now - g_travelLog.front().first > kAimLookbackMs + 500) {
        g_travelLog.pop_front();
    }
    POINT cursor;
    if (GetCursorPos(&cursor)) {
        g_pointerLog.push_back({now, cursor});
        while (g_pointerLog.size() > 1 && now - g_pointerLog.front().first > kAimLookbackMs + 500) {
            g_pointerLog.pop_front();
        }
    }
    if (g_two.scrolling) {
        size_t a = 0;
        size_t b = 0;
        double nearest = kScrollPairMm;
        for (size_t i = 0; i < allC.size(); i++) {
            for (size_t j = i + 1; j < allC.size(); j++) {
                double gap = std::hypot(allC[i]->x - allC[j]->x, allC[i]->y - allC[j]->y);
                if (gap <= nearest) {
                    nearest = gap;
                    a = i;
                    b = j;
                }
            }
        }
        if (g_vizOn && a != b) {
            g_viz.pair = {allC[a]->id, allC[b]->id};
        }
        // No pair: still fed, at rest, so that the touch contacts aren't canceled.
        if (!g_two.lifted) {
            Scroll(a != b ? (allX[a] + allX[b]) / 2 : 0, a != b ? (allY[a] + allY[b]) / 2 : 0, dt);
        }
        g_state.steps.clear();
        return;
    }
    if (touching == 2 && allX.size() == 2 && std::hypot(allX[0], allY[0]) >= kPinchMm &&
        std::hypot(allX[1], allY[1]) >= kPinchMm && allX[0] * allX[1] + allY[0] * allY[1] < 0) {
        g_pinchTick = now;
    }
    if (now - g_pinchTick < kPinchHoldMs) {
        g_state.accX = 0;
        g_state.accY = 0;
        g_state.steps.clear();
        return;
    }
    Move(g_state.fewMoving ? g_multLess.load() : g_multMore.load());
    if (g_vizOn && g_state.stopped) {
        g_viz.estimator = 1;
    }
}

// The touchpad.

double LogicalRange(const HIDP_VALUE_CAPS& caps) {
    LONGLONG min = caps.LogicalMin;
    LONGLONG max = caps.LogicalMax;
    // Some devices declare a maximum which only fits unsigned.
    if (max <= min && caps.BitSize > 0 && caps.BitSize <= 32) {
        min = 0;
        max = (1LL << caps.BitSize) - 1;
    }
    return (double)(max - min);
}

// Units: low nibble 1 = SI (cm), 3 = English (inch); UnitsExp is a signed nibble.
double MmPerUnit(const HIDP_VALUE_CAPS& caps) {
    double logical = LogicalRange(caps);
    if (logical <= 0) {
        return 0;
    }
    int exponent = caps.UnitsExp & 0xF;
    if (exponent > 7) {
        exponent -= 16;
    }
    ULONG system = caps.Units & 0xF;
    double unitMm = system == 1 ? 10.0 : system == 3 ? 25.4 : 0;
    double extentMm = ((double)caps.PhysicalMax - caps.PhysicalMin) * std::pow(10.0, exponent) * unitMm;
    if (extentMm < 20 || extentMm > 400) {
        return 0;
    }
    return extentMm / logical;
}

const Device* GetDevice(HANDLE hDevice) {
    auto it = g_devices.find(hDevice);
    if (it != g_devices.end()) {
        return it->second.valid ? &it->second : nullptr;
    }

    Device& dev = g_devices[hDevice];

    UINT size = 0;
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA, nullptr, &size) != 0 || !size) {
        return nullptr;
    }
    dev.preparsed.resize(size);
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA, dev.preparsed.data(), &size) == (UINT)-1) {
        return nullptr;
    }

    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();
    HIDP_CAPS caps;
    if (HidP_GetCaps(preparsed, &caps) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    USHORT valueCapsCount = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> valueCaps(valueCapsCount);
    if (!valueCapsCount ||
        HidP_GetValueCaps(HidP_Input, valueCaps.data(), &valueCapsCount, preparsed) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    const HIDP_VALUE_CAPS* xCaps = nullptr;
    const HIDP_VALUE_CAPS* yCaps = nullptr;
    for (USHORT i = 0; i < valueCapsCount; i++) {
        const HIDP_VALUE_CAPS& vc = valueCaps[i];
        USAGE usage = vc.IsRange ? vc.Range.UsageMin : vc.NotRange.Usage;
        if (vc.UsagePage == kPageDigitizer && usage == kUsageContactCount) {
            dev.hasContactCount = true;
            dev.contactCountCollection = vc.LinkCollection;
        } else if (vc.UsagePage == kPageGeneric && usage == kUsageX) {
            if (std::find(dev.slots.begin(), dev.slots.end(), vc.LinkCollection) == dev.slots.end()) {
                dev.slots.push_back(vc.LinkCollection);
            }
            if (!xCaps) {
                xCaps = &vc;
            }
        } else if (vc.UsagePage == kPageGeneric && usage == kUsageY && !yCaps) {
            yCaps = &vc;
        }
    }
    if (!xCaps || !yCaps) {
        return nullptr;
    }

    dev.mmX = MmPerUnit(*xCaps);
    dev.mmY = MmPerUnit(*yCaps);
    if (dev.mmX <= 0) {
        double range = LogicalRange(*xCaps);
        dev.mmX = range > 0 ? 100.0 / range : 0;  // no physical size: assume 100 mm wide
    }
    if (dev.mmY <= 0) {
        dev.mmY = dev.mmX;
    }

    dev.valid = !dev.slots.empty() && dev.mmX > 0;
    if (dev.valid) {
        auto bounds = [](const HIDP_VALUE_CAPS& caps, double mm, double& lo, double& hi) {
            LONGLONG min = caps.LogicalMin;
            LONGLONG max = caps.LogicalMax;
            if (max <= min && caps.BitSize > 0 && caps.BitSize <= 32) {
                min = 0;
                max = (1LL << caps.BitSize) - 1;
            }
            lo = min * mm;
            hi = max * mm;
        };
        g_unitMm = std::max(dev.mmX, dev.mmY);
        bounds(*xCaps, dev.mmX, g_padX0, g_padX1);
        bounds(*yCaps, dev.mmY, g_padY0, g_padY1);
        g_viz.padW = g_padX1;
        g_viz.padH = g_padY1;
    }
    Wh_Log(L"Touchpad %p: valid=%d, slots=%u, contact count=%d, ~%.0fx%.0f mm", hDevice, dev.valid,
           (UINT)dev.slots.size(), dev.hasContactCount, LogicalRange(*xCaps) * dev.mmX,
           LogicalRange(*yCaps) * dev.mmY);

    return dev.valid ? &dev : nullptr;
}

// In hybrid mode a frame is spread over several reports: only the first carries a contact count above zero,
// and the following ones carry zero with the remaining contacts.
void OnReport(const Device& dev, PCHAR report, ULONG length) {
    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();

    if (dev.hasContactCount) {
        ULONG count = 0;
        if (HidP_GetUsageValue(HidP_Input, kPageDigitizer, dev.contactCountCollection, kUsageContactCount, &count,
                               preparsed, report, length) == HIDP_STATUS_SUCCESS &&
            count > 0) {
            g_frame.clear();
            g_frameExpected = count;
        }
        // The rest of a frame whose start was missed.
        if (!g_frameExpected) {
            return;
        }
    } else {
        g_frame.clear();
        g_frameExpected = (ULONG)dev.slots.size();
    }

    for (USHORT collection : dev.slots) {
        if (g_frame.size() >= g_frameExpected) {
            break;
        }

        ULONG x;
        ULONG y;
        if (HidP_GetUsageValue(HidP_Input, kPageGeneric, collection, kUsageX, &x, preparsed, report, length) !=
                HIDP_STATUS_SUCCESS ||
            HidP_GetUsageValue(HidP_Input, kPageGeneric, collection, kUsageY, &y, preparsed, report, length) !=
                HIDP_STATUS_SUCCESS) {
            continue;
        }

        ULONG id = collection;
        HidP_GetUsageValue(HidP_Input, kPageDigitizer, collection, kUsageContactId, &id, preparsed, report, length);

        bool tip = false;
        bool conf = false;
        USAGE usages[16];
        ULONG usageCount = ARRAYSIZE(usages);
        if (HidP_GetUsages(HidP_Input, kPageDigitizer, collection, usages, &usageCount, preparsed, report,
                           length) == HIDP_STATUS_SUCCESS) {
            for (ULONG k = 0; k < usageCount; k++) {
                if (usages[k] == kUsageTipSwitch) {
                    tip = true;
                } else if (usages[k] == kUsageConfidence) {
                    conf = true;
                }
            }
        }

        g_frame.push_back({id, x * dev.mmX, y * dev.mmY, tip, conf});
    }

    if (g_frame.size() >= g_frameExpected) {
        ProcessFrame();
        g_frame.clear();
        g_frameExpected = 0;
    }
}

void OnRawInput(HRAWINPUT hRawInput) {
    UINT size = 0;
    if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) != 0 || !size) {
        return;
    }

    static std::vector<BYTE> buffer;
    buffer.resize(size);
    if (GetRawInputData(hRawInput, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) == (UINT)-1) {
        return;
    }

    const RAWINPUT* raw = (const RAWINPUT*)buffer.data();
    if (raw->header.dwType != RIM_TYPEHID) {
        return;
    }

    const Device* dev = GetDevice(raw->header.hDevice);
    if (!dev) {
        return;
    }
    g_lastReportTick = GetTickCount();

    const RAWHID& hid = raw->data.hid;
    for (DWORD i = 0; i < hid.dwCount; i++) {
        OnReport(*dev, (PCHAR)hid.bRawData + i * hid.dwSizeHid, hid.dwSizeHid);
    }
}

// The Windows two-finger scroll is decided when the touch starts and goes on with five fingers; while
// gliding it's dropped here. The hook only exists during a glide: while installed, every mouse message of
// the system goes through it.
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) &&
        ((MSLLHOOKSTRUCT*)lParam)->dwExtraInfo != kInputMark && GetTickCount() - g_lastReportTick < kWheelReportMs &&
        GetTickCount() - g_pinchTick >= kPinchHoldMs && GetAsyncKeyState(VK_CONTROL) >= 0) {
        return 1;
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// The hook runs on a thread of its own, which does nothing else: the system waits for it on every mouse
// event, and on the touchpad thread, busy moving the pointer, it held all input, the touchpad's included,
// for up to a second.
DWORD WINAPI HookThread(LPVOID readyEvent) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&MouseHookProc, &hInstance);

    // Creates this thread's message queue before anything is posted to it.
    MSG msg;
    PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
    SetEvent((HANDLE)readyEvent);

    HHOOK hook = nullptr;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message != kWheelBlockMessage) {
            continue;
        }
        if (msg.wParam && !hook) {
            hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInstance, 0);
            if (!hook) {
                Wh_Log(L"SetWindowsHookEx error: %u", GetLastError());
            }
        } else if (!msg.wParam && hook) {
            UnhookWindowsHookEx(hook);
            hook = nullptr;
        }
    }

    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    return 0;
}

void SetWheelBlock(bool on) {
    if (g_hookThreadId) {
        PostThreadMessage(g_hookThreadId, kWheelBlockMessage, on, 0);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INPUT:
            OnRawInput((HRAWINPUT)lParam);
            break;

        case WM_HOTKEY:
            if (wParam == kVizHotkeyId) {
                VizShow(!g_vizShown);
            }
            return 0;

        case kHotkeySettingMessage:
            VizUpdateHotkey();
            return 0;

        case WM_TIMER:
            if (wParam == kShowPointerTimerId) {
                ShowPointer();
                return 0;
            }
            if (wParam == kDoubleTimerId) {
                FirePendingDouble(GetTickCount());
                return 0;
            }
            if (wParam == kInertiaTimerId) {
                InertiaStep();
                return 0;
            }
            if (wParam == kTwoTapTimerId) {
                if (!g_two.deciding) {
                    SettleTwoTap();
                }
                return 0;
            }
            if (wParam == kStaleTimerId) {
                if (g_state.armed && GetTickCount() - g_state.lastFrameTick > kStaleFrameMs) {
                    ResetSession();
                }
                return 0;
            }
            break;

        // A handle can be reused by another device.
        case WM_INPUT_DEVICE_CHANGE:
            g_devices.erase((HANDLE)lParam);
            if (wParam == GIDC_REMOVAL) {
                ResetSession();
            }
            return 0;

        // The lift of the fingers may never be seen from here on, and an ongoing glide would block scrolling.
        case WM_POWERBROADCAST:
            if (wParam == PBT_APMSUSPEND) {
                ResetSession();
            }
            break;

        case WM_WTSSESSION_CHANGE:
            if (wParam == WTS_SESSION_LOCK || wParam == WTS_CONSOLE_DISCONNECT || wParam == WTS_REMOTE_DISCONNECT) {
                ResetSession();
            }
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RunMessageLoop() {
    while (true) {
        DWORD result = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) {
            return;
        }

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

DWORD WINAPI WorkerThread(LPVOID) {
    // A delay in scheduling this thread shows as jitter in the pointer.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&WndProc, &hInstance);

    WNDCLASS wc{
        .lpfnWndProc = WndProc,
        .hInstance = hInstance,
        .lpszClassName = kWindowClassName,
    };
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass error: %u", GetLastError());
        return 0;
    }

    // A hidden top level window rather than a message-only one, which raw input in the background isn't
    // reliably delivered to.
    g_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, kWindowClassName, nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                            hInstance, nullptr);
    if (!g_hWnd) {
        Wh_Log(L"CreateWindowEx error: %u", GetLastError());
        UnregisterClass(kWindowClassName, hInstance);
        return 0;
    }

    VizCreate(hInstance);
    VizUpdateHotkey();

    RAWINPUTDEVICE rid{
        .usUsagePage = kPageDigitizer,
        .usUsage = kUsageTouchPad,
        .dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY,
        .hwndTarget = g_hWnd,
    };
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        WTSRegisterSessionNotification(g_hWnd, NOTIFY_FOR_THIS_SESSION);

        RunMessageLoop();

        SetWheelBlock(false);
        WTSUnRegisterSessionNotification(g_hWnd);

        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = nullptr;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    } else {
        Wh_Log(L"RegisterRawInputDevices error: %u", GetLastError());
    }

    UnregisterHotKey(g_hWnd, kVizHotkeyId);
    VizDestroy(hInstance);
    DestroyWindow(g_hWnd);
    g_hWnd = nullptr;
    UnregisterClass(kWindowClassName, hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    LoadCurve();
    g_lastSave = Now();

    HANDLE hookReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (hookReady) {
        g_hookThread = CreateThread(nullptr, 0, HookThread, hookReady, 0, &g_hookThreadId);
        if (g_hookThread) {
            WaitForSingleObject(hookReady, 5000);
        } else {
            g_hookThreadId = 0;
            Wh_Log(L"Can't start the hook thread: %u", GetLastError());
        }
        CloseHandle(hookReady);
    }

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) {
        g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    }
    if (!g_thread) {
        Wh_Log(L"Can't start the touchpad thread: %u", GetLastError());
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    if (g_hWnd) {
        PostMessage(g_hWnd, kHotkeySettingMessage, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;

    if (g_hookThread) {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    SaveCurve();
}

#if !__has_include(<windhawk_tool_mod.h>)

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

#endif
