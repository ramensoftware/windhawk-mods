// ==WindhawkMod==
// @id              taskbar-auto-hide-speed
// @name            Taskbar auto-hide speed
// @description     Customize the taskbar auto-hide animation speed and frame rate to make it feel less sluggish and janky
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar auto-hide speed

Customize the taskbar auto-hide animation speed and frame rate to make it feel
less sluggish and janky.

![Demonstration](https://i.imgur.com/x7pg5xX.gif)

## Smoothness options

* **Sync to display refresh rate**: steps the slide once per screen refresh
  (via `DwmFlush`) instead of waiting a fixed time between frames. Timed frames
  drift against the display's refresh, so steps land unevenly on screen; with
  this option the slide runs at the monitor's refresh rate (144/165/240 Hz).
* **Show/hide curve**: applies an easing curve to the slide. For a macOS-like
  feel, try "Ease-out strong" for show and "Ease-in-out" for hide with a
  speedup of 100-120%.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showSpeedup: 250
  $name: Show animation speedup
  $description: In percentage of the original speed.
- hideSpeedup: 250
  $name: Hide animation speedup
  $description: In percentage of the original speed.
- showEasing: linear
  $name: Show animation curve
  $description: Has no effect on the new Windows 11 auto-hide animation.
  $options:
  - linear: Linear (default)
  - easeOutCubic: Ease-out
  - easeOutQuint: Ease-out strong
  - easeInOutCubic: Ease-in-out
- hideEasing: sameAsShow
  $name: Hide animation curve
  $description: Has no effect on the new Windows 11 auto-hide animation.
  $options:
  - sameAsShow: Same as show animation curve
  - linear: Linear
  - easeOutCubic: Ease-out
  - easeOutQuint: Ease-out strong
  - easeInOutCubic: Ease-in-out
- vsync: true
  $name: Sync to display refresh rate
  $description: >-
    Step the animation once per screen refresh for the smoothest motion. When
    disabled, the frame rate below is used. Has no effect on the new Windows 11
    auto-hide animation.
- frameRate: 90
  $name: Animation frame rate
  $description: >-
    Frames per second, used when syncing to the display refresh rate is
    disabled. Higher frame rate will use more CPU. Has no effect on the new
    Windows 11 auto-hide animation.
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <winrt/Windows.UI.Composition.h>

#include <dwmapi.h>
#include <psapi.h>

#include <atomic>
#include <cmath>

enum class Easing {
    Linear,
    EaseOutCubic,
    EaseOutQuint,
    EaseInOutCubic,
};

struct {
    int showSpeedup;
    int hideSpeedup;
    Easing showEasing;
    Easing hideEasing;
    bool vsync;
    int frameRate;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

double g_recipCyclesPerSecond;
HANDLE g_frameTimer;

std::atomic<DWORD> g_slideWindowThreadId;
int g_slideWindowSpeedup;
bool g_slideWindowShow;
double g_slideWindowStartTime;
double g_slideWindowLastFrameStartTime;
double g_slideWindowLastReturnedMs;

[[clang::no_destroy]] winrt::Windows::UI::Composition::KeyFrameAnimation
    g_collapseAnimation = nullptr;
winrt::Windows::Foundation::TimeSpan g_collapseOriginalDuration;
[[clang::no_destroy]] winrt::Windows::UI::Composition::KeyFrameAnimation
    g_expandAnimation = nullptr;
winrt::Windows::Foundation::TimeSpan g_expandOriginalDuration;

void SetAnimationDuration(
    winrt::Windows::UI::Composition::KeyFrameAnimation animation,
    winrt::Windows::Foundation::TimeSpan duration) {
    // From the docs: Minimum allowed value is 1ms and maximum allowed value is
    // 24 days.
    constexpr auto kMin = duration_cast<winrt::Windows::Foundation::TimeSpan>(
        std::chrono::milliseconds{1});
    constexpr auto kMax = duration_cast<winrt::Windows::Foundation::TimeSpan>(
        std::chrono::hours{24 * 24});
    if (duration < kMin) {
        duration = kMin;
    } else if (duration > kMax) {
        duration = kMax;
    }
    animation.Duration(duration);
}

using SharedAnimations_TaskbarCollapseAnimation_t =
    void**(WINAPI*)(void* pThis, void* param1);
SharedAnimations_TaskbarCollapseAnimation_t
    SharedAnimations_TaskbarCollapseAnimation_Original;
void** WINAPI SharedAnimations_TaskbarCollapseAnimation_Hook(void* pThis,
                                                             void* param1) {
    Wh_Log(L">");

    void** ret =
        SharedAnimations_TaskbarCollapseAnimation_Original(pThis, param1);

    winrt::Windows::UI::Composition::KeyFrameAnimation animation = nullptr;
    winrt::copy_from_abi(animation, ret[0]);

    if (!g_collapseAnimation) {
        g_collapseOriginalDuration = animation.Duration();
        g_collapseAnimation = animation;
        SetAnimationDuration(animation, g_collapseOriginalDuration * 100 /
                                            g_settings.hideSpeedup);
    }

    return ret;
}

using SharedAnimations_TaskbarExpandAnimation_t = void**(WINAPI*)(void* pThis,
                                                                  void* param1);
SharedAnimations_TaskbarExpandAnimation_t
    SharedAnimations_TaskbarExpandAnimation_Original;
void** WINAPI SharedAnimations_TaskbarExpandAnimation_Hook(void* pThis,
                                                           void* param1) {
    Wh_Log(L">");

    void** ret =
        SharedAnimations_TaskbarExpandAnimation_Original(pThis, param1);

    winrt::Windows::UI::Composition::KeyFrameAnimation animation = nullptr;
    winrt::copy_from_abi(animation, ret[0]);

    if (!g_expandAnimation) {
        g_expandOriginalDuration = animation.Duration();
        g_expandAnimation = animation;
        SetAnimationDuration(
            animation, g_expandOriginalDuration * 100 / g_settings.showSpeedup);
    }

    return ret;
}

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double cyclesPerSecond = static_cast<double>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

double TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<double>(T1.QuadPart);
}

double TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

// Unlike Sleep, which is rounded up to the system timer tick (15.6 ms by
// default), a high resolution timer wakes close to the requested time.
bool TimerWait(double ms) {
    if (!g_frameTimer) {
        return false;
    }

    // Negative for a relative time, in 100 nanosecond units.
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -static_cast<LONGLONG>(ms * 10000.0);
    if (!SetWaitableTimer(g_frameTimer, &dueTime, 0, nullptr, nullptr, FALSE)) {
        return false;
    }

    WaitForSingleObject(g_frameTimer, INFINITE);
    return true;
}

Easing ParseEasing(PCWSTR name) {
    if (wcscmp(name, L"easeOutCubic") == 0) {
        return Easing::EaseOutCubic;
    }
    if (wcscmp(name, L"easeOutQuint") == 0) {
        return Easing::EaseOutQuint;
    }
    if (wcscmp(name, L"easeInOutCubic") == 0) {
        return Easing::EaseInOutCubic;
    }
    return Easing::Linear;
}

double EaseCurve(Easing easing, double x) {
    switch (easing) {
        case Easing::EaseOutCubic:
            return 1.0 - std::pow(1.0 - x, 3.0);
        case Easing::EaseOutQuint:
            return 1.0 - std::pow(1.0 - x, 5.0);
        case Easing::EaseInOutCubic:
            return x < 0.5 ? 4.0 * x * x * x
                           : 1.0 - std::pow(-2.0 * x + 2.0, 3.0) / 2.0;
        case Easing::Linear:
            break;
    }
    return x;
}

// Length of the stock slide in the stock animation's own milliseconds, index 0
// for hide and 1 for show. Starts at TrayUI's constants and is lowered by
// observed slides: the stock loop exits on the first frame whose GetTickCount
// value reaches the duration, so the minimum observation converges on it.
double g_learnedSlideDuration[2] = {400, 200};

double ApplyEasing(double ms) {
    Easing easing =
        g_slideWindowShow ? g_settings.showEasing : g_settings.hideEasing;
    double duration = g_learnedSlideDuration[g_slideWindowShow];
    if (easing == Easing::Linear || ms <= 0 || ms >= duration) {
        // Pass the time through unchanged outside the learned span, so that the
        // stock loop always reaches its end. Since ease(1) == 1, the mapping
        // stays continuous and monotonic.
        return ms;
    }

    return EaseCurve(easing, ms / duration) * duration;
}

void LearnSlideDuration() {
    double observed = g_slideWindowLastReturnedMs;
    double& learned = g_learnedSlideDuration[g_slideWindowShow];
    if (observed >= 30.0 && observed < learned) {
        learned = observed;
    }
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rect,
                                           HMONITOR monitor,
                                           bool show,
                                           bool animate);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rect,
                                    HMONITOR monitor,
                                    bool show,
                                    bool animate) {
    Wh_Log(L">");

    g_slideWindowSpeedup =
        show ? g_settings.showSpeedup : g_settings.hideSpeedup;
    g_slideWindowStartTime = TimerGetSeconds();
    g_slideWindowLastFrameStartTime = g_slideWindowStartTime;
    g_slideWindowShow = show;
    g_slideWindowLastReturnedMs = 0;
    g_slideWindowThreadId = GetCurrentThreadId();

    TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);

    g_slideWindowThreadId = 0;

    if (animate) {
        LearnSlideDuration();
    }
}

