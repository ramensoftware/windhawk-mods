// ==WindhawkMod==
// @id              smooth-virtual-desktop-switching
// @name            Smooth Virtual Desktop Switching
// @description     Smooth native touchpad and Ctrl+Win+Arrow virtual desktop transitions.
// @version         1.0.0
// @author          enesky
// @github          https://github.com/enesky
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smooth Virtual Desktop Switching

Adjusts the animation that finishes a native three- or four-finger touchpad
swipe between virtual desktops after you lift your fingers. Ctrl+Win+Left/Right
Arrow switches have a separately adjustable duration.

## Settings

- **Touchpad transition duration (ms):** 750 by default, clamped to 150–5000.
- **Keyboard transition duration (ms):** 750 by default, clamped to 150–5000.
- **Transition curve:** Native Windows (default), Linear, or Ease-out (0.22, 1, 0.36, 1).
- **Animate even when Windows animation effects are off:** enabled by default
  to preserve the mod's original behavior. Turn this off to respect Windows'
  animation preference. The system setting itself is never changed.

Windows' gesture direction, finger tracking, and desktop selection remain native.
Touchscreen swipes and taskbar visibility are unchanged. Use Windhawk's per-mod
logging toggle for diagnostics.

Keyboard hooks are optional: missing keyboard symbols leave touchpad support
available. Keyboard animation preference overrides require both keyboard hooks.
Long transitions may be interrupted by another native desktop switch. Windows
controls interruption and transition delays; the mod does not queue switches.

## Compatibility

Targets Explorer on Windows 11. Tested by the author on x64 Windows 11 25H2
with 26100-family shell DLLs, including revision 9444. Other builds and ARM64
have not been tested. Required function symbols must resolve; otherwise
initialization stops. Windows updates can change the private animation behavior
and require a mod update. The first load can take time while Microsoft symbols
are downloaded.

The taskbar may disappear during the native desktop slide and return when it
finishes. This mod does not change taskbar visibility.

Do not combine with Disable Virtual Desktop Transition Animation. Disabling
this mod restores the original behavior. If the effect is missing, enable
Windhawk debug logging and check for initialization or compatibility errors.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- durationMs: 750
  $name: Touchpad transition duration (ms)
  $description: "Clamped to 150–5000 ms. Applies after lifting your fingers."
- keyboardDurationMs: 750
  $name: Keyboard transition duration (ms)
  $description: "Clamped to 150–5000 ms. Applies to Ctrl+Win+Left/Right Arrow."
- transitionCurve: native
  $name: Transition curve
  $options:
  - native: Native Windows
  - linear: Linear
  - easeOut: Ease-out
- forceAnimations: true
  $name: Animate even when Windows animation effects are off
  $description: Disable to respect the Windows animation preference.
*/
// ==/WindhawkModSettings==

#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <atomic>
#include <algorithm>
#include <uxtheme.h>


using WindowCommit = HRESULT (*)(void*, void*, unsigned, float, bool);
using AddTransition = HRESULT (*)(void*, unsigned, unsigned, TA_TIMINGFUNCTION*, void*,
                                  void*, const double*, unsigned, bool);
WindowCommit g_originalCommit;
AddTransition g_originalTransition;
std::atomic<unsigned> g_duration{750};
std::atomic<unsigned> g_keyboardDuration{750};
enum class Curve { Native, Linear, EaseOut };
std::atomic<Curve> g_curve{Curve::Native};
std::atomic<bool> g_forceAnimations{true};
bool g_keyboardHooksAvailable = false;
thread_local unsigned g_hotkeyCommitDepth = 0;
thread_local unsigned g_commitDepth = 0;
thread_local unsigned g_keyboardDepth = 0;
using HotkeyCommit = HRESULT (*)(void*, void*, float);
using CycleInDirection = HRESULT (*)(void*, int);
HotkeyCommit g_originalHotkeyCommit;
CycleInDirection g_originalCycle;
decltype(&SystemParametersInfoW) g_originalSystemParametersInfo;


using AnimationsEnabledFunction = bool (*)(void*);
AnimationsEnabledFunction g_animationsEnabledOriginal;
bool AnimationsEnabledHook(void* self) {
    bool enabled = g_animationsEnabledOriginal(self);
    if (g_forceAnimations.load() && (g_commitDepth || g_keyboardDepth) && !enabled) {
        Wh_Log(L"Allowing animation inside desktop transition");
        return true;
    }
    return enabled;
}

struct DepthScope {
    unsigned& depth;
    explicit DepthScope(unsigned& value) : depth(value) { ++depth; }
    ~DepthScope() { --depth; }
    DepthScope(const DepthScope&) = delete;
    DepthScope& operator=(const DepthScope&) = delete;
};

HRESULT CommitHook(void* self, void* handler, unsigned token,
                   float target, bool touch) {
    // FinishSwipe passes false; FinishTouchSwipe passes true.
    if (touch) return g_originalCommit(self, handler, token, target, touch);
        Wh_Log(L"gesture window commit: target=%f touch=%d token=%u", target, touch, token);
    DepthScope scope(g_commitDepth);
    return g_originalCommit(self, handler, token, target, touch);
}

HRESULT HotkeyCommitHook(void* self, void* animator, float target) {
    Wh_Log(L"keyboard window commit: target=%f", target);
    DepthScope keyboardScope(g_hotkeyCommitDepth);
    DepthScope scope(g_commitDepth);
    return g_originalHotkeyCommit(self, animator, target);
}

