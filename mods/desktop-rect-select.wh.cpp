// ==WindhawkMod==
// @id              desktop-rect-select
// @name            2D Rectangular Selection for Desktop
// @name:zh-CN      桌面图标矩形选择
// @description     Bring 2D rectangular selection to desktop icons, similar to Excel.
// @description:zh-CN  在桌面图标上使用类似 Excel 的二维矩形选择方式。
// @version         1.0
// @author          Joe Ye
// @github          https://github.com/JoeYe-233
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 2D Rectangular Selection for Desktop Icons

The Windows Desktop uses a different ListView implementation compared to standard Explorer folder views to enable free placement of icons. However, this distinct architecture means some standard features may not behave as expected. Most notably, the `Shift` + `Click` selection behavior defaults to a "1D" linear selection (item-by-item) rather than a 2D rectangular selection.

This mod changes the `Shift` + `Click` behavior on Desktop to use a **2D rectangular selection**. When you hold Shift and click to select a range, the mod selects all icons within the *rectangular area* defined by your start and end points (similar to selecting a range of cells with `Shift` + `Click` in Excel), rather than selecting every icon sequentially between them.

**Key Improvements:**
* **Intuitive Selection:** Matches the visual layout of your icons rather than their underlying index order. It's just how you'd expect it to work!
* **Multi-Monitor Safety:** Prevents accidental selection of icons on secondary displays, which often happens with the default linear behavior and can lead to unintended layout disruptions.
* **Complex Selections:** Supports adding or removing rectangular regions from your current selection by holding `Ctrl` + `Shift` + `Click`.

**Tested and verified on the following 64-bit platforms:**
* Windows 10 (22H2, 19045.4412)
* Windows 11 (22H2, 22621.1702)
* Windows Server 2022 (21H2, 20348.2700)
* Windows 7 SP1 (6.1.7601)
* Windows 8 CP (6.2.8250)

If it doesn't work on your system, please open an issue and provide your version of `comctl32.dll` (which should be above 1MB in size, in case there are multiple versions on your system) at my [GitHub repository](https://github.com/JoeYe-233/windhawk-mods). Generally, the right version is located at `C:\Windows\WinSxS\[amd64, x86]_microsoft.windows.common-controls_[version string]\comctl32.dll`, where `[amd64, x86]` and `[version string]` varies based on your system and architecture.

*Note: This mod applies only to the Desktop, other Explorer windows are not affected.*

## Screenshots

### Before (Default Linear Selection)
![Before](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/desktop-rect-select-before.gif) 

### After (2D Rectangular Selection)
![After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/desktop-rect-select-after.gif)

### Complex Selections
![Complex Selections](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/desktop-rect-select-after2.gif)

*/
// ==/WindhawkModReadme==

#include <vector>
#include <windows.h>
#include <psapi.h>
#include <atomic>

#define LOG(fmt, ...) Wh_Log(fmt, ##__VA_ARGS__)

struct RestoreUnit {
    void* targetAddr;
    std::vector<uint8_t> originalBytes;
};

std::vector<RestoreUnit> g_appliedPatches;
HANDLE g_hInitThread = nullptr;
std::atomic<bool> g_stopThread(false);

enum PatternType {
    TYPE_STANDARD,      // Win10/11: Simple NOP and byte replacement
    TYPE_WIN7_REWRITE   // Win7: Requires rewriting entire instruction block to support long offsets
};

struct PatchConfig {
    size_t offset_jge;      
    size_t offset_cmp_left; 
    uint8_t val_left;       
    size_t offset_jle;      
    size_t offset_cmp_right;
    uint8_t val_right;      
};

struct PatternDef {
    PatternType type;
    const char* name;
    std::vector<uint8_t> bytes;
    const char* mask;
    PatchConfig config;
};

