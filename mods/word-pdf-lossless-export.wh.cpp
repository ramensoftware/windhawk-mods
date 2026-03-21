// ==WindhawkMod==
// @id            word-pdf-lossless-export
// @name          Word PDF Lossless Export
// @name:zh-CN    Word 图像无损导出 PDF
// @description   Forces Word to export PDFs with lossless image quality
// @description:zh-CN   强制 Word 导出 PDF 时使用 100% 无损图像质量
// @version       1.1
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

**Key Improvements:**

  * **Pixel-Perfect Pictures:** Solid PNG images, JPEGs and BMPs, etc. are exported with absolute 100% pixel accuracy. No quality loss, no artifacts. PNGs with transparency are guaranteed > 98% pixel accuracy (this is due to limits of GDI+, which does not handle alpha channel perfectly, but it's still a huge improvement over what we currently have. For detailed information, see Test Results below).
  * **True Lossless Quality:** Bypasses Word's forced secondary JPEG compression entirely, preserving the exact quality of your original high-resolution inserts.
  * **Overrides Broken Settings:** Bypasses the hardcoded internal DPI limits that Word's built-in *so called* "High fidelity" setting fails to disable.
  * **Cross-Architecture Support:** Dynamically adapts to both 64-bit and 32-bit versions of Office using precise memory offsets and calling conventions.

*Note: this mod needs pdb symbol of `mso.dll` to work. The symbol file is expected to be quite large (~90MB in size). Windhawk will download it automatically when launching Word first time after you installed the mod (the popup at right bottom corner of your screen, please make sure that it shows percentage like "Loading symbols... 0% (mso.dll)", wait until it reaches 100% and the pop up disappears, otherwise please switch your network and try again) please wait patiently and relaunch Word after it finishes.*

**Attention**: this mod utilizes functions and data structures in `DOCEXIMAGE` class, which is undocumented and is subject to change without notice. If the mod causes crash when exporting PDFs, please **open an issue** at my [GitHub repository](https://github.com/JoeYe-233/windhawk-mods/issues) and provide your version of `mso.dll` (usually located in `C:\Program Files\Microsoft Office\root\vfs\ProgramFilesCommon[X64, X86]\Microsoft Shared\OFFICE16\MSO.DLL` where `[X64, X86]` varies based on your Microsoft Office architecture. For 64-bit Office, usually both X86 and X64 are available, use the X64 one; for 32-bit Office, use the X86 one).

**Test Results and Verifications:**

* **Lossless performance guaranteed for JPEGs, BMPs, and other non-transparent formats**: 100% lossless pixel-perfect accuracy. No downscaling, no compression artifacts or quality loss.

* **Lossless performance guaranteed for PNGs**:
  * 100% lossless for pngs that **do not contain** transparent regions. (same as above, no downscaling, no compression artifacts or quality loss).
  * > 98.4% (absolute visually lossless) for PNGs that **contain** transparent regions. (No downscaling, no compression artifacts, and negligible quality loss). This is because of how GDI+ handles transparent images (Pre-multiplied Alpha and Float to Integer rounding error). Combined, these may cause up to ±4 drift out of 255 (±1.6%) on each of 3 RGB channels. Also, RGB values for pixels on complete transparent regions (i.e., alpha strictly equals 0) are discarded by GDI+ for better performance. (which is actually a good thing as it increases redundancy, thus decreasing size of end product).

* Also, pictures embedded in SVGs are lossless too, because the mod hooks the core image processing pipeline, which applies to all images regardless of their source.

Lossless picture extractor of PDF files are also provided to help you verify the output PDF files. You can get the Python script [here](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/PDF_Image_Extractor_Lossless.py).

### Before (input vs output)

![Before](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-before.png)

(Image courtesy of [Nicky ❤️🌿🐞🌿❤️](https://pixabay.com/photos/winter-nature-trees-snow-cold-6762640/) from Pixabay)
### After (input vs output)

![After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-after.png)

### Before vs After at 800% Zoom

![Before vs After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-before-vs-after.png)

(Notice the severe downscaling and compression artifacts in the "Before" image, which are completely gone in the "After" image.)

### PNG with Transparency Test
(Image courtesy of [Sunriseforever](https://pixabay.com/illustrations/fruit-nutrition-organic-healthy-6925630/) from Pixabay)

Before (input vs output):

![Before](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-png-before.png)

After (input vs output):

![After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-pdf-lossless-export-png-after.png)

(Notice the pixel value difference of A=0 (fully transparent) pixels, which is caused by GDI+'s handling of transparent images. This is expected.)

*/
// ==/WindhawkModReadme==

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
// Core Symbol Hook logic
// =============================================================
void ScanAndHookMso() {
    HMODULE hMso = GetModuleHandleW(L"mso.dll");
    if (!hMso || g_bMsoHooked.exchange(true)) return;

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

    if (WindhawkUtils::HookSymbols(hMso, msoDllHook, ARRAYSIZE(msoDllHook), &options)) {

        // Don't forget to apply the hooks after setting them up, otherwise they won't take effect!
        Wh_ApplyHookOperations();

        Wh_Log(L"[Success] Hooks applied successfully to mso.dll! HrComputeSize and HrCheckForLosslessOutput are now hooked.");
    } else {
        Wh_Log(L"[Error] Failed to hook symbols in mso.dll. HrComputeSize and HrCheckForLosslessOutput are not hooked, and the mod will not work. Please make sure your Office version is supported and the symbol files are properly downloaded.");
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
        // If already loaded, start thread directly
        ScanAndHookMso();
    } else {
        // Not loaded yet, hook LoadLibrary to stand guard
        Wh_SetFunctionHook((void*)LoadLibraryExW, (void*)Hook_LoadLibraryExW, (void**)&pOrig_LoadLibraryExW);
    }

    return TRUE;
}

void Wh_ModUninit() {}