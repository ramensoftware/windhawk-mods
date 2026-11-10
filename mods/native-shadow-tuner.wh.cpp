// ==WindhawkMod==
// @id              native-shadow-tuner
// @name            Windows Shadows Tuner
// @description     Adjust the size, blur and intensity of native Windows shadows.
// @version         0.9.4
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lwevtapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Shadows Tuner

Customize native Windows window shadows: make them lighter or more pronounced,
and adjust their size and blur.

## Examples

**Shadow intensity and size comparison**

![Windows Shadows TUNER comparison](https://i.imgur.com/epfTlMZ.png)

**Dynamic shadows**

![Dynamic shadows demo](https://i.imgur.com/x83s8F1.gif)

## Setup

`dwm.exe` is a critical system process and Windhawk doesn't inject mods into it
by default. Open Windhawk Settings, go to **Advanced settings**, then
**More advanced settings**, and add `dwm.exe` to the process inclusion list.

![Windhawk advanced settings](https://i.imgur.com/LRhREtJ.png)

## Controls

- **Intensity**: controls the opacity of the native shadow. The default value is
  180%. Set it to 100% to restore the original Windows intensity, 50% for a
  lighter shadow, or 0% to make it invisible.
- **Size and blur**: controls the shadow spread and blur. The default value is
  150%. Set it to 100% to restore the original Windows size.
- **Dynamic shadows (experimental)**: simulates a light source at a fixed point
  of the screen. Each window's shadow fades gradually as the window moves away
  from the light, and updates live while the window is dragged.
  - **Light position**: where the light is, relative to the monitor the
    window is on. A corner or center point fades shadows by distance from
    that point. An edge (top, bottom, left or right) lights the whole side of
    the screen and fades shadows only as windows move away from it.
  - **Dimming strength**: how much the shadow fades at the fade distance.
    100% makes it invisible.
  - **Fade distance**: how far from the light the shadow reaches its minimum,
    as a percentage of the screen diagonal for point lights, or of the screen
    height (top and bottom edges) or width (left and right edges) for edge
    lights. Lower values give a shorter, more pronounced fade.
  - **Fade curve**: how the shadow fades along that distance.

  Dynamic shadows can only make shadows lighter. To get a strong contrast, raise
  **Intensity** and let the distance from the light fade it.

When both controls are set to 100% and dynamic shadows are off, the mod doesn't
install any hooks because the requested appearance is identical to the
original Windows appearance.

## How it works

The mod hooks `CWindowBorder::GetShadowParameters` and scales the radius and
alpha values it produces. To apply changes to windows that are already open
and restore the original shadows when the mod is disabled, the mod briefly
toggles the system drop-shadow setting off and on in memory so DWM rebuilds
every window's shadow. The user profile isn't modified and no settings-change
message is broadcast. Shadows can blink for a fraction of a second when the
mod is loaded, unloaded, or its settings are changed.

Dynamic shadows hook `CTopLevelWindow::OnOffsetUpdated`, which DWM calls every
time a window's position changes, and `CTopLevelWindow::UpdateWindowVisuals`.
It sets the opacity of each window's shadow sprite through the public
`Windows.UI.Composition.IVisual` interface. The shadow sprite also draws the
thin window border, so the border fades together with the shadow. Each
monitor has its own light, so a window's shadow can change abruptly when the
window's center crosses to another monitor. While "Show shadows under windows" is off
in Windows, dynamic shadows don't change anything.

The mod creates no overlay windows. The only value it stores is the time of
its last initialization, used to detect a DWM crash loop.

## Compatibility

Tested with uDWM.dll 10.0.26100.9549 (Windows 11 24H2/25H2 servicing
branch). The uDWM symbols and field offsets used by dynamic shadows were also
verified by disassembly on uDWM.dll 10.0.26100.9278.

Dynamic shadows read four internal field offsets. At startup the mod checks
the uDWM code that uses them and, if anything doesn't match, disables only
dynamic shadows and keeps the intensity and size controls working.

Other builds haven't been tested. If the required uDWM symbol can't be
resolved, the mod logs the error and doesn't load.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacityPercent: 180
  $name: Shadow Intensity (% of the original value)
  $description: From 0 to 300. 100 original, 50 less visible, 150 more visible. 0 to make invisible.
- sizePercent: 150
  $name: Dimension and Blur (% of the original value)
  $description: From 50 to 150. 100 keeps the original value.
- dynamicShadows: false
  $name: Dynamic shadows (experimental)
  $description: Fades each window's shadow based on its distance from a virtual light source, live while dragging.
- lightPosition: topRight
  $name: Light position
  $options:
  - topEdge: Top edge
  - bottomEdge: Bottom edge
  - leftEdge: Left edge
  - rightEdge: Right edge
  - topLeft: Top left
  - topCenter: Top center
  - topRight: Top right
  - middleLeft: Middle left
  - middleRight: Middle right
  - bottomLeft: Bottom left
  - bottomCenter: Bottom center
  - bottomRight: Bottom right
- dimmingStrength: 60
  $name: Dimming strength (%)
  $description: From 0 to 100. How much a shadow fades at the fade distance. 0 no change, 100 invisible.
- fadeDistance: 100
  $name: Fade distance (%)
  $description: From 10 to 100. Distance from the light at which the shadow reaches its minimum, as a percentage of the screen diagonal for point lights, or of the screen height or width for edge lights. Lower values make the fade shorter and more pronounced; beyond this distance the shadow stays at its minimum.
- fadeCurve: linear
  $name: Fade curve
  $options:
  - linear: Linear
  - fast: Fast (fades quickly near the light)
  - smooth: Smooth (slow at both ends)
  - late: Late (stays bright, then fades)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winevt.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>

namespace {

constexpr int kOpacityMinimum = 0;
constexpr int kOpacityMaximum = 300;
constexpr int kSizeMinimum = 50;
constexpr int kSizeMaximum = 150;

using GetShadowParameters_t =
    void(__cdecl*)(int, int, float*, float*, float*, float*);
GetShadowParameters_t getShadowParameters_Original = nullptr;

float opacityScale = 1.0f;
float sizeScale = 1.0f;

// ---------------------------------------------------------------------------
// Dynamic shadows: uDWM internals
// ---------------------------------------------------------------------------
//
// Verified on uDWM.dll 10.0.26100.9278 and 10.0.26100.9549:
//
// - CTopLevelWindow::OnOffsetUpdated() is called by
//   CWindowList::OnPositionChange() whenever a window moves, and calls
//   CVisual::SetOffset() with the new position. It's the per-frame "tick"
//   while a window is dragged.
// - CTopLevelWindow + 0x2C8 holds its CWindowData, whose screen rectangle
//   is at + 0x30 (OnOffsetUpdated: mov r9, [rcx+2C8h]; mov edx, [r9+30h]).
//   CTopLevelWindow::GetActualWindowRect reads the same fields.
// - CTopLevelWindow + 0xE0 holds the window's CWindowBorder
//   (UpdateWindowVisuals: mov r14, [r15+0E0h] before SetBorderParameters).
// - CWindowBorder + 0xE8 holds the ISpriteVisual that draws the border and
//   the shadow (ValidateVisual: mov rdx, [rbx+0E8h] before
//   CreateAndAttachBorderBrush).
// - CreateAndAttachBorderBrush queries that sprite for IVisual
//   {117E202D-A859-4C89-873B-C2AA566788E3} and calls vtable slot 0xA8
//   (put_Offset) and 0x120 (put_Size), matching the public IVisual layout.
//   put_Opacity is slot 0xB8 (index 23).

constexpr size_t kTopLevelWindowDataOffset = 0x2C8;
constexpr size_t kWindowDataRectOffset = 0x30;
constexpr size_t kTopLevelBorderOffset = 0xE0;
constexpr size_t kBorderShadowSpriteOffset = 0xE8;
constexpr size_t kIVisualGetOpacityIndex = 22;
constexpr size_t kIVisualPutOpacityIndex = 23;

// Opacity changes smaller than this aren't sent to the compositor.
constexpr float kOpacityEpsilon = 0.002f;

constexpr GUID kIID_IVisual = {
    0x117e202d,
    0xa859,
    0x4c89,
    {0x87, 0x3b, 0xc2, 0xaa, 0x56, 0x67, 0x88, 0xe3}};

// mov rdx, qword ptr [rbx+0E8h]
constexpr BYTE kBorderSpritePattern[] = {0x48, 0x8B, 0x93, 0xE8,
                                         0x00, 0x00, 0x00};
// mov r14, qword ptr [r15+0E0h]
constexpr BYTE kTopLevelBorderPattern[] = {0x4D, 0x8B, 0xB7, 0xE0,
                                           0x00, 0x00, 0x00};

// mov r9, qword ptr [rcx+2C8h]
constexpr BYTE kWindowDataPattern[] = {0x4C, 0x8B, 0x89, 0xC8,
                                       0x02, 0x00, 0x00};
// mov edx, dword ptr [r9+30h]
constexpr BYTE kWindowRectPattern[] = {0x41, 0x8B, 0x51, 0x30};

using GetOpacity_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, float*);
using PutOpacity_t = HRESULT(STDMETHODCALLTYPE*)(IUnknown*, float);

using OnOffsetUpdated_t = void(__cdecl*)(void*);
OnOffsetUpdated_t onOffsetUpdated_Original = nullptr;

using UpdateWindowVisuals_t = long(__cdecl*)(void*);
UpdateWindowVisuals_t updateWindowVisuals_Original = nullptr;

// ---------------------------------------------------------------------------
// Dynamic shadows: settings and math
// ---------------------------------------------------------------------------

// A point light fades shadows by distance from a point of the monitor. An
// edge light covers a whole side of the monitor and fades shadows by distance
// from that side only.
enum class LightKind { Point, TopEdge, BottomEdge, LeftEdge, RightEdge };

struct LightPositionEntry {
    PCWSTR name;
    LightKind kind;
    float x;  // Point lights: 0 = left edge of the monitor, 1 = right edge
    float y;  // Point lights: 0 = top edge of the monitor, 1 = bottom edge
};

constexpr LightPositionEntry kLightPositions[] = {
    {L"topEdge", LightKind::TopEdge, 0.0f, 0.0f},
    {L"bottomEdge", LightKind::BottomEdge, 0.0f, 0.0f},
    {L"leftEdge", LightKind::LeftEdge, 0.0f, 0.0f},
    {L"rightEdge", LightKind::RightEdge, 0.0f, 0.0f},
    {L"topLeft", LightKind::Point, 0.0f, 0.0f},
    {L"topCenter", LightKind::Point, 0.5f, 0.0f},
    {L"topRight", LightKind::Point, 1.0f, 0.0f},
    {L"middleLeft", LightKind::Point, 0.0f, 0.5f},
    {L"middleRight", LightKind::Point, 1.0f, 0.5f},
    {L"bottomLeft", LightKind::Point, 0.0f, 1.0f},
    {L"bottomCenter", LightKind::Point, 0.5f, 1.0f},
    {L"bottomRight", LightKind::Point, 1.0f, 1.0f},
};

bool dynamicShadows = false;
LightKind lightKind = LightKind::Point;
float lightX = 1.0f;
float lightY = 0.0f;
float dimmingStrength = 0.6f;
float fadeDistance = 1.0f;

enum class FadeCurve { Linear, Fast, Smooth, Late };
FadeCurve fadeCurve = FadeCurve::Linear;

std::atomic<bool> unloading{false};

// Mirrors "Show shadows under windows". The sprite dimmed by dynamic shadows
// also draws the thin window border, so while shadows are off it's kept at
// full opacity: otherwise the border would stay dimmed after unloading,
// because the shadow refresh used to reset sprites does nothing then.
std::atomic<bool> dropShadowsOn{false};
std::atomic<int> resetCount{0};

void UpdateDropShadowsOn() {
    BOOL shadows = FALSE;
    dropShadowsOn =
        SystemParametersInfoW(SPI_GETDROPSHADOW, 0, &shadows, 0) && shadows;
}

void LoadLightPosition() {
    lightKind = LightKind::Point;
    lightX = 1.0f;
    lightY = 0.0f;

    PCWSTR value = Wh_GetStringSetting(L"lightPosition");
    for (const auto& entry : kLightPositions) {
        if (wcscmp(value, entry.name) == 0) {
            lightKind = entry.kind;
            lightX = entry.x;
            lightY = entry.y;
            break;
        }
    }
    Wh_FreeStringSetting(value);
}

void LoadFadeCurve() {
    fadeCurve = FadeCurve::Linear;

    PCWSTR value = Wh_GetStringSetting(L"fadeCurve");
    if (wcscmp(value, L"fast") == 0) {
        fadeCurve = FadeCurve::Fast;
    } else if (wcscmp(value, L"smooth") == 0) {
        fadeCurve = FadeCurve::Smooth;
    } else if (wcscmp(value, L"late") == 0) {
        fadeCurve = FadeCurve::Late;
    }
    Wh_FreeStringSetting(value);
}

// Maps 0..1 (light .. fade distance) to 0..1 (no fade .. full fade).
float ApplyFadeCurve(float t) {
    switch (fadeCurve) {
        case FadeCurve::Fast:
            return std::sqrt(t);
        case FadeCurve::Smooth:
            return t * t * (3.0f - 2.0f * t);
        case FadeCurve::Late:
            return t * t;
        case FadeCurve::Linear:
        default:
            return t;
    }
}

// 1.0 at the light, 1.0 - dimmingStrength at the fade distance and beyond.
float ComputeLightOpacity(const RECT& window) {
    HMONITOR monitor = MonitorFromRect(&window, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return 1.0f;
    }

    const RECT& area = monitorInfo.rcMonitor;
    const float width = static_cast<float>(area.right - area.left);
    const float height = static_cast<float>(area.bottom - area.top);
    if (width <= 0.0f || height <= 0.0f) {
        return 1.0f;
    }

    const float centerX = (window.left + window.right) * 0.5f;
    const float centerY = (window.top + window.bottom) * 0.5f;

    // Normalized distance from the light: 0 at the light, 1 at the opposite
    // side (edge lights) or the farthest corner (corner point lights).
    float distance;
    switch (lightKind) {
        case LightKind::TopEdge:
            distance = (centerY - area.top) / height;
            break;
        case LightKind::BottomEdge:
            distance = (area.bottom - centerY) / height;
            break;
        case LightKind::LeftEdge:
            distance = (centerX - area.left) / width;
            break;
        case LightKind::RightEdge:
            distance = (area.right - centerX) / width;
            break;
        case LightKind::Point:
        default: {
            const float dx = centerX - (area.left + lightX * width);
            const float dy = centerY - (area.top + lightY * height);
            const float diagonal = std::sqrt(width * width + height * height);
            distance = std::sqrt(dx * dx + dy * dy) / diagonal;
            break;
        }
    }

    const float t = std::clamp(distance / fadeDistance, 0.0f, 1.0f);

    return std::clamp(1.0f - dimmingStrength * ApplyFadeCurve(t), 0.0f,
                      1.0f);
}

// Runs on DWM's own thread, inside its window update path. The pointers read
// here belong to a live CTopLevelWindow: DWM is calling one of its methods.
void ApplyDynamicShadows(void* topLevel) {
    if (!dynamicShadows || !topLevel) {
        return;
    }

    BYTE* border = *reinterpret_cast<BYTE**>(static_cast<BYTE*>(topLevel) +
                                             kTopLevelBorderOffset);
    if (!border) {
        return;
    }

    IUnknown* sprite =
        *reinterpret_cast<IUnknown**>(border + kBorderShadowSpriteOffset);
    if (!sprite) {
        return;
    }

    float opacity = 1.0f;
    if (!unloading && dropShadowsOn) {
        const BYTE* windowData = *reinterpret_cast<BYTE**>(
            static_cast<BYTE*>(topLevel) + kTopLevelWindowDataOffset);
        if (!windowData) {
            return;
        }
        const RECT rect =
            *reinterpret_cast<const RECT*>(windowData + kWindowDataRectOffset);
        opacity = ComputeLightOpacity(rect);
    }

    IUnknown* visual = nullptr;
    if (FAILED(sprite->QueryInterface(kIID_IVisual,
                                      reinterpret_cast<void**>(&visual))) ||
        !visual) {
        return;
    }

    void** vtable = *reinterpret_cast<void***>(visual);
    const auto getOpacity =
        reinterpret_cast<GetOpacity_t>(vtable[kIVisualGetOpacityIndex]);
    const auto putOpacity =
        reinterpret_cast<PutOpacity_t>(vtable[kIVisualPutOpacityIndex]);

    // OnOffsetUpdated runs for every position change of every window. Skip
    // the write when nothing changes, so the visual isn't marked dirty.
    float current = -1.0f;
    const bool unchanged = SUCCEEDED(getOpacity(visual, &current)) &&
                           std::fabs(current - opacity) < kOpacityEpsilon;
    if (!unchanged) {
        putOpacity(visual, opacity);
    }
    visual->Release();

    if (unloading) {
        resetCount++;
    }
}

void __cdecl OnOffsetUpdated_Hook(void* pThis) {
    onOffsetUpdated_Original(pThis);
    ApplyDynamicShadows(pThis);
}

long __cdecl UpdateWindowVisuals_Hook(void* pThis) {
    const long result = updateWindowVisuals_Original(pThis);
    // UpdateWindowVisuals runs for every window when the drop shadow setting
    // changes, so the cached flag is always current when it's applied.
    UpdateDropShadowsOn();
    ApplyDynamicShadows(pThis);
    return result;
}

const BYTE* FindPattern(const void* function,
                        size_t searchSize,
                        const BYTE* pattern,
                        size_t patternSize) {
    const BYTE* code = static_cast<const BYTE*>(function);
    for (size_t i = 0; i + patternSize <= searchSize; i++) {
        if (memcmp(code + i, pattern, patternSize) == 0) {
            return code + i;
        }
    }
    return nullptr;
}

bool CodeContains(const void* function,
                  size_t searchSize,
                  const BYTE* pattern,
                  size_t patternSize) {
    return FindPattern(function, searchSize, pattern, patternSize) != nullptr;
}

// True if a relative call (E8 rel32) to target starts within searchSize bytes
// of start. Checking the destination rules out E8 bytes that are part of
// other instructions.
bool HasCallTo(const BYTE* start, size_t searchSize, const void* target) {
    for (size_t i = 0; i < searchSize; i++) {
        if (start[i] != 0xE8) {
            continue;
        }
        int32_t displacement;
        memcpy(&displacement, start + i + 1, sizeof(displacement));
        if (start + i + 5 + displacement == target) {
            return true;
        }
    }
    return false;
}

// Addresses of the uDWM functions used by dynamic shadows. They're resolved
// in the same HookSymbols call as the static hook, as optional entries.
struct DynamicShadowSymbols {
    void* validateVisual = nullptr;
    void* createAndAttachBorderBrush = nullptr;
    OnOffsetUpdated_t onOffsetUpdated = nullptr;
    UpdateWindowVisuals_t updateWindowVisuals = nullptr;
};

bool InitDynamicShadows(const DynamicShadowSymbols& symbols) {
    if (!symbols.validateVisual || !symbols.createAndAttachBorderBrush ||
        !symbols.onOffsetUpdated || !symbols.updateWindowVisuals) {
        Wh_Log(L"Dynamic shadows: required symbols not found.");
        return false;
    }

    const void* onOffsetUpdatedCode =
        reinterpret_cast<const void*>(symbols.onOffsetUpdated);
    const void* updateWindowVisualsCode =
        reinterpret_cast<const void*>(symbols.updateWindowVisuals);

    // The sprite pointer is used to call into a COM vtable, so require the
    // exact code shape: the load must be followed by the call that attaches
    // the shadow brush to that sprite.
    const BYTE* spriteLoad =
        FindPattern(symbols.validateVisual, 0x200, kBorderSpritePattern,
                    sizeof(kBorderSpritePattern));

    if (!spriteLoad ||
        !HasCallTo(spriteLoad + sizeof(kBorderSpritePattern), 0x20,
                   symbols.createAndAttachBorderBrush) ||
        !CodeContains(updateWindowVisualsCode, 0x800, kTopLevelBorderPattern,
                      sizeof(kTopLevelBorderPattern)) ||
        !CodeContains(onOffsetUpdatedCode, 0x40, kWindowDataPattern,
                      sizeof(kWindowDataPattern)) ||
        !CodeContains(onOffsetUpdatedCode, 0x40, kWindowRectPattern,
                      sizeof(kWindowRectPattern))) {
        Wh_Log(L"Dynamic shadows: field offsets don't match this uDWM build.");
        return false;
    }

    // Hooks set before Wh_ModInit returns are applied together with the
    // symbol hooks. If only the first one succeeds, it stays harmless: both
    // hooks do nothing while dynamicShadows is false.
    if (!WindhawkUtils::SetFunctionHook(symbols.onOffsetUpdated,
                                        OnOffsetUpdated_Hook,
                                        &onOffsetUpdated_Original) ||
        !WindhawkUtils::SetFunctionHook(symbols.updateWindowVisuals,
                                        UpdateWindowVisuals_Hook,
                                        &updateWindowVisuals_Original)) {
        Wh_Log(L"Dynamic shadows: hooks could not be registered.");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Crash loop protection
// ---------------------------------------------------------------------------

// DWM records a Dwminit warning when it crashes and is restarted. Two such
// warnings in one minute are treated as a possible crash loop.
bool HasMultipleDwminitWarningsInLastMinute() {
    constexpr WCHAR kQueryPath[] = L"Application";
    constexpr WCHAR kQuery[] =
        L"*[System[Provider[@Name='Dwminit'] and (Level=3) and "
        L"TimeCreated[timediff(@SystemTime) <= 60000]]]";

    EVT_HANDLE queryHandle =
        EvtQuery(nullptr, kQueryPath, kQuery, EvtQueryChannelPath);
    if (!queryHandle) {
        Wh_Log(L"EvtQuery failed with error: %u", GetLastError());
        return false;
    }

    EVT_HANDLE events[2] = {};
    DWORD returned = 0;
    constexpr DWORD kTimeoutMilliseconds = 1000;
    const BOOL ok = EvtNext(queryHandle, ARRAYSIZE(events), events,
                            kTimeoutMilliseconds, 0, &returned);

    if (!ok && GetLastError() != ERROR_NO_MORE_ITEMS) {
        Wh_Log(L"EvtNext failed with error: %u", GetLastError());
    }

    for (DWORD i = 0; i < returned; i++) {
        EvtClose(events[i]);
    }

    EvtClose(queryHandle);
    return ok && returned >= ARRAYSIZE(events);
}

bool IsPossibleDwmCrashLoop() {
    FILETIME nowFileTime;
    GetSystemTimeAsFileTime(&nowFileTime);

    const ULONGLONG now =
        (static_cast<ULONGLONG>(nowFileTime.dwHighDateTime) << 32) |
        nowFileTime.dwLowDateTime;

    ULONGLONG lastInitTime = 0;
    Wh_GetBinaryValue(L"lastInitTime", &lastInitTime, sizeof(lastInitTime));
    Wh_SetBinaryValue(L"lastInitTime", &now, sizeof(now));

    // Querying the event log can wait for up to one second. Skip it during
    // normal DWM startup and only query after another recent initialization.
    constexpr ULONGLONG kOneMinute = 60 * 10000000ULL;
    return now - lastInitTime <= kOneMinute &&
           HasMultipleDwminitWarningsInLastMinute();
}

// ---------------------------------------------------------------------------
// Static shadow parameters
// ---------------------------------------------------------------------------

void __cdecl GetShadowParameters_Hook(int style,
                                      int dpi,
                                      float* radius1,
                                      float* radius2,
                                      float* alpha1,
                                      float* alpha2) {
    getShadowParameters_Original(style, dpi, radius1, radius2, alpha1,
                                 alpha2);

    *radius1 *= sizeScale;
    *radius2 *= sizeScale;
    *alpha1 = std::clamp(*alpha1 * opacityScale, 0.0f, 1.0f);
    *alpha2 = std::clamp(*alpha2 * opacityScale, 0.0f, 1.0f);
}

void RequestDwmRefresh() {
    BOOL shadows = FALSE;
    if (!SystemParametersInfoW(SPI_GETDROPSHADOW, 0, &shadows, 0) ||
        !shadows) {
        return;
    }

    SystemParametersInfoW(SPI_SETDROPSHADOW, 0,
                          reinterpret_cast<PVOID>(FALSE), 0);

    // DWM releases the existing shadow brushes on the next composition frame.
    Sleep(100);

    SystemParametersInfoW(SPI_SETDROPSHADOW, 0,
                          reinterpret_cast<PVOID>(TRUE), 0);
}

}  // namespace

BOOL Wh_ModInit() {
    HMODULE module = GetModuleHandleW(L"uDWM.dll");
    if (!module) {
        Wh_Log(L"uDWM.dll is not loaded.");
        return FALSE;
    }

    const int opacityPercent = std::clamp(Wh_GetIntSetting(L"opacityPercent"),
                                          kOpacityMinimum, kOpacityMaximum);
    const int sizePercent = std::clamp(Wh_GetIntSetting(L"sizePercent"),
                                       kSizeMinimum, kSizeMaximum);

    dynamicShadows = Wh_GetIntSetting(L"dynamicShadows") != 0;
    dimmingStrength =
        std::clamp(Wh_GetIntSetting(L"dimmingStrength"), 0, 100) / 100.0f;
    fadeDistance =
        std::clamp(Wh_GetIntSetting(L"fadeDistance"), 10, 100) / 100.0f;
    LoadLightPosition();
    LoadFadeCurve();

    // Windhawk reloads the mod after every settings change. If the settings
    // request the original Windows appearance, no hooks need to be installed.
    if (opacityPercent == 100 && sizePercent == 100 && !dynamicShadows) {
        Wh_Log(L"Neutral shadow settings selected; "
               L"no DWM hooks are required.");
        return FALSE;
    }

    if (IsPossibleDwmCrashLoop()) {
        Wh_Log(L"Refusing to load: multiple recent Dwminit warnings indicate "
               L"a possible DWM crash loop.");
        return FALSE;
    }

    opacityScale = opacityPercent / 100.0f;
    sizeScale = sizePercent / 100.0f;

    // A single HookSymbols call resolves every uDWM symbol, so Windhawk's
    // symbol cache stays valid. The array is the same whatever the settings
    // are, so changing them doesn't invalidate the cache either.
    DynamicShadowSymbols dynamicSymbols;

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {
                LR"(private: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
                LR"(public: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
            },
            &getShadowParameters_Original,
            GetShadowParameters_Hook,
        },
        // Dynamic shadows: addresses only, hooked after the offsets are
        // validated.
        {
            {LR"(public: virtual long __cdecl CWindowBorder::ValidateVisual(void))"},
            &dynamicSymbols.validateVisual,
            nullptr,
            true,
        },
        {
            {LR"(private: long __cdecl CWindowBorder::CreateAndAttachBorderBrush(struct Windows::UI::Composition::ISpriteVisual *))"},
            &dynamicSymbols.createAndAttachBorderBrush,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl CTopLevelWindow::OnOffsetUpdated(void))"},
            &dynamicSymbols.onOffsetUpdated,
            nullptr,
            true,
        },
        {
            {LR"(private: long __cdecl CTopLevelWindow::UpdateWindowVisuals(void))"},
            &dynamicSymbols.updateWindowVisuals,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, udwmDllHooks,
                                    ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"Required uDWM shadow symbols were not found.");
        return FALSE;
    }

    UpdateDropShadowsOn();

    if (dynamicShadows && !InitDynamicShadows(dynamicSymbols)) {
        Wh_Log(L"Dynamic shadows disabled; static shadow tuning still active.");
        dynamicShadows = false;
    }

    Wh_Log(L"CONFIG opacity=%.2f size=%.2f dynamic=%d light=%d(%.1f,%.1f) "
           L"dimming=%.2f fadeDistance=%.2f curve=%d",
           static_cast<double>(opacityScale), static_cast<double>(sizeScale),
           dynamicShadows ? 1 : 0, static_cast<int>(lightKind),
           static_cast<double>(lightX),
           static_cast<double>(lightY), static_cast<double>(dimmingStrength),
           static_cast<double>(fadeDistance), static_cast<int>(fadeCurve));

    return TRUE;
}

void Wh_ModAfterInit() {
    // Rebuilding the shadows also runs UpdateWindowVisuals for every window,
    // which applies the initial dynamic shadow opacity.
    RequestDwmRefresh();
}

void Wh_ModBeforeUninit() {
    if (!dynamicShadows) {
        return;
    }

    // While shadows are off, sprites are kept at full opacity and the refresh
    // below would do nothing, so there's nothing to reset or wait for.
    BOOL shadows = FALSE;
    if (!SystemParametersInfoW(SPI_GETDROPSHADOW, 0, &shadows, 0) ||
        !shadows) {
        return;
    }

    // The hooks are still active here. Rebuild the shadows once more so that
    // UpdateWindowVisuals resets every sprite to full opacity on DWM's thread.
    unloading = true;
    RequestDwmRefresh();

    // DWM rebuilds the shadows asynchronously. Wait until the reset count
    // stops growing, for at most one second.
    int previous = -1;
    for (int i = 0; i < 20; i++) {
        Sleep(50);
        const int current = resetCount.load();
        if (current > 0 && current == previous) {
            break;
        }
        previous = current;
    }

    Wh_Log(L"Dynamic shadows: reset %d shadow sprites.", resetCount.load());
}

void Wh_ModUninit() {
    // The mod's hooks are removed once Wh_ModBeforeUninit returns, so by the
    // time Wh_ModUninit runs this refresh necessarily rebuilds shadows
    // through DWM's original, unmodified code path.
    RequestDwmRefresh();
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
