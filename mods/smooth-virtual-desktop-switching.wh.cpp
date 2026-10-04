// ==WindhawkMod==
// @id              smooth-virtual-desktop-switching
// @name            Smooth Virtual Desktop Switching
// @description     Adjustable settling duration for native touchpad desktop swipes.
// @version         1.0.0
// @author          enesky
// @github          https://github.com/enesky
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smooth Virtual Desktop Switching

Adjusts the animation that finishes a native three- or four-finger touchpad
swipe between virtual desktops after you lift your fingers.

## Settings

- **Release animation duration:** 750 ms by default, adjustable from 150 to 5000 ms.
- **Constant-speed release:** a linear curve. Off by default.
- **Use custom ease-out curve:** uses (0.22, 1, 0.36, 1) when constant speed is off.
  Both options off preserves Windows' original curve.
- **Debug logging:** optional Windhawk logs; no files are written.

Windows' gesture direction, finger tracking, and desktop selection threshold
remain native. Keyboard shortcuts and touchscreen swipes are not changed.
The mod permits thumbnail animations during the release even if Windows'
client-area animation preference is off; it does not change that system setting.

## Compatibility

Targets Explorer on x64 Windows 11. Tested by the author on Windows 11 25H2
with 26100-family shell DLLs, including revision 9444. Other versions are not
claimed to be supported. Required symbols and inspected function-entry code
must match; otherwise initialization stops. A Windows update can require an update
to this mod. The first load can take time while Microsoft symbols are downloaded.

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
  $description: Clamped to 150–5000 ms. Default: 750 ms; 2000 gives a two-second release.
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

#include <windows.h>
#include <windhawk_api.h>
#include <atomic>
#include <algorithm>
#include <cwchar>
#include <string>
#include <vector>
#include <cstring>


// ABI of TA_TIMINGFUNCTION_CUBICBEZIER verified from twinui::_AddTransition:
// type at +0, control points at +4, +8, +12, +16. Type 1 is cubic Bezier.
struct TimingCurve { unsigned type; float x1, y1, x2, y2; };
static_assert(sizeof(TimingCurve) == 20);
using WindowCommit = HRESULT (*)(void*, void*, unsigned, float, bool);
using AddTransition = HRESULT (*)(void*, unsigned, unsigned, void*, void*,
                                  void*, const double*, unsigned, bool);
WindowCommit g_originalCommit;
AddTransition g_originalTransition;
std::atomic<unsigned> g_duration{750};
std::atomic<bool> g_easeOut{false}, g_logging{false};
std::atomic<bool> g_linear{false};
thread_local unsigned g_commitDepth = 0;


using AnimationsEnabledFunction = bool (*)(void*);
AnimationsEnabledFunction g_animationsEnabledOriginal;
bool AnimationsEnabledHook(void* self) {
    bool enabled = g_animationsEnabledOriginal(self);
    if (g_commitDepth && !enabled) {
        if (g_logging.load()) Wh_Log(L"Allowing animation inside trackpad release");
        return true;
    }
    return enabled;
}

struct CommitScope {
    CommitScope() { ++g_commitDepth; }
    ~CommitScope() { --g_commitDepth; }
};

HRESULT CommitHook(void* self, void* handler, unsigned token,
                   float target, bool touch) {
    // FinishSwipe passes false; FinishTouchSwipe passes true.
    if (touch) return g_originalCommit(self, handler, token, target, touch);
    if (g_logging.load())
        Wh_Log(L"gesture window commit: target=%f touch=%d token=%u", target, touch, token);
    CommitScope scope;
    HRESULT result = g_originalCommit(self, handler, token, target, touch);
    return result;
}

HRESULT TransitionHook(void* self, unsigned delayMs, unsigned durationMs,
                       void* timing, void* storyboard, void* variable,
                       const double* values, unsigned count, bool force) {
    TimingCurve ease{1, .22f, 1.f, .36f, 1.f};
    TimingCurve linear{1, 1.f/3.f, 1.f/3.f, 2.f/3.f, 2.f/3.f};
    // Only animated cubic transitions within a gesture commit are changed.
    // Instantaneous updates during dragging stay untouched.
    if (g_commitDepth && durationMs && timing &&
        *static_cast<const unsigned*>(timing) == 1) {
        unsigned chosen = g_duration.load();
        if (g_logging.load())
            Wh_Log(L"settle transition: delay=%u native=%u requested=%u dimensions=%u",
                   delayMs, durationMs, chosen, count);
        durationMs = chosen;
        if (g_linear.load()) timing = &linear;
        else if (g_easeOut.load()) timing = &ease;
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

bool CheckVersion(HMODULE module) {
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(module, path, MAX_PATH)) return false;
    DWORD size = GetFileVersionInfoSizeW(path, nullptr);
    if (!size) return false;
    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(path, 0, size, data.data())) return false;
    VS_FIXEDFILEINFO* info = nullptr; UINT len = 0;
    if (!VerQueryValueW(data.data(), L"\\", reinterpret_cast<void**>(&info), &len)
        || len < sizeof(*info)) return false;
    Wh_Log(L"DLL version: %u.%u.%u.%u", HIWORD(info->dwFileVersionMS),
           LOWORD(info->dwFileVersionMS), HIWORD(info->dwFileVersionLS),
           LOWORD(info->dwFileVersionLS));
    return info->dwFileVersionMS == static_cast<DWORD>(MAKELONG(0, 10)) &&
           HIWORD(info->dwFileVersionLS) == 26100;
}