using GetTickCount_t = decltype(&GetTickCount);
GetTickCount_t GetTickCount_Original;
DWORD WINAPI GetTickCount_Hook() {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        return GetTickCount_Original();
    }

    double elapsedMs =
        (g_slideWindowLastFrameStartTime - g_slideWindowStartTime) * 1000.0 *
        (g_slideWindowSpeedup / 100.0);
    double easedMs = ApplyEasing(elapsedMs);
    g_slideWindowLastReturnedMs = easedMs;
    DWORD ms = easedMs + 0.5;

    Wh_Log(L"> %u", ms);

    return ms;
}

using Sleep_t = decltype(&Sleep);
Sleep_t Sleep_Original;
void WINAPI Sleep_Hook(DWORD dwMilliseconds) {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        Sleep_Original(dwMilliseconds);
        return;
    }

    // Wait for the next DWM composition, i.e. the next display refresh, so that
    // each step lands on its own frame. Fall back to timed pacing on failure.
    if (g_settings.vsync && SUCCEEDED(DwmFlush())) {
        g_slideWindowLastFrameStartTime = TimerGetSeconds();
        return;
    }

    double frameTotalTime = 1000.0 / g_settings.frameRate;

    double frameElapsedTime =
        (TimerGetSeconds() - g_slideWindowLastFrameStartTime) * 1000.0;

    double sleepTime = frameTotalTime - frameElapsedTime;

    Wh_Log(L"> %f - %f = %f", frameTotalTime, frameElapsedTime, sleepTime);

    if (sleepTime > 0 && !TimerWait(sleepTime)) {
        Sleep_Original(sleepTime + 0.5);
    }

    g_slideWindowLastFrameStartTime = TimerGetSeconds();
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: struct std::pair<struct winrt::Windows::UI::Composition::KeyFrameAnimation,struct winrt::WindowsUdk::UI::Composition::AnimationLastFrameTelemetry> __cdecl winrt::Taskbar::implementation::SharedAnimations::TaskbarCollapseAnimation(void)const )"},
            &SharedAnimations_TaskbarCollapseAnimation_Original,
            SharedAnimations_TaskbarCollapseAnimation_Hook,
            true,  // For the new Windows 11 animation.
        },
        {
            {LR"(public: struct std::pair<struct winrt::Windows::UI::Composition::KeyFrameAnimation,struct winrt::WindowsUdk::UI::Composition::AnimationLastFrameTelemetry> __cdecl winrt::Taskbar::implementation::SharedAnimations::TaskbarExpandAnimation(void)const )"},
            &SharedAnimations_TaskbarExpandAnimation_Original,
            SharedAnimations_TaskbarExpandAnimation_Hook,
            true,  // For the new Windows 11 animation.
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion(bool* hasWin10Taskbar) {
    *hasWin10Taskbar = false;

    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            // explorer.exe includes the Win10 taskbar before this build.
            *hasWin10Taskbar = build < 26100 || (build == 26100 && qfe < 1301);
            if (build < 22000) {
                return WinVersion::Win10;
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    g_settings.showSpeedup = std::max(1, Wh_GetIntSetting(L"showSpeedup"));
    g_settings.hideSpeedup = std::max(1, Wh_GetIntSetting(L"hideSpeedup"));

    PCWSTR easing = Wh_GetStringSetting(L"showEasing");
    g_settings.showEasing = ParseEasing(easing);
    Wh_FreeStringSetting(easing);

    easing = Wh_GetStringSetting(L"hideEasing");
    g_settings.hideEasing = wcscmp(easing, L"sameAsShow") == 0
                                ? g_settings.showEasing
                                : ParseEasing(easing);
    Wh_FreeStringSetting(easing);

    g_settings.vsync = Wh_GetIntSetting(L"vsync");
    g_settings.frameRate = std::max(1, Wh_GetIntSetting(L"frameRate"));
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    bool hasWin10Taskbar;
    g_winVersion = GetExplorerVersion(&hasWin10Taskbar);
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (g_winVersion >= WinVersion::Win11) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                return FALSE;
            }
        } else {
            Wh_Log(L"Taskbar view module not loaded yet");
        }

        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    } else {
        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    auto pKernelBaseGetTickCount = (decltype(&GetTickCount))GetProcAddress(
        kernelBaseModule, "GetTickCount");
    WindhawkUtils::SetFunctionHook(pKernelBaseGetTickCount, GetTickCount_Hook,
                                   &GetTickCount_Original);

    auto pKernelBaseSleep =
        (decltype(&Sleep))GetProcAddress(kernelBaseModule, "Sleep");
    WindhawkUtils::SetFunctionHook(pKernelBaseSleep, Sleep_Hook,
                                   &Sleep_Original);

    TimerInitialize();

    // Requires Windows 10 version 1803, Sleep is used otherwise.
    g_frameTimer = CreateWaitableTimerEx(nullptr, nullptr,
                                         CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                         TIMER_ALL_ACCESS);
    if (!g_frameTimer) {
        Wh_Log(L"CreateWaitableTimerEx failed: %u", GetLastError());
    }

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_collapseAnimation) {
        g_collapseAnimation.Duration(g_collapseOriginalDuration);
        g_collapseAnimation = nullptr;
    }

    if (g_expandAnimation) {
        g_expandAnimation.Duration(g_expandOriginalDuration);
        g_expandAnimation = nullptr;
    }

    if (g_frameTimer) {
        CloseHandle(g_frameTimer);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_collapseAnimation) {
        SetAnimationDuration(
            g_collapseAnimation,
            g_collapseOriginalDuration * 100 / g_settings.hideSpeedup);
    }

    if (g_expandAnimation) {
        SetAnimationDuration(g_expandAnimation, g_expandOriginalDuration * 100 /
                                                    g_settings.showSpeedup);
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
