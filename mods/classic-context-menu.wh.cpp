// ==WindhawkMod==
// @id              classic-context-menu
// @name            Classic Context Menu
// @description     Restores the classic (non-immersive) context menus
// @version         1.0.0
// @author          YungDawn
// @github          https://github.com/YungDawn
// @include         *
// ==/WindhawkMod==
/**/
// ==WindhawkModReadme==
/*
# Classic Context Menu

Windows 10 draws most context menus itself instead of letting the classic menu
code draw them. That is why they ignore the current visual style: no msstyles
borders, no item icons, no classic look.

This mod stops the shell from doing that conversion, so the classic renderer
draws the menus again. Both 64-bit and 32-bit processes are handled.

**Windows 10 only.** Windows 11 moved to a different menu system where the
classic menu lives behind "Show more options".

## How it works

A single shell function, `ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu`
(present in both `shell32.dll` and `ExplorerFrame.dll`), walks the menu items and
ORs `MFT_OWNERDRAW` into each item's `fType`. The shell then draws the items
itself, so theming never applies.

The same function also re-applies a `MENUITEMINFO` whose `fMask` contains
`MIIM_BITMAP` while `hbmpItem` was never filled in, which clears the item's icon.
That is invisible while the item is owner-draw - the shell paints the icon from
the owner-draw data - but becomes visible as soon as it is not.

* **64-bit:** the whole function is replaced with `xor eax,eax; ret`.
* **32-bit:** the function has to keep running. The navigation pane keeps an
  owner-draw array pointer and touches it while dismissing the menu, so skipping
  the function leaves a dangling object and crashes Explorer on close. Instead
  six single bytes are changed: the `or eax,100h` that sets `MFT_OWNERDRAW`, and
  the `MIIM_BITMAP` bit in each of the three `fMask` writes that wipe the icon.

## Notes

* The mod has to be injected into every process, because `shell32.dll` and
  `ExplorerFrame.dll` are loaded almost everywhere.
* If a Windows update moves the code, each site is found again by byte
  signature. If a signature cannot be found, that site is skipped and the
  Windhawk log says so - no crash, the menu simply stays immersive.
* Turning the mod off (or one of the two patches) restores the original bytes in
  running processes, no restart needed.
* Verified against `shell32.dll` / `ExplorerFrame.dll` 10.0.19041.3636
  (System32) and 10.0.19041.3758 (SysWOW64), Windows 10 22H2 (19045).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DisableX64: false
  $name: Disable the 64-bit patch (shell32.dll + ExplorerFrame.dll)
  $description: Leave this off. Replaces the conversion function entirely; this is the verified configuration - icons, submenus and msstyles all behave.
- DisableX86: false
  $name: Disable the 32-bit patch (shell32.dll + ExplorerFrame.dll)
  $description: Leave this off. Single-byte surgical fix - the function still runs completely, it just never marks items owner-draw. Turn on only if a 32-bit application misbehaves.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#define MAX_APPLIED 8   // 6 sites in 32-bit, 2 in 64-bit

struct Applied
{
    BYTE* addr;
    BYTE  orig[4];
    int   len;
};

static Applied g_applied[MAX_APPLIED];
static int     g_appliedCount = 0;

static void Remember(BYTE* addr, int len)
{
    if (g_appliedCount >= MAX_APPLIED || len <= 0 || len > 4) return;
    g_applied[g_appliedCount].addr = addr;
    g_applied[g_appliedCount].len  = len;
    memcpy(g_applied[g_appliedCount].orig, addr, len);
    ++g_appliedCount;
}

static void WriteBytes(BYTE* p, const BYTE* src, int len)
{
    DWORD old = 0;
    if (!VirtualProtect(p, len, PAGE_EXECUTE_READWRITE, &old))
    {
        Wh_Log(L"VirtualProtect failed at %p (%lu)", p, GetLastError());
        return;
    }
    Remember(p, len);
    memcpy(p, src, len);
    VirtualProtect(p, len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), p, len);
}

static void RestoreAll()
{
    for (int i = 0; i < g_appliedCount; ++i)
    {
        BYTE* a = g_applied[i].addr;
        int   n = g_applied[i].len;
        DWORD old = 0;
        if (VirtualProtect(a, n, PAGE_EXECUTE_READWRITE, &old))
        {
            memcpy(a, g_applied[i].orig, n);
            VirtualProtect(a, n, old, &old);
            FlushInstructionCache(GetCurrentProcess(), a, n);
        }
    }
    g_appliedCount = 0;
}

static BYTE* FindSignature(HMODULE h, const BYTE* pat, int len)
{
    BYTE* base = (BYTE*)h;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;

    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;

    BYTE* found = nullptr;
    IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec)
    {
        if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;

        BYTE* p = base + sec->VirtualAddress;
        DWORD n = sec->Misc.VirtualSize ? sec->Misc.VirtualSize : sec->SizeOfRawData;

        for (DWORD k = 0; k + (DWORD)len <= n; ++k)
        {
            if (memcmp(p + k, pat, len) == 0)
            {
                if (found) return nullptr;   // ambiguous - refuse to guess
                found = p + k;
            }
        }
    }
    return found;
}

#define SENTINEL_LEN 20

static const BYTE kP64_SHELL32[SENTINEL_LEN] = {   // shell32.dll       +0x5767AC
    0x40,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0xAC,0x24,0x78,0xFD,0xFF };
static const BYTE kP64_EXFRAME[SENTINEL_LEN] = {   // ExplorerFrame.dll +0x19B0EC
    0x40,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0xAC,0x24,0x78,0xFD,0xFF };

struct Target
{
    const wchar_t* module;
    DWORD          rva;
    const BYTE*    bytes;   // 20 bytes at the target's entry point
};

static const Target kTargets64[] = {
    { L"shell32.dll",       0x005767AC, kP64_SHELL32 },
    { L"ExplorerFrame.dll", 0x0019B0EC, kP64_EXFRAME },
};

static BOOL IsStubbed(const BYTE* p)
{
    return p[0] == 0x33 && p[1] == 0xC0 && p[2] == 0xC3;   // xor eax,eax / ret
}

static BOOL DoStub(const Target& t)
{
    HMODULE h = GetModuleHandleW(t.module);
    if (!h) return FALSE;

    BYTE* p = (BYTE*)h + t.rva;

    if (IsStubbed(p)) return TRUE;

    if (memcmp(p, t.bytes, SENTINEL_LEN) != 0)
    {
        BYTE* found = FindSignature(h, t.bytes, SENTINEL_LEN);
        if (!found)
        {
            BYTE stubbed[SENTINEL_LEN];
            memcpy(stubbed, t.bytes, SENTINEL_LEN);
            stubbed[0] = 0x33; stubbed[1] = 0xC0; stubbed[2] = 0xC3;
            if (FindSignature(h, stubbed, SENTINEL_LEN)) return TRUE;

            Wh_Log(L"%s: prologue signature not found at +0x%X or anywhere else, "
                   L"skipping (is this a different Windows build?)", t.module, t.rva);
            return FALSE;
        }
        p = found;
        if (IsStubbed(p)) return TRUE;
    }

    BYTE stub[4];
    memcpy(stub, p, 4);
    stub[0] = 0x33; stub[1] = 0xC0; stub[2] = 0xC3;
    WriteBytes(p, stub, 4);
    return TRUE;
}

#if !defined(_WIN64)

static const BYTE kOD_X86[16] = {
    0x8B,0x44,0x24,0x38,
    0x0D,0x00,0x01,0x00,0x00,
    0xC7,0x44,0x24,0x60,0x30,0x00,0x00
};

static const BYTE kFMASK_X86[16] = {
    0xC7,0x44,0x24,0x64,
    0x88,0x01,0x00,0x00,
    0x89,0x44,0x24,0x68,
    0x57,0xFF,0x74,0x24
};

static const BYTE kFMASK_A2_X86[16] = {
    0xC7,0x44,0x24,0x6C,
    0xA8,0x01,0x00,0x00,
    0x8B,0xD7,
    0x89,0xBC,0x24,0x88,0x00,0x00
};

static const BYTE kFMASK_B_X86[16] = {
    0xC7,0x44,0x24,0x4C,
    0x80,0x01,0x00,0x00,
    0x89,0x44,0x24,0x50,
    0x85,0xD2,0x74,0x41
};

struct BitTarget
{
    const wchar_t* module;
    DWORD          sentinelRva;   // where the 16 verification bytes start
    const BYTE*    expect;        // those 16 bytes, none of them relocated
    DWORD          patchRva;      // the byte to change
    BYTE           from;
    BYTE           to;
};

static const BitTarget kBitTargets32[] = {
    { L"shell32.dll",       0x00506A3C, kOD_X86,      0x00506A42, 0x01, 0x00 },
    { L"ExplorerFrame.dll", 0x0017D944, kOD_X86,      0x0017D94A, 0x01, 0x00 },
};

static const BitTarget kMaskTargets32[] = {
    { L"shell32.dll",       0x00506A4D, kFMASK_X86,   0x00506A51, 0x88, 0x08 },
    { L"ExplorerFrame.dll", 0x0017D955, kFMASK_X86,   0x0017D959, 0x88, 0x08 },
    { L"shell32.dll",       0x00506A62, kFMASK_A2_X86,0x00506A66, 0xA8, 0x28 },
    { L"ExplorerFrame.dll", 0x0017D96A, kFMASK_A2_X86,0x0017D96E, 0xA8, 0x28 },
    { L"shell32.dll",       0x00507268, kFMASK_B_X86, 0x0050726C, 0x80, 0x00 },
    { L"ExplorerFrame.dll", 0x0017E336, kFMASK_B_X86, 0x0017E33A, 0x80, 0x00 },
};

static BOOL DoBitFlip(const BitTarget& t)
{
    HMODULE h = GetModuleHandleW(t.module);
    if (!h) return FALSE;

    BYTE* p = (BYTE*)h + t.patchRva;

    if (*p == t.to) return TRUE;

    if (memcmp((BYTE*)h + t.sentinelRva, t.expect, 16) != 0)
    {
        BYTE* found = FindSignature(h, t.expect, 16);
        if (!found)
        {
            int off = (int)(t.patchRva - t.sentinelRva);
            if (off >= 0 && off < 16)
            {
                BYTE patched[16];
                memcpy(patched, t.expect, 16);
                patched[off] = t.to;
                if (FindSignature(h, patched, 16)) return TRUE;
            }
            Wh_Log(L"%s: no signature for the site at +0x%X, skipping it",
                   t.module, t.patchRva);
            return FALSE;
        }
        p = found + (t.patchRva - t.sentinelRva);
        if (*p == t.to) return TRUE;
    }

    if (*p != t.from)
    {
        Wh_Log(L"%s: expected %02X at +0x%X but found %02X, skipping it",
               t.module, t.from, t.patchRva, *p);
        return FALSE;
    }

    BYTE v = t.to;
    WriteBytes(p, &v, 1);
    return TRUE;
}

#endif  // !_WIN64

static BOOL g_disabled = FALSE;   // this architecture's opt-out setting

static BOOL TryPatchAll()
{
    if (g_disabled) return TRUE;

    BOOL ok = TRUE;

#if defined(_WIN64)
    for (int i = 0; i < ARRAYSIZE(kTargets64); ++i)
        ok &= DoStub(kTargets64[i]);
#else
    for (int i = 0; i < ARRAYSIZE(kBitTargets32); ++i)
        ok &= DoBitFlip(kBitTargets32[i]);
    for (int i = 0; i < ARRAYSIZE(kMaskTargets32); ++i)
        ok &= DoBitFlip(kMaskTargets32[i]);
#endif

    return ok;
}

using LoadLibraryExW_t = HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t g_origLoadLibraryExW;
static BOOL             g_allDone = FALSE;

static HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE h = g_origLoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (h && !g_allDone) g_allDone = TryPatchAll();
    return h;
}

#define RETRY_INTERVAL_MS 100
#define RETRY_ATTEMPTS    100      // ~10 s, then give up quietly

static HANDLE        g_retryThread = NULL;
static volatile LONG g_retryStop   = 0;

static DWORD WINAPI RetryProc(LPVOID)
{
    for (int i = 0; i < RETRY_ATTEMPTS; ++i)
    {
        if (g_retryStop) return 0;
        if (TryPatchAll()) return 0;
        Sleep(RETRY_INTERVAL_MS);
    }
    Wh_Log(L"gave up waiting for shell32/ExplorerFrame after %d ms", RETRY_ATTEMPTS * RETRY_INTERVAL_MS);
    return 0;
}

BOOL Wh_ModInit()
{
#if defined(_WIN64)
    g_disabled = (BOOL)Wh_GetIntSetting(L"DisableX64");
#else
    g_disabled = (BOOL)Wh_GetIntSetting(L"DisableX86");
#endif
    if (g_disabled) return TRUE;

    g_allDone = TryPatchAll();

    HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
    if (k32)
    {
        void* pLoad = (void*)GetProcAddress(k32, "LoadLibraryExW");
        if (pLoad)
            Wh_SetFunctionHook(pLoad, (void*)Hook_LoadLibraryExW, (void**)&g_origLoadLibraryExW);
    }

    if (!g_allDone)
        g_retryThread = CreateThread(nullptr, 0, RetryProc, nullptr, 0, nullptr);

    return TRUE;
}

void Wh_ModUninit()
{
    g_retryStop = 1;
    if (g_retryThread)
    {
        WaitForSingleObject(g_retryThread, 2000);
        CloseHandle(g_retryThread);
        g_retryThread = NULL;
    }

    RestoreAll();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload)
{
    *bReload = TRUE;
    return TRUE;
}
