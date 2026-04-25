// ==WindhawkMod==
// @id              word-omath-shade-fix
// @name            Word OfficeMath EditBox Shade Fix
// @name:zh-CN      Word OfficeMath 公式编辑框阴影修复
// @description     Fix the shade color issue of OfficeMath equation edit boxes in Word dark mode.
// @description:zh-CN 解决 Word 深色模式下 OfficeMath 公式编辑框阴影颜色问题
// @version         1.0.0
// @author          Joe Ye
// @github          https://github.com/JoeYe-233
// @include         winword.exe
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Word OfficeMath EditBox Shade Fix

This mod fixes the issue where the shade color of OfficeMath equation edit boxes does not change according to the theme (light/dark) in Microsoft Word. It achieves this by hooking into the `DrawMathShd` function in `wwlib.dll`, which is responsible for drawing the shade, and dynamically patching the hardcoded color value based on the current theme (can be customized in the settings).

*Note: this mod needs pdb symbol of `wwlib.dll` to work. The symbol file is expected to be a bit large (~90MB in size). Windhawk will download it automatically when you launch Word first time after you installed the mod (the popup at right bottom corner of your screen, please make sure that it shows percentage like "Loading symbols... 0% (wwlib.dll)", wait until it reaches 100% and the pop up disappears, otherwise please switch your network and try again) please wait patiently and make sure to **relaunch Word AS ADMINISTRATOR at least once** after it finishes, this is to write symbols being used to SymbolCache, which speeds up launching later on.*

## ⚠️ Note:
- It is advised to **turn off automatic updates** for Office applications, as PDB may need to be downloaded every time after updates.
- Please relaunch Word as administrator *at least once* after installing the mod and wait for symbol download to complete, this is to write symbols being used to SymbolCache, which speeds up launching later on.

*This mod is part of **The Ultimate Office Dark Mode Project***

