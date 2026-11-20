// ==WindhawkMod==
// @id              start-menu-touch-gesture-bridge
// @name            Start Menu Touch Gesture Bridge
// @description     Decides the shell's Start close from explorer and publishes the live Win10 Start edge-gesture handle and state
// @version         0.25.0
// @author          Zhanyuanium
// @github          https://github.com/Zhanyuanium
// @license         MIT
// @homepage        https://github.com/Zhanyuanium/swipe-for-10start
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Start Menu Touch Gesture Bridge (explorer side)
> **This is one half of a pair.** The gesture is decided in
> [Legacy Start Edge Swipe](https://windhawk.net/mods/start-menu-legacy-swipe) (`StartMenuExperienceHost.exe`);
> install both mods, or the menu opens without the swipe following the finger and
> closes without the settle.
>
> Requires ExplorerPatcher's legacy Windows 10 Start menu (`Start_ShowClassicMode=1`).

Explorer-side half of the touch-swipe bridge for the ExplorerPatcher legacy
Windows 10 Start menu.

The mod owns the Start toggle on the explorer side. While the Start side's
controller owns the panel, its published phase decides the direction; otherwise
a cached open boolean is resynced from the window's cloak
(`IsStartWindowVisible`) before every decision. It calls the shell's own
primitives — `XamlLauncher::ShowStartView` to open, `XamlLauncher::Dismiss` to
close — so the shell keeps every visual and state transition. Nothing here
writes the launcher's state.

The triggers and their entries:

- The Windows key and the shell hook enter `XamlLauncher::OnHotkeyInvoke`
  (RVA `0x14195C`): the launcher is `self + 0x18`. A bare press toggles through
  the decision. A press with Shift/Ctrl/Alt held is a combination: with the menu
  open it is forwarded as an external takeover and never animates, with the menu
  closed it is swallowed and never opens Start. This is a hard gate; a press
  during the controller's own close is an open by the published phase, not by
  the shell's state.
- The taskbar Start button does **not** call `XamlLauncher::OnStartButtonPressed`
  on this build, although that hook is installed and probed. It calls
  `ShowStartView` / `Dismiss` directly: show method 11 is the Start button's
  open, dismiss method 6 its close, and show method 16 is the shell's own edge
  gesture. Any other call is swallowed while the controller owns the panel
  (`exec swallow ... reason=owning`) or passed with a cloak resync.
- A click outside enters `XamlLauncherState`'s dismiss handler
  (`0x24FC50` on this build) and calls `HideStartAndCortana(7)` directly; an
  older delegate (`0x4916D0`) does the same. Both are closes of ours and animate.
- `DismissSynchronouslyWithoutTransition` (lock, display change) and any caller
  this build does not recognise pass through at once, with only a cloak resync.
  A lock is never delayed. Win+Tab enters as method 5 and passes the same way.

Every close executes in one place: the Start side plans the mirrored close and
publishes `exec dismiss` at the end of its settle plus 30 ms; the bridge's timer
runs `XamlLauncher::Dismiss` on the launcher thread. A reveal contact that was
driving the panel becomes a deferred pull in the Start mod: the menu closes, and
the pull may open the next one. A close the Start side's own gesture causes runs
the same command, scheduled at the settle's end. A command the publisher has
already given up on is not run late: a dispatch that starts at or past 400 ms after
its scheduled time is answered expired (`exec show|dismiss ... expired`) and the
primitive is not called. A successful dismiss is not reported as a hide on the
strength of `hr=0` alone; a 300 ms probe records whether the window really went
away (`exec dismiss confirmed|unconfirmed|unknown seq=... visible=hidden|visible|unknown`),
and `unconfirmed`/`unknown` is a terminal repair on the Start side. A record whose
publisher activation changed is answered `superseded` immediately before the
call; the Start side also cancels a record the previous activation left queued
when it attaches.

A press during a close is a reopen, and the direction comes from the Start side's
published controller direction, never from the shell's. That controller publishes
`closing` before a close's first frame, so the bridge's one toggle rule reads the
press as an open for the whole close, including the window's last visible
frame. A close still settling is cancelled while it is still the Start side's to
cancel: the Start side clears its local dismiss slot and any command it has not
sent yet, and settles the panel back from its live position. Once the executor's
dismiss is armed the bridge latches the press instead (below). A dismiss that
has already entered the shell's `Dismiss` call
cannot be recalled; a press there is latched (one slot) and served as the same
open intent when the hide lands (`own press served wait=...`). The serve is armed by the hidden event and
published 50 ms later, off that event's stack: a show at the hidden event's own
tick was swallowed and the window stayed in state 3, while a show 16 ms after it
opened normally. For the gesture path's own dismiss the landing instead reissues
the cancelled open (`intent open recovered`) — but only when the cancel had a
command pending; a local-only cancel arms no recovery ticket, so an unrelated
hide of the same menu is not served. The immediate `ShowStartView`
inside the shell's own Hide was measured to wedge the UI thread, so the latch is
the action for that shape. The latch is not trusted: every newer press supersedes
it, it stores the epoch the admission was decided under, a serve re-reads the peer
and that epoch, and a latch whose hide
never lands (a refused or expired dismiss) is dropped with
`own press latch dropped reason=...` instead of retrying. A latch whose first
timer cannot be armed is dropped too: the hidden event alone is not a guaranteed
serve point.

External takeovers (`Win+Tab`, the lock/display method-7 path, and a modified
Windows press the Win hook can recognise) publish `intent=external` and pass
through immediately: no close intent, no settle, no hold, no animation. The Start
side reacts to that intent: it revokes both close schedules (the local gesture
deadline and the executor dismiss) and the cancel-recovery ticket, clears the
suppression, drops the pull and resyncs from the cloak, so a pull that was under
the takeover is dropped (`reveal dropped (window closed by shell)`) instead of
reopening it. The bridge's own reaction is the cloak resync.

The mod also observes the taskbar-edge gesture path in `twinui.pcshell.dll`,
then publishes the live `GestureAnimationData` handle (source/provider id), the
edge, the start point and the gesture state to shared memory:

- mapping: `Local\WindhawkStartGestureShared` (`StartGestureShared`)
- event:   `Local\WindhawkStartGestureEvent` (auto-reset, signalled on write)

A StartMenuExperienceHost-side consumer could read the mapping and bind a
Composition animation with `InputAnimationTarget::GetInputAnimationObject`; the
shipping Start mod does not read it. Its live couplings are the intent and its
ack, the executor command and its ack, the open generation, the controller
direction and the Start side's liveness, all in
`Local\SwipeFor10Start_v7_*` (see `swipe_ipc_shared.h`).


Hooked functions are forwarded unchanged except for the triggers above: the bare
Win press and the Start button's open (ShowStartView method 11) and close
(Dismiss method 6) are decided, and the shell's own duplicate show or dismiss is
swallowed only while the controller owns the panel.
The mod creates one hidden message-only window and one worker thread for the
executor, and no COM objects. It refuses to initialize unless its own image can
be pinned, and on unload it stops the executor first: admission to window
creation closes, a creator that is inside is drained, the worker is joined, and
the identified window and its timers are destroyed — directly on the owner thread,
otherwise through a private teardown message the owner answers, and only for the
window this activation recorded. A re-enable proves the creator is out before it
takes the window snapshot and refuses rather than clearing a reservation; the
window's procedure also refuses every ordinary dispatch for an HWND other than the
published current one, so a survivor cannot act on the new activation's channel.
It then waits with no deadline until
every close-hook frame and every executor dispatch that is inside has left, and
closes the channel only when the executor's teardown also completed. A frame that
does not return therefore holds the whole Windhawk reload until it does; that is
the price of never handing the host a trampoline a live frame can still call. A
join, a creator drain or a window-close timeout keeps every handle and the channel,
defers the close with `ipc close deferred`, and makes the next activation refuse
until that exact join or destruction has completed (`exec reinit refused: ...`); a
worker that exits later is reaped and the enable then proceeds.

Notes:
- Windhawk compiles the mod from stdin, so the repository `mods` folder is
  passed via `@compilerOptions -I...` to resolve `start_gesture_shared.h` and
  `swipe_ipc_shared.h`. Update that path if the repository is moved.
- RVAs are specific to `twinui.pcshell.dll` 26100.9949 (x86-64). Before any
  hook is installed, the loaded module must report that exact file version
  (and, when readable, the PE timestamp of the loaded image must match the file
  on disk). Otherwise the mod logs one mismatch line and installs nothing.
- Every own hook is probed before it is installed: the target must lie in an
  executable section of the loaded image, its first 24 bytes must equal the
  build's prologue, its body must still carry the landmark the decision was read
  from, and the direct call sites the executor depends on must still target the
  functions they name. Each failure logs its own
  `own hook skipped name=... build=... ...` line and only that hook is left out;
  the observer hooks keep working, and a close is then not animated instead of
  being delayed by a hook that cannot decide.
- The caller-site probe is a capability, not a warning. `HideCallerSitesProbe`
  proves that the return addresses the close hook reads are the ordinary, the
  click-away and the lock call sites; without it no close can be attributed, so
  the decision hooks (the primitives, the Win key and the Start button) are not
  installed and every unverified close passes to the shell. The installed close
  hook then answers `Unknown` for every caller and never animates one.
- Input is intercepted only while the Start side is alive, its liveness tick is
  younger than 600 ms, the executor worker runs, the call is on the recorded
  launcher thread, and the window and the primitive the decision needs exist. The
  bridge creates the channel, so a mapped section proves nothing by itself: with
  the Start mod disabled the section is still live. Every trigger is then
  forwarded to the shell unchanged and logged as `own pass trigger=...
  reason=no-peer` (or `no-executor`/`wrong-thread`/`no-window` for a missing
  executor, and `no-primitive` for the close intent at `HideStartAndCortana`);
  nothing is swallowed for a command nobody would execute. "Alive" is the intent
  consumer's readiness (window, message loop, intent event), not the digitizer's:
  a refused raw-input registration is logged and does not withdraw it. A clean
  disable
  publishes `peerState=gone` at once; a killed host is caught when its tick goes
  stale. That 600 ms is a detection budget, not a delivery guarantee. A press the
  bridge already swallowed is not forwarded later if its peer dies before the
  serve: it is dropped.
- Hooks are installed only after `twinui.pcshell.dll` is loaded, via a
  `LoadLibraryExW` hook plus the module check in `Wh_ModAfterInit`.
- The sequence field is a seqlock: a reader must retry when it is odd or when
  it changed between two reads. The IPC channel uses the same rule per record; a
  single section-wide counter would not work because the two sides write
  different records.
- `edge` is written verbatim from the shell. In the Phase 0 log, a swipe that
  started at (45, 900) on a 1440x900 screen (taskbar on the left) reported
  edge 0, matching `StartGestureEdge::Left`.
- The point is published once per gesture from `ReportEdgeGesture`, in the same
  write as the edge and the handle. There is no per-frame position sampling.
- Each gesture state transition appends one line to `bridge.log` in the mod
  storage folder: `state=... edge=... src=... prov=... <reason>`. The channel to
  the Start mod logs one `ipc open created=...` at init, one
  `activation state reset` when a re-enable re-initialises the pinned image (a
  refused one logs `mod init refused: ...` and installs nothing instead), and
  one `intent kind=... trigger=... seq=... open=...` line per decision.
- The two channels have two lifetimes. The v7 channel (the section mapping and
  view, the mutex and the five events) is closed when the mod is disabled, and
  only then: the disable flag keeps new closes out of it, the executor's own
  teardown has completed (otherwise the close is deferred with
  `ipc close deferred: ...` and every handle stays), and the unload has waited
  for every close that already entered to leave. That
  wait has no deadline, so its outcome is one of two. `drained`: the count
  reached zero, the channel is closed, the unload returns. `stalled`: the count
  is still nonzero and nothing else happens — no close, no return, no handle
  left behind. The price of `stalled` is exact: a close that does not return,
  because the shell call under it never came back or a debugger suspended its
  thread, holds the Windhawk reload until it resumes. A re-enable of the pinned
  image reuses a channel an earlier activation left open instead of opening a
  second one over it, and refuses (`exec reinit refused: ...`) while the deferred
  teardown still owns it.
  The gesture mapping and its event stay for the rest of the explorer process
  lifetime instead: a consumer maps them independently, and an in-flight gesture
  callback may still hold the view, so unmapping them is unsafe. The image is
  pinned, so a re-enable re-initialises this same mapping instead of opening a
  new one; a different installed version maps its own handles and leaves the
  older image's pair to process exit. That is two handles per pinned image,
  deliberate; process exit reclaims them.
- The in-flight count buys the unload's close, and one more thing. The lease
  spans both close hooks, the original calls included, so a count of zero also
  says that no frame of either hook can still need the MinHook trampoline. That
  zero is the only condition on which this mod hands trampoline removal back to
  the host: a frame that is still about to call its original holds a lease, so
  no timeout can stand in for the count, and a timeout that expired under a live
  frame would return to the host a frame whose trampoline it is about to free.
  An ordinary unload is quiet at once, because every wait inside the hooks ends
  within one 10 ms step of the disable flag; a wait long enough to be noticed
  means a frame that cannot run at all. 500 ms is where the wait first reports
  one (`ipc unload drain over 500 ms closes=...`), and from then on it reports
  every 10 s (`ipc unload stalled closes=... waited=... ms`) until the count
  reaches zero. Both lines are diagnostics: neither ends the wait and neither
  changes what happens next.
- The count answers "is a close still inside", never "this close runs after that
  one": two closes on different threads hold it at once, and a re-entrant close
  on one thread holds one lease per frame, so it balances by itself. The lease is
  taken before the hook reads anything and released on every exit, so a path
  that returns early and an exception out of `FileLog` both release it exactly
  once. A call that has entered the detour but has not yet taken its lease is
  invisible to the count; nothing inside the mod can close that window.
- Two limits survive, and they are not the same limit. For an uncounted entry
  the pin keeps this image's code and the lease destructor mapped, so its return
  address and its release stay valid. For the trampoline under a frame, the
  allocation is the host's: Windhawk does not promise a drained hook at
  `Wh_ModUninit`. The shipped engine reports 2.0.0-alpha.8 and its `windhawk.dll`
  carries the same `LoadedMod::BeforeUninit` / `_Z12Wh_ModUninitv` lifecycle, and
  at that tag `ModsManager::ReloadModsAndSettings` calls `mod.BeforeUninit()`
  (which queues the disable), applies it with `MH_ApplyQueuedEx`, then calls
  `mod.Uninitialize()` — `Wh_ModUninit` — and only after that waits for threads
  inside the mod image with `ThreadsCallStackWaitForRegions(..., 200, 400)`
  before `m_mods.erase(it)` frees the module; `~CustomizationSession` reaches the
  same order by destroying its `MinHookScopeApply` (which disables every hook)
  before its `ModsManager`. `LoadedMod::BeforeUninit` says what it waits for
  itself: "Waits for in-flight hook creations, then locks out new ones." That
  wait is bounded and its result is discarded, so "the scan found the frame" and
  "the frame exited before removal" are two claims: the first is what the host
  gives when the scan can see the stack, the second is what this mod's drain
  gives for every counted frame. `MH_RemoveHookEx` frees a trampoline only in
  `~LoadedMod`, after that wait, and every caller of the original has this hook
  below it on the stack, so a scan that sees the frame is enough for that frame;
  a scan blind to it leaves a return into freed code, and that is Windhawk's
  unload to fix, not a mod's. Neither claim reaches a trampoline, because a
  trampoline is not in this image: the pin is the cover for this module's code,
  and the drain is the cover for every counted frame's trampoline, because it
  returns only at zero. What is left is the entry window the count cannot see, and
  only the host's scan can cover that.
- The pin is a prerequisite. `Wh_ModInit` returns `FALSE` when it fails and logs
  `mod init refused: the image could not be pinned; no hook, no channel and no
  thread installed`; a shell-owned close thread needs valid return code as much
  as a worker would, so installing a hook over an image that may go away is not
  an option. Success is
  `GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
  GET_MODULE_HANDLE_EX_FLAG_PIN)` on an address in this module, logged once as
  `mod pin image=... pinned=1` (or `mod pin failed err=...`). The price is an
  image that stays mapped for the rest of the explorer process lifetime: a
  re-enable re-initialises the same pinned DLL, and a version change leaves the
  old image mapped as well. Against a return into unmapped code, that mapping is
  the cheaper of the two risks.
- A re-enable refuses while the previous activation still has a close inside:
  `mod init refused: a close from the previous activation is still inside; no
  hook, no channel and no thread installed`. Clearing `g_disabled` and
  re-pointing the globals under that frame would hand it a half-reset module, so
  the old state stays exactly as the unload left it. A drained unload returns at
  zero, so this refusal is reachable only through the entry window the count
  cannot see; it is kept because that window is real. The same refusal covers an
  executor teardown that did not complete (a join timeout, a window-close post
  failure, an unacknowledged `WM_CLOSE`, or a creator that did not leave
  `CreateWindowExW` in 500 ms): the handles and the channel stay with the live
  participant, and `exec reinit refused: ...` names which one. A worker that has
  exited since is joined and its handles closed when the next enable asks, and a
  window destroyed since clears the identified stop target then, so the refusal
  is not permanent. When nothing from the previous activation is left,
  `ResetReinitState` clears the lifecycle flags, the executor's pending command,
  latch and thread ids, reopens admission, retries a failed gesture mapping, and
  the init logs `activation state reset`; a ready mapping and a channel an
  earlier activation left open are reused rather than rebuilt.

The decision and the executor:

- The bridge decides the direction from the Start side's one published
  direction word. `closing` means a close is in flight, so a Win, Start-button or
  click-away press is a reopen; `open` is the open the controller owns, so a
  press is a close; `unowned` falls back to the shell. The shell's cloak and
  `InternalState` are the resync only for a panel the controller does not own.
  The shell cannot decide a controller close: a gesture close retracts the panel
  with the window still uncloaked and state 5 until the executor's `Dismiss`.
- A press during a cancellable close publishes an open intent at once; the Start
  side drops the close's local dismiss slot and its pending executor command and
  settles the panel back
  from its live position. A press during a shell hide (state 9/11) or with an
  executor dismiss already armed is latched in one slot and served as the same
  open intent at the hide's landing (`own press served wait=...`). The Start
  side's own gesture dismiss can be inside the shell's `Dismiss` call when a
  press lands; the landing then reissues the open the press cancelled.
- Every close is one `exec dismiss` from the Start side: the launcher's
  `Dismiss` on the launcher thread. Nothing here holds the shell's
  `HideStartAndCortana` and nothing asks the Start side for a close duration.
- `Hide` is `HRESULT (void* this, uint32_t reason)` on
  twinui.pcshell.dll 26100.9949. The prologue reads `rcx` (this) and `edx`
  (reason), compares `edx` with 7 and returns 0 on success or the negative
  HRESULT of the failing inner call. It never reads `r8` or `r9`.
- `HideStartAndCortana` is `void (void* this, uint32_t method)`. This is the
  interface whose state dword sits at `this+0x28`: the function reads it on
  entry and writes 9 there before it calls the manager. The hook reads it for
  the log only; it never writes it.
*/
// ==/WindhawkModReadme==

/*
 * [INPUT]: twinui.pcshell.dll 10.0.26100.9949 in explorer.exe: the launcher
 *   singleton at +0x948E80 (the outer object; the execution interface is
 *   +0x18), ShowStartView +0x141AF0, Dismiss +0x25D670, IsVisible +0x1F9B40,
 *   XamlLauncher::OnHotkeyInvoke +0x14195C, XamlLauncherState::OnStartShown/
 *   OnStartHidden, HideStartAndCortana +0x24BCB0, and CStartExperienceManager::
 *   Show/Hide for the observation lines. The launcher thread is the thread that
 *   runs OnHotkeyInvoke (measured tid=7108); the taskbar thread is a different
 *   thread and a show there fail-fasts explorer (measured). Also depends on
 *   swipe_ipc_shared.h v7 for the intent/ack, executor command/ack, open
 *   generation, controller direction and peer liveness, and on
 *   start_gesture_shared.h for the diagnostic animation ids.
 * [OUTPUT]: the hidden executor window WindhawkSwipeExecutor_<WndProc> on the
 *   launcher thread, fed by a worker on the Exec event; every command logs an
 *   `exec show`, `exec dismiss` or `exec visible` line with the sequence and the
 *   launcher read it used, and the launcher read refuses `reason=not-ready`
 *   when `[+0x160] <= 0x10000`.
 *   One decision per trigger: `own toggle decide trigger=... state=... open=...
 *   kind=... phase=...` and `intent kind=... trigger=... seq=...` for the Win
 *   key, the Start button (ShowStartView method 11 / Dismiss method 6) and the
 *   click-away (HideStartAndCortana method 7). The three physical entries share
 *   one dispatcher, so a reopen from Dismiss(6) asks for Show(11) and a close
 *   from Show(11) asks for Dismiss(6); the click-away stays directional.
 *   IntentShouldClose is the only toggle rule (the controller's one direction
 *   word: closing is a reopen, open is a close; unowned falls back to the cloak
 *   and the internal state). The direction is never read from the shell while
 *   the controller owns: a gesture close retracts the panel with the cloak still
 *   open and the state still 5 until the executor's Dismiss, which is why the
 *   controller publishes closing at the close's start. Input is intercepted only
 *   while the Start side is alive (its published tick younger than 600 ms), the
 *   executor worker runs, the call is on the recorded launcher thread, and the
 *   window and primitive the decision needs exist; otherwise every trigger passes
 *   to the shell (`own pass trigger=... reason=no-peer|no-executor|wrong-thread|
 *   no-window`, and `exec pass hide ... reason=no-primitive` for the close
 *   intent). A press is normally recovered: a hide in flight (state 9/11) or a
 *   dismiss already armed is latched (one slot, tick, activation epoch) and
 *   served as the same open intent 50 ms after the
 *   hidden event (`own press served wait=...`); the deferral keeps the show off
 *   the hidden event's stack (a show at its own tick was swallowed, state 3, no
 *   Shown). A close still cancellable publishes open at once and the Start side
 *   settles the panel back from its live position; a newer press supersedes the
 *   slot; a serve re-reads the peer and the epoch; a refused or expired dismiss
 *   invalidates it; and a press lost to the 600 ms detection budget is served by
 *   the shell. The click-away the shell
 *   issues while the window is appearing (state 1/3) and the Hide(7) inside our
 *   own ShowStartView in state 1/3 are the pointer-away check and are swallowed
 *   (`reason=opening`/`reason=own-show`). The shell's own show/dismiss is
 *   swallowed only while the controller owns the panel and only as a duplicate
 *   (method 16/other dismiss); the opening touch's click-away is swallowed while
 *   `controllerTouch`. External takeovers (Win+Tab, lock/display, a forwarded
 *   modified press) publish `intent=external` and pass at once. The v7 channel
 *   closes on unload after the worker is joined and the identified window and its
 *   timers are gone, at counted quiescence (`ipc unload drained`); a teardown that
 *   did not complete defers that close (`ipc close deferred`) and the next
 *   activation refuses until it does. The process-lifetime
 *   gesture mapping is kept for its independent consumer.
 * [POS]: the explorer-side executor and trigger adapter. It does not decide a
 *   toggle: the Start side consumes the intents, runs the one mirrored settle,
 *   and publishes `exec show`/`exec dismiss`/`exec cancel`. The bridge performs
 *   the shell call on the launcher's own thread and confirms with IsVisible
 *   (honest 0 right after Show, 1 at the 300 ms probe; for Dismiss a second probe
 *   records whether the window actually went away). The
 *   image is pinned as an init prerequisite; a reinstall gets a new window class
 *   name so no old WndProc can receive the new channel's commands.
 * [PROTOCOL]: Update this header on change, then check AGENTS.md
 */

#include <windows.h>
#include <winver.h>

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#if defined(_MSC_VER)
#include <intrin.h>
#define OWN_RETURN_ADDRESS() _ReturnAddress()
#else
#define OWN_RETURN_ADDRESS() __builtin_return_address(0)
#endif

// ---- begin start_gesture_shared.h (inlined: the store takes one self-contained file) ----
// The shared contract defines the vocabulary for both sides. This file is one
// side of it, so the unused half of these constants is deliberate.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"

#include <cstdint>
#include <windows.h>

// Shared-memory contract between the explorer-side gesture bridge and the
// StartMenuExperienceHost-side renderer.
//
// Explorer writes the live gesture handle and state; Start reads it and binds a
// Composition animation to the legacy Start UI's root visual.
//
// The name is local to the user session. Both processes run as the same user.

constexpr uint32_t kWindhawkStartGestureMagic = 0x53475042;  // 'SGPB'
constexpr uint32_t kWindhawkStartGestureVersion = 1;

constexpr wchar_t kWindhawkStartGestureMappingName[] =
    L"Local\\WindhawkStartGestureShared";
constexpr wchar_t kWindhawkStartGestureEventName[] =
    L"Local\\WindhawkStartGestureEvent";

enum class StartGestureState : uint32_t {
    None = 0,
    Active = 1,
    Committed = 2,
    Cancelled = 3,
};

enum class StartGestureEdge : int32_t {
    Left = 0,
    Top = 1,
    Right = 2,
    Bottom = 3,
};

struct StartGestureShared {
    uint32_t magic;
    uint32_t version;

    // Incremented around every write so the reader can detect a torn update.
    volatile uint32_t sequence;

    volatile uint32_t state;  // StartGestureState
    volatile int32_t edge;    // StartGestureEdge

    // Live input gesture handle from GestureAnimationData.
    volatile int64_t sourceId;
    volatile int64_t providerId;

    // Physical screen coordinates of the invocation/current point. These are
    // only used by the fallback per-frame path.
    volatile float startX;
    volatile float startY;
    volatile float currentX;
    volatile float currentY;

    volatile uint64_t gestureStartTick;
    volatile uint64_t lastUpdateTick;
};

static_assert(sizeof(StartGestureShared) <= 128,
              "StartGestureShared should stay cache-line sized");
#pragma clang diagnostic pop
// ---- end start_gesture_shared.h ----
// ---- begin swipe_ipc_shared.h (inlined: the store takes one self-contained file) ----
// The shared contract defines the vocabulary for both sides. This file is one
// side of it, so the unused half of these constants is deliberate.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <windows.h>


// Shared-memory contract between the StartMenuExperienceHost-side controller and
// the explorer-side bridge.
//
// Two processes, one session-local section, four auto-reset events, one
// manual-reset command event and one mutex. There is no file and no polling:
// every record carries its own sequence number, and a reader accepts a record
// only when that number did not move while the record was being copied.
//
//   Local\SwipeFor10Start_v7_State        SwipeIpcState
//   Local\SwipeFor10Start_v7_Cmd          manual-reset, tools -> Start
//   Local\SwipeFor10Start_v7_CmdWrite     mutex,         one command writer
//   Local\SwipeFor10Start_v7_Intent       auto-reset,   bridge -> Start
//   Local\SwipeFor10Start_v7_IntentAck    auto-reset,   Start -> bridge
//   Local\SwipeFor10Start_v7_Exec         auto-reset,   Start -> bridge
//   Local\SwipeFor10Start_v7_ExecAck      auto-reset,   bridge -> Start
//
// The names are session-local because both processes run as the same user in the
// same session. The version is part of every name, so a layout change is a new
// channel instead of a silent mismatch.
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
// different processes (the tools the command, the Start side the open generation
// and the executor command, the bridge the intent), and a shared counter would
// invalidate a record another side is reading.
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
//   2. The claimer writes magic, version and layoutBytes, then publishes
//      initState = ready (2).
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
//                      while magic, version or layoutBytes disagrees.
//
// Neither side ever destroys a section the other side may hold: SwipeIpcClose
// closes this process's handles only.
//
// ---------------------------------------------------------------------------
// The records
// ---------------------------------------------------------------------------
//
// Command (tools -> Start): one line in the syntax `docs/ipc.md` documents. The
// command event is manual-reset. Only one writer may publish at a time:
// `tools/ipc.ps1` takes kSwipeIpcCommandMutexName.
//
// Open generation (Start -> bridge): a counter the Start side publishes when it
// sees a fresh menu open. The even value is the identity of that menu. A counter
// of 0 is not a generation; SwipeIpcReadOpen reports ERROR_NOT_READY.
//
// Intent (bridge -> Start): the one decision record. kind is open/close/external,
// trigger names the adapter (win, startbutton, clickaway, touch, override), and
// method/flags are the shell call's own. `open` is the generation the bridge saw.
// The Start side answers with intentAck (state, duration, entry, hr), echoing the
// intent's sequence.
//
// Executor command (Start -> bridge): show, dismiss or cancel. dismiss carries
// the GetTickCount64 deadline at which the bridge must call the launcher; cancel
// names the dismiss sequence it supersedes. `execTick` is the publish tick and is
// the deadline of a show and a cancel. A command older than the publisher's
// budget past its deadline is stale: the publisher has already cleared its
// sequence and released ownership, so the bridge answers it as expired instead of
// calling the launcher. The bridge answers with execAck (state, hr, visible),
// where visible is the IsVisible result and is advisory for a dismiss.
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
    L"Local\\SwipeFor10Start_v7_State";
constexpr wchar_t kSwipeIpcCommandEventName[] =
    L"Local\\SwipeFor10Start_v7_Cmd";
constexpr wchar_t kSwipeIpcCommandMutexName[] =
    L"Local\\SwipeFor10Start_v7_CmdWrite";
constexpr wchar_t kSwipeIpcIntentEventName[] =
    L"Local\\SwipeFor10Start_v7_Intent";
constexpr wchar_t kSwipeIpcIntentAckEventName[] =
    L"Local\\SwipeFor10Start_v7_IntentAck";
constexpr wchar_t kSwipeIpcExecEventName[] =
    L"Local\\SwipeFor10Start_v7_Exec";
constexpr wchar_t kSwipeIpcExecAckEventName[] =
    L"Local\\SwipeFor10Start_v7_ExecAck";

constexpr uint32_t kSwipeIpcMagic = 0x53313053;  // 'S' '0' '1' 'S', LE
// v7 adds the Start side's activation identity (`peerEpoch`) beside the liveness
// pair. The section keeps its 744-byte size: `peerEpoch` fills the four bytes
// after `peerState` that alignment had left, so the size alone no longer names
// the layout and the version word, in every object name and in the section, is
// what separates v6 from v7.
// v6 added the liveness pair at the tail. v5 replaced v4's `controllerOwning` +
// `controllerPhase` pair with one direction word and dropped `controllerOpen`,
// which no reader used. Each change moves the section size or its meaning, so the
// version is bumped deliberately and stays part of every object name: a v6
// section left by an older build is a different channel, and `layoutBytes` (or
// `version`, for the same-size v7) refuses it if one is ever handed over anyway.
constexpr uint32_t kSwipeIpcVersion = 7;

// One command per line, in the same syntax `docs/ipc.md` documents.
constexpr size_t kSwipeIpcCommandMaxBytes = 512;

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
    uint32_t version;

    // Tools -> Start. cmdSeq is the record's seqlock counter.
    volatile LONG64 cmdSeq;
    char cmdText[kSwipeIpcCommandMaxBytes];
    volatile LONG64 cmdTick;

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
    // names the adapter.
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
    // ownership beside the previous phase. The v4 section published owning and
    // phase as separate words; a reader could see owning=1 with the old phase,
    // and the bridge read the still-open window as a close during a retract.
    volatile LONG controllerDirection;  // kSwipeDirection*
    // 1 while a touch contact owns the opening panel. The bridge swallows the
    // shell's click-away from that same opening contact; it is the finger that
    // pulled the menu out, not a press outside it.
    volatile LONG controllerTouch;
    // v6/v7: the Start side's liveness and identity. peerTick is the low 32 bits
    // of GetTickCount64, so the reader compares by unsigned difference and a wrap
    // after 49.7 days is not a discontinuity. peerState is kSwipePeer*.
    // peerEpoch counts attachments: it is incremented before the tick and alive,
    // so an epoch read beside `alive` names the activation that published them.
    volatile LONG peerTick;
    volatile LONG peerState;
    volatile LONG peerEpoch;
};