void* ScanPattern(HMODULE module, const std::vector<uint8_t>& pattern, const char* mask) {
    MODULEINFO moduleInfo = {0};
    if (!GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(MODULEINFO))) {
        return nullptr;
    }
    const uint8_t* start = (const uint8_t*)moduleInfo.lpBaseOfDll;
    const uint8_t* end = start + moduleInfo.SizeOfImage;
    size_t patternLen = strlen(mask);

    for (const uint8_t* p = start; p < end - patternLen; p++) {
        bool found = true;
        for (size_t i = 0; i < patternLen; i++) {
            if (mask[i] != '?' && pattern[i] != p[i]) {
                found = false; 
                break; 
            }
        }
        if (found) return (void*)p;
    }
    return nullptr;
}

bool PatchMemory(void* addr, const std::vector<uint8_t>& newBytes) {
    DWORD oldProtect;
    if (VirtualProtect(addr, newBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        RestoreUnit restore;
        restore.targetAddr = addr;
        restore.originalBytes.resize(newBytes.size());
        memcpy(restore.originalBytes.data(), addr, newBytes.size());
        g_appliedPatches.push_back(restore);

        memcpy(addr, newBytes.data(), newBytes.size());
        VirtualProtect(addr, newBytes.size(), oldProtect, &oldProtect);
        return true;
    }
    return false;
}

DWORD WINAPI PatchThread(LPVOID lpParam) {
    int maxWait = 300; 
    HMODULE hComctl = nullptr;
    
    while (maxWait > 0 && !g_stopThread) {
        hComctl = GetModuleHandle(L"comctl32.dll");
        if (hComctl) break;
        Sleep(100);
        maxWait--;
    }

    if (g_stopThread || !hComctl) return 0;

    std::vector<PatternDef> patterns;

    // Win11 (Short Jump)
    patterns.push_back({
        TYPE_STANDARD, "Win11 (RBP/ShortJump)",
        { 0x3B, 0x45, 0x00, 0x7D, 0x00, 0x44, 0x3B, 0x45, 0x00, 0x7C, 0x00, 0x3B, 0x45, 0x00, 0x7E, 0x00, 0x44, 0x3B, 0x45, 0x00 },
        "xx?x?xxx?x?xx?x?xxx?",
        { 3, 8, 0x07, 14, 19, 0x0F }
    });

    // Win10 (Long Jump)
    patterns.push_back({
        TYPE_STANDARD, "Win10 (RBP/NearJump)",
        { 0x3B, 0x45, 0x00, 0x7D, 0x00, 0x44, 0x3B, 0x45, 0x00, 0x0F, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x45, 0x00, 0x7E, 0x00, 0x44, 0x3B, 0x45, 0x00 },
        "xx?x?xxx?xx????xx?x?xxx?",
        { 3, 8, 0x07, 18, 23, 0x0F }
    });

    // Win7 (RSP/SIB) - Block Rewrite
    // Config only serves as a placeholder or for special purposes
    patterns.push_back({
        TYPE_WIN7_REWRITE, "Win7 (RSP/SIB Rewrite)",
        { 
            0x3B, 0x44, 0x24, 0x00,       // CMP EAX, Bottom
            0x7D, 0x00,                   // JGE
            0x44, 0x3B, 0x44, 0x24, 0x00, // CMP R8D, Left
            0x7C, 0x00,                   // JL (Offset 11, Byte at 12)
            0x3B, 0x44, 0x24, 0x00,       // CMP EAX, Top
            0x7E, 0x00,                   // JLE
            0x44, 0x3B, 0x44, 0x24, 0x00  // CMP R8D, Right
        },
        "xxx?x?xxxx?x?xxx?x?xxxx?",
        { 0, 0, 0x88, 0, 0, 0x90 } // Only val_left=0x88, val_right=0x90 are used
    });

    void* foundAddress = nullptr;
    PatternDef* matchedPattern = nullptr;

    for (auto& pat : patterns) {
        if (g_stopThread) return 0;
        foundAddress = ScanPattern(hComctl, pat.bytes, pat.mask);
        if (foundAddress) {
            matchedPattern = &pat;
            break;
        }
    }

    if (foundAddress && matchedPattern) {
        LOG(L"Target found (%S) at %p.", matchedPattern->name, foundAddress);
        uint8_t* base = (uint8_t*)foundAddress;
        
        if (matchedPattern->type == TYPE_WIN7_REWRITE) {
            // ==========================================================
            // Windows 7 specific rewrite logic
            // ==========================================================
            LOG(L"Applying Win7 Block Rewrite Patch...");

            // 1. Get the original JL jump offset (at offset 12)
            // pattern: ... 7C [xx] ...
            uint8_t originalJumpOffset = *(base + 12);
            
            // Calculate new jump offset
            // Original instruction end position relative to base is 13 (offset 11 + 2 bytes)
            // New instruction end position relative to base is 10 (8 bytes CMP + 2 bytes JL)
            // Difference = 3 bytes (we saved 3 bytes)
            // So the jump target relative distance increased by 3
            uint8_t newJumpOffset = originalJumpOffset + 3;

            // --- Block 1: Bottom + Left Check ---
            // Target: CMP R8D, [RSP+88h] (8 bytes) + JL newOffset (2 bytes) + NOPs (3 bytes) = 13 bytes
            std::vector<uint8_t> block1;
            // 44 3B 84 24 [88 00 00 00]
            block1 = { 0x44, 0x3B, 0x84, 0x24, matchedPattern->config.val_left, 0x00, 0x00, 0x00 }; 
            block1.push_back(0x7C);            // JL
            block1.push_back(newJumpOffset);   // Corrected Offset
            block1.push_back(0x90); block1.push_back(0x90); block1.push_back(0x90); // 3 NOPs

            // Apply Block 1 (from Base + 0, covering 13 bytes)
            PatchMemory(base, block1);

            // --- Block 2: Top + Right Check ---
            // Target: CMP R8D, [RSP+90h] (8 bytes) + NOPs (3 bytes) = 11 bytes
            // Starting position: Base + 13
            std::vector<uint8_t> block2;
            // 44 3B 84 24 [90 00 00 00]
            block2 = { 0x44, 0x3B, 0x84, 0x24, matchedPattern->config.val_right, 0x00, 0x00, 0x00 };
            block2.push_back(0x90); block2.push_back(0x90); block2.push_back(0x90); // 3 NOPs

            // Apply Block 2
            PatchMemory(base + 13, block2);

        } else {
            // ==========================================================
            // Windows 10/11 standard patch
            // ==========================================================
            const auto& cfg = matchedPattern->config;
            PatchMemory(base + cfg.offset_jge, {0x90, 0x90});
            PatchMemory(base + cfg.offset_cmp_left, {cfg.val_left});
            PatchMemory(base + cfg.offset_jle, {0x90, 0x90});
            PatchMemory(base + cfg.offset_cmp_right, {cfg.val_right});
        }

        LOG(L"Desktop Patch Applied Successfully.");

    } else {
        LOG(L"Pattern not found.");
    }
    return 0;
}

BOOL Wh_ModInit() {
    LPWSTR cmdLine = GetCommandLineW();
    bool isFileExplorer = false;
    if (wcsstr(cmdLine, L"/factory") != nullptr || 
        wcsstr(cmdLine, L"-Embedding") != nullptr) {
        isFileExplorer = true;
    }
    if (isFileExplorer) return FALSE; 

    LOG(L"Desktop Process detected. Initializing patcher...");
    g_stopThread = false;
    g_hInitThread = CreateThread(nullptr, 0, PatchThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    g_stopThread = true;
    if (g_hInitThread) {
        WaitForSingleObject(g_hInitThread, 1000);
        CloseHandle(g_hInitThread);
        g_hInitThread = nullptr;
    }
    if (!g_appliedPatches.empty()) {
        for (auto it = g_appliedPatches.rbegin(); it != g_appliedPatches.rend(); ++it) {
            DWORD oldProtect;
            if (VirtualProtect(it->targetAddr, it->originalBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                memcpy(it->targetAddr, it->originalBytes.data(), it->originalBytes.size());
                VirtualProtect(it->targetAddr, it->originalBytes.size(), oldProtect, &oldProtect);
            }
        }
        g_appliedPatches.clear();
        LOG(L"Memory restored.");
    }
}