// Resolve the exact inspected signatures without depending on pointer-spacing
// differences between symbol providers. Reject ambiguous matches.
void* FindUnique(HMODULE module, const wchar_t* needle) {
    WH_FIND_SYMBOL symbol{};
    HANDLE search = Wh_FindFirstSymbol(module, nullptr, &symbol);
    if (!search) { Wh_Log(L"Symbol enumeration failed, error=%lu", GetLastError()); return nullptr; }
    void* found = nullptr; unsigned count = 0;
    do {
        if (!symbol.symbol) continue;
        std::wstring normalized(symbol.symbol);
        size_t p;
        while ((p = normalized.find(L"__ptr64")) != std::wstring::npos)
            normalized.erase(p, 7);
        normalized.erase(std::remove_if(normalized.begin(), normalized.end(),
            [](wchar_t ch) { return ch == L' ' || ch == L'\t'; }), normalized.end());
        if (normalized.find(needle) != std::wstring::npos &&
            normalized.find(L"dtor$") == std::wstring::npos &&
            normalized.find(L"`") == std::wstring::npos) {
            if (found != symbol.address) { found = symbol.address; ++count; }
        }
    } while (Wh_FindNextSymbol(search, &symbol));
    Wh_FindCloseSymbol(search);
    if (count != 1) { Wh_Log(L"symbol match count=%u for %s", count, needle); return nullptr; }
    Wh_Log(L"Resolved symbol: %s", needle);
    return found;
}


// Fail closed when the inspected register/stack entry layouts change.
// These guards are compatibility checks, not proof of the complete animation path.
bool CheckEntryCode(void* commit, void* transition) {
    const BYTE commitPrefix[] = {0x48,0x8b,0xc4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x55,0x41,0x56,0x41,0x57,0x48,0x8d,0x68,0xa9,0x48,0x81,0xec,0xb0,0,0,0,0x0f,0x29,0x78,0xd8};
    const BYTE transitionPrefix[] = {0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8d,0x68,0xb9,0x48,0x81,0xec,0xc8,0,0,0,0x0f,0x29,0x70,0xa8,0x44,0x0f,0x29,0x40,0x98};
    const BYTE commitArguments[] = {0x0f,0x28,0xfb,0x41,0x8b,0xd8,0x48,0x8b,0xf2,0x4c,0x8b,0xf1};
    const BYTE transitionArguments[] = {0x4d,0x8b,0xe1,0x41,0x8b,0xd8,0x89,0x5d,0x97,0x8b,0xf2,0x89,0x55,0x9f};
    const BYTE* c = static_cast<const BYTE*>(commit);
    const BYTE* a = static_cast<const BYTE*>(transition);
    bool valid = !std::memcmp(c, commitPrefix, sizeof(commitPrefix)) &&
                 !std::memcmp(a, transitionPrefix, sizeof(transitionPrefix)) &&
                 !std::memcmp(c+45, commitArguments, sizeof(commitArguments)) &&
                 !std::memcmp(a+49, transitionArguments, sizeof(transitionArguments));
    Wh_Log(L"Entry-code compatibility: %s", valid ? L"matched" : L"FAILED; no hooks installed");
    return valid;
}

BOOL Wh_ModInit() {
    Wh_Log(L"v1.0.0 initialization entered");
    LoadSettings();
    HMODULE shell = GetModuleHandleW(L"twinui.pcshell.dll");
    HMODULE thumbnails = GetModuleHandleW(L"twinui.dll");
    if (!shell || !thumbnails) { Wh_Log(L"Required shell modules not loaded; reload mod after Explorer is ready."); return FALSE; }
    if (!CheckVersion(shell) || !CheckVersion(thumbnails)) {
        Wh_Log(L"Unsupported DLL version. No hooks installed."); return FALSE;
    }
    void* commit = FindUnique(shell,
        L"VirtualDesktopGestureWindow::Commit(structIVirtualDesktopGestureHandlerPrivate*,unsignedint,float,bool)");
    void* transition = FindUnique(thumbnails,
        L"CDCompAbstractThumbnail::_AddTransition(unsignedint,unsignedint,structTA_TIMINGFUNCTION*");
    void* animationPolicy=FindUnique(thumbnails,L"CSwitchThumbnailDeviceManager::AnimationsEnabled(void)");
    if (!commit || !transition || !animationPolicy) { Wh_Log(L"Required symbols missing. No hooks installed."); return FALSE; }
    if (!CheckEntryCode(commit, transition)) return FALSE;
    if (!Wh_SetFunctionHook(commit, reinterpret_cast<void*>(CommitHook),
                            reinterpret_cast<void**>(&g_originalCommit)) ||
        !Wh_SetFunctionHook(transition, reinterpret_cast<void*>(TransitionHook),
                            reinterpret_cast<void**>(&g_originalTransition))) { Wh_Log(L"Hook registration failed"); return FALSE; }


    if (!animationPolicy || !Wh_SetFunctionHook(animationPolicy,
        reinterpret_cast<void*>(AnimationsEnabledHook),
        reinterpret_cast<void**>(&g_animationsEnabledOriginal))) {
        Wh_Log(L"Animation-policy hook failed; mod initialization aborted");
        return FALSE;
    }
    Wh_Log(L"Smooth release initialized; duration=%u ms. Ready.", g_duration.load());
    return TRUE;
}

void Wh_ModSettingsChanged() { LoadSettings(); }
void Wh_ModUninit() { Wh_Log(L"Smooth release unloaded; native behavior restored."); }