# Before vs After
![Before-After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-omath-shade-fix-before-vs-after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- lightModeColor: ""
  $name: Light Mode Shade Color
  $name:zh-CN: 浅色模式阴影颜色
  $description: "Hex color code for light theme (Format: #RRGGBB or rrggbb) Leave blank = #D7DCE6"
  $description:zh-CN: "十六进制颜色代码，格式为 #RRGGBB 或 rrggbb，留空则使用默认值 #D7DCE6"
- darkModeColor: ""
  $name: Dark Mode Shade Color
  $name:zh-CN: 深色模式阴影颜色
  $description: "Hex color code for dark theme (Format: #RRGGBB or rrggbb) Leave blank = #333D4E"
  $description:zh-CN: "十六进制颜色代码，格式为 #RRGGBB 或 rrggbb，留空则使用默认值 #333D4E"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <windhawk_utils.h>
#include <atomic>
#include <vector>
#include <wctype.h> // Required for iswxdigit

// =============================================================
// Global State & Function Pointers
// =============================================================
DWORD g_lightColor = 0x00E6DCD7;
DWORD g_darkColor  = 0x004E3D33;

bool g_isDarkTheme = false;
std::atomic<bool> g_wwlibHooked{false};

void* pOrig_DrawMathShd = nullptr;

// Store all scanned hardcoded color addresses (because multiple branches in the function may use the same color)
std::vector<void*> g_colorInstructions;

#ifdef _WIN64
    #define WH_CALLCONV
    #define SYM_SetDarkMode L"?SetDarkMode@DarkModeState@@QEAAX_N0K@Z"
    #define SYM_DrawMathShd L"?DrawMathShd@@YAXPEAUols@PTLS7@@AEAVRT@@PEBUMTXLI@@AEBUPT@@PEBVlsrun@2@PEBUtagLSPOINT@2@PEBUheights@2@JKPEBURC@@@Z"
#else
    #define WH_CALLCONV __thiscall
    #define SYM_SetDarkMode L"?SetDarkMode@DarkModeState@@QAEX_N0K@Z"
    #define SYM_DrawMathShd L"?DrawMathShd@@YGXPAUols@PTLS7@@AAVRT@@PBUMTXLI@@ABUPT@@PBVlsrun@2@PBUtagLSPOINT@2@PBUheights@2@JKPBURC@@@Z"
#endif

DWORD ParseHexColor(PCWSTR hexStr, DWORD defaultColor) {
    // 1. Null pointer check
    if (!hexStr || hexStr[0] == L'\0') return defaultColor;
    
    // 2. Skip the optional '#' prefix
    if (hexStr[0] == L'#') {
        hexStr++; 
    }
    
    // 3. Strict length check: must be exactly 6 characters (RRGGBB)
    if (wcslen(hexStr) != 6) {
        Wh_Log(L"wcslen(hexStr) != 6");
        return defaultColor;
    }
    
    // 4. Strict character check: must be valid hexadecimal digits
    for (int i = 0; i < 6; i++) {
        if (!iswxdigit(hexStr[i])) {
            return defaultColor; 
        }
    }
    
    // 5. Parse the string; the result is an integer in 0x00RRGGBB format
    DWORD rrggbb = (DWORD)wcstoul(hexStr, nullptr, 16);
    
    // 6. Extract the R, G, and B channels
    // rrggbb >> 16 yields 0x000000RR, then mask with 0xFF to keep it clean
    DWORD r = (rrggbb >> 16) & 0xFF; 
    DWORD g = (rrggbb >> 8)  & 0xFF;
    DWORD b = rrggbb & 0xFF;
    
    // 7. Recombine into 0x00BBGGRR format and return
    return (b << 16) | (g << 8) | r;
}

void LoadSettings() {
    PCWSTR lightStr = Wh_GetStringSetting(L"lightModeColor");
    g_lightColor = ParseHexColor(lightStr, 0x00E6DCD7);
    if (lightStr) Wh_FreeStringSetting(lightStr);

    PCWSTR darkStr = Wh_GetStringSetting(L"darkModeColor");
    g_darkColor = ParseHexColor(darkStr, 0x004E3D33);
    if (darkStr) Wh_FreeStringSetting(darkStr);
    
    Wh_Log(L"[Settings] Colors loaded. Light: 0x%08X, Dark: 0x%08X", g_lightColor, g_darkColor);
}

// =============================================================
// Memory Patch Logic
// =============================================================
void ApplyColorPatch(bool isDark) {
    if (g_colorInstructions.empty()) return;

    // Use the color loaded from settings instead of a hardcoded value
    DWORD newColor = isDark ? g_darkColor : g_lightColor; 
    
    // Iterate over all discovered instruction addresses and replace them all
    for (void* pInstr : g_colorInstructions) {
        DWORD oldProtect;
        if (VirtualProtect(pInstr, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            *(DWORD*)pInstr = newColor;
            VirtualProtect(pInstr, sizeof(DWORD), oldProtect, &oldProtect);
            Wh_Log(L"[Patch] Successfully patched shade color at %p to: 0x%08X", pInstr, newColor);
        } else {
            Wh_Log(L"[Error] VirtualProtect failed on address %p", pInstr);
        }
    }
}

// Ensure 1-byte struct packing to avoid padding when storing to local storage
#pragma pack(push, 1)
struct PatchCacheEntry {
    DWORD offset;       // Offset of the instruction relative to the start of DrawMathShd
    BYTE  instrLen;     // Total instruction length
    BYTE  opcodes[11];  // Opcode bytes after removing the immediate value (max length 15 - 4 = 11)
};
#pragma pack(pop)

#define CACHE_KEY_OFFSETS L"DrawMathShd_ColorPatchCache_v1"

bool TryFastPathPatch() {
    PatchCacheEntry cache[10]; // Assume at most 10 locations
    size_t bytesRead = Wh_GetBinaryValue(CACHE_KEY_OFFSETS, cache, sizeof(cache));

    size_t count = bytesRead / sizeof(PatchCacheEntry);
    Wh_Log(L"Cache count: %d", count);
    if (count == 0 || bytesRead % sizeof(PatchCacheEntry) != 0) {
        Wh_Log(L"No cache found or size mismatch, exiting.");
        return false; // No cache exists or the size does not match
    }

    std::vector<void*> tempColorPointers;

    for (size_t i = 0; i < count; i++) {
        BYTE* pInstrStart = (BYTE*)pOrig_DrawMathShd + cache[i].offset;

        // 1. Extremely strict prefix validation: the opcode, registers, and displacement must match exactly
        if (memcmp(pInstrStart, cache[i].opcodes, cache[i].instrLen - 4) != 0) {
            Wh_Log(L"[Cache] Opcode mismatch at offset 0x%X. Office updated?", cache[i].offset);
            return false;
        }

        // 2. Double check: inspect the last 4 bytes of the instruction
        // They are either the original light color or the dark color left by the previous hot reload
        DWORD currentColor = *(DWORD*)(pInstrStart + cache[i].instrLen - 4);

        if (currentColor != g_lightColor && currentColor != g_darkColor) {
            Wh_Log(L"[Cache] Color mismatch at offset 0x%X (found %08X).", cache[i].offset, currentColor);
            return false;
        }

        // Validation passed perfectly! Record the memory address to patch (directly pointing to the last 4 bytes)
        tempColorPointers.push_back(pInstrStart + cache[i].instrLen - 4);
    }

    g_colorInstructions = tempColorPointers;
    Wh_Log(L"[Cache] Fast path verified %Id locations using raw opcode match!", count);

    ApplyColorPatch(g_isDarkTheme);
    return true;
}

void FindAndPatchColorWithDisasm() {
    if (!pOrig_DrawMathShd) return;

    if (TryFastPathPatch()) return; // If the fast path succeeds, we are done

    BYTE* p = (BYTE*)pOrig_DrawMathShd;
    Wh_Log(L"[Scanner] Starting slow path disassembly scan...");

    g_colorInstructions.clear();
    std::vector<PatchCacheEntry> cacheToSave; // Used to build new cache data

    size_t total_bytes = 0;
    while (total_bytes < 2048) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) break;

        if (result.length >= 4) {
            size_t j = result.length - 4;
            if (p[j] == 0xD7 && p[j+1] == 0xDC && p[j+2] == 0xE6 && p[j+3] == 0x00) {

                g_colorInstructions.push_back(&p[j]);

                // Build a cache entry
                PatchCacheEntry entry = {0};
                entry.offset = (DWORD)total_bytes;
                entry.instrLen = result.length;
                // Copy the prefix bytes after removing the last 4 bytes (the color)
                memcpy(entry.opcodes, p, result.length - 4);

                cacheToSave.push_back(entry);
                Wh_Log(L"[Scanner] Found at +0x%X, cached %d bytes of opcode.", total_bytes, result.length - 4);
            }
        }
        
#ifdef _WIN64
        // For 64-bit architecture, the function epilogue is typically a simple `ret` instruction with opcode 0xC3. When we encounter this opcode, it indicates that we've reached the end of the function, and we can stop scanning further.
        if (result.length == 1 && p[0] == 0xC3) {
            Wh_Log(L"[Scanner] Reached function epilogue (ret) at offset +0x%X. Scan complete.", total_bytes);
            break;
        }
#else
        // For 32-bit architecture, the function epilogue often consists of a `retn 20h` instruction, which has the opcode sequence `C2 20 00`. When we encounter this sequence, it indicates that we've reached the end of the function, and we can stop scanning further.
        if (result.length == 3 && p[0] == 0xC2 && p[1] == 0x20 && p[2] == 0x00) {
            Wh_Log(L"[Scanner] Reached function epilogue (retn 20h) at offset +0x%X. Scan complete.", total_bytes);
            break;
        }
#endif
        p += result.length;
        total_bytes += result.length;
    }

    if (!g_colorInstructions.empty()) {
        ApplyColorPatch(g_isDarkTheme);
        Wh_Log(L"!g_colorInstructions.empty");
        Wh_Log(L"[Scanner] Saving %Id exact opcode patterns to cache.", cacheToSave.size());
        // Write the struct array directly to local storage
        // If this fails, run Word as administrator!!
        if (Wh_SetBinaryValue(CACHE_KEY_OFFSETS, cacheToSave.data(), cacheToSave.size() * sizeof(PatchCacheEntry))) {
            Wh_Log(L"[Scanner] Saved %Id exact opcode patterns to cache.", cacheToSave.size());
        }
    }
}

