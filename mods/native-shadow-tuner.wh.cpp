// ==WindhawkMod==
// @id              native-shadow-tuner
// @name            Windows Shadows TUNER
// @description     Personalizza intensita, dimensione e colore delle ombre native delle finestre.
// @version         0.3.0
// @author          HaVeN80
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lbcrypt -masm=att
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Shadows TUNER
### HaVeN80

Customize the native Windows window shadows: make them lighter or more pronounced, adjust their size and blur, and choose a custom color.

## Controls

- **Enable shadow modification**: activates or deactivates the customization.
- **Intensity**: 100 maintains the original opacity, 50 cuts it in half, 150 increases it. At 0, the shadow becomes invisible, even if colored.
- **Size and Blur**: adjusts the drop and blur from 50% to 150%
- **Update shadows on new requests**: prevents the reuse of previous surfaces. Leave this active to apply your customizations.
- **Custom color**: enables the chosen color; 
- **Shadow color**: #RRGGBB code, for example #2878FF (blue), #00A860 (green), #000000 (black). An invalid code will default back to black.

To test the color, use intensity 150, size 100, custom color active and #2878FF. Open a new window and switch focus. The result also depends on your background and the set opacity.

## Aggiornamento

Sostituisci il sorgente della mod esistente e ricompila. L'identificativo
interno e le chiavi delle impostazioni precedenti restano gli stessi.
Il colore personalizzato e' inizialmente disattivato.

## How it works
It intercepts the creation of native shadows inside DWM, without adding overlapping windows. The tint is applied to the two components of the shadow separately from the border. The DLLs on the disk are not modified. Windows may retain already created surfaces. Opening new ones or changing focus allows you to verify the new settings; in some cases you may need to log out and log back in, even after deactivation.

## Compatibilita

Intended for the analyzed uDWM.dll version uDWM.dll 10.0.26100.9278.
windows 11 25h2.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: false
  $name: Abilita modifica ombre
  $description: Attiva la personalizzazione delle ombre native.
- opacityPercent: 100
  $name: Intensita ombra (% del valore nativo)
  $description: Da 0 a 300. 100 originale, 50 piu chiara, 150 piu scura. Con 0 anche il colore e invisibile.
- sizePercent: 100
  $name: Dimensione e sfumatura (% del valore nativo)
  $description: Da 50 a 150. 100 mantiene la dimensione originale.
- bypassCache: true
  $name: Aggiorna ombre alle nuove richieste
  $description: Ricrea le superfici per applicare le impostazioni. Puo aumentare il carico grafico.
- customColor: false
  $name: Colore personalizzato
  $description: Colora le due componenti dell'ombra. Disattivato mantiene il nero nativo.
- shadowColor: '#2878FF'
  $name: Colore ombra
  $description: "Codice esadecimale #RRGGBB. Esempi - #2878FF blu, #00A860 verde, #000000 nero."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <atomic>
#include <cstring>

