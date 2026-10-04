// ==WindhawkMod==
// @id              smooth-desktop-icons-auto-hide
// @name            Smooth Desktop Icons Auto-Hide
// @description     Smoothly auto-hide Windows desktop icons with click-to-show, double-click-to-hide, drag reveal and configurable fade animation.
// @version         0.10.4
// @author          HeyOkay
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smooth Desktop Icons Auto-Hide

**Smooth Desktop Icons Auto-Hide** is a Windhawk mod for Windows Explorer that automatically hides desktop icons when they are not needed and smoothly reveals them when the desktop is used.

## Features

- **Automatic icon hiding**
  - Desktop icons are hidden after a configurable period of inactivity.
  - Default timeout: **5 seconds**.
  - Configurable from **1 to 60 seconds**.

- **Single-click to show**
  - Clicking an empty area of the desktop reveals the icons.
  - Mouse movement alone does not reveal them.

- **Double-click to hide**
  - Double-clicking an empty area of the desktop hides the icons.

- **Smooth fade animation**
  - Icons appear and disappear using a smooth alpha fade.
  - Animation duration is configurable from **50 to 1000 ms**.
  - Default: **250 ms**.

- **Drag & Drop support**
  - Dragging a file toward the desktop reveals the icons before the drop.
  - The icons remain available while the drag is active.

- **Desktop activity awareness**
  - After the user interacts with the desktop, auto-hide is paused while the desktop remains the active surface.
  - When the desktop becomes inactive, a new full auto-hide countdown starts.

- **Win+D support**
  - The mod tracks the standard Windows `Win+D` desktop shortcut and keeps icon visibility and auto-hide state synchronized with desktop activation.


## How It Works

Windows Explorer displays desktop icons through a `SysListView32` window located inside `SHELLDLL_DefView`.

The mod discovers the desktop ShellView and its ListView, then subclasses these existing Explorer windows to monitor desktop interaction and control icon visibility.

```text
Windows Explorer
       │
       ▼
SHELLDLL_DefView
       │
       ▼
SysListView32
       │
       ├── Desktop icon visibility
       ├── Mouse interaction
       ├── Double-click handling
       └── Drag & Drop detection
```

### Desktop window discovery

The mod monitors creation of Explorer windows and looks for:

- `SHELLDLL_DefView` — the desktop ShellView.
- `SysListView32` — the ListView that contains the desktop icons.

Existing desktop windows are also discovered when the mod starts.

The mod subclasses both relevant windows so that it can process their messages without replacing or recreating the desktop ListView.

### Showing and hiding icons

The mod directly controls the existing desktop `SysListView32` with:

```text
ShowWindow(list, SW_SHOW)
ShowWindow(list, SW_HIDE)
```

The existing ListView remains in place. The mod does not use Explorer's internal desktop-icon toggle command for normal show/hide operations.

Keeping the same ListView preserves its existing icon state and avoids an unnecessary Explorer desktop-icon refresh during visibility changes.

### Fade animation

The fade effect is implemented on the existing ListView using `WS_EX_LAYERED` and `SetLayeredWindowAttributes`.

When showing:

```text
ListView hidden
      │
      ▼
ShowWindow(SW_SHOW)
      │
      ▼
Alpha = 0
      │
      ▼
Gradually increase alpha
      │
      ▼
Alpha = 255
      │
      ▼
Fully visible
```

When hiding, the same process runs in reverse:

```text
Alpha = 255
      │
      ▼
Gradually decrease alpha
      │
      ▼
Alpha = 0
      │
      ▼
ShowWindow(SW_HIDE)
```

The animation uses smoothstep easing:

```text
t² × (3 − 2t)
```

This produces a smooth acceleration/deceleration instead of a linear opacity change.

### Icon state machine

The mod tracks four icon states:

```text
Hidden
   │
   ▼
Showing
   │
   ▼
Visible
   │
   ▼
Hiding
   │
   ▼
Hidden
```

A new show/hide request can interrupt the current transition, allowing the animation to move smoothly toward the new target state.

### Click handling

When the icons are hidden, a left-click on the empty desktop is handled by `SHELLDLL_DefView` and reveals the ListView.

When the icons are visible, `SysListView32` receives the mouse interaction. The mod performs a ListView hit test so that double-click behavior is applied only to empty desktop space and not to an actual icon.

A real desktop click also marks the desktop as actively used and temporarily cancels the auto-hide countdown.

### Auto-hide logic

Auto-hide is state-aware rather than an unconditional timer.

```text
User activates desktop
        │
        ▼
   Icons visible
        │
        ▼
 Auto-hide paused
        │
        ▼
Desktop becomes inactive
        │
        ▼
Start fresh countdown
        │
        ▼
     Timeout
        │
        ▼
   Fade-out animation
        │
        ▼
   Icons hidden
```

The mod checks the actual foreground/root window relationship to determine whether the desktop is still the active surface.

This prevents the icons from disappearing while the user is actively working with the desktop.

### Drag detection

The mod does not install a global low-level mouse hook.

Instead, a lightweight polling timer checks the physical left-button state and the window under the cursor. When the left button is held and the cursor enters the desktop surface, the mod treats this as a drag/desktop interaction and reveals the icons.

This allows a drag that started in another Explorer window to reveal the desktop icons before the file is dropped.

Mouse movement with no button held does not reveal the icons.


### Win+D handling