// =============================================================
// DarkModeState::SetDarkMode Hook
// =============================================================
typedef void (WH_CALLCONV *SetDarkMode_t)(void* pThis, bool isDark, bool a2, unsigned long a3);
SetDarkMode_t pOrig_SetDarkMode = nullptr;

void WH_CALLCONV Hook_SetDarkMode(void* pThis, bool isDark, bool a2, unsigned long a3) {
    g_isDarkTheme = isDark;
    Wh_Log(L"[Theme] SetDarkMode Intercepted! New State: %d", isDark);

    // When switching between light and dark themes, update the in-memory drawing color at the same time
    ApplyColorPatch(isDark);

    pOrig_SetDarkMode(pThis, isDark, a2, a3);
}

// =============================================================
// Symbol Hooking Loader
// =============================================================
void ScanAndHookWwlib() {
    HMODULE hWwlib = GetModuleHandleW(L"wwlib.dll");
    if (!hWwlib || g_wwlibHooked.exchange(true)) return;
    
    // wwlib.dll
    WindhawkUtils::SYMBOL_HOOK wwlibHook[] = {
        {
            // SetDarkMode symbol
            { SYM_SetDarkMode },
            (void**)&pOrig_SetDarkMode,
            (void*)Hook_SetDarkMode,
            false
        },
        {
            // The full DrawMathShd symbol and the fallback short symbol are written directly in the inline initializer list
            { SYM_DrawMathShd },
            (void**)&pOrig_DrawMathShd,
            (void*)nullptr, // <--- Important: cast to (void*) to fix template type deduction errors
            false
        }
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {0};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE; // <--- Critical fix: this must remain TRUE, otherwise all mangled-name matching fails!
    options.onlineCacheUrl = L"";

    Wh_Log(L"[Init] Attempting to hook wwlib.dll...");
    if (WindhawkUtils::HookSymbols(hWwlib, wwlibHook, ARRAYSIZE(wwlibHook), &options)) {
        Wh_ApplyHookOperations();
        Wh_Log(L"[Success] wwlib.dll symbols resolved successfully.");

        // Symbol resolution completed; start the brute-force memory scan
        FindAndPatchColorWithDisasm();
    } else {
        Wh_Log(L"[Error] Failed to resolve symbols in wwlib.dll.");
        g_wwlibHooked = false;
    }
}

// =============================================================
// Late-Load Catching
// =============================================================
typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *LoadLibraryW_t)(LPCWSTR);
LoadLibraryExW_t pOrig_LoadLibraryExW = nullptr;
LoadLibraryW_t pOrig_LoadLibraryW = nullptr;

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hMod = pOrig_LoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hMod && lpLibFileName && StrStrIW(lpLibFileName, L"wwlib.dll")) {
        ScanAndHookWwlib();
    }
    return hMod;
}

