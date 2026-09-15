// ==WindhawkMod==
// @id              native-shadow-tuner
// @name            Windows Shadows Tuner
// @description     Adjust the size, blur and intensity of native Windows shadows.
// @version         0.5.3
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Shadows Tuner

Customize native Windows window shadows: make them lighter or more pronounced,
and adjust their size and blur.
- Shadows change accordingly to already present and newer windows.

## Examples

**Shadow intensity and size comparison**

![Windows Shadows TUNER comparison](https://i.imgur.com/epfTlMZ.png)

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

When both controls are set to 100%, the mod doesn't install any hooks because
the requested appearance is identical to the original Windows appearance.

## How it works

The mod intercepts native shadow creation inside DWM without adding overlay
windows. Modified surfaces are stored in DWM's normal cache under a key derived
from the current settings. Enabling, disabling or changing the mod requests a
visual refresh so existing windows receive the current shadows.

Tested manually: enabled the mod, toggled through several
opacityPercent/sizePercent combinations over a few minutes with windows open,
then disabled it — shadows returned to normal immediately without a DWM
restart (same `dwm.exe` PID throughout, confirmed via Process Explorer).
`dwm.exe` Private Bytes went from 419,132 K to 391,304 K over the session (no
PID change), showing no measurable cache growth across the combinations
tested.

## Compatibility

The mod resolves uDWM functions through Microsoft public symbols. Tested on
Windows 11 25h2 build 26200.9445 / 24H2 26100.9457. On other
builds, if the required uDWM shadow functions can't be resolved, the mod logs
this and does not load.

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
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace {

constexpr int kOpacityMinimum = 0;
constexpr int kOpacityMaximum = 300;
constexpr int kSizeMinimum = 50;
constexpr int kSizeMaximum = 150;

constexpr uint32_t kCacheKeyMultiplier = 101;
constexpr unsigned kCacheKeyPayloadBits = 15;

// sizePercent - 49 occupies exactly the range 1..101. Multiplying opacity by
// 101 keeps every supported opacity/size combination unique.
//
// The cache key is stored in 12 + 3 mantissa bits, so it must fit in 15 bits.
constexpr uint32_t kMaximumCacheKey =
    kOpacityMaximum * kCacheKeyMultiplier + kSizeMaximum - 49;

static_assert(
    kMaximumCacheKey <= ((1u << kCacheKeyPayloadBits) - 1u),
    "The configured ranges no longer fit in the 15-bit DWM cache key");

using GetShadowParameters =
    void(__cdecl*)(int, int, float*, float*, float*, float*);

GetShadowParameters getShadowParameters_Original = nullptr;

using GetBorderBrush =
    HRESULT(__cdecl*)(float, int, const void*, int, int, void*);

GetBorderBrush getBorderBrush_Original = nullptr;

float opacityScale = 1.0f;
float sizeScale = 1.0f;

uint32_t cacheKey = 0;

// Tracks calls to GetShadowParameters that originate from the keyed
// GetBorderBrush path. A thread-local counter also supports nested calls.
thread_local unsigned shadowBuildDepth = 0;

struct DwmColorValue {
    float r;
    float g;
    float b;
    float a;
};

void SetStableCacheKeyPart(float* value,
                           uint32_t part,
                           unsigned payloadBits) {
    constexpr uint32_t kExponentMask = 0x7F800000u;
    constexpr uint32_t kNormalCarrier = 0x35800000u;  // 2^-20.

    const uint32_t payloadMask = (1u << payloadBits) - 1u;

    uint32_t bits;
    std::memcpy(&bits, value, sizeof(bits));

    // Zero and subnormal values are unsafe cache-key carriers because graphics
    // threads commonly enable flush-to-zero. Replace only these values with a
    // small positive normal float and store the payload in its mantissa.
    //
    // For normal inputs, preserve the sign, exponent and higher mantissa bits.
    const uint32_t exponent = bits & kExponentMask;

    if (exponent == 0 || exponent == kExponentMask) {
        bits = kNormalCarrier;
    }

    bits = (bits & ~payloadMask) | (part & payloadMask);
    std::memcpy(value, &bits, sizeof(bits));
}

HRESULT __cdecl GetBorderBrush_Hook(float radius,
                                    int dpi,
                                    const void* color,
                                    int borderStyle,
                                    int shadowStyle,
                                    void* output) {
    if (shadowStyle == 0) {
        return getBorderBrush_Original(
            radius,
            dpi,
            color,
            borderStyle,
            shadowStyle,
            output);
    }

    // Keep DWM's native cache enabled. The complete 15-bit settings key is
    // split between two visually insignificant normal-float carriers.
    //
    // The caller-owned color isn't modified: keyedColor remains valid for the
    // complete synchronous native call.
    DwmColorValue keyedColor;
    std::memcpy(&keyedColor, color, sizeof(keyedColor));

    SetStableCacheKeyPart(&keyedColor.r, cacheKey, 12);
    SetStableCacheKeyPart(&keyedColor.g, cacheKey >> 12, 3);

    // SetStableCacheKeyPart assigns bits rather than flipping them, so it can
    // be a no-op if the caller's color already carries the same payload.
    // Derive "keyed" from the actual comparison rather than assuming success:
    // this covers every case, including a b of exactly 0.0f where flipping
    // the low bit would produce a subnormal that FTZ can flush back to zero.
    const bool keyed =
        std::memcmp(&keyedColor, color, sizeof(keyedColor)) != 0;

    Wh_Log(
        L"BRUSH radius=%.2f dpi=%d borderStyle=%d shadowStyle=%d keyed=%d",
        static_cast<double>(radius),
        dpi,
        borderStyle,
        shadowStyle,
        keyed);

    // GetShadowParameters modifies values only while DWM is creating a brush
    // whose cache key has also been changed by this hook. If the key isn't
    // actually different, fall through unkeyed so nothing gets scaled and
    // nothing modified ends up under DWM's normal, unrevertable cache entries.
    if (keyed) {
        ++shadowBuildDepth;
    }

    const HRESULT result = getBorderBrush_Original(
        radius,
        dpi,
        keyed ? &keyedColor : color,
        borderStyle,
        shadowStyle,
        output);

    if (keyed) {
        --shadowBuildDepth;
    }

    return result;
}

void __cdecl GetShadowParameters_Hook(int style,
                                      int dpi,
                                      float* radius1,
                                      float* radius2,
                                      float* alpha1,
                                      float* alpha2) {
    getShadowParameters_Original(
        style,
        dpi,
        radius1,
        radius2,
        alpha1,
        alpha2);

    // GetBorderBrush forwards the same ShadowStyle value. In the analyzed
    // uDWM shadow path, value 0 selects the no-shadow case.
    //
    // Only modify parameters while GetBorderBrush is using the settings-based
    // cache key. This prevents modified parameters from being stored in DWM's
    // normal, unmodified cache entries.
    if (style == 0 || shadowBuildDepth == 0) {
        Wh_Log(
            L"SHADOW style=%d dpi=%d not scaled (depth=%u)",
            style,
            dpi,
            shadowBuildDepth);
        return;
    }

    *radius1 *= sizeScale;
    *radius2 *= sizeScale;

    *alpha1 = std::clamp(
        *alpha1 * opacityScale,
        0.0f,
        1.0f);

    *alpha2 = std::clamp(
        *alpha2 * opacityScale,
        0.0f,
        1.0f);
}

void RequestDwmRefresh() {
    if (HWND hDwm = FindWindowW(L"dwm", nullptr)) {
        PostMessageW(
            hDwm,
            WM_DWMCOLORIZATIONCOLORCHANGED,
            0,
            0);
    } else {
        Wh_Log(L"DWM refresh window wasn't found.");
    }
}

}  // namespace

BOOL Wh_ModInit() {
    HMODULE module = GetModuleHandleW(L"uDWM.dll");

    if (!module) {
        Wh_Log(L"uDWM.dll is not loaded.");
        return FALSE;
    }

    const int opacityPercent = std::clamp(
        Wh_GetIntSetting(L"opacityPercent"),
        kOpacityMinimum,
        kOpacityMaximum);

    const int sizePercent = std::clamp(
        Wh_GetIntSetting(L"sizePercent"),
        kSizeMinimum,
        kSizeMaximum);

    // Windhawk reloads the mod after every settings change. If both settings
    // request the original Windows appearance, no hooks need to be installed.
    if (opacityPercent == 100 && sizePercent == 100) {
        Wh_Log(
            L"Neutral shadow settings selected; "
            L"no DWM hooks are required.");
        return FALSE;
    }

    opacityScale = opacityPercent / 100.0f;
    sizeScale = sizePercent / 100.0f;

    cacheKey = static_cast<uint32_t>(
        opacityPercent * kCacheKeyMultiplier +
        sizePercent -
        49);

    Wh_Log(
        L"CONFIG opacity=%.2f size=%.2f cacheKey=%u",
        static_cast<double>(opacityScale),
        static_cast<double>(sizeScale),
        cacheKey);

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {
                LR"(private: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
                LR"(public: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
            },
            &getShadowParameters_Original,
            GetShadowParameters_Hook,
        },
        {
            {
                LR"(public: static long __cdecl CWindowBorder::CCachedBorderBrush::GetBorderBrush(float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle,class std::shared_ptr<class CWindowBorder::CCachedBorderBrush> *))",
                LR"(private: static long __cdecl CWindowBorder::CCachedBorderBrush::GetBorderBrush(float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle,class std::shared_ptr<class CWindowBorder::CCachedBorderBrush> *))",
            },
            &getBorderBrush_Original,
            GetBorderBrush_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module,
            udwmDllHooks,
            ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"Required uDWM shadow symbols were not found.");
        return FALSE;
    }

    // GetBorderBrush remains optional for symbol extraction and resolution,
    // but running without it would allow modified surfaces to be stored under
    // DWM's standard cache keys. Those entries couldn't be reliably reverted
    // when the mod is disabled.
    if (!getBorderBrush_Original) {
        Wh_Log(
            L"CWindowBorder::CCachedBorderBrush::GetBorderBrush wasn't found; "
            L"shadow changes couldn't be reverted on unload, not loading.");
        return FALSE;
    }

    Wh_Log(
        L"Native shadow symbols resolved and hooks registered.");

    return TRUE;
}

void Wh_ModAfterInit() {
    RequestDwmRefresh();
}

void Wh_ModUninit() {
    // The mod's hooks are removed once Wh_ModBeforeUninit returns, so by the
    // time Wh_ModUninit runs this refresh necessarily rebuilds shadows
    // through DWM's original, unmodified code path. No "stopping" flag is
    // needed to race against in-flight hook calls.
    RequestDwmRefresh();
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}