// Color interception at two verified instruction boundaries, immediately
// AFTER native RGBA assembly and BEFORE ID2D1Properties::SetValue.
// RBP is the native CreateBorderSurface frame, RGB at +48/+4C/+50,
// alpha at +54. Preserve ALL registers, flags and stack position. No calls.
// Trampolines replay the displaced native instructions unchanged.
extern "C" {
__attribute__((used)) float wstRed = 0, wstGreen = 0, wstBlue = 0;
__attribute__((used)) void* wstColorResume1 = nullptr;
__attribute__((used)) void* wstColorResume2 = nullptr;

#ifdef _M_AMD64
__attribute__((naked, used)) void WstColorHook1() {
    __asm__("pushq %rax
"
            "movl wstRed(%rip), %eax
movl %eax, 0x48(%rbp)
"
            "movl wstGreen(%rip), %eax
movl %eax, 0x4c(%rbp)
"
            "movl wstBlue(%rip), %eax
movl %eax, 0x50(%rbp)
"
            "popq %rax
jmp *wstColorResume1(%rip)
");
}
__attribute__((naked, used)) void WstColorHook2() {
    __asm__("pushq %rax
"
            "movl wstRed(%rip), %eax
movl %eax, 0x48(%rbp)
"
            "movl wstGreen(%rip), %eax
movl %eax, 0x4c(%rbp)
"
            "movl wstBlue(%rip), %eax
movl %eax, 0x50(%rbp)
"
            "popq %rax
jmp *wstColorResume2(%rip)
");
}
#else
__attribute__((used)) void WstColorHook1() {}
__attribute__((used)) void WstColorHook2() {}
#endif
}

namespace {
using Parameters = void(__cdecl*)(int, int, float*, float*, float*, float*);
Parameters original = nullptr;
float opacityScale = 1.0f;
float sizeScale = 1.0f;
std::atomic<bool> stopping{false};
std::atomic<bool> reported{false};
std::atomic<unsigned> marginReports{0}, surfaceReports{0}, otherReports{0}, brushReports{0};
BYTE* moduleBase = nullptr;
bool bypassCache = true;
bool customColor = false;

bool ParseColor(PCWSTR text, float* red, float* green, float* blue) {
    if (!text) return false;
    if (*text == L'#') ++text;
    unsigned rgb = 0;
    for (int i=0; i<6; ++i) {
        wchar_t c = text[i];
        unsigned digit;
        if (c >= L'0' && c <= L'9') digit = c-L'0';
        else if (c >= L'a' && c <= L'f') digit = c-L'a'+10;
        else if (c >= L'A' && c <= L'F') digit = c-L'A'+10;
        else return false;
        rgb = (rgb << 4) | digit;
    }
    if (text[6]) return false;
    *red = ((rgb >> 16) & 255)/255.0f;
    *green = ((rgb >> 8) & 255)/255.0f;
    *blue = (rgb & 255)/255.0f;
    return true;
}

// Both static methods have the same x64 ABI. Color is a const reference
// (pointer). Output is an opaque pointer to a native std::shared_ptr;
// its ownership, allocation and destruction remain entirely inside uDWM.
using Brush = HRESULT(__cdecl*)(float, int, const void*, int, int, void*);
Brush getBrushOriginal = nullptr;
Brush createBrush = nullptr;

HRESULT __cdecl GetBrushHook(float radius, int dpi, const void* color,
                            int borderStyle, int shadowStyle, void* output) {
    if (stopping.load(std::memory_order_relaxed) || !bypassCache ||
        shadowStyle == 0 || (!customColor && opacityScale == 1.0f && sizeScale == 1.0f))
        return getBrushOriginal(radius, dpi, color, borderStyle, shadowStyle, output);
    // This is exactly the native cache-miss factory and argument list.
    // Returned resources stay owned by the caller; no foreign C++ ABI use.
    HRESULT hr = createBrush(radius, dpi, color, borderStyle, shadowStyle, output);
    if (brushReports.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"BRUSH_REBUILD style=%d dpi=%d HRESULT=0x%08X", shadowStyle, dpi,
               static_cast<unsigned>(hr));
    return hr;
}

void __cdecl ParametersHook(int style, int dpi, float* radius1,
                           float* radius2, float* alpha1, float* alpha2) {
    auto caller = reinterpret_cast<ULONG_PTR>(__builtin_return_address(0));
    auto rva = caller - reinterpret_cast<ULONG_PTR>(moduleBase);
    original(style, dpi, radius1, radius2, alpha1, alpha2);
    if (stopping.load(std::memory_order_relaxed)) return;
    // Settings are immutable for the lifetime of this hook. Windhawk reloads
    // the mod on changes; no allocation, I/O or object traversal here.
    float before1 = *alpha1, before2 = *alpha2;
    *radius1 *= sizeScale;
    *radius2 *= sizeScale;
    *alpha1 = std::clamp(*alpha1 * opacityScale, 0.0f, 1.0f);
    *alpha2 = std::clamp(*alpha2 * opacityScale, 0.0f, 1.0f);
    const wchar_t* path = rva == 0x37373 ? L"SURFACE" :
                          rva == 0x37154 ? L"MARGINS" : L"OTHER";
    auto& count = rva == 0x37373 ? surfaceReports :
                  rva == 0x37154 ? marginReports : otherReports;
    if (count.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"%s callerRVA=0x%llX style=%d dpi=%d alpha=(%.3f,%.3f)->(%.3f,%.3f) radii=(%.2f,%.2f)",
               path, static_cast<unsigned long long>(rva), style, dpi,
               static_cast<double>(before1), static_cast<double>(before2),
               static_cast<double>(*alpha1), static_cast<double>(*alpha2),
               static_cast<double>(*radius1), static_cast<double>(*radius2));
}

bool VerifyBody(BYTE* data, ULONG length, const char* expected) {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM,
                                    nullptr, 0) < 0) return false;
    BYTE digest[32]{};
    NTSTATUS status = BCryptHash(algorithm, nullptr, 0, data, length, digest, 32);
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (status < 0) return false;
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i=0; i<32; ++i)
        if (expected[2*i] != hex[digest[i] >> 4] ||
            expected[2*i+1] != hex[digest[i] & 15]) return false;
    return true;
}

