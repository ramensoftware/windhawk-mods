// ==WindhawkMod==
// @id             win11-round-corner-killer
// @name           Win11 Round Corner Killer
// @description    refer to https://github.com/rich-ayr/win11-toggle-rounded-corners, directly modify the value in .rdata selction to disable the round corner
// @version        1.0.0
// @author         howxu
// @github         https://github.com/HowXu
// @include        dwm.exe
// @architecture   x86-64
// @license        MIT
// ==/WindhawkMod==

#include <windhawk_utils.h>
#include <windows.h>
#include <vector>

struct PatchedFloat {
    float* address;
    float originalValue;
};
std::vector<PatchedFloat> g_patchedFloats;

const float kNearZeroRadius = 0.001f; // 0f - 1f is also used 

// locate .rdata
IMAGE_SECTION_HEADER* FindSection(HMODULE hModule, const char* sectionName) {
    auto dosHeader = (IMAGE_DOS_HEADER*)hModule;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;

    auto ntHeaders = (IMAGE_NT_HEADERS64*)((uint8_t*)hModule + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) return nullptr;

    auto section = IMAGE_FIRST_SECTION(ntHeaders);
    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++) {
        // 比较段名 (如 ".rdata")
        if (strncmp((const char*)section->Name, sectionName, 8) == 0) {
            return section;
        }
    }
    return nullptr;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing .rdata Corner Killer...");

    // handle
    HMODULE hUdwm = GetModuleHandle(L"udwm.dll");
    if (!hUdwm) {
        Wh_Log(L"Failed to find udwm.dll");
        return FALSE;
    }

    // find .rdata
    auto rdataSection = FindSection(hUdwm, ".rdata");
    if (!rdataSection) {
        Wh_Log(L"Failed to locate .rdata section");
        return FALSE;
    }

    // pointer
    float* startPtr = (float*)((uint8_t*)hUdwm + rdataSection->VirtualAddress);
    float* endPtr = (float*)((uint8_t*)startPtr + rdataSection->Misc.VirtualSize);

    // find 4.0f and 8.0f
    for (float* ptr = startPtr; ptr < endPtr; ptr++) {
        if (*ptr == 4.0f || *ptr == 8.0f) {
            
            DWORD oldProtect;
            // permission
            if (VirtualProtect(ptr, sizeof(float), PAGE_READWRITE, &oldProtect)) {
                
                // for unload
                g_patchedFloats.push_back({ptr, *ptr});
                
                // change
                *ptr = kNearZeroRadius;
                
                // revover
                VirtualProtect(ptr, sizeof(float), oldProtect, &oldProtect);
                
                Wh_Log(L"Patched float at %p", ptr);
            }
        }
    }

    Wh_Log(L"Successfully patched %zu values.", g_patchedFloats.size());
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing mod, restoring original corners...");
    
    // when unload
    for (const auto& patch : g_patchedFloats) {
        DWORD oldProtect;
        if (VirtualProtect(patch.address, sizeof(float), PAGE_READWRITE, &oldProtect)) {
            *(patch.address) = patch.originalValue;
            VirtualProtect(patch.address, sizeof(float), oldProtect, &oldProtect);
        }
    }
    g_patchedFloats.clear();
    Wh_Log(L"Restoration complete.");
}