static_assert(offsetof(SwipeIpcState, magic) == 0, "layout");
static_assert(offsetof(SwipeIpcState, version) == 4, "layout");
static_assert(offsetof(SwipeIpcState, cmdSeq) == 8, "layout");
static_assert(offsetof(SwipeIpcState, cmdText) == 16, "layout");
static_assert(offsetof(SwipeIpcState, cmdTick) == 528, "layout");
static_assert(offsetof(SwipeIpcState, openSeq) == 536, "layout");
static_assert(offsetof(SwipeIpcState, openTick) == 544, "layout");
static_assert(offsetof(SwipeIpcState, initState) == 552, "layout");
static_assert(offsetof(SwipeIpcState, layoutBytes) == 556, "layout");
static_assert(offsetof(SwipeIpcState, initTick) == 560, "layout");
static_assert(offsetof(SwipeIpcState, intentSeq) == 568, "layout");
static_assert(offsetof(SwipeIpcState, intentTick) == 576, "layout");
static_assert(offsetof(SwipeIpcState, intentKind) == 584, "layout");
static_assert(offsetof(SwipeIpcState, intentTrigger) == 588, "layout");
static_assert(offsetof(SwipeIpcState, intentMethod) == 592, "layout");
static_assert(offsetof(SwipeIpcState, intentFlags) == 596, "layout");
static_assert(offsetof(SwipeIpcState, intentOpen) == 600, "layout");
static_assert(offsetof(SwipeIpcState, intentAckSeq) == 608, "layout");
static_assert(offsetof(SwipeIpcState, intentAckFor) == 616, "layout");
static_assert(offsetof(SwipeIpcState, intentAckState) == 624, "layout");
static_assert(offsetof(SwipeIpcState, intentAckDuration) == 628, "layout");
static_assert(offsetof(SwipeIpcState, intentAckEntry) == 632, "layout");
static_assert(offsetof(SwipeIpcState, intentAckHr) == 636, "layout");
static_assert(offsetof(SwipeIpcState, execSeq) == 640, "layout");
static_assert(offsetof(SwipeIpcState, execTick) == 648, "layout");
static_assert(offsetof(SwipeIpcState, execKind) == 656, "layout");
static_assert(offsetof(SwipeIpcState, execMethod) == 660, "layout");
static_assert(offsetof(SwipeIpcState, execFlags) == 664, "layout");
static_assert(offsetof(SwipeIpcState, execExecuteAt) == 672, "layout");
static_assert(offsetof(SwipeIpcState, execOpen) == 680, "layout");
static_assert(offsetof(SwipeIpcState, execTarget) == 688, "layout");
static_assert(offsetof(SwipeIpcState, execAckSeq) == 696, "layout");
static_assert(offsetof(SwipeIpcState, execAckFor) == 704, "layout");
static_assert(offsetof(SwipeIpcState, execAckState) == 712, "layout");
static_assert(offsetof(SwipeIpcState, execAckHr) == 716, "layout");
static_assert(offsetof(SwipeIpcState, execAckVisible) == 720, "layout");
static_assert(offsetof(SwipeIpcState, controllerDirection) == 724, "layout");
static_assert(offsetof(SwipeIpcState, controllerTouch) == 728, "layout");
static_assert(offsetof(SwipeIpcState, peerTick) == 732, "layout");
static_assert(offsetof(SwipeIpcState, peerState) == 736, "layout");
static_assert(offsetof(SwipeIpcState, peerEpoch) == 740, "layout");
static_assert(sizeof(SwipeIpcState) == 744, "layout");

struct SwipeIpcChannel {
    SwipeIpcState* state;
    HANDLE mapping;
    HANDLE commandEvent;
    HANDLE commandMutex;
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
// event disables only the direction it carries, a missing mutex serialises
// nothing. On failure `*error` is ERROR_NOT_READY or ERROR_INVALID_DATA as
// described above, or the failing Win32 code.
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
        state->version = kSwipeIpcVersion;
        state->layoutBytes = static_cast<LONG>(sizeof(SwipeIpcState));
        MemoryBarrier();
        InterlockedExchange(&state->initState, kSwipeIpcReady);
        *created = true;
    }

    if (state->initState != kSwipeIpcReady ||
        state->magic != kSwipeIpcMagic || state->version != kSwipeIpcVersion ||
        state->layoutBytes != static_cast<LONG>(sizeof(SwipeIpcState))) {
        // Broken, or written by another layout. Nothing here is repaired: the
        // other side may be live on it.
        *error = ERROR_INVALID_DATA;
        UnmapViewOfFile(state);
        CloseHandle(mapping);
        return false;
    }

    // Auto-reset except for the command event: a command that arrived while the
    // reader was busy must not be forgotten by a second reset.
    HANDLE commandEvent =
        CreateEventW(nullptr, TRUE, FALSE, kSwipeIpcCommandEventName);
    HANDLE commandMutex =
        CreateMutexW(nullptr, FALSE, kSwipeIpcCommandMutexName);
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
    channel->commandEvent = commandEvent;
    channel->commandMutex = commandMutex;
    channel->intentEvent = intentEvent;
    channel->intentAckEvent = intentAckEvent;
    channel->execEvent = execEvent;
    channel->execAckEvent = execAckEvent;
    return true;
}

// Drops this process's handles. The section, its events and its mutex stay alive
// while the other side holds them, so a restarted side reopens the same channel
// and finds the other side's records where it left them.
inline void SwipeIpcClose(SwipeIpcChannel* channel) {
    if (channel->state) {
        UnmapViewOfFile(channel->state);
    }
    if (channel->mapping) {
        CloseHandle(channel->mapping);
    }
    if (channel->commandEvent) {
        CloseHandle(channel->commandEvent);
    }
    if (channel->commandMutex) {
        CloseHandle(channel->commandMutex);
    }
    // Every event this side opened, the intent event included: it was the one
    // handle SwipeIpcClose used to leak, so a re-enable opened a second one.
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

// Copies the newest command when it is newer than `consumed`, and advances it.
// A torn share of the record is retried inside SwipeIpcSnapshot; a record that
// never settles inside the attempt budget is reported as "no command" and is
// taken by the next event, or replaced by the next command.
inline bool SwipeIpcTakeCommand(SwipeIpcChannel* channel, char* buffer,
                                size_t count, LONG64* consumed) {
    if (!channel->state || count < 2) {
        return false;
    }

    char text[kSwipeIpcCommandMaxBytes] = {};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->cmdSeq, &sequence, [&]() {
            std::memcpy(text, channel->state->cmdText, sizeof(text));
        })) {
        return false;
    }
    if (sequence == 0 || sequence == *consumed) {
        return false;
    }

    std::strncpy(buffer, text, count - 1);
    buffer[count - 1] = '\0';
    *consumed = sequence;
    return true;
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
void EnsureExecutorWindow();
DWORD NoteLauncherThread(const wchar_t* source);
void EnsureExecutorOnLauncherThread(const wchar_t* source);
void* LauncherNow(uint64_t* stateOut, void** outerOut = nullptr);
void PublishBridgeIntent(int32_t kind, int32_t trigger, uint32_t method,
                         uint32_t flags);

