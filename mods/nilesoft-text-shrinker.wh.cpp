// ==WindhawkMod==
// @id              nilesoft-text-shrinker
// @name            Nilesoft Text Shrinker
// @description     Forces menu text to be small regardless of icon size.
// @version         1.0.0
// @author          Lockframe
// @github          https://www.github.com/Lockframe
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Nilesoft Text Shrinker

This mod allows the user to resize the text in Nilesoft Shell's context menu without affecting the glyphs, by using the `font` setting in `shell.nss` to resize the glyphs, and this mod to resize the font.

For it to work, the user MUST use a different font from `Segoe UI` in their `shell.nss` file, otherwise the mod may affect the `explorer.exe` process as a whole.

![](https://i.imgur.com/Sb5S3g4.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TargetFontSize: -11
  $name: Target Font Size
  $description: The size of the text to draw (negative values = pixels).
- SourceFontName: "Calibri"
  $name: Source Font Name (Filter)
  $description: Set this to the EXACT font name you used in theme.nss.
- ShrinkThreshold: 24
  $name: Shrink Threshold (px)
  $description: Only activate if the original font is taller than this.
- ForceRowHeight: 0
  $name: Force Row Height (px)
  $description: If > 0, forces the menu rows to be this specific height.
- VerticalOffset: 0
  $name: Vertical Offset (px)
  $description: Nudge text Up (negative) or Down (positive).
- TargetFontFace: "Segoe UI"
  $name: Target Font Face
  $description: The font family to use for the final small text.
- EnableDebug: false
  $name: Enable Debug Logging
  $description: Log matches to the Output tab.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <synchapi.h>
#include <atomic>

namespace {

    // ---------------------------------------------------------------------
    // Constants & Globals
    // ---------------------------------------------------------------------
    constexpr size_t FNV_PRIME = 1099511628211ULL;
    constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;

    std::atomic<int> g_threshold;
    std::atomic<int> g_forceRowHeight;
    std::atomic<int> g_verticalOffset;
    std::atomic<bool> g_debug;
    std::atomic<size_t> g_sourceFontHash;

    HFONT g_hSmallFont = NULL;
    int g_smallFontHeight = 0;
    SRWLOCK g_srwLock = SRWLOCK_INIT; 

    typedef int (WINAPI *DrawTextW_t)(HDC, LPCWSTR, int, LPRECT, UINT);
    DrawTextW_t DrawTextW_Original;

    // ---------------------------------------------------------------------
    // RAII Safety Wrappers
    // ---------------------------------------------------------------------
    
    // Automatically releases Shared Lock when out of scope
    class AutoSRWShared {
        SRWLOCK& _lock;
    public:
        AutoSRWShared(SRWLOCK& lock) : _lock(lock) { AcquireSRWLockShared(&_lock); }
        ~AutoSRWShared() { ReleaseSRWLockShared(&_lock); }
    };

    // Automatically restores the original GDI object when out of scope
    class AutoGDISelect {
        HDC _hdc;
        HGDIOBJ _hOld;
    public:
        AutoGDISelect(HDC hdc, HGDIOBJ hNew) : _hdc(hdc) { 
            _hOld = SelectObject(hdc, hNew); 
        }
        ~AutoGDISelect() { 
            if (_hOld) SelectObject(_hdc, _hOld); 
        }
    };

    // ---------------------------------------------------------------------
    // Helpers
    // ---------------------------------------------------------------------
    inline wchar_t FastLower(wchar_t c) {
        if (c >= L'A' && c <= L'Z') return c | 0x20; 
        return c;
    }

    size_t HashFontName(LPCWSTR str) {
        size_t hash = FNV_OFFSET_BASIS;
        while (str && *str) {
            hash ^= (size_t)FastLower(*str++); 
            hash *= FNV_PRIME;
        }
        return hash;
    }
}

// -------------------------------------------------------------------------
// Settings Update
// -------------------------------------------------------------------------
void UpdateSettings() {
    int newThreshold = Wh_GetIntSetting(L"ShrinkThreshold");
    int newForceRowHeight = Wh_GetIntSetting(L"ForceRowHeight");
    int newVerticalOffset = Wh_GetIntSetting(L"VerticalOffset");
    bool newDebug = Wh_GetIntSetting(L"EnableDebug");

    PCWSTR srcFont = Wh_GetStringSetting(L"SourceFontName");
    size_t newHash = HashFontName(srcFont);
    Wh_FreeStringSetting(srcFont);

    int targetSize = Wh_GetIntSetting(L"TargetFontSize");
    PCWSTR tgtFontName = Wh_GetStringSetting(L"TargetFontFace");
    
    // Safety: LF_FACESIZE (32) is the Windows max for font names.
    // We use _snwprintf_s with _TRUNCATE to ensure we never buffer overflow
    // even if the user types a novel into the settings box.
    wchar_t safeFontName[LF_FACESIZE];
    _snwprintf_s(safeFontName, _countof(safeFontName), _TRUNCATE, L"%s", tgtFontName ? tgtFontName : L"Segoe UI");
    Wh_FreeStringSetting(tgtFontName);

    HFONT hNewFont = CreateFontW(
        targetSize, 0, 0, 0, FW_NORMAL, 
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, safeFontName
    );

    int newFontHeight = 0;
    if (hNewFont) {
        HDC hdc = CreateCompatibleDC(NULL);
        if (hdc) {
            AutoGDISelect fontScope(hdc, hNewFont);
            TEXTMETRICW tm;
            GetTextMetricsW(hdc, &tm);
            newFontHeight = tm.tmHeight;
            DeleteDC(hdc);
        }
    }

    // Commit
    AcquireSRWLockExclusive(&g_srwLock);

    g_threshold.store(newThreshold, std::memory_order_relaxed);
    g_forceRowHeight.store(newForceRowHeight, std::memory_order_relaxed);
    g_verticalOffset.store(newVerticalOffset, std::memory_order_relaxed);
    g_debug.store(newDebug, std::memory_order_relaxed);
    g_sourceFontHash.store(newHash, std::memory_order_relaxed);

    HFONT hOldFontToDelete = g_hSmallFont;
    g_hSmallFont = hNewFont;
    g_smallFontHeight = newFontHeight;

    ReleaseSRWLockExclusive(&g_srwLock);

    if (hOldFontToDelete) DeleteObject(hOldFontToDelete);
}

// -------------------------------------------------------------------------
// Hook
// -------------------------------------------------------------------------
int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    
    // Phase 1: Lock-Free Filter
    if (!lpchText) return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    if (cchText == 0 || (cchText == -1 && *lpchText == L'\0')) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    TEXTMETRICW tm;
    if (!GetTextMetricsW(hdc, &tm)) return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    if (tm.tmHeight < g_threshold.load(std::memory_order_relaxed)) return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);

    wchar_t faceName[LF_FACESIZE];
    if (GetTextFaceW(hdc, _countof(faceName), faceName) <= 0) return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);

    if (HashFontName(faceName) != g_sourceFontHash.load(std::memory_order_relaxed)) {
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    // Phase 2: Execution (RAII-Protected)
    AutoSRWShared lockScope(g_srwLock);

    if (!g_hSmallFont) return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);

    if (g_debug.load(std::memory_order_relaxed)) {
        Wh_Log(L"Shrinking: %.20s", lpchText);
    }

    // A. Layout
    if (format & DT_CALCRECT) {
        int forcedHeight = g_forceRowHeight.load(std::memory_order_relaxed);
        if (forcedHeight > 0) {
            {
                AutoGDISelect fontScope(hdc, g_hSmallFont);
                DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
            } // fontScope releases here, restoring old font
            
            lprc->bottom = lprc->top + forcedHeight;
            return forcedHeight;
        }
        return DrawTextW_Original(hdc, lpchText, cchText, lprc, format);
    }

    // B. Painting
    RECT newRect = *lprc;
    int vOffset = g_verticalOffset.load(std::memory_order_relaxed);

    if ((format & DT_SINGLELINE) && !(format & DT_VCENTER)) {
        int boxHeight = lprc->bottom - lprc->top;
        vOffset += (boxHeight - g_smallFontHeight) / 2;
    }
    newRect.top += vOffset;
    newRect.bottom += vOffset; 

    // RAII Font Selection
    AutoGDISelect fontScope(hdc, g_hSmallFont);
    return DrawTextW_Original(hdc, lpchText, cchText, &newRect, format);
}

// -------------------------------------------------------------------------
// Init / Uninit
// -------------------------------------------------------------------------
void Wh_ModSettingsChanged() {
    UpdateSettings();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init Nilesoft Text Shrinker");
    UpdateSettings();

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return FALSE;

    void* pDrawTextW = (void*)GetProcAddress(hUser32, "DrawTextW");
    if (pDrawTextW) {
        if (!Wh_SetFunctionHook(pDrawTextW, (void*)DrawTextW_Hook, (void**)&DrawTextW_Original)) {
             Wh_Log(L"Failed to hook DrawTextW");
             return FALSE;
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hSmallFont) DeleteObject(g_hSmallFont);
}