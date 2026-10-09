// ==WindhawkMod==
// @id              visio-hd-thumbnail
// @name            Visio Thumbnail HD Fix
// @name:zh-CN      Visio 缩略图高清修复
// @description     Fix low-resolution thumbnails for Visio documents in File Explorer
// @description:zh-CN 修复资源管理器中 Visio 文档低质量缩略图问题
// @version         1.0.0
// @author          Joe Ye
// @github          https://github.com/JoeYe-233
// @include         visio.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Visio Thumbnail HD Fix
This mod addresses the issue of low-resolution thumbnails for Visio documents in File Explorer. Visio generates thumbnails according to fixed area sizes (i.e., a max total pixel count), which can lead to blurry thumbnails for larger documents. This mod intercepts the thumbnail generation function in `vislib.dll` and scales up the area parameter, enabling Visio to produce higher-resolution thumbnails, resulting in clearer previews in File Explorer.

# Notes
 - This will only take effect on newly saved or modified Visio files, as thumbnails for existing files were generated and saved at the time of their creation or last modification. To update thumbnails for existing files, simply open, do some modifications, and resave them in Visio. (Direct resave without modification may not trigger thumbnail regeneration, as Visio is kind of smart in that regard. Changing text color or adding and then removing a shape should be enough to force thumbnail update.)
 
 - Each file with HD thumbnails might take up extra 10KB of storage space (as of `Scale Multiplier` being set to 4). We consider this a reasonable trade-off for the improved thumbnail quality, especially for complex Visio documents where only clear thumbnails make sense.
 
# Before
![Before](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/visio-hd-thumbnail-before.png)
# After
![After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/visio-hd-thumbnail-after.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scale_multiplier: 4
  $name: Scale Multiplier
  $name:zh-CN: 放大倍率
  $description: "Set the scaling multiplier for the thumbnail.\n(e.g., 4 means both height and width are scaled by 4x, default = 4 which is a good balance between quality and storage cost). Cannot be set to 1 or less."
  $description:zh-CN: "设置缩略图的放大倍数\n例如：设为 4 表示宽和高都放大 4 倍，面积放大 16 倍（默认值 = 4，较好地平衡了质量与文件大小）。请勿设为小于 1 的值。"
*/
// ==/WindhawkModSettings==

#include <windows.h>

// Global variable to store the actual area multiplier (which is the square of the scale multiplier)
int g_areaMultiplier = 16; 

// --- Settings Reader ---
void LoadSettings() {
    int scale = Wh_GetIntSetting(L"scale_multiplier");
    // If the user sets the scale multiplier to 1 or less, we default it to 4 to ensure a noticeable improvement in thumbnail quality without risking failure.
    if (scale <= 1) {
        scale = 4; 
    }
    // Area = (Scale Multiplier)^2, because the area scales with the square of the linear dimensions.
    g_areaMultiplier = scale * scale;
    Wh_Log(L"Settings loaded: Scale Multiplier = %d (Area Multiplier = %d)", scale, g_areaMultiplier);
}

// This function is called when the user changes settings in the Windhawk panel
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}

// --- Pattern Scanning Engine with '?' Wildcard Support ---
void* FindPattern(const wchar_t* moduleName, const char* pattern, const char* mask) {
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (!hModule) return nullptr;

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;

    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) return nullptr;

    BYTE* baseAddress = (BYTE*)hModule;
    DWORD sizeOfImage = pNtHeaders->OptionalHeader.SizeOfImage;
    DWORD patternLength = (DWORD)strlen(mask);

    for (DWORD i = 0; i < sizeOfImage - patternLength; i++) {
        bool found = true;
        for (DWORD j = 0; j < patternLength; j++) {
            // mask[j] == '?' means we ignore this byte (wildcard), otherwise we must match the pattern byte
            if (mask[j] != '?' && baseAddress[i + j] != (BYTE)pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return (void*)(baseAddress + i);
        }
    }
    return nullptr;
}

// =====================================================================
// Hooking Logic for 64-bit (x64) Architecture
// =====================================================================
#ifdef _WIN64

typedef __int64 (__fastcall *GenerateThumbnail_t)(__int64 a1, int a2);
GenerateThumbnail_t pOriginalGenerateThumbnail;