namespace {

constexpr uintptr_t kTwinuiPreferredBase = 0x180000000;

// twinui.pcshell.dll 26100.9949 (x86-64), verified by the Phase 0 probe.
constexpr uintptr_t kTouchEdgeGestureOperationStartedVa = 0x1800CAD80;
constexpr uintptr_t kTouchEdgeGestureOperationEndedVa = 0x1805F8840;
constexpr uintptr_t kReportEdgeGestureVa = 0x1806F63BC;
constexpr uintptr_t kSetAnimationDataVa = 0x1803C7740;
constexpr uintptr_t kShowVa = 0x180254C90;
constexpr uintptr_t kHideVa = 0x1803C5960;
// XamlLauncherState::HideStartAndCortana, the decision every shell close reaches.
constexpr uintptr_t kHideStartAndCortanaVa = 0x18024BCB0;
// The two decision entries (XamlLauncher): the Windows key / shell hook and the
// taskbar Start button. Both are plain member functions; the launcher pointer is
// `self` for the button and `self + 0x18` for the hotkey.
constexpr uintptr_t kOnHotkeyInvokeVa = 0x18014195C;
constexpr uintptr_t kOnStartButtonPressedVa = 0x180492C90;
// The execution primitives: XamlLauncher::Dismiss (a forwarder to the state's
// DismissStartAndCortana) and XamlLauncher::ShowStartView (no state gate).
constexpr uintptr_t kDismissVa = 0x18025D670;
constexpr uintptr_t kShowStartViewVa = 0x180141AF0;
// The launcher state's two events. OnStartShown completes an open (state = 5);
// OnStartHidden completes a close (9 -> 0xA, 0x11 -> 0, 0xB -> 0, 0xE -> 7).
constexpr uintptr_t kOnStartShownVa = 0x180141DF0;
constexpr uintptr_t kOnStartHiddenVa = 0x180065E70;
// The three call sites that reach HideStartAndCortana, used as the caller
// discriminator. Ordinary is the direct call from DismissStartAndCortana
// (0x499252); ClickAway is the delegate sub_4916D0 (0x4916E2); Lock is
// DismissSynchronouslyWithoutTransition (0x48F7FB, method 7). The recorded value
// is the return address (call site + 5), so a runtime match is exact.
constexpr uintptr_t kHideCallerOrdinaryVa = 0x499257;
constexpr uintptr_t kHideCallerClickAwayVa = 0x4916EB;
constexpr uintptr_t kHideCallerLockVa = 0x48F804;
// The click-away measured on this layout: a desktop click enters
// XamlLauncherState's dismiss handler at 0x24FC50, which calls
// HideStartAndCortana(7) directly at 0x24FDDE. It is a close of ours and must
// animate; the lock path above is the only method-7 close that passes.
constexpr uintptr_t kHideCallerAwayVa = 0x24FDE3;
constexpr uintptr_t kClickAwayHandlerVa = 0x18024FC50;
// The three functions those return addresses belong to, used by the probe.
constexpr uintptr_t kDismissStartAndCortanaVa = 0x1804991C0;
constexpr uintptr_t kClickAwayDelegateVa = 0x1804916D0;
constexpr uintptr_t kLockDismissVa = 0x18048F7C0;
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

// The build the RVAs above were derived from.
constexpr unsigned kTwinuiVersion[4] = {10, 0, 26100, 9949};

// The first bytes of HideStartAndCortana on that build: `mov [rsp+18],rbx;
// mov [rsp+20],rsi; push rbp; push rdi; push r14; lea rbp,[rsp-0x200];
// sub rsp,...`.
constexpr uint8_t kHideStartAndCortanaPrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x18, 0x48, 0x89, 0x74, 0x24,
    0x20, 0x55, 0x57, 0x41, 0x56, 0x48, 0x8D, 0xAC, 0x24,
    0x00, 0xFE, 0xFF, 0xFF, 0x48, 0x81};

// The body bytes the probe requires beyond the prologue: the transient state
// write (`mov dword ptr [rcx+28],9`), the manager load (`mov rcx,[rdi+38]`) and
// its Hide slot (`mov rax,[rax+40]`). A build whose state machine moved but
// whose prologue survived is still refused.
constexpr size_t kHideStartAndCortanaProbeBytes = 0x400;

// The prologues of the four own hooks and the three close entrants, from
// twinui.pcshell.dll 10.0.26100.9949. A probe compares the first 24 bytes before
// a hook is installed; a mismatch logs `own hook skipped ...` and only that hook
// is left out.
constexpr uint8_t kOnHotkeyInvokePrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x10, 0x48, 0x89, 0x7C, 0x24,
    0x20, 0x55, 0x48, 0x8B, 0xEC, 0x48, 0x83, 0xEC, 0x70,
    0x8B, 0xFA, 0x48, 0x8B, 0xD9, 0x48};
constexpr uint8_t kOnStartButtonPressedPrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24,
    0x10, 0x57, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x8B, 0xD9,
    0x41, 0x8B, 0xF0, 0x48, 0x8B, 0x89};
constexpr uint8_t kDismissPrologue[] = {
    0x48, 0x83, 0xEC, 0x28, 0x48, 0x8B, 0x89, 0x60, 0x01,
    0x00, 0x00, 0x48, 0x85, 0xC9, 0x75, 0x07, 0xB8, 0x90,
    0x04, 0x07, 0x80, 0xEB, 0x0E, 0x48};
constexpr uint8_t kShowStartViewPrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x20, 0x55, 0x56, 0x57, 0x41,
    0x56, 0x41, 0x57, 0x48, 0x8D, 0xAC, 0x24, 0x70, 0xFF,
    0xFF, 0xFF, 0x48, 0x81, 0xEC, 0x90};
constexpr uint8_t kOnStartShownPrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x10, 0x48, 0x89, 0x74, 0x24,
    0x18, 0x55, 0x57, 0x41, 0x56, 0x48, 0x8D, 0xAC, 0x24,
    0x00, 0xFF, 0xFF, 0xFF, 0x48, 0x81};
constexpr uint8_t kOnStartHiddenPrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x08, 0x57, 0x48, 0x83, 0xEC,
    0x40, 0x48, 0x8B, 0xF9, 0x48, 0x8D, 0x59, 0x40, 0x8B,
    0x03, 0x89, 0x44, 0x24, 0x20, 0x8B};
constexpr uint8_t kClickAwayDelegatePrologue[] = {
    0x48, 0x83, 0xEC, 0x28, 0x48, 0x8B, 0x41, 0x10, 0x48,
    0x8B, 0x88, 0xA8, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x01,
    0x48, 0x8B, 0x40, 0x48, 0xE8, 0x25};
constexpr uint8_t kLockDismissPrologue[] = {
    0x40, 0x53, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x8B, 0xD9,
    0x48, 0x8B, 0x89, 0x60, 0x01, 0x00, 0x00, 0x48, 0x85,
    0xC9, 0x75, 0x07, 0xB8, 0x90, 0x04};
constexpr uint8_t kClickAwayHandlerPrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x20, 0x44, 0x89, 0x44, 0x24,
    0x18, 0x89, 0x54, 0x24, 0x10, 0x55, 0x56, 0x57, 0x41,
    0x56, 0x41, 0x57, 0x48, 0x83, 0xEC};
constexpr uint8_t kDismissStartAndCortanaPrologue[] = {
    0x48, 0x89, 0x5C, 0x24, 0x08, 0x57, 0x48, 0x83, 0xEC,
    0x20, 0x83, 0x64, 0x24, 0x38, 0x00, 0x8B, 0xFA, 0x48,
    0x8B, 0xD9, 0x89, 0x91, 0x18, 0x01};

// The Windows-key dismiss method (the shell's own measurement) and the method a
// Win+Tab close carries. Method 5 is an external takeover: it is never held and
// never animated.
constexpr uint32_t kDismissMethodWin = 1;
constexpr uint32_t kDismissMethodWinTab = 5;
constexpr uint32_t kDismissMethodStartButton = 6;
constexpr uint32_t kDismissMethodClickAway = 7;

// The M2 retract wait and its tapped-close stamp are retired. The M3 executor
// schedules the dismiss from the duration the Start side plans, and the Start
// side owns the single suppression that keeps a heal from restoring the panel.
// The old reopen latch (`g_ownPendingOpen`, its 1200 ms window and its serve
// timer) is retired too: the live reopen path is the press latch and the Start
// side's own cancel/recovery.

// The hide waits in 10 ms steps, so the wait can end early when the mod is being
// disabled under it.
constexpr DWORD kHidePollMs = 10;

// Where an unload that is still waiting first says so. Diagnostic only: reaching
// this mark grants nothing, and no mark ends the wait.
constexpr DWORD kIpcUnloadMarkMs = 500;

// How often a stalled unload repeats itself while it keeps waiting. Nothing else
// happens on that path: no close, no return, no handle left behind.
constexpr DWORD kIpcUnloadStallReportMs = 10000;

// The cross-process channel (swipe_ipc_shared.h). Opened once at init. Its
// handles are closed by Wh_ModUninit once no Hide can still be inside them; see
// the two lifetimes there. The gesture mapping and its event, unlike this
// channel, stay for the whole process lifetime.
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
// Window creation and window shutdown are one protocol, not two independent
// checks. A creator reserves `g_execCreating`, then asks admission, then
// publishes the HWND and releases the reservation; the stop path closes
// admission, drains the reservation, and then snapshots the HWND. A creator
// therefore either published before that snapshot or is refused after it, which
// is what closes the race in which a creator passed the disabled check before
// the stop saw a null HWND and published afterwards.
std::atomic<bool> g_execAdmissionOpen{true};
std::atomic<bool> g_execCreating{false};
// The window a shutdown owes. Set when the stop snapshots a live window and
// cleared by that window's own WM_NCDESTROY, so a later enable can tell "the
// destruction completed while nothing was waiting" from "the old window still
// runs". Only this exact HWND satisfies the wait: WM_NCDESTROY of any other
// window signals nothing.
std::atomic<HWND> g_execStopTarget{nullptr};
// How long the stop waits for a creator to leave CreateWindowExW. A creator
// that does not leave in this budget makes the teardown outstanding: the channel
// and the handles stay, and the next enable refuses.
constexpr DWORD kExecCreatorDrainMs = 500;
constexpr UINT kExecMessage = WM_APP + 0x5A;
// The private shutdown and latch messages. A generic WM_CLOSE says only "some
// window should close"; the teardown is a request this activation made about one
// identified window, and the WndProc answers it only when that window is the
// recorded target. The latch message moves the hidden event's arm onto the
// executor's owner thread, where the timer and the slot live.
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
// Set by WM_NCDESTROY on the launcher thread when the destroyed window is the
// one this activation recorded. StopExecutorWorker waits on it, so the window and
// its timers are gone before the channel closes. It is never closed: a stop that
// timed out leaves it for the teardown a later enable reaps.
HANDLE g_execWindowGone = nullptr;
// How long the unload waits for the worker to join and for the window to be
// destroyed. A timeout keeps every resource the outstanding participant still
// uses (the worker's handles, the channel), makes the unload defer, and makes
// the next activation refuse until the matching join or destruction completed.
constexpr DWORD kExecJoinWaitMs = 2000;
// The hidden event does not publish the latched open from its own stack. A show
// that ran at the hidden event's own tick was swallowed by the completing hide
// (measured on the auto-hidden layout: `own state event=hidden`, show and
// `exec show` in the same millisecond, `exec visible ... visible=0`, state
// stuck at 3, no Shown); a show 16 ms after the event opened normally. The
// timer fires in the message loop after the event has unwound; 50 ms keeps a
// margin over the measured 16 ms.
constexpr uint64_t kPressServeDelayMs = 50;
constexpr uintptr_t kLauncherSingletonRva = 0x948E80;
constexpr uintptr_t kLauncherStateOffset = 0x160;
constexpr uintptr_t kLauncherIsVisibleRva = 0x1F9B40;
constexpr uint64_t kLauncherStateReadyFloor = 0x10000;

using LauncherIsVisibleFn = HRESULT(WINAPI*)(void*, BOOL*);
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
// The confirmation probe: IsVisible right after Show returns is too early (the
// window is cloaked until the shell's own show animation runs), so the answer is
// read again 300 ms later on the same thread and logged with the command it
// belongs to.
struct Point {
    float x;
    float y;
};

struct GestureAnimationData {
    uint64_t sourceId;
    uint64_t providerId;
};

static_assert(sizeof(GestureAnimationData) == 16,
              "GestureAnimationData layout must match Phase 0");

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

std::atomic<bool> g_explorerHooked{false};
std::atomic<bool> g_afterInit{false};
std::atomic<bool> g_disabled{false};
// Set only after Wh_SetFunctionHook accepted the close-decision hook, so no
// close is ever arbitrated on a build where that hook was refused.
std::atomic<bool> g_closeHookEnabled{false};
// Close-hook frames currently inside, entry to exit: the decision hook's
// HideStartAndCortana and the observation hook's Hide. Wh_ModUninit closes the
// v7 channel only when this is zero, and Wh_ModInit refuses to re-activate this
// image while it is not. It is a lifetime guard for that channel, not a barrier
// between two closes: it answers "is a close still inside", never "this close
// runs after that one".
std::atomic<int> g_closeCalls{0};

// Set when Wh_ModInit refused to install anything, so the later lifecycle
// callbacks leave the live activation's channel and count alone. Wh_ModInit's
// FALSE already tells the host not to call them; this makes a host that calls
// them anyway harmless.
std::atomic<bool> g_initRefused{false};

// Counts one close-hook frame for as long as that frame is inside the hook, the
// original call included. The lease is taken before the hook reads anything and
// released on every exit, including unwinding, so no path can leave the count
// stranded.
//
// Two closes on different threads hold a lease at the same time, and a
// re-entrant close on one thread holds one lease per frame, so it balances by
// itself. The count reaching zero is `drained`, and it is the only condition on
// which Wh_ModUninit closes the channel or returns; because the lease spans the
// original call it also settles the trampoline question for every counted
// frame: a frame that is still about to call the original holds a lease, so the
// unload does not return under it and no timeout may stand in for the count. The
// one window the count cannot see is an entry that has not yet run the
// constructor below; the pinned image keeps that frame's own code and the
// destructor mapped, and its trampoline use is the host's unload to cover.
struct CloseCallLease {
    CloseCallLease() { g_closeCalls.fetch_add(1, std::memory_order_acq_rel); }
    ~CloseCallLease() { g_closeCalls.fetch_sub(1, std::memory_order_acq_rel); }

    CloseCallLease(const CloseCallLease&) = delete;
    CloseCallLease& operator=(const CloseCallLease&) = delete;
};

enum : int {
    kSharedNotStarted = 0,
    kSharedInitializing = 1,
    kSharedReady = 2,
    kSharedFailed = 3,
};

std::atomic<int> g_sharedInitState{kSharedNotStarted};
StartGestureShared* g_shared = nullptr;  // Published before kSharedReady
HANDLE g_sharedMapping = nullptr;
HANDLE g_sharedEvent = nullptr;
std::atomic<bool> g_sharedErrorLogged{false};

// The start point belongs to the gesture identified by gestureStartTick, so a
// later ReportEdgeGesture call for the same gesture never replaces it.
std::atomic<uint64_t> g_startPointTick{0};

// ---------------------------------------------------------------------------
// Shared memory
// ---------------------------------------------------------------------------

void LogSharedErrorOnce(const wchar_t* what, DWORD error) {
    if (!g_sharedErrorLogged.exchange(true)) {
        Wh_Log(L"%s failed (error %u); gesture data may be incomplete", what,
               static_cast<unsigned int>(error));
    }
}

void ResetSharedFields(StartGestureShared* shared) {
    // Unpublish first and publish magic last, so a consumer that mapped the
    // section concurrently never sees a half-initialized struct.
    shared->magic = 0;
    shared->version = 0;
    shared->sequence = 0;
    shared->state = static_cast<uint32_t>(StartGestureState::None);
    shared->edge = -1;
    shared->sourceId = 0;
    shared->providerId = 0;
    // Negative coordinates are not valid screen positions; the consumer can
    // use them to detect "no point reported yet".
    shared->startX = -1.0f;
    shared->startY = -1.0f;
    shared->currentX = -1.0f;
    shared->currentY = -1.0f;
    shared->gestureStartTick = 0;
    shared->lastUpdateTick = 0;
    shared->version = kWindhawkStartGestureVersion;
    shared->magic = kWindhawkStartGestureMagic;
}

StartGestureShared* CreateOrOpenShared() {
    HANDLE mapping = CreateFileMappingW(
        INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
        static_cast<DWORD>(sizeof(StartGestureShared)),
        kWindhawkStartGestureMappingName);
    if (!mapping) {
        LogSharedErrorOnce(L"CreateFileMappingW", GetLastError());
        return nullptr;
    }

    const bool fresh = GetLastError() != ERROR_ALREADY_EXISTS;

    void* view = MapViewOfFile(mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0,
                               sizeof(StartGestureShared));
    if (!view) {
        LogSharedErrorOnce(L"MapViewOfFile", GetLastError());
        CloseHandle(mapping);
        return nullptr;
    }
    g_sharedMapping = mapping;

    auto* shared = static_cast<StartGestureShared*>(view);
    const bool valid = !fresh &&
                       shared->magic == kWindhawkStartGestureMagic &&
                       shared->version == kWindhawkStartGestureVersion;
    // An odd sequence on an otherwise valid section means a previous mod
    // instance was unloaded in the middle of a write; reset it too.
    if (!valid || (shared->sequence & 1u) != 0u) {
        ResetSharedFields(shared);
    }

    g_sharedEvent =
        CreateEventW(nullptr, FALSE, FALSE, kWindhawkStartGestureEventName);
    if (!g_sharedEvent) {
        // Publishing still works; the consumer can poll the sequence.
        LogSharedErrorOnce(L"CreateEventW", GetLastError());
    }

    return shared;
}

StartGestureShared* EnsureShared() {
    const int state = g_sharedInitState.load(std::memory_order_acquire);
    if (state == kSharedReady) {
        return g_shared;
    }
    if (state == kSharedFailed) {
        return nullptr;
    }

    int expected = kSharedNotStarted;
    if (g_sharedInitState.compare_exchange_strong(expected,
                                                  kSharedInitializing,
                                                  std::memory_order_acq_rel)) {
        StartGestureShared* shared = CreateOrOpenShared();
        g_shared = shared;
        g_sharedInitState.store(shared ? kSharedReady : kSharedFailed,
                                std::memory_order_release);
        return shared;
    }

    // Another thread is initializing. Don't wait inside a hook; this write is
    // skipped and the next one will see the ready state.
    return g_sharedInitState.load(std::memory_order_acquire) == kSharedReady
               ? g_shared
               : nullptr;
}

struct SharedUpdate {
    bool written;
    uint32_t previousState;
    uint32_t newState;
    int32_t edge;
    int64_t sourceId;
    int64_t providerId;
};

// Serializes the seqlock writers. The gesture callbacks run on the explorer UI
// thread, but the unload path publishes from the Windhawk thread.
SRWLOCK g_sharedWriteLock = SRWLOCK_INIT;

// Bumps the seqlock around the write, updates the timestamp and signals the
// consumer.
template <typename Writer>
SharedUpdate UpdateShared(Writer&& writer) {
    SharedUpdate result{};

    if (g_disabled.load(std::memory_order_relaxed)) {
        return result;
    }

    StartGestureShared* shared = EnsureShared();
    if (!shared) {
        return result;
    }

    AcquireSRWLockExclusive(&g_sharedWriteLock);
    InterlockedIncrement(
        reinterpret_cast<volatile LONG*>(&shared->sequence));
    result.previousState = shared->state;
    writer(shared);
    shared->lastUpdateTick = GetTickCount64();
    result.newState = shared->state;
    result.edge = shared->edge;
    result.sourceId = shared->sourceId;
    result.providerId = shared->providerId;
    InterlockedIncrement(
        reinterpret_cast<volatile LONG*>(&shared->sequence));

    if (g_sharedEvent) {
        SetEvent(g_sharedEvent);
    }

    ReleaseSRWLockExclusive(&g_sharedWriteLock);

    result.written = true;
    return result;
}