bool VerifyTarget(HMODULE module) {
    auto base = reinterpret_cast<BYTE*>(module);
    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0 ||
        dos->e_lfanew > 0x800) return false;
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt->FileHeader.TimeDateStamp != 0x240e386a ||
        nt->OptionalHeader.SizeOfImage != 0x142000) return false;
    auto directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    if (directory.VirtualAddress != 0x10b860 || directory.Size != 0x70)
        return false;
    auto debug = reinterpret_cast<IMAGE_DEBUG_DIRECTORY*>(base + directory.VirtualAddress);
    const BYTE expectedIdentity[] = {
        0x52,0x53,0x44,0x53,0x7c,0x56,0x7a,0x7b,
        0xa0,0xba,0x17,0x41,0x55,0xde,0x63,0x8d,
        0x3f,0xd1,0x64,0xf6,0x01,0x00,0x00,0x00
    };
    bool identity = false;
    for (unsigned i=0; i<directory.Size/sizeof(*debug); ++i) {
        if (debug[i].Type == IMAGE_DEBUG_TYPE_CODEVIEW &&
            debug[i].SizeOfData >= sizeof(expectedIdentity) &&
            debug[i].AddressOfRawData <= 0x142000-sizeof(expectedIdentity) &&
            !std::memcmp(base+debug[i].AddressOfRawData, expectedIdentity,
                         sizeof(expectedIdentity))) identity = true;
    }
    if (!identity) return false;
    // Check full bodies of both new targets before using their addresses.
    if (!VerifyBody(base+0x3ab80, 0x22e,
        "c1e6b139c652ba5064389137e53646ed4b40943444a5a6cf88ed829dd78b1cfb") ||
        !VerifyBody(base+0x38318, 0xa04,
        "bd3fc277e7e3f633d9c200c907517dfc3cd826057770db1b13f781f46bcaae08")) return false;
    // Entire leaf function has no base relocations. Hash loaded instructions
    // before hooking, also rejecting an existing detour by another mod.
    const BYTE expectedHash[32] = {
        0x67,0x80,0xf6,0x24,0x7e,0xa7,0xec,0x9d,
        0xf6,0xc3,0x7f,0xcd,0xe6,0xc4,0x31,0x58,
        0x80,0xb6,0x36,0xd4,0x57,0xf0,0x63,0xf1,
        0x5d,0xde,0xc6,0x7f,0x37,0xd1,0x82,0x1d
    };
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM,
                                    nullptr, 0) < 0) return false;
    BYTE digest[32]{};
    NTSTATUS status = BCryptHash(algorithm, nullptr, 0, base+0x371a8,
                                 0xe4, digest, sizeof(digest));
    BCryptCloseAlgorithmProvider(algorithm, 0);
    return status >= 0 && !std::memcmp(digest, expectedHash, sizeof(digest));
}
}