__int64 __fastcall HookedGenerateThumbnail(__int64 a1, int a2) {
    int new_area = a2 * g_areaMultiplier;
    Wh_Log(L"[x64] Successfully intercepted thumbnail generation! Original area: %d, Scaled area: %d", a2, new_area);
    return pOriginalGenerateThumbnail(a1, new_area);
}

BOOL SetupHook() {
    // Sacred 53-byte pattern extracted from the x64 version of vislib.dll, with dynamic addresses replaced by \x00 for mask usage.
    const char pattern[] =
        "\x48\x8B\xC4\x48\x89\x58\x18\x55\x56\x57\x41\x54\x41\x55\x41\x56"
        "\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\x00\x0F\x29\x78"
        "\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00"
        "\x4C\x8B\xE1\x8B\xF2";

    // Precise mask for the 53-byte pattern: 'x' for exact match, '?' for wildcard (dynamic addresses)
    const char mask[] = "xxxxxxxxxxxxxxxxxxxxx????xxx?xxx?xxx????xxxxxxx?xxxxx";

    void* pTargetFunc = FindPattern(L"vislib.dll", pattern, mask);
    if (!pTargetFunc) {
        Wh_Log(L"[x64] Pattern match failed! Please check the vislib.dll version.");
        return FALSE;
    }

    Wh_Log(L"[x64] Pattern match successful! Target function address: %p", pTargetFunc);
    return Wh_SetFunctionHook(pTargetFunc, (void*)HookedGenerateThumbnail, (void**)&pOriginalGenerateThumbnail);
}

// =====================================================================
// Hooking Logic for 32-bit (x86) Architecture
// =====================================================================
#else

// Ensure the calling convention is __thiscall and there are 3 parameters on the stack (retn 0Ch)
typedef int (__thiscall *GenerateThumbnail32_t)(void* pThis, int a2_area, int arg3, int arg4);
GenerateThumbnail32_t pOriginalGenerateThumbnail32;

int __thiscall HookedGenerateThumbnail32(void* pThis, int a2_area, int arg3, int arg4) {
    int new_area = a2_area * g_areaMultiplier;

    Wh_Log(L"[x86] Successfully intercepted thumbnail generation! Original area: %d, Scaled area: %d", a2_area, new_area);

    // Replace the area parameter with the new scaled value, while passing through the this pointer and the other two unknown parameters as they are.
    return pOriginalGenerateThumbnail32(pThis, new_area, arg3, arg4);
}

BOOL SetupHook() {
    // 45-byte pattern extracted from the x86 version of vislib.dll
    // Address for ___security_cookie and GetDC are dynamic and can change between versions, so they are masked out.
    const char pattern[] =
        "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x3C\xA1\x00\x00\x00\x00\x33\xC4"
        "\x89\x44\x24\x38\x53\x56\x57\x8B\xF9\xC7\x44\x24\x18\x00\x00\x00"
        "\x00\x6A\x00\x89\x7C\x24\x34\xFF\x15\x00\x00\x00\x00";

    const char mask[] =
        "xxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxx????";

    void* pTargetFunc = FindPattern(L"vislib.dll", pattern, mask);
    if (!pTargetFunc) {
        Wh_Log(L"[x86] Pattern match failed! Please check the vislib.dll version.");
        return FALSE;
    }

    Wh_Log(L"[x86] Pattern match successful! Target function address: %p", pTargetFunc);
    return Wh_SetFunctionHook(pTargetFunc, (void*)HookedGenerateThumbnail32, (void**)&pOriginalGenerateThumbnail32);
}

#endif

// =====================================================================
// Windhawk Mod Lifecycle Functions
// =====================================================================
BOOL Wh_ModInit() {
    Wh_Log(L"Visio HD Thumbnail Mod Initializing...");
    
    // Read settings on initialization
    LoadSettings();

    if (SetupHook()) {
        Wh_Log(L"Hook successfully deployed!");
        return TRUE;
    }
    return FALSE;
}

void Wh_ModUninit() {
    Wh_Log(L"Visio HD Thumbnail Mod Uninitializing...");
}