// Used while the mod is being disabled, when normal writes are already
// blocked. Hooks are no longer applied after Wh_ModBeforeUninit returns.
void PublishInactiveOnUnload() {
    if (g_sharedInitState.load(std::memory_order_acquire) != kSharedReady) {
        return;
    }
    StartGestureShared* shared = g_shared;
    if (!shared) {
        return;
    }

    AcquireSRWLockExclusive(&g_sharedWriteLock);
    InterlockedIncrement(
        reinterpret_cast<volatile LONG*>(&shared->sequence));
    shared->state = static_cast<uint32_t>(StartGestureState::None);
    shared->edge = -1;
    shared->sourceId = 0;
    shared->providerId = 0;
    shared->startX = -1.0f;
    shared->startY = -1.0f;
    shared->currentX = -1.0f;
    shared->currentY = -1.0f;
    shared->gestureStartTick = 0;
    shared->lastUpdateTick = GetTickCount64();
    InterlockedIncrement(
        reinterpret_cast<volatile LONG*>(&shared->sequence));
    ReleaseSRWLockExclusive(&g_sharedWriteLock);

    if (g_sharedEvent) {
        SetEvent(g_sharedEvent);
    }
}

// ---------------------------------------------------------------------------
// Logging (state transitions only, never per frame)
// ---------------------------------------------------------------------------

// Appends to <mod storage>\bridge.log. Called once per gesture state
// transition.
void FileLog(const wchar_t* format, ...) {
    static SRWLOCK lock = SRWLOCK_INIT;

    WCHAR storage[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) {
        return;
    }

    WCHAR path[MAX_PATH] = {};
    swprintf_s(path, L"%s\\bridge.log", storage);

    WCHAR message[900] = {};
    va_list args;
    va_start(args, format);
    vswprintf_s(message, ARRAYSIZE(message), format, args);
    va_end(args);

    // Every line starts with the system tick, so lines from the two processes
    // can be ordered against each other and against an injected input.
    WCHAR line[1024] = {};
    swprintf_s(line, ARRAYSIZE(line), L"t=%llu %s",
               static_cast<unsigned long long>(GetTickCount64()), message);

    char utf8[2048] = {};
    int bytes = WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8,
                                    sizeof(utf8), nullptr, nullptr);
    if (bytes <= 1) {
        return;
    }

    AcquireSRWLockExclusive(&lock);
    HANDLE file = CreateFileW(path, FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(file, utf8, static_cast<DWORD>(bytes - 1), &written, nullptr);
        WriteFile(file, "\r\n", 2, &written, nullptr);
        CloseHandle(file);
    }
    ReleaseSRWLockExclusive(&lock);
}

void LogStateChange(const SharedUpdate& update, const wchar_t* reason) {
    if (!update.written || update.previousState == update.newState) {
        return;
    }
    FileLog(L"state=%u edge=%d src=%lld prov=%lld %s",
            static_cast<unsigned int>(update.newState),
            static_cast<int>(update.edge),
            static_cast<long long>(update.sourceId),
            static_cast<long long>(update.providerId), reason);
}

// ---------------------------------------------------------------------------
// Build gate
//
// The hardcoded RVAs are only valid for the build they were taken from. The
// gate checks the file version of the loaded module and, when the file can be
// read, that its PE timestamp is the one in memory. A mismatch installs
// nothing.
// ---------------------------------------------------------------------------

using GetFileVersionInfoSizeW_t = decltype(&GetFileVersionInfoSizeW);
using GetFileVersionInfoW_t = decltype(&GetFileVersionInfoW);
using VerQueryValueW_t = decltype(&VerQueryValueW);

struct VersionApi {
    HMODULE module = nullptr;
    bool ownsReference = false;
    GetFileVersionInfoSizeW_t size = nullptr;
    GetFileVersionInfoW_t info = nullptr;
    VerQueryValueW_t query = nullptr;
};

void UnloadVersionApi(VersionApi& api) {
    if (api.ownsReference && api.module) {
        FreeLibrary(api.module);
    }
    api = {};
}

bool LoadVersionApi(VersionApi* api) {
    HMODULE module = GetModuleHandleW(L"version.dll");
    if (!module) {
        module = LoadLibraryW(L"version.dll");
        if (!module) {
            return false;
        }
        api->ownsReference = true;
    }
    api->module = module;

    api->size = reinterpret_cast<GetFileVersionInfoSizeW_t>(
        GetProcAddress(module, "GetFileVersionInfoSizeW"));
    api->info = reinterpret_cast<GetFileVersionInfoW_t>(
        GetProcAddress(module, "GetFileVersionInfoW"));
    api->query = reinterpret_cast<VerQueryValueW_t>(
        GetProcAddress(module, "VerQueryValueW"));

    if (!api->size || !api->info || !api->query) {
        UnloadVersionApi(*api);
        return false;
    }
    return true;
}

// Reads FileVersion from the first StringFileInfo translation. The fixed
// version resource can lag the string on incremental Windows updates, so the
// string is what the gate compares.
bool QueryFileVersionString(const VersionApi& api, const wchar_t* path,
                            wchar_t* out, size_t outCount) {
    DWORD handle = 0;
    const DWORD size = api.size(path, &handle);
    if (size == 0 || size > 8192) {
        return false;
    }

    BYTE buffer[8192];
    if (!api.info(path, 0, size, buffer)) {
        return false;
    }

    struct Translation {
        WORD language;
        WORD codePage;
    };

    Translation* translations = nullptr;
    UINT translationBytes = 0;
    if (!api.query(buffer, L"\\VarFileInfo\\Translation",
                   reinterpret_cast<void**>(&translations),
                   &translationBytes) ||
        !translations) {
        return false;
    }

    const size_t translationCount = translationBytes / sizeof(Translation);
    for (size_t i = 0; i < translationCount; i++) {
        wchar_t key[64] = {};
        swprintf_s(key, ARRAYSIZE(key),
                   L"\\StringFileInfo\\%04x%04x\\FileVersion",
                   translations[i].language, translations[i].codePage);

        wchar_t* value = nullptr;
        UINT valueChars = 0;
        if (!api.query(buffer, key, reinterpret_cast<void**>(&value),
                       &valueChars) ||
            !value || valueChars == 0) {
            continue;
        }

        size_t length = 0;
        while (length + 1 < outCount && value[length] != L'\0') {
            out[length] = value[length];
            length++;
        }
        out[length] = L'\0';
        return length != 0;
    }

    return false;
}

// Parses the leading "a.b.c.d" of a file version string; a suffix such as
// " (WinBuild...)" is ignored.
bool ParseVersion(const wchar_t* text, unsigned* parts) {
    const wchar_t* cursor = text;
    for (int i = 0; i < 4; i++) {
        while (*cursor == L' ' || *cursor == L'\t') {
            cursor++;
        }
        if (*cursor < L'0' || *cursor > L'9') {
            return false;
        }

        unsigned value = 0;
        while (*cursor >= L'0' && *cursor <= L'9') {
            value = value * 10 + static_cast<unsigned>(*cursor - L'0');
            if (value > 0xFFFF) {
                return false;
            }
            cursor++;
        }
        parts[i] = value;

        if (i < 3) {
            if (*cursor != L'.') {
                return false;
            }
            cursor++;
        }
    }
    return true;
}

bool ReadLoadedPeTimestamp(HMODULE module, DWORD* timestamp) {
    const auto* base = reinterpret_cast<const BYTE*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0 ||
        dos->e_lfanew > 0x1000) {
        return false;
    }

    const auto* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }
    *timestamp = nt->FileHeader.TimeDateStamp;
    return true;
}

