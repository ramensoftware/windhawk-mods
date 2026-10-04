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
swipe between virtual desktops after you lift your fingers. The same duration
and curve also apply to Ctrl+Win+Left/Right Arrow desktop switches.

## Settings

- **Release animation duration:** 750 ms by default, adjustable from 150 to 5000 ms.
- **Constant-speed release:** a linear curve. Off by default.
- **Use custom ease-out curve:** uses (0.22, 1, 0.36, 1) when constant speed is off.
  Both options off preserves Windows' original curve.
- **Debug logging:** optional Windhawk logs; no files are written.

Windows' gesture direction, finger tracking, and desktop selection threshold
remain native. Keyboard shortcuts keep their native desktop selection; touchscreen
swipes are not changed.
The mod permits thumbnail animations during the release even if Windows'
client-area animation preference is off; it does not change that system setting.
For native keyboard cycling, the client-area animation query is overridden only
on the calling thread while the desktop hotkey handler runs.

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
  $name: Release animation duration (ms)
  $description: "Clamped to 150–5000 ms. Default: 750 ms; 2000 gives a two-second release."
- linearRelease: false
  $name: Constant-speed release
  $description: Use a linear curve to make the full duration visible.
- easeOut: false
  $name: Use custom ease-out curve
  $description: Off preserves the Windows curve; on uses (0.22, 1, 0.36, 1).
- debugLogging: false
  $name: Log gesture and transition calls
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
std::atomic<bool> g_easeOut{false}, g_logging{false};
std::atomic<bool> g_linear{false};
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
    if ((g_commitDepth || g_keyboardDepth) && !enabled) {
        if (g_logging.load()) Wh_Log(L"Allowing animation inside desktop transition");
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
    if (g_logging.load())
        Wh_Log(L"gesture window commit: target=%f touch=%d token=%u", target, touch, token);
    DepthScope scope(g_commitDepth);
    return g_originalCommit(self, handler, token, target, touch);
}

HRESULT HotkeyCommitHook(void* self, void* animator, float target) {
    if (g_logging.load()) Wh_Log(L"keyboard window commit: target=%f", target);
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
    if (g_keyboardDepth && action == SPI_GETCLIENTAREAANIMATION && value) {
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
        unsigned chosen = g_duration.load();
        if (g_logging.load())
            Wh_Log(L"settle transition: delay=%u native=%u requested=%u dimensions=%u",
                   delayMs, durationMs, chosen, count);
        durationMs = chosen;
        if (g_linear.load()) timing = &linear.header;
        else if (g_easeOut.load()) timing = &ease.header;
    }
    return g_originalTransition(self, delayMs, durationMs, timing, storyboard,
                                variable, values, count, force);
}

void LoadSettings() {
    g_duration = std::clamp(Wh_GetIntSetting(L"durationMs"), 150, 5000);
    g_linear = Wh_GetIntSetting(L"linearRelease") != 0;
    g_easeOut = Wh_GetIntSetting(L"easeOut") != 0;
    g_logging = Wh_GetIntSetting(L"debugLogging") != 0;
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
         HotkeyCommitHook},
        {{
             L"private: long __cdecl CVirtualDesktopHotkeyHandler::_CycleInDirection(enum VirtualDesktopSwitchDirection)",
         },
         &g_originalCycle,
         CycleInDirectionHook},
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
    if (!WindhawkUtils::SetFunctionHook(SystemParametersInfoW,
                                        SystemParametersInfoHook,
                                        &g_originalSystemParametersInfo)) {
        Wh_Log(L"Failed to hook the desktop hotkey animation preference query");
        return FALSE;
    }
    Wh_Log(L"Smooth release initialized; duration=%u ms. Ready.", g_duration.load());
    return TRUE;
}

void Wh_ModSettingsChanged() { LoadSettings(); }
void Wh_ModUninit() { Wh_Log(L"Smooth release unloaded; native behavior restored."); }