`Win+D` is handled through lightweight key-state polling because Explorer does not always send a reliable mouse or focus message to the desktop ShellView when the shortcut changes the active surface.

The mod uses this state to distinguish:

```text
Win+D
  │
  ├── Application → Desktop
  │       └── reveal icons / pause auto-hide
  │
  └── Desktop → Application
          └── start a fresh auto-hide countdown
```

### Timers

The implementation uses separate timers for different jobs:

- **Auto-hide timer** — controls the user-configured inactivity timeout.
- **Interaction/drag polling timer** — runs at a short interval to detect drag entry, reconcile desktop activity, recover menu/button state, and track `Win+D`.
- **Animation timer** — updates the ListView alpha during fade transitions.

These responsibilities are kept separate so that interaction polling does not reset the user's auto-hide countdown.

## Cleanup and Safety

When the mod is unloaded or disabled, it restores the desktop ListView to a normal Explorer state.

Cleanup:

1. Stops active timers.
2. Restores full opacity.
3. Removes the temporary layered-window style.
4. Makes the desktop ListView visible if it was hidden by the mod.
5. Removes the ListView and ShellView subclasses.
6. Resets the internal state.

This prevents the mod from leaving desktop icons permanently hidden after the mod is disabled or Explorer is restarted.

## Settings

### Enable Auto-hide

Enables or disables automatic hiding.

### Hide after seconds

Time of inactivity before the icons are hidden.

- **Range:** 1–60 seconds
- **Default:** 5 seconds

### Animation duration (ms)

Duration of the fade animation.

- **Range:** 50–1000 ms
- **Default:** 250 ms

## Known Issue

### Virtual Desktops

When switching to a newly created Windows Virtual Desktop, a **black background/visual artifact** may occasionally appear behind the desktop icons while the icons are visible.

This is related to the interaction between the layered `SysListView32` window and Explorer's rendering/composition behavior during Virtual Desktop transitions.

The issue does not affect the core functionality of the mod and is currently considered a known issue.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoHideEnabled: true
  $name: Enable Auto-hide
  $description: Automatically hide desktop icons after inactivity.
- autoHideDelay: 5
  $name: Hide after seconds
  $description: "Inactivity time before icons are hidden. Range: 1-60."
- animationDuration: 250
  $name: Animation duration (ms)
  $description: "Fade duration. Range: 50-1000 ms."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <windhawk_utils.h>

static constexpr UINT_PTR kTimerAutoHide = 0x53444901;
static constexpr UINT_PTR kTimerAnimation = 0x53444902;
static constexpr UINT_PTR kTimerDragPoll = 0x53444903;
static constexpr UINT kDragPollIntervalMs = 50;

static UINT g_msgRefresh = 0;
static UINT g_msgShow = 0;
static UINT g_msgHide = 0;

struct Settings {
    bool autoHideEnabled = true;
    int autoHideDelay = 5;
    int animationDuration = 250;
} g_settings;

enum class IconState {
    Hidden,
    Showing,
    Visible,
    Hiding,
};

struct DesktopState {
    HWND shell = nullptr;
    HWND list = nullptr;

    IconState state = IconState::Hidden;

    // Original v0.5 animation state. The real visibility is still controlled
    // by Explorer's native desktop-icons command.
    BYTE alpha = 0;
    BYTE animFrom = 0;
    BYTE animTo = 0;
    DWORD animStart = 0;

    bool dragActive = false;
    bool interactionActive = false;
    bool suppressDragPoll = false;
    bool suppressDesktopFocusUntilClick = false;
    bool contextMenuActive = false;
    // Once the user clicks the desktop, keep auto-hide paused while the
    // desktop remains the active/focused surface. This lets the user inspect
    // icons without the list disappearing underneath their eyes.
    bool desktopFocusActive = false;
    // Prevent the 50 ms polling loop from restarting the 5-second countdown.
    bool autoHideTimerArmed = false;

    // Win+D is polled because it does not reliably produce mouse/focus
    // messages for the desktop ShellView. These flags debounce the chord
    // and prevent the focus reconciliation from immediately undoing the
    // second Win+D transition.
    bool winDPressed = false;
    bool winDExitPending = false;

};

static DesktopState g_states[16]{};
static int g_stateCount = 0;

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t g_CreateWindowExW = nullptr;

static DesktopState* FindStateByShell(HWND shell);
static void CancelAutoHide(DesktopState* state);
static void StartAutoHide(DesktopState* state);
static void ShowIcons(DesktopState* state);

static DesktopState* FindStateByShell(HWND shell) {
    for (int i = 0; i < g_stateCount; ++i) {
        if (g_states[i].shell == shell)
            return &g_states[i];
    }
    return nullptr;
}

static DesktopState* FindStateByList(HWND list) {
    for (int i = 0; i < g_stateCount; ++i) {
        if (g_states[i].list == list)
            return &g_states[i];
    }
    return nullptr;
}

static DesktopState* GetOrCreateState(HWND shell) {
    if (auto* existing = FindStateByShell(shell))
        return existing;

    if (g_stateCount >= static_cast<int>(_countof(g_states)))
        return nullptr;

    auto* state = &g_states[g_stateCount++];
    *state = {};
    state->shell = shell;
    return state;
}

static bool IsClass(HWND hwnd, const wchar_t* className) {
    if (!hwnd)
        return false;

    wchar_t actual[128]{};

    return GetClassNameW(
               hwnd,
               actual,
               _countof(actual)) &&
           wcscmp(actual, className) == 0;
}

