// ==WindhawkMod==
// @id              office-ui-reverter
// @name            Office UI Reverter
// @description     Reverts Office 2022+/365 UI to Office 2016/2019
// @version         1.0.0
// @author          Amrsatrio
// @github          https://github.com/Amrsatrio
// @twitter         https://twitter.com/amrsatrio
// @include         WINWORD.EXE
// @include         EXCEL.EXE
// @include         POWERPNT.EXE
// @include         OUTLOOK.EXE
// @include         ONENOTE.EXE
// @include         MSPUB.EXE
// @include         MSACCESS.EXE
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Office UI Reverter

Reverts the **Office 2022+/Microsoft 365 UI** back to the look of **Office 2019** or **Office 2016**.

Some users prefer the older, less rounded, less visually distracting, and more compact Ribbon UI. This mod lets you force Office to render with those styles.

## Features
- Choose between:
  - **Default** (no modifications)
  - **Office 2019**
  - **Office 2016**
- Works with multiple Office apps (Word, Excel, PowerPoint, Outlook, OneNote, Publisher, Access).
- UI style patching is permanent throughout the lifetime of the process; mod loading/unloading won't result in unexpected behavior and crashes.

## Screenshots

### Office 2016
![Office 2016 style preview](https://i.imgur.com/PaxRT8T.png)

### Office 2019
![Office 2019 style preview](https://i.imgur.com/9gOgYxY.png)

## ⚠️ Note
- Early injection to Office processes is required to patch the UI style. This means the process used to launch Office must not be excluded from Windhawk injection.
- Please close all windows of an Office program and relaunch it to apply the new style.
- Older styles may lack icons for new features such as *Copilot*. Those are not covered by this mod.
- Be advised that Microsoft can remove the older styles anytime in the future. When that happens, I do not plan on resurrecting the older styles.

## Technical Details
- Hooks `LoadLibraryExW` to detect when `mso40uiWin32Client.dll` is loaded.
- Locates and patches the internal `VisualVersion::GetVisualStyleForSurface()` function.
- Replaces the return value with your selected style.

## Compatibility
- Works with **Office 2022+ perpetual** and **Microsoft 365 desktop apps**. Office 2019 and older are **not** supported.
- Supports x64 and ARM64 builds. 32-bit x86 is **not** supported.
- Tested on **Windows 11 23H2 and 24H2**, should also work on **Windows 10** with latest Office builds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- visualStyle: default
  $name: Visual style
  $description: "Note: Older styles may lack icons of newer actions such as Copilot, of which are not covered by this mod.\n
  Changes will take effect on new Office processes."
  $options:
  - default: Default (don't modify)
  - office2019: Office 2019
  - office2016: Office 2016
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <cinttypes>
#include <span>
#include <optional>

enum class VisualStyle : int
{
    Office2016 = 0,
    Office2019 = 1,
};

enum class VisualSurface : int
{
    // Unknown; don't need to handle since we're only replacing return value
};

struct
{
    VisualStyle visualStyle = (VisualStyle)-1;
} g_settings;

std::atomic<bool> g_bMso40UILoaded;

// https://github.com/CAS-Atlantic/AArch64-Encoding

__forceinline DWORD ARM64_ReadBits(DWORD value, int h, int l)
{
    return (value >> l) & ((1 << (h - l + 1)) - 1);
}

__forceinline int ARM64_SignExtend(DWORD value, int numBits)
{
    DWORD mask = 1 << (numBits - 1);
    if (value & mask)
        value |= ~((1 << numBits) - 1);
    return (int)value;
}

__forceinline int ARM64_ReadBitsSignExtend(DWORD insn, int h, int l)
{
    return ARM64_SignExtend(ARM64_ReadBits(insn, h, l), h - l + 1);
}

__forceinline BOOL ARM64_IsInRange(int value, int bitCount)
{
    int minVal = -(1 << (bitCount - 1));
    int maxVal = (1 << (bitCount - 1)) - 1;
    return value >= minVal && value <= maxVal;
}

__forceinline BOOL ARM64_IsBL(DWORD insn)
{
    return ARM64_ReadBits(insn, 31, 26) == 0b100101;
}

__forceinline DWORD* ARM64_FollowBL(DWORD* pInsnBL)
{
    DWORD insnBL = *pInsnBL;
    if (!ARM64_IsBL(insnBL))
        return NULL;
    int imm26 = ARM64_ReadBitsSignExtend(insnBL, 25, 0);
    return pInsnBL + imm26; // offset = imm26 * 4
}

__forceinline DWORD ARM64_BuildMOVZW(int imm16, int Rd)
{
    if (!ARM64_IsInRange(imm16, 16) || !ARM64_IsInRange(Rd, 5))
        return 0;
    return (0b010100101 << 23) | (0b00 << 21) | (imm16 << 5) | (Rd << 0); 
}

__forceinline BOOL ARM64_IsMOVZW(DWORD insn, int Rd_expected = -1)
{
    return ARM64_ReadBits(insn, 31, 21) == 0b010100101
        && (Rd_expected == -1 || (int)ARM64_ReadBits(insn, 4, 0) == Rd_expected);
}

inline BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
{
    for (PBYTE value = (PBYTE)pBuffer; *lpMask; ++lpPattern, ++lpMask, ++value)
    {
        if (*lpMask == 'x' && *(LPCBYTE)lpPattern != *value)
            return FALSE;
    }

    return TRUE;
}

inline PVOID FindPattern(PVOID pBase, SIZE_T dwSize, LPCSTR lpPattern, LPCSTR lpMask)
{
    dwSize -= strlen(lpMask);

    for (SIZE_T index = 0; index < dwSize; ++index)
    {
        PBYTE pAddress = (PBYTE)pBase + index;

        if (MaskCompare(pAddress, lpPattern, lpMask))
            return pAddress;
    }

    return NULL;
}

inline void SectionBeginAndSize(HMODULE hModule, const char* pszSectionName, PBYTE* beginSection, DWORD* sizeSection)
{
    *beginSection = nullptr;
    *sizeSection = 0;

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
    {
        PIMAGE_NT_HEADERS64 ntHeader = (PIMAGE_NT_HEADERS64)((BYTE*)dosHeader + dosHeader->e_lfanew);
        if (ntHeader->Signature == IMAGE_NT_SIGNATURE)
        {
            PIMAGE_SECTION_HEADER firstSection = IMAGE_FIRST_SECTION(ntHeader);
            for (unsigned int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
            {
                PIMAGE_SECTION_HEADER section = firstSection + i;
                if (strncmp((const char*)section->Name, pszSectionName, IMAGE_SIZEOF_SHORT_NAME) == 0)
                {
                    *beginSection = (PBYTE)dosHeader + section->VirtualAddress;
                    *sizeSection = section->SizeOfRawData;
                    break;
                }
            }
        }
    }
}

__forceinline void TextSectionBeginAndSize(HMODULE hModule, PBYTE* beginSection, DWORD* sizeSection)
{
    SectionBeginAndSize(hModule, ".text", beginSection, sizeSection);
}

typedef struct _IMAGE_CHPE_RANGE_ENTRY
{
    union
    {
        ULONG StartOffset;
        struct
        {
            ULONG NativeCode : 1;
            ULONG AddressBits : 31;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    ULONG Length;
} IMAGE_CHPE_RANGE_ENTRY, *PIMAGE_CHPE_RANGE_ENTRY;

typedef struct _IMAGE_ARM64EC_METADATA
{
    ULONG  Version;
    ULONG  CodeMap;
    ULONG  CodeMapCount;
    ULONG  CodeRangesToEntryPoints;
    ULONG  RedirectionMetadata;
    ULONG  __os_arm64x_dispatch_call_no_redirect;
    ULONG  __os_arm64x_dispatch_ret;
    ULONG  __os_arm64x_dispatch_call;
    ULONG  __os_arm64x_dispatch_icall;
    ULONG  __os_arm64x_dispatch_icall_cfg;
    ULONG  AlternateEntryPoint;
    ULONG  AuxiliaryIAT;
    ULONG  CodeRangesToEntryPointsCount;
    ULONG  RedirectionMetadataCount;
    ULONG  GetX64InformationFunctionPointer;
    ULONG  SetX64InformationFunctionPointer;
    ULONG  ExtraRFETable;
    ULONG  ExtraRFETableSize;
    ULONG  __os_arm64x_dispatch_fptr;
    ULONG  AuxiliaryIATCopy;
} IMAGE_ARM64EC_METADATA;

// https://github.com/ramensoftware/windhawk/blob/03963d65e7077b761e5295defc2ccd5378e650a2/src/windhawk/engine/symbol_enum.cpp#L251
template <typename IMAGE_NT_HEADERS_T, typename IMAGE_LOAD_CONFIG_DIRECTORY_T>
std::optional<std::span<const IMAGE_CHPE_RANGE_ENTRY>>
GetChpeRanges(const IMAGE_DOS_HEADER* dosHeader, const IMAGE_NT_HEADERS_T* ntHeader)
{
    auto* opt = &ntHeader->OptionalHeader;

    if (opt->NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG
        || !opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress)
    {
        return std::nullopt;
    }

    DWORD directorySize = opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size;

    const IMAGE_LOAD_CONFIG_DIRECTORY_T* cfg = (const IMAGE_LOAD_CONFIG_DIRECTORY_T*)(
        (const char*)dosHeader + opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress);

    constexpr DWORD kMinSize = offsetof(IMAGE_LOAD_CONFIG_DIRECTORY_T, CHPEMetadataPointer)
        + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY_T::CHPEMetadataPointer);

    if (directorySize < kMinSize || cfg->Size < kMinSize)
    {
        return std::nullopt;
    }

    if (!cfg->CHPEMetadataPointer)
    {
        return std::nullopt;
    }

    // Either IMAGE_CHPE_METADATA_X86 or IMAGE_ARM64EC_METADATA.
    const IMAGE_ARM64EC_METADATA* metadata = (const IMAGE_ARM64EC_METADATA*)(
        (const char*)dosHeader + cfg->CHPEMetadataPointer - opt->ImageBase);

    const IMAGE_CHPE_RANGE_ENTRY* codeMap = (const IMAGE_CHPE_RANGE_ENTRY*)(
        (const char*)dosHeader + metadata->CodeMap);

    return std::span(codeMap, metadata->CodeMapCount);
}

inline UINT_PTR RVAToFileOffset(PBYTE pBase, UINT_PTR rva)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBase;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(pBase + pDosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeaders);
    for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSection++)
    {
        if (rva >= pSection->VirtualAddress && rva < pSection->VirtualAddress + pSection->Misc.VirtualSize)
        {
            return rva - pSection->VirtualAddress + pSection->PointerToRawData;
        }
    }
    return 0;
}

bool InternalHookMso40UISymbols(HMODULE hModule)
{
    if (g_settings.visualStyle == (VisualStyle)-1)
    {
        return true;
    }

    bool bRet = false;
    
    PBYTE pbSearchBegin = nullptr;
    DWORD cbSearch = 0;
    TextSectionBeginAndSize(hModule, &pbSearchBegin, &cbSearch);
    if (!pbSearchBegin || !cbSearch)
    {
        return false;
    }

    // https://github.com/ramensoftware/windhawk/blob/03963d65e7077b761e5295defc2ccd5378e650a2/src/windhawk/engine/symbol_enum.cpp#L506
    const IMAGE_DOS_HEADER* dosHeader = (const IMAGE_DOS_HEADER*)hModule;
    const IMAGE_NT_HEADERS* ntHeader = (const IMAGE_NT_HEADERS*)((const char*)dosHeader + dosHeader->e_lfanew);
    WORD magic = ntHeader->OptionalHeader.Magic;

    std::optional<std::span<const IMAGE_CHPE_RANGE_ENTRY>> chpeRanges;
    switch (magic)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            /*chpeRanges = GetChpeRanges<IMAGE_NT_HEADERS32, IMAGE_LOAD_CONFIG_DIRECTORY32>(
                dosHeader, (const IMAGE_NT_HEADERS32*)ntHeader);
            break;*/
            Wh_Log(L"32-bit Office is not supported");
            return false;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            chpeRanges = GetChpeRanges<IMAGE_NT_HEADERS64, IMAGE_LOAD_CONFIG_DIRECTORY64>(
                dosHeader, (const IMAGE_NT_HEADERS64*)ntHeader);
            break;

        default:
            return false;
    }

    bool bIsARM64 = false;
    if (chpeRanges.has_value())
    {
        for (const IMAGE_CHPE_RANGE_ENTRY& range : *chpeRanges)
        {
            ULONG typeMask = magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC ? 1 : 3;
            ULONG start = range.StartOffset & ~typeMask; // RVA
            ULONG type = range.StartOffset & typeMask; // 0 = Arm64, 1 = Arm64EC, 2 = Amd64
            if (type == 1) // Arm64EC
            {
                bIsARM64 = true;
                pbSearchBegin = (PBYTE)hModule + RVAToFileOffset((PBYTE)hModule, start);
                cbSearch = range.Length;
                break;
            }
        }
    }

    PBYTE match;
    if (!bIsARM64)
    {
        // xor     ecx, ecx
        // call    ?GetVisualStyleForSurface@VisualVersion@@YA?AW4VisualStyle@1@W4VisualSurface@1@@Z
        // cmp     eax, 2
        // lea     rcx, ?
        // setnb   al
        // neg     al
        // sbb     edx, edx
        // 33 C9 E8 ?? ?? ?? ?? 83 F8 02 48 8D 0D ?? ?? ?? ?? 0F 93 C0 F6 D8 1B D2
        //       .  ^^^^^^^^^^^
        match = (PBYTE)FindPattern(
            pbSearchBegin,
            cbSearch,
            "\x33\xC9\xE8\x00\x00\x00\x00\x83\xF8\x02\x48\x8D\x0D\x00\x00\x00\x00\x0F\x93\xC0\xF6\xD8\x1B\xD2",
            "xxx????xxxxxx????xxxxxxx");
        if (match)
        {
            match += 2; // Position to E8 (call)
            match += 5 + *(int*)(match + 1); // Follow
        }
    }
    else
    {
        // CMN             W8, #1
        // B.NE            ?
        // MOV             W0, #0
        // BL              ?GetVisualStyleForSurface@VisualVersion@@$$hYA?AW4VisualStyle@1@W4VisualSurface@1@@Z
        // MOV             W9, #372
        // 1F 05 00 31 ?? ?? ?? ?? 00 00 80 52 ?? ?? ?? ?? 89 2E 80 52
        //                                     ^^^^^^^^^^^
        match = (PBYTE)FindPattern(
            pbSearchBegin,
            cbSearch,
            "\x1F\x05\x00\x31\x00\x00\x00\x00\x00\x00\x80\x52\x00\x00\x00\x00\x89\x2E\x80\x52",
            "xxxx????xxxx????xxxx");
        if (match)
        {
            match += 12; // Position to BL
            match = (PBYTE)ARM64_FollowBL((DWORD*)match);
        }
    }

    if (match)
    {
        Wh_Log(L"GetVisualStyleForSurface(): 0x%" PRIXPTR, match - (PBYTE)hModule);

        if (!bIsARM64)
        {
            BYTE rgbPatch[5 + 1];

            // mov eax, g_settings.visualStyle
            rgbPatch[0] = 0xB8;
            *(int*)(rgbPatch + 1) = (int)g_settings.visualStyle;

            // retn
            rgbPatch[5] = 0xC3;

            BYTE rgbExisting[ARRAYSIZE(rgbPatch)];
            memcpy(rgbExisting, match, sizeof(rgbExisting));
            if (rgbExisting[0] == 0xB8)
            {
                *(int*)(rgbExisting + 1) = *(int*)(rgbPatch + 1);
            }

            if (memcmp(rgbExisting, rgbPatch, sizeof(rgbPatch)) != 0)
            {
                DWORD dwOldProtect;
                if (VirtualProtect(match, sizeof(rgbPatch), PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    memcpy(match, rgbPatch, sizeof(rgbPatch));
                    VirtualProtect(match, sizeof(rgbPatch), dwOldProtect, &dwOldProtect);

                    bRet = true;
                }
            }
            else
            {
                Wh_Log(L"GetVisualStyleForSurface() already patched");
            }
        }
        else
        {
            BYTE rgbPatch[2 * 4];

            // MOV W0, #g_settings.visualStyle
            *(DWORD*)(rgbPatch + 0) = ARM64_BuildMOVZW((int)g_settings.visualStyle, 0 /*W0*/);

            // RET
            *(DWORD*)(rgbPatch + 4) = 0xD65F03C0;

            if (*(DWORD*)(rgbPatch + 0))
            {
                BYTE rgbExisting[ARRAYSIZE(rgbPatch)];
                memcpy(rgbExisting, match, sizeof(rgbExisting));
                if (ARM64_IsMOVZW(*(DWORD*)(rgbExisting + 0), 0 /*W0*/))
                {
                    *(DWORD*)(rgbExisting + 0) = *(DWORD*)(rgbPatch + 0);
                }

                if (memcmp(rgbExisting, rgbPatch, sizeof(rgbPatch)) != 0)
                {
                    DWORD dwOldProtect;
                    if (VirtualProtect(match, sizeof(rgbPatch), PAGE_EXECUTE_READWRITE, &dwOldProtect))
                    {
                        memcpy(match, rgbPatch, sizeof(rgbPatch));
                        VirtualProtect(match, sizeof(rgbPatch), dwOldProtect, &dwOldProtect);

                        bRet = true;
                    }
                }
                else
                {
                    Wh_Log(L"GetVisualStyleForSurface() already patched");
                }
            }
            else
            {
                Wh_Log(L"Failed to make MOV instruction");
            }
        }
    }
    else
    {
        Wh_Log(L"GetVisualStyleForSurface() not found");
    }

    return bRet;
}

void LoadSettings()
{
    LPCWSTR pszVisualStyle = Wh_GetStringSetting(L"visualStyle");
    if (wcscmp(pszVisualStyle, L"office2019") == 0)
    {
        g_settings.visualStyle = VisualStyle::Office2019;
    }
    else if (wcscmp(pszVisualStyle, L"office2016") == 0)
    {
        g_settings.visualStyle = VisualStyle::Office2016;
    }
    else
    {
        g_settings.visualStyle = (VisualStyle)-1; // Default
    }
    Wh_FreeStringSetting(pszVisualStyle);
}

void HookMso40UISymbols(HMODULE hModule)
{
    if (InternalHookMso40UISymbols(hModule))
    {
        // Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t g_pfnOriginalLoadLibraryExW;

HMODULE WINAPI HookedLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = g_pfnOriginalLoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hModule && (dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0 && lpLibFileName && !g_bMso40UILoaded)
    {
        LPCWSTR pszName = wcsrchr(lpLibFileName, L'\\');
        pszName = pszName ? pszName + 1 : lpLibFileName;
        if (wcsicmp(pszName, L"mso40uiWin32Client.dll") == 0 && !g_bMso40UILoaded.exchange(true))
        {
            Wh_Log(L"mso40uiWin32Client.dll loaded");
            HookMso40UISymbols(hModule);
        }
    }
    return hModule;
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    LoadSettings();

    HMODULE hMso40UI = GetModuleHandleW(L"mso40uiWin32Client.dll");
    if (!hMso40UI)
    {
        HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
        LoadLibraryExW_t pfnLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(hKernelBase, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pfnLoadLibraryExW, HookedLoadLibraryExW, &g_pfnOriginalLoadLibraryExW);
    }
    else
    {
        /*g_bMso40UILoaded = true;
        HookMso40UISymbols(hMso40UI);*/
        Wh_Log(L"mso40uiWin32Client.dll has already been loaded, we're too late");
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L">");
}

void Wh_ModUninit()
{
    Wh_Log(L">");
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L">");

    // LoadSettings();
}
