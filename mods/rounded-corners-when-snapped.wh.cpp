// ==WindhawkMod==
// @id              rounded-corners-when-snapped
// @name            Rounded corners when snapped or maximized
// @description     Makes DWM draw rounded corners for snapped and maximized windows (macOS-like), without changing the window state
// @version         2.1.0
// @author          Alexey
// @github          https://github.com/leshaalexey
// @include         dwm.exe
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Rounded corners when snapped or maximized

Windows stays in charge of the window: it is really snapped / really maximized
(Snap Layouts, Snap Assist, Win+Arrow, the maximize button — all untouched).
Only the *drawing* changes, inside the compositor.

Two functions of `uDWM.dll` are involved, both members of `CTopLevelWindow`:

* `GetEffectiveCornerStyle` decides the corner style of a composed surface and
  returns `1` (square) for maximized and snapped windows.
* `IsMaximizedOrSnapped` is DWM's own answer for the very same surface.

The hook combines them: when the style came out square *and* DWM says this
surface is a maximized or snapped window, it returns `2` (round) instead.

Some builds don't change the style at all — they keep the rounded style and
zero the *radius* instead. So the three radius getters of `CTopLevelWindow`
(`GetRadiusFromCornerStyle`, `GetFloatCornerRadiusForCurrentStyle`,
`GetDpiAdjustedFloatCornerRadius`) are hooked as well, and a radius of zero is
replaced — but only for surfaces `IsMaximizedOrSnapped` vouches for.

Everything else — the Start menu backdrop, the virtual-desktop switch
animation, drag previews, fullscreen windows — is left exactly as DWM wanted
it, so there are no stray outlines.

## Setup

**1. Let Windhawk into `dwm.exe`** — it is on the built-in list of critical
system processes and mods targeting it fail silently:

* Windhawk → **Settings** → **Advanced settings** → **More advanced settings**
* **Process inclusion list** → add `%systemroot%\system32\dwm.exe`

**2. Keep `explorer.exe` targeted (default)** — `dwm.exe` runs under a
restricted account and usually cannot download PDBs, so explorer resolves the
addresses once and caches them as RVAs for it.

## Requirements

* Windows 11, x64.
* No other DWM corner patcher running (ExplorerPatcher, StartAllBack,
  Win11DisableRoundedCorners).

## Mod authorship

This mod was created by:

- [x] The submitter, with AI assistance
- [x] Claude
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- roundStyle: round
  $name: Corner style
  $options:
  - round: Normal rounding (like an unsnapped window)
  - small: Small rounding (like a menu)
- noFilter: false
  $name: Round everything DWM wanted square
  $description: >-
    Ignores IsMaximizedOrSnapped. Rounds internal surfaces too, which causes
    outlines around the Start menu and desktop switching. For testing only.
- forceRadius: true
  $name: Force a non-zero radius
  $description: >-
    Needed on builds where DWM zeroes the corner radius for maximized windows
    instead of changing the corner style. Filtered by IsMaximizedOrSnapped just
    like the style, so it no longer touches other surfaces.
- radius: 8
  $name: Radius in pixels
  $description: Only used when the option above is on. Windows 11 default is 8.
- dumpSymbols: ""
  $name: Dump symbols containing
  $description: >-
    Diagnostics. A comma-separated list of substrings, for example
    "Hwnd, Maximiz, CornerStyle" - explorer.exe then lists every matching
    uDWM.dll symbol in the log. Leave empty normally.
- debugLog: false
  $name: Diagnostic logging
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>

// ---------------------------------------------------------------- constants

// uDWM CORNER_STYLE
constexpr int kCornerSquare = 1;
constexpr int kCornerRound = 2;
constexpr int kCornerRoundSmall = 3;

constexpr int kCacheVersion = 4;

constexpr PCWSTR kKeyCacheVer = L"cacheVersion";
constexpr PCWSTR kKeyStamp = L"uDwmTimeDateStamp";
constexpr PCWSTR kKeySize = L"uDwmSizeOfImage";
constexpr PCWSTR kKeyRvaCornerStyle = L"rvaGetEffectiveCornerStyle";
constexpr PCWSTR kKeyRvaMaximized = L"rvaIsMaximizedOrSnapped";
constexpr PCWSTR kKeyRvaRadiusStyle = L"rvaGetRadiusFromCornerStyle";
constexpr PCWSTR kKeyRvaRadiusCurrent = L"rvaGetFloatCornerRadiusForCurrentStyle";
constexpr PCWSTR kKeyRvaRadiusDpi = L"rvaGetDpiAdjustedFloatCornerRadius";