static HWND FindDesktopList(HWND shell) {
    if (!IsClass(shell, L"SHELLDLL_DefView"))
        return nullptr;

    return FindWindowExW(
        shell,
        nullptr,
        L"SysListView32",
        nullptr);
}

static void LoadSettings() {
    g_settings.autoHideEnabled =
        Wh_GetIntSetting(L"autoHideEnabled") != 0;

    g_settings.autoHideDelay =
        Wh_GetIntSetting(L"autoHideDelay");

    if (g_settings.autoHideDelay < 1)
        g_settings.autoHideDelay = 1;

    if (g_settings.autoHideDelay > 60)
        g_settings.autoHideDelay = 60;

    g_settings.animationDuration =
        Wh_GetIntSetting(L"animationDuration");

    if (g_settings.animationDuration < 50)
        g_settings.animationDuration = 50;

    if (g_settings.animationDuration > 1000)
        g_settings.animationDuration = 1000;

    Wh_Log(
        L"[SmoothDesktop] Settings: autoHide=%d delay=%d animation=%d",
        static_cast<int>(g_settings.autoHideEnabled),
        g_settings.autoHideDelay,
        g_settings.animationDuration);
}

static void CancelAutoHide(DesktopState* state) {
    if (state && state->shell) {
        KillTimer(
            state->shell,
            kTimerAutoHide);
        state->autoHideTimerArmed = false;
    }
}

static void StartAutoHide(DesktopState* state) {
    if (!state ||
        !state->shell ||
        !g_settings.autoHideEnabled)
        return;

    // Keep the timer alive even while Desktop is active or a context menu is
    // open. The timer callback decides whether it is currently allowed to hide.
    // This is important because Explorer does not reliably deliver the menu
    // loop messages to the desktop view; making timer creation depend on those
    // messages can leave auto-hide permanently disabled.
    // PollDragReveal() runs every 50 ms. Do not restart the countdown on
    // every poll; doing so makes a 5-second timer effectively never expire.
    if (state->autoHideTimerArmed)
        return;

    SetTimer(
        state->shell,
        kTimerAutoHide,
        static_cast<UINT>(
            g_settings.autoHideDelay * 1000),
        nullptr);

    state->autoHideTimerArmed = true;
}

// Directly show/hide the already-created desktop ListView, matching the
// visibility mechanism used by the original v0.5 implementation. The
// ListView itself is not rebuilt or re-toggled through Explorer's 0x7402
// command, which avoids the generic file-type icon refresh observed with the
// native Explorer toggle. Cleanup explicitly restores visibility before
// unloading the mod.
static bool ToggleDesktopIconsNative(DesktopState* state) {
    if (!state || !state->list || !IsWindow(state->list))
        return false;

    const bool visible =
        IsWindowVisible(state->list) != FALSE;

    ShowWindow(
        state->list,
        visible ? SW_HIDE : SW_SHOW);

    return (IsWindowVisible(state->list) != FALSE) != visible;
}

static bool SetDesktopIconsVisible(
    DesktopState* state,
    bool visible) {

    if (!state || !state->list || !IsWindow(state->list))
        return false;

    const bool currentVisible = IsWindowVisible(state->list) != FALSE;
    if (currentVisible == visible)
        return true;

    if (!ToggleDesktopIconsNative(state))
        return false;

    // ShowWindow acts synchronously for the existing ListView. Confirm the
    // resulting state before updating our own state machine.
    return (IsWindowVisible(state->list) != FALSE) == visible;
}

static void SetAlpha(
    DesktopState* state,
    BYTE alpha) {

    if (!state || !state->list)
        return;

    LONG_PTR exStyle =
        GetWindowLongPtrW(
            state->list,
            GWL_EXSTYLE);

    if (!(exStyle & WS_EX_LAYERED)) {
        SetWindowLongPtrW(
            state->list,
            GWL_EXSTYLE,
            exStyle | WS_EX_LAYERED);
    }

    SetLayeredWindowAttributes(
        state->list,
        0,
        alpha,
        LWA_ALPHA);

    state->alpha = alpha;
}

static BYTE EaseAlpha(
    BYTE from,
    BYTE to,
    float t) {

    if (t <= 0.0f)
        return from;

    if (t >= 1.0f)
        return to;

    // Exact smoothstep easing from v0.5.
    float eased =
        t * t * (3.0f - 2.0f * t);

    float value =
        static_cast<float>(from) +
        (static_cast<float>(to) -
         static_cast<float>(from)) * eased;

    if (value < 0.0f)
        value = 0.0f;

    if (value > 255.0f)
        value = 255.0f;

    return static_cast<BYTE>(
        value + 0.5f);
}

static void FinishAnimation(
    DesktopState* state) {

    if (!state || !state->list)
        return;

    KillTimer(
        state->shell,
        kTimerAnimation);

    SetAlpha(
        state,
        state->animTo);

    if (state->animTo == 0) {
        // Explorer remains the owner of actual visibility.
        SetDesktopIconsVisible(state, false);
        state->state = IconState::Hidden;
    } else {
        state->state = IconState::Visible;
    }
}

