// ==WindhawkMod==
// @id            word-pdf-lossless-export
// @name          Word PDF Lossless Export
// @name:zh-CN    Word 图像无损导出 PDF
// @description   Forces Word to export PDFs with lossless image quality
// @description:zh-CN   强制 Word 导出 PDF 时使用 100% 无损图像质量
// @version       1.2.0
// @author        Joe Ye
// @github        https://github.com/JoeYe-233
// @include       winword.exe
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Word PDF Lossless Export

*Downsampling and JPEG lossy compression are now over, for good.*

Microsoft Word has a notorious, long-standing issue when exporting documents to PDF (via `File -> Export -> Create PDF/XPS`): it aggressively downsamples and re-compresses images. Even if you enable "Do not compress images in file" and select "High fidelity" in Word's options, the internal PDF rendering engine (`mso.dll`) still runs a hidden optimization pass. It calculates the physical dimensions of the image on the page, *(almost always) decides* your high-resolution image is "too big", downscales it via GDI+, and forces a secondary JPEG compression. This ruins pixel-perfect diagrams, degrades high-res photos, and introduces irreversible compression artifacts. Which is especially frustrating and super annoying because Word's PDF export is often the go-to solution for sharing documents. We expect it to be every bit as good as the original.

This mod performs a deep, memory-level intervention on Word's internal graphics rendering pipeline to bypass these limitations. It intercepts the core image resolution calculator (`DOCEXIMAGE::HrComputeSize`) to prevent dimensional downscaling, and hooks the output validator (`DOCEXIMAGE::HrCheckForLosslessOutput`) to force the engine to use a lossless FLATE (Zlib) stream instead of the default JPEG encoder.

