// ==WindhawkMod==
// @id              shadowplay-do-not-disable
// @name            Shadowplay anti-disable
// @description     Prevent Nvidia ShadowPlay from disabling itself
// @version         1.1
// @author          Temm
// @github          https://github.com/leumasme
// @include         nvcontainer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shadowplay anti-disable
Nvidia ShadowPlay will normally turn off automatically/pause when detecting DRM content playing or when a Window requests to be hidden from screen captures.
This sucks, because ShadowPlay may just randomly turn off because a random window requests to be hidden from screen captures.
No more! This mod disables both of these checks.

Works with both the old "GeForce Experience" as well as the new "NVIDIA App"

![Screenshot](https://i.imgur.com/zdP0nhd.png)
*/
// ==/WindhawkModReadme==

#include <tlhelp32.h>
#include <windhawk_api.h>
#include <psapi.h>


// A flag is set to 1 when Widevine L1 (Hardware Module) is instantiated.
// These instructions in nvd3dumx.dll check the flag. Patch their instructions to pretend the flag is always 0.

// mov r8d,[rdx+00000170] ; test r8d,r8d
static BYTE g_originalPattern1[] = {0x44, 0x8B, 0x82, 0x70, 0x01, 0x00, 0x00, 0x45, 0x85, 0xC0};
// xor r8d,r8d + NOPs + test r8d,r8d
static BYTE g_patchedPattern1[] = {0x45, 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90, 0x45, 0x85, 0xC0};

// mov ecx,[rax+00000170] ; test ecx,ecx  
static BYTE g_originalPattern2[] = {0x8B, 0x88, 0x70, 0x01, 0x00, 0x00, 0x85, 0xC9};
// xor ecx,ecx + NOPs + test ecx,ecx
static BYTE g_patchedPattern2[] = {0x31, 0xC9, 0x90, 0x90, 0x90, 0x90, 0x85, 0xC9};

typedef struct {
    BYTE* searchPattern;
    BYTE* replacePattern;
    size_t patternSize;
} PatternPair;

#define APPLY_PATTERNS(hModule, patterns) \
    ApplyPatterns(hModule, patterns, sizeof(patterns) / sizeof(patterns[0]));

// Apply list of search-replace memory patterns to the given module
void ApplyPatterns(HMODULE hModule, PatternPair* patterns, int patternCount) {
    if (!hModule) return;
    
    // Get module info
    MODULEINFO modInfo = {0};
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) {
        Wh_Log(L"Failed to get module information to apply patterns");
        return;
    }
    
    BYTE* baseAddr = (BYTE*)modInfo.lpBaseOfDll;
    SIZE_T moduleSize = modInfo.SizeOfImage;
    
    Wh_Log(L"Applying patterns at base: 0x%p, size: 0x%zX", baseAddr, moduleSize);
    
    int changeCount = 0;
    
    for (int patternIndex = 0; patternIndex < patternCount; patternIndex++) {
        PatternPair* pattern = &patterns[patternIndex];
        
        bool success = false;

        // Search for the pattern
        for (SIZE_T i = 0; i <= moduleSize - pattern->patternSize; i++) {
            if (memcmp(baseAddr + i, pattern->searchPattern, pattern->patternSize) == 0) {
                Wh_Log(L"Found pattern %d at offset: 0x%zX", patternIndex + 1, i);
                
                DWORD oldProtect;
                if (VirtualProtect(baseAddr + i, pattern->patternSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    memcpy(baseAddr + i, pattern->replacePattern, pattern->patternSize);
                    VirtualProtect(baseAddr + i, pattern->patternSize, oldProtect, &oldProtect);
                    Wh_Log(L"Successfully applied pattern %d at offset: 0x%zX", patternIndex + 1, i);
                    success = true;
                }
                break;
            }
        }

        if (success == false) {
            Wh_Log(L"Failed to find pattern %d. Is the patch already applied or was there a driver update?", patternIndex + 1);
        }
    }
    
    Wh_Log(L"Applying Patterns complete");
}

void PatchNvd3dumx(HMODULE hModule) {
    PatternPair patchPatterns[] = {
        {g_originalPattern1, g_patchedPattern1, sizeof(g_originalPattern1)},
        {g_originalPattern2, g_patchedPattern2, sizeof(g_originalPattern2)}
    };
    
    APPLY_PATTERNS(hModule, patchPatterns);
}

void UnpatchNvd3dumx(HMODULE hModule) {
    // Apply replacement in reverse to unpatch
    PatternPair unpatchPatterns[] = {
        {g_patchedPattern1, g_originalPattern1, sizeof(g_patchedPattern1)},
        {g_patchedPattern2, g_originalPattern2, sizeof(g_patchedPattern2)}
    };
    APPLY_PATTERNS(hModule, unpatchPatterns);
}

BOOL WINAPI GetWindowDisplayAffinity_Hook(IN HWND hWnd, OUT DWORD *pwdAffinity) {
    // Drivers try to not record windows that have their Window Display Affinity set to anything other than NONE,
    // since those windows request to not be included on screen captures.
    // These windows are also hidden (transparent/absent) on normal screenshots and screensharing software, but the way the Nvidia
    // driver records, it would also capture these hidden windows - so it just refuses to record instead.
    // Block this by just returning NONE affinity.
    *pwdAffinity = WDA_NONE; 
    return TRUE;
}

BOOL WINAPI Module32FirstW_Hook(IN HANDLE hSnapshot, IN OUT LPMODULEENTRY32W lpme) {
    // Drivers try to detect DRM dll being loaded into browsers by creating a toolhelp module snapshot
    // and then iterating over the modules, starting with this function.
    // Block this by just not writing the module entry and returning false to indicate no further modules.
    return FALSE;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = NULL;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    // Call the original function first
    HMODULE result = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    
    if (lpLibFileName && result) {
        // nvd3dumx should have the memory patches applied when it gets loaded.
        // The library is only loaded while ShadowPlay is turned on, and gets dynamically
        // loaded & unloaded when you toggle shadowplay.
        const wchar_t* target = L"nvd3dumx.dll";
        size_t fileNameLen = wcslen(lpLibFileName);
        size_t targetLen = wcslen(target);
        
        if (fileNameLen >= targetLen) {
            // Does lpLibFileName end with nvd3dumx.dll? (case insensitive)
            if (_wcsicmp(lpLibFileName + fileNameLen - targetLen, target) == 0) {
                Wh_Log(L"Detected nvd3dumx.dll loading: %s", lpLibFileName);
                PatchNvd3dumx(result);
            }
        }
    }
    
    return result;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // Check if nvd3dumx.dll is already loaded and patch it
    HMODULE hNvd3dumx = GetModuleHandle(L"nvd3dumx.dll");
    if (hNvd3dumx) {
        Wh_Log(L"Found nvd3dumx.dll already loaded, patching immediately");
        PatchNvd3dumx(hNvd3dumx);
    }

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    void* hookingGetWindowDisplayAffinity = (void*)GetProcAddress(hUser32, "GetWindowDisplayAffinity");
    Wh_SetFunctionHook(hookingGetWindowDisplayAffinity, (void*)GetWindowDisplayAffinity_Hook, NULL);

    HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
    void* hookingModule32FirstW = (void*)GetProcAddress(hKernel32, "Module32FirstW");
    Wh_SetFunctionHook(hookingModule32FirstW, (void*)Module32FirstW_Hook, NULL);
    
    HMODULE hKernelbase = GetModuleHandle(L"kernelbase.dll");
    // Hook LoadlibraryExW to patch nvd3dumx when it gets loaded (shadowplay toggled on)
    void* hookingLoadLibraryExW = (void*)GetProcAddress(hKernelbase, "LoadLibraryExW");
    Wh_SetFunctionHook(hookingLoadLibraryExW, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    
    // Check if nvd3dumx.dll is still loaded and undo patches
    HMODULE hNvd3dumx = GetModuleHandle(L"nvd3dumx.dll");
    if (hNvd3dumx) {
        Wh_Log(L"Found nvd3dumx.dll still loaded, restoring original code");
        UnpatchNvd3dumx(hNvd3dumx);
    } else {
        Wh_Log(L"nvd3dumx.dll not loaded, no patches to restore");
    }
}