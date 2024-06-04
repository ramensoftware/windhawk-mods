// ==WindhawkMod==
// @id              ce-disable-process-button-flashing
// @name            CE Disable Process Button Flashing
// @description     Disables the flashing on the Open Process button in Cheat Engine
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         cheatengine-x86_64.exe
// @include         cheatengine-x86_64-SSE4-AVX2.exe
// @include         cheatengine-i386.exe
// @compilerOptions -masm=intel
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# CE Disable Process Button Flashing
When Cheat Engine is started, the Open Process button is flashed until the button is pressed:

![Screenshot](https://i.imgur.com/FGMthzy.gif)

This mod disables this flashing on the Open Process button.
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

int g_terminatedOffset{};

__attribute__((always_inline)) inline void setTerminated(BYTE *param1)
{
    bool *pTerminated = (bool*)(param1 + g_terminatedOffset); // The button is flashed until terminated is set to 1 (which is done when the open process button is pressed)
    *pTerminated = true;
}

#ifdef _WIN64
typedef long long (__fastcall *TFlash_Execute_t)(BYTE*);
TFlash_Execute_t TFlash_Execute_orig;
long long __fastcall TFlash_Execute_hook(BYTE *param1)
{
    setTerminated(param1);

    return TFlash_Execute_orig(param1);
}
#else
typedef int (*TFlash_Execute_t)();
TFlash_Execute_t TFlash_Execute_orig;
int TFlash_Execute_hook() // In 32-bit mode, this function uses a custom calling convention. From IDA: int __usercall TFlash_Execute@<eax>(int param1@<eax>)
{                         // The first parameter goes into eax, and the return value also goes into eax
    BYTE *param1;
    asm ("mov %0, eax" : "=r"(param1)); // mov param1, eax

    setTerminated(param1);

    int ret;
    asm (
        "mov eax, %1\n" // mov eax, param1
        "call %2\n"     // call TFlash_Execute_orig
        "mov %0, eax\n" // mov ret, eax
        : "=r"(ret)
        : "r"(param1), "r"(TFlash_Execute_orig)
        : "eax"
    );

    return ret;
}
#endif

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    HMODULE hCheatEngine = GetModuleHandleW(NULL);
    if (!hCheatEngine)
    {
        Wh_Log(L"Failed to load module");
        return FALSE;
    }

#ifdef _WIN64
    std::vector<BYTE> pattern = {
        0x53, 0x57, 0x56, 0x41, 0x54, 0x48, 0x8D, 0x64, 0x24, 0xC8,
        0x48, 0x89, 0xCB, 0x41, 0xB4, 0x01, 0x40, 0xB7, 0xFE, 0x40,
        0x30, 0xF6
    };
    g_terminatedOffset = 0x10;
#else
    std::vector<BYTE> pattern = {
        0x53, 0x8D, 0x64, 0x24, 0xEC, 0x89, 0xC3, 0xC6, 0x44, 0x24,
        0x08, 0x01, 0xC6, 0x44, 0x24, 0x0C, 0xFE, 0xC6, 0x44, 0x24,
        0x10, 0x00
    };
    g_terminatedOffset = 0x8;
#endif

    IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)hCheatEngine;
    IMAGE_NT_HEADERS* pNtHeaders = (IMAGE_NT_HEADERS*)((BYTE*)pDosHeader + pDosHeader->e_lfanew);

    BYTE *scanBytes = (BYTE*)hCheatEngine;
    DWORD sizeOfImage = pNtHeaders->OptionalHeader.SizeOfImage;

    DWORD size = pattern.size();
    BYTE *data = pattern.data();

    TFlash_Execute_t TFlash_Execute{};
    for (auto i = 0ul; i < sizeOfImage - size; ++i)
    {
        bool found = true;
        for(auto j = 0ul; j < size; ++j)
        {
            if(scanBytes[i + j] != data[j] && data[j] != -1)
            {
                found = false;
                break;
            }
        }
        if(found) TFlash_Execute = (TFlash_Execute_t)&scanBytes[i];
    }
    if (!TFlash_Execute)
    {
        Wh_Log(L"Unable to find pattern");
        return FALSE;
    }
    
    if(!WindhawkUtils::Wh_SetFunctionHookT(TFlash_Execute, TFlash_Execute_hook, &TFlash_Execute_orig))
    {
        Wh_Log(L"Failed to hook TFlash.Execute");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
