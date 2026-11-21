// ==WindhawkMod==
// @id              swipe-for-10start
// @name            Legacy Start Swipe
// @description     Swipe in from the taskbar edge to pull the Start menu out with your finger, and swipe back to close it
// @version         0.7.0
// @author          Zhanyuanium
// @github          https://github.com/Zhanyuanium
// @license         MIT
// @homepage        https://github.com/Zhanyuanium/swipe-for-10start
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luser32 -lhid -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Legacy Start Swipe

Touch and pen gestures for the Windows 10 Start menu that ExplorerPatcher
restores on Windows 11. Swipe in from the taskbar edge and the menu follows
your finger out; drag the open menu back toward that edge and it slides away.
The Windows key, the Start button and a click away play the same animation.

What it does:

- Pull the menu out from the taskbar edge with one finger; the panel stays
  under the finger, and lifting with enough travel lets it settle open.
- A tap on the taskbar background opens nothing; the pull must travel 24 DIP.
- Drag the open menu back toward the taskbar to close it with the same spring.
- A press during a close, or a pull still down when it lands, takes the menu
  back; a menu is never left shown but empty.

What you need:

- Windows 11, x86-64, with ExplorerPatcher's legacy Start
  (`Start_ShowClassicMode=1`). Verified on build 26300.9974 with
  ExplorerPatcher 26100.9711.
- Windhawk 2.0 or later and a touchscreen or pen.

Install **Legacy Start Swipe** from Windhawk and enable it. One mod covers both
processes: the gesture runs in StartMenuExperienceHost, the shell calls in Explorer.

Settings:

- **Menu animation style** — *Windows 11* glides like Quick Settings;
  *Windows 10* is slower off the mark and lands firmer.
- **Animation start speed** — how fast the menu keeps moving when you lift your
  finger, as a percentage of the finger's speed: 0 starts from a standstill,
  100 continues at that speed, up to 300 snaps it away. Windows-key opens and
  closes start from rest.
Known limits:

- The shell owns the reveal, so the first frame can arrive about a quarter of a
  second after the open starts.
- If the launcher's private symbols or the menu layout are unavailable, the mod
  steps aside and the shell handles every press natively; the Start side also
  withdraws while the menu's root is replaced or a failed write invalidates it.
- Tested on one 2880x1800 display at 200 %: the live left docked taskbar and,
  earlier, the bottom layouts. The gesture reads the taskbar's edge rather than
  assuming a side.

The repository has the [user guide](https://github.com/Zhanyuanium/swipe-for-10start#readme)
and the full [limits and troubleshooting](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/docs/limits.md).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- AnimationStyle: Windows 11
  $name: Menu animation style
  $description: >-
    How the menu moves whenever it slides: touch swipes, and Windows-key
    opens and closes too. Both choices are the same spring, only the damping
    differs. Windows 11 (1.72) is fitted to the native Quick Settings panel
    (Win+A) and glides into place; Windows 10 (0.9) is slower off the mark
    and lands firmer, the feel this mod shipped with. A bigger menu takes
    longer rather than moving faster: the duration follows the travel and is
    not a setting.
  $options:
  - Windows 11: glides like the Win+A Quick Settings panel
  - Windows 10: slower off the mark, lands firmer (as shipped)
- GestureStartSpeedPercent: 100
  $name: Animation start speed (% of swipe speed)
  $description: >-
    How fast the menu keeps moving at the instant you lift your finger. The
    speed is measured from an 8-30 ms window of reports that ends at the newest
    point, which must itself be no more than 30 ms before you lift, and it is
    projected onto the direction the menu is about to move: 100 continues at
    that component, a higher value snaps it away faster (up to 300), 0 starts
    from a standstill. A finger held still has no speed to carry, and sideways
    motion does not count, because only the part toward the target enters the
    motion. Neither do Windows-key opens and closes: they start from rest
    whatever you set.
- DiagnosticTrace: false
  $name: Write a diagnostic trace file
  $description: >-
    Writes the mod's decision lines to a log file in its storage folder, one
    file per process (renderer.log in the Start menu host, bridge.log in
    Explorer). Turn it on only when a maintainer or a bug report asks for it:
    the trace is capped, but it is meant for diagnosis, not for everyday use.
    Off, nothing is written to disk.
*/
// ==/WindhawkModSettings==

/*
 * [INPUT]: one image in two hosts, never both in one process. In
 *   StartMenuExperienceHost.exe (namespace start): StartUI_.dll
 *   (PrepareRootElement and the root it returns), Raw Input (RegisterRawInputDevices/
 *   GetRawInputData/GetRawInputDeviceInfoW) for the digitizer, the window, DWM
 *   and taskbar APIs for geometry and layout, SetWinEventHook for the
 *   CoreWindow's cloak transitions, the composition Visual/Element
 *   APIs for Progress, and swipe_ipc_shared.h for the intents, executor
 *   commands and acks, the open generation, the controller direction and peer
 *   liveness. In explorer.exe (namespace bridge): twinui.pcshell.dll
 *   10.0.26100.9949 (the launcher singleton, ShowStartView, Dismiss, IsVisible,
 *   XamlLauncher::OnHotkeyInvoke, XamlLauncherState::OnStartShown/
 *   OnStartHidden/HideStartAndCortana, CStartExperienceManager::Show), the same
 *   channel from the other end.
 * [OUTPUT]: the panel's Progress through the one damped-spring settle, the
 *   reveal and dismiss follows, the coast and the geometry snapshot; the hidden
 *   executor window on explorer's launcher thread that runs the one shell call
 *   per command; one decision line and one executor line per trigger. Every
 *   line goes through `TraceLog`: `Wh_Log` always, and the role's file only
 *   while the `DiagnosticTrace` setting is on. Wh_ModInit
 *   selects exactly one role table from the host executable name
 *   (start::Role or bridge::Role), and the four Windhawk entry points forward
 *   to that table.
 * [POS]: the single shipping source. The two halves were two mods before 0.7.0
 *   and keep their two runtime lifetimes: explorer outlives a Start restart, and
 *   a touch-first open can reach the launcher before any Win or Start-button
 *   hook ran. The controller decides; the executor performs. The one shared rule
 *   is the suppression window: a close suppresses the retracted panel until the
 *   executor's dismiss has landed. The cloak read is tri-state (a failed read is
 *   unknown, never hidden), and a landing is bound to the open generation the
 *   dismiss command was stamped with — a close intent's zero is stamped with the
 *   section's current generation, and zero stays valid only while no open has
 *   been published. A fresh open cancels a still-queued command at the bridge.
 *   A landing the cloak never reports — the
 *   window was already hidden when `Dismiss` ran — is finished by the executor's
 *   own confirmation (`win close observed reason=confirmed-without-transition`),
 *   never by inventing a transition; a confirmation whose cloak sample disagrees
 *   or cannot answer keeps the command, and its bound, for the transition.
 *   Nothing in either half runs on a period: the
 *   controller arms one high-resolution one-shot for the soonest deadline it has
 *   and waits on its events otherwise (the open/close state is a WinEvent with a
 *   bounded fallback poll), and the executor's worker blocks on its events once
 *   the launcher symbols resolved or were refused. The only idle wake is the
 *   100 ms peer-tick heartbeat, and only while the peer is advertised.
 * [PROTOCOL]: Update this header on change, then check AGENTS.md
 */

#include <windhawk_utils.h>
#include <windows.h>
#undef GetCurrentTime  // MinGW winbase.h macro vs. Timeline.GetCurrentTime

#include <shellapi.h>  // SHAppBarMessage(ABM_GETTASKBARPOS): the taskbar's dock edge
#include <hidusage.h>
#include <hidpi.h>
#include <winver.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <utility>
#include <vector>

// ---- begin swipe_ipc_shared.h (inlined: the store takes one self-contained file) ----
// The shared contract defines the vocabulary for both sides. This file is one
// side of it, so the unused half of these constants is deliberate.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"

#include <cstddef>
#include <cstdint>
#include <windows.h>


// Shared-memory contract between the StartMenuExperienceHost-side controller and
// the explorer-side bridge.
//
// Two processes, one session-local section and four auto-reset events. There is
// no file, no mutex and no polling: every record carries its own sequence
// number, and a reader accepts a record only when that number did not move
// while the record was being copied.
//
//   Local\SwipeFor10Start_SingleMod_State      SwipeIpcState
//   Local\SwipeFor10Start_SingleMod_Intent     auto-reset, bridge -> Start
//   Local\SwipeFor10Start_SingleMod_IntentAck  auto-reset, Start -> bridge
//   Local\SwipeFor10Start_SingleMod_Exec       auto-reset, Start -> bridge
//   Local\SwipeFor10Start_SingleMod_ExecAck    auto-reset, bridge -> Start
//
// The names are session-local because both processes run as the same user in the
// same session. There is no product version word: the ABI namespace *is* the
// layout identity. A future incompatible layout changes the prefix, so an old
// pinned development image can never join this channel and a same-size change is
// never reinterpreted across a staggered update. Only a single shipping mod
// uses these names.
//
// ---------------------------------------------------------------------------
// Publishing: one seqlock per record
// ---------------------------------------------------------------------------
//
// Every record a reader copies more than one word from has its own counter. The
// writer bumps the counter to an odd value, writes the fields, and bumps it back
// to even. The reader reads the counter, copies the fields, reads the counter
// again, and accepts the copy only when both reads are equal and even. A torn
// copy is therefore impossible to accept.
//
// One counter for the whole section would not work: the records are written by
// different processes (the Start side the open generation and the executor
// command, the bridge the intent), and a shared counter would invalidate a
// record another side is reading.
//
// A counter left odd means its writer died between the two bumps. The next
// writer takes the record at the next odd value, so no reader can accept the
// fields the dead writer left behind; a reader treats an odd counter as "no
// stable record" after a bounded number of attempts and falls back.
//
// ---------------------------------------------------------------------------
// Who initialises the section
// ---------------------------------------------------------------------------
//
// The section exists as soon as the first of the two processes calls
// CreateFileMappingW, and the other can map it before a single byte was written.
// Creation is therefore not initialisation:
//
//   1. A process claims the section by an interlocked exchange of initState from
//      blank (0) or abandoned (3) to initialising (1). It writes initTick first.
//   2. The claimer writes magic and layoutBytes, then publishes initState =
//      ready (2).
//   3. A process that finds initState = initialising waits and re-checks, in one
//      millisecond steps, for kSwipeIpcOpenBudgetMs: kSwipeIpcInitAbandonMs plus
//      one kSwipeIpcInitWaitMs window, so a dead claim becomes abandonable while
//      this call is still waiting for it.
//   4. A claim older than kSwipeIpcInitAbandonMs is marked abandoned, and the
//      next attempt claims it. A process that died inside step 2 therefore does
//      not brick the channel for the rest of the session.
//   5. Any other initState value is refused with ERROR_INVALID_DATA at once.
//
// SwipeIpcOpen may sleep for its whole budget. Call it from mod init and never
// from a hook.
//
// A reader tells the two failures apart by the error code:
//   ERROR_NOT_READY    no section was handed over: the section is not ready yet
//                      or the budget expired. Nothing was changed.
//   ERROR_INVALID_DATA initState is not one of the four values, or it says ready
//                      while magic or layoutBytes disagrees.
//
// Neither side ever destroys a section the other side may hold: SwipeIpcClose
// closes this process's handles only.
//
// ---------------------------------------------------------------------------
// The records
// ---------------------------------------------------------------------------
//
// Open generation (Start -> bridge): a counter the Start side publishes when it
// sees a fresh menu open. The even value is the identity of that menu. A counter
// of 0 is not a generation; SwipeIpcReadOpen reports ERROR_NOT_READY.
//
// Intent (bridge -> Start): the one decision record. kind is open/close/external,
// trigger names the adapter (win, startbutton, clickaway, touch, override), and
// method/flags are the shell call's own. `open` is the generation the bridge saw.
// A close intent's method is evidence about the shell path that produced it; the
// Start side chooses the executor's execution method separately. The Start side
// answers with intentAck (state, duration, entry, hr), echoing the intent's
// sequence.
//
// Executor command (Start -> bridge): show, dismiss or cancel. The bridge
// validates the record before it calls the launcher: a show names a show method
// (1, 11 or 16) and a flag word from the verified mask; a dismiss names a
// dismiss method (1 or 6) and no flags; a cancel names the dismiss sequence it
// supersedes. A dismiss carries the GetTickCount64 deadline at which the bridge
// must call the launcher; `execTick` is the publish tick and is the deadline of a
// show and a cancel. A command older than the publisher's budget past its
// deadline is stale: the publisher has already cleared its sequence and released
// ownership, so the bridge answers it as expired instead of calling the launcher.
// The bridge answers with execAck (state, hr, visible), where visible is the
// IsVisible result and is advisory for a dismiss.
//
// Controller state (Start -> bridge): one direction word and one touch flag.
// controllerDirection is the whole decision: the bridge reads that single word,
// never a pair of flags, so it cannot see new ownership beside a stale phase.
// Values: kSwipeDirectionUnowned / Open / Closing.
//
// Peer liveness (Start -> bridge): peerEpoch, peerTick and peerState. The bridge
// creates the section, so a mapped section says nothing about whether the Start
// side is running; this triple is the answer the bridge's interception gate
// needs. peerEpoch identifies the activation: it is incremented once per attach,
// before the tick, so a decision the bridge latched against one activation can
// tell that a different one now answers. peerState is alive after the consumer
// is running and gone as soon as that consumer stops; peerTick is the low 32 bits
// of GetTickCount64, refreshed from the Start side's input thread at most once
// per kSwipePeerTickIntervalMs. The bridge intercepts only while the state is
// alive and the tick is younger than kSwipePeerStaleMs: a clean disable is
// immediate through peerState, and a crashed host is caught by the tick going
// stale. That window is a detection budget, not a delivery guarantee.

constexpr wchar_t kSwipeIpcStateName[] =
    L"Local\\SwipeFor10Start_SingleMod_State";
constexpr wchar_t kSwipeIpcIntentEventName[] =
    L"Local\\SwipeFor10Start_SingleMod_Intent";
constexpr wchar_t kSwipeIpcIntentAckEventName[] =
    L"Local\\SwipeFor10Start_SingleMod_IntentAck";
constexpr wchar_t kSwipeIpcExecEventName[] =
    L"Local\\SwipeFor10Start_SingleMod_Exec";
constexpr wchar_t kSwipeIpcExecAckEventName[] =
    L"Local\\SwipeFor10Start_SingleMod_ExecAck";

constexpr uint32_t kSwipeIpcMagic = 0x53313053;  // 'S' '0' '1' 'S', LE

// M3 intents, executor commands and acknowledgements.
constexpr int32_t kSwipeIntentOpen = 0;
constexpr int32_t kSwipeIntentClose = 1;
constexpr int32_t kSwipeIntentExternal = 2;

constexpr int32_t kSwipeTriggerWin = 0;
constexpr int32_t kSwipeTriggerStartButton = 1;
constexpr int32_t kSwipeTriggerClickAway = 2;
constexpr int32_t kSwipeTriggerTouch = 3;
constexpr int32_t kSwipeTriggerOverride = 4;

constexpr int32_t kSwipeExecShow = 0;
constexpr int32_t kSwipeExecDismiss = 1;
constexpr int32_t kSwipeExecCancel = 2;

constexpr int32_t kSwipeAckPending = 0;
constexpr int32_t kSwipeAckExecuted = 1;
constexpr int32_t kSwipeAckRefused = 2;
constexpr int32_t kSwipeAckExpired = 3;
// The dismiss confirmation, kept apart from the call's own outcome. `Executed`
// means the primitive returned; `Confirmed` means a later probe read the window
// hidden; `Unconfirmed` means the probe read it visible; `Unknown` means the
// probe could not answer (no launcher, no state, a failed call). A dismiss is
// never reported as a hide on the strength of its return value alone.
constexpr int32_t kSwipeAckConfirmed = 4;
constexpr int32_t kSwipeAckUnconfirmed = 5;
constexpr int32_t kSwipeAckUnknown = 6;
// A record a newer command or a new activation superseded before it could run.
// Distinct from `Expired`, which means the publisher's budget ran out.
constexpr int32_t kSwipeAckSuperseded = 7;

// The `execAckVisible` word. Negative means the probe could not answer; it is
// not a hidden window.
constexpr int32_t kSwipeVisibilityUnknown = -1;
constexpr int32_t kSwipeVisibilityHidden = 0;
constexpr int32_t kSwipeVisibilityVisible = 1;

// initState values; see "Who initialises the section" above.
constexpr LONG kSwipeIpcBlank = 0;
constexpr LONG kSwipeIpcInitialising = 1;
constexpr LONG kSwipeIpcReady = 2;
constexpr LONG kSwipeIpcAbandoned = 3;

// One wait step for a live claim, the age at which a claim is dead, and the
// whole budget of one SwipeIpcOpen call: the budget must outlast the abandon age,
// or a caller that arrives while a claim is still young would leave without a
// channel. See "Who initialises the section" above.
constexpr ULONGLONG kSwipeIpcInitWaitMs = 50;
constexpr ULONGLONG kSwipeIpcInitAbandonMs = 500;
constexpr ULONGLONG kSwipeIpcOpenBudgetMs =
    kSwipeIpcInitAbandonMs + kSwipeIpcInitWaitMs;

// How many times a reader retries a record whose counter moved. A record is a
// handful of word-sized writes, so a live writer leaves the odd state within
// nanoseconds; a writer that is preempted mid-record fails these attempts and
// the caller falls back instead of spinning.
constexpr int kSwipeIpcReadAttempts = 8;

// Peer liveness. The publisher refreshes peerTick at most once per interval; the
// reader declares the peer gone after the window without a refresh. The window
// is six intervals: five consecutive refreshes may be missed without closing the
// gate, so an ordinary stall of the input thread (a settle, a busy message loop)
// does not flap it, while a killed Start host stops every interception within
// 0.6 s.
constexpr uint32_t kSwipePeerTickIntervalMs = 100;
constexpr uint32_t kSwipePeerStaleMs = 600;

// peerState values. A fresh section reads 0, so `gone` is also what the bridge
// finds before the Start side ever attaches.
constexpr int32_t kSwipePeerGone = 0;
constexpr int32_t kSwipePeerAlive = 1;

// The layout is written once, here, and both sides include this header.
// Windhawk compiles each mod from stdin against the repository include path, and
// a copy in each source would drift. The asserts below pin every field.
struct SwipeIpcState {
    uint32_t magic;

    // Start -> bridge. openSeq is the record's counter and its even value is
    // the open generation.
    volatile LONG64 openSeq;
    volatile LONG64 openTick;

    // Initialisation handshake; see "Who initialises the section".
    volatile LONG initState;
    volatile LONG layoutBytes;
    volatile LONG64 initTick;

    // M3: bridge -> Start intent (seqlock). `intentOpen` is the open generation
    // the bridge saw; `intentKind` is open/close/external and `intentTrigger`
    // names the adapter. `intentMethod` is the shell method that produced the
    // trigger (7 for a click-away) and is evidence, not an execution method.
    volatile LONG64 intentSeq;
    volatile LONG64 intentTick;
    volatile LONG intentKind;
    volatile LONG intentTrigger;
    volatile LONG intentMethod;
    volatile LONG intentFlags;
    volatile LONG64 intentOpen;

    // M3: Start -> bridge answer to an intent (seqlock). `intentAckFor` echoes
    // the intent sequence; durationMs is the close settle the Start planned;
    // entryPercent is the open entry in tenths of a percent.
    volatile LONG64 intentAckSeq;
    volatile LONG64 intentAckFor;
    volatile LONG intentAckState;     // kSwipeAck*
    volatile LONG intentAckDuration;
    volatile LONG intentAckEntry;
    volatile LONG intentAckHr;

    // M3: Start -> bridge executor command (seqlock). Show and cancel run now;
    // dismiss is due at execExecuteAt (a GetTickCount64 deadline). `execOpen` is
    // the open generation from SwipeIpcReadOpen, not the open tick: 0 means the
    // record names no menu and the menu guard allows it.
    volatile LONG64 execSeq;
    volatile LONG64 execTick;
    volatile LONG execKind;           // kSwipeExec*
    volatile LONG execMethod;
    volatile LONG execFlags;
    volatile LONG64 execExecuteAt;
    volatile LONG64 execOpen;
    volatile LONG64 execTarget;       // the dismiss sequence a cancel names

    // M3: bridge -> Start executor answer (seqlock). state is the outcome; hr is
    // the launcher's HRESULT and visible is the visibility probe afterwards
    // (kSwipeVisibility*). A dismiss is answered twice: once when the primitive
    // returns (`Executed`) and once when the confirmation probe reads the window
    // (`Confirmed`/`Unconfirmed`/`Unknown`). The record is one answer, not a
    // queue: a later answer replaces it before the Start side reads it, so the
    // reader matches `execAckFor` and never attributes one state to another
    // sequence.
    volatile LONG64 execAckSeq;
    volatile LONG64 execAckFor;
    volatile LONG execAckState;
    volatile LONG execAckHr;
    volatile LONG execAckVisible;

    // M3/v5: the Start-side session. One authoritative direction word, so the
    // bridge's toggle rule reads a single atomic value and can never see new
    // ownership beside the previous phase.
    volatile LONG controllerDirection;  // kSwipeDirection*
    // 1 while a touch contact owns the opening panel. The bridge swallows the
    // shell's click-away from that same opening contact; it is the finger that
    // pulled the menu out, not a press outside it.
    volatile LONG controllerTouch;
    // The Start side's liveness and identity. peerTick is the low 32 bits of
    // GetTickCount64, so the reader compares by unsigned difference and a wrap
    // after 49.7 days is not a discontinuity. peerState is kSwipePeer*.
    // peerEpoch counts attachments: it is incremented before the tick and alive,
    // so an epoch read beside `alive` names the activation that published them.
    volatile LONG peerTick;
    volatile LONG peerState;
    volatile LONG peerEpoch;
};

static_assert(offsetof(SwipeIpcState, magic) == 0, "layout");
static_assert(offsetof(SwipeIpcState, openSeq) == 8, "layout");
static_assert(offsetof(SwipeIpcState, openTick) == 16, "layout");
static_assert(offsetof(SwipeIpcState, initState) == 24, "layout");
static_assert(offsetof(SwipeIpcState, layoutBytes) == 28, "layout");
static_assert(offsetof(SwipeIpcState, initTick) == 32, "layout");
static_assert(offsetof(SwipeIpcState, intentSeq) == 40, "layout");
static_assert(offsetof(SwipeIpcState, intentTick) == 48, "layout");
static_assert(offsetof(SwipeIpcState, intentKind) == 56, "layout");
static_assert(offsetof(SwipeIpcState, intentTrigger) == 60, "layout");
static_assert(offsetof(SwipeIpcState, intentMethod) == 64, "layout");
static_assert(offsetof(SwipeIpcState, intentFlags) == 68, "layout");
static_assert(offsetof(SwipeIpcState, intentOpen) == 72, "layout");
static_assert(offsetof(SwipeIpcState, intentAckSeq) == 80, "layout");
static_assert(offsetof(SwipeIpcState, intentAckFor) == 88, "layout");
static_assert(offsetof(SwipeIpcState, intentAckState) == 96, "layout");
static_assert(offsetof(SwipeIpcState, intentAckDuration) == 100, "layout");
static_assert(offsetof(SwipeIpcState, intentAckEntry) == 104, "layout");
static_assert(offsetof(SwipeIpcState, intentAckHr) == 108, "layout");
static_assert(offsetof(SwipeIpcState, execSeq) == 112, "layout");
static_assert(offsetof(SwipeIpcState, execTick) == 120, "layout");
static_assert(offsetof(SwipeIpcState, execKind) == 128, "layout");
static_assert(offsetof(SwipeIpcState, execMethod) == 132, "layout");
static_assert(offsetof(SwipeIpcState, execFlags) == 136, "layout");
static_assert(offsetof(SwipeIpcState, execExecuteAt) == 144, "layout");
static_assert(offsetof(SwipeIpcState, execOpen) == 152, "layout");
static_assert(offsetof(SwipeIpcState, execTarget) == 160, "layout");
static_assert(offsetof(SwipeIpcState, execAckSeq) == 168, "layout");
static_assert(offsetof(SwipeIpcState, execAckFor) == 176, "layout");
static_assert(offsetof(SwipeIpcState, execAckState) == 184, "layout");
static_assert(offsetof(SwipeIpcState, execAckHr) == 188, "layout");
static_assert(offsetof(SwipeIpcState, execAckVisible) == 192, "layout");
static_assert(offsetof(SwipeIpcState, controllerDirection) == 196, "layout");
static_assert(offsetof(SwipeIpcState, controllerTouch) == 200, "layout");
static_assert(offsetof(SwipeIpcState, peerTick) == 204, "layout");
static_assert(offsetof(SwipeIpcState, peerState) == 208, "layout");
static_assert(offsetof(SwipeIpcState, peerEpoch) == 212, "layout");
static_assert(sizeof(SwipeIpcState) == 216, "layout");

struct SwipeIpcChannel {
    SwipeIpcState* state;
    HANDLE mapping;
    HANDLE intentEvent;
    HANDLE intentAckEvent;
    HANDLE execEvent;
    HANDLE execAckEvent;
};

// Reads a 64-bit field that the other process may write at any time.
inline LONG64 SwipeIpcLoad64(volatile LONG64* value) {
    return InterlockedCompareExchange64(value, 0, 0);
}

// Takes a record for writing: the returned value is odd. A counter left odd by
// a writer that died is stepped forward to the next odd value, never back to the
// even one it left behind: that even value would be accepted by a reader
// together with the dead writer's half-written fields, and only the odd state
// keeps every reader out until this write is complete.
inline LONG64 SwipeIpcBeginWrite(volatile LONG64* sequence) {
    const LONG64 current = SwipeIpcLoad64(sequence);
    if ((current & 1) != 0) {
        return InterlockedExchangeAdd64(sequence, 2) + 2;
    }
    return InterlockedIncrement64(sequence);
}

// Releases a record: the returned value is even and is the published sequence.
inline LONG64 SwipeIpcEndWrite(volatile LONG64* sequence) {
    return InterlockedIncrement64(sequence);
}

// Copies one record and returns the sequence it was copied under. `copy` writes
// the fields into the caller's snapshot; it runs at least once and at most
// kSwipeIpcReadAttempts times. False means no stable record was copied.
template <typename Copier>
inline bool SwipeIpcSnapshot(volatile LONG64* sequence, LONG64* outSequence,
                             Copier&& copy) {
    for (int attempt = 0; attempt < kSwipeIpcReadAttempts; ++attempt) {
        const LONG64 before = SwipeIpcLoad64(sequence);
        if ((before & 1) != 0) {
            MemoryBarrier();
            continue;  // a write is in progress
        }
        copy();
        MemoryBarrier();
        if (SwipeIpcLoad64(sequence) != before) {
            continue;  // the record moved under the copy
        }
        *outSequence = before;
        return true;
    }
    return false;
}

// Creates the channel when the other side has not, initialises it when neither
// side has, then validates it. `created` reports that this call performed the
// initialisation. Every member is optional except the section itself: a missing
// event disables only the direction it carries. On failure `*error` is
// ERROR_NOT_READY or ERROR_INVALID_DATA as described above, or the failing Win32
// code.
//
// This call may sleep for kSwipeIpcOpenBudgetMs while another process finishes
// its claim, so it belongs at mod init. A hook must not call it: a hook that
// finds no channel takes its own fixed lead.
inline bool SwipeIpcOpen(SwipeIpcChannel* channel, bool* created, DWORD* error) {
    *created = false;
    *error = 0;

    HANDLE mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr,
                                        PAGE_READWRITE, 0,
                                        sizeof(SwipeIpcState),
                                        kSwipeIpcStateName);
    if (!mapping) {
        *error = GetLastError();
        return false;
    }

    auto* state = static_cast<SwipeIpcState*>(
        MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0,
                      sizeof(SwipeIpcState)));
    if (!state) {
        *error = GetLastError();
        CloseHandle(mapping);
        return false;
    }

    // Claim, wait, or discover ready. A section that another process created is
    // initialised by whichever of the two gets here first, never by "the
    // creator": the creator may still be between its CreateFileMappingW and its
    // first write. The deadline outlasts the abandon age on purpose, so a claim
    // that is alive but stuck becomes this caller's instead of ending the wait
    // with no channel.
    const ULONGLONG deadline = GetTickCount64() + kSwipeIpcOpenBudgetMs;
    const auto fail = [&](DWORD code) -> bool {
        *error = code;
        UnmapViewOfFile(state);
        CloseHandle(mapping);
        return false;
    };

    bool owner = false;
    for (;;) {
        const LONG observed = InterlockedCompareExchange(&state->initState, 0, 0);
        if (observed == kSwipeIpcReady) {
            break;
        }
        if (observed == kSwipeIpcBlank || observed == kSwipeIpcAbandoned) {
            // initTick goes first: a claim must carry its own age before it can
            // be judged dead by anyone.
            state->initTick = static_cast<LONG64>(GetTickCount64());
            if (InterlockedCompareExchange(&state->initState,
                                           kSwipeIpcInitialising,
                                           observed) == observed) {
                owner = true;
                break;
            }
            // Another process claimed it first. The next pass sees its state;
            // the budget still bounds this loop.
            if (GetTickCount64() >= deadline) {
                return fail(ERROR_NOT_READY);
            }
            continue;
        }
        if (observed != kSwipeIpcInitialising) {
            // Not a state this header publishes. Waiting cannot turn it into a
            // claim, and a loop that waited anyway would outlast the documented
            // budget. The section belongs to another build or is corrupt; it is
            // left untouched for its owner.
            return fail(ERROR_INVALID_DATA);
        }

        // Initialising. Either a live claim or one whose owner died inside it.
        // A claim older than the abandon age is taken over, which is what the
        // caller's whole budget is for; only a claim with no readable age can
        // outlast the budget.
        const ULONGLONG claimed =
            static_cast<ULONGLONG>(SwipeIpcLoad64(&state->initTick));
        const ULONGLONG now = GetTickCount64();
        if (claimed != 0 && now > claimed &&
            now - claimed > kSwipeIpcInitAbandonMs) {
            InterlockedCompareExchange(&state->initState, kSwipeIpcAbandoned,
                                       kSwipeIpcInitialising);
            // Re-observe whether this process or another one marked it; the
            // deadline check covers a takeover that another caller won first.
            if (GetTickCount64() >= deadline) {
                return fail(ERROR_NOT_READY);
            }
            continue;
        }
        if (now >= deadline) {
            return fail(ERROR_NOT_READY);
        }
        Sleep(1);
    }

    if (owner) {
        state->magic = kSwipeIpcMagic;
        state->layoutBytes = static_cast<LONG>(sizeof(SwipeIpcState));
        MemoryBarrier();
        InterlockedExchange(&state->initState, kSwipeIpcReady);
        *created = true;
    }

    if (state->initState != kSwipeIpcReady ||
        state->magic != kSwipeIpcMagic ||
        state->layoutBytes != static_cast<LONG>(sizeof(SwipeIpcState))) {
        // Broken, or written by another layout. Nothing here is repaired: the
        // other side may be live on it.
        *error = ERROR_INVALID_DATA;
        UnmapViewOfFile(state);
        CloseHandle(mapping);
        return false;
    }

    HANDLE intentEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcIntentEventName);
    HANDLE intentAckEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcIntentAckEventName);
    HANDLE execEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcExecEventName);
    HANDLE execAckEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcExecAckEventName);

    *error = 0;
    channel->state = state;
    channel->mapping = mapping;
    channel->intentEvent = intentEvent;
    channel->intentAckEvent = intentAckEvent;
    channel->execEvent = execEvent;
    channel->execAckEvent = execAckEvent;
    return true;
}

// Drops this process's handles. The section and its events stay alive while the
// other side holds them, so a restarted side reopens the same channel and finds
// the other side's records where it left them.
inline void SwipeIpcClose(SwipeIpcChannel* channel) {
    if (channel->state) {
        UnmapViewOfFile(channel->state);
    }
    if (channel->mapping) {
        CloseHandle(channel->mapping);
    }
    if (channel->intentEvent) {
        CloseHandle(channel->intentEvent);
    }
    if (channel->intentAckEvent) {
        CloseHandle(channel->intentAckEvent);
    }
    if (channel->execEvent) {
        CloseHandle(channel->execEvent);
    }
    if (channel->execAckEvent) {
        CloseHandle(channel->execAckEvent);
    }
    *channel = SwipeIpcChannel{};
}

// Publishes a fresh menu open and returns its generation (the even value of the
// record's counter, unique for every open). Called by the Start side when it
// sees the window uncloak, which is also where the open settle starts. This is
// where the Start side must cancel the close stamp it left; the bridge refuses a
// stamp older than this tick whether or not it does.
inline LONG64 SwipeIpcPublishOpen(SwipeIpcChannel* channel, uint64_t tickMs) {
    if (!channel->state) {
        return 0;
    }

    SwipeIpcBeginWrite(&channel->state->openSeq);
    InterlockedExchange64(&channel->state->openTick,
                          static_cast<LONG64>(tickMs));
    return SwipeIpcEndWrite(&channel->state->openSeq);
}

// Copies the open generation and its tick. A counter of 0 is not a generation:
// no open has been published yet, so the call fails with ERROR_NOT_READY and
// leaves the outputs untouched. A record that never settled fails with
// ERROR_RETRY. `error` may be null. No caller may treat 0 as a generation.
inline bool SwipeIpcReadOpen(SwipeIpcChannel* channel, uint64_t* generation,
                             uint64_t* tickMs, DWORD* error = nullptr) {
    if (error) {
        *error = 0;
    }
    if (!channel->state) {
        if (error) {
            *error = ERROR_INVALID_HANDLE;
        }
        return false;
    }

    uint64_t tick = 0;
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->openSeq, &sequence, [&]() {
            tick = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->openTick));
        })) {
        if (error) {
            *error = ERROR_RETRY;
        }
        return false;
    }
    if (sequence == 0) {
        if (error) {
            *error = ERROR_NOT_READY;
        }
        return false;
    }

    *generation = static_cast<uint64_t>(sequence);
    *tickMs = tick;
    return true;
}

// ---------------------------------------------------------------------------
// M3: intents, executor commands and acks
// ---------------------------------------------------------------------------

struct SwipeIpcIntent {
    LONG64 seq = 0;
    uint64_t tick = 0;
    int32_t kind = kSwipeIntentOpen;
    int32_t trigger = kSwipeTriggerWin;
    int32_t method = 0;
    int32_t flags = 0;
    uint64_t open = 0;
};

struct SwipeIpcIntentAck {
    LONG64 seq = 0;
    LONG64 intent = 0;
    int32_t state = kSwipeAckPending;
    int32_t durationMs = 0;
    int32_t entryPercent = 0;
    int32_t hr = 0;
};

struct SwipeIpcExecCommand {
    LONG64 seq = 0;
    uint64_t tick = 0;
    int32_t kind = kSwipeExecShow;
    int32_t method = 0;
    int32_t flags = 0;
    uint64_t executeAt = 0;
    uint64_t open = 0;
    int64_t target = 0;
};

struct SwipeIpcExecAck {
    LONG64 seq = 0;
    LONG64 command = 0;
    int32_t state = kSwipeAckPending;
    int32_t hr = 0;
    int32_t visible = 0;
};

inline LONG64 SwipeIpcPublishIntent(SwipeIpcChannel* channel, int32_t kind,
                                    int32_t trigger, int32_t method,
                                    int32_t flags, uint64_t open) {
    if (!channel->state) {
        return 0;
    }
    SwipeIpcBeginWrite(&channel->state->intentSeq);
    InterlockedExchange64(&channel->state->intentTick,
                          static_cast<LONG64>(GetTickCount64()));
    InterlockedExchange(&channel->state->intentKind, kind);
    InterlockedExchange(&channel->state->intentTrigger, trigger);
    InterlockedExchange(&channel->state->intentMethod, method);
    InterlockedExchange(&channel->state->intentFlags, flags);
    InterlockedExchange64(&channel->state->intentOpen,
                          static_cast<LONG64>(open));
    const LONG64 sequence = SwipeIpcEndWrite(&channel->state->intentSeq);
    if (channel->intentEvent) {
        SetEvent(channel->intentEvent);
    }
    return sequence;
}

inline bool SwipeIpcReadIntent(SwipeIpcChannel* channel, SwipeIpcIntent* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcIntent record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->intentSeq, &sequence, [&]() {
            record.tick = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->intentTick));
            record.kind =
                InterlockedCompareExchange(&channel->state->intentKind, 0, 0);
            record.trigger = InterlockedCompareExchange(
                &channel->state->intentTrigger, 0, 0);
            record.method = InterlockedCompareExchange(
                &channel->state->intentMethod, 0, 0);
            record.flags = InterlockedCompareExchange(
                &channel->state->intentFlags, 0, 0);
            record.open = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->intentOpen));
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

inline bool SwipeIpcAnswerIntent(SwipeIpcChannel* channel, LONG64 intentSeq,
                                 int32_t state, int32_t durationMs,
                                 int32_t entryPercent, int32_t hr) {
    if (!channel->state || intentSeq <= 0) {
        return false;
    }
    SwipeIpcBeginWrite(&channel->state->intentAckSeq);
    InterlockedExchange64(&channel->state->intentAckFor, intentSeq);
    InterlockedExchange(&channel->state->intentAckState, state);
    InterlockedExchange(&channel->state->intentAckDuration, durationMs);
    InterlockedExchange(&channel->state->intentAckEntry, entryPercent);
    InterlockedExchange(&channel->state->intentAckHr, hr);
    SwipeIpcEndWrite(&channel->state->intentAckSeq);
    return channel->intentAckEvent &&
           SetEvent(channel->intentAckEvent) != FALSE;
}

inline bool SwipeIpcReadIntentAck(SwipeIpcChannel* channel,
                                  SwipeIpcIntentAck* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcIntentAck record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->intentAckSeq, &sequence, [&]() {
            record.intent = SwipeIpcLoad64(
                &channel->state->intentAckFor);
            record.state = InterlockedCompareExchange(
                &channel->state->intentAckState, 0, 0);
            record.durationMs = InterlockedCompareExchange(
                &channel->state->intentAckDuration, 0, 0);
            record.entryPercent = InterlockedCompareExchange(
                &channel->state->intentAckEntry, 0, 0);
            record.hr = InterlockedCompareExchange(
                &channel->state->intentAckHr, 0, 0);
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

inline LONG64 SwipeIpcPublishExec(SwipeIpcChannel* channel, int32_t kind,
                                  int32_t method, int32_t flags,
                                  uint64_t executeAt, uint64_t open,
                                  int64_t target = 0) {
    if (!channel->state) {
        return 0;
    }
    SwipeIpcBeginWrite(&channel->state->execSeq);
    InterlockedExchange64(&channel->state->execTick,
                          static_cast<LONG64>(GetTickCount64()));
    InterlockedExchange(&channel->state->execKind, kind);
    InterlockedExchange(&channel->state->execMethod, method);
    InterlockedExchange(&channel->state->execFlags, flags);
    InterlockedExchange64(&channel->state->execExecuteAt,
                          static_cast<LONG64>(executeAt));
    InterlockedExchange64(&channel->state->execOpen,
                          static_cast<LONG64>(open));
    InterlockedExchange64(&channel->state->execTarget, target);
    const LONG64 sequence = SwipeIpcEndWrite(&channel->state->execSeq);
    if (channel->execEvent) {
        SetEvent(channel->execEvent);
    }
    return sequence;
}

inline bool SwipeIpcReadExec(SwipeIpcChannel* channel,
                             SwipeIpcExecCommand* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcExecCommand record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->execSeq, &sequence, [&]() {
            record.tick = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->execTick));
            record.kind = InterlockedCompareExchange(
                &channel->state->execKind, 0, 0);
            record.method = InterlockedCompareExchange(
                &channel->state->execMethod, 0, 0);
            record.flags = InterlockedCompareExchange(
                &channel->state->execFlags, 0, 0);
            record.executeAt = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->execExecuteAt));
            record.open = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->execOpen));
            record.target = static_cast<int64_t>(
                SwipeIpcLoad64(&channel->state->execTarget));
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

inline bool SwipeIpcAnswerExec(SwipeIpcChannel* channel, LONG64 execSeq,
                               int32_t state, int32_t hr, int32_t visible) {
    if (!channel->state || execSeq <= 0) {
        return false;
    }
    SwipeIpcBeginWrite(&channel->state->execAckSeq);
    InterlockedExchange64(&channel->state->execAckFor, execSeq);
    InterlockedExchange(&channel->state->execAckState, state);
    InterlockedExchange(&channel->state->execAckHr, hr);
    InterlockedExchange(&channel->state->execAckVisible, visible);
    SwipeIpcEndWrite(&channel->state->execAckSeq);
    return channel->execAckEvent &&
           SetEvent(channel->execAckEvent) != FALSE;
}

inline bool SwipeIpcReadExecAck(SwipeIpcChannel* channel,
                                SwipeIpcExecAck* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcExecAck record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->execAckSeq, &sequence, [&]() {
            record.command = SwipeIpcLoad64(
                &channel->state->execAckFor);
            record.state = InterlockedCompareExchange(
                &channel->state->execAckState, 0, 0);
            record.hr = InterlockedCompareExchange(
                &channel->state->execAckHr, 0, 0);
            record.visible = InterlockedCompareExchange(
                &channel->state->execAckVisible, 0, 0);
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

// The controller's published direction, one atomic word. The three values are
// the whole vocabulary the bridge needs: it is a close when the controller owns
// an open, a reopen while the controller is closing, and the shell's cloak
// otherwise. `controllerTouch` stays a separate flag because it answers a
// different question (which contact owns the opening panel) and no reader
// combines it with the direction.
constexpr int32_t kSwipeDirectionUnowned = 0;
constexpr int32_t kSwipeDirectionOpen = 1;
constexpr int32_t kSwipeDirectionClosing = 2;

inline void SwipeIpcSetControllerDirection(SwipeIpcChannel* channel,
                                           int32_t direction) {
    if (channel->state) {
        InterlockedExchange(&channel->state->controllerDirection, direction);
    }
}

inline int32_t SwipeIpcControllerDirection(SwipeIpcChannel* channel) {
    return channel->state
               ? InterlockedCompareExchange(
                     &channel->state->controllerDirection, 0, 0)
               : kSwipeDirectionUnowned;
}

inline void SwipeIpcSetControllerTouch(SwipeIpcChannel* channel, bool touch) {
    if (channel->state) {
        InterlockedExchange(&channel->state->controllerTouch, touch ? 1 : 0);
    }
}

inline bool SwipeIpcControllerTouch(SwipeIpcChannel* channel) {
    return channel->state &&
           InterlockedCompareExchange(&channel->state->controllerTouch, 0,
                                      0) != 0;
}

// ---------------------------------------------------------------------------
// Peer liveness (Start -> bridge)
// ---------------------------------------------------------------------------

// The Start side's input thread refreshes this from its idle tick and from every
// positioned report, throttled by kSwipePeerTickIntervalMs. The bridge never
// writes either word.
inline void SwipeIpcPublishPeerTick(SwipeIpcChannel* channel, uint64_t tickMs) {
    if (channel->state) {
        InterlockedExchange(
            &channel->state->peerTick,
            static_cast<LONG>(static_cast<uint32_t>(tickMs)));
    }
}

// `alive` must follow a fresh tick. The reader pairs an alive state with the
// tick it reads after it, so the previous activation's tick is never accepted as
// this one's.
inline void SwipeIpcSetPeerState(SwipeIpcChannel* channel, int32_t state) {
    if (channel->state) {
        InterlockedExchange(&channel->state->peerState, state);
    }
}

// Claims the next activation identity. Called once per attach, before any tick
// or alive is published, so an activation's epoch differs from the one that came
// before it even when both run inside the same session and the same process
// image. The returned value is what this activation's readers compare.
inline uint32_t SwipeIpcBeginPeerActivation(SwipeIpcChannel* channel) {
    if (!channel->state) {
        return 0;
    }
    return static_cast<uint32_t>(
        InterlockedIncrement(&channel->state->peerEpoch));
}

inline uint32_t SwipeIpcPeerEpoch(SwipeIpcChannel* channel) {
    return channel->state
               ? static_cast<uint32_t>(
                     InterlockedCompareExchange(&channel->state->peerEpoch, 0,
                                                0))
               : 0;
}

// True while the Start side is attached and its tick is younger than `staleMs`.
// The comparison clock is read here, after the tick, never passed in by the
// caller: a tick published between the caller's own clock read and this call
// would subtract to a huge age (the unsigned difference of a heartbeat newer than
// `now`) and close the gate on a peer that had just proved it runs. The state is
// read on both sides of the tick, so a `gone` that lands during the read is not
// missed.
inline bool SwipeIpcPeerAlive(SwipeIpcChannel* channel, uint32_t staleMs) {
    if (!channel->state) {
        return false;
    }
    if (InterlockedCompareExchange(&channel->state->peerState, 0, 0) !=
        kSwipePeerAlive) {
        return false;
    }
    const uint32_t tick = static_cast<uint32_t>(
        InterlockedCompareExchange(&channel->state->peerTick, 0, 0));
    const uint32_t now = static_cast<uint32_t>(GetTickCount64());
    if (InterlockedCompareExchange(&channel->state->peerState, 0, 0) !=
        kSwipePeerAlive) {
        return false;
    }
    return now - tick < staleMs;
}
#pragma clang diagnostic pop
// ---- end swipe_ipc_shared.h ----
// Process termination may skip Wh_ModUninit. These WinRT/Composition projections
// must not run their destructors during that teardown; ordinary unload clears
// them explicitly.
#if defined(__clang__)
#define NO_DESTROY [[clang::no_destroy]]
#else
#define NO_DESTROY
#endif

// ---------------------------------------------------------------------------
// Roles
// ---------------------------------------------------------------------------
//
// The mod is one image, installed once and loaded into two hosts that never
// share a process. Each half publishes one lifecycle table; Wh_ModInit picks the
// table from the host executable name and every Windhawk entry point forwards to
// it. A half's hooks, threads, windows and handles are therefore created only in
// the process that selected that half.

struct SwipeModRole {
    BOOL (*modInit)();
    void (*modAfterInit)();
    void (*modBeforeUninit)();
    void (*modUninit)();
    void (*modSettingsChanged)();
};

// ---------------------------------------------------------------------------
// Diagnostic trace
// ---------------------------------------------------------------------------
//
// Every decision line goes to Windhawk's own log through Wh_Log. That call is
// always made; Windhawk's debug logging decides whether it is stored, and its
// default is off. The same line is also written to one file per role when the
// DiagnosticTrace setting is on, because the regression tools read the lines by
// timestamp from <mod storage>\renderer.log (Start) or \bridge.log (explorer).
//
// The file is capped: after kTraceMaxLines lines the next write restarts it, so
// a trace that is left on cannot grow without bound. The setting is loaded at
// init and on every settings change; it is the only debug switch.

constexpr uint32_t kTraceMaxLines = 20000;

SRWLOCK g_traceLock = SRWLOCK_INIT;
std::atomic<bool> g_traceFile{false};
// Set when the cap cannot truncate the file. The file half stops for this
// session instead of appending past the cap; Wh_Log keeps receiving the lines.
std::atomic<bool> g_traceCapFailed{false};
const wchar_t* g_traceFileName = L"trace.log";
uint32_t g_traceLines = 0;   // guarded by g_traceLock
bool g_traceSeeded = false;  // guarded by g_traceLock

// The role's file name. Called once at init, before the first line and before
// any hook can run this process's half of the mod.
void SetTraceFileName(const wchar_t* name) {
    g_traceFileName = name;
}

// Counts the complete lines already in the file into g_traceLines. Returns true
// when the count is complete: a file that does not exist yet counts as empty
// (the first append creates it), while an existing file that cannot be opened or
// read is *not* seeded, so the next write retries instead of making the cap
// believe the file is empty and growing it past kTraceMaxLines. A read that
// fails midway discards the partial count for the same reason. Called with
// g_traceLock held.
bool SeedTraceLines(const wchar_t* path) {
    g_traceLines = 0;
    HANDLE file = CreateFileW(path, GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    }
    char buffer[8192];
    DWORD read = 0;
    bool complete = true;
    for (;;) {
        if (!ReadFile(file, buffer, sizeof(buffer), &read, nullptr)) {
            complete = false;
            break;
        }
        if (read == 0) {
            break;  // end of file
        }
        for (DWORD i = 0; i < read; ++i) {
            if (buffer[i] == '\n') {
                ++g_traceLines;
            }
        }
    }
    CloseHandle(file);
    if (!complete) {
        g_traceLines = 0;
    }
    return complete;
}

// Reads the setting and gives the next write a fresh line count. Called from
// each role's init and settings change, never from a hook.
void RefreshTraceSetting() {
    g_traceFile.store(Wh_GetIntSetting(L"DiagnosticTrace") != 0,
                      std::memory_order_relaxed);
    g_traceCapFailed.store(false, std::memory_order_relaxed);
    AcquireSRWLockExclusive(&g_traceLock);
    g_traceSeeded = false;
    ReleaseSRWLockExclusive(&g_traceLock);
}

// The one logging entry point. Wh_Log always sees the line; the file sees it
// only while DiagnosticTrace is on. Formatting is truncating: a caller-supplied
// string (a setting, an object name) can be arbitrarily long, and a fixed-buffer
// sprintf would call the CRT invalid-parameter handler and terminate the host.
void TraceLog(const wchar_t* format, ...) {
    WCHAR message[900] = {};
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(message, ARRAYSIZE(message), _TRUNCATE, format, args);
    va_end(args);

    Wh_Log(L"%s", message);

    if (!g_traceFile.load(std::memory_order_relaxed) ||
        g_traceCapFailed.load(std::memory_order_relaxed)) {
        return;
    }
    WCHAR storage[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) {
        return;
    }
    WCHAR path[MAX_PATH] = {};
    if (_snwprintf_s(path, ARRAYSIZE(path), _TRUNCATE, L"%s\\%s", storage,
                     g_traceFileName) <= 0) {
        return;
    }
    // Every file line starts with the system tick, so lines from the two
    // processes can be ordered against each other and against an injection.
    WCHAR line[1024] = {};
    _snwprintf_s(line, ARRAYSIZE(line), _TRUNCATE, L"t=%llu %s",
                 static_cast<unsigned long long>(GetTickCount64()), message);
    // Four bytes per wide char at most: 4096 for the text, two for the CRLF.
    char utf8[4098] = {};
    int bytes = WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8, sizeof(utf8) - 2,
                                    nullptr, nullptr);
    if (bytes <= 1) {
        return;
    }
    // One WriteFile per line, CRLF included: a single append is atomic against
    // the other activation's lines, two calls could interleave and tear both.
    utf8[bytes - 1] = '\r';
    utf8[bytes] = '\n';
    const DWORD lineBytes = static_cast<DWORD>(bytes + 1);

    AcquireSRWLockExclusive(&g_traceLock);
    if (!g_traceSeeded) {
        g_traceSeeded = SeedTraceLines(path);
    }
    // The cap is cut on its own handle: SetEndOfFile needs FILE_WRITE_DATA, and
    // a handle that carries FILE_WRITE_DATA does not append — measured on this
    // system, FILE_APPEND_DATA|FILE_WRITE_DATA writes at offset 0. The line is
    // then written through an append-only handle, so one line is one append
    // from either activation.
    if (g_traceLines >= kTraceMaxLines) {
        HANDLE cut = CreateFileW(path, GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        DWORD error = 0;
        bool truncated = false;
        if (cut != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER zero{};
            const BOOL moved = SetFilePointerEx(cut, zero, nullptr, FILE_BEGIN);
            truncated = moved && SetEndOfFile(cut);
            error = truncated ? 0 : GetLastError();
            CloseHandle(cut);
        } else {
            error = GetLastError();
        }
        if (!truncated) {
            g_traceCapFailed.store(true, std::memory_order_relaxed);
            ReleaseSRWLockExclusive(&g_traceLock);
            Wh_Log(L"DiagnosticTrace stopped: the trace file could not be "
                   L"truncated at its %u-line cap (err=%u); Wh_Log keeps "
                   L"receiving the lines",
                   kTraceMaxLines, error);
            return;
        }
        g_traceLines = 0;
    }
    HANDLE file = CreateFileW(path, FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        const BOOL ok = WriteFile(file, utf8, lineBytes, &written, nullptr);
        // Only a complete line counts. A partial or failed append leaves a torn
        // line the next seed re-counts from the file itself; counting it here
        // would drift the cap by a line the file does not have.
        if (ok && written == lineBytes && g_traceSeeded) {
            ++g_traceLines;
        }
        CloseHandle(file);
    }
    ReleaseSRWLockExclusive(&g_traceLock);
}

// The one show/dismiss vocabulary shared by the bridge's producers and the
// executor. Producer admission checks it before a press is swallowed, so an
// unsupported method/flags combination reaches the shell's own path instead of
// being published and refused later; the executor checks it again as defence in
// depth. The numbers are the shell's; nothing is clamped or widened.
constexpr uint32_t kSwipeShowFlagMask = 0x8 | 0x4;
constexpr uint32_t kSwipeShowMethodKeyboard = 1;
constexpr uint32_t kSwipeShowMethodStartButton = 11;
constexpr uint32_t kSwipeShowMethodTouch = 16;
constexpr uint32_t kSwipeDismissMethodWin = 1;
constexpr uint32_t kSwipeDismissMethodStartButton = 6;
constexpr uint32_t kSwipeDismissMethodClickAwayEvidence = 7;

bool SwipeShowAllowed(uint32_t method, uint32_t flags) {
    return (method == kSwipeShowMethodKeyboard ||
            method == kSwipeShowMethodStartButton ||
            method == kSwipeShowMethodTouch) &&
           (flags & ~kSwipeShowFlagMask) == 0;
}

bool SwipeDismissAllowed(uint32_t method) {
    return method == kSwipeDismissMethodWin ||
           method == kSwipeDismissMethodStartButton;
}

// The intent carries the shell's observed close method. 7 is the click-away's
// evidence and is mapped to 6 at the producer, but only from the caller scope
// that proves the source (`OwnCaller::ClickAway`); 1 and 6 are the executor's own
// dismiss values. 5 (Win+Tab) is an external takeover, never a close intent. A
// method-7 Hide from an ordinary caller is the shell's own close and passes.
bool SwipeCloseIntentAllowed(uint32_t method) {
    return method == kSwipeDismissMethodWin ||
           method == kSwipeDismissMethodStartButton ||
           method == kSwipeDismissMethodClickAwayEvidence;
}

namespace start {


namespace wuc = winrt::Windows::UI::Composition;
namespace wucore = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;

using winrt::Windows::Foundation::Numerics::float2;
using winrt::Windows::Foundation::Numerics::float3;

// StartUI::App::PrepareRootElement(Frame^, PerFrameData^, bool) -> UIElement^,
// a private static C++/CX function, is resolved by its public symbol, not by an
// address: the address moves with the build, the name does not. The name is a
// literal in the hook entry, because the store's symbol extractor reads a name
// only where the entry carries it literally.
//
// The reference address is module + 0xAD730, and it is a test expectation: a
// resolved address that differs is logged, not refused. The decorated spelling
// of the same function is
//   ?PrepareRootElement@App@StartUI@@CAPE$AAVUIElement@Xaml@UI@Windows@@PE$AAVFrame@Controls@456@PE$AAVPerFrameData@2@_N@Z
constexpr uintptr_t kPrepareRootElementExpectedRva = 0xAD730;
constexpr wchar_t kStartUIModuleName[] = L"StartUI_.dll";

// ---------------------------------------------------------------------------
// The gesture settle: one damped spring, two parameter sets
//
// The setting picks the parameter set; every outcome uses the picked one: the
// dismiss commit and cancel, the reveal commit and cancel. Both styles are the
// same generator in normalized travel, so open and close mirror exactly and
// both start at the finger's release speed.
//
// Why a spring and not a bezier. A cubic-bezier with a fixed duration has an
// initial slope that is only a multiple of the animation's own average speed,
// and covering the remaining travel in a fixed time forces the curve to
// accelerate. cubic-bezier(0.1, 0.9, 0.2, 1), the style this mod used to call
// Windows 11, starts at 9x its average speed: on a panel released half open it
// moved 21.6 DIP in the first millisecond. That is the jump the user saw. A
// spring's initial velocity is the finger's release speed (scaled by the
// setting) in either direction, and its damping supplies the deceleration.
//
// The spring works in normalized travel `y = (p - from) / (to - from)`, so
// y(0) = 0 and y(inf) = 1. With natural frequency omega, damping ratio zeta and
// `decay = zeta*omega`:
//
//   underdamped (zeta < 1), wd = omega*sqrt(1 - zeta^2):
//     y(t) = 1 - exp(-decay*t)
//                 * (cos(wd*t) + ((decay - y0) / wd) * sin(wd*t))
//   critically damped (zeta = 1):
//     y(t) = 1 - exp(-omega*t) * (1 + (omega - y0) * t)
//   overdamped (zeta > 1), wd = omega*sqrt(zeta^2 - 1):
//     y(t) = 1 - exp(-decay*t)
//                 * (cosh(wd*t) + ((decay - y0) / wd) * sinh(wd*t))
//
// y0 = y'(0) is the scaled release speed divided by the travel left in DIP.
// The gesture reports that speed as a magnitude (DIP/ms along the axis, away
// from the release point), so y0 is positive and the spring always relaxes from
// 0 toward 1; `(to - from)` carries the direction of the panel.
//
// The duration comes from the speed, not from a fitted time. The native Quick
// Settings flyout is 578 DIP tall and travels it in 262 ms, an average of
// 2.2 DIP/ms. Copying its 253 ms instead made the 900+ DIP Start menu race and
// made the feel follow the panel's size, so every settle lasts the time its own
// travel needs at that speed:
//
//   remaining = |to - from| * Distance                DIP of panel travel
//   T         = clamp(remaining / 2.2 DIP/ms, 160, 520)  ms
//   omega     = 5 / (time constant of the slowest root * T)
//
// The frequency follows from T, not from a period. Five time constants of the
// slowest root land exactly on T. That root is zeta*omega while the spring
// oscillates, so omega = 5/(zeta*T) there, and omega*(zeta - sqrt(zeta^2-1))
// once it is overdamped: applying zeta*omega to the overdamped Windows 11 set
// would put only 0.93 time constants in T and leave 44% of the travel for the
// last frame. Either way the curve is one shape stretched in time: the damping
// ratio is the style, T is the travel.
//
// The analytic curve is 1 - exp(-5) ~ 0.7% short of the target at T. Writing
// the exact target on a final keyframe made the panel hop that remainder after
// a frame that had moved under 0.1 DIP (3.8 DIP on a 501 DIP travel), so the
// whole curve is scaled by its own value at T instead: every sample rises by the
// same fraction and the last one reaches the target with no step of its own.
// One segment is at most 4 ms, so at 120 Hz (8.3 ms per frame) no corner can
// fall inside a displayed frame; the floor of 12 segments keeps the shape of
// the shortest settle, and 128 caps the keyframe count.
//
// Windows 10 keeps the damping this mod shipped with: 0.9.
//
// Windows 11 is fitted to the native flyout on this machine. Measured with
// Win+A (Quick Settings, the same Fluent motion), 9 ms per frame, panel offset
// tracked against its settled frame: the native show travels its 578 DIP in
// 262 ms. The area-minimizing spring with y0 = 0 over that curve is damping
// 1.72, period 0.102 s at 578 DIP: mean |y - d| = 0.034 over the 306 ms
// measured, at most 0.012 after the first 90 ms. The damping is what the fit
// keeps; the period is derived from the travel now, so the 900 DIP panel takes
// 409 ms and moves at the flyout's 2.2 DIP/ms. The native curve is faster than
// that spring in its first 30 ms (0.54 of the travel against 0.38), because the
// shell starts that animation with its own initial velocity; a settle that must
// continue the finger cannot copy that, and this fit deliberately matches the
// deceleration after the first frames instead.
// ---------------------------------------------------------------------------
constexpr double kWin10SpringDamping = 0.9;
constexpr double kWin11SpringDamping = 1.72;
constexpr double kSettleTimeConstants = 5.0;
// The native panel's average speed: 578 DIP in 262 ms. Duration is travel
// divided by this, so the 900 DIP menu takes 409 ms and a half-open one 205 ms,
// whatever the panel measures.
constexpr float kSettleSpeedDipPerMs = 2.2f;
constexpr std::chrono::milliseconds kSettleMinDuration{160};
constexpr std::chrono::milliseconds kSettleMaxDuration{520};
// One segment per 4 ms: at 120 Hz a segment is shorter than one compositor
// frame, so a corner never lands inside a displayed frame.
constexpr double kSettleSegmentMs = 4.0;
constexpr int kSettleMinSteps = 12;
constexpr int kSettleMaxSteps = 128;
// The frames: the segments plus the landing frame at T.
constexpr int kSettleMaxFrames = kSettleMaxSteps + 1;

// The Windows 11 style used to be one eased pair on the Fluent decelerate
// curve over a fixed 300 ms. That curve is gone: it starts at 9x its average
// speed, which is the jump a release at half open produced. Both styles now run
// the spring below.

// The gesture start-speed setting: the settle's initial velocity as a
// percentage of the component of the finger's end speed that points at the
// target. 100 means that component, divided then only by the curve's own
// end-value scale (about 1/0.993); 0 starts from rest. Both styles read it.
constexpr int kGestureStartSpeedDefault = 100;
constexpr int kGestureStartSpeedMin = 0;
constexpr int kGestureStartSpeedMax = 300;

// Every gesture close ends in one executor dismiss at the end of its settle. The
// dismiss is the bridge's Dismiss on the launcher thread: no key handler, no
// explorer-side request and no re-arm exist. The settle runs first, so the panel
// is already off screen when the window hides.

// A close this mod started hides the menu a moment later. Until then the shell
// still reports the window open, so the visibility service must not read that as
// a fresh open and restore Progress while the close is in flight.
constexpr uint64_t kRestoreSuppressMs = 600;

// The scheduled dismiss has to run inside that window, or the visibility service
// reads the still-open window as a fresh open and restores Progress while the
// close is in flight. Both styles must land inside it, and the clamp is their
// upper bound.
static_assert(static_cast<uint64_t>(kSettleMaxDuration.count()) <=
                  kRestoreSuppressMs,
              "the close dismiss must run inside the restore suppression");

// Gesture thresholds. Delta and Distance are DIP, velocity is DIP/ms.
constexpr float kCommitDistanceFraction = 0.20f;
// A vertical dismiss starts counting only once the list underneath has reached
// its end, so the remaining drag is short by nature; 20% of the panel would ask
// for a drag most people never make.
constexpr float kCommitDistanceFractionVertical = 0.10f;
constexpr float kCommitVelocity = 0.45f;
constexpr float kScrollAbortPerp = 120.0f;
constexpr float kScrollAbortRatio = 1.5f;
constexpr float kScrollAbortMaxDelta = 60.0f;
// The insert filter is a screen distance, not a progress fraction: one step of
// the scalar must move the panel by at least this many DIP. A fixed 0.004 was
// 5.8 DIP on the 1440 DIP distance here, which is 11.5 physical px at 200%
// scaling and read as a staircase during a drag. The panel reports at ~170 Hz,
// so writing Progress per report is affordable and buys a continuous follow.
constexpr float kProgressEpsilonDip = 0.5f;
constexpr float kWindowMarginDip = 4.0f;

// A vertical dismiss and the app list's own scroll are the same downward drag.
// The element that actually scrolls the app list is cached per open, and a
// dismiss may only start on it when it is already at the end the drag would
// scroll away from: a list at its top cannot scroll further up, so the drag has
// nothing left to do and belongs to the dismiss. `kScrollerEndSlopDip` absorbs
// the fraction of a DIP a viewport leaves at its end; `kScrollerAxisSlopDip`
// decides which viewer counts as scrollable at all (the wrapper ScrollViewer
// reports a horizontal extent of 8380 and must never be picked).
// The list rarely stops exactly at its end after a fling, so the end test is
// generous; the panel-band test keeps a tight tolerance.
constexpr float kScrollerEndSlopDip = 24.0f;
constexpr float kPanelBandSlopDip = 2.0f;
constexpr float kScrollerAxisSlopDip = 0.5f;
constexpr uint32_t kScrollerScanMaxDepth = 32;
// The reject line is diagnostic, not a per-sample measurement: the digitizer
// reports at ~170 Hz, so it speaks at most once per window.
constexpr uint64_t kDismissRejectLogMs = 500;

// Reveal follow (edge swipe to open). A contact that starts inside the edge
// band on the taskbar side can drive the menu's own open. The release decision
// uses the dismiss thresholds on the travel away from the edge, not on the
// mapped progress: the screen edge already maps to Progress 0.269 here, so any
// progress threshold commits every release.
//
// `kEdgeBandDip` is only the fallback for the first reports of a process, before
// the snapshot carries the taskbar's measured band (GestureGeometry::band): the
// band is the bar's strip plus one bar thickness, so an auto-hidden bar keeps
// the same 96 DIP even though it reserves no work area. The old fixed 12 DIP is
// narrower than the strip a hand starts its pull in, which is why a pull that
// began 20 DIP inside the hidden bar never became a candidate.
constexpr float kEdgeBandDip = 12.0f;
// A contact that started while the menu was closed and has already pulled this
// far along the opening axis is a drag, not a tap. The number is what separates
// a pull from the few DIP a finger slides during a tap: the shell opens the menu
// under the contact, and only movement beyond it may take the open over. It is
// also the travel the edge candidate needs before its open counts as its own,
// so a tap on the taskbar still gets the keyboard-style settle.
constexpr float kAdoptTravelDip = 24.0f;
// A contact that started inside the reveal band and has already moved this far
// away from the taskbar is the tail of a pull that is opening the menu, not a
// drag of the open menu. It must not take Progress over from the open's settle,
// and its release must not settle anything: the pull that opened the menu owns
// both. The number is small because the move is a direction test, not a threshold
// for a gesture; a press in the band that moves toward the taskbar is a dismiss
// from its first sample.
constexpr float kBandTailTravelDip = 8.0f;
// How many of a contact's positioned reports the release window keeps. This is
// the same count the end-velocity estimator holds: the window is replayed from
// the contact's own reports when the menu opens under it, so a takeover one or
// two reports before the lift still measures the finger instead of starting
// from one sample.
constexpr int kEndVelocitySamples = 8;
// The panel reports at ~170 Hz (about 6 ms apart) and keeps reporting while the
// finger rests, so a short silence really is a lift. 18 ms is above the 15.6 ms
// quantum that an injected step really takes (measured: Sleep(8) in pwsh yields
// 15-16 ms gaps), so a step never reads as a lift. The release is a deadline on
// this window, armed as a one-shot at the last report, so it lands at 18-19 ms
// instead of waiting for a periodic tick.
constexpr uint64_t kIdleReleaseMs = 18;
// A band candidate survives a sparse report stream. A touch panel that samples
// slower than the built-in one (or an injected test with 20-30 ms steps) would
// otherwise lose the candidate at the 18 ms idle and re-admit it at the crossing
// point, so a 25 DIP flick could never reach the claim. The candidate's own
// watchdog is longer; a real lift is a report or this window. The 18 ms idle
// still ends a dismiss session, whose panel samples at ~170 Hz.
constexpr uint64_t kCandidateIdleHoldMs = 120;
// The fallback tick: only armed when the high-resolution timer cannot be
// created, and only while a deadline is outstanding, at that deadline's own
// period. WM_TIMER cannot go faster than the 15.6 ms system quantum, so it
// degrades the release to 31-34 ms rather than losing it.
constexpr UINT_PTR kIdleTimerId = 1;
// The DWM cloaked query is a syscall. The report handler accepts one answer
// per interval and reuses it for every other report. This is not the idle
// state's poll: the transition itself is a WinEvent, and the slow fallback poll
// below only runs while a transition is outstanding.
constexpr uint64_t kOpenCheckIntervalMs = 20;

// The one idle wake left while the peer service is advertised: the bridge reads
// a tick younger than kSwipePeerStaleMs (600), and the panic write is throttled
// to kSwipePeerTickIntervalMs (100) in the shared header. Ten wakes a second on
// this event alone, none of them a timer, are the price of a live controller.
constexpr uint64_t kPeerHeartbeatMs = kSwipePeerTickIntervalMs;

// The input thread's root-symbol retry interval (StartUI_'s PrepareRootElement).
// It is also the wait planner's bootstrap deadline: while the answer is neither
// ready nor terminally refused, the thread has to come back for the probe, so a
// module that loads late is still noticed. See kRootPendingDeadlineMs for the
// budget this retry runs inside.
constexpr uint64_t kRootProbeMs = 100;

// The fallback poll under the cloak event. A WinEvent can be missed (a dropped
// message, a hook installed after the transition); a menu left in the wrong
// state is a bug the user sees, so a request keeps a bounded window open in
// which the poll runs at this slow cadence. It is not a 20 ms poll: nothing
// polls while no transition is outstanding.
constexpr uint64_t kVisibilityFallbackMs = 100;
// How long a requested transition waits for its cloak event. Longer than the
// executor's 400 ms expiry plus its 300 ms confirmation probe and the shell's
// own hide, and short enough that a stuck state is repaired inside a second.
constexpr uint64_t kVisibilityGraceMs = 1000;
// How long an observed transition keeps the fallback watching after it landed.
// The unhealed-panel repair needs a poll after the open's own measurement turn
// has run (an open that landed before the first publish restores at rest), and
// that turn is posted, not waited for.
constexpr uint64_t kVisibilitySettleWatchMs = 500;

// A measured panel rectangle is this layout's panel only when it lies on the
// monitor and the side against the taskbar sits in the strip that side defines:
// the band the taskbar reserves, extended by one bar thickness toward the desktop
// (TaskbarProbe measures it, because an auto-hidden bar reserves no work area).
// The settled panel sits 12 DIP from the bar's inner edge, so the strip is twice
// the bar's thickness and the 2 DIP margin only rounds it.
//
// `kPanelLayoutMarginDip` rounds a border that sits exactly on an edge; nothing
// larger is a layout fact. `kPanelAnchorMarginDip` is the same rounding for the
// taskbar strip. Neither is
// `kScrollerEndSlopDip` (24 DIP): that number is a scroll-position allowance for
// a list a fling stops short of its end, and reusing it for layout is what
// accepted a storyboard frame on a work area whose origin is 0 (this machine:
// panel rect 12,-12..1005,539 while the settled rect is 236,289..1229,840).
constexpr float kPanelLayoutMarginDip = 2.0f;
constexpr float kPanelAnchorMarginDip = 2.0f;

// The settled measurement's guaranteed path. XAML may never deliver a second
// LayoutUpdated while nothing moves, so a request that only waits for the layout
// watch could wait forever; this deadline runs the same pass.
constexpr uint64_t kSettledMeasureDeadlineMs = 400;

// ---------------------------------------------------------------------------
// Coast between reports
//
// The release is one idle window plus one tick: report, 18 ms of silence, then
// the tick that plans the close, measured 20-23 ms. The panel stood still for
// that whole window, and the eye reads the stop before the retract starts. The
// panel now keeps moving at the finger's own speed until the close takes over:
//
// - the speed is the average over the last three reports, so one slow report
//   cannot fling the panel, and it is capped at 6 DIP/ms so a bad sample cannot
//   throw it across the screen either;
// - the coast starts after 8 ms without a report, so it is already moving when
//   the release is planned 10-15 ms later;
// - a report ends it, pins the interpolated value and inserts the real one, so
//   the correction is the few DIP the panel drifted;
// - the release ends it first and plans the close from where it got to.
//
// The 200 ms run is the animation's own length, not how long it lives: the
// release or the next report always interrupts it. `Progress` is linear here,
// never eased: a curve would slow the panel in the first frames, which is the
// stop the coast exists to hide.
//
// The real digitizer reports at ~170 Hz (5.9 ms), so a contact that is still
// down never leaves an 8 ms gap and a held finger never coasts.
constexpr uint64_t kCoastStartMs = 8;
constexpr std::chrono::milliseconds kCoastDuration{200};
// Below this speed the coast would move the panel about one DIP in the window it
// lives, and a report stream slower than the panel's own, such as an injector's
// 10 ms keep-alive, must not nudge it. No coast starts there.
constexpr float kCoastMinVelocityDip = 0.1f;
// Progress moves with the panel in both gestures, so the one clamp serves both:
// DIP per millisecond, reached here with Distance.
constexpr float kCoastMaxVelocityDip = 6.0f;
// The reveal maps Progress from the panel edge the snapshot carries; for the
// first 50 ms of a follow that mapping is still settling.
constexpr uint64_t kRevealCoastSettleMs = 50;
// The live follow chases the finger's absolute position at `kSettleSpeedDipPerMs`
// (2.2 DIP/ms, the planner's own speed). A step is bounded by that speed times the
// time since the last report; this cap only keeps a stalled thread from turning
// the bound into a jump, and it is longer than the 18 ms idle that ends a contact.
constexpr uint64_t kRevealChaseMaxStepMs = 50;
// The entry is the first visible position, so it is written and then held for the
// first presentation (two frames at 60 fps) before the chase clock starts. Without
// that beat the chase's own first steps are already 40-90 px up when the first
// frame after the uncloak is composited, and the panel appears a few percent open
// instead of exactly at the edge. It costs the catch-up that much extra time.
constexpr uint64_t kRevealEntryHoldMs = 30;
// The coast start/stop lines measured the coast; off in the shipping build.
constexpr bool g_debugCoast = false;
// The `vel window` and `follow`/`take` lines print the release-speed window one
// release reads; off in the shipping build, like g_debugCoast.
constexpr bool g_debugVelocity = false;

// Raw input and HID decoding limits.
constexpr int kMaxHidDevices = 4;
constexpr int kMaxContactCollections = 8;
constexpr int kMaxLinkNodes = 64;
constexpr int kHidPathChars = 512;
constexpr wchar_t kHidWindowClass[] =
    L"WindhawkLegacyStartSwipeRawInputWindow";
// The private wake the UI thread posts when it publishes a deadline the input
// thread's planner may already have planned past (the scroller retry). The
// message is only a nudge: the loop re-plans after it. Distinct from the
// bridge's `kExecMessage` (WM_APP + 0x5A), which lives in the other process.
constexpr UINT kInputWakeMessage = WM_APP + 0x61;

constexpr USAGE kUsagePageGeneric = 0x01;
constexpr USAGE kUsagePageDigitizer = 0x0D;
constexpr USAGE kUsageTouchScreen = 0x04;
constexpr USAGE kUsagePen = 0x02;
constexpr USAGE kUsageContact = 0x05;
constexpr USAGE kUsageFinger = 0x22;
// A pen's data lives in its own link collection, not in a contact collection.
constexpr USAGE kUsageStylus = 0x20;
constexpr USAGE kUsageX = 0x30;
constexpr USAGE kUsageY = 0x31;
constexpr USAGE kUsageInRange = 0x32;
constexpr USAGE kUsageTipSwitch = 0x42;
// Contact Identifier: per-contact identity for the lifetime of one contact, so a
// contact node that is reused by another finger does not transfer the gesture.
constexpr USAGE kUsageContactIdentifier = 0x51;
constexpr USAGE kUsageContactCount = 0x54;

// ICoreWindowInterop (45D64A29-A63E-4CB6-B498-5781D298CB4F). Slot 3 is
// get_WindowHandle; the interface is classic COM, so it is called directly.
constexpr GUID kCoreWindowInteropIid = {
    0x45D64A29, 0xA63E, 0x4CB6,
    {0xB4, 0x98, 0x57, 0x81, 0xD2, 0x98, 0xCB, 0x4F}};
using GetCoreWindowHandleAbi = HRESULT(STDMETHODCALLTYPE*)(void*, HWND*);

// DWMWA_CLOAKED (dwmapi.h value 14). The CoreWindow HWND outlives the menu and
// IsWindowVisible stays TRUE while it is closed, so cloaking is the only
// reliable open/closed signal: 0 while the menu is shown.
constexpr DWORD kDwmwaCloaked = 14;
using DwmGetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);
// Timer resolution and the wake policy
//
// The input thread owns no period. It wakes for an event, or for the one
// deadline it armed a high-resolution one-shot timer for, and with no deadline
// it waits on its events alone. Two clocks have to be kept exact, and both were
// measured on this build (26300.9974):
//
// - the tick: a 1 ms timeBeginPeriod request is held while a contact is down,
//   as asked, and it raises the process resolution (Sleep(1) takes 1-2 ms).
//   WM_TIMER ignores it: a 5 ms SetTimer still delivered every 15.6 ms. The
//   deadline timer is therefore a waitable timer created with
//   CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, which fires at the requested
//   millisecond with or without the period request (measured 5-6 ms at a 5 ms
//   due). SetTimer remains the fallback when the timer cannot be created.
// - the clock: GetTickCount64 also advances only every 15.6 ms, so an 18 ms
//   silence read as 31. The silence is measured with QueryPerformanceCounter
//   (QpcMsNow below).
//
// The silence window itself is 18 ms: above the 15.6 ms quantum that an
// injected step really takes, below the 20 ms the mod used to wait. A 15 ms
// window split every injected step into its own one-sample session. Nothing
// here changes that: the deadline is `lastAliveQpc + 18`, armed as a one-shot,
// so the release lands at 18-19 ms instead of on the 20 ms tick it used to
// land on.
// ---------------------------------------------------------------------------

constexpr UINT kContactTimerPeriodMs = 1;
using TimePeriod_t = UINT(WINAPI*)(UINT);

// CREATE_WAITABLE_TIMER_HIGH_RESOLUTION (winbase.h 0x00000002). Spelled out so
// the build does not depend on the MinGW headers carrying it.
constexpr DWORD kCreateWaitableTimerHighResolution = 0x00000002;

std::atomic<TimePeriod_t> g_timeBeginPeriod{nullptr};
std::atomic<TimePeriod_t> g_timeEndPeriod{nullptr};

bool g_timerPeriodHeld = false;  // input thread only

void BeginContactTiming() {
    if (g_timerPeriodHeld) {
        return;
    }

    const TimePeriod_t begin =
        g_timeBeginPeriod.load(std::memory_order_relaxed);
    if (!begin || begin(kContactTimerPeriodMs) != 0) {
        // No 1 ms quantum: the release still works on the default tick, only
        // later. The flag stays clear so no unmatched end is sent.
        return;
    }

    g_timerPeriodHeld = true;
}

void EndContactTiming() {
    if (!g_timerPeriodHeld) {
        return;
    }

    g_timerPeriodHeld = false;
    const TimePeriod_t end = g_timeEndPeriod.load(std::memory_order_relaxed);
    if (end) {
        end(kContactTimerPeriodMs);
    }
}

// The deadline timer, independent of the 15.6 ms WM_TIMER quantum. Created on
// the input thread and closed by it, and never armed here: the loop's planner
// arms it for the one deadline it has and disarms it when that is served, so a
// loaded mod with nothing happening costs no timer at all. nullptr means "use
// the WM_TIMER fallback", which is coarser but still serves every deadline.
HANDLE CreateDeadlineTimer() {
    return CreateWaitableTimerExW(nullptr, nullptr,
                                 kCreateWaitableTimerHighResolution,
                                 TIMER_ALL_ACCESS);
}

// Arms the one-shot for `delayMs` from now. The relative due time is in 100 ns
// units and the timer is not periodic: a periodic timer is exactly the 5 ms
// wake this design removes.
bool ArmDeadlineTimer(HANDLE timer, uint64_t delayMs) {
    LARGE_INTEGER due{};
    due.QuadPart = -static_cast<LONGLONG>(delayMs) * 10000;
    return SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE) != FALSE;
}

void DisarmDeadlineTimer(HANDLE timer) {
    CancelWaitableTimer(timer);
}

// ---------------------------------------------------------------------------
// The transition watch
//
// The open/close transition is a WinEvent now (EVENT_OBJECT_CLOAKED /
// UNCLOAKED on the CoreWindow), not a 20 ms poll of the cloaked attribute. The
// event is the signal; this watch is the net under it, and it is the only thing
// that can bring the cloaked query back. It is open in exactly two situations:
//
// - a transition this mod asked for (a show or a dismiss request) has not been
//   observed yet: `want` names the direction, and the window ends when the
//   matching event lands or after kVisibilityGraceMs, whichever is first. A
//   missed event then still lands the state instead of leaving it stuck.
// - a transition has just been observed: every observation keeps
//   kVisibilitySettleWatchMs of slow polling, because the unhealed-panel repair
//   runs after the open's measurement turn, and that turn is posted.
//
// Nothing here polls while neither holds: an idle mod has no transition to
// watch.
// ---------------------------------------------------------------------------
enum : uint32_t {
    kVisibilityWantNone = 0,
    kVisibilityWantOpen = 1,
    kVisibilityWantClosed = 2,
};

// Input thread only: the requests and the service both run there, and the
// WinEvent callback touches none of them.
uint64_t g_visibilityPollUntil = 0;   // tick; 0 = no watch
uint32_t g_visibilityWant = kVisibilityWantNone;
uint64_t g_lastVisibilityPollTick = 0;

// Opens the watch for a direction this mod asked for. The later deadline wins,
// so two requests inside one window cannot shorten each other. The slow cadence
// starts at the request: the event is the fast path, and an immediate poll would
// race it and log a fallback that "saw" the change only because it ran first.
void NoteVisibilityTransition(uint32_t want) {
    g_visibilityWant = want;
    const uint64_t now = GetTickCount64();
    const uint64_t until = now + kVisibilityGraceMs;
    if (until > g_visibilityPollUntil) {
        g_visibilityPollUntil = until;
    }
    g_lastVisibilityPollTick = now;
}

// GetTickCount64 advances on the system tick (15.6 ms here), so it cannot tell
// an 18 ms silence from a 31 ms one: an 18 ms threshold read as 31 and the
// release stayed where it was. The contact silence that ends a gesture is
// measured with QueryPerformanceCounter; the coarse tick stays for the velocity
// windows and the suppression deadlines, which do not need better than a tick.
uint64_t QpcMsNow() {
    static const uint64_t frequency = []() {
        LARGE_INTEGER value{};
        QueryPerformanceFrequency(&value);
        return value.QuadPart > 0 ? static_cast<uint64_t>(value.QuadPart)
                                  : 1;
    }();

    LARGE_INTEGER counter{};
    QueryPerformanceCounter(&counter);
    return static_cast<uint64_t>(counter.QuadPart) * 1000 / frequency;
}

enum class DismissAxis : int32_t {
    Left = 0,
    Top = 1,
    Right = 2,
    Bottom = 3,
};

bool IsHorizontalAxis(int32_t axis) {
    return axis == static_cast<int32_t>(DismissAxis::Left) ||
           axis == static_cast<int32_t>(DismissAxis::Right);
}

// The translation the composition expression applies at Progress 0: the
// direction the panel leaves along and the root extent it travels. Setup bakes
// these in, and the event-driven refresh rewrites them when the taskbar moves.
void AxisTranslation(int32_t axis, float width, float height, float& dirX,
                     float& dirY, float& distance) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            dirX = -1.0f;
            dirY = 0.0f;
            distance = width;
            break;
        case DismissAxis::Top:
            dirX = 0.0f;
            dirY = -1.0f;
            distance = height;
            break;
        case DismissAxis::Right:
            dirX = 1.0f;
            dirY = 0.0f;
            distance = width;
            break;
        case DismissAxis::Bottom:
            dirX = 0.0f;
            dirY = 1.0f;
            distance = height;
            break;
    }
}

std::atomic<bool> g_disabled{false};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_stopDismissCallback{false};

// ---------------------------------------------------------------------------
// Module lifetime
//
// The image is not pinned. Windhawk removes the hooks, calls Wh_ModUninit and
// frees the image, so this role must leave no thread and no reachable callback
// behind before that call returns. A fresh DLL starts clean: there is no
// re-init state to reset and no process instance guard to carry across
// activations.
//
// Unload order:
//   1. BeforeUninit closes admission, withdraws the peer, stops the input
//      thread and joins it INFINITE. The unload never runs on that thread.
//   2. Uninit runs one UI teardown turn: revoke the XAML handlers and the
//      settle batch, stop the animations, restore resting progress and neutral
//      translation, drop the UI-owned objects. The turn signals completion and
//      the wait is on that signal, not on a registration count.
//   3. The counted module entries (a batch completion already dispatched) are
//      drained through their event. Only then are the IPC view and the handles
//      closed.
//
// A failed revoke is not a successful unload: the wait stays in teardown until
// the owning source is demonstrably gone. A diagnostic repeats; it never
// returns early.
// ---------------------------------------------------------------------------

// Set at the end of RoleInit. A fresh image starts false; every later callback
// returns at once until the role actually initialised.
bool g_initialised = false;

// Signalled when the counted entry count reaches zero. Created before the first
// lease, waited on at unload. See ModuleEntryLease.
HANDLE g_moduleEntriesGone = nullptr;

// The id of the input thread, published by the thread itself. The unload checks
// it before joining so a teardown that somehow ran on that thread cannot wait
// on itself.
std::atomic<DWORD> g_hidThreadId{0};

// ---------------------------------------------------------------------------
// Counted entries into module code
//
// The compositor and the dispatcher call into this module on threads and turns
// the unload does not own: a settle batch's `Completed` handler and every UI turn
// that touches module state. Each one takes a lease when it is submitted, before
// it can run, and the lease drops the count when the last reference to it is
// gone: a queued handler holds one until its delegate is revoked or its turn is
// destroyed, and a handler that is already running holds one until it returns.
//
// `Wh_ModUninit` revokes first, so no new entry can start, and then waits for the
// count to reach zero. That is the invariant: every entry into module code from
// the compositor or the dispatcher is counted from before it can run.
// ---------------------------------------------------------------------------
std::atomic<int> g_moduleEntryCount{0};

class ModuleEntryLease {
  public:
    ModuleEntryLease() {
        g_moduleEntryCount.fetch_add(1, std::memory_order_seq_cst);
    }
    ~ModuleEntryLease() {
        if (g_moduleEntryCount.fetch_sub(1, std::memory_order_acq_rel) == 1 &&
            g_moduleEntriesGone) {
            // The last entry is leaving. The unload waits on this event instead
            // of polling the count, so the release is observable at once.
            SetEvent(g_moduleEntriesGone);
        }
    }

    ModuleEntryLease(const ModuleEntryLease&) = delete;
    ModuleEntryLease& operator=(const ModuleEntryLease&) = delete;
};

// ---------------------------------------------------------------------------
// The settle style setting
//
// The setting stores its display label, so the label the user picks is the
// value the code compares: exactly "Windows 10" selects the spring, and every
// other value (including the default "Windows 11") selects the Fluent curve. A
// missing or unrecognized value therefore lands on the default behaviour
// instead of inventing a third one.
// ---------------------------------------------------------------------------
enum class SettleStyle : int32_t {
    Win11 = 0,
    Win10 = 1,
};

constexpr wchar_t kAnimationStyleWin10[] = L"Windows 10";

// Read once at init and once per settings change; every settle reads this
// atomic instead of the setting string.
std::atomic<SettleStyle> g_settleStyle{SettleStyle::Win11};

const wchar_t* SettleStyleName(SettleStyle style) {
    return style == SettleStyle::Win10 ? L"win10" : L"win11";
}

// Reads the style setting into the atomic. Wh_ModInit calls it once and
// Wh_ModSettingsChanged calls it again, so no settle pays for a string lookup.
void LoadAnimationStyle() {
    PCWSTR value = Wh_GetStringSetting(L"AnimationStyle");
    const SettleStyle style =
        wcscmp(value, kAnimationStyleWin10) == 0 ? SettleStyle::Win10
                                                 : SettleStyle::Win11;
    TraceLog(L"settle style selected=%s setting=%s", SettleStyleName(style),
            value);
    Wh_FreeStringSetting(value);
    g_settleStyle.store(style, std::memory_order_relaxed);
}

// The percentage from the mod settings: the Windows 10 spring's initial
// velocity as a percentage of the finger's release speed. Wh_ModInit and
// Wh_ModSettingsChanged write it, every spring settle reads it once; no setting
// lookup runs per settle.
std::atomic<int> g_gestureStartSpeedPercent{kGestureStartSpeedDefault};

// Reads the setting into the atomic, clamped. Wh_ModInit calls it once and
// Wh_ModSettingsChanged calls it again, so no settle pays for a setting lookup.
void LoadGestureStartSpeed() {
    const int percent =
        std::clamp(Wh_GetIntSetting(L"GestureStartSpeedPercent"),
                   kGestureStartSpeedMin, kGestureStartSpeedMax);
    g_gestureStartSpeedPercent.store(percent, std::memory_order_relaxed);
    TraceLog(L"settle start speed=%d%%", percent);
}

// ---------------------------------------------------------------------------
// Composition state
//
// Progress is the only animated scalar; Distance is the static window width
// or height. Both the property set and the visual are agile Composition
// objects, so the input thread and the settle batch callback can use them.
// The mutex protects the projection pointers and the check-then-update
// sequence against teardown; it is never held across a wait.
// ---------------------------------------------------------------------------

// `Progress` 1 is the menu at rest on screen, 0 is it fully slid out.
constexpr float kProgressRest = 1.0f;

std::mutex g_compositionMutex;
NO_DESTROY wuc::Compositor g_compositor{nullptr};
NO_DESTROY wuc::CompositionPropertySet g_state{nullptr};
NO_DESTROY wuc::ExpressionAnimation g_translationExpression{nullptr};
NO_DESTROY wuc::Visual g_visual{nullptr};
NO_DESTROY wuc::CompositionScopedBatch g_dismissBatch{nullptr};
// The Completed registration of that batch, kept so the handler can be removed
// before the module goes away. Written under g_compositionMutex. A revoke that
// throws keeps both the batch and the token: the token is the only handle on a
// handler that can still run, and the unload keeps waiting for it.
winrt::event_token g_dismissBatchToken{};
// The counted entry of that registration. The handler's closure holds a second
// reference while it is queued or running, so the count falls only when the
// delegate is revoked and no handler is left inside module code. Written under
// g_compositionMutex.
std::shared_ptr<ModuleEntryLease> g_settleLease;

// ---------------------------------------------------------------------------
// The panel controller
//
// One object owns the panel's motion: the scalar the composition expression
// reads, the target the current motion is heading for, the last measured or
// committed speed in Progress per millisecond, and the phase that says which
// motion owns the value. The animation record (samples, start, duration) and the
// coast's line are the controller's evidence: the base value is only a target
// while a motion runs, and `TakeProgressNow` is the one reader that turns that
// evidence into where the panel actually is.
//
// The four intents are the only way an input adapter changes the panel. M1 keeps
// the touch adapter as the only caller; the non-touch paths still use their old
// functions, which write the same controller so the two cannot disagree.
// ---------------------------------------------------------------------------
enum class PanelPhase : int32_t {
    Idle = 0,    // no motion: the value is the position
    Drag = 1,    // a contact or a direct write owns the value
    Coast = 2,   // the 200 ms continuation between reports
    Settle = 3,  // a sampled curve is playing
};

const wchar_t* PanelPhaseName(PanelPhase phase) {
    switch (phase) {
        case PanelPhase::Drag:
            return L"drag";
        case PanelPhase::Coast:
            return L"coast";
        case PanelPhase::Settle:
            return L"settle";
        default:
            return L"idle";
    }
}

struct PanelController {
    // The value written to the property set. While a settle plays this is the
    // animation's target, not its position; only TakeProgressNow may read it as
    // a position, and only when no motion is recorded.
    std::atomic<float> progress{kProgressRest};
    std::atomic<bool> coasting{false};
    // Guarded by g_compositionMutex, like the animation record below.
    float target = kProgressRest;
    float velocity = 0.0f;  // Progress per millisecond, signed along the axis
    PanelPhase phase = PanelPhase::Idle;
    float samples[kSettleMaxFrames] = {};
    int frames = 0;
    uint64_t startQpc = 0;
    uint64_t durationMs = 0;
    uint64_t coastStartQpc = 0;
    float coastFrom = 1.0f;
    float coastTo = 1.0f;

    // The four intents. Begin and Move are the contact's; End is the lift with
    // the measured speed; Commit is a key or click that targets a direction with
    // zero seam speed. Every one starts from the value the panel has now.
    void Begin(const float* position);
    bool Move(float position);
    void End(float to, float endVelocity);
    void Commit(float to);
};

PanelController g_panel;

NO_DESTROY wux::FrameworkElement g_root{nullptr};  // UI thread only
// Guarded by g_compositionMutex: Setup writes it on the UI thread, the input
// thread copies it into a local before posting, and the unload revoke copies it
// the same way. A CoreDispatcher copy is a COM reference-counted pointer, so it
// must never be read on one thread while another overwrites it.
NO_DESTROY wucore::CoreDispatcher g_dispatcher{nullptr};
std::atomic<DWORD> g_uiThreadId{0};
std::atomic<uint32_t> g_setupGeneration{0};

// The settled-trail watch. The open transition re-measures the panel edge once,
// but that measurement can land while the shell's open storyboard is still
// growing the frame into place (measured on a bottom taskbar: panel top -12 DIP
// where the settled panel starts at 200). The layout event is the only signal
// that says the panel has been placed, so the watch counts layout passes and
// corrects the trail on the second one, then detaches itself.
//
// `g_layoutTicksLeft` is 0 when no watch is armed, which is also what the close
// teardown leaves behind; every field here is UI thread only.
winrt::event_token g_layoutToken{};
uint32_t g_layoutTicksLeft = 0;
// The element the layout token was registered on, so its revoke does not depend
// on g_root still pointing at it.
NO_DESTROY wux::FrameworkElement g_layoutRoot{nullptr};
// Identity of the armed watch. The callback captures the value it was created
// with and acts only while that value is still current, so a handler whose
// revoke failed can never advance or revoke a watch armed later. The mint skips
// zero so "no watch" is a distinct value.
uint32_t g_layoutWatchSerial = 0;
uint32_t g_layoutWatchMint = 0;

// ---------------------------------------------------------------------------
// The scrolling regions of the open menu
//
// A vertical dismiss and a list's own scroll are the same drag, so the dismiss
// needs to know whether the element *under the press point* can still scroll
// away from it. One cached element was not enough: the menu scrolls in more than
// one place (the app list, and the tile grid when its tiles overflow), and a
// single-element gate answered "not on the scrolling content" for every press in
// the other, which let a tile-grid drag start the dismiss. The survey therefore
// enumerates every vertical ScrollViewer under the root once per open, keeps each
// one's screen rect, offset and extent, and the input thread picks the innermost
// region that contains the press. Every scrolling area is gated by the same rule,
// and no coordinate special case exists.
//
// The numbers of one region belong together, so the whole survey is one POD
// behind a mutex instead of a set of atomics: a torn rect/offset pair could allow
// or reject the wrong press. The lock is taken once per down on the input side
// and only when the survey is measured or scrolled on the UI side, so it is
// never contended.
//
// "Nothing scrollable here" and "not measured yet" were once the same
// `valid == false` cache, and the gate read both as "cannot decide", so a panel
// with genuinely nothing scrollable could never be dismissed from inside. They
// are separate states:
//
// - Unknown: no measurement that speaks for this open — nothing measured yet, a
//   measurement that failed, or the open-time pass whose rects belong to a window
//   the shell is still moving. The gate defers while the press is inside the
//   panel and lets a press outside it pass.
// - Nothing: measured after the shell settled, and nothing in the panel can
//   scroll vertically. Every vertical press belongs to the dismiss.
// - Scrollable: measured after the shell settled; `regions` holds every
//   scrollable area. A press inside one may scroll, so that region's
//   end-of-list test decides; a press inside none of them belongs to the
//   dismiss.
// ---------------------------------------------------------------------------
enum class ScrollerState {
    Unknown = 0,
    Nothing = 1,
    Scrollable = 2,
};

// How many scrollable regions one survey keeps. The menu has one vertical
// scroller in the app list and one in the tile grid (plus their non-scrolling
// wrappers, which the survey rejects); the bound is what keeps the array in the
// snapshot fixed and the copy to the input thread cheap. A survey that finds
// more keeps the deepest ones and says so once.
constexpr int kMaxScrollRegions = 6;
constexpr int kScrollerLabelChars = 32;

// One scrollable region the survey found, in screen DIP. The input thread tests
// a press against these rects and gates the dismiss on the innermost one that
// contains it, so every scrolling area is gated by the same rule instead of by
// whichever single element was measured.
struct ScrollerRegion {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float verticalOffset = 0.0f;
    float scrollableHeight = 0.0f;
    uint32_t depth = 0;  // visual-tree depth, the innermost pick
    int watchSlot = -1;  // the UI-thread watch that refreshes this entry, -1 none
    // The element's own name when it has one, else its nearest named ancestor's,
    // else its type. The deferral line prints it so a log says which region
    // took the drag, without the input thread holding a XAML object.
    wchar_t label[kScrollerLabelChars] = {};
};

// The whole survey, not one element. `state` is the survey's own answer:
// Unknown = no whole measurement speaks for this open (defer inside the panel),
// Nothing = measured and nothing here scrolls vertically (allow the dismiss),
// Scrollable = `regions[0..count)` are the scrollable areas (a press inside one
// is gated by that region's offsets, a press outside all of them is not).
struct ScrollerCache {
    ScrollerState state = ScrollerState::Unknown;
    uint32_t generation = 0;  // the open epoch this survey was measured in
    // The layout identity token the rects were mapped in. A resize inside one
    // open does not move the epoch, so the generation alone cannot tell that a
    // rect now describes a panel the shell has already moved.
    uint32_t layoutVersion = 0;
    int count = 0;
    ScrollerRegion regions[kMaxScrollRegions];
};

std::mutex g_scrollerMutex;
ScrollerCache g_scroller;

// UI thread only. One watch per committed region. Two roles per slot, on
// purpose: `owner` is the object the ViewChanged token was registered on — it
// outlives a revoke that threw, so the token can be retried and its slot is not
// reused — while `viewer` is the object the committed region was measured from,
// the only one whose handler may write that region. One variable per slot made
// the two the same object, and then a failed revoke let the old viewer keep
// writing into a region another viewer's rect had just replaced.
struct ScrollerWatch {
    wuxc::ScrollViewer owner{nullptr};
    wuxc::ScrollViewer viewer{nullptr};
    winrt::event_token token{};
};
NO_DESTROY std::optional<std::array<ScrollerWatch, kMaxScrollRegions>>
    g_scrollerWatches;

// UI thread only. The watches are emplaced on the first committed region and
// reset by the unload teardown, so the process-exit path never runs their
// WinRT destructors.
std::array<ScrollerWatch, kMaxScrollRegions>& ScrollerWatches() {
    if (!g_scrollerWatches) {
        g_scrollerWatches.emplace();
    }
    return *g_scrollerWatches;
}
// The rect lines speak once per open per region, not once per measurement: the
// open transition and the settled pass both measure the same elements, and only
// a rect that actually moved is worth a second line. The `scroller none` line is
// the settled pass telling the log that nothing here scrolls vertically.
bool g_scrollerLogged = false;
bool g_scrollerNoneLogged = false;
bool g_scrollerOverflowLogged = false;
bool g_scrollerRegionLogged[kMaxScrollRegions] = {};
float g_scrollerLoggedRect[kMaxScrollRegions][4] = {};
// Coarse tick of the last `dismiss deferred to scroll` line. The digitizer
// reports at ~170 Hz, so the rejection is rate limited; input thread only.
uint64_t g_scrollerRejectLogTick = 0;
// The same latch for `dismiss skipped band tail`: one line per window while a
// contact stays the tail of an opening pull; input thread only.
uint64_t g_bandTailLogTick = 0;
// The tick of the report that just cancelled a close for the contact it carried.
// Its admission names the cancel instead of the unhealed state the aborted close
// leaves behind. Input thread only.
uint64_t g_closeCancelledTick = 0;

winrt::event_token g_loadedToken{};
winrt::event_token g_sizeToken{};
// The tokens belong to the objects they were registered on, and a Setup that
// never finished leaves g_root null while a Loaded handler is still attached.
// These keep the exact element so the revoke can run unconditionally.
NO_DESTROY wux::FrameworkElement g_loadedRoot{nullptr};   // UI thread only
NO_DESTROY wux::FrameworkElement g_sizeRoot{nullptr};     // UI thread only

std::atomic<bool> g_dismissSettling{false};
std::atomic<uint32_t> g_dismissSerial{0};
// Set whenever this mod starts a close itself: the committed dismiss and the
// cancelled reveal both set it when their settle starts. ServiceRootVisibility
// must not read the still-open window as a reopen while the close is in flight.
// 0 means no close of this mod is pending.
std::atomic<uint64_t> g_suppressRestoreUntil{0};

// ---------------------------------------------------------------------------
// The gesture close
//
// Both close paths end in the same place: one executor dismiss at the end of the
// settle. The bridge runs the shell's Dismiss on the launcher thread, exactly as
// it does for a keyboard close, and the next Windows press opens the menu. The
// settle runs first, so the panel is off screen when the window hides.
//
// A new pull that starts during the settle cancels the close: the armed dismiss
// is dropped before the bridge is asked, so no Dismiss runs and the menu stays
// open. The pull generation is sampled at the release and checked once when the
// dismiss would run; the cancel is published the moment the pull begins, not at
// the dismiss deadline. This is one scheduled dismiss, not a request file.
// ---------------------------------------------------------------------------
enum class CloseKind {
    None,
    Dismiss,
    RevealCancel,
};

// A pull is a contact that starts in the taskbar edge band and becomes a reveal
// candidate (BeginEdgeCandidate). The counter names the pull generation: a close
// released under one generation is dropped when a later pull exists by the time
// its dismiss would run.
uint32_t g_pullSerial = 0;  // input thread only

// The one dismiss slot. `g_pendingTapPull` is the pull generation sampled when
// the dismiss was armed: a newer pull replaces it, and the runner drops a slot
// whose generation no longer matches. Zero in `g_pendingTapTick` means the slot
// is empty, so no separate dismiss identity is needed.
uint64_t g_pendingTapTick = 0;  // input thread only, 0 = none
CloseKind g_pendingTapKind = CloseKind::None;
uint32_t g_pendingTapPull = 0;

// Reports which of the two close schedules a cancel actually dropped: the local
// gesture deadline (`g_pendingTapTick`) and the executor dismiss command. The
// name says what it counts; `CancelCloseSchedules` is the one caller that acts on
// both, and the local-slot-only path is a separate operation.
struct CloseScheduleCancel {
    bool slot = false;
    bool command = false;
};

// Clearing one schedule and cancelling both are different operations: a landing
// must not publish an executor command, while a superseding action must drop
// both. `ClearLocalCloseTap` clears the local deadline alone and reports whether
// one was there; `CancelCloseSchedules` additionally cancels the executor
// command and reports both.
bool ClearLocalCloseTap();
CloseScheduleCancel CancelCloseSchedules(const wchar_t* reason);

// Arms the dismiss for a close that has just started its settle. The deadline is
// the end of that settle.
void ScheduleCloseTap(CloseKind kind, uint64_t delayMs) {
    g_pendingTapTick = GetTickCount64() + delayMs;
    g_pendingTapKind = kind;
    g_pendingTapPull = g_pullSerial;
}

std::atomic<HWND> g_coreHwnd{nullptr};
// Resolved in Wh_ModInit. A runtime resolve keeps the mod loadable without
// dwmapi.dll in the import table.
std::atomic<DwmGetWindowAttribute_t> g_dwmGetWindowAttribute{nullptr};

// ---------------------------------------------------------------------------
// The cloak event
//
// The menu closes by cloaking its CoreWindow, not by destroying or hiding it,
// and the cloaked attribute is the one reliable open/closed signal. It used to
// be read every 20 ms for the whole life of the process; it is now pushed:
// EVENT_OBJECT_CLOAKED / EVENT_OBJECT_UNCLOAKED for this process's CoreWindow,
// delivered out of context to the input thread that installed the hook.
//
// The callback is a filter and a signal, nothing else. It takes no lock, reads
// no XAML, queries no DWM and writes no trace: it compares the HWND against the
// one the input thread recorded and sets an auto-reset event. Everything it
// could have done runs on the input thread after its wait returns, where the
// state machine and its locks live. An event that arrives for any other window
// of this process is dropped here, before it can become a wake.
// ---------------------------------------------------------------------------
std::atomic<HANDLE> g_cloakEvent{nullptr};  // owned by the input thread
HWINEVENTHOOK g_cloakHook = nullptr;        // owned by the input thread

void CALLBACK CloakWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event != EVENT_OBJECT_CLOAKED && event != EVENT_OBJECT_UNCLOAKED) {
        return;
    }
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    HWND core = g_coreHwnd.load(std::memory_order_relaxed);
    if (!core || hwnd != core) {
        return;
    }
    HANDLE signal = g_cloakEvent.load(std::memory_order_relaxed);
    if (signal) {
        SetEvent(signal);  // the whole callback
    }
}

// Filled in at setup and on every PrepareRootElement.
int32_t g_dismissAxis = static_cast<int32_t>(DismissAxis::Left);
float g_dismissDistance = 0.0f;

// Published by the UI thread and read by the input thread. No XAML object is
// shared: only plain values.
//
// Readiness answers "is there a property set and an expression to drive right
// now". Setup raises it, ResetComposition clears it when the root, its property
// set and its expression are dropped. A historical "was set up once" latch cannot
// answer that: after a ResetRenderer the old flag stayed set and a gesture could
// start against a renderer that no longer existed. Nothing compares renderer
// identities, so this is a readiness flag and not a generation.
std::atomic<bool> g_rendererReady{false};
// Set when a scalar write failed after an earlier one had already landed. The
// property set then carries a mixture of two layouts, and no snapshot can
// describe it: gestures stay refused until a full apply rewrites all three
// scalars. Cleared by that apply and by the renderer teardown.
std::atomic<bool> g_rendererInvalid{false};

bool RendererReady() {
    return g_rendererReady.load(std::memory_order_acquire) &&
           !g_rendererInvalid.load(std::memory_order_acquire);
}

// Signalled by the UI thread whenever RendererReady() can change: the root is
// attached, torn down, or invalidated by a failed scalar write. The input thread
// withdraws the peer on this event and republishes when a usable replacement is
// attached, instead of testing the raw ready bit once per activation. The event
// is how round 3's refactor stays event-driven; the loop's own check is only a
// safety net for a missed signal.
HANDLE g_rendererEvent = nullptr;

void SignalRendererChanged() {
    if (g_rendererEvent) {
        SetEvent(g_rendererEvent);
    }
}

void SetRendererReady(bool ready) {
    g_rendererReady.store(ready, std::memory_order_release);
    SignalRendererChanged();
}

void SetRendererInvalid(bool invalid) {
    g_rendererInvalid.store(invalid, std::memory_order_release);
    SignalRendererChanged();
}
// Diagnostics for a dismiss that did not start: one line per second, not per
// report, because a deferred scroll produces a sample every ~6 ms.
uint64_t g_skipLogTick = 0;
// The unhealed-open diagnostic: the window is uncloaked with the panel off it,
// and no animation owns Progress. Once per second while it stands.
uint64_t g_unhealedLogTick = 0;
std::atomic<int64_t> g_winRectDip[4]{};  // left, top, right, bottom

// ---------------------------------------------------------------------------
// The gesture geometry snapshot
//
// Placing a press, mapping the reveal follow and planning a settle all need the
// same measurement: which side the taskbar is on, how far the root travels, where
// the visible panel is and which of its edges meets the finger. Published as
// separate atomics, a rejected measurement still wrote its rectangle, so a "keep
// the last good value" decision left a rect from the new measurement beside a
// trail from the old one, and the hit test and the follow then read different
// geometries. The rect and the trail could not even be compared: nothing said
// which measurement either belonged to.
//
// One plain struct carries the whole value behind a short writer lock: the UI
// thread is the only writer and takes `g_geometryLock` exclusively for one struct
// assignment; the input thread tries a shared acquisition per sample and answers
// its last accepted copy when the writer holds the lock. A seqlock alone was not
// enough here: its sequence numbers detect a change but do not legalize reading
// the plain struct while the writer assigns it, which is a data race under the
// C++ memory model however wide the fences are.
//
// `layout` names the whole layout the snapshot was taken in: the axis, the
// travel, the XAML client origin and the work area and monitor. `layoutVersion`
// changes whenever that identity changes, so a reader holding a cached copy can
// tell that it predates a resize and must ask the writer again instead of
// starting a gesture from an old rectangle.
//
// `generation` names the root/open epoch the measurement belongs to. Every
// recorded request and every published snapshot carries it, so a measurement
// recorded before an open cannot be published for the next one.
// ---------------------------------------------------------------------------
struct LayoutIdentity {
    int32_t axis = static_cast<int32_t>(DismissAxis::Left);
    float distance = 0.0f;  // the root extent the composition slides by, DIP
    float originX = 0.0f;   // XAML client origin, absolute screen DIP
    float originY = 0.0f;
    float workLeft = 0.0f;
    float workTop = 0.0f;
    float workRight = 0.0f;
    float workBottom = 0.0f;
    float monitorLeft = 0.0f;
    float monitorTop = 0.0f;
    float monitorRight = 0.0f;
    float monitorBottom = 0.0f;
};

struct GestureGeometry {
    uint32_t generation = 0;     // root/open epoch the measurement was taken in
    uint32_t layoutVersion = 0;  // identity token, bumped when `layout` changes
    LayoutIdentity layout;
    bool panel = false;          // left/top/right/bottom hold a real measurement
    float left = 0.0f;           // the visible panel, absolute screen DIP
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    // The strip this layout's taskbar reserves at the screen edge, extended by
    // one bar thickness toward the desktop, in absolute screen DIP along the
    // dismiss axis. The reveal candidate starts anywhere inside it, so the
    // band is the taskbar's own measurement (TaskbarProbe) and not a fixed
    // number of DIP from the screen edge: an auto-hidden bar keeps its 48 DIP
    // while reserving no work area at all. `band` is false before the first
    // probe; the candidate test then falls back to kEdgeBandDip.
    bool band = false;
    float bandLow = 0.0f;
    float bandHigh = 0.0f;

    int32_t axis() const { return layout.axis; }
    float distance() const { return layout.distance; }
};

// The one "same travel" policy. RefreshGestureGeometry's change test and the
// carry-over test in the publish path both ask this function, so a layout can
// never count as unchanged in one place and changed in the other.
constexpr float kLayoutDistanceEpsilonDip = 0.5f;

bool SameLayoutDistance(float a, float b) {
    return std::fabs(a - b) <= kLayoutDistanceEpsilonDip;
}

// The whole identity, not just the axis and the travel: the same extents on a
// different work area, taskbar thickness or cross-axis origin put the panel
// somewhere else, and a rect carried across that boundary is not this layout's
// panel.
bool SameLayoutIdentity(const LayoutIdentity& a, const LayoutIdentity& b) {
    return a.axis == b.axis && SameLayoutDistance(a.distance, b.distance) &&
           a.originX == b.originX && a.originY == b.originY &&
           a.workLeft == b.workLeft && a.workTop == b.workTop &&
           a.workRight == b.workRight && a.workBottom == b.workBottom &&
           a.monitorLeft == b.monitorLeft && a.monitorTop == b.monitorTop &&
           a.monitorRight == b.monitorRight &&
           a.monitorBottom == b.monitorBottom;
}

// Odd while g_geometry is being written is no longer needed: the writer holds
// this lock exclusively and a reader that cannot take it shared uses its cache.
SRWLOCK g_geometryLock = SRWLOCK_INIT;
// The identity token of the latest published layout. The writer raises it when
// the identity changes and stamps the snapshot with it; a reader compares its
// cached copy against it before starting a gesture.
std::atomic<uint32_t> g_layoutVersion{1};
GestureGeometry g_geometry;       // guarded by g_geometryLock, writer copy
GestureGeometry g_inputGeometry;  // input thread only: the last whole read

// The epoch a measurement belongs to: a root installed or dropped, or an open or
// close of the reused root. 0 is reserved for "no expectation", so the count
// starts at 1.
std::atomic<uint32_t> g_openGeneration{1};
// M3 bridge-intent and executor state. `g_intentOpenActive` holds the open
// intent's entry until the uncloak, so the first visible frame is the edge and
// the settle starts there. `g_pendingDismissExec` is the scheduled dismiss the
// controller can cancel on a takeover.
std::atomic<bool> g_intentOpenActive{false};
std::atomic<float> g_intentOpenEntry{0.0f};
std::atomic<LONG64> g_pendingDismissExec{0};
// The GetTickCount64 deadline the bridge was told to run the dismiss at. The ack
// expiry is based on it, not on the request time: the bridge arms its timer for
// this deadline, so an ack cannot be expected before it (see
// CheckExecAckTimeout).
std::atomic<uint64_t> g_pendingDismissExecAt{0};
std::atomic<LONG64> g_pendingShowExec{0};
std::atomic<uint64_t> g_pendingShowUntil{0};
LONG64 g_lastIntentSeq = 0;  // input thread only; seeded by OpenIpcChannel
// An open intent that cancelled a close whose window was still on screen. The
// window keeps showing (the local dismiss slot and the executor command are
// dropped), but the dismiss can already be inside the shell's Dismiss call when
// the press lands. That close still lands, so `win close observed` reissues the
// same open once.
//
// The ticket is bound to the close it belongs to, not to "a landing": openTick is
// the low 32 bits of the tick the menu's open record published, captured when the
// cancel ran; dismissSeq is the executor command it revoked (0 when the close had
// none yet). A landing for a different menu can therefore never serve it. Set by
// the open-intent cancel path, cleared by a new close, an external takeover, a
// terminal answer for its sequence, or the landing that consumes it.
struct CloseCancelTicket {
    bool pending = false;
    uint32_t openTick = 0;
    LONG64 dismissSeq = 0;
    // The instant after which the revoked dismiss can no longer land: its
    // scheduled deadline plus the publisher's budget plus the confirmation
    // probe. A landing after that is not this press's close — the command either
    // ran (and its own landing or confirmation has already spoken) or it was
    // cancelled before entering the shell.
    uint64_t expireAt = 0;
};
CloseCancelTicket g_intentCancel;  // input thread only

// The dismiss whose primitive returned but whose 300 ms visibility probe has not
// answered yet. `executed` and `confirmed` are different outcomes: an executed
// ack says only that Dismiss returned. This slot keeps the command identity until
// the probe (or a landing) closes it, so an unconfirmed dismiss can still be
// repaired after the immediate ack cleared the pending slot.
LONG64 g_pendingDismissConfirm = 0;  // input thread only
// The open generation each pending dismiss was stamped with: the section's
// `open` value, the same one the bridge checks before it calls the launcher. A
// landing may claim the close only while that generation is still the current
// one; a retained confirmation from an older menu must not land a later hide.
uint64_t g_pendingDismissExecGeneration = 0;     // input thread only
uint64_t g_pendingDismissConfirmGeneration = 0;  // input thread only
// Set while a confirmation is retained because the cloak disagreed or could not
// answer. The bound is the landing's wait for the transition; no landing can
// claim the command after it, and the ordinary terminal repair runs instead.
uint64_t g_pendingDismissConfirmUntil = 0;  // input thread only
bool g_pendingDismissConfirmRetained = false;  // input thread only

// The confirmation slot and everything bound to it. The exec slot keeps its own
// generation; a landing checks the two separately.
void ForgetConfirmedDismiss() {
    g_pendingDismissConfirm = 0;
    g_pendingDismissConfirmGeneration = 0;
    g_pendingDismissConfirmUntil = 0;
    g_pendingDismissConfirmRetained = false;
}

// Both halves of a dismiss: the command may still be queued at the executor and
// the confirmation may still be owed. Used where the close is revoked as a whole
// (a new close, an external takeover, an open-intent cancel, the ack timeout).
void ForgetPendingDismiss() {
    g_pendingDismissExec.store(0, std::memory_order_relaxed);
    g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
    g_pendingDismissExecGeneration = 0;
    ForgetConfirmedDismiss();
}

float TrailEdge(int32_t axis, float left, float top, float right, float bottom);
float GeometryTrailDip(const GestureGeometry& geometry);
float FallbackTrailDip(int32_t axis, float distance);
bool PanelContainsPress(const GestureGeometry& geometry, float x, float y);
GestureGeometry ReadGestureGeometry(bool waitForWriter = false);
bool GeometryLayoutCurrent(const GestureGeometry& geometry);
// The snapshot itself (one struct assignment under the writer lock, no syscall),
// the window-bounds diagnostic, and the two together for the paths that are not
// inside a commit section.
void StoreGestureGeometry(const GestureGeometry& candidate);
void PublishWindowRectDip();
void PublishGestureGeometry(const GestureGeometry& candidate);

// Input thread → UI thread: a live contact drives the panel edge right now. A
// request recorded on an idle panel must not move that edge under a finger that
// started in between, so the measurement re-reads this when its turn runs.
std::atomic<bool> g_panelOwned{false};

// The layout-application reservation. The UI thread announces one whole apply
// (renderer scalars, panel rect, scroller) with this counter and re-reads
// ownership while it is raised; a gesture admission claims the panel and then
// re-reads the counter. Both sides use seq_cst, so at least one of them sees the
// other: either the apply defers whole, or the admission waits for the next
// report. `g_panelOwned` alone left the window between "no owner" and the first
// scalar write open, and a finger that claimed it there drove a panel whose
// axis and travel were still being rewritten.
std::atomic<uint32_t> g_layoutApplying{0};

// UI thread. Raises the reservation and answers whether no contact owns the
// panel. On false the reservation is already released and the caller must defer
// the whole change.
class LayoutApplyGuard {
  public:
    LayoutApplyGuard() = default;
    LayoutApplyGuard(const LayoutApplyGuard&) = delete;
    LayoutApplyGuard& operator=(const LayoutApplyGuard&) = delete;

    ~LayoutApplyGuard() {
        if (held_) {
            g_layoutApplying.fetch_sub(1, std::memory_order_seq_cst);
        }
    }

    bool Begin() {
        g_layoutApplying.fetch_add(1, std::memory_order_seq_cst);
        if (g_panelOwned.load(std::memory_order_seq_cst)) {
            g_layoutApplying.fetch_sub(1, std::memory_order_seq_cst);
            return false;
        }
        held_ = true;
        return true;
    }

  private:
    bool held_ = false;
};

// ---------------------------------------------------------------------------
// The settled-measurement request
//
// The open only says "the layout is about to move": the panel's final place and
// the list's final extent arrive at the second LayoutUpdated pass, and the first
// pass usually precedes them. One request is therefore recorded when the menu
// opens and when the system announces a work-area or display change, and two
// triggers can complete it — the layout watch's second pass and a deadline,
// whichever comes first.
//
// Both triggers used to run their own copy of the work, the trail-idle decision
// was frozen when it was queued, and neither checked the state when it ran, so a
// stale post could measure a menu that had already closed. One request now does
// both: it carries a serial that a newer request replaces, the root/open epoch it
// was recorded in and the trail-idle decision; the turn that runs it re-reads the
// epoch, whether a root is attached and whether a gesture owns the panel, and the
// first trigger to run ends the request so the other finds nothing to do.
//
// The lock is taken once per open and once per deadline, never per report.
//
// The same mutex is the commit lock. The request identity, the open epoch and the
// write of the thing the request measured are one critical section: a publish
// validates `{request, epoch}` under this lock and writes the snapshot or the
// scroller cache before releasing it, and the epoch bump takes the lock too, so a
// close that advances the epoch cannot slip in between the check and the write.
// The slow XAML measurement stays outside it: the turn builds its candidate
// first, then commits. Reader-side tags alone were rejected because the input
// thread has no better value to fall back on when a per-sample read is stale, and
// because the two writers would still have to agree on which copy is obsolete;
// one short writer section needs no new consumer rule.
// ---------------------------------------------------------------------------
struct SettledMeasureRequest {
    // A 64-bit serial that skips 0, so a delayed cleanup can never match a reused
    // id and 0 keeps meaning "no request". The generation is part of the identity:
    // an operation that ends or validates a request takes both.
    uint64_t id = 0;
    uint32_t generation = 0;  // the epoch it was recorded in
    bool trailIdle = false;   // no contact owned the edge when it was recorded
    uint64_t dueTick = 0;     // its deadline, 0 once the deadline was taken
};

std::mutex g_measureMutex;
SettledMeasureRequest g_measureRequest;
uint64_t g_measureSerial = 0;  // guarded by g_measureMutex, input thread only

// The one epoch bump. The menu's open and close transitions and the renderer
// teardown call it, and it takes the commit lock, so a commit that passed its
// epoch check writes before the bump or fails the check after it.
uint32_t AdvanceOpenGeneration();
// Called with g_measureMutex held, from inside a commit section.
bool MeasureRequestLiveLocked(uint32_t generation, uint64_t id);
SettledMeasureRequest StartMeasureRequest(bool trailIdle);
bool TakeMeasureDeadline(uint64_t tickMs, SettledMeasureRequest& request);
// The one request-identity test and the one request-identity clear. Both take the
// full identity, so a turn that outlived its open matches neither.
bool MeasureRequestLive(uint32_t generation, uint64_t id);
bool EndMeasureRequest(uint32_t generation, uint64_t id);
void CancelMeasureRequest();
SettledMeasureRequest SnapshotMeasureRequest();

// A settled scroller pass can fail to commit: the layout tree threw, the depth
// limit cut the search short, or a contact owned the panel while it ran. The
// cache stays Unknown then, and the input thread re-posts the same settled pass
// after this delay while the same open is still on screen. `generation` binds
// the retry to that open, so a close or a new open drops it.
constexpr uint64_t kScrollerRetryMs = 100;
// A retry that keeps failing doubles its delay up to this bound. A traversal that
// cannot finish (a permanent depth-limit miss, a tree that stays unreadable) must
// not re-post at 10 Hz for the whole open, so the retries also stop after
// kScrollerRetryLimit attempts: the gate then stays Unknown for that open, the
// honest answer, and the next open starts the search again.
constexpr uint64_t kScrollerRetryBackoffMaxMs = 400;
constexpr uint32_t kScrollerRetryLimit = 3;
std::atomic<uint64_t> g_scrollerRetryAt{0};
std::atomic<uint32_t> g_scrollerRetryGeneration{0};

// UI thread only. Setup resets the open log; a failed mapping logs its fallback
// once per process so a missing panel does not flood the log every resize. A
// measurement rejected by the layout guard logs once per open: the generation it
// was rejected in is part of the latch, so a retried pass in the same open stays
// silent and the next open speaks again.
bool g_panelTrailLogged = false;
bool g_panelTrailFallbackLogged = false;
bool g_panelTrailRejectedLogged = false;
uint32_t g_panelTrailRejectedGeneration = 0;

HANDLE g_hidThread = nullptr;
std::atomic<HWND> g_hidHwnd{nullptr};
// The input thread's stop signal, created at init: the thread's wait watches it
// next to the idle tick and the two IPC events, so it leaves the loop even when
// the thread has not published g_hidHwnd yet. Set once, by the unload.
HANDLE g_hidStopEvent = nullptr;
// Set when a thread outlived its wait. Unload then leaves every resource in
// place and says so: the module cannot honestly claim it is gone while module
// code still runs on a thread Windhawk is about to unmap.
std::atomic<bool> g_hidThreadLeaked{false};

// The root hook's outcome. `ready` is set only when
// StartUI::App::PrepareRootElement resolved and its hook was installed;
// `refused` when the symbol, the signature or the hook failed. PublishPeerReady
// reads `ready`: a controller that cannot drive the panel must not advertise an
// interception service, or the bridge would swallow a native toggle for a menu
// that never animates.
std::atomic<bool> g_rootHookReady{false};
std::atomic<bool> g_rootHookRefused{false};

// One count per registered XAML callback (Loaded, SizeChanged, LayoutUpdated,
// ViewChanged). Every registration is revoked on the object it was made on; the
// count is the unload's proof that nothing is left to fire, and it is what lets
// the unload tell "nothing was ever registered" from "the dispatcher could not
// revoke it". The settle batch's Completed handler is not counted here: it is a
// counted module entry, so the unload waits for its lease as well as for its
// token.
std::atomic<uint32_t> g_uiRegistrationCount{0};

// A layout change — a SizeChanged, a work-area or display broadcast, or the open
// turn that would refresh the renderer — may not re-anchor the geometry while a
// contact owns the panel. One flag defers the change whole (renderer scalars,
// panel rect and scroller cache); the input thread posts the deferred settled pass
// when ownership ends, so the change is applied once, after the finger is gone.
std::atomic<bool> g_measureDeferred{false};

// Defined with the unload helpers; PrepareRootElement uses them to reset on
// every open.
void RevokeHandlers();
void ResetComposition();
void ResetRenderer();
HWND FindStartCoreWindow();

// Defined with the UI-thread publication below. The one geometry measurement and
// its single publish point: `measurePanel` re-derives the rect, and
// `expectedGeneration` is the epoch a queued request was recorded in (0 for the
// synchronous Setup/SizeChanged paths). `requestId` is 0 on those synchronous
// paths and the live request's id otherwise: the publish re-checks it at commit,
// after the candidate is built, so a superseded turn publishes nothing.
bool PublishPanelTrailDip(bool measurePanel, uint32_t expectedGeneration,
                          uint64_t requestId);

// Defined with the UI-thread publication below. Posts one turn of the measurement
// for a request: the open/work-area turn passes `settled` false, the deadline and
// the layout watch pass it true for the pass after the shell placed the panel.
void PostGeometryMeasurement(const SettledMeasureRequest& request, bool settled);

// Defined with the UI-thread publication below. The open transition arms this
// watch, which runs the settled measurement on the second layout pass, when the
// shell's open storyboard has placed the panel. The close teardown disarms it.
void ArmLayoutWatch(const SettledMeasureRequest& request);
void StopSettledTrailWatch();
void StopSettledTrailWatchIfGeneration(uint32_t maxGeneration);

// Defined with the scroller cache below. Measures the scrolling element an open
// menu scrolls with, publishes its screen rect and offsets for the input thread,
// and drops the cached element and its ViewChanged handler on close/teardown. It
// validates the request and the epoch when it commits, and an incomplete or
// owned pass leaves the state Unknown for the retry below.
void CacheScrollableElement(bool settled, uint32_t expectedGeneration,
                            uint64_t requestId);
void ReleaseScrollableElement();
void ReleaseScrollableElementIfGeneration(uint32_t maxGeneration);
void ClearScrollerRetry();

// Defined with the UI-thread publication below. Recomputes the dismiss axis and
// the translation the composition expression bakes in, and rewrites the
// composition scalars when either moved. It publishes nothing: its callers
// publish the one snapshot afterwards. False when the composition write failed:
// the axis and travel are then left at the values the expression still holds.
bool RefreshGestureGeometry();

// Defined with the input-thread gesture state below. Publishes whether a live
// contact drives the panel edge, which the UI-side measurement re-reads when it
// runs. Called after every state change on that thread.
void PublishPanelOwnership();

// Defined with the input-thread gesture state below. The debug `retract` command
// clears it: the shell is about to hide the window, so nothing needs to restore
// `Progress`, and the next open restores it anyway.
extern bool g_commitNeedsProgressReset;

// Defined with the gesture state below: both close families open the same
// ownership state (no session drives Progress, and the suppression window is
// open) through this one helper.
void TakeCloseOwnership(uint64_t tickMs, bool contactStillDown);

// Defined with the visibility service below: the one close-landing body, run by a
// cloak transition and by the executor's confirmation when the window is already
// hidden. `reason` is null for the transition and names the probe otherwise.
void LandClosedMenu(uint64_t tickMs, const wchar_t* reason);

// The cross-process channel with the explorer-side bridge and the debug tools.
// Defined with the channel helpers below; a close publishes its executor
// command through it.
SwipeIpcChannel g_ipc;

// The close's executor command goes through the shared channel below, on the
// input thread: the intent is read and answered, and the dismiss is published
// for the end of the settle. The bridge is the only launcher caller.

// Defined with the input-thread gesture state below; the scheduled dismiss
// needs the kind for its log line.
const wchar_t* CloseKindName(CloseKind kind);

// Defined with the cloaked-window cache below; the debug `state` command reads
// the window's cloak state on demand.
bool StartWindowIsOpen();

// ---------------------------------------------------------------------------
// Composition helpers
// ---------------------------------------------------------------------------

// The dispatcher is UI-thread state, so every other thread copies it here first:
// the copy holds its own reference and cannot race the next Setup. No lock is
// held while the caller posts.
wucore::CoreDispatcher CopyDispatcher() {
    std::lock_guard lock(g_compositionMutex);
    return g_dispatcher;
}

// Posts one UI turn that touches module state, and counts it from before it can
// run: the queued callable owns the lease, the runtime keeps that callable alive
// until the turn has run or the dispatcher refused it, and the unload drains the
// count before it frees anything the turn can touch. No thread waits for the
// result; the count is the only thing the unload needs.
template <typename Fn>
void PostCountedUiTurn(wucore::CoreDispatcher const& dispatcher, Fn&& turn) {
    std::shared_ptr<ModuleEntryLease> lease;
    try {
        lease = std::make_shared<ModuleEntryLease>();
    } catch (...) {
        return;
    }

    try {
        // The returned operation owns the queued callable and completes after it
        // has run, or at once when the dispatcher refused it. Dropping the
        // operation here does not cancel it and does not block this thread.
        dispatcher.TryRunAsync(
            wucore::CoreDispatcherPriority::Normal,
            [lease = std::move(lease), turn = std::forward<Fn>(turn)]() {
                // Holding the lease is the point of the capture: it keeps the
                // count up until this callable is destroyed.
                static_cast<void>(lease);
                turn();
            });
    } catch (...) {
        // The temporary callable is destroyed with its lease.
    }
}

// The settle batch's generation is the only proof that a batch completion
// belongs to the animation that is still on screen. The compositor runs a
// `Completed` handler after the fact, so a late one can arrive when a newer
// settle already owns `Progress`; the handler compares its generation with this
// counter and touches nothing when they differ. Every path that stops or replaces
// the settle animation therefore advances the counter, and the `g_dismissSettling`
// flag ends with the settle it described. Callers hold g_compositionMutex.
void InvalidateDismissBatchLocked() {
    g_dismissSerial.fetch_add(1, std::memory_order_relaxed);
    g_dismissSettling.store(false, std::memory_order_relaxed);
}

// Detaches the stored batch's Completed handler so a completion that has not been
// dispatched yet never reaches module code that no longer owns the animation.
// Caller holds g_compositionMutex. A handler the compositor has already
// dispatched cannot be recalled here; it holds its own reference to the counted
// entry, so the unload's drain waits for it, and the handler reads the stop flag
// before it touches any shared state.
//
// Returns false when the revoke threw. The batch, its token and its counted
// entry are then all kept: the token is the only handle on a handler that can
// still run, and the unload must keep waiting instead of freeing the
// objects the handler uses.
bool RevokeDismissBatchLocked() {
    if (!g_dismissBatch || !g_dismissBatchToken.value) {
        g_dismissBatch = nullptr;
        g_dismissBatchToken = {};
        g_settleLease.reset();
        return true;
    }

    try {
        g_dismissBatch.Completed(g_dismissBatchToken);
    } catch (...) {
        return false;
    }

    g_dismissBatch = nullptr;
    g_dismissBatchToken = {};
    g_settleLease.reset();
    return true;
}

bool ShowAtRest() {
    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {
        return false;
    }

    try {
        g_state.StopAnimation(L"Progress");
        // No settle is on screen any more, so no retract may plan from its curve
        // and no batch completion may still stop Progress.
        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        InvalidateDismissBatchLocked();
        g_panel.coasting.store(false, std::memory_order_relaxed);
        g_state.InsertScalar(L"Progress", kProgressRest);
        g_panel.progress.store(kProgressRest, std::memory_order_relaxed);
        g_panel.target = kProgressRest;
        g_panel.velocity = 0.0f;
        g_panel.phase = PanelPhase::Idle;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Composition reset failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Composition reset failed with an unknown exception");
        return false;
    }

    return true;
}

bool InsertProgress(float progress) {
    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {
        return false;
    }

    try {
        // A direct insert is the new base value, so any animation driving the
        // scalar ends here: the sample it belongs to has already been replaced.
        // A coast is the usual case; a settle still on screen is the takeover
        // case, and its batch completion must not fire against the new value.
        const bool coasting =
            g_panel.coasting.exchange(false, std::memory_order_relaxed);
        if (coasting || g_panel.frames > 0) {
            if (g_panel.frames > 0) {
                RevokeDismissBatchLocked();
                InvalidateDismissBatchLocked();
                g_panel.frames = 0;
            }
            g_state.StopAnimation(L"Progress");
        }
        g_state.InsertScalar(L"Progress", progress);
        g_panel.progress.store(progress, std::memory_order_relaxed);
        g_panel.target = progress;
        g_panel.velocity = 0.0f;
        g_panel.phase = PanelPhase::Drag;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress update failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress update failed with an unknown exception");
        return false;
    }

    return true;
}

// The one Progress animation in this file: a sampled polyline. Every keyframe is
// linear and no easing function is attached, because the spring samples are the
// curve. An easing function on a segment would zero or reshape the initial slope
// the spring exists to keep. The frame count is 13-129: one segment per 4 ms of
// the settle plus the landing frame at T, where the scaled curve is the target.
//
// Every open and every close comes through here, from a gesture and from the
// Windows key alike, so there is one motion and no second curve to drift from it.
//
// The sample list starts at the current position, so the animation needs no
// separate start value: `samples[0]` is where the panel is.
//
// The target is inserted as the base value *before* StartAnimation, so a
// cancelled or interrupted animation still lands on the intended end state
// instead of snapping back to the drag position.
bool AnimateProgressTo(float to, std::chrono::milliseconds duration,
                       const float* settleSamples, int settleSampleCount,
                       uint32_t* serialOut) {
    if (serialOut) {
        *serialOut = 0;
    }
    std::lock_guard lock(g_compositionMutex);
    if (!g_state || !g_compositor || !settleSamples || settleSampleCount < 2) {
        return false;
    }

    try {
        wuc::ScalarKeyFrameAnimation animation =
            g_compositor.CreateScalarKeyFrameAnimation();
        // Equal steps across the duration; the first sample is the release
        // position and the last is the target.
        const float last = static_cast<float>(settleSampleCount - 1);
        for (int i = 0; i < settleSampleCount; ++i) {
            animation.InsertKeyFrame(
                static_cast<float>(i) / last,
                std::clamp(settleSamples[i], 0.0f, 1.0f));
        }
        animation.Duration(duration);

        const uint64_t startQpc = QpcMsNow();
        g_state.StopAnimation(L"Progress");
        // The previous curve stops here, so it may not be read as a position from
        // this point on, and its batch completion may not touch the animation that
        // replaces it. If the new animation never starts, the base value inserted
        // below is where the panel is.
        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        g_panel.coasting.store(false, std::memory_order_relaxed);
        g_state.InsertScalar(L"Progress", std::clamp(to, 0.0f, 1.0f));
        g_state.StartAnimation(L"Progress", animation);
        g_panel.progress.store(std::clamp(to, 0.0f, 1.0f),
                            std::memory_order_relaxed);
        g_panel.target = std::clamp(to, 0.0f, 1.0f);
        g_panel.phase = PanelPhase::Settle;
        // This animation replaces whatever owned Progress: an older batch's
        // completion must not stop it, and no dismiss settle is still running.
        // The serial this animation installs is minted here, under the same lock
        // that started it: a batch that reads it later cannot capture the serial
        // of a reset that happened in between.
        InvalidateDismissBatchLocked();
        if (serialOut) {
            *serialOut = g_dismissSerial.load(std::memory_order_relaxed);
        }
        // Keep the curve for every interruption, which has to know where this
        // animation leaves the panel. The samples are the keyframes just handed
        // over, so the interpolation is the same motion the compositor plays.
        const int frames = std::min(settleSampleCount, kSettleMaxFrames);
        std::memcpy(g_panel.samples, settleSamples, sizeof(float) * frames);
        g_panel.frames = frames;
        g_panel.startQpc = startQpc;
        g_panel.durationMs = static_cast<uint64_t>(duration.count());
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress animation failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress animation failed with an unknown exception");
        return false;
    }

    return true;
}

// Where the linear coast is at qpcNow: from + (to - from) * elapsed / duration.
// Exact, because the animation has no easing and its start time is known here.
float CoastValueAt(uint64_t qpcNow) {
    if (qpcNow <= g_panel.coastStartQpc) {
        return g_panel.coastFrom;
    }

    const uint64_t duration = static_cast<uint64_t>(kCoastDuration.count());
    const uint64_t elapsed = qpcNow - g_panel.coastStartQpc;
    if (elapsed >= duration) {
        return g_panel.coastTo;
    }

    const float t = static_cast<float>(elapsed) / static_cast<float>(duration);
    return std::clamp(g_panel.coastFrom + (g_panel.coastTo - g_panel.coastFrom) * t, 0.0f, 1.0f);
}

// Where the recorded settle puts `Progress` at qpcNow. Its keyframes are exactly
// g_panel.samples, equally spaced and linear, so the value is one interpolation
// between two of them. Once the duration has passed the answer is the last
// sample, which is the target the animation was handed and where the panel
// actually is. Caller holds g_compositionMutex.
float SettleValueAt(uint64_t qpcNow) {
    if (g_panel.frames < 2 || g_panel.durationMs == 0) {
        return std::clamp(g_panel.progress.load(std::memory_order_relaxed), 0.0f,
                          1.0f);
    }
    if (qpcNow <= g_panel.startQpc) {
        return std::clamp(g_panel.samples[0], 0.0f, 1.0f);
    }

    const uint64_t elapsed = qpcNow - g_panel.startQpc;
    if (elapsed >= g_panel.durationMs) {
        return std::clamp(g_panel.samples[g_panel.frames - 1], 0.0f, 1.0f);
    }

    const double position = static_cast<double>(elapsed) /
                            static_cast<double>(g_panel.durationMs) *
                            (g_panel.frames - 1);
    const int index = std::min(static_cast<int>(position), g_panel.frames - 2);
    const float fraction = static_cast<float>(position - index);
    const float value =
        g_panel.samples[index] +
        (g_panel.samples[index + 1] - g_panel.samples[index]) * fraction;
    return std::clamp(value, 0.0f, 1.0f);
}

// ---------------------------------------------------------------------------
// Where `Progress` is at time t: one owner
// ---------------------------------------------------------------------------
//
// `g_panel.progress` records the base value written to the property set, and a
// settle inserts its TARGET as that base before it animates the samples over it.
// While an animation runs, the recorded value is therefore where the panel is
// going, not where it is. Reading it as a position made every interruption start
// the panel at the animation's end and jump there before moving again: the
// keyboard retract during the open settle, a reveal taking a close over, one
// settle replacing another, and the close after a coast. A settle started from a
// target value is the bug this function prevents.
//
// This is the single entry point, and it takes rather than reads: it answers from
// the curve the compositor was handed (or from the coast's own line), stops that
// motion, and pins the answer as the new base value. The caller starts from a
// panel that is standing still where its last frame left it. With nothing
// recorded nothing animates, so the base value is a real position and the answer
// is the resting value.
float TakeProgressNow(const wchar_t* caller) {
    const uint64_t qpcNow = QpcMsNow();

    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {
        // No renderer, so no animation can be on screen and the resting value is
        // the only answer that cannot be a stale target.
        return kProgressRest;
    }

    const bool coasting = g_panel.coasting.load(std::memory_order_relaxed);
    const bool settling = g_panel.frames >= 2 && g_panel.durationMs != 0;
    if (!coasting && !settling) {
        // Nothing animates, so the panel is at rest and the value every stop
        // wrote IS its resting position: the base value is only a target while an
        // animation runs over it, and no path leaves a stopped animation's target
        // behind (ShowAtRest, StopCoast, the take itself and every direct insert
        // write the value first). With no renderer there is no recording at all,
        // and the resting value 1 is the only answer that cannot be a target.
        return std::clamp(g_panel.progress.load(std::memory_order_relaxed), 0.0f,
                          1.0f);
    }

    const float value =
        std::clamp(coasting ? CoastValueAt(qpcNow) : SettleValueAt(qpcNow),
                   0.0f, 1.0f);
    // A settle taken over at its very start is the shape of a dead open: the
    // animation that should lift the panel is stopped a few milliseconds in and
    // the hidden rest is pinned as the base. Rare and diagnostic.
    if (settling && value < 0.05f) {
        const uint64_t elapsed =
            qpcNow > g_panel.startQpc ? qpcNow - g_panel.startQpc : 0;
        TraceLog(L"progress take settle at=%.3f elapsed=%llu dur=%llu caller=%s",
                value, static_cast<unsigned long long>(elapsed),
                static_cast<unsigned long long>(g_panel.durationMs), caller);
    }
    try {
        g_state.StopAnimation(L"Progress");
        g_state.InsertScalar(L"Progress", value);
        g_panel.progress.store(value, std::memory_order_relaxed);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress take failed: %08X %s",
               static_cast<unsigned>(e.code()), e.message().c_str());
        return value;
    } catch (...) {
        Wh_Log(L"Progress take failed with an unknown exception");
        return value;
    }

    // The motion is gone: neither its curve nor its batch completion may act on
    // the value that was just pinned.
    g_panel.frames = 0;
    g_panel.durationMs = 0;
    g_panel.coasting.store(false, std::memory_order_relaxed);
    g_panel.target = value;
    g_panel.velocity = 0.0f;
    g_panel.phase = PanelPhase::Idle;
    RevokeDismissBatchLocked();
    InvalidateDismissBatchLocked();
    return value;
}

// Where the panel really shows at this instant, for a settle that starts from the
// recorded base. The only value `TakeProgressNow` answers while nothing animates
// is that base. At the first interaction of a process the base is the open rest
// (1), which a hidden panel never showed: the shell's window is cloaked and no
// follow and no settle ran through the cloak, so the compositor held the
// off-screen rest. That case therefore answers 0, and a hidden panel's only
// visible value is the off-screen rest. A panel that was already on screen is
// answered by its live position as it is: the caller refuses the paths where a
// close still owns the animation, and at the first millisecond of a retract the
// live value is already below the rest.
//
// This is the settle rule, not the follow rule. A settle may start below the
// screen edge because it spends time crossing that hidden travel without the
// finger; a follow spends the finger's travel, so it starts at
// `RevealEntryProgress` and then closes the gap at the planner's rate, so it
// pays in catch-up time instead of leaving the panel behind forever.
float TakeVisibleProgressNow(const wchar_t* caller = L"visible") {
    const float live = TakeProgressNow(caller);
    return live >= 1.0f ? 0.0f : live;
}

// The same answer as `TakeProgressNow`, without taking it. A diagnostic and the
// refusal test read the live position while a settle or a coast may still own it,
// so this must not stop the motion or pin a new base: it only evaluates the same
// curve the compositor was handed. With nothing recorded, the base value is the
// position. Callers hold no lock; the composition lock is taken here.
float PeekProgressNow() {
    const uint64_t qpcNow = QpcMsNow();
    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {
        return kProgressRest;
    }
    const bool coasting = g_panel.coasting.load(std::memory_order_relaxed);
    const bool settling = g_panel.frames >= 2 && g_panel.durationMs != 0;
    if (!coasting && !settling) {
        return std::clamp(g_panel.progress.load(std::memory_order_relaxed), 0.0f,
                          1.0f);
    }
    return std::clamp(coasting ? CoastValueAt(qpcNow) : SettleValueAt(qpcNow),
                      0.0f, 1.0f);
}

// Whether a settle or a coast is still moving `Progress`. The unhealed check needs
// to tell a panel that is on its way up from one that no animation owns any more.
// A completed settle keeps its recorded samples until the next take, so the
// elapsed time, not the record's presence, is what says whether it is still
// running: a landed curve is not motion.
bool ProgressMotionActive() {
    std::lock_guard lock(g_compositionMutex);
    if (g_panel.coasting.load(std::memory_order_relaxed)) {
        return true;
    }
    if (g_panel.frames < 2 || g_panel.durationMs == 0) {
        if (g_panel.phase == PanelPhase::Settle ||
            g_panel.phase == PanelPhase::Coast) {
            g_panel.phase = PanelPhase::Idle;
            g_panel.velocity = 0.0f;
        }
        return false;
    }
    const uint64_t qpcNow = QpcMsNow();
    const uint64_t elapsed =
        qpcNow > g_panel.startQpc ? qpcNow - g_panel.startQpc : 0;
    if (elapsed >= g_panel.durationMs) {
        if (g_panel.phase == PanelPhase::Settle) {
            g_panel.phase = PanelPhase::Idle;
            g_panel.velocity = 0.0f;
        }
        return false;
    }
    return true;
}

// Starts the continuation between reports. This is deliberately not
// AnimateProgressTo: that helper inserts the target as the base value, and a
// coast must leave the base alone so a report can replace the motion without the
// panel jumping to the end of the coast first. Nothing is logged per coast in the
// shipping build.
bool StartCoast(float from, float to) {
    const uint64_t qpcNow = QpcMsNow();
    from = std::clamp(from, 0.0f, 1.0f);
    to = std::clamp(to, 0.0f, 1.0f);

    std::lock_guard lock(g_compositionMutex);
    if (!g_state || !g_compositor) {
        return false;
    }

    try {
        wuc::ScalarKeyFrameAnimation animation =
            g_compositor.CreateScalarKeyFrameAnimation();
        animation.InsertKeyFrame(0.0f, from);
        // No easing on the keyframe at t=1: the panel keeps the finger's speed
        // for the whole run, which is the only reason the coast is invisible.
        animation.InsertKeyFrame(1.0f, to);
        animation.Duration(kCoastDuration);

        g_state.StopAnimation(L"Progress");
        g_state.StartAnimation(L"Progress", animation);
        // The coast now owns Progress; a settle curve and its batch recorded
        // earlier are gone.
        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        InvalidateDismissBatchLocked();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress coast failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress coast failed with an unknown exception");
        return false;
    }

    g_panel.coastFrom = from;
    g_panel.coastTo = to;
    g_panel.coastStartQpc = qpcNow;
    g_panel.coasting.store(true, std::memory_order_relaxed);
    g_panel.target = to;
    g_panel.velocity =
        (to - from) / static_cast<float>(kCoastDuration.count());
    g_panel.phase = PanelPhase::Coast;
    return true;
}

// Ends the coast and pins the value the panel shows at this instant, so the next
// sample or the close animation continues from a moving panel instead of
// snapping back to the last report.
//
// The linear line above is the source of truth, not a read-back: measured on this
// build, CompositionPropertySet.TryGetScalar answers with the stored base value
// while an animation younger than one compositor frame is running (a coast that
// lived 15 ms at -0.23 DIP/ms read back 0.722, the value of the report before it,
// where the line read 0.720). Pinning the read-back would undo exactly the motion
// the coast was started for.
bool StopCoast(float& value) {
    const uint64_t qpcNow = QpcMsNow();
    value = CoastValueAt(qpcNow);

    std::lock_guard lock(g_compositionMutex);
    g_panel.coasting.store(false, std::memory_order_relaxed);
    if (!g_state) {
        return false;
    }

    try {
        // The animation has to be stopped before its value can become the base.
        g_state.StopAnimation(L"Progress");
        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        InvalidateDismissBatchLocked();
        g_state.InsertScalar(L"Progress", value);
        g_panel.progress.store(value, std::memory_order_relaxed);
        g_panel.target = value;
        g_panel.phase = PanelPhase::Drag;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress coast stop failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress coast stop failed with an unknown exception");
        return false;
    }

    if (g_debugCoast) {
        TraceLog(L"coast stop value=%.3f", value);
    }
    return true;
}

// The four intents. Each records what the controller is doing before the motion
// itself starts, so a diagnostic can name the phase without reconstructing it
// from the animation record. A caller never waits for a previous motion: the
// value the panel has now is the value the animation was handed.
void PanelController::Begin(const float* position) {
    {
        std::lock_guard lock(g_compositionMutex);
        if (position) {
            target = std::clamp(*position, 0.0f, 1.0f);
        }
        velocity = 0.0f;
        phase = PanelPhase::Drag;
    }
    if (position) {
        InsertProgress(*position);
    }
    TraceLog(L"panel intent=begin progress=%.3f source=touch",
            position ? std::clamp(*position, 0.0f, 1.0f)
                     : progress.load(std::memory_order_relaxed));
}

bool PanelController::Move(float position) {
    const float value = std::clamp(position, 0.0f, 1.0f);
    if (!InsertProgress(value)) {
        return false;
    }
    std::lock_guard lock(g_compositionMutex);
    target = value;
    phase = PanelPhase::Drag;
    return true;
}

void PanelController::End(float to, float endVelocity) {
    {
        std::lock_guard lock(g_compositionMutex);
        target = std::clamp(to, 0.0f, 1.0f);
        velocity = endVelocity;
        phase = PanelPhase::Settle;
    }
    TraceLog(L"panel intent=end progress=%.3f velocity=%.2f target=%.0f",
            progress.load(std::memory_order_relaxed), endVelocity,
            std::clamp(to, 0.0f, 1.0f));
}

// A key or click is the degenerate drag: it starts where the panel is, targets
// the far end, and has no seam speed. The samples are planned by the caller,
// exactly as a release's are.
void PanelController::Commit(float to) {
    {
        std::lock_guard lock(g_compositionMutex);
        target = std::clamp(to, 0.0f, 1.0f);
        velocity = 0.0f;
        phase = PanelPhase::Settle;
    }
    TraceLog(L"panel intent=commit progress=%.3f target=%.0f",
            progress.load(std::memory_order_relaxed),
            std::clamp(to, 0.0f, 1.0f));
}

// The settle every gesture outcome runs: the dismiss commit, the dismiss
// cancel, the reveal commit and the reveal cancel. `from` is where the panel is
// at release, `to` is 0 for a close and 1 for an open, and `velocity` is the
// release speed in Progress per millisecond, signed along the gesture axis, as
// the end-velocity estimator reports it. `duration` is the animation's length,
// which is also what the scheduled dismiss is armed with.
//
// One generator, two parameter sets. The style setting only picks the damping
// ratio; everything below runs once for both:
//
// - Open and close are exact mirrors, because the spring works in normalized
//   travel: y(0) = 0, y(inf) = 1, and y'(0) = y0 is the scaled release speed
//   divided by the travel left. The sign of the settle direction cancels there,
//   because (to - from) carries it; the spring always relaxes toward its target.
//   A release at rest gets y0 = 0, the plain step response, so a finger held
//   still for a moment starts the settle at zero speed.
//   A release faster than the remaining travel can absorb overshoots the
//   target, and the 0..1 clamp turns that overshoot into a plateau at the rest
//   position instead of a translation past it.
// - The duration is the travel divided by kSettleSpeedDipPerMs, clamped to
//   160-520 ms, so the panel moves at one speed whatever its size is. The
//   frequency is derived from that duration (see the constant block), so five
//   time constants of the spring's slowest root land on T.
// - The frames are a polyline of 12-128 equal segments, at most 4 ms each, plus
//   one landing frame at T. The whole curve is scaled by its own value at T, so
//   the samples converge on the target with no step at the landing. Each sample
//   is clamped to 0..1 and to the previous sample, so convergence is monotonic.
// - Both sets are overdamped or near it; the sampler therefore carries the
//   underdamped, critically damped and overdamped forms of the same response.

// At most 4 ms per segment, at least 12, at most 128. The samples take one
// segment less than the frames: the last frame is the scaled curve at T, which is
// the target.
int SettleStepsFor(int64_t durationMs) {
    const double steps =
        std::ceil(static_cast<double>(durationMs) / kSettleSegmentMs);
    return std::clamp(static_cast<int>(steps), kSettleMinSteps,
                      kSettleMaxSteps);
}

// The release speed projected onto the settle's direction, in Progress per
// millisecond: the settle scales its initial slope by this component, and both
// release lines print it in DIP. `from` and `to` are the settle's endpoints, so
// the projection is the same one the spring makes.
//
// A release still moving away from its target contributes 0: the finger has
// already reversed, and a settle must never leave the seam backwards.
float TowardTarget(float progressPerMs, float from, float to) {
    if (to == from) {
        return 0.0f;  // the panel already stands at its target
    }
    return std::max(0.0f, to > from ? progressPerMs : -progressPerMs);
}

// The same component in DIP per millisecond: both release lines and the settle
// line print DIP, and the settle divides this by the travel it plans against.
// One projection, one formula, so the number the log prints is the number the
// spring starts with.
float SpeedTowardDip(float progressPerMs, float from, float to, float distance) {
    return TowardTarget(progressPerMs, from, to) * std::max(distance, 0.0f);
}

// One generator, two consumers. `BuildSpringSamples` fills the polyline for a
// damped-spring move from `start` to `target`; the gesture outcomes and the
// keyboard open use it directly, and the shell-triggered close uses it to build
// the keyboard open's curve and plays that backwards. One formula, so the open,
// the gesture close and the shell-triggered close cannot drift apart.
//
// - Open and close are exact mirrors, because the spring works in normalized
//   travel: y(0) = 0, y(inf) = 1, and y'(0) = y0 is the scaled release speed
//   divided by the travel left. The sign of the settle direction cancels there,
//   because (to - from) carries it; the spring always relaxes toward its target.
//   A release at rest gets y0 = 0, the plain step response, so a finger held
//   still for a moment starts the settle at zero speed.
//   A release faster than the remaining travel can absorb overshoots the
//   target, and the 0..1 clamp turns that overshoot into a plateau at the rest
//   position instead of a translation past it.
// - The duration is the travel divided by kSettleSpeedDipPerMs, clamped to
//   160-520 ms, so the panel moves at one speed whatever its size is. The
//   frequency is derived from that duration (see the constant block), so five
//   time constants of the spring's slowest root land on T.
// - The frames are a polyline of 12-128 equal segments, at most 4 ms each, plus
//   one landing frame at T. The whole curve is scaled by its own value at T, so
//   the samples converge on the target with no step at the landing. Each sample
//   is clamped to 0..1 and to the previous sample, so convergence is monotonic.
// - Both sets are overdamped or near it; the sampler therefore carries the
//   underdamped, critically damped and overdamped forms of the same response.
int BuildSpringSamples(float start, float target, float velocity,
                       int64_t durationMs, double zeta, float* samples) {
    const float startScale =
        static_cast<float>(g_gestureStartSpeedPercent.load(
            std::memory_order_relaxed)) /
        100.0f;

    // `velocity` is Progress per millisecond, signed along the gesture axis:
    // positive means Progress is rising. The settle only ever moves toward its
    // target, so a release that is still moving away from it (a pull back before
    // letting go) contributes nothing instead of starting the settle backwards;
    // the finger has already reversed and there is no speed to carry that way.
    // `TowardTarget` is the one projection: the release lines and this call print
    // and scale the same component, one in DIP and one in Progress units.
    const float travel = target - start;  // signed, Progress units
    const float toward = TowardTarget(velocity, start, target);

    // Work in milliseconds throughout: the duration is in ms and y0 is 1/ms.
    // Keeping y0 in seconds would make the initial slope 1000x too small and
    // leave the seam without the finger's velocity. The travel is in Progress
    // units, so dividing by it cancels the same `Distance` the estimator divided
    // by: both gesture families hand over Progress/ms and one formula serves them.
    const double tTotal = static_cast<double>(durationMs);  // ms

    // Normalized initial slope, 1/ms. The sign cancels against (to - from), so
    // this is always positive: the spring leaves the release position toward
    // the target in both directions.
    const double y0 =
        std::fabs(travel) > 0.0f
            ? static_cast<double>(startScale * toward) /
                  static_cast<double>(std::fabs(travel))
            : 0.0;

    // Pick omega so that five time constants of the slowest root land on T.
    // That root is zeta*omega while the spring oscillates; once it is
    // overdamped the slow pole is omega*(zeta - sqrt(zeta^2-1)), which is why
    // the ratio is normalized by the slowest root instead of by zeta.
    const bool overdamped = zeta > 1.0;
    const bool critical = std::fabs(zeta - 1.0) <= 1e-9;
    const double slowestFactor =
        overdamped ? zeta - std::sqrt(zeta * zeta - 1.0) : zeta;
    const double omega = kSettleTimeConstants / (slowestFactor * tTotal);
    const double decay = zeta * omega;  // 1/ms
    const double wd =
        overdamped
            ? omega * std::sqrt(zeta * zeta - 1.0)
            : omega * std::sqrt(std::max(0.0, 1.0 - zeta * zeta));

    // The samples stop one segment short of T; the frame after them is the
    // landing at T. The analytic curve is still short of the target at T:
    // five time constants of the slowest root leave 1 - exp(-5) ~ 0.7%, which is
    // 3.8 DIP on a 501 DIP travel and read as one small hop when the landing
    // frame carried the target itself. Scaling the whole curve by its own value
    // at T makes every sample 0.7% larger instead, so the samples converge on the
    // target with no step at all.
    const auto curveAt = [&](double t) -> double {
        const double envelope = std::exp(-decay * t);
        if (overdamped) {
            return 1.0 - envelope *
                             (std::cosh(wd * t) +
                              ((decay - y0) / wd) * std::sinh(wd * t));
        }
        if (critical) {
            return 1.0 - envelope * (1.0 + (omega - y0) * t);
        }
        return 1.0 - envelope *
                         (std::cos(wd * t) +
                          ((decay - y0) / wd) * std::sin(wd * t));
    };
    // A curve that already reaches the target before T is left alone: it is
    // clamped to 1 there and the landing frame adds nothing.
    const double endValue = std::clamp(curveAt(tTotal), 0.5, 1.0);

    const int steps = SettleStepsFor(durationMs);
    samples[0] = start;
    float previous = start;
    for (int i = 1; i < steps; ++i) {
        const double t = tTotal * i / static_cast<double>(steps);
        const double y = std::clamp(curveAt(t) / endValue, 0.0, 1.0);
        float value = std::clamp(
            start + (target - start) * static_cast<float>(y), 0.0f, 1.0f);
        // Monotonic convergence: a sample never moves the panel back past one
        // it already showed, so an overshoot reads as a stop at rest and a
        // ringing curve never rebounds on screen.
        value = target >= start ? std::max(value, previous)
                                : std::min(value, previous);
        samples[i] = value;
        previous = value;
    }
    samples[steps] = target;
    return steps;
}

// One settle line for every consumer: the style, the direction, the seam speed
// in DIP, the duration and travel, and the first/middle/last segment step. The
// steps are the same numbers the compositor was handed.
void LogSettle(SettleStyle style, const wchar_t* direction, float dipVelocity,
               int64_t durationMs, float remaining, int steps, double zeta,
               const float* samples, float distance) {
    TraceLog(L"settle style=%s dir=%s vel=%.2f dur=%llu remaining=%.0f "
            L"frames=%d zeta=%.2f step0=%.1f stepmid=%.1f stepend=%.1f",
            SettleStyleName(style), direction, dipVelocity,
            static_cast<unsigned long long>(durationMs), remaining, steps + 1,
            zeta, (samples[1] - samples[0]) * distance,
            (samples[steps / 2 + 1] - samples[steps / 2]) * distance,
            (samples[steps] - samples[steps - 1]) * distance);
}

bool BeginGestureSettle(float from, float to, float velocity,
                        std::chrono::milliseconds& duration,
                        uint32_t* serialOut = nullptr) {
    const float start = std::clamp(from, 0.0f, 1.0f);
    const float target = std::clamp(to, 0.0f, 1.0f);
    g_panel.End(target, velocity);

    const SettleStyle style = g_settleStyle.load(std::memory_order_relaxed);
    const double zeta =
        style == SettleStyle::Win11 ? kWin11SpringDamping : kWin10SpringDamping;

    // Plan from the travel, not from a time: the native flyout crosses its 578
    // DIP in 262 ms and this keeps that speed, so a 900 DIP panel takes 409 ms
    // and a resized one takes what its own travel needs. The travel is the
    // snapshot's, so the plane the gesture followed is the plane the settle plans
    // against.
    const float distance =
        std::max(ReadGestureGeometry().distance(), 0.0f);
    const float remaining = std::fabs(target - start) * distance;
    const int64_t durationMs = std::clamp(
        static_cast<int64_t>(std::llround(
            static_cast<double>(remaining) /
            static_cast<double>(kSettleSpeedDipPerMs))),
        static_cast<int64_t>(kSettleMinDuration.count()),
        static_cast<int64_t>(kSettleMaxDuration.count()));
    duration = std::chrono::milliseconds(durationMs);

    float samples[kSettleMaxFrames] = {};
    const int steps = BuildSpringSamples(start, target, velocity, durationMs,
                                         zeta, samples);
    if (!AnimateProgressTo(target, duration, samples, steps + 1, serialOut)) {
        return false;
    }

    LogSettle(style, target >= start ? L"open" : L"close",
              SpeedTowardDip(velocity, start, target, distance), durationMs,
              remaining, steps, zeta, samples, distance);
    return true;
}

// The three shell-triggered closes — the Windows key, the Start button and a
// click-away — use the keyboard open's curve played from the live position to 0:
// the same spring generated for 0 -> 1 and sampled backwards
// (value(i) = start * openY(steps - i)), no seam speed, and the open's own
// full-travel duration. The close therefore starts almost still and accelerates
// into the edge. The gesture close is not this: it keeps following the finger
// and settles with its release spring (`BeginGestureSettle`).
bool BeginMirrorCloseSettle(float from, std::chrono::milliseconds& duration,
                            uint32_t* serialOut = nullptr) {
    const float start = std::clamp(from, 0.0f, 1.0f);
    g_panel.Commit(0.0f);

    const SettleStyle style = g_settleStyle.load(std::memory_order_relaxed);
    const double zeta =
        style == SettleStyle::Win11 ? kWin11SpringDamping : kWin10SpringDamping;

    const float distance =
        std::max(ReadGestureGeometry().distance(), 0.0f);
    const float remaining = distance;
    const int64_t durationMs = std::clamp(
        static_cast<int64_t>(std::llround(
            static_cast<double>(remaining) /
            static_cast<double>(kSettleSpeedDipPerMs))),
        static_cast<int64_t>(kSettleMinDuration.count()),
        static_cast<int64_t>(kSettleMaxDuration.count()));
    duration = std::chrono::milliseconds(durationMs);

    float openSamples[kSettleMaxFrames] = {};
    const int steps = BuildSpringSamples(0.0f, 1.0f, 0.0f, durationMs, zeta,
                                         openSamples);
    float samples[kSettleMaxFrames] = {};
    for (int i = 0; i <= steps; ++i) {
        samples[i] = std::clamp(start * openSamples[steps - i], 0.0f, 1.0f);
    }
    samples[steps] = 0.0f;

    if (!AnimateProgressTo(0.0f, duration, samples, steps + 1, serialOut)) {
        return false;
    }

    LogSettle(style, L"close", 0.0f, durationMs, remaining, steps, zeta,
              samples, distance);
    return true;
}


const wchar_t* CloseKindName(CloseKind kind) {
    return kind == CloseKind::Dismiss ? L"dismiss" : L"reveal";
}

// The tail every close shares: ownership and the dismiss armed for the end of
// the settle. The dismiss and the reveal cancel both come through here.
void ArmGestureClose(CloseKind kind, std::chrono::milliseconds duration) {
    // A finger that has just lifted is already leaving the panel, so this close
    // does not have to mute a contact that will keep reporting.
    TakeCloseOwnership(GetTickCount64(), false);
    ScheduleCloseTap(kind, static_cast<uint64_t>(duration.count()));
}

// The commit decision both gesture families make at release. The travel at
// release and the session average are the whole-gesture tests this mod was
// measured against; only the settle's seam reads the end speed. One function so
// the dismiss and the reveal cannot drift apart on the thresholds.
//
// `travelDip` is the release travel away from the start, `elapsedMs` the session
// length, and `distance` the panel travel the threshold is a fraction of.
bool ReleaseCommits(bool allowCommit, float travelDip, uint64_t elapsedMs,
                    float distance, int32_t axis) {
    if (!allowCommit || !(distance > 0.0f)) {
        return false;
    }
    const float averageVelocity =
        travelDip / static_cast<float>(elapsedMs > 0 ? elapsedMs : 1);
    const float threshold = IsHorizontalAxis(axis)
                                ? kCommitDistanceFraction
                                : kCommitDistanceFractionVertical;
    return travelDip > threshold * distance ||
           averageVelocity > kCommitVelocity;
}

// What a close attempt left behind: whether its settle runs. A close exists once
// that settle started, and only then is a dismiss armed; there is no separate
// dismiss identity to compare, because the pull generation already decides
// cancellation.
struct GestureClose {
    bool started = false;
};

// The close every gesture path runs, from the value the coast left. A close
// exists only once its settle started: the suppression, the closing direction and
// the armed dismiss follow that animation, never a failed one. The dismiss is armed
// for the end of the settle, so the panel is off screen when the shell hides the
// window. The committed dismiss wraps this call in its settle batch; the reveal
// cancel calls it directly.
GestureClose BeginGestureClose(float from, float velocity, CloseKind kind,
                               uint32_t* serialOut = nullptr) {
    GestureClose close;
    std::chrono::milliseconds duration{0};
    if (!BeginGestureSettle(from, 0.0f, velocity, duration, serialOut)) {
        return close;
    }
    close.started = true;
    ArmGestureClose(kind, duration);
    return close;
}

// Puts the panel back for a close that provably never started.
//
// A close exists only once its settle starts: ArmGestureClose publishes the
// closing direction and arms the dismiss after BeginGestureSettle returned true. A
// close that never started therefore published no direction and armed no dismiss, so
// there is nothing to recall here; the restore is the whole operation. This is
// the only path that
// may restore a panel after a failed close, and the log names the visible
// outcome: the missing settle line already says the close never started, and
// this line says what the panel did instead.
bool AbandonClose(CloseKind kind) {
    TraceLog(L"close abandoned kind=%s", CloseKindName(kind));
    return ShowAtRest();
}

// The open counterpart of BeginGestureClose: the same settle style and the same
// release velocity, mirrored. A reveal commit and a cancelled dismiss
// therefore settle with one mechanism and only the target differs.
bool BeginGestureOpen(float from, float velocity) {
    std::chrono::milliseconds duration{0};
    return BeginGestureSettle(from, 1.0f, velocity, duration);
}

// Removes the settle batch's Completed handler. Called before a new batch
// replaces the old one and at unload, so no compositor callback can run module
// code that is about to disappear. A handler that was already dispatched holds
// its own counted entry, which the unload drains. Returns false when the handler
// could not be removed: the batch, its token and its counted entry are then kept,
// because the token is the only handle on a handler that can still run.
bool RevokeDismissBatch() {
    std::lock_guard lock(g_compositionMutex);
    return RevokeDismissBatchLocked();
}

// What a dismiss close attempt left behind.
enum class DismissClose {
    NotStarted,         // no settle ran: nothing is published and no dismiss is armed
    Started,            // the settle runs and its batch observer is registered
    StartedUnobserved,  // the settle runs; the observer was lost, the close goes on
};

struct DismissCloseResult {
    DismissClose start = DismissClose::NotStarted;
};

// The close both gesture paths schedule. The settle runs first; the dismiss is
// armed for its end, where RunPendingCloseTap checks the pull generation and
// either asks for it or drops the close. No deferral file exists.
//
// Committed dismiss: the settle batch observes the settle, the shared close tail
// arms the dismiss. The settle itself, the log line, the suppression window and
// the closing direction all come from BeginGestureClose, so this path and the reveal
// cancel cannot diverge.
//
// The batch observer is optional; the close is not. When the registration loses
// the animation (a reset or a takeover replaced it while the lock was free) or
// the registration itself fails, the settle and its armed dismiss still run, so
// the result says StartedUnobserved and the caller must not put the panel back. Only
// a settle that failed to start leaves the panel in place.
DismissCloseResult BeginDismissSettle(float from, float velocity) {
    DismissCloseResult result;
    wuc::CompositionScopedBatch batch{nullptr};
    winrt::event_token batchToken{};
    uint32_t serial = 0;
    bool handlerRegistered = false;

    // One batch at a time: the previous handler goes before this one exists.
    RevokeDismissBatch();

    try {
        {
            std::lock_guard lock(g_compositionMutex);
            if (!g_state || !g_compositor) {
                return result;
            }

            // The batch must be open before the animation starts, or it scopes
            // nothing. Its handler is registered after the start, because only
            // then is the generation of the animation that is actually on screen
            // known.
            batch = g_compositor.CreateScopedBatch(
                wuc::CompositionBatchTypes::Animation);
        }

        // The serial is minted inside AnimateProgressTo, under the lock that
        // starts the animation. Reading it after that lock is released let a
        // reset in between hand this batch the replacement's serial.
        const GestureClose close =
            BeginGestureClose(from, velocity, CloseKind::Dismiss, &serial);
        if (!close.started) {
            // No animation, so the batch holds nothing and no handler may be
            // registered: a close that never settled must not clear a flag, and
            // it published no direction and armed no dismiss.
            {
                std::lock_guard lock(g_compositionMutex);
                InvalidateDismissBatchLocked();
            }
            batch.End();
            return result;
        }

        // From here on the close exists: its settle runs, its closing direction
        // is published and its dismiss is scheduled. Nothing below may restore the
        // panel.
        result.start = DismissClose::StartedUnobserved;

        // The counted entry is taken before the batch is ended, so a completion
        // that fires at once is already counted. The handler's closure holds a
        // second reference, so the count falls only when the delegate is revoked
        // and no handler is left inside module code.
        std::shared_ptr<ModuleEntryLease> lease;
        try {
            lease = std::make_shared<ModuleEntryLease>();
        } catch (...) {
            // Without a counted entry no handler may be registered: the unload
            // could not prove it has finished.
            TraceLog(L"close observer lost kind=dismiss");
            batch.End();
            return result;
        }

        // The serial belongs to the animation on screen only if nothing replaced
        // it while the lock was free. A reset that did replaced it; register
        // nothing then.
        bool serialLive = false;
        {
            std::lock_guard lock(g_compositionMutex);
            serialLive =
                serial != 0 &&
                serial == g_dismissSerial.load(std::memory_order_relaxed);
            if (serialLive) {
                g_dismissSettling.store(true, std::memory_order_relaxed);
            }
        }
        if (!serialLive) {
            TraceLog(L"close observer lost kind=dismiss");
            batch.End();
            return result;
        }

        batchToken = batch.Completed(
            [serial, lease](
                winrt::Windows::Foundation::IInspectable const&,
                wuc::CompositionBatchCompletedEventArgs const&) {
                // Holding the lease is the point of the capture: it keeps the
                // unload's entry up for as long as this handler can run.
                static_cast<void>(lease);

                // A completion the unload has already revoked can still be
                // dispatched. It must touch no shared state then, so the stop
                // flag is read before the lock.
                if (g_stopDismissCallback.load(std::memory_order_relaxed)) {
                    return;
                }

                try {
                    std::lock_guard lock(g_compositionMutex);

                    // The serial guards the state update against a newer settle
                    // or reveal that replaced the animation.
                    if (serial != g_dismissSerial.load(
                                      std::memory_order_relaxed)) {
                        return;
                    }

                    // This is the settle that registered the handler, and it has
                    // reached its target. Leave Progress there: resetting it
                    // here flashes the fully open menu for the frames between
                    // this reset and the shell hiding the window. The next
                    // session resets it instead. The hide itself is the
                    // executor's job, so this compositor callback only ends the
                    // settle, and its curve is no longer on screen.
                    if (g_state) {
                        g_state.StopAnimation(L"Progress");
                    }
                    g_panel.frames = 0;
                    g_dismissSettling.store(false, std::memory_order_relaxed);
                } catch (...) {
                    g_dismissSettling.store(false, std::memory_order_relaxed);
                }
            });
        handlerRegistered = batchToken.value != 0;

        // End the batch outside the lock: its completion handler takes the
        // same mutex.
        batch.End();

        bool keepHandler = false;
        {
            std::lock_guard lock(g_compositionMutex);
            if (handlerRegistered &&
                serial == g_dismissSerial.load(std::memory_order_relaxed)) {
                g_dismissBatch = batch;
                g_dismissBatchToken = batchToken;
                g_settleLease = lease;
                keepHandler = true;
            }
        }
        if (keepHandler) {
            result.start = DismissClose::Started;
            return result;
        }

        // The registration lost its animation: revoke it, and keep the close.
        // The token is the only handle on a handler that could still run, so it
        // is removed here rather than dropped.
        if (handlerRegistered) {
            try {
                batch.Completed(batchToken);
            } catch (...) {
            }
        }
        TraceLog(L"close observer lost kind=dismiss");
        return result;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Dismiss animation failed: %08X %s", e.code(),
               e.message().c_str());
        if (batch && batchToken.value) {
            try {
                batch.Completed(batchToken);
            } catch (...) {
            }
        }
        std::lock_guard lock(g_compositionMutex);
        InvalidateDismissBatchLocked();
        // `result.start` is already the answer: the settle starts before any of
        // the batch work, and a throw after that leaves a close with an armed
        // dismiss, which only a close that never started may abandon.
        return result;
    } catch (...) {
        Wh_Log(L"Dismiss animation failed with an unknown exception");
        if (batch && batchToken.value) {
            try {
                batch.Completed(batchToken);
            } catch (...) {
            }
        }
        std::lock_guard lock(g_compositionMutex);
        InvalidateDismissBatchLocked();
        // `result.start` is already the answer: the settle starts before any of
        // the batch work, and a throw after that leaves a close with an armed
        // dismiss, which only a close that never started may abandon.
        return result;
    }
}

// ---------------------------------------------------------------------------
// Cross-process channel
//
// Kept in the shipping build. Both directions run through the one section in
// swipe_ipc_shared.h and its events, so no file is involved and nothing polls:
//   Local\SwipeFor10Start_SingleMod_Intent  kind, trigger, method, flags, open
//                                           (bridge -> here)
//   Local\SwipeFor10Start_SingleMod_Exec    show, dismiss, cancel (here -> bridge)
//   ..._State.openSeq                "this is a new menu"               (here)
//   ..._State.peerTick/peerState     "a controller is here"             (here)
//   ..._State.peerEpoch              "which activation answers"         (here)
//
// Every record is a seqlock: this side bumps the counter to odd, writes, and
// bumps it to even, so the bridge can never accept a half-written intent, exec
// command or open generation, and this side never accepts half a command. The
// command record is additionally serialised by a named mutex, because more than
// one tool can publish into it.
//
// The section outlives either process, so its sequence numbers start from
// whatever the other side left. This mod records them at attach and acts only
// on what is newer, which is what the one-second freshness check on the old
// command file did: a command or intent that predates this process belongs to
// a menu this process never drew. An intent carries the open generation the
// bridge saw, so a close is tied to the menu it was read for.
//
// The liveness triple is not a record: it is three words the bridge polls at a
// decision, never a seqlock. The bridge creates the section, so peerState is how
// this side says "the controller is here"; peerTick is the freshness that
// survives a process kill; peerEpoch is the identity of the activation that
// answers. See "Peer liveness" in swipe_ipc_shared.h.
// ---------------------------------------------------------------------------

// The low 32 bits of the tick at which this process last observed a fresh open:
// input thread only. The cancel-recovery ticket compares it at the landing to
// prove that the hide belongs to the menu the cancel named. It is a tick, not
// the open generation: an executor record or an intent that names a menu takes
// SwipeIpcReadOpen's first out-parameter.
uint32_t g_ipcOpenTickLow = 0;

// The last peer tick this side published, as GetTickCount64's low 32 bits. Set
// by OpenIpcChannel before the input thread exists and by that thread from then
// on, so the two writers never overlap. It is the throttle: an idle tick or a
// report inside the same 100 ms window does not write again.
uint32_t g_peerTickLast = 0;

// The activation identity this attach claimed. Written once by OpenIpcChannel,
// before the input thread exists, and read only for the log line and the bridge's
// comparison; 0 means this attachment never opened a channel.
uint32_t g_peerEpoch = 0;

// Whether the shared section currently says `alive`. Only the transition is
// logged, so the withdraw on a clean unload and the one the consumer runs as it
// exits do not read as two different events.
std::atomic<bool> g_peerPublished{false};

// The raw-input registration results, written once on the input thread before
// availability and reported with it. Not gates: readiness is the intent
// consumer's, and a refused registration only means the gesture half is absent
// on this machine.
bool g_touchRegistered = false;
bool g_penRegistered = false;

// The readiness transition and its terminal withdrawal are one critical section,
// not a flag check followed by an independent store. The consumer publishes from
// the input thread while Wh_ModBeforeUninit withdraws from the unload thread; a
// check-then-store let the startup write `alive` after the unload had written
// `gone`, which the bridge then read as a live controller for a thread that was
// already leaving. `g_peerTerminal` is set inside the same lock, so after the
// withdrawal returns no publication can follow it.
SRWLOCK g_peerLock = SRWLOCK_INIT;
bool g_peerTerminal = false;

// Refreshes the bridge's liveness clock. Called from the deadline service and
// from every positioned report, so silence on the digitizer cannot make a live
// controller look gone. Nothing is written while this activation is not
// advertised: the bridge reads the state first, and an idle mod must not write
// into a channel it does not serve.
void PublishPeerTick(uint64_t tickMs) {
    if (!g_ipc.state || !g_peerPublished.load(std::memory_order_relaxed)) {
        return;
    }
    const uint32_t now = static_cast<uint32_t>(tickMs);
    if (now - g_peerTickLast < kSwipePeerTickIntervalMs) {
        return;
    }
    g_peerTickLast = now;
    SwipeIpcPublishPeerTick(&g_ipc, tickMs);
}

// Withdraws availability and, in the same step, the words that were only
// meaningful while it was there. This runs before the consumer stops (a disable)
// and again as the consumer leaves for any reason, so the bridge never reads
// this activation's last direction as the next one's, and never treats a killed
// `closing` (or `touch=1`) as a decision it may act on.
//
// It is the last word of a controller that served its last intent: it goes before
// the view is released. The bridge must pass every trigger from then on, whether
// or not this process finishes unloading.
//
// `terminal` means this activation will not answer again: the flag is set under
// the same lock PublishPeerReady takes, so a startup publication that is already
// in flight either lands before this withdrawal or is refused after it. The
// attach uses false: it withdraws the previous activation's words but leaves this
// one free to publish.
void SetPeerGone(const wchar_t* why, bool terminal) {
    if (!g_ipc.state) {
        return;
    }
    bool logged = false;
    AcquireSRWLockExclusive(&g_peerLock);
    if (terminal) {
        g_peerTerminal = true;
    }
    SwipeIpcSetPeerState(&g_ipc, kSwipePeerGone);
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
    SwipeIpcSetControllerTouch(&g_ipc, false);
    logged = g_peerPublished.exchange(false, std::memory_order_relaxed);
    ReleaseSRWLockExclusive(&g_peerLock);
    if (logged) {
        TraceLog(L"ipc peer gone reason=%s terminal=%d", why, terminal ? 1 : 0);
    }
}

void PublishPeerGone(const wchar_t* why) {
    SetPeerGone(why, true);
}

// Publishes availability from the consumer that is about to serve the intents,
// never from init: an `alive` written before that thread runs is a promise of a
// service that does not exist yet, and a press in that window would be swallowed
// for a consumer that is not there. Tick first, then alive: the bridge pairs an
// alive state with the tick it reads after it, so it never accepts the previous
// activation's tick as this one's. This write is not throttled.
//
// "Availability" here is intent-consumer readiness: this thread's window, its
// message loop and the intent event exist, the root hook is installed, and a
// root is attached (`g_rendererReady`). A refused raw-input registration is
// logged by RegisterHidUsage and does not withdraw it — the touch half degrades,
// the intent half that the bridge's interception depends on still runs.
//
// Returns true when this call published `alive`.
bool PublishPeerReady(const wchar_t* why) {
    if (!g_ipc.state) {
        return false;
    }
    // A controller that could not resolve its root hook cannot drive the panel:
    // it must not advertise an interception service, or the bridge would
    // swallow a native toggle for a menu this side cannot animate. The refusal
    // already logged its one reason; this is the service being withheld.
    if (!g_rootHookReady.load(std::memory_order_acquire)) {
        TraceLog(L"ipc peer not published reason=no-root-hook refused=%d",
                g_rootHookRefused.load(std::memory_order_acquire) ? 1 : 0);
        return false;
    }
    // A root hook without an attached root has no geometry: the controller
    // cannot plan a settle or follow a finger, and a press admitted in that
    // window is swallowed for a service that can only pass it through. The cold
    // session's Win close produced `win open restore (no geometry)` and left the
    // menu open. The peer is announced when the root is.
    if (!g_rendererReady.load(std::memory_order_acquire)) {
        TraceLog(L"ipc peer not published reason=no-renderer");
        return false;
    }
    if (!g_ipc.intentEvent) {
        // Without the intent event the bridge can publish a decision nobody
        // wakes for. That is not a consumer, so it is not announced.
        TraceLog(L"ipc peer not published reason=no-intent-event");
        return false;
    }
    bool published = false;
    AcquireSRWLockExclusive(&g_peerLock);
    if (g_peerTerminal) {
        ReleaseSRWLockExclusive(&g_peerLock);
        TraceLog(L"ipc peer not published reason=terminal-withdrawal");
        return false;
    }
    const uint64_t now = GetTickCount64();
    g_peerTickLast = static_cast<uint32_t>(now);
    SwipeIpcPublishPeerTick(&g_ipc, now);
    SwipeIpcSetPeerState(&g_ipc, kSwipePeerAlive);
    g_peerPublished.store(true, std::memory_order_relaxed);
    published = true;
    ReleaseSRWLockExclusive(&g_peerLock);
    if (published) {
        TraceLog(L"ipc peer alive reason=%s epoch=%u touch=%d pen=%d", why,
                g_peerEpoch, g_touchRegistered ? 1 : 0,
                g_penRegistered ? 1 : 0);
    }
    return published;
}

void OpenIpcChannel() {
    bool created = false;
    DWORD error = 0;
    if (!SwipeIpcOpen(&g_ipc, &created, &error)) {
        TraceLog(L"ipc open failed err=%u", static_cast<unsigned>(error));
        return;
    }

    // The section may still carry the previous activation's `alive` and its last
    // decision. Both are withdrawn before this attach claims anything: the bridge
    // must not decide for a consumer that has not started, and a kill while
    // `closing` must not survive into this activation. This attachment is not
    // terminal: it clears the previous flag under the same lock PublishPeerReady
    // takes, so its own consumer can still announce itself.
    AcquireSRWLockExclusive(&g_peerLock);
    g_peerTerminal = false;
    ReleaseSRWLockExclusive(&g_peerLock);
    SetPeerGone(L"attach", false);

    // Everything published before this attach belongs to an earlier menu. The
    // raw counter is enough for a seed: the first record this process accepts
    // must be newer than anything the previous one could have written. An intent
    // published while no consumer was listening belongs to no menu of this
    // activation, so only a record newer than this boundary is consumed.
    g_lastIntentSeq = SwipeIpcLoad64(&g_ipc.state->intentSeq);
    g_ipcOpenTickLow = 0;
    // The activation identity goes before anything this activation publishes. A
    // bridge decision that was latched against the previous activation can then
    // see that a different one answers, even though both run the same image.
    // image.
    g_peerEpoch = SwipeIpcBeginPeerActivation(&g_ipc);

    // A show or dismiss from the previous activation can still be queued or
    // armed at the bridge when this one attaches. This activation has published
    // no command yet, so a cancel with target 0 can only supersede the previous
    // one; the executor never reads it as its newest show or dismiss. A call
    // already inside the shell's own `Dismiss` cannot be recalled — the bridge
    // checks the activation epoch immediately before the call, and the landing
    // rules handle what an entered call does.
    if (g_ipc.state) {
        SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0, 0);
        TraceLog(L"exec cancel request target=0 reason=attach epoch=%u",
                g_peerEpoch);
    }

    TraceLog(L"ipc open created=%d intent=%lld epoch=%u", created ? 1 : 0,
            static_cast<long long>(g_lastIntentSeq), g_peerEpoch);
}

// M3 executor requests. The bridge owns ShowStartView/Dismiss; these publish the
// command and its sequence. Each returns whether a consumer can actually run it:
// no section, or no Exec event to wake the worker, means nothing will execute the
// command, and the caller must not claim a panel for it. A command that is not
// acked expires at the bridge and the Start side resyncs from the cloak.
constexpr uint64_t kBridgeDismissLeadMs = 30;
constexpr uint64_t kBridgeAckTimeoutMs = 400;
// The bridge's own confirmation delay after a dismiss returns. A recovery ticket
// is meaningful for the deadline plus the ack budget plus this probe; after that
// the cancelled command cannot still be inside the shell's Dismiss call.
constexpr uint64_t kDismissConfirmGraceMs = 300;

bool RequestBridgeShow(uint32_t method, uint32_t flags) {
    if (!g_ipc.state || !g_ipc.execEvent) {
        TraceLog(L"exec show request skipped seq=none method=%u reason=%s",
                method, g_ipc.state ? L"no-executor" : L"no-channel");
        return false;
    }
    // A show is an open; any dismiss still awaiting its visibility confirmation
    // belongs to the close this open ended.
    ForgetConfirmedDismiss();
    const LONG64 sequence = SwipeIpcPublishExec(
        &g_ipc, kSwipeExecShow, static_cast<int32_t>(method),
        static_cast<int32_t>(flags), 0, 0);
    if (sequence <= 0) {
        TraceLog(L"exec show request skipped seq=none method=%u "
                L"reason=no-sequence",
                method);
        return false;
    }
    g_pendingShowExec.store(sequence, std::memory_order_relaxed);
    g_pendingShowUntil.store(GetTickCount64() + kBridgeAckTimeoutMs,
                             std::memory_order_relaxed);
    // The open is on its way: watch for its cloak event, and poll slowly if it
    // never arrives.
    NoteVisibilityTransition(kVisibilityWantOpen);
    TraceLog(L"exec show request seq=%lld method=%u flags=%u",
            static_cast<long long>(sequence), method, flags);
    return true;
}

// Maps a close intent's observed shell method to the executor's dismiss
// vocabulary. A click-away's intent carries the Hide method 7, which is
// evidence about the shell path, not a dismiss value this mod sends: the
// executor calls XamlLauncher::Dismiss with 1 (Win) or 6 (Start button). A
// controller-owned click-away executes as 6, the pointer-driven close the Start
// button's own dismiss uses, so the close still animates exactly once. The
// intent record keeps 7 for the decision log.
//
// The number alone is not the evidence: the conversion needs the click-away
// *trigger* too. A method-7 intent from the Win trigger is not something this
// bridge publishes; if one arrives, it is forwarded as the number it carries and
// the executor refuses it, rather than being laundered into a dismiss.
constexpr int32_t kClickAwayShellMethod = 7;
constexpr uint32_t kClickAwayExecMethod = 6;

uint32_t ExecDismissMethodForIntent(int32_t intentMethod, int32_t intentTrigger) {
    if (intentTrigger == kSwipeTriggerClickAway &&
        intentMethod == kClickAwayShellMethod) {
        return kClickAwayExecMethod;
    }
    return static_cast<uint32_t>(intentMethod);
}

bool RequestBridgeDismiss(uint32_t method, uint64_t executeAt, uint64_t open) {
    if (!g_ipc.state || !g_ipc.execEvent) {
        TraceLog(L"exec dismiss request skipped seq=none method=%u reason=%s",
                method, g_ipc.state ? L"no-executor" : L"no-channel");
        return false;
    }
    // The command's generation is the section's current one. A close intent can
    // carry 0 when its publisher could not read the open record; letting that
    // travel on would make the bridge's "0 matches any" rule, and this side's
    // confirmation, accept a later menu's hide as this close. The read is the
    // publisher's own open record, so the command carries the same identity the
    // bridge will compare before it calls the launcher.
    uint64_t commandOpen = open;
    if (commandOpen == 0) {
        uint64_t currentGeneration = 0;
        uint64_t currentTick = 0;
        if (SwipeIpcReadOpen(&g_ipc, &currentGeneration, &currentTick)) {
            commandOpen = currentGeneration;
        }
    }
    const LONG64 sequence = SwipeIpcPublishExec(
        &g_ipc, kSwipeExecDismiss, static_cast<int32_t>(method), 0, executeAt,
        commandOpen);
    if (sequence <= 0) {
        TraceLog(L"exec dismiss request skipped seq=none method=%u "
                L"reason=no-sequence",
                method);
        return false;
    }
    g_pendingDismissExec.store(sequence, std::memory_order_relaxed);
    g_pendingDismissExecAt.store(executeAt, std::memory_order_relaxed);
    // The same stamp, kept for the confirmation and the landing. A landing for
    // a later menu must not claim this command.
    g_pendingDismissExecGeneration = commandOpen;
    // The new command replaces whatever confirmation an earlier dismiss still
    // owed; the bridge answers the superseded one with its own state.
    ForgetConfirmedDismiss();
    // The close is on its way: watch for its cloak event, and poll slowly if it
    // never arrives.
    NoteVisibilityTransition(kVisibilityWantClosed);
    TraceLog(L"exec dismiss request seq=%lld method=%u executeAt=%llu open=%llu "
            L"intent_open=%llu",
            static_cast<long long>(sequence), method,
            static_cast<unsigned long long>(executeAt),
            static_cast<unsigned long long>(commandOpen),
            static_cast<unsigned long long>(open));
    return true;
}

// Stops the executor's dismiss timer, if one was requested. Returns the cancelled
// sequence, or 0 when nothing was armed; the log line is written only for one.
// The caller keeps the sequence when it must bind a later recovery to this close.
LONG64 CancelPendingDismiss(const wchar_t* reason) {
    const LONG64 target =
        g_pendingDismissExec.exchange(0, std::memory_order_relaxed);
    g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
    if (target == 0) {
        return 0;
    }
    if (g_ipc.state) {
        SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0, target);
    }
    TraceLog(L"exec cancel request target=%lld reason=%s",
            static_cast<long long>(target), reason);
    return target;
}

// A fresh open ends every dismiss of the menu it replaced. A command still in the
// exec slot may be queued at the bridge: cancel it through the one cancel path so
// the bridge is not left with a command to run against the new menu. A
// confirmation (a command whose primitive returned) is dropped; its generation is
// older than the one just published, so it could never land this menu's hide.
void RetirePendingDismissForFreshOpen(uint64_t freshOpen) {
    if (g_pendingDismissConfirm != 0) {
        TraceLog(L"exec dismiss confirm dropped command=%lld gen=%llu "
                L"(fresh open=%llu)",
                static_cast<long long>(g_pendingDismissConfirm),
                static_cast<unsigned long long>(
                    g_pendingDismissConfirmGeneration),
                static_cast<unsigned long long>(freshOpen));
    }
    if (g_pendingDismissExec.load(std::memory_order_relaxed) != 0) {
        CancelPendingDismiss(L"fresh-open");
    }
    ForgetPendingDismiss();
}

// The repair every terminal executor failure performs, whatever kind the command
// was: the controller stops owning the panel and stops marking the touch in the
// same step, and the pending open entry is dropped. The published direction then
// follows the window's cloak on the next transition, which is the one source of
// truth for a panel the controller no longer drives. Declared here because the
// open and close paths above use it; defined with the ack handling.
void ReleaseExecutorOwnership();

// ---------------------------------------------------------------------------
// Raw HID input
//
// The legacy Start consumes the pointer stream before XAML sees it, so the
// only reliable live source is raw input. Each digitizer is decoded from its
// RIDI_PREPARSEDDATA blob; the device is never opened.
// ---------------------------------------------------------------------------

struct HidDevice {
    HANDLE handle = nullptr;
    bool ready = false;
    bool decodeFailLogged = false;
    uint64_t lastUsedTick = 0;
    WCHAR path[kHidPathChars] = {};
    PHIDP_PREPARSED_DATA preparsed = nullptr;
    std::vector<HIDP_VALUE_CAPS> valueCaps;
    std::vector<HIDP_BUTTON_CAPS> buttonCaps;
    USHORT contactNodes[kMaxContactCollections] = {};
    int contactNodeCount = 0;
    USHORT contactCountLink = 0;
    bool hasContactCount = false;
    // Decoded once at init. `hasReportIds` says the first byte of every raw
    // report is the report ID (HID's layout); the two flags say whether the
    // device exposes the pen/like button usages at all, which is what the
    // `hid usages` line records.
    bool hasReportIds = false;
    bool hasTipSwitch = false;
    bool hasInRange = false;
    // The device declares the Contact Identifier (0x0D/0x51) inside one of its
    // contact collections, so a contact's identity is that identifier for as long
    // as it is down. A contact node index is only a position in the report; the
    // device may hand the same node to the next finger without a lift in between,
    // which would transfer a drag to a finger that never started one.
    bool hasContactIdentifier = false;
    // The device exposes neither Tip Switch nor In Range, so no report can say
    // whether a slot is a real contact. Decided once at init, logged as
    // `coordpath` in `hid usages`, and the count/position path is then the only
    // answer for this device. Every other device is decoded by its buttons.
    bool coordinatePath = false;
};

HidDevice g_hidDevices[kMaxHidDevices];
int g_hidDeviceCount = 0;  // input thread only
bool g_hidCacheFullLogged = false;

void CopyDevicePath(HANDLE handle, WCHAR* out, size_t outCount) {
    out[0] = L'\0';

    UINT chars = 0;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(handle, RIDI_DEVICENAME, nullptr, &chars) ==
            static_cast<UINT>(-1) ||
        chars == 0) {
        wcsncpy_s(out, outCount, L"?", _TRUNCATE);
        return;
    }

    WCHAR* name = new (std::nothrow) WCHAR[chars + 1];
    if (!name) {
        wcsncpy_s(out, outCount, L"?", _TRUNCATE);
        return;
    }

    name[0] = L'\0';
    UINT fetched = chars;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(handle, RIDI_DEVICENAME, name, &fetched) ==
        static_cast<UINT>(-1)) {
        wcsncpy_s(out, outCount, L"?", _TRUNCATE);
    } else {
        name[chars] = L'\0';
        wcsncpy_s(out, outCount, name, _TRUNCATE);
    }

    delete[] name;
}

void LogDecodeFail(HidDevice& device, unsigned error) {
    if (device.decodeFailLogged) {
        return;
    }

    device.decodeFailLogged = true;
    TraceLog(L"hid decodefail dev=%s err=%u", device.path, error);
}

// Reads the path, the preparsed blob, the contact collection indices and the
// value caps once per device. Runs on the input thread.
void InitHidDevice(HidDevice& device) {
    CopyDevicePath(device.handle, device.path, ARRAYSIZE(device.path));

    UINT bytes = 0;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(device.handle, RIDI_PREPARSEDDATA, nullptr,
                               &bytes) == static_cast<UINT>(-1) ||
        bytes == 0 || bytes > 64 * 1024) {
        LogDecodeFail(device, GetLastError());
        return;
    }

    BYTE* blob = new (std::nothrow) BYTE[bytes];
    if (!blob) {
        return;
    }

    UINT fetched = bytes;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(device.handle, RIDI_PREPARSEDDATA, blob,
                               &fetched) == static_cast<UINT>(-1) ||
        fetched == 0) {
        LogDecodeFail(device, GetLastError());
        delete[] blob;
        return;
    }

    device.preparsed = reinterpret_cast<PHIDP_PREPARSED_DATA>(blob);

    HIDP_CAPS caps{};
    const NTSTATUS capsStatus = HidP_GetCaps(device.preparsed, &caps);
    if (capsStatus != HIDP_STATUS_SUCCESS) {
        device.decodeFailLogged = true;
        TraceLog(L"hid decodefail dev=%s err=0x%08X", device.path,
                static_cast<unsigned>(capsStatus));
        return;
    }

    // The logical range of X and Y is needed to map raw values to pixels.
    if (caps.NumberInputValueCaps > 0) {
        device.valueCaps.resize(caps.NumberInputValueCaps);
        USHORT length = caps.NumberInputValueCaps;
        if (HidP_GetValueCaps(HidP_Input, device.valueCaps.data(), &length,
                              device.preparsed) == HIDP_STATUS_SUCCESS) {
            device.valueCaps.resize(length);
        } else {
            device.valueCaps.clear();
        }
    }

    // Button usages: Tip Switch and In Range are buttons, not values, so they
    // are read with HidP_GetUsages. The caps are also the only way to know
    // whether a report carries them at all.
    if (caps.NumberInputButtonCaps > 0) {
        device.buttonCaps.resize(caps.NumberInputButtonCaps);
        USHORT length = caps.NumberInputButtonCaps;
        if (HidP_GetButtonCaps(HidP_Input, device.buttonCaps.data(), &length,
                               device.preparsed) == HIDP_STATUS_SUCCESS) {
            device.buttonCaps.resize(length);
        } else {
            device.buttonCaps.clear();
        }
    }

    // HID's own report-ID rule: if any input cap declares a non-zero ReportID,
    // every report of the device starts with that ID byte and the payload
    // follows it. RAWINPUTHEADER.wParam is RIM_INPUT/RIM_INPUTSINK and carries
    // no report ID, so this is the only place the answer can come from.
    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.ReportID != 0) {
            device.hasReportIds = true;
            break;
        }
    }
    if (!device.hasReportIds) {
        for (const HIDP_BUTTON_CAPS& cap : device.buttonCaps) {
            if (cap.ReportID != 0) {
                device.hasReportIds = true;
                break;
            }
        }
    }

    // Contact collections are 0x0D/0x05 on classic digitizers and 0x0D/0x22
    // (Finger) on the ELAN2513. Both carry X/Y per contact. A pen's data lives
    // in its own 0x0D/0x02 or 0x0D/0x20 link node, so it is collected too: on
    // a touch-only panel neither exists and nothing changes.
    HIDP_LINK_COLLECTION_NODE nodes[kMaxLinkNodes]{};
    ULONG nodeCount = kMaxLinkNodes;
    if (HidP_GetLinkCollectionNodes(nodes, &nodeCount, device.preparsed) ==
        HIDP_STATUS_SUCCESS) {
        if (nodeCount > kMaxLinkNodes) {
            nodeCount = kMaxLinkNodes;
        }

        // Two passes, so a contact/finger node always precedes a pen node: the
        // first collection with a position wins while no session is active, and
        // a touch must never lose that tie to a pen hovering nearby.
        for (int pass = 0; pass < 2; ++pass) {
            for (ULONG i = 0; i < nodeCount; ++i) {
                if (nodes[i].LinkUsagePage != kUsagePageDigitizer) {
                    continue;
                }
                const USAGE usage = nodes[i].LinkUsage;
                const bool isContact =
                    usage == kUsageContact || usage == kUsageFinger;
                const bool isPen = usage == kUsagePen || usage == kUsageStylus;
                if ((pass == 0) != isContact || (!isContact && !isPen)) {
                    continue;
                }
                if (device.contactNodeCount < kMaxContactCollections) {
                    device.contactNodes[device.contactNodeCount++] =
                        static_cast<USHORT>(i);
                }
            }
        }
    }

    // Which of the two button usages this device actually exposes. The flags
    // name the usage, not the report: the per-report test in
    // ReadContactButtons also checks the report ID and the link collection.
    for (const HIDP_BUTTON_CAPS& cap : device.buttonCaps) {
        if (cap.UsagePage != kUsagePageDigitizer) {
            continue;
        }
        const bool matchesTip =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageTipSwitch &&
                           kUsageTipSwitch <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageTipSwitch);
        const bool matchesRange =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageInRange &&
                           kUsageInRange <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageInRange);
        device.hasTipSwitch = device.hasTipSwitch || matchesTip;
        device.hasInRange = device.hasInRange || matchesRange;
    }

    // A device that exposes neither usage can never say whether a slot is a
    // real contact. That decision is made once here, not guessed per report,
    // and the `hid usages` line records it: the count/position path is the only
    // answer such a device has.
    device.coordinatePath = !device.hasTipSwitch && !device.hasInRange;

    // Contact Identifier (0x0D/0x51) names one contact for as long as it is down,
    // so it survives a slot reassignment. Only an identifier that lives inside a
    // contact collection can name the slot's contact: one declared in the
    // top-level collection is a single number for the whole device.
    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.UsagePage != kUsagePageDigitizer) {
            continue;
        }
        const bool matches =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageContactIdentifier &&
                           kUsageContactIdentifier <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageContactIdentifier);
        if (!matches) {
            continue;
        }
        for (int i = 0; i < device.contactNodeCount; ++i) {
            if (cap.LinkCollection == device.contactNodes[i]) {
                device.hasContactIdentifier = true;
                break;
            }
        }
        if (device.hasContactIdentifier) {
            break;
        }
    }

    // The Contact Count that reports how many contacts are down lives in the
    // top-level digitizer collection, not in collection 0. Reading it with the
    // wrong collection makes a lift report look like "still one contact",
    // which forces the idle timer to guess the release.
    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.UsagePage != kUsagePageDigitizer) {
            continue;
        }
        const bool matches =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageContactCount &&
                           kUsageContactCount <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageContactCount);
        if (!matches) {
            continue;
        }
        device.contactCountLink = cap.LinkCollection;
        device.hasContactCount = true;
        break;
    }
    TraceLog(L"hidcc dev=%s have=%d link=%u", device.path,
            device.hasContactCount ? 1 : 0,
            static_cast<unsigned>(device.contactCountLink));

    // One line per device so a future pen test can tell a hover that was
    // ignored from a device that never reported the usage at all. `coordpath`
    // is the logged fallback decision: 1 means this device carries no button
    // usage the mod can read, so its count/position path is used. `identity`
    // says which contact name the tracked gesture uses: the Contact Identifier
    // when the device declares one per contact, the contact-node index otherwise.
    TraceLog(L"hid usages dev=%s tip=%d inrange=%d count=%d buttons=%u "
            L"reportids=%d coordpath=%d identity=%s",
            device.path, device.hasTipSwitch ? 1 : 0,
            device.hasInRange ? 1 : 0, device.hasContactCount ? 1 : 0,
            static_cast<unsigned>(device.buttonCaps.size()),
            device.hasReportIds ? 1 : 0, device.coordinatePath ? 1 : 0,
            device.hasContactIdentifier ? L"contactid" : L"node");

    device.ready = true;
}

HidDevice* GetHidDevice(HANDLE handle) {
    if (!handle) {
        return nullptr;
    }

    const uint64_t now = GetTickCount64();
    for (int i = 0; i < g_hidDeviceCount; ++i) {
        if (g_hidDevices[i].handle == handle) {
            g_hidDevices[i].lastUsedTick = now;
            return &g_hidDevices[i];
        }
    }

    HidDevice* device = nullptr;
    if (g_hidDeviceCount < kMaxHidDevices) {
        device = &g_hidDevices[g_hidDeviceCount++];
    } else {
        // Injectors create a fresh virtual digitizer handle for every process,
        // so the table must recycle instead of going deaf after four devices.
        device = &g_hidDevices[0];
        for (int i = 1; i < g_hidDeviceCount; ++i) {
            if (g_hidDevices[i].lastUsedTick < device->lastUsedTick) {
                device = &g_hidDevices[i];
            }
        }
        delete[] reinterpret_cast<BYTE*>(device->preparsed);
        if (!g_hidCacheFullLogged) {
            g_hidCacheFullLogged = true;
            TraceLog(L"hid device table recycled (was full)");
        }
    }

    *device = HidDevice{};
    device->handle = handle;
    device->lastUsedTick = now;
    InitHidDevice(*device);
    return device;
}

void FreeHidDevices() {
    for (int i = 0; i < g_hidDeviceCount; ++i) {
        delete[] reinterpret_cast<BYTE*>(g_hidDevices[i].preparsed);
        g_hidDevices[i] = HidDevice{};
    }
    g_hidDeviceCount = 0;
}

// The answer of one HidP read. `Absent` means the report does not carry the
// field at all (another report ID, another collection), which is not a failure;
// `Error` means the field belongs to this report and could not be decoded. The
// two must stay apart: only `Absent` may be read as "this device has no such
// usage", and `Error` must never become "no contact".
enum class HidField : uint8_t { Ok, Absent, Error };

HidField ClassifyHidStatus(NTSTATUS status) {
    if (status == HIDP_STATUS_SUCCESS) {
        return HidField::Ok;
    }
    // HID's own answers for "this report or collection does not carry it".
    if (status == HIDP_STATUS_USAGE_NOT_FOUND ||
        status == HIDP_STATUS_INCOMPATIBLE_REPORT_ID ||
        status == HIDP_STATUS_REPORT_DOES_NOT_EXIST) {
        return HidField::Absent;
    }
    return HidField::Error;
}

HidField ReadUsageValue(const HidDevice& device, USAGE usagePage,
                        USHORT collection, USAGE usage, const BYTE* report,
                        ULONG reportLength, ULONG* value,
                        NTSTATUS* statusOut = nullptr) {
    if (!device.preparsed) {
        if (statusOut) {
            *statusOut = HIDP_STATUS_INVALID_PREPARSED_DATA;
        }
        return HidField::Error;
    }

    ULONG parsed = 0;
    const NTSTATUS status = HidP_GetUsageValue(
        HidP_Input, usagePage, collection, usage, &parsed, device.preparsed,
        reinterpret_cast<PCHAR>(const_cast<BYTE*>(report)), reportLength);
    if (statusOut) {
        *statusOut = status;
    }
    if (status != HIDP_STATUS_SUCCESS) {
        return ClassifyHidStatus(status);
    }

    *value = parsed;
    return HidField::Ok;
}

struct HidAxisRange {
    long min = 0;
    long max = 0;
    bool valid = false;
};

// Selects the value cap that describes this report: the axis must be the one
// read out of `collection`, under the report's own ID. Matching by usage and
// report ID alone could pick a range that belongs to another collection in the
// same report (a pen node beside a finger node, or a second contact node), and
// the raw value would then be scaled by a range it was never encoded in. A
// device without report IDs keeps reportId 0 (see HidDevice::hasReportIds).
HidAxisRange FindAxisRange(const HidDevice& device, USAGE usage,
                           USHORT reportId, USHORT collection) {
    HidAxisRange range;

    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.UsagePage != kUsagePageGeneric ||
            cap.LinkCollection != collection) {
            continue;
        }

        const bool usageMatches =
            cap.IsRange
                ? (usage >= cap.Range.UsageMin && usage <= cap.Range.UsageMax)
                : (cap.NotRange.Usage == usage);
        if (!usageMatches) {
            continue;
        }
        if (device.hasReportIds ? cap.ReportID != reportId
                                : cap.ReportID != 0) {
            continue;
        }
        if (cap.LogicalMax <= cap.LogicalMin) {
            continue;
        }

        range.min = cap.LogicalMin;
        range.max = cap.LogicalMax;
        range.valid = true;
        break;
    }

    return range;
}

// Reads the button usages of one contact collection in one report. Tip Switch
// and In Range are buttons, so HidP_GetUsageValue never finds them; the value
// caps above only ever describe axes. `field` keeps three answers apart: the
// report carries the collection's digitizer buttons, it does not, or the decode
// failed. The last two used to be the same empty answer, so a decode failure
// was indistinguishable from a device that declares no buttons at all.
struct HidContactButtons {
    HidField field = HidField::Absent;
    NTSTATUS status = HIDP_STATUS_SUCCESS;
    bool tipDeclared = false;
    bool tipDown = false;
    bool inRangeDeclared = false;
    bool inRangeDown = false;
};

HidContactButtons ReadContactButtons(const HidDevice& device, USHORT collection,
                                     USHORT reportId, const BYTE* report,
                                     ULONG reportLength) {
    HidContactButtons state;
    if (!device.preparsed) {
        state.field = HidField::Error;
        state.status = HIDP_STATUS_INVALID_PREPARSED_DATA;
        return state;
    }
    if (device.buttonCaps.empty()) {
        return state;  // the device declares no button usages at all
    }

    USAGE usages[128] = {};
    ULONG count = ARRAYSIZE(usages);
    const NTSTATUS status = HidP_GetUsages(
        HidP_Input, kUsagePageDigitizer, collection, usages, &count,
        device.preparsed, reinterpret_cast<PCHAR>(const_cast<BYTE*>(report)),
        reportLength);
    state.status = status;
    state.field = ClassifyHidStatus(status);
    if (state.field != HidField::Ok) {
        // Absent: this report or collection carries none of the device's
        // digitizer buttons. Error: it does and the decode failed.
        return state;
    }

    for (const HIDP_BUTTON_CAPS& cap : device.buttonCaps) {
        if (cap.UsagePage != kUsagePageDigitizer ||
            cap.LinkCollection != collection ||
            (device.hasReportIds ? cap.ReportID != reportId
                                 : cap.ReportID != 0)) {
            continue;
        }

        const auto pressed = [&usages, count](USAGE usage) {
            for (ULONG i = 0; i < count; ++i) {
                if (usages[i] == usage) {
                    return true;
                }
            }
            return false;
        };
        const bool matchesTip =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageTipSwitch &&
                           kUsageTipSwitch <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageTipSwitch);
        const bool matchesRange =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageInRange &&
                           kUsageInRange <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageInRange);
        if (matchesTip) {
            state.tipDeclared = true;
            state.tipDown = state.tipDown || pressed(kUsageTipSwitch);
        }
        if (matchesRange) {
            state.inRangeDeclared = true;
            state.inRangeDown = state.inRangeDown || pressed(kUsageInRange);
        }
    }

    return state;
}

// One contact slot decoded from one report. `declared` says the report carries
// a contact field for this slot (a coordinate or a digitizer button);
// `hasPosition` says both coordinates are present; `atOrigin` says that pair is
// (0,0), which is the lift heuristic of the coordinate-only path and a real
// screen corner on a device whose tip says the contact is down.
struct HidContactSlot {
    bool declared = false;
    bool hasPosition = false;
    bool atOrigin = false;
    long rawX = 0;
    long rawY = 0;
    // The contact's own name in this report, when the device declares one. A
    // report ID that omits the field says nothing about the name, so `hasIdentifier`
    // stays false and the node index is used.
    bool hasIdentifier = false;
    ULONG identifier = 0;
    HidContactButtons buttons{};
};

// Decodes one slot's coordinates, name and buttons. False means the report
// carries a field for this slot that could not be decoded; `error` is HID's
// status.
bool DecodeContactSlot(const HidDevice& device, USHORT collection,
                       USHORT reportId, const BYTE* report, ULONG reportLength,
                       HidContactSlot& slot, NTSTATUS& error) {
    ULONG x = 0;
    ULONG y = 0;
    NTSTATUS status = HIDP_STATUS_SUCCESS;
    const HidField xField = ReadUsageValue(device, kUsagePageGeneric, collection,
                                           kUsageX, report, reportLength, &x,
                                           &status);
    if (xField == HidField::Error) {
        error = status;
        return false;
    }
    const HidField yField = ReadUsageValue(device, kUsagePageGeneric, collection,
                                           kUsageY, report, reportLength, &y,
                                           &status);
    if (yField == HidField::Error) {
        error = status;
        return false;
    }

    // Both coordinates present is a position. Whether (0,0) also means "no
    // point" depends on the path: a device whose tip says the contact is down can
    // legitimately report the screen corner (the taskbar is on top on this
    // machine, so the corner is a real place to start a gesture), while the
    // coordinate-only fallback has nothing but this value and keeps the old
    // heuristic there.
    slot.hasPosition = xField == HidField::Ok && yField == HidField::Ok;
    slot.atOrigin = slot.hasPosition && x == 0 && y == 0;
    slot.rawX = static_cast<long>(x);
    slot.rawY = static_cast<long>(y);

    if (device.hasContactIdentifier) {
        ULONG identifier = 0;
        const HidField idField =
            ReadUsageValue(device, kUsagePageDigitizer, collection,
                           kUsageContactIdentifier, report, reportLength,
                           &identifier, &status);
        if (idField == HidField::Error) {
            error = status;
            return false;
        }
        slot.hasIdentifier = idField == HidField::Ok;
        slot.identifier = identifier;
    }

    slot.buttons =
        ReadContactButtons(device, collection, reportId, report, reportLength);
    if (slot.buttons.field == HidField::Error) {
        error = slot.buttons.status;
        return false;
    }

    slot.declared = xField == HidField::Ok || yField == HidField::Ok ||
                    slot.buttons.field == HidField::Ok;
    return true;
}

// What this report says about a slot. `Unknown` means it carries no tip and no
// in-range field for the slot, so it cannot say whether a contact is real; it is
// never read as a lift.
enum class HidContactState : uint8_t { Contact, NotContact, Unknown };

HidContactState SlotContactState(const HidContactSlot& slot) {
    if (slot.buttons.tipDeclared) {
        return slot.buttons.tipDown ? HidContactState::Contact
                                    : HidContactState::NotContact;
    }
    if (slot.buttons.inRangeDeclared) {
        return slot.buttons.inRangeDown ? HidContactState::Contact
                                        : HidContactState::NotContact;
    }
    return HidContactState::Unknown;
}

int32_t ToScreenPixel(long raw, const HidAxisRange& range, int screenSize) {
    if (!range.valid || screenSize <= 1) {
        return static_cast<int32_t>(raw);
    }

    const int64_t span = static_cast<int64_t>(range.max) - range.min;
    if (span <= 0) {
        return static_cast<int32_t>(raw);
    }

    const int64_t value = static_cast<int64_t>(raw) - range.min;
    int64_t pixel = value * (screenSize - 1) / span;
    if (pixel < 0) {
        pixel = 0;
    }
    if (pixel > screenSize - 1) {
        pixel = screenSize - 1;
    }
    return static_cast<int32_t>(pixel);
}

// The HID thread never sees the XAML thread's CoreWindow pointer. The UI
// thread stores it in Setup; if that did not happen, look it up once.
HWND FindStartCoreWindow() {
    DWORD processId = GetCurrentProcessId();
    HWND hwnd = FindWindowW(L"Windows.UI.Core.CoreWindow", nullptr);
    while (hwnd) {
        DWORD owner = 0;
        GetWindowThreadProcessId(hwnd, &owner);
        if (owner == processId) {
            return hwnd;
        }
        hwnd = FindWindowExW(nullptr, hwnd, L"Windows.UI.Core.CoreWindow",
                             nullptr);
    }
    return nullptr;
}

// Cached answer of StartWindowIsOpen() for the report handler. The cloaked
// query is a syscall, so one answer serves every report inside 20 ms; the
// cache is dropped on each lift so a down always starts from a fresh query.
bool g_openCheckValid = false;     // input thread only
bool g_openCheckOpen = false;      // input thread only
uint64_t g_lastOpenCheckTick = 0;  // input thread only

// The menu closes by cloaking its CoreWindow, not by destroying or hiding it.
// Called once per down; nothing is cached here, because the menu opens and
// closes many times on the same HWND.
//
// The read is tri-state on purpose. A failed attribute read (no window, no DWM
// entry point, a failed call) is neither open nor hidden: a confirmation that
// finds it hidden would otherwise be allowed to complete a close without ever
// seeing a hidden sample, and the visibility service would read a failure as a
// transition.
enum class CloakState { Open, Hidden, Unknown };

CloakState ReadCloakState() {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return CloakState::Unknown;
    }

    DwmGetWindowAttribute_t dwmGetWindowAttribute =
        g_dwmGetWindowAttribute.load(std::memory_order_relaxed);
    if (!dwmGetWindowAttribute) {
        return CloakState::Unknown;
    }

    DWORD cloaked = 0;
    const HRESULT hr =
        dwmGetWindowAttribute(hwnd, kDwmwaCloaked, &cloaked, sizeof(cloaked));
    if (hr != S_OK) {
        return CloakState::Unknown;
    }
    return cloaked == 0 ? CloakState::Open : CloakState::Hidden;
}

const wchar_t* CloakStateName(CloakState state) {
    switch (state) {
        case CloakState::Open:
            return L"open";
        case CloakState::Hidden:
            return L"hidden";
        default:
            return L"unknown";
    }
}

// The gesture paths want the binary answer: only an uncloaked window is open.
// A failed read is not "open", so admission and the unhealed rule keep their old
// conservative answer; the landing and the visibility service use the tri-state.
bool StartWindowIsOpen() {
    return ReadCloakState() == CloakState::Open;
}

// The candidate contact needs the open state on every report, but the cloaked
// query is a syscall. One answer serves all reports inside 20 ms; the cache is
// invalidated when a contact lifts, so a down always starts from a fresh one.
bool StartWindowIsOpenThrottled(uint64_t tickMs) {
    if (g_openCheckValid &&
        tickMs - g_lastOpenCheckTick < kOpenCheckIntervalMs) {
        return g_openCheckOpen;
    }

    g_lastOpenCheckTick = tickMs;
    g_openCheckValid = true;
    g_openCheckOpen = StartWindowIsOpen();
    return g_openCheckOpen;
}

float HidDpiScale() {
    static HWND cachedHwnd = nullptr;
    static float cachedScale = 1.0f;

    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        hwnd = FindStartCoreWindow();
        if (hwnd) {
            g_coreHwnd.store(hwnd, std::memory_order_relaxed);
        }
    }
    if (!hwnd) {
        return 1.0f;
    }

    if (hwnd != cachedHwnd) {
        const UINT dpi = GetDpiForWindow(hwnd);
        if (dpi == 0) {
            // The window may still be initializing; retry on the next report
            // instead of caching a wrong scale for the whole session.
            return 1.0f;
        }
        cachedHwnd = hwnd;
        cachedScale = static_cast<float>(dpi) / 96.0f;
    }

    return cachedScale;
}

struct HidGestureSession {
    bool active = false;
    bool suppress = false;  // ignore samples until all contacts are released
    long lastRawX = 0;
    long lastRawY = 0;
    float startX = 0.0f;
    float startY = 0.0f;
    // Progress where the drag began. A drag that takes over a running settle
    // continues from the position the panel is at, so the mapping is relative to
    // that base instead of always to the resting value.
    float startProgress = 1.0f;
    uint64_t startTick = 0;
    uint64_t lastTick = 0;
    // Last position sample on the QPC clock: where the coast measures its
    // silence from. GetTickCount64 only advances every 15.6 ms, so the QPC clock
    // is what tells an 8 ms gap from a 20 ms one.
    uint64_t lastSampleQpc = 0;
    // The liveness clock: the last report that carried positive evidence of the
    // contact, whether or not it had a position. The idle release reads this one
    // only, so a status report keeps the contact alive without making an old
    // velocity sample look current to the coast.
    uint64_t lastAliveQpc = 0;
    float curDelta = 0.0f;
    float maxDelta = 0.0f;
    float maxPerp = 0.0f;
    float lastProgress = 1.0f;
    int samples = 0;
};

HidGestureSession g_hidSession;  // input thread only

// Reveal follow. The contact that starts on the taskbar edge is remembered
// while the menu is closed. EP opens the menu by itself once the contact
// passes its own edge threshold, and the same contact is still down at that
// moment, so it takes Progress over and the menu follows it from there.
//
// `distance` is the root extent along the axis: Progress 1 leaves the root at
// rest, Progress 0 slides it out by Distance. `panelTrail` is the panel edge in
// absolute screen DIP that the finger meets: the reveal is
// Progress = 1 - (finger - panelTrail) / (dir * distance), so the panel's
// trailing edge would sit under the finger at every sample and the value no
// longer depends on when the menu was seen opening. That mapping is the target
// the follow chases, not the value it writes: `visible` moves toward it at
// `kSettleSpeedDipPerMs` and never past it, so a finger that was already at
// 0.9 when the menu opened is met within a fraction of a second instead of
// leaving the panel permanently short. `target` keeps the last mapped value for
// the `behind=` diagnostic.
// `travel` is the decision quantity: the distance the finger moved away from
// the taskbar edge since the touch began, clamped at 0, so pulling out and back
// reads 0 at release. `maxTravel` is the furthest it got; the log shows it.
// `progress` is the last visible value the chase produced, `lastInserted` the
// last one that reached the property set, so the 0.5 DIP change filter compares
// against what the renderer actually shows.
struct HidRevealSession {
    bool edgeCandidate = false;
    // The candidate arrived while a close owned the panel. It is measured (its
    // travel, its end-speed window and its release record) but it writes no
    // Progress and leaves the close's dismiss armed, so the retract is the only
    // motion; the close's landing releases it and the open adopts it.
    bool deferred = false;
    // The one open claim of this contact. It is set when the qualified pull
    // first reaches kAdoptTravelDip and attempts the claim, whatever the
    // outcome: a tap never sets it because a tap never reaches the threshold,
    // and a refused claim is not retried on later reports. An open the shell
    // performs anyway is adopted instead.
    bool openClaimAttempted = false;
    bool active = false;
    float edgeStartX = 0.0f;  // first report, kept for the edge-band test
    float edgeStartY = 0.0f;
    float distance = 0.0f;
    float panelTrail = 0.0f;
    float travel = 0.0f;
    float maxTravel = 0.0f;
    float progress = 0.0f;
    // The chased position written to Progress, and the finger's absolute mapped
    // position it moves toward. The difference is the catch-up left, logged as
    // `behind=` at the release.
    float visible = 0.0f;
    float target = 0.0f;
    // The gap the follow inherited at its takeover (mapped_at_activation - the
    // visible entry): the distance the chase has to close. Logged as `offset=`.
    float activationGap = 0.0f;
    // When the last chase step ran. The step is bounded by
    // kSettleSpeedDipPerMs * (now - chaseQpc), so the bound is a speed, not a
    // per-report distance.
    uint64_t chaseQpc = 0;
    float lastInserted = 1.0f;
    int samples = 0;
    uint64_t startTick = 0;
    uint64_t lastTick = 0;
    uint64_t lastSampleQpc = 0;  // position clock, see HidGestureSession
    uint64_t lastAliveQpc = 0;   // liveness clock, see HidGestureSession
    // QPC time of the report that took Progress over. The trail mapping is still
    // settling for the first 50 ms of the follow, so the coast stays off there.
    uint64_t activeSinceQpc = 0;
};

HidRevealSession g_reveal;  // input thread only

// The current contact's first and last positioned point, screen DIP, whether or
// not a session owns it. The reveal candidate is admitted on the down, but the
// shell can open the menu under a contact that started a little further in than
// the band: the menu then opens because of the same pull, and the finger that
// caused it is still down and moving. Adoption reads this record — the down
// point to measure travel from, the last point to place the panel at — so the
// follow continues from where the panel is instead of animating from 0. The
// record starts on the first positioned report of a contact and ends on the
// lift or the idle that ends the contact, so two contacts can never share a
// down point.
struct HidContactOrigin {
    bool valid = false;
    bool startedOpen = false;  // the menu was open when this contact went down
    float downX = 0.0f;
    float downY = 0.0f;
    float x = 0.0f;  // last positioned point
    float y = 0.0f;
    uint64_t downTick = 0;

    // The last positioned reports of this contact, oldest first. The reveal's
    // release window is replayed from them when the menu opens under the
    // contact, so the seam measures the finger even when the takeover lands one
    // report before the lift.
    struct Point {
        float x = 0.0f;
        float y = 0.0f;
        uint64_t qpc = 0;
    };
    Point points[kEndVelocitySamples];
    int pointCount = 0;
};

HidContactOrigin g_contactOrigin;  // input thread only

// Appends one positioned report of the current contact, oldest first. A full
// ring drops the oldest. Duplicates are kept on purpose: a finger that rests
// repeats its position, and the end-velocity window is what collapses that to a
// flat series.
void SampleOriginPoint(float x, float y, uint64_t qpcMs) {
    if (g_contactOrigin.pointCount == kEndVelocitySamples) {
        std::memmove(g_contactOrigin.points, g_contactOrigin.points + 1,
                     sizeof(HidContactOrigin::Point) *
                         (kEndVelocitySamples - 1));
        g_contactOrigin.pointCount--;
    }
    g_contactOrigin.points[g_contactOrigin.pointCount++] =
        HidContactOrigin::Point{x, y, qpcMs};
}

// A reveal pull that ended before the menu appeared. The shell's edge gesture can
// open the menu only after the finger lifts (measured on this machine: 107-251 ms
// after the release, because the shell finishes its gesture and then starts the
// open), and the contact is gone by then, so a live adoption has nothing to adopt.
// The release still knows everything the seam needs: the travel it reached, the
// mapped position the follow would have written first, and the end speed of that
// release. This record carries them across the gap, for a short window only, and
// the closed-to-open path starts the settle from them instead of from rest.
//
// Every field is written and read on the input thread: the release runs in the
// report handler or the idle tick, and the open is detected by the same thread's
// visibility poll (WM_TIMER or the waitable timer), so no publication exists and
// none is needed. `pullSerial` binds the record to the pull that produced it: a
// new admitted pull invalidates it, because that pull owns whatever opens next.
// A bare down does not: the contact that arrives first cannot be the pull that
// opened the menu.
struct RevealLateRelease {
    bool valid = false;
    uint64_t releaseQpc = 0;  // QPC; kLateRevealWindowMs bounds the age
    // A pull that lifted while a close still owned the panel. Its record only
    // becomes consumable when that close lands, so the age is measured from
    // `readyQpc` (the landing) instead of the release: the pull waited for the
    // close it must not fight, and the open that follows is still its own.
    bool deferred = false;
    uint64_t readyQpc = 0;    // QPC of the close's landing, 0 while none
    float travel = 0.0f;      // DIP away from the edge at the release
    // Mapped Progress the follow would have written; diagnostic only, never a
    // start value. The settle starts from the position the panel actually shows.
    float position = 0.0f;
    float velocity = 0.0f;    // Progress per ms along the opening axis
    uint32_t pullSerial = 0;  // the pull that produced it
};

RevealLateRelease g_lateRelease;  // input thread only

// How long a release may wait for the shell's open. Measured on this machine with
// ten fast pulls plus short slow ones from the bottom edge: the release line and
// the shell's open were 107-251 ms apart (the shell finishes its edge gesture after
// the lift and starts the open then), and a 120 ms window expired at 124 ms in the
// first run, before the open the record existed for. 300 ms covers the measured
// range with margin and stays short enough that it cannot reach the next
// interaction: a live takeover or a new admitted pull clears the record first.
constexpr uint64_t kLateRevealWindowMs = 300;

// True from the first report of a contact until it lifts. The edge test runs
// once per contact, never on a finger that slides into the band later.
bool g_contactDown = false;      // input thread only
uint64_t g_contactTick = 0;      // input thread only, last report of a contact

// Which decoded contact the mod follows: the raw-input device whose report
// carried it, the contact's own name when the device declares one, and the node
// index that name was last seen in. Contact identity has to be carried, not
// re-guessed per report: slot 2 can still hold the previous finger's coordinates
// while slot 0 is the live one, and a lift from the stale slot must not end the
// tracked finger. The node index alone is not an identity on a device that
// declares Contact Identifier: the device may hand the same node to another
// finger between two reports, and the identifier is what proves it did not.
//
// `lastEvidenceTick` is the identity's own clock, on the coarse tick the contact
// latch uses, and it moves only on evidence: a report that asserts the contact
// (`TipSwitch`/`InRange`, or a slot state of Contact) or carries a point this
// decoder feeds for it. A report that merely names the contact — a device that
// keeps an unused slot's retained identifier — moves nothing. That distinction
// is what makes a refused admission survivable: the layout handshake
// deliberately leaves the latch clear so the next report can retry, so a contact
// that then vanished without a lift would otherwise hold the identity forever,
// with the latch's cleanup unable to reach it and every later finger rejected as
// "another contact". The idle tick that ends contacts forgets an identity whose
// last evidence is that old, admitted or not.
struct HidContactTrack {
    HANDLE device = nullptr;
    int slot = -1;
    bool hasIdentifier = false;
    ULONG identifier = 0;
    uint64_t lastEvidenceTick = 0;
};

HidContactTrack g_trackedContact;  // input thread only

// One line per activation when a digitizer declares the same Contact Identifier
// down in two slots: the tracked slot wins, and the line names the ambiguity
// instead of logging it per report.
bool g_duplicateContactIdLogged = false;  // input thread only

void ForgetTrackedContact() {
    g_trackedContact = HidContactTrack{};
}

// Screen size in DIP, rewritten by FeedHidReport before every state-machine
// step. The right and bottom edge bands are measured from it.
float g_screenDipWidth = 0.0f;   // input thread only
float g_screenDipHeight = 0.0f;  // input thread only

// Set when a commit hands Progress to the settle animation. The reuse of one
// XAML root means the next drag can still see a translated root or a base
// value of 0, so BeginHidSession restores the resting value first.
bool g_commitNeedsProgressReset = false;  // input thread only
bool g_rootWasOpen = false;              // input thread only
uint64_t g_edgeLogTick = 0;              // input thread only, diagnostic
// The edge line was a measurement instrument: it is off in the shipping build.
constexpr bool g_debugEdge = false;

// ---------------------------------------------------------------------------
// A close suppresses the panel
// ---------------------------------------------------------------------------
//
// A gesture close and a keyboard close must leave the mod in one state, or the
// two families diverge where it matters: the keyboard close arrives while the
// finger is still on the glass and a session is still driving `Progress`. Without
// this the next report of that contact dragged the panel back and the release
// settled the menu open over a close that was already running.
//
// The ownership state is:
//
// - the suppression window is open, so the reopen poll does not restore the
//   resting value and no drag starts until the shell has hidden the window;
// - no gesture session owns Progress. `contactStillDown` mutes the rest of that
//   contact (its reports would otherwise start a new session, and its release
//   would settle an open); a gesture close arrives from its own release, where
//   the contact is already ending, so nothing is muted there. The mute is read
//   from the contact latch, so it covers the finger whether the dismiss session
//   or the reveal owned it;
// - the 1 ms timer period is released: no session is left that needs it.
//
// One helper, called by both families. It changes no dismiss slot: the
// keyboard close drops the armed dismiss at its caller, the gesture close arms
// one. The name answers only whether the close-suppression window is still open
// (`tickMs < g_suppressRestoreUntil`); the settle owns Progress, and
// CloseCancellableByPull() answers whether a pull may still abort the close. A
// pull that arrives while the suppression is true is deferred, never a second
// owner.
bool CloseInFlight(uint64_t tickMs) {
    return tickMs < g_suppressRestoreUntil.load(std::memory_order_relaxed);
}

// Whether a pull may abort the close it lands in. Only a gesture close still has
// a close this mod can cancel: its executor command is armed for the end of the
// settle, so dropping it aborts the close before the bridge is asked. That is
// the accepted cancel path, and it is one owner because the same call clears
// the suppression and the follow takes the panel from its live position.
// Every other close — a keyboard close, a Start-button or click-away close
// (both arrive as the bridge's close intent), or a gesture close whose dismiss
// already went out — has its dismiss in the shell's or the bridge's hand. A
// pull cannot abort that, so it is deferred and the close lands first.
bool CloseCancellableByPull() {
    return g_pendingTapTick != 0;
}

void TakeCloseOwnership(uint64_t tickMs, bool contactStillDown) {
    g_suppressRestoreUntil.store(tickMs + kRestoreSuppressMs,
                                 std::memory_order_relaxed);
    SwipeIpcSetControllerTouch(&g_ipc, false);
    // A new close decides the menu: an earlier close's confirmation, retained or
    // not, can no longer land this one's hide.
    ForgetConfirmedDismiss();
    // The controller publishes the close before its first frame. The bridge's
    // one toggle rule reads this direction for a Win, Start-button or click-away
    // press, so a press during the settle is a reopen, not a second close. The
    // shell's state cannot answer that question: on this path the panel is
    // already retracting while the window still reports state 5 and the cloak
    // still reports open until the executor's Dismiss runs. Publishing the
    // direction only at the settle's end left the whole settle readable
    // as "open", and a press in it restarted the close instead of taking the
    // menu back.
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionClosing);
    // A new close decides the menu: an earlier open that cancelled a close is
    // no longer owed.
    g_intentCancel = CloseCancelTicket{};

    // A new close decides the menu, so a release record left by an earlier pull
    // is moot: its open either already came or never will. The record of a pull
    // that lands during *this* close is written after this line, so it is not
    // touched here.
    g_lateRelease = RevealLateRelease{};

    // The mute belongs to the contact, not to the session that happened to own
    // it. Read the latch and both owners before either is cleared: a keyboard
    // close can arrive while the *reveal* owns the contact, and clearing only
    // the reveal flags left the finger live, so its next report could start a
    // dismiss session on the reopened menu and its release could settle an open.
    //
    // A reveal that is still driving the panel is the exception: it is the pull
    // that may open the menu this close makes room for, so it becomes a deferred
    // candidate (measured, never written) instead of a muted contact. The next
    // open releases it, exactly as it releases a pull that landed during the
    // close; the takeover owns the screen until then.
    const bool contactLive =
        g_contactDown || g_hidSession.active || g_reveal.edgeCandidate;
    const bool liveReveal =
        contactStillDown && (g_reveal.active || g_reveal.deferred);
    if (contactStillDown && contactLive && !liveReveal) {
        g_hidSession.suppress = true;
    }

    g_hidSession.active = false;
    // A live reveal keeps its candidate and its deferral; a keyboard close must
    // not leave it owning the panel, and its release must not settle an open.
    // Only the active flag is cleared; the measurements the release line prints
    // stay readable. A pull deferred by an earlier close is superseded only when
    // there is no live reveal to carry over.
    if (!liveReveal) {
        g_reveal.deferred = false;
        g_reveal.edgeCandidate = false;
    }
    g_reveal.active = false;
    EndContactTiming();
    // Nothing owns the panel edge any more, so a settled measurement may move it
    // even though the finger is still on the glass.
    PublishPanelOwnership();
}

// The coast runs on the finger's own speed, smoothed over the last three
// reports. At the panel's ~170 Hz that is a 12 ms window: long enough to ignore
// one slow report, short enough to follow a real stop. Each sample is the mapped
// progress and the QPC time it arrived at. Progress moves with the panel in both
// gestures, so its sign needs no per-gesture correction.
struct CoastVelocitySample {
    float progress = 0.0f;
    uint64_t qpc = 0;
};

constexpr int kCoastVelocitySamples = 3;
CoastVelocitySample g_coastVelocity[kCoastVelocitySamples];  // input thread
int g_coastVelocityCount = 0;                                // input thread

void ResetCoastVelocity() {
    g_coastVelocityCount = 0;
}

// Keeps the last three reports and drops the oldest.
void SampleCoastVelocity(float progress, uint64_t qpcMs) {
    if (g_coastVelocityCount == kCoastVelocitySamples) {
        std::memmove(g_coastVelocity, g_coastVelocity + 1,
                     sizeof(CoastVelocitySample) *
                         (kCoastVelocitySamples - 1));
        g_coastVelocityCount--;
    }
    g_coastVelocity[g_coastVelocityCount++] =
        CoastVelocitySample{progress, qpcMs};
}

// Progress per millisecond over the whole window. The 6 DIP/ms cap lives in DIP
// space: on this 1440 DIP root it is a fast flick, and one injected jump between
// two reports must not throw the panel across the screen.
float CoastVelocity(float distance) {
    if (g_coastVelocityCount < 2 || !(distance > 0.0f)) {
        return 0.0f;
    }

    const CoastVelocitySample& oldest = g_coastVelocity[0];
    const CoastVelocitySample& newest =
        g_coastVelocity[g_coastVelocityCount - 1];
    if (newest.qpc <= oldest.qpc) {
        return 0.0f;
    }

    const float elapsedMs = static_cast<float>(newest.qpc - oldest.qpc);
    const float velocity = (newest.progress - oldest.progress) / elapsedMs;
    const float limit = kCoastMaxVelocityDip / distance;
    return std::clamp(velocity, -limit, limit);
}

// ---------------------------------------------------------------------------
// The release speed: the end of the gesture, not its average
// ---------------------------------------------------------------------------
//
// `travel / elapsed` over the whole session is not a release speed. A finger
// that dragged 300 DIP, rested for 1.2 s and then lifted reported 0.20 DIP/ms,
// and the settle left the seam at a speed the finger did not have; the settings
// page's promise that a held finger starts from rest was false. The release
// speed is therefore measured from the reports inside one short window at the
// end of the gesture, in two steps: a freshness check on the newest position,
// then a span measured back from that position.
//
// - Freshness: the newest stored position must be no more than 30 ms older than
//   the release. A status-only hold keeps the contact alive without feeding a
//   position, and the speed from before that hold is not the release speed.
// - Span: 8-30 ms, measured backward from the newest position and not from the
//   release, so the pair can lie up to 60 ms before the release. At most 30 ms so
//   a stop is seen: a finger that held still repeats one value across the span
//   and the answer is 0. At least 8 ms so one report cannot decide it.
// - Elapsed between the first and the last report of that window.
//
// The sample value is Progress: the quantity the follow inserts and the settle
// animates, which is what lets one estimator serve both gesture families. The
// projection onto the gesture axis is already in that mapping (AxisDelta carries
// the dismiss axis sign, AxisOpeningSign the reveal direction) and so is the
// division by the same `Distance` the settle plans its travel with. A report that
// did not move the panel only moves the window's end, so a contact resting on the
// glass cannot accumulate speed from an old motion.
//
// The value is signed along the gesture axis: positive means Progress is rising.
// `SpeedTowardDip` projects it onto the settle's direction and clamps a release
// that is moving away from its target to 0.
constexpr uint64_t kEndVelocityWindowMs = 30;
constexpr uint64_t kEndVelocityMinMs = 8;
// The `vel window` line prints the whole window one release reads; off in the
// shipping build, like g_debugCoast.

struct EndVelocitySample {
    float progress = 0.0f;
    uint64_t qpc = 0;
};

EndVelocitySample g_endVelocity[kEndVelocitySamples];
int g_endVelocityCount = 0;  // input thread only
int g_endVelocityCalls = 0;  // input thread only, diagnostic
int g_endVelocityRepeats = 0;  // input thread only, diagnostic

void ResetEndVelocity() {
    g_endVelocityCount = 0;
    g_endVelocityCalls = 0;
    g_endVelocityRepeats = 0;
}

// Keeps the last few reports of the moving panel. A report that repeats the last
// value did not move the panel: it only makes the window end here.
void SampleEndVelocity(float progress, uint64_t qpcMs) {
    g_endVelocityCalls++;
    if (g_endVelocityCount > 0) {
        EndVelocitySample& newest = g_endVelocity[g_endVelocityCount - 1];
        if (newest.progress == progress) {
            g_endVelocityRepeats++;
            newest.qpc = qpcMs;
            return;
        }
    }

    if (g_endVelocityCount == kEndVelocitySamples) {
        std::memmove(g_endVelocity, g_endVelocity + 1,
                     sizeof(EndVelocitySample) * (kEndVelocitySamples - 1));
        g_endVelocityCount--;
    }
    g_endVelocity[g_endVelocityCount++] = EndVelocitySample{progress, qpcMs};
}

// Progress per millisecond at the release, signed along the gesture axis. 0 when
// the window holds no movement, which is what a held contact reports.
//
// Two checks, and they are separate. First freshness: the newest stored position
// must be no more than `kEndVelocityWindowMs` older than the release, or the
// answer is 0 — an asserted contact with no point keeps the gesture alive without
// feeding a position, so a finger that moved quickly and then only held its tip
// down must not hand the settle the velocity from before the hold. Second, the
// span: it is measured backward from that newest position, not from the release.
// Its oldest sample is the first one at least `kEndVelocityMinMs` and at most
// `kEndVelocityWindowMs` older than the newest, so the pair is 8-30 ms wide and
// may lie up to 60 ms before the release when the stream paused just before the
// lift. The whole pair therefore need not lie inside the last 30 ms.
float EndVelocityProgressPerMs(uint64_t releaseQpc) {
    float result = 0.0f;
    int firstIndex = -1;

    do {
        if (g_endVelocityCount < 2) {
            break;
        }

        const EndVelocitySample& newest =
            g_endVelocity[g_endVelocityCount - 1];
        if (releaseQpc > newest.qpc &&
            releaseQpc - newest.qpc > kEndVelocityWindowMs) {
            break;
        }
        const EndVelocitySample* first = nullptr;
        // Oldest to newest, so the ages only shrink. The first entry inside the
        // band is the oldest one that is at least 8 ms old and at most 30 ms
        // old, which is the widest window the rules allow; every later entry is
        // shorter. A sample younger than 8 ms ends the scan because every later
        // one is younger still. No entry in the band (one report, equal
        // timestamps, or a stream coarser than the window) answers zero without
        // a division.
        for (int i = 0; i < g_endVelocityCount - 1; ++i) {
            const uint64_t age =
                newest.qpc >= g_endVelocity[i].qpc
                    ? newest.qpc - g_endVelocity[i].qpc
                    : 0;
            if (age > kEndVelocityWindowMs) {
                continue;
            }
            if (age < kEndVelocityMinMs) {
                break;
            }
            first = &g_endVelocity[i];
            firstIndex = i;
            break;
        }

        if (!first || newest.qpc <= first->qpc) {
            break;
        }

        const float elapsed = static_cast<float>(newest.qpc - first->qpc);
        if (!(elapsed > 0.0f)) {
            break;
        }
        result = (newest.progress - first->progress) / elapsed;
    } while (false);

    if (g_debugVelocity) {
        const uint64_t newestQpc =
            g_endVelocityCount > 0
                ? g_endVelocity[g_endVelocityCount - 1].qpc
                : 0;
        const uint64_t age =
            releaseQpc > newestQpc ? releaseQpc - newestQpc : 0;
        const uint64_t firstAge =
            firstIndex >= 0 ? newestQpc - g_endVelocity[firstIndex].qpc : 0;
        const float delta =
            firstIndex >= 0
                ? g_endVelocity[g_endVelocityCount - 1].progress -
                      g_endVelocity[firstIndex].progress
                : 0.0f;
        TraceLog(L"vel window count=%d calls=%d repeats=%d rel=%llu newest=%llu "
                L"age=%llu first=%d firstage=%llu d=%.5f raw=%.4f",
                g_endVelocityCount, g_endVelocityCalls, g_endVelocityRepeats,
                static_cast<unsigned long long>(releaseQpc),
                static_cast<unsigned long long>(newestQpc),
                static_cast<unsigned long long>(age), firstIndex,
                static_cast<unsigned long long>(firstAge), delta, result);
    }

    return result;
}

// ---------------------------------------------------------------------------
// One activation's input state
// ---------------------------------------------------------------------------
//
// This is the complete list of what one activation of this image leaves
// behind in the input layer. Every start and every stop of an activation calls
// it, so a field added later has one obvious home:
//
// - the tracked contact identity and the contact latch;
// - both gesture sessions (the dismiss drag and the reveal follow);
// - the close mute and the deferred-layout flag, plus the panel-ownership
//   publication derived from the sessions;
// - the pending dismiss and the pull generation it was armed under;
// - the executor sequences and deadlines (show, dismiss), the pending open entry
//   and the last consumed intent sequence;
// - the recovery ticket of an open that cancelled a close, and the activation's
//   peer epoch (the next attach claims a new one);
// - the coast and both velocity windows;
// - the diagnostic latches that steer behaviour, and the `anim` debug override.
//
// It runs before the input thread exists (Wh_ModInit, after the guard proved no
// previous activation is live) or on that thread as it stops (HidInputThread).
// It never runs concurrently with a report handler.
void ResetActivationState() {
    ForgetTrackedContact();
    g_duplicateContactIdLogged = false;
    g_contactDown = false;
    g_contactTick = 0;
    g_contactOrigin = HidContactOrigin{};
    g_lateRelease = RevealLateRelease{};

    g_hidSession = HidGestureSession{};
    g_reveal = HidRevealSession{};

    g_suppressRestoreUntil.store(0, std::memory_order_relaxed);
    g_measureDeferred.store(false, std::memory_order_relaxed);
    g_panelOwned.store(false, std::memory_order_relaxed);
    g_dismissSettling.store(false, std::memory_order_relaxed);
    g_dismissSerial.store(0, std::memory_order_relaxed);

    g_pendingTapTick = 0;
    g_pendingTapKind = CloseKind::None;
    g_pendingTapPull = 0;
    g_pullSerial = 0;
    g_touchRegistered = false;
    g_penRegistered = false;
    // The executor side of the same activation: a sequence or a deadline from the
    // previous one would answer this activation's channel with a command nobody
    // wrote, and an open entry would seed the next menu from a menu that is gone.
    g_pendingShowExec.store(0, std::memory_order_relaxed);
    g_pendingShowUntil.store(0, std::memory_order_relaxed);
    // The confirmation slot names a command of the previous activation; its ack
    // can no longer arrive through this activation's channel, and neither its
    // generation nor its retained deadline may survive.
    ForgetPendingDismiss();
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    g_intentOpenEntry.store(0.0f, std::memory_order_relaxed);
    // The attach seeds the next intent boundary; 0 here means this activation has
    // acted on nothing, and OpenIpcChannel overwrites it before the thread runs.
    g_lastIntentSeq = 0;
    g_intentCancel = CloseCancelTicket{};
    // The next attach claims a new identity; until then this activation has none.
    g_peerEpoch = 0;

    g_panel.coasting.store(false, std::memory_order_relaxed);
    g_panel.coastStartQpc = 0;
    g_panel.coastFrom = 1.0f;
    g_panel.coastTo = 1.0f;
    ResetCoastVelocity();
    ResetEndVelocity();

    g_commitNeedsProgressReset = false;
    g_rootWasOpen = false;
    g_openCheckValid = false;
    g_openCheckOpen = false;
    g_lastOpenCheckTick = 0;
    g_skipLogTick = 0;
    g_unhealedLogTick = 0;
    g_edgeLogTick = 0;
    g_scrollerRejectLogTick = 0;
    g_bandTailLogTick = 0;
    g_closeCancelledTick = 0;

    // No reveal of this activation names a menu any more. The open tick is the
    // only published identity left; there is no claim record to withdraw. The
    // liveness throttle starts over with the next OpenIpcChannel.
    g_ipcOpenTickLow = 0;
    g_peerTickLast = 0;

    EndContactTiming();
    TraceLog(L"activation state reset");
}

// Input thread. Claims the panel for a gesture admission and re-reads the
// layout reservation. The seq_cst pair with LayoutApplyGuard::Begin is the
// handshake: either the apply sees this claim and defers whole, or this claim
// sees the apply and refuses the admission, whose next report retries it. A
// refused claim restores the derived ownership, so a report that admitted
// nothing leaves `g_panelOwned` truthful.
bool ClaimPanelForAdmission() {
    g_panelOwned.store(true, std::memory_order_seq_cst);
    if (g_layoutApplying.load(std::memory_order_seq_cst) != 0) {
        g_panelOwned.store(g_hidSession.active || g_reveal.edgeCandidate,
                           std::memory_order_seq_cst);
        return false;
    }
    return true;
}

float AxisDelta(int32_t axis, const HidGestureSession& session, float x,
                float y) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            return session.startX - x;
        case DismissAxis::Right:
            return x - session.startX;
        case DismissAxis::Top:
            return session.startY - y;
        case DismissAxis::Bottom:
            return y - session.startY;
    }
    return session.startX - x;
}

float PerpDelta(int32_t axis, const HidGestureSession& session, float x,
                float y) {
    if (IsHorizontalAxis(axis)) {
        return std::fabs(y - session.startY);
    }
    return std::fabs(x - session.startX);
}

bool PointInStartWindow(float x, float y) {
    const int64_t left = g_winRectDip[0].load(std::memory_order_relaxed);
    const int64_t top = g_winRectDip[1].load(std::memory_order_relaxed);
    const int64_t right = g_winRectDip[2].load(std::memory_order_relaxed);
    const int64_t bottom = g_winRectDip[3].load(std::memory_order_relaxed);

    // Without published bounds, do not reject a plausible touch.
    if (right <= left || bottom <= top) {
        return true;
    }

    return x >= static_cast<float>(left) - kWindowMarginDip &&
           x <= static_cast<float>(right) + kWindowMarginDip &&
           y >= static_cast<float>(top) - kWindowMarginDip &&
           y <= static_cast<float>(bottom) + kWindowMarginDip;
}

// The taskbar sits on the side the dismiss axis slides out along, so that axis
// also names the band the reveal starts from: the bar's own strip plus one bar
// thickness toward the desktop, as the snapshot publishes it. `kEdgeBandDip` is
// the fallback while no snapshot has carried a band yet (a fresh process, or a
// taskbar probe that named no edge), which is the same rule over a smaller span.
bool RevealBandDip(const GestureGeometry& geometry, float& low, float& high) {
    const int32_t axis = geometry.axis();
    if (geometry.band) {
        low = std::min(geometry.bandLow, geometry.bandHigh);
        high = std::max(geometry.bandLow, geometry.bandHigh);
        return high > low;
    }
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            low = 0.0f;
            high = kEdgeBandDip;
            return true;
        case DismissAxis::Top:
            low = 0.0f;
            high = kEdgeBandDip;
            return true;
        case DismissAxis::Right:
            if (!(g_screenDipWidth > kEdgeBandDip)) {
                return false;
            }
            low = g_screenDipWidth - kEdgeBandDip;
            high = g_screenDipWidth;
            return true;
        case DismissAxis::Bottom:
            if (!(g_screenDipHeight > kEdgeBandDip)) {
                return false;
            }
            low = g_screenDipHeight - kEdgeBandDip;
            high = g_screenDipHeight;
            return true;
    }
    return false;
}

// Finger position on the gesture axis in absolute screen DIP. The reveal
// formula compares it to the panel trail directly, so the same drag yields the
// same progress on every open.
float AxisScreenPosition(int32_t axis, float x, float y) {
    return IsHorizontalAxis(axis) ? x : y;
}

// The direction the menu slides out along, as the DirX/DirY sign: -1 for a
// left or top taskbar, +1 for right or bottom.
float AxisDirection(int32_t axis) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
        case DismissAxis::Top:
            return -1.0f;
        case DismissAxis::Right:
        case DismissAxis::Bottom:
            return 1.0f;
    }
    return -1.0f;
}

// The direction a revealing finger moves away from the taskbar edge: the
// opposite of AxisDirection, so +1 for a left or top taskbar and -1 for right
// or bottom. The reveal's travel and velocity use it; the visual mapping does
// not, because that one already divides by AxisDirection.
float AxisOpeningSign(int32_t axis) {
    return -AxisDirection(axis);
}

// How far the finger has moved away from the taskbar edge since the contact
// began, signed by the opening direction and clamped at 0: a pull back toward
// the edge reads 0. The reveal's travel, the open-ownership test and the
// adoption rule all read this one number.
float RevealTravelDip(int32_t axis, float downX, float downY, float x, float y) {
    return std::max(
        0.0f,
        (AxisScreenPosition(axis, x, y) -
         AxisScreenPosition(axis, downX, downY)) *
            AxisOpeningSign(axis));
}

// ---------------------------------------------------------------------------
// The geometry snapshot, read side
// ---------------------------------------------------------------------------

// The rect side the panel is revealed toward: the side away from the taskbar,
// which is the one that meets the finger. A left taskbar reveals the panel's
// right edge, a top taskbar its bottom, a right taskbar its left, a bottom
// taskbar its top. This is the only place the mapping exists, so a published rect
// and the edge derived from it cannot disagree.
float TrailEdge(int32_t axis, float left, float top, float right, float bottom) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            return right;
        case DismissAxis::Top:
            return bottom;
        case DismissAxis::Right:
            return left;
        case DismissAxis::Bottom:
            return top;
    }
    return right;
}

float GeometryTrailDip(const GestureGeometry& geometry) {
    return geometry.panel
               ? TrailEdge(geometry.axis(), geometry.left, geometry.top,
                           geometry.right, geometry.bottom)
               : 0.0f;
}

// The trail a reader uses when no panel has been measured: the root-width travel
// this mod shipped with, whose rest position is the far screen edge minus the
// travel. `distance` spans the screen, so on a left or top taskbar that position
// is the travel itself and on a right or bottom one it is 0, the screen origin.
// Both put Progress 0 at the taskbar edge and Progress 1 at the far edge, so a
// reveal that starts before the first good measurement still follows the finger
// on either side instead of pinning the panel open.
float FallbackTrailDip(int32_t axis, float distance) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
        case DismissAxis::Top:
            return distance;
        case DismissAxis::Right:
        case DismissAxis::Bottom:
            return 0.0f;
    }
    return distance;
}

// The smallest `Progress` at which any of the panel is on screen: the value whose
// translated trail edge sits on the work area's edge on the side the panel leaves
// through. The composition moves that edge by `(1 - Progress) * Direction *
// Distance`, so the answer is `1 - Direction * (edge - trail) / Distance`, and a
// measured panel's trail comes from the snapshot; without one the fallback trail
// is defined so the answer is 0, exactly the old behaviour.
//
// The live follow starts here, not at the off-screen rest. A settle may start at
// 0 because it spends time crossing the hidden travel (the 409 ms open crosses
// it in about 50 ms), but a follow spends the finger's own travel: starting at 0
// made the panel one hidden travel behind the finger for the whole gesture
// one hidden travel behind the finger. Starting at the entry puts the first
// visible frame exactly on the screen edge, and the lag is then only the travel
// the cloak hid. The work area, not the monitor, is the boundary: a docked bar's
// strip is where the panel stops being visible, and a hidden bar reserves
// nothing, so the two agree on this machine.
float RevealEntryProgress(const GestureGeometry& geometry) {
    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        return 0.0f;
    }
    const int32_t axis = geometry.axis();
    const float direction = AxisDirection(axis);
    const float trail = geometry.panel ? GeometryTrailDip(geometry)
                                       : FallbackTrailDip(axis, distance);
    const bool horizontal = IsHorizontalAxis(axis);
    const float edge =
        direction > 0.0f
            ? (horizontal ? geometry.layout.workRight : geometry.layout.workBottom)
            : (horizontal ? geometry.layout.workLeft : geometry.layout.workTop);
    return std::clamp(1.0f - direction * (edge - trail) / distance, 0.0f, 1.0f);
}

// Whether a press belongs to the panel. The measured rectangle decides it, with
// the 2 DIP the rect's own border needs. A panel that was never measured cannot
// place a press, so it does not reject one: the window test and the gesture's own
// guards still apply, and a wrong "outside" would swallow a real dismiss.
bool PanelContainsPress(const GestureGeometry& geometry, float x, float y) {
    if (!geometry.panel) {
        return true;
    }
    return x >= geometry.left - kPanelBandSlopDip &&
           x <= geometry.right + kPanelBandSlopDip &&
           y >= geometry.top - kPanelBandSlopDip &&
           y <= geometry.bottom + kPanelBandSlopDip;
}

// Input thread only: the only reader of the snapshot. The write side reads
// `g_geometry` under the same lock, because it is the thread that writes it.
//
// The writer holds g_geometryLock for one struct assignment, so a shared
// acquisition normally succeeds outright. If it does not, the caller answers the
// last whole copy instead of blocking a report; a new gesture asks for
// `waitForWriter` because it must not start from a snapshot the writer is
// replacing at that instant.
GestureGeometry ReadGestureGeometry(bool waitForWriter) {
    if (!waitForWriter) {
        if (TryAcquireSRWLockShared(&g_geometryLock)) {
            g_inputGeometry = g_geometry;
            ReleaseSRWLockShared(&g_geometryLock);
            return g_inputGeometry;
        }
        return g_inputGeometry;
    }

    AcquireSRWLockShared(&g_geometryLock);
    g_inputGeometry = g_geometry;
    ReleaseSRWLockShared(&g_geometryLock);
    return g_inputGeometry;
}

// Whether this snapshot describes the layout that is still current. A cached
// copy from before a resize fails the test: the cached struct's identity token
// is older than the one the writer published with the new layout.
bool GeometryLayoutCurrent(const GestureGeometry& geometry) {
    return geometry.layoutVersion ==
           g_layoutVersion.load(std::memory_order_acquire);
}

// Whether a vertical dismiss may start here. A vertical drag competes with the
// content's own scrolling: dragging toward the taskbar is how a list scrolls
// back, and a dismiss started on that drag closed the menu instead. The gate asks
// the region *under the press*, never one pre-cached element:
//
// - the survey state is Nothing: it measured that nothing in the panel scrolls
//   vertically, so the drag has no content to scroll and belongs to the dismiss;
// - the state is Unknown: no whole measurement speaks for this open. Wait for
//   the settled pass, but only while the press is inside the panel: a press on
//   the desktop still passes;
// - Scrollable: the innermost region whose rect contains the press decides. No
//   such region (the panel background, the rail, the taskbar strip, a scroll view
//   at rest with nothing to scroll) means the drag has no content to scroll;
// - inside one, the drag belongs to the scroll unless that region is already at
//   the end the drag would scroll away from (offset 0 for a bottom taskbar,
//   offset == scrollable height for a top one), in which case there is nothing
//   left to scroll and the dismiss may start;
// - otherwise the same drag would scroll that region, so the dismiss waits, and
//   the line names the region. The contact is not consumed: BeginHidSession runs
//   again on every later sample, so once the user has scrolled the region to its
//   end the rest of that same drag becomes a dismiss.
//
// Left and right keep the plain drag: a sideways drag does not scroll a vertical
// list.
bool VerticalDismissAllowed(const GestureGeometry& geometry, int32_t axis,
                            float x, float y, uint64_t tickMs) {
    if (IsHorizontalAxis(axis)) {
        return true;
    }

    ScrollerCache scroller{};
    {
        std::lock_guard lock(g_scrollerMutex);
        scroller = g_scroller;
    }

    // A survey measured for another open says nothing about this one. The close
    // path releases it, but a reopen that lands before that deferred turn runs
    // must not gate its first press against the previous menu's rects.
    if (scroller.generation !=
        g_openGeneration.load(std::memory_order_acquire)) {
        scroller = ScrollerCache{};
    }
    // A resize inside one open keeps the generation, so the layout token is the
    // only thing that says the rects describe a panel the shell has moved since.
    if (scroller.layoutVersion !=
        g_layoutVersion.load(std::memory_order_acquire)) {
        scroller = ScrollerCache{};
    }

    if (scroller.state == ScrollerState::Nothing) {
        return true;  // measured: nothing in this panel can take the drag
    }
    if (scroller.state == ScrollerState::Unknown) {
        // No measurement yet, or only the transitional one. Failing open here let
        // a swipe right after the open dismiss the menu while the user was
        // scrolling; wait for the settled pass instead, but only while the press
        // is inside the panel so a press on the desktop still passes.
        return !PanelContainsPress(geometry, x, y);
    }

    // The innermost region containing the press. Depth beats area, so a list
    // inside a scroll host wins over the host; at equal depth the smaller rect
    // wins, because the more specific element is the one a press lands on.
    const ScrollerRegion* hit = nullptr;
    for (int i = 0; i < scroller.count; ++i) {
        const ScrollerRegion& region = scroller.regions[i];
        if (x < region.left || x > region.right || y < region.top ||
            y > region.bottom) {
            continue;
        }
        if (!hit || region.depth > hit->depth ||
            (region.depth == hit->depth &&
             (region.right - region.left) * (region.bottom - region.top) <
                 (hit->right - hit->left) * (hit->bottom - hit->top))) {
            hit = &region;
        }
    }
    if (!hit) {
        return true;  // the press is not on any scrolling content
    }

    const bool atEnd =
        axis == static_cast<int32_t>(DismissAxis::Bottom)
            ? hit->verticalOffset <= kScrollerEndSlopDip
            : hit->verticalOffset >=
                  hit->scrollableHeight - kScrollerEndSlopDip;
    if (atEnd) {
        return true;
    }

    // At most one line per window: the digitizer keeps reporting while the
    // finger rests, and each report asks again. The name says which region took
    // the drag, because the rule is the region under the press and not one
    // cached element.
    if (tickMs >= g_scrollerRejectLogTick &&
        tickMs - g_scrollerRejectLogTick >= kDismissRejectLogMs) {
        g_scrollerRejectLogTick = tickMs;
        TraceLog(L"dismiss deferred to scroll vo=%.0f sh=%.0f region=%s",
                hit->verticalOffset, hit->scrollableHeight, hit->label);
    }
    return false;
}

// The stores both own-opens write: the entry, its insertion, the pending open and
// its entry, the controller's direction and touch, and the show request. The two
// callers keep their own preconditions and their own log line — the band admits a
// contact, a landing serves a close — and this function owns exactly the writes
// they share. `touch` is the shared click-away guard: it is true only for a
// contact that is still down and owns the panel, never for a pull that already
// lifted (a late release record) or for a close-cancel recovery. The closed
// keyboard intent's `g_panel.Begin` is deliberately not
// here: that path starts the controller at the entry, while these two replace a
// panel that is being pulled or restored from a settled position.
//
// False means the show could not be requested. Nothing may claim an open that no
// executor can present, so the claim is withdrawn in one step and the caller
// leaves the panel to the path that can still open it.
bool PublishOpenClaim(const GestureGeometry& geometry, bool touch,
                      float& entryOut) {
    const float entry = std::clamp(RevealEntryProgress(geometry), 0.0f, 1.0f);
    InsertProgress(entry);
    g_intentOpenActive.store(true, std::memory_order_relaxed);
    g_intentOpenEntry.store(entry, std::memory_order_relaxed);
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
    SwipeIpcSetControllerTouch(&g_ipc, touch);
    entryOut = entry;
    if (!RequestBridgeShow(1, 0)) {
        ReleaseExecutorOwnership();
        return false;
    }
    return true;
}

// Starts one reveal follow. Both entry points come through here — a contact
// admitted on the down because it began inside the taskbar band, and a contact
// adopted because the menu opened under it while it was already pulling — so the
// clocks and the release history cannot diverge. `downX/downY` is the first
// positioned point of the contact and is what travel is measured from, whatever
// moment the follow actually takes the panel over.
//
// Admission is inert: it records the contact and its down point and nothing
// else. The window is requested by TryClaimRevealOpen, on the first positioned
// report that reaches the travel threshold; a tap anywhere in the band never
// gets there.
//
// `deferred` names a contact that arrived while a close owned the panel. It is
// the pull that opens the *next* menu, so it changes nothing about this close:
// it neither claims nor cancels one, which is what keeps the close's dismiss
// armed and its retract the only motion. `ActivateRevealFollow`
// refuses while the close is in flight, so the session stays a candidate that is
// measured but never written.
void StartRevealFollow(float downX, float downY, uint64_t downTick,
                       uint64_t tickMs, bool deferred = false) {
    g_reveal = HidRevealSession{};
    g_reveal.edgeCandidate = true;
    g_reveal.deferred = deferred;
    g_reveal.edgeStartX = downX;
    g_reveal.edgeStartY = downY;
    g_reveal.startTick = downTick;
    g_reveal.lastTick = tickMs;
    g_reveal.lastSampleQpc = QpcMsNow();
    g_reveal.lastAliveQpc = g_reveal.lastSampleQpc;
    BeginContactTiming();
    // The release speed belongs to this contact only.
    ResetEndVelocity();
}

// A qualified pull may still abort a close this mod owns: the finger has
// crossed kAdoptTravelDip while the close's own dismiss was still armed, so no
// Dismiss has been requested and the menu can stay open. It must not run on
// contact-down: a contact that never enters the band or never travels would
// otherwise keep a menu the user was closing.
bool CancelCloseForQualifiedPull(uint64_t tickMs) {
    if (!CloseInFlight(tickMs) || !CloseCancellableByPull()) {
        return false;
    }
    if (!CancelCloseSchedules(L"pull").slot) {
        return false;
    }
    g_suppressRestoreUntil.store(0, std::memory_order_relaxed);
    g_closeCancelledTick = tickMs;
    // The close is gone and the menu stays open, so the controller owns an open
    // again. The direction must follow: closing with a shown menu would turn
    // the next press into a second reopen.
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
    const float live = TakeProgressNow(L"pull");
    const float distance = ReadGestureGeometry().distance();
    if (distance > 0.0f &&
        (1.0f - live) * distance < kProgressEpsilonDip) {
        ShowAtRest();
    } else {
        std::chrono::milliseconds duration{0};
        if (BeginGestureSettle(live, 1.0f, 0.0f, duration)) {
            g_commitNeedsProgressReset = false;
        }
    }
    return true;
}

// The one open claim of a band contact. Admission records the contact; the
// window is requested here, on the first positioned report whose signed inward
// travel reaches kAdoptTravelDip. A tap anywhere in the band — twice the
// taskbar's thickness — never requests a show, so it stays a native press.
//
// One attempt per contact, marked before the outcome: a refused claim means no
// executor can present the open, and asking again on every later report would
// turn one failed request into a stream of them. An open the shell performs
// anyway is adopted by this same contact.
void TryClaimRevealOpen(const GestureGeometry& geometry, float x, float y,
                        uint64_t tickMs) {
    HidRevealSession& reveal = g_reveal;
    if (!reveal.edgeCandidate || reveal.active || reveal.openClaimAttempted) {
        return;
    }
    const float travel = RevealTravelDip(geometry.axis(), reveal.edgeStartX,
                                         reveal.edgeStartY, x, y);
    if (!(travel >= kAdoptTravelDip)) {
        return;
    }
    reveal.openClaimAttempted = true;
    TraceLog(L"reveal pull qualified travel=%.0f axis=%d", travel,
            geometry.axis());
    // The pull owns whatever opens next: a new generation and no release record
    // from the previous pull. These are the admission's old ownership side
    // effects; they run at the qualified pull so a tap leaves them alone, and
    // before the close test so an aborted close cannot leave the old record.
    g_pullSerial++;
    g_lateRelease = RevealLateRelease{};
    // A close this mod can still abort is aborted here and only here. This is
    // the qualified moment; contact-down must not cancel a close.
    if (CancelCloseForQualifiedPull(tickMs)) {
        // The close is gone; the follow takes the still-open menu over from
        // this report.
        reveal.deferred = false;
        return;
    }
    if (reveal.deferred) {
        // A close that cannot be aborted owns the panel. The landing releases
        // the deferral and reopens for a qualified pull.
        return;
    }
    // The cloak answers "open?": a window already showing is a panel the follow
    // takes over on this same report, and an open this side already requested is
    // on its way. Neither may be answered with a second show.
    if (StartWindowIsOpen() ||
        g_intentOpenActive.load(std::memory_order_relaxed)) {
        return;
    }
    // A close that began after admission and cannot be aborted still owns the
    // panel: this pull waits for its landing (BeginLandingOpen) instead.
    if (CloseInFlight(tickMs)) {
        reveal.deferred = true;
        return;
    }
    if (g_pendingTapTick != 0) {
        CancelCloseSchedules(L"pull");
    }
    float entry = 0.0f;
    if (PublishOpenClaim(geometry, true, entry)) {
        TraceLog(L"reveal show request entry=%.3f", entry);
    } else {
        // No executor can present this open: the claim is withdrawn by the
        // helper and the shell's own band gesture remains the owner of the
        // open. The pull keeps its candidate and its release history, so an
        // open the shell performs anyway is adopted at the next cloak event.
        TraceLog(L"reveal show request failed entry=%.3f reason=no-executor",
                entry);
    }
}

// The absolute panel position the finger asks for: the panel's trailing edge
// would meet the finger there, in absolute screen DIP on both sides, so the value
// does not depend on where the finger was when the menu was first seen open. One
// mapping for the follow's samples and for the window replayed out of the
// contact's own reports, so the two series cannot disagree about where the
// finger is. A live takeover starts at the visible entry and then chases this
// value at the planner's rate, so the part of it that happened while the menu
// was cloaked is closed over time instead of being skipped or subtracted;
// travel, commit and the speed window keep reading the mapped value as it is.
float RevealProgressDip(const GestureGeometry& geometry, float distance, float x,
                        float y, float& panelTrailOut) {
    const int32_t axis = geometry.axis();
    const float finger = AxisScreenPosition(axis, x, y);
    // The trail is the panel edge this open measured. Without a measurement the
    // root-width fallback places it at the far screen edge, which is where the
    // travel maps Progress 0 to the taskbar edge and 1 to the far one.
    panelTrailOut = geometry.panel ? GeometryTrailDip(geometry)
                                   : FallbackTrailDip(axis, distance);
    if (!(distance > 0.0f)) {
        return 0.0f;
    }
    return std::clamp(
        1.0f - (finger - panelTrailOut) / (AxisDirection(axis) * distance), 0.0f,
        1.0f);
}

// Rebuilds the release-speed window from the contact's own positioned reports.
// Used at a takeover and at a release that ended before the menu opened, so both
// measure the finger from the same history through the same trail mapping. The
// window holds the *mapped* series, never the chased visible one: the end speed
// belongs to the finger, and the chase deliberately moves at the planner's rate
// instead of the finger's.
void SeedRevealEndVelocity(const GestureGeometry& geometry, float distance) {
    ResetEndVelocity();
    float ignoredTrail = 0.0f;
    for (int i = 0; i < g_contactOrigin.pointCount; ++i) {
        const HidContactOrigin::Point& point = g_contactOrigin.points[i];
        SampleEndVelocity(
            RevealProgressDip(geometry, distance, point.x, point.y,
                              ignoredTrail),
            point.qpc);
    }
}

// Takes Progress over for a contact whose menu is now open. The panel's position
// is taken, never the animation's target; the trail mapping is read from this
// snapshot and every later report drives Progress from the finger. One place, so
// the admission's first sample and the adoption's first sample cannot differ.
//
// The follow starts where the panel becomes visible and then chases the finger.
// The shell uncloaks the menu 60-250 ms after the pull starts (or after the
// lift), so a finger that is still down has travelled 400-600 DIP by then and the
// absolute mapping reads 0.76-1.00. Writing that on the first visible frame made
// the panel appear already 76-100 % open — the owner's "pause, then snap open" —
// and subtracting the difference as a constant made it trail the finger forever
// and stop short of full (measured: 0.25-0.27 while the finger was 450-560 DIP
// out). So the start is `RevealEntryProgress`, the smallest progress at which the
// panel's trail edge sits on the work area's edge, and from there `DriveRevealFollow`
// moves the visible position toward the mapped one at the planner's rate:
//
//     step    = clamp(mapped - visible, -rate * dt, +rate * dt)
//     visible = clamp(visible + step, 0, 1)
//
// A settle may start at 0 because it spends time crossing the hidden travel; a
// follow spends the finger's travel, so it starts at the entry and then closes
// the gap at 2.2 DIP/ms — the speed every keyboard open and settle already uses,
// so the catch-up is the panel opening at the system's own speed, not a new
// mechanism. A 600 DIP gap closes in ~303 ms (30 ms hold + 273 ms) and a 100 DIP
// gap in ~75 ms, and from then on the panel is glued to the finger (a finger
// moves at 0.5-5 DIP/ms).
//
// The trail mapping still measures the finger for `travel`, the commit test,
// `max=` and the end-speed window; only the value written to Progress chases. A
// panel that was already on screen (a pull that cancels a close makes the follow
// take a live retract over) starts at its live value, so the chase begins from
// where it is.
bool ActivateRevealFollow(const GestureGeometry& geometry, float x, float y,
                          uint64_t sampleQpc, uint64_t tickMs) {
    HidRevealSession& reveal = g_reveal;
    if (CloseInFlight(tickMs)) {
        // A close owns the panel: its retract is the only motion, and the pull
        // that will open the next menu must not write Progress under it. The
        // session keeps its candidate, its clocks and its release history; the
        // close's landing releases the deferral and the open adopts it.
        return false;
    }
    reveal.deferred = false;
    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        // A new root is being installed under the contact, so this contact no
        // longer has a renderer to drive.
        reveal.edgeCandidate = false;
        EndContactTiming();
        return false;
    }

    reveal.active = true;
    reveal.activeSinceQpc = sampleQpc;
    reveal.distance = distance;
    reveal.progress = 0.0f;
    reveal.samples = 0;
    // The mapped progress below (0.269 at the screen edge on this layout)
    // replaces whatever the panel was doing, so the first step of the follow
    // must not become the coast speed, and the window starts at the value the
    // panel really shows, not at the target of a settle it is taking over.
    ResetCoastVelocity();

    // A live takeover supersedes any record a previous pull left for the open
    // path: this finger owns the panel now.
    g_lateRelease = RevealLateRelease{};

    float panelTrail = 0.0f;
    const float mapped =
        RevealProgressDip(geometry, distance, x, y, panelTrail);
    reveal.panelTrail = panelTrail;
    reveal.target = mapped;

    // A cancel or dismiss animation from a previous gesture may still own
    // Progress; the follow must drive the scalar from this sample on, and the
    // takeover pins the position the panel has now so no frame between the two
    // writes shows the old animation's target. A scheduled dismiss that is still
    // ahead is not touched here: it re-checks the pull generation once, so a pull
    // that began first drops the close.
    const float entry = RevealEntryProgress(geometry);
    const float live = TakeProgressNow(L"activate");
    // A hidden panel's visible value is the entry, never the recorded open rest
    // (1) of a process that has not shown it yet; a panel already above the entry
    // keeps its live position, and one below it is off screen, so the entry is
    // the honest start there too.
    const float visible = live >= 1.0f ? entry : std::max(live, entry);
    reveal.visible = visible;
    reveal.activationGap = mapped - visible;
    // The chase clock starts after the entry has been held for the first
    // presentation, so the first visible frame is the edge and not the chase's
    // first steps. Until then `chaseQpc` lies ahead of the sample time and the
    // step bound is zero.
    reveal.chaseQpc = sampleQpc + kRevealEntryHoldMs;
    // The first frame after this call is the first one that can be composited,
    // so the visible start is written now. When nothing animated, the base was
    // already that value and this insert writes the same one; when the base was
    // the open rest of a process that has not shown the panel yet, it is what
    // keeps that frame from presenting the panel at rest.
    // The contact takes the panel over: record the intent before the first
    // frame that can be composited.
    g_panel.Begin(&visible);
    reveal.lastInserted = visible;
    reveal.progress = visible;

    // The release window starts at the contact's own reports, not at the
    // takeover: the shell opens the menu late when the taskbar is auto-hidden
    // (the pull first reveals the bar), so a takeover one or two reports before
    // the lift had no window to measure and the settle started from rest while
    // the finger was moving.
    SeedRevealEndVelocity(geometry, distance);

    // Diagnostic: where the follow starts and the gap it inherited. The chase
    // closes that gap at the planner's rate; the line says how much of it there
    // was, not a value that is subtracted from every insert.
    TraceLog(L"reveal takeover from=%.3f offset=%.3f", visible,
            reveal.activationGap);
    // The opening contact now owns the panel through the follow. The entry the
    // touch show reserved belongs to the reveal takeover, not to a later keyboard
    // settle. `controllerTouch` stays set for the whole contact: the shell's
    // click-away from this same contact can arrive after this takeover too, and
    // it is still the finger that pulled the menu out. It clears when the contact
    // lifts or a close takes ownership.
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    return true;
}

// Runs on the first report of a contact. A contact that starts inside the
// taskbar's band while the menu is closed can own the menu's own open; anything
// else is left to the dismiss path. A touch inside the band of an *open* menu is
// a dismiss start, not a candidate, unless a close of this mod is still in
// flight.
//
// Clears the local gesture deadline only. The executor command is a separate
// schedule and is not touched: a caller that must not publish (the landing) uses
// this, and the local slot's runner then finds nothing to request.
bool ClearLocalCloseTap() {
    if (g_pendingTapTick == 0) {
        return false;
    }
    const CloseKind dropped = g_pendingTapKind;
    g_pendingTapTick = 0;
    g_pendingTapKind = CloseKind::None;
    g_pendingTapPull = 0;
    TraceLog(L"close dismiss cancelled kind=%s", CloseKindName(dropped));
    return true;
}

// Drops both close schedules a close leaves behind: the local gesture deadline
// and the executor dismiss, the latter cancelled with a command that names it.
// It reports which of the two was present, because the callers act on different
// parts: the open path takes the panel back when a local slot was dropped, and an
// external takeover needs only both schedules gone. This is the named "cancel
// both"; the local-only operation above is a different thing.
CloseScheduleCancel CancelCloseSchedules(const wchar_t* reason) {
    CloseScheduleCancel dropped;
    dropped.command = CancelPendingDismiss(reason) != 0;
    dropped.slot = ClearLocalCloseTap();
    return dropped;
}

// A dismiss that is still settling does not block a candidate: the edge contact
// during its retract is the pull that can still cancel the close if it began
// before the dismiss, and refusing it would let the dismiss take the menu away
// under the finger.
bool BeginEdgeCandidate(const GestureGeometry& geometry, float x, float y,
                        uint64_t tickMs) {
    float low = 0.0f;
    float high = 0.0f;
    const bool haveBand = RevealBandDip(geometry, low, high);
    const int32_t axis = geometry.axis();
    const float position = AxisScreenPosition(axis, x, y);
    const bool inBand = haveBand && position >= low && position <= high;
    // The live position and the restore the state owes, for the `reason=open`
    // refusal: the window reporting itself open is not proof that the menu is
    // usable, and the two numbers are what tell a healthy refusal from the
    // unhealed pair (window uncloaked, panel at the hidden rest).
    const float progress = inBand ? PeekProgressNow() : -1.0f;
    const bool restoreOwed = g_commitNeedsProgressReset;

    // The line speaks once per contact, and only for a contact that landed in
    // the band: a tap anywhere else is not this gesture and stays as quiet as it
    // was. A contact that starts inside the band and is refused names the test
    // that refused it, which is what a pull that never followed the finger reads.
    const wchar_t* reason = L"admitted";
    bool admitted = false;
    // A close this mod started owns the panel until the shell has hidden the
    // window. A pull that lands in that window is the pull that opens the menu
    // this close makes room for: it is tracked, never refused and never muted,
    // and it writes no Progress. Treating it as a candidate of *this* panel is
    // what produced two owners: the follow took the retract over, the shell hid
    // the window under it, and the reopen replayed the open.
    const bool closeInFlight = CloseInFlight(tickMs);
    do {
        if (!inBand) {
            break;  // a press outside the band is not this gesture
        }
        if (!RendererReady()) {
            reason = L"renderer";
            break;
        }
        if (!GeometryLayoutCurrent(geometry)) {
            reason = L"layout";
            break;
        }
        if (!(geometry.distance() > 0.0f)) {
            reason = L"distance";  // renderer not ready: no travel to follow
            break;
        }
        if (closeInFlight) {
            StartRevealFollow(x, y, tickMs, tickMs, true);
            admitted = true;
            reason = L"deferred";
            break;
        }
        if (StartWindowIsOpenThrottled(tickMs)) {
            // An uncloaked window is not proof that the menu is usable: a close
            // whose Hide landed, or a settle that died at the hidden rest, leaves
            // the window shown with the panel translated off it. Only a panel at
            // rest is a menu this contact must not touch; a panel anywhere else is
            // the pull's to bring back, so it is admitted as the reveal instead of
            // being refused and then muted as the band tail. A close the previous
            // report cancelled leaves exactly that state, and the line says so.
            const float distance = geometry.distance();
            const float live = PeekProgressNow();
            const bool atRest =
                distance > 0.0f &&
                (1.0f - live) * distance < kProgressEpsilonDip;
            if (atRest) {
                reason = L"open";
                break;
            }
            reason = g_closeCancelledTick == tickMs ? L"cancelled" : L"unhealed";
        }
        StartRevealFollow(x, y, tickMs, tickMs);
        admitted = true;
    } while (false);

    if (inBand) {
        TraceLog(L"reveal admit x=%.0f y=%.0f axis=%d band=%.0f,%.0f ok=%d "
                L"reason=%s progress=%.3f restore=%d",
                x, y, axis, haveBand ? low : -1.0f, haveBand ? high : -1.0f,
                admitted ? 1 : 0, reason, progress, restoreOwed ? 1 : 0);
        // A contact in the band may open the menu without this mod asking: the
        // shell's own gesture starts from a strip wider than any band this mod
        // admits, and a pull that is refused here (or lifts before the claim)
        // still opens it. That open has no show request of ours, so this is the
        // one transition whose event must not be the only source: the watch
        // gives it the same slow fallback poll a requested one gets.
        NoteVisibilityTransition(kVisibilityWantOpen);
    }
    return admitted;
}

// The follow's one sample: the mapped Progress from the finger, the trail the
// panel actually shows, the coast handoff and the release-speed window. It runs
// on every report once the follow is active, including the adoption's own
// report, which is why the adoption needs no second code path.
void DriveRevealFollow(const GestureGeometry& geometry, float x, float y,
                       uint64_t sampleQpc) {
    HidRevealSession& reveal = g_reveal;
    const int32_t axis = geometry.axis();
    // Decision quantity: how far the finger moved away from the taskbar edge
    // since the touch began. Signed by the opening direction and clamped at 0,
    // so a pull back toward the edge reads 0 and cannot commit.
    reveal.travel = RevealTravelDip(axis, reveal.edgeStartX, reveal.edgeStartY,
                                    x, y);
    reveal.maxTravel = std::max(reveal.maxTravel, reveal.travel);

    float panelTrail = 0.0f;
    const float mapped =
        RevealProgressDip(geometry, reveal.distance, x, y, panelTrail);
    reveal.panelTrail = panelTrail;
    reveal.target = mapped;

    // A report ends the coast: pin where it got to, then chase from there, so the
    // panel continues from the position the coast actually reached instead of
    // snapping back to the value before it.
    float coastValue = reveal.visible;
    const bool resumed =
        g_panel.coasting.load(std::memory_order_relaxed) && StopCoast(coastValue);
    if (resumed) {
        reveal.visible = std::clamp(coastValue, 0.0f, 1.0f);
    }

    // The visible position chases the finger's absolute position and never passes
    // it. The bound is a speed, not a per-report distance: the same
    // kSettleSpeedDipPerMs (2.2 DIP/ms) every keyboard open and every settle
    // uses, so closing a gap looks like the panel opening at the system's own
    // speed. A 600 DIP gap takes ~273 ms of chasing (30 ms more with the entry
    // hold) and a 100 DIP gap ~45 ms; after that the chase keeps up with any
    // finger (0.5-5 DIP/ms) and the panel is glued. The clock is still in the entry
    // hold for the first presentation, and a sample inside that beat must not move
    // the entry or start the clock early.
    if (sampleQpc > reveal.chaseQpc) {
        const uint64_t elapsed = sampleQpc - reveal.chaseQpc;
        // A stalled thread must not turn the speed bound into a jump: 50 ms is
        // more than the 18 ms idle that ends a contact, so this only ever trims a
        // gap the gesture cannot really have bridged.
        const uint64_t stepMs =
            std::min<uint64_t>(elapsed, kRevealChaseMaxStepMs);
        const float stepLimit =
            kSettleSpeedDipPerMs * static_cast<float>(stepMs) / reveal.distance;
        const float step =
            std::clamp(mapped - reveal.visible, -stepLimit, stepLimit);
        reveal.visible = std::clamp(reveal.visible + step, 0.0f, 1.0f);
        reveal.chaseQpc = sampleQpc;
    }

    const float progress = reveal.visible;
    reveal.progress = progress;
    reveal.samples++;
    SampleCoastVelocity(progress, sampleQpc);
    if ((resumed ||
         std::fabs(progress - reveal.lastInserted) * reveal.distance >=
             kProgressEpsilonDip) &&
        g_panel.Move(progress)) {
        reveal.lastInserted = progress;
    }
    // The release speed belongs to the finger, not to the chase: the window reads
    // the mapped value, so a catch-up that is still in flight cannot be mistaken
    // for the finger's end speed.
    SampleEndVelocity(mapped, sampleQpc);
}

// One reveal step. While the menu is still closed the contact is only
// remembered; the moment it is open under the contact, the follow takes
// Progress over and every later report drives it.
void UpdateRevealSession(const GestureGeometry& geometry, float x, float y,
                         uint64_t tickMs) {
    HidRevealSession& reveal = g_reveal;
    const uint64_t sampleQpc = QpcMsNow();
    reveal.lastTick = tickMs;
    reveal.lastSampleQpc = sampleQpc;
    reveal.lastAliveQpc = sampleQpc;

    if (!reveal.active) {
        if (reveal.deferred) {
            if (CloseInFlight(tickMs)) {
                // The close owns the panel. This contact is measured only: no
                // Progress, no coast, no release settle. Its own reports keep the
                // liveness clock and its history fresh so the open that follows
                // can adopt it from its down point.
                return;
            }
            // The close landed: this pull is released and the ordinary takeover
            // below decides, exactly as it would for a pull on a closed menu.
            reveal.deferred = false;
        }
        if (!StartWindowIsOpenThrottled(tickMs)) {
            return;
        }
        // A shown window is not a takeover license by itself: a contact that
        // landed on a panel off its rest (the unhealed case) must not acquire
        // Progress. The same 24 DIP the open claim and the adoption use
        // qualifies it; a contact that already qualified before keeps its
        // claim (openClaimAttempted) even as it comes back toward the edge.
        if (!reveal.openClaimAttempted) {
            const float travel =
                RevealTravelDip(geometry.axis(), reveal.edgeStartX,
                                reveal.edgeStartY, x, y);
            if (!(travel >= kAdoptTravelDip)) {
                return;
            }
        }
        if (!ActivateRevealFollow(geometry, x, y, sampleQpc, tickMs)) {
            return;
        }
    }

    DriveRevealFollow(geometry, x, y, sampleQpc);
}

// Adopts a live contact as the reveal when the menu opens under it without a
// candidate having admitted it. The shell's own edge gesture opens the menu from
// a strip wider than any fixed band this mod can guess, and a pull that began
// inside the taskbar's thickness therefore used to reach the menu as a
// keyboard-style open from 0 while the finger was pulling. Adoption is the
// takeover that was missing: the same contact, measured from its own down point,
// takes the reveal over and the panel counts that finger's motion from the screen
// edge (the visible entry through the cloak), not the absolute mapped position it
// never displayed.
//
// Three tests keep a tap a tap: the contact must have started while the menu was
// closed (a contact on the open panel is a dismiss), it must have moved
// `kAdoptTravelDip` along the opening axis (a tap does not move), and no session
// may already own it. The adoption runs from the open transition's own service
// pass and from the report path, so a flick whose lift arrives before the cloak
// event is still adopted.
bool AdoptRevealContact(const GestureGeometry& geometry, uint64_t tickMs) {
    if (g_reveal.edgeCandidate || g_reveal.active || g_hidSession.active ||
        g_hidSession.suppress || !g_contactOrigin.valid ||
        g_contactOrigin.startedOpen || !RendererReady() ||
        !GeometryLayoutCurrent(geometry)) {
        return false;
    }

    const int32_t axis = geometry.axis();
    const float travel =
        RevealTravelDip(axis, g_contactOrigin.downX, g_contactOrigin.downY,
                        g_contactOrigin.x, g_contactOrigin.y);
    if (!(travel >= kAdoptTravelDip)) {
        return false;
    }

    float low = 0.0f;
    float high = 0.0f;
    const bool haveBand = RevealBandDip(geometry, low, high);
    // A contact that began while a close owned the panel is the pull that opens
    // the next menu, not a takeover of this one: it is tracked as a deferred
    // candidate and promoted at the open the close makes room for.
    const bool closeInFlight = CloseInFlight(tickMs);
    StartRevealFollow(g_contactOrigin.downX, g_contactOrigin.downY,
                      g_contactOrigin.downTick, tickMs, closeInFlight);
    if (closeInFlight) {
        return false;
    }
    const uint64_t sampleQpc = QpcMsNow();
    if (!ActivateRevealFollow(geometry, g_contactOrigin.x, g_contactOrigin.y,
                              sampleQpc, tickMs)) {
        return false;
    }
    DriveRevealFollow(geometry, g_contactOrigin.x, g_contactOrigin.y,
                      sampleQpc);
    TraceLog(L"reveal adopt x=%.0f y=%.0f travel=%.0f axis=%d band=%.0f,%.0f gen=%u",
            g_contactOrigin.x, g_contactOrigin.y, travel, axis,
            haveBand ? low : -1.0f, haveBand ? high : -1.0f,
            geometry.generation);
    return true;
}

// The open found a finger that is still down and already pulling. A candidate is
// promoted here, at the open: it would activate on its next report anyway, and
// waiting for that report is exactly the race that lost the seam when the finger
// lifted first. A contact with no candidate is adopted from its own down point.
// Either way the follow owns Progress from this frame, which is the only
// suppression the open decision may use.
void PromoteRevealContact(const GestureGeometry& geometry, uint64_t tickMs) {
    if (g_reveal.active) {
        return;  // a follow already owns the panel
    }

    if (g_reveal.edgeCandidate) {
        // The finger that admitted the candidate must have moved past the drag
        // threshold: a tap on the taskbar keeps the keyboard-style settle, and a
        // finger resting at the edge is not pulling yet.
        const bool moved =
            g_contactOrigin.valid &&
            RevealTravelDip(geometry.axis(), g_reveal.edgeStartX,
                            g_reveal.edgeStartY, g_contactOrigin.x,
                            g_contactOrigin.y) >= kAdoptTravelDip;
        if (!moved) {
            return;
        }
        const uint64_t sampleQpc = QpcMsNow();
        if (ActivateRevealFollow(geometry, g_contactOrigin.x,
                                 g_contactOrigin.y, sampleQpc, tickMs)) {
            DriveRevealFollow(geometry, g_contactOrigin.x, g_contactOrigin.y,
                              sampleQpc);
        }
        return;
    }

    AdoptRevealContact(geometry, tickMs);
}

// Consumes a release that ended before its menu opened. Runs on the input thread
// from the visibility service, in the same pass that just saw the closed-to-open
// transition, so the record it reads was written by this thread: the release runs
// in the report handler or the deadline service and this pass runs in the same
// message loop, so no publication exists and none is needed. A live takeover
// already cleared the record; an admitted newer pull invalidates it; an expired
// one is dropped.
//
// The settle is the ordinary one, from the position the panel actually shows at
// this instant (`TakeProgressNow()`) at the recorded speed, so a fast pull leaves
// the seam at the finger's speed and the panel still rises from where it appears
// — off the bottom edge. The record's mapped position is a diagnostic, not a
// start value: it is where the follow would have put a panel that had been on
// screen, and starting there made the panel appear already open. Nothing is
// invented when the record is absent: the caller then runs the keyboard open from
// rest exactly as before.
bool ConsumeLateRevealRelease(const GestureGeometry& geometry) {
    RevealLateRelease& record = g_lateRelease;
    if (!record.valid) {
        return false;
    }

    const uint64_t qpcNow = QpcMsNow();
    const uint64_t ageFrom = record.readyQpc != 0 ? record.readyQpc
                                                  : record.releaseQpc;
    const uint64_t age = qpcNow > ageFrom ? qpcNow - ageFrom : 0;
    if (age > kLateRevealWindowMs || record.pullSerial != g_pullSerial) {
        record = RevealLateRelease{};
        return false;
    }

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        record = RevealLateRelease{};
        return false;
    }

    // One open consumes it, whatever the settle does next.
    record.valid = false;
    // The value the panel will show when this frame is composited. The root was
    // cloaked when the state was last read, and the follow and every settle are
    // gated on an uncloaked root, so nothing wrote a visible position through
    // that cloak: the compositor holds the off-screen rest (0 after any close),
    // not the record's mapped `pos`. Starting from `pos` made the panel appear
    // already 80 % open, because the settle had been running for the
    // state-read-to-presentation gap by the time the first frame was composited.
    // The same reading covers a
    // panel that was already on screen when the state was read: then it is
    // the live position, not 0. `TakeProgressNow()` is the one reader that turns
    // an animation's recorded base into a position, and the caller already
    // refuses the record while a close is still running, so this cannot take an
    // animation over. The rest sentinel of a process that has not shown this
    // panel yet is the off-screen rest, and `TakeVisibleProgressNow` is where that
    // rule lives. This path keeps starting at 0 while the live follow starts at
    // the entry: a settle spends time crossing the hidden travel, a follow spends
    // the finger's travel, and this record has no finger left.
    const float from = TakeVisibleProgressNow(L"late");
    std::chrono::milliseconds duration{0};
    if (!BeginGestureSettle(from, 1.0f, record.velocity, duration)) {
        return false;
    }
    // The animation owns the whole travel now, so no later path needs to restore
    // the resting value for this open.
    g_commitNeedsProgressReset = false;
    // The record is a lifted contact's open: the finger that pulled is gone, so
    // nothing owns the panel for the click-away guard, and the pending entry the
    // show reserved is spent. Both are cleared here rather than in the keyboard
    // branch, which a successful adoption skips, so the show-in-flight hold
    // cannot outlive the open it waited for.
    SwipeIpcSetControllerTouch(&g_ipc, false);
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    TraceLog(L"reveal adopt late travel=%.0f vel=%.2f age=%llu gen=%u from=%.3f "
            L"pos=%.3f",
            record.travel, SpeedTowardDip(record.velocity, from, 1.0f, distance),
            static_cast<unsigned long long>(age),
            g_openGeneration.load(std::memory_order_relaxed), from,
            record.position);
    return true;
}

// Whether this contact is the tail of a pull that is opening the menu, rather
// than a drag of the open menu. The test reads the contact's own down point: a
// finger that started inside the taskbar band and has already moved away from the
// taskbar came to reveal the menu, and the open's settle (live, late or
// keyboard) owns Progress. A press in the band that moves toward the taskbar is a
// dismiss from its first sample, so the rule only ever takes away a drag that
// would have pushed the panel away from its resting position.
bool BandTailReveal(const GestureGeometry& geometry, float x, float y) {
    if (!g_contactOrigin.valid || g_reveal.edgeCandidate) {
        return false;
    }
    float low = 0.0f;
    float high = 0.0f;
    if (!RevealBandDip(geometry, low, high)) {
        return false;
    }
    const int32_t axis = geometry.axis();
    const float down = AxisScreenPosition(axis, g_contactOrigin.downX,
                                          g_contactOrigin.downY);
    if (down < low || down > high) {
        return false;
    }
    return RevealTravelDip(axis, g_contactOrigin.downX, g_contactOrigin.downY,
                           x, y) >= kBandTailTravelDip;
}

// Release of the edge contact. A candidate that never saw the menu open did not
// touch Progress; when it was a real pull, one line says the menu never opened,
// which is the difference between "the shell did not open it" and "the shell
// opened it without the follow". A reveal that ran logs exactly one line.
//
// The decision mirrors ReleaseHidSession: it judges the travel at the moment of
// release, not the mapped progress. Progress reads 0.269 at the screen edge on
// this layout, so a progress threshold would commit a pull-out that never left
// the edge and a pull out and back alike.
void ReleaseRevealSession(const GestureGeometry& geometry) {
    HidRevealSession& reveal = g_reveal;
    if (!reveal.edgeCandidate) {
        return;
    }

    reveal.edgeCandidate = false;
    // The opening contact is gone: the shell's click-away from it can no longer
    // arrive, and a real click-away is a trigger again.
    SwipeIpcSetControllerTouch(&g_ipc, false);
    if (!reveal.active) {
        // The contact is gone and the menu never appeared under it. A tap is not
        // worth a line (it never moved); a pull that travelled says the shell
        // never opened the menu, so there was nothing to follow — and it carries
        // everything the settle needs if the shell opens the menu a moment later.
        const uint64_t releaseQpc = QpcMsNow();
        const float distance = geometry.distance();
        const float travel =
            g_contactOrigin.valid
                ? RevealTravelDip(geometry.axis(), reveal.edgeStartX,
                                  reveal.edgeStartY, g_contactOrigin.x,
                                  g_contactOrigin.y)
                : 0.0f;
        if (travel >= kAdoptTravelDip && distance > 0.0f &&
            g_contactOrigin.valid) {
            // The panel is still closed, so the mapped position is what the
            // follow would have written on its first sample; the end speed comes
            // from the same history the takeover would replay, measured at this
            // release so the freshness rule applies to it.
            SeedRevealEndVelocity(geometry, distance);
            const float velocity = EndVelocityProgressPerMs(releaseQpc);
            float panelTrail = 0.0f;
            const float position = RevealProgressDip(
                geometry, distance, g_contactOrigin.x, g_contactOrigin.y,
                panelTrail);
            g_lateRelease.valid = true;
            g_lateRelease.releaseQpc = releaseQpc;
            // A pull that lifted while a close was still in flight waited for
            // that close: its record becomes consumable at the landing, not now.
            g_lateRelease.deferred = g_reveal.deferred;
            g_lateRelease.readyQpc = 0;
            g_lateRelease.travel = travel;
            g_lateRelease.position = position;
            g_lateRelease.velocity = velocity;
            g_lateRelease.pullSerial = g_pullSerial;
            TraceLog(L"reveal candidate released unopened travel=%.0f axis=%d "
                    L"vel=%.2f pos=%.2f",
                    travel, geometry.axis(),
                    SpeedTowardDip(velocity, position, 1.0f, distance), position);
        } else if (travel >= kAdoptTravelDip) {
            // A pull with no travel to map: the line still names it, but there is
            // nothing to carry across the gap.
            TraceLog(L"reveal candidate released unopened travel=%.0f axis=%d",
                    travel, geometry.axis());
        }
        EndContactTiming();
        return;
    }
    reveal.active = false;

    // The commit decision keeps the whole-gesture tests this mod was measured
    // against: the travel at release and the session average. Only the seam reads
    // the end of the gesture.
    const uint64_t elapsed = reveal.lastTick > reveal.startTick
                                 ? reveal.lastTick - reveal.startTick
                                 : 0;
    const float travel = std::max(0.0f, reveal.travel);
    const float distance = reveal.distance;
    const float progress = std::clamp(reveal.progress, 0.0f, 1.0f);
    const bool commit =
        ReleaseCommits(true, travel, elapsed, distance, geometry.axis());

    // The release line is written after the animation starts: the file write
    // costs time on the input thread, and every millisecond before the
    // animation is a frame the panel stands still at the seam.
    const uint64_t releaseQpc = QpcMsNow();
    EndContactTiming();

    // The coast carried the panel past the last report, so both outcomes start
    // from where it got to. `progress` above is the last chased value and
    // `behind` the gap to the finger's mapped position that the chase had not
    // closed yet; the settle starts from the position the coast actually reached.
    const float from = TakeProgressNow(L"reveal-release");
    const float behind =
        std::clamp(reveal.target - progress, 0.0f, 1.0f);
    // Progress per millisecond at the end of the gesture, signed along the
    // reveal axis. The settle projects it onto its own direction. The release
    // time makes the freshness test: a held tip with no point answers 0.
    const float endVelocity = EndVelocityProgressPerMs(releaseQpc);
    const float releaseSpeed = SpeedTowardDip(endVelocity, from,
                                              commit ? 1.0f : 0.0f, distance);

    if (commit) {
        // Let go past the commit point: the menu stays open and the follow
        // finishes the slide instead of snapping.
        const bool started = BeginGestureOpen(from, endVelocity);
        TraceLog(L"open follow travel=%.0f max=%.0f progress=%.2f behind=%.3f "
                L"offset=%.3f vel=%.2f commit=%d panel=%.0f n=%d",
                travel, reveal.maxTravel, progress, behind,
                reveal.activationGap, releaseSpeed, commit ? 1 : 0,
                reveal.panelTrail, reveal.samples);
        if (!started) {
            ShowAtRest();
        }
        return;
    }

    // Short reveal: the same settle, the same log line, the same close direction,
    // the same suppression window and the same armed dismiss as the committed
    // dismiss, from the same release speed. Both close paths read identically
    // from any finger position.
    //
    // Progress stays at 0 afterwards and is not restored here: the hide takes a
    // moment, and restoring now would flash the fully open menu while the close
    // is in flight. The flag makes the next down restore the resting value once
    // the shell has finished hiding the reused root.
    const GestureClose close =
        BeginGestureClose(from, endVelocity, CloseKind::RevealCancel);
    TraceLog(L"open follow travel=%.0f max=%.0f progress=%.2f behind=%.3f "
            L"offset=%.3f vel=%.2f commit=%d panel=%.0f n=%d",
            travel, reveal.maxTravel, progress, behind, reveal.activationGap,
            releaseSpeed, commit ? 1 : 0, reveal.panelTrail, reveal.samples);
    if (close.started) {
        g_commitNeedsProgressReset = true;
        return;
    }

    // No settle ran, so no close is owed and no dismiss may be armed under a
    // panel that is still on screen. The committed dismiss puts its panel back
    // in the same case; this one leaves the menu open, fully in.
    AbandonClose(CloseKind::RevealCancel);
}

void LogHidRelease(const HidGestureSession& session, float velocity,
                   bool commit, uint64_t releaseQpc, const wchar_t* source) {
    const uint64_t releaseDelay = releaseQpc > session.lastSampleQpc
                                      ? releaseQpc - session.lastSampleQpc
                                      : 0;
    TraceLog(
        L"hid release src=%s delta=%.1f final=%.1f perp=%.1f vel=%.2f "
        L"commit=%d n=%d relms=%llu",
        source, session.maxDelta, session.curDelta, session.maxPerp, velocity,
        commit ? 1 : 0, session.samples,
        static_cast<unsigned long long>(releaseDelay));
}

void BeginHidSession(const GestureGeometry& geometry, float x, float y,
                     uint64_t tickMs) {
    // No gesture starts while a layout apply owns the renderer: the claim sees
    // the reservation, or the apply sees this claim and defers whole. The next
    // report retries, because no session was started.
    if (!ClaimPanelForAdmission()) {
        return;
    }

    // A close this mod started is still in flight: the panel is leaving, the
    // shell's Hide is ahead, and a session here would drag Progress back to rest
    // and flash the menu while it is being hidden.
    if (g_dismissSettling.load(std::memory_order_relaxed) ||
        tickMs < g_suppressRestoreUntil.load(std::memory_order_relaxed)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped settling");
        }
        return;
    }

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped distance=0");
        }
        return;
    }

    // The renderer must exist and the menu must be on screen right now. A
    // closed menu keeps its HWND and reports itself visible, so only the DWM
    // cloaked attribute answers this.
    const bool rendererReady = RendererReady();
    if (!rendererReady || !GeometryLayoutCurrent(geometry) ||
        !StartWindowIsOpenThrottled(tickMs)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped closed root=%d",
                    rendererReady ? 1 : 0);
        }
        return;
    }

    const int32_t axis = geometry.axis();

    // A press outside the panel belongs to the desktop, not to the menu. The
    // Start window is full-screen, so PointInStartWindow alone accepted a click
    // on the desktop next to the panel; on release that session animated
    // Progress back to 1 and flashed the menu while the shell was hiding it.
    // The rectangle and the axis come from the same snapshot, so the test cannot
    // mix a rect from one measurement with a trail from another.
    if (!PanelContainsPress(geometry, x, y)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped outside panel axis=%d x=%.0f y=%.0f "
                    L"rect=%.0f,%.0f,%.0f,%.0f trail=%.0f gen=%u",
                    axis, x, y, geometry.left, geometry.top, geometry.right,
                    geometry.bottom, GeometryTrailDip(geometry),
                    geometry.generation);
        }
        return;
    }

    // A vertical drag on the app list belongs to the list while it can still
    // scroll that way. The rejection starts no session, and this function runs
    // again on the next sample of the same contact, so a drag that scrolls the
    // list to its end still turns into a dismiss without a new contact. The
    // contact latch itself is left untouched: it only tracks that a finger is
    // down, and the lift path clears it, so the next down is unaffected.
    if (!VerticalDismissAllowed(geometry, axis, x, y, tickMs)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped vertical axis=%d x=%.0f y=%.0f", axis,
                    x, y);
        }
        return;
    }

    HidGestureSession& session = g_hidSession;
    session = HidGestureSession{};
    session.active = true;
    session.startX = x;
    session.startY = y;
    session.startTick = tickMs;
    session.lastTick = tickMs;
    session.lastSampleQpc = QpcMsNow();
    session.lastAliveQpc = session.lastSampleQpc;
    session.samples = 1;
    BeginContactTiming();
    // The speed of the previous gesture must not survive into this one.
    ResetCoastVelocity();
    ResetEndVelocity();

    // A cancel or open settle from the previous gesture may still be running;
    // take the position the panel has now and stop it. Taking is what keeps this
    // drag from starting at the animation's target: the drag continues from the
    // pinned value, so a takeover mid-settle has no jump. After a commit the base
    // value can still be 0, because the settle callback may not have run yet, so
    // restore the resting value before the first sample drives the scalar. The
    // restore is skipped while a close this mod started is still in flight:
    // inserting 1 there flashed the fully open panel until the shell hid the
    // window. The flag stays set so the next open restores the scalar.
    session.startProgress = TakeProgressNow(L"hid-begin");
    if (g_commitNeedsProgressReset &&
        tickMs >= g_suppressRestoreUntil.load(std::memory_order_relaxed)) {
        g_commitNeedsProgressReset = false;
        session.startProgress = kProgressRest;
    }
    // The contact begins at the position the panel has now. Begin records the
    // drag phase and pins that value; the first sample then moves it.
    g_panel.Begin(&session.startProgress);
    session.lastProgress = session.startProgress;

    TraceLog(L"hid down x=%.1f y=%.1f axis=%d dist=%.0f", x, y, axis,
            distance);
}

void ReleaseHidSession(const GestureGeometry& geometry, bool allowCommit,
                       const wchar_t* source) {
    HidGestureSession& session = g_hidSession;
    if (!session.active) {
        return;
    }

    // The commit decision keeps the whole-gesture tests this mod was measured
    // against: the travel at release and the session average. Only the seam reads
    // the end of the gesture.
    const uint64_t lastSample = session.lastTick;
    const uint64_t elapsed =
        lastSample > session.startTick ? lastSample - session.startTick : 0;
    // Judge on where the finger actually let go, not on how far it once went:
    // dragging out and back in must cancel, not dismiss.
    const float finalDelta = std::max(0.0f, session.curDelta);
    const float distance = geometry.distance();
    const bool commit =
        ReleaseCommits(allowCommit, finalDelta, elapsed, distance,
                       geometry.axis());

    const uint64_t releaseQpc = QpcMsNow();
    EndContactTiming();

    // The coast continued the drag after the last report; the close has to start
    // from where it got to, not from the last report, or the panel jumps back
    // before it settles. Both gesture families read it through the same call.
    const float from = TakeProgressNow(L"hid-release");
    // Progress per millisecond at the end of the gesture, signed along the
    // dismiss axis: a finger still moving toward the taskbar is negative. The
    // release time makes the freshness test: a held tip with no point answers 0.
    const float endVelocity = EndVelocityProgressPerMs(releaseQpc);
    const float releaseSpeed = SpeedTowardDip(
        endVelocity, from, commit ? 0.0f : 1.0f, distance);

    if (commit) {
        const DismissCloseResult close = BeginDismissSettle(from, endVelocity);
        // The release line is written after the settle started: the file write
        // costs time on the input thread, and every millisecond before the
        // animation is a frame the panel stands still at the seam.
        LogHidRelease(session, releaseSpeed, commit, releaseQpc, source);
        if (close.start != DismissClose::NotStarted) {
            g_commitNeedsProgressReset = true;
        } else {
            // The settle never started, so there is no close to wait for and no
            // dismiss was armed by it. The helper puts the panel back and says so.
            AbandonClose(CloseKind::Dismiss);
        }
        session.active = false;
        return;
    }

    session.active = false;
    // A close started by the shell (a click outside, a Windows-key press) hides
    // the window while this contact is still up. Animating Progress back to rest
    // then replays the panel over a window that is being hidden, which is the
    // flash the user reported. Leave Progress to the close.
    if (g_dismissSettling.load(std::memory_order_relaxed) ||
        GetTickCount64() <
            g_suppressRestoreUntil.load(std::memory_order_relaxed)) {
        TraceLog(L"cancel animation skipped (close in flight)");
        return;
    }
    if (distance > 0.0f) {
        BeginGestureOpen(from, endVelocity);
    } else {
        ShowAtRest();
    }
    LogHidRelease(session, releaseSpeed, commit, releaseQpc, source);
}

// Scroll abort: the menu returns to its resting position at once; no
// animation and no dismissal.
void AbortHidSession(const GestureGeometry& geometry) {
    HidGestureSession& session = g_hidSession;
    if (!session.active) {
        return;
    }

    // The same end-of-gesture speed the release lines report, without a target
    // to project onto: an abort has no seam, so the magnitude is enough.
    const float distance = std::max(geometry.distance(), 0.0f);
    const uint64_t releaseQpc = QpcMsNow();
    const float velocity =
        std::fabs(EndVelocityProgressPerMs(releaseQpc)) * distance;

    LogHidRelease(session, velocity, false, releaseQpc, L"abort");
    session.active = false;
    EndContactTiming();
    ShowAtRest();
}

void UpdateHidSession(const GestureGeometry& geometry, float x, float y,
                      uint64_t tickMs) {
    HidGestureSession& session = g_hidSession;

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        AbortHidSession(geometry);
        return;
    }

    // A report ends the coast: pin where it got to, then insert the real value
    // below even when it is inside the 0.5 DIP filter, so the panel returns to
    // the finger at once. The correction is only the few DIP it drifted.
    float coastValue = session.lastProgress;
    const bool resumed =
        g_panel.coasting.load(std::memory_order_relaxed) && StopCoast(coastValue);

    const int32_t axis = geometry.axis();
    const float delta = AxisDelta(axis, session, x, y);
    const float perpendicular = PerpDelta(axis, session, x, y);

    session.lastTick = tickMs;
    const uint64_t sampleQpc = QpcMsNow();
    session.lastSampleQpc = sampleQpc;
    session.lastAliveQpc = sampleQpc;
    session.curDelta = delta;
    session.maxDelta = std::max(session.maxDelta, delta);
    session.maxPerp = std::max(session.maxPerp, perpendicular);
    session.samples++;

    // A mostly perpendicular movement with little axis travel is a scroll of
    // the content, not a dismiss.
    if (session.maxPerp > kScrollAbortPerp &&
        session.maxPerp > kScrollAbortRatio * session.maxDelta &&
        session.maxDelta < kScrollAbortMaxDelta) {
        TraceLog(L"dismiss aborted perpendicular perp=%.0f delta=%.0f",
                session.maxPerp, session.maxDelta);
        AbortHidSession(geometry);
        return;
    }

    // A drag that took over a running settle continues from the position the
    // panel was at, so the mapping is measured from the session's own base.
    const float progress = std::clamp(
        session.startProgress - delta / distance, 0.0f, 1.0f);
    SampleCoastVelocity(progress, sampleQpc);
    // The 0.5 DIP filter is the whole gate: a movement toward the open side of a
    // panel already at rest lands on Progress 1 and changes nothing, and a drag
    // that started mid-settle must follow in either direction.
    if (resumed ||
        std::fabs(progress - session.lastProgress) * distance >=
            kProgressEpsilonDip) {
        g_panel.Move(progress);
        session.lastProgress = progress;
    }
    // The release speed reads the value the panel actually shows.
    SampleEndVelocity(session.lastProgress, sampleQpc);
}

// One state-machine step per report. It never touches a XAML object;
// composition goes through the mutex-guarded helpers. The geometry snapshot comes
// in as one value for the whole report, so the hit test, the follow and the
// release of this sample cannot read different measurements.
void OnHidSampleStep(const GestureGeometry& geometry, int contacts, float xDip,
                     float yDip, uint64_t tickMs) {
    HidGestureSession& session = g_hidSession;

    // Extra contacts (palm, ghost slot) are filtered when the report is
    // decoded; by the time a sample reaches here it always means "the tracked
    // finger is present".
    if (contacts <= 0) {
        if (session.active) {
            ReleaseHidSession(geometry, true, L"report");
        }
        session.suppress = false;
        // The origin stays for this release: a candidate that never saw the menu
        // open reads its last point to say how far the pull got before it ends.
        ReleaseRevealSession(geometry);
        g_contactOrigin = HidContactOrigin{};
        g_contactDown = false;
        g_openCheckValid = false;
        return;
    }

    // The contact's own down point, kept for a follow that is adopted later: the
    // menu can open under a pull that began outside every band this mod can
    // admit, and adoption measures its travel from here. The record starts once
    // per contact and is cleared by the lift above and by the idle that ends a
    // contact, so a retried admission cannot move the down point.
    if (!g_contactOrigin.valid) {
        g_contactOrigin.valid = true;
        // A close in flight is not an open panel. A finger that lands during the
        // retract is the user taking the menu back, so adoption must read it as
        // a pull from its own down point once it has travelled
        // kAdoptTravelDip, exactly as it does with the menu closed. Reading the
        // shown-but-leaving window as open refused that pull: it stayed inert,
        // the armed dismiss hid the menu under it, and the reopen replayed the open.
        const bool closeInFlight =
            tickMs < g_suppressRestoreUntil.load(std::memory_order_relaxed);
        const bool windowOpen =
            !closeInFlight && StartWindowIsOpenThrottled(tickMs);
        // The same reading covers the other uncloaked-but-empty window: a close
        // whose Hide landed and left the panel at the hidden rest. That is not an
        // open menu either, so a pull that lands there is adoptable and brings the
        // panel back instead of being refused as a press on an open panel.
        const bool unhealed =
            windowOpen && geometry.distance() > 0.0f &&
            PeekProgressNow() < RevealEntryProgress(geometry);
        g_contactOrigin.startedOpen = windowOpen && !unhealed;
        g_contactOrigin.downX = xDip;
        g_contactOrigin.downY = yDip;
        g_contactOrigin.downTick = tickMs;
        g_contactOrigin.pointCount = 0;
        // A new down does not clear the late-release record: the record names the
        // pull that opened the menu, and the new contact may be the tail of that
        // same gesture or a contact refused on its next report. The record is
        // bound to its pull generation instead, and a new pull that is admitted
        // changes that generation (StartRevealFollow clears it then).
    }
    g_contactOrigin.x = xDip;
    g_contactOrigin.y = yDip;
    SampleOriginPoint(xDip, yDip, QpcMsNow());

    // The edge test belongs to the first report of the contact: a finger that
    // slides into the band after starting elsewhere is not a reveal candidate,
    // and a candidate that already exists owns the contact until it lifts. A
    // report gap is not a new down by itself: a sparse digitizer (or an injected
    // 20-30 ms step) would otherwise re-admit at the crossing point and lose the
    // admission origin. The latch is cleared by the lift or by the candidate's
    // own watchdog, which is the missing-lift case.
    const bool newContact = !g_contactDown;

    // A contact that lands while a close is in flight does not cancel it here.
    // The close is aborted only when this contact becomes a qualified pull
    // (CancelCloseForQualifiedPull, called from TryClaimRevealOpen), so a tap
    // that never enters the band or never travels leaves the close alone. A
    // close that cannot be aborted is deferred and the landing releases it.

    if (session.suppress) {
        // A close muted this contact: its reports are not a gesture, but the
        // contact is still on the glass, so the latch must not expire under it.
        // The lift report clears both.
        g_contactDown = true;
        g_contactTick = tickMs;
        return;
    }

    if (session.active) {
        UpdateHidSession(geometry, xDip, yDip, tickMs);
        return;
    }

    if (newContact && !ClaimPanelForAdmission()) {
        // A layout apply owns the renderer: admit nothing, and leave the latch
        // untouched so the next report of this contact is still new and retries
        // the edge test.
        return;
    }
    if (newContact && !GeometryLayoutCurrent(geometry)) {
        // The layout was applied while this report was being decoded: the
        // snapshot it carries is the old panel, so no admission starts from it.
        // The next report re-reads the current snapshot.
        return;
    }
    g_contactDown = true;
    g_contactTick = tickMs;
    if (newContact) {
        BeginEdgeCandidate(geometry, xDip, yDip, tickMs);
    }
    if (g_reveal.edgeCandidate) {
        // Admission only recorded the contact. The open request belongs to the
        // first positioned report that reaches the travel threshold, and it must
        // run before UpdateRevealSession: ActivateRevealFollow expects a window
        // that is already open and would never request one.
        TryClaimRevealOpen(geometry, xDip, yDip, tickMs);
        UpdateRevealSession(geometry, xDip, yDip, tickMs);
        return;
    }

    // The menu opened under this contact while it was already pulling, and the
    // shell's own edge gesture is what opened it: the same pull adopts it and
    // the panel follows from where it is. A tap cannot reach this line: it has
    // no travel, and the record says the menu was open at its down if it began
    // on the panel.
    if (StartWindowIsOpenThrottled(tickMs) && AdoptRevealContact(geometry, tickMs)) {
        UpdateRevealSession(geometry, xDip, yDip, tickMs);
        return;
    }

    // A contact that started in the reveal band and is moving away from the
    // taskbar is the tail of the pull that opened this menu: the open's settle
    // owns Progress, and this contact must neither take it over nor settle
    // anything at its release. Reversing toward the taskbar makes it a dismiss on
    // that same sample, so a real drag of the open panel is never lost.
    if (BandTailReveal(geometry, xDip, yDip)) {
        // One line per window, like the other skipped-dismiss lines: the
        // digitizer keeps reporting while the finger moves, and the point of the
        // line is to name the contact the open took away.
        if (tickMs >= g_bandTailLogTick &&
            tickMs - g_bandTailLogTick >= kDismissRejectLogMs) {
            g_bandTailLogTick = tickMs;
            TraceLog(L"dismiss skipped band tail travel=%.0f",
                    RevealTravelDip(geometry.axis(), g_contactOrigin.downX,
                                    g_contactOrigin.downY, xDip, yDip));
        }
        return;
    }

    if (!PointInStartWindow(xDip, yDip)) {
        return;
    }
    BeginHidSession(geometry, xDip, yDip, tickMs);
}

// Publishes whether a live contact owns the panel edge. The UI side re-reads it
// when a measurement runs, so a request recorded on an idle panel cannot move the
// edge under a finger that started in between.
//
// The same call is where a SizeChanged-deferred measurement is released: the
// contact that owned the panel just ended, so the edge can move again. One post
// per deferred resize, never a poll.
void PublishPanelOwnership() {
    const bool owned = g_hidSession.active || g_reveal.edgeCandidate;
    // seq_cst: the admission claim and the UI's layout reservation read this
    // flag, and their handshake is what keeps a layout change out from under a
    // gesture that has just started.
    g_panelOwned.store(owned, std::memory_order_seq_cst);
    if (!owned && g_measureDeferred.exchange(false, std::memory_order_relaxed) &&
        !g_unloading.load(std::memory_order_relaxed)) {
        // The change that was held back is applied once now: renderer scalars,
        // panel rect and scroller cache together.
        const SettledMeasureRequest request = StartMeasureRequest(true);
        PostGeometryMeasurement(request, true);
    }
}

// Every report enters here. The snapshot is taken once, for the whole sample.
void OnHidSample(int contacts, float xDip, float yDip, uint64_t tickMs) {
    GestureGeometry geometry = ReadGestureGeometry();
    // A new contact may not start from a cached copy that predates a resize.
    // The report path uses the try-read; the first report of a contact can
    // afford the writer lock, and an obsolete identity means no gesture until
    // the writer publishes the new layout. The latch is the contact's identity:
    // a report gap alone is not a new down (see OnHidSampleStep).
    const bool newContact = !g_contactDown;
    if (newContact && !GeometryLayoutCurrent(geometry)) {
        geometry = ReadGestureGeometry(true);
    }
    OnHidSampleStep(geometry, contacts, xDip, yDip, tickMs);
    PublishPanelOwnership();
}

// The coast through the window between the last report and the release. Only the
// idle tick runs while the stream is silent, so it starts the coast, and it is
// also the tick that ends the session 10-15 ms later: that release stops the
// coast and plans the close from its value.
void MaybeCoast(const GestureGeometry& geometry, uint64_t tickMs,
                uint64_t qpcNow) {
    if (g_panel.coasting.load(std::memory_order_relaxed)) {
        // Nothing interrupted this coast: pin the value it was heading to. The
        // animation is over at this point, and its base value is the report
        // before it, which would snap the panel back.
        if (qpcNow >= g_panel.coastStartQpc &&
            qpcNow - g_panel.coastStartQpc >=
                static_cast<uint64_t>(kCoastDuration.count())) {
            if (g_debugCoast) {
                TraceLog(L"coast end value=%.3f", g_panel.coastTo);
            }
            g_panel.Move(g_panel.coastTo);  // ends the coast, see InsertProgress
        }
        return;
    }

    // A close this mod started still owns Progress, and a menu that is already
    // closing has no finger motion left to continue.
    if (tickMs < g_suppressRestoreUntil.load(std::memory_order_relaxed)) {
        return;
    }

    const bool revealActive = g_reveal.active;
    if (!g_hidSession.active && !revealActive) {
        return;
    }

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        return;
    }

    // The reveal maps Progress from the panel edge in the snapshot, which the UI
    // thread publishes per open. The first 50 ms of the follow settle.
    if (revealActive &&
        (qpcNow < g_reveal.activeSinceQpc ||
         qpcNow - g_reveal.activeSinceQpc < kRevealCoastSettleMs)) {
        return;
    }

    const uint64_t lastSample =
        revealActive ? g_reveal.lastSampleQpc : g_hidSession.lastSampleQpc;
    if (qpcNow < lastSample || qpcNow - lastSample < kCoastStartMs) {
        return;
    }

    // The HWND outlives the menu and IsWindowVisible stays TRUE while it is
    // closed, so only the cloaked attribute says the menu is on screen. One
    // query per coast is affordable where one per report would not be.
    if (!StartWindowIsOpen()) {
        return;
    }

    const float velocity = CoastVelocity(distance);
    if (std::fabs(velocity) * distance < kCoastMinVelocityDip) {
        return;
    }

    const float from = std::clamp(
        revealActive ? g_reveal.lastInserted : g_hidSession.lastProgress,
        0.0f, 1.0f);
    const float to = std::clamp(
        from + velocity * static_cast<float>(kCoastDuration.count()), 0.0f,
        1.0f);
    if (to == from) {
        return;  // the travel is used up; there is nothing to continue
    }

    if (StartCoast(from, to) && g_debugCoast) {
        TraceLog(L"coast start from=%.3f to=%.3f vel=%.2f", from, to,
                velocity * distance);
    }
}

// Whether the idle release of both gesture families is held. The two holds end
// on an event, never on a clock, and that is why the wait planner leaves the
// release deadline unarmed while either is up: a deadline that cannot act would
// spin the thread at the timer's smallest delay.
//
// - A touch show is in flight: the shell is uncloaking the window and the
//   digitizer may go quiet for that whole window. The uncloak (or the show's
//   refusal) is the event that ends it.
// - A close is in flight and a contact owns the panel: the key press and the
//   shell's hide pause the digitizer too, and the landing is the event that
//   releases the deferral.
bool IdleReleaseHeld(uint64_t tickMs) {
    const bool touchShowInFlight =
        SwipeIpcControllerTouch(&g_ipc) ||
        g_intentOpenActive.load(std::memory_order_relaxed);
    const bool holdThroughClose =
        CloseInFlight(tickMs) && (g_reveal.edgeCandidate || g_reveal.active);
    return touchShowInFlight || holdThroughClose;
}

// A device that leaves the surface without a lift report still ends the
// gesture; the deadline service only has to notice the silence, which is
// measured on the QPC clock because the tick clock is quantised to 15.6 ms.
void CheckHidIdleRelease(uint64_t tickMs) {
    // This is also this thread's heartbeat: it runs on every wake, so a live
    // controller never looks stale between gestures. The throttle makes this
    // one interlocked write per 100 ms at most, and the write itself is skipped
    // while no peer is advertised.
    PublishPeerTick(tickMs);

    HidGestureSession& session = g_hidSession;
    const uint64_t qpcNow = QpcMsNow();
    // One snapshot for the whole tick: the coast, the release and the reveal all
    // decide against the same measurement.
    const GestureGeometry geometry = ReadGestureGeometry();

    // A release record that no open consumed inside its window is dead: drop it
    // here so it cannot be read long after the gesture, and so the next keyboard
    // open finds none. The deadline is the record's readiness, not its release:
    // a pull that lifted while a close owned the panel is not consumable until
    // that close lands, so starting the window at the release would kill it
    // before the landing it waits for. While it is still deferred, the landing
    // (or the qualified pull that cancels the close) owns its fate.
    const uint64_t recordQpc =
        g_lateRelease.readyQpc != 0 ? g_lateRelease.readyQpc
                                    : g_lateRelease.releaseQpc;
    if (g_lateRelease.valid && !g_lateRelease.deferred &&
        (qpcNow < recordQpc ||
         qpcNow - recordQpc > kLateRevealWindowMs)) {
        g_lateRelease = RevealLateRelease{};
    }

    // First, before any release is decided. The coast has to be running when the
    // release is planned, and the release is the very next thing this tick does.
    // On a clean 5 ms tick the coast starts 8-13 ms after the last report and the
    // release lands 10-16 ms later; a coalesced tick, which a busy input thread
    // produces, could otherwise cross both thresholds at once and leave the
    // panel standing still for the whole gap. Starting it here makes that tick
    // start the coast and then hand it to the release, which stops it and settles
    // from its value, exactly as on the clean path.
    MaybeCoast(geometry, tickMs, qpcNow);

    // The settled measurement's deadline: the layout watch needs a second layout
    // pass, and one may never come after the open storyboard finishes. The
    // request names itself, so the turn this posts and the layout watch's pass
    // cannot both measure: whichever runs first clears the request.
    SettledMeasureRequest due{};
    if (TakeMeasureDeadline(tickMs, due)) {
        PostGeometryMeasurement(due, true);
    }

    // A settled scroller pass that could not commit (an unreadable tree, the
    // depth limit, or a live contact on the panel) left the gate Unknown for
    // this open. Re-post that pass once the delay has passed, but only while the
    // same open is still on screen: a close or a new open drops the token.
    const uint64_t scrollerRetryAt =
        g_scrollerRetryAt.load(std::memory_order_relaxed);
    if (scrollerRetryAt != 0 && tickMs >= scrollerRetryAt &&
        g_scrollerRetryAt.exchange(0, std::memory_order_relaxed) ==
            scrollerRetryAt) {
        const uint32_t retryGeneration =
            g_scrollerRetryGeneration.load(std::memory_order_relaxed);
        if (retryGeneration != 0 &&
            retryGeneration ==
                g_openGeneration.load(std::memory_order_relaxed) &&
            StartWindowIsOpen()) {
            PostGeometryMeasurement(
                StartMeasureRequest(!g_hidSession.active &&
                                    !g_reveal.edgeCandidate),
                true);
        }
    }

    // A touch show in flight holds the release: the shell is uncloaking the
    // window and the digitizer may go quiet for that whole window (the show
    // starts at the qualified pull, before the shell's own gesture would have).
    // The contact is still down as far as this mod knows — no lift report
    // arrived — so the idle must not release the candidate the open is about to
    // adopt. A close in flight is a gap in the digitizer stream too (the key
    // press and the shell's hide both pause it); a contact that was already
    // owning the panel is held through that gap, and its real lift is a report
    // that releases it explicitly. Both holds are IdleReleaseHeld's, and its
    // comment says why the wait planner reads the same predicate.
    const bool held = IdleReleaseHeld(tickMs);
    const bool sessionIdle =
        session.active && qpcNow >= session.lastAliveQpc &&
        qpcNow - session.lastAliveQpc >= kIdleReleaseMs;
    if (sessionIdle && !held) {
        ReleaseHidSession(geometry, true, L"idle");
    }

    const bool revealIdle =
        g_reveal.edgeCandidate && qpcNow >= g_reveal.lastAliveQpc &&
        qpcNow - g_reveal.lastAliveQpc >= kCandidateIdleHoldMs;
    if (revealIdle && !held) {
        ReleaseRevealSession(geometry);
        // This watchdog is the missing lift (a slow digitizer is the other
        // reading). The next report is a new down, not a continuation whose
        // original admission point this latch still holds.
        g_contactDown = false;
        g_contactOrigin = HidContactOrigin{};
        g_openCheckValid = false;
        ForgetTrackedContact();
    }

    // The tracked identity carries its own evidence clock, independent of the
    // contact latch. A report the decoder tracked can be refused admission by
    // the layout handshake, which leaves the latch clear so the next report can
    // retry; a contact that then vanishes without a lift would hold the identity
    // forever, and every later finger would be rejected as another contact. The
    // clock moves only on evidence — an asserted contact or a point this decoder
    // feeds — never on a report that merely names the contact, so the same idle
    // window that ends a contact forgets an identity no report has proven since,
    // admitted or not. An identity that expires with the latch clear is the case
    // this clock exists for, and it is the one worth a line: no lift and no latch
    // cleanup could ever have released it.
    // No session may own the contact here: a live session's reports carry the
    // points (or the asserted tip) that refresh this clock, and its own idle
    // release runs first in this tick.
    if (!session.active && !g_reveal.edgeCandidate &&
        g_trackedContact.slot >= 0 &&
        tickMs > g_trackedContact.lastEvidenceTick &&
        tickMs - g_trackedContact.lastEvidenceTick >= kIdleReleaseMs) {
        if (!g_contactDown) {
            TraceLog(L"hid identity expired slot=%d latch=0",
                    g_trackedContact.slot);
        }
        ForgetTrackedContact();
    }

    // A contact that holds no session is not tracked per report, so the same
    // silence that ends a gesture ends the "this contact already started"
    // latch. Without it a lost lift would swallow the next edge swipe. The
    // mute a keyboard close installed belongs to that contact too: with the
    // contact gone there is nothing left to suppress, and the next contact must
    // start normally instead of being ignored until some later lift.
    if (!session.active && !g_reveal.edgeCandidate && g_contactDown &&
        tickMs > g_contactTick && tickMs - g_contactTick >= kIdleReleaseMs) {
        g_contactDown = false;
        g_contactOrigin = HidContactOrigin{};
        session.suppress = false;
        g_openCheckValid = false;
        ForgetTrackedContact();
    }

    PublishPanelOwnership();
}

// The dismiss both close paths arm. It runs on the input thread at the end of
// the settle. A pull that started after the release owns the menu again, so the
// dismiss is dropped and no Dismiss request reaches the bridge; the pull
// generation sampled at the arm decides the drop.
void RunPendingCloseTap(uint64_t tickMs) {
    if (g_pendingTapTick == 0 || tickMs < g_pendingTapTick) {
        return;
    }

    const CloseKind kind = g_pendingTapKind;
    const uint32_t pull = g_pendingTapPull;

    if (g_pullSerial != pull || g_reveal.active ||
        (g_reveal.edgeCandidate && !g_reveal.deferred)) {
        CancelCloseSchedules(L"superseded");
        return;
    }

    g_pendingTapTick = 0;
    g_pendingTapKind = CloseKind::None;
    g_pendingTapPull = 0;

    // A Win press during the retract cancels this dismiss (the open intent drops
    // the slot), so a window already closed here was hidden by something else: an
    // external takeover or the shell's own close. The dismiss would only ask the
    // shell to close a window that is already closed.
    if (!StartWindowIsOpen()) {
        TraceLog(L"close dismiss skipped kind=%s (already closed)",
                CloseKindName(kind));
        return;
    }

    // The close's settle has reached 0; the bridge is the only executor of the
    // shell's Dismiss. The controller owns the panel until the Hide lands, so
    // the bridge swallows the shell's own calls meanwhile.
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionClosing);
    // The menu this close belongs to. The bridge refuses the record when the
    // section has already moved to a later open, so an old gesture dismiss cannot
    // hide a newly opened menu. That identity is the section's open *generation*,
    // which is what every other producer stamps into the record; the tick this
    // close was published at is a different quantity and must not be substituted
    // here.
    uint64_t openGeneration = 0;
    uint64_t openTick = 0;
    SwipeIpcReadOpen(&g_ipc, &openGeneration, &openTick);
    if (!RequestBridgeDismiss(1, GetTickCount64(), openGeneration)) {
        // Nothing can run this dismiss: the window would stay shown over a
        // retracted panel. Give the ownership up now, and the unhealed rule
        // restores the panel once the suppression window ends.
        ReleaseExecutorOwnership();
        TraceLog(L"close dismiss skipped kind=%s reason=no-executor",
                CloseKindName(kind));
        return;
    }
    TraceLog(L"close dismiss kind=%s", CloseKindName(kind));
}

// ---------------------------------------------------------------------------
// The measurement request, both sides, and the one open epoch
// ---------------------------------------------------------------------------

// The only writer is the input thread's open/close observation and the UI
// thread's renderer teardown; all of them are serialized by this lock, which is
// what makes a commit's epoch test and its write one step. The count skips 0
// because 0 means "no expectation".
uint32_t AdvanceOpenGeneration() {
    std::lock_guard lock(g_measureMutex);
    uint32_t next = g_openGeneration.load(std::memory_order_relaxed) + 1;
    if (next == 0) {
        next = 1;
    }
    g_openGeneration.store(next, std::memory_order_relaxed);
    return next;
}

// Input thread only. Records the one live request. A newer request replaces the
// older one, so a turn already posted for the old one finds itself superseded.
// The serial is 64-bit and never 0: after a wrap it skips the reserved zero, so
// a delayed cleanup cannot match a reused id.
SettledMeasureRequest StartMeasureRequest(bool trailIdle) {
    std::lock_guard lock(g_measureMutex);
    if (++g_measureSerial == 0) {
        ++g_measureSerial;
    }
    g_measureRequest.id = g_measureSerial;
    g_measureRequest.generation =
        g_openGeneration.load(std::memory_order_relaxed);
    g_measureRequest.trailIdle = trailIdle;
    g_measureRequest.dueTick = GetTickCount64() + kSettledMeasureDeadlineMs;
    return g_measureRequest;
}

// Input thread only: the deadline, taken once. A request that the layout watch
// already completed has no live id, so this answers false.
bool TakeMeasureDeadline(uint64_t tickMs, SettledMeasureRequest& request) {
    std::lock_guard lock(g_measureMutex);
    if (g_measureRequest.id == 0 || g_measureRequest.dueTick == 0 ||
        tickMs < g_measureRequest.dueTick) {
        return false;
    }
    g_measureRequest.dueTick = 0;  // one shot per request
    request = g_measureRequest;
    return true;
}

// Input thread only: the same deadline, read without taking it, so the wait
// planner can arm the timer for it. The lock is the request's own; no other
// lock is held here and none is taken inside it.
uint64_t PeekMeasureDeadlineTick() {
    std::lock_guard lock(g_measureMutex);
    return g_measureRequest.id != 0 ? g_measureRequest.dueTick : 0;
}

// Both parts are compared: the id is unique while the process lives and the
// generation names the open it was recorded for, so a turn that outlived its
// close matches neither. The cached answers are the whole test, not an opinion
// about the open: the caller holds the commit lock, so the epoch cannot move
// between this answer and the write that follows it.
bool MeasureRequestLiveLocked(uint32_t generation, uint64_t id) {
    return id != 0 && g_measureRequest.id == id &&
           g_measureRequest.generation == generation;
}

// Whether this identity is still the live request, for callers outside a commit
// section.
bool MeasureRequestLive(uint32_t generation, uint64_t id) {
    if (id == 0) {
        return false;
    }
    std::lock_guard lock(g_measureMutex);
    return MeasureRequestLiveLocked(generation, id);
}

// The one request-identity clear, used by the turn that completed a request and
// by the close turn that cancels the request it captured: only the request this
// identity names is ended, and a newer one keeps its own.
bool EndMeasureRequest(uint32_t generation, uint64_t id) {
    if (id == 0) {
        return false;
    }
    std::lock_guard lock(g_measureMutex);
    if (g_measureRequest.id != id ||
        g_measureRequest.generation != generation) {
        return false;
    }
    g_measureRequest = SettledMeasureRequest{};
    return true;
}

// Drops whatever is live, on teardown: nothing measured for the previous open
// may land on the next one.
void CancelMeasureRequest() {
    std::lock_guard lock(g_measureMutex);
    g_measureRequest = SettledMeasureRequest{};
}

// The close path captures the request before the UI turn is posted, so the turn
// can cancel exactly that request and no later one.
SettledMeasureRequest SnapshotMeasureRequest() {
    std::lock_guard lock(g_measureMutex);
    return g_measureRequest;
}

// Runs the one geometry measurement on the UI thread. Every trigger comes through
// here, so the axis, the travel, the panel rect and the scroller are measured and
// validated against the state at the moment of the run: the epoch the request was
// recorded in, whether a root is attached, and whether a gesture owns the panel.
// `settled` is the pass after the shell placed the panel; it is also the pass
// that ends the request.
void RunGeometryMeasurement(const SettledMeasureRequest& request, bool settled) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }
    if (!MeasureRequestLive(request.generation, request.id)) {
        return;  // superseded, already run, or cancelled by a close
    }
    if (request.generation != g_openGeneration.load(std::memory_order_relaxed)) {
        // The menu this was recorded for is gone: a close or a new open moved
        // the epoch, so nothing measured now belongs to what is on screen.
        EndMeasureRequest(request.generation, request.id);
        return;
    }
    if (!g_root) {
        EndMeasureRequest(request.generation, request.id);
        return;
    }
    bool haveRenderer = false;
    {
        std::lock_guard lock(g_compositionMutex);
        haveRenderer = g_state != nullptr;
    }
    if (!haveRenderer) {
        // The measure mutex is never taken under the composition mutex; the two
        // sections here are separate for that reason.
        EndMeasureRequest(request.generation, request.id);
        return;
    }

    try {
        // One policy for every layout change: the whole apply runs under one
        // reservation that gesture admission observes. While a live contact owns
        // the panel, or claims it while this pass is entering, the change is
        // deferred whole and re-applied once when ownership ends. The ownership
        // test must be part of the reservation, not a read before it: a finger
        // that took the panel in between would otherwise drive scalars that are
        // being rewritten under it.
        LayoutApplyGuard apply;
        if (!apply.Begin()) {
            g_measureDeferred.store(true, std::memory_order_relaxed);
            // The deferred settled pass is this open's measurement turn, so it is
            // the one that writes the once-per-open trail line.
            g_panelTrailLogged = false;
            // One line per request: only the first of the two triggers runs it.
            TraceLog(L"gesture geometry deferred reason=panel-owned gen=%u",
                    request.generation);
            EndMeasureRequest(request.generation, request.id);
            return;
        }
        if (!RefreshGestureGeometry()) {
            // The composition did not take the new axis or travel, so the
            // published snapshot must keep the old ones. Nothing to measure.
            EndMeasureRequest(request.generation, request.id);
            return;
        }
        if (!MeasureRequestLive(request.generation, request.id)) {
            return;  // a newer request owns the open now
        }
        if (!settled) {
            // One trail line per open, and the open-time turn is where it starts:
            // the settled turn then only speaks when the value moved.
            g_panelTrailLogged = false;
        }
        // `g_panelTrailRejectedLogged` is not touched here: the publish keys it
        // by the open generation, so a rejected rect speaks once per open, from
        // whichever pass sees the first one.
        // The edge may only move while no contact drives it. This is re-read
        // inside the publish, at commit, not taken when the request was recorded:
        // a finger that started between the two must not have the edge moved
        // under it. The generation and the request id are re-checked there too.
        const bool edgeIdle =
            request.trailIdle && !g_panelOwned.load(std::memory_order_relaxed);
        if (!PublishPanelTrailDip(edgeIdle, request.generation, request.id)) {
            EndMeasureRequest(request.generation, request.id);
            return;
        }
        if (!MeasureRequestLive(request.generation, request.id)) {
            return;
        }
        // The dismiss gate needs the scrolling element of this open. The
        // open-time pass records it, the settled pass records it again with the
        // list's final extent, and the settled pass is what arms the gate. It
        // validates the same request and epoch before it commits.
        CacheScrollableElement(settled, request.generation, request.id);
    } catch (...) {
        // A closing menu must not break the dispatcher turn.
        EndMeasureRequest(request.generation, request.id);
        return;
    }

    if (settled) {
        EndMeasureRequest(request.generation, request.id);
        return;
    }

    // The settled pass belongs to the layout watch's second pass; the deadline in
    // the request is the guaranteed path when no second pass comes.
    ArmLayoutWatch(request);
}

// Posts one turn of the measurement. The request travels with the turn, so a turn
// that runs late can tell a superseded request from the live one. The turn is a
// counted module entry: the unload waits for it before it frees the XAML and
// composition state the measurement reads.
void PostGeometryMeasurement(const SettledMeasureRequest& request, bool settled) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }
    wucore::CoreDispatcher dispatcher = CopyDispatcher();
    if (!dispatcher) {
        return;
    }
    try {
        PostCountedUiTurn(dispatcher, [request, settled]() {
            RunGeometryMeasurement(request, settled);
        });
    } catch (winrt::hresult_error const& e) {
        TraceLog(L"gesture geometry queue err=%08X",
                static_cast<unsigned>(e.code()));
    } catch (...) {
        TraceLog(L"gesture geometry queue err=unknown");
    }
}

// Posted by the input thread when the shell reports the window cloaked. The
// measurement request and both watches belong to the open that just ended: a
// measurement of that layout must not publish into the next open, and a cached
// scroller would gate the next open against the old rect and offsets. The UI
// thread releases them.
//
// The turn names what it owns instead of clearing whatever is live: the request
// identity captured before the close (id and the open it was recorded for), and
// the closed open's generation for the layout watch and the scroller cache. A
// reopen that installs its own request and its own watch before this turn runs
// keeps them. The turn is a counted module entry, like every posted turn that
// touches module state.
void PostUiOpenWatchRelease(uint32_t closedGeneration,
                            const SettledMeasureRequest& closing) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    wucore::CoreDispatcher dispatcher = CopyDispatcher();
    if (!dispatcher) {
        return;
    }

    try {
        PostCountedUiTurn(dispatcher, [closedGeneration, closing]() {
            try {
                if (g_unloading.load(std::memory_order_relaxed)) {
                    return;
                }
                EndMeasureRequest(closing.generation, closing.id);
                StopSettledTrailWatchIfGeneration(closedGeneration);
                ReleaseScrollableElementIfGeneration(closedGeneration);
                // The retry token is not cleared here: a newer open may
                // already have scheduled its own, and a stale token is
                // dropped by the generation check in the idle tick.
            } catch (...) {
                // Never let an exception escape the dispatcher callback.
            }
        });
    } catch (winrt::hresult_error const& e) {
        TraceLog(L"open watch release err=%08X", static_cast<unsigned>(e.code()));
    } catch (...) {
        TraceLog(L"open watch release err=unknown");
    }
}

// The open/close transitions the service below observes, including why a close
// that is not this mod's own is not fought, are documented at
// ServiceRootVisibility.

// M3: one intent handler for the bridge's four triggers. The controller never
// branches on the trigger: open sets the entry and asks the bridge for Show,
// close runs the one mirrored close and asks for Dismiss, external drops the
// pull and resyncs. The bridge's executor acks through execAck.
//
// The record is passed in, not re-read. The landing reconciliation examined a
// snapshot before it applied anything; re-reading there could consume a newer
// record (an open published while the external was being examined) and then
// claim the external that was seen had been applied.
void ApplyBridgeIntentRecord(const SwipeIpcIntent& intent) {
    if (intent.seq == 0 || intent.seq == g_lastIntentSeq) {
        return;
    }
    g_lastIntentSeq = intent.seq;
    const wchar_t* kind = intent.kind == kSwipeIntentOpen
                              ? L"open"
                              : (intent.kind == kSwipeIntentClose ? L"close"
                                                                  : L"external");
    const wchar_t* trigger =
        intent.trigger == kSwipeTriggerWin
            ? L"win"
            : (intent.trigger == kSwipeTriggerStartButton
                   ? L"startbutton"
                   : (intent.trigger == kSwipeTriggerClickAway ? L"clickaway"
                                                               : L"override"));
    TraceLog(L"intent recv seq=%lld kind=%s trigger=%s method=%u flags=%u "
            L"open=%llu",
            static_cast<long long>(intent.seq), kind, trigger, intent.method,
            intent.flags, static_cast<unsigned long long>(intent.open));

    int32_t ackState = kSwipeAckExecuted;
    int32_t ackDuration = 0;
    int32_t ackEntry = 0;
    HRESULT ackHr = S_OK;

    if (intent.kind == kSwipeIntentExternal) {
        // External ownership wins: both close schedules are revoked and the
        // cancel-recovery ticket is dropped, so nothing of ours can still hide
        // the window or reissue an open the shell did not ask for. The
        // suppression goes too: this takeover's close is the shell's, and the
        // poll must classify its landing as external (drop the pull, no reopen).
        const CloseScheduleCancel dropped = CancelCloseSchedules(L"external");
        g_intentCancel = CloseCancelTicket{};
        g_suppressRestoreUntil.store(0, std::memory_order_relaxed);
        g_intentOpenActive.store(false, std::memory_order_relaxed);
        ForgetPendingDismiss();
        if (g_hidSession.active) {
            g_hidSession.active = false;
            EndContactTiming();
        }
        g_reveal = HidRevealSession{};
        g_contactOrigin = HidContactOrigin{};
        g_lateRelease = RevealLateRelease{};
        SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
        SwipeIpcSetControllerTouch(&g_ipc, false);
        g_commitNeedsProgressReset = false;
        // The takeover's own close is still a transition this mod must classify
        // (the landing tells an external close from ours), so it is watched the
        // same way a requested one is.
        NoteVisibilityTransition(kVisibilityWantClosed);
        TraceLog(L"intent external drop trigger=%s slot=%d command=%d", trigger,
                dropped.slot ? 1 : 0, dropped.command ? 1 : 0);
    } else if (intent.kind == kSwipeIntentOpen) {
        // The deadline the cancelled command was scheduled for, read before the
        // cancel clears it, and the dismiss whose call already returned but whose
        // hide has not been confirmed. Both are read before the cancel: either
        // one means a dismiss may be inside the shell.
        const uint64_t dismissedAt =
            g_pendingDismissExecAt.load(std::memory_order_relaxed);
        const LONG64 executedDismiss = g_pendingDismissConfirm;
        const LONG64 cancelled = CancelPendingDismiss(L"open-intent");
        // The controller owns an open from this line, whichever shape the press
        // has; the bridge reads this direction for the next toggle.
        SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
        const GestureGeometry geometry = ReadGestureGeometry();
        if (StartWindowIsOpen()) {
            // The close this press answers has not hidden the window yet, so the
            // press cancels it. The local dismiss slot is dropped before the
            // executor command can fire, and that command is cancelled above;
            // the panel is taken where the
            // retract left it and settles back up with the pull-cancel's own
            // curve. The window never hides, so no show is requested and no entry
            // is applied: the menu is on screen at its live position.
            ClearLocalCloseTap();
            g_suppressRestoreUntil.store(0, std::memory_order_relaxed);
            g_commitNeedsProgressReset = false;
            g_intentOpenActive.store(false, std::memory_order_relaxed);
            ForgetPendingDismiss();
            // A dismiss that already entered the shell's Dismiss call cannot be
            // recalled by the cancel above; if it still hides the window, the
            // close's landing reissues this open once. The ticket names the menu
            // and the command, so no other landing can serve it.
            //
            // The ticket is armed only for a command that was published:
            //   - `cancelled != 0`: the record was still queued or armed and may
            //     already have entered `Dismiss`;
            //   - `executedDismiss != 0`: the call returned, its executed ack
            //     cleared the pending slot, and the hide may still land.
            // A local-only cancel (`cancelled == 0`, no executed dismiss) armed
            // nothing: no dismiss is inside the shell, so a later hide of this
            // same menu is not this press's close and must not request Show.
            //
            // The deadline bounds the ticket. A still-published command can run up
            // to its deadline, then take the ack budget and the confirmation probe;
            // a command whose call returned is measured from this press, because
            // its hide is imminent.
            const LONG64 ticketSeq =
                cancelled != 0 ? cancelled : executedDismiss;
            const uint64_t now = GetTickCount64();
            const uint64_t expireAt =
                ticketSeq != 0
                    ? ((cancelled != 0 && dismissedAt != 0) ? dismissedAt : now) +
                          kBridgeAckTimeoutMs + kDismissConfirmGraceMs
                    : 0;
            g_intentCancel =
                ticketSeq != 0
                    ? CloseCancelTicket{true, g_ipcOpenTickLow, ticketSeq,
                                        expireAt}
                    : CloseCancelTicket{};
            const float live = TakeProgressNow(L"intent-open-cancel");
            std::chrono::milliseconds duration{0};
            if (!BeginGestureSettle(live, 1.0f, 0.0f, duration)) {
                ShowAtRest();
            }
            TraceLog(L"intent open cancel from=%.3f ticket=%d ticket_open=%u "
                    L"ticket_seq=%lld ticket_expire=%llu",
                    live, g_intentCancel.pending ? 1 : 0,
                    g_intentCancel.openTick,
                    static_cast<long long>(g_intentCancel.dismissSeq),
                    static_cast<unsigned long long>(g_intentCancel.expireAt));
        } else {
            // The window is already hidden: the close this press answers has
            // landed, so its command (already cancelled above) and any retained
            // confirmation end before the new open claims the panel.
            ForgetPendingDismiss();
            const float entry =
                std::clamp(RevealEntryProgress(geometry), 0.0f, 1.0f);
            g_intentOpenEntry.store(entry, std::memory_order_relaxed);
            g_intentOpenActive.store(true, std::memory_order_relaxed);
            g_panel.Begin(&entry);
            if (RequestBridgeShow(intent.method ? intent.method : 1,
                                  intent.flags)) {
                ackEntry = static_cast<int32_t>(std::llround(entry * 1000.0f));
                TraceLog(L"intent open entry=%.3f", entry);
            } else {
                // No sequence was published, so there is nothing a timeout could
                // repair. The panel is not claimed: the shell's own gesture (the
                // band contact) opens the menu and the next cloak event adopts it.
                ShowAtRest();
                g_commitNeedsProgressReset = false;
                ReleaseExecutorOwnership();
                ackState = kSwipeAckRefused;
                ackHr = E_FAIL;
                TraceLog(L"intent open refused entry=%.3f reason=no-executor",
                        entry);
            }
        }
    } else if (intent.kind == kSwipeIntentClose) {
        // One close at a time: a close intent supersedes whatever an earlier
        // close left scheduled, local slot and executor command alike.
        CancelCloseSchedules(L"close-intent");
        // The cancel above needs the exec slot to publish its `exec cancel`; the
        // confirmation and the generation are void from here.
        ForgetPendingDismiss();
        // The close owns the panel now: one close mechanism, the same ownership
        // the gesture and keyboard closes take. It sets the restore suppression
        // so the visibility poll cannot "heal" the retracted panel while the
        // bridge's dismiss is still on its way, and it mutes a contact still on
        // the glass.
        TakeCloseOwnership(GetTickCount64(), g_contactDown);
        const float from = TakeProgressNow(L"intent-close");
        std::chrono::milliseconds duration{0};
        if (!BeginMirrorCloseSettle(from, duration)) {
            // No settle, so there is no close to own: the panel returns to rest
            // and the controller states its release in one place.
            ShowAtRest();
            g_commitNeedsProgressReset = false;
            ReleaseExecutorOwnership();
            ackState = kSwipeAckRefused;
            ackHr = E_FAIL;
        } else {
            g_commitNeedsProgressReset = true;
            SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionClosing);
            const uint64_t executeAt =
                GetTickCount64() + static_cast<uint64_t>(duration.count()) +
                kBridgeDismissLeadMs;
            if (RequestBridgeDismiss(
                    ExecDismissMethodForIntent(intent.method, intent.trigger),
                    executeAt, intent.open)) {
                ackDuration = static_cast<int32_t>(duration.count());
                TraceLog(L"intent close from=%.3f dur=%lld", from,
                        static_cast<long long>(duration.count()));
            } else {
                // Nothing will hide this window. The settle already retracted the
                // panel; the ownership goes back now and the unhealed rule
                // restores the panel after the suppression window.
                ReleaseExecutorOwnership();
                ackState = kSwipeAckRefused;
                ackHr = E_FAIL;
                TraceLog(L"intent close refused from=%.3f reason=no-executor",
                        from);
            }
        }
    }
    SwipeIpcAnswerIntent(&g_ipc, intent.seq, ackState, ackDuration, ackEntry,
                         static_cast<int32_t>(ackHr));
}

void ApplyBridgeIntent() {
    SwipeIpcIntent intent{};
    if (!SwipeIpcReadIntent(&g_ipc, &intent)) {
        return;
    }
    ApplyBridgeIntentRecord(intent);
}

// The one repair every terminal executor failure performs, whatever kind the
// command was. The controller stops owning the panel and stops marking the touch
// in the same step, and the pending open entry is dropped. The published
// direction then follows the window's cloak on the next transition, which is the
// one source of truth for a panel the controller no longer drives. A failed
// dismiss leaves the panel retracted with the window still shown; the
// suppression window still expires, and the unhealed rule then brings the panel
// back instead of leaving an invisible menu attached to a shown window.
void ReleaseExecutorOwnership() {
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
    SwipeIpcSetControllerTouch(&g_ipc, false);
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    g_intentOpenEntry.store(0.0f, std::memory_order_relaxed);
}

// Whether a dismiss command still belongs to the menu the section is showing.
// The command carries the open generation the Start side stamped when it was
// requested; the bridge refuses a record whose generation no longer matches, and
// this side uses the same rule before a landing may claim a close. The comparison
// is exact, including 0: a zero stamp is valid only while the section still has
// no open record. Once an open has been published, a zero-stamped command belongs
// to the menu before it and must not land the newer menu's hide. A failed read
// proves nothing and is never admissible.
bool DismissGenerationCurrent(uint64_t commandGeneration) {
    uint64_t currentGeneration = 0;
    uint64_t openTick = 0;
    if (!SwipeIpcReadOpen(&g_ipc, &currentGeneration, &openTick)) {
        return false;
    }
    return commandGeneration == currentGeneration;
}

void ApplyExecAck() {
    SwipeIpcExecAck ack{};
    if (!SwipeIpcReadExecAck(&g_ipc, &ack)) {
        return;
    }
    // The visible word is a tri-state now; a negative value says the bridge could
    // not answer, not that the window is hidden.
    TraceLog(L"exec ack seq=%lld command=%lld state=%u hr=%08X visible=%d",
            static_cast<long long>(ack.seq),
            static_cast<long long>(ack.command), ack.state,
            static_cast<unsigned>(ack.hr), ack.visible);
    const bool failed =
        ack.state == kSwipeAckRefused || ack.state == kSwipeAckExpired;
    const bool executed = ack.state == kSwipeAckExecuted;
    const bool confirmed = ack.state == kSwipeAckConfirmed;
    const bool unconfirmed = ack.state == kSwipeAckUnconfirmed ||
                             ack.state == kSwipeAckUnknown;
    const bool dismissPending =
        ack.command != 0 &&
        ack.command == g_pendingDismissExec.load(std::memory_order_relaxed);
    const bool dismissConfirming =
        ack.command != 0 && ack.command == g_pendingDismissConfirm;
    // The recovery ticket can outlive both pending slots: the open intent that
    // cancelled the close clears them before the bridge's answer arrives. The
    // ticket is bound to the command it revoked, so that sequence is the whole
    // test; a terminal answer for it is the same proof the slot path uses — the
    // command cannot still hide the window. `executed` is not terminal (the call
    // returned and the hide may still land), `confirmed` belongs to the landing,
    // and `unknown` may still hide the window, so none of them retires it.
    const bool dismissTicket =
        g_intentCancel.pending && ack.command != 0 &&
        g_intentCancel.dismissSeq == ack.command;
    if (dismissTicket &&
        (failed || ack.state == kSwipeAckSuperseded)) {
        const wchar_t* reason = ack.state == kSwipeAckSuperseded
                                    ? L"superseded"
                                    : (ack.state == kSwipeAckRefused
                                           ? L"refused"
                                           : L"expired");
        TraceLog(L"intent recovery ticket retired seq=%lld reason=%s",
                static_cast<long long>(ack.command), reason);
        g_intentCancel = CloseCancelTicket{};
    }
    if (dismissPending || dismissConfirming) {
        // One dismiss has up to three answers: the primitive's (`Executed` or a
        // terminal failure), the visibility probe's (`Confirmed`), and the
        // repair a probe that could not confirm performs. All are handled here,
        // and the pending slot is cleared only by the outcome that ends it.
        if (executed) {
            // Returned is not hidden. The command stays identified until the
            // confirmation probe (or the landing) says what happened.
            if (dismissPending) {
                g_pendingDismissConfirmGeneration =
                    g_pendingDismissExecGeneration;
                g_pendingDismissExec.store(0, std::memory_order_relaxed);
                g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
                g_pendingDismissExecGeneration = 0;
            }
            g_pendingDismissConfirm = ack.command;
            TraceLog(L"exec dismiss executed command=%lld visible=%d "
                    L"(awaiting confirmation)",
                    static_cast<long long>(ack.command), ack.visible);
            return;
        }
        if (confirmed) {
            TraceLog(L"exec dismiss confirmed command=%lld visible=%d",
                    static_cast<long long>(ack.command), ack.visible);
            // The probe said the window is hidden. If a cloak transition had
            // carried this hide, it would have landed the close and cleared the
            // slot that still names this command; the slot surviving means no
            // transition arrived inside the probe's own bound. The cloak is read
            // again here because it, not the probe, is the state, and the read is
            // tri-state: a failed read is unknown, not hidden.
            bool retained = false;
            if (ack.visible == 0) {
                const uint64_t commandGeneration =
                    dismissConfirming
                        ? g_pendingDismissConfirmGeneration
                        : g_pendingDismissExecGeneration;
                const CloakState cloak = ReadCloakState();
                if (!DismissGenerationCurrent(commandGeneration)) {
                    // A newer open owns the section: this command belongs to the
                    // menu that open replaced, and no hide of the current one is
                    // its close.
                    TraceLog(L"exec dismiss stale command=%lld gen=%llu",
                            static_cast<long long>(ack.command),
                            static_cast<unsigned long long>(commandGeneration));
                } else if (cloak == CloakState::Hidden) {
                    // The window genuinely reads hidden and no transition carried
                    // it: the close finishes here, under the same identity.
                    LandClosedMenu(GetTickCount64(),
                                   L"confirmed-without-transition");
                } else {
                    // The cloak disagrees or cannot answer. Keep the command and
                    // its generation: the transition is the one event allowed to
                    // land it, and the pull it holds must not lose its identity
                    // because a sample disagreed. The bound retires it if no
                    // transition comes.
                    if (dismissPending) {
                        g_pendingDismissConfirm = ack.command;
                        g_pendingDismissConfirmGeneration =
                            g_pendingDismissExecGeneration;
                    }
                    g_pendingDismissConfirmUntil =
                        GetTickCount64() + kDismissConfirmGraceMs;
                    g_pendingDismissConfirmRetained = true;
                    retained = true;
                    TraceLog(L"exec dismiss retained command=%lld gen=%llu "
                            L"cloak=%s",
                            static_cast<long long>(ack.command),
                            static_cast<unsigned long long>(
                                g_pendingDismissConfirmGeneration),
                            CloakStateName(cloak));
                }
            }
            // The exec slot has served its purpose; a landing or the retained
            // slot owns the command from here. A confirmation that could not be
            // retained (or was landed) retires the answer.
            g_pendingDismissExec.store(0, std::memory_order_relaxed);
            g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
            g_pendingDismissExecGeneration = 0;
            if (!retained) {
                ForgetConfirmedDismiss();
            }
            return;
        }
        if (dismissPending) {
            g_pendingDismissExec.store(0, std::memory_order_relaxed);
            g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
            g_pendingDismissExecGeneration = 0;
        }
        if (unconfirmed || failed) {
            // A dismiss that did not hide the window — refused, expired, still
            // visible at the probe, or unanswerable — is terminal: the panel is
            // retracted, the controller stops owning it, and the next transition
            // reads the cloak. A refused/expired ticket for this sequence was
            // retired above, before this gate; `unconfirmed`/`unknown` retires
            // one here, because the pending slot still names the command.
            if (unconfirmed && g_intentCancel.pending &&
                g_intentCancel.dismissSeq == ack.command) {
                g_intentCancel = CloseCancelTicket{};
            }
            ForgetConfirmedDismiss();
            ReleaseExecutorOwnership();
            TraceLog(L"exec dismiss %s command=%lld state=%u hr=%08X visible=%d",
                    unconfirmed ? L"unconfirmed" : L"failed",
                    static_cast<long long>(ack.command), ack.state,
                    static_cast<unsigned>(ack.hr), ack.visible);
            return;
        }
        // A superseded confirmation needs no repair: a newer command (or a new
        // activation) already owns the window. It is reported so the two cases
        // are distinguishable in the log.
        if (ack.state == kSwipeAckSuperseded) {
            ForgetConfirmedDismiss();
            TraceLog(L"exec dismiss superseded command=%lld",
                    static_cast<long long>(ack.command));
            return;
        }
        return;
    }
    if (ack.command != g_pendingShowExec.load(std::memory_order_relaxed)) {
        return;
    }
    g_pendingShowExec.store(0, std::memory_order_relaxed);
    g_pendingShowUntil.store(0, std::memory_order_relaxed);
    if (ack.state == kSwipeAckSuperseded) {
        TraceLog(L"exec show superseded command=%lld",
                static_cast<long long>(ack.command));
        return;
    }
    if (failed) {
        // The bridge did not run the show (no launcher, no window). Release the
        // panel so the shell's own gesture, which follows the band contact, is
        // passed through instead of swallowed. A close that is already pending
        // owns the panel now, and its state is not this failure's to clear.
        if (g_pendingDismissExec.load(std::memory_order_relaxed) == 0) {
            ReleaseExecutorOwnership();
        }
        TraceLog(L"exec show failed command=%lld state=%u hr=%08X",
                static_cast<long long>(ack.command), ack.state,
                static_cast<unsigned>(ack.hr));
    }
}

// A command the bridge never answered: the Start side stops owning the panel
// and reads the truth from the window's cloak on the next transition, exactly as
// it does for any open it did not perform. A dismiss expires from the deadline it
// was scheduled for, not from the request: the bridge's timer runs at
// `executeAt`, so the ack cannot be expected before that plus the same budget.
//
// The timeout invalidates the record at the publisher, not only in this process's
// bookkeeping. The executor can still have the record queued or armed, and a
// retracted panel waiting for a dismiss that never comes would be answered by
// running it late; the cancel names the sequence for a dismiss, and a show—which
// has no armed command to revoke—is superseded by a cancel record so the executor
// can never read it as its newest command.
void CheckExecAckTimeout(uint64_t now) {
    const LONG64 show = g_pendingShowExec.load(std::memory_order_relaxed);
    if (show != 0 && now >= g_pendingShowUntil.load(std::memory_order_relaxed)) {
        g_pendingShowExec.store(0, std::memory_order_relaxed);
        g_pendingShowUntil.store(0, std::memory_order_relaxed);
        // A pending close has already written its own record over this show's
        // slot, so a cancel now would replace the dismiss that is about to run.
        // Only a timeout with no close behind it supersedes the record.
        const bool dismissPending =
            g_pendingDismissExec.load(std::memory_order_relaxed) != 0;
        if (!dismissPending && g_ipc.state) {
            SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0, 0);
            TraceLog(L"exec cancel request target=0 reason=show-expired");
        }
        // Same rule as a refused show: a pending close owns the panel and its
        // state is not this silence's to clear.
        if (!dismissPending) {
            ReleaseExecutorOwnership();
        }
        TraceLog(L"exec ack timeout seq=%lld kind=show open=%d (resync from "
                L"cloak)",
                static_cast<long long>(show), StartWindowIsOpen() ? 1 : 0);
    }
    const LONG64 dismiss = g_pendingDismissExec.load(std::memory_order_relaxed);
    if (dismiss != 0) {
        const uint64_t executeAt =
            g_pendingDismissExecAt.load(std::memory_order_relaxed);
        if (executeAt != 0 && now >= executeAt + kBridgeAckTimeoutMs) {
            g_pendingDismissExec.store(0, std::memory_order_relaxed);
            g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
            g_pendingDismissExecGeneration = 0;
            if (g_ipc.state) {
                SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0,
                                    dismiss);
                TraceLog(L"exec cancel request target=%lld "
                        L"reason=dismiss-expired",
                        static_cast<long long>(dismiss));
            }
            ForgetConfirmedDismiss();
            ReleaseExecutorOwnership();
            TraceLog(L"exec ack timeout seq=%lld kind=dismiss open=%d (resync "
                    L"from cloak)",
                    static_cast<long long>(dismiss),
                    StartWindowIsOpen() ? 1 : 0);
            return;
        }
    }
    // A confirmation retained because the cloak disagreed or could not answer
    // waits for its transition only until this bound. Past it no landing can
    // claim the command, and the ordinary terminal repair resyncs from the cloak.
    const LONG64 retained = g_pendingDismissConfirm;
    if (!g_pendingDismissConfirmRetained || retained == 0 ||
        g_pendingDismissConfirmUntil == 0 ||
        now < g_pendingDismissConfirmUntil) {
        return;
    }
    TraceLog(L"exec dismiss retained expired command=%lld gen=%llu (resync "
            L"from cloak)",
            static_cast<long long>(retained),
            static_cast<unsigned long long>(
                g_pendingDismissConfirmGeneration));
    ForgetPendingDismiss();
    ReleaseExecutorOwnership();
}

// A superseding intent that arrived with the landing. See ServiceRootVisibility:
// the cloak event and the intent event are both waited on, so an intent that
// arrived with the landing is read here before the landing acts. Only `external`
// is reconciled here — an open or close intent is the user
// talking to a menu, and the landing's own pull/cancel rules already own that
// case. ApplyBridgeIntent consumes the record, so the event's later wake finds
// nothing new.
void ReconcileSupersedingIntentBeforeLanding() {
    SwipeIpcIntent intent{};
    if (!SwipeIpcReadIntent(&g_ipc, &intent) || intent.seq == 0 ||
        intent.seq == g_lastIntentSeq) {
        return;
    }
    if (intent.kind != kSwipeIntentExternal) {
        return;
    }
    TraceLog(L"intent reconcile seq=%lld kind=external (before landing recovery)",
            static_cast<long long>(intent.seq));
    // Apply the record this function examined, not whatever the section holds by
    // the time it runs: the claim is "the external we saw was applied", and a
    // second read could consume a newer open instead.
    ApplyBridgeIntentRecord(intent);
}

// The one landing-open transaction. Both reasons a close's landing can serve an
// open end here: a pull that was down (or lifted inside the late window) when the
// close landed, and an open intent that cancelled a close whose dismiss still
// ran. Entry, progress, the pending flag, the controller state and the show
// request are one sequence; the caller chooses the reason and the log line.
//
// Touch ownership follows the *finger*, not the reason. Only a contact that is
// still down owns the opening panel (`pullDown`); a release record means the
// contact already lifted, and marking the shared touch word for it made the
// bridge swallow the next real desktop click as `reason=touch-owner` on a menu
// no finger was touching.
bool BeginLandingOpen(bool pullReopen, bool pullDown, bool lateFresh) {
    if (!RendererReady() || StartWindowIsOpen() ||
        g_intentOpenActive.load(std::memory_order_relaxed)) {
        return false;
    }
    const GestureGeometry geometry = ReadGestureGeometry();
    float entry = 0.0f;
    if (!PublishOpenClaim(geometry, pullDown, entry)) {
        // The landing cannot present anything: the panel stays retracted with the
        // window hidden, and the controller claims nothing. The next open
        // (the shell's own or a later pull) starts from the entry again.
        TraceLog(L"landing open failed entry=%.3f pull=%d reason=no-executor",
                entry, pullReopen ? 1 : 0);
        return false;
    }
    if (pullReopen) {
        TraceLog(L"reveal reopen after close pull=%d late=%d entry=%.3f",
                pullDown ? 1 : 0, lateFresh ? 1 : 0, entry);
    } else {
        TraceLog(L"intent open recovered entry=%.3f", entry);
    }
    return true;
}

// The pull that may reopen Start when a close lands. A contact merely resting in
// the band during the retract is not that pull: its last positioned report must
// have moved away from the edge by kAdoptTravelDip, the same qualification the
// open claim uses. An active follow qualifies by construction: it has driven the
// panel. A qualified late release is a separate record and does not come through
// here.
bool RevealPullQualified(const GestureGeometry& geometry) {
    if (g_reveal.active) {
        return true;
    }
    if (!g_reveal.edgeCandidate || !g_contactOrigin.valid) {
        return false;
    }
    return RevealTravelDip(geometry.axis(), g_reveal.edgeStartX,
                           g_reveal.edgeStartY, g_contactOrigin.x,
                           g_contactOrigin.y) >= kAdoptTravelDip;
}

// The close-landing body. Its usual caller is the cloak transition: the window
// went from shown to hidden and this classifies the close, releases a pull that
// waited for it, and resets the controller. The executor's confirmation probe is
// the second caller: it can find the window already hidden with no transition
// left to arrive (the dismiss ran on a window something else had hidden, a
// native hide won the race, or the transition was lost). The body is shared on
// purpose — one landing, one set of generation and ticket rules — but the log
// names the cause, because only the first one is a transition the shell
// performed. The command identity is the same in both: a pending dismiss slot (or
// the answer that names it) must still call this close its own.
void LandClosedMenu(uint64_t tickMs, const wchar_t* reason) {
    // A landing must not outrun a takeover that arrived with it. The idle
    // timer and the intent event are two wait objects of this thread, and the
    // timer has the lower index: the landing can be served before the
    // `external` intent that came with it is processed, and processing it
    // afterwards does not revoke a Show this branch already issued. The
    // already-published superseding intent is reconciled first, and it clears
    // the suppression and the recovery ticket as any external takeover does.
    ReconcileSupersedingIntentBeforeLanding();
    // Whether the hide that just landed belongs to a close this mod
    // started. The suppression *window* is not the evidence: it is 600 ms,
    // and a maximum-duration close (520 ms settle + the 30 ms lead +
    // launcher dispatch + cloak delivery) measured 579-594 ms, so a busy
    // launcher can pass it and a genuine pull held through that close would
    // be dropped as an external takeover. The command is the identity: a
    // dismiss of ours is queued/armed, or executed and awaiting its
    // confirmation, for exactly the menu this landing closes. Every path
    // that revokes it (a cancel, a failure, an external intent) clears those
    // slots first, so this cannot claim someone else's close. A close whose
    // command an open intent revoked is added below through its ticket.
    //
    // The command is only ours while the open generation it was stamped with is
    // still the section's current one. A retained confirmation from a menu that a
    // later open replaced is a leftover, not this landing's close; the bridge
    // refuses a record by the same rule, so this side must too.
    const bool execCommand =
        g_pendingDismissExec.load(std::memory_order_relaxed) != 0 &&
        DismissGenerationCurrent(g_pendingDismissExecGeneration);
    const bool confirmCommand =
        g_pendingDismissConfirm != 0 &&
        DismissGenerationCurrent(g_pendingDismissConfirmGeneration);
    const bool closeCommand = execCommand || confirmCommand;
    // An open that cancelled a close while the window still showed is owed
    // only if that close still lands, and only for the menu the cancel named.
    // The ticket is consumed here, by the landing, and by nothing else: the
    // still-open read below must not clear it, because between the cancel and
    // the cloak transition the window is open for the whole window in which
    // the dismiss can still land, and the suppression may already have expired
    // by the time it does. A new close clears it in TakeCloseOwnership, an
    // external takeover clears it in ApplyBridgeIntent, and a failed dismiss
    // clears it in ApplyExecAck; those are the explicit superseding actions.
    //
    // The ticket also has a deadline: after the revoked command's own
    // deadline plus the ack budget and the confirmation probe, the dismiss can
    // no longer be inside the shell, so a landing after that is some other
    // close of the same menu, not this press's.
    const bool ticketCurrent =
        g_intentCancel.pending && g_intentCancel.openTick == g_ipcOpenTickLow;
    const bool ticketLive =
        ticketCurrent &&
        (g_intentCancel.expireAt == 0 || tickMs <= g_intentCancel.expireAt);
    const bool intentCancelLost = ticketLive;
    if (g_intentCancel.pending) {
        TraceLog(L"intent recovery ticket open=%u now=%u seq=%lld served=%d "
                L"expired=%d",
                g_intentCancel.openTick, g_ipcOpenTickLow,
                static_cast<long long>(g_intentCancel.dismissSeq),
                intentCancelLost ? 1 : 0,
                ticketCurrent && !ticketLive ? 1 : 0);
    }
    g_intentCancel = CloseCancelTicket{};
    // A close whose command an open intent revoked can still land. The
    // ticket names that command and the menu it belongs to, so the landing
    // is still this mod's close and its pull is released, not dropped.
    const bool closeOurs = closeCommand || intentCancelLost;
    if (closeOurs && confirmCommand && g_pendingDismissConfirmRetained) {
        // The confirmation the cloak refused to settle has now been carried by
        // the transition. Naming the command and generation here is the proof
        // that the retained close, not a later menu's hide, is what landed.
        TraceLog(L"exec dismiss retained served command=%lld gen=%llu",
                static_cast<long long>(g_pendingDismissConfirm),
                static_cast<unsigned long long>(
                    g_pendingDismissConfirmGeneration));
    }
    // The close has landed, so its suppression no longer applies. Without
    // this a Win press right after the hide opened the window with Progress
    // still at 0, and the panel was invisible until the next open.
    g_suppressRestoreUntil.store(0, std::memory_order_relaxed);
    if (closeOurs) {
        // A pull that arrived during the close is the pull that opens the
        // menu this hide made room for. The landing is the one moment its
        // deferral can end: the retract is over, nothing else writes
        // Progress, and the open's own poll either promotes the contact from
        // its down point (still down) or consumes the release record it left
        // (already lifted).
        if (g_reveal.deferred) {
            g_reveal.deferred = false;
            TraceLog(L"reveal defer released candidate=%d down=%d",
                    g_reveal.edgeCandidate ? 1 : 0,
                    g_contactOrigin.valid ? 1 : 0);
        }
        // The release record of a pull that lifted during this close becomes
        // consumable now: its end speed was measured at the lift, and the
        // window it must arrive in starts at the landing, not at the lift.
        if (g_lateRelease.valid && g_lateRelease.deferred) {
            g_lateRelease.deferred = false;
            g_lateRelease.readyQpc = QpcMsNow();
        }
    } else {
        // A close this mod did not start: an external takeover owns the
        // screen. The pull is dropped, not deferred — the takeover's window
        // is not a menu to reopen — and the rest of the contact stays inert
        // until it lifts. No dismiss command, no Progress write: the
        // shell did all of it.
        const bool contactLive =
            g_contactDown || g_hidSession.active || g_reveal.active ||
            g_reveal.edgeCandidate;
        if (g_reveal.active || g_reveal.edgeCandidate ||
            g_reveal.deferred) {
            g_reveal = HidRevealSession{};
            TraceLog(L"reveal dropped (window closed by shell)");
        }
        if (g_hidSession.active) {
            g_hidSession.active = false;
            EndContactTiming();
            TraceLog(L"hid dropped (window closed by shell)");
        }
        if (contactLive) {
            g_hidSession.suppress = true;
            g_contactOrigin = HidContactOrigin{};
        }
        g_lateRelease = RevealLateRelease{};
    }
    // The closed menu shows no panel, and the layout of the next open is a
    // new epoch: a measurement still in flight belongs to the menu that just
    // went away. The request id is captured before the epoch moves, so the
    // deferred release cancels exactly the request of this open.
    const uint32_t closedGeneration =
        g_openGeneration.load(std::memory_order_relaxed);
    const SettledMeasureRequest closing = SnapshotMeasureRequest();
    AdvanceOpenGeneration();
    if (reason) {
        // A close the confirmation probe had to finish: no cloak event carried
        // it, so the cause is named. The word `observed` stays, because the
        // close is as real as any the transition reports.
        TraceLog(L"win close observed reason=%s", reason);
    } else {
        TraceLog(L"win close observed");
    }
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
    SwipeIpcSetControllerTouch(&g_ipc, false);
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    // Both slots and both generations end here, whether the landing consumed
    // them as its close or classified the close as external and dropped them.
    ForgetPendingDismiss();
    // The measurement request and the cached scroller belong to the open that
    // just ended; the UI thread releases them.
    PostUiOpenWatchRelease(closedGeneration, closing);

    // A pull that was down when this close landed is the pull that takes the
    // menu back — but only a qualified one. A finger merely resting in the
    // band during the retract is not a pull, and the retract is not
    // permission to reopen the menu it just closed. The qualification is the
    // same signed inward travel the open claim uses; a release record left
    // by a real pull (lateFresh) stays valid on its own.
    const uint64_t closeQpc = QpcMsNow();
    const bool pullDown =
        g_contactDown && RevealPullQualified(ReadGestureGeometry());
    // The window starts when the record became consumable: readyQpc for a
    // pull that waited for this landing, releaseQpc for one that lifted on
    // the closed menu. The release time stays the age the adopt line
    // reports, never the deadline of a record that became usable late.
    const uint64_t readyQpc =
        g_lateRelease.readyQpc != 0 ? g_lateRelease.readyQpc
                                    : g_lateRelease.releaseQpc;
    const bool lateFresh =
        g_lateRelease.valid && !g_lateRelease.deferred &&
        readyQpc != 0 && closeQpc >= readyQpc &&
        closeQpc - readyQpc <= kLateRevealWindowMs;
    // The reason is computed first, then one transaction runs: the pull keeps
    // its precedence over the cancel-recovery, and only the pull marks the
    // contact as the panel's owner.
    const bool pullReopen = pullDown || lateFresh;
    if (pullReopen) {
        BeginLandingOpen(true, pullDown, lateFresh);
    } else if (intentCancelLost) {
        BeginLandingOpen(false, false, false);
    }
    // The window is hidden from this point on. A cloak wake that was already
    // queued for the same hide reads this and finds no transition to run, so
    // the landing is served once whatever completed it.
    g_rootWasOpen = false;
}

// Two things happen here. A closed-to-open transition that no reveal owns is a
// keyboard, Start-button or tray open: the shell slides its own storyboard and
// the mod makes `Progress` follow from 0, so the same root slides the same way a
// gesture open does. The settle needs the measured travel, so an open that lands
// before the first geometry publish restores the resting value and lets the
// shell's storyboard run instead. A close that is not this mod's own (an
// external takeover: Win+Tab, the lock screen, a display change) is not fought:
// nothing is inserted, the pull is dropped, and the next open starts from 0
// again.
//
// It runs when a cloak event says the window changed, not on a clock. The
// uncloak used to be found by a 20 ms poll: measured against the uncloak, a
// 100 ms throttle started the open slide up to 94 ms late, so the panel sat at
// rest first and then jumped out and slid back in, and 20 ms kept the start
// within one compositor frame of the shell's own open. The event is the same
// latency without the query: the thread is signalled the moment DWM cloaks or
// uncloaks the CoreWindow, and the attribute read below is then the answer, not
// a sample of a slow clock. A transition whose event is missed is still caught
// by the watch (see NoteVisibilityTransition): a requested or just-observed
// transition opens a bounded window in which the slow fallback poll runs.
//
// The open transition is also the one place the gesture geometry is refreshed
// without a system event: the XAML root is reused between opens, so
// PrepareRootElement does not fire again and the axis, Distance and DirX/DirY
// captured by Setup can belong to a taskbar that has moved since.
//
// The landing runs on this thread as well: a superseding intent that arrived
// with the landing is reconciled before the landing may serve a recovery
// (ReconcileSupersedingIntentBeforeLanding), or the now-external close would
// reissue an open the takeover had just revoked. The intent event and the cloak
// event are waited on in the same set, and the landing reads the intent record
// itself, so the order between the two wakes cannot resurrect a revoked open.
bool ServiceRootVisibility(uint64_t tickMs, bool viaFallback) {
    // A failed read is not a state. Keep the last sample and let the event or the
    // watch try again: treating the failure as hidden would land a close nothing
    // observed, and treating it as open would run the open path for a menu that
    // may still be closed.
    const CloakState sample = ReadCloakState();
    if (sample == CloakState::Unknown) {
        return false;
    }
    const bool open = sample == CloakState::Open;
    const bool wasOpen = g_rootWasOpen;
    // The open state this call is a sample of. A request stops being waited for
    // the moment the state matches it; every observation keeps a short settle
    // watch so the unhealed repair below still gets its polls after the open's
    // posted measurement turn. A fallback call never extends the watch, or the
    // poll would keep itself alive.
    const uint32_t want = g_visibilityWant;
    if ((want == kVisibilityWantOpen && open) ||
        (want == kVisibilityWantClosed && !open)) {
        g_visibilityWant = kVisibilityWantNone;
    }
    if (!viaFallback) {
        const uint64_t watch = tickMs + kVisibilitySettleWatchMs;
        if (watch > g_visibilityPollUntil) {
            g_visibilityPollUntil = watch;
        }
    }
    const bool justOpened = open && !wasOpen;
    // A close this mod started reports the window open until the shell has run
    // the Hide its dismiss caused. Reading that as a reopen restored Progress 1
    // while the close was still in flight, so a follow that began in that window
    // flashed the panel fully open and the window then vanished.
    const bool closeInFlight = CloseInFlight(tickMs);
    if (justOpened) {
        // An open the shell really performed while this mod still held a close in
        // flight. The window is shown again, so the close is over: its Hide either
        // already ran (and this is the next open) or was superseded. The
        // suppression is dropped here so the one open path below runs, and the
        // settle starts from the position the panel has now instead of from 0, so
        // the retract's last frame is continued rather than replayed.
        const bool openDuringClose = closeInFlight;
        if (openDuringClose) {
            TraceLog(L"win open during close progress=%.3f", PeekProgressNow());
            g_suppressRestoreUntil.store(0, std::memory_order_relaxed);
            g_reveal.deferred = false;
        }
        // A fresh menu, so every executor command written for the previous one is
        // void from this point on, whatever its own deadline says. The
        // generation goes out before any close of this open can be answered, and
        // the open was published on this same pass, so no user closes a menu they
        // have not seen yet.
        const LONG64 publishedOpen = SwipeIpcPublishOpen(&g_ipc, tickMs);
        g_ipcOpenTickLow = static_cast<uint32_t>(tickMs);
        // A fresh open retires every dismiss of the menu it replaced: a queued
        // command is cancelled at the bridge, and a confirmation is dropped. The
        // command's generation is older than the one just published, so it could
        // never land this menu's hide.
        RetirePendingDismissForFreshOpen(
            static_cast<uint64_t>(publishedOpen));
        // A new open is also a new geometry epoch: every measurement recorded for
        // the previous one is stale from this point on, and a request that was
        // still live is cancelled by the epoch check in its turn.
        AdvanceOpenGeneration();

        // The shell opens the menu from a strip wider than any fixed band this
        // mod can admit, so a pull that began inside the taskbar's thickness can
        // reach this line without a candidate. Adopt it here, at the open, so
        // the finger that caused the open owns the scalar from this frame and no
        // keyboard-style slide starts under it. A contact that began on the open
        // panel is a dismiss, and a tap has not moved: PromoteRevealContact tests
        // both, and promotes a candidate that is still down instead of waiting
        // for a report it may never get.
        const GestureGeometry geometry = ReadGestureGeometry();
        PromoteRevealContact(geometry, tickMs);

        // Exactly one of the three open paths runs, in this order:
        //
        // 1. the live takeover above — a still-down, moving finger that started
        //    while the menu was closed, promoted from its candidate or adopted
        //    without one; it owns Progress from this frame;
        // 2. the late release record — the pull that caused this open already
        //    lifted, and the record is what its settle starts from. It is read
        //    even when a new contact is already down, because that contact cannot
        //    be the one that opened the menu and may be refused on its next
        //    report;
        // 3. the keyboard settle — nothing preceded the open.
        //
        // The suppression test is that actual takeover, never the existence of an
        // edge candidate: a candidate that is down but not following (the open
        // has not reached the report path, and it may never reach it if the
        // finger lifts) must not take the animation away. PromoteRevealContact
        // above turns a moving candidate into the takeover, so the only candidate
        // left here is a tap or a resting finger, and the keyboard settle is the
        // right seam for both.
        const bool gestureOwnsProgress = g_reveal.active;

        // A pull that lifted just before the shell opened the menu left no live
        // contact, but it left the release record. Consume it before the keyboard
        // settle: the menu then continues from where the finger let go, at the
        // speed it let go with. A keyboard open with no such release finds no
        // record and plays from rest exactly as before.
        const bool lateAdopted =
            !gestureOwnsProgress && ConsumeLateRevealRelease(geometry);

        // The keyboard open is the gesture open from rest: one curve for every
        // open, so a Windows press and an edge pull cannot look different, and
        // the travel plans the duration exactly as it does there. An open that
        // overlapped a close still on its way out starts from where the panel is:
        // starting it at 0 would rewind the retract it just superseded.
        if (!gestureOwnsProgress && !lateAdopted) {
            // The settle needs the measured travel, from the snapshot the follow
            // reads. The measurement request below runs after this poll, so an
            // open that lands before the first publish still has Distance 0: a
            // settle planned from it would run the 160 ms floor. Restore the
            // resting value instead and let the shell's own storyboard run.
            if (geometry.distance() > 0.0f) {
                const bool intentEntry =
                    g_intentOpenActive.exchange(false, std::memory_order_relaxed);
                const float intentFrom =
                    g_intentOpenEntry.load(std::memory_order_relaxed);
                const float from = intentEntry
                                       ? intentFrom
                                       : (openDuringClose
                                              ? TakeVisibleProgressNow(
                                                    L"win-during-close")
                                              : 0.0f);
                std::chrono::milliseconds duration{0};
                if (BeginGestureSettle(from, 1.0f, 0.0f, duration)) {
                    // The animation now owns Progress, so no later path needs to
                    // restore the resting value for this open.
                    g_commitNeedsProgressReset = false;
                    if (intentEntry) {
                        TraceLog(L"win open intent from=%.3f dur=%llu", from,
                                static_cast<unsigned long long>(duration.count()));
                    } else if (openDuringClose) {
                        TraceLog(L"win open during close from=%.3f dur=%llu", from,
                                static_cast<unsigned long long>(duration.count()));
                    } else {
                        TraceLog(L"win open dur=%llu",
                                static_cast<unsigned long long>(duration.count()));
                    }
                }
            } else {
                ShowAtRest();
                // The open landed and there is no settle to start from the
                // intended entry, so the entry is spent. Leaving the flag set
                // would hold the idle release (a contact still down when the
                // menu appeared would never be released, and the landing's own
                // open test treats the flag as "an entry is pending").
                g_intentOpenActive.store(false, std::memory_order_relaxed);
                TraceLog(L"win open restore (no geometry)");
            }
        }

        // One measurement request per open, recorded before the first dismiss of
        // this open can start. The trail is only re-measured while no contact owns
        // the edge: a live follow reads it every sample, and the run-time check
        // catches a finger that starts between this record and the turn.
        const SettledMeasureRequest request = StartMeasureRequest(
            !g_hidSession.active && !g_reveal.edgeCandidate);
        PostGeometryMeasurement(request, false);
    } else if (!open && g_rootWasOpen) {
        LandClosedMenu(tickMs, nullptr);
    } else if (open) {
        // The window stayed uncloaked. The cancel-recovery ticket is deliberately
        // not cleared merely because this poll sees the window open: this poll can
        // run before the close's cloak transition, and the dismiss may still land
        // afterwards. Its own deadline is the one retirement that is not a claim
        // about the window: past it the revoked command cannot still be inside
        // the shell, so the ticket is dropped instead of waiting for a landing
        // that would belong to some later close of the same menu.
        if (g_intentCancel.pending && g_intentCancel.expireAt != 0 &&
            tickMs > g_intentCancel.expireAt) {
            TraceLog(L"intent recovery ticket retired open=%u now=%u seq=%lld "
                    L"reason=expired",
                    g_intentCancel.openTick, g_ipcOpenTickLow,
                    static_cast<long long>(g_intentCancel.dismissSeq));
            g_intentCancel = CloseCancelTicket{};
        }
        // The window has stayed uncloaked across polls. That is the healthy state
        // for an open menu, but a close that hid the menu and a reopen whose
        // settle never ran leave the same window shown with the panel at the
        // hidden rest: nothing of the menu is on screen, every pull is refused
        // with `reason=open` because the window is shown, and the band-tail mute
        // silences the rest. The pair is never left standing: log it once and
        // bring the panel back from where it is. A settle on the way up, a live
        // follow or a dismiss drag is left alone.
        const GestureGeometry geometry = ReadGestureGeometry();
        const float distance = geometry.distance();
        const float live = PeekProgressNow();
        const bool offScreen =
            distance > 0.0f && live < RevealEntryProgress(geometry);
        // A close still in flight owns this window: its Hide is ahead, and
        // restoring the panel there is the flash the suppression window exists to
        // prevent. Only a window whose close is over can be unhealed.
        const bool closeInFlight =
            tickMs < g_suppressRestoreUntil.load(std::memory_order_relaxed);
        if (offScreen && !closeInFlight && !ProgressMotionActive() &&
            !g_reveal.active && !g_hidSession.active) {
            if (tickMs > g_unhealedLogTick &&
                tickMs - g_unhealedLogTick >= 1000) {
                g_unhealedLogTick = tickMs;
                TraceLog(L"win open unhealed progress=%.3f", live);
            }
            std::chrono::milliseconds duration{0};
            if (!BeginGestureSettle(live, 1.0f, 0.0f, duration)) {
                ShowAtRest();
            }
            g_commitNeedsProgressReset = false;
        }
    }

    if (viaFallback && open != wasOpen) {
        // The net under the cloak event. A clean run never writes this line: the
        // event lands first and the watch is the only thing that would poll. It
        // appearing in a trace is the proof the fallback served a missed event.
        TraceLog(L"win state fallback open=%d want=%u", open ? 1 : 0, want);
    }
    g_rootWasOpen = open;
    return true;
}

// ---------------------------------------------------------------------------
// The one deadline
//
// Everything the input thread has to do on a clock is a deadline, and this is
// where they are collected. The soonest is armed as a single one-shot; with
// none, the timer is disarmed and the thread waits on its events alone. That
// state — no deadline — is what a loaded mod spends its life in.
//
// The two clocks stay apart. `tick` is GetTickCount64 (quantised to 15.6 ms
// here), `qpc` is QueryPerformanceCounter: the contact release, the candidate
// watchdog and the coast are measured on QPC because the tick cannot tell 18 ms
// from 31, while the close tap, the executor budgets and the measurement turn
// were always tick deadlines and stay on it. Both are reduced to "milliseconds
// from now", which is what the timer takes, and both are monotonic, so the
// minimum of the two is a real instant.
//
// A deadline that has already passed is armed at 1 ms only where serving it
// clears the condition that produced it (a one-shot turn, a release, a watchdog).
// The coast start is the exception: it is armed only while it is still ahead.
// MaybeCoast runs again on the release wake, and a coast that cannot start now
// must not be retried every millisecond.
//
// A release that is held (IdleReleaseHeld) contributes nothing: its hold ends on
// a cloak event or an executor answer, and a deadline that cannot act would spin
// the thread at the timer's smallest delay.
// ---------------------------------------------------------------------------
uint64_t NextInputDeadlineMs(uint64_t nowTick, uint64_t nowQpc) {
    uint64_t soonest = 0;
    const auto consider = [&soonest](uint64_t delay) {
        if (delay != 0 && (soonest == 0 || delay < soonest)) {
            soonest = delay;
        }
    };
    const auto considerTick = [&](uint64_t due) {
        consider(due > nowTick ? due - nowTick : 1);
    };
    const auto considerQpc = [&](uint64_t due) {
        consider(due > nowQpc ? due - nowQpc : 1);
    };
    const auto considerQpcAhead = [&](uint64_t due) {
        if (due > nowQpc) {
            consider(due - nowQpc);
        }
    };

    // The root-hook bootstrap. The module and the symbol resolve from this
    // thread, so until the answer is ready or terminally refused the loop has to
    // come back; the probe itself is throttled to the same interval. This is the
    // only deadline a healthy activation has before its first publication.
    if (!g_rootHookReady.load(std::memory_order_acquire) &&
        !g_rootHookRefused.load(std::memory_order_acquire)) {
        consider(kRootProbeMs);
    }

    const bool coasting = g_panel.coasting.load(std::memory_order_relaxed);
    // The coast's own end: when no report and no release stopped it, this is the
    // wake that pins the value it reached.
    if (coasting) {
        considerQpc(g_panel.coastStartQpc +
                    static_cast<uint64_t>(kCoastDuration.count()));
    }

    if (!IdleReleaseHeld(nowTick)) {
        // The coast starts 8 ms after the last point, so the panel is already
        // moving when the release is planned 10 ms later. The release itself is
        // the 18 ms silence, and the candidate's watchdog is its longer window.
        if (!coasting) {
            if (g_hidSession.active) {
                considerQpcAhead(g_hidSession.lastSampleQpc + kCoastStartMs);
            }
            if (g_reveal.active) {
                // The earliest time MaybeCoast can actually start a follow
                // coast: its report gap *and* the follow's first 50 ms, which
                // its settle guard rejects. Scheduling the gap alone armed a
                // wake the guard refused, and a silent stream never tried the
                // coast again.
                considerQpcAhead(std::max(
                    g_reveal.lastSampleQpc + kCoastStartMs,
                    g_reveal.activeSinceQpc + kRevealCoastSettleMs));
            }
        }
        if (g_hidSession.active) {
            considerQpc(g_hidSession.lastAliveQpc + kIdleReleaseMs);
        }
        if (g_reveal.edgeCandidate) {
            considerQpc(g_reveal.lastAliveQpc + kCandidateIdleHoldMs);
        }
        // The latch and the identity clocks expire on the same 18 ms silence as
        // a session, and only while no session owns the contact.
        if (!g_hidSession.active && !g_reveal.edgeCandidate) {
            if (g_contactDown) {
                considerTick(g_contactTick + kIdleReleaseMs);
            }
            if (g_trackedContact.slot >= 0) {
                considerTick(g_trackedContact.lastEvidenceTick + kIdleReleaseMs);
            }
        }
    }

    // The close tap's deadline: the end of the settle it was armed for.
    if (g_pendingTapTick != 0) {
        considerTick(g_pendingTapTick);
    }
    // The executor's budget. A show expires at its own deadline; a dismiss is
    // measured from the time it was scheduled to run, because the bridge's timer
    // fires there and the ack cannot arrive before that.
    const uint64_t showUntil = g_pendingShowUntil.load(std::memory_order_relaxed);
    if (showUntil != 0) {
        considerTick(showUntil);
    }
    const uint64_t dismissAt =
        g_pendingDismissExecAt.load(std::memory_order_relaxed);
    if (g_pendingDismissExec.load(std::memory_order_relaxed) != 0 &&
        dismissAt != 0) {
        considerTick(dismissAt + kBridgeAckTimeoutMs);
    }
    // A confirmation retained because the cloak disagreed or could not answer
    // must come back when its wait for the transition ends, even if no event and
    // no report arrives.
    if (g_pendingDismissConfirmRetained && g_pendingDismissConfirmUntil != 0) {
        considerTick(g_pendingDismissConfirmUntil);
    }
    // The settled measurement's own turn, and the scroller retry that replaces a
    // pass which could not commit. Both are posted to the UI thread; the deadline
    // here is the guaranteed path when the layout watch never delivers.
    const uint64_t measureDue = PeekMeasureDeadlineTick();
    if (measureDue != 0) {
        considerTick(measureDue);
    }
    const uint64_t scrollerRetry =
        g_scrollerRetryAt.load(std::memory_order_relaxed);
    if (scrollerRetry != 0) {
        considerTick(scrollerRetry);
    }

    // The net under the cloak event: only while a transition is being waited for
    // or has just landed. The cadence is slow because it only has to keep a
    // missed event from leaving the state stuck.
    if (g_visibilityPollUntil != 0 && nowTick < g_visibilityPollUntil) {
        const uint64_t due =
            (g_lastVisibilityPollTick != 0
                 ? g_lastVisibilityPollTick + kVisibilityFallbackMs
                 : nowTick + kVisibilityFallbackMs);
        considerTick(due < g_visibilityPollUntil ? due : g_visibilityPollUntil);
    }

    return soonest;
}

// One service pass for a due deadline. Each step is the same work the old 5 ms
// tick did; what changed is that nothing calls it while nothing is due.
void ServiceDeadlineWork(uint64_t tickMs) {
    RunPendingCloseTap(tickMs);
    CheckHidIdleRelease(tickMs);
    CheckExecAckTimeout(tickMs);
    if (g_visibilityPollUntil != 0) {
        if (tickMs >= g_visibilityPollUntil) {
            // The window closes: take one last sample before it does. The
            // callback may collapse two transitions into one wake, and a change
            // that landed after the last poll would otherwise be forgotten with
            // the watch. The watch is cleared first so the sample cannot renew
            // it; the state it reads is the current DWM answer, not an event's.
            g_visibilityPollUntil = 0;
            g_visibilityWant = kVisibilityWantNone;
            ServiceRootVisibility(tickMs, true);
        } else if (g_lastVisibilityPollTick == 0 ||
                   tickMs - g_lastVisibilityPollTick >= kVisibilityFallbackMs) {
            g_lastVisibilityPollTick = tickMs;
            ServiceRootVisibility(tickMs, true);
        }
    }
}

// A report that names the tracked contact but carries no point is still that
// contact being down when the report asserts it. It refreshes the liveness clock
// the idle window reads, so 18 ms of coordinate-less reports never release a
// finger that is still on the glass and never let the same finger start a second
// gesture. The position clock is not touched, because the coast measures its own
// silence from the last point and a status report is not one. No state-machine
// step runs: there is no position to feed and the panel must not move.
void NoteContactAlive(uint64_t tickMs) {
    g_contactDown = true;
    g_contactTick = tickMs;
    const uint64_t qpc = QpcMsNow();
    if (g_hidSession.active) {
        g_hidSession.lastTick = tickMs;
        g_hidSession.lastAliveQpc = qpc;
    }
    if (g_reveal.edgeCandidate) {
        g_reveal.lastTick = tickMs;
        g_reveal.lastAliveQpc = qpc;
    }
}

// Maps one decoded contact point to screen DIP and feeds it to the state
// machine: one call per report that names a real contact at a position. The
// collection is the one the point was read from, so the axis ranges are the ones
// that collection's caps declare.
void FeedContactPoint(HidDevice& device, long rawX, long rawY, USHORT reportId,
                      USHORT collection, uint64_t tickMs) {
    // A positioned report is the moment this thread is known to be doing the
    // work an intent would feed; refresh the bridge's liveness clock here as
    // well as on the idle tick.
    PublishPeerTick(tickMs);

    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (screenWidth <= 1 || screenHeight <= 1) {
        LogDecodeFail(device, 0);
        return;
    }

    const HidAxisRange xRange =
        FindAxisRange(device, kUsageX, reportId, collection);
    const HidAxisRange yRange =
        FindAxisRange(device, kUsageY, reportId, collection);
    const float dpiScale = HidDpiScale();

    const int32_t pixelX = ToScreenPixel(rawX, xRange, screenWidth);
    const int32_t pixelY = ToScreenPixel(rawY, yRange, screenHeight);
    const float xDip = static_cast<float>(pixelX) / dpiScale;
    const float yDip = static_cast<float>(pixelY) / dpiScale;

    // The right and bottom edge bands are measured from the screen, which this
    // thread only knows in pixels; publish it in the same DIP space as xDip.
    g_screenDipWidth = static_cast<float>(screenWidth) / dpiScale;
    g_screenDipHeight = static_cast<float>(screenHeight) / dpiScale;

    // Diagnostic: does the digitizer see a touch that starts on the screen edge
    // before the menu is open? Off in the shipping build; it measured the edge
    // contact and EP's own open threshold.
    if (g_debugEdge && !g_hidSession.active && xDip < 200.0f &&
        tickMs > g_edgeLogTick + 100) {
        g_edgeLogTick = tickMs;
        TraceLog(L"edge x=%.0f y=%.0f open=%d", xDip, yDip,
                StartWindowIsOpen() ? 1 : 0);
    }

    OnHidSample(1, xDip, yDip, tickMs);
    // Where this contact was last seen. Only the tie between two real contacts
    // in one report reads it, and it is written after the state machine, so a
    // session that starts on this report cannot reset it.
    g_hidSession.lastRawX = rawX;
    g_hidSession.lastRawY = rawY;
}

void FeedHidReport(HidDevice& device, const BYTE* report,
                   ULONG reportLength, uint64_t tickMs) {
    if (!device.ready || !device.preparsed) {
        LogDecodeFail(device, 0);
        return;
    }

    // HID puts the report ID in the first byte of a report from a device that
    // declares report IDs. RAWINPUTHEADER.wParam is not it: that is RIM_INPUT
    // (0) or RIM_INPUTSINK (1), so using it here would either defeat the
    // report-ID filter or select the wrong value cap.
    const USHORT reportId =
        device.hasReportIds && reportLength > 0 ? report[0] : 0;

    USHORT collections[kMaxContactCollections] = {};
    int slotCount = 0;
    if (device.contactNodeCount > 0) {
        slotCount = device.contactNodeCount;
        for (int i = 0; i < slotCount; ++i) {
            collections[i] = device.contactNodes[i];
        }
    } else {
        slotCount = 1;
        collections[0] = 0;
    }

    // Contact Count is read for two reasons: a report that carries only it still
    // speaks about contact state (it is not an unrelated report ID), and on a
    // count/position device it is the number of fingers. It never speaks for a
    // tracked contact whose own slot this report does not carry.
    ULONG topLevelCount = 0;
    const HidField countField =
        device.hasContactCount
            ? ReadUsageValue(device, kUsagePageDigitizer,
                             device.contactCountLink, kUsageContactCount,
                             report, reportLength, &topLevelCount)
            : HidField::Absent;
    if (countField == HidField::Error) {
        LogDecodeFail(device, 0);
        return;
    }

    // Every candidate slot is decoded once, before identity is decided. The
    // tracked contact may be named by an identifier in any slot (a node index is
    // only where it was last seen), and a report that carries a tip-down or an
    // in-range assertion without a point still proves the contact is alive; the
    // identifier alone does not.
    HidContactSlot slots[kMaxContactCollections];
    bool anyDeclared = countField == HidField::Ok;
    for (int i = 0; i < slotCount; ++i) {
        NTSTATUS error = HIDP_STATUS_SUCCESS;
        if (!DecodeContactSlot(device, collections[i], reportId, report,
                               reportLength, slots[i], error)) {
            LogDecodeFail(device, static_cast<unsigned>(error));
            return;
        }
        anyDeclared = anyDeclared || slots[i].declared;
    }

    // Identity first. A tracked contact is moved or ended only by the contact the
    // report names: its identifier when the device declares one, its node index
    // otherwise. Another slot's coordinates, another slot's tip-up or another
    // device cannot speak for it. A report whose ID carries none of the contact
    // fields finds nothing declared here and is dropped, instead of feeding a
    // zero that would synthesize a lift.
    if (g_trackedContact.slot >= 0) {
        if (g_trackedContact.device != device.handle) {
            return;
        }

        int trackedSlot = -1;
        if (g_trackedContact.hasIdentifier) {
            // The identifier decides: a slot that now carries a different one is
            // another contact, even at the index the tracked contact last used.
            // An unused slot can keep or zero its identifier, so a slot that
            // positively asserts contact wins over one that merely carries the
            // name; when two assert it, the tracked slot is the one taken and the
            // ambiguity is logged once.
            int firstMatch = -1;
            int trackedMatch = -1;
            int assertedMatch = -1;
            int assertedCount = 0;
            for (int i = 0; i < slotCount; ++i) {
                if (!slots[i].hasIdentifier ||
                    slots[i].identifier != g_trackedContact.identifier) {
                    continue;
                }
                if (firstMatch < 0) {
                    firstMatch = i;
                }
                if (i == g_trackedContact.slot) {
                    trackedMatch = i;
                }
                if (SlotContactState(slots[i]) == HidContactState::Contact) {
                    assertedCount++;
                    if (assertedMatch < 0 || i == g_trackedContact.slot) {
                        assertedMatch = i;
                    }
                }
            }
            if (assertedMatch >= 0) {
                trackedSlot = assertedMatch;
                if (assertedCount > 1 && !g_duplicateContactIdLogged) {
                    g_duplicateContactIdLogged = true;
                    TraceLog(L"hid contact id=%lu declared down in %d slots; "
                            L"using=%d",
                            g_trackedContact.identifier, assertedCount,
                            trackedSlot);
                }
            } else {
                // Nothing asserts contact: the tracked slot is the best evidence
                // left, else the first slot that carries the name.
                trackedSlot = trackedMatch >= 0 ? trackedMatch : firstMatch;
            }
        }
        if (trackedSlot < 0 && g_trackedContact.slot < slotCount &&
            slots[g_trackedContact.slot].declared) {
            // No identifier names the contact in this report: the node index it
            // was last seen in is the only name left. A slot that does declare a
            // different identifier is another contact, not a silent return of
            // this one.
            const HidContactSlot& candidate = slots[g_trackedContact.slot];
            const bool namesAnother =
                g_trackedContact.hasIdentifier && candidate.hasIdentifier &&
                candidate.identifier != g_trackedContact.identifier;
            if (!namesAnother) {
                trackedSlot = g_trackedContact.slot;
            }
        }
        if (trackedSlot < 0) {
            if (g_trackedContact.slot >= slotCount) {
                // The device table recycled this handle into a device with fewer
                // contact nodes: the index the contact was last seen on cannot
                // name anything in this report any more.
                ForgetTrackedContact();
            }
            // This report says nothing about the tracked contact. It is not a
            // lift: only the contact's own declared tip-up or in-range clear is,
            // and the idle window ends a contact that really went away.
            return;
        }

        const HidContactSlot& slot = slots[trackedSlot];
        // A name is not evidence. This report names the tracked contact, but it
        // may carry neither a point nor an asserted tip, and such a report must
        // not hold the tracker: after a refused admission the latch is clear, so
        // no lift and no latch cleanup could ever release this identity.
        // `lastEvidenceTick` therefore moves only where the report proves the
        // contact — an asserting tip or in-range bit below, or a point this path
        // feeds — and a name-only report updates the slot and the name
        // (diagnostics, and the identity the expiry line prints) and nothing
        // else.
        // A contact the report names is named by that name from here on, so a
        // later slot reassignment cannot transfer the gesture.
        if (slot.hasIdentifier) {
            g_trackedContact.hasIdentifier = true;
            g_trackedContact.identifier = slot.identifier;
        }
        g_trackedContact.slot = trackedSlot;

        const HidContactState state = SlotContactState(slot);
        if (state == HidContactState::NotContact) {
            // The tracked contact ended: its declared tip went up (a pen lift) or
            // its declared in-range bit cleared. No other slot in this report
            // restarts it. `Unknown` never ends it: a report that carries no
            // contact field for the slot cannot speak for the contact.
            TraceLog(L"hid tracked notcontact field=%d tip=%d tipdown=%d range=%d "
                    L"rangedown=%d id=%d count=%d coord=%d report=%u pos=%d",
                    static_cast<int>(slot.buttons.field),
                    slot.buttons.tipDeclared ? 1 : 0,
                    slot.buttons.tipDown ? 1 : 0,
                    slot.buttons.inRangeDeclared ? 1 : 0,
                    slot.buttons.inRangeDown ? 1 : 0,
                    slot.hasIdentifier ? 1 : 0,
                    countField == HidField::Ok ? topLevelCount : -1,
                    device.coordinatePath ? 1 : 0, reportId,
                    slot.hasPosition ? 1 : 0);
            ForgetTrackedContact();
            OnHidSample(0, 0.0f, 0.0f, tickMs);
            return;
        }
        // A coordinate-only device cannot assert its contact, so the old
        // fallback's lift rule decides here too, even after the contact is
        // tracked: Contact Count 0 is the lift (the position fields often keep
        // their last value), and a zero pair is the no-position report its lift
        // looks like, not a point at the screen corner.
        if (device.coordinatePath && countField == HidField::Ok &&
            topLevelCount == 0) {
            ForgetTrackedContact();
            OnHidSample(0, 0.0f, 0.0f, tickMs);
            return;
        }
        if (!slot.hasPosition) {
            // A contact that is asserted but carries no point in this report is
            // still down. It refreshes the liveness clock without feeding a
            // position: silence used to end a contact that was being asserted,
            // and the same finger could then start a second gesture. The
            // identifier alone is not evidence: a device whose unused slots keep
            // or zero their ids would keep a stale finger alive and every other
            // device out.
            if (state == HidContactState::Contact) {
                g_trackedContact.lastEvidenceTick = tickMs;  // asserted, not named
                NoteContactAlive(tickMs);
            }
            return;
        }
        if (device.coordinatePath && slot.atOrigin) {
            // A zero pair on the count/position path is its no-position report:
            // there is no point to feed and therefore none to count as evidence.
            return;
        }
        // A point this path is about to feed is the other half of the evidence
        // rule; it is written before the feed, so a feed that ends the contact
        // cannot leave a clock behind on a forgotten identity.
        g_trackedContact.lastEvidenceTick = tickMs;
        FeedContactPoint(device, slot.rawX, slot.rawY, reportId,
                         collections[trackedSlot], tickMs);
        return;
    }

    if (!anyDeclared) {
        return;  // unrelated report: no contact field, no sample
    }

    if (device.coordinatePath) {
        // The device declares neither Tip Switch nor In Range, so a position is
        // its only contact evidence. This is the path this mod shipped with and
        // the only per-device fallback left. Here the old (0,0) heuristic stays:
        // with nothing asserting the tip, a zero pair is how a lift report
        // usually looks.
        int nonZero = 0;
        int chosen = -1;
        long chosenX = 0;
        long chosenY = 0;
        long bestDistance = 0;
        for (int i = 0; i < slotCount; ++i) {
            if (!slots[i].hasPosition || slots[i].atOrigin) {
                continue;
            }
            nonZero++;
            if (!g_hidSession.active) {
                if (chosen < 0) {
                    chosen = i;
                    chosenX = slots[i].rawX;
                    chosenY = slots[i].rawY;
                }
                continue;
            }
            const long dx = slots[i].rawX - g_hidSession.lastRawX;
            const long dy = slots[i].rawY - g_hidSession.lastRawY;
            const long distance = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
            if (chosen < 0 || distance < bestDistance) {
                chosen = i;
                chosenX = slots[i].rawX;
                chosenY = slots[i].rawY;
                bestDistance = distance;
            }
        }

        const int contacts = countField == HidField::Ok
                                 ? static_cast<int>(topLevelCount)
                                 : nonZero;
        if (countField == HidField::Ok && contacts == 0) {
            // Contact Count is authoritative: on a lift report the position
            // fields often keep their last value, so trusting them would hide
            // the lift and force the idle timer to guess.
            OnHidSample(0, 0.0f, 0.0f, tickMs);
            return;
        }
        if (chosen < 0) {
            // No positioned collection: with no count to trust this is a lift;
            // with a count it means the position fields were not decoded.
            if (contacts <= 0) {
                OnHidSample(0, 0.0f, 0.0f, tickMs);
            } else {
                LogDecodeFail(device, 0);
            }
            return;
        }
        // This fallback has only positions, so the contact is named by its node
        // index: an identifier with nothing asserting the contact would name a
        // stale one.
        g_trackedContact.device = device.handle;
        g_trackedContact.slot = chosen;
        g_trackedContact.hasIdentifier = false;
        g_trackedContact.identifier = 0;
        g_trackedContact.lastEvidenceTick = tickMs;
        FeedContactPoint(device, chosenX, chosenY, reportId, collections[chosen],
                         tickMs);
        return;
    }

    // A button device: the report's real contacts are the candidates, so a slot
    // whose tip is up with stale coordinates can neither win the choice nor
    // restart the tracked finger. The slot nearest the last tracked raw position
    // breaks a tie between two real contacts.
    int realContacts = 0;
    int chosen = -1;
    long bestDistance = 0;
    for (int i = 0; i < slotCount; ++i) {
        if (SlotContactState(slots[i]) != HidContactState::Contact) {
            continue;
        }
        realContacts++;
        if (!slots[i].hasPosition) {
            continue;
        }
        const long dx = slots[i].rawX - g_hidSession.lastRawX;
        const long dy = slots[i].rawY - g_hidSession.lastRawY;
        const long distance = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
        if (chosen < 0 || distance < bestDistance) {
            chosen = i;
            bestDistance = distance;
        }
    }

    if (chosen < 0) {
        if (realContacts > 0) {
            // A real contact whose point has not arrived yet; the report that
            // carries one starts it.
            return;
        }
        if (countField == HidField::Ok && topLevelCount > 0) {
            // Contacts are down, but this report names none of them with a
            // position. Feeding a lift here would end a contact it does not
            // speak for.
            return;
        }
        // No contact in this report: this is the lift, and doing nothing would
        // leave the gesture to the 18 ms idle timer.
        OnHidSample(0, 0.0f, 0.0f, tickMs);
        return;
    }

    g_trackedContact.device = device.handle;
    g_trackedContact.slot = chosen;
    g_trackedContact.hasIdentifier = slots[chosen].hasIdentifier;
    g_trackedContact.identifier = slots[chosen].identifier;
    g_trackedContact.lastEvidenceTick = tickMs;
    FeedContactPoint(device, slots[chosen].rawX, slots[chosen].rawY, reportId,
                     collections[chosen], tickMs);
}

void HandleHidInput(const RAWINPUT* raw) {
    const RAWHID& hid = raw->data.hid;
    if (hid.dwSizeHid == 0 || hid.dwCount == 0) {
        return;
    }

    HidDevice* device = GetHidDevice(raw->header.hDevice);
    if (!device) {
        return;
    }

    const uint64_t tickMs = GetTickCount64();
    for (UINT i = 0; i < hid.dwCount; ++i) {
        const BYTE* report =
            hid.bRawData + static_cast<size_t>(i) * hid.dwSizeHid;
        FeedHidReport(*device, report, hid.dwSizeHid, tickMs);
    }
}

void HandleRawInput(HRAWINPUT rawHandle) {
    BYTE stackBuffer[512];
    UINT size = sizeof(stackBuffer);
    SetLastError(0);
    UINT read = GetRawInputData(rawHandle, RID_INPUT, stackBuffer, &size,
                                sizeof(RAWINPUTHEADER));
    BYTE* heapBuffer = nullptr;

    if (read == static_cast<UINT>(-1)) {
        if (size <= sizeof(stackBuffer) || size > 64 * 1024) {
            return;
        }
        heapBuffer = new (std::nothrow) BYTE[size];
        if (!heapBuffer) {
            return;
        }
        UINT heapSize = size;
        read = GetRawInputData(rawHandle, RID_INPUT, heapBuffer, &heapSize,
                               sizeof(RAWINPUTHEADER));
        if (read == static_cast<UINT>(-1)) {
            delete[] heapBuffer;
            return;
        }
    }

    const auto* raw = reinterpret_cast<const RAWINPUT*>(
        heapBuffer ? heapBuffer : stackBuffer);
    if (raw->header.dwType == RIM_TYPEHID) {
        HandleHidInput(raw);
    }

    delete[] heapBuffer;
}

LRESULT CALLBACK HidWindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_INPUT:
            HandleRawInput(reinterpret_cast<HRAWINPUT>(lParam));
            return 0;
        case WM_TIMER:
            // The fallback tick, armed by the wait planner only when the
            // high-resolution timer could not be created and only while a
            // deadline is outstanding. It runs the same pass the timer does.
            if (wParam == kIdleTimerId) {
                ServiceDeadlineWork(GetTickCount64());
            }
            return 0;
        // The taskbar moved or the display layout changed: the dismiss axis and
        // the translation baked into the composition expression are stale, and
        // the panel rect was measured against the old work area. This is the
        // event that replaces any polling for both. The message arrives on this
        // thread; the measurement is posted to the UI thread, which owns the XAML
        // root and the property set.
        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETWORKAREA) {
                TraceLog(L"work area changed");
                PostGeometryMeasurement(
                    StartMeasureRequest(!g_hidSession.active &&
                                        !g_reveal.edgeCandidate),
                    false);
            }
            return 0;
        case WM_DISPLAYCHANGE:
            TraceLog(L"work area changed");
            PostGeometryMeasurement(
                StartMeasureRequest(!g_hidSession.active &&
                                    !g_reveal.edgeCandidate),
                false);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case kInputWakeMessage:
            // A cross-thread deadline was published (the scroller retry). The
            // loop re-plans after this batch; the message itself carries nothing.
            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

void RegisterHidUsage(USAGE usage, HWND hwnd) {
    RAWINPUTDEVICE device{};
    device.usUsagePage = kUsagePageDigitizer;
    device.usUsage = usage;
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = hwnd;

    SetLastError(0);
    const BOOL ok = RegisterRawInputDevices(&device, 1, sizeof(device));
    const DWORD error = ok ? 0 : GetLastError();
    if (usage == kUsageTouchScreen) {
        g_touchRegistered = ok != FALSE;
    } else if (usage == kUsagePen) {
        g_penRegistered = ok != FALSE;
    }
    TraceLog(L"hid reg usage=%u ok=%d err=%u", static_cast<unsigned>(usage),
            ok ? 1 : 0, static_cast<unsigned>(error));
}

void UnregisterHidUsage(USAGE usage) {
    RAWINPUTDEVICE device{};
    device.usUsagePage = kUsagePageDigitizer;
    device.usUsage = usage;
    device.dwFlags = RIDEV_REMOVE;
    device.hwndTarget = nullptr;
    RegisterRawInputDevices(&device, 1, sizeof(device));
}

// Defined with the root-hook installer. Runs on the input thread only, because
// the symbol load blocks for Windhawk's PDB fetch and must never block the
// shell's loader thread.
void ProbeRootSymbolFromInputThread();

// Unregisters the fixed raw-input class under the instance that registered it.
// The class belongs to this image, so a class left registered would leave a
// window procedure pointing into an unloaded module. That is the same
// discipline as the executor's fixed class: `ERROR_CLASS_ALREADY_EXISTS` is a
// failure, never adopted, and a failed unregister keeps the unload trying
// instead of returning success into an image the class still points at.
extern "C" IMAGE_DOS_HEADER __ImageBase;
void UnregisterHidWindowClass(HINSTANCE instance) {
    for (;;) {
        if (UnregisterClassW(kHidWindowClass, instance)) {
            TraceLog(L"hid window class unregistered");
            return;
        }
        const DWORD error = GetLastError();
        if (error == ERROR_CLASS_DOES_NOT_EXIST) {
            TraceLog(L"hid window class already gone");
            return;
        }
        TraceLog(L"hid window class unregister failed err=%u", error);
        Sleep(1000);
    }
}

DWORD WINAPI HidInputThread(LPVOID) {
    g_hidThreadId.store(GetCurrentThreadId(), std::memory_order_relaxed);
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = HidWindowProc;
    // Fixed name, owned by this image. The host's instance would make the class
    // outlive the mod, and its procedure would point into the unloaded image.
    windowClass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    windowClass.lpszClassName = kHidWindowClass;

    if (!RegisterClassExW(&windowClass)) {
        // ERROR_CLASS_ALREADY_EXISTS included: a previous window of this image
        // survived its unload, and adopting that class would hand this
        // activation's raw input to a procedure that is about to be unmapped.
        TraceLog(L"hid window class failed err=%u", GetLastError());
        return 0;
    }

    // A hidden top-level window, not a message-only one: message-only windows
    // are documented not to receive broadcast messages, and WM_SETTINGCHANGE
    // (SPI_SETWORKAREA) and WM_DISPLAYCHANGE are broadcasts. That is the whole
    // event-driven axis refresh. The window is never shown, has no size and no
    // extended style, so it does not appear in Alt-Tab or the taskbar; Raw Input
    // with RIDEV_INPUTSINK targets it exactly as it did a message-only window.
    HWND hwnd = CreateWindowExW(0, kHidWindowClass, L"", 0, 0, 0, 0, 0, nullptr,
                                nullptr, windowClass.hInstance, nullptr);
    if (!hwnd) {
        TraceLog(L"hid window create failed err=%u",
                static_cast<unsigned>(GetLastError()));
        UnregisterHidWindowClass(windowClass.hInstance);
        return 0;
    }

    g_hidHwnd.store(hwnd, std::memory_order_relaxed);

    RegisterHidUsage(kUsageTouchScreen, hwnd);
    RegisterHidUsage(kUsagePen, hwnd);

    // The deadline timer: a high-resolution waitable timer, because WM_TIMER
    // stays on the 15.6 ms tick on this build even while a 1 ms period is held.
    // It is never armed here; the wait planner below arms it for the one deadline
    // the thread has and disarms it when that is served. The WM_TIMER tick stays
    // as the fallback for a host that cannot create the timer.
    HANDLE deadlineTimer = CreateDeadlineTimer();
    if (!deadlineTimer) {
        TraceLog(L"hid timer fallback err=%u",
                static_cast<unsigned>(GetLastError()));
    }

    // The cloak signal. The event is created and published before the hook is
    // installed, so no delivery can find a null handle; both are destroyed on
    // this thread (the hook is unhooked first, so no callback can outlive the
    // event).
    HANDLE cloakEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!cloakEvent) {
        TraceLog(L"hid cloak event failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }
    g_cloakEvent.store(cloakEvent, std::memory_order_relaxed);
    if (cloakEvent) {
        g_cloakHook = SetWinEventHook(
            EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
            CloakWinEventProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
        if (!g_cloakHook) {
            TraceLog(L"hid cloak hook failed err=%u",
                    static_cast<unsigned>(GetLastError()));
        }
    }

    // Object 0 is the stop event, then the deadline timer and the cloak event,
    // then the intent, exec-ack and renderer events, then the message queue. The
    // stop event does not depend on the window: the unload sets it even if this
    // thread has not published the HWND yet, and it is checked again after every
    // message batch, because a queued message can win the wait over a
    // just-signalled event.
    HANDLE waits[8] = {};
    DWORD waitCount = 0;
    if (g_hidStopEvent) {
        waits[waitCount++] = g_hidStopEvent;
    }
    if (deadlineTimer) {
        waits[waitCount++] = deadlineTimer;
    }
    if (g_ipc.intentEvent) {
        waits[waitCount++] = g_ipc.intentEvent;
    }
    if (g_ipc.execAckEvent) {
        waits[waitCount++] = g_ipc.execAckEvent;
    }
    if (g_rendererEvent) {
        waits[waitCount++] = g_rendererEvent;
    }
    if (cloakEvent) {
        waits[waitCount++] = cloakEvent;
    }

    MSG msg;
    bool running = true;
    // Whether the one-shot is currently armed, and whether the WM_TIMER fallback
    // is. Both exist so a plan that does not change the wake state costs no
    // syscall: a report-heavy gesture would otherwise cancel and re-arm on every
    // iteration.
    bool timerArmed = false;
    bool fallbackArmed = false;
    DWORD fallbackPeriod = 0;
    bool timerFailLogged = false;
    // The consumer is ready here: the window exists, both raw-input usages have
    // been registered (a refused registration is logged and leaves the intent
    // half of this thread working, so it does not withdraw availability), the
    // timer and the event set are built, and this thread is about to wait on the
    // intents. Availability is published from this point and never from init: an
    // `alive` written earlier promises a service that does not exist yet, and the
    // bridge would swallow a press for a consumer that has not begun to serve.
    //
    // The root hook and an attached root are the second condition. The hook is
    // normally installed before this thread exists; a StartUI_ that loads later
    // is picked up by the LoadLibraryExW notification, and this thread publishes
    // once Setup attaches the root — still from the consumer itself, never from
    // the loading thread.
    bool peerAnnounced = false;
    bool rendererWaitLogged = false;
    // Whether this activation has taken its first visibility sample since the
    // peer service became available. The state is a sample, not an event's
    // payload: without the seed, a menu that was already on screen at attach
    // would be read as "closed" until some later event.
    bool visibilitySeeded = false;
    // The root hook is resolved here and not by the LoadLibraryExW notification:
    // the first symbol load blocks for Windhawk's PDB fetch, and this thread is
    // the mod's own. A pending result leaves the peer unpublished.
    ProbeRootSymbolFromInputThread();
    if (g_rootHookReady.load(std::memory_order_acquire) && RendererReady()) {
        peerAnnounced = PublishPeerReady(L"consumer");
    }
    while (running) {
        // The module appears after this thread starts in the measured host, so
        // the bootstrap deadline (NextInputDeadlineMs) is what brings this thread
        // back to look. The call is throttled while the result is pending and
        // stops as soon as ready or refused is set.
        ProbeRootSymbolFromInputThread();
        // Readiness is a continuing capability, not a one-time announcement. A
        // root that is torn down, replaced or invalidated by a failed scalar
        // write must withdraw the peer first; the bridge then passes the press
        // natively instead of swallowing it for a controller that cannot animate
        // it. The renderer event is the path that wakes this loop for the
        // transition; these tests are what turn the wake into a publication.
        if (peerAnnounced && !RendererReady()) {
            SetPeerGone(L"renderer-lost", false);
            peerAnnounced = false;
            visibilitySeeded = false;
        }
        if (!peerAnnounced &&
            g_rootHookReady.load(std::memory_order_acquire) && RendererReady()) {
            peerAnnounced = PublishPeerReady(L"consumer");
        }
        if (!peerAnnounced && !rendererWaitLogged &&
            g_rootHookReady.load(std::memory_order_acquire) &&
            !RendererReady()) {
            // One line per activation: the hook is ready, the root is not. The
            // service waits for Setup; a session that never attaches advertises
            // nothing and every press stays native.
            rendererWaitLogged = true;
            TraceLog(L"ipc peer withheld reason=no-renderer");
        }
        if (peerAnnounced && !visibilitySeeded &&
            ServiceRootVisibility(GetTickCount64(), false)) {
            // First sample once this activation can serve: the state is the
            // current DWM answer, so a menu already on screen is adopted and a
            // closed one seeds "closed" instead of leaving a stale default that
            // makes the next close look like the first transition. The same
            // sample runs again after a renderer replacement, because the window
            // may have changed while this side could not drive it. A failed read
            // returns false and the next heartbeat retries the seed.
            visibilitySeeded = true;
        }

        // Arm the one thing this thread has to do on a clock, or nothing. The
        // deadline is recomputed after every wake, so a served deadline disappears
        // from the plan and its arm is undone here, not in each handler.
        const uint64_t nowTick = GetTickCount64();
        const uint64_t deadlineMs = NextInputDeadlineMs(nowTick, QpcMsNow());
        if (deadlineTimer && deadlineMs != 0) {
            const bool armed = ArmDeadlineTimer(deadlineTimer, deadlineMs);
            if (!armed && !timerFailLogged) {
                timerFailLogged = true;
                TraceLog(L"hid timer arm failed err=%u",
                        static_cast<unsigned>(GetLastError()));
            }
            timerArmed = armed;
        } else if (timerArmed) {
            DisarmDeadlineTimer(deadlineTimer);
            timerArmed = false;
        }
        // The message timer is the fallback whenever the one-shot is not armed —
        // a missing handle and a failed SetWaitableTimer are the same condition.
        // It cannot go faster than the system quantum, so a sub-quantum deadline
        // arrives late (the release then lands at 31-34 ms) but it still arrives;
        // before this, a failed arm left no fallback at all and a bootstrap
        // deadline could wait forever.
        if (!timerArmed && deadlineMs != 0) {
            const DWORD period = static_cast<DWORD>(
                deadlineMs < 0x7FFFFFFF ? deadlineMs : 0x7FFFFFFF);
            // Re-arm only when the period changes: SetTimer with the same id
            // restarts the timer, and a report stream faster than the period
            // would keep pushing it out.
            if (!fallbackArmed || fallbackPeriod != period) {
                fallbackArmed =
                    SetTimer(hwnd, kIdleTimerId, period, nullptr) != 0;
                fallbackPeriod = fallbackArmed ? period : 0;
            }
        } else if (fallbackArmed) {
            KillTimer(hwnd, kIdleTimerId);
            fallbackArmed = false;
            fallbackPeriod = 0;
        }

        // Nothing armed for the deadline means nothing will wake this thread at
        // it: no event, no timer. That includes the root bootstrap, which has
        // neither a peer heartbeat nor a renderer wait yet. Bound the wait by the
        // deadline itself; a successfully armed timer keeps the pure event wait.
        DWORD waitMs = (g_peerPublished.load(std::memory_order_relaxed) ||
                        (!peerAnnounced &&
                         g_rootHookReady.load(std::memory_order_acquire) &&
                         !RendererReady()))
                           ? static_cast<DWORD>(kPeerHeartbeatMs)
                           : INFINITE;
        if (deadlineMs != 0 && !timerArmed) {
            const DWORD due = static_cast<DWORD>(
                deadlineMs < 0x7FFFFFFF ? deadlineMs : 0x7FFFFFFF);
            if (waitMs == INFINITE || due < waitMs) {
                waitMs = due;
            }
        }
        const DWORD wait =
            MsgWaitForMultipleObjects(waitCount, waits, FALSE, waitMs,
                                      QS_ALLINPUT);
        if (wait == WAIT_TIMEOUT) {
            if (g_hidStopEvent &&
                WaitForSingleObject(g_hidStopEvent, 0) == WAIT_OBJECT_0) {
                running = false;
                continue;
            }
            // The heartbeat itself: the pass publishes the tick and serves any
            // deadline that came due inside the timeout.
            ServiceDeadlineWork(GetTickCount64());
            continue;
        }
        if (wait == WAIT_OBJECT_0 + waitCount) {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    running = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (running && g_hidStopEvent &&
                WaitForSingleObject(g_hidStopEvent, 0) == WAIT_OBJECT_0) {
                running = false;
            }
            continue;
        }
        if (wait >= WAIT_OBJECT_0 && wait < WAIT_OBJECT_0 + waitCount) {
            const HANDLE signalled = waits[wait - WAIT_OBJECT_0];
            if (signalled == g_hidStopEvent) {
                running = false;
                continue;
            }
            if (signalled == deadlineTimer) {
                ServiceDeadlineWork(GetTickCount64());
                continue;
            }
            if (signalled == cloakEvent) {
                // The event itself is the authority on the transition: one
                // attribute read, then the same service the poll used to run.
                ServiceRootVisibility(GetTickCount64(), false);
                continue;
            }
            if (signalled == g_ipc.intentEvent) {
                ApplyBridgeIntent();
                continue;
            }
            if (signalled == g_ipc.execAckEvent) {
                ApplyExecAck();
                continue;
            }
            if (signalled == g_rendererEvent) {
                // The renderer just became usable or unusable. Withdraw before
                // any interception can happen; republish only when the replacement
                // is fully set up (RendererReady covers the invalidation bit).
                if (peerAnnounced && !RendererReady()) {
                    SetPeerGone(L"renderer-lost", false);
                    peerAnnounced = false;
                    visibilitySeeded = false;
                } else if (!peerAnnounced &&
                           g_rootHookReady.load(std::memory_order_acquire) &&
                           RendererReady()) {
                    peerAnnounced = PublishPeerReady(L"renderer");
                }
                continue;
            }
            continue;
        }
        break;  // WAIT_FAILED: leave the loop
    }
    if (running) {
        TraceLog(L"hid wait failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }

    // The hook first, then the event it signals: a callback that runs after the
    // handle is closed would set a recycled handle. Both belong to this thread,
    // and the unload has already joined it before anything else is freed.
    if (g_cloakHook) {
        UnhookWinEvent(g_cloakHook);
        g_cloakHook = nullptr;
        TraceLog(L"hid cloak hook removed");
    }
    g_cloakEvent.store(nullptr, std::memory_order_relaxed);
    if (cloakEvent) {
        CloseHandle(cloakEvent);
    }
    KillTimer(hwnd, kIdleTimerId);
    if (deadlineTimer) {
        CancelWaitableTimer(deadlineTimer);
        CloseHandle(deadlineTimer);
    }

    // Availability goes before anything this activation owns is reset, and it
    // goes on every exit: the stop event, WM_QUIT and WAIT_FAILED all land here.
    // The bridge must not read the last direction of a consumer that is gone,
    // which is also why the shared direction and touch are cleared here rather
    // than left for the next attach. It precedes the unregistrations too: this
    // thread is already leaving the intent loop, so the gate closes here, not
    // after the last device is detached.
    PublishPeerGone(L"consumer-exit");
    UnregisterHidUsage(kUsageTouchScreen);
    UnregisterHidUsage(kUsagePen);

    // One per-activation reset, the same function Wh_ModInit calls, so a clean
    // fresh image starts from the state the first activation
    // did. Nothing a gesture left behind survives: the tracked identity, the
    // latch, the sessions, the close mute, the deferred layout, the dismiss
    // slot, the coast and the debug switches.
    ResetActivationState();
    // No measurement of this thread's opens is left to run.
    CancelMeasureRequest();
    FreeHidDevices();

    // The 1 ms period is per-contact; a contact still down when the unload
    // arrives would otherwise leave the request held process-wide. This thread
    // owns the flag, so the release belongs here, before it exits.
    EndContactTiming();
    if (IsWindow(hwnd)) {
        DestroyWindow(hwnd);
    }
    g_hidHwnd.store(nullptr, std::memory_order_relaxed);
    UnregisterHidWindowClass(windowClass.hInstance);
    g_hidThreadId.store(0, std::memory_order_relaxed);
    return 0;
}

// ---------------------------------------------------------------------------
// UI-thread state publication
// ---------------------------------------------------------------------------

float DpiScaleForWindow(HWND hwnd) {
    const UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 0;
    return dpi ? static_cast<float>(dpi) / 96.0f : 1.0f;
}

// UI thread only. The single writer of the snapshot the input thread reads: one
// complete value in, one complete value out, under the short exclusive writer
// lock. The snapshot's identity token is minted here: the same identity keeps the
// token, a changed one raises it, so a reader can tell a copy from before a resize
// from the layout on screen now.
//
// No syscall and no XAML call sits in this function, so a commit can run it with
// the commit lock held.
void StoreGestureGeometry(const GestureGeometry& candidate) {
    AcquireSRWLockExclusive(&g_geometryLock);
    const bool sameLayout = SameLayoutIdentity(g_geometry.layout, candidate.layout);
    GestureGeometry published = candidate;
    if (sameLayout && g_geometry.layoutVersion != 0) {
        published.layoutVersion = g_geometry.layoutVersion;
    } else {
        // A RefreshGestureGeometry that committed a changed layout already
        // raised the token; otherwise (Setup, teardown) the publish itself is
        // the change and raises it here.
        uint32_t version = g_layoutVersion.load(std::memory_order_acquire);
        if (version <= g_geometry.layoutVersion) {
            version =
                g_layoutVersion.fetch_add(1, std::memory_order_acq_rel) + 1;
        }
        published.layoutVersion = version;
    }
    g_geometry = published;
    ReleaseSRWLockExclusive(&g_geometryLock);
}

// UI thread only. The window bounds the press hit test reads, queried once per
// publish. Kept out of the commit section: it is a window query, and the hit test
// reads it as its own atomics rather than as part of the layout identity.
void PublishWindowRectDip() {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return;
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) {
        return;
    }

    const float scale = DpiScaleForWindow(hwnd);
    g_winRectDip[0].store(
        static_cast<int64_t>(std::llround(rect.left / scale)),
        std::memory_order_relaxed);
    g_winRectDip[1].store(
        static_cast<int64_t>(std::llround(rect.top / scale)),
        std::memory_order_relaxed);
    g_winRectDip[2].store(
        static_cast<int64_t>(std::llround(rect.right / scale)),
        std::memory_order_relaxed);
    g_winRectDip[3].store(
        static_cast<int64_t>(std::llround(rect.bottom / scale)),
        std::memory_order_relaxed);
}

void PublishGestureGeometry(const GestureGeometry& candidate) {
    StoreGestureGeometry(candidate);
    PublishWindowRectDip();
}

// Maps the panel rectangle into absolute screen DIP. The visible panel is the
// largest visible child of the root that does not fill it (StartSizingFrame); the
// rest of the root canvas is transparent and must not be part of the reveal
// travel. The edge the reveal anchors to is derived from this rect by TrailEdge,
// so the rect and the edge cannot disagree. Runs on the UI thread and returns
// false while that child is not laid out yet.
//
// The screen origin of the XAML coordinate space, in DIP. The content is laid
// out in the host window's client area, and that area does not start at the
// work-area origin: measured on a bottom taskbar, the CoreWindow sits at x=447
// px while the work area starts at 0, so the panel's frame right of 1005 DIP is
// drawn at 1228.5 DIP, not 1005. The work area only matched on a left taskbar
// because the island was inset by exactly the taskbar width; the client origin
// holds for every axis and every layout. Both the panel rect and the cached
// scroller rect are mapped through it, so it lives in one place.
bool XamlOriginDip(float& originX, float& originY) {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return false;
    }

    POINT clientOrigin{0, 0};
    if (!ClientToScreen(hwnd, &clientOrigin)) {
        return false;
    }

    const float scale = DpiScaleForWindow(hwnd);
    if (!(scale > 0.0f)) {
        return false;
    }

    originX = static_cast<float>(clientOrigin.x) / scale;
    originY = static_cast<float>(clientOrigin.y) / scale;
    return true;
}

// The panel rect a measurement produced, before anything is published. Only a
// measurement that PanelRectAccepted sanctions becomes part of a snapshot.
struct PanelMeasurement {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
};

// A screen rectangle in DIP.
struct DisplayRectDip {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
};

// ---------------------------------------------------------------------------
// Which edge the taskbar is docked to
//
// The work area cannot answer this. With auto-hide enabled the shell reserves
// nothing, so rcWork equals rcMonitor on every side and the work-area inset is 0
// by design. Measured on this machine with the Windows 11 taskbar auto-hidden at
// the bottom: work=0,0,1440,900 mon=0,0,1440,900 in DIP, while the bar sits at
// the bottom. A zero inset is therefore not an edge signal, and the old fallback
// to the Start window's own rect could only ever answer Left or Top: that window
// is full screen when it is idle and slides across the screen while the shell
// animates it in, so it describes the storyboard, not the taskbar.
//
// Two sources describe the bar itself, and both are read here:
//
// - SHAppBarMessage(ABM_GETTASKBARPOS) is the shell's own dock record: uEdge is
//   ABE_LEFT/TOP/RIGHT/BOTTOM and rc is the *docked* rectangle, a 48 DIP strip at
//   the screen edge, reported unchanged while the bar is hidden. Measured here
//   with the bar auto-hidden: uEdge=3 rc=0,1704,2880,1800. It answers for the
//   primary taskbar only, so it is consulted on the primary monitor.
// - Shell_TrayWnd (Shell_SecondaryTrayWnd on the other monitors) is the bar
//   window. An auto-hidden bar keeps its thickness and slides out along its
//   axis, leaving a 2 px reveal strip on screen: measured here 0,1798,2880,1894
//   for the bottom bar. The docked side is therefore the side whose distance to
//   the monitor edge is within the bar's own thickness plus that strip, and the
//   bar is thin across that axis.
//
// Both are physical pixels, exactly like GetMonitorInfo; the single conversion to
// DIP is 1 / DpiScaleForWindow, the same path PanelHomeForWindow uses for the
// work area and the monitor, so no coordinate space is mixed. When neither
// source names an edge, the work-area inset is the last resort and the probe
// says so: that answer only exists when the bar reserves space at all.
//
// The probe also returns the band the bar reserves along the edge it named, so
// the axis and the rectangle's anchor come from one measurement and
// PanelRectAccepted admits the panel by the bar's own span instead of by an inset
// that auto-hide never provides.
// ---------------------------------------------------------------------------

// The measured answer of one probe. `source` names the winning one for the log
// line: "appbar", "bar", "work" or "none". The two rectangles are kept for that
// line, so a disagreement between the sources is visible where it happens.
struct TaskbarProbe {
    bool appBar = false;      // ABM_GETTASKBARPOS answered
    uint32_t appBarEdge = 0;  // ABE_* it reported
    bool appBarRect = false;  // ... with a real docked rectangle
    RECT appBarRectPx{};
    bool barWindow = false;  // a bar window is docked on this monitor
    DismissAxis barWindowEdge = DismissAxis::Left;
    RECT barWindowRectPx{};
    bool decided = false;
    DismissAxis edge = DismissAxis::Bottom;
    const wchar_t* source = L"none";
    bool band = false;  // the bar's span along `edge`, DIP
    float bandLow = 0.0f;
    float bandHigh = 0.0f;
};

// UI thread only: the probe of the geometry turn in progress. Every publish
// follows a probe in the same turn (Setup, RunGeometryMeasurement and the
// SizeChanged handler each probe first), so the band and the axis cannot come
// from two different measurements.
TaskbarProbe g_taskbarProbe;
HMONITOR g_taskbarProbeMonitor = nullptr;

// UI thread only. False until a probe has named an edge at all; a probe that
// cannot name one keeps `g_dismissAxis` rather than falling back to a side.
bool g_axisKnown = false;
bool g_axisKeptLogged = false;

bool AxisFromAppBarEdge(uint32_t edge, DismissAxis& axis) {
    switch (edge) {
        case ABE_LEFT:
            axis = DismissAxis::Left;
            return true;
        case ABE_TOP:
            axis = DismissAxis::Top;
            return true;
        case ABE_RIGHT:
            axis = DismissAxis::Right;
            return true;
        case ABE_BOTTOM:
            axis = DismissAxis::Bottom;
            return true;
    }
    return false;
}

// The edge one bar rectangle is docked to on this monitor. The bar is thin
// across its docked axis, so only that axis is a candidate: a full-width bottom
// bar is flush on left, right and bottom at once, and the 96 px height against
// the 2880 px width is what picks its real side. An auto-hidden bar has slid out
// by its own thickness, so the docked side is admitted within one thickness plus
// the reveal strip that stays on screen (2 px measured here; 8 px covers it at
// every scale a bar can be measured at). Two candidates at the same distance are
// ambiguous and answer nothing, which keeps a maximized window from reading as a
// bar.
bool AxisFromBarRect(const RECT& bar, const RECT& monitor, DismissAxis& axis) {
    const long width = bar.right - bar.left;
    const long height = bar.bottom - bar.top;
    if (!(width > 0) || !(height > 0)) {
        return false;
    }

    constexpr long kRevealSlackPx = 8;

    const auto overlaps = [](long low, long high, long otherLow, long otherHigh) {
        return low < otherHigh && otherLow < high;
    };

    int found = 0;
    long bestDistance = 0;
    DismissAxis best = DismissAxis::Bottom;
    const auto consider = [&](DismissAxis candidate, long distance) {
        if (found == 0 || distance < bestDistance) {
            found = 1;
            bestDistance = distance;
            best = candidate;
        } else if (distance == bestDistance) {
            ++found;
        }
    };

    if (width < height && overlaps(bar.top, bar.bottom, monitor.top,
                                   monitor.bottom)) {
        const long left = std::labs(bar.left - monitor.left);
        const long right = std::labs(monitor.right - bar.right);
        if (left <= width + kRevealSlackPx) {
            consider(DismissAxis::Left, left);
        }
        if (right <= width + kRevealSlackPx) {
            consider(DismissAxis::Right, right);
        }
    }
    if (height < width && overlaps(bar.left, bar.right, monitor.left,
                                   monitor.right)) {
        const long top = std::labs(bar.top - monitor.top);
        const long bottom = std::labs(monitor.bottom - bar.bottom);
        if (top <= height + kRevealSlackPx) {
            consider(DismissAxis::Top, top);
        }
        if (bottom <= height + kRevealSlackPx) {
            consider(DismissAxis::Bottom, bottom);
        }
    }

    if (found != 1) {
        return false;
    }
    axis = best;
    return true;
}

// One bar window per monitor: the primary bar is Shell_TrayWnd, every other
// monitor's bar is a Shell_SecondaryTrayWnd window. A bar answers only for a
// monitor it is docked on, so a bar belonging to another monitor fails the
// overlap test and the next one is tried.
bool BarWindowForMonitor(const RECT& monitor, RECT& bar, DismissAxis& edge) {
    HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (primary) {
        RECT rect{};
        if (GetWindowRect(primary, &rect) &&
            AxisFromBarRect(rect, monitor, edge)) {
            bar = rect;
            return true;
        }
    }

    for (HWND current = nullptr;;) {
        current = FindWindowExW(nullptr, current, L"Shell_SecondaryTrayWnd",
                                nullptr);
        if (!current) {
            break;
        }
        RECT rect{};
        if (!GetWindowRect(current, &rect)) {
            continue;
        }
        if (AxisFromBarRect(rect, monitor, edge)) {
            bar = rect;
            return true;
        }
    }
    return false;
}

// The strip the panel's taskbar-side edge must lie in, in DIP: the band the bar
// reserves at the screen edge, extended by one bar thickness toward the desktop.
// The bar's thickness is what the bar itself measures, so auto-hide cannot
// narrow it (the measured hidden bar is 0,1798,2880,1894 and still occupies the
// 48 DIP it has when shown), and the extension is a measured fact, not a
// tolerance: the settled panel sits 12 DIP from the bar's inner edge, so the
// band is twice the bar's thickness — `0..96` for a top taskbar with the panel's
// top at 36, `804..900` for a bottom one with the panel's bottom at 840 — while
// the open storyboard's rect is 313 DIP away. Half a monitor is not a bar and
// answers nothing.
bool TaskbarBandDip(DismissAxis axis, const RECT& bar, const RECT& monitor,
                    float scale, float& low, float& high) {
    if (!(scale > 0.0f)) {
        return false;
    }

    const float monLeft = static_cast<float>(monitor.left) / scale;
    const float monTop = static_cast<float>(monitor.top) / scale;
    const float monRight = static_cast<float>(monitor.right) / scale;
    const float monBottom = static_cast<float>(monitor.bottom) / scale;
    const float width = static_cast<float>(bar.right - bar.left) / scale;
    const float height = static_cast<float>(bar.bottom - bar.top) / scale;

    switch (axis) {
        case DismissAxis::Left:
            if (!(width > 0.0f) || width >= (monRight - monLeft) / 2.0f) {
                return false;
            }
            low = monLeft;
            high = monLeft + 2.0f * width;
            return true;
        case DismissAxis::Top:
            if (!(height > 0.0f) || height >= (monBottom - monTop) / 2.0f) {
                return false;
            }
            low = monTop;
            high = monTop + 2.0f * height;
            return true;
        case DismissAxis::Right:
            if (!(width > 0.0f) || width >= (monRight - monLeft) / 2.0f) {
                return false;
            }
            low = monRight - 2.0f * width;
            high = monRight;
            return true;
        case DismissAxis::Bottom:
            if (!(height > 0.0f) || height >= (monBottom - monTop) / 2.0f) {
                return false;
            }
            low = monBottom - 2.0f * height;
            high = monBottom;
            return true;
    }
    return false;
}

// Reads every source once and decides the edge in a fixed order: the shell's
// dock record first, the bar window second, the work-area inset last. A source
// that answers differently from an earlier one is left in the struct so the one
// log line reports both.
TaskbarProbe ProbeTaskbar(const MONITORINFO& info, float scale) {
    TaskbarProbe probe;
    const RECT& monitorRect = info.rcMonitor;

    if (info.dwFlags & MONITORINFOF_PRIMARY) {
        APPBARDATA data{};
        data.cbSize = sizeof(data);
        if (SHAppBarMessage(ABM_GETTASKBARPOS, &data)) {
            DismissAxis appBarAxis = DismissAxis::Bottom;
            if (AxisFromAppBarEdge(data.uEdge, appBarAxis)) {
                probe.appBar = true;
                probe.appBarEdge = data.uEdge;
                probe.appBarRect = data.rc.right > data.rc.left &&
                                   data.rc.bottom > data.rc.top;
                probe.appBarRectPx = data.rc;
                probe.decided = true;
                probe.edge = appBarAxis;
                probe.source = L"appbar";
            }
        }
    }

    RECT bar{};
    DismissAxis barAxis = DismissAxis::Bottom;
    if (BarWindowForMonitor(monitorRect, bar, barAxis)) {
        probe.barWindow = true;
        probe.barWindowEdge = barAxis;
        probe.barWindowRectPx = bar;
        if (!probe.decided) {
            probe.decided = true;
            probe.edge = barAxis;
            probe.source = L"bar";
        }
    }

    // The band follows the decided edge and prefers the docked rectangle: it is
    // the bar's registered position, while the window may be translated by
    // auto-hide. The window's own thickness is the same number when the bar is
    // shown and still the same number when it is hidden.
    const RECT* bandRect = probe.appBarRect
                               ? &probe.appBarRectPx
                               : (probe.barWindow ? &probe.barWindowRectPx
                                                  : nullptr);
    if (probe.decided && bandRect) {
        probe.band = TaskbarBandDip(probe.edge, *bandRect, monitorRect, scale,
                                    probe.bandLow, probe.bandHigh);
    }

    if (!probe.decided) {
        // Last resort, and only when it truly insets: under auto-hide the work
        // area is the full monitor, so this branch says nothing.
        const long insetLeft = info.rcWork.left - info.rcMonitor.left;
        const long insetTop = info.rcWork.top - info.rcMonitor.top;
        const long insetRight = info.rcMonitor.right - info.rcWork.right;
        const long insetBottom = info.rcMonitor.bottom - info.rcWork.bottom;
        const long largest = std::max(std::max(insetLeft, insetTop),
                                      std::max(insetRight, insetBottom));
        if (largest > 4) {
            probe.decided = true;
            probe.source = L"work";
            probe.edge = largest == insetLeft
                             ? DismissAxis::Left
                             : (largest == insetTop
                                    ? DismissAxis::Top
                                    : (largest == insetRight ? DismissAxis::Right
                                                             : DismissAxis::Bottom));
        }
    }

    return probe;
}

// Where the panel lives: the monitor's work area and the monitor itself. The
// panel is attached to the taskbar's side of the desktop, so `taskbarBand` names
// the strip the bar reserves along the dismiss axis; without a probe the work
// area supplies that strip, which is the same edge when the bar reserves space.
struct PanelHomeDip {
    DisplayRectDip work;
    DisplayRectDip monitor;
    bool taskbarBand = false;
    float taskbarLow = 0.0f;
    float taskbarHigh = 0.0f;
};

// The band the reveal candidate starts on, when no taskbar probe measured one:
// the work-area inset is the bar's thickness, so the band is the bar's strip
// plus one more thickness toward the desktop, the same span TaskbarBandDip
// builds from the probe. An auto-hidden bar reserves no inset, so this answers
// false and the candidate test falls back to kEdgeBandDip.
bool WorkAreaBandDip(const PanelHomeDip& home, int32_t axis, float& low,
                     float& high) {
    float inset = 0.0f;
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            inset = home.work.left - home.monitor.left;
            break;
        case DismissAxis::Top:
            inset = home.work.top - home.monitor.top;
            break;
        case DismissAxis::Right:
            inset = home.monitor.right - home.work.right;
            break;
        case DismissAxis::Bottom:
            inset = home.monitor.bottom - home.work.bottom;
            break;
    }
    if (!(inset > 0.0f)) {
        return false;
    }
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            low = home.monitor.left;
            high = home.monitor.left + 2.0f * inset;
            break;
        case DismissAxis::Top:
            low = home.monitor.top;
            high = home.monitor.top + 2.0f * inset;
            break;
        case DismissAxis::Right:
            low = home.monitor.right - 2.0f * inset;
            high = home.monitor.right;
            break;
        case DismissAxis::Bottom:
            low = home.monitor.bottom - 2.0f * inset;
            high = home.monitor.bottom;
            break;
    }
    return true;
}


bool MeasurePanelRect(PanelMeasurement& panel) {
    wux::FrameworkElement root = g_root;
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!root || !hwnd) {
        return false;
    }

    float frameLeft = 0.0f;
    float frameTop = 0.0f;
    float frameRight = 0.0f;
    float frameBottom = 0.0f;
    bool measured = false;
    float bestArea = -1.0f;
    bool haveNonFilling = false;
    float nonFillingArea = -1.0f;
    float nfLeft = 0.0f, nfTop = 0.0f, nfRight = 0.0f, nfBottom = 0.0f;

    // The visible panel is the largest child that actually lies inside the
    // root. Taking the first visible child picked a wrapper that sits 12 DIP
    // above the root on a bottom taskbar, which put the reveal anchor outside
    // the screen.
    const float rootWidth = static_cast<float>(root.ActualWidth());
    const float rootHeight = static_cast<float>(root.ActualHeight());

    const int32_t childCount =
        wux::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int32_t i = 0; i < childCount; ++i) {
        wux::FrameworkElement child =
            wux::Media::VisualTreeHelper::GetChild(root, i)
                .try_as<wux::FrameworkElement>();
        if (!child) {
            continue;
        }
        const float width = static_cast<float>(child.ActualWidth());
        const float height = static_cast<float>(child.ActualHeight());
        if (!(width > 0.0f) || !(height > 0.0f) ||
            child.Visibility() != wux::Visibility::Visible) {
            continue;
        }

        // The transform is relative to the window's XAML root; null asks for
        // that root. A scaling or offset transform stays in the mapping.
        wux::Media::GeneralTransform transform = child.TransformToVisual(nullptr);
        const winrt::Windows::Foundation::Point topLeft =
            transform.TransformPoint({0.0f, 0.0f});
        const float left = topLeft.X;
        const float top = topLeft.Y;
        const float right = transform.TransformPoint({width, 0.0f}).X;
        const float bottom = transform.TransformPoint({0.0f, height}).Y;

        const float overlapWidth =
            std::max(0.0f, std::min(right, rootWidth) - std::max(left, 0.0f));
        const float overlapHeight =
            std::max(0.0f, std::min(bottom, rootHeight) - std::max(top, 0.0f));
        const float area = overlapWidth * overlapHeight;
        if (!(area > 0.0f)) {
            continue;
        }

        // A child that fills the root is the hosting wrapper, not the panel, so
        // it is only a fallback when nothing smaller is visible.
        const float rootArea = rootWidth * rootHeight;
        if (rootArea <= 0.0f || area < 0.98f * rootArea) {
            if (area > nonFillingArea) {
                nonFillingArea = area;
                nfLeft = left;
                nfTop = top;
                nfRight = right;
                nfBottom = bottom;
                haveNonFilling = true;
            }
        }
        if (area > bestArea) {
            bestArea = area;
            frameLeft = left;
            frameTop = top;
            frameRight = right;
            frameBottom = bottom;
            measured = true;
        }
    }

    if (haveNonFilling) {
        frameLeft = nfLeft;
        frameTop = nfTop;
        frameRight = nfRight;
        frameBottom = nfBottom;
    }

    if (!measured) {
        return false;
    }

    // The frame is in XAML root coordinates; the origin turns it into a screen
    // rectangle for whichever side the taskbar is on.
    float originX = 0.0f;
    float originY = 0.0f;
    if (!XamlOriginDip(originX, originY)) {
        return false;
    }

    panel.left = originX + frameLeft;
    panel.top = originY + frameTop;
    panel.right = originX + frameRight;
    panel.bottom = originY + frameBottom;
    return true;
}

bool PanelHomeForWindow(PanelHomeDip& home) {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return false;
    }

    const float scale = DpiScaleForWindow(hwnd);
    if (!(scale > 0.0f)) {
        return false;
    }

    home.work.left = info.rcWork.left / scale;
    home.work.top = info.rcWork.top / scale;
    home.work.right = info.rcWork.right / scale;
    home.work.bottom = info.rcWork.bottom / scale;
    home.monitor.left = info.rcMonitor.left / scale;
    home.monitor.top = info.rcMonitor.top / scale;
    home.monitor.right = info.rcMonitor.right / scale;
    home.monitor.bottom = info.rcMonitor.bottom / scale;

    // The strip the bar reserves, from the probe the current turn took. A probe
    // for another monitor cannot describe this monitor's bar, so the work area
    // is the band then, exactly as it was before the probe existed.
    if (g_taskbarProbeMonitor == monitor && g_taskbarProbe.band) {
        home.taskbarBand = true;
        home.taskbarLow = g_taskbarProbe.bandLow;
        home.taskbarHigh = g_taskbarProbe.bandHigh;
    }
    return true;
}

// Whether a measured rect is the panel this layout shows. The rule lives here,
// once, and both of its tests say why their margin is what it is:
//
// - the rect has an extent and lies on the monitor. kPanelLayoutMarginDip rounds
//   a border that sits exactly on an edge; nothing larger is a layout fact, and
//   the 24 DIP of kScrollerEndSlopDip is a scroll-position allowance, not a
//   layout one.
// - the side against the taskbar lies inside the strip the bar's edge defines:
//   the menu is attached to the taskbar's side of the desktop, so that edge is
//   near the screen edge and nowhere near the middle of the screen. The strip
//   comes from the bar itself (TaskbarProbe), not from the work area, because an
//   auto-hidden bar reserves no work area at all while still occupying its 48
//   DIP at the edge and the panel keeps its 12 DIP offset from it. Without a
//   probe the strip is the work-area inset, which is the same band when the bar
//   reserves space; the margin only rounds a border that sits exactly on the
//   strip's inner edge.
//
// This is the test that separates the settled frame from the open storyboard's.
// Measured here (right taskbar, work 0,0,2784,1800, monitor 0,0,2880,1800 at
// 200%): the storyboard rect is 12,-12..1005,539 and the settled one is
// 411,163..1404,714. The storyboard's right edge is 387 DIP short of the
// taskbar band, so it is rejected whole; the settled right edge at 1404 sits
// 12 DIP under the work-area edge and 36 DIP inside the screen, which is where
// the shell attaches it.
bool PanelRectAccepted(const PanelMeasurement& panel,
                       const PanelHomeDip& home, int32_t axis) {
    if (!(panel.right > panel.left) || !(panel.bottom > panel.top)) {
        return false;
    }
    if (panel.left < home.monitor.left - kPanelLayoutMarginDip ||
        panel.right > home.monitor.right + kPanelLayoutMarginDip ||
        panel.top < home.monitor.top - kPanelLayoutMarginDip ||
        panel.bottom > home.monitor.bottom + kPanelLayoutMarginDip) {
        return false;
    }

    // The taskbar's side of the panel, and the strip that side must lie in: the
    // bar's reserved span when the probe measured it, the work-area inset
    // otherwise.
    float edge = 0.0f;
    float low = 0.0f;
    float high = 0.0f;
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            edge = panel.left;
            low = home.work.left;
            high = home.monitor.left;
            break;
        case DismissAxis::Top:
            edge = panel.top;
            low = home.work.top;
            high = home.monitor.top;
            break;
        case DismissAxis::Right:
            edge = panel.right;
            low = home.work.right;
            high = home.monitor.right;
            break;
        case DismissAxis::Bottom:
            edge = panel.bottom;
            low = home.work.bottom;
            high = home.monitor.bottom;
            break;
        default:
            return false;
    }
    if (home.taskbarBand) {
        low = home.taskbarLow;
        high = home.taskbarHigh;
    }

    const float from = std::min(low, high);
    const float to = std::max(low, high);
    return edge >= from - kPanelAnchorMarginDip &&
           edge <= to + kPanelAnchorMarginDip;
}

// The one measurement and the one publish point of the geometry.
//
// `measurePanel` says this pass may re-derive the rect. It is false when a live
// gesture owns the edge (the edge must not move under the finger) and for a
// request that only refreshes the axis and the travel.
//
// The trail line speaks once per open — the request's open-time turn is what
// clears the flag — and again whenever the value moved by more than 1 DIP, so a
// stable layout does not repeat it and a resize cannot hide it.
//
// `expectedGeneration` is the epoch a queued request was recorded in; a
// measurement that runs after that epoch ended publishes nothing. 0 publishes
// whatever root is attached, which is what the synchronous Setup and SizeChanged
// paths want. `requestId` is the live measurement request on those queued paths,
// and 0 on the synchronous ones. The candidate is built first and validated at
// commit, so a turn that a newer request replaced measures but publishes nothing.
//
// The previous rect is carried over only when the whole layout identity matches:
// axis, travel, client origin, work area and monitor. The same travel on a moved
// taskbar or a different cross-axis origin is not the same panel; the carried
// rect is also tested against the current taskbar band before it is accepted.
// Dropping it leaves the snapshot with no panel, which reads as "not measured
// yet" and makes the follow use its fallback.
bool PublishPanelTrailDip(bool measurePanel, uint32_t expectedGeneration,
                          uint64_t requestId) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }
    if (!(g_dismissDistance > 0.0f)) {
        return false;  // no renderer, no travel to publish
    }

    const uint32_t generation = g_openGeneration.load(std::memory_order_relaxed);
    if (expectedGeneration != 0 && generation != expectedGeneration) {
        return false;
    }

    GestureGeometry previous;
    AcquireSRWLockShared(&g_geometryLock);
    previous = g_geometry;
    ReleaseSRWLockShared(&g_geometryLock);

    GestureGeometry candidate;
    candidate.generation = generation;
    candidate.layout.axis = g_dismissAxis;
    candidate.layout.distance = g_dismissDistance;

    float originX = 0.0f;
    float originY = 0.0f;
    const bool haveOrigin = XamlOriginDip(originX, originY);
    if (haveOrigin) {
        candidate.layout.originX = originX;
        candidate.layout.originY = originY;
    }

    // The panel's home on every axis, so it is read once for the validity test
    // and for the lines that report it.
    PanelHomeDip home;
    const bool haveHome = PanelHomeForWindow(home);
    if (haveHome) {
        candidate.layout.workLeft = home.work.left;
        candidate.layout.workTop = home.work.top;
        candidate.layout.workRight = home.work.right;
        candidate.layout.workBottom = home.work.bottom;
        candidate.layout.monitorLeft = home.monitor.left;
        candidate.layout.monitorTop = home.monitor.top;
        candidate.layout.monitorRight = home.monitor.right;
        candidate.layout.monitorBottom = home.monitor.bottom;

        // The reveal candidate's band travels with the snapshot, so the input
        // thread starts a pull on the taskbar's own strip instead of a fixed
        // 12 DIP: the bar's measured span plus one thickness toward the desktop.
        if (home.taskbarBand) {
            candidate.band = true;
            candidate.bandLow = home.taskbarLow;
            candidate.bandHigh = home.taskbarHigh;
        } else {
            candidate.band = WorkAreaBandDip(home, candidate.layout.axis,
                                             candidate.bandLow,
                                             candidate.bandHigh);
        }
    }

    // A rect may be carried only inside one layout identity, and only when it
    // still sits on the taskbar band that identity describes.
    bool carriedPanel = false;
    PanelMeasurement carried;
    if (previous.panel && haveHome && haveOrigin &&
        SameLayoutIdentity(previous.layout, candidate.layout)) {
        carried.left = previous.left;
        carried.top = previous.top;
        carried.right = previous.right;
        carried.bottom = previous.bottom;
        if (PanelRectAccepted(carried, home, candidate.layout.axis)) {
            carriedPanel = true;
            candidate.panel = true;
            candidate.left = previous.left;
            candidate.top = previous.top;
            candidate.right = previous.right;
            candidate.bottom = previous.bottom;
        }
    }

    bool measuredPanel = false;
    if (measurePanel) {
        PanelMeasurement panel;
        bool havePanel = false;
        try {
            havePanel = MeasurePanelRect(panel);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Panel rect mapping failed: %08X", e.code());
        } catch (...) {
            // A tree mid-teardown must not break Setup or SizeChanged.
        }

        if (havePanel && haveHome) {
            if (PanelRectAccepted(panel, home, candidate.layout.axis)) {
                candidate.panel = true;
                candidate.left = panel.left;
                candidate.top = panel.top;
                candidate.right = panel.right;
                candidate.bottom = panel.bottom;
                measuredPanel = true;
            } else {
                // The rect was measured but is not a panel: the storyboard has
                // not placed it, or the layout is between sizes. Nothing from it
                // is published; the snapshot keeps its last whole value. One line
                // per open: the generation is part of the latch, so a retried
                // pass in the same open stays silent and the next open speaks
                // again. A pass whose open is already gone stays silent too.
                const uint32_t openGeneration =
                    g_openGeneration.load(std::memory_order_relaxed);
                if (expectedGeneration == 0 ||
                    expectedGeneration == openGeneration) {
                    const uint32_t rejectedGeneration =
                        expectedGeneration != 0 ? expectedGeneration
                                                : openGeneration;
                    if (!g_panelTrailRejectedLogged ||
                        g_panelTrailRejectedGeneration != rejectedGeneration) {
                        g_panelTrailRejectedLogged = true;
                        g_panelTrailRejectedGeneration = rejectedGeneration;
                        TraceLog(
                            L"reveal panelTrail rejected %.0f axis=%d "
                            L"rect=%.0f,%.0f,%.0f,%.0f work=%.0f,%.0f,%.0f,%.0f "
                            L"mon=%.0f,%.0f,%.0f,%.0f band=%.0f,%.0f",
                            TrailEdge(candidate.layout.axis, panel.left,
                                      panel.top, panel.right, panel.bottom),
                            candidate.layout.axis, panel.left, panel.top,
                            panel.right, panel.bottom, home.work.left,
                            home.work.top, home.work.right, home.work.bottom,
                            home.monitor.left, home.monitor.top,
                            home.monitor.right, home.monitor.bottom,
                            home.taskbarBand ? home.taskbarLow : -1.0f,
                            home.taskbarBand ? home.taskbarHigh : -1.0f);
                    }
                }
            }
        }
    }

    // The commit decision and the write are one critical section, under the lock
    // the epoch bump and a newer request also take: the request that asked for
    // this pass must still be live, its epoch must still be the open on screen,
    // the layout the candidate was built from must still be the one the writer
    // holds, and no live contact may own the panel edge. A contact that appeared
    // while the measurement ran drops the freshly measured rect; a carried rect
    // from the same identity stays only because the identity did not move. The
    // candidate is complete by now, so nothing is measured here: one struct
    // assignment and no window query.
    {
        std::lock_guard lock(g_measureMutex);
        if (g_unloading.load(std::memory_order_relaxed)) {
            return false;
        }
        if (expectedGeneration != 0 &&
            g_openGeneration.load(std::memory_order_relaxed) !=
                expectedGeneration) {
            return false;
        }
        if (requestId != 0 &&
            !MeasureRequestLiveLocked(expectedGeneration, requestId)) {
            return false;
        }
        if (g_dismissAxis != candidate.layout.axis ||
            !SameLayoutDistance(g_dismissDistance, candidate.layout.distance)) {
            return false;
        }
        if (g_panelOwned.load(std::memory_order_relaxed) && measuredPanel) {
            // The edge may not move under the finger. A freshly measured rect is
            // discarded; the rect this layout already carried stays, because the
            // identity did not move and it is the anchor the follow is using.
            measuredPanel = false;
            candidate.panel = carriedPanel;
            if (carriedPanel) {
                candidate.left = carried.left;
                candidate.top = carried.top;
                candidate.right = carried.right;
                candidate.bottom = carried.bottom;
            }
        }

        // The snapshot goes out as one unit: the axis and the travel always, and
        // the panel only with a rect this pass or a carried one this layout
        // sanctioned. The epoch is read here, inside the section, so the
        // snapshot names the open it was actually published in.
        candidate.generation = g_openGeneration.load(std::memory_order_relaxed);
        StoreGestureGeometry(candidate);
    }

    // The window bounds the press hit test reads are a separate diagnostic and
    // stay outside the section.
    PublishWindowRectDip();

    if (candidate.panel) {
        const float trail = GeometryTrailDip(candidate);
        if (measuredPanel) {
            // One trail line per open: the open-time turn speaks even when the
            // value did not move (the request cleared the flag), the settled turn
            // and every later pass only when it did, so a stable layout does not
            // repeat the line and a moved one cannot hide.
            const float previousTrail =
                previous.panel ? GeometryTrailDip(previous) : -1.0f;
            const bool moved = !previous.panel ||
                               std::fabs(trail - previousTrail) > 1.0f;
            if (moved || !g_panelTrailLogged) {
                g_panelTrailLogged = true;
                TraceLog(L"reveal panelTrail=%.0f axis=%d rect=%.0f,%.0f,%.0f,%.0f "
                        L"work=%.0f,%.0f,%.0f,%.0f band=%.0f,%.0f gen=%u",
                        trail, candidate.layout.axis, candidate.left,
                        candidate.top, candidate.right, candidate.bottom,
                        home.work.left, home.work.top, home.work.right,
                        home.work.bottom,
                        home.taskbarBand ? home.taskbarLow : -1.0f,
                        home.taskbarBand ? home.taskbarHigh : -1.0f,
                        candidate.generation);
            }
        }
        return true;
    }

    // No panel measured for this layout: the follow uses the root-width fallback,
    // which is the mapping this mod shipped with, and the hit test accepts a press
    // it cannot place.
    if (!g_panelTrailFallbackLogged) {
        g_panelTrailFallbackLogged = true;
        TraceLog(L"reveal panelTrail=%.0f axis=%d (fallback)",
                FallbackTrailDip(candidate.layout.axis,
                                 candidate.layout.distance),
                candidate.layout.axis);
    }
    return true;
}

// ---------------------------------------------------------------------------
// The settled measurement's layout watch
//
// The open re-measures the panel once, but that measurement can land while the
// shell's own open storyboard still has the frame out of place: measured on this
// machine (right taskbar), the storyboard rect is 12,-12..1005,539 where the
// settled one is 411,163..1404,714, and a bottom taskbar read a panel top of -12
// DIP where the settled panel starts at 200. The reveal follow maps `Progress`
// against that edge, so an early measurement anchors the panel by the width of
// the panel for that whole open.
//
// LayoutUpdated is the one signal that arrives when XAML has placed the panel, and
// it costs nothing while no watch is armed. The watch skips the first pass (it
// usually precedes the placement), runs the request's settled measurement on the
// second, then revokes its own token, so at most one watch ever exists and no
// handler is left behind. The close path and the renderer teardown revoke it too,
// because a menu that closed before the second pass must not measure anything
// later.
//
// The watch carries the request it belongs to and its own serial: a callback
// that outlived a newer request, or a newer watch, measures and revokes nothing.
// A revoke that throws keeps the token and its owner, so the handle can be
// retried; a root that still carries a registration refuses the next one.
// ---------------------------------------------------------------------------
SettledMeasureRequest g_layoutRequest{};  // UI thread only

void StopSettledTrailWatch() {
    wux::FrameworkElement root = g_layoutRoot ? g_layoutRoot : g_root;
    if (g_layoutToken.value) {
        if (!root) {
            Wh_Log(L"LayoutUpdated registration has no owner to revoke on");
            return;
        }
        try {
            root.LayoutUpdated(g_layoutToken);
            g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"LayoutUpdated revoke failed: %08X", e.code());
            // The token and its owner stay: the token is the only handle on a
            // handler that can still fire, and overwriting it with the next
            // watch would lose it for the process lifetime. The request and the
            // serial stay with it, so the handler still acts only for itself.
            return;
        } catch (...) {
            Wh_Log(L"LayoutUpdated revoke failed with an unknown exception");
            return;
        }
    }
    g_layoutToken = {};
    g_layoutRoot = nullptr;
    g_layoutTicksLeft = 0;
    g_layoutWatchSerial = 0;
    g_layoutRequest = SettledMeasureRequest{};
}

// The close path's deferred cleanup names the epoch that ended. A watch armed by
// a newer open belongs to that open and is left alone.
void StopSettledTrailWatchIfGeneration(uint32_t maxGeneration) {
    if (g_layoutTicksLeft != 0 && g_layoutRequest.generation > maxGeneration) {
        return;
    }
    StopSettledTrailWatch();
}

void ArmLayoutWatch(const SettledMeasureRequest& request) {
    wux::FrameworkElement root = g_root;
    if (!root) {
        return;  // no root, no layout to wait for
    }

    // One watch per open: a work-area broadcast records a new request, and the
    // previous watch would otherwise measure the same settled layout twice. A
    // registration that survived its revoke keeps its token and its owner:
    // overwriting them would lose the only handle on a handler that can still
    // fire. This open then gets no layout watch; its deadline still measures.
    StopSettledTrailWatch();
    if (g_layoutToken.value) {
        Wh_Log(L"LayoutUpdated registration survived its revoke; this open gets "
               L"no layout watch");
        TraceLog(L"xaml layout handler left attached; new registration refused");
        return;
    }
    g_layoutTicksLeft = 2;
    g_layoutRequest = request;
    g_layoutRoot = root;

    uint32_t watchSerial = ++g_layoutWatchMint;
    if (watchSerial == 0) {
        watchSerial = ++g_layoutWatchMint;  // 0 always means "no watch"
    }
    // The serial is current before the registration exists: a callback can only
    // run after it, and the catch below clears the serial with the token.
    g_layoutWatchSerial = watchSerial;
    try {
        g_layoutToken = root.LayoutUpdated(
            [watchSerial, request](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) {
                try {
                    if (watchSerial != g_layoutWatchSerial) {
                        return;  // a callback that outlived its revoke
                    }
                    if (g_layoutTicksLeft == 0) {
                        return;
                    }
                    if (--g_layoutTicksLeft > 0) {
                        return;  // the first pass precedes the panel placement
                    }
                    RunGeometryMeasurement(request, true);
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"Settled trail failed: %08X", e.code());
                } catch (...) {
                    // Never let an exception escape into XAML.
                }
                // Revoked after the measurement, and also when one threw, so the
                // handler never outlives the open it belongs to. Only this
                // watch's own stop: a newer watch may already own the fields.
                if (watchSerial == g_layoutWatchSerial) {
                    StopSettledTrailWatch();
                }
            });
        if (g_layoutToken.value) {
            g_uiRegistrationCount.fetch_add(1, std::memory_order_relaxed);
        } else {
            g_layoutWatchSerial = 0;
            g_layoutRoot = nullptr;
            g_layoutTicksLeft = 0;
            g_layoutRequest = SettledMeasureRequest{};
        }
    } catch (winrt::hresult_error const& e) {
        g_layoutToken = {};
        g_layoutWatchSerial = 0;
        g_layoutRoot = nullptr;
        g_layoutTicksLeft = 0;
        g_layoutRequest = SettledMeasureRequest{};
        TraceLog(L"layout watch failed err=%08X", static_cast<unsigned>(e.code()));
    } catch (...) {
        g_layoutToken = {};
        g_layoutWatchSerial = 0;
        g_layoutRoot = nullptr;
        g_layoutTicksLeft = 0;
        g_layoutRequest = SettledMeasureRequest{};
        TraceLog(L"layout watch failed err=unknown");
    }
}

// ---------------------------------------------------------------------------
// Scroller cache (the element a vertical dismiss competes with)
//
// Measured on this build, the menu contains three ScrollViewers, and only one of
// them can take a drag away from a dismiss:
//
// - the wrapper around StartUI.AllAppsGridListView (depth 16, 85,230-345,839)
//   reports ScrollableWidth 8380 and VerticalOffset 2252 whatever the content
//   does: its offsets are not the visual position, and its huge horizontal
//   extent marks it as a host rather than a list;
// - StartUI.TileListView's viewer (353,225-1005,839) and the rail's
//   (13,656-60,794) report ScrollableHeight 0, so they have nothing to scroll
//   and a drag on them belongs to the dismiss;
// - the ScrollViewer inside the AllApps ListView (depth 21, same rect as the
//   wrapper, ScrollableHeight 3664) is the one that moves with the list.
//
// Selection is therefore: the innermost ScrollViewer whose ScrollableWidth is
// zero and whose ScrollableHeight is not, with a non-empty rect. Depth beats
// area, so the live list wins over its wrapper at the same rect; at equal depth
// the smaller rect wins, because the more specific element is the one a press
// lands on. The rect is mapped to screen DIP through the same origin as the
// panel rect, so the input thread can test a press point against it directly.
// ---------------------------------------------------------------------------
// One scrollable element the survey found. `area` is the pick's tie-break and
// never leaves this file.
struct ScrollerFound {
    wuxc::ScrollViewer viewer{nullptr};
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float area = 0.0f;
    uint32_t depth = 0;
    wchar_t label[kScrollerLabelChars] = {};
};

struct ScrollerSurvey {
    ScrollerFound found[kMaxScrollRegions];
    int count = 0;
    // `complete` says the traversal reached every branch it was allowed to read.
    // A throw, an unreadable child or the depth limit clears it: "no element
    // found" then means "the search did not finish", never "nothing here
    // scrolls".
    bool complete = true;
    // More scrollable viewers than slots: the deepest `kMaxScrollRegions` are
    // kept, so the ones a press is most likely to land on survive.
    bool overflow = false;
};

// Copies `text` into a fixed label, cut at the array bound. A label is a
// diagnostic, so a cut name is better than no line.
void CopyScrollerLabel(const wchar_t* text, wchar_t (&label)[kScrollerLabelChars]) {
    const size_t length = text ? std::min(wcslen(text),
                                          static_cast<size_t>(kScrollerLabelChars - 1))
                               : 0;
    if (length > 0) {
        std::wmemcpy(label, text, length);
    }
    label[length] = L'\0';
}

// The containers every control template puts between a list and its own
// ScrollViewer, by full and by short type name. A region's label skips them:
// "StartUI.TileListView" names the element a user drags, "Border" does not.
bool GenericContainerName(const winrt::hstring& name) {
    static const wchar_t* const kGeneric[] = {
        L"Windows.UI.Xaml.Controls.ScrollViewer",
        L"Windows.UI.Xaml.Controls.Border",
        L"Windows.UI.Xaml.Controls.Grid",
        L"Windows.UI.Xaml.Controls.ContentPresenter",
        L"Windows.UI.Xaml.Controls.ScrollContentPresenter",
        L"Windows.UI.Xaml.Controls.ItemsPresenter",
        L"Windows.UI.Xaml.Controls.StackPanel",
        L"ScrollViewer",
        L"Border",
        L"Grid",
        L"ContentPresenter",
        L"ScrollContentPresenter",
        L"ItemsPresenter",
        L"StackPanel",
    };
    for (const wchar_t* generic : kGeneric) {
        if (name == generic) {
            return true;
        }
    }
    return false;
}

// The part of a WinRT class name after the last dot: `ScrollViewer` for
// `Windows.UI.Xaml.Controls.ScrollViewer`.
winrt::hstring ShortTypeName(const winrt::hstring& type) {
    const std::wstring_view view = type;
    const size_t dot = view.rfind(L'.');
    if (dot == std::wstring_view::npos) {
        return type;
    }
    return winrt::hstring(view.substr(dot + 1));
}

// A name is worth a log line only when it names the element rather than its
// template: the shell's templated ScrollViewer is literally named
// "ScrollViewer", so its own name says nothing the type does not.
bool MeaningfulElementName(const winrt::hstring& name,
                           const winrt::hstring& type) {
    if (name.empty() || GenericContainerName(name)) {
        return false;
    }
    return name != type && name != ShortTypeName(type);
}

// The name a log line can use for this element: its own Name when that names the
// element rather than its template, the short type name of the nearest ancestor
// that is not a generic container (the list a templated ScrollViewer belongs to,
// `TileListView` / `AllAppsGridListView`), and the two joined as `name>owner`
// when both exist. Runs on the UI thread, once per region per survey.
void DescribeScrollerElement(const wux::UIElement& element,
                             wchar_t (&label)[kScrollerLabelChars]) {
    winrt::hstring elementType;
    try {
        elementType = winrt::get_class_name(element);
    } catch (...) {
    }

    winrt::hstring ownName;
    try {
        wux::FrameworkElement named = element.try_as<wux::FrameworkElement>();
        if (named && MeaningfulElementName(named.Name(), elementType)) {
            ownName = named.Name();
        }
    } catch (...) {
    }

    winrt::hstring ownerType;
    wux::DependencyObject ancestor = element;
    for (int level = 0; level < 12 && ancestor; ++level) {
        try {
            ancestor = wux::Media::VisualTreeHelper::GetParent(ancestor);
        } catch (...) {
            break;
        }
        if (!ancestor) {
            break;
        }
        try {
            const winrt::hstring type = winrt::get_class_name(ancestor);
            if (ownerType.empty() && !GenericContainerName(type) &&
                !GenericContainerName(ShortTypeName(type))) {
                ownerType = ShortTypeName(type);
            }
        } catch (...) {
        }
    }

    wchar_t text[kScrollerLabelChars * 2] = {};
    if (!ownName.empty() && !ownerType.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s>%s",
                     ownName.c_str(), ownerType.c_str());
    } else if (!ownName.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s", ownName.c_str());
    } else if (!ownerType.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s",
                     ownerType.c_str());
    } else if (!elementType.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s",
                     ShortTypeName(elementType).c_str());
    } else {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"ScrollViewer");
    }
    CopyScrollerLabel(text, label);
}

// Keeps the deepest scrollable viewers, dropping the shallowest when full: a
// scroll host and the list inside it share a rect, and the list is the one a
// press lands on.
void AddScrollerFound(ScrollerSurvey& survey, const ScrollerFound& found) {
    if (survey.count < kMaxScrollRegions) {
        survey.found[survey.count++] = found;
        return;
    }
    survey.overflow = true;
    int shallowest = 0;
    for (int i = 1; i < survey.count; ++i) {
        if (survey.found[i].depth < survey.found[shallowest].depth) {
            shallowest = i;
        }
    }
    if (found.depth > survey.found[shallowest].depth) {
        survey.found[shallowest] = found;
    }
}

void CollectScrollableElements(const wux::UIElement& element, uint32_t depth,
                               float originX, float originY,
                               ScrollerSurvey& survey) {
    if (!element) {
        return;
    }
    if (depth > kScrollerScanMaxDepth) {
        survey.complete = false;
        return;
    }

    try {
        wuxc::ScrollViewer viewer = element.try_as<wuxc::ScrollViewer>();
        if (viewer) {
            const winrt::Windows::Foundation::Size size = element.ActualSize();
            const wux::Media::GeneralTransform transform =
                element.TransformToVisual(nullptr);
            const winrt::Windows::Foundation::Point topLeft =
                transform.TransformPoint({0.0f, 0.0f});
            const winrt::Windows::Foundation::Point bottomRight =
                transform.TransformPoint({size.Width, size.Height});

            const float left = originX + topLeft.X;
            const float top = originY + topLeft.Y;
            const float right = originX + bottomRight.X;
            const float bottom = originY + bottomRight.Y;
            const float area = (right - left) * (bottom - top);

            // A viewer that cannot scroll vertically cannot take the drag, and
            // a collapsed or inverted rect is not a place a finger can press.
            if (viewer.ScrollableWidth() <= kScrollerAxisSlopDip &&
                viewer.ScrollableHeight() > kScrollerAxisSlopDip &&
                right > left && bottom > top && area > 1.0f) {
                ScrollerFound found;
                found.viewer = viewer;
                found.left = left;
                found.top = top;
                found.right = right;
                found.bottom = bottom;
                found.area = area;
                found.depth = depth;
                DescribeScrollerElement(element, found.label);
                AddScrollerFound(survey, found);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Scroller measure failed: %08X", e.code());
        survey.complete = false;
    } catch (...) {
        // A branch mid-teardown ends this branch only, but the answer is no
        // longer whole.
        survey.complete = false;
    }

    try {
        const int32_t count =
            wux::Media::VisualTreeHelper::GetChildrenCount(element);
        if (depth >= kScrollerScanMaxDepth && count > 0) {
            survey.complete = false;  // children exist below the limit; not searched
            return;
        }
        for (int32_t i = 0; i < count; ++i) {
            CollectScrollableElements(
                wux::Media::VisualTreeHelper::GetChild(element, i)
                    .try_as<wux::UIElement>(),
                depth + 1, originX, originY, survey);
        }
    } catch (...) {
        // A child being attached or removed ends this branch only.
        survey.complete = false;
    }
}

// Drops one slot's handler, so a re-measure can subscribe to the same or another
// element. Runs on the UI thread. A revoke that throws keeps the token and its
// owner — `owner`, not the slot's `viewer`: the token is the only handle on a
// handler that can still fire, and overwriting it with the next registration
// would lose it for the process lifetime, so the slot also stays occupied and is
// not reused. Clearing the slot's `viewer` is the caller's decision, because a
// committed region that keeps its rect keeps the viewer that measured it.
void RevokeScrollerWatch(int slot) {
    if (slot < 0 || slot >= kMaxScrollRegions) {
        return;
    }
    ScrollerWatch& watch = ScrollerWatches()[slot];
    wuxc::ScrollViewer owner = watch.owner;
    if (watch.token.value) {
        if (!owner) {
            Wh_Log(L"ViewChanged registration has no owner to revoke on");
            return;
        }
        try {
            owner.ViewChanged(watch.token);
            g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"ViewChanged revoke failed: %08X", e.code());
            return;
        } catch (...) {
            Wh_Log(L"ViewChanged revoke failed with an unknown exception");
            return;
        }
    }
    watch.token = {};
    watch.owner = nullptr;
}

// Publishes the offsets of the region a committed watch describes. ViewChanged
// fires on the UI thread and only while that region actually scrolls, so this is
// the whole cost of keeping the gate honest.
//
// The authorization is the cache's own identity, not the registration: a handler
// may write only while its slot still names a region of the committed survey, the
// survey is `Scrollable`, and it belongs to the open and the layout on screen. A
// handler whose revoke threw is still the registration owner and still fires, so
// it is exactly the case that must not be trusted: after a replacement its numbers
// would land under another region's rect, and a vertical drag would then be gated
// against the wrong element.
void RefreshScrollerOffsets(wuxc::ScrollViewer const& watched) {
    if (!watched) {
        return;
    }

    int slot = -1;
    for (int i = 0; i < kMaxScrollRegions; ++i) {
        if (ScrollerWatches()[i].viewer == watched) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        return;
    }

    try {
        const float verticalOffset = watched.VerticalOffset();
        const float scrollableHeight = watched.ScrollableHeight();
        std::lock_guard lock(g_scrollerMutex);
        if (g_scroller.state != ScrollerState::Scrollable ||
            g_scroller.generation !=
                g_openGeneration.load(std::memory_order_relaxed) ||
            g_scroller.layoutVersion !=
                g_layoutVersion.load(std::memory_order_acquire)) {
            return;
        }
        for (int i = 0; i < g_scroller.count; ++i) {
            if (g_scroller.regions[i].watchSlot == slot) {
                g_scroller.regions[i].verticalOffset = verticalOffset;
                g_scroller.regions[i].scrollableHeight = scrollableHeight;
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Scroller offset read failed: %08X", e.code());
    } catch (...) {
        // A closed menu must not break the handler.
    }
}

// Why a completed scroller answer was written or not. The caller turns the two
// refusals into the log line and the retry decision; `Obsolete` is silent because
// the pass belongs to a request or an open that no longer exists.
enum class ScrollerCommitState {
    Commit = 0,
    Obsolete = 1,
    Owned = 2,
    Moved = 3,
};

// Writes one completed answer if — and only if — the request that asked for it is
// live, its open is still on screen, the layout it was mapped in is still the one
// published, and no live contact owns the panel. The checks and the write are one
// critical section under the commit lock, which the epoch bump and the request
// replacement also take, so a close, a newer request or a resize cannot slip
// between the decision and the write. The search that produced `answer` stayed
// outside it.
ScrollerCommitState CommitScrollerCache(const ScrollerCache& answer,
                                        uint32_t expectedGeneration,
                                        uint64_t requestId,
                                        uint32_t layoutVersion) {
    std::lock_guard lock(g_measureMutex);
    if (g_unloading.load(std::memory_order_relaxed)) {
        return ScrollerCommitState::Obsolete;
    }
    if (expectedGeneration != 0 &&
        g_openGeneration.load(std::memory_order_relaxed) != expectedGeneration) {
        return ScrollerCommitState::Obsolete;
    }
    if (requestId != 0 &&
        !MeasureRequestLiveLocked(expectedGeneration, requestId)) {
        return ScrollerCommitState::Obsolete;
    }
    if (g_panelOwned.load(std::memory_order_relaxed)) {
        return ScrollerCommitState::Owned;
    }
    if (g_layoutVersion.load(std::memory_order_acquire) != layoutVersion) {
        return ScrollerCommitState::Moved;
    }
    std::lock_guard scrollerLock(g_scrollerMutex);
    g_scroller = answer;
    return ScrollerCommitState::Commit;
}

// Binds the retry to the open that asked for it, and bounds it. The input thread
// only re-posts while that generation is still current and the menu is still on
// screen. A generation change starts a new attempt count, so the next open gets
// its own few retries; within one open the delay doubles and then the retries
// stop, because a traversal that cannot finish will not finish at 10 Hz either.
// UI thread only.
uint32_t g_scrollerRetryCount = 0;
uint32_t g_scrollerRetryGenerationScheduled = 0;
bool g_scrollerRetryExhaustedLogged = false;

void ScheduleScrollerRetry(uint32_t generation) {
    if (generation == 0) {
        return;
    }
    if (generation != g_scrollerRetryGenerationScheduled) {
        g_scrollerRetryGenerationScheduled = generation;
        g_scrollerRetryCount = 0;
        g_scrollerRetryExhaustedLogged = false;
    }
    if (g_scrollerRetryCount >= kScrollerRetryLimit) {
        g_scrollerRetryAt.store(0, std::memory_order_relaxed);
        g_scrollerRetryGeneration.store(generation, std::memory_order_relaxed);
        if (!g_scrollerRetryExhaustedLogged) {
            g_scrollerRetryExhaustedLogged = true;
            TraceLog(L"scroller retry exhausted gen=%u", generation);
        }
        return;
    }
    const uint32_t attempt = g_scrollerRetryCount++;
    uint64_t delay = kScrollerRetryMs << std::min(attempt, 2u);
    if (delay > kScrollerRetryBackoffMaxMs) {
        delay = kScrollerRetryBackoffMaxMs;
    }
    g_scrollerRetryGeneration.store(generation, std::memory_order_relaxed);
    g_scrollerRetryAt.store(GetTickCount64() + delay, std::memory_order_relaxed);
    // The input thread may already be waiting for a later deadline; wake it so
    // its planner re-reads this one. The post is the whole signal: a null or
    // already-destroyed window fails harmlessly, and the unload flag keeps this
    // from racing a window that is being torn down.
    if (!g_unloading.load(std::memory_order_relaxed)) {
        HWND hwnd = g_hidHwnd.load(std::memory_order_relaxed);
        if (hwnd) {
            PostMessageW(hwnd, kInputWakeMessage, 0, 0);
        }
    }
}

// UI thread only: one line per open naming why a settled scroller pass could not
// commit, so a retry loop is visible in the log instead of silent.
bool g_scrollerRetryLogged = false;

// A failed settled pass. A live contact on the panel is not a failure to retry at
// 10 Hz: the input thread posts one deferred pass when ownership ends, exactly as
// it does for a layout change under the finger. A pass that lost its request or
// its open is left to the request that replaced it. Everything else is retried
// with a bounded backoff while the same open is still on screen. The attempt
// number is in the line, so a search that gives up is readable as such.
void NoteScrollerFailure(const wchar_t* reason, uint32_t generation,
                         bool retry) {
    if (!g_scrollerRetryLogged) {
        g_scrollerRetryLogged = true;
        TraceLog(L"scroller retry why=%s gen=%u try=%u/%u", reason, generation,
                static_cast<unsigned>(g_scrollerRetryCount + 1),
                static_cast<unsigned>(kScrollerRetryLimit));
    }
    if (g_panelOwned.load(std::memory_order_relaxed)) {
        g_measureDeferred.store(true, std::memory_order_relaxed);
        return;
    }
    if (retry) {
        ScheduleScrollerRetry(generation);
    }
}

void ClearScrollerRetry() {
    g_scrollerRetryAt.store(0, std::memory_order_relaxed);
}

// UI thread only. A settled pass publishes Unknown when it ends without an answer:
// the previous Nothing or Scrollable spoke for a measurement this pass could not
// repeat, and the input thread must not gate a dismiss on it. Unknown inside the
// panel only defers, so the pass fails safe.
void PublishScrollerUnknown(uint32_t generation) {
    // No region describes the cache any more, so no handler is authorized to
    // write it; the next positive commit names the regions it measured.
    std::lock_guard lock(g_scrollerMutex);
    g_scroller = ScrollerCache{};
    g_scroller.generation = generation;
    g_scroller.layoutVersion =
        g_layoutVersion.load(std::memory_order_acquire);
}

// Surveys the menu for every place a vertical drag can scroll. Runs on the UI
// thread from the open transition and again from the settled layout pass; the
// second call replaces the first survey, which can still belong to the
// storyboard.
//
// Only the settled pass can arm the gate. The open-time pass records the regions
// (so their ViewChanged handlers are attached early and their rects are logged)
// but leaves the state Unknown: the rects belong to a window the shell is still
// moving, and the old code armed the gate against it as soon as a rect existed.
//
// A settled pass that completes and finds no scrollable element records Nothing,
// not Unknown. That is the state that lets a vertical press inside the panel
// dismiss the menu, which is what a panel with genuinely nothing scrollable has
// to do; Unknown is reserved for "no whole measurement speaks for this open".
// A search that threw, that hit the depth limit, or that ran while a contact
// owned the panel is not a negative answer: it stays Unknown and schedules the
// bounded retry the input thread posts.
//
// The result is built locally and committed once, inside the commit section: the
// request, the epoch, the layout version and panel ownership are all decided and
// the cache is written before the section ends, so a close or a resize cannot
// slip between the decision and the write. Nothing is measured there.
void CacheScrollableElement(bool settled, uint32_t expectedGeneration,
                            uint64_t requestId) {
    wux::FrameworkElement root = g_root;
    if (!root) {
        return;
    }

    float originX = 0.0f;
    float originY = 0.0f;
    if (!XamlOriginDip(originX, originY)) {
        PublishScrollerUnknown(expectedGeneration);
        if (settled) {
            NoteScrollerFailure(L"origin", expectedGeneration, true);
        }
        return;
    }

    // This pass replaces the previous answer: until it commits, "not measured" is
    // the truth for this layout. The old code returned here on an incomplete
    // search and left a previous Nothing or Scrollable in place, so the gate went
    // on gating against an answer another layout (or an earlier tree) produced.
    // The layout the rects are about to be mapped in is read with them, so a
    // resize that lands while the search runs is caught at commit.
    const uint32_t layoutVersion = g_layoutVersion.load(std::memory_order_acquire);
    // The cache is about to hold no rect at all, so no handler may write it while
    // this pass runs; a positive commit names its regions again.
    {
        std::lock_guard lock(g_scrollerMutex);
        g_scroller = ScrollerCache{};
    }

    ScrollerSurvey survey;
    try {
        CollectScrollableElements(root, 0, originX, originY, survey);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Scroller search failed: %08X", e.code());
        survey.complete = false;
    } catch (...) {
        // An unreadable tree means this pass has no answer, not that the previous
        // one still holds.
        survey.complete = false;
    }

    // "Not found" is only a measurement when the traversal finished. A found
    // element is usable even if another branch was unreadable: its own values were
    // read whole, and refusing the whole survey would leave the gate Unknown for
    // certain.
    if (survey.overflow && !g_scrollerOverflowLogged) {
        g_scrollerOverflowLogged = true;
        TraceLog(L"scroller overflow (kept %d deepest regions)",
                kMaxScrollRegions);
    }
    if (survey.count == 0 && !survey.complete) {
        // Unknown so the vertical gate keeps deferring; the retry is bounded.
        PublishScrollerUnknown(expectedGeneration);
        if (settled) {
            NoteScrollerFailure(L"incomplete", expectedGeneration, true);
        }
        return;
    }

    if (survey.count == 0) {
        // Nothing scrolls here, and only the settled pass can make that a fact:
        // the open transition can see an empty tree while the shell is still
        // building the lists.
        ScrollerCache answer;
        answer.state =
            settled ? ScrollerState::Nothing : ScrollerState::Unknown;
        answer.generation = expectedGeneration;
        answer.layoutVersion = layoutVersion;
        switch (CommitScrollerCache(answer, expectedGeneration, requestId,
                                    layoutVersion)) {
            case ScrollerCommitState::Commit:
                break;
            case ScrollerCommitState::Obsolete:
                return;
            case ScrollerCommitState::Owned:
                PublishScrollerUnknown(expectedGeneration);
                if (settled) {
                    NoteScrollerFailure(L"not-allowed", expectedGeneration,
                                        false);
                }
                return;
            case ScrollerCommitState::Moved:
                PublishScrollerUnknown(expectedGeneration);
                if (settled) {
                    NoteScrollerFailure(L"commit", expectedGeneration, true);
                }
                return;
        }
        // The old handlers belong to elements this answer no longer names, so a
        // completed negative survey revokes them as well. An attached handler on a
        // viewer the panel no longer shows would keep refreshing offsets the gate
        // can never use. The cache now names no region, so it authorizes no
        // handler either; the cache was cleared before the search.
        for (int i = 0; i < kMaxScrollRegions; ++i) {
            RevokeScrollerWatch(i);
        }
        if (settled && !g_scrollerNoneLogged) {
            g_scrollerNoneLogged = true;
            TraceLog(L"scroller none (measured, nothing scrolls)");
        }
        return;
    }

    // One region per scrollable viewer, in tree order. An offset pair that cannot
    // be read drops that region only: the others were measured whole, and one
    // unreadable viewer is not a reason to leave the gate Unknown. A survey left
    // with no readable region answers Unknown like a failed pass.
    ScrollerCache answer;
    answer.state = settled ? ScrollerState::Scrollable : ScrollerState::Unknown;
    answer.generation = expectedGeneration;
    answer.layoutVersion = layoutVersion;
    wuxc::ScrollViewer answerViewers[kMaxScrollRegions]{nullptr};
    for (int i = 0; i < survey.count && answer.count < kMaxScrollRegions; ++i) {
        const ScrollerFound& found = survey.found[i];
        float verticalOffset = 0.0f;
        float scrollableHeight = 0.0f;
        try {
            verticalOffset = found.viewer.VerticalOffset();
            scrollableHeight = found.viewer.ScrollableHeight();
        } catch (...) {
            continue;
        }
        ScrollerRegion& region = answer.regions[answer.count];
        answerViewers[answer.count] = found.viewer;
        answer.count++;
        region.left = found.left;
        region.top = found.top;
        region.right = found.right;
        region.bottom = found.bottom;
        region.verticalOffset = verticalOffset;
        region.scrollableHeight = scrollableHeight;
        region.depth = found.depth;
        std::wmemcpy(region.label, found.label, kScrollerLabelChars);
    }

    if (answer.count == 0) {
        // A viewer that cannot be read is no better than no viewer, and it does
        // not tell the gate anything: this pass publishes Unknown.
        PublishScrollerUnknown(expectedGeneration);
        if (settled) {
            NoteScrollerFailure(L"offsets", expectedGeneration, true);
        }
        return;
    }

    // The client origin the rects were mapped through must still be the one in
    // force. This runs on the UI thread, which is also the origin's only writer,
    // so this is a check of the search's assumption, not a measurement of the
    // tree.
    float commitOriginX = 0.0f;
    float commitOriginY = 0.0f;
    if (!XamlOriginDip(commitOriginX, commitOriginY) ||
        commitOriginX != originX || commitOriginY != originY) {
        PublishScrollerUnknown(expectedGeneration);
        if (settled) {
            NoteScrollerFailure(L"commit", expectedGeneration, true);
        }
        return;
    }

    // The decision and the write are one section; the search above is not in it.
    switch (CommitScrollerCache(answer, expectedGeneration, requestId,
                                layoutVersion)) {
        case ScrollerCommitState::Commit:
            break;
        case ScrollerCommitState::Obsolete:
            return;  // the turn a newer request or a close replaced writes nothing
        case ScrollerCommitState::Owned:
            PublishScrollerUnknown(expectedGeneration);
            if (settled) {
                NoteScrollerFailure(L"not-allowed", expectedGeneration, false);
            }
            return;
        case ScrollerCommitState::Moved:
            PublishScrollerUnknown(expectedGeneration);
            if (settled) {
                NoteScrollerFailure(L"commit", expectedGeneration, true);
            }
            return;
    }

    // The committed regions are these elements', so these elements' handlers are
    // the ones authorized to refresh them. The watch slots move in the same
    // uninterrupted UI-thread step as the commit — no XAML call and no message
    // pump between them — so no handler can see the new rects under an old
    // element's name.
    //
    // The revoke and the new registrations are XAML calls, so they stay outside
    // the commit section; the cache is already written. A registration that
    // survived its revoke keeps its token and its owner: its slot is not reused,
    // and the old handler is not authorized for this cache anyway, so it writes
    // nothing even though it still fires.
    for (int i = 0; i < kMaxScrollRegions; ++i) {
        ScrollerWatch& watch = ScrollerWatches()[i];
        if (!watch.owner) {
            continue;
        }
        bool named = false;
        for (int r = 0; r < answer.count; ++r) {
            if (watch.owner == answerViewers[r]) {
                named = true;
                break;
            }
        }
        if (!named) {
            RevokeScrollerWatch(i);
            watch.viewer = nullptr;
        }
    }
    bool watchRefused = false;
    for (int r = 0; r < answer.count; ++r) {
        const wuxc::ScrollViewer viewer = answerViewers[r];
        int slot = -1;
        for (int i = 0; i < kMaxScrollRegions; ++i) {
            if (ScrollerWatches()[i].owner == viewer &&
                ScrollerWatches()[i].token.value) {
                slot = i;  // the same element this slot already watches
                break;
            }
        }
        if (slot < 0) {
            for (int i = 0; i < kMaxScrollRegions; ++i) {
                ScrollerWatch& watch = ScrollerWatches()[i];
                if (watch.owner || watch.token.value) {
                    continue;  // a survivor: its slot cannot be reused
                }
                watch.owner = viewer;
                watch.viewer = viewer;
                try {
                    watch.token = viewer.ViewChanged(
                        [viewer](auto const&, auto const&) {
                            RefreshScrollerOffsets(viewer);
                        });
                    if (watch.token.value) {
                        g_uiRegistrationCount.fetch_add(1,
                                                        std::memory_order_relaxed);
                        slot = i;
                    }
                } catch (winrt::hresult_error const& e) {
                    watch.token = {};
                    watch.owner = nullptr;
                    watch.viewer = nullptr;
                    TraceLog(L"scroller watch failed err=%08X",
                            static_cast<unsigned>(e.code()));
                } catch (...) {
                    watch.token = {};
                    watch.owner = nullptr;
                    watch.viewer = nullptr;
                    TraceLog(L"scroller watch failed err=unknown");
                }
                break;
            }
        }
        if (slot < 0) {
            // A region that cannot be watched does not make the survey Unknown:
            // its rect and offsets were both measured in this pass, so the gate
            // is coherent now, and Unknown would defer every press inside the
            // panel for the rest of the open. What is lost is future refresh only.
            watchRefused = true;
            continue;
        }
        ScrollerWatches()[slot].viewer = viewer;
        answer.regions[r].watchSlot = slot;
    }
    {
        // Publish the slot binding the handlers test. Written after the commit
        // and before anything can pump a message: the regions' rects are already
        // committed, and only their authorization is added here.
        std::lock_guard lock(g_scrollerMutex);
        if (g_scroller.generation == expectedGeneration &&
            g_scroller.layoutVersion == layoutVersion) {
            for (int r = 0; r < answer.count; ++r) {
                if (r < g_scroller.count) {
                    g_scroller.regions[r].watchSlot = answer.regions[r].watchSlot;
                }
            }
        }
    }
    if (watchRefused) {
        Wh_Log(L"ViewChanged registration is still attached; this pass adds no "
               L"scroll watcher");
        TraceLog(L"xaml viewchanged handler left attached; new registration "
                L"refused");
    }

    // One line per region per open. The settled pass speaks only when it moved a
    // rect the first pass logged, so a storyboard that was still growing cannot
    // hide the final geometry.
    for (int r = 0; r < answer.count; ++r) {
        const ScrollerRegion& region = answer.regions[r];
        const bool moved =
            !g_scrollerRegionLogged[r] ||
            std::fabs(region.left - g_scrollerLoggedRect[r][0]) > 1.0f ||
            std::fabs(region.top - g_scrollerLoggedRect[r][1]) > 1.0f ||
            std::fabs(region.right - g_scrollerLoggedRect[r][2]) > 1.0f ||
            std::fabs(region.bottom - g_scrollerLoggedRect[r][3]) > 1.0f;
        if (!moved) {
            continue;
        }
        g_scrollerRegionLogged[r] = true;
        g_scrollerLogged = true;
        g_scrollerLoggedRect[r][0] = region.left;
        g_scrollerLoggedRect[r][1] = region.top;
        g_scrollerLoggedRect[r][2] = region.right;
        g_scrollerLoggedRect[r][3] = region.bottom;
        TraceLog(L"scroller rect=%.0f,%.0f,%.0f,%.0f vo=%.0f sh=%.0f "
                L"name=%s depth=%u",
                region.left, region.top, region.right, region.bottom,
                region.verticalOffset, region.scrollableHeight, region.label,
                region.depth);
    }
}

// Drops every cached region, its offsets and its handler. The close path and the
// renderer teardown call it, so a closed menu can never gate a press against the
// rects of the previous open, and the next open starts from Unknown.
void ReleaseScrollableElement() {
    for (int i = 0; i < kMaxScrollRegions; ++i) {
        RevokeScrollerWatch(i);
        // The cache is emptied below, so its viewer is gone too: no surviving
        // handler may write offsets into a region that describes no element.
        ScrollerWatches()[i].viewer = nullptr;
    }
    g_scrollerLogged = false;
    g_scrollerNoneLogged = false;
    g_scrollerOverflowLogged = false;
    g_scrollerRetryLogged = false;
    g_scrollerRetryCount = 0;
    g_scrollerRetryGenerationScheduled = 0;
    g_scrollerRetryExhaustedLogged = false;
    for (int i = 0; i < kMaxScrollRegions; ++i) {
        g_scrollerRegionLogged[i] = false;
        g_scrollerLoggedRect[i][0] = 0.0f;
        g_scrollerLoggedRect[i][1] = 0.0f;
        g_scrollerLoggedRect[i][2] = 0.0f;
        g_scrollerLoggedRect[i][3] = 0.0f;
    }
    std::lock_guard lock(g_scrollerMutex);
    g_scroller = ScrollerCache{};
}

// The close's deferred cleanup names the epoch that ended. A cache already
// measured for a newer open belongs to that open and is left alone.
void ReleaseScrollableElementIfGeneration(uint32_t maxGeneration) {
    {
        std::lock_guard lock(g_scrollerMutex);
        if (g_scroller.generation > maxGeneration) {
            return;
        }
    }
    ReleaseScrollableElement();
}

// The HWND is needed for the DWM cloaked query that decides whether the menu
// is open, and for the window bounds the input thread tests against.
void CaptureCoreWindowHandle() {
    g_coreHwnd.store(nullptr, std::memory_order_relaxed);

    try {
        wucore::CoreWindow coreWindow =
            wucore::CoreWindow::GetForCurrentThread();
        if (!coreWindow) {
            return;
        }

        void* windowAbi = winrt::get_abi(coreWindow);
        if (!windowAbi) {
            return;
        }

        void* interop = nullptr;
        if (FAILED(static_cast<IUnknown*>(windowAbi)->QueryInterface(
                kCoreWindowInteropIid, &interop)) ||
            !interop) {
            return;
        }

        HWND hwnd = nullptr;
        auto getWindowHandle = reinterpret_cast<GetCoreWindowHandleAbi>(
            (*reinterpret_cast<void***>(interop))[3]);
        if (getWindowHandle) {
            getWindowHandle(interop, &hwnd);
        }
        static_cast<IUnknown*>(interop)->Release();
        g_coreHwnd.store(hwnd, std::memory_order_relaxed);
    } catch (...) {
        // No HWND means no cloaked query, so no gesture starts.
    }
}

// Chooses the dismiss direction from the taskbar itself. Runs on the UI thread,
// from Setup, from every open and from the work-area/display broadcasts.
// `logAxis` writes the full measurement line: the refresh passes false and
// speaks only when the axis actually moved, so an unchanged layout does not
// repeat four rect values on every open. A probe that cannot name an edge keeps
// the last answer and says so once per undecided episode; a wrong axis is a
// visible regression, a stale one that was right is not.
void UpdateDismissAxis(bool logAxis) {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return;
    }

    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return;
    }

    const RECT& work = info.rcWork;
    const RECT& monitorRect = info.rcMonitor;

    // The window's own rect is not an input: it is full screen while the menu is
    // idle and slides across the screen while the shell animates it in, so it
    // describes the storyboard, not the taskbar. The probe reads the sources that
    // do describe the taskbar, and its result is kept for the publish this same
    // turn runs, so the axis and the panel band come from one measurement.
    const TaskbarProbe probe =
        ProbeTaskbar(info, DpiScaleForWindow(hwnd));
    // A probe that named nothing keeps the last one for this monitor: the edge in
    // force is the last one a source named, so the band that describes it is still
    // the right band. Only a monitor change makes an older probe meaningless.
    if (probe.decided || g_taskbarProbeMonitor != monitor) {
        g_taskbarProbe = probe;
        g_taskbarProbeMonitor = monitor;
    }

    const int32_t previousAxis = g_dismissAxis;
    if (probe.decided) {
        g_dismissAxis = static_cast<int32_t>(probe.edge);
        g_axisKnown = true;
    }
    // else: keep g_dismissAxis. It is the last edge a source named, or the
    // initializer before the first answer, and the log line below says which.

    bool log = logAxis || g_dismissAxis != previousAxis;
    if (!probe.decided) {
        // One line per undecided episode, not one per open: the sources are read
        // again on the next open and on the next broadcast.
        if (g_axisKeptLogged) {
            log = false;
        } else {
            g_axisKeptLogged = true;
            log = true;
        }
        if (log) {
            TraceLog(L"dismiss axis=%d kept, no source named an edge "
                    L"(0=left,1=top,2=right,3=bottom) %s "
                    L"appbar=%d rc=%d,%d,%d,%d bar=%d rect=%d,%d,%d,%d "
                    L"work=%d,%d,%d,%d mon=%d,%d,%d,%d",
                    g_dismissAxis, g_axisKnown ? L"last-known" : L"initializer",
                    probe.appBar ? static_cast<int>(probe.appBarEdge) : -1,
                    static_cast<int>(probe.appBarRectPx.left),
                    static_cast<int>(probe.appBarRectPx.top),
                    static_cast<int>(probe.appBarRectPx.right),
                    static_cast<int>(probe.appBarRectPx.bottom),
                    probe.barWindow ? static_cast<int>(probe.barWindowEdge) : -1,
                    static_cast<int>(probe.barWindowRectPx.left),
                    static_cast<int>(probe.barWindowRectPx.top),
                    static_cast<int>(probe.barWindowRectPx.right),
                    static_cast<int>(probe.barWindowRectPx.bottom),
                    static_cast<int>(work.left), static_cast<int>(work.top),
                    static_cast<int>(work.right), static_cast<int>(work.bottom),
                    static_cast<int>(monitorRect.left),
                    static_cast<int>(monitorRect.top),
                    static_cast<int>(monitorRect.right),
                    static_cast<int>(monitorRect.bottom));
        }
    } else {
        g_axisKeptLogged = false;
        if (log) {
            TraceLog(L"dismiss axis=%d (0=left,1=top,2=right,3=bottom) src=%s "
                    L"appbar=%d rc=%d,%d,%d,%d bar=%d rect=%d,%d,%d,%d "
                    L"work=%d,%d,%d,%d mon=%d,%d,%d,%d",
                    g_dismissAxis, probe.source,
                    probe.appBar ? static_cast<int>(probe.appBarEdge) : -1,
                    static_cast<int>(probe.appBarRectPx.left),
                    static_cast<int>(probe.appBarRectPx.top),
                    static_cast<int>(probe.appBarRectPx.right),
                    static_cast<int>(probe.appBarRectPx.bottom),
                    probe.barWindow ? static_cast<int>(probe.barWindowEdge) : -1,
                    static_cast<int>(probe.barWindowRectPx.left),
                    static_cast<int>(probe.barWindowRectPx.top),
                    static_cast<int>(probe.barWindowRectPx.right),
                    static_cast<int>(probe.barWindowRectPx.bottom),
                    static_cast<int>(work.left), static_cast<int>(work.top),
                    static_cast<int>(work.right), static_cast<int>(work.bottom),
                    static_cast<int>(monitorRect.left),
                    static_cast<int>(monitorRect.top),
                    static_cast<int>(monitorRect.right),
                    static_cast<int>(monitorRect.bottom));
        }
    }

    // The snapshot is not published here: the caller publishes the whole
    // measurement once, after the travel it needs is computed.
}

// Refreshes the dismiss axis and the translation the composition expression
// bakes in. Runs on the UI thread when the menu opens and when the system
// announces a work-area or display change.
//
// `StartUI::App::PrepareRootElement` does not fire on every open: the XAML root
// is reused, so the axis, Distance, DirX and DirY captured by Setup can belong
// to a taskbar that has moved since, and the expression would keep sliding the
// panel the old way. The refresh rewrites all three composition inputs when the
// axis or the extent changed; an unchanged layout keeps the scalars the property
// set already holds and only records the check.
//
// It publishes nothing: the caller follows it with one PublishPanelTrailDip, so
// the scalars and the snapshot cannot disagree.
//
// A root that is not attached has no renderer to write: the refresh is a no-op
// then.
bool RefreshGestureGeometry() {
    wux::FrameworkElement root = g_root;
    if (!root) {
        return false;
    }

    {
        std::lock_guard lock(g_compositionMutex);
        if (!g_state) {
            return false;
        }
    }

    const int32_t previousAxis = g_dismissAxis;
    const float previousDistance = g_dismissDistance;
    UpdateDismissAxis(false);

    float dirX = 0.0f;
    float dirY = 0.0f;
    float distance = 0.0f;
    AxisTranslation(g_dismissAxis, static_cast<float>(root.ActualWidth()),
                    static_cast<float>(root.ActualHeight()), dirX, dirY,
                    distance);
    if (!(distance > 0.0f)) {
        // The root has not been laid out yet. Restore the axis so a failed read
        // does not leave the hit test on a side the expression never took.
        g_dismissAxis = previousAxis;
        return false;
    }

    const bool previouslyInvalid =
        g_rendererInvalid.load(std::memory_order_acquire);
    const bool changed = previouslyInvalid || g_dismissAxis != previousAxis ||
                         !SameLayoutDistance(distance, previousDistance);

    {
        std::lock_guard lock(g_compositionMutex);
        if (!g_state) {
            g_dismissAxis = previousAxis;
            return false;
        }
        if (changed) {
            try {
                g_state.InsertScalar(L"Distance", distance);
                g_state.InsertScalar(L"DirX", dirX);
                g_state.InsertScalar(L"DirY", dirY);
            } catch (winrt::hresult_error const& e) {
                TraceLog(L"gesture geometry write failed err=%08X",
                        static_cast<unsigned>(e.code()));
                // The expression still slides by the old axis and travel, so the
                // published snapshot must keep describing them. A half-applied
                // write is rolled back as far as the scalar the geometry owns,
                // and the renderer stays invalid: the property set may carry a
                // mixture of two layouts, so no gesture may start against it
                // until a later apply rewrites all three.
                g_dismissAxis = previousAxis;
                SetRendererInvalid(true);
                TraceLog(L"gesture renderer invalid until a successful reapply");
                return false;
            } catch (...) {
                TraceLog(L"gesture geometry write failed err=unknown");
                g_dismissAxis = previousAxis;
                SetRendererInvalid(true);
                TraceLog(L"gesture renderer invalid until a successful reapply");
                return false;
            }
        }
    }

    // Only now, with the composition carrying the new scalars, is the axis and
    // travel committed as the renderer geometry. A failed write above leaves the
    // old values in place, so no reader can follow a layout the expression does
    // not use. A changed identity also raises the layout token here, before any
    // publish: until a snapshot carries it, a cached copy from the old layout is
    // rejected for a new gesture instead of starting from stale travel.
    //
    // The distance is the *applied* value: it advances only when the scalars were
    // written. Advancing it on an unchanged check let a sequence of sub-epsilon
    // moves (each within the 0.5 DIP tolerance of the last) walk the cache away
    // from what the compositor holds, and the renderer then never received any of
    // them.
    if (changed) {
        g_dismissDistance = distance;
        g_layoutVersion.fetch_add(1, std::memory_order_acq_rel);
    }
    if (previouslyInvalid) {
        // This pass rewrote all three scalars from one layout again; the
        // property set and the committed travel agree from here.
        SetRendererInvalid(false);
    }

    // The published snapshot has to match the scalars the expression reads, so it
    // is written by the caller right after this, never on its own.
    TraceLog(changed ? L"gesture geometry updated axis=%d dist=%.0f"
                    : L"gesture geometry axis=%d dist=%.0f (unchanged)",
            g_dismissAxis, distance);
    return true;
}

// ---------------------------------------------------------------------------
// Runs on the UI thread, after the root element is loaded.
bool Setup(wux::FrameworkElement const& root) {
    if (!root) {
        return false;
    }

    {
        std::lock_guard lock(g_compositionMutex);
        if (g_state) {
            return true;
        }
    }

    g_uiThreadId.store(GetCurrentThreadId(), std::memory_order_relaxed);
    g_root = root;
    g_panelTrailLogged = false;  // one reveal trail line per open

    CaptureCoreWindowHandle();
    if (!g_coreHwnd.load(std::memory_order_relaxed)) {
        Wh_Log(L"No CoreWindow HWND; gestures stay disabled");
    }
    UpdateDismissAxis(true);

    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(root, true);

    wuc::Visual visual = wuxh::ElementCompositionPreview::GetElementVisual(root);
    wuc::Compositor compositor = visual.Compositor();

    const float width = static_cast<float>(root.ActualWidth());
    const float height = static_cast<float>(root.ActualHeight());

    // The first geometry of this renderer. The open transition and every
    // work-area change after it refresh these three values in place.
    float dirX = 0.0f;
    float dirY = 0.0f;
    float distance = 0.0f;
    AxisTranslation(g_dismissAxis, width, height, dirX, dirY, distance);

    wuc::CompositionPropertySet state = compositor.CreatePropertySet();
    state.InsertScalar(L"Progress", 1.0f);  // Normal Start is visible.
    state.InsertScalar(L"Distance", distance);
    state.InsertScalar(L"DirX", dirX);
    state.InsertScalar(L"DirY", dirY);

    // The translation property must exist before the expression is started.
    visual.Properties().InsertVector3(L"Translation",
                                      float3{0.0f, 0.0f, 0.0f});

    wuc::ExpressionAnimation translation = compositor.CreateExpressionAnimation(
        L"Vector3((1 - state.Progress) * state.DirX * state.Distance, "
        L"(1 - state.Progress) * state.DirY * state.Distance, 0)");
    translation.SetReferenceParameter(L"state", state);
    visual.StartAnimation(L"Translation", translation);

    {
        // Captured before the lock: the reference-counted copy is made outside
        // the section the compositor callback also takes.
        wucore::CoreDispatcher dispatcher = root.Dispatcher();
        std::lock_guard lock(g_compositionMutex);
        g_compositor = compositor;
        g_visual = visual;
        g_state = state;
        g_translationExpression = translation;
        // Published under the mutex every reader copies it from: the input
        // thread posts through CopyDispatcher.
        g_dispatcher = dispatcher;
    }


    // The property set now carries this travel, so the renderer geometry may be
    // committed to it. A throw above leaves the old value and no renderer.
    g_dismissDistance = distance;
    SetRendererInvalid(false);

    // A resize changes the extent the expression slides by, and it may be the
    // first real layout after Setup. The refresh owns the axis check as well, so
    // a root that is laid out again after a taskbar move is corrected here too.
    //
    // A registration that survived its revoke keeps its token and its owner:
    // overwriting them would lose the only handle on a handler that can still
    // fire. This root then gets no resize handler, and the unload waits for it
    // instead of pretending it is gone.
    if (g_sizeToken.value) {
        Wh_Log(L"SizeChanged registration from the previous root is still "
               L"attached; this root gets no resize handler");
        TraceLog(L"xaml size handler left attached; new registration refused");
    } else {
        const uint32_t sizeGeneration =
            g_setupGeneration.load(std::memory_order_relaxed);
        g_sizeRoot = root;
        g_sizeToken = root.SizeChanged(
            [sizeGeneration](winrt::Windows::Foundation::IInspectable const&,
                             wux::SizeChangedEventArgs const&) {
                try {
                    // A callback from a root this renderer no longer owns must
                    // touch nothing: its revoke failed once, and the state it
                    // would measure belongs to a newer layout.
                    if (sizeGeneration !=
                        g_setupGeneration.load(std::memory_order_relaxed)) {
                        return;
                    }
                    // A contact owns the panel: the edge must not move under the
                    // finger, so defer the whole layout change instead of
                    // re-anchoring it. The input thread posts the deferred
                    // settled pass when the contact ends. The reservation below
                    // is the authoritative check: a finger that claims the
                    // panel while this callback is entering makes the whole
                    // apply wait for the next deferred pass.
                    if (g_panelOwned.load(std::memory_order_relaxed)) {
                        g_measureDeferred.store(true,
                                                std::memory_order_relaxed);
                        // The deferred settled pass is this open's measurement
                        // turn, so it is the one that writes the once-per-open
                        // trail line.
                        g_panelTrailLogged = false;
                        TraceLog(L"gesture geometry deferred "
                                L"reason=panel-owned gen=%u",
                                g_openGeneration.load(
                                    std::memory_order_relaxed));
                        return;
                    }
                    LayoutApplyGuard apply;
                    if (!apply.Begin()) {
                        g_measureDeferred.store(true,
                                                std::memory_order_relaxed);
                        g_panelTrailLogged = false;
                        TraceLog(L"gesture geometry deferred "
                                L"reason=panel-owned gen=%u",
                                g_openGeneration.load(
                                    std::memory_order_relaxed));
                        return;
                    }
                    if (!RefreshGestureGeometry()) {
                        return;
                    }
                    // The root was resized by the panel layout, so the rect is
                    // remeasured with the new extent in force. The pass is
                    // synchronous with the layout event, so it needs no epoch of
                    // its own: the snapshot it writes carries the current one.
                    PublishPanelTrailDip(true, 0, 0);
                    // The list sits in the same panel, so a resize moves its
                    // rect as well. Re-measuring only the panel left the old
                    // scroller rect eligible for the rest of this open, and a
                    // press outside the new list read as inside the old one. The
                    // pass is settled: the size is a fact of this layout, and an
                    // incomplete search publishes Unknown and retries.
                    CacheScrollableElement(true,
                                           g_openGeneration.load(
                                               std::memory_order_relaxed),
                                           0);
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"SizeChanged update failed: %08X", e.code());
                } catch (...) {
                    // Never let an exception escape into XAML.
                }
            });
        if (g_sizeToken.value) {
            g_uiRegistrationCount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // Everything the input thread reads is published before the root opens. This
    // is the initial anchor: the frame can still be off its settled position
    // here, so the open transition re-measures it, and the measurement below is
    // rejected as "not the panel" when the storyboard has not placed it.
    PublishPanelTrailDip(true, 0, 0);
    SetRendererReady(true);

    const winrt::hstring rootClassName = winrt::get_class_name(root);
    TraceLog(L"attached root=%s size=%.0fx%.0f", rootClassName.c_str(), width,
            height);

    Wh_Log(L"Dismiss input armed; composition translation enabled "
           L"root=%s (distance=%.1f)",
           rootClassName.c_str(), distance);
    return true;
}

// ---------------------------------------------------------------------------
// Hook: StartUI::App::PrepareRootElement
// ---------------------------------------------------------------------------

using PrepareRootElement_t = void* (*)(void*, void*, bool);
PrepareRootElement_t PrepareRootElement_Original = nullptr;

void* PrepareRootElement_Hook(void* frame, void* perFrameData, bool flag) {
    void* result = PrepareRootElement_Original
                       ? PrepareRootElement_Original(frame, perFrameData, flag)
                       : nullptr;
    if (!result || g_unloading.load(std::memory_order_relaxed)) {
        return result;
    }

    try {
        wux::UIElement element{nullptr};
        winrt::copy_from_abi(element, result);  // Adds a reference.

        wux::FrameworkElement root = element.try_as<wux::FrameworkElement>();
        if (!root) {
            Wh_Log(L"PrepareRootElement returned a non-FrameworkElement root");
            return result;
        }

        Wh_Log(L"PrepareRootElement root class=%s name=%s",
               winrt::get_class_name(root).c_str(), root.Name().c_str());

        // A new open: drop the previous root's property set, animation and
        // gesture state before installing the fresh renderer.
        ResetRenderer();

        // The dispatcher is UI-thread state and the only way to revoke a
        // delegate registered below if Setup never runs. Capture it here, under
        // the same mutex every reader takes, before the first registration.
        try {
            wucore::CoreDispatcher dispatcher = root.Dispatcher();
            if (dispatcher) {
                std::lock_guard lock(g_compositionMutex);
                g_dispatcher = dispatcher;
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Dispatcher capture failed: %08X", e.code());
        } catch (...) {
            Wh_Log(L"Dispatcher capture failed with an unknown exception");
        }

        bool alreadyLoaded = false;
        try {
            alreadyLoaded = root.IsLoaded();
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"IsLoaded check failed: %08X", e.code());
        }

        if (alreadyLoaded) {
            Setup(root);
        } else if (g_loadedToken.value) {
            // The previous Loaded registration survived its revoke. Its token is
            // the only handle on a handler that can still fire, so it is kept:
            // overwriting it would lose that handle for the rest of the process,
            // and the unload would wait on a handler it cannot clear. This
            // root then installs no renderer, and the log says why.
            Wh_Log(L"Loaded registration from the previous root is still "
                   L"attached; this root gets no renderer");
            TraceLog(L"xaml loaded handler left attached; new registration "
                    L"refused");
        } else {
            const winrt::hstring rootClassName = winrt::get_class_name(root);
            TraceLog(L"root pending load class=%s", rootClassName.c_str());

            const uint32_t generation =
                g_setupGeneration.load(std::memory_order_relaxed);
            auto weak = winrt::make_weak(root);
            g_loadedRoot = root;
            g_loadedToken = root.Loaded(
                [weak, generation](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    wux::RoutedEventArgs const&) {
                    try {
                        if (generation != g_setupGeneration.load(
                                              std::memory_order_relaxed)) {
                            return;
                        }
                        wux::FrameworkElement current =
                            sender.try_as<wux::FrameworkElement>();
                        if (!current) {
                            current = weak.get();
                        }
                        if (current) {
                            Setup(current);
                        }
                    } catch (winrt::hresult_error const& e) {
                        Wh_Log(L"Loaded setup failed: %08X %s", e.code(),
                               e.message().c_str());
                    } catch (...) {
                        Wh_Log(L"Loaded setup failed with an unknown exception");
                    }
                });
            if (g_loadedToken.value) {
                g_uiRegistrationCount.fetch_add(1, std::memory_order_relaxed);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"PrepareRootElement hook failed: %08X %s", e.code(),
               e.message().c_str());
    } catch (...) {
        Wh_Log(L"PrepareRootElement hook failed with an unknown exception");
    }

    return result;
}

// ---------------------------------------------------------------------------
// Root hook: StartUI::App::PrepareRootElement by symbol
// ---------------------------------------------------------------------------

// The image the notification saw, and the image an install currently owns.
// `Seen` is written by the `LoadLibraryExW` notification and read by the input
// thread; `HookModule` is the one-attempt claim that keeps two install paths
// from loading symbols at once. A failed attempt clears the claim so the input
// thread can retry the same image.
std::atomic<HMODULE> g_startUIModuleSeen{nullptr};
std::atomic<HMODULE> g_startUIHookModule{nullptr};
std::atomic<bool> g_loadHookMatchLogged{false};

// The input thread's retry schedule for the root symbol. The first symbol load
// also starts Windhawk's PDB fetch, which can take tens of seconds on a cold
// cache; a false result before it finishes is `pending`, not a refusal. 120 s is
// two full downloads at the measured ~600 KB/s for the 35 MB reference PDB, plus
// the symbol parse, and still bounded before a user concludes the feature is
// broken. Input thread only; a fresh image starts them at zero.
//
// `kRootProbeMs` is the retry interval and, through NextInputDeadlineMs, the wait
// planner's bootstrap deadline: while the answer is neither ready nor refused,
// the thread has to come back for the probe.
constexpr uint64_t kRootPendingDeadlineMs = 120000;
uint64_t g_rootProbeNextTick = 0;
uint64_t g_rootProbeFirstTick = 0;
int g_rootProbeAttempts = 0;

// Serialises the symbol install against the stop. Wh_ModBeforeUninit takes it
// before it sets g_disabled, so an install either finishes first or sees the
// stop and installs nothing. HookSymbols loads symbols and queues the hook, so
// it must not run beside the unload.
SRWLOCK g_startUIHookLock = SRWLOCK_INIT;

// Attempts the root hook. This must never run on a thread the shell owns: the
// first call starts Windhawk's PDB fetch and blocks for its duration, and
// blocking the loader thread makes the menu host look hung to the shell's
// watchdog, which then restarts it and the fetch starts over. The input thread
// drives every attempt (the init probe only runs when StartUI_ is already
// loaded, which is the warm case).
//
// Returns true only when this call installed the hook, so a caller applies the
// queued hook operations only for an install it owns. A false result is
// `pending`: the caller decides when it becomes a refusal.
bool InstallStartUIHooks(HMODULE module) {
    if (!module) {
        return false;
    }

    AcquireSRWLockExclusive(&g_startUIHookLock);
    // The lock is the stop barrier BeforeUninit takes, so a stop and this
    // install cannot interleave. The claim also marks the image this call is
    // resolving, so a refused result is retried later rather than resolved twice
    // at once.
    HMODULE expected = nullptr;
    if (!g_startUIHookModule.compare_exchange_strong(expected, module)) {
        ReleaseSRWLockExclusive(&g_startUIHookLock);
        return false;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {
        // The stop won the race. No hook may be added after BeforeUninit.
        ReleaseSRWLockExclusive(&g_startUIHookLock);
        return false;
    }
    TraceLog(L"root install begin module=%p", module);

    // The Windows 11 image is StartUI_.dll. The collection's file-name index
    // knows the component as startui.dll, so the array carries that name and no
    // module comment of its own.
    //
    // The entry's name is the exact undecorated spelling, read from the
    // reference StartUI.pdb (10.0.22621.3733) with the same DIA undecorator the
    // Windhawk symbol API matches against.
    WindhawkUtils::SYMBOL_HOOK startuiDllHooks[] = {
        {
            {L"private: static class Windows::UI::Xaml::UIElement ^ __cdecl "
             L"StartUI::App::PrepareRootElement(class Windows::UI::Xaml::"
             L"Controls::Frame ^,class StartUI::PerFrameData ^,bool)"},
            &PrepareRootElement_Original,
            &PrepareRootElement_Hook,
        },
    };
    const uintptr_t base = reinterpret_cast<uintptr_t>(module);
    const bool hooked = WindhawkUtils::HookSymbols(
        module, startuiDllHooks, ARRAYSIZE(startuiDllHooks));
    const uintptr_t resolved =
        reinterpret_cast<uintptr_t>(PrepareRootElement_Original);
    if (!hooked || !resolved) {
        // Pending, not refused: while the engine is fetching or parsing the PDB
        // this is the expected answer, and the input thread retries.
        g_startUIHookModule.store(nullptr, std::memory_order_release);
        ReleaseSRWLockExclusive(&g_startUIHookLock);
        return false;
    }

    const uintptr_t rva = resolved - base;
    g_rootHookReady.store(true, std::memory_order_release);
    // The apply belongs to this serialized install, not to the caller. A stop
    // that takes this lock between the return and a caller-side apply would
    // return while this thread still added the root hook. Applying here means
    // BeforeUninit either waits for the complete install or sees g_disabled and
    // makes this call refuse before HookSymbols.
    Wh_ApplyHookOperations();
    TraceLog(L"root install applied tid=%u", GetCurrentThreadId());
    ReleaseSRWLockExclusive(&g_startUIHookLock);

    TraceLog(L"startui root symbol resolved base=0x%llX at=0x%llX rva=0x%llX "
            L"expected=0x%llX match=%d",
            static_cast<unsigned long long>(base),
            static_cast<unsigned long long>(resolved),
            static_cast<unsigned long long>(rva),
            static_cast<unsigned long long>(kPrepareRootElementExpectedRva),
            rva == kPrepareRootElementExpectedRva ? 1 : 0);
    if (rva == kPrepareRootElementExpectedRva) {
        Wh_Log(L"StartUI::App::PrepareRootElement resolved at %p "
               L"(module+0x%llX, the reference address)",
               reinterpret_cast<void*>(resolved),
               static_cast<unsigned long long>(rva));
    } else {
        Wh_Log(L"StartUI::App::PrepareRootElement resolved at %p "
               L"(module+0x%llX; the reference is module+0x%llX)",
               reinterpret_cast<void*>(resolved),
               static_cast<unsigned long long>(rva),
               static_cast<unsigned long long>(kPrepareRootElementExpectedRva));
    }
    return true;
}

// One look for an already-loaded image, from init and AfterInit. It only
// records the module for the input thread; it must never resolve the symbol.
// Both callers run on the host's own thread, and the first symbol load blocks
// for Windhawk's PDB fetch: on a shell thread that makes the menu host look
// hung, and the root hook is then queued by a thread that cannot apply it. The
// input thread owns every resolution and its own Wh_ApplyHookOperations.
void RecordLoadedStartUIModule() {
    if (g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    HMODULE startUI = GetModuleHandleW(kStartUIModuleName);
    if (startUI) {
        g_startUIModuleSeen.store(startUI, std::memory_order_release);
    }
}

// Resolves the root hook from the mod's own input thread. Called on every loop
// tick: immediate when the module is first known, then throttled while the
// result is pending. The shell keeps pumping for the whole fetch because no
// symbol work ever runs on its threads.
void ProbeRootSymbolFromInputThread() {
    if (g_rootHookReady.load(std::memory_order_acquire) ||
        g_rootHookRefused.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    HMODULE module = g_startUIModuleSeen.load(std::memory_order_acquire);
    if (!module) {
        module = GetModuleHandleW(kStartUIModuleName);
        if (!module) {
            return;
        }
        g_startUIModuleSeen.store(module, std::memory_order_release);
    }

    const uint64_t now = GetTickCount64();
    const bool first = g_rootProbeFirstTick == 0;
    if (!first && now < g_rootProbeNextTick) {
        return;
    }
    g_rootProbeNextTick = now + kRootProbeMs;
    if (first) {
        g_rootProbeFirstTick = now;
    }

    if (InstallStartUIHooks(module)) {
        // The install applied its own queued hook under the lock.
        return;
    }
    if (g_rootHookReady.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return;
    }

    g_rootProbeAttempts++;
    const uint64_t waited = GetTickCount64() - g_rootProbeFirstTick;
    if (waited >= kRootPendingDeadlineMs) {
        g_rootHookRefused.store(true, std::memory_order_release);
        TraceLog(L"startui root symbol deadline attempts=%d waited=%llums "
                L"deadline=%llums",
                g_rootProbeAttempts, static_cast<unsigned long long>(waited),
                static_cast<unsigned long long>(kRootPendingDeadlineMs));
        TraceLog(L"startui root symbol unresolved name=StartUI::App::"
                L"PrepareRootElement (symbol, signature or hook refused)");
        Wh_Log(L"StartUI::App::PrepareRootElement could not be resolved in "
               L"StartUI_.dll within the %llu ms pending deadline; the root "
               L"hook is refused and the controller advertises no service",
               static_cast<unsigned long long>(kRootPendingDeadlineMs));
        PublishPeerGone(L"root-hook-refused");
        return;
    }
    TraceLog(L"startui root symbol pending attempt=%d waited=%llums "
            L"deadline=%llums",
            g_rootProbeAttempts, static_cast<unsigned long long>(waited),
            static_cast<unsigned long long>(kRootPendingDeadlineMs));
}

// ---------------------------------------------------------------------------
// Late load of StartUI_.dll
// ---------------------------------------------------------------------------

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original
                         ? LoadLibraryExW_Original(fileName, file, flags)
                         : nullptr;

    // Record the returned module and return. Nothing may resolve symbols here:
    // this is the shell's loader thread, and the first symbol load blocks for
    // the PDB fetch. The input thread picks the image up on its next tick.
    // Filtering on the returned module, not the requested name, keeps a refused
    // or redirected request from looking like a load; this mod never
    // force-loads StartUI_.
    if (module && module == GetModuleHandleW(kStartUIModuleName)) {
        g_startUIModuleSeen.store(module, std::memory_order_release);
        if (!g_loadHookMatchLogged.exchange(true)) {
            TraceLog(L"ll hook startui returned=%p file=%s", module,
                    fileName ? fileName : L"(null)");
        }
    }

    return module;
}

// ---------------------------------------------------------------------------
// Unload
// ---------------------------------------------------------------------------

// Runs on the UI thread. Each registration is revoked on the object it was made
// on, in its own try block: a throw on one must not lose the others.
void RevokeHandlers() {
    // The settled-trail watch and the cached scroller belong to this root, so
    // both are dropped before its reference goes away: their handlers hold XAML
    // objects and must never run against the next open.
    StopSettledTrailWatch();
    ReleaseScrollableElement();

    // A Setup that never finished leaves g_root null while the Loaded handler is
    // already attached, so each root keeps its own reference until its token is
    // revoked.
    wux::FrameworkElement root = g_root;
    wux::FrameworkElement sizeRoot = g_sizeRoot ? g_sizeRoot : root;
    wux::FrameworkElement loadedRoot = g_loadedRoot ? g_loadedRoot : root;

    if (g_sizeToken.value) {
        if (!sizeRoot) {
            Wh_Log(L"SizeChanged registration has no owner to revoke on");
        } else {
            try {
                sizeRoot.SizeChanged(g_sizeToken);
                g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
                g_sizeToken = {};
                g_sizeRoot = nullptr;
            } catch (winrt::hresult_error const& e) {
                // The token and its owner stay: the token is the only handle on
                // a handler that can still fire, and overwriting it with the
                // next registration would lose it for the process lifetime.
                Wh_Log(L"Failed to revoke SizeChanged: %08X %s", e.code(),
                       e.message().c_str());
            } catch (...) {
                Wh_Log(L"Failed to revoke SizeChanged with an unknown "
                       L"exception");
            }
        }
    } else {
        g_sizeRoot = nullptr;
    }
    if (g_loadedToken.value) {
        if (!loadedRoot) {
            Wh_Log(L"Loaded registration has no owner to revoke on");
        } else {
            try {
                loadedRoot.Loaded(g_loadedToken);
                g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
                g_loadedToken = {};
                g_loadedRoot = nullptr;
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Failed to revoke Loaded: %08X %s", e.code(),
                       e.message().c_str());
            } catch (...) {
                Wh_Log(L"Failed to revoke Loaded with an unknown exception");
            }
        }
    } else {
        g_loadedRoot = nullptr;
    }

    // Release the XAML reference on the UI thread. A surviving registration
    // keeps its own owner reference above, so this one can go.
    g_root = nullptr;
}

void ResetComposition() {
    // The batch's handler is a delegate like any other: remove it before the
    // batch and the state it closes over go away. Takes the mutex itself, so it
    // runs before the lock below. A revoke that threw keeps the batch, its token
    // and its counted entry: the handler can still run, and the unload waits
    // for it instead of freeing under it.
    const bool batchRevoked = RevokeDismissBatch();

    std::lock_guard lock(g_compositionMutex);

    // Invalidate a pending dismiss completion before stopping its animation,
    // so an old batch cannot touch a close in a newly opened menu. The recorded
    // settle curve belongs to the root that is going away.
    InvalidateDismissBatchLocked();
    g_panel.frames = 0;
    g_panel.durationMs = 0;

    try {
        if (g_state) {
            g_state.StopAnimation(L"Progress");
            g_panel.coasting.store(false, std::memory_order_relaxed);
            g_state.InsertScalar(L"Progress", kProgressRest);
            g_panel.progress.store(kProgressRest, std::memory_order_relaxed);
        }
        g_panel.target = kProgressRest;
        g_panel.velocity = 0.0f;
        g_panel.phase = PanelPhase::Idle;
        if (g_visual) {
            g_visual.StopAnimation(L"Translation");
            g_visual.Properties().InsertVector3(
                L"Translation", float3{0.0f, 0.0f, 0.0f});
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Composition cleanup failed: %08X", e.code());
    } catch (...) {
    }

    if (batchRevoked) {
        g_dismissBatch = nullptr;
        g_dismissBatchToken = {};
        g_settleLease.reset();
    }
    g_translationExpression = nullptr;
    g_compositor = nullptr;
    g_state = nullptr;
    g_visual = nullptr;
    // The renderer is gone: every gate that asks whether gestures may start
    // reads this, so a gesture cannot run against a property set that no longer
    // exists. A half-applied scalar set dies with it.
    SetRendererReady(false);
    SetRendererInvalid(false);
    // The old root's dispatcher dies with it; PrepareRootElement captures the
    // next root's before it registers anything on it.
    g_dispatcher = nullptr;

    // A root that is going away owns no geometry. The snapshot is emptied whole
    // and the epoch moves, so a request recorded for this root can never publish
    // into the next one. The measure mutex nests inside this one here, which is
    // the only place the two meet; AdvanceOpenGeneration takes it for the bump,
    // so a commit already in its critical section finishes first.
    CancelMeasureRequest();
    ClearScrollerRetry();
    g_dismissDistance = 0.0f;
    const uint32_t emptyGeneration = AdvanceOpenGeneration();
    GestureGeometry empty;
    empty.generation = emptyGeneration;
    empty.layout.axis = g_dismissAxis;
    PublishGestureGeometry(empty);

    // Invalidate a stale Loaded handler from the previous root.
    g_setupGeneration.fetch_add(1, std::memory_order_relaxed);
}

// Drops the previous root's renderer before a new open installs its own
// property set, animation and input state. Runs on the UI thread.
void ResetRenderer() {
    RevokeHandlers();
    ResetComposition();
}

// ---------------------------------------------------------------------------
// Module lifetime
// ---------------------------------------------------------------------------

// Runs on the UI thread, or directly when the unload already runs there.
// Revokes every delegate first, then restores the panel and drops the
// UI-owned objects. The counted batch entry (if one was already dispatched)
// keeps module code alive until it returns; the caller drains it below.
void TeardownUi() {
    // The input thread is joined, so nothing posts a new turn. The XAML
    // handlers go first: a Loaded/SizeChanged/ViewChanged delegate must not
    // survive the root it closes over.
    RevokeHandlers();
    // The settle batch's Completed handler is the one delegate the compositor
    // may have dispatched already. Revoking removes the registration; the
    // lease count proves an in-flight call is gone.
    RevokeDismissBatch();
    // Stops the animations, restores resting progress and the neutral
    // translation, and nulls the composition projections. Every user takes the
    // composition mutex, so a late handler sees null and returns.
    ResetComposition();
    g_scrollerWatches.reset();
    g_root = nullptr;
    g_loadedRoot = nullptr;
    g_sizeRoot = nullptr;
    g_layoutRoot = nullptr;
    g_dispatcher = nullptr;
    g_uiThreadId.store(0, std::memory_order_relaxed);
    std::lock_guard lock(g_compositionMutex);
    g_settleLease.reset();
}

// Posts the one teardown turn and waits for its completion, or runs it inline
// when the unload already runs on the captured UI thread. The dispatcher is
// kept through this turn. A missing dispatcher with registrations still live
// has no safe answer: the loop reports and keeps waiting rather than freeing a
// delegate that can still run.
void TeardownUiOnUiThread() {
    const DWORD uiThread = g_uiThreadId.load(std::memory_order_relaxed);
    if (uiThread == 0 || uiThread == GetCurrentThreadId()) {
        TeardownUi();
        return;
    }

    wucore::CoreDispatcher dispatcher = CopyDispatcher();
    if (!dispatcher) {
        const bool anythingRegistered =
            g_uiRegistrationCount.load(std::memory_order_acquire) != 0 ||
            g_dismissBatchToken.value != 0;
        if (!anythingRegistered) {
            return;
        }
        ULONGLONG nextReport = 0;
        for (;;) {
            const ULONGLONG now = GetTickCount64();
            if (now >= nextReport) {
                nextReport = now + 5000;
                TraceLog(L"unload teardown waiting: no dispatcher, entries=%d",
                        g_moduleEntryCount.load(std::memory_order_relaxed));
            }
            Sleep(1000);
        }
    }

    HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!done) {
        ULONGLONG nextReport = 0;
        for (;;) {
            const ULONGLONG now = GetTickCount64();
            if (now >= nextReport) {
                nextReport = now + 5000;
                TraceLog(L"unload teardown waiting: no completion event");
            }
            Sleep(1000);
        }
    }

    try {
        dispatcher.RunAsync(wucore::CoreDispatcherPriority::Normal, [done]() {
            try {
                TeardownUi();
            } catch (...) {
                // Never let an exception escape the dispatcher callback. The
                // event still releases the unload, which then waits for the
                // counted entries.
            }
            SetEvent(done);
        });
    } catch (winrt::hresult_error const& e) {
        TraceLog(L"unload teardown queue failed: %08X %s", e.code(),
                e.message().c_str());
        CloseHandle(done);
        for (;;) {
            TraceLog(L"unload teardown waiting: dispatch refused");
            Sleep(5000);
        }
    } catch (...) {
        CloseHandle(done);
        for (;;) {
            TraceLog(L"unload teardown waiting: dispatch failed");
            Sleep(5000);
        }
    }

    WaitForSingleObject(done, INFINITE);
    CloseHandle(done);
}

// Waits until no counted module entry can still run. The event is set by the
// last lease release, so this is completion signalling, not a poll for zero.
// The wait has no deadline: returning under a live delegate would free the
// image its code still runs in.
void WaitForModuleEntries() {
    ULONGLONG nextReport = 0;
    for (;;) {
        if (g_moduleEntryCount.load(std::memory_order_acquire) == 0) {
            return;
        }
        const ULONGLONG now = GetTickCount64();
        if (now >= nextReport) {
            nextReport = now + 5000;
            TraceLog(L"unload waiting for module entries=%d",
                    g_moduleEntryCount.load(std::memory_order_relaxed));
        }
        if (g_moduleEntriesGone) {
            WaitForSingleObject(g_moduleEntriesGone, 1000);
        } else {
            Sleep(1000);
        }
    }
}

BOOL RoleInit() {
    SetTraceFileName(L"renderer.log");
    RefreshTraceSetting();
    TraceLog(L"mod init");
    Wh_Log(L"Legacy Start Swipe 0.7.0 initializing (start role)");

    LoadAnimationStyle();
    LoadGestureStartSpeed();
    // The stop event the input thread's wait watches. Created before the thread
    // exists, so the unload has a signal that does not depend on the window.
    g_hidStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hidStopEvent) {
        TraceLog(L"hid stop event failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }
    // The renderer-usability event: the UI thread sets it whenever the root is
    // attached, torn down or invalidated, and the input thread withdraws or
    // republishes the peer from it. A missing event leaves the loop's own check
    // as the fallback.
    g_rendererEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_rendererEvent) {
        TraceLog(L"renderer event failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }
    // The completion event for the counted module entries. Created before any
    // turn or batch completion can take a lease.
    g_moduleEntriesGone = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_moduleEntriesGone) {
        TraceLog(L"module entry event failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }
    // The cross-process channel: without it a gesture close still runs its
    // settle, but the dismiss command never reaches the bridge and the window
    // stays shown over the retracted panel.
    OpenIpcChannel();
    TraceLog(L"settle plan speed=%.2f dip/ms dur=%llu..%llu segment=%.1f ms "
            L"frames=%d..%d",
            static_cast<double>(kSettleSpeedDipPerMs),
            static_cast<unsigned long long>(kSettleMinDuration.count()),
            static_cast<unsigned long long>(kSettleMaxDuration.count()),
            kSettleSegmentMs, kSettleMinSteps + 1, kSettleMaxFrames);
    TraceLog(L"settle spring win10 zeta=%.2f", kWin10SpringDamping);
    TraceLog(L"settle spring win11 zeta=%.2f", kWin11SpringDamping);

    // The open/closed signal comes from DWM. Resolve it at runtime so a build
    // without the dwmapi import library still links and loads.
    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (dwmapi) {
        g_dwmGetWindowAttribute.store(
            reinterpret_cast<DwmGetWindowAttribute_t>(
                GetProcAddress(dwmapi, "DwmGetWindowAttribute")),
            std::memory_order_relaxed);
    }
    if (!g_dwmGetWindowAttribute.load(std::memory_order_relaxed)) {
        Wh_Log(L"DwmGetWindowAttribute not resolved; gestures stay disabled");
    }

    // The 1 ms timer period is what makes the release land 18-22 ms after the
    // last report instead of 31. Resolved at runtime; the module is kept loaded
    // for the process lifetime so the resolved pointers never dangle.
    HMODULE winmm = LoadLibraryW(L"winmm.dll");
    if (winmm) {
        g_timeBeginPeriod.store(
            reinterpret_cast<TimePeriod_t>(
                GetProcAddress(winmm, "timeBeginPeriod")),
            std::memory_order_relaxed);
        g_timeEndPeriod.store(
            reinterpret_cast<TimePeriod_t>(GetProcAddress(winmm, "timeEndPeriod")),
            std::memory_order_relaxed);
    }
    if (!g_timeBeginPeriod.load(std::memory_order_relaxed) ||
        !g_timeEndPeriod.load(std::memory_order_relaxed)) {
        Wh_Log(L"timeBeginPeriod not resolved; release keeps the default timer "
               L"quantum");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase
                              ? reinterpret_cast<LoadLibraryExW_t>(
                                    GetProcAddress(kernelBase,
                                                   "LoadLibraryExW"))
                              : nullptr;
    if (!loadLibraryExW) {
        // Without the notification the image is still recorded below and the
        // input thread installs from it; only a StartUI_ that loads later than
        // this init would be missed.
        Wh_Log(L"LoadLibraryExW not found; the late StartUI_ load is not "
               L"notified");
    } else if (!Wh_SetFunctionHook(
                   reinterpret_cast<void*>(loadLibraryExW),
                   reinterpret_cast<void*>(LoadLibraryExW_Hook),
                   reinterpret_cast<void**>(&LoadLibraryExW_Original))) {
        Wh_Log(L"Failed to hook LoadLibraryExW; the late StartUI_ load is not "
               L"notified");
    }

    // The usual case: StartUI_ is already loaded when the host creates its
    // window. Record the image here, on this host thread, and let the input
    // thread's first tick install the root hook; resolving it here would run
    // the PDB fetch on the shell's own thread.
    RecordLoadedStartUIModule();

    g_initialised = true;
    return TRUE;
}

void RoleAfterInit() {
    if (!g_initialised) {
        return;
    }
    // The LoadLibraryExW notification takes effect here. Record an image that
    // loaded during init; the input thread installs from the record, so no
    // symbol work runs on this host thread.
    Wh_ApplyHookOperations();
    RecordLoadedStartUIModule();

    TraceLog(L"hid init");
    if (!g_hidStopEvent) {
        // A gesture source that cannot be stopped is worse than no gesture:
        // unloading would have to hope the thread notices instead of
        // signalling it. The diagnostics above still name the reason.
        TraceLog(L"hid thread not started (stop event unavailable)");
        return;
    }
    g_hidThread =
        CreateThread(nullptr, 0, HidInputThread, nullptr, 0, nullptr);
    if (!g_hidThread) {
        TraceLog(L"hid thread fail err=%u",
                static_cast<unsigned>(GetLastError()));
    }
}

void RoleBeforeUninit() {
    if (!g_initialised) {
        return;
    }

    // Stop hook registration and code that can run on its own thread before
    // Windhawk removes the hooks (Wh_SetFunctionHook/Wh_ApplyHookOperations
    // must not be called after this function returns). The symbol install takes
    // the same lock, so a late StartUI_.dll load either finished before this
    // point or sees g_disabled and installs nothing.
    AcquireSRWLockExclusive(&g_startUIHookLock);
    g_disabled = true;
    ReleaseSRWLockExclusive(&g_startUIHookLock);
    g_unloading = true;

    // Stop the asynchronous dismiss-settle callback from touching state once
    // the module starts to unload. The handler reads this before it takes any
    // lock; its counted entry is what the drain waits for.
    g_stopDismissCallback = true;

    // Withdraw availability first, before anything is stopped or joined. The
    // moment the input thread is told to stop it can no longer serve an intent,
    // so the bridge must not intercept one. The same write resets the shared
    // direction and touch, so a disable in the middle of a close cannot leave
    // `closing` readable as the next activation's decision.
    PublishPeerGone(L"unload");

    // 1. Stop the input thread. The event does not depend on the window: the
    //    thread may still be between CreateThread and publishing the HWND.
    if (g_hidStopEvent) {
        SetEvent(g_hidStopEvent);
    }
    HWND hwnd = g_hidHwnd.load(std::memory_order_relaxed);
    if (hwnd) {
        // Belt to the event: a message already queued keeps the loop's
        // MsgWaitForMultipleObjects on the message branch, and this is what
        // ends it there.
        PostMessageW(hwnd, WM_QUIT, 0, 0);
    }

    // 2. Join it INFINITE, off its own thread. Nothing it touches may be freed
    //    before this returns, and the image is about to be unmapped: a thread
    //    that ignores the stop is a bug that blocks the unload, never a reason
    //    to close its handles or return success.
    if (g_hidThread) {
        if (GetCurrentThreadId() ==
            g_hidThreadId.load(std::memory_order_relaxed)) {
            TraceLog(L"unload hid thread join refused: called on that thread");
        } else if (WaitForSingleObject(g_hidThread, INFINITE) ==
                   WAIT_OBJECT_0) {
            CloseHandle(g_hidThread);
            g_hidThread = nullptr;
            TraceLog(L"unload hid thread joined");
        } else {
            // A wait that fails on a valid handle is not expected; there is
            // still no safe return while the thread can run module code.
            ULONGLONG nextReport = 0;
            for (;;) {
                const ULONGLONG now = GetTickCount64();
                if (now >= nextReport) {
                    nextReport = now + 5000;
                    TraceLog(L"unload hid thread join failed err=%u",
                            static_cast<unsigned>(GetLastError()));
                }
                Sleep(1000);
            }
        }
    } else {
        TraceLog(L"unload hid thread joined");
    }
}

void RoleUninit() {
    if (!g_initialised) {
        return;
    }

    // The bridge's interception precondition fails here, before anything below
    // is freed: from this point no intent is consumed, so no trigger may be
    // swallowed for one. The withdrawal already ran in BeforeUninit, before the
    // consumer was stopped; this repeat covers an uninit that skipped it.
    PublishPeerGone(L"uninit");

    // An uninit that skipped BeforeUninit still has the thread. Join it here
    // before touching what it runs on; the join has no deadline by design.
    if (g_hidThread) {
        if (g_hidStopEvent) {
            SetEvent(g_hidStopEvent);
        }
        if (WaitForSingleObject(g_hidThread, INFINITE) == WAIT_OBJECT_0) {
            CloseHandle(g_hidThread);
            g_hidThread = nullptr;
            TraceLog(L"unload hid thread joined (uninit)");
        }
    }

    // 3. One UI teardown turn: revoke the XAML handlers and the settle batch,
    //    restore resting progress and neutral translation, drop the UI-owned
    //    objects. Waiting for the turn is the proof the revokes ran.
    TeardownUiOnUiThread();

    // 4. Wait for a batch completion the compositor had already dispatched.
    //    The count is the proof its code is gone; the IPC view and handles stay
    //    until it returns.
    WaitForModuleEntries();

    // 5. A XAML revoke that failed leaves a delegate attached to the root it
    //    closes over. There is no safe free of that root or of this image, so
    //    the unload reports and keeps waiting; it never returns success with a
    //    mod-owned callback still reachable.
    {
        ULONGLONG nextReport = 0;
        while (g_uiRegistrationCount.load(std::memory_order_acquire) != 0) {
            const ULONGLONG now = GetTickCount64();
            if (now >= nextReport) {
                nextReport = now + 5000;
                TraceLog(L"unload waiting for xaml revokes=%d",
                        g_uiRegistrationCount.load(std::memory_order_relaxed));
            }
            Sleep(1000);
        }
    }

    ClearScrollerRetry();
    SwipeIpcClose(&g_ipc);

    if (g_hidStopEvent) {
        CloseHandle(g_hidStopEvent);
        g_hidStopEvent = nullptr;
    }
    if (g_rendererEvent) {
        CloseHandle(g_rendererEvent);
        g_rendererEvent = nullptr;
    }
    if (g_moduleEntriesGone) {
        CloseHandle(g_moduleEntriesGone);
        g_moduleEntriesGone = nullptr;
    }
    g_initialised = false;
    Wh_Log(L"Legacy Start Swipe uninitialized (start role)");
}

// A settings change only refreshes the atomics: the next settle reads them, no
// renderer state and no cached curve is touched.
void RoleSettingsChanged() {
    if (!g_initialised) {
        return;
    }
    LoadAnimationStyle();
    LoadGestureStartSpeed();
    RefreshTraceSetting();
}

// The StartMenuExperienceHost half: the touch gesture, the panel's motion and
// the intent consumer. Selected only in StartMenuExperienceHost.exe.
const SwipeModRole Role = {RoleInit, RoleAfterInit, RoleBeforeUninit,
                           RoleUninit, RoleSettingsChanged};

}  // namespace start

namespace bridge {

void EnsureExecutorWindow();
DWORD NoteLauncherThread(const wchar_t* source);
void EnsureExecutorOnLauncherThread(const wchar_t* source);
void* LauncherNow(uint64_t* stateOut, void** outerOut = nullptr);
void PublishBridgeIntent(int32_t kind, int32_t trigger, uint32_t method,
                         uint32_t flags);

// twinui.pcshell.dll is resolved by public symbol, not by address. The hook
// array in InstallExplorerHooks carries every name inline: each is the exact
// DIA undecorated name for 10.0.26100.9949, read from twinui.pcshell.pdb with
// the same undecorator the Windhawk symbol API matches, and a name is a literal
// because the store's symbol extractor reads only a literal. The decorated
// spellings are in docs/re/ and are not used.

// The reference RVAs, for the resolution log only. A different value is reported
// and the symbol is still used: the name is the contract, the address is
// evidence.
constexpr uintptr_t kRvaShow = 0x254C90;
constexpr uintptr_t kRvaHideStartAndCortana = 0x24BCB0;
constexpr uintptr_t kRvaOnStartShown = 0x141DF0;
constexpr uintptr_t kRvaOnStartHidden = 0x65E70;
constexpr uintptr_t kRvaOnHotkeyInvoke = 0x14195C;
constexpr uintptr_t kRvaOnStartButtonPressed = 0x492C90;
constexpr uintptr_t kRvaDismiss = 0x25D670;
constexpr uintptr_t kRvaShowStartView = 0x141AF0;
constexpr uintptr_t kRvaLauncherSlot = 0x948E80;
constexpr uintptr_t kRvaLauncherIsVisible = 0x1F9B40;
constexpr uintptr_t kRvaDismissStartAndCortana = 0x4991C0;
constexpr uintptr_t kRvaDismissSynchronously = 0x48F7C0;
constexpr uintptr_t kRvaActiveZBand = 0x24FC50;

// The private offsets below were read from one image: twinui.pcshell.dll fixed
// file version 10.0.26100.9974, with all 13 named symbols at the reference
// RVAs. A name alone is not the layout: a rebuild can keep every name and move
// a member or a vtable slot, and the failure mode is an Explorer crash. The
// offset-dependent capability is therefore enabled only when this exact
// identity is present; anything else is a native pass-through, never a
// null/sentinel probe.
constexpr uint64_t kTwinuiReferenceVersion =
    (static_cast<uint64_t>(10) << 48) |
    (static_cast<uint64_t>(26100) << 16) | 9974;
std::atomic<bool> g_launcherLayoutVerified{false};

// The fixed file version of a loaded module, or 0 when it cannot be read.
// version.dll is resolved at runtime, so the build gains no import.
uint64_t ModuleFixedFileVersion(HMODULE module) {
    using GetSize_t = DWORD(WINAPI*)(LPCWSTR, LPDWORD);
    using GetInfo_t = BOOL(WINAPI*)(LPCWSTR, DWORD, DWORD, LPVOID);
    using Query_t = BOOL(WINAPI*)(LPVOID, LPCWSTR, LPVOID*, PUINT);
    static HMODULE versionDll = LoadLibraryW(L"version.dll");
    static GetSize_t getSize =
        versionDll ? reinterpret_cast<GetSize_t>(
                         GetProcAddress(versionDll,
                                        "GetFileVersionInfoSizeW"))
                   : nullptr;
    static GetInfo_t getInfo =
        versionDll ? reinterpret_cast<GetInfo_t>(
                         GetProcAddress(versionDll, "GetFileVersionInfoW"))
                   : nullptr;
    static Query_t query =
        versionDll ? reinterpret_cast<Query_t>(
                         GetProcAddress(versionDll, "VerQueryValueW"))
                   : nullptr;
    if (!getSize || !getInfo || !query) {
        return 0;
    }
    WCHAR path[MAX_PATH] = {};
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return 0;
    }
    DWORD handle = 0;
    const DWORD size = getSize(path, &handle);
    if (size == 0) {
        return 0;
    }
    std::vector<BYTE> data(size);
    if (!getInfo(path, 0, size, data.data())) {
        return 0;
    }
    VS_FIXEDFILEINFO* info = nullptr;
    UINT length = 0;
    if (!query(data.data(), L"\\", reinterpret_cast<void**>(&info), &length) ||
        !info || length < sizeof(VS_FIXEDFILEINFO) ||
        info->dwSignature != 0xFEEF04BD) {
        return 0;
    }
    return (static_cast<uint64_t>(info->dwFileVersionMS) << 32) |
           info->dwFileVersionLS;
}

// The launcher keeps its IXamlLauncherState interface at +0x160 (the hotkey's
// outer object keeps the launcher at +0x18, and the same state at +0x178).
constexpr uintptr_t kLauncherStatePointerOffset = 0x160;
constexpr uintptr_t kHotkeyLauncherOffset = 0x18;
// The hotkey's special open flag: with this byte set the shell opens the tray
// surface instead of the Start view, so that press is passed through.
constexpr uintptr_t kHotkeyShellOpenFlagOffset = 0x161;
// InternalState, read from the IXamlLauncherState interface for the close
// transient test and for the logs. Never written, never the open truth.
constexpr uintptr_t kLauncherInternalStateOffset = 0x28;

constexpr wchar_t kTwinuiModuleName[] = L"twinui.pcshell.dll";

// The Windows-key dismiss method (the shell's own measurement) and the method a
// Win+Tab close carries. Method 5 is an external takeover: it is never held and
// never animated.
constexpr uint32_t kDismissMethodWin = 1;
constexpr uint32_t kDismissMethodWinTab = 5;
constexpr uint32_t kDismissMethodStartButton = 6;
constexpr uint32_t kDismissMethodClickAway = 7;

// The executor's allowlist uses the one shared vocabulary
// (SwipeShowAllowed/SwipeDismissAllowed): a show carries method 1, 11 or 16 with
// flags under the mask the shell's ShowStartView reads; a dismiss carries 1 or 6
// and no flags; a cancel names only its target. 7 is the click-away's evidence,
// mapped to 6 by the producer. The producer checks the same vocabulary before it
// swallows a press; this check is defence in depth, and anything outside it is
// refused, never clamped into a legitimate shell action.

// The M2 retract wait and its tapped-close stamp are retired. The M3 executor
// schedules the dismiss from the duration the Start side plans, and the Start
// side owns the single suppression that keeps a heal from restoring the panel.
// The old reopen latch (`g_ownPendingOpen`, its 1200 ms window and its serve
// timer) is retired too: the live reopen path is the press latch and the Start
// side's own cancel/recovery.

// The cross-process channel (swipe_ipc_shared.h). Opened once at init. Its
// handles are closed by RoleUninit once the executor and its window are gone.
SwipeIpcChannel g_ipc;

// M3 executor: one hidden window on explorer's launcher thread. The hooks post a
// message and return; the WndProc reads the Start side's command and calls the
// launcher with an empty stack. `t_execCall` marks our own call so the primitive
// hooks pass it through instead of reading it as a shell trigger.
std::atomic<HWND> g_execWindow{nullptr};
// The thread that created the window. Written before the window is published and
// read after it, so a non-null g_execWindow carries its owner.
DWORD g_execThreadId = 0;
std::atomic<DWORD> g_launcherThread{0};
thread_local int t_execCall = 0;
HANDLE g_execStopEvent = nullptr;
HANDLE g_execWorker = nullptr;
DWORD g_execWorkerId = 0;
// The fixed window class. It is registered under this image's `&__ImageBase`,
// owned by this activation, and unregistered only after the window is gone. A
// surviving class from an earlier image is a failure, never adopted.
extern "C" IMAGE_DOS_HEADER __ImageBase;
constexpr wchar_t kExecClassName[] = L"WindhawkSwipeFor10Start.Executor";
bool g_execClassRegistered = false;
HINSTANCE g_execClassInstance = nullptr;
constexpr UINT kExecMessage = WM_APP + 0x5A;
// The private shutdown and latch messages. A generic WM_CLOSE says only "some
// window should close"; the teardown is a request this activation made about
// its own window, and the WndProc answers it only for the published HWND. The
// latch message moves the hidden event's arm onto the executor's owner thread,
// where the timer and the slot live.
constexpr UINT kExecTeardownMessage = WM_APP + 0x5B;
constexpr UINT kExecLatchMessage = WM_APP + 0x5C;
constexpr WPARAM kExecTeardownMagic = 0x53575444;  // "SWTD"
constexpr WPARAM kExecLatchMagic = 0x53574C41;     // "SWLA"
constexpr UINT_PTR kExecDismissTimer = 0x5358;
constexpr UINT_PTR kExecProbeTimer = 0x5359;
constexpr UINT_PTR kPressLatchTimer = 0x535A;
constexpr UINT_PTR kExecDismissProbeTimer = 0x535B;
constexpr uint64_t kPressLatchTimeoutMs = 400;
constexpr uint64_t kPressLatchRetryMs = 100;
// The controller's own budget for one command (its kBridgeAckTimeoutMs): a show
// is answered within 400 ms of its publication, a dismiss within 400 ms of the
// deadline it was given. A dispatch that starts after that is stale — the
// publisher has already cleared its sequence and released ownership — so the
// record is answered expired and the primitive is not called. A cancelled or
// superseded record is unreachable anyway; this is the race the publisher cannot
// close, because its timeout and the executor's timer are two threads.
constexpr uint64_t kExecExpiryMs = 400;
// A dismiss ack reports the primitive, not the hide. `visible` right after the
// call can still be 1 (the shell hides on its own schedule), so the ack keeps the
// answer and this probe reads it once more; a still-visible window at the probe is
// logged as unconfirmed instead of being reported as a hide.
constexpr UINT kExecDismissConfirmMs = 300;
// Set once the worker runs. Every interception needs it: an intent whose command
// nobody executes would be swallowed while the controller waits for a show or a
// dismiss that never comes. A failed worker startup leaves it false and the
// shell's own path serves every trigger.
std::atomic<bool> g_execReady{false};
// Set by WM_NCDESTROY on the launcher thread when the published window is
// destroyed. The unload resets it before posting the teardown and waits on it
// without a deadline, so the window and its timers are gone before the class is
// unregistered and the channel closed.
HANDLE g_execWindowGone = nullptr;
// The hidden event does not publish the latched open from its own stack. A show
// that ran at the hidden event's own tick was swallowed by the completing hide
// (measured on the auto-hidden layout: `own state event=hidden`, show and
// `exec show` in the same millisecond, `exec visible ... visible=0`, state
// stuck at 3, no Shown); a show 16 ms after the event opened normally. The
// timer fires in the message loop after the event has unwound; 50 ms keeps a
// margin over the measured 16 ms.
constexpr uint64_t kPressServeDelayMs = 50;
constexpr uint64_t kLauncherStateReadyFloor = 0x10000;

using LauncherIsVisibleFn = HRESULT(WINAPI*)(void*, BOOL*);

// Resolved once by symbol. `g_launcherSlot` is the address of the s_pLauncher
// pointer slot; the singleton is read from it on each execution, on its owner
// thread, and never cached. `g_launcherIsVisible` is the visibility primitive.
void** g_launcherSlot = nullptr;
LauncherIsVisibleFn g_launcherIsVisible = nullptr;

LONG64 g_pendingDismissSeq = 0;   // UI thread only
LONG64 g_pendingShowSeq = 0;       // UI thread only
// The dismiss that returned success and is still owed its visibility
// confirmation. Zero means none; the probe timer reads it 300 ms after the call.
LONG64 g_pendingDismissConfirmSeq = 0;  // launcher thread only
int32_t g_pendingDismissConfirmVisible = 0;  // launcher thread only
// When the confirmation probe was armed, so the log prints the elapsed time it
// actually took instead of the requested delay.
uint64_t g_pendingDismissConfirmTick = 0;  // launcher thread only
// The activation that published the dismiss this window is armed for. A Start
// restart inside the execution budget would otherwise let the old record hide a
// menu the new activation just opened.
uint32_t g_pendingDismissEpoch = 0;  // launcher thread only
// The activation that published the record now being dispatched. Compared
// immediately before the launcher call: the record cannot carry it without a
// channel field, and reading it at acceptance is enough because the only way the
// publisher changes is a new attach, which increments the epoch first.
uint32_t g_dispatchEpoch = 0;  // launcher thread only
// The launcher thread's watermark: the last executor record it accepted, for
// every kind. The worker posts a tokenless message per Exec signal, and an
// auto-reset event can deliver the same record twice; without the watermark a
// duplicate post would run a show or a dismiss a second time. A delayed dismiss
// is marked accepted when it is armed, and its timer runs the record directly
// (ExecuteDueDismiss does not consult the watermark).
LONG64 g_execAcceptedSeq = 0;  // launcher thread only
// A bounded retry for a dismiss record that could not be read at its deadline.
int g_dismissReadRetries = 0;  // launcher thread only
constexpr int kExecDismissReadRetries = 10;
constexpr uint32_t kExecDismissRetryMs = 10;

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

// The twinui.pcshell.dll image the notification saw, the image an install
// currently owns, and the resolution outcome. The bridge's worker probes the
// recorded module; a false result while Windhawk's PDB fetch runs is pending,
// not a refusal.
std::atomic<HMODULE> g_explorerModuleSeen{nullptr};
std::atomic<HMODULE> g_explorerSymbolClaim{nullptr};
std::atomic<bool> g_explorerSymbolsReady{false};
std::atomic<bool> g_explorerSymbolsRefused{false};
std::atomic<bool> g_explorerLoadLogged{false};
std::atomic<bool> g_afterInit{false};
std::atomic<bool> g_disabled{false};
// Serialises the symbol install against the stop: Wh_ModBeforeUninit takes it
// before it sets g_disabled, so a fetch that is still running either finishes
// first or the install sees the stop and adds nothing.
SRWLOCK g_explorerSymbolLock = SRWLOCK_INIT;
// The worker's retry schedule. Worker thread only; a fresh image starts these at
// zero. The deadline covers the 33 MB twinui.pcshell.pdb
// (the measured download took 180 s) and one retry after a failed parse; it is
// printed on every pending line.
constexpr uint64_t kExecSymbolProbeMs = 100;
constexpr uint64_t kExecSymbolPendingDeadlineMs = 240000;
uint64_t g_execProbeNextTick = 0;
uint64_t g_execProbeFirstTick = 0;
int g_execProbeAttempts = 0;
// Set only after Wh_SetFunctionHook accepted the close-decision hook, so no
// close is ever arbitrated on a build where that hook was refused.
std::atomic<bool> g_closeHookEnabled{false};

// Set at the end of RoleInit; a fresh image starts false. The later callbacks
// return at once until the role actually initialised.
bool g_initialised = false;

// ---------------------------------------------------------------------------
// Logging (state transitions only, never per frame)
// ---------------------------------------------------------------------------

// Appends to <mod storage>\bridge.log. One line per decision or executor
// event, never per frame.
// ---------------------------------------------------------------------------
// twinui.pcshell.dll hooks
// ---------------------------------------------------------------------------

// CStartExperienceManager::Show: the shell's own open, and the touch-first
// path's first launcher entry. The executor window is created here when no
// Win/Start-button hook has run yet; the Show itself is never held.
using Show_t = HRESULT (*)(void*, uint8_t, uint8_t, uint32_t);
Show_t Show_Original = nullptr;

HRESULT Show_Hook(void* self, uint8_t a, uint8_t b, uint32_t c) {
    TraceLog(L"show a=%u b=%u c=%u tid=%u", static_cast<unsigned int>(a),
            static_cast<unsigned int>(b), static_cast<unsigned int>(c),
            GetCurrentThreadId());
    // The manager's Show runs on the launcher thread. It is the touch-first
    // path's first launcher entry: creating the window here keeps a band open
    // working when no Win/Start-button hook ran yet. The gate adopts the thread
    // only when no entry has been seen, and never creates a second window on a
    // thread that is not that one.
    EnsureExecutorOnLauncherThread(L"show");

    return Show_Original ? Show_Original(self, a, b, c) : S_OK;
}

using HideStartAndCortana_t = void (*)(void*, uint32_t);
HideStartAndCortana_t HideStartAndCortana_Original = nullptr;
// ---------------------------------------------------------------------------
// The Start toggle this mod owns
// ---------------------------------------------------------------------------
//
// The shell's launcher decides open vs close from InternalState, and while a
// close runs it still calls state 9/11 "visible", so a second press is swallowed
// instead of reversing the close. This mod therefore keeps its own boolean and
// calls the shell's own open/close primitives itself:
//
// - truth is the window's cloak (IsStartWindowVisible, vtable +0x70), never
//   InternalState: state 9/11 is "closing", not "open";
// - the Shown/Hidden events mirror the boolean between presses, and every
//   decision re-reads the cloak, so a divergence is one `own state resync` line
//   and never a stuck toggle;
// - a press while the menu is open and settled closes it; a press while it is
//   closing (state 9/11) asks for an open again, whose immediate call is latched
//   in case the close's Hidden event lands last;
// - the close itself is executed in one place: the executor's `Dismiss` on the
//   launcher thread, from the command the Start side schedules. The decision
//   entries publish the intent; the HideStartAndCortana hook never holds a
//   close, and an external takeover passes at once with only a resync.
//
// Nothing here writes InternalState and nothing caches a manager or a state
// pointer across calls.

// The caller category is a thread-local scope set by the wrapper hook around a
// proven caller's original call, not a return-address match. Each wrapper saves
// and restores the previous value, so nesting and re-entry are safe; an
// invocation from a function that is not wrapped stays Unknown and passes to the
// shell. The three sites measured on this layout:
//   DismissStartAndCortana            -> Ordinary (direct call at 0x499252)
//   ActiveZBandChangedForMonitor      -> ClickAway (synchronous method-7 call at
//                                        0x24FDDE, verified in the binary)
//   DismissSynchronouslyWithoutTransition -> Lock (method 7, 0x48F7FF)
// The old fourth return address, 0x4916D0, is a WRL delegate invoker with no
// public symbol: DIA names this instantiation after a CortanaExperienceManager
// typed event, but WRL shares one invoker per signature, so the name does not
// identify the event source (the RE notes record it as unidentified). The
// invoker forwards edx to vtable+0x48, the same Hide, so it cannot be classified
// by name. It is not wrapped: a Hide it reaches outside a wrapped caller stays
// Unknown and native. The measured desktop click-away runs inside the
// ActiveZBandChangedForMonitor scope above, which covers it.
enum class OwnCaller : int {
    Ordinary = 0,
    ClickAway,
    Lock,
    Unknown,
};

thread_local int t_ownCaller = static_cast<int>(OwnCaller::Unknown);

struct OwnCallerScope {
    OwnCaller previous;
    explicit OwnCallerScope(OwnCaller caller)
        : previous(static_cast<OwnCaller>(t_ownCaller)) {
        t_ownCaller = static_cast<int>(caller);
    }
    ~OwnCallerScope() { t_ownCaller = static_cast<int>(previous); }
    OwnCallerScope(const OwnCallerScope&) = delete;
    OwnCallerScope& operator=(const OwnCallerScope&) = delete;
};

const wchar_t* OwnCallerName(OwnCaller caller) {
    switch (caller) {
        case OwnCaller::Ordinary:
            return L"ordinary";
        case OwnCaller::ClickAway:
            return L"clickaway";
        case OwnCaller::Lock:
            return L"lock";
        default:
            return L"unknown";
    }
}

// The one admission rule for a Hide close intent. Method 7 is click-away
// evidence, so it is admitted only from the wrapper that proves that source; a
// method-7 Hide from any other caller is the shell's own close and passes. 1 and
// 6 are admitted from every caller that reaches this point. `reason` names a
// refusal for the decision line; the caller keeps the capability gate and the
// trigger ahead of this rule.
bool CloseIntentFromCaller(uint32_t method, OwnCaller caller,
                           const wchar_t** reason) {
    if (method == kDismissMethodClickAway && caller != OwnCaller::ClickAway) {
        if (reason) {
            *reason = L"caller-scope";
        }
        return false;
    }
    if (!SwipeCloseIntentAllowed(method)) {
        if (reason) {
            *reason = L"unsupported";
        }
        return false;
    }
    return true;
}

using DismissFn = HRESULT (*)(void*, uint32_t);
using ShowStartViewFn = HRESULT (*)(void*, uint32_t, uint32_t);

DismissFn g_dismiss = nullptr;
ShowStartViewFn g_showStartView = nullptr;

// Guards the cached toggle. Every hook runs on explorer's launcher thread, but
// the unload and a re-init run elsewhere, so the transitions are serialised; the
// lock is never held across a shell call or a wait. No launcher or state pointer
// is cached here: a delayed callback reacquires the singleton through
// LauncherNow, which is the one checked reader.
SRWLOCK g_ownLock = SRWLOCK_INIT;
bool g_ownOpen = false;

std::atomic<bool> g_ownWinEnabled{false};
std::atomic<bool> g_ownButtonEnabled{false};
std::atomic<bool> g_ownEventsEnabled{false};
std::atomic<bool> g_ownPrimitivesEnabled{false};
// The caller-classification capability. True only after every launcher symbol
// resolved and every caller wrapper is installed; without it no close can be
// attributed to the lock screen, a click-away or an ordinary close, so the hooks
// that turn a call into a decision stay pass-through and every unverified call
// goes to the shell.
std::atomic<bool> g_callersVerified{false};


// The one-shot mark a forwarded modified press leaves for the synchronous close
// it causes. Thread-local by construction: the close the original call performs
// runs on the same thread, nested inside it. The mark is consumed by the first
// matching executor frame and restored by the guard, so a later or unrelated
// call on the same thread cannot inherit it. See OnHotkeyInvoke_Hook.
thread_local uint32_t t_ownExternalMethod = 0;

struct OwnExternalForwardGuard {
    uint32_t previous;
    explicit OwnExternalForwardGuard(uint32_t method)
        : previous(t_ownExternalMethod) {
        t_ownExternalMethod = method;
    }
    ~OwnExternalForwardGuard() { t_ownExternalMethod = previous; }

    OwnExternalForwardGuard(const OwnExternalForwardGuard&) = delete;
    OwnExternalForwardGuard& operator=(const OwnExternalForwardGuard&) = delete;
};

// Consumes the mark, and only for the method it was armed with. One-shot: a
// second close nested inside the same original call is a different request and
// is decided on its own.
bool OwnTakeExternalForward(uint32_t method) {
    if (t_ownExternalMethod == 0 || t_ownExternalMethod != method) {
        return false;
    }
    t_ownExternalMethod = 0;
    return true;
}

void* OwnStateOf(void* launcher) {
    if (!launcher) {
        return nullptr;
    }
    return *reinterpret_cast<void* const*>(
        static_cast<const uint8_t*>(launcher) + kLauncherStatePointerOffset);
}

// The one readiness test for an IXamlLauncherState. The constructor leaves the
// 0x100 sentinel and a detached interface reads 0; both are dereferenced by
// OwnIsVisible and OwnInternalState, so every read of the state goes through
// this first. LauncherNow (the singleton read), the hook paths (the +0x160
// read) and the post-call resync share it, so no reader can disagree about what
// is safe.
bool LauncherStateReady(uint64_t state) {
    return state > kLauncherStateReadyFloor;
}

// The launcher state's IsStartWindowVisible (IXamlLauncherState vtable +0x70):
// the shell's own answer to "is the panel on screen", which follows the cloak.
// Refuses an unready pointer itself: the readiness floor is checked by every
// caller, but this is the one place the interface is dereferenced, and a
// post-call read can observe a state the pre-call check never saw.
bool OwnIsVisible(void* state) {
    if (!LauncherStateReady(
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state)))) {
        return false;
    }
    void** vtable = *reinterpret_cast<void***>(state);
    if (!vtable) {
        return false;
    }
    const auto fn =
        reinterpret_cast<bool (*)(void*)>(vtable[0x70 / sizeof(void*)]);
    return fn && fn(state);
}

uint32_t OwnInternalState(void* state) {
    if (!LauncherStateReady(
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state)))) {
        return 0;
    }
    return *reinterpret_cast<const volatile uint32_t*>(
        static_cast<const uint8_t*>(state) + kLauncherInternalStateOffset);
}

// The classification capability: true only when every caller wrapper (and the
// Hide hook) is installed, so a thread-local category exists for every close the
// mod animates. A capability, not a warning: without it no close is intercepted.
OwnCaller OwnCallerNow() {
    if (!g_callersVerified.load(std::memory_order_relaxed)) {
        return OwnCaller::Unknown;
    }
    return static_cast<OwnCaller>(t_ownCaller);
}

struct OwnSnapshot {
    bool ready;  // the state was ready and both fields were read
    bool open;
    uint32_t state;
};

// Reads the cloak truth before a decision and adopts it. A divergence is logged
// once, with both values; the shell is always right. An unready state (the
// constructor's 0x100 sentinel, a detached interface, or a pointer there is no
// reason to trust) is refused here, and the hook that asked passes the call to
// the shell: deciding from an unread state would be a guess about the direction.
OwnSnapshot OwnSync(void* launcher) {
    void* state = OwnStateOf(launcher);
    const uint64_t raw =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state));
    if (!LauncherStateReady(raw)) {
        return OwnSnapshot{false, false, 0};
    }
    const bool shellOpen = OwnIsVisible(state);
    const uint32_t internal = OwnInternalState(state);

    bool resync = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (shellOpen != g_ownOpen) {
            resync = true;
            g_ownOpen = shellOpen;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }

    if (resync) {
        TraceLog(L"own state resync cached=%d shell=%d", shellOpen ? 0 : 1,
                shellOpen ? 1 : 0);
    }
    return OwnSnapshot{true, shellOpen, internal};
}

// The same adoption after a close the executor passed or served. It reads the
// state the hook received, which is the IXamlLauncherState interface.
//
// The readiness floor is checked HERE, not only before the call. A forwarded
// Show/Dismiss reloads `[launcher+0x160]` after the shell returned, and that
// value is the new one: a state that detached, or the constructor's 0x100
// sentinel during an activation, was never seen by the pre-call check. An
// unready value leaves the cached observation untouched — deciding from it would
// turn "the state is gone" into "the panel is hidden".
void OwnResyncFromState(void* state) {
    const uint64_t raw =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state));
    if (!LauncherStateReady(raw)) {
        TraceLog(L"own state resync skipped reason=state-not-ready raw=0x%llX",
                static_cast<unsigned long long>(raw));
        return;
    }
    const bool shellOpen = OwnIsVisible(state);
    bool resync = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (shellOpen != g_ownOpen) {
            resync = true;
            g_ownOpen = shellOpen;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }
    if (resync) {
        TraceLog(L"own state resync cached=%d shell=%d", shellOpen ? 0 : 1,
                shellOpen ? 1 : 0);
    }
}

// OnStartShown: the state is 5 and the panel is on screen. Nothing is served
// from here: a show issued inside a shell transition is swallowed by the
// completing hide (measured), which is why the press latch serves its intent
// from the hidden event's timer instead.
void OwnEventShown() {
    bool changed = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (!g_ownOpen) {
            g_ownOpen = true;
            changed = true;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }
    if (changed) {
        TraceLog(L"own state event=shown open=1 tid=%u", GetCurrentThreadId());
    }
}

// OnStartHidden, after the original wrote the settled closed state. The menu is
// closed now, so the boolean follows. The press latch is served by the caller
// (OnStartHidden_Hook), which defers it off this event's stack.
void OwnEventHidden() {
    bool changed = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (g_ownOpen) {
            g_ownOpen = false;
            changed = true;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }
    if (changed) {
        TraceLog(L"own state event=hidden open=0 tid=%u", GetCurrentThreadId());
    }
}

// The Start side's liveness. This process creates the channel and keeps it
// mapped for its whole attachment, so a live section says only that the bridge
// runs; with the Start mod disabled the section is still there, and a gate that
// reads only the channel decides for a controller that is not listening. The
// published triple is the real signal: the state is alive and the tick is younger
// than the window. The window is six 100 ms refreshes: five consecutive refreshes
// may be missed without closing the gate, so a settle or a busy message loop is
// not mistaken for a dead controller, while a killed host restores the shell's
// own toggle within 0.6 s. That 0.6 s is a detection budget, not a delivery
// guarantee: a decision taken inside the window can still be published after the
// peer died, which is why the latch revalidates at its serve.
bool PeerAliveNow() {
    return SwipeIpcPeerAlive(&g_ipc, kSwipePeerStaleMs);
}

// The activation that answers right now. 0 on a section with no attach yet.
uint32_t PeerEpochNow() {
    return g_ipc.state ? SwipeIpcPeerEpoch(&g_ipc) : 0;
}

// The executor half of the gate: the worker that turns a command into a
// launcher call. Without it, a command the controller publishes would have no
// thread to run it.
bool BridgeCanServe() {
    return g_ipc.state != nullptr &&
           g_execReady.load(std::memory_order_acquire) &&
           !g_disabled.load(std::memory_order_relaxed);
}

// The window this activation dispatches through, on the calling thread. A window
// on another thread is never usable here: its messages belong to that thread's
// loop, and the fields a decision touches (the latch, the armed dismiss, the
// watermark) belong to this one.
bool ExecutorWindowOnThisThread() {
    if (!g_execWindow.load(std::memory_order_acquire)) {
        return false;
    }
    return g_execThreadId == GetCurrentThreadId();
}

// The recorded launcher thread, without adopting one. A Hide that reaches a
// thread this mod has not proven is never made the executor's thread by being
// the first entry it ran on: only the four decision entries adopt.
bool OnRecordedLauncherThread() {
    const DWORD known = g_launcherThread.load(std::memory_order_relaxed);
    return known != 0 && known == GetCurrentThreadId();
}

// The one gate every decision hook takes before it reads the panel. The peer is
// first: without a live controller an intent has no consumer, so deciding would
// only delay the shell's own toggle. Then a started worker, then this thread: a
// hook on another thread must not read the latch or the armed dismiss. The window
// is created here, on a known launcher entry, and never from the library-load
// hook or the worker's wait loop. Creating it takes no wait, so this is safe
// inside the hook.
//
// `epochOut` is the activation the admission was decided under. The latch stores
// this value, not a fresh read: a restart between this decision and the latch
// creation would otherwise label an old press with the new activation's epoch,
// and the serve would accept it.
bool BridgeCanServeOnLauncherThread(const wchar_t* trigger,
                                    uint32_t* epochOut) {
    if (epochOut) {
        *epochOut = 0;
    }
    if (!PeerAliveNow()) {
        // No controller: the caller forwards to the shell. One line per press
        // names the trigger and says why nothing was decided.
        TraceLog(L"own pass trigger=%s reason=no-peer", trigger);
        return false;
    }
    const uint32_t epoch = PeerEpochNow();
    if (!BridgeCanServe()) {
        TraceLog(L"own pass trigger=%s reason=no-executor", trigger);
        return false;
    }
    if (NoteLauncherThread(trigger) == 0) {
        // The thread test comes before the window test, so a hook on another
        // thread cannot reach the launcher-local fields through an existing
        // window.
        TraceLog(L"own pass trigger=%s reason=wrong-thread", trigger);
        return false;
    }
    EnsureExecutorWindow();
    if (!ExecutorWindowOnThisThread()) {
        TraceLog(L"own pass trigger=%s reason=no-window", trigger);
        return false;
    }
    if (epochOut) {
        *epochOut = epoch;
    }
    return true;
}

// The close intent's capability, one gate for the directional Hide. Its command
// is a close the Start side turns into `exec dismiss`, so it needs an executor
// window on this thread, the thread already recorded, and the Dismiss primitive
// that command will call. One missing piece and the original runs: converting a
// native close into a command nothing can execute would leave the window shown
// over the controller's retracted panel.
bool HideCanServeClose(const wchar_t** reason) {
    if (!PeerAliveNow()) {
        *reason = L"no-peer";
        return false;
    }
    if (!g_callersVerified.load(std::memory_order_relaxed)) {
        // The caller wrappers are the only thing that tells a lock screen from
        // the click-away; without them every close is unattributable and must not
        // be animated.
        *reason = L"callers-unverified";
        return false;
    }
    if (!BridgeCanServe()) {
        *reason = L"no-executor";
        return false;
    }
    if (!OnRecordedLauncherThread()) {
        // Not adopted: a Hide on an unknown thread is not evidence about the
        // launcher thread.
        *reason = L"wrong-thread";
        return false;
    }
    if (!ExecutorWindowOnThisThread()) {
        *reason = L"no-window";
        return false;
    }
    if (!g_dismiss) {
        // The primitive probe is a capability: without the callable Dismiss the
        // command has no executor even though the worker runs.
        *reason = L"no-primitive";
        return false;
    }
    return true;
}

// XamlLauncherState::HideStartAndCortana(void* self, uint32_t method)
// twinui.pcshell.dll 26100.9949, RVA 0x24BCB0. The function reads the dismiss
// method in edx, writes the transient state at [self+0x28] and calls the
// manager's Hide through [self+0x38]; it returns nothing.
void HideStartAndCortana_Hook(void* self, uint32_t method) {
    auto callOriginal = [self, method]() {
        if (HideStartAndCortana_Original) {
            HideStartAndCortana_Original(self, method);
        }
    };
    if (!g_closeHookEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        callOriginal();
        return;
    }
    if (t_execCall > 0) {
        const uint32_t execState = OwnInternalState(self);
        const OwnCaller execCaller = OwnCallerNow();
        // The pointer-away of our own Show enters HideStartAndCortana(7)
        // directly, with no wrapper scope (Unknown). A lock or a real click-away
        // on this same executor stack is not that source and must not be
        // swallowed; the caller and the depth are logged with the decision.
        if (method == kDismissMethodClickAway &&
            (execState == 1 || execState == 3) &&
            execCaller == OwnCaller::Unknown) {
            TraceLog(L"exec swallow hide caller=%s method=%u state=%u depth=%d "
                    L"reason=own-show",
                    OwnCallerName(execCaller), method, execState, t_execCall);
            return;
        }
        TraceLog(L"exec pass hide caller=%s method=%u state=%u depth=%d "
                L"reason=executor",
                OwnCallerName(execCaller), method, execState, t_execCall);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    // The close intent is a command the Start side will ask the executor to run:
    // one line names the missing capability and the shell's own close runs. Peer,
    // caller sites and the executor come from HideCanServeClose, which also keeps
    // a Hide on an unproven thread from adopting that thread.
    const wchar_t* reason = nullptr;
    if (!HideCanServeClose(&reason)) {
        TraceLog(L"exec pass hide method=%u reason=%s", method,
                reason ? reason : L"unknown");
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    const OwnCaller caller = OwnCallerNow();
    // The ActiveZBand wrapper covers its whole function; only the synchronous
    // method-7 call verified in it is the click-away. Anything else reached in
    // that scope stays unclassified and passes to the shell.
    if (caller == OwnCaller::ClickAway && method != kDismissMethodClickAway) {
        TraceLog(L"intent external trigger=hide caller=unknown method=%u "
                L"reason=unexpected-method",
                method);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                            method, 0);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }
    // External and lock precedence, before any opening or touch-owner
    // suppression: a lock, an unattributed caller, Win+Tab's method 5 and a
    // forwarded Win press are takeovers, not this mod's close, and they pass.
    if (caller == OwnCaller::Lock || caller == OwnCaller::Unknown ||
        method == kDismissMethodWinTab || OwnTakeExternalForward(method)) {
        TraceLog(L"intent external trigger=hide caller=%s method=%u",
                OwnCallerName(caller), method);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                            method, 0);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    // Every other Hide reaching this hook is a close trigger — the click-away,
    // the shell's Windows key when the decision hook passed it, or a Start-button
    // path — not a duplicate of a close the controller already runs. It is
    // published even while the controller owns the panel; the controller decides
    // whether it is a new close or a reopen.
    //
    // The one exception is the opening touch: when a contact at the band pulls
    // the menu out, the shell sees that same contact outside the freshly shown
    // window and issues its click-away close. It is the finger that opened the
    // menu, not a press outside it, and it is swallowed until the controller's
    // open has landed.
    // The opening window is not a click-away target yet. The shell issues a
    // method-7 close while the window is still appearing when the pointer is
    // outside it (a keyboard open with the mouse parked on the desktop, or the
    // pointer that opened it); that close would hide the menu the user just
    // asked for. Swallow it only in the opening states; a click-away on the
    // settled panel (state 5) is a real trigger.
    const uint32_t currentState = OwnInternalState(self);
    // Only the observed click-away source is the opening pointer-away. A
    // method-7 number alone is not: the lock and unattributed callers already
    // passed above as external, and an ordinary caller's 7 is not a click-away.
    if (caller == OwnCaller::ClickAway &&
        method == kDismissMethodClickAway &&
        (currentState == 1 || currentState == 3)) {
        TraceLog(L"intent ignore trigger=clickaway caller=%s method=%u state=%u "
                L"depth=%d reason=opening",
                OwnCallerName(caller), method, currentState, t_execCall);
        return;
    }
    if (caller == OwnCaller::ClickAway &&
        method == kDismissMethodClickAway &&
        SwipeIpcControllerTouch(&g_ipc)) {
        TraceLog(L"intent ignore trigger=clickaway caller=%s method=%u state=%u "
                L"depth=%d reason=touch-owner",
                OwnCallerName(caller), method, currentState, t_execCall);
        return;
    }

    const int32_t trigger = caller == OwnCaller::ClickAway
                                ? kSwipeTriggerClickAway
                                : kSwipeTriggerWin;
    // The close intent's method is forwarded to the executor. 7 is click-away
    // *evidence*, so only the caller scope that proves the source may carry it;
    // an ordinary caller's method-7 Hide is the shell's own close and passes.
    // Anything else outside the shared vocabulary passes too, instead of being
    // swallowed for a command that would be refused.
    const wchar_t* methodReason = nullptr;
    if (!CloseIntentFromCaller(method, caller, &methodReason)) {
        TraceLog(L"intent pass close trigger=%s caller=%s method=%u reason=%s",
                trigger == kSwipeTriggerClickAway ? L"clickaway" : L"win",
                OwnCallerName(caller), method, methodReason);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }
    PublishBridgeIntent(kSwipeIntentClose, trigger, method, 0);
    TraceLog(L"intent close trigger=%s caller=%s method=%u state=%u depth=%d",
            trigger == kSwipeTriggerClickAway ? L"clickaway" : L"win",
            OwnCallerName(caller), method, currentState, t_execCall);
}
// ---------------------------------------------------------------------------
// The decision hooks: Windows key, Start button, and the two state events
// ---------------------------------------------------------------------------

// One toggle decision for every trigger. The controller's published direction is
// the whole answer while it owns the panel: closing means a close is in flight,
// so a press reopens; an owned open means a press closes. The launcher's state
// is only the resync for a panel the controller does not own (the shell's own
// open or close), never the direction for a controller close.
//
// The shell cannot answer this: a gesture close retracts the panel while the
// window still reports InternalState 5 and the cloak still reports open, and
// only the executor's Dismiss ends that. Reading the shell there classified a
// press during the retract as a second close, and the owner's press did nothing.
// The controller publishes closing before the close's first frame precisely so
// this rule has the direction while the shell still looks open.
bool IntentShouldClose(const OwnSnapshot& own, int32_t direction) {
    if (direction != kSwipeDirectionUnowned) {
        return direction != kSwipeDirectionClosing;
    }
    if (own.open) {
        return true;
    }
    return own.state == 1 || own.state == 3 || own.state == 5;
}

// ---------------------------------------------------------------------------
// The press latch: a press that lands while the shell is already hiding
// ---------------------------------------------------------------------------
//
// The shell's state 9/11 is a hide that has claimed the panel: ShowStartView
// from there is a no-op, and the shell's own Hide for the same press does
// nothing. Neither the cancel path (the dismiss is already issued) nor a new
// show can serve the press, so it is latched here and served as the same open
// intent once the hide has landed (OnStartHidden, or the closed cloak for a
// landing the event missed). One slot; a newer press replaces the older one and
// says so. The wait is bounded by the hide, not by a fixed delay.

struct PressLatch {
    bool pending = false;
    uint64_t tick = 0;
    int32_t kind = kSwipeIntentOpen;
    int32_t trigger = kSwipeTriggerWin;
    uint32_t method = 0;
    uint32_t flags = 0;
    // The Start activation that was answering when the press was latched. A
    // serve against a different epoch is a serve into a controller that never
    // saw this press, so the slot is invalidated instead.
    uint32_t epoch = 0;
};

PressLatch g_pressLatch;  // launcher thread only
// Why the timer that will serve g_pressLatch was armed: the hidden event's
// deferral ("hidden") or the 400 ms fallback ("timeout"). The serve line names
// it. Launcher thread only, like the latch.
const wchar_t* g_pressServeReason = L"timeout";
// The longest a latch may wait for its landing. The 400 ms fallback plus one
// retry window is the whole obligation: a hide that has not landed by then is
// not this press's hide (a dismiss the shell refused, a window that stayed
// shown), and a slot kept past it would fire into whatever the user does next.
constexpr uint64_t kPressLatchMaxWaitMs = 1200;

bool ServeLatchedPress(const wchar_t* reason);

void DropPressLatch(const wchar_t* reason, const wchar_t* detail) {
    if (!g_pressLatch.pending) {
        return;
    }
    TraceLog(L"own press latch dropped reason=%s %s wait=%llu", reason, detail,
            static_cast<unsigned long long>(GetTickCount64() -
                                            g_pressLatch.tick));
    g_pressLatch = PressLatch{};
}

const wchar_t* PressTriggerName(int32_t trigger) {
    switch (trigger) {
        case kSwipeTriggerStartButton:
            return L"startbutton";
        case kSwipeTriggerClickAway:
            return L"clickaway";
        case kSwipeTriggerTouch:
            return L"touch";
        case kSwipeTriggerOverride:
            return L"override";
        default:
            return L"win";
    }
}

bool ShellHideInFlight(const OwnSnapshot& own) {
    return own.state == 9 || own.state == 11;
}

void LatchPress(const OwnSnapshot& own, int32_t trigger, uint32_t method,
                uint32_t flags, uint32_t admittingEpoch) {
    const uint64_t now = GetTickCount64();
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        // Neither the fallback timer nor the hidden event's arm has a window to
        // run on. A slot with no guaranteed callback is not armed at all.
        TraceLog(L"own press latch dropped reason=no-window state=%u",
                own.state);
        return;
    }
    if (g_pressLatch.pending) {
        TraceLog(L"own press latch replaced old_tick=%llu old_wait=%llu state=%u",
                static_cast<unsigned long long>(g_pressLatch.tick),
                static_cast<unsigned long long>(now - g_pressLatch.tick),
                own.state);
    }
    g_pressLatch = PressLatch{true,            now,  kSwipeIntentOpen,
                              trigger,         method, flags,
                              admittingEpoch};
    TraceLog(L"own press latched state=%u trigger=%s tick=%llu epoch=%u "
            L"(hide in flight)",
            own.state, PressTriggerName(trigger),
            static_cast<unsigned long long>(now), g_pressLatch.epoch);
    g_pressServeReason = L"timeout";
    if (SetTimer(window, kPressLatchTimer, kPressLatchTimeoutMs, nullptr) == 0) {
        // The hidden event is a second serve point, but it is not a guaranteed
        // one: the landing may already have passed. Today the initial timer is
        // the only promise, so a latch that cannot arm it is dropped.
        TraceLog(L"own press latch timer failed err=%u", GetLastError());
        DropPressLatch(L"timer-failed", L"latch");
    }
}

// The latch waits for the hide's own event; the event then arms this deferral
// instead of publishing from its stack. See kPressServeDelayMs.
//
// The slot and its timer are launcher-thread state. The hidden event is expected
// there, but the check is not an assumption: the owner-thread test comes before
// the first read of the latch. A callback on another thread posts one private
// notification to the executor window and the owner arms it; it never reads the
// slot itself.
void ArmLatchedServe() {
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!ExecutorWindowOnThisThread()) {
        if (!window) {
            // No window to post to. The latch's own timeout timer runs on its
            // owner thread, so there is nothing to do from here.
            return;
        }
        if (!PostMessageW(window, kExecLatchMessage, kExecLatchMagic, 0)) {
            TraceLog(L"own press latch arm post failed err=%u", GetLastError());
        } else {
            TraceLog(L"own press latch arm posted tid=%u owner=%u",
                    GetCurrentThreadId(), g_execThreadId);
        }
        return;
    }
    if (!g_pressLatch.pending) {
        return;
    }
    if (!window) {
        DropPressLatch(L"no-window", L"hidden");
        return;
    }
    g_pressServeReason = L"hidden";
    if (SetTimer(window, kPressLatchTimer,
                 static_cast<UINT>(kPressServeDelayMs), nullptr) == 0) {
        // Serving from this event's stack is the one shape measured to swallow
        // the show, so a latch that cannot be deferred is dropped instead.
        DropPressLatch(L"timer-failed", L"hidden");
    }
}

// Serves the latched press once the hide has landed. The hidden event is the
// normal reason; the timer's retry is the safety net for a landing the event
// missed (the press arrived between our settle and the shell's hidden event).
// The launcher is reacquired from the singleton on this (launcher) thread: no
// pointer borrowed from an earlier hook is held across the wait.
//
// The gate is re-read here, not only at the press. A latch armed up to a hide
// before this point outlives the peer that armed it: 600 ms is a detection
// budget, not a delivery guarantee, and a controller that died in between has no
// actor for this intent. The activation epoch answers the second half — a new
// Start activation would consume the open as its own. Both cases invalidate the
// slot instead of publishing into whatever answers now.
bool ServeLatchedPress(const wchar_t* reason) {
    if (!g_pressLatch.pending) {
        return false;
    }
    const uint64_t now = GetTickCount64();
    if (now > g_pressLatch.tick && now - g_pressLatch.tick > kPressLatchMaxWaitMs) {
        DropPressLatch(L"expired", reason);
        return false;
    }
    if (!PeerAliveNow() || !BridgeCanServe()) {
        DropPressLatch(L"peer-gone", reason);
        return false;
    }
    const uint32_t epoch = PeerEpochNow();
    if (epoch != g_pressLatch.epoch) {
        wchar_t detail[64] = {};
        swprintf_s(detail, L"latch_epoch=%u now=%u", g_pressLatch.epoch, epoch);
        DropPressLatch(L"peer-changed", detail);
        return false;
    }
    uint64_t state = 0;
    void* launcher = LauncherNow(&state);
    if (!launcher) {
        TraceLog(L"own press serve refused reason=%s launcher=0 state=0x%llX",
                reason, static_cast<unsigned long long>(state));
        return false;
    }
    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        return false;
    }
    if (own.open || ShellHideInFlight(own)) {
        // The landing has not happened. A definitely failed hide is dropped at
        // its own observation instead of here: a refused or expired dismiss in
        // ExecuteDismiss/ExecuteDueDismiss, and a still-visible window at the
        // 300 ms confirmation (`dismiss-unconfirmed`). The settled-open state is
        // not used as that test, because the retract-before-dismiss and the
        // dismiss-returned-before-cloak windows both read as a shown panel too.
        return false;
    }
    const PressLatch latch = g_pressLatch;
    g_pressLatch = PressLatch{};
    const uint64_t wait = GetTickCount64() - latch.tick;
    PublishBridgeIntent(kSwipeIntentOpen, latch.trigger, latch.method,
                        latch.flags);
    TraceLog(L"own press served wait=%llu state=%u trigger=%s reason=%s "
            L"epoch=%u",
            static_cast<unsigned long long>(wait), own.state,
            PressTriggerName(latch.trigger), reason, latch.epoch);
    return true;
}

// The one toggle dispatcher for the three physical entries: the bare Windows
// press, the Start button's open (ShowStartView method 11) and its close
// (Dismiss method 6). `showMethod`/`dismissMethod` are the methods this entry
// uses for each action; the action the direction chose keeps its own method. The
// click-away is directional and never comes through here.
//
// The Start button used to publish a close from Dismiss(6) without asking the
// direction, so a press during a retract entered as a second close while the
// controller said the close was still in flight and the owner's press did
// nothing. Both button entries now run the same rule as the Win key.
//
// Supersession lives here, not in one entry: any newer press whose panel is not
// hiding makes the older latched reopen obsolete, whichever primitive carried it.
// A physical Start-button close on a shown panel used to leave the old slot for
// the hidden event to serve.
bool DispatchToggle(const OwnSnapshot& own, int32_t trigger, uint32_t showMethod,
                    uint32_t dismissMethod, uint32_t flags,
                    uint32_t admittingEpoch) {
    if (g_pressLatch.pending && !ShellHideInFlight(own)) {
        // The landing event was missed and this newer press is the one the user
        // means.
        DropPressLatch(L"superseded", L"newer-press");
    }
    // One sample of the controller's direction drives both the decision and its
    // line. Reading it twice could log a direction the decision never used.
    const int32_t direction = SwipeIpcControllerDirection(&g_ipc);
    const int32_t kind = IntentShouldClose(own, direction) ? kSwipeIntentClose
                                                           : kSwipeIntentOpen;
    // The one admission check. The method and flags below are the ones the
    // action will publish, not necessarily the ones the shell called with: a
    // reopen from Dismiss(6) asks for Show(11), and a close from Show(11) asks
    // for Dismiss(6). A combination outside the shared vocabulary goes to the
    // original; the executor's whitelist would only refuse it after the press
    // was already swallowed.
    const uint32_t method =
        kind == kSwipeIntentClose ? dismissMethod : showMethod;
    const bool supported = kind == kSwipeIntentClose
                               ? SwipeDismissAllowed(method)
                               : SwipeShowAllowed(method, flags);
    if (!supported) {
        TraceLog(L"own pass trigger=%s state=%u open=%d kind=%s method=%u "
                L"flags=%u reason=unsupported",
                PressTriggerName(trigger), own.state, own.open ? 1 : 0,
                kind == kSwipeIntentClose ? L"close" : L"open", method, flags);
        return false;
    }
    // One decision line for every trigger. The method below is the one the
    // action will use, not the one the shell called with: a reopen from
    // Dismiss(6) asks for Show(11), and a close from Show(11) asks for
    // Dismiss(6).
    TraceLog(L"own toggle decide trigger=%s state=%u open=%d kind=%s dir=%d",
            PressTriggerName(trigger), own.state, own.open ? 1 : 0,
            kind == kSwipeIntentClose ? L"close" : L"open", direction);
    if (kind == kSwipeIntentOpen &&
        (ShellHideInFlight(own) || g_pendingDismissSeq != 0)) {
        // The hide or the armed dismiss has already taken the window; the open
        // is served at the landing instead of being published into a no-op show.
        LatchPress(own, trigger, showMethod, flags, admittingEpoch);
        return true;
    }
    PublishBridgeIntent(kind, trigger,
                        kind == kSwipeIntentClose ? dismissMethod : showMethod,
                        flags);
    return true;
}

using OnHotkeyInvoke_t = HRESULT (*)(void*, uint32_t);
OnHotkeyInvoke_t OnHotkeyInvoke_Original = nullptr;

// XamlLauncher::OnHotkeyInvoke(self, flags): the Windows key and the shell hook.
// `self` is the outer object; the launcher is `self + 0x18`. The function carries
// no key information, so a combination can only be recognised by the modifier
// state at entry. That check is a hard gate for the open direction: a modified
// press must never open Start. For the close direction, a modified press that is
// forwarded passes through as an external takeover; one that cannot be
// recognised falls back to the ordinary close, which may animate it.
HRESULT OnHotkeyInvoke_Hook(void* self, uint32_t flags) {
    uint32_t admittingEpoch = 0;
    if (!g_ownWinEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) ||
        !BridgeCanServeOnLauncherThread(L"win", &admittingEpoch)) {
        // No controller, no worker, or no window on this thread: the shell's own
        // toggle serves the press instead of an intent nobody can execute.
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    void* launcher = static_cast<uint8_t*>(self) + kHotkeyLauncherOffset;
    const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    const bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    TraceLog(L"own win enter flags=%u shift=%d ctrl=%d alt=%d tid=%u",
            flags, shift ? 1 : 0, control ? 1 : 0, alt ? 1 : 0,
            GetCurrentThreadId());

    if (shift || control || alt) {
        const OwnSnapshot own = OwnSync(launcher);
        if (!own.ready) {
            TraceLog(L"own win pass reason=state-not-ready flags=%u", flags);
            return OnHotkeyInvoke_Original
                       ? OnHotkeyInvoke_Original(self, flags)
                       : S_OK;
        }
        if (own.open) {
            PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                                kDismissMethodWin, flags);
            TraceLog(L"own win pass reason=external open=1 state=%u", own.state);
            OwnExternalForwardGuard guard(kDismissMethodWin);
            return OnHotkeyInvoke_Original
                       ? OnHotkeyInvoke_Original(self, flags)
                       : S_OK;
        }
        TraceLog(L"own win pass reason=external open=0 state=%u (forwarded)",
                own.state);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                            kDismissMethodWin, flags);
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    if (static_cast<const uint8_t*>(self)[kHotkeyShellOpenFlagOffset] != 0) {
        // The shell's own tray reopen: not the Start toggle, and it has side
        // effects this mod does not reproduce.
        TraceLog(L"own win pass reason=shell-open-flag");
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        TraceLog(L"own win pass reason=state-not-ready flags=%u", flags);
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }
    if (!DispatchToggle(own, kSwipeTriggerWin, kDismissMethodWin,
                        kDismissMethodWin, flags, admittingEpoch)) {
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }
    return S_OK;
}
using OnStartButtonPressed_t = HRESULT (*)(void*, uint32_t, uint32_t);
OnStartButtonPressed_t OnStartButtonPressed_Original = nullptr;

// XamlLauncher::OnStartButtonPressed(self, showMethod, dismissMethod): `self` is
// the launcher and the function is a pure toggle. Nothing else reaches it, so
// there is no combination to gate.
HRESULT OnStartButtonPressed_Hook(void* self, uint32_t showMethod,
                                  uint32_t dismissMethod) {
    uint32_t admittingEpoch = 0;
    if (!g_ownButtonEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) ||
        !BridgeCanServeOnLauncherThread(L"startbutton", &admittingEpoch)) {
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }

    TraceLog(L"own button enter show=%u dismiss=%u", showMethod, dismissMethod);
    const OwnSnapshot own = OwnSync(self);
    if (!own.ready) {
        TraceLog(L"own button pass reason=state-not-ready show=%u dismiss=%u",
                showMethod, dismissMethod);
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }
    if (!DispatchToggle(own, kSwipeTriggerStartButton, showMethod,
                        dismissMethod, 0, admittingEpoch)) {
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }
    return S_OK;
}

// The symbols are `long __cdecl XamlLauncherState::OnStartShown(
// IStartExperienceManager*, IInspectable*)`; the hook keeps that exact shape and
// returns the original's result, because the detour replaces the function the
// shell called.
using OnStartShown_t = HRESULT (*)(void*, void*, void*);
OnStartShown_t OnStartShown_Original = nullptr;

// `self` is the launcher state object; its IXamlLauncherState interface is
// `self + 0x18` and its InternalState is `[self + 0x40]`, which is the same
// dword the interface reads at +0x28.
HRESULT OnStartShown_Hook(void* self, void* manager, void* inspectable) {
    const HRESULT hr = OnStartShown_Original
                           ? OnStartShown_Original(self, manager, inspectable)
                           : S_OK;
    if (!g_ownEventsEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return hr;
    }
    OwnEventShown();
    return hr;
}

using OnStartHidden_t = HRESULT (*)(void*, void*, void*);
OnStartHidden_t OnStartHidden_Original = nullptr;

// Runs after the original, so the state is already the settled closed value and
// the latch's ShowStartView cannot be overwritten by this transition again.
HRESULT OnStartHidden_Hook(void* self, void* manager, void* inspectable) {
    const HRESULT hr = OnStartHidden_Original
                           ? OnStartHidden_Original(self, manager, inspectable)
                           : S_OK;
    if (!g_ownEventsEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return hr;
    }
    OwnEventHidden();
    // The landing the latch exists for. The serve is deferred off this event's
    // stack: a show requested at the hidden event's own tick was swallowed
    // (kPressServeDelayMs). If the cloak has already flipped, the press becomes
    // the same open intent a normal press would have published.
    ArmLatchedServe();
    return hr;
}

// XamlLauncher::Dismiss / XamlLauncher::ShowStartView. The taskbar's Start
// button on this build does not call OnStartButtonPressed: it calls these
// launcher methods directly (measured: the open arrives as the manager's Show
// with no decision-hook frame; the close arrives as method 6 through
// DismissStartAndCortana). These two hooks are the decision layer for such a
// caller: method 11 is the Start button's open and method 6 its close; any
// other call is swallowed while the controller owns the panel, or passed with a
// cloak resync. Our own decision calls are excluded by the thread-local guard.
//
// Both Start-button methods go through DispatchToggle, so a physical press is
// one decision whichever primitive the shell used: during a retract the same
// button can enter as Dismiss(6) while the direction says the close is in
// flight, and that press is a reopen.
using DismissHook_t = HRESULT (*)(void*, uint32_t);
DismissHook_t Dismiss_Original = nullptr;

HRESULT Dismiss_Hook(void* launcher, uint32_t method) {
    uint32_t admittingEpoch = 0;
    if (!g_ownPrimitivesEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) || t_execCall > 0) {
        return Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    }
    if (!BridgeCanServeOnLauncherThread(L"dismiss", &admittingEpoch)) {
        return Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    }
    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        TraceLog(L"exec pass dismiss method=%u reason=state-not-ready", method);
        return Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    }
    // The Start button's close is a trigger, not a duplicate: it arrives through
    // this primitive with method 6 and must reach the controller even while the
    // controller owns the panel. Its reopen form asks for Show(11) instead.
    if (method == kDismissMethodStartButton) {
        if (!DispatchToggle(own, kSwipeTriggerStartButton, 11, method, 0,
                            admittingEpoch)) {
            return Dismiss_Original ? Dismiss_Original(launcher, method)
                                    : E_FAIL;
        }
        return S_OK;
    }
    if (SwipeIpcControllerDirection(&g_ipc) != kSwipeDirectionUnowned) {
        TraceLog(L"exec swallow dismiss method=%u state=%u reason=owning",
                method, own.state);
        return S_OK;
    }
    const HRESULT hr =
        Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    OwnResyncFromState(OwnStateOf(launcher));
    return hr;
}

using ShowStartViewHook_t = HRESULT (*)(void*, uint32_t, uint32_t);
ShowStartViewHook_t ShowStartView_Original = nullptr;

HRESULT ShowStartView_Hook(void* launcher, uint32_t method, uint32_t flags) {
    uint32_t admittingEpoch = 0;
    if (!g_ownPrimitivesEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) || t_execCall > 0) {
        return ShowStartView_Original
                   ? ShowStartView_Original(launcher, method, flags)
                   : E_FAIL;
    }
    if (!BridgeCanServeOnLauncherThread(L"showstartview", &admittingEpoch)) {
        return ShowStartView_Original
                   ? ShowStartView_Original(launcher, method, flags)
                   : E_FAIL;
    }
    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        TraceLog(L"exec pass show method=%u flags=%u reason=state-not-ready",
                method, flags);
        return ShowStartView_Original
                   ? ShowStartView_Original(launcher, method, flags)
                   : E_FAIL;
    }
    // The Start button's open is a trigger too (method 11); it must reach the
    // controller while the controller owns a close, so it is decided here before
    // the duplicate suppression. Its close form asks for Dismiss(6).
    if (method == 11) {
        if (!DispatchToggle(own, kSwipeTriggerStartButton, method, 6, flags,
                            admittingEpoch)) {
            return ShowStartView_Original
                       ? ShowStartView_Original(launcher, method, flags)
                       : E_FAIL;
        }
        return S_OK;
    }
    if (SwipeIpcControllerDirection(&g_ipc) != kSwipeDirectionUnowned) {
        TraceLog(L"exec swallow show method=%u flags=%u state=%u reason=owning",
                method, flags, own.state);
        return S_OK;
    }
    const HRESULT hr = ShowStartView_Original
                           ? ShowStartView_Original(launcher, method, flags)
                           : E_FAIL;
    OwnResyncFromState(OwnStateOf(launcher));
    return hr;
}



// ---------------------------------------------------------------------------
// Launcher symbols
// ---------------------------------------------------------------------------

// The notification only records the image. Resolution runs on the executor
// worker: HookSymbols starts Windhawk's PDB fetch for twinui.pcshell.pdb (33 MB)
// and blocks until it finishes, and doing that on a thread Explorer owns would
// freeze the shell -- the same failure the Start side measured and fixed.
using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original
                         ? LoadLibraryExW_Original(fileName, file, flags)
                         : nullptr;

    // Filter on the returned module, not on the requested name: the call may
    // have asked for another file and still brought twinui in, and a refused or
    // redirected request must not look like a load. This mod never force-loads
    // twinui.
    if (module && module == GetModuleHandleW(kTwinuiModuleName)) {
        g_explorerModuleSeen.store(module, std::memory_order_release);
        if (!g_explorerLoadLogged.exchange(true)) {
            TraceLog(L"exec launcher module seen module=%p", module);
        }
    }

    return module;
}

// The proven caller wrappers: a thread-local category around the original call,
// restoring the previous value. They never decide anything and always call the
// original; the Hide hook reads the category.
using DismissStartAndCortana_t = void (*)(void*, uint32_t);
DismissStartAndCortana_t DismissStartAndCortana_Original = nullptr;

void DismissStartAndCortana_Hook(void* self, uint32_t method) {
    const OwnCallerScope scope(OwnCaller::Ordinary);
    if (DismissStartAndCortana_Original) {
        DismissStartAndCortana_Original(self, method);
    }
}

using DismissSynchronously_t = HRESULT (*)(void*);
DismissSynchronously_t DismissSynchronously_Original = nullptr;

HRESULT DismissSynchronously_Hook(void* self) {
    const OwnCallerScope scope(OwnCaller::Lock);
    return DismissSynchronously_Original ? DismissSynchronously_Original(self)
                                         : E_FAIL;
}

// ImmersiveMonitorHelper::ActiveZBandChangedForMonitor is the click-away scope:
// its body carries the synchronous `mov edx, 7; call [vtable+0x48]` at +0x18E
// (verified on 10.0.26100.9949), so a method-7 Hide reached inside it is the
// desktop click. Any other Hide from that scope is treated as unclassified by
// the Hide hook, not as a second click-away.
using ActiveZBand_t = HRESULT (*)(void*, unsigned long, unsigned long, void*,
                                  int);
ActiveZBand_t ActiveZBand_Original = nullptr;

HRESULT ActiveZBand_Hook(void* self, unsigned long a, unsigned long b,
                         void* monitor, int reason) {
    const OwnCallerScope scope(OwnCaller::ClickAway);
    return ActiveZBand_Original ? ActiveZBand_Original(self, a, b, monitor, reason)
                                : E_FAIL;
}

// One resolution attempt. Returns true only when this call installed every hook;
// a false result while the PDB fetch runs is pending, and the worker retries.
bool InstallExplorerHooks(HMODULE module) {
    if (!module) {
        return false;
    }

    AcquireSRWLockExclusive(&g_explorerSymbolLock);
    HMODULE expected = nullptr;
    if (!g_explorerSymbolClaim.compare_exchange_strong(expected, module)) {
        ReleaseSRWLockExclusive(&g_explorerSymbolLock);
        return false;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {
        ReleaseSRWLockExclusive(&g_explorerSymbolLock);
        return false;
    }
    TraceLog(L"exec launcher symbols begin module=%p", module);

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {{L"public: virtual long __cdecl "
          L"CStartExperienceManager::Show(unsigned char,unsigned char,"
          L"unsigned int)"},
         &Show_Original, &Show_Hook},
        {{L"public: virtual void __cdecl "
          L"XamlLauncherState::HideStartAndCortana(enum "
          L"IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &HideStartAndCortana_Original, &HideStartAndCortana_Hook},
        {{L"public: long __cdecl XamlLauncherState::OnStartShown(struct "
          L"Windows::Internal::Shell::Experience::IStartExperienceManager *,"
          L"struct IInspectable *)"},
         &OnStartShown_Original, &OnStartShown_Hook},
        {{L"public: long __cdecl XamlLauncherState::OnStartHidden(struct "
          L"Windows::Internal::Shell::Experience::IStartExperienceManager *,"
          L"struct IInspectable *)"},
         &OnStartHidden_Original, &OnStartHidden_Hook},
        {{L"private: long __cdecl XamlLauncher::OnHotkeyInvoke(enum "
          L"IMMERSIVELAUNCHERSHOWFLAGS)"},
         &OnHotkeyInvoke_Original, &OnHotkeyInvoke_Hook},
        {{L"public: virtual long __cdecl "
          L"XamlLauncher::OnStartButtonPressed(enum "
          L"IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &OnStartButtonPressed_Original, &OnStartButtonPressed_Hook},
        {{L"public: virtual long __cdecl XamlLauncher::Dismiss(enum "
          L"IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &Dismiss_Original, &Dismiss_Hook},
        {{L"public: virtual long __cdecl XamlLauncher::ShowStartView(enum "
          L"IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS)"},
         &ShowStartView_Original, &ShowStartView_Hook},
        {{L"public: virtual void __cdecl "
          L"XamlLauncherState::DismissStartAndCortana(enum "
          L"IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &DismissStartAndCortana_Original, &DismissStartAndCortana_Hook},
        {{L"public: virtual long __cdecl "
          L"XamlLauncher::DismissSynchronouslyWithoutTransition(void)"},
         &DismissSynchronously_Original, &DismissSynchronously_Hook},
        {{L"public: virtual long __cdecl "
          L"ImmersiveMonitorHelper::ActiveZBandChangedForMonitor(unsigned "
          L"long,unsigned long,struct IImmersiveMonitor *,"
          L"enum MODE_CHANGE_REASON)"},
         &ActiveZBand_Original, &ActiveZBand_Hook},
        {{L"private: static class XamlLauncher * XamlLauncher::s_pLauncher"},
         &g_launcherSlot, nullptr},
        {{L"public: virtual long __cdecl XamlLauncher::IsVisible(int *)"},
         &g_launcherIsVisible, nullptr},
    };
    const bool hooked =
        WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
    // Every symbol is required. A partially resolved set is not installed at
    // all: a toggle that can open but not close (or a close that cannot be
    // classified) is worse than the shell's own path.
    if (!hooked || !Show_Original || !HideStartAndCortana_Original ||
        !OnStartShown_Original || !OnStartHidden_Original ||
        !OnHotkeyInvoke_Original || !OnStartButtonPressed_Original ||
        !Dismiss_Original || !ShowStartView_Original ||
        !DismissStartAndCortana_Original || !DismissSynchronously_Original ||
        !ActiveZBand_Original || !g_launcherSlot || !g_launcherIsVisible) {
        g_explorerSymbolClaim.store(nullptr, std::memory_order_release);
        ReleaseSRWLockExclusive(&g_explorerSymbolLock);
        return false;
    }

    const uintptr_t base = reinterpret_cast<uintptr_t>(module);
    struct ResolvedSymbol {
        const wchar_t* label;
        const void* address;
        uintptr_t expectedRva;
    };
    const ResolvedSymbol resolved[] = {
        {L"show", reinterpret_cast<const void*>(Show_Original), kRvaShow},
        {L"hide", reinterpret_cast<const void*>(HideStartAndCortana_Original),
         kRvaHideStartAndCortana},
        {L"shown", reinterpret_cast<const void*>(OnStartShown_Original),
         kRvaOnStartShown},
        {L"hidden", reinterpret_cast<const void*>(OnStartHidden_Original),
         kRvaOnStartHidden},
        {L"win", reinterpret_cast<const void*>(OnHotkeyInvoke_Original),
         kRvaOnHotkeyInvoke},
        {L"button", reinterpret_cast<const void*>(OnStartButtonPressed_Original),
         kRvaOnStartButtonPressed},
        {L"dismiss", reinterpret_cast<const void*>(Dismiss_Original),
         kRvaDismiss},
        {L"showview", reinterpret_cast<const void*>(ShowStartView_Original),
         kRvaShowStartView},
        {L"ordinary", reinterpret_cast<const void*>(DismissStartAndCortana_Original),
         kRvaDismissStartAndCortana},
        {L"lock", reinterpret_cast<const void*>(DismissSynchronously_Original),
         kRvaDismissSynchronously},
        {L"clickaway", reinterpret_cast<const void*>(ActiveZBand_Original),
         kRvaActiveZBand},
        {L"slot", reinterpret_cast<const void*>(g_launcherSlot),
         kRvaLauncherSlot},
        {L"visible", reinterpret_cast<const void*>(g_launcherIsVisible),
         kRvaLauncherIsVisible},
    };
    // The identity is settled before any capability is enabled. The names
    // resolved; the RVAs and the fixed file version must also be the reference
    // image the private offsets were read from. A mismatch is a native
    // pass-through, never a probe of a guessed layout.
    int matches = 0;
    for (const ResolvedSymbol& symbol : resolved) {
        const uintptr_t rva =
            reinterpret_cast<uintptr_t>(symbol.address) - base;
        matches += rva == symbol.expectedRva ? 1 : 0;
    }
    const uint64_t version = ModuleFixedFileVersion(module);
    const bool layoutVerified =
        matches == static_cast<int>(ARRAYSIZE(resolved)) &&
        version == kTwinuiReferenceVersion;
    g_launcherLayoutVerified.store(layoutVerified, std::memory_order_release);

    // The executor calls the trampolines: its own calls are not triggers.
    // t_execCall still marks the frame for the Hide hook. Every capability that
    // reads the private layout is enabled only on the verified identity.
    g_dismiss = Dismiss_Original;
    g_showStartView = ShowStartView_Original;
    g_closeHookEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownPrimitivesEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownWinEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownButtonEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownEventsEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_callersVerified.store(layoutVerified, std::memory_order_relaxed);
    g_explorerSymbolsReady.store(true, std::memory_order_release);
    // The apply belongs to this serialized install, not to the caller: a stop
    // that takes the lock between the release and a caller-side apply would
    // return while this thread still added hooks. Applying here means
    // BeforeUninit either waits for the complete install or sees the stop and
    // makes this call refuse before HookSymbols.
    Wh_ApplyHookOperations();
    TraceLog(L"exec launcher hooks applied tid=%u", GetCurrentThreadId());
    ReleaseSRWLockExclusive(&g_explorerSymbolLock);

    for (const ResolvedSymbol& symbol : resolved) {
        const uintptr_t rva =
            reinterpret_cast<uintptr_t>(symbol.address) - base;
        TraceLog(L"exec launcher symbol label=%s rva=0x%llX expected=0x%llX "
                L"match=%d",
                symbol.label, static_cast<unsigned long long>(rva),
                static_cast<unsigned long long>(symbol.expectedRva),
                rva == symbol.expectedRva ? 1 : 0);
    }
    TraceLog(L"exec launcher symbols resolved base=0x%llX count=%d matches=%d "
            L"match=%d",
            static_cast<unsigned long long>(base),
            static_cast<int>(ARRAYSIZE(resolved)), matches,
            matches == static_cast<int>(ARRAYSIZE(resolved)) ? 1 : 0);
    // One line states the identity and what it enabled. The degrade path is a
    // single native pass-through announcement, not a per-trigger repeat.
    TraceLog(L"exec launcher layout %s version=%u.%u.%u.%u matches=%d "
            L"native=%d reason=%s",
            layoutVerified ? L"verified" : L"unverified",
            static_cast<unsigned>((version >> 48) & 0xFFFF),
            static_cast<unsigned>((version >> 32) & 0xFFFF),
            static_cast<unsigned>((version >> 16) & 0xFFFF),
            static_cast<unsigned>(version & 0xFFFF), matches,
            layoutVerified ? 0 : 1,
            version != kTwinuiReferenceVersion
                ? L"version"
                : (matches != static_cast<int>(ARRAYSIZE(resolved)) ? L"rva"
                                                                    : L"none"));
    Wh_Log(L"twinui.pcshell.dll symbols resolved at %p (%d/%d at the reference "
           L"RVAs); launcher hooks installed layout=%d",
           reinterpret_cast<void*>(base), matches,
           static_cast<int>(ARRAYSIZE(resolved)), layoutVerified ? 1 : 0);
    return true;
}

// Records an already-loaded twinui. Called from init and AfterInit; it never
// resolves anything (that is the worker's job, so the shell's thread is never
// blocked by the PDB fetch).
void NoteLoadedExplorerModule() {
    if (g_disabled.load(std::memory_order_relaxed) ||
        g_explorerSymbolsReady.load(std::memory_order_acquire) ||
        g_explorerSymbolsRefused.load(std::memory_order_acquire)) {
        return;
    }
    HMODULE twinui = GetModuleHandleW(kTwinuiModuleName);
    if (twinui) {
        g_explorerModuleSeen.store(twinui, std::memory_order_release);
    }
}

// Resolves the launcher symbols from the executor worker. Called on every wait
// timeout: immediate when the module is first known, then throttled while the
// result is pending. Never called on a thread Explorer owns.
void ProbeExplorerSymbolsFromWorker() {
    if (!g_afterInit.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) ||
        g_explorerSymbolsReady.load(std::memory_order_acquire) ||
        g_explorerSymbolsRefused.load(std::memory_order_acquire)) {
        return;
    }
    HMODULE module = g_explorerModuleSeen.load(std::memory_order_acquire);
    if (!module) {
        module = GetModuleHandleW(kTwinuiModuleName);
        if (!module) {
            return;
        }
        g_explorerModuleSeen.store(module, std::memory_order_release);
    }

    const uint64_t now = GetTickCount64();
    const bool first = g_execProbeFirstTick == 0;
    if (!first && now < g_execProbeNextTick) {
        return;
    }
    g_execProbeNextTick = now + kExecSymbolProbeMs;
    if (first) {
        g_execProbeFirstTick = now;
    }

    if (InstallExplorerHooks(module)) {
        // The install applied its own queued hooks under the lock.
        return;
    }
    if (g_explorerSymbolsReady.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return;
    }

    g_execProbeAttempts++;
    const uint64_t waited = GetTickCount64() - g_execProbeFirstTick;
    if (waited >= kExecSymbolPendingDeadlineMs) {
        g_explorerSymbolsRefused.store(true, std::memory_order_release);
        TraceLog(L"exec launcher symbols deadline attempts=%d waited=%llums "
                L"deadline=%llums",
                g_execProbeAttempts, static_cast<unsigned long long>(waited),
                static_cast<unsigned long long>(
                    kExecSymbolPendingDeadlineMs));
        TraceLog(L"exec launcher symbols unresolved name=twinui.pcshell.dll "
                L"(symbol, signature or hook refused)");
        Wh_Log(L"twinui.pcshell.dll symbols could not be resolved within the "
               L"%llu ms pending deadline; no launcher hook is installed and "
               L"every press stays native",
               static_cast<unsigned long long>(kExecSymbolPendingDeadlineMs));
        return;
    }
    TraceLog(L"exec launcher symbols pending attempt=%d waited=%llums "
            L"deadline=%llums",
            g_execProbeAttempts, static_cast<unsigned long long>(waited),
            static_cast<unsigned long long>(kExecSymbolPendingDeadlineMs));
}

// ---------------------------------------------------------------------------
// Module lifetime
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// M3 executor: hidden window, launcher read, commands
// ---------------------------------------------------------------------------

// `s_pLauncher` holds the outer XamlLauncher object. The execution entry points
// take its IXamlLauncher subobject, `outer + 0x18` — the same pointer the shell
// passes to ShowStartView/Dismiss and the one the decision hooks call `launcher`.
// Measured in explorer: outer vtable twinui+0x77E1C0 has `[+0x160] = 0`, while
// `[outer+0x18+0x160]` is the live IXamlLauncherState whose vtable+0x38 is
// `XamlLauncherState::ShowStartView` at twinui+0x26F780. Reading the state from
// the outer object returns the 0x80070490 path; the subobject is the interface.
void* LauncherNow(uint64_t* stateOut, void** outerOut) {
    if (stateOut) {
        *stateOut = 0;
    }
    if (outerOut) {
        *outerOut = nullptr;
    }
    // The offsets below belong to the verified reference image. On any other
    // image the executor call is refused here and the shell serves the press;
    // nothing reads a member or a vtable slot of a layout this mod has not
    // proved. `exec launcher layout unverified ...` states the reason once.
    if (!g_launcherLayoutVerified.load(std::memory_order_acquire)) {
        return nullptr;
    }
    if (!g_launcherSlot) {
        return nullptr;
    }
    // Read the pointer slot on each execution, on the caller's thread; the
    // singleton is never cached here.
    void* outer = *g_launcherSlot;
    if (outerOut) {
        *outerOut = outer;
    }
    if (!outer) {
        return nullptr;
    }
    void* launcher = static_cast<uint8_t*>(outer) + kHotkeyLauncherOffset;
    const uint64_t state = *reinterpret_cast<uint64_t*>(
        static_cast<uint8_t*>(launcher) + kLauncherStatePointerOffset);
    if (stateOut) {
        *stateOut = state;
    }
    if (!LauncherStateReady(state)) {
        return nullptr;
    }
    return launcher;
}

void AnswerExec(LONG64 sequence, int32_t state, HRESULT hr, int visible) {
    if (sequence > 0) {
        SwipeIpcAnswerExec(&g_ipc, sequence, state, hr, visible);
    }
}

// The visibility probe is three-valued. "No launcher" and "IsVisible failed" are
// not "hidden": reporting those as a confirmed hide is how a dismiss that never
// ran looked successful.
enum class ProbeVisibility { Hidden, Visible, Unknown };

int32_t ProbeVisibilityValue(ProbeVisibility value) {
    switch (value) {
        case ProbeVisibility::Hidden:
            return kSwipeVisibilityHidden;
        case ProbeVisibility::Visible:
            return kSwipeVisibilityVisible;
        default:
            return kSwipeVisibilityUnknown;
    }
}

const wchar_t* ProbeVisibilityName(ProbeVisibility value) {
    switch (value) {
        case ProbeVisibility::Hidden:
            return L"hidden";
        case ProbeVisibility::Visible:
            return L"visible";
        default:
            return L"unknown";
    }
}

ProbeVisibility LauncherVisibleNow(void* launcher) {
    if (!launcher || !g_launcherIsVisible) {
        return ProbeVisibility::Unknown;
    }
    BOOL visible = FALSE;
    const HRESULT hr = g_launcherIsVisible(launcher, &visible);
    if (FAILED(hr)) {
        return ProbeVisibility::Unknown;
    }
    return visible != FALSE ? ProbeVisibility::Visible
                            : ProbeVisibility::Hidden;
}

void ArmVisibleProbe(LONG64 sequence) {
    g_pendingShowSeq = sequence;
    if (g_execWindow.load(std::memory_order_relaxed)) {
        SetTimer(g_execWindow.load(std::memory_order_relaxed), kExecProbeTimer,
                 300, nullptr);
    }
}

// Whether a record has outlived the publisher's budget. The scheduled time is the
// dismiss deadline, or the publication tick for a show and a cancel. A zero means
// the publisher never wrote one (a record from an older layout), and such a
// record is not judged here.
//
// The comparison is `>=` on both sides — here and in the Start side's
// CheckExecAckTimeout — because the record and the publisher are two clocks: a
// dispatch at exactly due+400 has already outlived the budget, and the side that
// sees equality must not be the only one that treats it as live. This is not a
// lease on the shell call: a primitive that entered before the deadline can
// return after the publisher timed out, and nothing here can recall it. The
// publisher's timeout is a repair, not a cancel.
bool ExecCommandExpired(const SwipeIpcExecCommand& command, uint64_t now) {
    const uint64_t due = command.kind == kSwipeExecDismiss ? command.executeAt
                                                           : command.tick;
    return due != 0 && now >= due && now - due >= kExecExpiryMs;
}

// Arms the visibility confirmation a successful dismiss owes. Any previous one
// belongs to a command this one supersedes, and that record is answered
// `Superseded` so its publisher can tell it from a timer failure. A timer that
// cannot be armed answers `Unknown`: without the probe the hide is not confirmed,
// and silence would let the Start side read the executed ack as one.
void ArmDismissConfirm(LONG64 sequence, ProbeVisibility visible) {
    if (g_pendingDismissConfirmSeq != 0 &&
        g_pendingDismissConfirmSeq != sequence) {
        TraceLog(L"exec dismiss superseded seq=%lld by=%lld",
                static_cast<long long>(g_pendingDismissConfirmSeq),
                static_cast<long long>(sequence));
        AnswerExec(g_pendingDismissConfirmSeq, kSwipeAckSuperseded, S_OK,
                   g_pendingDismissConfirmVisible);
    }
    g_pendingDismissConfirmSeq = sequence;
    g_pendingDismissConfirmVisible = ProbeVisibilityValue(visible);
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        TraceLog(L"exec dismiss confirm timer failed seq=%lld reason=no-window",
                static_cast<long long>(sequence));
        g_pendingDismissConfirmSeq = 0;
        AnswerExec(sequence, kSwipeAckUnknown, S_OK, kSwipeVisibilityUnknown);
        return;
    }
    if (SetTimer(window, kExecDismissProbeTimer,
                 static_cast<UINT>(kExecDismissConfirmMs), nullptr) == 0) {
        TraceLog(L"exec dismiss confirm timer failed seq=%lld err=%u",
                static_cast<long long>(sequence), GetLastError());
        g_pendingDismissConfirmSeq = 0;
        AnswerExec(sequence, kSwipeAckUnknown, S_OK, kSwipeVisibilityUnknown);
        return;
    }
    g_pendingDismissConfirmTick = GetTickCount64();
}

void KillDismissConfirm() {
    g_pendingDismissConfirmSeq = 0;
    g_pendingDismissConfirmVisible = 0;
    g_pendingDismissConfirmTick = 0;
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (window) {
        KillTimer(window, kExecDismissProbeTimer);
    }
}

// A record names the menu it was published for. When the section has already
// moved to a newer menu, a record for the old one must not act on the new one —
// the case a Start restart leaves behind when the old record is accepted after
// the new activation attached. A zero in either word means the record predates
// the binding (or no menu has opened yet), and the epoch check is then the one
// that guards it.
bool ExecCommandForCurrentMenu(const SwipeIpcExecCommand& command,
                               const wchar_t* kind) {
    uint64_t open = 0;
    uint64_t openTick = 0;
    SwipeIpcReadOpen(&g_ipc, &open, &openTick);
    // `open` is the generation and `openTick` the tick it was published at. The
    // record carries the generation, so the two generations are what must be
    // compared; comparing a generation against a tick refused every command.
    if (command.open == 0 || open == 0 || command.open == open) {
        return true;
    }
    TraceLog(L"exec %s seq=%lld superseded menu=%llu now=%llu", kind,
            static_cast<long long>(command.seq),
            static_cast<unsigned long long>(command.open),
            static_cast<unsigned long long>(open));
    return false;
}

// Validates a command before anything arms a timer or touches the launcher. The
// vocabulary is the one this mod's own producers use, not a numeric range: a
// method the shell does not receive from us is refused, and no flag word is
// forwarded that a command of that kind does not carry.
bool ValidExecCommand(const SwipeIpcExecCommand& command) {
    switch (command.kind) {
        case kSwipeExecShow:
            return command.executeAt == 0 && command.target == 0 &&
                   SwipeShowAllowed(static_cast<uint32_t>(command.method),
                                    static_cast<uint32_t>(command.flags));
        case kSwipeExecDismiss:
            return command.flags == 0 && command.target == 0 &&
                   SwipeDismissAllowed(static_cast<uint32_t>(command.method));
        case kSwipeExecCancel:
            return command.method == 0 && command.flags == 0 &&
                   command.executeAt == 0;
        default:
            return false;
    }
}

void ExecuteShow(const SwipeIpcExecCommand& command) {
    const uint64_t now = GetTickCount64();
    if (ExecCommandExpired(command, now)) {
        // The publisher gave up before this dispatch started: its sequence is
        // cleared and its ownership released. Presenting now would open a menu no
        // controller owns, so the record is expired and the primitive is not
        // called.
        TraceLog(L"exec show seq=%lld expired age=%llu (publisher released it)",
                static_cast<long long>(command.seq),
                static_cast<unsigned long long>(now - command.tick));
        AnswerExec(command.seq, kSwipeAckExpired,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        return;
    }
    if (!ExecCommandForCurrentMenu(command, L"show")) {
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        return;
    }
    // A show supersedes whatever confirmation an earlier dismiss still owed: the
    // window is opening again, and the old probe would read this open. The
    // publisher is told the old confirmation will not come.
    if (g_pendingDismissConfirmSeq != 0) {
        TraceLog(L"exec dismiss confirm superseded seq=%lld reason=show",
                static_cast<long long>(g_pendingDismissConfirmSeq));
        AnswerExec(g_pendingDismissConfirmSeq, kSwipeAckSuperseded, S_OK,
                   g_pendingDismissConfirmVisible);
    }
    KillDismissConfirm();
    uint64_t state = 0;
    void* outer = nullptr;
    void* launcher = LauncherNow(&state, &outer);
    if (!launcher) {
        TraceLog(L"exec show seq=%lld refused outer=%p launcher=%p state=0x%llX "
                L"reason=not-ready",
                static_cast<long long>(command.seq), outer, launcher,
                static_cast<unsigned long long>(state));
        AnswerExec(command.seq, kSwipeAckRefused,
                   HRESULT_FROM_WIN32(ERROR_NOT_FOUND), kSwipeVisibilityUnknown);
        return;
    }
    const uint32_t epoch = PeerEpochNow();
    if (epoch != g_dispatchEpoch) {
        // The activation that published this record is gone. Calling Show now
        // would present a menu for a controller that has already released it.
        TraceLog(L"exec show seq=%lld superseded epoch=%u now=%u",
                static_cast<long long>(command.seq), g_dispatchEpoch, epoch);
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        return;
    }
    ++t_execCall;
    const HRESULT hr = g_showStartView
                           ? g_showStartView(launcher, command.method,
                                             command.flags)
                           : E_FAIL;
    // IsVisible right after the call is honest but early: the shell's show is
    // still running and the window is cloaked, so the answer is 0 here. The
    // confirmation is read again by the probe timer on this same thread.
    const ProbeVisibility visible = LauncherVisibleNow(launcher);
    --t_execCall;
    TraceLog(L"exec show seq=%lld outer=%p launcher=%p state=0x%llX hr=%08X "
            L"visible=%s",
            static_cast<long long>(command.seq), outer, launcher,
            static_cast<unsigned long long>(state),
            static_cast<unsigned>(hr), ProbeVisibilityName(visible));
    AnswerExec(command.seq, SUCCEEDED(hr) ? kSwipeAckExecuted : kSwipeAckRefused,
               hr, ProbeVisibilityValue(visible));
    if (SUCCEEDED(hr)) {
        ArmVisibleProbe(command.seq);
    }
}

void ExecuteDismiss(const SwipeIpcExecCommand& command) {
    const uint64_t now = GetTickCount64();
    if (ExecCommandExpired(command, now)) {
        // The deadline plus the controller's budget has passed. The controller
        // cleared the sequence and released the panel, so a Dismiss now would
        // hide a window it has already stopped tracking — or a newer menu.
        TraceLog(L"exec dismiss seq=%lld expired age=%llu candidate=%lld",
                static_cast<long long>(command.seq),
                static_cast<unsigned long long>(now > command.executeAt
                                                    ? now - command.executeAt
                                                    : 0),
                static_cast<long long>(g_pendingDismissSeq));
        AnswerExec(command.seq, kSwipeAckExpired,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-expired", L"executor");
        return;
    }
    if (!ExecCommandForCurrentMenu(command, L"dismiss")) {
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-superseded", L"menu");
        return;
    }
    // A pending show probe belongs to a window this dismiss is about to hide; its
    // answer would describe the close, not the open.
    g_pendingShowSeq = 0;
    if (g_execWindow.load(std::memory_order_relaxed)) {
        KillTimer(g_execWindow.load(std::memory_order_relaxed), kExecProbeTimer);
    }
    uint64_t state = 0;
    void* outer = nullptr;
    void* launcher = LauncherNow(&state, &outer);
    if (!launcher) {
        TraceLog(L"exec dismiss seq=%lld refused outer=%p launcher=%p "
                L"state=0x%llX reason=not-ready",
                static_cast<long long>(command.seq), outer, launcher,
                static_cast<unsigned long long>(state));
        AnswerExec(command.seq, kSwipeAckRefused,
                   HRESULT_FROM_WIN32(ERROR_NOT_FOUND), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-refused", L"executor");
        return;
    }
    // The command was accepted against one activation and runs against whatever
    // answers now. A fast Start restart inside the execution budget is otherwise
    // invisible: the epoch is read immediately before the call, and a mismatch
    // means this Dismiss would hide a menu the new activation may have opened.
    // No scheme can recall a call that has already entered the shell; this is the
    // last point at which the decision is still free.
    const uint32_t epoch = PeerEpochNow();
    if (epoch != g_pendingDismissEpoch) {
        TraceLog(L"exec dismiss seq=%lld superseded epoch=%u now=%u",
                static_cast<long long>(command.seq), g_pendingDismissEpoch,
                epoch);
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-superseded", L"epoch");
        return;
    }
    ++t_execCall;
    const HRESULT hr =
        g_dismiss ? g_dismiss(launcher, command.method)
                  : E_FAIL;
    // `hr` is the primitive's answer, and the visibility right after it is
    // checked again by the confirmation probe: success is not yet a hide.
    const ProbeVisibility visible = LauncherVisibleNow(launcher);
    --t_execCall;
    TraceLog(L"exec dismiss seq=%lld outer=%p launcher=%p state=0x%llX hr=%08X "
            L"visible=%s",
            static_cast<long long>(command.seq), outer, launcher,
            static_cast<unsigned long long>(state),
            static_cast<unsigned>(hr), ProbeVisibilityName(visible));
    AnswerExec(command.seq, SUCCEEDED(hr) ? kSwipeAckExecuted : kSwipeAckRefused,
               hr, ProbeVisibilityValue(visible));
    if (SUCCEEDED(hr)) {
        ArmDismissConfirm(command.seq, visible);
    } else {
        // The one command this press waited for was refused; the latch has no
        // hide to serve and must not retry until its deadline.
        DropPressLatch(L"dismiss-refused", L"executor");
    }
}

// Arms the one dismiss timer. False means the message queue refused it, and the
// caller must not leave a dismiss armed without a deadline: the Start side would
// wait for a command nobody runs.
bool ArmDismissTimer(uint32_t delayMs) {
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        return false;
    }
    if (SetTimer(window, kExecDismissTimer, delayMs, nullptr) == 0) {
        TraceLog(L"exec dismiss timer failed err=%u", GetLastError());
        return false;
    }
    return true;
}

void ExecuteDueDismiss() {
    if (g_pendingDismissSeq == 0) {
        return;
    }
    SwipeIpcExecCommand command{};
    if (!SwipeIpcReadExec(&g_ipc, &command)) {
        // A torn record, not a missing command: the timer is already dead and the
        // deadline has passed, so retry shortly instead of dropping the one
        // command this close depends on. A record that never settles inside the
        // retry budget is answered as refused, which is a terminal report the
        // Start side can repair from; silence would leave it waiting.
        if (++g_dismissReadRetries <= kExecDismissReadRetries &&
            ArmDismissTimer(kExecDismissRetryMs)) {
            TraceLog(L"exec dismiss seq=%lld read retry=%d",
                    static_cast<long long>(g_pendingDismissSeq),
                    g_dismissReadRetries);
            return;
        }
        const LONG64 sequence = g_pendingDismissSeq;
        const int retries = g_dismissReadRetries;
        g_pendingDismissSeq = 0;
        g_dismissReadRetries = 0;
        TraceLog(L"exec dismiss seq=%lld refused reason=unreadable retries=%d",
                static_cast<long long>(sequence), retries);
        AnswerExec(sequence, kSwipeAckRefused, E_FAIL, 0);
        DropPressLatch(L"dismiss-refused", L"unreadable");
        return;
    }
    g_dismissReadRetries = 0;
    if (command.seq != g_pendingDismissSeq ||
        command.kind != kSwipeExecDismiss) {
        TraceLog(L"exec dismiss seq=%lld superseded by %lld",
                static_cast<long long>(g_pendingDismissSeq),
                static_cast<long long>(command.seq));
        if (g_pendingDismissSeq != 0) {
            AnswerExec(g_pendingDismissSeq, kSwipeAckSuperseded, S_OK,
                       kSwipeVisibilityUnknown);
        }
        g_pendingDismissSeq = 0;
        g_pendingDismissEpoch = 0;
        return;
    }
    g_pendingDismissSeq = 0;
    ExecuteDismiss(command);
}

void ExecutePendingCommand() {
    SwipeIpcExecCommand command{};
    if (!SwipeIpcReadExec(&g_ipc, &command) || command.seq == 0) {
        return;
    }
    if (command.seq == g_execAcceptedSeq) {
        // A second post for a record already accepted. The auto-reset Exec event
        // can deliver the same signal twice when a new publish lands during the
        // read; the record is the same, and running it again would present a show
        // twice or dismiss a menu this close already hid.
        return;
    }
    g_execAcceptedSeq = command.seq;
    // The activation that published this record, read once at acceptance. A
    // dismiss may be armed for hundreds of milliseconds; the epoch is compared
    // again immediately before the shell call, because a Start restart inside
    // that budget must not let the old record act on a new menu.
    g_dispatchEpoch = PeerEpochNow();
    if (!ValidExecCommand(command)) {
        TraceLog(L"exec refused seq=%lld kind=%d method=%u flags=%u target=%lld "
                L"reason=invalid",
                static_cast<long long>(command.seq),
                static_cast<int>(command.kind), command.method, command.flags,
                static_cast<long long>(command.target));
        AnswerExec(command.seq, kSwipeAckRefused, E_INVALIDARG,
                   kSwipeVisibilityUnknown);
        return;
    }
    if (command.kind == kSwipeExecCancel) {
        if (g_pendingDismissSeq != 0 &&
            (command.target == 0 || command.target == g_pendingDismissSeq)) {
            KillTimer(g_execWindow.load(), kExecDismissTimer);
            TraceLog(L"exec cancel seq=%lld target=%lld",
                    static_cast<long long>(command.seq),
                    static_cast<long long>(g_pendingDismissSeq));
            // The cancel's own answer first, the target's last. The ACK slot is
            // one record with an auto-reset event: two answers written before the
            // reader wakes leave only the second readable. The Start side retires
            // its recovery ticket on the *dismiss's* answer, so that answer has to
            // be the one left behind; the cancel itself is fire-and-forget there.
            AnswerExec(command.seq, kSwipeAckExecuted, S_OK, 0);
            AnswerExec(g_pendingDismissSeq, kSwipeAckSuperseded, S_OK,
                       kSwipeVisibilityUnknown);
            g_pendingDismissSeq = 0;
            g_pendingDismissEpoch = 0;
        } else {
            AnswerExec(command.seq, kSwipeAckExecuted, S_OK, 0);
        }
        return;
    }
    if (g_pendingDismissSeq != 0 && command.seq != g_pendingDismissSeq) {
        KillTimer(g_execWindow.load(), kExecDismissTimer);
        AnswerExec(g_pendingDismissSeq, kSwipeAckSuperseded, S_OK,
                   kSwipeVisibilityUnknown);
        g_pendingDismissSeq = 0;
        g_pendingDismissEpoch = 0;
    }
    if (command.kind == kSwipeExecShow) {
        ExecuteShow(command);
        return;
    }
    if (command.kind == kSwipeExecDismiss) {
        const uint64_t now = GetTickCount64();
        if (command.executeAt > now) {
            const uint32_t delay = static_cast<UINT>(command.executeAt - now);
            if (ArmDismissTimer(delay)) {
                g_pendingDismissSeq = command.seq;
                g_pendingDismissEpoch = g_dispatchEpoch;
                g_dismissReadRetries = 0;
                TraceLog(L"exec dismiss seq=%lld armed in=%u ms epoch=%u",
                        static_cast<long long>(command.seq), delay,
                        g_pendingDismissEpoch);
                return;
            }
            // No timer means the command would be armed forever. Run it now: a
            // close that cuts its settle short is still a close, while a dismiss
            // nobody runs leaves the controller owning a panel whose window is
            // still on screen.
            TraceLog(L"exec dismiss seq=%lld armed=0 reason=no-timer",
                    static_cast<long long>(command.seq));
        }
        g_pendingDismissEpoch = g_dispatchEpoch;
        ExecuteDismiss(command);
    }
}

LRESULT CALLBACK ExecutorWindowProc(HWND window, UINT message, WPARAM wParam,
                                    LPARAM lParam) {
    if (message == kExecTeardownMessage) {
        // The owner-thread shutdown. This is a private request, not a generic
        // WM_CLOSE: the one-shot timers would otherwise outlive the window, and a
        // timer that fires after the unload would read a session the channel no
        // longer maps. Only this activation's published window answers.
        if (wParam != kExecTeardownMagic ||
            window != g_execWindow.load(std::memory_order_acquire)) {
            TraceLog(L"exec teardown ignored hwnd=%p", window);
            return 0;
        }
        KillTimer(window, kExecDismissTimer);
        KillTimer(window, kExecProbeTimer);
        KillTimer(window, kPressLatchTimer);
        KillTimer(window, kExecDismissProbeTimer);
        DestroyWindow(window);
        // The acknowledgement is set after DestroyWindow returns, not from
        // WM_NCDESTROY inside it: the class's window count is decremented when
        // the call completes, and the unload must not unregister the class
        // between the two.
        if (g_execWindowGone) {
            SetEvent(g_execWindowGone);
        }
        return 0;
    }
    if (message == WM_NCDESTROY) {
        // The destructor of the one published window. Any other window of this
        // class (there is none: the class is owned and unregistered) must not
        // answer for it. The class acknowledgement is set by the teardown above
        // after DestroyWindow returns.
        const bool identified =
            g_execWindow.load(std::memory_order_acquire) == window;
        if (identified) {
            g_execWindow.store(nullptr, std::memory_order_release);
        }
        TraceLog(L"exec window gone hwnd=%p identified=%d tid=%u", window,
                identified ? 1 : 0, GetCurrentThreadId());
        return 0;
    }
    // A window this activation did not publish is not a dispatcher. A survivor
    // from an earlier image is impossible (the class is owned and unregistered),
    // but the test keeps a stray message from reaching this activation's state.
    if (window != g_execWindow.load(std::memory_order_acquire)) {
        return DefWindowProcW(window, message, wParam, lParam);
    }
    if (!g_execReady.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {
        // An unload is in flight: no command and no timer may touch the launcher
        // or the channel after this point.
        return DefWindowProcW(window, message, wParam, lParam);
    }
    if (message == kExecLatchMessage) {
        if (wParam == kExecLatchMagic) {
            // The hidden event ran on another thread; the slot and its timer
            // belong to this one.
            ArmLatchedServe();
        }
        return 0;
    }
    if (message == kExecMessage) {
        ExecutePendingCommand();
        return 0;
    }
    if (message == WM_TIMER && wParam == kExecDismissTimer) {
        KillTimer(window, kExecDismissTimer);
        ExecuteDueDismiss();
        return 0;
    }
    if (message == WM_TIMER && wParam == kExecProbeTimer) {
        KillTimer(window, kExecProbeTimer);
        uint64_t state = 0;
        void* outer = nullptr;
        void* launcher = LauncherNow(&state, &outer);
        const ProbeVisibility visible = LauncherVisibleNow(launcher);
        TraceLog(L"exec visible seq=%lld launcher=%p state=0x%llX visible=%s",
                static_cast<long long>(g_pendingShowSeq), launcher,
                static_cast<unsigned long long>(state),
                ProbeVisibilityName(visible));
        return 0;
    }
    if (message == WM_TIMER && wParam == kExecDismissProbeTimer) {
        KillTimer(window, kExecDismissProbeTimer);
        const LONG64 sequence = g_pendingDismissConfirmSeq;
        const int32_t atCall = g_pendingDismissConfirmVisible;
        const uint64_t armedAt = g_pendingDismissConfirmTick;
        g_pendingDismissConfirmSeq = 0;
        g_pendingDismissConfirmVisible = 0;
        g_pendingDismissConfirmTick = 0;
        if (sequence != 0) {
            uint64_t state = 0;
            void* outer = nullptr;
            void* launcher = LauncherNow(&state, &outer);
            const ProbeVisibility visible = LauncherVisibleNow(launcher);
            const uint64_t elapsed =
                armedAt != 0 && GetTickCount64() > armedAt
                    ? GetTickCount64() - armedAt
                    : 0;
            // The bounded confirmation: `hr=0` said the primitive returned, and
            // this says whether the window actually went away. A hidden window is
            // confirmed; a visible one, or a probe that could not answer, is
            // answered as its own outcome so the publisher can repair.
            const int32_t answer =
                visible == ProbeVisibility::Hidden
                    ? kSwipeAckConfirmed
                    : (visible == ProbeVisibility::Visible ? kSwipeAckUnconfirmed
                                                           : kSwipeAckUnknown);
            AnswerExec(sequence, answer, S_OK, ProbeVisibilityValue(visible));
            TraceLog(L"exec dismiss %s seq=%lld visible=%s at_call=%d after=%llu "
                    L"ms",
                    answer == kSwipeAckConfirmed
                        ? L"confirmed"
                        : (answer == kSwipeAckUnconfirmed ? L"unconfirmed"
                                                          : L"unknown"),
                    static_cast<long long>(sequence),
                    ProbeVisibilityName(visible), atCall,
                    static_cast<unsigned long long>(elapsed));
            if (visible != ProbeVisibility::Hidden) {
                // The window did not go away, or nothing can say it did; the
                // latch has no landing to be served at.
                DropPressLatch(answer == kSwipeAckUnconfirmed
                                   ? L"dismiss-unconfirmed"
                                   : L"dismiss-unknown",
                               L"confirmation");
            }
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == kPressLatchTimer) {
        KillTimer(window, kPressLatchTimer);
        if (!ServeLatchedPress(g_pressServeReason) && g_pressLatch.pending) {
            // The hide has not landed yet. Retry; the hidden event is the normal
            // serve point, this only covers a landing it missed. A timer that
            // cannot be armed again ends the latch: retrying without a timer was
            // never the design, and serving on the hidden event's stack is the
            // measured swallow.
            g_pressServeReason = L"timeout";
            if (SetTimer(window, kPressLatchTimer,
                         static_cast<UINT>(kPressLatchRetryMs), nullptr) == 0) {
                DropPressLatch(L"timer-failed", L"retry");
            }
        }
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

// The launcher thread is the thread that runs OnHotkeyInvoke (measured tid=7108
// and 11096 after an explorer restart); Dismiss, ShowStartView and the manager's
// Show are that same thread's own calls. The first of these entries adopts the
// thread; a later one on a different thread logs the mismatch and creates
// nothing, because a window on any other thread never pumps this thread's
// messages. The library-load hook is deliberately not an entry: it runs on
// whichever thread loaded twinui, and the taskbar thread is the one measured as
// an explorer fail-fast for a show.
DWORD NoteLauncherThread(const wchar_t* source) {
    const DWORD tid = GetCurrentThreadId();
    DWORD known = g_launcherThread.load(std::memory_order_relaxed);
    if (known == 0) {
        if (g_launcherThread.compare_exchange_strong(known, tid)) {
            TraceLog(L"exec launcher thread tid=%u source=%s", tid, source);
            known = tid;
        }
    }
    if (known != tid) {
        TraceLog(L"exec launcher thread mismatch known=%u tid=%u source=%s",
                known, tid, source);
        return 0;
    }
    return tid;
}

// The manager's Show is the shell's own open on the launcher thread, and it is
// the first launcher entry of a touch-first open. It is an adoption point like
// the decision entries: the thread test comes before anything is created, so a
// Show on a thread this mod has not recorded cannot create the executor there.
void EnsureExecutorOnLauncherThread(const wchar_t* source) {
    if (ExecutorWindowOnThisThread()) {
        return;  // already created on this thread
    }
    if (NoteLauncherThread(source) == 0) {
        return;  // not the launcher thread: never create a window here
    }
    EnsureExecutorWindow();
}

void EnsureExecutorWindow() {
    if (g_disabled.load(std::memory_order_relaxed) ||
        g_execWindow.load(std::memory_order_relaxed)) {
        return;
    }
    // The class is fixed and owned by this image. `ERROR_CLASS_ALREADY_EXISTS`
    // is a failure: it means a previous window of this image survived its
    // unload, and adopting that class would hand this activation's channel to a
    // procedure whose module is about to be unmapped.
    if (!g_execClassRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = ExecutorWindowProc;
        windowClass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        windowClass.lpszClassName = kExecClassName;
        if (!RegisterClassExW(&windowClass)) {
            TraceLog(L"exec window class failed err=%u", GetLastError());
            return;
        }
        g_execClassRegistered = true;
        g_execClassInstance = windowClass.hInstance;
    }
    HWND window = CreateWindowExW(0, kExecClassName, L"", 0, 0, 0, 0,
                                  0, HWND_MESSAGE, nullptr,
                                  g_execClassInstance, nullptr);
    if (!window) {
        TraceLog(L"exec window create failed err=%u", GetLastError());
        return;
    }
    // The owner is written before the window is published, so the pair a reader
    // sees is never a window without its thread.
    g_execThreadId = GetCurrentThreadId();
    g_execWindow.store(window, std::memory_order_release);
    // The assertion the owner-thread report is for: a created window must be on
    // the launcher thread, never on the thread that happened to call a hook.
    const DWORD launcherThread = g_launcherThread.load(std::memory_order_relaxed);
    if (launcherThread != 0 && launcherThread != g_execThreadId) {
        TraceLog(L"exec window thread mismatch owner=%u launcher=%u",
                g_execThreadId, launcherThread);
    }
    TraceLog(L"exec window hwnd=%p tid=%u class=%s", window, g_execThreadId,
            kExecClassName);
}

LRESULT CALLBACK ExecutorThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && !g_execWindow.load(std::memory_order_relaxed) &&
        GetCurrentThreadId() == g_launcherThread.load(std::memory_order_relaxed)) {
        // A WH_GETMESSAGE hook installed for the recorded launcher thread runs
        // only on that thread; the test states the invariant the window's
        // lifetime depends on.
        EnsureExecutorWindow();
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool CreateExecutorWindowOnUiThread(DWORD timeoutMs) {
    if (g_execWindow.load(std::memory_order_relaxed)) {
        return true;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {
        return false;  // a shutdown owns the lifetime now
    }
    const DWORD target = g_launcherThread.load(std::memory_order_relaxed);
    if (target == 0) {
        // No launcher entry has run yet. A touch-first open falls back to the
        // shell's own gesture; the first OnHotkeyInvoke, Dismiss, ShowStartView
        // or manager Show creates the window.
        return false;
    }
    if (target == GetCurrentThreadId()) {
        EnsureExecutorWindow();
        return g_execWindow.load(std::memory_order_relaxed) != nullptr;
    }

    HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, ExecutorThreadHook, nullptr,
                                   target);
    if (!hook) {
        TraceLog(L"exec window hook failed err=%u", GetLastError());
        return false;
    }
    const ULONGLONG start = GetTickCount64();
    while (!g_execWindow.load(std::memory_order_relaxed) &&
           !g_disabled.load(std::memory_order_relaxed) &&
           !(g_execStopEvent &&
             WaitForSingleObject(g_execStopEvent, 0) == WAIT_OBJECT_0) &&
           GetTickCount64() - start < timeoutMs) {
        Sleep(5);
    }
    UnhookWindowsHookEx(hook);
    const bool ready = g_execWindow.load(std::memory_order_relaxed) != nullptr;
    if (!ready) {
        TraceLog(L"exec window deferred: launcher thread %u did not pump in "
                L"%llu ms",
                target, static_cast<unsigned long long>(timeoutMs));
    }
    return ready;
}

DWORD WINAPI ExecutorWorkerProc(LPVOID) {
    // Every exit clears readiness, including the unexpected ones: a worker that
    // returned on WAIT_FAILED or a refused wait must not leave the gate open for
    // a command nobody will post. The unload clears it too; this covers the exit
    // that has no unload.
    struct ReadyOnExit {
        ~ReadyOnExit() {
            g_execReady.store(false, std::memory_order_release);
        }
    } readyOnExit;

    // The executor must live on explorer's launcher thread -- the thread that
    // runs OnHotkeyInvoke and the shell's own ShowStartView/Dismiss, not the
    // thread that owns Shell_TrayWnd. Creating the window from a hook frame puts
    // it on whichever thread called that hook; the one-shot WH_GETMESSAGE hook
    // below is targeted at the launcher thread the hooks observed, so the window
    // is created there and that thread's loop dispatches its messages.
    CreateExecutorWindowOnUiThread(2000);

    // Availability is published HERE, not by the thread that called
    // CreateThread. The new thread can run (and exit) before CreateThread
    // returns; a creator's store of `true` after the exit guard's `false` would
    // leave the gate open for a worker that no longer exists. The gate also
    // reads g_disabled, which BeforeUninit sets before it signals the stop.
    if (!g_disabled.load(std::memory_order_relaxed)) {
        g_execReady.store(true, std::memory_order_release);
        TraceLog(L"exec worker ready tid=%u window=%p", GetCurrentThreadId(),
                g_execWindow.load(std::memory_order_relaxed));
    }

    HANDLE events[2] = {g_execStopEvent, g_ipc.execEvent};
    for (;;) {
        // The only deadline this worker owns is the launcher symbol probe. HookSymbols
        // blocks on Windhawk's PDB fetch for twinui.pcshell.pdb, and running that on
        // a thread Explorer owns freezes the shell, so the install is scheduled from
        // here; the command event still wakes the loop at once.
        //
        // The probe is throttled by `g_execProbeNextTick` while an attempt is
        // pending, and it is over for good once the symbols resolved or the 240 s
        // budget refused them: at that point this wait has no timeout at all and the
        // worker sleeps on its two events. The executor's own timers — the dismiss
        // delay, the visibility confirmation and the press latch — are message
        // timers on the launcher thread's window, which the worker never services,
        // so they add no wake here; they are listed in the AGENTS contract so a
        // later change does not turn this into a poll again.
        DWORD waitMs = INFINITE;
        if (!g_explorerSymbolsReady.load(std::memory_order_acquire) &&
            !g_explorerSymbolsRefused.load(std::memory_order_acquire)) {
            const uint64_t now = GetTickCount64();
            waitMs = g_execProbeNextTick > now
                         ? static_cast<DWORD>(g_execProbeNextTick - now)
                         : static_cast<DWORD>(kExecSymbolProbeMs);
        }
        const DWORD wait = WaitForMultipleObjects(2, events, FALSE, waitMs);
        if (wait == WAIT_OBJECT_0) {
            return 0;
        }
        if (wait == WAIT_FAILED) {
            TraceLog(L"exec worker wait failed err=%u", GetLastError());
            return 0;
        }
        if (wait == WAIT_TIMEOUT) {
            ProbeExplorerSymbolsFromWorker();
            continue;
        }
        if (wait != WAIT_OBJECT_0 + 1) {
            TraceLog(L"exec worker wait ended unexpectedly result=%u", wait);
            return 0;
        }
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        for (int i = 0; i < 50 && !window; ++i) {
            Sleep(1);
            window = g_execWindow.load(std::memory_order_relaxed);
        }
        if (!window) {
            TraceLog(L"exec command dropped: no window");
            CreateExecutorWindowOnUiThread(500);
            window = g_execWindow.load(std::memory_order_relaxed);
            if (!window) {
                continue;
            }
        }
        if (!PostMessageW(window, kExecMessage, 0, 0)) {
            TraceLog(L"exec command post failed err=%u", GetLastError());
        }
    }
}

void StartExecutorWorker() {
    if (g_execWorker || !g_ipc.execEvent) {
        return;
    }
    g_execStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_execStopEvent) {
        TraceLog(L"exec worker stop event failed err=%u", GetLastError());
        return;
    }
    g_execWorker = CreateThread(nullptr, 0, ExecutorWorkerProc, nullptr, 0,
                                nullptr);
    if (!g_execWorker) {
        TraceLog(L"exec worker thread failed err=%u", GetLastError());
        CloseHandle(g_execStopEvent);
        g_execStopEvent = nullptr;
        return;
    }
    g_execWorkerId = GetThreadId(g_execWorker);
    TraceLog(L"exec worker started tid=%u", g_execWorkerId);
}

// Joins the posting worker. The stop event is already set by BeforeUninit; the
// worker never synchronously calls the launcher and never waits for an owner
// callback, so this join cannot be held up by the launcher thread's own work.
// A worker that does not exit is a bug that blocks the unload; there is no safe
// return while it can still post into module code.
void JoinExecutorWorker() {
    if (!g_execWorker) {
        return;
    }
    if (WaitForSingleObject(g_execWorker, INFINITE) == WAIT_OBJECT_0) {
        CloseHandle(g_execWorker);
        g_execWorker = nullptr;
        TraceLog(L"exec worker joined tid=%u", g_execWorkerId);
        g_execWorkerId = 0;
        return;
    }
    ULONGLONG nextReport = 0;
    for (;;) {
        const ULONGLONG now = GetTickCount64();
        if (now >= nextReport) {
            nextReport = now + 5000;
            TraceLog(L"exec worker join failed err=%u tid=%u", GetLastError(),
                    g_execWorkerId);
        }
        Sleep(1000);
    }
}

// Destroys the one published window on its owner thread and waits for
// WM_NCDESTROY. Called after the worker is joined, so no new post can arrive.
// DestroyWindow is asynchronous only across threads: the teardown message makes
// the owner run it, and WM_NCDESTROY signals the event. On the owner thread it
// runs inline. A window that does not go away has no safe successful unload.
void DestroyExecutorWindow() {
    HWND window = g_execWindow.load(std::memory_order_acquire);
    if (!window) {
        return;
    }
    if (g_execThreadId == GetCurrentThreadId()) {
        DestroyWindow(window);
    } else {
        if (!g_execWindowGone) {
            TraceLog(L"exec window gone event missing hwnd=%p", window);
            for (;;) {
                Sleep(5000);
            }
        }
        ResetEvent(g_execWindowGone);
        if (!PostMessageW(window, kExecTeardownMessage, kExecTeardownMagic, 0)) {
            if (!g_execWindow.load(std::memory_order_acquire)) {
                return;  // it died before the message
            }
            ULONGLONG nextReport = 0;
            for (;;) {
                const ULONGLONG now = GetTickCount64();
                if (now >= nextReport) {
                    nextReport = now + 5000;
                    TraceLog(L"exec window teardown post failed err=%u hwnd=%p",
                            GetLastError(), window);
                }
                Sleep(1000);
            }
        }
        if (WaitForSingleObject(g_execWindowGone, INFINITE) != WAIT_OBJECT_0) {
            ULONGLONG nextReport = 0;
            for (;;) {
                const ULONGLONG now = GetTickCount64();
                if (now >= nextReport) {
                    nextReport = now + 5000;
                    TraceLog(L"exec window teardown wait failed err=%u hwnd=%p",
                            GetLastError(), window);
                }
                Sleep(1000);
            }
        }
        TraceLog(L"exec window destroyed tid=%u",
                static_cast<unsigned>(g_execThreadId));
    }
    if (g_execWindow.load(std::memory_order_acquire)) {
        // DestroyWindow refused or the owner never published the destruction.
        // Freeing the class or the image under it is not an option.
        HWND remaining = g_execWindow.load(std::memory_order_acquire);
        ULONGLONG nextReport = 0;
        for (;;) {
            const ULONGLONG now = GetTickCount64();
            if (now >= nextReport) {
                nextReport = now + 5000;
                TraceLog(L"exec window still published hwnd=%p tid=%u",
                        remaining, static_cast<unsigned>(g_execThreadId));
            }
            Sleep(1000);
        }
    }
}

// Unregisters the fixed class under the instance that registered it. The window
// is gone by now, so no procedure can still run. A failure other than "already
// gone" leaves the WndProc reachable through the class, so the unload keeps
// trying instead of returning into an image the class still points at.
void UnregisterExecutorClass() {
    if (!g_execClassRegistered) {
        return;
    }
    for (;;) {
        if (UnregisterClassW(kExecClassName, g_execClassInstance)) {
            TraceLog(L"exec window class unregistered");
            break;
        }
        const DWORD error = GetLastError();
        if (error == ERROR_CLASS_DOES_NOT_EXIST) {
            TraceLog(L"exec window class already gone");
            break;
        }
        TraceLog(L"exec window class unregister failed err=%u", error);
        Sleep(1000);
    }
    g_execClassRegistered = false;
    g_execClassInstance = nullptr;
}

void PublishBridgeIntent(int32_t kind, int32_t trigger, uint32_t method,
                         uint32_t flags) {
    if (!g_ipc.state) {
        return;
    }
    if (kind == kSwipeIntentExternal) {
        // External ownership supersedes every reopen this mod still holds: the
        // takeover is closing or replacing the menu, and a latched open would
        // fire into it.
        DropPressLatch(L"external", L"intent");
    }
    uint64_t open = 0;
    uint64_t openTick = 0;
    SwipeIpcReadOpen(&g_ipc, &open, &openTick);
    const LONG64 sequence =
        SwipeIpcPublishIntent(&g_ipc, kind, trigger, method, flags, open);
    TraceLog(L"intent kind=%s trigger=%s seq=%lld open=%llu method=%u flags=%u",
            kind == kSwipeIntentOpen
                ? L"open"
                : (kind == kSwipeIntentClose ? L"close" : L"external"),
            trigger == kSwipeTriggerWin
                ? L"win"
                : (trigger == kSwipeTriggerStartButton
                       ? L"startbutton"
                       : (trigger == kSwipeTriggerClickAway ? L"clickaway"
                                                            : L"override")),
            static_cast<long long>(sequence),
            static_cast<unsigned long long>(open), method, flags);
}


BOOL RoleInit() {
    SetTraceFileName(L"bridge.log");
    RefreshTraceSetting();
    Wh_Log(L"Legacy Start Swipe 0.7.0 initializing (explorer role)");

    // The channel to the Start side. Opening it here means the section and its
    // events exist before the first Hide, and a Start mod that attaches later
    // joins the same ones. This is the only call: SwipeIpcOpen may sleep for its
    // init budget while the other side finishes a claim, so it must not run on
    // a hook. A failure here means every close runs at once for the life
    // of this attachment, which the budget makes reachable only for a section
    // that cannot be initialised at all.
    bool created = false;
    DWORD ipcError = 0;
    if (!SwipeIpcOpen(&g_ipc, &created, &ipcError)) {
        Wh_Log(L"Swipe IPC channel unavailable (%u); the shell's own path "
               L"serves every trigger",
               static_cast<unsigned>(ipcError));
    } else {
        TraceLog(L"ipc open created=%d", created ? 1 : 0);
    }

    // The acknowledgement event for the window's destruction. Created before
    // any window can exist; the unload resets it before it posts the teardown
    // and waits on it without a deadline.
    g_execWindowGone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_execWindowGone) {
        TraceLog(L"exec window gone event failed err=%u", GetLastError());
    }

    if (g_ipc.state) {
        StartExecutorWorker();
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase
                              ? reinterpret_cast<LoadLibraryExW_t>(
                                    GetProcAddress(kernelBase,
                                                   "LoadLibraryExW"))
                              : nullptr;
    if (!loadLibraryExW) {
        Wh_Log(L"LoadLibraryExW not found; twinui.pcshell.dll hooks may not "
               L"be installed");
    } else if (!Wh_SetFunctionHook(
                   reinterpret_cast<void*>(loadLibraryExW),
                   reinterpret_cast<void*>(LoadLibraryExW_Hook),
                   reinterpret_cast<void**>(&LoadLibraryExW_Original))) {
        Wh_Log(L"Failed to hook LoadLibraryExW; twinui.pcshell.dll hooks may "
               L"not be installed");
    } else {
        Wh_Log(L"Hooked LoadLibraryExW");
    }

    // Record the image if it is already loaded; resolution happens on the
    // executor worker so the PDB fetch never blocks Explorer's thread.
    NoteLoadedExplorerModule();

    g_initialised = true;
    return TRUE;
}

void RoleAfterInit() {
    if (!g_initialised) {
        return;
    }
    g_afterInit = true;
    NoteLoadedExplorerModule();
    Wh_ApplyHookOperations();
}

void RoleBeforeUninit() {
    if (!g_initialised) {
        return;
    }
    // This must be set before Windhawk removes the hooks. Every hook and the
    // executor's WndProc reads it: a close or a dispatch that enters after this
    // point skips the channel. The lock is what makes the flag authoritative
    // for the worker's symbol install: a HookSymbols call that is already inside
    // the lock finishes first (and its install sees the flag), and one that
    // starts after this point refuses before it queues anything.
    AcquireSRWLockExclusive(&g_explorerSymbolLock);
    g_disabled = true;
    ReleaseSRWLockExclusive(&g_explorerSymbolLock);
    // Dispatch is closed before the worker is signalled: no new message may be
    // posted for a channel that is about to be unmapped, and the worker's own
    // wait sees the stop before the Exec event.
    g_execReady.store(false, std::memory_order_release);
    if (g_execStopEvent) {
        SetEvent(g_execStopEvent);
    }
}

void RoleUninit() {
    if (!g_initialised) {
        return;
    }

    // The posting worker first. It never calls the launcher synchronously, so
    // the join does not depend on the launcher thread; it sees the stop event
    // set in BeforeUninit.
    g_execReady.store(false, std::memory_order_release);
    if (g_execStopEvent) {
        SetEvent(g_execStopEvent);
    }
    JoinExecutorWorker();

    // Then the one window, on its owner thread, with the worker already joined:
    // no new post can arrive and no timer can survive the destruction.
    DestroyExecutorWindow();

    // The class is unregistered only after the window is gone, under the same
    // instance that registered it. A class that outlives the image would point
    // at an unmapped WndProc.
    UnregisterExecutorClass();

    if (g_execWindowGone) {
        CloseHandle(g_execWindowGone);
        g_execWindowGone = nullptr;
    }
    if (g_execStopEvent) {
        CloseHandle(g_execStopEvent);
        g_execStopEvent = nullptr;
    }

    // The channel last: no hook path and no worker reads it after this point.
    if (g_ipc.state) {
        SwipeIpcClose(&g_ipc);
        TraceLog(L"ipc close");
    }

    g_initialised = false;
    Wh_Log(L"Legacy Start Swipe uninitialized (explorer role)");
}

// The explorer role reads no setting: the one settings block belongs to the
// controller, and nothing here refreshes anything.
void RoleSettingsChanged() {
    if (!g_initialised) {
        return;
    }
    RefreshTraceSetting();
}

// The explorer half: the launcher hooks and the executor. Selected only in
// explorer.exe.
const SwipeModRole Role = {RoleInit, RoleAfterInit, RoleBeforeUninit,
                           RoleUninit, RoleSettingsChanged};

}  // namespace bridge

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

namespace {

// The host's executable name decides the half. The path is read with
// GetModuleFileNameW and the basename is compared case-insensitively with the
// two full names; an unreadable or truncated path, or any other host, selects
// nothing and Wh_ModInit installs nothing. One selection per process: the other
// callbacks read g_modRole and never rediscover the process.
const SwipeModRole* ResolveModRole() {
    wchar_t path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (length == 0 || length >= ARRAYSIZE(path)) {
        return nullptr;
    }

    const wchar_t* name = path;
    for (const wchar_t* cursor = path; *cursor; cursor++) {
        if (*cursor == L'\\' || *cursor == L'/') {
            name = cursor + 1;
        }
    }

    if (CompareStringOrdinal(name, -1, L"explorer.exe", -1, TRUE) ==
        CSTR_EQUAL) {
        return &bridge::Role;
    }
    if (CompareStringOrdinal(name, -1, L"StartMenuExperienceHost.exe", -1,
                             TRUE) == CSTR_EQUAL) {
        return &start::Role;
    }
    return nullptr;
}

// Set before any other operation of Wh_ModInit and read by the other three
// callbacks. nullptr means no role was selected, so nothing is installed.
const SwipeModRole* g_modRole = nullptr;

}  // namespace

BOOL Wh_ModInit() {
    // The first operation, before either half touches anything: this process
    // picks its half, and the other half's init never runs here.
    g_modRole = ResolveModRole();
    if (!g_modRole) {
        return FALSE;
    }
    return g_modRole->modInit();
}

void Wh_ModAfterInit() {
    if (g_modRole) {
        g_modRole->modAfterInit();
    }
}

void Wh_ModBeforeUninit() {
    if (g_modRole) {
        g_modRole->modBeforeUninit();
    }
}

void Wh_ModUninit() {
    // A refused init returns here too, and the selected half unwinds only what
    // it acquired: its own flags say how far it got.
    if (g_modRole) {
        g_modRole->modUninit();
    }
}

void Wh_ModSettingsChanged() {
    if (g_modRole) {
        g_modRole->modSettingsChanged();
    }
}