bool ReadFilePeTimestamp(const wchar_t* path, DWORD* timestamp) {
    HANDLE file = CreateFileW(path, GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    bool ok = false;
    IMAGE_DOS_HEADER dos{};
    DWORD read = 0;
    if (ReadFile(file, &dos, sizeof(dos), &read, nullptr) &&
        read == static_cast<DWORD>(sizeof(dos)) &&
        dos.e_magic == IMAGE_DOS_SIGNATURE && dos.e_lfanew > 0 &&
        dos.e_lfanew <= 0x1000) {
        LARGE_INTEGER offset{};
        offset.QuadPart = dos.e_lfanew;
        if (SetFilePointerEx(file, offset, nullptr, FILE_BEGIN)) {
            IMAGE_NT_HEADERS nt{};
            if (ReadFile(file, &nt, sizeof(nt), &read, nullptr) &&
                read == static_cast<DWORD>(sizeof(nt)) &&
                nt.Signature == IMAGE_NT_SIGNATURE) {
                *timestamp = nt.FileHeader.TimeDateStamp;
                ok = true;
            }
        }
    }

    CloseHandle(file);
    return ok;
}

bool TwinuiBuildMatches(HMODULE module) {
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        Wh_Log(L"twinui build mismatch unknown; refusing to install hooks");
        return false;
    }

    wchar_t rawVersion[64] = {};
    wchar_t displayVersion[64] = L"unknown";
    unsigned parts[4] = {};
    bool versionMatches = false;

    VersionApi api;
    if (LoadVersionApi(&api)) {
        if (QueryFileVersionString(api, path, rawVersion,
                                   ARRAYSIZE(rawVersion)) &&
            ParseVersion(rawVersion, parts)) {
            swprintf_s(displayVersion, ARRAYSIZE(displayVersion),
                       L"%u.%u.%u.%u", parts[0], parts[1], parts[2],
                       parts[3]);
            versionMatches = true;
            for (int i = 0; i < 4; i++) {
                if (parts[i] != kTwinuiVersion[i]) {
                    versionMatches = false;
                    break;
                }
            }
        }
        UnloadVersionApi(api);
    }

    if (!versionMatches) {
        Wh_Log(L"twinui build mismatch %s; refusing to install hooks",
               displayVersion);
        return false;
    }

    // A version match is enough to proceed; the timestamp check is an extra
    // guard against a file that was replaced while the module is loaded.
    DWORD loadedTimestamp = 0;
    DWORD fileTimestamp = 0;
    if (ReadLoadedPeTimestamp(module, &loadedTimestamp) &&
        ReadFilePeTimestamp(path, &fileTimestamp) &&
        loadedTimestamp != fileTimestamp) {
        Wh_Log(L"twinui build mismatch %s (timestamp 0x%08X != 0x%08X); "
               L"refusing to install hooks",
               displayVersion, loadedTimestamp, fileTimestamp);
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// twinui.pcshell.dll hooks
// ---------------------------------------------------------------------------

using InputGestureOperationStarted_t = void (*)(void*, const void*, const void*);
InputGestureOperationStarted_t InputGestureOperationStarted_Original = nullptr;

void InputGestureOperationStarted_Hook(void* self, const void* operation,
                                       const void* args) {
    const uint64_t now = GetTickCount64();
    const SharedUpdate update = UpdateShared([now](StartGestureShared* shared) {
        // The complete start record: no field of the previous gesture stays
        // visible next to the new state and timestamp.
        shared->state = static_cast<uint32_t>(StartGestureState::Active);
        shared->edge = -1;
        shared->sourceId = 0;
        shared->providerId = 0;
        shared->startX = -1.0f;
        shared->startY = -1.0f;
        shared->currentX = -1.0f;
        shared->currentY = -1.0f;
        shared->gestureStartTick = now;
    });
    LogStateChange(update, L"InputGestureOperationStarted");

    if (InputGestureOperationStarted_Original) {
        InputGestureOperationStarted_Original(self, operation, args);
    }
}

using InputGestureOperationEnded_t = void (*)(void*, const void*, const void*);
InputGestureOperationEnded_t InputGestureOperationEnded_Original = nullptr;

void InputGestureOperationEnded_Hook(void* self, const void* operation,
                                     const void* args) {
    // Committed means only "the gesture finished while its handle was live",
    // which is the open signal the Start side consumes. It is not a claim
    // about what the shell did with the window.
    const SharedUpdate update = UpdateShared([](StartGestureShared* shared) {
        if (shared->state ==
                static_cast<uint32_t>(StartGestureState::Active) &&
            (shared->sourceId != 0 || shared->providerId != 0)) {
            shared->state =
                static_cast<uint32_t>(StartGestureState::Committed);
        }
    });
    LogStateChange(update,
                   L"InputGestureOperationEnded (ended with a live handle)");

    // The handle is deliberately kept: the subsequent Hide clears it.
    if (InputGestureOperationEnded_Original) {
        InputGestureOperationEnded_Original(self, operation, args);
    }
}

using ReportEdgeGesture_t = void (*)(void*, int, const GestureAnimationData*,
                                     Point);
ReportEdgeGesture_t ReportEdgeGesture_Original = nullptr;

void ReportEdgeGesture_Hook(void* self, int edge,
                            const GestureAnimationData* data, Point point) {
    // Called once per gesture right after InputGestureOperationStarted. This
    // is the first call that carries the edge, the handle and a real point, so
    // it republishes the whole start record in one seqlock write.
    const SharedUpdate update =
        UpdateShared([edge, data, point](StartGestureShared* shared) {
            const uint64_t now = GetTickCount64();
            shared->state = static_cast<uint32_t>(StartGestureState::Active);
            shared->edge = edge;

            uint64_t startTick = shared->gestureStartTick;
            if (startTick == 0) {
                startTick = now;
            }
            // Keep the first point of this gesture if the shell reports the
            // edge more than once.
            if (g_startPointTick.load(std::memory_order_relaxed) !=
                startTick) {
                shared->startX = point.x;
                shared->startY = point.y;
                g_startPointTick.store(startTick, std::memory_order_relaxed);
            }
            shared->currentX = point.x;
            shared->currentY = point.y;
            shared->gestureStartTick = startTick;

            if (data) {
                shared->sourceId = static_cast<int64_t>(data->sourceId);
                shared->providerId = static_cast<int64_t>(data->providerId);
            }
        });
    LogStateChange(update, L"ReportEdgeGesture");

    if (ReportEdgeGesture_Original) {
        ReportEdgeGesture_Original(self, edge, data, point);
    }
}

using SetAnimationData_t = HRESULT (*)(void*, uint64_t, uint64_t, uint64_t);
SetAnimationData_t SetAnimationData_Original = nullptr;

HRESULT SetAnimationData_Hook(void* self, uint64_t a, uint64_t b, uint64_t c) {
    // Phase 0: a=0, b=SourceId, c=ProviderId at gesture start.
    const uint64_t now = GetTickCount64();
    const SharedUpdate update =
        UpdateShared([b, c, now](StartGestureShared* shared) {
            if (shared->state !=
                static_cast<uint32_t>(StartGestureState::Active)) {
                // A new gesture outside the Started path starts with a clean
                // record rather than the previous gesture's edge/point.
                shared->gestureStartTick = now;
                shared->edge = -1;
                shared->startX = -1.0f;
                shared->startY = -1.0f;
                shared->currentX = -1.0f;
                shared->currentY = -1.0f;
            }
            shared->sourceId = static_cast<int64_t>(b);
            shared->providerId = static_cast<int64_t>(c);
            shared->state = static_cast<uint32_t>(StartGestureState::Active);
        });
    LogStateChange(update, L"SetAnimationData");

    return SetAnimationData_Original ? SetAnimationData_Original(self, a, b, c)
                                     : S_OK;
}

using Show_t = HRESULT (*)(void*, uint8_t, uint8_t, uint32_t);
Show_t Show_Original = nullptr;

HRESULT Show_Hook(void* self, uint8_t a, uint8_t b, uint32_t c) {
    FileLog(L"show a=%u b=%u c=%u tid=%u", static_cast<unsigned int>(a),
            static_cast<unsigned int>(b), static_cast<unsigned int>(c),
            GetCurrentThreadId());
    // The manager's Show is the shell's own open on the launcher thread. It is
    // the touch-first path's first launcher entry: creating the window here keeps
    // a band open working when no Win/Start-button hook ran yet. The gate adopts
    // the thread only when no entry has been seen, and never creates a second
    // window on a thread that is not that one.
    EnsureExecutorOnLauncherThread(L"show");

    // Only a Show that follows a real gesture handle publication is a
    // gesture-driven open. A normal mouse/keyboard open must not look like a
    // gesture to the Start side.
    StartGestureShared* sharedState = EnsureShared();
    if (sharedState &&
        (sharedState->sourceId != 0 || sharedState->providerId != 0)) {
        const SharedUpdate update = UpdateShared([](StartGestureShared* shared) {
            if (shared->sourceId != 0 || shared->providerId != 0) {
                shared->state =
                    static_cast<uint32_t>(StartGestureState::Active);
            }
        });
        LogStateChange(update, L"Show (live handle)");
    }

    return Show_Original ? Show_Original(self, a, b, c) : S_OK;
}

using Hide_t = HRESULT (*)(void*, uint32_t);
Hide_t Hide_Original = nullptr;

// Clears the published gesture record when the shell hides the Start window.
void ClearSharedGestureState(const wchar_t* reason) {
    const SharedUpdate update = UpdateShared([](StartGestureShared* shared) {
        shared->state = static_cast<uint32_t>(StartGestureState::None);
        // Drop the handle together with the Start window, so a later
        // non-gesture Show cannot expose an expired handle.
        shared->sourceId = 0;
        shared->providerId = 0;
        shared->gestureStartTick = 0;
        shared->edge = -1;
        shared->startX = -1.0f;
        shared->startY = -1.0f;
        shared->currentX = -1.0f;
        shared->currentY = -1.0f;
    });
    LogStateChange(update, reason);
    g_startPointTick.store(0, std::memory_order_relaxed);
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

enum class OwnCaller : int {
    Ordinary = 0,  // DismissStartAndCortana: Win, Start button, our Dismiss
    ClickAway,     // the delegate sub_4916D0
    Lock,          // DismissSynchronouslyWithoutTransition (lock, display)
    Unknown,       // never held: an unrecognised close passes at once
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

using DismissFn = HRESULT (*)(void*, uint32_t);
using ShowStartViewFn = HRESULT (*)(void*, uint32_t, uint32_t);

DismissFn g_dismiss = nullptr;
ShowStartViewFn g_showStartView = nullptr;
uintptr_t g_twinuiBase = 0;

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
// The caller-site probe's result. Without it no close can be attributed to the
// lock screen, a click-away or an ordinary close, so the hooks that turn a call
// into a decision are refused (InstallExplorerHooks) and every unverified call
// passes to the shell.
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

OwnCaller OwnResolveCaller(void* returnAddress) {
    if (!g_twinuiBase || !g_callersVerified.load(std::memory_order_relaxed)) {
        // Nothing proved these call sites on this build. An unverified return
        // address is not a lock, a click-away or an ordinary close; the caller
        // passes to the shell instead of being animated as one of them.
        return OwnCaller::Unknown;
    }
    const uintptr_t rva =
        reinterpret_cast<uintptr_t>(returnAddress) - g_twinuiBase;
    if (rva == kHideCallerOrdinaryVa) {
        return OwnCaller::Ordinary;
    }
    if (rva == kHideCallerClickAwayVa) {
        return OwnCaller::ClickAway;
    }
    if (rva == kHideCallerAwayVa) {
        return OwnCaller::ClickAway;
    }
    if (rva == kHideCallerLockVa) {
        return OwnCaller::Lock;
    }
    return OwnCaller::Unknown;
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
        FileLog(L"own state resync cached=%d shell=%d", shellOpen ? 0 : 1,
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
        FileLog(L"own state resync skipped reason=state-not-ready raw=0x%llX",
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
        FileLog(L"own state resync cached=%d shell=%d", shellOpen ? 0 : 1,
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
        FileLog(L"own state event=shown open=1 tid=%u", GetCurrentThreadId());
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
        FileLog(L"own state event=hidden open=0 tid=%u", GetCurrentThreadId());
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
           g_execAdmissionOpen.load(std::memory_order_acquire) &&
           g_execReady.load(std::memory_order_acquire) &&
           !g_disabled.load(std::memory_order_relaxed) &&
           !g_initRefused.load(std::memory_order_relaxed);
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
        FileLog(L"own pass trigger=%s reason=no-peer", trigger);
        return false;
    }
    const uint32_t epoch = PeerEpochNow();
    if (!BridgeCanServe()) {
        FileLog(L"own pass trigger=%s reason=no-executor", trigger);
        return false;
    }
    if (NoteLauncherThread(trigger) == 0) {
        // The thread test comes before the window test, so a hook on another
        // thread cannot reach the launcher-local fields through an existing
        // window.
        FileLog(L"own pass trigger=%s reason=wrong-thread", trigger);
        return false;
    }
    EnsureExecutorWindow();
    if (!ExecutorWindowOnThisThread()) {
        FileLog(L"own pass trigger=%s reason=no-window", trigger);
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
        // The return address is the only thing that tells a lock screen from the
        // click-away; without the proven call sites every close is
        // unattributable and must not be animated.
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
    const CloseCallLease closeCall;
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
        if (method == kDismissMethodClickAway &&
            (execState == 1 || execState == 3)) {
            // Our own Show is still on the stack and the shell is closing the
            // window it is just presenting because the pointer is outside it.
            // That is the pointer-away check, not a user click; swallowing it is
            // what lets the requested open land.
            FileLog(L"exec swallow hide method=7 state=%u reason=own-show",
                    execState);
            return;
        }
        FileLog(L"exec pass hide method=%u state=%u reason=executor", method,
                execState);
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
        FileLog(L"exec pass hide method=%u reason=%s", method,
                reason ? reason : L"unknown");
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    const OwnCaller caller = OwnResolveCaller(OWN_RETURN_ADDRESS());
    if (caller == OwnCaller::Lock || caller == OwnCaller::Unknown ||
        method == kDismissMethodWinTab || OwnTakeExternalForward(method)) {
        FileLog(L"intent external trigger=hide caller=%s method=%u", 
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
    if (method == kDismissMethodClickAway &&
        (currentState == 1 || currentState == 3)) {
        FileLog(L"intent ignore trigger=clickaway method=%u state=%u "
                L"reason=opening",
                method, currentState);
        return;
    }
    if (method == kDismissMethodClickAway &&
        SwipeIpcControllerTouch(&g_ipc)) {
        FileLog(L"intent ignore trigger=clickaway method=%u reason=touch-owner",
                method);
        return;
    }

    const int32_t trigger = caller == OwnCaller::ClickAway
                                ? kSwipeTriggerClickAway
                                : kSwipeTriggerWin;
    PublishBridgeIntent(kSwipeIntentClose, trigger, method, 0);
    FileLog(L"intent close trigger=%s method=%u state=%u",
            trigger == kSwipeTriggerClickAway ? L"clickaway" : L"win", method,
            OwnInternalState(self));
}
// CStartExperienceManager::Hide(void* self, uint32_t reason) -> HRESULT
// twinui.pcshell.dll 26100.9949, RVA 0x3C5960, vtable slot after Show.
//
// The prologue stores rcx (this) and edx (reason), zeroes dil and compares
// `edx` with 7; r8 and r9 are written before they are read, so the function
// takes two arguments. Both return paths set eax: 0 on success, the negative
// HRESULT of a failing inner call otherwise, so the return type is HRESULT.
//
// Observation only, and every path calls the original. The pause belongs to the
// decision hook above: a hold here would delay the same close a second time, and
// a return without the original would strand the launcher's state at 9.
HRESULT Hide_Hook(void* self, uint32_t reason) {
    // The lease covers the whole call, so the unload's count includes this frame
    // and its trampoline use for as long as it can run.
    const CloseCallLease closeCall;
    FileLog(L"hide reason=%u", reason);
    ClearSharedGestureState(L"Hide");
    return Hide_Original ? Hide_Original(self, reason) : S_OK;
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
    FileLog(L"own press latch dropped reason=%s %s wait=%llu", reason, detail,
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
        FileLog(L"own press latch dropped reason=no-window state=%u",
                own.state);
        return;
    }
    if (g_pressLatch.pending) {
        FileLog(L"own press latch replaced old_tick=%llu old_wait=%llu state=%u",
                static_cast<unsigned long long>(g_pressLatch.tick),
                static_cast<unsigned long long>(now - g_pressLatch.tick),
                own.state);
    }
    g_pressLatch = PressLatch{true,            now,  kSwipeIntentOpen,
                              trigger,         method, flags,
                              admittingEpoch};
    FileLog(L"own press latched state=%u trigger=%s tick=%llu epoch=%u "
            L"(hide in flight)",
            own.state, PressTriggerName(trigger),
            static_cast<unsigned long long>(now), g_pressLatch.epoch);
    g_pressServeReason = L"timeout";
    if (SetTimer(window, kPressLatchTimer, kPressLatchTimeoutMs, nullptr) == 0) {
        // The hidden event is a second serve point, but it is not a guaranteed
        // one: the landing may already have passed. Today the initial timer is
        // the only promise, so a latch that cannot arm it is dropped.
        FileLog(L"own press latch timer failed err=%u", GetLastError());
        DropPressLatch(L"timer-failed", L"latch");
    }
}

// The latch waits for the hide's own event; the event then arms this deferral
// instead of publishing from its stack. See kPressServeDelayMs.
//
// The slot and its timer are launcher-thread state. The hidden event is expected
// there, but the check is not an assumption: a callback on another thread posts
// one private notification to the executor window and the owner arms it.
void ArmLatchedServe() {
    if (!g_pressLatch.pending) {
        return;
    }
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        DropPressLatch(L"no-window", L"hidden");
        return;
    }
    if (!ExecutorWindowOnThisThread()) {
        if (!PostMessageW(window, kExecLatchMessage, kExecLatchMagic, 0)) {
            FileLog(L"own press latch arm post failed err=%u", GetLastError());
        } else {
            FileLog(L"own press latch arm posted tid=%u owner=%u",
                    GetCurrentThreadId(), g_execThreadId);
        }
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
        FileLog(L"own press serve refused reason=%s launcher=0 state=0x%llX",
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
    FileLog(L"own press served wait=%llu state=%u trigger=%s reason=%s "
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
void DispatchToggle(const OwnSnapshot& own, int32_t trigger, uint32_t showMethod,
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
    // One decision line for every trigger. The method below is the one the action
    // will use, not the one the shell called with: a reopen from Dismiss(6) asks
    // for Show(11), and a close from Show(11) asks for Dismiss(6).
    FileLog(L"own toggle decide trigger=%s state=%u open=%d kind=%s dir=%d",
            PressTriggerName(trigger), own.state, own.open ? 1 : 0,
            kind == kSwipeIntentClose ? L"close" : L"open", direction);
    if (kind == kSwipeIntentOpen &&
        (ShellHideInFlight(own) || g_pendingDismissSeq != 0)) {
        // The hide or the armed dismiss has already taken the window; the open
        // is served at the landing instead of being published into a no-op show.
        LatchPress(own, trigger, showMethod, flags, admittingEpoch);
        return;
    }
    PublishBridgeIntent(kind, trigger,
                        kind == kSwipeIntentClose ? dismissMethod : showMethod,
                        flags);
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
    const CloseCallLease closeCall;
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
    FileLog(L"own win enter flags=%u shift=%d ctrl=%d alt=%d tid=%u",
            flags, shift ? 1 : 0, control ? 1 : 0, alt ? 1 : 0,
            GetCurrentThreadId());

    if (shift || control || alt) {
        const OwnSnapshot own = OwnSync(launcher);
        if (!own.ready) {
            FileLog(L"own win pass reason=state-not-ready flags=%u", flags);
            return OnHotkeyInvoke_Original
                       ? OnHotkeyInvoke_Original(self, flags)
                       : S_OK;
        }
        if (own.open) {
            PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                                kDismissMethodWin, flags);
            FileLog(L"own win pass reason=external open=1 state=%u", own.state);
            OwnExternalForwardGuard guard(kDismissMethodWin);
            return OnHotkeyInvoke_Original
                       ? OnHotkeyInvoke_Original(self, flags)
                       : S_OK;
        }
        FileLog(L"own win pass reason=external open=0 state=%u (no open)",
                own.state);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                            kDismissMethodWin, flags);
        return S_OK;
    }

    if (static_cast<const uint8_t*>(self)[kHotkeyShellOpenFlagOffset] != 0) {
        // The shell's own tray reopen: not the Start toggle, and it has side
        // effects this mod does not reproduce.
        FileLog(L"own win pass reason=shell-open-flag");
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        FileLog(L"own win pass reason=state-not-ready flags=%u", flags);
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }
    DispatchToggle(own, kSwipeTriggerWin, kDismissMethodWin, kDismissMethodWin,
                   flags, admittingEpoch);
    return S_OK;
}
using OnStartButtonPressed_t = HRESULT (*)(void*, uint32_t, uint32_t);
OnStartButtonPressed_t OnStartButtonPressed_Original = nullptr;

// XamlLauncher::OnStartButtonPressed(self, showMethod, dismissMethod): `self` is
// the launcher and the function is a pure toggle. Nothing else reaches it, so
// there is no combination to gate.
HRESULT OnStartButtonPressed_Hook(void* self, uint32_t showMethod,
                                  uint32_t dismissMethod) {
    const CloseCallLease closeCall;
    uint32_t admittingEpoch = 0;
    if (!g_ownButtonEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) ||
        !BridgeCanServeOnLauncherThread(L"startbutton", &admittingEpoch)) {
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }

    FileLog(L"own button enter show=%u dismiss=%u", showMethod, dismissMethod);
    const OwnSnapshot own = OwnSync(self);
    if (!own.ready) {
        FileLog(L"own button pass reason=state-not-ready show=%u dismiss=%u",
                showMethod, dismissMethod);
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }
    DispatchToggle(own, kSwipeTriggerStartButton, showMethod, dismissMethod, 0,
                   admittingEpoch);
    return S_OK;
}

using OnStartShown_t = void (*)(void*);
OnStartShown_t OnStartShown_Original = nullptr;

// XamlLauncherState::OnStartShown(self): `self` is the launcher state object;
// its IXamlLauncherState interface is `self + 0x18` and its InternalState is
// `[self + 0x40]`, which is the same dword the interface reads at +0x28.
void OnStartShown_Hook(void* self) {
    const CloseCallLease closeCall;
    if (OnStartShown_Original) {
        OnStartShown_Original(self);
    }
    if (!g_ownEventsEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    OwnEventShown();
}

using OnStartHidden_t = void (*)(void*);
OnStartHidden_t OnStartHidden_Original = nullptr;

// XamlLauncherState::OnStartHidden(self): runs after the original, so the state
// is already the settled closed value and the latch's ShowStartView cannot be
// overwritten by this transition again.
void OnStartHidden_Hook(void* self) {
    const CloseCallLease closeCall;
    if (OnStartHidden_Original) {
        OnStartHidden_Original(self);
    }
    if (!g_ownEventsEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    OwnEventHidden();
    // The landing the latch exists for. The serve is deferred off this event's
    // stack: a show requested at the hidden event's own tick was swallowed
    // (kPressServeDelayMs). If the cloak has already flipped, the press becomes
    // the same open intent a normal press would have published.
    ArmLatchedServe();
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
    const CloseCallLease closeCall;
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
        FileLog(L"exec pass dismiss method=%u reason=state-not-ready", method);
        return Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    }
    // The Start button's close is a trigger, not a duplicate: it arrives through
    // this primitive with method 6 and must reach the controller even while the
    // controller owns the panel. Its reopen form asks for Show(11) instead.
    if (method == kDismissMethodStartButton) {
        DispatchToggle(own, kSwipeTriggerStartButton, 11, method, 0,
                       admittingEpoch);
        return S_OK;
    }
    if (SwipeIpcControllerDirection(&g_ipc) != kSwipeDirectionUnowned) {
        FileLog(L"exec swallow dismiss method=%u state=%u reason=owning",
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
    const CloseCallLease closeCall;
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
        FileLog(L"exec pass show method=%u flags=%u reason=state-not-ready",
                method, flags);
        return ShowStartView_Original
                   ? ShowStartView_Original(launcher, method, flags)
                   : E_FAIL;
    }
    // The Start button's open is a trigger too (method 11); it must reach the
    // controller while the controller owns a close, so it is decided here before
    // the duplicate suppression. Its close form asks for Dismiss(6).
    if (method == 11) {
        DispatchToggle(own, kSwipeTriggerStartButton, method, 6, flags,
                       admittingEpoch);
        return S_OK;
    }
    if (SwipeIpcControllerDirection(&g_ipc) != kSwipeDirectionUnowned) {
        FileLog(L"exec swallow show method=%u flags=%u state=%u reason=owning",
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
// Hook installation
// ---------------------------------------------------------------------------

// True when [rva, rva + bytes) lies inside an executable section of the loaded
// image and inside its SizeOfImage. Both probe reads use fixed RVAs, so this is
// what separates "this build is not the supported one" from "the address is not
// code at all": without it a shorter or differently laid-out image faults while
// the gate is still deciding.
//
// Every read is bounded by a value already read from the mapped header page: the
// DOS header, the NT signature, the optional header and the section table all
// live in the first page of every image the loader maps. An image whose header
// is not there is refused like any other unreadable target.
bool ImageRangeExecutable(HMODULE module, uintptr_t rva, size_t bytes) {
    if (!module || bytes == 0) {
        return false;
    }

    constexpr size_t kHeaderPage = 0x1000;
    const auto* base = reinterpret_cast<const uint8_t*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }

    const size_t ntOffset = static_cast<size_t>(dos->e_lfanew);
    if (ntOffset < sizeof(IMAGE_DOS_HEADER) ||
        ntOffset + sizeof(IMAGE_NT_HEADERS64) > kHeaderPage) {
        return false;
    }

    const auto* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + ntOffset);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt->FileHeader.SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER64) ||
        nt->FileHeader.NumberOfSections == 0) {
        return false;
    }

    const size_t sizeOfImage = nt->OptionalHeader.SizeOfImage;
    if (sizeOfImage == 0 || rva > sizeOfImage || bytes > sizeOfImage - rva) {
        return false;
    }

    const size_t sectionTable = ntOffset + sizeof(DWORD) +
                                sizeof(IMAGE_FILE_HEADER) +
                                nt->FileHeader.SizeOfOptionalHeader;
    const size_t sectionBytes = static_cast<size_t>(
                                    nt->FileHeader.NumberOfSections) *
                                sizeof(IMAGE_SECTION_HEADER);
    if (sectionTable > kHeaderPage || sectionBytes > kHeaderPage ||
        sectionTable + sectionBytes > kHeaderPage) {
        return false;
    }

    const auto* sections = reinterpret_cast<const IMAGE_SECTION_HEADER*>(
        base + sectionTable);
    for (size_t i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const IMAGE_SECTION_HEADER& section = sections[i];
        const uint64_t start = section.VirtualAddress;
        const uint64_t span = std::max<uint64_t>(section.Misc.VirtualSize,
                                                 section.SizeOfRawData);
        if (span == 0 || rva < start || rva - start >= span) {
            continue;
        }
        if (bytes > span - (rva - start)) {
            continue;
        }
        if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0) {
            return true;
        }
    }
    return false;
}

// Confirms that the target is the function the decision was read from: the
// address is executable code of the loaded image, its first bytes match the
// build's prologue, and its body still carries the manager call site the state
// machine was traced through. Each failure has its own line, and only the close
// hook is left out; the observation hooks keep working.

// ---------------------------------------------------------------------------
// Own-hook probes
//
// A fixed RVA is only trusted after four checks: the range is executable code of
// the loaded image, the prologue matches this build, the body still carries the
// landmark the decision was read from, and every direct call site the executor
// depends on still targets the function it names. A failure logs one line with
// the hook's name and leaves only that hook out.
// ---------------------------------------------------------------------------

bool PrologueMatches(HMODULE module, uintptr_t rva, const uint8_t* expected,
                     size_t count) {
    if (!ImageRangeExecutable(module, rva, count)) {
        return false;
    }
    const uint8_t* code = reinterpret_cast<const uint8_t*>(module) + rva;
    return std::memcmp(code, expected, count) == 0;
}

bool BodyContains(HMODULE module, uintptr_t rva, size_t span,
                  const uint8_t* needle, size_t count) {
    if (count == 0 || span < count || !ImageRangeExecutable(module, rva, span)) {
        return false;
    }
    const uint8_t* code = reinterpret_cast<const uint8_t*>(module) + rva;
    for (size_t i = 0; i + count <= span; i++) {
        if (std::memcmp(code + i, needle, count) == 0) {
            return true;
        }
    }
    return false;
}

bool DirectCallTargets(HMODULE module, uintptr_t callRva, uintptr_t targetRva) {
    if (!ImageRangeExecutable(module, callRva, 5)) {
        return false;
    }
    const uint8_t* code = reinterpret_cast<const uint8_t*>(module) + callRva;
    if (code[0] != 0xE8) {
        return false;
    }
    int32_t displacement = 0;
    std::memcpy(&displacement, code + 1, sizeof(displacement));
    return callRva + 5 + static_cast<int64_t>(displacement) == targetRva;
}

// XamlLauncher::OnHotkeyInvoke: `[self+0x178]` is the state, `vtable+0x68` is
// IsStartVisible, `Dismiss(self+0x18, 1)` is the close and
// `ShowStartView(self+0x18, 1, flags)` the open.
bool OnHotkeyInvokeProbe(HMODULE module) {
    const uintptr_t rva = kOnHotkeyInvokeVa - kTwinuiPreferredBase;
    const uint8_t stateLoad[] = {0x48, 0x8B, 0x89, 0x78, 0x01, 0x00, 0x00};
    const uint8_t isVisible[] = {0x48, 0x8B, 0x40, 0x68};
    const uint8_t launcherBias[] = {0x48, 0x8D, 0x4B, 0x18};
    if (!PrologueMatches(module, rva, kOnHotkeyInvokePrologue,
                         sizeof(kOnHotkeyInvokePrologue)) ||
        !BodyContains(module, rva, 0x400, stateLoad, sizeof(stateLoad)) ||
        !BodyContains(module, rva, 0x400, isVisible, sizeof(isVisible)) ||
        !BodyContains(module, rva, 0x400, launcherBias,
                      sizeof(launcherBias)) ||
        !DirectCallTargets(module, 0x1419A2, 0x25D670) ||
        !DirectCallTargets(module, 0x141AC9, 0x141AF0)) {
        FileLog(L"own hook skipped name=win build=10.0.26100.9949 body moved");
        Wh_Log(L"OnHotkeyInvoke does not carry the state, launcher bias and "
               L"Dismiss/ShowStartView calls this hook was written for; the "
               L"Windows-key hook is refused");
        return false;
    }
    return true;
}

// XamlLauncher::OnStartButtonPressed: `[self+0x160]` is the state, the visible
// branch calls the state's DismissStartAndCortana (vtable+0x40) and the else
// branch calls ShowStartView.
bool OnStartButtonPressedProbe(HMODULE module) {
    const uintptr_t rva = kOnStartButtonPressedVa - kTwinuiPreferredBase;
    const uint8_t stateLoad[] = {0x48, 0x8B, 0x89, 0x60, 0x01, 0x00, 0x00};
    const uint8_t isVisible[] = {0x48, 0x8B, 0x40, 0x68};
    const uint8_t dismissSlot[] = {0x48, 0x8B, 0x40, 0x40};
    if (!PrologueMatches(module, rva, kOnStartButtonPressedPrologue,
                         sizeof(kOnStartButtonPressedPrologue)) ||
        !BodyContains(module, rva, 0x100, stateLoad, sizeof(stateLoad)) ||
        !BodyContains(module, rva, 0x100, isVisible, sizeof(isVisible)) ||
        !BodyContains(module, rva, 0x100, dismissSlot, sizeof(dismissSlot)) ||
        !DirectCallTargets(module, 0x492CFA, 0x141AF0)) {
        FileLog(L"own hook skipped name=startbutton build=10.0.26100.9949 "
                L"body moved");
        Wh_Log(L"OnStartButtonPressed does not carry the state, the dismiss "
               L"slot and the ShowStartView call this hook was written for; "
               L"the Start-button hook is refused");
        return false;
    }
    return true;
}

bool DismissProbe(HMODULE module) {
    const uintptr_t rva = kDismissVa - kTwinuiPreferredBase;
    const uint8_t stateLoad[] = {0x48, 0x8B, 0x89, 0x60, 0x01, 0x00, 0x00};
    const uint8_t dismissSlot[] = {0x48, 0x8B, 0x40, 0x40};
    if (!PrologueMatches(module, rva, kDismissPrologue,
                         sizeof(kDismissPrologue)) ||
        !BodyContains(module, rva, 0x40, stateLoad, sizeof(stateLoad)) ||
        !BodyContains(module, rva, 0x40, dismissSlot,
                      sizeof(dismissSlot))) {
        FileLog(L"own hook skipped name=dismiss build=10.0.26100.9949 body "
                L"moved");
        return false;
    }
    return true;
}

bool ShowStartViewProbe(HMODULE module) {
    const uintptr_t rva = kShowStartViewVa - kTwinuiPreferredBase;
    const uint8_t stateCheck[] = {0x48, 0x83, 0xB9, 0x60, 0x01,
                                  0x00, 0x00, 0x00};
    const uint8_t showSlot[] = {0x48, 0x8B, 0x40, 0x38};
    if (!PrologueMatches(module, rva, kShowStartViewPrologue,
                         sizeof(kShowStartViewPrologue)) ||
        !BodyContains(module, rva, 0x400, stateCheck, sizeof(stateCheck)) ||
        !BodyContains(module, rva, 0x400, showSlot, sizeof(showSlot))) {
        FileLog(L"own hook skipped name=showstartview build=10.0.26100.9949 "
                L"body moved");
        return false;
    }
    return true;
}

// The event hooks read the state object at [self+0x40] (the same dword the
// IXamlLauncherState interface reads at +0x28). OnStartShown writes 5 there;
// OnStartHidden completes the known transitions.
bool OnStartShownProbe(HMODULE module) {
    const uintptr_t rva = kOnStartShownVa - kTwinuiPreferredBase;
    const uint8_t stateBias[] = {0x48, 0x8D, 0x77, 0x40};
    const uint8_t stateFive[] = {0xC7, 0x06, 0x05, 0x00, 0x00, 0x00};
    if (!PrologueMatches(module, rva, kOnStartShownPrologue,
                         sizeof(kOnStartShownPrologue)) ||
        !BodyContains(module, rva, 0x600, stateBias, sizeof(stateBias)) ||
        !BodyContains(module, rva, 0x600, stateFive, sizeof(stateFive))) {
        FileLog(L"own hook skipped name=shown build=10.0.26100.9949 body "
                L"moved");
        return false;
    }
    return true;
}

bool OnStartHiddenProbe(HMODULE module) {
    const uintptr_t rva = kOnStartHiddenVa - kTwinuiPreferredBase;
    const uint8_t stateBias[] = {0x48, 0x8D, 0x59, 0x40};
    const uint8_t stateTen[] = {0xC7, 0x03, 0x0A, 0x00, 0x00, 0x00};
    const bool prologue = PrologueMatches(module, rva, kOnStartHiddenPrologue,
                                          sizeof(kOnStartHiddenPrologue));
    const bool bias =
        BodyContains(module, rva, 0x100, stateBias, sizeof(stateBias));
    const bool ten =
        BodyContains(module, rva, 0x100, stateTen, sizeof(stateTen));
    if (!prologue || !bias || !ten) {
        FileLog(L"own hook skipped name=hidden build=10.0.26100.9949 "
                L"prologue=%d bias=%d ten=%d",
                prologue ? 1 : 0, bias ? 1 : 0, ten ? 1 : 0);
        return false;
    }
    return true;
}

// The three entrances the caller discriminator names. Each is proven twice: the
// function body is the one the RVA was read from, and the return address the
// hook will see is the instruction after a call that is still there.
bool HideCallerSitesProbe(HMODULE module) {
    // The caller constants are RVAs; only the three function starts below are
    // preferred VAs.
    const uintptr_t ordinaryRva = kHideCallerOrdinaryVa - 5;
    const uintptr_t clickAwayRva = kHideCallerClickAwayVa - 5;
    const uintptr_t lockRva = kHideCallerLockVa - 5;
    const uint8_t vtableSlot[] = {0x48, 0x8B, 0x40, 0x48};

    const bool ordinary =
        PrologueMatches(module, kDismissStartAndCortanaVa - kTwinuiPreferredBase,
                        kDismissStartAndCortanaPrologue,
                        sizeof(kDismissStartAndCortanaPrologue)) &&
        DirectCallTargets(module, ordinaryRva,
                          kHideStartAndCortanaVa - kTwinuiPreferredBase);
    const bool clickAway =
        PrologueMatches(module, kClickAwayDelegateVa - kTwinuiPreferredBase,
                        kClickAwayDelegatePrologue,
                        sizeof(kClickAwayDelegatePrologue)) &&
        BodyContains(module, clickAwayRva - 4, 4, vtableSlot,
                     sizeof(vtableSlot)) &&
        DirectCallTargets(module, clickAwayRva, 0x76E010);
    const uint8_t lockSite[] = {0xBA, 0x07, 0x00, 0x00, 0x00, 0x48,
                                0x8B, 0x01, 0x48, 0x8B, 0x40, 0x48};
    const bool lock =
        PrologueMatches(module, kLockDismissVa - kTwinuiPreferredBase,
                        kLockDismissPrologue,
                        sizeof(kLockDismissPrologue)) &&
        BodyContains(module, kLockDismissVa - kTwinuiPreferredBase,
                     0x60, lockSite, sizeof(lockSite)) &&
        DirectCallTargets(module, lockRva, 0x76E010);

    const uint8_t awaySite[] = {0xBA, 0x07, 0x00, 0x00, 0x00, 0x48,
                                0x8B, 0x01, 0x48, 0x8B, 0x40, 0x48};
    const bool away =
        PrologueMatches(module, kClickAwayHandlerVa - kTwinuiPreferredBase,
                        kClickAwayHandlerPrologue,
                        sizeof(kClickAwayHandlerPrologue)) &&
        BodyContains(module, kClickAwayHandlerVa - kTwinuiPreferredBase, 0x200,
                     awaySite, sizeof(awaySite)) &&
        DirectCallTargets(module, kHideCallerAwayVa - 5, 0x76E010);

    if (!ordinary || !clickAway || !lock || !away) {
        FileLog(L"own hook skipped name=callers build=10.0.26100.9949 "
                L"ordinary=%d clickaway=%d lock=%d away=%d",
                ordinary ? 1 : 0, clickAway ? 1 : 0, lock ? 1 : 0,
                away ? 1 : 0);
        return false;
    }
    return true;
}

bool HideStartAndCortanaProbe(HMODULE module) {
    const uintptr_t functionRva = kHideStartAndCortanaVa - kTwinuiPreferredBase;
    if (!ImageRangeExecutable(module, functionRva,
                              kHideStartAndCortanaProbeBytes)) {
        FileLog(L"close hook skipped (target not readable)");
        Wh_Log(L"twinui.pcshell.dll does not map the HideStartAndCortana "
               L"address into an executable section of the image; the close "
               L"hook is refused");
        return false;
    }

    const auto* code = reinterpret_cast<const uint8_t*>(module) + functionRva;
    for (size_t i = 0; i < sizeof(kHideStartAndCortanaPrologue); i++) {
        if (code[i] != kHideStartAndCortanaPrologue[i]) {
            FileLog(L"close hook skipped build=10.0.26100.9949 prologue "
                    L"mismatch");
            Wh_Log(L"twinui.pcshell.dll 10.0.26100.9949 does not carry the "
                   L"HideStartAndCortana prologue this hook was written for; "
                   L"the close hook is refused");
            return false;
        }
    }

    // The three body markers: the transient write 5 -> 9, the manager load from
    // this object's +0x38 and the Hide slot +0x40. They are what proves the
    // function still moves the state the decision depends on.
    const uint8_t stateNine[] = {0xC7, 0x41, 0x28, 0x09, 0x00, 0x00, 0x00};
    const uint8_t managerLoad[] = {0x48, 0x8B, 0x4F, 0x38};
    const uint8_t hideSlot[] = {0x48, 0x8B, 0x40, 0x40};
    bool foundState = false;
    bool foundManager = false;
    bool foundSlot = false;
    for (size_t i = 0; i + sizeof(stateNine) <= kHideStartAndCortanaProbeBytes;
         i++) {
        if (!foundState &&
            std::memcmp(code + i, stateNine, sizeof(stateNine)) == 0) {
            foundState = true;
        }
        if (!foundManager &&
            std::memcmp(code + i, managerLoad, sizeof(managerLoad)) == 0) {
            foundManager = true;
        }
        if (!foundSlot && std::memcmp(code + i, hideSlot, sizeof(hideSlot)) == 0) {
            foundSlot = true;
        }
    }
    if (!foundState || !foundManager || !foundSlot) {
        FileLog(L"close hook skipped build=10.0.26100.9949 state machine moved");
        Wh_Log(L"twinui.pcshell.dll 10.0.26100.9949 carries the "
               L"HideStartAndCortana prologue but not the state write and "
               L"manager call this hook was written for; the close hook is "
               L"refused");
        return false;
    }

    return true;
}

bool InstallHook(void* target, void* hook, void** original,
                 const wchar_t* name) {
    if (!target) {
        Wh_Log(L"Cannot hook %s: null target", name);
        return false;
    }
    if (!Wh_SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Failed to hook %s", name);
        return false;
    }
    Wh_Log(L"Hooked %s", name);
    return true;
}

void InstallExplorerHooks(HMODULE module) {
    if (!module) {
        return;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    // One attempt per process: a mismatch is not retried on every module load.
    if (g_explorerHooked.exchange(true)) {
        return;
    }
    if (!TwinuiBuildMatches(module)) {
        // The gate logged the mismatch; no hook is installed at all.
        return;
    }

    const uintptr_t base = reinterpret_cast<uintptr_t>(module);
    const auto at = [base](uintptr_t preferredVa) -> void* {
        return reinterpret_cast<void*>(base +
                                       (preferredVa - kTwinuiPreferredBase));
    };

    InstallHook(
        at(kTouchEdgeGestureOperationStartedVa),
        reinterpret_cast<void*>(InputGestureOperationStarted_Hook),
        reinterpret_cast<void**>(&InputGestureOperationStarted_Original),
        L"TouchEdgeGestureProcessor::InputGestureOperationStarted");

    InstallHook(
        at(kTouchEdgeGestureOperationEndedVa),
        reinterpret_cast<void*>(InputGestureOperationEnded_Hook),
        reinterpret_cast<void**>(&InputGestureOperationEnded_Original),
        L"TouchEdgeGestureProcessor::InputGestureOperationEnded");

    InstallHook(
        at(kReportEdgeGestureVa),
        reinterpret_cast<void*>(ReportEdgeGesture_Hook),
        reinterpret_cast<void**>(&ReportEdgeGesture_Original),
        L"EdgeAnimationRelationshipManager::ReportEdgeGesture");

    InstallHook(at(kSetAnimationDataVa),
                reinterpret_cast<void*>(SetAnimationData_Hook),
                reinterpret_cast<void**>(&SetAnimationData_Original),
                L"CStartExperienceManager::SetAnimationData");

    InstallHook(at(kShowVa), reinterpret_cast<void*>(Show_Hook),
                reinterpret_cast<void**>(&Show_Original),
                L"CStartExperienceManager::Show");

    // The close executor first, so the observation hook below can never be the
    // only one installed on a build whose state machine moved: the close is then
    // not animated at all instead of being delayed by a hook that cannot decide.
    bool closeHook = false;
    if (HideStartAndCortanaProbe(module) &&
        InstallHook(at(kHideStartAndCortanaVa),
                    reinterpret_cast<void*>(HideStartAndCortana_Hook),
                    reinterpret_cast<void**>(&HideStartAndCortana_Original),
                    L"XamlLauncherState::HideStartAndCortana")) {
        g_closeHookEnabled.store(true, std::memory_order_relaxed);
        closeHook = true;
        g_twinuiBase = base;
        FileLog(L"close trigger hook installed rva=24bcb0");
    }

    // The caller discriminator is what keeps the lock screen out of the
    // animation and the click-away in it. Its result is a capability, not a
    // warning: without the proven sites every close is unattributable, so the
    // hook that turns a close into a decision is refused and every close passes
    // to the shell (OwnResolveCaller answers Unknown for the same reason).
    const bool callers = closeHook && HideCallerSitesProbe(module);
    g_callersVerified.store(callers, std::memory_order_relaxed);
    if (closeHook && !callers) {
        FileLog(L"own close hook installed without callers; no close decision "
                L"is intercepted");
    }

    // The primitives the two decision hooks call. Probed together: a decision
    // that can only close or only open would be half a toggle. They also depend
    // on the caller capability, because a primitive that receives method 6/11
    // without it cannot tell a physical press from a caller the build moved.
    const bool primitives =
        closeHook && callers && DismissProbe(module) && ShowStartViewProbe(module);
    if (primitives) {
        g_dismiss = reinterpret_cast<DismissFn>(at(kDismissVa));
        g_showStartView = reinterpret_cast<ShowStartViewFn>(at(kShowStartViewVa));
        if (InstallHook(at(kDismissVa),
                        reinterpret_cast<void*>(Dismiss_Hook),
                        reinterpret_cast<void**>(&Dismiss_Original),
                        L"XamlLauncher::Dismiss") &&
            InstallHook(at(kShowStartViewVa),
                        reinterpret_cast<void*>(ShowStartView_Hook),
                        reinterpret_cast<void**>(&ShowStartView_Original),
                        L"XamlLauncher::ShowStartView")) {
            g_ownPrimitivesEnabled.store(true, std::memory_order_relaxed);
            FileLog(L"own hooks installed name=primitives");
        }
    } else {
        FileLog(L"own hook skipped name=primitives build=10.0.26100.9949 "
                L"callers=%d",
                callers ? 1 : 0);
    }

    // Both decisions share the one owner state. Each hook is probed and installed
    // alone: a refused Windows-key hook leaves the Start button working and the
    // shell's own decision for the key, and the reverse.
    if (primitives && OnHotkeyInvokeProbe(module) &&
        InstallHook(at(kOnHotkeyInvokeVa),
                    reinterpret_cast<void*>(OnHotkeyInvoke_Hook),
                    reinterpret_cast<void**>(&OnHotkeyInvoke_Original),
                    L"XamlLauncher::OnHotkeyInvoke")) {
        g_ownWinEnabled.store(true, std::memory_order_relaxed);
        FileLog(L"own hook installed name=win rva=14195c");
    }
    if (primitives && OnStartButtonPressedProbe(module) &&
        InstallHook(at(kOnStartButtonPressedVa),
                    reinterpret_cast<void*>(OnStartButtonPressed_Hook),
                    reinterpret_cast<void**>(&OnStartButtonPressed_Original),
                    L"XamlLauncher::OnStartButtonPressed")) {
        g_ownButtonEnabled.store(true, std::memory_order_relaxed);
        FileLog(L"own hook installed name=startbutton rva=492c90");
    }

    // The events only mirror the boolean and serve the reopen latch. Both are
    // probed together: a Shown without a Hidden would show an open that no close
    // ever clears.
    if (OnStartShownProbe(module) && OnStartHiddenProbe(module) &&
        InstallHook(at(kOnStartShownVa),
                    reinterpret_cast<void*>(OnStartShown_Hook),
                    reinterpret_cast<void**>(&OnStartShown_Original),
                    L"XamlLauncherState::OnStartShown") &&
        InstallHook(at(kOnStartHiddenVa),
                    reinterpret_cast<void*>(OnStartHidden_Hook),
                    reinterpret_cast<void**>(&OnStartHidden_Original),
                    L"XamlLauncherState::OnStartHidden")) {
        g_ownEventsEnabled.store(true, std::memory_order_relaxed);
        FileLog(L"own hooks installed name=events");
    }

    InstallHook(at(kHideVa), reinterpret_cast<void*>(Hide_Hook),
                reinterpret_cast<void**>(&Hide_Original),
                L"CStartExperienceManager::Hide");
}

void TryInstallFromLoadedModules(bool apply) {
    if (g_disabled.load(std::memory_order_relaxed)) {
        return;
    }

    bool installed = false;
    if (!g_explorerHooked.load(std::memory_order_relaxed)) {
        HMODULE twinui = GetModuleHandleW(kTwinuiModuleName);
        if (twinui) {
            InstallExplorerHooks(twinui);
            installed = true;
        }
    }

    if (installed && apply && g_afterInit.load(std::memory_order_relaxed)) {
        Wh_ApplyHookOperations();
    }
}

// ---------------------------------------------------------------------------
// Late load of twinui.pcshell.dll
// ---------------------------------------------------------------------------

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original
                         ? LoadLibraryExW_Original(fileName, file, flags)
                         : nullptr;

    if (module && !g_explorerHooked.load(std::memory_order_relaxed)) {
        if (module == GetModuleHandleW(kTwinuiModuleName)) {
            TryInstallFromLoadedModules(true);
            // Nothing is created here. This hook runs on whichever thread called
            // LoadLibraryExW -- a late load can be any thread, and a window on
            // the wrong one is never pumped (the taskbar thread is the one this
            // project measured as an explorer fail-fast for a show). The window
            // is created from the next known launcher entry (OnHotkeyInvoke,
            // Dismiss, ShowStartView, the manager's Show) or by the worker's
            // WH_GETMESSAGE hook targeted at the launcher thread it observed.
        }
    }

    return module;
}

// ---------------------------------------------------------------------------
// Module lifetime
// ---------------------------------------------------------------------------

// Any address in this module identifies the image that must stay mapped. The
// function is never called; only its address is taken.
void ModulePinAnchor() {}

// The pinned image, once per process. A re-init of the same image keeps it.
HMODULE g_modulePin = nullptr;

// Pins this image until the process terminates. A close-hook frame that returns
// after Wh_ModUninit still needs its own return address and the CloseCallLease
// destructor to be valid, and a hook-only mod cannot wait for its caller the way
// a threaded mod can. The pin covers exactly that: this image's code, its
// globals and the lease destructor.
//
// It does not cover the MinHook trampoline. That allocation belongs to the host,
// which removes hooks and frees trampolines only after Wh_ModUninit, following
// its ThreadsCallStackWaitForRegions(..., 200, 400) scan; the scan is bounded and
// its result is discarded, so a frame it sees is a frame it waited on, and a
// frame it does not see is one nothing covered. "The scan saw the frame" and
// "the frame exited before removal" are two claims, and only the first follows
// from that scan. The counted frames are covered instead by the drain, which
// returns only when the count has reached zero.
//
// A failure is a refusal, not a warning: Wh_ModInit returns FALSE and installs
// nothing. A shell-owned thread that resumes in an unmapped image is the crash
// this pin exists to prevent, and "the mod owns no thread" says nothing about a
// thread the shell owns. The success is logged once; a re-enable of the same
// image runs Wh_ModInit again and finds g_modulePin already set.
bool PinThisModule() {
    if (g_modulePin) {
        return true;  // a re-init of the same pinned image
    }
    HMODULE module = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_PIN,
                           reinterpret_cast<LPCWSTR>(&ModulePinAnchor),
                           &module)) {
        g_modulePin = module;
        FileLog(L"mod pin image=%p pinned=1", reinterpret_cast<void*>(module));
        return true;
    }
    FileLog(L"mod pin failed err=%u", static_cast<unsigned>(GetLastError()));
    return false;
}

// A pinned image is initialised a second time in the same process: after a
// clean unload the module is still mapped, so the next enable runs Wh_ModInit
// on that same image and the loader initialises nothing. Every lifecycle flag
// the previous activation left behind is cleared here, so the second init
// installs its hooks again instead of believing they are still there.
//
// Wh_ModInit calls this only when g_closeCalls is zero: a close from the previous
// activation that is still inside would otherwise see g_disabled cleared and the
// globals re-pointed under it. A refused activation never reaches this function.
// A drained unload returns at zero, so that refusal is reachable only through the
// entry window the count cannot see; g_closeCalls itself is still deliberately not
// touched, because such a frame owns its lease and that count must say so.
void ResetReinitState() {
    g_initRefused.store(false, std::memory_order_relaxed);
    g_disabled.store(false, std::memory_order_relaxed);
    g_afterInit.store(false, std::memory_order_relaxed);
    g_explorerHooked.store(false, std::memory_order_relaxed);
    g_closeHookEnabled.store(false, std::memory_order_relaxed);
    g_startPointTick.store(0, std::memory_order_relaxed);
    // The decision layer's per-activation state: a re-enable starts with no
    // cached toggle, no latch, no forwarded mark and no primitives from the
    // previous activation. The executor's state belongs here too: a pending
    // command, a latch or a thread id inherited from the previous activation
    // would post into a window that no longer exists, or create the next one on
    // the wrong thread. The worker and the window were joined and destroyed by
    // the unload; a timeout there refuses the re-enable instead of clearing them.
    g_ownWinEnabled.store(false, std::memory_order_relaxed);
    g_ownButtonEnabled.store(false, std::memory_order_relaxed);
    g_ownEventsEnabled.store(false, std::memory_order_relaxed);
    g_ownPrimitivesEnabled.store(false, std::memory_order_relaxed);
    g_callersVerified.store(false, std::memory_order_relaxed);
    g_dismiss = nullptr;
    g_showStartView = nullptr;
    g_twinuiBase = 0;
    g_execReady.store(false, std::memory_order_relaxed);
    g_execWindow.store(nullptr, std::memory_order_relaxed);
    g_execThreadId = 0;
    g_launcherThread.store(0, std::memory_order_relaxed);
    g_execAcceptedSeq = 0;
    g_pendingDismissSeq = 0;
    g_pendingShowSeq = 0;
    g_dismissReadRetries = 0;
    g_pendingDismissConfirmSeq = 0;
    g_pendingDismissConfirmVisible = 0;
    g_pendingDismissConfirmTick = 0;
    g_pendingDismissEpoch = 0;
    g_dispatchEpoch = 0;
    g_pressLatch = PressLatch{};
    g_pressServeReason = L"timeout";
    t_ownExternalMethod = 0;
    // The executor's lifetime protocol, which the refusal checks above have
    // proved complete: no worker, no identified window, no creator. Only now is
    // admission reopened; a stop that did not complete left it closed and the
    // refusal above kept this function out. The creator reservation is NOT set
    // here: a flag cleared by the reaper would be a proof it did not obtain.
    g_execAdmissionOpen.store(true, std::memory_order_release);
    g_execStopTarget.store(nullptr, std::memory_order_release);
    g_execWorkerId = 0;
    AcquireSRWLockExclusive(&g_ownLock);
    g_ownOpen = false;
    ReleaseSRWLockExclusive(&g_ownLock);
    // A failed gesture mapping is retried by the next activation, as it was
    // when every activation loaded a fresh image. A ready one is kept: the
    // section and this process's view of it are still valid.
    if (g_sharedInitState.load(std::memory_order_relaxed) == kSharedFailed) {
        g_sharedInitState.store(kSharedNotStarted, std::memory_order_relaxed);
        g_sharedErrorLogged.store(false, std::memory_order_relaxed);
    }
}

}  // namespace

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
    if (!g_twinuiBase) {
        return nullptr;
    }
    void** slot = reinterpret_cast<void**>(
        g_twinuiBase + kLauncherSingletonRva);
    void* outer = *slot;
    if (outerOut) {
        *outerOut = outer;
    }
    if (!outer) {
        return nullptr;
    }
    void* launcher = static_cast<uint8_t*>(outer) + kHotkeyLauncherOffset;
    const uint64_t state = *reinterpret_cast<uint64_t*>(
        static_cast<uint8_t*>(launcher) + kLauncherStateOffset);
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
    if (!launcher || !g_twinuiBase) {
        return ProbeVisibility::Unknown;
    }
    const LauncherIsVisibleFn isVisible = reinterpret_cast<LauncherIsVisibleFn>(
        g_twinuiBase + kLauncherIsVisibleRva);
    BOOL visible = FALSE;
    const HRESULT hr = isVisible(launcher, &visible);
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
        FileLog(L"exec dismiss superseded seq=%lld by=%lld",
                static_cast<long long>(g_pendingDismissConfirmSeq),
                static_cast<long long>(sequence));
        AnswerExec(g_pendingDismissConfirmSeq, kSwipeAckSuperseded, S_OK,
                   g_pendingDismissConfirmVisible);
    }
    g_pendingDismissConfirmSeq = sequence;
    g_pendingDismissConfirmVisible = ProbeVisibilityValue(visible);
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        FileLog(L"exec dismiss confirm timer failed seq=%lld reason=no-window",
                static_cast<long long>(sequence));
        g_pendingDismissConfirmSeq = 0;
        AnswerExec(sequence, kSwipeAckUnknown, S_OK, kSwipeVisibilityUnknown);
        return;
    }
    if (SetTimer(window, kExecDismissProbeTimer,
                 static_cast<UINT>(kExecDismissConfirmMs), nullptr) == 0) {
        FileLog(L"exec dismiss confirm timer failed seq=%lld err=%u",
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
    FileLog(L"exec %s seq=%lld superseded menu=%llu now=%llu", kind,
            static_cast<long long>(command.seq),
            static_cast<unsigned long long>(command.open),
            static_cast<unsigned long long>(open));
    return false;
}

void ExecuteShow(const SwipeIpcExecCommand& command) {
    const uint64_t now = GetTickCount64();
    if (ExecCommandExpired(command, now)) {
        // The publisher gave up before this dispatch started: its sequence is
        // cleared and its ownership released. Presenting now would open a menu no
        // controller owns, so the record is expired and the primitive is not
        // called.
        FileLog(L"exec show seq=%lld expired age=%llu (publisher released it)",
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
        FileLog(L"exec dismiss confirm superseded seq=%lld reason=show",
                static_cast<long long>(g_pendingDismissConfirmSeq));
        AnswerExec(g_pendingDismissConfirmSeq, kSwipeAckSuperseded, S_OK,
                   g_pendingDismissConfirmVisible);
    }
    KillDismissConfirm();
    uint64_t state = 0;
    void* outer = nullptr;
    void* launcher = LauncherNow(&state, &outer);
    if (!launcher) {
        FileLog(L"exec show seq=%lld refused outer=%p launcher=%p state=0x%llX "
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
        FileLog(L"exec show seq=%lld superseded epoch=%u now=%u",
                static_cast<long long>(command.seq), g_dispatchEpoch, epoch);
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        return;
    }
    ++t_execCall;
    const HRESULT hr = g_showStartView
                           ? g_showStartView(launcher, command.method ? command.method : 1,
                                             command.flags)
                           : E_FAIL;
    // IsVisible right after the call is honest but early: the shell's show is
    // still running and the window is cloaked, so the answer is 0 here. The
    // confirmation is read again by the probe timer on this same thread.
    const ProbeVisibility visible = LauncherVisibleNow(launcher);
    --t_execCall;
    FileLog(L"exec show seq=%lld outer=%p launcher=%p state=0x%llX hr=%08X "
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
        FileLog(L"exec dismiss seq=%lld expired age=%llu candidate=%lld",
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
        FileLog(L"exec dismiss seq=%lld refused outer=%p launcher=%p "
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
        FileLog(L"exec dismiss seq=%lld superseded epoch=%u now=%u",
                static_cast<long long>(command.seq), g_pendingDismissEpoch,
                epoch);
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-superseded", L"epoch");
        return;
    }
    ++t_execCall;
    const HRESULT hr =
        g_dismiss ? g_dismiss(launcher, command.method ? command.method : 1)
                  : E_FAIL;
    // `hr` is the primitive's answer, and the visibility right after it is
    // checked again by the confirmation probe: success is not yet a hide.
    const ProbeVisibility visible = LauncherVisibleNow(launcher);
    --t_execCall;
    FileLog(L"exec dismiss seq=%lld outer=%p launcher=%p state=0x%llX hr=%08X "
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
        FileLog(L"exec dismiss timer failed err=%u", GetLastError());
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
            FileLog(L"exec dismiss seq=%lld read retry=%d",
                    static_cast<long long>(g_pendingDismissSeq),
                    g_dismissReadRetries);
            return;
        }
        const LONG64 sequence = g_pendingDismissSeq;
        const int retries = g_dismissReadRetries;
        g_pendingDismissSeq = 0;
        g_dismissReadRetries = 0;
        FileLog(L"exec dismiss seq=%lld refused reason=unreadable retries=%d",
                static_cast<long long>(sequence), retries);
        AnswerExec(sequence, kSwipeAckRefused, E_FAIL, 0);
        DropPressLatch(L"dismiss-refused", L"unreadable");
        return;
    }
    g_dismissReadRetries = 0;
    if (command.seq != g_pendingDismissSeq ||
        command.kind != kSwipeExecDismiss) {
        FileLog(L"exec dismiss seq=%lld superseded by %lld",
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
    if (command.kind == kSwipeExecCancel) {
        if (g_pendingDismissSeq != 0 &&
            (command.target == 0 || command.target == g_pendingDismissSeq)) {
            KillTimer(g_execWindow.load(), kExecDismissTimer);
            FileLog(L"exec cancel seq=%lld target=%lld",
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
                FileLog(L"exec dismiss seq=%lld armed in=%u ms epoch=%u",
                        static_cast<long long>(command.seq), delay,
                        g_pendingDismissEpoch);
                return;
            }
            // No timer means the command would be armed forever. Run it now: a
            // close that cuts its settle short is still a close, while a dismiss
            // nobody runs leaves the controller owning a panel whose window is
            // still on screen.
            FileLog(L"exec dismiss seq=%lld armed=0 reason=no-timer",
                    static_cast<long long>(command.seq));
        }
        g_pendingDismissEpoch = g_dispatchEpoch;
        ExecuteDismiss(command);
    }
}

LRESULT CALLBACK ExecutorWindowProc(HWND window, UINT message, WPARAM wParam,
                                    LPARAM lParam) {
    // Every dispatch that can reach the launcher or the channel holds a lease.
    // Wh_ModUninit waits for the count to reach zero before it closes the
    // channel, so no command can run against a closed section.
    const CloseCallLease dispatch;
    if (message == kExecTeardownMessage) {
        // The owner-thread shutdown. This is a private request, not a generic
        // WM_CLOSE: the one-shot timers would otherwise outlive the window, and a
        // timer that fires after Wh_ModUninit would read a session the channel no
        // longer maps. Only the identified target answers: a recycled HWND that
        // now belongs to another window never reaches this procedure, and a
        // window of this image that this activation did not name refuses, which
        // is what makes the handle a statement about this window.
        const bool identified =
            g_execStopTarget.load(std::memory_order_acquire) == window;
        if (wParam != kExecTeardownMagic || !identified) {
            FileLog(L"exec teardown ignored hwnd=%p identified=%d",
                    window, identified ? 1 : 0);
            return 0;
        }
        KillTimer(window, kExecDismissTimer);
        KillTimer(window, kExecProbeTimer);
        KillTimer(window, kPressLatchTimer);
        KillTimer(window, kExecDismissProbeTimer);
        DestroyWindow(window);
        return 0;
    }
    if (message == WM_NCDESTROY) {
        // The acknowledgement names the window it is for. A stop waits on one
        // identified HWND; any other window's destruction (an old activation's,
        // a class the shell recreated) must not answer for it.
        bool identified = false;
        if (g_execWindow.load(std::memory_order_acquire) == window) {
            g_execWindow.store(nullptr, std::memory_order_release);
            identified = true;
        }
        if (g_execStopTarget.load(std::memory_order_acquire) == window) {
            g_execStopTarget.store(nullptr, std::memory_order_release);
            identified = true;
        }
        if (identified && g_execWindowGone) {
            SetEvent(g_execWindowGone);
        }
        FileLog(L"exec window gone hwnd=%p identified=%d tid=%u", window,
                identified ? 1 : 0, GetCurrentThreadId());
        return 0;
    }
    // A window this activation did not publish is not a dispatcher. A survivor
    // from a previous activation has this image's procedure and would otherwise
    // see this activation's globals and channel; its timers are the sharper
    // edge, because a dismiss armed for an old menu would fire into a new one.
    // The teardown above is the only message such a window answers.
    if (window != g_execWindow.load(std::memory_order_acquire)) {
        return DefWindowProcW(window, message, wParam, lParam);
    }
    if (!g_execReady.load(std::memory_order_acquire) ||
        !g_execAdmissionOpen.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed) ||
        g_initRefused.load(std::memory_order_relaxed)) {
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
        FileLog(L"exec visible seq=%lld launcher=%p state=0x%llX visible=%s",
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
            FileLog(L"exec dismiss %s seq=%lld visible=%s at_call=%d after=%llu "
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
            FileLog(L"exec launcher thread tid=%u source=%s", tid, source);
            known = tid;
        }
    }
    if (known != tid) {
        FileLog(L"exec launcher thread mismatch known=%u tid=%u source=%s",
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
    // One creator at a time. The reservation is what a shutdown drains: a creator
    // that is inside CreateWindowExW must publish before the stop reads the HWND,
    // and a creator that arrives after admission closed must not publish at all.
    // The admission test sits after the reservation, not before it, so those two
    // cases are the only two a stop can see.
    bool expected = false;
    if (!g_execCreating.compare_exchange_strong(expected, true,
                                                std::memory_order_acq_rel)) {
        return;  // another creator holds it; its outcome is this call's outcome
    }
    struct CreatorDone {
        ~CreatorDone() {
            g_execCreating.store(false, std::memory_order_release);
        }
    } creatorDone;

    if (!g_execAdmissionOpen.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed) ||
        g_initRefused.load(std::memory_order_relaxed) ||
        g_execWindow.load(std::memory_order_relaxed)) {
        return;
    }
    // The class is unique per activation. A Windhawk reinstall maps a new image
    // at a new base while the old one stays pinned; a fixed class name would
    // return ERROR_CLASS_ALREADY_EXISTS and CreateWindowEx would use the old
    // image's WndProc, whose channel and globals belong to the previous
    // activation. Naming the class by this image's WndProc address keeps every
    // activation's window on its own procedure.
    static wchar_t className[64] = L"";
    if (!className[0]) {
        swprintf_s(className, L"WindhawkSwipeExecutor_%p",
                   reinterpret_cast<void*>(&ExecutorWindowProc));
    }
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = ExecutorWindowProc;
        windowClass.hInstance = GetModuleHandleW(nullptr);
        windowClass.lpszClassName = className;
        if (!RegisterClassExW(&windowClass) &&
            GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            FileLog(L"exec window class failed err=%u", GetLastError());
            return;
        }
        registered = true;
    }
    HWND window = CreateWindowExW(0, className, L"", 0, 0, 0, 0,
                                  0, HWND_MESSAGE, nullptr,
                                  GetModuleHandleW(nullptr), nullptr);
    if (!window) {
        FileLog(L"exec window create failed err=%u", GetLastError());
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
        FileLog(L"exec window thread mismatch owner=%u launcher=%u",
                g_execThreadId, launcherThread);
    }
    FileLog(L"exec window hwnd=%p tid=%u class=%s", window, g_execThreadId,
            className);
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
    if (!g_execAdmissionOpen.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {
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
        FileLog(L"exec window hook failed err=%u", GetLastError());
        return false;
    }
    const ULONGLONG start = GetTickCount64();
    while (!g_execWindow.load(std::memory_order_relaxed) &&
           GetTickCount64() - start < timeoutMs) {
        Sleep(5);
    }
    UnhookWindowsHookEx(hook);
    const bool ready = g_execWindow.load(std::memory_order_relaxed) != nullptr;
    if (!ready) {
        FileLog(L"exec window deferred: launcher thread %u did not pump in "
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

    // Availability is published HERE, not by the thread that called CreateThread.
    // The new thread can run (and exit) before CreateThread returns; a creator's
    // store of `true` after the exit guard's `false` would leave the gate open for
    // a worker that no longer exists. The positive publication, the exit
    // withdrawal and the stop therefore all belong to this thread, and the gate
    // also requires admission, which the stop closes first.
    if (g_execAdmissionOpen.load(std::memory_order_acquire) &&
        !g_disabled.load(std::memory_order_relaxed) &&
        !g_initRefused.load(std::memory_order_relaxed)) {
        g_execReady.store(true, std::memory_order_release);
        FileLog(L"exec worker ready tid=%u window=%p", GetCurrentThreadId(),
                g_execWindow.load(std::memory_order_relaxed));
    }

    HANDLE events[2] = {g_execStopEvent, g_ipc.execEvent};
    for (;;) {
        const DWORD wait = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0) {
            return 0;
        }
        if (wait == WAIT_FAILED) {
            FileLog(L"exec worker wait failed err=%u", GetLastError());
            return 0;
        }
        if (wait != WAIT_OBJECT_0 + 1) {
            FileLog(L"exec worker wait ended unexpectedly result=%u", wait);
            return 0;
        }
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        for (int i = 0; i < 50 && !window; ++i) {
            Sleep(1);
            window = g_execWindow.load(std::memory_order_relaxed);
        }
        if (!window) {
            FileLog(L"exec command dropped: no window");
            CreateExecutorWindowOnUiThread(500);
            window = g_execWindow.load(std::memory_order_relaxed);
            if (!window) {
                continue;
            }
        }
        if (!PostMessageW(window, kExecMessage, 0, 0)) {
            FileLog(L"exec command post failed err=%u", GetLastError());
        }
    }
}

// Closes the worker's two handles and clears its identity. Only called when the
// thread has been joined, because a handle a live thread still waits on must
// never be closed: the next enable would otherwise hand that thread a recycled
// value for its wait.
void ReapExecutorWorker() {
    if (!g_execWorker) {
        return;
    }
    CloseHandle(g_execWorker);
    g_execWorker = nullptr;
    if (g_execStopEvent) {
        CloseHandle(g_execStopEvent);
        g_execStopEvent = nullptr;
    }
    FileLog(L"exec worker joined tid=%u", g_execWorkerId);
    g_execWorkerId = 0;
}

void StartExecutorWorker() {
    if (g_execWorker || !g_ipc.execEvent) {
        return;
    }
    if (!g_execWindowGone) {
        g_execWindowGone = CreateEventW(nullptr, TRUE, TRUE, nullptr);
        if (!g_execWindowGone) {
            FileLog(L"exec window gone event failed err=%u", GetLastError());
        }
    }
    g_execStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_execStopEvent) {
        FileLog(L"exec worker stop event failed err=%u", GetLastError());
        return;
    }
    g_execWorker = CreateThread(nullptr, 0, ExecutorWorkerProc, nullptr, 0,
                                nullptr);
    if (!g_execWorker) {
        FileLog(L"exec worker thread failed err=%u", GetLastError());
        CloseHandle(g_execStopEvent);
        g_execStopEvent = nullptr;
        return;
    }
    g_execWorkerId = GetThreadId(g_execWorker);
    FileLog(L"exec worker started tid=%u", g_execWorkerId);
}

// Proves that no creator of this activation is still between CreateWindowExW and
// its publication. Admission must already be closed: a creator that arrives
// after this proof cannot start, so `false` here is a durable statement and not
// one moment of an interleaving. A timeout is returned, never repaired by
// clearing the reservation: a flag cleared by the reaper would let the same
// window publish behind the snapshot it was cleared for.
bool DrainExecutorCreator(const wchar_t* why) {
    const ULONGLONG deadline = GetTickCount64() + kExecCreatorDrainMs;
    while (g_execCreating.load(std::memory_order_acquire) &&
           GetTickCount64() < deadline) {
        Sleep(1);  // a creator is inside CreateWindowExW; its publish is next
    }
    if (g_execCreating.load(std::memory_order_acquire)) {
        FileLog(L"exec creator still inside (%s)", why);
        return false;
    }
    return true;
}

// The private shutdown. A generic WM_CLOSE would only say "close some window";
// this request names the window this activation is tearing down, and the
// procedure answers it only when that window is the recorded target.
bool PostExecutorTeardown(HWND window) {
    return PostMessageW(window, kExecTeardownMessage, kExecTeardownMagic, 0) !=
           FALSE;
}

// The one window shutdown. Returns true only when no worker and no window of this
// activation can run a command any more. False means an outstanding participant
// still uses the channel, the stop event or the window's message loop: the caller
// keeps all of them and the next activation refuses until the matching join or
// destruction has completed.
//
// The order is the fix for a same-image re-enable. Admission closes first, so no
// creator can publish a window behind the snapshot and no command can be served;
// the creator reservation is drained, so a creation that already passed admission
// finishes and is seen. Then the worker is joined, and only then is the
// identified window destroyed — directly when this call already runs on its owner
// thread, otherwise with the private teardown the owner acknowledges from
// WM_NCDESTROY.
bool StopExecutorWorker() {
    // Admission first, readiness second: the worker publishes its own readiness
    // and the window's procedure requires admission, so a late positive
    // publication from a worker that has not yet noticed the stop is never
    // readable as a service.
    g_execAdmissionOpen.store(false, std::memory_order_release);
    g_execReady.store(false, std::memory_order_release);

    const bool creatorDrained = DrainExecutorCreator(L"stop");
    if (!creatorDrained) {
        FileLog(L"exec window creator did not leave in %u ms",
                static_cast<unsigned>(kExecCreatorDrainMs));
    }

    // A window that was already published belongs to this activation even when
    // its creator has not released the reservation yet; a window that is still
    // null under a live creator is not a snapshot this stop may record. The
    // re-enable proves the creator out first and takes the window then.
    HWND window = g_execWindow.load(std::memory_order_acquire);
    if (window) {
        g_execStopTarget.store(window, std::memory_order_release);
        if (g_execWindow.load(std::memory_order_acquire) != window) {
            // The window was destroyed between the snapshot and the target store,
            // so its WM_NCDESTROY could not see the target and cleared nothing. A
            // stale target here would refuse every later enable forever.
            g_execStopTarget.store(nullptr, std::memory_order_release);
            FileLog(L"exec window already gone hwnd=%p", window);
            window = nullptr;
        }
    }

    bool workerStopped = true;
    if (g_execWorker) {
        SetEvent(g_execStopEvent);
        const DWORD joined = WaitForSingleObject(g_execWorker, kExecJoinWaitMs);
        if (joined == WAIT_OBJECT_0) {
            ReapExecutorWorker();
        } else {
            // The worker still waits on both handles and on the channel's Exec
            // event. Closing either would hand it a recycled handle, and closing
            // the channel would unmap the event it is waiting on; all three stay
            // with the pinned image, and the next enable refuses until this join
            // has actually completed.
            workerStopped = false;
            FileLog(L"exec worker join timeout tid=%u (handles and channel kept)",
                    g_execWorkerId);
        }
    }

    bool windowStopped = true;
    if (window) {
        if (g_execThreadId == GetCurrentThreadId()) {
            // Already on the owner thread: destroy directly, no post and no ack.
            DestroyWindow(window);
            windowStopped = g_execWindow.load(std::memory_order_acquire) == nullptr;
            if (windowStopped) {
                FileLog(L"exec window destroyed directly tid=%u",
                        static_cast<unsigned>(g_execThreadId));
            } else {
                FileLog(L"exec window destroy refused tid=%u",
                        static_cast<unsigned>(g_execThreadId));
            }
        } else {
            if (!g_execWindowGone) {
                g_execWindowGone = CreateEventW(nullptr, TRUE, TRUE, nullptr);
                if (!g_execWindowGone) {
                    FileLog(L"exec window gone event failed err=%u",
                            GetLastError());
                }
            }
            if (!g_execWindowGone) {
                // Nothing can acknowledge the destruction, and leaving the
                // window is not a stop. Keep it and make the teardown
                // outstanding rather than pretending it completed.
                windowStopped = false;
                FileLog(L"exec window close unacknowledged tid=%u (no event)",
                        static_cast<unsigned>(g_execThreadId));
            } else if (g_execStopTarget.load(std::memory_order_acquire) !=
                       window) {
                // The owner destroyed it while the worker was being joined; its
                // acknowledgement was already consumed (or could not name this
                // target yet). Nothing is left to wait for.
                FileLog(L"exec window gone before the close hwnd=%p", window);
            } else {
                ResetEvent(g_execWindowGone);
                if (!PostExecutorTeardown(window)) {
                    FileLog(L"exec window teardown post failed err=%u",
                            GetLastError());
                    windowStopped = false;
                } else if (WaitForSingleObject(g_execWindowGone,
                                               kExecJoinWaitMs) ==
                           WAIT_OBJECT_0) {
                    FileLog(L"exec window destroyed tid=%u",
                            static_cast<unsigned>(g_execThreadId));
                } else {
                    // The teardown kills the timers and destroys the window; the
                    // ack is WM_NCDESTROY's, and it has not come. The dispatch
                    // gate keeps the surviving window harmless, but the
                    // activation is not gone and nothing is closed under it.
                    FileLog(L"exec window close timeout tid=%u",
                            static_cast<unsigned>(g_execThreadId));
                    windowStopped = false;
                }
            }
        }
    }
    if (windowStopped) {
        g_execStopTarget.store(nullptr, std::memory_order_release);
    }
    return creatorDrained && workerStopped && windowStopped;
}

// Answers whether a teardown that a previous activation left outstanding has
// completed since. It is also the second half of the creation race: a creator
// that was inside CreateWindowExW when the stop ran its drain can publish a
// window after that stop returned, so the window found here is destroyed exactly
// as the unload would have destroyed it. A worker that has exited is joined and
// its handles closed. True means nothing of that activation is left, which is the
// one state a re-enable may inherit.
//
// The two moments of the old version are the bug this order removes. It read the
// published HWND and then the creator reservation: a creator that publishes and
// releases between those two reads was seen as "no HWND" and "no creator", and
// the enable cleared the HWND that had just been written — the exact window the
// reaper existed to destroy, still running with this image's procedure and a new
// activation's channel. With admission closed, the reservation is drained FIRST;
// the publish happens before the release, so a null HWND read after the drain is
// a durable answer. The reservation is never cleared to look quiescent.
bool ReapExecutorTeardown() {
    if (g_execWorker) {
        if (WaitForSingleObject(g_execWorker, 0) == WAIT_OBJECT_0) {
            ReapExecutorWorker();
        } else {
            FileLog(L"exec reinit refused: the previous worker is still running "
                    L"tid=%u",
                    g_execWorkerId);
            return false;
        }
    }
    if (!DrainExecutorCreator(L"reinit")) {
        FileLog(L"exec reinit refused: a window creation from the previous "
                L"activation is still inside");
        return false;
    }
    HWND target = g_execStopTarget.load(std::memory_order_acquire);
    if (!target) {
        target = g_execWindow.load(std::memory_order_acquire);
        if (target) {
            // A window nobody was waiting for: the stop timed out on its creator.
            // From here it is the identified target, and the creator proof above
            // is what makes it the only one this activation can have.
            g_execStopTarget.store(target, std::memory_order_release);
            FileLog(L"exec teardown found a surviving window hwnd=%p", target);
        }
    }
    if (target) {
        if (!IsWindow(target)) {
            g_execWindow.store(nullptr, std::memory_order_release);
            g_execStopTarget.store(nullptr, std::memory_order_release);
            FileLog(L"exec teardown cleared a dead window hwnd=%p", target);
        } else {
            // Finish the stop here rather than refusing forever: the window is
            // this image's, so its own teardown path destroys it and
            // acknowledges. The event is reset for this target, never read as an
            // acknowledgement left by an earlier stop. IsWindow alone is not the
            // identity test — a recycled handle would answer it — which is why
            // the request carries the private message and the owner checks it
            // against the recorded target before destroying.
            if (!g_execWindowGone) {
                g_execWindowGone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
                if (!g_execWindowGone) {
                    FileLog(L"exec reinit refused: no acknowledgement event for "
                            L"the surviving window hwnd=%p",
                            target);
                    return false;
                }
            }
            ResetEvent(g_execWindowGone);
            bool destroyed = false;
            if (g_execThreadId == GetCurrentThreadId()) {
                DestroyWindow(target);
                destroyed = !IsWindow(target);
            } else if (PostExecutorTeardown(target) &&
                       WaitForSingleObject(g_execWindowGone, kExecJoinWaitMs) ==
                           WAIT_OBJECT_0) {
                destroyed = true;
            }
            if (!destroyed) {
                FileLog(L"exec reinit refused: the previous window still exists "
                        L"hwnd=%p",
                        target);
                return false;
            }
            g_execWindow.store(nullptr, std::memory_order_release);
            g_execStopTarget.store(nullptr, std::memory_order_release);
            FileLog(L"exec teardown completed late hwnd=%p", target);
        }
    }
    if (g_execCreating.load(std::memory_order_acquire)) {
        // Cannot happen with admission closed and the drain above; kept as the
        // refusal that would hold if a future path reopened admission early.
        FileLog(L"exec reinit refused: a window creation appeared after the "
                L"creator proof");
        return false;
    }
    FileLog(L"exec teardown reaped hwnd=%p", static_cast<void*>(target));
    return true;
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
    FileLog(L"intent kind=%s trigger=%s seq=%lld open=%llu method=%u flags=%u",
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


BOOL Wh_ModInit() {
    Wh_Log(L"Start Menu Touch Gesture Bridge 0.25.0 initializing");

    // A disable/enable cycle re-initialises this same image, because the pin
    // keeps it mapped. Nothing in it was re-initialised by the loader, so the
    // previous activation's lifecycle flags must be cleared explicitly — but
    // only once nothing of that activation is still running. A drained unload
    // returns at zero, so a close can still be inside only through the entry
    // window the count cannot see; clearing g_disabled and re-pointing the
    // globals under that frame would hand it a half-reset module, so this
    // activation refuses and leaves the previous state exactly as the unload
    // left it. The next enable tries again.
    const bool reinit = g_modulePin != nullptr;
    if (reinit) {
        // A re-enable takes the executor's lifetime over with admission closed.
        // The proof in ReapExecutorTeardown is only durable while no creator can
        // start, and ResetReinitState is the one place that reopens it.
        g_execAdmissionOpen.store(false, std::memory_order_release);
    }
    if (reinit && g_closeCalls.load(std::memory_order_acquire) != 0) {
        g_initRefused.store(true, std::memory_order_relaxed);
        FileLog(L"mod init refused: a close from the previous activation is "
                L"still inside; no hook, no channel and no thread installed");
        return FALSE;
    }
    if (reinit && !ReapExecutorTeardown()) {
        // A stop that timed out leaves the worker's handles, the window and the
        // channel in place. Clearing the HWND, the owner TID and the watermark
        // here would hand a surviving window this activation's channel, so the
        // enable refuses instead. A later enable reaps the completion, because
        // the acknowledgement names the window it waited for.
        g_initRefused.store(true, std::memory_order_relaxed);
        FileLog(L"mod init refused: the previous activation's executor has not "
                L"finished stopping; no hook, no channel and no thread "
                L"installed");
        return FALSE;
    }
    ResetReinitState();
    if (reinit) {
        FileLog(L"activation state reset");
    }

    // A prerequisite, before the channel and the hooks: a close frame that
    // outlives the unload must return into mapped code, and a shell-owned thread
    // needs valid return code as much as a worker would. Nothing is installed
    // when this fails; see PinThisModule.
    if (!PinThisModule()) {
        g_initRefused.store(true, std::memory_order_relaxed);
        FileLog(L"mod init refused: the image could not be pinned; no hook, no "
                L"channel and no thread installed");
        return FALSE;
    }

    // Create the objects up front so the first gesture never pays for it.
    EnsureShared();

    // The channel to the Start side. Opening it here means the section and its
    // events exist before the first Hide, and a Start mod that attaches later
    // joins the same ones. This is the only call: SwipeIpcOpen may sleep for its
    // init budget while the other side finishes a claim, so it must not run on
    // a hook. A failure here means every close runs at once for the life
    // of this attachment, which the budget makes reachable only for a section
    // that cannot be initialised at all.
    //
    // A re-enable of the pinned image starts with no channel, because a drained
    // unload closes it before Wh_ModUninit returns; a refused activation never
    // opened one. Either way, do not map over a channel this process already
    // holds: SwipeIpcOpen would leak the first and swap the globals under it.
    bool created = false;
    DWORD ipcError = 0;
    const bool channelOpen = g_ipc.state != nullptr;
    if (!channelOpen && !SwipeIpcOpen(&g_ipc, &created, &ipcError)) {
        Wh_Log(L"Swipe IPC channel unavailable (%u); the shell's own path "
               L"serves every trigger",
               static_cast<unsigned>(ipcError));
    } else {
        FileLog(L"ipc open created=%d", created ? 1 : 0);
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

    TryInstallFromLoadedModules(false);

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_initRefused.load(std::memory_order_relaxed)) {
        return;
    }
    g_afterInit = true;
    TryInstallFromLoadedModules(false);
    Wh_ApplyHookOperations();
}

void Wh_ModBeforeUninit() {
    // A refused activation installed nothing; the flag and the reset below
    // belong to the activation that is still running.
    if (g_initRefused.load(std::memory_order_relaxed)) {
        return;
    }
    // This must be set before Windhawk removes the hooks. Every hook and the
    // executor's WndProc reads it: a close or a dispatch that enters after this
    // point skips the channel, and one that is already inside holds its lease
    // until it returns. The v7 channel is closed in Wh_ModUninit, after that flag
    // and after the drain.
    g_disabled = true;
    g_startPointTick.store(0, std::memory_order_relaxed);

    PublishInactiveOnUnload();
}

void Wh_ModUninit() {
    // A refused activation installed nothing, so the channel and the count
    // below belong to the activation that is still running.
    if (g_initRefused.load(std::memory_order_relaxed)) {
        return;
    }

    // The executor first: it posts to the hidden window and its dispatches call
    // the launcher. StopExecutorWorker closes admission to window creation,
    // drains a creator, joins the worker before it closes either of the worker's
    // handles, and has the window destroyed and acknowledged. Its result is the
    // answer this unload needs: false means an outstanding participant still uses
    // the channel, the stop event or the window, so the channel stays and the
    // next enable refuses until that join or destruction has completed.
    const bool executorStopped = StopExecutorWorker();

    // Two channels, two lifetimes, for two reasons.
    //
    // The gesture mapping (Local\WindhawkStartGestureShared) and its event are
    // kept for the rest of the explorer process lifetime, on purpose. A consumer
    // maps the section independently and may poll it at any time, and an
    // in-flight gesture callback can still hold the view pointer that
    // EnsureShared cached; unmapping it here would turn either into an access to
    // a freed view. A re-enable re-initialises this same pinned image and
    // reuses the pair it already mapped; a different installed version maps its
    // own handles and leaves the previous image's pair to process exit. The
    // reset published by PublishInactiveOnUnload is what tells a reader that
    // this instance is gone, not the handle lifetime. Process exit reclaims
    // every pair.
    //
    // The v7 channel is closed, because its objects are named and its records
    // belong to the other process: g_ipc is unmapped and its mapping, mutex and
    // five events are closed once no close hook and no executor dispatch can
    // still be inside them AND the executor's own teardown completed. g_disabled
    // was set in Wh_ModBeforeUninit and the executor's ready flag was cleared by
    // StopExecutorWorker, so neither a new close decision nor a queued command
    // reaches the channel; g_closeCalls names the frames that are already inside.
    //
    // The wait below has no deadline, and that is the whole point. The lease
    // spans the whole frame, the original calls included, so a count of zero also
    // proves that no frame of either hook can still need the MinHook trampoline.
    // Windhawk removes the hooks and frees their trampolines as soon as this
    // function returns, so returning with a frame still about to call
    // the original would hand the host a frame whose trampoline it is about to
    // free. A timeout cannot stand in for the count: it would expire under
    // exactly that frame. The two outcomes are `drained` — the count reached
    // zero, the channel is closed, this call returns — and `stalled` — the count
    // is still nonzero and nothing else happens: no close, no return, no handle
    // left behind. The price is paid where it is owed: a close that does not
    // return, because the shell call under it never came back or a debugger
    // suspended its thread, blocks the Windhawk reload until it resumes.
    //
    // kIpcUnloadMarkMs and kIpcUnloadStallReportMs are diagnostics only. Neither
    // grants anything and neither ends the wait. The reports go through FileLog,
    // so a close wedged inside FileLog keeps them out of the file; that changes no
    // outcome, because this call is blocked either way.
    //
    // The pin and the count are not the same cover. The pin keeps an entry that
    // has not yet taken its lease from returning into unmapped code; the drain
    // covers the trampoline use of every counted frame. What remains is the
    // host's unload: Windhawk calls this function before it waits for threads
    // inside the mod image (ModsManager::ReloadModsAndSettings calls
    // Wh_ModUninit, then ThreadsCallStackWaitForRegions(..., 200, 400), then
    // frees the module), and that wait is bounded and its result discarded. So
    // "the scan found the frame" and "the frame exited before removal" are two
    // claims: the first is what that scan gives when it can see the stack, the
    // second is what this drain gives for every counted frame, and neither one
    // is proof for an uncounted entry. The pin does not reach the trampoline; it
    // is the host's allocation, not this image.
    const ULONGLONG drainStart = GetTickCount64();
    bool markLogged = false;
    ULONGLONG nextStallReport = 0;
    for (;;) {
        const int closes = g_closeCalls.load(std::memory_order_acquire);
        if (closes == 0) {
            break;
        }
        const ULONGLONG now = GetTickCount64();
        if (!markLogged &&
            now - drainStart >= static_cast<ULONGLONG>(kIpcUnloadMarkMs)) {
            markLogged = true;
            nextStallReport = now + kIpcUnloadStallReportMs;
            FileLog(L"ipc unload drain over %u ms closes=%d",
                    static_cast<unsigned>(kIpcUnloadMarkMs), closes);
        }
        if (markLogged && now >= nextStallReport) {
            nextStallReport = now + kIpcUnloadStallReportMs;
            FileLog(L"ipc unload stalled closes=%d waited=%llu ms", closes,
                    static_cast<unsigned long long>(now - drainStart));
        }
        Sleep(kHidePollMs);
    }

    // Drained: the count reached zero, so no counted frame can still need the
    // trampolines. The channel is a second question: it is closed only when the
    // executor's half of the unload also completed. A deferred teardown keeps it,
    // because the surviving worker waits on its Exec event and the surviving
    // window belongs to the same activation; closing it would hand a live thread
    // a closed handle and a reused one later.
    FileLog(L"ipc unload drained waited=%llu ms",
            static_cast<unsigned long long>(GetTickCount64() - drainStart));
    if (!executorStopped) {
        FileLog(L"ipc close deferred: the previous activation's executor has not "
                L"finished stopping; the channel and its handles stay");
        return;
    }
    if (g_ipc.state) {
        SwipeIpcClose(&g_ipc);
        FileLog(L"ipc close");
    }
}