HMODULE WINAPI Hook_LoadLibraryW(LPCWSTR lpLibFileName) {
    HMODULE hMod = pOrig_LoadLibraryW(lpLibFileName);
    if (hMod && lpLibFileName && StrStrIW(lpLibFileName, L"wwlib.dll")) {
        ScanAndHookWwlib();
    }
    return hMod;
}

// =============================================================
// Lifecycle
// =============================================================
BOOL Wh_ModInit() {
    Wh_Log(L"OMath Shade Patch Loaded. Initializing...");

    // Load settings during initialization
    LoadSettings();

    HMODULE hKernel = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernel) hKernel = GetModuleHandleW(L"kernel32.dll");

    if (hKernel) {
        void* pLoadLibraryExW = (void*)GetProcAddress(hKernel, "LoadLibraryExW");
        if (pLoadLibraryExW) Wh_SetFunctionHook(pLoadLibraryExW, (void*)Hook_LoadLibraryExW, (void**)&pOrig_LoadLibraryExW);

        void* pLoadLibraryW = (void*)GetProcAddress(hKernel, "LoadLibraryW");
        if (pLoadLibraryW) Wh_SetFunctionHook(pLoadLibraryW, (void*)Hook_LoadLibraryW, (void**)&pOrig_LoadLibraryW);
    }

    if (GetModuleHandleW(L"wwlib.dll")) {
        ScanAndHookWwlib();
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"OMath Shade Patch Unloaded. Restoring original colors...");
    DWORD originalOfficeColor = 0x00E6DCD7; 
    for (void* pInstr : g_colorInstructions) {
        DWORD oldProtect;
        if (VirtualProtect(pInstr, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            *(DWORD*)pInstr = originalOfficeColor;
            VirtualProtect(pInstr, sizeof(DWORD), oldProtect, &oldProtect);
        }
    }
}

// Listen for the user changing settings in the Windhawk panel
void Wh_ModSettingsChanged() {
    Wh_Log(L"[Settings] Settings changed by user, reloading...");
    LoadSettings();
    
    // If the memory addresses have already been discovered, patch again with the new color to enable hot updates
    if (!g_colorInstructions.empty()) {
        ApplyColorPatch(g_isDarkTheme);
    }
}