static void TickAnimation(
    DesktopState* state) {

    if (!state || !state->list)
        return;

    DWORD elapsed =
        GetTickCount() -
        state->animStart;

    float progress =
        static_cast<float>(elapsed) /
        static_cast<float>(
            g_settings.animationDuration);

    if (progress >= 1.0f) {
        FinishAnimation(state);
        return;
    }

    SetAlpha(
        state,
        EaseAlpha(
            state->animFrom,
            state->animTo,
            progress));
}

static void AnimateTo(
    DesktopState* state,
    BYTE targetAlpha) {

    if (!state || !state->list)
        return;

    KillTimer(
        state->shell,
        kTimerAnimation);

    if (targetAlpha == state->alpha &&
        ((targetAlpha == 0 &&
          state->state == IconState::Hidden) ||
         (targetAlpha == 255 &&
          state->state == IconState::Visible))) {
        return;
    }

    if (targetAlpha != 0 &&
        !IsWindowVisible(state->list)) {

        // v0.5's important ordering: put the hidden ListView at alpha 0
        // before making it visible. The actual show is delegated to Explorer.
        SetAlpha(
            state,
            0);

        if (!SetDesktopIconsVisible(state, true)) {
            SetAlpha(state, 255);
            return;
        }
    }

    state->animFrom =
        state->alpha;

    state->animTo =
        targetAlpha;

    state->animStart =
        GetTickCount();

    state->state =
        targetAlpha > state->alpha
            ? IconState::Showing
            : IconState::Hiding;

    SetTimer(
        state->shell,
        kTimerAnimation,
        15,
        nullptr);
}

static void ShowIcons(
    DesktopState* state) {

    if (!state || !state->list)
        return;

    CancelAutoHide(state);

    if (state->state == IconState::Visible ||
        state->state == IconState::Showing) {

        StartAutoHide(state);
        return;
    }

    AnimateTo(
        state,
        255);

    StartAutoHide(state);
}

static void HideIcons(
    DesktopState* state) {

    if (!state || !state->list)
        return;

    if (state->dragActive ||
        state->interactionActive ||
        state->contextMenuActive)
        return;

    CancelAutoHide(state);

    AnimateTo(
        state,
        0);
}

static bool IsEmptyListPoint(
    HWND list,
    LPARAM lParam) {

    LVHITTESTINFO hit{};

    hit.pt.x =
        GET_X_LPARAM(lParam);

    hit.pt.y =
        GET_Y_LPARAM(lParam);

    LRESULT result =
        SendMessageW(
            list,
            LVM_HITTEST,
            0,
            reinterpret_cast<LPARAM>(&hit));

    return result == -1;
}

static bool IsEmptyDesktopPoint(DesktopState* state, LPARAM shellLParam) {
    if (!state || !state->list)
        return false;

    POINT pt{};
    pt.x = GET_X_LPARAM(shellLParam);
    pt.y = GET_Y_LPARAM(shellLParam);

    // Convert the SHELLDLL_DefView client coordinates to screen coordinates,
    // then to SysListView32 client coordinates before performing the hit test.
    if (!ClientToScreen(state->shell, &pt))
        return false;
    if (!ScreenToClient(state->list, &pt))
        return false;

    LPARAM listLParam = MAKELPARAM(
        static_cast<short>(pt.x),
        static_cast<short>(pt.y));

    return IsEmptyListPoint(state->list, listLParam);
}

static bool IsExplorerMenuLoopStillActive() {
    // Explorer's classic desktop context menu is normally tracked by a
    // temporary menu window (class #32768) that owns mouse capture.
    // WM_EXITMENULOOP is not guaranteed to reach our SHELLDLL_DefView/ListView
    // subclass on every Explorer code path, so use the live menu window as a
    // fallback source of truth.
    HWND capture = GetCapture();
    if (capture && IsWindow(capture)) {
        wchar_t cls[64]{};
        if (GetClassNameW(capture, cls, ARRAYSIZE(cls)) > 0 &&
            wcscmp(cls, L"#32768") == 0) {
            return true;
        }
    }

    HWND foreground = GetForegroundWindow();
    if (foreground && IsWindow(foreground)) {
        wchar_t cls[64]{};
        if (GetClassNameW(foreground, cls, ARRAYSIZE(cls)) > 0 &&
            wcscmp(cls, L"#32768") == 0) {
            return true;
        }
    }

    return false;
}

static bool IsCursorOverDesktop(DesktopState* state) {
    if (!state || !state->shell || !IsWindow(state->shell))
        return false;

    POINT pt{};
    if (!GetCursorPos(&pt))
        return false;

    // Do not use the SHELLDLL_DefView rectangle as the test. DefView/WorkerW
    // can cover the whole monitor, so that would also classify clicks in
    // other Explorer/application windows as desktop clicks.
    //
    // Instead compare the real window under the cursor with the desktop's
    // root window. When the ListView is hidden, WindowFromPoint normally
    // resolves to WorkerW (or one of its children), whose root is the same
    // root that contains SHELLDLL_DefView.
    HWND hit = WindowFromPoint(pt);
    if (!hit)
        return false;

    HWND desktopRoot = GetAncestor(state->shell, GA_ROOT);
    HWND hitRoot = GetAncestor(hit, GA_ROOT);

    if (desktopRoot && hitRoot == desktopRoot)
        return true;

    // If the icon ListView is visible, its descendants are also valid.
    HWND p = hit;
    for (int depth = 0; depth < 16 && p; ++depth) {
        if (p == state->shell || p == state->list)
            return true;
        p = GetParent(p);
    }

    return false;
}