BOOL Wh_ModInit() {
    stopping.store(false);
    reported.store(false);
    marginReports.store(0); surfaceReports.store(0);
    otherReports.store(0); brushReports.store(0);
    HMODULE module = GetModuleHandleW(L"uDWM.dll");
    if (!module || !VerifyTarget(module)) {
        Wh_Log(L"REFUSED: uDWM not loaded, unsupported DLL, or code already modified.");
        return FALSE;
    }
    Wh_Log(L"Exact uDWM identity and function bytes verified.");
    moduleBase = reinterpret_cast<BYTE*>(module);
    if (!Wh_GetIntSetting(L"enabled")) {
        Wh_Log(L"Diagnostic only: shadow changes disabled in settings.");
        return TRUE;
    }
    opacityScale = std::clamp(Wh_GetIntSetting(L"opacityPercent"), 0, 300)/100.0f;
    sizeScale = std::clamp(Wh_GetIntSetting(L"sizePercent"), 50, 150)/100.0f;
    bypassCache = Wh_GetIntSetting(L"bypassCache") != 0;
    customColor = Wh_GetIntSetting(L"customColor") != 0;
    if (customColor) {
        PCWSTR color = Wh_GetStringSetting(L"shadowColor");
        bool valid = ParseColor(color, &wstRed, &wstGreen, &wstBlue);
        if (color) Wh_FreeStringSetting(color);
        if (!valid) {
            customColor = false;
            Wh_Log(L"COLOR disabled: use a six-digit #RRGGBB value.");
        }
        // Only color-on checks/installs the new targets. Color-off retains
        // the 0.2 hook set and execution paths, including cache behavior.
        if (customColor && !VerifyBody(moduleBase+0x37294, 0xfed,
            "e66c5c849c8c1321446e42379e8b68fcdb856be61a6d1f3bfc4311e8d1522ece")) {
            customColor = false;
            Wh_Log(L"COLOR disabled: surface code differs. Base controls retained.");
        }
    }
    Wh_Log(L"CONFIG opacity=%.2f size=%.2f bypassCache=%d",
           static_cast<double>(opacityScale), static_cast<double>(sizeScale), bypassCache);
    createBrush = reinterpret_cast<Brush>(moduleBase+0x38318);
    auto target = reinterpret_cast<BYTE*>(module)+0x371a8;
    if (!Wh_SetFunctionHook(target, reinterpret_cast<void*>(ParametersHook),
                           reinterpret_cast<void**>(&original))) {
        Wh_Log(L"Hook registration failed.");
        return FALSE;
    }
    if (bypassCache && !Wh_SetFunctionHook(moduleBase+0x3ab80,
            reinterpret_cast<void*>(GetBrushHook),
            reinterpret_cast<void**>(&getBrushOriginal))) {
        Wh_Log(L"Cache hook registration failed; initialization aborted.");
        return FALSE;
    }
    if (customColor) {
#ifdef _M_AMD64
        if (!Wh_SetFunctionHook(moduleBase+0x37918,
                reinterpret_cast<void*>(WstColorHook1), &wstColorResume1) ||
            !Wh_SetFunctionHook(moduleBase+0x37a40,
                reinterpret_cast<void*>(WstColorHook2), &wstColorResume2)) {
            // Abort the transaction; never run with only one layer colored.
            Wh_Log(L"COLOR hook registration failed. Turn customColor off to use the base.");
            return FALSE;
        }
        Wh_Log(L"COLOR enabled RGB=(%.3f,%.3f,%.3f). Both native shadow layers hooked.",
               static_cast<double>(wstRed), static_cast<double>(wstGreen),
               static_cast<double>(wstBlue));
        if (!bypassCache) Wh_Log(L"COLOR: enable bypassCache if existing brushes keep the old color.");
#else
        Wh_Log(L"COLOR hook skipped: non-x64 architecture detected.");
#endif
    }
    Wh_Log(L"Native shadow hooks registered. Open a NEW window and change focus for testing.");
    return TRUE;
}

void Wh_ModBeforeUninit() { stopping.store(true); }
void Wh_ModUninit() {}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}