### ✨ Key Improvements

  * **Pixel-Perfect Pictures:** Solid PNG images, JPEGs and BMPs, etc. are exported with absolute 100% pixel accuracy. No quality loss, no artifacts. PNGs with transparency are guaranteed > 98% pixel accuracy (this is due to limits of GDI+, which does not handle alpha channel perfectly, but it's still a huge improvement over what we currently have. For detailed information, see Test Results below).
  * **True Lossless Quality:** Bypasses Word's forced secondary JPEG compression entirely, preserving the exact quality of your original high-resolution inserts.
  * **Overrides Broken Settings:** Bypasses the hardcoded internal DPI limits that Word's built-in *so called* "High fidelity" setting fails to disable.
  * **Cross-Architecture Support:** Dynamically adapts to both 64-bit and 32-bit versions of Office using precise memory offsets and calling conventions.

---

### ⚙️ Hook Method & Symbol Download Instructions

This mod now features a **Hook Method** option in the settings, meaning the large PDB symbol file is no longer strictly mandatory. Both methods prioritize a local cache for optimal speed and will automatically fall back to the other if one fails:

* **Pattern First (Faster):** Prioritizes AOB pattern scanning. It does not require downloading PDBs, though it may be more susceptible to breaking with Office updates.
* **PDB First (More Robust):** Prioritizes symbol-based hooking. If you select this (or if the mod uses it as a fallback), Windhawk needs the `mso.dll` PDB symbol (~90MB) and will download it automatically when you first launch Word.
  * *If downloading PDB:* Please watch the popup in the bottom right corner of your screen (it will show progress like "Loading symbols... 0% (mso.dll)"). Wait patiently until it reaches 100% and disappears. If it fails, please switch your network and try again.

**⚠️ IMPORTANT:** Whenever you modify the Hook Method option, or after the PDB finishes downloading for the first time, please **relaunch Word AS ADMINISTRATOR**. This writes the target functions to the local cache, which drastically speeds up Word's launch times later on.

---

### 🖼️ Recommended Word Settings for Lossless Images

To ensure Word does not degrade your picture quality, please configure the following built-in settings:

1. Navigate to **File** > **Options** > **Advanced**.
2. Scroll down to the **Image size and quality** section.
3. *Recommended: In the combobox right next to "Image size and quality", select **All New Documents** to apply these settings to all future documents created on your PC.
4. Tick the box for **"Do not compress images in file"**.
5. Select **High fidelity** as the default resolution.

**Note:** If your document was saved *before* this option was applied, its existing images may have already been compressed. You may need to replace and re-insert all pictures, then re-save the document to ensure the images stored within the Word document itself (and not just the exported PDF) are truly lossless.

---

### 🧪 Test Results and Verifications

* **Lossless performance guaranteed for JPEGs, BMPs, and other non-transparent formats**: 100% lossless pixel-perfect accuracy. No downscaling, no compression artifacts or quality loss.

* **Lossless performance guaranteed for PNGs**:
  * 100% lossless for PNGs that **do not contain** transparent regions. (same as above, no downscaling, no compression artifacts or quality loss).
  * > 98.4% lossless for PNGs that **contain** transparent regions. (No downscaling, no compression artifacts, and negligible quality loss). This is because of how GDI+ handles transparent images (Pre-multiplied Alpha and Float to Integer rounding error). Combined, these may cause up to ±4 drift out of 255 (±1.6%) on each of 3 RGB channels. Also, RGB values for pixels on complete transparent regions (i.e., alpha strictly equals 0) are discarded by GDI+ for better performance. (which is actually a good thing as it increases redundancy, thus decreasing size of end product).

* Also, pictures embedded in SVGs are lossless too, because the mod hooks the core image processing pipeline, which applies to all images regardless of their source.

Lossless picture extractor of PDF files are also provided to help you verify the output PDF files. You can get the Python script [here](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/PDF_Image_Extractor_Lossless.py).

### ❌ Before (Input vs Output)

![Before](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-before.png)

(Image courtesy of [Nicky ❤️🌿🐞🌿❤️](https://pixabay.com/photos/winter-nature-trees-snow-cold-6762640/) from Pixabay)

### ✅ After (Input vs Output)

![After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-after.png)

### 🔎 Before vs After at 800% Zoom

![Before vs After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-before-vs-after-new.png)

(Notice the severe downscaling and compression artifacts in the "Before" image, which are completely gone in the "After" image.)

### 🖼️ PNG with Transparency Test

(Image courtesy of [Sunriseforever](https://pixabay.com/illustrations/fruit-nutrition-organic-healthy-6925630/) from Pixabay)

Before (input vs output):

![Before](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-png-before.png)

After (input vs output):

![After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-png-after.png)

(Notice the pixel value difference of A=0 (fully transparent) pixels, which is caused by GDI+'s handling of transparent images. This is expected.)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hook_method: pdb
  $name: Hook Method
  $name:zh-CN: Hook 方式
  $description: |
    Determines the primary method for locating target functions in mso.dll.
    * The "PDB First" option prioritizes symbol-based hooking using PDB files, which is more robust against Office updates but relies on successful PDB retrieval.
    * The "Pattern First" option prioritizes AOB pattern scanning, which is faster and does not require PDBs but may be more susceptible to breaking with Office updates. 
    Both methods will fall back to the other if the primary method fails, and both will always attempt to use the local cache first for optimal performance.
    
    Note: After changing this setting, please restart Word AS ADMINISTRATOR to ensure the cache is updated correctly.
  $description:zh-CN: |
    选择首选的函数定位方式:
    * “PDB 优先”选项优先使用 PDB 符号文件进行符号定位，具有更强的抗 Office 更新能力，但依赖于成功获取 PDB。
    * “特征码优先”选项优先使用 AOB 特征码扫描进行函数定位，速度更快且不依赖 PDB，但可能更容易受到 Office 更新的影响。
    两者互为备用，且均会优先读取之前保存的本地极速缓存
    
    注意：修改选项后，需要重新【以管理员身份】启动 Word 以写入缓存
  $options:
    - pdb: PDB First (Pattern as Fallback)
    - pattern: Pattern First (PDB as Fallback)
  $options:zh-CN:
    - pdb: PDB 优先 (特征码作为备用)
    - pattern: 特征码优先 (PDB 作为备用)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <atomic>

std::atomic<bool> g_bMsoHooked{false};

#ifdef _WIN64
    // 64-bit Office offsets
    #define OFFSET_ORIG_W 48
    #define OFFSET_ORIG_H 52
    #define OFFSET_TARG_W 120
    #define OFFSET_TARG_H 124
    #define OFFSET_FLAG   355
    #define CC_CALL __fastcall
#else
    // 32-bit Office offsets
    #define OFFSET_ORIG_W 44
    #define OFFSET_ORIG_H 48
    #define OFFSET_TARG_W 104
    #define OFFSET_TARG_H 108
    #define OFFSET_FLAG   251
    #define CC_CALL __thiscall
#endif

// =============================================================
// Hook HrComputeSize to force target size = original size and clear the resample flag, ensuring Word never performs downsampling. (Step 1: Keep original size)
// =============================================================
typedef __int64 (CC_CALL *HrComputeSize_t)(void* pThis, float* a2, void* a3);
HrComputeSize_t pOrig_HrComputeSize = nullptr;

__int64 CC_CALL Hook_HrComputeSize(void* pThis, float* a2, void* a3) {
    __int64 res = pOrig_HrComputeSize(pThis, a2, a3);
    if (pThis) {
        int orig_w = *((int*)((char*)pThis + OFFSET_ORIG_W));
        int orig_h = *((int*)((char*)pThis + OFFSET_ORIG_H));
        int target_w = *((int*)((char*)pThis + OFFSET_TARG_W));
        int target_h = *((int*)((char*)pThis + OFFSET_TARG_H));
        bool needs_resample = *((bool*)((char*)pThis + OFFSET_FLAG));

        if (needs_resample || target_w != orig_w || target_h != orig_h) {
            *((int*)((char*)pThis + OFFSET_TARG_W)) = orig_w;
            *((int*)((char*)pThis + OFFSET_TARG_H)) = orig_h;
            *((bool*)((char*)pThis + OFFSET_FLAG)) = false;
            Wh_Log(L"[HrComputeSize] Surgery successful! Flag cleared. Target forced to %dx%d.", orig_w, orig_h);
        }
    }
    return res;
}

// =============================================================
// Hook HrCheckForLosslessOutput to intercept any attempt to use JPEG compression and force it to use lossless FLATE instead. (Step 2: Intercept JPEG usage)
// =============================================================

/* LOSSLESS_FLAG_OFFSET is based on the following reverse engineering of DOCEXIMAGE::HrCheckForLosslessOutput.

According to the last lines of the function (similar to following pseudo-code), the return value is determined by a byte flag that is set based on the result of ImageAnalyzer::FUseJpeg. The flag is set to 1 (FLATE) if FUseJpeg returns 0, and set to 2 (JPEG) if FUseJpeg returns 1. By hooking this function and forcing it to 1 instead of 2, we can effectively force Word to use FLATE compression even when it thinks JPEG would be acceptable.
---------------------------- Pseudo-code ----------------------------
*((_DWORD *)this + 56) = (unsigned __int8)ImageAnalyzer::FUseJpeg(
                                            *((struct Gdiplus::GpBitmap **)this + 4 * *((int *)this + 50) + 13),
                                            v10,
                                            v4,
                                            v9,
                                            *((_BYTE *)this + 359)) + 1;
---------------------------------------------------------------------
Then, LOSSLESS_FLAG_OFFSET can be determined as follows:

64-bit platform：*((_DWORD *)this + 56) ---> LOSSLESS_FLAG_OFFSET = 56 * 4 = 224
32-bit platform：*((_DWORD *)this + 41) ---> LOSSLESS_FLAG_OFFSET = 41 * 4 = 164
*/

// =============================================================
// Architecture-dependent offsets and symbol signatures
// =============================================================
#ifdef _WIN64
    // 64-bit Mangled Names
    #define SYM_HrComputeSize          L"?HrComputeSize@DOCEXIMAGE@@AEAAJPEAMPEBVPointF@Gdiplus@@@Z"
    #define SYM_HrCheckForLossless     L"?HrCheckForLosslessOutput@DOCEXIMAGE@@MEBAJH@Z"
    #define LOSSLESS_FLAG_OFFSET 224
#else
    // 32-bit Mangled Names
    #define SYM_HrComputeSize          L"?HrComputeSize@DOCEXIMAGE@@AAEJPAMPBVPointF@Gdiplus@@@Z"
    #define SYM_HrCheckForLossless     L"?HrCheckForLosslessOutput@DOCEXIMAGE@@MBEJH@Z"
    #define LOSSLESS_FLAG_OFFSET 164
#endif

typedef int (CC_CALL *HrCheckForLosslessOutput_t)(void* pThis, int a1);
HrCheckForLosslessOutput_t pOrig_HrCheckForLosslessOutput = nullptr;

int CC_CALL Hook_HrCheckForLosslessOutput(void* pThis, int a1) {
    int res = pOrig_HrCheckForLosslessOutput(pThis, a1);

    if (pThis) {
        int* pLosslessFlag = (int*)((char*)pThis + LOSSLESS_FLAG_OFFSET);
        // If Word ever decides to use JPEG (2), we will forcibly change it to FLATE (1)
        if (*pLosslessFlag == 2) {
            *pLosslessFlag = 1;
            Wh_Log(L"[HrCheckForLosslessOutput] Attempt intercepted! Forced Word to use FLATE(1) instead of JPEG(2).");
        }
    }
    return res;
}

// =============================================================
// Unified Cache & Core Routing
// =============================================================

// Updated the cache key to avoid loading older cache data with the wrong format.
#define CACHE_KEY_MSO_HOOKS L"WordLossless_MsoHooksCache"

#pragma pack(push, 1)
struct HookStateCache {
    DWORD pdbFailCount;             // Number of consecutive PDB hook failures.
    DWORD offset_HrComputeSize;
    BYTE  prefix_HrComputeSize[8];
    DWORD offset_HrCheckForLosslessOutput;
    BYTE  prefix_HrCheckForLosslessOutput[8];
    DWORD lastHookMethod;           // Records the strategy used when this cache was generated (0=PDB, 1=Pattern)
};
#pragma pack(pop)

HookStateCache g_cache = {0};
bool g_cacheLoaded = false;

void LoadCache() {
    size_t bytesRead = Wh_GetBinaryValue(CACHE_KEY_MSO_HOOKS, &g_cache, sizeof(g_cache));
    if (bytesRead == sizeof(g_cache)) {
        g_cacheLoaded = true;
        Wh_Log(L"[Cache] Loaded successfully. PDB Fail Count: %lu", g_cache.pdbFailCount);
    } else {
        Wh_Log(L"[Cache] No valid cache found (Read %zu bytes). Initializing new.", bytesRead);
        memset(&g_cache, 0, sizeof(g_cache));
    }
}

void SaveCache() {
    Wh_SetBinaryValue(CACHE_KEY_MSO_HOOKS, &g_cache, sizeof(g_cache));
}

bool CheckPattern(const BYTE* pData, const BYTE* bMask, const char* szMask) {
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return false;
    return (*szMask) == '\0';
}

BYTE* FindPatternInTextSection(HMODULE hMod, const BYTE* bMask, const char* szMask) {
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hMod;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hMod + dosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

    for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        if (memcmp(section[i].Name, ".text", 5) == 0) {
            BYTE* start = (BYTE*)hMod + section[i].VirtualAddress;
            DWORD size = section[i].SizeOfRawData;

            for (DWORD j = 0; j < size; j++) {
                if (CheckPattern(start + j, bMask, szMask)) {
                    return start + j;
                }
            }
        }
    }
    return nullptr;
}

bool TryFastPathHooks(HMODULE hMso) {
    if (!g_cacheLoaded || g_cache.offset_HrComputeSize == 0 || g_cache.offset_HrCheckForLosslessOutput == 0) {
        return false;
    }

    BYTE* pComputeSize = (BYTE*)hMso + g_cache.offset_HrComputeSize;
    BYTE* pCheckLossless = (BYTE*)hMso + g_cache.offset_HrCheckForLosslessOutput;

    // Strict validation: Verify the first 8 bytes haven't changed due to Office updates
    if (memcmp(pComputeSize, g_cache.prefix_HrComputeSize, 8) != 0 ||
        memcmp(pCheckLossless, g_cache.prefix_HrCheckForLosslessOutput, 8) != 0) {
        Wh_Log(L"[Cache] Opcode prefix mismatch. Office updated? Forcing full scan/PDB.");
        
        // Office has updated, so reset the PDB failure counter and give PDB another chance.
        g_cache.pdbFailCount = 0;
        SaveCache();
        return false;
    }

    // Validation passed, apply hooks
    Wh_SetFunctionHook((void*)pComputeSize, (void*)Hook_HrComputeSize, (void**)&pOrig_HrComputeSize);
    Wh_SetFunctionHook((void*)pCheckLossless, (void*)Hook_HrCheckForLosslessOutput, (void**)&pOrig_HrCheckForLosslessOutput);
    
    // Important: commit the hook operations to memory.
    Wh_ApplyHookOperations();

    Wh_Log(L"[FastPath] Fast path successful! Hooks applied via cached offsets.");
    return true;
}

// -------------------------------------------------------------
// Route 1: PDB Hooking
// -------------------------------------------------------------
bool RunPdbRoute(HMODULE hMso) {
    if (g_cache.pdbFailCount >= 2) {
        Wh_Log(L"[PDB Route] PDB failed %lu times previously. Skipping PDB.", g_cache.pdbFailCount);
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK msoDllHook[] = {
        {
            // private: long __thiscall DOCEXIMAGE::HrComputeSize(float *,class Gdiplus::PointF const *)
            // (Note: MSVC DIA quirk adds a trailing space after "const *)" in undecorated names)
            { SYM_HrComputeSize },
            (void**)&pOrig_HrComputeSize,
            (void*)Hook_HrComputeSize,
            false
        },
        {
            // protected: virtual long __thiscall DOCEXIMAGE::HrCheckForLosslessOutput(int)const
            // (Note: MSVC DIA quirk adds a trailing space after "const" in undecorated names,
            // using exact mangled names with noUndecoratedSymbols=TRUE bypasses this issue entirely)
            { SYM_HrCheckForLossless },
            (void**)&pOrig_HrCheckForLosslessOutput,
            (void*)Hook_HrCheckForLosslessOutput,
            false
        }
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {0};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    options.onlineCacheUrl = L""; // Gracefully stops it from trying to get online symbols. We will rely entirely on local symbol cache, which is populated when you launch Word as administrator after installing the mod and waiting for symbol download to complete.

    Wh_Log(L"[PDB Route] Attempting to hook mso.dll via PDB symbols...");
    if (WindhawkUtils::HookSymbols(hMso, msoDllHook, ARRAYSIZE(msoDllHook), &options)) {
        Wh_ApplyHookOperations();
        Wh_Log(L"[Success] Hooks applied successfully via PDB!");
        
        // Clear the failure counter on success.
        if (g_cache.pdbFailCount != 0) {
            g_cache.pdbFailCount = 0;
            SaveCache();
        }
        return true;
    } else {
        g_cache.pdbFailCount++;
        Wh_Log(L"[Warning] PDB symbol hook failed. Fail Count: %lu", g_cache.pdbFailCount);
        SaveCache();
        return false;
    }
}

// -------------------------------------------------------------
// Route 2: Pattern Scanning
// -------------------------------------------------------------
bool RunPatternRoute(HMODULE hMso) {
    Wh_Log(L"[Pattern Route] Starting full AOB pattern scan in mso.dll...");

#ifdef _WIN64
    const BYTE sig_ComputeSize[] = {0x48, 0x8B, 0xC4, 0x48, 0x89, 0x58, 0x10, 0x48, 0x89, 0x70, 0x18, 0x55, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x48, 0x8B, 0xEC};
    const char mask_ComputeSize[] = "xxxxxxxxxxxxxxxxxxxxxx";

    const BYTE sig_CheckLossless[] = {0x48, 0x89, 0x5C, 0x24, 0x10, 0x55, 0x56, 0x57, 0x48, 0x83, 0xEC, 0x30, 0x33, 0xED, 0x8B, 0xF2, 0x48, 0x8B, 0xD9, 0x39, 0xA9};
    const char mask_CheckLossless[] = "xxxxxxxxxxxxxxxxxxxxx";
#else
    const BYTE sig_ComputeSize[] = {
        0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8, 0x83, 0xEC, 0x00, 0x53, 0x56, 0x8B, 
        0xF1, 0xC7, 0x44, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xDB, 0x43, 0x57
    };
    const char mask_ComputeSize[] = "xxxxxxxx?xxxxxxx?xxxxxxxx";

    const BYTE sig_CheckLossless[] = {
        0x55, 0x8B, 0xEC, 0x51, 0x56, 0x8B, 0xF1, 0x83, 0xBE, 0xA4, 0x00, 0x00, 
        0x00, 0x00, 0x0F, 0x85, 0x00, 0x00, 0x00, 0x00, 0x53, 0x57, 0x6A, 0x00
    };
    const char mask_CheckLossless[] = "xxxxxxxxxxxxxxxx????xxxx";
#endif

    BYTE* pComputeSize = FindPatternInTextSection(hMso, sig_ComputeSize, mask_ComputeSize);
    BYTE* pCheckLossless = FindPatternInTextSection(hMso, sig_CheckLossless, mask_CheckLossless);

    if (pComputeSize && pCheckLossless) {
        Wh_Log(L"[Pattern Route] Patterns found! ComputeSize at %p, CheckLossless at %p", pComputeSize, pCheckLossless);

        // Update cache data
        g_cache.offset_HrComputeSize = (DWORD)(pComputeSize - (BYTE*)hMso);
        memcpy(g_cache.prefix_HrComputeSize, pComputeSize, 8);
        
        g_cache.offset_HrCheckForLosslessOutput = (DWORD)(pCheckLossless - (BYTE*)hMso);
        memcpy(g_cache.prefix_HrCheckForLosslessOutput, pCheckLossless, 8);

        SaveCache();
        Wh_Log(L"[Success] Offsets and 8-byte prefixes saved to local cache via Pattern Route.");
        
        Wh_SetFunctionHook((void*)pComputeSize, (void*)Hook_HrComputeSize, (void**)&pOrig_HrComputeSize);
        Wh_SetFunctionHook((void*)pCheckLossless, (void*)Hook_HrCheckForLosslessOutput, (void**)&pOrig_HrCheckForLosslessOutput);

        // Important: commit the hook operations to memory.
        Wh_ApplyHookOperations();
        
        return true;
    }

    Wh_Log(L"[Error] Pattern scan failed to locate functions.");
    return false;
}

// =============================================================
// Core Loader
// =============================================================
void ScanAndHookMso() {
    HMODULE hMso = GetModuleHandleW(L"mso.dll");
    if (!hMso || g_bMsoHooked.exchange(true)) return;

    // 1. Initialize and load the global cache.
    LoadCache();

    // 2. Read the user's preferred setting (string value).
    LPCWSTR pszHookMethod = Wh_GetStringSetting(L"hook_method");
    bool isPdbFirst = (wcscmp(pszHookMethod, L"pdb") == 0);
    Wh_FreeStringSetting(pszHookMethod); // Free the allocated string.
    
    DWORD currentMethodEnum = isPdbFirst ? 0 : 1;

    // 3. [Core Fix]: Check if settings were modified while Word was closed
    // If the current settings are different from the last settings recorded in the cache, it means the user changed preferences while offline
    if (g_cacheLoaded && g_cache.lastHookMethod != currentMethodEnum) {
        Wh_Log(L"[Init] Setting changed while Word was closed. Wiping cache to force re-evaluation.");
        memset(&g_cache, 0, sizeof(g_cache)); // Clear the cache completely
        g_cache.lastHookMethod = currentMethodEnum; // Record the new settings
        SaveCache();
        // Since the cache has been cleared, subsequent TryFastPathHooks will naturally become invalid
    } else {
        // Synchronize as well to ensure the newly generated cache can correctly record the current state
        g_cache.lastHookMethod = currentMethodEnum;
    }

    // 2. Ignore user settings; the fast cache path always has highest priority.
    if (TryFastPathHooks(hMso)) {
        return;
    }



    bool success = false;

    if (isPdbFirst) {
        Wh_Log(L"[Init] Mode: PDB First");
        success = RunPdbRoute(hMso);
        if (!success) {
            Wh_Log(L"[Init] PDB Route failed, falling back to Pattern Route...");
            success = RunPatternRoute(hMso);
        }
    } else {
        Wh_Log(L"[Init] Mode: Pattern First");
        success = RunPatternRoute(hMso);
        if (!success) {
            Wh_Log(L"[Init] Pattern Route failed, falling back to PDB Route...");
            success = RunPdbRoute(hMso);
        }
    }

    // 4. Final validation.
    if (!success) {
        Wh_Log(L"[Fatal] Both PDB and Pattern hooking failed. Mod will not function.");
        // If everything fails, reset state to avoid getting stuck in the failure branch next time.
        g_cache.pdbFailCount = 0;
        SaveCache();
        g_bMsoHooked = false; 
    }
}
// =============================================================
// Intercept LoadLibraryExW to elegantly monitor mso.dll loading
// =============================================================
typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExW_t pOrig_LoadLibraryExW = nullptr;

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = pOrig_LoadLibraryExW(lpLibFileName, hFile, dwFlags);

    if (hModule && lpLibFileName && !g_bMsoHooked.load()) {
        const wchar_t* fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;

        if (_wcsicmp(fileName, L"mso.dll") == 0) {
            ScanAndHookMso();
        }
    }

    return hModule;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Word PDF Lossless Export Ultimate Loaded");

    if (GetModuleHandleW(L"mso.dll")) {
        // If already loaded, hook immediately.
        ScanAndHookMso();
    } else {
        // Not loaded yet, hook LoadLibrary to stand guard
        Wh_SetFunctionHook((void*)LoadLibraryExW, (void*)Hook_LoadLibraryExW, (void**)&pOrig_LoadLibraryExW);
    }

    return TRUE;
}

void Wh_ModUninit() {}

// Listen for setting change callbacks so that switching preferences takes effect immediately
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"[Settings] Preference switched. Wiping cache to force re-evaluation...");

    // Completely clear the entire cache structure (including failure counts and saved offset addresses)
    memset(&g_cache, 0, sizeof(g_cache));

    // Overwrite local files, equivalent to "formatting" the cache
    SaveCache();

    *bReload = TRUE; // Tell Windhawk to reload Mod
    return TRUE;
}