static bool IsDesktopFocusStillActive(DesktopState* state) {
    if (!state || !state->shell || !IsWindow(state->shell))
        return false;

    HWND foreground = GetForegroundWindow();
    if (!foreground)
        return false;

    HWND desktopRoot = GetAncestor(state->shell, GA_ROOT);
    HWND foregroundRoot = GetAncestor(foreground, GA_ROOT);

    // The foreground window is the authoritative signal for whether the user
    // is currently on the desktop. Do NOT fall back to GetFocus() here.
    // GetFocus() is thread-local, and this code runs on Explorer's thread; it
    // can continue to return SysListView32 even after the user has switched to
    // another application. That stale focus made the auto-hide countdown stop
    // working after a focus change.
    return desktopRoot && foregroundRoot == desktopRoot;
}

static void UpdateDesktopFocusState(DesktopState* state) {
    if (!state)
        return;

    // A context menu or a deliberate double-click temporarily ends the
    // "desktop is being used" state. Do NOT clear this latch merely because
    // the foreground window changes: that was the source of the bug where a
    // context menu on a hidden desktop caused the icons to reappear when
    // focus changed. The latch is cleared only by a real left-click on the
    // desktop.
    const bool desktopActive = IsDesktopFocusStillActive(state);
    if (state->suppressDesktopFocusUntilClick)
        return;

    // The desktop can become the active surface without sending a mouse
    // message to our ShellView/ListView. Win+D is the important example:
    // Windows dismisses the foreground application and activates the desktop
    // directly. Therefore do not only test this while our flag is already
    // set; continuously reconcile the flag with the real desktop foreground
    // state.
    //
    // When the user presses Win+D a second time, we deliberately suppress
    // this reconciliation for the duration of the chord. Otherwise the
    // desktop can still look active for a few polling ticks and immediately
    // restore desktopFocusActive after we have decided to leave it.
    if (state->winDExitPending)
        return;

    if (desktopActive) {
        if (!state->desktopFocusActive) {
            state->desktopFocusActive = true;
            CancelAutoHide(state);

            // Win+D can activate the desktop without sending a mouse message
            // to SHELLDLL_DefView. In that case the normal click path never
            // calls ShowIcons(), so the desktop would become active while the
            // ListView stayed hidden. Treat the desktop becoming active as a
            // real show request. Because desktopFocusActive is already true,
            // ShowIcons() will keep the icons visible without starting the
            // auto-hide timer.
            if (state->state == IconState::Hidden ||
                state->state == IconState::Hiding) {
                ShowIcons(state);
            }
        }
        return;
    }

    if (state->desktopFocusActive) {
        // Losing Desktop focus is the exact start of a new inactivity period.
        // Always discard any timer that may have been armed while Desktop was
        // active and start a completely fresh full countdown from this
        // transition. This makes the behavior identical whether the user
        // leaves Desktop after 1 second or after 30 seconds.
        state->desktopFocusActive = false;
        CancelAutoHide(state);
        StartAutoHide(state);
    }
}

