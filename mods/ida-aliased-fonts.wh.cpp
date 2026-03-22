// ==WindhawkMod==
// @id              ida-aliased-fonts
// @name            IDA Aliased Fonts
// @description     Fix up aliased font rendering in IDA 9.x.
// @version         1.0
// @author          Isabella Lulamoon (kawapure)
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @homepage        https://kawapure.github.io/
// @include         ida.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# IDA Aliased Fonts

Fixes aliased font rendering in IDA 9.x.
*/
// ==/WindhawkModReadme==

#include <cstdint>
FARPROC g_pfnQWindowsFontEngineDirectWrite_ctor = nullptr;

// Makes the code view not force antialiasing on fonts:
void __cdecl (*QFont__setStyleStrategy_orig)(void *pThis, int iStrategy);
void __cdecl QFont__setStyleStrategy_hook(void *pThis, int iStrategy)
{
}

/**
 * Nuke code paths which make Qt6 use DirectWrite for font rendering. This forces a fallback to
 * GDI rendering where aliased fonts are better handled.
 *
 * This code only supports x64.
 */
void NukeDirectWrite(void *ptr)
{
    WH_DISASM_RESULT disasm = { 0 };

    // Maximum number of instructions to look at:
    // For my beta copy of IDA 9.2, this is 556 instructions. The branch to be modified
    // is in the middle of the function, so it's not super important for the whole
    // function to be accounted for.
    constexpr DWORD c_dwfnSizeMax = 600 * 4;

    // Find the call point of the DirectWrite font engine constructor.
    void *pDwEngineCtorCallSite = nullptr;
    void *pLastJzRel = nullptr;

    for (DWORD i = 0; i < c_dwfnSizeMax;)
    {
        void *pCurAddr = (void *)(((size_t)ptr + (size_t)i));
        Wh_Disasm(pCurAddr, &disasm);
        Wh_Log(L"%S", disasm.text);

        if (strstr(disasm.text, "jz 0x") == disasm.text)
        {
            if (*(uint16_t *)pCurAddr == 0x840F) // 0F 84 = opcode "JZ rel16/32"
            {
                Wh_Log(L"Noting down last relative JZ instruction.");
                pLastJzRel = pCurAddr;
            }
        }

        if (strstr(disasm.text, "call 0x") == disasm.text)
        {
            // Figure out the address:
            size_t pAddr = (size_t)strtoull(disasm.text + sizeof("call 0x"), NULL, 16);

            Wh_Log(L"Calling %p", pAddr);

            if (pAddr == (size_t)g_pfnQWindowsFontEngineDirectWrite_ctor)
            {
                Wh_Log(L"Found the address of the DirectWrite constructor %p", pAddr);
                pDwEngineCtorCallSite = pCurAddr;
                break;
            }
        }

        i += disasm.length;
    }

    // Look behind the call point for the branch. I have observed this
    // to be a JZ instruction. We want to re-encode this to be an unconditional JMP
    // in order to avoid calling into the DirectWrite logic.
    if (pLastJzRel)
    {
        void *pCurAddr = pLastJzRel;
        if (*(uint16_t *)pCurAddr == 0x840F) // 0F 84 = opcode "JZ rel16/32"
        {
            Wh_Log(L"We found the right instruction (probably), so let's patch it!");

            constexpr size_t c_cbJzRel = 6;

            // This instruction is relatively addressed, meaning that we need to
            // do two things:
            //   1. Of course, we need to extract the address.
            //   2. But that's not enough. Because "JMP rel32" instructions are
            //      smaller than the "JZ rel16/32" that we're working with, we
            //      actually end up with an extra instruction following our JMP.
            //      This offsets the relative instruction, so we need to increment
            //      the target address by 1.
            size_t rel = 
                (*((uint16_t *)pCurAddr + 2) << 16) // Upper 2 bytes
                | ((*(uint32_t *)pCurAddr) >> 16); // Lower 2 bytes

            rel += 1;

            DWORD dwOldProtect, dwOldProtect2;
            VirtualProtect(pCurAddr, c_cbJzRel, PAGE_READWRITE, &dwOldProtect);

            // Encode JMP rel32:
            *(BYTE *)pCurAddr = 0xE9; // Opcode
            *(DWORD *)((BYTE *)pCurAddr + 1) = rel; // Parameter
            *((BYTE *)pCurAddr + 5) = 0x90; // NOP opcode to account for the size difference

            VirtualProtect(pCurAddr, c_cbJzRel, dwOldProtect, &dwOldProtect2);

            WH_DISASM_RESULT disasm2 = { 0 };
            Wh_Disasm(pCurAddr, &disasm2);
            WH_DISASM_RESULT disasm3 = { 0 };
            Wh_Disasm((void *)((size_t)pCurAddr + disasm2.length), &disasm3);
            Wh_Log(L"We should be done: %S; %S", disasm2.text, disasm3.text);
        }
    }
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
#include <cstdio>
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    bool fIsQt5 = false;
    HMODULE hmQtGui = GetModuleHandleW(L"qt6gui.dll");

    if (!hmQtGui)
    {
        Wh_Log(L"Failed to find qt6gui.dll. Trying Qt5 path (IDA 9.1)");

        hmQtGui = GetModuleHandleW(L"Qt5Gui.dll");
        fIsQt5 = true;

        if (!hmQtGui)
        {
            Wh_Log(L"Failed to find Qt5Gui.dll. No other paths are possible, so the mod will now bail.");
            return FALSE;
        }
    }

    // The following patches are only useful for IDA 9.2, which uses Qt6.
    // Qt5 does not have these problems, but IDA 9.x still has forced smooth fonts in text editor
    // views that are undesirable.
    if (!fIsQt5)
    {
        FARPROC pfnQWindowsFontDatabase__createEngine = GetProcAddress(
            hmQtGui, 
            "?createEngine@QWindowsFontDatabase@QT@@SAPEAVQFontEngine@2@AEBUQFontDef@2@AEBVQString@2@HAEBV?$QSharedPointer@VQWindowsFontEngineData@QT@@@2@@Z"
        );

        g_pfnQWindowsFontEngineDirectWrite_ctor = GetProcAddress(
            hmQtGui, 
            "??0QWindowsFontEngineDirectWrite@QT@@QEAA@PEAUIDWriteFontFace@@NAEBV?$QSharedPointer@VQWindowsFontEngineData@QT@@@1@@Z"
        );

        NukeDirectWrite((void *)pfnQWindowsFontDatabase__createEngine);
    }

    FARPROC pfnQFont__setStyleStrategy = GetProcAddress(
        hmQtGui, 
        "?setStyleStrategy@QFont@QT@@QEAAXW4StyleStrategy@12@@Z"
    );

    Wh_SetFunctionHook(
        (void *)pfnQFont__setStyleStrategy,
        (void *)QFont__setStyleStrategy_hook,
        (void **)&QFont__setStyleStrategy_orig
    );

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}