HRESULT CycleInDirectionHook(void* self, int direction) {
    // Windows checks SPI_GETCLIENTAREAANIMATION before choosing its animated path.
    // Keep this permission scoped separately from duration changes in Commit.
    DepthScope scope(g_keyboardDepth);
    return g_originalCycle(self, direction);
}

BOOL WINAPI SystemParametersInfoHook(UINT action, UINT param, PVOID value, UINT flags) {
    if (g_keyboardHooksAvailable && g_forceAnimations.load() && g_keyboardDepth && action == SPI_GETCLIENTAREAANIMATION && value) {
        *static_cast<BOOL*>(value) = TRUE;
        return TRUE;
    }
    return g_originalSystemParametersInfo(action, param, value, flags);
}

HRESULT TransitionHook(void* self, unsigned delayMs, unsigned durationMs,
                       TA_TIMINGFUNCTION* timing, void* storyboard, void* variable,
                       const double* values, unsigned count, bool force) {
    static TA_CUBIC_BEZIER ease{{TTFT_CUBIC_BEZIER}, .22f, 1.f, .36f, 1.f};
    static TA_CUBIC_BEZIER linear{{TTFT_CUBIC_BEZIER}, 1.f/3.f, 1.f/3.f, 2.f/3.f, 2.f/3.f};
    // Only animated cubic transitions within a gesture or hotkey commit are changed.
    // Instantaneous updates during dragging stay untouched.
    if (g_commitDepth && durationMs && timing &&
        timing->eTimingFunctionType == TTFT_CUBIC_BEZIER) {
        unsigned chosen = g_hotkeyCommitDepth ? g_keyboardDuration.load() : g_duration.load();
                Wh_Log(L"settle transition: delay=%u native=%u requested=%u dimensions=%u",
                   delayMs, durationMs, chosen, count);
        durationMs = chosen;
        if (g_curve.load() == Curve::Linear) timing = &linear.header;
        else if (g_curve.load() == Curve::EaseOut) timing = &ease.header;
    }
    return g_originalTransition(self, delayMs, durationMs, timing, storyboard,
                                variable, values, count, force);
}

void LoadSettings() {
    g_duration = std::clamp(Wh_GetIntSetting(L"durationMs"), 150, 5000);
    g_keyboardDuration = std::clamp(Wh_GetIntSetting(L"keyboardDurationMs"), 150, 5000);
    PCWSTR curve = Wh_GetStringSetting(L"transitionCurve");
    g_curve = wcscmp(curve, L"linear") == 0 ? Curve::Linear :
              wcscmp(curve, L"easeOut") == 0 ? Curve::EaseOut : Curve::Native;
    Wh_FreeStringSetting(curve);
    g_forceAnimations = Wh_GetIntSetting(L"forceAnimations") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"v1.0.0 initialization entered");
    LoadSettings();
    HMODULE shell = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE thumbnails = LoadLibraryExW(L"twinui.dll", nullptr,
                                        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!shell || !thumbnails) {
        Wh_Log(L"Failed to load required shell modules");
        return FALSE;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK shellHooks[] = {
        {{
             L"public: long __cdecl VirtualDesktopGestureWindow::Commit(struct IVirtualDesktopGestureHandlerPrivate *,unsigned int,float,bool)",
         },
         &g_originalCommit,
         CommitHook},
        {{
             L"public: long __cdecl VirtualDesktopHotKeyWindow::Commit(struct IVirtualDesktopSwitchAnimator2 *,float)",
         },
         &g_originalHotkeyCommit,
         HotkeyCommitHook, true},
        {{
             L"private: long __cdecl CVirtualDesktopHotkeyHandler::_CycleInDirection(enum VirtualDesktopSwitchDirection)",
         },
         &g_originalCycle,
         CycleInDirectionHook, true},
    };
    WindhawkUtils::SYMBOL_HOOK twinuiDllHooks[] = {
        {{
             L"private: long __cdecl CDCompAbstractThumbnail::_AddTransition(unsigned int,unsigned int,struct TA_TIMINGFUNCTION *,struct IUIAnimationStoryboard2 *,struct IUIAnimationVariable2 *,double * const,unsigned int,bool)",
         },
         &g_originalTransition,
         TransitionHook},
        {{
             L"public: virtual bool __cdecl CSwitchThumbnailDeviceManager::AnimationsEnabled(void)",
         },
         &g_animationsEnabledOriginal,
         AnimationsEnabledHook},
    };
    if (!WindhawkUtils::HookSymbols(shell, shellHooks, ARRAYSIZE(shellHooks)) ||
        !WindhawkUtils::HookSymbols(thumbnails, twinuiDllHooks, ARRAYSIZE(twinuiDllHooks))) {
        Wh_Log(L"Failed to hook required symbols; initialization aborted.");
        return FALSE;
    }
    g_keyboardHooksAvailable = g_originalHotkeyCommit && g_originalCycle;
    if (!g_keyboardHooksAvailable) {
        Wh_Log(L"Keyboard symbols unavailable; touchpad support remains active");
    }
    if (g_keyboardHooksAvailable && !WindhawkUtils::SetFunctionHook(SystemParametersInfoW,
                                        SystemParametersInfoHook,
                                        &g_originalSystemParametersInfo)) {
        Wh_Log(L"Failed to hook the desktop hotkey animation preference query");
        g_keyboardHooksAvailable = false;
    }
    Wh_Log(L"Smooth release initialized; duration=%u ms. Ready.", g_duration.load());
    return TRUE;
}

void Wh_ModSettingsChanged() { LoadSettings(); }