static void PollWinDToggle(DesktopState* state) {
    if (!state)
        return;

    const bool leftWin = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0;
    const bool rightWin = (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    const bool dDown = (GetAsyncKeyState('D') & 0x8000) != 0;
    const bool chordDown = (leftWin || rightWin) && dDown;

    if (chordDown && !state->winDPressed) {
        state->winDPressed = true;

        // Win+D is an explicit desktop command, so it must be able to recover
        // from the context-menu suppression state. Unlike a focus poll, this
        // is an intentional user action.
        state->suppressDesktopFocusUntilClick = false;

        // If Desktop is already our active surface, this is the second
        // Win+D: leave Desktop mode and start the normal auto-hide countdown.
        if (state->desktopFocusActive) {
            state->desktopFocusActive = false;
            state->winDExitPending = true;
            // Win+D is an explicit Desktop -> inactive transition. Reset the
            // countdown so it always starts from this exact moment.
            CancelAutoHide(state);
            StartAutoHide(state);
        } else {
            // First Win+D activates the desktop. If the icons are hidden, the
            // explicit command must reveal them even though no mouse message
            // reaches SHELLDLL_DefView.
            state->desktopFocusActive = true;
            state->winDExitPending = false;
            if (state->state == IconState::Hidden ||
                state->state == IconState::Hiding) {
                ShowIcons(state);
            } else {
                CancelAutoHide(state);
            }
        }
    }

    if (!chordDown) {
        state->winDPressed = false;

        if (state->winDExitPending) {
            state->winDExitPending = false;
        }
    }
}

static void PollDragReveal(DesktopState* state) {
    if (!state || !state->shell || !IsWindow(state->shell))
        return;

    PollWinDToggle(state);

    // Explorer does not reliably deliver WM_EXITMENULOOP to the desktop
    // ListView/SHELLDLL_DefView. If that happens, contextMenuActive can stay
    // stuck forever and StartAutoHide() will refuse to arm the hide timer.
    // Poll the actual menu window instead and release the guard as soon as
    // the context menu has really disappeared.
    const bool leftButtonDown =
        (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    const bool rightButtonDown =
        (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

    if (state->contextMenuActive && !IsExplorerMenuLoopStillActive()) {
        state->contextMenuActive = false;
    }

    // Explorer can also lose the mouse-button/menu-loop messages around the
    // desktop context menu. In that case interactionActive remains latched
    // even though the physical right button is already released. A stale
    // interaction flag blocks StartAutoHide(), and changing foreground focus
    // cannot repair it because the flag is local to our state. Use the real
    // button state as a final recovery condition.
    if (state->interactionActive &&
        !leftButtonDown &&
        !rightButtonDown &&
        !state->contextMenuActive) {
        state->interactionActive = false;
    }

    if (!state->interactionActive &&
        !state->contextMenuActive) {
        // If the context menu just closed and no mouse interaction is active,
        // re-arm the normal auto-hide countdown.
        StartAutoHide(state);
    }

    UpdateDesktopFocusState(state);

    // A deliberate double-click on empty desktop is a hide command.
    // Suppress the drag poll until the second mouse button release so the
    // polling fallback cannot immediately turn the icons back on.
    if (state->suppressDragPoll) {
        if (!leftButtonDown) {
            state->suppressDragPoll = false;
            state->dragActive = false;
        }
        return;
    }

    const bool overDesktop =
        leftButtonDown && IsCursorOverDesktop(state);

    if (overDesktop) {
        // This is deliberately gated by the physical left button state, so
        // ordinary mouse movement never reveals hidden icons. It catches both
        // a file being dragged from Explorer and a normal click/hold on the
        // empty desktop.
        if (!state->dragActive) {
            state->dragActive = true;
            CancelAutoHide(state);
        }

        if (state->state == IconState::Hidden ||
            state->state == IconState::Hiding) {
            ShowIcons(state);
        }
    } else if (state->dragActive && !leftButtonDown) {
        state->dragActive = false;
        state->interactionActive = false;
        StartAutoHide(state);
    }
}

static void HandleTimer(
    DesktopState* state,
    WPARAM timerId) {

    if (!state)
        return;

    if (timerId == kTimerAnimation) {
        TickAnimation(state);
        return;
    }

    if (timerId == kTimerDragPoll) {
        PollDragReveal(state);
        return;
    }

    if (timerId == kTimerAutoHide) {
        KillTimer(
            state->shell,
            kTimerAutoHide);
        state->autoHideTimerArmed = false;

        // If the countdown expires while Desktop is active, it has no meaning:
        // the user is allowed to stay on Desktop indefinitely. Do NOT turn it
        // into a 100 ms retry timer. When focus later leaves Desktop,
        // UpdateDesktopFocusState() will cancel any stale timer and start a
        // fresh full countdown from that exact transition.
        if (state->desktopFocusActive) {
            return;
        }

        // Dragging or a context menu can end without a foreground transition,
        // so keep a short retry while those states remain active.
        if (state->dragActive ||
            state->interactionActive ||
            state->contextMenuActive) {
            SetTimer(
                state->shell,
                kTimerAutoHide,
                100,
                nullptr);
            state->autoHideTimerArmed = true;
            return;
        }

        if (state->state == IconState::Visible) {
            HideIcons(state);
        }

        return;
    }

}

LRESULT CALLBACK DesktopListSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR) {

    DesktopState* state = FindStateByList(hWnd);
    if (!state)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
    case WM_LBUTTONDOWN:
        // A real left-click is the explicit signal that the user resumed
        // interacting with the desktop after a context menu/double-click.
        state->suppressDesktopFocusUntilClick = false;
        state->desktopFocusActive = true;
        state->interactionActive = true;
        CancelAutoHide(state);
        break;

    case WM_LBUTTONUP:
        state->interactionActive = false;
        state->dragActive = false;
        // Do NOT clear suppressDragPoll here. The drag-poll timer must keep
        // the suppression active until it observes that the physical left
        // button has actually been released. Otherwise the WM_LBUTTONUP that
        // follows WM_LBUTTONDBLCLK clears the flag too early and the polling
        // fallback immediately shows the icons again while the button is still
        // part of the double-click sequence.
        StartAutoHide(state);
        break;

    case WM_RBUTTONDOWN:
        state->interactionActive = true;
        // Opening the desktop context menu must end the "desktop is actively
        // being used" state. Otherwise desktopFocusActive remains true after
        // the menu closes and StartAutoHide() is permanently blocked until
        // some later left-button action (which is why double-click appeared
        // to fix the problem). Keep focus polling suppressed until the next
        // real desktop left-click.
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        CancelAutoHide(state);
        break;

    case WM_RBUTTONUP:
        state->interactionActive = false;
        StartAutoHide(state);
        break;

    case WM_CONTEXTMENU:
        // Explorer may route WM_CONTEXTMENU differently from the mouse/menu
        // loop messages. Treat it as the authoritative beginning of a desktop
        // context-menu interaction as well.
        state->contextMenuActive = true;
        state->interactionActive = false;
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        CancelAutoHide(state);
        break;

    case WM_ENTERMENULOOP:
        // The menu loop means the desktop is no longer an active interaction
        // surface. Suspend desktop-focus tracking for this menu invocation so
        // it cannot immediately re-block the auto-hide timer after the menu
        // closes. A subsequent left-click on the desktop re-enables it.
        state->contextMenuActive = true;
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        CancelAutoHide(state);
        break;

    case WM_EXITMENULOOP:
        state->contextMenuActive = false;
        StartAutoHide(state);
        break;

    case WM_LBUTTONDBLCLK:
        // The ListView is the real hit-test surface while icons are visible.
        // Toggle only when the double-click is on empty desktop space.
        if (IsEmptyListPoint(hWnd, lParam)) {
            // WM_LBUTTONDOWN arrives immediately before WM_LBUTTONDBLCLK and
            // marks the interaction as active. Clear that transient flag so
            // the deliberate double-click can hide the icons.
            // A context menu can be opened through Explorer's menu system,
            // and in some routing paths WM_EXITMENULOOP is not delivered to
            // the ListView subclass. A later deliberate double-click must
            // therefore clear the stale menu state itself.
            state->contextMenuActive = false;
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDragPoll = true;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            HideIcons(state);
            return 0;
        }
        break;

    case WM_TIMER:
        HandleTimer(state, wParam);
        if (wParam == kTimerAnimation ||
            wParam == kTimerAutoHide ||
            wParam == kTimerDragPoll)
            return 0;
        break;

    case WM_KILLFOCUS:
        state->desktopFocusActive = false;
        CancelAutoHide(state);
        StartAutoHide(state);
        break;

    case WM_NCDESTROY:
        KillTimer(state->shell, kTimerAnimation);
        KillTimer(state->shell, kTimerAutoHide);
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DesktopShellSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR) {

    DesktopState* state = GetOrCreateState(hWnd);
    if (!state)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == g_msgRefresh) {
        HWND newList = FindDesktopList(hWnd);
        if (newList && newList != state->list) {
            state->list = newList;
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                state->list, DesktopListSubclassProc, 0);
        } else if (!newList) {
            state->list = nullptr;
        }

        if (state->list) {
            KillTimer(hWnd, kTimerAutoHide);
            KillTimer(hWnd, kTimerAnimation);
            KillTimer(hWnd, kTimerDragPoll);
            SetTimer(hWnd, kTimerDragPoll, kDragPollIntervalMs, nullptr);

            // Required startup state: icons are hidden. Hide the existing
            // ListView directly, matching v0.5, then prepare it at alpha 0
            // for the v0.5 fade-in path.
            if (IsWindowVisible(state->list)) {
                ToggleDesktopIconsNative(state);
            }
            SetAlpha(state, 0);
            state->state = IconState::Hidden;
            state->dragActive = false;
            state->interactionActive = false;
            state->suppressDragPoll = false;
            state->suppressDesktopFocusUntilClick = false;
            state->contextMenuActive = false;
            state->desktopFocusActive = false;
            state->autoHideTimerArmed = false;
            state->winDPressed = false;
            state->winDExitPending = false;
        }

        return 0;
    }

    if (uMsg == g_msgShow) {
        ShowIcons(state);
        return 0;
    }

    if (uMsg == g_msgHide) {
        HideIcons(state);
        return 0;
    }

    switch (uMsg) {
    case WM_LBUTTONDBLCLK:
        // Depending on Explorer's hit-test routing, a double-click can reach
        // SHELLDLL_DefView instead of SysListView32. When the click is on
        // empty desktop space, hide the icons here as well.
        if (state->list && IsEmptyDesktopPoint(state, lParam)) {
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDragPoll = true;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            HideIcons(state);
            return 0;
        }
        break;

    case WM_LBUTTONDOWN:
        // A real left-click is the explicit signal that the user resumed
        // interacting with the desktop after a context menu/double-click.
        state->suppressDesktopFocusUntilClick = false;
        state->desktopFocusActive = true;
        // When SysListView32 is hidden, SHELLDLL_DefView receives the click.
        // A single click reveals the icons. The next click of a double-click
        // is then handled by SysListView32 and can hide them again.
        if (state->state == IconState::Hidden ||
            state->state == IconState::Hiding) {
            ShowIcons(state);

            // The first click reaches SHELLDLL_DefView while the ListView is
            // hidden. ShowIcons() makes the ListView visible, but Windows does
            // not automatically move keyboard focus to it. Without doing so,
            // the next keyboard command (for example Ctrl+A) is still sent to
            // the previously active application.
            if (state->list && IsWindow(state->list)) {
                SetFocus(state->list);
            }
        }
        state->interactionActive = true;
        CancelAutoHide(state);
        break;

    case WM_LBUTTONUP:
        state->interactionActive = false;
        // suppressDragPoll is cleared by PollDragReveal only after the
        // physical left button is released. Keeping it set here prevents the
        // polling fallback from undoing a deliberate double-click hide.
        StartAutoHide(state);
        break;

    case WM_RBUTTONDOWN:
        state->interactionActive = true;
        // Opening the desktop context menu must end the "desktop is actively
        // being used" state. Otherwise desktopFocusActive remains true after
        // the menu closes and StartAutoHide() is permanently blocked until
        // some later left-button action (which is why double-click appeared
        // to fix the problem). Keep focus polling suppressed until the next
        // real desktop left-click.
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        CancelAutoHide(state);
        break;

    case WM_RBUTTONUP:
        state->interactionActive = false;
        StartAutoHide(state);
        break;

    case WM_ENTERMENULOOP:
        // The menu loop means the desktop is no longer an active interaction
        // surface. Suspend desktop-focus tracking for this menu invocation so
        // it cannot immediately re-block the auto-hide timer after the menu
        // closes. A subsequent left-click on the desktop re-enables it.
        state->contextMenuActive = true;
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        CancelAutoHide(state);
        break;

    case WM_EXITMENULOOP:
        state->contextMenuActive = false;
        StartAutoHide(state);
        break;

    case WM_TIMER:
        HandleTimer(state, wParam);
        if (wParam == kTimerAnimation ||
            wParam == kTimerAutoHide ||
            wParam == kTimerDragPoll)
            return 0;
        break;

    case WM_KILLFOCUS:
        state->desktopFocusActive = false;
        CancelAutoHide(state);
        StartAutoHide(state);
        break;

    case WM_NCDESTROY:
        KillTimer(hWnd, kTimerAnimation);
        KillTimer(hWnd, kTimerAutoHide);
        KillTimer(hWnd, kTimerDragPoll);
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SubclassDesktopShell(
    HWND shell) {

    if (!shell)
        return;

    if (!WindhawkUtils::
            SetWindowSubclassFromAnyThread(
                shell,
                DesktopShellSubclassProc,
                0)) {

        Wh_Log(
            L"[SmoothDesktop] Failed to subclass ShellView %p",
            shell);

        return;
    }

    PostMessageW(
        shell,
        g_msgRefresh,
        0,
        0);
}

static BOOL CALLBACK EnumWindowsProc(
    HWND hWnd,
    LPARAM) {

    if (!IsClass(hWnd, L"Progman") &&
        !IsClass(hWnd, L"WorkerW"))
        return TRUE;

    HWND shell =
        FindWindowExW(
            hWnd,
            nullptr,
            L"SHELLDLL_DefView",
            nullptr);

    if (shell)
        SubclassDesktopShell(shell);

    return TRUE;
}

static void DiscoverExistingDesktop() {
    EnumWindows(
        EnumWindowsProc,
        0);
}

static HWND WINAPI HookCreateWindowExW(
    DWORD exStyle,
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

    HWND hWnd =
        g_CreateWindowExW(
            exStyle,
            className,
            windowName,
            style,
            x,
            y,
            width,
            height,
            parent,
            menu,
            instance,
            param);

    if (!hWnd ||
        !className ||
        IS_INTRESOURCE(className))
        return hWnd;

    if (wcscmp(
            className,
            L"SHELLDLL_DefView") == 0) {

        SubclassDesktopShell(hWnd);

    } else if (
        wcscmp(
            className,
            L"SysListView32") == 0 &&
        parent &&
        IsClass(
            parent,
            L"SHELLDLL_DefView")) {

        DesktopState* state =
            FindStateByShell(parent);

        if (state) {
            state->list = hWnd;

            WindhawkUtils::
                SetWindowSubclassFromAnyThread(
                    hWnd,
                    DesktopListSubclassProc,
                    0);

            PostMessageW(
                parent,
                g_msgRefresh,
                0,
                0);
        }
    }

    return hWnd;
}

static void Cleanup() {

    for (int i = 0;
         i < g_stateCount;
         ++i) {

        DesktopState* state =
            &g_states[i];

        if (state->shell) {
            KillTimer(
                state->shell,
                kTimerAutoHide);

            KillTimer(
                state->shell,
                kTimerDragPoll);
        }

        if (state->list &&
            IsWindow(state->list)) {

            // Restore the temporary v0.5 layered animation state before
            // unloading, then make sure the real ListView is visible.
            KillTimer(state->shell, kTimerAnimation);
            SetAlpha(state, 255);

            if (!IsWindowVisible(state->list)) {
                ToggleDesktopIconsNative(state);
            }

            LONG_PTR exStyle =
                GetWindowLongPtrW(
                    state->list,
                    GWL_EXSTYLE);
            if (exStyle & WS_EX_LAYERED) {
                SetWindowLongPtrW(
                    state->list,
                    GWL_EXSTYLE,
                    exStyle & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
            }

            WindhawkUtils::
                RemoveWindowSubclassFromAnyThread(
                    state->list,
                    DesktopListSubclassProc);
        }

        if (state->shell &&
            IsWindow(state->shell)) {

            WindhawkUtils::
                RemoveWindowSubclassFromAnyThread(
                    state->shell,
                    DesktopShellSubclassProc);
        }
    }

    g_stateCount = 0;
}

BOOL Wh_ModInit() {

    Wh_Log(
        L"[SmoothDesktop] Init");

    LoadSettings();

    g_msgRefresh =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Refresh");

    g_msgShow =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Show");

    g_msgHide =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Hide");

    if (!g_msgRefresh ||
        !g_msgShow ||
        !g_msgHide) {

        Wh_Log(
            L"[SmoothDesktop] Failed to register messages");

        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(
                CreateWindowExW),
            reinterpret_cast<void*>(
                HookCreateWindowExW),
            reinterpret_cast<void**>(
                &g_CreateWindowExW))) {

        Wh_Log(
            L"[SmoothDesktop] CreateWindowExW hook failed");

        return FALSE;
    }

    DiscoverExistingDesktop();


    Wh_Log(
        L"[SmoothDesktop] Init complete");

    return TRUE;
}

void Wh_ModUninit() {

    Wh_Log(
        L"[SmoothDesktop] Uninit");

    Cleanup();
}

void Wh_ModSettingsChanged() {

    Wh_Log(
        L"[SmoothDesktop] Settings changed");

    LoadSettings();

    for (int i = 0;
         i < g_stateCount;
         ++i) {

        if (g_states[i].shell) {

            PostMessageW(
                g_states[i].shell,
                g_msgRefresh,
                0,
                0);
        }
    }
}
