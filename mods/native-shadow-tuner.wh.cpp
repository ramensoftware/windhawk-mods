// ==WindhawkMod==
// @id              native-shadow-tuner
// @name            Windows Shadows Tuner
// @description     Adjust the size, blur and intensity of native Windows shadows.
// @version         0.5.0
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Shadows TUNER
### HaVeN80

Customize native Windows window shadows: make them lighter or more pronounced,
and adjust their size and blur.

## Setup

`dwm.exe` is a critical system process and Windhawk doesn't inject mods into it
by default. Open Windhawk Settings, go to **Advanced settings**, then
**More advanced settings**, and add `dwm.exe` to the process inclusion list.

![Windhawk advanced settings](https://i.imgur.com/epfTlMZ.png)

## Controls

- **Intensity**: 100 keeps the original opacity, 50 halves it and 150 makes
  the shadow more pronounced. At 0, the shadow becomes invisible.
- **Size and blur**: adjusts the shadow spread and blur from 50% to 150%.


## How it works

The mod intercepts native shadow creation inside DWM without adding overlay
windows. Modified surfaces are stored in DWM's normal cache under a key derived
from the current settings. Enabling, disabling or changing the mod requests a
visual refresh so existing windows receive the current shadows.


## Compatibility

The mod resolves uDWM functions through Microsoft public symbols and supports
Windows 11 builds that expose the required shadow functions.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacityPercent: 100
  $name: Shadow Intensity (% of the original value)
  $description: From 0 to 300. 100 original, 50 less visible, 150 more visible. 0 to make invisible.
- sizePercent: 100
  $name: Dimension and Blur (% of the original value)
  $description: From 50 to 150. 100 keeps the original value.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>

namespace {
using Parameters = void(__cdecl*)(int, int, float*, float*, float*, float*);
Parameters original = nullptr;
float opacityScale = 1.0f;
float sizeScale = 1.0f;

std::atomic<bool> stopping{false};
std::atomic<unsigned> surfaceReports{0}, brushReports{0};

uint32_t cacheKey = 0;

// Color is a const reference (pointer). Output is an opaque pointer to a native
// std::shared_ptr; ownership remains entirely inside uDWM.
using Brush = HRESULT(__cdecl*)(float, int, const void*, int, int, void*);
Brush getBrushOriginal = nullptr;

struct DwmColorValue {
    float r;
    float g;
    float b;
    float a;
};

void SetCacheKeyPart(float* value, uint32_t part) {
    uint32_t bits;
    std::memcpy(&bits, value, sizeof(bits));
    bits = (bits & ~0xFFFu) | (part & 0xFFFu);
    std::memcpy(value, &bits, sizeof(bits));
}

HRESULT __cdecl GetBrushHook(float radius, int dpi, const void* color,
                            int borderStyle, int shadowStyle, void* output) {
    if (stopping.load(std::memory_order_relaxed) || shadowStyle == 0 ||
        (opacityScale == 1.0f && sizeScale == 1.0f))
        return getBrushOriginal(radius, dpi, color, borderStyle, shadowStyle, output);

    // Keep DWM's native cache active. Two visually insignificant mantissa
    // fragments make the cache key unique for the current size/opacity pair.
    // The local copy is valid for the complete synchronous native call.
    DwmColorValue keyedColor;
    std::memcpy(&keyedColor, color, sizeof(keyedColor));
    SetCacheKeyPart(&keyedColor.r, cacheKey);
    SetCacheKeyPart(&keyedColor.g, cacheKey >> 12);
    if (std::memcmp(&keyedColor, color, sizeof(keyedColor)) == 0) {
        uint32_t bits;
        std::memcpy(&bits, &keyedColor.b, sizeof(bits));
        bits ^= 1;
        std::memcpy(&keyedColor.b, &bits, sizeof(bits));
    }
    HRESULT hr = getBrushOriginal(radius, dpi, &keyedColor, borderStyle,
                                  shadowStyle, output);
    if (brushReports.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"SHADOW_CACHE key=%u style=%d dpi=%d HRESULT=0x%08X", cacheKey,
               shadowStyle, dpi,
               static_cast<unsigned>(hr));
    return hr;
}

void __cdecl ParametersHook(int style, int dpi, float* radius1,
                           float* radius2, float* alpha1, float* alpha2) {
    original(style, dpi, radius1, radius2, alpha1, alpha2);
    if (stopping.load(std::memory_order_relaxed)) return;
    // Settings are immutable for the lifetime of this hook. Windhawk reloads
    // the mod on changes; no allocation, I/O or object traversal here.
    float before1 = *alpha1, before2 = *alpha2;
    *radius1 *= sizeScale;
    *radius2 *= sizeScale;
    *alpha1 = std::clamp(*alpha1 * opacityScale, 0.0f, 1.0f);
    *alpha2 = std::clamp(*alpha2 * opacityScale, 0.0f, 1.0f);
    if (surfaceReports.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"SHADOW style=%d dpi=%d alpha=(%.3f,%.3f)->(%.3f,%.3f) radii=(%.2f,%.2f)",
               style, dpi,
               static_cast<double>(before1), static_cast<double>(before2),
               static_cast<double>(*alpha1), static_cast<double>(*alpha2),
               static_cast<double>(*radius1), static_cast<double>(*radius2));
}

void RequestDwmRefresh() {
    if (HWND hDwm = FindWindowW(L"dwm", nullptr)) {
        PostMessageW(hDwm, WM_DWMCOLORIZATIONCOLORCHANGED, 0, 0);
    } else {
        Wh_Log(L"DWM refresh window wasn't found.");
    }
}

}

BOOL Wh_ModInit() {
    stopping.store(false);
    surfaceReports.store(0);
    brushReports.store(0);
    HMODULE module = GetModuleHandleW(L"uDWM.dll");
    if (!module) {
        Wh_Log(L"uDWM.dll is not loaded.");
        return FALSE;
    }

    int opacityPercent = std::clamp(Wh_GetIntSetting(L"opacityPercent"), 0, 300);
    int sizePercent = std::clamp(Wh_GetIntSetting(L"sizePercent"), 50, 150);
    opacityScale = opacityPercent / 100.0f;
    sizeScale = sizePercent / 100.0f;
    cacheKey = static_cast<uint32_t>(opacityPercent * 101 + sizePercent - 49);
    Wh_Log(L"CONFIG opacity=%.2f size=%.2f cacheKey=%u",
           static_cast<double>(opacityScale), static_cast<double>(sizeScale), cacheKey);

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {
                LR"(private: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
                LR"(public: static void __cdecl CWindowBorder::GetShadowParameters(enum CWindowBorder::ShadowStyle,int,float *,float *,float *,float *))",
            },
            reinterpret_cast<void**>(&original),
            reinterpret_cast<void*>(ParametersHook),
        },
        {
            {
                LR"(public: static long __cdecl CWindowBorder::CCachedBorderBrush::GetBorderBrush(float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle,class std::shared_ptr<class CWindowBorder::CCachedBorderBrush> *))",
            },
            reinterpret_cast<void**>(&getBrushOriginal),
            reinterpret_cast<void*>(GetBrushHook),
        },
    };

    if (!WindhawkUtils::HookSymbols(module, udwmDllHooks,
                                    ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"Required uDWM shadow symbols were not found.");
        return FALSE;
    }

    Wh_Log(L"Native shadow symbols resolved and hooks registered.");
    return TRUE;
}

void Wh_ModAfterInit() {
    RequestDwmRefresh();
}

void Wh_ModBeforeUninit() {
    stopping.store(true);
    RequestDwmRefresh();
}
void Wh_ModUninit() {}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