// ----------------------------------------------------------------- settings

struct Settings {
    int roundStyle;
    bool noFilter;
    bool forceRadius;
    float radius;
    bool debugLog;
} g_settings;

// -------------------------------------------------------------------- hooks

using GetEffectiveCornerStyle_t = int(__fastcall*)(void* pThis);
using IsMaximizedOrSnapped_t = bool(__fastcall*)(void* pThis);
using FloatGetter_t = float(__fastcall*)(void* pThis);

GetEffectiveCornerStyle_t GetEffectiveCornerStyle_orig;
IsMaximizedOrSnapped_t IsMaximizedOrSnapped;
FloatGetter_t GetRadiusFromCornerStyle_orig;
FloatGetter_t GetFloatCornerRadiusForCurrentStyle_orig;
FloatGetter_t GetDpiAdjustedFloatCornerRadius_orig;

static void LogThrottled(PCWSTR message) {
    static ULONGLONG lastTick;

    if (!g_settings.debugLog) {
        return;
    }
    ULONGLONG now = GetTickCount64();
    if (now - lastTick < 1000) {
        return;
    }
    lastTick = now;
    Wh_Log(L"%s", message);
}

int __fastcall GetEffectiveCornerStyle_hook(void* pThis) {
    int style = GetEffectiveCornerStyle_orig(pThis);

    if (style != kCornerSquare) {
        return style;
    }

    if (!g_settings.noFilter) {
        if (!IsMaximizedOrSnapped || !IsMaximizedOrSnapped(pThis)) {
            return style;
        }
    }

    LogThrottled(L"rounding a maximized/snapped window");
    return g_settings.roundStyle;
}

// All three radius getters are CTopLevelWindow methods, so the very same
// `this` can be asked whether this surface is a maximized/snapped window.
static bool SurfaceWantsRounding(void* pThis) {
    if (!g_settings.forceRadius) {
        return false;
    }
    if (g_settings.noFilter) {
        return true;
    }
    return IsMaximizedOrSnapped && IsMaximizedOrSnapped(pThis);
}

static float FixRadius(void* pThis, float value) {
    if (value > 0.01f || !SurfaceWantsRounding(pThis)) {
        return value;
    }
    LogThrottled(L"forcing radius on a maximized/snapped window");
    return g_settings.radius;
}

float __fastcall GetRadiusFromCornerStyle_hook(void* pThis) {
    return FixRadius(pThis, GetRadiusFromCornerStyle_orig(pThis));
}

float __fastcall GetFloatCornerRadiusForCurrentStyle_hook(void* pThis) {
    return FixRadius(pThis, GetFloatCornerRadiusForCurrentStyle_orig(pThis));
}

float __fastcall GetDpiAdjustedFloatCornerRadius_hook(void* pThis) {
    float value = GetDpiAdjustedFloatCornerRadius_orig(pThis);
    if (value > 0.01f || !SurfaceWantsRounding(pThis)) {
        return value;
    }
    UINT dpi = GetDpiForSystem();
    if (!dpi) dpi = 96;
    return g_settings.radius * (dpi / 96.0f);
}

// ------------------------------------------------------- symbols and cache

struct ResolvedRvas {
    int cornerStyle;
    int isMaximized;
    int radiusStyle;
    int radiusCurrent;
    int radiusDpi;
};

static bool GetModuleIdentity(HMODULE module, int* stamp, int* size) {
    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
    if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<BYTE*>(module) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }
    *stamp = static_cast<int>(nt->FileHeader.TimeDateStamp);
    *size = static_cast<int>(nt->OptionalHeader.SizeOfImage);
    return true;
}

// filter is a comma-separated list of substrings; empty means "no match".
static bool MatchesFilterList(PCWSTR text, PCWSTR filter) {
    if (!text || !filter || !*filter) {
        return false;
    }

    while (*filter) {
        while (*filter == L' ' || *filter == L',') {
            filter++;
        }
        WCHAR token[64];
        int n = 0;
        while (*filter && *filter != L',' && n < ARRAYSIZE(token) - 1) {
            token[n++] = *filter++;
        }
        while (n > 0 && token[n - 1] == L' ') {
            n--;
        }
        token[n] = L'\0';
        if (n && wcsstr(text, token)) {
            return true;
        }
        while (*filter && *filter != L',') {
            filter++;
        }
    }
    return false;
}

static bool ResolveBySymbols(HMODULE module, ResolvedRvas* out,
                             PCWSTR dumpFilter) {
    WH_FIND_SYMBOL_OPTIONS options = {sizeof(options)};
    WH_FIND_SYMBOL symbol = {};

    HANDLE find = Wh_FindFirstSymbol(module, &options, &symbol);
    if (!find) {
        return false;
    }

    auto base = reinterpret_cast<BYTE*>(module);
    *out = {};

    do {
        if (!symbol.symbol || !symbol.address) {
            continue;
        }
        int rva = static_cast<int>(reinterpret_cast<BYTE*>(symbol.address) - base);

        if (MatchesFilterList(symbol.symbol, dumpFilter)) {
            Wh_Log(L"[%08X] %s", rva, symbol.symbol);
        }

        if (wcsstr(symbol.symbol, L"CTopLevelWindow::GetEffectiveCornerStyle")) {
            out->cornerStyle = rva;
        } else if (wcsstr(symbol.symbol, L"CTopLevelWindow::IsMaximizedOrSnapped")) {
            out->isMaximized = rva;
        } else if (wcsstr(symbol.symbol, L"CTopLevelWindow::GetRadiusFromCornerStyle")) {
            out->radiusStyle = rva;
        } else if (wcsstr(symbol.symbol,
                          L"CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle")) {
            out->radiusCurrent = rva;
        } else if (wcsstr(symbol.symbol,
                          L"CTopLevelWindow::GetDpiAdjustedFloatCornerRadius")) {
            out->radiusDpi = rva;
        }
    } while (Wh_FindNextSymbol(find, &symbol));

    Wh_FindCloseSymbol(find);
    return out->cornerStyle != 0;
}

static void StoreRvas(int stamp, int size, const ResolvedRvas& rvas) {
    Wh_SetIntValue(kKeyRvaCornerStyle, rvas.cornerStyle);
    Wh_SetIntValue(kKeyRvaMaximized, rvas.isMaximized);
    Wh_SetIntValue(kKeyRvaRadiusStyle, rvas.radiusStyle);
    Wh_SetIntValue(kKeyRvaRadiusCurrent, rvas.radiusCurrent);
    Wh_SetIntValue(kKeyRvaRadiusDpi, rvas.radiusDpi);
    // Identity last: it validates everything above.
    Wh_SetIntValue(kKeyStamp, stamp);
    Wh_SetIntValue(kKeySize, size);
    Wh_SetIntValue(kKeyCacheVer, kCacheVersion);
}

static bool LoadRvas(int stamp, int size, ResolvedRvas* out) {
    if (Wh_GetIntValue(kKeyCacheVer, 0) != kCacheVersion ||
        Wh_GetIntValue(kKeyStamp, 0) != stamp ||
        Wh_GetIntValue(kKeySize, 0) != size) {
        return false;
    }
    out->cornerStyle = Wh_GetIntValue(kKeyRvaCornerStyle, 0);
    out->isMaximized = Wh_GetIntValue(kKeyRvaMaximized, 0);
    out->radiusStyle = Wh_GetIntValue(kKeyRvaRadiusStyle, 0);
    out->radiusCurrent = Wh_GetIntValue(kKeyRvaRadiusCurrent, 0);
    out->radiusDpi = Wh_GetIntValue(kKeyRvaRadiusDpi, 0);
    return out->cornerStyle != 0;
}

// ---------------------------------------------------------------- settings

static void LoadSettings() {
    PCWSTR style = Wh_GetStringSetting(L"roundStyle");
    g_settings.roundStyle =
        (style && wcscmp(style, L"small") == 0) ? kCornerRoundSmall : kCornerRound;
    Wh_FreeStringSetting(style);

    g_settings.noFilter = Wh_GetIntSetting(L"noFilter") != 0;
    g_settings.forceRadius = Wh_GetIntSetting(L"forceRadius") != 0;
    g_settings.debugLog = Wh_GetIntSetting(L"debugLog") != 0;

    int radius = Wh_GetIntSetting(L"radius");
    if (radius < 1) radius = 8;
    if (radius > 60) radius = 60;
    g_settings.radius = static_cast<float>(radius);
}

// -------------------------------------------------------------- entrypoints

static bool RunningInDwm() {
    WCHAR path[MAX_PATH]{};
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) {
        return false;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"dwm.exe") == 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE hUDWM = GetModuleHandleW(L"uDWM.dll");
    if (!hUDWM) {
        hUDWM = LoadLibraryW(L"uDWM.dll");
    }
    if (!hUDWM) {
        Wh_Log(L"uDWM.dll not available");
        return FALSE;
    }

    int stamp = 0, size = 0;
    if (!GetModuleIdentity(hUDWM, &stamp, &size)) {
        Wh_Log(L"Bad uDWM.dll headers");
        return FALSE;
    }

    // --- Warmer mode: resolve and cache the addresses for dwm.exe.
    if (!RunningInDwm()) {
        PCWSTR dump = Wh_GetStringSetting(L"dumpSymbols");
        bool wantDump = dump && *dump;

        ResolvedRvas cached{};
        if (!wantDump && LoadRvas(stamp, size, &cached)) {
            Wh_Log(L"warmer: RVAs already cached for this uDWM build");
            Wh_FreeStringSetting(dump);
            return TRUE;
        }

        ResolvedRvas rvas{};
        bool ok = ResolveBySymbols(hUDWM, &rvas, dump);
        Wh_FreeStringSetting(dump);

        if (!ok) {
            Wh_Log(L"warmer: failed to enumerate uDWM.dll symbols here too");
            return TRUE;
        }

        StoreRvas(stamp, size, rvas);
        Wh_Log(L"warmer: stored RVAs style=%08X maximized=%08X", rvas.cornerStyle,
               rvas.isMaximized);
        return TRUE;
    }

    // --- dwm.exe: cache first, own enumeration as a fallback.
    ResolvedRvas rvas{};
    if (LoadRvas(stamp, size, &rvas)) {
        Wh_Log(L"dwm: using cached RVAs");
    } else if (ResolveBySymbols(hUDWM, &rvas, nullptr)) {
        Wh_Log(L"dwm: resolved symbols locally");
        StoreRvas(stamp, size, rvas);
    } else {
        Wh_Log(L"dwm: no cached RVAs and no symbols - let explorer.exe warm "
               L"the cache, then re-enable the mod");
        return FALSE;
    }

    auto base = reinterpret_cast<BYTE*>(hUDWM);

    if (rvas.isMaximized) {
        IsMaximizedOrSnapped =
            reinterpret_cast<IsMaximizedOrSnapped_t>(base + rvas.isMaximized);
    } else if (!g_settings.noFilter) {
        Wh_Log(L"dwm: CTopLevelWindow::IsMaximizedOrSnapped not found - "
               L"staying passive to avoid rounding internal surfaces");
    }

    Wh_Log(L"dwm: RVAs style=%08X maximized=%08X radius=%08X/%08X/%08X",
           rvas.cornerStyle, rvas.isMaximized, rvas.radiusStyle,
           rvas.radiusCurrent, rvas.radiusDpi);

    int hooked = 0;

    if (rvas.cornerStyle &&
        Wh_SetFunctionHook(base + rvas.cornerStyle,
                           (void*)GetEffectiveCornerStyle_hook,
                           (void**)&GetEffectiveCornerStyle_orig)) {
        hooked++;
    }
    // Always hooked; whether they change anything is decided per call, so the
    // radius setting can be toggled without recompiling.
    if (rvas.radiusStyle &&
        Wh_SetFunctionHook(base + rvas.radiusStyle,
                           (void*)GetRadiusFromCornerStyle_hook,
                           (void**)&GetRadiusFromCornerStyle_orig)) {
        hooked++;
    }
    if (rvas.radiusCurrent &&
        Wh_SetFunctionHook(base + rvas.radiusCurrent,
                           (void*)GetFloatCornerRadiusForCurrentStyle_hook,
                           (void**)&GetFloatCornerRadiusForCurrentStyle_orig)) {
        hooked++;
    }
    if (rvas.radiusDpi &&
        Wh_SetFunctionHook(base + rvas.radiusDpi,
                           (void*)GetDpiAdjustedFloatCornerRadius_hook,
                           (void**)&GetDpiAdjustedFloatCornerRadius_orig)) {
        hooked++;
    }

    if (!hooked) {
        Wh_Log(L"dwm: nothing hooked");
        return FALSE;
    }

    Wh_Log(L"dwm: hooked %d function(s), filter=%s", hooked,
           IsMaximizedOrSnapped ? L"IsMaximizedOrSnapped" : L"none");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, TRUE, nullptr, SPIF_SENDCHANGE);
}

void Wh_ModUninit() {
    Wh_Log(L"Unloaded");
}
